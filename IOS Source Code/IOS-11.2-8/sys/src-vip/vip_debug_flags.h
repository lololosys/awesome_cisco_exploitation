/* $Id: vip_debug_flags.h,v 3.2.62.1 1996/04/19 15:31:58 che Exp $
 * $Source: /release/112/cvs/Xsys/src-vip/vip_debug_flags.h,v $
 *------------------------------------------------------------------
 * vip_debug_flags.h - debugging flags for VIP
 *
 * March 1995, Michael Beesley
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vip_debug_flags.h,v $
 * Revision 3.2.62.1  1996/04/19  15:31:58  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.3  1996/04/02  20:21:39  carrel
 * cvs fixes
 *
 * Revision 2.1.6.2  1996/01/09  18:41:38  xliu
 * Branch: IosSec_branch
 * Add crypto engine vip debug.
 *
 * Revision 2.1.6.1  1995/11/17  18:19:11  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.2  1995/11/17  18:52:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:43  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:59:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from the VIP code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */

#include "../ui/debug.h"
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */

DEBUG_ARRDECL(vip_debug_arr)

DEBUG_FLAG(vip_debug_cmdq,DEBUG_VIP_CMDQ,"Malu Attention Command Queue")
DEBUG_FLAG(vip_debug_memd,DEBUG_VIP_MEMD,"MEMD Usage")
DEBUG_FLAG(crypto_engine_vip_debug, DEBUG_VIP_CRYPTO_ENGINE, "VIP crypto engine")
DEBUG_FLAG(vip_debug_dma_tx,DEBUG_VIP_DMA_TX,"DMA Events (Tx Direction)")
DEBUG_FLAG(vip_debug_dma_rx,DEBUG_VIP_DMA_RX,"DMA Events (Rx Direction)")

DEBUG_ARRDONE
