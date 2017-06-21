/* $Id: rvip_dma.c,v 3.9.4.6 1996/08/28 13:14:51 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-rvip/rvip_dma.c,v $
 *------------------------------------------------------------------
 * rvip_dma.c - DMA Engine support for Checkers
 *
 * January 1995, David Getchell and Paulina Tran
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rvip_dma.c,v $
 * Revision 3.9.4.6  1996/08/28  13:14:51  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.9.4.5  1996/05/31  22:33:55  feisal
 * CSCdi56695:  ISL Broken on certain card
 * Branch: California_branch
 *
 * Revision 3.9.4.4  1996/04/12  17:43:34  kao
 * CSCdi53527:  Need EOIR support on VIP2
 * Branch: California_branch
 *
 * Revision 3.9.4.3  1996/04/08  13:44:58  getchell
 * CSCdi52361:  DFS broken using unlike media
 * Branch: California_branch
 *
 * Revision 3.9.4.2  1996/03/21  23:50:48  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.9.4.1  1996/03/18  22:03:17  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.7  1996/03/18  23:35:55  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.6  1996/02/21  20:36:51  getchell
 * CSCdi49474:  Time to turn on IPC/DFS on viper
 * Branch: ELC_branch
 *
 * Revision 3.2.2.5  1996/02/10  00:19:08  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.4  1996/01/16  02:36:07  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.3  1995/12/11  19:24:46  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.2.2.2  1995/12/05  06:36:24  mbeesley
 * Branch: ELC_branch
 * Safeguard static paktype headers from being freed.
 *
 * Revision 3.2.2.1  1995/12/04  22:18:49  mbeesley
 * Branch: ELC_branch
 * Perform mini-sync - particle refcount changes.
 *
 * Revision 3.9  1996/03/08  23:20:08  kao
 * CSCdi50763:  TRIP OIR causes mic rel under traffic thru VIP
 *
 * Revision 3.8  1996/02/23  20:02:39  feisal
 * CSCdi45142:  Support VLAN ISL for new 1FE port adapter
 *
 * Revision 3.7  1996/02/18  19:42:32  kao
 * CSCdi45136:  EOIR Problems with VIP card
 *
 * Revision 3.6  1995/12/10  04:01:38  smackie
 * Add support for public particle pools. Make particle chunk growth less
 * aggressive for dynamic pools. (CSCdi45336)
 *
 * Revision 3.5  1995/12/01  21:50:55  mbeesley
 * CSCdi44770:  Checkers DMA device driver has buffer leak
 *
 * Revision 3.4  1995/11/30  04:34:29  dlobete
 * CSCdi44690:  ADMINDOWN VIP interfaces don't allow IPC or love-letters
 *
 * Revision 3.3  1995/11/28  02:26:25  smackie
 * Move particle refcount into local memory to improve handling speed. Add
 * utility functions to allow some particle internals to be removed from
 * the drivers. (CSCdi44505)
 *
 * Revision 3.2  1995/11/17  18:50:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:11  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:59:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "stacks.h"
#include "ttysrv.h"
#include "logger.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"

#include "../pas/if_pas.h"
#include "../src-vip/vip_dma.h"
#include "../src-vip/vip_memd.h"
#include "../src-vip/vip_debug.h"
#include "../src-vip/vip_isl.h"

#include "msg_dma.c"			/* Not a typo, see logger.h */
#include "rvip_dma.h"

/*
 * Local storage
 */
volatile boolean dma_working;
int dma_timer_check;
dma_entry_t *dma_free, *dma_reqt, *dma_done;
dma_cntl_t dmac;

/*
 * vip_set_ipc_queues:
 * Setup IPC queues. Only on RSP system.
 */
boolean vip_set_ipc_queues (malu_cmd_t *cmd)
{
    if (RPSP_SELECT) {
	return(FALSE);
    }
    vsys_vec.rsp_ipc_tx_q = 
	(volatile ulong *)(RSP_MEMD2VIRTUAL(cmd->arg0));
    vsys_vec.rsp_ipc_rx_q = 
	(volatile ulong *)(RSP_MEMD2VIRTUAL(cmd->arg1));
    vsys_vec.rsp_ipc_free_q = 
	(volatile ulong *)(RSP_MEMD2VIRTUAL(cmd->arg2));
    return(TRUE);
}

/*
 * vip_set_tx_pointers:
 * Setup transmit pointers. Not supported on RP/SP systems
 */
boolean vip_set_tx_pointers (malu_cmd_t *cmd)
{
    vip_memd_ifcntl_t *ifc;

    if (vsys_vec.rpsp_host) {
	return(FALSE);
    }
    ifc = cmd->pascb->vip_memd_ifcntl;
    ifc->rsp_tx_q = (volatile ulong *)(RSP_MEMD2VIRTUAL(cmd->arg0));
    ifc->rsp_tx_acc = (volatile short *)(RSP_MEMD2VIRTUAL(cmd->arg1));
    return(TRUE);
}

/*
 * rvip_dma_flush_pak:
 * Cleanup the temporary DMA paktype
 */
static inline void rvip_dma_flush_pak (paktype *pak)
{
    pak->datagramsize = 0;
    pak->datagramstart = NULL;
    pak->particlequeue.count = 0;
    pak->particlequeue.qhead = NULL;
    pak->particlequeue.qtail = NULL;
}

/*
 * rvip_poll_txqueue:
 * Poll a queue for a buffer to transmit
 */
static inline vip_bhdr_t *rvip_poll_txqueue (vip_memd_ifcntl_t *ifc,
					     rvip_dma_poll_t *ptr)
{
    vip_bhdr_t *bh;

    if (!vsys_vec.rpsp_host) {

	/*
	 * Read a buffer header off the tx queue for this interface
	 */
	bh = vip_rsp_memd_readbh(ifc->rsp_tx_q);
	return(bh);

    } else {

	/*
	 * We are running on an RP/SP system. SP queues must be scraped when 
	 * read, a record must be maintained of local copies of the SP tx
	 * queues.
	 */
	bh = ifc->rpsp_scraped_txq;
	if (bh) {
	    ifc->rpsp_scraped_txq = vip_rpsp_memd_readbh(&bh->bf_rpsp_link);
	    return(bh);
	}

	/*
	 * No previously scraped queue. Read memd for new queue
	 */
	bh = vip_rpsp_memd_readbh(ptr->icb_tx_q);
	if (bh) {
	    *(ptr->icb_tx_q) = 0;
	    ifc->rpsp_scraped_txq = vip_rpsp_memd_readbh(&bh->bf_rpsp_link);
	    return(bh);
	}
    }
    return(NULL);
}

