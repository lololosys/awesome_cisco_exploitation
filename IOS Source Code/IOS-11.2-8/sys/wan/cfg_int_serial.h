/* $Id: cfg_int_serial.h,v 3.2 1995/11/17 18:01:12 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/wan/cfg_int_serial.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ S E R I A L . H
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_serial.h,v $
 * Revision 3.2  1995/11/17  18:01:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:57  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:07:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

/***************************************************************
 *	serial restart-delay <time-in-seconds>
 *	no serial restart-delay [<time-in-seconds>]
 *
 * OBJ(int,1) = <time-in-seconds>
 */

PARAMS_KEYONLY (config_serial_restart_delay, NONE,
		"restart-delay", OBJ(int,1), 0, 900, 
		serial_restart_delay_command, 0,
		"Configure serial interface restart-delay",
		"Restart delay (seconds)", PRIV_CONF);

KEYWORD	(config_serial, config_serial_restart_delay, NONE,
	 "serial", "serial interface commands",
	 PRIV_CONF);

TEST_IDBSTATUS(config_serial_test, config_serial, NONE, ALTERNATE, IDB_SERIAL);

#undef	ALTERNATE
#define	ALTERNATE	config_serial_test
