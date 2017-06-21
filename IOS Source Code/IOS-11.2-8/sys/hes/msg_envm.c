/* $Id: msg_envm.c,v 3.2.60.2 1996/09/13 23:46:34 hampton Exp $
 * $Source: /release/112/cvs/Xsys/hes/msg_envm.c,v $
 *------------------------------------------------------------------
 * msg_envm.c - Message file for envm function
 *
 * Oct 6, 1992, Gary Kramling
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_envm.c,v $
 * Revision 3.2.60.2  1996/09/13  23:46:34  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/08/28  12:45:43  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  09:19:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:41:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:43:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(ENVM);				/* Define datalink facility */
msgdef_section("Environmental monitor error messages for the Cisco 7000
	series");

msgdef(TEMP, ENVM, LOG_CRIT, 0, 
       "%s temperature has reached %s level at %d(C)");
msgdef_explanation(
	"The temperature parameter shown reached a warning or critical level
	and is approaching or approached a condition that is outside the
	acceptable range.");
msgdef_recommended_action(
	"Attempt to resolve the temperature problem. If the problem is not
	resolved, call your technical support representative for assistance.");
msgdef_ddts_component("");


msgdef(VOLTAGE, ENVM, LOG_CRIT, 0, 
       "%s volts has reached %s level at %s(V)");
msgdef_explanation(
	"The voltage parameter shown reached a critical level and is now out of
	specification.");
msgdef_recommended_action(
	"Verify the failure and call your technical support representative for
	repair or replacement.");
msgdef_ddts_component("");


msgdef(SUPPLY, ENVM, LOG_CRIT, 0, 
       "%s Power Supply is Non-Operational");
msgdef_explanation(
	"A power supply is not working or is not turned on.");
msgdef_recommended_action(
	"Check that the power supply is turned on; if it is turned on and still
	does not operate, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(SHUTDOWN, ENVM, LOG_ALERT, 0,
       "Environmental Monitor initiated shutdown");
msgdef_explanation(
	"The system reached a SHUTDOWN temperature level, and the router is
	being shut down to avoid any damage.");
msgdef_recommended_action(
	"Attempt to resolve the temperature problem. If the problem is not
	resolved, call your technical support representative for assistance.");
msgdef_ddts_component("");


msgdef(FAN, ENVM, LOG_CRIT, 0,
       "Fan array has reached %s level.");
msgdef_explanation(
	"There might be a hardware problem with the fan array. If the error
	condition continues for two minutes, the router automatically shuts
	down to prevent damage to router components.");
msgdef_recommended_action(
	"If the system shuts down, replace the fan tray. Copy the error message
	exactly as it appears, and report it to your technical support
	representative.");
msgdef_ddts_component("");


msgdef(FANOK, ENVM, LOG_CRIT, 0,
       "Fan array has recovered.");
msgdef_explanation(
	"One or more of the variable speed fans on the 7010 recovered from a
	prior failure.");
msgdef_recommended_action(
	"Informational message only. No action required.");
msgdef_ddts_component("");
