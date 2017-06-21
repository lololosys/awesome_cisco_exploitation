/* $Id: msg_dbus.c,v 3.5.4.3 1996/09/13 23:46:30 hampton Exp $
 * $Source: /release/112/cvs/Xsys/hes/msg_dbus.c,v $
 *------------------------------------------------------------------ 
 *  msg_dbus.c - Message file for dbus facility
 *
 *  William H. Palmer, Tue Oct  6 19:58:30 1992  
 *
 *  Copyright (c) 1992-1997 by cisco Systems, Inc.  
 *  All rights reserved.  
 *------------------------------------------------------------------
 * $Log: msg_dbus.c,v $
 * Revision 3.5.4.3  1996/09/13  23:46:30  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.5.4.2  1996/08/28  12:45:37  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.4.1  1996/04/06  19:28:27  foster
 * CSCdi52285:  OIR causes HSA system to panic with %DBUS-3-BADHSACONF
 * Branch: California_branch
 *
 * Revision 3.5  1996/03/07  05:56:10  dbath
 * CSCdi50895:  HSA master sometimes hangs on dbus flow control
 * If while downloading the slave, he gets seriously wedged, don't
 * panic.  Log a message and keep trucking, otherwise neither of
 * them will route packets.
 *
 * Revision 3.4  1996/01/15  18:04:14  schaefer
 * CSCdi45761: support RSP in new chassis
 *
 * Revision 3.3  1995/12/28  08:18:49  foster
 * CSCdi46157:  Dbus down-load needs to work on rsp2 slave down-load
 *
 * Revision 3.2  1995/11/17  09:19:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:41:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:43:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(DBUS);				/* Define the DBUS facility */
msgdef_section("Data bus error messages");


msgdef(CXBUSERR, DBUS, LOG_ERR, 0,
       "Slot %d, CBus Error");
