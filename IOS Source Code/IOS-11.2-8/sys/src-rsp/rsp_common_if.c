/* $Id: rsp_common_if.c,v 3.6.20.6 1996/08/28 13:13:37 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_common_if.c,v $
 *------------------------------------------------------------------
 * Interface routines for RSP
 *
 * May 1994, Jack Jenney
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_common_if.c,v $
 * Revision 3.6.20.6  1996/08/28  13:13:37  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.20.5  1996/08/13  02:24:09  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.6.20.4  1996/08/02  00:33:15  mwu
 * CSCdi64153:  mips txlimit set to zero in a loaded 75xx
 * Branch: California_branch
 *
 * Revision 3.6.20.3  1996/07/08  22:52:35  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.6.20.2  1996/06/22  00:42:09  ganesh
 * CSCdi57740:  backing store should not default to on
 * Branch: California_branch
 *
 * Revision 3.6.20.1  1996/03/18  22:02:01  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.12.3  1996/03/13  02:01:08  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.12.2  1996/02/20  18:00:24  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/01  06:09:47  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5.12.1  1996/01/24  23:03:50  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.5  1995/12/19  00:41:06  hampton
 * Obsolete the GET_NONZERO_TIMESTAMP routine.  It is no longer necessary
 * with 64bit timestamps.  [CSCdi45208]
 *
 * Revision 3.4  1995/12/18  06:52:42  schaefer
 * CSCdi42912: cxbus IPs displayed as cybus
 *
 * Revision 3.3  1995/11/17  18:47:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:32:53  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:27:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:28:15  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/08/09  00:21:57  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.5  1995/08/08  19:31:53  fred
 * CSCdi36704:  IP Fastswitching / WFQ cleanup for high-end
 * Assure that PQ/CQ/WFQ use a minimum tx-queue-limit of two
 *
 * Revision 2.4  1995/06/30  20:01:02  fred
 * CSCdi36499:  priority-group causes fddi interface down
 *              resulted from incorrect type of variable
 *
 * Revision 2.3  1995/06/23  18:48:34  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.2  1995/06/16  17:26:10  fred
 * Propagate the MCI change in the default tx-queue-limit for priority to
 * the RSP, and to LAPB while we're at it.
 *
 * Revision 2.1  1995/06/07  22:57:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "logger.h"
#include "interface_private.h"
#include "rsp_pcmap.h"
#include "../src-rsp/rsp_if.h"
#include "../src-rsp/rsp_bufhdr.h"
#include "../src-rsp/rsp_qasic.h"
#include "../hes/if_hes_common.h"
#include "../hes/ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/if_fci.h"
#include "../hes/slot.h"
#include "../hes/if_cmd.h"
#include "../if/if_controller.h"
#include "../hes/hes_controller.h"
#include "../hes/if_mci.h"
#include "../hes/cbus_registry.h"
#include "../hes/if_hes_serial.h"
#include "../if/network.h"
#include "../hes/msg_cbus.c"		/* Not a typo, see logger.h */
#include "msg_rsp.c"			/* Not a typo, see logger.h */
#include "rsp_common_if.h"
#include "media_registry.h"



/*****************************************************************************
 * Name: serial_ctrlr_init
 *
 * Description: per-card initizliation for the serial IP
 *   This function is called during the initialization phase (after analyze)
 *   and after the configuration has been parsed.  The fields in the slottype
 *   have been filled in -- the information needs to be sent to the IP now. 
 *   It may also be necessary to pass global information to the IP.
 *   The algorithm is 
 *
 *       Send Set Global Queue Ptrs command
 *
 * Notes: 
 *   This function is *NEVER* called from interrupt level.
 *
 * Input: 
 *   slot -- pointer to a slottype structure for the IP.  All fields should be
 *     valid when this function is called, but specifically
 *       slot_num -- slot number for this card
 *       ccb_ptr -- pointer to the CCB for the IP
 *       cmd_q_ptr -- pointer to the CCB for the IP
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
void serial_ctrlr_init (slottype *slot)
{
    rsp_common_ctrlr_init(slot);
    return;
}

/*****************************************************************************
 * Name: serial_if_init
 *
 * Description: per-interface initialization for the serial IP
 *
 * Notes: 
 *   This function is *NEVER* called from interrupt level.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface.  All fields
 *      should be valid when this function is called (as many as possible),
 *      but specifically:
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
 *   Commands sent to the card
 *
 *****************************************************************************/