/*
 * rvip_check_tx_queues:
 * Check the transmit queueus
 */
static inline vip_bhdr_t *rvip_check_tx_queues (vip_memd_ifcntl_t **ifc)
{
    int i, time;
    rvip_dma_poll_t *ptr;
    vip_bhdr_t *bh;

    /*
     * Check that we have something to poll. If so, poll at 
     * most rvip_poll_count different queues.
     */
    for (i=0; i<dmac.rvip_dma_runq.count; i++) {

	/*
	 * Poll dmac.rvip_poll_ptr queue
	 */
	ptr = dmac.rvip_poll_ptr;
	dmac.rvip_poll_ptr = ptr->next;
	if (!dmac.rvip_poll_ptr) {		   	/* check wrap */
	    dmac.rvip_poll_ptr = dmac.rvip_dma_runq.qhead;
	}
	*ifc = ptr->ifc;

	/*
	 * If the output_qcount is above the threshold, do not poll 
	 * this interface as it could result in an output drop.
	 */
	if ((*ifc)->idb->output_qcount > RVIP_DMA_OUTPUTQ_LIMIT) {
	    continue;
	}
	    
	/*
	 * If the last time we polled this queue, there was nothing
	 * there, we started up the usec_time timer. If that timer
	 * has not expired, do not bother polling this queue. If the
	 * timer has expired, reset it.
	 */
	if (ptr->last_poll_failed) {
	    time = USEC_ELAPSED_TIME(ptr->usec_time);
	    if (time < ptr->poll_usec_interval) {
		continue;
	    }
	    USEC_GET_TIMESTAMP(ptr->usec_time);
	}

	if (bh = rvip_poll_txqueue(*ifc, ptr), bh) {
	    ptr->last_poll_failed = FALSE;
	    return(bh);
	}
	ptr->last_poll_failed = TRUE;
	USEC_GET_TIMESTAMP(ptr->usec_time);
    }
    return(NULL);
}

/*
 * dma_startup:
 * Start up the dma engine if it is idle.
 */
static inline void dma_startup (void)
{
    int temp;

    /*
     * Make sure the dma engine is not busy
     */
    if (dma_working || (dma_reqt == dma_free) || is_cbus_quiesced()) {
	return;
    }

    /*
     * Jump start the dma engine
     */
    temp = rvip_dma_jumpstart();
    dmac.dma_start_count += 1;
    if (!temp) {
	errmsg(&msgsym(BADJUMPSTART, DMA), temp);
    }
}

/*
 * Flush dma_reqt. Must be called at ALL_DISABLE level (DMA interrupt still 
 * enabledor) or up to prevent new DMA request. Put extra delay before setting
 * dma_reqt to dma_free if DMA interrupt is not disabled when calling this
 * function
 */
void vip_flush_dmaq (void)
{
    dma_entry_t *dma;

    /*
     * Take the current snapshot of dma_reqt, vip_dma_interrupt may be active
     * or occur later but since 
     */
    dma = dma_reqt;
    while (dma != dma_free) {
	dmac.flushed_reqt_count++;
	/*
	 * Mark entry as DONE and not REQUEST
	 */
	dma->status &= ~RVIP_REQUEST_DMA_ENTRY;
	dma->status |= RVIP_DONE_DMA_ENTRY;
	dma = dma->next;
    }

    /*
     * Okay to change dma_reqt since DMA should be inactive now
     */
    DELAY(2);
    dma_reqt = dma_free;
    vip_dmaq_consumer();
    dmac.tx_bh = NULL;
}

/*
 * rvip_manual_dma:
 * Do a short DMA by hand (mitigate for broken hw)
 */
static void rvip_manual_dma (dma_entry_t *dma_ptr, ushort status)
{
    ulong memd_addr, pci_addr, ltemp;
    ushort stemp;
    uchar ctemp;
    int i;
    
    pci_addr = (ulong)(dma_ptr->pci_addr);
    memd_addr = (ulong)dma_ptr->memd_addr | ADRSPC_CBUS;

    /*
     * Are we going from memd ?
     * If bus stall, vip_memdread32/16/8 returns FALSE
     */
    if (status & RVIP_DMA_FROM_MEMD) {

	if (!(memd_addr & 0x03)) {
	    /*
	     * Do long transfers
	     */
	    for (i=0; i<dma_ptr->length; i += 4) {
		vip_memdread32((ulong *)memd_addr, &ltemp);
		PUTLONG((ulong *)pci_addr, ltemp);
		memd_addr += 4;
		pci_addr += 4;
	    }
	} else if (!(memd_addr & 0x01)) {
	    /*
	     * Do short transfers
	     */
	    for (i=0; i<dma_ptr->length; i += 2) {
		vip_memdread16((ushort *)memd_addr, &stemp);
		PUTSHORT((ushort *)pci_addr, stemp);
		memd_addr += 2;
		pci_addr += 2;
	    }
	} else {
	    /*
	     * Do byte transfers
	     */
	    for (i=0; i<dma_ptr->length; i += 1) {
		vip_memdread8((uchar *)memd_addr, &ctemp);
		*((uchar *)(pci_addr)) = ctemp;
		memd_addr += 1;
		pci_addr += 1;
	    }
	}	    

    } else {
	
	/*
	 * We are going into memd from pci memory
	 */
	if (!(memd_addr & 0x03)) {
	    /*
	     * Do long transfers
	     */
	    for (i=0; i<dma_ptr->length; i += 4) {
		ltemp = GETLONG((ulong *)pci_addr);
		*((ulong *)memd_addr) = ltemp;
		memd_addr += 4;
		pci_addr += 4;
	    }
	} else if (!(memd_addr & 0x01)) {
	    /*
	     * Do short transfers
	     */
	    for (i=0; i<dma_ptr->length; i += 2) {
		stemp = GETSHORT((ushort *)pci_addr);
		*((ushort *)memd_addr) = stemp;
		memd_addr += 2;
		pci_addr += 2;
	    }
	} else {
	    /*
	     * Do byte transfers
	     */
	    for (i=0; i<dma_ptr->length; i += 1) {
		*((uchar *)memd_addr) = *((uchar *)pci_addr);
		memd_addr += 1;
		pci_addr += 1;
	    }
	}	    
    }

    /*
     * And clean up
     */
    dmac.dma_manual_copy_count += 1;
    dma_ptr->status = (status | (RVIP_MANUAL_DMA_ENTRY|RVIP_DONE_DMA_ENTRY));
}

