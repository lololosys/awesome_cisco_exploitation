/* $Id: novell_public.h,v 3.2 1995/11/17 19:23:12 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/novell_public.h,v $
 *------------------------------------------------------------------
 * Public definitions for IPX
 *
 * January 1993, Dave Katz
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: novell_public.h,v $
 * Revision 3.2  1995/11/17  19:23:12  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:59:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:39:15  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  23:28:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _NOVELL_PUBLIC_H_
#define _NOVELL_PUBLIC_H_

/*
 * nov_cost_type
 *
 * Used to carry lots of parameters from the innards of ISIS/NLSP down to
 * the IPX routines.  This info is needed to make the order-of-preference
 * strategy work.  Gawd.
 */
typedef struct nov_cost_type_ {
    ulong total_throughput;		/* End-to-end throughput */
    ulong total_delay;			/* End-to-end delay */
    ushort mtu;				/* End-to-end MTU */
    uchar system_id[IEEEBYTES];		/* System ID of next hop */
    uchar mac_addr[IEEEBYTES];		/* MAC address of next hop */
    uchar circuit_id;			/* Circuit ID of outgoing interface */
    uchar dummy;			/* For alignment */
} nov_cost_type;

typedef struct nov_cost_param_type_ {
    ulong metric;			/* end-to-end path cost */
    ushort delay;			/* Total ticks of path */
    ushort hops;			/* Total hops of path */
    ushort ext_delay;			/* External ticks */
    ushort ext_hops;			/* External hops */
    ushort area_count;                  /* Area count */
    nov_cost_type oop;			/* Order of preference costs */
} nov_cost_param_type;

/*
 * External declarations
 */

extern boolean novell_on_idb(idbtype *);
extern boolean novell_on_hwidb(hwidbtype *);

#endif /* _NOVELL_PUBLIC_H_ */
