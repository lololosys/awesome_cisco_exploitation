/* $Id: exec_debug_ip_nhrp.h,v 3.3 1996/02/20 21:18:15 bcole Exp $
 * $Source: /release/111/cvs/Xsys/iprouting/exec_debug_ip_nhrp.h,v $
 *------------------------------------------------------------------
 * Next Hop Resolution Protocol.
 *
 * October 1995, Bruce Cole
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_ip_nhrp.h,v $
 * Revision 3.3  1996/02/20  21:18:15  bcole
 * CSCdi49357:  NHRP: Problems with debug ip nhrp, cache flushing
 *
 * Repair debug command backwards compatibility.  Fix ordering problem
 * when flushing cache during deconfiguration.
 *
 * Revision 3.2  1995/11/17  17:33:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:06:20  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  21:54:55  bcole
 * Placeholder files for 11.1 NHRP work.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Note: These commands are all in the name of backwards compatability.
 * See 'debug nhrp' for the new form of these commands.
 */

/******************************************************************
 * debug ip nhrp [options | rate]
 *
 */
EOLS	(debug_ip_nhrp_eol, debug_command, DEBUG_NHRP);
EOLS	(debug_ip_nhrp_rate_eol, debug_command, DEBUG_NHRP_RATE);
KEYWORD (debug_ip_nhrp_rate, debug_ip_nhrp_rate_eol, debug_ip_nhrp_eol,
	 "rate", "NHRP rate limiting", PRIV_OPR);
EOLS	(debug_ip_nhrp_opt_eol, debug_command, DEBUG_NHRP_OPT);
KEYWORD (debug_ip_nhrp_opt, debug_ip_nhrp_opt_eol, debug_ip_nhrp_rate,
	 "options", "NHRP option processing", PRIV_OPR);
KEYWORD_DEBUG (debug_ip_nhrp, debug_ip_nhrp_opt, ALTERNATE,
	       OBJ(pdb,1), nhrp_debug_arr,
	       "nhrp", "NHRP protocol", PRIV_OPR | PRIV_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	debug_ip_nhrp
