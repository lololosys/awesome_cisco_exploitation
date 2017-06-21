/* $Id: rsp_mip.c,v 3.9.12.11 1996/08/28 13:14:14 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_mip.c,v $
 *------------------------------------------------------------------
 * rsp_mip.c -- Support for the MIP card specific to RSP
 *
 * Feb 1995, David Bath
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: rsp_mip.c,v $
 * Revision 3.9.12.11  1996/08/28  13:14:14  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.9.12.10  1996/08/27  07:13:34  mwu
 * CSCdi67143:  MIP interfaces dont come up on 7000 platform
 * Branch: California_branch
 *
 * Revision 3.9.12.9  1996/08/19  23:16:02  mwu
 * CSCdi66057:  mip interface outhang during booting - controller and
 * interface initialized after the cofniguration is done; apply guard
 * time only when setting new txql.
 * Branch: California_branch
 *
 * Revision 3.9.12.8  1996/08/13  02:24:25  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.9.12.7  1996/08/11  20:39:16  mwu
 * CSCdi65038:  admin-down interfaces upped on EOIR - don't change
 * interface state for eoir mip; also optimize code for interface
 * creation/reset.
 * Branch: California_branch
 *
 * Revision 3.9.12.6  1996/08/09  17:22:10  mwu
 * CSCdi59746:  MIP EOIR recovery time is exceedingly long.
 * Branch: California_branch
 *
 * Revision 3.9.12.5  1996/08/02  00:33:17  mwu
 * CSCdi64153:  mips txlimit set to zero in a loaded 75xx
 * Branch: California_branch
 *
 * Revision 3.9.12.4  1996/07/23  13:28:45  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.9.12.3  1996/07/08  22:52:48  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.9.12.2  1996/05/17  11:54:35  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.9.2.2  1996/04/26  07:57:46  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.9.2.1  1996/04/03  21:43:16  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.9.12.1  1996/03/18  22:02:27  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.14.3  1996/03/07  10:45:07  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.14.2  1996/02/20  18:01:27  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/02/13  08:16:04  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.8  1996/02/08  23:11:21  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.7  1996/02/08  22:22:39  wmay
 * CSCdi48458:  mip uses old scheduler - use watched queues for love
 * note handler
 *
 * Revision 3.6  1996/02/01  06:09:54  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5.14.1  1996/01/24  23:04:10  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.5  1995/12/15  08:02:02  rramacha
 * CSCdi42101:  sh ver does not recognize CX-SMIP or CX-SSIP cards
 *
 * Revision 3.4  1995/11/28  00:24:29  fred
 * CSCdi43173:  Fast-switching fails over mip
 *         If Fair Queuing already configured on MIP, deconfigure it.
 *         If Fair Queuing selected but not yet configured, select FCFS
 *         instead.
 *         MIP does not support queuing that requires backpressure
 *
 * Revision 3.3  1995/11/17  18:49:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:33:09  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:28:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/16  22:31:06  getchell
 * CSCdi35501:  src-rsp/ccb_cmd.h should be moved to hes
 * src-rsp/[ccb.h,ccb_cmd.h] combined and moved to hes/ccb_cmd.h.
 * rsp_command prototypes removed from ccb_cmd.h and moved to
 * src-rsp/rsp_commands.h.  Files including altered files modifed to
 * reflect new file structure.
 *
 * Revision 2.3  1995/06/23  18:48:43  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.2  1995/06/08  09:08:37  dbath
 * CSCdi35309:  state change from admin-down to up - MIP/OIR
 * Be sure to initialise pre_hotstall_state on dynamic idb's, lest
 * restart_reinsertions() will bring to life interfaces that were
 * admindown before an EOIR event.
 *
 * Revision 2.1  1995/06/07  22:58:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "plural.h"
#include "logger.h"
#include "interface_private.h"
#include "rsp_if.h"
#include "rsp_pcmap.h"
#include "rsp_commands.h"
#include "sys_registry.h"
#include "packet.h"
#include "../if/network.h"
#include "../hes/cbus_registry.h"
#include "../hes/if_hes_common.h"
#include "../hes/if_fci.h"
#include "../hes/if_mci.h"
#include "rsp_common_if.h"
#include "../hes/if_cmd.h"
#include "../hes/ccb_cmd.h"
#include "../hes/ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"
#include "lovenote.h"
#include "../if/if_controller.h"
#include "../hes/hes_controller.h"
#include "../if/dsx1.h"
#include "../hes/if_mip.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "../wan/smds.h"
#include "../if/priority_private.h"
#include "../hes/msg_cbus.c"		/* Not a typo, see logger.h */
#include "../src-rsp/msg_qa.c"		/* Not a typo, see logger.h */
#include "../src-rsp/msg_rsp.c"		/* Not a typo, see logger.h */
#include "../iprouting/igrp.h"
#include "rsp_bufhdr.h"
#include "rsp_qasic.h"


static uchar nMIPs;
static uchar nSMIPs;

/*
 * mip_platform_configured_check
 * Check to see if the system has been configured during
 * the system initialization.
 */
boolean mip_platform_configured_check (void)
{
    return (system_configured);
}
/*
 * platform_chain_hwidb_in
 * Platform specific routine to chain a hwidb into the 'system'
 * On RSP, this means attaching it to the appropriate entry in
 * the interface_list[] array in the slot structure.
 */
