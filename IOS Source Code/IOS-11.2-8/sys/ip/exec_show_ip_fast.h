/* $Id: exec_show_ip_fast.h,v 3.4.6.2 1996/08/12 16:03:00 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ip/exec_show_ip_fast.h,v $
 *------------------------------------------------------------------
 * IP fast show commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_ip_fast.h,v $
 * Revision 3.4.6.2  1996/08/12  16:03:00  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4.6.1  1996/04/19  15:41:03  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.4.18.1  1996/03/30  05:38:47  xliu
 * Branch: IosSec_branch
 * Commit before CA split.
 *
 * Revision 3.4  1996/03/04  15:30:54  dkerr
 * CSCdi50623:  IP flow cache needs better accounting granularity
 * Better show output.  Also allow flow stats to be exported.
 *
 * Revision 3.3  1996/01/18  00:35:40  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 *
 * Revision 3.2  1995/11/17  09:32:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/25  08:31:46  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.1  1995/07/01  12:55:46  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:24:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show ip cache
 * 
 */
EOLNS	(show_ip_cache_eol, ipcache_show);
KEYWORD_ID(show_ip_flow_crypto, show_ip_cache_eol, show_ip_cache_eol,
	   OBJ(int,5), TRUE,
	   "crypto", "crypto info of flow cache", PRIV_ROOT);
KEYWORD_ID(show_ip_cache_optimum, show_ip_cache_eol, show_ip_cache_eol,
	   OBJ(int,4), TRUE,
	   "optimum", "optimum version of prefix cache", PRIV_USER);
KEYWORD_ID(show_ip_cache_cbus, show_ip_cache_eol, show_ip_cache_optimum,
	   OBJ(int,3), TRUE,
	   "cbus", "cbus cache entries", PRIV_USER);
KEYWORD_ID(show_ip_cache_flow, show_ip_flow_crypto, show_ip_cache_cbus,
	   OBJ(int,2), TRUE,
	   "flow", "flow cache entries", PRIV_USER);
KEYWORD_ID(show_ip_cache_verbose, show_ip_cache_flow, show_ip_cache_flow,
	   OBJ(int,1), TRUE,
	   "verbose", "display extra information", PRIV_USER);
INTERFACE (show_ip_cache_int, show_ip_cache_verbose, show_ip_cache_verbose,
	 OBJ(idb,1), IFTYPE_ALL);
IPADDR  (show_ip_cache_mask, show_ip_cache_int, no_alt,
	 OBJ(paddr,2), "prefix mask");
IPADDR  (show_ip_cache_prefix, show_ip_cache_mask, show_ip_cache_int,
         OBJ(paddr,1), "prefix of entries to show");
KEYWORD (show_ip_cache, show_ip_cache_prefix, ALTERNATE,
	 "cache", "IP fast-switching route cache", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_ip_cache
