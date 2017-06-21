/* $Id: msg_slip.c,v 3.2.60.2 1996/09/13 23:49:28 hampton Exp $
 * $Source: /release/112/cvs/Xsys/if/msg_slip.c,v $
 *------------------------------------------------------------------
 * msg_slip.c - Message file for slip facility
 *
 * July 19, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_slip.c,v $
 * Revision 3.2.60.2  1996/09/13  23:49:28  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/08/28  12:48:24  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  09:30:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:50:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:53:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(SLIP);				/* Define the SLIP facility */
msgdef_section("Serial Line Internet Protocol error messages");


msgdef(BADSTATE, SLIP, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Impossible input state %x");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If either of these messages recurs, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(BADQUOTE, SLIP, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Impossible quoted character %x");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If either of these messages recurs, call your technical support
	representative for assistance.");
msgdef_ddts_component("");
