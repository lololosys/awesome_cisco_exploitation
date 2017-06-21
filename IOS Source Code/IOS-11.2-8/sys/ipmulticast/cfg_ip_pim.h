/* $Id: cfg_ip_pim.h,v 3.4.10.5 1996/09/10 05:31:33 lwei Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/cfg_ip_pim.h,v $
 *------------------------------------------------------------------
 * cfg_ip_pim.h - PIM configuration commands.
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_ip_pim.h,v $
 * Revision 3.4.10.5  1996/09/10  05:31:33  lwei
 * CSCdi68465:  RP information is too difficult to change in last-hop
 *              routers
 * Branch: California_branch
 *
 * Revision 3.4.10.4  1996/09/05  18:06:52  chwhite
 * CSCdi67932:  Can not unconfigure static mroute
 * Branch: California_branch
 *
 * Revision 3.4.10.3  1996/08/23  16:18:25  chwhite
 * CSCdi66534:  Removing tunnel doesnt not get rid of static mroute
 * Branch: California_branch
 *
 * Revision 3.4.10.2  1996/08/12  16:03:16  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4.10.1  1996/05/22  19:20:01  mleelani
 * CSCdi58181:  7000: crash at _fast_check
 * Branch: California_branch
 * Wrong check for slow acl.
 *
 * Revision 3.4  1996/02/22  09:03:09  dino
 * CSCdi49566:  Allow control over which RPs are used in transit routers.
 *
 * Revision 3.3  1995/12/17  19:24:17  dino
 * CSCdi44742:  PIM should only switch to the SPT when traffic threshold
 *              is reached
 *
 * Revision 3.2  1995/11/17  17:29:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:00:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/29  01:09:17  mordock
 * CSCdi37882:  currekt missed-spelled wurds
 * Corrected spelling mistakes in various messages.
 *
 * Revision 2.1  1995/07/01  12:56:49  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.2  1995/06/10  03:52:25  dino
 * CSCdi35623:  EFT bugs found during 11.0 integration period.
 *
 * Revision 2.1  1995/06/07  22:08:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*	
 * [no] ip mroute <source> [<mask>] | [<protocol><as-number>] [route-map <map>]
 *                <rpf-address> | <interface> [<distance>]
 *
 * OBJ(paddr,1) = <source>
 * OBJ(paddr,2) = <mask>
 * OBJ(pdb,1) = pdb of unicast routing protocol
 * OBJ(paddr,3) = RPF address (either network number or IP address)
 * OBJ(idb,1) = RPF interface
 * OBJ(int,1) = distance
 * OBJ(string,1) = <map>
 */
EOLNS (cip_mroute_eol, ip_mroute_command);
NUMBER (cip_mroute_distance, cip_mroute_eol, cip_mroute_eol,
	OBJ(int,1), 0, 255, "Administrative distance for mroute");
INTERFACE (cip_mroute_rpf_idb, cip_mroute_distance, no_alt,
	   OBJ(idb,1), IFTYPE_ANYSUB);
IPADDR (cip_mroute_rpf_addr, cip_mroute_distance, cip_mroute_rpf_idb,
	OBJ(paddr,3), "RPF neighbor address or route");
NOPREFIX(cip_mroute_noprefix, cip_mroute_rpf_addr, cip_mroute_eol);
TEST_MULTIPLE_FUNCS(cip_mroute_ambig, cip_mroute_noprefix, cip_mroute_rpf_addr,
		    no_alt);
STRING (cip_mroute_routemap_name, cip_mroute_ambig, no_alt,
	OBJ(string,1), "Route-map name");
KEYWORD (cip_mroute_routemap, cip_mroute_routemap_name, cip_mroute_ambig,
	 "route-map", "Route-map reference", PRIV_CONF);
ROUTER (cip_mroute_pdb, cip_mroute_routemap, cip_mroute_routemap);
IPADDR (cip_mroute_mask, cip_mroute_pdb, no_alt,
	OBJ(paddr,2), "Network mask");
IPADDR (cip_mroute_source, cip_mroute_mask, no_alt,
	OBJ(paddr,1), "Source address");
NVGENNS (cip_mroute_nvgen, cip_mroute_source, ip_mroute_command);
KEYWORD (cip_mroute, cip_mroute_nvgen, ALTERNATE,
	 "mroute", "Configure static multicast routes", PRIV_CONF);

/*
 * ------------------------------------------------------------------------
 */

/*
 * ip pim message-interval <time-in-seconds>
 * no ip pim message-interval
 *
 * OBJ(int,1) = message interval in seconds
 */
EOLS (cip_pim_msg_int_eol, pim_commands, PIM_MSG_INTERVAL);
NUMBER (cip_pim_msg_int_num, cip_pim_msg_int_eol, no_alt, 
	OBJ(int,1), 1, 0xffff, "Interval in seconds");
