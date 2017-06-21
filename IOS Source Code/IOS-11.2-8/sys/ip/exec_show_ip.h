/* $Id: exec_show_ip.h,v 3.5.6.3 1996/08/12 16:02:56 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ip/exec_show_ip.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ I P . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_ip.h,v $
 * Revision 3.5.6.3  1996/08/12  16:02:56  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.5.6.2  1996/06/24  22:30:44  perryl
 * CSCdi57969:  show ip interface not showing Vaccess
 * Branch: California_branch
 *
 * Revision 3.5.6.1  1996/05/21  09:51:00  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5  1996/03/01  20:20:49  rchandra
 * CSCdi50381:  IP: show ip arp command is not flexible
 * - make 'show ip arp [<address>] [<hostname>] [<mac>] [<interface>]' work
 *
 * Revision 3.4  1996/02/01  23:33:43  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.3  1995/12/07  17:45:06  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.2  1995/11/17  09:32:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/17  01:43:15  tli
 * CSCdi38878:  DHCP: change ip local-pool to ip local pool
 *
 * Revision 2.1  1995/07/01  12:55:44  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:24:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(show_ip_extend_here, no_alt);

/******************************************************************
 * show ip local
 * 
 */
LINK_TRANS(show_ip_local_extend_here, no_alt);
KEYWORD (show_ip_local, show_ip_local_extend_here, show_ip_extend_here,
	"local", "IP local options", PRIV_USER);

/******************************************************************
 * show ip traffic
 * 
 */
EOLNS	(show_ip_traffic_eol, show_iptraffic);
KEYWORD (show_ip_traffic, show_ip_traffic_eol, show_ip_local,
	"traffic", "IP protocol statistics", PRIV_USER);

/******************************************************************
 * show ip sockets
 * 
 */
EOLNS	(show_ip_sockets_eol, show_ipsockets);
KEYWORD (show_ip_sockets, show_ip_sockets_eol, show_ip_traffic,
	"sockets", "Open IP sockets", PRIV_USER);

/******************************************************************
 * show ip redirects [<network>]
 * 
 */
EOLNS	(show_ip_redirects_eol, show_paths);
IPADDR	(show_ip_redirects_network, show_ip_redirects_eol,
	 show_ip_redirects_eol, OBJ(paddr,1), "Network to display");
KEYWORD (show_ip_redirects, show_ip_redirects_network, show_ip_sockets,
	 "redirects", "IP redirects", PRIV_USER);

/******************************************************************
 * show ip interface [brief] [<interface>] 
 * 
 * OBJ(idb,1) = <interface>
 * OBJ(int,1) = TRUE -> brief
 *		FALSE -> full
 */
EOLNS	(show_ip_interface_eol, ip_show_interface);
INTERFACE (show_ip_get_interface, show_ip_interface_eol, show_ip_interface_eol,
	   OBJ(idb,1), (IFTYPE_ANYSUB | IFTYPE_VACCESS));
SET     (show_ip_int_set_brief, show_ip_get_interface, OBJ(int,1), TRUE);
KEYWORD	(show_ip_interface_brief_short, show_ip_int_set_brief,
            show_ip_get_interface,
            "brief", "Brief summary of IP status and configuration",
            PRIV_USER);
KEYWORD_MM (show_ip_interface_brief_long, show_ip_int_set_brief,
            show_ip_get_interface,
            "brief", "Brief summary of IP status and configuration",
            PRIV_USER, 4);
TEST_INT(show_ip_interface_brief, show_ip_interface_brief_long,
	 show_ip_interface_brief_short, NONE, nbrinets);
KEYWORD (show_ip_interface, show_ip_interface_brief, show_ip_redirects,
	 common_str_interface, "IP interface status and configuration", PRIV_USER);

/******************************************************************
 * show ip arp [<interface>] [<mac-addr>] [<ip-addr>] [<hostname>]
 * 
 */
EOLS	(show_ip_arp_eol, ip_arp_show_command, SHOW_ARP);

EOLS	(show_ip_arp_addr_eol, ip_arp_show_command, SHOW_ARP_ADDR);
IPADDR_NAME(show_ip_arp_addr, show_ip_arp_addr_eol, NONE,
	    OBJ(paddr,1), "IP address or hostname of ARP entry");
TEST_MULTIPLE_FUNCS(show_ip_arp_test, show_ip_arp_addr,
		    NONE, show_ip_arp_eol);

EOLS	(show_ip_arp_mac_eol, ip_arp_show_command, SHOW_ARP_MAC);
ENADDR	(show_ip_arp_mac, show_ip_arp_mac_eol, show_ip_arp_test,
	 OBJ(hwaddr,1), "48-bit hardware address of ARP entry");

EOLS	(show_ip_arp_interface_eol, ip_arp_show_command, SHOW_ARP_INTERFACE);
INTERFACE (show_ip_arp_interface, show_ip_arp_interface_eol, show_ip_arp_mac,
	   OBJ(idb,1), IFTYPE_ANYSUB);

KEYWORD (show_ip_arp_kwd, show_ip_arp_interface, show_ip_interface,
	 "arp", "IP ARP table", PRIV_USER);

/******************************************************************/
KEYWORD (show_ip, show_ip_arp_kwd, ALTERNATE,
	 "ip", "IP information", PRIV_USER|PRIV_USER_HIDDEN);


#undef	ALTERNATE
#define	ALTERNATE	show_ip
