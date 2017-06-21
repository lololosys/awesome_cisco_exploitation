/* $Id: cfg_router_neighbor_bgp.h,v 3.3.20.3 1996/07/30 06:56:06 rchandra Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_neighbor_bgp.h,v $
 *------------------------------------------------------------------
 * C F G _ R O U T E R _ N E I G H B O R _ B G P . H
 * 
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_router_neighbor_bgp.h,v $
 * Revision 3.3.20.3  1996/07/30  06:56:06  rchandra
 * CSCdi64489:  BGP: private-AS number cannot be removed from AS-path
 * Branch: California_branch
 * - add 'neighbor x.x.x.x remove-private-AS' command.
 * - add 'inbound' keyword to 'neighbor x.x.x.x soft-reconfiguration'
 *   command (CSCdi64487)
 *
 * Revision 3.3.20.2  1996/05/04  01:21:21  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.3.20.1  1996/04/16  19:01:24  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.3  1996/02/01  23:33:58  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  17:32:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/08/28  21:14:24  rchandra
 * CSCdi39436:  BGP : Allow conditional unsuppression of suppressed routes
 * - add 'neighbor x.x.x.x unsuppress-map <map>' command
 *
 * Revision 2.4  1995/07/29  01:09:59  mordock
 * CSCdi37882:  currekt missed-spelled wurds
 * Corrected spelling mistakes in various messages.
 *
 * Revision 2.3  1995/07/07  02:08:43  ahh
 * CSCdi36796:  typo in help for passwords
 * Add missing parenthesis.
 *
 * Revision 2.2  1995/07/06  23:12:25  rchandra
 * CSCdi36759:  BGP : Add conditional per neighbor default generation
 * - add 'neighbor <x.x.x.x> default-originate route-map <tag>' command.
 *
 * Revision 2.1  1995/07/01  12:15:56  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.3  1995/06/30  20:24:43  rchandra
 * CSCdi36620:  BGP requires IBGP neighbors to be fully meshed
 *
 * Revision 2.2  1995/06/21  18:17:22  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:12:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*********************************************************************
 *	no neighbor <address/tag>
 */

EOLS	(rtr_neigh_noprefix_eol, neighbor_command, NEIGHBOR_STD);
ASSERT	(rtr_neigh_noprefix, rtr_neigh_noprefix_eol, NONE, !csb->sense);

/**********************************************************************
 * [no] neighbor <address> advertise-map <route-map> non-exist-map <route-map>
 *
 * OBJ(string, 2) = advertise route map name
 * OBJ(string, 3) = condition route map name
 */
EOLS	(rtr_neigh_cond_adv_eol, neighbor_command, NEIGHBOR_BGP_COND_ADV);
STRING  (rtr_neigh_cond_map, rtr_neigh_cond_adv_eol, no_alt,
	 OBJ(string, 3), "condition route-map name");
KEYWORD (rtr_neigh_cond_adv_nonexist, rtr_neigh_cond_map, no_alt,
	 "non-exist-map",
	 "advertise prefix only if prefix in the condition does not exist",
	 PRIV_CONF);
STRING  (rtr_neigh_cond_adv_map, rtr_neigh_cond_adv_nonexist, no_alt,
	 OBJ(string, 2), "advertise route-map name");
KEYWORD (rtr_neigh_cond_adv, rtr_neigh_cond_adv_map, rtr_neigh_noprefix,
	 "advertise-map",
	 "specify route-map for conditional advertisement", PRIV_CONF);


/******************************************************************
 *	neighbor <address> password [0-7] <text>
 *	no neighbor <address> password
 *
 * OBJ(int,1) = [0-7] (encryption type), or -1 if no type given
 * OBJ(string,2) = <password>
 *
 * Parse chain duped from line config command.
 */

EOLS	(rtr_neigh_pass_eol, neighbor_command, NEIGHBOR_BGP_PASSWORD);
TEXT	(rtr_neigh_pass_passwd, rtr_neigh_pass_eol, no_alt,
	 OBJ(string,2), "The password");
TEST_MULTIPLE_FUNCS(rtr_neigh_pass_test, rtr_neigh_pass_passwd,
	 no_alt, no_alt);
