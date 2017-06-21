/* $Id: sched_test.c,v 3.13.12.4 1996/08/28 13:04:20 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/sched_test.c,v $
 *------------------------------------------------------------------
 * Test routines for changes to the scheduler.
 *
 * April 1995, David Hampton
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sched_test.c,v $
 * Revision 3.13.12.4  1996/08/28  13:04:20  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.13.12.3  1996/05/21  09:58:26  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.13.12.2  1996/05/08  00:07:03  hampton
 * Add better semaphore errors and better code to wait for a semaphore.
 * The routine process_wait_for_event_timed() shouldn't post a TIMER_EVENT
 * when the temporary timer expires.  [CSCdi56999]
 * Branch: California_branch
 *
 * Revision 3.13.12.1  1996/03/27  21:52:35  hampton
 * Try to catch and report thrashing processes.  A few other minor
 * scheduler cleanups and enhancements.  [CSCdi50130]
 * Branch: California_branch
 *
 * Revision 3.13  1996/02/16  21:16:26  hampton
 * Need routine to check for saved scheduler events.  [CSCdi49223]
 *
 * Revision 3.12  1996/02/01  06:07:54  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.11  1996/01/30  21:01:33  vandys
 * CSCdi46080:  Deep six CS500 in source base
 *
 * Revision 3.10  1996/01/25  23:52:41  hampton
 * Add support to the scheduler for nesting wakeup event lists.  This
 * should make it easier to convert portion of the code.  Also allow for
 * the creation of a detached list of watched events.  [CSCdi46566]
 *
 * Revision 3.9  1996/01/06  03:22:49  hampton
 * Remove extraneous includes of if_timer.h.  Rename if_timer.h and move
 * it to the 'os' directory.  [CSCdi46482]
 *
 * Revision 3.8  1995/12/22  21:26:17  hampton
 * Fix compilation error on the cs5000.  [CSCdi45965]
 *
 * Revision 3.7  1995/12/21  16:29:50  hampton
 * Switch the 68K version of DELAY() to call the existing usecdelay
 * routine which uses the timer chip.  For all other platforms, move a
 * routine or two between files.  [CSCdi45965]
 *
 * Revision 3.6  1995/12/19  00:43:02  hampton
 * Trim some more memory usage out of the timers code.  [CSCdi45208]
 *
 * Revision 3.5  1995/12/17  18:34:25  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.4  1995/11/18  06:06:23  hampton
 * Don't let the entire router be dragged down by a single process with a
 * busted timer.  [CSCdi44073]
 *
 * Revision 3.3  1995/11/17  18:51:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:18:49  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:48:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/09/05  16:38:08  hampton
 * Fix autoinstall over frame relay.  [CSCdi38852]
 *
 * 1) Udp should only add one-shot queue watches inside of read_ipsocket.
 * 2) Allow a existing one-shot watch to be upgraded to a recurring watch,
 *    but prevent the downgrade of an existing recurring watch to a
 *    one-shot watch.
 * 3) Add some conditionally compiled debugging to the scheduler.
 *
 * Revision 2.5  1995/07/24 09:24:16  hampton
 * Requested scheduler additions.  [CSCdi37542]
 *
 * Revision 2.4  1995/06/28 09:18:47  smackie
 * Finally make subsystem sequencing robust with multiple hierarchy
 * sequences. Tidy up subsystem code into the bargain and split the
 * parser code into a new file. Make the subsystem code far noisier
 * about bogusness. (CSCdi23568)
 *
 * Revision 2.3  1995/06/26  23:03:12  hampton
 * Catch and complain about calls to printf that are made while interrupts
 * are disabled.  [CSCdi36437]
 *
 * Revision 2.2  1995/06/09 13:12:16  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:58:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../os/msg_schedtest.c"	/* Not a typo, see logger.h */
#include "../ui/common_strings.h"
#include "sched_private.h"
#include "config.h"
#include "parser.h"
#include "signal.h"
#include "subsys.h"
#include "free.h"
#include "ttysrv.h"
#include "mgd_timers.h"
#include "../util/random.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_scheduler.h"
#include "../os/clock.h"
#include "../os/timer_generic.h"

/*
 * Scheduler TEST commands
 */
#define ALTERNATE NONE
#include        "exec_test_scheduler.h"
LINK_POINT      (test_scheduler_commands, ALTERNATE);
#undef  ALTERNATE

#define INDEX_A 0x80000001
#define INDEX_B 0x80000002
#define INDEX_Q 0x80000003

static boolean test_bool_a;
static boolean test_bool_b;
static boolean test_bool_quit;
static int loop_count;
static mgd_timer stest_managed_timer;
static queuetype dummy;
static semaphore thesem;
static stest_bitfield_flag;
static sys_timestamp stest_simple_timer;
static watched_boolean *test_wbool_a = NULL;
static watched_boolean *test_wbool_b = NULL;
static watched_boolean *test_wbool_quit = NULL;
static watched_queue *dummy_wqueue = NULL;
static watched_bitfield *stest_w_bitfield = NULL;
static watched_semaphore *stest_w_semaphore = NULL;

extern void show_one_proc_all_events(sprocess *);


boolean stest_load_block (void)
{
    if (test_bool_quit)
	return(FALSE);
    if (!QUEUEEMPTY(&dummy))
	return(FALSE);
    return(TRUE);
}

