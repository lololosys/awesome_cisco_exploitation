/* $Id: if_les_serial.h,v 3.2 1995/11/17 00:00:02 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/if_les_serial.h,v $
 *------------------------------------------------------------------
 * if_les_serial.h - Header file for g eneric LES serial driver routines
 *
 * July 1995, Greg Stovall
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_les_serial.h,v $
 * Revision 3.2  1995/11/17  00:00:02  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:33:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/15  18:41:39  gstovall
 * Adding placholders.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

void les_serial_show_errors(hwidbtype *idb);
void les_serial_init_idb_common(hwidbtype *idb);
void les_serial_init_offset(hwidbtype *hwidb);
void les_serial_setup_bridging(hwidbtype *hwidb);
