/* $Id: cfg_mop.h,v 3.2 1995/11/17 08:58:28 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/decnet/cfg_mop.h,v $
 *------------------------------------------------------------------
 * C F G _ M O P . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_mop.h,v $
 * Revision 3.2  1995/11/17  08:58:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:15:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:44:26  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:11:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * mop
 *
 * mop configure interface commands
 */

/***************************************************************
 * mop retries <number>
 *
 */
PARAMS_KEYONLY(mop_retry, no_alt, "retries",
	       OBJ(int,1), MOP_MIN_RETRIES, MOP_MAX_RETRIES,
	       mop_config, MOP_CFG_RETRIES,
               "Set maximum retransmissions of MOP boot requests",
               "Maximum MOP boot request retransmissions", PRIV_CONF);

/***************************************************************
 * mop retransmit-timer <number>
 *
 */
PARAMS_KEYONLY(mop_retransmit, mop_retry, "retransmit-timer",
	       OBJ(int,1), MOP_MIN_RETRANSMIT, MOP_MAX_RETRANSMIT,
	       mop_config, MOP_CFG_RETRANSMIT,
	       "Set timeout period for retransmitting MOP boot requests",
	       "Timeout period for retransmitting MOP boot requests",
	       PRIV_CONF);

/***************************************************************
 * mop device-code { ds200 | cisco }
 *
 */
EOLS	(mop_dev_eol, mop_config, MOP_CFG_DEVICE_CODE);
KEYWORD_ID(mop_dev_ds200, mop_dev_eol, no_alt,
	   OBJ(int,1), MOP_DEVICE_DS200,
	   "ds200", "DECserver 200 device code", PRIV_CONF);
KEYWORD_ID(mop_dev_cisco, mop_dev_eol, mop_dev_ds200,
	   OBJ(int,1), MOP_DEVICE_CISCO,
	   "cisco", "cisco device code", PRIV_CONF);
NOPREFIX(mop_dev_noprefix, mop_dev_cisco, mop_dev_eol);
KEYWORD	(mop_device, mop_dev_noprefix, mop_retransmit,
	 "device-code", "Set the device code to use in sysid messages",
	 PRIV_CONF);

KEYWORD	(mop, mop_device, ALTERNATE, "mop",
	 "Configure the DEC MOP Server", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	mop
