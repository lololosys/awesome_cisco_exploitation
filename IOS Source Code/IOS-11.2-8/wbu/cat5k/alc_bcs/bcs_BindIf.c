/* $Id: bcs_BindIf.c,v 1.1.4.1 1996/05/09 14:54:06 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_bcs/bcs_BindIf.c,v $
 *-----------------------------------------------------------------
 * bcs_BindIf.c -- Boot Code Service (BCS) Client API.
 *		Called get a handle to BCS, specifically the BCS
 *		service entry point.  Runs in caller's environment.
 *
 *              To ensure backward compatibility with older Boot code,
 *              ADD new APIs but do NOT CHANGE any existing ones.
 *
 * January 10, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bcs_BindIf.c,v $
 * Revision 1.1.4.1  1996/05/09  14:54:06  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:13:34  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:37:58  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:09:32  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:40:30  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:02  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.3.6.1  1996/01/18  05:02:26  rlowe
 * Manufacturability changes to bootrom and flash boot code.
 *
 * Branch: ATM21_branch
 *
 * Revision 1.3  1995/10/13  06:06:55  rlowe
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
#include "cbi_defs.h"
#include "libcom.h"
#include "varargs.h"


/* --------------------------------------------------------------------	*/
/*	BCS lookup API, to return the BCS service entry point address.	*/
/*	This function is expected to be called by Boot code, so it must	*/
/*	punch the HW watchdog periodically.  This routine returns	*/
/*		BCSSR_FAILURE on any failure.				*/
/*									*/
/*	Optionally, the	image body can be validated against a good	*/
/*	matching header.  This routine is callable by Primary Boots,	*/
/*	and thus should NOT be changed lightly.				*/
/*	Typical call:							*/
/*		pFcn = BCS_FindSvc (u_int32 API_Rev, u_int32 fAmBoot,	*/
/*						u_int32 CtlBits) ;	*/
/* --------------------------------------------------------------------	*/
/*VARARGS*/
p_void BCS_FindSvc (va_alist)
	va_dcl
{
	va_list		pvar ;
	p_void		pRC ;
	u_int32 *	pHdr ;
	u_int32		API_Rev ;
	u_int32		fAmBoot ;
	u_int32		CtlBits ;


	va_start (pvar) ;
	API_Rev = va_arg (pvar, u_int32) ;			/* 1st arg. */
	switch (API_Rev)
	{
	    default:
		fAmBoot = va_arg (pvar, u_int32) ;		/* 2nd arg. */
		CtlBits = va_arg (pvar, u_int32) ;		/* 3rd arg. */
		break ;
	}
	va_end (pvar) ;

	if (fAmBoot)
		PUNCH_WD ;
	pRC  = (p_void) (BCSSR_FAILURE) ;		/* Default RC. */
	pHdr = NULL ;

	/* ------------------------------------------------------------	*/
	/*	Default Policy is to locate Secondary Boot image header	*/
	/*	record at start of containing flash segment -- using	*/
	/* 	segmented flash	devices.  This should correspond to	*/
	/*	start of TEXT section for the Secondary Boot image.	*/
	/* ------------------------------------------------------------	*/
	/*	Post-rev-1 policy is to locate BCS Image Headers at	*/
	/*	start of flash simm.  This may be disjoint from Boot-2.	*/
	/* ------------------------------------------------------------	*/
	/* NB.  Various policies can be indicated via bitflags in the	*/
	/*	"CtlBits" parameter.					*/
	/* ------------------------------------------------------------	*/

	switch (CtlBits)
	{
	    default:
#ifdef	ACP_REV1_HW
		pHdr = (u_int32 *) (BCS_IH_BOOT2_FBASE) ;
#else
		pHdr = (u_int32 *) (ACP_HW_FLASH_BASE) ;
#endif
		break ;
	}

	/* ------------------------------------------------------------	*/
	/* Validate the image header, and as desired, the image body.	*/
	/* ------------------------------------------------------------	*/
	if (! BCS_LkValidIHdr (pHdr, CtlBits, BCS_IH_TYPE_2ndBT, fAmBoot))
		pHdr = NULL ;
	
	else if (! BCS_LkValidIBody (pHdr, CtlBits, fAmBoot))
		pHdr = NULL ;

	/* ------------------------------------------------------------	*/
	/*	Extract service entry point if header/body are good.	*/
	/*	Ensure that the service entry address is in flash area.	*/
	/* ------------------------------------------------------------	*/
	if (pHdr != NULL)
	{
		pRC = (p_void)(pHdr [BCS_IHI_IMAGE_SVC]) ;

		if (! BCS_LkChkFlashAddr ((u_int32)(pRC), FALSE))
		{
			buginf ("Code server entry check FAILED\n") ;
			pRC = (p_void) (BCSSR_FAILURE) ;
		}
		else	buginf ("Code server entry check passed\n") ;
	}

	if (fAmBoot)
		PUNCH_WD ;

	return (pRC) ;

}	/* BCS_FindSvc */

/* --------------------------------------------------------------------	*/
/*	Do a checksum on some memory region.  Length is given in bytes.	*/
/*	Checksum here is 4-bytes wide; catalyst code uses 1-byte wide.	*/
/*	(qv. btPUD_FlashChkSum and A_ChkSumBody/AR_ChkSumBody.)		*/
/* --------------------------------------------------------------------	*/
u_int32 BCS_CheckSum (register u_int32 * Addr, register u_int32 Len,
			register u_int32 Seed, u_int32 fAmBoot)
{
	if (fAmBoot)
		PUNCH_WD ;


	if (Addr == (u_int32 *)(BCS_IH_ILLEGAL_ADDR))
		return (Seed) ;


		/* Round up to integer number of longwords. */
	Len = ((Len + 3) >> 2) ;

	Seed = checksum (Addr, Len, Seed) ;


	if (fAmBoot)
		PUNCH_WD ;

	return (Seed) ;

}	/* BCS_CheckSum */
