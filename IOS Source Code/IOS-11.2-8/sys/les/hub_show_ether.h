/* $Id: hub_show_ether.h,v 3.2 1995/11/17 17:38:03 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/hub_show_ether.h,v $
 *------------------------------------------------------------------
 * hub_show_ether.h - header for ether hub specific show functions.
 *
 * March 1994, Steve Garcia
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Gurus: Steve Garcia, Viet Nguyen
 *------------------------------------------------------------------
 * $Log: hub_show_ether.h,v $
 * Revision 3.2  1995/11/17  17:38:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:30:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:32:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* 
 * Prevent multiple inclusion
 */
#ifndef __HUB_SHOW_ETHER_H__
#define __HUB_SHOW_ETHER_H__

/*
 * Function prototypes
 */
extern void hub_show_ether_general(hub_access_t *);
extern void hub_show_aui_port(hub_access_t *);
extern void hub_show_tp_port(int, hub_access_t *);

#endif

