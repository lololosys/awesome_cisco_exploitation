/* $Id: msg_aip.c,v 3.3.52.2 1996/09/13 23:46:22 hampton Exp $
 * $Source: /release/112/cvs/Xsys/hes/msg_aip.c,v $
 *------------------------------------------------------------------
 * AIP error messages.
 *
 * June 1995, Andrew Wu
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_aip.c,v $
 * Revision 3.3.52.2  1996/09/13  23:46:22  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.52.1  1996/08/28  12:45:28  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3  1995/11/29  19:25:43  weismann
 * CSCdi34084: Cosmetic fix to remove spurious characters from message
 * output.
 *
 * Revision 3.2  1995/11/23  10:39:36  weismann
 * CSCdi34084:  peak/ave/burst is not always displayed properly in show
 *              atm vc. Fixed atm debug,show output. Handle atm vc
 *              defaults with flags.
 *
 * Revision 3.1  1995/11/09  11:41:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/07  20:13:09  awu
 * CSCdi36817:  display the error code in hex in the aiprejcmd msg.
 *
 * Revision 2.1  1995/07/01  05:58:07  awu
 * CSCdi36167:  ATM calls printf from interrupt level.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(AIP);
msgdef_section("ATM Interface Processor error messages");


msgdef_limit(AIPREJCMD, AIP, LOG_ERR, 0,
	     "Interface %s, AIP driver rejected %s command (error code 0x%04x)",
	     MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"The AIP microcode rejected a command sent by the Route Processor (RP).
	The error code is the status value returned by the command.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(AIPFAILSETUPVC, AIP, LOG_ERR, 0,
	     "Interface %s, Failed to setup vc %d (Cause: %s)",
	     MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"The AIP driver failed to set up a virtual circuit.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(AIPFAILSETUPVCAVG, AIP, LOG_ERR, 0,
             "Interface %s, Failed to setup vc %d (Cause: %s) Expected range [%d .. %d]Kbps for AIP Average Rate Metering. Average Rate=Peak Rate= %d Kbps turns Average Rate Metering OFF.", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");
