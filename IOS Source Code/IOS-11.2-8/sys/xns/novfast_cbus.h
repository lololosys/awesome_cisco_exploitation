/* $Id: novfast_cbus.h,v 3.2 1995/11/17 19:23:33 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/novfast_cbus.h,v $
 *------------------------------------------------------------------
 * Public prototypes for novfast_cbus.c
 *
 * February 1993, R. Li Fo Sjoe
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: novfast_cbus.h,v $
 * Revision 3.2  1995/11/17  19:23:33  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:59:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:29:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern void cbus_ipxcache_init(void);
extern void cbus_ipxcache_init_callback(void);
extern void cbus_ipxcache_show(void);
extern void cbus_ipxcache_update (paktype *);
extern void cbus_ipxcache_version (void);
extern void cbus_ipxcache_invalidate(novcachetype *);

