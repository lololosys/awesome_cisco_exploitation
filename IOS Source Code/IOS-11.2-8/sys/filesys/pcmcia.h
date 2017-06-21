/* $Id: pcmcia.h,v 3.2 1995/11/17 09:08:30 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/filesys/pcmcia.h,v $
 *------------------------------------------------------------------
 * Include file that defines PCMCIA spec things (Like the CIS values)
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pcmcia.h,v $
 * Revision 3.2  1995/11/17  09:08:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:32:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(__pcmcia_h__)
#define __pcmcia_h__

/*----------------------------------------------------------------------------*
 * Index constants for the registers -- these values are written to
 * the index register to access the internal registers via the data
 * register. 
 *----------------------------------------------------------------------------*/
#define CISTPL_NULL           0x00
#define CISTPL_DEVICE         0x01
#define CISTPL_CHECKSUM       0x10
#define CISTPL_LONGLINK_A     0x11
#define CISTPL_LONGLINK_B     0x12
#define CISTPL_LINKTARGET     0x13
#define CISTPL_NO_LINK        0x14
#define CISTPL_VERS_1         0x15
#define CISTPL_ALTSTR         0x16
#define CISTPL_DEVICE_A       0x17
#define CISTPL_JEDEC_C        0x18
#define CISTPL_JEDEC_A        0x19
#define CISTPL_CONFIG         0x1a
#define CISTPL_CFTABLE_ENTRY  0x1b
#define CISTPL_DEVICE_OC      0x1c
#define CISTPL_DEVICE_OA      0x1d
#define CISTPL_DEVICE_GEO     0x1e
#define CISTPL_DEVICE_GEO_A   0x1f
#define CISTPL_MANFID         0x20
#define CISTPL_FUNCID         0x21
#define CISTPL_FUNCE          0x22
#define CISTPL_SWIL           0x23
#define CISTPL_MANFID         0x20
#define CISTPL_VERS_2         0x40
#define CISTPL_FORMAT         0x41
#define CISTPL_GEOMETRY       0x42
#define CISTPL_BYTEORDER      0x43
#define CISTPL_DATE           0x44
#define CISTPL_BATTERY        0x45
#define CISTPL_ORG            0x46
#define CISTPL_END            0xff


#endif /* !defined(__pcmcia_h__) */
