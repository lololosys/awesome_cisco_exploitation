/* $Id: msg_pim.c,v 3.1.2.3 1996/09/13 23:49:51 hampton Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/msg_pim.c,v $
 *------------------------------------------------------------------
 * M S G _ P I M . C
 *
 * August 1996, Nick Thille 
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_pim.c,v $
 * Revision 3.1.2.3  1996/09/13  23:49:51  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/09/10  20:58:38  lwei
 * CSCdi68465:  RP information is too difficult to change in last-hop
 * routers
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/08/28  12:51:12  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1  1996/08/25  01:17:18  thille
 * Placeholders for cleanup of message files and externs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


/*
 * Message definitions.
 */
facdef(PIM);
msgdef_section("Protocol-independent multicast error messages.");


msgdef_limit(INVALID_RP_JOIN, PIM, LOG_ALERT, 0,
	     "Received (*, %i) Join from %i for invalid RP %i", 
	     MSGDEF_LIMIT_GLACIAL);
msgdef_explanation(
	"A downstream PIM router sent a join message for the shared tree, which
	this router does not want to accept. This behavior indicates that this
	router will let only downstream routers join to a specific rendezvous
	point.");
msgdef_recommended_action(
	"Configure all downstream leaf routers to join to the RP that is
	allowed by upstream routers toward the validated rendezvous point.");
msgdef_ddts_component("");


msgdef_limit(INVALID_RP_REG, PIM, LOG_ALERT, 0,
	     "Received Register from %i for %i, not willing to be RP",
	     MSGDEF_LIMIT_GLACIAL);
msgdef_explanation(
	"A PIM router received a register message from another PIM router that
	thinks it is the rendezvous point. If the router is not configured for
	another rendezvous point, it will not accept the register message.");
msgdef_recommended_action(
	"Configure all leaf routers (first-hop routers to multicast sources)
	with the IP address of the valid rendezvous point.");
msgdef_ddts_component("");


/*
 * Autorp message definitions.
 */
facdef(AUTORP);
msgdef_section("");


msgdef(RADIXINIT, AUTORP, LOG_CRIT, 0,
       "Error initializing the Auto-RP radix tree");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(RADIXINSERT, AUTORP, LOG_WARNING, 0,
       "Error inserting a new entry into the RP-mapping radix tree");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(PRM_DEL, AUTORP, LOG_WARNING, 0,
        "Error expiring semi-static RP-mapping entry (%i/%d, RP:%i)");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(MAPPING, AUTORP, LOG_NOTICE, 0, "RP for %i/%d is now %i");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef_limit(MANY_RPS, AUTORP, LOG_WARNING, MSG_TRACEBACK | MSG_PROCESS,
       "Many items in learned RP queue: %d", MSGDEF_LIMIT_MEDIUM);
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
