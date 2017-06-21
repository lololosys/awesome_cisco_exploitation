/* $Id: msg_rsp.c,v 3.9.8.10 1996/09/14 00:02:05 hampton Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/msg_rsp.c,v $
 *------------------------------------------------------------------
 * RSP messages
 *
 * April 1994, Chris Shaker
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_rsp.c,v $
 * Revision 3.9.8.10  1996/09/14  00:02:05  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.9.8.9  1996/08/29  22:55:51  ganesh
 * CSCdi62683:  HSA requires that both RSP2s have same amount of DRAM
 * Branch: California_branch
 * Added HSA_MEM and HSA_MINMEM warning messages.
 *
 * Revision 3.9.8.8  1996/08/28  13:13:06  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.9.8.7  1996/08/15  15:31:59  skufer
 * CSCdi60891:  HSA needs to work with SVIP and RVIP
 * Branch: California_branch
 *         HSA (fail-over dual RSP) now works with VIP.
 *
 * Revision 3.9.8.6  1996/06/20  21:03:28  shaker
 * CSCdi60921:  More sanity checking needed on ReturnQ processing
 * Branch: California_branch
 * Verify that the queue pointer is non zero before using it,
 * to prevent 'reserved exception' errors from crashing the router.
 *
 * Revision 3.9.8.5  1996/06/20  15:40:30  skufer
 * CSCdi60833:  rsp HSA does not work with CIP
 * Branch: California_branch
 *         Added CIP as a supported interface processor with HSA (dual
 *         rsp in 7507 or 7513).  While the bonnet's up, changed the
 *         error message to better inform customers how to proceed.
 *
 * Revision 3.9.8.4  1996/06/09  23:15:23  skufer
 * CSCdi57905:  HSA needs error message when customer installs VIP or CIP
 * Branch: California_branch
 *     When HSA (two RSP's in a 7507 or 7513) is enabled, it currently
 *     cannot coexist with CIP, RVIP or SVIP adapters.  A function was
 *     introduced to detect this condition, print out an informative
 *     error message and crash the system (thereby preventing operation
 *     with both a second RSP and aforementioned xIP).  This will be
 *     removed when IPC allows these to coexist.
 *
 * Revision 3.9.8.3  1996/06/09  09:04:43  rramacha
 * CSCdi58196:  Transmitter-delay broken on FSIP DCE interfaces
 * o FSIP panic message support, Use generic IP_PANIC for other interfaces
 *   too.
 * Branch: California_branch
 *
 * Revision 3.9.8.2  1996/05/06  07:15:31  schaefer
 * CSCdi49868: crash at interrupt level on rsp2 (7513)
 * Branch: California_branch
 * complain & return early if no IDB found for MIP channel
 *
 * Revision 3.9.8.1  1996/04/22  18:58:57  ssangiah
 * CSCdi54920:  Need to add support for the flash MIB on c7200.
 * Branch: California_branch
 *
 * Revision 3.9  1996/02/28  09:33:47  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.8  1996/02/04  12:17:24  shaker
 * CSCdi47971:  RSP-3-NORESTART error message
 * Use portable array references instead of post incremented pointer
 * dereferences when passing arguments to errmsg(), because the order
 * of function argument evaluation varies.
 *
 * Revision 3.7  1996/02/04  02:29:24  shaker
 * CSCdi47971:  RSP-3-NORESTART error message
 * Prevent this error from happening. Also enhance error logging
 * for some hard to reproduce error cases.
 *
 * Revision 3.6  1996/01/23  01:17:57  dbath
 * CSCdi47277:  HSA slave leaks IPC buffers when version mismatch
 * Generate some form of response to all rpc requests, no matter how
 * little we might understand them.  Also, add a simple m/s exchange
 * version check on sign-in.
 *
 * Revision 3.5  1996/01/06  08:52:45  dlobete
 * CSCdi46092:  %IPC_RSP_CBUS-3-NOBUF message on RSP with VIP under traffic
 * Fall back to other free queues if IPC queue is depleted.
 * Make no-ipc-buffer message have traceback and also rate-limited.
 *
 * Revision 3.4  1995/12/15  11:57:37  ogrady
 * CSCdi43520:  custom queueing locks up serial interface on 7513
 * Send buffers that restart interface output queues to the return queue
 * instead of the original free queue.  Add a few sanity checks and error
 * messages.
 *
 * Revision 3.3  1995/11/22  03:43:51  dbath
 * CSCdi43201:  Unecessary errmsgs need to be collapsed into existing
 * errmsg
 * Rationalise error messages as promised in the doco review.
 *
 * Revision 3.2  1995/11/17  18:46:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:27:49  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/09/17  20:55:11  motto
 * CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
 *
 * Revision 2.3  1995/07/21  19:48:44  schaefer
 * CSCdi36254: traceback displayed when changing serial encaps
 * emit traceback only if "debug cbus" when recarving
 *
 * Revision 2.2  1995/07/17  08:21:49  schaefer
 * CSCdi36181: more robust microcode management (on RSP)
 *
 * Revision 2.1  1995/06/07  22:57:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(RSP);
msgdef_section("Route Switch Processor error messages");


msgdef_limit(NOMEMORY, RSP, LOG_CRIT, 0,
	     "No memory available for %s", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"An attempt at memory allocation failed.");
msgdef_recommended_action(
	"Try these actions to remedy the problem:

	Add memory.

	Disable some features.

	Apply filtering to decrease the size of system data structures - the
	routing table, for example.

	In general, reduce other system activities to ease memory demands. If
	conditions warrant, upgrade to a larger memory configuration.");
msgdef_ddts_component("");


msgdef(FOREVER, RSP, LOG_ERR, MSG_TRACEBACK,
	     "cmd %u to %s%u took %u usecs, done %x");
msgdef_explanation(
	"A command from the RSP to an IP took longer than expected.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(QAERROR, RSP, LOG_CRIT, 0,
	     "%s error, %s at addr %04x (%s)\n"
	     "  log %08x, data %08x %08x");
msgdef_explanation(
	"A software error was detected during packet switching.");
msgdef_recommended_action(
	"Call your technical support representative for support.");
msgdef_ddts_component("");


msgdef(NOSTART, RSP, LOG_ERR, 0,
	     "No microcode for %s card, slot %u");
msgdef_explanation(
	"No microcode is defined or available for the specified card.");
msgdef_recommended_action(
	"Reconfigure the card to specify an existing microcode file.");
msgdef_ddts_component("");


msgdef(XBUFHDR, RSP, LOG_ERR, MSG_TRACEBACK,
	     "corrupt bufhdr %x: %08x %08x %08x %08x");
msgdef_explanation(
	"Miscommunication occurred between the RSP and an IP.");
msgdef_recommended_action(
	"Report this error to your technical support representative.");
msgdef_ddts_component("");


msgdef(BADHWREV, RSP, LOG_ERR, 0,
	     "%s (slot %u) has wrong hardware revision %u.%u");
msgdef_explanation(
	"The hardware in the indicated slot needs to upgraded for operation
	with the RSP.");
msgdef_recommended_action(
	"Upgrade the board.");
msgdef_ddts_component("");


msgdef(ERROR, RSP, LOG_ERR, 0, NULL);
msgdef_explanation(
	"This message can take many forms. It provides information about a
	software error.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(STALL, RSP, LOG_CRIT, 0,
	     "partially inserted or removed IPs on cyBus%u");
msgdef_explanation(
	"Most likely, a board is not fully seated in the chassis. A less likely
	possibility is a hardware problem with the RSP or backplane.");
msgdef_recommended_action(
	"Try reseating the boards. Call your technical support representative
	for assistance if this message persists.");
msgdef_ddts_component("");


msgdef(IDPROM, RSP, LOG_ERR, 0,
	     "Bad or missing ID EEPROM, controller type %u");
msgdef_explanation(
	"The ID EEPROM on the RSP is missing or bad.");
msgdef_recommended_action(
	"This message indicates a manufacturing error. Report this error to
	your technical support representative.");
msgdef_ddts_component("");


msgdef(NOMAC, RSP, LOG_ERR, 0,
	     "Can't allocate MAC address for interface %u/%u");
msgdef_explanation(
	"No MAC address was available for allocation to the specified
	interface.");
msgdef_recommended_action(
	"Report this error to your technical support representative.");
msgdef_ddts_component("");


msgdef(COOKIE, RSP, LOG_WARNING, 0,
	     "Corrupt or missing MAC address cookie\n"
	     " using random base %e");
msgdef_explanation(
	"This message indicates the part containing the MAC addresses allocated
	to this chassis could not be accessed or was found to be corrupt,
	either due to a hardware problem or manufacturing problem.");
msgdef_recommended_action(
	"Report this error to your technical support representative.");
msgdef_ddts_component("");


msgdef(RESTART, RSP, LOG_ERR, 0, NULL);
msgdef_explanation(
	"The cBus buffer memory was reset and reallocated.");
msgdef_recommended_action(
	"Report this error to your technical support representative, if memory
	is not reset manually (by changing the MTU on an interface, for
	example).");
msgdef_ddts_component("");


msgdef(LOVEGIANT, RSP, LOG_ERR, 0,
	     "Card %s wants %d byte love letters, but only got %d bytes");
msgdef_explanation(
	"An inconsistency between the microcode and the system code was
	detected.");
msgdef_recommended_action(
	"Report this error to your technical support representative.");
msgdef_ddts_component("");


msgdef(LOVENOTE, RSP, LOG_ERR, 0,
	     "%x corrupt: %08x %08x %08x %08x");
msgdef_explanation(
	"This message indicates miscommunication between the RSP and an IP.");
msgdef_recommended_action(
	"Report this error to your technical support representative.");
msgdef_ddts_component("");


msgdef(NOIDB, RSP, LOG_ERR, 0,
	     "bad vc %u on %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(IP_PANIC, RSP, LOG_ERR, 0,
	     "Panic: %s %08x %08x %08x %08x");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(NORESTART, RSP, LOG_ERR, MSG_TRACEBACK,
	     "%s: unable to schedule restart for output queue");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(INVRTN, RSP, LOG_ERR, MSG_TRACEBACK,
	     "Invalid return queue, next=%#x, hwidb=%#x, type=%#x\n queue_ptr=%#x, bufhdr_offset=%#x, id=%d, bcast_id=%d\n bufhdr %x: %08x %08x %08x %08x");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(INVRTNBCASTID, RSP, LOG_ERR, MSG_TRACEBACK,
	     "Invalid return queue bcast_id=%d\n bufhdr %x: %08x %08x %08x %08x");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(INVQPTR, RSP, LOG_ERR, MSG_TRACEBACK,
	     "queue=%#x, bufhdr %x:\n  %08x %08x %08x %08x");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADBUFHDR, RSP, LOG_ERR, MSG_TRACEBACK,
	     "%s, address %x");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(IPC, RSP, LOG_ERR, 0, "%s %s");
msgdef_explanation(
	"An Interprocess Communication (IPC) error occurred. The details about
	what was attempted and what went wrong will be printed.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears. Copy down the router's
	configuration along with any other relevant information. Contact your
	technical support representative for assistance.");
msgdef_ddts_component("");


msgdef(SLAVECHANGE, RSP, LOG_ERR, 0,
	     "Slave changed state from %s to %s");
msgdef_explanation(
	"The slave RSP has undergone a hardware state change. Both the old
	state and new state are shown.

	Possible states are

	nonparticipant

	slave

	master

	unplugged 

	Possible state changes indicated by the above message are

	any to unplugged

	unplugged to nonparticipant or slave

	nonparticipant to slave 

	slave to nonparticipant 

	Any other combination is unexpected.");
msgdef_recommended_action(
	"The slave RSP has been removed. Consider reinstalling it if continued
	high system availability (HSA) operation is required. If the slave RSP
	is present, ensure that it is properly seated in the card cage.

	A slave RSP has been installed. Configure the router for HSA. See the
	Configuration Fundamentals Configuration Guide, in the "Loading System
	Images and Configuration Files" chapter (Cisco 7500 series only) for
	more details. In particular, use the slave sync config command to
	ensure that the new slave RSP is configured consistently with the
	current master RSP.

	CAUTION: Failure to ensure a consistent configuration on a freshly
	installed slave RSP might result in undefined behavior if the router
	reloads.

	A previously crashed slave RSP has been reset, or a newly installed
	slave RSP is in transition from unplugged to nonparticipant, and
	finally to slave state. No action is required.

	The slave RSP image has crashed. Log in to the slave RSP's console
	using the if-console slot command. You will now be connected to the ROM
	monitor prompt on the slave RSP. Diagnose the slave RSP failure. For
	example, capture the output from the stack and context ROM monitor
	commands. Provide that information to your technical support
	representative, along with the router's configuration and any other
	relevant information, so that the problem can be investigated.

	Log out from the slave RSP's console port using Ctrl-c or Ctrl-z, and
	enter the slave reload global configuration command on the master RSP
	to bring the slave RSP back online.

	All other state changes indicate a software or hardware error. Provide
	the router's configuration and any other relevant information to
	customer support so that the problem can be investigated.");
msgdef_ddts_component("");


msgdef(SLAVEUP, RSP, LOG_NOTICE, 0,
	     "Slave changed to state Running");
msgdef_explanation(
	"This is an informational message to indicate that the slave RSP has
	started running the slave RSP image. This message after the router
	boots or after the slave reload global configuration command is issued
	on the master RSP.");
msgdef_recommended_action(
	"No action required.");
msgdef_ddts_component("");


msgdef(MSFIX, RSP, LOG_ERR, 0, 
	     "Fixing %s by setting to default %s");
msgdef_explanation(
	"The characters shown can be either on the slave RSP setting or on the
	master RSP setting.

	The ROM monitor default slave RSP setting is not synchronized with the
	system configuration file specifications. During bootup, the system
	image detects the mismatch and modifies the ROM monitor setting to
	conform with these specifications.

	This mismatch most likely occurred because a new RSP was being
	installed and booted in a high system availability (HSA) environment
	(dual RSPs) for the first time. Alternatively, in established HSA
	configurations, this message may result from a previous Interprocess
	Communication (IPC) error occurring while the HSA environment is
	reconfigured.");
msgdef_recommended_action(
	"Using the show version or show boot command, determine which slot
	contains the current slave RSP.

	Using the show config command, determine which slot has been specified
	as the default slave RSP. If there is no slave default-slot n command
	in the configuration file, the value defaults to the highest numbered
	CPU slot (slot 3 on a Cisco 7507, and slot 7 on a Cisco 7513).

	If the slot number obtained in Step 1 is the same as the number
	obtained in Step 2, then no further action is required. You have
	configured HSA in simple hardware backup mode, and both RSPs are
	identical.

	However, if the two slot numbers differ, you have configured HSA in
	software error protection mode, and will have different images running,
	depending on which RSP is currently the slave RSP. If the slot numbers
	discovered in Steps 1 and 2 differ, reload your router to ensure that
	the desired system image is running. After the reload, the actual slave
	RSP will match the default slave RSP specified in (or inferred from)
	the system configuration file.

	For details of various HSA configuration modes consult the Cisco
	IOSConfiguration Fundamentals Configuration Guide. See the "Loading
	System Images and Configuration Files" chapter (Cisco 7500 series
	only).");
msgdef_ddts_component("");

msgdef(MSVERS, RSP, LOG_ERR, 0, 
	     "Master has m/s version %d, slave has m/s version %d");
msgdef_explanation(
	"The master and slave are running incompatible software versions with
	regards to the master/slave exchange process.");
msgdef_recommended_action(
	"If the slave image global configuration is used to override the
	default slave image from the bundle, then the slave image is
	incompatible. Update either the master or slave image to ensure the two
	are compatible.

	If the slave is running the image from the bundle, execute and record
	the output of show version, show running-config, and show controller
	cbus commands. Report this information and the error message to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(MSDOG, RSP, LOG_ERR, 0,
	     "Master/slave watchdog timed out");
msgdef_explanation(
	"The system software failed to reset the master/slave watchdog timer,
	causing it to time out. This behavior could indicate a software problem
	or a hardware problem.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears. Copy down the router's
	configuration along with any other relevant information. Contact your
	technical support representative for assistance.");
msgdef_ddts_component("");


msgdef(SLAVEMASTER, RSP, LOG_ERR, 0,
	     "Slave stole mastership");
msgdef_explanation(
	"The master RSP detected that the slave RSP had taken mastership of the
	router. The old master RSP will reload and become the slave RSP,
	letting the new master RSP take over. This behavior indicates a
	software or hardware error.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears. Copy down the router's
	configuration along with any other relevant information. Contact your
	technical support representative for assistance.");
msgdef_ddts_component("");
 

msgdef(HSA_MEM, RSP, LOG_WARNING, 0,
             "size mismatch, master %uMB, slave %uMB");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(HSA_MINMEM, RSP, LOG_WARNING, 0,
             "%uMB required for HSA");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");




/*
 *	Error messages used by IPC RSP CBus driver.
 *	XXX. We may have to move these to different files.
 */
