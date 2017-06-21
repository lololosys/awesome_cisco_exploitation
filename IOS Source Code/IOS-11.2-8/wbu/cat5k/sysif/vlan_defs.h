/* $Id: vlan_defs.h,v 1.1.4.1 1996/05/09 15:01:56 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/sysif/vlan_defs.h,v $
 *-----------------------------------------------------------------
 * vlan_defs.h -- 
 *
 * February 1995, umahajan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vlan_defs.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:56  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:50  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:43:22  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:14:01  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:44  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:58  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.8  1995/10/04  01:07:33  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.7  1995/07/27  01:25:04  rlowe
 * Add definitions for SYNERGY_MIN_USR_VLAN and SYNERGY_MAX_USR_VLAN
 * so that we know what VLan IDs are in range for user traffic.  Used
 * by ATM line card LCP driver for filtering Set Port State messages
 * (spanning-tree) to give to LANE.
 *
 * Revision 1.6  1995/07/26  03:13:26  rlowe
 * Identify max vlan number (range) used with Synergy (1024).  Assists
 * ACP LCP driver in determining which vlan ids are for internal use.
 *
 * Revision 1.5  1995/07/06  23:06:47  vinod
 * Added SAGE1_FLUSH_VLAN_NUM for SAGE1 bug.
 *
 * Revision 1.4  1995/05/03  17:51:15  vinod
 * Renamed and reserved 3 VLANS for Synergy Diagnostics
 *
 * Revision 1.3  1995/03/09  17:57:14  umahajan
 * Deleted comment concerning vlan 0.
 *
 * Revision 1.2  1995/02/23  21:32:56  umahajan
 * Added SPT_VLAN_NUM 1022 define and reserved it for 10 Base T ports. Cuurent
 * rev of SAINT needs this workaround.
 *
 * Revision 1.1  1995/02/21  17:34:57  umahajan
 * Created.
 *
*
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef _VLAN_DEFS_H
#define _VLAN_DEFS_H


#define INB_VLAN_NUM	0  	/* Inband Vlan */
#define MGMT_VLAN_NUM	1	/* Network Managament VLAN */
#define SAGE1_FLUSH_VLAN_NUM 1018 /* To fix SAGE1 flush bug */
#define SYNDIAG_VLAN_NUM   1019 /*Reserved for Synergy Diags*/
#define SYNDIAG_VLAN_NUM2  1020 /*Reserved for Synergy Diags*/
#define SYNDIAG_VLAN_NUM3  1021 /*Reserved for Synergy Diags*/
#define SPT_VLAN_NUM	1022	/* Reserved for 10 base T ports SPT work around */ 
#define BRDCST_VLAN_NUM	1023	/* Vlan reserved for broadcasting across all Ports */

#define	SYNERGY_MAX_VLAN_NUM	1024	/* Synergy Vlan nbrs fall below this. */
#define	SYNERGY_MIN_USR_VLAN	1	/* Min Vlan nbr available to users.   */
#define	SYNERGY_MAX_USR_VLAN	1000	/* Max Vlan nbr available to users.   */

#endif
