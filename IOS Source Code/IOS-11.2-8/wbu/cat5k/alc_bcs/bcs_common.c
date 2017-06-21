/* $Id: bcs_common.c,v 1.1.4.1 1996/05/09 14:54:08 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_bcs/bcs_common.c,v $
 *-----------------------------------------------------------------
 * bcs_common.c -- Boot Code Service (BCS) common code.
 *		Code common to Client and Server sides of BCS.
 *
 *              To ensure backward compatibility with older Boot code,
 *              ADD but do NOT CHANGE any definitions given here.
 *
 * January 10, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bcs_common.c,v $
 * Revision 1.1.4.1  1996/05/09  14:54:08  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:13:37  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.2  1996/04/26  05:51:17  rlowe
 * Treat compiler warnings as errors.  Eliminate such warnings from
 * current ACP source.  Revise LCP handshake pulse width to be closer
 * to 1.0 usec.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:01  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:09:35  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:40:32  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:04  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.7.2.1.4.1  1996/01/18  05:02:28  rlowe
 * Manufacturability changes to bootrom and flash boot code.
 *
 * Branch: ATM21_branch
 *
 * Revision 1.7.2.1  1995/11/15  23:14:19  rbau
 * merge mainline code changes in 1.4 branch
 *
 * Revision 1.8  1995/11/15  21:21:44  rbau
 * enable support for downloading off-line diag sar code
 *
 * Revision 1.7  1995/11/02  04:00:47  rlowe
 * Add (but disable) support for Tx/Rx SAR manufacturing loopback diag code.
 *
 * Revision 1.6  1995/10/13  06:06:56  rlowe
 * Enable and use BCS checksums in PROM and flash Boots.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "acp_basic_types.h"
#include "lo_wdog.h"
#include "acp_hw_defs.h"
#include "acp_c_defs.h"
#include "acp_image_map.h"
#include "bcs_api.h"
#include "bcs_image_hdr.h"
#include "bcs_commonIF.h"


/* --------------------------------------------------------------------	*/
/*	Identify the non-path part of the filename given by a BCS Hdr.	*/
/* --------------------------------------------------------------------	*/
u_int8 * BCS_ShortFileName (register u_int32 * pHdr)
{
	register u_int8 *	pName ;


		/* ROM and flash version records are special cases. */
	if ((pHdr [BCS_IHI_IMAGE_TYPE] == BCS_IHT_PROM_VINFO) &&
	    (pHdr [BCS_IHI_IMAGE_FLAGS] & BCS_IH_FG_INPROM))
		pName = "ROM version" ;

	else  if ((pHdr [BCS_IHI_IMAGE_TYPE] == BCS_IH_TYPE_VINFO) &&
		  ! (pHdr [BCS_IHI_IMAGE_FLAGS] & BCS_IH_FG_INPROM))
		pName = "flash version" ;

	else	/* Normal sub-image header record. */
	{
		pName = (u_int8 *) strrchr
				((u_int8 *)(pHdr [BCS_IHI_IMAGE_NAME]), '/') ;

		if (pName == NULL)
			pName = (u_int8 *)(pHdr [BCS_IHI_IMAGE_NAME]) ;
		else
			++ pName ;		/* Step past '/'. */
	}

	return (pName) ;

}	/* BCS_ShortFileName */

