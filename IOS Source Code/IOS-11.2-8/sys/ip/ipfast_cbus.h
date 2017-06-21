/* $Id: ipfast_cbus.h,v 3.2 1995/11/17 09:34:44 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ip/ipfast_cbus.h,v $
 *------------------------------------------------------------------
 * Public prototypes for ipfast_cbus.c
 *
 * February 1993, Dave Katz
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: ipfast_cbus.h,v $
 * Revision 3.2  1995/11/17  09:34:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:56:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:58:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern void cbus_ipcache_init(void);
extern void cbus_ipcache_init_callback(void);
extern void cbus_routecache_parser_init(void);


