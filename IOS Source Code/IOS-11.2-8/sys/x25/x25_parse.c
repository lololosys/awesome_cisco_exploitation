/* $Id: x25_parse.c,v 3.5.18.2 1996/09/11 01:46:28 che Exp $
 * $Source: /release/112/cvs/Xsys/x25/x25_parse.c,v $
 *------------------------------------------------------------------
 * x25_parse.c -- X25 configuration
 *
 * January 1988, Greg Satz
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x25_parse.c,v $
 * Revision 3.5.18.2  1996/09/11  01:46:28  che
 * CSCdi66795:  Double authentication with NASI and TACACS+
 * Branch: California_branch
 * Bypass exec authentication if we have already done NASI authentication.
 *
 * Revision 3.5.18.1  1996/03/18  22:50:24  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/13  02:13:37  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.2  1996/03/07  11:19:44  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:56:14  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/07  16:17:56  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.4  1996/02/01  06:13:09  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1996/01/29  07:31:35  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:07:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:53:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/02  22:01:59  carvalho
 * CSCdi39783:  CMNS call to ethernet destination fails if LLC2 not open.
 *
 * Revision 2.2  1995/08/25  18:46:30  dkatz
 * CSCdi38487:  ISIS LSP MTU needs to be settable
 * Missed a file in the commit.
 *
 * Revision 2.1  1995/06/07  23:22:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "sys_registry.h"
#include "../h/auth_registry.regh"
#include "ttysrv.h"
#include "config.h"
#include "parser.h"
#include "x25.h"
#include "x25_address.h"
#include "x25_route.h"
#include "x25_proto.h"
#include "x25_bfe.h"
#include "pad.h"
#include "aaa.h"
#include "parser_defs_x25.h"
#include "../clns/parser_defs_clns.h"
#include "../ui/parse_util.h"
#include "../clns/clns_adj.h"

extern queuetype x25rpQ;

/*
 * x25_rpoa_lookup
 * Find a given RPOA entry given its name
 */
rpoatype *x25_rpoa_lookup (char *name)
{
    rpoatype *rp;

    for (rp = (rpoatype *)x25rpQ.qhead; rp; rp = rp->next)
	if (strcmp(rp->name, name) == 0)
	    return(rp);
    return(NULL);
}

/*
 * x25_pvc_setup
 * Create (or find) the lci for a PVC
 */
lcitype *x25_pvc_setup (
    idbtype *idb,
    int pvc)
{
    lcitype *lci;

    lci = x25_lci_lookup(idb->hwptr, pvc, NULL);

    if (lci == NULL)
	lci = x25_lci_enter(idb, pvc, NULL);

    if (lci == NULL)
	printf("\nCouldn't create PVC for LCI %d", pvc);
    else
	lci->lci_linktype = LINK_X25;

    return(lci);
}

/*
 * show_x25
 * Display X.25 information
 */
void show_x25 (parseinfo *csb)
{
    switch (csb->which) {
      case SHOW_X25_BFE:
	show_x25bfe();
        break;
      case SHOW_X25_MAP:
	show_x25_maps();
        break;
      case SHOW_X25_PAD:
	show_padstatus(GETOBJ(int,1));
        break;
      case SHOW_X25_ROUTE:
	show_x25route();
        break;
      case SHOW_X25_VC:
	automore_enable(NULL);
	show_x25status(GETOBJ(int,1));
	automore_disable();
        break;
      default:
	bad_parser_subcommand(csb, csb->which);
        break;
    }
}

/*
 * Parse a clns address into the x25 address maps
 */
boolean x25_clns_parse_addr_map (
    parseinfo *csb,
    hwaddrtype *snpa,
    addrtype *addr)
{
    x25map *map;

    if (is_x25(csb->interface->hwptr)) {
	memset(addr, 0, sizeof(addrtype));
	addr->type = ADDR_X121;
	addr->length = snpa->length;
	bcopy(snpa->addr, addr->x121_addr, snpa->length);

	if ((map = x25_map_find(csb->interface->hwptr, addr, 
				X25_MATCH_EXACT, X25_MAPS_CLNS)) &&
	    (map->map_idb == csb->interface))
	    return(TRUE);
    }

    return(FALSE);
}

/*
 * x25_parse_nuid
 * Parse and validate a Network User ID
 */
/*made global to conserve stack space*/
char user[X25_MAX_NUID_SIZE], pw[X25_MAX_NUID_SIZE];

char x25_parse_nuid (lcitype *lci)
{
    char *reason;
    uchar *c;
    int i, j;

    if (lci->lci_nuid == NULL || lci->lci_nuidlen == 0)
	return(TRUE);
    if((i = lci->lci_nuidlen) >= X25_MAX_NUID_SIZE)
	return(X25_ERROR);
    c = lci->lci_nuid;
    if (*c++ != 1)
	return(X25_ERROR);
    i--;
    j = *c++;
    i--;
    if (j > i)
	return(X25_ERROR);
    bcopy(c, user, j);
    user[j] = '\0';
    c += j;
    i -= j;
    j = *c++;
    i--;
    if (j > i)
	return(X25_ERROR);
    bcopy(c, pw, j);
    pw[j] = '\0';
    if (old_access_control) {
	return(reg_invoke_login_login(stdio, user, pw, &reason));
    } else {
	return(aaa_simple_login_check(stdio, user, pw, NULL, FALSE, AAA_SVC_X25));
    }
}
