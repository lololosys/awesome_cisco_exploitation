/* $Id: svip_dma.c,v 3.1.62.14 1996/08/28 13:15:05 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-svip/svip_dma.c,v $
 *------------------------------------------------------------------
 * svip_dma.c - DMA Engine support for Viper
 *
 * October 1995, David Getchell
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_dma.c,v $
 * Revision 3.1.62.14  1996/08/28  13:15:05  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.62.13  1996/06/26  03:21:58  ondiaye
 * CSCdi50724:  A microcode reload caused vip2 reset lots of times when 2
 * vip2 inbox
 * Branch: California_branch
 *
 * Revision 3.1.62.12  1996/06/04  05:05:17  kao
 * CSCdi57759:  VIP2 system exception & DBUS-3-DBUSINTERR error when doing
 * eoir.
 * always put bracket around for loop even if there is only one statement
 * Branch: California_branch
 *
 * Revision 3.1.62.11  1996/06/01  16:02:37  kao
 * CSCdi57759:  VIP2 system exception & DBUS-3-DBUSINTERR error when doing
 * eoir.
 * Flush DFS/IPC message before memd recarve to have a consistent DFS
 * cache after EOIR. Also clean up pending post read registers during
 * QUIESCE.
 * Branch: California_branch
 *
 * Revision 3.1.62.10  1996/05/03  02:26:53  ondiaye
 * CSCdi56171:  need to add major and minor ver to qe code
 * Branch: California_branch
 *
 * Revision 3.1.62.9  1996/04/30  21:26:05  getchell
 * CSCdi55918:  VIP2 lose its connectivity when 16 Ether transmitting at
 * wire rate.
 * Branch: California_branch
 *
 * Revision 3.1.62.8  1996/04/20  17:32:12  getchell
 * CSCdi54568:  FE/VIP2 perf. is severely impacted by presence of second
 * FE.
 * Branch: California_branch
 *
 * Revision 3.1.62.7  1996/04/20  15:08:03  getchell
 * CSCdi54520:  FE/VIP2 stops forwarding traffic @ > 147 Kpps
 * Branch: California_branch
 *
 * Revision 3.1.62.6  1996/04/12  23:04:54  kao
 * CSCdi53527:  Need EOIR support on VIP2 (remove lines brought in during
 * "cvs update")
 * Branch: California_branch
 *
 * Revision 3.1.62.5  1996/04/12  17:44:13  kao
 * CSCdi53527:  Need EOIR support on VIP2
 * Branch: California_branch
 *
 * Revision 3.1.62.4  1996/04/08  13:45:01  getchell
 * CSCdi52361:  DFS broken using unlike media
 * Branch: California_branch
 *
 * Revision 3.1.62.3  1996/04/08  05:38:48  getchell
 * CSCdi52876:  corrupt sw2idb pointer
 * Branch: California_branch
 *
 * Revision 3.1.62.2  1996/04/06  21:56:28  ondiaye
 * CSCdi52394:  svip support for merged qe codes header file
 *              Also takes care of CSCdi52731 and the DRQ dealock.
 * Branch: California_branch
 *
 * Revision 3.1.62.1  1996/03/21  23:51:27  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.10.5  1996/03/07  17:29:42  getchell
 * CSCdi50953:  viper dfs mods
 * Branch: ELC_branch
 *
 * Revision 3.1.10.4  1996/02/21  20:37:20  getchell
 * CSCdi49474:  Time to turn on IPC/DFS on viper
 * Do some cleanup while at it...
 * Branch: ELC_branch
 *
 * Revision 3.1.10.3  1996/02/16  22:33:56  getchell
 * CSCdi49237:  svip kernel and asm files out of whack
 * cache invalidate before reading instead of after
 * add some accounting useful for debugging
 * change function definitions for drq/dtq consumer (no longer static)
 * Branch: ELC_branch
 *
 * Revision 3.1.10.2  1996/02/12  23:33:02  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch. Fix support for
 * receiving single particle packets into memd.
 * Branch: ELC_branch
 *
 * Revision 3.1.10.1  1996/02/10  00:22:01  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/12/26  21:18:38  getchell
 * Placeholder files for Viper development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "types.h"
#include "interface_private.h"
#include "packet.h"
#include "sched.h"
#include "stacks.h"
#include "ttysrv.h"
#include "logger.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"

#include "../pas/if_pas.h"
#include "../hes/if_fci.h"
#include "../src-vip/vip_dma.h"
#include "../src-vip/vip_memd.h"
#include "../src-vip/vip_debug.h"

#include "svip_msg_dma.c"		/* Not a typo, see logger.h */
#include "svip_dma.h"
#include "svip_memd.h"

#include "../firmware/vip2_qe_image.h" 

/*
 * Local storage
 * These local variables are stand alone variables (rather than members
 * of a structure) for performance reasons (gp addressing)
 */

static paktype       *dmac_pak;

static pooltype      *dmac_pool;
static pooltype      *dmac_headerpool;
static int	      dmac_pool_group;
static int            dmac_drq_full;
static int            dmac_no_memd_buff;
static int            txq_index;
static char           *cya_sw_code;

