/* $Id: msg_qem.c,v 3.1.2.2 1996/09/14 00:02:03 hampton Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/msg_qem.c,v $
 *------------------------------------------------------------------
 * M S G _ Q E M . C
 *
 * August 1996, Nick Thille
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_qem.c,v $
 * Revision 3.1.2.2  1996/09/14  00:02:03  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/08/28  13:13:05  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1  1996/08/27  23:02:21  thille
 * Placeholders for cleanup of message files and externs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(QEM);
msgdef_section("");


msgdef(DISCOVER, QEM, LOG_ERR, 0, "Found %s switch processors");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
