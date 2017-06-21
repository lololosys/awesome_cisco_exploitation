/* $Id: process.c,v 3.14.12.14 1996/08/28 13:04:03 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/process.c,v $
 *------------------------------------------------------------------
 * Scheduler primitives for process manipulation.
 *
 * February 1995, David Hampton
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: process.c,v $
 * Revision 3.14.12.14  1996/08/28  13:04:03  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.14.12.13  1996/08/08  14:46:03  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Add a call to determine the depth of a watched queue.
 *
 * Revision 3.14.12.12  1996/08/03  23:34:53  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.14.12.11  1996/08/03  16:54:54  ahh
 * CSCdi64554:  process_lock_semaphore semaphore can start a timer with -1
 * -1 is a magic number meaning "no timeout" -- don't start a timer anyway.
 * Branch: California_branch
 *
 * Revision 3.14.12.10  1996/08/03  08:25:32  hampton
 * Tweak thrashing check for watched queues.  [CSCdi63793]
 * Branch: California_branch
 *
 * Revision 3.14.12.9  1996/06/27  16:36:21  hampton
 * Add new scheduler accessor functions.  [CSCdi61518]
 * Branch: California_branch
 *
 * Revision 3.14.12.8  1996/05/28  05:51:25  jjohnson
 * CSCdi58781:  modular-ukernel image does not build
 * it does now
 *
 * Revision 3.14.12.7  1996/05/15  23:38:00  rcraig
 * Junking this bug.  David Hampton already fixed the problem.
 *
 * CSCdi55958:  process_kill may schedule a dead process on the ready queue
 * Branch: California_branch
 *
 * Revision 3.14.12.6  1996/05/15  17:15:48  rcraig
 * CSCdi55958:  process_kill may schedule a dead process on the ready queue
 * Branch: California_branch
 *
 * Revision 3.14.12.5  1996/05/08  00:06:47  hampton
 * Add better semaphore errors and better code to wait for a semaphore.
 * The routine process_wait_for_event_timed() shouldn't post a TIMER_EVENT
 * when the temporary timer expires.  [CSCdi56999]
 * Branch: California_branch
 *
 * Revision 3.14.12.4  1996/05/07  08:40:35  hampton
 * Put better protection in process_wait_on_system_init() and
 * process_wait_on_system_config().  [CSCdi54394]
 * Branch: California_branch
 *
 * Revision 3.14.12.3  1996/04/03  03:10:59  mbeesley
 * CSCdi53386:  11.2 C7100 modular-ukernel image wont compile
 * Branch: California_branch
 *
 * Revision 3.14.12.2  1996/03/27  21:52:09  hampton
 * Try to catch and report thrashing processes.  A few other minor
 * scheduler cleanups and enhancements.  [CSCdi50130]
 * Branch: California_branch
 *
 * Revision 3.14.12.1  1996/03/18  21:30:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.11.2.4  1996/03/16  07:21:04  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.11.2.3  1996/03/09  05:10:08  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.11.2.2  1996/03/07  10:19:23  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.11.2.1  1996/02/20  16:41:10  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.14  1996/02/16  21:16:24  hampton
 * Need routine to check for saved scheduler events.  [CSCdi49223]
 *
 * Revision 3.13  1996/02/09  05:16:58  hampton
 * CSCdi48519:  process_pop_event_list() modifies low memory
 *
 * Revision 3.12  1996/01/25  23:52:32  hampton
 * Add support to the scheduler for nesting wakeup event lists.  This
 * should make it easier to convert portion of the code.  Also allow for
 * the creation of a detached list of watched events.  [CSCdi46566]
 *
 * Revision 3.11  1996/01/22  06:56:20  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.10  1996/01/19  00:25:42  hampton
 * Implement code review comments.  [CSCdi46938]
 *
 * Revision 3.9  1996/01/17  23:26:48  hampton
 * The process_lock_semaphore() routine needs to do the right thing when
 * called from and interrupt driver with a timeout.  [CSCdi47057]
 *
 * Revision 3.8  1996/01/16  01:16:14  hampton
 * Need support for requeueing packets on a watched queue.  [CSCdi46938]
 *
 * Revision 3.7  1995/12/22  21:58:43  hampton
 * Pass pointers to timestamps instead of passing the complete timestamp
 * structure.  [CSCdi45966]
 *
 * Revision 3.6  1995/12/07  22:31:09  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.5  1995/11/29  01:16:22  hampton
 * Previous fix for a timing window didn't work right.  Nail the problem
 * this time.  [CSCdi44034]
 *
 * Revision 3.4  1995/11/18  06:06:19  hampton
 * Don't let the entire router be dragged down by a single process with a
 * busted timer.  [CSCdi44073]
 *
 * Revision 3.3  1995/11/17  18:50:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:18:33  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:47:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.21  1995/10/26  05:56:36  hampton
 * Plug a small timing window in the expiration of a managed timer being
 * watched by the scheduler.  [CSCdi42124]
 *
 * Revision 2.20  1995/10/04  22:46:25  hampton
 * The process_lock_semphore may wait too long before returning.
 * [CSCdi41552]
 *
 * Revision 2.19  1995/09/28  07:34:00  hampton
 * Fix small timing window when an old style processes call the
 * process_wait_for_event routine. [CSCdi41191]
 *
 * Revision 2.18  1995/09/23 00:18:11  hampton
 * Allow a semaphore to be unlocked from an interrupt handler.
 * [CSCdi40930]
 *
 * Revision 2.17  1995/09/19 18:38:47  smackie
 * Allow chunks to be created for a specified memory pool or with a
 * declared base alignment by allowing the data to be detached from
 * the chunk header and freelist. (CSCdi40637)
 *
 * Revision 2.16  1995/09/18  07:27:31  smackie
 * Prune buffer code into discrete and manageable sections to restore some
 * sanity and focus to buffers.c (CSCdi40529).
 *
 *   o buffers.c - buffer management code
 *   o element.c - queue element code
 *   o queue.c   - singly-linked queue support
 *
 * Revision 2.15  1995/09/05  16:38:00  hampton
 * Fix autoinstall over frame relay.  [CSCdi38852]
 *
 * 1) Udp should only add one-shot queue watches inside of read_ipsocket.
 * 2) Allow a existing one-shot watch to be upgraded to a recurring watch,
 *    but prevent the downgrade of an existing recurring watch to a
 *    one-shot watch.
 * 3) Add some conditionally compiled debugging to the scheduler.
 *
 * Revision 2.14  1995/08/07 22:00:12  hampton
 * Wrong amount of memory allocated for watched semaphores. [CSCdi38345]
 *
 * Revision 2.13  1995/08/03 07:42:42  hampton
 * EIGRP stops processing input.  Convert the IP socket input queue into a
 * watched_queue.  This allows the scheduler to see if there is input
 * waiting when the process dismisses.  [CSCdi38156]
 *
 * Revision 2.12  1995/07/27 03:33:38  hampton
 * Clean up next_event pointer when an event is removed.  [CSCdi37740]
 *
 * Revision 2.11  1995/07/24 09:23:59  hampton
 * Requested scheduler additions.  [CSCdi37542]
 *
 * Revision 2.10  1995/07/22 07:06:07  hampton
 * Check each event once per scheduler invocation, not once per call to
 * process_get_event.  This prevents a high input arrival rate from
 * causing a process to hog the processor.  [CSCdi37126]
 *
 * Revision 2.9  1995/07/18 10:20:46  hampton
 * Fix a timing window where a process that has received a new event may
 * still be put onto the idle queue.  [CSCdi36795]
 *
 * Revision 2.8  1995/07/18 06:48:22  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.7  1995/07/07 05:26:45  hampton
 * Hide the definitions of watched data structures.  These data structures
 * should never be referenced directly.  [CSCdi36799]
 *
 * Revision 2.6  1995/06/23 18:48:25  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.5  1995/06/19  06:48:51  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.4  1995/06/18  05:31:44  hampton
 * Close a small timing window in the scheduler's calls to the
 * mgd_timer_link and mgd_timer_delink routines.  [CSCdi35906]
 *
 * Revision 2.3  1995/06/09 15:07:43  hampton
 * Only change stdio when a new terminal is being assigned to the
 * currently executing process.  [CSCdi35585]
 *
 * Revision 2.2  1995/06/09 13:12:12  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:57:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include "sys_registry.h"
#include "sched_private.h"
#include "stacks.h"
#include "logger.h"
#include "ttysrv.h"
#include "exception.h"
#include "interface_private.h"
#include "../if/network.h"
#include "profile.h"
#include "async.h"
#include "config.h"
#include "mgd_timers.h"
#include "chunk.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */

#define STATIC static
#define INLINE inline

/*
 * Local storage
 */
static chunk_type *message_chunks;
watched_boolean *watched_system_init;   /* watched version of systeminit_flag */
watched_boolean *watched_system_config; /* watched version of system_configured */
watched_boolean *critical_background_flag;


/*********************************************************************
 *
 *			MISCELLANEOUS ROUTINES
 *
 *********************************************************************/

char *event_type2text (ulong type)
{
    switch (type) {
      case TIMER_EVENT:		return("timer");
      case QUEUE_EVENT:		return("queue");
      case BOOLEAN_EVENT:	return("boolean");
      case SEMAPHORE_EVENT:	return("semaphore");
      case MESSAGE_EVENT:	return("signal");
      case IPC_EVENT:		return("ipc");
      case DIRECT_EVENT:	return("direct");
    }
    return("unknown");
};

static char *event_type2name (ulong type)
{
    switch (type) {
      case TIMER_EVENT:		return("Watched Timer");
      case QUEUE_EVENT:		return("Watched Queue");
      case BOOLEAN_EVENT:	return("Watched Boolean");
      case SEMAPHORE_EVENT:	return("Watched Semaphore");
      case MESSAGE_EVENT:	return("Watched Signal");
    }
    return("Unknown Watch");
};

/*
 * process_run_add
 *
 * Given a process, add it to the correct scheduler run queue based
 * upon its priority.
 */
void process_run_add (sprocess *p)
{
    list_header *new_list;

    switch (p->priority) {
      case PRIO_CRITICAL:	new_list = &procq_ready_c;	break;
      case PRIO_HIGH:		new_list = &procq_ready_h;	break;
      case PRIO_NORMAL:		new_list = &procq_ready_m;	break;
      case PRIO_LOW:		new_list = &procq_ready_l;	break;
      default:/*need something*/new_list = &procq_ready_l;	break;
    }
    list_move(new_list, &p->sched_list);
    p->on_old_queue = FALSE;
}

/*
 * process_pid2ptr
 *
 * Translate a process ID to a fork pointer
 */
/* inline */ sprocess *process_pid2ptr (pid_t pid)
{
    sprocess_list *array;
    pid_t local;

    if ((pid == THIS_PROCESS) || (forkx && (pid == forkx->pid)))
	return(forkx);
    if (pid == 0)
	return(NULL);
    local = pid - 1;
    for (array = sarrayQ.qhead;
	 array && (local >= MAX_PROC_PER_ARRAY);
	 array = array->sl_next, local -= MAX_PROC_PER_ARRAY)
	;
    if (!array || !array->sl_process[local])
      return(NULL);
    return(array->sl_process[local]);
}

