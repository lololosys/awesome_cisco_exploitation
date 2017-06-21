/* $Id: vip_debug.h,v 3.2 1995/11/17 18:51:57 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-vip/vip_debug.h,v $
 *------------------------------------------------------------------
 * vip_debug.h - Headers for routines used for VIP debugging.
 *
 * March 1995, Michael Beesley
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vip_debug.h,v $
 * Revision 3.2  1995/11/17  18:51:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:42  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:59:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Prototypes
 */ 
extern void vip_init_debug(void);

/*
 * The actual debugging flags are defined in vip_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */

#include "vip_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "vip_debug_flags.h"

/*
 * Externals
 */
extern boolean vip_debug_cmdq;
extern boolean vip_debug_memd;
extern boolean vip_debug_dma_tx;
extern boolean vip_debug_dma_rx;

/* end of file */
