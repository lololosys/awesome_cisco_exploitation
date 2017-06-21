/* $Id: cfg_router.h,v 3.2.60.2 1996/08/12 16:03:38 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router.h,v $
 *------------------------------------------------------------------
 * C F G _ R O U T E R . H
 *
 * Enter router subcommand mode
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_router.h,v $
 * Revision 3.2.60.2  1996/08/12  16:03:38  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.60.1  1996/04/15  21:17:43  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:31:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:46  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:12:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * [no] router { bgp <AS> | 
 *		 egp <AS> |
 *		 hello |
 *		 igrp <AS> |
 *		 igrp2 <AS> |
 *		 rip |
 *		 ospf <ID> |
 *		 isis <tag> |
 *               ddrp |
 *		 iso-igrp <tag> }
 *
 * router_name.name = protocol name
 * router_name.as = AS number for IP protocols that use it
 * router_name.tag = ISO tag for ISO protocols
 *
 */

EOLNS	(config_router_eol, router_command);

ROUTER	(config_router_name, config_router_eol, no_alt);

/************************************************************************/
KEYWORD_ID(config_router, config_router_name, NONE,
	   leave_subcommand, TRUE,
	   "router", "Enable a routing process", PRIV_CONF);

TEST_BOOLEAN(config_router_test, config_router, NONE, ALTERNATE,
	     router_enable);

#undef	ALTERNATE
#define	ALTERNATE	config_router_test
