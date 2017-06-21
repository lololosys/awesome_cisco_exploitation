/* $Id: random.h,v 3.2 1996/01/11 11:32:41 billw Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/util/random.h,v $
 *------------------------------------------------------------------
 * random.h - Definitions and Externs for random number generator
 *
 * June 1995, David Hampton
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: random.h,v $
 * Revision 3.2  1996/01/11  11:32:41  billw
 * CSCdi43115:  PPP CHAP should use larger challenge for radius
 * compatibility
 * and a .h  Sigh.
 *
 * Revision 3.1  1995/11/09  13:43:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/30  05:21:47  hampton
 * Move externs for the random number generator to their own file.  Remove
 * redundant initialization of the random number generator.  [CSCdi36588]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * util/random.c
 */
extern void random_init(long, long);
extern long random_gen(void);

/*
 * util/random_fill.h
 */
extern void random_fill(uchar*, int, boolean);
