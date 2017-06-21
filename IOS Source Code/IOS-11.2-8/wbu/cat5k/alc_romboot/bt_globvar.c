/* $Id: bt_globvar.c,v 1.1.4.1 1996/05/09 14:59:58 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_romboot/bt_globvar.c,v $
 *-----------------------------------------------------------------
 * bt_globvar.c --  ACP Boot global variable declarations file.
 *
 * November 30, 1994, Rick Lowe
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bt_globvar.c,v $
 * Revision 1.1.4.1  1996/05/09  14:59:58  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:54  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:03  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:04  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.2  1996/03/20  10:52:41  rlowe
 * Integrate with ios/wbu sub-tree and makefiles.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:27  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:07  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "acp_basic_types.h"
#include "boot_defs.h"
#include "bt_globvar.h"


u_int32	BootStack [BOOT_MAX_STACK / 4] ;

s_int8	cli_print_buf [80 * 2 + 4];	/* Enough for 2 tty lines. */

u_int32	bt_exception_pc;
u_int16	bt_exception_status; 
u_int16	bt_exception_vector;
u_int32	bt_exception_info [BOOT_EXCP_DATA_SZ / 4];

