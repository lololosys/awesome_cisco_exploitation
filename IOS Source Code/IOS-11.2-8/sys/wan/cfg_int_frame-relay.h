/* $Id: cfg_int_frame-relay.h,v 3.6.12.10 1996/09/04 18:25:14 liqin Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_frame-relay.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ F R A M E R E L A Y . H
 *
 * frame-relay specific commands
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_frame-relay.h,v $
 * Revision 3.6.12.10  1996/09/04  18:25:14  liqin
 * CSCdi66691:  frame-relay interface-dlci shouldnt have broadcast option
 * Branch: California_branch
 *
 * Revision 3.6.12.9  1996/07/03  01:57:17  chifei
 * CSCdi58333:  Some frame relay SVC commands are available in
 * non-enterprise images
 * Branch: California_branch
 * Modularized "frame svc" and "frame lapf" commands to go with FR SVC
 * subsystem; fixed/clean-up LAPF command format; consolidated DL
 * primitives define's.
 *
 * Revision 3.6.12.8  1996/06/19  14:20:50  fowler
 * CSCdi58333:  Some frame relay SVC commands are available in
 *              non-enterprise images
 * Branch: California_branch
 *
 * Revision 3.6.12.7  1996/05/29  06:34:57  fowler
 * CSCdi58766:  Link errors in 11.2 build
 * Branch: California_branch
 *
 * Revision 3.6.12.6  1996/05/21  10:09:25  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.12.5  1996/05/17  21:13:41  ccpoe
 * CSCdi57877:  Frame relay traffic shaping breaks modular images
 * Branch: California_branch
 *
 * Revision 3.6.12.4  1996/05/15  05:54:35  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.6.12.3  1996/05/11  06:16:18  speakman
 * CSCdi57085:  frame-relay interface-dlci command has invalid options on
 * mpt i/f
 * Branch: California_branch
 * Bit by the cAPitALIZatiON police ... compile first, then commit.
 *
 * Revision 3.6.12.2  1996/05/10  21:26:30  ccpoe
 * CSCdi57085:  frame-relay interface-dlci command has invalid options on
 * mpt i/f
 * Branch: California_branch
 *
 * Revision 3.6.12.1  1996/04/25  23:23:05  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.6.14.1  1996/04/24  04:11:11  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.6  1996/02/15  20:59:22  chifei
 * CSCdi48715:  Frame Relay prompts for RFC 1294 should be RFC1490
 *
 * Revision 3.5  1996/01/15  19:17:44  turadek
 * CSCdi46908:  cant configure point-to-point frame-relay payload
 * compression
 *
 * permit the command "frame-relay payload-compress packet-by-packet" to
 * be parsed on a frame-relay encapsulated subinterface.
 *
 * also fixes automatic enabling of "half-compression" (i.e.,
 * decompression of
 * incoming packets without compressing outgoing packets.
 *
 * also fixes a bug relating to enabling/disabling compression on multiple
 * DLCIs via the frame-relay map command
 *
 * also removes an error message when attempt is made to compress a packet
 * with refCount > 1
 *
 * Revision 3.4  1995/12/18  20:09:20  turadek
 * CSCdi45797:  cant enable tcp-ip header compression on frame relay VCs
 * re-enable parse-chain after the frame-relay payload compress command
 *
 * Revision 3.3  1995/11/21  20:44:00  turadek
 * CSCdi43841:  add frame relay payload compression feature
 * syncing code to 11.0
 *
 * Revision 3.2  1995/11/17  18:01:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/16  06:04:09  ccpoe
 * CSCdi38475:  Parser does not accept frame-relay de-group subcommand
 * during reload.
 *
 * Revision 2.1  1995/07/02  01:45:52  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.3  1995/06/21  18:16:45  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.2  1995/06/13  21:32:02  fowler
 * CSCdi35674:  Frame relay de-group command not saved when applied to
 *              subifs
 *
 * Revision 2.1  1995/06/07  22:05:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 *
 * ALWAYS ADD COMMANDS AT THE TOP OF THIS FILE
 * THE Alternate PART OF EVERY KEYWORD MUST POINT TO THE NEXT COMMAND
 * THIS IS FOR TREE BUILDING AND PARSING
 */
