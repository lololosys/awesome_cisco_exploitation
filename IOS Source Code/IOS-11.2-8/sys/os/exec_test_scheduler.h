/* $Id: exec_test_scheduler.h,v 3.1.40.2 1996/08/12 16:05:22 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/exec_test_scheduler.h,v $
 *------------------------------------------------------------------
 * Test routines for changes to the scheduler.
 *
 * April 1995, David Hampton
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_test_scheduler.h,v $
 * Revision 3.1.40.2  1996/08/12  16:05:22  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.40.1  1996/03/27  21:51:53  hampton
 * Try to catch and report thrashing processes.  A few other minor
 * scheduler cleanups and enhancements.  [CSCdi50130]
 * Branch: California_branch
 *
 * Revision 3.1  1996/02/01  04:58:59  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.7  1996/01/25  23:52:45  hampton
 * Add support to the scheduler for nesting wakeup event lists.  This
 * should make it easier to convert portion of the code.  Also allow for
 * the creation of a detached list of watched events.  [CSCdi46566]
 *
 * Revision 3.6  1995/12/23  13:10:26  hampton
 * Fix a timing window where interfaces can be mistakenly throttled.
 * [CSCdi44951]
 *
 * Revision 3.5  1995/12/21  16:29:52  hampton
 * Switch the 68K version of DELAY() to call the existing usecdelay
 * routine which uses the timer chip.  For all other platforms, move a
 * routine or two between files.  [CSCdi45965]
 *
 * Revision 3.4  1995/11/18  06:06:30  hampton
 * Don't let the entire router be dragged down by a single process with a
 * busted timer.  [CSCdi44073]
 *
 * Revision 3.3  1995/11/17  18:48:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:20:19  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:53:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:20:26  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/09/05  16:38:11  hampton
 * Fix autoinstall over frame relay.  [CSCdi38852]
 *
 * 1) Udp should only add one-shot queue watches inside of read_ipsocket.
 * 2) Allow a existing one-shot watch to be upgraded to a recurring watch,
 *    but prevent the downgrade of an existing recurring watch to a
 *    one-shot watch.
 * 3) Add some conditionally compiled debugging to the scheduler.
 *
 * Revision 2.2  1995/07/24 18:39:22  hampton
 * Requested scheduler additions.  [CSCdi37542]
 *
 * Revision 2.1  1995/06/07 22:27:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define SCHED_TEST_BITFIELD		1
#define SCHED_TEST_DEBUG		2
#define SCHED_TEST_EXIT			3
#define SCHED_TEST_NEW_PROC		4
#define SCHED_TEST_OLD_PROC		5
#define SCHED_TEST_PRINT_ROUTINE	6
#define SCHED_TEST_PRINT_TIMESTAMP	7
#define SCHED_TEST_SEMAPHORE		8
#define SCHED_TEST_WATCHDOG		9
#define SCHED_TEST_WATCHDOG_NF		10
#define SCHED_TEST_MATH			11
#define SCHED_TEST_STUCK_TIMER		12
#define SCHED_TEST_DELAY		13
#define SCHED_TEST_LIBRARY		14
#define SCHED_TEST_THRASHING		15
#define SCHED_TEST_TRASH		16

#define SCHED_TRASH_EVENTSET		0x00000001
#define SCHED_TRASH_FORKX1		0x00000002
#define SCHED_TRASH_FORKX2		0x00000004
#define SCHED_TRASH_FORKX3		0x00000008
#define SCHED_TRASH_MEMORY		0x00000010
#define SCHED_TRASH_RED			0x00000020
#define SCHED_TRASH_SP			0x00000040
#define SCHED_TRASH_STACK		0x00000080

#define SCHED_TRASH_EDISMS		1
#define SCHED_TRASH_SLEEP		2
#define SCHED_TRASH_SUSPEND		3
#define SCHED_TRASH_WAIT		4

/******************************************************************
 * test scheduler ...
 *
 * test scheduler bitfield
 *   - test the routines process_set_bitfield(), process_keep_bitfield(), and
 *     process_clear_bitfield().
 *
 * test scheduler debug { execution | wakeup | delay | watch | off } <process>
 *     
 * test scheduler delay
 *   - chek times on the usecdelay() and msecdelay() routines.
 *
 * test scheduler exit
 *   - check exit hook routines
 *
 * test scheduler library
 *   - check the process_push/pop_event_list() routines.
 *
 * test scheduler math
 *   - test 64 bit math routines.
 *
 * test scheduler new <load> <loop_count>
 * test scheduler old <load> <loop_count>
 *   - Create some number of new or old style processes.  These processes will
 *     loop around the scheduler call the indicated number of times and then
 *     quit.
 *
 * test scheduler printing { routines | timestamp }
 *   - 'routines' tests printf(), buginf(), errmsg().
 *   - 'timestamps' tests the various '%Tx' printf formatting characters
 *
 * test scheduler semaphore
 *   - test semaphore primitives
 *
 * test scheduler stuck
 *   - test for detection of a stuck timer
 *
 * test scheduler thrashing
 *   - test for detection of a thrashing process
 *
 * test scheduler trash <scheduler_call> <item_to_trash>
 *   - test for detection of various trashed memory locations.
 *     - The scheduler_call argument may be varied to insure that all calls
 *       detect the problem.  The possible values are 'edisms', 'wait',
 *       'suspend', and 'sleep'.  For items detected in the routined
 *       process_ok_to_reschedule(), only one of these need be checked.
 *     - The item_to_trash argument is one of:
 *         stack:   Zero the bottom word of th stack.
 *         sp:      Move the stack pointer below the stack.
 *         redzone: Overwrite the red zone of this processes' stack.
 *         memory:  Overwrite the block header of this processes' stack.
 *
 * test scheduler wd-fatal
 *   - cause a fatal watchdog error.
 *
 * test scheduler wd-non-fatal
 *   - cause a non-fatal watchdog error.
 */

