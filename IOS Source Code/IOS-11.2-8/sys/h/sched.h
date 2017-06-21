/* $Id: sched.h,v 3.7.12.6 1996/08/08 14:45:53 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/h/sched.h,v $
 *------------------------------------------------------------------
 * sched.h -- scheduler definitions
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sched.h,v $
 * Revision 3.7.12.6  1996/08/08  14:45:53  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Add a call to determine the depth of a watched queue.
 *
 * Revision 3.7.12.5  1996/08/03  23:22:34  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.7.12.4  1996/07/17  06:00:14  fox
 * CSCdi63199:  process_set_crashblock() ignores passed pid parameter
 * Branch: California_branch
 * Use 'pid' parameter instead of THIS_PROCESS to support calls by
 * process creators.
 *
 * Revision 3.7.12.3  1996/06/27  16:35:33  hampton
 * Add new scheduler accessor functions.  [CSCdi61518]
 * Branch: California_branch
 *
 * Revision 3.7.12.2  1996/05/24  19:29:37  hampton
 * Improve the granularity of low memory corruption checks during
 * subsystem initialization.  [CSCdi58674]
 * Branch: California_branch
 *
 * Revision 3.7.12.1  1996/03/18  19:37:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.4.3  1996/03/16  06:44:50  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.4.2  1996/03/09  05:05:06  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.5.4.1  1996/02/20  00:49:24  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/16  21:16:21  hampton
 * Need routine to check for saved scheduler events.  [CSCdi49223]
 *
 * Revision 3.6  1996/01/25  23:52:17  hampton
 * Add support to the scheduler for nesting wakeup event lists.  This
 * should make it easier to convert portion of the code.  Also allow for
 * the creation of a detached list of watched events.  [CSCdi46566]
 *
 * Revision 3.5  1996/01/16  01:16:06  hampton
 * Need support for requeueing packets on a watched queue.  [CSCdi46938]
 *
 * Revision 3.4  1995/12/22  21:58:26  hampton
 * Pass pointers to timestamps instead of passing the complete timestamp
 * structure.  [CSCdi45966]
 *
 * Revision 3.3  1995/11/18  02:17:32  pst
 * CSCdi43519: Add ability to public code to query scheduler for
 * suspendability.
 *
 * Revision 3.2  1995/11/17  09:13:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:36:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/11/08  20:59:01  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.9  1995/10/24  23:31:56  hampton
 * Remove the old inline s_kill_self_with_no_analysis by pushing it into
 * the routines that call it.  [CSCdi42780]
 *
 * Revision 2.8  1995/09/05  16:37:44  hampton
 * Fix autoinstall over frame relay.  [CSCdi38852]
 *
 * 1) Udp should only add one-shot queue watches inside of read_ipsocket.
 * 2) Allow a existing one-shot watch to be upgraded to a recurring watch,
 *    but prevent the downgrade of an existing recurring watch to a
 *    one-shot watch.
 * 3) Add some conditionally compiled debugging to the scheduler.
 *
 * Revision 2.7  1995/08/03 07:41:49  hampton
 * EIGRP stops processing input.  Convert the IP socket input queue into a
 * watched_queue.  This allows the scheduler to see if there is input
 * waiting when the process dismisses.  [CSCdi38156]
 *
 * Revision 2.6  1995/07/24 09:23:36  hampton
 * Requested scheduler additions.  [CSCdi37542]
 *
 * Revision 2.5  1995/07/07 05:26:04  hampton
 * Hide the definitions of watched data structures.  These data structures
 * should never be referenced directly.  [CSCdi36799]
 *
 * Revision 2.4  1995/06/20 06:39:48  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Invoke a "show mem fail alloc" when the parser can't get enough memory
 * to parse a command.
 *
 * Revision 2.3  1995/06/18  06:18:20  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.2  1995/06/09 13:28:45  hampton
 * Map the names of old scheduler calls into the new naming scheme.
 * There is no functionality change, as the old functions names were
 * only preprocessor definitions.  [CSCdi35514]
 *
 * The names changed were:
 *
 *  adisms           => process_sleep_until
 *  pdisms           => process_sleep_periodic
 *  tdisms           => process_sleep_for
 *  s_killp          => process_kill
 *  s_suspend        => process_suspend
 *  check_suspend    => process_may_suspend
 *  set_process_name => process_set_name
 *
 * Revision 2.1  1995/06/07 20:36:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SCHED_H__
#define __SCHED_H__

#define	HOTSWAP_SCHED_INTERVAL	1	/* 1 millisecond -- assures we get
					 * to hotswap process task ASAP.
					 * Note that the code relies on this
					 * value being illegal under normal
					 * circumstances.
					 */

