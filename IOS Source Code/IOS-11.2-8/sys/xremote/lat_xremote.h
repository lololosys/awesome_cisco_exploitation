/* $Id: lat_xremote.h,v 3.2 1995/11/17 19:26:34 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xremote/lat_xremote.h,v $
 *------------------------------------------------------------------
 * lat_xremote.h - XRemote lat definitions
 *  
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lat_xremote.h,v $
 * Revision 3.2  1995/11/17  19:26:34  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:01:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:31:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __LAT_XREMOTE_H__
#define __LAT_XREMOTE_H__

/*
 * Function Prototypes
 */

extern boolean xremote_lat_session_connect(char *);
extern void xremote_lat_client_writable(struct client *);
extern boolean xremote_lat_client_connected(struct client *);
extern void xremote_lat_get_data(struct client *);
extern void xremote_lat_close_client(struct client *);
extern void show_xremote_lat_client(struct client *);
extern boolean xremote_lat_file_read(xrcontext *, char *);
#endif __LAT_XREMOTE_H__
