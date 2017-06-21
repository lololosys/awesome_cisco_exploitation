/* $Id: cfg_routemap_set_ip_policy.h,v 3.3.52.1 1996/06/18 19:06:39 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_routemap_set_ip_policy.h,v $
 *------------------------------------------------------------------
 * cfg_routemap_set_ip_policy.h -- IP policy specific route map clauses
 *
 * Mar 1995 Tony Li
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_routemap_set_ip_policy.h,v $
 * Revision 3.3.52.1  1996/06/18  19:06:39  widmer
 * CSCdi59706:  parser lets you set tos/precedence bits forever (input
 * loop)
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/28  19:50:38  widmer
 * CSCdi44251:  Replace Ifelse(csb->in_help)/Eval with Func
 *
 * Revision 3.2  1995/11/17  17:31:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/08/16  01:01:15  tli
 * CSCdi38512:  Policy: TOS and precedence long help says match instead of
 * set
 *
 * Revision 2.4  1995/07/12  07:19:09  tli
 * CSCdi36426:  Policy routing will not disable on an interface
 * - Policy routed packets were creating fastswitching cache entries.
 *   That's not till _next_ release.
 * - Add counters to record packet and byte count on a per route map entry
 *   basis.
 * - Marketing identified a catastrophic deficiency.  Implemented "set ip
 *   tos" and "set ip precedence" to cover this deficiency.
 *
 * Revision 2.3  1995/06/21  18:16:02  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.2  1995/06/15  15:18:47  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.1  1995/06/07  21:05:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_EXIT(routemap_set_ippolicy_return, no_alt);

/***************************************************************
 * set ip precedence <precedence>
 *
 * OBJ(int, 1) = Precedence value
 */

pdecl(routemap_set_ip_precedence_value);

FUNC 	(routemap_set_ip_precedence_func,
	 routemap_set_ip_precedence_value,
	 route_map_set_ip_precedence_command);
/*
 *        111 - Network Control
 *        110 - Internetwork Control
 *        101 - CRITIC/ECP
 *        100 - Flash Override
 *        011 - Flash
 *        010 - Immediate
 *        001 - Priority
 *        000 - Routine
 * Values are taken from RFC 791
 */

KEYWORD_ID (routemap_set_prec_ippredroutine, routemap_set_ip_precedence_func,
	    routemap_set_ippolicy_return,
	    OBJ(int, 1), PRECEDENCE_ROUTINE,
	    PRECEDENCE_ROUTINE_STR, "Set routine precedence (0)",
	    PRIV_CONF);

KEYWORD_ID (routemap_set_prec_ippredpriority, routemap_set_ip_precedence_func,
	    routemap_set_prec_ippredroutine,
	    OBJ(int, 1), PRECEDENCE_PRIORITY,
	    PRECEDENCE_PRIORITY_STR, "Set priority precedence (1)",
	    PRIV_CONF);

KEYWORD_ID (routemap_set_prec_ippredimmediate, routemap_set_ip_precedence_func,
	    routemap_set_prec_ippredpriority,
	    OBJ(int, 1), PRECEDENCE_IMMEDIATE,
	    PRECEDENCE_IMMEDIATE_STR, "Set immediate precedence (2)",
	    PRIV_CONF);

KEYWORD_ID (routemap_set_prec_ippredflash, routemap_set_ip_precedence_func,
	    routemap_set_prec_ippredimmediate,
	    OBJ(int, 1), PRECEDENCE_FLASH,
	    PRECEDENCE_FLASH_STR, "Set flash precedence (3)",
	    PRIV_CONF);

KEYWORD_ID_MM(routemap_set_prec_ippredflashover,
	      routemap_set_ip_precedence_func,
	      routemap_set_prec_ippredflash,
	      OBJ(int, 1), PRECEDENCE_FLASHOVER,
	      PRECEDENCE_FLASHOVER_STR, "Set flash override precedence (4)",
	      PRIV_CONF, 6);

KEYWORD_ID (routemap_set_prec_ippredcrit, routemap_set_ip_precedence_func,
	    routemap_set_prec_ippredflashover,
	    OBJ(int, 1), PRECEDENCE_CRITICAL,
	    PRECEDENCE_CRITICAL_STR, "Set critical precedence (5)",
	    PRIV_CONF);

