/* $Id: cfg_sntp.h,v 3.1.2.2 1996/08/12 16:04:50 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ntp/cfg_sntp.h,v $
 *------------------------------------------------------------------
 * Parse chains for SNTP
 *
 * June 1996, Dave Katz
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_sntp.h,v $
 * Revision 3.1.2.2  1996/08/12  16:04:50  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.1  1996/06/23  04:40:22  dkatz
 * CSCdi61119:  Commit SNTP Code
 * Branch: California_branch
 * Add an SNTP client.  This is not included in shipping IOS images.
 *
 * Revision 3.1  1996/06/22  21:14:38  dkatz
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* [no] sntp broadcast client
*/

EOLNS	(sntp_broadcast_eol, sntp_broadcast_command);
KEYWORD (sntp_broadcast_client, sntp_broadcast_eol, no_alt,
	 "client", "Enable SNTP broadcast client mode", PRIV_CONF);
KEYWORD (sntp_broadcast, sntp_broadcast_client, no_alt,
	 "broadcast", "Configure SNTP broadcast services", PRIV_CONF);

/******************************************************************************
* [no] sntp server ipaddr [version #]
*/

EOLNS	(sntp_server_eol, sntp_server_command);
NUMBER	(sntp_server_vernum, sntp_server_eol, no_alt,
	 OBJ(int,1), 1, NTP_VERSION, "NTP version number");
KEYWORD (sntp_server_version, sntp_server_vernum, sntp_server_eol,
	 "version", "Configure NTP version", PRIV_CONF);
SET	(sntp_server_version_set, sntp_server_version,
	 OBJ(int,1), SNTP_DEFAULT_VERSION);
NOPREFIX (sntp_server_noprefix, sntp_server_version_set, sntp_server_eol);
IPADDR_NAME (sntp_server_addr, sntp_server_noprefix, no_alt,
	     OBJ(paddr,1), "Name or IP address of server");
NVGENNS  (sntp_server_nv, sntp_server_addr, sntp_server_command);
KEYWORD (sntp_server, sntp_server_nv, sntp_broadcast,
	 "server", "Configure SNTP server", PRIV_CONF);
KEYWORD (sntp, sntp_server, ALTERNATE,
	 "sntp", "Configure SNTP", PRIV_CONF);

#undef ALTERNATE
#define ALTERNATE sntp

