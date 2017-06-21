/* $Id: ipfast_rsp_ipc.h,v 3.4.10.5 1996/08/12 06:12:22 dlobete Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast_rsp_ipc.h,v $
 *------------------------------------------------------------------
 * IPC Distributed Fast-Switching Application RSP defines
 *
 * July 1995, Don Lobete and Darren Kerr 
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast_rsp_ipc.h,v $
 * Revision 3.4.10.5  1996/08/12  06:12:22  dlobete
 * CSCdi59677:  distributed switching broken if MIP queuing fixed
 * Branch: California_branch
 * Eliminate mci_index from DFS scope and replace with the more
 * suitable hw_if_index. This not only fixes DFS to MIP channel-groups
 * but also paves the way for input from the new channelized port-adaptors
 * into DFS.
 *
 * Revision 3.4.10.4  1996/06/05  05:08:57  kao
 * CSCdi53846:  EOIR of EIP wipes route cache of entries for that interface
 * No cache invalidation if reset during EOIR
 * Populate txq and txacc down to all IPs after memd recarve for those
 * DFS caches pending on new txq and txacc
 * Branch: California_branch
 *
 * Revision 3.4.10.3  1996/06/04  05:04:04  kao
 * CSCdi57759:  VIP2 system exception & DBUS-3-DBUSINTERR error when doing
 * eoir.
 * move all DFS/IPC related variables and routines to rsp_ipc_cbus.c
 * Branch: California_branch
 *
 * Revision 3.4.10.2  1996/06/01  16:03:14  kao
 * CSCdi57759:  VIP2 system exception & DBUS-3-DBUSINTERR error when doing
 * eoir.
 * Flush DFS/IPC message before memd recarve to have a consistent DFS
 * cache after EOIR.
 * Branch: California_branch
 *
 * Revision 3.4.10.1  1996/05/04  01:04:12  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - ARP, IP protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on CRB/IRB is enabled or not.
 *
 * Constrained Multicast Flooding
 * - Convert tbridge flood array to a linked list of tbifd's.
 *
 * Name Access List
 *
 * Revision 3.4  1996/02/24  02:14:35  dkerr
 * CSCdi49787:  IPC rolls over and dies under load
 * Make IPC more robust in an RSP->VIP distributed switching environment
 * (and increase performance from the miserly 100 msgs per minute to
 * something respectable, like 300K msgs per second).
 *
 * Revision 3.3  1996/01/19  03:02:54  dlobete
 * CSCdi44954:  Local-switching for DFS required
 * But that's not all! Also part of this jumbo Distributed IP
 * Switching commit:
 * - CSCdi43302: Illegal Access to Low Address crashes RVIP/4E (RSP2)
 * - DFS config which mirrors RSP route-cache setting
 *         (ie configurable distributed flow or optimal switching)
 * - Interrupt level IPC updates and invalidates for DFS cache operations
 * - Stay in closer synch with RSP cache for prefix invalidations
 *
 * Revision 3.2  1995/11/17  09:35:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:57:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:04:58  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/26  05:50:04  dlobete
 * Placeholders for VIP_branch development.
 *------------------------------------------------------------------
 * $Endlog$
 */

extern void ipc_ipfast_rsp_send_add (ipcache_rntype *entry, paktype *pak,
				     ipaddrtype destination);

extern void ipc_ipfast_rsp_send_invalid(ipcache_rntype *entry);

extern void ipc_send_enable_ds(hwidbtype *hwidb);

extern void ipc_ipfast_rsp_init(void);

extern void ipc_ipfast_rsp_access_change(ushort add_notdelete,
					 acl_headertype *acl,
					 void *fh_p, void *fi_p,
					 void *si_p); 

extern void ipc_ipfast_rsp_acl_intface_all(void);

extern void ipc_access_group_callback(idbtype *idb, boolean groupin,
				      acl_headertype *acl);

extern void populate_txq_txacc_to_ips(void);

extern boolean rsp_ipc_send_buffer(void);

extern void ip_ipc_flush_msg(void);

extern void *ip_ipc_get_msg(int size);

extern void ip_ipc_send_msg(int size, ipc_message_type type, boolean flush);
