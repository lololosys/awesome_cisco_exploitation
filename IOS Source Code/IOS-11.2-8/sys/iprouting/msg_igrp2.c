/* $Id: msg_igrp2.c,v 3.2.58.3 1996/09/13 23:50:00 hampton Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/msg_igrp2.c,v $
 *------------------------------------------------------------------
 * msg_igrp2.c - Message file for igrp2 facility
 *
 * February, 1994, Bob Albrightson
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_igrp2.c,v $
 * Revision 3.2.58.3  1996/09/13  23:50:00  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.58.2  1996/08/28  12:52:55  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.58.1  1996/04/02  05:37:16  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.2  1995/11/17  17:35:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/19  01:07:50  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:08:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * msg_igrp2.c - Message file for igrp2 facility
 *
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(DUAL);
msgdef_section("Enhanced Interior Gateway Protocol error messages");


msgdef(SIA, DUAL, LOG_ERR, 0,
       "Route %s stuck-in-active state in %s %d.  Cleaning up");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(SIANOHANDLE, DUAL, LOG_ERR, 0,
     "Stuck-in-active and %d unallocated handle(s) in reply-status table "
     "(%#x).  Cleaning up");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(NOMATCH, DUAL, LOG_ERR, MSG_TRACEBACK,
       "Nexthop and infosource don't match.  N = %s, I = %s");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(NOHANDLE, DUAL, LOG_ERR, 0, "Handle %d is not allocated in pool.");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(LINKSTILLUP, DUAL, LOG_ERR, 0, "Link %d %s still exists on %s");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(LINKSEXIST, DUAL, LOG_ERR, MSG_TRACEBACK,
       "Interface %s going down and %d links exist");
msgdef_explanation(
	"An interface is going down or is being removed from routing through
	IGRP, but not all links (neighbors) have been removed from the topology
	table.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADCOUNT, DUAL, LOG_ERR, MSG_TRACEBACK,
       "Route %s, %d successors, %d rdbs");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(EVENT, DUAL, LOG_INFO, 0, "%s, %s");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(NOSOCKET, DUAL, LOG_ERR, 0, "Unable to open socket for AS %d");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(NOBUFFER, DUAL, LOG_ERR, 0,
	     "No buffers available for %u byte packet", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"The DUAL software was unable to allocate a packet buffer. The system
	may be out of memory.");
msgdef_recommended_action(
	"Record the output from the following commands: show proc mem (repeated
	twice), show mem, show buffers, show version, write terminal. Provide
	this information to your technical support representative.");
msgdef_ddts_component("");


msgdef_limit(UNKTIMER, DUAL, LOG_ERR, MSG_TRACEBACK,
  	     "Unknown timer type %d expiration", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(REFCOUNT, DUAL, LOG_ERR, 0,
  	     "Negative refcount in pakdesc %x", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(PROBESEQ, DUAL, LOG_ERR, 0,
  	     "Bad probe sequence %d from %s, expecting %d",
  	     /*MSGDEF_LIMIT_MEDIUM*/0);
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(MULTIFLOW, DUAL, LOG_ERR, 0,
  	     "Flow control error, %s, on %s", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(PEERSEXIST, DUAL, LOG_ERR, 0,
  	     "%d peers exist on IIDB %s", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(ANCHORCOUNT, DUAL, LOG_ERR, MSG_TRACEBACK,
  	     "Anchor count negative", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(NBRCHANGE, DUAL, LOG_NOTICE, 0,
       "%s %d: Neighbor %s (%s) is %s: %s");
msgdef_explanation(
	"A neighbor went up or down.");
msgdef_recommended_action(
	"No action is required.");
msgdef_ddts_component("");


msgdef_limit(RDBLEFT, DUAL, LOG_ERR, 0, "Lingering DRDB deleting IIDB, "
	     "dest %s, nexthop %s (%s), origin %s", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(INTERNAL, DUAL, LOG_ERR, MSG_TRACEBACK, "%s %d: Internal Error",
	     MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(UPQUERY, DUAL, LOG_ERR, 0, "%s %d: sendupdate with query set, %s",
	     MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
