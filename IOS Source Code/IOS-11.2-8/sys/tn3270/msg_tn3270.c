/* $Id: msg_tn3270.c,v 3.1.2.2 1996/09/14 00:09:45 hampton Exp $
 * $Source: /release/112/cvs/Xsys/tn3270/msg_tn3270.c,v $
 *------------------------------------------------------------------
 * M S G  _ T N 3 2 7 0 . C
 *
 * August 1996, Nick Thille
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_tn3270.c,v $
 * Revision 3.1.2.2  1996/09/14  00:09:45  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/08/28  13:17:38  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1  1996/08/25  01:18:01  thille
 * Placeholders for cleanup of message files and externs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(TN3270);
msgdef_section("TN3270 error messages relate to the TN3270 protocol, a
	variation of the IBM VT (virtual terminal) protocol.");


msgdef(INP_OVF1, TN3270, LOG_CRIT, MSG_PROCESS | MSG_TRACEBACK,
       "Input Buffer overflow");
msgdef_explanation(
	"A problem in TN3270 protocol was detected. Eight KB (8192) bytes of
	data were received without a TN3270 end-of-packet indicator. The TN3270
	buffer overflowed.");
msgdef_recommended_action(
	"Copy this error message exactly as it appears, and contact your
	technical support representative for assistance.");
msgdef_ddts_component("");
