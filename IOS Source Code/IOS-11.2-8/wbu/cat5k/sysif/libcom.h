/* $Id: libcom.h,v 1.1.4.1 1996/05/09 15:01:49 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/sysif/libcom.h,v $
 *------------------------------------------------------------------
 * libcom.h:  definitions for common flash support library.
 *
 * June 1995, 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: libcom.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:49  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:44  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:43:14  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:55  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:38  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:52  rlowe
 * Placeholders for California.
 *
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.4  1995/10/04  01:07:26  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.3  1995/07/27  09:11:41  rlowe
 * Restructure flash driver so that existing APIs and functionality are
 * preserved, while allowing "printf" and "yield_cpu" to be passed in to
 * the flash driver as function pointers.  This allows the exploder
 * (which uses the flash driver) to be truly independent of the Boot/Kernel
 * context (the caller provides "printf" and "yield_cpu" instead of linking
 * them in directly, or using HW-specific macros).
 *
 * Revision 1.2  1995/06/12  22:15:55  rfa
 * Fixed header comment
 *
 * Revision 1.1  1995/06/12  22:12:02  rfa
 * Moved libcom.h from nmpflash/h/libcom.h
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern u_long checksum(u_long *, int, u_long);
extern u_long checksum2(u_long *, int, u_long, int (*)());
extern void border(void);
extern void format(char *);
extern void format2(char *, int (*)());
extern void lcopy(u_long *, u_long *, int);
extern void lcopy2(u_long *, u_long *, int, int (*)());