/*
 * Queueing engine queues
*/
static drq_entry_t   *dmac_drq;
static ulong          dmac_drq_read;
static ulong          dmac_drq_write;
static particletype **dmac_drq_shdw;
static ulong          dmac_drq_avail;
static ulong          dmac_drq_pkt_count;

static dtq_entry_t   *dmac_dtq;
static ulong          dmac_dtq_read;
static ulong          dmac_dtq_write;
static particletype **dmac_dtq_shdw;
static ulong          dmac_dtq_pkt_count;

static ushort         dmac_qe_status_shdw;

static pas_cbtype    *dmac_polling_suspended_queue_head;
 
static hwidbtype     *dmac_sw2idb[MAX_TX_QUEUES];

static ulong          dmac_cache_end;
static void          *dmac_cache[SVIP_DMA_BUFFERS];


ulong cya_diag_opt_shdw = QE_JS_WAIT_DISABLE_BIT;

/*
 * svip_dma_flush_pak:
 * Cleanup the temporary DMA paktype
 */
static inline void svip_dma_flush_pak (paktype *pak)
{
    pak->datagramsize = 0;
}

/*
 * dtq_getparticle:
 * Get a particle from the DTQ particle pool
 */
static inline particletype *dtq_getparticle (void)
{
    particletype *particle = NULL;

    if (dmac_cache_end) {
	particle = dmac_cache[--dmac_cache_end];
    }
    return(particle);
}

particletype *dma_getparticle(void)
{
     return (dtq_getparticle());
}

/*
 * vip_drq_consumer:
 * Perform post-processing on packets dma'd from the viper
 * Only the transfer headers will have the Own bit cleared
 */
void vip_drq_consumer (void)
{
    ulong word0;
    ulong particle;

    DAS_DEBUG(0x3333000b);

    while (dmac_drq_read != dmac_drq_write) {

	word0 = dmac_drq[dmac_drq_read].drq_word0; 
	if (word0 & DRQ_DMA_OWN) {
	    /*
	     * All finished 
	     */
	    break;
	}

	/* Check to make sure entry is a transfer header */
	if (word0 & DRQ_DMA_TRANSFER) {
	    
	    for(;;) {

		dmac_drq_read = DRQ_ADVANCE_RING_INDEX(dmac_drq_read);
		dmac_drq_avail += 1;

		particle = (ulong)dmac_drq_shdw[dmac_drq_read];
		
		retparticle_inline((particletype *)(particle & 0xfffffffe));

		if (particle & 1) {
		    /* EOP indicated */
		    dmac_drq_read = DRQ_ADVANCE_RING_INDEX(dmac_drq_read);
		    dmac_drq_avail += 1;
		    break;
		}
	    }
	} /* FIXME add errmsg clause */

    }
}


/*
 * dtq_repopulate:
 * This routine is installed in the DTQ pool item.ret vector.  It takes a
 * "dirty" particle, cleans it up a bit, and adds it to DTQ
 */
static void dtq_repopulate (pooltype *pool, void *item)
{
    dtq_entry_t dtq_entry;
    particletype *particle = item;
    ulong new_index;

    particle->data_start = (char *)particle->data_block;

    new_index = DTQ_ADVANCE_RING_INDEX(dmac_dtq_write);

    DAS_DEBUG(0x3333000e);

    new_index = DTQ_ADVANCE_RING_INDEX(dmac_dtq_write);
    if (new_index == dmac_dtq_read) {
	/*
	 * DTQ is full, just add particle to cache
	 */
	dmac_cache[dmac_cache_end++] = item;
    } else {
	/*
	 * Add particle to DTQ directly
	 */

	/* Queueing engine does not mind virtual address */
	dtq_entry.dtq_word1 = (ulong)particle->data_start;
	dtq_entry.dtq_word0 = DTQ_DMA_OWN | VIP_PARTICLE_BYTES;
	move64((u64 *)&dtq_entry, (u64 *)&dmac_dtq[dmac_dtq_write]);

	dmac_dtq_shdw[dmac_dtq_write] = particle;

	dmac_dtq_write = new_index;
    }

}

/* 
 * dtq_populate:
 * Populate the dtq with particles
 */
static void dtq_populate (void)
{
    ulong qwrite;
    particletype *particle_ptr;
    dtq_entry_t dtq_entry;
    
    DAS_DEBUG(0x33330001);

    qwrite = DTQ_ADVANCE_RING_INDEX(dmac_dtq_write);

    while (qwrite != dmac_dtq_read) {

	particle_ptr = dtq_getparticle();

	if (!particle_ptr) {
	    return;
	}

	/* Queueing engine does not mind virtual address */
	dtq_entry.dtq_word1 = (ulong)particle_ptr->data_start;
	dtq_entry.dtq_word0 = DTQ_DMA_OWN | VIP_PARTICLE_BYTES;
	move64((u64 *)&dtq_entry, (u64 *)&dmac_dtq[dmac_dtq_write]);
	dmac_dtq_shdw[dmac_dtq_write] = particle_ptr;

	dmac_dtq_write = qwrite;
	qwrite = DTQ_ADVANCE_RING_INDEX(dmac_dtq_write);
    }
}

