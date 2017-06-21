/* $Id: msg_telnet.c,v 3.2.62.3 1996/09/14 00:09:42 hampton Exp $
 * $Source: /release/112/cvs/Xsys/tcp/msg_telnet.c,v $
 *------------------------------------------------------------------
 * msg_telnet.c - Message file for telnet facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_telnet.c,v $
 * Revision 3.2.62.3  1996/09/14  00:09:42  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/08/28  13:17:05  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.62.1  1996/07/12  20:09:28  widmer
 * CSCdi62775:  Bogus externs in telnet.c cause 4k builds to fail
 * Branch: California_branch
 * Remove externs and move externs to header files
 *
 * Revision 3.2  1995/11/17  18:48:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:35:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:05:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(TN);
msgdef_section("Telnet error messages");


msgdef(READLINE, TN, LOG_ERR, 0,
       "Unknown return code %d from telnet_readline()");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(BADSTATE, TN, LOG_ERR, 0, "Illegal state %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(BADLOGIN, TN, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Bad login string pointer %#x");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");
