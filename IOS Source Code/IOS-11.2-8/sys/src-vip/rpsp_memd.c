/* $Id: rpsp_memd.c,v 3.4.4.4 1996/08/28 13:15:18 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-vip/rpsp_memd.c,v $
 *------------------------------------------------------------------
 * rpsp_memd.c : VIP RPSP support routines (rvip/svip independent)
 *
 * February 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rpsp_memd.c,v $
 * Revision 3.4.4.4  1996/08/28  13:15:18  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.4.3  1996/04/12  17:44:37  kao
 * CSCdi53527:  Need EOIR support on VIP2
 * Branch: California_branch
 *
 * Revision 3.4.4.2  1996/03/21  23:52:08  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.4.4.1  1996/03/18  22:03:37  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.3  1996/03/18  23:36:26  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/02/21  02:54:46  mbeesley
 * CSCdi49429:  Messages to host (RP or RSP) are broken
 *         o Fix IPC and love letter processing
 *         o Fix buffer header stack processing for VIP1 on RP/SP
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1996/02/10  00:22:19  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.4  1996/03/08  23:20:16  kao
 * CSCdi50763:  TRIP OIR causes mic rel under traffic thru VIP
 *
 * Revision 3.3  1996/02/18  19:42:39  kao
 * CSCdi45136:  EOIR Problems with VIP card
 *
 * Revision 3.2  1995/11/17  18:51:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:28  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:59:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "packet.h"
#include "ttysrv.h"
#include "logger.h"

#include "../pas/if_pas.h"
#include "../pas/msg_pas.c"		/* Not a typo, see logger.h */
#include "../hes/ccb_cmd.h"
#include "../hes/dgbus.h"
#include "../hes/if_fci.h"

#include "vip_dma.h"
#include "vip_memd.h"
#include "vip_dbus.h"
#include "msg_vip.c"			/* Not a typo, see logger.h */

#if defined(RVIP)
# include "../src-rvip/rvip_dma.h"
#endif /* RVIP */

#if defined(SVIP)
# include "../src-svip/svip_dma.h"
#endif /* SVIP */

/*
 * Local storage
 */
static int icb_assigned;

/*
 * vip_rpsp_icb_address :
 * Determine an interfaces icb pointer in memd
 */
vip_rpsp_icb_t *vip_rpsp_icb_address (pas_cbtype *pascb)
{
    ulong base, addr;

    /*
     * Assign an ICB. If we are using extended ICB's, then PA0 gets
     * the normal ones, and PA1 gets the extended ones. If not, then
     * the eight normal ICB's are shared across both PA's.
     */
    if (pascb->pa_device >= MAX_USABLE_INTF_PER_PA) {
	return(NULL);
    }

    if (pascb->pa_bay == PA0) {
	base = ICB_BASE;
    } else {
	base = EXT_ICB_BASE;
    }
    addr = base +
	   (ICB_LENGTH * ICB_INTFCS * vsys_vec.vip_slot) +
	   (ICB_LENGTH * pascb->pa_device) +
	   ADRSPC_CBUS;
    return((vip_rpsp_icb_t *)addr);
}

/*
 * vip_rpsp_if_init:
 * Do vip specific interface initialization for an RPSP system.
 */
void vip_rpsp_if_init (hwidbtype *hwidb)
{
    pas_cbtype *pascb;
    vip_rpsp_icb_t *icb;
    vip_memd_ifcntl_t *ifc;
    int intfc;

    pascb = INSTANCE(hwidb);
    if (icb_assigned >= MAX_ICB_PER_SLOT) {
	errmsg(&msgsym(NOICBS,VIP), pascb->pa_bay, pascb->pa_device);
	return;
    }

    /*
     * Get flat interface handle and assign the icb pointer
     */
    icb = vip_rpsp_icb_address(pascb);
    if (!icb) {
	return;
    }
    intfc = (pascb->pa_bay * MAX_INTERFACES_PER_PA) + pascb->pa_device;
    icb_assigned += 1;

    /*
     * Record pascb in memd data structure
     */
    memd_cntl.pascb_table[intfc] = pascb;
    pascb->pa_ifindex = intfc;
    ifc = &vip_ifcntl_array[intfc];
    pascb->vip_memd_ifcntl = ifc;
    ifc->icb_ptr = icb;
    vip_rpsp_icb_array[intfc] = icb;

    /*
     * Initialize ifc fields
     */
    ifc->idb = hwidb;

    init_ifc_queues_caches(ifc);

    ifc->vip_if_status = VIP_DISABLE;
    ifc->old_vip_if_status = VIP_DISABLE;

    ifc->rx_offset = 0;

    /*
     * Initialize pascb fields
     */
    pascb->idb = hwidb;
    vip_pak_vector_init(pascb);

    /* 
     * Write fci_type used by malu attn interrupt handler 
     */
    memd_cntl.if_fcitype[intfc] = (hwidb->fci_type << 16) | CCB_DONE_OK;

    /*
     * currently RESET handler ccb_cmd_reset only re-write
     * the FCI types for all our interfaces into vip_rpsp_icb_array[]
     * not the icb in MEMD lower memory
     */
    if (vsys_vec.bufferin_enabled_sp) {
	icb->icb_rpsp_type = (hwidb->fci_type | FCI_BUFFERIN);
    } else {
	icb->icb_rpsp_type = hwidb->fci_type;
    }
}

