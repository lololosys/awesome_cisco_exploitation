/* $Id: robbed_bit.c,v 1.8.6.1 1997/04/01 19:51:27 jchlin Exp $
 * $Source: /cscka/t1_neat/src/common/robbed_bit.c,v $
 *------------------------------------------------------------------
 * robbed_bit.c -- NEAT Processor's Robbed Bit Signaling Routines
 *
 * April, 1996 - Jonathan Chiung-Hsi Lin
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: robbed_bit.c,v $
 * Revision 1.8.6.1  1997/04/01 19:51:27  jchlin
 * o       CSCdi85287: busyout the channel after call is disconnected.
 * o       CSCdi91038: LS/GS is waiting 1 second to answer the IC to
 *                     avoid false signalling now.
 * o       CSCdi92824: Fix the race conditiion between CO and CSM during
 *                     disconnecting the modem calls.
 * o       CSCdj00011: Sending Ground Ring under GS provision when the time
 *                     slot is busyout.
 *
 * Revision 1.8  1996/10/16 18:46:02  jchlin
 * CSCdi72629:
 * Loose the timing check for the ringing cadence and do the fast ringing
 * for loopstart and groundstart incoming calls to make the incoming
 * call setup time shorter.
 *
 * Revision 1.7  1996/10/10  23:45:24  jchlin
 * o       Set the framer with default value of SF and AMI during the
 *         initialization of framer.
 * o       Enable the robbed bit interrupt mask in framer chip.
 * o       Change the timer timeout process so that it can process
 *         the timeout timer in the iteration of certain number
 *         before it releases CPU control.
 *
 * Revision 1.6  1996/09/10  02:53:08  jchlin
 * Add thw following new features:
 * o       busy dsx0 back to CO trunk when there's no free modem available
 *         in the system
 * o       Support E&M feature group D, FXS loop start and ground start,
 *         and SAS loop start and ground start.
 *
 * Revision 1.5  1996/07/13  05:32:04  towu
 * o Modified state machine array tables.
 * o Code clean up.
 *
 * Revision 1.4  1996/06/19  18:26:36  jchlin
 * o       Make sure that the incoming call will not be winked if
 *         there's no free modem to accept the call.
 * o       Use the right variable to check the framing types.
 *
 * Revision 1.3  1996/05/12  22:24:55  jchlin
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
 * Revision 1.2  1996/05/09  20:35:50  jchlin
 * The initial commit for the robbed bit signaling protocol engine.
 * The engine is designed in a table driven functions which can be
 * extended easily to support different rbs protocols in the future.
 * The initial vserion will support the E&M type II, FGB & FBD.
 *
 * Revision 1.1  1996/04/05  00:34:31  jchlin
 * Placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifdef T1_FIRMWARE
#include "t1_headers.h"
#else
#include "e1_headers.h"
#endif

#define static     /*
                    * make all functions extern so that the symbol table
                    * can record the address. It's for debug purpose
                    */

#define MAX_CONTROLLER   MAX_T1
#define MAX_B_CHANNEL    24
#define MAX_LOCK_WAITING_INDEX (MAX_CONTROLLER*MAX_B_CHANNEL)

static void tx_abcd_bit_signal(signal_info_t *signal_info, 
                               robbed_bit_state_t new_signal_to_tx);
static void send_event_to_host(signal_info_t  *signal_info, signal_event_t event,
                               ushort cause, neat_ipc_msg_priority priority);
static void receive_event_from_host(csm_message_t *csm_message);
static void delayed_dialout_handler(timer_t *timer,
                                    signal_info_t *signal_info, void *para2);
static boolean do_action_glare(signal_info_t *signal_info);
static boolean do_action_rx_mf(signal_info_t *signal_info);
static boolean do_action_tx_mf(signal_info_t *signal_info);
static boolean do_action_connect(signal_info_t *signal_info);
static boolean do_action_idle(signal_info_t *signal_info);
static boolean do_action_fallback(signal_info_t *signal_info);
static boolean do_action_init_chnl_lock(signal_info_t *signal_info);
static boolean do_action_check_channel_lock(signal_info_t *signal_info);
static boolean do_action_ignore(signal_info_t *signal_info);

static void call_idle_state(signal_info_t *signal_info,
			   signal_event_t event);
static void call_disconnect_state(signal_info_t *signal_info, 
				  signal_event_t event);
static void call_signaling_state(signal_info_t *signal_info, 
				 signal_event_t event);
static void action_execution(signal_info_t *signal_info);


static ushort global_call_id;
static signal_info_t *signal_change_head=NULL;
static signal_info_t *free_channel_head=NULL;
static signal_info_t all_signal_info[MAX_CONTROLLER][MAX_B_CHANNEL];
static signal_info_t *lock_waiting_list[MAX_LOCK_WAITING_INDEX];
static ushort lock_waiting_index;
static boolean debug_rbs=FALSE;

/**************************************************************************************************************************
 * RBS State machine tables
 *
 */
static const state_table_t EM_FGB_ic_state[] = {
/*                                                                                    next             next
 * input_state           output_state       action_func     recovery_func       min_guard_time(ms) max_guard_time(ms)
 */
{ LOOP_CLOSURE,        LOOP_OPEN,           NULL,           do_action_idle,        T_NONE,         EM_SEIZURE_DEBOUNCE  },
{ NO_BIT_STATE_CHANGE, NO_BIT_STATE_CHANGE, do_action_init_chnl_lock,  do_action_idle, T_NONE,     EMIC_SEIZURE_TO_WINK },
{ NO_BIT_STATE_CHANGE, LOOP_CLOSURE,        do_action_check_channel_lock,do_action_idle, T_NONE,   EMIC_WINK_START_DELAY}, 
{ NO_BIT_STATE_CHANGE, LOOP_OPEN,           do_action_rx_mf,do_action_idle,        T_NONE,         EMIC_WINK_TO_DTMF    },
{ NO_BIT_STATE_CHANGE, NO_BIT_STATE_CHANGE, NULL,           do_action_idle,        T_NONE,         EMIC_DTMF_DURATION   },
{ NO_BIT_STATE_CHANGE, NO_BIT_STATE_CHANGE, NULL,           do_action_idle,        T_NONE,         EMIC_STONE_TO_ANSWER },
{ NO_BIT_STATE_CHANGE, LOOP_CLOSURE,        do_action_connect, do_action_idle,     T_NONE,                T_NONE},
{ LOOP_OPEN,           NO_BIT_STATE_CHANGE, NULL,           NULL,                  T_NONE,         EMIC_ELEAD_DISC      },
{ NO_BIT_STATE_CHANGE, NO_BIT_STATE_CHANGE, NULL,           do_action_fallback,    T_NONE,         EMIC_ELEAD2MLEAD_DISC},
{ NO_BIT_STATE_CHANGE, LOOP_OPEN,           do_action_idle, do_action_idle,        T_NONE,                T_NONE},
{ NULL_BIT_STATE,      NULL_BIT_STATE,      do_action_ignore, do_action_ignore,    T_NONE,                T_NONE},
};

static const state_table_t EM_FGB_oc_state[] = {
/*                                                                                    next             next
 * input_state           output_state       action_func     recovery_func       min_guard_time(ms) max_guard_time(ms)
 */
{ NO_BIT_STATE_CHANGE, LOOP_CLOSURE,        NULL,           do_action_idle,     EMOC_MIN_ELEAD_SEIZURE,  EMOC_MAX_ELEAD_SEIZURE },
{ LOOP_CLOSURE,        NO_BIT_STATE_CHANGE, NULL,           do_action_idle,     EM_MIN_SEIZURE_DEBOUNCE, EM_MAX_SEIZURE_DEBOUNCE},
{ NO_BIT_STATE_CHANGE, NO_BIT_STATE_CHANGE, NULL,           do_action_idle,     EMOC_MIN_WINK_START_DELAY, EMOC_MAX_WINK_START_DELAY},
{ LOOP_OPEN,           NO_BIT_STATE_CHANGE, NULL,           do_action_glare,    EM_MIN_SEIZURE_DEBOUNCE, EM_MAX_SEIZURE_DEBOUNCE},
{ NO_BIT_STATE_CHANGE, NO_BIT_STATE_CHANGE, do_action_tx_mf, do_action_fallback,   T_NONE,               EMOC_MAX_DTMF_DURATION },
{ LOOP_CLOSURE,        NO_BIT_STATE_CHANGE, NULL,           do_action_idle,     EM_MIN_SEIZURE_DEBOUNCE, EM_MAX_SEIZURE_DEBOUNCE},
{ NO_BIT_STATE_CHANGE, NO_BIT_STATE_CHANGE, do_action_connect, do_action_fallback, T_NONE,                T_NONE},
{ LOOP_OPEN,           NO_BIT_STATE_CHANGE, NULL,           NULL,               EM_MIN_SEIZURE_DEBOUNCE, EM_MAX_SEIZURE_DEBOUNCE},
{ NO_BIT_STATE_CHANGE, LOOP_OPEN,           do_action_idle, do_action_idle,        T_NONE,                T_NONE},
{ NULL_BIT_STATE,      NULL_BIT_STATE,      do_action_ignore, do_action_ignore,    T_NONE,                T_NONE},
};