boolean stest_proc1_block (void)
{
    return(!test_bool_a);
}

boolean stest_proc2_block (void)
{
    return(!test_bool_b);
}

void stest_load (void)
{
    while (!test_bool_quit)
	edisms((blockproc*)stest_load_block, 0);
    process_kill(THIS_PROCESS);
}

void stest_proc1 (void)
{
    char str[80];
    int i;

    GET_TIMESTAMP(stest_simple_timer);
    for (i = 0; i < loop_count; i++) {
	edisms((blockproc *)stest_proc1_block, 0);
	test_bool_a = FALSE;
	test_bool_b = TRUE;
    }
    sprint_dhms(str, stest_simple_timer);
    printf("\nElapsed time for %d loops: %s\n", loop_count, str);
    flush();
    test_bool_quit = TRUE;
    process_kill(THIS_PROCESS);
}

void stest_proc2 (void)
{
    int i;

    for (i = 0; (i < loop_count) && !test_bool_quit; i++) {
	edisms((blockproc *)stest_proc2_block, 0);
	test_bool_b = FALSE;
	test_bool_a = TRUE;
    }
    process_kill(THIS_PROCESS);
}

static process stest_new_load (void)
{
    process_watch_queue(dummy_wqueue, ENABLE, RECURRING);
    process_watch_boolean(test_wbool_quit, ENABLE, RECURRING);
    process_wait_for_event();
    process_kill(THIS_PROCESS);
}

static process stest_new_proc1 (void)
{
    char str[80];
    int i;

    /*
     * Setup
     */
    process_watch_boolean(test_wbool_a, ENABLE, RECURRING);

    /*
     * Start the test
     */
    for (i = 0; i < loop_count; i++) {
	process_wait_for_event();
	process_set_boolean(test_wbool_a, FALSE);
	process_set_boolean(test_wbool_b, TRUE);
    }
    sprint_dhms(str, stest_simple_timer);
    printf("\nElapsed time for %d loops: %s\n", loop_count, str);
    flush();
    process_set_boolean(test_wbool_quit, TRUE);

    /*
     * Wait for the other processes to go away and then clean up.
     */
    process_sleep_for(ONESEC);
    process_kill(THIS_PROCESS);
}

static process stest_new_proc2 (void)
{
    int i;

    process_watch_boolean(test_wbool_b, ENABLE, RECURRING);
    process_watch_boolean(test_wbool_quit, ENABLE, RECURRING);
    for (i = 0; (i < loop_count) && !WB_VALUE(test_wbool_quit); i++) {
	process_wait_for_event();
	process_set_boolean(test_wbool_b, FALSE);
	process_set_boolean(test_wbool_a, TRUE);
    }
    process_kill(THIS_PROCESS);
}

static pid_t exit_pid;

void stest_exit_handler (int signal, int dummy1, void *dummy2, char *dummy3)
{
    errmsg(&msgsym(EXITHOOK, SCHED_TEST), forkx->name);
}

static process stest_exit_a (void)
{
    signal_permanent(SIGEXIT, stest_exit_handler);
    process_sleep_for(ONESEC);
    process_kill(exit_pid);
    process_kill(THIS_PROCESS);
}

static process stest_exit_b (void)
{
    signal_permanent(SIGEXIT, stest_exit_handler);
    process_sleep_for(5*ONESEC);
    process_kill(THIS_PROCESS);
}

static volatile int stest_wd_test;

void stest_watchdog_handler (int signal, int dummy1, void *dummy2, char *dummy3)
{
    printf("\nWatchdog handler for process %s.", forkx->name);
    flush();
    stest_wd_test++;
}

static process stest_watchdog_non_fatal (void)
{
    signal_permanent(SIGWDOG, stest_watchdog_handler);
    watchdog_time = 5 * ONESEC;
    stest_wd_test = 1;

    printf("\n This watchdog timeout should be caught...");
    flush();
    process_sleep_for(ONESEC);
    while (stest_wd_test == 1)
	;

    printf("\n As should this one...");
    flush();
    process_sleep_for(ONESEC);
    while (stest_wd_test == 2)
	;

    printf("\n This line should never be reached.");
    flush();
    process_sleep_for(ONESEC);
}

static process stest_watchdog_fatal (void)
{
    watchdog_time = 5 * ONESEC;

    printf("\n This watchdog timeout should be fatal...");
    flush();
    process_sleep_for(ONESEC);
    stest_wd_test = 1;
    while (stest_wd_test == 1)
	;
}

static process stest_print_routine (void)
{
    leveltype level;

    process_set_ttysoc(THIS_PROCESS, console);
    printf("\nThis printf should not produce an error.");

    printf("\nThe next printf (interrupt level) should produce an error.");
    flush();
    process_sleep_for(ONESEC/2);
    level = raise_interrupt_level(ALL_DISABLE);
    printf("\nBoo.");
    reset_interrupt_level(level);
    logger_flush();

    printf("\nThe next printf (no tty) should produce an error.");
    flush();
    process_sleep_for(ONESEC/2);
    process_set_ttysoc(THIS_PROCESS, NULL);
    printf("\nThis printf should produce an error.");
    logger_flush();

    buginf("\nThis buginf should not produce an error.");
    logger_flush();

    errmsg(&msgsym(ERRMSG,SCHED_TEST));  
    logger_flush();

    process_kill(THIS_PROCESS);
}