/*
 * drq_io:
 * Move particle information from a paktype to dma request queue
 */
static inline boolean drq_io (paktype *pak,
			      vip_bhdr_t *bh, 
			      vip_memd_ifcntl_t *ifc,
			      volatile ulong *rq)

{
    dma_entry_t *dma_ptr, *dma_ptr_eop;
    particletype *mp;
    uchar *memd_addr;
    ushort status;
    int particle_count;

    particle_count = pak_particle_count(pak);
    if (particle_count) {

	/*
	 * Handle the case of one particle seperately for speed
	 */
	if (particle_count == 1) {

	    /*
	     * Get one request block
	     */
	    dma_ptr = dma_get_free_blocks(1);
	    if (!dma_ptr) {
		return(FALSE);
	    }
	    
	    /*
	     * Setup parameters and get the buffer header parameters
	     */
	    if (rq) {
		dma_ptr->responseq = rq;
		/* If bus stall, vip_memdread32 returns FALSE and
		 * dma_ptr->memd_addr is NULL. dma_free is not updated yet,
		 * simply return
		 */
		dma_ptr->memd_addr = vip_rsp_lookup_bhcache(bh);
		if (dma_ptr->memd_addr == NULL)
		    return(FALSE);
		status = (RVIP_SCATTERED_DMA|RVIP_REQUEST_DMA_ENTRY|
			  RVIP_DMA_EOP|RVIP_DMA_DFS);
	    } else {
		dma_ptr->memd_addr = vip_memd_buffer_start(bh, ifc);
		if (dma_ptr->memd_addr == NULL)
		    return(FALSE);
		status = (RVIP_SCATTERED_DMA|RVIP_REQUEST_DMA_ENTRY|
			  RVIP_DMA_EOP);
	    }

	    /*
	     * Take care of the ISL frames on RP/SP series
	     */
	    if (vsys_vec.rpsp_host && (pak->flags & PAK_IS_ISL)) {
		/*
		 * Adjust the start of the frame in memd
		 */
		dma_ptr->memd_addr -= ISL_ENCAPS_FRONT;
	    }

	    mp = pak->particlequeue.qhead;
	    dma_ptr->tot_length = mp->data_bytes;
 	    dma_ptr->length = mp->data_bytes;
	    dma_ptr->bhdr = bh;
	    dma_ptr->ifc = ifc;
	    dma_ptr->pci_addr = mp->data_start;
	    dma_ptr->entry_data = mp;
	    
	    /*
	     * Print out some debugging stuff
	     */
	    if (vip_debug_dma_rx) {
		uchar *md_a = 
		    (uchar *)((ulong)dma_ptr->memd_addr | ADRSPC_CBUS);
		buginf("DMA: SRqt, from PCI(%#x) to MEMD(%#x), bh=%#x, "
		       "%d bytes\n",
		       pak->datagramstart, md_a, bh, pak->datagramsize);
	    }
	    
	    /*
	     * Fill in the request blocks
	     */
	    if (dma_ptr->length <= RVIP_MIN_DMA_LENGTH) {
		/* If bus stall, vip_memdread32 returns FALSE and
		 * dma_ptr->memd_addr is NULL. dma_free is not updated yet,
		 * simply return
		 */
		rvip_manual_dma(dma_ptr, status);
		if (is_cbus_quiesced())
		    return(FALSE);
	    } else {
		/*
		 * If bus stall before this point, don't activate this DMA.
		 * Contents of write on bh fields might have failed during 
		 * bus stall. Entry in dma_reqt must have valid information.
		 */
		if (is_cbus_quiesced())
		    return(FALSE);

		dma_ptr->status = status;
	    }
	    dma_free = dma_ptr->next;

	    /*
	     * Sanitize the pak
	     */
	    pak->particlequeue.qhead = pak->particlequeue.qtail = NULL;
	    pak->particlequeue.count = 0;
	    pak->datagramstart = NULL;
	    pak->datagramsize = 0;

	    /*
	     * Possibly kick start the DMA engine and clean up any completed
	     * DMA requests.
	     */
	    if (dma_incr_request_count()) {
		vip_dmaq_consumer();
	    }

	} else {

	    /*
	     * Get enough request blocks
	     */
	    dma_ptr = dma_get_free_blocks(particle_count);
	    if (!dma_ptr) {
		return(FALSE);
	    }
	    
	    /*
	     * Setup parameters and get the buffer header parameters
	     */
	    dma_ptr_eop = dma_ptr;
	    dma_ptr->tot_length = pak->datagramsize;
	    dma_ptr->bhdr = bh;
	    dma_ptr->ifc = ifc;
	    
	    /*
	     * Fill in the request blocks
	     */
	    if (rq) {
		/* If bus stall, vip_memdread32 returns FALSE and
		 * dma_ptr->memd_addr is NULL. dma_free is not updated yet,
		 * simply return
		 */
		memd_addr = vip_rsp_lookup_bhcache(bh);
		if (memd_addr == NULL)
		    return(FALSE);
		dma_ptr->responseq = rq;
		status = (RVIP_SCATTERED_DMA|RVIP_REQUEST_DMA_ENTRY|
			  RVIP_DMA_DFS);
	    } else {
		memd_addr = vip_memd_buffer_start(bh, ifc);
		if (memd_addr == NULL)
		    return(FALSE);
		status = (RVIP_SCATTERED_DMA|RVIP_REQUEST_DMA_ENTRY);
	    }

	    /*
	     * Take care of the ISL frames on RP/SP series
	     */
	    if (vsys_vec.rpsp_host && (pak->flags & PAK_IS_ISL)) {
		/*
		 * Adjust the start of the frame in memd
		 */
		memd_addr -= ISL_ENCAPS_FRONT;
	    }

	    /*
	     * Print out some debugging stuff
	     */
	    if (vip_debug_dma_rx) {
		uchar *md_a = (uchar *)((ulong)memd_addr | ADRSPC_CBUS);
		buginf("DMA: SRqt, from PCI(%#x) to MEMD(%#x), bh=%#x, "
		       "%d bytes (%d)\n",
		       pak->datagramstart, md_a, bh, pak->datagramsize,
		       particle_count);
	    }

	    while (mp = particle_dequeue_inline(pak), mp) {
		dma_ptr->memd_addr = memd_addr;
		dma_ptr->pci_addr = mp->data_start;
		dma_ptr->entry_data = mp;
		dma_ptr->length = mp->data_bytes;
		memd_addr += dma_ptr->length;
		if (dma_ptr->length <= RVIP_MIN_DMA_LENGTH) {
		    /* If bus stall, vip_memdread32 returns FALSE and
		     * dma_ptr->memd_addr is NULL. dma_free is not updated yet,
		     * simply return
		     */
		    rvip_manual_dma(dma_ptr, status);
		    if (is_cbus_quiesced())
			return(FALSE);
		} else {
		    /*
		     * If bus stall before this point, don't activate this DMA
		     * Contents of write on bh fields might have failed during 
		     * bus stall. Entry in dma_reqt must have valid information.
		     */
		    if (is_cbus_quiesced())
			return(FALSE);

		    dma_ptr->status = status;
		}
		dma_ptr_eop = dma_ptr;
		dma_ptr = dma_ptr->next;
	    }
	    
	    /*
	     * Mark the end of the packet and enqueue the requests
	     */
	    dma_ptr_eop->status |= RVIP_DMA_EOP;
	    dma_free = dma_ptr_eop->next;

	    /*
	     * Possibly kick start the DMA engine and clean up any
	     * completed DMA requests.
	     */
	    if (dma_incr_request_count()) {
		vip_dmaq_consumer();
	    }
	}

    } else {

	dma_ptr = dma_get_free_blocks(1);
	if (!dma_ptr) {
	    return(FALSE);
	}

	/*
	 * Setup entries and get memd address from bh
	 */
	dma_ptr->tot_length = pak->datagramsize;
	dma_ptr->length = pak->datagramsize;
	dma_ptr->bhdr = bh;
	dma_ptr->ifc = ifc;

	/*
	 * Fill in the request blocks
	 */
	dma_ptr->pci_addr = pak->datagramstart;
	dma_ptr->entry_data = pak;
	if (rq) {
	    /* If bus stall, vip_memdread32 returns FALSE and
	     * dma_ptr->memd_addr is NULL. dma_free is not updated yet,
	     * simply return
	     */
	    dma_ptr->memd_addr = vip_rsp_lookup_bhcache(bh);
	    if (dma_ptr->memd_addr == NULL)
		return(FALSE);
	    dma_ptr->responseq = rq;
	    status = (RVIP_DMA_EOP|RVIP_REQUEST_DMA_ENTRY|
		      RVIP_DMA_DFS);
	} else {
	    dma_ptr->memd_addr = vip_memd_buffer_start(bh, ifc);
	    if (dma_ptr->memd_addr == NULL)
		return(FALSE);
	    status = (RVIP_DMA_EOP|RVIP_REQUEST_DMA_ENTRY);
	}

	/*
	 * Take care of the ISL frames on RP/SP series
	 */
	if (vsys_vec.rpsp_host && (pak->flags & PAK_IS_ISL)) {
	    /*
	     * Adjust the start of the frame in memd
	     */
	    dma_ptr->memd_addr -= ISL_ENCAPS_FRONT;
	}

	/*
	 * Print out some debugging stuff
	 */
	if (vip_debug_dma_rx) {
	    uchar *md_a = (uchar *)((ulong)dma_ptr->memd_addr | ADRSPC_CBUS);
	    buginf("DMA: GRqt, from PCI(%#x) to MEMD(%#x), bh=%#x, %d bytes\n",
		   pak->datagramstart, md_a, bh, pak->datagramsize);
	}

	if (dma_ptr->length <= RVIP_MIN_DMA_LENGTH) {
	    /* If bus stall, vip_memdread32 returns FALSE and
	     * dma_ptr->memd_addr is NULL. dma_free is not updated yet,
	     * simply return
	     */
	    rvip_manual_dma(dma_ptr, status);
	    if (is_cbus_quiesced())
	        return(FALSE);
	} else {
	    /*
	     * If bus stall before this point, don't activate this DMA.
	     * Contents of write on bh fields might have failed during 
	     * bus stall. Entry in dma_reqt must have valid information.
	     */
	    if (is_cbus_quiesced())
		return(FALSE);

	    dma_ptr->status = status;
	}
	dma_free = dma_ptr->next;

	/*
	 * Possibly kick start the DMA engine and cleanup
	 * any completed DMA requests.
	 */
	if (dma_incr_request_count()) {
	    vip_dmaq_consumer();
	}
    }
    return(TRUE);
}

