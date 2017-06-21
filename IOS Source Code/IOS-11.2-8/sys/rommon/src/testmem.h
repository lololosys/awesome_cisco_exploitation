/* $Id: testmem.h,v 3.2 1995/11/17 18:43:25 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src/testmem.h,v $
 *------------------------------------------------------------------
 * testmem.h
 *
 * December 1992, Rob Clevenger
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Defines for the memory test routines.
 *------------------------------------------------------------------
 * $Log: testmem.h,v $
 * Revision 3.2  1995/11/17  18:43:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:38:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __TESTMEM_H__
#define __TESTMEM_H__

/*
** Structure for our memory test function.
*/

struct testmem {
  unsigned *start, *rdaddr;
  long length;
  long passcount;
  unsigned char flag;
};

/* defines for the above flag field */
#define QUITONERR  0x01    /* abort the test on first error */
#define QUICKTEST  0x02    /* do only the checkerboard portion of the test */
#define INDIAGS    0x04    /* use the diagnostic environment provided */
#define RDADDR     0x08    /* separate read and write addresses */
#define MUTE       0x10    /* keep quiet about errors, etc. */

extern int testmem(struct testmem *tmemp);
extern int memrefresh(struct testmem *tmemp);
extern int meminfo(int argc, char *argv[]);

#endif  /* __TESTMEM_H__ */
