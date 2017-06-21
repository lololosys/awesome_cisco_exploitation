/* $Id: sched_private.h,v 3.8.12.7 1996/08/03 23:35:05 fox Exp $
 * $Source: /release/112/cvs/Xsys/os/sched_private.h,v $
 *------------------------------------------------------------------
 * sched_private.h -- scheduler definitions
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sched_private.h,v $
 * Revision 3.8.12.7  1996/08/03  23:35:05  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.8.12.6  1996/08/03  08:25:35  hampton
 * Tweak thrashing check for watched queues.  [CSCdi63793]
 * Branch: California_branch
 *
 * Revision 3.8.12.5  1996/05/08  07:33:33  hampton
 * Add comments describing how the scheduler uses of managed timers, and
 * which timers are affected by which dismiss routines.  [CSCdi56999]
 * Branch: California_branch
 *
 * Revision 3.8.12.4  1996/05/08  00:07:00  hampton
 * Add better semaphore errors and better code to wait for a semaphore.
 * The routine process_wait_for_event_timed() shouldn't post a TIMER_EVENT
 * when the temporary timer expires.  [CSCdi56999]
 * Branch: California_branch
 *
 * Revision 3.8.12.3  1996/04/13  06:05:38  dkatz
 * CSCdi53190:  Init time memory charged to *Dead* process
 * Branch: California_branch
 * Ensure that memory allocated by the ephemeral Init process is charged
 * to *Init*.  Copy the allocator PC from the allocated block into the
 * fragment when fragmenting memory.  Add an option to "show memory" to
 * display the allocating process with each block.
 *
 * Revision 3.8.12.2  1996/03/27  21:52:30  hampton
 * Try to catch and report thrashing processes.  A few other minor
 * scheduler cleanups and enhancements.  [CSCdi50130]
 * Branch: California_branch
 *
 * Revision 3.8.12.1  1996/03/18  21:31:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.22.3  1996/03/09  05:10:27  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.5.22.2  1996/03/07  10:19:50  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.22.1  1996/02/20  16:41:41  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/13  20:25:05  hampton
 * Move platform specific defines from common include files to platform
 * specific include files.  [CSCdi48844]
 *
 * Revision 3.7  1996/01/25  23:52:40  hampton
 * Add support to the scheduler for nesting wakeup event lists.  This
 * should make it easier to convert portion of the code.  Also allow for
 * the creation of a detached list of watched events.  [CSCdi46566]
 *
 * Revision 3.6  1996/01/25  18:38:12  hampton
 * Fix per-process heap checking code.  Also convert it from conditional
 * compilation to an "internal" command.  [CSCdi47517]
 *
 * Revision 3.5  1995/11/28  02:19:57  henniger
 * CSCdi38897:  Race condition in CPU HOG incorrectly flags non-hogging
 * process
 * in process_execute() assign last_int_count earlier.
 *
 * Revision 3.4  1995/11/18  06:31:42  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Setup a tt_soc with null input and output vectors.  Provide routine
 * which disables io for calling process by changing controlling tty to
 * the null_io_tty and returns previous controlling tty.  Provide routine
 * which restores io by setting the controlling tty to the tty passed in.
 *
 * Revision 3.3  1995/11/18  06:06:21  hampton
 * Don't let the entire router be dragged down by a single process with a
 * busted timer.  [CSCdi44073]
 *
 * Revision 3.2  1995/11/17  18:51:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:48:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/09/25  08:37:15  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.6  1995/09/08  16:16:58  hampton
 * Don't spin the random number generator as frequently from the
 * scheduler.  Fix test in signal_receive.  [CSCdi39965]
 *
 * Revision 2.5  1995/09/05 16:38:07  hampton
 * Fix autoinstall over frame relay.  [CSCdi38852]
 *
 * 1) Udp should only add one-shot queue watches inside of read_ipsocket.
 * 2) Allow a existing one-shot watch to be upgraded to a recurring watch,
 *    but prevent the downgrade of an existing recurring watch to a
 *    one-shot watch.
 * 3) Add some conditionally compiled debugging to the scheduler.
 *
 * Revision 2.4  1995/07/24 09:24:13  hampton
 * Requested scheduler additions.  [CSCdi37542]
 *
 * Revision 2.3  1995/07/22 07:06:12  hampton
 * Check each event once per scheduler invocation, not once per call to
 * process_get_event.  This prevents a high input arrival rate from
 * causing a process to hog the processor.  [CSCdi37126]
 *
 * Revision 2.2  1995/07/07 05:26:47  hampton
 * Hide the definitions of watched data structures.  These data structures
 * should never be referenced directly.  [CSCdi36799]
 *
 * Revision 2.1  1995/06/07 21:58:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SCHED_PRIVATE_H__
#define __SCHED_PRIVATE_H__

#define BACKWARD_COMPATIBLE		/* Support old scheduler model. */

