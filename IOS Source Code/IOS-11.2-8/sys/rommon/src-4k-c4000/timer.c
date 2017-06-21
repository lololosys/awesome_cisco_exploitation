/* $Id: timer.c,v 3.2.58.1 1996/03/21 23:31:15 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-c4000/timer.c,v $
 *------------------------------------------------------------------
 * timer.c
 *
 * May 1993, Michael Beesley
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Timer test/control for Sierra.
 *------------------------------------------------------------------
 * $Log: timer.c,v $
 * Revision 3.2.58.1  1996/03/21  23:31:15  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:23:44  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:46:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:40:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
** Adapted from timer.c in DiagMon for XX
*/
#include "monitor.h"
#include "mon_defs.h"
#include "cpu_mips.h"
#include "signal_4k.h"
#include "error.h"

#define  TIMERCOUNT 500

extern int sys_state;

volatile unsigned char intimerint;  /* 0: never got into timer int handler
				       !0: got into timer int handler */

static volatile int alarm_setting;  /* What to set the alarm to */
static volatile int alarm_count;    /* Number of milliseconds left */

/*
 * disable_watchdog:
 * Turn watchdog off
 */
void
disable_watchdog()
{
    KSEG1_IO_ASIC->ccr &= ~CCR_CT0_EN;
    flush_io_wb();
}
    
/*
 * test_watchdog: 
 * Test the watchdog reboot mechanism
 */
int
test_watchdog(int argc, char *argv[])
{
    long delaycnt, i;

    if (argc != 1) {
	printf("usage: %s\n", argv[0]);
	return(1);
    }

    delaycnt = timer_calibrate(1000);	/* 1ms calibrated time */

    /*
     * Initialize the watchdog and let it run out, Should reboot us.
     */
    printf("starting the watchdog ... should reboot in 16ms\n");
    KSEG1_IO_ASIC->ct0 = 16000;		/* 16 ms              */
    KSEG1_IO_ASIC->ccr |= CCR_CT0_EN;	/* enable watchdog    */
    flush_io_wb();

    /*
     * waste aways 100ms
     */
    for (i=0; i<100; i++) {
	wastetime(delaycnt);
    }
    
    /*
     * we should never get here
     */
    i = KSEG1_IO_ASIC->ct0;
    KSEG1_IO_ASIC->ccr &= ~CCR_CT0_EN;
    flush_io_wb();
    printf("%s: error, the watchdog did not reboot us, ct0= %d\n",
	   argv[0], i);
    return(1);
}

/*
 * timertest :
 * Use Refresh Timer (Counter 2) for timer interrupt test,
 * and Counter 1 for the polled test
 */
int 
timertest(void)
{
    unsigned short count;
    long delaycnt;
    
    delaycnt = timer_calibrate(800);
    testname("timer interrupt");

    /*
     * Initialize Counter 1 for polling, waste some time, and check
     * that CT1 is enabled and count has decremented
     */
    KSEG1_IO_ASIC->ct1 = TIMERCOUNT;
    KSEG1_IO_ASIC->ccr |= CCR_CT1_EN;
    flush_io_wb();
    wastetime(25);
    count = KSEG1_IO_ASIC->ct1;
    if ( ! (KSEG1_IO_ASIC->ccr & CCR_CT1_EN) ) {
	cterr('f',0,"Revceived incorrect status from Counter Control Register.");
	return(1);
    }
    KSEG1_IO_ASIC->ccr &= ~CCR_CT1_EN;
    flush_io_wb();
    if(!(count < TIMERCOUNT)) {
	cterr('f',0,"Timer chip is not counting.");
	return(1);
    }

    /*
     * Initialize channel 2 (refresh) for interrupt test
     */
    KSEG1_IO_ASIC->ccr &= ~CCR_CT2_EN; 
    flush_io_wb();
    KSEG1_IO_ASIC->ct2 = TIMERCOUNT;
    intimerint = 0;
    KSEG1_IO_ASIC->ccr |= CCR_CT2_EN;
    flush_io_wb();

    /* 
     * LATER : May have to clear any existing interrupts to allow
     * the timer to interrupt. R4000 should be set up to accept
     * Nevada interrupts.
     */
    wastetime(delaycnt);            /* interrupt should happen here */
    KSEG1_IO_ASIC->ccr &= ~CCR_CT2_EN;
    flush_io_wb();
    if(!intimerint) {
	cterr('f',0,"Didn't receive a timer interrupt.");
	return(1);
    }

    prcomplete(testpass, errcount, 0);
    return(0);
}

