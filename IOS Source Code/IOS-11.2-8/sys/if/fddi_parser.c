/* $Id: fddi_parser.c,v 3.4.18.2 1996/06/18 20:05:35 fsunaval Exp $
 * $Source: /release/112/cvs/Xsys/if/fddi_parser.c,v $
 *------------------------------------------------------------------
 * fddi_parser.c -- Fiber Distributed Data Interface parser routines
 *
 * Sept. 1995, Greg Stovall
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fddi_parser.c,v $
 * Revision 3.4.18.2  1996/06/18  20:05:35  fsunaval
 * CSCdi59153:  Provide support for cmt con and cmt dis.
 * Branch: California_branch
 * 1."cmt con" and "cmt dis" put into platform specific files.
 *
 * Revision 3.4.18.1  1996/03/18  20:13:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.28.2  1996/03/13  01:35:14  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.28.1  1996/02/20  00:56:57  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/07  16:12:31  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.3  1996/02/01  06:04:00  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.2  1995/11/16  23:37:46  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:49:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/17  23:06:05  gstovall
 * Placeholders for Talisker_branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../xdi/cschdrsif.h"
#include "config.h"
#include "parser.h"
#include "../if/parser_defs_fddi.h"

/* XDI includes */
#include "../xdi/xdihdrsif.h"
#include "../xdi/csphdr.h"
#include "../xdi/smtmsg.h"
#include "../xdi/cspproto.h"

/*
 * cmt_command
 *
 * cmt disconnect <interface> <phy-a | phy-b>
 * cmt connect    <interface> <phy-a | phy-b>
 */
void cmt_command (parseinfo *csb)
{
    switch (csb->func) {
      case CMT_CONNECT:
	cmt_connect(hwidb_or_null(GETOBJ(idb,1)), GETOBJ(int,2));
	break;
      case CMT_DISCONNECT:
	cmt_disconnect(hwidb_or_null(GETOBJ(idb,1)), GETOBJ(int,2));
	break;
      default:
	bad_parser_subcommand(csb, csb->func);
	break;
    }
}
