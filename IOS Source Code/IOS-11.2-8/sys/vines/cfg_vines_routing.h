/* $Id: cfg_vines_routing.h,v 3.3 1995/11/27 21:25:07 widmer Exp $
 * $Source: /swtal/cherf.111/ios/sys/vines/cfg_vines_routing.h,v $
 *------------------------------------------------------------------
 * C F G _ V I N E S _ R O U T I N G . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_vines_routing.h,v $
 * Revision 3.3  1995/11/27  21:25:07  widmer
 * CSCdi43607:  Add keyword_option macro to parse multiple keywords
 * Also create permit_deny macro using keyword_option macro to parse
 * permit/deny keywords.
 * Change "ip security multilevel" command to use keyword_option macro.
 *
 * Revision 3.2  1995/11/17  17:57:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:44:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:44:04  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:14:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************************
 * Lists 1-100:
 *    vines access-list <list> { permit | deny }
 *	ip <src> <src-mask> <dest> <dest-mask>
 *
 *    vines access-list <list> { permit | deny }
 *	<protocol> <src> <src-mask> <src-port> <dest> <dest-mask> <dest-port>
 *
 * Lists 101-200:
 *    vines access-list <list> { permit | deny }
 *	<protocol>
 *      <src> <src-mask> <src-port> <src-port-mask>
 *      <dest> <dest-mask> <dest-port> <dest-port-mask>
 *
 * All Lists:
 *    no vines access-list <list>
 *
 * <protocol> == { 1-255 | SPP | RTP | ARP | IPC | ICP }
 *
 *
 * OBJ(int,1)  == access list number
 * OBJ(int,2)  == TRUE for permit, FALSE for deny
 * OBJ(int,3)  == protocol type (or number)
 * OBJ(int,10) == Protocol type "IP" was specified
 *
 */

/***************************************************************************
 * Vines Standard Access List (1-100: protocol type 'IPC' or 'SPP')
 *	Source address, source mask, source port,
 *	Destination address, destination mask, destination port,
 *	Then EOL.
 *
 * Vines Standard Access List (1-100: any other protocol type)
 *	Source address, source mask,
 *	Destination address, destination mask,
 *	Then EOL.
 *
 * Vines Extended Access List (101-200)
 *	Source address, source mask, source port, source port mask,
 *	Destination address, destination mask, destination port,
 *	destination port mask,
 *	Then EOL.
 *
 * Vines Simple Access List (201-300)
 *	address,  mask, then EOL.
 *
 * Protocol name defines should map into the appropriate protocol type-codes.
 */


EOLS	(cr_vinesAL_eol, vines_command, VINES_ACCESSLIST);

/* Destination address, mask, and port.  Also port mask if extended */
HEXDIGIT(cr_vinesAL_destportmask, cr_vinesAL_eol, no_alt,
	 OBJ(int,7), 0, 0xFFFF, "Destination port mask");
TESTVAR	(cr_vinesAL_destextnd, cr_vinesAL_eol, NONE, cr_vinesAL_eol,
	 cr_vinesAL_destportmask, NONE, OBJ(int,1), VINESMAXOACC);
HEXDIGIT(cr_vinesAL_destport, cr_vinesAL_destextnd, no_alt,
	 OBJ(int,6), 0, 0xFFFF, "Destination port");
SPLIT	(cr_vinesAL_desttest2, cr_vinesAL_destport, cr_vinesAL_eol,
	 OBJ(int,3), VINES_SPP);
SPLIT	(cr_vinesAL_desttest1, cr_vinesAL_destport, cr_vinesAL_desttest2,
	 OBJ(int,3), VINES_IPC);
VINADDR	(cr_vinesAL_destmsk, cr_vinesAL_desttest1, no_alt,
	 OBJ(paddr,4), "Destination mask");
VINADDR	(cr_vinesAL_dest, cr_vinesAL_destmsk, no_alt,
	 OBJ(paddr,3), "Destination address");

/* Source address, mask, and port.  Also port mask if extended */
HEXDIGIT(cr_vinesAL_srcportmask, cr_vinesAL_dest, no_alt,
	 OBJ(int,5), 0, 0xFFFF, "Source port mask");
TESTVAR	(cr_vinesAL_srcextnd, cr_vinesAL_dest, NONE, cr_vinesAL_dest,
	 cr_vinesAL_srcportmask, NONE, OBJ(int,1), VINESMAXOACC);
HEXDIGIT(cr_vinesAL_srcport, cr_vinesAL_srcextnd, no_alt,
	 OBJ(int,4), 0, 0xFFFF, "Source port");
SPLIT	(cr_vinesAL_srctest2, cr_vinesAL_srcport, cr_vinesAL_dest,
	 OBJ(int,3), VINES_SPP);
SPLIT	(cr_vinesAL_srctest1, cr_vinesAL_srcport, cr_vinesAL_srctest2,
	 OBJ(int,3), VINES_IPC);
