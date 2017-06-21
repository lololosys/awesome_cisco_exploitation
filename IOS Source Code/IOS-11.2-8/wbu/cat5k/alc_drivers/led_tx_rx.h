/* $Id: led_tx_rx.h,v 1.1.4.1 1996/05/09 14:55:01 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/led_tx_rx.h,v $
 *------------------------------------------------------------------
 * led_tx_rx.h -- Header file for LED blinker Finite State Machine
 *
 * August 1995, Sanjaya Kumar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: led_tx_rx.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:01  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:28  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:15  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.2  1996/04/04  22:08:01  sakumar
 * LED Blinker process now uses managed timers and new process model (no
 * edisms).
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.16.1  1996/03/22  09:10:33  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:27  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:56  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

typedef struct led_info_t_ {
	int	state;					/* State in LED FSM 		*/
	mgd_timer timer;			/* IOS Managed Timer		*/
	UINT32 on_duration;			/* LED on duration (ms)		*/
	UINT32 off_duration;		/* Led off duration (ms)	*/
	UINT32 pkt_count;			/* packet count				*/
	UINT32 delta;				/* packets since last poll	*/
	UINT32 led_id;				/* which LED 				*/
	UINT16 led_on_color;		/* Green					*/
	UINT16 led_off_color;		/* Off						*/
} led_info_t;

typedef enum led_state_t_ {
	LED_ST_IDLE = 1,
	LED_ST_ON,
	LED_ST_OFF,
	MAX_LED_FSM_STATE = LED_ST_OFF,
} led_state_t;

typedef enum led_major_t_ {
	LED_MAJOR_NZDELTA,		/* Non-Zero delta since last frame-count poll */
	LED_MAJOR_ZDELTA,		/* Zero count since last frame-count poll     */
	LED_TIMER_POP,			/* timer pop 								  */
} led_major_t;

/*
 * Function prototypes
 */
extern int		led_fsm_decode(led_info_t *led_info, int major, int minor);

/*
 * LED state machine action functions
 */

extern ushort a_turn_led_on(led_info_t *led_info);

extern ushort a_turn_led_off(led_info_t *led_info);

extern ushort a_calc_off_duration(led_info_t *led_info);

extern ushort a_go_to_idle(led_info_t *led_info);

extern ushort a_idle_to_on(led_info_t *led_info);
