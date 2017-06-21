/* $Id: cfg_ip.h,v 3.7.4.2 1996/04/15 21:15:20 bcole Exp $
 * $Source: /release/112/cvs/Xsys/ip/cfg_ip.h,v $
 *------------------------------------------------------------------
 * C F G _ I P . H
 *
 * Global configuration IP commands.
 * 
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_ip.h,v $
 * Revision 3.7.4.2  1996/04/15  21:15:20  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.7.4.1  1996/03/28  00:15:29  jenny
 * CSCdi51402:  TCP Path-MTU Discovery needs to be dynamic
 * Branch: California_branch
 * Implemented TCP PMTU dynamic discovery and PMTU timers (RFC 1191).
 * Moved ip tcp parser command into a seperated parser chain in TCP for
 * modularity.
 *
 * Revision 3.7  1996/03/11  19:25:50  ahh
 * CSCdi43020:  netbooting from not-directly connected tftp-server fails
 * Allow user to force a boot interface when spurious redirects cause
 * our automagic booting to fail.
 *
 * Revision 3.6  1996/03/04  15:30:53  dkerr
 * CSCdi50623:  IP flow cache needs better accounting granularity
 * Better show output.  Also allow flow stats to be exported.
 *
 * Revision 3.5  1996/02/01  23:33:39  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.4  1995/11/21  07:41:18  jenny
 * CSCdi43195:  TFTP: cannot specify source address
 *
 * Revision 3.3  1995/11/20  23:21:47  ahh
 * CSCdi41919:  PT: cannot specify source interface for TCP connections
 * Add source-interface option to outgoing TCP translation and telnet.
 * Also convert some parser defines to enums.
 *
 * Revision 3.2  1995/11/17  09:32:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/09/25  08:31:45  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.4  1995/08/29  21:04:17  jenny
 * CSCdi38480:  UDP: should be able to disable toy servers
 * Added new knob "service udp-small-servers" to turn on/off ECHO, DISCARD,
 * and CHARGEN ports and new knob "ip bootp server" to turn on/off BOOTP
 * service.
 *
 * Revision 2.3  1995/08/29  06:05:01  fox
 * CSCdi38536:  peer default ip address pool command does not match
 * documentation
 * Commit changes to mainline in order to match 11.0 Release
 * documentation.
 *
 * Revision 2.2  1995/08/17  01:43:13  tli
 * CSCdi38878:  DHCP: change ip local-pool to ip local pool
 *
 * Revision 2.1  1995/07/01  12:55:36  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:08:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(cfg_ip_extend_here, NONE);

/******************************************************************************
 *	ip gratuitous-arps
 *	no ip gratuitous-arps
 */
EOLS	(cip_gratuitous_arps_eol, ip_command, IP_GRATUITOUS_ARPS);
KEYWORD	(cip_gratuitous_arps, cip_gratuitous_arps_eol, cfg_ip_extend_here,
	 "gratuitous-arps",
	 "Generate gratuitous ARPs for PPP/SLIP peer addresses", PRIV_CONF);

/******************************************************************************
 * ip flow-export <ip-addr> <port-number>
 * no ip flow-export <ip-addr> <port-number>
 *
 */
EOLS    (cip_flow_addr_port_eol, ip_command, IP_FLOW_CACHE);
NUMBER  (cip_flow_portnum, cip_flow_addr_port_eol, no_alt,
         OBJ(int,1), 0, 65535, "UDP port number");
IPADDR  (cip_flow_dsthostaddr, cip_flow_portnum, no_alt,
         OBJ(paddr, 1), "Destination IP address");
NVGENS  (cip_flow_nvgen, cip_flow_dsthostaddr, ip_command, IP_FLOW_CACHE);
KEYWORD (cip_flow_keyword, cip_flow_nvgen, cip_gratuitous_arps,
         "flow-export", "Specify host/port to send flow statistics",
	 PRIV_CONF);

/******************************************************************************
 *	[no] ip host-routing
 *
 * OBJ(int,1) = TRUE if host routing, FALSE for real routing
 */
EOLS	(cip_routing_eol, ip_command, IP_ROUTING);
KEYWORD_ID_MM(cip_host_routing, cip_routing_eol, cip_flow_keyword,
	      OBJ(int, 1), TRUE,
	      "host-routing",
	      "Enable host-based routing (proxy ARP and redirect)",
	      PRIV_CONF, 5);

/******************************************************************************
 *	[no] ip routing
 *
 */
KEYWORD	(cip_routing_kwd, cip_routing_eol, NONE,
	 "routing", "Enable IP routing", PRIV_CONF);
TEST_BOOLEAN(cip_routing, cip_routing_kwd, NONE, cip_host_routing,
	     router_enable);

/******************************************************************************
 *	ip security eso-info <source> <compartment byte size> <default bit>
 *	no ip security eso-info <source> <compartment byte size> <default bit>
 */

EOLS	(cip_secur_eso_eol, ip_command, IP_SECURITY);
NUMBER	(cip_secur_eso_dbit, cip_secur_eso_eol, no_alt,
	 OBJ(int,3), 0, 1, "default bit value");
NUMBER	(cip_secur_eso_cbit, cip_secur_eso_dbit, no_alt,
	 OBJ(int,2), 1, 16, "Max # of compartment bytes");
NOPREFIX (cip_secur_eso_noprefix, cip_secur_eso_cbit, cip_secur_eso_eol);
NUMBER	(cip_secur_eso_source, cip_secur_eso_noprefix, no_alt,
	 OBJ(int,1), 0, 255, "NLESO source");
NVGENS  (cip_secur_nvgen, cip_secur_eso_source, ip_command, IP_SECURITY);
KEYWORD	(cip_secur_eso, cip_secur_nvgen, no_alt,
	 "eso-info", "Specify accreditation range table info", PRIV_CONF);
KEYWORD	(cip_secur, cip_secur_eso, cip_routing,
	 "security", "Specify system wide security information", PRIV_CONF);

/******************************************************************************
 *	ip source-route
 *	no ip source-route
 */
EOLS	(cip_srcroute_eol, ip_command, IP_SRCROUTE);
KEYWORD	(cip_source_route, cip_srcroute_eol, cip_secur,
	 "source-route", "Process packets with source routing header options",
	 PRIV_CONF);

/******************************************************************************
 *	ip subnet-zero
 *	no ip subnet-zero
 */

EOLS	(cip_subnetzero_eol, ip_command, IP_SUBNETZERO);
KEYWORD	(cip_subnetzero, cip_subnetzero_eol, cip_source_route,
	 "subnet-zero", "Allow 'subnet zero' subnets", PRIV_CONF);

/***************************************************************************/
HELP	(configure_ip_sub, cip_subnetzero,
	 "Global IP configuration subcommands:\n");
KEYWORD	(configure_ip, configure_ip_sub, ALTERNATE, "ip",
	 "Global IP configuration subcommands", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	configure_ip
