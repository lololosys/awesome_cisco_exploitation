/* $Id: stptest.c,v 3.2.62.2 1996/09/10 03:22:55 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ts/stptest.c,v $
 *------------------------------------------------------------------
 * stptest.c -- Routines for the STP line card diagnostic
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stptest.c,v $
 * Revision 3.2.62.2  1996/09/10  03:22:55  snyder
 * CSCdi68568:  more constant opportuniites
 *              216 out of data
 *               52 out of image
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/03/18  22:20:09  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:58:13  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:25:15  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:56:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:40:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/09  13:22:09  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:10:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ctype.h>
#undef   toupper			/* use library function call */
#include "stacks.h"
#include "ttysrv.h"
#include "stp.h"
#include "../ts/s2681.h"
#include "../os/async.h"

#define	NDUARTS	8
#define	TIMERLIMIT 3000
#define	TXLIMIT 5
#define NBAUDS (S1BAUD19200+1)
#define STPTEST_BUFSIZ 50

/*
 * Forward and external declarations
 */
extern const int baudtable[];
extern void STPinterrupt(void);
void modem_flap(uartaddress *duart, int line);
void stpinit(uartaddress *addr, int n, int loopflag);
void stpintr(void);
void stpbaud(uartaddress *addr, int line, int baud);
void stpdisable(uartaddress *addr, int line);
void stpenable(uartaddress *addr, int line);
void stptxservice(uartaddress *addr);
void stprxservice(uartaddress *addr);

/*
 * Local Storage
 */
short climit;
volatile short tcount;
volatile short rcount;
short RXUspurious, RXLspurious;
short TXUspurious, TXLspurious;
uartaddress *teststp;
uartaddress *testduart;
boolean stpverbose;
char buf1[STPTEST_BUFSIZ];
char buf2[STPTEST_BUFSIZ];

/*
 * ecotest
 * Check whether the board seems to need the CSC/3 ECO (only works on
 * a csc/3 processor).  Write registers and see if we can reliably
 * read them back.
 */
static const uchar ecodata[8] = { 0x33, 0xCC, 0xAA, 0x55, 
                                  0x33, 0xCC, 0xAA, 0x55 };

boolean ecotest (uartaddress *stp)
{
    int i, result = TRUE;
    leveltype status;
    status = raise_interrupt_level(ALL_DISABLE);

    stp->commandA = RESETMRPTR;
    stp->modeA = RXRDYINTR|CHARERROR|PRTYOFF|DBITS8;
    stp->modeA = (MODELOCALLOOP|STOP16X16);

    for (i=0; i < 8; i+=2) {
	stp->dataA = ecodata[i];
	stp->dataA = ecodata[i+1];
	DELAY(10);
	if ((stp->statusA & RXREADY) && stp->dataA == ecodata[i])
	    if ((stp->statusA & RXREADY) && stp->dataA == ecodata[i+1])
		continue;
	result = FALSE;
	break;
    }
    stp->commandA = RESETMRPTR;
    stp->modeA = RXRDYINTR|CHARERROR|PRTYOFF|DBITS8;
    stp->modeA = (MODENORMAL|STOP16X16);
    reset_interrupt_level(status);
    return(FALSE);
}

/*
 * test_STPlines
 * Test STP async serial line cards
 */

