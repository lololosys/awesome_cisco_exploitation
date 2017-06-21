/* $Id: llclocks.c,v 3.3.56.1 1996/07/31 06:48:42 gcox Exp $
 * $Source: /release/112/cvs/Xsys/llc2/llclocks.c,v $
 *------------------------------------------------------------------
 * llclocks.c -- LLC2 clock code
 * 
 * January 1991, Rick Watson
 * 
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: llclocks.c,v $
 * Revision 3.3.56.1  1996/07/31  06:48:42  gcox
 * CSCdi64685:  Convert LLC2 to interface_generic.h (except llc2.c)
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/21  19:34:02  sberl
 * CSCdi42181:  More than 1000 LLC2 sessions causes excessive tracebacks
 * Remove the check for > 1000 llc sessions from llc2_tick().
 * Add a counter to keep track of how many llc2 sessions there really are.
 * Add this count to banner for "show llc2".
 * Make llc2_starttimer() a real function instead of an inline. It was way
 * too big.
 *
 * Revision 3.2  1995/11/17  17:44:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:35:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/03  21:09:15  hampton
 * Convert LLC2 to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi38203]
 *
 * Revision 2.2  1995/06/16 17:09:09  dkatz
 * CSCdi35956:  Timer declarations should be in a separate file
 * Eliminate obsolete macro references.
 *
 * Revision 2.1  1995/06/07  21:42:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "packet.h"
#include "interface_generic.h"
#include "llc2.h"
#include "lltimers.h"

/*
 * llc2_tick
 */