#define SCHED_VOLUNTARY_TASK_TIME 200   /* How long to run before doing a */
					/*   voluntary suspend */

/*
 * Text base address line printed in "show version" and 
 * "show stacks" by print_text_base_string(), 
 * and saved with the ROM MON.
 */
#define TEXTBASE_STRING "\nImage text-base: 0x%08x, data-base: 0x%08x\n\n"

typedef enum ENABLESTATE_ {
    DISABLE = 0,
    ENABLE = 1
} ENABLESTATE;

typedef enum ONESHOT_ {
    RECURRING = 0,
    ONE_SHOT = 1
} ONESHOTTYPE;

typedef enum SYSTEM_MSGS_ {
    MSG_ROUTE_ADJUST = 1,
    MSG_SOCKET_ADJUST,
    MSG_FR_PVC_ACTIVE,
    MSG_FR_PVC_INACTIVE
} SYSTEM_MSGS;


/*********************************************************************
 *
 *		  Definitions for get_next_event().
 *
 * These are the major event types that will cause a process to be
 * awakened.  All minor codes are specific to a major code.  Minor type
 * codes are generally only used for IPC type events.  These minor type
 * codes may be either global to the system, or local to a specific
 * process or set of processes.  All type numbers in the range
 * [0x00000000-0x7FFFFFFF] are reserverd for system messages.  Numbers in
 * the range [0x80000000-0xFFFFFFFF] are left for use by cooperating
 * processes.
 *
 *********************************************************************/
typedef enum major_event_type_ {
    TIMER_EVENT		= 0x00000001,
    QUEUE_EVENT		= 0x00000002,
    BOOLEAN_EVENT	= 0x00000004,
    SEMAPHORE_EVENT	= 0x00000008,
    MESSAGE_EVENT	= 0x00000010,
    IPC_EVENT		= 0x00000020,
    TTY_EVENT		= 0x00000040,
    DIRECT_EVENT	= 0x00000080,
    BITFIELD_EVENT	= 0x00000100,
    ALL_EVENTS		= 0x0FFFFFFF,	/* so grovel compiles */
} major_event_type;

typedef enum minor_event_type_ {
    WB_SYSTEM_INIT,
    WB_SYSTEM_CONFIG,
    WB_SYSTEM_CRASH,
    WB_IP_CONFIG,
    WB_CRITICAL_BACKGROUND
} minor_event_type;


/********************************************************************
 *
 * 		Definitions for process_[gs]et_info().
 *
 * These routines are used to read or set selecteds process
 * attributes, and may only be accessed through wrapper routines.
 *
 *********************************************************************/

/*
 * The process attributes that can be read or set via these routines.
 */
enum process_info_types {
    PI_ANALYZE,
    PI_ARG,
    PI_CRASHBLOCK,
    PI_NAME,
    PI_PID,
    PI_PRIORITY,
    PI_PROFILE,
    PI_RUNTIME,
    PI_STACKSIZE,
    PI_STARTTIME,
    PI_TIMING_INFO,
    PI_TTYSOC,
    PI_TTYNUM,
    PI_WAKEUP_REASONS
};

/*
 * Process numbers
 */
#define THIS_PROCESS	-1	/* Arg to process related calls */
#define NO_PROCESS	-1	/* Process creation error return */

/*
 * Process priorities.
 */
typedef enum process_priority_t_ {
    PRIO_UNKNOWN = 0,     		/* priority not assigned */
    PRIO_CRITICAL = 1,		        /* critical priority */
    PRIO_HIGH     = 2,		        /* highest priority */
    PRIO_NORMAL   = 3,	        	/* interactive priority */
    PRIO_LOW      = 4		        /* lowest priority */
} process_priority_t;

/*
 * Per process debugging flags
 */
#define PROC_DEBUG_EXECUTION	0x01
#define PROC_DEBUG_WAKEUP	0x02
#define PROC_DEBUG_WAKEUP_DELAY	0x04
#define PROC_DEBUG_WATCH	0x08


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

STRUCTURE_REF(sched_event_set);

