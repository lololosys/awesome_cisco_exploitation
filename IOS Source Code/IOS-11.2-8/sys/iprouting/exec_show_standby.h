/* $Id: exec_show_standby.h,v 3.2.56.2 1996/08/12 16:04:14 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/exec_show_standby.h,v $
 *------------------------------------------------------------------
 * exec_show_standby.h  -- Show commands for hot standby protocol
 *
 * Sep 1993 Tony Li
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_standby.h,v $
 * Revision 3.2.56.2  1996/08/12  16:04:14  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.56.1  1996/05/09  14:31:37  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.82.1  1996/04/27  07:05:31  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.72.1  1996/04/08  01:55:58  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.62.1  1996/03/22  22:52:45  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.48.1  1996/03/05  06:14:49  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.36.1  1996/02/27  21:09:25  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.14.1  1996/01/11  01:45:45  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.10.1  1995/12/29  01:06:11  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  17:33:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:16:22  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:26:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLNS (show_standby_eols, show_standby);

INTERFACE (show_standby_interface, show_standby_eols, 
	   show_standby_eols, OBJ (idb, 1), 
	   (IFTYPE_IEEE | IFTYPE_ATM | IFTYPE_SUBIF));

KEYWORD	(show_standby, show_standby_interface, ALTERNATE, "standby",
	 "Hot standby protocol information", 
	 PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_standby