static process stest_semaphore_proc (void)
{
    boolean result;
    ulong i, major, minor;

    result = process_lock_semaphore(stest_w_semaphore, 10*ONESEC);
    printf("\n  Second watched semaphore lock was %ssuccessful (%s)",
	   result ? "": "un", result ? "failed" : "passed");
    flush();
    process_set_boolean(test_wbool_a, TRUE);

    /* Wait to cycle around for the next try */
    process_watch_boolean(test_wbool_b, ENABLE, RECURRING);
    while (process_get_wakeup(&major, &minor)) {
	printf("\n  **Test process had a %s (%d) event waiting.",
	       event_type2text(major), minor);
    }
    process_wait_for_event();
    process_set_boolean(test_wbool_b, FALSE);
    process_watch_boolean(test_wbool_b, DISABLE, RECURRING);

    /* Now continue */
    result = process_lock_semaphore(stest_w_semaphore, 10*ONESEC);
    printf("\n  Second watched semaphore lock was %ssuccessful (%s)",
	   result ? "": "un", result ? "failed" : "passed");
    flush();
    process_set_boolean(test_wbool_a, TRUE);


    /* Once more */
    for (i = 0; i < 160; i++) {
	result = process_lock_semaphore(stest_w_semaphore, MAXULONG);
	printf(result ? "T" : "t");
	flush();
	if (random_gen() & 1)
	    process_suspend();
	process_unlock_semaphore(stest_w_semaphore);
	if (random_gen() & 1)
	    process_suspend();
    }
    process_kill(THIS_PROCESS);
}

static void stest_semaphore (void)
{
    pid_t pid;
    boolean result;
    ulong i, major, minor;

    printf("\nBasic semaphore primitives.");

    thesem = FALSE;
    result = lock_semaphore(&thesem);
    printf("\n  Initial semaphore lock was %ssuccessful (%s)",
	   result ? "" : "un", result ? "passed" : "failed");
    
    result = lock_semaphore(&thesem);
    printf("\n  Second semaphore lock was %ssuccessful (%s)",
	   result ? "": "un", result ? "failed" : "passed");
    unlock_semaphore(&thesem);
    printf("\n  Semaphore unlocked");
    result = lock_semaphore(&thesem);
    printf("\n  Third semaphore lock was %ssuccessful (%s)",
	   result ? "": "un", result ? "passed" : "failed");
    unlock_semaphore(&thesem);
    printf("\n  Semaphore unlocked");
    flush();

    printf("\nBasic watched semaphore primitives.");
    result = process_lock_semaphore(stest_w_semaphore, 10*ONESEC);
    printf("\n  Initial watched semaphore lock was %ssuccessful (%s)",
	   result ? "": "un", result ? "passed" : "failed");
    flush();
    result = process_lock_semaphore(stest_w_semaphore, 10*ONESEC);
    printf("\n  Second watched semaphore lock was %ssuccessful (%s)",
	   result ? "": "un", result ? "failed" : "passed");
    flush();
    process_unlock_semaphore(stest_w_semaphore);
    printf("\n  Watched semaphore unlocked");
    flush();
    result = process_lock_semaphore(stest_w_semaphore, 10*ONESEC);
    printf("\n  Third watched semaphore lock was %ssuccessful (%s)",
	   result ? "": "un", result ? "passed" : "failed");
    flush();
    process_unlock_semaphore(stest_w_semaphore);
    printf("\n  Watched semaphore unlocked");

    pid = process_create(stest_semaphore_proc, "Stest semaphore", NORMAL_STACK,
			 PRIO_NORMAL);
    if (pid == NO_PROCESS)
	return;
    if (!process_set_ttysoc(pid, stdio))
	return;

    /******/

    printf("\nTwo-Process watched semaphore primitives.");
    result = process_lock_semaphore(stest_w_semaphore, 10*ONESEC);
    printf("\n  Initial watched semaphore lock was %ssuccessful (%s)",
	   result ? "": "un", result ? "passed" : "failed");
    flush();
    /* Let other process try for the lock */
    process_watch_boolean(test_wbool_a, ENABLE, RECURRING);
    while (process_get_wakeup(&major, &minor)) {
	printf("\n  **Exec process had a %s (%d) event waiting.",
	       event_type2text(major), minor);
    }
    process_wait_for_event();
    process_set_boolean(test_wbool_a, FALSE);
    process_watch_boolean(test_wbool_a, DISABLE, RECURRING);
    /* Now continue */
    process_unlock_semaphore(stest_w_semaphore);
    printf("\n  Watched semaphore unlocked");
    flush();
    result = process_lock_semaphore(stest_w_semaphore, 10*ONESEC);
    printf("\n  Third watched semaphore lock was %ssuccessful (%s)",
	   result ? "": "un", result ? "passed" : "failed");
    flush();
    process_unlock_semaphore(stest_w_semaphore);
    printf("\n  Watched semaphore unlocked");

    /******/

    printf("\nTwo-Process watched semaphore primitives. (again)");
    result = process_lock_semaphore(stest_w_semaphore, 10*ONESEC);
    printf("\n  Initial watched semaphore lock was %ssuccessful (%s)",
	   result ? "": "un", result ? "passed" : "failed");
    flush();
    /* Let other process try for the lock */
    process_set_boolean(test_wbool_b, TRUE);
    process_watch_boolean(test_wbool_a, ENABLE, RECURRING);
    while (process_get_wakeup(&major, &minor)) {
	printf("\n  **Exec process had a %s (%d) event waiting.",
	       event_type2text(major), minor);
    }
    process_wait_for_event();
    process_set_boolean(test_wbool_a, FALSE);
    process_watch_boolean(test_wbool_a, DISABLE, RECURRING);
    /* Now continue */
    process_unlock_semaphore(stest_w_semaphore);
    printf("\n  Watched semaphore unlocked");
    flush();
    result = process_lock_semaphore(stest_w_semaphore, 10*ONESEC);
    printf("\n  Third watched semaphore lock was %ssuccessful (%s)",
	   result ? "": "un", result ? "passed" : "failed");
    flush();
    process_unlock_semaphore(stest_w_semaphore);
    printf("\n  Watched semaphore unlocked");

    /******/

    printf("\nTwo-Process watched semaphore primitives. (yet again)\n");
    for (i = 0; i < 160; i++) {
	result = process_lock_semaphore(stest_w_semaphore, MAXULONG);
	printf(result ? "E" : "e");
	flush();
	if (random_gen() & 1)
	    process_suspend();
	process_unlock_semaphore(stest_w_semaphore);
	if (random_gen() & 1)
	    process_suspend();
    }

    process_sleep_for(10*ONESEC);
}

