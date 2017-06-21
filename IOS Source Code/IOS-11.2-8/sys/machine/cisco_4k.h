/* $Id: cisco_4k.h,v 3.3 1995/11/17 17:49:50 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/machine/cisco_4k.h,v $
 *------------------------------------------------------------------
 * cisco_4k.h - Definitions for cisco R4X00 platforms
 *
 * December 1994, Scott Mackie
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cisco_4k.h,v $
 * Revision 3.3  1995/11/17  17:49:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:01:55  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:40:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/13  04:07:26  hampton
 * The Fall of the House of Msclock.  [CSCdi40182]
 *
 * Revision 2.1  1995/06/07 21:42:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "cpu_4k.h"

/*
 * Frame alignment defines
 */
#define ALIGN_DATAGRAM

/*
 * Time defines
 *
 * All of the MIPS R4X00 products use the same time mechanism
 *
 */

#ifndef ASMINCLUDE
extern volatile unsigned long r4k_clock, r4k_hclock;
#endif

/*
 * On R4k based boxes, the system image is the keeper
 * of the millisecond timer variables
 */
#define GET_CLOCKADDR	&r4k_clock
#define GET_HCLOCKADDR	&r4k_hclock

#define ONESEC          (1000L)			/* tics per second */
#define ONEMIN          (60L*ONESEC)		/* tics per minute */
#define ONEHOUR         (60L*ONEMIN)		/* tics per hour */

/*
 * TTY defines
 */

#define CONSOLE_LINENUM 0
