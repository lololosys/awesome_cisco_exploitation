/* $Id: refresh.c,v 3.1.58.1 1996/05/09 13:54:32 rbadri Exp $
 * $Source: /release/112/cvs/Xboot/src-68-alc/refresh.c,v $
 *------------------------------------------------------------------
 * refresh.c -- Synergy ATM line card HW clock interrupt/event handling
 *		for MC 68901 MFP timers (based on c3000)
 *
 * April 13, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: refresh.c,v $
 * Revision 3.1.58.1  1996/05/09  13:54:32  rbadri
 * Branch: California_branch
 * Synergy features to the mainline.
 *
 * Revision 3.1.88.1  1996/04/27  06:13:30  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.78.1  1996/04/08  01:35:01  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.64.2  1996/03/23  04:43:15  rlowe
 * Fix includes to look inside wbu top-level directory.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.64.1  1996/03/22  09:07:25  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.50.1  1996/03/03  21:19:25  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.48.1  1996/03/02  01:07:25  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.32.1  1996/02/29  07:42:25  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.1  1995/11/09  09:08:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.62.1  1996/02/03  06:12:02  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.1  1995/09/11  01:39:57  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.28.2  1995/09/02  00:02:04  rlowe
 * Fix includes so that makefile can give search paths, rather than
 * hardcoding them.
 *
 * Branch: Synalc2_branch
 *
 * Revision 2.1.28.1  1995/08/22  18:18:57  lcheung
 * move files from synergy_atmlc_branch to synalc2_branch
 * Branch: Synalc2_branch
 *
 * Revision 2.1  1995/06/07  19:15:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1.2.1  1995/04/19  02:28:57  rlowe
 * Merge Synergy ATM line card code (from 10.2) into IOS 10.3.
 *
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1  1995/04/14 05:22:11  rlowe
 * Placeholders for IOS port to Synergy ATM line card.
 *
 *------------------------------------------------------------------
 * Revision 1.1.1.1  1995/02/21  19:33:46  kchristi
 * creating new ios directory
 *
 * Revision 1.3  1995/02/09  04:55:56  rlowe
 * Initial port of IOS kernel to SYNALC.
 *
 * Revision 1.2  1995/02/04  06:09:30  rlowe
 * Add extra argument for MFP_MaskIntr and MFP_UnmaskIntr.  Adjust
 * to use image address constants from ACP header files.
 *
 * Revision 1.1  1995/02/01  10:59:58  rlowe
 * Ram's initial merge of ALC IOS-Boot code (from older 10.2 tree).
 *
 * Revision 1.1  1995/01/11  23:04:39  rlowe
 * Initial port of IOS Boot to ACP.  Got Monitor up.
 *
 * Revision 1.1.1.1  1994/08/31  22:16:59  rahuja
 * IPS ver 10.2 snapshot
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * On Pancake, refresh is done by hardware and the millisecond clock
 * is handled by our caller.  We check for the <Break> key and may do
 * other things later:
 * The Brut project has its own routine because the Brut timer registers
 * are accessed 16 bits at a time instead of 8 bits at a time.
 * They are also at a different address. The Brut box also doesn't
 * access the control register. This is done in the ASIC. A seperate
 * ISR is provided to prevent run time checks for a Brut box.
 */

#include "../defs.h"
#include "../stdio.h"
#include "../i8254.h"
#include "../reason.h"
#include "../extern.h"
#ifdef	SYNALC
#include "../../wbu/cat5k/alc_include/acp_c_defs.h"
#include "../../wbu/cat5k/alc_include/mfp_acp_defs.h"
#endif

extern cookie_t *cookie;

