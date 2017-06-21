/* $Id: ipfast_vip_ipc.h,v 3.3 1996/01/19 03:03:00 dlobete Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/ip/ipfast_vip_ipc.h,v $
 *------------------------------------------------------------------
 * IPC Distributed Fast-Switching Application VIP defines
 *
 * July 1995, Don Lobete and Darren Kerr 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast_vip_ipc.h,v $
 * Revision 3.3  1996/01/19  03:03:00  dlobete
 * CSCdi44954:  Local-switching for DFS required
 * But that's not all! Also part of this jumbo Distributed IP
 * Switching commit:
 * - CSCdi43302: Illegal Access to Low Address crashes RVIP/4E (RSP2)
 * - DFS config which mirrors RSP route-cache setting
 *         (ie configurable distributed flow or optimal switching)
 * - Interrupt level IPC updates and invalidates for DFS cache operations
 * - Stay in closer synch with RSP cache for prefix invalidations
 *
 * Revision 3.2  1995/11/17  09:35:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:57:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:05:03  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/26  05:50:02  dlobete
 * Placeholders for VIP_branch development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern void ipc_ipfast_vip_send_stats(dfsidbtype *hwidb, ushort hwidb_counter,
				ushort protocol_type);

extern void ipc_ipfast_vip_init(void);

extern void ipc_ipfast_get_dfs_cfg(void);
 
extern void ipc_ipfast_get_acl_cfg(void);
