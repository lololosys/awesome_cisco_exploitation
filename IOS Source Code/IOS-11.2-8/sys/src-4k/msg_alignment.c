/* $Id: msg_alignment.c,v 3.1.2.2 1996/09/14 00:01:53 hampton Exp $
 * $Source: /release/112/cvs/Xsys/src-4k/msg_alignment.c,v $
 *------------------------------------------------------------------
 * M S G  _ A L I G N M E N T . C
 *
 * August 1996, Nick Thille
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_alignment.c,v $
 * Revision 3.1.2.2  1996/09/14  00:01:53  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/08/28  13:10:58  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1  1996/08/25  01:17:57  thille
 * Placeholders for cleanup of message files and externs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(ALIGN);
msgdef_section("The ALIGN facility controls a feature in reduced
	instruction-set computer (RISC)-processor-based Cisco products that
	identifies software in need of memory use optimization.");


msgdef(CORRECT, ALIGN, LOG_ERR, 0,
       "Alignment correction made at 0x%x %sing 0x%x");
msgdef_explanation(
	"A software component within the router needs memory optimization.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");



msgdef(FULL, ALIGN, LOG_ERR, 0,
       "Alignment log is full - no new entries will be recorded.");
msgdef_explanation(
	"The router identified more software components in need of memory
	optimization than it can record.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");



msgdef_limit(SPURIOUS, ALIGN, LOG_ERR, 0,
       "Spurious memory access made at 0x%x reading 0x%x", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"An error was identified (and temporarily corrected) within a software
	component in the router.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");
