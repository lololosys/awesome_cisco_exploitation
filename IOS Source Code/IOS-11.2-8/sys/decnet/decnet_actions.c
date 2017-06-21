/* $Id: decnet_actions.c,v 3.3.44.2 1996/06/12 19:08:49 pst Exp $
 * $Source: /release/112/cvs/Xsys/decnet/decnet_actions.c,v $
 *------------------------------------------------------------------
 * Decnet specific parsing functions.
 *
 * April 1993,  Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: decnet_actions.c,v $
 * Revision 3.3.44.2  1996/06/12  19:08:49  pst
 * CSCdi60173:  Eliminate cisco definition of ntohl in favor of
 * swap_xx_bits (partial commit part 1)
 * Branch: California_branch
 *
 * Revision 3.3.44.1  1996/05/21  09:44:53  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3  1995/12/17  18:25:40  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  08:58:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:15:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:22:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "../parser/parser_actions.h"
#include "../decnet/decnet_actions.h"

char *decnet_addr_short_help (parseinfo *csb, uint flag, idbtype *idb)
{
    return("N.H");
}

boolean match_dn_addr (parseinfo *csb, char *buf, int *pi, void *addr_pointer,
                       int in_help, uint flag, idbtype *idb)
{
    addrtype *addrp = (addrtype *)addr_pointer;
    ushort area;
    ushort node;

    if (match_decimal(buf, 0, 63, pi, &area) &&
        match_char(buf, pi, '.') &&
        match_decimal(buf, 0, 1023, pi, &node)) {

        addrp->type = ADDR_DECNET;
        addrp->length = ADDRLEN_DECNET;
        addrp->decnet_addr = (area & 0x3f) << 10 | (node & 0x3ff);
        if (flag & ADDR_PROTO_ADDR) {
	    addrp->decnet_addr = swap_16bit_word(addrp->decnet_addr);
	}
        return(TRUE);
    }
    return(FALSE);
}
