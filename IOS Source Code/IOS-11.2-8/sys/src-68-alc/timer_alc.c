/* $Id: timer_alc.c,v 3.1.60.2 1996/08/19 19:02:05 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/src-68-alc/timer_alc.c,v $
 *------------------------------------------------------------------
 * timer_alc.c --	(was if_68_alc_timer.c)
 *			timer support specific to Synergy ATM line card.
 *			ALC uses MC 68901 MFP timers, not i8254.
 *
 * August 31, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: timer_alc.c,v $
 * Revision 3.1.60.2  1996/08/19  19:02:05  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.1.60.1  1996/05/09  14:41:16  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.90.1  1996/04/27  07:25:33  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.80.1  1996/04/08  02:09:04  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.66.2  1996/03/22  23:40:17  rlowe
 * Give relative paths to wbu include files.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.66.1  1996/03/22  09:43:49  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.50.1  1996/03/03  21:25:01  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.48.1  1996/03/02  01:21:16  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.30.1  1996/02/29  07:50:22  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.1  1996/01/06  00:12:20  hampton
 * New files for restructuring the code that plays with hardware timer
 * chips.
 *
 * Revision 2.1  1995/08/23  21:52:16  lcheung
 * placholder file for synalc2_branch
 *
 *------------------------------------------------------------------
 * As if_68_alc_timer.c:
 * Revision 2.1.32.1  1996/02/03  07:08:15  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.2.2  1995/08/31  14:01:47  rlowe
 * Carryover useful code from old Synergy_ATMlc_branch (10.3) to new
 * Synalc2_branch (11.0).
 *
 * Branch: Synalc2_branch
 *
 * Revision 2.1.2.1  1995/08/23  21:55:04  lcheung
 * move file from Synergy_atmlc_branch to synalc2_branch
 * Branch: Synalc2_branch
 *
 * Revision 2.1  1995/08/23  21:52:16  lcheung
 * placholder file for synalc2_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"

#include "../../boot/src-68-alc/defs_alc.h"
#include "../../boot/src-68-alc/pcmap.h"
#include "../../wbu/cat5k/alc_include/mfp_acp_defs.h"

#include "../os/timer_generic.h"
#include "../os/platform_clock.h"

/*
 * Local defines
 */
#define MSEC_SETTING    1000		/* timer setting for 1ms */

/*************************************************************************
 * time_it:
 *     spin waits for count of cnt,
 *     returns loop time in usecs.
 *************************************************************************/

/* NB - the non-XX stuff includes the special code for the
 *      brut spin on the timer.  the new and improved DEC-Hack!
 */

/* NB - 8254 command settling time.  The 8254-2 chip specs the command
 *      settling time as 165 ns.  This time should occur between
 *      any write and a following read, or any two reads.
 *
 *      We used WASTETIME(5) which is sufficient on clean.
 *
 *      These times do not affect calibration, since the timer runs
 *      from the msb write to the timer register to the command
 *      to latch the current counter value.
 */

 
/*
 * Synergy ATM line card uses 68901 MFP timers, not i8254.
 */
 
/*
 * time_it:
 * Perform time measurements
 */
int
time_it (int cnt)
{
        return (cnt) ;          /* RKL:  Fix this later !! */
}
 
#ifdef DELAY_TIMER
 
/*
 * time_DELAY:
 * Do some timing and print out the results
 */
int
time_DELAY(int cnt)
{
        return (cnt * 1000) ;   /* RKL:  Fix this later !! */
}
 
#endif DELAY_TIMER
 
/*
 * restart_t2:
 * Enable timer/refresh interrupt
 * for BRUT insures 16 ms without interrupt.
 */
 
void restart_t2 (void)
{
        MFP_UnmaskIntrW (MFP_4MS_TMR_BITMASK, TRUE, TRUE) ;
}
 
/*
 * SYNALC watchdog is always running in HW (unless jumpered off).
 * So just punch it...
 */
void
restart_watchdog (int t)
{
        PUNCH_WD ;
}


/***************************************************************************
 *
 *         time delay routines          
 *
 ***************************************************************************/
/* (clev: for merlot)
 * wait at least n microseconds 
 * by watching 8254 timer counter 1 while looping.
 * the sequence of instructions is critical in this module due to the
 * slow internal workings of the 8254 chip.
 * especially important is the time between writing the control register
 * and reading something back.
 */
void usecdelay (unsigned n)
{
        MFP_Poll_uSec (n) ;
}

/*
 * get_timer_parameters
 *
 * Get information about the hardware timer (used by the system clock
 * support)
 */
void get_timer_parameters (ulong *freq_numerator, ulong *freq_denominator,
			   ulong *freq_divisor, 
			   get_resid_type *get_residual,
			   ulong *precision)
{
    *freq_numerator = NMI_FREQ_NUMERATOR;
    *freq_denominator = NMI_FREQ_DENOMINATOR;
    *freq_divisor = 1;			/* No residual. */
    *get_residual = NULL;
    *precision = 7;			/* No residual. */
}

/* End of file */