/*
 * vip_rpsp_local_txq_sync:
 * Add the local tx queue to the memd return queue
 * Return false if unable to complete routine due to lock starvation
 */
boolean vip_rpsp_local_txq_sync (void)
{
    leveltype level;
    vip_bhdr_t *bh;

    if (!vsys_vec.rpsp_tx_qtail) {
	return(TRUE);
    }

    /*
     * Lock out interrupts and try to get the global lock
     */
    GLOBAL_INTERRUPT_BLOCK(level);
    if (!vip_acquire_global_lock()) {
	/*
	 * Clear the global lock request, re-enable interrupts
	 * and return FALSE
	 */
	vip_clear_global_lock_request();
	GLOBAL_INTERRUPT_UNBLOCK(level);
	return(FALSE);
    }
    
    /*
     * We were succesfull in acquiring the global lock.
     * Clear link field of last buffer and read the
     * rpsp_q_rettail
     */
    vsys_vec.rpsp_tx_qtail->bf_rpsp_link = 0;
    bh = vip_rpsp_memd_readbh(vsys_vec.rpsp_q_rettail);
    if (bh) {
	bh->bf_rpsp_link = RPSP_VIRTUAL2MEMD(vsys_vec.rpsp_tx_qhead);
	*(vsys_vec.rpsp_q_rettail) = RPSP_VIRTUAL2MEMD(vsys_vec.rpsp_tx_qtail);
    } else {
	*(vsys_vec.rpsp_q_rethead_tail) = 
	    ((ulong)vsys_vec.rpsp_tx_qhead << VIP_RPSP_BHFIELD_SHIFT) |
	    (RPSP_VIRTUAL2MEMD(vsys_vec.rpsp_tx_qtail));
    }
    
    /*
     * Clear local rpsp_tx_qtail variable, clear global lock request and
     * re-enable interrupts
     */
    vsys_vec.rpsp_tx_qtail = NULL;
    vip_clear_global_lock_request();
    GLOBAL_INTERRUPT_UNBLOCK(level);
    return(TRUE);
}

/*
 * vip_rpsp_local_rxq_sync:
 * Add the local rx queue to the memd raw queue
 * Return false if unable to complete routine due to lock starvation
*/
boolean vip_rpsp_local_rxq_sync (void)
{
    leveltype level;
    vip_bhdr_t *bh;

    if (!vsys_vec.rpsp_rx_qtail) {
	return(TRUE);
    }

    /*
     * Lock out interrupts and try to get the global lock
     */
    GLOBAL_INTERRUPT_BLOCK(level);
    if (!vip_acquire_global_lock()) {
	/*
	 * Clear the global lock request, re-enable interrupts
	 * and return FALSE
	 */
	vip_clear_global_lock_request();
	GLOBAL_INTERRUPT_UNBLOCK(level);
	return(FALSE);
    }

    /*
     * We were succesfull in acquiring the global lock.
     * Zero link field of last buffer and set length.
     */
    vsys_vec.rpsp_rx_qtail->bf_word0 = vsys_vec.rpsp_rx_last_length;
    bh = vip_rpsp_memd_readbh(vsys_vec.rpsp_q_rawtail);
    if (bh) {
	bh->bf_rpsp_link = RPSP_VIRTUAL2MEMD(vsys_vec.rpsp_rx_qhead);
	*(vsys_vec.rpsp_q_rawtail) = 
	    RPSP_VIRTUAL2MEMD(vsys_vec.rpsp_rx_qtail);
    } else {
	*(vsys_vec.rpsp_q_rawhead_tail) = 
	    ((ulong)vsys_vec.rpsp_rx_qhead << VIP_RPSP_BHFIELD_SHIFT) |
	    (RPSP_VIRTUAL2MEMD(vsys_vec.rpsp_rx_qtail));
    }
	
    /*
     * Clear local rpsp_rx_qtail variable, clear global lock request and
     * re-enable interrupts
     */
    vsys_vec.rpsp_rx_qtail = NULL;
    vip_clear_global_lock_request();
    GLOBAL_INTERRUPT_UNBLOCK(level);
    return(TRUE);
}
 
