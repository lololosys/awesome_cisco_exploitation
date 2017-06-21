/* $Id: msg_dlc.c,v 3.2.60.3 1996/09/13 23:46:00 hampton Exp $
 * $Source: /release/112/cvs/Xsys/cls/msg_dlc.c,v $
 *------------------------------------------------------------------
 * DLC debug facility and message definition
 *
 * 11/25/94 - rich livingston
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_dlc.c,v $
 * Revision 3.2.60.3  1996/09/13  23:46:00  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/08/28  12:39:37  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.60.1  1996/05/17  10:43:18  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/11  14:36:21  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.1  1996/04/03  13:55:10  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2  1995/11/17  08:56:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:12:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:20:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define __DLC_MSG_EXTERN_H__

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(DLC);				/* Define the DLC facility */
msgdef_section("Data-link control error messages");
facdef(CLS);
msgdef_section("Cisco Link Services error message");

msgdef(OPTNULL, DLC, LOG_WARNING, MSG_TRACEBACK | MSG_PROCESS, "Error, NULL parameter unexpected.");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Call your technical support representative and report the error
	message and the system version. (Use the show version command to obtain
	the software version.)");
msgdef_ddts_component("");


msgdef(BADPARAM, DLC, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS, "Function %s:  value %#x passed in parameter %s");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Call your technical support representative and report the error
	message and the system version. (Use the show version command to obtain
	the software version.)");
msgdef_ddts_component("");


msgdef_limit(NOMEMORY, DLC, LOG_ERR, 0, "no memory for %s", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"There was not enough free memory to complete the operation.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(WPUTERR, DLC, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS, "unknown port type %x", MSGDEF_LIMIT_FAST);
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(INVPCEP, DLC, LOG_ERR, 0, "Close Station, invalid P_CEP");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(CLSFAIL, CLS, LOG_ERR, MSG_TRACEBACK, "CLS: Assertion failed: %s", MSGDEF_LIMIT_FAST);
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");
