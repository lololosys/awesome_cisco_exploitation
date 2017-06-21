/* $Id: cisco_68.h,v 3.4 1995/11/22 09:18:02 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/machine/cisco_68.h,v $
 *------------------------------------------------------------------
 * cisco_68.h - Definitions for cisco 68K based platforms
 *
 * December 1994, Scott Mackie
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cisco_68.h,v $
 * Revision 3.4  1995/11/22  09:18:02  hampton
 * Fix the mips routine to read/write unaligned longlong words.  While
 * there, rework the entire family of routines to reduce overhead.  This
 * necessitates a change to code calling these routines.  [CSCdi44257]
 *
 * Revision 3.3  1995/11/17  17:49:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:01:56  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:40:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/13  04:07:27  hampton
 * The Fall of the House of Msclock.  [CSCdi40182]
 *
 * Revision 2.1  1995/06/07 21:42:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "cpu_68.h"


/*
 * Alignment definitions
 */

/*
 * Short and Long accesses. Note that this code assumes a CPU32
 * core that'll cope with unaligned accesses.
 */
#define GETSHORT(ptr)		 (*((ushort *) (ptr)))
#define PUTSHORT(ptr, val)	 (*((ushort *) (ptr)) = (ushort) (val))
#define GETLONG(addr)		 (*((ulong *)(addr)))
#define PUTLONG(addr, value)	 (*((ulong *)(addr)) = (ulong)(value))
#define GETLONGLONG(addr, vptr)	 (*((ulonglong *)vptr) = *((ulonglong *)(addr)))
#define PUTLONGLONG(addr, vptr)  (*((ulonglong *)(addr)) = *(ulonglong*)(vptr))

/*
 * Use GET2WD and PUT2WD when retrieving a long word that is known
 * to be short aligned.  Used heavily by XNS, Novell, Apollo, and Vines.
 */
#define	GET2WD(ptr)  GETLONG(ptr)
#define PUT2WD(ptr, val)  PUTLONG(ptr, val)


/*
 * Time defines
 *
 * All of the CPU32 68K products use the same time mechanism
 *
 */

/*
 * On 68k based boxes, the ROM Monitor is the keeper of
 * the millisecond timer variables.
 */
#define GET_CLOCKADDR	mon_clockaddr()
#define GET_HCLOCKADDR	mon_hclockaddr()

#define ONESEC (1000L)			/* tics per second */
#define ONEMIN (60L*ONESEC)		/* tics per minute */
#define ONEHOUR (60L*ONEMIN)		/* tics per hour */

/*
 * TTY defines
 */

#define CONSOLE_LINENUM 0





