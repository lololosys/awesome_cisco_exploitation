/* $Id: bcs_image_hdr.h,v 1.1.4.1 1996/05/09 14:54:10 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_bcs/bcs_image_hdr.h,v $
 *-----------------------------------------------------------------
 * bcs_image_hdr.h -- Boot Code Service (BCS) image header definitions.
 *
 *		To ensure backward compatibility with older Boot code,
 *		ADD but do NOT CHANGE any definitions given here.
 *
 * December 21, 1995, Rick Lowe
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bcs_image_hdr.h,v $
 * Revision 1.1.4.1  1996/05/09  14:54:10  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:13:39  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:04  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:09:37  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:40:35  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:07  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.8  1995/10/13  06:06:58  rlowe
 * Enable and use BCS checksums in PROM and flash Boots.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__BCS_IMAGE_HDR_H
#define	__BCS_IMAGE_HDR_H

	/* ============================================================	*/
	/*	Each header is simply an array of 4-byte items that may	*/
	/*	be well-known to PROM (fixed) Boot.  The parts of the	*/
	/*	header that are known to the PROM Boot must be fixed	*/
	/*	and cannot change (ie. all new fields are added to the	*/
	/*	end of the header).					*/
	/* ============================================================	*/

#ifndef	__ACP_HW_DEFS_H
#include "acp_hw_defs.h"
#endif
#ifndef	__ACP_IMAGE_MAP_H
#include "acp_image_map.h"
#endif

	/* ------------------------------------------------------------	*/
	/* Following are array indices for identifying specific fields. */
	/* The first set of values is known/used by the Primary Boot    */
	/* and so canNOT be changed.  Add new entries at the end.	*/
	/*								*/
	/* The "image property" is a value derived from the image flash	*/
	/* base address, image length, image checksum, image ram addr,	*/
	/* and image entry pt.  It is used to reduce the chance of	*/
	/* random values imitating a valid image header.		*/
	/*								*/
	/* Date/time, and other ASCII, strings are all expected to be	*/
	/* NUL-byte (0) terminated, but this canNOT be guaranteed.      */
	/* ------------------------------------------------------------	*/

#define	BCS_IHI_FLAG_PATTERN	(0)	/* Flags start of image header.  */
#define	BCS_IHI_IMG_PRESENT_	(1)	/* 0 when image written to flash.*/
#define	BCS_IHI_IMG_INVALID_	(2)	/* 0 to invalidate for new image.*/
#define	BCS_IHI_HDR_VERSION	(3)	/* Version of image header.      */
#define	BCS_IHI_IMAGE_TYPE	(4)	/* Type of image in flash.       */
#define	BCS_IHI_IMAGE_FLAGS	(5)	/* Flags about image in flash.   */
#define	BCS_IHI_FLASH_BASE	(6)	/* Image base address in flash.  */
#define	BCS_IHI_FSH_IMAGE_LEN	(7)	/* Compressed Flash image length.*/
#define	BCS_IHI_RAM_BASE	(8)	/* RAM base (copy) addr of image.*/
#define	BCS_IHI_RAM_IMAGE_LEN	(9)	/* Uncompressed RAM image length.*/
#define	BCS_IHI_IMAGE_ENTRY	(10)	/* Image entry point (in RAM).   */
#define	BCS_IHI_IMAGE_SVC	(11)	/* Image service entry point.    */
#define	BCS_IHI_IMAGE_CHK	(12)	/* Check value for image.        */
#define	BCS_IHI_IMAGE_PROP	(13)	/* Property value for image.     */
#define	BCS_IHI_IMAGE_NAME	(14)	/* Ptr to image file name.       */
#define	BCS_IHI_IMAGE_VERSION	(15)	/* Ptr to image version string.  */
#define	BCS_IHI_IMAGE_DATE	(16)	/* Ptr to image build date/time. */
#define	BCS_IHI_INSTALL_DATE	(17)	/* Ptr to install date/time.     */
#define	BCS_IHI_SPARE_1		(18)	/* For expansion use, 1 long.    */
#define	BCS_IHI_HDR_CHK		(19)	/* Check value for image header. */
#define	BCS_IHI_BOOT_USED	(BCS_IHI_HDR_CHK + 1)
					/* Fields known to Primary Boot. */

	/* ------------------------------------------------------------	*/
	/*	Add new image header fields here.			*/
	/* ------------------------------------------------------------	*/

#define	BCS_IHI_END_NBR		(BCS_IHI_BOOT_USED)
					/* Final nbr entries in array.	*/

	/* ------------------------------------------------------------	*/
	/*	Following are values for various image header fields.	*/
	/* ------------------------------------------------------------	*/

#define	BCS_IH_UNBURNED_FIELD	(0xffffffff)
#define	BCS_IH_UNBURNED_STRING	"\377"
#define	BCS_CHKSUM_SEED		(0)

