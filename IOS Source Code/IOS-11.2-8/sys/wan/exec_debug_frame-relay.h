/* $Id: exec_debug_frame-relay.h,v 3.2.62.2 1996/05/31 18:19:20 vrawat Exp $
 * $Source: /release/112/cvs/Xsys/wan/exec_debug_frame-relay.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ F R A M E R E L A Y . H
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_frame-relay.h,v $
 * Revision 3.2.62.2  1996/05/31  18:19:20  vrawat
 * CSCdi58846:  frame-relay debugging enhancements
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/04/30  18:37:16  shankar
 * CSCdi56208:  Parser support for FR SVC debugging is missing
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:01:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:05  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.3  1995/06/19  14:48:03  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/08  15:55:45  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  22:18:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * FR SVC related material
 */

EOLS  (debug_fr_svc_detailed_eol, debug_command, DEBUG_FRAME_RELAY_SVC_DETAILED);
KEYWORD (debug_fr_svc_detailed, debug_fr_svc_detailed_eol,
	no_alt, "detailed", "Detailed Debug: Only for Lab use", PRIV_OPR);
EOLS  (debug_fr_svc_nli_eol, debug_command, DEBUG_FRAME_RELAY_SVC_NLI);
KEYWORD (debug_fr_svc_nli, debug_fr_svc_nli_eol,
	debug_fr_svc_detailed, "nli", "Network Layer interface", PRIV_OPR);
EOLS  (debug_fr_svc_l3ie_eol, debug_command, DEBUG_FRAME_RELAY_SVC_L3IE);
KEYWORD (debug_fr_svc_l3ie, debug_fr_svc_l3ie_eol,
	debug_fr_svc_nli, "l3ie", "Frame Relay IE parsing/construction", PRIV_OPR);
EOLS  (debug_fr_svc_l3cc_eol, debug_command, DEBUG_FRAME_RELAY_SVC_L3CC);
KEYWORD (debug_fr_svc_l3, debug_fr_svc_l3cc_eol,
	debug_fr_svc_l3ie, "l3cc", "Frame Relay Layer 3 Call Control", PRIV_OPR);

/******************************************************************
 * debug frame-relay lapf             -cwc
 */
EOLS  (debug_frame_relay_lapf_eol, debug_command, DEBUG_FRAME_RELAY_LAPF);
KEYWORD_ID(debug_lapf, debug_frame_relay_lapf_eol,
                debug_fr_svc_l3, OBJ(int,10), TRUE,
                "lapf", "Frame Relay SVC Layer 2", PRIV_OPR );

/******************************************************************
 * debug frame-relay ip tcp compression
 *
 */
EOLS  (debug_fr_tcp_header_comp_eol, debug_command,
       DEBUG_FRAME_RELAY_TCPIP_COMP);
KEYWORD (debug_fr_header_compr, debug_fr_tcp_header_comp_eol,
	 debug_fr_tcp_header_comp_eol,
	 "header-compression", "TCP/IP Header Compression", PRIV_OPR);
KEYWORD (debug_fr_tcpip, debug_fr_header_compr, no_alt,
	 "tcp", "Frame Relay Transport Control Protocol", PRIV_OPR);
KEYWORD (debug_fr_ip, debug_fr_tcpip, debug_lapf,
	 "ip", "Frame Relay Internet Protocol", PRIV_OPR);

/*
 * debug frame-relay rsrb
 *
 */
EOLS  (debug_fr_rsrb_packet_eol, debug_command, DEBUG_FRAME_RELAY_RSRB);
KEYWORD (debug_fr_rsrb, debug_fr_rsrb_packet_eol, debug_fr_ip,
	 "rsrb", "Frame Relay rsrb", PRIV_OPR);

/*
 * debug frame-relay dlsw
 *
 */
