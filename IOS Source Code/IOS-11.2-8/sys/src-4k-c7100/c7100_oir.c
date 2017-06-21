/* $Id: c7100_oir.c,v 3.2.58.10 1996/08/28 13:11:36 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c7100/c7100_oir.c,v $
 *------------------------------------------------------------------
 * c7100_oir.c - Predator OIR support
 *
 * August 1995, Dave Carroll
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c7100_oir.c,v $
 * Revision 3.2.58.10  1996/08/28  13:11:36  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.58.9  1996/07/11  19:10:40  dcarroll
 * CSCdi61747:  PA insertion time not reset after OIR
 * Branch: California_branch
 * Reset insertion time on OIR removal.
 *
 * Revision 3.2.58.8  1996/06/11  14:13:04  dcarroll
 * CSCdi59976:  Partially inserted PA will remain unusable
 * Branch: California_branch
 * Reset cached eeprom data when deactivating PA.
 *
 * Revision 3.2.58.7  1996/05/22  18:55:07  dcarroll
 * CSCdi57774:  Indefinite retries if init fails.
 * Branch: California_branch
 * Add support to limit init retries.
 *
 * Revision 3.2.58.6  1996/05/21  06:39:37  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.2.58.5  1996/05/01  13:56:17  dcarroll
 * CSCdi56037:  provide oir active check routine
 * Branch: California_branch
 * Need 'oir active check' routine.
 *
 * Revision 3.2.58.4  1996/05/01  13:28:24  dcarroll
 * CSCdi54895:  oir fix for ethernet port adapter
 * Branch: California_branch
 * OIR support for pas ethernets.
 *
 * Revision 3.2.58.3  1996/04/22  18:15:51  gstovall
 * CSCdi53071:  media idb init routines will fail on second pass
 * Branch: California_branch
 * Allow media idb init routines to be called again.  Also clean up the
 * handling of the media idb queues.
 *
 * Revision 3.2.58.2  1996/04/05  05:33:00  dcarroll
 * CSCdi53595:  allow for multiple passes through pas idb init code
 * Branch: California_branch
 * Break up the init idb routines and keep track of idb init state.
 *
 * Revision 3.2.58.1  1996/03/21  23:48:07  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.8  1996/02/21  23:06:18  mbeesley
 * CSCdi49494:  Predator needs to support cdbs for channelized interfaces
 * Branch: ELC_branch
 *
 * Revision 3.2.2.7  1996/02/08  09:26:52  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.6  1996/01/26  22:10:36  mbeesley
 * Branch: ELC_branch
 *         o Debug GT64010 timers and add support for counters
 *         o Increase GT64010 cache line size
 *         o Finish support for fine granularity throttling
 *
 * Revision 3.2.2.5  1995/12/13  14:32:18  dcarroll
 * Branch: ELC_branch
 * Move pas_add_card_entry. Fix some debug messages.
 *
 * Revision 3.2.2.4  1995/12/09  22:52:17  mbeesley
 * Branch: ELC_branch
 * Defeat OIR, ENV processes until hw is verified. Fix initial bugs:
 *         o NVRAM access must be 32 bit cycle, only using bits 7:0
 *         o Always create DRAM particle pool
 *         o Read 32 bytes, not 64 bytes from MP eeprom
 *         o Correct some misdefined hw addresses
 *         o Make packet SRAM usage on Rev 1 optional, with default as
 *           no until hw is verified (define C7100_CPU_SRAM to use it)
 *
 * Revision 3.2.2.3  1995/12/08  19:21:33  dcarroll
 * Branch: ELC_branch
 * OIR and ENVMON improvements.
 *
 * Revision 3.2.2.2  1995/11/30  02:14:22  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1995/11/22  21:43:20  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  18:41:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:23:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Includes
 */
#include "master.h"
#include "logger.h"
#include "stacks.h"
#include "subsys.h"
#include "sys_registry.h"
#include "ttysrv.h"
#include "sched.h"
#include "interface_private.h"
#include "config.h"
#include "mgd_timers.h"