/* --------------------------------------------------------------------	*/
/*	Dump a particular BCS header for debug purposes.		*/
/* --------------------------------------------------------------------	*/
void BCS_DumpHdr (u_int8 * pTag, register u_int32 * pHdr)
{
	PUNCH_WD ;
	if (! C_CHK_JMPR (ACP_RRB_JMP_DEBUG_))
		return ;

#ifdef	DEBUG_BCS
	if (pTag == NULL)
		pTag = "Dumping" ;

	buginf ("\n%s BCS hdr @ $%08x:\n", pTag, pHdr) ;

	buginf ("\tflag: %08x  pres: %08x  invl: %08x  hver: %08x\n",
		pHdr [BCS_IHI_FLAG_PATTERN], pHdr [BCS_IHI_IMG_PRESENT_],
		pHdr [BCS_IHI_IMG_INVALID_], pHdr [BCS_IHI_HDR_VERSION]) ;

	buginf ("\ttype: %08x  flgs: %08x  fadr: %08x  flen: %08x\n",
		pHdr [BCS_IHI_IMAGE_TYPE], pHdr [BCS_IHI_IMAGE_FLAGS],
		pHdr [BCS_IHI_FLASH_BASE], pHdr [BCS_IHI_FSH_IMAGE_LEN]) ;

	buginf ("\tradr: %08x  rlen: %08x  entr: %08x  srvc: %08x\n",
		pHdr [BCS_IHI_RAM_BASE], pHdr [BCS_IHI_RAM_IMAGE_LEN],
		pHdr [BCS_IHI_IMAGE_ENTRY], pHdr [BCS_IHI_IMAGE_SVC]) ;

	buginf ("\tIchk: %08x  prop: %08x  spr1: %08x  Hchk: %08x\n",
		pHdr [BCS_IHI_IMAGE_CHK], pHdr [BCS_IHI_IMAGE_PROP],
		pHdr [BCS_IHI_SPARE_1], pHdr [BCS_IHI_HDR_CHK]) ;

	buginf ("\tName: %s\n", pHdr [BCS_IHI_IMAGE_NAME]) ;
	buginf ("\tVers: %s\n", pHdr [BCS_IHI_IMAGE_VERSION]) ;
	buginf ("\tDate: %s\n", pHdr [BCS_IHI_IMAGE_DATE]) ;
	buginf ("\tInst: %s\n", pHdr [BCS_IHI_INSTALL_DATE]) ;
#endif
}	/* BCS_DumpHdr */

/* --------------------------------------------------------------------	*/
/*	Does additional checks for image headers beyond baseline	*/
/*	version.  Just a stub for now.					*/
/* --------------------------------------------------------------------	*/
u_int32 BCS_LkHighVersChks (u_int32 * pHdr, u_int32 CtlBits)
{
	u_int32	RC = FALSE ;


	switch (pHdr [BCS_IHI_HDR_VERSION])
	{
	    case BCS_IH_BASE_VERSION:
	    default:
		RC = TRUE ;		/* Stub to be forward-compatible. */
		break ;
	}

	return (RC) ;

}	/* BCS_LkHighVersChks */

/* --------------------------------------------------------------------	*/
/*	Checks that a given address is within Flash area.		*/
/*	For ACP rev-1, this is above the flash segments allocated	*/
/*	to Primary Boot (ie. PROM) code.				*/
/*	For ACP rev-2, this is for all of Flash (PROM is separate).	*/
/* --------------------------------------------------------------------	*/
u_int32 BCS_LkChkFlashAddr (u_int32 Addr, u_int32 fOverride)
{
	if (Addr == BCS_IH_ILLEGAL_ADDR)
		return (fOverride) ;

	else if ((Addr >= (ACP_HW_FLASH_BASE)) &&
			(Addr < (ACP_HW_FLASH_BASE + ACP_HW_FLASH_SIZE)))
		return (TRUE) ;

#ifdef	ACP_REV1_HW
	else if ((Addr >= (ACP_HW_PROM_BASE)) &&
			(Addr < (ACP_HW_PROM_BASE + ACP_HW_PROM_SIZE)))
		return (TRUE) ;
#endif

	else return (FALSE) ;

}	/* BCS_LkChkFlashAddr */

/* --------------------------------------------------------------------	*/
/*	Checks that a given address is within TxSAR shared RAM area.	*/
/* --------------------------------------------------------------------	*/
u_int32 BCS_LkChkTSarAddr (u_int32 Addr, u_int32 fOverride)
{
	if (Addr == BCS_IH_ILLEGAL_ADDR)
		return (fOverride) ;

	else if ((Addr >= (ACP_HW_TSAR_RAM_BASE)) &&
			(Addr < (ACP_HW_TSAR_RAM_BASE + ACP_HW_TSAR_RAM_SIZE)))
		return (TRUE) ;
	
	else return (FALSE) ;

}	/* BCS_LkChkTSarAddr */

