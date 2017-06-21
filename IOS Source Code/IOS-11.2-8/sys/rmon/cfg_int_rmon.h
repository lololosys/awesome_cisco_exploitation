/* $Id: cfg_int_rmon.h,v 3.2 1995/11/17 18:39:07 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rmon/cfg_int_rmon.h,v $
 *------------------------------------------------------------------
 * cfg_int_rmon.h:  Remote Monitoring Interface Configuration Commands
 *
 * August 1995, Jeffrey T. Johnson
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_rmon.h,v $
 * Revision 3.2  1995/11/17  18:39:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:02:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLS    (rmon_promiscuous_eol, rmon_int_command, RMON_PROMISCUOUS);
KEYWORD (rmon_promiscuous, rmon_promiscuous_eol, NONE,
	 "promiscuous", "Monitor the interface in promiscuous mode",
	 PRIV_CONF);

EOLS    (rmon_native_eol, rmon_int_command, RMON_NATIVE);
KEYWORD (rmon_native, rmon_native_eol, rmon_promiscuous,
	 "native", "Monitor the interface in native mode", PRIV_CONF);
NOPREFIX(rmon_no, rmon_native, rmon_native_eol);
KEYWORD (rmon, rmon_no, NONE,
         "rmon", "Configure Remote Monitoring on an interface", PRIV_CONF);

TEST_IDBSTATUS(rmon_int_test, rmon, NONE, ALTERNATE, IDB_ETHER);

#undef  ALTERNATE
#define ALTERNATE rmon_int_test
