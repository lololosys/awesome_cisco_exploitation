/* $Id: x25_tcp.h,v 3.4 1996/02/08 18:02:20 ahh Exp $
 * $Source: /release/111/cvs/Xsys/x25/x25_tcp.h,v $
 *------------------------------------------------------------------
 * x25_tcp.c -- X25 TCP support
 *
 * July 1989, Greg Satz
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x25_tcp.h,v $
 * Revision 3.4  1996/02/08  18:02:20  ahh
 * CSCdi48380:  TCP: TCP Driver close callback needs more context
 * Have some t with your cookie.
 *
 * Revision 3.3  1995/11/21  08:27:41  ahh
 * CSCdi42413:  XOT: cannot tune TCP keepalive behavior
 * Allow keepalive rate to be controlled for XOT connections.
 *
 * Revision 3.2  1995/11/17  18:08:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:54:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:23:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __X25_TCP_H__
#define __X25_TCP_H__

/*
 * x25_tcp.c
 */
extern tcp_encaps *x25_switch_tcp_open(ipaddrtype, ipaddrtype, void *,
				       uchar *diag);
extern void x25_switch_tcp_close(lcitype *, tcp_encaps *, paktype *);
extern void x25_switch_tcp_listen(tcbtype *);
extern void x25_switch_tcp_netclose(tcp_encaps *, void *);
extern void x25_switch_tcp_read(tcp_encaps *, tcbtype *, int, void *);
extern void x25_switch_tcp_readtrans(tcp_encaps *, tcbtype *, int, void *);
extern void x25_switch_tcp_read_flush(tcp_encaps *, tcbtype *, int, void *);
extern void x25_switch_tcp_send(lcitype *, paktype *);
extern void x25_switch_tcp_nvparms(x25routeentrytype *);
extern void x25_switch_tcp_setparms(tcp_encaps *, ulong, int);

#endif /* __X25_TCP_H__ */
