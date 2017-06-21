/* $Id: if_fddi_public.h,v 3.4 1996/01/05 04:55:55 hampton Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/if/if_fddi_public.h,v $
 *------------------------------------------------------------------
 * Global FDDI definitions used by the Cisco router software.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Longer description here.
 *------------------------------------------------------------------
 * $Log: if_fddi_public.h,v $
 * Revision 3.4  1996/01/05  04:55:55  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.3  1995/11/17  09:27:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:37:56  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:50:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:52:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern ushort get_smt_bufs(ushort n);
extern void adjust_smt_bufs(void);
extern int fddi_smt_bufs_needed(void);

extern void fddi_dup_addr_shutdown(ushort stn_id, ushort mac_id);
extern void fddi_smt_frame(hwidbtype *idb, uchar *ptr, ushort size,
			   uchar eac_bits);
extern void init_cmt(hwidbtype *idb);
extern void fddi_connect(ushort stnid);
extern void fddi_disconnect(ushort stnid);
extern void set_rt_timer(timer_t *timer, ulong value);
extern void stop_rt_timer(timer_t *timer);
extern void rt_timer_int_hand(void);
extern void rt_timer_init(void);
