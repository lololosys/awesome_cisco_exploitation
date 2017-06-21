/* $Id: msg_mci.c,v 3.3.60.2 1996/09/13 23:46:36 hampton Exp $
 * $Source: /release/112/cvs/Xsys/hes/msg_mci.c,v $
 *------------------------------------------------------------------
 * msg_mci.c - Message file for mci facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_mci.c,v $
 * Revision 3.3.60.2  1996/09/13  23:46:36  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.60.1  1996/08/28  12:45:46  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3  1995/11/17  09:19:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:29:09  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:41:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:43:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(MCI);				/* Define the MCI facility */
msgdef_section("Multiport Communications Interface error messages");


msgdef(TESTFAIL, MCI, LOG_WARNING, 0, "Unit %d failed %s test, skipping");
msgdef_explanation(
	"A hardware component failed an internal diagnostic test.");
msgdef_recommended_action(
	"You need to replace the malfunctioning component. Copy the error
	message exactly as it appears, and report it to your technical support
	representative.");
msgdef_ddts_component("");


msgdef(NOCOOKIE, MCI, LOG_WARNING, 0, "MCI controller %d missing configuration data - disabled");
msgdef_explanation(
	"The configuration PAL is missing.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(INPUTERR, MCI, LOG_NOTICE, 0,
       "Interface %s excessive input error rate");
msgdef_explanation(
	"The input error rate was so high that the interface was temporarily
	disabled. The interface will be automatically reenabled in 30
	seconds.");
msgdef_recommended_action(
	"If this message recurs, check the communication lines.");
msgdef_ddts_component("");


msgdef(SETUPERR, MCI, LOG_ERR, 0,
       "Unit %d, Error (%-04x) on setup, index %-04x, restarting controller -- mci_interrupt()");
msgdef_explanation(
	"A hardware device did not respond appropriately to a request. This
	generally indicates a hardware problem.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(RXINDEX, MCI, LOG_ERR, 0, "Unit %d, invalid RX index %d");
msgdef_explanation(
	"The MCI did not answer a Multibus request. This generally indicates a
	hardware problem.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(OBSOLETE, MCI, LOG_NOTICE, 0,
       "Obsolete MCI firmware: can't route %s and bridge simultaneously");
msgdef_explanation(
	"The firmware on your MCI controller card is out of date.");
msgdef_recommended_action(
	"Upgrade your MCI firmware.");
msgdef_ddts_component("");
