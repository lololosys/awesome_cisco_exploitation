/* $Id: termcap.c,v 3.3.28.3 1996/05/13 07:18:18 bjthomas Exp $
 * $Source: /release/112/cvs/Xsys/termcap/termcap.c,v $
 *------------------------------------------------------------------
 * TERMCAP - routines for dealing with terminal capability database.
 * 
 * June 1991, Robert Snyder
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: termcap.c,v $
 * Revision 3.3.28.3  1996/05/13  07:18:18  bjthomas
 * CSCdi55624:  show keymap <specific> fails to find aliased names
 * Branch: California_branch
 *
 * Revision 3.3.28.2  1996/03/28  21:02:20  irfan
 * CSCdi12246:  Customer wants visible cursor when in insert mode (7171
 * compatib.)
 * Branch: California_branch
 * Enable code and add default termcap strings for insert mode cursor
 *
 * Revision 3.3.28.1  1996/03/18  22:18:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.1  1996/03/07  10:53:36  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3  1996/01/17  22:16:26  vandys
 * CSCdi44497:  TN3270 forces 80 columns, even for TN3270 MOD5 terminals
 * Inhibit forced mode switch.  Fix losing non-ANSI wrapped lines
 * while I'm in here.
 *
 * Revision 3.2  1995/11/17  18:50:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:36:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:07:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Copyright (c) 1981 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*#define	TERMCAP_BUFSIZ		1024*/
#define MAXHOP		32	/* max number of tc= indirections */
#define	E_TERMCAP	"/etc/termcap"

#include "master.h"
#include <ctype.h>
#include "ttysrv.h"
#include "interface_private.h"
#include "packet.h"
#include "connect.h"
#include "termcapglo.h"
#include "termcap_public.h"

tty_3270def *termcap_list;      /* pointer to termcap defs that curses uses */

/*
 * termcap - routines for dealing with the terminal capability data base
 *
 * BUG:		Should use a "last" pointer in tbuf, so that searching
 *		for capabilities alphabetically would not be a n**2/2
 *		process when large numbers of capabilities are given.
 * Note:	If we add a last pointer now we will screw up the
 *		tc capability. We really should compile termcap.
 *
 * Essentially all the work here is scanning and decoding escapes
 * in string capabilities.  We don't use stdio because the editor
 * doesn't, and because living w/o it is not hard.
 */


/*static	char *tbuf;
static	int hopcount;	/ * detect infinite loops in termcap, init 0 */
static char	*tskip(char *);
char	*tgetstr(char *, char **, conntype *);
static char	*tdecode(char *, char **);

/*
 * Get an entry for terminal name in buffer bp,
 * from the termcap file.  Parse is very rudimentary;
 * we just notice escaped newlines.
 */
#ifdef FORDEBUG
char default_xterm[] =
"unknown|vs|xterm|vs100|xterm terminal emulator (X window system):"
"li#48:co#80:  :li#48:co#80:  :\n:cr=^M:do=^J:nl=^J:bl=^G:le=^H:"
"ho=\\E[H:\n:co#80:li#48:cl=\\E[H\\E[2J:bs:am:cx:ms:cm=\\E[%i%d;%dH:"
"nd=\\E[C:up=\\E[A:\n:ce=\\E[K:cd=\\E[J:so=\\E[7m:se=\\E[m:us=\\E[4m:"
"ue=\\E[m:\n:md=\\E[1m:mr=\\E[7m:me=\\E[m:\n:ku=\\EOA:kd=\\EOB:"
"kr=\\EOC:kl=\\EOD:kb=^H:\n:k1=\\EOP:k2=\\EOQ:k3=\\EOR:k4=\\EOS:"
"ta=^I:pt:sf=\n:sr=\\EM:\n:al=\\E[L:dl=\\E[M:ic=\\E[@:dc=\\E[P:\n:"
"MT:ks=\\E[?1h\\E=:ke=\\E[?1l\\E>:\n:"
"is=\\E[r\\E[m\\E[2J\\E[H\\E[?7h\\E[?1;3;4;6l:\n:"
"rs=\\E[r\\E<\\E[m\\E[2J\\E[H\\E[?7h\\E[?1;3;4;6l:xn:\n:AL=\\E[%dL:"
"DL=\\E[%dM:IC=\\E[%d@:DC=\\E[%dP:\n:ti=\\E7\\E[?47h:"
"te=\\E[2J\\E[?47l\\E8:\n:hs:ts=\\E[?E\\E[?%i%dT:fs=\\E[?F:"
"es:ds=\\E[?E:";
#endif
char default_termcap[] =
"d0|vt100|vt100-am|vt100am|dec vt100:"
"\\\n	:do=^J:co#80:li#24:cl=50\\E[;H\\E[2J:bs:am:cx:ms:\\\n	:"
"cm=5\\E[%i%d;%dH:nd=2\\E[C:\\\n	:up=2\\E[A:ce=3\\E[K:"
"\\\n	:ho=\\E[H:xn:sc=\\E7:rc=\\E8:cs=\\E[%i%d;%dr:md=\\E[1m:"
"me=\\E[m:\\\n	:mr=\\E[7m:mb=\\E[5m:us=\\E[4m:x1=\\E[34m:x2=\\E[31m:"
"x3=\\E[35m:\\\n	:x4=\\E[32m:x5=\\E[36m:x6=\\E[33m:x7=\\E[37m:"
"vs=\\E[?7l:ve=\\E[?7h:"
"\\\n	:rs=\\E>\\E[?4l\\E[?5l\\E[?7h\\E[?8h:\\\n	:"
"is=^m\\E[3g^m\\EH        \\EH        \\EH        \\EH        \\E"
"H       \\EH       \\EH        \\EH        \\EH        \\EH        \\EH";