/*
 * process_exists
 *
 * Given a pid, check to see if there is a process.
 */
boolean process_exists (pid_t pid)
{
    sprocess *proc;

    proc = process_pid2ptr(pid);
    if (proc)
        return(TRUE);
    return(FALSE);
}

/*
 * process_is_ok
 *
 * Given a pid, check to see if there is a process, and that it is in
 * a runnable state.
 */
boolean process_is_ok (int pid)
{
    sprocess *proc;

    proc = process_pid2ptr(pid);
    if (proc == NULL) 
	return(FALSE);
    if (proc->killed || proc->crashed || proc->corrupt)
	return(FALSE);
    return(TRUE);
}

/*
 * process_set_bitfield_minor
 * process_set_boolean_minor
 * process_set_semaphore_minor
 * process_set_queue_minor
 *
 * Change the minor number of an existing watched item.  These routines
 * can be used if the appropriate minor number isn't known when the
 * item is created.  (I.E. A common subroutine creates the watched
 * item.)
 * Use these AFTER creating the watched event but BEFORE watching it.
 */

STATIC INLINE void
set_wi_minor_inline (watched_item *wi, ulong minor)
{
    wi->minor_type = minor;
}

void process_set_bitfield_minor (watched_bitfield *event, ulong minor)
{
    set_wi_minor_inline(&event->wi, minor);
}

void process_set_boolean_minor (watched_boolean *event, ulong minor)
{
    set_wi_minor_inline(&event->wi, minor);
}

void process_set_semaphore_minor (watched_semaphore *event, ulong minor)
{
    set_wi_minor_inline(&event->wi, minor);
}

void process_set_queue_minor (watched_queue *event, ulong minor)
{
    set_wi_minor_inline(&event->wi, minor);
}


/*
 * process_set_bitfield_first_only
 * process_set_boolean_first_only
 * process_set_semaphore_first_only
 * process_set_queue_first_only
 *
 * Change the first_only number of an existing watched item.  These routines
 * can be used if the fdefault first_only value (off) isn't appropriate for
 * the item created.  Use these AFTER creating the watched event but BEFORE
 * watching it.
 */

STATIC INLINE void
set_wi_first_only_inline (watched_item *wi, boolean first_only)
{
    wi->wake_first_only = first_only;
}

void process_set_bitfield_first_only (watched_bitfield *event, boolean first_only)
{
    set_wi_first_only_inline(&event->wi, first_only);
}

void process_set_boolean_first_only (watched_boolean *event, boolean first_only)
{
    set_wi_first_only_inline(&event->wi, first_only);
}

void process_set_semaphore_first_only (watched_semaphore *event, boolean first_only)
{
    set_wi_first_only_inline(&event->wi, first_only);
}

void process_set_queue_first_only (watched_queue *event, boolean first_only)
{
    set_wi_first_only_inline(&event->wi, first_only);
}


/*********************************************************************
 *
 *		   PROCESSOR RELINQUISHING ROUTINES
 *			   SIMPLE SUSPENDS
 *
 *********************************************************************/

/*
 * process_suspend_internal 
 *
 * The shared code fragment used by all routines that are suspending a process
 * that will execute again on the next scheduler pass.  This can be either
 * from an explicit request to suspend, or from a request to sleep when there
 * are outstanding events to process.
 */
STATIC INLINE void process_suspend_internal (void)
{
    if (forkx->prefers_new) {
	forkx->state = SCHEDSTATE_READY;
	if (forkx->on_old_queue)
	    process_run_add(forkx);
    } else {
	forkx->state = SCHEDSTATE_SUSPEND;
	if (!forkx->on_old_queue)
	    old_run_add(forkx);
    }
    suspend();				/* switch to scheduler context */
}

/*
 * process_suspend
 *
 * The process is voluntarily relinquishing the processor.  Make sure to
 * shuffle it back to the set of queues that it understands.  This is
 * primarily intended for the case of a new style process that tripped
 * over an edisms and got stuffed onto an old style queue.  All the other
 * routines for relinquishing the processor involve requeuing the
 * process, so that is the only case where this is really necessary.
 */
void process_suspend (void)
{
    SAVE_CALLER();

    /*
     * Check validity of call.
     */
    if (!process_ok_to_reschedule("suspend")) {
	crashdump(10);
    }

    /*
     * Remember where we came from.
     */
    forkx->caller_pc = caller();   	/* record PC of caller */
    forkx->callee_pc = current_pc();	/* record current address */
    forkx->ngiveups++;
    forkx->thrash_count = 0;

    process_suspend_internal();
}

/*
 * process_check_suspend_inline
 * Suspend if a high priority process must run.
 * Return TRUE if suspended, FALSE otherwise
 */

STATIC INLINE boolean process_would_suspend_inline (void)
{
    boolean yes = FALSE;

    if (!LIST_EMPTY(&procq_ready_c))
	yes = TRUE;
    else if (!LIST_EMPTY(&procq_ready_h))
	yes = TRUE;
    else if (process_quantum_expired)
	yes = TRUE;
    
    return(yes);
}

/*
 * process_would_suspend
 * Return TRUE if a high or critical process is ready to run or the
 * current processes' quantum has expired, FALSE otherwise.
 */
boolean process_would_suspend (void)
{
    return(process_would_suspend_inline());
}

STATIC INLINE boolean process_may_suspend_inline (void)
{
    if (process_would_suspend_inline()) {
	process_suspend();
	return(TRUE);
    } else
	return(FALSE);
}

/*
 * process_may_suspend
 * Suspend if a high priority process must run.
 * Return TRUE if suspended, FALSE otherwise
 */
boolean process_may_suspend (void)
{
    return(process_may_suspend_inline());
}

/*********************************************************************
 *
 *		   PROCESSOR RELINQUISHING ROUTINES
 *			 TIMER ONLY SUSPENDS
 *
 *********************************************************************/

/*
 * process_setup_sleep_inline
 *
 * Common routine for all variants of the sleep call.
 */
STATIC INLINE boolean process_setup_sleep_inline (char *text)
{
    SAVE_CALLER();

    /*
     * Check validity of call.
     */
    if (!process_ok_to_reschedule(text))
	return(FALSE);

    /*
     * Remember where we came from.
     */
    forkx->caller_pc = caller();   	/* record PC of caller */
    forkx->callee_pc = current_pc();	/* record current address */
    forkx->thrash_count = 0;
    return(TRUE);
}

/*
 * process_do_sleep_inline
 *
 * Common routine for all variants of the sleep call.
 */
STATIC INLINE void process_do_sleep_inline (sched_state indicator)
{
    forkx->state = indicator;
    forkx->on_old_queue = FALSE;
    forkx->wakeup_reasons = TIMER_EVENT;	/* One Shot */
    list_move(&procq_idle, &forkx->sched_list);
    suspend();				/* switch to scheduler context */
}

/*
 * process_sleep_until
 *
 * Put a process to sleep until the system clock reaches an absolute time
 * value.  The process may not be awakened for any other reason.
 */
void process_sleep_until (sys_timestamp *wakeup_time)
{
    if (!process_setup_sleep_inline("sleep until"))
	return;

    /*
     * Diddle the per-process timer.  If the requested wakeup time is
     * zero ensure a minimum sleep time so that it doesn't execute again
     * immediately.  Make sure not to modify *wakeup_timer itself, as this
     * now lives in the calling process instead of being a local copy.
     */
    if (TIMER_RUNNING(*wakeup_time))
	mgd_timer_set_exptime(&forkx->sched_timer_temp, wakeup_time);
    else
	mgd_timer_start(&forkx->sched_timer_temp, 1 + refresh_time);

    /* Nighty night. */
    process_do_sleep_inline(SCHEDSTATE_SLEEP_ABS);
}

/*
 * process_sleep_for
 *
 * Put a process to sleep for the requested number of milliseconds from
 * the current time.  The process may not be awakened for any other reason.
 */
void process_sleep_for (ulong delta_time)
{
    if (!process_setup_sleep_inline("sleep for"))
	return;

    /*
     * Diddle the per-process timer.  Ensure a minimum sleep time so that
     * the process doesn't execute again immediately.
     */
    if (delta_time <= refresh_time)
	delta_time = 1 + refresh_time;
    mgd_timer_start(&forkx->sched_timer_temp, delta_time);

    /* Nighty night. */
    process_do_sleep_inline(SCHEDSTATE_SLEEP_INTERVAL);
}

/*
 * process_sleep_periodic
 *
 * Put a process to sleep for the requested number of milliseconds from its
 * last wakeup time.  The process may not be awakened for any other reason.
 *
 * Use this instead of process_sleep_for if we must activate periodically
 * and we can't afford to have possible long processing delays delay our
 * next activation.
 */
void process_sleep_periodic (ulong deltat)
{
    if (!process_setup_sleep_inline("sleep periodic"))
	return;

    /*
     * Diddle the pre-process timer.  Ensure a minimum sleep time so that
     * the process doesn't execute again immediately.
     */
    if (deltat <= refresh_time)		
	deltat = 1 + refresh_time;
    if (deltat > ELAPSED_TIME(forkx->prev_timer))
	deltat -= ELAPSED_TIME(forkx->prev_timer);
    mgd_timer_start(&forkx->sched_timer_temp, deltat);

    /* Nighty night. */
    process_do_sleep_inline(SCHEDSTATE_SLEEP_PERIODIC);
}

/*
 * process_sleep_on_timer
 *
 * Put a process to sleep until its managed timer expires.  This routine
 * actually links the process master timer into the scheduler timers, so that
 * any changes to one of the processes timer will cascade all the way up into
 * the scheduler and affect its wakeup time.  The process may not be awakened
 * for any other reason.
 *
 * If a process is maintaining more that a very small handful of timers,
 * it should be rewritten to use managed timers and this routine should
 * be the preferred method of relinquishing the processor.
 */
void process_sleep_on_timer (mgd_timer *private_timer)
{
    if (!process_setup_sleep_inline("sleep on timer"))
	return;

    /*
     * Diddle the per-process timer.
     */
    mgd_timer_link(private_timer, &forkx->sched_timer_managed,
		   &forkx->sched_linked_mtimer, TRUE);

    /* Nighty night. */
    process_do_sleep_inline(SCHEDSTATE_SLEEP_TIMER);
}


/*********************************************************************
 *
 *		   PROCESSOR RELINQUISHING ROUTINES
 *		SUSPENDS UNTIL ONE OF MULTIPLE EVENTS
 *
 *********************************************************************/

typedef enum process_ev_result_ {
    PROC_EVR_SUSPEND,
    PROC_EVR_THRASHING,
    PROC_EVR_BROKEN_TIMER,
    PROC_EVR_IDLE
} process_ev_result;

