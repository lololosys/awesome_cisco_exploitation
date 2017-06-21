/* $Id: xx_fddi_public.h,v 3.3 1995/11/17 18:43:35 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-68-c4000/xx_fddi_public.h,v $
 *------------------------------------------------------------------
 * External Prototypes for the Cisco XX FDDI Module.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: xx_fddi_public.h,v $
 * Revision 3.3  1995/11/17  18:43:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:31:16  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:25:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:55:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

void set_synoptics_leds(ushort cf_state);
void rt_timer_start(void);
void csp_rt_timer_int_hand(uchar *nullptr0, uchar *nullptr1, uchar *nullptr2);