int Ctgetent(bp, name, conn, usedefault)
	char **bp, *name;
        register conntype *conn;
	boolean usedefault;
{
        tty_3270def *t;

        t = termcap_list;
        while(t) {
             TermGloPtr(tbuf) = t->def;
             if (strcmp(name, t->def_name) == 0) {
                  TermGloPtr(tbuf) = *bp = t->def;
                  return(1);
             }
             if (tnamatch(name, t->def, FALSE)) {
                  TermGloPtr(tbuf) = *bp = t->def;
                  return(1);
             }
             t = (tty_3270def *)t->next_def;
        }
	if (usedefault == FALSE)
	    return(0);
        t = termcap_list;
        while(t) {
             TermGloPtr(tbuf) = t->def;
             if (tnamatch("default", t->def, FALSE)) {
                  TermGloPtr(tbuf) = *bp = t->def;
                  return(1);
             }
             t = (tty_3270def *)t->next_def;
        }
	TermGloPtr(tbuf) = *bp = default_termcap;
        return(1);
}

#ifdef NOTACHANCE


/*
 * tnchktc: check the last entry, see if it's tc=xxx. If so,
 * recursively find xxx and append that entry (minus the names)
 * to take the place of the tc=xxx entry. This allows termcap
 * entries to say "like an HP2621 but doesn't turn on the labels".
 * Note that this works because of the left to right scan.
 */
tnchktc()
{
	register char *p, *q;
	char tcname[16];	/* name of similar terminal */
	/*char tcbuf[TERMCAP_BUFSIZ];*/
	char *holdtbuf = TermGloPtr(tbuf);
	int l;

	p = TermGloPtr(tbuf) + strlen(TermGloPtr(tbuf)) - 2;
                              	/* before the last colon */
	while (*--p != ':')
		if (p<TermGloPtr(tbuf)) {
			write(2, "Bad termcap entry\n", 18);
			return (0);
		}
	p++;
	/* p now points to beginning of last field */
	if (p[0] != 't' || p[1] != 'c')
		return(1);
	strcpy(tcname,p+3);
	q = tcname;
	while (*q && *q != ':')
		q++;
	*q = 0;
	if (++TermGloPtr(hopcount) > MAXHOP) {
		write(2, "Infinite tc= loop\n", 18);
		return (0);
	}
	if (tgetent(tcbuf, tcname) != 1) {
		TermGloPtr(hopcount) = 0;		/* unwind recursion */
		return(0);
	}
	for (q=tcbuf; *q != ':'; q++)
		;
	l = p - holdtbuf + strlen(q);
	if (l > TERMCAP_BUFSIZ) {
		write(2, "Termcap entry too long\n", 23);
		q[TERMCAP_BUFSIZ - (p-TermGloPtr(tbuf))] = 0;
	}
	strcpy(p, q+1);
	TermGloPtr(tbuf) = holdtbuf;
	TermGloPtr(hopcount) = 0;			/* unwind recursion */
	return(1);
}

#endif
/*
 * Tnamatch deals with name matching.  The first field (termcap or
 * keymap) is a sequence of names separated by |'s, so we compare
 * against each such name. 
 *
 * For keymap == TRUE, the end character is '{' and comparison is 
 * made after conversion to upper case ascii.
 *
 * For keymap == FALSE, the end character is ':' and there is no
 * case conversion for the comparison.
 *
 * NOTE: for 11.2 california use toupper() instead of cvu().
 *	 toupper() before 11.2 is usable for arguments
 *	 in the range 'a' - 'z'.
 *
 */

