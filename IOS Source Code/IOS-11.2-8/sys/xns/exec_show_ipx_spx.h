/* $Id: exec_show_ipx_spx.h,v 3.2.62.1 1996/08/16 00:14:23 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/exec_show_ipx_spx.h,v $
 *------------------------------------------------------------------
 * exec_show_ipx_spx.h  SPX Protocol (parser chain for show ipx spx)
 *
 * September 1995, Syed Mohammad Irfan Ashraf
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_ipx_spx.h,v $
 * Revision 3.2.62.1  1996/08/16  00:14:23  hampton
 * Change parser comments to reflect ipx command usage.  [CSCdi66208]
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  19:21:40  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:38:26  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/26  11:57:19  irfan
 * Sequenced Packet Exchange Protocol: holding area
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show ipx spx
 * 
 */

EOLS	(show_spx_eol, show_spx, SHOW_SPX_ALL);

KEYWORD (show_ipx_spx, show_spx_eol, ALTERNATE,
	 "spx-protocol", "Sequenced Packet Exchange protocol status",
	 PRIV_ROOT);

#undef	ALTERNATE
#define	ALTERNATE	show_ipx_spx