/*
 * abort_alarm : Stub to jump back to the monitor when
 * an un-caught alarm signal occurs. longjmps with a value of 4.
 */
static void
abort_alarm(void)
{
    setcp0_sreg( (getcp0_sreg() & ~(SREG_ERL|SREG_EXL)) | SREG_IE );
    sys_state = MONITOR_RUNNING;
    cache_flush(0, -1);
    if(monjmpptr) longjmp(monjmpptr,4);
}

long 
timerint(r4k_context_t *frame)
{
    extern int (*sigalrmfcn)();

    /*
     * Write the Clear Timer Interrupt Register to ack the interrupt
     */
    KSEG1_IO_ASIC->cti = 0;
    flush_io_wb();
    intimerint++;

    if ( !alarm_setting )
	return(0);                 /* Alarm function not initialized */
    alarm_count--;
    if ( alarm_count == 0 ) {   /* Time to send alarm */
	alarm_setting = 0;      /* clear the alarm */
	t_disable();
   	switch ( (int)sigalrmfcn ) {
	case SIG_DFL:
	    break;
	case SIG_IGN:
	    return(0);
	default:
	    if((*sigalrmfcn)(frame) == (int)SIG_DFL) break;
	    return(0);
	}
	/* default action terminates program */
	if(monjmpptr) {
	    frame->epc = (unsigned long)abort_alarm;
	    frame->epc_hi = KSEG_MSB_ADDR;
	}
    }
    return(0);
}

void
scopetimer(void)
{
    unsigned long ticks1 = 40000;
    unsigned long ticks2 = 40;

    printf("scope triggers every %d and %d usecs.\n",ticks1,ticks2);
    ticks1 = timer_calibrate(ticks1);
    ticks2 = timer_calibrate(ticks2);
    while(1) {
	KSEG1_IO_ASIC->sys_ctl &= ~CONTROL_RUNLAMP;
	wastetime(ticks1);
	KSEG1_IO_ASIC->sys_ctl |= CONTROL_RUNLAMP;
	wastetime(ticks2);
    }
}

/*
 * disable timer and return possible count
 */
int
t_disable(void)
{
    int count;

    count = KSEG1_IO_ASIC->ct2;
    KSEG1_IO_ASIC->ccr &= ~CCR_CT2_EN;
    flush_io_wb();
    return(count);
}
 
/*
 * t_enable
 * enable timer 2 and load with count
 */
void
t_enable(unsigned short count)
{
    KSEG1_IO_ASIC->ct2 = count;
    KSEG1_IO_ASIC->ccr |= CCR_CT2_EN;
    flush_io_wb();
}

/*
 * alarm : Setup the alarm mechanism
 * If cnt == 0, disable the alarm mechanism, else initialize the mechanism
 * and set alarm_setting to cnt. cnt is the number of 1ms intervals that
 * pass before the sigalrmfcn signal will be called. Return the amount of
 * time left before the alarm would go off.
 */
int
alarm(int cnt)
{
    int tmp = alarm_count;

    t_disable();              /* Ensure mutual exclusion on variables */
    alarm_setting = cnt;      /* Initialize the alarm settings */
    alarm_count = cnt;
    if ( cnt ) {
	t_enable(ALARM_FREQUENCY);  /* start the timer */
    }
    return(tmp);
}

/*
** pause - stop until alarm signal or a user interrupt
*/
int
pause(void)
{
    if(alarm_count) while(alarm_count);
    else while(1);
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
    int tmp;
    SIG_PF sigsav;

    tmp = t_disable();     /* ensure mutual exclusion on variables */
    if(alarm_count && alarm_count < msecs) {
	/* alarm is currently in use */
	msecs -= alarm_count;
	t_enable(tmp ? tmp : 1);
	pause();
    }
    sigsav = signal(SIGALRM, SIG_IGN);
    tmp = alarm(msecs);
    pause();
    signal(SIGALRM, sigsav);
    if(tmp > msecs) alarm(tmp - msecs);
    else alarm(0);
}

/* End of Module */