static const state_table_t EM_FGD_ic_state[] = {
/*                                                                                    next             next
 * input_state           output_state       action_func     recovery_func       min_guard_time(ms) max_guard_time(ms)
 */
{ LOOP_CLOSURE,        LOOP_OPEN,           NULL,           do_action_idle,        T_NONE,         EM_SEIZURE_DEBOUNCE},
{ NO_BIT_STATE_CHANGE, NO_BIT_STATE_CHANGE, do_action_init_chnl_lock, do_action_idle, T_NONE,      EMIC_SEIZURE_TO_WINK},
{ NO_BIT_STATE_CHANGE, LOOP_CLOSURE,        do_action_check_channel_lock,do_action_idle, T_NONE,   EMIC_WINK_START_DELAY}, 
{ NO_BIT_STATE_CHANGE, LOOP_OPEN,           do_action_rx_mf,do_action_idle,        T_NONE,         EMIC_WINK_TO_DTMF},
{ NO_BIT_STATE_CHANGE, NO_BIT_STATE_CHANGE, NULL,           do_action_idle,        T_NONE,         EMIC_ANI_DNIS_DTMF},
{ NO_BIT_STATE_CHANGE, NO_BIT_STATE_CHANGE, NULL,           do_action_idle,        T_NONE,         EMIC_DNIS_TO_ACK_WINK},
{ NO_BIT_STATE_CHANGE, LOOP_CLOSURE,        NULL,           do_action_idle,        T_NONE,         EMIC_ACK_WINK_START}, 
{ NO_BIT_STATE_CHANGE, LOOP_OPEN,           NULL,           do_action_idle,        T_NONE,         EMIC_ACK_WINK_TO_ANSWER},
{ NO_BIT_STATE_CHANGE, LOOP_CLOSURE,        do_action_connect, do_action_idle,     T_NONE,                T_NONE},
{ LOOP_OPEN,           NO_BIT_STATE_CHANGE, NULL,           NULL,                  T_NONE,         EMIC_ELEAD_DISC},
{ NO_BIT_STATE_CHANGE, NO_BIT_STATE_CHANGE, NULL,           do_action_fallback,    T_NONE,         EMIC_ELEAD2MLEAD_DISC},
{ NO_BIT_STATE_CHANGE, LOOP_OPEN,           do_action_idle, do_action_idle,        T_NONE,                T_NONE}, 
{ NULL_BIT_STATE,      NULL_BIT_STATE,      do_action_ignore, do_action_ignore,    T_NONE,                T_NONE},
};

static const state_table_t EM_FGD_oc_state[] = {
/*                                                                                    next             next
 * input_state           output_state       action_func     recovery_func       min_guard_time(ms) max_guard_time(ms)
 */
{ NO_BIT_STATE_CHANGE, LOOP_CLOSURE,        NULL,           do_action_idle,   EMOC_MIN_ELEAD_SEIZURE,    EMOC_MAX_ELEAD_SEIZURE},
{ LOOP_CLOSURE,        NO_BIT_STATE_CHANGE, NULL,           do_action_idle,   EM_MIN_SEIZURE_DEBOUNCE,   EM_MAX_SEIZURE_DEBOUNCE}, 
{ NO_BIT_STATE_CHANGE, NO_BIT_STATE_CHANGE, NULL,           do_action_idle,   EMOC_MIN_WINK_START_DELAY, EMOC_MAX_WINK_START_DELAY}, 
{ LOOP_OPEN,           NO_BIT_STATE_CHANGE, NULL,           do_action_glare,  EM_MIN_SEIZURE_DEBOUNCE,   EM_MAX_SEIZURE_DEBOUNCE},
{ NO_BIT_STATE_CHANGE, NO_BIT_STATE_CHANGE, NULL,           do_action_fallback, EMOC_MIN_WINK_TO_DTMF,   EMOC_WINK_TO_DTMF},
{ NO_BIT_STATE_CHANGE, NO_BIT_STATE_CHANGE, do_action_tx_mf, do_action_idle,       T_NONE,               EMOC_MAX_DTMF_DURATION},

{ LOOP_CLOSURE,        NO_BIT_STATE_CHANGE, NULL,           do_action_idle,   EM_MIN_SEIZURE_DEBOUNCE,   EM_MAX_SEIZURE_DEBOUNCE}, 
{ NO_BIT_STATE_CHANGE, NO_BIT_STATE_CHANGE, NULL,           do_action_fallback, EMOC_MIN_ACK_WINK_START, EMOC_MAX_ACK_WINK_START}, 
{ LOOP_OPEN,           NO_BIT_STATE_CHANGE, NULL,           do_action_idle,   EM_MIN_SEIZURE_DEBOUNCE,   EM_MAX_SEIZURE_DEBOUNCE},
{ NO_BIT_STATE_CHANGE, NO_BIT_STATE_CHANGE, NULL,           do_action_fallback, EMOC_MIN_ACK_WINK_TO_ANSWER, EMOC_MAX_ACK_WINK_TO_ANSWER}, 

{ LOOP_CLOSURE,        NO_BIT_STATE_CHANGE, NULL,           do_action_idle,   EM_MIN_SEIZURE_DEBOUNCE,   EM_MAX_SEIZURE_DEBOUNCE},
{ NO_BIT_STATE_CHANGE, NO_BIT_STATE_CHANGE, do_action_connect, do_action_fallback, T_NONE,                T_NONE},
{ LOOP_OPEN,           NO_BIT_STATE_CHANGE, NULL,           NULL,             EM_MIN_SEIZURE_DEBOUNCE,   EM_MAX_SEIZURE_DEBOUNCE},
{ NO_BIT_STATE_CHANGE, LOOP_OPEN,           do_action_idle, do_action_idle,        T_NONE,                T_NONE},
{ NULL_BIT_STATE,      NULL_BIT_STATE,      do_action_ignore, do_action_ignore,    T_NONE,                T_NONE},
};

static const state_table_t FXS_GS_ic_state[] = {
/*                                                                                    next             next
 * input_state           output_state       action_func     recovery_func       min_guard_time(ms) max_guard_time(ms)
 */
#if LSGS_FAST_RING_FLAG
{ TIP_RINGING,          LOOP_OPEN,           NULL,           do_action_idle,       T_NONE,              LSGS_MIN_RINGING_TIME},
{ NO_BIT_STATE_CHANGE,  NO_BIT_STATE_CHANGE, NULL,           do_action_idle,       T_CONTINUE,          T_CONTINUE},
#else
{ TIP_RINGING,          LOOP_OPEN,           NULL,           do_action_idle,   LSGS_MIN_RINGING_TIME,   LSGS_MAX_RINGING_TIME},
{ TIP_RINGING,          NO_BIT_STATE_CHANGE, NULL,           do_action_idle,   LSGS_MIN_RINGING_TIME,   LSGS_MAX_RINGING_TIME},
{ TIP_RINGING,          NO_BIT_STATE_CHANGE, NULL,           do_action_idle,       T_CONTINUE,          T_CONTINUE},
#endif
{(TIP_RINGING|NO_BIT_STATE_CHANGE), 
                        NO_BIT_STATE_CHANGE, do_action_init_chnl_lock, do_action_idle,  T_NONE,         LSGS_MAX_WAIT_CHANNEL_LOCK},
{(TIP_RINGING|NO_BIT_STATE_CHANGE),
                        NO_BIT_STATE_CHANGE, do_action_check_channel_lock, do_action_idle, T_CONTINUE,  T_CONTINUE},
{(TIP_RINGING|NO_BIT_STATE_CHANGE),
                        NO_BIT_STATE_CHANGE, do_action_rx_mf, do_action_idle,      T_NONE,              LSGS_MAX_RING_DELAY_TO_CONNECT},
{(TIP_RINGING|NO_BIT_STATE_CHANGE),
                       LOOP_CLOSURE,        do_action_connect, do_action_idle,     T_NONE,              T_NONE},

{ TIP_OPEN,            LOOP_OPEN,           do_action_idle, do_action_fallback,    T_NONE,              T_NONE},
{ NULL_BIT_STATE,      NULL_BIT_STATE,      do_action_ignore, do_action_ignore,    T_NONE,                T_NONE},
};