#include "../dev/monitor1.h"
#include "../os/gdb.h"

#include "../if/network.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/fddi.h"
#include "../if/if_controller.h"
#include "../wan/serial.h"

#include "../ui/debug.h"

#include "../pas/if_pas.h"
#include "../pas/msg_pas.c"		/* Not a typo, see logger.h */
#include "../hes/msg_oir.c"		/* Not a typo, see logger.h */
#include "../pas/pas_id.h"

#include "../dev/flash_dvr_spec.h"

#include "../hes/if_fci.h"
#include "../hes/ucode.h"
#include "../hes/slot.h"
#include "../hes/oir_debug_flags.h"
#include "../hes/parser_defs_ucode.h"

#include "../snmp/chassismib_registry.h"

#include "../src-4k/gt64010.h"
#include "../src-4k/vspace_4k.h"
#include "c7100_fpga.h"
#include "c7100_pcmap.h"

/*
 * oir info structure
 */
typedef struct oir_debug_t_ {
    ulong 	pa_status_lsw;
    int		int_clear_tries;
    int		stall_time;
    int		removal_time;
    ushort 	flags;
    tinybool	removed;
    tinybool	semi_removed;
    tinybool	insert_restart;
    tinybool	vaddr_problem;
    tinybool	slot_action;
} oir_debug_t;

/*
 * Globals
 */
slottype                slots[MAX_SLOTS];      /* slot based structure */
leveltype               oir_level;             /* global oir interrupt level */
boolean                 oir_init_mode;         /* global oir init flag */

/*
 * Local Storage
 */
static int              hotstall_count;        /* oir interrupt count */
static queuetype        hwidb_remQ;            /* queue for old idbs */
static watched_boolean  *oir_wakeup_boolean;   /* process activation boolean */
static sys_timestamp    oir_max_timer;         /* process activation 
						  timer max */
static sys_timestamp    oir_min_timer;         /* process activation timer 
						  minimum */
static tinybool         oir_retry;             /* insertion failure 
						  indication */
static uchar		oir_msg[MAX_PA_BAYS];  /* msgs between intr. and pro */

/*
 * Defines
 */
#define STALL_TIMEOUT   (ONESEC * 5)    /* give-up if stall lasts this long */
#define STALL_DEBOUNCE  100             /* milliseconds */
#define REMOVAL_TIMEOUT	(ONESEC * 2)	/* max amount of time to remove a pa */
#define REMOVAL_HYSTER	100		/* milliseconds */

#define OIR_MSG_SEMIREM	1		/* Half removed adaptor */

/*
 * c7100_remove_hwidbs:
 * Remove hwidbs for pulled interfaces
 */
void c7100_remove_hwidbs (slottype *sp) 
{
    hwidbtype 	*hwidb;
    idbtype 	*swidb;
    pas_cbtype 	*ds = NULL;

    sp->flags &= ~BOARD_ANALYZED;
    if (oir_debug) {
	buginf("\nOIR: Remove hwidbs for slot %d", sp->slot_num);
    }

    /*
     * Clean up this slot and turn off the power
     */
    pas_oir_cleanup(sp->slot_num);
    reg_invoke_chassismib_remove_card_entry((void *)(sp->slot_num + 1));
    pas_control_power(sp->slot_num, FALSE);
    if (TIMER_RUNNING(sp->insert_time)) {
	TIMER_STOP(sp->insert_time);
    }

    /*
     * Clean up the interfaces on this slot
     */
    while (sp->interface_count > 0) {

	hwidb = sp->interface_list[--sp->interface_count];
	if (hwidb == NULL) {
	    continue;
	}
	sp->interface_list[sp->interface_count] = NULL;

	/*
	 * Get the instance data structure and deallocate device driver
	 * resources
	 */
	ds = INSTANCE(hwidb);
	if (ds) {
	    (*(ds->pas_deallocate))(hwidb);
	}

	/*
	 * Take the sw/hw idb's off the various lists
	 */
	while (hwidb) {

	    unqueue(hwidbQ, hwidb);
	    holdq_reset(hwidb);
	    if (hwidb->status & IDB_ETHER) {
		ether_idb_dequeue(hwidb);
	    } else if (hwidb->status & IDB_SERIAL) {
		serial_idb_dequeue(hwidb);
	    } else if (hwidb->status & IDB_TR) {
		tring_idb_dequeue(hwidb);
	    } else if (hwidb->status & IDB_FDDI) {
		fddi_idb_dequeue(hwidb);
	    }

	    /*
	     * Take the sw idbs off of the hwidb
	     */
	    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb)
		unqueue(swidbQ, swidb);
	    enqueue(&hwidb_remQ, hwidb);
	    hwidb = hwidb->next_idb_on_physint;
	}
    }
}

