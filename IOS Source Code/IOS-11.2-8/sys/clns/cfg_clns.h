/* $Id: cfg_clns.h,v 3.3.20.1 1996/08/12 16:00:38 widmer Exp $
 * $Source: /release/112/cvs/Xsys/clns/cfg_clns.h,v $
 *------------------------------------------------------------------
 * C F G _ C L N S . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_clns.h,v $
 * Revision 3.3.20.1  1996/08/12  16:00:38  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/02/01  23:32:59  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  08:51:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:06:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:54  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:02:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 * clns template-alias name expression
 * no clns template-alias name
 *
 * OBJ(string,1) = name
 * OBJ(string,2) = template
 */
EOLNS    (clns_tempalias_eol, clns_template_alias_cmd);
STRING  (clns_tempalias_expr, clns_tempalias_eol, no_alt,
	 OBJ(string,2), "Address template");
NOPREFIX(clns_tempalias_noprefix, clns_tempalias_expr, clns_tempalias_eol);
STRING  (clns_tempalias_name, clns_tempalias_noprefix, no_alt,
	 OBJ(string,1), "Template name");
NVGENNS	(clns_tempalias_nvgen, clns_tempalias_name,
	 clns_template_alias_cmd);
KEYWORD (clns_filter_alias, clns_tempalias_nvgen, NONE,
	 "template-alias", "Define CLNS template alias", PRIV_CONF);

/******************************************************************************
 * clns filter-expr name {term | not term  | term {and | or | xor} term }
 * no clns filter-expr name
 *  term := {name | (source | src} name | destination name}
 *
 * OBJ(string,1) = filter name
 * OBJ(string,2) = parm 1 name
 * OBJ(string,3) = parm 2 name
 * OBJ(int,1) = operator (FILTER_OP_AND, FILTER_OP_OR,
 *                        FILTER_OP_XOR, FILTER_OP_NOT)
 * OBJ(int,2) = parm 1 qualifier (FILTER_TERM_SRC, FILTER_TERM_DST)
 * OBJ(int,3) = parm 2 qualifier
 * OBJ(int,4) = protocol (CLNS)
 */
EOLNS    (clns_filexp_eol, filter_expr_cmd);
STRING  (clns_filexp_parm2, clns_filexp_eol, no_alt,
	 OBJ(string,3), "Expression or set name");
TEST_MULTIPLE_FUNCS(clns_filexp_ambig2, clns_filexp_parm2, clns_filexp_eol,
		    NONE);
SET     (clns_filexp_set2, clns_filexp_ambig2,
	 OBJ(int,3), FILTER_UNKNOWN);
KEYWORD_ID(clns_filexp_dst2, clns_filexp_ambig2, clns_filexp_set2,
	   OBJ(int,3), FILTER_TERM_DST,
	   "destination", "Test destination address", PRIV_CONF);
KEYWORD_ID(clns_filexp_src2a, clns_filexp_ambig2, clns_filexp_dst2,
	   OBJ(int,3), FILTER_TERM_SRC,
	   "src", "Test source address", PRIV_CONF);
KEYWORD_ID(clns_filexp_src2, clns_filexp_ambig2, clns_filexp_src2a,
	   OBJ(int,3), FILTER_TERM_SRC,
	   "source", "Test source address", PRIV_CONF);
KEYWORD_ID(clns_filexp_xor, clns_filexp_src2, clns_filexp_eol,
	   OBJ(int,1), FILTER_OP_XOR,
	   "xor", "logical xor", PRIV_CONF);
KEYWORD_ID(clns_filexp_or, clns_filexp_src2, clns_filexp_xor,
	   OBJ(int,1), FILTER_OP_OR,
	   "or", "logical or", PRIV_CONF);
KEYWORD_ID(clns_filexp_and, clns_filexp_src2, clns_filexp_or,
	   OBJ(int,1), FILTER_OP_AND,
	   "and", "logical and", PRIV_CONF);