void test_STPlines (void)
{
    int i, j, k;
    int unit, bline, eline, skip;
    boolean loopflag;
    uartaddress *addr, *duart;
    leveltype status;
    boolean parsed;

    if (stdio->type != CTY_LINE) {
	printf("Must be on console line");
	return;
    }
    printf("Serial line card diagnostic\n\n");

    /*
     * Get unit number, determine base address, loopback status
     */
    unit = 0;
    printf("Serial line card unit number [%d]? ", unit);
    if (rdtty(buf1, STPTEST_BUFSIZ)) {
        i = parse_unsigned(buf1, &parsed);
        if (parsed)
	    unit = i;
    }
    bline = 0;
    printf("Starting DUART number (0-7) to test [%d]? ", bline);
    if (rdtty(buf1, STPTEST_BUFSIZ)) {
        i = parse_unsigned(buf1, &parsed);
        if (parsed)
	    bline = i;
    }
    eline = NDUARTS;
    printf("Ending DUART number (0-7) to test [%d]? ", eline-1);
    if (rdtty(buf1, STPTEST_BUFSIZ)) {
        i = parse_unsigned(buf1, &parsed);
        if (parsed)
	    eline = i + 1;
    }
    addr = &UARTSTART[unit];
    printf("Serial line card base address [%#x]? ", addr);
    if (rdtty(buf1, STPTEST_BUFSIZ)) {
        i = parse_unsigned(buf1, &parsed);
        if (parsed)
    	    addr = (uartaddress *)((ulong) i);
    }
    teststp = addr;
    if (!ecotest(teststp))
	printf("\n%% STP unit %d does not have CSC/3 ECO\n", unit);
    printf("Internal loopback [y]? ");
    buf1[0] = '\0';
    rdtty(buf1, STPTEST_BUFSIZ);
    loopflag = ((toupper(buf1[0]) == 'Y') || !buf1[0]) ? FALSE : TRUE;
    printf("Verbose mode [n]? ");
    buf1[0] = '\0';
    rdtty(buf1, STPTEST_BUFSIZ);
    stpverbose = ((toupper(buf1[0]) == 'N') || !buf1[0]) ? FALSE : TRUE;
    psion(stdio);

    /*
     * Announce what we're doing
     */
    printf("\nTesting card %d, lines %d-%d, at %#x using %sternal loopback.",
        unit, bline*2, eline*2-1, addr, loopflag ? "ex" : "in");

    /*
     * Check that all registers can be read
     */
    printf("\nReading all registers...");
    for (i = 0; i < ((sizeof(uartaddress)) >> 1); i++) {
        if (!touch_device(((char *)addr+i*(sizeof(short))))) {
	    printf("\nUnable to read register %d", i);
	    if (i == 0)
    	        printf(", no such device?");
 	    printc('\n');
    	    return;
        }
    }

    /*
     * Now repeat loop to unwedge UART 0
     */
    for (i = 0; i < ((sizeof(uartaddress)) >> 1); i++) {
        if (!touch_device((char *)addr+i*(sizeof(short)))) {
    	    printf("\nUnable to read register %d", i);
    	    if (i == 0)
    	        printf(", no such device?");
    	    printc('\n');
    	    return;
        }
    }

    /*
     * Now check that at least the low 8 bits all are working
     * by checking all 256 patterns to uart 0 reg 0.
     */
    printf("[OK]\nTesting 8 bit bus...");
    for (i = 0; i < 256; i++) {
        addr->commandA = RESETMRPTR;
        addr->modeA = (ushort) i;
        addr->commandA = RESETMRPTR;
        j = (addr->modeA & 0xFF);
        if (j != i)
	    printf("\nLow bus failure: wrote %#x, read %#x", i, j);
    }

    /*
     * Check address decoding to see if we can touch all 8
     * UART chips. Do this by writing a pattern into each
     * chip mode register and then reading it back.
     */
    if (psipending()) {
	psioff(stdio);
	return;
    }
    printf("[OK]\nTesting basic address decoding...");
    for (i = bline; i < eline; i++) {
	duart = (uartaddress *) & addr[i];
	duart->commandA = RESETMRPTR;
	duart->modeA = (ushort) (129 + i);
    }
    for (i = bline; i < eline; i++) {
	duart = (uartaddress *) & addr[i];
	duart->commandA = RESETMRPTR;
	j = (int) (duart->modeA & 0xFF);
	if (j != 129 + i)
	    printf("\nAddress test UART%d got %#x, not %#x", i, j, 129+i);
    }

    /*
     * Do internal or external loopback transmit/receive tests
     */
    if (psipending()) {
	psioff(stdio);
        return;
    }
    printf("[OK]\nStarting loopback tests...\n");
    changelevel(LEVEL2, stpintr, NULL);
    stpinit(addr, NDUARTS, loopflag);
    status = set_interrupt_level(ALL_ENABLE);
    for (i = bline*2; (i < 2*eline) && !psipending(); i++) {
	RXUspurious = RXLspurious = TXUspurious = TXLspurious = 0;
    	printf("\nLine #%d:", i);
    	duart = (uartaddress *) & addr[i>>1];
	testduart = duart;
	
    	if (loopflag)
	    modem_flap(duart,i);
    	skip = 0;
    	for (j = 0; (j < NBAUDS) && !psipending() && (skip < 20); j++) {
	    printf(" %d", baudtable[j]);
    	    stpbaud(duart, i, j);
	    process_sleep_for(ONESEC/5);
    	    climit = 5 * j + TXLIMIT;
    	    if (climit > STPTEST_BUFSIZ/2)
    		climit = STPTEST_BUFSIZ/2;
    	    for (k = 0; k < climit; k++)
    		buf1[k] = i + k + 65;
    	    k = 0;
    	    tcount = 0;	
    	    rcount = 0;	
    	    stpenable(duart, i);
    	    while (k < TIMERLIMIT && tcount < climit) {
    		DELAY(1);
		k++;
	    }
    	    if (k >= TIMERLIMIT) {
    		printf("\nTX timeout after sending %d chars of %d possible\n",
    				tcount,climit);
    		skip++;
    		stpdisable(duart, i);
    		continue;
    	    }
    	    if (rcount != tcount && k < TIMERLIMIT) {
    		k = 0;
    		while (tcount != rcount && k < TIMERLIMIT) {
		    DELAY(1);
		    k++;
		}
		stpdisable(duart, i);
    		if (k >= TIMERLIMIT) {
    		    printf("\nRX timeout after rcving %d chars of %d sent\n",
    					rcount, tcount);
		    skip++;
    		    continue;
		}
	    } else
    		stpdisable(duart, i);
    	    for (k = 0; k < climit; k++) {
    		if (buf1[k] != buf2[k]) {
    		    printf("\nMismatch at loc %d, should be %#x, is %#x\n",
			    k, buf1[k]&0xFF, buf2[k]&0xFF);
		    skip++;
    		    break;
		}
	    }
    	}
	if (RXUspurious) {
	    printf("\nSpurious RX boardstatus, uarts");
	    for (k=0; k < 8; k++)
		if (RXUspurious & (1<<k))
		    printf (" %d", k);
	}
	if (RXLspurious) {
	    printf("\nSpurious RX characters, lines");
	    for (k=0; k < 16; k++)
		if (RXLspurious & (1<<k))
		    printf (" %d", k);
	}
	if (TXUspurious) {
	    printf("\nSpurious TX boardstatus, uarts");
	    for (k=0; k < 8; k++)
		if (TXUspurious & (1<<k))
		    printf (" %d", k);
	}
	if (TXLspurious) {
	    printf("\nSpurious TX characters, lines");
	    for (k=0; k < 16; k++)
		if (TXLspurious & (1<<k))
		    printf (" %d", k);
	}
    }
    printf("\n\nDone testing card %d in %sternal loopback\n",
    		unit, loopflag ? "ex" : "in");
    changelevel(LEVEL2, STPinterrupt, "RS232 serial line cards");
    psioff(stdio);
    i = unit * 16 + bline*2 + 1;
    j = unit * 16 + eline*2;
    printf("\nResetting lines %t through %t", i, j);
    for (k=i; k <= j; k++) {
	tty_clearbuffers(MODEMS[k]);
	tty_reset(MODEMS[k]);
    }
    reset_interrupt_level(status);
}

