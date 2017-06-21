/* $Id: msg_mailbox.c,v 3.2.62.2 1996/09/13 23:50:38 hampton Exp $
 * $Source: /release/112/cvs/Xsys/les/msg_mailbox.c,v $
 *------------------------------------------------------------------
 * msg_mailbox.c - Message file for sbe facility
 *
 * February 28, 1991, Chris Shaker
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_mailbox.c,v $
 * Revision 3.2.62.2  1996/09/13  23:50:38  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/08/28  12:59:40  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  17:42:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:34:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/21  03:01:23  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.1  1995/06/07  21:39:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(MAILBOX);
msgdef_section("ChipCom mailbox error messages");


msgdef(ONLINE, MAILBOX, LOG_DEBUG, 0, "%s mailbox coming online.");
msgdef_explanation(
	"This message is generated only when mailbox debugging is enabled. It
	provides information about the state of each incoming or outgoing
	mailbox.");
msgdef_recommended_action(
	"Advisory message only. No action required.");
msgdef_ddts_component("");


msgdef(OFFLINE, MAILBOX, LOG_ERR, 0,
       "%s mailbox is offline. Interrupt ignored.");
msgdef_explanation(
	"This message is generated when an attempt is made by the management
	module to communicate with an offline mailbox. When it appears, it
	indicates a problem exists between the perceived state of the mailbox
	from the router's side versus the management module's side.");
msgdef_recommended_action(
	"Issue a system-wide reset on the management module. If the error
	persists, call your technical support representative for assistance.");
msgdef_ddts_component("");


msgdef_limit(TIMEOUT, MAILBOX, LOG_ERR, 0,
      "Intra-PDU timeout occurred on %s mailbox data.", 5000);
msgdef_explanation(
	"A timeout occurred while sending or receiving the characters of a
	protocol data unit (PDU). The entire PDU will be ignored.");
msgdef_recommended_action(
	"Informational message only. No action required");
msgdef_ddts_component("");


msgdef(INITFAIL, MAILBOX, LOG_ERR, 0,
      "Mailbox initialization failure. %s Mailbox offline.");
msgdef_explanation(
	"A catastrophic failure involving the initialization of the
	administrative mailbox occurred. The mailbox will be taken offline and
	remain in that state until a router module reset, or a system reload,
	occurs At that time, initialization will again be attempted. Note that
	the functionality of the router, that is, it's ability to receive and
	forward packets, is not affected by this error.");
msgdef_recommended_action(
	"Confirm the router module installation. Make sure the software
	revision on the management module and the router module carrier card is
	up-to-date. If the error persists, call your technicalsupport
	representative for assistance.");
msgdef_ddts_component("");


msgdef(INITOK, MAILBOX, LOG_INFO, 0, "Mailbox initialization successful.");
msgdef_explanation(
	"This message is generated after a router reload to indicate the
	mailbox was successfully initialized.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef(BADCHKSUM, MAILBOX, LOG_ERR, 0,
      "Checksum failed. Expected = 0x%02x, calculated = 0x%02x. Ignoring PDU.");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADECHO, MAILBOX, LOG_ERR, 0,
       "Echo-response did not match echo-request!");
msgdef_explanation(
	"The data received from an ECHO_RESPONSE protocol data unit (PDU) did
	not match the original data provided in the ECHO_REQUEST. Usually, this
	message is seen during initialization, and indicates a catastrophic
	failure of the mailbox interface. See MAILBOX-3-INITFAIL for more
	information.");
msgdef_recommended_action(
	"Confirm the router module installation. Make sure the software
	revision on the management module and the router module carrier card is
	up-to-date. If the error persists, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(NOECHO, MAILBOX, LOG_ERR, 0,
      "Echo-request timed out. No response received. Mailbox offline.");
msgdef_explanation(
	"An ECHO_RESPONSE was not received in the appropriate time after the
	generation of an ECHO_REQUEST. This failure only occurs during mailbox
	initialization, and indicates a problem between the communication path
	of the router module and its carrier card.");
msgdef_recommended_action(
	"Confirm the router module installation. Make sure the software
	revision on the management module and the router module carrier card is
	up-to-date. If the error persists, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(BADPDU, MAILBOX, LOG_ERR, 0,
      "PDU of type %s received. Invalid or unsupported. Ignoring.");
msgdef_explanation(
	"The protocol data unit (PDU) received was valid, but the type is not
	supported in the current software implementation. It will be
	ignored.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef(WRITE, MAILBOX, LOG_DEBUG, 0, "Writing 0x%02x to carrier.");
msgdef_explanation(
	"This message is generated only when mailbox debugging is enabled. It
	provides very low-level information about the outgoing mailbox data
	stream.");
msgdef_recommended_action(
	"Advisory message only. No action required.");
msgdef_ddts_component("");


msgdef(READ, MAILBOX, LOG_DEBUG, 0, "Reading 0x%02x from carrier.");
msgdef_explanation(
	"This message is generated only when mailbox debugging is enabled. It
	provides very low-level information about the incoming mailbox data
	stream.");
msgdef_recommended_action(
	"Advisory message only. No action required.");
msgdef_ddts_component("");


msgdef(MBOXDEBUG, MAILBOX, LOG_DEBUG, 0, "%s");
msgdef_explanation(
	"This message header is paired with general debugging messages used to
	provide information about the functionality of the mailbox. To enable
	mailbox debugging, issue the command debug mailbox.");
msgdef_recommended_action(
	"Advisory message only. No action required.");
msgdef_ddts_component("");
