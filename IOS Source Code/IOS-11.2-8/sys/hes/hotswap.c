/* $Id: hotswap.c,v 3.6.10.5 1996/08/28 12:44:09 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/hotswap.c,v $ 
 *------------------------------------------------------------------
 * hotswap.c - hot swap support
 * 
 * William H. Palmer, Fri Apr 17 12:34:34 1992  
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: hotswap.c,v $
 * Revision 3.6.10.5  1996/08/28  12:44:09  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.10.4  1996/07/08  22:25:06  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.6.10.3  1996/06/28  23:06:14  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.6.10.2  1996/05/25  22:39:27  shaker
 * CSCdi57620:  Missing check for null return from IDB allocation
 * Branch: California_branch
 * Add checks for NULL returns from allocation routines. Fixes
 * 'day one' bug in C7000.
 *
 * Revision 3.6.10.1  1996/03/18  19:40:49  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.16.4  1996/03/16  06:49:57  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.16.3  1996/03/14  00:56:29  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.5.16.2  1996/03/07  09:39:10  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.16.1  1996/01/24  21:45:43  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.6  1996/02/23  10:15:25  feisal
 * CSCdi45132:  Crash when remove VIP when on IF-CON to VIP
 *
 * Revision 3.5  1995/12/07  22:28:31  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.4  1995/12/06  02:15:29  motto
 * CSCdi43046:  Shutdown interfaces automatically change to no shut after
 * mic reload
 *
 * Revision 3.3  1995/11/17  09:16:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:26:41  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:39:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/21  07:40:02  hampton
 * Transition the hes directory code to use the passive timer macros for
 * all its timers.  It no longer references the system clock directly.
 * [CSCdi39033]
 *
 * Revision 2.3  1995/07/31 21:28:44  schaefer
 * CSCdi37941: OIR processes should block
 * remove edisms
 *
 * Revision 2.2  1995/06/28  09:24:29  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:40:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "subsys.h"
#include "interface_private.h"
#include "logger.h"
#include "stacks.h"
#include "mgd_timers.h"
#include "if_mci.h"
#include "if_fci.h"
#include "if_hes_common.h"
#include "if_cmd.h"
#include "../if/tring.h"
#include "if_trip.h"
#include "../if/if_controller.h"
#include "../hes/hes_controller.h"
#include "../if/fddi.h"
#include "../if/fddi_private.h"
#include "dbus.h"
#include "ucode.h"
#include "../if/network.h"
#include "../if/dsx1.h"
#include "if_mip.h"
#include "oir_debug_flags.h"
#include "ttysrv.h"
#include "msg_oir.c"			/* Not a typo, see logger.h */

 
extern queuetype cbuspreQ;	/* preallocated memory for cbus_analyze  */
extern queuetype idbpreQ;	/* on a hot swap interrupt */
extern queuetype dcipreQ;

watched_boolean* oir_process_wakeup;	/* wake-up the handler process */

boolean eoir_capable;		/* box can/should do EOIR */
static boolean eoir_active;	/* doing an EOIR cycle */

static int save_sched_run_interval;


/*
 * hotstall_interrupt
 * handle a hot stall interrupt from the switch processor
 */

