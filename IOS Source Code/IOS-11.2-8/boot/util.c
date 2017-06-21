/* $Id: util.c,v 3.2 1995/11/17 07:42:04 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/util.c,v $
 *------------------------------------------------------------------
 * Hardware independent utilities
 *
 * December 1989, Chris Shaker
 *
 * Copyright (c) 1989-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: util.c,v $
 * Revision 3.2  1995/11/17  07:42:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:05:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:13:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../defs.h"
#include "../stdio.h"
#include "../ctype.h"
#include "../reason.h"
#include "../extern.h"
#include "../test.h"
#include "../m68vectors.h"

extern char evec[], evece[];
char		cvu();
extern char *edata;
#ifdef PAN
extern long refreshe_brut;
extern cookie_t *cookie;
#endif PAN

/*
 * Restore the MC68000 vectors in RAM:
 */
void restore_vectors()
{
    extern int unexp_int();
    long *vptr;
    int i;
    ulong *dst_ptr = (ulong *)ADRSPC_RAM;
    ulong *src_ptr = (ulong *)evec;

    /* copy the compiled in exception table down to the exception */
    /* area in memory */

    /* 
     * evece - evec is count in byte of the number of bytes
     * in the vector table.  Since we're doing long copies
     * we need only do 1/4th the number of the copies or >> 2
     */

    for (i=0; i < (evece-evec) >> 2 ; i++) {
# if defined(PAN)
	if (i == NMI_VECTOR && 
	    (cookie->processor == PROCESSOR_BRUT || 
	     cookie->processor == PROCESSOR_CANCUN) ) {

	    /* 
	     * kludge to install a unique timer (NMI) interrupt 
	     * service routine for the brut/cancun. brut/cancun 
	     * timers are accessed differently and the registers 
	     * are at different addressses. to prevent run time 
	     * checks in timer isr's, a seperate wrapper and isr 
	     * is installed for the brut/cancun box 
	     */

	    dst_ptr[i] = (ulong)(&refreshe_brut);
	} else
	    
#endif	/* defined(PAN) */
	    dst_ptr[i] = src_ptr[i];
    }

#ifdef XX
    /*
    ** Initialize up to &edata (0x200) with valid vectors in case
    ** NIMs cause unexpected vectored interrupt.
    ** Some of these will be overwritten by system code when booted.
    */

    for(vptr = (long *)(evece - evec); vptr < (long *)&edata;)
	*vptr++ = (long)unexp_int;
#endif XX
#ifndef MC68000
    setvbr(ADRSPC_RAM);
#endif
}

#ifdef NOT_DEF
/*
 * Wait for a real time clock tick (4 ms ticks):
 */
void wait_for_tick()
{
    u_long first_clock;

    first_clock = clock;
    while (clock == first_clock)
	;			/* Spin wait */
}

/*
 * Spin wait at least n milliseconds:
 */
delay(n)
    int	n;
{
    int ms;

    wait_for_tick();		/* Synchronize with clock interrupt */
    while (n > 0) {
	wait_for_tick();	/* Wait another 4 ms */
	n = n - 4;
    }
}
#endif

/*
 * copy at most 'max - 1' characters, always null terminate
 */
void strncpy (dst, src, max)
    char *dst;
    char *src;
    ulong max;
{
    while ((max-- > 1) && (*src)) {
	*dst = *src;
	dst++;
	src++;
    }
    *dst = '\0';
}

/*
 * Copy a string
 */
void strcpy(dest, source)
    char *dest, *source;
{
    bcopy(source, dest, strlen(source)+1);
}


/*
 * strcmp
 * case sensitive string match
 * returns an integer <,=,> 0 depending on whether s1 is <,=,> s2
 */

int strcmp (const char *s1, const char *s2)
{
    if ((!s1 && s2) || (s1 && !s2)) /* no match if only one ptr is NULL */
	return (int) (s1 - s2);	/* if one of these is NULL it will be the
				   lesser of the two values and therefore
				   we'll get the proper sign in the int */

    if (s1 == s2)		/* match if both ptrs the same (e.g. NULL) */
	return 0;

    while (*s1 == *s2++)
	if (*s1++ == '\0')
	     return 0;

    return *(unsigned char *)s1 - *(unsigned char *)--s2;
}

void strncat (char *s1, char const *s2, ulong max)
{
     while (*s1)
         s1++;
     while ((max-- > 1) && (*s2)) {
         *s1 = *s2;
         s1++;
         s2++;
     }
     *s1 = 0;
}


/*
 * Upper case a string
 */
void strtoupper(source)
    char *source;
{
    char *c;

    for (c = source; *c; c++)
	if (islower(*c))
	    *c = toupper(*c);
}

