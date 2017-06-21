/* $Id: msg_pad.c,v 3.2.62.2 1996/09/14 00:10:17 hampton Exp $
 * $Source: /release/112/cvs/Xsys/x25/msg_pad.c,v $
 *------------------------------------------------------------------
 * msg_pad.c - Message file for pad facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_pad.c,v $
 * Revision 3.2.62.2  1996/09/14  00:10:17  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/08/28  13:23:13  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  18:06:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:21:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(PAD);
msgdef_section("X.25 packet assembler/disassembler error messages");


msgdef(GETLINE, PAD, LOG_ERR, 0,
       "Tty%t, bad return code %d from x3_getline()");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, copy the error message exactly as it
	appears and report it to your technical support representative.");
msgdef_ddts_component("");


msgdef(PUTSETUP, PAD, LOG_CRIT, MSG_PROCESS | MSG_TRACEBACK,
       "Tty%t, buffer already setup");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, copy the error message exactly as it
	appears and report it to your technical support representative.");
msgdef_ddts_component("");


msgdef(INTR, PAD, LOG_CRIT, MSG_PROCESS | MSG_TRACEBACK,
       "%s called at interrupt level %x");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, copy the error message exactly as it
	appears and report it to your technical support representative.");
msgdef_ddts_component("");
