/* $Id: cfg_ip_routing_ospf.h,v 3.2 1995/11/17 17:31:39 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/cfg_ip_routing_ospf.h,v $
 *------------------------------------------------------------------
 * OSPF ip routing commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_ip_routing_ospf.h,v $
 * Revision 3.2  1995/11/17  17:31:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:04:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/17  05:23:53  raj
 * CSCdi36887:  OSPF command ip ospf-name-lookup has inconsistent syntax.
 *
 * Revision 2.1  1995/07/01  12:15:41  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:08:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************************
 *	ip ospf name-lookup
 * 	no ip ospf name-lookup
 *
 * csb->sense = TRUE = perform name lookups for ospf
 *		FALSE = don't perform name lookups for ospf
 */
EOLS	(cr_ip_ospf_namelookup_eol, ospf_routing_commands, IP_OSPFNAMELOOKUP);
KEYWORD	(cr_ip_ospf_namelookup, cr_ip_ospf_namelookup_eol, ALTERNATE,
	"name-lookup","Display OSPF router ids as DNS names", PRIV_CONF);
KEYWORD	(cr_ip_ospf, cr_ip_ospf_namelookup, ALTERNATE,
	"ospf","OSPF", PRIV_CONF);

/* below is the old way */
KEYWORD	(cr_ip_ospfnamelookup, cr_ip_ospf_namelookup_eol, cr_ip_ospf,
	"ospf-name-lookup","Display OSPF router ids as DNS names", 
	PRIV_CONF|PRIV_NONVGEN|PRIV_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	cr_ip_ospfnamelookup
