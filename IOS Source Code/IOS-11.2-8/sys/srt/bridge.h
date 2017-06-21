/* $Id: bridge.h,v 3.2.62.1 1996/08/03 23:37:15 fox Exp $
 * $Source: /release/112/cvs/Xsys/srt/bridge.h,v $
 *------------------------------------------------------------------
 * bridge.h -- Device Independent Transparent Bridge Support
 *
 * June 1991, David Hampton
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bridge.h,v $
 * Revision 3.2.62.1  1996/08/03  23:37:15  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.2  1995/11/17  18:52:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:00:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * bridge.c
 */
extern boolean bridge_floodcheck(idbtype *, paktype *, int);
extern void bridge_address_check(btetype *);
extern void bridge_debug_lat(idbtype *, paktype *, char *);
extern void bridge_out(idbtype *, paktype *, multi_media_bridge_mac_t *, uchar *, int);
extern void tbridge_ager(void);
extern btetype *bridge_get_stn(uchar *, btetype **);
extern btetype *bridge_get_next_stn(uchar *, btetype **);