/*
 * modem_flap
 * Make sure CTS and RING inputs follow DTR
 */

void modem_flap (uartaddress *duart, int line)
{
    ushort dtrbit, ctsbit, ringbit, status;
    tt_soc *tty = MODEMS[line];

    dtrbit = (line & 1) ? DTRB : DTRA;
    ctsbit = (line & 1) ? CTSB : CTSA;
    ringbit = (line & 1) ? RINGB : RINGA;
    duart->resetport = dtrbit;
    tty->modem_bits &= ~MODEMBIT_DTR;	/* Update saved modem state */
    status = ~duart->portchange;
    if (status & ctsbit)
	printf("\nCTS did not go down with DTR");
    if (status & ringbit)
	printf("\nRING did not go down with DTR");
    duart->setport = dtrbit;
    tty->modem_bits |= MODEMBIT_DTR;	/* Update saved modem state */
    status = ~duart->portchange;
    if (!(status & ctsbit))
	printf("\nCTS did not go up with DTR");
    if (!(status & ringbit))
	printf("\nRING did not up down with DTR");
}

/*
 * stpinit
 * Initialize a DUART.
 */

void stpinit (uartaddress *addr, int n, int loopflag)
{
    uartaddress *stp;

    for (stp = addr; stp < &addr[n]; stp++) {
    	stp->commandA = RESETMRPTR;
    	stp->commandA = RESETTX;
    	stp->commandA = RESETRX;
    	stp->commandA = RESETERROR;
    	stp->commandA = RESETMRPTR;
    	stp->modeA = RXRDYINTR|CHARERROR|PRTYOFF|DBITS8;
    	stp->modeA = loopflag ? (MODENORMAL|STOP16X16) : (MODELOCALLOOP|STOP16X16);
    	stp->commandA = RESETMRPTR;
    	stp->statusA = (RXCLOCK*S1BAUD9600)|(TXCLOCK*S1BAUD9600);
    	stp->commandA = TXDISABLE|RXDISABLE;
    	stp->portchange = BRGSET1|RINGBXENB|RINGAXENB|CTSBXENB|CTSAXENB;
    	stp->interrupt= RXRDYB|TXRDYB|RXRDYA|TXRDYA;
    	stp->CTUpper = 0;
    	stp->CTLower = 2;
    	stp->commandB = RESETMRPTR;
    	stp->commandB = RESETTX;
    	stp->commandB = RESETRX;
    	stp->commandB = RESETERROR;
    	stp->commandB = RESETMRPTR;
    	stp->modeB = RXRDYINTR|CHARERROR|PRTYOFF|DBITS8;
    	stp->modeB = loopflag ? (MODENORMAL|STOP16X16) : (MODELOCALLOOP|STOP16X16);
    	stp->statusB = (RXCLOCK*S1BAUD9600)|(TXCLOCK*S1BAUD9600);
    	stp->portstatus = OPRXRDYA|OPTXRDYA|OPRXRDYB|OPTXRDYB;
    	stp->commandB = TXDISABLE|RXDISABLE;
    	stp->resetport = 255;
/*
 * Ought to clear the modem_bits, but don't have a handle on
 * what tty we are looking at.
 */
    }
}

