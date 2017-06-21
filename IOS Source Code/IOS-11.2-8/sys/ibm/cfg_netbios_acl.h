/* $Id: cfg_netbios_acl.h,v 3.3 1996/03/04 07:14:01 kchiu Exp $
 * $Source: /release/111/cvs/Xsys/ibm/cfg_netbios_acl.h,v $
 *------------------------------------------------------------------
 * Netbios access list list config commands
 *
 * September 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_netbios_acl.h,v $
 * Revision 3.3  1996/03/04  07:14:01  kchiu
 * CSCdi49101:  make netbios name recognized filtering optional
 *
 * Revision 3.2  1995/11/17  09:22:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:51  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:11:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/***************************************************************
 * [no] netbios access-list filter-name-recognized
 *
 */

EOLS  (netbios_acl_filter_nr_eol, netbios_acl,
       NETBIOS_ACCESS_FILTER_NAME_RECOGNIZED);

NVGENS (netbios_acl_filter_nr_nvgen, netbios_acl_filter_nr_eol,
        netbios_acl, NETBIOS_ACCESS_FILTER_NAME_RECOGNIZED );

KEYWORD (config_netbios_acl_filter_nr, netbios_acl_filter_nr_nvgen, no_alt,
       "filter-name-recognized", "Filter the Name Recognized frame", PRIV_CONF);


/***************************************************************
 * netbios access-list bytes <name> {permit|deny} <offset> <pattern>
 * no netbios access-list bytes <name> [{permit|deny} <offset> <pattern>]
 *
 * OBJ(string,1) = name
 * OBJ(int,1) = 1 = permit
 *		2 = deny
 *		0 = none given
 * OBJ(int,2) = offset
 * OBJ(string,2) = pattern
 *
 */

EOLS	(config_netbios_albytes_eol, netbios_acl,
	 NETBIOS_ACCESS_BYTES);

STRING	(config_netbios_albytes_pattern, config_netbios_albytes_eol, no_alt,
	 OBJ(string,2), "Hexadecimal byte pattern to match");

NUMBER(config_netbios_albytes_offset, config_netbios_albytes_pattern, no_alt,
	OBJ(int,2), 0, -1, "Byte offset where comparison should begin");

KEYWORD_ID(config_netbios_albytes_deny, config_netbios_albytes_offset,
	   no_alt, OBJ(int,1), 2,
	   "deny","Specify packets to reject", PRIV_CONF);
KEYWORD_ID(config_netbios_albytes_permit, config_netbios_albytes_offset,
	   config_netbios_albytes_deny,
	   OBJ(int,1), 1,
	   "permit","Specify packets to forward", PRIV_CONF);

NOPREFIX(config_netbios_albytes_noprefix, config_netbios_albytes_permit,
	 config_netbios_albytes_eol);

STRING(config_netbios_albytes_name, config_netbios_albytes_noprefix, no_alt,
       OBJ(string,1), "NETBIOS station name");

NVGENS	(config_netbios_albytes_nvgen, config_netbios_albytes_name,
	 netbios_acl, NETBIOS_ACCESS_BYTES );

KEYWORD	(config_netbios_albytes, config_netbios_albytes_nvgen, 
         config_netbios_acl_filter_nr,
	 "bytes","NETBIOS byte offset access list", PRIV_CONF);

/***************************************************************
 * netbios access-list host <name> {permit|deny} <pattern>
 * no netbios access-list host <name> [{permit|deny} <pattern>]
 *
 * OBJ(string,1) = name
 * OBJ(int,1) = 1 = permit
 *		2 = deny
 *		0 = none given
 * OBJ(string,2) = pattern
 *
 */

EOLS	(config_netbios_alhost_eol, netbios_acl,
	 NETBIOS_ACCESS_HOST);
STRING	(config_netbios_alhost_pattern, config_netbios_alhost_eol, no_alt,
	 OBJ(string,2), "Pattern for a set of NETBIOS station names");

KEYWORD_ID(config_netbios_alhost_deny, config_netbios_alhost_pattern,
	   no_alt,	OBJ(int,1), 2,
	   "deny","Specify packets to reject", PRIV_CONF);
KEYWORD_ID(config_netbios_alhost_permit, config_netbios_alhost_pattern,
	   config_netbios_alhost_deny, OBJ(int,1), 1,
	   "permit", "Specify packets to forward", PRIV_CONF);

NOPREFIX(config_netbios_alhost_noprefix, config_netbios_alhost_permit,
	 config_netbios_alhost_eol);
STRING	(config_netbios_alhost_name, config_netbios_alhost_noprefix, no_alt,
	 OBJ(string,1), "NETBIOS station name");
NVGENS	(config_netbios_alhost_nvgen, config_netbios_alhost_name,
	 netbios_acl, NETBIOS_ACCESS_HOST );

KEYWORD(config_netbios_alhost, config_netbios_alhost_nvgen,
	config_netbios_albytes,
	"host", "Assign name of access list to NETBIOS station[s]", PRIV_CONF);


/***************************************************************/
KEYWORD	(config_netbios_al, config_netbios_alhost, ALTERNATE,
	 "access-list", "Define NETBIOS station access list", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	config_netbios_al
