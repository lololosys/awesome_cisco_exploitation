/* $Id: mc68ec030_defs.h,v 1.1.4.1 1996/05/09 14:55:41 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/mc68ec030_defs.h,v $
 *-----------------------------------------------------------------
 * mc68ec030_defs.h -- definitions specific to the Motorola
 *                     MC68EC030.  These definitions can be used by
 *                     both C and assembly code.  Anything C-specific
 *                     is indicated (negatively) with the flag
 *                     ASM_MODE_FLG.
 *
 * November 1994, Rick Lowe
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mc68ec030_defs.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:41  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:10  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:40:07  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:19  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:12  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:40  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__MC68EC030_DEFS_H
#define	__MC68EC030_DEFS_H

	/* ============================================================	*/
	/*	This header file has definitions specific to the	*/
	/*	Motorola MC68EC030.  These definitions can be used	*/
	/*	by both C and assembly code.  Anything C-specific is	*/
	/*	indicated (negatively) with the flag ASM_MODE_FLG.	*/
	/*	By Rick Lowe, cisco, 94-11-16.				*/
	/*	This is:  %W% %G%	*/
	/* ============================================================	*/

#define	CPU030_ENAB_D_CACHE	0x00000100
#define	CPU030_ENAB_I_CACHE	0x00000001
#define	CPU030_ENAB_CACHES	(CPU030_ENAB_D_CACHE+CPU030_ENAB_I_CACHE)
#define	CPU030_DISAB_CACHES	0x00000000

#define	CPU030_FLUSH_D_CACHE	0x00000800
#define	CPU030_FLUSH_I_CACHE	0x00000008
#define	CPU030_FLUSH_CACHES	(CPU030_FLUSH_D_CACHE+CPU030_FLUSH_I_CACHE)


#define	CPU030_ACU_0		(0)
#define	CPU030_ACU_1		(1)
#define	CPU030_DISAB_ACU	0x00000000
#define	CPU030_ACU_DISAB	(CPU030_DISAB_ACU)
#define	CPU030_ACU_ENAB		(1 << 15)
#define	CPU030_ACU_INHIB	(1 << 10)
#define	CPU030_ACU_READ		(1 << 9)
#define	CPU030_ACU_RMSK		(1 << 8)

#define	ACU_ENABLE		(1)
#define	ACU_INHIB_CACHE		(1)
#define	ACU_WRITE_O		(0)
#define	ACU_RDWR		(1)
#define	ACU_READ_O		(2)
#define	ACU_FC_MSK_ONLY		(0)
#define	ACU_FC_MSK_PRIV		(3)
#define	ACU_FC_MSK_CODE		(4)
#define	ACU_FC_MSK_DATA		(4)
#define	ACU_FC_MSK_ANY		(7)
#define	ACU_FLASH_CTL		(CPU030_ACU_1)	/* Used when modifying flash. */


#define	CPU030_FC_UDATA		(1)
#define	CPU030_FC_UCODE		(2)
#define	CPU030_FC_SDATA		(5)
#define	CPU030_FC_SCODE		(6)
#define	CPU030_FC_CPU		(7)


#ifndef	ASM_MODE_FLG
#define	CPU030_ACU_VALUE(ad_b, size, ena, inh, r_w, fc_b, fc_m)       \
				(((ad_b) & 0xff000000)              | \
				 ((((size) - 1) >> 8) & 0x00ff0000) | \
				 ((ena)   ? CPU030_ACU_ENAB  : 0)   | \
				 ((inh)   ? CPU030_ACU_INHIB : 0)   | \
			 	 (((r_w) & 0x03) << 8)              | \
				 (((fc_b) & 0x07) << 4)             | \
				 ((fc_m) & 0x07))

	/* See sys_cache.S for these. */
extern unsigned long	Sys_SetCpuCacheX (unsigned long NewCtl) ;
extern unsigned long	Sys_ModCpuCACR (unsigned long NewCtl,
					unsigned long Mask) ;
extern unsigned long	Sys_SetCpuACUx (unsigned long AcuNbr,
					unsigned long NewACU_Val) ;
extern unsigned long	Sys_ModCpuACUx (unsigned long AcuNbr,
					unsigned long NewACU_Val,
					unsigned long ACU_Mask) ;
#endif

#endif	/* __MC68EC030_DEFS_H */
