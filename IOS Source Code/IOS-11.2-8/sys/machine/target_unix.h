/* $Id: target_unix.h,v 3.6.8.2 1996/06/04 22:55:18 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/machine/target_unix.h,v $
 *------------------------------------------------------------------
 * target_unix.h - Definitions for Unix targets
 *
 * December 1994, Scott Mackie
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: target_unix.h,v $
 * Revision 3.6.8.2  1996/06/04  22:55:18  mbeesley
 * CSCdi59380:  Optimize buffer code for traditional platforms
 * Branch: California_branch
 *
 * Revision 3.6.8.1  1996/05/10  23:33:44  gstovall
 * CSCdi56792:  Need to remove unused files
 * Branch: California_branch
 * Nuke support for the Irix image, also stamp out the last of the CS500.
 *
 * Revision 3.6  1996/02/28  23:11:44  hampton
 * Fix timer errors in unix images.  [CSCdi45965]
 *
 * Revision 3.5  1996/02/14  17:47:31  ahh
 * CSCdi48003:  Buffer manager not optimal on all boxes
 * Make unix image compile.
 *
 * Revision 3.4  1995/12/21  16:29:18  hampton
 * Switch the 68K version of DELAY() to call the existing usecdelay
 * routine which uses the timer chip.  For all other platforms, move a
 * routine or two between files.  [CSCdi45965]
 *
 * Revision 3.3  1995/11/17  17:50:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:02:05  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:40:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/10/15  18:54:03  eschaffe
 * CSCdi40294:  Increase the number of Protocol Translation sessions
 *
 * Revision 2.3  1995/09/13  04:07:28  hampton
 * The Fall of the House of Msclock.  [CSCdi40182]
 *
 * Revision 2.2  1995/09/01 06:54:52  hampton
 * Make the unix images compile again.  [CSCdi39708]
 *
 * Revision 2.1  1995/06/07 21:43:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TARGET_UNIX_H__
#define __TARGET_UNIX_H__

#ifdef __sparc__
#include "cpu_sparc.h"
#endif

#ifdef __mips__
#include "cpu_4k.h"
#endif

#ifdef __sun__
#ifdef __svr4__
#include "unix_solaris.h"
#else
#include "unix_sunos.h"
#endif
#endif


/*
 * Interrupt level assignments for system software
 */
#define LEVEL_CONSOLE 5		/* console UART */

extern volatile ulong clock_low;
extern volatile ulong clock_high;
#define GET_CLOCKADDR	&clock_low
#define GET_HCLOCKADDR	&clock_high

#define WASTETIME(usecs) wastetime(usecs)
#define DELAY(ms)        msecdelay(ms)

/*
 * Frame alignment defines
 */
#define ALIGN_DATAGRAM

#define ONESEC (1000L)			/* tics per second */
#define ONEMIN (60L*ONESEC)		/* tics per minute */
#define ONEHOUR (60L*ONEMIN)		/* tics per hour */

#define REFRESHTIME          4          /* 4ms per tick */                 
#define NMI_FREQ_NUMERATOR 2		/* NMI freq numerator (Hz) */
#define NMI_FREQ_DENOMINATOR 1          /* NMI freq denominator */

/*
 * Address space definitions
 */

#define RAMBASE			0x00000000 /* base of RAM address space */

/*
 * Define the address of the pointer to the crash info data structure
 * This pointer (and data structure) are used by GDB as a starting point
 * for analyzing a crash dump.  This location does not need to survive a
 * reload, it just needs to appear in the crash dump.  If you change
 * this value, you must coordinate with Cisco's GDB maintainers.
 */
#define CRASH_INFO (*((crashinfo **)(RAMBASE + 0xFFC)))

/*
 * TTY defines
 */

#define CONSOLE_LINENUM 0
/*
 * Max possible lines for Unix platforms
 */
#define PLATFORM_MAXLINES 7


/*
 * unix does not cache packets in shared memory
 */
#define pak_set_dirty(x)

/*
 * unix never ignores the input holdqueue count
 */
#define pak_ignore_inputq(pak)	FALSE

/*
 * unix never deals with scattered packets
 */
#define pak_has_particles(pak)	FALSE

/*
 * Common interrupt levels
 */

#define PLATFORM_ALL_DISABLE  6	    /* disable all interrupts */
#define PLATFORM_TTY_DISABLE  5	    /* priority to shut off all types of TTY */
#define PLATFORM_NETS_DISABLE 4	    /* highest priority of a network device */
#define PLATFORM_ALL_ENABLE   0	    /* enable all interrupts */

#define	PLATFORM_FAMTYPE FAMILY_UNIX


#endif /* __TARGET_UNIX_H__ */
