/* $Id: exec_show_frame-relay.h,v 3.2.62.3 1996/05/31 18:19:21 vrawat Exp $
 * $Source: /release/112/cvs/Xsys/wan/exec_show_frame-relay.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ F R A M E - R E L A Y . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_frame-relay.h,v $
 * Revision 3.2.62.3  1996/05/31  18:19:21  vrawat
 * CSCdi58846:  frame-relay debugging enhancements
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/05/21  10:09:31  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2.62.1  1996/04/25  23:23:10  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.2.64.1  1996/04/24  04:11:13  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.2  1995/11/17  18:02:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:09  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:23:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*****************************************************************
 * show frame-relay svc maplist <name>
 */
EOLS    (show_fr_svc_maplist_eol, show_frame_relay, SHOW_FR_SVC_MAPLIST);
STRING  (show_fr_svc_maplist_name, show_fr_svc_maplist_eol, no_alt,
	OBJ(string, 1), "Static map list name");
KEYWORD (sh_fr_svc_maplist, show_fr_svc_maplist_name, no_alt,
	"maplist", "SVCs belonging to a maplist", PRIV_USER);
KEYWORD (sh_fr_svc, sh_fr_svc_maplist, no_alt,
	"svc", "show frame relay SVC stuff", PRIV_USER);

/*****************************************************************
  * show frame-relay tcp/ip header compression statistics
   */
EOLS    (show_fr_tcpip_compr_eol, show_frame_relay, SHOW_FRAME_RELAY_COMP);
pdecl   (show_fr_tcpip_compr_opts);
INTERFACE (show_fr_tcpip_compr_int, show_fr_tcpip_compr_opts, no_alt,
	OBJ(idb,1), IFTYPE_SERIAL | IFTYPE_HSSI);
KEYWORD (show_fr_tcpip_compr_interface, show_fr_tcpip_compr_int, no_alt,
	common_str_interface, "", PRIV_USER);
NOP     (show_fr_tcpip_compr_opts, show_fr_tcpip_compr_interface,
	show_fr_tcpip_compr_eol);
KEYWORD (sh_fr_header_compr, show_fr_tcpip_compr_opts, no_alt,
	"header-compression",
	"show header compression statistics",
	PRIV_USER);
KEYWORD (sh_fr_tcpip, sh_fr_header_compr, no_alt,
	"tcp", "show frame relay TCP statistics", PRIV_USER);
KEYWORD (sh_fr_ip, sh_fr_tcpip, sh_fr_svc,
	"ip", "show frame relay IP statistics", PRIV_USER);

/*****************************************************************
 * show frame-relay route
 */
EOLS (show_fr_route_eol, show_frame_relay, SHOW_FRAME_RELAY_ROUTE);
KEYWORD (show_fr_route, show_fr_route_eol, sh_fr_ip,
	 "route", "show frame relay route", PRIV_USER);

/*****************************************************************
 * show frame-relay lmi [<interface>]
 */
EOLS (show_fr_lmi_eol, show_frame_relay, SHOW_FRAME_RELAY_LMI);
pdecl (show_fr_lmi_opts);
INTERFACE (show_fr_lmi_int, show_fr_lmi_opts, no_alt,
	   OBJ(idb,1), IFTYPE_SERIAL | IFTYPE_HSSI);
KEYWORD (show_fr_lmi_interface, show_fr_lmi_int, no_alt,
	 common_str_interface, "", PRIV_USER);
NOP (show_fr_lmi_opts, show_fr_lmi_interface, show_fr_lmi_eol);
KEYWORD (show_fr_lmi, show_fr_lmi_opts, show_fr_route,
	 "lmi", "show frame relay lmi statistics", PRIV_USER);

/*****************************************************************
 * show frame-relay lapf [<interface>]
 */
EOLS (show_fr_lapf_eol, show_frame_relay, SHOW_FRAME_RELAY_LAPF);
pdecl (show_fr_lapf_opts);
INTERFACE (show_fr_lapf_int, show_fr_lapf_opts, no_alt,
		OBJ(idb,1), IFTYPE_SERIAL | IFTYPE_HSSI);
KEYWORD (show_fr_lapf_interface, show_fr_lapf_int, no_alt,
		common_str_interface, "", PRIV_USER);
NOP (show_fr_lapf_opts, show_fr_lapf_interface, show_fr_lapf_eol);
KEYWORD (show_fr_lapf, show_fr_lapf_opts, show_fr_lmi,
	"lapf", "show frame relay lapf status/statistics", PRIV_USER);

/*****************************************************************
 * show frame-relay pvc [<interface>] [dlci]
 */
EOLS (show_fr_pvc_eol, show_frame_relay, SHOW_FRAME_RELAY_PVC);
pdecl (show_fr_pvc_opts);
NUMBER (show_fr_pvc_dlci, show_fr_pvc_eol, no_alt,
	OBJ(int,1), 16, 1022,"DLCI");
INTERFACE (show_fr_pvc_int, show_fr_pvc_opts, no_alt, 
	   OBJ(idb,1), IFTYPE_SERIAL | IFTYPE_HSSI | IFTYPE_TUNNEL | 
	   IFTYPE_DIALER | IFTYPE_BRI | IFTYPE_SUBIF);
KEYWORD (show_fr_pvc_interface, show_fr_pvc_int, show_fr_pvc_dlci,
	 common_str_interface, "", PRIV_USER);
NOP (show_fr_pvc_opts, show_fr_pvc_interface, show_fr_pvc_eol);
KEYWORD (show_fr_pvc, show_fr_pvc_opts, show_fr_lapf,
	 "pvc", "show frame relay pvc statistics", PRIV_USER);

/******************************************************************
 * show frame-relay map
 * show frame-relay traffic
 * 
 */

EOLS	(show_frame_traffic_eol, show_frame_relay, SHOW_FRAME_RELAY_TRAFFIC);
KEYWORD (show_frame_traffic, show_frame_traffic_eol, show_fr_pvc,
	 "traffic", "Frame-Relay protocol statistics", PRIV_USER);

EOLS	(show_frame_map_eol, show_frame_relay, SHOW_FRAME_RELAY_MAP);
KEYWORD (show_frame_map, show_frame_map_eol, show_frame_traffic,
	 "map", "Frame-Relay map table", PRIV_USER);


KEYWORD (show_frame, show_frame_map, ALTERNATE,
	 "frame-relay", "Frame-Relay information", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_frame
