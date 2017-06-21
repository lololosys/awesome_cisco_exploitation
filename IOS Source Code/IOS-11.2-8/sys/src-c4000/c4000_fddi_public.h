/* $Id: c4000_fddi_public.h,v 3.4 1996/01/05 04:56:58 hampton Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/src-c4000/c4000_fddi_public.h,v $
 *------------------------------------------------------------------
 * external prototypes for c4000_fddi.c
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * external protos for xx specific xdi/chipset interface code
 *------------------------------------------------------------------
 * $Log: c4000_fddi_public.h,v $
 * Revision 3.4  1996/01/05  04:56:58  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.3  1995/11/17  18:45:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:32:26  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:26:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:56:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __C4000_FDDI_C__
#define __C4000_FDDI_C__

extern void fddi_system_configured(hwidbtype *idb);
extern void xxfddi_clearcounters (hwidbtype *idb);
extern timer_t *create_rt_timer(void *func, void *arg1, void *arg2,
				void *arg3, char *name, timer_types type);

#endif __C4000_FDDI_C__

