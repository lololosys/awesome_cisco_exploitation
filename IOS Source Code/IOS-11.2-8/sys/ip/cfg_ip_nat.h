/* $Id: cfg_ip_nat.h,v 3.1.6.6 1996/08/14 05:37:20 ahh Exp $
 * $Source: /release/112/cvs/Xsys/ip/cfg_ip_nat.h,v $
 *------------------------------------------------------------------
 * C F G _ I P _ N A T . H
 *
 * Global configuration IP NAT commands.
 * 
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_ip_nat.h,v $
 * Revision 3.1.6.6  1996/08/14  05:37:20  ahh
 * CSCdi65971:  NAT: need static translations with ports
 * Allow user to configure protocol and ports in a static translation.
 * Branch: California_branch
 *
 * Revision 3.1.6.5  1996/07/30  23:42:52  ahh
 * CSCdi64604:  NAT: need support for multi-homing
 * Allow the use of route maps for choosing a global address pool.
 * Branch: California_branch
 *
 * Revision 3.1.6.4  1996/06/03  23:23:07  ahh
 * CSCdi59119:  IP: NAT should support named access-lists
 * And extended access-lists and non-contiguous address pools.
 * Branch: California_branch
 *
 * Revision 3.1.6.3  1996/05/13  23:20:34  ahh
 * CSCdi56426:  NAT: EFT bugs and feedback
 * Fix hash algorithm, fix address block coalescing, reduce up-front
 * memory allocation, drop untranslatable packets, clean up debugs.
 * Branch: California_branch
 *
 * Revision 3.1.6.2  1996/04/24  00:05:49  ahh
 * CSCdi55483:  NAT: code review comments, various bug fixes
 * Fast-path fixup, cruft removal.
 * Branch: California_branch
 *
 * Revision 3.1.6.1  1996/04/16  18:52:59  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/07  05:58:42  ahh
 * Placeholder...
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * ip nat outside destination { static <ipaddr> <ipaddr> |
 *				static { tcp | udp } <ipaddr> <port>
 *				    <ipaddr> <port> }
 *
 * no ip nat destination static <ipaddr> [ <port> ]
 *
 * OBJ(int,2) = flags for overloading, etc
 * OBJ(int,3) = { IPNAT_INSIDE, IPNAT_OUTSIDE }
 * OBJ(int,4) = { IPNAT_LOCAL, IPNAT_GLOBAL }
 * OBJ(int,5) = protocol
 * OBJ(int,6) = local port
 * OBJ(int,7) = global port
 * OBJ(paddr,1) = local address
 * OBJ(paddr,2) = global address
 */

pdecl(cip_nat_odest_sopts);

EOLS	(cip_nat_odest_eol, ip_nat_command, IPNAT_ODESTINATION);

KEYWORD_ORTEST(cip_nat_odest_noalias, cip_nat_odest_sopts, NONE,
	 OBJ(int,2), IPNAT_NOALIAS_SET,
	 "no-alias", "Do not create an alias for the local address",
	 PRIV_CONF);
KEYWORD_ORTEST(cip_nat_odest_extend, cip_nat_odest_sopts,
	 cip_nat_odest_noalias,
	 OBJ(int,2), IPNAT_EXTEND_SET,
	 "extendable", "Extend this translation when used",
	 PRIV_CONF);
NOP	(cip_nat_odest_sopts, cip_nat_odest_extend, cip_nat_odest_eol);

NUMBER	(cip_nat_odest_static_gport, cip_nat_odest_eol, no_alt,
	 OBJ(int,7), 1, 65535,
	 "Global UDP/TCP port");
IPADDR  (cip_nat_odest_global2, cip_nat_odest_static_gport, NONE,
	 OBJ(paddr,2), "Outside global IP address");
NOPREFIX(cip_nat_odest_stat_noprefix2, cip_nat_odest_global2,
	 cip_nat_odest_eol);
NUMBER	(cip_nat_odest_static_lport, cip_nat_odest_stat_noprefix2, no_alt,
	 OBJ(int,6), 1, 65535,
	 "Local UDP/TCP port");
IPADDR  (cip_nat_odest_local2, cip_nat_odest_static_lport, NONE,
	 OBJ(paddr,1), "Outside local IP address");

IPADDR  (cip_nat_odest_global, cip_nat_odest_eol, NONE,
	 OBJ(paddr,2), "Outside global IP address");
NOPREFIX(cip_nat_odest_stat_noprefix, cip_nat_odest_global,
	 cip_nat_odest_eol);
IPADDR  (cip_nat_odest_local, cip_nat_odest_stat_noprefix, NONE,
	 OBJ(paddr,1), "Outside local IP address");

KEYWORD_ID(cip_nat_odest_static_udp, cip_nat_odest_local2,
	 cip_nat_odest_local,
	 OBJ(int,5), UDP_PROT,
	 UDP_PROT_NAME, "User Datagram Protocol", PRIV_CONF);
KEYWORD_ID(cip_nat_odest_static_tcp, cip_nat_odest_local2,
	 cip_nat_odest_static_udp,
	 OBJ(int,5), TCP_PROT,
	 TCP_PROT_NAME, "Transmission Control Protocol", PRIV_CONF);

