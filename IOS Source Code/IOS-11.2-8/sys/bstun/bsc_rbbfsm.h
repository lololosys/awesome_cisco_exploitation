/* $Id: bsc_rbbfsm.h,v 3.3.52.2 1996/07/19 22:45:05 snyder Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bsc_rbbfsm.h,v $
 *------------------------------------------------------------------
 * bsc_rbbfsm.h - BSC Receive Block Building FSM definition.
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * FSM which builds frames from the single character interrupts
 *
 *------------------------------------------------------------------
 * $Log: bsc_rbbfsm.h,v $
 * Revision 3.3.52.2  1996/07/19  22:45:05  snyder
 * CSCdi63560:  move fsm tables to text space from data
 *              gets 53,716 bytes of data back for run from flash platforms
 *              gets 68 bytes of image for all
 *
 * Branch: California_branch
 *
 * Revision 3.3.52.1  1996/06/17  08:30:07  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.3  1995/11/21  05:43:54  jbalestr
 * CSCdi42577:  Router crashes on no bstun peer-name command
 *
 * Revision 3.2  1995/11/17  08:48:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/26  06:17:48  arothwel
 * CSCdi37680:  Fix BSC BADSHARE problem. Fix BSC FDX cfg prob. Fix BSC
 * dbg/sh cmd.
 *
 * Revision 2.1  1995/06/07  20:14:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __BSC_RBBFSM_H
#define __BSC_RBBFSM_H

extern char* const
bsc_rbb_state_names[];

extern void
bsc_rxChar_int(hwidbtype* idb, uchar input, boolean overrun);

extern void 
bsc_rxFramer_int(hwidbtype *idb, paktype **pak);

#endif  /* __BSC_RBBFSM_H. */
/*---- End include file: bsc_rbbfsm.h. */
