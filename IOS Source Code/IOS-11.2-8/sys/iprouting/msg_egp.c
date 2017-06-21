/* $Id: msg_egp.c,v 3.2.58.2 1996/09/13 23:49:56 hampton Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/msg_egp.c,v $
 *------------------------------------------------------------------
 * msg_egp.c - Message file for egp facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_egp.c,v $
 * Revision 3.2.58.2  1996/09/13  23:49:56  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.58.1  1996/08/28  12:52:52  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  17:35:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:08:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(EGP);
msgdef_section("Exterior Gateway Protocol error messages");


msgdef(NOPDB, EGP, LOG_ERR, 0, "No pdb for %i");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance."); 
msgdef_ddts_component("");


msgdef(TOOBIG, EGP, LOG_ERR, MSG_TRACEBACK,
       "Insufficient (%d) buffering for update message");
msgdef_explanation(
	"An EGP update message was too large to fit into a single buffer.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");