KEYWORD	(cip_nat_odest_static, cip_nat_odest_static_tcp, NONE,
	 "static",
	 "Specify static local->global mapping",
	 PRIV_CONF);

NVGENS	(cip_nat_odest_nvgen, cip_nat_odest_static,
	 ip_nat_command, IPNAT_ODESTINATION);
KEYWORD_ID(cip_nat_odestination, cip_nat_odest_nvgen, no_alt,
	 OBJ(int,4), IPNAT_LOCAL,
	 "destination",
	 "Destination address translation",
	 PRIV_CONF|PRIV_HIDDEN);

/************************************************************************
 * ip nat outside source { list <acl> pool <name> |
 *			   static <ipaddr> <ipaddr> |
 *			   static { tcp | udp } <ipaddr> <port>
 *			       <ipaddr> <port> }
 *		   
 * no ip nat outside source list <acl>
 * no ip nat outside source static <ipaddr> [ <port> ]
 *
 * OBJ(int,1) = <acl>
 * OBJ(int,2) = flags for overloading, etc
 * OBJ(int,3) = { IPNAT_INSIDE, IPNAT_OUTSIDE }
 * OBJ(int,4) = { IPNAT_LOCAL, IPNAT_GLOBAL }
 * OBJ(int,5) = protocol
 * OBJ(int,6) = global port
 * OBJ(int,7) = local port
 * OBJ(string,1) = <pool-name>
 * OBJ(string,2) = <acl-name>
 * OBJ(paddr,1) = global address
 * OBJ(paddr,2) = local address
 * OBJ(idb,1) = interface
 */

pdecl(cip_nat_osource_sopts);
pdecl(cip_nat_osource_dopts);

EOLS	(cip_nat_osource_eol, ip_nat_command, IPNAT_OSOURCE);

KEYWORD_ORTEST(cip_nat_osource_noalias, cip_nat_osource_sopts, NONE,
	 OBJ(int,2), IPNAT_NOALIAS_SET,
	 "no-alias", "Do not create an alias for the local address",
	 PRIV_CONF);
KEYWORD_ORTEST(cip_nat_osource_extend, cip_nat_osource_sopts,
	 cip_nat_osource_noalias,
	 OBJ(int,2), IPNAT_EXTEND_SET,
	 "extendable", "Extend this translation when used",
	 PRIV_CONF);
NOP	(cip_nat_osource_sopts, cip_nat_osource_extend, cip_nat_osource_eol);

NUMBER	(cip_nat_osource_static_lport, cip_nat_osource_sopts, no_alt,
	 OBJ(int,7), 1, 65535,
	 "Local UDP/TCP port");
IPADDR  (cip_nat_osource_local2, cip_nat_osource_static_lport, NONE,
	 OBJ(paddr,2), "Outside local IP address");
NOPREFIX(cip_nat_osource_stat_noprefix2, cip_nat_osource_local2,
	 cip_nat_osource_eol);
NUMBER	(cip_nat_osource_static_gport, cip_nat_osource_stat_noprefix2, no_alt,
	 OBJ(int,6), 1, 65535,
	 "Global UDP/TCP port");
IPADDR  (cip_nat_osource_global2, cip_nat_osource_static_gport, NONE,
	 OBJ(paddr,1), "Outside global IP address");

IPADDR  (cip_nat_osource_local, cip_nat_osource_sopts, NONE,
	 OBJ(paddr,2), "Outside local IP address");
NOPREFIX(cip_nat_osource_stat_noprefix, cip_nat_osource_local,
	 cip_nat_osource_eol);
IPADDR  (cip_nat_osource_global, cip_nat_osource_stat_noprefix, NONE,
	 OBJ(paddr,1), "Outside global IP address");

KEYWORD_ID(cip_nat_osource_static_udp, cip_nat_osource_global2,
	 cip_nat_osource_global,
	 OBJ(int,5), UDP_PROT,
	 UDP_PROT_NAME, "User Datagram Protocol", PRIV_CONF);
KEYWORD_ID(cip_nat_osource_static_tcp, cip_nat_osource_global2,
	 cip_nat_osource_static_udp,
	 OBJ(int,5), TCP_PROT,
	 TCP_PROT_NAME, "Transmission Control Protocol", PRIV_CONF);

KEYWORD	(cip_nat_osource_static, cip_nat_osource_static_tcp, NONE,
	 "static",
	 "Specify static global->local mapping",
	 PRIV_CONF);

KEYWORD_ORTEST(cip_nat_osource_overload, cip_nat_osource_dopts, NONE,
	 OBJ(int,2), IPNAT_OVERLOAD_SET,
	 "overload", "Overload an address translation",
	 PRIV_CONF|PRIV_HIDDEN);

NOP	(cip_nat_osource_dopts, cip_nat_osource_overload, cip_nat_osource_eol);