KEYWORD_ID (routemap_set_prec_ippredinternet, routemap_set_ip_precedence_func,
	    routemap_set_prec_ippredcrit,
	    OBJ(int, 1), PRECEDENCE_INTERNET,
	    PRECEDENCE_INTERNET_STR, "Set internetwork control precedence (6)",
	    PRIV_CONF);

KEYWORD_ID (routemap_set_prec_ipprednet, routemap_set_ip_precedence_func,
	    routemap_set_prec_ippredinternet,
	    OBJ(int, 1), PRECEDENCE_NETWORK,
	    PRECEDENCE_NETWORK_STR, "Set network control precedence (7)",
	    PRIV_CONF);

NUMBER	(routemap_set_ip_precedence_value, routemap_set_ip_precedence_func,
	 routemap_set_prec_ipprednet,
	 OBJ(int, 1), 0, 7, "Precedence value");

KEYWORD (routemap_set_ip_precedence, routemap_set_ip_precedence_value,
	 ALTERNATE,
	 "precedence", "Set precedence field", PRIV_CONF);

/***************************************************************
 * set ip tos <tos>
 *
 * OBJ(int, 1) = Type of service
 */

FUNC 	(routemap_set_ip_tos_func,
	 routemap_set_ippolicy_return,
	 route_map_set_ip_tos_command);

/*
 *     1000   --   minimize delay
 *     0100   --   maximize throughput
 *     0010   --   maximize reliability
 *     0001   --   minimize monetary cost
 *     0000   --   normal service
 */

KEYWORD_ID (routemap_set_ip_tos_iptosnormal, routemap_set_ip_tos_func,
	    routemap_set_ippolicy_return,
	    OBJ(int, 1), IP_TOS_NORMAL,
	    TOS_NORMAL_NAME, "Set normal TOS (0)", PRIV_CONF);


KEYWORD_ID (routemap_set_ip_tos_iptoscost, routemap_set_ip_tos_func,
	    routemap_set_ip_tos_iptosnormal,
	    OBJ(int, 1), IP_TOS_LOW_COST,
	    TOS_COST_NAME, "Set min monetary cost TOS (1)", PRIV_CONF);

KEYWORD_ID (routemap_set_ip_tos_iptosreliab, routemap_set_ip_tos_func,
	    routemap_set_ip_tos_iptoscost,
	    OBJ(int, 1), IP_TOS_HIGH_RELY,
	    TOS_RELIABLE_NAME, "Set max reliable TOS (2)", PRIV_CONF);

KEYWORD_ID (routemap_set_ip_tos_iptosthrough, routemap_set_ip_tos_func,
	    routemap_set_ip_tos_iptosreliab,
	    OBJ(int, 1), IP_TOS_HIGH_TPUT,
	    TOS_THROUGH_NAME, "Set max throughput TOS (4)", PRIV_CONF);

KEYWORD_ID (routemap_set_ip_tos_iptosdelay, routemap_set_ip_tos_func,
	    routemap_set_ip_tos_iptosthrough,
	    OBJ(int, 1), IP_TOS_LOW_DELAY,
	    TOS_DELAY_NAME, "Set min delay TOS (8)", PRIV_CONF);

NUMBER	(routemap_set_ip_tos_value, routemap_set_ip_tos_func,
	 routemap_set_ip_tos_iptosdelay,
	 OBJ(int, 1), 0, 15, "Type of service value");

KEYWORD (routemap_set_ip_tos, routemap_set_ip_tos_value,
	 routemap_set_ip_precedence,
	 "tos", "Set type of service field", PRIV_CONF);

/***************************************************************
 * set ip default next-hop <address> [<address> ... <address>]
 *
 * OBJ(paddr, 1) = IP address
 */

pdecl(routemap_set_default_nh_ip_addr);

FUNC 	(routemap_set_default_nh_func,
	 routemap_set_default_nh_ip_addr,
	 route_map_set_default_nh_command);
IPADDR	(routemap_set_default_nh_ip_addr, routemap_set_default_nh_func,
	 routemap_set_ippolicy_return,
	 OBJ(paddr, 1), "Default next hop IP address");
KEYWORD (routemap_set_default_nh, routemap_set_default_nh_ip_addr, no_alt,
	 "next-hop", "Next hop along path", PRIV_CONF);

KEYWORD	(routemap_set_ip_default, routemap_set_default_nh,
	 routemap_set_ip_tos,
	 "default", "Set default information", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	routemap_set_ip_default
