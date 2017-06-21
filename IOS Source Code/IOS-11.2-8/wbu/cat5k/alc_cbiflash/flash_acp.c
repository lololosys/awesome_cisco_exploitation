/* $Id: flash_acp.c,v 1.1.4.1 1996/05/09 14:54:21 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_cbiflash/flash_acp.c,v $
 *------------------------------------------------------------------
 * flash_acp.c -- ACP specific code for generic Synergy/NMP flash driver.
 *
 * July 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flash_acp.c,v $
 * Revision 1.1.4.1  1996/05/09  14:54:21  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:13:45  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:26  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:09:46  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:40:45  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:16  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.1  1995/07/27  08:06:06  rlowe
 * Initial checkin.  Contains "yield_cpu" routine for flash driver.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "lo_wdog.h"
#include "acp_c_defs.h"
#include "cbi_defs.h"
#include "flash.h"


void yield_cpu ()
{
	PUNCH_WD ;

}	/* yield_cpu */

