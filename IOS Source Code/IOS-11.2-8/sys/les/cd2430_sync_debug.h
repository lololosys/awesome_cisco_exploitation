/* $Id: cd2430_sync_debug.h,v 3.1.24.2 1996/07/31 01:43:11 snyder Exp $
 * $Source: /release/112/cvs/Xsys/les/cd2430_sync_debug.h,v $
 *-----------------------------------------------------------------------
 * cd2430_sync_debug.h - Debug code for the Cirrus CL-CD2430 synchronous
 *                       interface driver
 *
 * February 1996,  Suresh Sangiah
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *-----------------------------------------------------------------------
 * $Log: cd2430_sync_debug.h,v $
 * Revision 3.1.24.2  1996/07/31  01:43:11  snyder
 * CSCdi64671:  include file should have been mention in c file
 *              declare some arrays const, real reason I was here.
 * Branch: California_branch
 *
 * Revision 3.1.24.1  1996/06/17  08:33:28  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.1  1996/02/13  07:14:22  ssangiah
 * Placeholder files for porting new platform code from 11.0.
 *
 *
 * Port the Cobra platform into 11.1
 *
 * Revision 3.1  1996/02/13  07:14:22  ssangiah
 * Placeholder files for porting new platform code from 11.0.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __CD2430_SYNC_DEBUG_H__
#define __CD2430_SYNC_DEBUG_H__

/*
 * Descriptive strings for the transmit states.
 */
extern char *const hdx_tx_state_list[];

/*
 * Descriptive strings for the receive states.
 */
extern char *const hdx_rx_state_list[];

/*
 * Descriptive strings for the half-duplex events.
 */
extern char *const hdx_event_list[];

/*
 * Descriptive name strings for the state machines.
 */
extern char *dte_rx_fsm ;
extern char *dte_tx_fsm ;
extern char *dce_rx_fsm ;
extern char *dce_tx_fsm ;

/*
 * Descriptive strings for figures displayed in debug output.
 */
extern char *dtr_assert_fig ;
extern char *dtr_deassert_fig ;
extern char *rts_assert_fig ;
extern char *rts_deassert_fig ;
extern char *dcd_assert_fig ;
extern char *dcd_deassert_fig ;
extern char *dsr_assert_fig ;
extern char *dsr_deassert_fig ;
extern char *cts_assert_fig ;
extern char *cts_deassert_fig ;

/*
 * Indentation for DTE and DCE
 */
extern char *dte_indent;
extern char *dce_indent;

#define HDX_LOG(ctx, fsm_string, state_string, event_string) {              \
        if (ctx->hdx_log) { 						    \
	    log_half_duplex_data(ctx, fsm_string, state_string, event_string); \
	}								    \
}

#define HDX_LOG_FIG(ctx, figure_string) {                                   \
        if (ctx->hdx_log) {						    \
	    log_half_duplex_figure(ctx, figure_string);                     \
	}								    \
}
#endif  /* of __CD2430_SYNC_DEBUG_H__ */

