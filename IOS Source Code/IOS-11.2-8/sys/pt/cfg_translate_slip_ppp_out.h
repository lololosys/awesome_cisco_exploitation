/* $Id: cfg_translate_slip_ppp_out.h,v 3.4.10.1 1996/04/02 17:19:31 swenson Exp $
 * $Source: /release/112/cvs/Xsys/pt/cfg_translate_slip_ppp_out.h,v $
 *------------------------------------------------------------------
 * SLIP or PPP translate subcommands
 *
 * May 1994, Eman Schaffer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_translate_slip_ppp_out.h,v $
 * Revision 3.4.10.1  1996/04/02  17:19:31  swenson
 * CSCdi52228:  translate command restart-time option not written to nvram
 * Branch: California_branch
 *
 * Revision 3.4  1996/02/22  05:20:24  fox
 * CSCdi35908:  Should allow CHAP and PAP clients to same dialin pool
 * Add support for multiple acceptable inbound PPP authentication methods
 *
 * Revision 3.3  1995/12/07  17:45:28  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.2  1995/11/17  18:58:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:00:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:56:27  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:14:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_EXIT(trans_out_slip_ppp_return, no_alt);

/************************************************************************
 *  slip/ppp and out options
 *
 * [slip | ppp] [<ipaddr> | <hostname> | <ip-pool [scope-name name]>] 
 * [routing | header-compression [passive] | keepalive | mtu]
 * For PPP only:
 * ppp [authentication <pap | chap>] [use-tacacs]
 *
 * slip/ppp <ipaddr> = OBJ(paddr,7)
 * trans_proto = OBJ(int,7)
 * proto = OBJ(int,8)
 * routing = OBJ(int,9)
 * headercompressed = OBJ(int,10)
 * passive = OBJ(int,11)
 * proto (slip/ppp) = OBJ*int.12)
 * use_tacacs = OBJ(int,15)	[if old_access_control ]
 * method in use = OBJ(int,15)	[if ! old_access_control ]
 * method = OBJ(string,4)	[if ! old_access_control ]
 * keepalive = OBJ(int,16)
 * keepalive_period = OBJ(int,17)
 * mtu = OBJ(int,18)
 * mtu_size = OBJ(int,19)
 * scope_name = OBJ(string,5)
 * ipx_idb = OBJ(idb,1)
 * confreq_resttime = OBJ(int,20)
 * resettime_value = OBJ(int,21)
 * authentication methods = uiarray [int,13 and int,14 are used temporarily]
 */

pdecl(trans_out_slip_ppp_opts);

KEYWORD_ID(trans_out_ppp_use_tacacs_kwd, trans_out_slip_ppp_opts, NONE,
	   OBJ(int,15), TRUE,
	   "use-tacacs", "Use TACACS to verify PPP Authentications",
	   PRIV_CONF);
TEST_BOOLEAN(trans_out_ppp_use_tacacs, trans_out_ppp_use_tacacs_kwd,
	     no_alt, NONE, old_access_control);

SET     (trans_out_ppp_auth_list_set, trans_out_slip_ppp_opts, OBJ(int,15), 2);
STRING  (trans_out_ppp_auth_list_name, trans_out_ppp_auth_list_set,
         trans_out_slip_ppp_opts,
         OBJ(string,4), "Use an authentication list with this name.");
TEST_MULTIPLE_FUNCS (trans_out_ppp_auth_list_if, trans_out_ppp_auth_list_name,
		     NONE, no_alt);
KEYWORD_ID(trans_out_ppp_auth_list_kwd, trans_out_slip_ppp_opts,
	   trans_out_ppp_auth_list_if, OBJ(int,15), 1,
           "default", "Use the default authentication list.", PRIV_CONF);
TEST_BOOLEAN(trans_out_ppp_auth_list, trans_out_slip_ppp_opts,
	     trans_out_ppp_auth_list_kwd, NONE, old_access_control);
pdecl(trans_out_ppp_auth_opts);
TESTVAR(trans_out_ppp_auth_loop_test, no_alt, NONE, NONE, NONE, NONE,
	OBJ(int,14), 0);
EVAL(trans_out_ppp_auth_chap_eval, trans_out_ppp_auth_opts,
     csb->uiarray[GETOBJ(int,14) ++] = PPP_AUTH_CHAP);
KEYWORD_ORTEST(trans_out_ppp_auth_chap, trans_out_ppp_auth_chap_eval,
	       trans_out_ppp_auth_loop_test,
               OBJ(int,13), PPP_AUTH_CHAP_MASK,
               "chap", "Challenge Handshake Access Protocol (CHAP)", PRIV_CONF);
EVAL(trans_out_ppp_auth_pap_eval, trans_out_ppp_auth_opts,
     csb->uiarray[GETOBJ(int,14) ++] = PPP_AUTH_PAP);
KEYWORD_ORTEST(trans_out_ppp_auth_pap, trans_out_ppp_auth_pap_eval,
	       trans_out_ppp_auth_chap, OBJ(int,13), PPP_AUTH_PAP_MASK,
               "pap", "Password Authentication Protocol (PAP)", PRIV_CONF);
