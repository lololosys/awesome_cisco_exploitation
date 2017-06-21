/* $Id: msg_comp.c,v 3.2.62.3 1996/09/14 00:10:13 hampton Exp $
 * $Source: /release/112/cvs/Xsys/x25/msg_comp.c,v $
 *------------------------------------------------------------------
 * msg_comp.c - Message file for compression facility
 *
 * July 29, 1993, Bill Thomas
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_comp.c,v $
 * Revision 3.2.62.3  1996/09/14  00:10:13  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/08/28  13:23:10  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.62.1  1996/07/23  13:32:31  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.2  1995/11/17  18:06:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:21:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(COMP);
msgdef_section("Compression error messages");

/* 4th arg can be MSG_PROCESS | MSG_TRACEBACK | 0 */


msgdef(NOBUF, COMP, LOG_NOTICE, 0, "Interface %s %s");
msgdef_explanation(
	"Interface serial0 compression getbuffer failed, causing Link Access
	Procedure, Balanced (LAPB) retransmissions.");
msgdef_recommended_action(
	"Configure additional large and big buffers.");
msgdef_ddts_component("");

msgdef(SYNCBAD, COMP, LOG_ERR, 0,
       "Interface %s Decompression sync error");
msgdef_explanation(
	"One interface is configured for predictor compression, the other is
	not.");
msgdef_recommended_action(
	"Configure both interfaces for predictor compression.");
msgdef_ddts_component("");

msgdef(VERSIONBAD, COMP, LOG_ERR, 0, 
       "Interface %s Compression Version mismatch expected %x received %x");
msgdef_explanation(
	"One interface is configured for predictor compression, the other is
	not.");
msgdef_recommended_action(
	"Configure both interfaces for predictor compression.");
msgdef_ddts_component("");


msgdef(SIZEBAD, COMP, LOG_ERR, 0, 
       "Interface %s Decompression size error");
msgdef_explanation(
	"One interface is configured for predictor compression, the other is
	not.");
msgdef_recommended_action(
	"Configure both interfaces for predictor compression.");
msgdef_ddts_component("");


msgdef(CONFIGBAD, COMP, LOG_ERR,  MSG_TRACEBACK, 
       "Interface %s  Compression internal configuration error");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");