VINADDR	(cr_vinesAL_srcmsk, cr_vinesAL_srctest1, no_alt,
	 OBJ(paddr,2), "Source mask");
VINADDR	(cr_vinesAL_src, cr_vinesAL_srcmsk, no_alt,
	 OBJ(paddr,1), "Source address");

/* Protocol types: { 1-255 | ARP | ICP | IP | IPC | RTP | SPP } */
NUMBER	(cr_vinesAL_proto, cr_vinesAL_src, no_alt,
	 OBJ(int,3), 1, 255, "Protocol type-code");

KEYWORD_ID(cr_vinesAL_icp, cr_vinesAL_src, cr_vinesAL_proto,
	   OBJ(int,3), VINES_ICP,
	   "icp", "Filter ICP protocol packets", PRIV_CONF);

KEYWORD_ID(cr_vinesAL_arp, cr_vinesAL_src, cr_vinesAL_icp,
	   OBJ(int,3), VINES_ARP,
	   "arp", "Filter Address Resolution protocol packets", PRIV_CONF);

KEYWORD_ID(cr_vinesAL_rtp, cr_vinesAL_src, cr_vinesAL_arp,
	   OBJ(int,3), VINES_RTP,
	   "rtp", "Filter Routing Table protocol packets", PRIV_CONF);

KEYWORD_ID(cr_vinesAL_spp, cr_vinesAL_src, cr_vinesAL_rtp,
	   OBJ(int,3), VINES_SPP,
	   "spp", "Filter Sequence Packet protocol packets", PRIV_CONF);

/* When parsing 'ipc', first check that 'ip' hasn't also matched.
 * if 'ip' matched, then OBJ(int,3) will be 1 and we should
 * skip this keyword.
 */
KEYWORD_ID(cr_vinesAL_ipc, cr_vinesAL_src, cr_vinesAL_spp,
	   OBJ(int,3), VINES_IPC,
	   "ipc", "Filter Interprocess Communication packets", PRIV_CONF);
TESTVAR	(cr_vinesAL_ipctest, cr_vinesAL_spp, cr_vinesAL_ipc,
	 NONE, NONE, NONE, OBJ(int,10), TRUE);

SET	(cr_vinesAL_ip_set, cr_vinesAL_src, OBJ(int,10), TRUE );
KEYWORD_ID(cr_vinesAL_ip, cr_vinesAL_ip_set, cr_vinesAL_ipctest,
	   OBJ(int,3), VINES_IP,
	   "ip", "Filter IP protocol packets (Standard access list)", PRIV_CONF);

/* Short access lists.  Only contains one address/mask per line */
VINADDR	(cr_vinesAL_shortmsk, cr_vinesAL_eol, no_alt,
	 OBJ(paddr,2), "Mask");
VINADDR	(cr_vinesAL_shortaddr, cr_vinesAL_shortmsk, no_alt,
	 OBJ(paddr,1), "Address");
TESTVAR	(cr_vinesAL_short2, cr_vinesAL_shortaddr, NONE, cr_vinesAL_shortaddr,
	 cr_vinesAL_ip, NONE, OBJ(int,1), VINESMAXSACC);
TESTVAR	(cr_vinesAL_short, cr_vinesAL_shortaddr, NONE, cr_vinesAL_ip,
	 cr_vinesAL_short2, NONE, OBJ(int,1), VINESMINSACC);

/* Permit & Deny keywords */
PERMIT_DENY(cr_vinesAL_permit_deny, cr_vinesAL_short, no_alt,
	    OBJ(int,2), PRIV_CONF);

/* Skip all other arguments if this is the 'no' version of the command */
NOPREFIX(cr_vinesAL_noprefix, cr_vinesAL_permit_deny, cr_vinesAL_eol);

/* List number */
NUMBER	(cr_vinesAL_list, cr_vinesAL_noprefix, no_alt, OBJ(int,1),
	 VINESMINACC, VINESMAXACC, "VINES access list");
NVGENS	(cr_vinesAL_nvgen, cr_vinesAL_list, vines_command,
	 VINES_ACCESSLIST);
KEYWORD (cr_vinesAL, cr_vinesAL_nvgen, no_alt,
	 "access-list", "VINES access list", PRIV_CONF);

/***************************************************************************
 * [no] vines time access-group <list> [ in | out ]
 * [no] vines time destination <address>
 * [no] vines time participate
 * [no] vines time set-system
 * [no] vines time use-system
 *
 */
EOLS	       (cr_v_t_use_eol, vss_command, VINES_TIME_USE);
KEYWORD        (cr_v_t_use, cr_v_t_use_eol, no_alt,
	        "use-system", "Synchronize VINES time with system time", PRIV_CONF);

EOLS	       (cr_v_t_set_eol, vss_command, VINES_TIME_SET);
KEYWORD        (cr_v_t_set, cr_v_t_set_eol, cr_v_t_use,
	        "set-system", "Synchronize system time with VINES time", PRIV_CONF);

