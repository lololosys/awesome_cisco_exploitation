/* $Id: cd2430_sync_debug.c,v 3.1.2.2 1996/07/31 01:43:11 snyder Exp $
 * $Source: /release/112/cvs/Xsys/les/cd2430_sync_debug.c,v $
 *------------------------------------------------------------------
 * cd2430_sync_debug.c - Debug code for the Cirrus CL-CD2430 synchronous
 *                       interface driver
 *
 * April 1996, Jose Hernandez
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cd2430_sync_debug.c,v $
 * Revision 3.1.2.2  1996/07/31  01:43:11  snyder
 * CSCdi64671:  include file should have been mention in c file
 *              declare some arrays const, real reason I was here.
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/06/17  08:33:27  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.1  1996/04/23  18:32:17  samuell
 * Placeholder files for porting Modular 2500 Cobra platform from 11.1.
 * Modular 2500 Cobra files were derived from the original Cobra 11.1
 * Release by code separation into platform-independent code and 2500
 * platform-dependent code files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../les/cd2430_sync_debug.h"

/*
 * Half-duplex debug logging related strings.
 */


/*
 * Descriptive strings for the transmit states.
 */
char *const hdx_tx_state_list[] = {
  "Invalid Tx state",                   /* HDX_TX_STATE_NULL                  */
  "DTE Tx Ready state",                 /* HDX_DTE_TX_STATE_READY             */
  "DTE Tx Transmit Delay state",	/* HDX_DTE_TX_STATE_TRANSMIT_DELAY    */
  "DTE Tx Wait CTS state",              /* HDX_DTE_TX_STATE_WAIT_CTS          */
  "DTE Tx Transmit state",              /* HDX_DTE_TX_STATE_TRANSMIT          */
  "DTE Tx Wait TxEmpty state",          /* HDX_DTE_TX_STATE_WAIT_TXMPTY       */
  "DTE Tx Wait RTS Drop Delay state",   /* HDX_DTE_TX_STATE_WAIT_RTSDROP_DELAY*/
  "DTE Tx Wait CTS Drop state",         /* HDX_DTE_TX_STATE_WAIT_CTSDROP      */
  "DCE Tx Ready state",                 /* HDX_DCE_TX_STATE_READY             */
  "DCE Tx Transmit Delay state",	/* HDX_DCE_TX_STATE_TRANSMIT_DELAY    */
  "DCE TX DCD TxStart Delay state",	/* HDX_DCE_TX_STATE_DCD_TXSTART_DELAY */
  "DCE Tx Transmit state",              /* HDX_DCE_TX_STATE_TRANSMIT          */
  "DCE Tx Wait TxEmpty state",          /* HDX_DCE_TX_STATE_WAIT_TXMPTY       */
  "DCE Tx DCD Drop Delay state"		/* HDX_DCE_TX_STATE_DCD_DROP_DELAY    */
};

/*
 * Descriptive strings for the receive states.
 */
char *const hdx_rx_state_list[] = {
  "Invalid Rx state",                   /* HDX_RX_STATE_NULL                  */
  "DTE Rx Ready state",                 /* HDX_DTE_RX_STATE_READY             */
  "DTE Rx In Giant state",              /* HDX_DTE_RX_STATE_INGIANT           */
  "DCE Rx Ready state",                 /* HDX_DCE_RX_STATE_READY             */
  "DCE Rx CTS Delay state",             /* HDX_DCE_RX_STATE_CTS_DELAY         */
  "DCE Rx Receive state",               /* HDX_DCE_RX_STATE_RECEIVE           */
  "DCE Rx In Giant state"               /* HDX_DCE_RX_STATE_INGIANT           */
};

/*
 * Descriptive strings for the half-duplex events.
 */
char *const hdx_event_list[] = {
  "Entry event",                /* HDX_ENTRY_EVENT              */
  "Timer1 event",               /* HDX_TIMER1_EVENT             */
  "Timer2 event",               /* HDX_TIMER2_EVENT             */
  "RTS Change event",           /* HDX_RTS_CHANGE_EVENT         */
  "CTS Change event",           /* HDX_CTS_CHANGE_EVENT         */
  "Tx Bus Error event",         /* HDX_TX_BUSERROR_EVENT        */
  "Tx Underrun event",          /* HDX_TX_UNDERRUN_EVENT        */
  "Tx Empty event",             /* HDX_TX_EMPTY_EVENT           */
  "Tx EOB event",               /* HDX_TX_EOB_EVENT             */
  "Tx EOF event",               /* HDX_TX_EOF_EVENT             */
  "Rx Abort event",             /* HDX_RX_ABORT_EVENT           */
  "Rx CRC Error event",         /* HDX_RX_CRC_ERROR_EVENT       */
  "Rx Overrun event",           /* HDX_RX_OVERRUN_EVENT         */
  "Rx Residual Ind event",      /* HDX_RX_RESIND_EVENT          */
  "Rx Clear Detect event",      /* HDX_RX_CLR_DTCT_EVENT        */
  "Rx Bus Error event",         /* HDX_RX_BUSERROR_EVENT        */
  "Rx EOB event",               /* HDX_RX_EOB_EVENT             */
  "Rx EOF event"                /* HDX_RX_EOF_EVENT             */
};

/*
 * Descriptive name strings for the state machines.
 */
char *dte_rx_fsm = "DTE Rx FSM";
char *dte_tx_fsm = "DTE Tx FSM";
char *dce_rx_fsm = "DCE Rx FSM";
char *dce_tx_fsm = "DCE Tx FSM";

/*
 * Descriptive strings for figures displayed in debug output.
 */
char *dtr_assert_fig    = "\t\t\t\t\tDTR\t--->\n";
char *dtr_deassert_fig  = "\t\t\t\t\tnoDTR\t--->\n";
char *rts_assert_fig    = "\t\t\t\t\tRTS\t--->\n";
char *rts_deassert_fig  = "\t\t\t\t\tnoRTS\t--->\n";
char *dcd_assert_fig    = "\t\t\t\t\t\t\t<---\tDCD\n";
char *dcd_deassert_fig  = "\t\t\t\t\t\t\t<---\tnoDCD\n";
char *dsr_assert_fig    = "\t\t\t\t\t\t\t<---\tDSR\n";
char *dsr_deassert_fig  = "\t\t\t\t\t\t\t<---\tnoDSR\n";
char *cts_assert_fig    = "\t\t\t\t\t\t\t<---\tCTS\n";
char *cts_deassert_fig  = "\t\t\t\t\t\t\t<---\tnoCTS\n";

/*
 * Indentation for DTE and DCE
 */
char *dte_indent = "\t\t\t\t";
char *dce_indent = "\t\t\t\t\t\t\t\t";





