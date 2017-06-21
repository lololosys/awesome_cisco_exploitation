/* $Id: msg_standby.c,v 3.2.60.3 1996/09/13 23:50:08 hampton Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/msg_standby.c,v $
 *------------------------------------------------------------------
 * msg_standby.c -- Messages for hot standby protocol
 *
 * Sep 1993 Tony Li
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_standby.c,v $
 * Revision 3.2.60.3  1996/09/13  23:50:08  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/08/28  12:53:04  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.60.1  1996/07/23  13:25:02  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.2  1995/11/17  17:35:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:09:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:08:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(STANDBY);
msgdef_section("Hot Standby Router Protocol error messages");


msgdef(NOSOCKET, STANDBY, LOG_ERR, 0, "Unable to open socket");
msgdef_explanation(
	"The system was unable to initialize an IP connection for the Hot
	Standby protocol.");
msgdef_recommended_action(
	"Make sure that there is at least one interface configured to run
	IP.");
msgdef_ddts_component("");


msgdef(BADAUTH, STANDBY, LOG_ERR, 0, "Bad authentication from %i,"
       " remote state %s");
msgdef_explanation(
	"Two routers participating in the Hot Standby protocol disagree on the
	valid authentication string.");
msgdef_recommended_action(
	"Use the standby authentication interface command to repair the Hot
	Standby protocol authentication discrepancy between the local system
	and the one whose IP address is reported.");
msgdef_ddts_component("");


msgdef(MISCONFIG, STANDBY, LOG_ERR, 0, "Attempt to change %s MAC address"
       " to %e when DECNET already running");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");