void hotstall_interrupt (void)
{
    mcitype *cbus;
    mciregtype *regptr;
    sys_timestamp hotstall_time, debounce_time, longstall_time;
    ulong hotstall_length, cbus_analyze_time, dgbus_analyze_time;
    sys_timestamp start_time;
    boolean stall_active;
    ushort saved_argreg;
    uint onint = onintstack();
#define BUFLEN 160
    char errbuf[BUFLEN];

#define HOTSTALL_DELAY_TIME 1000
#define HOTSTALL_DEBOUNCE_TIME 50
#define HOTSTALL_POWERUP_DELAY 150
#define HOTSTALL_EXCESSIVE_STALL_TIME (5*ONESEC)    
    
    GET_TIMESTAMP(hotstall_time);
    GET_TIMESTAMP(debounce_time);
    TIMER_START(longstall_time, HOTSTALL_EXCESSIVE_STALL_TIME);

    stall_active = TRUE;
    eoir_active = FALSE;
/*
 * We come here on a level 4 interrupt from the Switch Processor.  We need 
 * to determine, in concert with the SP, when the hot stall condition has gone
 * away, so that we can arrange for cbus_analyze to be called.
 */

    cbus = (mcitype*)cbusQ.qhead;
    regptr = cbus->regptr; 
    saved_argreg = regptr->argreg;
    if (onint) {
	if (mciQ.count || !eoir_capable) {
	    DELAY(HOTSTALL_DELAY_TIME); /* allow cards to power up/down */
	} else {
	    eoir_active = TRUE;
	    /* debounce fragment */
	    /* 
	     * termination condition:
	     * stall is not active
	     * stall has not been active for HOTSTALL_DEBOUNCE_TIME
	     * time of last transition to stall inactive in debounce_time 
	     */
	    
	    while (CLOCK_IN_INTERVAL(debounce_time, HOTSTALL_DEBOUNCE_TIME) ||
		   stall_active) {
		if (TIMER_RUNNING_AND_AWAKE(longstall_time)) {
		    memset(errbuf, 0, BUFLEN);
		    errfmt(&msgsym(LONGSTALL, OIR), errbuf, BUFLEN,
			   ELAPSED_TIME(hotstall_time)/ONESEC);
		    ttyprintf(CONTTY, "%s", errbuf);
		    TIMER_START(longstall_time, HOTSTALL_EXCESSIVE_STALL_TIME);
		}
		regptr->argreg = FCI_HS_STALL_SENSE; /* ck if stall active */
		regptr->cmdreg = FCI_CMD_HS_OP;
		if (regptr->argreg == MCI_RSP_OKAY) { /* stall not active */
		    if (stall_active) {
			GET_TIMESTAMP(debounce_time); /* stall went away */
			stall_active = FALSE;
		    }
		} else {		/* stall active */
		    stall_active = TRUE;
		    while (stall_active) {
			regptr->argreg = FCI_HS_STALL_SENSE;
			regptr->cmdreg = FCI_CMD_HS_OP;
			if (regptr->argreg == MCI_RSP_OKAY) {
			    GET_TIMESTAMP(debounce_time);
			    stall_active = FALSE;
			}
			if (TIMER_RUNNING_AND_AWAKE(longstall_time)) {
			    memset(errbuf, 0, BUFLEN);
			    errfmt(&msgsym(LONGSTALL, OIR), errbuf, BUFLEN,
				   ELAPSED_TIME(hotstall_time)/ONESEC);
			    ttyprintf(CONTTY, "%s", errbuf);
			    TIMER_START(longstall_time,
					HOTSTALL_EXCESSIVE_STALL_TIME);
			}
		    }
		}
	    }
	}
	/* end of debounce fragment */
	if (eoir_capable) {
	    regptr->argreg = FCI_INTLEVEL; /* restore normal interrupt level */
	    regptr->cmdreg = FCI_CMD_INTERRUPT;
	    /*
	     * This delay is to allow any newly inserted cards to finish
	     * powering up and initializing the dbus interface.  Schedule
	     * this delay via the process-level code scheduler test to
	     * minimize stall time.
	     */
#define CIP_POWERUP_DELAY 3*ONESEC

	    ucode_loader_wakeup(HOTSTALL_POWERUP_DELAY -
				HOTSTALL_DEBOUNCE_TIME + CIP_POWERUP_DELAY);
	}
    }
    
    if (oir_debug) {
	buginf("\nStall detected, debounced, duration %d ms",
	       ELAPSED_TIME(hotstall_time));
    }
    
    /*
     * Now we must analyze the cards in the box and decide
     * what changed.  Find any newly inserted cards, identify any
     * removed cards, and note any cards which will require new
     * microcode to be loaded (especially the switch processor).
     * If the switch processor needs new microcode, we need to
     * do the old, painful OIR cycle.  
     */
    
    if (!eoir_active) {
	GET_TIMESTAMP(start_time);
	dgbus_analyze(FALSE);	/* look for new cards and update slot db */
	dgbus_analyze_time = ELAPSED_TIME(start_time);
	GET_TIMESTAMP(start_time);
	cbus_analyze(FALSE, FALSE);	
	cbus_analyze_time = ELAPSED_TIME(start_time);
	hotstall_flag = TRUE;	/* signal process level to clean up */
	process_set_boolean(oir_process_wakeup, TRUE);
    } else {
	if (oir_debug)
	    buginf("\nMarking removed cards BOARD_NONEXIST");
	mark_removed_cards();   /* scan the dbus and mark removed cards */
    }
    hotstall_flag = TRUE;
    hotstall_length = ELAPSED_TIME(hotstall_time);

/* 
 *  Now send signal to SP that we are done with this HS event's interrupt
 *  processing, but that it should ignore commands from the RP until we tell
 *  it to resume normal processing, from the interrupt level code.  This is to 
 *  prevent problems where we are in the level 3 cbus interrupt handler when
 *  we receive a level 4 stall interrupt.  N.B. We only do this if we were
 *  called by a hotstall interrupt, *not* if we were just downloading new
 *  microcode from process level.  This is not needed by EOIR.
 */
    if (onint && !eoir_capable) {
	for (cbus = (mcitype*)cbusQ.qhead; cbus; cbus = cbus->next) {
	    cbus->regptr->argreg = FCI_HS_DONOTHING_MODE;
	    cbus->regptr->cmdreg = FCI_CMD_HS_OP;
	}
    }
    /*
     *  We need to guarantee that our process level routine will get to run.
     *  If scheduler-interval is not set, it won't get called if we are
     *  under heavy load.  So fake out the interval time, if necessary.
     */
    if (sched_run_interval != HOTSWAP_SCHED_INTERVAL) {
	save_sched_run_interval = sched_run_interval;
	sched_run_interval = HOTSWAP_SCHED_INTERVAL;
    }
    if (oir_debug && eoir_active) {
	buginf("\nsignalled ucode_loader");
    }
    if (eoir_active) {
	/*
	 * Turn on bus and restore previous response register
	 */
	regptr->argreg = FCI_HS_NORMAL_MODE;
	regptr->cmdreg = FCI_CMD_HS_OP;
	regptr->argreg = saved_argreg;	/* restore saved argreg */
    }
}