process stest_bitfield_watcher (void)
{
    ulong bits;

    bits = process_get_bitfield(stest_w_bitfield);
    printf("Initial value is: %08x\n", bits);

    process_watch_bitfield(stest_w_bitfield, ENABLE, RECURRING);
    while (stest_bitfield_flag) {
	process_wait_for_event();
	bits = process_get_bitfield(stest_w_bitfield);
	printf("Bitfield value is: %08x\n", bits);
    }
    process_kill(THIS_PROCESS);
}

static void stest_bitfield (void)
{
    pid_t pid;

    stest_bitfield_flag = TRUE;
    pid = process_create(stest_bitfield_watcher, "stest bit watcher",
			 NORMAL_STACK, PRIO_NORMAL);
    process_set_ttysoc(pid, stdio);
    process_suspend();
    
    process_set_bitfield(stest_w_bitfield, 0xAAAAAAAA, TRUE);
    printf("Set bits 0xAAAAAAAA w/ wakeup.\n");
    process_suspend();

    process_keep_bitfield(stest_w_bitfield, 0x0A0A0A0A, TRUE);
    printf("Kept bits 0x0A0A0A0A w/ wakeup.\n");
    process_suspend();

    process_set_bitfield(stest_w_bitfield, 0x50505050, FALSE);
    printf("Set bits 0x50505050 w/o wakeup.\n");
    process_suspend();

    stest_bitfield_flag = FALSE;
    process_clear_bitfield(stest_w_bitfield, 0x0A0A0A0A, TRUE);
    printf("Cleared bits 0x0A0A0A0A w/ wakeup.\n");
    process_suspend();
}

process stest_stuck_timer_process (void)
{
    sys_timestamp exit;

    mgd_timer_start(&stest_managed_timer, 0);
    GET_TIMESTAMP(stest_simple_timer);

    /*
     * Test stuck simple timer.
     */
    process_watch_timer(&stest_simple_timer, ENABLE);
    TIMER_START(exit, 65*ONESEC);
    while (SLEEPING(exit)) {
	process_wait_for_event(); /* Should get two warnings in 65 seconds */
    }
    process_watch_timer(&stest_simple_timer, DISABLE);
    process_suspend();

    /*
     * Test stuck managed timer.
     */
    process_watch_mgd_timer(&stest_managed_timer, ENABLE);
    TIMER_START(exit, 65*ONESEC);
    while (SLEEPING(exit)) {
	process_wait_for_event(); /* Should get two warnings in 65 seconds */
    }
    process_watch_mgd_timer(&stest_managed_timer, DISABLE);
    mgd_timer_stop(&stest_managed_timer);

    process_sleep_for(30*ONESEC);
    process_kill(THIS_PROCESS);
}

process stest_library (void)
{
    pid_t pid;

    /*
     * Setup the process' event list.
     */
    process_watch_boolean(test_wbool_a, ENABLE, RECURRING);
    process_watch_boolean(test_wbool_quit, ENABLE, RECURRING);


    /*
     * Setup the library event list.
     */
    if (!process_push_event_list(NULL)) {
	printf("process_push_event_list call failed.\n");
	process_kill(THIS_PROCESS);
    }
    process_watch_boolean(test_wbool_b, ENABLE, RECURRING);
    process_watch_bitfield(stest_w_bitfield, ENABLE, RECURRING);
    process_watch_queue(dummy_wqueue, ENABLE, RECURRING);

    /*
     * Instruct user to issue "show process" command, and then wait.
     */
    process_get_pid(&pid);
    process_wait_for_event_timed(2*ONEMIN);
    printf("\nLibrary test process awake: caller %s events.",
	   process_caller_has_events() ? "has" : "does not have");

    /*
     * Clean Up after the library.
     */
    if (!process_pop_event_list(NULL))
	printf("process_pop_event_list call failed.\n");

    /*
     * Instruct user to issue "show process" command, and then wait.
     */
    while (process_get_boolean(test_wbool_quit) == FALSE)
	process_wait_for_event_timed(2*ONESEC);
    process_kill(THIS_PROCESS);
}