void platform_chain_hwidb_in (cdbtype *cdb, hwidbtype *hwidb)
{
    slottype *slotp = cdb->platform_hware_handle;
    int plughole_no = cdb->appl_no;

    /*
     *  After hot swap of another board, or a microcode reload, this
     *  structure is nuked.  We need to guarantee our entries are
     *  there.  The idb chain is still valid from before, we just need
     *  to set the pointer to the head of the chain.
     *
     *  If the entry is still there, then this must simply be a
     *  remap of an existing channel.  This is perfectly legal (and
     *  fairly common).  This routine is smart enough to deal with
     *  this.
     */
    if (slotp->interface_list[plughole_no] == NULL) {
	slotp->interface_list[plughole_no] = hwidb;
    }
    else {
	hwidbtype *tmp;
	/* 
	 * Traverse the list looking for the one < the
	 * one we're attempting to add, or the one 
	 * we're attempting to add . Keep it in order of vc.
	 */
	tmp = slotp->interface_list[plughole_no];
	if (hwidb->vc < tmp->vc) {
	    slotp->interface_list[plughole_no] = hwidb;
	    hwidb->next_idb_on_physint = tmp;
	    return;
	}
	while (tmp != hwidb) {
	    if (tmp->next_idb_on_physint == NULL) {
		tmp->next_idb_on_physint = hwidb;
		hwidb->next_idb_on_physint = NULL;
		break;
	    }
	    else if (hwidb->vc < tmp->next_idb_on_physint->vc) {
		hwidb->next_idb_on_physint = tmp->next_idb_on_physint;
		tmp->next_idb_on_physint = hwidb;
		break;
	    }
	    tmp = tmp->next_idb_on_physint;
	}
    }
}

/*
 * platform_unchain_hwidb
 * Platform specific routine to unchain a hwidb from the 'system'
 * On RSP, this means removing it from the appropriate entry in
 * the interface_list[] array in the slot structure.
 */
void platform_unchain_hwidb (cdbtype *cdb, hwidbtype *hwidb)
{
    hwidbtype *cur, *prev;
    slottype *slotp = cdb->platform_hware_handle;
    int plughole_no = cdb->appl_no;

    for (cur = slotp->interface_list[plughole_no], prev = NULL ;
	 cur != hwidb && cur != NULL ; 
	 prev = cur, cur = cur->next_idb_on_physint)
	;
    if (cur == NULL) {
	/* how very odd */
	buginf("\n%%unlink_idb(): Cannot find IDB %x", hwidb);
    } else {
	if (prev == NULL) {
	    /* first on list */
	    slotp->interface_list[plughole_no] = hwidb->next_idb_on_physint;
	} else
	    prev->next_idb_on_physint = hwidb->next_idb_on_physint;
    }
}
/*
 * mip_platform_get_txqlength
 * Get txq length
 */
int mip_platform_get_txqlength (hwidbtype *hwidb)
{
    if (hwidb->tx_acc_ptr)
        return ((int) hwacc_peek(hwidb->tx_acc_ptr));
    else {
	/* should not happen! */
        buginf("\nMIP %s: tx_acc got lost", hwidb->hw_namestring);
        return 0;
    }
}

/*
 * mip_acc_guard
 * Prevent accumulator out of syn.
 */
static inline void mip_acc_guard (hwidbtype *hwidb)
{
    int delay;
    ushort acc_value, txlimit;

    /*
     * Don't need to apply guard time during system
     * initialization or hotstall processing because
     * there should not be any outstanding packets
     * on the MIP.
     */
    if ((hotstall_flag) || (systeminit_flag == FALSE))
        return;
    
    acc_value = hwacc_peek(hwidb->tx_acc_ptr);
    txlimit = hwidb->old_txcount;

    if (!txlimit)
        return;

    if (acc_value > txlimit) {
	/*
	 * In case acc is out of synch. or hw error?
	 */
	errmsg(&msgsym(ACCERROR, RSP), hwidb->hw_namestring, 
  	       hwidb->tx_acc_ptr, acc_value);
	return;
    }

    if ((hwidb->state == IDBS_UP) && (acc_value < txlimit)) {
	/*
	 * Some packets have not been done yet; we will
	 * allow the mip to finish them.
	 */
	/* determine guard time in ms */
	delay = ((txlimit - acc_value) * hwidb->maxmtu) /
	  (hwidb->firstsw->visible_bandwidth / 8);
	DELAY(delay);
    }
}

/*
 * mip_get_acc
 * Allocate accumulator for a channel
 */
static inline boolean mip_get_acc (hwidbtype *hwidb)
{
    if ((hwidb->tx_acc_ptr = get_hwacc()) == NULL) {
	errmsg(&msgsym(ALLOC, QA), "Ran out accumulator");
	return FALSE;
    }

    /* clean up left over value */
    hwacc_set(hwidb->tx_acc_ptr, 0);

    return TRUE;
}

/*
 * mip_set_default_txlimit
 * Assign default txlimit for a channel based on channel's
 * bandwidth.
 */