DECIMAL (rtr_neigh_pass_enctype, rtr_neigh_pass_passwd, rtr_neigh_pass_test,
	 OBJ(int,1), ENCRYPT_NONE, ENCRYPT_TEMP,
	 "Encryption type (0 to disable encryption, 7 for proprietary)");
NOPREFIX (rtr_neigh_pass_noprefix, rtr_neigh_pass_enctype, rtr_neigh_pass_eol);
NVGENS	(rtr_neigh_pass_nvgen, rtr_neigh_pass_noprefix,
	 neighbor_command, NEIGHBOR_BGP_PASSWORD);
KEYWORD	(rtr_neigh_pass, rtr_neigh_pass_nvgen, rtr_neigh_cond_adv,
	 "password", "Set a password", PRIV_CONF);


/**********************************************************************
 *	[no] neighbor <address> peer-group <tag>
 *
 * (*OBJ(paddr,1)) = <address>
 * OBJ(string,1) = peer group name
 */

EOLS	(rtr_neigh_peergrp_mem_eol, neighbor_command, NEIGHBOR_BGP_PEERGP_MEM);
STRING	(rtr_neigh_peergrp_mem_s, rtr_neigh_peergrp_mem_eol, no_alt, 
	 OBJ(string,1), "peer-group name");
KEYWORD (rtr_neigh_peergrp_mem, rtr_neigh_peergrp_mem_s, NONE,
	 "peer-group", "Member of the peer-group", PRIV_CONF);
ASSERT	(rtr_neigh_peergrp_mem_assert, rtr_neigh_peergrp_mem, 
	 rtr_neigh_pass, GETOBJ(paddr, 1)->type);

/**********************************************************************
 *	[no] neighbor <address/tag> route-reflector-client
 *
 */

EOLS	(rtr_neigh_rrclient_eol, neighbor_command, NEIGHBOR_BGP_RRCLIENT);
KEYWORD	(rtr_neigh_rrclient, rtr_neigh_rrclient_eol, 
	 rtr_neigh_peergrp_mem_assert,
	 "route-reflector-client", 
	 "Configure a neighbor as Route Reflector client", PRIV_CONF);


/**********************************************************************
 *	[no] neighbor <address> backwards-compatible
 *
 * (*OBJ(paddr,1)) = <address>
 */

EOLS	(rtr_neigh_backc_eol, neighbor_command, NEIGHBOR_BGP_BACKCOMPAT);
KEYWORD (rtr_neigh_backc, rtr_neigh_backc_eol, rtr_neigh_rrclient,
	 "backwards-compatible",
	 "Backwards compatibility with earlier BGP implementation",
	 PRIV_CONF|PRIV_INTERNAL);

/**********************************************************************
 * 	[no] neighbor <address> remove-private-AS
 *
 */
EOLS	(rtr_neigh_nopriv_as_eol, neighbor_command, 
	 NEIGHBOR_BGP_REMOVE_PRIVATE_AS);
KEYWORD (rtr_neigh_nopriv_as, rtr_neigh_nopriv_as_eol, rtr_neigh_backc,
	 "remove-private-AS", 
	 "Remove private AS number from outbound updates", PRIV_CONF);

/**********************************************************************
 * 	[no] neighbor <address> soft-reconfiguration inbound
 *
 */
EOLS	(rtr_neigh_reconfig_eol, neighbor_command, NEIGHBOR_BGP_RECONFIG_INOK);
KEYWORD (rtr_neigh_reconfig_in, rtr_neigh_reconfig_eol, no_alt,
	 "inbound", 
	 "Allow inbound soft reconfiguration for this neighbor", PRIV_CONF);
KEYWORD	(rtr_neigh_reconfig, rtr_neigh_reconfig_in, rtr_neigh_nopriv_as,
	 "soft-reconfiguration", 
	 "Per neighbor soft reconfiguration", PRIV_CONF);

/**********************************************************************
 * 	[no] neighbor <address> default-originate route-map <route-map>
 *
 * OBJ(string, 2) = route map name
 */
EOLS	(rtr_neigh_default_eol, neighbor_command, NEIGHBOR_BGP_DEFAULT);
STRING  (rtr_neigh_default_map_s, rtr_neigh_default_eol, no_alt,
	 OBJ(string, 2), "route-map name");
