/* $Id: platform_clock.h,v 3.2.62.1 1996/08/19 19:01:24 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/os/platform_clock.h,v $
 *------------------------------------------------------------------
 * Definitions for use with platform-dependent clock support
 *
 * December 1993, Dave Katz
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: platform_clock.h,v $
 * Revision 3.2.62.1  1996/08/19  19:01:24  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.2  1995/11/17  18:50:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:47:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/16  04:29:23  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  21:57:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

typedef ulong (*get_resid_type)(void);

void get_timer_parameters (ulong *, ulong *, ulong *, get_resid_type *,
			   ulong *);
