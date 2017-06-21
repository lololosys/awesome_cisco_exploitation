/* $Id: cfg_int_netbios.h,v 3.3 1996/01/16 21:18:10 widmer Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/parser/cfg_int_netbios.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ N E T B I O S . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_netbios.h,v $
 * Revision 3.3  1996/01/16  21:18:10  widmer
 * CSCdi33940:  show extended command doesnt work
 * Add keyword_trans macro that doesn't accept the keyword
 *  unless commands have been added to the link point.
 *
 * Revision 3.2  1995/11/17  18:43:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:06:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************/
KEYWORD_TRANS(ci_netbios, ci_netbios_extend_here, no_alt, ALTERNATE,
	      "netbios",
	      "Use a defined NETBIOS access list or enable name-caching",
	      PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	ci_netbios