NOPREFIX (cip_pim_msg_int_no, cip_pim_msg_int_num, cip_pim_msg_int_eol);
NVGENS (cip_pim_msg_int_nvgen, cip_pim_msg_int_no, pim_commands, 
	PIM_MSG_INTERVAL);
KEYWORD (cip_pim_msg_int, cip_pim_msg_int_nvgen, no_alt,
	 "message-interval", "PIM periodic message interval", PRIV_CONF);

/*
 * ip pim fast-register-stop
 * no ip pim fast-register-stop
 */
EOLS (cip_pim_fast_reg_stop_eol, pim_commands, PIM_FAST_REG_STOP);
NOPREFIX (cip_pim_fast_reg_stop_no, cip_pim_fast_reg_stop_eol,
	  cip_pim_fast_reg_stop_eol);
NVGENS (cip_pim_fast_reg_stop_nvgen, cip_pim_fast_reg_stop_no, pim_commands,
	PIM_FAST_REG_STOP);
KEYWORD (cip_pim_fast_reg_stop, cip_pim_fast_reg_stop_nvgen, cip_pim_msg_int,
	 "fast-register-stop", "Immediately send register-stops on registers",
	 PRIV_CONF | PRIV_HIDDEN);

/*
 * ip pim send-rp-discovery scope <ttl>
 * no ip pim send-rp-discovery
 *
 * OBJ(int,1) = ttl
 */
EOLS (cip_pim_rp_disc_eol, pim_commands, PIM_SEND_RP_DISCOVERY);
NUMBER (cip_pim_rp_disc_num, cip_pim_rp_disc_eol, no_alt,
	OBJ(int,1), 1, TTL_255, "TTL");
KEYWORD (cip_pim_rp_disc_scop, cip_pim_rp_disc_num, no_alt,
	 "scope", "Scope of the RP discovery packets", PRIV_CONF);
NOPREFIX (cip_pim_rp_disc_no, cip_pim_rp_disc_scop, cip_pim_rp_disc_eol);
NVGENS (cip_pim_rp_disc_nvgen, cip_pim_rp_disc_no, pim_commands,
	PIM_SEND_RP_DISCOVERY);
KEYWORD (cip_pim_rp_disc, cip_pim_rp_disc_nvgen, cip_pim_fast_reg_stop,
	 "send-rp-discovery",
	 "Auto-RP send RP discovery message (as RP-mapping agent)",
	 PRIV_CONF);

/*
 *      ip pim send-rp-announce <interface-unit> scope <ttl> [group-list <acl>]
 *   no ip pim send-rp-announce <interface-unit>
 *
 * OBJ(idb,1) = IP address of RP
 * OBJ(int,1) = TTL
 * OBJ(int,2) | OBJ(string,2) = access-list reference
 */
EOLS (cip_pim_rp_annce_eol, pim_commands, PIM_SEND_RP_ANNOUNCE);
GENERAL_STRING (cip_pim_rp_annce_nacl, cip_pim_rp_annce_eol, no_alt,
		OBJ(string,2), "IP Named Standard Access list",
		STRING_HELP_CHECK);
TEST_MULTIPLE_FUNCS (cip_pim_rp_annce_stest, cip_pim_rp_annce_nacl, no_alt,
		     NONE);
NUMBER (cip_pim_rp_annce_acl, cip_pim_rp_annce_eol, cip_pim_rp_annce_stest,
	OBJ(int,2), MINFASTACC, MAXFASTACC,
	"Access-list reference for multicast groups");
KEYWORD (cip_pim_rp_annce_group, cip_pim_rp_annce_acl, cip_pim_rp_annce_eol,
	 "group-list", "Group access-list", PRIV_CONF);
NUMBER (cip_pim_rp_annce_ttl, cip_pim_rp_annce_group, no_alt,
	OBJ(int,1), 1, TTL_255, "TTL of the RP announce packet");
KEYWORD (cip_pim_rp_annce_scope, cip_pim_rp_annce_ttl, no_alt,
	 "scope", "RP announcement scope", PRIV_CONF);
NOPREFIX (cip_pim_rp_annce_no, cip_pim_rp_annce_scope, cip_pim_rp_annce_eol);
INTERFACE (cip_pim_rp_annce_int, cip_pim_rp_annce_no, no_alt,
	   OBJ(idb,1), IFTYPE_ANYSUB);
NVGENS (cip_pim_rp_annce_nvgen, cip_pim_rp_annce_int, pim_commands,
	PIM_SEND_RP_ANNOUNCE);
KEYWORD (cip_pim_rp_annce, cip_pim_rp_annce_nvgen, cip_pim_rp_disc,
	 "send-rp-announce", "Auto-RP send RP announcement", PRIV_CONF);

