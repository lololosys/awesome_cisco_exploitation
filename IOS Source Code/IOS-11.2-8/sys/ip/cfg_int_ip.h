/* $Id: cfg_int_ip.h,v 3.4.42.5 1996/08/29 23:09:09 gchristy Exp $
 * $Source: /release/112/cvs/Xsys/ip/cfg_int_ip.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ I P . H
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_ip.h,v $
 * Revision 3.4.42.5  1996/08/29  23:09:09  gchristy
 * CSCdi67444:  IP NACL: resolve no ip access-group ambiguity
 * Branch: California_branch
 * - Make access-list names "in" and "out" illegal.
 * - Fix parse chain to do the right thing.
 *
 * Revision 3.4.42.4  1996/08/15  00:06:10  gchristy
 * CSCdi66015:  A warning display, ambiguous command on no ip access-group
 * in
 * Branch: California_branch
 * - Resolve parse chain ambiguity.
 *
 * Revision 3.4.42.3  1996/08/13  21:31:43  dawnli
 * CSCdi65746:  ip directed broadcast command requires access-list
 * Branch: California_branch
 *
 * Revision 3.4.42.2  1996/08/12  16:02:46  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4.42.1  1996/05/04  01:01:46  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - ARP, IP protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on CRB/IRB is enabled or not.
 *
 * Constrained Multicast Flooding
 * - Convert tbridge flood array to a linked list of tbifd's.
 *
 * Name Access List
 *
 * Revision 3.4  1995/12/13  22:57:17  lol
 * CSCdi39791:  access-lists applied by tacacs+ dont work when
 * fast-switched
 *
 * Revision 3.3  1995/11/27  21:24:42  widmer
 * CSCdi43607:  Add keyword_option macro to parse multiple keywords
 * Also create permit_deny macro using keyword_option macro to parse
 * permit/deny keywords.
 * Change "ip security multilevel" command to use keyword_option macro.
 *
 * Revision 3.2  1995/11/17  09:32:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:53:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:03:55  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/08/03  02:46:34  myeung
 * CSCdi38147:  Help for ip ospf authentication-key command screwed up
 *
 * Revision 2.1  1995/07/01  12:55:35  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:05:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(ci_ip_extend_here, NONE);

/************************************************************************/
/****  BEGIN IP SECURITY  ****/

/* Note: the same level/authorities code is used for all (four) places
 * it can appear, switching off and on a variable (OBJ(int,5))
 */

pdecl(ci_ip_secur_auth);
pdecl(ci_ip_secur_auth_visible);
pdecl(ci_ip_secur_level);

/************************************************************************
 *	no ip security
 */

EOLS	(ci_ip_secur_disable_eol, ip_security_command, P_IPSEC_DEFAULT);
TESTVAR	(ci_ip_secur_disable, ci_ip_secur_disable_eol, no_alt,
	 NONE, NONE, NONE, sense, FALSE);

/************************************************************************
 *	[no] ip security { eso-min | eso-max | aeso } <source>
 *		<compartment-bits>
 */

EOLNS	(ci_ip_secur_eso_eol, ip_security_ext_command);
HEXDATA (ci_ip_secur_compart, ci_ip_secur_eso_eol, no_alt,
	 OBJ(string,1), OBJ(int,3), "Compartment bits in hex",
	 IP_SEC_MAX_COMPARTMENT_BYTES);
NOPREFIX (ci_ip_secur_eso_noprefix, ci_ip_secur_compart, ci_ip_secur_eso_eol);
NUMBER	(ci_ip_secur_source, ci_ip_secur_eso_noprefix, no_alt,
	 OBJ(int,2), 0, 255, "ESO source");
NVGENNS	(ci_ip_secur_eso_nvgen, ci_ip_secur_source,
	 ip_security_ext_command);
