/* $Id: rsp_oir.c,v 3.10.6.14 1996/08/28 13:14:18 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_oir.c,v $
 *------------------------------------------------------------------
 * OIR for RSP
 *
 * March 1994, Carl Schaefer
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_oir.c,v $
 * Revision 3.10.6.14  1996/08/28  13:14:18  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.10.6.13  1996/07/25  00:24:44  ganesh
 * CSCdi63845:  Slave OIR reload fix in CSCdi57076 fails on rsp1, rsp7000.
 * Branch: California_branch
 * Before accessing master-slave command,status registers check if the
 * system we are on is dual capable. RSP1, rsp7000 systems are not
 * dual capable and therefore they do not have master, slave registers.
 *
 * Revision 3.10.6.12  1996/07/17  18:13:00  ganesh
 * CSCdi57076:  HSA Slave excision/removal crashes 7513--Router
 * RELOADS-OIR Fails
 * Branch: California_branch
 * Master RSP may lose its Master status sometimes during slave OIR!.
 * Software workaround is to reclaim Master status by writing into
 * master-slave command register.  A hardware ddts bug has been filed
 * to probe further.(CSCdi63040)
 *
 * Revision 3.10.6.11  1996/07/08  22:52:51  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.10.6.10  1996/06/26  02:21:10  ganesh
 * CSCdi61231:  EOIR may crash rsp(improve debounce algorithm).
 * Branch: California_branch
 * Look at both LATCH and UNLATCH bits to decide if stall is
 * debounced.
 *
 * Revision 3.10.6.9  1996/06/07  19:02:37  kramling
 * CSCdi55236:  Problems with FEIP OIR
 * Branch: California_branch
 *
 * Revision 3.10.6.8  1996/06/05  05:09:31  kao
 * CSCdi53846:  EOIR of EIP wipes route cache of entries for that interface
 * No cache invalidation during EOIR
 * Branch: California_branch
 *
 * Revision 3.10.6.7  1996/06/01  16:01:17  kao
 * CSCdi57759:  VIP2 system exception & DBUS-3-DBUSINTERR error when doing
 * eoir.
 * Flush DFS/IPC message before memd recarve to have a consistent DFS
 * cache after EOIR.
 * Branch: California_branch
 *
 * Revision 3.10.6.6  1996/05/31  22:04:37  motto
 * CSCdi54224:  CSNA Virtual Port 2 disappeared when new Internal LAN
 * added
 * Branch: California_branch
 *    Dbus/Cbus errors caused hwidbs to be removed from the hwidbQ and
 *    placed then put back on, but not in proper order. (restore_idbs
 *    function
 *    did not put back in order.)
 *
 * Revision 3.10.6.5  1996/05/21  06:40:30  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.10.6.4  1996/05/09  14:46:19  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.10.6.3.4.1  1996/04/27  07:26:05  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.10.6.3  1996/04/22  18:16:27  gstovall
 * CSCdi53071:  media idb init routines will fail on second pass
 * Branch: California_branch
 * Allow media idb init routines to be called again.  Also clean up the
 * handling of the media idb queues.
 *
 * Revision 3.10.6.2  1996/04/06  18:48:33  foster
 * CSCdi52519:  HSA: Master/slave functionality fails after master
 * remove/rerplace
 * Branch: California_branch
 *
 * Revision 3.10.6.1.8.1  1996/04/08  02:09:32  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.10.6.1  1996/03/18  22:02:32  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.10.10.1  1996/03/23  04:54:48  rlowe
 * More sync-time fun and games.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.10  1996/03/06  19:13:06  foster
 * CSCdi49134:  HSA : OIR event causes router to crash
 * don't reset the slave if it is running durning oir
 *
 * Revision 3.9  1996/03/04  07:14:09  kao
 * CSCdi49674:  While VIP is cofigured with DFS, OIR event will crash the
 * VIP; invalidate all IP cache during MEMD recarve
 *
 * Revision 3.8  1996/03/04  00:40:25  dbath
 * CSCdi50569:  CCB timeouts during EOIR events.
 * Improve accuracy of comments, as suggested by code reviewer.
 *
 * Revision 3.7  1996/03/03  07:34:34  dbath
 * CSCdi50569:  CCB timeouts during EOIR events.
 * Ensure that nobody relinquishes the cpu between the
 * time we issue the quiesce commands, and the time we
 * get the IPs back up on their feet, by protecting
 * the region with blocking_disabled.
 *
 * Revision 3.6  1996/02/28  09:33:53  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.5  1995/12/28  08:25:00  foster
 * CSCdi46157:  Dbus down-load needs to work on rsp2 slave down-load
 *
 * Revision 3.4  1995/12/06  18:32:40  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.3  1995/11/17  18:49:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:33:12  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:28:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/10/11  07:10:48  mcieslak
 * Reload the microcode for a slot that has failed and allow
 * for IPs that are disabled to be removed cleanly.
 * CSCdi41907:  complex restart / EOIR of disabled cards broken
 *
 * Revision 2.8  1995/10/05  18:43:10  motto
 * CSCdi40680:  OIR with CIP internal LANs, Not all internal LANs get
 * added back
 *
 * Revision 2.7  1995/07/31  21:29:09  schaefer
 * CSCdi37941: OIR processes should block
 * remove edisms
 *
 * Revision 2.6  1995/07/17  08:22:06  schaefer
 * CSCdi36181: more robust microcode management (on RSP)
 *
 * Revision 2.5  1995/07/16  22:31:07  getchell
 * CSCdi35501:  src-rsp/ccb_cmd.h should be moved to hes
 * src-rsp/[ccb.h,ccb_cmd.h] combined and moved to hes/ccb_cmd.h.
 * rsp_command prototypes removed from ccb_cmd.h and moved to
 * src-rsp/rsp_commands.h.  Files including altered files modifed to
 * reflect new file structure.
 *
 * Revision 2.4  1995/07/14  00:53:24  schaefer
 * CSCdi37087: nnets not adjusted for new online-inserted IPs
 *
 * Revision 2.3  1995/06/19  07:17:14  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  01:13:53  schaefer
 * CSCdi35057: LONGSTALL + MIP = cbus error
 * on RSP, incomplete OIR causes MIP to generate invalid cbus cycle
 *
 * Revision 2.1  1995/06/07  22:58:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "logger.h"
#include "interface_private.h"
#include "stacks.h"
#include "mgd_timers.h"
#include "../if/network.h"
#include "../hes/ucode.h"
#include "../hes/dbus.h"
#include "../hes/dgbus.h"
#include "../hes/if_fci.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"
#include "../hes/if_cmd.h"
#include "../hes/if_hes_common.h"
#include "../if/if_controller.h"
#include "../hes/hes_controller.h"
#include "../hes/ccb_cmd.h"
#include "../ui/debug.h"
#include "../hes/oir_debug_flags.h"
#include "rsp_bufhdr.h"
#include "rsp_if.h"
#include "rsp_qasic.h"
#include "rsp_pcmap.h"
#include "rsp_common_if.h"
#include "../snmp/chassismib_registry.h"
#include "../hes/cbus_registry.h"
#include "../if/tring.h"
#include "../if/fddi.h"
#include "../hes/if_hes_serial.h"
#include "../wan/serial.h"
#include "../hes/msg_oir.c"		/* Not a typo, see logger.h */
#include "../src-rsp/msg_rsp.c"		/* Not a typo, see logger.h */
#include "sys_registry.h"
#include "../ip/ipfast_rsp_ipc.h"