static inline process_ev_result process_check_thrashing (char *string)
{
    if (++forkx->thrash_count >= SCHED_THRASH_THRESHHOLD) {
	if ((forkx->thrash_count % SCHED_THRASH_THRESHHOLD) == 0)
	    (*kernel_errmsg)(&msgsym(THRASHING, SCHED), string);
	return(PROC_EVR_THRASHING);
    }
    return(PROC_EVR_SUSPEND);
}

/*
 * process_events_waiting_p
 *
 * Are there any outstanding events for this process?  Outstanding events
 * are events that have arrived while a process was running, or elements
 * left on a watched queue.  The latter case is common for protocol input
 * processes that always suspend after processing a set number of frames.
 */
static process_ev_result process_events_waiting_p (void)
{
    list_element *element;
    watcher_info *wakeup;
    watched_item *event;
    watched_boolean *bool;
    watched_queue *queue;
    sched_event_set *info;
    sys_timestamp exp_time;
    mgd_timer *expired_timer;
    char buffer[80];
    int current_size, old_size;

    /*
     * Check for events that arrived while the process was executing.
     */
    if (forkx->wakeup_posted) {
	forkx->thrash_count = 0;
	return(PROC_EVR_SUSPEND);
    }

    /*
     * Check to see if the timers have already expired.  If they have, be
     * polite and assume that it expired as the process was going to
     * sleep.  Remember it though, in case the process is really busted.
     */
    info = forkx->current_events;
    if (info->watched_mgd_timer) {
	if (mgd_timer_expired(info->watched_mgd_timer)) {
	    info->watch_timers.wi_occurred = TRUE;
	    exp_time = mgd_timer_exp_time(info->watched_mgd_timer);
	    if (TIMERS_EQUAL(exp_time, forkx->exp_mgdtimer_time)) {
		expired_timer =
		    mgd_timer_first_expired(info->watched_mgd_timer);
		(*kernel_errmsg)(&msgsym(STUCKMTMR, SCHED), expired_timer,
		       exp_time, exp_time);
		return(PROC_EVR_BROKEN_TIMER);
	    }
	    COPY_TIMESTAMP(exp_time, forkx->exp_mgdtimer_time);
	    sprintf(buffer, "managed timer (%#x)", info->watched_mgd_timer);
	    return(process_check_thrashing(buffer));
	}
    }
    if (info->watched_simple_timer) {
	if (TIMER_RUNNING_AND_AWAKE(*info->watched_simple_timer)) {
	    info->watch_timers.wi_occurred = TRUE;
	    exp_time = *info->watched_simple_timer;
	    if (TIMERS_EQUAL(exp_time, forkx->exp_timer_time)) {
		(*kernel_errmsg)(&msgsym(STUCKTMR, SCHED),
		       info->watched_simple_timer, exp_time, exp_time);
		return(PROC_EVR_BROKEN_TIMER);
	    }
	    COPY_TIMESTAMP(exp_time, forkx->exp_timer_time);
	    sprintf(buffer, "simple timer (%#x)", info->watched_simple_timer);
	    return(process_check_thrashing(buffer));
	}
    }

    /*
     * Any messages pending?  Messages may only be sent to the main process.
     */
    if (QUEUEEMPTY(&forkx->process_event_setQ) &&
	!QUEUEEMPTY(&forkx->message_queue)) {
	forkx->watch_messages.wi_occurred = TRUE;
	return(process_check_thrashing("message event"));
    }

    /*
     * Walk the list of notifications looking for events that have
     * already occurred, or watched queues that are non-empty.  If there
     * are any, the process should only suspend.
     */
    FOR_ALL_DATA_IN_LIST(&info->process_events, element, wakeup) {
	if (wakeup->wi_one_shot && wakeup->wi_shot)
	    continue;
	event = wakeup->wi_event;
	switch (event->major_type) {
	  case QUEUE_EVENT:
	    queue = (watched_queue *)event;
	    if (!QUEUEEMPTY(&queue->q)) {
		old_size = queue->old_size;
		current_size = QUEUESIZE(&queue->q);
		queue->old_size = current_size;
		wakeup->wi_occurred = TRUE;
		if (current_size < (old_size - 10)) {
		    forkx->thrash_count = 0;
		    return(PROC_EVR_SUSPEND);
		}
		sprintf(buffer, "queue '%s' (count %d)",
			event->name, current_size);
		return(process_check_thrashing(buffer));
	    }
	    queue->old_size = 0;
	    break;
	  case BOOLEAN_EVENT:
	    bool = (watched_boolean *)event;
	    if (bool->value == TRUE) {
		wakeup->wi_occurred = TRUE;
		sprintf(buffer, "boolean '%s'", event->name);
		return(process_check_thrashing(buffer));
	    }
	    break;
	  default:
	    break;
	}
    }
    forkx->thrash_count = 0;
    return(PROC_EVR_IDLE);
}

/*
 * process_idle_internal
 *
 * The shared code fragment used by all routines that are suspending a process
 * until some event occurs, or until a timer expires.
 *
 * Perform a final check to see if any new events have arrived.  This is
 * performed under interrupt lock to guarantee that the if the process
 * has new events it is not moved onto the idle queue.
 */
STATIC INLINE void process_idle_internal (
    caller_t saved_caller_pc,
    sched_state indicator,
    ulong deltat)
{
    leveltype level;
    sched_event_set *info;

    /*
     * Remember where we came from.
     */
    forkx->caller_pc = caller();   	/* record PC of caller */
    forkx->callee_pc = current_pc();	/* record current address */

    /*
     * Don't let the process go to sleep if some watched event has
     * occurred since the process began executing.  The process might
     * have missed it, and the scheduler must be conservative and err on
     * the side of too many executions.
     */
    level = set_interrupt_level(ALL_DISABLE);
    info = forkx->current_events;
    switch (process_events_waiting_p()) {
      case PROC_EVR_SUSPEND:
	reset_interrupt_level(level);
	process_suspend_internal();
	return;		/* Process has already suspended */

      case PROC_EVR_THRASHING:
      case PROC_EVR_BROKEN_TIMER:
	/*
	 * Something is broken in the process.  Either it has broken
	 * timers, or it does not recognize an event that it has
	 * received.  If the process hasn't provided a delta wakeup, then
	 * wake up after one second.  If the process did provide a delta
	 * wakeup, make sure its no more than one second, so whatever has
	 * the stuck timer will run in a reasonable amount of time.
	 */
	if ((deltat == 0) || (deltat > ONESEC))
	    deltat = ONESEC;
	break;		/* suspend occurs below */

      case PROC_EVR_IDLE:
	/*
	 * Link in any watched timers.
	 */
	if (info->watched_mgd_timer) {
	    mgd_timer_link(info->watched_mgd_timer,
			   &forkx->sched_timer_managed,
			   &forkx->sched_linked_mtimer,
			   TRUE);
	}
	if (info->watched_simple_timer &&
	        TIMER_RUNNING(*info->watched_simple_timer))
	    mgd_timer_set_exptime(&forkx->sched_timer_simple,
				  info->watched_simple_timer);
	break;		/* suspend occurs below */
    }

    /*
     * Link in the delta timer (if applicable).
     */
    if (deltat)
	mgd_timer_start(&forkx->sched_timer_temp, deltat);

    /*
     * Do the queue shuffle.
     */
    forkx->state = indicator;
    forkx->on_old_queue = FALSE;
    list_move(&procq_idle, &forkx->sched_list);
    reset_interrupt_level(level);
    suspend();				/* switch to scheduler context */
}

/*
 * process_wait_for_event
 *
 * Put the process to sleep until some asynchronous event awakens it.
 * One of these events may be a timer that the process is watching.  If
 * there is no timer running, there must be some active effort by another
 * part of the system to awaken this process.
 */
void process_wait_for_event (void)
{
    SAVE_CALLER();

    /*
     * Check validity of call.
     */
    if (!process_ok_to_reschedule("idle"))
	return;

    process_idle_internal(saved_caller_pc, SCHEDSTATE_IDLE, 0);
}

/*
 * process_wait_for_event_timed
 *
 * Put the process to sleep until some asynchronous event awakens it.
 * This routine is identical to the process_wait_for_event() routine,
 * except that a maximum wait time is supplied.  If no events have
 * arrived within the specified time period, this routine will return
 * anyway.  There is no overt indication that the timeout expired.
 * The only clue available is that the process_get_wakeup() routine
 * will return FALSE indication that there are no events to be
 * processed (assuming no event arrives between the two calls).
 */
void process_wait_for_event_timed (ulong deltat)
{
    SAVE_CALLER();

    /*
     * Check validity of call.
     */
    if (!process_ok_to_reschedule("t_idle"))
	return;

    process_idle_internal(saved_caller_pc, SCHEDSTATE_IDLE, deltat);
}

/*
 * process_hang
 *
 * Routine which will hang the process forever.  This is a bastardized
 * copy of process_wait_for_event that can never return.
 */
void process_hang (void)
{
    SAVE_CALLER();

    forkx->caller_pc = caller();   	/* record PC of caller */
    forkx->callee_pc = current_pc();	/* record current address */

    for (;;) {
	forkx->state = SCHEDSTATE_HUNG;
	forkx->on_old_queue = FALSE;
	list_move(&procq_idle, &forkx->sched_list);
	suspend();
    }
}


/*********************************************************************
 *
 *		       PROCESS WAKEUP ROUTINES
 *
 *********************************************************************/

/*
 * process_wakeup_internal
 *
 * Common code for awakening processes.
 */
