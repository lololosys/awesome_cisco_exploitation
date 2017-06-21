/* $Id: ctype.h,v 3.2.60.1 1996/03/18 19:36:55 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/h/ctype.h,v $
 *------------------------------------------------------------------
 * ctype.h -- character type bit definitions
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ctype.h,v $
 * Revision 3.2.60.1  1996/03/18  19:36:55  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/03/07  09:37:08  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2  1995/11/17  09:12:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:35:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:34:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __CTYPE_H__
#define __CTYPE_H__

#define	_U	01		/* upper case        class character */
#define	_L	02		/* lower case        class character */
#define	_N	04		/* numeric digit     class character */
#define	_S	010		/* whitespace        class character */
#define _P	020		/* punctuation       class character */
#define _C	040		/* control character class character */
#define _X	0100		/* hexadecimal digit class character */
#define	_B	0200		/* blank             class character */

extern const unsigned char _ctype_[];
int isalnum(int c);
int isalpha(int c);
int isascii(int c);
int iscntrl(int c);
int isdigit(int c);
int isgraph(int c);
int islower(int c);
int isprint(int c);
int ispunct(int c);
int isspace(int c);
int isupper(int c);
int isxdigit(int c);
int toascii(int c);
int tolower(int c);
int _tolower(int c);
int toupper(int c);
int _toupper(int c);

/*
 * Implementation note: _ctype_ is only 129 bytes in size. The isXXXXX
 * macros are able to properly accept values from -1 (EOF) through 127
 * values outside of that range will have undefined results.
 */
#define	isalpha(c)	((_ctype_+1)[c]&(_U|_L))
#define	isupper(c)	((_ctype_+1)[c]&_U)
#define	islower(c)	((_ctype_+1)[c]&_L)
#define	isdigit(c)	((_ctype_+1)[c]&_N)
#define	isxdigit(c)	((_ctype_+1)[c]&(_N|_X))
#define	isspace(c)	((_ctype_+1)[c]&_S)
#define ispunct(c)	((_ctype_+1)[c]&_P)
#define isalnum(c)	((_ctype_+1)[c]&(_U|_L|_N))
#define isprint(c)	((_ctype_+1)[c]&(_P|_U|_L|_N|_B))
#define isgraph(c)	((_ctype_+1)[c]&(_P|_U|_L|_N))
#define iscntrl(c)	((_ctype_+1)[c]&_C)
#define isascii(c)	((unsigned)(c)<=0177)
#define toascii(c)	((c)&0177)
/*
 * _toupper and _tolower are not really ANSI C, but they are useful
 * variations on the toupper and tolower functions. Just be sure to
 * check for islower or isupper before using them.
 */
#define _toupper(c)	((c)-'a'+'A')
#define _tolower(c)	((c)-'A'+'a')

/*
 * Non-gcc versions will get the library versions, and will be
 * slightly slower. The old macros didn't properly do nothing when the
 * character was not in the set of characters to be converted. Use the
 * macros _toupper and _tolower if you want the old behavior and have
 * made sure that the character is within the correct range.
 *
 * A macro is used here in place of an inline function so that users
 * may easily '#undef toupper' or '#undef tolower' to get the library
 * function rather than waste code space doing it inline.
 */
#ifdef __GNUC__
#define toupper(c)	\
	({ int __x = (c); islower(__x) ? (__x - 'a' + 'A') : __x;})
#define tolower(c)	\
	({ int __x = (c); isupper(__x) ? (__x - 'A' + 'a') : __x;})
#endif

#endif /* __CTYPE_H__ */