/*
 * vip_pak_to_host:
 * Send packet to RP/RSP. Allocate buffer header, and enqueue 
 * the packet onto the DMA queue
 */
boolean vip_pak_to_host (hwidbtype *idb, pas_cbtype *pascb, paktype *pak)
{
    vip_bhdr_t *bh;
    vip_memd_ifcntl_t *ifc;

    if (is_cbus_quiesced())
	return(FALSE);

    /* 
     * Allocate MEMD buffer header for this received packet
     */
    ifc = pascb->vip_memd_ifcntl;
    bh = vip_allocate_memd_bh(pascb);
    if (bh) {
	/*
	 * Fill in relevant fields in the buffer header and queue it
	 * for dma.
	 */

	vip_rxbh_setup(ifc, bh, pak);
	return(drq_io(pak, bh, ifc, NULL));
    } else {
	return(FALSE);
    }
}

/*
 * vip_pak_switch:
 * Send packet to a specified RSP queue. This is used for distributed
 * fastswitching and RSP loveletters (both stats and IPC).
 */
boolean vip_pak_switch (vip_memd_ifcntl_t *ifc, paktype *pak, 
			vip_bhdr_t *bh, volatile ulong *responseq)
{
    if (is_cbus_quiesced()) {
	return(FALSE);
    }
    return(drq_io(pak, bh, ifc, responseq));
}

/*
 * dma_getparticle:
 * Get a particle from the DMA particle pool
 */
static inline particletype *dma_getparticle (void)
{
    particletype *particle = NULL;

    particle = pool_dequeue_cache(dmac.pool);
    if (particle) {
	particle->data_start = (uchar *)particle->data_block + 
	    PARTICLE_ENCAPBYTES;
    } else {
	dmac.dma_pool_miss += 1;
    }
    return(particle);
}

/*
 * rvip_dma_pak_from_memd: 
 * Setup DMA requests for packets from MEMD that are so large that they
 * do not fit in one particle.
 */