#define RANDOMIZE_FREQUENCY	100	/* spin random after this many passes */

#define MIN_SCHED_INTERVAL	100	/* 100 milliseconds -- .1 sec */

#define WATCHDOG_TIME_SHIFT	6	/* If the max sched task time shifted */
					/* by this value is exceeded, invoke  */
					/* the watchdog.  The default action  */
					/* is to crash and the time is ~2 min */

#define WATCHDOG_FATAL_TIME (12*ONEHOUR)/* If the watchdog is invoked twice   */
					/* on the same process in this time,  */
					/* it is sumarily killed.             */

#define	PROC_MAGIC	0xbeefcafe

#define LOAD_SCALAR 1024		/* Load percentages scale factor */

#define MESSAGES_PER_CHUNK 50

/*
 * If a process calls sched_wait_for_event() this many times without any
 * intervening event occurring, something is broken in the process.  The
 * most likely cause is that the process missed an event, and the
 * scheduler keeps trying to get the process to consume it.  This may
 * also happen is the process somehow gets an event that it does not
 * recognize, and the process hasn't been coded to handle unexpected
 * events.
 */
#define SCHED_THRASH_THRESHHOLD 50

/*
 * The enum for  current state of a process.  This must match the
 * character display codes in sched_compatibility.c.
 */
typedef enum sched_state_ {
    SCHEDSTATE_RUNNING = 0,	/* The currently running process */
    SCHEDSTATE_EDISMS,		/* Arbitrary code fragment - OLD */
    SCHEDSTATE_SUSPEND,		/* Temporary Suspension - OLD */
    SCHEDSTATE_READY,		/* Ready to Execute */
    SCHEDSTATE_IDLE,		/* Waiting for some event (incl timer) */
    SCHEDSTATE_SLEEP_ABS,	/* Sleeping until some absolute time */
    SCHEDSTATE_SLEEP_INTERVAL,	/* Sleeping for some interval of time */
    SCHEDSTATE_SLEEP_PERIODIC,	/* Sleeping for some periodic time interval
    				   This starts from the time of the last
    				   wakeup, not the time of the sleep call.
    				   It helps prevent clock skew. */
    SCHEDSTATE_SLEEP_TIMER,	/* Sleeping until a managed timer expires */
    SCHEDSTATE_HUNG,		/* Hung - will never run again */
    SCHEDSTATE_DEAD		/* Dead, but not yet buried */
} sched_state;



/*
 * The schedulers internal definition of a message.  This is how a message and its
 * arguments are maintained internally from the time the sender transmits the
 * message to the time the receiver reads the message.  This data structure is
 * never revealed outside of the scheduler.  This is essentially a very quick and
 * dirty IPC mechanism that only functions on a single processor.
 */

STRUCTURE_REF(message_type);
struct message_type_ {
    message_type *msg_next;
    ulong         msg_id;
    void         *msg_ptr_arg;
    ulong         msg_num_arg;
};