void serial_if_init (hwidbtype *hwidb)
{
    leveltype level;

    /*
     * disable interrupts and select the interface
     */
    level = raise_interrupt_level(NETS_DISABLE);
    select_if(hwidb);

    rsp_common_if_init(hwidb);

    send_rxbytes_cmd(hwidb,hwidb->max_buffer_size);
 
    hwidb->reset = (reset_t) cbus_mci_serial_reset;
    if (hwidb->status & IDB_POS) {
    send_specific_cmd_1arg_lret(hwidb, FCI_POSIP_SPECIFIC, POSIP_GET_MEM_SIZE,
				&hwidb->slot_ptr->mem_size);
    }


    /* 
     * reset the interrupt level to its original value 
     */
    reset_interrupt_level(level);
}

/*****************************************************************************
 * Name: platform_serial_reset_encsize
 *
 * Description: platform-specific serial reset encsize adjustments to the
 *   other fields of the hwidb.
 *
 * Note: hwidb->rxoffset and hwidb->span_macoffset are in bytes for the RSP
 *   and words for the c7000 and ags.
 *
 * Input:
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: None
 *
 *****************************************************************************/
void platform_serial_reset_encsize (hwidbtype *hwidb)
{
    rsp_serial_set_rxoffset(hwidb);
}

/*
 * Common routine used to set up serial interface HW IDBs.
 * Called by analyze code, and by interface reset code.
 */

void rsp_serial_set_rxoffset (hwidbtype *hwidb)
{
    /*
     * Note, that the units (bytes Vs words) used in the rxoffset
     * field is platform dependant.  On the RSP, the value represents
     * bytes (on other platforms, like the 7000, this value 
     * represents words).  The IP ucode on each platform has been
     * modified to expect the correct units for that platform.
     */
    idbtype *swidb;

    /* DK: aligned for rsp_ipflow_fs. */
    hwidb->rxoffset = RSP_SERIAL_PROTOCOL_OFFSET - hwidb->encsize;

    hwidb->ip_memd_offset = RSP_SERIAL_PROTOCOL_OFFSET + ADRSPC_MEMD_WRITETHRU;

    switch (hwidb->enctype) {
    case ET_PPP:
    case ET_SMDS:
        hwidb->span_encapsize = hwidb->encsize + 2;
        hwidb->span_macoffset = hwidb->rxoffset + hwidb->span_encapsize; 
	hwidb->rsrb_vringoffset = hwidb->span_macoffset;
        break;

    case ET_HDLC: 
	if (!(system_loading | bootstrap_enable)) {
	    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
		reg_invoke_media_interesting_addresses(swidb);
	    }
        }  /* fall through */
	
    default:
        hwidb->span_encapsize = hwidb->encsize;
	hwidb->span_macoffset = RSP_SERIAL_PROTOCOL_OFFSET;
	hwidb->rsrb_vringoffset = RSP_SERIAL_PROTOCOL_OFFSET;
        break;
    }
}


/*****************************************************************************
 * Name: calculate_txlimit
 *
 * Description:  Calculate txq limit
 *
 * Input: 
 *   hwidb -- a pointer to the hardware IDB for the interface 
 *      
 * Return Value: txlimit
 *
 *****************************************************************************/