/*******************************************************************
 * frame-relay idle-timer [number]
 * [no] frame-relay idle-timer
 */
EOLS  (ci_fr_svc_it_eol, frame_relay_command, FR_SVC_IDLE_TIMER_SET);
NUMBER (ci_fr_svc_it_val, ci_fr_svc_it_eol, no_alt,
	OBJ(int,2), 0, 2000000, "Number of seconds of inactivity");
NUMBER (ci_fr_svc_it_id, ci_fr_svc_it_val, no_alt,
	OBJ(int,1), 0, 1024, "DLCI of SVC");
NVGENS (ci_fr_svc_it_nvgen, ci_fr_svc_it_id, frame_relay_command,
	FR_SVC_IDLE_TIMER_SET);
KEYWORD (ci_fr_svc_idle_timer, ci_fr_svc_it_nvgen, no_alt, "idle-timer",
	"frame relay svc idle timer", PRIV_CONF|PRIV_HIDDEN);

LINK_TRANS(ci_fr_lapf_extend_here, ci_fr_svc_idle_timer);

LINK_TRANS(ci_fr_svc_extend_here, ci_fr_lapf_extend_here);

/*************************************************************
 * frame-relay lmi-n393dce [<number>]
 * [no] frame-relay lmi-n393dce [<number>]
 */
PARAMS_KEYONLY (ci_FR_lmi_n393dce, ci_fr_svc_extend_here,
		"lmi-n393dce", OBJ(int,1), 1, 10,
		frame_relay_command, FR_LMI_N393DCE,
		"set LMI monitored event count", "event", PRIV_CONF);

/*************************************************************
 * frame-relay lmi-n392dce [<number>]
 * [no] frame-relay lmi-n392dce [<number>]
 */
PARAMS_KEYONLY (ci_FR_lmi_n392dce, ci_FR_lmi_n393dce,
  		"lmi-n392dce", OBJ(int,1), 1, 10,
		frame_relay_command, FR_LMI_N392DCE,
		"LMI error threshold", "event", PRIV_CONF);
  
/*************************************************************
 * frame-relay lmi-t392dce [<number>]
 * [no] frame-relay lmi-t392dce [<number>]
 */
PARAMS_KEYONLY (ci_FR_lmi_t392dce, ci_FR_lmi_n392dce,
                "lmi-t392dce", OBJ(int,1), 5, 30,
                frame_relay_command, FR_LMI_T392DCE,
                "set DCE polling verification timer", "Timer", PRIV_CONF);
 
/*************************************************************
 * frame-relay lmi-n393dte [<number>]
 * [no] frame-relay lmi-n393dte [<number>]
 */
PARAMS_KEYONLY (ci_FR_lmi_n393dte, ci_FR_lmi_t392dce,
                "lmi-n393dte", OBJ(int,1), 1, 10,
                frame_relay_command, FR_LMI_N393DTE,
                "set LMI monitored event count", "event", PRIV_CONF);
 
/*************************************************************
 * frame-relay lmi-n392dte [<number>]
 * [no] frame-relay lmi-n392dte [<number>]
 */
PARAMS_KEYONLY (ci_FR_lmi_n392dte, ci_FR_lmi_n393dte,
                "lmi-n392dte", OBJ(int,1), 1, 10,
                frame_relay_command, FR_LMI_N392DTE,
                "LMI error threshold", "event", PRIV_CONF);
 
/**************************************************************
 * frame-relay lmi-n391dte [<number>]
 * [no] frame-relay lmi-n39dte1 [<number>]
 */
PARAMS_KEYONLY (ci_FR_lmi_n391dte, ci_FR_lmi_n392dte,
  		"lmi-n391dte", OBJ(int,1), 1, 255,
		frame_relay_command, FR_LMI_N391DTE,
		"set full status polling counter ", "event", PRIV_CONF);