/*
 * Scheduler timers.
 *
 * The scheduler maintains a small managed timer tree for each process.
 * Here is a picture showing how the overall timers are arranged:
 *
 *                          sched_master_timer
 *                        ___/ 	 |          \___
 *                    ___/     	 |              \___
 *                   /         	 |                  \
 *  sched_force_int_excl-F   proc1->sched_timer ... procn->sched_timer
 *
 *
 * And a picture showing how the individual per-process are arranged:
 *
 *
 *                       procn->sched_timer
 *                      ___/    |      \___
 *                  ___/        |          \___
 *                 /            |              \
 *  sched_timer_temp-F  sched_timer_simple-F  sched_timer_managed-F
 *                                               |
 *                                              ---
 *                                               |
 *                      sched_linked_mtimer--->(xxx)
 *
 * The variable names used here are the actual names, the "F" stands for
 * a "fenced" timer, and the "---" indicates a parent-child relationship
 * that is dynamically created with a call to mgd_timer_link().  The
 * timer "xxx" may be either the master timer for a process or some other
 * generic managed timer passed to the scheduler.
 *
 * There are two other per-process variables that are not part of the
 * managed timer tree itself, but are related to the scheduler timer
 * support.  These are watched_mgd_timer and watched_simple_timer.  They
 * are used to save pointers the timers specified in calls to
 * process_watch_timer() and process_watch_mgd_timer().  The timer
 * sched_force_int_excl is a dummy timer used to insure that the
 * scheduler timers are always marked to me modified with interrupts
 * disabled.
 *
 * When a process calls process_wait_for_event(), the following happens.
 *
 *   1) If there is a managed timer being watched (i.e. watched_mgd_timer
 *      is non-NULL), the pointer is copied into sched_linked_mtimer and
 *      the timer is linked into the timer tree for the process.
 *
 *   2) If there is a simple timer being watched (i.e.
 *      watched_simple_timer is non-NULL), its expiration time is used to
 *      start the timer sched_timer_simple.
 *
 *   3) If the variant routine process_wait_for_event_timed() was called,
 *      the timer sched_timer_temp is started with the delay specified.
 *
 * When a process calls process_sleep_on_timer(), the following happens.
 *
 *   -) The pointer specified in the call is copied into the variable
 *      sched_linked_mtimer,  and the timer is linked into the timer tree
 *      for the process.
 *
 * When a process calls process_sleep_until(), process_sleep_for(), or
 * process_sleep_periodic(), the following happens:
 *
 *   -) The expiration time or delay specified in the call is used to set
 *      the variable sched_timer_temp.
 *
 * The following enum is used to indicate which of the fenced
 * scheduler timers has expired so that the scheduler can take the
 * appropriate action.
 */

enum sched_timer_types_ {
    STT_PROCESS_MANAGED,/* This is a per-process managed timer. */
    STT_PROCESS_SIMPLE,	/* This is a per-process simple timer. */
    STT_PROCESS_TEMP,	/* This is a per-process temporary timer. */
    STT_FORCE		/* This is the special timer used by the scheduler to
			   insure that the its timer tree is always procected
			   from interrupts. */
};

/*
 * The managed timers code must always use interrupt exclusion when
 * dealing with scheduler timers.  This is forced by having a dummy
 * timers that is always running, and are marked as requiring interrupt
 * exclusion.  Whenever this timer expires, it is reset to some arbitrarily
 * long time.
 */
#define ST_FORCE_TIME 0x40000000

/*********************************************************************
 *
 *		      "WATCHED" DATA STRUCTURES
 *
 *********************************************************************/

/*
 * Note--these fields should remain opaque!  Do not refer to them directly!
 * Use the routines exported in sched.h!
 *
 * The common data structure that is the basis for all watched items.
 * The advantage of watched items is that they eliminate the need for a
 * processes to continually poll the data structure to see if the state
 * has changes or if there is data present.  The onus is placed onto the
 * process changing or queueueing data.  This is the proactive way of
 * doing things, instead of the passive background polling of the current
 * system.
 */
typedef struct watched_item_ {
    list_element by_class;		/* link all of same type */
    list_header wakeup_list;		/* who wants wakeup notification */
    boolean wake_first_only;		/* wake only first or all procs */
    char *name;
    ulong major_type;
    ulong minor_type;
} watched_item;

/*
 * Watched_boolean data structure
 *
 * This is a watched booolean, where any watcher will be notified when
 * the boolean is set to the logical value of TRUE.  You may use the
 * process_get_boolean routine to read the boolean value, should you need
 * to for some reason.
 */
struct watched_boolean_ {
    watched_item wi;
    boolean value;
};

static inline boolean wb_value_inline (watched_boolean *wb)
{
    return(wb->value);
}
#define WB_VALUE(wb)	wb_value_inline(wb)


/*
 * Watched_bitfield data structure
 *
 * This is a watched bitfield, where any watcher will be notified when
 * any of the bits are changed.  You may use the process_get_bitfield
 * routine to read the bitfield.
 */
struct watched_bitfield_ {
    watched_item wi;
    ulong bits;
};

/*
 * Watched_queue data structure
 *
 * This is a watched queue, where any watchers will be notified when
 * new data is enqueued.
 */

