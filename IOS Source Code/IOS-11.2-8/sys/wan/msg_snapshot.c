/* $Id: msg_snapshot.c,v 3.2.62.2 1996/09/14 00:10:10 hampton Exp $
 * $Source: /release/112/cvs/Xsys/wan/msg_snapshot.c,v $
 *------------------------------------------------------------------
 * msg_snapshot.c - Message file for Snapshot Routing
 *
 * April, 1994, Joel P. Bion
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_snapshot.c,v $
 * Revision 3.2.62.2  1996/09/14  00:10:10  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/08/28  13:22:09  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  18:03:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:50:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/20  20:38:16  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  23:18:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(SNAPSHOT);
msgdef_section("Snapshot dial-on-demand routing error messages");


msgdef(BADSTATE, SNAPSHOT, LOG_CRIT, 0,
       "Bad state for Snapshot block %s[%d], location %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(TOOMANYDIALERS, SNAPSHOT, LOG_CRIT, 0,
       "Exceeded maximum dialer interfaces to watch. Ignoring %s");
msgdef_explanation(
	"Snapshot is being used over more than 100 DDR interfaces.");
msgdef_recommended_action(
	"Do not use snapshot over more than 100 DDR interfaces.");
msgdef_ddts_component("");
