/* $Id: rvip_dma.h,v 3.3.12.1 1996/03/21 23:50:50 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-rvip/rvip_dma.h,v $
 *------------------------------------------------------------------
 * rvip_dma.h - RVIP DMA definitions
 *
 * 2/95, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rvip_dma.h,v $
 * Revision 3.3.12.1  1996/03/21  23:50:50  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/03/18  23:35:57  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.1  1996/02/10  00:19:10  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.3  1996/02/18  19:42:33  kao
 * CSCdi45136:  EOIR Problems with VIP card
 *
 * Revision 3.2  1995/11/17  18:50:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:12  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Definitions
 */
#define RVIP_MIN_DMA_LENGTH 		8
#define RVIP_MAX_DMA_LENGTH		4096
#define RVIP_MAX_DMA_LEN_SHIFT		12
#define RVIP_MAX_LOCK_SPIN		1000
#define RVIP_DMA_ENTRIES		1000
#define RVIP_DMA_USEC_TIME		100
#define RVIP_DMA_FORCE_THRESH		15
#define RVIP_DMA_OUTPUTQ_LIMIT		20

#define RVIP_DMA_POLL_POOL		(MAX_VIP_INTERFACES+1)

#define RVIP_DMA_BUFFERS		256		/* 256 particles */

/*
 * DMA entry status bit fields
 */
#define RVIP_DMA_FROM_MEMD		0x01		/* entry from memd */
#define RVIP_DMA_EOP			0x02		/* EOP entry */
#define RVIP_FREE_DMA_ENTRY		0x04		/* Free entry */
#define RVIP_REQUEST_DMA_ENTRY		0x08		/* Request entry */
#define RVIP_DONE_DMA_ENTRY		0x10		/* Done entry */
#define RVIP_MANUAL_DMA_ENTRY		0x20		/* Small dma */
#define RVIP_SCATTERED_DMA		0x40		/* particle */
#define RVIP_DMA_DFS			0x80		/* DFS frame */

#define RVIP_DMA_COMPLETE		(RVIP_DONE_DMA_ENTRY|RVIP_DMA_EOP)

/*
 * DMA engine hardware definitions
 */
#define RVIP_DMA_DIRECTION_TX 		0x04000000	/* from memd, hw */
#define RVIP_DMA_DIRECTION_RX 		0x00000000	/* to memd, hw */
#define RVIP_DMA_DISABLE		0x01000000	/* enable ints */
#define RVIP_DMA_INT_ENABLE   		0x02000000	/* enable ints */

#define RVIP_DMA_LENGTH_SHIFT 		16

/*
 * Defines for the dma entry fields. Used by asm code
 */
#define DMA_ENTRY_OFS_NEXT		0
#define DMA_ENTRY_OFS_LENGTH		4
#define DMA_ENTRY_OFS_STATUS		6
#define DMA_ENTRY_OFS_TOT_LENGTH	8
#define DMA_ENTRY_OFS_ENTRY_DATA	12
#define DMA_ENTRY_OFS_IFCNTL		16
#define DMA_ENTRY_OFS_BHDR		20
#define DMA_ENTRY_OFS_MEMD_ADDR		24
#define DMA_ENTRY_OFS_PCI_ADDR		28

#ifndef ASMINCLUDE

/*
 * typedef for each dma request
 * Note: keep the C coded structures compatible with the asm defines.
 */
typedef volatile struct dma_entry_t_ {

    volatile struct dma_entry_t_ *next;/* next pointer */

    ushort length;			/* length of this request */
    ushort status;			/* status for this request */
    ulong tot_length;			/* total length of a packet */

    void *entry_data;			/* pak / particle pointer */

    vip_memd_ifcntl_t *ifc;		/* VIP memd interface control */
    vip_bhdr_t *bhdr;			/* buffer header pointer */

    uchar *memd_addr;			/* memd address */
    uchar *pci_addr;			/* PCI address */

    volatile ulong *responseq;		/* RSP response queue pointer */

} dma_entry_t;

/*
 * typedef for the data structure that controls polling of tx queues
 */
typedef struct rvip_dma_poll_t_ rvip_dma_poll_t;

struct rvip_dma_poll_t_ {

    rvip_dma_poll_t 	*next;
    
    /*
     * Pointers to controlling data structure
     */
    vip_memd_ifcntl_t	*ifc;
    volatile ushort	*icb_tx_q;

