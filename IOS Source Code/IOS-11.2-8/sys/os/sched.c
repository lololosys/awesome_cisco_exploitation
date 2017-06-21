/* $Id: sched.c,v 3.17.8.9 1996/08/28 13:04:15 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/sched.c,v $
 *------------------------------------------------------------------
 * sched.c  --  process scheduling
 *  
 * June 1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sched.c,v $
 * Revision 3.17.8.9  1996/08/28  13:04:15  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.17.8.8  1996/08/24  00:20:22  sberl
 * CSCdi66828:  process_create_common should validate start address
 * Branch: California_branch
 * Check that process start address is not NULL and it is not greater than
 * etext.
 *
 * Revision 3.17.8.7  1996/08/03  23:34:58  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.17.8.6  1996/06/11  19:19:48  mshannon
 * CSCdi45507:  sanity checking code required for memory corruptions
 * Branch: California_branch
 * Restore ukernel build using 'kernel_validblock_diagnose'
 *
 * Revision 3.17.8.5  1996/05/24  19:29:58  hampton
 * Improve the granularity of low memory corruption checks during
 * subsystem initialization.  [CSCdi58674]
 * Branch: California_branch
 *
 * Revision 3.17.8.4  1996/05/10  23:53:15  gstovall
 * CSCdi56792:  Need to remove unused files
 * Branch: California_branch
 * Nuke support for the Irix image, also stamp out the last of the CS500.
 *
 * Revision 3.17.8.3  1996/05/08  00:06:54  hampton
 * Add better semaphore errors and better code to wait for a semaphore.
 * The routine process_wait_for_event_timed() shouldn't post a TIMER_EVENT
 * when the temporary timer expires.  [CSCdi56999]
 * Branch: California_branch
 *
 * Revision 3.17.8.2  1996/03/27  21:52:18  hampton
 * Try to catch and report thrashing processes.  A few other minor
 * scheduler cleanups and enhancements.  [CSCdi50130]
 * Branch: California_branch
 *
 * Revision 3.17.8.1  1996/03/18  21:31:18  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.12.12.4  1996/03/16  07:21:12  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.12.12.3  1996/03/09  05:10:23  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.12.12.2  1996/03/07  10:19:44  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.12.12.1  1996/02/20  16:41:33  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.17  1996/02/28  09:33:30  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.16  1996/02/27  06:00:04  afactor
 * CSCdi47194:  Add cookie capability for jalapeno mechanical option
 *
 * Revision 3.15  1996/02/01  06:07:50  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.14  1996/01/25  23:52:35  hampton
 * Add support to the scheduler for nesting wakeup event lists.  This
 * should make it easier to convert portion of the code.  Also allow for
 * the creation of a detached list of watched events.  [CSCdi46566]
 *
 * Revision 3.13  1996/01/25  18:38:07  hampton
 * Fix per-process heap checking code.  Also convert it from conditional
 * compilation to an "internal" command.  [CSCdi47517]
 *
 * Revision 3.12  1996/01/04  07:38:43  hampton
 * Process_last_gasp() should call suspend() directly instead of via
 * process_suspend().  [CSCdi46405]
 *
 * Revision 3.11  1995/12/23  13:10:22  hampton
 * Fix a timing window where interfaces can be mistakenly throttled.
 * [CSCdi44951]
 *
 * Revision 3.10  1995/12/21  16:29:46  hampton
 * Switch the 68K version of DELAY() to call the existing usecdelay
 * routine which uses the timer chip.  For all other platforms, move a
 * routine or two between files.  [CSCdi45965]
 *
 * Revision 3.9  1995/12/19  00:40:51  hampton
 * Obsolete the GET_NONZERO_TIMESTAMP routine.  It is no longer necessary
 * with 64bit timestamps.  [CSCdi45208]
 *
 * Revision 3.8  1995/12/07  22:31:13  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.7  1995/12/03  17:58:24  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.6  1995/11/28  02:19:55  henniger
 * CSCdi38897:  Race condition in CPU HOG incorrectly flags non-hogging
 * process
 * in process_execute() assign last_int_count earlier.
 *
 * Revision 3.5  1995/11/18  06:31:40  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Setup a tt_soc with null input and output vectors.  Provide routine
 * which disables io for calling process by changing controlling tty to
 * the null_io_tty and returns previous controlling tty.  Provide routine
 * which restores io by setting the controlling tty to the tty passed in.
 *
 * Revision 3.4  1995/11/18  02:17:59  pst
 * CSCdi43519: Add ability to public code to query scheduler for
 * suspendability.
 *
 * Revision 3.3  1995/11/17  18:51:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:18:43  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:47:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.19  1995/11/08  21:19:20  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.18  1995/10/21  00:55:16  rramacha
 * CSCdi38236:  booting gs7 image on a ags results in a frozen router
 *
 * Revision 2.17  1995/09/08  16:16:55  hampton
 * Don't spin the random number generator as frequently from the
 * scheduler.  Fix test in signal_receive.  [CSCdi39965]
 *
 * Revision 2.16  1995/09/07 07:43:31  hampton
 * Set the clockptr variable as soon as possible.  It may need to be
 * referenced during platform initialization.  [CSCdi39878]
 *
 * Revision 2.15  1995/09/05 16:38:03  hampton
 * Fix autoinstall over frame relay.  [CSCdi38852]
 *
 * 1) Udp should only add one-shot queue watches inside of read_ipsocket.
 * 2) Allow a existing one-shot watch to be upgraded to a recurring watch,
 *    but prevent the downgrade of an existing recurring watch to a
 *    one-shot watch.
 * 3) Add some conditionally compiled debugging to the scheduler.
 *
 * Revision 2.14  1995/08/25 11:44:49  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.13  1995/07/24 09:24:05  hampton
 * Requested scheduler additions.  [CSCdi37542]
 *
 * Revision 2.12  1995/07/22 20:42:26  hampton
 * Reverse the order in which process_create() mallocs a process and its
 * stack.  Try to put the stack lower in memory so an overflow doesn't
 * wipe out the perpetrator's process data structure.  [CSCdi36985]
 *
 * Revision 2.11  1995/07/22 07:06:10  hampton
 * Check each event once per scheduler invocation, not once per call to
 * process_get_event.  This prevents a high input arrival rate from
 * causing a process to hog the processor.  [CSCdi37126]
 *
 * Revision 2.10  1995/07/18 06:48:24  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.9  1995/06/30 05:21:41  hampton
 * Move externs for the random number generator to their own file.  Remove
 * redundant initialization of the random number generator.  [CSCdi36588]
 *
 * Revision 2.8  1995/06/26 23:03:08  hampton
 * Catch and complain about calls to printf that are made while interrupts
 * are disabled.  [CSCdi36437]
 *
 * Revision 2.7  1995/06/21 09:24:32  hampton
 * CSCdi36157:  arizona rxboot fails to net/flash boot
 *
 * Revision 2.6  1995/06/19 06:48:56  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.5  1995/06/18  05:31:47  hampton
 * Close a small timing window in the scheduler's calls to the
 * mgd_timer_link and mgd_timer_delink routines.  [CSCdi35906]
 *
 * Revision 2.4  1995/06/15 22:11:54  dkatz
 * CSCdi35915:  Watchdog fires during slow flash boot
 * Don't allow the watchdog to fire if systeminit_flag is clear.
 *
 * Revision 2.3  1995/06/13  08:42:22  hampton
 * Fix a hang while writing out a core file.  [CSCdi35755]
 *
 * Revision 2.2  1995/06/09 13:12:14  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:58:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <reent.h>
#include <ciscolib.h>
#include "../dev/monitor1.h"

#include "subsys.h"
#include "config_register.h"
#include "../../boot/buff.h"
#include "../../boot/cpu.h"
#include "sched_private.h"
#include "stacks.h"
#include "logger.h"
#include "signal.h"
#include "ttysrv.h"
#include "exception.h"
#include "../os/sum.h"
#include "interface_private.h"			/* required by network.h */
#include "profile.h"
#include "async.h"
#include "config.h"
#include "mgd_timers.h"
#include "chunk.h"
#include "sys_registry.h"
#include "gdb.h"
#include "free.h"
#include "init.h"
#include "../if/network.h"
#include "parser_defs_scheduler.h"
#include "../util/random.h"
#include "../util/radix.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../os/msgtxt_sys.h"
#include "packet.h"			/* for data_enqueue() */

#define PROC_HISTORY_LIMIT 100

/*
 * Local Storage
 */
uint textsum;			/* sum of text segment */
int cpu_type;			/* CPU ID (used by delay and Print_Hardware) */
unsigned long cpu_family = 0;	/* cpu family type FAMILY_CSC2, FAMILY_PAN...*/
boolean ciscopro_platform = FALSE;	/* Is the box a CiscoPRo? */
boolean ciscoet_platform = FALSE;	/* Is the box a CiscoET?  */
char *version;			/* version string to use, based on OEM bit */
tt_soc *stdio;			/* pointer to current standard I/O terminal */
tt_soc *console;		/* pointer to console terminal data struct */
ushort configregister;		/* contents of configuration register */
uchar sched_debug_events;
boolean checkheaps_per_proc =FALSE; /* check after each process executes */
boolean checkheaps_per_poll =FALSE; /* check after each edisms test routine */
char *last_sysversion;		/* version string of previous image */
int startup_ttynum;		/* Terminal number to use for startup output */
boolean sysinit_process_started = FALSE;
boolean sched_run_degraded = FALSE;
static boolean sched_already_degraded = FALSE;

/*
 * The state of the router.
 */
boolean system_running = FALSE;	/* boolean for system continuation */
boolean system_loading;		/* boolean for running to load an image */
boolean system_configured;	/* boolean for system has read configuration */
boolean systeminit_flag;	/* TRUE when system is initialized */
sprocess *forkx;		/* current process */
queuetype sarrayQ;		/* active processes ptrs */
char *last_proc_name;		/* proc running when router crashed */
char *process_history[PROC_HISTORY_LIMIT];/* list of recently executed procs */
int process_history_index;	/* index into the previous list */

/*
 * Scheduler queues and the like.  (See also sched_compatibility.c)
 */
list_header *forkx_list;	/* list current process is executing from */
list_header procq_ready_c;	/* ready to run - Critical */
list_header procq_ready_h;	/* ready to run - High */
list_header procq_ready_m;	/* ready to run - Normal */
list_header procq_ready_l;	/* ready to run - Low */
list_header procq_idle;		/* Waiting processes */
list_header procq_dead;		/* Dead Processes */
queuetype   procq_corrupt;	/* Corrupted processes */
list_header s_notify_queue;	/* Notify these processes when enqueueing */
list_header s_notify_boolean;	/* Notify these processes when val changes */
list_header s_notify_bitfield;	/* Notify these processes when bit changes */
list_header s_notify_semaphore;	/* Notify these processes when freed */
chunk_type *message_chunks;
mgd_timer sched_master_timer;
mgd_timer sched_force_int_excl;
watched_item sched_dummy_timer;
watched_item sched_dummy_message;


/*
 * Affecting or affected by the main scheduler loop
 */
