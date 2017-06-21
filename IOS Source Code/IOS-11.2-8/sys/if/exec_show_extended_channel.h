/* $Id: exec_show_extended_channel.h,v 3.3.20.4 1996/08/16 06:14:20 bchan Exp $
 * $Source: /release/112/cvs/Xsys/if/exec_show_extended_channel.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ E X T E N D E D _ C H A N N E L . H
 *
 * July 1994, Stig Thormodsrud
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_extended_channel.h,v $
 * Revision 3.3.20.4  1996/08/16  06:14:20  bchan
 * CSCdi66094:  add show lu history for tn3270 server
 * Branch: California_branch
 *
 * Revision 3.3.20.3  1996/06/20  07:21:26  mkirk
 * CSCdi59893: show ext x/2 tn ... commands had wrong help text, also min
 * and max
 * LOCADDR constants were wrong (0-256 instead of 1-255)
 * Branch: California_branch
 *
 * Revision 3.3.20.2  1996/05/21  09:49:45  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3.20.1  1996/05/17  11:22:27  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.6.1  1996/03/28  11:29:08  bchan
 * CSCdi52724:  merge tn3280 server code
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3  1996/02/01  23:33:36  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  09:27:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:49:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/13  19:33:42  motto
 * CSCdi36478:  Show interface statistical info is missing for Channel
 * interface
 *
 * Revision 2.3  1995/07/18  21:48:24  motto
 * Fixed filtering support for LLC2 show command
 * Update MIBs to current level of CIP support
 * Cleaned up CSNA show command support
 * CSCdi35714:  Some show commands for CIP-SNA are not working
 *
 * Revision 2.2  1995/07/13  19:42:25  motto
 * Fixed all channel tcp/ip show commands to be consistent and work
 * properly.
 * CSCdi36252:  show ex ch <slot>/<port> tcp-connections needs parms to be
 * optional
 *
 * Revision 2.1  1995/07/02  10:53:25  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.3  1995/06/25  21:27:35  kmitchel
 * CSCdi35580:  CIP-SNA Virtual Lan: verbiage change - virtual to internal
 *
 * Revision 2.2  1995/06/21  20:27:03  motto
 * Fix 'show extended channel x/2 <llc2|csna|connection-map|
 * max-llc2-sessions> ....' commands for SNA support on the CIP card.
 * CSCdi35714:  Some show commands for CIP-SNA are not working
 *
 * Revision 2.1  1995/06/07  22:23:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#undef  CHANNEL_NEXT_IN_CHAIN
#define CHANNEL_NEXT_IN_CHAIN no_alt

/* =========================== TN3270 =========================== */
/******************************************************************
 * show extended channel [slot/port] 
 *  tn3270-server [pu <pu-name> [lu <lu-number>]]
 *                [client-ip-address <ip-address>]
 *                [dlur]
 *                [dlurlink <link-name>]
 *
 * OBJ(idb,1) = <interface>
 * OBJ(int,1) =  TN3270S_SHOW_SVR
 *               TN3270S_SHOW_PU_THIS
 *               TN3270S_SHOW_PU_THIS_LU_THIS
 *               TN3270S_SHOW_CLIENT_THIS
 *               TN3270S_SHOW_DLUR
 *               TN3270S_SHOW_DLURLINK_THIS
 * ---------------------------------------------------
 * TN3270S_SHOW_PU_THIS
 * OBJ(string,1) = <pu-name>  
 *
 * TN3270S_SHOW_PU_THIS_LU_THIS
 * OBJ(string,1) = <pu-name>  
 * OBJ(int,2)    = <lu-number>
 *
 * ---------------------------------------------------
 * TN3270S_SHOW_DLURLINK_THIS
 * OBJ(string,1) = <link-name>  
 *
 * ---------------------------------------------------
 * TN3270S_SHOW_CLIENT_THIS
 * OBJ(paddr,1)  = <ip-address>
 * OBJ(int,2) = TN3270S_SHOW_CLIENT_THIS_INSESSION
 *              TN3270S_SHOW_CLIENT_THIS_DISCONNECTED
 *              TN3270S_SHOW_CLIENT_THIS_PENDING
 * ---------------------------------------------------
 */  
EOLS	(show_tn3270s_eol, tn3270s_show_ext_channel_command, 
         0);
#undef   NEXT_TN3270S_SHOW
#define  NEXT_TN3270S_SHOW show_tn3270s_eol

STRING     (show_tn3270s_dlurlink_name, show_tn3270s_eol,
	    no_alt,
	    OBJ(string,1), "name of DLUR link");
