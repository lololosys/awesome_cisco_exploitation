/* $Id: snapshot_actions.c,v 3.3.46.1 1996/05/21 10:09:56 thille Exp $
 * $Source: /release/112/cvs/Xsys/wan/snapshot_actions.c,v $
 *------------------------------------------------------------------
 * Snapshot specific parsing functions.
 *
 * June 1994, Joel P. Bion
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: snapshot_actions.c,v $
 * Revision 3.3.46.1  1996/05/21  10:09:56  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3  1995/12/17  18:41:28  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:04:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:51:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:19:59  hampton
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
#include "../wan/snapshot_actions.h"
#include "../wan/snapshot.h"
#include "../wan/snapshot_private.h"

char *snapshot_addr_short_help (parseinfo *csb, uint flag, idbtype *idb)
{
    return("N");
}

boolean match_snapshot_addr (parseinfo *csb, char *buf, int *pi,
			     void *addr_pointer, int in_help, uint flag,
			     idbtype *idb)
{
    addrtype *addrp = (addrtype *)addr_pointer;
    ushort num;

    if (match_decimal(buf, 1, 255, pi, &num)) {
	addrp->snapshot_addr = num;
	addrp->type = ADDR_SNAPSHOT;
	addrp->length  = ADDRLEN_SNAPSHOT;
	return(TRUE);
    }
    return(FALSE);
}