#define MAXLONGLONG   0x7FFFFFFFFFFFFFFFLL
#define MAXULONGLONG   0xFFFFFFFFFFFFFFFFLL

static ulonglong stest_add(ulonglong, ulonglong);
static ulonglong stest_sub(ulonglong, ulonglong);
static ulonglong stest_mul(ulonglong, ulonglong);
static ulonglong stest_div(ulonglong, ulonglong);

static void stest_math (void)
{
    ulonglong a, b, c;

    printf("\nTesting 64 bit math:\n");

    a = 0xFFFFFFFF;
    b = 1;
    c = stest_add(a, b);
    printf("  000 FFFFFFFF + 000 00000001 = 001 00000000 : %-12llx\n", c);

    a = 0xFFFFFFFF;
    b = 0xFFFFFFFF;
    c = stest_add(a, b);
    printf("  000 FFFFFFFF + 000 FFFFFFFF = 001 FFFFFFFE : %-12llx\n", c);

    a = 0x123FFFFFFFFLL;
    b = 0x1230FFFFFFFLL;
    c = stest_sub(a, b);
    printf("  123 FFFFFFFF - 123 0FFFFFFF = 000 F0000000 : %-12llx\n", c);

    a = 0xFFFFFFFF;
    b = 6;
    c = stest_mul(a, b);
    printf("  000 FFFFFFFF * 000 00000006 = 005 FFFFFFFA : %-12llx\n", c);

    a = c;
    b = 3;
    c = stest_div(a, b);
    printf("  005 FFFFFFFA / 000 00000003 = 001 FFFFFFFE : %-12llx\n", c);

    a = c;
    b = 2;
    c = stest_div(a, b);
    printf("  001 FFFFFFFE / 000 00000003 = 000 FFFFFFFF : %-12llx\n", c);
}

static ulonglong stest_add(ulonglong a, ulonglong b)
{
    return(a + b);
}

static ulonglong stest_sub(ulonglong a, ulonglong b)
{
    return(a - b);
}

static ulonglong stest_mul(ulonglong a, ulonglong b)
{
    return(a * b);
}

static ulonglong stest_div(ulonglong a, ulonglong b)
{
    return(a / b);
}

static void stest_print_timestamps (void)
{
    sys_timestamp zero, second, minute, hour, day, week, month, year,
	sign, limit;
    uchar	a = 1;
    ushort	b = 2;
    ulong	c = 3;
    ulonglong	d = 4;

    printf("\nTesting argument widths.  The numbers should "
	   "read 4, 3, 2, 1.\n");
    printf("Arg Test: Longlong %lld, Long %ld, Short %d, Char %d\n\n",
	   d, c, b, a);

    d =  0x123456789abcdef0LL;
    printf("This number should print as 0x123456789ABCDEF0: %llx\n\n", d);

    printf("Printing various timestamps:\n");
    printf("Now:  %-10s %-13s %-10s %-13s %-13s %-13s\n",
	   "%TN", "%lTN", "%#TN", "%Tn", "%lTn", "%#Tn");
    printf("      %-10TN %-13lTN %-#10TN %-13Tn %-13lTn %-#13Tn\n\n");

    TIMER_STOP(zero);
    TIMER_START(second, ONESEC+50);
    TIMER_START(minute, ONEMIN+50);
    TIMER_START(hour,   ONEHOUR+50);
    TIMER_START(day,    24*ONEHOUR+ONESEC);
    TIMER_START(week,   7*24*ONEHOUR+ONESEC);
    TIMER_START64(month,(ulonglong)31*24*ONEHOUR+ONESEC);
    TIMER_START64(year, (ulonglong)366*24*ONEHOUR+ONESEC);
    TIMER_START64(sign,  MAXLONGLONG - ONEMIN);
    TIMER_STOP(limit);   TIMER_SUB_DELTA(limit, ONEMIN);

    printf("Futu: %-10s %-13s %-10s %-13s %-13s %-13s\n",
	   "%TF", "%lTF", "%#TF", "%Tf", "%lTf", "%#Tf");
    printf("Zero: %-10TF %-13lTF %-#10TF %-13Tf %-13lTf %-#13Tf\n",
	   zero, zero, zero, zero, zero, zero);
    printf(" Sec: %-10TF %-13lTF %-#10TF %-13Tf %-13lTf %-#13Tf\n",
	   second, second, second, second, second, second);
    printf(" Min: %-10TF %-13lTF %-#10TF %-13Tf %-13lTf %-#13Tf\n",
	   minute, minute, minute, minute, minute, minute);
    printf(" Hr:  %-10TF %-13lTF %-#10TF %-13Tf %-13lTf %-#13Tf\n",
	   hour, hour, hour, hour, hour, hour);
    printf(" Day: %-10TF %-13lTF %-#10TF %-13Tf %-13lTf %-#13Tf\n",
	   day, day, day, day, day, day);
    printf(" Wk:  %-10TF %-13lTF %-#10TF %-13Tf %-13lTf %-#13Tf\n",
	   week, week, week, week, week, week);
    printf(" Mth: %-10TF %-13lTF %-#10TF %-13Tf %-13lTf %-#13Tf\n",
	   month, month, month, month, month, month);
    printf(" Yr:  %-10TF %-13lTF %-#10TF %-13Tf %-13lTf %-#13Tf\n",
	   year, year, year, year, year, year);
    printf("Sign: %-13TF %-13lTF %-#10TF %-13Tf %-13lTf %-#13Tf\n\n",
	   sign, sign, sign, sign, sign, sign);
    printf(" Max: %-13TF %-13lTF %-#10TF %-13Tf %-13lTf %-#13Tf\n\n",
	   limit, limit, limit, limit, limit, limit);

    TIMER_START64(second, -ONESEC);
    TIMER_START64(minute, -ONEMIN);
    TIMER_START64(hour,   -ONEHOUR);
    TIMER_START64(day,    -24*ONEHOUR);
    TIMER_START64(week,   -7*24*ONEHOUR);
    TIMER_START64(month,  (longlong)-31*24*ONEHOUR);
    TIMER_START64(year,   (longlong)-366*24*ONEHOUR+500);
    TIMER_START64(sign,   -MAXLONGLONG);
    TIMER_START64(limit,  ONEMIN);

    printf("Past: %-10s %-13s %-10s %-13s %-13s %-13s\n",
	   "%TE", "%lTE", "%#TE", "%Te", "%lTe", "%#Te");
    printf("Zero: %-10TE %-13lTE %-#10TE %-13Te %-13lTe %-#13Te\n",
	   zero, zero, zero, zero, zero, zero);
    printf(" Sec: %-10TE %-13lTE %-#10TE %-13Te %-13lTe %-#13Te\n",
	   second, second, second, second, second, second);
    printf(" Min: %-10TE %-13lTE %-#10TE %-13Te %-13lTe %-#13Te\n",
	   minute, minute, minute, minute, minute, minute);
    printf(" Hr:  %-10TE %-13lTE %-#10TE %-13Te %-13lTe %-#13Te\n",
	   hour, hour, hour, hour, hour, hour);
    printf(" Day: %-10TE %-13lTE %-#10TE %-13Te %-13lTe %-#13Te\n",
	   day, day, day, day, day, day);
    printf(" Wk:  %-10TE %-13lTE %-#10TE %-13Te %-13lTe %-#13Te\n",
	   week, week, week, week, week, week);
    printf(" Mth: %-10TE %-13lTE %-#10TE %-13Te %-13lTe %-#13Te\n",
	   month, month, month, month, month, month);
    printf(" Yr:  %-10TE %-13lTE %-#10TE %-13Te %-13lTe %-#13Te\n",
	   year, year, year, year, year, year);
    printf("Sign: %-13TE %-13lTE %-#10TE %-13Te %-13lTe %-#13Te\n\n",
	   sign, sign, sign, sign, sign, sign);
    printf(" Max: %-13TE %-13lTE %-#10TE %-13Te %-13lTe %-#13Te\n\n",
	   limit, limit, limit, limit, limit, limit);
}