/**************************************************************
 * frame-relay route <in dlci> <out intf> <out dlci>
 * [no] frame-relay route <in dlci> <out intf> <out dlci>
 */
EOLS (ci_FR_route_eol, frame_relay_command, FRAME_RELAY_ROUTE);
NUMBER (ci_FR_route_outdlci, ci_FR_route_eol, no_alt,
	OBJ(int,2), 16, 1007, "output dlci to use when switching");
FORWARD_INTERFACE (ci_FR_route_outintf, ci_FR_route_outdlci, no_alt,
	   OBJ(idb,1), IFTYPE_SERIAL | IFTYPE_HSSI | IFTYPE_TUNNEL);
KEYWORD (ci_FR_interface, ci_FR_route_outintf, no_alt,
	 common_str_interface, "outgoing interface for pvc switching", PRIV_CONF);
NOPREFIX (ci_FR_route_noprefix, ci_FR_interface, ci_FR_route_eol);
NUMBER (ci_FR_route_indlci, ci_FR_route_noprefix, no_alt,
	OBJ(int,1), 16, 1007, "input dlci to be switched");
NVGENS (ci_FR_route_nvgen, ci_FR_route_indlci, frame_relay_command,
	FRAME_RELAY_ROUTE);
KEYWORD (ci_FR_route, ci_FR_route_nvgen, ci_FR_lmi_n391dte,
	 "route", "frame relay route for pvc switching", PRIV_CONF);

/**************************************************************
 * frame-relay intf-type [keyword]
 * [no] frame-relay intf-type [keyword]
 * OBJ(int,1) to 0 if dte
 * OBJ(int,1) to 1 if dce
 * OBJ(int,1) to 2 if nni
 */
EOLS	(ci_FR_intftype_eol, frame_relay_command, FRAME_RELAY_INTF);
KEYWORD_ID (ci_FR_nni, ci_FR_intftype_eol, no_alt,
	    OBJ(int,1), 2, "nni", "Configure a FR NNI", PRIV_CONF);
KEYWORD_ID (ci_FR_dce, ci_FR_intftype_eol, ci_FR_nni,
	    OBJ(int,1), 1, "dce", "Configure a FR DCE", PRIV_CONF);
KEYWORD_ID (ci_FR_dte, ci_FR_intftype_eol, ci_FR_dce,
	    OBJ(int,1), 0, "dte", "Configure a FR DTE", PRIV_CONF);
NVGENS  (ci_FR_intftype_nvgen, ci_FR_dte, frame_relay_command,
	 FRAME_RELAY_INTF);
KEYWORD	(ci_FR_intftype, ci_FR_intftype_nvgen, ci_FR_route,
	 "intf-type", "Configure a FR DTE/DCE/NNI interface", PRIV_CONF);

/***************************************************************
 * frame-relay zap
 */
EOLS	(ci_FR_zap_eol, frame_relay_command, FRAME_RELAY_ZAP);
KEYWORD	(ci_FR_zap, ci_FR_zap_eol, ci_FR_intftype,
	 "zap", "Clear (zap) an entire frame relay map",
	 PRIV_CONF | PRIV_HIDDEN);

/***************************************************************
 * frame-relay multicast-dlci <number>
 * no frame-relay multicast-dlci [<number>]
 */
PARAMS_KEYONLY (ci_FR_multicastdlci, ci_FR_zap,
	"multicast-dlci", OBJ(int,1), 1019, 1022,
	frame_relay_command, FRAME_RELAY_MULTICAST,
	"Set DLCI of a multicast group", "DLCI", PRIV_CONF);

/***************************************************************
 * frame-relay payload-compress
 * no frame-relay payload-compress
 *
 * turn on/off payload compression for point-to-point interfaces
 */
EOLS    (ci_FR_comp_eol, frame_relay_command, FR_STAC_COMPRESS);
KEYWORD_ID(ci_FR_comp_cisco_p2p, ci_FR_comp_eol, no_alt,
	OBJ(int,4), FR_ENCAPS_CISCO2, "packet-by-packet", 
	"cisco proprietary encapsulation", PRIV_CONF|PRIV_SUBIF);
	/* ietf-style compression-style selector will go right here */
