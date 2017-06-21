/* $Id: bsc_cmd_set.h,v 3.2 1995/11/17 08:48:04 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/bstun/bsc_cmd_set.h,v $
 *------------------------------------------------------------------
 * BSC command set definitions.
 *
 * February 1995, Andrew R. Rothwell.
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bsc_cmd_set.h,v $
 * Revision 3.2  1995/11/17  08:48:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/16  06:24:21  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:13:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __BSC_CMD_SET_H
#define __BSC_CMD_SET_H

/*---- Exported constants. */

/*---- Pass through command set belongs here. */

/*---- The local-ack command set. */

#define NULL_CMD           0x10
#define POLL_ADDR_ADD      0x11
#define POLL_ADDR_DELETE   0x12
#define POLL_ADDR_ACTIVE   0x13
#define POLL_ADDR_INACTIVE 0x14
#define POLL_DATA_ETB      0x15
#define POLL_DATA_ETX      0x16
#define SPLIT_POLL_DATA    0x17
#define DEVICE_STATUS_DATA 0x18
#define SELECT_DATA_ETB    0x19
#define SELECT_DATA_ETX    0x1a
#define SPLIT_SELECT_DATA  0x1b
#define POLL_DATA_ABORT    0x1c
#define SEL_DATA_ABORT     0x1d
#define DEVICE_DO_WACK     0x1e
#define DEVICE_DONT_WACK   0x1f

#endif  /* __BSC_CMD_SET_H. */
/*---- End include file: bsc_cmd_set.h. */
