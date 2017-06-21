/* $Id: msg_tunnel.c,v 3.3.60.2 1996/09/13 23:49:33 hampton Exp $
 * $Source: /release/112/cvs/Xsys/if/msg_tunnel.c,v $
 *------------------------------------------------------------------
 * msg_tunnel.c - Message file for tunnel facility
 *
 * August 11, 1992 - Paul Traina
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_tunnel.c,v $
 * Revision 3.3.60.2  1996/09/13  23:49:33  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.60.1  1996/08/28  12:48:29  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3  1995/11/17  09:30:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:38:28  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:51:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:53:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(TUN);				/* Define the tunnel facility */
msgdef_section("Tunnel error messages");


msgdef(RECURDOWN, TUN, LOG_WARNING, 0,
       "%s temporarily disabled due to recursive routing");
msgdef_explanation(
	"When routing the same payload protocol over a carrier protocol
	(tunneling IP over IP, for example), it is possible to misconfigure
	your network so that you will try to route to the tunnel destination
	address via the tunnel. This is usually caused by a misconfiguration of
	the tunnel or a temporary instability due to route flapping elsewhere
	in your network. It is important to take steps to ensure that routing
	information between the carrier networks and the payload networks is
	not allowed to mix. If the router discovers a recursive routing
	problem, it will temporarily shut down the tunnel interface for a few
	minutes so that the situation that is causing this problem can resolve
	itself as routing protocols converge. If the problem is caused by
	misconfiguration, the link may oscillate indefinitely.");
msgdef_recommended_action(
	"No action required.");
msgdef_ddts_component("");
