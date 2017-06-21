/* $Id: cfg_int_pulse-time.h,v 3.2 1995/11/17 18:43:45 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/cfg_int_pulse-time.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ P U L S E T I M E . H
 *
 * Copyright (c) 1986-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_pulse-time.h,v $
 * Revision 3.2  1995/11/17  18:43:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:07:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 *	pulse-time <seconds>
 *	no pulse-time [<seconds>]
 */
PARAMS_KEYONLY (config_pulsetime, NONE,
		"pulse-time", OBJ(int,1), 0, -1, pulsetime_command, 0,
		"Enables pulsing of DTR during resets",
		"Seconds", PRIV_CONF);

TEST_IDBSTATUS(config_pulsetime_test, config_pulsetime, NONE, ALTERNATE,
	       IDB_SERIAL);

#undef	ALTERNATE
#define	ALTERNATE	config_pulsetime_test
