/* $Id: msg_mroute.c,v 3.2.70.2 1996/09/13 23:49:49 hampton Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/msg_mroute.c,v $
 *------------------------------------------------------------------
 * msg_mroute.c - Message file for mroute facility.
 *
 * Nov 1995, Manoj Leelanivas
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_mroute.c,v $
 * Revision 3.2.70.2  1996/09/13  23:49:49  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.70.1  1996/08/28  12:51:11  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/29  00:01:00  mleelani
 * CSCdi43368:  7000 crash in ip_get_mdb
 * Add mroute message facility. Make sure the radix entry is
 * deleted before freeing the mdb. Fix memory leaks.
 *
 * Revision 3.1  1995/11/28  22:40:56  mleelani
 * Placeholder for mroute message facility.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(MROUTE);
msgdef_section("Multicast route error messages");


msgdef(RADIXINIT, MROUTE, LOG_CRIT, 0,
       "Error initializing IP multicast radix for %i ");
msgdef_explanation(
	"Insufficient memory is available to initialize the IP multicast
	routing table.");
msgdef_recommended_action(
	"Record the output from the following commands: show process memory
	(repeated twice), show memory, show buffers, show version, and show
	runing config. Provide this information to your technical support
	representative.");
msgdef_ddts_component("");


msgdef(RADIXDELETE, MROUTE, LOG_WARNING, MSG_TRACEBACK | MSG_PROCESS,
       "Error trying to delete multicast route entry %i/%d for %i "
       "(expected %#x, got %#x)");
msgdef_explanation(
	"An error in the multicast routing table occurred. A route could not be
	deleted from the routing table.");
msgdef_recommended_action(
	"Execute a clear ip mroute command. Determine whether the router is low
	on memory. If the problem persists, copy the error message exactly as
	it appears, and report it to your technical support representative.");
msgdef_ddts_component("");


msgdef(RADIXINSERT, MROUTE, LOG_WARNING, MSG_TRACEBACK | MSG_PROCESS,
       "Error trying to add multicast route entry %i/%d for %i "
       "(expected %#x, got %#x)");
msgdef_explanation(
	"An error in the multicast routing table occurred. A route could not be
	inserted in to the routing table.");
msgdef_recommended_action(
	"Execute a clear ip mroute command. Determine whether the router is low
	on memory. If the problem persists, copy the error message exactly as
	it appears, and report it to your technical support representative.");
msgdef_ddts_component("");
