/* $Id: setjmp.h,v 3.2 1995/11/17 09:13:25 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/h/setjmp.h,v $
 *------------------------------------------------------------------
 * ANSI C Standard include file
 *
 * May 1995, Steve Berl
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: setjmp.h,v $
 * Revision 3.2  1995/11/17  09:13:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:36:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:36:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SETJMP_H__
#define __SETJMP_H__

/*
 * Target platform definitions
 */

#ifdef TARGET_CISCO
#include "target_cisco.h"
#endif

#ifdef TARGET_UNIX
#include "target_unix.h"
#endif

volatile void longjmp(jmp_buf *, int);
int setjmp(jmp_buf *);


#endif /* __SETJMP_H__ */
