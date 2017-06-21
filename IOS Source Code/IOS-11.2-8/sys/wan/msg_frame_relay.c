/* $Id: msg_frame_relay.c,v 3.1.2.2 1996/09/14 00:10:06 hampton Exp $
 * $Source: /release/112/cvs/Xsys/wan/msg_frame_relay.c,v $
 *------------------------------------------------------------------
 * M S G  _ F R A M E _ R E L A Y . C
 *
 * August 1996, Nick Thille
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_frame_relay.c,v $
 * Revision 3.1.2.2  1996/09/14  00:10:06  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/08/28  13:22:04  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1  1996/08/25  01:18:29  thille
 * Placeholders for cleanup of message files and externs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(FR_LMI);
msgdef_section("");


msgdef(LMITIMER, FR_LMI, LOG_NOTICE, 0, "Interface %s - LMI TIMER %s failed");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");



facdef(FR);
msgdef_section("Frame Relay error messages");


msgdef(DLCICHANGE, FR, LOG_NOTICE, 0,
       "Interface %s - DLCI %d state changed to %s");
msgdef_explanation(
	"The state of the Frame Relay PVC specified by the DLCI changed.");
msgdef_recommended_action(
	"Notification message only. No action required.");
msgdef_ddts_component("");


msgdef(NULL_INT, FR, LOG_ERR, 0,
       "Null output interface. Broadcast packet dropped!");
msgdef_explanation("Output interface is NULL, broadcast packet is dropped!");
msgdef_recommended_action(
	"Inform Cisco technical support representative");
msgdef_ddts_component("fr");

msgdef(INCORRECT_INT, FR, LOG_ERR, 0,
       "Incorrect output (sub)interface, broadcast packet dropped! ");
msgdef_explanation("Output (sub)interface specified in packet is incorrect, broadcast packet is dropped");
msgdef_recommended_action(
	"Inform Cisco technical support representative");
msgdef_ddts_component("fr");