/*
 * vip_rpsp_stoke_rx_buffers:
 * keep some receive buffers on hand
 */
vip_bhdr_t *vip_rpsp_stoke_rx_buffers (pas_cbtype *pascb) 
{
    vip_memd_ifcntl_t *ifc = pascb->vip_memd_ifcntl;
    leveltype level;
    vip_bhdr_t *bh1, *bh2, *bh3;
    boolean got_buffer = FALSE;

    if (!vip_empty_bh_stack(ifc)) {
	bh1 = vip_dequeue_bh_stack(ifc);
	return(bh1);
    }

    /*
     * Lock out interrupts and get the global lock
     */
    GLOBAL_INTERRUPT_BLOCK(level);
    if (!vip_acquire_global_lock()) {
	/*
	 * Clear the global lock request, re-enable interrupts
	 * and return FALSE
	 */
	vip_clear_global_lock_request();
	GLOBAL_INTERRUPT_UNBLOCK(level);
	return(NULL);
    }
    
    /*
     * We got the global lock.  See if there are any buffers 
     * available. Remember we can't take the last buffer.
     */
    bh1 = vip_rpsp_memd_readbh(&ifc->rpsp_local_free_q->bf_rpsp_link);
    if (bh1) {
	/* 
	 * See if buffer header is hanging off of free queue 
	 */
	bh2 = vip_rpsp_memd_readbh(&bh1->bf_rpsp_link);
	if (bh2) {
	    if (!pending_cbus_dbus_intrpt()) {
		/* 
		 * See if we can grab two buffers off of free queue 
		 */
		bh3 = vip_rpsp_memd_readbh(&bh2->bf_rpsp_link);
		if (bh3){
		    ifc->rpsp_local_free_q->bf_rpsp_link = 
			RPSP_VIRTUAL2MEMD(bh3);
		    vip_clear_global_lock_request();
		    GLOBAL_INTERRUPT_UNBLOCK(level);
		    vip_enqueue_bh_stack(ifc, bh2);
		    got_buffer = TRUE;
		    
		    /* 
		     * Write owner field into buffer header 
		     */
		    bh2->bf_rpsp_rxicb = ifc->icb_ptr_memd;
		} else { 
		    ifc->rpsp_local_free_q->bf_rpsp_link = 
			RPSP_VIRTUAL2MEMD(bh2);
		    vip_clear_global_lock_request();
		    GLOBAL_INTERRUPT_UNBLOCK(level);
		}

	    } else {
		/* 
		 * Higher level interrupt pending - get out of here! 
		 */
		ifc->rpsp_local_free_q->bf_rpsp_link = RPSP_VIRTUAL2MEMD(bh2);
		vip_clear_global_lock_request();
		GLOBAL_INTERRUPT_UNBLOCK(level);
	    }
	    
	    /* 
	     * Update the stack for the first buffer we grabbed 
	     * off of the global free queue
	     */
	    vip_enqueue_bh_stack(ifc, bh1);
	    got_buffer = TRUE;
	    
	    /* 
	     * Write owner field into buffer header 
	     */
	    bh1->bf_rpsp_rxicb = ifc->icb_ptr_memd;

	} else {

	    /*
	     * bh2 was NULL
	     */
	    vip_clear_global_lock_request();
	    GLOBAL_INTERRUPT_UNBLOCK(level);
	}

    } else {

	/*
	 * bh1 was NULL
	 */
	vip_clear_global_lock_request();
	GLOBAL_INTERRUPT_UNBLOCK(level);
    }

    if (!got_buffer) {
	return(NULL);
    } else {
	return(vip_dequeue_bh_stack(ifc));
    }
}