void process_wakeup_internal (sprocess *proc, ulong major, ulong minor)
{
    leveltype saved_level;
    boolean wakeup;

    if (proc == NULL)
	return;
    if (proc->killed || proc->crashed || proc->corrupt)
	return;
    wakeup = (proc->wakeup_reasons & major);

    /*
     * Some debugging code.  Put a breakpoint here to find out why a
     particular pid is being awakened.
     */
#ifdef SCHED_DEBUG
    if (proc->debug_events & PROC_DEBUG_WAKEUP) {
	(*kernel_buginf)("\nSCHED: Wakeup of process %s, %s %d %s", proc->name,
	       event_type2text(major), minor, wakeup ? "" : "(ignored)");
	if (proc->debug_events & PROC_DEBUG_WAKEUP_DELAY) {
	    sched_debug_events = PROC_DEBUG_EXECUTION;
	}
    }
#endif

    /*
     * Stop the timer.  Make sure that either we're really going to wake up the
     * process, or that is was the timer that fired.  If the timer fired, it must
     * be stopped regardless of whether the process is going to wake up or not,
     * otherwise it screws up the entire scheduler.
     */
    if (wakeup || (major == TIMER_EVENT)) {
	switch (mgd_timer_type(&proc->sched_timer)) {
	  case STT_PROCESS_MANAGED:
	  case STT_PROCESS_SIMPLE:
	  case STT_PROCESS_TEMP:
	    /*
	     * The reference to proc->sched_linked_mtimer must appear
	     * after interrupts are disabled.  If it appears earlier a
	     * timing window is created where the system can attempt to
	     * delink the same timer twice.
	     */
	    saved_level = raise_interrupt_level(ALL_DISABLE);
	    mgd_timer_delink(&proc->sched_linked_mtimer);
	    proc->sched_linked_mtimer = NULL;
	    reset_interrupt_level(saved_level);
	    /*
	     * Individually stop the timers.  Stopping the parent and
	     * having the stop cascade downwards leaves room for a
	     * potential timing window where a child is stopped twice.
	     */
	    mgd_timer_stop(&proc->sched_timer_simple);
	    mgd_timer_stop(&proc->sched_timer_temp);
	    break;
	  case STT_FORCE:
	    break;
	}
    }

    /*
     * Really wakeup?  If so, reset the scheduler to its default set of
     * permissible wakeup reasons.
     */
    if (!wakeup)
	return;
    proc->wakeup_reasons = proc->default_wakeup_reasons;
    proc->wakeup_posted = TRUE;

    /*
     * Wake up an old style process.  If its already on an old style
     * queue, there is nothing to do.
     */
    if (!proc->prefers_new) {
	if (proc->on_old_queue)
	    return;
	old_run_add(proc);
	return;
    }

    /*
     * Wake up a new style process.  If it already been moved to a ready
     * queue, the only thing to do is signal the arrival of a new event.
     * NOTE: If a 'new' style process has tripped across an edisms call,
     * this code fragment will leave it awaiting resolution of the
     * blocking routine.  This is the correct thing to do, as prematurely
     * returning from the edisms may cause unexpected problems.
     */
    if (proc->sched_list.list != &procq_idle)
	return;
    proc->state = SCHEDSTATE_READY;
    process_run_add(proc);
}

/*
 * process_wakeup_w_reason
 *
 * One process is signaling to another process to wake up.  Do not wake
 * up processes that are still running under the old scheduler, as this
 * may screw up process_sleep_for calls or other things.  Also check
 * to unsure that information is not lost.  Multiple direct wakeups
 * are allowable, as long as both the major and minor codes are the
 * same for each wakeup.  If they are not, then information is being
 * lost and the system will complain.
 */
void process_wakeup_w_reason (pid_t pid, ulong major, ulong minor)
{
    sprocess *proc;

    if (pid == 0)
	return;
    proc = process_pid2ptr(pid);
    if (proc == NULL)
	return;
    if (proc->direct_wakeup_major != 0) {
	if ((proc->direct_wakeup_major != major) &&
	    (proc->direct_wakeup_minor != minor)) {
	    (*kernel_errmsg)(&msgsym(LOSTWAKEUP, SCHED), proc->name, major,
			     minor);
	    return;
	}
    }
    proc->direct_wakeup_major = major;
    proc->direct_wakeup_minor = minor;
    process_wakeup_internal(proc, major, minor);
}

/*
 * process_wakeup
 *
 * One process is signaling to another process to wake up.  Do not wake
 * up processes that are still running under the old scheduler, as this
 * may screw up process_sleep_for calls or other things.
 */
void process_wakeup (pid_t pid)
{
    process_wakeup_w_reason(pid, DIRECT_EVENT, 0);
}


/**********************************************************************
 *
 *		      PROCESS WATCHING ROUTINES
 *		       (internal generic code)
 *
 **********************************************************************/

/*
 * process_add_wakeup
 *
 * Add a watch block to an event.  This also links the same watch block onto the
 * list of items being watched by a given process.
 */
static boolean process_add_wakeup (
    watched_item *event,
    ONESHOTTYPE one_shot,
    boolean already_happened)
{
    list_element *element;
    watcher_info *wakeup;

    /*
     * Is this event already queued?  An existing event may not be changed
     * from RECURRING to ONE_SHOT, but all other attributes can be
     * updated.  This is mainly needed by UDP queues, because some need
     * to be watched permanently, while some only need to be watched
     * during the actual read call.  The read_ipsocket routine needs to
     * be able to install a one_shot watch without disturbing and
     * pre-existing recurring watch.
     */
    FOR_ALL_DATA_IN_LIST(&event->wakeup_list, element, wakeup) {
	if (wakeup->wi_process == forkx) {
	    wakeup->wi_occurred = already_happened;
	    wakeup->wi_one_shot = wakeup->wi_one_shot & one_shot;
	    wakeup->wi_shot = one_shot & already_happened;
	    if (already_happened)
		forkx->wakeup_posted = TRUE;
#ifdef SCHED_DEBUG
	    if (forkx->debug_events & PROC_DEBUG_WATCH)
		(*kernel_buginf)("\nSCHED: %s: update watched %s %s(%x), os:%d ah:%d",
		       forkx->name, event_type2text(event->major_type),
		       event->name, event, wakeup->wi_one_shot, wakeup->wi_shot);
#endif
	    return(TRUE);
	}
    }

    wakeup = malloc_named(sizeof(watcher_info), "Event Threads");
    if (wakeup == NULL)
	return(FALSE);

    list_enqueue(&event->wakeup_list, &wakeup->wi_by_event, wakeup);
    list_enqueue(&forkx->current_events->process_events, &wakeup->wi_by_process, wakeup);
    wakeup->wi_process = forkx;
    wakeup->wi_event = event;
    wakeup->wi_occurred = already_happened;
    wakeup->wi_one_shot = one_shot;
    wakeup->wi_shot = one_shot & already_happened;
    if (already_happened)
	forkx->wakeup_posted = TRUE;
#ifdef SCHED_DEBUG
    if (forkx->debug_events & PROC_DEBUG_WATCH)
	(*kernel_buginf)("\nSCHED: %s: install watched %s %s(%x), os:%d ah:%d",
	       forkx->name, event_type2text(event->major_type),
	       event->name, event, one_shot, already_happened);
#endif
    return(TRUE);
}

/*
 * process_remove_wakeup
 *
 * Remove a watch block from an event.  Also removes the watch block from the
 * per-process list, and purges the memory.
 */
static void process_remove_wakeup (watched_item *event)
{
    list_element *element;
    watcher_info *wakeup;
    sched_event_set *event_set;

    /*
     * Find the item in the queue.
     */
    FOR_ALL_DATA_IN_LIST(&event->wakeup_list, element, wakeup) {
	if (wakeup->wi_process != forkx)
	    continue;
	event_set = forkx->current_events;
	if (event_set->next_event == &wakeup->wi_by_process)
	    event_set->next_event = LIST_NEXT_ELEMENT(&wakeup->wi_by_process);
	list_remove(&event->wakeup_list, &wakeup->wi_by_event, wakeup);
	list_remove(&event_set->process_events, &wakeup->wi_by_process, wakeup);
#ifdef SCHED_DEBUG
	if (forkx->debug_events & PROC_DEBUG_WATCH)
	    (*kernel_buginf)("\nSCHED: %s: remove watched %s %s(%x)",
		   forkx->name, event_type2text(event->major_type),
		   event->name, event);
#endif
	free(wakeup);
	return;
    }
}

/*
 * process_watch_common
 *
 * Common code for linking an event into the master list for that class of
 * event, and for adding or removing a watch block for this process.
 */
STATIC INLINE void process_watch_common (
    watched_item *event,
    ENABLESTATE watch,
    ONESHOTTYPE one_shot,
    boolean already_happened)
{
    if (watch == ENABLE) {
	process_add_wakeup(event, one_shot, already_happened);
    } else {
	process_remove_wakeup(event);
    }
}

/*
 * watcher_initialize_common
 */
void watcher_initialize_common (
    list_header *master_list,
    watched_item *event,
    char *name,
    major_event_type major,
    ulong minor)
{
    if (master_list)
	list_enqueue(master_list, &event->by_class, event);
    list_create(&event->wakeup_list, 0, master_list ? name : NULL,
		LIST_FLAGS_INTERRUPT_SAFE);
    list_set_info(&event->wakeup_list, process_list_wakeups);
    event->name = name;
    event->major_type = major;
    event->minor_type = minor;
}

/*
 * watcher_create_common
 *
 * Common code for creating managed events.  The caller is responsible for
 * specifying the size of the event, including the area for maintaining the
 * notification information.  This information is then filled in, and the pointer
 * returned to the caller.  This returned pointer will then be cast to the
 * appropriate type (thanks to void* pointers.)
 */
static void *watcher_create_common (
    list_header *master_list,
    int size,
    char *name,
    major_event_type major,
    ulong minor)
{
    watched_item *event;

    event = malloc_named(size, event_type2name(major));
    if (event == NULL) {
	return(NULL);
    }
    watcher_initialize_common(master_list, event, name, major, minor);
    return(event);
}

/*
 * watcher_delete_common
 *
 * Common code for purging an event from memory.  This routine will unlink the
 * event from its master queue, and throw away any wakeup blocks threaded onto
 * this list.
 */
static boolean watcher_delete_common (watched_item *event, char *class)
{
    watcher_info *wakeup;
    sprocess *process;
    sched_event_set *event_set;

    /*
     * Sanity check the argument.
     */
    if (event == NULL) {
	(*kernel_errmsg)(&msgsym(WATCH, SCHED), "delete", class, event);
	return(FALSE);
    }

    /*
     * Remove from the master list for this class of event.
     */
    list_remove(event->by_class.list, &event->by_class, event);

    /*
     * Now free all wakeup blocks that are attached to this event.  The
     * use of dequeue cleans up the 'wi_by_event' thread, so all that is
     * left is the 'wi_by_process' thread.
     */
    while ((wakeup = list_dequeue(&event->wakeup_list)) != NULL) {
	process = wakeup->wi_process;
	event_set = process->current_events;
	if (event_set->next_event == &wakeup->wi_by_process)
	    event_set->next_event = LIST_NEXT_ELEMENT(&wakeup->wi_by_process);
	list_remove(wakeup->wi_by_process.list, &wakeup->wi_by_process, wakeup);
	free(wakeup);
    }
    list_destroy(&event->wakeup_list);
    return(TRUE);
}

/*
 * process_notify_event
 *
 * Notify all process that are waiting for a given event to occur.
 */
static void process_notify_event (watched_item *event)
{
    list_element *element;
    watcher_info *wakeup;
    sprocess *proc;

    FOR_ALL_DATA_IN_LIST(&event->wakeup_list, element, wakeup) {
	if (wakeup->wi_one_shot) {
	    if (wakeup->wi_shot)
		continue;
	    wakeup->wi_shot = TRUE;
	}
	wakeup->wi_occurred = TRUE;
	proc = wakeup->wi_process;
	if (proc == NULL)	/* Floating event list */
	    continue;
	/*
	 * If this process has already been moved to a run queue, or if
	 * the wakeup is for a saved event, then there is no need to call
	 * the wakeup routine.
	 */
	if (proc->corrupt || proc->crashed || proc->killed) {
	    /* Do nothing. */
	} else if (!proc->wakeup_posted &&
	    (wakeup->wi_by_process.list == &proc->current_events->process_events)) {
	    process_wakeup_internal(proc, event->major_type, event->minor_type);
	}
	if (event->wake_first_only) {
	    /*
	     * Move this event to the end of the wakeup list. Use the
	     * list_move routine so that the move is atomic.
	     */
	    list_move(&event->wakeup_list, element);
	    return;
	}
    }
}