/*
 * term_char
 * Return TRUE if character is a token terminator, FALSE otherwise
 */

boolean term_char(c)
    char c;
{
    if ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r') || (c == 0))
	return(TRUE);
    return(FALSE);
}
/*
 * deblank
 * Remove leading white space
 */

char *deblank (s)
    register char *s;
{
    while (*s == ' ' || *s == '\t')
	++s;
    return(s);
}

#ifdef RP1

/*
 * touch_device
 * Determines if an address exists. Returns boolean.
 */

boolean touch_device(addr)
    short *addr;
{
	volatile short temp;
	char old_abortstate = abortstate;
	void* old_buserrhandler = buserrhandler;

	abortstate = ABORT_INTERNAL;
	buserrhandler = buserrnote;
	buserrcnt = 0;
	temp = stouchr(addr);
	abortstate = old_abortstate;
	buserrhandler = old_buserrhandler;
	return (buserrcnt == 0);
}

#endif 

/*
 * bzero
 * Zero some memory, takes address and byte count
 */

void bzero(address,count)
    register uchar *address;
    register int count;
{
    while (count--)
	*(address++) = 0;
}

/*
 * parse_unsigned
 * Parse an unsigned number.
 * If number starts with '0x', interpret as hexadecimal.
 * If number starts with '0', interpret as octal.
 * Anything else is assigned to be unsigned decimal.
 * Returns -1 on error.
 */

ulong parse_unsigned(buff)
    register char *buff;
{
    register char c;
    register ulong num;
    char *startptr;

    buff = (char *) deblank(buff);
    num = 0;
    c = *buff++;
    if (c == '0') {

	/*
	 * If '0x', we've parsed a hexadecimal number
	 */
	if ((*buff == 'x') || (*buff == 'X')) {
	    buff++;
	    c = cvu(*buff++);
	    startptr = buff;
	    while (((c >= '0') && (c <= '9')) || ((c >= 'A') && (c <= 'F'))) {
		if ((c >= '0') && (c <= '9')) 
		    num = num*16 + (c - '0');
		else
		    num = num*16 + (c - '7');
		c = cvu(*buff++);
	    }
	    if ((buff == startptr) || (term_char(c) == FALSE))
		return(-1);
	    return(num);
	}
	
	/*
	 * Check for an octal number, or just zero.
	 */
	startptr = buff-1;
	while ((c >= '0') && (c <= '7')) {
	    num = num*8 + (c - '0');
	    c = *buff++;
	}
	if (buff == startptr || (term_char(c) == FALSE))
	    return(-1);
	return(num);
    }

    /*
     * No leading zero, looks like a positive decimal number.
     */
    startptr = buff;
    while ((c >= '0') && (c <= '9')) {
	num = num*10 + (c - '0');
	c = *buff++;
    }
    if ((buff == startptr) || (term_char(c) == FALSE))
	return(-1);
    return(num);
}

/*
 * null
 * check for null string
 */

boolean null (s)
    register char *s;
{
    register char c;

    if (!s)
	return(FALSE);
    while ((c = *s++) != '\0')
	if ((c != ' ') && (c != '\t'))
	    return(FALSE);
    return(TRUE);
}

/*
 * cvu, cvl, uppercase, lowercase
 * Case conversion routines
 */

char cvu (c)
     char c;
{
    if ('a' <= c && c <= 'z')
	return (c - 040);
    else
	return (c);
}

/*
 * more
 * Asks the question " --More-- ", and returns TRUE if the user responsed
 * with a SPACE, "Y", or "y".  Returns FALSE otherwise.  Assumes STDIO.
 * Assumes TRUE always if screen length is zero.
 */

boolean more ()
{
    register char c;

    printf("\n --More-- ");
    c = getchar();
    c &= 0177;
    putchar(c);
    switch (c) {
	case 040: 
	case 'Y': 
	case 'y': 
	    return(TRUE);
	default: 
	    return(FALSE);
    }
}


/*
 * quit_display
 * Similar to more(), but does it every 22 lines
 */

boolean quit_display (n, s)
    register int n;
    char *s;
{
    if (n != 0 && (n % (22)) == 0) {
	if (more() == TRUE) {
	    if (s)
		printf(s);		/* wants more, print banner */
	    return(FALSE);		/* not time to quit */
	}
	return(TRUE);			/* wants to quit.  Enough! */
    }
    return (FALSE);			/* not time to ask */
}


void lowercase (str)
    char *str;
{
    char ch;

    while (ch = *str) {
        if ((ch >= 'A') && (ch <= 'Z'))
            ch += 040;
        *str++ = ch;
    }
}
