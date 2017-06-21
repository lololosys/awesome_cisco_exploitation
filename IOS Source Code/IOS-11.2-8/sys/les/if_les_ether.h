/* $Id: if_les_ether.h,v 3.3 1995/11/17 17:40:19 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/if_les_ether.h,v $
 *------------------------------------------------------------------
 * if_les_ether.h -- definitions for Ethernet generic driver
 * 
 * November 1994, Ashwin Baindur
 *
 * Copyright (c) 1988-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_les_ether.h,v $
 * Revision 3.3  1995/11/17  17:40:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:59:16  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:32:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:36:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IF_LES_ETHER_H__
#define __IF_LES_ETHER_H__

/*
 * Prototypes
 */
void les_ethernet_line_up(hwidbtype *idb);
void les_ethernet_idb_init(hwidbtype * hwidb);
void les_ethernet_periodic(hwidbtype *hwidb);
void les_ethernet_show_errors(hwidbtype *hwidb);

#endif   /* __IF_LES_ETHER_H__ */
