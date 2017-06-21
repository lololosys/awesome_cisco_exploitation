/* $Id: msg_bgp.c,v 3.3.20.2 1996/09/13 23:49:54 hampton Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/msg_bgp.c,v $
 *------------------------------------------------------------------
 * msg_bgp.c - Message file for bgp facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_bgp.c,v $
 * Revision 3.3.20.2  1996/09/13  23:49:54  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.20.1  1996/08/28  12:52:50  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3  1996/02/06  07:52:21  rchandra
 * CSCdi46449:  BGP should sanity-check/sanitize confederation info from
 *              path attrs
 * - If confed info is present in update from non-confed neighbors,
 *   replace confed info with peers AS-number
 *
 * Revision 3.2  1995/11/17  17:35:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:08:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(BGP);
msgdef_section("Border Gateway Protocol error messages");


msgdef_limit(NOMEMORY, BGP, LOG_ERR, 0,
	     "No memory for %s entry, resetting", MSGDEF_LIMIT_SLOW);
msgdef_limit(INSUFMEM, BGP, LOG_ERR, 0,
	     "Insufficient memory for %s entry", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"The requested operation could not be accomplished because of a low
	memory condition.");
msgdef_recommended_action(
	"Reduce other system activity to ease memory demands. If conditions
	warrant, upgrade to a larger memory configuration.");
msgdef_ddts_component("");


msgdef(RADIXINIT, BGP, LOG_ERR, 0, "Radix trie initialization failed");
msgdef_explanation(
	"Initialization of the radix tree could not be accomplished because of
	a low memory condition.");
msgdef_recommended_action(
	"Reduce other system activity to ease memory demands. If conditions
	warrant, upgrade to a larger memory configuration.");
msgdef_ddts_component("");


msgdef(BADMASK, BGP, LOG_ERR, 0, "Attempt to install route for %i "
       "without valid mask in %s");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Copy the message exactly as it appears, and report it your technical
	support representative.");
msgdef_ddts_component("");


msgdef_limit(NEXTHOP, BGP, LOG_INFO, 0,
	     "Invalid next hop (%i) received from %i: %s", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"The remote BGP peer sent in a route with an illegal next hop.The given
	route was ignored.");
msgdef_recommended_action(
	"This is an informational message only. The remote peer may be
	misconfigured.");
msgdef_ddts_component("");


msgdef_limit(ASPATH, BGP, LOG_INFO, 0,
	     "%s AS path %s received from %i: %s", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"The remote BGP peer sent in an update with an invalid AS path.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(ADDROUTE, BGP, LOG_ERR, MSG_TRACEBACK,
	     "Unable to add route for %i%m to radix trie", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
        "Addition to the radix trie could not be completed due to error");
msgdef_recommended_action(
	"Copy the error message exactly as it appears and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(DELROUTE, BGP, LOG_ERR, MSG_TRACEBACK,
	     "Unable to remove route for %i%m from radix trie",
	     MSGDEF_LIMIT_SLOW);
msgdef_explanation(
         "Deletion from the radix trie could not be completed due to error");
msgdef_recommended_action(
         "Copy the error message exactly as it appears and report it to your
         technical support representative.");
msgdef_ddts_component("");


msgdef_limit(MAXPATHS, BGP, LOG_ERR, 0,
	     "Too many parallel paths for %i%m, maximum is %d",
	     MSGDEF_LIMIT_SLOW);
msgdef_explanation(
         "Number of parallel paths are more than the configured limit");
msgdef_recommended_action(
         "Increase the number of parallel paths allowed.");
msgdef_ddts_component("");



msgdef_limit(DELPATH, BGP, LOG_ERR, MSG_TRACEBACK,
	     "Attempted to delete path from an empty net for %i%m",
	     MSGDEF_LIMIT_SLOW);
msgdef_explanation(
         "Deletion of a BGP path cannot be completed due to error");
msgdef_recommended_action(
	"Copy the error message exactly as it appears and report it to your
	technical support representative.");
msgdef_ddts_component("");
