/* $Id: msg_vip.c,v 3.2.62.2 1996/09/14 00:02:12 hampton Exp $
 * $Source: /release/112/cvs/Xsys/src-vip/msg_vip.c,v $
 *------------------------------------------------------------------
 * msg_vip.c - Declaration of VIP error messages
 *
 * March 1995, David Getchell 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_vip.c,v $
 * Revision 3.2.62.2  1996/09/14  00:02:12  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/08/28  13:15:15  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  18:51:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:25  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:59:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(VIP);				/* Define the facility */
msgdef_section("Versatile Interface Processor error messages");


msgdef(NOICBS, VIP, LOG_CRIT, 0, 
       "No MEMD icbs left for PA %d Interface %d");
msgdef_explanation(
	"No Switch Processor (SP) interrupt control block structure was
	associated with the port adaptor or the interface.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(NODISPATCH, VIP, LOG_ERR, 0, 
       "Dispatch vector Null, cmd=%d, dintf=%d");
msgdef_explanation(
	"No command dispatch vector was found for the specified interface.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADMALUCMD, VIP, LOG_ERR, 0, 
       "Unsupported MALU command %d, arg=%#x, pascb=%#x");
msgdef_explanation(
	"The Route Processor (RP) or the Route/Switch Processor (RSP) passed
	down a command that the software was not prepared to handle.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(CMDNOPASCB, VIP, LOG_ERR, 0, 
       "PASCB Null for command %d, arg=%#x");
msgdef_explanation(
	"The Route Processor (RP) or the Route/Switch Processor (RSP) passed
	down a command that the software was unprepared to handle.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(UNDEFIDBTYPE, VIP, LOG_ERR, 0, 
       "Undefined idb type %d");
msgdef_explanation(
	"This message is sent from the VIP kernel when it sees a media type
	other than Ethernet, Token-Ring, or serial. These are the media types
	supported by Cisco IOS Release 11.2 on the VIP. The message might be
	caused by having an older version of the software running on a device
	with a new VIP port adapter.");
msgdef_recommended_action(
	"Check whether the version of the software being run supports the VIP
	port adapter. If an old software version does not appear to be the
	problem, copy the error message exactly as it appears and report it to
	your technical support representative.");
msgdef_ddts_component("");
