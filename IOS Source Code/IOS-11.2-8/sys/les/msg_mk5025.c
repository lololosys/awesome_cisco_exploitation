/* $Id: msg_mk5025.c,v 3.3.62.2 1996/09/13 23:50:40 hampton Exp $
 * $Source: /release/112/cvs/Xsys/les/msg_mk5025.c,v $
 *------------------------------------------------------------------
 * msg_mk5025.c - Message file for mk5025 facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_mk5025.c,v $
 * Revision 3.3.62.2  1996/09/13  23:50:40  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.62.1  1996/08/28  12:59:42  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3  1995/11/17  17:42:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:00:37  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:34:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:39:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"
#include "../os/msgtxt_sys.h"


facdef(MK5);				/* Define the facility */
msgdef_section("MK5025 serial controller error messages");


msgdef(BADRING, MK5, LOG_ALERT, 0, msgtxt_badringsize);
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If either of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef_limit(NOMEMORY, MK5, LOG_ALERT, 0, msgtxt_nomemory, MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"The requested operation could not be accomplished because of a low
	memory condition.");
msgdef_recommended_action(
	"Reduce other system activity to ease memory demands. If conditions
	warrant, upgrade to a larger memory configuration.");
msgdef_ddts_component("");


msgdef(INITFAIL, MK5, LOG_ALERT, 0, msgtxt_initfail);
msgdef_explanation(
	"The hardware failed to initialize correctly.");
msgdef_recommended_action(
	"Repair or replace the controller.");
msgdef_ddts_component("");


msgdef(INITUERR, MK5, LOG_ALERT, 0,
       "Unit %d, initialization CSR1_UERR failure, csr1=0x%04x");
msgdef_explanation(
	"The hardware failed to initialize correctly.");
msgdef_recommended_action(
	"Repair or replace the controller.");
msgdef_ddts_component("");


msgdef(INITNOPPRIM, MK5, LOG_ALERT, 0,
   "Unit %d, initialization failure - No CSR1_PPRIM_INIT_CONF, csr1 = 0x%04x");
msgdef_explanation(
	"The hardware failed to initialize correctly.");
msgdef_recommended_action(
	"Repair or replace the controller.");
msgdef_ddts_component("");


msgdef(OUTENCAP, MK5, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS,
       "Unit %d, bad output packet encapsulation: 0x%x");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If either of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(MEMERR, MK5, LOG_ALERT, 0, msgtxt_memoryerror);
msgdef_explanation(
	"A network serial interface detected a hardware problem.");
msgdef_recommended_action(
	"Repair or replace the controller.");
msgdef_ddts_component("");


msgdef(UPRIMERR, MK5, LOG_ERR, 0,
       "Unit %d, user primitive error, csr0=0x%04x, csr1=0x%04x");
msgdef_explanation(
	"A network serial interface detected a hardware problem.");
msgdef_recommended_action(
	"Repair or replace the controller.");
msgdef_ddts_component("");


msgdef(PLOSTERR, MK5, LOG_ERR, 0,
       "Unit %d, provider primitive lost, csr0=0x%04x, csr1=0x%04x");
msgdef_explanation(
	"A network serial interface detected a hardware problem.");
msgdef_recommended_action(
	"Repair or replace the controller.");
msgdef_ddts_component("");


msgdef(PPRIMERR, MK5, LOG_ERR, 0,
       "Unit %d, unexpected provider primitive, csr0=0x%04x, csr1=0x%04x");
msgdef_explanation(
	"A network serial interface detected a hardware problem.");
msgdef_recommended_action(
	"Repair or replace the controller.");
msgdef_ddts_component("");


msgdef(SPURPPRIMERR, MK5, LOG_ERR, 0,
       "Unit %d, spurious provider primitive interrupt, csr0=0x%04x, csr1=0x%04x");
msgdef_explanation(
	"A network serial interface detected a hardware problem.");
msgdef_recommended_action(
	"Repair or replace the controller.");
msgdef_ddts_component("");


msgdef(LINEFLAP, MK5, LOG_NOTICE, 0,
       "Unit %d excessive modem control changes");
msgdef_explanation(
	"Too many modem control interrupts have been received. The port was
	disabled to prevent excessive use of the CPU.");
msgdef_recommended_action(
	"Check the cable on the serial port.");
msgdef_ddts_component("");
