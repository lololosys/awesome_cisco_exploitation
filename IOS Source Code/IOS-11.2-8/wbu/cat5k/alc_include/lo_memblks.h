/* $Id: lo_memblks.h,v 1.1.4.1 1996/05/09 14:55:39 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/lo_memblks.h,v $
 *-----------------------------------------------------------------
 * lo_memblks.h -- Function prototypes and related definitions for
 *                 generic low-level utility routines.
 *
 * November 1994, Rick Lowe
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lo_memblks.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:39  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:08  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:40:04  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:16  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:09  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:37  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	_LO_MEMBLKS_H
#define	_LO_MEMBLKS_H

	/* ====================================================	*/
	/*	Function prototypes and related definitions	*/
	/*	for generic low-level utility routines.		*/
	/*	By Rick Lowe, cisco, 11-22-94.			*/
	/*	This is:  %W% %G%	*/
	/* ====================================================	*/

#ifndef	_ACP_BASIC_TYPES_H
#include "acp_basic_types.h"
#endif	/* _ACP_BASIC_TYPES_H */


extern s_int32 A_BlockFill (	u_int8 *	pByte,
				u_int32		MemSize,
				u_int8		TestByte,
				void		(* WdogFcn)() ) ;

extern s_int32 A_BlockCopy (	u_int8 *	pSrc,
				u_int8 *	pDst,
				u_int32		MemSize,
				void		(* WdogFcn)() ) ;

#endif	/* _LO_MEMBLKS_H */