#define	BCS_IH_PROM_FPAT	(0xc01acafe)	/* For ACP 1st Boot. */
#define	BCS_IH_BOOT_FPAT	(0xc1a0be1a)	/* For ACP 2nd Boot. */
#define	BCS_IH_IMAGE_FPAT	(0x1aceb01d)	/* For RAM images.   */
#define	BCS_IH_PRESENT_		(~ BCS_IH_UNBURNED_FIELD)	/* 0 */
#define	BCS_IH_NOT_PRESENT_	(BCS_IH_UNBURNED_FIELD)
#define	BCS_IH_VALID_IMAGE	(BCS_IH_UNBURNED_FIELD)
#define	BCS_IH_INVALID_IMAGE	(~ BCS_IH_UNBURNED_FIELD)	/* 0 */
#define	BCS_IH_BASE_VERSION	(0x1)

#define	BCS_IH_FG_NONE		(0x0)		/* No special image flags. */
#define	BCS_IH_FG_COMPRESS1	(0x01)		/* Image compression type 1. */
#define	BCS_IH_FG_INPROM	(0x02)		/* PROM resident flag. */

#define	BCS_IH_NO_RAM_BASE	(BCS_IH_ILLEGAL_ADDR)
						/* Image not run from RAM. */
#define	BCS_IH_NO_ACP_ENTRY	(BCS_IH_ILLEGAL_ADDR)
						/* Image not run from ACP. */

#define	BCS_NULL_CHKSUM		(BCS_IH_ILLEGAL_ADDR)
#define	BCS_NULL_PAD		(BCS_IH_ILLEGAL_ADDR)
#define	BCS_NULL_PROPERTY	(BCS_IH_ILLEGAL_ADDR)

	/* ------------------------------------------------------------	*/
	/*	Following are standard values for Flash/Ram image	*/
	/*	base addresses and lengths.				*/
	/*	These MUST match those given in the image/image.ld	*/
	/*	GNU-linker script.					*/
	/* ------------------------------------------------------------	*/

#define	BCS_IH_BOOT1_PBASE	(ARAM_1ST_BOOT_TEXT)
#define	BCS_IH_BT1_MFGDG_PBASE	(1 * ACP_HW_PROM_SEGSIZE)

#define	BCS_IH_BOOT2_FBASE	(ARAM_2ND_BOOT_TEXT)
#define	BCS_IH_BOOT2_LEN	(3 * ACP_HW_OFLASH_SEGSIZE)
#define	ACP_BOOT2_DRAM_AREA	(ARAM_2ND_BOOT_DATA)

#define	BCS_IH_IOSBT_FBASE	(ARAM_IOS_BOOT_TEXT)
#define	BCS_IH_IOSBT_LEN	(2 * ACP_HW_OFLASH_SEGSIZE)
#define	BCS_IH_IOSBT_ENTRY	(BCS_IH_IOSBT_FBASE)

#define	BCS_IH_IOSKR_FBASE	(ARAM_IOS_KERN_FLASH)
#define	BCS_IH_IOSKR_RBASE	(ARAM_IOS_KERN_TEXT)
#ifdef	ACP_REV1_HW
#define	BCS_IH_IOSKR_LEN	(24 * ACP_HW_OFLASH_SEGSIZE)	/* worst case */
#else
#define	BCS_IH_IOSKR_LEN	(32 * ACP_HW_OFLASH_SEGSIZE)	/* worst case */
#endif
#define	BCS_IH_IOSKR_ENTRY	(ARAM_IOS_KERN_ENTRY)

#define	BCS_IH_SAR_PMONBASE	(6 * ACP_HW_OFLASH_SEGSIZE)	/* for pmon */
#define	BCS_IH_SAR_BLEN		(2 * ACP_HW_OFLASH_SEGSIZE)
#define	BCS_IH_SAR_SLEN		(ACP_HW_OFLASH_SEGSIZE / 8)
#define	BCS_IH_SAR_PUD_BASE	((8 * ACP_HW_OFLASH_SEGSIZE) -	\
				 (3 * BCS_IH_SAR_SLEN))		/* for pud. */

#define	BCS_IH_TXS_RT_BASE	(BCS_IH_SAR_PUD_BASE + BCS_IH_SAR_SLEN)
#define	BCS_IH_TXSAR_RBASE	(ACP_HW_TSAR_RAM_BASE)

#define	BCS_IH_RXS_RT_BASE	(BCS_IH_TXS_RT_BASE + BCS_IH_SAR_SLEN)
#define	BCS_IH_RXSAR_RBASE	(ACP_HW_RSAR_RAM_BASE)

#endif	/* __BCS_IMAGE_HDR_H */
