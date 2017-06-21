/* $Id: exec_debug_vines.h,v 3.2.62.1 1996/08/12 16:09:09 widmer Exp $
 * $Source: /release/112/cvs/Xsys/vines/exec_debug_vines.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ V I N E S . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_vines.h,v $
 * Revision 3.2.62.1  1996/08/12  16:09:09  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:57:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:44:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:44:06  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/19  14:49:23  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:20:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * A word to the wise:  The variable OBJ(int,9) has been used
 * throughout this file to indicate the presence of the keyword
 * "verbose".  The code in vines_debug.c relies upon this convention.
 */

/******************************************************************
 * debug ..... verbose
 *
 */
EOLNS      (debug_vines_verbose_eol, vines_debug_command);
KEYWORD_ID(debug_vines_verbose, debug_vines_verbose_eol, debug_vines_verbose_eol,    
	   OBJ(int,9), TRUE,
	   "verbose", "Verbose detail", PRIV_OPR);


/******************************************************************
 * debug vines unknown
 *
 */
EOLS	(debug_vines_unknown_eol, vines_debug_command, DEBUG_VINES_UNKNOWN);
KEYWORD (debug_vines_unknown, debug_vines_unknown_eol, no_alt,
	 "unknown", NULL /*"unknown packets"*/, PRIV_OPR|PRIV_HIDDEN);

/******************************************************************
 * debug vines test
 *
 */
EOLS	(debug_vines_test_eol, vines_debug_command, DEBUG_VINESTEST);
KEYWORD (debug_vines_test, debug_vines_test_eol, debug_vines_unknown,
	 "test", NULL /*"test code"*/, PRIV_OPR|PRIV_HIDDEN);

/******************************************************************
 * debug vines table [ <number> ]
 *
 */
EOLS	(debug_vines_table_eol, vines_debug_command, DEBUG_VINESTABLE);
NUMBER	(debug_vines_table_list, debug_vines_table_eol, debug_vines_table_eol,
	 OBJ(int,1), VINESMINSACC, VINESMAXSACC, "Access list");
KEYWORD (debug_vines_table, debug_vines_table_list, debug_vines_test,
	 "table", "routing table changes", PRIV_OPR);

/******************************************************************
 * debug vines service [< fs | nsm | ss | st | vs > [verbose]]
 *
 */

KEYWORD_ID(debug_vines_svc_vs, debug_vines_verbose, no_alt,
	   OBJ(int,10), DEBUG_VINES_VS,
	   "vs", "Security Service activity", PRIV_OPR);
KEYWORD_ID(debug_vines_svc_st, debug_vines_verbose, debug_vines_svc_vs,
	   OBJ(int,10), DEBUG_VINES_ST,
	   "st", "StreetTalk activity", PRIV_OPR);
KEYWORD_ID(debug_vines_svc_ss, debug_vines_verbose, debug_vines_svc_st,
	   OBJ(int,10), DEBUG_VINES_SS,
	   "ss", "Server Service activity", PRIV_OPR);
KEYWORD_ID(debug_vines_svc_nsm, debug_vines_verbose, debug_vines_svc_ss,
	   OBJ(int,10), DEBUG_VINES_NSM,
	   "nsm", "Server and Network Management Service activity", PRIV_OPR);
KEYWORD_ID(debug_vines_svc_fs, debug_vines_verbose, debug_vines_svc_nsm,
	   OBJ(int,10), DEBUG_VINES_FS,
	   "fs", "File Service activity", PRIV_OPR);

KEYWORD (debug_vines_service, debug_vines_svc_fs, debug_vines_table,
	 "service", "service level activity", PRIV_OPR);
  
/******************************************************************
 * debug vines state
 *
 */
EOLS	(debug_vines_state_eol, vines_debug_command, DEBUG_VINESSTATE);
KEYWORD (debug_vines_state, debug_vines_state_eol, debug_vines_service,
	 "state", "SRTP state machine activity", PRIV_OPR);

/******************************************************************
 * debug vines routing [ <number> ] [ verbose ]
 *
 */
NUMBER	  (debug_vines_routing_list, debug_vines_verbose, debug_vines_verbose,
	   OBJ(int,1), VINESMINSACC, VINESMAXSACC, "Access list");