KEYWORD_ID_MM (show_tn3270s_dlurlink, show_tn3270s_dlurlink_name,
	       NEXT_TN3270S_SHOW,
	       OBJ(int,1), TN3270S_SHOW_DLURLINK_THIS,  
	       TN3270S_SHOW_KEY_DLURLINK,
	       "status of a DLUR link", PRIV_USER,
	       5);
#undef   NEXT_TN3270S_SHOW
#define  NEXT_TN3270S_SHOW show_tn3270s_dlurlink

KEYWORD_ID (show_tn3270s_dlur, show_tn3270s_eol, 
	    NEXT_TN3270S_SHOW,
	    OBJ(int,1), TN3270S_SHOW_DLUR,  
	    TN3270S_SHOW_KEY_DLUR,
	    "status of DLUR", PRIV_USER);
#undef   NEXT_TN3270S_SHOW
#define  NEXT_TN3270S_SHOW show_tn3270s_dlur
KEYWORD_ID (show_tn3270s_client_pending, show_tn3270s_eol, 
 	    show_tn3270s_eol,
 	    OBJ(int,2), TN3270S_SHOW_CLIENT_THIS_PENDING,
 	    TN3270S_SHOW_KEY_CLIENT_PENDING,
 	    "show clients in pending state", PRIV_USER);
 
KEYWORD_ID (show_tn3270s_client_disconnect, show_tn3270s_eol, 
 	    show_tn3270s_client_pending,
 	    OBJ(int,2), TN3270S_SHOW_CLIENT_THIS_DISCONNECTED,
 	    TN3270S_SHOW_KEY_CLIENT_DISCONNECTED,
 	    "show clients in disconnected state", PRIV_USER);
 
KEYWORD_ID (show_tn3270s_client_insession, show_tn3270s_eol, 
 	    show_tn3270s_client_disconnect,
 	    OBJ(int,2), TN3270S_SHOW_CLIENT_THIS_INSESSION ,
 	    TN3270S_SHOW_KEY_CLIENT_INSESSION,
 	    "show clients in active session state", PRIV_USER);
 
SET	(show_tn3270s_client_setstate, show_tn3270s_client_insession,
 	 OBJ(int,2), TN3270S_SHOW_CLIENT_THIS_ALL);

IPADDR     (show_tn3270s_client_ipaddr, show_tn3270s_client_setstate,
	    no_alt,
	    OBJ(paddr,1),
	    "IP address of client");
KEYWORD_ID (show_tn3270s_client, show_tn3270s_client_ipaddr, 
	    NEXT_TN3270S_SHOW,
	    OBJ(int,1), TN3270S_SHOW_CLIENT_THIS,
	    TN3270S_SHOW_KEY_CLIENT,
	    "status of clients with given IP address", PRIV_USER);
#undef   NEXT_TN3270S_SHOW
#define  NEXT_TN3270S_SHOW show_tn3270s_client

KEYWORD_ID (show_tn3270s_lu_history, show_tn3270s_eol, 
	    show_tn3270s_eol,
	    OBJ(int,3), TRUE,
            "history", 
	    "history of an LU", PRIV_USER);
NUMBER     (show_tn3270s_lu_number, show_tn3270s_lu_history,
	    no_alt,
	    OBJ(int,2), TN3270S_LU_NUM_MIN, TN3270S_LU_NUM_MAX,
	    "LOCADDR of LU");
KEYWORD_ID (show_tn3270s_lu, show_tn3270s_lu_number, 
	    show_tn3270s_eol,
	    OBJ(int,1), TN3270S_SHOW_PU_THIS_LU_THIS,  
	    TN3270S_SHOW_KEY_LU,
	    "status of an LU", PRIV_USER);

STRING     (show_tn3270s_pu_name, show_tn3270s_lu,
	    no_alt,
	    OBJ(string,1), "name of PU");
KEYWORD_ID (show_tn3270s_pu, show_tn3270s_pu_name, 
	    NEXT_TN3270S_SHOW,
	    OBJ(int,1), TN3270S_SHOW_PU_THIS,  
	    TN3270S_SHOW_KEY_PU,
	    "status of a PU", PRIV_USER);
#undef   NEXT_TN3270S_SHOW
#define  NEXT_TN3270S_SHOW show_tn3270s_pu

KEYWORD_ID (show_tn3270s, NEXT_TN3270S_SHOW, 
	    CHANNEL_NEXT_IN_CHAIN,
	    OBJ(int,1), TN3270S_SHOW_SVR,  
	    TN3270S_SHOW_KEY_SVR,
	    "TN3270 server status", PRIV_USER);
