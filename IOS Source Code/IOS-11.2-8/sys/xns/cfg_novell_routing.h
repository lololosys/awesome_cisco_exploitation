/* $Id: cfg_novell_routing.h,v 3.4.6.4 1996/08/16 00:14:10 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/cfg_novell_routing.h,v $
 *------------------------------------------------------------------
 * C F G _ N O V E L L _ R O U T I N G . H
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_novell_routing.h,v $
 * Revision 3.4.6.4  1996/08/16  00:14:10  hampton
 * Change parser comments to reflect ipx command usage.  [CSCdi66208]
 * Branch: California_branch
 *
 * Revision 3.4.6.3  1996/08/12  16:09:59  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4.6.2  1996/05/30  23:47:57  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.4.6.1  1996/04/09  22:32:08  john
 * CSCdi52606:  IPX static routes can cause error reading from NETWORK on
 *              client. Allow configurable metrics.
 * Branch: California_branch
 *
 * Revision 3.4  1996/03/05  23:11:56  sluong
 * CSCdi50715:  NLSP route aggregation conformance (phase 2)
 *
 * Revision 3.3  1996/02/01  23:35:09  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  19:21:15  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:38:14  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/06/14  18:12:10  john
 * CSCdi35779:  IPX static routes to LAN interfaces should not be a
 *              parser option
 *
 * Revision 2.2  1995/06/11  17:28:01  john
 * CSCdi35588:  IPX Statics on unnumbered interfaces dont allow
 * subinterfaces
 *
 * Revision 2.1  1995/06/07  23:27:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(cfg_novell_routing_extend_here, no_alt);

/**********************************************************************
 * ipx sap <hex> <string> <novaddr> <hex> <num 1-15>
 * no ipx sap <hex> <string>
 */
EOLS    (conf_novell_sap_eol, novell_command, NOVELL_SAP);
NUMBER  (conf_novell_sap_hop, conf_novell_sap_eol, no_alt,
         OBJ(int,3), 1, 254, "Hop count");
HEXNUM  (conf_novell_sap_socket, conf_novell_sap_hop, no_alt,
         OBJ(int,2), "Socket number");
NOVADDR (conf_novell_sap_addr, conf_novell_sap_socket, no_alt,
         OBJ(paddr,1), "IPX address");
NOPREFIX(conf_novell_sap_noprefix, conf_novell_sap_addr, conf_novell_sap_eol);
STRING  (conf_novell_sap_service, conf_novell_sap_noprefix, no_alt,
         OBJ(string,1), "Sap service name");
HEXDIGIT(conf_novell_sap_type, conf_novell_sap_service, no_alt,
         OBJ(int,1), 0, 0xFFFF, "Sap type");
NVGENS  (conf_novell_sap_nvgen, conf_novell_sap_type,
	 novell_command, NOVELL_SAP);
KEYWORD (conf_novell_sap, conf_novell_sap_nvgen, cfg_novell_routing_extend_here,
         "sap", "Set an IPX static SAP table entry", PRIV_CONF);

/**********************************************************************
 *  ipx router igrp <as>
 *  no ipx router igrp <as>
 *  ipx router rip
 *  no ipx router rip
 *
 * Control global IPX IGRP and RIP routing processs
 */
EOLNS	(cr_novell_router_eol, novell_router_command);
NOVELL_ROUTER(cr_novell_router_router, cr_novell_router_eol, no_alt);
KEYWORD_ID_MM(cr_novell_router, cr_novell_router_router, conf_novell_sap,
	      leave_subcommand, TRUE,
	      "router", "Control IPX routing", PRIV_CONF, 6);



/**********************************************************************
 * ipx route <net> [<mask>] (<net.address> | <IPXWAN idb> | internal) 
 *          [<floating-static>] 
 * no ipx route <net> [(<net.address> | <IPXWAN idb> | internal)] 
 * (int,2) != 0 if "floating" specified 
 * (int,3) != 0 if interface specified 
 * (int,4) = mask if mask is entered
 * (int,5) is set to 1 if mask is entered
 */

EOLS	(cr_novell_route_eol, novell_command, NOVELL_ROUTE);
KEYWORD_ID (cr_novell_route_float, cr_novell_route_eol, cr_novell_route_eol,
	OBJ(int,2), 1, "floating-static",
	"Permit static route to be overwritten by dynamic information",
	PRIV_CONF);
NUMBER  (cr_novell_route_hop, cr_novell_route_float, no_alt,
         OBJ(int,7), 1, 254, "Hop/Area count");
NUMBER  (cr_novell_route_tick, cr_novell_route_hop, cr_novell_route_float,
         OBJ(int,6), 1, 0xfffe, "ticks/delay");
SET       (cr_novell_route_idb_yes, cr_novell_route_tick, OBJ(int,3), 1); 
INTERFACE (cr_novell_route_idb, cr_novell_route_idb_yes, no_alt, 
	   OBJ(idb, 1),~IFTYPE_IEEE); 
NOVADDR   (cr_novell_route_host, cr_novell_route_tick, cr_novell_route_idb, 
	   OBJ(paddr,1), "Address of forwarding host"); 
SET       (cr_novell_route_set3, cr_novell_route_host, OBJ(int,3), 0); 
SET       (cr_novell_route_set6, cr_novell_route_set3, OBJ(int,6),
           NOVELL_DEFAULT_CONNECTED_TICKS);
SET       (cr_novell_route_set7, cr_novell_route_set6, OBJ(int,7),
           NOVELL_DEFAULT_CONNECTED_HOPS);
NOPREFIX  (cr_novell_noroute, cr_novell_route_set7, cr_novell_route_eol); 
KEYWORD_ID_MM(cr_novell_route_ripdef, cr_novell_noroute, no_alt,
	      OBJ(int,1), NOV_RIP_DEFAULT_ROUTE,
	      "default", "Default route", PRIV_CONF, 5);
SET       (cr_novell_route_mask_yes, cr_novell_noroute, OBJ(int,5), 1); 
HEXDIGIT (cr_novell_route_mask, cr_novell_route_mask_yes, 
	  cr_novell_route_host, OBJ(int,4), 0x00000000, 0xffffffff, 
	  "Network mask");
HEXDIGIT (cr_novell_route_net, cr_novell_route_mask, cr_novell_route_ripdef,
	  OBJ(int,1), 0, NOV_MAX_NET,
	  "Destination network");
NVGENS	(cr_novell_route_nvgen, cr_novell_route_net, novell_command,
	 NOVELL_ROUTE);
KEYWORD	(cr_novell_route, cr_novell_route_nvgen, cr_novell_router,
	 "route", "Set an IPX static routing table entry", PRIV_CONF);

/**********************************************************************
 * Must match at least 3 chars to distinguish 'no' from 'novell'.
 */
NOVELL_IPX_KEYWORD (cr_novell, cr_novell_route, ALTERNATE,
		    NULL, PRIV_CONF | PRIV_DUPLICATE);

#undef	ALTERNATE
#define	ALTERNATE	cr_novell
