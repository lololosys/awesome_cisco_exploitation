/* $Id: at_actions.c,v 3.3.44.2 1996/07/10 22:17:39 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_actions.c,v $
 *------------------------------------------------------------------
 * AppleTalk specific parsing functions.
 *
 * April 1993,  Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: at_actions.c,v $
 * Revision 3.3.44.2  1996/07/10  22:17:39  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.3.44.1  1996/05/21  09:42:23  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3  1995/12/17  18:24:17  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  08:41:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:56:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:07:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "config.h"
#include "../parser/parser_actions.h"
#include "at_actions.h"

char *apple_addr_short_help (parseinfo *csb, uint flag, idbtype *idb)
{
    return("N.H");
}

/*
 * APPLETALK ADDRESS: AppleTalk addresses are 24 bits, a 16 bit net number
 * and an 8 bit node number. These are expected to be decimals, separated
 * by a dot: net.node
 */

boolean match_apple_addr (parseinfo *csb, char *buf, int *pi,
			  void *addr_pointer, int in_help, uint flag,
			  idbtype *idb)
{
    addrtype *addrp = (addrtype *)addr_pointer;
    ushort net;
    ushort host;

    if (match_decimal(buf, 0, 0xffff, pi, &net) &&
        match_char(buf, pi, '.') &&
        match_decimal(buf, 0, 0xff, pi, &host)) {

        addrp->type = ADDR_APPLE;
        addrp->length = ADDRLEN_APPLE;
        addrp->apple_addr.net = net;
        addrp->apple_addr.host = host;
	return(TRUE);
    }
    return(FALSE);
}