#undef  CHANNEL_NEXT_IN_CHAIN
#define CHANNEL_NEXT_IN_CHAIN show_tn3270s
/* =========================== TN3270 =========================== */
/******************************************************************
 * show extended channel [slot/port] llc2 {admin | oper | stats} \
 *                                    [lmac [*|lsap [*|rmac [rsap]]]]
 *
 * OBJ(hwaddr,1) = lmac
 * OBJ(hwaddr,2) = rmac
 * OBJ(int,4) = lsap
 * OBJ(int,5) = rsap
 *
 * OBJ(int,6) = (FALSE) | TRUE  wild card rmac
 * OBJ(int,3) = (FALSE) | TRUE  wild card lsap
 * OBJ(int,2) = filter index
 * OBJ(int,1) = CSNA_SHO_EXT_TYPE_ADMN |
 *              CSNA_SHO_EXT_TYPE_OPER |
 *              CSNA_SHO_EXT_TYPE_STAT |
 *              CSNA_SHO_EXT_TYPE_DFLT
 * OBJ(idb,1) = <interface>
 */
EOLS	(sho_ext_ch_llc2_eol, show_ext_channel_llc2_command,
	 CHANNEL_CSNA_LLC2);
SET	(sho_ext_ch_llc2_set_filter_rsap, sho_ext_ch_llc2_eol,
	 OBJ(int,2), LLC2_SHOW_EXT_CHANNEL_FILTER_RSAP);
OPT_HEXNUM(sho_ext_ch_llc2_rsap, sho_ext_ch_llc2_set_filter_rsap,
	   sho_ext_ch_llc2_eol,
           OBJ(int,5), "remote SAP");
SET	(sho_ext_ch_llc2_set_filter_rmac, sho_ext_ch_llc2_rsap,
	 OBJ(int,2), LLC2_SHOW_EXT_CHANNEL_FILTER_RMAC);
ENADDR	(sho_ext_ch_llc2_rmac, sho_ext_ch_llc2_set_filter_rmac,
	 sho_ext_ch_llc2_eol,
	 OBJ(hwaddr,2), "remote MAC address");
KEYWORD_ID(sho_ext_ch_llc2_rmac_wc, sho_ext_ch_llc2_set_filter_rmac,
           sho_ext_ch_llc2_rmac,
	   OBJ(int,6), TRUE,
	   "*", "all LLC2 connections for specified local MAC/SAP",
	   PRIV_CONF);
SET	(sho_ext_ch_llc2_set_filter_lsap, sho_ext_ch_llc2_rmac_wc,
	 OBJ(int,2), LLC2_SHOW_EXT_CHANNEL_FILTER_LSAP);
OPT_HEXNUM(sho_ext_ch_llc2_lsap, sho_ext_ch_llc2_set_filter_lsap,
	   sho_ext_ch_llc2_eol,
	   OBJ(int,4), "local SAP");
KEYWORD_ID(sho_ext_ch_llc2_lsap_wc, sho_ext_ch_llc2_set_filter_lsap,
           sho_ext_ch_llc2_lsap, OBJ(int,3), TRUE,
	   "*", "all local SAPs on specified local MAC", PRIV_CONF);
SET	(sho_ext_ch_llc2_set_filter_lmac, sho_ext_ch_llc2_lsap_wc,
	 OBJ(int,2), LLC2_SHOW_EXT_CHANNEL_FILTER_LMAC);
SET	(sho_ext_ch_llc2_set_filter_lmac_admin, sho_ext_ch_llc2_eol,
	 OBJ(int,2), LLC2_SHOW_EXT_CHANNEL_FILTER_LMAC);
ENADDR	(sho_ext_ch_llc2_lmac, sho_ext_ch_llc2_set_filter_lmac,
	 sho_ext_ch_llc2_eol,
	 OBJ(hwaddr,1), "local MAC address");
ENADDR	(sho_ext_ch_llc2_lmac_admin, sho_ext_ch_llc2_set_filter_lmac_admin,
	 sho_ext_ch_llc2_eol, OBJ(hwaddr,1), "local MAC address");
KEYWORD_ID(sho_ext_ch_llc2_stat, sho_ext_ch_llc2_lmac,
	   sho_ext_ch_llc2_lmac_admin,
	   OBJ(int,1), CSNA_SHO_EXT_TYPE_STAT,
	   "stats", "show statistics", PRIV_CONF);
KEYWORD_ID(sho_ext_ch_llc2_oper, sho_ext_ch_llc2_lmac, sho_ext_ch_llc2_stat,
	   OBJ(int,1), CSNA_SHO_EXT_TYPE_OPER,
	   "oper", "show operational values", PRIV_CONF);
