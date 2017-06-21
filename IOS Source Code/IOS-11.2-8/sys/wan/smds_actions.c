/* $Id: smds_actions.c,v 3.3.46.3 1996/05/21 10:09:54 thille Exp $
 * $Source: /release/112/cvs/Xsys/wan/smds_actions.c,v $
 *------------------------------------------------------------------
 * SMDS specific parsing functions.
 *
 * April 1993,  Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: smds_actions.c,v $
 * Revision 3.3.46.3  1996/05/21  10:09:54  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3.46.2  1996/04/29  10:03:33  mdb
 * The 96q1 gcc is much more particular about putting parentheses around
 * comparisons for some operands ("&", "^" and "|") [CSCdi56015].
 * Branch: California_branch
 *
 * Revision 3.3.46.1  1996/03/18  22:48:47  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.2  1996/03/07  11:08:24  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.14.1  1996/02/20  21:53:21  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/12/17  18:41:26  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:04:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:51:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:19:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <ciscolib.h>
#include "config.h"
#include "../parser/parser_actions.h"
#include "../wan/smds_actions.h"

#include "../wan/smds.h"

char *smds_addr_short_help (parseinfo *csb, uint flag, idbtype *idb)
{
    return("HD.DD.DD.HH");
}

boolean match_smds_addr (parseinfo *csb, char *buf, int *pi,
			 void *addr_pointer, int in_help, uint flag,
			 idbtype *idb)
{
    uchar addr[STATIONLEN_SMDS64];
    char c;
    ulong num;
    int i;
    hwaddrtype *hwaddrp = (hwaddrtype *)addr_pointer;

    num = 0;
    for (i=0; i<2; i++) {
        c = toupper(buf[(*pi)++]);
        if (isxdigit(c)) {
            num = num*16 + CVHTOB(c);
        } else {
            return(FALSE);
        }
    }

    /* Brain dead SMDS address are one byte of hex
     * followed be 7 bytes of bcd.
     */
    if (!SMDS_ISGROUP(num) && !SMDS_ISINDIV(num)) {
        return(FALSE);
    }
    addr[0] = (uchar) num;
    for (i=1; i<8; i++) {
        int j;
        c = buf[*pi];
	if((!c | (c == ' ')) && i >=5)
	    break;
        if (c == '.') {
            (*pi)++;
        }
        addr[i] = 0;
        for (j=0; j<2; j++) {

            c = buf[*pi];
            if ((c >= '0') && (c <= '9')) {
                num = c - '0';
	    } else {
		if (i >= 6) {
		    if (c != 'f' && c != 'F') {
		        return(FALSE);
		    }
		    num = 0xF;
		} else {
		    return(FALSE);
		}
	    }
            addr[i] = (addr[i] << 4) | num;
            (*pi)++;
        }
    }
    /*
     * Fill in the remainder with 0xFF for unused fields
     */
    for(;i < 8; i++) addr[i] = 0xff;

    hwaddrp->type = STATION_SMDS64;
    hwaddrp->length = STATIONLEN_SMDS64;
    hwaddrp->target_type = TARGET_UNICAST;
    bcopy(addr, hwaddrp->addr, STATIONLEN_SMDS64);

    return(TRUE);
}