/*
 * queue of removed hwidbs
 */
static queuetype hwidb_remQ;

/*
 * something to wake-up the OIR handler process
 */
watched_boolean* oir_process_wakeup;

/*
 * not really the interrupt handler on the RSP, but the name
 * is fixed because it's called by the common "microcode reload"
 * command to reset, reload microcode, restart IPs
 */

void hotstall_interrupt (void)
{
    slottype* sp;

    if (oir_debug)
	buginf("\nStarting reload");

    FOR_ALL_ENABLED_IP_SLOTS(sp)
	disable_slot(sp->slot_num);

    if (oir_debug)
	buginf("\nDbus analyze");

    dgbus_analyze(FALSE /* not EOIR */);

    if (oir_debug)
	buginf("\nCbus analyze");

    cbus_analyze(FALSE /* not the first time */, FALSE /* not EOIR */);

    /*
     * return if executing "micro reload" from on-board config
     */
    if (!system_configured)
	return;

    if (oir_debug)
	buginf("\nRecarving");

    rsp_memd_carve(FALSE /* not EOIR */);

    if (oir_debug)
	buginf("\nReload done");

    return;
}

/*****************************************************************************/

/*
 * send quiesce commands to all interfaces that aren't shutdown
 * finds IDBs from the arrays in the slot structures
 */
