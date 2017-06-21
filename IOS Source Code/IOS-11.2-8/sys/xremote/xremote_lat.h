/* $Id: xremote_lat.h,v 3.2 1995/11/17 19:27:04 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xremote/xremote_lat.h,v $
 *------------------------------------------------------------------
 * xremote_lat.h - XRemote lat definitions
 *  
 * December 4, 1992, Kevin Paul Herbert
 *
 * Copyright (c) 1991-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xremote_lat.h,v $
 * Revision 3.2  1995/11/17  19:27:04  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:01:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:35:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __XREMOTE_LAT_H__
#define __XREMOTE_LAT_H__

/*
 * Function Prototypes
 */

extern void xremote_lat_get_char(sb_ptr, of_byte, int, int *);
extern void xremote_lat_put_char(sb_ptr, if_byte, int);
extern boolean xremote_lat_connect(hi_ptr);
extern void xremote_lat_session(sb_ptr, int, boolean *);
extern void xremote_lat_ox_session(sb_ptr, ob_ptr, int, boolean *);
extern void xremote_show_lat(tt_soc *);

#endif __XREMOTE_LAT_H__
