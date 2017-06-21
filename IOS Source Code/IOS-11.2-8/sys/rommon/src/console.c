/* $Id: console.c,v 3.3.28.3 1996/07/08 22:42:33 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src/console.c,v $
 *------------------------------------------------------------------
 * console.c
 *
 * April 1993, Rob Clevenger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Console I/O routines.
 *------------------------------------------------------------------
 * $Log: console.c,v $
 * Revision 3.3.28.3  1996/07/08  22:42:33  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.3.28.2  1996/06/16  21:18:09  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.3.28.1  1996/03/21  23:26:45  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/02/08  08:57:48  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.1  1995/12/01  04:19:56  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.3  1996/01/17  23:30:12  mansonw
 * CSCdi47058:  Fix the broken ROMMON build
 *
 * Revision 3.2  1995/11/17  18:41:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:04:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:36:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
** Console routines.
*/
#include "monitor.h"
#include "console.h"
#include "mon_defs.h"
#include "signal_4k.h"
#include "confreg.h"

#ifdef RSP2 
#include "queue.h"
extern boolean dbus_hooked;
#endif

/*
 * Note the S1BAUD - must set bit 7 in the ACR (BRGSET1):
 */
unsigned char UARTspeeds[] = {
    S1BAUD9600,				/* 9600 */
    S1BAUD4800,				/* 4800 */
    S1BAUD1200,				/* 1200 */
    S1BAUD2400,				/* 2400 */
    S1BAUD300,				/* 300 */
    S1BAUD600,				/* 600 */
    S1BAUD110,				/* 110 */
    S1BAUD9600				/* 9600 or something else */
};

/*
** Enable the console receiver and xmitter
*/
enacon(void)
{
    uartaddr->a.command = RESETDBREAK|STP_TXENABLE|STP_RXENABLE;
}

/*
** Disable console interrupts
*/
disconint(void)
{
    uartaddr->a.aux2 = 0;	/* disable all interrupts */
}


#ifdef VIRTUAL_CONSOLE
extern vc_t *vcp;
 
void
uartout(char c)
{
    while ((uartaddr->a.state & TXREADY) == 0)
        ;                       /* wait for ready */
    uartaddr->a.data = c;
}
 
int 
uartin(void)
{
    if (uartaddr->a.state & RXREADY)
        return(uartaddr->a.data);
    return(-1);
}
#endif


/*
** Raw console output routine.
*/
short linepos, linecnt;

void
conout(char c)
{
#ifdef VIRTUAL_CONSOLE
    (check_slave() && dbus_hooked) ? vcp->putc(c) : uartout(c);
#else
    while ((uartaddr->a.state & TXREADY) == 0)
        ;                       /* wait for ready */
    uartaddr->a.data = c;
#endif
}

/*
** Raw console input routine.
*/
int
conin(void)
{
#ifdef VIRTUAL_CONSOLE
    return (check_slave() ? vcp->getc() : uartin());
#else
    if (uartaddr->a.state & RXREADY)
        return(uartaddr->a.data);
    return(-1);
#endif
}

/*
** Console status routine.
** Returns true (1) if there is a character to be had, false (0) otherwise.
*/
int
constat(void)
{
    if (uartaddr->a.state & RXREADY) return(1);
    return(0);  /* no character */
}

/*
** Return a character from the console device.
** This routine blocks if the input queue is empty.
** This routine throws away break chars.
*/
char
getchar(void)
{
  register char c;

  while((c = conin()) < 0 || c == 0); /* ignore breaks */
  return(c & 0177);	/* strip parity */
}

/*
** Put a character on the console device.
** Keep track of the char count on line (linepos) for the clrline()
** routine.
** Also supply the "more" (pagination) mechanism.
*/
#define MAXLINES 22
void
putchar(char c)
{
    conout(c);  /* write to console */
    switch(c) {
    case '\r':
	linepos = 0;
	break;
    case '\b':
	if(linepos) linepos--;
	break;
    case '\n':
	conout('\r'); /* expand newlines */
	linepos = 0;
	if(hkeepflags & H_MORE && ++linecnt >= MAXLINES) {
	    linecnt = 0;
	    puts("*** MORE ***  ");
	    c = getchar();
	    clrline("\0");
	    if(c == 'q' && monjmpptr)
		longjmp(monjmpptr, 3);  /* quiet termination */
	    if(c != ' ')
		linecnt = MAXLINES - 1;
	}
	break;
    default:
	linepos++;
	break;
    }
}

void
setmore(void)
{
    hkeepflags |= H_MORE;
    linecnt = 0;
}

