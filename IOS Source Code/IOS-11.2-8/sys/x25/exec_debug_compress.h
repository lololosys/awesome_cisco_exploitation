/* $Id: exec_debug_compress.h,v 3.2 1995/11/17 18:06:05 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/x25/exec_debug_compress.h,v $
 *------------------------------------------------------------------
 * EXEC_DEBUG_COMPRESS.H
 *
 * 10/29/93 Bill Thomas
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_compress.h,v $
 * Revision 3.2  1995/11/17  18:06:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:06  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/19  14:47:41  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:18:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug compress
 *
 */
EOLS	(debug_compress_eol, debug_command, DEBUG_COMPRESS);
KEYWORD_DEBUG(debug_compress, debug_compress_eol, ALTERNATE,
	      OBJ(pdb,1), compress_debug_arr,
	      "compress", "COMPRESS traffic", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_compress
