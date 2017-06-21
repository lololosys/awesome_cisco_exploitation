/* $Id: x25_route.h,v 3.3.56.2 1996/09/10 03:08:35 snyder Exp $
 * $Source: /release/112/cvs/Xsys/x25/x25_route.h,v $
 *------------------------------------------------------------------
 * x25_route.h	-- Prototypes for x25_route.c
 *
 * October 12, 1991, Tony Li
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x25_route.h,v $
 * Revision 3.3.56.2  1996/09/10  03:08:35  snyder
 * CSCdi68568:  more constant opportuniites
 *              160 out of data
 *               72 out of image
 * Branch: California_branch
 *
 * Revision 3.3.56.1  1996/08/07  09:04:51  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/21  08:27:36  ahh
 * CSCdi42413:  XOT: cannot tune TCP keepalive behavior
 * Allow keepalive rate to be controlled for XOT connections.
 *
 * Revision 3.2  1995/11/17  18:08:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:54:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/20  20:39:06  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  23:22:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * x25_route.c
 */
extern char *const x25_funcs[];
extern const char not_routing_x25_msg[];
extern void x25_route_init(void);
extern x25routeentrytype *x25_route_lookup(idbtype *idb, addrtype *addr,
					   uchar *cudptr, uint cudlen);  
extern boolean x25_for_interf(lcitype *lci, char *cud, uint cud_len);
extern boolean x25_direct_call(lcitype *lci, uchar *cud, uint cud_len,
			       uchar *cause, uchar *diag, uchar **reason);
extern x25routeentrytype *x25_forus_stream(addrtype *addr,
					   uchar *cud, int cud_len); 
extern void x25_add_route(boolean, uchar *, uchar *, uchar *, uchar *, 
	idbtype *, void *, int, int, int, ipaddrtype, ipaddrtype,
	ipaddrtype, ipaddrtype, ipaddrtype, ipaddrtype, ulong, int);
extern void show_x25route(void);
extern idbtype *pick_x25_idb(uchar *x121addr, uchar *cud, int cudlen);
