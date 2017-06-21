/* $Id: msg_llc.c,v 3.4.56.3 1996/09/13 23:50:52 hampton Exp $
 * $Source: /release/112/cvs/Xsys/llc2/msg_llc.c,v $
 *------------------------------------------------------------------
 * msg_llc.c - Message file for LLC facility
 *
 * April, 1994, Rich Livingston
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: msg_llc.c,v $
 * Revision 3.4.56.3  1996/09/13  23:50:52  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.4.56.2  1996/08/28  13:00:20  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.56.1  1996/07/31  06:48:49  gcox
 * CSCdi64685:  Convert LLC2 to interface_generic.h (except llc2.c)
 * Branch: California_branch
 *
 * Revision 3.4  1995/11/21  19:34:05  sberl
 * CSCdi42181:  More than 1000 LLC2 sessions causes excessive tracebacks
 * Remove the check for > 1000 llc sessions from llc2_tick().
 * Add a counter to keep track of how many llc2 sessions there really are.
 * Add this count to banner for "show llc2".
 * Make llc2_starttimer() a real function instead of an inline. It was way
 * too big.
 *
 * Revision 3.3  1995/11/17  17:44:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:01:38  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:35:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:42:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(LLC);
msgdef_section("Logical Link Control, type 2 error messages");


msgdef(T1_ERR,  LLC, LOG_WARNING, 0, "LLC2: %s  INCREASE LLC TIMERS");
msgdef_explanation(
	"The T1 timer popped for an LLC2 session. However, the frame which is
	to be retransmitted is still on a queue inside the router awaiting
	transmission over RSRB, Frame Relay, or other protocol. If the
	retransmission occurs, the LLC2 session will probably fail due to N2
	retry failures.");
msgdef_recommended_action(
	"Increasing the LLC2 acknowledgment timers decreases the likelihood of
	failure. In addition, investigate sporadic congestion in a routed
	network.");
msgdef_ddts_component("");


msgdef(UNEXPECT, LLC, LOG_CRIT, MSG_TRACEBACK, "LLC2: %s  UNEXPECTED EVENT");
msgdef_explanation(
	"An internal software error was found. A software component is trying
	to use LLC2 structures that do not exist.");
msgdef_recommended_action(
	"Record the configuration and any information that would be useful in
	recreating the error and call your technical support representative.");
msgdef_ddts_component("");


msgdef_limit(TOO_MANY_LLC, LLC, LOG_WARNING, MSG_PROCESS, 
	     "LLC2: TIMER QUEUE CORRUPTED", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