KEYWORD_ID(debug_vines_routing, debug_vines_routing_list, debug_vines_state,
	   OBJ(int,10), DEBUG_VINESRTP,
	   "routing", "RTP protocol activity", PRIV_OPR);

/******************************************************************
 * debug vines reinit
 *
 */
EOLS	(debug_vines_reinit_eol, vines_debug_command, DEBUG_VINES_REINIT);
KEYWORD (debug_vines_reinit, debug_vines_reinit_eol, debug_vines_routing,
	 "reinit", "SRTP Reinit causes", PRIV_OPR);

/******************************************************************
 * debug vines proxy
 *
 */
EOLS	(debug_vines_proxy_eol, vines_debug_command, DEBUG_VINESPROXY);
KEYWORD (debug_vines_proxy, debug_vines_proxy_eol, debug_vines_reinit,
	 "proxy", "Proxy service support", PRIV_OPR);

/******************************************************************
 * debug vines packet [ <number> ]
 *
 */
EOLS	(debug_vines_packet_eol, vines_debug_command, DEBUG_VINESPKT);
NUMBER	(debug_vines_packet_list, debug_vines_packet_eol, debug_vines_packet_eol,
	 OBJ(int,1), VINESMINSACC, VINESMAXSACC, "Access list");
KEYWORD (debug_vines_packet, debug_vines_packet_list, debug_vines_proxy,
	 "packet", "IP protocol activity", PRIV_OPR);

/******************************************************************
 * debug vines netrpc
 *
 */
EOLS	(debug_vines_netrpc_eol, vines_debug_command, DEBUG_VINESNETRPC);
KEYWORD (debug_vines_netrpc, debug_vines_netrpc_eol, debug_vines_packet,
	 "netrpc", "NetRPC protocol activity", PRIV_OPR);

/******************************************************************
 * debug vines ipc
 *
 */
EOLS	(debug_vines_ipc_eol, vines_debug_command, DEBUG_VINESIPC);
KEYWORD (debug_vines_ipc, debug_vines_ipc_eol, debug_vines_netrpc,
	 "ipc", "IPC protocol activity", PRIV_OPR);

/******************************************************************
 * debug vines echo
 *
 */
EOLS	(debug_vines_echo_eol, vines_debug_command, DEBUG_VINESECHO);
KEYWORD (debug_vines_echo, debug_vines_echo_eol, debug_vines_ipc,
	 "echo", "MAC-level echo activity", PRIV_OPR);

/******************************************************************
 * debug vines cache
 *
 */
EOLS	(debug_vines_cache_eol, vines_debug_command, DEBUG_VINESCACHE);
KEYWORD (debug_vines_cache, debug_vines_cache_eol, debug_vines_echo,
	 "cache", "Fast switching cache activity", PRIV_OPR);

/******************************************************************
 * debug vines broadcasts
 *
 */
EOLS	(debug_vines_bcast_eol, vines_debug_command, DEBUG_VINES_BCAST);
KEYWORD (debug_vines_bcast, debug_vines_bcast_eol, debug_vines_cache,
	 "broadcasts", "Broadcast forwarding", PRIV_OPR);

/******************************************************************
 * debug vines banyan
 *
 */
EOLS	(debug_vines_banyan_eol, vines_debug_command, DEBUG_VINESBANYAN);
KEYWORD (debug_vines_banyan, debug_vines_banyan_eol, debug_vines_bcast,
	 "banyan", NULL /*"Banyan bugs"*/, PRIV_OPR|PRIV_HIDDEN);

/******************************************************************
 * debug vines arp
 *
 */
EOLS	(debug_vines_arp_eol, vines_debug_command, DEBUG_VINESARP);
KEYWORD (debug_vines_arp, debug_vines_arp_eol, debug_vines_banyan,
	 "arp", "ARP protocol activity", PRIV_OPR);

/******************************************************************
 * debug vines
 *
 */
KEYWORD_DEBUG(debug_vines, debug_vines_arp, ALTERNATE,
	      OBJ(pdb,1), vines_debug_arr,
	      "vines", "VINES information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_vines
