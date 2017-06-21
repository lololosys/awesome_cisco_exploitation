/* $Id: printf.c,v 3.2.62.1 1996/03/18 21:46:23 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/printf.c,v $
 *------------------------------------------------------------------
 * printf.c
 *
 * November 1992, Rob Clevenger
 *
 * Copyright (c) 1994,1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Subset of the C library routines printf and sprintf for the
 * standalone Monitor environment.
 *------------------------------------------------------------------
 * $Log: printf.c,v $
 * Revision 3.2.62.1  1996/03/18  21:46:23  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:41:44  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  17:04:24  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:42:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:38:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
** Subset of the C library routines printf and sprintf for the
** standalone environment.  Everything is supported as documented in
** the Un*x manuals except for the float operations (f, e, E, g, or
** G).  In addition, the b option was added for binary representation.
** When b is used with the # option, a pointer to an array of bitdef
** structures (one string for each bit position in the binary number)
** must be provided as an argument on the stack.  If a bit in the binary
** number matches the active state, the corresponding array string is
** printed, else nothing is printed.  A precision option defines how many
** bits in the binary number are valid starting at the LSB (rightmost).
** Default binary precision is 8.  The Most Significant (leftmost) bits
** are tested first and must be represented in in that order in the array.
** Example:
**    struct bitdef ctlregbits[] = {
**        1,"main memory parity error\n",           (MSB or bit 3)
**        1,"main memory bus error\n",
**        1,"shared memory parity error\n",
**        1,"shared memory bus error\n",            (LSB or bit 0)
**    }
**    printf("control register: %#.4b\n, ctlregbits, *ctlreg);
**
** Remember that arguments to printf are treated as longs - the bits
** you wish to be decoded must be LSBs in the long argument.  If the
** desired valid bits in ctlreg are bits 24-31 in the above example
** then it would be necessary to manipulate the bits into position
** (*ctlreg >> 24).
*/

#include "bitdef.h"
#include <stdarg.h>
#include "proto.h"

#define ALTFORM 0x01
#define NEGNUM  0x02

char caphexnums[] = "0123456789ABCDEF";
char hexnums[] = "0123456789abcdef";

struct prstuff {
    char ljust, padchar, *bufptr;
    long precision, fldwidth;
    int count;
};

/* forward declarations */
static void pad(int num, struct prstuff *prp);
static void bufchar(int c, struct prstuff *prp);
static int bufns(char *cptr, struct prstuff *prp);


int
sprintf(char *destbuf, char *fmtptr, ...)
{
    struct prstuff prstuff;
    va_list ap;

    va_start(ap, fmtptr);
    prstuff.bufptr = destbuf;
    _do_print(&prstuff, fmtptr, ap);
    va_end(ap);
    *prstuff.bufptr = '\0';              /* terminate the string */
    return(prstuff.count);               /* return number of characters */
}

int
printf(char *fmtptr, ...)
{
    struct prstuff prstuff;
    va_list ap;

    va_start(ap, fmtptr);
    prstuff.bufptr = (char *)0;  /* function as normal printf */
    _do_print(&prstuff, fmtptr, ap);
    va_end(ap);
    return(prstuff.count);  /* return number of characters */
}  