static void mip_set_default_txlimit (hwidbtype *hwidb)
{
    dsx1info *dsx1;
    hwidbtype *base_hwidb;
    mipinfo *mip;
    int channel = hwidb->vc;
    int timeslots;
    int mini_default_txcount;
    
    mip = mip_find_by_idb(hwidb);
    timeslots = mip->vc_ts_no[channel];
    dsx1 = dsx1_find_by_unit(mip->unit);
    /*
     * Divide plughole's default for a channel.
     * Note that the plughole's default txcount can be less than
     * MAXTS in a 75xx box. If it is, the division can result a
     * zero value. A zero default can turn the interface to
     * restart. 
     */
    base_hwidb = slots[mip->slot].interface_list[mip->appl_no];

    mini_default_txcount = RSP_MIP_MIN_TQL + 
	                              ((timeslots - 1) * 1);

    hwidb->mci_txcount_default =  (timeslots * 
       base_hwidb->mci_txcount_default) /
        (mip->cdb->idbtype == IDBTYPE_T1 ? T1_MAXTS : E1_MAXTS);

    if (hwidb->mci_txcount_default < mini_default_txcount) {
	/*
	 * Make sure the channel interface will have minimum
	 * txlimit.
	 */
	hwidb->mci_txcount_default = mini_default_txcount;
    }

    set_default_interface_bandwidth(hwidb->firstsw,
				    (num_bits(dsx1->speed[channel]) * 8) * 
				    timeslots);
}
/*
 * mip_platform_if_reset
 * Set vc tx limit and instruct the MIP to use allocated accumulator address.
 * Assume network int has been disabled and if/vc have been selected.
 */
void mip_platform_if_reset (mipinfo *mip, hwidbtype *hwidb)
{
    if (hwidb->tx_acc_ptr == NULL) {
	return;
    }

    if (hwidb->tx_acc_ptr) {

	if (mip->vc_ts_no[hwidb->vc]) {
	    mip_acc_guard(hwidb);
	    mip_set_default_txlimit(hwidb);
	    cbus_mci_txlimit(hwidb);

	    mip_command_arg(mip, FCI_CMD_VC_TXACCUM, 
			    BUFHDR_ADDR_TO_OFF(hwidb->tx_acc_ptr));    
	    if ((serial_debug) || (oir_debug)) {
		buginf("\nsending MIP VC %d acc_ptr %x", 
		       hwidb->vc, hwidb->tx_acc_ptr);
	    }
	}
    }
}

/*
 * mip_platform_idb_create
 * 
 */
boolean mip_platform_idb_create (hwidbtype *hwidb)
{
    /* Allocate an accumulator for this channel; the accumulator
     * value will be set later.
     * When parsing the nvram at the system initialization,
     * we don't need to allocate accumulator because memd 
     * will be carved again after the configuration is done.
     * The interface's accumulator will then be assigned
     * later when interface is initialized after the memd
     * is carved.
     */

    if ((system_configured) && (hwidb->tx_acc_ptr == NULL)) {
	return (mip_get_acc(hwidb));
    }

    return TRUE;
}
/*
 * mip_platform_idb_delete
 * return accumulator
 */
void mip_platform_idb_delete (hwidbtype *hwidb)
{
    if (hwidb->tx_acc_ptr) {
	ret_hwacc(hwidb->tx_acc_ptr);
	hwidb->tx_acc_ptr = NULL;
    }
}


/*
 * mip_platform_map_channel
 * store channel timeslots
 */
void mip_platform_map_channel (mipinfo *mip, hwidbtype *hwidb,
			       ulong bitfield)
{
    /* 
     * During system init, we only need to remember channel
     * timeslot. txlimit default will be set when channel
     * interface is initialized after the controller is up.
     */
    mip->vc_ts_no[hwidb->vc] = (uchar) num_bits(bitfield);    

    if (bitfield) {
	/* chain it after vc is set */
	platform_chain_hwidb_in(mip->cdb, hwidb);
    }

    if ((systeminit_flag == FALSE) || (!bitfield)) {
	return;
    }

    /* 
     * For a pri B channel, the channel speed has been
     * set in dsx1_pri_speed per call and will be used to set
     * tql. For a channelized t1 channel, the channel
     * speed is set in dsx1_map_channel per configuration.
     */
    mip_set_default_txlimit(hwidb);

    /*
     * Prime the tx acc to prevent unwanted delay
     * when the interface is reset later.
     */
    cbus_mci_txlimit(hwidb);
}

/*
 * free_mip
 * Free a mipinfo block
 */
void free_mip (mipinfo *mip)
{
    /*
     * On the RSP, we truly free this guy.  The preQ's are not used,
     * and should remain empty.
     */
    free(mip);
}

/*
 * platform_mip_carve_in_progress
 *
 * Determine whether or not it's safe to send buffer pool info
 * down to the card.  If we're in the middle of recarving MEMD,
 * then it's not safe.  On the RSP, this information is held
 * in the memd_carve_in_progress global.  Hence we ignore the
 * supplied cdb.
 */
boolean platform_mip_carve_in_progress (cdbtype *cdb)
{
    return(memd_carve_in_progress);
}

/*
 * platform_mip_reset_in_progress
 *
 * Determine whether or not we're in the middle of resetting
 * the cbus.  On the RSP, this is indicated by the state
 * of the cbus_being_reset global flag.  We can therefore ignore the cdb
 * passed in (it's only needed in the c7000 implementation
 * of this routine).
 */
boolean platform_mip_reset_in_progress (cdbtype *cdb)
{
    return(cbus_being_reset);
}

/*
 * platform_mip_update_carve_dbase
 *
 * Gets called by mip_set_maxdgram when the user makes a
 * significant change to a vc's MTU,  where significant is
 * defined as making it larger or smaller than the current
 * largest or smallest for all vc's on that plughole.
 * This routine gets called after the fields in the cdb have
 * been adjusted to reflect the new settings, but before we
 * recarve.  Copy the relevant fields out of the cdb into
 * the base hwidb, so that the carving code will see them.
 */