volatile boolean schedflag;	/* TRUE if in scheduler, FALSE otherwise */
    /*
     * Note that the setting of this flag is not an absolute guarantee of
     * which stack the processor is executing from.  Every attempt has
     * been made to make this flag as accurate as possible, but the
     * changing of execution stacks and the changing of this variable are
     * not atomic, and can not be made that way on all processors.  There
     * is always the chance that the processor will take an interrupt
     * between the two actions.  This problem may be solvable on the
     * R4000 processor by disabling all interrupts, but it cannot be
     * solved on a 68K processor because of the NMI interrupt.
     */

int sched_run_interval;    	/* sched should run this often, def == 0.0 secs */
sys_timestamp sched_run_time;	/* time for next run of scheduler */
int refresh_time = 0;		/* Clock refresh time (1 || 4MS) */
static ulong process_max_time;	/* Max allowed tick count for any process */
int sched_max_task_time;	/* A task should only run this long */
ulong process_timer;		/* Time left for this process */
boolean process_quantum_expired;/* Signal process that time's up */
boolean blocking_disabled;	/* TRUE if process doesn't expect to block */
sys_timestamp blocking_timer;   /* Used to unhang blocking_disabled */

/*
 * Software watchdog timer
 */
ulong watchdog_time;		/* Max time process can execute */
enum sched_watchdog_options sched_watchdog_action; /* User configured */
				/* action on watchdog barks */

/*
 * For computing loads
 */
static sys_timestamp interval_start; /* msclock at start of current interval */
ulong current_count;		/* # of ticks in scheduler in 5 seconds */
volatile ulong int_count;	/* Count of int. clock samples */
volatile ulong last_int_count;	/* Interrupt count at start of task */
ulong idle_count;		/* # of msec in 5 seconds when idle */
boolean idle_count_wired;	/* if TRUE something wired it, don't adjust */
long last_busy_percent_scaled;	/* busy time % * LOAD_SCALAR, last 5 secs */
long last_busy_percent;		/* above, but a true %age for SNMP */
long last_int_percent_scaled;	/* intrpt time % * LOAD_SCALAR, last 5 secs */
long avg_busy1_percent_scaled;	/* 1 min avg. busy time % * LOAD_SCALAR */
long avg_busy1_percent;		/* above, but a true %age for SNMP */
long avg_busy5_percent_scaled;	/* 5 min avg. busy time % * LOAD_SCALAR */
long avg_busy5_percent;		/* above, but a true %age for SNMP */

/*
 * Memory counting.
 */
ulong init_totmalloc;		/* How much memory malloced from *init* */
ulong init_totfree;		/* How much memory freed from *init* */
ulong init_totgetbufmem;	/* How much memory malloced from *init* */
ulong init_totretbufmem;	/* How much memory freed from *init* */
ulong sched_totmalloc;		/* How much memory malloced from *sched* */
ulong sched_totfree;		/* How much memory freed from *sched* */
ulong sched_totgetbufmem;	/* How much memory malloced from *sched* */
ulong sched_totretbufmem;	/* How much memory freed from *sched* */
ulong dead_totmalloc;		/* How much memory dead process malloced */
ulong dead_totfree;		/* How much memory dead processed freed */
ulong dead_totgetbufmem;	/* Buffers allocated by all dead procs */
ulong dead_totretbufmem;	/* Buffers returned by all dead procs */

/*
 * Variables needed for segregating the kernel from the rest of the
 * system.
 */
kernel_buginf_t kernel_buginf;
kernel_errmsg_t kernel_errmsg;
kernel_sched_loop_hook_t kernel_sched_loop_hook;
kernel_validblock_diagnose_t kernel_validblock_diagnose;

/*
 * Duplicates of certain scheduler variables.  Some process keeps
 * trashing memory and causing the scheduler to trip up.  These variables
 * are used to validate key data structures.
 *
 * DO NOT MOVE THESE VARIABLES.  They are purposefully some distance away
 * from their original declarations.
 *
 * DO NOT MODIFY THESE DECLARATIONS.  They are set up to force some
 * variables into the DATA segment and some into the BSS.  This further
 * separates the variables in memory and prevents accidental corruption
 * of both variables.
 */
sprocess *forkx_2 = NULL;	/* current process (data copy) */
sprocess *forkx_3;		/* current process (bss copy) */


/*
 * Local definitions
 */
static process loadometer(void);
#define STATIC static
#define INLINE inline
#define LOWTIMEOUT 15


/*
 * kernel_buginf_default
 *
 * This routine is invoked for debugging messages if no error message
 * facility has been compiled into the image.
 */
static void kernel_buginf_default (const char *msg, ...)
{
    return;	/* Do nothing. */
}

void kernel_set_buginf_hook (
    kernel_buginf_t new_hook)
{
    kernel_buginf = new_hook;
}

/*
 * kernel_errmsg_default
 *
 * This routine is invoked for error messages if no error message
 * facility has been compiled into the image.
 */
static void kernel_errmsg_default (const message *msg, ...)
{
    return;	/* Do nothing. */
}

void kernel_set_errmsg_hook (
    kernel_errmsg_t new_hook)
{
    kernel_errmsg = new_hook;
}

/*
 * kernel_validblock_diagnose_default
 *
 * This routine is used in CRASH_AND_BURN to report on the memory
 * corruption detected.  A dummy routine for modular kernel.
 */
static char *kernel_validblock_diagnose_default (mempool *mp, 
						 blocktype *bp, 
						 bad_code_t code)
{
    return(NULL);
}

void kernel_set_validblock_diagnose_hook (
    kernel_validblock_diagnose_t new_hook)
{
    kernel_validblock_diagnose = new_hook;
}

/*
 * kernel_sched_loop_end_default
 *
 * This routine is invoked for at the bottom of every complete pass of
 * the scheduler tables.
 */
static void kernel_sched_loop_end_default (boolean dummy)
{
    leveltype level;

    level = disable_interrupts();
    TIMER_START(sched_run_time, sched_run_interval);
    enable_interrupts(level);
}

void kernel_set_sched_loop_hook (
    kernel_sched_loop_hook_t new_hook)
{
    kernel_sched_loop_hook = new_hook;
}

/*
 * reload_system - Reload the system
 */

void reload_system (reload_type_t type)
{
    if (type == RELOAD_NORMAL) {
	(*kernel_errmsg)(&msgsym(RELOAD, SYS));
	reg_invoke_will_reload();
	flush();
	process_sleep_for(ONESEC * 3);
    }

    raise_interrupt_level(ALL_DISABLE);	/* Stop the world, I want to get off */

    reg_invoke_hardware_shutdown();

    while (TRUE) {
	mon_reload();			/* Loop just in case the ROMs don't */
    }					/* understand the reload function */
}

/*
 * obsolete_rom
 * Print out message about obsolete ROMs and reboot
 */

static void obsolete_rom (void)
{
    console_message("\nObsolete ROM software");
    while (TRUE) {
	mon_reload();			/* Loop just in case the ROMs don't */
    }					/* understand the reload function */
}

/*
 * print_restricted_rights
 * Amazing legalese required by the Federal Government so that they
 * will not have the right to steal the software.  This text has been
 * approved by Lawyers; do not alter it or change when it is emitted without
 * consulting them!
 */
 
static void print_restricted_rights (void)
{
    printf("\n              Restricted Rights Legend\n"
           "\nUse, duplication, or disclosure by the Government is"
           "\nsubject to restrictions as set forth in subparagraph"
           "\n(c) of the Commercial Computer Software - Restricted"
           "\nRights clause at FAR sec. 52.227-19 and subparagraph"
           "\n(c) (1) (ii) of the Rights in Technical Data and Computer"
           "\nSoftware clause at DFARS sec. 252.227-7013."
           "\n\n           cisco Systems, Inc."
           "\n           170 West Tasman Drive"
           "\n           San Jose, California 95134-1706\n\n\n");
}

void print_text_base_string (void)
{
    printf(TEXTBASE_STRING, TEXT_START, DATA_START);
}

/*
 * save_version_string
 *
 * Create the version string for this image and save it off
 * to the rom monitor.
 */
static void save_version_string (void)
{
    char* cp;
    char *ver_str;
    int oemlen, totlen;

    cp = mon_vstring_sys();
    if (cp != (char*) -1) {
	setstring(&last_sysversion, cp);

	/*
	 * Save our soul, ie., our version string.  VSTRING_BUFF_SIZE is
	 * the size of the buffer defined in the ROMMON to hold the
	 * version string.  We will always save the oemstring catenated
	 * with the text-base string because the resultant string is the
	 * smallest one that should fit within VSTRING_BUFF_SIZE and yet
	 * contain all the info we really need.
	 *
	 * To malloc our buffer, we will take the larger of the combined
	 * string length (= length of oemversion + length of the
	 * text-base string + a safety factor of 16 to account for the
	 * addresses that will get expanded out by the two %x format
	 * specifiers) and VSTRING_BUFF_SIZE.  If the combined string
	 * length exceeds VSTRING_BUFF_SIZE, we will store only the
	 * oemversion.  If even the oemversion length exceeds
	 * VSTRING_BUFF_SIZE, the ROM MON code will truncate the
	 * oemversion string.
	 */
	/*
	 * Calculate size of combined string.
	 */
	oemlen = strlen(oemversion);
	totlen = oemlen + strlen(TEXTBASE_STRING) + 16;
	/*
	 * Malloc largest possible buffer needed.
	 */
	ver_str = malloc(max(VSTRING_BUFF_SIZE, totlen));
	if (ver_str) {
	    strcpy(ver_str, oemversion);  /* Save oemversion */
	    sprintf(ver_str + oemlen, TEXTBASE_STRING,
		    TEXT_START, DATA_START);
	    if (strlen(ver_str) >= VSTRING_BUFF_SIZE) {
		/*
		 * Combined string does not fit within 
		 * VSTRING_BUFF_SIZE. Chop off text-base string.
		 */
		ver_str[oemlen] = '\0';
	    }
	    mon_vstring_sys_set(ver_str);
	    free(ver_str);
	}
	else  /* Malloc failure. Might as well save oemversion */
	    mon_vstring_sys_set(oemversion);
    }
}

/*
 * __main
 * A call to this function is generated by versions of gcc 2.x to do
 * global C++ constructor initializations for mixed C and C++
 * programs. We do not need to worry about C++ code right now. This
 * avoids the need of having -lgcc used for gcc 2.x compiles.
 */
void __main (void)
{
}

/*
 * main
 * The beginning of the world.
 * High priority process get a lot of attention.  They should be used only
 * if the activation of many other processes depends on their actions.
 * Normal priority process get normal attention and are intended
 * for interactive use.  No one ever hears from low priority processes,
 * so there better not be a human typist waiting on one.
 */
