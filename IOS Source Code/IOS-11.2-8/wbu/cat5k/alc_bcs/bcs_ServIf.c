/* $Id: bcs_ServIf.c,v 1.1.4.1 1996/05/09 14:54:07 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_bcs/bcs_ServIf.c,v $
 *-----------------------------------------------------------------
 * bcs_ServIf.h -- Boot Code Service (BCS) Service API.
 *		BCS Service entry point and related code.  Runs in
 *		caller's environment, yet has detailed knowledge of
 *		layout of image(s) packaged with it.
 *
 *              To ensure backward compatibility with older Boot code,
 *              ADD but do NOT CHANGE any definitions given here.
 *
 * January 23, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bcs_ServIf.c,v $
 * Revision 1.1.4.1  1996/05/09  14:54:07  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:13:36  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:00  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:09:34  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:40:31  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:04  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.5.6.1  1996/01/18  05:02:28  rlowe
 * Manufacturability changes to bootrom and flash boot code.
 *
 * Branch: ATM21_branch
 *
 * Revision 1.5  1995/10/13  06:06:56  rlowe
 * Enable and use BCS checksums in PROM and flash Boots.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "acp_basic_types.h"
#include "lo_wdog.h"
#include "lo_memblks.h"
#include "acp_hw_defs.h"
#include "mfp_hw_defs.h"
#include "mfp_acp_defs.h"
#include "acp_c_defs.h"
#include "acp_image_map.h"
#include "bcs_api.h"
#include "bcs_image_hdr.h"
#include "bcs_commonIF.h"
#include "mc68ec030_defs.h"
#include "varargs.h"

const u_int8 BCS_UnknownStr [] = "Unknown" ;	/* Replaces bad hdr strings. */


/* --------------------------------------------------------------------	*/
/*	This function implements policies for finding the image header	*/
/*	records accompanying the BCS.  If found the desired image	*/
/*	header is validated.  NULL is returned if a valid header cannot	*/
/*	be found.  Otherwise, a	pointer to the header record is given.	*/
/*	Optionally, the	image body can be validated against a good	*/
/*	matching header.						*/
/* --------------------------------------------------------------------	*/
u_int32 * BCS_SrchIHdr (u_int32 fAmBoot, u_int32 I_Type, u_int32 CtlBits)
{
	register u_int32 *	pHdr ;
	u_int16			Count ;


	/* ------------------------------------------------------------	*/
	/*	Policy is to allocate one image header per image type	*/
	/*	contained in the flash.  These headers are all located	*/
	/*	contiguously at the start of the flash segment holding	*/
	/*	Secondary Boot/BCS, ie. at start of Boot's TEXT section.*/
	/*	Secondary Boot/BCS header is assumed to be first.	*/
	/* ------------------------------------------------------------	*/
	/*	Post-rev-1 policy is to locate BCS Image Headers at	*/
	/*	start of flash simm.  This may be disjoint from Boot-2.	*/
	/* ------------------------------------------------------------	*/
	/*	NB.  Various policies can be indicated by way		*/
	/*	     of bitflags in "CtlBits" variable.			*/
	/* ------------------------------------------------------------	*/
	/*	Assume that the BCS header is valid, since it was used	*/
	/*	to find the Service entry point that called here.	*/
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

	/* ----------------------------------------------------------------- */
	/*	Current policy has a linear array of headers; may have other */
	/*	policies in future.  Search headers for match on image type. */
	/*	Note that each header is itself an array of longwords.       */
	/* ----------------------------------------------------------------- */

	for (Count = BCS_IH_TYPE_FIRST ; Count <= BCS_IH_TYPE_LAST ; ++ Count)
	{
		/* ----------------------------------------------------	*/
		/* Validate image header, and if desired, image body.	*/
		/* ----------------------------------------------------	*/

		if (pHdr [BCS_IHI_IMAGE_TYPE] == I_Type)
		{
		    if (BCS_LkValidIHdr (pHdr, CtlBits, I_Type, fAmBoot))
			if (BCS_LkValidIBody (pHdr, CtlBits, fAmBoot))
			    return (pHdr) ;	/* Found it. */
		}

		pHdr += BCS_IHI_END_NBR ;	/* Hdr is array of longwords. */
	}

	return (NULL) ;				/* Could not find it. */

}	/* BCS_SrchIHdr */