_do_print(struct prstuff *prstuffptr, char *fmtptr, va_list argptr)
{
    int i;
    unsigned long number, bd;
    char c, base, *bptr;
    struct bitdef *bdptr;
    char flags, longarg, *numstr;
    char tmpbuf[64];
    
    
    prstuffptr->count = 0;  /* init */
    while(c = *fmtptr++) {
	if(c != '%') bufchar(c,prstuffptr);  /* print all ordinary characters */
	else {  /* beginning of special sequence */
	    flags = 0;  /* initialize special sequence variables */
	    prstuffptr->precision = 0;  /* init */
	    prstuffptr->fldwidth = 0;  /* init */
	    longarg = 0;  /* init */
	    prstuffptr->ljust = 0;  /* default is right justified */
	    prstuffptr->padchar = ' ';  /* default is to pad with spaces */
	    numstr = hexnums;  /* default */
notsatisfied:
	    c = *fmtptr++;
	    switch(c) {
	    case 'd':  /* print a decimal number */
		base = 10;
		goto prnum;
	    case 'X':  /* print a hexidecimal number with upper case letters */
		numstr = caphexnums;
	    case 'x':  /* hexidecimal with lower case letters (default) */
		base = 16;
		goto prnum;
	    case 'b':  /* print a binary number */
		if(!prstuffptr->precision)
		    prstuffptr->precision = 8;  /* default binary precision */
		if(flags & ALTFORM) {  /* there is an array of strings to represent bits */
		    bdptr = va_arg(argptr, struct bitdef *); /* pick up bitdef ptr */
		    i = (prstuffptr->precision - 1);  /* get number of bits to check */
		    prstuffptr->precision = 0;  /* print entire string */
		    bd = va_arg(argptr, long);
		    for( ; i >= 0; i--, bdptr++)  /* check every bit starting at left (MSB) */
			if(((bd >> i) & 1) == bdptr->actstate)
			    bufns(bdptr->string,prstuffptr);  /* print string */
		    break;
		}
		base = 2;
		goto prnum;
	    case 'o':  /* print an octal number */
		base = 8;
prnum:
		number = va_arg(argptr, unsigned);  /* get number to be displayed from stack */
		if(base == 10 && (long)number < 0) {  /* decimal and negative? */
		    number = -(long)number;
		    flags |= NEGNUM;
		}
		/*
		** Construct and stack the number in a temporary buffer
		** for later use.  This is done in order to correctly calculate
		** the fieldwidth in all cases.
		*/
		bptr = tmpbuf;
		do {  /* construct and stack the ascii number (LIFO) */
		    *bptr++ = numstr[number % base];
		    number /= base;
		} while(number);
		while(bptr - tmpbuf < prstuffptr->precision)
		    *bptr++ = '0';  /* pad with 0's to the desired precision */
		/*
		** Now calculate the number of pads needed for fieldwidth.
		*/
		number = (prstuffptr->fldwidth - (bptr - tmpbuf));  /* number of pads */
		if(flags & NEGNUM) number--;  /* allow space for neg sign */
		if(flags & ALTFORM) {
		    if(base == 8) number--;   /* allow space for the leading '0' */
		    else if(base == 16) number -= 2;  /* or "0x" */
		}
		/*
		** Finally print out stacked number w/fieldwidth padding, etc.
		*/
		if(!prstuffptr->ljust && prstuffptr->padchar == ' ') { /* pad w/spaces */
		    pad(number,prstuffptr);  /* right justify this field (pad on left) */
		    number = 0;  /* all done padding */
		}
		if(flags & NEGNUM) bufchar('-',prstuffptr);  /* print negative sign */
		if(flags & ALTFORM) {
		    if(base == 16 || base == 8)	bufchar('0',prstuffptr); /* print '0' */
		    if(base == 16) bufchar(c,prstuffptr);  /* print 'x' or 'X' */
		}
		if(!prstuffptr->ljust || prstuffptr->padchar == '0')
		    /* right justify this field (pad on left, zeroes always on left) */
		    pad(number,prstuffptr);
		do {  /* unstack and buffer (or display) the number */
		    bufchar(*--bptr,prstuffptr);
		} while(bptr > tmpbuf);
		if(prstuffptr->ljust && prstuffptr->padchar != '0')
		    pad(number,prstuffptr);  /* left justify this field (pad on right) */
		break;
	    case 'c':
		bufchar( va_arg(argptr, int), prstuffptr);
		break;
	    case 's':  /* print out string pointed to by arg */
		bufns( va_arg(argptr, char *),prstuffptr);
		break;
	    case '.':  /* specified precision */
		if(*fmtptr == '-') fmtptr++;  /* no meaning, skip over */
		if(*fmtptr == '*') {
		    prstuffptr->precision = va_arg(argptr, int);  /* arg specifies precision */
		    fmtptr++;
		} else fmtptr += getnnum(fmtptr, 10, &prstuffptr->precision, 0);
		goto notsatisfied;
	    case '*':  /* arg specifies field width */
		prstuffptr->fldwidth = va_arg(argptr, int);
		goto notsatisfied;
	    case '#':  /* alternate format specified */
		flags |= ALTFORM;
		goto notsatisfied;
	    case '-':  /* left justify the field */
		prstuffptr->ljust = 1;
		goto notsatisfied;
	    case 'l':  /* argument is a longword */
		longarg = 1;
		goto notsatisfied;
	    case '%':  /* print out a '%' */
		bufchar(c,prstuffptr);
		break;
	    default:  /* fldwidth must be specified */
		fmtptr--;  /* compensate for the increment at the top */
		if(*fmtptr == '0') prstuffptr->padchar = '0';  /* pad with zeroes */
		fmtptr += getnnum(fmtptr, 10, &prstuffptr->fldwidth, 0);
		goto notsatisfied;
	    }
	}
    }
}

/*
** Buffer up the null terminated string.
** Fieldwidth specifies the minimum number of characters to display and
** if precision > 0 specifies the maximum number of characters to display.
** The pad character is taken from the padchar variable.
*/
static int
bufns(char *cptr, struct prstuff *prp)
{
    char c;
    int numpad;
    
    numpad = (prp->fldwidth - strlen(cptr));
    if(!prp->ljust) pad(numpad,prp);  /* right justify field (pad on left) */
    while((c = *cptr++) != '\0') {
	bufchar(c,prp);
	if(--prp->precision == 0) break;
    }
    if(prp->ljust)
	pad(numpad,prp);  /* left justify this field (pad on right) */
}

static void
pad(int num, struct prstuff *prp)
{
    if(num <= 0) return;
    while(num-- > 0) bufchar(prp->padchar,prp);
}

static void
bufchar(int c, struct prstuff *prp)
{
    if(prp->bufptr) *prp->bufptr++ = c;
    else putchar(c);
    prp->count++;
}
