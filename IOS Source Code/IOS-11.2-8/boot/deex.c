/* $Id: deex.c,v 3.2.58.2 1996/05/10 23:16:50 gstovall Exp $
 * $Source: /release/112/cvs/Xboot/deex.c,v $
 *------------------------------------------------------------------
 * deex.c -- perform the deposit and examine functions
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: deex.c,v $
 * Revision 3.2.58.2  1996/05/10  23:16:50  gstovall
 * CSCdi56792:  Need to remove unused files
 * Branch: California_branch
 * The beast still exists, finish killing off the CS500, and while I am in
 * there, nuke the even crustier CSC1.
 *
 * Revision 3.2.58.1  1996/05/09  13:59:41  rbadri
 * Branch: California_branch
 * Synergy features
 *
 * Revision 3.2.88.1  1996/04/27  06:12:50  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.78.1  1996/04/08  01:34:10  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.64.1  1996/03/22  09:06:44  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.50.1  1996/03/03  21:18:48  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.48.1  1996/03/02  01:06:27  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.32.1  1996/02/29  07:41:46  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.2  1995/11/17  07:41:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.62.1  1996/02/03  06:11:29  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.1  1995/10/03  07:04:29  rlowe
 * Re-port IOS Boot for Synergy ATM line card from 10.3 to 11.0.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1  1995/06/07  19:11:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * Revision 1.2.104.1  1995/05/27  23:19:53  rlowe
 * Platform address map spans 32 bits, specifically valid addresses
 * can have 32nd (sign) bit set.
 * Branch: Synergy_ATMlc_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *	d /s m l v	deposit value v of size s into location l, modifier m
 *	e /s m l	examine location l with size s, modifier m
 *
 *	Sizes include:	B - byte
 *			W, S - word or short
 *			L - long
 *			M - segment map
 *			P - page map
 *			I - instructions
 *			A - ascii string
 *
 *	Modifiers are:	R - examine registers
 *			S - byte swap values
 */

#include "defs.h"
#include "ctype.h"
#include "stdio.h"
#include "reason.h"
#include "extern.h"

#define	R_SS	15

void dinstr();

char *register_names[] = { "D0","D1","D2","D3","D4","D5","D6","D7","A0","A1",
			   "A2","A3","A4","A5","A6","SS","US", NULL,"SR","PC" };
short nregisters = sizeof(register_names) / sizeof(register_names[0]);

char *skipblanks(s)
	char *s;
{
	while (*s && isspace(*s))
		s++;
	return(s);
}

char *skiphex(s)
	char *s;
{
	while (*s && isxdigit(*s))
		s++;
	return(s);
}

typesize(t)
	char t;
{
	switch (t) {
	case 'B': return(1);
	case 'A': return(4);
	case 'L': return(4);
	case 'I': return(instrsize);
	default: return(2);
	}
}

deposit(s, raddr)
	char *s;
	long *raddr;
{
	char size, byteswap;
	long loc, val;

	if (!parse(s, raddr, &size, &loc, &val, &byteswap, TRUE))
		return;
	if (loc != (long)&raddr[R_SS]) {	/* can't modify SS */
		contents(raddr, size, loc, val, byteswap, TRUE);
		printf(" -> %x\n", val); 
	} else
		printf("SS not modifiable\n");
}

examine(s, raddr)
	char *s;
	long *raddr;
{
	char size, cmd, byteswap;
	long loc, val;

	lastswap = FALSE;
	if (!parse(s, raddr, &size, &loc, &val, &byteswap, FALSE))
		return;
	while (TRUE) {
		contents(raddr, size, loc, val, byteswap, FALSE);
		putchar(' ');
		echoing = FALSE;
		cmd = (char) getchar();		
		echoing = TRUE;
		putchar('\n');
		switch (cmd) {
		case '\n':
		case '\r':
			if (!parse(NULL, raddr, &size, &loc, &val, &byteswap, FALSE))
				return;
			break;
		case '\b':
		case '^':
			lastloc -= 2*typesize(lastsize);
			if (!parse(NULL, raddr, &size, &loc, &val, &byteswap, FALSE))
				return;
			break;
		case '\033':
		case 'Q':
			return;
		default:
			printf("Type Q to quit, RETURN for next location, BACKSPACE for previous location\n");
			break;
		}
	}
}

