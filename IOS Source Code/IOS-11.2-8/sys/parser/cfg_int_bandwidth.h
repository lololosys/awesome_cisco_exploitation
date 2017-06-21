/* $Id: cfg_int_bandwidth.h,v 3.2 1995/11/17 18:43:01 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/cfg_int_bandwidth.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ B A N D W I D T H . H
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_bandwidth.h,v $
 * Revision 3.2  1995/11/17  18:43:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:03:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * bandwidth <kilobits>
 * no bandwidth [<kilobits>]
 */
PARAMS_KEYONLY(ci_bandwidth, ALTERNATE, "bandwidth",
	       OBJ(int,1), 1, BANDWIDTH_SCALE,
	       bandwidth_command, 0,
	       "Set bandwidth informational parameter",
	       "Bandwidth in kilobits", PRIV_CONF|PRIV_SUBIF);

#undef	ALTERNATE
#define	ALTERNATE	ci_bandwidth