EOLNS	(schedtest_eol, sched_test_command);
NUMBER  (schedtest_loop, schedtest_eol, no_alt,
	 OBJ(int,2), 0, MAXUINT, "Loop Count");
NUMBER  (schedtest_load, schedtest_loop, no_alt,
	 OBJ(int,1), 0, MAXUINT, "Load Count");

KEYWORD_ID (schedtest_timestamp, schedtest_eol, no_alt,
	    OBJ(int,3), SCHED_TEST_PRINT_TIMESTAMP,
	    "timestamp", "Timestamp printing", PRIV_ROOT);
KEYWORD_ID (schedtest_routines, schedtest_eol, schedtest_timestamp,
	    OBJ(int,3), SCHED_TEST_PRINT_ROUTINE,
	    "routines", "Printf/buginf/errmsg routines", PRIV_ROOT);
KEYWORD    (schedtest_print, schedtest_routines, no_alt,
	    "printing", "Printing related items", PRIV_ROOT);

NUMBER	   (st_debug_proc, schedtest_eol, no_alt,
	    OBJ(int,1), 0, MAXUINT, "Process (0 == global)");
KEYWORD_ID (st_debug_off, st_debug_proc, st_debug_proc,
	    OBJ(int,2), 0,
	    "off", NULL, PRIV_ROOT);
KEYWORD_ID (st_debug_watch, st_debug_proc, st_debug_off,
	    OBJ(int,2), PROC_DEBUG_WATCH,
	    "watch", NULL, PRIV_ROOT);
KEYWORD_ID (st_debug_delay, st_debug_proc, st_debug_watch,
	    OBJ(int,2), PROC_DEBUG_WAKEUP_DELAY,
	    "delay", NULL, PRIV_ROOT);
KEYWORD_ID (st_debug_wakeup, st_debug_proc, st_debug_delay,
	    OBJ(int,2), PROC_DEBUG_WAKEUP,
	    "wakeup", NULL, PRIV_ROOT);
KEYWORD_ID (st_debug_execution, st_debug_proc, st_debug_wakeup,
	    OBJ(int,2), PROC_DEBUG_EXECUTION,
	    "execution", NULL, PRIV_ROOT);
KEYWORD_ID (schedtest_debug, st_debug_execution, schedtest_print,
	    OBJ(int,3), SCHED_TEST_DEBUG,
	    "debug", "Debug a process", PRIV_ROOT);


pdecl(sched_trash_opts);
EOLNS      (sched_trash_eol, sched_trash_command);
NEQ        (sched_trash_test, sched_trash_eol, no_alt, OBJ(int,4), 0);
KEYWORD_OR (sched_trash_events, sched_trash_opts, sched_trash_test,
	    OBJ(int,4), SCHED_TRASH_EVENTSET,
	    "event-set", "Trash event-set variable", PRIV_ROOT);
KEYWORD_OR (sched_trash_forkx1, sched_trash_opts, sched_trash_events,
	    OBJ(int,4), SCHED_TRASH_FORKX1,
	    "forkx1", "Trash forkx variable", PRIV_ROOT);
KEYWORD_OR (sched_trash_forkx2, sched_trash_opts, sched_trash_forkx1,
	    OBJ(int,4), SCHED_TRASH_FORKX2,
	    "forkx2", "Trash forkx_2 variable", PRIV_ROOT);
KEYWORD_OR (sched_trash_forkx3, sched_trash_opts, sched_trash_forkx2,
	    OBJ(int,4), SCHED_TRASH_FORKX3,
	    "forkx3", "Trash forkx_3 variable", PRIV_ROOT);
KEYWORD_OR (sched_trash_memory, sched_trash_opts, sched_trash_forkx3,
	    OBJ(int,4), SCHED_TRASH_MEMORY,
	    "memory", "Trash memory block header", PRIV_ROOT);