static void stest_delay (void)
{
    ulong start, oneus, onems, ones, tens;
#ifndef SAPPHIRE
    ulong spin, spincount;
#endif

    printf("Please wait while checking delays.\n");
    flush();
    process_sleep_for(ONESEC);

#ifndef SAPPHIRE
    spincount = timer_calibrate(1000, FALSE);
    start = clock_get_microsecs();
    WASTETIME(spincount);
    spin = clock_get_microsecs() - start;
#endif

    start = clock_get_microsecs();
    usecdelay(1);
    oneus = clock_get_microsecs() - start;

    start = clock_get_microsecs();
    msecdelay(1);
    onems = clock_get_microsecs() - start;

    start = clock_get_microsecs();
    msecdelay(ONESEC);
    ones = clock_get_microsecs() - start;

    start = clock_get_microsecs();
    msecdelay(10*ONESEC);
    tens = clock_get_microsecs() - start;

    printf("\nRequested\tActual usec");
    printf("\n1us\t\t%d", oneus);
    printf("\n1ms\t\t%d", onems);
#ifndef SAPPHIRE
    printf("\n1ms spin\t%d", spin);
#else
    printf("\n1ms spin\tcannot calibrate");
#endif
    printf("\n1s\t\t%d",  ones);
    printf("\n10s\t\t%d", tens);
    printf("\nIgnore the CPUHOG message.\n");
}

typedef struct stest_q_element_t {
    struct stest_q_element_t *next;
} stest_queue_element;