EOLS  (debug_fr_dlsw_packet_eol, debug_command, DEBUG_FRAME_RELAY_DLSW);
KEYWORD (debug_fr_dlsw, debug_fr_dlsw_packet_eol, debug_fr_rsrb,
	 "dlsw", "Frame Relay dlsw", PRIV_OPR);

/*
 * debug frame-relay llc2
 *
 */
EOLS  (debug_fr_llc2_packet_eol, debug_command, DEBUG_FRAME_RELAY_LLC2);
KEYWORD (debug_fr_llc2, debug_fr_llc2_packet_eol, debug_fr_dlsw,
	 "llc2", "Frame Relay llc2", PRIV_OPR);

/*
 * debug frame-relay packet [interface <int> [dlci <dlci>]]
 *
 */
EOLS	(debug_frame_relay_packet_eol, fr_debug_command, DEBUG_FRAME_RELAY_PACKET);
NUMBER  (debug_frame_relay_packet_dlci_no, debug_frame_relay_packet_eol,
         no_alt, OBJ(int,1), 16, 1007, " DLCI # ");
KEYWORD (debug_frame_relay_packet_dlci, debug_frame_relay_packet_dlci_no,
         debug_frame_relay_packet_eol, "dlci", " frame-relay dlci", PRIV_OPR);
INTERFACE (debug_frame_relay_packet_int, debug_frame_relay_packet_dlci,
           no_alt, OBJ(idb,1), IFTYPE_SERIAL | IFTYPE_HSSI |
	   IFTYPE_TUNNEL | IFTYPE_DIALER | IFTYPE_BRI | IFTYPE_SUBIF);
KEYWORD (debug_frame_relay_packet_interface,debug_frame_relay_packet_int,
         debug_frame_relay_packet_eol, common_str_interface,
         "Frame Relay Interface", PRIV_OPR);
KEYWORD (debug_frame_relay_packet, debug_frame_relay_packet_interface, 
	 debug_fr_llc2,
	 "packet", "Frame Relay packets", PRIV_OPR);

/******************************************************************
 * debug frame-relay lmi  [interface <int>]
 */
EOLS	(debug_frame_relay_lmi_eol, fr_debug_command, DEBUG_FRAME_RELAY_LMI);
INTERFACE (debug_frame_relay_lmi_int, debug_frame_relay_lmi_eol, no_alt,
           OBJ(idb,1), (IFTYPE_SERIAL | IFTYPE_HSSI | IFTYPE_DIALER | IFTYPE_BRI));
KEYWORD(debug_frame_relay_lmi_interface,debug_frame_relay_lmi_int,
           debug_frame_relay_lmi_eol, common_str_interface,
           "Frame Relay Interface", PRIV_OPR);
KEYWORD (debug_frame_relay_lmi, debug_frame_relay_lmi_interface, debug_frame_relay_packet,
	 "lmi", "LMI packet exchanges with service provider", PRIV_OPR);

/******************************************************************
 * debug frame-relay events
 */
EOLS	(debug_frame_relay_events_eol, debug_command, DEBUG_FRAME_RELAY_EVENTS);
KEYWORD (debug_frame_relay_events, debug_frame_relay_events_eol,
	 debug_frame_relay_lmi,
	 "events", "Important Frame Relay packet events", PRIV_OPR);

/******************************************************************
 * debug frame-relay verbose
 */
EOLS	(debug_frame_relay_verbose_eol, debug_command, DEBUG_FRAME_RELAY);
KEYWORD_ID(debug_frame_relay_verbose, debug_frame_relay_verbose_eol,
	   debug_frame_relay_events,
	   OBJ(int,10), TRUE,
	   "verbose", "Frame Relay", PRIV_OPR);

/******************************************************************
 * debug frame-relay
 */
KEYWORD_DEBUG(debug_frame_relay, debug_frame_relay_verbose, ALTERNATE,
	      OBJ(pdb,1), fr_debug_arr,
	      "frame-relay", "Frame Relay", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_frame_relay
