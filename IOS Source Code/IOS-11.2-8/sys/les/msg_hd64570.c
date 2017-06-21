/* $Id: msg_hd64570.c,v 3.3.62.2 1996/09/13 23:50:31 hampton Exp $
 * $Source: /release/112/cvs/Xsys/les/msg_hd64570.c,v $
 *------------------------------------------------------------------
 * msg_hd64570.c - Message file for hd64570 facility
 *
 * August 1992, Greg Stovall
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: msg_hd64570.c,v $
 * Revision 3.3.62.2  1996/09/13  23:50:31  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.62.1  1996/08/28  12:59:33  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3  1995/11/17  17:42:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:00:31  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:33:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/19  06:47:59  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:38:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"
#include "../os/msgtxt_sys.h"


facdef(HD);				/* Define the facility */
msgdef_section("HD64570 serial controller error messages");


msgdef(BADRING, HD, LOG_ALERT, 0, msgtxt_badringsize);
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(BADPORTADAPTER, HD, LOG_ALERT, 0,
       "4T Port Adapter fault on port %d in slot %d");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(LINEFLAP, HD, LOG_NOTICE, 0, 
       "Unit %d excessive modem control changes");
msgdef_explanation(
	"Too many modem control interrupts have been received. The port was
	disabled to prevent excessive use of the CPU.");
msgdef_recommended_action(
	"Check the cable on the serial port.");
msgdef_ddts_component("");


msgdef(BADLOOPCABLE, HD, LOG_ALERT, 0,
       "Loopback not supported for a 4T DTE/X.21, port %d in slot %d");
msgdef_explanation(
	"Loopback mode is not allowed when using an X.21 DTE cable on a Quad
	serial NIM port.");
msgdef_recommended_action(
	"Do not enable loopback, or use a different cable type.");
msgdef_ddts_component("");
