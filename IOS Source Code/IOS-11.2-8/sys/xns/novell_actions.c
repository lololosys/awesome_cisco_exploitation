/* $Id: novell_actions.c,v 3.3.46.1 1996/05/21 10:11:38 thille Exp $
 * $Source: /release/112/cvs/Xsys/xns/novell_actions.c,v $
 *------------------------------------------------------------------
 * Novell specific parsing functions.
 *
 * April 1993,  Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: novell_actions.c,v $
 * Revision 3.3.46.1  1996/05/21  10:11:38  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3  1995/12/17  18:42:37  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  19:22:59  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:58:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:39:09  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  23:28:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "packet.h"
#include "../parser/parser_actions.h"
#include "../xns/novell_actions.h"
#include "../xns/xns.h"
#include "../xns/novell.h"	/* novpdbQ */

/*
 * Forward declarations
 */
static boolean match_nov_net(char *, int *, ulong *);
static boolean match_nov_host(char *, int *, ushort [3]);

char *novell_addr_short_help (parseinfo *csb, uint flag, idbtype *idb)
{
    if (flag & ADDR_HOST_ADDR_ONLY) {
	return("H.H.H");
    } else {
	return("N.H.H.H");
    }
}

/*
 * Match both net and host parts of an NOVELL address. It's like XNS but
 * the net part is hex not decimal.
 */
boolean match_nov_addr (parseinfo *csb, char *buf, int *pi, void *addr_pointer,
                        int in_help, uint flag, idbtype *idb)
{
    addrtype *addrp = (addrtype *)addr_pointer;
    ulong net;
    ushort values[3];

    if ((flag & ADDR_HOST_ADDR_ONLY) == 0) {
	if (!match_nov_net(buf, pi, &net) || !match_char(buf, pi, '.')) {
	    return(FALSE);
	}
    } else {
	net = 0;
    }
    if (match_nov_host(buf, pi, values)) {
        addrp->type = ADDR_NOVELL;
        addrp->length = ADDRLEN_NOVELL;
        addrp->nov_addr.net = net;
        ((ushort *) addrp->nov_addr.host)[0] = values[0];
        ((ushort *) addrp->nov_addr.host)[1] = values[1];
        ((ushort *) addrp->nov_addr.host)[2] = values[2];
        return(TRUE);
    }
    return(FALSE);
}

/*
 * Novell net is a long hex or -1
 */
static boolean match_nov_net (char *buf, int *pi, ulong *pnet)
{
    if (match_minusone(buf, pi)) {
        /* "-1.*" case */
        *pnet = (ulong) (-1L);
        return(TRUE);
    } else {
        return(match_ulong_hexadecimal(buf, 0L, ULONG_MAX, pi, pnet));
    }
}

static boolean match_nov_host (char *buf, int *pi, ushort values[3])
{
    return(match_hexadecimal(buf, pi, &values[0])
           && match_char(buf, pi, '.')
           && match_hexadecimal(buf, pi, &values[1])
           && match_char(buf, pi, '.')
           && match_hexadecimal(buf, pi, &values[2]));
}

void parser_novell_router (parseinfo *csb, const char *name,
			   char *cp, transition *accept)
{
    novpdbtype *nov_pdb;
    char buffer[26];

    for (nov_pdb = (novpdbtype *) novpdbQ.qhead; nov_pdb;
	 nov_pdb = nov_pdb->next) {
	if (strcmp(name, nov_pdb->name) == 0) {
	    switch (nov_pdb->proctype) {
	      case PROC_IGRP2:
	      case NET_IGRP:
		sprintf(buffer, " %s %d", nov_pdb->name, nov_pdb->asystem);
		strcpy(cp, buffer);
		break;
	      case PROC_NLSP:
	      case NET_NLSP:
		if (nov_pdb->pname) {
		    sprintf(buffer, " %s %s", nov_pdb->name, nov_pdb->pname);
		    strcpy(cp, buffer);
		}
		else {
		    *cp = ' ';
		    strcpy((cp + 1), nov_pdb->name);
		}
		break;
	      case PROC_RIP:
	      case NET_RIP:
		*cp = ' ';
		strcpy((cp + 1), nov_pdb->name);
		break;
	      default:
		break;
	    }
	    SETOBJ(int,1) = PDB_NOVELL;
	    SETOBJ(pdb,1) = nov_pdb;
	    push_node(csb, accept);
	}
    }
}