KEYWORD_ID(sho_ext_ch_llc2_admin, sho_ext_ch_llc2_lmac_admin,
	   sho_ext_ch_llc2_oper,
	   OBJ(int,1), CSNA_SHO_EXT_TYPE_ADMN,
	   "admin", "show configured values", PRIV_CONF);
SET	(sho_ext_ch_llc2_set_admin, sho_ext_ch_llc2_admin,
	 OBJ(int,1), CSNA_SHO_EXT_TYPE_DFLT);
SET	(sho_ext_ch_llc2_set_filter, sho_ext_ch_llc2_set_admin,
	 OBJ(int,2), LLC2_SHOW_EXT_CHANNEL_FILTER_NONE);
SET	(sho_ext_ch_llc2_set_lsap_wc_dflt, sho_ext_ch_llc2_set_filter,
	 OBJ(int,3), FALSE);
SET	(sho_ext_ch_llc2_set_rmac_wc_dflt, sho_ext_ch_llc2_set_lsap_wc_dflt,
	 OBJ(int,6), FALSE);
KEYWORD (sho_ext_ch_llc2, sho_ext_ch_llc2_set_rmac_wc_dflt,
	 CHANNEL_NEXT_IN_CHAIN,
	 "llc2", "show channel interface LLC information", PRIV_USER);
#undef  CHANNEL_NEXT_IN_CHAIN
#define CHANNEL_NEXT_IN_CHAIN sho_ext_ch_llc2

/******************************************************************
 * show extended channel [slot/port] connection-map <llc2|...>
 *
 * OBJ(int,1) = CSNA_CONN_MAP_LLC2 |
 *
 * OBJ(idb,1) = <interface>
 */
EOLS	(sho_ext_ch_csna_conn_map_eol, show_ext_channel_csna_conn_map_command,
         CHANNEL_CSNA_CONN_MAP);
KEYWORD_ID(sho_ext_ch_conn_map_type, sho_ext_ch_csna_conn_map_eol,
           no_alt, OBJ(int,1), CSNA_CONN_MAP_LLC2,
	   "llc2", "show connection map between llc2 and csna", PRIV_CONF);
KEYWORD (sho_ext_ch_csna_conn_map, sho_ext_ch_conn_map_type,
         CHANNEL_NEXT_IN_CHAIN,
	 "connection-map", "connection map between protocol and csna",
         PRIV_USER);
#undef  CHANNEL_NEXT_IN_CHAIN
#define CHANNEL_NEXT_IN_CHAIN sho_ext_ch_csna_conn_map

/******************************************************************
 * show extended channel [slot/port] max-llc2-sessions
 *
 * OBJ(idb,1) = <interface>
 */
EOLS	(sho_ext_ch_max_llc2_sess_eol, show_ext_channel_max_llc2_sess_command,
         CHANNEL_MAX_LLC2_SESS);
KEYWORD (sho_ext_ch_max_llc2_sess, sho_ext_ch_max_llc2_sess_eol,
         CHANNEL_NEXT_IN_CHAIN,
	 "max-llc2-sessions", "maximum llc2 session statistics", PRIV_USER);
#undef  CHANNEL_NEXT_IN_CHAIN
#define CHANNEL_NEXT_IN_CHAIN sho_ext_ch_max_llc2_sess

/******************************************************************
 * show extended channel [slot/port] lan [lan-type [lan-id [adapno]]]
 *
 * OBJ(int,3) = adapno   | CSNA_VLAN_MAX_VADAPTER_NUM+1
 * OBJ(int,2) = lan-id   | CSNA_VLAN_MAX_LANID_NUM+1
 * OBJ(int,1) = lan-type | CSNA_VLAN_MAXTYPE
 * OBJ(idb,1) = <interface>
 */
EOLS	(sho_ext_ch_vlan_int_eol, show_ext_channel_vlan_command,
	 CHANNEL_CSNA_VLAN_INT);
EOLS	(sho_ext_ch_vlan_eol, show_ext_channel_vlan_command,
	 CHANNEL_CSNA_VLAN);
NUMBER	(sho_ext_ch_vlan_adapno, sho_ext_ch_vlan_eol, sho_ext_ch_vlan_eol,
	 OBJ(int,3), 0, CSNA_VLAN_MAX_VADAPTER_NUM, "Adapter Number");
KEYWORD	(sho_ext_ch_vlan_int, sho_ext_ch_vlan_int_eol, sho_ext_ch_vlan_adapno,
	 common_str_interface, "CIP internal LAN interface", PRIV_USER|PRIV_HIDDEN);