/*
 * Generic kernel stuff.  For those platforms that must get their
 * grody fingers into the main loop of the scheduler.
 */
typedef void (*kernel_sched_loop_hook_t)(boolean run_norm);
extern void kernel_set_sched_loop_hook (kernel_sched_loop_hook_t new_hook);

/*
 * os/sched.c
 */
extern char* last_sysversion;
extern boolean blocking_disabled;
extern int cpu_type;
extern long avg_busy1_percent;
extern long avg_busy5_percent;
extern long last_busy_percent;
extern uchar feature_set;
extern ulong cpu_family;
#ifndef GROVEL
extern boolean kernel_debugging;
extern volatile boolean schedflag;	/* See comments in sched.c. */
extern int sched_run_interval;
extern sys_timestamp sched_run_time;
#endif /*   GROVEL    */

extern boolean check_cpuhog(void);
extern boolean sched_test_page_zero(void);
extern pid_t process_create(process_t *, char *, stack_size_t, process_priority_t);
extern void print_text_base_string(void);
extern void process_kill(pid_t);
extern void process_hari_kari(void);
extern void process_count_malloc(ulong);
extern void process_count_free(ulong);
extern void process_count_getbuffer(ulong);
extern void process_count_retbuffer(ulong);
extern void wastetime(ulong);

typedef enum {
    RELOAD_NORMAL = 1,    /* do notification, registry calls, etc. */
    RELOAD_QUICK,
} reload_type_t;
extern void reload_system(reload_type_t);

/*
 * os/process.c
 */
extern boolean process_enqueue(watched_queue *queue, void *whatever);
extern boolean process_exists(pid_t pid);
extern boolean process_caller_has_events(void);
extern boolean process_get_boolean(watched_boolean *bool);
extern boolean process_get_info(int type, pid_t pid, ...);
extern boolean process_get_message(ulong *, void **, ulong *);
extern boolean process_get_wakeup(ulong *major, ulong *minor);
extern boolean process_is_queue_empty(watched_queue *queue);
extern boolean process_is_queue_full(watched_queue *queue);
extern boolean process_lock_semaphore(watched_semaphore *sem, ulong timeout);
extern boolean process_may_suspend(void);
extern boolean process_pop_event_list(sched_event_set **);
extern boolean process_push_event_list(sched_event_set *);
extern boolean process_requeue(watched_queue *, void *);
extern boolean process_suspends_allowed(void);
extern boolean process_send_message(pid_t, ulong, void *, ulong);
extern boolean process_set_info(int type, pid_t pid, ...);
extern boolean process_would_suspend(void);
extern int process_queue_max_size(watched_queue *queue);
extern int process_queue_size(watched_queue *queue);
extern ulong process_get_bitfield(watched_bitfield *wb);
extern void *process_dequeue(watched_queue *item);
extern void process_enqueue_pak(watched_queue *queue, paktype *pak);
extern void delete_watched_bitfield(watched_bitfield **);
extern void delete_watched_boolean(watched_boolean **);
extern void delete_watched_queue(watched_queue **);
extern void delete_watched_semaphore(watched_semaphore **);
extern void process_clear_bitfield(watched_bitfield *cell, ulong value, boolean send_wakeup);
extern void process_keep_bitfield(watched_bitfield *cell, ulong value, boolean send_wakeup);
extern void *process_queue_peek(watched_queue *);
extern void process_requeue_pak(watched_queue *, paktype *);
extern void process_resize_queue(watched_queue *, int, queuetype *);
extern void process_set_all_profiles(boolean profile_me);
extern void process_set_bitfield(watched_bitfield *cell, ulong value, boolean send_wakeup);
extern void process_set_boolean(watched_boolean *cell, boolean value);
extern void process_set_debug(pid_t pid, uchar flags);
extern void process_sleep_for(ulong);
extern void process_sleep_on_timer(mgd_timer *);
extern void process_sleep_periodic(ulong);
extern void process_sleep_until(sys_timestamp *);
extern void process_suspend(void);
extern void process_unlock_semaphore(watched_semaphore *sem);
extern void process_wait_for_event(void);
extern void process_wait_for_event_timed(ulong);
extern void process_wait_on_system_config(void);
extern void process_wait_on_system_init(void);
extern void process_wakeup(pid_t);
extern void process_wakeup_w_reason(pid_t, ulong, ulong);
extern void process_watch_bitfield(watched_bitfield *, ENABLESTATE, ONESHOTTYPE);
extern void process_watch_boolean(watched_boolean *, ENABLESTATE, ONESHOTTYPE);
extern void process_watch_mgd_timer(mgd_timer *, ENABLESTATE);
extern void process_watch_queue(watched_queue *, ENABLESTATE, ONESHOTTYPE);
extern void process_watch_semaphore(watched_semaphore *, ENABLESTATE, ONESHOTTYPE);
extern void process_watch_timer(sys_timestamp *, ENABLESTATE);
extern watched_bitfield *create_watched_bitfield(char *name, ulong id);
extern watched_boolean *create_watched_boolean(char *name, ulong id);
extern watched_queue *create_watched_queue(char *name, int maximum, ulong id);
extern watched_semaphore *create_watched_semaphore(char *name, ulong id);
extern void process_set_bitfield_minor(watched_bitfield *event, ulong minor);
extern void process_set_boolean_minor(watched_boolean *event, ulong minor);
extern void process_set_semaphore_minor(watched_semaphore *event, ulong minor);
extern void process_set_queue_minor(watched_queue *event, ulong minor);
extern void process_set_bitfield_first_only(watched_bitfield *event, boolean first_only);
extern void process_set_boolean_first_only(watched_boolean *event, boolean first_only);
extern void process_set_semaphore_first_only(watched_semaphore *event, boolean first_only);
extern void process_set_queue_first_only(watched_queue *event, boolean first_only);

