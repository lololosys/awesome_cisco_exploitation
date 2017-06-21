/* $Id: setjmp.h,v 1.1.4.1 1996/05/09 15:00:10 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarinc/setjmp.h,v $
 *-----------------------------------------------------------------
 * setjmp.h --  
 *
 * ?
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: setjmp.h,v $
 * Revision 1.1.4.1  1996/05/09  15:00:10  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:06  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:17  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:15  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:37:45  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:22  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _SETJMP_
#define _SETJMP_

/* defines for longjmp buffer */
#define JB_S0		0
#define JB_S1		1
#define JB_S2		2
#define JB_S3		3
#define JB_S4		4
#define JB_S5		5
#define JB_S6		6
#define JB_S7		7
#define JB_FP		8
#define JB_SP		9
#define JB_RA		10
#define JB_SIZ		11

#ifdef LANGUAGE_C
typedef int jmp_buf[JB_SIZ];
#endif

#endif /* _SETJMP_ */
