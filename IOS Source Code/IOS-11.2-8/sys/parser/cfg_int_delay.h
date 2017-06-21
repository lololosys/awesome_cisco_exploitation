/* $Id: cfg_int_delay.h,v 3.2 1995/11/17 18:43:11 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/cfg_int_delay.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ D E L A Y . H
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_delay.h,v $
 * Revision 3.2  1995/11/17  18:43:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:04:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************
 *	delay <tens-of-microseconds>
 *	no delay [<tens-of-microseconds>]
 */
PARAMS_KEYONLY (ci_delay, ALTERNATE,
		"delay", OBJ(int,1), 1, 0xFFFFFF, 
		delay_command, 0,
		"Specify interface throughput delay",
		"Throughput delay (tens of microseconds)", PRIV_CONF|PRIV_SUBIF);

#undef	ALTERNATE
#define	ALTERNATE	ci_delay
