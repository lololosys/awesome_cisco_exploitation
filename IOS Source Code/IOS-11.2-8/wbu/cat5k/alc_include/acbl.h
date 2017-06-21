/* $Id: acbl.h,v 1.1.4.1 1996/05/09 14:55:25 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/acbl.h,v $
 *-----------------------------------------------------------------
 * acbl.h -- Synergy ATM line card binary linker header file.
 *		(adapted from NMP/syn_common cbl.h).
 *
 * July 13, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: acbl.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:25  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:53  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:45  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:01  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:52  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:21  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.2  1995/10/04  01:31:01  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.1  1995/07/27  10:42:22  rlowe
 * Initial check-in; header file for ALC binary linker for combined images.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This file contains the description of the Object Control Set and the
 * Object Control Block used by both the Combined Binary Linker and the
 * Run Time Exploder.
 *
 * rfa 950209-28
 *
 * ---------------------------------------------------------------
 * Revision 1.1  1995/07/27  10:42:22  rlowe
 * Initial check-in; header file for ALC binary linker for combined images.
 *
 * Revision 1.1  1995/06/12  22:16:32  rfa
 * Moved from nmpflash/h/cbl.h
 *
 */

#ifndef	__ACBL_H
#define	__ACBL_H

#define ABL_MAGIC	"CATALYST 5000 ATM line card"	/* SYNALC sign. str */
#define ABL_CKSUM_SEED  (0)		       /* Std. checksum init value. */

/*
 * Number of characters available in a tag
 * string including the null terminator.
 */
#define TAGSIZ		32
#define HDRSIZ		256
#define MODSIZ(p)	(HDRSIZ + (p)->size + (p)->pad + sizeof(long))

/*
 * Object Control Set.  There is one of these structures at the
 * front of a Combined Binary Image as produced by the cbl (Glue)
 * utility.
 */
typedef struct ocs
{
	char		tag[TAGSIZ];	/* Synergy signature */
	u_long		Version ;	/* OCS version code. */
	u_long		ExplBase ;	/* Base DRAM address of exploder. */
	u_long		ExplSize ;	/* Size of exploder. */
	u_long		ExplEnAddr ;	/* Entry addr for exploder. */
	u_long		ExplOfst ;	/* File offset to exploder from OCS. */
	u_long		ImageOfst ;	/* File offset to flash images. */
	u_long		BCS_Ofst ;	/* File offset to BCS headers. */
	u_long		cbi_size ;	/* Combined image file size in bytes. */
	u_long		cbi_mods ;	/* Number of load modules in image. */
	u_long		cbi_cksum;	/* Check sum for entire image file. */
	u_long		ocs_cksum;	/* Check sum for this header. */
} ocs_t;

#define	OCS_CUR_VERSION	(0)		/* Current revision of OCS header. */


#define ALIGN(x,n) \
	((*((int *)&(x)) & ((n) - 1)) ? \
		((*((long *)&(x)) + (n)) & ~((n) - 1)) : \
				*((long *)&(x)))

#endif	/* __ACBL_H */
