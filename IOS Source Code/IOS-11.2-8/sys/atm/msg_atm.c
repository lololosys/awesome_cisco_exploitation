/* $Id: msg_atm.c,v 3.3.20.8 1996/09/13 23:45:50 hampton Exp $
 * $Source: /release/112/cvs/Xsys/atm/msg_atm.c,v $
 *------------------------------------------------------------------
 * ATM error messages
 *
 * February 1995, Ronnie Kon
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_atm.c,v $
 * Revision 3.3.20.8  1996/09/13  23:45:50  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.20.7  1996/09/11  01:10:42  rzagst
 * CSCdi67786:  %ATM-4-MTUCALLMISMATCH message can be confusing
 * Change message to indicate PDU mismatch and relationship between
 * interface
 * PDU and MTU.
 * Branch: California_branch
 *
 * Revision 3.3.20.6  1996/08/28  12:37:26  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.20.5  1996/08/05  22:12:38  jwjang
 * CSCdi65057:  print out err message when sscop detects unmatch uni
 * version
 * Branch: California_branch
 *         CSCdi64992, changed malloc() to malloc_nsmae() in atm_arp.c
 *         fix sscop_rcvQ_cnt and sscop_ackQ_cnt, fixed  no timer running
 *         in  sscop outErrorRecovery state
 *
 * Revision 3.3.20.4  1996/05/31  15:30:42  rzagst
 * CSCdi54940:  ATM driver creates low-bit-rate SVCs
 * Branch: California_branch
 *
 * Revision 3.3.20.3  1996/05/15  21:36:22  mdavison
 * CSCdi56536:  Call mtu mismatch message needs to show calling party
 * address
 * Branch: California_branch
 *
 * Revision 3.3.20.2  1996/04/29  21:08:39  rzagst
 * CSCdi55300:  Invalid vcd of 65535 causes FAIL_CREATEVC error
 * Branch: California_branch
 *
 * Revision 3.3.20.1  1996/04/25  23:02:52  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.3.24.1  1996/03/22  23:13:14  rzagst
 * CSCdi43565:  AIP needs to reject calls with MTU other than default
 * Branch: ATM_Cal_branch
 *
 * Revision 3.3  1996/01/26  03:06:52  awu
 * CSCdi43779:  Declaring or removing a PVC can cause loss of all SVCs.
 *  -change the way of setting up a rateq so that resetting the interf card
 *   is being bypassed.
 *
 * Revision 3.2  1995/11/17  08:47:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:01:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/07/31  22:54:18  awu
 * CSCdi37938:  FAILCREATEVC uses VCD in hex, but show atm vc uses VCD in
 * decimal.
 *
 * Revision 2.4  1995/07/27  14:46:12  awu
 * CSCdi36054:  ATM driver calls printf.
 *
 * Revision 2.3  1995/06/20  07:13:10  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/18  20:31:50  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:12:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"
#include "../os/msgtxt_sys.h"


facdef(ATM);				/* Define ATM facility */
msgdef_section("Asynchronous Transfer Mode error messages");

msgdef(NOPUSRAM, ATM, LOG_ALERT, 0, "Unit %d, not enough interface memory");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef_limit(UNSUPPORTED_QOS_PARAMS, ATM, LOG_ERR, 0,
	     "ATM failed to create VC on Interface %s, (Cause of the failure:"
 	     " Invalid traffic shaping parameters or out of rate queues."
 	     ,MSGDEF_LIMIT_FAST);
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");



msgdef_limit(OUT_OF_VCDS, ATM, LOG_ERR, 0,
	     "ATM failed to create VC on Interface %s, (Cause of the failure: All VCDS used.)",MSGDEF_LIMIT_FAST);
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef_limit(MTUCALLMISMATCH,ATM,LOG_WARNING,0,
	     "Call from: \n"
	     "     ATM NSAP - %s\n"
  	     "     has mismatched PDU size - forward size is %d\n"
	     "     - backward size is %d,\n"
  	     "     int %s PDU size %s %s  ",MSGDEF_LIMIT_FAST);
msgdef_explanation(
	"The calling or called party has a maximum transmission unit (MTU) size
	configured that is different from the MTU size on the current
	interface.");
msgdef_recommended_action(
	"Either change the configured MTU size on the current interface to that
	of the caller's or called party's interface, or change the caller's or
	called party's interface to match the current interface.");
msgdef_ddts_component("");


msgdef_limit(FAILCREATEVC, ATM, LOG_ERR, 0,
	     "ATM failed to create VC(VCD=%d, VPI=%d, VCI=%d) on Interface %s, (Cause of the failure: %s)",
	     MSGDEF_LIMIT_FAST);
msgdef_explanation(
	"This message can occur for the following unrelated reasons.
	Each reason is accompanied by a recommended action.

	The virtual circuit descriptor (VCD) exceeded the maximum available
	memory. - Use a smaller VCD number.

	Address Resolution Protocol (ARP) is allowed only on AAL5-SNAP virtual
	circuits. - Change the encapsulation to AAL5SNAP.

	The virtual path identifier-virtual channel identifier (VPI-VCI) pair
	or the VCD is already in use. - Try using a different VPI-VCI or VCD
	number.

	Only one virtual channel connection (VCC) is allowed on a
	point-to-point interface. - Try using a different subinterface.

	AAL3/4-SMDS virtual circuit must be the only virtual circuit on an
	interface. - Turn off AAL3/4.

	AAL3/4-SMDS MID exceeds the maximum. - Try a smaller Multiplex ID (MID)
	value.

	Switched Multimegabit Data Service (SMDS) is not enabled. - Enable SMDS.

	The driver failed to accept the virtual circuit. - Try initiating the
	virtual circuit again.");
msgdef_recommended_action(
	"If none of the previous actions fix the problem, copy the error
	message exactly as it appears and report it to your technical support
	representative.");
msgdef_ddts_component("");


msgdef_limit(OVERSUBSCRIBED, ATM, LOG_WARNING, 0,
	     "Interface %s: Total %s allocation %u%s exceeded maximum %s of %u%s.",
	     MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");
