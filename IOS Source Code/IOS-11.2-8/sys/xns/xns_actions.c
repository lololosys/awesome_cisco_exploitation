/* $Id: xns_actions.c,v 3.3.46.1 1996/05/21 10:11:51 thille Exp $
 * $Source: /release/112/cvs/Xsys/xns/xns_actions.c,v $
 *------------------------------------------------------------------
 * XNS specific parsing functions.
 *
 * April 1993,  Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: xns_actions.c,v $
 * Revision 3.3.46.1  1996/05/21  10:11:51  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3  1995/12/17  18:42:41  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  19:25:19  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:00:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:30:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "../parser/parser_actions.h"

/*
 * Forward declarations
 */
static boolean match_xns_net(char *, int *, ulong *);
static boolean match_xns_host (char *, int *, ushort[3]);

char *xns_addr_short_help (parseinfo *csb, uint flag, idbtype *idb)
{
    if (flag & ADDR_HOST_ADDR_ONLY) {
	return("H.H.H");
    } else {
	return("N.H.H.H");
    }
}

/* Match both net and host parts of an XNS address. */
boolean match_xns_addr (parseinfo *csb, char *buf, int *pi, void *addr_pointer,
                        int in_help, uint flag, idbtype *idb)
{
    addrtype *addrp = (addrtype *)addr_pointer;
    ulong net;
    ushort values[3];
    boolean matched = FALSE;

    if (flag & ADDR_HOST_ADDR_ONLY) {
        matched = match_xns_host(buf, pi, values);
        net = 0;
    } else {
        matched = (match_xns_net(buf, pi, &net) &&
                   match_char(buf, pi, '.') &&
                   match_xns_host(buf, pi, values));
    }

    if (matched) {

        addrp->type = ADDR_XNS;
        addrp->length = ADDRLEN_XNS;

        addrp->xns_addr.net = net;
        ((ushort *) addrp->xns_addr.host)[0] = values[0];
        ((ushort *) addrp->xns_addr.host)[1] = values[1];
        ((ushort *) addrp->xns_addr.host)[2] = values[2];

        return(TRUE);
    }
    return(FALSE);
}

/*
 * Try to match an xns_net in <buf>. Put the value found into
 * <*pvalue> and the number of characters in the match into *pi.
 */
static boolean match_xns_net (char *buf, int *pi, ulong *pvalue)
{
    if (match_minusone(buf, pi)) {
        /* "-1.H.H.H" case */
        *pvalue = (ulong) (-1L);
        return(TRUE);
    } else {

        /*
         * The network number is a 32-bit decimal, i.e. up to 8 hex digits.
         */
        return(match_ulong_decimal(buf, 0L, ULONG_MAX, pi, pvalue));
    }
}

static boolean match_xns_host (char *buf, int *pi, ushort values[3])
{
    return(match_hexadecimal(buf, pi, &values[0])
           && match_char(buf, pi, '.')
           && match_hexadecimal(buf, pi, &values[1])
           && match_char(buf, pi, '.')
           && match_hexadecimal(buf, pi, &values[2]));
}