ushort calculate_cbus_txlimit (hwidbtype *hwidb)
{
    int txlimit;

    if (hwidb->mci_txcount_enabled) {
	txlimit = hwidb->mci_txcount;
    } else if (hwidb_use_short_tx_q_inline(hwidb)) {
	if (hwidb->type == IDBTYPE_E1 || hwidb->type == IDBTYPE_T1) {
	    /*
	     * The channelized interface with NxDS0 may not have sufficient 
	     * txlimit to support its line rate with the default formula
	     * in the 'else' branch that scales the txlimit with 512. 
	     * Experiement indicates that txlimit=4 can support a 64kbps
	     * channel. The MIP can support the channel with less than 
	     * 64kbps. 
	     */
	    txlimit = RSP_MIP_MIN_TQL +
	      (hwidb->firstsw->visible_bandwidth / 65);
	} else {
	    /*
	     * get the bandwidth of the link from the "bandwidth" command.
	     * set tx-queue-count to a depth related to the actual bandwidth
	     */
	    txlimit = RSP_RES_PROC_TX + 1 + 
	      hwidb->firstsw->visible_bandwidth / MCI_DOUBLE_BUFFER;
	}
	if (txlimit > hwidb->mci_txcount_default)
	    txlimit = hwidb->mci_txcount_default;
    } else {
	txlimit = hwidb->mci_txcount_default;
    }

    if (!hwidb->mci_txcount_enabled) {
	/*
	 * allow at least one outstanding packet on the IP.
	 * The RSP will put packet on the hold queue if
	 * tql is <= RSP_RES_PROC_TX.
	 */
	if (txlimit <= RSP_RES_PROC_TX)
	    txlimit = RSP_RES_PROC_TX + 1;
    }
    return txlimit;
}

/*****************************************************************************
 * Name: cbus_mci_txlimit
 *
 * Description:  Set the TX output queue limit for an interface.
 *   If priority queueing is enabled, it sets the txlimit to 1.
 *
 * Input: 
 *   hwidb -- a pointer to the hardware IDB for the interface 
 *     priority_list -- indicates if priority queuing is done on this interface
 *     mci_txcount -- the value to set the TX Limit to.  Note the problem
 *       that this is sometimes called txlimit and sometimes called txcount.
 *      
 * Return Value: None
 *
 *****************************************************************************/
void cbus_mci_txlimit (hwidbtype *hwidb)
{
    ulong txlimit;

    txlimit = calculate_cbus_txlimit(hwidb);
    hwidb->mci_txcount = txlimit;
    if (hwidb->old_txcount == 0)
	hwacc_set(hwidb->tx_acc_ptr, txlimit);
    else
	hwacc_sub(hwidb->tx_acc_ptr, hwidb->old_txcount - txlimit);
    hwidb->old_txcount = txlimit;
    return;
}

/*****************************************************************************
 * Name: cbus_mci_idb_typestring
 *
 * Description: Return point to type string
 *   This function is used in the "hwidb->typestring" vector in the IDB.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: 
 *   typestring -- pointer to the typestring
 *
 *****************************************************************************/
char *cbus_mci_idb_typestring (hwidbtype *hwidb)
{
    switch (hwidb->type) {
      case IDBTYPE_CBUSETHER:
	return "cxBus Ethernet";

      case IDBTYPE_CBUSSERIAL:
	return "cxBus Serial";

      case IDBTYPE_FDDIT:
	return "cxBus FDDI";

      case IDBTYPE_CTR:
	return "cxBus Token Ring";

      case IDBTYPE_HSSI:
	if (hwidb->status & IDB_POS) 
	    return "cyBus Packet over Sonet";
	else
	    return "cxBus HSSI";

      case IDBTYPE_T1:
	return "cxBus T1";

      case IDBTYPE_E1:
	return "cxBus E1";

      case IDBTYPE_AIP:
	return "cxBus ATM";

      case IDBTYPE_CIP:
	return "cyBus Channel Interface";

      case IDBTYPE_FEIP:
	return "cyBus FastEthernet Interface";

      default:
	break;
    }
    return("unknown");

}