process stest_thrash_process (void)
{
    sys_timestamp exit;
    ulong type, value;
    uchar *pointer;
    pid_t pid;
    stest_queue_element element;

    mgd_timer_start(&stest_managed_timer, 0);
    GET_TIMESTAMP(stest_simple_timer);

    /*
     * Test simple timer thrashing.
     */
    process_watch_timer(&stest_simple_timer, ENABLE);
    TIMER_START(exit, ONEMIN);
    while (SLEEPING(exit)) {
	process_wait_for_event();
	TIMER_UPDATE(stest_simple_timer, 1);
    }
    process_watch_timer(&stest_simple_timer, DISABLE);
    process_suspend();

    /*
     * Test managed timer thrashing.
     */
    process_watch_mgd_timer(&stest_managed_timer, ENABLE);
    TIMER_START(exit, ONEMIN);
    while (SLEEPING(exit)) {
	process_wait_for_event();
	mgd_timer_update(&stest_managed_timer, 1);
    }
    process_watch_mgd_timer(&stest_managed_timer, DISABLE);
    mgd_timer_stop(&stest_managed_timer);
    process_suspend();

    /*
     * Test message thrashing
     */
    process_get_pid(&pid);
    process_send_message(pid, 0, NULL, 0);
    TIMER_START(exit, ONEMIN);
    while (SLEEPING(exit)) {
	process_wait_for_event();
    }
    process_get_message(&type, (void **)&pointer, &value);
    process_suspend();

    /*
     * Test boolean thrashing
     */
    process_watch_boolean(test_wbool_a, ENABLE, RECURRING);
    process_set_boolean(test_wbool_a, TRUE);
    TIMER_START(exit, ONEMIN);
    while (SLEEPING(exit)) {
	process_wait_for_event();
    }
    process_watch_boolean(test_wbool_a, DISABLE, RECURRING);
    process_suspend();

    /*
     * Test queue thrashing
     */
    element.next = NULL;
    process_watch_queue(dummy_wqueue, ENABLE, RECURRING);
    process_enqueue(dummy_wqueue, &element);
    TIMER_START(exit, ONEMIN);
    while (SLEEPING(exit)) {
	process_wait_for_event();
    }
    process_watch_queue(dummy_wqueue, DISABLE, RECURRING);
    process_dequeue(dummy_wqueue);
    process_suspend();

    /*
     * Die!
     */
    process_kill(THIS_PROCESS);
}

void sched_test_command (parseinfo *csb)
{
    int i;
    pid_t pid;
    sprocess *proc;

    if (dummy_wqueue == NULL) {
	dummy_wqueue = create_watched_queue("Dummy queue", 0, 0);
	test_wbool_a = create_watched_boolean("A flag", INDEX_A);
	test_wbool_b = create_watched_boolean("B flag", INDEX_B);
	test_wbool_quit = create_watched_boolean("Quit flag", INDEX_Q);
	stest_w_bitfield = create_watched_bitfield("test bitfield", 0);
	stest_w_semaphore = create_watched_semaphore("Test Semaphore", 0);
    }
    if (!mgd_timer_initialized(&stest_managed_timer))
	mgd_timer_init_leaf(&stest_managed_timer, NULL, 0, NULL, FALSE);
    test_bool_quit = FALSE;
    test_bool_a = TRUE;
    test_bool_b = FALSE;
    loop_count = GETOBJ(int,2);

    switch ( GETOBJ(int,3) ) {
      case SCHED_TEST_BITFIELD:
	stest_bitfield();
	break;

      case SCHED_TEST_DEBUG:
	proc = process_pid2ptr(GETOBJ(int,1));
	if (proc)
	    proc->debug_events = GETOBJ(int,2);
	break;

      case SCHED_TEST_EXIT:
	printf("\nRunning exit test.");
	flush();
	process_create(stest_exit_a, "Exit Test A", NORMAL_STACK, PRIO_NORMAL);
	exit_pid = process_create(stest_exit_b, "Exit Test B", NORMAL_STACK,
				  PRIO_NORMAL);
	break;

      case SCHED_TEST_LIBRARY:
	pid = process_create(stest_library, "Library Call Test", NORMAL_STACK,
			     PRIO_NORMAL);
	process_set_ttysoc(pid, stdio);
	process_suspend();
	printf("Issuing the command \"show process events %d\".", pid);
	proc = process_pid2ptr(pid);
	show_one_proc_all_events(proc);

	printf("\n\nSetting Boolean A.  Should not cause a wakeup.", pid);
	process_set_boolean(test_wbool_a, TRUE);
	process_suspend();
	printf("\nIssuing the command \"show process events %d\".", pid);
	proc = process_pid2ptr(pid);
	show_one_proc_all_events(proc);

	printf("\n\nSetting Boolean B.  Should cause a wakeup.", pid);
	printf("\nProcess level should have already handled boolean A, or it");
	printf("\nshould be ready to run due to boolean A being TRUE.", pid);
	process_set_boolean(test_wbool_b, TRUE);
	process_suspend();
	printf("\nIssuing the command \"show process events %d\".", pid);
	proc = process_pid2ptr(pid);
	show_one_proc_all_events(proc);

	printf("\n\nProcess should exit.", pid);
	process_set_boolean(test_wbool_quit, TRUE);
	process_sleep_for(200);
	proc = process_pid2ptr(pid);
	if (proc) {
	    printf("\nThe process did not terminate.", pid);
	    printf("\nIssuing the command \"show process events %d\".", pid);
	    show_one_proc_all_events(proc);
	} else {
	    printf("\nThe process has terminated successfully.", pid);
	}
	break;

      case SCHED_TEST_NEW_PROC:
	test_wbool_a = create_watched_boolean("A flag", INDEX_A);
	test_wbool_b = create_watched_boolean("B flag", INDEX_B);
	test_wbool_quit = create_watched_boolean("Quit flag", INDEX_Q);
	dummy_wqueue = create_watched_queue("Dummy queue", 0, 0);
	for (i = 0; i < GETOBJ(int,1); i++)
	    process_create(stest_new_load, "New STEST Load", NORMAL_STACK,
			   PRIO_NORMAL);
	pid = process_create(stest_new_proc1, "New Test 1", NORMAL_STACK,
			     PRIO_NORMAL);
	process_set_ttysoc(pid, stdio);
	pid = process_create(stest_new_proc2, "New Test 2", NORMAL_STACK,
			     PRIO_NORMAL);
	process_set_ttysoc(pid, stdio);
	process_sleep_for(ONESEC);
	GET_TIMESTAMP(stest_simple_timer);
	process_set_boolean(test_wbool_a, TRUE);
	break;

      case SCHED_TEST_OLD_PROC:
	for (i = 0; i < GETOBJ(int,1); i++)
	    cfork(stest_load, 0, 0, "STEST Load", CONSOLE_LINENUM);
	cfork(stest_proc1, 0, 0, "Test 1", CONSOLE_LINENUM);
	cfork(stest_proc2, 0, 0, "Test 2", CONSOLE_LINENUM);
	break;

      case SCHED_TEST_PRINT_ROUTINE:
	process_create(stest_print_routine, "Printing Routine Test", NORMAL_STACK, PRIO_NORMAL);
	process_sleep_for(3*ONESEC);
	break;

      case SCHED_TEST_PRINT_TIMESTAMP:
	stest_print_timestamps();
	break;

      case SCHED_TEST_SEMAPHORE:
	stest_semaphore();
	break;

      case SCHED_TEST_WATCHDOG:
	printf("\nRunning fatal watchdog test.");
	flush();
	process_create(stest_watchdog_fatal, "F Watchdog Test", NORMAL_STACK,
		       PRIO_NORMAL);
	break;

      case SCHED_TEST_WATCHDOG_NF:
	printf("\nRunning non-fatal watchdog test.");
	flush();
	process_create(stest_watchdog_non_fatal, "NF Watchdog Test",
		       NORMAL_STACK, PRIO_NORMAL);
	break;

      case SCHED_TEST_STUCK_TIMER:
	printf("Creating 'stuck timer' test process.\n");
	flush();
	process_create(stest_stuck_timer_process, "stest stuck timer",
		       NORMAL_STACK, PRIO_HIGH);
	break;

      case SCHED_TEST_MATH:
	stest_math();
	break;

      case SCHED_TEST_DELAY:
	stest_delay();
	break;

      case SCHED_TEST_THRASHING:
	printf("Creating 'thrash' process.\n");
	flush();
	process_create(stest_thrash_process, "stest thrash",
		       NORMAL_STACK, PRIO_HIGH);
	break;

      default:
	break;
    }
}

