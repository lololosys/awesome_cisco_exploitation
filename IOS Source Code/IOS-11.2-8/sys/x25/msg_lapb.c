/* $Id: msg_lapb.c,v 3.2.62.2 1996/09/14 00:10:15 hampton Exp $
 * $Source: /release/112/cvs/Xsys/x25/msg_lapb.c,v $
 *------------------------------------------------------------------
 * msg_lapb.c - Message file for lapb facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_lapb.c,v $
 * Revision 3.2.62.2  1996/09/14  00:10:15  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/08/28  13:23:12  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  18:06:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:21:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(LAPB);
msgdef_section("X.25 Link Access Procedure, Balanced error messages");


msgdef(NULLPAK, LAPB, LOG_ERR, 0,
       "Interface %s, NULL packet ptr, rvr %d, vs %d, vr %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If either of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(NOINPIDB, LAPB, LOG_ERR, MSG_TRACEBACK, "Input idb not set");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If either of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(FRAMEERR, LAPB, LOG_WARNING, 0,
       "Interface %s, Frame error: CF %#x, VS %d %c VR %d, Reason %#x");
msgdef_explanation(
	"A FRMR frame was received, indicating that a protocol violation was
	detected. The Reason field will be interpreted by generating INFOBAD,
	CTRLBAD, N1TOOBIG, or INVNR messages immediately after this message;
	refer to the explanations for these messages for more information.");
msgdef_recommended_action(
	"The protocol will recover by resetting the service. This will disrupt
	any network service on top of LAPB (for example, X.25 virtual circuits
	will be cleared). Follow the recommended action for any error message
	following the FRMR reports; if service becomes unacceptable, check the
	serial line and its attached devices.");
msgdef_ddts_component("");


msgdef(INFOBAD, LAPB, LOG_WARNING, 0,
       "Interface %s, Info field not permitted");
msgdef_explanation(
	"A received FRMR frame reported that an information field was present
	in a frame where no information field is allowed.");
msgdef_recommended_action(
	"Check the serial line and the device attached to the line.");
msgdef_ddts_component("");


msgdef(CTRLBAD, LAPB, LOG_WARNING, 0,
       "Interface %s, Invalid control field");
msgdef_explanation(
	"A received FRMR reported a frame with an invalid control code.");
msgdef_recommended_action(
	"Check the serial line and the devices attached to the line.");
msgdef_ddts_component("");


msgdef(N1TOOBIG, LAPB, LOG_WARNING, 0,
       "Interface %s, N1 too large");
msgdef_explanation(
	"A received FRMR reported an information frame that was too long.");
msgdef_recommended_action(
	"Verify that the equipment attached to the serial line is configured
	with the same N1 value. N1 is in bits and includes the entire frame
	plus 16 bits of CRC. However, some equipment may require configuration
	in octets, the omission of the two CRC octets, or the omission of the
	three octets of LAPB control information.");
msgdef_ddts_component("");


msgdef(INVNR, LAPB, LOG_WARNING, 0,
       "Interface %s, Invalid NR value");
msgdef_explanation(
	"A received FRMR reported a frame with an invalid N(R) value.");
msgdef_recommended_action(
	"Verify that the equipment attached to the serial line is configured
	with the same K value (maximum number of outstanding I frames).");
msgdef_ddts_component("");


msgdef(NOBUF, LAPB, LOG_CRIT, 0,
       "Interface %s, no buffer available to %s");
msgdef_explanation(
	"There was insufficient memory for the LAPB protocol engine to send a
	message.");
msgdef_recommended_action(
	"This message is acceptable if it occurs infrequently, because the LAPB
	protocol is designed to handle loss of frames. Frequent occurrences can
	cause disruption of service. The system can be configured to ease
	memory demands or, if conditions warrant, the system can be upgraded to
	a larger memory configuration.");
msgdef_ddts_component("");