EOLS	       (cr_v_t_part_eol, vss_command, VINES_TIME_PARTICIPATE);
KEYWORD        (cr_v_t_part, cr_v_t_part_eol, cr_v_t_set,
	        "participate", "Participate in time of day protocol", PRIV_CONF);

EOLS	       (cr_v_t_dest_eol, vss_command, VINES_TIME_DEST);
VINADDR        (cr_v_t_dest_addr, cr_v_t_dest_eol, no_alt,
	        OBJ(paddr,1), "Destination station address");
NVGENS	       (cr_v_t_dest_nvgens, cr_v_t_dest_addr, vss_command, VINES_TIME_DEST);
KEYWORD        (cr_v_t_dest, cr_v_t_dest_nvgens, cr_v_t_part,
	        "destination", "Send time messages to specific stations", PRIV_CONF);

PARAMS_KEYONLY (cr_v_t_list, cr_v_t_dest, "access-group",
		OBJ(int,1), VINESMINSACC, VINESMAXSACC,
		vss_command, VINES_TIME_LIST,
		"Filter received time messages",
		"A valid VINES access list number", PRIV_CONF);

KEYWORD	       (cr_vines_time, cr_v_t_list, cr_vinesAL,
		"time", "VINES time of day support", PRIV_CONF);


/***************************************************************************
 * vines single-route
 * no vines single-route
 *
 */
EOLS	(cr_vines_single_eol, vines_command, VINES_SINGLE);
KEYWORD	(cr_vines_single, cr_vines_single_eol, cr_vines_time,
	 "single-route", "Maintain only one route per server", PRIV_CONF);

/***************************************************************************
 * vines route <net> <address> <metric>
 * no vines route <net> <address>
 *
 */
EOLS	(cr_vines_route_eol, vines_command, VINES_ROUTE);
NUMBER	(cr_v_r_fractional, cr_vines_route_eol, cr_vines_route_eol,
	 OBJ(int,3), 0, 9999, "metric - fractional part (in 10,000ths)");
NUMBER	(cr_v_r_whole, cr_v_r_fractional, cr_vines_route_eol,
	 OBJ(int,2), 0, 4095, "metric - whole number part");
NOPREFIX(cr_vines_route_noprefix, cr_v_r_whole, cr_vines_route_eol);
VINADDR (cr_vines_route_addr, cr_vines_route_noprefix, no_alt,
	 OBJ(paddr,2), "Next hop for the specified address");
VINESNET(cr_vines_route_net, cr_vines_route_addr, no_alt, OBJ(paddr,1),
	 "Destination server number");
NVGENS	(cr_vines_route_nvgens, cr_vines_route_net, vines_command, VINES_ROUTE);
KEYWORD	(cr_vines_route, cr_vines_route_nvgens, cr_vines_single,
	 "route", "Set VINES static route", PRIV_CONF);

/***************************************************************************
 * vines host <name> <address>
 * no vines host <name>
 * no vines host
 *
 * OBJ(string,1) == hostname		(If OBJ(string,1) is empty,
 *					 then 'no vines host'.)
 * (*OBJ(paddr,1)) == vines address
 */
EOLS	(cr_vines_hosteol, vines_command, VINES_HOST);
TESTVAR	(cr_vines_nohostaddr, cr_vines_hosteol, no_alt,
	 NONE, NONE, NONE, sense, FALSE);
SET	(cr_vines_hostaddr_set, cr_vines_hosteol, OBJ(int,10), TRUE );
VINADDR	(cr_vines_hostaddr, cr_vines_hostaddr_set, cr_vines_nohostaddr,
	 OBJ(paddr,1), "VINES address");
TESTVAR	(cr_vines_nohostname, cr_vines_hosteol, no_alt,
	 NONE, NONE, NONE, sense, FALSE);
SET	(cr_vines_hostname_set, cr_vines_hostaddr, OBJ(int,9), TRUE );
STRING	(cr_vines_hostname, cr_vines_hostname_set, cr_vines_nohostname,
	 OBJ(string,1), "VINES hostname");
NVGENS	(cr_vines_hostnvgen, cr_vines_hostname, vines_command, VINES_HOST);
KEYWORD	(cr_vines_host, cr_vines_hostnvgen, cr_vines_route,
	 "host", "Add an entry to the VINES name-to-address table", PRIV_CONF);

/***************************************************************************
 * vines enhancements
 * no vines enhancements
 *
 */
EOLS	(cr_vines_enhance_eol, vines_command, VINES_ENHANCEMENTS);
KEYWORD (cr_vines_enhance, cr_vines_enhance_eol, cr_vines_host,
	 "enhancements", "Use enhancements to VINES RTP protocol", PRIV_CONF);

KEYWORD (cr_vines, cr_vines_enhance, ALTERNATE,
	 "vines", " ", PRIV_CONF | PRIV_DUPLICATE);

#undef	ALTERNATE
#define	ALTERNATE	cr_vines
