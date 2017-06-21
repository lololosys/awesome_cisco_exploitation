/* $Id: msg_pas.c,v 3.3.32.3 1996/09/14 00:01:27 hampton Exp $
 * $Source: /release/112/cvs/Xsys/pas/msg_pas.c,v $
 *------------------------------------------------------------------
 * msg_pas.c - Declaration of platform independent PA messages
 *
 * January 1995, David Getchell 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_pas.c,v $
 * Revision 3.3.32.3  1996/09/14  00:01:27  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.32.2  1996/08/28  13:07:29  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.32.1  1996/04/29  19:27:51  mbeesley
 * CSCdi55915:  One broken PA causes all interfaces to be ignored
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/12  21:52:48  jkuzma
 * CSCdi46358:  VIP1 PA mngmt and DMA interrupt stacks swapped
 *              General debug of PA management interrupts
 *
 * Revision 3.2  1995/11/17  18:55:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:51  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:33:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* 
 * Include files
 */
#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(PA);				/* Define the facility */
msgdef_section("Port adapter error messages");


msgdef(BADPABAY, PA, LOG_CRIT, 0,
       "Out of range PCI Port Adaptor bay %d");
msgdef_explanation(
	"The software specified an out-of-range port adapter bay.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADINTERFACE, PA, LOG_CRIT, 0,
       "Out of range PCI Port Adaptor interface, %d on bay %d");
msgdef_explanation(
	"The software specified an out-of-range port adapter interface.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(UNDEFPA, PA, LOG_CRIT, 0,
       "Undefined Port Adaptor type %d in bay %d");
msgdef_explanation(
	"The software already has a driver for that port adapter type in the
	specified bay.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(UNDEFPABRIDGE, PA, LOG_CRIT, 0,
       "Unsupported Port Adaptor Bridge %#x in bay %d");
msgdef_explanation(
	"The software does not have a driver for that port adapter protocol
	control information bridge in the specified bay.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(PABRIDGE, PA, LOG_CRIT, 0,
       "Failed to config bridge for PA %d");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(BADIDB, PA, LOG_CRIT, 0, "PA interface idb incorrect, %#x");
msgdef_explanation(
	"The port adapter system control block (PASCB) data structure indicates
	a zero interface descriptor block (IDB).");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADPASCB, PA, LOG_CRIT, 0, "PA interface pascb incorrect, %#x");
msgdef_explanation(
	"The port adapter system control block (PASCB) data structure was
	incorrectly set in the command data structure.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(RUPTCONFLICT, PA, LOG_EMERG, 0,
       "Conflict detected when trying to install %s interrupt handler, bay %d.");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