NUMBER	(sho_ext_ch_vlan_id, sho_ext_ch_vlan_int, sho_ext_ch_vlan_eol,
	 OBJ(int,2), 0, CSNA_VLAN_MAX_LANID_NUM, "Lan Id");
KEYWORD_ID(sho_ext_ch_vlan_type_fddi, sho_ext_ch_vlan_id, sho_ext_ch_vlan_eol,
	   OBJ(int,1), CSNA_VLAN_FDDI,
	   "fddi", "FDDI", PRIV_CONF);
KEYWORD_ID(sho_ext_ch_vlan_type_token, sho_ext_ch_vlan_id,
	   sho_ext_ch_vlan_type_fddi,
	   OBJ(int,1), CSNA_VLAN_TOKEN, "tokenring",
	   "TokenRing", PRIV_CONF);
KEYWORD_ID(sho_ext_ch_vlan_type_ether, sho_ext_ch_vlan_id,
	   sho_ext_ch_vlan_type_token,
	   OBJ(int,1), CSNA_VLAN_ETHER, "ethernet",
	   "Ethernet", PRIV_CONF);
SET	(sho_ext_ch_vlan_set_3, sho_ext_ch_vlan_type_ether,
	 OBJ(int,3), CSNA_VLAN_MAX_VADAPTER_NUM+1);
SET	(sho_ext_ch_vlan_set_2, sho_ext_ch_vlan_set_3,
	 OBJ(int,2), CSNA_VLAN_MAX_LANID_NUM+1);
SET	(sho_ext_ch_vlan_set_1, sho_ext_ch_vlan_set_2,
	 OBJ(int,1), CSNA_VLAN_MAXTYPE);
KEYWORD	(sho_ext_ch_vlan, sho_ext_ch_vlan_set_1, CHANNEL_NEXT_IN_CHAIN,
	 "lan", "CIP internal Lans", PRIV_USER);
#undef  CHANNEL_NEXT_IN_CHAIN
#define CHANNEL_NEXT_IN_CHAIN sho_ext_ch_vlan

/******************************************************************
 * show extended channel [slot/port] csna {admin | oper | stats} [path [dev]]
 *
 * OBJ(int,4) = <dev>
 * OBJ(int,3) = <path>
 *
 * OBJ(int,2) = CHANNEL_STATS_PATH    |
 *              CHANNEL_STATS_DEVICE  |
 *              CHANNEL_STATS_DEFAULT
 *
 * OBJ(int,1) = CSNA_SHO_EXT_TYPE_ADMN |
 *              CSNA_SHO_EXT_TYPE_OPER |
 *              CSNA_SHO_EXT_TYPE_STAT |
 *              CSNA_SHO_EXT_TYPE_DFLT
 * OBJ(idb,1) = <interface>
 */
EOLS	(sho_ext_ch_csna_eol, show_ext_channel_csna_command,
	 CHANNEL_CSNA_DEVICE);
SET	(sho_ext_ch_csna_set_device, sho_ext_ch_csna_eol,
	 OBJ(int,2), CHANNEL_STATS_DEVICE);
OPT_HEXNUM (sho_ext_ch_csna_device, sho_ext_ch_csna_set_device,
	    sho_ext_ch_csna_eol, OBJ(int,4), "Device address");
SET	(sho_ext_ch_csna_set_path, sho_ext_ch_csna_device,
	 OBJ(int,2), CHANNEL_STATS_PATH);
OPT_HEXNUM (sho_ext_ch_csna_path, sho_ext_ch_csna_set_path,
	    sho_ext_ch_csna_eol, OBJ(int,3), "Hex path identifier");
KEYWORD_ID(sho_ext_ch_csna_stat, sho_ext_ch_csna_path, sho_ext_ch_csna_path,
	   OBJ(int,1), CSNA_SHO_EXT_TYPE_STAT,
	   "stats", "show statistics", PRIV_CONF);
KEYWORD_ID(sho_ext_ch_csna_oper, sho_ext_ch_csna_path, sho_ext_ch_csna_stat,
	   OBJ(int,1), CSNA_SHO_EXT_TYPE_OPER,
	   "oper", "show operational values", PRIV_CONF);
KEYWORD_ID(sho_ext_ch_csna_admn, sho_ext_ch_csna_path, sho_ext_ch_csna_oper,
	   OBJ(int,1), CSNA_SHO_EXT_TYPE_ADMN,
	   "admin", "show configured values", PRIV_CONF);