void platform_mip_update_carve_dbase (cdbtype *cdb)
{
    hwidbtype  *base_hwidb;

    base_hwidb = slots[cdb->slot].interface_list[cdb->appl_no];
    if_maxdgram_adjust(base_hwidb, cdb->max_buffer_size);
    base_hwidb->buffersize = cdb->buffersize;
}

/*
 * platform_mip_request_bus_reset
 *
 * Request that the system reset the cbus at its earliest
 * convenience.  We can ignore the cdb passed in; it's
 * only needed by the c7000 implementation of this routine.
 */
void platform_mip_request_bus_reset (cdbtype *cdb)
{
    platform_req_complex_restart(NULL);  /* No hwidb, just do it */
}

/*
 * sync the IDBs buffer variables to its CDBs point of view
 * Should be called after cbus_init.  Doesn't necessarilly sync all the
 * fields.  
 */
void load_idb_buffer_info (hwidbtype *hwidb, cdbtype *cdb)
{
    hwidbtype *base_hwidb;
    slottype *slotptr;

    if (oir_debug) {
	buginf("\nDoing platform_mip_dynamic_idb_init on %s, cdb %s, hwidb %#x",
	       hwidb->hw_namestring, cdb->cdb_namestring, hwidb);
    }

    slotptr = cdb->platform_hware_handle;
    base_hwidb = slotptr->interface_list[cdb->appl_no];
    hwidb->tx_q_ptr = base_hwidb->tx_q_ptr;
}

/*
 * platform_mip_dynamic_idb_init
 *
 * Do the 'normal'  hwidb init stuff that usually happens at
 * cbus analyze time, for a freshly created MIP idb.
 * Returns TRUE if there's an error, otherwise FALSE.
 * On the RSP, we always return FALSE.
 */
boolean platform_mip_dynamic_idb_init (cdbtype *cdb, mipinfo* mip,
				       hwidbtype * hwidb)
{
    hwidbtype *base_hwidb;
    slottype *slotptr;

    slotptr = cdb->platform_hware_handle;
    base_hwidb = slotptr->interface_list[cdb->appl_no];

    hwidb->slot = base_hwidb->slot;
    hwidb->slotunit = base_hwidb->slotunit;
    hwidb->slot_ptr = base_hwidb->slot_ptr;
    hwidb->type = base_hwidb->type;
    hwidb->status = base_hwidb->status;
    if (oir_debug) {
	buginf("\nSetting pre_hotstall_state %d, was %d, for %s",
	       base_hwidb->pre_hotstall_state, hwidb->pre_hotstall_state,
	       hwidb->hw_namestring);
    }
    hwidb->pre_hotstall_state = base_hwidb->pre_hotstall_state;

    hes_idb_init_before(hwidb);

    hwidb->buffersize = base_hwidb->buffersize;
    hwidb->rxoffset = base_hwidb->rxoffset;
    hwidb->ip_memd_offset = base_hwidb->ip_memd_offset;

    serial_idb_init(hwidb, MAXSERIALSIZE, MCI_MAXDGRAM_DEF,
		    VISIBLE_MCI_SERIAL_BANDFAST, MCI_SERIAL_DELAY);

    (cdb->idb_init)(cdb, hwidb);

    hes_idb_init_after(hwidb);

    ++nserialnets;
    ++nnets;

    return(FALSE);
}
ushort mip_select_if (mipinfo *mip)
{
    slottype *slotptr = mip->cdb->platform_hware_handle;

    send_ip_cmd_1arg(slotptr, CCB_CMD_SELECT, mip->appl_no);
    return (get_ip_status(slotptr));
}

ushort mip_controller_status (mipinfo *mip, ushort param)
{
    slottype *slotptr = mip->cdb->platform_hware_handle;
    
    send_ip_cmd_1arg(slotptr, CCB_CMD_STATUS, param);
    return(get_ip_status(slotptr));
}

/*
 * mip_simple_command
 * Send a command with no parameter down to the card
 */
ushort mip_simple_command (mipinfo *mip, int command)
{
    slottype *slotptr = mip->cdb->platform_hware_handle;
    
    send_ip_cmd(slotptr, command);
    return(get_ip_status(slotptr));
}

/*
 * mip_simple_command_result
 * Send a command with no parameter down to the card, and get
 * back a result
 */
ushort mip_simple_command_result (mipinfo *mip, int command, 
					 ushort *result)
{
    ushort status;
    slottype *slotptr = mip->cdb->platform_hware_handle;
    
    send_ip_cmd(slotptr, command);
    status = get_ip_status(slotptr);
    if (status == CMD_RSP_OKAY) {
	*result = get_ip_val(slotptr);
    }
    return(status);
}

/*
 * mip_command_arg
 * Send a basic command with a parameter down to the card
 */
ushort mip_command_arg (mipinfo *mip, int command, ushort param)
{
    slottype *slotptr = mip->cdb->platform_hware_handle;
    
    send_ip_cmd_1arg(slotptr, command, param);
    return(get_ip_status(slotptr));
}

/*
 * mip_command_larg
 * Send a basic command with a large parameter down to the card
 */
ushort mip_command_larg (mipinfo *mip, int command, ulong param)
{
    slottype *slotptr = mip->cdb->platform_hware_handle;
    
    send_ip_cmd_larg(slotptr, command, param);
    return(get_ip_status(slotptr));
}

/*
 * platform_mip_adjust_mtu_for_love
 * Tweak various fields relating to MTUs to take into account
 * whether or not we ship loveletters in these buffers.   On
 * platforms where loveletters have their own buffer pools (e.g. RSP)
 * this routine does nothing.
 */
