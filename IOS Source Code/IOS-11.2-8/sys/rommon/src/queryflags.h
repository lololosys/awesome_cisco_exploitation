/* $Id: queryflags.h,v 3.2 1995/11/17 18:42:57 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src/queryflags.h,v $
 *------------------------------------------------------------------
 * queryflags.h
 *
 * February 1993, Rob Clevenger
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * queryflags.h - defines for query_user() in query.c
 *------------------------------------------------------------------
 * $Log: queryflags.h,v $
 * Revision 3.2  1995/11/17  18:42:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:38:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __QUERYFLAGS_H__
#define __QUERYFLAGS_H__

/*
** queryflags.h - defines for query_user() in query.c
**
** Mechanism to query the user for test parameters.
** Note that the order of the assigned bits is critical
** to the proper functioning of the query() routine
** (esp. QU_R_WR, QU_VALUE, and QU_OPSIZ).
*/

typedef unsigned int QUERYFLAG;

#define QU_SOURCE      0x01
#define QU_SOURCE_BIT  0
#define QU_DEST        0x02
#define QU_DEST_BIT    1
#define QU_START       0x04
#define QU_START_BIT   2
#define QU_SIZE        0x08
#define QU_SIZE_BIT    3
#define QU_R_WR        0x10
#define QU_R_WR_BIT    4
#define QU_VALUE       0x20
#define QU_VALUE_BIT   5
#define QU_PASSES      0x40
#define QU_PASSES_BIT  6
#define QU_OPSIZ       0x80
#define QU_OPSIZ_BIT   7
#define QU_INCPAT      0x100
#define QU_INCPAT_BIT  8
#define QU_TRIGGER     0x200
#define QU_TRIGGER_BIT 9

extern void query_user(QUERYFLAG query, ...);
extern int getc_answer(char *msg, char *cmpstr, char curval);
extern unsigned gethex_answer(char *msgstr, unsigned currentval, unsigned min, unsigned max);

#endif  /* __QUERYFLAGS_H__ */