/*
 * sched_chain.c
 */
extern void show_proc_memory(parseinfo *csb);

/*
 * sched_compatibility.c
 */
#define SUPPORT_OLD_SCHEDULER
#ifdef SUPPORT_OLD_SCHEDULER
extern boolean process_is_ok(int pid);
extern ulong runtime(void);
extern void edisms(blockproc *, ulong);
extern int cfork(forkproc *, long, int, char *, int);
#endif


/***********************************************************************
 *
 *			       Inlines
 *
 ***********************************************************************/

/**************************************************
 *
 * Inlines for toggling the blocking_disabled flag
 * both before and after calling a scheduler
 * primitive.
 *
 *************************************************/

static inline void process_safe_suspend (void)
{
    blocking_disabled = FALSE;	/* O.K. for process to block */
    process_suspend();
    blocking_disabled = TRUE;	/* blocking is unexpected now */
}

static inline void process_safe_wait_for_event (void)
{
    blocking_disabled = FALSE;
    process_wait_for_event();
    blocking_disabled = TRUE;
}

static inline void process_safe_sleep_for (ulong delta)
{
    blocking_disabled = FALSE;
    process_sleep_for(delta);
    blocking_disabled = TRUE;
}

static inline void process_safe_sleep_until (sys_timestamp *when)
{
    blocking_disabled = FALSE;
    process_sleep_until(when);
    blocking_disabled = TRUE;
}

static inline void process_safe_sleep_on_timer (mgd_timer *timer)
{
    blocking_disabled = FALSE;
    process_sleep_on_timer(timer);
    blocking_disabled = TRUE;
}

static inline void process_safe_sleep_periodic (ulong delta)
{
    blocking_disabled = FALSE;
    process_sleep_periodic(delta);
    blocking_disabled = TRUE;
}


/**************************************************
 *
 * Inlines for getting and/or setting process
 * attributes.  *Always* use these inlines and
 * never call process_[gs]et_info directly.  The
 * inlines are there to enforce checking on the
 * number and type of the arguments.  Bypassing
 * them is just begging for problems.
 *
 **************************************************/

static inline boolean process_get_analyze (boolean *value)
{
    return(process_get_info(PI_PROFILE, THIS_PROCESS, value));
}

static inline boolean process_get_arg_num (ulong *arg)
{
    return(process_get_info(PI_ARG, THIS_PROCESS, arg));
}

static inline boolean process_get_arg_ptr (void **arg)
{
    return(process_get_info(PI_ARG, THIS_PROCESS, arg));
}

static inline boolean process_get_crashblock (boolean *value)
{
    return(process_get_info(PI_PROFILE, THIS_PROCESS, value));
}

static inline boolean process_get_name (pid_t pid, const char **name)
{
    return(process_get_info(PI_NAME, pid, name));
}

static inline boolean process_get_pid (pid_t *pid)
{
    return(process_get_info(PI_PID, THIS_PROCESS, pid));
}

