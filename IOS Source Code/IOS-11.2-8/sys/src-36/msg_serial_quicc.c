/* $Id: msg_serial_quicc.c,v 3.2.62.3 1996/09/14 00:01:47 hampton Exp $
 * $Source: /release/112/cvs/Xsys/src-36/msg_serial_quicc.c,v $
 *------------------------------------------------------------------
 * msg_quicc_serial.c - Message file for QUICC serial facility.
 *
 * August 22, 1995, John Ng
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_serial_quicc.c,v $
 * Revision 3.2.62.3  1996/09/14  00:01:47  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/08/28  13:10:29  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.62.1  1996/06/12  19:56:39  jng
 * CSCdi60260:  Async catwoman commit
 * Branch: California_branch
 * Async Catwoman commit into California
 *
 * Revision 3.2  1995/11/17  18:39:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:19:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:26:22  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/22  18:49:02  jng
 * Add place holder for 11.0
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"
#include "../os/msgtxt_sys.h"


facdef(QUICC_SERIAL);
msgdef_section("MC68360 Quad Integrated Communications Controller error
	messages");


msgdef(INITFAIL, QUICC_SERIAL, LOG_ALERT, 0, msgtxt_initfail);
msgdef_explanation(
	"The serial interface controller of the QUICC chip could not be
	initialized or started for operation. There is probably a hardware
	problem.");
msgdef_recommended_action(
	"Power cycle the system. If the error reoccurs, replace the unit.");
msgdef_ddts_component("");


msgdef(UNDERFLO, QUICC_SERIAL, LOG_ERR, 0, "Unit %d, underflow error");
msgdef_explanation(
	"While transmitting a frame, the serial controller chip's local buffer
	received insufficient data because data could not be transferred to the
	chip fast enough to keep pace with its output rate. Normally, such a
	problem is temporary, depending on transient peak loads within the
	system.");
msgdef_recommended_action(
	"The system should recover. No action is required.

	If the problem reccurs, it indicates a hardware error that might be
	related to data traffic patterns. Copy the error message exactly as it
	appears, and report it to your technical support representative.");
msgdef_ddts_component("");


msgdef(CTSLOST, QUICC_SERIAL, LOG_ERR, 0, "Unit %d, Clear to Send Lost");
msgdef_explanation(
	"The Clear To Send (CTS) input signal on a data terminal equipment
	(DTE) serial interface became inactive while transmitting a frame. This
	problem stems from a communication line failure or cable
	disconnection.");
msgdef_recommended_action(
	"Check the serial interface cable and or communication equipment such
	as the channel service unit/data service unit (CSU/DSU).");
msgdef_ddts_component("");


msgdef(LINEFLAP, QUICC_SERIAL, LOG_NOTICE, 0,
       "Unit %d, excessive modem control changes");
msgdef_explanation(
	"The system received too many modem control signal interrupts. Modem
	control signals are hardware handshake signals between data terminal
	equipment (DTE) and data communications equipment (DCE). The signals
	include either a data carrier detect (DCD) or a data set ready (DSR),
	or both a DCD and a DSR.");
msgdef_recommended_action(
	"Check the serial interface cable. The error can occur if the cable is
	disconnected or has come loose and is picking up noise. If the cable
	appears to be connected correctly, check the equipment connected to the
	cable.");
msgdef_ddts_component("");



facdef(QUICC_ASYNC);
msgdef_section("");


msgdef(CTSLOST, QUICC_ASYNC, LOG_ERR, 0, "Unit %d, Clear to Send Lost");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(INITFAIL, QUICC_ASYNC, LOG_ALERT, 0, msgtxt_initfail);
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(UNDERFLO, QUICC_ASYNC, LOG_ERR, 0, "Unit %d, underflow error");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
