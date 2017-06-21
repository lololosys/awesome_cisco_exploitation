/* $Id: vines_actions.c,v 3.3.46.2 1996/08/28 13:19:26 thille Exp $
 * $Source: /release/112/cvs/Xsys/vines/vines_actions.c,v $
 *------------------------------------------------------------------
 * Vines specific parsing functions.
 *
 * April 1993,  Robert Widmer
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: vines_actions.c,v $
 * Revision 3.3.46.2  1996/08/28  13:19:26  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.46.1  1996/05/21  10:08:28  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3  1995/12/17  18:40:44  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:57:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:44:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/20  07:19:58  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  23:14:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "config.h"
#include "logger.h"
#include "../parser/msg_parser.c"	/* Not a typo, see logger.h */
#include "../parser/parser_actions.h"
#include "../vines/vines_actions.h"

#include "packet.h"
#include "vines_private.h"

/*
 * Forward declarations
 */
static boolean match_vines_name(char *, int *, ulong *, ushort *);

char *vines_addr_short_help (parseinfo *csb, uint flag, idbtype *idb)
{
    if (flag & ADDR_HOST_NET_ONLY) {
	return("N");
    } else {
	return("N:H");
    }
}

boolean match_vines_addr (parseinfo *csb, char *buf, int *pi,
			  void *addr_pointer, int in_help, uint flag,
			  idbtype *idb)
{
    addrtype *addrp = (addrtype *)addr_pointer;
    ulong net;
    ushort host;

    /* First, check for a vines name. */
    if (flag & ADDR_NAME_LOOKUP) {
	if (match_vines_name(buf, pi, &net, &host)) {
	    addrp->type = ADDR_VINES;
	    addrp->length = ADDRLEN_VINES;
	    addrp->vines_addr.net = net;
	    addrp->vines_addr.host = host;
	    return((flag & ADDR_HOST_NET_ONLY) ? (host == 1) : TRUE);
	}
    }

    net = 0;
    host = 0;

    if (match_minusone(buf, pi)) {
        /* Special net number '-1'. */
        net = VINES_ALLNETS;
    } else if (vines_decimal_addresses) {
        if (!match_ulong_decimal(buf, 0L, -1L, pi, &net)) {
            /*
             * If here, we cannot get a real net number.
             */
            return(FALSE);
        }
    } else if (!match_ulong_hexadecimal(buf, 0L, -1L, pi, &net)) {
        /*
         * If here, we cannot get a real net number (in hex).
         */
        return(FALSE);
    }

    /* Survived to here, got net part successfully. Is this all? */
    if (flag & ADDR_HOST_NET_ONLY) {
        addrp->type = ADDR_VINES;
        addrp->length = ADDRLEN_VINES;
        addrp->vines_addr.net = net;
        addrp->vines_addr.host = 1;
        return(TRUE);
    }

    /* Need the host portion too.  Go get it. */
    if (!match_char(buf, pi, ':')) {
        return(FALSE);
    }

    if (match_minusone(buf, pi)) {
        host = VINES_ALLHOSTS;
        addrp->type = ADDR_VINES;
        addrp->length = ADDRLEN_VINES;
        addrp->vines_addr.net = net;
        addrp->vines_addr.host = host;
        return(TRUE);
    }
    if (vines_decimal_addresses) {
        if (match_decimal(buf, 0, 65535, pi, &host)) {
            addrp->type = ADDR_VINES;
            addrp->length = ADDRLEN_VINES;
            addrp->vines_addr.net = net;
            addrp->vines_addr.host = host;
            return(TRUE);
        }
    } else {
        if (match_hexadecimal(buf, pi, &host)) {
            addrp->type = ADDR_VINES;
            addrp->length = ADDRLEN_VINES;
            addrp->vines_addr.net = net;
            addrp->vines_addr.host = host;
            return(TRUE);
        }
    }
    return(FALSE);
}


/*
 * Get a vines address by lookup in the name cache.
 */
static boolean match_vines_name (char *buf, int *pi,
                                 ulong *pnet, ushort * phost)
{
    char *name;
    int i;

    name = malloc(PARSEBUF);
    if (name == NULL) {
        return(FALSE);
    }

    if (!get_name(&buf[*pi], name, &i, PARSEBUF-1)) {
        free(name);
        return(FALSE);
    }

    /* We have the name, look it up. */
    if (vines_lookup_address(name, pnet, phost)) {
        *pi += i;
        free(name);
        return(TRUE);
    }
    free(name);
    return(FALSE);
}