SET	(sho_ext_ch_csna_set_def, sho_ext_ch_csna_admn,
	 OBJ(int,2), CHANNEL_STATS_DEFAULT);
SET	(sho_ext_ch_csna_set_1, sho_ext_ch_csna_set_def,
	 OBJ(int,1), CSNA_SHO_EXT_TYPE_DFLT);
KEYWORD (sho_ext_ch_csna, sho_ext_ch_csna_set_1, CHANNEL_NEXT_IN_CHAIN,
	 "csna", "CSNA device", PRIV_USER);
#undef  CHANNEL_NEXT_IN_CHAIN
#define CHANNEL_NEXT_IN_CHAIN sho_ext_ch_csna

/******************************************************************
 * show extended channel [slot/port] cta-test-tool
 *
 * OBJ(idb,1) = <interface>
 */

EOLS	(show_extended_channel_cta_tt_eol, show_extended_channel_commands,
         CHANNEL_CTA_TT);
KEYWORD (show_extended_channel_cta_tt, show_extended_channel_cta_tt_eol,
         CHANNEL_NEXT_IN_CHAIN, "cta-test-tool", "CIP-SNA CTA Test Tool",
         PRIV_USER|PRIV_HIDDEN);

/******************************************************************
 * show extended channel <slot/port> udp-listeners [ipaddr]
 *
 * OBJ(idb,1)   = <interface>
 * OBJ(int,1)   = <index level>
 * OBJ(paddr,1) = <Local ip address>
 */
EOLS	(show_extended_channel_udplisten_eol,
	 show_extended_channel_tcpip_command,
	 CHANNEL_SHOW_UDP_LISTENERS);
SET	(show_extended_channel_udplisten_set_def,
	 show_extended_channel_udplisten_eol,
	 OBJ(int,1), CHANNEL_SHOW_IPADDR_MATCH);
IPADDR	(show_extended_channel_udplisten_ipaddr,
	 show_extended_channel_udplisten_set_def,
	 show_extended_channel_udplisten_eol, OBJ(paddr,1),
	 "CIP IP address for UDP Stack");
KEYWORD_ID(show_extended_channel_udplisten,
	   show_extended_channel_udplisten_ipaddr,
	   show_extended_channel_cta_tt,
	   OBJ(int,1), CHANNEL_SHOW_IPADDR_ALL,
	   "udp-listeners", "CIP UDP Listeners Statistics", PRIV_USER);

/******************************************************************
 * show extended channel <slot/port> udp-stack [ipaddr]
 *
 * OBJ(idb,1)   = <interface>
 * OBJ(int,1)   = <index level specified>
 * OBJ(paddr,1) = <ip address>
 */
EOLS	(show_extended_channel_udp_eol, show_extended_channel_tcpip_command,
	 CHANNEL_SHOW_UDP);
SET	(show_extended_channel_udp_set_ipaddr,
	 show_extended_channel_udp_eol,
	 OBJ(int,1), CHANNEL_SHOW_IPADDR_MATCH);
IPADDR	(show_extended_channel_udp_ipaddr,
	 show_extended_channel_udp_set_ipaddr,
	 show_extended_channel_udp_eol,
	 OBJ(paddr,1), "CIP IP address for UDP Stack");
KEYWORD_ID(show_extended_channel_udp,
	   show_extended_channel_udp_ipaddr,
	   show_extended_channel_udplisten,
	   OBJ(int,1), CHANNEL_SHOW_IPADDR_ALL,
	   "udp-stack", "CIP UDP Statistics", PRIV_USER);

/******************************************************************
 * show extended channel <slot/port> icmp-stack [ipaddr]
 *
 * OBJ(idb,1)   = <interface>
 * OBJ(int,1)   = <index level specified>
 * OBJ(paddr,1) = <ip address>
 */
EOLS	(show_extended_channel_icmp_eol, show_extended_channel_tcpip_command,
	 CHANNEL_SHOW_ICMP);
SET	(show_extended_channel_icmp_set_ipaddr,
	 show_extended_channel_icmp_eol,
	 OBJ(int,1), CHANNEL_SHOW_IPADDR_MATCH);
IPADDR	(show_extended_channel_icmp_ipaddr,
	 show_extended_channel_icmp_set_ipaddr,
	 show_extended_channel_icmp_eol,
	 OBJ(paddr,1), "CIP IP address for ICMP Stack");
KEYWORD_ID(show_extended_channel_icmp, show_extended_channel_icmp_ipaddr,
	   show_extended_channel_udp,
	   OBJ(int,1), CHANNEL_SHOW_IPADDR_ALL,
	   "icmp-stack", "CIP ICMP Statistics", PRIV_USER);