static inline dma_entry_t *rvip_dma_pak_from_memd (paktype *pak, 
						   int tx_length, 
						   dma_entry_t *dma_ptr)
{
    uchar *pak_data_addr;

    dma_ptr->tot_length = tx_length;
    dma_ptr->bhdr = dmac.tx_bh;
    dma_ptr->ifc = dmac.tx_ifc;
    dma_ptr->entry_data = pak;
    pak->datagramsize = tx_length;
    pak_data_addr = pak->datagramstart;

    for (;;) {
	dma_ptr->memd_addr = dmac.tx_memd_addr;
	dmac.tx_memd_addr += RVIP_MAX_DMA_LENGTH;
	dma_ptr->pci_addr = pak_data_addr;
	pak_data_addr += RVIP_MAX_DMA_LENGTH;

	if (tx_length > RVIP_MAX_DMA_LENGTH) {
	    dma_ptr->length = RVIP_MAX_DMA_LENGTH;
	    dma_ptr->status = (RVIP_DMA_FROM_MEMD|RVIP_REQUEST_DMA_ENTRY);
	    tx_length -= RVIP_MAX_DMA_LENGTH;
	    dma_ptr = dma_ptr->next;
	} else {
	    if (tx_length <= RVIP_MIN_DMA_LENGTH) {
		dma_ptr->length = RVIP_MIN_DMA_LENGTH;
	    } else {
		dma_ptr->length = tx_length;
	    }
	    dma_ptr->status = 
		(RVIP_DMA_FROM_MEMD|RVIP_DMA_EOP|RVIP_REQUEST_DMA_ENTRY);
	    return(dma_ptr);
	}
    }
}

/*
 * dma_required_blocks:
 * Return the no. of DMA request required to DMA a large packet from
 * memd given that the DMA engine has a max transfer size smaller
 * than the max packet we will use.
 */
static inline ulong dma_required_blocks (ulong len)
{
    return(((len - 1) >> RVIP_MAX_DMA_LEN_SHIFT) + 1);
}
 
/*
 * vip_poll_tx_queues:
 * poll memd tx queues looking for work. Return FALSE when we run out
 * of either work to do, or resources in which to do it.
 */
boolean vip_poll_tx_queues (void)
{
    paktype *pak;
    dma_entry_t *dma_ptr;
    particletype *mp;
    ulong tx_length, blocks;
    ushort status;
    int count = 0;

    for (;;) {

	/*
	 * Extra check in "for" loop whether VIP is in QUIESCE period
	 */
	if (is_cbus_quiesced())
	    return(FALSE);

	if (!dmac.tx_bh) {

	    /* 
	     * Get MEMD buffer header pointer and interface control
	     * data structure pointer
	     */
	    if (dmac.tx_bh = rvip_check_tx_queues(&dmac.tx_ifc), !dmac.tx_bh) {
		/*
		 * No buffer headers on enabled transmit queues.  
		 * Try again later. Note, since we did not actually move
		 * the dma free pointer, there is no recovery needed.
		 */
		if (is_cbus_quiesced())
		    return(FALSE);
		break;
	    }
	    
	    /*
	     * setup tx length and memd addr from buffer header
	     */
	    tx_length = vip_bh2txparams(dmac.tx_bh, &dmac.tx_memd_addr);

	    /*
	     * Take care of ISL frames on RP/SP series
	     */
	    if (vsys_vec.rpsp_host && 
		(dmac.tx_ifc->idb->type == IDBTYPE_FEIP)) {
		tx_length &= 0x1fff;
	    }

	    if (tx_length == 0) {
		/*
		 * If bus stall, vip_memdread32 returns FALSE and
		 * tx_length is 0.
		 * For RSP, vip_rsp_txcached_q is not setup. bh is thrown
		 * away and will be recovered after MEMD carve
		 */
		if (vsys_vec.rpsp_host) {
		    vip_rpsp_local_returnq_add(dmac.tx_bh);
		} else {
		    vip_rsp_return_queue_add(dmac.tx_ifc, dmac.tx_bh);
		}
		dmac.tx_bh = NULL;
		return(FALSE);
	    }

	} else {
	    
	    /*
	     * We had previously brought down a buffer header
	     * that we were unable to get a VIP on-board buffer
	     * for. So re-instate the state from this transaction
	     */
	    tx_length = dmac.tx_stalled_length;
	}

	/*
	 * We have a buffer in memd to come down to the VIP.
	 * If the packet fits into one DMA particle, get a particle and
	 * DMA down into that. Otherwise get a real packet buffer.
	 */
	if (tx_length <= VIP_PARTICLE_BYTES) {
	    dma_ptr = dma_get_free_blocks(1);
	    mp = dma_getparticle();
	    if (!mp || !dma_ptr) {
		/*
		 * We are not able to get a VIP onboard buffer for this
		 * MEMD buffer or we are out of dma request blocks. 
		 * So save state in dmac_cntl and break
		 */
		if (mp) {
		    retparticle(mp);
		}
		dmac.tx_stalled_length = tx_length;
		dmac.dma_tx_stall_count += 1;
		break;
	    }

	    /*
	     * Print out some debugging stuff
	     */
	    if (vip_debug_dma_tx) {
		uchar *md_a = (uchar *)((ulong)dmac.tx_memd_addr | 
					ADRSPC_CBUS);
		buginf("DMA: SRqt, from MEMD(%#x) to PCI(%#x), bh=%#x, "
		       "%d bytes\n",
		       md_a, mp->data_start, dmac.tx_bh, tx_length);
	    }

	    /*
	     * Set up dma queue with tx parameters
	     */
	    mp->data_bytes = tx_length;
	    dma_ptr->memd_addr = dmac.tx_memd_addr;
	    dma_ptr->pci_addr = mp->data_start;
	    dma_ptr->entry_data = mp;
	    dma_ptr->length = tx_length;
	    dma_ptr->tot_length = tx_length;
	    dma_ptr->bhdr = dmac.tx_bh;
	    dma_ptr->ifc = dmac.tx_ifc;
	    status = (RVIP_DMA_FROM_MEMD|RVIP_DMA_EOP|
		      RVIP_REQUEST_DMA_ENTRY|RVIP_SCATTERED_DMA);
	    if (dma_ptr->length <= RVIP_MIN_DMA_LENGTH) {
		/* If bus stall, vip_memdread32 returns FALSE and
		 * dma_ptr->memd_addr is NULL. dma_free is not updated yet,
		 * simply return
		 */
		rvip_manual_dma(dma_ptr, status);
		if (is_cbus_quiesced())
		    return(FALSE);
	    } else {
		/*
		 * If bus stall before this point, don't activate this DMA.
		 * Contents of write on bh fields might have failed during 
		 * bus stall. Entry in dma_reqt must have valid information.
		 */
		if (is_cbus_quiesced())
		    return(FALSE);

		dma_ptr->status = status;
	    }

	} else {

	    /*
	     * The MEMD packet does not fit in a DMA particle, so 
	     * try and get a full packet buffer and enough dma request blocks.
	     */
	    pak = getbuffer(tx_length);
	    blocks = dma_required_blocks(tx_length);
	    dma_ptr = dma_get_free_blocks(blocks);
	    if (!pak || !dma_ptr) {
		/*
		 * We are not able to get a VIP onboard buffer for this
		 * MEMD buffer or there is not enough dma request blocks.
		 * So save state in dmac_cntl and break
		 */
		if (pak) {
		    datagram_done(pak);
		}
		dmac.tx_stalled_length = tx_length;
		dmac.dma_tx_stall_count += 1;
		break;
	    }
	    
	    /*
	     * Print out some debugging stuff
	     */
	    if (vip_debug_dma_tx) {
		uchar *md_a = (uchar *)((ulong)dmac.tx_memd_addr | 
					ADRSPC_CBUS);
		buginf("DMA: GRqt, from MEMD(%#x) to PCI(%#x), bh=%#x, "
		       "%d bytes (%d)\n",
		       md_a, pak->datagramstart, dmac.tx_bh, tx_length, 
		       blocks);
	    }
	    
	    /*
	     * Set up dma queue with tx parameters
	     */
	    dma_ptr = rvip_dma_pak_from_memd(pak, tx_length, dma_ptr);
	}

	/*
	 * Move the free pointer and clear dmac.tx_bh
	 */
	dma_free = dma_ptr->next;
	dmac.tx_bh = NULL;

	/*
	 * Possibly kick start the DMA engine
	 */
	if (!count) {
	    dma_startup();
	}
	count += 1;
    }
    return(count);
}

