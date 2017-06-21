/* $Id: msg_lineproto.c,v 3.2.60.2 1996/09/13 23:49:27 hampton Exp $
 * $Source: /release/112/cvs/Xsys/if/msg_lineproto.c,v $
 *------------------------------------------------------------------
 * msg_lineproto.c - Message file for line protocol
 *
 * Aug 6, 1992, Robert Snyder
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_lineproto.c,v $
 * Revision 3.2.60.2  1996/09/13  23:49:27  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/08/28  12:48:21  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  09:30:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:50:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:53:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(LINEPROTO);			/* Define datalink facility */
msgdef_section("Line Protocol error message");


msgdef(UPDOWN, LINEPROTO, LOG_NOTICE, 0, "Line protocol on Interface %s, changed state to %s");
msgdef_explanation(
	"The data link level line protocol changed state.");
msgdef_recommended_action(
	"Notification message only. No action required.");
msgdef_ddts_component("");
