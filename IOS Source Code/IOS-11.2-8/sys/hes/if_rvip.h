/* $Id: if_rvip.h,v 3.2 1995/11/17 09:18:35 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/if_rvip.h,v $
 *------------------------------------------------------------------
 * if_rvip.h:  RVIP specific constants & definitions
 *
 * July 1995, David Getchell
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_rvip.h,v $
 * Revision 3.2  1995/11/17  09:18:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:40:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:00:51  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/19  19:18:23  getchell
 * Placeholders for VIP development
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Prototypes
 */
extern void rvip_debug_init(void);
extern void rvip_platform_init(subsystype *);
extern void rvip_show_cbus(ushort);

