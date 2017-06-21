/* $Id: exec_show_ip_pim.h,v 3.6.12.4 1996/09/10 05:31:36 lwei Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/exec_show_ip_pim.h,v $
 *------------------------------------------------------------------
 * PIM show commands.
 *
 * November 1993.
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_ip_pim.h,v $
 * Revision 3.6.12.4  1996/09/10  05:31:36  lwei
 * CSCdi68465:  RP information is too difficult to change in last-hop
 *              routers
 * Branch: California_branch
 *
 * Revision 3.6.12.3  1996/08/12  16:03:21  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.6.12.2  1996/05/21  09:52:20  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.12.1  1996/05/06  16:07:21  dino
 * CSCdi56516:  The sd proecss doesnt process sdr annoucements
 * Branch: California_branch
 *
 * Revision 3.6  1996/02/16  19:42:54  dino
 * CSCdi49202:  Users need an easy way to determine RPF failures due to
 *              misconfig
 *
 * Revision 3.5  1995/12/14  13:07:20  widmer
 * CSCdi45510:  Add General_String macro to replace other string macros
 * Change Var_String, String, Txt_Line, and Text to use General_String
 * Add fail argument to Priv_Test
 * Change Neg1 to use Keyword_Id
 * Remove Sense macro
 *
 * Revision 3.4  1995/12/14  09:54:58  dino
 * CSCdi45564:  Users are complaining that show ip pim interface is
 *              unreadable
 *
 * Revision 3.3  1995/11/28  19:50:27  widmer
 * CSCdi44251:  Replace Ifelse(csb->in_help)/Eval with Func
 *
 * Revision 3.2  1995/11/17  17:29:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:00:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/17  08:28:56  dino
 * CSCdi37181:  Need to show active multicast sources - show ip mroute
 * active.
 *
 * Revision 2.1  1995/07/01  12:56:52  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.2  1995/06/21  18:18:04  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:24:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show ip pim interface [<interface-unit>] [count]
 *
 * OBJ(idb,1) = optional interface name.
 * OBJ(int,1) = TRUE if "count" specified
 */
EOLS	(show_ip_pim_int_eol, pim_show_commands, PIM_SHOW_IDB);
KEYWORD_ID (show_ip_pim_int_count, show_ip_pim_int_eol, show_ip_pim_int_eol,
	    OBJ(int,1), TRUE, "count", "Interface multicast packet count", 
 	    PRIV_USER);
