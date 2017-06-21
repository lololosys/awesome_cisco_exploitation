/* $Id: timer.c,v 1.5 1996/10/10 23:45:26 jchlin Exp $
 * $Source: /cscka/t1_neat/src/common/timer.c,v $
 *------------------------------------------------------------------
 * timer.c -- NEAT Processor's Timer Management routines.
 *
 * April, 1996 - Jonathan Chiung-Hsi Lin
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: timer.c,v $
 * Revision 1.5  1996/10/10 23:45:26  jchlin
 * o       Set the framer with default value of SF and AMI during the
 *         initialization of framer.
 * o       Enable the robbed bit interrupt mask in framer chip.
 * o       Change the timer timeout process so that it can process
 *         the timeout timer in the iteration of certain number
 *         before it releases CPU control.
 *
 * Revision 1.4  1996/09/09  05:29:49  jchlin
 * o       The timer_countdown_left() doesn't count right.
 *
 * Revision 1.3  1996/07/13  05:44:09  towu
 * o Raised interupt level to PRIO_FDL.
 * o Added timer_countdown_left routine.
 * o Code clean up.
 *
 * Revision 1.2  1996/05/09  20:28:49  jchlin
 * The initial commit for timer management utility.
 * This timer utility is designed only for one shot
 * timer. The periodical timer can be simulated by
 * using the one shot timer.
 *
 * Revision 1.1  1996/04/05  00:34:35  jchlin
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

static timer_t *timer_active_head=NULL;
static timer_t *timer_timeout_head=NULL;

static void timer_timeout (timer_t *timer)
{
    if (timer) {
        timer->state = TIMER_TIMEOUT;
        if (timer_timeout_head == NULL) {
            timer_timeout_head = timer;
        }
        else 
            dlink_node_insert_prev((dlink_node_t*) timer_timeout_head, 
                                   (dlink_node_t*) timer);
    }
}

static void timer_add (timer_t *timer)
{
    ushort level_saved;

    level_saved = set_ipl(PRIO_FDL);

    if (!timer->countdown_val) {
        /*
         * Timeout immediately
         */
        timer_timeout(timer);
        reset_ipl(level_saved);
        return;
    }


    if (timer_active_head == NULL) {
        timer_active_head = timer;
    }
    else {
        timer_t *curr_timer;
        boolean found;

        found = FALSE;
        curr_timer = timer_active_head;
        do {
            if (curr_timer->countdown_val) {
                if (curr_timer->countdown_val < timer->countdown_val) {
                    timer->countdown_val -= curr_timer->countdown_val;
                }
                else if (curr_timer->countdown_val == timer->countdown_val) {
                    /*
                     * an entry is found
                     */
                    timer->countdown_val = 0;
                    dlink_node_insert_next((dlink_node_t*) curr_timer, 
                                           (dlink_node_t*) timer);
                    found = TRUE;
                    break;
                }
                else {
                    /*
                     * an entry is found
                     */
                    curr_timer->countdown_val -= timer->countdown_val;
                    dlink_node_insert_prev((dlink_node_t*) curr_timer,
                                           (dlink_node_t*) timer);
                    if (curr_timer == timer_active_head)
                        timer_active_head = timer;
                    found        = TRUE;
                    break;
                }
            }
            curr_timer = curr_timer->next;
        } while (curr_timer != timer_active_head);

        if (!found) {
            /*
             * Add to the end of list
             */
            dlink_node_insert_next((dlink_node_t*) timer_active_head->prev, 
                                   (dlink_node_t*) timer);
        }
    }

    timer->state = TIMER_RUNNING;
    reset_ipl(level_saved);
}

timer_t *timer_delete (timer_t *timer)
{
    ushort level_saved;

    if (timer == NULL)
        return NULL;

    level_saved = set_ipl(PRIO_FDL);

    if (timer->state == TIMER_RUNNING) {
        timer_t *next_timer;

        next_timer = timer->next;
        if (next_timer != timer_active_head)
            next_timer->countdown_val += timer->countdown_val;
        
        dlink_node_delete((dlink_node_t**) &timer_active_head, 
                          (dlink_node_t*) timer);
    } else if (timer->state == TIMER_TIMEOUT)
        dlink_node_delete((dlink_node_t**) &timer_timeout_head,
                          (dlink_node_t*) timer);
        
    timer->state = TIMER_IDLE;
    reset_ipl(level_saved);

    return timer;
}

