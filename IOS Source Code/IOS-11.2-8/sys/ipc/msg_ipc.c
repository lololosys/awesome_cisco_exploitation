/* $Id: msg_ipc.c,v 3.2.60.2 1996/09/13 23:49:45 hampton Exp $
 * $Source: /release/112/cvs/Xsys/ipc/msg_ipc.c,v $
 *------------------------------------------------------------------
 * msg_ipc.c - Logger error messages for IPC system.
 *
 * March 1995, Jim Hayes
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_ipc.c,v $
 * Revision 3.2.60.2  1996/09/13  23:49:45  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/08/28  12:50:55  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  17:29:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:00:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:05:50  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/14  15:18:32  anke
 * CSCdi35807:  Yet more spellink errors
 *
 * cls/dlcqllc.c:         Unexepcted -> Unexpected
 * ipc/msg_ipc.c:         availble -> available
 * exec_debug_channel.h:  debuggeries -> debugging
 * filesys/fslib.c:       inconsistant -> inconsistent
 * atm/sscop.c:           initalize -> initialize
 * filesys/fslib_print.c: progresss -> progress
 * ts/callback.c:         unnexpected -> unexpecte
 *
 * Revision 2.1  1995/06/07  21:03:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(IPC);
msgdef_section("Interprocess Communication error messages");


msgdef(NOMEM, IPC, LOG_CRIT, 0, "No memory available for %s.");
msgdef_explanation(
	"The IPC protocol subsystem could not obtain the memory it needed.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(NOBUFFER, IPC, LOG_CRIT, MSG_PROCESS,
	     "Could not fetch a new message buffer.",
	     MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(CONSISTENCY, IPC, LOG_CRIT, MSG_TRACEBACK,
       "Message failed consistency check: %s");
msgdef_explanation(
	"An internal inconsistency was found in some IPC data structures.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef(SLAVELOG, IPC, LOG_NOTICE, 0, "VIP-%s");
msgdef_explanation(
	"The Cisco IOS software, running on a Versatile Interface Processor
	(VIP) card, generated this message. The error message has been passed
	up to the Route Processor or Route Switch Processor (RP/RSP) for
	display. This message appears only if the user has configured service
	slavelog.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(NODELFUNC, IPC, LOG_NOTICE, 0, "Delayed init function not available");
msgdef_explanation(
	"The IPC application cannot be initialized because its initialization
	function does not appear in the IPC initialization function list.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(UNLOCK, IPC, LOG_CRIT, MSG_TRACEBACK,
       "Unlock done on already unlocked element.");
msgdef_explanation(
	"An internal inconsistency was found in some IPC data structures.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef(LOCK, IPC, LOG_CRIT, MSG_TRACEBACK, "Lock done a deleted element.");
msgdef_explanation(
	"An internal inconsistency was found in some IPC data structures.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef(CANT_SEND, IPC, LOG_CRIT, MSG_TRACEBACK, "Cannot send IPC message: %s");
msgdef_explanation(
	"An error occurred in the IPC slave discovery mechanism. It might
	result in a malfunction in the operation of the IPC.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(NODISPATCH, IPC, LOG_CRIT, 0,
	     "Message from %d.%d has no receive queue or dispatch routine.",
	     MSGDEF_LIMIT_FAST);
msgdef_explanation(
	"The IPC user failed to provide any means of handling the received
	message.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef_limit(DELETED, IPC, LOG_CRIT, MSG_TRACEBACK,
	     "Attempt to delete an IPC message (0x%x) a second time.",
	     MSGDEF_LIMIT_FAST);
msgdef_explanation(
	"An internal inconsistency was found in some IPC data structures.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef_limit(ONINT, IPC, LOG_CRIT, MSG_TRACEBACK,
 	     "Called from interrupt level: %s", MSGDEF_LIMIT_FAST);
msgdef_explanation(
	"The IPC user attempted a prohibited call into IPC while IPC was
	running on the interrupt stack.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");