/* 
 * vip_dtq_consumer:
 * Perform post-processing on packets dma'd to the viper
 */
void vip_dtq_consumer (void)
{
    hwidbtype *idb;
    uchar ifc;
    ulong dtqword0;
    particletype *particle_ptr;
    pas_cbtype *pascb, *prev_pascb=NULL;
    boolean cache_line_invalid = TRUE;
    vip_memd_ifcntl_t *ifcntl;
    int loopcount;

    DAS_DEBUG(0x33330002);

    for (loopcount = 0; 
	 (dmac_dtq_read != dmac_dtq_write) && (loopcount < DTQ_SIZE);
	 loopcount++) {

	if (cache_line_invalid) {
	    /*
	     * Each dtq entry is 8 bytes - invalidate whenever index wraps
	     * past 4 entries
	     */
	    hit_invalidate_cache_line_inline(&dmac_dtq[dmac_dtq_read]);
	    cache_line_invalid = FALSE;
	}

	dtqword0 = dmac_dtq[dmac_dtq_read].dtq_word0;

	if (dtqword0 & DTQ_DMA_OWN) {
	    /*
	     * All finished 
	     * Add some buffers to dtq to replace one's just removed
             */
	    if (loopcount) {
		dtq_populate();
	    }
	    break;
	}

	particle_ptr = dmac_dtq_shdw[dmac_dtq_read];

	/*
	 * Queueing engine done with request.  Update the entries particle
	 * information
	 */
	particle_ptr->data_bytes = dtqword0 & DTQ_LENGTH_MASK;;
	particle_enqueue_inline(dmac_pak, particle_ptr);

	if (dtqword0 & DRQ_DMA_EOP) {
	    /*
	     * Last particle in packet.  Get interface number from
	     * queueing engine entry
	     */
	    ifc = (dtqword0 & DTQ_INTFC_MASK) >> DTQ_INTFC_SHIFT;

	    idb = dmac_sw2idb[ifc];

	    pascb = (pas_cbtype *)INSTANCE(idb);

	    if (*pascb->txring_cnt_ptr >= pascb->tx_polling_high) {
		DAS_DEBUG(0x3333000c);
		vip_suspend_tx_polling(pascb);
	    }

	    STATISTICS(dmac_dtq_pkt_count++);

	    DAS_DEBUG(0x33330055);
	    (*idb->fastsend)(idb, dmac_pak);

	    svip_dma_flush_pak(dmac_pak);
	}

	dmac_dtq_read = DTQ_ADVANCE_RING_INDEX(dmac_dtq_read);
	
	if(!(dmac_dtq_read & 0x3)){
	    cache_line_invalid = TRUE;
	}
    }

    /* See if any polling needs to be re-enabled */
    for (pascb = dmac_polling_suspended_queue_head; 
	 pascb != NULL; 
	 pascb = pascb->next) {

	if ((*pascb->txring_cnt_ptr) < pascb->tx_polling_low) {
	    /* 
	     * Set memd_poll_disable_queued to false so that 
	     * vip_enable_tx will not remove it from list, we can do it faster
             * here.
	     */
	    ifcntl = pascb->vip_memd_ifcntl;
	    ifcntl->memd_poll_suspend_queued = FALSE;
	    vip_enable_tx_polling(pascb, 0, 0);
	    if (pascb == dmac_polling_suspended_queue_head) {
		dmac_polling_suspended_queue_head = pascb->next;
	    } else {
		prev_pascb->next = pascb->next;
	    }
	} else {
	    prev_pascb = pascb;
	}
    }
}

void vip_dmaq_consumer (void)
{
    vip_dtq_consumer();
    vip_drq_consumer();
}

boolean vip_poll_tx_queues (void)
{
    return(FALSE);
}

/*
 * drq_io:
 * Move particle information from a paktype to dma request queue
 */
static inline boolean drq_io (paktype *pak,
			      vip_bhdr_t *bh, 
			      vip_memd_ifcntl_t *ifc,  /* FIXME rpsp needs? */
			      ushort   memd_offset,
			      volatile ulong *rq)

