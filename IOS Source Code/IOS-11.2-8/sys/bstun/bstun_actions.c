/* $Id: bstun_actions.c,v 3.4.42.3 1996/05/21 09:43:21 thille Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bstun_actions.c,v $
 *------------------------------------------------------------------
 * BSTUN specific parsing functions.
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bstun_actions.c,v $
 * Revision 3.4.42.3  1996/05/21  09:43:21  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4.42.2  1996/05/17  10:40:57  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.30.2  1996/04/11  08:19:45  jbalestr
 * CSCdi51807:  need a test command to transmit raw bisync frames
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.30.1  1996/04/03  13:50:32  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.42.1  1996/03/18  19:01:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.2  1996/03/07  08:39:10  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.12.1  1996/02/20  13:38:38  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/12/20  02:07:55  jbalestr
 * CSCdi45821:  bstun_actions doesnt include .h file
 *
 * Revision 3.3  1995/12/17  18:24:53  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  08:48:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:14:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ctype.h>
#undef   toupper			/* use library function call */
#include "address.h"
#include "config.h"
#include "interface_private.h"
#include "../parser/parser_actions.h"
#include "logger.h"
#include "bstun.h"
#include "bstun_actions.h"

/*
 * Forwared declarations
 */
static void num_to_byte(ulong, uchar *, int);

char *bstun_addr_short_help (parseinfo *csb, uint flag, idbtype *idb)
{
    char format;

    if (csb->interface && !csb->bstungroup) {
	csb->bstungroup = csb->interface->hwptr->bstun_group;
    }
    if (csb->bstungroup) {
	format = bstun_pinfo[csb->bstungroup]->addrfmt;
    } else {
	format = 'x';
    }

    switch (format) {
      case 'o':
	return("Oct Num");
      case 'd':
	return("Dec Num");
      default:
      case 'x':
	return("Hex Num");
    }
}

boolean match_bstun_addr (parseinfo *csb, char *buf, int *pi,
			 void *addr_pointer, int in_help, uint flag,
			 idbtype *idb)
{
    addrtype *addrp = (addrtype *)addr_pointer;
    int j;
    ulong addr;
    ulong ubound;
    char addrfmt;
    int addrlen;
    char *s;
    int matched = FALSE;
    int group = csb->bstungroup;

    if ((group > 0) && (group <= BSTUN_MAX_GROUP) && bstun_pinfo[group]) {
	addrfmt = bstun_pinfo[group]->addrfmt;
	addrlen = bstun_pinfo[group]->addrlength;
    } else {
	addrfmt = 'x';
	addrlen = 8;
    }

    /* Allow only addrlen bytes. */
    addrlen = min(addrlen, (addrfmt == 'x' ? 8 : 4));

    /* Check for zero length addr */
    if (addrlen == 0) {

        /*
         *  A hack to accept a zero length addr.
         *  Accept a multi-byte addr, then set the
         *  addr to zero.
         */

        addrlen = (addrfmt == 'x' ? 8 : 4);
    }

    /* Find largest acceptable number */
    ubound = 0;
    for (j = 0; j < addrlen; j++) {
	ubound = (ubound << 8) | 0xff;
    }

    switch (addrfmt) {
      case 'o':
	matched = match_ulong_octal(buf, 0L, ubound, pi, &addr);
	break;

      case 'd':
	matched = match_ulong_decimal(buf, 0L, ubound, pi, &addr);
	break;

      default:
      case 'x':
	s = buf;
	if ((*s == '0') && (toupper(*(s + 1)) == 'X')) {
	    *pi += 2;           /* ignore leading 0x if present */
	}
	matched =match_ulong_hexadecimal(buf, 0L, ubound, pi, &addr);
    }

    if (addrlen == 0) {
	addr = 0;
    }

    if (matched) {

	addrp->type = ADDR_BSTUN;
	addrp->length = ADDRLEN_BSTUN;

	/*
	 * Even though cisco stores numeric bytes (of which there are at most
	 * 4), they seem to be using the input length as the output length
	 * (which therefore may be up to 8).
	 */
	num_to_byte(addr, addrp->bstun_addr, addrlen);

	return(TRUE);
    }
    return(FALSE);
}

/* Copy number in addr to buf, putting Least Sig. Byte of
 * the number in the highest address in buf. Use only num bytes.
 */
static void num_to_byte (ulong addr, uchar *buf, int num)
{
    int i;
    ulong mask = 0xff;

    for (i = num - 1; i >= 0; i--) {
        buf[i] = (uchar) (addr & mask);
        addr = addr >> 8;
    }
}
