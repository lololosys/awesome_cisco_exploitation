/* $Id: rvip_memd.h,v 3.2.62.3 1996/04/12 17:43:40 kao Exp $
 * $Source: /release/112/cvs/Xsys/src-rvip/rvip_memd.h,v $
 *------------------------------------------------------------------
 * rvip_memd.h - MEMD header file for Checkers
 *
 * January 1995, David Getchell
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rvip_memd.h,v $
 * Revision 3.2.62.3  1996/04/12  17:43:40  kao
 * CSCdi53527:  Need EOIR support on VIP2
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/03/28  02:06:56  hampton
 * If it doesn't return a value, it's gotta be void. Fix numerous
 * examples of inlines that slipped through the net.  [CSCdi52864]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/03/21  23:50:54  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/03/18  23:36:00  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.1  1996/03/07  17:29:24  getchell
 * CSCdi50953:  viper dfs mods
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  18:50:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:59:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern void r4k_update_timestamp(int ms_loss);

extern volatile boolean dma_working;
extern int vip_control_flags;

/*
 * Inline functions
 */

/*
 * Check the transmit queue limit of an outbound interface
 * during distributed switching.
 *
 * We do this from a function call in order to deal with
 * possible cbus stall handlers in the future.
 */
static inline boolean vip_rsp_check_tql (volatile ushort *tqlptr)
{
    ushort tql;
 
    tql = *tqlptr;
 
    if (tql > 1)
      return(TRUE);
 
    if (tql == 1)
      *tqlptr = 0xFFFF; /* Increment it back */
 
    return(FALSE);
}

/*
 * vip_allocate_memd_bh:
 * Allocate a memd buffer header. Returns it in VIP addressing format.
 */
static inline vip_bhdr_t *vip_allocate_memd_bh (pas_cbtype *pascb)
{
    vip_bhdr_t *bh;
    vip_memd_ifcntl_t *ifc = pascb->vip_memd_ifcntl;

    if (RPSP_SELECT) {

	bh = vip_rpsp_local_freeq_fetch(ifc);
	if (bh) {
	    return(bh);
	}
	
	/*
	 * We did not get one off of the local queue. Try global
	 */
	bh = vip_dequeue_bh_stack(ifc);
	if (bh) {
	    return(bh);
	}
	
	/*
	 * Global is empty as well. Stoke up the rx buffers.
	 */
	bh = vip_rpsp_stoke_rx_buffers(pascb);
	return(bh);

    } else {

	bh = vip_rsp_memd_readbh(ifc->rsp_local_free_q);
	if (bh) {
	    return(bh);
	}

	/*
	 * Failed to get one from the local freeq. Check our limits.
	 */
	if (*ifc->rx_count_ptr > ifc->rx_limits_high) {
	    return(NULL);
	}

	/*
	 * Try and get one from the global freeq
	 */
	bh = vip_rsp_memd_readbh(ifc->rsp_global_free_q);
	if (bh) {
	    *ifc->rx_count_ptr += 1;
	    return(bh);
	} else {
	    return(NULL);
	}
    }
}

/*
 * vip_bh2txparams:
 * Process the buffer header parameters for a tx request. Return length.
 * The bh parameter is in VIP addressing format.
 */
static inline ulong vip_bh2txparams (vip_bhdr_t *bh, uchar **tx_addr)
{
    ulong tx_len, tx_start;

    if (RPSP_SELECT) {
	
	/*
	 * Running on an RP/SP system
	 * If bus stall, vip_memdread32 returns FALSE
	 */
	if (vip_memdread32(&bh->bf_word1, &tx_len) == FALSE)
	    return(0);
	tx_start = (tx_len >> (VIP_RPSP_BF_TX_START_SHIFT-1));
	tx_len = (tx_len & VIP_RPSP_BF_TX_BYTES_MASK);
	*tx_addr = vip_rpsp_lookup_bhcache(bh);
	if (tx_len & ODD_BYTE_START) {
	    *tx_addr += (tx_start + 1);
	    return(tx_len & VALID_PKT_LENGTH);
	} else {
	    *tx_addr += tx_start;
	    return(tx_len);
	}

    } else {

	/*
	 * Running on an RSP system. Save return queue pointer
	 * adjust the length and read the start address.
	 * If bus stall, vip_memdread32 returns FALSE
	 */
	if (vip_memdread32(&bh->bf_word1, &tx_len) == FALSE)
	    return(0);
	vip_rsp_insert_bh_txcache(bh, tx_len);
	tx_len = (tx_len >> 16);
	if (vip_memdread32(&bh->bf_rsp_txptr, &tx_start) == FALSE)
	    return(0);
	*tx_addr = (uchar *)(tx_start & 0x00ffffff);
	return(tx_len);
    }
}

/*
 * vip_memd_buffer_start:
 * Return the VIP cpu address of the first byte in a memd buffer.
 * Note : bh is in VIP addressing format.
 */
static inline uchar *vip_memd_buffer_start (vip_bhdr_t *bh,
					    vip_memd_ifcntl_t *ifc)
{
    uchar *ptr;

    if (RPSP_SELECT) {
	ptr = vip_rpsp_lookup_bhcache(bh);
    } else {
	ptr = vip_rsp_lookup_bhcache(bh);
    }
    /*
     * If bus stall, vip_memdread32 returns FALSE and ptr is NULL
     */
    if (ptr == NULL)
	return(NULL);
    else
	return(ptr + ifc->rx_offset);
}