{
    drq_entry_t drq_entry;
    particletype *particle_ptr;
    int particle_count, drq_alloc;
    int loopcount;

    DAS_DEBUG(0x3333000d);

    particle_count = pak_particle_count(pak);

    /* Allocate an extra space for drq receive header */
    drq_alloc = particle_count + 1;

    if (drq_alloc > dmac_drq_avail) {

	loopcount = 0;

	do {
	    vip_drq_consumer();

	    if (drq_alloc <=  dmac_drq_avail) {
		break;
	    }
	
	    /* 
	     * Since we're busy, see if anything to go on dtq
	     */
	    vip_dtq_consumer();

	    if (loopcount++ > 10000) {
                vip_rsp_qa_enqueue(ifc->rsp_local_free_q, bh);
                dmac_drq_full++;
                return(FALSE);
            }

	} while (drq_alloc > dmac_drq_avail);

    }

    /* decrement the number of available drq entries */
    dmac_drq_avail -= (drq_alloc);

    /* Set up the transfer format entry */
	
    drq_entry.drq_word1 = ((ulong)(rq) << DRQ_QUEUE_SHIFT) |
	                       memd_offset;

    drq_entry.drq_word0 = DRQ_DMA_OWN | DRQ_DMA_TRANSFER |
	                       RSP_VIRTUAL2MEMD(bh);

    move64((u64*)&drq_entry, (u64*)&dmac_drq[dmac_drq_write]);

    dmac_drq_write = DRQ_ADVANCE_RING_INDEX(dmac_drq_write);

    STATISTICS(dmac_drq_pkt_count++);

    /*
     * Print out some debugging stuff
     */
    if (SVIP_DEBUG_SUPPORT && vip_debug_dma_rx) {
	buginf("DRQ: Transfer: Queue(%#x), Offset(%#x), bh (%#x) \n",
		       rq, memd_offset, bh);
    }


    if (particle_count == 1) {

	particle_ptr = particle_dequeue_inline(pak);

	drq_entry.drq_word1 = (ulong)particle_ptr->data_start;

	drq_entry.drq_word0 = DRQ_DMA_OWN | DRQ_DMA_EOP |
	                          particle_ptr->data_bytes;

	move64((u64*)&drq_entry, (u64*)&dmac_drq[dmac_drq_write]);

	/*
	 * Print out some debugging stuff
         */
	if (SVIP_DEBUG_SUPPORT && vip_debug_dma_rx) {
	    buginf("DRQ: length (%#x), pci addr (%#x) \n",
		       particle_ptr->data_bytes, particle_ptr->data_start);

	}

	/* Increment particle ptr to indicate EOP */
	particle_ptr = (particletype *)((ulong)particle_ptr + 1);
	dmac_drq_shdw[dmac_drq_write] = particle_ptr;

	dmac_drq_write = DRQ_ADVANCE_RING_INDEX(dmac_drq_write);

    } else {

	while (particle_count--) {

	    particle_ptr = particle_dequeue_inline(pak);

	    drq_entry.drq_word1 = (ulong)particle_ptr->data_start;

	    if (particle_count) {
		drq_entry.drq_word0 = DRQ_DMA_OWN | 
		                                particle_ptr->data_bytes;
	    } else {
		drq_entry.drq_word0 = DRQ_DMA_OWN | DRQ_DMA_EOP |
		                                particle_ptr->data_bytes;
		particle_ptr = (particletype *)((ulong)particle_ptr + 1);
	    }

	    move64((u64*)&drq_entry, (u64*)&dmac_drq[dmac_drq_write]);

	    dmac_drq_shdw[dmac_drq_write] = particle_ptr;

	    dmac_drq_write = DRQ_ADVANCE_RING_INDEX(dmac_drq_write);

	    /*
	     * Print out some debugging stuff
	     */
	    if (SVIP_DEBUG_SUPPORT && vip_debug_dma_rx) {
		buginf("DRQ: length (%#x), pci addr (%#x) \n",
		       pak->datagramsize, particle_ptr->data_start);
	    }
	}
	    
    }  /* if particle_count... */

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

	if (!RPSP_SELECT) {
	    return(drq_io(pak, bh, ifc, ifc->rx_offset, vsys_vec.rsp_global_raw_q));
	} else {
	    return(FALSE);
	}
	
    } else {
	STATISTICS(dmac_no_memd_buff++);
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
    return(drq_io(pak, bh, ifc, 0, responseq));
}

/*
 * dma_interrupt_handler:
 * Handle DMA interrupts apart from "done".
 */
void dma_interrupt_handler (void)
{

    /* FIXME - needs a little work ;-) */

    ttyprintf(CONTTY, "\nCrashing the system ...");
    crashpoint();

    /*
     * EOIR support. Some thing needs to be done here to clean
     * up the request queue when the DMA engine gets bitten
     * by a cbus stall event from an EOIR condition.
     */
}

/*
 * cya_download:
 * Copy the microcode array into the qe program memory. 
 */
static boolean cya_download (void)
{                         
    int i;
    double_word_t *xmem_start;
    ulong *cya_code;
    ulong *cya_data;

    if(RPSP_SELECT) {
	return (FALSE);          /* cxbus support not yet implemented */
    } else {

        get_pa_conf_type();         /* set pa_conf_type in vsys_vec  */	
	if (vsys_vec.pa_conf_type == PA_FAST) {
	    cya_code = cya_2tx_code;
	    cya_data = cya_2tx_data;
	    cya_sw_code = "2tx";
	} else {
	    cya_code = cya_16tx_code;
	    cya_data = cya_16tx_data;
	    cya_sw_code = "16tx";
	} 	 

	for (i=0; i<CYA_CODE_SIZE; i++) {
	    /* 
	     * this is because qe space is 8byte registers
	     * from orion's view.
	     */
	    CYA_ASIC->qe_pgm_mem[(i<<1)+1]= cya_code[i]; 
	}
	for (i=0; i<CYA_CODE_SIZE; i++) {
	    if(CYA_ASIC->qe_pgm_mem[(i<<1)+1] != cya_code[i]) 
		return (FALSE);
	}
	
	xmem_start= (double_word_t *) (&(CYA_ASIC->qe.xmem));
	for (i=0; i<CYA_DATA_SIZE; i++) {
	    xmem_start->reg= cya_data[i]; 
	    xmem_start++;
	}
	for (i=0; i<CYA_DATA_SIZE; i++) {
	    if (xmem_start->reg != cya_data[i])
		return(FALSE);
	    xmem_start++;
	}
	return (TRUE);    
    }
}

