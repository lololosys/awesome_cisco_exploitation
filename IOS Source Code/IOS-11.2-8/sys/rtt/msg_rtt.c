/* $Id: msg_rtt.c,v 1.1.4.5 1996/09/14 00:01:32 hampton Exp $
 * $Source: /release/112/cvs/Xsys/rtt/msg_rtt.c,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor error messages.
 *
 * Apr 1996, Larry Metzger
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_rtt.c,v $
 * Revision 1.1.4.5  1996/09/14  00:01:32  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 1.1.4.4  1996/08/28  13:08:09  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 1.1.4.3  1996/06/07  22:25:51  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.2  1996/06/04  18:28:16  lmetzger
 * CSCdi59459:  See CSCdi589xx, xx = 29,30,31,32,33,35,36,47
 * Branch: California_branch
 * - Group of defects that are interdependent and tested.
 *
 * Revision 1.1.4.1  1996/05/17  11:40:49  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.1  1996/05/14  06:20:22  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 1.1  1996/05/14  00:34:56  lmetzger
 * Placeholder for Response Time Reporter code review updates
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(RTT);
msgdef_section("");


msgdef(GlobalInitFailed, RTT, LOG_ERR, 0,
       "RTR 0: %s - Failed to initialize local storage");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(BufferInitFailed, RTT, LOG_ERR, 0,
       "RTR: Initialization of buffers failed!");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(SemaphoreInitFailed, RTT, LOG_ERR, 0,
       "RTR: Initialization of rttMonConfigSemaphore failed!");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(MissingInit, RTT, LOG_ERR, 0,
       "%%RTR: required initialization failed, thus cannot configure");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(BadLoc, RTT, LOG_ERR, 0,
       "%%RTR: We should not have gotten to rttmon_config_entry_command");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(MissingEchoStruct, RTT, LOG_ERR, 0,
       "%%RTR: failed to get EchoAdmin Struct");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