/*
 * c7100_shutdown_pull_interfaces:
 * Walk idb queue and shut down any newly removed interfaces
 */
static void c7100_shutdown_pulled_interfaces (void)
{
    hwidbtype* idb;
    ulong state;
 
    FOR_ALL_HWIDBS(idb) {
        if (idb->status & IDB_NOHARDWARE) {
	    if (oir_debug) {
		buginf("\nOIR: Shutdown pulled interfaces for %s", 
		       idb->hw_namestring);
	    }
            state = idb->state;
            if (idb->state != IDBS_ADMINDOWN) {
                shutdown_going_down(idb);
	    }
            idb->pre_hotstall_state = state;
        }
    }
}
 
/*
 * c7100_init_new_insertions:
 * Find all interfaces that are brand new. This means that a
 * PA has been inserted in a slot that has never had this
 * card before. Perform the initialization that would have
 * been done from "idb final init" routine.
 */
static void c7100_init_new_insertions (void)
{
    hwidbtype 	*idb;
    idbtype 	*swidb;

    FOR_ALL_HWIDBS(idb) {

	if ((idb->oir_flags & IDB_FINAL_INIT)) { 
	    if (oir_debug) {
		buginf("\nOIR: Init new insertions for %s", 
		       idb->hw_namestring);
	    }
	    swidb = idb->firstsw;
	    idb_final_hw_init(idb);
	    idb_final_sw_init(swidb);
	    
	    /*
	     * Initialize protocol support in the swidb including hardware
	     * data structures.  Then initialize protocol support in
	     * the hwidb. Then setup fast switching.
	     * Code taken from "init new hwidb".
	     */
	    reg_invoke_sw_idb_init(swidb, TRUE);
	    reg_invoke_hw_idb_init(idb);
	    reg_invoke_fast_setup(idb);
	    idb->oir_flags &= ~IDB_FINAL_INIT;
	}
    }
}

/*
 * c7100_restart_reinsertions:
 * find interfaces that were removed and reinserted, and restore them
 * to their pre-removal state
 */
static void c7100_restart_reinsertions (void)
{
    hwidbtype *hwidb;
 
    FOR_ALL_HWIDBS(hwidb) {
        if ((hwidb->pre_hotstall_state != IDBS_ADMINDOWN)) {
            if (oir_debug) {
                buginf("\nOIR: Restart reinsertions for %s", 
		       hwidb->hw_namestring);
	    }
            hwidb->pre_hotstall_state = IDBS_ADMINDOWN;
            mgd_timer_stop(&hwidb->carriertimer);
            shutdown_coming_up(hwidb);
        }
    }
}

/*
 * c7100_process_oir_messages:
 * Process any and all messages coming from the interrupt handler
 */
static void c7100_process_oir_messages (void)
{
    int 	i;
    uchar 	msg;

    /*
     * Process and clear the message array
     */
    for (i=1; i<MAX_PA_BAYS; i++) {
	msg = oir_msg[i];
	oir_msg[i] = 0;
	if (msg == OIR_MSG_SEMIREM) {
	    errmsg(&msgsym(SEATED, OIR), i);
	}
    }
}

/*
 * c7100_oir_process
 * Process level code for pred oir support.
 */