boolean main (boolean loading)
{
    int configreg_temp;
    uchar *bss;
    boolean result;
    ulong curtime;
    pid_t loadometer_pid;

    /*
     * At this point, all maskable interrupts are masked
     */

    /*
     * Initialize the BSS to zero:
     * BEWARE!!!  Anything previously declared in the BSS
     * is now zeroed and may need to be reinitialized!
     */
    for (bss = (uchar *)DATA_END; bss < (uchar *)_END; bss++)
	*bss = '\000';
    
    /*
     * Wipe out page zero here on platforms that have memory at address
     * zero.  This cannot be left in gdb_init_exceptions where other
     * parts of page zero are modified, because that routine is called
     * before the bss is cleared.
     */
#ifdef TRAP_PAGE_ZERO
    sched_save_page_zero();
#endif

    /*
     * Initialize kernel hooks to a known state before proceeding any
     * further.
     */
    kernel_buginf = kernel_buginf_default;
    kernel_errmsg = kernel_errmsg_default;
    kernel_sched_loop_hook = kernel_sched_loop_end_default;
    kernel_validblock_diagnose = kernel_validblock_diagnose_default;

    /*
     * Initialize the system clock.
     *
     * WARNING!!!  The system clock must not be used before this point,
     * as it will always return a value of zero.
     */
    if (!system_clock_init())
	obsolete_rom();
    
    /*
     * Set up our platform-independant interrupt handler basics
     */
    level_init();
    
    /*
     * Determine our processor and family type
     */
    cpu_type   = platform_get_value(PLATFORM_VALUE_CPU_TYPE);
    cpu_type  &= ~CPU_CISCOPRO_MASK;
    cpu_family = platform_get_value(PLATFORM_VALUE_FAMILY_TYPE);

    if (cpu_family != FAMTYPE) {
	ulong i;
	console_message("\nWrong system software for this hardware\n");
	for (i=1000000; i>0; i--);   /* DELAY() is processor dependent */
	/*
	 * reload_system() is platform specific. So use mon_reload()
	 * with loop just in case ROMs don't understand the reload
	 * function
	 */
	while (TRUE) {
	    mon_reload();	
        }
    }

    /*
     * Let the platforms have their wicked way
     */
    platform_main();

    /*
     * Calculate the text segment checksum.
     */
    textsum = checksum_text_segment();

    /*
     * Initialize the basic list toolkit. The memory management and
     * region manager depend on this.
     */
    list_init();

    /*
     * Initialize the region, memory and chunk managers
     */
    memory_init();

    /*
     * Initialize automatic elements for the list manager once
     * the memory and chunk managers are running
     */
    list_memory_init();
    
    /*
     * Initialize platform exception handling
     */
    platform_exception_init();

    /*
     * Install level for performing system initialization. All handlers
     * for interrupt levels above this point must be installed by now.
     */
    set_interrupt_level(LEVEL_CONSOLE);

    /*
     * Get boot configuration register
     */
    configreg_temp = mon_boot_confreg();
    if (configreg_temp == -1) {
	/*
	 * Old ROMs, can't do it
	 */
	configregister = (ushort) ~mon_getconfig();
    } else {
	/*
	 * Modern ROMs, use boot-time value
	 */
	configregister = (ushort) configreg_temp;
    }

    /*
     * Initialize version string.
     */
    version = (configregister & CFG_OEM) ? oemversion : ciscoversion;
    mon_vstring_sys_rom_set(version);
    system_loading = (boolean)loading;
    if (!system_loading)
	save_version_string();

    /*
     * Initialize I/O to and from the console only.
     */
    stdio = console_init();		/* init the console UART */
    console = stdio;			/* remember this special pointer */

    /*
     * Print our version herald and restricted rights banner.
     * Don't print any banner if we are netbooting.
     * Don't print restricted rights banner if we are OEM equipment.
     */
    if (!system_loading) {
	if (version == ciscoversion)
	    print_restricted_rights();
        printf("\n%s", version);
        print_text_base_string();
    }

    /*
     * Initialize random number generator
     */
    GET_TIMESTAMP32(curtime);
    random_init((long)curtime, (long)textsum);

    /*
     * Initialize the scheduler database.
     */
    result = sched_init(loading);
    systeminit_flag = FALSE;	/* not initialized yet */
    watched_system_init = create_watched_boolean("System Initialized", WB_SYSTEM_INIT);
    process_set_boolean(watched_system_init, FALSE);
    system_configured = FALSE;	/* no configuration yet */
    watched_system_config = create_watched_boolean("System Configured", WB_SYSTEM_CONFIG);
    process_set_boolean(watched_system_config, FALSE);
    system_crashed = create_watched_boolean("System Crash", WB_SYSTEM_CRASH);
    process_set_boolean(system_crashed, FALSE);
    critical_background_flag =
	create_watched_boolean("Critical Background", WB_CRITICAL_BACKGROUND);
    process_set_boolean(critical_background_flag, FALSE);
    loadometer_pid = process_create(loadometer, "Load Meter", SMALL_STACK,
				    PRIO_CRITICAL);

    /*
     * Discover the subsystems.  Activate only the SYSTEMINIT subsystem.
     * The rest will be activated by that subsystem.
     */
    if (result) {
	subsys_init();
	subsys_init_class(SUBSYS_CLASS_SYSINIT);
    }

    /*
     * Init registration mechanism
     */
    registry_init();

    /*
     * Any problems so far?  If so, there's no sense at all in
     * continuing.
     */
    if (!result || !sysinit_process_started || (loadometer_pid == NO_PROCESS)) {
	printf("\n\n%%SYSTEM-1-INITFAIL: Cannot initialize system.  "
	       "Not enough memory.\n\n");
	DELAY(2*ONESEC);
	mon_reload();
    }

    system_running = TRUE;		/* we are off and running */
    set_interrupt_level(ALL_ENABLE);	/* open up interrupts */

    scheduler();		/* Invoke the scheduler */

    reg_invoke_hardware_shutdown();

    restore_vectors();		/* Restore the system-changed vectors */
    return(TRUE);
}

/*
 * Put a static pattern into low memory and check it constantly.
 */
#ifdef TRAP_PAGE_ZERO
static ulong saved_rommon_vec_table[VECTORCOUNT_ROMMON];
static boolean saved_rommon_vec_valid = FALSE;
boolean sched_pagezero_enabled = TRUE;

static inline boolean sched_test_page_zero_inline (void)
{
    ulong *rommon_vec_table;
    boolean result = FALSE;

    if (!sched_pagezero_enabled || !saved_rommon_vec_valid)
	return(FALSE);
    for (rommon_vec_table = 0;
	 (ulong)rommon_vec_table < VECTORCOUNT_ROMMON * 4;
	 rommon_vec_table++) {
	if (*rommon_vec_table != ILLEGAL_INSTRUCTION) {
	    result = TRUE;
	    (*kernel_errmsg)(&msgsym(PAGEZERO, SCHED), forkx->name,
			     rommon_vec_table, *rommon_vec_table);
	    *rommon_vec_table = ILLEGAL_INSTRUCTION;
	}
    }
    return(result);
}

boolean sched_test_page_zero (void)
{
    return(sched_test_page_zero_inline());
}

void sched_restore_page_zero (void)
{
    int exception;
    ulong *rommon_vec_table = 0;

    if (!saved_rommon_vec_valid)
	return;
    saved_rommon_vec_valid = FALSE;
    for (exception = 0; exception < VECTORCOUNT_ROMMON; exception++) {
	rommon_vec_table[exception] = saved_rommon_vec_table[exception];
    }
}

void sched_save_page_zero (void)
{
    int exception;
    ulong *rommon_vec_table = 0;

    if (!sched_pagezero_enabled || saved_rommon_vec_valid)
	return;
    saved_rommon_vec_valid = TRUE;
    for (exception = 0; exception < VECTORCOUNT_ROMMON; exception++) {
	saved_rommon_vec_table[exception] = rommon_vec_table[exception];
	rommon_vec_table[exception] = ILLEGAL_INSTRUCTION;
    }
}
#else
boolean sched_test_page_zero (void)
{
    return(FALSE);
}
#endif


/*********************************************************************
 *
 *			DETERMINE SYSTEM LOAD
 *			 AND CPU UTILIZATION
 *
 *********************************************************************/

/*
 * compute_busy
 *
 * Compute the busy time in the last 5 second period, and the five minute,
 * exponentially-decayed busy time.  We are called from the scheduler every
 * 5 seconds.
 *
 *	average = ((average - interval) * exp (-t/C)) + interval
 * 
 *  where t is five sec and C is 5 min,  exp(-5/(60*5)) == .983 ~= 1007/1024
 *  where t is five sec and C is 1 min,  exp(-5/60)     == .920 ~= 942/1024
 *
 * The residuals are stored scaled by 1024 to reduce truncation error for
 * small values.  If we let A' = (average * 1024) and I' = (interval * 1024),
 * the equation becomes 
 *
 *      A' = ((A' - I') * exp (-t/C)) + (I') 
 */

static void compute_busy (void)
{
    sprocess_list *array;
    sprocess *pp;
    ulong actual_interval, i;
    ulong local_int_count, local_current_count;

    /* Grab all the stats up front to reduce race conditions. */

    actual_interval = ELAPSED_TIME(interval_start);
    local_int_count = int_count;
    int_count = last_int_count = 0;
    local_current_count = current_count;
    current_count = 0;
    GET_TIMESTAMP(interval_start);

    if (actual_interval == 0)
	actual_interval = 1;

    /*
     * Here's the conceptual formula.  The actual code is a little
     * different, to avoid truncation errors and overflow errors
     *
     * last_busy_percent = 100 * ((interval - idle)/ interval)
     */

    /* Scale the counts to milliseconds. */

    local_current_count *= refresh_time;
    idle_count = local_current_count;

    local_int_count *= refresh_time;
    if (local_current_count > actual_interval)
	local_current_count = actual_interval;
    if (local_int_count > actual_interval)
	local_int_count = actual_interval;

    /* Figure out the box-wide stats. */

    last_int_percent_scaled = ((local_int_count * 100 * LOAD_SCALAR) / 
			       actual_interval);
    last_busy_percent_scaled = (100 * LOAD_SCALAR * 
				(actual_interval - local_current_count)) / 
				    actual_interval;
    last_busy_percent = last_busy_percent_scaled / LOAD_SCALAR;	/* for SNMP */
    avg_busy5_percent_scaled = last_busy_percent_scaled + 
	(((avg_busy5_percent_scaled - last_busy_percent_scaled) * 1007)/1024);
    avg_busy5_percent = avg_busy5_percent_scaled / LOAD_SCALAR; /* for SNMP */
    avg_busy1_percent_scaled = last_busy_percent_scaled + 
	(((avg_busy1_percent_scaled - last_busy_percent_scaled) * 942)/1024);
    avg_busy1_percent = avg_busy1_percent_scaled / LOAD_SCALAR;	/* for SNMP */

    /* Now do it per task. */

    for (array = sarrayQ.qhead; array; array = array->sl_next) {
      for (i = 0; i < MAX_PROC_PER_ARRAY; ++i) {
	long last_5sec_pct, local_tick_count;
	pp = array->sl_process[i];
	if (!pp)
	    continue;		        /* empty slot */
	local_tick_count = pp->cpu_ticks_5sec;
	pp->cpu_ticks_5sec = 0;
	local_tick_count *= refresh_time;
	if (local_tick_count > actual_interval)
	    local_tick_count = actual_interval;
	last_5sec_pct = (local_tick_count * 100 * LOAD_SCALAR) / 
	    actual_interval;
	pp->pct_cpu_5sec = last_5sec_pct;
	pp->pct_cpu_5min = last_5sec_pct + 
	    (((pp->pct_cpu_5min - last_5sec_pct) * 1007) / 1024);
	pp->pct_cpu_1min = last_5sec_pct + 
	    (((pp->pct_cpu_1min - last_5sec_pct) * 942) / 1024);
      }
    }
}

/*
 * loadometer
 * Periodically measure the load averages.  We do this in a process so
 * that the timers are not constantly checked in the scheduler loop.
 */