/**********************************************************************
 *
 *		  PROCESS QUEUE MANAGEMENT ROUTINES
 *			  EXTERNAL INTERFACE
 *
 **********************************************************************/

/*
 * process_caller_has_events
 *
 * Check all saved event lists to see if any events have been posted.
 * If so, return TRUE so that the caller knows to pop the event stack
 * and return to his caller.
 */
boolean process_caller_has_events (void)
{
    sched_event_set *event_set;
    list_element *element;
    watcher_info *wakeup;

    /* Shouldn't be here because there is no higher level */
    if (QUEUEEMPTY(&forkx->process_event_setQ))
	return(FALSE);

    /* Check direct */
    if (forkx->direct_wakeup_major)
	return(TRUE);

    /* Run the saved event sets checking each item */
    event_set = forkx->process_event_setQ.qhead;
    for ( ; event_set; event_set = event_set->next) {
	FOR_ALL_ELEMENTS_IN_LIST(&event_set->process_events, element) {
	    wakeup = LIST_GET_DATA(element);
	    if (wakeup->wi_occurred)
		return(TRUE);
	}
    }
    return(FALSE);
}

/*
 *
 */
sched_event_set *process_malloc_event_set (void)
{
    sched_event_set *info;

    info = malloc_named(sizeof(sched_event_set), "Process Events");
    if (!info)
	return(NULL);
    info->magic = SCHED_EVENT_MAGIC;
    return(info);
}

/*
 * process_push_event_list
 *
 * Stash the current event list away in a queue, and create a new event
 * list.  This routine is called by libraries of code that need to
 * specify an exact event list without worrying about preexisting events.
 */
boolean process_push_event_list (sched_event_set *pre_existing)
{
    leveltype level;
    list_header *new_list;
    list_element *element;
    sched_event_set *info;
    watcher_info *wakeup;

    /*
     * Were we handed a pre-existing set of scheduler events?  If so,
     * massage the wakeups so that they point to this process.
     */
    if (pre_existing) {
	info = pre_existing;
        FOR_ALL_ELEMENTS_IN_LIST(&info->process_events, element) {
	    wakeup = LIST_GET_DATA(element);
	    wakeup->wi_process = forkx;
	}
    } else {
        /*
	 * Get and setup a new data block for a new event set.
	 */
	info = process_malloc_event_set();
	if (!info)
	    return(FALSE);
	new_list = list_create(&info->process_events, 0, forkx->name,
			       LIST_FLAGS_INTERRUPT_SAFE);
	if (new_list == NULL) {
	    /* Oops.  Something died horribly in list_create. */
	    free(info);
	    return(FALSE);
	}
	new_list->info     = process_list_wakeups;
	list_enqueue(&info->process_events, &info->watch_timers.wi_by_process,
		     &info->watch_timers);
	info->watch_timers.wi_event = &sched_dummy_timer;
    }

    /*
     * Switch to the new event set.
     */
    forkx->wakeup_posted = FALSE;
    level = disable_interrupts();
    requeue(&forkx->process_event_setQ, forkx->current_events);
    forkx->current_events = info;
    enable_interrupts(level);
    return(TRUE);
}

/*
 * process_pop_event_list
 *
 * Restore the precious event list, and delete the one that had been in
 * use.  This routine is called by libraries of code that need to specify
 * an exact event list without worrying about preexisting events.
 */
boolean process_pop_event_list (sched_event_set **save_current)
{
    leveltype level;
    sched_event_set *info;
    list_element *element;
    watcher_info *wakeup;

    /*
     * Switch to the previous event set.
     */
    if (QUEUEEMPTY(&forkx->process_event_setQ))
	return(FALSE);
    forkx->wakeup_posted = FALSE;
    level = disable_interrupts();
    info = forkx->current_events;
    forkx->current_events = dequeue(&forkx->process_event_setQ);
    enable_interrupts(level);

    /*
     * The caller has indicated that they want to detach the current
     * event list, but otherwise keep it intact.
     */
    if (save_current) {
        FOR_ALL_ELEMENTS_IN_LIST(&info->process_events, element) {
	    wakeup = LIST_GET_DATA(element);
	    wakeup->wi_process = NULL;
	}
	*save_current = info;
	return(TRUE);
    }

    /*
     * Scheduler cleanup.
     */
    list_remove(&info->process_events, &info->watch_timers.wi_by_process,
		   &info->watch_timers);

    /*
     * The caller should have already cleaned up.  If there is anything
     * left after the scheduler cleans up its part, complain.
     */
    if (info->watched_simple_timer)
	(*kernel_errmsg)(&msgsym(STILLWATCHINGT, SCHED), "simple",
			 info->watched_simple_timer);
    if (info->watched_mgd_timer)
	(*kernel_errmsg)(&msgsym(STILLWATCHINGT, SCHED), "managed",
			 info->watched_mgd_timer);
    while ((wakeup = list_dequeue(&info->process_events)) != NULL) {
	list_remove(wakeup->wi_by_event.list, &wakeup->wi_by_event, wakeup);
	(*kernel_errmsg)(&msgsym(STILLWATCHING, SCHED),
	       event_type2text(wakeup->wi_event->major_type),
	       wakeup->wi_event->name);
	free(wakeup);
    }
    list_destroy(&info->process_events);
    free(info);
    return(TRUE);
}

/**********************************************************************
 *
 *		  PROCESS QUEUE MANAGEMENT ROUTINES
 *			  EXTERNAL INTERFACE
 *
 **********************************************************************/

/*
 * create_watched_queue
 *
 * Create a queue with the additional information necessary for it to
 * be manipulated by the scheduler.
 */
watched_queue *create_watched_queue (char *name, int maximum, ulong id)
{
    watched_queue *event;

    event = watcher_create_common(&s_notify_queue, sizeof(watched_queue), name,
				  QUEUE_EVENT, id);
    if (event)
	queue_init(&event->q, maximum);
    return(event);
}

/*
 * delete_watched_queue
 *
 * Unthread a watched queue from any notification lists, and then discard
 * it.  This routine also discards all items on the queue.  It has no way
 * to determine whether or not these items were malloced, so it cannot know if
 * it should free or retbuffer them.
 */
void delete_watched_queue (watched_queue **user_event)
{
    watched_queue *event;
    leveltype level;
    char *dummy;

    if (!user_event) {
	errmsg(&msgsym(WATCH, SCHED), "delete", "queue", user_event);
	return;
    }
    event = *user_event;
    if (!event) {
	errmsg(&msgsym(WATCH, SCHED), "delete", "queue", event);
	return;
    }
    level = raise_interrupt_level(ALL_DISABLE);
    if (!watcher_delete_common(&event->wi, "queue")) {
	reset_interrupt_level(level);
	return;
    }
    if (WQ_QUEUESIZE(event)) {
	(*kernel_errmsg)(&msgsym(QUEUENOTEMPTY, SCHED), event->wi.name, event);
	while ((dummy = p_dequeue(&event->q)) != NULL) {
	    /*
	     * Abandon the items on this queue.
	     */
	}
    }
    free(event);
    *user_event = NULL;
    reset_interrupt_level(level);
}

/*
 * process_watch_queue
 *
 * Add this process to the list of processes that want notification when
 * a particular queue has a new element added to it.
 */
void process_watch_queue (
    watched_queue *event,
    ENABLESTATE watch,
    ONESHOTTYPE one_shot)
{
    if (event == NULL) {
	(*kernel_errmsg)(&msgsym(WATCH, SCHED), "monitor", "queue", event);
	return;
    }
    process_watch_common(&event->wi, watch, one_shot,
			 !WQ_QUEUEEMPTY(event));
}

/*
 * process_enqueue
 *
 * Enqueue an element onto a queue, and then notify any processes that are
 * interested in the contents of this queue.
 */
boolean process_enqueue (watched_queue *event, void *whatever)
{
    if (event == NULL) {
	(*kernel_errmsg)(&msgsym(WATCH, SCHED), "enqueue", "queue", event);
	return(FALSE);
    }
    if (QUEUEFULL(&event->q))
	return(FALSE);
    p_enqueue(&event->q, whatever);
    process_notify_event(&event->wi);
    return(TRUE);
}

/*
 * process_enqueue_pak
 *
 * This routine has moved to if/network.c because it has explicit
 * knowledge of the protocol_discard routine.  It will also never be
 * called except by code that has knowledte of interfaces, so that's a
 * more appropriate place.
 */

/*
 * process_requeue
 *
 * Requeue an element onto a queue, and then notify any processes that are
 * interested in the contents of this queue.
 */
boolean process_requeue (watched_queue *event, void *whatever)
{
    if (event == NULL) {
	(*kernel_errmsg)(&msgsym(WATCH, SCHED), "requeue", "queue", event);
	return(FALSE);
    }
    if (QUEUEFULL(&event->q))
	return(FALSE);
    p_requeue(&event->q, whatever);
    process_notify_event(&event->wi);
    return(TRUE);
}

/*
 * process_requeue_pak
 *
 * This routine has moved to if/network.c because it has explicit
 * knowledge of the protocol_discard routine.  It will also never be
 * called except by code that has knowledte of interfaces, so that's a
 * more appropriate place.
 */

/*
 * process_dequeue
 *
 * Dequeue an element from a queue.  This wrapper in only needed
 * because the internals of the data structure are hidden from the
 * world.
 */
void *process_dequeue (watched_queue *event)
{
    if (event == NULL) {
	(*kernel_errmsg)(&msgsym(WATCH, SCHED), "dequeue", "queue", event);
	return(NULL);
    }
    return(p_dequeue(&event->q));
}

/*
 * process_is_queue_empty
 *
 * Indicate whether or not the queue is empty.  This routine is necessary
 * because the data structure is completely opaque to the user.
 */

boolean process_is_queue_empty (watched_queue *queue)
{
    if (queue == NULL)
	return(TRUE);
    return(QUEUEEMPTY(&queue->q));
}

/*
 * process_is_queue_full
 *
 * Indicate whether or not the queue is full.  This routine is necessary
 * because the data structure is completely opaque to the user.
 */

boolean process_is_queue_full (watched_queue *queue)
{
    if (queue == NULL)
	return(TRUE);
    return(QUEUEFULL(&queue->q));
}

/*
 * process_queue_resize
 *
 * If new size is smaller than number of packets on queue, return all
 * the overflow elements on the provided queue.  They cannot be
 * returned to the system by this routine, because it has no way of
 * knowing whether they are packet buffers or allocated memory.
 */