/*
 * [no] ip pim rp-announce-filter [rp-list <acl>] [group-list <acl>]
 * 
 * OBJ(int,1) = rp-list <acl>
 * OBJ(int,2) = group-list <acl>
 */
EOLS (cip_pim_rp_annce_filter_eol, pim_commands, PIM_ANNOUNCE_FILTER);
ASSERT (cip_pim_rp_annce_filter_test, cip_pim_rp_annce_filter_eol,
	no_alt, (GETOBJ(int,1) || *GETOBJ(string,1) ||
		 GETOBJ(int,2) || *GETOBJ(string,2)));
GENERAL_STRING (cip_pim_rp_annce_filter_grouplist_nacl,
		cip_pim_rp_annce_filter_eol, no_alt, OBJ(string,2),
		"IP Named Standard Access list", STRING_HELP_CHECK);
TEST_MULTIPLE_FUNCS (cip_pim_rp_annce_filter_grouplist_stest,
		     cip_pim_rp_annce_filter_grouplist_nacl, no_alt, NONE);
NUMBER (cip_pim_rp_annce_filter_grouplist_acl, cip_pim_rp_annce_filter_eol, 
	cip_pim_rp_annce_filter_grouplist_stest, OBJ(int,2), MINFASTACC,
	MAXFASTACC, "Access-list reference for group");
KEYWORD (cip_pim_rp_annce_filter_grouplist, 
	 cip_pim_rp_annce_filter_grouplist_acl, 
	 cip_pim_rp_annce_filter_test,
	 "group-list", "Group address access-list", PRIV_CONF);
GENERAL_STRING (cip_pim_rp_annce_filter_rplist_nacl,
		cip_pim_rp_annce_filter_grouplist, no_alt, OBJ(string,1),
		"IP Named Standard Access list", STRING_HELP_CHECK);
TEST_MULTIPLE_FUNCS (cip_pim_rp_annce_filter_rplist_stest,
		     cip_pim_rp_annce_filter_rplist_nacl, no_alt, NONE);
NUMBER (cip_pim_rp_annce_filter_rplist_acl, cip_pim_rp_annce_filter_grouplist, 
	cip_pim_rp_annce_filter_rplist_stest, OBJ(int,1), MINFASTACC,
	MAXFASTACC, "Access-list reference for RP");
KEYWORD (cip_pim_rp_annce_filter_rplist, cip_pim_rp_annce_filter_rplist_acl, 
	 cip_pim_rp_annce_filter_grouplist,
	 "rp-list", "RP address access-list", PRIV_CONF);
NVGENS (cip_pim_rp_annce_filter_nvgen, cip_pim_rp_annce_filter_rplist, 
	pim_commands, PIM_ANNOUNCE_FILTER);
KEYWORD (cip_pim_rp_annce_filter, cip_pim_rp_annce_filter_nvgen, 
	 cip_pim_rp_annce,
	 "rp-announce-filter", "Auto-RP announce message filter", PRIV_CONF);

/*
 *      ip pim accept-rp {<ip-address> | auto-rp} [<group-access-list>]
 *   no ip pim accept-rp {<ip-address> | auto-rp}
 *
 * OBJ(paddr,1) = IP address of RP
 * OBJ(int,1)   = access-list reference
 * OBJ(int,2)   = "Auto-RP" was supplied
 */
EOLS (cip_pim_accept_rp_eol, pim_commands, PIM_ACCEPT_RP);
GENERAL_STRING (cip_pim_accept_rp_nacl, cip_pim_accept_rp_eol,
		cip_pim_accept_rp_eol,
		OBJ(string,1), "IP Named Standard Access list",
		STRING_HELP_CHECK);
TEST_MULTIPLE_FUNCS (cip_pim_accept_rp_stest, cip_pim_accept_rp_nacl,
		     no_alt, NONE);
NUMBER (cip_pim_accept_rp_list, cip_pim_accept_rp_eol,
	cip_pim_accept_rp_stest, 
	OBJ(int,1), MINFASTACC, MAXFASTACC, "Access-list reference for group");
NOPREFIX (cip_pim_accept_rp_no, cip_pim_accept_rp_list, cip_pim_accept_rp_eol);
IPADDR (cip_pim_accept_rp_addr, cip_pim_accept_rp_no, no_alt, 
	OBJ(paddr,1), "IP address of RP for group");
KEYWORD_ID (cip_pim_accept_rp_autorp, cip_pim_accept_rp_no,
	 cip_pim_accept_rp_addr, OBJ(int,2), TRUE,
	 "auto-rp", "only RP-mapping from Auto-RP",
	 PRIV_CONF);
NVGENS (cip_pim_accept_rp_nvgen, cip_pim_accept_rp_autorp, pim_commands, 
	PIM_ACCEPT_RP);
