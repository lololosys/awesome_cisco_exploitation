/* $Id: ntp_public.h,v 3.2.62.2 1996/08/19 18:55:11 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/ntp_public.h,v $
 *------------------------------------------------------------------
 * Publicly visible NTP prototypes
 *
 * February 1993, Dave Katz
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: ntp_public.h,v $
 * Revision 3.2.62.2  1996/08/19  18:55:11  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.2.62.1  1996/04/16  19:03:21  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:51:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:41:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:45:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef NTP_PUBLIC_H
#define NTP_PUBLIC_H

/* External declarations */

extern void ntp_init_idb(idbtype *, boolean);
extern void ntp_set_drift_incr(ulong, ulong);
extern void ntp_pps_tick(tt_soc *, clock_epoch *);
extern void ntp_refclock_timestamp(tt_soc *, clock_epoch *);

#endif /* NTP_PUBLIC_H */
