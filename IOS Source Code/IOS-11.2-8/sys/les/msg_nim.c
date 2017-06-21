/* $Id: msg_nim.c,v 3.2.62.2 1996/09/13 23:50:44 hampton Exp $
 * $Source: /release/112/cvs/Xsys/les/msg_nim.c,v $
 *------------------------------------------------------------------
 * Message file for XX platform
 *
 * March 1993, Scott Mackie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_nim.c,v $
 * Revision 3.2.62.2  1996/09/13  23:50:44  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/08/28  12:59:45  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  17:42:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:34:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:39:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(NIM);				/* Define the facility */
msgdef_section("Network interface module error messages");


msgdef(NOHWADDRESS, NIM, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "All hardware addresses have been allocated - maximum of %d\n");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADSUBUNIT, NIM, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Illegal reference to non-existent subunit %d in slot %d\n");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADSLOT, NIM, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Illegal reference to non-existent slot %d\n");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(DEADICHAINQ, NIM, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Attempted to manipulate uninitialized ichainQ in %s\n");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(LOSTICHAINQ, NIM, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Couldn't find idb 0x%x in ichainQ in %s\n");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADNIMREV, NIM, LOG_CRIT, 0,
       "Unsupported version of %s NIM in slot %d  Version 0x%x\n");
msgdef_explanation(
	"The indicated network interface module (NIM) is not supported because
	it is not of the correct revision. Certain platforms (Cisco 4500 or
	Cisco 4700, for example) require some NIMs to be of a minimum rev
	level.");
msgdef_recommended_action(
	"Upgrade the NIM to the proper level.");
msgdef_ddts_component("");


msgdef(UNSUPNIM, NIM, LOG_CRIT, 0, "Unsupported %s NIM in slot %d\n");
msgdef_explanation(
	"The indicated NIM is not supported on this platform.");
msgdef_recommended_action(
	"Remove the NIM.");
msgdef_ddts_component("");
