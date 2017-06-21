/* $Id: cfg_rsrb.h,v 3.4.28.2 1996/05/21 10:05:19 thille Exp $
 * $Source: /release/112/cvs/Xsys/srt/cfg_rsrb.h,v $
 *------------------------------------------------------------------

 * C F G _ R S R B . H   non pure srb commands
 *
 * Nov 1994, Jacob Teplitsky
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_rsrb.h,v $
 * Revision 3.4.28.2  1996/05/21  10:05:19  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4.28.1  1996/03/23  22:49:07  ppearce
 * SR/TLB fastswitching feature
 * Branch: California_branch
 *
 * Revision 3.4  1996/01/18  02:15:19  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.3  1995/12/07  17:45:33  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.2  1995/11/17  18:53:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:57:59  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/21  18:17:23  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:13:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * source-bridge keepalive <seconds>
 * no source-bridge keepalive
 */
EOLS	(srb_keepalive_eol, srb_command, SRB_REMPR_KEEP);

NUMBER	(srb_keepalive_seconds, srb_keepalive_eol, srb_keepalive_eol,
	 OBJ(int,1), 10, 300, "RSRB remote peer keepalive interval (default 30 seconds)");

NVGENS	(srb_keepalive_nvgen, srb_keepalive_seconds, srb_command,
              SRB_REMPR_KEEP);

/* Set OBJ(int,1) to 30 for the default period. */
KEYWORD_ID (srb_keepalive, srb_keepalive_nvgen, ALTERNATE,
         OBJ(int,1), 30, "keepalive", 
         "Enable RSRB remote peer keepalive", PRIV_CONF);


/**********************************************************************
 * [no] source-bridge passthrough <ring>
 */
PARAMS (srb_passthru, srb_keepalive, "passthrough",
	     OBJ(int,1), 1, SRB_MAX_RING_NUM,
	     srb_command, SRB_LACK_PASSTHRU,
	     "Do not local-ack frames to the specified ring",
	     "Token-ring number", PRIV_CONF);

/**********************************************************************
 * source-bridge lack-connection-timeout <seconds>
 * no source-bridge lack-connection-timeout [<seconds>]
 */
PARAMS_KEYONLY (srb_lackcon, srb_passthru,
		"lack-connection-timeout", OBJ(int,1), 1, -1L,
		srb_command, SRB_LACK_CON_TIMEOUT,
		"Local ACK connection timeout", "Seconds", PRIV_CONF);

/**********************************************************************
 * source-bridge tcp-queue-max <queue-size>
 * no source-bridge tcp-queue-max [<queue-size>]
 */
PARAMS_KEYONLY (srb_tcpqmax, srb_lackcon,
		"tcp-queue-max", OBJ(int,1), 1, 4096,
		srb_command, SRB_TCPQMAX,
		"Maximum TCP queue size for SRB", "Maximum queue size",
		PRIV_CONF);

/**********************************************************************
 * source-bridge connection-timeout <seconds>
 * no source-bridge connection-timeout [<seconds>]
 */
PARAMS_KEYONLY (srb_con, srb_tcpqmax,
		"connection-timeout", OBJ(int,1), 1, -1L,
		srb_command, SRB_CONTMO,
		"Connection timeout", "Seconds", PRIV_CONF);

/**********************************************************************
 *  source-bridge cos-enable
 */
EOLS	(srb_cosenable_eol, srb_command, SRB_COS);
KEYWORD	(srb_cosenable, srb_cosenable_eol, srb_con,
	 "cos-enable", "Prioritize traffic by Class of Service", PRIV_CONF);

/**********************************************************************
 *  source-bridge proxy-netbios-only
 */
EOLS	(srb_proxynetbios_eol, srb_command, SRB_PROXY_NETBIOS);
KEYWORD	(srb_proxynetbios, srb_proxynetbios_eol, srb_cosenable,
	 "proxy-netbios-only", "Proxy explorers only for NetBIOS", PRIV_CONF);

/**********************************************************************
 *  source-bridge qllc-local-ack
 */
EOLS	(srb_qllclack_eol, srb_command, SRB_QLLC_LAK);
KEYWORD	(srb_qllclack, srb_qllclack_eol, srb_proxynetbios,
	 "qllc-local-ack", "Locally terminate QLLC sessions", PRIV_CONF);

/**********************************************************************
 *  source-bridge sdllc-local-ack
 */
