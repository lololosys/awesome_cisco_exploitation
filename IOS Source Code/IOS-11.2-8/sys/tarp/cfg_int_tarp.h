/* $Id: cfg_int_tarp.h,v 3.2 1995/11/17 18:44:24 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/tarp/cfg_int_tarp.h,v $
 *------------------------------------------------------------------
 * TARP (Target ID address Resolution Protocol)
 *
 * October 1995, Arun Sastry
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_tarp.h,v $
 * Revision 3.2  1995/11/17  18:44:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:32:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/***************************************************************
 * [no] tarp propagate
 *
 */
EOLS	(tarp_int_propagate_eol, tarp_intfc_command, TARP_PROPAGATE_CMD);
KEYWORD	(tarp_int_propagate, tarp_int_propagate_eol, no_alt,
	 "propagate", "Enable propagation of TARP PDU's out on this interface",
	 PRIV_CONF|PRIV_SUBIF);

/***************************************************************
 * [no] tarp enable
 *
 */
EOLS	(tarp_int_enable_eol, tarp_intfc_command, TARP_ENABLE);
KEYWORD	(tarp_int_enable, tarp_int_enable_eol, tarp_int_propagate,
	 "enable", "Enable TARP on interface", PRIV_CONF|PRIV_SUBIF);

/*********************************************************************/
KEYWORD	(tarp_int, tarp_int_enable, NONE,
	 "tarp", "TARP interface subcommands", PRIV_CONF|PRIV_SUBIF);

ASSERT  (tarp_int_subif, tarp_int, ALTERNATE, main_if_or_vc(csb->interface));

#undef	ALTERNATE
#define	ALTERNATE	tarp_int_subif
