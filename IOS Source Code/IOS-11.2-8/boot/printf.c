/* $Id: printf.c,v 3.2 1995/11/17 07:41:34 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/printf.c,v $
 *------------------------------------------------------------------
 * printf.c -- formatted string printer
 *
 * September 1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1993 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: printf.c,v $
 * Revision 3.2  1995/11/17  07:41:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:12:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "stdio.h"

/*
 * printf
 * Print a formatted string
 */

printf(fmt, args)			/*VARARGS2*/
    char *fmt;
    int args;
{
    return(_doprnt (NULL, fmt, &args));
}
/*
 * sprintf
 * String version of printf
 */

sprintf(cp, fmt, args)			/*VARARGS2*/
    char *cp, *fmt;
    int args;
{
    int count;

    count = _doprnt (cp, fmt, &args);
    cp[count] = '\0';
    return(count);
}
/*
 * _doprnt - master printf routine.
 * The conversions %o, %x, %d, %u, %s, %c, and %% are implemented.
 * The -, 0, *, and # options are supported for numbers only.
 * The <width> option is supported for numbers, strings, and characters.
 */

_doprnt (cp, fmt, argp)			/*VARARGS2*/
    char *cp;
    register char *fmt;
    register int *argp;
{
    register char c, *q;
    register short radix, d, length;
    register int charcount;
    register unsigned long n;
    char digs[15];
    short hash, rjust, width;
    char fill;

    charcount = 0;			/* nothing printed yet */
    while (c = *fmt++)			/* work until end of string */
	if (c != '%') {
	    doprintc(&cp, c);	 	/* output normal characters */
	    charcount++;
	} else {
	    width = -1;			/* default width is unspecified */
	    length = 0;			/* no length yet */
	    fill = ' ';			/* default fill is space */
	    rjust = 0;			/* default is left justified */
	    hash = 0;			/* defaul is no leading 0 or 0x */
	    if (*fmt == '-') {
		rjust++;		/* want right justified */
		fmt++;
	    }
	    if (*fmt == '0') {
		fill = '0';		/* set fill char to '0' */
		fmt++;
	    }
	    if (*fmt == '#') { 		/* look to args for width */
		hash++;			/* want leading 0 or 0x */
		fmt++;
	    }

	    if (*fmt == '*') { 		/* look to args for width */
		width = (int) *argp++;
		fmt++;
	    }
	    else			/* width possibly specified */
		while ((*fmt >= '0') && (*fmt <= '9')) {
		    if (width == -1)
			width = 0;
		    width = width*10 + (*fmt++ - '0');
		}

	    switch (*fmt++) {		/* get char after the %, switch */

		case 'o':		 /* octal */
		case 'O': 
		    radix = 8;
		    n = (int) *argp++;
		    if (hash) {
			doprintc(&cp, '0');
			length++;
			charcount++;
		    }
		    goto compute;

		case 'x':		 /* hexadecimal */
		case 'X': 
		    radix = 16;
		    n = (int) *argp++;
		    if (hash) {
			doprintc(&cp, '0');
			doprintc(&cp, 'x');
			length += 2;
			charcount += 2;
		    }
		    goto compute;

		case 'u':		/* unsigned decimal */
		case 'U':
		    radix = 10;
		    n = (int) *argp++;
		    goto compute;

		case 'd': 		 /* signed decimal */
		case 'D': 
		    radix = 10;
		    if ((int)*argp < 0) {
			doprintc (&cp, '-');
			length++;
			charcount++;
			n = -(int) *argp++;
		    }
		    else
			n = (int) *argp++;
	    compute: 
		    if (n == 0) {
			q = &digs[0];
			*q++ = '0';
			length++;
		    }
		    else
			for (q = &digs[0]; n != 0; n = n / radix) {
			    d = n % radix;  length++;
			    *q++ = d + (d < 10 ? '0' : '7');
			}
		    q--;		/* back up the pointer a bit */
		    if ((rjust || fill == '0') && width != -1 && (length < width))
			for (d = 0; d < (width - length); d++) {
			    doprintc(&cp, fill);	/* leading fill */
			    charcount++;
			}
		    while (q >= &digs[0]) {
			doprintc (&cp, *q--);
			charcount++;
		    }
		    if (!rjust && width != -1 && (length < width))
			for (d = 0; d < (width-length); d++) {
			    doprintc(&cp ,' '); /* trailing fill - spaces*/
			    charcount++;
			}
		    break;

		case 'c':			 /* character */
		    if (rjust && width != -1)
			for (d = 0; d < width-1; d++) {
			    doprintc(&cp ,' '); /* leading fill - spaces */
			    charcount++;
			}
		    doprintc (&cp, *argp++);
		    charcount++;
		    if (!rjust && width != -1)
			for (d = 0; d < width-1; d++) {
			    doprintc(&cp ,' '); /* trailing fill - spaces*/
			    charcount++;
			}
		    break;

		case 's': 			 /* string */ 
		    q = *(char **) argp;	/* point to string */
		    if (!q || !*q) {
			argp++;
			break;
		    }
		    while (*q++) ++length;	/* figure its length */
		    if (rjust && width != -1 && (length < width))
			for (d = 0; d < (width - length); d++) {
			    doprintc(&cp, ' '); /* leading fill - spaces */
			    charcount++;
			}
		    q = *(char **) argp++;	/* get pointer again */
		    while (*q != 0) {
			if (width != -1 && (int)(q - *argp) >= width)
			    break;
			doprintc(&cp, *q++);
			charcount++;
		    }
		    if (!rjust && width != -1 && (length < width))
			for (d = 0; d < (width-length); d++) {
			    doprintc(&cp, ' ');/* trailing fill - spaces */
			    charcount++;
			}
		    break;

		case '%': 			 /* percent sign */ 
		    doprintc(&cp, '%');
		    charcount++;
		    break;
	    }
	}
    return(charcount);
}
/*
 * doprintc
 * Handle a single character for _doprnt
 */

doprintc(cp, ch)
    char **cp;
    register char ch;
{
    if (*cp) {				/* if writing to a string */
	*(*cp)++ = ch;			/* don't do any formatting */
	return;
    }
    putchar(ch);
}