/*
 * vip_rpsp_send_lovenote:
 * Allocate a memd buffer for sending the specified love note.
 */
boolean vip_rpsp_send_lovenote(pas_cbtype *pascb, ushort love_error)
{
    leveltype level;
    vip_bhdr_t *bh1, *bh2, *lq;
    vip_memd_ifcntl_t *ifc = pascb->vip_memd_ifcntl;

    /* 
     * Get buffer from love note queue 
     */
    lq = ifc->rpsp_loveq;
    if (!lq) {
	return(FALSE);
    }

    /*
     * Lock out interrupts and request the global lock
     */
    GLOBAL_INTERRUPT_BLOCK(level);
    if (!vip_acquire_global_lock()) {
	/*
	 * Clear the global lock request, re-enable interrupts
	 * and return FALSE
	 */
	vip_clear_global_lock_request();
	GLOBAL_INTERRUPT_UNBLOCK(level);
	return(FALSE);
    }

    /* 
     * Get the buffer header pointer at head of love note queue 
     */
    bh1 = vip_rpsp_memd_readbh(&lq->bf_rpsp_link);
    if (!bh1) {
	vip_clear_global_lock_request();
	GLOBAL_INTERRUPT_UNBLOCK(level);
	return(FALSE);
    }

    /* 
     * We cannot take the last entry off of the love note queue
     * See if another buffer header is hanging off of love queue 
     */
    bh2 = vip_rpsp_memd_readbh(&bh1->bf_rpsp_link);
    if (bh2) {
	lq->bf_rpsp_link = RPSP_VIRTUAL2MEMD(bh2);
	/*
	 * Release the global lock and let interrupts go
	 */
	vip_clear_global_lock_request();
	GLOBAL_INTERRUPT_UNBLOCK(level);
	
	/* 
	 * Indicate this is a love note by clearing the bytes
	 * field - clear link field while at it 
	 */
	bh1->bf_word0 = 0;
	bh1->bf_rpsp_rxsts = love_error;
	
	/*
	 * Write owner field into buffer header 
	 */
	bh1->bf_rpsp_rxicb = ifc->icb_ptr_memd;
	
	/* 
	 * Add MEMD buffer header to raw queue. Lock out interrupts, 
	 * get the global lock and add the MEMD buffer header to raw queue
	 */
	GLOBAL_INTERRUPT_BLOCK(level);
	if (!vip_acquire_global_lock()) {
	    /*
	     * Clear the global lock request, re-enable interrupts
	     * and return FALSE. *FIXME*, this looses the bh
	     */
	    vip_clear_global_lock_request();
	    GLOBAL_INTERRUPT_UNBLOCK(level);
	    return(FALSE);
	}
	
	bh2 = vip_rpsp_memd_readbh(vsys_vec.rpsp_q_rawtail);
	if (bh2) {
	    bh2->bf_rpsp_link = RPSP_VIRTUAL2MEMD(bh1);
	    *(vsys_vec.rpsp_q_rawtail) = RPSP_VIRTUAL2MEMD(bh1);
	} else {
	    *(vsys_vec.rpsp_q_rawhead_tail) = 
		((ulong)bh1 << VIP_RPSP_BHFIELD_SHIFT) |
		(RPSP_VIRTUAL2MEMD(bh1));
	}
    }

    /*
     * Release the global lock, re-enable interrupts and return
     */
    vip_clear_global_lock_request();
    GLOBAL_INTERRUPT_UNBLOCK(level);
    return(TRUE);
}

/*
 * vip_rpsp_return_scraped_tx_queue:
 * return buffers headers on scraped transmit queue
 */
static void vip_rpsp_return_scraped_tx_queue (vip_memd_ifcntl_t *ifc)
{
    leveltype level;
    vip_bhdr_t *head, *tail, *bh;

    /* 
     * Return transmit buffers that have been scraped
     */
    head = ifc->rpsp_scraped_txq;
    tail = ifc->rpsp_scraped_txq;
    bh = ifc->rpsp_scraped_txq;
    while (bh) {
	tail = bh;
	bh = vip_rpsp_memd_readbh(&bh->bf_rpsp_link);
    }

    if (head) {
	level = raise_interrupt_level(LEVEL_DMA); 	/* fixme */
	vip_rpsp_local_returnq_add_list(head, tail);
	reset_interrupt_level(level);
    }
}