EOLS	(srb_sdllclack_eol, srb_command, SRB_SDLLC_LAK);
KEYWORD	(srb_sdllclack, srb_sdllclack_eol, srb_qllclack,
	 "sdllc-local-ack", "Locally terminate SDLC sessions", PRIV_CONF);

/**********************************************************************
 * source-bridge transparent <ring-group> <ring> <bridge> <bridge-group>
 *			[ 90-compatible | standard | cisco ] [enable-name-cache]
 *
 * no source-bridge transparent <ring-group>
 *
 * [no] source-bridge transparent <ring-group> fastswitch
 *
 * OBJ(int,1) = <ring-group>
 * OBJ(int,2) = <pseudo-ring>
 * OBJ(int,3) = <virtual-bridge>
 * OBJ(int,4) = <bridge-group>
 *
 */
EOLS	(srb_trans_eol, srb_command, SRB_TRANSPARENT);
pdecl( srb_trans_opts);

KEYWORD_ID(srb_trans_name_cache, srb_trans_opts, no_alt,
	   OBJ(int,6), TRUE,
	   "enable-name-cache", "Enable Netbios name cache", PRIV_CONF);

KEYWORD_ID(srb_trans_cisco, srb_trans_opts, srb_trans_name_cache,
	   OBJ(int,5), SRB_TRANSPARENT_CISCO,
	   "cisco", "Cisco specific OUI", PRIV_CONF);

KEYWORD_ID(srb_trans_standard, srb_trans_opts, srb_trans_cisco,
	   OBJ(int,5), SRB_TRANSPARENT_STANDARD,
	   "standard", "Standard OUI", PRIV_CONF);

KEYWORD_ID(srb_trans_90compat, srb_trans_opts, srb_trans_standard,
	   OBJ(int,5), SRB_TRANSPARENT_90COMPAT,
	   "90-compatible", "Cisco release 9.0 compatible OUI", PRIV_CONF);

NOP	(srb_trans_opts, srb_trans_90compat, srb_trans_eol);

NUMBER	(srb_trans_bridgegrp, srb_trans_opts, no_alt,
	 OBJ(int,4), 1, SPAN_MAXINDEX - 1, 
         "Transparent bridge group attached to the virtual ring");

NUMBER	(srb_trans_bridge, srb_trans_bridgegrp, no_alt,
	 OBJ(int,3), 1, SRB_MAX_BRIDGE_NUM, 
         "Bridge number to the transparent bridge");

NUMBER	(srb_trans_ring, srb_trans_bridge, no_alt,
	 OBJ(int,2), 1, SRB_MAX_RING_NUM, 
	 "Pseudo-ring number of the transparent bridge");

KEYWORD_ID (srb_trans_fast, srb_trans_eol, srb_trans_ring,
          OBJ(int,2), -1, "fastswitch",
          "Enable/disable fastswitching on sr/tlb bridge", PRIV_CONF);

NUMBER	(srb_trans_rg, srb_trans_fast, no_alt,
	 OBJ(int,1), 1, SRB_MAX_RING_NUM,
	 "Source-route ring group attached to the transparent bridge");

NVGENS	(srb_trans_nvgen, srb_trans_rg, srb_command, SRB_TRANSPARENT);

KEYWORD	(srb_transparent, srb_trans_nvgen, srb_sdllclack,
	 "transparent",
	 "Bridging between a transparent and a source-route bridge",
	 PRIV_CONF);

/**********************************************************************
 * [no] source-bridge remote-peer <ring-group> 
 *	{ interface <name> [<mac-addr>] | tcp <ipaddr> | fst <ipaddr> }
 *	[lf <size>] [version <num>] [local-ack] [backup-group <group>]
 *	[priority] [rnr-disable] [tcp-receive-window <num>]
 *
 * The following two options are not to be included yet as per jpbion.
 * See srb.c for more details.
 *	[host-netbios-out <netbios-list-name>]
 *	[bytes-netbios-out <netbios-bytes-list-name>]
 *
 * OBJ(int,1) = <ring-group>
 * OBJ(int,2) = SRB_REMPR_TCP | SRB_REMPR_FST | SRB_REMPR_INT | SRB_REMPR_FTCP
 * OBJ(int,3) = <size>
 * OBJ(int,4) = TRUE if "local-ack"
 * OBJ(int,5) = TRUE if "version"
 * OBJ(int,6) = <num>
 * OBJ(int,7) = TRUE if "backup-group"
 * OBJ(int,8) = <group>
 * OBJ(int,9) = TRUE (non-zero) if <name> is a LAN interface
 * OBJ(int,10) = TRUE if "host-netbios-out"
 * OBJ(string,1) = <netbios-list-name>
 * OBJ(int,11) = TRUE if "bytes-netbios-out"
 * OBJ(string,2) = <netbios-bytes-list-name>
 * OBJ(int,12) = TRUE if "priority"
 * OBJ(int,13) = TRUE if "rnr-disable"
 * OBJ(int,14) = dlci number
 * (*OBJ(paddr,1)) = <ipaddr>
 * (*OBJ(hwaddr,1)) = <mac-addr> if <name> is a LAN interface
 * OBJ(idb,1) = <name>
 * OBJ(int,15) = TCP receive-window size
 *
 */

