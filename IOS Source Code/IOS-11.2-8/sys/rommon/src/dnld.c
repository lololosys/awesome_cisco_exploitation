/* $Id: dnld.c,v 3.2.58.1 1996/03/21 23:27:05 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/dnld.c,v $
 *------------------------------------------------------------------
 * dnld.c
 *
 * June 1993, Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * The binary serial downloader.
 *------------------------------------------------------------------
 * $Log: dnld.c,v $
 * Revision 3.2.58.1  1996/03/21  23:27:05  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:20:13  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:41:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:37:05  hampton
 * Bump version numbers from 1.x to 2.x.
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
#include "monitor.h"
#include "dnld.h"
#include "signal_4k.h"
#include "fsio.h"
#include "fstat.h"
#include "getdents.h"
#include "monlib.h"
#include "mon_boot.h"

#define MAXCOUNT 1000000

static jmp_buf *ldjmpptr;
static char auxport;
static void dl_putchar(char c);
static int dl_getchar();
unsigned long entrypt;   /* entry point for launch command */
int upload(unsigned char *addr, int size, int pktsiz);  /* forward declaration */


int
dnld(int argc, char *argv[])
{
    int i;
    static char optstr[] = "axv:up:";
    extern int optind;
    extern char *optarg;
    char exec = 1;  /* init */
    int verbose = 0;
    struct pib pib;
    int launchcode = PROM2_BOOTLOAD;  /* default */
    int upld = 0;
    int paksiz = 0;
    long addr, size;

    auxport = 0;  /* init */
    while((i = getopt(argc,argv,optstr)) >= 0) {
	switch(i) {
	case 'a':  /* use the auxport */
	    auxport = 1;
	    initaux();
	    break;
	case 'x':  /* do not execute */
	    exec = 0;
	    break;
	case 'd':  /* launch with debug code */
	    launchcode = PROM2_BOOTDEBUG;
	    break;
	case 'v':  /* verbose level */
	    getnum(optarg,10,&verbose);
	    break;
	case 'u':  /* upload requested */
	    upld = 1;
	    break;
	case 'p':  /* user wishes to define packet size */
	    getnum(optarg,16,&paksiz);
	    break;
	default:
usage:
	    printf("usage: %s [-%s] [args for downloaded pgm]\n"
		   "or:    %s -u <address> <size>\n",
		   argv[0], optstr, argv[0]);
	    puts("-a  load via AUX port\n"
		 "-x  download but do not execute\n"
		 "-d  launch with debugger break\n"
		 "-v:  specify verbose level\n"
		 "-u  upload\n"
		 "-p:  define upload packet size in hex\n");
		return(1);
	}
    }
    if(upld) {
	if((argc - optind) != 2 ||
	   getnum(argv[optind++],16,&addr) == 0 ||
	   getnum(argv[optind],16,&size) == 0) goto usage;
	if(!paksiz) paksiz = 256;
	if(upload((unsigned char *)addr, size, paksiz) < 0) return(1);
    } else {
	if(download(argv[0],verbose) < 0) return(1);
	argc = shift(optind, argv);  /* shift out progname and options */
	pib.mifp = mifp;
	pib.argc = argc;
	pib.argv = argv;
#if defined(SIERRA) || defined(RSP) || defined(RSP2)
	brk_clear();  /* clear the breakpoint just in case */
#endif
	if(exec) return(launch( launchcode, (struct plb *)0, &pib, entrypt));
    }
    return(0);
}

static
cleanup()
{
    dl_putchar(DNLD_HNDSHK_ABORT);  /* tell sender we are aborting */
    return((int)SIG_DFL);
}

int
download(char *progname, int verbose)
{
    unsigned short datacount, chksum;
    unsigned char c, *addr, *startaddr, nodata;
    unsigned temp;
    int i;
    int retrycount = 0;
    long packnum = 0;
    jmp_buf dnldjmp;

    signal(SIGINT,(SIG_PF)cleanup);  /* in case user interrupts */
    signal(SIGBUS,(SIG_PF)cleanup);  /* in case of bad download address */
    for(i=0; i<0x1000; i++); /* pause a moment for host */
    if(!setjmp(dnldjmp)) {
	ldjmpptr = &dnldjmp;
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
	    temp |= dl_getchar() & 0xff;
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
		    return(-1);
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
	return(0);
    } else cleanup();
    return(-1);
}

static int
dl_getchar(void)
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
     longjmp(ldjmpptr,1);
}

static void
dl_putchar(char c)
{
    if(auxport) auxout(c);
    else conout(c);
}


static unsigned short temp_chksum;

void chksum_putchar(int c);  /* forward declaration */

int
upload(unsigned char *addr, int size, int pktsiz)
{
    int i;
    jmp_buf upldjmp;

    if(pktsiz > DNLD_MAXPACKET) {
	printf("Warning: packet size adjusted to maximum of %#x bytes\n",
	       DNLD_MAXPACKET);
	pktsiz = DNLD_MAXPACKET;
    }
    if(!setjmp(upldjmp)) {
	ldjmpptr = &upldjmp;
	/* eat characters until we get an OK */
	while(dl_getchar() != DNLD_HNDSHK_ACK);
	while(size) {
	    temp_chksum = 0;
	    if(size < pktsiz) pktsiz = size;
	    chksum_putchar(((pktsiz | DNLD_32BITFLG) >> 8) & 0xff);
	    chksum_putchar(pktsiz & 0xff);
	    chksum_putchar(((int)addr >> 24) & 0xff);
	    chksum_putchar(((int)addr >> 16) & 0xff);
	    chksum_putchar(((int)addr >> 8) & 0xff);
	    chksum_putchar((int)addr & 0xff);
	    for(i=0; i<pktsiz; i++) chksum_putchar(*(addr + i));
	    dl_putchar((temp_chksum >> 8) & 0xff);
	    dl_putchar(temp_chksum & 0xff);
	    switch(dl_getchar()) {
	    case DNLD_HNDSHK_RESEND: continue;
	    case DNLD_HNDSHK_ACK: break;
	    default:
		return(-1);
	    }
	    addr += pktsiz;
	    size -= pktsiz;
	}
	dl_putchar(0);
	dl_putchar(0);  /* terminate the session */
	return(0);
    }
    return(-1);
}

/*
** Put chars and fold them into chksum.
*/
void
chksum_putchar(int c)
{
    temp_chksum += c;
    dl_putchar(c);
}
