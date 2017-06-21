/* $Id: tcplat.h,v 3.2 1995/11/17 19:00:26 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/pt/tcplat.h,v $
 *------------------------------------------------------------------
 * TCP/LAT conversion specific definitions and prototypes
 *
 * December 1993, Eman Schaffer, Scott Mackie
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: tcplat.h,v $
 * Revision 3.2  1995/11/17  19:00:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:02:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:35:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Prototypes
 */

/*
 * tcplat.c
 */
void pt_tcplat_init(subsystype *subsys);
int tcp_transto_lat(translatetype *trans, tt_soc *tty);

/*
 * lattcp.c
 */

int lat_transto_tcp(translatetype *trans, tt_soc *tty);




