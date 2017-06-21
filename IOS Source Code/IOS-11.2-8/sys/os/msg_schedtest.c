/* $Id: msg_schedtest.c,v 3.1.2.2 1996/09/14 00:00:58 hampton Exp $
 * $Source: /release/112/cvs/Xsys/os/msg_schedtest.c,v $
 *------------------------------------------------------------------
 * M S G  _ S C H E D T E S T. C
 *
 * August 1996, Nick Thille
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_schedtest.c,v $
 * Revision 3.1.2.2  1996/09/14  00:00:58  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/08/28  13:03:35  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1  1996/08/25  01:17:49  thille
 * Placeholders for cleanup of message files and externs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"





/*
 * Error messages produced by scheduler test code.
 *
 * THESE MESSAGES SHOULD NOT BE DOCUMENTED.  THEY WILL NEVER APPEAR
 * IN PRODUCTION CODE.
 */
facdef(SCHED_TEST);
msgdef_section("Scheduler Testing Error Messages");


msgdef(ERRMSG, SCHED_TEST, LOG_INFO, MSG_PROCESS,
       "This errmsg should print correctly.");
msgdef_do_not_document("");


msgdef(EXITHOOK, SCHED_TEST, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Exit routine invoked for process %s.");
msgdef_do_not_document("");