EOLS	(srb_rpeer_eol, srb_command, SRB_REMPR);
pdecl( srb_rpeer_opts);

/* The below options loop back to srb_rpeer_opts so that they can
 * be input in any order.  If an option is specified multiple times,
 * the last value is used.
 */

/*
 * TCP-only keywords
 */

/* rnr-disable */
KEYWORD_ID (srb_rpeer_rnr_disable, srb_rpeer_opts, NONE,
	    OBJ(int,13), TRUE,
	    "rnr-disable", "Enable RNR-disable feature", PRIV_CONF);

/* priority */
KEYWORD_ID (srb_rpeer_priority, srb_rpeer_opts, srb_rpeer_rnr_disable,
	    OBJ(int,12), TRUE,
	    "priority", "Enable prioritization features", PRIV_CONF);

/*
 * Skip past TCP-only keywords if not using TCP encapsulation.
 */
TESTVAR	(srb_rpeer_tcponly_test, srb_rpeer_priority, NONE,
	 NONE, NONE, no_alt, OBJ(int,2), SRB_REMPR_TCP);

/*
 * TCP- and FTCP-only keywords
 */

/* tcp-receive-window <num> */
NUMBER (srb_rpeer_tcp_window_num, srb_rpeer_opts, no_alt,
	OBJ(int,15), RSRB_TCP_WINDOW_SIZE, MAXTCPWINDOW,
	"TCP receive window size (bytes)");
KEYWORD (srb_rpeer_tcp_window, srb_rpeer_tcp_window_num, srb_rpeer_tcponly_test,
	 "tcp-receive-window", "TCP receive window", PRIV_CONF);

/* local-ack */
KEYWORD_ID(srb_rpeer_lack, srb_rpeer_opts, srb_rpeer_tcp_window,
	   OBJ(int,4), 1,
	   "local-ack",
	   "Locally administer and acknowledge sessions"
	   " destined for specified remote peer",
	   PRIV_CONF);

/*
 * Skip past TCP- and FTCP-only keywords if not using TCP encapsulations.
 */
IFELSE(srb_rpeer_tcptest, srb_rpeer_lack, no_alt,
       ((GETOBJ(int,2) == SRB_REMPR_TCP) ||
	(GETOBJ(int,2) == SRB_REMPR_FTCP)));

/* bytes-netbios-out <netbios-bytes-list-name> */
STRING	(srb_rpeer_bytesnetbios_name, srb_rpeer_opts, no_alt,
	 OBJ(string,2), "Netbios bytes access list name");
KEYWORD_ID(srb_rpeer_bytesnetbios, srb_rpeer_bytesnetbios_name, 
	   srb_rpeer_tcptest,
	   OBJ(int,11), TRUE,
	   "bytes-netbios-out", "", PRIV_CONF);

/* host-netbios-out <netbios-list-name> */
STRING	(srb_rpeer_hostnetbios_name, srb_rpeer_opts, no_alt,
	 OBJ(string,1), "Netbios host access list name");
KEYWORD_ID(srb_rpeer_hostnetbios, srb_rpeer_hostnetbios_name,
	   srb_rpeer_bytesnetbios,
	   OBJ(int,10), TRUE,
	   "host-netbios-out", "", PRIV_CONF);

/* backup-group <group> */
NUMBER	(srb_rpeer_backup_group_num, srb_rpeer_opts, no_alt,
	 OBJ(int,8), 1, BACKUP_GROUP_COUNT - 1, "Backup Group number");
KEYWORD_ID(srb_rpeer_backup_group, srb_rpeer_backup_group_num,
	   srb_rpeer_hostnetbios,
	   OBJ(int,7), TRUE,
	   "backup-group", "",
	   PRIV_CONF);