/******************************************************************
 * show extended channel <slot/port> tcp-connections [local-ipaddr \
 * [*|local-port [remote-ipadd [remote-port]]]] [summary|detailed]
 *
 * OBJ(idb,1)   = <interface>
 * OBJ(int,1)   = <index level>
 * OBJ(paddr,1) = <local ip address>
 * OBJ(paddr,2) = <remote ip address>
 * OBJ(int,2)   = <local port>
 * OBJ(int,3)   = <remote port>
 * OBJ(int,4)   = <display type>
 * OBJ(int,5)   = <wild card local port>
 */
EOLS	(show_extended_channel_tcpconns_eol,
	 show_extended_channel_tcpconns_command, 0);
KEYWORD_ID (show_extended_channel_tcpconns_detail,
            show_extended_channel_tcpconns_eol,
            show_extended_channel_tcpconns_eol,
            OBJ(int,4), CHANNEL_SHOW_TCPCONNS_DETAIL,
	    "detail", "detailed tcp-connections stats", PRIV_CONF);
KEYWORD_ID (show_extended_channel_tcpconns_summary,
            show_extended_channel_tcpconns_eol,
            show_extended_channel_tcpconns_detail,
            OBJ(int,4), CHANNEL_SHOW_TCPCONNS_SUMMARY,
	    "summary", "summarized tcp-connections stats", PRIV_CONF);
SET	(show_extended_channel_tcpconns_set_type,
	 show_extended_channel_tcpconns_summary,
	 OBJ(int,4), CHANNEL_SHOW_TCPCONNS_SUMMARY);
SET	(show_extended_channel_tcpconns_set_fport,
	 show_extended_channel_tcpconns_set_type,
	 OBJ(int,1), CHANNEL_SHOW_TCPCONNS_FOREIGN_PORT);
NUMBER	(show_extended_channel_tcpconns_fport,
	 show_extended_channel_tcpconns_set_fport,
	 show_extended_channel_tcpconns_set_type,
	 OBJ(int,3), 0, 65535, "CIP TCP Remote Port Number");
SET	(show_extended_channel_tcpconns_set_fipaddr,
	 show_extended_channel_tcpconns_fport,
	 OBJ(int,1), CHANNEL_SHOW_TCPCONNS_FOREIGN_ADDR);
IPADDR	(show_extended_channel_tcpconns_fipaddr,
	 show_extended_channel_tcpconns_set_fipaddr,
	 show_extended_channel_tcpconns_set_type,
	 OBJ(paddr,2), "CIP Remote IP address");
SET	(show_extended_channel_tcpconns_set_lport,
	 show_extended_channel_tcpconns_fipaddr,
	 OBJ(int,1), CHANNEL_SHOW_TCPCONNS_LOCAL_PORT);
NUMBER	(show_extended_channel_tcpconns_lport,
	 show_extended_channel_tcpconns_set_lport,
	 show_extended_channel_tcpconns_set_type,
	 OBJ(int,2), 0, 65535, "CIP TCP Local Port Number");
/*
 * Bypass setting the CHANNEL_SHOW_TCPCONNS_LOCAL_PORT
 * if "*" is selected. This is because if they don't specify any
 * more search indices, then we can treat this command as if they didn't
 * enter "*" or wild card.
 */
KEYWORD_ID (show_extended_channel_tcpconns_lportwc,
            show_extended_channel_tcpconns_fipaddr,
            show_extended_channel_tcpconns_lport,
            OBJ(int,5), 1,
	    "*", "All local ports on local IP address", PRIV_CONF);
SET	(show_extended_channel_tcpconns_set_lipaddr,
	 show_extended_channel_tcpconns_lportwc,
	 OBJ(int,1), CHANNEL_SHOW_TCPCONNS_LOCAL_ADDR);
IPADDR	(show_extended_channel_tcpconns_lipaddr,
	 show_extended_channel_tcpconns_set_lipaddr,
	 show_extended_channel_tcpconns_set_type,
	 OBJ(paddr,1), "CIP IP Address for TCP Connections");
SET	(show_extended_channel_tcpconns_set_lportwc,
	 show_extended_channel_tcpconns_lipaddr,
	 OBJ(int,5), 0);
KEYWORD_ID(show_extended_channel_tcpconns,
	   show_extended_channel_tcpconns_set_lportwc,
	   show_extended_channel_icmp,
	   OBJ(int,1), CHANNEL_SHOW_TCPCONNS_DEFAULT,
	   "tcp-connections", "CIP TCP Connections Statistics", PRIV_USER);