void process_resize_queue (watched_queue *wq, int new_size, queuetype *overflow)
{
    nexthelper *prev, *current;
    queuetype *queue;
    leveltype status;
    int i;

    if (!wq) {
	errmsg(&msgsym(WATCH, SCHED), "use", "queue", wq);
	return;
    }
    queue = &wq->q;
    if ((new_size == 0) || (new_size >= queue->count)) {
	queue->maximum = new_size;
	return;
    }

    status = raise_interrupt_level(ALL_DISABLE);
    prev = NULL;
    current = (nexthelper *)queue->qhead;
    for (i = 0; (i < new_size) && (current != NULL); i++) {
	prev = current;
	current = current->next;	
    }

    overflow->qhead = current;
    overflow->qtail = queue->qtail;
    overflow->count = queue->count - new_size;
    overflow->maximum = 0;

    queue->qtail = prev;
    queue->count = new_size;
    queue->maximum = new_size;
    prev->next = NULL;
    reset_interrupt_level(status);
}

/*
 * process_queue_peek
 *
 * Return the first item on the queue.  This routine is necessary
 * because the data structure is completely opaque to the user.
 */
void *process_queue_peek (watched_queue *queue)
{
    if (queue == NULL)
	return(NULL);
    return(queue->q.qhead);
}

/*
 * process_queue_size
 *
 * Indicate the current size of the queue.  This routine is necessary
 * because the data structure is completely opaque to the user.
 */
int process_queue_size (watched_queue *queue)
{
    if (queue == NULL)
	return(0);
    return(QUEUESIZE(&queue->q));
}

/*
 * process_queue_max_size
 *
 * Indicate the maximum size of the queue.  This routine is necessary
 * because the data structure is completely opaque to the user.
 */
int process_queue_max_size (watched_queue *queue)
{
    if (queue == NULL)
	return(0);
    return(QUEUEMAXIMUM(&queue->q));
}

/**********************************************************************
 *
 *		    PROCESS TIMER MANAGEMENT ROUTINES
 *			  EXTERNAL INTERFACE
 *
 **********************************************************************/

/*
 * process_watch_mgd_timer
 *
 * Add a managed timer to the list of things that can wake up this
 * process.  The timer is not linked into the scheduler's timer tree at
 * this point, but will be linked in during the call to
 * process_wait_for_event.  This routine and process_watch_timer are
 * mutually exclusive.
 */
void process_watch_mgd_timer (
    mgd_timer *timer,
    ENABLESTATE watch)
{
    sched_event_set *info;

    if (timer == NULL) {
	(*kernel_errmsg)(&msgsym(WATCH, SCHED), "monitor", "mgd timer", timer);
	return;
    }
    info = forkx->current_events;
    if (watch == ENABLE) {
	info->watched_mgd_timer = timer;
	return;
    }

    if (info->watched_mgd_timer != timer) {
	(*kernel_errmsg)(&msgsym(NOTWATCHTIMER, SCHED), "managed", timer);
	return;
    }
    info->watched_mgd_timer = NULL;
}

/*
 * process_watch_timer
 *
 * Add a simple timer to the list of things that can wake up this
 * process.  The scheduler's per-process timer will be set with the value
 * of this timer during the call to process_wait_for_event.  This routine
 * and process_watch_mgd_timer are mutually exclusive.
 */
void process_watch_timer (
    sys_timestamp *timer,
    ENABLESTATE watch)
{
    sched_event_set *info;

    if (timer == NULL) {
	(*kernel_errmsg)(&msgsym(WATCH, SCHED), "monitor", "timer", timer);
	return;
    }
    info = forkx->current_events;
    if (watch == ENABLE) {
	info->watched_simple_timer = timer;
	return;
    }

    if (info->watched_simple_timer != timer) {
	(*kernel_errmsg)(&msgsym(NOTWATCHTIMER, SCHED), "simple", timer);
	return;
    }
    info->watched_simple_timer = NULL;
}


/**********************************************************************
 *
 *		 PROCESS BOOLEAN MANAGEMENT ROUTINES
 *			  EXTERNAL INTERFACE
 *
 **********************************************************************/

/*
 * create_watched_boolean
 *
 * Create a boolean with the additional information necessary for it to
 * be manipulated by the scheduler.
 */
watched_boolean *create_watched_boolean (char *name, ulong id)
{
    watched_boolean *event;

    event = watcher_create_common(&s_notify_boolean, sizeof(watched_boolean),
				  name, BOOLEAN_EVENT, id);
    return(event);
}

/*
 * delete_watched_boolean
 *
 * Unthread a watched boolean from any notification lists, and then
 * discard it.
 */
void delete_watched_boolean (watched_boolean **user_bool)
{
    watched_boolean *bool;
    leveltype level;

    if (!user_bool) {
	errmsg(&msgsym(WATCH, SCHED), "delete", "bool", user_bool);
	return;
    }
    bool = *user_bool;
    if (!bool) {
	errmsg(&msgsym(WATCH, SCHED), "delete", "bool", bool);
	return;
    }
    level = raise_interrupt_level(ALL_DISABLE);
    if (!watcher_delete_common(&bool->wi, "boolean")) {
	reset_interrupt_level(level);
	return;
    }
    free(bool);
    *user_bool = NULL;
    reset_interrupt_level(level);
}

/*
 * process_watch_boolean
 *
 * Add this process to the list of processes that want notification when
 * a particular boolean changes state.
 */
void process_watch_boolean (
    watched_boolean *event,
    ENABLESTATE watch,
    ONESHOTTYPE one_shot)
{
    if (event == NULL) {
	(*kernel_errmsg)(&msgsym(WATCH, SCHED), "monitor", "boolean", event);
	return;
    }
    process_watch_common(&event->wi, watch, one_shot, WB_VALUE(event));
}

/*
 * process_get_boolean
 *
 * Get the value of a boolean value.  This routine is necessary
 * because the data structure is completely opaque to the user.
 */
boolean process_get_boolean (watched_boolean *bool)
{
    return(bool ? bool->value : FALSE);
}

/*
 * process_set_boolean
 *
 * Set a boolean value, and then notify any processes that are
 * interested in the state of this boolean.
 */
void process_set_boolean (watched_boolean *event, boolean value)
{
    boolean already_set;

    if (event == NULL) {
	(*kernel_errmsg)(&msgsym(WATCH, SCHED), "set", "boolean", event);
	return;
    }
    already_set = event->value;
    event->value = value;
    if (value && !already_set)
	process_notify_event(&event->wi);
}

/*
 * process_wait_on_system_init
 *
 * Wait for the system to start up.  This is nothing more than a
 * blocking test on the system initialization flag.
 */
void process_wait_on_system_init (void)
{
    /* Already started? */
    if (WB_VALUE(watched_system_init))
	return;

    /*
     * Nope, wait for it.  See if the current event list can be saved.
     * If not, poll intermittently.
     */
    while (!process_push_event_list(NULL)) {
	if (WB_VALUE(watched_system_init))
	    return;
	process_sleep_for(ONESEC);
    }

    /*
     * Current event list saved.  Install a watch on the system init
     * variable and go to sleep.
     */
    process_watch_boolean(watched_system_init, ENABLE, ONE_SHOT);
    while (!WB_VALUE(watched_system_init)) {
	process_set_wakeup_reasons(BOOLEAN_EVENT, ONE_SHOT);
	process_wait_for_event();
    }
    process_watch_boolean(watched_system_init, DISABLE, ONE_SHOT);
    process_pop_event_list(NULL);
}

/*
 * process_wait_on_system_config
 *
 * Wait for the system to be configured.  This is nothing more than a
 * blocking test on the system configured flag.
 */
void process_wait_on_system_config (void)
{
    /* Already started? */
    if (WB_VALUE(watched_system_config))
	return;

    /*
     * Nope, wait for it.  See if the current event list can be saved.
     * If not, poll intermittently.
     */
    while (!process_push_event_list(NULL)) {
	if (WB_VALUE(watched_system_config))
	    return;
	process_sleep_for(ONESEC);
    }
    /*
     * Current event list saved.  Install a watch on the system config
     * variable and go to sleep.
     */
    process_watch_boolean(watched_system_config, ENABLE, ONE_SHOT);
    while (!WB_VALUE(watched_system_config)) {
	process_set_wakeup_reasons(BOOLEAN_EVENT, ONE_SHOT);
	process_wait_for_event();
    }
    process_watch_boolean(watched_system_config, DISABLE, ONE_SHOT);
    process_pop_event_list(NULL);
}


/**********************************************************************
 *
 *		 PROCESS BITFIELD MANAGEMENT ROUTINES
 *			  EXTERNAL INTERFACE
 *
 **********************************************************************/

/*
 * create_watched_bitfield
 *
 * Create a bitfield with the additional information necessary for it to
 * be manipulated by the scheduler.
 */
watched_bitfield *create_watched_bitfield (char *name, ulong id)
{
    watched_bitfield *event;

    event = watcher_create_common(&s_notify_bitfield, sizeof(watched_bitfield),
				  name, BITFIELD_EVENT, id);
    return(event);
}

/*
 * delete_watched_bitfield
 *
 * Unthread a watched bitfield from any notification lists, and then
 * discard it.
 */
void delete_watched_bitfield (watched_bitfield **user_bits)
{
    watched_bitfield *bits;
    leveltype level;

    if (!user_bits) {
	errmsg(&msgsym(WATCH, SCHED), "delete", "bitfield", user_bits);
	return;
    }
    bits = *user_bits;
    if (!bits) {
	errmsg(&msgsym(WATCH, SCHED), "delete", "bitfield", bits);
	return;
    }
    level = raise_interrupt_level(ALL_DISABLE);
    if (!watcher_delete_common(&bits->wi, "bitfield")) {
	reset_interrupt_level(level);
	return;
    }
    free(bits);
    *user_bits = NULL;
    reset_interrupt_level(level);
}

/*
 * process_watch_bitfield
 *
 * Add this process to the list of processes that want notification when
 * a particular bitfield changes state.
 */
void process_watch_bitfield (
    watched_bitfield *event,
    ENABLESTATE watch,
    ONESHOTTYPE one_shot)
{
    if (event == NULL) {
	(*kernel_errmsg)(&msgsym(WATCH, SCHED), "monitor", "bitfield", event);
	return;
    }
    process_watch_common(&event->wi, watch, one_shot, FALSE);
}

/*
 * process_get_bitfield
 *
 * Get the value of a bitfield value.  This routine is necessary
 * because the data structure is completely opaque to the user.
 */
ulong process_get_bitfield (watched_bitfield *bitfield)
{
    return(bitfield ? bitfield->bits : 0);
}

/*
 * process_set_bitfield
 *
 * Set the specified bits in a bitfield value, and then notify any
 * processes that are interested in the state of this bitfield.
 */
void process_set_bitfield (
    watched_bitfield *event,
    ulong value,
    boolean send_wakeup)
{
    if (event == NULL) {
	(*kernel_errmsg)(&msgsym(WATCH, SCHED), "set", "bitfield", event);
	return;
    }
    event->bits |= value;
    if (send_wakeup)
	process_notify_event(&event->wi);
}

