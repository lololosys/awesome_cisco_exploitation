/* $Id: prom.ld,v 1.1.4.1 1996/05/09 15:01:30 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/obj-68-alc/prom.ld,v $
 *-----------------------------------------------------------------
 * prom.ld -- Linker command file for ACP PROM Boot images.
 *
 * October 11, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: prom.ld,v $
 * Revision 1.1.4.1  1996/05/09  15:01:30  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:26  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:54  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:37  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.2  1996/03/20  10:52:51  rlowe
 * Integrate with ios/wbu sub-tree and makefiles.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:38:30  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
# Revision 1.1  1996/03/11  07:44:45  rlowe
# Placeholders for California.
#
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
# Revision 1.1  1995/10/13  06:24:01  rlowe
# Enable and use BCS checksums in PROM and flash Boots.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

TARGET ("srec")
OUTPUT_FORMAT ("srec")
MEMORY
{
	PROM1 : ORIGIN =     0, LENGTH =  1*64K	/* PROM Boot, image headers */
	PROM2 : ORIGIN = 1*64K, LENGTH =  7*64K	/* PROM manufacturing diags */
}
SECTIONS
{
	.boot :			/* ACP PROM Boot */
	{
		acppowbt.srec
		. = ALIGN (4) ;
	} > PROM1

	.hdrs 1*64K-864 :	/* PROM code headers */
	{
		acpprom.hdr
		. = ALIGN (4) ;
	} > PROM1

	.mfgd :			/* PROM mfg diags */
	{
		. = ALIGN (4) ;
	} > PROM2
}
