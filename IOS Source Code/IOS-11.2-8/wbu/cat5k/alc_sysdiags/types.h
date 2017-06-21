/* $Id: types.h,v 1.1.4.1 1996/05/09 15:01:18 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/types.h,v $
 *------------------------------------------------------------------
 * types.h -- 
 *
 * ?
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: types.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:18  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:17  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:42  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:27  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:54:14  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:46  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************/
/*                                                                            */
/* DIAGNOSTICS DRIVER                                                         */
/*                                                                            */
/* This file contains "Typedef & Format Definitions" that are included as     */
/* part of Diagnostics Driver.                                                */
/*                                                                            */
/* NOTE TO THE REVIEWER:                                                      */
/* --------------------                                                       */
/* You may find that Comments & Remarks are kept to the minimum extent, which */
/* eventually would be updated in due course of time.  The author believes in */
/* "Self-documentation" by way of meaningful names for procedures, variables  */
/* etc. and "Hierarchical programming" style which would work out better in   */
/* terms of time & productivity.  In the meantime, if you encounter any       */
/* problems in going thru the code or have any comments/suggestions, do let   */
/* the author know about it.                                                  */
/*                                                                            */
/******************************************************************************/

typedef unsigned char	uChar;
typedef	unsigned short	uInt16;
typedef	unsigned long	uInt32;
typedef	int short	Int16;
typedef	int long	Int32;

#define	NULL		(char *) 0

