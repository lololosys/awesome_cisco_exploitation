/* $Id: cfg_int_x25_enc.h,v 3.1.2.1 1996/04/11 23:47:39 gglick Exp $
 * $Source: /release/112/cvs/Xsys/x25/cfg_int_x25_enc.h,v $
 *------------------------------------------------------------------
 * X.25 encapsulation client interface parsing support
 *
 * April 1996, Gilbert Glick
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_x25_enc.h,v $
 * Revision 3.1.2.1  1996/04/11  23:47:39  gglick
 * CSCdi54514:  X.25 parser modularity files
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/11  23:40:01  gglick
 * Initial commit of X.25 parser modularization files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/************************************************************************
 *	...[options]
 * (encapsulation map and PVC command option external linkage)
 */

LINK_EXIT(ci_x25enc_mappvc_exit, no_alt);
EVAL	(x25enc_pvc_opt_eval, ci_x25enc_mappvc_exit,
         csb->uiarray[0]&=X25OPT_PVC_OPTIONS);
SPLIT	(x25enc_mappvc_opt_split, ci_x25enc_mappvc_exit,
	 x25enc_pvc_opt_eval,
	 OBJ(int,1), 0);

/************************************************************************
 *	...<protocol><address> [<proto2><addr2> [...]]] ...
 * (encapsulation map and PVC command protocol specification)
 *
 * For both map and PVC commands:
 *  csb->uiarray[N]/OBJ(paddr,N) = <protocol>/<address>, for 1 <= N <= 9
 *  OBJ(paddr,10) = X.121 address
 */

pdecl(x25enc_mappvc_protocols);		/* protocol specification re-entry */

X121ADDR(x25enc_mappvc_x121, x25enc_mappvc_opt_split, NONE,
	 OBJ(paddr,10), "Destination host address");

/* endpoint for datagram specification(s) */
NEQ	(x25enc_mappvc_proto_end, x25enc_mappvc_x121, NONE,
	 uiarray[0], 0);

EVAL	(x25enc_mappvc_xns_opt, x25enc_mappvc_protocols,
	 csb->uiarray[0]|=X25OPT_ENC_OPTIONS);
PROTOADDR(x25enc_mappvc_xns, x25enc_mappvc_xns_opt, NONE,
	  uiarray[9], OBJ(paddr,9), PMATCH_XNS);
NEQ	(x25enc_mappvc_xns_test, x25enc_mappvc_xns,
	 x25enc_mappvc_proto_end, uiarray[9], LINK_XNS);

EVAL	(x25enc_mappvc_vines_opt, x25enc_mappvc_protocols,
	 csb->uiarray[0]|=X25OPT_ENC_OPTIONS);
PROTOADDR(x25enc_mappvc_vines, x25enc_mappvc_vines_opt, NONE,
	  uiarray[8], OBJ(paddr,8), PMATCH_VINES);
NEQ	(x25enc_mappvc_vines_test, x25enc_mappvc_vines,
	 x25enc_mappvc_xns_test, uiarray[8], LINK_VINES);

EVAL	(x25enc_mappvc_novell_opt, x25enc_mappvc_protocols,
	 csb->uiarray[0]|=X25OPT_ENC_OPTIONS);
PROTOADDR(x25enc_mappvc_novell, x25enc_mappvc_novell_opt, NONE,
	  uiarray[7], OBJ(paddr,7), PMATCH_NOVELL);
NEQ	(x25enc_mappvc_novell_test, x25enc_mappvc_novell,
	 x25enc_mappvc_vines_test, uiarray[7], LINK_NOVELL);

EVAL	(x25enc_mappvc_ip_opt, x25enc_mappvc_protocols,
	 csb->uiarray[0]|=X25OPT_ENC_OPTIONS);
PROTOADDR(x25enc_mappvc_ip, x25enc_mappvc_ip_opt, NONE,
	  uiarray[6], OBJ(paddr,6), PMATCH_IP);
NEQ	(x25enc_mappvc_ip_test, x25enc_mappvc_ip,
	 x25enc_mappvc_novell_test, uiarray[6], LINK_IP);

EVAL	(x25enc_mappvc_decnet_opt, x25enc_mappvc_protocols,
	 csb->uiarray[0]|=X25OPT_ENC_OPTIONS);
PROTOADDR(x25enc_mappvc_decnet, x25enc_mappvc_decnet_opt, NONE,
	  uiarray[5], OBJ(paddr,5), PMATCH_DECNET);
NEQ	(x25enc_mappvc_decnet_test, x25enc_mappvc_decnet,
	 x25enc_mappvc_ip_test, uiarray[5], LINK_DECNET);

