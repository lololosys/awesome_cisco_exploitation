/* $Id: msg_rip.c,v 3.2.60.2 1996/09/13 23:50:06 hampton Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/msg_rip.c,v $
 *------------------------------------------------------------------
 * msg_rip.c - Message file for rip facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_rip.c,v $
 * Revision 3.2.60.2  1996/09/13  23:50:06  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/08/28  12:53:02  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  17:35:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:09:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:08:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(RIP);
msgdef_section("IP Routing Information Protocol error message");


msgdef(NOSOCKET, RIP, LOG_ERR, 0, "Unable to open socket");
msgdef_explanation(
	"The requested operation could not be accomplished because of a low
	memory condition.");
msgdef_recommended_action(
	"Reduce other system activity to ease memory demands. If conditions
	warrant, upgrade to a larger memory configuration.");
msgdef_ddts_component("");