KEYWORD (rtr_neigh_default_map, rtr_neigh_default_map_s, rtr_neigh_default_eol,
	 "route-map",
	 "Route-map to specify criteria to originate default", PRIV_CONF);
KEYWORD (rtr_neigh_default, rtr_neigh_default_map, rtr_neigh_reconfig,
	 "default-originate",
	 "Originate default route to this neighbor", PRIV_CONF);


/**********************************************************************
 *	[no] neighbor <address> ebgp-multihop [ttl]
 *
 * (*OBJ(paddr,1)) = <address>
 * OBJ(int,3) = ttl (if not specified, ttl == 255 for backwards compat)
 */

EOLS	(rtr_neigh_ebgpm_eol, neighbor_command, NEIGHBOR_BGP_EMHOP);
SET	(rtr_neigh_ebgpm_ttlbc, rtr_neigh_ebgpm_eol, OBJ(int,3), 255);
NUMBER	(rtr_neigh_ebgpm_ttl, rtr_neigh_ebgpm_eol, rtr_neigh_ebgpm_ttlbc,
	 OBJ(int,3), 1, 255, "maximum hop count");
KEYWORD (rtr_neigh_ebgpm, rtr_neigh_ebgpm_ttl, rtr_neigh_default,
	 "ebgp-multihop",
	 "Allow EBGP neighbors not on directly connected networks", PRIV_CONF);

/**********************************************************************
 *	[no] neighbor <address> next-hop-self
 *
 * (*OBJ(paddr,1)) = <address>
 */

EOLS	(rtr_neigh_nexthop_eol, neighbor_command, NEIGHBOR_BGP_NHSELF);
KEYWORD (rtr_neigh_nexthop, rtr_neigh_nexthop_eol, rtr_neigh_ebgpm,
	 "next-hop-self",
	 "Disable the next hop calculation for this neighbor", PRIV_CONF);

/**********************************************************************
 *	[no] neighbor <address> send-community
 *
 * (*OBJ(paddr,1)) = <address>
 */

EOLS	(rtr_neigh_send_comm_eol, neighbor_command, NEIGHBOR_BGP_COMMOK);
KEYWORD (rtr_neigh_send_comm, rtr_neigh_send_comm_eol, rtr_neigh_nexthop,
	 "send-community",
	 "Send Community attribute to this neighbor", PRIV_CONF);


/**********************************************************************
 *	[no] neighbor <address> update-source <interface>
 *
 * (*OBJ(paddr,1)) = <address>
 * OBJ(idb,1) = <interface>
 */