/*****************************************************************************
 * Name: rsp_idb_unthrottle
 *
 * Description: Unthrottle an interface if it was previously throttled.
 *   This function is called via the (poorly named) enable() vector in
 *   in the hwidb.  This vector is used for interface unthrottling, not
 *   interface enabling, as the name may suggest. rsp_idb_unthrottle()
 *   is the RSP equivalent of the ags/c7000's cbus_mci_enable().
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *****************************************************************************/
static void rsp_idb_unthrottle (hwidbtype *hwidb)
{
    if (idb_is_throttled(hwidb)) {
	idb_mark_unthrottled(hwidb);
	
	/*
	 * We have been throttling this interface -- fool keepalives
	 * into thinking that keepalives are still functioning.
	 */
	GET_TIMESTAMP(hwidb->mci_lineuptimer);
    }
}

/*
 * return number of buffers on the interface's tx queue
 */
static int
rsp_txqlength (hwidbtype* hwidb)
{
    return hwidb->old_txcount - hwacc_peek(hwidb->tx_acc_ptr);
}

/*****************************************************************************
 * Name: platform_idb_init
 *
 * Description: Platform-specific IDB initialization for RSP
 *   This function is called from common_idb_init_first() to do all the IDB
 *   initialization that is peculiar to the RSP, and does not need to
 *   be done on other platforms.  It is called for all interface
 *   types.  Interface type specific initialization should go in other
 *   functions -- not here.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *     The following fields in the IDB must be filled in beforehand
 *       slot -- the slot number in which the interface
 *       slotunit -- the bit-significant status of the IDB
 *****************************************************************************/
void platform_idb_init (hwidbtype *hwidb)
{
    hwidb->pak_backing_store = FALSE;		/* Disable by default */
    hwidb->txqlength = rsp_txqlength;
    hwidb->enable = rsp_idb_unthrottle;
    hwidb->soutput = rsp_tx_start;
    hwidb->fastsend = rsp_fastsend;
    hwidb->cbus_ctrlr_index = hwidb->slot;

    hwidb->ip_turbo_fs = (ip_turbo_fs_t) turbo_disabled;

    /*
     * interfaces created at system init time are "down" (but not
     * "admin-down") by default, while those created at insertion time
     * are either "admin-down" (set after idb is created) or retain
     * their pre-removal state
     */
    if (systeminit_flag == FALSE)
	hwidb->state = IDBS_DOWN;

    /*
     * Set hwidb->devctl to NULL so that the bad access trap handler
     * catches and flags the NULL pointer dereferences:
     */
    hwidb->devctl = NULL;
    hwidb->mci_index = hwidb->slotunit + hwidb->slot * MAX_CTRLR_INTERFACES;

    /*
     * attempt to allocate a MAC address
     */
    (void) reg_invoke_assign_mac_addr(hwidb);

    return;
}

/*
 * send common per-slot init stuff down to IP.
 */
void
rsp_common_ctrlr_init (slottype* sp)
{
    ushort status;
    ushort max_love_letter_length;

    /*
     * raw queue ptr command
     */
    status =
	send_global_ptrs_cmd(sp,
			     BUFHDR_ADDR_TO_OFF(sp->event_q_ptr),
			     BUFHDR_ADDR_TO_OFF(sp->love_note_free_q_ptr),
			     BUFHDR_ADDR_TO_OFF(sp->love_letter_free_q_ptr),
			     &max_love_letter_length);

    if (status != FCI_RSP_OKAY) {
	char name[CARDNAMELEN + 5]; /* room for name + slot number */

	sprintf(name, "%s%u", sp->card_name, sp->slot_num);
	errmsg(&msgsym(CMD, CBUS), "global ptrs", status, name);
    }
    else if (max_love_letter_length > RSP_MAX_LOVE_LETTER) {
	char name[CARDNAMELEN + 5]; /* room for name + slot number */

	sprintf(name, "%s%u", sp->card_name, sp->slot_num);
	errmsg(&msgsym(LOVEGIANT, RSP), name, max_love_letter_length,
	       RSP_MAX_LOVE_LETTER);
    }
    return;
}

/*
 * send common per-interface init stuff down to IP.
 * requires NETS_DISABLE interrupt level, and the 
 * appropriate interface be selected
 */
