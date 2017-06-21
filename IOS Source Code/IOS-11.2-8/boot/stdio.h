/* $Id: stdio.h,v 3.4 1996/01/24 19:29:23 mdb Exp $
 * $Source: /vws/aam/cherf.111/ios/boot/stdio.h,v $
 *------------------------------------------------------------------
 * stdio.h -- standard I/O definitions
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stdio.h,v $
 * Revision 3.4  1996/01/24  19:29:23  mdb
 * CSCdi47425:  missing paren in NULL macro from CSCdi47263
 *
 * Revision 3.3  1996/01/22  08:59:04  mdb
 * CSCdi47263:  change definition of NULL to ((void *) 0) from 0
 *
 * Revision 3.2  1995/11/17  07:41:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:12:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * No buffering is performed; all character IO is
 * done directly to the UARTs.
 */

#define	NULL	((void *) 0)

/*
 * Macro for deblanking a string pointer, ss
 */
#define DEBLANK(ss) while (ss && *ss && isspace(*ss)) ss++;

/*
 * Externals:
 */

extern char preserve_case;
extern char digits[];
extern struct regpair *console;		/* console registers */

/*
 * Functions:
 */
char getch();
char getchar();
char *gets();
long getnum();
long gethexnum();
void putchar();
void puts();
void putnum();
void flush();
void init_console();

#ifdef	NOT_USED
long gethexbyte();
void puthexnum();
#endif
