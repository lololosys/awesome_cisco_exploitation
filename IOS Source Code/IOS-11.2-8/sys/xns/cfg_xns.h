/* $Id: cfg_xns.h,v 3.2 1995/11/17 19:21:19 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/cfg_xns.h,v $
 *------------------------------------------------------------------
 * C F G _ X N S . H
 *
 * XNS global configuration commands.
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_xns.h,v $
 * Revision 3.2  1995/11/17  19:21:19  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:42:58  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:14:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*********************************************************************
 * xns maximum-paths [<paths>]
 * no xns maximum-paths [<paths>]
 */
PARAMS_KEYONLY	(conf_xns_maxpath, no_alt,
	 "maximum-paths", OBJ(int,1), 1, 512, 
	 xns_command, XNS_PATHS,
	 "Forward XNS packets over multiple paths", 
	 "Number of paths", PRIV_CONF);

/**********************************************************************
 * xns forward-protocol <type>
 * no xns forward-protocol <type>
 */

PARAMS (conf_xns_fwdproto, conf_xns_maxpath,
	     "forward-protocol", OBJ(int,1), 0, -1L,
	     xns_command, XNS_FORWARD,
	     "Forward selected broadcast protocols", "Protocol type number", PRIV_CONF);

/*********************************************************************
 * xns ub-emulation
 * no xns ub-emulation
 */

EOLS	(conf_xns_ubemulation_eol, xns_command, XNS_UBEMULATION);
KEYWORD	(conf_xns_ubemulation, conf_xns_ubemulation_eol, conf_xns_fwdproto,
	 "ub-emulation", "Enable XNS Ungermann-Bass emulation", PRIV_CONF);

/*********************************************************************
 * xns rip-delay
 * no xns rip-delay
 */

EOLS	(conf_xns_ripdelay_eol, xns_command, XNS_RIP_DELAY);
KEYWORD	(conf_xns_ripdelay, conf_xns_ripdelay_eol, conf_xns_ubemulation,
	 "rip-delay", "Enable delay between RIP packets", PRIV_CONF);

/*********************************************************************
 * xns routing [<addr>]
 * no xns routing [<addr>]
 *
 * OBJ(int,1) = TRUE if <host>, FALSE if not
 * (*OBJ(paddr,1)) = <host>
 */

EOLS	(conf_xns_routing_eol, xns_command, XNS_ROUTING);
SET	(conf_xns_routing_gotaddr, conf_xns_routing_eol, OBJ(int,1), TRUE);
XNSHOST	(conf_xns_routing_addr, conf_xns_routing_gotaddr, conf_xns_routing_eol,
	 OBJ(paddr,1), "The XNS address of this router");
NOPREFIX (conf_xns_routing_noprefix, conf_xns_routing_addr, conf_xns_routing_eol);
NVGENS	(conf_xns_routing_nvgen, conf_xns_routing_noprefix,
	 xns_command, XNS_ROUTING);
KEYWORD (conf_xns_address, conf_xns_routing_nvgen, conf_xns_ripdelay,
	 "address", "", PRIV_CONF | PRIV_HIDDEN | PRIV_NONVGEN);
KEYWORD	(conf_xns_routing, conf_xns_routing_nvgen, conf_xns_address,
	 "routing", "Enable XNS routing", PRIV_CONF);



/**********************************************************************/
KEYWORD	(configure_xns_global, conf_xns_routing, ALTERNATE,
	 "xns", "XNS global configuration commands", PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE configure_xns_global