forktype c7100_oir_process (void)
{
    boolean 	analyze, test_slots, work_to_do;
    ulong 	major, minor, time_remaining;
    char 	*wake_reason;
    slottype 	*sp;

    process_watch_boolean(oir_wakeup_boolean, ENABLE, RECURRING);
    process_watch_timer(&oir_max_timer, ENABLE);

    for (;;) {

	/*
	 * Wait for something to wake the process up.
	 * After being awakened, determine why.
	 *
	 * The boolean is from the interrupt handler. If
	 * it hasn't been a long enough break from this
	 * process, then go back to sleep and wake up from
	 * the timer. Otherwise, have fun in the analyze code.
	 *
	 * If the timer wakes up the process, it is either that there
	 * is an interface that failed its initialization and 
	 * needs to be retried, or the interrupt boolean woke up
	 * the process earlier and was put back to sleep until
	 * the minimum timer has passed. Also, since the analyze
	 * code does put the process to sleep while waiting for
	 * the pa hardware initialize, there may be a stale wakeup
	 * reason from that. Just nop those, based on if the max
	 * timer is running or not.
	 */
	process_wait_for_event();
	test_slots = analyze = FALSE;
	wake_reason = "No reason";

	while (process_get_wakeup(&major, &minor)) {

	    switch (major) {

	    /*
	     * Woke up by an OIR interrupt event
	     */
	    case BOOLEAN_EVENT:
		process_set_boolean(oir_wakeup_boolean, FALSE);
		if (TIMER_RUNNING_AND_SLEEPING(oir_min_timer)) {
		    time_remaining = TIME_LEFT_SLEEPING(oir_min_timer);
		    wake_reason = "Event(min timeout)";
		    test_slots = FALSE;
		    TIMER_STOP(oir_min_timer);
		    if (!TIMER_RUNNING(oir_max_timer)) {
			wake_reason = "Event(max not running)";
			TIMER_START(oir_max_timer, time_remaining);
		    }
		} else if (TIMER_RUNNING(oir_max_timer)) {
		    wake_reason = "Event(max running)";
		    test_slots = FALSE;
		    TIMER_STOP(oir_min_timer);
		} else {
		    wake_reason = "Event";
		    test_slots = TRUE;
		    TIMER_STOP(oir_min_timer);
		}
		break;

	    /*
	     * Woke up by the timer
	     */
	    case TIMER_EVENT:
		if (TIMER_RUNNING(oir_max_timer)) {
		    wake_reason = "Timer(max running)";
		    test_slots = TRUE;
		    TIMER_STOP(oir_max_timer);
		} else {
		    wake_reason = "Timer";
		}
		break;
	    }

	    /*
	     * Process any and all messages from the interrupt handler
	     */
	    c7100_process_oir_messages();
	    
	    /*
	     * Print some debug info
	     */
	    if (oir_debug) {
		buginf("\nOIR: Process woke, '%s', stall=%d, usec=%#x", 
		       wake_reason, hotstall_count, getcp0_count());
	    }
	}

	/*
	 * If there is stuff to do go thru the slots
	 */
	if (test_slots) {

	    /*
	     * Shut down any newly removed interfaces.
	     */
	    c7100_shutdown_pulled_interfaces();
	    c7100_oir_throttle(TRUE);

	    /*
	     * Per slot - Detect change in slot status and either analyze the
	     * inserted PA or deallocate the removed PA.
	     */
	    work_to_do = FALSE;
	    FOR_ALL_SLOTS(sp) {

		if ((sp->flags & BOARD_ANALYZED) && 
		    (sp->flags & BOARD_NONEXIST)) {

		    work_to_do = TRUE;
		    errmsg(&msgsym(REMCARD, OIR), sp->slot_num);
		    c7100_remove_hwidbs(sp);

		} else if ((!(sp->flags & BOARD_ANALYZED)) && 
			   (pas_pa_present(sp->slot_num))) {

		    work_to_do = TRUE;
		    if (sp->retry_ok && sp->retry_count) {
			/*
			 * Initializing or re-initializing a pa
			 */
			if (oir_debug) {
			    buginf("\nOIR: (Re)Init card %d, retry_count=%d",
				   sp->slot_num, sp->retry_count);
			}
			errmsg(&msgsym(INSCARD, OIR), sp->slot_num);
			analyze = TRUE;
		    } else {
			/*
			 * The adaptor is either broken or semi-plugged in
			 */
			if (oir_debug) {
			    buginf("\nOIR: Failed to init card %d", 
				   sp->slot_num);
			}
			errmsg(&msgsym(SEATED, OIR), sp->slot_num);
		    }
		}
	    }

	    if (oir_debug && !work_to_do) {
		buginf("\nOIR: Process woke for no apparent reason, %#x usec",
		       getcp0_count());
	    }

	    /*
	     * If there were any insertions, analyze them now
	     */
	    if (analyze) {
		oir_level = raise_interrupt_level(NETS_DISABLE);
		oir_init_mode = TRUE;
		pas_analyze_interfaces();
		oir_init_mode = FALSE;
		reset_interrupt_level(oir_level);
	    }

	    /*
	     * Turn off throttling IE restart net io interrupts.
	     */
	    c7100_init_new_insertions();
	    c7100_restart_reinsertions();

	    /*
	     * Start timers. Min timer is the minimum interval that
	     * the oir process can actually do any work. If the process
	     * is called before the min time is up, it won't check the
	     * slots for changes. The max timer is set if the process
	     * must be awakened to retry a failed insertion init.
	     */
	    TIMER_START(oir_min_timer, (5 * ONESEC));
	    if (oir_retry) {
		oir_retry = FALSE;
		TIMER_START(oir_max_timer, (10 * ONESEC));
	    }
	} else {
	    c7100_oir_throttle(TRUE);
	}
    }
}

