/* $Id: bcs_api.h,v 1.1.4.1 1996/05/09 14:55:29 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/bcs_api.h,v $
 *-----------------------------------------------------------------
 * bcs_api.h -- Boot Code Service (BCS) API.
 *		To ensure backward compatibility with older Boot code,
 *		ADD but do NOT CHANGE any definitions given here.
 *
 * January 23, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bcs_api.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:29  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:58  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:51  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:06  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:58  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:26  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.14.2.1  1995/11/15  23:23:14  rbau
 * merge mainline code changes in 1.4 branch
 *
 * Revision 1.15  1995/11/15  21:25:34  rbau
 * enable support for downloading off-line diags sar code
 *
 * Revision 1.14  1995/11/02  04:00:42  rlowe
 * Add (but disable) support for Tx/Rx SAR manufacturing loopback diag code.
 *
 * Revision 1.13  1995/10/13  06:06:53  rlowe
 * Enable and use BCS checksums in PROM and flash Boots.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__BCS_API_H
#define	__BCS_API_H

	/* ------------------------------------------------------------	*/
	/*	Following are various flag values and return codes.	*/
	/* ------------------------------------------------------------	*/
#define	BCS_IH_ILLEGAL_ADDR	(0xffffffff)

#define	BCSSR_SUCCESS		(0)
#define	BCSSR_FAILURE		(BCS_IH_ILLEGAL_ADDR)


	/* ------------------------------------------------------------	*/
	/*	Following are PROM image types.  Always add to end.	*/
	/* ------------------------------------------------------------	*/
#define	BCS_IHT_PROM_FIRST	(0x1)
#define	BCS_IHT_PROM_1stBT	(BCS_IHT_PROM_FIRST)
#define	BCS_IHT_PROM_MFGDG	(0x2)
#define	BCS_IHT_PROM_VINFO	(0x3)	/* Version info BCS record. */
#define	BCS_IHT_PROM_LAST	(BCS_IHT_PROM_VINFO)

	/* ------------------------------------------------------------	*/
	/*	Following are FLASH image types.  Always add to end.	*/
	/* ------------------------------------------------------------	*/
#define	BCS_IH_TYPE_FIRST	(0x1)
#define	BCS_IH_TYPE_2ndBT	(BCS_IH_TYPE_FIRST)
#define	BCS_IH_TYPE_IOSBT	(0x2)
#define	BCS_IH_TYPE_TXS_PMON	(0x3)
#define	BCS_IH_TYPE_RXS_PMON	(0x4)
#define	BCS_IH_TYPE_TXS_PUD	(0x5)
#define	BCS_IH_TYPE_RXS_PUD	(0x6)
#define	BCS_IH_TYPE_TXS_RT	(0x7)
#define	BCS_IH_TYPE_RXS_RT	(0x8)
#define	BCS_IH_TYPE_IOSKR	(0x9)
#define	BCS_IH_TYPE_VINFO	(0xa)	/* Version info BCS record. */

	/* ------------------------------------------------------------	*/
	/*	Next two are added to support off-line manufacturing	*/
	/*	diags for post-FCS release.				*/
	/* ------------------------------------------------------------	*/
#define	BCS_IH_TYPE_TXS_ODIAG	(0xb)
#define	BCS_IH_TYPE_RXS_ODIAG	(0xc)
#define	BCS_IH_TYPE_LAST	(BCS_IH_TYPE_RXS_ODIAG)

	/* ------------------------------------------------------------	*/
	/*	Following are sizes of various (null-terminated)	*/
	/*	character strings pointed to by the Image Header.	*/
	/*	Pick sizes that are whole long-words for checksumming.	*/
	/* ------------------------------------------------------------	*/
#define	BCS_IHL_DATE_TIME	(32)	/* Size of date/time strings.    */
#define	BCS_IHL_IMAGE_VERSION	(16)	/* Size of image version string. */
#define	BCS_IHL_IMAGE_NAME	(128)	/* Size of image name string.    */


	/* ------------------------------------------------------------	*/
	/*	Following are standard service request types.		*/
	/*	Unimplementated features always return failure value.	*/
	/* ------------------------------------------------------------	*/
#define	BCSSR_FIRST		(0)
#define	BCSSR_RUN_BOOT		(BCSSR_FIRST)		/* Fixed in PROM Boot.*/
#define	BCSSR_WALK_IMAGES	(BCSSR_RUN_BOOT + 1)	/* Fixed in PROM Boot.*/
#define	BCSSR_BOOT_EXTENSION	(BCSSR_WALK_IMAGES + 1)	/* Fixed in PROM Boot.*/

