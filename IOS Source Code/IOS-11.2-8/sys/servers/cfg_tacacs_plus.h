/* $Id: cfg_tacacs_plus.h,v 3.1 1996/02/01 04:59:07 hampton Exp $
 * $Source: /release/111/cvs/Xsys/servers/cfg_tacacs_plus.h,v $
 *------------------------------------------------------------------
 * TACACS+ specific parse chains
 *
 * March 1995, Robert Widmer
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_tacacs_plus.h,v $
 * Revision 3.1  1996/02/01  04:59:07  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:45:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:14:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/* [no] tacacs-server dns-alias-lookup
 *
 * Defines whether or not we should do a dns lookup to find possible
 * secondary addresses for a server and try contacting the server
 * through secondary interfaces.
 *
 * 'no' means do not find secondary records.
 *
 */
EOLS (conf_tacacs_alias_eol, tacacs_plus_command, TACACS_ALIAS_LOOKUP);
KEYWORD (conf_tacacs_alias, conf_tacacs_alias_eol, NONE,
	 "dns-alias-lookup",
	 "Enable IP Domain Name System Alias lookup for TACACS servers",
	 PRIV_CONF);

/******************************************************************
 * [no] tacacs-server key <string>
 */
EOLS (conf_tacacs_key_eol, tacacs_plus_command, TACACS_KEY);
TEXT (conf_tacacs_key_str, conf_tacacs_key_eol, no_alt, OBJ (string, 1),
      "Encryption key string");
NOPREFIX (conf_tacacs_key_no, conf_tacacs_key_str, conf_tacacs_key_eol);
NVGENS (conf_tacacs_key_nvgen, conf_tacacs_key_no, tacacs_plus_command,
	TACACS_KEY);
KEYWORD (conf_tacacs_key, conf_tacacs_key_nvgen, conf_tacacs_alias,
	 "key", "Set TACACS+ encryption key.", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	conf_tacacs_key
