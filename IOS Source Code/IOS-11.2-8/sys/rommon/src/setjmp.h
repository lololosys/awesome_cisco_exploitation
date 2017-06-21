/* $Id: setjmp.h,v 3.2 1995/11/17 18:43:04 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src/setjmp.h,v $
 *------------------------------------------------------------------
 * setjmp.h
 *
 * June 1993, Michael Beesley
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Defines for setjump, longjump.
 *------------------------------------------------------------------
 * $Log: setjmp.h,v $
 * Revision 3.2  1995/11/17  18:43:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:38:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __SETJMP_H__
#define __SETJMP_H__

typedef int jmp_buf[13];

extern int setjmp( jmp_buf );
extern void longjmp( jmp_buf *, int);
#endif  /* __SETJMP_H__ */
