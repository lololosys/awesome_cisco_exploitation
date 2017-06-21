/* $Id: exec_show_ip_igmp.h,v 3.2.60.2 1996/08/12 16:03:20 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/exec_show_ip_igmp.h,v $
 *------------------------------------------------------------------
 * IGMP show commands.
 *
 * November 1993.
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_ip_igmp.h,v $
 * Revision 3.2.60.2  1996/08/12  16:03:20  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.60.1  1996/05/21  09:52:19  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2  1995/11/17  17:29:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:00:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:56:52  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.3  1995/06/21  18:18:01  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.2  1995/06/12  06:27:41  dino
 * CSCdi35152:  Cant show DVMRP default route only.
 *
 * Revision 2.1  1995/06/07  22:24:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show ip dvmrp route [<address> | <interface>]
 * 
 * OBJ(paddr,1) = optional IP address.
 * OBJ(int,1) = address supplied.
 * OBJ(idb,1) = if <interface> specified.
 */
EOLNS (show_ip_dvmrp_route_eols, dvmrp_show_route);
SET (show_ip_dvmrp_route_set, show_ip_dvmrp_route_eols, OBJ(int,1), TRUE);
IPADDR_NAME (show_ip_dvmrp_route_addr, show_ip_dvmrp_route_set,
	     show_ip_dvmrp_route_eols, OBJ(paddr,1), "IP name or address");
TEST_MULTIPLE_FUNCS(show_ip_dvmrp_route_ambig, show_ip_dvmrp_route_addr,
 		    NONE, no_alt);
INTERFACE (show_ip_dvmrp_route_idb, show_ip_dvmrp_route_eols,
	   show_ip_dvmrp_route_ambig, OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD (show_ip_dvmrp_route, show_ip_dvmrp_route_idb, no_alt,
	 "route", "DVMRP routing information", PRIV_USER);

KEYWORD (show_ip_dvmrp, show_ip_dvmrp_route, ALTERNATE, 
	 "dvmrp", "DVMRP information", PRIV_USER);

/******************************************************************
 * show ip igmp groups [<group-name> | <group-address>] [<interface-unit>]
 * 
 * OBJ(paddr,1) = optional group IP address.
 * OBJ(idb,1)   = optional interface name.
 */
pdecl(show_ip_igmp_group_opts);

EOLS	(show_ip_igmp_group_eol, igmp_show_commands, IGMP_SHOW_GROUPS);
IPADDR_NAME (show_ip_igmp_group_addr, show_ip_igmp_group_opts,
	     show_ip_igmp_group_eol, OBJ(paddr,1), "IP name or group address");
TEST_MULTIPLE_FUNCS(show_ip_igmp_group_ambig, show_ip_igmp_group_addr,
		    NONE, no_alt);
INTERFACE (show_ip_igmp_group_idb, show_ip_igmp_group_opts, 
	   show_ip_igmp_group_ambig, OBJ(idb,1), IFTYPE_ANYSUB);
NOP(show_ip_igmp_group_opts, show_ip_igmp_group_idb, NONE);

KEYWORD (show_ip_igmp_group, show_ip_igmp_group_opts, no_alt,
	 "groups", "IGMP group membership information", PRIV_USER);

/******************************************************************
 * show ip igmp interface [<interface-unit>]
 * 
 * OBJ(idb,1) = optional interface name.
 */
EOLS	(show_ip_igmp_int_eol, igmp_show_commands, IGMP_SHOW_INTERFACE);
INTERFACE (show_ip_igmp_int_idb, show_ip_igmp_int_eol, show_ip_igmp_int_eol,
	   OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD (show_ip_igmp_int, show_ip_igmp_int_idb, show_ip_igmp_group, 
	 common_str_interface, "IGMP interface information", PRIV_USER);

KEYWORD (show_ip_igmp, show_ip_igmp_int, show_ip_dvmrp,
	 "igmp", "IGMP information", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_ip_igmp