EOLS	(rtr_neigh_updsrc_eol, neighbor_command, NEIGHBOR_BGP_UPDSRC);
INTERFACE (rtr_neigh_updsrc_if, rtr_neigh_updsrc_eol, no_alt, 
	   OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD (rtr_neigh_updsrc, rtr_neigh_updsrc_if, rtr_neigh_send_comm,
	 "update-source", "Source of routing updates", PRIV_CONF);

/**********************************************************************
 *	[no] neighbor <address> min-advertisement-interval <seconds>
 *
 * (*OBJ(paddr,1)) = <address>
 * OBJ(int,1) = <time in seconds>
 */

EOLS	(rtr_neigh_minadvert_eol, neighbor_command, NEIGHBOR_BGP_MINADVERT);
EQUAL	(rtr_neigh_minadvert_no, rtr_neigh_minadvert_eol, no_alt,
	 sense, FALSE);
NUMBER  (rtr_neigh_minadvert_val, rtr_neigh_minadvert_eol,
	 rtr_neigh_minadvert_no, OBJ(int,1), 0, 600, "time in seconds");
NVGENS	(rtr_neigh_minadvert_nvgen, rtr_neigh_minadvert_val, neighbor_command,
	 NEIGHBOR_BGP_MINADVERT);
KEYWORD (rtr_neigh_minadvert, rtr_neigh_minadvert_nvgen, rtr_neigh_updsrc,
	 "advertisement-interval",
	 "Minimum interval between sending EBGP routing updates", PRIV_CONF);

/**********************************************************************
 *	[no] neighbor <address> weight <weight>
 *
 * (*OBJ(paddr,1)) = <address>
 * OBJ(int,1) = <weight>
 */

EOLS	(rtr_neigh_weight_eol, neighbor_command, NEIGHBOR_BGP_WEIGHT );
EQUAL	(rtr_neigh_weight_no, rtr_neigh_weight_eol, no_alt, sense, FALSE);
NUMBER  (rtr_neigh_weight_val, rtr_neigh_weight_eol, rtr_neigh_weight_no,
	   OBJ(int,1), BGP_MIN_WEIGHT, BGP_MAX_WEIGHT,
	   "default weight");
NVGENS	(rtr_neigh_weight_nvgen, rtr_neigh_weight_val, neighbor_command,
	 NEIGHBOR_BGP_WEIGHT);
KEYWORD (rtr_neigh_weight, rtr_neigh_weight_nvgen, rtr_neigh_minadvert,
	 "weight", "Set default weight for routes from this neighbor",
	 PRIV_CONF);

/************************************************************************
 * neighbor <address> version <value>
 * no neighbor <address> version <value>
 *
 * (*OBJ(paddr,1)) = <address>
 * OBJ(int,1) = <value>
 */
PARAMS_KEYONLY (rtr_neigh_vers, rtr_neigh_weight,
		"version", OBJ(int,1), BGP_MIN_VERSION, BGP_MAX_VERSION,
		neighbor_command, NEIGHBOR_BGP_VERSION,
		"Set the BGP version to match a neighbor",
		"Neighbor's BGP version", PRIV_CONF);

/**********************************************************************
 * neighbor <address> remote-as <number>
 * no neighbor <address> remote-as [<number>]
 *
 * (*OBJ(paddr,1)) = <address>
 * OBJ(int,2) = <as number>
 */
EOLS	(rtr_neigh_remas_eol, neighbor_command, NEIGHBOR_BGP_REMAS);
NUMBER	(rtr_neigh_remas_num, rtr_neigh_remas_eol, no_alt,
	 OBJ(int,1), 1, MAXUSHORT, "AS of remote neighbor");
NOPREFIX (rtr_neigh_remas_noprefix, rtr_neigh_remas_num, rtr_neigh_remas_eol);
KEYWORD (rtr_neigh_remas, rtr_neigh_remas_noprefix, rtr_neigh_vers,
	 "remote-as", "Specify a BGP neighbor", PRIV_CONF);

/**********************************************************************
 * neighbor <address> unsuppress-map <map>
 * no neighbor <address> unsuppress-map <map>
 *
 * (*OBJ(paddr,1)) = <address>
 * OBJ(string,2) = <map>
 */
EOLS	(rtr_neigh_unsup_map_eol, neighbor_command, NEIGHBOR_BGP_UNSUPPRESS);
STRING	(rtr_neigh_unsup_map_string, rtr_neigh_unsup_map_eol, no_alt,
	 OBJ(string,2), "Name of route map");
KEYWORD (rtr_neigh_unsup_map, rtr_neigh_unsup_map_string, rtr_neigh_remas,
	 "unsuppress-map", 
	 "Route-map to selectively unsuppress suppressed routes", PRIV_CONF);

/**********************************************************************
 * neighbor <address> route-map <map> { in | out }
 * no neighbor <address> route-map <map> [{ in | out }]
 *
 * (*OBJ(paddr,1)) = <address>
 * OBJ(string,2) = <map>
 * OBJ(int,2) = IN, OUT
 *
 */
EOLS	(rtr_neigh_map_eol, neighbor_command, NEIGHBOR_BGP_ROUTEMAP);
KEYWORD_ID (rtr_neigh_map_out, rtr_neigh_map_eol, no_alt,
	    OBJ(int,2), OUT, "out", "Apply map to outbound routes", PRIV_CONF);
KEYWORD_ID (rtr_neigh_map_in, rtr_neigh_map_eol, rtr_neigh_map_out,
	    OBJ(int,2), IN, "in", "Apply map to incoming routes", PRIV_CONF);
STRING	(rtr_neigh_map_string, rtr_neigh_map_in, no_alt,
	 OBJ(string,2), "Name of route map");
KEYWORD (rtr_neigh_map, rtr_neigh_map_string, rtr_neigh_unsup_map,
	 "route-map", "Apply route map to neighbor", PRIV_CONF);

/**********************************************************************
 * neighbor <address> filter-list <list> { in | out | weight <weight>}
 * no neighbor <address> filter-list <list> [{ in | out | weight <weight>}]
 *
 * (*OBJ(paddr,1)) = <address>
 * OBJ(int,1) = <list>
 * OBJ(int,2) = IN, OUT, NEIGHBOR_BGP_WEIGHT
 * OBJ(int,3) = <weight>
 *
 */
EOLS	(rtr_neigh_flist_eol, neighbor_command, NEIGHBOR_BGP_FILLIST);
NUMBER  (rtr_neigh_flist_wval, rtr_neigh_flist_eol, no_alt,
	 OBJ(int,3), BGP_MIN_WEIGHT, BGP_MAX_WEIGHT,
	 "weight to apply");
KEYWORD_ID (rtr_neigh_flist_weight, rtr_neigh_flist_wval, no_alt,
	    OBJ(int,2), NEIGHBOR_BGP_WEIGHT,
	    "weight", "Assign importance to a list", PRIV_CONF);
KEYWORD_ID (rtr_neigh_flist_out, rtr_neigh_flist_eol, rtr_neigh_flist_weight,
	    OBJ(int,2), OUT, "out", "Filter outgoing routes", PRIV_CONF);
KEYWORD_ID (rtr_neigh_flist_in, rtr_neigh_flist_eol, rtr_neigh_flist_out,
	    OBJ(int,2), IN, "in", "Filter incoming routes", PRIV_CONF);
NUMBER	(rtr_neigh_flist_list, rtr_neigh_flist_in, no_alt,
	 OBJ(int,1), MINREGEXPACC+1, MAXREGEXPACC, "AS path access list");
KEYWORD (rtr_neigh_flist, rtr_neigh_flist_list, rtr_neigh_map,
	 "filter-list", "Establish BGP filters", PRIV_CONF);

/**********************************************************************
 *	[no] neighbor <address> filter-as <number> {deny | permit <weight>}
 *
 * (*OBJ(paddr,1)) = <address>
 * OBJ(int,1)  = <number>
 * OBJ(int,2)  = TRUE for permit; FALSE for deny
 * OBJ(int,3)  = <weight> (if permit)
 */

EOLS	(rtr_neigh_filteras_eol, neighbor_command, NEIGHBOR_BGP_FILAS);
NUMBER  (rtr_neigh_filteras_weight, rtr_neigh_filteras_eol, no_alt,
	 OBJ(int,3), BGP_MIN_WEIGHT, BGP_MAX_WEIGHT,
	 "weight of route");
KEYWORD_ID(rtr_neigh_filteras_permit, rtr_neigh_filteras_weight, no_alt,
	   OBJ(int,2), TRUE,
	   "permit", "Accept information about this AS", PRIV_CONF);
KEYWORD	(rtr_neigh_filteras_deny, rtr_neigh_filteras_eol,
	 rtr_neigh_filteras_permit,
	 "deny", "Ignore information about this AS", PRIV_CONF);
NOPREFIX (rtr_neigh_filteras_noprefix, rtr_neigh_filteras_deny, 
	  rtr_neigh_filteras_eol);
NUMBER	(rtr_neigh_filteras_as, rtr_neigh_filteras_deny, no_alt,
	 OBJ(int,1), 1, 65535, "AS to filter");
KEYWORD	(rtr_neigh_filteras, rtr_neigh_filteras_as, rtr_neigh_flist,
	 "filter-as", "", 
	 PRIV_CONF | PRIV_HIDDEN);

/**********************************************************************
 *	[no] neighbor <address> distribute-list [<list #> | <list name>]
 *			{in | out} 
 *
 * OBJ(int,1)  = <list #>
 * OBJ(string,1) = <list name>
 * OBJ(int,2)  = IN, OUT
 * (*OBJ(paddr,1)) = <address>
 */

EOLS	(rtr_neigh_distlist_eol, neighbor_command, NEIGHBOR_BGP_DIST);
KEYWORD_ID (rtr_neigh_distlist_out, rtr_neigh_distlist_eol, no_alt, 
	 OBJ(int,2), OUT,
	 "out", "Filter outgoing updates", PRIV_CONF);
KEYWORD_ID (rtr_neigh_distlist_in, rtr_neigh_distlist_eol,
	 rtr_neigh_distlist_out, OBJ(int,2), IN,
	 "in", "Filter incoming updates", PRIV_CONF);

GENERAL_STRING (rtr_neigh_distlist_accname, rtr_neigh_distlist_in, no_alt,
		OBJ(string,1), "IP Access-list name", STRING_HELP_CHECK);
TEST_MULTIPLE_FUNCS(rtr_neigh_distlist_stest,
		    rtr_neigh_distlist_accname, NONE, no_alt);

NUMBER	(rtr_neigh_distlist_list, rtr_neigh_distlist_in,
	 rtr_neigh_distlist_stest, OBJ(int,1), 1, MAXSLOWACC,
	 "IP access list number"); 
KEYWORD	(rtr_neigh_distlist, rtr_neigh_distlist_list, rtr_neigh_filteras,
	 "distribute-list", "Filter updates to/from this neighbor", PRIV_CONF);


/**********************************************************************
 *	[no] neighbor <tag> peer-group
 *
 * (*OBJ(paddr,1)) = <address>
 * OBJ(string,1) = <tag>
 */

EOLS	(rtr_neigh_peergrp_eol, neighbor_command, NEIGHBOR_BGP_PEERGROUP);
KEYWORD (rtr_neigh_peergrp, rtr_neigh_peergrp_eol, NONE,
	 "peer-group", "Configure peer-group", PRIV_CONF);


/**********************************************************************
 *	[no] neighbor <tag> configure-neighbors
 *
 * tags only
 *
 * OBJ(string,1) = <tag>
 *
 */

EOLS	(rtr_neigh_pcfg_eol, neighbor_command, NEIGHBOR_PERM_CFG);
KEYWORD (rtr_neigh_pcfg, rtr_neigh_pcfg_eol, rtr_neigh_peergrp,
	 "configure-neighbors",
	 "Show anonymous neighbors in configuration", PRIV_CONF);

/**********************************************************************
 *	[no] neighbor <tag> neighbor-list {<list> | <list name>}
 *
 * tags only
 *
 * OBJ(int,1)    = <list>
 * OBJ(string,2) = <list name>
 * OBJ(string,1) = <tag>
 */

EOLS    (rtr_neigh_neighlist_eol, neighbor_command,
	 NEIGHBOR_BGP_NEIGHLIST); 

GENERAL_STRING (rtr_neigh_neighlist_accname,
		rtr_neigh_neighlist_eol, no_alt, OBJ(string,2),
		"IP standard access-list name", STRING_HELP_CHECK);  
TEST_MULTIPLE_FUNCS (rtr_neigh_neighlist_stest,
		    rtr_neigh_neighlist_accname, NONE, no_alt);
NUMBER	(rtr_neigh_neighlist_listnum, rtr_neigh_neighlist_eol,
	 rtr_neigh_neighlist_stest,
	 OBJ(int,1), 1, MAXFASTACC, "IP standard access-list number");
KEYWORD (rtr_neigh_neighlist_kw, rtr_neigh_neighlist_listnum,
	 rtr_neigh_pcfg, "neighbor-list",
	 "Access list of neighbors who may connect", PRIV_CONF);
/*
 * only do neighbor-list and configure-neighbors
 * if user hasn't entered an address
 */
ASSERT	(rtr_neigh_neighlist, rtr_neigh_neighlist_kw, rtr_neigh_distlist,
         !GETOBJ(paddr,1)->type);

TESTVAR	(rtr_neigh_bgp_noprefix, NONE, NONE,
	 NONE, NONE, rtr_neigh_neighlist, sense, FALSE);

ASSERT	(rtr_neigh_bgp, rtr_neigh_bgp_noprefix, ALTERNATE,
	 ((pdbtype *) csb->protocol)->proctype & PROC_BGP);

#undef	ALTERNATE
#define ALTERNATE	rtr_neigh_bgp