/*
 * c7100_oir_changes:
 * Check the slot to determine whether a PA was inserted or
 * removed. If a PA was removed, alter the TLB to point to 
 * the bit bucket for that slot. This is called from the OIR interrupt
 * handling routine.
 */
static void c7100_oir_changes (int pa_bay, oir_debug_t *oir_info)
{
    sys_timestamp 	removal_time;
    slottype 		*sp = &slots[pa_bay];
    hwidbtype		*hwidb;
    int 		ix;

    /*
     * Init the oir info structure
     */
    oir_info->flags = sp->flags;
    oir_info->pa_status_lsw = C7100_MP_FPGA->pa_status_lsw;
    oir_info->removed = FALSE;
    oir_info->semi_removed = FALSE;
    oir_info->insert_restart = FALSE;
    oir_info->vaddr_problem = FALSE;

    /*
     * Check for a possibly inserted port adaptor. This can be detected by
     * a port adaptor issuing an interrupt, but having BOARD_NONEXIST
     * set in the slots data structure
     */
    if (sp->flags & BOARD_NONEXIST) {
	if (!sp->retry_ok) {
	    sp->retry_ok = TRUE;
	    sp->retry_count = 3;
	}
	oir_info->insert_restart = TRUE;
	return;
    }

    /*
     * Check for a removed Port Adapter. This is indicated by flags
     * that signify an existing board, but the hardware is not detecting that
     * board anymore. Note, to deal with really slow removals, we will
     * try very hard to wait until the board has been yanked.
     */
    TIMER_START(removal_time, REMOVAL_TIMEOUT);
    for (ix=0; ; ) {
	DELAY(1);
	if (!pas_pa_present(pa_bay)) {
	    ix += 1;
	} else {
	    ix = 0;
	}
	if ((ix > REMOVAL_HYSTER) || AWAKE(removal_time)) {
	    break;
	}
    }
    oir_info->removal_time = (REMOVAL_TIMEOUT - 
			      TIME_LEFT_SLEEPING(removal_time));

    if (pas_pa_present(pa_bay)) {
	/*
	 * The system exceeded the max allowed removal time. This board is
	 * half out, where it is powered down, but the pins are still
	 * connected enough to indicate that it is present. Set a message
	 * for the OIR process so we can inform the user
	 */
	oir_msg[pa_bay] = OIR_MSG_SEMIREM;
	oir_info->semi_removed = TRUE;
    }

    /*
     * Update slot flags
     */
    oir_info->removed = TRUE;
    sp->flags |= BOARD_NONEXIST;
    sp->retry_ok = FALSE;
    
    /* 
     * Alter the tlb entry for this slot. The tlb's physical address
     * should point to the bit bucket so that any outstanding writes
     * will not cause errors. Later at process level the tlb entry
     * will be invalidated via the power control routine.
     */
    if (!(c7100_bay2vaddr(sp->slot_num, FALSE, TRUE))) {
	oir_info->vaddr_problem = TRUE;
    }
    
    ix = sp->interface_count;
    while (ix-- > 0) {
	hwidb = sp->interface_list[ix];
	while (hwidb) {
	    hwidb->status |= IDB_NOHARDWARE;
	    hwidb = hwidb->next_idb_on_physint;
	}
    }
}

