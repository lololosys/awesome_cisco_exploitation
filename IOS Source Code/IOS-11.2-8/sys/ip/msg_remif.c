/* $Id: msg_remif.c,v 3.1.2.2 1996/09/13 23:49:43 hampton Exp $
 * $Source: /release/112/cvs/Xsys/ip/msg_remif.c,v $
 *------------------------------------------------------------------
 * M S G  _ R E M I F . C
 *
 * August 1996, Nick Thille
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_remif.c,v $
 * Revision 3.1.2.2  1996/09/13  23:49:43  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/08/28  12:50:16  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1  1996/08/25  01:17:10  thille
 * Placeholders for cleanup of message files and externs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(REMIF);
msgdef_section("");


msgdef(NOMEM, REMIF, LOG_ERR, 0, "No available memory for %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(BADCMD, REMIF, LOG_ERR, 0, "%u from %i");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(OQUEUE, REMIF, LOG_ERR, MSG_TRACEBACK, "output on %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(IQUEUE, REMIF, LOG_ERR, MSG_TRACEBACK, "input on %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(IFNONE, REMIF, LOG_ERR, 0, "%s does not exist. Requested by %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(TCPERR, REMIF, LOG_ERR, 0, "TCP error, status %d");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(DUPOPEN, REMIF, LOG_ERR, 0, "%s is already open by %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(OPEN, REMIF, LOG_ERR, 0, "%s now owns %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(CLOSED, REMIF, LOG_ERR, 0, "%s has released %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
