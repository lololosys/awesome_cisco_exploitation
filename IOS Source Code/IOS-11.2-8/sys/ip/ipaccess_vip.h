/* $Id: ipaccess_vip.h,v 3.2.60.7 1996/08/30 16:41:27 drowell Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipaccess_vip.h,v $
 *------------------------------------------------------------------
 *
 * Includes for manipulating IP access lists for DFS.
 *
 * Aug 1995, Don Lobete
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipaccess_vip.h,v $
 * Revision 3.2.60.7  1996/08/30  16:41:27  drowell
 * CSCdi67055:  When VIP1 or VIP2 have 8MB DRAM dont run DFS
 * Branch: California_branch
 * Do not pre-allocate DFS arrays on a 8Meg VIP.
 *
 * Revision 3.2.60.6  1996/08/16  08:02:05  dlobete
 * CSCdi59971:  DACL needs modularity work
 * Branch: California_branch
 * For the sake of future maintenance of IP access-lists, share as much
 * source code as possible between VIP and RSP.
 *
 * Revision 3.2.60.5  1996/06/06  09:24:48  dlobete
 * CSCdi57425:  VIP2 max cache entries wt. 32MB & 64MB are the same
 * Branch: California_branch
 *
 * Revision 3.2.60.4  1996/05/28  16:13:37  xliu
 * CSCdi58788:  code changes for vip named access list and encryption perf
 * Branch: California_branch
 *
 * Revision 3.2.60.3  1996/05/08  01:09:48  gchristy
 * CSCdi56944:  Compilation errors from IP named access list commit
 * Branch: California_branch
 * - Modularize new ACL code for inclusion in VIP images.
 *
 * Revision 3.2.60.2  1996/04/29  04:46:37  dlobete
 * CSCdi54163:  Distributed-switched packets discarded on ATM interface
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/04/10  03:38:21  dlobete
 * CSCdi53121:  Distributed IP Access Lists need to run at interrupt-level
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:34:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:56:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:04:30  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  05:26:27  dlobete
 * Placeholder for VIP_branch development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define IPACCESS_BUFFS_MAX_HUGE		16000
#define IPACCESS_BUFFS_MAX_LARGE	8000
#define IPACCESS_BUFFS_MAX_MEDIUM	4000
#define IPACCESS_BUFFS_MAX_SMALL	0

typedef struct DACL_inputQ_elt_ {
    struct DACL_inputQ_elt_ *next;
    uchar data[0];
} DACL_inputQ_elt;

#define IPACCESS_DATA_SIZE_MAX (sizeof(ipc_ipfast_acc_slowtype))
#define IPACCESS_BUFF_SIZE_MAX (IPACCESS_DATA_SIZE_MAX+sizeof(DACL_inputQ_elt))

extern void acl_intface_process(void *acl_int);
extern void show_acl_intface_settings(acl_headertype *acl);
extern void ipaccess_vip_queue_to_pl(void *message);
extern void ipaccess_vip_init(ulong mem_size);