boolean tnamatch (char *np, char *Bp, boolean keymap)
{
    char *Np;
    char end;

    if (np == NULL || Bp == NULL)
	return (FALSE);
    if (keymap) {
	end = '{';
    } else {
	if (*Bp == '#')
	    return (FALSE);
	end = ':';
    }
    for (;;) {
	Np = np;
	while (*Np != '\0') {
	    if (keymap) {
		if (toupper(*Bp) != toupper(*Np))
		    break;
	    } else if (*Bp != *Np)
		    break;
	    Bp++; Np++;
	}
	if (*Np == '\0' && (*Bp == '|' || *Bp == end || *Bp == '\0'))
	     return (TRUE);
	while (*Bp != '\0' && *Bp != end && *Bp != '|')
	     Bp++;
	if (*Bp == '\0' || *Bp == end)
	     return (FALSE);
	Bp++;
    }
}
/*
 * Skip to the next field.  Notice that this is very dumb, not
 * knowing about \: escapes or any such.  If necessary, :'s can be put
 * into the termcap file in octal.
 */
static char *
tskip(
	register char *bp)
{

	while (*bp && *bp != ':')
		bp++;
	if (*bp == ':')
		bp++;
        /* cisco addition */
        /* now skip over tabs, spaces, \, lf, cr, and even ff */
        while (*bp && ((*bp == ' ') ||
               (*bp == '\t') ||
               (*bp == '\\') ||
               (*bp == '\n') ||
               (*bp == '\f')))
                bp++;

	while (*bp && *bp == ':')
	    bp++;

	return (bp);
}

/*
 * Get a string valued option.
 * These are given as
 *	cl=^Z
 * Much decoding is done on the strings, and the strings are
 * placed in area, which is a ref parameter which is updated.
 * No checking on area overflow.
 */
char *Ctgetstr(id, area, conn)
	char *id, **area;
        conntype *conn;
{
	register char *bp = TermGloPtr(tbuf);

	for (;;) {
		bp = tskip(bp);
		if (!*bp)
			return (0);
		if (*bp++ != id[0] || *bp == 0 || *bp++ != id[1])
			continue;
		if (*bp == '@')
			return(0);
		if (*bp != '=')
			continue;
		bp++;
		return (tdecode(bp, area));
	}
}

/*
 * Tlen does the grung work to decode the
 * string capability escapes to determine the termcap string length.
 */

int tlen(str)
	register char *str;
{
	register int c;
	register char *dp;
	int i;
        int length;

        length = 1; /* for null terminator */

	while ((c = *str++) && c != ':') {
		switch (c) {

		case '^':
			c = *str++ & 037;
			break;

		case '\\':
			dp = "E\033^^\\\\::n\nr\rt\tb\bf\f";
			c = *str++;
nextc:
			if (*dp++ == c) {
				c = *dp++;
				break;
			}
			dp++;
			if (*dp)
				goto nextc;
			if (isdigit(c)) {
				c -= '0', i = 2;
				do
					c <<= 3, c |= *str++ - '0';
				while (--i && isdigit(*str));
			}
			break;
		}
		length++;
	}
	return (length);
}

/*
 * Tdecode does the grung work to decode the
 * string capability escapes.
 */
static char *
tdecode(str, area)
	register char *str;
	char **area;
{
	register char *cp;
	register int c;
	register char *dp;
	int i;

	cp = *area;
	while ((c = *str++) && c != ':') {
		switch (c) {

		case '^':
			c = *str++ & 037;
			break;

		case '\\':
			dp = "E\033^^\\\\::n\nr\rt\tb\bf\f";
			c = *str++;
nextc:
			if (*dp++ == c) {
				c = *dp++;
				break;
			}
			dp++;
			if (*dp)
				goto nextc;
			if (isdigit(c)) {
				c -= '0', i = 2;
				do
					c <<= 3, c |= *str++ - '0';
				while (--i && isdigit(*str));
			}
			break;
		}
		*cp++ = c;
	}
	*cp++ = 0;
	str = *area;
	*area = cp;
	return (str);
}


/*
 * termcap_clearscreen
 * Attempt to clear the screen of stdio using information in the termcap
 * database.  Use dummy storage to similat a connection and whatnot.  This
 * routine is not reentrant, nor does it need to be!
 */

static conntype cls_conn;		/* Dummy connection data structure */
static termcap_globals_struct cls_termcapdata;	/* Dummy termcap info */
static char *cls_genbuf;		/* Dummy generic buffer. */

static void menuclr_putc (char c, conntype *conn)
{
    putc(stdio, c);
}

void termcap_clearscreen (void)
{
    char area[50];
    char *cp = area;
    int i;

    cls_conn.proto.tn.termcapdata = &cls_termcapdata;
    if (stdio->termtype) {
	if (Ctgetent(&cls_genbuf, stdio->termtype, &cls_conn, FALSE)) {
	    Ctgetstr("cl", &cp, &cls_conn);
	    if (area[0]) {
		Ctputs(area, stdio->tty_length, menuclr_putc, &cls_conn);
		return;
	    }
	}
    }
    /*
     * termcap failed us.  Do "dumb" output clear by sending multiple
     * blank lines.
     */
    for (i = 1; i <= stdio->tty_length; i++)
	printf("\n");
}

