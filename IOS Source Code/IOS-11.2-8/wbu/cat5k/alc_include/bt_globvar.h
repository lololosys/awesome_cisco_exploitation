/* $Id: bt_globvar.h,v 1.1.4.1 1996/05/09 14:55:32 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/bt_globvar.h,v $
 *-----------------------------------------------------------------
 * bt_globvar.h -- ACP PROM Boot global variable definitions.
 *                 This file is used with both assembly and C.
 *
 * November 30, 1994, Rick Lowe
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bt_globvar.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:32  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:01  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:55  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:09  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:01  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:29  rlowe
 * Placeholders for California.
 *
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.1.8.1  1996/01/18  05:02:25  rlowe
 * Manufacturability changes to bootrom and flash boot code.
 *
 * Branch: ATM21_branch
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__BT_GLOBVAR_H
#define	__BT_GLOBVAR_H


#ifndef	__BOOT_DEFS_H
#include "boot_defs.h"
#endif


#ifndef	ASM_MODE_FLG

#ifndef	_ACP_BASIC_TYPES_H
#include "acp_basic_types.h"
#endif


extern u_int32	BootStack [BOOT_MAX_STACK / 4] ;

extern s_int8	cli_print_buf [80 * 2 + 4];	/* Enough for 2 tty lines. */

extern u_int32	bt_exception_pc;
extern u_int16	bt_exception_status;
extern u_int16	bt_exception_vector;
extern u_int32	bt_exception_info [BOOT_EXCP_DATA_SZ / 4];

extern u_int8	bts_PUD_Blank    [] ;		/* Strings for trace output. */
extern u_int8	bts_PUD_NewLine0 [] ;
extern u_int8	bts_PUD_NewLine  [] ;
extern u_int8	bts_PUD_Passed   [] ;
extern u_int8	bts_PUD_Failed   [] ;
extern u_int8	bts_PUD_Bypass   [] ;
extern u_int8	bts_PUD_Enabled  [] ;
extern u_int8	bts_PUD_Disabled [] ;
extern u_int8	bts_PUD_Hangup   [] ;
extern u_int8	bts_PUD_Err_Unk  [] ;
extern u_int8	bts_PUD_Err_DB   [] ;
extern u_int8	bts_PUD_Err_AB   [] ;
extern u_int8	bts_PUD_Err_DW   [] ;
extern u_int8	bts_PUD_Err_P1   [] ;
extern u_int8	bts_PUD_Err_P2   [] ;
extern u_int8	bts_PUD_Err_P3   [] ;
extern u_int8	bts_PUD_Err_at   [] ;
extern u_int8	bts_PUD_Err_wr   [] ;
extern u_int8	bts_PUD_Err_rd   [] ;


#define BOOT_STACK_START	(& BootStack [ BOOT_MAX_STACK / 4 - 4 ])

#else

#define BOOT_STACK_START	(_BootStack + BOOT_MAX_STACK - 0x10)

#endif


#endif	/* __BT_GLOBVAR_H */
