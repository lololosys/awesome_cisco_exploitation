/* $Id: msg_tcp.c,v 3.3.28.2 1996/09/14 00:09:40 hampton Exp $
 * $Source: /release/112/cvs/Xsys/tcp/msg_tcp.c,v $
 *------------------------------------------------------------------
 * msg_tcp.c - Message file for tcp facility
 *
 * July 20, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_tcp.c,v $
 * Revision 3.3.28.2  1996/09/14  00:09:40  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.28.1  1996/08/28  13:17:04  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3  1996/01/16  16:53:09  ahh
 * CSCdi46924:  TCP: listener process can hang if callback does not accept
 * Throw the new connection away if the app refuses to take it.
 *
 * Revision 3.2  1995/11/17  18:47:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:35:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:05:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(TCP);
msgdef_section("Transmission Control Protocol error messages");


msgdef(BUFFER, TCP, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Tty%t, buffering bug");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If either message recurs, call your technical support representative
	for assistance.");
msgdef_ddts_component("");


msgdef(PUTBYTE, TCP, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Tty%t, tcp_putbyte() with blocking disabled");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If either message recurs, call your technical support representative
	for assistance.");
msgdef_ddts_component("");


msgdef(NOTREADY, TCP, LOG_CRIT, MSG_TRACEBACK,
       "%s called but TCP not initialized");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, copy the message exactly as it appears and
	contact your technical support.");
msgdef_ddts_component("");


msgdef(TOOBIG, TCP, LOG_INFO, MSG_TRACEBACK | MSG_PROCESS,
       "Tty%t, too many bytes of options (%d)");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADAUTH, TCP, LOG_INFO, 0,
       "%s MD5 digest from %i:%d to %i:%d%s");
msgdef_explanation(
	"The system received a TCP packet with an invalid MD5 authentication
	digest on a connection endpoint. The message ends with the string
	"(RST)" if the incoming packet was a TCP reset packet.");
msgdef_recommended_action(
	"Determine why the source of the packet is not generating correct
	digests on the connection. This could be due to either misconfiguration
	or security weaknesses.");
msgdef_ddts_component("");


msgdef(ACCEPT, TCP, LOG_CRIT, 0,
       "Callback failed to accept connection from %i:%d to %i:%d -- discarding");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, copy it down exactly as it appears and contact
	your technical support representative for assistance.");
msgdef_ddts_component("");


msgdef(BADQUEUE, TCP, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Multiple entry for packet %x");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, copy it down exactly as it appears and contact
	your technical support representative for assistance.");
msgdef_ddts_component("");
	

msgdef(BADREFCNT, TCP, LOG_CRIT, MSG_TRACEBACK,
	"Tty%t: Bad refcnt for packet 0x%x during retransmit, %s:%d to %s:%d, state %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, copy it down exactly as it appears and contact
	your technical support representative for assistance.");
msgdef_ddts_component("");
