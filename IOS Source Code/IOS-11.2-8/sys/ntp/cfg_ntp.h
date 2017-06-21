/* $Id: cfg_ntp.h,v 3.2.62.2 1996/08/19 18:54:35 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/cfg_ntp.h,v $
 *------------------------------------------------------------------
 * NTP command parser chains
 *
 * Dave Katz, November 1992
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_ntp.h,v $
 * Revision 3.2.62.2  1996/08/19  18:54:35  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.2.62.1  1996/08/12  16:04:49  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:50:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:40:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:44:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* no ntp
*/

EOLNS    (no_ntp_eol, no_ntp_command);
SPLIT   (no_ntp_test, no_ntp_eol, no_alt, sense, FALSE);

/******************************************************************************
* ntp { peer | server } ipaddr [version #] [key #] [source interface] [prefer]
* no ntp { peer | server } ipaddr
*
* OBJ(int,1) = mode (ACTIVE/CLIENT)
* OBJ(int,2) = version (1-3)
* OBJ(int,3) = TRUE if "key" present
* OBJ(int,4) = key number
* OBJ(int,5) = TRUE if "prefer" present
* OBJ(idb,1) = source interface or NULL
* OBJ(paddr,1) = peer address
*/

pdecl   (ntp_peer_options);
EOLS    (ntp_peer_eol, ntp_global_config_cmd, NTP_PEER_CMD);
KEYWORD_ID(ntp_peer_prefer, ntp_peer_options, ntp_peer_eol,
	   OBJ(int,5), TRUE,
	   "prefer", "Prefer this peer when possible", PRIV_CONF);
INTERFACE(ntp_peer_source_if, ntp_peer_options, no_alt,
	  OBJ(idb,1),  IFTYPE_ANYSUB);
KEYWORD (ntp_peer_source, ntp_peer_source_if, ntp_peer_prefer,
	 "source", "Interface for source address", PRIV_CONF);
NUMBER  (ntp_peer_key_num, ntp_peer_options, no_alt,
	 OBJ(int,4), 0, MAXUINT, "Peer key number");
KEYWORD_ID(ntp_peer_key, ntp_peer_key_num, ntp_peer_source,
	   OBJ(int,3), TRUE,
	   "key", "Configure peer authentication key", PRIV_CONF);
NUMBER  (ntp_peer_version_value, ntp_peer_options, no_alt,
	 OBJ(int,2), 1, NTP_VERSION, "NTP version number");
KEYWORD (ntp_peer_options, ntp_peer_version_value, ntp_peer_key,
	 "version", "Configure NTP version", PRIV_CONF);
SET     (ntp_peer_version, ntp_peer_options, OBJ(int,2), NTP_VERSION);
NOPREFIX(ntp_peer_noprefix, ntp_peer_version, ntp_peer_eol);
IPADDR_NAME  (ntp_peer_addr, ntp_peer_noprefix, no_alt,
	      OBJ(paddr,1), "IP address of peer");
KEYWORD_ID (ntp_server, ntp_peer_addr, no_ntp_test,
	    OBJ(int,1), MODE_CLIENT,
	    "server", "Configure NTP server", PRIV_CONF);
KEYWORD_ID (ntp_peer, ntp_peer_addr, ntp_server,
	    OBJ(int,1), MODE_ACTIVE,
	    "peer", "Configure NTP peer", PRIV_CONF);
NVGENS  (ntp_peer_nvgen, ntp_peer, ntp_global_config_cmd, NTP_PEER_CMD);

/******************************************************************************
* ntp max-associations <nn>
* no ntp max-associations
*
* OBJ(int,1) = association count
*/
EOLS	(ntp_maxass_eol, ntp_global_config_cmd, NTP_MAXASS_CMD);
NUMBER	(ntp_maxass_count, ntp_maxass_eol, ntp_maxass_eol, OBJ(int,1), 0,
	 MAXUINT, "Number of associations");
SET	(ntp_maxass_set, ntp_maxass_count, OBJ(int,1), NTP_DEF_MAX_PEERCOUNT);
NOPREFIX(ntp_maxass_no, ntp_maxass_set, ntp_maxass_eol);
NVGENS	(ntp_maxass_nv, ntp_maxass_no, ntp_global_config_cmd, NTP_MAXASS_CMD);
KEYWORD (ntp_maxass, ntp_maxass_nv, ntp_peer_nvgen, "max-associations",
	 "Set maximum number of associations", PRIV_CONF);

/******************************************************************************
* [no] ntp update-calendar
*/
EOLS    (ntp_update_calendar_eol, ntp_global_config_cmd, 
	 NTP_UPDATE_CALENDAR_CMD);