facdef(IPC_RSP_CBUS);
msgdef_section("Interprocess Communication error messages");


msgdef(INVNUM, IPC_RSP_CBUS, LOG_ERR, 0, "%s (%d)");
msgdef_explanation(
	"The IPC application tried to send a message that was larger than IPC
	could handle. The message was not transmitted.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(SEATERR, IPC_RSP_CBUS, LOG_ERR, 0, "%s (%d)");
msgdef_explanation(
	"The ID associated with an IPC card is either missing, duplicated, or
	cannot be created.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(CARDERR, IPC_RSP_CBUS, LOG_ERR, 0, "%s");
msgdef_explanation(
	"An IPC card has registered itself to the IPC master twice. The card
	malfunctioned and its IPC is not active.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(MSGERR, IPC_RSP_CBUS, LOG_ERR, 0, "%s (%d)");
msgdef_explanation(
	"The slave IPC cannot register with master. It cannot exchange messages
	with the master.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(NOHWQ, IPC_RSP_CBUS, LOG_ERR, 0,
	"Hardware queue for card at slot %d not found");
msgdef_explanation(
	"IPC messages cannot be transmitted due to a missing hardware queue.
	The messages were discarded, and subsequent messages will not be
	transmitted to that card.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(NOBUF, IPC_RSP_CBUS, LOG_ERR, 0, "%s", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"The IPC ran out of memory device (MEMD) buffers.");
msgdef_recommended_action(
	"Reduce the number of applications currently running on the IPC. Copy
	the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(QADIAG, RSP, LOG_CRIT, 0,
	     "QA Diagnostic %s error at %#08x");
msgdef_explanation(
	"An error occured while accessing RSP internal hardware.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");

msgdef(ACCERROR, RSP, LOG_ERR, 0,
       "%s acc %08x had bad value %d");
msgdef_explanation(
	"A wrong value has been written on an accumulator.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative. A micro reload can correct the error
        condition as a workaround.");
msgdef_ddts_component("");

msgdef(RSPDRAM, RSP, LOG_WARNING, 0,
             "Insufficient DRAM to adequately run this IOS version.  %dM is recommended.");
msgdef_explanation("The quantity of RSP DRAM is below the recommended amount.");
msgdef_recommended_action("Add DRAM.")
msgdef_ddts_component("");
