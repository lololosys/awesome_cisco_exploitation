/* $Id: cfg_int_ip2.h,v 3.3.22.1 1996/07/01 23:03:32 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/ip/cfg_int_ip2.h,v $
 *------------------------------------------------------------------
 * cfg_int_ip2.h
 *
 * March 1993, Sanjeev Newarikar
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * IP subcommands that come after the encapsulation in the NV order.
 *------------------------------------------------------------------
 * $Log: cfg_int_ip2.h,v $
 * Revision 3.3.22.1  1996/07/01  23:03:32  rbadri
 * CSCdi60896:  LANE: no fast-switching between second ip add unless LEC
 *              is configured on the main interface
 * Branch: California_branch
 * Permit "ip route-cache same-interface" to allow LANE sub-interfaces to
 * do fastswitching.
 *
 * Revision 3.3  1996/01/23  08:49:43  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 * Revision 3.2  1995/11/17  09:32:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:03:56  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/01  12:55:35  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:05:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


LINK_TRANS(ci_ip2_extend_here, no_alt);

/************************************************************************
 *	ip route-cache cbus
 *	ip route-cache sse
 *	ip route-cache flow		XXXFLOWNAME
 *	no ip route-cache cbus
 *	no ip route-cache sse
 *	no ip route-cache flow		XXXFLOWNAME
 *      no ip route-cache distributed
 *
 * OBJ(int,1) = 0 by default
 *		IP_CBUS_ROUTECACHE if 'cbus' was specified
 *		IP_SSE_ROUTECACHE if 'sse' was specified
 *		IP_FLOW_ROUTECACHE if 'flow' was specfied	XXXFLOWNAME
 *		IP_RSP_ROUTECACHE if 'rsp' was specified
 *		IP_DISTRIBUTED_ROUTECACHE if 'distributed' was specified
 */

EOLS	(ci_ip2_routecache_eol, routec_command, IP_ROUTEC);

LINK_TRANS(ci_ip2_routecache_return_here, ci_ip2_routecache_eol);
LINK_TRANS(ci_ip2_routecache_extend_here, ci_ip2_routecache_eol);

EOLS	(ci_ip2_routecache_same_eol, routec_same_int_command, IP_ROUTEC_SAME);
KEYWORD (ci_ip2_routecache_same, ci_ip2_routecache_same_eol,
	 ci_ip2_routecache_extend_here,
	 "same-interface", "Enable fast-switching on the same interface",
	 PRIV_CONF|PRIV_SUBIF);

KEYWORD	(ci_ip2_routecache, ci_ip2_routecache_same, ci_ip2_extend_here,
	 "route-cache", "Enable fast-switching cache for outgoing packets",
	 PRIV_CONF|PRIV_SUBIF);

/*************************************************************************/
HELP	(ci_ip2_help, ci_ip2_routecache, "");

KEYWORD	(ci_ip2, ci_ip2_help, NONE, 
	 "ip", "Interface Internet Protocol config commands", 
	 PRIV_CONF|PRIV_SUBIF|PRIV_DUPLICATE);

ASSERT  (ci_ip2_subif, ci_ip2, ALTERNATE,
	 main_if_or_vc_or_vlan(csb->interface));

#undef	ALTERNATE
#define	ALTERNATE	ci_ip2_subif
