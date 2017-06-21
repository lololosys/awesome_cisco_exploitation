/* $Id: msg_decnet.c,v 3.2.60.2 1996/09/13 23:46:02 hampton Exp $
 * $Source: /release/112/cvs/Xsys/decnet/msg_decnet.c,v $
 *------------------------------------------------------------------
 * msg_decnet.c - Message file for decnet facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_decnet.c,v $
 * Revision 3.2.60.2  1996/09/13  23:46:02  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/08/28  12:40:14  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  08:59:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:17:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/18  19:40:25  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:23:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(DNET);
msgdef_section("DECnet error messages");

msgdef(MAPCON, DNET, LOG_WARNING, 0,
       "Map entry %d.%d conflicts with adjacency to %d.%d");
msgdef_explanation(
	"Your DECnet configuration is incorrect. A host that is specified as
	nonlocal is present on your local network.");
msgdef_recommended_action(
	"Correct the configuration. Call your technical support representative
	if you need assistance.");
msgdef_ddts_component("");


msgdef(HEARSELF, DNET, LOG_ERR, 0,
       "Hello type %x for my address from %d.%d via %s");
msgdef_explanation(
	"The system is receiving its own DECnet packets. Either a serial line
	is looped back or another host with the same DECnet address is already
	present on the LAN.");
msgdef_recommended_action(
	"Check the serial lines (if present) and the DECnet configuration.");
msgdef_ddts_component("");