/*
 * process_clear_bitfield
 *
 * Clear the specified bits in a bitfield value, and then notify any
 * processes that are interested in the state of this bitfield.
 */
void process_clear_bitfield (
    watched_bitfield *event,
    ulong value,
    boolean send_wakeup)
{
    if (event == NULL) {
	(*kernel_errmsg)(&msgsym(WATCH, SCHED), "clear", "bitfield", event);
	return;
    }
    event->bits &= ~value;
    if (send_wakeup)
	process_notify_event(&event->wi);
}

/*
 * process_keep_bitfield
 *
 * Keep the specified bits in a bitfield value, and then notify any
 * processes that are interested in the state of this bitfield.
 */
void process_keep_bitfield (
    watched_bitfield *event,
    ulong value,
    boolean send_wakeup)
{
    if (event == NULL) {
	(*kernel_errmsg)(&msgsym(WATCH, SCHED), "keep", "bitfield", event);
	return;
    }
    event->bits &= value;
    if (send_wakeup)
	process_notify_event(&event->wi);
}

/**********************************************************************
 *
 *		PROCESS SEMAPHORE MANAGEMENT ROUTINES
 *			  EXTERNAL INTERFACE
 *
 **********************************************************************/

/*
 * create_watched_semaphore
 *
 * Create a semaphore with the additional information necessary for it to
 * be manipulated by the scheduler.
 */
watched_semaphore *create_watched_semaphore (char *name, ulong id)
{
    watched_semaphore *event;

    event = watcher_create_common(&s_notify_semaphore, sizeof(watched_semaphore),
				  name, SEMAPHORE_EVENT, id);
    if (event)
	event->owner = NO_PROCESS;
    return(event);
}

/*
 * delete_watched_semaphore
 *
 * Unthread a watched semaphore from any notification lists, and then
 * discard it.
 */
void delete_watched_semaphore (watched_semaphore **user_sem)
{
    watched_semaphore *sem;
    leveltype level;

    if (!user_sem) {
	errmsg(&msgsym(WATCH, SCHED), "delete", "semaphore", user_sem);
	return;
    }
    sem = *user_sem;
    if (!sem) {
	errmsg(&msgsym(WATCH, SCHED), "delete", "semaphore", sem);
	return;
    }
    level = raise_interrupt_level(ALL_DISABLE);
    if (!watcher_delete_common(&sem->wi, "semaphore")) {
	reset_interrupt_level(level);
	return;
    }
    free(sem);
    *user_sem = NULL;
    reset_interrupt_level(level);
}

/*
 * process_watch_semaphore
 *
 * Add this process to the list of processes that want notification when
 * a particular semaphore is freed.
 */
void process_watch_semaphore (
    watched_semaphore *event,
    ENABLESTATE watch,
    ONESHOTTYPE one_shot)
{
    if (event == NULL) {
	(*kernel_errmsg)(&msgsym(WATCH, SCHED), "monitor", "semaphore", event);
	return;
    }
    process_watch_common(&event->wi, watch, one_shot, FALSE);
}

/*
 * lock a semaphore
 *
 * Attempt to grab a semaphore.  Return true if the semaphore was
 * successfully snagged.  This routine may only be called from the
 * scheduler code.  Outside callers must call process_lock_semaphore.
 */
static boolean lock_watched_semaphore (watched_semaphore *event,
				       boolean in_interrupt)
{
    if (lock_semaphore(&event->s)) {
	if (in_interrupt)
	    event->owner = 0;
	else
	    event->owner = forkx->pid;
	return(TRUE);
    }
    return(FALSE);
}

/*
 * process_lock_semaphore
 *
 * Attempt to grab a semaphore.  This routine will block until either the
 * process gets the semaphore, or the timeout value expires.  It returns
 * true if the semaphore was successfully snagged.
 */
boolean process_lock_semaphore (watched_semaphore *sem, ulong timeout)
{
    sys_timestamp wakeup_time;
    boolean in_interrupt, success;

    if (sem == NULL) {
	(*kernel_errmsg)(&msgsym(WATCH, SCHED), "lock", "semaphore", sem);
	return(FALSE);
    }

    in_interrupt = onintstack();
    if (lock_watched_semaphore(sem, in_interrupt))
	return(TRUE);
    if ((timeout == 0) || in_interrupt)
	return(FALSE);

    /*
     * Make sure the timeout doesn't drift with repeated attempts to get
     * the semaphore.
     */
    if (timeout != -1L) {
	TIMER_START64(wakeup_time, timeout);
    }

    success = TRUE;
    if (!process_push_event_list(NULL))
	return(FALSE);
    process_watch_semaphore(sem, ENABLE, RECURRING);
    while (!lock_watched_semaphore(sem, in_interrupt)) {
	if (timeout == -1L) {
	    process_wait_for_event();
	} else {
	    process_wait_for_event_timed(TIME_LEFT_SLEEPING(wakeup_time));
	    if (AWAKE(wakeup_time)) {
		success = FALSE;
		break;
	    }
	}
    }
    process_watch_semaphore(sem, DISABLE, RECURRING);
    process_pop_event_list(NULL);
    return(success);
}

/*
 * process_unlock_semaphore
 *
 * Release the semaphore.  The caller must be the process that is holding
 * the semaphore.
 */
void process_unlock_semaphore (watched_semaphore *event)
{
    const char *owner_name, *caller_name;
    boolean interrupt_handler;

    if (event == NULL) {
	(*kernel_errmsg)(&msgsym(WATCH, SCHED), "unlock", "semaphore", event);
	return;
    }

    interrupt_handler = (onintstack() != 0);
    if (event->owner == NO_PROCESS) {
	caller_name = interrupt_handler ? "interrupt" : forkx->name;
	(*kernel_errmsg)(&msgsym(SEMNOTLOCKED, SCHED), caller_name);
	return;
    } else if (event->owner) {
	if ((event->owner != forkx->pid) || interrupt_handler) {
	    if (!process_get_name(event->owner, &owner_name))
		owner_name = "unknown";
	    caller_name = interrupt_handler ? "interrupt" : forkx->name;
	    (*kernel_errmsg)(&msgsym(SEMUNLOCK, SCHED), caller_name,
			     owner_name);
	    return;
	}
    } else if (!interrupt_handler) {
	(*kernel_errmsg)(&msgsym(SEMUNLOCK, SCHED), forkx->name, "interrupt");
	return;
    }
    event->owner = NO_PROCESS;
    event->s = 0;
    process_notify_event(&event->wi);
}


/**********************************************************************
 *
 *		    GENERIC MESSAGE SENDING/RECEIPT
 *			  EXTERNAL INTERFACE
 *
 **********************************************************************/

/*
 * process_get_message
 *
 * Get the next message outstanding for this process.  A message is
 * nothing more than a constant that identifies an action to perform, and
 * possibly one or two arguments.  The advantages of this over a callback
 * routine is that the code executes withing the process of the receiver,
 * not on the stack of the sender.
 */
boolean process_get_message (ulong *type, void **pointer, ulong *value)
{
    message_type *message;

    if (QUEUEEMPTY(&forkx->message_queue))
	return(FALSE);
    message = p_dequeue(&forkx->message_queue);
    *type = message->msg_id;
    *pointer = message->msg_ptr_arg;
    *value = message->msg_num_arg;
    chunk_free(message_chunks, message);
    return(TRUE);
}

/*
 * process_send_message
 *
 * Send a message to another process.  A message is nothing more than a
 * constant that identifies an action to perform, and possibly one or two
 * arguments.  The advantages of this over a callback routine is that the
 * code executes withing the process of the receiver, not on the stack of
 * the sender.
 */
boolean process_send_message (
    pid_t pid,
    ulong message_num,
    void *pointer,
    ulong numeric)
{
    message_type *message;
    sprocess *proc;

    if (pid == -1)
	return(FALSE);
    proc = process_pid2ptr(pid);
    if (proc == NULL)
	return(FALSE);

    message = chunk_malloc(message_chunks);
    if (message == NULL)
	return(FALSE);
    message->msg_id = message_num;
    message->msg_ptr_arg = pointer;
    message->msg_num_arg = numeric;
    p_enqueue(&proc->message_queue, message);
    proc->watch_messages.wi_occurred = TRUE;
    if (QUEUEEMPTY(&proc->process_event_setQ))
	process_wakeup_internal(proc, MESSAGE_EVENT, 0);
    return(TRUE);
}

/*
 * process_initialize_messages
 *
 * Initialize the message pool.
 */
void process_initialize_messages (void)
{
    message_chunks = chunk_create(sizeof(message_type), MESSAGES_PER_CHUNK,
				  CHUNK_FLAGS_DYNAMIC,
				  NULL, 0,
				  "messages");
}


/**********************************************************************
 *
 *		    READ/MODIFY PROCESS ATTRIBUTES
 *			  EXTERNAL INTERFACE
 *
 **********************************************************************/

/*
 * process_getset_lookup
 *
 * Convert from a pid to a sprocess structure, and then run a few
 * validity checks.
 */
static sprocess *process_getset_lookup (
    pid_t pid,
    boolean any,
    char *operation,
    char *field)
{
    sprocess *proc;

    /*
     * For the current process?  This is always a valid request.
     */
    if ((pid == THIS_PROCESS) || (forkx && (pid == forkx->pid)))
	return(forkx);

    /*
     * Not the current process.  Are we allowed to get information for
     * any other process?
     */
    if (!any) {
	(*kernel_errmsg)(&msgsym(ATTRCMD, SCHED), operation, field, pid);
	return(NULL);
    }

    /*
     * Yes.  Find the process and return it.
     */
    proc = process_pid2ptr(pid);
    if (proc == NULL) {
	(*kernel_errmsg)(&msgsym(NOPROCESS, SYS), pid);
	return(NULL);
    }
    return(proc);
}

/*
 * process_get_info
 *
 * Return the value of one of the processes attributes.  This routine
 * should never be called directly.  It should only ever be called via
 * one of the wrapper routines defined in process.h.
 */