/* --------------------------------------------------------------------	*/
/*	Checks that a given address is within RxSAR shared RAM area.	*/
/* --------------------------------------------------------------------	*/
u_int32 BCS_LkChkRSarAddr (u_int32 Addr, u_int32 fOverride)
{
	if (Addr == BCS_IH_ILLEGAL_ADDR)
		return (fOverride) ;

	else if ((Addr >= (ACP_HW_RSAR_RAM_BASE)) &&
			(Addr < (ACP_HW_RSAR_RAM_BASE + ACP_HW_RSAR_RAM_SIZE)))
		return (TRUE) ;
	
	else return (FALSE) ;

}	/* BCS_LkChkRSarAddr */

/* --------------------------------------------------------------------	*/
/*	Checks that a given address is within ACP DRAM area.		*/
/* --------------------------------------------------------------------	*/
u_int32 BCS_LkChkDramAddr (u_int32 Addr, u_int32 fOverride)
{
	if (Addr == BCS_IH_ILLEGAL_ADDR)
		return (fOverride) ;

	else if ((Addr >= (ACP_HW_DRAM_BASE)) &&
			(Addr < (ACP_HW_DRAM_BASE + ACP_HW_DRAM_SIZE)))
		return (TRUE) ;
	
	else if ((Addr >= (ACP_HW_NCRAM_BASE)) &&
			(Addr < (ACP_HW_NCRAM_BASE + ACP_HW_NCRAM_SIZE)))
		return (TRUE) ;
	
	else return (FALSE) ;

}	/* BCS_LkChkDramAddr */

/* --------------------------------------------------------------------	*/
/*	Assuming a valid flash image header, this routine validates	*/
/*	the image body in flash.  Return value indicates success (T)	*/
/*	or failure (F).							*/
/* --------------------------------------------------------------------	*/
u_int32 BCS_LkValidIBody (u_int32 * pHdr, u_int32 CtlBits, u_int32 fAmBoot)
{
	u_int32	RC ;
	u_int32	CS ;
	u_int8	fSkipIt ;


	do {
		fSkipIt = TRUE ;
		RC      = TRUE ;

		if (! (CtlBits & (BCSL_BF_DO_CKSUM)))
			break ;

			/* Flash BCS version info record has no body. */
		else  if ((pHdr [BCS_IHI_IMAGE_TYPE] == BCS_IH_TYPE_VINFO) &&
			  (! (pHdr [BCS_IHI_IMAGE_FLAGS] & BCS_IH_FG_INPROM)))
			break ;

			/* PROM BCS version info record has no body. */
		else  if ((pHdr [BCS_IHI_IMAGE_TYPE] == BCS_IHT_PROM_VINFO) &&
			  (pHdr [BCS_IHI_IMAGE_FLAGS] & BCS_IH_FG_INPROM))
			break ;


		fSkipIt = FALSE ;
		RC      = FALSE ;

		if (pHdr [BCS_IHI_FLASH_BASE] == BCS_IH_ILLEGAL_ADDR)
			break ;

		else	/* Validate checksum. */
		{
			CS = BCS_CheckSum ((u_int32 *)
						(pHdr [BCS_IHI_FLASH_BASE]),
						pHdr [BCS_IHI_FSH_IMAGE_LEN],
						BCS_CHKSUM_SEED,
						fAmBoot) ;

			RC = (CS == pHdr [BCS_IHI_IMAGE_CHK]) ;

#ifdef	DEBUG_BCS
			buginf
		    ("Image cksum %s: hdr $%08x, calc $%08x, expected $%08x\n",
				(RC ? "passed" : "FAILED"),
				pHdr, CS, pHdr [BCS_IHI_IMAGE_CHK]) ;
#endif
		}

	} while (FALSE) ;

		/* Do not give trace output if checksum calc was skipped. */
	if (! fSkipIt)
		buginf ("Image body checksum for $%08x (%s) %s\n",
			pHdr [BCS_IHI_FLASH_BASE], BCS_ShortFileName (pHdr),
			(RC ? "passed" : "FAILED")) ;

	return (RC) ;

}	/* BCS_LkValidIBody */

