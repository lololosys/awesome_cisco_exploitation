/* $Id: msg_lex.c,v 3.2.62.2 1996/09/13 23:50:48 hampton Exp $
 * $Source: /release/112/cvs/Xsys/lex/msg_lex.c,v $
 *------------------------------------------------------------------
 * msg_lex.c -- Message file for Lex
 *
 * June 1994, David Coli
 *
 * Copyright (c) 1994,1996 by Cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: msg_lex.c,v $
 * Revision 3.2.62.2  1996/09/13  23:50:48  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/08/28  13:00:09  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  17:43:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:35:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:41:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(LEX);
msgdef_section("LAN extension error messages");


msgdef(TIMEOUT, LEX, LOG_ERR, 0, "Retry count exceeded for %s : %s");
msgdef_explanation(
	"The host router re-sent the given message as many times as specified
	in the lex retry-count command configuration variable, without
	receiving an acknowledgment from the LAN Extender. At this point, the
	host router gives up trying to send this particular remote command
	message.");
msgdef_recommended_action(
	"Use the LEDs on the LAN Extender to determine if it is operational.
	Confirm that the lex timeout configuration command is set appropriately
	for the speed of the link.");
msgdef_ddts_component("");


msgdef(NOTSENT, LEX, LOG_ERR, 0, "Serial line down, %s : %s not sent");
msgdef_explanation(
	"The serial line went down, and the specified command was not sent to
	the LAN Extender.");
msgdef_recommended_action(
	"Determine why the serial line went down. For example, was the cable
	was pulled, or the LAN Extender powered off? The command will
	automatically be re-sent once the serial interface is operational.");
msgdef_ddts_component("");


msgdef(RCMDNAK, LEX, LOG_ERR, 0, " %s : %s could not be processed by LAN Extender");
msgdef_explanation(
	"The LAN Extender could not process the command from the host router.
	Possible causes are:

	The LAN Extender ran out of memory.

	The message from the host router is malformed.

	A request to write flash occurred while the LAN Extender was busy
	writing flash for a previous request.

	The size of a file to be downloaded is greater than the amount of flash
	memory available.");
msgdef_recommended_action(
	"If the command was copy tftp lex unit-number or copy flash lex
	unit-number, make sure that the correct file, that is, a FLEX image
	file, is specified. Only a single copy request can be handled at one
	time. If the problem was caused by a temporary out-of-memory condition
	on the LAN Extender, re-enter the command.");
msgdef_ddts_component("");


msgdef(RCMDREJ, LEX, LOG_ERR, 0, " %s : %s could not be processed by LAN Extender");
msgdef_explanation(
	"The LAN Extender did not recognize the code for the specified remote
	command. This might occur if the user is running old LAN Extender
	software with newer host router software that has support for new LEX
	remote commands.");
msgdef_recommended_action(
	"Update the LAN Extender software.");
msgdef_ddts_component("");