/* --------------------------------------------------------------------	*/
/*	Function to read date/version/name strings out of image		*/
/*	header and copy into user buffer.  If user provides NULL	*/
/*	buffer pointer, just return address of string in flash.		*/
/*	It is expected that caller is BCS Service entry point with	*/
/*	varargs list of additional arguments:				*/
/*		user buffer pointer, user buffer size.			*/
/* --------------------------------------------------------------------	*/
p_void BCS_ReadHdrString (u_int8 * pStr, u_int32 HdrStrSiz, va_list p_varargs)
{
	register u_int8 *	pBuf ;
	register u_int32	BufSiz ;
	p_void			pRC ;


	pBuf   = va_arg (p_varargs, u_int8 *) ;
	BufSiz = va_arg (p_varargs, u_int32)  ;
	pRC    = (p_void) (BCSSR_FAILURE) ;

	do {
		if (! BCS_LkChkFlashAddr ((u_int32)(pStr), FALSE))
			break ;

		else  if (pBuf == NULL)
		{
			pRC = (p_void)(pStr) ;
			break ;
		}
		else  if (! BCS_LkChkDramAddr ((u_int32)(pBuf), FALSE))
			break ;

		else  if (BufSiz < 2)		/* Illegal case. */
			break ;

		else  if (BufSiz > HdrStrSiz)	/* Do not exceed str or buf. */
			BufSiz = HdrStrSiz ;


		pBuf [-- BufSiz] = '\0' ;	/* For NUL-byte termination. */
		strncpy (pBuf, pStr, BufSiz) ;

			/* Replace string if it is all empty or */
			/* unburned flash bytes.                */
		if (strspn (pBuf, BCS_IH_UNBURNED_STRING) == strlen (pBuf))
			strncpy (pBuf, BCS_UnknownStr, BufSiz) ;

		pRC = (p_void) (BCSSR_SUCCESS) ;

	} while (FALSE) ;

	return (pRC) ;

}	/* BCS_ReadHdrString */

/* --------------------------------------------------------------------	*/
/*	Function to handle walking and verifying all BCS images.	*/
/*	Assume that BCS is embedded within Boot-2, and Boot-2 is	*/
/*	already valid (or else we would not be executing here).		*/
/* --------------------------------------------------------------------	*/
u_int32 BCS_WalkImages (u_int32 fAmBoot, u_int32 CtlBits)
{
	u_int32	I_Type ;


	for (I_Type = BCS_IH_TYPE_FIRST ; I_Type <= BCS_IH_TYPE_LAST ; ++I_Type)
	{
		if (fAmBoot)
			PUNCH_WD ;

			/* Do not bother checking Boot-2/BCS; that is us. */
		if (I_Type == BCS_IH_TYPE_2ndBT)
			continue ;

		if (BCS_SrchIHdr (fAmBoot, I_Type, CtlBits) == NULL)
			return (BCSSR_FAILURE) ;
	}

	return (BCSSR_SUCCESS) ;

}	/* BCS_WalkImages */