/*
 * dma_interrupt_handler:
 * Handle DMA interrupts apart from "done".
 */
static void dma_interrupt_handler (void)
{
    dma_reg3_t reg3;

    /*
     * Dma interrupt - clear it
     */
    reg3 = DMA_ASIC->dma_reg3;
    if (reg3.dma_err) {
	ttyprintf(CONTTY, "\n%%DMA-1-FATAL: interrupt handler entered with an "
		  "error indication\n");
	ttyprintf(CONTTY, " reg0 = 0x%8x, reg1 = 0x%8x, reg2 = 0x%8x\n",
		  DMA_ASIC->dma_int0, DMA_ASIC->dma_int1, DMA_ASIC->dma_int2);
	ttyprintf(CONTTY, " reg3 = 0x%8x, reg4 = 0x%8x, reg5 = 0x%8x\n",
		  DMA_ASIC->dma_int3, DMA_ASIC->dma_int4, DMA_ASIC->dma_int5);
	ttyprintf(CONTTY, "\nCrashing the system ...");
        crashpoint();
    }
}

/*
 * install_dma_handler :
 * Install the DMA interrupt handler
 */
void install_dma_handler (void)
{
    int i;
    dma_entry_t *ptr;
    rvip_dma_poll_t *dma_pool;

    /*
     * Create the interrupt stack
     */
    createlevel(LEVEL_DMA, (void (*)(void))dma_interrupt_handler,
		"DMA Interrupt");

    /*
     * Create the entry memory
     */
    dmac.dmaq = malloc_fast(sizeof(dma_entry_t) * RVIP_DMA_ENTRIES);
    if (!dmac.dmaq) {
	crashdump(0);
    }

    /*
     * Chain them all together and init the pointers
     */
    dma_free = &dmac.dmaq[0];
    dma_reqt = dma_free;
    dma_done = dma_free;
    for (i=0; i<RVIP_DMA_ENTRIES; i++) {
	ptr = &dmac.dmaq[i];
	if (i == (RVIP_DMA_ENTRIES-1)) {
	    ptr->next = &dmac.dmaq[0];
	} else {
	    ptr->next = &(dmac.dmaq[i+1]);
	}
	ptr->status = RVIP_FREE_DMA_ENTRY;
    }

    /*
     * Setup the poll pool
     */
    queue_init(&dmac.rvip_dma_poll_poolq, 0);
    queue_init(&dmac.rvip_dma_runq, 0);
    dma_pool = malloc_fast(sizeof(rvip_dma_poll_t) * RVIP_DMA_POLL_POOL);
    if (!dma_pool) {
	crashdump(0);
    }
    for (i=0; i<RVIP_DMA_POLL_POOL; i++) {
	dma_pool[i].next = NULL;
	p_enqueue(&dmac.rvip_dma_poll_poolq, &dma_pool[i]);
    }
    dmac.rvip_poll_ptr = NULL;

    /* 
     * Initialize nevada to interrupt when enabled at 100us 
     */
    IO_ASIC->ct2 = RVIP_DMA_USEC_TIME;
    IO_ASIC->ccr = 0;
    dma_working = FALSE;
}

/*
 * rvip_init_dma_pools:
 * Initialize buffer pools used by the DMA driver
 */
void rvip_init_dma_pools (void)
{
    mempool *dma_buffer_mempool;

    /*
     * Init a pool of paktype headers
     */
    dmac.headerpool = pas_interface_header_pool(NULL);
    if (!dmac.headerpool) {
	crashdump(0);
    }

    /*
     * Setup a dma paktype.
     */
    dmac.dma_pak = pas_allocate_fspak();
    if (!dmac.dma_pak) {
	crashdump(0);
    }
    rvip_dma_flush_pak(dmac.dma_pak);

    /*
     * Init the DMA buffer pool
     */
    dma_buffer_mempool = vip_dma_buffer_mempool();

    dmac.pool_group = pool_create_group();
    dmac.pool = particle_pool_create("DMA",
				     dmac.pool_group,
				     VIP_PARTICLE_BYTES, 
				     POOL_SANITY,
				     32,
				     dma_buffer_mempool);

    if (!dmac.pool) {
	crashdump(0);
    }
    pool_adjust(dmac.pool, 0, RVIP_DMA_BUFFERS, RVIP_DMA_BUFFERS, TRUE);
    particle_pool_create_cache(dmac.pool, RVIP_DMA_BUFFERS);
    pool_adjust_cache(dmac.pool, RVIP_DMA_BUFFERS);
    dmac.pool->fallback = pas_interface_fallback_pool(NULL);
}

