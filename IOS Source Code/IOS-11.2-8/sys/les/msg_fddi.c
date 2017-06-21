/* $Id: msg_fddi.c,v 3.3.62.2 1996/09/13 23:50:29 hampton Exp $
 * $Source: /release/112/cvs/Xsys/les/msg_fddi.c,v $
 *------------------------------------------------------------------
 * Message file for FDDI interfaces facility
 *
 * december 1991, chris shaker
 * june 1992, steve elias
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: msg_fddi.c,v $
 * Revision 3.3.62.2  1996/09/13  23:50:29  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.62.1  1996/08/28  12:59:32  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3  1995/11/17  17:42:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:00:29  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:33:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:38:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __MSG_FDDI_C__
#define __MSG_FDDI_C__

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"
#include "../os/msgtxt_sys.h"


facdef(FDDI);
msgdef_section("Fiber Distributed Data Interface error messages");


msgdef(BADUNIT, FDDI, LOG_ERR, 0, msgtxt_badunit);
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef_limit(NOMEMORY, FDDI, LOG_ALERT, 0, msgtxt_nomemory, MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"The FDDI Management Information Base/Station Management (MIB/SMT)
	processing functions could not initialize due to insufficient
	memory.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADSLOT, FDDI, LOG_ERR, 0, "Unit %d, Slot %d does not exist.");
msgdef_explanation(
	"The main processor detects an invalid slot location for one of the
	network interface cards. This should never happen.");
msgdef_recommended_action(
	"Call your technical support representative about obtaining a
	replacement card.");
msgdef_ddts_component("");


msgdef(NOCAM, FDDI, LOG_ERR, 0,
       "Unit %d, No CAM exists for this interface.");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(FDDIFAIL, FDDI, LOG_ERR, 0, "Interface %s, %s, %s = %#x");
msgdef_explanation(
	"The FDDI interface failed.");
msgdef_recommended_action(
	"Save the output of the error message and call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(FDDIFAIL2, FDDI, LOG_ERR, 0, "Interface %s, %s, %s = %#x, %s = %#x");
msgdef_explanation(
	"The FDDI interface failed. This is the second line of the preceding
	FDDIFAIL message. This line contains debug information.");
msgdef_recommended_action(
	"Save the output of the error message and call your technical support
	representative for assistance.");
msgdef_ddts_component("");

#endif /* MSG_FDDI_C */
