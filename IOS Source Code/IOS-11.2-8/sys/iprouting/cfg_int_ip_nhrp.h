/* $Id: cfg_int_ip_nhrp.h,v 3.3.48.2 1996/03/28 03:31:16 bcole Exp $
 * $Source: /release/111/cvs/Xsys/iprouting/cfg_int_ip_nhrp.h,v $
 *------------------------------------------------------------------
 * Next Hop Resolution Protocol.
 *
 * October 1995, Bruce Cole
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_ip_nhrp.h,v $
 * Revision 3.3.48.2  1996/03/28  03:31:16  bcole
 * CSCdi51494:  NHRP: Prefix entries not appearing in cache in many cases
 * Branch: California_branch
 *
 * Bring 11.1 fixes forward...
 *
 * Revision 3.3.48.1  1996/03/18  20:27:48  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.16.2  1996/03/13  01:39:10  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.16.1  1996/02/20  14:28:05  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/12/07  17:45:12  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.2  1995/11/17  17:31:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:04:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:06:11  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  21:54:54  bcole
 * Placeholder files for 11.1 NHRP work.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * ip nhrp use [address [mask]] <usage-count>
 *
 */

PARAMS_KEYONLY (ci_ip_nhrp_use, no_alt, "use",
		OBJ(int,1), 1, 0xffff, nhrp_ip_if_command,
		NHRP_CMD_USE,
		"specify usage count for sending requests",
		"number of packets to cause NHRP request",
		PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * ip nhrp max-send <pkt-count> every <interval>
 *
 */

EOLS	(ci_ip_nhrp_max_eol, nhrp_ip_if_command, NHRP_CMD_MAX);
NUMBER (ci_ip_nhrp_max_evnum, ci_ip_nhrp_max_eol, no_alt,
	OBJ(int,2), 10, 0xffff, "interval in seconds");
KEYWORD (ci_ip_nhrp_max_every, ci_ip_nhrp_max_evnum, no_alt,
	 "every", "specify interval over which to throttle",
	 PRIV_CONF|PRIV_SUBIF);
NUMBER (ci_ip_nhrp_max_num, ci_ip_nhrp_max_every, no_alt,
	OBJ(int,1), 1, 0xffff, "packet count");
NOPREFIX (ci_ip_nhrp_max_no, ci_ip_nhrp_max_num, ci_ip_nhrp_max_eol);
NVGENS (ci_ip_nhrp_max_nv, ci_ip_nhrp_max_no, nhrp_ip_if_command,
	NHRP_CMD_MAX);
KEYWORD (ci_ip_nhrp_max, ci_ip_nhrp_max_nv, ci_ip_nhrp_use,
	 "max-send", "rate limit NHRP traffic", PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * ip nhrp record
 *
 */

EOLS	(ci_ip_nhrp_rec_eol, nhrp_ip_if_command, NHRP_CMD_RECORD);
KEYWORD (ci_ip_nhrp_rec, ci_ip_nhrp_rec_eol, ci_ip_nhrp_max,
	 "record", "Allow NHRP record option", PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * ip nhrp nhs <nhs-addr> [<net> <netmask>]
 *
 */

EOLS	(ci_ip_nhrp_nhs_eol, nhrp_ip_if_command, NHRP_CMD_NHS);
IPADDR	(ci_ip_nhrp_nhs_mask, ci_ip_nhrp_nhs_eol, no_alt,
	 OBJ(paddr,3), "Network mask for IP network served by this NHS");
IPADDR	(ci_ip_nhrp_nhs_net, ci_ip_nhrp_nhs_mask, ci_ip_nhrp_nhs_eol,
	 OBJ(paddr,2), "IP network served by this NHS");
IPADDR	(ci_ip_nhrp_nhsa, ci_ip_nhrp_nhs_net, no_alt,
	 OBJ(paddr,1), "IP address of NHS");
NVGENS (ci_ip_nhrp_nhs_nvgen, ci_ip_nhrp_nhsa, nhrp_ip_if_command,
	NHRP_CMD_NHS);
KEYWORD (ci_ip_nhrp_nhs, ci_ip_nhrp_nhs_nvgen, ci_ip_nhrp_rec,
	 "nhs", "Specify a next hop server", PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * ip nhrp interest <acl>
 *
 */

PARAMS_KEYONLY (ci_ip_nhrp_intr, ci_ip_nhrp_nhs, "interest",
		OBJ(int,1), MINFASTACC, MAXSLOWACC, nhrp_ip_if_command,
		NHRP_CMD_INTEREST,
		"specify an access list", "IP access list",
		PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * ip nhrp holdtime <secs-positive> [<secs-negative>]
 *
 */

EOLS	(ci_ip_nhrp_hold_eol, nhrp_ip_if_command, NHRP_CMD_HOLD);
NUMBER (ci_ip_nhrp_hold_neg, ci_ip_nhrp_hold_eol, ci_ip_nhrp_hold_eol,
	OBJ(int,2), 1, 0xffff, "number of seconds for negative responses");
NUMBER (ci_ip_nhrp_hold_pos, ci_ip_nhrp_hold_neg, no_alt,
	OBJ(int,1), 1, 0xffff, "number of seconds for positive responses");
NVGENS (ci_ip_nhrp_hold_nvgen, ci_ip_nhrp_hold_pos, nhrp_ip_if_command,
	NHRP_CMD_HOLD);
KEYWORD (ci_ip_nhrp_hold, ci_ip_nhrp_hold_nvgen, ci_ip_nhrp_intr,
	 "holdtime", "Advertised holdtime", PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * ip nhrp network-id <num>
 *
 */

PARAMS_KEYONLY (ci_ip_nhrp_net, ci_ip_nhrp_hold, "network-id",
		OBJ(int,1), 1, -1, nhrp_ip_if_command, NHRP_CMD_NET,
		"NBMA network identifier", "network-id", PRIV_CONF|PRIV_SUBIF);



/******************************************************************
 * ip nhrp map {multicast | <ipaddr> [<mask>]} <nbma addr>
 *
 */
EOLS	(ci_ip_nhrp_map_eol, nhrp_ip_if_command, NHRP_CMD_MAP);
SMDSADDR(ci_ip_nhrp_map_smds, ci_ip_nhrp_map_eol, no_alt,
	 OBJ(hwaddr,1), "SMDS E.164 address");
ENADDR	(ci_ip_nhrp_map_ieee, ci_ip_nhrp_map_eol, no_alt,
	 OBJ(hwaddr,1), "48-bit IEEE address");
IPADDR	(ci_ip_nhrp_map_ip, ci_ip_nhrp_map_eol, no_alt,
	 OBJ(paddr,2), "IP NBMA address");
ATMNSAPADDR(ci_ip_nhrp_map_nsap, ci_ip_nhrp_map_eol, no_alt,
	 OBJ(hwaddr,1), "20-byte NSAP address");

IFELSE  (ci_ip_nhrp_map_iftype3, ci_ip_nhrp_map_nsap, no_alt,
	 (csb->interface->hwptr->enctype == ET_ATM));
IFELSE  (ci_ip_nhrp_map_iftype2, ci_ip_nhrp_map_smds, ci_ip_nhrp_map_iftype3,
	 (csb->interface->hwptr->enctype == ET_SMDS));
TEST_IDBSTATUS(ci_ip_nhrp_map_iftype1, ci_ip_nhrp_map_ieee,
	       ci_ip_nhrp_map_iftype2, NONE, IDB_IEEEMASK);
IFELSE  (ci_ip_nhrp_map_iftype, ci_ip_nhrp_map_ip, ci_ip_nhrp_map_iftype1,
	 is_tunnel(csb->interface->hwptr));
SET	(ci_ip_nhrp_map_nomask, ci_ip_nhrp_map_iftype, OBJ(int,10), TRUE);
IPADDR	(ci_ip_nhrp_map_mask, ci_ip_nhrp_map_iftype, ci_ip_nhrp_map_nomask,
	 OBJ(paddr,3), "IP mask of destination");
IPADDR	(ci_ip_nhrp_map_addr, ci_ip_nhrp_map_mask, no_alt, 
	 OBJ(paddr,1), "IP address of destination");
KEYWORD_ID (ci_ip_nhrp_map_mcast, ci_ip_nhrp_map_iftype, ci_ip_nhrp_map_addr,
	    OBJ(int,1), 1, "multicast",
	    "use this NBMA mapping for broadcasts/multicasts",
	    PRIV_CONF|PRIV_SUBIF);
NVGENS (ci_ip_nhrp_map_nvgen, ci_ip_nhrp_map_mcast, nhrp_ip_if_command,
	NHRP_CMD_MAP);
KEYWORD (ci_ip_nhrp_map, ci_ip_nhrp_map_nvgen, ci_ip_nhrp_net,
	 "map", "map dest IP addresses to NBMA addresses",
	 PRIV_CONF|PRIV_SUBIF);


/******************************************************************
 * ip nhrp responder <interface>
 *
 */
EOLS	(ci_ip_nhrp_resp_eol, nhrp_ip_if_command, NHRP_CMD_RESP);
INTERFACE (ci_ip_nhrp_resp_if, ci_ip_nhrp_resp_eol, no_alt,
	   OBJ(idb,1), IFTYPE_ANYSUB);
NOPREFIX (ci_ip_nhrp_resp_noprefix, ci_ip_nhrp_resp_if,
	  ci_ip_nhrp_resp_eol);
NVGENS (ci_ip_nhrp_resp_nvgen, ci_ip_nhrp_resp_noprefix, nhrp_ip_if_command,
	NHRP_CMD_RESP);
KEYWORD	(ci_ip_nhrp_resp, ci_ip_nhrp_resp_nvgen, ci_ip_nhrp_map,
	 "responder", "responder interface", PRIV_CONF|PRIV_SUBIF);

/******************************************************************
 * ip nhrp authentication <string>
 *
 */
EOLS	(ci_ip_nhrp_auth_eol, nhrp_ip_if_command, NHRP_CMD_AUTH);
STRING	(ci_ip_nhrp_auth_string, ci_ip_nhrp_auth_eol, no_alt,
	 OBJ(string,1), "authentication string" );
NOPREFIX (ci_ip_nhrp_auth_noprefix, ci_ip_nhrp_auth_string,
	  ci_ip_nhrp_auth_eol);
NVGENS (ci_ip_nhrp_auth_nvgen, ci_ip_nhrp_auth_noprefix, nhrp_ip_if_command,
	NHRP_CMD_AUTH);
KEYWORD	(ci_ip_nhrp_auth, ci_ip_nhrp_auth_nvgen, ci_ip_nhrp_resp,
	 "authentication", "Authentication string", PRIV_CONF|PRIV_SUBIF);

KEYWORD (ci_ip_nhrp, ci_ip_nhrp_auth, NONE,
	 "nhrp", "NHRP interface subcommands", PRIV_CONF|PRIV_SUBIF);
TEST_IDBSTATUS(ci_ip_nhrp_test, ci_ip_nhrp, NONE, ALTERNATE,
	       (IDB_SERIAL | IDB_TUNNEL | IDB_IEEEMASK | IDB_ATM));

#undef	ALTERNATE
#define	ALTERNATE	ci_ip_nhrp_test
