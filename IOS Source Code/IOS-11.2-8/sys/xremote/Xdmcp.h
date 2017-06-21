/* $Id: Xdmcp.h,v 3.2 1995/11/17 19:26:15 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xremote/Xdmcp.h,v $
 *------------------------------------------------------------------
 * Xdmcp.h - X Display Manager Control Protocol definitions
 *
 * February 27, 1991, Kevin Paul Herbert
 *
 * Copyright (c) 1991-1994 by cisco Systems, Inc.
 *------------------------------------------------------------------
 * $Log: Xdmcp.h,v $
 * Revision 3.2  1995/11/17  19:26:15  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:01:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:31:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Copyright 1989 Network Computing Devices, Inc., Mountain View, California.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of N.C.D. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  N.C.D. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 */

#ifndef _XDMCP_H_
#define _XDMCP_H_
#define XDM_PROTOCOL_VERSION	1
#define XDM_UDP_PORT		177
#define XDM_MAX_MSGLEN		8192
#define XDM_MIN_RTX		2
#define XDM_MAX_RTX		32
#define XDM_RTX_LIMIT		7
#define XDM_KA_RTX_LIMIT	4
#define XDM_DEF_DORMANCY	(3 * 60)	/* 3 minutes */
#define XDM_MAX_DORMANCY	(24 * 60 * 60)	/* 24 hours */

typedef enum {
    BROADCAST_QUERY = 1, QUERY, INDIRECT_QUERY, FORWARD_QUERY,
    WILLING, UNWILLING, REQUEST, ACCEPT, DECLINE, MANAGE, REFUSE, 
    FAILED, KEEPALIVE, ALIVE 
} xdmOpCode;

typedef enum {
    XDM_QUERY, XDM_BROADCAST, XDM_INDIRECT, XDM_COLLECT_QUERY,
    XDM_COLLECT_BROADCAST_QUERY, XDM_COLLECT_INDIRECT_QUERY,
    XDM_START_CONNECTION, XDM_AWAIT_REQUEST_RESPONSE,
    XDM_AWAIT_MANAGE_RESPONSE, XDM_MANAGE, XDM_RUN_SESSION, XDM_OFF,
    XDM_KEEPALIVE, XDM_AWAIT_ALIVE_RESPONSE
} xdmcp_states;

#define XDM_QUERY_MAX 10
#define XDM_REQUEST_MAX 26
#define XDM_MANAGE_MAX 14
#define XDM_KEEPALIVE_MAX 12

#define X_FAMILY_INTERNET 0

#endif /* _XDMCP_H_ */