static void
send_quiesce_cmds (void)
{
    slottype* sp;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    FOR_ALL_ENABLED_IP_SLOTS(sp) {
	int ix;

	/*
	 * Don't try to QUIESCE boards that were just inserted
	 * and haven't been reinitialized yet:
	 */
	if (sp->flags & BOARD_RELOADED) {
	    if (oir_debug) {
		buginf("\nskipping newly (re)installed %s in slot %d",
		       sp->card_name, sp->slot_num);
	    }
	    continue;
	}

	if ((sp->ctrlr_type == FCI_ETHER_CONTROLLER) ||
	   (sp->ctrlr_type == FCI_FEIP_CONTROLLER)) {
	    disable_slot(sp->slot_num);
	    sp->flags |= BOARD_CCBNEEDED;
	    continue;
	}

	/*
	 * Check for EOIR capable MIP hardware, and
	 * don't try to EOIR old versions.
	 */
	if ((sp->ctrlr_type == FCI_MIP_CONTROLLER) &&
	    (sp->hw_version == MIP_NO_EOIR_VERSION)) {
	    disable_slot(sp->slot_num);
	    sp->flags |= BOARD_CCBNEEDED;
	    continue;
	}

	for (ix = 0; ix < sp->interface_count; ++ix) {
	    hwidbtype*	hwidb;
	    ushort	result;

	    hwidb = sp->interface_list[ix];
	    if (hwidb == NULL)
		continue;

	    /*
	     * Do QUIESCE for base IDBs (for MIP) even when 'shutdown':
	     */
	    if (hwidb->state == IDBS_ADMINDOWN) {
		/*
		 * This is a quick fix for no QUIESCE when all interfaces
		 * on certain IPs sre in shutdown state.
		 * Once new per slot QUIESCE/UNQUIESCE command is implemented,
		 * this special FCI_xxx_CONTROLLER check will be removed.
		 */
                if (((hwidb->fci_type & FCI_TYPE_BASE_FLAG) == 0) &&
		     (sp->ctrlr_type != FCI_SVIP_CONTROLLER) &&
		     (sp->ctrlr_type != FCI_RVIP_CONTROLLER) &&
		     (sp->ctrlr_type != FCI_CIP_CONTROLLER)) {
		    continue;
		}
	    }

	    /*
	     * QUIESCE this interface so we can recarve MEMD without
	     * the interface poking into MEMD while we are doing it:
	     */
	    select_if(hwidb);
	    result = send_status_cmd(hwidb, FCI_STATUS_QUIESCE);
	    if (result != CMD_RSP_OKAY) {
		if (oir_debug) {
		    buginf("\nQuiescing %s returned %x",
			   hwidb->hw_namestring, result);
		}
		disable_slot(sp->slot_num);
		sp->flags |= BOARD_CCBNEEDED;
	    }
	}
    }
    reset_interrupt_level(level);
    return;
}

/*
 * do the work to restore from the system removed IDB queue the IDBs for the
 * given slot
 */
void
restore_idbs (slottype* sp)
{
    int intfc_indx;

    /* Ensure that the IDBs are currently removed. */
    if  ((sp->flags & BOARD_IDBSREMOVED) == 0)
	return;

    for (intfc_indx = 0; intfc_indx < sp->interface_count; intfc_indx++) {
	hwidbtype* hwidb;

	hwidb = sp->interface_list[intfc_indx];
	if (hwidb == NULL)
	    continue;

	/*
	 * treat "base" IDBs differently (e.g. MIP)
	 * base IDB isn't on system queue, but its sub-IDBs are
	 */
	unqueue(&hwidb_remQ, hwidb);
	if (hwidb->fci_type & FCI_TYPE_BASE) {
	    hwidb = hwidb->next_idb_on_physint;
	}		    

	while (hwidb) {
            media_idb_enqueue(hwidb);
	    hwidb = hwidb->next_idb_on_physint;
	}
    }
    sp->flags &= ~BOARD_IDBSREMOVED;
    return;
}

static void idb_removal (hwidbtype *hwidb) {
    idbtype *swidb;

    unqueue(hwidbQ, hwidb);

    if (hwidb->status & IDB_ETHER) {
	ether_idb_dequeue(hwidb);
    } else if (hwidb->status & IDB_SERIAL) {
	serial_idb_dequeue(hwidb);
    } else if (hwidb->status & IDB_TR) {
	tring_idb_dequeue(hwidb);
    } else if (hwidb->status & IDB_FDDI) {
	fddi_idb_dequeue(hwidb);
    }
    holdq_reset(hwidb);
    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb)
	unqueue(swidbQ, swidb);
}

