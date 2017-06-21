/* $Id: clns_actions.c,v 3.4.18.5 1996/08/28 12:38:22 thille Exp $
 * $Source: /release/112/cvs/Xsys/clns/clns_actions.c,v $
 *------------------------------------------------------------------
 * CLNS specific parsing functions.
 *
 * April 1993,  Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: clns_actions.c,v $
 * Revision 3.4.18.5  1996/08/28  12:38:22  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.18.4  1996/08/08  14:50:09  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.  Get rid of gratuitous type coercion.
 *
 * Revision 3.4.18.3  1996/06/28  23:05:15  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4.18.2  1996/05/21  09:43:52  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4.18.1  1996/03/18  19:02:28  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.2  1996/03/07  08:39:50  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.14.1  1996/02/20  00:20:42  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/07  21:14:52  widmer
 * CSCdi48343:  Remove unused #ifdef STANDALONE from parser code
 *
 * Revision 3.3  1995/12/17  18:25:11  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  08:51:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:07:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/18  19:39:33  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:16:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../parser/msg_parser.c"	/* Not a typo, see logger.h */
#include "../ui/common_strings.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../clns/clns_actions.h"

#include "../clns/clns_route.h"
#include "../clns/clns_externs.h"
#include "name.h"

/*
 * Forward declarations
 */
static boolean match_nsap_name(char *, int *, NSAP_T, int *, int);

char *clns_addr_short_help (parseinfo *csb, uint flag, idbtype *idb)
{
    return("XX.XXXX. ... .XXX.XX");
}

/*
 * Match an NSAP (clns) address of the form xx.xxxx.xxxx...xxxx.xx
 *
 * Use the new routines.
 */

boolean match_clns_addr (parseinfo *csb, char *buf, int *pi,
			 void *addr_pointer, int in_help, uint flag,
			 idbtype *idb)
{
    addrtype *addrp = (addrtype *)addr_pointer;
    int offset;
    NSAP_T nsap;
    int nbytes;
    boolean parse_ok;

    nbytes = 0;
    memset(nsap, 0, sizeof(NSAP_T));

    parse_ok = get_nsap_addr(&buf[*pi], NSAP_T_to_nsap_addr(nsap), &offset);
    *pi += offset;

    if (parse_ok) {
	/* copy nsap + length */
	bcopy(nsap, addrp->clns_addr, nsap[0] + 1);
	addrp->type = ADDR_CLNS;
	addrp->length = nsap[0];
    } else {
	/* Check for a name match */
	if (match_nsap_name(buf, pi, nsap, &nbytes, csb->in_help)) {
	    bcopy(nsap, addrp->clns_addr, nbytes);
	    addrp->type = ADDR_CLNS;
	    addrp->length = nbytes;
	    parse_ok = TRUE;
	}
    }
    return(parse_ok);
}


static boolean match_nsap_name (char *buf, int *pi, NSAP_T nsap,
				int *nbytes, int in_help)
{
    char *name;
    int i;
    nametype *cp;
    int dummy;

    name = malloc(PARSEBUF);
    if (name == NULL) {
	return(FALSE);
    }

    if (!get_name(&buf[*pi], name, &i, PARSEBUF-1)) {
	free(name);
	return(FALSE);
    }

    if (in_help) {
	*pi += i;
	free(name);
	return(TRUE);
    }

    /* We have the name, look it up. */
    cp = name_lookup(name, &dummy, NAM_CLNS);
    if (cp) {
	nsap_copy(*(NSAP_T*)cp->v.clns.adr[0].addr.clns_address, nsap);
	*nbytes = nsap[0] + 1;
	*pi += i;
	free(name);
	return(TRUE);
    }
    free(name);
    return(FALSE);
}

void parser_clns_router (parseinfo *csb, const char *name,
			 char *cp, transition *accept)
{
    clns_pdbtype *clns_pdb;
    char *buffer;

    for (clns_pdb = clns_protoQ.qhead; clns_pdb; clns_pdb = clns_pdb->next) {

	if (!strcmp(name, clns_pdb->name)) {
	    if ((clns_pdb->typeflag & ISO_IGRPMASK) &&
		clns_pdb->level == CLNS_LEVEL2) {
		continue;
	    }
	    if (clns_pdb->typeflag == ISO_STATIC) {
		continue;
	    }
	    buffer = malloc(strlen(clns_pdb->name) + 
			    strlen(clns_pdb->pname) + 4);
	    if (!buffer) {
		return;
	    }
	    sprintf(buffer, " %s %s", clns_pdb->name, clns_pdb->pname);
	    strcpy(cp, buffer);
	    free(buffer);
	    SETOBJ(int,1) = PDB_OSI;
	    SETOBJ(pdb,1) = clns_pdb;
	    push_node(csb, accept);
	}
    }
}