static inline boolean process_get_priority (ulong *value)
{
    return(process_get_info(PI_PRIORITY, THIS_PROCESS, value));
}

static inline boolean process_get_profile (boolean *value)
{
    return(process_get_info(PI_PROFILE, THIS_PROCESS, value));
}

static inline boolean process_get_runtime (ulong *value)
{
    return(process_get_info(PI_RUNTIME, THIS_PROCESS, value));
}

static inline boolean process_get_stacksize (pid_t pid, ulong *stacksize)
{
    return(process_get_info(PI_STACKSIZE, pid, stacksize));
}

static inline boolean process_get_starttime (pid_t pid, sys_timestamp *time)
{
    return(process_get_info(PI_STARTTIME, pid, time));
}

static inline boolean process_get_ttynum (pid_t pid, ulong *num_ptr)
{
    return(process_get_info(PI_TTYNUM, pid, num_ptr));
}

static inline boolean process_get_ttysoc (pid_t pid, tt_soc **ttsoc_ptr)
{
    return(process_get_info(PI_TTYSOC, pid, ttsoc_ptr));
}

static inline boolean process_get_wakeup_reasons (ulong *value)
{
    return(process_get_info(PI_WAKEUP_REASONS, THIS_PROCESS, value));
}

static inline boolean process_is_high_priority (void)
{
    int priority;

    if (process_get_info(PI_PRIORITY, THIS_PROCESS, &priority))
	return(priority == PRIO_HIGH);
    return(FALSE);
}


/* - */

static inline boolean process_set_analyze (boolean do_analysis)
{
    return(process_set_info(PI_ANALYZE, THIS_PROCESS, do_analysis));
}

static inline boolean process_set_arg_num (pid_t pid, ulong arg)
{
    return(process_set_info(PI_ARG, pid, arg));
}

static inline boolean process_set_arg_ptr (pid_t pid, void *arg)
{
    return(process_set_info(PI_ARG, pid, arg));
}

static inline boolean process_set_crashblock (
    pid_t pid,
    boolean stop_when_crashing)
{
    return(process_set_info(PI_CRASHBLOCK, pid, stop_when_crashing));
}

static inline boolean process_set_name (char *name)
{
    return(process_set_info(PI_NAME, THIS_PROCESS, name));
}

static inline boolean process_set_priority (int priority)
{
    return(process_set_info(PI_PRIORITY, THIS_PROCESS, priority));
}

static inline boolean process_set_profile (pid_t pid, boolean enabled)
{
    return(process_set_info(PI_PROFILE, pid, enabled));
}

static inline boolean process_set_timing (pid_t pid, ulong elapsed, ulong calls)
{
    return(process_set_info(PI_TIMING_INFO, pid, elapsed, calls));
}

static inline boolean process_set_ttynum (pid_t pid, int ttynum)
{
    return(process_set_info(PI_TTYNUM, pid, ttynum));
}

static inline boolean process_set_ttysoc (pid_t pid, tt_soc *tty)
{
    return(process_set_info(PI_TTYSOC, pid, tty));
}

static inline boolean process_set_wakeup_reasons (
    ulong reasons,
    ONESHOTTYPE one_shot)
{
    return(process_set_info(PI_WAKEUP_REASONS, THIS_PROCESS, reasons, one_shot));
}

/**************************************************
 *
 * Miscellaneous Inlines
 *
 **************************************************/

/*
 * debugger_active
 *
 * Returns TRUE if the debugger is running.
 */
static inline boolean debugger_active(void)
{
    return(kernel_debugging);
}

/*
 * process_time_exceeded
 *
 * Has the currently running process used up its allocated portion of
 * CPU time?  If so, it must now suspend.
 *
 * Note: Do not ever reference the process_quantum_expired global
 * directly.  It may not always be present, especially once IOS begins to
 * be ported to other platforms.
 */
static inline boolean process_time_exceeded (void)
{
    extern boolean process_quantum_expired;

    return(process_quantum_expired);
}

/*
 * process_suspend_if_req
 *
 * Has the currently running process used up its allocated portion of
 * CPU time?  If so, it must now suspend.
 */
static inline boolean process_suspend_if_req (void)
{
    if (process_time_exceeded()) {
	process_suspend();
	return(TRUE);
    }
    return(FALSE);
}

#endif		/* __SCHED_H__ */
