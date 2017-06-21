/* $Id: msg_bri.c,v 3.3.62.2 1996/09/13 23:50:21 hampton Exp $
 * $Source: /release/112/cvs/Xsys/les/msg_bri.c,v $
 *----------------------------------------------------------------------
 * msg_bri.c - Message file for bri facility
 *
 * Nov 20, 1991 Greg Stovall
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *----------------------------------------------------------------------
 * $Log: msg_bri.c,v $
 * Revision 3.3.62.2  1996/09/13  23:50:21  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.62.1  1996/08/28  12:59:23  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3  1995/11/17  17:41:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:00:26  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:33:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:38:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *----------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"
#include "../os/msgtxt_sys.h"


facdef(BRI);				/* Define the facility */
msgdef_section("ISDN Basic Rate Interface error messages");


msgdef_limit(NOMEMORY, BRI, LOG_ALERT, 0, msgtxt_nomemory, MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"The requested operation could not be accomplished because of a low
	memory condition.");
msgdef_recommended_action(
	"Reduce other system activity to ease memory demands. If conditions
	warrant, upgrade to a larger memory configuration.");
msgdef_ddts_component("");


msgdef(INITFAIL, BRI, LOG_ALERT, 0, msgtxt_initfail);
msgdef_explanation(
	"The hardware failed to initialize correctly.");
msgdef_recommended_action(
	"Repair or replace the controller.");
msgdef_ddts_component("");
