/* $Id: svip_memd.h,v 3.1.62.3 1996/04/12 17:44:16 kao Exp $
 * $Source: /release/112/cvs/Xsys/src-svip/svip_memd.h,v $
 *------------------------------------------------------------------
 * svip_memd.h - SVIP memd support defines
 *
 * December 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_memd.h,v $
 * Revision 3.1.62.3  1996/04/12  17:44:16  kao
 * CSCdi53527:  Need EOIR support on VIP2
 * Branch: California_branch
 *
 * Revision 3.1.62.2  1996/04/04  20:12:22  getchell
 * CSCdi52350:  Buffer header posted read broken
 * Branch: California_branch
 *
 * Revision 3.1.62.1  1996/03/21  23:51:29  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.10.3  1996/03/18  23:36:13  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.1.10.2  1996/03/07  17:29:46  getchell
 * CSCdi50953:  viper dfs mods
 * Branch: ELC_branch
 *
 * Revision 3.1.10.1  1996/02/10  00:22:04  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/12/26  21:18:40  getchell
 * Placeholder files for Viper development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SVIP_MEMD_H__
#define __SVIP_MEMD_H__

#define RSP_QA2VIRTUAL(addr)        (addr)

#ifdef ACC_POSTED_READ_SUPPORT

struct {
    ushort acc;
    boolean valid;
} tx_acc_cache[MAX_RSP_ACCUMULATORS];

/*
 * pr_2B_aligned_addr:
 * Massage a 2B aligned memd address for acumulator (only!) posted read 
 * operation 
 */
static inline ulong pr_2B_aligned_addr (void *addr)
{
    return (((ulong)addr & MEMD_ADDR_MASK) | PR_MASK_2BYTE);
}


/*
 * tx_acc_posted_read
 * This routine starts a posted read to the specified accumulator
 */
static inline void tx_acc_posted_read (void *tx_acc_ptr)
{
    if (vsys_vec.pr2_tx_acc) {
	/*
	 * Posted Read in progress for tx accumulator, finish it up.
	 * Note that this implementation assumes that all accumulators
	 * are aligned on 8 byte + 2 byte boundaries
	 */
/* FIXME -fix definition of posted read so that we can do the two byte
  access without adding two.
*/
	 tx_acc_cache[vsys_vec.pr2_tx_acc].acc = 
	                       *(ushort *)((ulong)&CYA_ASIC->pr.reg2u + 2);
	 tx_acc_cache[vsys_vec.pr2_tx_acc].valid = TRUE;

     }

    /*
     * Start a posted read on the requested accumulator
     */
    CYA_ASIC->pr.reg2 = pr_2B_aligned_addr(tx_acc_ptr);
    vsys_vec.pr2_tx_acc = RSP_VIRTUAL2MEMD(tx_acc_ptr)/8;
}
#endif

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
#ifdef ACC_POSTED_READ_SUPPORT
    ulong acc_number;

    /*
     * Start a posted read to replace the tx_acc we are about to use
     */
    tx_acc_posted_read((void *)tqlptr);

    acc_number = RSP_VIRTUAL2MEMD(tqlptr)/8;

    if (tx_acc_cache[acc_number].valid) {
	tql = tx_acc_cache[acc_number].acc;
	tx_acc_cache[vsys_vec.pr2_tx_acc].valid = FALSE;
    } else {
	/*
	 * No tx accumulator value cached - read a fresh value 
	 */
	tql = *tqlptr;
    }
#else
	tql = *tqlptr;
#endif
 
    if (tql > 1)
      return(TRUE);
 
    if (tql == 1)
      *tqlptr = 0xFFFF; /* Increment it back */
 
    return(FALSE);
}

#ifdef BH_POSTED_READS

/*
 * pr_4B_aligned_addr:
 * Massage a 4B aligned memd address for posted read operation
 */
static inline ulong pr_4B_aligned_addr (void *addr)
{
    return (((ulong)addr & MEMD_ADDR_MASK) | PR_MASK_4BYTE);
}

/*
 * svip_bhdr_posted_read:
 * This routine completes any posted reads that are in effect and 
 * starts a new posted read for the specified interface
 */
static inline void svip_bhdr_posted_read (vip_memd_ifcntl_t *ifc)
{
    vip_bhdr_t *bh;
    ulong temp;

    if (vsys_vec.pr1_ifc) {
	/*
	 * Posted Read in progress for local free queue, finish it up.
	 * Note that this implementation assumes that all buffer header
	 * posted read targets access 32 bits aligned on 8 byte boundaries
	 */
	temp = CYA_ASIC->pr.reg1u;
	
	if (temp) {
	    bh = (vip_bhdr_t *)RSP_QA2VIRTUAL(temp);
	    vsys_vec.pr1_ifc->bh_stack[0] = bh;
	} else {
	    vsys_vec.pr1_ifc->bh_stack[0] = NULL;
	}
	
    }

    /*
     * Start a posted read on the local free queue
     */
    CYA_ASIC->pr.reg1 = pr_4B_aligned_addr((ulong *)ifc->rsp_local_free_q);
    vsys_vec.pr1_ifc = ifc;
}

#endif

/*
 * vip_allocate_memd_bh:
 * Allocate a memd buffer header. Returns it in VIP addressing format.
 * The algorithm we use is that we initially preallocate memd buffers for
 * active interfaces.  When a memd buffer is needed we grab (one of?) the 
 * allocated memd buffer and start a posted read to replace the buffer.
 * The preallocated memd buffer will either be sitting in a posted read 
 * register, or will be stacked.
 */
static inline vip_bhdr_t *vip_allocate_memd_bh (pas_cbtype *pascb)
{
    vip_memd_ifcntl_t *ifc=pascb->vip_memd_ifcntl;
    vip_bhdr_t *bh;
    
    /*
     * Start a posted read to replace the buffer we are about to use
     */
#ifdef BH_POSTED_READS
    svip_bhdr_posted_read(ifc);
    bh = ifc->bh_stack[0];
#else
    bh = (vip_bhdr_t *)RSP_QA2VIRTUAL(*ifc->rsp_local_free_q);
#endif

    if (!bh) {
      if (*ifc->rx_count_ptr <  ifc->rx_limits_high) {
	/*
	 * Attempt to get a buffer header from the global free queue
	 */
	bh = (vip_bhdr_t *)RSP_QA2VIRTUAL(*ifc->rsp_global_free_q);
	if (bh) {
	    *ifc->rx_count_ptr += 1;
	}
      } else {
	DAS_DEBUG(0x33337777);
      }
    }

    return (bh);
}

#endif /* !__SVIP_MEMD_H__ */

/* end of file */
