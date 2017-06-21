/* $Id: rsp_vip.h,v 3.2 1995/11/17 18:49:47 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-rsp/rsp_vip.h,v $
 *------------------------------------------------------------------
 * Versatile Interface Processor support code prototypes - RSP
 *
 * July 1995, David Getchell
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_vip.h,v $
 * Revision 3.2  1995/11/17  18:49:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:28:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:28:51  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/19  15:00:15  getchell
 * Placeholder for VIP development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern void vip_rsp_love_letter(hwidbtype *hwidb, lovenotetype *lovenote);
extern int  vip2standard_types(int pa_type, enum IDBTYPE *idbtype);
extern void vip_ctrlr_init(slottype *slot);
extern void vip_if_init(hwidbtype *hwidb);