NVGENS  (ci_FR_comp_nvgen, ci_FR_comp_cisco_p2p,
	frame_relay_command, FR_STAC_COMPRESS);
KEYWORD (ci_FR_comp, ci_FR_comp_nvgen, ci_FR_multicastdlci,
	"payload-compression", "Use payload compression",
	PRIV_CONF|PRIV_SUBIF);

/***********************************************************************
*
* frame-relay ip tcp header-compression [passive]
* no frame-relay ip tcp header-compression [passive]
*/
EOLS    (ci_fr_hdrcomp_eol, frame_relay_command, FR_TCP_COMPRESS);
KEYWORD_ID (ci_fr_hdrcomp_passive, ci_fr_hdrcomp_eol, ci_fr_hdrcomp_eol,
	OBJ(int,1), FR_INTF_PASSIVE,
	"passive", "Compress for destinations sending compressed headers",
	PRIV_CONF|PRIV_SUBIF);
NOPREFIX (ci_fr_hdrcomp_noprefix, ci_fr_hdrcomp_passive, ci_fr_hdrcomp_eol);
NVGENS  (ci_fr_hdrcomp_nvgen, ci_fr_hdrcomp_noprefix,
	frame_relay_command, FR_TCP_COMPRESS);
KEYWORD (ci_FR_tcpip_hdr_comp, ci_fr_hdrcomp_nvgen, no_alt,
	"header-compression", "Enable TCP/IP header compression",
	PRIV_CONF|PRIV_SUBIF);
KEYWORD (ci_FR_ip, ci_FR_tcpip_hdr_comp, no_alt,
	"tcp", "TCP header compression parameters ",
	PRIV_CONF|PRIV_SUBIF);
KEYWORD (ci_FR_tcp, ci_FR_ip, ci_FR_comp,
	"ip", "Frame Relay Internet Protocol config commands",
	PRIV_CONF|PRIV_SUBIF);

/***************************************************************
 * frame-relay local-dlci <number>
 * no frame-relay local-dlci [<number>]
 */
PARAMS_KEYONLY (ci_FR_localdlci, ci_FR_tcp, "local-dlci",
		OBJ(int,1), 16, 1007,
		frame_relay_command, FRAME_RELAY_LOCAL_DLCI,
		"Set source DLCI when LMI is not supported",
		"Source DLCI number", PRIV_CONF);

/***************************************************************
 * frame-relay lmi-type
 * no frame-relay lmi-type
 * hack it to set 
 * OBJ(int,1) to 1 if cisco
 * OBJ(int,1) to 2 if ansi
 * OBJ(int,1) to 3 if ccitt
 */
EOLS	(ci_FR_lmitype_eol, frame_relay_command, FRAME_RELAY_LMI_TYPE);
/*
 * Map the LMI strings to the appropriate value
*/
KEYWORD_ID (ci_FR_q933a_lmi, ci_FR_lmitype_eol, no_alt,
	    OBJ(int,1), FR_CCITT_LMI, "q933a", "", PRIV_CONF);