void platform_mip_adjust_mtu_for_love (mipinfo *mip,
				       ushort *maxmtu,
				       ulong *buffersize,
				       ulong *maxdgram)
{
    /*
     * On the RSP, loveletters do not use data buffers, so this
     * routine makes no adjustments.
     */
    return;
}

/*
 * platform_mip_set_offsets
 * Set up the rxoffsets, span_ stuff, SMDS stuff
 */
void platform_mip_set_offsets (hwidbtype *hwidb)
{
    /*
     * Set up all pak pointers and offsets to look like a normal
     * serial, since we'll be stripping the vc before these are
     * used.
     */
    hwidb->rxoffset = RSP_SERIAL_PROTOCOL_OFFSET - HDLC_ENCAPBYTES;
    hwidb->ip_memd_offset = RSP_SERIAL_PROTOCOL_OFFSET + 
	ADRSPC_MEMD_WRITETHRU;
    switch (hwidb->enctype) {
    case ET_PPP:
	hwidb->span_macoffset = hwidb->rxoffset + hwidb->encsize + 2;
	hwidb->span_encapsize = hwidb->encsize + 2;
	break;
    case ET_SMDS:
	hwidb->span_macoffset = hwidb->rxoffset + hwidb->encsize + 2;
	hwidb->span_encapsize = hwidb->encsize + 2;

	/*
	 * On the RSP, we strip the MIP board encaps off the packet
	 * immediately upon arrival, so that we can use the low end
	 * switching code. As such, we do not want to adjust 
	 * fast_input_modelen.  On output, and while manipulating
	 * cache entries, we do have the two bytes of board encaps,
	 * and hence, our fast_output_modelen needs to be adjusted
	 * by MIP_ENCAPSIZE_BYTES compared with our fast_input_modelen.
	 */
	hwidb->smds_pdb->fast_input_modelen = 
	    (hwidb->smds_pdb->dximode ? SMDS_DXIBYTES : SMDS_CISCOBYTES);
	hwidb->smds_pdb->fast_output_modelen = 
	    hwidb->smds_pdb->fast_input_modelen + MIP_ENCAPSIZE_BYTES;

	break;
    default:
	hwidb->span_macoffset = hwidb->rxoffset + hwidb->encsize;
	hwidb->span_encapsize = hwidb->encsize;
	break;
    }
    /*
     * Now give the MIP it's two extra vc bytes.
     */
    hwidb->rxoffset -= MIP_ENCAPSIZE_BYTES;
}
/*
 * platform_mip_restart_init 
 *
 */
void platform_mip_restart_init (mipinfo *mip)
{
}

/*
 * platform_mip_adjust_encsize
 * Adjust the encsize field in the hwidb, in a platform
 * specific way.
 *
 * On the RSP we leave it alone.  When a packet arrives,
 * dgstart and dgsize are immediately adjusted to skip
 * the vc, hence we don't want it to appear in our encsize
 * field either.  So, simply do nothing.
 */
void platform_mip_adjust_encsize (hwidbtype *hwidb)
{
}

/*****************************************************************************
 * Name: mip_love_missive
 *
 * Description: love missive handler for MIP
 *   Note, this routine handles both love notes and love letters.
 *  
 * Input: 
 *   hwidb -- pointer to the base h'ware IDB for the MIP plughole that
 *     generated the event (love note)
 *   lovenote -- pointer to the love note 
 *
 * Return Value: None
 *
 *****************************************************************************/
