/* $Id: printf.c,v 3.1 1995/11/09 09:09:47 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c4000/printf.c,v $
 *------------------------------------------------------------------
 * printf.c - formatted string routine
 *
 * April 1992, Rob Clevenger
 *
 * Copyright (c) 1992-1993 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: printf.c,v $
 * Revision 3.1  1995/11/09  09:09:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:16:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1993/09/24 16:34:06  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.7  1993/03/30  00:31:04  mdb
 * workaround for gcc 2.x warning unsigned value < 0 is always 0
 *
 * Revision 1.6  1993/03/26  08:39:53  mdb
 * cleanup gcc 2.x warning messages.
 *
 * Revision 1.5  1992/05/14  00:18:01  clev
 * Fix the decoding of shared memory parity error information.
 *
 * Revision 1.4  1992/05/06  22:33:00  clev
 * XX cleanup stuff:
 *
 * ../asm.S - fix clear caches in the restart routine
 *
 * ../prom2.c - handle netbooting a system image with a load point different from
 *              firmware system image
 *
 * decodebe.c & printf.c - fix reporting of bus errors and parity errors
 *
 * defs_xx.h - defined BUSERROR for the XX for conditional compiles
 *
 * fldvr.c & rom_tests.c - provide abbreviated flash test for manufacturing
 *
 * init.c - fix the clobbering of bootinfo stuff
 *
 * Revision 1.3  1992/04/19  03:50:51  clev
 * Get the error messages right for shared and local memory parity errors.
 *
 * Revision 1.2  1992/04/18  00:00:38  clev
 * Added better support for shared memory parity errors and support for
 * EMT_ENABLE_DCACHE (and disable).  Also, added support for break enable
 * during boot and better decoding of bus errors.  Init was enhanced to
 * flash an error number in the run LED should the power-on confidence
 * test fail.  Init was also improved to initialize interrupt vectors up
 * to 0x200.
 *
 * Revision 1.1  1992/04/15  02:11:54  clev
 * Initial checkin for XX
 *
 * Revision 1.1  1992/03/09  08:38:08  hampton
 * Initial conversion from RCS to CVS.
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
** of the valid are tested first and must be represented in in that order
** in the array.
** Example:
**    struct bitdef ctlregbits[] = {
**        1,"main memory parity error\n",           (MSB or bit 3)
**        1,"main memory bus error\n",
**        1,"shared memory parity error\n",
**        1,"shared memory bus error\n",            (LSB or bit 0)
**    }
**    print("control register: %#.4b\n, ctlregbits, *ctlreg);
**
** Remember that arguments to printf are treated as longs - the bits
** you wish to be decoded must be LSBs in the long argument.  If the
** desired valid bits in ctlreg are bits 24-31 in the above example
** then it would be necessary to manipulate the bits into position
** (*ctlreg >> 24).
*/

#include "bitdef.h"
#define ALTFORM 0x01
#define NEGNUM  0x02
static bufns(), pad(), bufchar();
struct prstuff {
  char ljust, padchar, *bufptr;
  long precision, fldwidth;
  int count;
};

sprintf(destbuf, fmtptr, arg)
  char *destbuf, *fmtptr;
  unsigned long arg;
{
    struct prstuff prstuff;

    prstuff.bufptr = destbuf;
    _do_print(&prstuff, fmtptr, &arg);
    *prstuff.bufptr = '\0';  /* terminate the string */
    return(prstuff.count);  /* return number of characters */
}

printf(fmtptr,arg)
     char *fmtptr;
     unsigned long arg;
{
    struct prstuff prstuff;

    prstuff.bufptr = (char *)0;  /* function as normal printf */
    _do_print(&prstuff, fmtptr, &arg);
    return(prstuff.count);  /* return number of characters */
}  

_do_print(prstuffptr, fmtptr, argptr)
  struct prstuff *prstuffptr;
  char *fmtptr;
  unsigned long *argptr;
{
  int i;
  register unsigned long number;
  register char c, base, *bptr;
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
      numstr = "0123456789abcdef";  /* default */
notsatisfied:
      c = *fmtptr++;
      switch(c) {
      case 'd':  /* print a decimal number */
	base = 10;
	goto prnum;
      case 'X':  /* print a hexidecimal number with upper case letters */
        numstr = "0123456789ABCDEF";
      case 'x':  /* hexidecimal with lower case letters (default) */
	base = 16;
	goto prnum;
      case 'b':  /* print a binary number */
	if(!prstuffptr->precision)
	  prstuffptr->precision = 8;  /* default binary precision */
	if(flags & ALTFORM) {  /* there is an array of strings to represent bits */
	  bdptr = (struct bitdef *)(*argptr++);  /* pick up bitdef ptr */
	  i = (prstuffptr->precision - 1);  /* get number of bits to check */
	  prstuffptr->precision = 0;  /* print entire string */
	  for( ; i >= 0; i--, bdptr++)  /* check every bit starting at left (MSB) */
	    if(((*argptr >> i) & 1) == bdptr->actstate)
	      bufns(bdptr->string,prstuffptr);  /* print string */
	  argptr++;
	  break;
	}
	base = 2;
	goto prnum;
      case 'o':  /* print an octal number */
	base = 8;
prnum:
	number = *argptr++;  /* get number to be displayed from stack */
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
	bufchar(*argptr++,prstuffptr);
	break;
      case 's':  /* print out string pointed to by arg */
	bufns((char *)(*argptr++),prstuffptr);
	break;
      case '.':  /* specified precision */
	if(*fmtptr == '-') fmtptr++;  /* no meaning, skip over */
	if(*fmtptr == '*') {
	  prstuffptr->precision = *argptr++;  /* arg specifies precision */
	  fmtptr++;
	} else fmtptr += getnnum(fmtptr, 10, &prstuffptr->precision, 0);
	goto notsatisfied;
      case '*':  /* arg specifies field width */
	prstuffptr->fldwidth = *argptr++;
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
static
bufns(cptr,prp)
     register char *cptr;
     register struct prstuff *prp;
{
  register char c;
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

static
pad(num,prp)
     struct prstuff *prp;
    int num;
{
    if(num <= 0) return;
    while(num-- > 0) bufchar(prp->padchar,prp);
}

static
bufchar(c,prp)
     int c;
     register struct prstuff *prp;
{
  if(prp->bufptr) *prp->bufptr++ = c;
  else putchar(c);
  prp->count++;
}

/*
** Convert the ascii string pointed to by cptr to binary according to base.
** Result is placed in *longret.
** Return value is the number of characters processed.
** Maxchars defines the maximum number of characters to process.  If
** maxchars == 0, process until an invalid character occurs.
*/

getnnum(cptr, base, longret, maxchars)
     register char *cptr;  /* character buffer pointer */
     int base;
     long *longret;  /* for the result */
     int maxchars;
{
  register char cval;
  register unsigned value = 0; /* init */
  register count = 0; /* init */

  while(1) {
    cval = atoh(*cptr);
    if(cval < 0 || cval >= base) break;  /* invalid character encountered */
    value = (value * base) + cval;
    cptr++;
    count++;
    if(maxchars && count == maxchars) break;
  }
  *longret = value;  /* place result */
  return(count);
}


/*
** Return the value for the ascii hex character or -1 if invalid.
*/
atoh(c)
     register char c;
{
  if(c >= '0' && c <= '9') return(c - '0');
  if(c >= 'A' && c <= 'F') return(c - ('A' - 10));
  if(c >= 'a' && c <= 'f') return(c - ('a' - 10));
  return(-1);
}


