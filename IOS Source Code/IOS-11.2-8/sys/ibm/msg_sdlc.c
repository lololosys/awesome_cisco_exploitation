/* $Id: msg_sdlc.c,v 3.3.58.4 1996/09/13 23:49:11 hampton Exp $
 * $Source: /release/112/cvs/Xsys/ibm/msg_sdlc.c,v $
 *------------------------------------------------------------------
 * msg_sdlc.c - Message file for SDLC facility
 *
 * August, 1991, Joel P. Bion
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_sdlc.c,v $
 * Revision 3.3.58.4  1996/09/13  23:49:11  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.58.3  1996/08/28  12:46:53  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.58.2  1996/07/23  13:23:18  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.3.58.1  1996/05/17  11:20:43  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.46.2  1996/04/11  14:37:47  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.46.1  1996/04/03  14:34:05  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3  1995/11/17  09:23:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:35:37  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:45:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:47:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(SDLC);
msgdef_section("Synchronous Data Link Control error messages");


msgdef_limit(DROPPED, SDLC, LOG_WARNING, 0,
       "Dropped SDLC packet from SDLC hold queue", 10);
msgdef_explanation(
	"An SDLC frame had to be dropped from the output hold queue, usually
	because of excessive congestion on the output link.");
msgdef_recommended_action(
	"If this message occurs frequently, determine why your SDLC link is
	being overloaded with data and resolve the cause (typically by either
	increasing bandwidth to your SDLC line or reducing the load on your
	link).");
msgdef_ddts_component("");


msgdef_limit(NULLPAK, SDLC, LOG_ERR, 0,
       "Interface %s, NULL packet ptr, rvr %d, vs %d, vr %d", 10);
msgdef_explanation(
	"An internal software error occurred in the router's SDLC processing
	system.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(NOINPIDB, SDLC, LOG_ERR, MSG_TRACEBACK, "Input idb not set", 10);
msgdef_explanation(
	"This is a software error. A frame was given to the SDLC handler to
	process, but the interface on which the frame arrived is not known.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(FRAMEERR, SDLC, LOG_WARNING, 0,
       "Interface %s, Frame error: CF %#x, VS %d %c VR %d, Reason %#x", 10);
msgdef_explanation(
	"A FRMR frame was received. This can be due to a noisy serial line, an
	overloaded SDLC device, or corrupted data.");
msgdef_recommended_action(
	"If this message recurs, service the serial line and the devices
	attached to it.");
msgdef_ddts_component("");


msgdef_limit(INFOBAD, SDLC, LOG_WARNING, 0,
       "Interface %s, Info field not permitted", 10);
msgdef_explanation(
	"A bad SDLC frame was received.");
msgdef_recommended_action(
	"If any of these messages recur, check the SDLC serial line and the
	devices attached to it.");
msgdef_ddts_component("");


msgdef_limit(CTRLBAD, SDLC, LOG_WARNING, 0,
       "Interface %s, Invalid control field", 10);
msgdef_explanation(
	"A bad SDLC frame was received.");
msgdef_recommended_action(
	"If this message recurs, check the SDLC serial line and the devices
	attached to it.");
msgdef_ddts_component("");


msgdef_limit(N1TOOBIG, SDLC, LOG_WARNING, 0,
       "Interface %s, N1 too large", 10);
msgdef_explanation(
	"An information frame was received from the other end of the SDLC link
	that was larger than allowed with the N1 parameter on this link.");
msgdef_recommended_action(
	"Either increase the value of the N1 parameter for this interface on
	the router or reduce the size of the maximum information frame sent by
	the other end of the link.");
msgdef_ddts_component("");


msgdef_limit(INVNR, SDLC, LOG_WARNING, 0,
       "Interface %s, Invalid NR value", 10);
msgdef_explanation(
	"A bad SDLC frame was received.");
msgdef_recommended_action(
	"If any of these messages recur, check the SDLC serial line and the
	devices attached to it.");
msgdef_ddts_component("");


msgdef_limit(SDLC_ERR, SDLC, LOG_WARNING, 0,
       "SDLC_ERROR with no poll", 10);
msgdef_explanation(
	"A protocol error was detected on a secondary station, but FRMR could
	not be sent because the station did not own the poll bit. This message
	indicates either poor line conditions or a faulty SDLC
	implementation.");
msgdef_recommended_action(
	"If the message recurs, run traces on the SDLC line. Copy the error
	message exactly as it appears, and report it to your technical support
	representative.");
msgdef_ddts_component("");


msgdef_limit(ILLEGSTATE, SDLC, LOG_CRIT, 0,
       "%s, illegal state for addr %02x, %s(%d)", 10);
msgdef_explanation(
	"An internal SDLC state violation was detected.");
msgdef_recommended_action(
	"Record the entire error message and call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef_limit(NOPOINTER, SDLC, LOG_CRIT, 0,
       "AW_SDLC_SEC with no sdllc_llc pointer.", 10);
msgdef_explanation(
	"An error condition occurred during SDLLC initiation.");
msgdef_recommended_action(
	"No action is required. The SDLLC session will restart without operator
	intervention.");
msgdef_ddts_component("");


msgdef_limit(CONFIGERR, SDLC, LOG_NOTICE, 0,
       "%s, addr %02x, Sent SNRM, received SNRM.  Check link station roles.", 
	     20*ONESEC);
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(NOUA, SDLC, LOG_WARNING, 0,
       "%s, addr %02x received command %02x in response to SNRM, expected UA", 
	     20*ONESEC);
msgdef_explanation(
	"The router received a frame other than a UA in response to a SNRM.");
msgdef_recommended_action(
	"Check the end station to see if the SNA stack is up and running.");
msgdef_ddts_component("");


msgdef_limit(INVLDGRPPOLL, SDLC, LOG_NOTICE, 0,
	     "Interface %s, not configured for group-poll frame, received %02x", 60*ONEMIN);
msgdef_explanation(
	"An unnumbered poll (UP) was received with the wrong group poll
	address.");
msgdef_recommended_action(
	"Make sure the group poll address configured on the router matches the
	one configured in the physical unit macro (GP3174=XX) in the Network
	Control Program (NCP) generate file.");
msgdef_ddts_component("");


msgdef_limit(INVLDGRPCFG, SDLC, LOG_NOTICE, 0,
	     "Interface %s, ignoring group-poll mismatch, UPOLL %02x", 
	     60*ONEMIN);
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(BADFRAME, SDLC, LOG_INFO, 0,
	     "%s, Received bad SDLC %sframe, address %02x, control %2X ", 
	     30*ONESEC);
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(DLU_ERROR, SDLC, LOG_ERR, MSG_TRACEBACK,
	     "%s, DLU failed to close station %02x before re-opening, %s/%s",
	     ONESEC);
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(XID_NOMATCH, SDLC, LOG_WARNING, 0,
	     " %s, XID mismatch for %02x. CONNECTION REJECTED", 10);
msgdef_explanation(
	"An inbound exchange identification (XID) from a PU 2.0/PU 2.1 node was
	rejected. The targeted SDLC address is displayed. The XID information
	is displayed in a subsequent message = "SDLLC-6-XID_DISPLAY" or
	"SDLLC-6-FORMAT."");
msgdef_recommended_action(
	"This is a security feature. Additional information is displayed to
	help trace the source of the rejected XID. If the message is
	undesirable, simply remove the XID statement from the router
	configuration.");
msgdef_ddts_component("");


msgdef_limit(XID_DISPLAY, SDLC, LOG_INFO, 0,
	     " sa %e da %e ss %x ds %x [%02x%02x%02x%02x]", 10);
msgdef_explanation(
	"This message supplies the information for the received and rejected
	XID.");
msgdef_recommended_action(
	"See the "SDLC-6-XID_NOMATCH" error message.");
msgdef_ddts_component("");


msgdef_limit(XID_FORMAT, SDLC, LOG_INFO, 0, 
	     "Unsupported XID format received, format %d", 10);
msgdef_explanation(
	"An inappropriate exchange identification (XID) format was received for
	this mode of SDLLC operation. Review the type of devices you are
	connecting with SDLLC to make sure they are supported.");
msgdef_recommended_action(
	"See the "SDLLC-6-XID_NOMATCH" error message.");
msgdef_ddts_component("");



facdef(SDLLC);
msgdef_section("Synchronous Data Link Control/Logical Link Control, type 2
	translation error messages");


msgdef(ACT_LINK,  SDLLC, LOG_NOTICE, 0, "SDLLC: %s LINK address %02x ACTIVATED: %s");
msgdef_explanation(
	"An SDLLC media conversion session was activated.");
msgdef_recommended_action(
	"No action is required.");
msgdef_ddts_component("");


msgdef(DACT_LINK, SDLLC, LOG_NOTICE, 0, "SDLLC: %s LINK address %02x DEACTIVATED: %s");
msgdef_explanation(
	"An SDLLC media conversion session was deactivated. If deactivation is
	a result of an error condition, this message will include a reason.");
msgdef_recommended_action(
	"If the message does not include a description of an error, the
	deactivation was normal and the message is for information only. If the
	message does include a description of an error, begin problem analysis.
	Determine whether session loss is related to LLC2 timers by issuing the
	debug llc2-err command. If the problem is related to LLC2 timers,
	consider using SDLLC with the local acknowledgment feature. Copy the
	error message exactly as it appears, and report it to your technical
	support representative.");
msgdef_ddts_component("");
