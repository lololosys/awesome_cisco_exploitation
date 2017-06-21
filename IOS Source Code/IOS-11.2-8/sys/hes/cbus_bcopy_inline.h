/* $Id: cbus_bcopy_inline.h,v 3.2 1995/11/17 09:14:42 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/cbus_bcopy_inline.h,v $
 *------------------------------------------------------------------
 * hes/cbus_bcopy_inline.h
 *
 * RP ucode bcopy from RX pointer to TX pointer
 *
 * May 1995, Wilber Su
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cbus_bcopy_inline.h,v $
 * Revision 3.2  1995/11/17  09:14:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:37:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:37:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Max number of bytes to ask the cbus controller to bcopy for us at any
 * one time.
 */
#define FCI_BCOPY_LIMIT 200

/*
 * cbus_bcopy
 *
 * Use ucode to bcopy from the RX pointer to the TX pointer.
 * This has to be done at most FCI_BCOPY_LIMIT bytes at a time, or the SP
 * will cause a multibus timeout.  Note for the truly performance greedy:
 * since the RP does not block while the bcopy is going on, you could
 * actually copy this function and do calculations while the copy itself is
 * running.  Just put stuff in the inner loop or unroll the loop.
 */
static inline void cbus_bcopy (mciregtype *regptr, ushort i)
{
    ushort j;

    while (i) {
        regptr->rx2tx1bcopy = j = min(i, FCI_BCOPY_LIMIT);
        i -= j;
    }
}
