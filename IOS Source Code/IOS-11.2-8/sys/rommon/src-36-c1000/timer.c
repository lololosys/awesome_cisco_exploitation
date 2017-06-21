/* $Id: timer.c,v 3.2.58.1 1996/03/21 23:29:28 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-36-c1000/timer.c,v $
 *------------------------------------------------------------------
 * timer.c
 *
 * November 1994, Rob Clevenger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Timer routines for Sapphire.
 *------------------------------------------------------------------
 * $Log: timer.c,v $
 * Revision 3.2.58.1  1996/03/21  23:29:28  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:22:29  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:44:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:24:13  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/09/22  04:36:22  enf
 * CSCdi40869:  Add frankenstorm to 11.0
 *
 * Revision 2.1  1995/06/07  22:39:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "monitor.h"
#include "quicc.h"
#include "mon_defs.h"
#include "exception.h"
#include "context.h"
#include "signal_4k.h"
#include "extern.h"


static volatile int alarm_setting;  /* What to set the alarm to */
static volatile int alarm_count;    /* Number of milliseconds left */
static volatile unsigned long ticks;
static volatile unsigned long break_ticks;
int timer_int(unsigned long stack_frame);  /* forward declaration */

/*
** Initialize the PIT timer in the QUICC chip for 4ms ticks.
*/

void
timer_init(void)
{
    quicc_internal_regs_t *qirp = (quicc_internal_regs_t *)ADRSPC_QUICC_REGB;

    /*
     * The PIT timer is calebrated into mseconds by applying the
     * formula on Page 6-11 QUICC Users Manual.  The Values are:
     *
     *  EXTAL Frequency = 4MHz
     *  Prescale Value = 1 (No Prescale)
     *  Divider = 128
     *
     *  PIT Period = PITR Count Value / 7812
     *
     * For PIT Period = 1ms ===> PITR count aprox 8 (7.81).  Thus
     * take the delta and shift left by 3.
     */

    /*
     * Install the periodic function timer and start
     * it off.  The vector is already installed in EPROM.
     */
    qirp->sim_picr = QUICC_PICR_PIV(PIT_VECTOR_NUM) | QUICC_PICR_PIRQL(7);
    qirp->sim_avr  = QUICC_AVR_VAL;  /* make sure we autovector */
    qirp->sim_pitr = QUICC_PITR_SWP(1) | QUICC_PITR_PITR(MON_TIMER_VAL);
    ticks = 0;
    break_ticks = 1;  /* disable <break> detection for 1 second */
    pet_the_dog();    /* keep the watchdog happy */
}

/*
** Turn a running PIT timer off.
*/
void
timer_disable(void)
{
    quicc_internal_regs_t *qirp = (quicc_internal_regs_t *)ADRSPC_QUICC_REGB;

    qirp->sim_pitr &= ~QUICC_PITR_PITR_M;  /* program a zero count */
    /*
    ** we do not have to wait for the last interrupt here
    ** but call timer_int() routine for housekeeping purposes
    */
    break_ticks = 0;
    timer_int(0L);
}

/*
** and back on again.
*/
void
timer_enable(void)
{
    timer_init();  /* start it */
}

#define LED_TICK_COUNT (500 / MON_TIMER_INTERVAL)  /* 1/2 second */
static volatile int led_ticks;