KEYWORD (cip_pim_accept_rp, cip_pim_accept_rp_nvgen, cip_pim_rp_annce_filter,
	 "accept-rp", "RP accept filter", PRIV_CONF);

/*
 * [no] ip pim spt-threshold <kbps> | infinity [group-list <acl>]
 *
 * OBJ(int,1) = kbps or -1 (constant PIM_INFINITE_THRESHOLD) for "infinity"
 * OBJ(int,2) = <acl>, 0 if not specified
 */
EOLS (cip_pim_spt_thres_eol, pim_commands, PIM_SPT_THRESHOLD);
GENERAL_STRING (cip_pim_spt_thres_nacl, cip_pim_spt_thres_eol, no_alt,
		OBJ(string,2), "IP Named Standard Access list",
		STRING_HELP_CHECK);
TEST_MULTIPLE_FUNCS (cip_pim_spt_thres_stest, cip_pim_spt_thres_nacl, no_alt,
		     NONE);
NUMBER (cip_pim_spt_thres_acl, cip_pim_spt_thres_eol, cip_pim_spt_thres_stest,
	OBJ(int,2), MINFASTACC, MAXFASTACC, "Simple access-list");
KEYWORD (cip_pim_spt_thres_group, cip_pim_spt_thres_acl, cip_pim_spt_thres_eol,
	 "group-list", "Group address access-list", PRIV_CONF);
KEYWORD_ID (cip_pim_spt_thres_infinity, cip_pim_spt_thres_group, no_alt,
	    OBJ(int,1), PIM_INFINITE_THRESHOLD, "infinity", 
	    "Never switch to source-tree", PRIV_CONF);
NUMBER (cip_pim_spt_thres_num, cip_pim_spt_thres_group, 
	cip_pim_spt_thres_infinity, OBJ(int,1), 0, 0xfffffffe, 
	"Traffic rate in kilobits per second");
NVGENS (cip_pim_spt_thres_nvgen, cip_pim_spt_thres_num, pim_commands, 
	PIM_SPT_THRESHOLD);
KEYWORD (cip_pim_spt_thres, cip_pim_spt_thres_nvgen, cip_pim_accept_rp,
	 "spt-threshold", "Source-tree switching threshold", PRIV_CONF);

/*
 * [no] ip pim version [1 | 2]
 *
 * OBJ(int,1) = 1 | 2
 */
EOLS(cip_pim_ver_eol, pim_commands, PIM_VERSION);
NUMBER (cip_pim_ver_num, cip_pim_ver_eol, no_alt, OBJ(int,1), 1, 0x2,
	"version number");
NOPREFIX (cip_pim_ver_no, cip_pim_ver_num, cip_pim_ver_eol);
NVGENS (cip_pim_ver_nvgen, cip_pim_ver_no, pim_commands, PIM_VERSION);
KEYWORD (cip_pim_ver, cip_pim_ver_nvgen, cip_pim_spt_thres,
	 "version", "PIM version", PRIV_CONF | PRIV_HIDDEN);

/*
 * [no] ip pim rp-address <ip-address> [<group-access-list>] [override]
 * 
 * OBJ(paddr,1) = IP address of RP
 * OBJ(int,1)   = access-list reference
 * OBJ(int,2)   = "override" was supplied
 */
EOLS (cip_pim_rp_eol, pim_commands, PIM_RP_ADDRESS);
KEYWORD_ID (cip_pim_rp_over, cip_pim_rp_eol, cip_pim_rp_eol,
	    OBJ(int,2), TRUE, 
	    "override", "Overrides Auto RP messages", PRIV_CONF);
GENERAL_STRING (cip_pim_rp_nacl, cip_pim_rp_over, cip_pim_rp_eol,
		OBJ(string,1), "IP Named Standard Access list",
		STRING_HELP_CHECK);
TEST_MULTIPLE_FUNCS (cip_pim_rp_stest, cip_pim_rp_nacl, no_alt, NONE);
NUMBER (cip_pim_rp_list, cip_pim_rp_over, cip_pim_rp_stest, 
	OBJ(int,1), MINFASTACC, MAXFASTACC, "Access-list reference for group");
IPADDR (cip_pim_rp_addr, cip_pim_rp_list, no_alt, 
	OBJ(paddr,1), "IP address of Rendezvous-point for group");
NVGENS (cip_pim_rp_nvgen, cip_pim_rp_addr, pim_commands, PIM_RP_ADDRESS);
KEYWORD_MM (cip_pim_rp, cip_pim_rp_nvgen, cip_pim_ver,
	 "rp-address", "PIM RP-address (Rendezvous Point)", PRIV_CONF, 2);

KEYWORD (cip_pim, cip_pim_rp, cip_mroute,
	 "pim", "PIM global commands", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cip_pim
