/* $Id: stptest.c,v 3.2 1995/11/17 07:41:49 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/stptest.c,v $
 *------------------------------------------------------------------
 * stptest.c -- Routines for the STP line card diagnostic
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1992 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stptest.c,v $
 * Revision 3.2  1995/11/17  07:41:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:12:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <sys/types.h>
#include "defs.h"
#include "stdio.h"
#include "ctype.h"
#include "../../sys/h/s2681.h"
#include "../../sys/ts/stp.h"
#include "m68vectors.h"
#include "reason.h"
#include "extern.h"
#include "test.h"

#define	NDUARTS	8
#define	TIMERLIMIT 0x100000
#define	TXLIMIT 5

/*
 * Note that we are using baud rate generator #1, not #0
 */

#define NBAUDS (S1BAUD19200+1)

ushort baudtable[NBAUDS] = {
75, 110, 134, 150, 300, 600, 1200, 2000, 2400, 4800, 1800, 9600, 19200
};


/*
 * Variables unique to this module
 */
short climit;
short tcount;
short rcount;
uartaddress *teststp;
char stpverbose;

long stplev2();				/* assembler caller of stpintr */

stptest()
{
	register int i, j, k;
	int unit, bline, eline, loopflag, handler, skip;
	uartaddress *addr, *duart;

	printf("Serial line card diagnostic\n\n");

	/*
	 * Get unit number, determine base address, loopback status
	 */
	unit = 0;
	printf("Serial line card unit number [%d]? ", unit);
	gets(buf);
	if ((i = getnum(buf)) >= 0)
		unit = i;
	bline = 0;
	printf("Starting DUART number (0-7) to test [%d]? ", bline);
	gets(buf);
	if ((i = getnum(buf)) >= 0)
	    bline = i;
	eline = NDUARTS;
	printf("Ending DUART number (0-7) to test [%d]? ", eline-1);
	gets(buf);
	if ((i = getnum(buf)) >= 0)
	    eline = i + 1;
	printf("Serial line card base address [%#x]? ", &UARTSTART[unit]);
	gets(buf);
	if ((i = gethexnum(buf)) >= 0)
		addr = (uartaddress *)((u_long) i);
	else
		addr = &UARTSTART[unit];
	teststp = addr;
	printf("Internal loopback [y]? ");
	gets(buf);
	loopflag = ((buf[0] == 'Y') || !buf[0]) ? FALSE : TRUE;
	printf("Verbose mode [n]? ");
	gets(buf);
	stpverbose = ((buf[0] == 'N') || !buf[0]) ? FALSE : TRUE;
	abort = FALSE;

	/*
	 * Announce what we're doing
	 */
	printf("\nTesting serial line card %d, lines %d-%d, at %#x using %sternal loopback.",
	    unit, bline, eline, addr, loopflag ? "ex" : "in");
	/*
	 * Check that all registers can be read
	 */
	abortstate = ABORT_INTERNAL;
	buserrhandler = buserrnote;
	buserrcnt = 0;
	printf("\nReading all registers...");
	for (i = 0; i < ((sizeof(uartaddress)) >> 1) && !abort; i++) {
		j = *(short *)((char *)addr + i*(sizeof(short)));
		if (buserrcnt) {
			buserrhandler = 0;
			printf("Unable to read register %d", i);
			if (i == 0)
				printf(", no such device?");
			putchar('\n');
			abortstate = ABORT_IGNORE;
			abort = FALSE;
			buserrhandler = 0;
			return;
		}
	}

	/*
	 * Now check that at least the low 8 bits all are working
	 * by checking all 256 patterns to uart 0 reg 0.
	 */
	printf("[OK]\nTesting 8 bit bus...");
	for (i = 0; i < 256 && !abort; i++) {
	    addr->commandA = RESETMRPTR;
	    addr->modeA = (u_short) i;
	    addr->commandA = RESETMRPTR;
	    j = (addr->modeA & 0xFF);
	    if (j != i)
		printf("\nLow bus failure: wrote %#x, read %#x", i, j);
	}

	/*
	 * Check address decoding to see if we can touch all 8
	 * Uart chips. Do this by writing a pattern into each
	 * chip mode register and then reading it back.
	 */
	if (!abort) {
	    printf("[OK]\nTesting basic address decoding...");
	    for (i = bline; i < eline; i++) {
		duart = (uartaddress *) & addr[i];
		duart->commandA = RESETMRPTR;
		duart->modeA = (u_short) (129 + i);
	    }
	    for (i = bline; i < eline; i++) {
		duart = (uartaddress *) & addr[i];
		duart->commandA = RESETMRPTR;
		j = (int) (duart->modeA & 0xFF);
		if (j != 129 + i) {
		    printf("\nAddress test UART %d got %#x expected %#x",
			i,j,129+i);
		    abort = TRUE;
		}
	    }
	}

	/*
	 * Do  internal or external loopback transmit/receive tests
	 */
	if (abort) {
	    abortstate = ABORT_IGNORE;
	    abort = FALSE;
	    buserrhandler = 0;
	    return;
	}
	printf("[OK]\nStarting loopback tests...\n");
	function = "STP";
	handler = *EVEC_LEVEL2;
	*EVEC_LEVEL2 = (long) stplev2;
	stpinit(addr, NDUARTS, loopflag);
	sil(0);
	for (i = bline; (i < 2*eline) && !abort; i++) {
		printf("\nLine #%d:", i);
		duart = (uartaddress *) & addr[i>>1];
		if (loopflag)
			modem_flap(duart,i);
		skip = 0;
		for (j = 0; (j < NBAUDS) && !abort && (skip < 20); j++) {
			printf(" %d ", baudtable[j]);
			stpbaud(duart, i, j);
			climit = 5 * j + TXLIMIT;
			if (climit > BUFSIZ/2)
				climit = BUFSIZ/2;
			for (k = 0; k < climit; k++)
				buf1[k] = i + k + 65;
			k = 0;
			tcount = 0;	
			rcount = 0;	
			stpenable(duart, i);
			while (k < TIMERLIMIT && tcount < climit && !abort)
				k++;
			if (k >= TIMERLIMIT) {
				printf("\nTX timeout after sending %d chars of %d possible\n",
					tcount,climit);
				skip++;
				stpdisable(duart, i);
				continue;
			}
			if (rcount != tcount && k < TIMERLIMIT && !abort) {
				k = 0;
				while (tcount != rcount && k < TIMERLIMIT &&
				    !abort)
					k++;
				stpdisable(duart, i);
				if (k >= TIMERLIMIT) {
					printf("\nRX timeout after receiving %d chars of %d sent\n",
						rcount, tcount);
					skip++;
					continue;
				}
			} else
				stpdisable(duart, i);
			if (abort)
				break;
			for (k = 0; k < climit; k++) {
				if (buf1[k] != buf2[k]) {
					printf("\nData mismatch at location %d, should be %#x, is %#x\n",
					    k, buf1[k]&0xFF, buf2[k]&0xFF);
					skip++;
					break;
				}
			}
		}
	}
	printf("\n\nDone testing card %d in %sternal loopback\n",
			unit, loopflag ? "ex" : "in");
	sil(7);
	abortstate = ABORT_IGNORE;
	abort = FALSE;
	buserrhandler = 0;
	*EVEC_LEVEL2 = (long) handler;
}