static process loadometer (void)
{
    process_wait_on_system_init();

    /*
     * Run the calculation immediately to initialize.
     */
    compute_busy();

    while (TRUE) {
  	/*
  	 * Compute busy every five seconds.
	 */
	process_sleep_periodic(5*ONESEC);
	compute_busy();
    }
}

/*
 * Account for CPU used in the current task
 *
 * Called from the NMI thread when the system is in the process context
 * Return TRUE if the current process is causing a watchdog problem.
 *
 * Note that there is a slim chance that this routine can be called
 * from the scheduler context, due to a race condition with the setting
 * of schedflag.  It is disastrous for the watchdog to fire in this
 * scenario, since the game that the NMI routine plays with the stack
 * will fail miserably.
 *
 * We rely on proc->starttime being set before schedflag is set FALSE
 * to avoid this problem (since time_this_run will then equal zero).
 *
 * We also never fire the watchdog if systeminit_flag is clear, since
 * there are parts of the system that insist on not suspending if this
 * is the case.
 */
boolean count_process_tick(void)
{
    long time_this_run;
    long interrupt_time_this_run;
    long process_time_this_run;
    sprocess *proc;

    proc = forkx;
    if (proc == NULL)
	return(FALSE);
    proc->cpu_ticks_5sec++;

    if (--process_timer == 0)
	process_quantum_expired = TRUE;

    if (!watchdog_time)
	return(FALSE);

    time_this_run = ELAPSED_TIME(proc->starttime);
    if (time_this_run > watchdog_time) {
	interrupt_time_this_run = (int_count - last_int_count) *
	    refresh_time; 
	if (interrupt_time_this_run < 0) /* The task cleared it */
	    interrupt_time_this_run = 0; /* Close enough. */

	/*
	 * Calculate the time used as a signed value, so that the
	 * '>' test works even if for some bizarre reason we
	 * end up in a situation where the interrupt time is
	 * greater than the total.
	 */
	process_time_this_run = time_this_run -
	    interrupt_time_this_run; /* Signed so no surprises. */
	if ((process_time_this_run > watchdog_time) &&
	    systeminit_flag) {
	    GET_TIMESTAMP(proc->starttime); /* give it another */
	    /* chance while it dies*/
	    return(TRUE);
	}
    }
    return(FALSE);
}

/**********************************************************************
 *
 *		    PER-PROCESS MEMORY ACCOUNTING
 *
 **********************************************************************/

/*
 * process_count_malloc
 *
 * Count the number of bytes malloc'd against the correct process.
 */
void process_count_malloc (ulong nbytes)
{
    if (!system_running)
	init_totmalloc += nbytes;
    else if (schedflag)
	sched_totmalloc += nbytes;
    else
	forkx->totmalloc += nbytes;
}

/*
 * process_count_free
 *
 * Count the number of bytes freed against the correct process.
 */
void process_count_free (ulong nbytes)
{
    if (!system_running)
	init_totfree += nbytes;
    else if (schedflag)
	sched_totfree += nbytes;
    else 
	forkx->totfree += nbytes;
}

/*
 * process_count_getbuffer
 *
 * Count the number of bytes in a buffer against the correct process.
 */
void process_count_getbuffer (ulong nbytes)
{
    if (!system_running)
	init_totgetbufmem += nbytes;
    else if (schedflag)
	sched_totgetbufmem += nbytes;
    else
	forkx->totgetbufmem += nbytes;
}

/*
 * process_count_retbuffer
 *
 * Count the number of bytes in a buffer against the correct process.
 */
void process_count_retbuffer (ulong nbytes)
{
    if (!system_running)
	init_totretbufmem += nbytes;
    else if (schedflag)
	sched_totretbufmem += nbytes;
    else
	forkx->totretbufmem += nbytes;
}

/**********************************************************************
 *
 *			  LIST INFO ROUTINES
 *
 **********************************************************************/

/*
 * process_list_info
 *
 * The list info function used by all of the scheduler pre-process lists.
 */
static boolean process_list_info (list_header  *list,
				  list_element *element,
				  char         *buffer)
{
    sprocess *p;

    if (!element)
	return(FALSE);
    p = LIST_GET_DATA(element);
    sprintf(buffer, "Pid %-2u, Name %s", p->pid, p->name);
    return(TRUE);
}

/*
 * process_list_boolean
 *
 * The list info function used by the scheduler boolean event master list.
 */
static boolean process_list_boolean (list_header  *list,
				     list_element *element,
				     char         *buffer)
{
    watched_boolean *wb;

    if (!element)
	return(FALSE);
    wb = LIST_GET_DATA(element);
    sprintf(buffer, "(%d,%-2d) %5s  %18s", wb->wi.major_type, wb->wi.minor_type,
	    WB_VALUE(wb)?"TRUE":"FALSE", wb->wi.name);
    return(TRUE);
}

/*
 * process_list_bitfield
 *
 * The list info function used by the scheduler bitfield event master list.
 */
static boolean process_list_bitfield (list_header  *list,
				      list_element *element,
				      char         *buffer)
{
    watched_bitfield *wb;

    if (!element)
	return(FALSE);
    wb = LIST_GET_DATA(element);
    sprintf(buffer, "(%d,%-2d) %08x  %18s", wb->wi.major_type, wb->wi.minor_type,
	    wb->bits, wb->wi.name);
    return(TRUE);
}

/*
 * process_list_queue
 *
 * The list info function used by the scheduler queue event master list.
 */
static boolean process_list_queue (list_header  *list,
				   list_element *element,
				   char         *buffer)
{
    watched_queue *wq;

    if (!element)
	return(FALSE);
    wq = LIST_GET_DATA(element);
    sprintf(buffer, "(%d,%-2d) ct %-2d  %18s", wq->wi.major_type, wq->wi.minor_type,
	    WQ_QUEUESIZE(wq), wq->wi.name);
    return(TRUE);
}

/*
 * process_list_semaphore
 *
 * The list info function used by the scheduler semaphore event master list.
 */
static boolean process_list_semaphore (list_header  *list,
				       list_element *element,
				       char         *buffer)
{
    watched_semaphore *ws;

    if (!element)
	return(FALSE);
    ws = LIST_GET_DATA(element);
    sprintf(buffer, "(%d,%-2d) %5s  %18s", ws->wi.major_type, ws->wi.minor_type,
	    ws->s?"TAKEN":"FREE", ws->wi.name);
    return(TRUE);
}

/*
 * process_list_wakeups
 *
 * The list info function used by the scheduler semaphore event master list.
 */
boolean process_list_wakeups (list_header  *list,
			      list_element *element,
			      char         *buffer)
{
    watcher_info *wakeup;
    sprocess *proc;

    if (!element)
	return(FALSE);
    wakeup = LIST_GET_DATA(element);
    if (element == &wakeup->wi_by_event) {
        proc = wakeup->wi_process;
	sprintf(buffer, "Process: %s", proc ? proc->name : "None");
    } else {
	sprintf(buffer, "Event: %s", wakeup->wi_event->name);
    }
    return(TRUE);
}

/**********************************************************************
 *
 *		       SCHEDULER INITIALIZATION
 *
 **********************************************************************/

/*
 * sched_init
 *
 * Initialize scheduler variables
 */
boolean sched_init (boolean loading)
{
    /*
     * Ensure that the ROM monitor contains the necessary support
     * features. If we are here to bootstrap a system, we require
     * at least a V4.4 ROM monitor, due to incompatible changes
     * in the booting interface.
     *
     * Otherwise, any ROM monitor which supports the EMT_CLOCKADDR
     * function is acceptable.
     */

    if (loading && (mon_version() < (4 * 256 + 4)))
	obsolete_rom();

    GET_TIMESTAMP(interval_start);
    current_count = 0;
    last_int_percent_scaled = last_busy_percent = last_busy_percent_scaled = 0;
    avg_busy1_percent_scaled = avg_busy1_percent = 0;
    avg_busy5_percent_scaled = avg_busy5_percent = 0;
    idle_count_wired = FALSE;		/* we let SNMP set this to TRUE */
    int_count = idle_count = current_count = 0;			/* Clear it. */
    sched_max_task_time = SCHED_MAX_TASK_TIME_DEFAULT;
    watchdog_time = sched_max_task_time << WATCHDOG_TIME_SHIFT;
    sched_watchdog_action = SCHED_WATCHDOG_NORMAL;
    schedflag = TRUE;			/* we're running in the scheduler */
    refresh_time = platform_get_value(PLATFORM_VALUE_REFRESH_TIME);
    process_max_time = SCHED_VOLUNTARY_TASK_TIME/refresh_time;
    blocking_disabled = FALSE;		/* blocking not disable */
    TIMER_STOP(blocking_timer);
    last_proc_name = NULL;
    bzero(process_history, sizeof(char *) * PROC_HISTORY_LIMIT);
    process_history_index = 0;

    /*
     * Initialize the new scheduler lists.
     */
    queue_init(&sarrayQ, 0);
    queue_init(&procq_corrupt, 0);
    list_create(&procq_ready_c, 0, "Sched Critical",LIST_FLAGS_INTERRUPT_SAFE);
    list_create(&procq_ready_h, 0, "Sched High",    LIST_FLAGS_INTERRUPT_SAFE);
    list_create(&procq_ready_m, 0, "Sched Normal",  LIST_FLAGS_INTERRUPT_SAFE);
    list_create(&procq_ready_l, 0, "Sched Low",     LIST_FLAGS_INTERRUPT_SAFE);
    list_create(&procq_idle,    0, "Sched Idle",    LIST_FLAGS_INTERRUPT_SAFE);
    list_create(&procq_dead,    0, "Sched Dead",    LIST_FLAGS_INTERRUPT_SAFE);

#ifdef BACKWARD_COMPATIBLE
    /*
     * Initialize and validate the backward compatibility scheduler lists.
     * These should eventually go away.
     */
    list_create(&procq_runnorm, 0, "Sched Normal (Old)",LIST_FLAGS_INTERRUPT_SAFE);
    list_create(&procq_runlow,  0, "Sched Low (Old)",   LIST_FLAGS_INTERRUPT_SAFE);
#endif BACKWARD_COMPATIBLE

    /*
     * Now set up the lists for well known event types.
     */
    list_create(&s_notify_queue, 0, "Watched Queues", LIST_FLAGS_INTERRUPT_SAFE);
    list_create(&s_notify_boolean, 0, "Watched Booleans", LIST_FLAGS_INTERRUPT_SAFE);
    list_create(&s_notify_bitfield, 0, "Watched Bitfield", LIST_FLAGS_INTERRUPT_SAFE);
    list_create(&s_notify_semaphore, 0, "Watched Semaphores", LIST_FLAGS_INTERRUPT_SAFE);

    /*
     * validate the lists.  If this ever fails, the router is really SOL.
     */
    if (!(LIST_VALID(&procq_ready_c)    && LIST_VALID(&procq_ready_h)    &&
	  LIST_VALID(&procq_ready_m)    && LIST_VALID(&procq_ready_l)    &&
	  LIST_VALID(&procq_idle)       && LIST_VALID(&procq_dead)       &&
	  LIST_VALID(&s_notify_queue)   && LIST_VALID(&s_notify_boolean) &&
	  LIST_VALID(&s_notify_bitfield)&& LIST_VALID(&s_notify_semaphore)))
	return(FALSE);
#ifdef BACKWARD_COMPATIBLE
    if (!(LIST_VALID(&procq_runnorm)    && LIST_VALID(&procq_runlow)))
	return(FALSE);
#endif BACKWARD_COMPATIBLE

    /*
     * Setup the info routines for the basic scheduler lists.  The event
     * lists have no information routines available.
     */
    list_set_info(&procq_ready_c, process_list_info);
    list_set_info(&procq_ready_h, process_list_info);
    list_set_info(&procq_ready_m, process_list_info);
    list_set_info(&procq_ready_l, process_list_info);
    list_set_info(&procq_idle, process_list_info);
    list_set_info(&procq_dead, process_list_info);
#ifdef BACKWARD_COMPATIBLE
    list_set_info(&procq_runnorm, process_list_info);
    list_set_info(&procq_runlow, process_list_info);
#endif BACKWARD_COMPATIBLE
    list_set_info(&s_notify_queue, process_list_queue);
    list_set_info(&s_notify_boolean, process_list_boolean);
    list_set_info(&s_notify_bitfield, process_list_bitfield);
    list_set_info(&s_notify_semaphore, process_list_semaphore);

    /*
     * Initialize the dummy event blocks used by timers and messages.
     * These are signaled by class, not by individual event.
     */
    watcher_initialize_common(NULL, &sched_dummy_timer, "dummy timer",
			      TIMER_EVENT, 0);
    watcher_initialize_common(NULL, &sched_dummy_message, "dummy message",
			      MESSAGE_EVENT, 0);

    /*
     * Initialize the message pool.
     */
    process_initialize_messages();

    /*
     * Initialize the scheduler's master timer for sleeping processes.
     */
    mgd_timer_init_parent(&sched_master_timer, NULL);
    mgd_timer_init_leaf(&sched_force_int_excl, &sched_master_timer,
			STT_FORCE, NULL, TRUE);
    mgd_timer_set_fenced(&sched_force_int_excl, TRUE);
    mgd_timer_start(&sched_force_int_excl, ST_FORCE_TIME);
    return(TRUE);
}

