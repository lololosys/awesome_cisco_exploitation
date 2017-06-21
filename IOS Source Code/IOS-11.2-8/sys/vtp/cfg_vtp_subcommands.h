/* $Id: cfg_vtp_subcommands.h,v 1.1.60.1 1996/05/09 14:48:17 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/vtp/cfg_vtp_subcommands.h,v $
 *------------------------------------------------------------------
 * cfg_vtp_subcommands.h - CLI header file for VTP database subcommands.
 *
 * Jan 1996, Dean Cheng 
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_vtp_subcommands.h,v $
 * Revision 1.1.60.1  1996/05/09  14:48:17  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.1.88.1  1996/04/27  07:38:19  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.78.1  1996/04/08  02:42:08  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.66.2  1996/03/25  02:21:38  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.38.5  1996/03/22  03:15:59  bbenson
 * Branch: LE_Cal_V111_1_0_3_branch
 * Rearrange cadillac fins on the VTP command parser.  Install
 * electroglide subcommand modes for vtp-domain and vtp-database commands.
 * Running lights coming in next commit.
 *
 * Revision 1.1.38.4  1996/03/13  17:27:49  cyoung
 * Get rid of vtp_edit_vlan, use download instead.
 * Add vtp_store_domain_info, one stop database storing.
 * Put in semaphore to avoid saving database being loaded in.
 * More TFTP support.
 * Add initial code for 'vtp configured' action routines.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.1.38.3  1996/03/07  02:34:57  cyoung
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
 * Revision 1.1.38.2  1996/03/05  21:42:54  cyoung
 * Make the new CLI commands more civilized.  Start to add basic nvgen
 * support for VTP commands.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.1.38.1  1996/02/27  21:20:13  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.1.6.3  1996/02/09  01:09:54  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Add tlb vlan support.  Various misc bug fixes.
 *
 * Revision 1.1.6.2  1996/01/23  00:47:09  percyk
 * Branch: LE_Cal_V111_0_16_branch
 * add default vlans at startup time.  add meat to debug files.
 *
 * Revision 1.1.6.1  1996/01/11  01:47:45  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.1.2.1  1996/01/03  01:24:47  dcheng
 * Initial commit.
 * Branch: LE_Calif_branch
 *
 * Revision 1.1  1996/01/03  01:19:17  dcheng
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Parse chains were split into cfg_vtp_db_subcommands.h for the
 * database subcommands and cfg_vtp_dom_subcommands.h for the domain
 * subcommands.
 */
