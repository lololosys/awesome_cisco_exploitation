/* $Id: cfg_int_ip_irdp.h,v 3.2 1995/11/17 17:31:25 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/cfg_int_ip_irdp.h,v $
 *------------------------------------------------------------------
 * IRDP interface commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_int_ip_irdp.h,v $
 * Revision 3.2  1995/11/17  17:31:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:04:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:34  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:05:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 *	[no] ip irdp
 *      [no] ip irdp address <ip-addr> [<preference> - default = 0]
 *	[no] ip irdp preference <preference>
 *	[no] ip irdp holdtime <holdtime>
 *	[no] ip irdp minadvertinterval <minadvert>
 *	[no] ip irdp maxadvertinterval <minadvert>
 *	[no] ip irdp multicast
 *
 */
EOLS	(ci_ip_irdp_addr_eol, ip_irdp_command, IRDP_CMD_ADDRESS );
EQUAL	(ci_ip_irdp_noaddr, ci_ip_irdp_addr_eol, no_alt, sense, FALSE);
SIGNED_DEC (ci_ip_irdp_addr_pref, ci_ip_irdp_addr_eol, ci_ip_irdp_noaddr,
	 OBJ(int,1), MININT, MAXINT, "preference for this address" );
IPADDR	(ci_ip_irdp_addr_addr, ci_ip_irdp_addr_pref, no_alt,
	 OBJ(paddr,1), "address to advertise" );
NVGENS	(ci_ip_irdp_addr_nvgen, ci_ip_irdp_addr_addr,
	 ip_irdp_command, IRDP_CMD_ADDRESS);
KEYWORD(ci_ip_irdp_addr, ci_ip_irdp_addr_nvgen, no_alt,
	"address", "addresses to proxy-advertise", PRIV_CONF|PRIV_SUBIF);

EOLS	(ci_ip_irdp_pref_eol, ip_irdp_command, IRDP_CMD_PREFERENCE );
EQUAL	(ci_ip_irdp_nopref, ci_ip_irdp_pref_eol, no_alt, sense, FALSE);
SIGNED_DEC(ci_ip_irdp_pref_val, ci_ip_irdp_pref_eol, ci_ip_irdp_nopref,
	   OBJ(int,1), MININT, MAXINT,
	   "preference for this address (higher values preferred)");
NVGENS	(ci_ip_irdp_pref_nvgen, ci_ip_irdp_pref_val,
	 ip_irdp_command, IRDP_CMD_PREFERENCE);
KEYWORD	(ci_ip_irdp_pref, ci_ip_irdp_pref_nvgen, ci_ip_irdp_addr,
	"preference", "preference level for this interface",
	 PRIV_CONF|PRIV_SUBIF);

PARAMS_KEYONLY(ci_ip_irdp_holdtime, ci_ip_irdp_pref,
	       "holdtime", OBJ(int,1), 0, IRDP_MAX_HOLDTIME,
	       ip_irdp_command, IRDP_CMD_HOLDTIME,
	       "how long a receiver should believe the information", 
	       "holdtime (default 1800 seconds)", PRIV_CONF|PRIV_SUBIF);

PARAMS_KEYONLY(ci_ip_irdp_minadvint, ci_ip_irdp_holdtime,
	       "minadvertinterval", OBJ(int,1), 0, -1,
	       ip_irdp_command, IRDP_CMD_MINADVERT,
	       "minimum time between advertisements",
	       "minimum time between advertisements (default 450 seconds)",
	       PRIV_CONF|PRIV_SUBIF);

PARAMS_KEYONLY(ci_ip_irdp_maxadvint, ci_ip_irdp_minadvint, 
	       "maxadvertinterval", OBJ(int,1), 0, -1,
	       ip_irdp_command, IRDP_CMD_MAXADVERT,
	       "maximum time between advertisements",
	       "maximum time between advertisements (default 600 seconds)",
	       PRIV_CONF|PRIV_SUBIF);

EOLS	(ci_ip_irdp_multicast_eol, ip_irdp_command, IRDP_CMD_MULTICAST);
KEYWORD	(ci_ip_irdp_multicast, ci_ip_irdp_multicast_eol, ci_ip_irdp_maxadvint,
	 "multicast", "advertisements are sent with multicasts",
	 PRIV_CONF|PRIV_SUBIF);

EOLS	(ci_ip_irdp_eol, ip_irdp_command, IRDP_CMD);
NOP	(ci_ip_irdp_nop, ci_ip_irdp_eol, ci_ip_irdp_multicast);

KEYWORD	(ci_ip_irdp, ci_ip_irdp_nop, ALTERNATE,
	 "irdp", "ICMP Router Discovery Protocol", PRIV_CONF|PRIV_SUBIF);

#undef	ALTERNATE
#define	ALTERNATE	ci_ip_irdp