/*********************************************************************
 *
 *			    PROCESS TABLES
 *
 *********************************************************************/

/*
 * process_slot_create
 *
 * Find or create a slot in the set of all known processes.  This set is
 * a link list of arrays of 256 processes.  This makes lookup very easy
 * as the upper 24 bits of the process number are the number of links to
 * walk, and the lower 8 bits are the index into the array.
 */
static pid_t process_slot_create (void)
{
    sprocess_list *array;
    sprocess **sp;
    int i, base;

    /*
     * See if there's an opening in the current process arrays.
     */
    for (array = sarrayQ.qhead, base = 0;
	 array;
	 array = array->sl_next, base += MAX_PROC_PER_ARRAY) {
	if (array->sl_count == MAX_PROC_PER_ARRAY)
	    continue;
	sp = &array->sl_process[0];
	for (i = 0; i < MAX_PROC_PER_ARRAY; i++, sp++) {
	    if (*sp)
		continue;
	    return(base + i + 1);
	}
    }
    
    /*
     * Nope, need to allocate a new process array.
     */
    array = malloc_named(sizeof(sprocess_list), "Process Array");
    if (!array)
	return(NO_PROCESS);
    p_enqueue(&sarrayQ, array);
    return(base + 1);
}

/*
 * process_slot_set
 *
 * Given a pid, store a data structure into the correct location in the
 * table of all processes.  The data structure will have already been
 * grown (if necessary) to accommodate this process.
 */
static void process_slot_set (pid_t pid, sprocess *sp)
{
    sprocess_list *array;
    pid_t local;

    local = pid - 1;
    for (array = sarrayQ.qhead;
	 array && (local >= MAX_PROC_PER_ARRAY);
	 array = array->sl_next, local -= MAX_PROC_PER_ARRAY)
	;
    if (!array || array->sl_process[local]) {
	(*kernel_errmsg)(&msgsym(BADPROCESS,SYS), pid, "setting");
	return;
    }
    array->sl_process[local] = sp;
    array->sl_count++;
}

/*
 * process_slot_clear
 *
 * Given a pid, clear the a data structure at the correct location in the
 * table of all processes.  If this is the last process is an array, and
 * the array is the last one in the queue, it may be removed.  We can't
 * remove anything but the last array because it would involve changing
 * the IDs of running processes.
 */
static void process_slot_clear (pid_t pid)
{
    sprocess_list *array;
    pid_t local;

    local = pid - 1;
    for (array = sarrayQ.qhead;
	 array && (local >= MAX_PROC_PER_ARRAY);
	 array = array->sl_next, local -= MAX_PROC_PER_ARRAY)
	;
    if (!array || !array->sl_process[local]) {
	(*kernel_errmsg)(&msgsym(BADPROCESS,SYS), pid, "clearing");
	return;
    }
    array->sl_process[local] = NULL;
    if ((--array->sl_count == 0) && (array->sl_next == NULL)) {
	p_unqueue(&sarrayQ, array);
	free(array);
    }
}


/**********************************************************************
 *
 *	      INDIVIDUAL PROCESS INITIALIZATION/CLEANUP
 *
 **********************************************************************/

/*
 * process_setup_sched_info
 *
 * Initialize a bunch of values in the process data structure.  This
 * sets up the event threading that is used to remember why a
 * processes was awakened.
 */
static void process_setup_sched_info (sprocess *sp)
{
    sched_event_set *info;

    /*
     * Initialize the per-process timer.  Make sure to set up the context
     * pointer and the fenced flag.  All scheduler timers are fenced so
     * that the private timer of a process can be linked in to them, and
     * the timer expiration routines will stop at the fence instead of
     * walking into private timers.
     */
    mgd_timer_init_parent(&sp->sched_timer, &sched_master_timer);

    mgd_timer_init_parent(&sp->sched_timer_managed, &sp->sched_timer);
    mgd_timer_set_additional_context(&sp->sched_timer_managed, 0, sp);
    mgd_timer_set_fenced(&sp->sched_timer_managed, TRUE);
    mgd_timer_set_type(&sp->sched_timer_managed, STT_PROCESS_MANAGED);

    mgd_timer_init_leaf(&sp->sched_timer_simple, &sp->sched_timer,
			STT_PROCESS_SIMPLE, sp, TRUE);
    mgd_timer_set_fenced(&sp->sched_timer_simple, TRUE);

    mgd_timer_init_leaf(&sp->sched_timer_temp, &sp->sched_timer,
			STT_PROCESS_TEMP, sp, TRUE);
    mgd_timer_set_fenced(&sp->sched_timer_temp, TRUE);

    /*
     * Initialize messages.
     */
    queue_init(&sp->message_queue, 0);

    /*
     * Initialize the event list for events that may wake up this process.
     */
    info = sp->current_events;
    queue_init(&sp->process_event_setQ, 0);
    list_create(&info->process_events, 0, sp->name, LIST_FLAGS_INTERRUPT_SAFE);
    info->process_events.info = process_list_wakeups;

    /*
     * Create and link the two statically defined entries for this list.
     */
    list_enqueue(&info->process_events, &info->watch_timers.wi_by_process,
		 &info->watch_timers);
    list_enqueue(&info->process_events, &sp->watch_messages.wi_by_process,
		 &sp->watch_messages);
    info->watch_timers.wi_event = &sched_dummy_timer;
    sp->watch_messages.wi_event = &sched_dummy_message;
    sp->direct_wakeup_major = 0;
    sp->direct_wakeup_minor = 0;
}

/*
 * process_cleanup_sched_info
 *
 * Clean up all the crap associated with the new scheduler.  Stop/unlink
 * the wakeup timer.  Unthread all the wakeup events and return the
 * memory blocks to the appropriate places.
 */
static void process_cleanup_sched_info (sprocess *sp)
{
    watcher_info *wakeup;
    message_type *message;
    sched_event_set *info;

    /*
     * These two were statically allocated and were only linked onto
     * the 'by_process' thread.
     */
    info = sp->current_events;
    list_remove(&info->process_events, &info->watch_timers.wi_by_process,
		   &info->watch_timers);
    list_remove(&info->process_events, &sp->watch_messages.wi_by_process,
		   &sp->watch_messages);

    /*
     * All the rest were dynamically allocated and are multiply linked.
     * Dequeuing an event cleans up the 'wi_by_process' thread, so all
     * that's left is the 'wi_by_event' thread.
     */
    while ((wakeup = list_dequeue(&info->process_events)) != NULL) {
	list_remove(wakeup->wi_by_event.list, &wakeup->wi_by_event, wakeup);
	free(wakeup);
    }

    /*
     * Now for the list itself.
     */
    list_destroy(&info->process_events);

    /*
     * Clean up timers.  De-link any linked timer as it will not
     * necessarily be destroyed along with the process structure.
     * Individually stop the timers.  Stopping the parent and having the
     * stop cascade downwards leaves room for a potential timing window
     * where a child is stopped twice.
     */
    if (sp->sched_linked_mtimer)
	mgd_timer_delink(&sp->sched_linked_mtimer);
    mgd_timer_stop(&sp->sched_timer_simple);
    mgd_timer_stop(&sp->sched_timer_temp);

    /*
     * Clean up messages.
     */
    while ((message = p_dequeue(&sp->message_queue)) != NULL)
	chunk_free(message_chunks, message);
}

/*
 * process_create_common
 *
 * Common routine that performs all the work of creating a process.
 */
