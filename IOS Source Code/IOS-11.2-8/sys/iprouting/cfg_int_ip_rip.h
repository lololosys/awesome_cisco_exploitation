/* $Id: cfg_int_ip_rip.h,v 3.5 1995/12/10 00:37:30 pst Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/iprouting/cfg_int_ip_rip.h,v $
 *------------------------------------------------------------------
 * RIP interface commands
 *
 * March 1995, Paul Traina
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_ip_rip.h,v $
 * Revision 3.5  1995/12/10  00:37:30  pst
 * CSCdi45333:  RIP: parse chain calls wrong nvgen routine
 *
 * Revision 3.4  1995/12/02  10:15:19  pst
 * CSCdi44892:  make the authentication mode command internal for the
 * moment
 *
 * Revision 3.3  1995/12/02  10:12:58  pst
 * CSCdi44892:  RIP: do clean up pass on authentication and version
 * control code
 *
 * Revision 3.2  1995/11/17  17:31:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:04:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:06:12  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/11  19:37:55  pst
 * Add placeholders
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 *	[no] ip rip authentication key-chain <name-of-chain>
 *
 * OBJ(string,1) = chain name
 */

EOLS	(ci_ip_rip_auth_key_eol, ip_rip_int_command, RIP_AUTH_KEY_CMD);
TEXT	(ci_ip_rip_auth_key_text, ci_ip_rip_auth_key_eol, no_alt, 
	 OBJ(string,1), "name of key-chain");

NOPREFIX (ci_ip_rip_auth_key_noprefix, ci_ip_rip_auth_key_text, 
	  ci_ip_rip_auth_key_eol);

NVGENS	(ci_ip_rip_auth_key_nvgen, ci_ip_rip_auth_key_noprefix,
	 ip_rip_int_command, RIP_AUTH_KEY_CMD);

KEYWORD (ci_ip_rip_auth_key, ci_ip_rip_auth_key_nvgen, no_alt,
	 "key-chain", "Authentication key-chain", PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 *      [no] ip rip authentication mode { text | md5 }
 *
 * OBJ(int,1) = mode
 */
EOLS	(ci_ip_rip_auth_mode_eol, ip_rip_int_command, RIP_AUTH_MODE_CMD);

KEYWORD_ID(ci_ip_rip_auth_mode_md, ci_ip_rip_auth_mode_eol, no_alt,
	  OBJ(int,1), RIP_AUTH_TYPE_MD,
	  "md5", "Keyed message digest", PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(ci_ip_rip_auth_mode_kw, ci_ip_rip_auth_mode_eol,
	   ci_ip_rip_auth_mode_md, OBJ(int,1), RIP_AUTH_TYPE_TEXT,
	   "text", "Clear text authentication", PRIV_CONF|PRIV_SUBIF);

NOPREFIX (ci_ip_rip_auth_mode_no, ci_ip_rip_auth_mode_kw,
	  ci_ip_rip_auth_mode_eol);

NVGENS	(ci_ip_rip_auth_mode_nv, ci_ip_rip_auth_mode_no,
	 ip_rip_int_command, RIP_AUTH_MODE_CMD);

KEYWORD (ci_ip_rip_auth_mode, ci_ip_rip_auth_mode_nv, ci_ip_rip_auth_key,
	 "mode", "Authentication mode", PRIV_CONF|PRIV_SUBIF);

KEYWORD	(ci_ip_rip_auth, ci_ip_rip_auth_mode, no_alt,
	 "authentication", "Authentication control", PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 *	[no] ip rip receive version { [1] [2] }
 *
 * OBJ(int,1) = mode
 */
EOLS	(ci_ip_rip_recv_ver_eol, ip_rip_int_command, RIP_RECV_VERS_CMD);

pdecl(ci_ip_rip_recv_ver_opts);

KEYWORD_ORTEST(ci_ip_rip_recv_ver_2, ci_ip_rip_recv_ver_opts, NONE,
	       OBJ(int,1), (1<<2), "2", "RIP version 2", PRIV_CONF|PRIV_SUBIF);

KEYWORD_ORTEST(ci_ip_rip_recv_ver_1, ci_ip_rip_recv_ver_opts,
	       ci_ip_rip_recv_ver_2,
	       OBJ(int,1), (1<<1), "1", "RIP version 1", PRIV_CONF|PRIV_SUBIF);

NOP	(ci_ip_rip_recv_ver_opts, ci_ip_rip_recv_ver_1, ci_ip_rip_recv_ver_eol);

NOPREFIX(ci_ip_rip_recv_ver_no, ci_ip_rip_recv_ver_opts,
	 ci_ip_rip_recv_ver_eol);

NVGENS	(ci_ip_rip_recv_ver_nv, ci_ip_rip_recv_ver_no,
	 ip_rip_int_command, RIP_RECV_VERS_CMD);

KEYWORD	(ci_ip_rip_recv_ver, ci_ip_rip_recv_ver_nv, NONE,
	 "version", "version control", PRIV_CONF|PRIV_SUBIF);

KEYWORD	(ci_ip_rip_recv, ci_ip_rip_recv_ver, ci_ip_rip_auth,
	 "receive", "advertisement reception", PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 *	[no] ip rip send version { [1] [2] }
 *
 * OBJ(int,1) = mode
 */
EOLS	(ci_ip_rip_send_ver_eol, ip_rip_int_command, RIP_SEND_VERS_CMD);

pdecl(ci_ip_rip_send_ver_opts);

KEYWORD_ORTEST(ci_ip_rip_send_ver_2, ci_ip_rip_send_ver_opts, NONE,
	       OBJ(int,1), (1<<2), "2", "RIP version 2", PRIV_CONF|PRIV_SUBIF);

KEYWORD_ORTEST(ci_ip_rip_send_ver_1, ci_ip_rip_send_ver_opts,
	       ci_ip_rip_send_ver_2,
	       OBJ(int,1), (1<<1), "1", "RIP version 1", PRIV_CONF|PRIV_SUBIF);

NOP	(ci_ip_rip_send_ver_opts, ci_ip_rip_send_ver_1, ci_ip_rip_send_ver_eol);

NOPREFIX(ci_ip_rip_send_ver_no, ci_ip_rip_send_ver_opts,
	 ci_ip_rip_send_ver_eol);

NVGENS	(ci_ip_rip_send_ver_nv, ci_ip_rip_send_ver_no,
	 ip_rip_int_command, RIP_SEND_VERS_CMD);

KEYWORD	(ci_ip_rip_send_ver, ci_ip_rip_send_ver_nv, NONE,
	 "version", "version control", PRIV_CONF|PRIV_SUBIF);

KEYWORD	(ci_ip_rip_send, ci_ip_rip_send_ver, ci_ip_rip_recv,
	 "send", "advertisement transmission", PRIV_CONF|PRIV_SUBIF);

KEYWORD	(ci_ip_rip, ci_ip_rip_send, ALTERNATE,
	 "rip", "Router Information Protocol", PRIV_CONF|PRIV_SUBIF);

#undef	ALTERNATE
#define	ALTERNATE	ci_ip_rip
