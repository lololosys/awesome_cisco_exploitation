/* $Id: cfg_int_tx-queue-limit.h,v 3.2 1995/11/17 18:43:53 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/parser/cfg_int_tx-queue-limit.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ T X - Q U E U E - L I M I T . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_tx-queue-limit.h,v $
 * Revision 3.2  1995/11/17  18:43:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:07:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 *	tx-queue-limit <number>
 *	no tx-queue-limit [<number>]
 *
 * OBJ(int,1) = <number>
 */
PARAMS_KEYONLY (config_txqueuelimit, ALTERNATE,
		"tx-queue-limit", OBJ(int,1), 1, MAXSHORT,
		tx_queue_limit_command, 0,
		"Configure card level transmit queue limit",
		"Number (queue limit)", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	config_txqueuelimit