/*
 * modem_flap
 * Make sure CTS and RING inputs follow DTR
 */

modem_flap (duart, line)
    uartaddress *duart;
    int line;
{
    ushort dtrbit, ctsbit, ringbit, status;

    dtrbit = (line & 1) ? DTRB : DTRA;
    ctsbit = (line & 1) ? CTSB : CTSA;
    ringbit = (line & 1) ? RINGB : RINGA;

    duart->resetport = dtrbit;
    status = ~duart->portchange;
    if (status & ctsbit)
	printf("\nCTS did not go down with DTR");
    if (status & ringbit)
	printf("\nRING did not go down with DTR");
    duart->setport = dtrbit;
    status = ~duart->portchange;
    if (!(status & ctsbit))
	printf("\nCTS did not go up with DTR");
    if (!(status & ringbit))
	printf("\nRING did not up down with DTR");
}

/*
 * Setup STP double-uarts
 */

stpinit(addr, n, loopflag)
	uartaddress *addr;
	int n, loopflag;
{
	register uartaddress *stp;

	for (stp = addr; stp < &addr[n]; stp++) {
		stp->commandA = RESETMRPTR;
		stp->commandA = RESETTX;
		stp->commandA = RESETRX;
		stp->commandA = RESETERROR;
		stp->commandA = RESETMRPTR;
		stp->modeA = RXRDYINTR|CHARERROR|PRTYOFF|DBITS8;
		stp->modeA = loopflag ? (MODENORMAL|STOP16X16) : (MODELOCALLOOP|STOP16X16);
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
	}
}

stpbaud(addr, line, baud)
	uartaddress *addr;
	int line, baud;
{
	if (line & 1)
		addr->statusB = (RXCLOCK*baud)|(TXCLOCK*baud);
	else
		addr->statusA = (RXCLOCK*baud)|(TXCLOCK*baud);
}

stpdisable(addr, line)
	uartaddress *addr;
	int line;
{
	if (line & 1)
		addr->commandB = TXDISABLE + RXDISABLE;
	else
		addr->commandA = TXDISABLE + RXDISABLE;
}

stpenable(addr, line)
	uartaddress *addr;
	int line;
{
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
 * All STP interrupts come here
 */

stpintr()
{
	register uartaddress *addr = teststp;
	register u_short status;

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

stprxservice(addr)
	uartaddress *addr;
{
	char *cp;
	u_short status;

	while ((status = addr->statusA) & RXREADY) {
		cp = &buf2[rcount++];
		*cp = (char) addr->dataA;
		if (stpverbose) {
			if (status & (RCVBREAK+FRMERROR)) {
				putchar('*');
				rcount--;
			}
			else
				putchar('r');
			puthexnum(*cp & 0xFF);
		}
	}
	while ((status = addr->statusB) & RXREADY) {
		cp = &buf2[rcount++];
		*cp = (char) addr->dataB;
		if (stpverbose) {
			if (status & (RCVBREAK+FRMERROR)) {
				putchar('*');
				rcount--;
			}
			else
				putchar('r');
			puthexnum(*cp & 0xFF);
		}
	}
}

stptxservice(addr)
	uartaddress *addr;
{
	if (tcount < climit) {
		if (addr->statusA & TXREADY) {	/* fifo hungry */
			addr->dataA = (u_short) buf1[tcount]; /* yes */
			if (stpverbose) {
				putchar('t');
				puthexnum(buf1[tcount] & 0xFF);
			}
			tcount++;
		}
		if (addr->statusB & TXREADY) {
			addr->dataB = (u_short) buf1[tcount];
			if (stpverbose) {
				putchar('t');
				puthexnum(buf1[tcount] & 0xFF);
			}
			tcount++;
		}
	} else {
		if (addr->statusA & TXEMPTY)	/* fifo empty? */
			addr->commandA = TXDISABLE;/* yes, disable tx */ 
		if (addr->statusB & TXEMPTY)
			addr->commandB = TXDISABLE;
	}
}
