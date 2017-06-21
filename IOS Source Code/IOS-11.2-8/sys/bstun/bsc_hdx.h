/* $Id: bsc_hdx.h,v 3.2.60.1 1996/07/22 18:22:15 snyder Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bsc_hdx.h,v $
 *------------------------------------------------------------------
 * bsc_hdx.h - BSC Half Duplex Enforceement
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * Implementation of FSM which forces BSC to work in a half duplex 
 * transmission mode
 * 
 *------------------------------------------------------------------
 * $Log: bsc_hdx.h,v $
 * Revision 3.2.60.1  1996/07/22  18:22:15  snyder
 * CSCdi63677:  fix a pointer problem, and move more text to data
 *              moves 2692 bytes to data, 16 out of image completely
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:48:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/26  06:17:44  arothwel
 * CSCdi37680:  Fix BSC BADSHARE problem. Fix BSC FDX cfg prob. Fix BSC
 * dbg/sh cmd.
 *
 * Revision 2.1  1995/06/07  20:13:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __BSC_HDX_H
#define __BSC_HDX_H


typedef enum {
        bscHDX_TxFrame ,
        bscHDX_TxCompEOT,
        bscHDX_TxCompOther,
        bscHDX_RxFrame,
        bscHDX_Timeout
        } bsc_hdx_frame_e;

extern char* const
bsc_map_hdx_state[];

extern void
bsc_enforce_HDX_transmit(bsc_hdx_frame_e ,
                         hwidbtype *);

#endif  /* __BSC_HDX_H. */
/*---- End include file: bsc_hdx.h. */
