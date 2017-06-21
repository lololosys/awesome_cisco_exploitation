/* $Id: msg_iproute.c,v 3.2.60.2 1996/09/13 23:50:02 hampton Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/msg_iproute.c,v $
 *------------------------------------------------------------------
 * msg_iproute.c - Message file for iproute facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_iproute.c,v $
 * Revision 3.2.60.2  1996/09/13  23:50:02  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/08/28  12:52:58  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  17:35:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:08:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:08:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(IPRT);
msgdef_section("Internet Protocol routing error messages");


msgdef_limit(NOMEMORY, IPRT, LOG_ERR, 0,
	"No memory available for %s", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"The requested operation failed because of a low memory condition.");
msgdef_recommended_action(
	"Reduce other system activity to ease memory demands. If conditions
	warrant, upgrade to a larger memory configuration.");
msgdef_ddts_component("");


msgdef(COMPRESS, IPRT, LOG_CRIT, MSG_PROCESS | MSG_TRACEBACK,
       "Bad route_compress() call, sdb= %x");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef_limit(TOOMANYNETS, IPRT, LOG_ERR, 0,
       "Too many networks in %s %d (limit %d)", ONEMIN);
msgdef_explanation(
	"Too many network subcommands have been entered for this routing
	protocol.");
msgdef_recommended_action(
	"Advisory message only. No action required.");
msgdef_ddts_component("");


msgdef_limit(PATHIDX, IPRT, LOG_ERR, MSG_PROCESS | MSG_TRACEBACK,
	"Bad path index of %d for %i, %d max", ONEMIN);
msgdef_explanation(
	"A software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(BADCOMLIST, IPRT, LOG_ERR, MSG_PROCESS | MSG_TRACEBACK,
	"Community list check with bad list %d", 10*ONESEC);
msgdef_explanation(
	"A user specified an invalid BGP community list number.");
msgdef_recommended_action(
	"Check the system configuration to make sure that the community list
	specified falls within the valid range.");
msgdef_ddts_component("");