/* --------------------------------------------------------------------	*/
/*	BCS Service API, to service various requests for callers.	*/
/*	This function is expected to be called by Boot code, so it must	*/
/*	punch the HW watchdog periodically.  This routine returns	*/
/*		BCSSR_FAILURE on any failure.				*/
/*									*/
/*	This routine is callable by PROM Boots, and thus should NOT	*/
/*	be changed lightly.						*/
/*	Typical call:							*/
/*		pRes = BCS_Service (u_int32 API_Rev, u_int32 fAmBoot,	*/
/*				u_int32 SvcReq,	u_int32 ImageType,	*/
/*				u_int32 CtlBits <, optional args>) ;	*/
/*									*/
/*	When reading date/name/version strings from image header,	*/
/*	two optional args must be supplied:  user buffer pointer	*/
/*	and user buffer size.						*/
/*									*/
/*	Flush the CPU I/D caches whenever we do a jump to some		*/
/*	other code.  It might NOT be located contiguous with our	*/
/*	current Program Counter (PC).  ie. Boot vs. flash vs. dram.	*/
/* --------------------------------------------------------------------	*/
/*VARARGS*/
p_void BCS_Service (va_alist)
	va_dcl
{
	va_list		pvar ;
	p_void		pRC ;
	u_int32 *	pHdr ;
	u_int32		API_Rev ;
	u_int32		fAmBoot ;
	u_int32		SvcReq  ;
	u_int32		ImageType ;
	u_int32		CtlBits ;


	va_start (pvar) ;
	API_Rev = va_arg (pvar, u_int32) ;			/* 1st arg. */
	switch (API_Rev)
	{
	    default:
		fAmBoot   = va_arg (pvar, u_int32) ;		/* 2nd arg. */
		SvcReq    = va_arg (pvar, u_int32) ;		/* 3rd arg. */
		ImageType = va_arg (pvar, u_int32) ;		/* 4th arg. */
		CtlBits   = va_arg (pvar, u_int32) ;		/* 5th arg. */
		break ;
	}

	if (fAmBoot)
		PUNCH_WD ;

	pRC = (p_void) (BCSSR_FAILURE) ;		/* Default return. */

		/* ------------------------------------------------------ */
		/* Following are two special cases to support PROM boots. */
		/* To get trace output to work, we must call IOBT_Init.   */
		/* This is assuming that the MFP/UART has already been    */
		/* initialized by the PROM boots.                         */
		/* ------------------------------------------------------ */

		/* Special-case walk over all BCS images. */
	if (SvcReq == BCSSR_WALK_IMAGES)
	{
	  IOBT_Init () ;
	  puts ("\r\n") ;
	  pRC = (p_void) BCS_WalkImages (fAmBoot, CtlBits) ;
	}
	else  if (SvcReq == BCSSR_BOOT_EXTENSION)
	{
		/* Do nothing special at this time. */
	  IOBT_Init () ;
	  puts ("\r\n") ;
	  pRC = (p_void) (BCSSR_SUCCESS) ;
	}
	else
	{
		/* Find and validate image header of interest.  This will  */
		/* checksum image body if BCSL_BF_DO_CKSUM set in CtlBits. */
		/* This may also cause trace output to be sent to console. */
	  pHdr = BCS_SrchIHdr (fAmBoot, ImageType, CtlBits) ;

		/* Now that we have relevant image header, get its data.  */
		/* Some cases handled above will be taken as errors here. */
	  if (pHdr != NULL)
	    switch (SvcReq)
	    {
		case BCSSR_WALK_IMAGES:	/* For PROM Boots, do NOT change. */
			pRC = (p_void) (BCSSR_FAILURE) ;	/* abnormal */
			break ;

		case BCSSR_BOOT_EXTENSION: /* For PROM Boots, do NOT change. */
			pRC = (p_void) (BCSSR_FAILURE) ;	/* abnormal */
			break ;

		case BCSSR_GET_IMAGE_HDR:
			pRC = (p_void) (pHdr) ;
			break ;

		case BCSSR_GET_IMG_HDR_SIZE:
			pRC = (p_void) ((BCS_IHI_END_NBR) << 2) ;
			break ;

		case BCSSR_GET_FLASH_BASE:
			pRC = (p_void) (pHdr [BCS_IHI_FLASH_BASE]) ;
			break ;

		case BCSSR_GET_FLASH_SIZE:
			pRC = (p_void) (pHdr [BCS_IHI_FSH_IMAGE_LEN]) ;
			break ;

		case BCSSR_GET_RAM_BASE:
			pRC = (p_void) (pHdr [BCS_IHI_RAM_BASE]) ;
			break ;

		case BCSSR_GET_RAM_SIZE:
			pRC = (p_void) (pHdr [BCS_IHI_RAM_IMAGE_LEN]) ;
			break ;

		case BCSSR_GET_ENTRY_ADDR:
			pRC = (p_void) (pHdr [BCS_IHI_IMAGE_ENTRY]) ;
			break ;

		case BCSSR_GET_SERVICE_ADDR:
			pRC = (p_void) (pHdr [BCS_IHI_IMAGE_SVC]) ;
			break ;

		case BCSSR_COPY_IMAGE:   /* No support for compression today. */
			if ((pHdr [BCS_IHI_FLASH_BASE] != BCS_IH_ILLEGAL_ADDR)
			 && (pHdr [BCS_IHI_RAM_BASE] != BCS_IH_ILLEGAL_ADDR)
			 && (pHdr [BCS_IHI_FSH_IMAGE_LEN] ==
						 pHdr [BCS_IHI_RAM_IMAGE_LEN])
			 && (pHdr [BCS_IHI_RAM_IMAGE_LEN] > 0))
			{
				if (fAmBoot)
					PUNCH_WD ;

				(void) A_BlockCopy
					((u_int8 *) (pHdr [BCS_IHI_FLASH_BASE]),
					 (u_int8 *) (pHdr [BCS_IHI_RAM_BASE]),
					 pHdr [BCS_IHI_RAM_IMAGE_LEN],
					 AR_SvcWatchdog) ;

				pRC = (p_void) (BCSSR_SUCCESS) ;
			}
			else	pRC = (p_void) (BCSSR_FAILURE) ;
			break ;

		case BCSSR_RUN_BOOT:	/* For PROM Boots, do NOT change. */

			/* Same as run entry point case for now. */

		case BCSSR_RUN_ENTRY_PT:
			if (pHdr [BCS_IHI_IMAGE_ENTRY] != BCS_IH_ILLEGAL_ADDR)
			{
				if (fAmBoot)
					PUNCH_WD ;

					/* Flush I/D caches before jump. */
				Sys_ModCpuCACR (CPU030_FLUSH_CACHES,
						CPU030_FLUSH_CACHES) ;

					/* Assume no return value for call. */
				(* (void (*)())
					(pHdr [BCS_IHI_IMAGE_ENTRY])) (pvar) ;

				pRC = (p_void) (BCSSR_SUCCESS) ;
			}
			else	pRC = (p_void) (BCSSR_FAILURE) ;
			break ;

		case BCSSR_RUN_VEC_TABL:
			if (pHdr [BCS_IHI_IMAGE_ENTRY] != BCS_IH_ILLEGAL_ADDR)
			{
				if (fAmBoot)
					PUNCH_WD ;

					/* Flush I/D caches before jump. */
				Sys_ModCpuCACR (CPU030_FLUSH_CACHES,
						CPU030_FLUSH_CACHES) ;

					/* Assume no return value for call. */
				bcs_Glue1 (pHdr [BCS_IHI_IMAGE_ENTRY]) ;

				pRC = (p_void) (BCSSR_SUCCESS) ;
			}
			else	pRC = (p_void) (BCSSR_FAILURE) ;
			break ;

		case BCSSR_RUN_SERVICE_REQ:
			if (pHdr [BCS_IHI_IMAGE_SVC] != BCS_IH_ILLEGAL_ADDR)
			{
				if (fAmBoot)
					PUNCH_WD ;

					/* Flush I/D caches before jump. */
				Sys_ModCpuCACR (CPU030_FLUSH_CACHES,
						CPU030_FLUSH_CACHES) ;

					/* Assume no return value for call. */
				(* (void (*)())
					(pHdr [BCS_IHI_IMAGE_SVC])) (pvar) ;
				pRC = (p_void) (BCSSR_SUCCESS) ;
			}
			else	pRC = (p_void) (BCSSR_FAILURE) ;
			break ;

		case BCSSR_READ_IMAGE_NAME:
			/* --------------------------------------------- */
			/* Expect two additional varargs arguments:	 */
			/*		user buf pointer, user buf size. */
			/* --------------------------------------------- */
			pRC = BCS_ReadHdrString
				((u_int8 *)(pHdr [BCS_IHI_IMAGE_NAME]),
				 BCS_IHL_IMAGE_NAME, pvar) ;
			break ;

		case BCSSR_READ_IMAGE_VERS:
			/* --------------------------------------------- */
			/* Expect two additional varargs arguments:	 */
			/*		user buf pointer, user buf size. */
			/* --------------------------------------------- */
			pRC = BCS_ReadHdrString
				((u_int8 *)(pHdr [BCS_IHI_IMAGE_VERSION]),
				 BCS_IHL_IMAGE_VERSION, pvar) ;
			break ;

		case BCSSR_READ_IMAGE_DATE:
			/* --------------------------------------------- */
			/* Expect two additional varargs arguments:	 */
			/*		user buf pointer, user buf size. */
			/* --------------------------------------------- */
			pRC = BCS_ReadHdrString
				((u_int8 *)(pHdr [BCS_IHI_IMAGE_DATE]),
				 BCS_IHL_DATE_TIME, pvar) ;
			break ;

		case BCSSR_READ_INSTALL_DATE:
			/* --------------------------------------------- */
			/* Expect two additional varargs arguments:	 */
			/*		user buf pointer, user buf size. */
			/* --------------------------------------------- */
			pRC = BCS_ReadHdrString
				((u_int8 *)(pHdr [BCS_IHI_INSTALL_DATE]),
				 BCS_IHL_DATE_TIME, pvar) ;
			break ;

		case BCSSR_RUN_DIAGS:			/* Not supported. */
		case BCSSR_DOWNLOAD_IMAGE:		/* Not supported. */
		case BCSSR_SCRATCH_IMAGE:		/* Not supported. */
		default:				/* Unknown request. */
			pRC = (p_void) (BCSSR_FAILURE) ;
			break ;
	    }
	}

	if (fAmBoot)
		PUNCH_WD ;

	va_end (pvar) ;

	return (pRC) ;

}	/* BCS_Service */