/*
** The console interrupt service routine.
**
** Handle a console interrupt on break.
** Actually, two interrupts happen on a <break> keystroke, break-on and
** break-off.  The code is written to ignore break-offs.
*/

static char breakon;
static char coninttest;
extern int (*sigintfcn)();

long
conint(void)
{
#if !defined(SIERRA) && !defined(RSP) && !defined(RSP2) && !defined(PREDATOR) && !defined(SVIP)
    uartaddr->a.command = RESETDBREAK;  /* reset the interrupt */

    if(breakon) {  /* ignore the second interrupt */
	breakon = 0;
	return((long)0);
    }
    breakon = 1;
    hkeepflags |= H_USRINT;  /* set the user interrupt flag */
    switch((int)sigintfcn) {
    case SIG_IGN:  /* do nothing */
	return((long)0);
    case SIG_DFL:  /* the default action for <break> */
	break;
    default:
	if( (*sigintfcn)() == (int)SIG_DFL) break;
	return((long)0);
    }
    /*
    ** Return the new rte PC to be put on the stack by
    ** the exception wrapper in start.S
    */
    if(monjmpptr)
	return((long)&default_break);
#else
    if(coninttest) {
	(*sigintfcn)();
	return((long)1);
    }
    return((long)0);
#endif
}

#if defined(SIERRA) || defined(RSP) || defined(RSP2) || defined(PREDATOR) || defined(SVIP)
/*
** For Sierra we do not use console interrupts for the break mechanism,
** we use the count/compare interrupt and poll the UART for a break
** condition.
*/
long
isconbrk()
{
    int c;
    long retval = 0;

    /*
     * Check the break key for Abort
     * We have to wait for the break condition to end:
     */
    if(uartaddr->a.aux2 & BREAKXA) {
	breakon = 1;
	uartaddr->a.command = RESETERROR;
	uartaddr->a.command = RESETDBREAK;
    } else if(breakon) {
	hkeepflags |= H_USRINT;  /* set the user interrupt flag */
	retval = (long)&default_break;
	breakon = 0;
    }
    return(retval);
}
#endif  /* SIERRA || RSP || RSP2 */


/*
** The default action for <break> is to longjmp to monjmpptr.
**
** This must be done outside of the interrupt service routine
** in order to execute the rte instruction.
*/
void
default_break(void)
{
#if defined(SIERRA) || defined(RSP) || defined(RSP2) || defined(PREDATOR) ||defined(SVIP)
    extern int sys_state;

    sys_state = MONITOR_RUNNING;
    cache_flush(0, -1);
#endif /* SIERRA || RSP || RSP2 || PREDATOR */

    if(monjmpptr) longjmp(monjmpptr,1);
}

static int
testbrkint(void)
{
    puts("break interrupt received\n");
    return((int)SIG_IGN);
}

/*
** Console <break> interrupt test.
*/
int
conbrkinttest(void)
{
    SIG_PF savfcn;

    testname("console break interrupt");
    coninttest = 1;
    savfcn = signal(SIGINT, (SIG_PF)testbrkint);
    while(getchar() == '\000')
	puts("Hit the break key to test, any other char to quit\n");
    coninttest = 0;
    signal(SIGINT, savfcn);
    return(0);
}

void flush()
{
    /* make sure transmitter is enabled */
    uartaddr->a.command = STP_TXENABLE|STP_RXENABLE;
    /*
     * Wait until the transmitter is empty:
     */
    while ((uartaddr->a.state & TXEMPTY) == 0)
        ;
}

/*
** Handy console routines for calling from assembler.
*/

void
init_con(int baud_index)
{
    uartaddr->a.aux2 = 0;       /* Turn off interrupts (IMR=0) */
    uartaddr->a.command = RESETMRPTR;
    uartaddr->a.mode = CHARERROR | PRTYOFF | DBITS8;
    uartaddr->a.mode = MODENORMAL | STOP16X16;
    uartaddr->a.aux1 = BRGSET1 | TIMXTAL;
    uartaddr->a.state = (RXCLOCK * UARTspeeds[baud_index]) |
                        (TXCLOCK * UARTspeeds[baud_index]);
    uartaddr->a.command = RESETDBREAK|STP_TXENABLE|STP_RXENABLE;
    uartaddr->b.aux2 = 0;       /* All outputs are driven by OPR */
    uartaddr->b.aux3 = CDA;     /* Set Carrier detect on Console port */
 
}

void
re_init_con(void)
{
    int i;
    /*
     *  Loop to make sure transmitter is empty
     */
    for(i=0; i<0x10000; i++) ;
 
    init_con(((NVRAM)->confreg & CONSOLE_MASK) >> CONSOLE_SHIFT);
}

/* end of module */
