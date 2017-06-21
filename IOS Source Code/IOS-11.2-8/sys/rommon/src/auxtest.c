/* $Id: auxtest.c,v 3.2.58.1 1996/03/21 23:26:31 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/auxtest.c,v $
 *------------------------------------------------------------------
 * auxtest.c
 *
 * February 1993, Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Auxilliary port test routines.
 *------------------------------------------------------------------
 * $Log: auxtest.c,v $
 * Revision 3.2.58.1  1996/03/21  23:26:31  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:19:39  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:40:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:04:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:36:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "monitor.h"
#include "console.h"
#include "error.h"
#include "signal_4k.h"
#include "confreg.h"

#define  LOOPINIT   30
#define  RECEIVED   1
#define  OWNINT     1     /* use my own vect 5 interrupt routine */
#define  DEFAULTINT 0     /* use default vect 5 interrupt routine */
#define  MAXL       0xff  /* length of characters array */

static char timeout1str[] = "Timeout on receive of first char - make sure loopback adaptor is installed";
char doauxtest ;  /*  0 : perform default vector 5 interrupt (conint) */
                  /* !0 : perform AUX test interrupt         (auxint) */

/*
**  Test MC2681 (DUART) by echoing character through AUX port (channel b).
*/
int
auxecho(void)
{
    char c, *cptr;
    
    testname("AUX port echo");
    printf("\nThe AUX port should echo characters.\n"
	   "Type any character to console port to quit.\n");

    initaux();     /* Initialize the UART */

    while (1) {
	if(auxstat()) {
	    auxout(auxin());
	}
	if(constat()) {
	    conout(conin());
	    break;
	}
    }
    return;
}

/* 
** Polled AUX port loopback test.
*/

int
auxloopback(void)
{
    register unsigned char txchar;
    register unsigned char rxchar = 0;
    int loopctr1, loopctr2;
    char charflag;
    long delaycnt;
    
    delaycnt = timer_calibrate(40);
    
    testname("Polled AUX loopback");

    initaux();                                 /* Initialize the UART */
     
    for (txchar=0x1 ; txchar!=0 ; txchar++) { 
	charflag = 0;                          /* clear flag */
	loopctr1 = 20;                       /* initialize counter */
	while (loopctr1 && (charflag != RECEIVED)) {
	    if (uartaddr->b.state & TXREADY) { /* wait for transmit signal */
		uartaddr->b.data = txchar;     /* send out character */
		loopctr2=40;
		do {
		    if (uartaddr->b.state & RXREADY) {
			rxchar = uartaddr->b.data;
			if (rxchar != txchar) {
			    cterr('f',0,"Received different character at "
				  "transfer %d, expected %x, got %x",
				  txchar, txchar, rxchar);
			    return(1);
			}
			if(uartaddr->b.state & RXREADY) {
			    cterr('f', 0, "reciever ready does not clear on char read");
			    return(1);
			}
			charflag = RECEIVED;
			break;
		    }
		    wastetime(delaycnt);
		} while(--loopctr2);
		if (!loopctr2) {
		    if(rxchar == 0)
			cterr('f',0,timeout1str);
		    else cterr('f',0,"timeout on %d th character receive.", txchar);
		    return(1);
		}
	    }
	    wastetime(delaycnt);
       	    loopctr1--;
	}
	if (!loopctr1) {
	    cterr('f',0,"timeout on %d th character transmit.", txchar);
	    return(1);
	}	    
    }
    prcomplete(testpass, errcount, 0);
    return(0);
}

static char breakon;
static char txcharary[0xff];            /* array which hold the transfer characters */
static char rxcharary[0xff];            /* array which hold the received characters */
static unsigned char txarypt,rxarypt;   /* pointer to element of tx/rx array */
static unsigned char txcount,rxcount;   /* number of characters got tx/rx */
static cleanup();
static setup();

/*
** AUX interrupt test. All the AUX port activities are interrupt driven.
** Interrupt on Rx FIFO full.
** 
** Returns :
**    0    : normal exit
**    1    : error found
*/
int
auxinttest(void)
{
    unsigned int loopctr;
    unsigned char i,c;
    long delaycnt;
    
    delaycnt = timer_calibrate(50);
    testname("Interrupt driven AUX loopback");
  
    for(i=0;i<0xff;i++) txcharary[i]=i+1;/* initialize transfer array */
    for(i=0;i<0xff;i++) rxcharary[i]=0;  /* clear receive array */
    txarypt = rxarypt=0;                 /* array entry */
    loopctr = LOOPINIT;

    initaux();                           /* Initialize the UART */
    setup();                             /* set up registers and variable */

    while ((rxarypt != MAXL)&&loopctr) {
	if (txcount || rxcount) {
#ifdef DEBUG
	    printf("Transfered %d and received %d characters in total.\n",txarypt,rxarypt);
	    printf("TXpointer %d and Rxpointer %d\n",txarypt,rxarypt);
#endif
	    txcount=rxcount=0;
	    loopctr=LOOPINIT;
	}
	if ((txarypt-rxarypt) > 5) break; /* never received any character */
	wastetime(delaycnt);
	loopctr--;
    }

    cleanup();

    if(rxarypt == 0) { /* never received any character */
	cterr('f',0,timeout1str);
	return(1);
    }

    if (loopctr==0) { /* this means we ran out of time */
	cterr('f',0,"get stuck on %d th transfer and %d th receive.\n",txarypt,rxarypt);
	return(1);
    }

    for(i=0;i<0xff;i++) {
	if (txcharary[i]!=rxcharary[i]){
	    cterr('f',0,"received different character at %d th transfer.\n",i);
	    return(1);
	}
    }

    prcomplete(testpass, errcount, 0);
    return(0);
}

/* AUX port interrupt service routine. */
long
auxint(void)
{
    while(uartaddr->a.aux2 & (TXRDYB | RXRDYB | BREAKXA)) {
	if (uartaddr->a.aux2 & BREAKXA){
	    uartaddr->a.command = RESETDBREAK; /* reset the interrupt */
	    hkeepflags |= H_USRINT;       /* set the user interrupt flag */
	    if(breakon) {                      /* ignore the second interrupt */
		breakon = 0;
		return((long)0);
	    }
	    breakon=1;
	    cleanup();
	    if(monjmpptr)
		return((long)&default_break);
        }
	while(uartaddr->b.state & FIFOFULL) {
	    if (rxarypt==0xff) break;
	    rxcharary[rxarypt++] = uartaddr->b.data;
	    ++rxcount;
	}
	while(uartaddr->b.state & TXREADY) {
	    if (txarypt==0xff) 
		uartaddr->b.data = 0;
	    else
		uartaddr->b.data = txcharary[txarypt++];
	    ++txcount;
    	}
    }
    return((long)0);
}

static
setup(void)
{
    /* enables interrupts for transmit ready(b) and receive ready(b). */
    uartaddr->b.command = RESETMRPTR;
    uartaddr->b.mode    = CHARERROR | PRTYOFF | DBITS8 | RXFULLINTR;
    doauxtest           = OWNINT;     /* redirect vect 5 */
    uartaddr->a.aux2    = TXRDYB|RXRDYB;
}

static 
cleanup(void)
{
    uartaddr->a.aux2    = 0;
    uartaddr->b.command = RESETMRPTR;
    uartaddr->b.mode    = CHARERROR | PRTYOFF | DBITS8 ;
    doauxtest           = DEFAULTINT; /* restore vect 5 interrupt routine */
}
