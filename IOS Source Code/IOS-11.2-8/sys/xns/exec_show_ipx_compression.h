/* $Id: exec_show_ipx_compression.h,v 3.2.62.2 1996/08/16 00:14:19 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/exec_show_ipx_compression.h,v $
 *------------------------------------------------------------------
 * Parser chain for show ipx compression command
 *
 * Sept 1995, Syed Irfan, (Joel Bion)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_ipx_compression.h,v $
 * Revision 3.2.62.2  1996/08/16  00:14:19  hampton
 * Change parser comments to reflect ipx command usage.  [CSCdi66208]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/08/12  16:10:04  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  19:21:36  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:38:23  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  08:18:26  irfan
 * First breath in the IOS world.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show ipx compression [detail] [interface]
 * 
 * OBJ(int,1) =	TRUE = 'detail' keyword parsed
 *		FALSE = 'detail' keyword not parsed
 * OBJ(idb,1) if interface parsed
 */
EOLNS	(show_nov_compression_eol, ipx_compression_show_command);

INTERFACE (show_nov_compression_idb, show_nov_compression_eol,
	   show_nov_compression_eol, OBJ(idb,1), IFTYPE_ANYSUB);

KEYWORD_ID(show_nov_compression_detail, show_nov_compression_idb, 
	   show_nov_compression_idb,
	   OBJ(int,1), TRUE, "detail", 
	   "Detailed link state database information", PRIV_USER);

KEYWORD_ID(show_nov_compression, show_nov_compression_detail, ALTERNATE,
	   OBJ(int,1), FALSE,
	   "compression", "IPX compression information", PRIV_USER);


#undef	ALTERNATE
#define	ALTERNATE	show_nov_compression