static const state_table_t FXS_GS_oc_state[] = {
/*                                                                                    next             next
 * input_state           output_state       action_func     recovery_func       min_guard_time(ms) max_guard_time(ms)
 */
{ NO_BIT_STATE_CHANGE, RING_GROUND,         NULL,           do_action_idle,        T_NONE,               LSGS_MAX_WAIT_FOR_DIALTONE},
{ TIP_GROUND,          LOOP_CLOSURE,        do_action_tx_mf,do_action_idle,        T_NONE,               LSGS_MAX_LAST_TONE_DIALED},
{(TIP_GROUND|NO_BIT_STATE_CHANGE), 
                       NO_BIT_STATE_CHANGE, do_action_connect, do_action_idle,     T_NONE,               T_NONE},

{ TIP_OPEN,            LOOP_OPEN,           do_action_idle, do_action_fallback,    T_NONE,               T_NONE},
{ NULL_BIT_STATE,      NULL_BIT_STATE,      do_action_ignore, do_action_ignore,    T_NONE,                T_NONE},
};

static const state_table_t FXS_LS_ic_state[] = {
/*                                                                                    next             next
 * input_state           output_state       action_func     recovery_func       min_guard_time(ms) max_guard_time(ms)
 */
#if LSGS_FAST_RING_FLAG
{ TIP_RINGING,         LOOP_OPEN,           NULL,           do_action_idle,        T_NONE,               LSGS_MIN_RINGING_TIME},
{ NO_BIT_STATE_CHANGE, NO_BIT_STATE_CHANGE, NULL,           do_action_idle,        T_CONTINUE,           T_CONTINUE},
#else
{ TIP_RINGING,         LOOP_OPEN,           NULL,           do_action_idle,   LSGS_MIN_RINGING_TIME,     LSGS_MAX_RINGING_TIME},
{ TIP_RINGING,         NO_BIT_STATE_CHANGE, NULL,           do_action_idle,   LSGS_MIN_RINGING_TIME,     LSGS_MAX_RINGING_TIME},
{ TIP_RINGING,         NO_BIT_STATE_CHANGE, NULL,           do_action_idle,        T_CONTINUE,           T_CONTINUE},
#endif
{(TIP_RINGING|NO_BIT_STATE_CHANGE),
                        NO_BIT_STATE_CHANGE, do_action_init_chnl_lock, do_action_idle,  T_NONE,          LSGS_MAX_WAIT_CHANNEL_LOCK},
{(TIP_RINGING|NO_BIT_STATE_CHANGE),
                        NO_BIT_STATE_CHANGE, do_action_check_channel_lock, do_action_idle, T_CONTINUE,   T_CONTINUE},
{(TIP_RINGING|NO_BIT_STATE_CHANGE),
                        NO_BIT_STATE_CHANGE, do_action_rx_mf, do_action_idle,      T_NONE,               LSGS_MAX_RING_DELAY_TO_CONNECT},
{(TIP_RINGING|NO_BIT_STATE_CHANGE),
                       LOOP_CLOSURE,        do_action_connect, do_action_idle,     T_NONE,               T_NONE},

{(TIP_RINGING|NO_BIT_STATE_CHANGE),         
                       NO_BIT_STATE_CHANGE, do_action_fallback, do_action_idle,    T_NONE,               T_NONE},
{ NULL_BIT_STATE,      NULL_BIT_STATE,      do_action_ignore, do_action_ignore,    T_NONE,                T_NONE},
};

static const state_table_t FXS_LS_oc_state[] = {
/*                                                                                    next             next
 * input_state           output_state       action_func     recovery_func       min_guard_time(ms) max_guard_time(ms)
 */
{ NO_BIT_STATE_CHANGE, LOOP_CLOSURE,        do_action_tx_mf, NULL,                 T_NONE,               LSGS_MAX_LAST_TONE_DIALED},
{ NO_BIT_STATE_CHANGE, NO_BIT_STATE_CHANGE, do_action_connect, do_action_idle,     T_NONE,               T_NONE},

{(TIP_GROUND|NO_BIT_STATE_CHANGE),         
                         NO_BIT_STATE_CHANGE, do_action_fallback, do_action_idle,  T_NONE,               T_NONE},
{ NULL_BIT_STATE,      NULL_BIT_STATE,      do_action_ignore, do_action_ignore,    T_NONE,                T_NONE},
};

static const state_table_t null_state[] = {
/*                                                                                    next             next
 * input_state           output_state       action_func     recovery_func       min_guard_time(ms) max_guard_time(ms)
 */
{ NULL_BIT_STATE,      NULL_BIT_STATE,      do_action_ignore, do_action_ignore,    T_NONE,                T_NONE},
};
/*
 *
 * End of RBS State machine tables
 **************************************************************************************************************************/

static robbed_bit_signaling_t get_rx_onhook_abcd_bits(signal_info_t *signal_info)
{
    switch (signal_info->signaling_type) {
    case E_AND_M_FGB:
    case E_AND_M_FGD:
         return (LOOP_OPEN);
        
    case FXS_GROUND_START:
    case SAS_GROUND_START:
         return (TIP_OPEN);

    case FXS_LOOP_START:
    case SAS_LOOP_START:
         return (TIP_GROUND_NORING);

    default:
         return (NULL_BIT_STATE);
    }
}

static inline robbed_bit_signaling_t get_tx_onhook_abcd_bits_inline(signal_info_t *signal_info)
{
    return (LOOP_OPEN);
}

static inline robbed_bit_signaling_t get_tx_offhook_abcd_bits_inline(signal_info_t *signal_info)
{
    return (LOOP_CLOSURE);
}

static inline robbed_bit_signaling_t get_tx_busy_abcd_bits_inline(signal_info_t
                                                                  *signal_info)
{
    switch (signal_info->signaling_type) {
    case E_AND_M_FGB:
    case E_AND_M_FGD:
    case FXS_LOOP_START:
    case SAS_LOOP_START:
         return (LOOP_CLOSURE);
 
    case FXS_GROUND_START:
    case SAS_GROUND_START:
         return (RING_GROUND);

    default:
         return (NULL_BIT_STATE);
    }
}
 
static void insert_free_channel (signal_info_t *signal_info)
{
    ushort level_saved;
 
    level_saved = set_ipl(PRIO_FRAMER);

    if (IS_ENLISTED(signal_info) || !IS_CHANNEL_CONFIGURED(signal_info)) {
        reset_ipl(level_saved);
	return;
    }

    if (free_channel_head)
        dlink_node_insert_prev((dlink_node_t*) free_channel_head, (dlink_node_t*) signal_info);
    else
        free_channel_head = signal_info;
    signal_info->status |= STATUS_FREE;
    reset_ipl(level_saved);
}

static signal_info_t *delete_free_channel (signal_info_t *signal_info)
{
    ushort level_saved;
 
    level_saved = set_ipl(PRIO_FRAMER);

    if (!signal_info)
        signal_info = free_channel_head;

    if (!signal_info || !IS_CHANNEL_FREE(signal_info)) {
        reset_ipl(level_saved);
        return (NULL);
    }

    dlink_node_delete((dlink_node_t**) &free_channel_head, (dlink_node_t*) signal_info);
    signal_info->status &= ~STATUS_FREE;
    reset_ipl(level_saved);
    return (signal_info);
}

static void insert_state_change_list (signal_info_t *signal_info)
{
    ushort level_saved;
 
    level_saved = set_ipl(PRIO_FRAMER);

    if (IS_BIT_STATE_CHANGED(signal_info) || !IS_CHANNEL_CONFIGURED(signal_info)) {
        reset_ipl(level_saved);
	return;
    }

    if (signal_change_head)
        dlink_node_insert_prev((dlink_node_t*) signal_change_head, (dlink_node_t*) signal_info);
    else
        signal_change_head = signal_info;
    signal_info->status |= STATUS_BIT_STATE_CHANGED;
    reset_ipl(level_saved);
}

static signal_info_t * delete_state_change_list (signal_info_t *signal_info)
{
    ushort level_saved;
 
    level_saved = set_ipl(PRIO_FRAMER);

    if (!signal_info)
        signal_info = signal_change_head;

    if (!IS_BIT_STATE_CHANGED(signal_info)) {
        reset_ipl(level_saved);
	return (NULL);
    }

    dlink_node_delete((dlink_node_t**) &signal_change_head, (dlink_node_t*) signal_info);
    signal_info->status &= ~STATUS_BIT_STATE_CHANGED;
    reset_ipl(level_saved);
    return (signal_info);
}

static void rbs_allocate_signal_channel (signal_info_t *signal_info, ushort cause)
{
    
    T1InfoType    *t1;    
    ushort         level_saved;

    t1 = &Interface[signal_info->controller];

    /*
     * initialize the new line state
     */
    tx_abcd_bit_signal(signal_info, get_tx_onhook_abcd_bits_inline(signal_info));

    if (IS_CHANNEL_CONFIGURED(signal_info))
        return;

    level_saved = set_ipl(PRIO_FRAMER);

    if (t1->Carrier->carrier_state != CARRIER_UP) {
        signal_info->status = STATUS_FRAMING_DOWN;
        reset_ipl(level_saved);
        return;
    }

    signal_info->status = STATUS_CONFIGURED;
    reset_ipl(level_saved);
    insert_free_channel(signal_info);
    send_event_to_host(signal_info, EVENT_ENABLE_CHANNEL, cause, NEAT_IPC_MSG_PRI_LO);
}

