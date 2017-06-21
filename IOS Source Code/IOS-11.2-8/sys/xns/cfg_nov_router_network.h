/* $Id: cfg_nov_router_network.h,v 3.3.20.3 1996/08/12 16:09:55 widmer Exp $
 * $Source: /release/112/cvs/Xsys/xns/cfg_nov_router_network.h,v $
 *------------------------------------------------------------------
 * C F G _ N O V _ R O U T E R _ N E T W O R K . H
 * 
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_nov_router_network.h,v $
 * Revision 3.3.20.3  1996/08/12  16:09:55  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.20.2  1996/08/02  07:32:32  akr
 * CSCdi64830:  Cant Input IPX Network Number
 * Branch: California_branch
 * Network 'a' conflicts with keyword "all"
 *
 * Revision 3.3.20.1  1996/05/03  02:16:00  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.3  1996/02/01  23:35:08  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  19:21:09  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:38:09  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  23:27:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 * Format:
 * [no] network <network>
 *
 * router subcommand
 */

EOLNS    (nov_network_eol, nov_network_command);
KEYWORD_ID_MM(nov_network_all, nov_network_eol, no_alt, OBJ(int,2), TRUE,
	   "all", "All IPX networks", PRIV_CONF, 2);
HEXDIGIT  (nov_network_net, nov_network_eol, nov_network_all, OBJ(int,1),
	   NOV_MIN_NET, NOV_MAX_NET, "IPX Network number");
NVGENNS	(nov_network_nvgen, nov_network_net, nov_network_command);
KEYWORD	(nov_network, nov_network_nvgen, NONE,
	 "network", "Enable routing on an IPX network", PRIV_CONF);
ASSERT	(nov_network_test, nov_network, router_novell_extend_here,
	 (((novpdbtype *)csb->protocol)->proctype != NET_NLSP));


#undef  ALTERNATE
#define ALTERNATE 	nov_network_test
