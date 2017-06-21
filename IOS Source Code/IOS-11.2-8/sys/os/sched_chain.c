/* $Id: sched_chain.c,v 3.10.10.6 1996/09/11 17:39:15 snyder Exp $
 * $Source: /release/112/cvs/Xsys/os/sched_chain.c,v $
 *------------------------------------------------------------------
 * sched_chain.c - Parse chains and code for scheduler commands
 *
 * March 1995, Scott Mackie
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sched_chain.c,v $
 * Revision 3.10.10.6  1996/09/11  17:39:15  snyder
 * CSCdi68797:  char pointers to char arrays and make const
 *              120 data, 152 image
 * Branch: California_branch
 *
 * Revision 3.10.10.5  1996/09/06  01:11:42  snyder
 * CSCdi68277:  make shr_core, shr_ukernel, sub_cxxxx sub_core_platform
 *              things const, save 712 out of data, 208 from image
 * Branch: California_branch
 *
 * Revision 3.10.10.4  1996/08/07  09:01:57  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.10.10.3  1996/06/17  23:35:30  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.10.10.2  1996/05/21  09:58:24  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.10.10.1  1996/04/15  14:59:14  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.10  1996/02/22  14:36:52  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.9  1996/02/12  18:18:31  abaindur
 * CSCdi48693:  show process mem does not include alternate mempool mallocs
 *
 * Revision 3.8  1996/02/07  16:14:32  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.7  1996/02/01  06:07:52  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/25  23:52:37  hampton
 * Add support to the scheduler for nesting wakeup event lists.  This
 * should make it easier to convert portion of the code.  Also allow for
 * the creation of a detached list of watched events.  [CSCdi46566]
 *
 * Revision 3.5  1996/01/25  18:38:10  hampton
 * Fix per-process heap checking code.  Also convert it from conditional
 * compilation to an "internal" command.  [CSCdi47517]
 *
 * Revision 3.4  1995/12/17  18:34:23  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  18:51:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:18:46  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:47:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/09/05  16:38:05  hampton
 * Fix autoinstall over frame relay.  [CSCdi38852]
 *
 * 1) Udp should only add one-shot queue watches inside of read_ipsocket.
 * 2) Allow a existing one-shot watch to be upgraded to a recurring watch,
 *    but prevent the downgrade of an existing recurring watch to a
 *    one-shot watch.
 * 3) Add some conditionally compiled debugging to the scheduler.
 *
 * Revision 2.4  1995/07/24 09:24:10  hampton
 * Requested scheduler additions.  [CSCdi37542]
 *
 * Revision 2.3  1995/07/02 10:51:19  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/18  07:31:11  hampton
 * Remove extraneous field from show process output.  [CSCdi36041]
 *
 * Revision 2.1  1995/06/07 21:58:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "sched_private.h"
#include "config.h"
#include "subsys.h"
#include "parser.h"
#include "ttysrv.h"
#include "stacks.h"
#include "logger.h"
#include "mgd_timers.h"
#include "../os/free.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_scheduler.h"
#include "techsupport.h"
#include "sys_registry.h"

/*
 * Sheduler config commands
 */
#define ALTERNATE NONE
#include	"cfg_scheduler.h"
LINK_POINT      (scheduler_commands, ALTERNATE);
#undef  ALTERNATE

#define ALTERNATE NONE
#include	"exec_show_processes.h"
LINK_POINT      (scheduler_show_commands, ALTERNATE);
#undef  ALTERNATE