pid_t process_create_common (
    process_t (*padd),	/* process start address */
    long pp,		/* possible parameter (backwards compatibility) */
    int stacksize,	/* explicit stack size in BYTES */ 
    char *name,		/* a human readable name */
    int priority,	/* process priority */
    int prefers_new)
{
    int pid;
    sprocess *sp;
    long stackstart;
    ulong *stackptr;

    if (onintstack()) {
	(*kernel_errmsg)(&msgsym(CFORKLEV, SYS), name, get_interrupt_level());
	return(NO_PROCESS);
    }

    if ((padd == NULL) || ((uchar *)padd > etext)) {
	(*kernel_errmsg)(&msgsym(CFORKBADFUNCT, SYS), padd);
	return(NO_PROCESS);
    }

    /* 
     * Initialize process call - find available slot.
     */
    pid = process_slot_create();
    if (pid == NO_PROCESS) {
	(*kernel_errmsg)(&msgsym(NOMEMORY, SYS), name);
	return(NO_PROCESS);
    }

    /*
     * Create both the stack and the process data structure.  The stack
     * is created first, so that it will generally be lower in memory
     * than the process data structure.  This way, if the stack does
     * overflow, it doesn't wipe out the evidence of which process had
     * problems.  Also, initialize the stack to all ones so we can
     * determine stack usage
     */
    stackptr = mempool_malloc(MEMPOOL_CLASS_PSTACK, stacksize);
    if (!stackptr) {
	(*kernel_errmsg)(&msgsym(CFORKMEM, SYS), name);
	return(NO_PROCESS);
    }
    sp = malloc(sizeof(sprocess));
    if (!sp) {
	free(stackptr);
	(*kernel_errmsg)(&msgsym(CFORKMEM, SYS), name);
	return(NO_PROCESS);
    }
    sp->current_events = process_malloc_event_set();
    if (!sp->current_events) {
	free(stackptr);
	free(sp);
	(*kernel_errmsg)(&msgsym(CFORKMEM, SYS), name);
	return(NO_PROCESS);
    }

    memset(stackptr, 0xff, stacksize);
    sp->stack = stackptr;
    memory_specify_owner(sp, "Process", pid, (ulong)sp);
    memory_specify_owner(sp->stack, "Process Stack", pid, (ulong)sp);
    memory_specify_owner(sp->current_events, "Process Events", pid, (ulong)sp);

    /*
     * Compute start of stack frame.  On both 68K and SPARC the stack
     * grows from high address to low address.  The SPARC insists on
     * double longword alignment (low 3 bits zero); the 68K doesn't care.
     * Treat them both the same way.
     * Start address, arguments, etc., are put on stack according to our
     * hardware type by stack_setup_process().
     */
    stackstart = (long)sp->stack; /* get address of stack memory block */
    stackstart += (stacksize - 4); /* compute stack start address */
    stackstart &= ~7;		/* align stack on double long boundary */
    sp->stacksize = stacksize;	/* remember stack size */
    sp->lowstack = stacksize;	/* remember low water mark */
    stack_setup_process(sp, stackstart, padd, pp);
    sp->pid = pid;		/* assign process ID */
    sp->analyze = TRUE;		/* Analyze death */
    sp->totmalloc = 0;		/* no memory used so far */
    sp->totfree = 0;		/* no memory used so far */
    sp->totgetbufmem = 0;	/* no memory used so far */
    sp->totretbufmem = 0;	/* no memory used so far */
    sp->name = name;		/* we have a name */
#ifdef NULL_STDIO
    sp->ttyptr = NULL;		/* no longer a default tty */
#else
    sp->ttyptr = console;	/* temporary workaround */
#endif    
    sp->magic = PROC_MAGIC;	/* check for trashed stacks */
    GET_TIMESTAMP(sp->prev_timer);  /* initialize for process_sleep_periodic() */
    process_slot_set(pid, sp);	/* set active process pointer to local var */
    process_setup_sched_info(sp);
    sp->default_wakeup_reasons = ALL_EVENTS;
    sp->wakeup_reasons = ALL_EVENTS;
    sp->prefers_new = prefers_new;
    sp->priority = priority;
    sp->reent_block = _reent_init();

    /*
     * Slam the process on the idle queue and then move it to the
     * appropriate run queue.  This streamlines the routines that move
     * processes between queues, because they no longer need to check
     * to see if a process is currently on a queue.  It always will be.
     */
    list_enqueue(&procq_idle, &sp->sched_list, sp);
    if (prefers_new) {
	process_run_add(sp);
    } else {
	old_run_add(sp);
    }
    return(sp->pid);		/* return process ID */
}

/*
 * process_create
 *
 * Create a process that is aware of the new scheduler primitives.  This
 * process does not take any arguments.  The stack size must be declared
 * in BYTES, and the processor priority must also be declared.
 */
pid_t process_create (
    process_t (*padd),		 /* process start address */
    char *name,			 /* a human readable name */
    stack_size_t stacksize,	 /* stack size constant */ 
    process_priority_t priority) /* priority to schedule process */
{
    /*
     * Validate the stack size and process priority.  It would be really
     * nice if GCC would validate this enum at compile time, but it
     * doesn't work that way.
     */
    switch (stacksize) {
      case SMALL_STACK:
      case NORMAL_STACK:
      case IPROUTING_STACK:
      case LARGE_STACK:
      case HUGE_STACK:
	break;
      default:
	(*kernel_errmsg)(&msgsym(STACKSIZE, SCHED), stacksize, name);
	return(NO_PROCESS);
    }
    switch (priority) {
      case PRIO_CRITICAL:
      case PRIO_HIGH:
      case PRIO_NORMAL:
      case PRIO_LOW:
	break;
      default:
	(*kernel_errmsg)(&msgsym(PRIORITY, SCHED), priority, name);
	return(NO_PROCESS);
    }
    return(process_create_common(padd, 0, stacksize, name, priority, TRUE));
}


/*********************************************************************
 *
 *		     50 WAYS TO KILL YOUR PROCESS
 *
 *********************************************************************/

/*
 * process_last_gasp
 *
 * If the process has registered a cleanup handler, run it.  When
 * finished, mop up the per-process data structure and insure that the
 * process never runs again.
 */
void process_last_gasp (void)
{
    boolean process_already_dead;

    /*
     * Always clear the blocking_disabled flag.  The process might
     * have died while it already had blocking disabled.
     */
    blocking_disabled = FALSE;

    /*
     * Mark the process as dead.  (Dying, actually.)
     */
    process_already_dead = forkx->killed;
    forkx->killed = TRUE;

    /*
     * Give the process one last chance to clean up.  If the process is
     * already dead, then the code got here as the result of a
     * signal(pid, SIGEXIT) and not a process_kill(pid).
     */
    if (!process_already_dead)
	signal_send(forkx->pid, SIGEXIT);

    /*
     * Clean up the reent_block
     */
    if (forkx->reent_block) {
	_reent_cleanup(forkx->reent_block);
	forkx->reent_block = NULL;
    }	

#ifdef FUTURE_WORK
    /*
     * Clean up any terminal session.  Make sure to clear the tiptop field so
     * that death() doesn't call process_kill() again on this same process.
     * Too, too many processes have the console line for their ttyptr, so
     * don't touch that one.  Any exec will get cleared by invoking death()
     * first.
     */
    if ((forkx->ttyptr) && (forkx->ttyptr != MODEMS[0])) {
	forkx->ttyptr->tiptop = 0;
	death(forkx->ttyptr);
    }
#endif

    /*
     * Unlink all possible scheduler wakeups, and then put the process
     * onto the living dead queue.  The grim reaper will get around to
     * picking it up.
     */
    process_cleanup_sched_info(forkx);
    forkx->state = SCHEDSTATE_DEAD;
    list_move(&procq_dead, &forkx->sched_list);

    /*
     * Relinquish the processor.  This should never return, but just
     * in case...
     */
    for (;;) {
	suspend();
    }
}

/*
 * process_kill
 *
 * Mark a process for destruction.  Also sends a signal to the process
 * so it gets a changes to run its exit handler.
 */
void process_kill (pid_t pid)
{
    sprocess *proc;

    proc = process_pid2ptr(pid);
    if (!proc) {
	(*kernel_errmsg)(&msgsym(NOPROCESS, SYS), pid);
	return;
    }

    if (proc->killed)
	(*kernel_errmsg)(&msgsym(NOTDEAD, SYS), proc->name, proc->pid);
#ifdef SCHED_DEBUG
    if (proc->debug_events & PROC_DEBUG_EXECUTION)
	(*kernel_buginf)("\nSCHED: Killing process %s from process %s",
			 proc->name, forkx->name);
#endif
    if (proc == forkx) {
	process_last_gasp();		/* never returns */
    } else {
	alter_suspended_process(proc, process_last_gasp);
	proc->edisms = NULL;		/* wake up old procs */
	proc->wakeup_reasons = ALL_EVENTS;
	process_wakeup_internal(proc, DIRECT_EVENT, 0);
    }
}

/*
 * mark_process_dead
 *
 * Mark a single process as dead, and move it to the dead process queue.
 */
STATIC INLINE void mark_process_dead (sprocess *proc)
{
    proc->crashed = TRUE;
    process_cleanup_sched_info(proc);
    list_move(&procq_dead, &proc->sched_list);
}

/*
 * mark_list_dead
 *
 * Run one of the scheduler lists, checking each process to see if it has
 * the crashblock flag set and, if so, marking that process as dead.
 */
STATIC INLINE void mark_list_dead (list_header *list)
{
    list_element *element;
    sprocess *proc;

    FOR_ALL_DATA_IN_LIST(list, element, proc) {
        if (proc->crashblock)
	    mark_process_dead(proc);
    }
}

/*
 * sched_mark_router_crashing
 *
 * Go and mark all processes that have the 'block on crash' flag set
 * as 'crashed' so they will not be executed by the scheduler.
 * This routine is called by the exception handler upon system crash.
 */
void sched_mark_router_crashing (void)
{
    mark_list_dead(&procq_ready_c);
    mark_list_dead(&procq_ready_h);
    mark_list_dead(&procq_ready_m);
    mark_list_dead(&procq_ready_l);
    mark_list_dead(&procq_idle);
#ifdef BACKWARD_COMPATIBLE
    mark_list_dead(&procq_runnorm);
    mark_list_dead(&procq_runlow);
#endif BACKWARD_COMPATIBLE

    if (forkx->crashblock)
	mark_process_dead(forkx);
}

/*
 * process_bury
 *
 * Scan active queue for dead processes
 */
static void process_bury (void)
{
    sprocess *sp;
    list_element *elem, *next;

    FOR_ALL_DATA_IN_LIST_W_NEXT(&procq_dead, elem, next, sp) {
	if (sp->debugger)		/* Is this process being debugged? */
	    if (gdb_kill_debug(sp))	/* Will GDB save it? */
		continue;		/* Yes, skip this process */
	if (sp->crashed)		/* Don't touch either if crashed */
	    continue;
	dead_totmalloc += sp->totmalloc;
	dead_totfree += sp->totfree;
	dead_totgetbufmem += sp->totgetbufmem;
	dead_totretbufmem += sp->totretbufmem;
	list_remove(&procq_dead, &sp->sched_list, sp);
	if (sp->analyze)
	    stack_analyze(sp);		/* analyze stack usage */
	process_slot_clear(sp->pid);
	free(sp->current_events);	/* release event list */
	free(sp->stack);		/* release stack */
	free(sp->signal);		/* Release signal context */
	free(sp);			/* release the data block */
    }
}

/*
 * process_terminate
 *
 * A canonical watchdog handler.
 */
static void process_terminate (void)
{
    reg_invoke_process_death(forkx->pid);
    process_kill(THIS_PROCESS);
}

/*
 * process_handle_watchdog
 *
 * If a process causes a watchdog failure, the NMI fudges it to receive a
 * SIGWDOG signal.  The signal_receive routine calls any handler routine
 * that may have bee registered, and then calls this routine.  If the
 * process is not watchdog aware or has not registered a handler routine,
 * the default action is to crash the router.  If the process has
 * registered a handler routine, it is still only allowed to take a
 * watchdog timeout once in a really looooong period of time.  A second
 * watchdog signal in this period cause the process to be summarily
 * killed.
 */
void process_handle_watchdog (boolean handler_exists)
{
    boolean fatal;

    (*kernel_errmsg)(&msgsym(WATCHDOG, SYS), forkx->name);
    switch(sched_watchdog_action) {
      default:
      case SCHED_WATCHDOG_NORMAL:
	if (handler_exists) {
	    fatal = (TIMER_RUNNING(forkx->last_watchdog) &&
		     (ELAPSED_TIME(forkx->last_watchdog) < WATCHDOG_FATAL_TIME));
	    GET_TIMESTAMP(forkx->last_watchdog);
	    if (fatal)
		process_kill(THIS_PROCESS);
	} else {
	    crashdump(10);
	}
	break;
      case SCHED_WATCHDOG_RELOAD:
	crashdump(10);
	break;
      case SCHED_WATCHDOG_HANG:
	process_hang();
	break;
    case SCHED_WATCHDOG_TERMINATE:
	process_terminate();
	break;
    }
}