/*
 * do the work to remove from the system IDB queues the IDBs for the
 * given slot
 */
void
remove_idbs (slottype* sp)
{
    int intfc_indx;

    /* A crashed card may already have its IDBs removed. */
    if (sp->flags & BOARD_IDBSREMOVED)
	return;

    for (intfc_indx = 0; intfc_indx < sp->interface_count; intfc_indx++) {
	hwidbtype* hwidb;

	hwidb = sp->interface_list[intfc_indx];
	if (hwidb == NULL)
	    continue;

	/*
	 * treat "base" IDBs differently (e.g. MIP)
	 * base IDB isn't on system queue, but its sub-IDBs are
	 */
	if (!(hwidb->fci_type & FCI_TYPE_BASE)) {
            idb_removal(hwidb);
        }	    
        enqueue(&hwidb_remQ, hwidb);
	hwidb = hwidb->next_idb_on_physint;
	while (hwidb) {
            idb_removal(hwidb);
	    hwidb = hwidb->next_idb_on_physint;
	}
    }
    sp->flags |= BOARD_IDBSREMOVED;
    return;
}

/*
 * do the work to remove from the system CDB queues the CDBs for the
 * given slot
 */
static void
remove_cdbs (slottype* sp)
{
    int intfc_indx;

    for (intfc_indx = 0; intfc_indx < sp->interface_count; intfc_indx++) {
	cdbtype *cdb;
	hwidbtype* hwidb;

	hwidb = sp->interface_list[intfc_indx];
	if (hwidb == NULL)
	    continue;
	cdb = controller_find_by_unit(hwidb->mci_index);
	if (cdb)
	    controller_remove(cdb);
    }
    return;
}

/*
 * remove NOHARDWARE IDBs and CDBs from the active system queues
 * reset interface hold queue
 */
static void
remove_pulled_hwidbs (void)
{
    slottype *sp;

    FOR_ALL_SLOTS(sp) {
	if ((sp->flags & BOARD_NONEXIST) &&
	    (sp->flags & BOARD_ANALYZED)) {
	    sp->flags &= ~BOARD_ANALYZED;
	    reg_invoke_chassismib_remove_card_entry(sp);
	    reg_invoke_ctrlr_removed(sp->ctrlr_type, sp);
	    remove_idbs(sp);
	    remove_cdbs(sp);
	}
    }
    return;
}

/*
 * find interfaces that were removed and reinserted, and restore them
 * to their pre-removal state
 */
static void
restart_reinsertions (void)
{
    hwidbtype *hwidb;

    if (oir_debug) {
	buginf("\nLooking for interfaces to restart...");
    }
    FOR_ALL_HWIDBS(hwidb) {
	if ((hwidb->state == IDBS_ADMINDOWN) &&
	    (hwidb->pre_hotstall_state != IDBS_ADMINDOWN)) {
	    if (oir_debug) {
		buginf("\nBringing up %s, pre_hotstall_state=%d",
		       hwidb->hw_namestring, hwidb->pre_hotstall_state);
	    }
	    hwidb->pre_hotstall_state = IDBS_ADMINDOWN;
	    mgd_timer_stop(&hwidb->carriertimer);
	    shutdown_coming_up(hwidb);
	}
    }
    return;
}

/*
 * find any interfaces that have queued output and restart their output
 */
static void
restart_output (void)
{
    hwidbtype *hwidb;

    if (oir_debug)
	buginf("\nLooking for queued output to restart...");

    FOR_ALL_HWIDBS(hwidb) {
	if ( (hwidb->output_qcount > 0) &&
	     (hwidb->state != IDBS_ADMINDOWN) ) {
	    if (oir_debug)
		buginf("\n Restarting output on %s",
		       hwidb->hw_namestring);
	    (*hwidb->soutput)(hwidb);
	}
    }
    return;
}

/*
 * this process does most of the work associated with OIR
 */
