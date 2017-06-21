/* $Id: msg_unix.c,v 3.1.2.3 1996/09/14 00:09:52 hampton Exp $
 * $Source: /release/112/cvs/Xsys/unix/msg_unix.c,v $
 *------------------------------------------------------------------
 * M S G  _ U N I X . C
 *
 * August 1996, Nick Thille
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_unix.c,v $
 * Revision 3.1.2.3  1996/09/14  00:09:52  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/09/13  04:14:59  smackie
 * Fix some of the major unix image unhappiness. (CSCdi69048)
 *
 *    o Remove problematic unix NIT support
 *    o Change default memory pool alignment to align 64 bit integers
 *    o Fix message compilation errors
 *
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/08/28  13:18:50  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1  1996/08/25  01:18:15  thille
 * Placeholders for cleanup of message files and externs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(UNIX);
msgdef_section("");


msgdef(SYSABORT, UNIX, LOG_ALERT, MSG_PROCESS | MSG_TRACEBACK,
	"System aborted");
msgdef_explanation(
	"The system aborted. Record the output from the following commands: 

	show proc mem (repeated twice)

	show memory

	show buffers

	show version

	show runing-config.");
msgdef_recommended_action(
	"Provide this information and the error message to your
	technical support representative.");
msgdef_ddts_component("");
