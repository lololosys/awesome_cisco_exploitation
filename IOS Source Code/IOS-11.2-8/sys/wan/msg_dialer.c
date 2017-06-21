/* $Id: msg_dialer.c,v 3.2.62.4 1996/09/14 00:10:04 hampton Exp $
 * $Source: /release/112/cvs/Xsys/wan/msg_dialer.c,v $
 *------------------------------------------------------------------
 * msg_dialer.c - Message file for Dialer support
 *
 * April, 1994, Joel P. Bion
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_dialer.c,v $
 * Revision 3.2.62.4  1996/09/14  00:10:04  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.62.3  1996/08/28  13:22:02  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.62.2  1996/07/23  13:31:39  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.2.62.1  1996/04/27  06:36:59  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.2.20.1  1996/04/23  00:24:35  suresh
 * Branch: Dial1_branch
 * Add Dialer Profiles support to Dial1_branch
 *
 * Revision 3.2  1995/11/17  18:03:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:50:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:18:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(DIALER);
msgdef_section("Dial-on-demand routing (DDR) error messages");


msgdef(NOSWIDB, DIALER, LOG_CRIT, 0,
       "No software interface associated with hw interface %s");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Call your technical support representative and report the error
	message, the system version, and the router configuration. (Use the
	show version command to obtain the software version.)");
msgdef_ddts_component("");


msgdef(NULLPTR, DIALER, LOG_CRIT, 0,
       "Unexpected null pointer in %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(BIND, DIALER, LOG_INFO, 0,
       "Interface %s bound to profile %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(UNBIND, DIALER, LOG_INFO, 0,
       "Interface %s unbound from profile %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
