/* $Id: xremote_local.h,v 3.2 1995/11/17 19:27:08 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xremote/xremote_local.h,v $
 *------------------------------------------------------------------
 * xremote_local.h - XRemote local client definitions
 *  
 * September 21, 1991, Kevin Paul Herbert
 *
 * Copyright (c) 1991-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xremote_local.h,v $
 * Revision 3.2  1995/11/17  19:27:08  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:01:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:35:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __XREMOTE_LOCAL_H__
#define __XREMOTE_LOCAL_H__

#include "Xproto.h"

/*
 * Data structures
 */

/*
 * Queue header for something on the local request queue. The actual
 * request follows.
 */

typedef struct local_req_ {
    struct local_req_ *next;
} local_req;

/*
 * Function Prototypes
 */

extern boolean xremote_local_client_writable(struct client *);
extern boolean xremote_local_client_readable(struct client *);
extern void xremote_local_client_write(struct client *);
struct client *xremote_local_connect(tt_soc *tty);
extern void xremote_local_client_close(struct client *);

extern void XSetFontPath(struct client *, char **, int);

#endif __XREMOTE_LOCAL_H__
