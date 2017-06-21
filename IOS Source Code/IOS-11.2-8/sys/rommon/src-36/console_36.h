/* $Id: console_36.h,v 3.2 1995/11/17 18:43:34 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src-36/console_36.h,v $
 *------------------------------------------------------------------
 * console.h
 *
 * January 1995, Rob Clevenger
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Console defines for Sapphire.
 *------------------------------------------------------------------
 * $Log: console_36.h,v $
 * Revision 3.2  1995/11/17  18:43:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:38:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __CONSOLE_36_H__
#define __CONSOLE_36_H__

extern unsigned short uart_speeds[];
extern char caphexnums[];

/*
** 360 baud rate defines
*/
#define BAUD9600 162
#define BAUD4800 324
#define BAUD1200 1301
#define BAUD2400 650

/*
 * Basic IO primitives needed when initializing (since you can't
 * yet trust memory).
 */

#define PUTSTRING(str) basic_strout(str);

#define PUTDIGIT(digit,shift) \
    conout(caphexnums[((digit >> shift) & 0xF)])

#define PUTHEXNUM(hexnum) \
    PUTDIGIT(hexnum,28); \
    PUTDIGIT(hexnum,24); \
    PUTDIGIT(hexnum,20); \
    PUTDIGIT(hexnum,16); \
    PUTDIGIT(hexnum,12); \
    PUTDIGIT(hexnum,8); \
    PUTDIGIT(hexnum,4); \
    PUTDIGIT(hexnum,0);

/* console.c */
extern void basic_strout(char *cp);
extern void conout(char c);
extern int conin(void);
extern int constat(void);
extern char getchar(void);
extern void putchar(char c);
extern void setmore(void);

#endif  /* __CONSOLE_36_H__ */