static forktype
rsp_oir_process (void)
{
    process_watch_boolean(oir_process_wakeup, ENABLE, RECURRING);
    for (;;) {
	slottype *sp;
	sys_timestamp bufretn_time;

	process_wait_for_event();
	process_set_boolean(oir_process_wakeup, FALSE);

	if (oir_debug)
	    buginf("\nOIR starting");

	dgbus_analyze(TRUE /* EOIR */);

	if (oir_debug)
	    buginf("\nQuiescing");

	/*
	 * From the time we send the quisece, through to when we
	 * have reinitialised the IPs with the results of the new carve,
	 * we can't afford to relinquish the cpu.  If we do, there's lots
	 * of other processes out there that will attempt to send MEMD
	 * related commands down to the IPs (for example: buffer return,
	 * stats, fddi smt stuff). Since the IPs are quiesced, they won't 
	 * respond, and we'll cop a CCB timeout.  While this code doesn't look
	 * as though it suspends, it calls deep into the soft dbus quagmire
	 * which may suspend (but hopefully not if blocking_disabled is TRUE)
	 */
	blocking_disabled = TRUE;

	send_quiesce_cmds();

	if (oir_debug)
	    buginf("\nQuiesced");

	TIMER_START(bufretn_time, EOIR_BUFFER_RETURN_TIME);

	shutdown_pulled_interfaces();

	remove_pulled_hwidbs();

	FOR_ALL_SLOTS(sp) {
	    if ((sp->flags & (BOARD_NONEXIST | BOARD_ANALYZED)) == 0) {
		if (!(FCI_TYPE_RSP(sp->ctrlr_type))) {
		    errmsg(&msgsym(INSCARD, OIR), sp->slot_num);
		}
		sp->flags |= BOARD_CCBNEEDED;
	    }
	}

	/*
	 * DFS/IPC message is sent only when the DFS/IPC buffer is full or
	 * 1 second timer expires.
	 * The cache invalidation DFS/IPC message if exists need to be flushed
	 * down to VIPs before MEMD recarve and UNQUIESCE to maintain consistent
	 * caches between RSP and VIPs
	 */
	ip_ipc_flush_msg();

	if (oir_debug)
	    buginf("\nSleeping %u", TIME_LEFT_SLEEPING(bufretn_time));

	DELAY(TIME_LEFT_SLEEPING(bufretn_time));

	if (oir_debug)
	    buginf("\nCbus analyze");

	cbus_analyze(FALSE /* not firsttime */, TRUE /* EOIR */);

	if (oir_debug)
	    buginf("\nRecarving");

	rsp_memd_carve(TRUE /* EOIR */);

	populate_txq_txacc_to_ips();

	/*
	 * By the time we return from rsp_memd_carve, the IPs are back
	 * on their feet, so it's safe to let other code run again that
	 * might send commands to them.
	 */
	blocking_disabled = FALSE;

	if (oir_debug)
	    buginf("\nReinsertions");
	restart_reinsertions();

	if (oir_debug)
	    buginf("\nRestart queued output");
	restart_output();

	if (oir_debug)
	    buginf("\nOIR done");

	hotstall_flag = FALSE;
    }
}

/*
 * find interfaces on removed cards and mark them as not having hardware
 */
static void
mark_nohardware (void)
{
    slottype* sp;

    FOR_ALL_NONEMPTY_SLOTS(sp) {

	/*
	 * Only disable old MIP hardware
	 */
	if ( (sp->ctrlr_type == FCI_MIP_CONTROLLER) &&
	    (sp->hw_version == MIP_NO_EOIR_VERSION) ) {
	    disable_slot(sp->slot_num);
	}

	if ((sp->flags & BOARD_ANALYZED) &&
	    !slot_present(sp->slot_num)) {
	    int ix;
	    ccbtype *ccb;

	    /* mark as removed */
	    sp->flags |= BOARD_NONEXIST;
	    errmsg(&msgsym(REMCARD, OIR), sp->slot_num);
	    ccb = sp->ccb_ptr;
	    if (ccb)
		ccb->ccb_done = CMD_RSP_ERR_HSA;

	    ix = sp->interface_count;
	    while (ix-- > 0) {
		hwidbtype* hwidb;

		hwidb = sp->interface_list[ix];
		if (hwidb)
		    mark_hwidbs_nohardware(hwidb);
	    }
	}
    }
    return;
}

#define STALL_POWERUP	(ONESEC * 3)	/* time to allow CIP to power-up */
#define STALL_TIMEOUT	(ONESEC * 5)	/* give-up if stall lasts this long */
#define STALL_DEBOUNCE	50		/* milliseconds */

