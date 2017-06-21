/* $Id: cfg_int_ipx_nhrp.h,v 3.3.50.2 1996/05/30 23:47:43 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/cfg_int_ipx_nhrp.h,v $
 *------------------------------------------------------------------
 * Next Hop Resolution Protocol.
 *
 * October 1995, Bruce Cole
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_ipx_nhrp.h,v $
 * Revision 3.3.50.2  1996/05/30  23:47:43  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.3.50.1  1996/03/18  22:52:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.16.2  1996/03/13  02:14:13  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.16.1  1996/02/20  21:58:37  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/12/07  17:45:44  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.2  1995/11/17  19:20:58  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:38:01  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  21:55:08  bcole
 * Placeholder files for 11.1 NHRP work.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * ipx nhrp use [address [mask]] <usage-count>
 *
 */

PARAMS_KEYONLY (ci_ipx_nhrp_use, no_alt, "use",
		OBJ(int,1), 1, 0xffff, nhrp_ipx_if_command,
		NHRP_CMD_USE,
		"specify usage count for sending requests",
		"number of packets to cause NHRP request",
		PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * ipx nhrp max-send <pkt-count> every <interval>
 *
 */

EOLS	(ci_ipx_nhrp_max_eol, nhrp_ipx_if_command, NHRP_CMD_MAX);
NUMBER (ci_ipx_nhrp_max_evnum, ci_ipx_nhrp_max_eol, no_alt,
	OBJ(int,2), 10, 0xffff, "interval in seconds");
KEYWORD (ci_ipx_nhrp_max_every, ci_ipx_nhrp_max_evnum, no_alt,
	 "every", "specify interval over which to throttle",
	 PRIV_CONF|PRIV_SUBIF);
NUMBER (ci_ipx_nhrp_max_num, ci_ipx_nhrp_max_every, no_alt,
	OBJ(int,1), 1, 0xffff, "packet count");
NOPREFIX (ci_ipx_nhrp_max_no, ci_ipx_nhrp_max_num, ci_ipx_nhrp_max_eol);
NVGENS (ci_ipx_nhrp_max_nv, ci_ipx_nhrp_max_no, nhrp_ipx_if_command,
	NHRP_CMD_MAX);
KEYWORD (ci_ipx_nhrp_max, ci_ipx_nhrp_max_nv, ci_ipx_nhrp_use,
	 "max-send", "rate limit NHRP traffic", PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * ipx nhrp record
 *
 */

EOLS	(ci_ipx_nhrp_rec_eol, nhrp_ipx_if_command, NHRP_CMD_RECORD);
KEYWORD (ci_ipx_nhrp_rec, ci_ipx_nhrp_rec_eol, ci_ipx_nhrp_max,
	 "record", "Allow NHRP record option", PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * ipx nhrp nhs <nhs-addr> [<net>]
 *
 */

EOLS	(ci_ipx_nhrp_nhs_eol, nhrp_ipx_if_command, NHRP_CMD_NHS);
NOVADDR	(ci_ipx_nhrp_nhs_net, ci_ipx_nhrp_nhs_eol, no_alt,
	 OBJ(paddr,2), "IXP network served by this NHS");
NOVADDR	(ci_ipx_nhrp_nhsa, ci_ipx_nhrp_nhs_net, no_alt,
	 OBJ(paddr,1), "IXP address of NHS");
NVGENS (ci_ipx_nhrp_nhs_nvgen, ci_ipx_nhrp_nhsa, nhrp_ipx_if_command,
	NHRP_CMD_NHS);
KEYWORD (ci_ipx_nhrp_nhs, ci_ipx_nhrp_nhs_nvgen, ci_ipx_nhrp_rec,
	 "nhs", "Specify a next hop server", PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * ipx nhrp interest <acl>
 *
 */

PARAMS_ALIST   (ci_ipx_nhrp_intr, ci_ipx_nhrp_nhs, "interest",
		OBJ(int,1), MINNOVACC, MAXENOVACC, 
		IPX_MIN_STD_ACL, IPX_MAX_EXT_ACL, nhrp_ipx_if_command,
		NHRP_CMD_INTEREST,
		"specify an access list", "IPX access list",
		PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * ipx nhrp holdtime <secs-positive> [<secs-negative>]
 *
 */

EOLS	(ci_ipx_nhrp_hold_eol, nhrp_ipx_if_command, NHRP_CMD_HOLD);
NUMBER (ci_ipx_nhrp_hold_neg, ci_ipx_nhrp_hold_eol, ci_ipx_nhrp_hold_eol,
	OBJ(int,2), 1, 0xffff, "number of seconds for negative responses");
NUMBER (ci_ipx_nhrp_hold_pos, ci_ipx_nhrp_hold_neg, no_alt,
	OBJ(int,1), 1, 0xffff, "number of seconds for positive responses");
NVGENS (ci_ipx_nhrp_hold_nvgen, ci_ipx_nhrp_hold_pos, nhrp_ipx_if_command,
	NHRP_CMD_HOLD);
KEYWORD (ci_ipx_nhrp_hold, ci_ipx_nhrp_hold_nvgen, ci_ipx_nhrp_intr,
	 "holdtime", "Advertised holdtime", PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * ipx nhrp network-id <num>
 *
 */

PARAMS_KEYONLY (ci_ipx_nhrp_net, ci_ipx_nhrp_hold, "network-id",
		OBJ(int,1), 1, -1, nhrp_ipx_if_command, NHRP_CMD_NET,
		"NBMA network identifier", "network-id", PRIV_CONF|PRIV_SUBIF);



/******************************************************************
 * ipx nhrp map [multicast | <NOVADDR>] <nbma addr>
 *
 */
EOLS	(ci_ipx_nhrp_map_eol, nhrp_ipx_if_command, NHRP_CMD_MAP);
SMDSADDR(ci_ipx_nhrp_map_smds, ci_ipx_nhrp_map_eol, no_alt,
	 OBJ(hwaddr,1), "SMDS E.164 address");
ENADDR	(ci_ipx_nhrp_map_ieee, ci_ipx_nhrp_map_eol, no_alt,
	 OBJ(hwaddr,1), "48-bit IEEE address");
IPADDR	(ci_ipx_nhrp_map_ip, ci_ipx_nhrp_map_eol, no_alt,
	 OBJ(paddr,2), "IP NBMA address");
ATMNSAPADDR(ci_ipx_nhrp_map_nsap, ci_ipx_nhrp_map_eol, no_alt,
	 OBJ(hwaddr,1), "20-byte NSAP address");

IFELSE  (ci_ipx_nhrp_map_iftype3, ci_ipx_nhrp_map_nsap, no_alt,
	 (csb->interface->hwptr->enctype == ET_ATM));
IFELSE  (ci_ipx_nhrp_map_iftype2, ci_ipx_nhrp_map_smds, ci_ipx_nhrp_map_iftype3,
	 (csb->interface->hwptr->enctype == ET_SMDS));
TEST_IDBSTATUS(ci_ipx_nhrp_map_iftype1, ci_ipx_nhrp_map_ieee,
	       ci_ipx_nhrp_map_iftype2, NONE, IDB_IEEEMASK);
IFELSE  (ci_ipx_nhrp_map_iftype, ci_ipx_nhrp_map_ip, ci_ipx_nhrp_map_iftype1,
	 is_tunnel(csb->interface->hwptr));
NOVADDR	(ci_ipx_nhrp_map_addr, ci_ipx_nhrp_map_iftype, no_alt, 
	 OBJ(paddr,1), "IPX address of destination");
KEYWORD_ID (ci_ipx_nhrp_map_mcast, ci_ipx_nhrp_map_iftype, ci_ipx_nhrp_map_addr,
	    OBJ(int,1), 1, "multicast",
	    "use this NBMA mapping for broadcasts/multicasts",
	    PRIV_CONF|PRIV_SUBIF);
NVGENS (ci_ipx_nhrp_map_nvgen, ci_ipx_nhrp_map_mcast, nhrp_ipx_if_command,
	NHRP_CMD_MAP);
KEYWORD (ci_ipx_nhrp_map, ci_ipx_nhrp_map_nvgen, ci_ipx_nhrp_net,
	 "map", "map dest IPX addresses to NBMA addresses",
	 PRIV_CONF|PRIV_SUBIF);


/******************************************************************
 * ipx nhrp responder <interface>
 *
 */
EOLS	(ci_ipx_nhrp_resp_eol, nhrp_ipx_if_command, NHRP_CMD_RESP);
INTERFACE (ci_ipx_nhrp_resp_if, ci_ipx_nhrp_resp_eol, no_alt,
	   OBJ(idb,1), IFTYPE_ANYSUB);
NOPREFIX (ci_ipx_nhrp_resp_noprefix, ci_ipx_nhrp_resp_if,
	  ci_ipx_nhrp_resp_eol);
NVGENS (ci_ipx_nhrp_resp_nvgen, ci_ipx_nhrp_resp_noprefix, nhrp_ipx_if_command,
	NHRP_CMD_RESP);
KEYWORD	(ci_ipx_nhrp_resp, ci_ipx_nhrp_resp_nvgen, ci_ipx_nhrp_map,
	 "responder", "responder interface", PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * ipx nhrp authentication <string>
 *
 */
EOLS	(ci_ipx_nhrp_auth_eol, nhrp_ipx_if_command, NHRP_CMD_AUTH);
STRING	(ci_ipx_nhrp_auth_string, ci_ipx_nhrp_auth_eol, no_alt,
	 OBJ(string,1), "authentication string" );
NOPREFIX (ci_ipx_nhrp_auth_noprefix, ci_ipx_nhrp_auth_string,
	  ci_ipx_nhrp_auth_eol);
NVGENS (ci_ipx_nhrp_auth_nvgen, ci_ipx_nhrp_auth_noprefix, nhrp_ipx_if_command,
	NHRP_CMD_AUTH);
KEYWORD	(ci_ipx_nhrp_auth, ci_ipx_nhrp_auth_nvgen, ci_ipx_nhrp_resp,
	 "authentication", "Authentication string", PRIV_CONF|PRIV_SUBIF);

KEYWORD (ci_ipx_nhrp, ci_ipx_nhrp_auth, NONE,
	 "nhrp", "NHRP interface subcommands", PRIV_CONF|PRIV_SUBIF);
TEST_IDBSTATUS(ci_ipx_nhrp_test, ci_ipx_nhrp, NONE, ALTERNATE,
	       (IDB_SERIAL | IDB_TUNNEL | IDB_ATM));

#undef	ALTERNATE
#define	ALTERNATE	ci_ipx_nhrp_test
