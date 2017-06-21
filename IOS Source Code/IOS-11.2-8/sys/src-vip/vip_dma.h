/* $Id: vip_dma.h,v 3.2.62.1 1996/04/20 17:32:22 getchell Exp $
 * $Source: /release/112/cvs/Xsys/src-vip/vip_dma.h,v $
 *------------------------------------------------------------------
 * vip_dma.h - VIP DMA definitions
 *
 * 2/95, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vip_dma.h,v $
 * Revision 3.2.62.1  1996/04/20  17:32:22  getchell
 * CSCdi54568:  FE/VIP2 perf. is severely impacted by presence of second
 * FE.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:52:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:45  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __VIP_DMA_H__
#define __VIP_DMA_H__

/*
 * VIP DMA debug support. If VIP_DEBUG_DMA_FLAG is defined as FALSE,
 * the compiler will not compile up the code for it, thus saving
 * the read cycle and test to check vip_debug_dma_tx and vip_debug_dma_rx, 
 * and thus rendering "debug vip dma-[tx|rx]" unusable.
 *
 * If "debug vip dma" is to have an effect IE turning on DMA debugging
 * and getting some output, then VIP_DEBUG_DMA_FLAG should be
 * defined (and the fast path will be slightly slower
 * due to the checking of the debug flags). For now we will enable it.
 */
#define VIP_DEBUG_DMA_FLAG	(vip_debug_dma_tx || vip_debug_dma_rx)

/*
 * Prototypes
 */
extern boolean	vip_enable_tx_polling(pas_cbtype *, volatile ushort *, int);
extern boolean	vip_disable_tx_polling(pas_cbtype *);
extern boolean  vip_suspend_tx_polling(pas_cbtype *);

#endif /* !__VIP_DMA_H__ */