/*
 * stpbaud
 * Set baudrate for a particular line
 */

void stpbaud (uartaddress *addr, int line, int baud)
{
    if (line & 1)
    	addr->statusB = (RXCLOCK*baud)|(TXCLOCK*baud);
    else
    	addr->statusA = (RXCLOCK*baud)|(TXCLOCK*baud);
}

/*
 * stpdisable
 * Disable TX and RX interrupts
 */

void stpdisable (uartaddress *addr, int line)
{
    if (line & 1)
    	addr->commandB = TXDISABLE + RXDISABLE;
    else
    	addr->commandA = TXDISABLE + RXDISABLE;
}

/*
 * stpenable
 * Enable TX and RX interrupts for a line
 */

void stpenable (uartaddress *addr, int line)
{
    addr->interrupt= RXRDYB|TXRDYB|RXRDYA|TXRDYA;
    if (line & 1) {
    	addr->commandB = RESETTX;
    	addr->commandB = RESETRX;
    	addr->commandB = RESETERROR;
    	addr->commandB = STP_TXENABLE + STP_RXENABLE;
    } else {
    	addr->commandA = RESETTX;
    	addr->commandA = RESETRX;
    	addr->commandA = RESETERROR;
    	addr->commandA = STP_TXENABLE + STP_RXENABLE;
    }
}    

/*
 * stpintr
 * All STP interrupts come here
 */

