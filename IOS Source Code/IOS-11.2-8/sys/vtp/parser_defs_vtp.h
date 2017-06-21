/* $Id: parser_defs_vtp.h,v 1.1.40.2 1996/06/20 19:50:13 cyoung Exp $
 * $Source: /release/112/cvs/Xsys/vtp/parser_defs_vtp.h,v $
 *------------------------------------------------------------------
 * parser_defs_vtp.h - Parser interface header file for VTP.
 *
 * Dec 1995, Dean Cheng 
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_vtp.h,v $
 * Revision 1.1.40.2  1996/06/20  19:50:13  cyoung
 * Use correct VLAN count to calculate TFTP file size
 * CSCdi60853:  wrong behaviour while reading tftp file --> vlan info
 * recovery fail Branch: California_branch
 * Also while bonnet is up: Put in a limiting depth in for waiting for
 * semaphore to be released.  Allow null domain to receive domain change
 * notifications.  Provide function to manually load TFTP database and
 * load the database not only when TFTP server or file change but also
 * VTP entity state (client --> server).
 *
 * Revision 1.1.40.1  1996/05/09  14:48:20  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.1.70.1  1996/04/27  07:38:22  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.58.2  1996/04/16  18:58:56  cyoung
 * Using 'exit','end' or '^Z' as an implicit apply breaks when the parser
 * needs to pop out of the sub-command scope to check against the global
 * parse chain.  Make 'apply' an explicit command to avoid this blunder.
 * CSCdi54003:  write to tftp file when it shouldnt, doesnt when it should
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.58.1  1996/04/08  02:42:11  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.46.2  1996/03/25  02:21:39  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.46.1  1996/03/22  09:44:51  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.8.5  1996/03/22  16:22:08  cyoung
 * MIB: allow TR types to be created, check range on newly created vlan
 *      ids, add trap cli commands
 * VTP core: replace REV_LESS macro with function, include notification
 *           for deleted domain
 * VTP dep: clean up after deleted trunks and domains (including startup
 *          time), add some code for vlan modification notification.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.1.8.4  1996/03/22  03:16:01  bbenson
 * Branch: LE_Cal_V111_1_0_3_branch
 * Rearrange cadillac fins on the VTP command parser.  Install
 * electroglide subcommand modes for vtp-domain and vtp-database commands.
 * Running lights coming in next commit.
 *
 * Revision 1.1.8.3  1996/03/07  02:34:58  cyoung
 * Added vtp network command.
 * Cleanup database subcommand looping.
 * Don't require ring keyword for fddi type vlans.
 * Choose better values for default VLAN fields.
 * Assign domain index before inserting a new domain into the list.
 * Assign correct state when going through vtp_edit_vlan.
 * Add support for removing configuration as a result of VLAN deletion.
 * Rename spanning tree defines to match the MIB.
 * nv_gen support for all VTP commands.
 * set the parent_valid field correctly.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.1.8.2  1996/03/05  01:54:37  cyoung
 * Add vtp configured interface command.  Provide function to retrieve
 * SNMP stats. Fix problem which tries to access NULL pointer when
 * adding a domain after all have been deleted.  Make vtp_upload_info
 * more useful.  Put in beginning TFTP support.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.1.8.1  1996/02/27  21:20:16  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.10.2  1996/01/24  00:36:47  percyk
 * Branch: LE_Cal_V111_0_16_branch
 * add new cmd for vtp.
 *
 * Revision 3.1.10.1  1996/01/11  01:47:36  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.1.2.2  1996/01/03  01:48:48  dcheng
 * Submit CLI code for VTP.
 * Branch: LE_Calif_branch
 *
 * Revision 3.1.2.1  1995/12/23  02:46:49  dcheng
 * Branch: LE_Calif_branch
 *
 * Revision 3.1  1995/12/23  02:43:58  dcheng
 * Placeholder.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define SHOW_VTP_STATUS             0
#define SHOW_VTP_TRUNK              (SHOW_VTP_STATUS+1)
#define SHOW_VTP_DATABASE           (SHOW_VTP_TRUNK+1)
#define SHOW_VTP_STATISTICS         (SHOW_VTP_DATABASE+1)
#define SHOW_VTP_PRUNING            (SHOW_VTP_STATISTICS+1)
#define SHOW_VTP_PRUNING_VLAN       (SHOW_VTP_PRUNING+1)

#define LOAD_VTP_DATABASE           0
/* 
 * VTP config command categories 
 */
enum vtp_global_config_enums {
    VTP_DOMAIN,
    VTP_DOMAIN_PASSWORD,
    VTP_DOMAIN_INTERVAL,
    VTP_DOMAIN_PRUNING,
    VTP_DOMAIN_SERVER,
    VTP_DOMAIN_CLIENT,
    VTP_NETWORK,
    VTP_DATABASE,
    VTP_DATABASE_VLAN,
    VTP_DATABASE_EXIT,
    VTP_DATABASE_ABORT,
    VTP_DATABASE_APPLY
};

/* flags for VLAN optional parameters */
#define VTP_VLAN_NAME              0x0001              
#define VTP_VLAN_SAID              0x0002               
#define VTP_VLAN_STATE             0x0004               
#define VTP_VLAN_MTU               0x0008               
#define VTP_VLAN_RING              0x0010               
#define VTP_VLAN_PARENT            0x0020               
#define VTP_VLAN_BRIDGE            0x0040               
#define VTP_VLAN_STP               0x0080               
#define VTP_VLAN_TLB1              0x0100               
#define VTP_VLAN_TLB2              0x0200               

/* VTP VLAN type */
#define VTP_VLAN_ETH_TYPE          1
#define VTP_VLAN_FDDI_TYPE         2
#define VTP_VLAN_TR_TYPE           3
#define VTP_VLAN_FDDI_NET_TYPE     4
#define VTP_VLAN_TR_NET_TYPE       5

/* VTP VLAN state */
#define VTP_VLAN_OPERATIONAL_STATE 1
#define VTP_VLAN_SUSPENDED_STATE   2

/* STP type for VLAN of fddi/tr net */
#define VTP_VLAN_STP_SRT           1
#define VTP_VLAN_STP_SRB           2

/* VTP trunk command flags */
#define VTP_TRUNK               0x01
#define VTP_CONFIGURED          0x02
#define VTP_TRUNK_PRUNING       0x04
 