static void
rsp_oir_interrupt (void)
{
    volatile uchar *cbus0reg, *cbus1reg;
    sys_timestamp stall_debounce, stall_timeout;
    uchar cbus0val, cbus1val;
    volatile ushort *reg;

    hotstall_flag = TRUE;

    cbus0reg = (uchar*) ADRSPC_CBUS0REG;
    cbus1reg = (uchar*) ADRSPC_CBUS1REG;
    TIMER_START(stall_timeout, STALL_TIMEOUT);
    TIMER_START(stall_debounce, STALL_DEBOUNCE);

    /* Ensure both cybus's are inaccessible by the IP's. */
    if (!(*cbus0reg & CBUSX_LATCHED_STALL))
	*cbus0reg &= ~CBUSX_GRANT_ENABLE;
    if (!(*cbus1reg & CBUSX_LATCHED_STALL))
	*cbus1reg &= ~CBUSX_GRANT_ENABLE;

    if (oir_debug)
	buginf("\nStall detected");
    for (;;) {
	if (AWAKE(stall_timeout)) {
	    /*
	     * complain about incomplete stall.
	     * need to follow-through on OIR processing because
	     * downrev MIPs are wedged by now.
	     */
	    errmsg(&msgsym(LONGSTALL, OIR), STALL_TIMEOUT);
	    break;
	}
	/* 
	 * Check both Latch and Unlatch bits to determine if stall
	 * is debounced. Checking both bits is for robustness.
	 */
        cbus0val = *cbus0reg;
        cbus1val = *cbus1reg;
        if ((cbus0val & (CBUSX_UNLATCHED_STALL | CBUSX_LATCHED_STALL)) ||
            (cbus1val & (CBUSX_UNLATCHED_STALL | CBUSX_LATCHED_STALL))) {
            *cbus0reg = (cbus0val & ~CBUSX_LATCHED_STALL) | CBUSX_STALL_UPDATE;
            *cbus1reg = (cbus1val & ~CBUSX_LATCHED_STALL) | CBUSX_STALL_UPDATE;
	    TIMER_START(stall_debounce, STALL_DEBOUNCE);
	    continue;
	}
	if (AWAKE(stall_debounce)) {
	    break;
	}
    }
    if (oir_debug)
	buginf("\nStall debounced");

    /*
     * ucode loader process will activate OIR process
     */
    ucode_loader_wakeup(STALL_POWERUP);

    /*
     * Reset latched stall bits, clearing the interrupt.
     * This must be done before any dbus accesses on RSP2.
     */
    *cbus0reg = (*cbus0reg & ~CBUSX_LATCHED_STALL) | CBUSX_STALL_UPDATE;
    *cbus1reg = (*cbus1reg & ~CBUSX_LATCHED_STALL) | CBUSX_STALL_UPDATE;

    /*
     * Stall has been debounced. We need to make sure we are the Master,
     * else we may get into crash while accessing Dbus. 
     * Somehow, sometimes we lose Mastership here during OIR!
     * Kludge:  Force yourself to be Master by writing into Master
     * Slave command register.
     */
    if (rsp_is_master() && rsp_dual_capable()) {
        reg = (ushort*)ADRSPC_MASTER_STATUS;
        if ((*reg & MASTER_STATUS_THIS_MASK) != MASTER_STATUS_THIS_MASTER) {
          reg = (ushort *) ADRSPC_MASTER_CMD; 
          *reg = MASTER_CMD_MASTER;
	      /* 
	       * Flush the write, Read Master Slave Status register to validate 
	       * succesful completion of the command
	       */
          reg = (ushort*)ADRSPC_MASTER_STATUS;
          *reg;
          if (oir_debug)
              buginf("\nReclaimed Mastership during OIR");
        }
    }

    mark_nohardware();

    /* Allow the IP's to access the cybus's again. */
    *cbus0reg |= CBUSX_GRANT_ENABLE;
    *cbus1reg |= CBUSX_GRANT_ENABLE;

    return;
}

/*****************************************************************************
 * Name: rsp_find_matching_idb
 *
 * Description: search idbQ for an idb that matches slot, port, and type
 *
 * Input: 
 *   slot_num -- the slot number for the interface
 *   port_num -- the port number of the interface (0 based)
 *   fci_type -- the FCI_TYPE_ of the interface we are looking for
 *   idb_type -- one of enum IDBTYPE
 *
 * Return Value: 
 *   idb -- pointer to the idb that matches all of the above criteria,
 *     or NULL if no match is found.
 *
 *****************************************************************************/
