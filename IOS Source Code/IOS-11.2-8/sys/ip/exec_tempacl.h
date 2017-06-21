/* $Id: exec_tempacl.h,v 3.3.20.2 1996/08/12 16:03:03 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ip/exec_tempacl.h,v $
 *------------------------------------------------------------------
 * E X E C _ T E M P L A T E . H
 *
 *
 *
 * August 1995, Ricky Li Fo Sjoe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_tempacl.h,v $
 * Revision 3.3.20.2  1996/08/12  16:03:03  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.20.1  1996/05/04  01:01:55  wilber
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
 * Revision 3.3  1996/02/01  23:33:44  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  09:32:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:03:57  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/30  22:28:00  rlfs
 * Placeholder for 11.1 Lock&Key feature
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * access-enable: Allow temporary access into the router
 *
 * access-enable
 */
EOLNS(exec_accenab_eol, ip_accena_command);

NUMBER	(exec_accenab_time, exec_accenab_eol, no_alt,
	OBJ(int,2), MINIDLEACC, MAXIDLEACC, "Access-List Idle timeout");

KEYWORD	(exec_accenab_timeout, exec_accenab_time, exec_accenab_eol,
	"timeout", "Maximum idle time to expire this entry", PRIV_USER);
    
KEYWORD_ID(exec_accenab_host, exec_accenab_timeout, exec_accenab_timeout,
	   OBJ(int, 1), TRUE,
	   "host", "Enable a specific host only", PRIV_USER);

KEYWORD(exec_accenab, exec_accenab_host, ALTERNATE,
	   "access-enable", "Create a temporary Access-List entry", PRIV_USER);


/*
 * temp-access: Create a temporary access-list entry to (dis)allow
 *	access through the firewall.
 *
 * temp-access [<listnum> | <listname>] <dynname> <src> <dest>
 * 	       [timeout <mins.>] 
 *
 * OBJ(int,1) == Access-list #
 * OBJ(string,1) == Access-list name
 * OBJ(string,2) == Name of temporary list
 * OBJ(paddr,1) == src IP address
 * OBJ(paddr,2) == src IP mask
 * OBJ(paddr,3) == dest IP address
 * OBJ(paddr,4) == dest IP mask
 * OBJ(int,2) == timeout(in mins.)
 */
EOLNS(exec_tempacl_eol, ip_tempacl_command);

NUMBER	(exec_tempacl_time, exec_tempacl_eol, no_alt,
	OBJ(int,2), MINIDLEACC, MAXIDLEACC, "Access-List Idle timeout");

KEYWORD	(exec_tempacl_timeout, exec_tempacl_time, exec_tempacl_eol,
	"timeout", "Maximum idle time to expire this entry", PRIV_USER);

/*
 * Parse a destination
 * <dstaddr> <dstmask>
 * "any"
 * "host" <dstaddr>"
 */

IPADDR_NAME(exec_dsthostaddr, exec_tempacl_timeout, no_alt,
	 OBJ(paddr, 3), "Destination address");
KEYWORD (exec_dsthost, exec_dsthostaddr, no_alt,
	 "host", "A single destination host", PRIV_CONF);

KEYWORD_ID(exec_dstany, exec_tempacl_timeout, exec_dsthost,
	   OBJ(paddr, 4).ip_addr, WILD_ANY,
	   "any", "Any destination host", PRIV_CONF);

IPADDR	(exec_ipaddr4, exec_tempacl_timeout, no_alt,
	 OBJ(paddr,4), "Destination wildcard bits");
IPADDR	(exec_tempacl_dest, exec_ipaddr4, exec_dstany,
	 OBJ(paddr,3),"Destination address");
	 
/* 
 * Parse a source
 * <srcaddr> <srcmask> 
 * "any"
 * "host" <srcaddr>"
 */

IPADDR_NAME (exec_srchostaddr, exec_tempacl_dest, no_alt,
	 OBJ(paddr,1), "Source address");
KEYWORD (exec_srchost, exec_srchostaddr, no_alt,
	 "host", "A single source host", PRIV_CONF);

KEYWORD_ID(exec_srcany, exec_tempacl_dest, exec_srchost,
	   OBJ(paddr, 2).ip_addr, WILD_ANY,
	   "any", "Any source host", PRIV_CONF);

IPADDR	(exec_ipaddr2, exec_tempacl_dest, no_alt,
	 OBJ(paddr,2), "Source wildcard bits");
IPADDR	(exec_tempacl_src, exec_ipaddr2, exec_srcany,
	 OBJ(paddr,1), "Source address");

STRING	(exec_tempacl_name, exec_tempacl_src, no_alt,
	OBJ(string,2), "Name of Temp. list within the access-list");

GENERAL_STRING  (exec_tempacl_aname, exec_tempacl_name,
		 NONE,  OBJ(string,1), "Access-list name",
		 STRING_HELP_CHECK); 

TEST_MULTIPLE_FUNCS (exec_tempacl_stest, exec_tempacl_aname, NONE,
		     no_alt);

NUMBER	(exec_tempacl_lnum, exec_tempacl_name, exec_tempacl_stest,
	 OBJ(int,1), MINSLOWACC, MAXSLOWACC, "IP extended access list");

KEYWORD(exec_tempacl, exec_tempacl_lnum, exec_accenab,
	   "access-template", "Create a temporary Access-List entry", PRIV_ROOT);

#undef	ALTERNATE
#define	ALTERNATE	exec_tempacl