struct watched_queue_ {
    watched_item wi;
    queuetype q;
    int old_size;
};

static inline queuetype *wq_queue_inline (watched_queue *wq)
{
    return(&wq->q);
}
static inline int wq_queuesize_inline (watched_queue *wq)
{
    return(wq->q.count);
}
static inline boolean wq_queuefull_inline (watched_queue *wq)
{
    return((wq->q.maximum != 0) && (wq->q.count >= wq->q.maximum));
}
static inline boolean wq_queuefull_reserve_inline (watched_queue *wq, int reserve)
{
    return((wq->q.maximum != 0) && ((wq->q.count + reserve) >= wq->q.maximum));
}
static inline boolean wq_queueempty_inline (watched_queue *wq)
{
    return(wq->q.qhead == NULL);
}
#define WQ_QUEUE(wq)		wq_queue_inline(wq)
#define WQ_QUEUESIZE(wq)	wq_queuesize_inline(wq)
#define	WQ_QUEUEFULL(wq)	wq_queuefull_inline(wq)
#define	WQ_QUEUEFULL_RESERVE(wq, reserve) \
				wq_queuefull_reserve_inline(wq, reserve)
#define WQ_QUEUEEMPTY(wq)	wq_queueempty_inline(wq)

/*
 * Watched_semaphore data structure
 *
 * This is a watched semaphore, where any watchers will be notified
 * when the semaphore is released.
 */

struct watched_semaphore_ {
    watched_item wi;
    semaphore s;
    pid_t owner;
};

/*
 * The scheduler's internal data structure for remembering which process
 * is monitoring what event.  It is doubly threaded so the scheduler can
 * easily walk through all processes that are waiting for a given event,
 * or it can walk through all events that a given process is watching.
 */
typedef struct watcher_info_ {
    list_element  wi_by_event;		/* The event remains the same */
    list_element  wi_by_process;	/* The process remains the same */
    sprocess     *wi_process;		/* Which process */
    watched_item *wi_event;		/* Which event */
    boolean       wi_occurred;		/* Did the event happen yet? */
    boolean       wi_one_shot;		/* Recurring wakeup vs one time */
    boolean       wi_shot;		/* Did the one time happen yet? */
} watcher_info;


typedef void (*new_exec_point_t)(void);

struct sched_event_set_ {
    sched_event_set *next;
    ulong	     magic;
    list_header      process_events;	   /* Events this process is watching */
    list_element    *next_event;	   /* Next event to process */
    ulong            event_count;	   /* Events processed this invocation */
    mgd_timer       *watched_mgd_timer;    /* remember mgd_timer to watch */
    sys_timestamp   *watched_simple_timer; /* remember timestamp to watch */
    watcher_info     watch_timers;	   /* Linked in to catch timers */
};

#define SCHED_EVENT_MAGIC 0xdeadfeed

/*
 * Note: Do not move stack, savedsp, regs, or ps without changing the
 * corresponding code in resume() and suspend().
 */
struct sprocess_ {
	ulong *stack;			/* process stack */
	ulong *savedsp;			/* old stack pointer */
	ulong regs[PROCESSOR_NUM_REGS]; /* for saved registers */
	PROCESSOR_STATUS_TYPE ps;	/* saved processor status */
	uchar padding[PROCESSOR_STATUS_PAD];
	/* Offsets 68k:52, 4K:68, sparc:108 */
	ulong original_ra;		/* Where to return after handler */
	ulong routine_to_execute;	/* Handler address */
	ulong pending_signal;		/* Currently pending signal */
	/*
	 * Do not move anything above this line.  There is assembly
	 * code that 'knows' the offsets of these fields.
	 */

