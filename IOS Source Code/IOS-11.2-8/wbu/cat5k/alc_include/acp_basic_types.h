/* $Id: acp_basic_types.h,v 1.1.4.1 1996/05/09 14:55:25 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/acp_basic_types.h,v $
 *------------------------------------------------------------------
 * acp_basic_types.h -- basic definitions file for the ACP.
 *
 * October 1994, Rick Lowe
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: acp_basic_types.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:25  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:54  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:46  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.2  1996/04/02  05:08:12  rlowe
 * Fix up include file paths.  Cleanup main ALC makefile regarding
 * sar images and tools.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.16.1  1996/03/22  09:11:02  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:53  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:21  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	_ACP_BASIC_TYPES_H
#define	_ACP_BASIC_TYPES_H

	/* ====================================================	*/
	/*	This is the basic definitions file for the ACP.	*/
	/*	By Rick Lowe, cisco, 94-10-10.			*/
	/*	This is:  %W% %G%	*/
	/* ====================================================	*/

#ifndef	LANGUAGE_C
#define	LANGUAGE_C
#endif

#ifndef	_BASICT_
#include "../alc_cominc/basic_types.h"		/* Use rel path for IOS. */
#endif	/* _BASICT_ */


	/* Some more portability typedefs. */

typedef INT8	s_int8 ;
typedef UINT8	u_int8 ;

typedef INT16	s_int16 ;
typedef UINT16	u_int16 ;

typedef INT32	s_int32 ;
typedef UINT32	u_int32 ;

typedef void *	p_void ;
typedef	u_int32	bitfield ;

#ifndef	SYNALC			/* Do NOT define this for IOS-related builds. */
#ifndef	NULL
#define	NULL	((p_void) 0)
#endif
#endif

	/* Some Catalyst portability typedefs. */

typedef	INT8	Char ;
typedef	UINT8	uChar ;

typedef	INT16	Int16 ;
typedef	UINT16	uInt16 ;

typedef	INT32	Int32 ;
typedef	UINT32	uInt32 ;

#endif	/* _ACP_BASIC_TYPES_H */
