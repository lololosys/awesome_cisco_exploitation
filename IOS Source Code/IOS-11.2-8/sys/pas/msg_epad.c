/* $Id: msg_epad.c,v 3.1.64.3 1996/09/14 00:01:22 hampton Exp $
 * $Source: /release/112/cvs/Xsys/pas/msg_epad.c,v $
 *------------------------------------------------------------------
 * msg_epad.c - Encryption Port Adaptor Driver specific messages.
 *
 * August 1995, Paulina D. Tran
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_epad.c,v $
 * Revision 3.1.64.3  1996/09/14  00:01:22  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.64.2  1996/08/28  13:07:17  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.64.1  1996/04/19  15:26:17  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.1.2.3  1996/04/16  21:54:59  ptran
 * EPA driver clean up. Removed statistic counters used for debugging
 * purposes.
 * Removed parser commands used for testing. Modified to compile in
 * California
 * branch.  Incorporate code review comments.
 *
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.2  1996/02/02  23:14:55  ptran
 * Commit first release of EPA firmware.
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.1  1995/12/06  21:49:06  rmontino
 * Branch: IosSec_branch
 *
 * Revision 3.1  1995/11/09  12:58:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/23  02:27:42  ptran
 * Placeholder file for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(EPAD);				/* Define the facility */
msgdef_section("");


msgdef(ERROR, EPAD, LOG_ALERT, 0, "%s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(WARNING, EPAD, LOG_ALERT, 0, "%s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(INITFAIL, EPAD, LOG_ALERT, 0, "%s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
