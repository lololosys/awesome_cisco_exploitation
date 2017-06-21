/* $Id: robbed_bit.h,v 1.9.6.1 1997/04/01 19:51:28 jchlin Exp $
 * $Source: /cscka/t1_neat/src/common/robbed_bit.h,v $
 *------------------------------------------------------------------
 * robbed_bit.h -- NEAT Processor's Robbed Bit Signaling definition
 *
 * April, 1996 - Jonathan Chiung-Hsi Lin
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: robbed_bit.h,v $
 * Revision 1.9.6.1  1997/04/01 19:51:28  jchlin
 * o       CSCdi85287: busyout the channel after call is disconnected.
 * o       CSCdi91038: LS/GS is waiting 1 second to answer the IC to
 *                     avoid false signalling now.
 * o       CSCdi92824: Fix the race conditiion between CO and CSM during
 *                     disconnecting the modem calls.
 * o       CSCdj00011: Sending Ground Ring under GS provision when the time
 *                     slot is busyout.
 *
 * Revision 1.9  1996/10/25 22:26:03  jchlin
 * o       Make objects dependent on dependancies.
 * o       Make timer_timeout_process() sharable between t1 and e1.
 *
 * Revision 1.8  1996/10/16  18:46:03  jchlin
 * CSCdi72629:
 * Loose the timing check for the ringing cadence and do the fast ringing
 * for loopstart and groundstart incoming calls to make the incoming
 * call setup time shorter.
 *
 * Revision 1.7  1996/10/10  23:45:25  jchlin
 * o       Set the framer with default value of SF and AMI during the
 *         initialization of framer.
 * o       Enable the robbed bit interrupt mask in framer chip.
 * o       Change the timer timeout process so that it can process
 *         the timeout timer in the iteration of certain number
 *         before it releases CPU control.
 *
 * Revision 1.6  1996/09/10  02:53:09  jchlin
 * Add thw following new features:
 * o       busy dsx0 back to CO trunk when there's no free modem available
 *         in the system
 * o       Support E&M feature group D, FXS loop start and ground start,
 *         and SAS loop start and ground start.
 *
 * Revision 1.5  1996/07/13  05:34:58  towu
 * o Code clean up.
 * o Changed max disconnect time to 15000 ms.
 * o Added defination of E&M signalling.
 * o Added timeout fields in the structures.
 *
 * Revision 1.4  1996/06/19  18:26:38  jchlin
 * o       Make sure that the incoming call will not be winked if
 *         there's no free modem to accept the call.
 * o       Use the right variable to check the framing types.
 *
 * Revision 1.3  1996/05/12  22:24:53  jchlin
 * o       Add rbs_process() in the main process loop.
 * o       Change the message stream sent form neat to MPU
 *         from one to two. One is high priority and the other
 *         is low priority.
 * o       Add one more status STATUS_CONNECTED so that the state
 *         machine doesn't need to wait for the modem going off-ook
 *         before get the incoming call connected.
 * o       Make sure the NO_CIRCUIT_AVAILABLE is the same as ISDN cause
 *         number.
 *
 * Revision 1.2  1996/05/09  20:35:44  jchlin
 * The initial commit for the robbed bit signaling protocol engine.
 * The engine is designed in a table driven functions which can be
 * extended easily to support different rbs protocols in the future.
 * The initial vserion will support the E&M type II, FGB & FBD.
 *
 * Revision 1.1  1996/04/05  00:34:33  jchlin
 * Placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __ROBBED_BIT_H__
#define __ROBBED_BIT_H__

#define SIMULATION           0

#define ONE_SECOND           1000
#define NO_CAUSE             0
#define INVALID_B_CHANNEL   -1
#define UNKNOWN_B_CHANNEL    0xff
#define NO_CIRCUIT_AVAILABLE 0x22
#define USER_BUSY            0x11

#define INCOMING_CALL     TRUE
#define OUTGOING_CALL     FALSE
#define IC_CALL_ID_MASK   0x0fff

#define ROBBED_A_BIT      BIT(3)
#define ROBBED_B_BIT      BIT(2)
#define ROBBED_C_BIT      BIT(1)
#define ROBBED_D_BIT      BIT(0)

#define STATUS_CONFIGURED           BITW(0)
#define STATUS_BIT_STATE_CHANGED    BITW(1)
#define STATUS_FREE                 BITW(2)
#define STATUS_IDLE                 BITW(3)
#define STATUS_VERIFY_BIT_STATE     BITW(4)
#define STATUS_IN_LOCK_WAITING_LIST BITW(5)
#define STATUS_WAIT_VOICE_ANS       BITW(6)
#define STATUS_CONNECTED            BITW(7)
#define STATUS_FRAMING_DOWN         BITW(8)
#define STATUS_DYNAMIC_BUSYOUT      BITW(9)
#define STATUS_STATIC_BUSYOUT       BITW(10)
#define STATUS_DELAY_DIALOUT        BITW(11)
#define STATUS_RBS_RX_BIT_PENDING   BITW(12)

#define IS_CHANNEL_CONFIGURED(signal_info) (signal_info->status & STATUS_CONFIGURED)
#define IS_BIT_STATE_CHANGED(signal_info) (signal_info->status & STATUS_BIT_STATE_CHANGED)
#define IS_CHANNEL_FREE(signal_info) (signal_info->status & STATUS_FREE)
#define IS_CHANNEL_IDLE(signal_info) (signal_info->status & STATUS_IDLE)
#define IS_WAITING_LOCK(signal_info) (signal_info->status & STATUS_IN_LOCK_WAITING_LIST)
#define IS_ENLISTED(signal_info)     (signal_info->status & (STATUS_BIT_STATE_CHANGED|STATUS_FREE|STATUS_VERIFY_BIT_STATE))
#define IS_VERIFY_BIT_STATE(signal_info) (signal_info->status & STATUS_VERIFY_BIT_STATE)
#define IS_WAITING_VOICE_ANS(signal_info) (signal_info->status & STATUS_WAIT_VOICE_ANS)
#define IS_CONNECTED(signal_info) (signal_info->status & STATUS_CONNECTED)
#define IS_FRAMING_DOWN(signal_info) (signal_info->status & STATUS_FRAMING_DOWN)
#define IS_DYNAMIC_BUSYOUT(signal_info) (signal_info->status & STATUS_DYNAMIC_BUSYOUT)
#define IS_STATIC_BUSYOUT(signal_info) (signal_info->status & STATUS_STATIC_BUSYOUT)
#define IS_DELAY_DIALOUT(signal_info)   (signal_info->status & STATUS_DELAY_DIALOUT)
#define IS_RBS_RX_BIT_PENDING(signal_info) (signal_info->status & STATUS_RBS_RX_BIT_PENDING)

#define T_NONE     0
#define T_CONTINUE 1    /* 1ms is reserved for continuous execution of next state */
#define T_CONFIGURED(time)          (time > T_CONTINUE)
#define T_CONTINUE_CONFIGURED(time) (time == T_CONTINUE)

