/* $Id: msg_gt64010.c,v 3.1.2.2 1996/09/14 00:01:55 hampton Exp $
 * $Source: /release/112/cvs/Xsys/src-4k/msg_gt64010.c,v $
 *------------------------------------------------------------------
 * M S G  _ G T 6 4 0 1 0 . C
 *
 * August 1996, Nick Thille
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_gt64010.c,v $
 * Revision 3.1.2.2  1996/09/14  00:01:55  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/08/28  13:10:59  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1  1996/08/25  01:17:58  thille
 * Placeholders for cleanup of message files and externs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(GT64010);			/* Define the GT64010 facility */
msgdef_section("");


msgdef(DMA, GT64010, LOG_ERR, 0, 
       "Interrupt error, c=%#x, m=%#x, rc=%#x");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(TIMER, GT64010, LOG_ERR, 0,
       "Interrupt error, c=%#x, m=%#x, rc=%#x");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