static inline
hwidbtype* rsp_find_matching_idb (int slot_num, int port_num,
				  int fci_type, enum IDBTYPE idb_type)
{
    hwidbtype *hwidb;

    for (hwidb = (hwidbtype*) hwidb_remQ.qhead;
	 hwidb != NULL;
	 hwidb = (hwidbtype*) IDB_NEXT(hwidb)) {
	if ((hwidb->fci_type == fci_type) &&
	    (hwidb->type == idb_type) &&
	    (hwidb->slot == slot_num) &&
	    (hwidb->slotunit == port_num)) {
	    unqueue(&hwidb_remQ, hwidb);
	    return hwidb;
	}
    }
    return(NULL);
}

/*****************************************************************************
 * Name: rsp_get_matching_idb
 *
 * Description: search idbQ for an idb that matches slot, port, and
 *   type -- if no match is found, an IDB is created and enqueued on
 *   the IDB queue.
 *
 * Input: 
 *   slot_num -- the slot number for the interface
 *   port_num -- the port number of the interface (0 based)
 *   fci_type -- the FCI_TYPE_ of the interface we are looking for
 *   idb_type -- one of enum IDBTYPE
 *
 * Return Value: 
 *   idb -- pointer to the idb that matches all of the above criteria,
 *     or NULL if no match is found.
 *
 *****************************************************************************/

hwidbtype*
rsp_get_matching_idb (slottype *slot, int port_num,
		      int fci_type, enum IDBTYPE idb_type)
{
    ushort slot_num;
    hwidbtype *hwidb;

    slot_num = slot->slot_num;
    hwidb = rsp_find_matching_idb(slot_num, port_num, fci_type, idb_type);
    if (hwidb == NULL) {
	hwidb = idb_create();
	if (hwidb) {
	    hwidb->state = IDBS_ADMINDOWN;
	    hwidb->slot = slot_num;
	    hwidb->slotunit = port_num;
	    hwidb->unit = port_num;
	    hwidb->slot_ptr = slot;
	    hwidb->fci_type = fci_type;
	    hwidb->type = idb_type;
	    if (oir_debug)
		buginf("\nSetting pre_hotstall_state=IDBS_ADMINDOWN for unit %d, type %d, %s, slot %d",
		       port_num, idb_type, hwidb->hw_namestring, slot_num);
	    hwidb->pre_hotstall_state = IDBS_ADMINDOWN;
            hwidb->next_idb_on_physint = NULL;
	}
    }
    if (hwidb) {
	hwidbtype *tmp_hwidb;

	/*
	 * treat "base" IDBs differently (e.g. MIP)
	 * base IDB isn't on system queue, but its sub-IDBs are
	 */
	if (hwidb->fci_type & FCI_TYPE_BASE)
	    tmp_hwidb = hwidb->next_idb_on_physint;
	else
	    tmp_hwidb = hwidb;
	while (tmp_hwidb) {
	    media_idb_enqueue(tmp_hwidb);
	    if (system_configured)
		++nnets;
	    tmp_hwidb = tmp_hwidb->next_idb_on_physint;
	}
    }
    return(hwidb);
}

static void
oir_init (subsystype *subsys)
{
    hotstall_flag = FALSE;
    queue_init(&hwidb_remQ, 0 /* no maximum */);
    oir_process_wakeup = create_watched_boolean("OIR wakeup", 0);
    if (NO_PROCESS == process_create(rsp_oir_process, "OIR Handler",
			   SMALL_STACK, PRIO_NORMAL))
	return;
    createlevel(LEVEL_STALL, rsp_oir_interrupt, "OIR interrupt");
    oir_parser_init();
    oir_debug_init();
    return;
}

#define OIR_MAJVERSION 1
#define OIR_MINVERSION 0
#define OIR_EDITVERSION 1

SUBSYS_HEADER(oir, OIR_MAJVERSION, OIR_MINVERSION, OIR_EDITVERSION,
	      oir_init, SUBSYS_CLASS_KERNEL,
	      NULL,
	      NULL);

/*****************************************************************************/

/*
 * see if current SP microcode can do EOIR processing 
 */
void
set_eoir_capable (ushort slot)
{
    return;
}

/*
 * stub called before hotstall_interrupt to allocate necessary memory
 */
void hotswap_preallocate_memory (boolean alloc_additional_memory)
{
    return;
}