/*
 * process_hari_kari
 *
 * Proclaim loud and clear that we are about to fall on the knife, and
 * then do so.  This is mainly a debugging aid so that removed
 * processes do not crash the system without a trace.
 */
void process_hari_kari (void)
{
    (*kernel_errmsg)(&msgsym(HARIKARI, SYS), forkx->name);
    process_kill(THIS_PROCESS);
}


/**********************************************************************
 *
 *		    MISCELLANEOUS PROCESS TESTING
 *
 **********************************************************************/

/*
 * process_reschedule_test
 *
 * Make sure a scheduler invocation is reasonable, i.e.
 *  - never invoke from within scheduler
 *  - never invoke at interrupt level
 * Returns TRUE or FALSE.
 */

static inline boolean process_reschedule_test (char *str, boolean query)
{
    if (schedflag) {
	if (!query)
	    (*kernel_errmsg)(&msgsym(INSCHED, SYS), str);
	return(FALSE);
    }
    if (get_interrupt_level()) {
	if (!query)
	    (*kernel_errmsg)(&msgsym(INTSCHED, SYS), str,
			     get_interrupt_level());
	return(FALSE);
    }
    if (blocking_disabled) {		/* We shouldn't be here */
	if (query)
	    return (FALSE);

  	(*kernel_errmsg)(&msgsym(NOBLOCK, SYS), str);

 	/*
 	 * We exited the task with blocking disabled.  Check to see if
 	 * we're hung this way (if we are, the system is very dead,
 	 * which is not a good thing).  If blocking_disabled is set
 	 * for an entire second, we declare it hung and clear it.
 	 */
 	if (TIMER_RUNNING(blocking_timer)) {
 	    if (AWAKE(blocking_timer)) { /* We hung this way */
 		(*kernel_errmsg)(&msgsym(BLOCKHUNG, SYS), blocking_disabled);
 		blocking_disabled = FALSE; /* Unhang it. */
 		return(TRUE);		/* Let it suspend. */
 	    }
 	} else {			/* First time */
 	    TIMER_START(blocking_timer, ONESEC);
 	}
  	return(FALSE);
    } else {				/* Blocking isn't disabled */
	if (!query)
	    TIMER_STOP(blocking_timer);	/* It didn't hang */
    }
    return(TRUE);
}

/*
 * process_run_degraded_or_crash
 */
static void process_run_degraded_or_crash (void)
{
    /*
     * Cut the current process out of the scheduler's data structures and move it
     * intact.  The only change that must be make is to set the crashed flag so
     * that new events won't cause indirect references through possibly bad
     * fields.
     */
    forkx->corrupt = TRUE;
    list_excise(forkx_list, forkx);
    data_enqueue(&procq_corrupt, forkx);

    /*
     * Do we bail now, or try to keep running.  If the choice it to keep
     * running, short circut the suspend process to prevent any other
     * references to the corrupted process structure.  The suspend() call
     * should never return, but just in case...
     */
    if (!sched_run_degraded && !sched_already_degraded)
	crashdump(0);
    sched_already_degraded = TRUE;
    for (;;) {
	suspend();
    }
}


/*
 * process_ok_to_reschedule
 *
 * Used by scheduler internal code to determine if someone's tried to
 * reschedule when they shouldn't.
 */

boolean process_ok_to_reschedule (char *str)
{
    ulong *sp;
    char *var, *proc;
    sched_event_set *events;

    if (!process_reschedule_test(str, FALSE))
	return(FALSE);

    if (checkheaps_per_proc)
	checkheaps_for_scheduler(forkx->name, NULL);

    /*
     * The process is dismissing.  Sanity check the scheduler data before
     * dereferencing anything.
     */
    if (!((forkx == forkx_2) && (forkx == forkx_3))) {
	sprocess *expected, *actual;
	if (forkx_2 == forkx_3) {
	    var = "forkx";
	    proc = forkx_2->name;
	    expected = forkx_2;
	    actual = forkx;
	    forkx = forkx_2;		/* reset it */
	} else if (forkx != forkx_2) {
	    var = "forkx_2";
	    proc = forkx->name;
	    expected = forkx;
	    actual = forkx_2;
	} else {
	    var = "forkx_3";
	    proc = forkx->name;
	    expected = forkx;
	    actual = forkx_3;
	}
	(*kernel_errmsg)(&msgsym(CORRUPT, SCHED), var, proc, expected, actual);
    }

    /*
     * Check the process structure first to see if it has been corrupted.  If
     * so, bail out now.  If that passes, check the event sets.
     */
    if (forkx->magic != PROC_MAGIC) {
	(*kernel_errmsg)(&msgsym(CORRUPT, SCHED), "proc magic",
			 process_history[process_history_index],
			 PROC_MAGIC, forkx->magic);
	process_run_degraded_or_crash();
	/* Never returns */
    }
    events = forkx->current_events;
    while (events) {
	if (events->magic != SCHED_EVENT_MAGIC) {
	    (*kernel_errmsg)(&msgsym(CORRUPT, SCHED), "event magic",
			     forkx->name, SCHED_EVENT_MAGIC,
			     forkx->current_events->magic);
	    process_run_degraded_or_crash();
	    /* Never returns */
	}
	events = events->next;
    }

    /*
     * The basic process data struture seems valid.  Check out the stack now.
     * If its corrupted, bail out.  There are two checks here.  1) Is the
     * stack pointer currently below the bottom of the stack.  2) Has the
     * bottom word of the stack been used.
     */
    sp = current_stack_pointer();
    if (sp <= forkx->stack) {
	(*kernel_errmsg)(&msgsym(STACKLOW, SYS), "process", forkx->name,
			 sp - forkx->stack, forkx->stacksize);
	process_run_degraded_or_crash();
	/* Never returns */
    } else if (*forkx->stack != 0xFFFFFFFF) {
	(*kernel_errmsg)(&msgsym(STACKLOW, SYS), "process", forkx->name, 0,
			 forkx->stacksize);
	forkx->lowstack = 0;
	process_run_degraded_or_crash();
	/* Never returns */
    }
    return(TRUE);
}

/*
 * process_suspends_allowed
 *
 * This is the public version of process_ok_to_reschedule.
 * It may be used by a process to determine if it is in a context where
 * suspends are allowed (or desirable) vs a context where they would otherwise
 * be problematic (e.g. blocking is disabled or we're in interrupt context).
 *
 * This function should be used with care, it's better to write code that
 * doesn't need to know this sort of information in the first place.
 */

boolean process_suspends_allowed (void)
{
    return process_reschedule_test("process suspends allowed", TRUE);
}

/*
 * check_cpuhog_inline
 *
 * Returns TRUE if the current process is hogging the CPU.
 */
static inline boolean check_cpuhog_inline (long time_this_run,
					   long interrupt_time_this_run)
{
    /*
     * Check to see if the task hogged the CPU.  This is done by
     * taking the overall elapsed time, and fudging for the interrupt
     * time.  "int_count" is bumped by the NMI whenever the NMI fires
     * and the box is in interrupt state;  it is cleared by a
     * background task, so it will stay coherent for this run except
     * when the background task clears it!  We account for that
     * here...
     */
    if (sched_max_task_time) {
 	if (interrupt_time_this_run < 0)	/* The task cleared it */
 	    interrupt_time_this_run = 0;	/* Close enough. */
 	if (time_this_run - interrupt_time_this_run > sched_max_task_time) {
	    return(TRUE);
 	}
    }
    return(FALSE);
}

/*
 * check_cpuhog
 *
 * Returns TRUE if the current process is hogging the CPU.
 */
boolean check_cpuhog (void)
{
    long time_this_run, interrupt_time_this_run;

    time_this_run = ELAPSED_TIME(forkx->starttime);
    interrupt_time_this_run = (int_count - last_int_count) * refresh_time;
    return(check_cpuhog_inline(time_this_run, interrupt_time_this_run));
}

/*
 * process_hogcheck
 *
 * Check to see if the current process hogged the CPU and complain if so.
 */
static inline void process_hogcheck (void)
{
    long time_this_run;
    long interrupt_time_this_run;

    /*
     * Check to see if the task hogged the CPU.  This is done by
     * taking the overall elapsed time, and fudging for the interrupt
     * time.  "int_count" is bumped by the NMI whenever the NMI fires
     * and the box is in interrupt state;  it is cleared by a
     * background task, so it will stay coherent for this run except
     * when the background task clears it!  We account for that
     * here...
     */
    time_this_run = ELAPSED_TIME(forkx->starttime);
    interrupt_time_this_run = (int_count - last_int_count) * refresh_time;
    if (check_cpuhog_inline(time_this_run, interrupt_time_this_run)) {
	(*kernel_errmsg)(&msgsym(CPUHOG, SYS), 
	       time_this_run - interrupt_time_this_run,
	       int_count, last_int_count,
	       forkx->name, 
	       forkx->caller_pc);
	profile_hog_occurred();	/* Tell profiler we hogged */
    }
}


/*********************************************************************
 *
 *			    THE SCHEDULER
 *
 *********************************************************************/

/*
 * process_execute
 *
 * Perform housekeeping and execute a process.
 */
static void process_execute (sprocess *proc)
{
    if (proc->magic != PROC_MAGIC)	/* trashed process */
	(*kernel_errmsg)(&msgsym(CORRUPT, SCHED),
	       proc->name, "unknown", proc->magic, PROC_MAGIC);

    /* If we're profiling CPU hogs, clear the profile blocks now. */
 
    if (profile_check_for_hogs()) {
	profile_clear_blocks();
    }

#ifdef SCHED_DEBUG
    if (proc->debug_events & PROC_DEBUG_EXECUTION) {
	(*kernel_buginf)("\nSCHED: Starting execution of %s", proc->name);
	sched_debug_events = 0;
    } else if (sched_debug_events & PROC_DEBUG_EXECUTION) {
	(*kernel_buginf)("\nSCHED: Starting execution of %s", proc->name);
    }
#endif
    last_int_count = int_count;		/* Note the interrupt count */
    GET_TIMESTAMP(proc->starttime);	/* starting time */
    stdio = proc->ttyptr;		/* define standard I/O block */
    forkx = proc;			/* The current process to resume */
    forkx_2 = proc;			/* The current process to resume */
    forkx_3 = proc;			/* The current process to resume */
    proc->wakeup_posted = FALSE;        /* protect against losing events */
    proc->current_events->event_count=0; /* protect against event wrap */
    proc->state = SCHEDSTATE_RUNNING;	/* we are a running process */
    proc->caller_pc = 0;	        /* no blocking PC */
    proc->callee_pc = 0;	        /* no blocking PC */

    if (++process_history_index >= PROC_HISTORY_LIMIT)
	process_history_index = 0;
    process_history[process_history_index] = proc->name;
    process_timer = process_max_time;	/* Reset allowed tick count for process */
    process_quantum_expired = FALSE;
    resume();				/* run the user */

    process_hogcheck();			/* Check for hogs */
#ifdef TRAP_PAGE_ZERO
    sched_test_page_zero_inline();	/* Check for scribbling on low memory */
#endif

    proc->elapsedtime += ELAPSED_TIME(proc->starttime); /* ending time */
    ++proc->ncalls;			/* count calls */
#ifdef SCHED_DEBUG
    if ((proc->debug_events & PROC_DEBUG_EXECUTION) ||
	(sched_debug_events & PROC_DEBUG_EXECUTION)) {
	ulong time_this_run, interrupt_time_this_run;
	time_this_run = ELAPSED_TIME(forkx->starttime);
	interrupt_time_this_run = (int_count - last_int_count) * refresh_time;
	(*kernel_buginf)("\nSCHED: Finished execution of %s, ran for %d/%d ms",
	       proc->name, time_this_run - interrupt_time_this_run, 
	       time_this_run);
    }
#endif
}

