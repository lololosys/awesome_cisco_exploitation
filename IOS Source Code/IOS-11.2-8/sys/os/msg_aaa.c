/* $Id: msg_aaa.c,v 3.1.2.2 1996/09/14 00:00:55 hampton Exp $
 * $Source: /release/112/cvs/Xsys/os/msg_aaa.c,v $
 *------------------------------------------------------------------
 * M S G  _ A A A . C
 *
 * August 1996, Nick Thille
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_aaa.c,v $
 * Revision 3.1.2.2  1996/09/14  00:00:55  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/08/28  13:03:31  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1  1996/08/25  01:17:47  thille
 * Placeholders for cleanup of message files and externs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


/*
 * AAA Accounting facility
 */
facdef(AAAA);
msgdef_section("TACACS+ authentication, authorization, and accounting security
	accounting error messages");


msgdef_limit(BADSTR, AAAA, LOG_ERR, MSG_TRACEBACK, "Bad accounting data: %s",
	3*ONESEC);
msgdef_explanation(
	"During AAA accounting operations, the internal database of accounting
	information for a user was found to be corrupt. In all cases, this
	indicates an internal software error, and that accounting information
	for a particular user session has been lost.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(NULUSR, AAAA, LOG_ERR, MSG_TRACEBACK,
	      "accounting for null user");
msgdef_explanation(
	"This message indicates an internal software error. During shutdown of
	a line or interface, the system tried to send accounting information
	via AAA, but could not find the user to which the accounting
	information belonged. The most likely reason for this error is trying
	to do accounting twice for the same event. This error message can occur
	without any loss of accounting information.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(RELOGIN, AAAA, LOG_ERR, MSG_TRACEBACK,
	      "sanity check in re-login %s to %s");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");