/* version <num> */
NUMBER	(srb_rpeer_version_num, srb_rpeer_opts, no_alt,
	 OBJ(int,6), RSRB_MIN_VERSION, RSRB_VERSION, "Version number");
KEYWORD_ID(srb_rpeer_version, srb_rpeer_version_num, srb_rpeer_backup_group,
	   OBJ(int,5), TRUE,
	   "version", "Force remote peer's RSRB protocol version number",
	   PRIV_CONF|PRIV_HIDDEN);

/* lf <size> */
KEYWORD_ID(srb_rpeer_lf_17800, srb_rpeer_opts, no_alt,
	   OBJ(int,3), RC_LF17800,
	   "17800", "17800 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(srb_rpeer_lf_11454, srb_rpeer_opts, srb_rpeer_lf_17800,
	   OBJ(int,3), RC_LF11454,
	   "11454", "11454 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(srb_rpeer_lf_11407, srb_rpeer_opts, srb_rpeer_lf_11454,
	   OBJ(int,3), RC_LF11407,
	   "11407", "11407 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(srb_rpeer_lf_8144, srb_rpeer_opts, srb_rpeer_lf_11407,
	   OBJ(int,3), RC_LF8144,
	   "8144", "8144 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(srb_rpeer_lf_4472, srb_rpeer_opts, srb_rpeer_lf_8144,
	   OBJ(int,3), RC_LF4472,
	   "4472", "4472 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(srb_rpeer_lf_2052, srb_rpeer_opts, srb_rpeer_lf_4472,
	   OBJ(int,3), RC_LF2052,
	   "2052", "2052 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(srb_rpeer_lf_1500, srb_rpeer_opts, srb_rpeer_lf_2052,
	   OBJ(int,3), RC_LF1500,
	   "1500", "1500 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(srb_rpeer_lf_1470, srb_rpeer_opts, srb_rpeer_lf_1500,
	   OBJ(int,3), RC_LF1470,
	   "1470", "1470 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(srb_rpeer_lf_516, srb_rpeer_opts, srb_rpeer_lf_1470,
	   OBJ(int,3), RC_LF516,
	   "516", "516 byte maximum frame size", PRIV_CONF);
KEYWORD (srb_rpeer_lf, srb_rpeer_lf_516, srb_rpeer_version,
	 "lf", "Largest frame size used by peers in the ring-group",
	 PRIV_CONF);

/* Initialize <size> variable.  NOP is for looping to get options in
 * any order.
 */
NOP	(srb_rpeer_opts, srb_rpeer_lf, srb_rpeer_eol);
SET	(srb_rpeer_set, srb_rpeer_opts, OBJ(int,3), -1);
NOPREFIX(srb_rpeer_noprefix, srb_rpeer_set, srb_rpeer_eol);

/* interface <name> [<mac-addr>] */
ENADDR	(srb_rpeer_int_addr, srb_rpeer_noprefix, no_alt,
	 OBJ(hwaddr,1), "Ethernet Address");
TESTVAR	(srb_rpeer_int_test, srb_rpeer_noprefix, srb_rpeer_int_addr,
	 NONE, NONE, NONE, OBJ(int,9), FALSE);
EVAL	(srb_rpeer_int_eval, srb_rpeer_int_test,
	 (GETOBJ(int,9) = GETOBJ(idb,1)->hwptr->status & IDB_IEEEMASK));
NUMBER	(srb_rpeer_dlci, srb_rpeer_noprefix, no_alt, 
	 OBJ(int,14), 16, 1007, "Frame-Relay DLCI Number");
TESTVAR (srb_rpeer_fr_test, srb_rpeer_noprefix, srb_rpeer_dlci,
	 NONE, NONE, NONE, OBJ(int,9), FALSE);
EVAL    (srb_rpeer_fr_eval, srb_rpeer_fr_test,
         (GETOBJ(int,9) = GETOBJ(idb,1)->hwptr->status & IDB_SERIAL));


FORWARD_INTERFACE(srb_rpeer_fr_ser, srb_rpeer_fr_eval, no_alt, OBJ(idb,1), IFTYPE_SERIAL | IFTYPE_SUBIF);
KEYWORD_ID(srb_rpeer_fr_int, srb_rpeer_fr_ser, no_alt,
	   OBJ(int,2), SRB_REMPR_FR, common_str_interface,
	   "Serial interface over which to run TR traffic", PRIV_CONF);
