/* $Id: c1000_ukernel.c,v 3.1.20.2 1996/08/19 19:01:56 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/src-36-c1000/c1000_ukernel.c,v $
 *------------------------------------------------------------------
 * c1000_ukernel.c -- C1000 specific routines necessary to build
 *		      sub_ukernel.o 
 *
 * David Hampton, February 1996
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c1000_ukernel.c,v $
 * Revision 3.1.20.2  1996/08/19  19:01:56  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.1.20.1  1996/03/18  21:57:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.10.1  1996/03/09  05:11:29  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.1  1996/02/21  21:52:18  hampton
 * Placeholders for Port Ready.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "stacks.h"
#include "../os/platform_clock.h"
#include "../les/if_les.h"
#include "c1000.h"
#include "../src-36/quicc.h"
#include "../src-36/quicc_lib.h"
#include "../os/clock.h"
#include "config_register.h"
#include "../os/hostname.h"	/* for prototype of set_hostname() */
#include "sys_registry.h"
#include "../dev/monitor1.h"
#include "version.h"

/*
 * Local Storage
 */

ulong c1000_clock;
ulong c1000_hclock;

/*
 * This routine will return the value of MBAR.
 */
quicc_dpr_t *getmbar (void)
{
    quicc_dpr_t *result;

    asm("movql   #7, d0          | CPU space function code
         movc    d0, sfc         | Load sfc for cpu space
         lea     0x3FF00, a0     | A0 points to MBAR
         movsl   (a0), %0        | Get MBAR
         andl    #0xFFFFE000, %0 | Delete space and valid bits"
	: "=d" (result)
	: );
    return(result);
}

/*
 * Timer support
 */


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
    *freq_divisor = 1;			/* No residual available */
    *get_residual = NULL;
    *precision = NTP_PRECISION;
}

/*
 * =================================================================
 * Name: quicc_usec_delay
 *
 * Description:
 *      Does a spin-wait delay for the given number of micro-seconds.
 *
 * Parameters:
 *      Number of micro-seconds to delay.
 *
 * Returns:
 *      void - none
 * =================================================================
 */
void quicc_usec_delay (unsigned delay)
{
    quicc_dpr_t *dpr;
    unsigned short tmp;

    dpr = (quicc_dpr_t *) getmbar();

    /* Reset Timer 1 in the Timer Global Config Reg (TGCR) */
    dpr->regs.tmr_tgcr &= ~QUICC_TGCR_TMR1_EN;

    /* Set Timer 1 Mode Reg (TMR1) */
    dpr->regs.tmr_tmr1 = QUICC_TMR_USEC_PRESCALE |
                         QUICC_TMR_RESET_MODE | QUICC_TMR_ICLK_INTCLK;

    while (delay) {
        tmp = (delay & QUICC_TMR_TRR_MASK);
        if (tmp == 0)
            tmp = QUICC_TMR_TRR_MASK;
        delay -= tmp;

        /*
         * If delay value shorter than the time it takes to set up the
         * registers, then forget it.
         */
        if (tmp < 5)
            continue;

        /* Clear Timer 1 Counter Register */
        dpr->regs.tmr_tcn1 = 0;

        /* Set Timer 1 Reference Reg (TMR1) for the desired delay */
        dpr->regs.tmr_trr1 = tmp;

        /* Clear any event reg bits that may have been set */
        dpr->regs.tmr_ter1 = QUICC_TER_CAP_EVENT | QUICC_TER_REF_EVENT;

        /* Start the timer */
        dpr->regs.tmr_tgcr |= QUICC_TGCR_TMR1_EN;

        /* Wait for the reference event to occur */
        while (!(dpr->regs.tmr_ter1 & QUICC_TER_REF_EVENT))
            ;

        dpr->regs.tmr_tgcr &= ~QUICC_TGCR_TMR1_EN; /* Stop the timer */
    }
}


/*
 * =================================================================
 * Name: quicc_millisecond_delay
 *
 * Description:
 *      Does a spin-wait delay for the given number of milli-seconds.
 *
 * Parameters:
 *      Number of milli-seconds to delay.
 *
 * Returns:
 *      void - none
 * =================================================================
 */

void quicc_millisecond_delay (uint ms)
{
    uint i;
    for (i = 1; i <= ms; i++)
        quicc_usec_delay(1000);
}