/******************************************************************
 * show extended channel <slot/port> tcp-stack [ipaddr]
 *
 * OBJ(idb,1)   = <interface>
 * OBJ(int,1)   = <index level specified>
 * OBJ(paddr,1) = <ip address>
 */
EOLS	(show_extended_channel_tcp_eol, show_extended_channel_tcpip_command,
	 CHANNEL_SHOW_TCP);
SET	(show_extended_channel_tcp_set_ipaddr, show_extended_channel_tcp_eol,
	 OBJ(int,1), CHANNEL_SHOW_IPADDR_MATCH);
IPADDR	(show_extended_channel_tcp_ipaddr,
	 show_extended_channel_tcp_set_ipaddr,
	 show_extended_channel_tcp_eol,
	 OBJ(paddr,1),  "CIP IP address for TCP Stack");
KEYWORD_ID(show_extended_channel_tcp, show_extended_channel_tcp_ipaddr,
	   show_extended_channel_tcpconns,
	   OBJ(int,1), CHANNEL_SHOW_IPADDR_ALL,
	   "tcp-stack", "CIP TCP Stack Statistics", PRIV_USER);

/******************************************************************
 * show extended channel <slot/port> ip-stack [ipaddr]
 *
 * OBJ(idb,1)   = <interface>
 * OBJ(int,1)   = <index level specified>
 * OBJ(paddr,1) = <ip address>
 */
EOLS	(show_extended_channel_ip_eol, show_extended_channel_tcpip_command,
	 CHANNEL_SHOW_IP);
SET	(show_extended_channel_ip_set_ipaddr, show_extended_channel_ip_eol,
	 OBJ(int,1), CHANNEL_SHOW_IPADDR_MATCH);
IPADDR	(show_extended_channel_ip_ipaddr, show_extended_channel_ip_set_ipaddr,
	 show_extended_channel_ip_eol,
	 OBJ(paddr,1), "CIP IP address for IP Stack");
KEYWORD_ID(show_extended_channel_ip, show_extended_channel_ip_ipaddr,
	   show_extended_channel_tcp,
	   OBJ(int,1), CHANNEL_SHOW_IPADDR_ALL,
	   "ip-stack", "CIP IP Statistics", PRIV_USER);

/******************************************************************
 * show extended channel [slot/port] vc
 *
 * OBJ(idb,1) = <interface>
 */
EOLS	(show_extended_channel_vc_eol, show_extended_channel_commands,
	 CHANNEL_VC);
KEYWORD (show_extended_channel_vc, show_extended_channel_vc_eol,
         show_extended_channel_ip,
         "vc", "Virtual Circuits", PRIV_USER|PRIV_HIDDEN);

/******************************************************************
 * show extended channel [slot/port] statistics [path] [device]
 *
 * OBJ(idb,1) = <interface>
 */
EOLS	(show_extended_channel_stats_eol, show_extended_channel_commands,
	 CHANNEL_STATISTICS);
SET	(show_extended_channel_stats_set_device,
	 show_extended_channel_stats_eol,
	 OBJ(int,1), CHANNEL_STATS_DEVICE);
OPT_HEXNUM(show_extended_channel_stats_device,
	   show_extended_channel_stats_set_device,
	   show_extended_channel_stats_eol,
	   OBJ(int,3), "Device address");
SET	(show_extended_channel_stats_set_path,
	 show_extended_channel_stats_device,
	 OBJ(int,1), CHANNEL_STATS_PATH);
OPT_HEXNUM(show_extended_channel_stats_path,
	   show_extended_channel_stats_set_path,
	   show_extended_channel_stats_eol,
	   OBJ(int,2), "Hex path identifier");
KEYWORD_ID(show_extended_channel_stats,
	   show_extended_channel_stats_path,
	   show_extended_channel_vc,
	   OBJ(int,1), CHANNEL_STATS_DEFAULT,
	   "statistics", "Channel statistics", PRIV_USER);

/***********************************************************************
 * show extended channel [slot/port] subchannel
 *
 */
EOLS	(show_extended_channel_eol, show_extended_channel_commands,
	 CHANNEL_SUBCHANNEL);
KEYWORD (show_extended_channel_sub, show_extended_channel_eol,
	 show_extended_channel_stats,
	 "subchannel", "Subchannel information", PRIV_USER);

INTERFACE(show_extended_channel, show_extended_channel_sub, ALTERNATE,
	  OBJ(idb,1), IFTYPE_CHANNEL);

#undef ALTERNATE
#define ALTERNATE	show_extended_channel
