/* $Id: lltimers.h,v 3.3 1995/11/21 19:34:04 sberl Exp $
 * $Source: /swtal/cherf.111/ios/sys/llc2/lltimers.h,v $
 *------------------------------------------------------------------
 * lltimers.h -- LLC2 timer routines
 * 
 * January 1991, Rick Watson
 * 
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lltimers.h,v $
 * Revision 3.3  1995/11/21  19:34:04  sberl
 * CSCdi42181:  More than 1000 LLC2 sessions causes excessive tracebacks
 * Remove the check for > 1000 llc sessions from llc2_tick().
 * Add a counter to keep track of how many llc2 sessions there really are.
 * Add this count to banner for "show llc2".
 * Make llc2_starttimer() a real function instead of an inline. It was way
 * too big.
 *
 * Revision 3.2  1995/11/17  17:44:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:35:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/03  21:09:31  hampton
 * Convert LLC2 to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi38203]
 *
 * Revision 2.2  1995/06/16 17:09:11  dkatz
 * CSCdi35956:  Timer declarations should be in a separate file
 * Eliminate obsolete macro references.
 *
 * Revision 2.1  1995/06/07  21:42:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

void llc1_timer(llctype *llc);
void adm_timer(llctype *llc);
void llc2_starttimer(llctype *llc, int offset, ulong delta);

/*
 * llc2_stoptimer
 */
inline static void llc2_stoptimer (llctype *llc, int mask)
{
    leveltype level = raise_interrupt_level(ALL_DISABLE);

    llc->timer_flags &= ~mask;

    reset_interrupt_level(level);
}


