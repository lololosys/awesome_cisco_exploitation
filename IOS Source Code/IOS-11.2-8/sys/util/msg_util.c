/* $Id: msg_util.c,v 3.2.62.3 1996/09/14 00:09:56 hampton Exp $
 * $Source: /release/112/cvs/Xsys/util/msg_util.c,v $
 *------------------------------------------------------------------
 * msg_util.c -- Message definitions from utility routines.
 *
 * Feb 1994 Tony Li
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_util.c,v $
 * Revision 3.2.62.3  1996/09/14  00:09:56  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/08/28  13:18:59  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.62.1  1996/04/12  16:53:01  mshannon
 * CSCdi36175:  range code doesnt check malloc return code
 * Branch: California_branch
 * Null pointer to checks added to range list routines
 *
 * Revision 3.2  1995/11/17  17:56:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:43:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:13:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(UTIL);
msgdef_section("Utility error messages");


msgdef(RANGEINCON, UTIL, LOG_ERR, MSG_TRACEBACK,
       "internal inconsistency %d %x");
msgdef_explanation(
	"A fatal software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(RANGENULLINPUT, UTIL, LOG_ERR, MSG_TRACEBACK,
       "null pointer input to range function");
msgdef_explanation(
	"An attempt was made to access a range function with a NULL pointer.
	The range list creation failed.");
msgdef_recommended_action(
	"Review the error log for a corresponding malloc failed message. Copy
	the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(TREE, UTIL, LOG_ERR, MSG_TRACEBACK,
	     "Data structure error--%s", ONEMIN);
msgdef_explanation(
	"A software error occurred, resulting in a data structure
	inconsistency.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative. Ask for their assistance in obtaining
	a version of code with the error fixed.");
msgdef_ddts_component("");