KEYWORD_ID (ci_FR_ccitt_lmi, ci_FR_lmitype_eol, ci_FR_q933a_lmi,
	    OBJ(int,1), FR_CCITT_LMI, "ccitt", "", PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID (ci_FR_ansi_lmi, ci_FR_lmitype_eol, ci_FR_ccitt_lmi,
	    OBJ(int,1), FR_ANSI_D, "ansi", "", PRIV_CONF);
KEYWORD_ID (ci_FR_ansi_annexd_lmi, ci_FR_lmitype_eol, ci_FR_ansi_lmi,
	    OBJ(int,1), FR_ANSI_D, "annex D", "", PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID (ci_FR_cisco_lmi, ci_FR_lmitype_eol, ci_FR_ansi_annexd_lmi,
	    OBJ(int,1), FR_CISCO_LMI, "cisco", "", PRIV_CONF);
NVGENS  (ci_FR_lmitype_nvgen, ci_FR_cisco_lmi, frame_relay_command,
	 FRAME_RELAY_LMI_TYPE);
KEYWORD	(ci_FR_lmitype, ci_FR_lmitype_nvgen, ci_FR_localdlci,
	 "lmi-type", "Use CISCO-ANSI-CCITT type LMI", PRIV_CONF);

/***************************************************************
 * frame-relay keepalive <seconds>
 * frame-relay keepalive
 * no frame-relay keepalive [<seconds>]
 */
EOLS	(ci_FR_keepalive_eol, frame_relay_command, FRAME_RELAY_KEEPALIVE);
NUMBER	(ci_FR_keepalive_num, ci_FR_keepalive_eol, ci_FR_keepalive_eol,
	 OBJ(int,1), 5, 30, "Set LMI keepalive interval");
SET (ci_FR_keepalive_set, ci_FR_keepalive_num, OBJ(int,1), 10);
NVGENS	(ci_FR_keepalive_nvgen, ci_FR_keepalive_set,
	 frame_relay_command, FRAME_RELAY_KEEPALIVE);
KEYWORD	(ci_FR_keepalive, ci_FR_keepalive_nvgen, ci_FR_lmitype,
	    "keepalive", "Set LMI keepalive interval", PRIV_CONF);

LINK_TRANS(cfg_fr_inarp_extend_here, ci_FR_keepalive);


/***************************************************************
 * frame-relay interface-dlci <number> [BROADCAST]
 * no frame-relay interface-dlci <number>
*/
EOLS	(ci_FR_dlci_eol, frame_relay_command, FR_DLCI);

PROTOADDR (ci_intf_dlci_protoaddr, ci_FR_dlci_eol, NONE,
	   OBJ(int,10), OBJ(paddr,1), PMATCH_IP);

KEYWORD	(ci_intf_prot_keyword, ci_intf_dlci_protoaddr, ci_FR_dlci_eol,
	    "protocol", "Optional protocol information for remote end",
	 PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(ci_FR_dlci_ietf, ci_intf_prot_keyword, ci_intf_prot_keyword,
	   OBJ(int,4), FR_ENCAPS_IETF,
	   "ietf", "Use RFC1294 Encapsulation", PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(ci_FR_dlci_cisco, ci_intf_prot_keyword, ci_FR_dlci_ietf,
	   OBJ(int,4), FR_ENCAPS_CISCO,
	   "cisco", "Use CISCO Encapsulation", PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID (ci_FR_dlci_broadcast, ci_FR_dlci_cisco, ci_FR_dlci_cisco,
	   OBJ(int,3), TRUE,
	   "broadcast", "Broadcasts should be forwarded to this address",
	   PRIV_CONF|PRIV_SUBIF|PRIV_NOHELP);

IFELSE(ci_FR_p2p_tst, ci_FR_dlci_broadcast, ci_FR_dlci_eol,
       is_p2p(csb->interface));

SET     (ci_FR_dlci_encaps_def, ci_FR_p2p_tst, OBJ(int,4),
	 FR_ENCAPS_NOTSET);

NUMBER	(ci_FR_dlci_num, ci_FR_dlci_encaps_def, no_alt,
	 OBJ(int,1), 16, 1007, "Define a DLCI as part of the current "
	 "subinterface");
NVGENS	(ci_FR_dlci_nvgen, ci_FR_dlci_num,
	 frame_relay_command, FR_DLCI);

KEYWORD	(ci_FR_int_dlci, ci_FR_dlci_nvgen, cfg_fr_inarp_extend_here,
	    "interface-dlci", "Define a DLCI as part of a subinterface",
	 PRIV_CONF | PRIV_SUBIF);


/***************************************************************
 * frame-relay map <protocol> <protocoladdr> <dlci> .........
 * no frame-relay map <protocol> <protocoladdr> ..........
 *
 * OBJ(int,1) = <dlci>
 * OBJ(int,2) = set to FR_MAP_PASSIVE if "passive" compression is requested.
 *              When set to FR_MAP_ACTIVE, this map will not inherit
 *              the current "passive" setting of the interface. This allows the
 *              the freedom to configure a map independent of the attributes
 *              of the interface. "passive" and "active" are
 *              mutually exclusive.
 * OBJ(int,3) = TRUE if the "broadcast" option was given
 * OBJ(int,4) = FR_ENCAPS_[IETF|CISCO|CISCO2|unspec] encaps for this PVC
 * OBJ(int,5) = Akin to OBJ(int,2), the "nocompress" option for IP
 *              maps will ensure that this PVC will continue to run
 *              IP traffic even if the interface is configured
 *              for compression. ie. The IP map will not inherit
 *              the compression characteristics of the interface.
 * OBJ(int,10) = <protocol>
 * (*OBJ(paddr,1)) = <protocoladdr>
 */

EOLS	(ci_FR_map_eol, frame_relay_command, FRAME_RELAY_MAP);

pdecl(ci_FR_map_opts);

KEYWORD_ID (ci_FR_map_comprtcp_active, ci_FR_map_eol,
	ci_FR_map_eol, OBJ(int,2), FR_MAP_ACTIVE,  "active",
	"Always compress TCP/IP headers",
	PRIV_CONF|PRIV_SUBIF);

TESTVAR	(ci_FR_map_check_active, NONE, ci_FR_map_comprtcp_active, NONE, NONE,
	 NONE, OBJ(int,2), FR_MAP_ACTIVE);

KEYWORD_ID (ci_FR_map_comprtcp_passive, ci_FR_map_eol,
	ci_FR_map_check_active, OBJ(int,2), FR_MAP_PASSIVE, "passive",
	"Compress for destinations sending compressed headers",
	PRIV_CONF|PRIV_SUBIF);

TESTVAR	(ci_FR_map_check_passive, NONE, ci_FR_map_comprtcp_passive, NONE, NONE,
	 NONE, OBJ(int,2), FR_MAP_ACTIVE);

KEYWORD_ID (ci_FR_map_tcpip_hdr_comp, ci_FR_map_check_passive, no_alt,
	OBJ(int,10), LINK_COMPRESSED_TCP, "header-compression",
	"Enable TCP/IP header compression",
	PRIV_CONF|PRIV_SUBIF);

KEYWORD (ci_FR_map_tcp, ci_FR_map_tcpip_hdr_comp, no_alt,
	"tcp", "TCP header compression parameters",
	PRIV_CONF|PRIV_SUBIF);

ASSERT (ci_FR_map_test, ci_FR_map_tcp, NONE,
	((csb->sense && (GETOBJ(int,10) == LINK_IP)) &&
	(GETOBJ(int,4) != FR_ENCAPS_IETF) &&
	(GETOBJ(int,5) != FR_MAP_NO_COMPRESS)));

KEYWORD_ID (ci_FR_map_active, ci_FR_map_opts, ci_FR_map_test,
	OBJ(int,2), FR_MAP_ACTIVE, "active",
	"Always compress TCP/IP headers", PRIV_CONF|PRIV_SUBIF);

ASSERT (ci_FR_map_nocompress_test, ci_FR_map_active, NONE,
	((csb->sense && (GETOBJ(int,10) == LINK_IP)) &&
	(GETOBJ(int,4) != FR_ENCAPS_IETF) &&
	(GETOBJ(int,5) != FR_MAP_NO_COMPRESS)));

KEYWORD_ID (ci_FR_map_compr_opts, ci_FR_map_opts, ci_FR_map_nocompress_test,
	OBJ(int,5), FR_MAP_NO_COMPRESS, "nocompress",
	"Do not compress TCP/IP headers", PRIV_CONF|PRIV_SUBIF);

ASSERT (ci_FR_map_ip_test, ci_FR_map_compr_opts, NONE,
	((csb->sense && (GETOBJ(int,10) == LINK_IP)) &&
	(GETOBJ(int,2) != FR_MAP_ACTIVE)));

KEYWORD_ID(ci_FR_cisco, ci_FR_map_opts, ci_FR_map_ip_test,
	OBJ(int,4), FR_ENCAPS_CISCO, "cisco",
	"Use CISCO Encapsulation", PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(ci_FR_comp_cisco, ci_FR_map_eol, no_alt,
	OBJ(int,4), FR_ENCAPS_CISCO2, "packet-by-packet", 
	"cisco proprietary encapsulation", PRIV_CONF|PRIV_SUBIF);
	/* ietf-style compression-style selector will go right here */

KEYWORD(ci_FR_comp_payload, ci_FR_comp_cisco, ci_FR_cisco,
	"payload-compression", "Use payload compression", 
	PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(ci_FR_ietf, ci_FR_map_opts, ci_FR_comp_payload,
	OBJ(int,4), FR_ENCAPS_IETF, "ietf",
	"Use RFC1490 Encapsulation", PRIV_CONF|PRIV_SUBIF);

TESTVAR	(ci_FR_map_active_test, NONE, ci_FR_ietf, NONE, NONE, NONE,
	 OBJ(int,2), FR_MAP_ACTIVE);

KEYWORD_ID(ci_FR_broadcast, ci_FR_map_opts, ci_FR_map_active_test,
	OBJ(int,3), TRUE,
	"broadcast", "Broadcasts should be forwarded to this address",
	PRIV_CONF|PRIV_SUBIF);

NOP	(ci_FR_map_opts, ci_FR_broadcast, ci_FR_map_eol);

SET     (ci_FR_encaps_def, ci_FR_map_opts, OBJ(int,4), FR_ENCAPS_NOTSET);

SET     (ci_FR_init_options, ci_FR_encaps_def, OBJ(int,2), FR_MAP_INIT_OPTIONS);

NOPREFIX(ci_FR_map_noprefix2, ci_FR_init_options, ci_FR_map_eol);

NUMBER	(ci_FR_map_dlci, ci_FR_map_noprefix2, no_alt,
	 OBJ(int,1), 16, 1007, "DLCI");
NOPREFIX (ci_FR_map_noprefix1, ci_FR_map_dlci, ci_FR_map_eol);

IFELSE	(ci_FR_map_dlci_test, ci_FR_map_dlci, ci_FR_map_noprefix1,
	 LINK_LEARN_OK(GETOBJ(int,10)));

/* <protocol> <protocoladdr> */

PROTOADDR (ci_FR_map_protoaddr, ci_FR_map_dlci_test, no_alt,
	   OBJ(int,10), OBJ(paddr,1), PMATCH_DGRAM PMATCH_DLSW
	PMATCH_LLC2 PMATCH_RSRB PMATCH_STUN PMATCH_EXCLUDE PMATCH_AARP 
	PMATCH_ARP PMATCH_SNAPSHOT);
NVGENS	(ci_FR_map_nvgen, ci_FR_map_protoaddr,
	 frame_relay_command, FRAME_RELAY_MAP);
KEYWORD	(ci_FR_map, ci_FR_map_nvgen, ci_FR_int_dlci,
	 "map", "Map a protocol address to a DLCI address",
	 PRIV_CONF|PRIV_SUBIF);

/***************************************************************
 * frame-relay dlci-bits { 10 | 11 | 13 }
 * no frame-relay dlci-bits [{ 10 | 11 | 13 }]
 */
EOLS	(ci_FR_dlcibits_eol, frame_relay_command, FRAME_RELAY_DLCI_BITS);

KEYWORD_ID (ci_FR_dlcibits13, ci_FR_dlcibits_eol, no_alt,
	    OBJ(int,1), 13,
	    "13", "Number of bits in DLCI", PRIV_CONF|PRIV_UNSUPPORTED); 

KEYWORD_ID (ci_FR_dlcibits11, ci_FR_dlcibits_eol, ci_FR_dlcibits13,
	    OBJ(int,1), 11,
	    "11", "", PRIV_CONF|PRIV_UNSUPPORTED);

KEYWORD_ID (ci_FR_dlcibits10, ci_FR_dlcibits_eol, ci_FR_dlcibits11,
	    OBJ(int,1), 10,
	    "10", "Number of bits in DLCI", PRIV_CONF|PRIV_UNSUPPORTED);

NOPREFIX (ci_FR_dlcibits_noprefix, ci_FR_dlcibits10, ci_FR_dlcibits_eol);
KEYWORD (ci_FR_dlcibits, ci_FR_dlcibits_noprefix, ci_FR_map,
	"dlci-bits", "Set number of bits in the DLCI", PRIV_CONF|PRIV_UNSUPPORTED);

/***************************************************************
 * frame-relay de-group <number> <dlci>
 * no frame-relay de-group <number> <dlci>
 */

EOLS	(ci_FR_de_eol, frame_relay_command, FR_DEGROUP);

NOPREFIX (ci_FR_de_noprefix, ci_FR_de_eol, ci_FR_de_eol);

NUMBER	(ci_FR_de_dlci, ci_FR_de_noprefix, no_alt,
	 OBJ(int,3), 16, 1007, "DLCI value");

NUMBER	(ci_FR_de_group, ci_FR_de_dlci, no_alt,
	 OBJ(int,2), 1, 10, "DE group number");

NVGENS	(ci_FR_de_nvgen, ci_FR_de_group,
	 frame_relay_command, FR_DEGROUP);

KEYWORD	(ci_FR_de, ci_FR_de_nvgen, ci_FR_dlcibits,
	    "de-group", "Associate a DE group with a DLCI",
	 PRIV_CONF | PRIV_SUBIF);

/***************************************************************
 * frame-relay priority-dlci-group <group> <high> <medium> [normal] [low] dlci
 * no frame-relay priority-dlci-group 
*/

EOLS	(ci_FR_prdlci_eol, frame_relay_command, FR_DLCI_GROUP);

NUMBER	(ci_FR_prdlci_low, ci_FR_prdlci_eol, ci_FR_prdlci_eol,
	 OBJ(int,4), 16, 1007, "DLCI for low priority");

NUMBER	(ci_FR_prdlci_normal, ci_FR_prdlci_low, ci_FR_prdlci_eol,
	 OBJ(int,3), 16, 1007, 
	 "DLCI for normal priority");

NUMBER	(ci_FR_prdlci_medium, ci_FR_prdlci_normal, ci_FR_prdlci_eol,
	 OBJ(int,2), 16, 1007, "DLCI for medium priority");

NOPREFIX (ci_FR_prdlci_noprefix, ci_FR_prdlci_medium, ci_FR_prdlci_eol);

NUMBER	(ci_FR_prdlci_high, ci_FR_prdlci_noprefix, no_alt,
	 OBJ(int,1), 16, 1007, "DLCI for high priority");

NUMBER	(ci_FR_prdlci_group, ci_FR_prdlci_high, no_alt,
	 OBJ(int,5), 1, 16, "Assign priority group");

NVGENS	(ci_FR_prdlci_nvgen, ci_FR_prdlci_group,
	 frame_relay_command, FR_DLCI_GROUP);

KEYWORD	(ci_FR_prdlci, ci_FR_prdlci_nvgen, ci_FR_de,
	 "priority-dlci-group", "Define a priority group of DLCIs",
	 PRIV_CONF | PRIV_SUBIF);


LINK_TRANS(cfg_fr_vcq_extend_here, ci_FR_prdlci);

/***************************************************************/
KEYWORD	(ci_FR_start, cfg_fr_vcq_extend_here, ALTERNATE,
	 "frame-relay", "Set frame relay parameters", PRIV_CONF|PRIV_SUBIF);

ASSERT(ci_FR_tst, ci_FR_start, ALTERNATE,
       is_frame_relay(csb->interface->hwptr));
TEST_IDBSTATUS(ci_framerelay, ci_FR_tst , NONE, ALTERNATE, IDB_SERIAL);

#undef  ALTERNATE
#define ALTERNATE       ci_framerelay
