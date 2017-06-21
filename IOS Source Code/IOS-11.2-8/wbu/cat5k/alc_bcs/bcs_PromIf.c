/* $Id: bcs_PromIf.c,v 1.1.4.1 1996/05/09 14:54:06 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_bcs/bcs_PromIf.c,v $
 *-----------------------------------------------------------------
 * bcs_PromIf.h -- Boot Code Service (BCS) PROM API.
 *		Adaptation of BCS specifically for verifying PROM.
 *		This code is separate and distinct from normal BCS
 *		client/server stuff.
 *
 * October 11, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bcs_PromIf.c,v $
 * Revision 1.1.4.1  1996/05/09  14:54:06  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:13:35  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.2  1996/04/26  05:51:16  rlowe
 * Treat compiler warnings as errors.  Eliminate such warnings from
 * current ACP source.  Revise LCP handshake pulse width to be closer
 * to 1.0 usec.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:37:59  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:09:33  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:40:31  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:03  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.1.8.1  1996/01/18  05:02:27  rlowe
 * Manufacturability changes to bootrom and flash boot code.
 *
 * Branch: ATM21_branch
 *
 * Revision 1.1  1995/10/13  06:24:06  rlowe
 * Enable and use BCS checksums in PROM and flash Boots.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "acp_basic_types.h"
#include "lo_wdog.h"
#include "acp_hw_defs.h"
#include "acp_c_defs.h"
#include "bcs_api.h"
#include "bcs_image_hdr.h"
#include "bcs_commonIF.h"


extern u_int32	EndImage ;

/* --------------------------------------------------------------------	*/
/*	Checks that a given address is within ACP PROM area.		*/
/* --------------------------------------------------------------------	*/
u_int32 BCS_ChkPromAddr (u_int32 Addr, u_int32 fOverride)
{
	if (Addr == BCS_IH_ILLEGAL_ADDR)
		return (fOverride) ;

	else if ((Addr >= (ACP_HW_PROM_BASE)) &&
		 (Addr < (ACP_HW_PROM_BASE + ACP_HW_PROM_SIZE)))
		return (TRUE) ;

	else	return (FALSE) ;

}	/* BCS_ChkPromAddr */

/* --------------------------------------------------------------------	*/
/*	This routine validates a flash header.  It does this using	*/
/*	a number of consistency checks as well as a checksum.		*/
/* --------------------------------------------------------------------	*/
u_int32 BCS_ValidPromHdr (register u_int32 * pHdr, u_int32 CtlBits,
				u_int32 I_Type, u_int32 fAmBoot)
{
	register u_int32	Pass ;
	u_int32			Valu ;


    Pass = FALSE ;

    do {
	if (pHdr == NULL)
		break ;

	BCS_DumpHdr ("BCS_ValidPromHdr:", pHdr) ;

		/* Watch for FIRST PROM BCS header record. */
	if (I_Type == BCS_IHT_PROM_1stBT)
		Valu = BCS_IH_PROM_FPAT ;

		/* Expect a normal BCS header record. */
	else	Valu = BCS_IH_IMAGE_FPAT ;


		/* Check values that are (in-)valid on their face. */
		/* Most "image flags" are interpreted at use-time. */
		/* However, always check here for PROM image flag. */
	if (	(pHdr [BCS_IHI_FLAG_PATTERN] != Valu               ) ||
		(pHdr [BCS_IHI_IMG_PRESENT_] != BCS_IH_PRESENT_    ) ||
		(pHdr [BCS_IHI_IMG_INVALID_] != BCS_IH_VALID_IMAGE ) ||
		(pHdr [BCS_IHI_IMAGE_TYPE  ] != I_Type             ) ||
		(! (pHdr [BCS_IHI_IMAGE_FLAGS] & BCS_IH_FG_INPROM))   )
		break ;

		/* Check pointers and sizes in PROM to be in range. */
	if (	(! BCS_ChkPromAddr (pHdr [BCS_IHI_FLASH_BASE], FALSE))    ||
		(pHdr [BCS_IHI_FSH_IMAGE_LEN] > (ACP_HW_PROM_SIZE))       ||
		(! BCS_ChkPromAddr (pHdr [BCS_IHI_IMAGE_NAME], FALSE))    ||
		(! BCS_ChkPromAddr (pHdr [BCS_IHI_IMAGE_VERSION], FALSE)) ||
		(! BCS_ChkPromAddr (pHdr [BCS_IHI_IMAGE_DATE], FALSE))    ||
		(! BCS_ChkPromAddr (pHdr [BCS_IHI_INSTALL_DATE], FALSE))   )
		break ;

		/* Check entry and service point addresses to be in PROM. */
                /* BCS Version header record never checked for entry point. */
	Valu = (pHdr [BCS_IHI_IMAGE_TYPE] == BCS_IHT_PROM_VINFO) ;
	if (  (! BCS_ChkPromAddr (pHdr [BCS_IHI_IMAGE_ENTRY], Valu))
	   || (! BCS_ChkPromAddr (pHdr [BCS_IHI_IMAGE_SVC], TRUE))
	   || (! BCS_LkChkDramAddr  (pHdr [BCS_IHI_RAM_BASE], TRUE))
	   || (pHdr [BCS_IHI_RAM_IMAGE_LEN] > (ACP_HW_DRAM_SIZE)))
		break ;

	
		/* Check the image header checksum, and the image property. */
	Valu = BCS_ImageProperty (pHdr) ;
#ifdef	DEBUG_BCS
	buginf ("BCS_ValidPromHdr: calc = $%08x, Prop = $%08x\n",
		Valu, pHdr [BCS_IHI_IMAGE_PROP]) ;
#endif
	if ((pHdr [BCS_IHI_IMAGE_PROP] != Valu) || (Valu == 0))
		break ;


		/* Header checksum includes string data.  Assume */
		/* that all relevant string data is contiguous.  */
	Valu = BCS_CheckSum (pHdr,
				((BCS_IHI_HDR_CHK - BCS_IHI_FLAG_PATTERN) << 2),
				BCS_CHKSUM_SEED,
				fAmBoot) ;
	Valu = BCS_CheckSum ((u_int32 *) (pHdr [BCS_IHI_IMAGE_NAME]),
				pHdr [BCS_IHI_INSTALL_DATE]
					+ (BCS_IHL_DATE_TIME)
					- pHdr [BCS_IHI_IMAGE_NAME],
				Valu, fAmBoot) ;
#ifdef	DEBUG_BCS
	buginf ("BCS_ValidPromHdr: calc = $%08x, Hchk = $%08x\n",
		Valu, pHdr [BCS_IHI_HDR_CHK]) ;
#endif
	if (pHdr [BCS_IHI_HDR_CHK] != Valu)
		break ;

		/* Do additional or supplemental checks. */
	if (! BCS_LkHighVersChks (pHdr, CtlBits))
		break ;

	Pass = TRUE ;

    } while (FALSE) ;

    buginf ("ROM hdr lookup for $%08x (%s) %s\n",
	    pHdr, BCS_ShortFileName (pHdr), (Pass ? "passed" : "FAILED"));

    return (Pass) ;

}	/* BCS_ValidPromHdr */

