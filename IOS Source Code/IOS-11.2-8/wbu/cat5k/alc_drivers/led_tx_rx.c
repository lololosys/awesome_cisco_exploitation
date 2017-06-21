/* $Id: led_tx_rx.c,v 1.1.4.2 1996/08/03 16:12:35 fox Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/led_tx_rx.c,v $
 *------------------------------------------------------------------
 * led_tx_rx.c -- Synergy ATM Card LED blinker Finite State Machine
 *
 * August 1995, Sanjaya Kumar
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: led_tx_rx.c,v $
 * Revision 1.1.4.2  1996/08/03  16:12:35  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove activatehigh from everywhere.
 *
 * Revision 1.1.4.1  1996/05/09  14:55:00  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:27  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:14  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.2  1996/04/04  22:08:00  sakumar
 * LED Blinker process now uses managed timers and new process model (no
 * edisms).
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.16.1  1996/03/22  09:10:32  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.2  1996/03/20  10:52:32  rlowe
 * Integrate with ios/wbu sub-tree and makefiles.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:26  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:55  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "../../../sys/h/master.h"
#include "../../../sys/util/fsm.h"
#include "../../../sys/h/mgd_timers.h"


#include    "basic_types.h"
#include    "param.h"
#include    "queue_if.h"
#include    "buffer_if.h"
#include    "ivcimap.h"
#include    "ovcimap.h"
#include    "cam_if.h"
#include    "shram.h"
#include    "spram.h"
#include    "sar_if.h"
#include    "atmdrv_if.h"
#include    "../../../sys/src-68-alc/ios_atmdriver.h"
#include    "atmdvr_gdata.h"
#include    "ntc_client.h"

#include "led_if.h"
#include "led_tx_rx.h"

static pid_t led_tx_rx_pid;
mgd_timer led_master;
mgd_timer led_poll;

/* Leaf Timer types */
enum{LED_TX, LED_RX, LED_POLL};

/*
 * casting action functions
 */

#define A_TURN_LED_ON			FSM_ACTION_ROUTINE(a_turn_led_on)
#define A_TURN_LED_OFF			FSM_ACTION_ROUTINE(a_turn_led_off)
#define A_CALC_OFF_DUR			FSM_ACTION_ROUTINE(a_calc_off_duration)
#define A_GO_TO_IDLE			FSM_ACTION_ROUTINE(a_go_to_idle)
#define A_IDLE_TO_ON			FSM_ACTION_ROUTINE(a_idle_to_on)

static struct fsm_state led_idle[] =
{
/*	Action			Arguments		Next State		Decoded Input 		*/
	A_IDLE_TO_ON,	FSM_NOARG, 		LED_ST_ON,		/* Non Zero Delta	*/
	FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE,	/* Zero Delta		*/
	FSM_NOACTION,	FSM_NOARG,		FSM_NOCHANGE,	/* Timer Pops		*/
};
static struct fsm_state led_on[] =
{
/*	Action			Arguments		Next State		Decoded Input 		*/
	A_CALC_OFF_DUR, FSM_NOARG, 		FSM_NOCHANGE,	/* Non Zero Delta	*/
	A_GO_TO_IDLE,	FSM_NOARG,		LED_ST_IDLE,	/* Zero Delta		*/
	A_TURN_LED_OFF,	FSM_NOARG,		LED_ST_OFF,		/* Timer Pops		*/
};
static struct fsm_state led_off[] =
{
/*	Action			Arguments		Next State		Decoded Input 		*/
	A_CALC_OFF_DUR,	FSM_NOARG, 		FSM_NOCHANGE,	/* Non Zero Delta	*/
	A_GO_TO_IDLE,	FSM_NOARG,		LED_ST_IDLE,	/* Zero Delta		*/
	A_TURN_LED_ON,	FSM_NOARG,		LED_ST_ON,		/* Timer Pops		*/
};

#define LED_FSM_DECODE FSM_DECODE_ROUTINE(led_fsm_decode)

static struct finite_state_machine led_fsm_table[] =
{
/* 	State		Decoder				State Value		*/
{	led_idle,	LED_FSM_DECODE},	/* LEC_ST_IDLE	*/
{	led_on,		LED_FSM_DECODE},	/* LEC_ST_ON	*/
{	led_off,	LED_FSM_DECODE},	/* LEC_ST_OFF	*/
};

