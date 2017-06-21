/* $Id: msg_sysmgt.c,v 3.2.60.3 1996/09/13 23:46:42 hampton Exp $
 * $Source: /release/112/cvs/Xsys/hes/msg_sysmgt.c,v $
 *------------------------------------------------------------------
 * msg_sysmgt.c - Error message file for system management RPC.
 *
 * April 1995, Mike Otto
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_sysmgt.c,v $
 * Revision 3.2.60.3  1996/09/13  23:46:42  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/08/28  12:45:53  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.60.1  1996/03/18  19:42:56  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  21:47:53  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  09:19:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:41:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:43:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(SYSMGT_RPC);
msgdef_section("System Management error messages");


msgdef(IPC_ERROR, SYSMGT_RPC, LOG_ERR, 0,
      "SYSMGT RPC - IPC %s %s failed (%s)");
msgdef_explanation(
	"System Management Remote Procedure Call operation failure. Request for
	information from the CIP card will not be processed.");
msgdef_recommended_action(
	"Report this error to your technical support representative.");
msgdef_ddts_component("");


msgdef(NETMGT_EVENT, SYSMGT_RPC, LOG_ERR, 0,
      "SYSMGT RPC - NETMGT Event: %s (%d)");
msgdef_explanation(
	"System Management Remote Procedure Call Network management event error
	detected.");
msgdef_recommended_action(
	"Report this error to your technical support representative.");
msgdef_ddts_component("");
