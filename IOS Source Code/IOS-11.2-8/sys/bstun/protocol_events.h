/* $Id: protocol_events.h,v 3.2.60.1 1996/05/17 10:41:19 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/bstun/protocol_events.h,v $
 *------------------------------------------------------------------
 * BSC event types. Major and minor.
 *
 * March 1995, Andrew R. Rothwell.
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: protocol_events.h,v $
 * Revision 3.2.60.1  1996/05/17  10:41:19  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/22  08:57:52  jbalestr
 * CSCdi54541:  system gets stuck in BSC-3-BADLINESTATE
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.1  1996/04/03  13:50:58  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2  1995/11/17  08:49:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/16  06:24:53  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:14:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __PROTOCOL_EVENTS_H
#define __PROTOCOL_EVENTS_H

/*---- Major Events. */

#define PE_HARDWARE 0
#define PE_NETWORK  1
#define PE_TIMER    2

/*---- Minor Events (hardware). */

#define BSC_HW_RX_ACK0    1
#define BSC_HW_RX_ACK1    2
#define BSC_HW_RX_PLSL    3             /* 'addr' _plus_ ENQ. */
#define BSC_HW_RX_ENQ     4             /* Just ENQ. */
#define BSC_HW_RX_EOT     5             /* Just EOT. */
#define BSC_HW_RX_ETB     6
#define BSC_HW_RX_ETX     7
#define BSC_HW_RX_NAK     8
#define BSC_HW_RX_RVI     9
#define BSC_HW_RX_TTD     10
#define BSC_HW_RX_WACK    11
#define BSC_HW_RX_ABORT   12
#define BSC_HW_RX_HANGUP  13            /* DLE _and_ EOT. */
#define BSC_HW_RX_GIANT   14
#define BSC_HW_RX_INVALID 15
#define BSC_HW_TX_COMP    16
#define BSC_HW_LINKDOWN   17
#define BSC_HW_LINKUP     18

#endif  /* __PROTOCOL_EVENTS_H. */
/*---- End include file: bsc_events.h. */
