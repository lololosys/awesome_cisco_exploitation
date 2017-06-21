/* $Id: cfg_ipx_nlsp.h,v 3.4 1996/01/02 06:04:13 john Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/xns/cfg_ipx_nlsp.h,v $
 *------------------------------------------------------------------
 * C F G _ I P X _ N L S P . H
 * 
 * NLSP specific global commands.
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_ipx_nlsp.h,v $
 * Revision 3.4  1996/01/02  06:04:13  john
 * CSCdi46270:  ipx nlsp help display is confusing
 *              part 2, per code review. make this a hidden command
 *
 * Revision 3.3  1996/01/01  00:49:01  john
 * CSCdi46270:  ipx nlsp help display is confusing
 *
 * Revision 3.2  1995/11/17  19:21:04  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/21  18:18:29  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  23:27:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
 *	[no] ipx nlsp eigrp-preferred
 */
EOLS	(ipx_nlsp_eigrp_preferred_eol, nlsp_if_command,
	 IPX_NLSP_EIGRP_PREFERRED);

KEYWORD (ipx_nlsp_eigrp_preferred, ipx_nlsp_eigrp_preferred_eol, no_alt,
	 "eigrp-preferred", "Prefer EIGRP routes over NLSP routes",
	 PRIV_CONF|PRIV_HIDDEN);

KEYWORD	(ipx_nlsp_key, ipx_nlsp_eigrp_preferred, ALTERNATE,
	 "nlsp", "Select an NLSP global option",
	 PRIV_CONF|PRIV_HIDDEN);

#undef  ALTERNATE
#define ALTERNATE ipx_nlsp_key
