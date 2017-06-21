/* $Id: msg_modem.c,v 3.1.52.5 1996/09/13 23:50:42 hampton Exp $
 * $Source: /release/112/cvs/Xsys/les/msg_modem.c,v $
 *------------------------------------------------------------------
 * msg_modem.c - Message file for modem management facility
 *
 * January 1996, Viet Nguyen
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: msg_modem.c,v $
 * Revision 3.1.52.5  1996/09/13  23:50:42  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.52.4  1996/08/28  12:59:44  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.52.3  1996/07/16  11:31:09  styang
 * CSCdi58062:  %MODEM-3-UNEXPECTED_STSTUS_EV: Error -- status state
 * machine receive
 * Branch: California_branch
 * 1. The error message is generated wrong, code modified to correct such
 * error.
 * 2. Code clean up.
 *
 * Revision 3.1.52.2  1996/06/19  19:29:56  snyder
 * CSCdi60807:  spelllink errors
 * Branch: California_branch
 *
 *         wiht-> with
 *         occured -> occurred
 *         unexpeced -> unexpected
 *
 * Revision 3.1.52.1  1996/06/16  21:16:37  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1996/01/09  22:47:50  vnguyen
 * Placeholder for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(MODEM);				/* Define the facility */
msgdef_section("");


msgdef(BADEVENT, MODEM, LOG_ALERT, 0,
       "Invalid modem management %s event 0x%02x for modem(%d/%d)");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");

msgdef(MODEMPOLLOFF, MODEM, LOG_ALERT, 0,
       "Modem Status Poll Off after bad status poll event 0x%02x for modem(%d/%d)");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");

msgdef(BADMODEMS, MODEM, LOG_NOTICE, 0,
       "Modems (%d/%d) and (%d/%d) failed back-to-back test: %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(B2BMODEMS, MODEM, LOG_NOTICE, 0,
       "Modems (%d/%d) and (%d/%d) completed back-to-back test: success/packets = %d/%d");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(BADMODEM, MODEM, LOG_ALERT, 0,
       "Modem (%d/%d) failed %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(MODEM_OK, MODEM, LOG_NOTICE, 0,
       "Modem (%d/%d) passed the test");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(B2BCONNECT, MODEM, LOG_NOTICE, 0,
       "Modems (%d/%d) and (%d/%d) connected in back-to-back test: %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(DL_START, MODEM, LOG_NOTICE, 0,
       "Modem (%d/%d) started firmware download");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(DL_GOOD, MODEM, LOG_NOTICE, 0,
       "Modem (%d/%d) completed firmware download: %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(DL_FAIL, MODEM, LOG_ALERT, 0,
       "Modem (%d/%d) failed firmware download (%d): %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(NO_TIME_FOR_DC, MODEM, LOG_NOTICE, 0,
       "Warning -- Not enough time to process DC session");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(UNEXPECTED_STATUS_EV, MODEM, LOG_ERR, 0,
       "Error -- status state machine received unexpected event=%d for modem(%d/%d)");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(UNEXPECTED_STATUS_STATE, MODEM, LOG_ERR, 0,
       "Error -- status state machine in unknown state=%d for modem(%d/%d)");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(UNEXPECTED_DC_EV, MODEM, LOG_ERR, 0,
       "Error -- dc state machine received unexpected event=%d for modem(%d/%d)");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(UNEXPECTED_DC_STATE, MODEM, LOG_ERR, 0,
       "Error -- dc state machine in unknown state=%d for modem(%d/%d)");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


/*
 * Modem trace upload
 */

facdef(MODEM_HIST);			/* Define the facility */
msgdef_section("");


msgdef(TRACE_HDR, MODEM_HIST, LOG_ERR, 0,
       "\n\nModem %d/%d Events Log:");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(STARTUP, MODEM_HIST, LOG_ERR, 0,
       "\n  %8s: Startup Response: %s. \nModem (boot) firmware = %d.%d.%d (%d.%d.%d)");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(CONTROL, MODEM_HIST, LOG_ERR, 0,
       "\n  %8s: Control Reply: 0x%02x%02x");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(DC_SESSION, MODEM_HIST, LOG_ERR, 0,
       "\n  %8s: DC Session received reply");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(POLL_TX_RX, MODEM_HIST, LOG_ERR, 0,
       "\n  %8s: TX/RX event: TXD=%s, RXD=%s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(POLL_RS232, MODEM_HIST, LOG_ERR, 0,
       "\n  %8s: RS232 event: %s%c %s%c %s%c %s%c %s%c %s%c %s%c");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(POLL_MODEM_STATE, MODEM_HIST, LOG_ERR, 0,
       "\n  %8s: Modem State event: %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(POLL_CONN_1, MODEM_HIST, LOG_ERR, 0,
       "\n  %8s: Connection event: TX/RX Speed = %u/%u, Modulation = %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(POLL_CONN_2, MODEM_HIST, LOG_ERR, 0,
       "\n            Direction = %s, Protocol = %s, Compression = %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(POLL_CONX_UPDATE, MODEM_HIST, LOG_ERR, 0,
       "\n  %8s: Connection update event: TX/RX Speed = %u/%u, Modulation = %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(POLL_PROGRESS, MODEM_HIST, LOG_ERR, 0,
       "\n  %8s: Call Progress event: %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(POLL_DTR, MODEM_HIST, LOG_ERR, 0, "\n  %8s: DTR event: %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(POLL_INACT, MODEM_HIST, LOG_ERR, 0,
       "\n  %8s: Modem Activity event: %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(POLL_ANALOG, MODEM_HIST, LOG_ERR, 0,
       "\n  %8s: Modem Analog signal event: TX = %d, RX = %d, Signal to noise = %d");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(POLL_END_1, MODEM_HIST, LOG_ERR, 0,
       "\n  %8s: End connection event: Retransmits for MNP block (TX/RX) = %d/%d");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(POLL_END_2, MODEM_HIST, LOG_ERR, 0,
       "\n            Duration = %d:%-02d:%-02d, Number of TX/RX char = %d/%d");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(POLL_END_3, MODEM_HIST, LOG_ERR, 0,
       "\n            Local Disc Reason = %s, Remote Disc Reason = %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(POLL_NO_RESP, MODEM_HIST, LOG_ERR, 0,
       "\n  %8s: No poll response event: 0x%02x");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(POLL_EVT_LOST, MODEM_HIST, LOG_ERR, 0,
       "\n  %8s: Status event lost: 0x%02x");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(POLL_PHONE, MODEM_HIST, LOG_ERR, 0,
       "\n  %8s: Phone number event: %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(POLL_CALLER_ID, MODEM_HIST, LOG_ERR, 0,
       "\n  %8s: Caller ID event: %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(POLL_ERROR, MODEM_HIST, LOG_ERR, 0,
       "\n  %8s: Polling protocol error event: 0x%02x");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(POLL_UNKNOWN, MODEM_HIST, LOG_ERR, 0,
       "\n  %8s: Unknown or lost event 0x%02x");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(DC_EVT_POLL, MODEM_HIST, LOG_ERR, 0,
       "\n  %8s: DC session response: %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(UNKNOWN_FRAME, MODEM_HIST, LOG_ERR, 0,
       "\n  %8s: Error in events log");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(CSM_CALLED_NUM, MODEM_HIST, LOG_ERR, 0,
       "\n  %8s: ISDN outgoing called number: %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


msgdef(CSM_CALLER_NUM, MODEM_HIST, LOG_ERR, 0,
       "\n  %8s: ISDN incoming caller number: %s");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");