#define MAX_WAIT_FOR_CHANNEL_LOCK_TIME  (10 * ONE_SECOND)
#define MAX_DISCONNECT_DEBOUNCE_TIME    335    /* 335 ms */
#define MAX_DISCONNECT_REQ_GUARD_TIME   15000  /* 15000 ms */
#define MIN_STATE_CHANGE_DEBOUNCE_TIME  10     /* 10 ms */
#define MAX_STATE_CHANGE_DEBOUNCE_TIME  40     /* 40 ms */
#define CHNL_LOCK_POLLING_INTERVAL      60     /* 60 */
#define MIN_DELAY_DIALOUT_TIME          MAX_DISCONNECT_DEBOUNCE_TIME
#define MORE_THAN_HALF_DEBOUNCE_TIME(countdown) (countdown < (MAX_STATE_CHANGE_DEBOUNCE_TIME/2))
#define LESS_THAN_HALF_DEBOUNCE_TIME(countdown) (countdown > (MAX_STATE_CHANGE_DEBOUNCE_TIME/2))

/*
 * The E&M signaling timing definitions in ms
 */
#define EM_MIN_SEIZURE_DEBOUNCE     40
#define EM_MAX_SEIZURE_DEBOUNCE     50
#define EM_SEIZURE_DEBOUNCE         ((EM_MIN_SEIZURE_DEBOUNCE+EM_MAX_SEIZURE_DEBOUNCE)/2) /* 40-50 */

#define EMIC_MIN_SEIZURE_TO_WINK    210
#define EMIC_MAX_SEIZURE_TO_WINK    230
#define EMIC_SEIZURE_TO_WINK        (((EMIC_MIN_SEIZURE_TO_WINK+EMIC_MAX_SEIZURE_TO_WINK)/2)-EM_SEIZURE_DEBOUNCE) /* 170-180 */

