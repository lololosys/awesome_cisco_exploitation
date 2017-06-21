/* $Id: rpsp_memd.h,v 3.3.12.1 1996/03/21 23:52:10 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-vip/rpsp_memd.h,v $
 *------------------------------------------------------------------
 * rpsp_memd.h : VIP MEMD definitions for RP/SP Host
 *
 * February 1995, David Getchell
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rpsp_memd.h,v $
 * Revision 3.3.12.1  1996/03/21  23:52:10  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/03/18  23:36:28  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.1  1996/02/10  00:22:21  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.3  1996/02/18  19:42:40  kao
 * CSCdi45136:  EOIR Problems with VIP card
 *
 * Revision 3.2  1995/11/17  18:51:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:29  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:59:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(VIP_MEMD_STATIC_INLINES)

#define ODD_BYTE_START   	0x4000	/* Used to indicate odd byte start */
#define VALID_PKT_LENGTH 	0x3fff	/* due to odd byte start */

#define CCB_BASE   		0x40  	/* byte address of base MEMD CCB */
#define QUE_BASE   		0x90   	/* byte offset to start of q area */
#define ICB_BASE   		0xc0   	/* byte address of base MEMD ICB */
#define EXT_ICB_BASE 		0x2a0   /* byte address of extended ICB */
#define ICB_LENGTH 		12    	/* byte length of ICB */
#define ICB_INTFCS 		8     	/* number of interface ICBs per slot */
#define ICB_TXQHEAD  		2      	/* 16 bit offset to icb txqhead */
#define FCI_BUFFERIN 		0x8100 	/* indicates bufferin support to sp */

#define MAX_LOCK_SPIN 		1000  	/* fixme - need to determine a 
					   reasonable time */
#define MAX_ICB_PER_SLOT 	16 	/* allowed by new icb area in sp */

#define MEMD_QUE_RAW   		0x48
#define MEMD_QUE_RET   		0x4C

/*
 * Macros
 */
#define RPSP_MEMD2VIRTUAL(addr) (ADRSPC_CBUS | ((ulong)(addr)<<1))
#define RPSP_VIRTUAL2MEMD(addr) ((ushort)(((ulong)(addr)>>1) & 0xFFFF))

#define RPSP_PP_ADDR(pp)					\
    ((((ulong)pp << 8) & 0xff000) | ((ulong)pp >> 16))

#define RPSP_BH_CACHE_INDEX(bh)					\
    (((ulong)bh >> VIP_RPSP_BH_SHIFT) & 0xfff)

#define VIP_RPSP_BF_TX_START_SHIFT   	16
#define VIP_RPSP_BF_TX_BYTES_MASK    	0xffff

/*
 * The shift value below is for a buffer header pointer in VIP
 * addressing format, which shifted right will form an index
 * into the buffer address cache. It basically converts from
 * a 16 byte buffer header to an index.
 */
#define VIP_RPSP_BH_SHIFT		4

#define VIP_RPSP_BHFIELD_SHIFT		15

/*
 * vip_rpsp_que_t : queue structure for memd page 0 on RP/SP systems
 */
typedef struct {
    union {
	struct {
	    volatile ushort rawhead;
	    volatile ushort rawtail;
	} s0;
	volatile ulong rawhead_tail;
    } w0;
    
    volatile ulong unused1;
    
    union {
	struct {
	    volatile ushort rethead;
	    volatile ushort rettail;
	} s2;
	volatile ulong rethead_tail;
    } w2;
    
    volatile ushort freeque0;
    volatile ushort funused0;
    volatile ushort freeque1;
    volatile ushort funused1;
    volatile ushort freeque2;
    volatile ushort funused2;
    volatile ushort freeque3;
    volatile ushort funused3;
    volatile ushort freeque4;
    volatile ushort funused4;
    volatile ushort freeque5;
    volatile ushort funused5;
    volatile ushort freeque6;
    volatile ushort funused6;
    volatile ushort freeque7;
    volatile ushort funused7;
    volatile ushort diagsys;
    volatile ushort dunused0;
} vip_rpsp_que_t;

#define q_rpsp_rawhead 	w0.s0.rawhead
#define q_rpsp_rawtail 	w0.s0.rawtail
#define q_rpsp_rethead 	w2.s2.rethead
#define q_rpsp_rettail 	w2.s2.rettail
#define q_rpsp_rawque 	w0.rawhead_tail
#define q_rpsp_retque 	w2.rethead_tail

/*
 * Prototypes
 */
extern void		vip_rpsp_if_init(hwidbtype *);
extern boolean		vip_rpsp_local_txq_sync(void);
extern boolean		vip_rpsp_local_rxq_sync(void);
extern vip_bhdr_t     	*vip_rpsp_stoke_rx_buffers(pas_cbtype *);
extern boolean		vip_rpsp_send_lovenote(pas_cbtype *, ushort);
extern void		vip_rpsp_memd_disable_cleanup(pas_cbtype *);
extern void		vip_rpsp_kernel_ready(void);
extern void		vip_rpsp_init_memd_support(void);

#else /* VIP_MEMD_STATIC_INLINES */

/***************************************************************************
 * Static inlines
 ***************************************************************************/

/*
 * vip_rpsp_lookup_bhcache:
 * Look up the cache of buffer headers to see if we know the start
 * address of this memd buffer. If not read it from MEMD. 
 * Note: bh is in VIP addressing format.
 */
