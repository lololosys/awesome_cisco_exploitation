/* $Id: msg_termqueue.c,v 3.2.60.2 1996/09/14 00:09:49 hampton Exp $
 * $Source: /release/112/cvs/Xsys/ts/msg_termqueue.c,v $
 *------------------------------------------------------------------
 * msg_termqueue.c - Message file for termqueue facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_termqueue.c,v $
 * Revision 3.2.60.2  1996/09/14  00:09:49  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/08/28  13:18:17  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  18:55:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:39:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:09:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(TMQ);				/* Define termqueue facility */
msgdef_section("Inbound terminal port queuing error message");


msgdef(NOTFOUND, TMQ, LOG_ERR, MSG_PROCESS | MSG_TRACEBACK,
       "TMQ, Attempt to delete entry not in queue");
msgdef_explanation(
	"An attempt was made to delete an entry not in the queue.");
msgdef_recommended_action(
	"Advisory message only. No action required.");
msgdef_ddts_component("");
