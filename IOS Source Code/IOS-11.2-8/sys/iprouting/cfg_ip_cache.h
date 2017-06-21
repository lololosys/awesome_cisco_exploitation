/* $Id: cfg_ip_cache.h,v 3.1.2.1 1996/04/15 21:17:40 bcole Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_ip_cache.h,v $
 *------------------------------------------------------------------
 * IP route cache commands.
 *
 * January 1996, Bruce Cole
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_ip_cache.h,v $
 * Revision 3.1.2.1  1996/04/15  21:17:40  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/15  01:40:22  bcole
 * Placeholders for 11.2 features.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 * [no] ip cache-ager-interval [<runtime> <fast> <slow>]
 * OBJ(int,1) = run interval in seconds
 * OBJ(int,2) = fast age
 * OBJ(int,3) = slow (normal) age
 */

EOLS	(cip_cache_eol, ipcache_command, IPCACHE_AGER);
NUMBER	(cip_cache_ager_slow, cip_cache_eol, no_alt,
	 OBJ(int,3), 3, 100, "Fraction of cache to age per run (normal)");
NUMBER	(cip_cache_ager_fast, cip_cache_ager_slow, no_alt,
	 OBJ(int,2), 2, 50, "Fraction of cache to age per run (low memory)");
NUMBER	(cip_cache_ager_value, cip_cache_ager_fast, no_alt,
	 OBJ(int, 1), 0, MAXINT/ONESEC, "# seconds between ager runs");
NOPREFIX(cip_cache_ager_no, cip_cache_ager_value, cip_cache_eol);
NVGENS	(cip_cache_ager_nv, cip_cache_ager_no, ipcache_command, IPCACHE_AGER);
KEYWORD (cip_cache_ager, cip_cache_ager_nv, ALTERNATE,
	 "cache-ager", "Control IP route cache aging",
	 PRIV_CONF|PRIV_INTERNAL); 

/******************************************************************************
 * ip cache-invalidate-delay [min [max quiet thresh]]
 * no ip cache-invalidate-delay
 *
 * OBJ(int,1) = minimum time in seconds
 * OBJ(int,2) = maximum time in seconds (NZ if any parameters specified)
 * OBJ(int,3) = quiet interval in seconds
 * OBJ(int,4) = quiet threshold
 *
 * "no" turns it off
 * no parameters set defaults
 */
EOLS    (cip_cache_delay_eol, ipcache_command, IPCACHE_INVALIDATE);
NUMBER  (cip_cache_delay_thresh, cip_cache_delay_eol, no_alt,
	 OBJ(int,4), 0, -1, "Invalidation request quiet threshold");
NUMBER  (cip_cache_delay_quiet, cip_cache_delay_thresh, no_alt,
	 OBJ(int,3), 1, 600, "Quiet interval in seconds");
NUMBER  (cip_cache_delay_max, cip_cache_delay_quiet, no_alt,
	 OBJ(int,2), 1, 300, "Maximum delay in seconds");
NUMBER  (cip_cache_delay_min, cip_cache_delay_max, cip_cache_delay_eol,
	 OBJ(int,1), 0, 300, "Minimum delay in seconds");
NOPREFIX(cip_cache_delay_no, cip_cache_delay_min, cip_cache_delay_eol);
NVGENS  (cip_cache_delay_nv, cip_cache_delay_no, ipcache_command,
	 IPCACHE_INVALIDATE);
KEYWORD (cip_cache_delay, cip_cache_delay_nv, cip_cache_ager,
         "cache-invalidate-delay", "Delay IP route cache invalidation",
	 PRIV_CONF|PRIV_INTERNAL);
#undef	ALTERNATE
#define	ALTERNATE	cip_cache_delay
