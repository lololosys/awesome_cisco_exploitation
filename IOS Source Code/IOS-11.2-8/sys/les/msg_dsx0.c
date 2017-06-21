/* $Id: msg_dsx0.c,v 3.1.2.4 1996/09/13 23:50:25 hampton Exp $
 * $Source: /release/112/cvs/Xsys/les/msg_dsx0.c,v $
 *------------------------------------------------------------------
 * msg_dsx0.c - The message file for CT1 RBS timeslot status
 *
 * July 1997, Tony Wu
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_dsx0.c,v $
 * Revision 3.1.2.4  1996/09/13  23:50:25  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/09/09  20:56:52  jchlin
 * CSCdi66525:  ct1/rbs cannot busy back the CO trunk when theres no free
 * modem
 * Branch: California_branch
 * o       The bugfix will busy dsx0s back to CO thru RBS when there's
 *         no free modem to accept incoming calls.
 * o       The modem pool is changed to have multiple pools for different
 *         types of modems.
 * o       Add hidden debug commands.
 *
 * Revision 3.1.2.2  1996/08/28  12:59:29  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.2.1  1996/07/15  20:36:12  towu
 * CSCdi62430:  CT1 RBS is not enabled
 * Branch: California_branch
 *         o Code cleanup.
 *         o Create msg_dsx0.c for supporting CT1 RBS.
 *
 * Revision 3.1  1996/07/14  23:39:51  towu
 * Create a placeholder for supporting CT1 RBS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(DSX0);
msgdef_section("");


msgdef(RBSLINEUP, DSX0, LOG_NOTICE, 0, "RBS of controller %d timeslot %d is up");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(RBSLINEDOWN, DSX0, LOG_NOTICE, 0, "RBS of controller %d timeslot %d is down");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(RBSLINEBUSYOUT, DSX0, LOG_NOTICE, 0, "RBS of controller %d timeslot %d is busyout");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(RBSLINEUNBUSYOUT, DSX0, LOG_NOTICE, 0, "RBS of controller %d timeslot %d is unbusyout");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