void hotswap_preallocate_memory (boolean more)
{
    mcitype *cbus;
    int i;
    hwidbtype *idb;
    dcitype *dci;
    cdbtype *cdb;

    if (!more) {
	queue_init(&cbuspreQ, 0);	
	queue_init(&idbpreQ, 0);
	queue_init(&dcipreQ, 0);
	queue_init(&cdbpreQ, 0);
    }
    for (i = cbuspreQ.count; i < 2; i++) {
	cbus = malloc_named(sizeof(mcitype), "CBUS Controller");
	if (cbus)
	    enqueue(&cbuspreQ, cbus);
    }
    for (i = idbpreQ.count; i < CBUS2_MAX_CTRLR_INTERFACES*5; i++) {
	idb = idb_create();
	if (idb) {
	    idb->ctr_private = malloc_named(sizeof(spyglass_data_t),
					    "TRIP data");
	    idb->phy[0] = malloc_named(sizeof(phytype), "FIP Data");
	    idb->phy[1] = malloc_named(sizeof(phytype), "FIP Data"); 
	    enqueue(&idbpreQ, idb);
	}
    }
    for (i = dcipreQ.count; i < FCI_MAX_DCI_CTRLRS; i++) {
	dci = malloc_named(sizeof(dcitype), "CBUS Card");
	if (dci)
	    enqueue(&dcipreQ, dci);
    }
    for (i = cdbpreQ.count; i < CBUS2_MAX_CTRLR_INTERFACES; i++) {
	cdb = malloc_named(sizeof(cdbtype), "CDB Structure");
	if (cdb)
	    enqueue(&cdbpreQ, cdb);
    }
    mip_preallocate(more);	/* MIP-specific preallocation */
}

/* 
 * walk idb queue and initialize any newly inserted interfaces
 */
static void init_inserted_interfaces (void)
{
    hwidbtype *idb;

    if (oir_debug)
	buginf("\n  Starting init_inserted_interfaces()");
    FOR_ALL_HWIDBS(idb) {
	if (!(idb->status & IDB_NOHARDWARE) &&
	    (idb->state == IDBS_ADMINDOWN) &&
	    (idb->oir_flags & IDB_NEEDS_INIT) && (idb->status & IDB_CBUS)) { 
	    init_new_hwidb(idb);
	    idb->oir_flags &= ~IDB_NEEDS_INIT;
	}
    }
    if (oir_debug)
	buginf("\n  Ending init_inserted_interfaces()");
}

/* 
 * walk idb queue and start up any newly inserted interfaces
 */
static void start_inserted_interfaces (void)
{
    hwidbtype* idb;

    if (oir_debug)
	buginf("\n  Starting start_inserted_interfaces()");
    FOR_ALL_HWIDBS(idb) {
	if ( (!(idb->status & IDB_NOHARDWARE)) &&
	    (idb->state == IDBS_ADMINDOWN) &&
	    (idb->pre_hotstall_state != IDBS_ADMINDOWN) &&
	    (idb->status & IDB_CBUS) ) { 
            if (oir_debug)
                buginf("\nBringing up %s, pre_hotstall_state=%d",
		       idb->hw_namestring, idb->pre_hotstall_state);
            idb->pre_hotstall_state = IDBS_ADMINDOWN;
            mgd_timer_stop(&idb->carriertimer);
	    shutdown_coming_up(idb);
	}
    }
    if (oir_debug)
	buginf("\n  Ending start_inserted_interfaces()");
}

