/* $Id: msg_cbus.c,v 3.3.60.5 1996/09/13 23:46:25 hampton Exp $
 * $Source: /release/112/cvs/Xsys/hes/msg_cbus.c,v $
 *------------------------------------------------------------------
 * msg_cbus.c - Message file for cbus facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_cbus.c,v $
 * Revision 3.3.60.5  1996/09/13  23:46:25  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.60.4  1996/08/28  12:45:31  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.60.3  1996/07/23  13:22:59  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.3.60.2  1996/07/08  22:25:42  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.3.60.1  1996/06/06  18:17:49  motto
 * CSCdi58517:  CSNA Virtual Port x/2 hung in down/down state after
 * noShut/micro rel
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/17  09:19:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:29:07  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:41:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/11  10:23:53  kmitchel
 * CSCdi25909:  Erroneous CIP configuration commands are still added to
 *              config file
 *
 * Revision 2.3  1995/07/17  08:21:21  schaefer
 * CSCdi36181: more robust microcode management (on RSP)
 *
 * Revision 2.2  1995/06/18  23:17:36  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:42:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(CBUS);
msgdef_section("ciscoBus controller error messages");


msgdef(RSETFAIL, CBUS, LOG_WARNING, 0,
       "Interface %s failed to reset properly in %s, code %#x");
msgdef_explanation(
	"An interface reset command failed. This may be a software or hardware
	problem.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(OUTHUNG, CBUS, LOG_ERR, 0,
       "%s: tx%c output hung (%-04x - %s), %s");
msgdef_explanation(
	"A transmission attempt on an interface failed. The interface might not
	be attached to a cable or there might be a software problem.");
msgdef_recommended_action(
	"Check that the interfaces are all connected to the proper cables. If
	that is not the problem, call your technical support representative.");
msgdef_ddts_component("");


msgdef(NOCOOKIE, CBUS, LOG_WARNING, 0,
       "%s-%d controller missing configuration data - disabled");
msgdef_explanation(
	"The controller type was derived from a hardware probe. The controller
	number indicates the ciscoBus relative slot number. This is a hardware
	error");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(TESTFAIL, CBUS, LOG_ERR, 0,
       "Unit %d, failed %s test - interface disabled");
msgdef_explanation(
	"A hardware component failed an internal diagnostic test.");
msgdef_recommended_action(
	"Check switch settings on the interface cards. Check for proper unit
	numbers. Reset the cards. Replace the malfunctioning device.");
msgdef_ddts_component("");


msgdef(DAUGHTER, CBUS, LOG_ERR, 0,
       "Unit %d, daughter controller %d failed %s test - interface disabled");
msgdef_explanation(
	"A hardware component failed an internal diagnostic test. Test output
	and descriptions of ranges follow.

	Unit [dec] (ranges 0-14): Indicates the Multibus jumper setting for the
	ciscoBus controller, or in the Cisco 7000, a slot number.

	daughter controller [dec] (ranges 0-3): Indicates the ciscoBus relative
	slot number (0 to 3, or in the Cisco 7000, 0 to 4) of the daughter card
	that failed the test.

	failed [chars] test ([chars]= memd read, memd write):

	If the failed test was memd read, the processor was able to write to shared ciscoBus
	buffer memory from the ciscoBus registers, but was unable to read back
	that memory through special commands to the daughter card.

	If the failed test was memd write, writes to the memory device through
	the daughter card were not verified successfully with reads through the
	ciscoBus controller.

	The daughter card under test responds successfully to some commands
	(otherwise the software would have received a DAUGHTER_NO_RSP message),
	but its memory device tests failed. The daughter card must be able to
	write to some memory device addresses; otherwise, it cannot respond to
	any commands.");
msgdef_recommended_action(
	"First check that the daughter controller card is properly seated in
	its Multibus and ciscoBus connectors. Next, swap daughter cards and
	check whether the error follows the daughter card or the slot. If the
	message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(DAUGHTER_NO_RSP, CBUS, LOG_ERR, 0,
       "Unit %d, daughter %d, not responding (0x%04x) - disabled");
msgdef_explanation(
	"A hardware component failed an internal diagnostic test. Test output
	and descriptions of ranges follow.

	Unit [dec] (ranges 0-14): Indicates the Multibus jumper setting for the
	ciscoBus controller.

	daughter controller [dec] (ranges 0-3): Indicates the ciscoBus relative
	slot number (0 to 3, or in the Cisco 7000, 0 to 4) of the daughter card
	that failed the test.

	The daughter card's hardware was recognized across the ciscoBus
	backplane, but the daughter card did not respond to the first command
	issued to it.

	This condition is a result of one of the following: the daughter card
	did not reset, or the microcode on the card is in a hung state, or the
	card cannot fully communicate across the ciscoBus backplane (cannot
	read or write shared MEMD or does not get the MALU_ATTENTION signal
	across the ciscoBus).");
msgdef_recommended_action(
	"First, make sure that the daughter card is properly seated in its
	Multibus and ciscoBus connectors. If this action does not solve the
	problem, call your technical support representative.");
msgdef_ddts_component("");


msgdef(CORRUPT, CBUS, LOG_ERR, 0,
       "Controller %d, wrote 0x%-04x, read 0x%-04x, loc 0x%-04x - dci_memtest()");
msgdef_explanation(
	"A hardware component failed an internal diagnostic test.");
msgdef_recommended_action(
	"Replace the malfunctioning device.");
msgdef_ddts_component("");


msgdef(TXALLOC, CBUS, LOG_ERR, 0,
       "Error (%-04x) tx_allocate - cbus_init()");
msgdef_explanation(
	"A hardware device did not respond appropriately to a request.");
msgdef_recommended_action(
	"Make sure the device is functioning and is configured correctly.");
msgdef_ddts_component("");


msgdef(BUFFER, CBUS, LOG_ERR, 0,
       "Controller %d, Error (%-04x), Buffersize = %d, Bufferpool = %d, number %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(INITERR, CBUS, LOG_ERR, 0,
       "Interface %d, Error (%-04x), idb %08x %d %s - cbus_init()");
msgdef_explanation(
	"The switch processor or ciscoBus controller signaled an error while
	processing a packet or selecting an interface. This indicates a
	software problem.");
msgdef_recommended_action(
	"Call your technical support representative.");
msgdef_ddts_component("");


msgdef(INTR, CBUS, LOG_ERR, 0,
       "Interface %d, idb %08x %d %s - cbus_interrupt()");
msgdef_explanation(
	"The switch processor or ciscoBus controller returned a nonsensical
	value.");
msgdef_recommended_action(
	"Call your technical support representative.");
msgdef_ddts_component("");


msgdef(CBUSBCE, CBUS, LOG_ERR, 0,
       "Failed to select a BCE, response 0x%04x - cbus_bce_update_ifa()");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(CBUSBCEPERM, CBUS, LOG_ERR, 0,
       "Failed to select a BCE, response 0x%04x - cbus_bce_permissions_update()");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(FDDIRSET, CBUS, LOG_ERR, 0,
       "Interface %s, Error (%-04x) %s - fddi_reset()");
msgdef_explanation(
	"A hardware device did not respond appropriately to a request.");
msgdef_recommended_action(
	"Make sure the device is functioning and is configured correctly.");
msgdef_ddts_component("");


msgdef(FDDIRSETU, CBUS, LOG_ERR, 0,
       "Unit %d, Error (%-04x) %s - fddi_reset()");
msgdef_explanation(
	"A hardware device did not respond appropriately to a request.");
msgdef_recommended_action(
	"Make sure the device is functioning and is configured correctly.");
msgdef_ddts_component("");


msgdef(HSSIRSET, CBUS, LOG_ERR, 0,
       "Interface %s, Error (%-04x) %s - hssi_reset()");
msgdef_explanation(
	"A hardware component did not respond to a reset command.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(CTRRAMTEST, CBUS, LOG_ERR, 0,
       "Interface %s, RAM Addressing Test Failed - %s");
msgdef_explanation(
	"The Token Ring interface failed its memory diagnostic tests.");
msgdef_recommended_action(
	"Issue a clear interface command. If this message recurs, call your
	technical support representative for assistance.");
msgdef_ddts_component("");
msgdef_tac_details(
        "This message indicates a hardware fault on the token ring
         interface card.  Replace the failing interface card.");


msgdef(CTRINIT, CBUS, LOG_ERR, 0,
       "Interface %s, CTR Initialization Failed - %s");
msgdef_explanation(
	"The Token Ring interface failed one of its internal self-tests.");
msgdef_recommended_action(
	"Issue a clear interface command. If this message recurs, call your
	technical support representative for assistance.");
msgdef_ddts_component("");
msgdef_tac_details(
        "This message indicates a hardware fault on the token ring
         interface card.  Replace the failing interface card.");


msgdef(CTRCMDFAILED, CBUS, LOG_ERR, 0,
       "Interface %s, CTR Command %s Failed, %s (%#04x)");
msgdef_explanation(
	"A command from the system to the Token Ring interface failed to
	complete.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("high-token");
msgdef_tac_details(
        "The specified CTR command was attempted by the CBUS controller
         but failed to complete.  The token ring interface processor
         error message is reported after the word 'Failed'.  The CBUS
         error or lovenote code is reported in parenthesis.
         Contact Development Engineering and provide the exact text
         of the error message.  Also provide output from 
         'debug token event' if possible.");

msgdef(CTRCMDFAILED2, CBUS, LOG_ERR, 0,
       "Interface %s, CTR Command %s Failed, Code %#04x");
msgdef_explanation(
	"A command from the system to the Token Ring interface failed to
         complete.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("high-token");
msgdef_tac_details(
        "The specified CTR command was attempted by the CBUS controller
         but failed to complete.  The CBUS error code is reported after
         the word 'Code'.  Contact Development Engineering and provide
         the exact text of the error message.  Also provide output from
         'debug token event' if possible.");


msgdef(CTRRSET, CBUS, LOG_ERR, 0,
       "Interface %s failed to initialize");
msgdef_explanation(
	"The Token Ring interface failed to insert into the token ring.
	The interface is placed into 'reset' state and will not 
        automatically try to re-insert.");
msgdef_recommended_action(
	"Issue a clear interface command. If this message recurs, call your
	technical support representative for assistance.");
msgdef_ddts_component("high-token");
msgdef_tac_details(
        "This message appears if a token ring interface stays in
         'initializing' state for more than 90 seconds without
         successfully inserting into the token ring.  Collect any
         other error messages that may indicate why the initialization
         is failing.  If no other error messages appear, turn on
         'debug token event' and issue a clear interface command.
         Contact Development Engineering with the output of the
         debug and any other error messages.");


msgdef(CTRACHECK, CBUS, LOG_ERR, 0,
       "Interface %s, Adapter Check Error (%04x %04x %04x %04x)");
msgdef_explanation(
	"The Token Ring monitor firmware detected a fatal error on the
	interface card.");
msgdef_recommended_action(
	"Issue a clear interface command. If this message recurs, call your
	technical support representative for assistance.");
msgdef_ddts_component("high-token");
msgdef_tac_details(
        "This message may indicate a hardware failure.  Replace the
         token ring interface card first.  If the error continues,
         contact Development Engineering with the exact text of the
         message.");


msgdef(CTRUCHECK, CBUS, LOG_ERR, 0,
       "Interface %s, Microcode Check Error");
msgdef_explanation(
	"The token ring interface microcode detected an unrecoverable error.");
msgdef_recommended_action(
	"Issue a clear interface command. If this message recurs, call your
	technical support representative for assistance.");
msgdef_ddts_component("high-token");
msgdef_tac_details(
        "Contact Development Engineering with the exact text of the 
         message.  A MEMB dump may also be required.  If possible,
         make arrangements with the customer to get dial-up or telnet
         access to the affected router to collect the MEMB dump.");


msgdef(CTRBADLOVE1, CBUS, LOG_ERR, 0,
       "Interface %s, SRB_COMPLETE lovenote received with unknown command (0x%04x)");
msgdef_explanation(
       "The system received a message from the token ring interface
        but does not recognize the message.");
msgdef_recommended_action(
       "Copy the exact text of the error message and contact your 
        technical support representative for assistance.");
msgdef_ddts_component("high-token");
msgdef_tac_details(
       "A CBUS lovenote of type SRB_COMPLETE was received from the token
        ring interface but the command embedded in the lovenote is
        invalid.  A brief MEMB dump will follow this message.  Copy
        the exact text of the message and the dump and contact
        Development Engineering.");


msgdef(CTRBADLOVE1_DUMP, CBUS, LOG_ERR, 0,
       "Dump of MEMB follows\n-SRB_ADDR:  %04x %04x %04x %04x %04x %04x %04x %04x");
msgdef_explanation(
       "The system is printing codes related to a previous lovenote error
        message.");
msgdef_recommended_action(
       "Copy the exact text of the error message and contact your
        technical support representative for assistance.");
msgdef_ddts_component("high-token");
msgdef_tac_details(
       "Copy the exact text of the message and contact Development
        Engineering.");

msgdef(CTRBADLOVE2, CBUS, LOG_WARNING, 0,
       "Interface %s, Unexpected SRB_COMPLETE lovenote, command=0x%04x, result=0x%04x");
msgdef_explanation(
       "The system received an unsolicited  message from the token ring interface.
        The system ignored the message and continued normal processing.");
msgdef_recommended_action(
       "If the message recurrs repeatedly, copy the exact text of the
        message and contact your technical support representative for 
        assistance.");
msgdef_ddts_component("high-token");
msgdef_tac_details(
       "A CBUS lovenote of type SRB_COMPLETE was received from the 
        token ring interface but there are no outstanding SRB commands
        waiting for completion.  The system ignores the lovenote and
        continues.  This may indicate a non-critical software bug.
        If this message recurrs, copy the exact text and contact
        Development Engineering.");

 
msgdef(RESETNXI, CBUS, LOG_ERR, MSG_TRACEBACK|MSG_PROCESS,
       "Reset of removed interface %x (%s)");
msgdef_explanation(
	"An interface reset was attempted on an interface that was removed from
	the router. This message appears if you use the shutdown command to
	bring down a removed interface.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");


msgdef(FSIPRSET, CBUS, LOG_ERR, 0,
       "Interface %s, Error (%-04x) %s - fsip_reset()");
msgdef_explanation(
	"A Fast Serial Interface Processor (FSIP) did not respond to a reset
	request from the route processor.");
msgdef_recommended_action(
	"Check FSIP electrical connections, cable, and ciscoBus connections. An
	FSIP microcode reload will be required. If the check of electrical
	connections reveals no problems and the message recurs, call your
	technical support representative for assistance.");
msgdef_ddts_component("");


msgdef(AIPRSET, CBUS, LOG_ERR, 0,
       "Interface %s, Error (%-04x) %s - aip_reset()");
msgdef_explanation(
	"A software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(AIPRSETU, CBUS, LOG_ERR, 0,
       "Unit %d, Error (%-04x) %s - aip_reset()");
msgdef_explanation(
	"A software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(AIPCFG, CBUS, LOG_ERR, 0, 
       "Interface %s, Cannot config interface, AIP MEM 0x%x, status %d");
msgdef_explanation(
	"A software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(AIPINVPLIM, CBUS, LOG_ERR, 0,
       "Interface %s, Invalid PLIM detected.  Interface downed");
msgdef_explanation(
	"A software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(CIPRSET, CBUS, LOG_ERR, 0,
       "Interface %s, Error (%-04x) %s - cip_reset()");
msgdef_explanation(
	"The Channel Interface Processor (CIP) did not respond to a reset request from
	the route processor.");
msgdef_recommended_action(
	"Try each of the following fixes in sequence. If a fix corrects the
	problem, there's no need to perform the tasks which follow it.

	1) Clear the interface using the clear interface command
	2) Mic reload
	3) Reseat the CIP

	If the error still occurs, copy the error message exactly as it
	appears. Record the output from the following commands:

	show contr cbus
	show version
	show interface
	write term
	show running-config

	Record any of the other log messages that surround the CIPRSET message.
	Provide this information to your technical support representative.");
msgdef_ddts_component("");

msgdef_limit(BADVC, CBUS, LOG_ERR, 0,
	     "%s %d/%d got bad VC packet \n%08x %08x %08x %08x %08x %08x %08x %08x",
	     1 * ONEMIN);
msgdef_explanation(
	"An interface received packet with bad VC encapsulation. 
	This can indicate a software or hardware problem.");
msgdef_recommended_action(
	"Copy the debugging information exactly as it appears and report it to
	your technical support representative.");
msgdef_ddts_component("");

msgdef(POLLFAIL, CBUS, LOG_ERR, 0,
       "Interface %s failed to respond, debug info follows");
msgdef_explanation(
	"A Token Ring interface card failed to respond to periodic polling.
	This can indicate a software or hardware problem.");
msgdef_recommended_action(
	"Copy the debugging information exactly as it appears and report it to
	your technical support representative. See CBUS-3-POLLFAILDAT for the
	syntax of this message.");
msgdef_ddts_component("");


msgdef(POLLFAILDAT, CBUS, LOG_ERR, 0,
       "Int %d: %04X %04X %04X %04X %04X %04X");
msgdef_explanation(
	"This is the debugging information for the CBUS-3-POLLFAIL error.");
msgdef_recommended_action(
	"Copy the debugging information exactly as it appears and report it to
	your technical support representative.");
msgdef_ddts_component("");


msgdef(CIPADPTCHNG, CBUS, LOG_WARNING, 0,
       "Interface %s, adapter changed from %s to %s");
msgdef_explanation(
	"The port adapter on the CIP interface reported a different interface
	type than it previously did, which should not happen without your
	removing the CIP and reinstalling it.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(CIPDTRFAIL, CBUS, LOG_ERR, 0,
       "%s: Channel-protocol %s command failed; resetting to default.");
msgdef_explanation(
	"The channel-protocol configuration command sent to the CIP after a
	reset failed to complete successfully.");
msgdef_recommended_action(
	"The channel data transfer rate was reset to the default value. Reissue
	the configuration command.");
msgdef_ddts_component("");


msgdef(CIPCFGFAIL, CBUS, LOG_ERR, 0,
       "%s: configuration command %s %s failed");
msgdef_explanation(
	"A configuration command sent to the CIP after a reset failed to
	complete successfully.");
msgdef_recommended_action(
	"Reissue the configuration command.");
msgdef_ddts_component("");


msgdef(CIPNOPREQ, CBUS, LOG_ERR, 0,
       "%s: Unable to pre allocate response element; cfg not sent.");
msgdef_explanation(
	"An attempt to allocate a buffer for the CIP configuration response, or
	for the configuration state change request, was unsuccessful. The
	configuration request was not sent to the CIP.");
msgdef_recommended_action(
	"The configuration command must be reissued.");
msgdef_ddts_component("");


msgdef(CIPENABLE, CBUS, LOG_ERR, 0,
       "%s: No pre-allocated buffers available; enable not sent.");
msgdef_explanation(
	"An attempt to get a pre-allocated buffer for the CIP enable command
	was unsuccessful. This CIP interface will not be enabled.");
msgdef_recommended_action(
	"Reload the router to get the CIP interface enabled again. Report this
	error to your technical support representative.");
msgdef_ddts_component("");


msgdef(CIPENCAP, CBUS, LOG_ERR, 0,
       "%s: No elements available for message (%08X %08X); resetting interface.");
msgdef_explanation(
	"An encapsulated control message arrived from the CIP, but no elements
	were available to receive it. The Route Processor (RP) or the RSP will
	reset the interface.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears and copy down the
	router's configuration along with any other relevant information.
	Contact your technical support representative for assistance.");
msgdef_ddts_component("");


msgdef(BADENCAPSZ, CBUS, LOG_WARNING, 0,
       "%s: CIP encapsulated msg size (%d) exceeds max (%d); truncated.");
msgdef_explanation(
	"An encapsulated control message from the CIP had a size greater than
	the maximum expected size. The message was truncated.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears and copy down the
	router's configuration along with any other relevant information.
	Contact your technical support representative for assistance.");
msgdef_ddts_component("");


msgdef_limit(FCICMDFAIL0, CBUS, LOG_ERR, 0,
       "Controller %d, cmd (%d) failed (0x%-04x) count (%d)", 
		   MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"A command sent from the system to an interface processor failed to
	complete successfully.");
msgdef_recommended_action(
	"The system recovered by generating an error code to the requester");
msgdef_ddts_component("");


msgdef_limit(FCICMDFAIL1, CBUS, LOG_ERR, 0,
       "Controller %d, cmd (%d 0x%-08x) failed (0x%-04x) count (%d)", 
		   MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"A command sent from the system to an interface processor failed to
	complete successfully.");
msgdef_recommended_action(
	"The system recovered by generating an error code to the requester.");
msgdef_ddts_component("");


msgdef_limit(CTRLRCMDFAIL0, CBUS, LOG_ERR, MSG_TRACEBACK,
       "Controller %d, cmd (%d) failed (0x%-04x) count (%d)", 
		   MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"A command sent from the system to an interface processor failed to
	complete successfully.");
msgdef_recommended_action(
	"The system recovered by generating an error code to the requester.");
msgdef_ddts_component("");


msgdef_limit(CTRLRCMDFAIL1, CBUS, LOG_ERR, MSG_TRACEBACK,
       "Controller %d, cmd (%d 0x%-08x) failed (0x%-04x) count (%d)", 
		   MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"A command sent from the system to an interface processor failed to
	complete successfully.");
msgdef_recommended_action(
	"The system recovered by generating an error code to the requester.");
msgdef_ddts_component("");


msgdef(CCBCMDFAIL0, CBUS, LOG_ERR, 0,
       "Controller %d, cmd (%d) failed (0x%-04x)");
msgdef_explanation(
	"A command sent from the system to an interface processor failed to
	complete successfully.");
msgdef_recommended_action(
	"The system recovered by generating an error code to the requester.");
msgdef_ddts_component("");


msgdef(CCBCMDFAIL1, CBUS, LOG_ERR, 0,
       "Controller %d, cmd (%d 0x%-08x) failed (0x%-04x)");
msgdef_explanation(
	"A command sent from the system to an interface processor failed to
	complete successfully.");
msgdef_recommended_action(
	"The system recovered by generating an error code to the requester.");
msgdef_ddts_component("");


msgdef_limit(UNKENC, CBUS, LOG_ERR, 0,
	     "Interface %s, unknown encaps type %x",
	     15*ONESEC);
msgdef_explanation(
	"A packet was received from the CIP with an unknown encapsulation type.
	The packet will be dropped.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


#ifdef RSP

msgdef(CMDTIMEOUT, CBUS, LOG_ERR, MSG_TRACEBACK,
       "Cmd timed out, CCB 0x%x, slot %d, cmd code %d");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(CMDDROPPED, CBUS, LOG_ERR, MSG_TRACEBACK,
	"Cmd dropped, CCB 0x%x, slot %d, cmd code %d");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(CCBPTIMEOUT, CBUS, LOG_ERR, MSG_TRACEBACK,
       "CCB handover timed out, CCB 0x%x, slot %d");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(PORTTYPE, CBUS, LOG_ERR, MSG_TRACEBACK,
       "Unexpected interface type for %s%u, port %u, type %u");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(CMD, CBUS, LOG_ERR, MSG_TRACEBACK,
       "Cmd failed: %s, response %#04x, %s");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(MODULE, CBUS, LOG_ERR, 0,
       "Missing %s for %s");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(NOBUF,  CBUS, LOG_ERR, MSG_TRACEBACK,
       "Buffer allocation failure: %s");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");

#endif
