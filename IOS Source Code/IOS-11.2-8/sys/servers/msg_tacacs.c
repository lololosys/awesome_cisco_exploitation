/* $Id: msg_tacacs.c,v 3.2.62.2 1996/09/14 00:01:36 hampton Exp $
 * $Source: /release/112/cvs/Xsys/servers/msg_tacacs.c,v $
 *------------------------------------------------------------------
 * msg_tacacs.c - Message file for tacacs facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_tacacs.c,v $
 * Revision 3.2.62.2  1996/09/14  00:01:36  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/08/28  13:08:51  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  18:51:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:44:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(TAC);
msgdef_section(
	"Terminal Access Controller Access Control System (TACACS) protocol
	error messages");


msgdef(SENDTMO, TAC, LOG_INFO, 0, "Send type %d to %i timed out");
msgdef_explanation(
	"A background TACACS notification (enabled with the command tacacs
	notify) was not acknowledged by the TACACS server processor within the
	timeout period (5 minutes). The information contained in that
	notification was lost. This loss of information may interfere with
	accounting or auditing on the server.

	This condition arises when the TACACS server is misconfigured, crashed,
	or became unreachable via the network.");
msgdef_recommended_action(
	"Check the TACACS server and the network attached to it.");
msgdef_ddts_component("");


msgdef(UNEXREP, TAC, LOG_WARNING, 0,
       "Reply for non-existent request, %d on queue");
msgdef_explanation(
	"The TACACS facility received a message it was not expecting. This may
	occur when a TACACS server sends duplicate responses or when it
	responds to a request that has already timed out. It also may be due to
	an internal software problem.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");