/*
 * process_run_if_ready
 *
 * Run a process if it should be run.  This is an inline wrapper for
 * process_execute to avoid overhead in the usual non-run cases.
 *
 * Returns TRUE if the process actually ran.  This routine will only ever
 * see processes that have want the processor as soon as possible, or
 * process waiting on an edisms.  Processes waiting only a timer are all
 * handled by the new scheduler primitives.
 */

STATIC INLINE boolean process_run_if_ready (sprocess *proc)
{
    if (!proc)				/* Nothing to run */
 	return(FALSE);
    
    /* Is the process in a runnable state? */
    if (proc->crashed || proc->killed || proc->corrupt)
 	return(FALSE);

    if (proc->edisms) {			/* has event dismiss on */
 	boolean status;
 	status = (*proc->edisms)(proc->eparm);
	
	if (checkheaps_per_poll)
	    checkheaps_for_scheduler(proc->name, proc->edisms);
	
 	if (status)
 	    return(FALSE);		/* still dismissed, exit now */
 	proc->edisms = 0;		/* clear event dismiss */
    }
    process_execute(proc);
    return(TRUE);
}

/*
 * process_wake_sleepers
 *
 * "Let the sleeper awaken!"  This routine looks for any processes with
 * expired wakeup timer, and requeues them from the idle queue onto the
 * appropriate queue for their priority and awareness of the new
 * scheduler primitives.  It does not stop the timer, that is done in the
 * process wakeup routine so that timers are stopped no matter how the
 * process is awakened.
 */
STATIC INLINE void process_wake_sleepers (void)
{
    mgd_timer *expired_timer;
    sprocess *sleep;
    char buffer[100];
    int cc;

    while (mgd_timer_expired(&sched_master_timer)) {
	expired_timer = mgd_timer_first_fenced(&sched_master_timer);
	if (expired_timer == NULL) {
	    /*
	     * WTF?  Should never get here....
	     */
	    expired_timer = mgd_timer_first_expired(&sched_master_timer);
	    mgd_timer_stop(expired_timer);
	    cc = sprintf(buffer, "\n%%SYS-1-MTNOTFENCED: ");
	    sprintf(&buffer[cc], msgtxt_notfenced, expired_timer,
		    mgd_timer_type(expired_timer));
	    console_message(buffer);
	    crashdump(2);
	}

	switch (mgd_timer_type(expired_timer)) {
	  case STT_PROCESS_MANAGED:
	    sleep = mgd_timer_additional_context(expired_timer, 0);
	    sleep->current_events->watch_timers.wi_occurred = TRUE;
	    sleep->prev_timer = mgd_timer_exp_time(expired_timer);
	    process_wakeup_internal(sleep, TIMER_EVENT, 0);
	    break;

	  case STT_PROCESS_SIMPLE:
	    sleep = mgd_timer_context(expired_timer);
	    sleep->current_events->watch_timers.wi_occurred = TRUE;
	    sleep->prev_timer = mgd_timer_exp_time(expired_timer);
	    process_wakeup_internal(sleep, TIMER_EVENT, 0);
	    break;

	  case STT_PROCESS_TEMP:
	    sleep = mgd_timer_context(expired_timer);
	    /* Leave no trace in the process event record. */
	    process_wakeup_internal(sleep, TIMER_EVENT, 0);
	    break;

	  case STT_FORCE:
	    /*
	     * Restart the timer.  These force the mgd_timers code to always
	     * lock out interrupts fir the scheduler timers.
	     */
	    mgd_timer_update(expired_timer, ST_FORCE_TIME);
	    break;

	  default:
	    /*
	     * Should never get here....
	     */
	    (*kernel_errmsg)(&msgsym(UNEXPECTEDTIMER,SCHED), expired_timer,
		   mgd_timer_type(expired_timer));
	    mgd_timer_stop(expired_timer);
	    break;
	}
    }
}


/*
 * process_run_critical_queue
 *
 * Run all the critical-priority processes.
 */

STATIC INLINE void process_run_critical_queue (void)
{
    list_element *elem, *next;
    sprocess *proc;

    /*
     * Run any processes that are ready.
     */
    if (!LIST_SIZE(&procq_ready_c))
	return;
    if (LIST_EMPTY(&procq_ready_c))	/* Paranoia */
	crashdump(0);
    forkx_list = &procq_ready_c;
    FOR_ALL_DATA_IN_LIST_W_NEXT(&procq_ready_c, elem, next, proc) {
	if (!proc)			/* Paranoia */
	    continue;
	process_execute(proc);
	process_wake_sleepers();
    }
}


/*
 * process_run_high_queues
 *
 * Run all the high-priority processes in the new scheduler queue.
 * Returns TRUE if any high-priority process ran.
 */

STATIC INLINE boolean process_run_high_queues (void)
{
    list_element *elem, *next;
    sprocess *proc;
    boolean ran_something;
    
    /*
     * Run any processes that are ready.
     */
    ran_something = FALSE;
    if (LIST_SIZE(&procq_ready_h)) {
	if (LIST_EMPTY(&procq_ready_h))	/* Paranoia */
	    crashdump(0);
	forkx_list = &procq_ready_h;
	FOR_ALL_DATA_IN_LIST_W_NEXT(&procq_ready_h, elem, next, proc) {
	    if (!proc)
		continue;
	    process_execute(proc);
	    process_wake_sleepers();
	    process_run_critical_queue();
	    ran_something = TRUE;
	}
    }
    return(ran_something);
}

/*
 * process_run_norm_queues
 *
 * Run all the normal-priority processes in the new scheduler queue.  This
 * will also run all the processes in the normal-priority compatibility queue.
 *
 * Returns TRUE if any new process or compatible normal-priority process
 * ran.
 */

STATIC INLINE boolean process_run_norm_queues (void)
{
    list_element *elem, *next;
    sprocess *proc;
    boolean ran_something;
    
    ran_something = FALSE;

    if (LIST_SIZE(&procq_ready_m)) {
	if (LIST_EMPTY(&procq_ready_m))	/* Paranoia */
	    crashdump(0);
	forkx_list = &procq_ready_m;
	FOR_ALL_DATA_IN_LIST_W_NEXT(&procq_ready_m, elem, next, proc) {
	    if (!proc)
		continue;
	    process_execute(proc);
	    process_wake_sleepers();
	    process_run_critical_queue();
	    process_run_high_queues();
	    ran_something = TRUE;
	}
    }

#ifdef BACKWARD_COMPATIBLE
    if (LIST_EMPTY(&procq_runnorm))
 	return(ran_something);

    forkx_list = &procq_runnorm;
    FOR_ALL_DATA_IN_LIST_W_NEXT(&procq_runnorm, elem, next, proc) {
 	if (process_run_if_ready(proc)) { /* will execute if ready */
	    process_wake_sleepers();	  /* wake any up if executed */
	    process_run_critical_queue();
	    process_run_high_queues();
 	    ran_something = TRUE;
	}
    }
#endif BACKWARD_COMPATIBLE
    return(ran_something);
}


/*
 * process_run_low_queues
 *
 * Run all the low-priority processes in the new scheduler queue.  This
 * will also run all the processes in the low-priority compatibility queue.
 *
 * On entry, the size of the queue is remembered and we only process
 * that number of processes.  Otherwise, n (n > 1) low priority processes
 * that are always ready to run (but suspending) would lock out
 * normal priority processes.  Any processes added to the end of the list
 * after we start looking at the queue won't get processed until
 * the next time we start looking.
 */

STATIC INLINE void process_run_low_queues (void)
{
    list_element *elem, *next;
    sprocess *proc;
    int count;

    count = LIST_SIZE(&procq_ready_l);
    if (count) {
	if (LIST_EMPTY(&procq_ready_l))	/* Paranoia */
	    crashdump(0);
	forkx_list = &procq_ready_l;
	FOR_ALL_DATA_IN_LIST_W_NEXT(&procq_ready_l, elem, next, proc) {
	    count--;
	    if (!proc) {
		/*
		 * Isn't this strange?  Error condition?
		 */
		continue;
	    }
	    process_execute(proc);
	    process_wake_sleepers();
	    process_run_critical_queue();
	    process_run_high_queues();
	    if (count <= 0)
		break;
	}
    }

#ifdef BACKWARD_COMPATIBLE
    if (LIST_EMPTY(&procq_runlow))
 	return;

    forkx_list = &procq_runlow;
    count = LIST_SIZE(&procq_runlow);
    FOR_ALL_DATA_IN_LIST_W_NEXT(&procq_runlow, elem, next, proc) {
	if (process_run_if_ready(proc)) { /* will execute if ready */
	    process_wake_sleepers();	  /* wake any up if executed */
	    process_run_critical_queue();
	    process_run_high_queues();
	}
	if (--count <= 0)
	    break;
    }
#endif BACKWARD_COMPATIBLE
}

/*
 * scheduler
 *
 * The scheduler itself
 */
void scheduler (void)
{
    int lowcheck = LOWTIMEOUT;
    int pass_count = 0;

    while (system_running) {
 	boolean ran_norm;
	
	/*
	 * Always check on the critical priority processes first.  These
	 * must be run before anything else, because they control the
	 * gating items that may allow a process to execute properly.
	 * I.E.  Things like filling buffer pools, etc.
	 */
	process_run_critical_queue();

  	/*
	 * Run high priority processes if they exist.  If high priority
	 * processes are run, we go back to the top of the scheduler
	 * loop.  This effectively means that high priority processes
	 * will starve the system.
	 */
 	if (process_run_high_queues())
 	    continue;
	
 	/*
 	 * Run any normal priority processes.  We check for (and run)
 	 * the entire high priority queue interleaved with each
 	 * normal priority task.  This means that if we've made it
 	 * down this far, we're guaranteed to execute each normal task
 	 * at least once.
 	 */
 	ran_norm = process_run_norm_queues();
	
 	/*
 	 * Now for the low priority queue.  We run low priority tasks if
 	 * there were no normal priority tasks on this pass through
 	 * the scheduler, or if we've made so many passes that we
 	 * can't starve them any longer.  We check for (and run) the
 	 * entire critical and high priority queues interleaved with each low
 	 * priority task.
 	 */
 	lowcheck--;
 	if (!ran_norm || !lowcheck) {
 	    lowcheck = LOWTIMEOUT;
 	    process_run_low_queues();
  	}

	/*
	 * Make sure that we always check for sleeping processes at
	 * least once per iteration of the scheduler.
	 */
	process_wake_sleepers();

	/*
	 * Finish any process destruction.
	 */
	if (!LIST_EMPTY(&procq_dead))
	    process_bury();

        /*
         * "scheduler-interval" processing
	 */
	(*kernel_sched_loop_hook)(ran_norm);

	/*
	 * Jumble the random number generator
	 */
	if (++pass_count > RANDOMIZE_FREQUENCY) {
	    pass_count = 0;
	    (void) random_gen();
	}
    }
}
