/* $Id: led_dvr.c,v 1.1.4.1 1996/05/09 14:54:59 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/led_dvr.c,v $
 *------------------------------------------------------------------
 * led_dvr.c -- Synergy ATM line-card LED driver
 *
 * June 1995, sakumar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: led_dvr.c,v $
 * Revision 1.1.4.1  1996/05/09  14:54:59  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:26  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:13  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:31  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:26  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:55  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.3  1995/08/30  01:59:38  sakumar
 * Use macro to get led offset int led_shadow array.
 *
 * Revision 1.2  1995/06/17  02:21:45  sakumar
 * Fixed indexing of LED state shadow RAM.
 *
 * Revision 1.1  1995/06/06  23:24:59  sakumar
 * Code to set PMD LEDs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "acp_basic_types.h"
#include "acp_hw_defs.h"
#include "led_if.h"
 

UINT16 led_shadow[4];		/* A RAM array that keeps track of LED states */

#define LED_SHADOW_OFFSET(x) (((x) - ACP_WRRG_PMD_STAT_LED) >> 1)

/*
** set_led: set the specified led to the specified color
*/
void
set_led ( UINT32 which_led, UINT16 led_color )
{
	UINT32	Old_SR = 0 ;

#ifdef DEBUG
	/* Sanity check which_led field */
	if ( which_led < ACP_WRRG_PMD_STAT_LED ||
		 which_led > ACP_WRRG_PMD_BOT_LED    ) {

		printf("\nset_led: which_led value 0x%x out of range", which_led);
		return;
	}
#endif /* DEBUG */

		/* Make this shadow/HW write atomic. */
	Old_SR = splx (7) ;

	/* First set the shadow RAM copy of the LED */
	led_shadow[LED_SHADOW_OFFSET(which_led)] = led_color ;

	/* Then use the shadow value to write to the LED register */
	* (volatile UINT16 *) (ACP_HW_IOREG_BASE + which_led) =
	                      led_shadow[LED_SHADOW_OFFSET(which_led)]; 

	(void) splx (Old_SR) ;
}


/*
** led_status: report the status (color) of specified led
*/

UINT16
led_status ( int which_led )
{
#ifdef DEBUG
    /* Sanity check which_led field */
    if ( which_led < ACP_WRRG_PMD_STAT_LED ||
         which_led > ACP_WRRG_PMD_BOT_LED) {
 
        printf("\nled_status: which_led value 0x%x out of range", which_led);
        return (0);
    }
#endif /* DEBUG */

	return ( led_shadow[LED_SHADOW_OFFSET(which_led)] );
}