TESTVAR (clns_filexp_nottest, clns_filexp_eol, clns_filexp_and, NONE, NONE,
	 NONE, OBJ(int,1), FILTER_OP_NOT);
STRING  (clns_filexp_parm1, clns_filexp_nottest, no_alt,
	 OBJ(string,2), "Expression or set name");
TEST_MULTIPLE_FUNCS(clns_filexp_ambig1, clns_filexp_parm1, clns_filexp_nottest,
		    NONE);
SET     (clns_filexp_set1, clns_filexp_ambig1, OBJ(int,2), FILTER_UNKNOWN);
KEYWORD_ID(clns_filexp_dst1, clns_filexp_ambig1, clns_filexp_set1,
	   OBJ(int,2), FILTER_TERM_DST,
	   "destination", "Test destination address", PRIV_CONF);
KEYWORD_ID(clns_filexp_src1a, clns_filexp_ambig1, clns_filexp_dst1,
	   OBJ(int,2), FILTER_TERM_SRC,
	   "src", "Test source address", PRIV_CONF);
KEYWORD_ID(clns_filexp_src1, clns_filexp_ambig1, clns_filexp_src1a,
	   OBJ(int,2), FILTER_TERM_SRC,
	   "source", "Test source address", PRIV_CONF);
KEYWORD_ID(clns_filexp_not, clns_filexp_src1, clns_filexp_src1,
	   OBJ(int,1), FILTER_OP_NOT,
	   "not", "Negate expression", PRIV_CONF);
NOPREFIX(clns_filexp_noprefix, clns_filexp_not, clns_filexp_eol);
STRING  (clns_filexp_name, clns_filexp_noprefix, no_alt,
	 OBJ(string,1), "Name of filter expression");
NVGENNS  (clns_filexp_nvgen, clns_filexp_name, filter_expr_cmd);
KEYWORD_ID(clns_filter_expr, clns_filexp_nvgen, clns_filter_alias,
	   OBJ(int,4), FILTER_PROTO_CLNS,
	   "filter-expr", "Define CLNS filter expression", PRIV_CONF);


/******************************************************************************
 * clns filter-set name {[permit] | deny} template
 * no clns filter-set name
 *
 * OBJ(string,1) = filter name
 * OBJ(string,2) = address template or template alias name
 * OBJ(int,1) = TRUE if permit
 */

EOLNS	(clns_filset_eol, clns_filter_set_cmd);
STRING  (clns_filset_template, clns_filset_eol, no_alt,
	 OBJ(string,2), "Address template to filter");
TEST_MULTIPLE_FUNCS(clns_filset_test, clns_filset_template, clns_filset_eol,
		    NONE);
KEYWORD_ID(clns_filset_deny, clns_filset_template, clns_filset_test,
	   OBJ(int,1), FALSE,
	   "deny", "Deny matching addresses", PRIV_CONF);
KEYWORD (clns_filset_permit, clns_filset_template, clns_filset_deny,
	 "permit", "Permit matching addresses", PRIV_CONF);
SET     (clns_filset_set1, clns_filset_permit, OBJ(int,1), TRUE);
NOPREFIX(clns_filset_noprefix, clns_filset_set1, clns_filset_eol);
STRING  (clns_filset_name, clns_filset_noprefix, no_alt,
	 OBJ(string,1), "Name of filter set");
NVGENNS  (clns_filset_nvgen, clns_filset_name, clns_filter_set_cmd);
KEYWORD	(clns_filter_set, clns_filset_nvgen, clns_filter_expr,
	 "filter-set", "Define CLNS filter set", PRIV_CONF);


/******************************************************************************
 * [no] clns want-erpdu
 *
 */

EOLS	(clns_want_eol, clns_commands, CLNS_WANT_ERPDU);
KEYWORD	(clns_want, clns_want_eol, clns_filter_set,
	 "want-erpdu", "Request error PDUs on packets sourced by router",
	 PRIV_CONF);

