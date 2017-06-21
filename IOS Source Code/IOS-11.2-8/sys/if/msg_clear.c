/* $Id: msg_clear.c,v 3.2.60.2 1996/09/13 23:49:23 hampton Exp $ 
 * $Source: /release/112/cvs/Xsys/if/msg_clear.c,v $ 
 *------------------------------------------------------------------
 * msg_clear.c - Message file for clear command 
 *
 * Aug 11, 1994, Derek Yeung 
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_clear.c,v $
 * Revision 3.2.60.2  1996/09/13  23:49:23  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/08/28  12:48:13  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  09:30:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:50:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:53:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(CLEAR);				/* Define CLEAR facility */
msgdef_section("Clear error message");


msgdef(COUNTERS, CLEAR, LOG_NOTICE, 0, "Clear counter on %s %s by %s");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");
