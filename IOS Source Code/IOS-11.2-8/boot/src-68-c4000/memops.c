/* $Id: memops.c,v 3.1 1995/11/09 09:09:41 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c4000/memops.c,v $
 *------------------------------------------------------------------
 * memops.c - memory manipulation utility
 *
 * April 1992,Rob Clevenger
 *
 * Copyright (c) 1992-1993 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: memops.c,v $
 * Revision 3.1  1995/11/09  09:09:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:16:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1993/09/24 16:33:59  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.2  1993/03/26  23:03:00  mdb
 * cleanup gcc 2.x warning messages and fix bug found with warning
 * 'unsigned value < 0 is always 0'.
 *
 * Revision 1.1  1992/04/15  02:11:47  clev
 * Initial checkin for XX
 *
 * Revision 1.1  1992/03/09  08:38:08  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
** Memory operations.
** These utilities will access memory as words (16 bits) (default), bytes,
** or longwords (32 bits) with the exception of memtest which tests in
** longwords for efficiency.
*/

#include "../ctype.h"

static char optstr[] = "lbw";
static long length, addr;
static char memopsiz;

memops(cptr)
char *cptr;
{
    char c = *cptr++;  /* pick up memop command */

    while(isspace(*cptr)) cptr++; /* bump to next arg */
    switch(c) {
    case 'D':
	dis_mem(cptr);
	break;
    case 'A':
	alt_mem(cptr);
	break;
    default:
	printf("illegal memop command\n");
	break;
    }
}

getopsize(cptr)
     char *cptr;
{
    char c;
    int count = 1;

    memopsiz = 2;  /* default is word size operations */
    if(*cptr++ != '-') return(0);
    while(*cptr && !isspace(*cptr)) {
	switch(*cptr) {
	case 'L':
	    memopsiz = 4;
	    break;
	case 'B':
	    memopsiz = 1;
	    break;
	case 'W':
	    memopsiz = 2;
	    break;
	default:
	    return(-1);
	}
	cptr++;
	count++;
    }
    while(isspace(*cptr++)) count++;
    return(count);
}

static
_getnumber(cptr, base, longret)
char *cptr;
long base;
long *longret;
{
    int count = 0;

    if(*cptr == 0) return(0);  /* don't clobber longret if EOL */
    count = getnnum(cptr, base, longret, 0);
    cptr += count;
    while(isspace(*cptr++)) count++;
    return(count);
}

dis_mem(cptr)
     register char *cptr;
{
    int count;

    if((count = getopsize(cptr)) < 0) {
	printf("usage: dismem [-%s] addr length\n",optstr);
	return(1);
    }
    cptr += count;
    cptr += _getnumber(cptr,16,&addr);
    _getnumber(cptr,16,&length);
    dismem(addr,length,addr,memopsiz);
    return(0);
}

dismem(addr,length,disaddr,fldsize)
     register unsigned char *addr;
     long length;
     unsigned disaddr;
     long fldsize;
{
  register value = 0;
  register unsigned char i, j, c, linepos, asciistart = 0;
  register unsigned char *end = (addr + length);  /* the end boundary */
  register unsigned char *linend, *linestart;

  while(addr < end) {
    linepos = printf("%0#10x  ",disaddr);  /* display the line address */
    linestart = addr;  /* save for ASCII representation */
    linend = (addr + 16);
    for(i=0; i<2; i++) {  /* display twice, as hex and as ascii */
      addr = linestart;
      while(addr < linend) {
	switch(fldsize) {
	case 1:
	  value = *addr++;
	  break;
	case 2:
	  value = *(unsigned short *)addr;
	  addr += 2;
	  break;
	case 4:
	  value = *(unsigned *)addr;
	  addr += 4;
	  break;
	}
	if(i == 0) linepos += printf("%0*x ",fldsize * 2,value);  /* hex */
	else {  /* ascii representation */
#ifdef NATIVE
	  for(j=0; j<(fldsize * 8); j += 8) {
#else
	  for(j=(fldsize * 8); j;) {
	    j -= 8;
#endif
	    c = (value >> j);
	    putchar((c >= ' ') && (c < 0x7f) ? c : '.');
	  }
	}
	if(addr >= end) break;
      }
      if(!asciistart) asciistart = linepos;  /* record start 1st time */
      else while(linepos++ < asciistart) putchar(' ');  /* pad w/spaces */
    }
    putchar('\n');
    disaddr += 16;
  }
}

alt_mem(cptr)
     register char *cptr;
{
    union location {
	unsigned char byte;
	unsigned short word;
	unsigned lword;
    };
    register union location *laddr;
    char dummy;
    unsigned val;
    char inbuf[16];

    if((int)(val = getopsize(cptr)) < 0) {
	printf("usage: altmem [-%s] addr length\n",optstr);
	return(1);
    }
    cptr += val;
    _getnumber(cptr,16,&addr);
    
    laddr = (union location *)addr;
    while(1) {
	printf("%0#10x = ",laddr);
	switch(memopsiz) {
	case 1:
	    printf("%02x",laddr->byte);
	    break;
	case 2:
	    printf("%04x",laddr->word);
	    break;
	case 4:
	    printf("%08x",laddr->lword);
	    break;
	}
	puts(" > ");
	cptr = inbuf;
	gets(cptr);

	switch(*cptr) {
	case 'X':
	case 'Q': return(0);  /* quit */
	case ',':
	case 'P': /* prev location */
	    laddr = (union location *)((unsigned)laddr - memopsiz);
	    continue;
	case 'N':
	case 0: break; /* next location */
	default:
	    if(_getnumber(cptr,16,&val) == 0) {
		printf("bad value \"%s\"\n",cptr);
		continue; /* same location again */
	    } else {
		switch(memopsiz) {
		case 1:
		    laddr->byte = val;
		    break;
		case 2:
		    laddr->word = val;
		    break;
		case 4:
		    laddr->lword = val;
		    break;
		}
	    }
	    break; /* next location */
	}
	laddr = (union location *)((unsigned)laddr + memopsiz);
	/* bump address */
    }
}