FORWARD_INTERFACE(cip_nat_osource_int_name, cip_nat_osource_dopts, NONE,
	 OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD	(cip_nat_osource_interface, cip_nat_osource_int_name, no_alt,
	 "interface",
	 "Specify interface for local address",
	 PRIV_CONF|PRIV_HIDDEN);

STRING	(cip_nat_osource_pool_name, cip_nat_osource_dopts, NONE,
	 OBJ(string,1), "Pool name for local addresses");
KEYWORD	(cip_nat_osource_pool, cip_nat_osource_pool_name,
	 cip_nat_osource_interface,
	 "pool",
	 "Name pool of local addresses",
	 PRIV_CONF);

NOPREFIX (cip_nat_osource_map_noprefix, cip_nat_osource_pool,
	  cip_nat_osource_eol);
STRING	(cip_nat_osource_mapname, cip_nat_osource_map_noprefix, NONE,
	 OBJ(string,3), "Route-map name");
KEYWORD	(cip_nat_osource_map, cip_nat_osource_mapname, cip_nat_osource_static,
	 "route-map",
	 "Specify route-map",
	 PRIV_CONF);

NOPREFIX (cip_nat_osource_list_noprefix, cip_nat_osource_pool,
	  cip_nat_osource_eol);
GENERAL_STRING(cip_nat_osource_listname, cip_nat_osource_list_noprefix, NONE,
	 OBJ(string,2), "Access list name for global addresses",
         STRING_HELP_CHECK);
TEST_MULTIPLE_FUNCS(cip_nat_osource_list_test, cip_nat_osource_listname,
	 no_alt, NONE);
NUMBER	(cip_nat_osource_listnum, cip_nat_osource_list_noprefix,
	 cip_nat_osource_list_test,
	 OBJ(int,1), MINFASTACC, MAXSLOWACC,
	 "Access list number for global addresses");
KEYWORD	(cip_nat_osource_list, cip_nat_osource_listnum, cip_nat_osource_map,
	 "list",
	 "Specify access list describing global addresses",
	 PRIV_CONF);

NVGENS	(cip_nat_osource_nvgen, cip_nat_osource_list,
	 ip_nat_command, IPNAT_OSOURCE);
KEYWORD_ID(cip_nat_osource, cip_nat_osource_nvgen, cip_nat_odestination,
	 OBJ(int,4), IPNAT_GLOBAL,
	 "source",
	 "Source address translation",
	 PRIV_CONF);

KEYWORD_ID(cip_nat_outside, cip_nat_osource, no_alt,
	 OBJ(int,3), IPNAT_OUTSIDE,
	 "outside",
	 "Outside address translation",
	 PRIV_CONF);

/************************************************************************
 * ip nat inside destination { list <acl> pool <name> |
 *			       static <ipaddr> <ipaddr> |
 *			       static { tcp | udp } <ipaddr> <port>
 *				   <ipaddr> <port> }
 *		   
 * no ip nat inside destination list <acl>
 * no ip nat inside destination static <ipaddr> [ <port> ]
 *
 * OBJ(int,1) = <acl>
 * OBJ(int,3) = { IPNAT_INSIDE, IPNAT_OUTSIDE }
 * OBJ(int,4) = { IPNAT_LOCAL, IPNAT_GLOBAL }
 * OBJ(int,5) = protocol
 * OBJ(int,6) = global port
 * OBJ(int,7) = local port
 * OBJ(string,1) = <pool-name>
 * OBJ(string,2) = <acl-name>
 * OBJ(paddr,1) = global address
 * OBJ(paddr,2) = local address
 */

pdecl(cip_nat_idest_sopts);
pdecl(cip_nat_idest_dopts);

EOLS	(cip_nat_idest_eol, ip_nat_command, IPNAT_IDESTINATION);

KEYWORD_ORTEST(cip_nat_idest_noalias, cip_nat_idest_sopts, NONE,
	 OBJ(int,2), IPNAT_NOALIAS_SET,
	 "no-alias", "Do not create an alias for the global address",
	 PRIV_CONF);
KEYWORD_ORTEST(cip_nat_idest_extend, cip_nat_idest_sopts,
	 cip_nat_odest_noalias,
	 OBJ(int,2), IPNAT_EXTEND_SET,
	 "extendable", "Extend this translation when used",
	 PRIV_CONF);
NOP	(cip_nat_idest_sopts, cip_nat_idest_extend, cip_nat_idest_eol);

NUMBER	(cip_nat_idest_static_lport, cip_nat_idest_sopts, no_alt,
	 OBJ(int,7), 1, 65535,
	 "Local UDP/TCP port");
IPADDR  (cip_nat_idest_local2, cip_nat_idest_static_lport, NONE,
	 OBJ(paddr,2), "Outside local IP address");
NOPREFIX(cip_nat_idest_stat_noprefix2, cip_nat_idest_local2,
	 cip_nat_idest_eol);
NUMBER	(cip_nat_idest_static_gport, cip_nat_idest_stat_noprefix2, no_alt,
	 OBJ(int,6), 1, 65535,
	 "Global UDP/TCP port");
IPADDR  (cip_nat_idest_global2, cip_nat_idest_static_gport, NONE,
	 OBJ(paddr,1), "Outside global IP address");

IPADDR  (cip_nat_idest_local, cip_nat_idest_sopts, NONE,
	 OBJ(paddr,2), "Inside local IP address");
NOPREFIX(cip_nat_idest_stat_noprefix, cip_nat_idest_local,
	 cip_nat_idest_eol);
IPADDR  (cip_nat_idest_global, cip_nat_idest_stat_noprefix, NONE,
	 OBJ(paddr,1), "Inside global IP address");

KEYWORD_ID(cip_nat_idest_static_udp, cip_nat_idest_global2,
	 cip_nat_idest_global,
	 OBJ(int,5), UDP_PROT,
	 UDP_PROT_NAME, "User Datagram Protocol", PRIV_CONF);
KEYWORD_ID(cip_nat_idest_static_tcp, cip_nat_idest_global2,
	 cip_nat_idest_static_udp,
	 OBJ(int,5), TCP_PROT,
	 TCP_PROT_NAME, "Transmission Control Protocol", PRIV_CONF);

KEYWORD	(cip_nat_idest_static, cip_nat_idest_static_tcp, NONE,
	 "static",
	 "Specify static global->local mapping",
	 PRIV_CONF|PRIV_HIDDEN);

NOP	(cip_nat_idest_dopts, cip_nat_idest_eol, no_alt);

FORWARD_INTERFACE(cip_nat_idest_int_name, cip_nat_idest_dopts, NONE,
	 OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD	(cip_nat_idest_interface, cip_nat_idest_int_name, no_alt,
	 "interface",
	 "Specify interface for local address",
	 PRIV_CONF|PRIV_HIDDEN);

STRING	(cip_nat_idest_pool_name, cip_nat_idest_eol, NONE,
	 OBJ(string,1), "Pool name for local addresses");
KEYWORD (cip_nat_idest_pool, cip_nat_idest_pool_name, cip_nat_idest_interface,
	 "pool",
	 "Name pool of local addresses",
	 PRIV_CONF);

NOPREFIX (cip_nat_idest_list_noprefix, cip_nat_idest_pool,
	  cip_nat_idest_eol);
GENERAL_STRING(cip_nat_idest_listname, cip_nat_idest_list_noprefix, NONE,
	 OBJ(string,2), "Access list name for global addresses",
	 STRING_HELP_CHECK);
TEST_MULTIPLE_FUNCS(cip_nat_idest_list_test, cip_nat_idest_listname,
	 no_alt, NONE);
NUMBER	(cip_nat_idest_listnum, cip_nat_idest_list_noprefix,
	 cip_nat_idest_list_test,
	 OBJ(int,1), MINFASTACC, MAXSLOWACC,
	 "Access list number for global addresses");
KEYWORD	(cip_nat_idest_list, cip_nat_idest_listnum, cip_nat_idest_static,
	 "list",
	 "Specify access list describing global addresses",
	 PRIV_CONF);

NVGENS	(cip_nat_idest_nvgen, cip_nat_idest_list,
	 ip_nat_command, IPNAT_IDESTINATION);
KEYWORD_ID(cip_nat_idestination, cip_nat_idest_nvgen, no_alt,
	 OBJ(int,4), IPNAT_GLOBAL,
	 "destination",
	 "Destination address translation",
	 PRIV_CONF);

/************************************************************************
 * ip nat inside source { list <acl> pool <name> |
 *			  static <ipaddr> <ipaddr> } [ overload ] |
 *			  static { tcp | udp } <ipaddr> <port>
 *			      <ipaddr> <port> |
 *			  route-map <name> pool <name> }
 *		   
 * no ip nat inside source list <acl>
 * no ip nat inside source static <ipaddr> [ <port> ]
 *
 * OBJ(int,1) = <acl>
 * OBJ(int,2) = flags for overloading, etc
 * OBJ(int,3) = { IPNAT_INSIDE, IPNAT_OUTSIDE }
 * OBJ(int,4) = { IPNAT_LOCAL, IPNAT_GLOBAL }
 * OBJ(int,5) = protocol
 * OBJ(int,6) = local port
 * OBJ(int,7) = global port
 * OBJ(string,1) = <pool-name>
 * OBJ(string,2) = <acl-name>
 * OBJ(string,3) = <map-name>
 * OBJ(paddr,1) = local address
 * OBJ(paddr,2) = global address
 * OBJ(idb,1) = interface
 */

pdecl(cip_nat_isource_sopts);
pdecl(cip_nat_isource_dopts);

EOLS	(cip_nat_isource_eol, ip_nat_command, IPNAT_ISOURCE);

KEYWORD_ORTEST(cip_nat_isource_noalias, cip_nat_isource_sopts, NONE,
	 OBJ(int,2), IPNAT_NOALIAS_SET,
	 "no-alias", "Do not create an alias for the global address",
	 PRIV_CONF);
KEYWORD_ORTEST(cip_nat_isource_extend, cip_nat_isource_sopts,
	 cip_nat_isource_noalias,
	 OBJ(int,2), IPNAT_EXTEND_SET,
	 "extendable", "Extend this translation when used",
	 PRIV_CONF);
NOP	(cip_nat_isource_sopts, cip_nat_isource_extend, cip_nat_isource_eol);

NUMBER	(cip_nat_isource_static_gport, cip_nat_isource_sopts, no_alt,
	 OBJ(int,7), 1, 65535,
	 "Global UDP/TCP port");
IPADDR  (cip_nat_isource_global2, cip_nat_isource_static_gport, NONE,
	 OBJ(paddr,2), "Inside global IP address");
NOPREFIX(cip_nat_isource_stat_noprefix2, cip_nat_isource_global2,
	 cip_nat_isource_eol);
NUMBER	(cip_nat_isource_static_lport, cip_nat_isource_stat_noprefix2, no_alt,
	 OBJ(int,6), 1, 65535,
	 "Local UDP/TCP port");
IPADDR  (cip_nat_isource_local2, cip_nat_isource_static_lport, NONE,
	 OBJ(paddr,1), "Inside local IP address");

IPADDR  (cip_nat_isource_global, cip_nat_isource_sopts, NONE,
	 OBJ(paddr,2), "Inside global IP address");
NOPREFIX(cip_nat_isource_stat_noprefix, cip_nat_isource_global,
	 cip_nat_isource_eol);
IPADDR  (cip_nat_isource_local, cip_nat_isource_stat_noprefix, NONE,
	 OBJ(paddr,1), "Inside local IP address");

KEYWORD_ID(cip_nat_isource_static_udp, cip_nat_isource_local2,
	 cip_nat_isource_local,
	 OBJ(int,5), UDP_PROT,
	 UDP_PROT_NAME, "User Datagram Protocol", PRIV_CONF);
KEYWORD_ID(cip_nat_isource_static_tcp, cip_nat_isource_local2,
	 cip_nat_isource_static_udp,
	 OBJ(int,5), TCP_PROT,
	 TCP_PROT_NAME, "Transmission Control Protocol", PRIV_CONF);

KEYWORD	(cip_nat_isource_static, cip_nat_isource_static_tcp, NONE,
	 "static",
	 "Specify static local->global mapping",
	 PRIV_CONF);

KEYWORD_ORTEST(cip_nat_isource_overload, cip_nat_isource_dopts, NONE,
	 OBJ(int,2), IPNAT_OVERLOAD_SET,
	 "overload", "Overload an address translation",
	 PRIV_CONF);
NOP	(cip_nat_isource_dopts, cip_nat_isource_overload, cip_nat_isource_eol);

FORWARD_INTERFACE(cip_nat_isource_int_name, cip_nat_isource_dopts, NONE,
	 OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD	(cip_nat_isource_interface, cip_nat_isource_int_name, no_alt,
	 "interface",
	 "Specify interface for global address",
	 PRIV_CONF);

STRING	(cip_nat_isource_pool_name, cip_nat_isource_dopts, NONE,
	 OBJ(string,1), "Pool name for global addresses");
KEYWORD	(cip_nat_isource_pool, cip_nat_isource_pool_name,
	 cip_nat_isource_interface,
	 "pool",
	 "Name pool of global addresses",
	 PRIV_CONF);

NOPREFIX (cip_nat_isource_map_noprefix, cip_nat_isource_pool,
	  cip_nat_isource_eol);
STRING	(cip_nat_isource_mapname, cip_nat_isource_map_noprefix, NONE,
	 OBJ(string,3), "Route-map name");
KEYWORD	(cip_nat_isource_map, cip_nat_isource_mapname, cip_nat_isource_static,
	 "route-map",
	 "Specify route-map",
	 PRIV_CONF);

NOPREFIX (cip_nat_isource_list_noprefix, cip_nat_isource_pool,
	  cip_nat_isource_eol);
GENERAL_STRING(cip_nat_isource_listname, cip_nat_isource_list_noprefix, NONE,
	 OBJ(string,2), "Access list name for local addresses",
	 STRING_HELP_CHECK);
TEST_MULTIPLE_FUNCS(cip_nat_isource_list_test, cip_nat_isource_listname,
	 no_alt, NONE);
NUMBER	(cip_nat_isource_listnum, cip_nat_isource_list_noprefix,
	 cip_nat_isource_list_test,
	 OBJ(int,1), MINFASTACC, MAXSLOWACC,
	 "Access list number for local addresses");
KEYWORD	(cip_nat_isource_list, cip_nat_isource_listnum, cip_nat_isource_map,
	 "list",
	 "Specify access list describing local addresses",
	 PRIV_CONF);

NVGENS	(cip_nat_isource_nvgen, cip_nat_isource_list,
	 ip_nat_command, IPNAT_ISOURCE);
KEYWORD_ID(cip_nat_isource, cip_nat_isource_nvgen, cip_nat_idestination,
	 OBJ(int,4), IPNAT_LOCAL,
	 "source",
	 "Source address translation",
	 PRIV_CONF);

KEYWORD_ID(cip_nat_inside, cip_nat_isource, cip_nat_outside,
	 OBJ(int,3), IPNAT_INSIDE,
	 "inside",
	 "Inside address translation",
	 PRIV_CONF);

/************************************************************************
 * ip nat pool <name> { netmask <ipaddr> | prefix-length <prefix-len> }
 *		      [ type { rotary } | {match-host} ]
 * no ip nat pool <name>
 *
 * OBJ(string,1) = <name>
 * OBJ(paddr,1) = start address
 * OBJ(paddr,2) = end address
 * OBJ(paddr,3) = netmask
 * OBJ(int,1) = pool type
 * OBJ(int,2) = prefix length
 */

EOLS	(cip_nat_pool_eol, ip_nat_command, IPNAT_POOL);
KEYWORD_ID(cip_nat_pool_oneforone, cip_nat_pool_eol, NONE,
         OBJ(int,1), IPNAT_POOL_MATCHHOST,
         "match-host",
         "Keep host numbers the same after translation",
         PRIV_CONF);
KEYWORD_ID(cip_nat_pool_rotary, cip_nat_pool_eol, cip_nat_pool_oneforone,
	 OBJ(int,1), IPNAT_POOL_ROTARY,
	 "rotary",
	 "Rotary address pool",
	 PRIV_CONF);
KEYWORD	(cip_nat_pool_type, cip_nat_pool_rotary, cip_nat_pool_eol,
	 "type",
	 "Specify the pool type",
	 PRIV_CONF);
SET	(cip_nat_pool_set, cip_nat_pool_type, OBJ(int,1),
	 IPNAT_POOL_GENERIC);

NUMBER  (cip_nat_pool_prefix_len, cip_nat_pool_set, NONE,
	 OBJ(int,2), 1, 32, "Prefix length");
KEYWORD	(cip_nat_pool_prefix, cip_nat_pool_prefix_len, NONE,
	 "prefix-length",
	 "Specify the prefix length",
	 PRIV_CONF);

IPADDR	(cip_nat_pool_mask, cip_nat_pool_set, NONE,
	 OBJ(paddr,3), "Network mask");
KEYWORD	(cip_nat_pool_netmask, cip_nat_pool_mask, cip_nat_pool_prefix,
	 "netmask",
	 "Specify the network mask",
	 PRIV_CONF);

IPADDR	(cip_nat_pool_end, cip_nat_pool_netmask, NONE,
	 OBJ(paddr,2), "End IP address");
IPADDR	(cip_nat_pool_start, cip_nat_pool_end, cip_nat_pool_netmask,
	 OBJ(paddr,1), "Start IP address");

NOPREFIX(cip_nat_pool_noprefix, cip_nat_pool_start,
	  cip_nat_pool_eol);
STRING	(cip_nat_pool_name, cip_nat_pool_noprefix, NONE,
	 OBJ(string,1), "Pool name");
NVGENS	(cip_nat_pool_nvgen, cip_nat_pool_name,
	 ip_nat_command, IPNAT_POOL);

KEYWORD	(cip_nat_pool, cip_nat_pool_nvgen, cip_nat_inside,
	 "pool",
	 "Define pool of addresses",
	 PRIV_CONF);

/******************************************************************************
 * [no] ip nat translation max-entries <number>
 *
 * OBJ(int,1) = number
 */

PARAMS_KEYONLY(cip_nat_max_entries, NONE, "max-entries",
	       OBJ(int,1), 1, MAXINT,
	       ip_nat_command, IPNAT_MAX_ENTRIES,
	       "Specify maximum number of NAT entries",
	       "Number of entries", PRIV_CONF);

/******************************************************************************
 * [no] ip nat translation icmp-timeout { <seconds> | never }
 *
 * OBJ(int,1) = time
 * OBJ(int,2) = which timer
 * OBJ(int,3) = timeout type
 */

EOLS	(cip_nat_icmp_timeout_eol, ip_nat_command, IPNAT_TIMEOUT_CMD);
KEYWORD_ID(cip_nat_icmp_timeout_never, cip_nat_icmp_timeout_eol, NONE,
	 OBJ(int,3), IPNAT_TIMEOUT_NEVER,
	 "never",
	 "Never timeout",
	 PRIV_CONF);
NUMBER	(cip_nat_icmp_timeout_val, cip_nat_icmp_timeout_eol,
	 cip_nat_icmp_timeout_never,
	 OBJ(int,1), 0, MAXINT,
	 "Timeout in seconds");
SET	(cip_nat_icmp_timeout_set, cip_nat_icmp_timeout_val,
	 OBJ(int,3), IPNAT_TIMEOUT_VALUE);
NOPREFIX(cip_nat_icmp_timeout_noprefix, cip_nat_icmp_timeout_set,
	  cip_nat_icmp_timeout_eol);
NVGENS	(cip_nat_icmp_timeout_nvgen, cip_nat_icmp_timeout_noprefix,
	 ip_nat_command, IPNAT_TIMEOUT_CMD);
KEYWORD_ID(cip_nat_icmp_timeout, cip_nat_icmp_timeout_nvgen,
	 cip_nat_max_entries,
	 OBJ(int,2), IPNAT_ICMP_TIMEOUT,
	 "icmp-timeout",
	 "Specify timeout for NAT ICMP flows",
	 PRIV_CONF);

/******************************************************************************
 * [no] ip nat translation dns-timeout { <seconds> | never }
 *
 * OBJ(int,1) = time
 * OBJ(int,2) = which timer
 * OBJ(int,3) = timeout type
 */

EOLS	(cip_nat_dns_timeout_eol, ip_nat_command, IPNAT_TIMEOUT_CMD);
KEYWORD_ID(cip_nat_dns_timeout_never, cip_nat_dns_timeout_eol, NONE,
	 OBJ(int,3), IPNAT_TIMEOUT_NEVER,
	 "never", "Never timeout", PRIV_CONF);
NUMBER	(cip_nat_dns_timeout_val, cip_nat_dns_timeout_eol,
	 cip_nat_dns_timeout_never,
	 OBJ(int,1), 0, MAXINT,
	 "Timeout in seconds");
SET	(cip_nat_dns_timeout_set, cip_nat_dns_timeout_val,
	 OBJ(int,3), IPNAT_TIMEOUT_VALUE);
NOPREFIX(cip_nat_dns_timeout_noprefix, cip_nat_dns_timeout_set,
	  cip_nat_dns_timeout_eol);
NVGENS	(cip_nat_dns_timeout_nvgen, cip_nat_dns_timeout_noprefix,
	 ip_nat_command, IPNAT_TIMEOUT_CMD);
KEYWORD_ID(cip_nat_dns_timeout, cip_nat_dns_timeout_nvgen,
	 cip_nat_icmp_timeout,
	 OBJ(int,2), IPNAT_DNS_TIMEOUT,
	 "dns-timeout", 
	 "Specify timeout for NAT DNS flows",
	 PRIV_CONF);

/******************************************************************************
 * [no] ip nat translation syn-timeout { <seconds> | never }
 *
 * OBJ(int,1) = time
 * OBJ(int,2) = which timer
 * OBJ(int,3) = timeout type
 */

EOLS	(cip_nat_syn_timeout_eol, ip_nat_command, IPNAT_TIMEOUT_CMD);
KEYWORD_ID(cip_nat_syn_timeout_never, cip_nat_dns_timeout_eol, NONE,
	 OBJ(int,3), IPNAT_TIMEOUT_NEVER,
	 "never", "Never timeout", PRIV_CONF);
NUMBER	(cip_nat_syn_timeout_val, cip_nat_syn_timeout_eol,
	 cip_nat_syn_timeout_never,
	 OBJ(int,1), 0, MAXINT,
	 "Timeout in seconds");
SET	(cip_nat_syn_timeout_set, cip_nat_syn_timeout_val,
	 OBJ(int,3), IPNAT_TIMEOUT_VALUE);
NOPREFIX(cip_nat_syn_timeout_noprefix, cip_nat_syn_timeout_set,
	  cip_nat_syn_timeout_eol);
NVGENS	(cip_nat_syn_timeout_nvgen, cip_nat_syn_timeout_noprefix,
	 ip_nat_command, IPNAT_TIMEOUT_CMD);
KEYWORD_ID(cip_nat_syn_timeout, cip_nat_syn_timeout_nvgen,
	 cip_nat_dns_timeout,
	 OBJ(int,2), IPNAT_SYN_TIMEOUT,
	 "syn-timeout",
	 "Specify timeout for NAT TCP flows after a SYN and no further data",
	 PRIV_CONF);

/******************************************************************************
 * [no] ip nat translation finrst-timeout { <seconds> | never }
 *
 * OBJ(int,1) = time
 * OBJ(int,2) = which timer
 * OBJ(int,3) = timeout type
 */

EOLS	(cip_nat_finrst_timeout_eol, ip_nat_command, IPNAT_TIMEOUT_CMD);
KEYWORD_ID(cip_nat_finrst_timeout_never, cip_nat_dns_timeout_eol, NONE,
	 OBJ(int,3), IPNAT_TIMEOUT_NEVER,
	 "never", "Never timeout", PRIV_CONF);
NUMBER	(cip_nat_finrst_timeout_val, cip_nat_finrst_timeout_eol,
	 cip_nat_finrst_timeout_never,
	 OBJ(int,1), 0, MAXINT,
	 "Timeout in seconds");
SET	(cip_nat_finrst_timeout_set, cip_nat_finrst_timeout_val,
	 OBJ(int,3), IPNAT_TIMEOUT_VALUE);
NOPREFIX(cip_nat_finrst_timeout_noprefix, cip_nat_finrst_timeout_set,
	  cip_nat_finrst_timeout_eol);
NVGENS	(cip_nat_finrst_timeout_nvgen, cip_nat_finrst_timeout_noprefix,
	 ip_nat_command, IPNAT_TIMEOUT_CMD);
KEYWORD_ID(cip_nat_finrst_timeout, cip_nat_finrst_timeout_nvgen,
	 cip_nat_syn_timeout,
	 OBJ(int,2), IPNAT_FINRST_TIMEOUT,
	 "finrst-timeout",
	 "Specify timeout for NAT TCP flows after a FIN or RST",
	 PRIV_CONF);

/******************************************************************************
 * [no] ip nat translation udp-timeout { <seconds> | never }
 *
 * OBJ(int,1) = time
 * OBJ(int,2) = which timer
 * OBJ(int,3) = timeout type
 */

EOLS	(cip_nat_udp_timeout_eol, ip_nat_command, IPNAT_TIMEOUT_CMD);
KEYWORD_ID(cip_nat_udp_timeout_never, cip_nat_udp_timeout_eol, NONE,
	 OBJ(int,3), IPNAT_TIMEOUT_NEVER,
	 "never", "Never timeout", PRIV_CONF);
NUMBER	(cip_nat_udp_timeout_val, cip_nat_udp_timeout_eol,
	 cip_nat_udp_timeout_never,
	 OBJ(int,1), 0, MAXINT,
	 "Timeout in seconds");
SET	(cip_nat_udp_timeout_set, cip_nat_udp_timeout_val,
	 OBJ(int,3), IPNAT_TIMEOUT_VALUE);
NOPREFIX(cip_nat_udp_timeout_noprefix, cip_nat_udp_timeout_set,
	  cip_nat_udp_timeout_eol);
NVGENS	(cip_nat_udp_timeout_nvgen, cip_nat_udp_timeout_noprefix,
	 ip_nat_command, IPNAT_TIMEOUT_CMD);
KEYWORD_ID(cip_nat_udp_timeout, cip_nat_udp_timeout_nvgen,
	 cip_nat_finrst_timeout,
	 OBJ(int,2), IPNAT_UDP_TIMEOUT,
	 "udp-timeout",
	 "Specify timeout for NAT UDP flows",
	 PRIV_CONF);

/******************************************************************************
 * [no] ip nat translation tcp-timeout { <seconds> | never }
 *
 * OBJ(int,1) = time
 * OBJ(int,2) = which timer
 * OBJ(int,3) = timeout type
 */

EOLS	(cip_nat_tcp_timeout_eol, ip_nat_command, IPNAT_TIMEOUT_CMD);
KEYWORD_ID(cip_nat_tcp_timeout_never, cip_nat_tcp_timeout_eol, NONE,
	 OBJ(int,3), IPNAT_TIMEOUT_NEVER,
	 "never", "Never timeout", PRIV_CONF);
NUMBER	(cip_nat_tcp_timeout_val, cip_nat_tcp_timeout_eol,
	 cip_nat_tcp_timeout_never,
	 OBJ(int,1), 0, MAXINT,
	 "Timeout in seconds");
SET	(cip_nat_tcp_timeout_set, cip_nat_tcp_timeout_val,
	 OBJ(int,3), IPNAT_TIMEOUT_VALUE);
NOPREFIX(cip_nat_tcp_timeout_noprefix, cip_nat_tcp_timeout_set,
	  cip_nat_tcp_timeout_eol);
NVGENS	(cip_nat_tcp_timeout_nvgen, cip_nat_tcp_timeout_noprefix,
	 ip_nat_command, IPNAT_TIMEOUT_CMD);
KEYWORD_ID(cip_nat_tcp_timeout, cip_nat_tcp_timeout_nvgen,
	 cip_nat_udp_timeout,
	 OBJ(int,2), IPNAT_TCP_TIMEOUT,
	 "tcp-timeout",
	 "Specify timeout for NAT TCP flows",
	 PRIV_CONF);

/******************************************************************************
 * [no] ip nat translation timeout { <seconds> | never }
 *
 * OBJ(int,1) = time
 * OBJ(int,2) = which timer
 * OBJ(int,3) = timeout type
 */

EOLS	(cip_nat_trans_timeout_eol, ip_nat_command, IPNAT_TIMEOUT_CMD);
KEYWORD_ID(cip_nat_trans_timeout_never, cip_nat_trans_timeout_eol, NONE,
	 OBJ(int,3), IPNAT_TIMEOUT_NEVER,
	 "never", "Never timeout", PRIV_CONF);
NUMBER	(cip_nat_trans_timeout_val, cip_nat_trans_timeout_eol,
	 cip_nat_trans_timeout_never,
	 OBJ(int,1), 0, MAXINT,
	 "Timeout in seconds");
SET	(cip_nat_trans_timeout_set, cip_nat_trans_timeout_val,
	 OBJ(int,3), IPNAT_TIMEOUT_VALUE);
NOPREFIX(cip_nat_trans_timeout_noprefix, cip_nat_trans_timeout_set,
	  cip_nat_trans_timeout_eol);
NVGENS	(cip_nat_trans_timeout_nvgen, cip_nat_trans_timeout_noprefix,
	 ip_nat_command, IPNAT_TIMEOUT_CMD);
KEYWORD_ID(cip_nat_trans_timeout, cip_nat_trans_timeout_nvgen,
	 cip_nat_tcp_timeout,
	 OBJ(int,2), IPNAT_TIMEOUT,
	 "timeout",
	 "Specify timeout for dynamic NAT translations",
	 PRIV_CONF);

KEYWORD	(cip_nat_trans, cip_nat_trans_timeout, cip_nat_pool,
	 "translation", "NAT translation entry configuration",
	 PRIV_CONF);

KEYWORD	(cip_nat, cip_nat_trans, ALTERNATE,
	 "nat", "NAT configuration commands",
	 PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cip_nat