/*
 * rvip_gathered_request_complete:
 * A contigous DMA transfer was completed.
 */
static inline void rvip_gathered_request_complete (vip_memd_ifcntl_t *ifc,
						   dma_entry_t *dma_ptr,
						   ushort status)
{
    hwidbtype *idb;

    if (!(status & RVIP_DMA_FROM_MEMD)) {
	/*
	 * Something was moved into memd in a contigous packet
	 */
	datagram_done(dma_ptr->entry_data);

	if (is_cbus_quiesced())
	    return;

	if (vsys_vec.rpsp_host) {
	    vip_rpsp_local_raw_queue_add(dma_ptr->bhdr, 
					 dma_ptr->tot_length);
	} else {
	    if (!(status & RVIP_DMA_DFS)) {
		/*
		 * It was not a "switched" packet. So enqueue it on
		 * the raw queue
		 */
		vip_rsp_qa_enqueue(vsys_vec.rsp_global_raw_q, dma_ptr->bhdr);
	    } else {
		/*
		 * It was a "switched" packet (DFS, IPC, LL). So put it on
		 * the dma_ptr->responseq
		 */
		vip_rsp_qa_enqueue(dma_ptr->responseq, dma_ptr->bhdr);
	    }
	}
	
    } else if (!is_cbus_quiesced()) {
	/*
	 * Something was moved from memd in a contigous packet
	 */
	idb = ifc->idb;
	if (vsys_vec.rpsp_host) {
	    vip_rpsp_local_returnq_add(dma_ptr->bhdr);
	} else {
	    vip_rsp_return_queue_add(ifc, dma_ptr->bhdr);
	}
	(*idb->fastsend)(idb, dma_ptr->entry_data);
    }
}

/*
 * rvip_scattered_request_complete:
 * A scattered DMA request was completed
 */
static inline void rvip_scattered_request_complete (vip_memd_ifcntl_t *ifc,
						    dma_entry_t *dma_ptr,
						    ushort status)
{
    dma_entry_t *ptr;
    hwidbtype *idb;

    if (!(status & RVIP_DMA_FROM_MEMD)) {
	/*
	 * Something was moved into memd in particles
	 */
	for (ptr=dma_ptr; ptr != dma_done; ptr=ptr->next) {
	    retparticle(ptr->entry_data);
	}

	if (is_cbus_quiesced())
	    return;

	if (vsys_vec.rpsp_host) {
	    vip_rpsp_local_raw_queue_add(dma_ptr->bhdr, 
					 dma_ptr->tot_length);
	} else {
	    if (!(status & RVIP_DMA_DFS)) {
		/*
		 * It was not a "switched" packet. So enqueue it on
		 * the raw queue
		 */
		vip_rsp_qa_enqueue(vsys_vec.rsp_global_raw_q, dma_ptr->bhdr);
	    } else {
		/*
		 * It was a "switched" packet (DFS, IPC, LL). So put it on
		 * the dma_ptr->responseq
		 */
		vip_rsp_qa_enqueue(dma_ptr->responseq, dma_ptr->bhdr);
	    }
	}

    } else {
	/*
	 * Something was moved from memd in particles
	 */
	idb = ifc->idb;
	for (ptr=dma_ptr; ptr != dma_done; ptr=ptr->next) {
	    particle_enqueue_inline(dmac.dma_pak, ptr->entry_data);
	}
	if (!is_cbus_quiesced()) {
	    if (vsys_vec.rpsp_host) {
		vip_rpsp_local_returnq_add(dma_ptr->bhdr);
	    } else {
		vip_rsp_return_queue_add(ifc, dma_ptr->bhdr);
	    }
	    (*idb->fastsend)(idb, dmac.dma_pak);
	}
	rvip_dma_flush_pak(dmac.dma_pak);
    }
}

/*
 * vip_dmaq_consumer :
 * Perform post-processing on completed dma's.
 */
void vip_dmaq_consumer (void)
{
    dma_entry_t *dma_ptr;
    vip_memd_ifcntl_t *ifc;
    ushort status;

    /*
     * Make sure that any requested DMA transfers get
     * moved to the DMA engine
     */
    dma_startup();
    dmac.reqt_count = 0;

    for (;;) {

	dma_ptr = dma_get_done_blocks();
	if (!dma_ptr) {
	    break;
	}

	ifc = dma_ptr->ifc;
	status = dma_ptr->status;
	if (!(status & RVIP_SCATTERED_DMA)) {
	    rvip_gathered_request_complete(ifc, dma_ptr, status);
	} else {
	    rvip_scattered_request_complete(ifc, dma_ptr, status);
	}
	dma_mark_free(dma_ptr);
    }

    /*
     * Sync local rx and tx queues. RP/SP hosts only.
     */
    if (vsys_vec.rpsp_host && !is_cbus_quiesced()) {
	vip_rpsp_local_txq_sync();
	vip_rpsp_local_rxq_sync();
    }
}

/*
 * vip_enable_tx_polling:
 * Enable polling on a tx interface
 */
boolean vip_enable_tx_polling (pas_cbtype *pascb,
			       volatile ushort *poll_q,
			       int poll_time)
{
    leveltype level;
    rvip_dma_poll_t *dma;
    vip_memd_ifcntl_t *ifc;
    int i;

    /*
     * Raise interrupts and make sure that this guy is not
     * enabled already
     */
    ifc = pascb->vip_memd_ifcntl;
    level = raise_interrupt_level(ALL_DISABLE);
    dma = dmac.rvip_dma_runq.qhead;
    for (i=0; i<dmac.rvip_dma_runq.count; i++) {
	/*
	 * Has this been queued before ?
	 */
	if (dma->ifc == ifc) {
	    reset_interrupt_level(level);
	    return(FALSE);
	}
	dma = dma->next;
    }
	
    /*
     * This one has not been queued before. Get a new entry
     */
    dma = p_dequeue(&dmac.rvip_dma_poll_poolq);
    if (!dma) {
	reset_interrupt_level(level);
	return(FALSE);				/* should never happen */
    }

    /*
     * Initialize it and queue it
     */
    dma->ifc = ifc;
    USEC_GET_TIMESTAMP(dma->usec_time);
    if (poll_time) {
	dma->poll_usec_interval = poll_time;
    } else {
	dma->poll_usec_interval = 100;
    }
    dma->last_poll_failed = FALSE;

    if (vsys_vec.rpsp_host) {
	/*
	 * RP/SP host
	 */
	if (poll_q) {
	    dma->icb_tx_q = poll_q;
	} else {
	    dma->icb_tx_q = &(ifc->icb_ptr->icb_rpsp_txque);
	}
    }

    p_enqueue(&dmac.rvip_dma_runq, dma);
    dmac.rvip_poll_ptr = dmac.rvip_dma_runq.qhead;
    reset_interrupt_level(level);
    return(TRUE);
}

