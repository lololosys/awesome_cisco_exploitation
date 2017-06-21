/* $Id: if_les_sync_cd2430.h,v 3.1.2.4 1996/08/28 12:58:56 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les_sync_cd2430.h,v $
 *------------------------------------------------------------------
 * if_les_sync_cd2430.h - Definitions for synchronous serial driver
 *                        for Cirrus CL-CD2430 controller (common
 *                        code)
 *
 * May 1996, Jose Hernandez
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_les_sync_cd2430.h,v $
 * Revision 3.1.2.4  1996/08/28  12:58:56  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.2.3  1996/07/31  01:43:10  snyder
 * CSCdi64671:  include file should have been mention in c file
 *              declare some arrays const, real reason I was here.
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/06/20  16:16:15  widmer
 * CSCdi60890:  Bogus includes cause unnecessary compiles
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/06/17  08:34:04  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.1  1996/05/13  18:56:53  samuell
 * Placeholder files for porting Modular 2500 Cobra platform from 11.1.
 * Modular 2500 Cobra files were derived from the original Cobra 11.1
 * Release by code separation into platform-independent code and 2500
 * platform-dependent code files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __IF_LES_SYNC_CD2430_H__
#define __IF_LES_SYNC_CD2430_H__

/*
 * Definition of the half-duplex states and events
 * The states for the four state machines are defined followed by the
 * list of events for all state machines. 
 */

/*
 * States for the half-duplex transmit state machine
 */
enum HDX_TX_STATE {
    
        /* Invalid state */
	HDX_TX_STATE_NULL = 10,

        /* DTE states */
	HDX_DTE_TX_STATE_READY,
        HDX_DTE_TX_STATE_TRANSMIT_DELAY,
	HDX_DTE_TX_STATE_WAIT_CTS,
	HDX_DTE_TX_STATE_TRANSMIT,
	HDX_DTE_TX_STATE_WAIT_TXMPTY,
	HDX_DTE_TX_STATE_WAIT_RTSDROP_DELAY,
	HDX_DTE_TX_STATE_WAIT_CTSDROP,

        /* DCE states */
	HDX_DCE_TX_STATE_READY,
	HDX_DCE_TX_STATE_TRANSMIT_DELAY,
	HDX_DCE_TX_STATE_DCD_TXSTART_DELAY,
	HDX_DCE_TX_STATE_TRANSMIT,
	HDX_DCE_TX_STATE_WAIT_TXMPTY,
	HDX_DCE_TX_STATE_DCD_DROP_DELAY
};

/* 
 * States for the half-duplex receive state machine
 */
enum HDX_RX_STATE {

	/* Invalid receive state */
	HDX_RX_STATE_NULL = 50,

        /* DTE states */
	HDX_DTE_RX_STATE_READY,
	HDX_DTE_RX_STATE_INGIANT,

        /* DCE states */
	HDX_DCE_RX_STATE_READY,
	HDX_DCE_RX_STATE_CTS_DELAY,	
	HDX_DCE_RX_STATE_RECEIVE,
	HDX_DCE_RX_STATE_INGIANT
};

/*
 * Events for the half-duplex state machines
 */
enum HDX_EVENT {
        HDX_ENTRY_EVENT = 0,
	HDX_TIMER1_EVENT,
	HDX_TIMER2_EVENT,
	HDX_RTS_CHANGE_EVENT,
	HDX_CTS_CHANGE_EVENT,
	HDX_TX_BUSERROR_EVENT,
	HDX_TX_UNDERRUN_EVENT,
	HDX_TX_EMPTY_EVENT,
	HDX_TX_EOB_EVENT,
	HDX_TX_EOF_EVENT,
	HDX_RX_ABORT_EVENT,
	HDX_RX_CRC_ERROR_EVENT,
	HDX_RX_OVERRUN_EVENT,
	HDX_RX_RESIND_EVENT,
	HDX_RX_CLR_DTCT_EVENT,
	HDX_RX_BUSERROR_EVENT,
	HDX_RX_EOB_EVENT,
	HDX_RX_EOF_EVENT,
	HDX_MAX_EVENTS
};

/*
 * The structure to hold the pre-computed clock related divisors
 * for the pre-divider on the motherboard and the on-chip divisor to
 * be programmed into the transmit and receive bit-period registers.
 */
typedef struct cd2430_clocktype {
    int   rate;
    uchar pre_divider;
    uchar bpr;
} cd2430_clocktype_t;

/*
 * The structure to hold values to assist in setting up on-chip timers
 * when in half-duplex operation. The setting of the timer consists of
 * setting a timer pre-scaler via the Timer Period Register (TPR) and
 * the timer value itself via one of the General Timer registers.
 */
typedef struct cd2430_timer {
    ulong  msecval;   /* Timer value in msec units */
    uchar  tpr;       /* Timer Period Register value */
    ushort timer_val; /* Timer value to program General Timer register */
} cd2430_timer_t;

/*
 * Logging buffer element structure for the half-duplex log
 */
typedef struct hdx_log_element {
        sys_timestamp timestamp;
        char   *fsm;     /* Name of the state machine */
        char   *state;   /* Current state in the state machine */
        char   *event;   /* Current event in the state machine */
        char   *figure;  /* Any figure that needs to be added */
} hdx_log_element_t;
#define HDX_LOG_LENGTH  1000  /* Depth of the half-duplex log */

#define CD2430_DELAY_FLAGS_DEFAULT 2 /* Minimum flags at start of frame */

/*
 * CD2430 timer register related defines
 */
#define MAX_TICKS_TIMER1 65535  /* Max value for General Timer 1 Register */
#define MAX_TICKS_TIMER2 255    /* Max value for General Timer 2 Register */
#define MIN_PRE_SCALER	 10     /* Min allowed value for pre-scaler to 
                                   maintain timer accuracy */
#define MAX_PRE_SCALER 	 255    /* Max possible pre-scaler value */

#define CD2430_TX_TIMEOUT 	(5 * ONESEC) /* Transmit timeout interval */

extern hdx_log_element_t *hdx_log_base;

#ifdef PAN
#include "../les/if_c3000_sync_cd2430.h"
#endif

#endif  /* of __IF_LES_SYNC_CD2430_H__ */
