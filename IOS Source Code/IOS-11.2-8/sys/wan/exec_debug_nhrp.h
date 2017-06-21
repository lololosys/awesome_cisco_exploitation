/* $Id: exec_debug_nhrp.h,v 3.2.64.1 1996/03/28 03:31:24 bcole Exp $
 * $Source: /release/111/cvs/Xsys/wan/exec_debug_nhrp.h,v $
 *------------------------------------------------------------------
 * Next Hop Resolution Protocol.
 *
 * October 1995, Bruce Cole
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_nhrp.h,v $
 * Revision 3.2.64.1  1996/03/28  03:31:24  bcole
 * CSCdi51494:  NHRP: Prefix entries not appearing in cache in many cases
 * Branch: California_branch
 *
 * Bring 11.1 fixes forward...
 *
 * Revision 3.2  1995/11/17  18:01:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:35:16  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  21:55:05  bcole
 * Placeholder files for 11.1 NHRP work.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug nhrp [options | rate | cache]
 *
 */
EOLS	(debug_nhrp_eol, debug_command, DEBUG_NHRP);

EOLS	(debug_nhrp_cache_eol, debug_command, DEBUG_NHRP_CACHE);
KEYWORD (debug_nhrp_cache, debug_nhrp_cache_eol, debug_nhrp_eol,
	 "cache", "NHRP cache operations", PRIV_OPR);

EOLS	(debug_nhrp_rate_eol, debug_command, DEBUG_NHRP_RATE);
KEYWORD (debug_nhrp_rate, debug_nhrp_rate_eol, debug_nhrp_cache,
	 "rate", "NHRP rate limiting", PRIV_OPR);

EOLS	(debug_nhrp_opt_eol, debug_command, DEBUG_NHRP_OPT);
KEYWORD (debug_nhrp_opt, debug_nhrp_opt_eol, debug_nhrp_rate,
	 "options", "NHRP option processing", PRIV_OPR);
KEYWORD_DEBUG (debug_nhrp, debug_nhrp_opt, ALTERNATE,
	       OBJ(pdb,1), nhrp_debug_arr,
	       "nhrp", "NHRP protocol", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_nhrp
