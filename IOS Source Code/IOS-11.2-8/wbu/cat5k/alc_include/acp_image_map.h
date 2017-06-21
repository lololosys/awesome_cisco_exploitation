/* $Id: acp_image_map.h,v 1.1.4.1 1996/05/09 14:55:28 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/acp_image_map.h,v $
 *-----------------------------------------------------------------
 * acp_image_map.h -- ACP code image DRAM usage map file.
 *
 * This file is meant to be used with both C and assembly source,
 * so only #define statements are allowed here.
 *
 * December 1994, Rick Lowe
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: acp_image_map.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:28  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:56  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:49  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:05  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:56  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:24  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__ACP_IMAGE_MAP_H
#define	__ACP_IMAGE_MAP_H

	/* ============================================================	*/
	/*	This is the ACP code image DRAM usage map file.		*/
	/*	This file is meant to be used with both C and		*/
	/*	assembly source, so only #define statements are		*/
	/*	allowed here.						*/
	/*	By Rick Lowe, cisco, 94-12-22.				*/
	/*	This is:  %W% %G%	*/
	/* ============================================================	*/

#ifndef	__ACP_HW_DEFS_H
#include "acp_hw_defs.h"
#endif


	/* ============================================================	*/
	/*	Following are DRAM base addresses for various ACP SW.	*/
	/*	Changes here may need to be applied to relevant image	*/
	/*	makefiles and linker scripts as well.			*/
	/* ============================================================	*/

#define	ARAM_END_DRAM		(ACP_HW_DRAM_BASE + ACP_HW_DRAM_SIZE)

#define	ARAM_1ST_BOOT_DATA	(ARAM_END_DRAM      - 0x010000)	/* 0x40ff0000 */
#define	ARAM_2ND_BOOT_DATA	(ARAM_1ST_BOOT_DATA - 0x050000)	/* 0x40fa0000 */
#define	ARAM_EXPLODER_BUFR	(ARAM_2ND_BOOT_DATA - 0x020000)	/* 0x40f80000 */
#define	ARAM_DOWNLOAD_BUFR	(ARAM_EXPLODER_BUFR - 0x400000)	/* 0x40b80000 */

        /* ============================================================	*/
        /*	Following are regions of DRAM that must be set aside.	*/
        /*	Include areas for download buffer, exploder execution	*/
        /*	buffer, Boot data areas.  OS may set some aside beyond	*/
	/*	this too.						*/
        /* ============================================================	*/

#define	ACP_HW_DRAM_HIGH_RZ	(ARAM_DOWNLOAD_BUFR)		/* 0x40b80000 */
#define	ACP_HW_DRAM_SZ_HIGH_RZ	(ARAM_END_DRAM - ACP_HW_DRAM_HIGH_RZ)
								/* 0x00480000 */
 

#define	ARAM_IOS_BOOT_DATA	(ACP_HW_DRAM_BASE + 0x180)	/* 0x40000180 */


#define	ARAM_1ST_BOOT_TEXT	(ACP_HW_PROM_BASE)		/* 0x0        */

#ifdef	ACP_REV1_HW
#define	ARAM_2ND_BOOT_TEXT	(ACP_HW_PROM_BASE + (1 * ACP_HW_PROM_SEGSIZE))
							/* RKL	** 0x00010000 */
#define	ARAM_IOS_BOOT_TEXT	(ACP_HW_PROM_BASE + (4 * ACP_HW_PROM_SEGSIZE))
								/* 0x00040000 */
#define	ARAM_IOS_KERN_FLASH	(ACP_HW_PROM_BASE + (8 * ACP_HW_PROM_SEGSIZE))
								/* 0x00080000 */
#else
#define	ARAM_2ND_BOOT_TEXT	(ACP_HW_FLASH_BASE + (1 * ACP_HW_PROM_SEGSIZE))
							/* RKL	** 0x90010000 */
#define	ARAM_IOS_BOOT_TEXT	(ACP_HW_FLASH_BASE + (4 * ACP_HW_PROM_SEGSIZE))
								/* 0x90040000 */
#define	ARAM_IOS_KERN_FLASH	(ACP_HW_FLASH_BASE)		/* 0x90000000 */

#endif

#define	ARAM_IOS_KERN_TEXT	(ACP_HW_DRAM_BASE + 0x10000)	/* 0x40010000 */
#define	ARAM_IOS_KERN_ENTRY	(ARAM_IOS_KERN_TEXT)		/* 0x40010000 */

	/* ============================================================	*/
	/*	Add any new definitions just above this comment.	*/
	/* ============================================================	*/

#endif	/* __ACP_IMAGE_MAP_H */
