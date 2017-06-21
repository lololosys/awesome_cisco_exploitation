/* $Id: cfg_router_net.h,v 3.2.60.1 1996/08/12 16:00:41 widmer Exp $
 * $Source: /release/112/cvs/Xsys/clns/cfg_router_net.h,v $
 *------------------------------------------------------------------
 * C F G _ R O U T E R _ N E T . H
 * 
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_router_net.h,v $
 * Revision 3.2.60.1  1996/08/12  16:00:41  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  08:51:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:07:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:59  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:12:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 * net <network-entity-title>
 * no net [<network-entity-title>]
 *
 * (*OBJ(paddr,1)) = <NSAP-address>
 */

EOLNS	(rtr_net_eol, clns_net_cmd);
NSAPADDR (rtr_net_net, rtr_net_eol, no_alt, OBJ(paddr,1), 
	  "Network entity title (NET)");
NVGENNS	(rtr_net_nvgen, rtr_net_net, clns_net_cmd);

KEYWORD	(router_net_kw, rtr_net_nvgen, NONE,
	 "net", "A Network Entity Title for this process (OSI only)", 
	 PRIV_CONF);

ASSERT	(router_net, router_net_kw, ALTERNATE,
	 ((clns_pdbtype *) csb->protocol)->family == PDB_OSI);

#undef	ALTERNATE
#define ALTERNATE	router_net
