/* $Id: msg_dirresp.c,v 3.1.18.3 1996/09/14 00:01:34 hampton Exp $
 * $Source: /release/112/cvs/Xsys/servers/msg_dirresp.c,v $
 *------------------------------------------------------------------
 * msg_dirresp.c - Message file for tacacs facility
 *
 * Feburary 1996, Paul Traina
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_dirresp.c,v $
 * Revision 3.1.18.3  1996/09/14  00:01:34  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.18.2  1996/08/28  13:08:49  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.18.1  1996/07/16  08:01:29  raj
 * CSCdi61197:  Part of  CSCdi49395 patches missing from 11.2
 * Branch: California_branch
 * Inserted patches which were missing and update dirresp code as well.
 *
 * Revision 3.1  1996/02/20  23:50:14  pst
 * Add rest of client files...
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(DRP);
msgdef_section("Director Response Protocol error messages");


msgdef(SUBNETTED, DRP, LOG_ERR, 0,
       "unable to determine metric for %i, %i subnetted");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(NOROUTE, DRP, LOG_DEBUG, 0, "no route for destination %i");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(NOWRITE, DRP, LOG_ERR, 0, "unable to send response to %i:%d");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");