boolean process_get_info (int type, pid_t pid, ...)
{
    sprocess *proc;
    boolean *bool_ptr;
    ulong *num_ptr;
    char **name_ptr;
    pid_t *pid_ptr;
    void **ptr_ptr;
    sys_timestamp *tstmp_ptr;
    tt_soc **ttsoc_ptr;
    va_list ap;

    va_start(ap, pid);
    switch (type) {
      case PI_ANALYZE:
	proc = process_getset_lookup(pid, TRUE, "get", "analyze");
	if (proc) {
	    return(proc->analyze);
	    return(TRUE);
	}
	return(FALSE);

     case PI_ARG:
	ptr_ptr = va_arg(ap, void *);
	proc = process_getset_lookup(pid, FALSE, "get", "arg");
	if (proc && proc->process_arg_valid) {
	    *ptr_ptr = proc->process_arg;
	    return(TRUE);
	}
	return(FALSE);

      case PI_CRASHBLOCK:
	bool_ptr = va_arg(ap, boolean *);
	proc = process_getset_lookup(pid, TRUE, "get", "crashblock");
	if (proc) {
	    *bool_ptr = proc->crashblock;
	    return(TRUE);
	}
	return(FALSE);

      case PI_NAME:
	name_ptr = va_arg(ap, char **);
	if (!system_running)
	    *name_ptr = "*Init*";
	else if (schedflag)
	    *name_ptr = "*Sched*";
	else if ((proc = process_pid2ptr(pid)) != 0)
	    *name_ptr = proc->name;
	else
	    *name_ptr = "Unknown";
	return(TRUE);

      case PI_PID:
	pid_ptr = va_arg(ap, pid_t *);
	if (pid == THIS_PROCESS)
	    *pid_ptr = forkx->pid;
	else
	    *pid_ptr = pid;
	return(TRUE);

     case PI_PROFILE:
	bool_ptr = va_arg(ap, boolean *);
	proc = process_getset_lookup(pid, TRUE, "get", "profile");
	if (proc) {
	    *bool_ptr = proc->profile;
	    return(TRUE);
	}
	return(FALSE);

      case PI_PRIORITY:
	num_ptr = va_arg(ap, ulong *);
	proc = process_getset_lookup(pid, TRUE, "get", "priority");
	if (proc) {
	    *num_ptr = (proc->priority);
	    return(TRUE);
	}
	return(FALSE);

      case PI_RUNTIME:
	num_ptr = va_arg(ap, ulong *);
	proc = process_getset_lookup(pid, TRUE, "get", "runtime");
	if (proc) {
	    *num_ptr = proc->elapsedtime + ELAPSED_TIME(proc->starttime);
	    return(TRUE);
	}
	return(FALSE);

      case PI_STACKSIZE:
	num_ptr = va_arg(ap, ulong *);
	proc = process_getset_lookup(pid, TRUE, "get", "stacksize");
	if (proc) {
	    *num_ptr = proc->stacksize;
	    return(TRUE);
	}
	return(FALSE);

      case PI_STARTTIME:
	tstmp_ptr = va_arg(ap, sys_timestamp *);
	proc = process_getset_lookup(pid, TRUE, "get", "starttime");
	if (proc) {
	    *tstmp_ptr = proc->starttime;
	    return(TRUE);
	}
	return(FALSE);

      case PI_TTYSOC:
	ttsoc_ptr = va_arg(ap, tt_soc **);
	proc = process_getset_lookup(pid, TRUE, "get", "ttysoc");
	if (proc) {
	    *ttsoc_ptr = proc->ttyptr;
	    return(TRUE);
	}
	return(FALSE);

      case PI_TTYNUM:
	num_ptr = va_arg(ap, ulong *);
	proc = process_getset_lookup(pid, TRUE, "get", "ttynum");
	if (proc && proc->ttyptr) {
	    *num_ptr = proc->ttyptr->ttynum;
	    return(TRUE);
	}
	return(FALSE);

      case PI_WAKEUP_REASONS:
	num_ptr = va_arg(ap, ulong *);
	proc = process_getset_lookup(pid, TRUE, "get", "wakeup");
	if (proc) {
	    *num_ptr = proc->wakeup_reasons;
	    return(TRUE);
	}
	return(FALSE);

      default:
	(*kernel_errmsg)(&msgsym(NOATTR, SCHED), "get", type);
	return(FALSE);
    }
}

/*
 * process_set_info
 *
 * Modify the value of one of the processes attributes.  This routine
 * should never be called directly.  It should only ever be called via
 * one of the wrapper routines defined in process.h.
 */
boolean process_set_info (int type, pid_t pid, ...)
{
    sprocess *proc;
    int number;
    va_list ap;

    va_start(ap, pid);
    switch (type) {
      case PI_ANALYZE:
	proc = process_getset_lookup(pid, TRUE, "set", "analyze");
	if (proc)
	    proc->analyze = va_arg(ap, boolean);
	break;

      case PI_ARG:
	proc = process_getset_lookup(pid, TRUE, "set", "arg");
	if (proc) {
	    proc->process_arg = va_arg(ap, void *);
	    proc->process_arg_valid = TRUE;
	}
	break;

      case PI_CRASHBLOCK:
	proc = process_getset_lookup(pid, TRUE, "set", "crashblock");
	if (proc)
	    proc->crashblock = va_arg(ap, boolean);
	break;

      case PI_NAME:
	proc = process_getset_lookup(pid, FALSE, "set", "name");
	if (proc)
	    proc->name = va_arg(ap, char *);
	break;

      case PI_PRIORITY:
	proc = process_getset_lookup(pid, FALSE, "set", "priority");
	if (proc) {
	    number = va_arg(ap, int);
	    if ((number < PRIO_CRITICAL) || (number > PRIO_LOW)) {
		(*kernel_errmsg)(&msgsym(ATTRVALUE, SCHED), "priority",
				 number, pid);
		return(FALSE);
	    }
	    if (!proc->prefers_new && (number <= PRIO_HIGH)) {
		(*kernel_errmsg)(&msgsym(SETHIGHORCRIT, SCHED),
				 ((number == PRIO_HIGH) ? "high" : "critical"),
				 pid, proc->name);
		return(FALSE);
	    }
	    proc->priority = number;
	    if (proc->on_old_queue)
		old_run_add(proc);
	}
	break;

      case PI_PROFILE:
	proc = process_getset_lookup(pid, TRUE, "set", "profile");
	if (proc)
	    proc->profile = va_arg(ap, boolean);
	break;

      case PI_TIMING_INFO:
	proc = process_getset_lookup(pid, TRUE, "set", "timing");
	if (proc) {
	    proc->elapsedtime = va_arg(ap, ulong);
	    proc->ncalls = va_arg(ap, long);
	}
	break;

      case PI_TTYSOC:
	proc = process_getset_lookup(pid, TRUE, "set", "ttysoc");
	if (proc) {
	    proc->ttyptr = va_arg(ap, tt_soc *);/* redefine standard I/O block */
	    if (proc == forkx)
		stdio = proc->ttyptr;		/* Redefine stdio too */
	}
	break;

      case PI_TTYNUM:
	proc = process_getset_lookup(pid, TRUE, "set", "ttynum");
	if (proc) {
	    number = va_arg(ap, int);
	    proc->ttyptr = MODEMS[number];	/* redefine standard I/O block */
	    if (proc == forkx)
		stdio = proc->ttyptr;		/* Redefine stdio too */
	}
	break;

      case PI_WAKEUP_REASONS:
	proc = process_getset_lookup(pid, FALSE, "set", "wakeup");
	if (proc) {
	    proc->wakeup_reasons = va_arg(ap, ulong);
	    if (va_arg(ap, ulong) == RECURRING)
		proc->default_wakeup_reasons = proc->wakeup_reasons;
	}
	break;

      default:
	proc = NULL;
	(*kernel_errmsg)(&msgsym(NOATTR, SCHED), "set", type);
	break;
    }
    va_end(ap);
    return(proc ? TRUE : FALSE);
}

/*
 * process_set_all_profiles
 *
 * Clear the "profile_me" flag for all tasks.
 */
void process_set_all_profiles (boolean profile_me)
{
    sprocess_list *array;
    sprocess *sp;
    ulong i;

    for (array = sarrayQ.qhead; array; array = array->sl_next) {
	for (i = 0; i < MAX_PROC_PER_ARRAY; ++i) {
	    sp = array->sl_process[i];
	    if (sp) {
		sp->profile = profile_me;
	    }
	}
    }
}


/**********************************************************************
 *
 *		    LEARN WHY A PROCESS WOKE UP
 *
 **********************************************************************/

/*
 * process_get_wakeup
 *
 * Find out the next reason why a process was awakened.  This routine should be
 * called repeatedly until it returns FALSE.  The wakeup reasons are returned in
 * the arguments major and minor.  The major argument specifies a class of events
 * such as a TIMER or a QUEUE.  For QUEUE events, the minor argument specifies
 * which queue, assuming values were assigned when queue was created.
 */
boolean process_get_wakeup (ulong *major, ulong *minor)
{
    list_element *element;
    watcher_info *wakeup;
    watched_item *event;
    ulong count, count_max;
    sched_event_set *event_set;

    /*
     * Did someone explicitly kick this process?
     */
    if (forkx->direct_wakeup_major) {
	*major = forkx->direct_wakeup_major;
	*minor = forkx->direct_wakeup_minor;
	forkx->direct_wakeup_major = 0;
	forkx->direct_wakeup_minor = 0;
	return(TRUE);
    }

    /*
     * Starting at current position, work to the end of the list, and
     * then wrap back around to the beginning.  Make no more than one
     * complete pass around the loop per scheduler invocation, not per
     * call to this routine.  This provides fairness to other processes.
     */
    event_set = forkx->current_events;
    count_max = LIST_SIZE(&event_set->process_events);
    count = event_set->event_count;
    element = event_set->next_event;
    if (element == NULL)
	element = LIST_HEAD_ELEMENT(&event_set->process_events);

    do {
	wakeup = LIST_GET_DATA(element);
	if (wakeup->wi_occurred) {
	    wakeup->wi_occurred = FALSE;
	    event = wakeup->wi_event;
	    *major = event->major_type;
	    *minor = event->minor_type;
	    event_set->next_event = LIST_NEXT_ELEMENT(element);
	    event_set->event_count = ++count;
	    return(TRUE);
	}
	element = LIST_NEXT_ELEMENT(element);
	if (element == NULL)
	    element = LIST_HEAD_ELEMENT(&event_set->process_events);
    } while (++count < count_max);
    return(FALSE);
}


/**********************************************************************
 *
 *			      DEBUGGING
 *
 **********************************************************************/

#ifdef SCHED_DEBUG
void process_set_debug (pid_t pid, uchar mask)
{
    sprocess *proc;
    
    proc = process_pid2ptr(pid);
    if (proc)
	proc->debug_events = mask;
}
#endif


/*
 * critical_background
 *
 * Perform a variety of network related background tasks.
 * These tasks must be performed quickly and may not block for
 * any reason.
 */

process critical_background (void)
{
    process_watch_boolean(critical_background_flag, ENABLE, RECURRING);
    while (TRUE) {
	process_safe_wait_for_event();

	/*
	 * If someone ask for a new resource to be created at process
	 * level, execute the list of clients registered for this
	 * service.
	 */
	if (process_get_boolean(critical_background_flag)) {
	    process_set_boolean(critical_background_flag, FALSE);
	    reg_invoke_critical_background();
	}
    }
}

/*
 * critical_background_wake
 *
 * Wake up the Network Background process and tell it needs to create new
 * buffers because some component ran out.
 */

void critical_background_wake (void)
{
    process_set_boolean(critical_background_flag, TRUE);
}