/*
 * vip_disable_tx_polling:
 * Enable polling on a tx interface
 */
boolean vip_disable_tx_polling (pas_cbtype *pascb)
{
    leveltype level;
    rvip_dma_poll_t *dma;
    vip_memd_ifcntl_t *ifc;
    int i;

    /*
     * Raise interrupts and make sure that this guy is enabled
     */
    ifc = pascb->vip_memd_ifcntl;
    level = raise_interrupt_level(ALL_DISABLE);
    dma = dmac.rvip_dma_runq.qhead;
    for (i=0; i<dmac.rvip_dma_runq.count; i++) {
	/*
	 * Is this the guy ?
	 */
	if (dma->ifc == ifc) {
	    break;
	}
	dma = dma->next;
    }

    /*
     * If we found it, take it off the runq and put it back onto
     * the poll_poolq, and set the poll ptr to runq.qhead.
     */
    if (!dma) {
	reset_interrupt_level(level);
	return(FALSE);
    }
    p_unqueue(&dmac.rvip_dma_runq, dma);
    p_enqueue(&dmac.rvip_dma_poll_poolq, dma);
    dmac.rvip_poll_ptr = dmac.rvip_dma_runq.qhead;

    reset_interrupt_level(level);
    return(TRUE);
}

/*
 * vip_show_dma:
 * Display info about the DMA engine
 */
void vip_show_dma (parseinfo *csb)
{
    leveltype level;
    int free_count, req_count, done_count, i, count;
    rvip_dma_poll_t *ppool, *ptr;

    /*
     * Print out all the pointers and status
     */
    printf("VIP DMA Engine status:\n");
    printf(" dma_working=%d, dma_timer_check=%d, dma_start_count=%d\n",
	   (int)dma_working, dma_timer_check, dmac.dma_start_count);
    printf(" dma_manual_copy_count=%d, rvip_poll_count=%d, reqt_count=%d\n",
	   dmac.dma_manual_copy_count, dmac.rvip_dma_runq.count, 
	   dmac.reqt_count);
    printf(" pool=%#x, headerpool=%#x, cache_end=%d, miss=%d, hits=%d\n",
	   dmac.pool, dmac.headerpool, dmac.pool->cache_end,
	   dmac.pool->miss, dmac.pool->hit);
    printf(" dma_pak=%#x, dmaq=%#x, dma_free=%#x, dma_reqt=%#x\n",
	   dmac.dma_pak, dmac.dmaq, dma_free, dma_reqt);
    printf(" dma_done=%#x, dma_runq.qhead=%#x, dma_runq.qtail=%#x\n",
	   dma_done, dmac.rvip_dma_runq.qhead, dmac.rvip_dma_runq.qtail);
    printf(" tx_stall_count=%d, dma_pool_miss=%d, rvip_poll_ptr=%#x\n", 
	   dmac.dma_tx_stall_count, dmac.dma_pool_miss, dmac.rvip_poll_ptr);
    printf(" flushed_reqt_count=%d\n", dmac.flushed_reqt_count);

    /*
     * Count up the elements on each queue
     */
    level = raise_interrupt_level(LEVEL_DMA);
    for (free_count=0, i=0; i<RVIP_DMA_ENTRIES; i++) {
	if (dmac.dmaq[i].status & RVIP_FREE_DMA_ENTRY) {
	    free_count += 1;
	}
    }
    for (req_count=0, i=0; i<RVIP_DMA_ENTRIES; i++) {
	if (dmac.dmaq[i].status & RVIP_REQUEST_DMA_ENTRY) {
	    req_count += 1;
	}
    }
    for (done_count=0, i=0; i<RVIP_DMA_ENTRIES; i++) {
	if (dmac.dmaq[i].status & RVIP_DONE_DMA_ENTRY) {
	    done_count += 1;
	}
    }
    reset_interrupt_level(level);
    printf(" free count=%d, request count=%d, done count=%d\n",
	   free_count, req_count, done_count);

    /*
     * Take a snap shot of the polling data structures and print them out
     */
    ppool = malloc(sizeof(rvip_dma_poll_t) * RVIP_DMA_POLL_POOL);
    if (!ppool) {
	return;
    }

    level = raise_interrupt_level(ALL_DISABLE);
    count = dmac.rvip_dma_runq.count;
    ptr = dmac.rvip_dma_runq.qhead;
    for (i=0; i<count; i++) {
	bcopy(ptr, &(ppool[i]), sizeof(rvip_dma_poll_t));
	ptr = ptr->next;
    }	
    reset_interrupt_level(level);

    if (!count) {
	free(ppool);
	printf("\nNo elements exist on the tx polling queue\n");
	return;
    }

    /*
     * Print out the entries
     */
    for (i=0; i<count; i++) {
	vip_memd_ifcntl_t *ifc;

	ptr = &(ppool[i]);
	ifc = ptr->ifc;
	if (!ifc) {
	    printf("\nInterface %d: ifc is Null\n", i);
	    continue;
	}
	printf("\nInterface %s: Tx polling enabled\n",
	       ifc->idb->hw_namestring);
	printf(" ifc=%#x, icb_tx_q=%#x, rsp_tx_q=%#x, next=%#x\n",
	       ifc, ptr->icb_tx_q, ifc->rsp_tx_q, ptr->next);
	printf(" rsp_local_free_q=%#x, rsp_tx_acc=%#x\n",
	       ifc->rsp_local_free_q, ifc->rsp_tx_acc);
	printf(" usec_time=%#x, poll_usec_interval=%d, last_poll_failed=%s\n",
	       ptr->usec_time, ptr->poll_usec_interval,
	       (ptr->last_poll_failed ? "Yes" : "No"));
    }
    
    /*
     * Cleanup
     */
    free(ppool);
}

/*
 * vip_show_dtq:
 * Stub on checkers.
 */
void vip_show_dtq (parseinfo *csb)
{
}

/*
 * vip_show_drq:
 * Stub on checkers
 */
void vip_show_drq (parseinfo *csb)
{
}

/* end of file */