/*
 * vip_rpsp_memd_disable_cleanup:
 * Clean up memd data structures following a disable command
 */
void vip_rpsp_memd_disable_cleanup (pas_cbtype *pascb)
{
    leveltype level;
    vip_memd_ifcntl_t *ifc = pascb->vip_memd_ifcntl;
    vip_bhdr_t *bh, *head, *tail;

    /* 
     * Add stoked memd buffers to local raw queue 
     * Usually there is only one buffer on this stack (if any) 
     */
    while (bh = vip_dequeue_bh_stack(ifc), bh) {
	bh->bf_rpsp_rxsts = 0;
	level = raise_interrupt_level(LEVEL_DMA);	/* fixme */
	vip_rpsp_local_raw_queue_add(bh, 0);
	reset_interrupt_level(level);
    }

    /* 
     * Add local free queue to local raw queue 
     */
    head = ifc->rpsp_local_freeq_head;
    tail = ifc->rpsp_local_freeq_head;
    if (head) {
	/*
	 * Scrape the free queue
	 */
	ifc->icb_ptr->icb_rpsp_freeq = 0;

	/*
	 * Flush the write of the free queue as we need 300 ns to expire
	 * between that event and the read of the last buffer's link field
	 */
	flush_io_wbuffer();
	ifc->rpsp_local_freeq_head = NULL;

	/* 
	 * Walk the local free queue and mark the buffers so they
         * will be returned to the global free queue
	 */
	bh = head;
	while (head) {
	    head->bf_rpsp_rxbytes = 0;
	    head->bf_rpsp_rxsts = 0;
	    tail = head;
	    head = vip_rpsp_memd_readbh(&bh->bf_rpsp_link);
	}    
	level = raise_interrupt_level(LEVEL_DMA);	/* fixme */
	vip_rpsp_local_rawq_add_list(head, tail, 0);
	reset_interrupt_level(level);
    }

    /* 
     * return buffers headers on scraped transmit queue 
     */
    vip_rpsp_return_scraped_tx_queue(ifc);

    /*
     * Scrape memd transmit queue one final time 
     */
    bh = vip_rpsp_memd_readbh(&ifc->icb_ptr->icb_rpsp_txque);
    if (bh) {
	ifc->icb_ptr->icb_rpsp_txque = 0;
	ifc->rpsp_scraped_txq = 0;
	vip_rpsp_return_scraped_tx_queue(ifc);
    }
    
    /* 
     * Re-initialize scraped queue indicator 
     */
    ifc->rpsp_scraped_txq = 0;
}

/*
 * vip_rpsp_kernel_ready:
 * Tell RP we are ready
 */
void vip_rpsp_kernel_ready (void)
{
    vip_set_ready(TRUE);
}

/*
 * vip_rpsp_init_memd_support:
 * do any memd initialization necessary for rpsp environment
 */
void vip_rpsp_init_memd_support (void)
{
    vip_rpsp_que_t *cx_memd_qs;

    /*
     * This is a global used when adding rx memd buffer headers to
     * the local raw queue list
     */
    vsys_vec.rpsp_rx_last_length = 0;

    /* 
     * Set up MEMD pointers - Remember these have to be in bytes 
     */
    memd_cntl.ccbptr = (ccbtype *)(ADRSPC_CBUS + CCB_BASE + 
				   (CCB_LENGTH * vsys_vec.vip_slot));

    /*
     * The SP will always be running bufferin microcode
     */
    vsys_vec.bufferin_enabled_sp = TRUE;

    /*
     * Assign RP/SP vsys_vec fields
     */
    cx_memd_qs = (vip_rpsp_que_t *)(ADRSPC_CBUS + QUE_BASE);
    vsys_vec.rpsp_q_rawtail = &(cx_memd_qs->q_rpsp_rawtail);
    vsys_vec.rpsp_q_rawhead_tail = &(cx_memd_qs->q_rpsp_rawque);
    vsys_vec.rpsp_q_rettail = &(cx_memd_qs->q_rpsp_rettail);
    vsys_vec.rpsp_q_rethead_tail = &(cx_memd_qs->q_rpsp_retque);

    vsys_vec.rpsp_rx_qtail = NULL;
    vsys_vec.rpsp_rx_qhead = NULL;
    vsys_vec.rpsp_tx_qtail = NULL;
    vsys_vec.rpsp_tx_qhead = NULL;
}

/* end of file */
