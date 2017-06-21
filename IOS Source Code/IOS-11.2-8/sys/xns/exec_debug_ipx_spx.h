/* $Id: exec_debug_ipx_spx.h,v 3.2 1995/11/17 19:21:31 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/exec_debug_ipx_spx.h,v $
 *------------------------------------------------------------------
 * exec_debug_ipx_spx.h  SPX Protocol  (parser chain for debug flag)
 *
 * September 1995, Syed Mohammad Irfan Ashraf
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_ipx_spx.h,v $
 * Revision 3.2  1995/11/17  19:21:31  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:38:19  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/26  11:57:18  irfan
 * Sequenced Packet Exchange Protocol: holding area
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug ipx spx
 *
 */

EOLS    (debug_novell_spx_eol, spx_debug_command,
	 DEBUG_NOVELLSPX);
KEYWORD (debug_novell_spx,
	 debug_novell_spx_eol, no_alt,
	 "spx", "Sequenced Packet Exchange Protocol", PRIV_OPR);

#undef ALTERNATE
#define ALTERNATE debug_novell_spx