void scheduler_trash_sp (int which)
{
    uchar array[10000];

    array[0] = '1';
    switch (which) {
      case SCHED_TRASH_EDISMS:
	edisms((blockproc*)return_false, 0);
	break;
      case SCHED_TRASH_SLEEP:
	process_sleep_for(ONESEC);
	break;
      case SCHED_TRASH_SUSPEND:
	process_suspend();
	break;
      case SCHED_TRASH_WAIT:
	process_wait_for_event_timed(ONESEC);
	break;	    
      default:
	process_suspend();
	break;
    }
}

void sched_trash_command (parseinfo *csb)
{
    int i;
    ulong *ptr;
    ulong options = GETOBJ(int,4);
    sprocess *proc = forkx;

    if (options & SCHED_TRASH_EVENTSET)
	proc->current_events->magic--;
    if (options & SCHED_TRASH_FORKX1)
	forkx--;
    if (options & SCHED_TRASH_FORKX2)
	forkx_2--;
    if (options & SCHED_TRASH_FORKX3)
	forkx_3--;
    if (options & SCHED_TRASH_RED) {
	ptr = rz_ptr(data_to_block((uchar*)proc->stack));
	*ptr = 0;
    }
    if (options & SCHED_TRASH_STACK)
	*proc->stack = 0;
    if (options & SCHED_TRASH_MEMORY) {
	ptr = (ulong*)data_to_block((uchar*)proc->stack);
	for (i = 0; i < (sizeof(blocktype)/4); i++)
	    *ptr++ = 12345678;
    }

    /*
     * These must be last.
     */
    if (options & SCHED_TRASH_SP) {
	scheduler_trash_sp(GETOBJ(int,5));
    } else {
	switch (GETOBJ(int,5)) {
	  case SCHED_TRASH_EDISMS:
	    edisms((blockproc*)return_false, 0);
	    break;
	  case SCHED_TRASH_SLEEP:
	    process_sleep_for(ONESEC);
	    break;
	  case SCHED_TRASH_SUSPEND:
	    process_suspend();
	    break;
	  case SCHED_TRASH_WAIT:
	    process_wait_for_event_timed(ONESEC);
	    break;	    
	  default:
	    process_suspend();
	    break;
	}
    }
}

void scheduler_test_init (subsystype *subsys)
{
    parser_add_commands(PARSE_ADD_TEST_CMD,&pname(test_scheduler_commands),
			"scheduler");
}


/*
 * Scheduler Test subsystem header
 */
#define SCHEDTEST_MAJVERSION 1
#define SCHEDTEST_MINVERSION 0
#define SCHEDTEST_EDITVERSION  1

SUBSYS_HEADER(schedtest, SCHEDTEST_MAJVERSION, SCHEDTEST_MINVERSION, SCHEDTEST_EDITVERSION,
	      scheduler_test_init, SUBSYS_CLASS_PROTOCOL,
	      NULL, NULL);