/* Globals */
led_info_t SynALC_tx_led;
led_info_t SynALC_rx_led;


/* Forward Declarations */
forktype Syn_ledProcess(void);
void SynALC_led_stat_poll(void) ;

#define BLINK	100			/* One LED blink is 100 ms		*/
#define POLL_INTERVAL 1000	/* Poll SAR Sats every second	*/


int
led_fsm_decode (led_info_t *led_info, int major, int minor)
{
	switch(major) {
		case LED_MAJOR_NZDELTA:
		case LED_MAJOR_ZDELTA:
		case LED_TIMER_POP:
			return (major) ;
		default:
			return (FSM_FAULT);
	}
}

/* Action Function - Starts timer for ON_DURATION and turns LED on */
ushort a_turn_led_on(led_info_t *led_info)
{
	if (!led_info) {
		return(FSM_RC_DATA_ERROR);
	}

	/* Start timer with ON Duration */
	mgd_timer_start(&led_info->timer, led_info->on_duration);

	/* Turn LED on */
	set_led(led_info->led_id, led_info->led_on_color);

	
	return(FSM_OK);
}

/* Action Function - Starts timer for OFF_DURATION and turns LED off */
ushort a_turn_led_off(led_info_t *led_info)
{
	if (!led_info) {
		return(FSM_RC_DATA_ERROR);
	}

	/* Start timer with OFF Duration */
	mgd_timer_start(&led_info->timer, led_info->off_duration);
	
	/* Turn LED off */
	set_led(led_info->led_id, led_info->led_off_color);

	return(FSM_OK);
}


/* Action Function - Calculates the OFF_DURATION based on delta */
ushort a_calc_off_duration(led_info_t *led_info)
{
	if (!led_info) {
		return(FSM_RC_DATA_ERROR);
	}

	/* we calculate the off-duration in BLINKs. The off duration should */
	/* be roughly inversely proportional to the delta value				*/
	if (led_info->delta > 20000) {
		led_info->off_duration = BLINK/4;	/* LED on most of the time */
	} else if (led_info->delta > 10000) {
		led_info->off_duration = BLINK/2;
	} else if (led_info->delta > 5000) {
		led_info->off_duration = BLINK;
	} else if (led_info->delta > 2500) {
		led_info->off_duration = BLINK*2;
	} else if (led_info->delta > 1250) {
		led_info->off_duration = BLINK*4;
	} else if (led_info->delta > 625) {
		led_info->off_duration = BLINK*8;
	} else {
		led_info->off_duration = 16*BLINK;
	}
	return(FSM_OK);
}


/* Action Function - Stops timer turns LED off */
ushort a_go_to_idle(led_info_t *led_info)
{
	if (!led_info) {
		return(FSM_RC_DATA_ERROR);
	}

	/* Stop timer */
	mgd_timer_stop(&led_info->timer);
	
	/* Turn off LED */
	set_led(led_info->led_id, led_info->led_off_color);

	return(FSM_OK);
}



/* Action Function - Goes from IDLE state to ON State; calculates off */
/* duration, starts timer for ON_DURATION and turns LED on			  */

ushort a_idle_to_on(led_info_t *led_info)
{
	if (!led_info) {
		return(FSM_RC_DATA_ERROR);
	}

	a_calc_off_duration(led_info);

	a_turn_led_on(led_info);

	return(FSM_OK);
}

/* Led Blinker Initialization Function */
void
SynALC_led_init()
{
	SynALC_tx_led.led_id = ACP_WRRG_PMD_TOP_LED;
	SynALC_tx_led.led_on_color = ACP_PMD_LEVL_LED_GRN;
	SynALC_tx_led.led_off_color = ACP_PMD_LEVL_LED_OFF;
	SynALC_tx_led.on_duration = BLINK;
	SynALC_tx_led.pkt_count = 0;
	SynALC_tx_led.delta = 0;
	SynALC_tx_led.state = LED_ST_IDLE;

	SynALC_rx_led.led_id = ACP_WRRG_PMD_MID_LED;
	SynALC_rx_led.led_on_color = ACP_PMD_LEVL_LED_GRN;
	SynALC_rx_led.led_off_color = ACP_PMD_LEVL_LED_OFF;
	SynALC_rx_led.on_duration = BLINK;
	SynALC_rx_led.pkt_count = 0;
	SynALC_rx_led.delta = 0;
	SynALC_rx_led.state = LED_ST_IDLE;

    /* Initialize and startup timers */
    mgd_timer_init_parent(&led_master, NULL);
    mgd_timer_init_leaf(&SynALC_tx_led.timer, &led_master, LED_TX, NULL, FALSE);
    mgd_timer_init_leaf(&SynALC_rx_led.timer, &led_master, LED_RX, NULL, FALSE);
    mgd_timer_init_leaf(&led_poll, &led_master, LED_POLL, NULL, FALSE);

    led_tx_rx_pid = process_create(Syn_ledProcess, "Cat5K ATM LED", 
                                   NORMAL_STACK, PRIO_NORMAL);
}

