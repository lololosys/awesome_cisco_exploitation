/* $Id: msg_rcmd.c,v 3.2.62.3 1996/09/14 00:09:38 hampton Exp $
 * $Source: /release/112/cvs/Xsys/tcp/msg_rcmd.c,v $
 *------------------------------------------------------------------
 * msg_rcmd.c -- Message file for rcmd
 *
 * April 1994, Marciano Pitargue
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_rcmd.c,v $
 * Revision 3.2.62.3  1996/09/14  00:09:38  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/08/28  13:17:02  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.62.1  1996/04/15  21:23:26  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:47:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:35:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/20  07:19:25  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  23:05:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(RCMD);
msgdef_section("Remote command error messages");


/* 
 * Following messages are in all images: 
 */
msgdef(RSHPORTATTEMPT, RCMD, LOG_WARNING, 0,
       "Attempted to connect to RSHELL from %i");
msgdef_explanation(
	"An attempt was made to connect to a router through the rshell port
	(514), but the router was not configured as an RSH or RCP server.");
msgdef_recommended_action(
	"Configure an RSH or RCP server.");
msgdef_ddts_component("");


msgdef(RCPATTEMPTED, RCMD, LOG_WARNING, 0,
       "Remote copy from %s at %s denied");
msgdef_explanation(
	"An attempt was made to connect to a router through RCP, but the router
	was not configured as an RCP server.");
msgdef_recommended_action(
	"Configure an RCP server.");
msgdef_ddts_component("");


msgdef(RSHATTEMPTED, RCMD, LOG_WARNING, 0,
       "Remote shell from %s at %s denied");
msgdef_explanation(
	"An attempt was made to connect to a router through rsh, but the router
	was not configured as an rsh server.");
msgdef_recommended_action(
	"Configure an rsh server.");
msgdef_ddts_component("");


msgdef(RCMDDNSFAIL, RCMD, LOG_WARNING, 0,
       "DNS hostname/ip address mismatch.  %i unknown to DNS");
msgdef_explanation(
	"The IP address for an incoming rcmd request is not registered with
	DNS.");
msgdef_recommended_action(
	"Add the IP address to DNS.");
msgdef_ddts_component("");