static void mip_love_missive (hwidbtype *hwidb, lovenotetype *lovenote)
{

    /*
     * Use the length field to determine if it's a love note
     * or a love letter.
     */
    if (lovenote->data_length == 0) {
	cdbtype *cdb;

	/*
	 * Since it's just a love note, extract the 'status' from
	 * the love_code, and simply pass it to the love_note
	 * vector in the relevant cdb.
	 */
	cdb = controller_find(hwidb->slot, hwidb->slotunit);
	(cdb->love_note)(cdb, lovenote->love_code);
    } else {
	paktype *pak;

	/*
	 *  Because we don't free these until process level, on a system
	 *  doing a lot of fastswitching we can end up chewing up all our
	 *  buffers here.  So we set a limit of
	 *  (MAX_SLOTS * MIP_MAX_DAUGHTERS + 1) love letters we will
	 *  keep at any time.  When we pass that, we will throttle all MIPs
	 *  until we drain the queue.  Note these are system buffers, not
	 *  MEMD buffers.  We free the MEMD buffers here, at the time we
	 *  take the interrupt.
	 */
	if (process_queue_size(mip_loveletterQ) >=
	    (MAX_SLOTS * MIP_MAX_DAUGHTERS + 1))
	    mip_throttle_love_letters(TRUE);

	/*
	 * This takes a bit of explaining.  The loveletter arrives
	 * in MEMD with a 16-bit 'vc' of -1 on the front.  We are
	 * not interested in this at all.  The process level code
	 * that will process the system buffer, expects a 32-bit
	 * 'index' in the front of it's buffer, by which it may
	 * find the relevant mipinfo structure.  It expects
	 * datagramstart to point to the first byte *past* this
	 * 'index' field.
	 *
	 * To further complicate things, we need to maintain the
	 * longword alignment of the data in the system buffer
	 * compared to the data in the MEMD buffer, otherwise
	 * de-references in the receiving process will generate
	 * unaligned traps.  The only field that doesn't have
	 * to be aligned (and indeed, can't be aligned to satisfy
	 * all of the above constraints) is the 32-bit 'index' field.
	 * It is written here, and read by the process, using the
	 * PUTLONG/GETLONG macros thereby avoiding the trap.
	 *
	 * So, while copying from MEMD to the system buffer, we
	 * effectively have to replace the first 2 byte field
	 * with a 6 byte field comprised of 2 wasted bytes followed
	 * by 4 bytes of 'index'.  This means that our system
	 * buffer needs to be 4 bytes longer than the MEMD buffer,
	 * and when we copy, we need to start 2 bytes into the
	 * MEMD buffer, and 6 bytes into the system buffer.
	 * We copy 2 bytes less than the original MEMD length
	 * because we're not copying the first 2 bytes of vc.
	 *
	 * Hmmm... I bet I can draw a picture of this, faster than
	 * havarti can compile the source code to implement it....


______________________________________________________
|	|	|
| vc(2)	|cmd(2)	|remainder(n) [must start on longword]
|_______|_______|_____________________________________
	      \     \_____________
	       \__________       |
			 |       |
_________________________v_______v____________________________________
|	|		|	|
|wst(2)	| index(4)	|cmd(2)	|remainder(n) [must start on longword]
|_______|_______________|_______|_____________________________________
			^
			|
			|
		   datagramstart

	 * Where the top buffer represents the love letter in MEMD,
	 * and the bottom buffer represents it in a system buffer.
	 * where:
	 *    vc is the 2 byte channel group (always -1) that we ignore
	 *   cmd is the 2 byte love letter command code
	 * rmndr is the guts of the love letter (variable length)
	 *   wst is the 2 bytes of waste to ensure alignment of remainder
	 * index is the 4 byte value that we load with hwidb->mci_index
	 *       (note that index is not longword aligned, and so must
	 *        be accessed via PUTLONG and GETLONG)
	 *
	 * Finally, we mustn't change lovenote->buffer_ptr or
	 * lovenote->data_length, because the guy that called
	 * us is going to use them to invalidate the cache lines.
	 */
	if((pak = getbuffer(lovenote->data_length + 4)) == NULL) {
	    return;
	}
	pak->datagramstart = &pak->data_area[6];

	bcopy(MEMD_OFFSET_TO_ADDR(lovenote->buffer_ptr + 2),
	      pak->datagramstart, lovenote->data_length - 2);

	PUTLONG(&pak->datagramstart[-4], hwidb->mci_index);
	if (process_enqueue(mip_loveletterQ, pak) == FALSE)
	    retbuffer(pak);
    }
}

/*****************************************************************************
 * Name: mip_ctrlr_analyze
 *
 * Description: analyze part of initialization for a MIP
 *   This function is called when a board is first discovered and initialized.
 *   It is called the first time the board is plugged in, or when the router
 *   is resstarted.  It is *NOT* called for boards that are installed and
 *   running when a new board is inserted.
 *
 *   This function is responsible for discovering the interfaces
 *   on the MIP (via the CBUS -- select command), and creating
 *   and initializing the CDB for those interfaces.  Unlike other
 *   interfaces, the MIP does not create hwidbs until vc's are
 *   configured.
 *
 * Notes: 
 *   This function is *NEVER* called from interrupt level.
 *
 * Input: 
 *   slot -- pointer to a slottype structure for the IP with the
 *     following fields filled in:
 *       slot_num -- slot number for this card
 *       ccb_ptr -- pointer to the CCB for this card
 *       cmd_q_ptr -- pointer to the CCB for this card
 *
 * Output:
 *   The following fields in the slottype (pointed to by slot) are changed
 *     interface_count -- number of interfaces on this IP
 *     interface_list -- list of pointers to IDB's for the interfaces 
 *
 * Return Value: None
 *
 *****************************************************************************/