	tinybool profile;		/* This task is profiled */
	tinybool analyze;		/* Perform postmortem analysis */
	tinybool crashblock;		/* Block process when router crashes */
	tinybool crashed;		/* Process has crashed */
	tinybool killed;		/* Process has been killed */
	tinybool corrupt;		/* TRUE if process was corrupted */
	tinybool prefers_new;		/* Use new queues */
	tinybool on_old_queue;		/* Currently on old queue */
	tinybool wakeup_posted;		/* protect against losing events */
	tinybool profile_me;		/* TRUE if this task is profiled */
	tinybool process_arg_valid;	/* Was process_arg explicitly set */
	tinybool init_process;		/* TRUE if this is the Init process */
	uchar debug_events;		/* for debugging scheduler */
	sched_state state;		/* type of scheduler test */
	int pid;			/* process identification */
	ulong caller_pc;   		/* calling PC for "show process" */
	ulong callee_pc;   		/* called PC for 4500 trace_caller() */
	ulong totmalloc;		/* total malloced */
	ulong totfree;			/* total freed */
	ulong totgetbufmem;		/* total memory malloced for pools */
	ulong totretbufmem;		/* total freed from pools */
	sys_timestamp prev_timer;	/* used by process_sleep_periodic */
	blockproc *edisms;	        /* != 0 if address of blocking test */
	void *eparm;			/* edisms parameter */
	ulong elapsedtime;		/* time / call */
	volatile sys_timestamp starttime; /* for elapsed time calc. */
	long ncalls;			/* number of invocations */
	long ngiveups;			/* number of voluntary suspends */
	char *name;			/* name of process */
	int priority;			/* run queue number */
	long cpu_ticks_5sec;		/* CPU ticks so far in this 5 secs */
	long pct_cpu_5sec;		/* CPU % * LOAD_SCALAR, last 5 sec */
	long pct_cpu_1min;		/* CPU % * LOAD_SCALAR, last 1 min */
	long pct_cpu_5min;		/* CPU % * LOAD_SCALAR, last 5 min */
	ulong stacksize;		/* initial stack size in bytes */
	int lowstack;			/* low water mark for stack size */
	tt_soc *ttyptr;			/* pointer to terminal data block */
	debug_context *debugger;	/* Pointer to debugger context */
	signal_table *signal;		/* Pointer to signal table, if any */
	sys_timestamp last_watchdog;	/* time of last watchdog */
	ulong mem_holding;		/* Used by memory tally routine */

	void *process_arg;		/* The argument to the process */
	list_element sched_list;

        /* Timers used while the process is sleeping or idle. */
	mgd_timer sched_timer;		/* The master timer for the process */
	MGD_TIMER_EXTENDED(sched_timer_managed, 1);
					/* Sub-timer for linked mgd timer */
	mgd_timer sched_timer_simple;	/* Sub-timer for timestamp expiration */
	mgd_timer sched_timer_temp;	/* Sub-timer for delta times */
	mgd_timer *sched_linked_mtimer; /* Ptr to the currently linked timer.
					   Set by process_sleep_on_timer() or
					   process_wait_for_event(). */

	queuetype process_event_setQ;	/* saved process_events lists */
	sched_event_set *current_events;

	queuetype message_queue;	/* Messages posted for process */
	watcher_info watch_messages;	/* Linked in to catch messages */
	ulong wakeup_reasons;		/* What can wakeup process */
	ulong default_wakeup_reasons;	/* What can wakeup process */
	ulong direct_wakeup_major;	/* Reason for explicit wakeup */
	ulong direct_wakeup_minor;	/* Reason for explicit wakeup */
	sys_timestamp exp_timer_time;	/* Check for broken process */
	sys_timestamp exp_mgdtimer_time;/* Check for broken process */
	ulong thrash_count;		/* Catch a broken process */
        void *reent_block;		/* library reentrancy block */

	long magic;			/* check for trashed stack */
};

#define MAX_PROC_PER_ARRAY 256
STRUCTURE_REF(sprocess_list);
struct sprocess_list_ {
    sprocess_list *sl_next;		/* next array of processes */
    int            sl_count;		/* slots in use in this array */
    sprocess      *sl_process[MAX_PROC_PER_ARRAY];
};


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

/*
 * For computing loads
 */
extern ulong idle_count;
extern volatile ulong int_count;
extern boolean idle_count_wired;
extern long last_busy_percent_scaled;	/* busy time % * LOAD_SCALAR, last 5 secs */
extern long last_busy_percent;		/* above, but a true %age for SNMP */
extern long last_int_percent_scaled;	/* intrpt time % * LOAD_SCALAR, last 5 secs */
extern long avg_busy1_percent_scaled;	/* 1 min avg. busy time % * LOAD_SCALAR */
extern long avg_busy1_percent;		/* above, but a true %age for SNMP */
extern long avg_busy5_percent_scaled;	/* 5 min avg. busy time % * LOAD_SCALAR */
extern long avg_busy5_percent;		/* above, but a true %age for SNMP */

