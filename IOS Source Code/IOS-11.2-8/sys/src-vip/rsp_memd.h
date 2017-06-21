/* $Id: rsp_memd.h,v 3.5.4.1 1996/03/21 23:52:14 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-vip/rsp_memd.h,v $
 *------------------------------------------------------------------
 * rsp_memd.h : VIP RSP MEMD definitions
 *
 * June 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_memd.h,v $
 * Revision 3.5.4.1  1996/03/21  23:52:14  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/03/18  23:36:31  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/03/07  17:30:00  getchell
 * CSCdi50953:  viper dfs mods
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1996/02/10  00:22:24  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.5  1996/03/08  23:20:20  kao
 * CSCdi50763:  TRIP OIR causes mic rel under traffic thru VIP
 *
 * Revision 3.4  1996/03/04  07:14:39  kao
 * CSCdi49674:  While VIP is cofigured with DFS, OIR event will crash the
 * VIP;
 *
 * Revision 3.3  1996/02/18  19:42:43  kao
 * CSCdi45136:  EOIR Problems with VIP card
 *
 * Revision 3.2  1995/11/17  18:51:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(VIP_MEMD_STATIC_INLINES)

/*
 * Definitions
 */
#define VIP_RSP_BH_SHIFT		4	/* from 16 to 1 */
#define VIP_RSP_MAX_LETTER		0x400	/* max love letter on RSP */
#define MAX_RSP_ACCUMULATORS            1024

/*
 * RSP command queues
 */
#define RSP_CMD_QUEUE_BASE		0x80	/* byte offset to queue area */

/*
 * Macros
 */
#define RSP_VIRTUAL2MEMD(addr) 	((ushort)(((ulong)(addr)) & 0xFFFF))
#define RSP_MEMD2VIRTUAL(addr)        				\
    (ADRSPC_CBUS | ((ulong)(addr) & MEMD_ADDR_MASK))

#define RSP_BH_CACHE_INDEX(bh)					\
    (((ulong)bh & 0xffff) >> VIP_RSP_BH_SHIFT)

#define RSP_CMD_QUEUE(slot)					\
    (RSP_MEMD2VIRTUAL(RSP_CMD_QUEUE_BASE + ((slot) << 3)))

/*
 * Prototypes
 */
extern void		vip_rsp_init_memd_support(void);
extern void             vip_rsp_memd_disable_cleanup(pas_cbtype *);
extern void		vip_rsp_kernel_ready(void);
extern void		vip_rsp_if_init(hwidbtype *);
extern boolean		vip_rsp_send_lovenote(pas_cbtype *, ushort);

#else /* VIP_MEMD_STATIC_INLINES */

/***************************************************************************
 * Static inlines
 ***************************************************************************/

/*
 * vip_rsp_qa_enqueue:
 * Enqueue a buffer header on the QA asic
 */
static inline void vip_rsp_qa_enqueue (volatile ulong *bh_q, vip_bhdr_t *bh)
{
    *(volatile ushort *)bh_q = (ushort)((ulong)bh & 0xffff);
}

/*
 * vip_rsp_insert_bh_txcache:
 * Insert a return queue pointer into the tx buffer header cache.
 * Note: bh is in VIP addressing format.
 */
static inline void vip_rsp_insert_bh_txcache (vip_bhdr_t *bh, ushort temp)
{
    vip_rsp_txcached_q[RSP_BH_CACHE_INDEX(bh)] = 
	(volatile ushort *)(RSP_MEMD2VIRTUAL(temp));
}

/*
 * vip_rsp_lookup_bhcache:
 * Look the cache of buffer headers to see if we know the start
 * address of this memd buffer. If not read it from memd, stick it
 * away in the cache and return it. This is for Rx buffers only.
 * Note: bh is in VIP addressing format, and the buffer address
 * is in offset format (IE 24 bits)
 */
static inline uchar *vip_rsp_lookup_bhcache (vip_bhdr_t *bh)
{
    uchar *addr;

    addr = vip_bhcache[RSP_BH_CACHE_INDEX(bh)];
    if (!addr) {
	/*
	 * Read MEMD for the buffer pointer and store it in the cache
	 * as a 24 bit address (up to 16M of MEMD)
	 * If bus stall, vip_memdread32 returns FALSE
	 */
	if (vip_memdread32(&bh->bf_rsp_bufptr, (ulong *)&addr) == FALSE)
	    return(NULL);
	addr = (uchar *)((ulong)addr & 0x00ffffff);
	vip_bhcache[RSP_BH_CACHE_INDEX(bh)] = addr;
    }
    return(addr);
}

/*
 * vip_rsp_return_queue_add:
 * Queue the buffer header directly
 */
static inline void vip_rsp_return_queue_add (vip_memd_ifcntl_t *ifc,
					     vip_bhdr_t *bh)
{
    volatile ushort *qptr;

    if (ifc->rsp_tx_acc) {
	*(ifc->rsp_tx_acc) = (short)(-1);
    }
    qptr = vip_rsp_txcached_q[RSP_BH_CACHE_INDEX(bh)];
    if (!qptr)
	return;
    *qptr = (ushort)(RSP_VIRTUAL2MEMD(bh));
}

#endif /* VIP_MEMD_STATIC_INLINES */

/* end of file */
