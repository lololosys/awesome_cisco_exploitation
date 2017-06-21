/* $Id: exec_clear_ip_mrouting.h,v 3.2.60.3 1996/08/12 16:03:17 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/exec_clear_ip_mrouting.h,v $
 *------------------------------------------------------------------
 * IP multicast clear commands
 *
 * January, 1994.
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_clear_ip_mrouting.h,v $
 * Revision 3.2.60.3  1996/08/12  16:03:17  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.60.2  1996/05/06  16:07:20  dino
 * CSCdi56516:  The sd proecss doesnt process sdr annoucements
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/04/20  01:51:00  dino
 * CSCdi54104:  CGMP must support source-only systems in switched Cat5000
 *              network
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:29:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:00:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:56:50  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.2  1995/06/21  18:17:34  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:16:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear ip igmp group [<group-name> | <group-address>] | [<interface-unit>]
 *
 * OBJ(paddr,1) = <group>
 * OBJ(idb,1)   = <interface-unit>
 */
pdecl(clear_ip_igmp_group_opts);

EOLNS	(clear_ip_igmp_group_eol, igmp_clear_group_command);
IPADDR_NAME (clear_ip_igmp_group_addr, clear_ip_igmp_group_opts, 
	     clear_ip_igmp_group_eol, OBJ(paddr,1), 
	     "IP name or group address");
TEST_MULTIPLE_FUNCS(clear_ip_igmp_group_ambig, clear_ip_igmp_group_addr,
		    NONE, no_alt);
INTERFACE (clear_ip_igmp_group_idb, clear_ip_igmp_group_opts,
	   clear_ip_igmp_group_ambig, OBJ(idb,1), IFTYPE_ANYSUB);
NOP (clear_ip_igmp_group_opts, clear_ip_igmp_group_idb, NONE);

KEYWORD_ID(clear_ip_igmp_group, clear_ip_igmp_group_opts, no_alt,
	   OBJ(int,1), FALSE,
	   "group", "Delete IGMP group cache entries", PRIV_OPR);

KEYWORD (clear_ip_igmp, clear_ip_igmp_group, ALTERNATE,
	 "igmp", "IGMP clear commands", PRIV_OPR);

/******************************************************************
 * clear ip mroute [<group-or-name> [<source-address>]]  | [*]
 *
 * OBJ(paddr,1) = <group>
 * OBJ(paddr,2) = <source>
 * OBJ(int,1)   = TRUE if <group> specified.
 * OBJ(int,2)   = TRUE if <source> specified.
 */
EOLNS	(clear_ip_mroute_eol, ip_clear_mroute_command);
KEYWORD	(clear_ip_mroute_all, clear_ip_mroute_eol, no_alt,
	 "*", "Delete all multicast routes", PRIV_OPR);
SET (clear_ip_mroute_source_set, clear_ip_mroute_eol, OBJ(int,2), TRUE);
IPADDR_NAME (clear_ip_mroute_source, clear_ip_mroute_source_set, 
	clear_ip_mroute_eol, OBJ(paddr,2), "Source address" );
SET (clear_ip_mroute_group_set, clear_ip_mroute_source, OBJ(int,1), TRUE);
IPADDR_NAME (clear_ip_mroute_group, clear_ip_mroute_group_set, 
	     clear_ip_mroute_all, OBJ(paddr,1), "IP name or group address");
KEYWORD_ID(clear_ip_mroute, clear_ip_mroute_group, clear_ip_igmp,
	   OBJ(int,1), FALSE,
	   "mroute", "Delete multicast route table entries", PRIV_OPR);

/******************************************************************
 * clear ip cgmp [<interface>]
 *
 * OBJ(idb,1) = where to send CGMP Leave message. Optional.
 */
EOLNS (clear_ip_cgmp_eols, cgmp_clear_command);
INTERFACE (clear_ip_cgmp_idb, clear_ip_cgmp_eols, clear_ip_cgmp_eols,
	   OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD (clear_ip_cgmp, clear_ip_cgmp_idb, clear_ip_mroute,
	 "cgmp", "Cisco Group Management Protocol (CGMP)", PRIV_OPR);

/******************************************************************
 * clear ip sdr [<group-address> | "<session-name>"]
 *
 * OBJ(paddr,1) = optional group address.
 * OBJ(string,1) = session name.
 */
EOLS (clear_ip_sdr_eols, sd_clear_command, 0);
STRING (clear_ip_sdr_string, clear_ip_sdr_eols, clear_ip_sdr_eols,
	OBJ(string,1), "Session name (in double quotes)");
TEST_MULTIPLE_FUNCS(clear_ip_sdr_ambig, clear_ip_sdr_string, NONE, no_alt);
IPADDR (clear_ip_sdr_group, clear_ip_sdr_eols, clear_ip_sdr_ambig,
	OBJ(paddr,1), "IP group address");
KEYWORD (clear_ip_sdr, clear_ip_sdr_group, clear_ip_cgmp,
	 "sdr", "Session Directory (SDPv2) cache", PRIV_OPR);

/******************************************************************
 * clear ip dvmrp route <address-or-name> | *
 *
 * OBJ(paddr,1) = <dvmrp route address>
 */
EOLNS      (clear_ip_dvmrp_route_eol, dvmrp_clear_route);
KEYWORD	(clear_ip_dvmrp_route_all, clear_ip_dvmrp_route_eol, no_alt,
	 "*", "All DVMRP routes", PRIV_OPR);
IPADDR (clear_ip_dvmrp_route_route, clear_ip_dvmrp_route_eol,
	     clear_ip_dvmrp_route_all, OBJ(paddr,1), "IP route");
KEYWORD (clear_ip_dvmrp_route_word, clear_ip_dvmrp_route_route, no_alt,
           "route", "Clear DVMRP route table entries", PRIV_OPR);
KEYWORD (clear_ip_dvmrp_route, clear_ip_dvmrp_route_word, clear_ip_sdr,
	   "dvmrp", "DVMRP", PRIV_OPR);

/*****************************************************************
 * clear ip pim rp [<address>] 
 *
 * OBJ(paddr,1) = <rp address>
 * 
 */
EOLNS  (clear_ip_pim_autorp_eol, clear_autorp_command);
IPADDR (clear_ip_pim_autorp_addr, clear_ip_pim_autorp_eol,
	clear_ip_pim_autorp_eol, OBJ(paddr,1), "RP address");
KEYWORD (clear_ip_pim_autorp_word, clear_ip_pim_autorp_addr, no_alt,
	 "auto-rp", "Clear Auto-RP mapping table", PRIV_OPR);
KEYWORD (clear_ip_pim, clear_ip_pim_autorp_word, clear_ip_dvmrp_route,
	 "pim", "PIM", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	clear_ip_pim