KEYWORD_ID(srb_rpeer_fr, srb_rpeer_fr_int, no_alt,
	   OBJ(int,2), SRB_REMPR_FR, "frame-relay",
	   "Frame Relay Encapsulation over which to run TR traffic", PRIV_CONF);

INTERFACE(srb_rpeer_int, srb_rpeer_int_eval, no_alt, OBJ(idb,1),
	  IFTYPE_HWIDB     );
KEYWORD_ID(srb_rpeer_interface, srb_rpeer_int, srb_rpeer_fr,
	   OBJ(int,2), SRB_REMPR_INT, common_str_interface,
	   "Serial interface over which to run TR traffic", PRIV_CONF);

/* fst <ipaddr> */
IPADDR	(srb_rpeer_fst_ipaddr, srb_rpeer_noprefix, no_alt,
	 OBJ(paddr,1), "Remote FST peer's IP address");
KEYWORD_ID(srb_rpeer_fst, srb_rpeer_fst_ipaddr, srb_rpeer_interface,
	   OBJ(int,2), SRB_REMPR_FST,
	   "fst", "Fast Sequence Transport", PRIV_CONF);


/* ftcp <ipaddr> */
IPADDR	(srb_rpeer_ftcp_ipaddr, srb_rpeer_noprefix, no_alt,
	 OBJ(paddr,1), "Remote cisco peer router IP address");
KEYWORD_ID(srb_rpeer_ftcp, srb_rpeer_ftcp_ipaddr, srb_rpeer_fst,
	   OBJ(int,2), SRB_REMPR_FTCP,
	   "ftcp", "Use FTCP encapsulation", PRIV_CONF);

/* tcp <ipaddr> */
IPADDR	(srb_rpeer_ipaddr, srb_rpeer_noprefix, no_alt,
	 OBJ(paddr,1), "Remote cisco peer router IP address");
KEYWORD_ID(srb_rpeer_tcp, srb_rpeer_ipaddr, srb_rpeer_ftcp,
	   OBJ(int,2), SRB_REMPR_TCP,
	   "tcp", "Use TCP encapsulation", PRIV_CONF);

/* remote-peer <ring-group> */
NUMBER	(srb_rpeer_ring, srb_rpeer_tcp, no_alt, 
	 OBJ(int,1), 1, SRB_MAX_RING_NUM, "Ring Group");
NVGENS	(srb_rpeer_nvgen, srb_rpeer_ring, srb_command, SRB_REMPR);
KEYWORD	(srb_rpeer, srb_rpeer_nvgen, srb_transparent,
	 "remote-peer", 
	 "Specify a remote peer for remote SRB", PRIV_CONF);

/**********************************************************************
 * source-bridge sap-80d5 <num>
 */
EOLS	(srb_sap80d5_eol, srb_command, SRB_SAP_80D5);
HEXNUM (srb_sap80d5_sap, srb_sap80d5_eol, no_alt,
	 OBJ(int,1), "SAP identifier (in hex)");
KEYWORD	(srb_sap80D5, srb_sap80d5_sap, srb_rpeer,
	 "sap-80d5", "Translate specified SAP frames to 80d5 frames",
	 PRIV_CONF);

/**********************************************************************
 * source-bridge enable-80d5
 */
EOLS	(srb_enable80d5_eol, srb_command, SRB_ENABLE_80D5);
KEYWORD	(srb_enable80D5, srb_enable80d5_eol, srb_sap80D5,
	 "enable-80d5", "Convert TR LLC2 frames to Ethernet type 80d5 frames",
	 PRIV_CONF);

/**********************************************************************
 * source-bridge fst-peername <addr>
 */
EOLS	(srb_fstpeername_eol, srb_command, SRB_FST_PEERNAME);
IPADDR	(srb_fstpeername_addr, srb_fstpeername_eol, no_alt,
	 OBJ(paddr,1), "FST local interface address");
NOPREFIX(srb_fstpeername_noprefix, srb_fstpeername_addr, srb_fstpeername_eol);
NVGENS	(srb_fstpeername_nvgen, srb_fstpeername_noprefix,
	 srb_command, SRB_FST_PEERNAME);
KEYWORD	(srb_fstpeername, srb_fstpeername_nvgen, srb_enable80D5,
	 "fst-peername", "Fast Sequence Transport local interface address",
	 PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	srb_fstpeername