#define EMIC_MIN_WINK_START_DELAY   140
#define EMIC_MAX_WINK_START_DELAY   210
#define EMIC_WINK_START_DELAY       200 /* 140-210 */

#define EMIC_MIN_WINK_TO_DTMF       70
#define EMIC_WINK_TO_DTMF           100 /* 70-??  */

#define EMIC_MIN_DTMF_DURATION      260
#define EMIC_MAX_DTMF_DURATION      1400
#define EMIC_DTMF_DURATION          1300 /* 260-1400 */

#define EMIC_MIN_STONE_TO_ANSWER    100
#define EMIC_MAX_STONE_TO_ANSWER    120
#define EMIC_STONE_TO_ANSWER        110 /* 100-120 */

#define EMIC_MIN_ELEAD_DISC         300
#define EMIC_MAX_ELEAD_DISC         320
#define EMIC_ELEAD_DISC             315 /* 300-320 */

#define EMIC_MAX_ELEAD2MLEAD_DISC   50
#define EMIC_ELEAD2MLEAD_DISC       25 /* ??-50 */

#define EMIC_MIN_ANI_DNIS_DTMF      3000
#define EMIC_MAX_ANI_DNIS_DTMF      3900
#define EMIC_ANI_DNIS_DTMF          3800 /* 3000-3900 */

#define EMIC_MIN_DNIS_TO_ACK_WINK   200
#define EMIC_MAX_DNIS_TO_ACK_WINK   3500
#define EMIC_DNIS_TO_ACK_WINK       500 /* 200-3500 */

#define EMIC_MIN_ACK_WINK_START     140
#define EMIC_MAX_ACK_WINK_START     290
#define EMIC_ACK_WINK_START         200 /* 140-290 */

#define EMIC_MIN_ACK_WINK_TO_ANSWER 250
#define EMIC_ACK_WINK_TO_ANSWER     270 /* 250-??  */

#define EMOC_MIN_ELEAD_SEIZURE      210
#define EMOC_MAX_ELEAD_SEIZURE      5000 /* 210-5000 */

#define EMOC_MIN_WINK_START_DELAY   70
#define EMOC_MAX_WINK_START_DELAY   290  /* 70-290 */

#define EMOC_MIN_ACK_WINK_START     (140-EM_MIN_SEIZURE_DEBOUNCE)
#define EMOC_MAX_ACK_WINK_START     (290-EM_MAX_SEIZURE_DEBOUNCE)  /* 140 -290 */

#define EMOC_MIN_WINK_TO_DTMF       (70-EM_SEIZURE_DEBOUNCE)
#define EMOC_WINK_TO_DTMF           (100-EM_SEIZURE_DEBOUNCE) /* 70-??  */

#define EMOC_MIN_ACK_WINK_TO_ANSWER (250-EM_MIN_SEIZURE_DEBOUNCE)
#define EMOC_MAX_ACK_WINK_TO_ANSWER  300 /* 250-??  */

#define EMOC_MAX_DTMF_DURATION      60000 /* 0-60000 */


/*
 * The Loop Start & Ground Start signaling timing definitions in ms
 */
#define LSGS_FAST_RING_FLAG         1
#define LSGS_MIN_RINGING_TIME       1000
#define LSGS_MAX_RINGING_TIME       4200 /* 1000 - 4200 */
#define LSGS_MAX_WAIT_CHANNEL_LOCK  30   /* 30 ms */
#define LSGS_MAX_WAIT_FOR_DIALTONE  10000
#define LSGS_MAX_LAST_TONE_DIALED   3000
#define LSGS_MAX_RING_DELAY_TO_CONNECT 40

/*****************************************
 * CT1 definitions:
 *   The changes in this area should also reflect in
 *   the file if_as5200_modem.h of system source code.
 */
typedef enum {
    EVENT_BIT_STATE_CHANGE=0,
    EVENT_ENABLE_CHANNEL,
    EVENT_DISABLE_CHANNEL,
    EVENT_CALL_IDLE,
    EVENT_CALL_DIAL_IN,
    EVENT_CALL_DIAL_OUT,
    EVENT_CHANNEL_LOCK,
    EVENT_CHANNEL_CONNECTED,
    EVENT_START_RX_TONE,
    EVENT_START_TX_TONE,
    EVENT_VOICE_ANS,
    EVENT_RBS_DEBUG_ON,
    EVENT_RBS_DEBUG_OFF,
    EVENT_MAX_INDEX
} signal_event_t;

