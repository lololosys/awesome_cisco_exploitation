/* $Id: cfg_radius.h,v 3.5.32.3 1996/08/02 23:44:25 billw Exp $
 * $Source: /release/112/cvs/Xsys/servers/cfg_radius.h,v $
 *------------------------------------------------------------------
 * cfg_radius.h - parser macros for radius support
 *
 * July, 1995 - Bill Westfield
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_radius.h,v $
 * Revision 3.5.32.3  1996/08/02  23:44:25  billw
 * CSCdi60940:  RADIUS Accounting ID fields not globally unique
 * Branch: California_branch
 *
 * Revision 3.5.32.2  1996/06/24  23:29:42  billw
 * CSCdi54704:  RADIUS should be able to talk to different UDP ports
 * Branch: California_branch
 *
 * Revision 3.5.32.1  1996/04/24  02:06:18  billw
 * CSCdi51316:  RADIUS: implement better fallover behavior for down servers
 * Branch: California_branch
 *
 * Revision 3.5  1996/01/11  10:21:43  billw
 * CSCdi46570:  unsupported radius commands should be invisible
 * make them "internal" commands for slight easier addition later.
 *
 * Revision 3.4  1995/12/16  01:09:16  billw
 * CSCdi43681:  Some RADIUS code review comments not incorporated
 *
 * Revision 3.3  1995/12/12  06:41:29  billw
 * CSCdi44081:  radius secret should be radius key
 * continue to recognize both for a while.
 *
 * Revision 3.2  1995/11/17  18:50:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:24:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * radius_server key <text>
 * no radius_server key
 * Also "radius secret key, for compatibiity with EFT version
 */
EOLS	(conf_radius_secreteol, radius_cfgcmd, RADIUS_SECRET);
TEXT	(conf_radius_secrettxt, conf_radius_secreteol, no_alt,
	 OBJ(string,1), "Text of shared key");
NOPREFIX(conf_radius_secret_no, conf_radius_secrettxt, conf_radius_secreteol);
NVGENS	(conf_radius_secretnv, conf_radius_secret_no, radius_cfgcmd,
	 RADIUS_SECRET);
KEYWORD	(conf_radius_key, conf_radius_secretnv, ALTERNATE,
	"key",
	"encryption key shared with the radius servers",
	PRIV_CONF);
KEYWORD	(conf_radius_secret, conf_radius_secretnv, conf_radius_key,
	"secret","", PRIV_CONF|PRIV_HIDDEN|PRIV_NONVGEN);

/******************************************************************
 * radius_server unique-ident N
 * no radius_server unique-ident N
 */
PARAMS_KEYONLY (conf_radius_unique, conf_radius_secret,
		"unique-ident", OBJ(int,1), 1, 256,
		radius_cfgcmd, RADIUS_UNIQUEID,
		"high order bits for accounting ident",
		"high bits for accounting ident", PRIV_CONF|PRIV_HIDDEN);


/******************************************************************
 * radius_server optional-passwords
 * no radius_server optional-passwords
 */
EOLS	(conf_radius_pass_eol, radius_cfgcmd, RADIUS_PASSWORDOPT);
KEYWORD (conf_radius_pass_kwd, conf_radius_pass_eol, conf_radius_unique,
	 "optional-passwords",
	 "The first RADIUS request can be made without requesting a password",
	 PRIV_CONF);

/******************************************************************
 * radius_server extended-portnames
 * no radius_server extended-portnames
 */
EOLS	(conf_radius_extendedport_eol, radius_cfgcmd,
	 RADIUS_EXTENDED_PORTNAMES); 
KEYWORD (conf_radius_extendedport, conf_radius_extendedport_eol,
	 conf_radius_pass_kwd, 
	 "extended-portnames",
	 "Include parent channel info in NAS-Port Attribute",
	 PRIV_CONF);

/******************************************************************
 * radius_server directed-request
 * no radius_server directed-request
 */
EOLS	(conf_radius_directed_req_eol, radius_cfgcmd,
	 RADIUS_DIRECTED_REQUEST); 
KEYWORD (conf_radius_directed_req, conf_radius_directed_req_eol,
	 conf_radius_extendedport, 
	 "directed-request",
	 "Allow user to specify radius server to use with `@server'",
	 PRIV_CONF|PRIV_INTERNAL);

/******************************************************************
 * radius-server deadtime <minutes>
 * no radius-server deadtime
 */
PARAMS_KEYONLY (conf_radius_deadt, conf_radius_directed_req,
		"deadtime", OBJ(int,1), 1, 1440,
		radius_cfgcmd, RADIUS_DEADTVALUE,
		"Time to stop using a server that doesn't respond",
		"time in minutes", PRIV_CONF);

/******************************************************************
 * radius-server timeout <seconds>
 * no radius-server timeout [<seconds>]
 */
PARAMS_KEYONLY (conf_radius_time, conf_radius_deadt,
		"timeout", OBJ(int,1), 1, 1000,
		radius_cfgcmd, RADIUS_TIMEOUTVALUE,
		"Time to wait for a RADIUS server to reply",
		"Wait time (default 5 seconds)", PRIV_CONF);

/******************************************************************
 * radius-server retransmit <count>
 * no radius-server retransmit [<count>]
 */
PARAMS_KEYONLY (conf_radius_retrans, conf_radius_time,
		"retransmit", OBJ(int,1), 1, 100,
		radius_cfgcmd, RADIUS_RETRANSMIT,
		"Search iterations of the RADIUS server list",
		"Number of times to search the RADIUS list (default 2)",
		PRIV_CONF);

/******************************************************************
 * radius-server host {<ipaddr> | <name>}
 * no radius-server host {<ipaddr> | <name>}
 */
pdecl(conf_radacct_port);

EOLS	(conf_radius_host_eol, radius_cfgcmd, RADIUS_HOST);
NUMBER(conf_rad_portnum, conf_radacct_port, no_alt,
       OBJ(int,2), 0, 65536,
       "Port number");
KEYWORD(conf_rad_port, conf_rad_portnum, conf_radius_host_eol,
	"auth-port", 
	"UDP port for RADIUS authentication server (default is 1645)",
	PRIV_CONF);
NUMBER(conf_radacct_portnum, conf_rad_port, no_alt,
       OBJ(int,3), 0, 65536,
       "Port number");
KEYWORD(conf_radacct_port, conf_radacct_portnum, conf_rad_port,
	"acct-port", 
	"UDP port for RADIUS accounting server (default is 1646)",
	PRIV_CONF);

SET	(conf_radius_port1, conf_radacct_port,
	 OBJ(int,2), RADIUS_UDP_PORT);
SET	(conf_radius_port, conf_radius_port1,
	 OBJ(int,3), RADIUSACCT_UDP_PORT);

IPADDR_NAME (conf_radius_host_addr, conf_radius_port, no_alt,
	 OBJ(paddr,1), "IP address of RADIUS server");
NVGENS	(conf_radius_host_nvgen, conf_radius_host_addr, radius_cfgcmd, 
	 RADIUS_HOST);
KEYWORD	(conf_radius_host, conf_radius_host_nvgen, conf_radius_retrans,
	 "host", "Specify a RADIUS server", PRIV_CONF);

/******************************************************************/
KEYWORD	(conf_radius, conf_radius_host, ALTERNATE,
	 "radius-server", "Modify RADIUS query parameters", PRIV_CONF);

TEST_BOOLEAN(conf_radius_test, NONE, conf_radius, ALTERNATE,
	     old_access_control);


#undef	ALTERNATE
#define	ALTERNATE	conf_radius_test