void rbs_deallocate_signal_channel (signal_info_t *signal_info, ushort cause)
{
    ushort level_saved;
 
    if (!(signal_info->status & (STATUS_CONFIGURED|STATUS_FRAMING_DOWN|STATUS_DYNAMIC_BUSYOUT)))
        return;

    /*
     * busyout the line when it's deallocated
     */
    tx_abcd_bit_signal(signal_info, get_tx_busy_abcd_bits_inline(signal_info));

    if (!IS_CHANNEL_IDLE(signal_info) || IS_DELAY_DIALOUT(signal_info))
        (*signal_info->callstate_func)(signal_info, EVENT_CALL_IDLE);

    if (IS_DYNAMIC_BUSYOUT(signal_info) && (cause == NO_CAUSE))
        send_event_to_host(signal_info, EVENT_ENABLE_CHANNEL,
                           USER_BUSY, NEAT_IPC_MSG_PRI_LO);

    if (IS_CHANNEL_CONFIGURED(signal_info) || IS_DYNAMIC_BUSYOUT(signal_info))
        send_event_to_host(signal_info, EVENT_DISABLE_CHANNEL, cause, NEAT_IPC_MSG_PRI_LO);

    /*
     * delete from the lists after events are sent.
     */
    delete_free_channel(signal_info);
    delete_state_change_list(signal_info);
    level_saved = set_ipl(PRIO_FRAMER);
    if (cause == USER_BUSY) {
        signal_info->status &= ~(STATUS_CONFIGURED|STATUS_FRAMING_DOWN);
        signal_info->status |= STATUS_DYNAMIC_BUSYOUT;
    }
    else
        signal_info->status &= ~(STATUS_CONFIGURED|STATUS_FRAMING_DOWN|STATUS_DYNAMIC_BUSYOUT);
    reset_ipl(level_saved);
      
    if (TIMER_IS_RUNNING(&signal_info->hit_timer))
        timer_delete(&signal_info->hit_timer);
}

void rbs_enable_signal_channel (uchar controller, uchar channel, 
                                robbed_bit_signaling_t signal_type)
{
    signal_info_t *signal_info;

    signal_info = &all_signal_info[controller][channel];
    if (signal_type == signal_info->signaling_type)
        return;

    signal_info->signaling_type = signal_type; /* becomes effective next call */
    rbs_allocate_signal_channel(signal_info, NO_CAUSE);
}

void rbs_disable_signal_channel (uchar controller, uchar channel)
{
    signal_info_t *signal_info;

    signal_info = &all_signal_info[controller][channel];    
    rbs_deallocate_signal_channel(signal_info, NO_CAUSE);
    signal_info->signaling_type = NULL_SIGNAL_TYPE;
}

void rbs_init_signal_info (void)
{
    uchar ctrl, channel;
    signal_info_t *signal_info;

    global_call_id     = 0;
    signal_change_head = NULL;
    lock_waiting_index   = 0;

    neat_ipc_message_register(MSG_MPU_CSM2RBS,
                              (neat_ipc_msg_function_t) receive_event_from_host,
                              NEAT_IPC_MSG_PRI_LO);

    for (ctrl=0; ctrl < MAX_CONTROLLER; ctrl++) {
	for (channel=0; channel < MAX_B_CHANNEL; channel++) {
	    signal_info = &all_signal_info[ctrl][channel];
	    dlink_node_init((dlink_node_t*)signal_info);
	    signal_info->controller = ctrl;
	    signal_info->channel    = channel;
	    signal_info->call_id = 0;
	    signal_info->new_rx_abcd_bits = signal_info->current_rx_abcd_bits
		= signal_info->rx_abcd_bits_expected = signal_info->old_rx_abcd_bits_expected
                    = NO_BIT_STATE_CHANGE;
            signal_info->current_tx_abcd_bits = signal_info->old_tx_abcd_bits 
                = get_tx_onhook_abcd_bits_inline(signal_info);
	    signal_info->status = 0;
	    signal_info->modem_slot     = UNKNOWN_B_CHANNEL;
	    signal_info->modem_port     = UNKNOWN_B_CHANNEL;
	    signal_info->callstate_func = call_idle_state;
	    signal_info->signaling_type = NULL_SIGNAL_TYPE;   /* default */
	    signal_info->current_state  = (state_table_t*) null_state;
	    signal_info->fallback_max_guard_time = 0;
            signal_info->current_min_guard_time  = 0;
	    timer_init(&signal_info->guard_timer);
	    timer_init(&signal_info->hit_timer);
	}
    }
}

void rbs_t1_framing_down (uchar controller) 
{
    uchar channel;
    signal_info_t *signal_info;
    ushort level_saved;

    for (channel=0; channel < MAX_B_CHANNEL; channel++) {
        signal_info = &all_signal_info[controller][channel];
        /*
         * shutdown channels
         */
        if (IS_CHANNEL_CONFIGURED(signal_info) || IS_DYNAMIC_BUSYOUT(signal_info)) {
            rbs_deallocate_signal_channel(signal_info, NO_CAUSE);
            level_saved = set_ipl(PRIO_FRAMER);            
            signal_info->status |= STATUS_FRAMING_DOWN;
            reset_ipl(level_saved);
        }
    }
}

void rbs_t1_framing_up (uchar controller) 
{
    uchar channel;
    signal_info_t *signal_info;

    for (channel=0; channel < MAX_B_CHANNEL; channel++) {
        signal_info = &all_signal_info[controller][channel];
        /*
         * enable channels
         */
        if (IS_FRAMING_DOWN(signal_info))
            rbs_allocate_signal_channel(signal_info, NO_CAUSE);
    }
}

static void rbs_t1_busyout (ushort num_of_channel)
{
    signal_info_t *signal_info;

    while (num_of_channel--) {
        signal_info = free_channel_head;
        if (signal_info && !IS_DYNAMIC_BUSYOUT(signal_info)) {
            rbs_deallocate_signal_channel(signal_info, USER_BUSY);
        } else
            break;
    }
}

static void rbs_t1_unbusyout (ushort num_of_channel)
{
    uchar controller, channel;
    signal_info_t *signal_info;

    for (controller=0; (controller < MAX_CONTROLLER) && num_of_channel; controller++) {
        for (channel=0; (channel < MAX_B_CHANNEL) && num_of_channel; channel++) {
            signal_info = &all_signal_info[controller][channel];
            if (IS_DYNAMIC_BUSYOUT(signal_info)) {
                rbs_allocate_signal_channel(signal_info, USER_BUSY);
                num_of_channel--;
            }
        }
    }
}

static signal_info_t* recover_busyout_signal_channel (void)
{
    uchar controller, channel;
    signal_info_t    *signal_info;
 
    /*
     * When there's time, a change can be made so that the temporarily 
     * disabled channel can be enlisted in a list.
     */

    for (controller=0; (controller < MAX_CONTROLLER); controller++) {
        for (channel=0; (channel < MAX_B_CHANNEL); channel++) {
            signal_info = &all_signal_info[controller][channel];
            if (IS_DYNAMIC_BUSYOUT(signal_info)) {
                rbs_allocate_signal_channel(signal_info, USER_BUSY);
                return (signal_info);
            }
        }
    }
    return (NULL);
}

static void insert_to_lock_waiting_list (signal_info_t  *signal_info)
{
    ushort ix;
    ushort level_saved;

    if ((lock_waiting_index < MAX_LOCK_WAITING_INDEX) && !IS_WAITING_LOCK(signal_info)){
	ix = lock_waiting_index++;
	lock_waiting_list[ix] = signal_info;
        signal_info->lock_waiting_index = ix;
        level_saved = set_ipl(PRIO_FRAMER);
        signal_info->status |= STATUS_IN_LOCK_WAITING_LIST;
        reset_ipl(level_saved);
    }
}


static inline void delete_from_lock_waiting_list (signal_info_t  *signal_info)
{
    ushort ix;
    ushort level_saved;

    if (IS_WAITING_LOCK(signal_info)) {
	ix = signal_info->lock_waiting_index;
        if (lock_waiting_index)
            lock_waiting_list[ix] = lock_waiting_list[--lock_waiting_index];
        level_saved = set_ipl(PRIO_FRAMER);
        signal_info->status &= ~STATUS_IN_LOCK_WAITING_LIST;
        reset_ipl(level_saved);
    }
}

static signal_info_t* find_signal_info_by_call_id (ushort call_id)
{
    ushort ix;
    signal_info_t **signal_info;
    
    for (ix=0, signal_info=lock_waiting_list; ix < lock_waiting_index; ix++, signal_info++) {
	if ((*signal_info)->call_id == call_id)
            return (*signal_info);
    }
    
    return (NULL);
}