    /*
     * Timing variables to throttle polling of empty queues
     */
    usec_timestamp	usec_time;
    int			poll_usec_interval;
    boolean		last_poll_failed;

};

/*
 * Checkers DMA control data structure
 */
typedef struct dma_cntl_t_ {

    /*
     * DMA paktype pointer
     */
    paktype 		*dma_pak;

    /*
     * MEMD Tx queue polling support
     */
    vip_bhdr_t		*tx_bh;
    vip_memd_ifcntl_t	*tx_ifc;
    ulong		tx_stalled_length;
    uchar 		*tx_memd_addr;

    /*
     * DMA memd tx polling support and buffer pool
     */
    rvip_dma_poll_t 	*rvip_poll_ptr;
    pooltype		*pool;
    pooltype		*headerpool;
    queuetype 		rvip_dma_runq;
    queuetype 		rvip_dma_poll_poolq;
    int			pool_group;

    /*
     * Counters
     */
    int			reqt_count;
    int 		dma_manual_copy_count;
    int			dma_start_count;
    int			dma_tx_stall_count;
    int			dma_pool_miss;
    int			flushed_reqt_count;

    /*
     * DMA poll entry pool
     */
    dma_entry_t       	*dmaq;

} dma_cntl_t;

/* 
 * Externs
 */
extern dma_entry_t *dma_free, *dma_reqt, *dma_done;
extern dma_cntl_t dmac;

/*
 * Prototypes
 */
extern void		rvip_init_dma_pools(void);
extern int		rvip_dma_jumpstart(void);
extern boolean		vip_pak_to_host(hwidbtype *, pas_cbtype *, paktype *);
extern boolean		vip_pak_switch(vip_memd_ifcntl_t *, paktype *,
				       vip_bhdr_t *, volatile ulong *);

/**************************************************************************
 * Static inlines
 **************************************************************************/

/*
 * vip_acquire_global_lock:
 * Get the global lock
 */
static inline boolean vip_acquire_global_lock (void)
{
    int count = 0;

    while (!(DMA_ASIC)->dma_reg7.cbus_lock_granted) {
	if (count++ > RVIP_MAX_LOCK_SPIN) {
	    return(FALSE);
	}
    }
    return(TRUE);
}

/*
 * vip_clear_global_lock_request:
 * Give up the global lock
 */
static inline void vip_clear_global_lock_request (void)
{
    (DMA_ASIC)->dma_reg6.cbus_lock_request = FALSE;
}

/*
 * dma_incr_request_count:
 * Increment the reqt_count and return TRUE if above a threshold
 */
static inline boolean dma_incr_request_count (void)
{
    return((++dmac.reqt_count) > RVIP_DMA_FORCE_THRESH);
}

/*
 * dma_get_done_blocks:
 * Pick up completed DMA requests, up till the first EOP.
 */
static inline dma_entry_t *dma_get_done_blocks (void)
{
    dma_entry_t *dma, *ptr;

    ptr = dma = dma_done;
    while (ptr->status & RVIP_DONE_DMA_ENTRY) {
	if (ptr->status & RVIP_DMA_EOP) {
	    dma_done = ptr->next;
	    return(dma);
	}
	ptr = ptr->next;
    } 
    return(NULL);
}

/*
 * dma_mark_free:
 * Walk from dma_ptr to dma_done marking entries as free
 */
static inline void dma_mark_free (dma_entry_t *dma_ptr)
{
    while (dma_ptr != dma_done) {
	dma_ptr->status = RVIP_FREE_DMA_ENTRY;
	dma_ptr = dma_ptr->next;
    }
}

/*
 * dma_get_free_blocks:
 * Get count free entries. Do not move the free pointer.
 */
static inline dma_entry_t *dma_get_free_blocks (uint count)
{
    dma_entry_t *dma, *dma_p;
    int i;

    if (count == 1) {
	if (!(dma_free->status & RVIP_FREE_DMA_ENTRY)) {
	    return(NULL);
	}
	return(dma_free);
    } else {
	dma = dma_p = dma_free;
	for (i=0; i<(count-1); i++) {
	    if (!(dma->status & RVIP_FREE_DMA_ENTRY)) {
		return(NULL);
	    }
	    dma = dma->next;
	}
	return(dma_p);
    }
}

#endif /* ASMINCLUDE */

/* end of file */
