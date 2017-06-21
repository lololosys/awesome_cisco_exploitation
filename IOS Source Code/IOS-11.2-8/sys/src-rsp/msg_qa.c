/* $Id: msg_qa.c,v 3.2.62.2 1996/09/14 00:02:01 hampton Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/msg_qa.c,v $
 *------------------------------------------------------------------
 * QA messages
 *
 * May 1994, Carl Schaefer
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_qa.c,v $
 * Revision 3.2.62.2  1996/09/14  00:02:01  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/08/28  13:13:03  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  18:46:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:57:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(QA);
msgdef_section("Queue and accumulator error message");


msgdef(ALLOC, QA, LOG_ERR, 0, "%s");
msgdef_explanation(
	"This is a software error.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears. Also copy the output of
	show version and show cont cbus. Contact your technical support
	representative.");
msgdef_ddts_component("");

msgdef(DIAG, QA, LOG_ERR, 0, NULL);
msgdef_explanation(
        "This is a diagnostic message.");
msgdef_recommended_action(
        "This message appears after a QA error, and contains diagnostic 
         information regarding that error.
         Copy the original QA error message and all the following QA 
         diagnostic error messages exactly as they appear. Also copy the
         output of show version and show cont cbus. Contact your technical
         support representative.");
msgdef_ddts_component("");