static void mip_ctrlr_analyze (slottype *slot)
{
    int port_num;
    ushort status;
    ushort interface_type;
    hwidbtype *hwidb;
    int applique_type;
    int unit;
    cdbtype* cdb;

    /*
     * We loop through the interfaces on the MIP selecting them as we go. 
     * We use the select command to tell if the interface is there.  If the
     * select command is successful, it returns the interface type (which
     * should be FCI_TYPE_SERIAL).  If the select commands fails, or the
     * response says it is not a serial, we break out of this loop and
     * return.  If things go OK, we initialize the port.
     */
    for (port_num = 0; port_num < MAX_CTRLR_INTERFACES; ++port_num) {
	status = send_select_cmd(slot, port_num, &interface_type);
	if (status != FCI_RSP_OKAY) {
	    char name[CARDNAMELEN + 5]; /* room for name + slot number */

	    sprintf(name, "%s%u", slot->card_name, slot->slot_num);
	    errmsg(&msgsym(CMD, CBUS), "select", status, name);
	    break;
	}
	if (interface_type == FCI_TYPE_OUT_OF_RANGE)
	    break;
	if (interface_type != FCI_TYPE_SERIAL) {
	    errmsg(&msgsym(PORTTYPE,CBUS), slot->card_name, slot->slot_num,
		   port_num, interface_type);
	    break;
	}

	/*
	 * Now create the cdb for this plughole.
	 *
	 * Before calling create_context() we need to
	 * determine the applique type.  Since we don't
	 * have any mipinfo structures, or hwidbs, we do
	 * this with direct calls to the send_ip_cmd() layer.
	 * Note that the relevant plughole has already been
	 * selected at the top of the for loop.  Even in the
	 * case where an applique is not installed, 
	 * mip_create_context() will do the right thing,
	 * and return a NULL cdb.
	 */
	send_ip_cmd(slot, CCB_CMD_APPLIQUE);
	applique_type = get_ip_val(slot);
	unit = (slot->slot_num * MAX_CTRLR_INTERFACES) + port_num;

	/*
	 * On the RSP, we never get called at interrupt level,
	 * even during hotswap, hence we unconditionally pass
	 * firsttime=TRUE, to mip_create_context().  This has
	 * some implications for the code that 'deallocates'
	 * these structures (mip and dsx).  We don't want this
	 * code to put these back on the preQ.  If they do,
	 * we'll never get to use them again.
	 */
	cdb = mip_create_context(slot, unit, interface_type,
				     TRUE, applique_type);

	/*
	 * See if it created one.  The most likely reason for it
	 * not doing so, is that no applique is installed for this
	 * plughole.  In that case, we'll just ignore it, and we
	 * won't create any base idbs for it either.
	 */
	if (cdb == NULL)
	    continue;

	/*
	 * Get the 'base' IDB for this MIP plughole.  The 'base' IDB
	 * that we create for each  MIP plughole is not used for
	 * transferring data.  Rather, it is used to to allow the
	 * MEMD carving code to do it's stuff, since in RSP-land,
	 * the carving code is based on HWIDBs, not CDBs.
	 * If we find a matching one, we can just use it,
	 * otherwise we init a new one.  In either case, we want
	 * to reload it from the cdb, since the configuration may
	 * have changed, and the 'base' hwidbs are not kept up-to-date.
	 */
	hwidb = rsp_get_matching_idb(slot, port_num,
				     FCI_TYPE_BASE | FCI_TYPE_SERIAL,
				     cdb->idbtype);
	if (hwidb == NULL) {
	    break;		/* mip_get_matching_idb() complains */
	}			/* about malloc() failure */

	/*
	 * Set up a limited number of fields.  Because these 'base'
	 * idb's are only used for MEMD carving, only a select collection
	 * of relevant fields are set up.
	 */

	/*
	 * Protect ourselves in case anybody tries to reset us
	 * before MEMD is carved:
	 */
	hwidb->reset = (reset_t) return_nothing;
	hwidb->periodic = NULL;

	slot->interface_list[port_num] = hwidb;
	slot->interface_count = 1 + port_num;

	if (hwidb->status & IDB_NOHARDWARE) {
	    int num_mip_ints;

	    /* after hot swap */
	    num_mip_ints = mark_hwidbs_hardware_present(hwidb);

	    nserialnets += num_mip_ints;
	    nnets += num_mip_ints;

	    continue;
	}

	hwidb->next_idb_on_physint = NULL;
	hwidb->status = IDB_SERIAL | IDB_SYNC | FCI_IDBFLAGS;
	hwidb->cbus_ctrlr_index = hwidb->slot;

	/*
	 * this needs to match the way mci_index is set for other idbs
	 * in the common idb init code
	 */
	hwidb->mci_index = unit;

	/*
	 * Set up a few name strings.  Although the base hwidbs
	 * will never show up in a 'show interface', they are
	 * used as the basis of the 'show controller cbus' command.
	 * For this display, we need hw_namestring and applique_name.
	 */
	hwidb->hw_namestring = cdb->cdb_namestring;
	if (hwidb->hw_namestring)
	    mem_lock(hwidb->hw_namestring);
	mci_applique(hwidb);

	hwidb->love_note = mip_love_missive;

	/*
	 * The following fields are the one's used by the buffer
	 * carving logic.  The values we require have been loaded
	 * into the cdb by mip_create_context(), so we now place
	 * them into our 'base' hwidb, so that the buffer carving
	 * code can get at 'em.  Should the buffer carving logic
	 * ever require more fields, be sure to add them here as well.
	 */
	hwidb->buffersize = cdb->buffersize;
	if_maxdgram_adjust(hwidb, cdb->max_buffer_size);
	hwidb->max_buffers = cdb->max_buffers;
	hwidb->cbus_burst_count = cdb->cbus_burst_count;
	set_default_interface_bandwidth(hwidb->firstsw,
					SCALED_BANDWIDTH(cdb->bandwidth_def));


	/*
	 * This field is not actually used by the buffer carving
	 * code, but it is sent down to the plughole from within
	 * rsp_common_if_init() so we need to set it up before then.
	 * No time like the present.
	 */
	platform_mip_set_offsets(hwidb);
    }

    if (slot->ctrlr_specific == FCI_SMIP_CONTROLLER)
 	++nSMIPs;
    else
 	++nMIPs; 
}

/*****************************************************************************
 * Name: mip_ctrlr_init
 *
 * Description: per-card initialisation for the MIP.
 *   Alas, the use of the term controller is somewhat overloaded,
 *   especially in MIP land.  In this context (i.e. this routine),
 *   controller means the entire MIP card, *not* a particular plughole.
 *   This function is called during the initialization phase (after analyze)
 *   and after the configuration has been parsed.  The fields in the slottype
 *   have been filled in -- the information needs to be sent to the MIP now. 
 *   It may also be necessary to pass global information to the MIP.
 *   The algorithm is 
 *
 *       Send Set Global Queue Ptrs command
 *       Ensure the max love letter length is within allowable limits
 *
 * Notes: 
 *   This function is *NEVER* called from interrupt level.
 *
 * Input: 
 *   slot -- pointer to a slottype structure for the IP.  All fields should be
 *     valid when this function is called, but specifically
 *       slot_num -- slot number for this card
 *       ccb_ptr -- pointer to the CCB for the TRIP
 *       cmd_q_ptr -- pointer to the CCB for the TRIP
 *       event_q_ptr -- pointer to the event queue for this card to use
 *       love_note_free_q_ptr -- pointer to the queue of free love note buffer
 *         headers 
 *       love_letter_free_q_ptr -- pointer to the queue of free love letter
 *         buffers
 *
 * Output:
 *   Commands sent to the card
 *
 * Return Value: None
 *
 *****************************************************************************/