static timer_t *timer_list_delete (timer_t *first_timer)
{
    timer_t *timer, *last_timer;
    ushort level_saved;

    level_saved = set_ipl(PRIO_FDL);

    first_timer->state = TIMER_TIMEOUT;
    last_timer = first_timer;
    for(timer=first_timer->next; 
        (timer != timer_active_head) && (!timer->countdown_val);
        timer=timer->next) {
        last_timer = timer;
        last_timer->state = TIMER_TIMEOUT;
    }
    dlink_list_delete((dlink_node_t**) &timer_active_head,
                      (dlink_node_t*) first_timer,
                      (dlink_node_t*) last_timer);
    
    reset_ipl(level_saved);
    return first_timer;
}

void timer_start_one_shot (timer_t *timer, ulong countdown_val, 
                         callback_func_t callback_func,
                         void *callback_val1, void *callback_val2)
{
    if (timer) {
        if (!TIMER_IS_IDLE(timer))
            timer_delete(timer);
	timer->countdown_val = countdown_val;
        timer->callback_func = callback_func;
        timer->callback_val1 = callback_val1;
        timer->callback_val2 = callback_val2;
        timer_add(timer);
    }
}

void timer_chain_one_shot (timer_t *prev_timer, timer_t *timer,
                           ulong countdown_val,
                           callback_func_t callback_func,
                           void *callback_val1, void *callback_val2)
{
    if (timer) {
        if (timer->state != TIMER_IDLE)
            return;     /* wrong timer state */

	if (!prev_timer)   /* first timer */
            timer->countdown_val = countdown_val;
        else {
            if (prev_timer->state == TIMER_IDLE)
                return;     /* wrong timer state */
            timer->countdown_val = 0;            
        }
        timer->callback_func = callback_func;
        timer->callback_val1 = callback_val1;
        timer->callback_val2 = callback_val2;
        timer->state         = TIMER_RUNNING;
        dlink_node_insert_next((dlink_node_t*) prev_timer, 
                               (dlink_node_t*) timer);        
    }
}

void timer_start_one_shot_chain (timer_t *timer)
{
    timer_add(timer);
}

ulong timer_countdown_left (timer_t *timer)
{
    ushort level_saved;
    ulong  time_left;

    if (timer == NULL)
        return 0;

    time_left = 0;
    level_saved = set_ipl(PRIO_FDL);

    if (timer->state == TIMER_RUNNING) {
        for (;;) {
            if (timer->countdown_val)
                time_left += timer->countdown_val;
            if (timer == timer_active_head)
                break;
            timer = timer->prev;
        }
    }

    reset_ipl(level_saved);

    return time_left;
}

void timer_init (timer_t *timer)
{
    if (timer) {
        dlink_node_init((dlink_node_t*)timer);
        timer->callback_func = NULL;
        timer->callback_val1 = NULL;
	timer->callback_val2 = NULL;
	timer->state         = TIMER_IDLE;
    }
}

void timer_ticks (void) 
{
    ushort level_saved;
    /*
     * Running in the interrupt context.
     * No need to disable interrupt since it's already done.
     */

    level_saved = set_ipl(PRIO_FDL);
    if (timer_active_head) {
        timer_active_head->countdown_val--;
	if (timer_active_head && !timer_active_head->countdown_val)
	    timer_timeout(timer_list_delete(timer_active_head));
    }
    reset_ipl(level_saved);
}

void timer_timeout_process (ushort max_iteration)
{
    timer_t *timer;
    /*
     * Running in the process level.
     */
    if (!timer_timeout_head)
	return; /* no timeout event */

    do {
	timer = timer_timeout_head;
        timer->state = TIMER_IDLE;
	dlink_node_delete((dlink_node_t**) &timer_timeout_head,
			  (dlink_node_t*) timer_timeout_head);
	/*
	 * Timer is idle again so that it's ready to be used again
	 * in the callback_func.
	 */
	if (timer->callback_func)
	    (*timer->callback_func)(timer,
				    timer->callback_val1,
				    timer->callback_val2);
    } while (timer_timeout_head && --max_iteration);
}
