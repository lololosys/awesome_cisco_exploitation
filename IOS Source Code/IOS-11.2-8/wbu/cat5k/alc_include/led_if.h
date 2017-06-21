/* $Id: led_if.h,v 1.1.4.1 1996/05/09 14:55:37 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/led_if.h,v $
 *-----------------------------------------------------------------
 * led_if.h -- ATM line-card LED driver interface
 *
 * June 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: led_if.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:37  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:05  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:40:01  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:14  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:07  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:34  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.1  1995/06/06  23:26:04  sakumar
 * Interface functions to set PMD LEDs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _LED_IF_H
#define _LED_IF_H

#ifndef	__ACP_HW_DEFS_H
#include "acp_hw_defs.h"
#endif

/*
** Function prototypes
*/

void set_led ( UINT32 which_led, UINT16 led_color ) ;

UINT16 led_status ( int which_led ) ;

#endif /* _LED_IF_H */
