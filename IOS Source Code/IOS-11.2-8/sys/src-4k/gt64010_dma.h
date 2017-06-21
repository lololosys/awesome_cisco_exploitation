/* $Id: gt64010_dma.h,v 3.1.22.1 1996/03/21 23:46:39 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-4k/gt64010_dma.h,v $
 *------------------------------------------------------------------
 * gt64010_dma.h - Galileo system controller DMA support
 *
 * Febuary 1996, Michael Beesley
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: gt64010_dma.h,v $
 * Revision 3.1.22.1  1996/03/21  23:46:39  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.2  1996/03/09  01:06:39  mbeesley
 * CSCdi51152:  DMA ring in SRAM should swap entries, not CPU
 * Branch: ELC_branch
 *
 * Revision 3.1.2.1  1996/02/19  23:24:11  mbeesley
 * CSCdi49335:  Predator needs support for DMA driven coalesce
 * Branch: ELC_branch
 *
 * Revision 3.1  1996/02/19  21:27:45  mbeesley
 * Add placeholder file for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Force a fetch of the next DMA record, chained with ints enabled
 * and a data transfer limit of 32, incrementing source and destination
 */
#define GT64010_DMA_FF_CHAINED_INT	0xC03D0000

/*
 * DMA must be at least 4 bytes
 */
#define GT64010_MIN_DMA_BYTES		0x04

/*
 * Macro to convert an address for the DMA engine. If the memory
 * has been setup so that it will byteswap when the DMA engine
 * accesses it, then the CPU does not need to byteswap. It just
 * needs to or in the address mask. Otherwise the CPU needs to 
 * byteswap the value before writing it into the DMA ring.
 */
#define GT64010_DMA_ADDR(x)					\
    (dma0->byte_swap_memory ? 					\
        (((ulong)x) | dma0->byte_swap_addr_mask) : 		\
        (vaxorder((ulong)x)))

/*
 * Macro to convert data being written to the DMA engines ring,
 * such as source address, destination address and length. If the
 * memory is byteswapping, then the CPU can just write this
 * data directly. Otherwise the CPU has to byteswap the data.
 */
#define GT64010_DMA_DATA(x)					\
    (dma0->byte_swap_memory ?					\
        ((ulong)x) :						\
        (vaxorder((ulong)x)))

/*
 * typedef for dma ring entry. Dictated by the gt64010
 */
typedef struct gt64010_dma_ring_t_ {
    volatile ulong byte_cnt;
    volatile ulong source;
    volatile ulong dest;
    volatile ulong next_ptr;
} gt64010_dma_ring_t;

/*
 * typedef for dma shadow entry. Under software control
 */
typedef struct gt64010_dma0_entry_t_ {
    struct gt64010_dma0_entry_t_ *next;		/* next entry */
    particletype	      	*particle;	/* particle for this entry */
    gt64010_dma_ring_t 	      	*ring_ptr;	/* corresonding dma entry */

    /*
     * Following fields are only valid for an entry describing
     * the beginning of a packet
     */
    paktype 		       	*newpak;	/* packet buffer */
    hwidbtype 		       	*idb;		/* associated idb */
    iqueue_t 		       	iqueue;		/* associated input queue */
    pas_coalesce_v 	      	func;		/* callback function */
    ulong 		       	particle_count;	/* particles in this packet */

} gt64010_dma0_entry_t;

/*
 * typedef for DMA0 control (gt64010 driven coalesce)
 */
typedef struct gt64010_dma0_cntl_t_ {
    boolean			dma_working;
    boolean			byte_swap_memory;
    ulong			byte_swap_addr_mask;
    gt64010_dma0_entry_t	*thread;
    gt64010_dma0_entry_t	*thread_end;
    gt64010_dma0_entry_t	*backup_thread;
    gt64010_dma0_entry_t	*backup_thread_end;
    gt64010_dma0_entry_t	*dma_free;
    gt64010_dma0_entry_t	*dma_done;
    gt64010_dma0_entry_t	*dma_reqt;
    gt64010_dma0_entry_t	*dma_list;
    gt64010_dma_ring_t		*dma_ring;

    /*
     * Statistics
     */
    ulong		       	dma_complete;
    ulong		     	exhausted_dma_entries;
    ulong		     	post_coalesce_callback;
    ulong		      	post_coalesce_frames;

} gt64010_dma0_cntl_t;

/*
 * External data structures
 */
extern gt64010_dma0_cntl_t	*dma0;

/*
 * gt64010_dma0_start:
 * Start the DMA0 channel
 */
static inline void gt64010_dma0_start (gt64010_dma0_entry_t *d)
{
    ulong addr;

    /*
     * Update channel 0's next record pointer
     */
    addr = (ulong)(d->ring_ptr);
    if (dma0->byte_swap_memory) {
	addr |= dma0->byte_swap_addr_mask;
    }
    GT64010_REGS->ch0_next_ptr = vaxorder(addr);

    /*
     * Tell the DMA channel to fetch the next record
     */
    GT64010_REGS->ch0_ctrl = GT64010_DMA_FF_CHAINED_INT;
}

/*
 * gt64010_dma0_get_free_blocks
 * Get count free blocks
 */
static inline gt64010_dma0_entry_t *gt64010_dma0_get_free_blocks (int count)
{
    gt64010_dma0_entry_t *dma_save, *dma;
    int i;

    dma_save = dma = dma0->dma_free;
    for (i=0; i<count; i++) {
	if (dma->next == dma0->dma_reqt) {
	    dma0->exhausted_dma_entries += 1;
	    return(NULL);
	}
	dma = dma->next;
    }
    return(dma_save);
}

/*
 * gt64010_dma0_commit_chain:
 * Add a chain of work to DMA channel 0
 */
static inline void gt64010_dma0_commit (gt64010_dma0_entry_t *dma_head,
					gt64010_dma0_entry_t *dma_tail,
					int count)
{
    leveltype level;

    level = raise_interrupt_level(LEVEL_DMA);
    if (dma0->dma_working) {
	/*
	 * Put this one into the backup thread
	 */
	if (dma0->backup_thread) {
	    dma0->backup_thread_end->ring_ptr->next_ptr = 
		GT64010_DMA_ADDR(dma_head->ring_ptr);
	    dma0->backup_thread_end = dma_tail;
	} else {
	    dma0->backup_thread = dma_head;
	    dma0->backup_thread_end = dma_tail;
	}

    } else {
	/*
	 * Put it into the main thread and kick start the DMA.
	 */
	dma0->thread = dma_head;
	dma0->thread_end = dma_tail;
	dma0->dma_working = TRUE;
	gt64010_dma0_start(dma_head);
    }
    reset_interrupt_level(level);
}

/*
 * Function prototypes
 */
extern gt64010_dma_ring_t *gt64010_dma_allocate_ring(gt64010_dma0_cntl_t *);
extern void		  gt64010_pak_coalesce(paktype *, uchar *, int,
					       gt64010_dma0_entry_t *);

/* end of file */