/******************************************************************************
 * [no] clns security pass-through
 *
 */

EOLS	(clns_security_eol, clns_commands, CLNS_SECURITY_PASSTHRU);
KEYWORD	(clns_security, clns_security_eol, clns_want,
	 "security pass-through",
	 "Do not reject packet with security options set",
	 PRIV_CONF);

/******************************************************************************
 * [no] clns source-route
 *
 */
#ifdef	NOT_ALLOWED_YET	/* Commented out of 9.1 sources - tcs */
EOLS	(clns_source_eol, clns_commands, CLNS_SOURCEROUTE);
KEYWORD	(clns_source, clns_source_eol, clns_security,
	 "source-route", "Source route", PRIV_CONF);
#else	NOT_ALLOWED_YET
NOP (clns_source, clns_security, NONE);
#endif	NOT_ALLOWED_YET

/******************************************************************************
 * clns routing [<addr>]
 * no clns routing
 *
 */

EOLS (clns_routing_eol, clns_commands, CLNS_ROUTING);
NVGENS	(clns_routing_nvgen, clns_routing_eol, clns_commands, CLNS_ROUTING);
KEYWORD	(clns_routing_kwd, clns_routing_nvgen, NONE,
	 "routing", "Enable clns routing", PRIV_CONF);
TEST_MULTIPLE_FUNCS(clns_routing, clns_routing_kwd, NONE, clns_source);


/******************************************************************************
 * clns packet-lifetime <num>
 * no clns packet-lifetime
 */
PARAMS_KEYONLY( clns_packet, clns_routing, "packet-lifetime",
	        OBJ(int,1), 1, CLNS_MAX_LIFETIME , clns_commands,
	        CLNS_PACKET_LIFETIME,
	        "Define initial lifetime for locally generated packets",
	        "Initial lifetime for locally generated packets", PRIV_CONF);

/******************************************************************************
 * clns net <addr>
 * no clns net
 *
 * The old version of this command was:
 *	clns nsap <addr>, but is no longer used and should never be
 *			    generated.
 * Note that there is both a global and interface version
 * of this command.
 *
 * (*OBJ(paddr,1)) = <addr>
 */

EOLS	(clns_nsap_eol, clns_commands, CLNS_NSAP);
NSAPADDR(clns_nsap_addr, clns_nsap_eol, no_alt,
	 OBJ(paddr,1), "CLNS address or host name");
NVGENS	(clns_nsap_nvgen, clns_nsap_addr, clns_commands, CLNS_NSAP);
KEYWORD	(clns_nsap, clns_nsap_nvgen, clns_packet,
	 "nsap", "", PRIV_CONF | PRIV_HIDDEN | PRIV_NONVGEN);
KEYWORD	(clns_net, clns_nsap_nvgen, clns_nsap, "net",
	 "Define a Network Entity Title (NET)", PRIV_CONF);

/******************************************************************************
 * clns holding-time <time>
 * no clns holding-time
 *
 */
PARAMS_KEYONLY( clns_holding_time, clns_net, "holding-time",
	        OBJ(int,1), 1, CLNS_MAX_HOLDING_TIMER,
	        clns_commands, CLNS_HOLDING_TIMER,
	        "Holdtime advertised in ESH/ISH PDUs",
                "Holdtime advertised in ESH/ISH PDUs", PRIV_CONF);

/******************************************************************************
 * clns configuration-time <time>
 * no clns configuration-time
 *
 */
PARAMS_KEYONLY( clns_config_time, clns_holding_time, "configuration-time",
		OBJ(int,1), 1, CLNS_MAX_CONFIG_TIMER,
		clns_commands, CLNS_CONFIG_TIMER,
	        "Define the rate at which ESH and ISH packets are sent",
	        "Rate at which ESH and ISH packets are sent", PRIV_CONF);

KEYWORD	(clns, clns_config_time, ALTERNATE,
	 "clns", "Global CLNS configuration subcommands", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	clns