/**********************************************************************************
 * event/message functions
 */
static void send_reject_event_to_host (csm_message_t *csm_msg, signal_event_t event,
				       ushort cause)
{
	 csm_message_t csm_message;

	 csm_message.src_slot       = csm_msg->dest_slot;
	 csm_message.src_controller = csm_msg->dest_controller;
	 csm_message.src_entity     = csm_msg->dest_entity;
	 csm_message.dest_slot      = csm_msg->src_slot;
	 csm_message.dest_controller= csm_msg->src_controller;
	 csm_message.dest_entity    = csm_msg->src_entity;
	 csm_message.data.call_id   = csm_msg->data.call_id;
	 csm_message.event          = event;
	 csm_message.cause          = cause;

     {
         neat_ipc_msg_ret rc;

         rc = neat_ipc_message_send(MSG_NEAT_RBS2CSM_HI, 
                                    (uchar*) &csm_message,
                                    sizeof(csm_message_t));
         if (rc != IPCMSG_ERROR_NOERROR)
             neat_status_report("SEND_REJECT_EVENT_TO_HOST: failed rc = 0x%x", rc);
     }
}

static void send_event_to_host (signal_info_t  *signal_info, signal_event_t event,
				ushort cause, neat_ipc_msg_priority priority)
{
	 csm_message_t csm_message;

	 csm_message.src_slot       = 0;
	 csm_message.src_controller = signal_info->controller;
	 csm_message.src_entity     = signal_info->channel;
	 csm_message.dest_slot      = signal_info->modem_slot;
	 csm_message.dest_controller= 0;
	 csm_message.dest_entity    = signal_info->modem_port;
	 csm_message.data.call_id   = signal_info->call_id;
	 csm_message.event          = event;
	 csm_message.cause          = cause;

     {
         neat_ipc_msg_ret rc;
 
         rc = neat_ipc_message_send(((priority == NEAT_IPC_MSG_PRI_HI) ?
                                     MSG_NEAT_RBS2CSM_HI : MSG_NEAT_RBS2CSM_LO), 
                                    (uchar*) &csm_message,
                                    sizeof(csm_message_t));
         if (rc != IPCMSG_ERROR_NOERROR)
             neat_status_report("SEND_EVENT_TO_HOST: failed rc = 0x%x", rc);
     }
}

static void receive_event_from_host (csm_message_t *csm_message)
{
    signal_info_t  *signal_info, *signal_info_recovered;
    signal_event_t event;
    ushort call_id;
    ushort level_saved;

    event   = csm_message->event;
    call_id = csm_message->data.call_id;

    switch (event) {
    case EVENT_CALL_DIAL_OUT: 
         signal_info_recovered = recover_busyout_signal_channel();
         signal_info = delete_free_channel(free_channel_head);
         if (signal_info) {
             signal_info->modem_slot = csm_message->src_slot;
             signal_info->modem_port = csm_message->src_entity;
             signal_info->call_id    = call_id;
             insert_to_lock_waiting_list(signal_info);
             if (signal_info == signal_info_recovered) {
                 level_saved = set_ipl(PRIO_FRAMER);
                 signal_info->status |= STATUS_DELAY_DIALOUT;
                 reset_ipl(level_saved);
                 timer_start_one_shot(&signal_info->guard_timer,
                                      MIN_DELAY_DIALOUT_TIME,
                                      (callback_func_t)delayed_dialout_handler, 
                                      signal_info, NULL);
             } else
                 (*signal_info->callstate_func)(signal_info, event);
         } else
             /*
              * All channels are either busy or not available.
              */
             send_reject_event_to_host(csm_message, EVENT_CALL_IDLE,
                                       NO_CIRCUIT_AVAILABLE);
         break;

    case EVENT_DISABLE_CHANNEL:
         if (csm_message->cause == USER_BUSY)
             rbs_t1_busyout(csm_message->data.busyout_channels);
         break;

    case EVENT_ENABLE_CHANNEL:
         if (csm_message->cause == USER_BUSY)
             rbs_t1_unbusyout(csm_message->data.busyout_channels); 
         break;
         
    case EVENT_CHANNEL_LOCK:
         if ((csm_message->dest_controller == UNKNOWN_B_CHANNEL) 
             || (csm_message->dest_entity == UNKNOWN_B_CHANNEL)) {
             buginf("ERROR: EVENT_CHANNEL_LOCK: unknown dest# from (s%d/e%d)", 
                    csm_message->src_slot, csm_message->src_entity);
             return;
         }
         signal_info =
             &all_signal_info[csm_message->dest_controller][csm_message->dest_entity];         
         signal_info->modem_slot = csm_message->src_slot;
         signal_info->modem_port = csm_message->src_entity;
         break;

    case EVENT_RBS_DEBUG_ON:
         debug_rbs = TRUE;
         break;

    case EVENT_RBS_DEBUG_OFF:
         debug_rbs = FALSE;
         break;

    default:
         if (csm_message->dest_controller == UNKNOWN_B_CHANNEL) {
             signal_info = find_signal_info_by_call_id(call_id);
             if (!signal_info) {
                 send_reject_event_to_host(csm_message, EVENT_CALL_IDLE,
                                           NO_CIRCUIT_AVAILABLE);
                 return;
             }
         } else {
             signal_info = 
                 &all_signal_info[csm_message->dest_controller][csm_message->dest_entity]; 
             delete_from_lock_waiting_list(signal_info);
             if (call_id != signal_info->call_id) {
                 send_reject_event_to_host(csm_message, EVENT_CALL_IDLE,
                                           NO_CIRCUIT_AVAILABLE);
                 return;
             }
         }
         
         (*signal_info->callstate_func)(signal_info, event);
         break;
     }
}

void rbs_rx_abcd_bit_signal (uchar controller, uchar channel, uchar abcd_bit_val)
{
    /*
     * Check:
     * Didn't store the A/B/C/D bits information.
     * Either framer does it or me.
     */
    signal_info_t *signal_info;

    signal_info = &all_signal_info[controller][channel];

    if (signal_info->signaling_type == NULL_SIGNAL_TYPE)
        return;
 
    switch (signal_info->signaling_type) {
    case E_AND_M_FGB:
    case E_AND_M_FGD:
	 /*
          * EIA/TIA-464-A specifies that only A bit will
          * be verified for receiving signals when E&M is used.
          */
         if (abcd_bit_val & ROBBED_A_BIT) {
             if (signal_info->new_rx_abcd_bits == LOOP_CLOSURE)
                 return;
             signal_info->new_rx_abcd_bits = LOOP_CLOSURE;
             if (debug_rbs)
                 buginf("(%d/%d): Rx LOOP_CLOSURE", controller, channel);
         } else {
             if (signal_info->new_rx_abcd_bits == LOOP_OPEN)
                 return;
             signal_info->new_rx_abcd_bits = LOOP_OPEN;
             if (debug_rbs)                 
                 buginf("(%d/%d): Rx LOOP_OPEN", controller, channel);
         }
         break;
        
    case FXS_GROUND_START:
    case FXS_LOOP_START:
	 if (abcd_bit_val & ROBBED_A_BIT) {
             if (signal_info->new_rx_abcd_bits == TIP_OPEN)
                 return;
             /*
              * TIP_OPEN is only for Ground Start. However, some switches will use 
              * it for loop start to disconnect the call although it's not in the
              * EIA/TIA 464 spec.
              */
             signal_info->new_rx_abcd_bits = TIP_OPEN; /* A = 1, B = X */
             if (debug_rbs)
                 buginf("(%d/%d): Rx TIP_OPEN", controller, channel);
         } else if (abcd_bit_val & ROBBED_B_BIT) {
             if (signal_info->new_rx_abcd_bits == TIP_GROUND_NORING)
                 return;
             signal_info->new_rx_abcd_bits = TIP_GROUND_NORING; /* A = 0, B = 1 */
             if (debug_rbs)
                 buginf("(%d/%d): Rx TIP_GROUND_NORING", controller, channel);
         } else {
             if (signal_info->new_rx_abcd_bits == TIP_GROUND_RING)
                 return;
             signal_info->new_rx_abcd_bits = TIP_GROUND_RING; /* A = 0, B = 0 */
             if (debug_rbs)
                 buginf("(%d/%d): Rx TIP_GROUND_RING", controller, channel);
         }
         break;

    case SAS_GROUND_START:
    case SAS_LOOP_START:
	 if (!(abcd_bit_val & ROBBED_A_BIT)) {
             if (signal_info->new_rx_abcd_bits == TIP_OPEN)
                 return;
             signal_info->new_rx_abcd_bits = TIP_OPEN; /* A = 0, B = X */
             if (debug_rbs)
                 buginf("(%d/%d): Rx TIP_OPEN", controller, channel);
         } else if (abcd_bit_val & ROBBED_B_BIT) {
             if (signal_info->new_rx_abcd_bits == TIP_GROUND_NORING)
                 return;
             signal_info->new_rx_abcd_bits = TIP_GROUND_NORING; /* A = 1, B = 1 */
             if (debug_rbs)
                 buginf("(%d/%d): Rx TIP_GROUND_NORING", controller, channel);
         } else {
             if (signal_info->new_rx_abcd_bits == TIP_GROUND_RING)
                 return;
             signal_info->new_rx_abcd_bits = TIP_GROUND_RING; /* A = 1, B = 0 */
             if (debug_rbs)
                 buginf("(%d/%d): Rx TIP_GROUND_RING", controller, channel);
         }
         break;

    default:
         return;   /* unknown provisions*/
    }

    delete_free_channel(signal_info);
    insert_state_change_list(signal_info);
}

