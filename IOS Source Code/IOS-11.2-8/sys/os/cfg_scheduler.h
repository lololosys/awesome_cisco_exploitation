/* $Id: cfg_scheduler.h,v 3.4.20.1 1996/08/12 16:05:00 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/cfg_scheduler.h,v $
 *------------------------------------------------------------------
 * cfg_scheduler.h -- Scheduler commands
 *
 * Dec. 1994 Tony Li
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_scheduler.h,v $
 * Revision 3.4.20.1  1996/08/12  16:05:00  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4  1996/01/26  00:30:41  hampton
 * Remove old scheduler commands and files.  [CSCdi47573]
 *
 * Revision 3.3  1996/01/25  18:37:51  hampton
 * Fix per-process heap checking code.  Also convert it from conditional
 * compilation to an "internal" command.  [CSCdi47517]
 *
 * Revision 3.2  1995/11/17  18:45:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:04  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:13:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(cfg_scheduler_extend_here, no_alt);

/***************************************************************
 * [no] scheduler process-watchdog normal|hang|reload|terminate
 *
 *  OBJ(int,1) = option
 */
EOLNS    (sched_watch_eol, sched_watchdog_command);

KEYWORD_ID(sched_watch_terminate, sched_watch_eol, no_alt,
	   OBJ(int, 1), SCHED_WATCHDOG_TERMINATE,
	   "terminate", "Terminate the process and continue", PRIV_CONF);

KEYWORD_ID(sched_watch_reload, sched_watch_eol, sched_watch_terminate,
	   OBJ(int, 1), SCHED_WATCHDOG_RELOAD,
	   "reload", "Reload the system", PRIV_CONF);

KEYWORD_ID(sched_watch_hang, sched_watch_eol, sched_watch_reload,
	   OBJ(int, 1), SCHED_WATCHDOG_HANG,
	   "hang", "Retain the process but do not schedule it", PRIV_CONF);

KEYWORD_ID(sched_watch_normal, sched_watch_eol, sched_watch_hang,
	   OBJ(int, 1), SCHED_WATCHDOG_NORMAL,
	   "normal", "Factory specified per-process behavior", PRIV_CONF);

NOPREFIX(sched_watch_no, sched_watch_normal, sched_watch_eol);
KEYWORD (sched_watchdog, sched_watch_no, cfg_scheduler_extend_here,
	 "process-watchdog", "Action for looping processes", PRIV_CONF);


/***************************************************************
 * [no] scheduler monitor-page-zero
 *
 */
EOLNS    (sched_monitor_eol, sched_monitor_page_zero);
KEYWORD (sched_monitor_kwd, sched_monitor_eol, sched_watchdog,
	 "monitor-page-zero",
	 NULL /*"Monitor page zero for writes"*/, PRIV_CONF|PRIV_HIDDEN);
#ifdef TRAP_PAGE_ZERO
NOP (sched_monitor, sched_monitor_kwd, NONE);
#else
NOP (sched_monitor, sched_watchdog, NONE);
#endif

/***************************************************************
 * [no] scheduler max-task-time [<milliseconds>]
 *
 *  OBJ(int,1) = time, or zero if no parameter
 */
EOLNS    (sched_max_task_time_eol, sched_max_task_time_cmd);
NUMBER  (sched_max_task_time_value, sched_max_task_time_eol,
	 sched_max_task_time_eol, OBJ(int,1), 1, MAXINT,
	 NULL /*"Maximum time in milliseconds"*/);
SET     (sched_max_task_time_set, sched_max_task_time_value,
	 OBJ(int,1), SCHED_MAX_TASK_TIME_DEFAULT);
NVGENNS  (sched_max_task_time_nv, sched_max_task_time_set,
	 sched_max_task_time_cmd);
NOPREFIX(sched_max_task_time_no, sched_max_task_time_nv,
	 sched_max_task_time_eol);
KEYWORD (sched_max_task_time, sched_max_task_time_no, sched_monitor,
	 "max-task-time",
	 NULL /*"Maximum time that a task can run without flagging an error"*/,
	 PRIV_CONF|PRIV_HIDDEN);

/***************************************************************
 * [no] scheduler heapcheck process|poll
 *
 *  OBJ(int,1) = option
 */
EOLNS    (sched_heaps_eol, sched_heaps_command);

KEYWORD_ID(sched_heaps_poll, sched_heaps_eol, no_alt,
	   OBJ(int, 1), SCHED_HEAP_POLL,
	   "poll", "Check after each edisms poll routine",
	   PRIV_CONF|PRIV_INTERNAL);

KEYWORD_ID(sched_heaps_process, sched_heaps_eol, sched_heaps_poll,
	   OBJ(int, 1), SCHED_HEAP_PROC,
	   "process", "Check after each process executes", PRIV_CONF);

KEYWORD (sched_heaps, sched_heaps_process, sched_max_task_time,
	 "heapcheck", "Extra checking to validate memory",
	 PRIV_CONF|PRIV_HIDDEN);


/***************************************************************
 * [no] scheduler run-degraded
 *
 */
EOLNS   (sched_degraded_eol, sched_degraded_mode);
KEYWORD (sched_degraded, sched_degraded_eol, sched_heaps,
	 "run-degraded", "Continue running as long as possible.",
	 PRIV_CONF|PRIV_HIDDEN);


KEYWORD (scheduler, sched_degraded, ALTERNATE,
	 "scheduler", "Scheduler parameters", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	scheduler
