/* $Id: msg_appn.c,v 3.2.60.2 1996/09/13 23:45:35 hampton Exp $
 * $Source: /release/112/cvs/Xsys/appn/msg_appn.c,v $
 *------------------------------------------------------------------
 * Log messages for APPN
 *
 * May 1995, Steve Berl
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_appn.c,v $
 * Revision 3.2.60.2  1996/09/13  23:45:35  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/08/28  12:35:35  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  08:37:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:52:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/12  11:47:30  dcalia
 * Rate limit APPN alerts. Provide API to CLSReturnCodeToString and
 * string_getnext (CSCdi28882)
 *
 * Revision 2.1  1995/06/07  20:03:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"
#include "msg_appn.h"


facdef(APPN);
msgdef_section("Advanced Peer-to-Peer Networking error messages");

#define APPN_COMPLAIN_IVL (2*ONESEC)

msgdef(APPNEMERG, APPN, LOG_EMERG, MSG_TRACEBACK | MSG_PROCESS, "%s");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(APPNALERT, APPN, LOG_ALERT, MSG_TRACEBACK | MSG_PROCESS, "%s");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(APPNCRIT, APPN, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS, "%s");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(APPNERR, APPN, LOG_ERR, 0, "%s");
msgdef_explanation(
	"A race condition occurred between two components in the APPN
	software.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(APPNWARNING, APPN, LOG_WARNING, 0, "%s");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(APPNNOTICE, APPN, LOG_NOTICE, 0, "%s");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(APPNINFO, APPN, LOG_INFO, 0, "%s");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(APPNDEBUG, APPN, LOG_DEBUG, 0, "%s");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(APPNLOGMSG, APPN, LOG_INFO, 0, "%s");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(APPNSENDEVENT, APPN, LOG_DEBUG, 0, "%s");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(APPNSENDMSG, APPN, LOG_INFO, 0, "%s");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(APPNSENDREQ, APPN, LOG_NOTICE, 0, "%s");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(APPNETERROR, APPN, LOG_DEBUG, 0, "%s");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(APPNETLOG, APPN, LOG_ERR, 0, "%s");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(MSALERT, APPN, LOG_DEBUG, 0, 
	     "Alert %s issued with sense code %#08X by %s",
	     APPN_COMPLAIN_IVL);
msgdef_explanation(
	"An alertable condition occurred in the local node.");
msgdef_recommended_action(
	"Refer to "Detailed Alert" section of the IBM Systems Network
	Architecture Management Services Reference (order number SC30-3346) for
	a description of ALERTID. Also refer to the "Sense Data" section of IBM
	Systems Network Architecture Formats (order number GA27-3136) for a
	description of SENSE_CODE. Report this message to your technical
	support representative.");
msgdef_ddts_component("");