KEYWORD (ntp_update_calendar, ntp_update_calendar_eol, NONE,
	 "update-calendar", "Periodically update calendar with NTP time",
	 PRIV_CONF);

/*
 * Only allow the above command if the cpu supports calendar hardware.
 */
ASSERT	 (ntp_cal_test, ntp_update_calendar, ntp_maxass,
	  reg_invoke_cpu_supports_calendar());

/******************************************************************************
* ntp master [#]
* no ntp master
*
* OBJ(int,1) = stratum (default is 8)
*/
EOLS    (ntp_master_eol, ntp_global_config_cmd, NTP_MASTER_CMD);
NUMBER  (ntp_master_stratum, ntp_master_eol, ntp_master_eol,
	 OBJ(int,1), 1, 15, "Stratum number");
SET     (ntp_master_set, ntp_master_stratum,
	 OBJ(int,1), STRATUM_MASTER_DEFAULT);
NOPREFIX(ntp_master_no, ntp_master_set, ntp_master_eol);
NVGENS  (ntp_master_nv, ntp_master_no, ntp_global_config_cmd, NTP_MASTER_CMD);
KEYWORD (ntp_master, ntp_master_nv, ntp_cal_test,
	 "master", "Act as NTP master clock", PRIV_CONF);

/* Only allow this if the master mode driver is present */

IFELSE  (ntp_master_test, ntp_master, ntp_cal_test,
	 reg_invoke_ntp_refclock_present(REFCLK_MASTER));

/******************************************************************************
* ntp access-group {query-only|serve-only|serve|peer} <num>
* no ntp access-group {query-only|serve-only|serve|peer}
*
* OBJ(int,1) = server type
* OBJ(int,2) = access list number
*/
EOLNS    (ntp_access_eol, ntp_access_command);
NUMBER  (ntp_access_list, ntp_access_eol, no_alt,
	 OBJ(int, 2), 1, MAXFASTACC, "Standard IP access list");
NOPREFIX(ntp_access_no, ntp_access_list, ntp_access_eol);
NVGENNS  (ntp_access_nvgen, ntp_access_no, ntp_access_command);
KEYWORD_ID (ntp_access_sonly, ntp_access_nvgen, no_alt,
	    OBJ(int,1), NTP_ACC_SERVE_ONLY,
	    "serve-only", "Provide only server access", PRIV_CONF);
TEST_MULTIPLE_FUNCS(ntp_serve_test, ntp_access_sonly, no_alt, NONE);
KEYWORD_ID (ntp_access_serve, ntp_access_nvgen, ntp_serve_test,
	    OBJ(int,1),	NTP_ACC_SERVE,
	    "serve", "Provide server and query access", PRIV_CONF);
KEYWORD_ID (ntp_access_peer, ntp_access_nvgen, ntp_access_serve,
	    OBJ(int,1), NTP_ACC_PEER,
	    "peer", "Provide full access", PRIV_CONF);
KEYWORD_ID (ntp_access_qonly, ntp_access_nvgen, ntp_access_peer,
	    OBJ(int,1), NTP_ACC_QUERY_ONLY,
	    "query-only", "Allow only control queries", PRIV_CONF);
KEYWORD (ntp_access_group, ntp_access_qonly, ntp_master_test,
	 "access-group", "Control NTP access", PRIV_CONF);

/******************************************************************************
* ntp source interface
* no ntp source
*
* OBJ(idb,1) = interface to get source address from
*/

EOLS    (ntp_source_eol, ntp_global_config_cmd, NTP_SOURCE_CMD);
INTERFACE(ntp_source_if, ntp_source_eol, no_alt, OBJ(idb,1), IFTYPE_ANYSUB);
NOPREFIX(ntp_source_no, ntp_source_if, ntp_source_eol);
NVGENS  (ntp_source_nvgen, ntp_source_no,
	 ntp_global_config_cmd, NTP_SOURCE_CMD);
KEYWORD (ntp_source, ntp_source_nvgen, ntp_access_group, "source",
	 "Configure interface for source address", PRIV_CONF);

/******************************************************************************
* ntp clock-period nnn
* no ntp clock-period
*
* OBJ(int,1) = clock period (or 0 for "no")
*/

EOLS    (ntp_clkper_eol, ntp_global_config_cmd, NTP_CLOCK_PERIOD_CMD);
NUMBER  (ntp_clkper_value, ntp_clkper_eol, no_alt,
	 OBJ(int,1), 0, 0xffffffff, "Clock period in 2^-32 seconds");
NOPREFIX(ntp_clkper_no, ntp_clkper_value, ntp_clkper_eol);
NVGENS  (ntp_clkper_nvgen, ntp_clkper_no,
	 ntp_global_config_cmd, NTP_CLOCK_PERIOD_CMD);