EVAL	(x25enc_mappvc_ctcp_opt, x25enc_mappvc_protocols,
	 csb->uiarray[0]|=(X25OPT_ENC_OPTIONS|X25OPT_PASSIVE)&~X25OPT_NVC);
PROTOADDR(x25enc_mappvc_ctcp, x25enc_mappvc_ctcp_opt, NONE,
	  uiarray[4], OBJ(paddr,4), PMATCH_COMPRESSED_TCP);
NEQ	(x25enc_mappvc_ctcp_test, x25enc_mappvc_ctcp,
	 x25enc_mappvc_decnet_test, uiarray[4], LINK_COMPRESSED_TCP);

EVAL	(x25enc_mappvc_clns_opt, x25enc_mappvc_protocols,
	 csb->uiarray[0]|=X25OPT_ENC_OPTIONS);
PROTOADDR(x25enc_mappvc_clns, x25enc_mappvc_clns_opt, NONE,
	  uiarray[3], OBJ(paddr,3), PMATCH_CLNS);
NEQ	(x25enc_mappvc_clns_test, x25enc_mappvc_clns,
	 x25enc_mappvc_ctcp_test, uiarray[3], LINK_CLNS);

EVAL	(x25enc_mappvc_apple_opt, x25enc_mappvc_protocols,
	 csb->uiarray[0]|=X25OPT_ENC_OPTIONS);
PROTOADDR(x25enc_mappvc_apple, x25enc_mappvc_apple_opt, NONE,
	  uiarray[2], OBJ(paddr,2), PMATCH_APPLETALK);
NEQ	(x25enc_mappvc_apple_test, x25enc_mappvc_apple,
	 x25enc_mappvc_clns_test, uiarray[2], LINK_APPLETALK);

EVAL	(x25enc_mappvc_apollo_opt, x25enc_mappvc_protocols,
	 csb->uiarray[0]|=X25OPT_ENC_OPTIONS);
PROTOADDR(x25enc_mappvc_apollo, x25enc_mappvc_apollo_opt, NONE,
	  uiarray[1], OBJ(paddr,1), PMATCH_APOLLO);
NEQ	(x25enc_mappvc_apollo_test, x25enc_mappvc_apollo,
	 x25enc_mappvc_apple_test, uiarray[1], LINK_APOLLO);

/* entry point for datagram protocol specification(s) */
NOP	(x25enc_mappvc_protocols, x25enc_mappvc_apollo_test, no_alt);

SET	(x25enc_mappvc_bridge_opt, x25enc_mappvc_x121,
	 uiarray[0], X25OPT_ENC_OPTIONS & ~X25OPT_METHOD);
PROTONAME(x25enc_mappvc_bridge, x25enc_mappvc_bridge_opt,
	  x25enc_mappvc_protocols, uiarray[1], PMATCH_BRIDGE);

SET     (x25enc_mappvc_proto_set, x25enc_mappvc_bridge, uiarray[0], 0);
NOP     (x25enc_mappvc_proto_start, x25enc_mappvc_proto_set, ALTERNATE);

#undef	ALTERNATE
#define	ALTERNATE	x25enc_mappvc_proto_start
LINK_POINT(x25enc_mappvc_enter, ALTERNATE);
#undef ALTERNATE
#define ALTERNATE	NONE

/************************************************************************
 *	x25 remote-red <host-ipaddr> remote-black <blacker-ipaddr>
 * paddr,1	= <host-ipaddr>
 * paddr,2	= <blacker-ipaddr>
 */

EOLS	(x25enc_bfe_rem_eol, x25enc_configure_bfe, X25ENC_BFE_REMOTE);
IPADDR	(x25enc_bfe_rem_blackip, x25enc_bfe_rem_eol, no_alt,
	 OBJ(paddr,2), "IP address of remote BFE");
KEYWORD	(x25enc_bfe_rem_black, x25enc_bfe_rem_blackip, no_alt,
	 "remote-black", "Specify IP address of remote BFE", PRIV_CONF);
NOPREFIX(x25enc_bfe_rem_noprefix, x25enc_bfe_rem_black, x25enc_bfe_rem_eol);
IPADDR	(x25enc_bfe_rem_hostip, x25enc_bfe_rem_noprefix, no_alt,
	 OBJ(paddr,1), "IP address of host");
NVGENS	(x25enc_bfe_rem_nvcheck, x25enc_bfe_rem_hostip,
	 x25enc_configure_bfe, X25ENC_BFE_REMOTE);
KEYWORD	(x25enc_bfe_rem, x25enc_bfe_rem_nvcheck, NONE,
	 "remote-red", "Establish BFE Emergency Mode mapping",
	 PRIV_CONF);

/************************************************************************
 *	x25 bfe-decision {yes | no | ask}
 */