KEYWORD_OR (sched_trash_red, sched_trash_opts, sched_trash_memory,
	    OBJ(int,4), SCHED_TRASH_RED,
	    "redzone", "Trash red zone", PRIV_ROOT);
KEYWORD_OR (sched_trash_sp, sched_trash_opts, sched_trash_red,
	    OBJ(int,4), SCHED_TRASH_SP,
	    "sp", "Trash stack pointer", PRIV_ROOT);
KEYWORD_OR (sched_trash_stack, sched_trash_opts, sched_trash_sp,
	    OBJ(int,4), SCHED_TRASH_STACK,
	    "stack", "Trash stack memory", PRIV_ROOT);
NOP	   (sched_trash_opts, sched_trash_stack, NONE);

KEYWORD_ID (sched_trash_wait, sched_trash_opts, no_alt,
	    OBJ(int,5), SCHED_TRASH_WAIT,
	    "wait", "Call process_wait_for_event() before exiting", PRIV_ROOT);
KEYWORD_ID (sched_trash_suspend, sched_trash_opts, sched_trash_wait,
	    OBJ(int,5), SCHED_TRASH_SUSPEND,
	    "suspend", "Call process_suspend() before exiting", PRIV_ROOT);
KEYWORD_ID (sched_trash_sleep, sched_trash_opts, sched_trash_suspend,
	    OBJ(int,5), SCHED_TRASH_SLEEP,
	    "sleep", "Call process_sleep_for() before exiting", PRIV_ROOT);
KEYWORD_ID (sched_trash_edisms, sched_trash_opts, sched_trash_sleep,
	    OBJ(int,5), SCHED_TRASH_EDISMS,
	    "edisms", "Call edisms() before exiting", PRIV_ROOT);

KEYWORD    (schedtest_trash, sched_trash_edisms, schedtest_debug,
	    "trash", "Trash a data structure", PRIV_ROOT);
KEYWORD_ID (schedtest_thrash, schedtest_eol, schedtest_trash,
	    OBJ(int,3), SCHED_TEST_THRASHING,
	    "thrashing", "Process_thrashing", PRIV_ROOT);
KEYWORD_ID (schedtest_delay, schedtest_eol, schedtest_thrash,
	    OBJ(int,3), SCHED_TEST_DELAY,
	    "delay", "Delay primitives", PRIV_ROOT);
KEYWORD_ID (schedtest_library, schedtest_eol, schedtest_delay,
	    OBJ(int,3), SCHED_TEST_LIBRARY,
	    "library", "Library primitives", PRIV_ROOT);
KEYWORD_ID (schedtest_stuck, schedtest_eol, schedtest_library,
	    OBJ(int,3), SCHED_TEST_STUCK_TIMER,
	    "stuck", "Stuck timers in a process", PRIV_ROOT);
KEYWORD_ID (schedtest_math, schedtest_eol, schedtest_stuck,
	    OBJ(int,3), SCHED_TEST_MATH,
	    "math", "64 bit math primitives", PRIV_ROOT);
KEYWORD_ID (schedtest_bitfield, schedtest_eol, schedtest_math,
	    OBJ(int,3), SCHED_TEST_BITFIELD,
	    "bitfield", "Bitfield primitives", PRIV_ROOT);
KEYWORD_ID (schedtest_semaphore, schedtest_eol, schedtest_bitfield,
	    OBJ(int,3), SCHED_TEST_SEMAPHORE,
	    "semaphore", "Semaphore primitives", PRIV_ROOT);
KEYWORD_ID (schedtest_wd, schedtest_eol, schedtest_semaphore,
	    OBJ(int,3), SCHED_TEST_WATCHDOG,
	    "wd-fatal", "Watchdog (fatal)", PRIV_ROOT);
KEYWORD_ID (schedtest_wd_nf, schedtest_eol, schedtest_wd,
	    OBJ(int,3), SCHED_TEST_WATCHDOG_NF,
	    "wd-non-fatal", "Watchdog (non-fatal)", PRIV_ROOT);
KEYWORD_ID (schedtest_exit, schedtest_eol, schedtest_wd_nf,
	    OBJ(int,3), SCHED_TEST_EXIT,
	    "exit", "New exit hook", PRIV_ROOT);
KEYWORD_ID (schedtest_new, schedtest_load, schedtest_exit,
	    OBJ(int,3), SCHED_TEST_NEW_PROC,
	    "new", "New scheduler primitives", PRIV_ROOT);
KEYWORD_ID (schedtest_old, schedtest_load, schedtest_new,
	    OBJ(int,3), SCHED_TEST_OLD_PROC,
	    "old", "Old scheduler primitives", PRIV_ROOT);

KEYWORD	(test_scheduler, schedtest_old, ALTERNATE,
	 "scheduler", "scheduler test code", PRIV_OPR/*|PRIV_NOHELP*/);

#undef	ALTERNATE
#define	ALTERNATE	test_scheduler
