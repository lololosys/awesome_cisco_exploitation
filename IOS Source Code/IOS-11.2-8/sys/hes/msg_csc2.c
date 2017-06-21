/* $Id: msg_csc2.c,v 3.2.60.2 1996/09/13 23:46:28 hampton Exp $
 * $Source: /release/112/cvs/Xsys/hes/msg_csc2.c,v $
 *------------------------------------------------------------------
 * msg_csc2.c - Message file for csc2 facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_csc2.c,v $
 * Revision 3.2.60.2  1996/09/13  23:46:28  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/08/28  12:45:34  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  09:19:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:41:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:43:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(CSC2);
msgdef_section("CPU card error message");


msgdef(BREAK, CSC2, LOG_WARNING, 0,
       "AUX port hung because 'Transmit Break' would not clear");
msgdef_explanation(
	"The Universal Asynchronous Receiver/Transmitter (UART) in the console
	serial interface is malfunctioning.");
msgdef_recommended_action(
	"Service the console serial port hardware.");
msgdef_ddts_component("");
