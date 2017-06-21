/* $Id: msg_dvmrp.c,v 3.1.2.2 1996/09/13 23:49:47 hampton Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/msg_dvmrp.c,v $
 *------------------------------------------------------------------
 * M S G  _ D V M R P . C
 *
 * August 1996, Nick Thille
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_dvmrp.c,v $
 * Revision 3.1.2.2  1996/09/13  23:49:47  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/08/28  12:51:10  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1  1996/08/25  01:17:17  thille
 * Placeholders for cleanup of message files and externs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(DVMRP);
msgdef_section("Distance Vector Multicast Routing Protocol (DVMRP) error
	messages");


msgdef(ROUTEHOG, DVMRP, LOG_ALERT, 0,
       "Receiving %d routes from %i (%s) in the last %s");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");

msgdef_limit(ROUTELIMIT, DVMRP, LOG_ALERT, 0,
	     "Exceeded route-limit of %d routes, check your access-lists",
	     MSGDEF_LIMIT_GLACIAL);
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef_limit(BADMASK, DVMRP, LOG_ALERT, 0,
	     "Bad mask %i received from %i, Report ignored", 
	     MSGDEF_LIMIT_GLACIAL);
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(NBRUP, DVMRP, LOG_NOTICE, 0, "Neighbor %i is up on %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(NBRDOWN, DVMRP, LOG_NOTICE, 0, "Neighbor %i went down on %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef_limit(NONPRUNER, DVMRP, LOG_ALERT, 0,
	     "Rejecting non-pruning peer %i (%s)", 
	     MSGDEF_LIMIT_GLACIAL);
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