/*
 * intialize_qe:
 * Initialize the Queueing Engine including downloading of code
 */
static void initialize_qe (void)
{

    /* Stop the queueing engine running */
    stop_qe();

    /* Download code into the queueing engine */
    cya_download();

    /* Initialize the CYA queueing engine */
    CYA_ASIC->qe.ymem.dtq_ptr = VIPER2QE_PCIADDR(dmac_dtq);
    CYA_ASIC->qe.ymem.dtq_len = DTQ_SIZE * sizeof(dtq_entry_t);;
    CYA_ASIC->qe.ymem.dtq_bfr_size = VIP_PARTICLE_BYTES;
    CYA_ASIC->qe.xmem.drq_ptr = VIPER2QE_PCIADDR(dmac_drq);
    CYA_ASIC->qe.xmem.drq_len = DRQ_SIZE * sizeof(drq_entry_t);

    /* Start the queueing engine running */
    start_qe();
    set_qe_opt(QE_DEBUG_PC_BIT);
}

/*
 * install_dma_handler:
 * Install the DMA interrupt handler
 */
void install_dma_handler (void)
{

    DAS_DEBUG(0x33330003);

    /* 
     * Note that this createlevel is done here, instead of waiting for 
     * platform_interface_init as we might get a dma interrupt (error)
     * before the PA level is initialized
     */
    createlevel(LEVEL_DMA, NULL, "DMA/Network Interrupt");

    /* Initialize DMA Receive QUEUE and shadow */
    dmac_drq = malloc_named_pcimem(DRQ_SIZE * sizeof(drq_entry_t), "DRQ");
    if (!dmac_drq) {
	crashdump(0);
    }
    dmac_drq = pci_memaddr_convert(dmac_drq, TLB_UNCACHED);
    DAS_DEBUG(dmac_drq);

    dmac_drq_shdw = malloc_fast(DRQ_SIZE * sizeof(particletype *));
    if (!dmac_drq_shdw) {
	crashdump(0);
    }
    dmac_drq_read = dmac_drq_write = 0;
    dmac_drq_avail = DRQ_SIZE-1;

    /* Initialize DMA Transmit QUEUE and shadow */
    dmac_dtq = malloc_named_pcimem(DTQ_SIZE * sizeof(dtq_entry_t), "DTQ");
    if (!dmac_dtq) {
	crashdump(0);
    }

    dmac_dtq = pci_memaddr_convert(dmac_dtq, TLB_CACHE_WT);
    DAS_DEBUG(dmac_dtq);

    dmac_dtq_shdw = malloc_fast(DTQ_SIZE * sizeof(particletype *));
    if (!dmac_dtq_shdw) {
	crashdump(0);
    }

    dmac_dtq_read = dmac_dtq_write = 0;

    dmac_qe_status_shdw = 0;
    
    txq_index = 0;

    initialize_qe();

}

/*
 * svip_init_dma_pools:
 * Initialize buffer pools used by the DMA driver
 */
void svip_init_dma_pools (void)
{
    mempool *dma_buffer_mempool;
    particletype *particle;
    int i;

    DAS_DEBUG(0x33330004);

    /*
     * Init pointer to the global interface pool of paktype headers
     */
    dmac_headerpool = pas_interface_header_pool(NULL);
    if (!dmac_headerpool) {
	crashdump(0);
    }

    /*
     * Setup a dma paktype.
     */
    dmac_pak = pas_allocate_fspak();
    if (!dmac_pak) {
	crashdump(0);
    }

    svip_dma_flush_pak(dmac_pak);

    /*
     * Init the DMA particle pool
     */
    dma_buffer_mempool = vip_dma_buffer_mempool();

    dmac_pool_group = pool_create_group();


    dmac_pool = particle_pool_create("DMA",
				     dmac_pool_group,
				     VIP_PARTICLE_BYTES, 
				     POOL_SANITY,
				     32,
				     dma_buffer_mempool);

    if (!dmac_pool) {
	crashdump(0);
    }
    DAS_DEBUG(0x33330005);
    DAS_DEBUG(&dmac_pool);
    DAS_DEBUG(dmac_pool);
    
    pool_adjust(dmac_pool, 0, SVIP_DMA_BUFFERS, SVIP_DMA_BUFFERS, TRUE);
    dmac_pool->item.ret = dtq_repopulate;
    dmac_pool->fallback = NULL;

    dmac_cache_end = 0;
    /* Fill our local buffer cache */
    for (i = 0; i < SVIP_DMA_BUFFERS; i++) {
	particle = getparticle_inline(dmac_pool, dmac_pool->size, TRUE);
	if (particle) {
	    particle->data_start = (char *)particle->data_block;
	    dmac_cache[dmac_cache_end++] = particle;
	} else {
	    break;
	}
    }

    dtq_populate();
}

