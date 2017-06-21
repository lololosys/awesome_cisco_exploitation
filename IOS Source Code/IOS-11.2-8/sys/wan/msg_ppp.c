/* $Id: msg_ppp.c,v 3.2.62.4 1996/09/14 00:10:09 hampton Exp $
 * $Source: /release/112/cvs/Xsys/wan/msg_ppp.c,v $
 *------------------------------------------------------------------
 * msg_ppp.c - Message file for ppp facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_ppp.c,v $
 * Revision 3.2.62.4  1996/09/14  00:10:09  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.62.3  1996/08/28  13:22:07  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.62.2  1996/04/29  09:45:49  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.2.62.1  1996/04/27  06:37:03  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.2.20.1  1996/02/07  20:23:15  syiu
 * Branch: Dial1_branch
 * Add MLP Stackability calls support
 *
 * Revision 3.2  1995/11/17  18:03:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:50:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/25  17:28:52  wmay
 * CSCdi40802:  PPP looped message is annoying - remove it and put message
 * under debug - ppp error or ppp negotiation.
 *
 * Revision 2.1  1995/06/07  23:18:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(PPP);				/* Define the PPP facility */
msgdef_section("Point-to-Point Protocol error messages");


msgdef(CONFNAK, PPP, LOG_WARNING, 0,
       "fsm_rconfnak(%x) - possible CONFNAK loop");
msgdef_explanation(
	"The remote and local PPP cannot agree on a set of options that both
	can perform.");
msgdef_recommended_action(
	"If this message recurs, copy the error message exactly as it appears
	and report it to your technical support representative.");
msgdef_ddts_component("");


msgdef(IPXNET, PPP, LOG_WARNING, 0,
       "mismatched IPX network numbers.  Ours = %x, theirs = %x");
msgdef_explanation(
	"The two ends of a serial link have different IPX network numbers.");
msgdef_recommended_action(
	"Confirm the configuration of both devices.");
msgdef_ddts_component("");


msgdef_limit(NOMAC, PPP, LOG_WARNING, 0,
       "Lex ncp: no free Lex interface found with MAC address %e", ONEMIN);
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(NOEXTTACACS, PPP, LOG_WARNING, 0,
       "ppp TACACS is configured but extended TACACS is not.");
msgdef_explanation(
	"PPP was configured to use the Terminal Access Controller Access
	Control System (TACACS), but extended TACACS was not configured or was
	disabled.");
msgdef_recommended_action(
	"Either issue the no ppp use-tacacs command to stop PPP from using
	TACACS, or issue the tacacs-server extended command to enable extended
	TACACS.");
msgdef_ddts_component("");


msgdef(NOAPPOINT, PPP, LOG_WARNING, 0,
       "MLP Bundle %s not appointed");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(NOREGISTER, PPP, LOG_WARNING, 0,
       "NCP not registered, protocol = %d");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