static inline uchar *vip_rpsp_lookup_bhcache (vip_bhdr_t *bh)
{
    uchar *addr;

    addr = vip_bhcache[RPSP_BH_CACHE_INDEX(bh)];
    if (!addr) {
	/*
	 * Read memd to get buffer pointer and page number
	 * If bus stall, vip_memdread32 returns FALSE
	 */
	if (vip_memdread32(&bh->bf_rpsp_ptr_page, (ulong *)&addr) == FALSE)
	    return(NULL);
	addr = (uchar *)((RPSP_PP_ADDR(addr)) << 1);
	vip_bhcache[RPSP_BH_CACHE_INDEX(bh)] = addr;
    }
    return(addr);
}

/*
 * vip_rpsp_local_freeq_fetch:
 * Allocate a memd buffer header off of a local free queue. If
 * there is not one, try and create it.
 */
static inline vip_bhdr_t *vip_rpsp_local_freeq_fetch (vip_memd_ifcntl_t *ifc)
{
    vip_bhdr_t *bh, *localhead;

    bh = ifc->rpsp_local_freeq_head;
    if (bh) {
	/*
	 * We have a local freeq head. Use it.
	 */
	localhead = vip_rpsp_memd_readbh(&bh->bf_rpsp_link);
	if (localhead) {
	    ifc->rpsp_local_freeq_head = localhead;
	    return(bh);
	}
	return(NULL);
    }

    /*
     * We have not picked up the local freeq head. Try to now.
     */
    bh = vip_rpsp_memd_readbh(&ifc->icb_ptr->icb_rpsp_freeq);
    if (bh) {
	/* 
	 * Great, got ourselves a local free queue.  Now see if there 
	 * is more than one buffer on the queue (can't take the last one)
	 */
	localhead = vip_rpsp_memd_readbh(&bh->bf_rpsp_link);
	if (localhead) {
	    ifc->rpsp_local_freeq_head = localhead;
	    return(bh);
	} else {
	    /* 
	     * Only one buffer on local free queue - save the buffer as
	     * new local free queue head 
	     */
	    ifc->rpsp_local_freeq_head = bh;
	}
    }
    return(NULL);
}

/*
 * vip_rpsp_local_raw_queue_add:
 * Add a buffer header to the local raw queue.
 */
static inline void vip_rpsp_local_raw_queue_add (vip_bhdr_t *bh, int length)
{
    if (vsys_vec.rpsp_rx_qtail) {
	vsys_vec.rpsp_rx_qtail->bf_word0 = 
	    ((ulong)bh << VIP_RPSP_BHFIELD_SHIFT) | 
	    (vsys_vec.rpsp_rx_last_length);
	vsys_vec.rpsp_rx_qtail = bh;
    } else {
	vsys_vec.rpsp_rx_qhead = bh;
	vsys_vec.rpsp_rx_qtail = bh;
    }
    vsys_vec.rpsp_rx_last_length = length;
}

/*
 * vip_rpsp_return_rx_bh:
 * Return a received memd buffer to the SP.
 */
static inline void vip_rpsp_return_rx_bh (vip_bhdr_t *bh)
{
    bh->bf_rpsp_rxsts = 0;
    vip_rpsp_local_raw_queue_add(bh, 0);
}

/*
 * vip_rpsp_local_returnq_add:
 * Add a buffer header to the local return queue
 */
static inline void vip_rpsp_local_returnq_add (vip_bhdr_t *bh)
{
    if (vsys_vec.rpsp_tx_qtail) {
	vsys_vec.rpsp_tx_qtail->bf_rpsp_link = RPSP_VIRTUAL2MEMD(bh);
	vsys_vec.rpsp_tx_qtail = bh;
    } else {
	vsys_vec.rpsp_tx_qtail = bh;
	vsys_vec.rpsp_tx_qhead = bh;
    }
}

/*
 * vip_rpsp_local_returnq_add_list
 * Add a list of buffers headers to the local return queue 
 */
static inline void vip_rpsp_local_returnq_add_list (vip_bhdr_t *h, 
						    vip_bhdr_t *t)
{
    if (vsys_vec.rpsp_tx_qtail) {
	vsys_vec.rpsp_tx_qtail->bf_rpsp_link = RPSP_VIRTUAL2MEMD(h);
	vsys_vec.rpsp_tx_qtail = t;
    } else {
	vsys_vec.rpsp_tx_qhead = h;
	vsys_vec.rpsp_tx_qtail = t;
    }
}

/*
 * vip_rpsp_local_rawq_add_list:
 * Add a list of buffer headers to the local raw queue.
 */
static inline void vip_rpsp_local_rawq_add_list (vip_bhdr_t *h, vip_bhdr_t *t, 
						 int length)
{
    if (vsys_vec.rpsp_rx_qtail) {
	vsys_vec.rpsp_rx_qtail->bf_word0 = 
	    ((ulong)h << VIP_RPSP_BHFIELD_SHIFT) |
	    (vsys_vec.rpsp_rx_last_length);
	vsys_vec.rpsp_rx_qtail = t;
    } else {
	vsys_vec.rpsp_rx_qhead = h;
	vsys_vec.rpsp_rx_qtail = t;
    }
    vsys_vec.rpsp_rx_last_length = length;
}

#endif /* VIP_MEMD_STATIC_INLINES */

/* end of file */
