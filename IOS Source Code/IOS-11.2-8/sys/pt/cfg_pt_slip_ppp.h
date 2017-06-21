/* $Id: cfg_pt_slip_ppp.h,v 3.4 1996/02/22 05:20:22 fox Exp $
 * $Source: /release/111/cvs/Xsys/pt/cfg_pt_slip_ppp.h,v $
 *------------------------------------------------------------------
 * C F G _ P T _ S L I P _ P P P . H
 *
 * PT Virtual Async SLIP/PPP Global Command Support
 *
 * May 1994, Eman Schaffer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_pt_slip_ppp.h,v $
 * Revision 3.4  1996/02/22  05:20:22  fox
 * CSCdi35908:  Should allow CHAP and PAP clients to same dialin pool
 * Add support for multiple acceptable inbound PPP authentication methods
 *
 * Revision 3.3  1995/12/07  17:45:27  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.2  1995/11/17  18:58:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:00:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:56:24  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:11:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * vty-async
 * no vty-async
 *
 *  Enable SLIPMODE on all vty lines.  The no form will disable all vty-async
 *   sessions (including those in progress).
 * 
 */

EOLS    (vty_async_enable_eol, pt_vty_async_cfg_command, PT_VTY_ASYNC_ENABLED);


/************************************************************************
 * vty-async mtu <bytes>
 * no vty-async mtu
 *
 * Configure the max transfer unit that will be used for async protocols.
 */
EOLS       (vty_async_ip_mtu_eol, pt_vty_async_cfg_command, PT_INT_MTU);
NUMBER (vty_async_ip_mtu_number, vty_async_ip_mtu_eol, no_alt,
	OBJ(int,1), 64, 1000000, "MTU size in bytes");
KEYWORD (vty_async_ip_mtu, vty_async_ip_mtu_number, vty_async_enable_eol,
	 "mtu", "Set the interface Maximum Transmission Unit (MTU)",
	 PRIV_CONF);

/************************************************************************
 * vty-async keepalive <seconds>
 * no vty-async keepalive
 *
 * Configure the keepalive that will be used for async protocols.
 */
EOLS       (vty_async_keepalive_eol, pt_vty_async_cfg_command, PT_KEEPALIVE);
NUMBER (keepalive_seconds, vty_async_keepalive_eol, no_alt,
	OBJ(int,1), 0, 32767, "Keepalive period (default 10 seconds)");
KEYWORD (vty_async_keepalive, keepalive_seconds, vty_async_ip_mtu,
	 "keepalive", "Enable keepalive", PRIV_CONF);

 
/************************************************************************
 * vty-async ppp [authentication <pap | chap>] [use-tacacs]
 * no vty-async ppp [authentication <pap | chap>] [use-tacacs]
 *
 * These command (and their no forms) are equivalent to the ppp authentication
 *   commands found in cfg_int_ppp.h
 *
 */
EOLS    (vty_async_tacacs_eol, pt_vty_async_cfg_command, PT_PPP_USE_TACACS);
KEYWORD (vty_async_use_tacacs_kwd, vty_async_tacacs_eol, NONE,
         "use-tacacs", "Use TACACS to verify PPP Authentications",
         PRIV_CONF);
TEST_BOOLEAN(vty_async_use_tacacs, vty_async_use_tacacs_kwd, NONE, no_alt,
	     old_access_control);

EOLS	(vty_async_auth_eol, pt_vty_async_cfg_command, PT_PPP_AUTHENTICATION);
SET	(vty_async_auth_list_set, vty_async_auth_eol, OBJ(int,2), 2);
STRING	(vty_async_auth_list_name, vty_async_auth_list_set, vty_async_auth_eol,
	 OBJ(string,1), "Use an authentication list with this name.");
TEST_MULTIPLE_FUNCS (vty_async_auth_list_if, vty_async_auth_list_name,
		     NONE, no_alt);
KEYWORD_ID(vty_async_auth_list_kwd, vty_async_auth_eol, vty_async_auth_list_if,
	   OBJ(int,2), 1,
	   "default", "Use the default authentication list.", PRIV_CONF);
