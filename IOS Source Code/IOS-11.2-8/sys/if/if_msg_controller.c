/* $Id: if_msg_controller.c,v 3.2.60.3 1996/09/13 23:49:22 hampton Exp $
 * $Source: /release/112/cvs/Xsys/if/if_msg_controller.c,v $
 *------------------------------------------------------------------
 * if_msg_controller.c - controller msg facility.
 *               (based on hes/msg_controller.c by Carl Schaefer)
 *
 * Jan 30 1995, Manoj Leelanivas
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_msg_controller.c,v $
 * Revision 3.2.60.3  1996/09/13  23:49:22  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/08/28  12:48:03  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.60.1  1996/07/08  22:31:04  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:28:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:50:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/09  17:45:00  jliu
 * CSCdi35546:  One second update message may lose under heavy loading
 *
 * Revision 2.1  1995/06/07  20:52:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(CONTROLLER);
msgdef_section("Controller error messages");


msgdef(UPDOWN, CONTROLLER, LOG_ERR, 0,
       "Controller %s, changed state to %s");
msgdef_explanation(
	"A configurable controller changed its state.");
msgdef_recommended_action(
	"No action required.");
msgdef_ddts_component("");


msgdef(REMLOOP, CONTROLLER, LOG_ERR, 0,
       "Controller %s, remote loop %s %s");
msgdef_explanation(
	"A configurable controller entered remote loopback.");
msgdef_recommended_action(
	"No action required.");
msgdef_ddts_component("");


msgdef(CRASHED, CONTROLLER, LOG_CRIT, MSG_TRACEBACK,
       "Interface Processor in slot %d not responding (%x): shutting it down");
msgdef_explanation(
	"A controller stopped responding to commands over the cBus, so it was
	put into shutdown mode.");
msgdef_recommended_action(
	"Re-enable the controllers by issuing the no shut configuration command
	on all controllers on the card, or hotswap out the bad card and install
	the new one. You must still manually \"no shut\" the controllers.");
msgdef_ddts_component("");


msgdef(NOTFOUND, CONTROLLER, LOG_CRIT, 0,
       "Controller not found for slot %d.");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(FIRMWARE, CONTROLLER, LOG_CRIT, 0, "Controller %s, %s");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");
