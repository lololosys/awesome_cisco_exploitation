/* $Id: error.h,v 3.2 1995/11/17 18:41:44 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src/error.h,v $
 *------------------------------------------------------------------
 * error.h
 *
 * July 1992, Rob Clevenger
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Defines and prototypes for diagnostic error printing routines.
 *------------------------------------------------------------------
 * $Log: error.h,v $
 * Revision 3.2  1995/11/17  18:41:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:37:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __ERROR_H__
#define __ERROR_H__

#define TESTNAMEBUFSIZ 80
extern char *errlog_start;
extern char testnamebuf[];
extern unsigned long testpass, errcount, err_accum;
extern void testname(char *string,...);
extern void cterr(char errtype, int errnum, char *errstr, ...);
extern int stoponerr(void);
extern void errleds(int errnum);
extern void clrline(char *string);
extern void prpass(int pass, char *msg, ...);
extern void prcomplete(int pass, int errcount, char *msg, ...);
extern void bell(int n);
extern void logprintf(char *buffer);
extern int dumperrlog(void);
extern void clrerrlog(void);

#endif   /* __ERROR_H__ */