TEST_BOOLEAN(vty_async_auth_list, vty_async_auth_eol, vty_async_auth_list_kwd,
	     NONE, old_access_control);
pdecl(vty_async_auth_opts);
TESTVAR(vty_async_auth_looptest, no_alt, NONE, NONE, NONE, NONE, OBJ(int,9), 0);
EVAL(vty_async_auth_chap_eval, vty_async_auth_opts,
     csb->uiarray[GETOBJ(int, 9) ++] = PPP_AUTH_CHAP);
KEYWORD_ORTEST(vty_async_auth_chap, vty_async_auth_chap_eval,
	       vty_async_auth_looptest,
               OBJ(int,1), PPP_AUTH_CHAP_MASK,
               "chap", "Challenge Handshake Access Protocol (CHAP)", PRIV_CONF);
EVAL(vty_async_auth_pap_eval, vty_async_auth_opts,
     csb->uiarray[GETOBJ(int, 9) ++] = PPP_AUTH_PAP);
KEYWORD_ORTEST(vty_async_auth_pap, vty_async_auth_pap_eval, vty_async_auth_chap,
               OBJ(int,1), PPP_AUTH_PAP_MASK,
               "pap", "Password Authentication Protocol (PAP)", PRIV_CONF);
NOP (vty_async_auth_opts, vty_async_auth_pap, vty_async_auth_list);
KEYWORD (vty_async_auth, vty_async_auth_pap, vty_async_use_tacacs,
	 "authentication", "Set PPP link Authentication method", PRIV_CONF );
KEYWORD (vty_async_auth_command, vty_async_auth, vty_async_keepalive,
	 "ppp", "Point-to-point protocol", PRIV_CONF);

/************************************************************************
 * vty-async dynamic-routing
 * no vty-async dynamic-routing
 *
 * Permit routing other an async link.
 *
 */
EOLS    (vty_async_dynamic_routing_eol, pt_vty_async_cfg_command, 
	 PT_DYNAMIC_ROUTING);
KEYWORD (vty_async_dynamic_routing, vty_async_dynamic_routing_eol, 
	 vty_async_auth_command, "dynamic-routing", 
	 "Enable dynamic routing", PRIV_CONF);

/************************************************************************
 * vty-async header-compression [passive]
 * no vty-async header-compression [passive]
 *
 * Ask for header-compression during async protocol establishment (or just
 *  accept it if passive is configured).
 */
EOLS       (vty_async_hdrcomp_eol, pt_vty_async_cfg_command, PT_HEADER_COMP);
KEYWORD_ID (vty_async_hdrcomp_passive, vty_async_hdrcomp_eol, 
	    vty_async_hdrcomp_eol,
	    OBJ(int,1), TRUE, "passive", 
	    "Compress only for destinations which send compressed headers", 
	    PRIV_CONF);

KEYWORD (vty_async_hdrcomp, vty_async_hdrcomp_passive, 
	 vty_async_dynamic_routing, 
	 "header-compression", "Enable IP TCP header compression", PRIV_CONF); 

/************************************************************************
 * vty-async ipx ppp-client {interface}
 * no vty-async ipx ppp-client {interface}
 *
 * Allow PPP links to use ipx, and attach them to the specified interface.
 */
EOLS   (vty_async_ipx_eol, pt_vty_async_cfg_command, PT_IPX_PPPCLIENT);

FORWARD_INTERFACE  (vty_async_ipx_interface, vty_async_ipx_eol, no_alt,
	    OBJ(idb,1), IFTYPE_LOOPBACK);

KEYWORD (vty_async_ipx_pppclient, vty_async_ipx_interface, no_alt,
	 "ppp-client", "Enable IPX PPP client mode", PRIV_CONF);

KEYWORD (vty_async_ipx, vty_async_ipx_pppclient, vty_async_hdrcomp,
	 "ipx", "Enable IPX for Virtual Async", PRIV_CONF);




NVGENS  (vty_async_nvgen, vty_async_ipx, pt_vty_async_cfg_command,
         PT_VTY_ASYNC_ENABLED);

KEYWORD (cfg_pt_vty_async, vty_async_nvgen, ALTERNATE,
	 "vty-async", "Enable virtual async line configuration", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cfg_pt_vty_async


