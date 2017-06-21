/* $Id: exec_show_compress.h,v 3.2.62.2 1996/08/12 16:09:46 widmer Exp $
 * $Source: /release/112/cvs/Xsys/x25/exec_show_compress.h,v $
 *------------------------------------------------------------------
 * exec_show_compress.h
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_compress.h,v $
 * Revision 3.2.62.2  1996/08/12  16:09:46  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.62.1  1996/06/05  18:57:32  widmer
 * CSCdi59571:  PRIV_USER_HIDDEN used without a privilege level
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:06:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:10  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:22:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show compress 
 */

EOLNS	(show_compress_eol, show_compress);
KEYWORD (show_compression, show_compress_eol, ALTERNATE,
	 "compress", "Show compression statistics",
	 PRIV_USER | PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_compression
