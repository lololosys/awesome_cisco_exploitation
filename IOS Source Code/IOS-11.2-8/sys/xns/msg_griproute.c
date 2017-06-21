/* $Id: msg_griproute.c,v 3.2.62.2 1996/09/14 00:10:22 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/msg_griproute.c,v $
 *------------------------------------------------------------------
 * msg_griproute.c - Message file for Generic RIP Routing
 *
 * September 23, 1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_griproute.c,v $
 * Revision 3.2.62.2  1996/09/14  00:10:22  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/08/28  13:24:19  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  19:22:14  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:58:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:27:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(GRIP);
msgdef_section("Xerox Network Systems (XNS) Routing Protocol error messages");


msgdef(BADPATHS, GRIP, LOG_ERR, 0,
       "Invalid number of paths (%d) for %q");
msgdef_explanation(
	"An internal inconsistency was detected in the XNS routing table
	structure.");
msgdef_recommended_action(
	"Note the parameters associated with this message and call your
	technical support representative for assistance.");
msgdef_ddts_component("");


msgdef(BADROUTE, GRIP, LOG_CRIT, MSG_TRACEBACK,
       "Error %s route - null table");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");