void rbs_rx_ab_bit_signal (uchar controller, uchar channel, uchar ab_bit_val)
{
   rbs_rx_abcd_bit_signal(controller, channel, (ab_bit_val & (ROBBED_A_BIT|ROBBED_B_BIT)));
}

static void tx_abcd_bit_signal (signal_info_t *signal_info, robbed_bit_state_t new_signal_to_tx)
{
    uchar   controller, channel;
    boolean use_esf;

    if (signal_info->signaling_type == NULL_SIGNAL_TYPE)
        return;

    if (signal_info->current_tx_abcd_bits != new_signal_to_tx) {
        signal_info->old_tx_abcd_bits     = signal_info->current_tx_abcd_bits;
        signal_info->current_tx_abcd_bits = new_signal_to_tx;
    }

    controller = signal_info->controller;
    channel    = signal_info->channel;
    use_esf    = (Interface[controller].Fr.FrameType == DSX1_ESF);

    switch (signal_info->signaling_type) {
    case E_AND_M_FGB:
    case E_AND_M_FGD:
	 if (new_signal_to_tx == LOOP_OPEN) {
             t1_rbs_transmit(controller, channel, 0);
             if (debug_rbs)
                 buginf("(%d/%d): Tx LOOP_OPEN", controller, channel);
         } else if (new_signal_to_tx == LOOP_CLOSURE) {
             t1_rbs_transmit(controller, channel, 
                             (use_esf)
                             ? (ROBBED_A_BIT|ROBBED_B_BIT|ROBBED_C_BIT|ROBBED_D_BIT)
                             : (ROBBED_A_BIT|ROBBED_B_BIT));
             if (debug_rbs)
                 buginf("(%d/%d): Tx LOOP_CLOSURE", controller, channel);
         }
         break;
         
    case FXS_GROUND_START:
    case FXS_LOOP_START:
         if (new_signal_to_tx == LOOP_OPEN) {
             t1_rbs_transmit(controller, channel, 
                             (use_esf)
                             ? (ROBBED_B_BIT | ROBBED_D_BIT)
                             : ROBBED_B_BIT);
             if (debug_rbs)
                 buginf("(%d/%d): Tx LOOP_OPEN", controller, channel);
         } else if (new_signal_to_tx == LOOP_CLOSURE) {
             t1_rbs_transmit(controller, channel, 
                             (use_esf)
                             ? (ROBBED_A_BIT|ROBBED_B_BIT|ROBBED_C_BIT|ROBBED_D_BIT)
                             : (ROBBED_A_BIT|ROBBED_B_BIT));
             if (debug_rbs)
                 buginf("(%d/%d): Tx LOOP_CLOSURE", controller, channel);
         } else if (new_signal_to_tx == RING_GROUND) {
             t1_rbs_transmit(controller, channel, 0);
             if (debug_rbs)
                 buginf("(%d/%d): Tx RING_GROUND", controller, channel);
         }
         break;

    case SAS_GROUND_START:
    case SAS_LOOP_START:
         if (new_signal_to_tx == LOOP_OPEN) {
             t1_rbs_transmit(controller, channel, 0);
             if (debug_rbs)
                 buginf("(%d/%d): Tx LOOP_OPEN", controller, channel);
         } else if (new_signal_to_tx == LOOP_CLOSURE) {
             t1_rbs_transmit(controller, channel, 
                             (use_esf)
                             ? (ROBBED_A_BIT|ROBBED_C_BIT)
                             : ROBBED_A_BIT);
             if (debug_rbs)
                 buginf("(%d/%d): Tx LOOP_CLOSURE", controller, channel);
         } else if (new_signal_to_tx == RING_GROUND) {
             t1_rbs_transmit(controller, channel, 
                             (use_esf)
                             ? (ROBBED_B_BIT|ROBBED_D_BIT)
                             : ROBBED_B_BIT);
             if (debug_rbs)
                 buginf("(%d/%d): Tx RING_GROUND", controller, channel);
         }
         break;

    default:
	break;
    }
}



/**********************************************************************************
 * Timer timeout handlers
 */

static void postidle_handler (timer_t *timer,
			      signal_info_t *signal_info, void *param2)
{
    int     glare_resolution;
    ushort  level_saved;
    boolean modem_allocated;

    glare_resolution = (int) param2;
    
    send_event_to_host(signal_info, EVENT_CALL_IDLE, NO_CAUSE, 
                       NEAT_IPC_MSG_PRI_HI);
    modem_allocated = (signal_info->modem_slot != UNKNOWN_B_CHANNEL);
    signal_info->modem_slot     = UNKNOWN_B_CHANNEL;
    signal_info->modem_port     = UNKNOWN_B_CHANNEL;
    signal_info->callstate_func = call_idle_state;
    level_saved = set_ipl(PRIO_FRAMER);
    signal_info->status        &= ~STATUS_CONNECTED;
    signal_info->status        |= STATUS_IDLE;
    reset_ipl(level_saved);

    delete_from_lock_waiting_list(signal_info);

    if (IS_FRAMING_DOWN(signal_info)) {
        return;
    }
    if (IS_CHANNEL_CONFIGURED(signal_info) &&
        (IS_RBS_RX_BIT_PENDING(signal_info)
        || glare_resolution)) {
        signal_info->status &= ~STATUS_RBS_RX_BIT_PENDING;
        call_idle_state(signal_info, EVENT_BIT_STATE_CHANGE);
    }
    else if (modem_allocated) {
        /*
         * if the modem is allocated, busyout the timeslot to wait
         * for deallocating modem
         */
        rbs_deallocate_signal_channel(signal_info, USER_BUSY);
    }
    else
        insert_free_channel(signal_info);
}

static void invalid_bit_change_handler (timer_t       *timer,
                                        signal_info_t *signal_info, 
                                        void          *param2)
{
    state_table_t     *current_state;
    ushort level_saved;
 
    if (signal_info->status & STATUS_VERIFY_BIT_STATE) {
        level_saved = set_ipl(PRIO_FRAMER);
        signal_info->status &= ~STATUS_VERIFY_BIT_STATE;
        reset_ipl(level_saved);
        signal_info->current_rx_abcd_bits = signal_info->new_rx_abcd_bits;
        current_state = signal_info->current_state;
        if (current_state->recovery_func)
            (*current_state->recovery_func)(signal_info);
        else
            do_action_idle(signal_info);
    }
}
 
static void valid_bit_change_handler (timer_t *timer,
				      signal_info_t *signal_info, void *param2)
{
    if (signal_info->status & STATUS_VERIFY_BIT_STATE)
        (*signal_info->callstate_func)(signal_info, EVENT_BIT_STATE_CHANGE);
}

static void guardtime_handler (timer_t *timer,
			       signal_info_t *signal_info, void *param2)
{
    state_table_t     *current_state;

    current_state = signal_info->current_state;
    if (current_state->input_state & NO_BIT_STATE_CHANGE) {
	action_execution(signal_info);
    } else {
	/*
	 * Some expected state change doesn't happen
	 * Take whatever pending state change.
	 */
	if (TIMER_IS_RUNNING(&(signal_info->hit_timer)) &&
	    (signal_info->new_rx_abcd_bits & current_state->input_state)) {
	    timer_delete(&(signal_info->hit_timer));
	    valid_bit_change_handler(&(signal_info->guard_timer),
				     signal_info, NULL);
	} else if (current_state->recovery_func)
            (*current_state->recovery_func)(signal_info);
        else
            do_action_idle(signal_info);
    }
}

static void delayed_dialout_handler (timer_t *timer,
                                     signal_info_t *signal_info, void *para2)
{
    ushort level_saved;

    level_saved = set_ipl(PRIO_FRAMER);
    signal_info->status &= ~STATUS_DELAY_DIALOUT;
    reset_ipl(level_saved);
    
    (*signal_info->callstate_func)(signal_info, EVENT_CALL_DIAL_OUT);
}

/**********************************************************************************
 * Action and support routines
 */