parse(s, raddr, size, loc, val, byteswap, chkval)
	register char *s;
	long *raddr;
	char *size, *byteswap;
	long *loc, *val;
	short chkval;
{
	register int i;

	*size = lastsize;		/* default long */
	*loc = lastloc + typesize(lastsize); /* default last location */
	*byteswap = lastswap;		/* default byte swapping */
	*val = 0;
	if (s && *s == '/') {
		s = skipblanks(++s);
		switch(*s) {
		case 'L':		/* long */
		case 'W':		/* short */
		case 'S':		/* short */
		case 'B':		/* byte */
		case 'I':		/* instruction */
		case 'A':		/* ascii string */
			break;
		default:
			printf("Illegal size type\n");
			return(FALSE);
		}
		*size = *s++;
		s = skipblanks(s);
	}
	if (s && *s != '\0') {
		*loc = gethexnum(s);
#ifdef	SYNALC
		if (*loc != (-1)) {
#else
		if (*loc >= 0) {
#endif
		    s = skiphex(s);
		} else switch (*s) {
		    case 'R':		/* registers */
			s = skipblanks(++s);
			for (i = 0; i < nregisters; i++)
				if (register_names[i] &&
				    strncmp(register_names[i], s,
				    strlen(register_names[i])) == 0)
					break;
			if (i >= nregisters)
				goto bad;
			*loc = (long)&raddr[i];
			s += strlen(register_names[i]);
			s = skipblanks(s);
			break;
		    case 'S':		/* byte swapped */
			s = skipblanks(++s);
			*loc = gethexnum(s);
		        if (*loc < 0)
				goto bad;
			s = skipblanks(skiphex(s));
			*byteswap = TRUE;
			break;
		    default:
bad:			printf("Illegal location\n");
			return(FALSE);
		}
		s = skipblanks(s);
	}
	if (chkval) {
		if (*size == 'I' || *size == 'A') {
			printf("Illegal deposit\n");
			return(FALSE);
		}
		if (s != NULL) {
		    if (*s != '\0')
			*val = gethexnum(s);
		    else {
			printf("Missing value\n");
			return(FALSE);
		    }
		}
	}
	lastsize = *size;
	lastloc = *loc;
	lastswap = *byteswap;
	return(TRUE);
}

contents(raddr, size, loc, val, byteswap, storeval)
	long *raddr;
	char size, byteswap;
	long loc, val;
	short storeval;
{
	long i, fieldwidth;
	unsigned long con;

	fieldwidth = 4;
	con = 0L;
	switch (size) {
	case 'B':				/* byte */
		con = (unsigned long )(*(char *)loc & 0xff);
		if (storeval)
			*(char *)loc = (val & 0xff);
		fieldwidth = 2;
		break;
	case 'S':				/* short */
	case 'W':				/* word */
		con = (unsigned long)(*(short *)loc & 0xffff);
		if (byteswap) {
			con = swaps(con);
			val = swaps(val);
		}
		if (storeval)
			*(short *)loc = (val & 0xffff);
		fieldwidth = 4;
		break;
	case 'L':				/* long */
		con = *(long *)loc;
		if (byteswap) {
			con = swapl(con);
			val = swapl(val);
		}
		if (storeval)
			*(long *)loc = val;
		fieldwidth = 8;
		break;
	case 'I':				/* instruction */
		dinstr(loc);
		return;
	case 'A':				/* ascii string */
		printf("%s\n", loc);
		return;
	}
	i = (loc - (long)raddr) / sizeof(long);
	if (i >= 0 && i < nregisters && register_names[i])
		printf(register_names[i]);
	else
		printf("%x",loc);
	printf(": %-0*x", fieldwidth, con);
}