/*
 * vip_set_ipc_queues:
 * Setup IPC queues. Only on RSP system.
 */
boolean vip_set_ipc_queues (malu_cmd_t *cmd)
{
    pas_cbtype *pascb;
    vip_memd_ifcntl_t *ifc;

    if (RPSP_SELECT) {
        return(FALSE);
    }
    vsys_vec.rsp_ipc_tx_q = 
	(volatile ulong *)(RSP_MEMD2VIRTUAL(cmd->arg0));
    vsys_vec.rsp_ipc_rx_q = 
	(volatile ulong *)(RSP_MEMD2VIRTUAL(cmd->arg1));
    vsys_vec.rsp_ipc_free_q = 
	(volatile ulong *)(RSP_MEMD2VIRTUAL(cmd->arg2));

    pascb = vsys_vec.ipc_pascb;
    ifc = pascb->vip_memd_ifcntl;

    /* 
     * Set up the structures in the ifc so that normal enable and disable
     * tx polling work as expected .
     */
    ifc->status_word_register = IPC_STATUS_WORD;
    ifc->status_word_bit_mask = IPC_STATUS_WORD_BIT;

    stop_qe();

    /* GIVE QE the address of the transmit queue */
    CYA_ASIC->qe.xmem.ipc_qaddr = cmd->arg0;


    /* Value QE returns in DTQ entry */
    CYA_ASIC->qe.ymem.dtq_ipc_id = (IPC_STATUS_WORD<<DTQ_INTFC_STW_SHIFT)<<
                                                              DTQ_INTFC_SHIFT;

    start_qe();

    /* Create mapping between value QE returns in DTQ entry and idb */
    dmac_sw2idb[IPC_STATUS_WORD<<DTQ_INTFC_STW_SHIFT] = pascb->idb;

    /* enable QE ipc polling */
    vip_enable_tx_polling(pascb, 0, 0); 

    return(TRUE);	    
}

/*
 * vip_set_tx_pointers:
 * Setup transmit pointers. Not supported on RP/SP systems
 */
boolean vip_set_tx_pointers (malu_cmd_t *cmd)
{
    vip_memd_ifcntl_t *ifc;
    ulong status_word_addr;
    ulong queue_number;
    int modulo;

    if (RPSP_SELECT) {
        return(FALSE);
    }

    /* 
     * Set up the transmit queue and accumulator for the interface
     * specified in the command
     */
    ifc = cmd->pascb->vip_memd_ifcntl;
    ifc->rsp_tx_q = (volatile ulong *)(RSP_MEMD2VIRTUAL(cmd->arg0));
    ifc->rsp_tx_acc = (volatile short *)(RSP_MEMD2VIRTUAL(cmd->arg1));

    /* 
     * Determine the status word associated with the specified transmit queue
     * and the bit position for the queue within the status word 
     */
    queue_number = cmd->arg0/8;
    status_word_addr = (queue_number/QUEUES_PER_STATUS_REGISTER) * sizeof(ushort);
    modulo = queue_number % QUEUES_PER_STATUS_REGISTER;

    /*
     * Set the mask used to control Queueing Engine polling
     */
    ifc->status_word_bit_mask = 1 << modulo;

    /* 
     * Set the fast queue mask here too.
     */
    
    stop_qe();

    if (!txq_index) {
        /*
	 * status word has not been registered
	 */
        dmac_qe_status_shdw = status_word_addr;
	CYA_ASIC->qe.ymem.status = status_word_addr;
	/* no one enabled at this point */
	CYA_ASIC->qe.xmem.sts_mask[TX_STATUS_WORD].reg = 0;
	CYA_ASIC->qe.ymem.fastq_sts_bit[0].reg = 0;
	CYA_ASIC->qe.ymem.fastq_sts_bit[1].reg = 0;
    }	

    ifc->status_word_register = TX_STATUS_WORD;
    
    /* Create mapping between value QE returns in DTQ entry and idb */
    dmac_sw2idb[(TX_STATUS_WORD<<DTQ_INTFC_STW_SHIFT) | txq_index] =
                                                         cmd->pascb->idb;
    
     /* If the 2txq ucode is loaded, make sure that variables relevant to the 
      *   txq's are in the txq0 and txq1 in QE X/Y mem. 
      */
    if ( (vsys_vec.pa_conf_type == PA_FAST) && (txq_index < QE_FAST_QUEUE) ) {

        /*
	 * Set the mask used to control Queueing Engine polling
	 */
        ifc->status_word_bit_mask = 1 << txq_index;
 
	CYA_ASIC->qe.ymem.fastq_sts_bit[txq_index].reg =  1 << txq_index;

       if (is_channelized(cmd->pascb->pa_bay)) {
	   /* tell the QE not to inc the acc of this txq */
	   CYA_ASIC->qe.xmem.txq_addr[txq_index].reg = CHAN_TXQ_FLAG | cmd->arg0;
       } else {
	   CYA_ASIC->qe.xmem.txq_addr[txq_index].reg = cmd->arg0;
       } 
  	CYA_ASIC->qe.ymem.dtq_txq_id[txq_index].reg =
 	  (((TX_STATUS_WORD<<DTQ_INTFC_STW_SHIFT) | txq_index)
  	   <<DTQ_INTFC_SHIFT);
  
    } else {

        /*
 	 * Set the mask used to control Queueing Engine polling
 	 */
 	ifc->status_word_bit_mask = 1 << modulo;

	if (is_channelized(cmd->pascb->pa_bay)) {
	    /* tell the QE not inc the acc of this txq */
	    CYA_ASIC->qe.xmem.txq_addr[modulo].reg = CHAN_TXQ_FLAG | cmd->arg0 ;
	} else {
	    CYA_ASIC->qe.xmem.txq_addr[modulo].reg = cmd->arg0;
        }  
 
 	CYA_ASIC->qe.ymem.dtq_txq_id[modulo].reg =
 	  (((TX_STATUS_WORD<<DTQ_INTFC_STW_SHIFT) | txq_index)
 	   <<DTQ_INTFC_SHIFT);
     }
    txq_index++;

    start_qe();

    return(TRUE);
}

