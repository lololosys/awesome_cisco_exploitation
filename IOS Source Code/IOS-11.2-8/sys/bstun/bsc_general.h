/* $Id: bsc_general.h,v 3.3 1995/11/21 06:47:50 jbalestr Exp $
 * $Source: /swtal/cherf.111/ios/sys/bstun/bsc_general.h,v $
 *------------------------------------------------------------------
 * bsc_genral.h - BSC General Functions
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * BSC pass-through specific functions.
 * 
 *------------------------------------------------------------------
 * $Log: bsc_general.h,v $
 * Revision 3.3  1995/11/21  06:47:50  jbalestr
 * CSCdi43341:  BSC passthru, all bsc traffic stops while receiving a frame
 *
 * Revision 3.2  1995/11/17  08:48:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/03  01:03:56  arothwel
 * CSCdi39397:  Fix:
 *
 *         o Add specific poll support for local-ack pollee.
 *         o Remove superfluous fields from lcb.
 *         o Generalize protocol main handler.
 *         o Finish off replacing all deprecated process mgnt calls.
 *         o Multiple poll-adds causing hard-on polling.
 *         o Add safety code to ignore 'spurious' Tx completes.
 *
 * Revision 2.2  1995/06/16  06:24:28  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:13:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __BSC_GENERAL_H
#define __BSC_GENERAL_H

#define CB_NS     0x3F   /* mask for NS */
#define CB_FRAME  0x00   /* frame with NS */
#define CB_ENQ    0x40   /* Poll/Selection/Bid ENQ with NS */
#define CB_DLEEOT 0x80   /* control bits for DLE EOT frame  */
#define CB_EOT    0xC0   /* control bits for an EOT frame */
#define CB_MASK   0xC0   /* mask for the control bits */

extern void
bsc_restart_receiver(hwidbtype* idb);

extern void
bsc_disable_receiver(hwidbtype* idb);

extern void
bsc_frame_tx(hwidbtype* idb);

extern void
bsc_pass_hardware_handler(paktype* pak, hwidbtype* idb);

extern void
bsc_pass_network_handler(paktype* pak, hwidbtype* idb);

extern void
bsc_pass_timer_handler(paktype* pak, hwidbtype* idb);

#endif  /* __BSC_GENERAL_H. */
/*---- End include file: bsc_general.h. */