static parser_extension_request scheduler_init_table[] = {
    { PARSE_ADD_CFG_LAST_CMD, &pname(scheduler_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(scheduler_show_commands) },
    { PARSE_LIST_END, NULL },
};

/*
 * The display codes for the current state of a process.  This must match the
 * enum codes in sched_private.h.  Any changes made here must also be made
 * to grovel.c.
 */
static char *const sched_test_abbrev[] = {"* ", "E ", "S ", "rd", "we", "sa",
			     "si", "sp", "st", "hg", "xx"};
static char *const sched_test_words[] = {
    "Running",
    "Event Disms",
    "Suspend",
    "Ready to Run",
    "Waiting for Event",
    "Sleeping (Absolute Time)",
    "Sleeping (Interval)",
    "Sleeping (Periodic)",
    "Sleeping (Mgd Timer)",
    "Hung",
    "Dead"};

static void show_techsupport_processes (parseinfo *csb)
{
    show_techsupport_command(csb, "show process memory");
    show_techsupport_command(csb, "show process cpu");
}

static void sched_ui_init (subsystype *subsys)
{
    parser_add_command_list(scheduler_init_table, "sched");
    parser_add_link_point(PARSE_ADD_CFG_SCHED_CMD, "scheduler",
			  &pname(cfg_scheduler_extend_here));
    reg_add_show_techsupport(show_techsupport_processes,
			     "show_techsupport_processes");
}

/*
 * proc_prio_char
 * Return the process priority character.
 */

static char proc_prio_char (sprocess *sp)
{
    if (sp->killed)
	return('K');
    if (sp->crashed)
	return('D');
    if (sp->corrupt)
	return('X');
    switch (sp->priority) {		
      case PRIO_CRITICAL:
	return('C');
      case PRIO_HIGH:
	return('H');
      case PRIO_NORMAL:
	return('M');
      case PRIO_LOW:
	return('L');
      default:
	return('?');		/* a bug somewhere */
    }
}

/*
 * show_processes
 * Summarize the process lists
 */

static const char phead[] =
     " PID QTy       PC Runtime (ms)    Invoked   uSecs    Stacks TTY Process";

void show_processes (parseinfo *csb)
{
    sprocess_list *array, *next;
    sprocess *pp;
    unsigned int usecs, stackuse, msecs, calls;
    int i;

    automore_enable(NULL);
    printf("\nCPU utilization for five seconds: %d%%/%d%%; one minute: %d%%;"
	   " five minutes: %d%%",
	   last_busy_percent, last_int_percent_scaled/LOAD_SCALAR, 
	   avg_busy1_percent, avg_busy5_percent);
    automore_header(phead);
    for (array = sarrayQ.qhead; array; array = next) {
      mem_lock(array);
      for (i = 0; i < MAX_PROC_PER_ARRAY; ++i) {
	pp = array->sl_process[i];
	if (!pp)
	    continue;		        /* empty slot */
	msecs = pp->elapsedtime;
	calls = pp->ncalls;
	if (calls) {
	    while (msecs > 0xffffffff/1000) { /* Scale so we can multiply */
		msecs >>= 1;
		calls >>= 1;
	    }			/* Guaranteed to terminate in 10 loops max */
	    if (!calls)		/* Absurd, but we don't want to barf */
		calls = 1;
	    usecs = (msecs * 1000) / calls;
	} else
	    usecs = 0;			/* avoid zero divide traps */
	stackuse = stackleft(pp->stack, pp->stacksize);
	printf("\n%-4d %c%s %-8x%-13u%-11u%-8d%-5d/%5d%-3t %16s", pp->pid,
	       proc_prio_char(pp), 
	       sched_test_abbrev[pp->state], pp->caller_pc,
	       pp->elapsedtime, pp->ncalls, usecs,
	       stackuse, pp->stacksize, tt2number(pp->ttyptr), pp->name);
      }
      next = array->sl_next;
      free(array);
    }
    automore_disable();
}

/*
 * show_proc_cpu
 * Show processor utilization per task
 */

static const char spchead[] =
     " PID  Runtime(ms)  Invoked  uSecs    5Sec   1Min   5Min TTY Process";
/*    nnnn nnnnnnnnnnnnxxxxxxxxxnnnnnnn  xx.xx% nn.nn% xx.xx% 112 Process name */
static const char spcfmt[] =
     " \n%-4d %-11u%-10u%-7d  %-2d.%02d%% %-2d.%02d%% %-2d.%02d%% %-3t %s";
#define LOAD_INT(x)		((x)/LOAD_SCALAR)
#define LOAD_TENTHS(x)		(((x)/(LOAD_SCALAR/10)) % 10)
#define LOAD_HUNDREDTHS(x)	(((x)/(LOAD_SCALAR/100)) % 100)

void show_proc_cpu (parseinfo *csb)
{
    sprocess_list *array, *next;
    sprocess *pp;
    unsigned int usecs, msecs, calls;
    int i;

    automore_enable(NULL);
    printf("\nCPU utilization for five seconds: %d%%/%d%%; one minute: %d%%;"
	   " five minutes: %d%%",
	   last_busy_percent, last_int_percent_scaled/LOAD_SCALAR, 
	   avg_busy1_percent, avg_busy5_percent);
    automore_header(spchead);
    for (array = sarrayQ.qhead; array; array = next) {
      mem_lock(array);
      for (i = 0; i < MAX_PROC_PER_ARRAY; ++i) {
	pp = array->sl_process[i];
	if (!pp)
	    continue;		        /* empty slot */
	msecs = pp->elapsedtime;
	calls = pp->ncalls;
	if (calls) {
	    while (msecs > 0xffffffff/1000) { /* Scale so we can multiply */
		msecs >>= 1;
		calls >>= 1;
	    }			/* Guaranteed to terminate in 10 loops max */
	    if (!calls)		/* Absurd, but we don't want to barf */
		calls = 1;
	    usecs = (msecs * 1000) / calls;
	} else
	    usecs = 0;			/* avoid zero divide traps */
	printf(spcfmt, pp->pid, pp->elapsedtime, pp->ncalls, usecs, 
	       LOAD_INT(pp->pct_cpu_5sec), LOAD_HUNDREDTHS(pp->pct_cpu_5sec),
	       LOAD_INT(pp->pct_cpu_1min), LOAD_HUNDREDTHS(pp->pct_cpu_1min),
	       LOAD_INT(pp->pct_cpu_5min), LOAD_HUNDREDTHS(pp->pct_cpu_5min),
	       tt2number(pp->ttyptr), pp->name);
      }
      next = array->sl_next;
      free(array);
    }
    automore_disable();
}

/*
 * sched_max_task_time_cmd
 *
 * 	[no] scheduler max-task-time [<milliseconds>]
 *
 * Set the max time that a task can run without flagging an error.
 * The "no" version removes this check.  If specified without a parameter,
 * the default is used.
 */
void sched_max_task_time_cmd (parseinfo *csb)
{
    if (csb->nvgen) {
	if (sched_max_task_time == 0) {
	    nv_write(TRUE, "no %s", csb->nv_command);
	} else {
	    nv_write(sched_max_task_time != SCHED_MAX_TASK_TIME_DEFAULT,
		     "%s %d", csb->nv_command, sched_max_task_time);
	}
	return;
    }

    if (csb->set_to_default) {
	sched_max_task_time = SCHED_MAX_TASK_TIME_DEFAULT;
	watchdog_time = sched_max_task_time << WATCHDOG_TIME_SHIFT;
    } else if (csb->sense) {
	sched_max_task_time = GETOBJ(int,1);
	watchdog_time = sched_max_task_time << WATCHDOG_TIME_SHIFT;
    } else {
	sched_max_task_time = 0;
 	watchdog_time = 0;
    }
}

/*
 * show_proc_mem
 * Display memory allocated per each process and overhead
 */

static const char spmhead[] =
 {" PID  TTY  Allocated      Freed    Holding    Getbufs    Retbufs Process"};

static const char show_proc_mem_format_string[] =
    "\n%-4d %-4o %-10lu %-10lu %-10lu %-10lu %-10lu %s";

void show_proc_memory (parseinfo *csb)
{
    sprocess_list *array, *next;
    int i;
    ulong totactive = 0;
    ulong used, freeb;
    sprocess *pp;
    ulong initcount, schedcount, deadcount;

    automore_enable(NULL);
    freeb = mempool_get_free_bytes(MEMPOOL_CLASS_LOCAL,
				   MEMPOOL_ALTERNATE_INCLUDE);
    used =  mempool_get_used_bytes(MEMPOOL_CLASS_LOCAL,
				   MEMPOOL_ALTERNATE_INCLUDE);
    printf("\nTotal: %lu, Used: %lu, Free: %lu", used+freeb, used, freeb);
    automore_header(spmhead);

    tally_memory_usage(MEMPOOL_CLASS_LOCAL, MEMPOOL_ALTERNATE_INCLUDE,
		       &initcount, &schedcount, &deadcount);

    printf(show_proc_mem_format_string, 0, 0, init_totmalloc, init_totfree,
	   initcount, init_totgetbufmem, init_totretbufmem, "*Init*");
    totactive += initcount;

    printf(show_proc_mem_format_string, 0, 0, sched_totmalloc, sched_totfree,
	   schedcount, sched_totgetbufmem, sched_totretbufmem, "*Sched*");
    totactive += schedcount;

    printf(show_proc_mem_format_string, 0, 0, dead_totmalloc, dead_totfree,
	   deadcount, dead_totgetbufmem, dead_totretbufmem, "*Dead*");
    totactive += deadcount;

    for (array = sarrayQ.qhead; array; array = next) {
      mem_lock(array);
      for (i = 0; i < MAX_PROC_PER_ARRAY; ++i) {
	pp = array->sl_process[i];
	if (!pp)
	    continue;		       
	printf(show_proc_mem_format_string, pp->pid, tt2number(pp->ttyptr),
	       pp->totmalloc, pp->totfree, pp->mem_holding, pp->totgetbufmem,
	       pp->totretbufmem, pp->name);
	totactive += pp->mem_holding;
      }
      next = array->sl_next;
      free(array);
    }
    printf("\n%-42lu Total", totactive);
    automore_disable();
}

/*
 * show_proc_events
 *
 * Show wakeup reasons for all processes
 */

static const char timer_watch[] =
     "\n  Watching %s timer at address %#x.  ";

static boolean show_one_proc_one_event_list (sched_event_set *info)
{
    list_element *element;
    watched_item *event;
    watcher_info *wakeup;
    watched_queue *queue;
    watched_boolean *bool;
    watched_bitfield *bitfield;
    watched_semaphore *sem;
    boolean printed_something = FALSE;

    FOR_ALL_DATA_IN_LIST(&info->process_events, element, wakeup) {
	event = wakeup->wi_event;
	switch (event->major_type) {
	  case QUEUE_EVENT:
	    queue = (watched_queue *)event;
	    printf("\n  Watching queue '%s' (%#x), id %#x, count %d.",
		   event->name, event, event->minor_type,
		   QUEUESIZE(&queue->q));
	    printed_something = TRUE;
	    break;
	  case BOOLEAN_EVENT:
	    bool = (watched_boolean *)event;
	    printf("\n  Watching boolean '%s' (%#x), id %#x, value %s.",
		   event->name, event, event->minor_type,
		   WB_VALUE(bool) ? "True" : "False");
	    printed_something = TRUE;
	    break;
	  case BITFIELD_EVENT:
	    bitfield = (watched_bitfield *)event;
	    printf("\n  Watching bitfield '%s' (%#x), id %#x, value %08x.",
		   event->name, event, event->minor_type,
		   bitfield->bits);
	    printed_something = TRUE;
	    break;
	  case SEMAPHORE_EVENT:
	    sem = (watched_semaphore *)event;
	    printf("\n  Watching semaphore '%s' (%#x), id %#x, value %d.",
		   event->name, event, event->minor_type, sem->s);
	    printed_something = TRUE;
	    break;
	  case TIMER_EVENT:
	  case MESSAGE_EVENT:
	    break;
	}
    }
    
    /*
     * Timers and messaged are not maintained on the notification list.
     */
    if (info->watched_mgd_timer) {
	printf(timer_watch, "managed", info->watched_mgd_timer);
	if (mgd_timer_running(info->watched_mgd_timer)) {
	    printf("Expires in %TG.", info->watched_mgd_timer);
	} else {
	    printf("Not running.");
	}
	printed_something = TRUE;
    }
    if (info->watched_simple_timer) {
	printf(timer_watch, "simple", info->watched_simple_timer);
	if (TIMER_RUNNING(*info->watched_simple_timer)) {
	    printf("Expires in %TF.", *info->watched_simple_timer);
	} else {
	    printf("Not running.");
	}
	printed_something = TRUE;
    }
    return(printed_something);
}

void show_one_proc_all_events (sprocess *pp)
{
    boolean printed_something;
    sched_event_set *info;
  
    printf("\n\nProcess %d: %s, state: %s", pp->pid, pp->name,
	   sched_test_words[pp->state]);
    switch (pp->state) {
      default:
      case SCHEDSTATE_RUNNING:
      case SCHEDSTATE_SUSPEND:
      case SCHEDSTATE_READY:
      case SCHEDSTATE_HUNG:
      case SCHEDSTATE_DEAD:
	return;

      case SCHEDSTATE_EDISMS:
	printf("\n  Edisms on address %#x, argument %#x.",
	       pp->edisms, pp->eparm);
	return;

      case SCHEDSTATE_SLEEP_ABS:
      case SCHEDSTATE_SLEEP_INTERVAL:
      case SCHEDSTATE_SLEEP_PERIODIC:
      case SCHEDSTATE_SLEEP_TIMER:
	printf("\n  Sleeping for another %TG.", &pp->sched_timer);
	return;

      case SCHEDSTATE_IDLE:
	printed_something = show_one_proc_one_event_list(pp->current_events);
	if (mgd_timer_running(&pp->sched_timer_temp)) {
	    printf("\n  Watching a delta timer. Expires in %TG.",
		   &pp->sched_timer_temp);
	    printed_something = TRUE;
	}
	for (info = pp->process_event_setQ.qhead; info; info = info->next) {
	    printf("\n  ---");
	    printed_something |= show_one_proc_one_event_list(info);
	}
	if (!QUEUEEMPTY(&pp->message_queue)) {
	    printf("\n  %d waiting messages.", QUEUESIZE(&pp->message_queue));
	    printed_something = TRUE;
	}
	if (!printed_something) {
	    printf("\n  No events registered.");
	    return;
	}
	break;
    }
}

void show_proc_events (parseinfo *csb)
{
    sprocess_list *array, *next;
    sprocess *pp;
    int i;

    automore_enable(NULL);
    if (GETOBJ(int,1)) {
	pp = process_pid2ptr(GETOBJ(int,1));
	if (pp)
	    show_one_proc_all_events(pp);
    } else {
	for (array = sarrayQ.qhead; array; array = next) {
	    mem_lock(array);
	    for (i = 0; i < MAX_PROC_PER_ARRAY; ++i) {
		pp = array->sl_process[i];
		if (!pp)
		    continue;		        /* empty slot */
		show_one_proc_all_events(pp);
	    }
	    next = array->sl_next;
	    free(array);
	}
    }
    automore_disable();
}

/*
 * sched_watchdog_command
 *
 * Parse the "scheduler process-watchdog" command.
 */

void
sched_watchdog_command (parseinfo *csb)
{
    if (csb->nvgen) {
	if (sched_watchdog_action == SCHED_WATCHDOG_NORMAL)
	    return;
	nv_write((sched_watchdog_action == GETOBJ(int,1)), csb->nv_command);
	return;
    }

    if (!csb->sense) {
	sched_watchdog_action = SCHED_WATCHDOG_NORMAL;
	return;
    }

    sched_watchdog_action = GETOBJ(int, 1);
}

/*
 * sched_heaps_command
 *
 * Parse the "scheduler heapcheck" command.
 */

void sched_heaps_command (parseinfo *csb)
{
    if (csb->nvgen) {
	switch (GETOBJ(int,1)) {
	  case SCHED_HEAP_POLL:
	    nv_write(checkheaps_per_poll, csb->nv_command);
	    break;
	  case SCHED_HEAP_PROC:
	    nv_write(checkheaps_per_proc, csb->nv_command);
	    break;
	  default:
	    break;
	}
	return;
    }

    switch (GETOBJ(int,1)) {
      case SCHED_HEAP_POLL:
	checkheaps_per_poll = csb->sense;
	break;
      case SCHED_HEAP_PROC:
	checkheaps_per_proc = csb->sense;
	break;
      default:
	bad_parser_subcommand(csb, GETOBJ(int,1));
	break;
    }
}

/**********************************************************************
 *
 *		    NOTIFICATION DISPLAY ROUTINES
 *
 **********************************************************************/

static const
char spnothdr[] = "     Event    Name                      Pid  1 Process";

/*
 * process_show_one_queue
 *
 * Show all process that are waiting for a given event to occur.
 */
static void process_show_one_queue (list_header *event_list)
{
    list_element *ev_elem, *next_ev_elem;
    list_element *wake_elem, *next_wake_elem;
    watched_item *event;
    watcher_info *wakeup;
    sprocess *proc;
    char flag1, leader[40], tag[10];
    watched_queue *qi;
    watched_boolean *bo;
    watched_bitfield *bi;

    if (LIST_EMPTY(event_list))
	return;

    next_ev_elem = NULL;
    FOR_ALL_DATA_IN_LIST_LOCKED(event_list, ev_elem, next_ev_elem, event) {
	mem_lock(event);
	sprintf(leader, "%-8x  %c %25s", event, event->wake_first_only?'F':' ',
		event->name);
	switch (event->major_type) {
	  case QUEUE_EVENT:
	    qi = (watched_queue *)event;
	    sprintf(tag, "ct %d", WQ_QUEUESIZE(qi));
	    break;
	  case BOOLEAN_EVENT:
	    bo = (watched_boolean *)event;
	    sprintf(tag, "%s", WB_VALUE(bo) ? "TRUE" : "FALSE");
	    break;
	  case BITFIELD_EVENT:
	    bi = (watched_bitfield *)event;
	    sprintf(tag, "%08x", bi->bits);
	    break;
	  default:
	    tag[0] = '\0';
	    break;
	}

	if (LIST_EMPTY(&event->wakeup_list)) {
	    printf("\n  %37s        %22s  %s", leader, "(no watchers)", tag);
	    next_ev_elem = LIST_NEXT_ELEMENT(ev_elem);
	    free(event);
	    continue;
	}

	next_wake_elem = NULL;
	FOR_ALL_DATA_IN_LIST_LOCKED(&event->wakeup_list, wake_elem,
				    next_wake_elem, wakeup) {
	    mem_lock(wakeup);
	    proc = wakeup->wi_process;
	    flag1 = (wakeup->wi_one_shot ? (wakeup->wi_shot?'e':'a') : ' ');
	    printf("\n  %37s  %-2d  %c %22s  %s", leader, proc->pid, flag1,
		   proc->name, tag);
	    leader[0] = '\0';
	    tag[0] = '\0';
	    next_wake_elem = LIST_NEXT_ELEMENT(wake_elem);
	    free(wakeup);
	}
	next_ev_elem = LIST_NEXT_ELEMENT(ev_elem);
	free(event);
    }
}

/*
 * show_proc_allevents
 *
 * Show events for which processes have requested notification.
 */
void show_proc_allevents (parseinfo *csb)
{
    automore_enable(NULL);
    printf("Queue Notifications");
    automore_header(spnothdr);
    process_show_one_queue(&s_notify_queue);
    printf("\nBoolean Notifications");
    process_show_one_queue(&s_notify_boolean);
    printf("\nBitfield Notifications");
    process_show_one_queue(&s_notify_bitfield);
    printf("\nSemaphore Notifications");
    process_show_one_queue(&s_notify_semaphore);
    automore_disable();
}


/*
 * sched_monitor_page_zero
 *
 * Should this router be checking addresses 0x0 - 0xFF for illegal
 * writes?  The default is to have this enabled.  Obviously, this
 * feature cannot exist unless there is RAM at address zero.
 */
void sched_monitor_page_zero (parseinfo *csb)
{
#ifdef TRAP_PAGE_ZERO
    if (csb->nvgen) {
	nv_write(!sched_pagezero_enabled, "no %s", csb->nv_command);
	return;
    }

    sched_pagezero_enabled = csb->set_to_default || csb->sense;
    if (csb->sense) {
	sched_save_page_zero();
    } else {
	sched_restore_page_zero();
    }
#endif
}


/*
 * sched_degraded_mode
 *
 * Should this router try attempt to continue running after data
 * corruption is detected?
 */
void sched_degraded_mode (parseinfo *csb)
{
    if (csb->nvgen) {
	nv_write(sched_run_degraded, "%s", csb->nv_command);
	return;
    }

    sched_run_degraded = csb->set_to_default || csb->sense;
}

/*
 * sched_ui subsystem header
 */
 
#define SCHED_UI_MAJVERSION    1
#define SCHED_UI_MINVERSION    0
#define SCHED_UI_EDITVERSION   1
 
SUBSYS_HEADER(sched_ui, SCHED_UI_MAJVERSION, SCHED_UI_MINVERSION,
              SCHED_UI_EDITVERSION,
              sched_ui_init, SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);