void llc2_tick (void)
{
    boolean    first;
    leveltype  level;
    llctype    *llc, *l, **prev;
    ulong      llc_count;

    /*
     * The timer queue is kept in order, first timer to expire
     * first.  An entry's position in the queue must be recomputed
     * whenever a timer expires or a timer is stopped.  Stopping
     * a timer will just mean that we will go thru here, not find
     * any timers, and recompute the timer list for this llc.
     * If the first timer is still sleeping, the rest of the timers
     * in the list will be in the future also.
     */
    while ((llc = (llctype *)llctQ.qhead)) {

	if (SLEEPING(llc->next_timer))
	    break;

	/* 
	 * Process expired timers for this llc.
	 */
	if ((llc->timer_flags & ACK_TIMER_M) && 
	    !SLEEPING(llc->timers[ACK_TIMER])) {
	    llc2_stoptimer(llc, ACK_TIMER_M);
	    level = raise_interrupt_level(NETS_DISABLE);
	    llc2_stateswitch(llc, ACK_TIMER_EXP);
	    reset_interrupt_level(level);
	}

	if ((llc->timer_flags & P_TIMER_M) && 
	    !SLEEPING(llc->timers[P_TIMER])) {
	    llc2_stoptimer(llc, P_TIMER_M);
	    level = raise_interrupt_level(NETS_DISABLE);
	    llc2_stateswitch(llc, P_TIMER_EXP);
	    reset_interrupt_level(level);
	}

	/***** WHY IS THIS STUCK HERE AND NOT LATER *****/
	if (llc->txaction != 0) {
	    llc->event = CLOK_TX_ACTION;
	    level = raise_interrupt_level(NETS_DISABLE);
	    llc2_examaction(llc);
	    reset_interrupt_level(level);
	}
	    
	if ((llc->timer_flags & AKDELAY_TIMER_M) && 
	    !SLEEPING(llc->timers[AKDELAY_TIMER])) {
	    llc2_stoptimer(llc, AKDELAY_TIMER_M);
	    level = raise_interrupt_level(NETS_DISABLE);
	    llc2_sendRR(llc);
	    reset_interrupt_level(level);
	}

	if ((llc->timer_flags & REJ_TIMER_M) && 
	    !SLEEPING(llc->timers[REJ_TIMER])) {
	    llc2_stoptimer(llc, REJ_TIMER_M);
	    level = raise_interrupt_level(NETS_DISABLE);
	    llc2_stateswitch(llc, REJ_TIMER_EXP);
	    reset_interrupt_level(level);
	}

	if ((llc->timer_flags & BUSY_TIMER_M) && 
	    !SLEEPING(llc->timers[BUSY_TIMER])) {
	    llc2_stoptimer(llc, BUSY_TIMER_M);
	    level = raise_interrupt_level(NETS_DISABLE);
	    llc2_stateswitch(llc, BUSY_TIMER_EXP);
	    reset_interrupt_level(level);
	}

	if ((llc->timer_flags & LLC1_TIMER_M) && 
	    !SLEEPING(llc->timers[LLC1_TIMER])) {
	    llc2_stoptimer(llc, LLC1_TIMER_M);
	    level = raise_interrupt_level(NETS_DISABLE);
	    llc1_timer(llc);
	    reset_interrupt_level(level);
	}

	if ((llc->timer_flags & ADM_TIMER_M) &&
	    !SLEEPING(llc->timers[ADM_TIMER])) {
	    llc2_stoptimer(llc, ADM_TIMER_M);
	    level = raise_interrupt_level(NETS_DISABLE);
	    adm_timer(llc);
	    reset_interrupt_level(level);
	}

	if ((llc->timer_flags & IDLE_TIMER_M) && 
	    !SLEEPING(llc->timers[IDLE_TIMER])) {
	    llc2_starttimer(llc, IDLE_TIMER, llc->llc2_idle_time);
	    if (!(llc->timer_flags & ACK_TIMER_M)) { /* if not running */
		/* 
		 * if p_timer > 0 then p_flag = 1
		 * rej_timer > 0 => I frame recently received
		 * busy_timer > 0 => RNR frame recently received
		 * both of which would have reset idle_timer 
		 */
		level = raise_interrupt_level(NETS_DISABLE);
		llc2_stateswitch(llc, INIT_PF_CYCLE);
		reset_interrupt_level(level);
	    }
	}
   
	if ((llc->timer_flags & XID_RTRY_TIMER_M) && 
	    !SLEEPING(llc->timers[XID_RTRY_TIMER])) {
	    /*
	     * xid_rtry_time == 0 implies that we don't want
	     * any xids sent so ignore the timer.
	     */
	    if (llc->llc2_xid_rtry_time) {
		llc2_starttimer(llc, XID_RTRY_TIMER, 
				llc->llc2_xid_rtry_time);
		llc->txaction |= XID_R_BIT;
		llc->event = XID_TIME_EXP;
		level = raise_interrupt_level(NETS_DISABLE);
		llc2_examaction(llc);
		reset_interrupt_level(level);
	    } else
		llc2_stoptimer(llc, XID_RTRY_TIMER_M);
	}

	if ((llc->timer_flags & STATUS_TIMER_M) &&
	    !SLEEPING(llc->timers[STATUS_TIMER])) {
	    llc2_stoptimer(llc, STATUS_TIMER_M);
	    level = raise_interrupt_level(NETS_DISABLE);
	    if (llc->status_flags & LLCSF_OPEN)
		if (!llc->blocking && llc->statusproc)
		    (*llc->statusproc)(llc, LC_CONNECT);
	    if ((llc->status_flags & LLCSF_RESET) && llc->statusproc)
		(*llc->statusproc)(llc, LC_RESET);
	    llc->status_flags &= ~(LLCSF_RESET | LLCSF_OPEN);
	    reset_interrupt_level(level);
	}

	/* 
	 * Since timers are stopped by just clearing their bit
	 * in timer_flags, next_timer is now invalid.
	 * Remove llc from the timer queue. It will probably go back 
	 * in somewhere else or not at all.  Since someone may have
	 * done a starttimer above, we can't assume that we're
	 * now first in the queue.
	 */
	level = raise_interrupt_level(NETS_DISABLE);

	if (llc->timer_flags & ON_TIMERQ_M) {
	    unqueue(&llctQ, llc);
	    llc->timer_queue = (llctype *) NULL;
	    llc->timer_flags &= ~ON_TIMERQ_M;
	}

	/*
	 * Now, recompute timers for this llc. If no timers are
	 * running we're finished with this llc.
	 */
	if (llc->timer_flags == 0) {
	    reset_interrupt_level(level);
	    continue;
	}

	/*
	 * Recompute next timer for this llc.
	 */
	first = TRUE;
	if ((llc->timer_flags & XID_RTRY_TIMER_M) && 
	    (first || TIMER_EARLIER(llc->timers[XID_RTRY_TIMER], llc->next_timer))) {
	    first = FALSE;
	    COPY_TIMESTAMP(llc->timers[XID_RTRY_TIMER], llc->next_timer);
	}

	if ((llc->timer_flags & ACK_TIMER_M) && 
	    (first || TIMER_EARLIER(llc->timers[ACK_TIMER], llc->next_timer))) {
	    first = FALSE;
	    COPY_TIMESTAMP(llc->timers[ACK_TIMER], llc->next_timer);
	}

	if ((llc->timer_flags & P_TIMER_M) && 
	    (first || TIMER_EARLIER(llc->timers[P_TIMER], llc->next_timer))) {
	    first = FALSE;
	    COPY_TIMESTAMP(llc->timers[P_TIMER], llc->next_timer);
	}

	if ((llc->timer_flags & IDLE_TIMER_M) && 
	    (first || TIMER_EARLIER(llc->timers[IDLE_TIMER], llc->next_timer))) {
	    first = FALSE;
	    COPY_TIMESTAMP(llc->timers[IDLE_TIMER], llc->next_timer);
	}

	if ((llc->timer_flags & REJ_TIMER_M) && 
	    (first || TIMER_EARLIER(llc->timers[REJ_TIMER], llc->next_timer))) {
	    first = FALSE;
	    COPY_TIMESTAMP(llc->timers[REJ_TIMER], llc->next_timer);
	}

	if ((llc->timer_flags & BUSY_TIMER_M) && 
	    (first || TIMER_EARLIER(llc->timers[BUSY_TIMER], llc->next_timer))) {
	    first = FALSE;
	    COPY_TIMESTAMP(llc->timers[BUSY_TIMER], llc->next_timer);
	}

	if ((llc->timer_flags & AKDELAY_TIMER_M) && 
	    (first || TIMER_EARLIER(llc->timers[AKDELAY_TIMER], llc->next_timer))) {
	    first = FALSE;
	    COPY_TIMESTAMP(llc->timers[AKDELAY_TIMER], llc->next_timer);
	}

	if ((llc->timer_flags & STATUS_TIMER_M) && 
	    (first || TIMER_EARLIER(llc->timers[STATUS_TIMER], llc->next_timer))) {
	    first = FALSE;
	    COPY_TIMESTAMP(llc->timers[STATUS_TIMER], llc->next_timer);
	}

	if ((llc->timer_flags & LLC1_TIMER_M) && 
	    (first || TIMER_EARLIER(llc->timers[LLC1_TIMER], llc->next_timer))) {
	    first = FALSE;
	    COPY_TIMESTAMP(llc->timers[LLC1_TIMER], llc->next_timer);
	}

	if ((llc->timer_flags & ADM_TIMER_M) && 
	    (first || TIMER_EARLIER(llc->timers[ADM_TIMER], llc->next_timer))) {
	    first = FALSE;
	    COPY_TIMESTAMP(llc->timers[ADM_TIMER], llc->next_timer);
	}

	/***** FOR DEBUGGING, SHOULD NEVER HAPPEN *****/
	if (first) {
	    buginf("\nLLC: next_timer not settable");
	    reset_interrupt_level(level);
	    continue;
	}

	/*
	 * Insert into proper place in timer queue
	 */
	prev = (llctype **)&llctQ.qhead;
	l = *prev;
        llc_count = 0;
	for (;;) {
	    if ((l == NULL) || TIMER_EARLIER(llc->next_timer, l->next_timer)) {
		*prev = llc;
		llc->timer_queue = l;
		if (l == NULL)
		    llctQ.qtail = llc;
		llctQ.count++;
		break;
	    }

	    prev = &l->timer_queue;
	    l = *prev;
            ++llc_count;
            /* defensive check to prevent router hang */
	    /* Since this seems to be left over for protection from
	     * and old crash that no longer seems to ever occur, I
	     * think it is safe to remove the check. If we ever start
	     * to see hang problems in the llc timer process, we
	     * should probably turn this code on again. 
	     * [sberl 11/2/95]
	     *
             * if (llc_count > llc_in_hash_count) {
             *    errmsg(&msgsym(TOO_MANY_LLC, LLC));
             *    break;
             * }
	     */
	}
	llc->timer_flags |= ON_TIMERQ_M;

	reset_interrupt_level(level);
    }
    /* 
     * Set next wakeup.  If any llcs on the timer queue, this will
     * be the next_timer of the first entry on the queue since it
     * is sorted by time.  Otherwise, just wake up in a little
     * while.
     */
    if (llctQ.qhead)
	COPY_TIMESTAMP(((llctype *)(llctQ.qhead))->next_timer, llc2_nexttime);
    else
	TIMER_START(llc2_nexttime, ONESEC*10);
}

