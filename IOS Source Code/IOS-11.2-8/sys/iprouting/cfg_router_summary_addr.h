/* $Id: cfg_router_summary_addr.h,v 3.3.20.2 1996/08/12 16:03:51 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_summary_addr.h,v $
 *------------------------------------------------------------------
 * cfg_router_summary_addr.h
 *
 * Supports the router subcommand "summary-address". Provides IP aggregation
 * configuration.
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Longer description here.
 *------------------------------------------------------------------
 * $Log: cfg_router_summary_addr.h,v $
 * Revision 3.3.20.2  1996/08/12  16:03:51  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.20.1  1996/04/16  19:01:26  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.3  1996/02/01  23:34:01  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  17:32:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:06:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:16:01  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:13:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 * summary-address <IP address> <IP mask> [level-1 | level-2 | level-1-2]
 *		   [not-advertise] [tag <tag>]
 * no summary-address <IP address>
 *
 * (*OBJ(paddr,1)) = <IP address>
 * (*OBJ(paddr,2)) = <IP mask>
 * OBJ(int,1) = level or flags
 * OBJ(int,2) = TRUE if not-advertise
 * OBJ(int,3) = tag
 *
 */

/* ISIS version of command */

EOLS	(router_sum_addr_isis_eol, summary_command, ISIS_IP_SUM_ADDR_CMD);
KEYWORD_ID_MM(router_sum_addr_isis_level12, router_sum_addr_isis_eol,
	      router_sum_addr_isis_eol,
	      OBJ(int,1), ISIS_CIRCUIT_L1L2,
	      "level-1-2",
	      "Summarize into both area and sub-domain", PRIV_CONF, 8);
KEYWORD_ID(router_sum_addr_isis_level2, router_sum_addr_isis_eol,
	   router_sum_addr_isis_level12, OBJ(int,1), ISIS_CIRCUIT_L2,
	   "level-2", "Summarize into level-2 sub-domain", PRIV_CONF);
KEYWORD_ID(router_sum_addr_isis_level1, router_sum_addr_isis_eol,
	   router_sum_addr_isis_level2, OBJ(int,1), ISIS_CIRCUIT_L1,
	   "level-1", "Summarize into level-1 area", PRIV_CONF);
SET     (router_sum_addr_isis_default, router_sum_addr_isis_level1, 
        OBJ(int,1), ISIS_CIRCUIT_L2);
IPADDR (router_sum_addr_isis_mask, router_sum_addr_isis_default, no_alt,
	OBJ(paddr,2), "Summary mask");

NOPREFIX (router_sum_addr_isis_noprefix, router_sum_addr_isis_mask,
	 router_sum_addr_isis_eol);
IPADDR (router_sum_addr_isis_ipaddr, router_sum_addr_isis_noprefix, no_alt,
	OBJ(paddr,1), "IP summary address");

NVGENS (router_sum_addr_proto_isis, router_sum_addr_isis_ipaddr, 
	summary_command, ISIS_IP_SUM_ADDR_CMD);

/* IP version of command */

EOLNS (router_sum_addr_ip_eol, summary_command);

NUMBER (router_sum_addr_ip_tag_value, router_sum_addr_ip_eol, no_alt,
	OBJ(int,3), 0, MAXUINT, "32-bit tag value");
KEYWORD (router_sum_addr_ip_tag, router_sum_addr_ip_tag_value, 
	 router_sum_addr_ip_eol, "tag", "Set tag", PRIV_CONF);
KEYWORD_ID (router_sum_addr_ip_not_adv, router_sum_addr_ip_eol,
	    router_sum_addr_ip_tag, OBJ(int,2), TRUE, "not-advertise",
	    "Do not advertise when translating OSPF type-7 LSA", 
	    PRIV_CONF);
IFELSE (router_sum_addr_ospf_test, router_sum_addr_ip_not_adv,
	router_sum_addr_ip_eol, 
	(((pdbtype *) csb->protocol)->proctype & PROC_OSPF));
IPADDR (router_sum_addr_ip_mask, router_sum_addr_ospf_test, 
	router_sum_addr_ip_eol,
	OBJ(paddr,2), "Summary mask");
IPADDR (router_sum_addr_ip_ipaddr, router_sum_addr_ip_mask, no_alt,
	OBJ(paddr,1), "IP summary address");
NVGENNS (router_sum_addr_proto_ip, router_sum_addr_ip_ipaddr, 
	summary_command);

IFELSE (router_sum_addr_proto, router_sum_addr_proto_ip,
	router_sum_addr_proto_isis,
	(((pdbtype *) csb->protocol)->family == PDB_IP &&
	 ((pdbtype *) csb->protocol)->proctype & (PROC_BGP | PROC_OSPF)));

KEYWORD (router_sum_addr_kw, router_sum_addr_proto, NONE, 
	 "summary-address", "Configure IP address summaries",
	 PRIV_CONF);

ASSERT	(router_sum_addr, router_sum_addr_kw, ALTERNATE,
	 (((clns_pdbtype *) csb->protocol)->family == PDB_OSI &&
	  ((clns_pdbtype *) csb->protocol)->typeflag & ISO_ISISMASK) ||
	 (((pdbtype *) csb->protocol)->family == PDB_IP &&
	  ((pdbtype *) csb->protocol)->proctype  & (PROC_BGP | PROC_OSPF)));

#undef	ALTERNATE
#define	ALTERNATE	router_sum_addr