NOP (trans_out_ppp_auth_opts, trans_out_ppp_auth_pap, trans_out_ppp_auth_list);
KEYWORD(trans_out_ppp_authenticate, trans_out_ppp_auth_pap,
	trans_out_ppp_use_tacacs,
	"authentication", "Set PPP link Authentication method", PRIV_CONF );
TESTVAR	(trans_out_if_ppp, trans_out_ppp_authenticate, NONE, NONE, NONE, NONE,
	 OBJ(int,8), PPP_PROTO);

NUMBER (trans_out_restart_time, trans_out_slip_ppp_opts, no_alt, OBJ(int,21),
	0, -1, "CONFREQ retransmissions time (default 2 seconds");
KEYWORD_ID (trans_out_confreq_restart, trans_out_restart_time, 
	    trans_out_if_ppp, OBJ(int,20), TRUE, "restart-timer", 
	    "", PRIV_CONF | PRIV_HIDDEN);

NUMBER (trans_out_mtu_size, trans_out_slip_ppp_opts, no_alt,
	OBJ(int,19), 64, 1000000, "MTU size in bytes");
KEYWORD_ID (trans_out_slip_ppp_mtu, trans_out_mtu_size,
	    trans_out_confreq_restart, OBJ(int,18), TRUE,
	    "mtu", "Set the interface Maximum Transmission Unit (MTU)",
	    PRIV_CONF);
 
NUMBER (trans_out_keepalive_period, trans_out_slip_ppp_opts, no_alt,  
	OBJ(int,17), 0, 32767, "Keepalive period (default 10 seconds)");

KEYWORD_ID (trans_out_slip_ppp_keepalive, trans_out_keepalive_period,  
	    trans_out_slip_ppp_mtu, OBJ(int,16), TRUE,
	    "keepalive", "Enable keepalive", PRIV_CONF);

KEYWORD_ID(trans_out_slip_ppp_routing, trans_out_slip_ppp_opts, 
	   trans_out_slip_ppp_keepalive, OBJ(int,9), TRUE,
	   "routing", "Enable ip routing", PRIV_CONF);

FORWARD_INTERFACE (trans_out_slip_ppp_ipx_int, trans_out_slip_ppp_opts,
	no_alt, OBJ(idb,1), IFTYPE_LOOPBACK);
KEYWORD(trans_out_slip_ppp_ipx, trans_out_slip_ppp_ipx_int,
	trans_out_slip_ppp_routing, 
	"ipx", "Allow IPX", PRIV_CONF);

KEYWORD_ID(trans_out_slip_ppp_passive, trans_out_slip_ppp_opts, 
	   trans_out_slip_ppp_opts, OBJ(int,11), TRUE, "passive",
	   "Enable compression only for destinations which send compressed"
	   "headers", PRIV_CONF);
KEYWORD_ID (trans_out_slip_ppp_headercompressed, trans_out_slip_ppp_passive,
	 trans_out_slip_ppp_ipx, OBJ(int,10), TRUE,
	 "header-compression", "Enable Header Compression", PRIV_CONF);
NOP	(trans_out_slip_ppp_opts, trans_out_slip_ppp_headercompressed,
	 trans_out_slip_ppp_return);

/***********************************************************************/

IPADDR_NAME(trans_out_slip_ppp_addr, trans_out_slip_ppp_opts, 
	    NONE, OBJ(paddr,7), "IP address");

TEST_MULTIPLE_FUNCS(trans_out_slip_ppp_addr_test, trans_out_slip_ppp_addr, 
		    NONE, no_alt);
STRING(trans_out_slip_ppp_scope_name, trans_out_slip_ppp_opts, 
       no_alt, OBJ(string,5), "Scope Name");

KEYWORD(trans_out_slip_ppp_scope, trans_out_slip_ppp_scope_name,
	trans_out_slip_ppp_opts, "scope-name", 
	"Obtain the IP address from a specific Local scope name", PRIV_CONF);
KEYWORD(trans_out_slip_ppp_pool, trans_out_slip_ppp_scope,
	trans_out_slip_ppp_addr_test, "ip-pool",
	"Obtain the IP address from a DHCP Proxy Client or a Local pool",
	PRIV_CONF);

SET      (trans_out_slip_proto, trans_out_slip_ppp_pool, OBJ(int,8), 
	  SLIP_PROTO);
KEYWORD_ID(trans_out_slip, trans_out_slip_proto, ALTERNATE,
	   OBJ(int,6), TRANS_PROT_SLIP_PPP,
	   "slip", "Virtual async SLIP", PRIV_CONF);

SET      (trans_out_ppp_proto, trans_out_slip_ppp_pool, OBJ(int,8), 
	  PPP_PROTO);
KEYWORD_ID(trans_out_ppp, trans_out_ppp_proto, trans_out_slip,
	   OBJ(int,6), TRANS_PROT_SLIP_PPP,
	   "ppp", "Virtual async PPP", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	trans_out_ppp

