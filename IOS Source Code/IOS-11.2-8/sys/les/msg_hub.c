/* $Id: msg_hub.c,v 3.3.62.2 1996/09/13 23:50:33 hampton Exp $
 * $Source: /release/112/cvs/Xsys/les/msg_hub.c,v $
 *------------------------------------------------------------------
 * msg_hub.c - Message file for HUB interface
 *
 * September 1994, Viet Nguyen
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Gurus: Steve Garcia, Viet Nguyen
 *------------------------------------------------------------------
 * $Log: msg_hub.c,v $
 * Revision 3.3.62.2  1996/09/13  23:50:33  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.62.1  1996/08/28  12:59:35  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3  1995/11/17  17:42:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:00:33  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:33:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:38:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"
#include "../os/msgtxt_sys.h"


facdef(HUB);				/* Define the facility */
msgdef_section("Cisco Ethernet hub error messages");


msgdef(BADHUB, HUB, LOG_ALERT, 0, "Invalid hub type %d and number %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADUNIT, HUB, LOG_ALERT, 0, "Bad port number %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(NOMEMORY, HUB, LOG_ALERT, 0, msgtxt_nomemory, MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"The system detected that there is not enough memory for the hub
	initialization.");
msgdef_recommended_action(
	"If conditions warrant, upgrade to larger memory configuration.");
msgdef_ddts_component("");


msgdef(READERR, HUB, LOG_ALERT, 0, "Read op %d not allowed");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(WRITEERR, HUB, LOG_ALERT, 0, "Write op %d not allowed");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");
