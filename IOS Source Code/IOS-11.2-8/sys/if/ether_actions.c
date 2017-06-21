/* $Id: ether_actions.c,v 3.4.44.1 1996/05/21 09:49:42 thille Exp $
 * $Source: /release/112/cvs/Xsys/if/ether_actions.c,v $
 *------------------------------------------------------------------
 * Ethernet specific parsing functions.
 *
 * April 1993,  Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ether_actions.c,v $
 * Revision 3.4.44.1  1996/05/21  09:49:42  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4  1995/12/17  18:28:45  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/12/16  05:12:00  widmer
 * CSCdi45745:  Need to modularize ethernet address parsing more
 *
 * Revision 3.2  1995/11/17  09:26:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:51:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "../parser/parser_actions.h"
#include "subsys.h"

static char *en_addr_short_help (parseinfo *csb, uint flag, idbtype *idb)
{
    return("H.H.H");
}

static boolean match_en_addr (parseinfo *csb, char *buf, int *pi,
			      void *addr_pointer, int in_help,
			      uint flag, idbtype *idb)
{
    hwaddrtype *addrp = (hwaddrtype *)addr_pointer;
    ushort hexval[3];

    if (match_hexadecimal(buf, pi, &hexval[0]) &&
        match_char(buf, pi, '.') &&
        match_hexadecimal(buf, pi, &hexval[1]) &&
        match_char(buf, pi, '.') &&
        match_hexadecimal(buf, pi, &hexval[2])) {

        addrp->type = STATION_IEEE48;
        addrp->length = STATIONLEN_IEEE48;
        addrp->target_type = TARGET_UNICAST;
        ((ushort *) addrp->addr)[0] = hexval[0];
        ((ushort *) addrp->addr)[1] = hexval[1];
        ((ushort *) addrp->addr)[2] = hexval[2];
	return(TRUE);
    }
    return(FALSE);
}

static void ether_actions_init (subsystype *subsys)
{
    parser_add_address(ADDR_PARSE(STATION_IEEE48), en_addr_short_help,
		       match_en_addr);
}

/*
 *  Ethernet address parsing subsystem header
 */

#define ETHER_ACTIONS_MAJVERSION 1
#define ETHER_ACTIONS_MINVERSION 0
#define ETHER_ACTIONS_EDITVERSION  1

SUBSYS_HEADER(ether_actions, ETHER_ACTIONS_MAJVERSION,
	      ETHER_ACTIONS_MINVERSION, ETHER_ACTIONS_EDITVERSION,
	      ether_actions_init, SUBSYS_CLASS_MANAGEMENT,
	      NULL, NULL);