/*
 * remove_suspended_list:
 * remove the specified pascb from the suspended list
*/
static void remove_suspended_list (pas_cbtype *pascb)
{
    pas_cbtype *cur_pascb, *prev_pascb=NULL;

    /*
     * We need to take this puppy off the suspended list 
     */
    for (cur_pascb = dmac_polling_suspended_queue_head; 
	 cur_pascb != NULL; 
	 prev_pascb = cur_pascb, cur_pascb = cur_pascb->next) {

	if (cur_pascb == pascb) {
		 
	    if (pascb == dmac_polling_suspended_queue_head) {
		dmac_polling_suspended_queue_head = pascb->next;
	    } else {
		prev_pascb->next = pascb->next;
	    }
		
	    break;
	}
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
    vip_memd_ifcntl_t *ifc;

    /* 
       The VIPER RSP polling mechanism is implemented by the CYA queueing
       engine.  It has 2 registers which hold the addresses of 2 status 
       words.  Whenever the rsp issues a set_tx_pointers command, we check
       to see if the status word that corresponds to the transmit queue has
       been registered with the queueing engine.  We use shadow registers 
       stored in dmac for this purpose. Associated with each status word is
       a bit mask that enables / disables polling for the corresponding 
       interface.
       */

    ifc = pascb->vip_memd_ifcntl;

    stop_qe();

    CYA_ASIC->qe.xmem.sts_mask[ifc->status_word_register].reg |= 
	                                     ifc->status_word_bit_mask;              
    start_qe();

    if (ifc->memd_poll_suspend_queued) {
	remove_suspended_list(pascb);
	ifc->memd_poll_suspend_queued = FALSE;
    }

    return TRUE;
}

/*
 * vip_disable_tx_polling:
 * Disable polling on a tx interface
 */
boolean vip_disable_tx_polling (pas_cbtype *pascb)
{
    vip_memd_ifcntl_t *ifc;

    ifc = pascb->vip_memd_ifcntl;

    stop_qe();

    CYA_ASIC->qe.xmem.sts_mask[ifc->status_word_register].reg &= 
	                                     ~ifc->status_word_bit_mask;              
    start_qe();

    if (ifc->memd_poll_suspend_queued) {
	remove_suspended_list(pascb);
	ifc->memd_poll_suspend_queued = FALSE;
    }

    return TRUE;
}

/*
 * vip_suspend_tx_polling:
 * Suspend polling on a tx interface until interface catches up.
 */
boolean vip_suspend_tx_polling (pas_cbtype *pascb)
{
    vip_memd_ifcntl_t *ifc;

    ifc = pascb->vip_memd_ifcntl;

    if (ifc->memd_poll_suspend_queued)
	return TRUE;

    stop_qe();

    CYA_ASIC->qe.xmem.sts_mask[ifc->status_word_register].reg &= 
	                                     ~ifc->status_word_bit_mask;              
    start_qe();

    pascb->next = dmac_polling_suspended_queue_head;
    dmac_polling_suspended_queue_head = pascb;
    ifc->memd_poll_suspend_queued = TRUE;

    return TRUE;
}

/*
 * vip_show_dtq:
 * Stub on checkers.
 */
void vip_show_dtq (parseinfo *csb)
{
    printf("VIP DTQ status:\n");
    printf(" dtq=%#x, size=%d, read=%d, write=%d, pkts = %d\n",
	   dmac_dtq, DTQ_SIZE, dmac_dtq_read, dmac_dtq_write, dmac_dtq_pkt_count);
}

/*
 * vip_show_drq:
 * Stub on checkers
 */
void vip_show_drq (parseinfo *csb)
{
    printf("VIP DRQ status:\n");
    printf(" drq=%#x, size=%d, read=%d, write=%d, pkts =%d\n",
	    dmac_drq, DRQ_SIZE, dmac_drq_read, dmac_drq_write, 
	    dmac_drq_pkt_count);
}


/*
 * vip_show_dma:
 * Display info about the DMA engine
 */
void vip_show_dma (parseinfo *csb)
{
    leveltype level;
    pas_cbtype *pascb;
    boolean poll_array[MAX_PA_BAYS * MAX_INTERFACES_PER_PA];
    boolean polling_banner = FALSE;
    int i;
    ulong status_mask[QE_NUM_STATUS];

    /*
     * Print out all the pointers and status
     */
    printf("VIP DMA Engine status:\n");
    printf("QE ucode: %s ver: %d.%d:\n", cya_sw_code, cya_sw_version, cya_sw_minver);
    printf(" pool=%#x, headerpool=%#x, cache_end=%d\n",
	   dmac_pool, dmac_headerpool, dmac_cache_end);
    printf(" dmac_pak=%#x, drq_full=%d, no_memd_buff=%d\n",
	   dmac_pak, dmac_drq_full, dmac_no_memd_buff);


    level = raise_interrupt_level(NETS_DISABLE);
    stop_qe();
    for (i = 0; i < QE_NUM_STATUS; i++) {
        status_mask[i] = CYA_ASIC->qe.xmem.sts_mask[i].reg;
    }
    start_qe();
    reset_interrupt_level(level);
    for (i = 0; i < QE_NUM_STATUS; i++) {
        printf("QE Status Mask  = %#x\n", status_mask[i]);
    }

    for (i = 0; i< MAX_TX_QUEUES; i++) {
	if (dmac_sw2idb[i]) {
	    printf("DTQ ID %#x maps to idb %#x\n", i, dmac_sw2idb[i]);
	}
    }

    for (i = 0; i < MAX_PA_BAYS * MAX_INTERFACES_PER_PA; i++) {
	poll_array[i] = FALSE;
    }

    level = raise_interrupt_level(NETS_DISABLE);
    for (pascb = dmac_polling_suspended_queue_head; 
	 pascb != NULL; 
	 pascb = pascb->next) {
        poll_array[pascb->pa_ifindex] = TRUE;
    }
    reset_interrupt_level(level);

    for (i = 0; i < MAX_PA_BAYS * MAX_INTERFACES_PER_PA; i++) {
	if (poll_array[i]) {
	    if (!polling_banner) {
		printf("Polling disabled interfaces:\n");
		polling_banner = TRUE;
	    }
	    printf("%d ", i);
	}
    }
    if (polling_banner)
	printf("\n");

    vip_show_dtq(csb);
    vip_show_drq(csb);
}

/*
 * The following EOIR related routines can be put in svip_memd.c.
 * However they need reference to many DMA related variables 
 * defined in this file. To localize all DMA-related routines
 * into one place, EOIR related routines for VIP2 (SVIP) are put here.
 */

/*
 * vip_quiesce_one_interface:
 * VIP-dependent routine to quiesce the VIP system
 */
void vip_quiesce_all_interfaces (void)
{
    int i;

    /*
     * Delay half of quiesce grace period set in RSP/RPSP
     * to process any invalidation IPC/DFS message after QUIESCE before
     * MEMD recarve
     */
    for (i=0; i<EOIR_BUFFER_RETURN_TIME/2; i++) {
	DELAY(1);
	vip_dmaq_consumer();
    }

    /*
     * Stop Queueing Engine
     */
    stop_qe();

    /*
     * clean up post read cache
     */
    for (i=0; i<MAX_RSP_ACCUMULATORS; i++) {
        tx_acc_cache[i].acc = 0;
        tx_acc_cache[i].valid = FALSE;
    }

    /*
     * throw away all active post-read
     */
    if (vsys_vec.pr2_tx_acc)
	i = *(ushort *)((ulong)&CYA_ASIC->pr.reg2u + 2);
    vsys_vec.pr2_tx_acc = 0;

    if (vsys_vec.pr1_ifc)
	i = CYA_ASIC->pr.reg1u;
    vsys_vec.pr1_ifc = NULL;

    /*
     * reset working variables of Queueing Engine
     */
    txq_index = 0;

    for (i=0; i<QE_NUM_STATUS; i++) 
        CYA_ASIC->qe.xmem.sts_mask[i].reg = 0;

    for (i=0; i<QE_FAST_QUEUE; i++) 
	CYA_ASIC->qe.ymem.fastq_sts_bit[i].reg = 0;

    for (i=0; i<QE_NUM_QUEUES; i++)
	CYA_ASIC->qe.xmem.txq_addr[i].reg = 0;

    for (i = 0; i< MAX_TX_QUEUES; i++)
        dmac_sw2idb[i] = NULL;

    dmac_qe_status_shdw = 0;
}

/*
 * vip_quiesce_one_interface:
 * VIP-dependent routine for the VIP system to recover from quiesced
 */
void vip_unquiesce_all_interfaces (void)
{
}
/* end of file */