/*
** Note that nothing has to be done to this hardware timer.  It
** reloads automagically and requires no interrupt reset.
** Only the scratch registers are saved on the stack on entry.
*/
int
timer_int(unsigned long stack_frame)
{
    extern SIG_PF sigintfcn;
    extern SIG_PF sigalrmfcn;
    quicc_internal_regs_t *qirp =
	(quicc_internal_regs_t *)ADRSPC_QUICC_REGB;

    ticks++;             /* tick, tock, ... tick, tock */
    /*
    ** Flash the System OK led on the front panel at a rate of 
    ** once (on and off) per second.
    */
    if(++led_ticks > LED_TICK_COUNT) {
	qirp->pip_pbdat ^= SAP_QU_PT_B_PAR_RUNLED;
	led_ticks = 0;
    }
    pet_the_dog();       /* keep the watchdog happy */
    if(alarm_setting) {
	alarm_count--;
	if(alarm_count == 0) {   /* Time to send alarm */
	    alarm_setting = 0;   /* clear the alarm */
	    switch((int)sigalrmfcn) {
	    case SIG_DFL:
		/* default action terminates program */
		if(monjmpptr) longjmp(monjmpptr,4);
		break;
	    case SIG_IGN:
		break;
	    default:
		if((*sigalrmfcn)((int)&stack_frame) == (int)SIG_DFL)
		    /* default action terminates program */
		    if(monjmpptr) longjmp(monjmpptr,4);
		break;
	    }
	}
    }

    /*
    ** The following bit of code handles the detection of the user
    ** interrupt (<break> key).
    ** The SMC is polled once every timer tick to see if the user
    ** wishes to break.
    ** Code is in place to disable this break detection for a
    ** period of 1 second after the timer is initialized and after
    ** a break is detected.  This is done to avoid the detection of
    ** multiple breaks.
    */
    if(break_ticks) {  
	if(++break_ticks > (1000 / MON_TIMER_INTERVAL)) {  /* 1 second */
	    /* clear possible pending breaks */
	    qirp->smc_regs[CONSOLE_SMC_IDX].smc_smce = QUICC_SMCE_BRK;
	    break_ticks = 0;  /* turn off */
	} else {
	    return(0);        /* skip the break check */
	}
    }
    if(qirp->smc_regs[CONSOLE_SMC_IDX].smc_smce & QUICC_SMCE_BRK) {
	/* user hit the break key at the console - clear the bit */
	qirp->smc_regs[CONSOLE_SMC_IDX].smc_smce = QUICC_SMCE_BRK;
	/*
	** break detected
	** check sigintfcn and act accordingly
	** longjmp to monjmpptr if MONITOR_RUNNING
	** trap into the monitor if we are executing a loaded program by
	** returning 1.
	*/
	hkeepflags |= H_USRINT;  /* set the user interrupt flag */
	break_ticks = 1;  /* enable the timer */
	switch((int)sigintfcn) {
	case SIG_IGN:  /* do nothing */
	    return(0);
	case SIG_DFL:  /* the default action for <break> */
	    break;
	default:
	    if( (*sigintfcn)((int)&stack_frame) == (int)SIG_DFL) break;
	    return(0);
	}
	if(sys_state == MONITOR_RUNNING && monjmpptr) {
	    longjmp(monjmpptr,1);
	}
	else return(1);  /* assembler handles the trap to the monitor */
    }
    return(0);
}

/*
** alarm : Setup the alarm mechanism
** If cnt == 0, disable the alarm mechanism, else initialize the mechanism
** and set alarm_setting to mscnt. mscnt is the number of 1ms intervals that
** pass before the sigalrmfcn signal will be called. Return the amount of
** time left before the alarm would go off.
*/
int
alarm(int mscnt)
{
    int cnt;
    int tmp = alarm_count * MON_TIMER_INTERVAL;

    timer_disable();       /* Ensure mutual exclusion on variables */
    /*
    ** Compensate for the fact that we have a timer resolution
    ** greater than 1ms.
    */
    cnt = mscnt / MON_TIMER_INTERVAL;
    if(mscnt % MON_TIMER_INTERVAL) cnt++;  /* round up */
    alarm_setting = cnt;  /* Initialize the alarm settings */
    alarm_count = cnt;    /* and the count */
    timer_enable();       /* start the timer */
    return(tmp);
}

/*
** pause - stop until alarm signal
*/
int
pause(void)
{
    while(alarm_count);
    return(-1);  /* as per man pages */
}

/*
** sleep - do nothing for the specified number of milliseconds
**
** This routine tries to do the right thing if the alarm is
** already set.
*/
void
sleep(int msecs)
{
    SIG_PF sigsav;
    int tmp;

    timer_disable();     /* ensure mutual exclusion on variables */
    if(alarm_count && (alarm_count * MON_TIMER_INTERVAL) < msecs) {
	/* alarm is currently in use */
	msecs -= (alarm_count * MON_TIMER_INTERVAL);
	timer_enable();
	pause();
    }
    sigsav = signal(SIGALRM, SIG_IGN);
    tmp = alarm(msecs);
    pause();
    signal(SIGALRM, sigsav);
    if(tmp > msecs) alarm(tmp - msecs);
    else alarm(0);
}

long
timer_calibrate(long t) 
{
    /* This routine is not exact today. Only Flash routines use          */
    /* this function and they are not dependent on the accuracy of this  */
    /* routine. This is because the new Flash chips donot need fixed wait */
    /* loops to be supported - They can be polled for status            */
    return(t);
}

int
test_watchdog(int argc, char *argv[])
{
    puts("The watchdog timer should reset unit in 8 seconds ...");
    timer_disable();
    while(1);
}

/* End of Module */