/*
 * llc2_starttimer
 */
void llc2_starttimer(llctype *llc, int offset, ulong delta)
{
    int mask = 1 << offset;
    sys_timestamp time;
    llctype *l, **prev;
    leveltype level;

    TIMER_START(time, delta);
    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * If we are on the queue, remove us. We'll be re-inserted
     * in the proper place.
     */
    if (llc->timer_flags & ON_TIMERQ_M) {
	unqueue(&llctQ, llc);
	llc->timer_queue = NULL;
    }

    /*
     * If no timers are running or if the time we're setting is
     * sooner than the next one to go off, save the time of the 
     * next timer.
     */

    if (((llc->timer_flags & ~ON_TIMERQ_M) == 0) || 
	TIMER_EARLIER(time, llc->next_timer))
	COPY_TIMESTAMP(time, llc->next_timer);

    llc->timer_flags |= (mask | ON_TIMERQ_M);
    COPY_TIMESTAMP(time, llc->timers[offset]);

    /*
     * Find time ordered insertion point and insert into the queue.
     */
    prev = (llctype **)&llctQ.qhead;
    l = *prev;
    for (;;) {
	if ((l == NULL) || TIMER_EARLIER(llc->next_timer, l->next_timer)) {
	    *prev = llc;
	    llc->timer_queue = l;
	    if (l == NULL)		/* if new tail */
		llctQ.qtail = llc;
	    llctQ.count++;
	    break;
	}

	prev = &l->timer_queue;
	l = *prev;
    }

    /* set next wakeup */
    COPY_TIMESTAMP(((llctype *)(llctQ.qhead))->next_timer, llc2_nexttime);

    reset_interrupt_level(level);
}