/*
 * c7100_print_oir_debug:
 * Dump out the accumulated info in the oir debug structure
 */
static void c7100_print_oir_debug (oir_debug_t oir_info[MAX_PA_BAYS])
{
    oir_debug_t *ptr;
    int 	i;

    for (i=1; i<MAX_PA_BAYS; i++) {
	ptr = &oir_info[i];
	if (!ptr->slot_action) {
	    continue;
	}

	/*
	 * Print the information for this slot
	 */
	ttyprintf(CONTTY, "\nOIRINT: Slot%d, clear_tries=%d, "
		  "removal_time=%d msec, stall_time=%d msec",
		  i, ptr->int_clear_tries, ptr->removal_time, 
		  ptr->stall_time);
	if (ptr->insert_restart) {
	    ttyprintf(CONTTY, "\nOIRINT: Slot%d, insert/restart", i);
	    continue;
	}
	ttyprintf(CONTTY, "\nOIRINT: Slot%d, flags=%#x, pa_status_lsw=%#x%s",
		  i, ptr->flags, ptr->pa_status_lsw,
		  (ptr->removed ? ", removed" : ""));
	if (ptr->semi_removed) {
	    ttyprintf(CONTTY, "\nOIRINT: Slot%d, semi-removed card", i);
	}
	if (ptr->vaddr_problem) {
	    ttyprintf(CONTTY, "\nOIRINT: Slot%d, vaddr conversion problem", i);
	}
    }
}

/*
 * c7100_oir_interrupt
 * Handle OIR interrupts. The parameter has bits 6:1 indicating
 * which bay produced the interrupt. Return TRUE if the box should be
 * brought down.
 *
 * Note: This interrupt handler runs at the error interrupt level
 * which is above ALL_DISABLE. So no utilities that use buffers can
 * be called from within this handler (errmsg, printf, buginf etc).
 * ttyprintf(CONTTY,...) is safe to be called.
 * 
 * Note: Throttling. Fine granularity throttling is achieved
 * on the C7100 by enabling / disabling netio interrupts according
 * to CPU allocations set by the user. The disabling occurs at the
 * net IO interrupt handler level. The enabling can occur either due
 * to the SCHED_LOOP_END code or due to a timer firing, signifying
 * that it is now time to run the net IO handlers again. The timer fires
 * at the same interrupt level as the OIR interrupt handler.
 * 
 * When an OIR event happens, we want all net IO interrupt activity
 * to stop, so that process level will run and handle the event.
 * To achieve this, the OIR interrupt handler should first stop
 * the GT64010 timer being used to re-enable net IO interrupts.
 * It should then turn off net IO interrupts. This can all be 
 * achieved by calling c7100_oir_throttle(FALSE);
 *
 * Process level code will then be free to run. When it wants to re-enable
 * net IO interrupts, it should call c7100_oir_throttle(TRUE);
 */