/* --------------------------------------------------------------------	*/
/*	Find the BCS headers in PROM, then walk and verify them.	*/
/*	Returns ptr to version string on success, NULL on failure.	*/
/* --------------------------------------------------------------------	*/

u_int8 * BCSp_VerifyProm ()
{
	register u_int32 *	pHdr ;
	register u_int32 *	pVer ;
	register u_int32	I_Type ;
	register u_int32	H_Type ;


		/* ----------------------------------------------------	*/
		/*	Locate PROM BCS headers just below the first	*/
		/*	PROM segment boundary.  Hunt for the magic	*/
		/*	value delimiting the first header.		*/
		/* ----------------------------------------------------	*/
	PUNCH_WD ;
	for (pHdr = (u_int32 *) (ACP_HW_PROM_SEGSIZE) ; pHdr >= & EndImage ; )
		if (* -- pHdr == (BCS_IH_PROM_FPAT))
			break ;

	PUNCH_WD ;
	buginf ("ROM image directory lookup ") ;
	if (*pHdr != (BCS_IH_PROM_FPAT))
	{
		buginf ("FAILED\n") ;
		return (NULL) ;
	}
	else	buginf ("passed at $%08x\n", pHdr) ;


		/* ----------------------------------------------------	*/
		/*	Walk/verify PROM-resident BCS headers & images.	*/
		/*	Ensure that ALL headers are for PROM images.	*/
		/*	Loop over all possible PROM headers, not all	*/
		/*	may be present.					*/
		/* ----------------------------------------------------	*/
	PUNCH_WD ;
	pVer = NULL ;
	for (I_Type = BCS_IHT_PROM_FIRST ; I_Type <= BCS_IHT_PROM_LAST ;
								++ I_Type)
	{
		H_Type = pHdr [BCS_IHI_IMAGE_TYPE] ;
		if (! BCS_ValidPromHdr (pHdr, BCSL_BF_DO_CKSUM, H_Type, TRUE))
			return (NULL) ;

		if (! BCS_LkValidIBody (pHdr, BCSL_BF_DO_CKSUM, TRUE))
			return (NULL) ;

			/* Note if a BCS version record was found. */
		if (H_Type == BCS_IHT_PROM_VINFO)
			pVer = pHdr ;

			/* BCS array may be sparsely populated. */
		if (H_Type == BCS_IHT_PROM_LAST)
			break ;

		PUNCH_WD ;
		pHdr += BCS_IHI_BOOT_USED ;
	}

		/* Watch out if BCS array did NOT end properly or */
		/* if a BCS version record was NOT found.         */
	PUNCH_WD ;
	buginf ("ROM image directory completeness check ") ;
	if ((I_Type > BCS_IHT_PROM_LAST) || (pVer == NULL))
	{
		buginf ("FAILED\n") ;
		return (NULL) ;
	}
	else	buginf ("passed\n") ;

	PUNCH_WD ;
	return ((u_int8 *) (pVer [BCS_IHI_IMAGE_VERSION])) ;

}	/* BCSp_VerifyProm */