/****************************************/

typedef enum {
    NULL_SIGNAL_TYPE=0,
    E_AND_M_FGB,
    E_AND_M_FGD,
    FXS_GROUND_START,
    FXS_LOOP_START,
    SAS_GROUND_START,
    SAS_LOOP_START,
} robbed_bit_signaling_t;

typedef enum {
	 SIGNAL_BIT_0=0,
	 SIGNAL_BIT_1,
	 NO_SIGNAL_BIT
} signal_bit_val_t;

typedef enum {
    NULL_BIT_STATE            = 0,
    NO_BIT_STATE_CHANGE       = BITL(1),
    /*
     * E & M bit states
     */
    LOOP_OPEN                 = BITL(2), /* A = 0, B = X */
    LOOP_CLOSURE              = BITL(3), /* A = 1, B = X */
    /*
     * FXS Loop start & Ground Start
     *
     * LOOP_OPEN:    A = 0, B = 1.
     * LOOP_CLOSURE: A = 1, B = 1.
     */
    RING_GROUND               = BITL(4), /* A = 0, B = 0 */    
    TIP_OPEN                  = BITL(5), /* A = 1, B = X */
    TIP_GROUND_RING           = BITL(6), /* A = 0, B = 0 */
    TIP_GROUND_NORING         = BITL(7), /* A = 0, B = 1 */
    /*
     * SAS Loop start & Ground Start
     *
     * LOOP_OPEN:        A = 0, B = 0
     * LOOP_CLOSURE:     A = 1, B = 0
     * RING_GROUND       A = 0, B = 1
     * TIP_OPEN          A = 0, B = X
     * TIP_GROUND_RING   A = 1, B = 0 
     * TIP_GROUND_NORING A = 1, B = 1 
     */

    /*
     * Combination of states for rx state
     */
    TIP_GROUND                = (TIP_GROUND_RING | TIP_GROUND_NORING),   /* A = 0, B = X */
    TIP_RINGING               = TIP_GROUND                               /* A = 0, B = X */
} robbed_bit_state_t;

typedef struct signal_info_t_ signal_info_t;
typedef void (*callstate_func_t) (signal_info_t *signal_info, signal_event_t event);
typedef boolean (*action_func_t) (signal_info_t *signal_info);

typedef struct state_table_t_ {
    robbed_bit_state_t input_state;
    robbed_bit_state_t output_state;
    action_func_t      action_func;
    action_func_t      recovery_func;
    ulong              min_guard_time;
    ulong              max_guard_time;
} state_table_t;

struct signal_info_t_ { 
    struct signal_info_t_   *next;
    struct signal_info_t_   *prev;
    uchar                   controller;
    uchar                   channel;
    uchar                   modem_slot;
    uchar                   modem_port;
    ushort                  call_id;
    robbed_bit_state_t      new_rx_abcd_bits;
    robbed_bit_state_t      current_rx_abcd_bits;
    robbed_bit_state_t      rx_abcd_bits_expected;
    robbed_bit_state_t      old_rx_abcd_bits_expected;
    robbed_bit_state_t      current_tx_abcd_bits;
    robbed_bit_state_t      old_tx_abcd_bits;
    ushort                  status;
    callstate_func_t        callstate_func;
    robbed_bit_signaling_t  signaling_type;
    state_table_t          *current_state;   /* null when idle */
    timer_t                 guard_timer;
    timer_t                 hit_timer;
    ulong                   fallback_max_guard_time;
    ulong                   current_min_guard_time;
    ushort                  lock_waiting_index;
    uchar                   wait_for_channel_lock_count;
};

extern void rbs_rx_abcd_bit_signal(uchar controller, uchar channel, uchar abcd_bit_val);
extern void rbs_rx_ab_bit_signal(uchar controller, uchar channel, uchar ab_bit_val);
extern void rbs_enable_signal_channel(uchar controller, uchar channel,
                                      robbed_bit_signaling_t signal_type);
extern void rbs_disable_signal_channel(uchar controller, uchar channel);
extern void rbs_init_signal_info(void);
extern void rbs_t1_framing_down(uchar controller);
extern void rbs_t1_framing_up(uchar controller);
extern void rbs_process(void);

#endif /* __ROBBED_BIT_H__ */