/* LED process, drives all the timers and the LED FSM */
forktype Syn_ledProcess(void)
{
	/* wait for system to initialize */
	process_wait_on_system_init();

    mgd_timer_start(&led_poll, POLL_INTERVAL);

    process_watch_mgd_timer(&led_master, ENABLE);

	while (1) {
        process_wait_for_event();
		while (mgd_timer_expired(&led_master)) {
            mgd_timer *expired_timer;

            expired_timer = mgd_timer_first_expired(&led_master);

            switch (mgd_timer_type(expired_timer)) {
              case LED_TX:
			    fsm_execute("SynAlcLED",
						led_fsm_table,
						((int *) &(SynALC_tx_led.state)),
						MAX_LED_FSM_STATE,
						(void *) &SynALC_tx_led,
						LED_TIMER_POP,
						0,
						FALSE);
				break;

              case LED_RX:
			    fsm_execute("SynAlcLED",
						led_fsm_table,
						((int *) &(SynALC_rx_led.state)),
						MAX_LED_FSM_STATE,
						(void *) &SynALC_rx_led,
						LED_TIMER_POP,
						0,
						FALSE);
				break;

              case LED_POLL:
				SynALC_led_stat_poll();
				mgd_timer_update(expired_timer, POLL_INTERVAL);
		
				/* Also, poll for RX TX cell stats */
    			if (pmd_chip_type == PMD_PMC) {
					UINT32 rxcells, txcells;

        			/* Get cell count increments */
        			syn_atm_pmc_oc3_get_cell_stats(&rxcells, &txcells);
 
        			AtmdrvGcb.rxcellcount += rxcells;

					/* *** Because the TX SAR sends idle cells, they are  */
                	/* *** counted as non-idle by the PMC, since *it* is  */
                	/* *** supposed to send idle cells. So the PMC's idle */
                	/* *** cell count is wrong. The actual cell count is  */
                	/* *** kept by the TXSAR itself.                      */
        			AtmdrvGcb.txcellcount = acp_pTxSpram->txstat_gl.txgl_cell_xmit;
    			}
			}
        }
	}
}



/* This function is called periodically to update timer off durations 	*/
/* for the RX and TX leds												*/
void
SynALC_led_stat_poll(void)
{
	UINT32 new_count;

	/* First for TX LED */
	new_count = acp_pTxSpram->txstat_gl.txgl_upacket_output;
	SynALC_tx_led.delta = new_count - SynALC_tx_led.pkt_count;
	SynALC_tx_led.pkt_count = new_count;
	fsm_execute("SynAlcLED",
				led_fsm_table,
				((int *) &(SynALC_tx_led.state)),
				MAX_LED_FSM_STATE,
				(void *) &SynALC_tx_led,
				((SynALC_tx_led.delta == 0) ? 
						LED_MAJOR_ZDELTA : LED_MAJOR_NZDELTA),
				0,
				FALSE);
	
	/* Now for RX LED */
	new_count = acp_pRxSpram->rxstat_gl.rxgl_packet_input;
	SynALC_rx_led.delta = new_count - SynALC_rx_led.pkt_count;
	SynALC_rx_led.pkt_count = new_count;
	fsm_execute("SynAlcLED",
				led_fsm_table,
				((int *) &(SynALC_rx_led.state)),
				MAX_LED_FSM_STATE,
				(void *) &SynALC_rx_led,
				((SynALC_rx_led.delta == 0) ? 
						LED_MAJOR_ZDELTA : LED_MAJOR_NZDELTA),
				0,
				FALSE);
}
