/* $Id: exec_clear_ip_fast.h,v 3.3.20.1 1996/08/12 16:02:54 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ip/exec_clear_ip_fast.h,v $
 *------------------------------------------------------------------
 * IP Fast clear commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_clear_ip_fast.h,v $
 * Revision 3.3.20.1  1996/08/12  16:02:54  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/01/29  07:08:53  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  09:32:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:55:40  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:16:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * clear ip cache [prefix mask]
 *
 * OBJ(paddr,1) = prefix
 * OBJ(paddr,2) = mask
 *
 */
EOLNS	(clear_ip_cache_eol, ipcache_free_resources);

IPADDR  (clear_ip_cache_mask, clear_ip_cache_eol, no_alt,
	 OBJ(paddr,2), "Prefix mask");
IPADDR  (clear_ip_cache_prefix, clear_ip_cache_mask, clear_ip_cache_eol,
	 OBJ(paddr,1), "Address prefix");
KEYWORD (clear_ip_cache, clear_ip_cache_prefix, ALTERNATE,
	 "cache", "Delete cache table entries", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	clear_ip_cache
