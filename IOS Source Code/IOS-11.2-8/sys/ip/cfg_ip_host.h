/* $Id: cfg_ip_host.h,v 3.1.2.3 1996/09/05 23:06:12 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ip/cfg_ip_host.h,v $
 *------------------------------------------------------------------
 * IP nameservice related commands.
 *
 * Januar 1996, Bruce Cole
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_ip_host.h,v $
 * Revision 3.1.2.3  1996/09/05  23:06:12  snyder
 * CSCdi68126:  declare some stuff in ip const
 *              228 out of data, only 4 from image
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/08/12  16:02:47  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.1  1996/04/15  21:15:23  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/15  01:40:20  bcole
 * Placeholders for 11.2 features.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 *	ip name-server  <OBJ(paddr,1)> [ <OBJ(paddr,2)> <OBJ(paddr,3)>
 *		<OBJ(paddr,4)> <OBJ(paddr,5)> <OBJ(paddr,6)> ]
 *	no ip name-server [ <OBJ(paddr,1)> <OBJ(paddr,2)> <OBJ(paddr,3)>
 *		 <OBJ(paddr,4)> <OBJ(paddr,5)> <OBJ(paddr,6)> ]
 *
 * (*OBJ(paddr,1)) - (*OBJ(paddr,6)) = <OBJ(paddr,1)> - <OBJ(paddr,6)>
 *
 * Note: There are a maximum of MAXDOMAINSERVERS (6) domain servers allowed.
 */

static const char PARSE_ip_namesrv_help[] = "Domain server IP address (maximum of 6)";

EOLNS	(cr_ip_namesrv_eol, set_ipdomainservers);

/* <addr6> */
IPADDR	(cr_ip_namesrv_addr6, cr_ip_namesrv_eol, cr_ip_namesrv_eol,
	 OBJ(paddr,6), PARSE_ip_namesrv_help);

/* <addr5> */
IPADDR	(cr_ip_namesrv_addr5, cr_ip_namesrv_addr6, cr_ip_namesrv_eol,
	 OBJ(paddr,5), PARSE_ip_namesrv_help);

/* <addr4> */
IPADDR	(cr_ip_namesrv_addr4, cr_ip_namesrv_addr5, cr_ip_namesrv_eol,
	 OBJ(paddr,4), PARSE_ip_namesrv_help);

/* <addr3> */
IPADDR	(cr_ip_namesrv_addr3, cr_ip_namesrv_addr4, cr_ip_namesrv_eol,
	 OBJ(paddr,3), PARSE_ip_namesrv_help);

/* <addr2> */
IPADDR	(cr_ip_namesrv_addr2, cr_ip_namesrv_addr3, cr_ip_namesrv_eol,
	 OBJ(paddr,2), PARSE_ip_namesrv_help);

/* Allow 'no ip name-server'  as well as 'no ip name-server <addr1> ...' */
TESTVAR	(cr_ip_namesrv_noprefix, cr_ip_namesrv_eol, no_alt,
	 NONE, NONE, NONE, sense, FALSE);

/* <addr1> */
IPADDR	(cr_ip_namesrv_addr1, cr_ip_namesrv_addr2, cr_ip_namesrv_noprefix,
	 OBJ(paddr,1), PARSE_ip_namesrv_help);

NVGENNS	(cr_ip_namesrv_nvgen, cr_ip_namesrv_addr1, set_ipdomainservers);
KEYWORD	(cr_ip_namesrv, cr_ip_namesrv_nvgen, ALTERNATE,
	 "name-server", "Specify address of name server to use", PRIV_CONF);


/******************************************************************************
 *	ip domain-name <name>
 *	no ip domain-name [<name>]
 *
 * OBJ(string,1) = <name>
 */
EOLNS	(cr_ip_domainname_eol, ip_domainname_command);
STRING	(cr_ip_domainname_get, cr_ip_domainname_eol, no_alt, 
	 OBJ(string,1), "Default domain name");
NOPREFIX (cr_ip_domainname_noprefix, cr_ip_domainname_get,
	  cr_ip_domainname_eol);
NVGENNS	(cr_ip_domainname_nvgen, cr_ip_domainname_noprefix,
	 ip_domainname_command);
KEYWORD	(cr_ip_domainname, cr_ip_domainname_nvgen, cr_ip_namesrv,
	 "domain-name",
	 "Define the default domain name", PRIV_CONF);

/******************************************************************************
 *	ip hp-host <hostname> <ipaddr>
 *	no ip hp-host <hostname> [<ipaddr>]
 *
 * OBJ(string,1) = hostname
 * (*OBJ(paddr,1)) = ipaddr
 */
EOLNS	(cr_ip_hphost_eol, iphphost_command);
IPADDR	(cr_ip_hphost_addr, cr_ip_hphost_eol, no_alt,
	 OBJ(paddr,1), "IP address of the named host");
NOPREFIX (cr_ip_hphost_noprefix, cr_ip_hphost_addr, cr_ip_hphost_eol);
STRING	(cr_ip_hphost_name, cr_ip_hphost_noprefix, no_alt,
	 OBJ(string,1), "HP host's name");
