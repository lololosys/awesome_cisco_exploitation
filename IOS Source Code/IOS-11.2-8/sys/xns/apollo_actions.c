/* $Id: apollo_actions.c,v 3.3.46.2 1996/05/21 10:11:15 thille Exp $
 * $Source: /release/112/cvs/Xsys/xns/apollo_actions.c,v $
 *------------------------------------------------------------------
 * Apollo specific parsing functions.
 *
 * April 1993,  Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: apollo_actions.c,v $
 * Revision 3.3.46.2  1996/05/21  10:11:15  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3.46.1  1996/03/18  22:52:24  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.2  1996/03/07  11:21:21  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.14.1  1996/02/20  21:58:19  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/12/17  18:42:20  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  19:20:30  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:56:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:26:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "config.h"
#include "../parser/parser_actions.h"
#include "../xns/apollo_actions.h"

/*
 * Forward declarations
 */
static boolean match_apollo_net(char *, int *, ulong *);


char *apollo_addr_short_help (parseinfo *csb, uint flag, idbtype *idb)
{
    return("N.H");
}

boolean match_apollo_addr (parseinfo *csb, char *buf, int *pi,
			   void *addr_pointer, int in_help, uint flag,
			   idbtype *idb)
{
    addrtype *addrp = (addrtype *)addr_pointer;
    ulong net;
    ulong host;

    /*
     * Match_apollo_net allows for -1 or an unsigned long int. This is
     * what the macro version of this did.
     */

    /* at most 5 hex digits */
    if (match_apollo_net(buf, pi, &net) &&
        match_char(buf, pi, '.') &&
        match_ulong_hexadecimal(buf, 0L, 0xfffff, pi, &host)) {

        addrp->type = ADDR_APOLLO;
        addrp->length = ADDRLEN_APOLLO;

        addrp->apollo_addr.net = net;
	bcopy((uchar *)&host, addrp->apollo_addr.host + 2, 4);
	addrp->apollo_addr.host[0] = 0;
	addrp->apollo_addr.host[1] = 0;

        return(TRUE);
    }
    return(FALSE);
}

/*
 * Try to match an apollo_net in <buf>. Put the value found into
 * <*pvalue> and the number of characters in the match into *pi.
 */
static boolean match_apollo_net (char *buf, int *pi, ulong *pvalue)
{
    if (match_minusone(buf, pi)) {
        *pvalue = (ulong) (-1L);
        return(TRUE);
    } else {

        /*
         * The network number is a 32-bit hex number.
         */
        return(match_ulong_hexadecimal(buf, 0L, ULONG_MAX, pi, pvalue));
    }
}
