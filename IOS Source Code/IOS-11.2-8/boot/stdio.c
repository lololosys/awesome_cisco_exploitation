/* $Id: stdio.c,v 3.3 1996/01/19 23:51:23 mdb Exp $
 * $Source: /vws/aam/cherf.111/ios/boot/stdio.c,v $
 *------------------------------------------------------------------
 * stdio.c -- miniature standard I/O package for monitor software
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stdio.c,v $
 * Revision 3.3  1996/01/19  23:51:23  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  07:41:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:12:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "defs.h"
#include "stdio.h"
#include "ctype.h"
#include "reason.h"
#include "extern.h"
char preserve_case;

char *gets(s)
	register char *s;
{
	register char c, *ss = s;
	register short len = 0;

	while ((c = getch()) != '\r') {
		switch (c) {

		case CERASE1:		/* '\b' */
		case CERASE2:		/* delete */
			if (len <= 0) {
				putchar('\007');
				break;
			} else {
				if (*--s < ' ')
					puts("\b \b\b \b");
				else
					puts("\b \b");
				len--;
				*s = '\0';
			}
			break;

		case LKILL:		/* ^U */
			while (len > 0) {
				if (*--s < ' ')
					puts("\b \b\b \b");
				else
					puts("\b \b");
				len--;
				*s = '\0';
			}
			s = ss;
			*s = '\0';
			break;
			
		default:
			if (c == '\t')		/* convert tabs to spaces */
				c = ' ';
			if (len < BUFSIZ) {	/* character acceptable */
				putchar(c);	/* echo in typein case */
				if (preserve_case==FALSE){
				    if (islower(c)) /* force upper case */
					c = toupper(c);
				}
				*s++ = c;	/* add to string */
				len++;
			} else
				putchar('\007');  /* exceeded input buffer */
			break;
		}
	}
	*s = '\0';
	linelength = len;
	putchar('\n');
	return(ss);
}

long getnum(s)
	register char *s;
{
	register char *ss = s;
	register long n = 0;

	DEBLANK(ss);
	while (ss && *ss && isdigit(*ss))
		n = n * 10 + (*ss++ - '0');
	return(s == ss ? -1 : n);
}



long gethexnum(s)
	char *s;
{
	register char *ss = s;
	register long n = 0;
	register long num = 0;

	DEBLANK(s);
	while (ss && *ss && isxdigit(*ss)) {
	    if (isdigit(*ss))
		num = *ss++ - '0';
	    else 
		num = (islower(*ss) ? (*ss++ - 'a' + 10) : (*ss++ - 'A' + 10));
	    n = n*16 + num;
	}
	return(s == ss ? -1 : n);
}



void puts(s)
	register char *s;
{
	while (*s)
		putchar(*s++);
}



char digits[] = "0123456789ABCDEF";

void putnum(n)
	register long n;
{
	register long i, j;

	i = n % 10;
	if (j = n / 10)
		putnum(j);
	putchar(digits[i]);
}

#ifdef	NOT_USED
void puthexnum(n)
	register long n;
{
	register short i;

	for (i = 28; i > 0 && ((n>>i)&0xf) == 0; i -= 4)
		;			/* skip till first non-zero digit */
	for ( ; i >= 0; i -= 4)		/* will at least print out 0 */
		putchar(digits[(n>>i)&0xf]);
}



long gethexbyte(s)
	register char *s;
{
	register long n = -1;

	DEBLANK(s);
	if (s && s[0] && s[1] && isxdigit(s[0]) && isxdigit(s[1])) {
		n = isdigit(s[0]) ? (s[0] - '0') : (s[0] - 'A' + 10);
		n = n*16 + (isdigit(s[1]) ? (s[1] - '0') : (s[1] - 'A' + 10));
	}
	return(n);
}
#endif	/* NOT_USED */
