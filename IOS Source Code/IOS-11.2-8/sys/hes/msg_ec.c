/* $Id: msg_ec.c,v 3.2.60.2 1996/09/13 23:46:32 hampton Exp $
 * $Source: /release/112/cvs/Xsys/hes/msg_ec.c,v $
 *------------------------------------------------------------------
 * msg_ec.c - Message file for env card
 *
 * Aug 6, 1992, Robert Snyder
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_ec.c,v $
 * Revision 3.2.60.2  1996/09/13  23:46:32  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/08/28  12:45:40  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  09:19:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:41:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:43:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(ENV);				/* Define datalink facility */
msgdef_section("Environmental monitor card error messages");


msgdef(TEMP, ENV, LOG_CRIT, 0, 
       "Ambient temperature measured %s. Shutdown at %s");
msgdef_explanation(
	"The ENV card indicates the temperature it measured and the temperature
	at which it shuts down.");
msgdef_recommended_action(
	"Attempt to resolve the temperature problem. If the problem is not
	resolved, call your technical support representative for assistance.");
       
msgdef_ddts_component("");


msgdef(FLOW, ENV, LOG_CRIT, 0, 
       "Air flow appears marginal.");
msgdef_explanation(
	"A temperature reading indicates poor air flow.");
msgdef_recommended_action(
	"Attempt to resolve the air flow problem. If the problem is not
	resolved, call your technical support representative for assistance.");
       
msgdef_ddts_component("");


msgdef(VOLTAGE, ENV, LOG_CRIT, 0, 
       "%s testpoint measured %s. shutdown margin is %s.");
msgdef_explanation(
	"The ENV card indicates which voltage it is measuring (for example, +5,
	+12, -5, -12), the amount of voltage that it measured, and the
	voltage at which the processor will shut down the system.");
msgdef_recommended_action(
	"Attempt to resolve the voltage problem. If the problem is not
	resolved, call your technical support representative for assistance.");
       
msgdef_ddts_component("");


msgdef(SHUTDOWN, ENV, LOG_ALERT, 0,
       "Environmental Monitor initiated shutdown");
msgdef_explanation(
	"One of the environmental measurements reached SHUTDOWN status. The
	system is about to shut down the processor to prevent damage to the
	hardware.");
msgdef_recommended_action(
	"Attempt to resolve the temperature problem. If the problem is not
	resolved, call your technical support representative for assistance.");
       
msgdef_ddts_component("");