msgdef_explanation(
	"A processor reported a bus error. The processor will be reset and not
	used until the next on-line insertion and removal (OIR) event or
	microcode download, when it will be retested.");
msgdef_recommended_action(
	"Reinstall the processor or download the microcode. If this message
	recurs, call your technical support representative for assistance.");
msgdef_ddts_component("");


msgdef(DBUSINTERR, DBUS, LOG_ERR, 0,
       "Slot %d, Internal Error");
msgdef_explanation(
	"A processor reported an internal error. The processor will be reset
	and not used until the next OIR event or microcode download, when it
	will be retested.");
msgdef_recommended_action(
	"Reinstall the processor or download the microcode. If this message
	recurs, call your technical support representative for assistance.
	Currently no boards report this error.");
msgdef_ddts_component("");


msgdef(WCSCTRLRERR, DBUS, LOG_ERR, 0,
       "Slot %d, WCS Controller Instruction Error");
msgdef_explanation(
	"The writable control store (WCS) is the set of devices on each
	interface processor that contains the running microcode. It can be
	loaded either from the ROM on the interface processor or from the
	system by using the downloadable microcode facility. In this message, a
	processor reported a WCS controller instruction error. The processor
	will be reset and not used until the next OIR event or microcode
	download, when it will be retested.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(WCSPARERR, DBUS, LOG_ERR, 0,
       "Slot %d, WCS Controller Parity Error");
msgdef_explanation(
	"A processor reported a WCS parity error. The processor will be reset
	and not used until the next OIR event or microcode download, when it
	will be retested.");
msgdef_recommended_action(
	"Replace the processor or download microcode. If this message recurs,
	call your technical support representative for assistance.");
msgdef_ddts_component("");


msgdef(DAUGHTER_STATS, DBUS, LOG_ERR, 0,
       "Slot %d control 0x%x, status 0x%x");
msgdef_explanation(
	"A processor reported an error. This message is sent out in conjunction
	with other error messages to provide additional information.");
msgdef_recommended_action(
	"Call your technical support representative and report information
	about all error messages displayed.");
msgdef_ddts_component("");


msgdef(SLOTCOMP, DBUS, LOG_ERR, 0,
       "Slot %d, dbus error, slot (0x%x) and complement (0x%x) do not match");
msgdef_explanation(
	"A processor reported a mismatch between the contents of the slot
	register and its complement. This condition indicates a hardware
	failure in the diagnostic bus controller on that processor or on the
	diagnostic bus. The processor will be reset and not used until the next
	OIR event or microcode download, when it will be retested.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(WCSERR, DBUS, LOG_ERR, 0,
       "Slot %d, WCS controller in error state (0x%x), resetting");
msgdef_explanation(
	"The processor in the slot identified in the error message reported a
	writable control store (WCS) controller error. The processor will be
	reset and new microcode loaded. This error message appears only when
	microcode is downloaded.");
msgdef_recommended_action(
	"If this message recurs after the processor is reset and new microcode
	is loaded, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(WCSLDERR, DBUS, LOG_ERR, 0,
       "Slot %d, error loading WCS, status 0x%x cmd/data 0x%x pos %d");
msgdef_explanation(
	"A processor indicated an error condition while downloading microcode.
	The status, cmd/data, and pos numbers shown in the message reflect the
	error condition, the last byte of microcode loaded, and the position in
	the file, respectively. If this error appears, the microcode download
	is aborted and the ROM microcode is loaded.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(WCSFLOW, DBUS, LOG_ERR, 0,
       "Slot %d, flow cntrl error loading WCS, last data 0x%x pos %d");
msgdef_explanation(
	"A processor indicated an error condition while downloading microcode.
	The status, cmd/data, and pos numbers shown in the message reflect the
	error condition, the last byte of microcode loaded, and the position in
	the file, respectively. If this error appears, the microcode download
	is aborted and the ROM microcode is loaded.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(UNKNOWN, DBUS, LOG_ERR, 0,
       "Slot %d, unknown controller type 0x%x, ignoring");
msgdef_explanation(
	"A processor reported a controller type that is unknown to the system.
	The processor will not be used and needs to be repaired.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(XILINX, DBUS, LOG_ERR, 0, "ready wait timeout, slot %u");
msgdef_explanation(
	"The field-programmable gate arrays on an interface processor failed to
	load after being reset. This indicates a probable software or hardware
	problem.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(NOMAGIC, DBUS, LOG_ERR, 0,
       "No magic number found in RP1 EEPROM");
msgdef_explanation(
	"The structure containing the system MAC address needed by the OIR
	facility cannot be found in the RP EEPROM. The RP must be returned for
	reprogramming.");
msgdef_recommended_action(
	"Return the RP for reprogramming and call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(BADEEPROM, DBUS, LOG_ERR, 0,
       "Invalid EEPROM data found for card in slot %d, card disabled");
msgdef_explanation(
	"The ID EEPROM on the indicated card is not programmed correctly.");
msgdef_recommended_action(
	"Call your technical support representative for a replacement.");
msgdef_ddts_component("");


msgdef(SW_NOTRDY, DBUS, LOG_ERR, MSG_TRACEBACK,
       "DBUS software not ready after %s, elapsed %u, status 0x%x");
msgdef_explanation(
	"An interface processor failed to come online after being reset. A
	software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADCTRLRTYPE, DBUS, LOG_ERR, MSG_TRACEBACK,
       "Invalid controller type %d in slot %d");
msgdef_explanation(
	"An interface processor responded with an unknown or illegal controller
	type when queried. This should not happen unless the running software
	does not support the interface processor in question, or the interface
	processor fails.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(NORESP, DBUS, LOG_ERR, MSG_TRACEBACK,
       "bad or no response, slot %u");
msgdef_explanation(
	"A board in the slot identified in the error message failed to
	respond.");
msgdef_recommended_action(
	"Record the output of the following commands:

	show version
	show controllers cbus
	show diag
	show interface
	show running config

	Report this information to your technical support representative.");
msgdef_ddts_component("");


msgdef(BADHSACONF, DBUS, LOG_ERR, 0,
      "Down rev RSP EPROM found HSA system, upgrade EPROM, or remove one RSP");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");

msgdef(NOSUPPORT, DBUS, LOG_ERR, 0,
       "No driver support in this image for %s in slot %d (card type %d) - disabled");
msgdef_explanation(
       "The Cisco IOS image does not have driver support code
       for the specified interface processor.");
msgdef_recommended_action(
       "Remove the unsupported interface processor card, or
       reload the router with a Cisco IOS image that has driver
       support code for that interface processor card.");
msgdef_ddts_component("");
msgdef_tac_details(
       "Have the customer issue the show version command
       to determine which feature set they are running.");

