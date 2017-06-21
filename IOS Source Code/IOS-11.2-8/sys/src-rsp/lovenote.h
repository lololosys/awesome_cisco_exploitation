/* $Id: lovenote.h,v 3.2 1995/11/17 18:46:30 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-rsp/lovenote.h,v $
 *------------------------------------------------------------------
 * Love Note defines, structs and typedefs for RSP.
 *
 * April 1994, Jack Jenney
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lovenote.h,v $
 * Revision 3.2  1995/11/17  18:46:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:56:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(__LOVENOTE_H__)
#define __LOVENOTE_H__

/*---------------------------------------------------------------------------*
 * Love Note
 *---------------------------------------------------------------------------*/
struct lovenotetype_ {
    ushort  love_code;
    ushort  love_id;
    ushort  data_length;
    ushort  free_q_ptr;
    ushort  data1;
    ushort  data2;
    ulong   buffer_ptr;
};


/*---------------------------------------------------------------------------*
 *  The existing IPs want to continue to use the existing bit-encoding
 *  scheme in the love_code field.
 *---------------------------------------------------------------------------*/
#define LOVE_CODE_PANIC               0x8000 /* unusual condition detected */
#define LOVE_CODE_CARRIER_CHANGE      0x4000 /* carrier detect changed state */
#define LOVE_CODE_FIP_PHY_B           0x2000 /* status change on phy_b */
#define LOVE_CODE_FIP_PHY_A           0x1000 /* status change on phy_a */
#define LOVE_CODE_FIP_PHYA_JOIN       0x800  /* FDDIT, uCode CMT Join */
#define LOVE_CODE_FIP_PHYB_JOIN       0x400  /* FDDIT, uCode CMT Join */
#define LOVE_CODE_FIP_PHYA_SEND_BREAK 0x200
#define LOVE_CODE_FIP_PHYB_SEND_BREAK 0x100
#define LOVE_CODE_FIP_DUP_ADDRESS     0x80
#define LOVE_CODE_LOOP                0x0800 /* loopback is being requested */
#define LOVE_CODE_TRIP_STATUS         0x0400 /* token ring status change */

#endif  /* !defined(__LOVENOTE_H__) */