KEYWORD (ntp_clock_period, ntp_clkper_nvgen, ntp_source,
	 "clock-period", "Length of hardware clock tick", PRIV_CONF);

/******************************************************************************
* ntp broadcastdelay n
* no ntp broadcastdelay
*
* OBJ(int,1) = broadcast delay in microseconds
*/

EOLS    (ntp_broadcastdelay_eol, ntp_global_config_cmd, NTP_BROAD_DELAY_CMD);
NUMBER  (ntp_bdelay_value, ntp_broadcastdelay_eol, no_alt,
	 OBJ(int,1), 1, 999999, "Round-trip delay in microseconds");
NVGENS  (ntp_bcastdelay_nvgen, ntp_bdelay_value,
	 ntp_global_config_cmd, NTP_BROAD_DELAY_CMD);
NOPREFIX(ntp_bcastdelay_no, ntp_bcastdelay_nvgen, ntp_broadcastdelay_eol);
KEYWORD (ntp_bcastdelay, ntp_bcastdelay_no, ntp_clock_period,
	 "broadcastdelay", "Estimated round-trip delay", PRIV_CONF);
/******************************************************************************
* [no] ntp trusted-key ##
*
* OBJ(int,1) = key number
*/
EOLS    (ntp_trusted_key_eol, ntp_global_config_cmd, NTP_TRUSTED_KEY_CMD);
NUMBER  (ntp_trusted_key_number, ntp_trusted_key_eol, no_alt,
	 OBJ(int,1), 1, MAXUINT, "Key number");
NVGENS  (ntp_trusted_key_nvgen, ntp_trusted_key_number,
	 ntp_global_config_cmd, NTP_TRUSTED_KEY_CMD);
KEYWORD (ntp_trusted_key, ntp_trusted_key_nvgen, ntp_bcastdelay,
	 "trusted-key", "Key numbers for trusted time sources", PRIV_CONF);

/******************************************************************************
* [no] ntp authenticate
*
*/
EOLS    (ntp_authenticate_eol, ntp_global_config_cmd, NTP_AUTHENTICATE_CMD);
KEYWORD (ntp_authenticate, ntp_authenticate_eol, ntp_trusted_key, 
	 "authenticate", "Authenticate time sources", PRIV_CONF);

#ifdef notdef				/* dkatz 18 Aug 96 */
/******************************************************************************
* [no] ntp high-accuracy
*
*/
EOLS    (ntp_high_acc_eol, ntp_global_config_cmd, NTP_HIGH_ACC_CMD);
KEYWORD (ntp_high_acc, ntp_high_acc_eol, ntp_authenticate, 
	 "high-accuracy", "Enable high-accuracy NTP", PRIV_CONF);
#endif

/******************************************************************************
* ntp authentication-key ## md5 value [cryptotype]
* no ntp authentication-key ##
*
* OBJ(int,1) = key number
* OBJ(int,2) = value encryption type
* OBJ(int,3) = TRUE if encryption type specified
* OBJ(string,1) = key text
*/
EOLS    (ntp_auth_key_eol, ntp_global_config_cmd, NTP_AUTH_KEY_CMD);
SET	(ntp_auth_key_set, ntp_auth_key_eol, OBJ(int,3), TRUE);
NUMBER  (ntp_auth_key_crypto, ntp_auth_key_set, ntp_auth_key_eol,
	 OBJ(int,2), 0, MAXUINT, "Authentication key encryption type");
STRING  (ntp_auth_key_string, ntp_auth_key_crypto, no_alt,
	 OBJ(string,1), "Authentication key");
KEYWORD (ntp_auth_key_md5, ntp_auth_key_string, no_alt,
	 "md5", "MD5 authentication", PRIV_CONF);
NOPREFIX(ntp_auth_key_no, ntp_auth_key_md5, ntp_auth_key_eol);
NUMBER  (ntp_auth_key_number, ntp_auth_key_no, no_alt,
	 OBJ(int,1), 1, MAXUINT, "Key number");
NVGENS  (ntp_auth_key_nvgen, ntp_auth_key_number,
	 ntp_global_config_cmd, NTP_AUTH_KEY_CMD);
KEYWORD (ntp_auth_key, ntp_auth_key_nvgen, ntp_authenticate,
	 "authentication-key", "Authentication key for trusted time sources", 
	 PRIV_CONF);

KEYWORD (ntp, ntp_auth_key, ALTERNATE,
	 "ntp", "Configure NTP", PRIV_CONF);

#undef ALTERNATE
#define ALTERNATE ntp

