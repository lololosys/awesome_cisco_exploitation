/* $Id: dnld.c,v 3.1 1995/11/09 09:04:13 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/dnld.c,v $
 *------------------------------------------------------------------
 *
 * Copyright (c) 1993 - 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Author: clev
 *------------------------------------------------------------------
 * $Log: 
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
** The binary serial downloader
**
** A packet looks like:
**     2 bytes datasize (number of data bytes in this packet)
**     N bytes address  (destination address of this data packet)
**     N bytes data     (N is datasize)
**     2 bytes checksum (summation of above)
**
** Special notes:
**
**     Valid data sizes are 1 to 16k.
**
**     If the upper bit in datasize is set the address is 4 bytes
**     (32 bits) else the address is 3 bytes (24 bits).
**
**     If datasize is 0, terminate the load.
**
** A simple handshake takes place that allows packet retries and 
** dl_getchar() is timed.
*/
#include "../types.h"
#include "../ctype.h"
#include "../reason.h"
#include "dnld.h"
#include "console.h"
#define MAXCOUNT 8000000

unsigned long entrypt;

static char auxport;
static void dl_putchar(char c);
static int dl_getchar();
extern void conout(char c), auxout(char c);

void
dnld(char *c)
{
    int verbose = 0;
    char *progname = "download";

    auxport = 0;  /* init */
    while(*c) {
	while (*c && isspace(*c)) c++;    /* skip leading spaces */
	switch(*c++) {
	case 'a':  /* download thru the auxport */
	case 'A':
	    auxport = 1;
	    /* initialize the auxillary port */
	    uartaddr->a.aux2    = 0;	               /* Turn off interrupts (IMR=0) */
	    uartaddr->b.command = RESETMRPTR;
	    uartaddr->b.mode    = CHARERROR | PRTYOFF | DBITS8;
	    uartaddr->b.mode    = MODENORMAL | STOP16X16;
	    uartaddr->b.state   = (RXCLOCK * UARTspeeds[0]) | (TXCLOCK * UARTspeeds[0]);
	    uartaddr->b.command = RESETRX;
	    uartaddr->b.command = RESETTX;
	    uartaddr->b.command = RESETDBREAK|STP_TXENABLE|STP_RXENABLE;
	    uartaddr->a.aux2    = BREAKXA;
	    break;
	case 'v':  /* verbose level */
	case 'V':
	    verbose++;
	    break;
	default:
	    printf("usage: u [av]\n");
	    puts("\
a  load via AUX port\n\
v  verbose\n");
	    return;
	}
    }
    download(progname, verbose);
}

void
download(char *progname, int verbose)
{
    unsigned short datacount, chksum;
    unsigned char c, *addr, *startaddr, nodata;
    unsigned temp;
    int i, retrycount;
    long packnum = 0;

    addr = startaddr = (unsigned char *)0;  /* init for compiler */
    retrycount = 0;  /* init for compiler */
    for(i=0; i<0x1000; i++); /* pause a moment for host */
    dl_putchar(DNLD_HNDSHK_START); /* let host know we are alive */
    while(1) {
	c = dl_getchar();
	datacount = (c << 8) & 0xff00;
	chksum = c;
	c = dl_getchar();
	datacount |= c;
	chksum += c;
	if(verbose > 2) printf("got datacount %#x\n", datacount);
	if(datacount & DNLD_32BITFLG) {
	    datacount &= ~DNLD_32BITFLG;
	    i=24;  /* 4 byte (32 bit) address */
	} else i=16;  /* 3 byte (24 bit) address */
	temp = 0;
	while( i >= 0 ) {  /* get the address */
	    c = dl_getchar();
	    temp |= (c << i);
	    chksum += c;
	    i -= 8;
	}
	if(datacount) {
	    nodata = 0;
	    addr = (unsigned char *)temp;
	    if(verbose > 2) printf("got address %#x\n", addr);
	    if(packnum == 0) startaddr = addr;
	} else {
	    nodata = 1;
	    entrypt = temp;  /* termination record */
	}
	while(datacount) {  /* get the data */
	    c = dl_getchar();
	    *addr++ = c;
	    chksum += c;
	    if(verbose > 2) printf("%#x  ", c);
	    datacount--;
	}
	temp = 0;  /* get the sent checksum */
	temp = (dl_getchar() << 8) & 0xff00;
	temp |= dl_getchar();
	if(verbose > 2) printf("\ngot checksum %#x  ", temp);
	if(temp != chksum) {
	    if(verbose > 0) {
		printf("%s: checksum mismatch, expctd %#x  got %#x\n",
		       progname, chksum, temp);
		printf("%s: packet number %d is bad - requesting resend\n",
		       progname, packnum);
	    }
	    if(++retrycount > DNLD_MAXRETRIES) {
		printf("%s: aborting due to max retries on packet number %d\n",
		       progname, packnum);
		dl_putchar(DNLD_HNDSHK_ABORT);
		return;
	    }
	    dl_putchar(DNLD_HNDSHK_RESEND);
	} else {
	    dl_putchar(DNLD_HNDSHK_ACK);
	    retrycount = 0;
	    packnum++;
	}
	if(nodata) break;  /* terminate session */
    }
    if(entrypt == DNLD_ABORT_ADDR)
	printf("host download abort command recieved\n");
    if(verbose > 0)
	printf("\nrecieved %d packets - addresses %#x to %#x - entry point %#x\n",
	       packnum, startaddr, addr, entrypt);
}

static int
dl_getchar()
{
     int count = MAXCOUNT;

     do {
	 if(auxport) {
	     if(auxstat()) return(auxin());
	 } else {
	     if(constat()) return(conin());
	 }
     } while(count--);
     printf("timed out waiting for data - aborting\n");
     restart(EXC_RESET, 0, 0, 0);
     return(-1);  /* for compiler */
}

static void
dl_putchar(char c)
{
    if(auxport) auxout(c);
    else conout(c);
}

/*
** Raw console output routine.
*/

void
conout(char c)
{
    while ((uartaddr->a.state & TXREADY) == 0)
	;			/* wait for ready */
    uartaddr->a.data = c;
}

/*
** Raw console input routine.
*/
int
conin()
{
    if (uartaddr->a.state & RXREADY)
	return(uartaddr->a.data);
    return(-1);
}

/*
** Console status routine.
** Returns true (1) if there is a character to be had, false (0) otherwise.
*/
int
constat()
{
    if (uartaddr->a.state & RXREADY) return(1);
    return(0);  /* no character */
}

/*
** Raw auxport output routine.
*/

void
auxout(char c)
{
    while((uartaddr->b.state & TXREADY) == 0)
	;			/* wait for ready */
    uartaddr->b.data = c;
}

/*
** Raw auxport input routine.
*/
int
auxin()
{
    if(uartaddr->b.state & RXREADY)
	return(uartaddr->b.data);
    return(-1);
}

/*
** Auxport status routine.
** Returns true (1) if there is a character to be had, false (0) otherwise.
*/
int
auxstat()
{
    if(uartaddr->b.state & RXREADY) return(1);
    return(0);  /* no character */
}
