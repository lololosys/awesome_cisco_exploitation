/* $Id: tcalibrate.c,v 3.2.58.1 1996/03/21 23:31:09 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-c4000/tcalibrate.c,v $
 *------------------------------------------------------------------
 * tcalibrate.c
 *
 * June 1993, Michael Beesley and Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Timer Calibration for Nevada ASIC.
 *------------------------------------------------------------------
 * $Log: tcalibrate.c,v $
 * Revision 3.2.58.1  1996/03/21  23:31:09  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:23:37  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:46:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:40:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



/*
** Ported from DiagMon tcalibrate for XX
**
** Enhanced with a better algorithm 3/94 - clev
*/
#include "monitor.h"
#include "signal_4k.h"

/*
 * Local variable
 */
static SIG_PF savfcn;

/*
 * Function declarations
 */
static cleanup();
void wastetime();

/*
 * Constants
 */
#define C0 10
#define C1 100
#define C2 1000
#define C3 10000
#define PRESH 8  /* shift for precision */

long
timer_calibrate (long t) 
{
    long count, offset, countcmp, locount, hicount, loopcount;
    long overhead, t0, t1, t2, t3;
    long slope, tmeasured, temp;
    
    savfcn = signal(SIGINT, (SIG_PF)cleanup);
    
    overhead = time_it(0);
#ifdef DEBUG
    printf("overhead is %d\n", overhead);
#endif

    if(t < overhead) count = 0;
    else {

	/*
	** Get 2 points on a line and find the slope (delta y / delta x) and
	** the offset (y crossing).
	** Use the formula y = mx + b to calculate other points on the line
	** (m is the slope and b is the offset)
	** For our purposes the x axis is the time desired and the y axis is
	** the count needed to pass to wastetime().
	*/
	t0 = time_it(C0);
#ifdef DEBUG
    t1 = time_it(C1);
    t2 = time_it(C2);
#endif
	t3 = time_it(C3);

#ifdef DEBUG
printf("\noverhead = %d, t0 = %d, t1 = %d, t2 = %d, t3 = %d\n",
       overhead, t0, t1, t2, t3);
#endif

	/*
	** bump top value in order to get better precision and
	** compensate later
	** calculate the slope and the offset ( << 8 )
	** and the minimum time (where the line crosses the x axis)
	*/
	slope  = ((t3 - t0) << PRESH) / (C3 - C0);
	offset = (t3 << PRESH) - (slope * C3);

#ifdef DEBUG
printf("\nslope0 = %d, slope1 = %d, slope2 = %d, slope = %d,\n\
offset = %d, offsetchk = %d, mintime = %d\n",
       ((t1 - t0) << PRESH) / (C1 - C0),
       ((t2 - t1) << PRESH) / (C2 - C1),
       ((t3 - t2) << PRESH) / (C3 - C2),
       slope, offset, (t0 << PRESH) - (slope * C0), offset >> PRESH);
    printf("slope and offset are * 256\n");
#endif

	count = (( (t << PRESH) - offset ) / slope ) + 1;

	/* get as close as possible to the desired time */
	locount = hicount = 0;
	for(loopcount = 0; loopcount < 25; loopcount++) {
	    if(count <= 0) count = 1;  /* no zero divides, etc. */
	    tmeasured = time_it(count);

#ifdef DEBUG
    printf("calculated return count for time (t) %d usecs is %d, loop: %d\n",
	   t,count, loopcount);
    printf("actual time is %d\n", tmeasured);
    printf("actual mintime is %d\n", time_it(0));
#endif
	    countcmp = count;  /* remember value going in */
	    if(tmeasured == t) break;
	    if(tmeasured < t) {  /* less than desired time */
		locount = count;
	    } else {
		hicount = count;
	    }
	    if(locount && hicount) {
		count = (locount + hicount) / 2;  /* take the average */
	    } else {
		if(locount) {
		    temp = t/tmeasured;
		    if(temp < 2) temp = 2;
		    count *= temp;
		} else {
		    temp = tmeasured/t;
		    if(temp < 2) temp = 2;
		    count /= temp;
		}
	    }
#ifdef DEBUG
    printf("count after locount/hicount calc: %d\n",count);
#endif
	    if(countcmp == count) break;
	}
    }
    cleanup();
    signal(SIGINT,savfcn);
    return(count);
}

static int
cleanup(void)
{
    KSEG1_IO_ASIC->ccr &= ~CCR_CT1_EN;   /* stop counter 1            */
    return((int)SIG_DFL);           /* do the default action too */
}

static timerinit;

int 
time_it (int cnt)
{
    unsigned short t0 = 0xff00;  /* big number to count down */
    unsigned short tmp;
    
    cleanup();                        /* stop counter 1  */
    KSEG1_IO_ASIC->ct1 = t0;               /* load counter 1  */
    KSEG1_IO_ASIC->ccr |= CCR_CT1_EN;      /* start counter 1 */
    wastetime(cnt);
    tmp = KSEG1_IO_ASIC->ct1;              /* get count       */
    cleanup();                        /* stop counter 1  */
    if(!timerinit) {  /* for some reason timer is screwy first time */
	timerinit = 1;
	return(time_it(cnt));
    }
    return(t0 - tmp);
}

void 
wastetime (long n)
{
    while(n > 0) n--;
}

int
tcal(int argc, char *argv[])
{
    long value, tcval;

    if(argc != 2) {
	printf("usage: %s value\n", argv[0]);
	return(1);
    }
    if(getnum(argv[1],10,&value) == 0) return(1);
    if (value > 39999) {
	printf("%s : count must be less than 40000\n", argv[0]);
	return(1);
    }
    tcval = timer_calibrate(value);
    printf("actual time for value of %d is %d, count is %d\n",
	   value, time_it(tcval), tcval);
    return(0);
}

/* End of Module */
