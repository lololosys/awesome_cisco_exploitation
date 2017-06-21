/* $Id: cfg_int_cdp.h,v 3.2 1995/11/17 08:49:55 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/cdp/cfg_int_cdp.h,v $
 *------------------------------------------------------------------
 * Interface commands for CDP.
 * 
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_cdp.h,v $
 * Revision 3.2  1995/11/17  08:49:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:05:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:51:40  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/02  01:47:20  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:04:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] cdp enable
 *
 */
EOLS	(cdp_int_enable_eol, cdp_intfc_command, CDP_ENABLE);
KEYWORD	(cdp_int_enable, cdp_int_enable_eol, no_alt,
	 "enable", "Enable CDP on interface", PRIV_CONF|PRIV_SUBIF);

/*********************************************************************/
KEYWORD	(cdp_int, cdp_int_enable, NONE,
	 "cdp", "CDP interface subcommands", PRIV_CONF|PRIV_SUBIF);

ASSERT  (cdp_int_subif, cdp_int, ALTERNATE,
	 main_if_or_vc_or_vlan(csb->interface));

#undef	ALTERNATE
#define	ALTERNATE	cdp_int_subif
