/* $Id: timer.h,v 1.5 1996/10/25 22:26:04 jchlin Exp $
 * $Source: /cscka/t1_neat/src/common/timer.h,v $
 *------------------------------------------------------------------
 * timer.h -- NEAT processor's Timer Management definitions.
 *
 * April, 1996 - Jonathan Chiung-Hsi Lin
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: timer.h,v $
 * Revision 1.5  1996/10/25 22:26:04  jchlin
 * o       Make objects dependent on dependancies.
 * o       Make timer_timeout_process() sharable between t1 and e1.
 *
 * Revision 1.4  1996/10/10  23:45:27  jchlin
 * o       Set the framer with default value of SF and AMI during the
 *         initialization of framer.
 * o       Enable the robbed bit interrupt mask in framer chip.
 * o       Change the timer timeout process so that it can process
 *         the timeout timer in the iteration of certain number
 *         before it releases CPU control.
 *
 * Revision 1.3  1996/07/13  05:44:51  towu
 * o Added timer_countdown_left routine.
 *
 * Revision 1.2  1996/05/09  20:25:23  jchlin
 * The initial commit for timer management utility.
 * This timer utility is designed only for one shot
 * timer. The periodical timer can be simulated by
 * using the one shot timer.
 *
 * Revision 1.1  1996/04/05  00:34:34  jchlin
 * Placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __TIMER_H__
#define __TIMER_H__

#define SIMULATION 0
#define MAX_TIMEOUT_PROCESS_ITERATION 10
#define TIMER_IS_IDLE(timer) ((timer)->state == TIMER_IDLE)
#define TIMER_IS_RUNNING(timer) ((timer)->state != TIMER_IDLE)

typedef struct timer_t_ timer_t;

typedef void (*callback_func_t)(timer_t*, void*, void*);

typedef enum {
    TIMER_RUNNING,
    TIMER_TIMEOUT,
    TIMER_IDLE
} timer_state_t;

struct timer_t_ {
    struct timer_t_ *next;
    struct timer_t_ *prev;
    ulong           countdown_val;
    callback_func_t callback_func;
    void           *callback_val1;
    void           *callback_val2;
    timer_state_t   state;
};

extern void timer_init(timer_t *timer);
extern timer_t *timer_delete(timer_t *timer);
extern void timer_start_one_shot(timer_t *timer, ulong countdown_val, 
                         	 callback_func_t callback_func,
                         	 void *callback_val1, void *callback_val2);
extern void timer_chain_one_shot(timer_t *prev_timer, timer_t *timer,
                           	 ulong countdown_val,
                           	 callback_func_t callback_func,
                           	 void *callback_val1, void *callback_val2);
extern void timer_start_one_shot_chain(timer_t *timer);
extern void timer_ticks(void);
extern void timer_timeout_process(ushort max_iteration);
extern ulong timer_countdown_left (timer_t *timer);

#endif /* __TIMER_H__ */