/* --------------------------------------------------------------------	*/
/*	This routine validates a flash header.  It does this using	*/
/*	a number of consistency checks as well as a checksum.		*/
/* --------------------------------------------------------------------	*/
u_int32 BCS_LkValidIHdr (register u_int32 * pHdr, u_int32 CtlBits,
				u_int32 I_Type, u_int32 fAmBoot)
{
	register u_int32	Pass ;
	u_int32			Valu ;


    Pass = FALSE ;

    do {
	if (pHdr == NULL)
		break ;

	BCS_DumpHdr ("BCS_LkValidIHdr:", pHdr) ;

		/* Are we checking FIRST Flash BCS header record ? */
	Valu = ((I_Type == BCS_IH_TYPE_2ndBT) ?
				(BCS_IH_BOOT_FPAT) : (BCS_IH_IMAGE_FPAT)) ;

		/* Check values that are (in-)valid on their face. */
                /* Most "image flags" are interpreted at use-time. */
                /* However, always check here for PROM image flag. */
	if (	(pHdr [BCS_IHI_FLAG_PATTERN] != Valu               ) ||
		(pHdr [BCS_IHI_IMG_PRESENT_] != BCS_IH_PRESENT_    ) ||
		(pHdr [BCS_IHI_IMG_INVALID_] != BCS_IH_VALID_IMAGE ) ||
		(pHdr [BCS_IHI_IMAGE_TYPE  ] != I_Type             ) ||
        	(pHdr [BCS_IHI_IMAGE_FLAGS] & BCS_IH_FG_INPROM)       )
		break ;

		/* Check pointers and sizes in flash to be in range. */
	if (	(! BCS_LkChkFlashAddr (pHdr [BCS_IHI_FLASH_BASE], FALSE))    ||
		(pHdr [BCS_IHI_FSH_IMAGE_LEN] > (ACP_HW_FLASH_SIZE))         ||
		(! BCS_LkChkFlashAddr (pHdr [BCS_IHI_IMAGE_NAME], FALSE))    ||
		(! BCS_LkChkFlashAddr (pHdr [BCS_IHI_IMAGE_VERSION], FALSE)) ||
		(! BCS_LkChkFlashAddr (pHdr [BCS_IHI_IMAGE_DATE], FALSE))    ||
		(! BCS_LkChkFlashAddr (pHdr [BCS_IHI_INSTALL_DATE], FALSE))   )
		break ;

		/* For ACP code, check entry/service entry point addresses. */
		/* All ACP images must have an entry point, only Secondary  */
		/* Boot image has a service point (at this time).           */
		/* For forward-compatibility, do NOT check SAR entry points.*/
		/* Do NOT use a switch statement here; need breaks to take  */
		/* us out of outer loop !!                                  */
		/* BCS Version header record never checked for entry point. */
	if (pHdr [BCS_IHI_IMAGE_TYPE] == BCS_IH_TYPE_2ndBT)
	{
		if (  (! BCS_LkChkFlashAddr (pHdr [BCS_IHI_IMAGE_ENTRY], FALSE))
		   || (! BCS_LkChkFlashAddr (pHdr [BCS_IHI_IMAGE_SVC], FALSE))
		   || (! BCS_LkChkDramAddr  (pHdr [BCS_IHI_RAM_BASE], TRUE))
		   || (pHdr [BCS_IHI_RAM_IMAGE_LEN] > (ACP_HW_DRAM_SIZE)))
			break ;
	}
	else  if (pHdr [BCS_IHI_IMAGE_TYPE] == BCS_IH_TYPE_IOSBT)
	{
		if (  (! BCS_LkChkFlashAddr (pHdr [BCS_IHI_IMAGE_ENTRY], FALSE))
		   || (! BCS_LkChkFlashAddr (pHdr [BCS_IHI_IMAGE_SVC], TRUE))
		   || (! BCS_LkChkDramAddr  (pHdr [BCS_IHI_RAM_BASE], TRUE))
		   || (pHdr [BCS_IHI_RAM_IMAGE_LEN] > (ACP_HW_DRAM_SIZE)))
			break ;
	}
	else if (pHdr [BCS_IHI_IMAGE_TYPE] == BCS_IH_TYPE_IOSKR)
	{
		if (  (! BCS_LkChkDramAddr (pHdr [BCS_IHI_IMAGE_ENTRY], FALSE))
		   || (! BCS_LkChkDramAddr (pHdr [BCS_IHI_IMAGE_SVC], TRUE))
		   || (! BCS_LkChkDramAddr (pHdr [BCS_IHI_RAM_BASE], FALSE))
		   || (pHdr [BCS_IHI_RAM_IMAGE_LEN] > (ACP_HW_DRAM_SIZE)))
			break ;
	}
	else  if ((pHdr [BCS_IHI_IMAGE_TYPE] == BCS_IH_TYPE_TXS_PMON) ||
		  (pHdr [BCS_IHI_IMAGE_TYPE] == BCS_IH_TYPE_TXS_PUD)  ||
		  (pHdr [BCS_IHI_IMAGE_TYPE] == BCS_IH_TYPE_TXS_ODIAG)||
		  (pHdr [BCS_IHI_IMAGE_TYPE] == BCS_IH_TYPE_TXS_RT))
	{
		if (  (! BCS_LkChkTSarAddr (pHdr [BCS_IHI_IMAGE_ENTRY], TRUE))
		   || (! BCS_LkChkTSarAddr (pHdr [BCS_IHI_IMAGE_SVC], TRUE))
		   || (! BCS_LkChkTSarAddr (pHdr [BCS_IHI_RAM_BASE], FALSE))
		   || (pHdr [BCS_IHI_RAM_IMAGE_LEN] > (ACP_HW_TSAR_RAM_SIZE)))
			break ;
	}
	else  if ((pHdr [BCS_IHI_IMAGE_TYPE] == BCS_IH_TYPE_RXS_PMON) ||
		  (pHdr [BCS_IHI_IMAGE_TYPE] == BCS_IH_TYPE_RXS_PUD)  ||
		  (pHdr [BCS_IHI_IMAGE_TYPE] == BCS_IH_TYPE_RXS_ODIAG)||
		  (pHdr [BCS_IHI_IMAGE_TYPE] == BCS_IH_TYPE_RXS_RT))
	{
		if (  (! BCS_LkChkRSarAddr (pHdr [BCS_IHI_IMAGE_ENTRY], TRUE))
		   || (! BCS_LkChkRSarAddr (pHdr [BCS_IHI_IMAGE_SVC], TRUE))
		   || (! BCS_LkChkRSarAddr (pHdr [BCS_IHI_RAM_BASE], FALSE))
		   || (pHdr [BCS_IHI_RAM_IMAGE_LEN] > (ACP_HW_RSAR_RAM_SIZE)))
			break ;
	}
	else  if (pHdr [BCS_IHI_IMAGE_TYPE] == BCS_IH_TYPE_VINFO)
	{
		if (  (! BCS_LkChkFlashAddr (pHdr [BCS_IHI_IMAGE_ENTRY], TRUE))
		   || (! BCS_LkChkFlashAddr (pHdr [BCS_IHI_IMAGE_SVC], TRUE))
		   || (! BCS_LkChkDramAddr  (pHdr [BCS_IHI_RAM_BASE], TRUE))
		   || (pHdr [BCS_IHI_RAM_IMAGE_LEN] > (ACP_HW_DRAM_SIZE)))
			break ;
	}

		/* Check the image header checksum, and the image property. */
	Valu = BCS_ImageProperty (pHdr) ;
#ifdef	DEBUG_BCS
	buginf ("BCS_LkValidIHdr: calc = $%08x, Prop = $%08x\n",
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
	buginf ("BCS_LkValidIHdr: calc = $%08x, Hchk = $%08x\n",
		Valu, pHdr [BCS_IHI_HDR_CHK]) ;
#endif
	if (pHdr [BCS_IHI_HDR_CHK] != Valu)
		break ;

		/* Do additional or supplemental checks. */
	if (! BCS_LkHighVersChks (pHdr, CtlBits))
		break ;

	Pass = TRUE ;

    } while (FALSE) ;

    buginf ("Flash hdr lookup for $%08x (%s) %s\n",
	    pHdr, BCS_ShortFileName (pHdr), (Pass ? "passed" : "FAILED"));

    return (Pass) ;

}	/* BCS_LkValidIHdr */