KEYWORD_ID	(ci_ip_secur_aeso, ci_ip_secur_eso_nvgen, ci_ip_secur_disable,
		 OBJ(int,1), IP_SEC_AESO,
		 "aeso", "Specify AESO sources",
		 PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID	(ci_ip_secur_eso_max, ci_ip_secur_eso_nvgen, ci_ip_secur_aeso,
		 OBJ(int,1), IP_SEC_ESO_MAX,
		 "eso-max", "Specify maximum sensitivity level",
		 PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID	(ci_ip_secur_eso_min, ci_ip_secur_eso_nvgen,
		 ci_ip_secur_eso_max,
		 OBJ(int,1), IP_SEC_ESO_MIN,
		 "eso-min", "Specify minimum sensitivity level",
		 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 *	[no] ip security strip
 */

EOLS	(ci_ip_secur_strip_eol, ip_security_command, P_IPSECSTYLE_STRIP);
KEYWORD	(ci_ip_secur_strip, ci_ip_secur_strip_eol, ci_ip_secur_eso_min,
	 "strip", "Remove any basic security options on outgoing packets",
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 *	[no] ip security implicit-labelling [<level> <auth>...]
 *
 * OBJ(int,1) = IPSO Level Bit Pattern
 * OBJ(int,2) = bitwise-OR IPSO Authority Patterns
 *
 */

EOLS	(ci_ip_secur_ilabel_eol, ip_security_command, P_IPSECSTYLE_IMPLICIT);
/* Transition to 'ci_ip_secur_level to handle <level> <auth>
 * A check after <level> determines if the user omitted the <level> <auth>
 * keywords.
 */
SET	(ci_ip_secur_ilabel_set, ci_ip_secur_level, OBJ(int,5), 3);
NVGENS	(ci_ip_secur_ilabel_nvgen, ci_ip_secur_ilabel_set,
	 ip_security_command, P_IPSECSTYLE_IMPLICIT);
KEYWORD	(ci_ip_secur_ilabel, ci_ip_secur_ilabel_nvgen, ci_ip_secur_strip,
	 "implicit-labelling", "Accept datagrams without a security option",
	 PRIV_CONF|PRIV_SUBIF);
/************************************************************************
 *       [no] ip security ignore-cipso
 */

EOLS	(ci_ip_secur_cipso_eol, ip_security_command, P_IPSECSTYLE_IGNORECIPSO);
KEYWORD (ci_ip_secur_cipso, ci_ip_secur_cipso_eol, ci_ip_secur_ilabel,
	 "ignore-cipso", "Ignore all incoming CIPSO fields",
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 *	[no] ip security ignore-authorities
 */

EOLS	(ci_ip_secur_iauth_eol, ip_security_command, P_IPSECSTYLE_IGNOREAUTH);
KEYWORD	(ci_ip_secur_iauth, ci_ip_secur_iauth_eol, ci_ip_secur_cipso,
	 "ignore-authorities", "Ignore all incoming authorities fields",
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 *	[no] ip security first
 */

EOLS	(ci_ip_secur_first_eol, ip_security_command, P_IPSECSTYLE_FIRST);
KEYWORD	(ci_ip_secur_first, ci_ip_secur_first_eol, ci_ip_secur_iauth,
	 "first", "Ensure security option always first on outgoing packets",
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 *	[no] ip security reserved-allowed
 */

EOLS   (ci_ip_secur_resallow_eol, ip_security_command, P_IPSEC_ALLOW_RESERVED);
KEYWORD(ci_ip_secur_resallow, ci_ip_secur_resallow_eol, ci_ip_secur_first,
	"reserved-allowed",
	"Admit packets with security levels currently reserved",
	PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 *	[no] ip security extended-ignored
 */

EOLS	(ci_ip_secur_exignore_eol, ip_security_command,
	 P_IPSECSTYLE_IGNOREEXT);
KEYWORD	(ci_ip_secur_exignore, ci_ip_secur_exignore_eol, ci_ip_secur_resallow,
	 "extended-ignored",
	 "Ignore extended security options on incoming packets",
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 *	[no] ip security extended-allowed
 */

EOLS	(ci_ip_secur_exallow_eol, ip_security_command, P_IPSECSTYLE_EXTSECOK);
KEYWORD	(ci_ip_secur_exallow, ci_ip_secur_exallow_eol, ci_ip_secur_exignore,
	 "extended-allowed",
	 "Process packets with an extended security option",
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 *	[no] ip security add
 */

EOLS	(ci_ip_secur_add_eol, ip_security_command, P_IPSECSTYLE_ADD);
KEYWORD	(ci_ip_secur_add, ci_ip_secur_add_eol, ci_ip_secur_exallow,
	 "add", "Ensure all outgoing packets have a basic security option",
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 *	ip security dedicated <level> <auth>...
 *
 *	ip security multilevel <level> <auth>...
 *
 *
 * Note that for multilevel, OBJ(int,1)/OBJ(int,2) are for the second
 * pair, while OBJ(int,3)/OBJ(int,4) are for the first.  For dedicated,
 * only OBJ(int,1)/OBJ(int,2) are used.
 *
 * OBJ(int,1) = IPSO Level Bit Pattern
 * OBJ(int,2) = bitwise-OR IPSO Authority Patterns
 * OBJ(int,3) = Multilevel 1st Level
 * OBJ(int,4) = Multilevel 1st Authority
 * OBJ(int,5) = USED INTERNALLY (0 = dedicated; 1 = ml part 1; 2 = ml part 2;
 *			   3 = implict-labelling)
 */

EOLS	(ci_ip_secur_ded_eol, ip_security_command, P_IPSECMODE_DEDICATED);
EOLS	(ci_ip_secur_ml_eol, ip_security_command, P_IPSECMODE_MULTILEVEL);
SPLIT	(ci_ip_secur_dm_eol2, ci_ip_secur_ilabel_eol, ci_ip_secur_ml_eol,
	 OBJ(int,5), 3);
SPLIT	(ci_ip_secur_dm_eol, ci_ip_secur_ded_eol, ci_ip_secur_dm_eol2,
	 OBJ(int,5), 0);

/* "to"
 * Begin  second security level for 'ip security multilevel'.
 */
EVAL	(ci_ip_secur_save, ci_ip_secur_level,
	 (GETOBJ(int,3) = GETOBJ(int,1),
	  GETOBJ(int,4) = GETOBJ(int,2),
	  GETOBJ(int,1) = 0,
	  GETOBJ(int,2) = 0,
	  GETOBJ(int,5) = 2));
KEYWORD	(ci_ip_secur_to, ci_ip_secur_save, no_alt,
	 "to", "Begin second part of security specification",
	 PRIV_CONF|PRIV_SUBIF);
SPLIT	(ci_ip_secur_gotauth, no_alt, ci_ip_secur_dm_eol,
	 OBJ(int,2), AUTH_NONE);
SPLIT	(ci_ip_secur_auth_noalt, ci_ip_secur_to, ci_ip_secur_gotauth,
	 OBJ(int,5), 1);

/*
 * { genser | siop-esi | sci | nsa | doe }
 *
 * Authority security levels.
 *
 */

static keyword_options ip_secur_options[] = {
    { "doe", NULL, AUTH_DOE },
    { "genser", NULL, AUTH_GENSER },
    { "nsa", NULL, AUTH_NSA },
    { "sci", NULL, AUTH_SCI },
    { "siop-esi", NULL,  AUTH_SIOP_ESI },
    { NULL, NULL, 0 }
};

static keyword_options ip_secur_options_hidden[] = {
    { "dca", NULL, AUTH_GENSER },
    { "dsccs-criticom", NULL, AUTH_NSA },
    { "dia", NULL, AUTH_SCI },
    { "jcs", NULL, AUTH_SIOP_ESI },
    { NULL, NULL, 0 }
};

KEYWORD_OPTIONS(ci_ip_secur_auth_hidden, ci_ip_secur_auth_visible,
		ci_ip_secur_auth_noalt, ip_secur_options_hidden,
		OBJ(int,2), PRIV_HIDDEN | PRIV_NONVGEN | PRIV_CONF | PRIV_SUBIF,
		KEYWORD_OR_SET | KEYWORD_TEST);
KEYWORD_OPTIONS(ci_ip_secur_auth_visible, ci_ip_secur_auth_visible,
		ci_ip_secur_auth_hidden, ip_secur_options,
		OBJ(int,2), PRIV_CONF | PRIV_SUBIF,
		KEYWORD_OR_SET | KEYWORD_TEST);

SET	(ci_ip_secur_auth, ci_ip_secur_auth_visible, OBJ(int,2), AUTH_NONE);

/* { reserved1 | unclassified | reserved2 | reserved3 | confidential |
 *	secret | topsecret | reserved4 }
 *
 * Security level.
 */

EQUAL	(ci_ip_secur_no_alt, ci_ip_secur_ilabel_eol, no_alt, OBJ(int,5), 3);

KEYWORD_ID(ci_ip_secur_level_r4, ci_ip_secur_auth, ci_ip_secur_no_alt,
	   OBJ(int,1), SECLVL_RESERVED4,
	   "Reserved4", "", PRIV_CONF|PRIV_SUBIF | PRIV_HIDDEN);

KEYWORD_ID(ci_ip_secur_level_ts, ci_ip_secur_auth, ci_ip_secur_level_r4,
	   OBJ(int,1), SECLVL_TOPSECRET,
	   "TopSecret", "", PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(ci_ip_secur_level_se, ci_ip_secur_auth, ci_ip_secur_level_ts,
	   OBJ(int,1), SECLVL_SECRET,
	   "Secret", "", PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(ci_ip_secur_level_co, ci_ip_secur_auth, ci_ip_secur_level_se,
	   OBJ(int,1), SECLVL_CONFIDENTIAL,
	   "Confidential", "", PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(ci_ip_secur_level_r3, ci_ip_secur_auth, ci_ip_secur_level_co,
	   OBJ(int,1), SECLVL_RESERVED3,
	   "Reserved3", "", PRIV_CONF|PRIV_SUBIF | PRIV_HIDDEN);

KEYWORD_ID(ci_ip_secur_level_r2, ci_ip_secur_auth, ci_ip_secur_level_r3,
	   OBJ(int,1), SECLVL_RESERVED2,
	   "Reserved2", "", PRIV_CONF|PRIV_SUBIF | PRIV_HIDDEN);

KEYWORD_ID(ci_ip_secur_level_uc, ci_ip_secur_auth, ci_ip_secur_level_r2,
	   OBJ(int,1), SECLVL_UNCLASSIFIED,
	   "Unclassified", "", PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(ci_ip_secur_level_r1, ci_ip_secur_auth, ci_ip_secur_level_uc,
	   OBJ(int,1), SECLVL_RESERVED1,
	   "Reserved1", "", PRIV_CONF|PRIV_SUBIF | PRIV_HIDDEN);

NOP	(ci_ip_secur_level, ci_ip_secur_level_r1, NONE);
NVGENS	(ci_ip_secur_ml_nvgen, ci_ip_secur_level,
	 ip_security_command, P_IPSECMODE_MULTILEVEL);
NVGENS	(ci_ip_secur_ded_nvgen, ci_ip_secur_level,
	 ip_security_command, P_IPSECMODE_DEDICATED);

/* { multilevel | dedicated } */
KEYWORD_ID(ci_ip_secur_ml, ci_ip_secur_ml_nvgen, NONE,
	   OBJ(int,5), 1,
	   "multilevel", "Set interface to range of levels and authorities",
	   PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(ci_ip_secur_ded, ci_ip_secur_ded_nvgen, ci_ip_secur_ml,
	   OBJ(int,5), 0,
	   "dedicated", "Set interface to specified level and authorities",
	   PRIV_CONF|PRIV_SUBIF);

EQUAL	(ci_ip_secur_noprefix, ci_ip_secur_ded, ci_ip_secur_add,
	 sense, TRUE);

/* "security" */
KEYWORD	(ci_ip_security, ci_ip_secur_noprefix, ci_ip_extend_here,
	 "security", "DDN IP Security Option", PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 *	ip mtu <bytes>
 *	no ip mtu [<bytes>]
 */
PARAMS_KEYONLY	(ci_ip_mtu, ci_ip_security,
		 "mtu",	OBJ(int,1), 64, 1000000,
		 ip_mtu_command, IP_MTU,
		 "Set IP Maximum Transmission Unit", "MTU (bytes)",
		 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 *	[no] ip accounting
 */

EOLNS	(ci_ip_acct_eol, ip_accounting_command);
SET	(ci_ip_acct_noparm, ci_ip_acct_eol, OBJ(int,1),
	 NUM_IP_ACCOUNTING_TYPES);
SPLIT	(ci_ip_acct_nvcheck, ci_ip_acct_noparm, ci_ip_acct_eol,
	 nvgen, TRUE);
SPLIT	(ci_ip_acct_no2, ci_ip_acct_noparm, ci_ip_acct_nvcheck,
	 sense, FALSE);
KEYWORD_ID(ci_ip_acct_violation, ci_ip_acct_eol, ci_ip_acct_no2,
	   OBJ(int,1), IP_ACCOUNTING_VIOLATIONS,
	   "access-violations",
	   "Account for IP packets violating access lists on this interface",
	   PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(ci_ip_acct_opak, ci_ip_acct_eol, ci_ip_acct_violation,
	   OBJ(int,1), IP_ACCOUNTING_OUTPAKS,
	   "output-packets", "Account for IP packets output on this interface",
	   PRIV_CONF|PRIV_SUBIF);
KEYWORD	(ci_ip_acct, ci_ip_acct_opak, ci_ip_mtu,
	 "accounting", "Enable IP accounting on this interface",
	 PRIV_CONF);

/************************************************************************
 *	[no] ip probe proxy
 */

EOLS	(ci_ip_probe_eol, probe_command, IP_PROBE);
KEYWORD	(ci_ip_probe_proxy, ci_ip_probe_eol, no_alt,
	 "proxy", "Proxy support", PRIV_CONF|PRIV_SUBIF);
KEYWORD	(ci_ip_probe, ci_ip_probe_proxy, ci_ip_acct,
	 "probe", "Enable HP Probe support", PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 *	[no] ip proxy-arp
 */

EOLNS	(ci_ip_proxyarp_eol, ip_proxyarp_command);
KEYWORD	(ci_ip_proxyarp, ci_ip_proxyarp_eol, ci_ip_probe,
	 "proxy-arp", "Enable proxy ARP", PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 *	[no] ip directed-broadcast [<list>]
 */

EOLNS	(ci_ip_dbcast_eol, ip_directbcast_command);
NUMBER	(ci_ip_dbcast_list, ci_ip_dbcast_eol, ci_ip_dbcast_eol, OBJ(int,1),
	 MINFASTACC,
	 MAXSLOWACC, "A standard IP access list number");
NOPREFIX (ci_ip_dbcast_no, ci_ip_dbcast_list, ci_ip_dbcast_eol);
NVGENNS (ci_ip_dbcast_nvgen, ci_ip_dbcast_no, ip_directbcast_command);
KEYWORD	(ci_ip_dbcast, ci_ip_dbcast_nvgen, ci_ip_proxyarp,
	 "directed-broadcast", "Enable forwarding of directed broadcasts",
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 *	[no] ip unreachables
 */

EOLS	(ci_ip_unreach_eol, ip_unreachable_command, IP_UNREACHABLE);
KEYWORD	(ci_ip_unreach, ci_ip_unreach_eol, ci_ip_dbcast,
	 "unreachables", "Enable sending ICMP Unreachable messages",
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 *	[no] ip redirects
 */

EOLNS	(ci_ip_redirects_eol, ip_redirects_command);
KEYWORD	(ci_ip_redirects, ci_ip_redirects_eol, ci_ip_unreach,
	 "redirects", "Enable sending ICMP Redirect messages",
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 *	[no] ip mask-reply
 */

EOLNS	(ci_ip_maskreply_eol, ip_maskreply_command);
KEYWORD	(ci_ip_maskreply, ci_ip_maskreply_eol, ci_ip_redirects,
	 "mask-reply", "Enable sending ICMP Mask Reply messages",
	 PRIV_CONF|PRIV_SUBIF);

/******************************************************************************
 *	ip rarp-server <address>
 *	no ip rarp-server [<address>]
 */

EOLNS	(ci_ip_rarp_eol, rarp_server_command);
IPADDR	(ci_ip_rarp_addr, ci_ip_rarp_eol, no_alt,
	 OBJ(paddr,1), "IP server address");
NOPREFIX(ci_ip_rarp_no, ci_ip_rarp_addr, ci_ip_rarp_eol);
NVGENNS	(ci_ip_rarp_nvgen, ci_ip_rarp_no, rarp_server_command);
KEYWORD	(ci_ip_rarp, ci_ip_rarp_nvgen, ci_ip_maskreply,
	 "rarp-server", "Enable RARP server for static arp entries",
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 *	ip helper-address <address>
 *	no ip helper-address [<address>]
 *
 * (*OBJ(paddr,1)) = <address>
 */

EOLS	(ci_ip_helpaddr_eol, helper_command, IP_HELPER_ADDR);
TESTVAR	(ci_ip_nohelpaddr, ci_ip_helpaddr_eol, no_alt,
	 NONE, NONE, NONE, sense, FALSE);
IPADDR	(ci_ip_helpaddr_addr, ci_ip_helpaddr_eol, ci_ip_nohelpaddr,
	 OBJ(paddr,1), "IP destination address");
NVGENS	(ci_ip_helpaddr_nvgen, ci_ip_helpaddr_addr,
	 helper_command, IP_HELPER_ADDR);
KEYWORD	(ci_ip_helpaddr, ci_ip_helpaddr_nvgen, ci_ip_rarp,
	 "helper-address", "Specify a destination address for UDP broadcasts",
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 *	ip access-group <list> [in | out]
 *	no access-group [<list>] [in | out]
 *
 * Note: OBJ(int,10) is used by xtacacs/Tacacs+ to set a per-user
 * access list. 
 */

EOLS    (ci_ip_agrp_out_eol, ip_access_command, OUT);
EOLS    (ci_ip_agrp_in_eol, ip_access_command, IN);

KEYWORD (ci_ip_agrp_out, ci_ip_agrp_out_eol, ci_ip_agrp_out_eol,
	 "out", "outbound packets", PRIV_CONF|PRIV_SUBIF);
KEYWORD (ci_ip_agrp_in, ci_ip_agrp_in_eol, ci_ip_agrp_out,
	 "in", "inbound packets", PRIV_CONF|PRIV_SUBIF);

GENERAL_STRING (ci_ip_agrp_grpname2, ci_ip_agrp_in, no_alt,
		OBJ(string,1), "Access-list name", STRING_HELP_CHECK);
TEST_MULTIPLE_FUNCS(ci_ip_agrp_stest2, ci_ip_agrp_grpname2,
		    no_alt, ci_ip_agrp_out_eol);

KEYWORD (ci_ip_agrp_no_out, ci_ip_agrp_out_eol, ci_ip_agrp_stest2,
	 "out", "outbound packets", PRIV_CONF|PRIV_SUBIF);
KEYWORD (ci_ip_agrp_no_in, ci_ip_agrp_in_eol, ci_ip_agrp_no_out,
	 "in", "inbound packets", PRIV_CONF|PRIV_SUBIF);

GENERAL_STRING (ci_ip_agrp_grpname, ci_ip_agrp_in, no_alt,
		OBJ(string,1), "Access-list name", STRING_HELP_CHECK);
TEST_MULTIPLE_FUNCS(ci_ip_agrp_stest, ci_ip_agrp_grpname, NONE,
		    no_alt);

TESTVAR	(ci_ip_agrp_noprefix, ci_ip_agrp_no_in, ci_ip_agrp_stest,
	 NONE, NONE, NONE, sense, FALSE);

NUMBER	(ci_ip_agrp_grp, ci_ip_agrp_in, ci_ip_agrp_noprefix,
	 OBJ(int,1), 1, MAXSLOWACC, "IP access list (standard or extended)");
NVGENNS (ci_ip_agrp_nvgen, ci_ip_agrp_grp, ip_access_command);
KEYWORD (ci_ip_agrp, ci_ip_agrp_nvgen, ci_ip_helpaddr,
	 "access-group", "Specify access control for packets",
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************
 *	ip broadcast-address <ipaddr>
 *	no ip broadcast-address [<ipaddr>]
 */

EOLNS	(ci_ip_bcaddr_eol, ip_broadcast_command);
IPADDR	(ci_ip_bcaddr_get, ci_ip_bcaddr_eol, no_alt,
	 OBJ(paddr,1), "IP broadcast address");
NOPREFIX(ci_ip_nobcaddr, ci_ip_bcaddr_get, ci_ip_bcaddr_eol);
NVGENNS	(ci_ip_bcaddr_nvgen, ci_ip_nobcaddr, ip_broadcast_command);
KEYWORD	(ci_ip_bcaddr, ci_ip_bcaddr_nvgen, ci_ip_agrp,
	 "broadcast-address", "Set the broadcast address of an interface",
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 *	ip unnumbered <interface>
 *	no ip unnumbered [<interface>]
 *
 * OBJ(idb,1) = <interface>
 */
EOLS	(ci_ip_unnum_eol, ip_unnumbered_command, IP_UNNUMBERED);
FORWARD_INTERFACE(ci_ip_unnum_int, ci_ip_unnum_eol, no_alt, OBJ(idb,1),
		  IFTYPE_ANYSUB);
NOPREFIX(ci_ip_unnum_noprefix, ci_ip_unnum_int, ci_ip_unnum_eol);
NVGENS	(ci_ip_unnumnvgen, ci_ip_unnum_noprefix, ip_unnumbered_command,
	 IP_UNNUMBERED);
KEYWORD	(ci_ip_unnum, ci_ip_unnumnvgen, ci_ip_bcaddr,
	 "unnumbered", "Enable IP processing without an explicit address",
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************
 *	ip  address  <ipaddr>  <ip-subnet-mask> [secondary]
 *	no ip address [<ipaddr> [<ip-subnet-mask> [secondary]]]
 *
 * (*OBJ(paddr,1)) = ipaddr
 * (*OBJ(paddr,2)) = mask
 * OBJ(int,1) = TRUE if secondary flag present, else FALSE
 */
EOLNS	(ci_ip_addr_eol, ip_address_command);

/* secondary - GETOBJ(int,1) is initialized to FALSE (0) */
KEYWORD_ID(ci_ip_secondary, ci_ip_addr_eol, ci_ip_addr_eol,
	   OBJ(int,1), TRUE,
	   "secondary", "Make this IP address a secondary address",
	   PRIV_CONF|PRIV_SUBIF);

IPADDR	(ci_ip_get_mask, ci_ip_secondary, no_alt,
	 OBJ(paddr,2), "IP subnet mask");

/* ipaddr - If we've parsed a 'no' prefix, head for the door */
EQUAL	(ci_ip_noaddr, ci_ip_addr_eol, no_alt, sense, FALSE);
IPADDR	(ci_ip_get_addr, ci_ip_get_mask, ci_ip_noaddr,
	 OBJ(paddr,1), "IP address");

NVGENNS	(ci_ip_address_nvgen, ci_ip_get_addr, ip_address_command);
KEYWORD	(ci_ip_address, ci_ip_address_nvgen, ci_ip_unnum,
	 "address", "Set the IP address of an interface",
	 PRIV_CONF|PRIV_SUBIF);

/*************************************************************************/
HELP	(ci_ip_help, ci_ip_address,
	 "Interface IP configuration subcommands:\n");

KEYWORD	(ci_ip, ci_ip_help, NONE,
	 "ip", "Interface Internet Protocol config commands",
	 PRIV_CONF|PRIV_SUBIF);

ASSERT  (ci_ip_subif, ci_ip, ALTERNATE,
	 main_if_or_vc_or_vlan(csb->interface));

#undef	ALTERNATE
#define	ALTERNATE	ci_ip_subif