void
rsp_common_if_init (hwidbtype* hwidb)
{
    ushort status;

    /*
     * raw queue ptr command
     */
    status = send_raw_queue_ptr_cmd(hwidb,
				    BUFHDR_ADDR_TO_OFF(hwidb->raw_q_ptr));
    if (status != FCI_RSP_OKAY) {
	errmsg(&msgsym(CMD, CBUS), "raw queue", status, hwidb->hw_namestring);
	return;
    }

    /*
     * free queue ptr command
     */
    status =
	send_free_queue_ptrs_cmd(hwidb,
				 BUFHDR_ADDR_TO_OFF(hwidb->local_free_q_ptr),
				 BUFHDR_ADDR_TO_OFF(hwidb->global_free_q_ptr),
				 BUFHDR_ADDR_TO_OFF(hwidb->global_free_q_size)
				 );
    if (status != FCI_RSP_OKAY) {
	errmsg(&msgsym(CMD, CBUS), "free queue", status, hwidb->hw_namestring);
	return;
    }

    /*
     * rx limits command
     */
    status = send_rx_limits_cmd(hwidb,
				hwidb->rx_limit_lo, hwidb->rx_limit_hi);
    if (status != FCI_RSP_OKAY) {
	errmsg(&msgsym(CMD, CBUS), "rx limits", status, hwidb->hw_namestring);
	return;
    }

    /*
     * tx queue ptrs command
     */
    status = send_tx_ptrs_cmd(hwidb,
			      BUFHDR_ADDR_TO_OFF(hwidb->tx_q_ptr),
			      BUFHDR_ADDR_TO_OFF(hwidb->tx_acc_ptr));
    if (status != FCI_RSP_OKAY) {
	errmsg(&msgsym(CMD, CBUS), "tx queue", status, hwidb->hw_namestring);
	return;
    }

    /*
     * rx offset command, note the RSP stores rxoffset in BYTES.
     */
    status = send_rx_offset_cmd(hwidb, hwidb->rxoffset);
    if (status != FCI_RSP_OKAY) {
	errmsg(&msgsym(CMD, CBUS), "rx offset", status, hwidb->hw_namestring);
	return;
    }
    return;
}

/*****************************************************************************
 * Name: get_cbus_ctrlr_errors
 *
 * Description: Accumulates cBus interface errors kept by the SP.
 *   Called by the get_cbus_errors.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: None
 *
 *****************************************************************************/
void get_cbus_ctrlr_errors (hwidbtype  *hwidb)
{
    return;			/* gee, that was easy */
}

/*
 * ip_on_cbus2
 * Return true if the FDDI IP is in a CBUS II system,
 * or is in a CyBus system.
 */

boolean ip_on_cbus2 (hwidbtype *hwidb)
{
    return(TRUE);
}

/*
 * Platform specific routine to turn off an LED on an interface
 * in the next LED state variable:
 */

void platform_light_off (hwidbtype *hwidb)
{
    /* Nothing to do here on the RSP */
}




/*
 * Platform specific routine to turn on an LED on an interface
 * in the next LED state variable:
 */

void platform_light_on (hwidbtype *hwidb)
{
    /* Nothing to do here on the RSP */
}




/*
 * Platform specific routine to update the interface LEDs from
 * the  next LED state variable:
 */

void platform_update_lights (hwidbtype *hwidb)
{
    /* Nothing to do here on the RSP */
}

void
platform_show_switch_counts (hwidbtype *hwidb)
{
    idb_si_counters_t	*sic;

    sic = idb_get_counter_info(hwidb, IDB_COUNTER_SI_CNT_PTR);
    printf("\n%-24s %-10lu %-10lu %-10lu %-10lu",
	   "Distributed cache",
	   idb_auto_in(hwidb) - sic->auto_in,
	   idb_rx_auto_cumbytes(hwidb) - sic->rx_auto_cumbytes,
	   idb_auto_out(hwidb) - sic->auto_out,
	   idb_tx_auto_cumbytes(hwidb) - sic->tx_auto_cumbytes);
}