EOLS	(x25enc_bfe_dec_eol, x25enc_configure_bfe, X25ENC_BFE_DECISION);
KEYWORD_ID (x25enc_bfe_dec_ask, x25enc_bfe_dec_eol, no_alt,
	    OBJ(int,1), X25ENC_BFEDEC_ASK,
	    "ask", "Prompt administrator for emergency mode decision",
	    PRIV_CONF);
KEYWORD_ID (x25enc_bfe_dec_no, x25enc_bfe_dec_eol, x25enc_bfe_dec_ask,
	    OBJ(int,1), X25ENC_BFEDEC_NO,
	    "no", "Router will not participate in emergency mode decision",
	    PRIV_CONF);
KEYWORD_ID (x25enc_bfe_dec_yes, x25enc_bfe_dec_eol, x25enc_bfe_dec_no,
	    OBJ(int,1), X25ENC_BFEDEC_YES,
	    "yes", "Router will participate in emergency mode decision",
	    PRIV_CONF);

NOPREFIX (x25enc_bfe_dec_nocheck, x25enc_bfe_dec_yes, x25enc_bfe_dec_eol);
KEYWORD	(x25enc_bfe_dec, x25enc_bfe_dec_nocheck, x25enc_bfe_rem,
	 "bfe-decision", "Specify BFE Emergency Mode participation",
	 PRIV_CONF);

/************************************************************************
 *	x25 bfe-emergency {never | always | decision}
 */

EOLS	(x25enc_bfe_em_eol, x25enc_configure_bfe, X25ENC_BFE_EMER);
KEYWORD_ID (x25enc_bfe_em_decision, x25enc_bfe_em_eol, no_alt,
	    OBJ(int,1), X25ENC_BFEEM_DECISION,
	    "decision", "Enter Emergency Mode after receiving Emergency mode "
	    "window open packet",
	    PRIV_CONF);
KEYWORD_ID (x25enc_bfe_em_always, x25enc_bfe_em_eol, x25enc_bfe_em_decision,
	    OBJ(int,1), X25ENC_BFEEM_ALWAYS,
	    "always", "Enter BFE Emergency Mode when directed by BFE",
	    PRIV_CONF);
KEYWORD_ID (x25enc_bfe_em_never, x25enc_bfe_em_eol, x25enc_bfe_em_always,
	    OBJ(int,1), X25ENC_BFEEM_NEVER,
	    "never", "Never use BFE Emergency Mode",
	    PRIV_CONF);

NOPREFIX (x25enc_bfe_em_nocheck, x25enc_bfe_em_never, x25enc_bfe_em_eol);
KEYWORD	(x25enc_bfe_em, x25enc_bfe_em_nocheck, x25enc_bfe_dec,
	 "bfe-emergency", "Set Blacker Front End Emergency Mode",
	 PRIV_CONF);

/* Don't allow BFE commands for non-BFE interfaces. */
ASSERT	(x25enc_bfe, x25enc_bfe_em, no_alt,
	 reg_invoke_x25enc_bfe(csb->interface->hwptr));

/************************************************************************
 *	x25 nvc <count>
 */

PARAMS_KEYONLY	(x25enc_nvc, x25enc_bfe, "nvc",
		 OBJ(int,1), 1, X25ENC_MAXNVC_V,
		 x25enc_configure, X25ENC_NVC,
		 "Set maximum VCs simultaneously open to one host "
		 "per protocol",
		 "SVCs", PRIV_CONF);

/************************************************************************
 *	x25 ip-precedence
 */

EOLS	(x25enc_ipprec_eol, x25enc_configure, X25ENC_IPTOS);
KEYWORD	(x25enc_ipprec, x25enc_ipprec_eol, x25enc_nvc,
	 "ip-precedence", "Open one virtual circuit for each IP TOS",
	 PRIV_CONF);

/************************************************************************
 *	x25 hold-vc-timer <minutes>
 */

PARAMS_KEYONLY	(x25enc_hvctimer, x25enc_ipprec, "hold-vc-timer",
		 OBJ(int,1), 0, 1000,
		 x25enc_configure, X25ENC_HOLDTIME,
		 "Set time to prevent calls to a failed destination",
		 "Minutes; 0 to always try calls", PRIV_CONF);

/************************************************************************/

KEYWORD (x25enc_keyword, x25enc_hvctimer, NONE,
	 "x25", "", PRIV_CONF | PRIV_SUBIF | PRIV_DUPLICATE);

ASSERT (x25enc_keyword_test, x25enc_keyword, ALTERNATE,
	hwidb_is_x25_inline(csb->interface->hwptr));


#undef	ALTERNATE
#define	ALTERNATE	x25enc_keyword_test
