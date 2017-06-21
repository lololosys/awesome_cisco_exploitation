/* $Id: timer.c,v 1.1.68.1 1996/06/16 21:19:05 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-68-as5200/timer.c,v $
 *------------------------------------------------------------------
 * timer.c -- Timer routines for Brasil.
 *
 * October 1995, Joe Turner
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: timer.c,v $
 * Revision 1.1.68.1  1996/06/16  21:19:05  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 1.1  1995/12/03  21:18:48  jturner
 * Placeholder for Brasil.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Includes
 */
#include "monitor.h"
#include "cpu.h"
#include "mon_defs.h"
#include "setjmp.h"
#include "confreg.h"
#include "console.h"
#include "exception.h"
#include "context.h"
#include "signal_4k.h"
#include "extern.h"

static volatile int alarm_setting;  /* What to set the alarm to */
static volatile int alarm_count;    /* Number of milliseconds left */
int timer_int(unsigned long stack_frame);  /* forward declaration */

void
timer_init(void)
{
    /*
     * Install the periodic function timer and start it.
     */
    *(short *)ADRSPC_BRUT_TIMER2 = MON_TIMER_VAL;
    *(short *)ADRSPC_BRUT_TIMER0 = MON_TIMER_VAL * 4;
    /*
     * The following write clears any existing IRQ from the i8254
     * so that the timers will start interrupting us:
     */
    *(volatile uchar *)ADRSPC_TCONTROL = 0;
 
    /*
     * Enable or disable the watchdog timer:
     * Initialize the counter timers *BEFORE* doing this. Otherwise, we are
     * vulnerable to a momentary glitch in WE (watchdog enable) which will
     * reset the box if the 8254 comes up in a funny state with Counter 0
     * out already asserted.
     */
    if ((NVRAM_SAV)->confreg & WATCHDOG_MASK) {
        if ((NVRAM_SAV)->confreg & DIAG_MASK)
            puts("\nWatchdog timer disabled...");
    } else {
        if ((NVRAM_SAV)->confreg & DIAG_MASK)
            puts("\nEnabling Watchdog timer...");
        *(volatile u_char *)ADRSPC_TCONTROL |= TCONTROL_WE;
    }
}

/*
 * Turn a running PIT timer off.
 */
void
timer_disable(void)
{
    /*
     * Now get disable nmi's.  By writing a one (0x01) to counter  2  it
     * sets  up  a mode where the timer will never generate an interrupt
     * -- It will continually reload a 0x01  --  as  the  interrrupt  is
     * generated because of the transisition from 2 to 1.
     */
    *(short *)ADRSPC_BRUT_TIMER2 = 0x01;
 
    /*
     * Need to disable watchdog?
     */
    if (!((NVRAM_SAV)->confreg & WATCHDOG_MASK)) {
        *(volatile u_char *)ADRSPC_TCONTROL &= ~TCONTROL_WE;
    }
}

/*
 * and back on again.
 */
void
timer_enable(void)
{
    *(short *)ADRSPC_BRUT_TIMER2 = MON_TIMER_VAL;

    /*
     * Need to enable watchdog?
     */
    if (!((NVRAM_SAV)->confreg & WATCHDOG_MASK)) {
        *(volatile u_char *)ADRSPC_TCONTROL |= TCONTROL_WE;
    }

}

/*
 * ISR for level 7 timer interrupt.
 */
int
timer_int(unsigned long stack_frame)
{

    extern SIG_PF sigintfcn;
    extern SIG_PF sigalrmfcn;
    volatile int i;

    /*
     * reset watchdog timer counter:
     */
    *(short *)ADRSPC_BRUT_TIMER0 = MON_TIMER_VAL * 4;

    /*
     * Check for alarms
     */
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
     * Check the break key for Abort
     * We have to wait for the break condition to end:
     */
    if ((uartaddr->a.aux2 & BREAKXA) && (!((NVRAM)->confreg & DISABLE_BREAK) ||
        ((clock < 60000) && (hclock == 0)))) {
        if (brk) {
            /*
             * break detected
             * check sigintfcn and act accordingly
             * longjmp to monjmpptr if MONITOR_RUNNING
             * trap into the monitor if we are executing a loaded program by
             * returning 1.
             */
 	    brk = 0;
            while (uartaddr->a.state & RXREADY)
                i = uartaddr->a.data; /* Empty uart */
            uartaddr->a.command = RESETERROR;
            uartaddr->a.command = RESETDBREAK;
            hkeepflags |= H_USRINT;  /* set the user interrupt flag */
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
            } else {
		return(1);  /* assembler handles the trap to the monitor */
	    }
        } else {
 	    brk = 1;
            while (uartaddr->a.state & RXREADY)
                i = uartaddr->a.data; /* Empty uart */
            uartaddr->a.command = RESETERROR;
            uartaddr->a.command = RESETDBREAK;
        }
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

    /* reset watchdog timer counter */
    *(short *)ADRSPC_BRUT_TIMER0 = MON_TIMER_VAL * 4;

    timer_disable();       /* Ensure mutual exclusion on variables */

    /*
     * Compensate for the fact that we have a timer resolution
     * greater than 1ms.
     */
    cnt = mscnt / MON_TIMER_INTERVAL;
    if(mscnt % MON_TIMER_INTERVAL) cnt++;  /* round up */
    alarm_setting = cnt;  /* Initialize the alarm settings */
    alarm_count = cnt;    /* and the count */
    timer_enable();       /* start the timer */
    /* make sure the watchdog timer restarts */
    *(short *)ADRSPC_BRUT_TIMER0 = MON_TIMER_VAL * 4;

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

int
test_watchdog(int argc, char *argv[])
{
    puts("The watchdog timer should reset unit in a few seconds ...");
    /*
     * Writing a one (0x01) to counter 2 it sets up  a  mode  where  the
     * timer  will  never  generate  an interrupt -- It will continually
     * reload a 0x01 -- as the interrrupt is generated  because  of  the
     * transisition  from  2 to 1.  The watchdoggie, however, still will
     * go off.
     */
    *(short *)ADRSPC_BRUT_TIMER2 = 0x01;

    while(1);
}

/* End of Module */