static const state_table_t *get_state_table (robbed_bit_signaling_t  signaling_type,
                                             boolean incoming_call)
{
    switch(signaling_type) {
    case E_AND_M_FGB:
         return (incoming_call ? EM_FGB_ic_state : EM_FGB_oc_state);

    case E_AND_M_FGD:
         return (incoming_call ? EM_FGD_ic_state : EM_FGD_oc_state);

    case FXS_GROUND_START:
    case SAS_GROUND_START:
         return (incoming_call ? FXS_GS_ic_state : FXS_GS_oc_state);

    case FXS_LOOP_START:
    case SAS_LOOP_START:
         return (incoming_call ? FXS_LS_ic_state : FXS_LS_oc_state);

    default:
         return (null_state);
    }
}

static boolean do_action_rx_mf (signal_info_t *signal_info)
{
    if (signal_info->modem_slot == UNKNOWN_B_CHANNEL)
        return (do_action_idle(signal_info));

    send_event_to_host(signal_info, EVENT_START_RX_TONE, NO_CAUSE,
                       NEAT_IPC_MSG_PRI_HI);
    return (FALSE);
}

static boolean do_action_tx_mf (signal_info_t *signal_info)
{
    if (signal_info->modem_slot == UNKNOWN_B_CHANNEL)
        return (do_action_idle(signal_info));

    send_event_to_host(signal_info, EVENT_START_TX_TONE, NO_CAUSE,
                       NEAT_IPC_MSG_PRI_HI);
    return (FALSE);
}

static boolean do_action_connect (signal_info_t *signal_info)
{
    ushort level_saved;

    if (!IS_WAITING_VOICE_ANS(signal_info))
        send_event_to_host(signal_info, EVENT_CHANNEL_CONNECTED, NO_CAUSE,
                           NEAT_IPC_MSG_PRI_HI);
    level_saved = set_ipl(PRIO_FRAMER);
    signal_info->status |= STATUS_CONNECTED;
    reset_ipl(level_saved);
    return (FALSE);
}

static boolean do_action_idle (signal_info_t *signal_info)
{
    ushort level_saved;

    level_saved = set_ipl(PRIO_FRAMER);
    signal_info->status &= ~STATUS_CONNECTED;
    reset_ipl(level_saved);
    signal_info->current_state        = (state_table_t*) null_state;
    signal_info->fallback_max_guard_time = 0;
    signal_info->current_min_guard_time = 0;
    signal_info->callstate_func       = call_disconnect_state;
    /*
     * Idle for a while to make sure the remote side is aware of the disconnection.
     */
    timer_start_one_shot(&signal_info->guard_timer,
                         ((signal_info->current_rx_abcd_bits==get_rx_onhook_abcd_bits(signal_info)) ||
                          ((signal_info->signaling_type != E_AND_M_FGB) && 
                           (signal_info->signaling_type != E_AND_M_FGD)))
                         ? MAX_DISCONNECT_DEBOUNCE_TIME : MAX_DISCONNECT_REQ_GUARD_TIME,
                         (callback_func_t)postidle_handler, signal_info, FALSE);
    tx_abcd_bit_signal(signal_info, get_tx_onhook_abcd_bits_inline(signal_info));
    return (TRUE);
}

static boolean do_action_glare (signal_info_t *signal_info)
{
    /*
     * wink start expired, start the glare resolution 
     * by foregoing the outgoing call and answering  
     * the incoming call.
     */    
    postidle_handler(&signal_info->guard_timer, signal_info, (void *)TRUE);
    return (FALSE);
}

static boolean do_action_ignore (signal_info_t *signal_info)
{
    /*
     * Don't do any action and don't change the state table pointer
     */
    if  (debug_rbs) {
        buginf("(%d/%d): ignore event", signal_info->controller, 
               signal_info->channel);
    }
    return (TRUE);
}

static boolean do_action_fallback (signal_info_t *signal_info)
{
    if (TIMER_IS_RUNNING(&signal_info->guard_timer))
	timer_delete(&signal_info->guard_timer);

    if (!signal_info->fallback_max_guard_time && (signal_info->current_state-1)->max_guard_time)
        /*
         * If the action cannot be recovered, go back to idle...
	 */
        do_action_idle(signal_info);
    else {
        if (signal_info->fallback_max_guard_time && (signal_info->current_state-1)->max_guard_time)
	    timer_start_one_shot(&signal_info->guard_timer, signal_info->fallback_max_guard_time,
				 (callback_func_t)guardtime_handler, signal_info, NULL);

        signal_info->fallback_max_guard_time = 0;
        if (!((signal_info->current_state-1)->output_state & NO_BIT_STATE_CHANGE) &&
            ((signal_info->current_state-1)->output_state != signal_info->current_tx_abcd_bits))
            tx_abcd_bit_signal(signal_info, signal_info->old_tx_abcd_bits);
        if (!((signal_info->current_state-1)->input_state & NO_BIT_STATE_CHANGE))
            signal_info->rx_abcd_bits_expected = signal_info->old_rx_abcd_bits_expected;
    }
    return (TRUE);
}

static boolean do_action_init_chnl_lock (signal_info_t *signal_info)
{
    send_event_to_host(signal_info, EVENT_CALL_DIAL_IN, NO_CAUSE, 
                       NEAT_IPC_MSG_PRI_HI);

    signal_info->wait_for_channel_lock_count = 
      (MAX_WAIT_FOR_CHANNEL_LOCK_TIME / 
       (signal_info->current_state->max_guard_time
        ? signal_info->current_state->max_guard_time 
        : CHNL_LOCK_POLLING_INTERVAL));
    
    return (FALSE);
}

static boolean do_action_check_channel_lock (signal_info_t *signal_info)
{
    if (signal_info->modem_slot == UNKNOWN_B_CHANNEL) {
        /*
         * if timer is still running, let it continue running.
         * ignore this state change...
         */
        if (TIMER_IS_RUNNING(&(signal_info->guard_timer)))
            return (TRUE);  

        if (--signal_info->wait_for_channel_lock_count)
            signal_info->current_state--;  /* wait for the EVENT_CHANNEL_LOCK */
        else
            return (do_action_idle(signal_info)); /* cannot wait any longer */
    }
    return (FALSE);
}

static void action_execution (signal_info_t *signal_info)
{
    boolean           run_next_state;
    state_table_t     *current_state;
    ulong             state_change_delay;

    do {
        run_next_state = FALSE;

        /*
         * Increase the state now based on the assumption that the signal_info->current_state
         * is not used in action_func and tx_abcd_bit_signal.
         */
        current_state = signal_info->current_state; /* get initial current_state */

        if (current_state->input_state == NULL_BIT_STATE)
            return;

        if (current_state->action_func)
            if ((*current_state->action_func)(signal_info))
                return; /* exit */
        
        current_state = signal_info->current_state;  /* current_state may be changed in action_func */
        
        if (current_state->input_state & NO_BIT_STATE_CHANGE)
            state_change_delay = 0;
        else
            state_change_delay = MAX_STATE_CHANGE_DEBOUNCE_TIME;
        
        if (!(current_state->output_state & NO_BIT_STATE_CHANGE))
            tx_abcd_bit_signal(signal_info, current_state->output_state);
        
        if (current_state->input_state & ~NO_BIT_STATE_CHANGE) {
            signal_info->old_rx_abcd_bits_expected = signal_info->rx_abcd_bits_expected;
            signal_info->rx_abcd_bits_expected = current_state->input_state;
        }
        
        if (TIMER_IS_RUNNING(&signal_info->guard_timer)) {
            signal_info->fallback_max_guard_time = timer_countdown_left(&signal_info->guard_timer);
            timer_delete(&(signal_info->guard_timer));
        } else
            signal_info->fallback_max_guard_time = 0;
        
        if (T_CONFIGURED(current_state->min_guard_time))
            signal_info->current_min_guard_time = current_state->max_guard_time - current_state->min_guard_time;
        else
            signal_info->current_min_guard_time = 0;
        
        if (T_CONFIGURED(current_state->max_guard_time))
            timer_start_one_shot(&signal_info->guard_timer, 
                                 ((current_state->max_guard_time > state_change_delay)
                                  ? (current_state->max_guard_time-state_change_delay) : 1),
                                 (callback_func_t)guardtime_handler, signal_info, NULL);
        else {
            run_next_state = T_CONTINUE_CONFIGURED(current_state->min_guard_time) 
                          && T_CONTINUE_CONFIGURED(current_state->max_guard_time);
        }
        
        signal_info->current_state++;
    } while (run_next_state);
}

/**********************************************************************************
 * Call State functions
 */