boolean c7100_oir_interrupt (ulong oir_event)
{
    sys_timestamp 	stall_debounce, stall_timeout;
    ulong 		error_int;
    int 		ix, iy;
    oir_debug_t		oir_info[MAX_PA_BAYS];

    /*
     * Find each PA that has an OIR indication
     */
    for (ix = 1; ix < MAX_PA_BAYS; ix++) {

	/*
	 * Anything action on this slot ?
	 */
	if (!(oir_event & (1 << ix))) {
	    oir_info[ix].slot_action = FALSE;
	    continue;
	}

	/*
	 * This bay has activity, so set the flag
	 */
	oir_info[ix].slot_action = TRUE;

	/*
	 * Reset the interrupt in the error interrupt status register
	 * for the PA found above.
	 */
	C7100_MP_FPGA->err_int_stat_lsw &= (1 << ix);
	hotstall_count += 1;
	TIMER_START(stall_timeout, STALL_TIMEOUT);
	TIMER_START(stall_debounce, STALL_DEBOUNCE);

	/*
	 * Debounce the interrupt line for the specific PA.
	 */
	for (iy = 0; ; ) {

	    /*
	     * Check for long stalls
	     */
	    if (AWAKE(stall_timeout)) {
		if (oir_debug) {
		    ttyprintf(CONTTY, "\nOIRINT: Long OIR interrupt, slot %d", 
			      ix);
		}
		return(TRUE);
	    }
	    
	    /*
	     * Debounce the effected slot IE wait for interrupt to go inactive
	     */
	    error_int = C7100_MP_FPGA->err_int_stat_lsw;
	    if (error_int & (1 << ix)) {
		iy += 1;
		C7100_MP_FPGA->err_int_stat_lsw &= (1 << ix);
		TIMER_START(stall_debounce, STALL_DEBOUNCE);
		continue;
	    }

	    /*
	     * If the debounce timer is awake, we are done debouncing
	     */
	    if (AWAKE(stall_debounce)) {
		break;
	    }
	}

	/*
	 * Save some diagnostics for this slot
	 */
	oir_info[ix].int_clear_tries = iy;
	oir_info[ix].stall_time = (STALL_TIMEOUT - 
				   TIME_LEFT_SLEEPING(stall_timeout));

	/* 
	 * Detect the change in the PA.
	 */
	c7100_oir_changes(ix, &oir_info[ix]);
    }

    /*
     * Note do this printout after analyzing the interrupts. This helps
     * prevent the debug printout from altering critical event timing
     */
    if (oir_debug) {
	ttyprintf(CONTTY, "\nOIRINT: Interrupt, event=%#x, usec=%#x", 
		  oir_event, getcp0_count());
	c7100_print_oir_debug(oir_info);
    }

    /* 
     * Call the OIR process.
     * Throttle the netio interrupt.
     */
    process_set_boolean(oir_wakeup_boolean, TRUE);
    c7100_oir_throttle(FALSE);
    return(FALSE);
}

/*
 * c7100_get_matching_idb
 *
 * Find a previously allocated idb that matches the 
 * target hardware, or allocate a new idb.
 *
 * Find an idb on the remQ that matches the parameters.
 * What this does is point you to an idb that was
 * previously allocated for a PA that was removed and 
 * reinserted in the same slot.
 */

hwidbtype* c7100_get_matching_idb (int slot, int port,
				   int pa_type, enum IDBTYPE idb_type)
{
    hwidbtype* hwidb;
    
    if (oir_debug) {
	buginf("\nOIR: Matching slot=%d, port=%d, pa_type=%d, idb_type=%#x",
	       slot, port, pa_type, idb_type);
    }

    /*
     * Search the hwidb remQ
     */
    for (hwidb = (hwidbtype*) hwidb_remQ.qhead;
	 hwidb != NULL;
	 hwidb = (hwidbtype*) hwidb->next) {

	if ((hwidb->fci_type == pa_type) &&
	    (hwidb->type == idb_type) &&
	    (hwidb->slot == slot) &&
	    (hwidb->slotunit == port)) {

	    unqueue(&hwidb_remQ, hwidb);
	    if (oir_debug) {
		buginf("\nOIR: Found a matching idb, slot=%d, port=%d, %s",
		       slot, port, hwidb->hw_namestring);
	    }
	    break;
	}
    }

    if (hwidb == NULL) {
	if (oir_debug)
	    buginf("\nOIR: No matching idb, creating a new one");
	hwidb = idb_create();
	if (hwidb) {
	    hwidb->oir_flags |= (IDB_FIRST_INIT | IDB_FINAL_INIT);
	    hwidb->pre_hotstall_state = IDBS_ADMINDOWN;
	    hwidb->state = IDBS_ADMINDOWN;
	}
    }
    return(hwidb);
}
	
