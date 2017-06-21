/* $Id: exec_clear_access.h,v 3.3.20.3 1996/08/12 16:02:51 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ip/exec_clear_access.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L E A R _ A C C E S S . H
 *
 * August 1995, Ricky Li Fo Sjoe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_clear_access.h,v $
 * Revision 3.3.20.3  1996/08/12  16:02:51  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.20.2  1996/05/30  23:43:55  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.3.20.1  1996/05/04  01:01:50  wilber
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
 * Revision 3.3  1996/02/01  23:33:41  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  09:32:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:03:57  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/30  22:28:01  rlfs
 * Placeholder for 11.1 Lock&Key feature
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear ip access-template [<aclnum> | <name>] <dyn. name> <src> <dest>
 *
 */
EOLNS (clear_access_eol, clear_tempacl_command);

/*
 * Parse a destination
 * <dstaddr> <dstmask>
 * "any"
 * "host" <dstaddr>"
 */

IPADDR_NAME(clear_dsthostaddr, clear_access_eol, no_alt,
	 OBJ(paddr, 3), "Destination address");
KEYWORD_ID (clear_dsthost, clear_dsthostaddr, no_alt, OBJ(int,3),
	     1, "host", "A single destination host", PRIV_CONF);

KEYWORD_ID(clear_dstany, clear_access_eol, clear_dsthost,
	   OBJ(paddr, 3).ip_addr, WILD_ANY,
	   "any", "Any destination host", PRIV_CONF);

IPADDR	(clear_ipaddr4, clear_access_eol, no_alt,
	 OBJ(paddr,4), "Destination wildcard bits");
IPADDR	(clear_tempacl_dest, clear_ipaddr4, clear_dstany,
	 OBJ(paddr,3),"Destination address");

/*
 * Parse a source
 * <srcaddr> <srcmask>
 * "any"
 * "host" <srcaddr>"
 */

IPADDR_NAME (clear_srchostaddr, clear_tempacl_dest, no_alt,
	 OBJ(paddr,1), "Source address");
KEYWORD_ID (clear_srchost, clear_srchostaddr, no_alt, OBJ(int,2),
	     1, "host", "A single source host", PRIV_CONF);

KEYWORD_ID(clear_srcany, clear_tempacl_dest, clear_srchost,
	   OBJ(paddr, 1).ip_addr, WILD_ANY,
	   "any", "Any source host", PRIV_CONF);

IPADDR	(clear_ipaddr2, clear_tempacl_dest, no_alt,
	 OBJ(paddr,2), "Source wildcard bits");
IPADDR	(clear_tempacl_src, clear_ipaddr2, clear_srcany,
	 OBJ(paddr,1), "Source address");

GENERAL_STRING  (clear_tempacl_aname, clear_tempacl_src,
		 NONE,  OBJ(string,1), "Access-list name",
		 STRING_HELP_CHECK); 
TEST_MULTIPLE_FUNCS (clear_tempacl_stest, clear_tempacl_aname, NONE,
		     no_alt);

NUMBER	(clear_tempacl_lnum, clear_tempacl_stest, no_alt,
	 OBJ(int,1), MINSLOWACC, MAXSLOWACC, "IP extended access list");

KEYWORD (clear_tempacl, clear_tempacl_lnum, no_alt,
	"access-template", "Access-template", PRIV_OPR);

/*
 * Old access point (aka "clear access-template ..."
 */
NOP	(oclear_ip_acl, clear_tempacl, OALTERNATE);
#undef	OALTERNATE
#define	OALTERNATE	oclear_ip_acl

/******************************************************************
 * clear ip access-list counters [list]
 * 
 */
EOLNS	(clear_ip_acl_eol, clear_ip_access);
STRING  (clear_ip_acl_name, clear_ip_acl_eol, clear_ip_acl_eol,
	 OBJ(string,1), "ACL Name");
IFELSE  (cleap_ip_acl_num_chk, clear_ip_acl_name, clear_ip_acl_eol,
	 GETOBJ(int,1) == 0);
NUMBER	(clear_ip_acl_num, clear_ip_acl_eol, clear_ip_acl_name,
	 OBJ(int,1), 0, MAXSLOWACC, "Access list number");
KEYWORD (clear_ip_acl_co, clear_ip_acl_num, no_alt,
	 "counters", "Clear access list counters", PRIV_OPR);
KEYWORD (clear_ip_acl, clear_ip_acl_co, clear_tempacl,
	 "access-list", "Clear access list statistical information",
	 PRIV_OPR);


#undef	ALTERNATE
#define	ALTERNATE	clear_ip_acl