static void call_idle_state (signal_info_t *signal_info, signal_event_t event)
{
    ushort level_saved;

    /*
     * Check if there's an incoming call
     * Assume the incoming call has higher priority than outgoing call
     */
    switch(event) {
    case EVENT_BIT_STATE_CHANGE: 
         level_saved = set_ipl(PRIO_FRAMER);
         signal_info->status &= ~(STATUS_VERIFY_BIT_STATE|STATUS_CONNECTED);
         reset_ipl(level_saved);
         signal_info->current_rx_abcd_bits = signal_info->new_rx_abcd_bits;
         signal_info->current_state = 
             (state_table_t*) get_state_table(signal_info->signaling_type, INCOMING_CALL);
         if (signal_info->new_rx_abcd_bits & signal_info->current_state->input_state) {
             if (IS_DELAY_DIALOUT(signal_info)) {
                 /*
                  * glare is happening ...
                  */
                 level_saved = set_ipl(PRIO_FRAMER);
                 signal_info->status &= ~STATUS_DELAY_DIALOUT;                 
                 reset_ipl(level_saved);
                 send_event_to_host(signal_info, EVENT_CALL_IDLE, NO_CAUSE,
                                    NEAT_IPC_MSG_PRI_HI);
                 if (TIMER_IS_RUNNING(&(signal_info->guard_timer)))
                     timer_delete(&(signal_info->guard_timer));
                 delete_from_lock_waiting_list(signal_info);
             }
             level_saved = set_ipl(PRIO_FRAMER);
             signal_info->status &= ~STATUS_IDLE;
             signal_info->status |= STATUS_WAIT_VOICE_ANS;
             reset_ipl(level_saved);
             signal_info->call_id = global_call_id++;
             global_call_id &= IC_CALL_ID_MASK;
             signal_info->modem_slot     = UNKNOWN_B_CHANNEL;
             signal_info->modem_port     = UNKNOWN_B_CHANNEL;
             signal_info->callstate_func = call_signaling_state;
             signal_info->current_min_guard_time = 0;
             action_execution(signal_info);
         } else if (!IS_DELAY_DIALOUT(signal_info))
             insert_free_channel(signal_info);
         break;

    case EVENT_CALL_DIAL_OUT:
         level_saved = set_ipl(PRIO_FRAMER);
         signal_info->status &= ~(STATUS_IDLE|STATUS_WAIT_VOICE_ANS|STATUS_CONNECTED);
         reset_ipl(level_saved);
         signal_info->current_state =
             (state_table_t*) get_state_table(signal_info->signaling_type, OUTGOING_CALL);
         signal_info->callstate_func = call_signaling_state;
         /*
          * Send back the acknowledge first before entering the action_execution
          */
         send_event_to_host(signal_info, EVENT_CHANNEL_LOCK, NO_CAUSE,
                            NEAT_IPC_MSG_PRI_HI);
         /*
          * Don't expect any call at this point.
          */
         signal_info->rx_abcd_bits_expected = get_rx_onhook_abcd_bits(signal_info);
         signal_info->current_min_guard_time = 0;
         action_execution(signal_info);
         break;

    case EVENT_CALL_IDLE:
         if (IS_DELAY_DIALOUT(signal_info)) {
             level_saved = set_ipl(PRIO_FRAMER);
             signal_info->status &= ~STATUS_DELAY_DIALOUT;
             reset_ipl(level_saved);
             send_event_to_host(signal_info, EVENT_CALL_IDLE, NO_CAUSE,
                                NEAT_IPC_MSG_PRI_HI);
             if (TIMER_IS_RUNNING(&(signal_info->guard_timer)))
                 timer_delete(&(signal_info->guard_timer));
             delete_from_lock_waiting_list(signal_info);
             insert_free_channel(signal_info);
         }
         break;

    default:
         break;
    }
}

static void call_disconnect_state (signal_info_t *signal_info, signal_event_t event)
{
    ushort level_saved;

    if (event == EVENT_BIT_STATE_CHANGE) {
        level_saved = set_ipl(PRIO_FRAMER);
	signal_info->status &= ~STATUS_VERIFY_BIT_STATE;
        reset_ipl(level_saved);
	signal_info->current_rx_abcd_bits = signal_info->new_rx_abcd_bits;

	if (signal_info->new_rx_abcd_bits == get_rx_onhook_abcd_bits(signal_info)) {
            /*
             * Make sure the disconnect bit state holds
             */
            timer_start_one_shot(&signal_info->guard_timer, MAX_STATE_CHANGE_DEBOUNCE_TIME,
				 (callback_func_t)postidle_handler, signal_info, FALSE);
            signal_info->status &= ~STATUS_RBS_RX_BIT_PENDING;
	} else {
            signal_info->status |= STATUS_RBS_RX_BIT_PENDING;
        }        
    }
}

/*
 * Since the state change is handled under task, the delay between the interrupt of
 * state change and the task starts servicing the event will serve as small safety
 * time zone to filter out the error state changes.
 */
static void call_signaling_state (signal_info_t *signal_info, signal_event_t event)
{
    state_table_t     *current_state;
    ushort level_saved;

    switch (event) {
    case EVENT_BIT_STATE_CHANGE:
         current_state = signal_info->current_state;
         level_saved = set_ipl(PRIO_FRAMER);
         signal_info->status &= ~STATUS_VERIFY_BIT_STATE;
         reset_ipl(level_saved);             
         signal_info->current_rx_abcd_bits = signal_info->new_rx_abcd_bits;
         if (current_state->input_state == NULL_BIT_STATE) {
             do_action_idle(signal_info);
         } else if (signal_info->new_rx_abcd_bits & current_state->input_state) {
             /*
              * if the new bit state change is expected
              */
             if (signal_info->current_min_guard_time &&
                 (signal_info->current_min_guard_time < timer_countdown_left(&signal_info->guard_timer))) {
                 /*
                  * The countdown timer doesn't reach the minimum guard time value
                  */
                 do_action_idle(signal_info);
             } else
                 action_execution(signal_info);
         } else if (signal_info->new_rx_abcd_bits & signal_info->rx_abcd_bits_expected) {
             /*
              * the bit state changes back to the expected one. a hit is found.
              * multiple or duplicate interrupts are received
              * however, it may be caused by a hit too.
              * skip this state change!
              */
         } else {
             /*
              * The unexpected state change happens
              * Wait longer to make sure that the invalid state holds.
              */
             if (current_state->recovery_func == do_action_fallback) {
                 signal_info->current_rx_abcd_bits =
                     signal_info->new_rx_abcd_bits;
                 do_action_fallback(signal_info);
             } else {
                 level_saved = set_ipl(PRIO_FRAMER);
                 signal_info->status |= STATUS_VERIFY_BIT_STATE;
                 reset_ipl(level_saved);
                 timer_start_one_shot(&signal_info->hit_timer,
                                      MAX_STATE_CHANGE_DEBOUNCE_TIME,
                                      (callback_func_t)invalid_bit_change_handler, signal_info, NULL);
             }
         }
	 break;

    case EVENT_CALL_IDLE:
	 do_action_idle(signal_info);
         break;

    case EVENT_VOICE_ANS:
         level_saved = set_ipl(PRIO_FRAMER);
         signal_info->status &= ~STATUS_WAIT_VOICE_ANS;
         reset_ipl(level_saved);
         if (IS_CONNECTED(signal_info))
             send_event_to_host(signal_info, EVENT_CHANNEL_CONNECTED, NO_CAUSE,
                                NEAT_IPC_MSG_PRI_HI);
         break;

    default:
         break;
    }
}

static void process_signaling_state_change ()
{
    ushort level_saved;
    signal_info_t *signal_info;
    
    while (signal_change_head) {
        signal_info = delete_state_change_list(signal_change_head);
        if (signal_info) {
            level_saved = set_ipl(PRIO_FRAMER);
            signal_info->status &= ~STATUS_BIT_STATE_CHANGED;
            signal_info->status |= STATUS_VERIFY_BIT_STATE;
            reset_ipl(level_saved);
            if (TIMER_IS_RUNNING(&signal_info->hit_timer)) {
		/*
                 * The state changes again, treat the previous state change
		 * as a hit.
                 */
                timer_delete(&signal_info->hit_timer);
                /*
                 * If the bit changes back to the original one, it's a hit.
                 * Otherwise, validate the new bit change.
                 */
                if (signal_info->new_rx_abcd_bits != signal_info->current_rx_abcd_bits) 
		    timer_start_one_shot(&signal_info->hit_timer, MAX_STATE_CHANGE_DEBOUNCE_TIME,
					 (callback_func_t)valid_bit_change_handler, signal_info, NULL);
                else {
                    level_saved = set_ipl(PRIO_FRAMER);
                    signal_info->status &= ~STATUS_VERIFY_BIT_STATE;
                    reset_ipl(level_saved);
                }
	    } else
                /*
                 * start validating the bit change
                 */
                timer_start_one_shot(&signal_info->hit_timer, MAX_STATE_CHANGE_DEBOUNCE_TIME,
				     (callback_func_t)valid_bit_change_handler, signal_info, NULL);
        }
    }
}

void rbs_process ()
{
    process_signaling_state_change();
    timer_timeout_process(MAX_TIMEOUT_PROCESS_ITERATION);
}