#ifdef	SYNALC
refresh()
{
    volatile int i;
    register volatile u_int8 *	pReg ;
    register s_int16		Count ;

    /*
     * Restart watchdog so it barks later if we don't restart it again first:
     */
    PUNCH_WD ;
    
    /*
     * Check the break key for Abort
     * We have to wait for the break condition to end:
     */
    pReg  = (volatile u_int8 *)(ACP_HW_MFP_BASE) ;
    Count = MFP_UART_RXDEPTH + 1 ;

    do {
	if (pReg [MFP_RO_RSR] & MFP_RSR_BREAK_DETECT)
	{
		if (config.breakenable ||
		    ((clock < 60000) && (hclock == 0)))
		{
			brk = 1;
			MFP_ClrPendIntr ((MFP_IMSK_RXFL | MFP_IMSK_RXER),
									FALSE) ;
						/* Clear any BREAK interrupt. */
			i = pReg [MFP_RO_UDR] ;	/* Empty uart. */
		}
	}
	else  if (brk)		/* No longer BREAK detected at uart. */
	{
		abort = TRUE;
		brk = 0;
		/* Any Rx char now valid, do NOT empty uart. */
	}
	else	break ;		/* No BREAK condition to de-glitch. */

    } while (-- Count > 0) ;

} /* refresh() */
#else
refresh()
{
    volatile int i;
    
    /*
     * Restart watchdog so it barks later if we don't restart it again first:
     * Do this here, finish after we deal with UART (saves one NOP).
     */
    ctaddr->control = SEL_COUNTER0 | RW_LSBMSB | CT_MODE0;
    
    /*
     * Check the break key for Abort
     * We have to wait for the break condition to end:
     */
    if ((uartaddr->a.aux2 & BREAKXA) && (config.breakenable ||
	((clock < 60000) && (hclock == 0)))) {
	if (brk) {
	    abort = TRUE;
	    brk = 0;
	    while (console->state & RXREADY)
		i = console->data; /* Empty uart */
	    console->command = RESETERROR;
	    console->command = RESETDBREAK;
	} else {
	    brk = 1;
	    while (console->state & RXREADY)
		i = console->data; /* Empty uart */
	    console->command = RESETERROR;
	    console->command = RESETDBREAK;
	}
    }

    /*
     * Finish reseting watchdog timer counter:
     */
    ctaddr->counter0 = (REFRFREQUENCY*4) & 0xFF;
    asm("nop");			/* 68030 on Merlot is too fast! */
    asm("nop");
    ctaddr->counter0 = ((REFRFREQUENCY*4) >> 8) & 0xFF;

#ifdef	lint
    i = i;
#endif
} /* refresh() */
#endif	/* SYNALC */

#ifndef	SYNALC
refresh_brut()
{
    volatile int i;
    
    /*
     * Check the break key for Abort
     * We have to wait for the break condition to end:
     */
    if ((uartaddr->a.aux2 & BREAKXA) && (config.breakenable ||
	((clock < 60000) && (hclock == 0)))) {
	if (brk) {
	    abort = TRUE;
	    brk = 0;
	    while (console->state & RXREADY)
		i = console->data; /* Empty uart */
	    console->command = RESETERROR;
	    console->command = RESETDBREAK;
	} else {
	    brk = 1;
	    while (console->state & RXREADY)
		i = console->data; /* Empty uart */
	    console->command = RESETERROR;
	    console->command = RESETDBREAK;
	}
    }

    /*
     * reset watchdog timer counter:
     */
    *(short *)ADRSPC_BRUT_TIMER0 = REFRFREQUENCY * 4;

#ifdef	lint
    i = i;
#endif
} /* refresh_dec() */
#endif	/* SYNALC */

/*
 * t_disable
 * Disable timer/refresh interrupt 
 */
t_disable()
{
#ifndef	SYNALC
    ctaddr->control = SEL_COUNTER2 | RW_LSBMSB | CT_MODE2;
#else
	MFP_MaskIntrW (MFP_4MS_TMR_BITMASK, FALSE) ;
#endif
}

/*
 * t_disable_brut
 * Disable timer/refresh interrupt for the brut project.
 */
#ifndef	SYNALC
t_disable_brut()
{
    /* set the timer reset bit high */
    *(volatile u_short *)ADRSPC_CONTROL3 |= BRUT_RESET_COUNTER;
}
#endif	/* SYNALC */

/*
 * t_enable
 * Enable timer/refresh interrupt
 */
t_enable()
{
#ifndef	SYNALC
    ctaddr->control = SEL_COUNTER2 | RW_LSBMSB | CT_MODE2;
    ctaddr->counter2 = REFRFREQUENCY & 0xFF;
    ctaddr->counter2 = (REFRFREQUENCY >> 8) & 0xFF;
#else
	MFP_UnmaskIntrW (MFP_4MS_TMR_BITMASK, TRUE, TRUE) ;
#endif
}

/*
 * t_enable_brut
 * Enable timer/refresh interrupt for the brut project.
 */
#ifndef	SYNALC
t_enable_brut()
{
    /* clear the timer reset bit */
    *(volatile u_short *)ADRSPC_CONTROL3 &= ~(BRUT_RESET_COUNTER);
    /* reload a value into the timer */
    *(short *)ADRSPC_BRUT_TIMER2 = REFRFREQUENCY;
}
#endif	/* SYNALC */