/*
 * Memory counting.
 */
extern ulong init_totmalloc;	/* How much memory malloced from *init* */
extern ulong init_totfree;	/* How much memory freed from *init* */
extern ulong init_totgetbufmem;	/* How much memory malloced from *init* */
extern ulong init_totretbufmem;	/* How much memory freed from *init* */
extern ulong sched_totmalloc;	/* How much memory malloced from *sched* */
extern ulong sched_totfree;	/* How much memory freed from *sched* */
extern ulong sched_totgetbufmem;/* How much memory malloced from *sched* */
extern ulong sched_totretbufmem;/* How much memory freed from *sched* */
extern ulong dead_totmalloc;	/* How much memory dead process malloced */
extern ulong dead_totfree;	/* How much memory dead processed freed */
extern ulong dead_totgetbufmem;	/* Buffers allocated by all dead procs */
extern ulong dead_totretbufmem;	/* Buffers returned by all dead procs */

/*
 * Externs from process.c instead of gross externs in sched.c
 */
extern watched_boolean *watched_system_init;
extern watched_boolean *watched_system_config;
extern watched_boolean *critical_background_flag;

/*
 * Externs from sched.c instead of gross externs in init.c and process.c
 */
extern watched_item sched_dummy_timer;
extern boolean sysinit_process_started;

/*
 * Process queues
 */
extern list_header procq_ready_c;	/* ready to run - Critical */
extern list_header procq_ready_h;	/* ready to run - High */
extern list_header procq_ready_m;	/* ready to run - Normal */
extern list_header procq_ready_l;	/* ready to run - Low */
extern list_header procq_idle;		/* Waiting processes */
extern list_header procq_dead;		/* Dead Processes */

/*
 * Process queues (Compatibility code)
 */
#ifdef BACKWARD_COMPATIBLE
extern list_header procq_runnorm;	/* Standard Priority process list */
extern list_header procq_runlow;	/* Low Priority process list */
#endif BACKWARD_COMPATIBLE

/*
 * Events queues
 */
extern list_header s_notify_queue;	/* Notify these processes when enqueueing */
extern list_header s_notify_boolean;	/* Notify these processes when val changes */
extern list_header s_notify_bitfield;	/* Notify these processes when bit changes */
extern list_header s_notify_semaphore;	/* Notify these processes when freed */

/*
 * Extra memory checking
 */
extern boolean checkheaps_per_proc;	/* check heaps after each process executes */
extern boolean checkheaps_per_poll;	/* check heaps each edisms test routine */


#ifndef GROVEL
extern queuetype sarrayQ;
extern enum sched_watchdog_options sched_watchdog_action;
extern int refresh_time;
extern int sched_max_task_time;
extern ulong watchdog_time;
extern sprocess *forkx, *forkx_2, *forkx_3;
extern char *last_proc_name;
extern sprocess *process_pid2ptr(int);
extern void scheduler(void);
#endif         /*   GROVEL    */

extern uchar sched_debug_events;
extern boolean process_quantum_expired;
extern boolean sched_pagezero_enabled;
extern boolean sched_run_degraded;
extern boolean process_ok_to_reschedule(char *);
extern boolean sched_init(boolean);
extern char *event_type2text(ulong);
extern void process_run_add(sprocess *p);
extern void process_wakeup_internal(sprocess *proc, ulong major, ulong minor);
extern void process_init(void);
extern void process_hang(void);
extern void sched_mark_router_crashing(void);
extern void process_last_gasp(void);
extern void process_handle_watchdog(boolean);
extern void process_initialize_messages(void);
extern pid_t process_create_common (process_t (*padd), long pp, int stacksize,
				    char *name, int priority, int prefers_new);
extern void watcher_initialize_common(list_header *, watched_item *, char *,
				      major_event_type, ulong);
extern boolean process_list_wakeups(list_header *, list_element *, char *);
extern void sched_save_page_zero(void);
extern void sched_restore_page_zero(void);
extern sched_event_set *process_malloc_event_set(void);

/*
 * Compatability routines.
 */
extern void old_run_add(sprocess*);

/*
 * Assembler support routines
 */
extern void resume(void);
extern void suspend(void);
extern void return_from_exception(void);
extern void alter_suspended_process(sprocess *, new_exec_point_t);


#endif		/* __SCHED_PRIVATE_H__ */