static void mip_ctrlr_init (slottype *slot)
{
    rsp_common_ctrlr_init(slot);
}

/*****************************************************************************
 * Name: mip_if_init
 * 
 * Notes: 
 *   This function is *NEVER* called from interrupt level.
 *   The MIP has multiple hwidbs per plughole.  This routine
 *   will only ever be called for 'base' hwidbs of which there
 *   is only per plughole.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface.  All fields
 *	should be valid when this function is called (as many as possible),
 *	but specifically
 *       raw_q_ptr
 *       rx_limit_lo
 *       rx_limit_hi
 *       global_free_q_size
 *       global_free_q_ptr
 *       local_free_q_ptr
 *       tx_q_ptr
 *       tx_acc_ptr
 *       rx_offset
 *
 * Output:
 *   None.
 *****************************************************************************/
static void mip_if_init (hwidbtype *hwidb)
{
    leveltype level;
    cdbtype *cdb;
    hwidbtype *virtual_hwidb;

    level = raise_interrupt_level(NETS_DISABLE);

    cdb = controller_find(hwidb->slot, hwidb->slotunit);

    /* 
     * If there are already channel groups created at this time,
     * then they still have null buffer info fields since at the
     * time they were created, we hadn't carved yet.  Now that we
     * have carved, and we know that the base hwidb now has useful
     * buffer info in it, we'll traverse any existing channel groups
     * and re-program their buffer info fields.
     */
    virtual_hwidb = hwidb->next_idb_on_physint;
    while (virtual_hwidb) {
	load_idb_buffer_info(virtual_hwidb, cdb);
	virtual_hwidb->old_txcount = 0;
	virtual_hwidb = virtual_hwidb->next_idb_on_physint;
    }


    /*
     * Next, send down the per-plughole commands for this 
     * base hwidb.
     */
    select_if(hwidb);
    rsp_common_if_init(hwidb);
    send_rxbytes_cmd(hwidb,hwidb->max_buffer_size);

    reset_interrupt_level(level);
}

/*
 * invoked when a MIP has been removed from the system
 */
static void
mip_ctrlr_removed (slottype *sp)
{
    int ix;
    hwidbtype *hwidb;
    mipinfo *mip;

    for (ix = 0; ix < sp->interface_count; ++ix) {
	hwidb = sp->interface_list[ix];
	if (hwidb == NULL)
	    continue;
	mip = mip_find_by_idb(hwidb);
	if (mip) {
	    switch (mip->applique_type) {
	    case APPL_T1:
		if (sp->ctrlr_specific == FCI_SMIP_CONTROLLER)
		    --nst1ports;
		else
		    --nt1ports;
		break;
	    case APPL_E1_BAL:
	    case APPL_E1_UNBAL:
		if (sp->ctrlr_specific == FCI_SMIP_CONTROLLER)
		    --nse1ports;
		else
		    --ne1ports;
		break;
	    default:
		break;
	    }
	}
	while ((hwidb = hwidb->next_idb_on_physint) != NULL) {
	    --nnets;
	    --nserialnets;
	}
    }
    if (sp->ctrlr_specific == FCI_SMIP_CONTROLLER)
 	--nSMIPs;
    else
 	--nMIPs;
    return;
}

/*
 * display count of MIPs in the system
 */
static void
mip_print_controllers (void)
{
    if (nt1ports + ne1ports) {
	printf("%u MIP controller%s", ArgAndPlural(nMIPs,"","s"));
	if (nt1ports)
	    printf(" (%u T1)", nt1ports);
	if (ne1ports)
	    printf(" (%u E1)", ne1ports);
	printf(".\n");
    }
    if (nst1ports + nse1ports) {
	printf("%u SMIP controller%s", ArgAndPlural(nSMIPs,"","s"));
	if (nst1ports)
	    printf(" (%u T1)", nst1ports);
	if (nse1ports)
	    printf(" (%u E1)", nse1ports);
	printf(".\n");
    }
    return;
}

/*****************************************************************************
 * Name: mip_platform_init
 *
 * Description: platform specific MIP init
 *
 * Input: 
 *   subsys -- pointer to the subsystype for this subsystem
 *
 * Return Value: None
 *
 *****************************************************************************/
void mip_platform_init (subsystype *subsys)
{
    nMIPs = 0;
    nSMIPs = 0;
    nt1ports = 0;
    ne1ports = 0;
    nst1ports = 0;
    nse1ports = 0;
    reg_add_ctrlr_analyze(FCI_MIP_CONTROLLER,
			  mip_ctrlr_analyze, "mip_ctrlr_analyze");
    reg_add_ctrlr_init(FCI_MIP_CONTROLLER, mip_ctrlr_init, "mip_ctrlr_init");
    reg_add_if_init(FCI_MIP_CONTROLLER, mip_if_init, "mip_if_init");
    reg_add_ctrlr_removed(FCI_MIP_CONTROLLER,
			  mip_ctrlr_removed, "mip_ctrlr_removed");
    reg_add_print_controllers(mip_print_controllers,
			      "mip_print_controllers");
}
