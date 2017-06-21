/* $Id: msg_bsc.c,v 3.3.54.2 1996/09/13 23:45:53 hampton Exp $
 * $Source: /release/112/cvs/Xsys/bstun/msg_bsc.c,v $
 *------------------------------------------------------------------
 * Message def's for BSTUN
 *
 * March 1995, Andrew R. Rothwell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_bsc.c,v $
 * Revision 3.3.54.2  1996/09/13  23:45:53  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.54.1  1996/08/28  12:37:52  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3  1995/11/21  08:52:25  jbalestr
 * CSCdi42960:  bstun keepalive count
 * Added bstun keepalive interval and count
 *
 * Revision 3.2  1995/11/17  08:49:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/16  06:24:52  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:14:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(BSC);				/* Define the BSC facility */
msgdef_section("Binary Synchronous Communications (Bisync) protocol error
	messages");


msgdef(BADPEERTYPE, BSC, LOG_ERR, 0, "Peer %s is incorrect type");
msgdef_explanation(
	"This message occurs only when BSC local-ack is configured. It
	indicates the serial interfaces that are using the BSTUN tunnel are
	configured incorrectly, that is, both have been configured as a
	secondary.");
msgdef_recommended_action(
	"Reconfigure the serial interfaces correctly to be a primary and
	secondary pair.");
msgdef_ddts_component("");


msgdef(NOMEM, BSC, LOG_CRIT, 0, "No buffers available on interface %s.");
msgdef_explanation(
	"This message indicates that memory in the router was exhausted.");
msgdef_recommended_action(
	"Perform diagnostics on the router to locate the subsystem or interface
	responsible for consuming memory. The only method that will clear this
	situation is to reload the router.");
msgdef_ddts_component("");


msgdef(BADLINESTATE, BSC, LOG_ERR, 0,
       "Line state Tx when receiving non-Tx event: %d.");
msgdef_explanation(
	"An unexpected receive occurred during a transmission.");
msgdef_recommended_action(
	"Verify that the line has been consistently defined in regards to the
	carrier. If a message report was sent, hard-on stop and start the
	bisynchronous interface in question.");
msgdef_ddts_component("");