void stpintr (void)
{
    uartaddress *addr = teststp;
    ushort status;

    if (((&addr[1])->boardstatus | (&addr[2])->boardstatus) == 0) {
	STPinterrupt();
	return;
    }
    status = (&addr[1])->boardstatus;
    if (status) {			/* check RXRDY status register */
    	if (status & 0x0003)
	    stprxservice(&addr[0]);
    	if (status & 0x000C)
	    stprxservice(&addr[1]);
    	if (status & 0x0030)
	    stprxservice(&addr[2]);
    	if (status & 0x00C0)
	    stprxservice(&addr[3]);
    	if (status & 0x0300)
	    stprxservice(&addr[4]);
    	if (status & 0x0C00)
	    stprxservice(&addr[5]);
    	if (status & 0x3000)
	    stprxservice(&addr[6]);
    	if (status & 0xC000)
	    stprxservice(&addr[7]);
    }
    status = (&addr[2])->boardstatus;
    if (status) {			/* check TXRDY status register */
    	if (status & 0x0003)
	    stptxservice(&addr[0]);
    	if (status & 0x000C)
	    stptxservice(&addr[1]);
    	if (status & 0x0030)
	    stptxservice(&addr[2]);
    	if (status & 0x00C0)
	    stptxservice(&addr[3]);
    	if (status & 0x0300)
	    stptxservice(&addr[4]);
    	if (status & 0x0C00)
	    stptxservice(&addr[5]);
    	if (status & 0x3000)
	    stptxservice(&addr[6]);
    	if (status & 0xC000)
	    stptxservice(&addr[7]);
    }
}

/*
 * stprxservice
 * Process an RX interrupt condition
 */

void stprxservice (uartaddress *addr)
{
    char *cp;
    ushort status;

    if (addr != testduart) {
		RXUspurious |= 1<<(addr-teststp);
	while ((status = addr->statusA) & RXREADY) {
	    char c;
	    RXLspurious |= 1<< (2*(addr-teststp));
	    c = addr->dataA;
	}
	while ((status = addr->statusB) & RXREADY) {
	    char c;
	    RXLspurious |= 1<< (2*(addr-teststp)+1);
	    c = addr->dataB;
	}
    return;
    }	
    while ((status = addr->statusA) & RXREADY) {
    	cp = &buf2[rcount++];
    	*cp = (char) addr->dataA;
    	if (stpverbose) {
    	    if (status & (RCVBREAK+FRMERROR)) {
		printf(" *%-02x", *cp & 0xFF);
		rcount--;
    	    } else
		printf(" r%-02x", *cp & 0xFF);
    	}
    }
    while ((status = addr->statusB) & RXREADY) {
    	cp = &buf2[rcount++];
    	*cp = (char) addr->dataB;
    	if (stpverbose) {
	    if (status & (RCVBREAK+FRMERROR)) {
		printf(" *%-02x", *cp & 0xFF);
		rcount--;
    	    } else
    		printf(" r%-02x", *cp & 0xFF);
    	}
    }
}

/*
 * stptxservice
 * Process a TX interrupt
 */

void stptxservice (uartaddress *addr)
{
    if (addr != testduart) {
	TXUspurious |= 1<<(addr-teststp);
	while ((addr->statusA) & TXREADY) {
	    TXLspurious |= 1<< (2*(addr-teststp));
	    addr->dataA = 0;
	}
	while ((addr->statusB) & TXREADY) {
	    TXLspurious |= 1<< (2*(addr-teststp)+1);
	    addr->dataB = 0;
	}
	return;
    }	

    if (tcount < climit) {
    	if (addr->statusA & TXREADY) {	/* fifo hungry */
	    addr->dataA = (ushort) buf1[tcount]; /* yes */
	    if (stpverbose)
		printf(" t%-02x", buf1[tcount] & 0xFF);
    	    tcount++;
    	}
    	if (addr->statusB & TXREADY) {
    	    addr->dataB = (ushort) buf1[tcount];
    	    if (stpverbose)
		printf(" t%-02x", buf1[tcount] & 0xFF);
	    tcount++;
    	}
    } else {
    	if (addr->statusA & TXEMPTY)	/* fifo empty? */
	    addr->commandA = TXDISABLE;/* yes, disable tx */ 
    	if (addr->statusB & TXEMPTY)
	    addr->commandB = TXDISABLE;
    }
}
