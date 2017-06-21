/* $Id: cfg_sse.h,v 3.2 1995/11/17 09:15:09 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/cfg_sse.h,v $
 *------------------------------------------------------------------
 * SSE command parser chains
 *
 * March 1994 Tony Li
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_sse.h,v $
 * Revision 3.2  1995/11/17  09:15:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:37:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:50  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:13:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* [no] sse manager period <value>
* [no] sse manager quiet <value>
*/

EOLS	(sse_manager_period_eol, sse_manager_command, SSE_MANAGER_PERIOD);
NUMBER	(sse_manager_period_time, sse_manager_period_eol, no_alt,
	 OBJ(int,1), 1, -1, "Interval between updates");
NVGENS  (sse_manager_period_nvgen, sse_manager_period_time,
	 sse_manager_command, SSE_MANAGER_PERIOD);
KEYWORD (sse_manager_period, sse_manager_period_nvgen, no_alt,
	 "period", "Interval between updates", PRIV_CONF|PRIV_HIDDEN);

EOLS	(sse_manager_quiet_eol, sse_manager_command, SSE_MANAGER_QUIET);
NUMBER	(sse_manager_quiet_time, sse_manager_quiet_eol, no_alt, OBJ(int,1),
	 1, -1, "Minimum time between updates");
NVGENS  (sse_manager_quiet_nvgen, sse_manager_quiet_time,
	 sse_manager_command, SSE_MANAGER_QUIET);
KEYWORD (sse_manager_quiet, sse_manager_quiet_nvgen, sse_manager_period,
	 "quiet", "Minimum time between updates", PRIV_CONF|PRIV_HIDDEN);

KEYWORD (sse_manager, sse_manager_quiet, no_alt,
	 "manager", "Configure SSE manager", PRIV_CONF|PRIV_HIDDEN);

KEYWORD (sse, sse_manager, ALTERNATE, "sse", "Configure Silicon Switching",
	 PRIV_CONF|PRIV_HIDDEN);

#undef ALTERNATE
#define ALTERNATE sse

