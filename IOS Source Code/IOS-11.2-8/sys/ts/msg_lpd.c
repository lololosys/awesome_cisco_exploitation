/* $Id: msg_lpd.c,v 3.1.2.2 1996/09/14 00:09:47 hampton Exp $
 * $Source: /release/112/cvs/Xsys/ts/msg_lpd.c,v $
 *------------------------------------------------------------------
 * M S G  _ L P D . C
 *
 * August 1996, Nick Thille
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_lpd.c,v $
 * Revision 3.1.2.2  1996/09/14  00:09:47  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/08/28  13:18:16  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1  1996/08/25  01:18:09  thille
 * Placeholders for cleanup of message files and externs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(LPD);
msgdef_section("Line printer daemon error message");


msgdef(MANYLF, LPD, LOG_ERR, 0,
       "Line %t, packet has too many newlines to convert");
msgdef_explanation(
	"An internal buffer did not have enough room to add all the necessary
	carriage returns to a packet of LPD data destined for a printer
	configured with a newline-convert command. This message is unlikely to
	occur with a file containing valid data.");
msgdef_recommended_action(
	"Check the file being printed to see whether it contains valid,
	printable data.");
msgdef_ddts_component("");
