/* $Id: cfg_routemap_set_policy.h,v 3.3.52.1 1996/05/21 09:53:12 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_routemap_set_policy.h,v $
 *------------------------------------------------------------------
 * cfg_routemap_set_policy.h -- Policy specific route map clauses
 *
 * Mar 1995 Tony Li
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_routemap_set_policy.h,v $
 * Revision 3.3.52.1  1996/05/21  09:53:12  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3  1995/11/28  19:50:39  widmer
 * CSCdi44251:  Replace Ifelse(csb->in_help)/Eval with Func
 *
 * Revision 3.2  1995/11/17  17:31:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/21  18:16:03  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.2  1995/06/15  15:18:47  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  21:05:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * set default interface <interface> [<interface> ... <interface>]
 *
 * OBJ(idb, 1)  = default interface
 */

LINK_EXIT(routemap_set_policy_return, no_alt);

/*
 * set default interface <interface> ... <interface>
 */

pdecl(routemap_set_default_if);

FUNC 	(routemap_set_default_int_func,
	 routemap_set_default_if,
	 route_map_set_default_int_command);
INTERFACE (routemap_set_default_if, routemap_set_default_int_func,
	   routemap_set_policy_return, OBJ(idb,1), IFTYPE_ANYSUB);

KEYWORD (routemap_set_default_int, routemap_set_default_if, no_alt,
	 common_str_interface, "Default output interface", PRIV_CONF);

KEYWORD	(routemap_set_default, routemap_set_default_int, ALTERNATE,
	 "default", "Set default information", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	routemap_set_default
