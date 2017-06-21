/* $Id: dfs_vip_idb.h,v 3.8.6.7 1996/08/16 08:02:35 dlobete Exp $
 * $Source: /release/112/cvs/Xsys/src-vip/dfs_vip_idb.h,v $
 *------------------------------------------------------------------
 *
 * Includes for manipulating hwidbs for DFS.
 *
 * Aug 1995, Don Lobete
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dfs_vip_idb.h,v $
 * Revision 3.8.6.7  1996/08/16  08:02:35  dlobete
 * CSCdi59971:  DACL needs modularity work
 * Branch: California_branch
 * For the sake of future maintenance of IP access-lists, share as much
 * source code as possible between VIP and RSP.
 *
 * Revision 3.8.6.6  1996/08/12  06:12:45  dlobete
 * CSCdi59677:  distributed switching broken if MIP queuing fixed
 * Branch: California_branch
 * Eliminate mci_index from DFS scope and replace with the more
 * suitable hw_if_index. This not only fixes DFS to MIP channel-groups
 * but also paves the way for input from the new channelized port-adaptors
 * into DFS.
 *
 * Revision 3.8.6.5  1996/05/08  01:10:04  gchristy
 * CSCdi56944:  Compilation errors from IP named access list commit
 * Branch: California_branch
 * - Modularize new ACL code for inclusion in VIP images.
 *
 * Revision 3.8.6.4  1996/04/29  04:46:56  dlobete
 * CSCdi54163:  Distributed-switched packets discarded on ATM interface
 * Branch: California_branch
 *
 * Revision 3.8.6.3  1996/04/11  00:32:27  dlobete
 * CSCdi50256:  DFS displays incorrect statistics for output MIP interface
 * Branch: California_branch
 *
 * Revision 3.8.6.2  1996/04/10  03:39:02  dlobete
 * CSCdi53121:  Distributed IP Access Lists need to run at interrupt-level
 * Branch: California_branch
 *
 * Revision 3.8.6.1  1996/03/21  23:52:06  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.3  1996/03/18  23:36:19  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.2  1996/02/21  20:38:03  getchell
 * CSCdi49474:  Time to turn on IPC/DFS on viper
 * Also integrated patch from dlobete that allows dfs across mic rel
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1996/02/08  09:31:11  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.8  1996/03/06  21:28:02  dlobete
 * CSCdi49840:  Support for 32 MB DRAM required for DFS on RVIP
 * Make DFS dynamically react to DRAM size and grab cache memory
 * accordingly.
 *
 * Revision 3.7  1996/03/04  07:14:38  kao
 * CSCdi49674:  While VIP is cofigured with DFS, OIR event will crash the
 * VIP;
 *
 * Revision 3.6  1996/03/01  20:55:55  dlobete
 * CSCdi49838:  Illegal malloc at interrupt-level may happen for DFS
 * Pre-initialize the hwidb_array[] for DFS and while we're here
 * clean up some affected DFS statistics code.
 *
 * Revision 3.5  1996/02/18  19:42:38  kao
 * CSCdi45136:  EOIR Problems with VIP card
 *
 * Revision 3.4  1996/01/24  20:24:03  dlobete
 * CSCdi47211:  IP DFS cache operations stall with traffic load on RVIP
 * Use the right callback routine configuration to truly run IPC
 * at interrupt level (and thus DFS cache updates). Now the DFS cache
 * will be updated in a snappier way when the RSP IP cache changes.
 *
 * Revision 3.3  1996/01/19  03:03:35  dlobete
 * CSCdi44954:  Local-switching for DFS required
 * But that's not all! Also part of this jumbo Distributed IP
 * Switching commit:
 * - CSCdi43302: Illegal Access to Low Address crashes RVIP/4E (RSP2)
 * - DFS config which mirrors RSP route-cache setting
 *         (ie configurable distributed flow or optimal switching)
 * - Interrupt level IPC updates and invalidates for DFS cache operations
 * - Stay in closer synch with RSP cache for prefix invalidations
 *
 * Revision 3.2  1995/11/17  18:51:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:24  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/26  05:44:23  dlobete
 * DFS support.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define MAX_CTRLR_INTS	32	/* as defined for RSP */
#ifndef MAX_SLOTS		/* will be defined for slave RSP */
#define	MAX_SLOTS	13
#endif

#define DFS_NAMESTRING_LGTH 	80

#define DFS_STATS_INTERVAL  	10000000       /* 10 seconds in usecs */
#define RSP_MAX_IPC_BUFF_SIZE   4096

#define DFS_8MB_SIZE	       	0x800000
#define DFS_16MB_SIZE         	0x1000000
#define DFS_32MB_SIZE		0x2000000		
#define DFS_64MB_SIZE         	0x4000000

#define FOR_ALL_DFSIDBS(dhw) \
for ((dhw) = hwidb_list; (dhw); (dhw) = (dhw)->next)

/*
 * A structure used to represent other hwidbs in the
 * router housing this VIP; just a subset of the real hwidbtype.
 */
typedef struct dfsidbtype_ {
   struct dfsidbtype_ *next;
   char *hw_namestring;
   enum FSTYPE fast_switch_type;
   enum IDBTYPE	type;
   qa_hwq_t *tx_q_ptr;
   qa_acc_t *tx_acc_ptr;
   ulong ip_fast_flags;
   int ip_routecache;
   int hw_if_index;
   ulong slot_and_unit;
   ulong rx_cumbytes_auto[ACCT_PROTO_LAST];
   ulong inputs_auto[ACCT_PROTO_LAST];
   ulong tx_cumbytes_auto[ACCT_PROTO_LAST];
   ulong outputs_auto[ACCT_PROTO_LAST];
   ulong outputdrops;
   ulong input_drops;
   acl_headertype *ip_output_accesslist;
   ulong queue_algorithm;
   hwidbtype *hwidb;		/* pointer to full hwidb */
   fastsend_t local_sw_vector;  /* pointer to local hiwdb fastsend vector */
   boolean dfs_throttled;	/* TRUE means local-switching is off */
} dfsidbtype;


extern dfsidbtype *hwidb_array[];
extern dfsidbtype *hwidb_list;
extern ushort hwidb_counter;

extern dfsidbtype * dfs_vip_get_hwidb(ushort hw_if_index);
extern hwidbtype * dfs_get_local_hwidb(ushort slot_unit, short vc);
extern void dfs_vip_delete_hwidb(ushort hw_if_index);
extern void dfs_vip_delete_hwidb_all(void);
extern void dfs_check_periodic_tasks(void);
extern void dfs_vip_quiesce (void);
