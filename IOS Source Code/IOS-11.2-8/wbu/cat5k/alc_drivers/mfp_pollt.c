/* $Id: mfp_pollt.c,v 1.1.4.1 1996/05/09 14:55:03 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/mfp_pollt.c,v $
 *------------------------------------------------------------------
 * mfp_pollt.c -- ACP MFP polled timer routines.
 *		  (Split out from main MFP driver code.)
 *
 * July 24, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mfp_pollt.c,v $
 * Revision 1.1.4.1  1996/05/09  14:55:03  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:30  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:18  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:37  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:30  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:59  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.2  1995/10/13  06:33:21  rlowe
 * Cleaned up some ifdefs.
 *
 * Revision 1.1  1995/07/27  10:12:59  rlowe
 * Move polling-loop timer routines to a new file, mfp_pollt.c.  This
 * allows the embedded exploder to use the flash driver without pulling
 * in all of the MFP driver as well.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "acp_basic_types.h"
#include "lo_wdog.h"
#include "acp_c_defs.h"
#include "mfp_hw_defs.h"



/* --------------------------------------------------------------------	*/
/*	Poll a MFP timer for N usec delay.				*/
/*	This is only an approximate delay.				*/
/* --------------------------------------------------------------------	*/
void MFP_Poll_uSec (register u_int32 Nbr_uSec)
{
		/* Fix this later... @ 3.6864 MHz, MFP will only count */
		/* 3.6864 cycles per 1 usec, and min prescale is 4 !!  */
	while (Nbr_uSec > 0)
	{
		-- Nbr_uSec ;
	}

}	/* MFP_Poll_uSec */


/* --------------------------------------------------------------------	*/
/*	Poll a MFP timer for 10 usec delay.				*/
/*	This is only an approximate delay.				*/
/*	This routine is tuned to have a 10 usec delay out of i-cache.	*/
/* --------------------------------------------------------------------	*/
void MFP_Poll_10uSec ()
{
	register long	Count ;
	register long	Val ;


		/* Fix this later... @ 3.6864 MHz, MFP will only count */
		/* 3.6864 cycles per 1 usec, and min prescale is 4 !!  */

	PUNCH_WD ;		/* Always punch watchdog at least once. */

		/* This loop constant was tuned with a logic analyzer. */
		/* Original good value was 10, can try 11 since 10     */
		/* might be marginal when running with cache & DRAM.   */
	for (Count = 10 ; Count > 0 ; -- Count)
		++ Val ;

}	/* MFP_Poll_10uSec */


/* --------------------------------------------------------------------	*/
/*	Poll a MFP timer for N msec delay.				*/
/*	This is only an approximate delay.				*/
/* --------------------------------------------------------------------	*/
void MFP_Poll_mSec (register u_int32 Nbr_mSec)
{
	register long	Count ;


		/* Fix this later... @ 3.6864 MHz, MFP will only count */
		/* 3.6864 cycles per 1 usec, and min prescale is 4 !!  */

	PUNCH_WD ;		/* Always punch watchdog at least once. */

	while (Nbr_mSec > 0)
	{
		for (Count = 100 ; Count > 0 ; -- Count)
			MFP_Poll_10uSec () ;

		-- Nbr_mSec ;
	}

}	/* MFP_Poll_mSec */