INTERFACE (show_ip_pim_int_idb, show_ip_pim_int_count, show_ip_pim_int_count,
	   OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD (show_ip_pim_int, show_ip_pim_int_idb, no_alt,
	 common_str_interface, "PIM interface information", PRIV_USER);

/******************************************************************
 * show ip pim rp [[mapping [in-use]] [<group-or-name>]]
 *
 * OBJ(paddr,1) = IP group address
 * OBJ(int,1) = TRUE if "mapping" specified
 * OBJ(int,2) = TRUE if "in-use" specified
 */
EOLS	(show_ip_pim_rp_eol, pim_show_commands, PIM_SHOW_RP);
IPADDR_NAME (show_ip_pim_rp_group, show_ip_pim_rp_eol, show_ip_pim_rp_eol,
	     OBJ(paddr,1), "IP name or group address");
TEST_MULTIPLE_FUNCS(show_ip_pim_rp_ambig, show_ip_pim_rp_group, NONE, no_alt);
KEYWORD_ID (show_ip_pim_rp_inuse, show_ip_pim_rp_ambig, show_ip_pim_rp_ambig,
	    OBJ(int,2), TRUE, "in-use", "Show learned RPs in-use", PRIV_USER);
KEYWORD_ID (show_ip_pim_rp_mapping, show_ip_pim_rp_inuse, show_ip_pim_rp_ambig,
	 OBJ(int,1), TRUE, "mapping", "Show group-to-RP mappings", PRIV_USER);
KEYWORD (show_ip_pim_rp, show_ip_pim_rp_mapping, show_ip_pim_int,
	 "rp", "PIM Rendezvous Point (RP) information", PRIV_USER);

/******************************************************************
 * show ip pim neighbor [<interface-unit>]
 *
 * OBJ(idb,1) = optional interface name.
 */
EOLS	(show_ip_pim_nbr_eol, pim_show_commands, PIM_SHOW_NBR);
INTERFACE (show_ip_pim_nbr_idb, show_ip_pim_nbr_eol, show_ip_pim_nbr_eol,
	   OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD (show_ip_pim_nbr, show_ip_pim_nbr_idb, show_ip_pim_rp,
	 "neighbor", "PIM neighbor information", PRIV_USER);

KEYWORD (show_ip_pim, show_ip_pim_nbr, ALTERNATE,
	 "pim", "PIM information", PRIV_USER);


/******************************************************************
 * show ip mpacket [[<source-or-name> [<group-or-name>]] [detail]]
 *
 * This parse chain is a little tricky.  We first check for the
 * detail keyword.  If not there, we check for <source-or-name> and 
 * <group>.  If either are present, we loop back to check for the 
 * detail keyword again.  The IFELSE statements make sure we don't
 * double-check <source-or-name> and <group> and terminate at the
 * end of the line.
 *
 * show_ip_mpacket_help_check is there to provide help processing.
 * Without it, we won't get <cr> as option to terminate a line.
 */
EOLNS (show_ip_mpacket_eols, ip_show_mpacket);
pdecl(show_ip_mpacket_detail);
IFELSE (show_ip_mpacket_help_check, show_ip_mpacket_eols, no_alt,
	help_check(csb));
IPADDR_NAME (show_ip_mpacket_addr2, show_ip_mpacket_detail,
	     show_ip_mpacket_help_check,
	     OBJ(paddr,2),
	     "Group address");
TEST_MULTIPLE_FUNCS (show_ip_mpacket_test2, show_ip_mpacket_addr2, no_alt,
		     NONE);
IPADDR_NAME (show_ip_mpacket_addr1, show_ip_mpacket_detail,
	     show_ip_mpacket_help_check,
	     OBJ(paddr,1),
	     "Group address or source address or source name");
TEST_MULTIPLE_FUNCS (show_ip_mpacket_test, show_ip_mpacket_addr1, no_alt,
		     NONE);
IFELSE (show_ip_mpacket_if3, show_ip_mpacket_test2, show_ip_mpacket_help_check,
	GETOBJ(paddr,2)->ip_addr == 0);
IFELSE (show_ip_mpacket_if2, show_ip_mpacket_test, show_ip_mpacket_if3,
	GETOBJ(paddr,1)->ip_addr == 0);
IFELSE (show_ip_mpacket_if, show_ip_mpacket_eols, show_ip_mpacket_if2,
	eol_check(csb));
KEYWORD_ID (show_ip_mpacket_detail, show_ip_mpacket_eols,
	    show_ip_mpacket_if, OBJ(int,1), TRUE, "detail",
	    "Detail display of IP header and port numbers", PRIV_OPR);
KEYWORD (show_ip_mpacket, show_ip_mpacket_detail, show_ip_pim,
	 "mpacket", "Display possible duplicate multicast packets", PRIV_OPR);

/******************************************************************
 * show ip rpf <source-or-name>
 *
 * OBJ(paddr,1) = required IP address to RPF lookup for
 */
EOLNS (show_ip_rpf_eols, ip_show_rpf);
IPADDR_NAME (show_ip_rpf_addr, show_ip_rpf_eols, no_alt,
	     OBJ(paddr,1), "IP name or address of multicast source");
KEYWORD (show_ip_rpf, show_ip_rpf_addr, show_ip_mpacket,
	 "rpf", "Display RPF information for multicast source", PRIV_USER);

/******************************************************************
 * show ip mroute [<group-or-name> | static] 
 *                [[active [<kbps>]] | [[<source-or-name>] [summary | count]]]
 *
 * OBJ(paddr,1) = optional group address.
 * OBJ(paddr,2) = optional source address.
 * OBJ(int,1)   = TRUE if source address specified.
 * OBJ(int,2)   = TRUE if summary keyword specified.
 * OBJ(int,3)   = TRUE if count keyword specified.
 * OBJ(int,4)   = kbps per second, if active keyword is specified.
 * OBJ(int,5)   = TRUE if static keyword specified.
 *
 * This parse chain is a little tricky.  We first check for the active,
 * summary, count and static keywords.  If not there, we check for
 * <group-or-name>.  If there, we loop back to check for the active, summary and
 * count keywords again.  If keywords not there, we check for <source-or-name>.
 * If there, we loop back to check for summary or count keywords.  The
 * IFELSE statements make sure we don't double-check <group-or-name>
 * and <source-or-name> and terminate at the end of the line.
 *
 * show_ip_mroute_help_check is there to provide help processing.
 * Without it, we won't get <cr> as option to terminate a line.
 */

EOLNS (show_ip_mroute_eol, ip_show_mroute);
pdecl(show_ip_mroute_summary);
pdecl(show_ip_mroute_active);
IFELSE (show_ip_mroute_help_check, show_ip_mroute_eol, no_alt,
	help_check(csb));
IPADDR_NAME (show_ip_mroute_group, show_ip_mroute_summary,
	     no_alt,
	     OBJ(paddr,2), "IP name or group address");
TEST_MULTIPLE_FUNCS (show_ip_mroute_ambig3, show_ip_mroute_group, no_alt,
		     NONE);
SET (show_ip_mroute_source_set, show_ip_mroute_summary, OBJ(int,1), TRUE);
IPADDR_NAME (show_ip_mroute_source, show_ip_mroute_source_set,
	     show_ip_mroute_help_check,
	     OBJ(paddr,2), "IP name or source address");
TEST_MULTIPLE_FUNCS (show_ip_mroute_ambig2, show_ip_mroute_source, no_alt,
		     NONE);
SET (show_ip_mroute_source_set2, show_ip_mroute_ambig3, OBJ(int,1), TRUE);
IFELSE (show_ip_mroute_if4, show_ip_mroute_active, show_ip_mroute_source_set2,
	IPMULTICAST(GETOBJ(paddr,1)->ip_addr));
IPADDR_NAME (show_ip_mroute_group_or_source, show_ip_mroute_if4,
	     show_ip_mroute_help_check,
	     OBJ(paddr,1), "Source or group IP name or address");
TEST_MULTIPLE_FUNCS (show_ip_mroute_ambig, show_ip_mroute_group_or_source,
		     no_alt, NONE);
IFELSE (show_ip_mroute_if3, show_ip_mroute_ambig2, show_ip_mroute_help_check,
	GETOBJ(paddr,2)->ip_addr == 0);
IFELSE (show_ip_mroute_if2, show_ip_mroute_ambig, show_ip_mroute_if3,
	GETOBJ(paddr,1)->ip_addr == 0);
IFELSE (show_ip_mroute_if, show_ip_mroute_eol, show_ip_mroute_if2,
	eol_check(csb));
KEYWORD_ID (show_ip_mroute_count, show_ip_mroute_eol, show_ip_mroute_if,
	    OBJ(int,3), TRUE,
	    "count", "Route and packet count data", PRIV_USER);
KEYWORD_ID (show_ip_mroute_summary, show_ip_mroute_eol, show_ip_mroute_count,
	    OBJ(int,2), TRUE,
	    "summary", "Provide abbreviated display", PRIV_USER);
NUMBER (show_ip_mroute_num, show_ip_mroute_eol, show_ip_mroute_eol,
	OBJ(int,4), 1, (ulong) -1, "Rate in kilobits per second");
KEYWORD_ID (show_ip_mroute_active, show_ip_mroute_num, show_ip_mroute_summary,
	    OBJ(int,4), 4,
	    "active", "Active multicast sources", PRIV_USER);
KEYWORD_ID (show_ip_mroute_static, show_ip_mroute_eol, show_ip_mroute_active,
	    OBJ(int,5), TRUE,
	    "static", "Static multicast routes", PRIV_USER);
KEYWORD (show_ip_mroute, show_ip_mroute_static, show_ip_rpf,
	 "mroute", "IP multicast routing table", PRIV_USER);

/******************************************************************
 * show ip mcache [<group> [<source>]]
 *
 * OBJ(paddr,1) = optional group address.
 * OBJ(paddr,2) = optional source address, only when group is specified.
 */
EOLNS (show_ip_mcache_eols, ip_show_mcache);
IPADDR_NAME (show_ip_mcache_source, show_ip_mcache_eols, show_ip_mcache_eols,
	     OBJ(paddr,2), "IP name or address");
IPADDR_NAME (show_ip_mcache_group, show_ip_mcache_source, show_ip_mcache_eols,
	     OBJ(paddr,1), "IP name or group address");
KEYWORD (show_ip_mcache, show_ip_mcache_group, show_ip_mroute,
	 "mcache", "IP multicast fast-switching cache", PRIV_USER);

/******************************************************************
 * show ip sdr [<group-address> | "<session-name>" | detail]
 *
 * OBJ(paddr,1) = optional group address.
 * OBJ(string,1) = optional session name.
 * OBJ(int,1) = optional "detail" keyword.
 */
EOLNS (show_ip_sdr_eols, sd_show_command);
STRING (show_ip_sdr_string, show_ip_sdr_eols, show_ip_sdr_eols,
	OBJ(string,1), "Session name (in double quotes)");
TEST_MULTIPLE_FUNCS(show_ip_sdr_ambig2, show_ip_sdr_string, NONE, no_alt);
IPADDR (show_ip_sdr_group, show_ip_sdr_eols, show_ip_sdr_ambig2,
	OBJ(paddr,1), "IP group address");
TEST_MULTIPLE_FUNCS(show_ip_sdr_ambig, show_ip_sdr_group, NONE, no_alt);
KEYWORD_ID (show_ip_sdr_detail, show_ip_sdr_eols, show_ip_sdr_ambig,
	    OBJ(int,1), TRUE,
	    "detail", "Session details", PRIV_USER);
KEYWORD (show_ip_sdr, show_ip_sdr_detail, show_ip_mcache,
	 "sdr", "Session Directory (SDPv2) cache", PRIV_USER);


#undef	ALTERNATE
#define	ALTERNATE	show_ip_sdr