NVGENNS	(cr_ip_hphost_nvgen, cr_ip_hphost_name, iphphost_command);
KEYWORD	(cr_ip_hphost, cr_ip_hphost_nvgen, cr_ip_domainname,
	"hp-host", "Enable the HP proxy probe service", PRIV_CONF);

/******************************************************************************
 * ip host <name> [<port>] <OBJ(paddr,1)> [ <OBJ(paddr,2)> <OBJ(paddr,3)> <OBJ(paddr,4)> <OBJ(paddr,5)>
 *  <OBJ(paddr,6)> <OBJ(paddr,7)> <OBJ(paddr,8)> ]
 * no ip host <name> [[<port>] <OBJ(paddr,1)> [ <OBJ(paddr,2)> <OBJ(paddr,3)> <OBJ(paddr,4)> <OBJ(paddr,5)>
 *  <OBJ(paddr,6)> <OBJ(paddr,7)> <OBJ(paddr,8)> ]
 *
 * OBJ(int,1) = port
 * OBJ(int,2) == TRUE if a port number was specified.
 * (*OBJ(paddr,1)) - (*OBJ(paddr,8)) = addresses
 * OBJ(string,1) = name
 *
 * A maximum of MAXIPADDRESSES (currently 8) addresses can be specified.
 * For 8 addresses, it is easier to enumerate them than to build some
 * complex looping mechanism that is likely to be easily broken and
 * difficult to understand.
 *
 */

pdecl(cr_ip_host_addr1);
static const char PARSE_ip_addr_help[] = "Host IP address (maximum of 8)";

EOLNS	(cr_ip_host_eol, iphost_command);

/* <addr8> */
IPADDR	(cr_ip_host_addr8, cr_ip_host_eol, cr_ip_host_eol,
	 OBJ(paddr,8), PARSE_ip_addr_help);

/* <addr7> */
IPADDR	(cr_ip_host_addr7, cr_ip_host_addr8, cr_ip_host_eol,
	 OBJ(paddr,7), PARSE_ip_addr_help);

/* <addr6> */
IPADDR	(cr_ip_host_addr6, cr_ip_host_addr7, cr_ip_host_eol,
	 OBJ(paddr,6), PARSE_ip_addr_help);

/* <addr5> */
IPADDR	(cr_ip_host_addr5, cr_ip_host_addr6, cr_ip_host_eol,
	 OBJ(paddr,5), PARSE_ip_addr_help);

/* <addr4> */
IPADDR	(cr_ip_host_addr4, cr_ip_host_addr5, cr_ip_host_eol,
	 OBJ(paddr,4), PARSE_ip_addr_help);

/* <addr3> */
IPADDR	(cr_ip_host_addr3, cr_ip_host_addr4, cr_ip_host_eol,
	 OBJ(paddr,3), PARSE_ip_addr_help);

/* <addr2> */
IPADDR	(cr_ip_host_addr2, cr_ip_host_addr3, cr_ip_host_eol,
	 OBJ(paddr,2), PARSE_ip_addr_help);

/* [<port>] */
SET	(cr_ip_host_port_set, cr_ip_host_addr1, OBJ(int,2), TRUE);
NUMBER	(cr_ip_host_port_num, cr_ip_host_port_set, no_alt,
	 OBJ(int,1), 0, 65535, "Default telnet port number");
TESTVAR	(cr_ip_host_port, cr_ip_host_port_num, no_alt,
	 NONE, NONE, NONE, OBJ(int,2), FALSE);

/* <addr1> */
IPADDR	(cr_ip_host_addr1, cr_ip_host_addr2, cr_ip_host_port,
	 OBJ(paddr,1), PARSE_ip_addr_help);

/* <name> */
NOPREFIX (cr_ip_host_noprefix, cr_ip_host_addr1, cr_ip_host_eol);
STRING	(cr_ip_host_name, cr_ip_host_noprefix, no_alt,
	 OBJ(string,1), "Name of host");

NVGENNS	(cr_ip_host_nvgen, cr_ip_host_name, iphost_command);
KEYWORD	(cr_ip_host, cr_ip_host_nvgen, cr_ip_hphost,
	 "host", "Add an entry to the ip hostname table", PRIV_CONF);

/******************************************************************************
 *	ip domain-lookup
 * 	no ip domain-lookup
 *
 * csb->sense = TRUE = perform domain lookups
 *		FALSE = don't perform domain lookups
 */
EOLS	(cip_domainlookup_eol, ip_domainlookup_command, IP_DOMAINLOOKUP);
EOLS	(cip_domainlookup_nsap_eol, ip_domainlookup_command,
	 IP_DOMAINLOOKUP_NSAP);

KEYWORD	(cip_domainlookup_nsap, cip_domainlookup_nsap_eol,
	 cip_domainlookup_eol,
	 "nsap", "Enable IP DNS queries for CLNS NSAP addresses",
	 PRIV_CONF);
KEYWORD	(cip_domainlookup, cip_domainlookup_nsap, cr_ip_host,
	 "domain-lookup", "Enable IP Domain Name System hostname translation",
	 PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cip_domainlookup
