/* $Id: test.h,v 3.2 1995/11/17 07:42:00 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/test.h,v $
 *------------------------------------------------------------------
 * test.h -- common definitions for the test modules
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1992 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: test.h,v $
 * Revision 3.2  1995/11/17  07:42:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:05:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:12:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define	MAXBUSERRORS	20	/* number of bus errors we can tolerate */
#define PASSES		2	/* default number of passes */

extern char *function;		/* read or write function */
extern volatile short buserrcnt; /* bus error count */

void buserrnote();