#define	BCSSR_GET_IMAGE_HDR	(BCSSR_BOOT_EXTENSION + 1)
#define	BCSSR_GET_IMG_HDR_SIZE	(BCSSR_GET_IMAGE_HDR + 1)
#define	BCSSR_GET_FLASH_BASE	(BCSSR_GET_IMG_HDR_SIZE + 1)
#define	BCSSR_GET_FLASH_SIZE	(BCSSR_GET_FLASH_BASE + 1)
#define	BCSSR_GET_RAM_BASE	(BCSSR_GET_FLASH_SIZE + 1)
#define	BCSSR_GET_RAM_SIZE	(BCSSR_GET_RAM_BASE + 1)
#define	BCSSR_GET_ENTRY_ADDR	(BCSSR_GET_RAM_SIZE + 1)
#define	BCSSR_GET_SERVICE_ADDR	(BCSSR_GET_ENTRY_ADDR + 1)
#define	BCSSR_COPY_IMAGE	(BCSSR_GET_SERVICE_ADDR + 1) /* Copy to RAM. */
#define	BCSSR_RUN_ENTRY_PT	(BCSSR_COPY_IMAGE + 1)
#define	BCSSR_RUN_VEC_TABL	(BCSSR_RUN_ENTRY_PT + 1)
#define	BCSSR_RUN_SERVICE_REQ	(BCSSR_RUN_VEC_TABL + 1)
#define	BCSSR_RUN_DIAGS		(BCSSR_RUN_SERVICE_REQ + 1)
#define	BCSSR_DOWNLOAD_IMAGE	(BCSSR_RUN_DIAGS + 1)	/* Download to FLASH. */
#define	BCSSR_SCRATCH_IMAGE	(BCSSR_DOWNLOAD_IMAGE + 1)
#define	BCSSR_READ_IMAGE_NAME	(BCSSR_SCRATCH_IMAGE + 1)
#define	BCSSR_READ_IMAGE_VERS	(BCSSR_READ_IMAGE_NAME + 1)
#define	BCSSR_READ_IMAGE_DATE	(BCSSR_READ_IMAGE_VERS + 1)
#define	BCSSR_READ_INSTALL_DATE	(BCSSR_READ_IMAGE_DATE + 1)
#define	BCSSR_NBR_AVAIL		(BCSSR_READ_INSTALL_DATE + 1)


	/* ------------------------------------------------------------	*/
	/*	Following are version codes for the BCS API itself.	*/
	/* ------------------------------------------------------------	*/
#define	BCS_API_REV_0		(0)


	/* ------------------------------------------------------------	*/
	/*	Following are standard bitfield definitions for		*/
	/*	"CtlBits" parameter in service requests.		*/
	/*	Recommend allocate flags from MSB->LSB, and policy	*/
	/*	types from LSB->MSB.					*/
	/* ------------------------------------------------------------	*/
#define	BCSL_BF_NONE		(0)
#define	BCSL_BF_DO_CKSUM	(0x80000000)


	/* ------------------------------------	*/
	/*	C code definitions follow.	*/
	/* ------------------------------------	*/
#ifndef	ASM_MODE_FLG

#ifndef	_ACP_BASIC_TYPES_H
#include "acp_basic_types.h"
#endif

	/* ------------------------------------------------------------	*/
	/* Typical call:						*/
	/*	pFcn = BCS_FindSvc (u_int32 API_Rev, u_int32 fAmBoot,	*/
	/*					u_int32 CtlBits) ;	*/
	/* ------------------------------------------------------------	*/
extern p_void BCS_FindSvc () ;

	/* ------------------------------------------------------------	*/
	/* Typical call:						*/
	/*	pFcn = BCS_Service (u_int32 API_Rev, u_int32 fAmBoot,	*/
	/*			u_int32 SvcReq, u_int32 ImageType,	*/
	/*			u_int32 CtlBits <, optional args>) ;	*/
	/*								*/
	/* When reading date/name/version strings from image header,	*/
	/* two optional args must be supplied:  user buffer pointer	*/
	/* and user buffer size.					*/
	/* ------------------------------------------------------------	*/
extern p_void BCS_Service () ;

	/* ------------------------------------------------------------	*/
	/* Typical call:						*/
	/*	pChr = BCSp_VerifyProm () ;				*/
	/* ------------------------------------------------------------	*/
extern u_int8 * BCSp_VerifyProm () ;
#endif


#endif	/* __BCS_API_H */
