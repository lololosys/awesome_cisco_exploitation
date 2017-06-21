/* $Id: msg_c1000_ether.c,v 3.2.62.2 1996/09/14 00:01:49 hampton Exp $
 * $Source: /release/112/cvs/Xsys/src-36-c1000/msg_c1000_ether.c,v $
 *------------------------------------------------------------------
 * msg_c1000_ether.c - Message file for C1000 series ethernet facility
 *
 * June, 1995  Greg Stovall
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_c1000_ether.c,v $
 * Revision 3.2.62.2  1996/09/14  00:01:49  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/08/28  13:10:48  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  00:29:43  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:19:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/15  19:16:48  gstovall
 * Adding placeholder.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"
#include "../os/msgtxt_sys.h"


facdef(QUICC_ETHER);
msgdef_section("MC68360 Quad Integrated Communications Controller error
	messages");


msgdef(INITFAIL, QUICC_ETHER, LOG_ALERT, 0, msgtxt_initfail);
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(COLL, QUICC_ETHER, LOG_NOTICE, 0,
       "Unit %d, excessive collisions. Retry limit %d exceeded");
msgdef_explanation(
	"An Ethernet cable is broken or unterminated.");
msgdef_recommended_action(
	"Check cables for proper connections, termination, and so on.");
msgdef_ddts_component("");


msgdef(UNDERFLO, QUICC_ETHER, LOG_ERR, 0, 
       "Unit %d, underflow error");
msgdef_explanation(
	"While transmitting a frame, the Ethernet controller chip's local
	buffer received insufficient data because data could not be transferred
	to the chip fast enough to keep pace with its output rate. Normally,
	such a problem is temporary, depending on transient peak loads within
	the system.");
msgdef_recommended_action(
	"The system should recover. No action is required.

	If the problem reccurs, it indicates a hardware error that might be
	related to data traffic patterns. Copy the error message exactly as it
	appears, and report it to your technical support representative.");
msgdef_ddts_component("");


msgdef_limit(LOSTCARR, QUICC_ETHER, LOG_ALERT, 0,
             "Unit %d, lost carrier. Transceiver problem?",
	     MSGDEF_LIMIT_GLACIAL);
msgdef_explanation(
	"The Ethernet 10BaseT cable is unplugged.");
msgdef_recommended_action(
	"Connect the 10BaseT Ethernet cable.");
msgdef_ddts_component("");


msgdef(LATECOLL, QUICC_ETHER, LOG_NOTICE, 0, "Unit %d, late collision error");
msgdef_explanation(
	"The Ethernet cable might be too long, or there could be too many
	repeaters such that the delay from one end to the other is too long.
	The Ethernet cable might be overloaded with too many users.");
msgdef_recommended_action(
	"Verify that your Ethernet cable is the correct length and that you do
	not have too many repeaters in use. If these are not the problem, try
	removing hosts from the Ethernet segment to reduce the load.");
msgdef_ddts_component("");


msgdef(HBEAT, QUICC_ETHER, LOG_NOTICE, 0, "Unit %d, heartbeat check failure");
msgdef_explanation(
	"The Ethernet transceiver is malfunctioning.");
msgdef_recommended_action(
	"Replace the hardware.");
msgdef_ddts_component("");



facdef(ETHERNET);
msgdef_section("C1000 series Ethernet error messages");


msgdef(NOMEMORY, ETHERNET, LOG_ALERT, 0, msgtxt_nomemory);
msgdef_explanation(
	"Insufficient memory exists to allocate the necessary memory pools.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(TXERR, ETHERNET, LOG_ALERT, 0, 
       "%s: Fatal transmit error. Restarting...");
msgdef_explanation(
	"A fatal transmit error occurred on the Ethernet line, causing the line
	to be restarted.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");
