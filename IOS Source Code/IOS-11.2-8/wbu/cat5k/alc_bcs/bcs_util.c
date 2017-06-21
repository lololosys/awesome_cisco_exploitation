/* $Id: bcs_util.c,v 1.1.4.1 1996/05/09 14:54:11 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_bcs/bcs_util.c,v $
 *-----------------------------------------------------------------
 * bcs_util.c -- Boot Code Service (BCS) utility code.
 *		Code used in Client and Server sides of BCS, and
 *		also with utility programs and tools.
 *
 *              To ensure backward compatibility with older Boot code,
 *              ADD but do NOT CHANGE any definitions given here.
 *
 * October 6, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bcs_util.c,v $
 * Revision 1.1.4.1  1996/05/09  14:54:11  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:13:40  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:05  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:09:38  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.2  1996/03/20  10:52:16  rlowe
 * Integrate with ios/wbu sub-tree and makefiles.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:40:36  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:08  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.1  1995/10/13  06:24:06  rlowe
 * Enable and use BCS checksums in PROM and flash Boots.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "acp_basic_types.h"
#include "bcs_api.h"
#include "bcs_image_hdr.h"
#include "bcs_commonIF.h"

#ifdef	UTILITY
#ifndef	NULL
#define	NULL	((void *) 0)
#endif
#endif


/* ----------------------------------------------------	*/
/*	Compute image property value for image header.	*/
/*	Propery value can NEVER be zero.		*/
/* ----------------------------------------------------	*/
u_int32 BCS_ImageProperty (register u_int32 * pHdr)
{
	register u_int32	Calc = 0 ;
	register u_int32	Indx ;


	if ((pHdr == NULL) || (pHdr == (u_int32 *)(BCS_IH_ILLEGAL_ADDR)))
		return (Calc) ;

		/* Calculate the image property value from header data. */
		/* Assume that fields used are contiguous and in order. */
		/* Use rotate (right) and xor to build property value.  */
	for (Indx = BCS_IHI_IMAGE_TYPE ; Indx <= BCS_IHI_IMAGE_CHK ; ++ Indx)
	{
			/* This does a rotate right. */
		Calc  = (((Calc & 1) ? (1 << 31) : 0) | (Calc >> 1)) ;
		Calc ^= pHdr [Indx] ;
	}

		/* Do NOT allow a property (return) value of zero. */
	if (Calc == 0)
		++ Calc ;

	return (Calc) ;

}	/* BCS_ImageProperty */