/*
 * oir_init_name_strings
 * Make sure any newly created idbs or cdbs have name strings set 
 */
static void oir_init_name_strings (void)
{
    hwidbtype *hwidb;
    cdbtype *cdb;

    FOR_ALL_HWIDBS(hwidb) {
	idb_init_names(hwidb->firstsw, FALSE);
    }
    FOR_ALL_CDBS(cdb) {
	init_cdb_name(cdb);
    }
}

/*
 * check_eoir_capable
 * see if current SP microcode can do EOIR processing 
 */
void set_eoir_capable (ushort slot)
{
    boolean result = FALSE;
    mciregtype *regptr;

    regptr = (mciregtype*)(slot*MCIBYTES+(uchar*)FCISTART);
    regptr->cmdreg = FCI_AS_CAPABILITY;
    if (regptr->cmdreg == FCI_RSP_OKAY)
	if (regptr->argreg & FCI_CAP_OIR_RESET)
	    result = TRUE;
    eoir_capable = result;
    return;
}


/*
 * hotswap_proc_level
 * Take care of the various tasks we can't readily do at interrupt
 * level.
 */

static void hotswap_proc_level (void)
{
    mcitype		*cbus;
    leveltype		level;
    ulong		cbus_analyze_time, dgbus_analyze_time;
    sys_timestamp	start_time;

    process_watch_boolean(oir_process_wakeup, ENABLE, RECURRING);
    while (TRUE) {
	process_wait_for_event();
	level = raise_interrupt_level(NETS_DISABLE);
	process_set_boolean(oir_process_wakeup, FALSE);
	hotswap_preallocate_memory(TRUE); /* get more memory if needed */
	cbus = (mcitype*)cbusQ.qhead;
	if (oir_debug)
	    buginf("\n##Starting hotswap_proc_level(), cbus=%#x, eoir_active=%d",
		   cbus, eoir_active);
	if (!cbus)
	    continue;
	if (eoir_active) {
	    GET_TIMESTAMP(start_time);
	    dgbus_analyze(TRUE); /* look for new cards and update slot db */
	    dgbus_analyze_time = ELAPSED_TIME(start_time);
	    sp_eoir_quiescere(cbus);
	    DELAY(EOIR_BUFFER_RETURN_TIME);
	    reset_interrupt_level(level); /* allow ints momentarily
					     for love notes */
	    level = raise_interrupt_level(NETS_DISABLE);
	    GET_TIMESTAMP(start_time);
	    cbus_analyze(FALSE, TRUE); /* check out new cards */
	    cbus_analyze_time = ELAPSED_TIME(start_time);
	} else {
	    /*
	     * inform SP to resume normal processing now that we are sure
	     * that all is well
	     */
	    cbus->regptr->argreg = FCI_HS_NORMAL_MODE;
	    cbus->regptr->cmdreg = FCI_CMD_HS_OP;
	}

	/* stop forcing user tasks to be scheduled so frequently */
	sched_run_interval = save_sched_run_interval;
	hotstall_flag = FALSE;

	reset_interrupt_level(level);

	if (hotstall_smt_flag) {
	    hotstall_smt_flag = FALSE;
	    start_smt_input_proc();
	}

	oir_init_name_strings();

	/* initialize any inserted interfaces */
	init_inserted_interfaces();

	/* flush routing table of any bogosity from interfaces being yanked */
	shutdown_pulled_interfaces();

	/* turn on any new interfaces */
	start_inserted_interfaces();

	if (oir_debug)
	    buginf("\n##Ending hotswap_proc_level()");
    }
}


/*
 * hotswap_init
 * Initialize hot swap data structures.  Pre-allocate memory for daughter
 * controllers and idbs since that cannot be done at interrupt level.
 */

static void hotswap_init (subsystype *subsys)
{
    oir_parser_init();
    oir_debug_init();
    hotstall_flag = FALSE;
    oir_process_wakeup = create_watched_boolean("OIR wakeup", 0);
    hotswap_determine_system_mac_addr();
    process_create(hotswap_proc_level, "CXBus hot stall",
		   NORMAL_STACK, PRIO_NORMAL);
    createlevel(HOTSTALL_INTLEVEL, hotstall_interrupt, "CXBus hot stall");
    hotswap_preallocate_memory(FALSE);
    eoir_capable = FALSE;
}

#define OIR_MAJVERSION 2
#define OIR_MINVERSION 0
#define OIR_EDITVERSION 1

SUBSYS_HEADER(oir, OIR_MAJVERSION, OIR_MINVERSION, OIR_EDITVERSION,
	      hotswap_init, SUBSYS_CLASS_KERNEL,
	      NULL,
	      NULL);