/*
 * pas_deactivate_bay :
 * Put the specified port adaptor in reset
 */
void pas_deactivate_bay (uint pa_bay)
{
    slottype *sp;

    if (pa_bay >= MAX_PA_BAYS) {
	errmsg(&msgsym(BADPABAY, PA), pa_bay);
	return;
    }

    /*
     * Check for the IO slot
     */
    if (pa_bay == 0) {
	return;
    }
    sp = &slots[pa_bay];

    /*
     * Power down the PA and clean up
     */
    c7100_shutdown_pulled_interfaces();
    c7100_remove_hwidbs(&slots[pa_bay]);

    /*
     * Indicate that this PA needs to be initialized
     * again later. Only set this if the slot thinks it
     * is ok to and hasn't used up its max retries.
     */
    if ((sp->retry_ok) && (sp->retry_count)) {
	oir_retry = TRUE;
	sp->retry_count--;
    }
    if (!sp->retry_count) {
	sp->retry_ok = FALSE;
    }
}

/*
 * pas_oir_testport_enable
 * Enable/Disable the OIR testport on 7200 platforms.
 */
void c7100_oir_testport_enable (parseinfo *csb)
{
    uint midplane_version = C7100_MP_FPGA->pld_rev_lsw & 0xff;

    if (csb->nvgen) {
	if (midplane_version == 2) {
	    nv_write((C7100_MP_FPGA->mgmt_int_enbl_lsw & 
		      MGMT_INTR_ENABLE_OIRTEST),
		     "%s", csb->nv_command);
	} else {
	    nv_write(C7100_MP_FPGA->pa_tst_port_nsw & MGMT_INTR_ENABLE_OIRTEST,
		     "%s", csb->nv_command);
	}
	return;
    }

    if (csb->sense) {
	if (midplane_version == 2) {
	    C7100_MP_FPGA->mgmt_int_enbl_lsw |= MGMT_INTR_ENABLE_OIRTEST;
	} else {
	    C7100_MP_FPGA->pa_tst_port_nsw |= MGMT_INTR_ENABLE_OIRTEST;
	}
	if (oir_debug) {
	    buginf("\nOIR: Test port enabled");
	}
    } else {
	if (midplane_version == 2) {
	    C7100_MP_FPGA->mgmt_int_enbl_lsw &= ~MGMT_INTR_ENABLE_OIRTEST;
	} else {
	    C7100_MP_FPGA->pa_tst_port_nsw &= ~MGMT_INTR_ENABLE_OIRTEST;
	}
	if (oir_debug) {
	    buginf("\nOIR: Test port disabled");
	}
    }
}   

/*
 * c7100_oir_init
 * Called via a walk down the list of subsys headers.
 */
static void c7100_oir_init (subsystype *subsys)
{
    hotstall_count = 0;
    queue_init(&hwidb_remQ, 0 /* no maximum */);
    oir_wakeup_boolean = create_watched_boolean("OIR wakeup boolean", 0);
    oir_retry = FALSE;
    oir_init_mode = FALSE;

    if (NO_PROCESS == process_create(c7100_oir_process,
				     "OIR Handler",
				     NORMAL_STACK,
				     PRIO_NORMAL)) {
	return;
    }
    oir_parser_init();
    oir_debug_init();
}

#define OIR_MAJVERSION 1
#define OIR_MINVERSION 0
#define OIR_EDITVERSION 1

SUBSYS_HEADER(oir,
	      OIR_MAJVERSION,
	      OIR_MINVERSION,
	      OIR_EDITVERSION,
              c7100_oir_init, 
	      SUBSYS_CLASS_KERNEL,
              NULL,
              NULL);

/* end of file */
