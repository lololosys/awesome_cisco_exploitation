/* $Id: cfg_rlogin.h,v 3.1 1995/11/09 13:38:52 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/ts/cfg_rlogin.h,v $
 *------------------------------------------------------------------
 * Parse chain for rlogin config commands
 *
 * August 1995, Syed Mohammad Irfan Ashraf
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_rlogin.h,v $
 * Revision 3.1  1995/11/09  13:38:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/08  21:55:36  widmer
 * CSCdi38347:  parser help broken for ip
 * Broken alternate for rlogin config command
 *
 * Revision 2.1  1995/08/01  23:09:51  irfan
 * CSCdi37754:  rlogin should allow various sources of remote username
 * Allow a /user switch for rlogin command. Also let the current username
 * be passed as remote username, if rlogin trusted-remoteuser-source is
 * local. Besides, send the local username or "ciscoTS" if the current
 * username was authenticated via one of trusted methods.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 ***************************************************************
 * Config command:
 *
 *  [no]  rlogin trusted-localuser-source  < local | tacacs | radius >
 ***************************************************************
 */

EOLS(cfg_rlogin_trust_luser_radius_eol,
     rlogin_trusted_localuser_source_command,
     RLOGIN_TRUSTED_LNAME_RADIUS);

EOLS(cfg_rlogin_trust_luser_tacacs_eol,
     rlogin_trusted_localuser_source_command,
     RLOGIN_TRUSTED_LNAME_TACACS);

EOLS(cfg_rlogin_trust_luser_local_eol,
     rlogin_trusted_localuser_source_command,
     RLOGIN_TRUSTED_LNAME_LOCAL);

KEYWORD(cfg_rlogin_trust_luser_radius, cfg_rlogin_trust_luser_radius_eol,
	no_alt,
	"radius", "Use local username from radius authentication",
	PRIV_CONF);

KEYWORD(cfg_rlogin_trust_luser_tacacs, cfg_rlogin_trust_luser_tacacs_eol,
	cfg_rlogin_trust_luser_radius,
	"tacacs", "Use local username from tacacs authentication",
	PRIV_CONF);

KEYWORD(cfg_rlogin_trust_luser_local, cfg_rlogin_trust_luser_local_eol,
	cfg_rlogin_trust_luser_tacacs,
	"local", "Use local username from any authentication method",
	PRIV_CONF);

NVGENS(cfg_rlogin_trust_luser_nvgen, cfg_rlogin_trust_luser_local,
       rlogin_trusted_localuser_source_command, 0);

KEYWORD(cfg_rlogin_trust_luser, cfg_rlogin_trust_luser_nvgen,
	no_alt,
	"trusted-localuser-source",
	"Allowed authentication types for local username",
	PRIV_CONF);


/*
 ***************************************************************
 * Config command:
 *
 *  [no]  rlogin trusted-remoteuser-source  < local | tacacs | radius >
 ***************************************************************
 */


EOLS(cfg_rlogin_trust_ruser_radius_eol, rlogin_trust_remoteuser_command,
     RLOGIN_TRUSTEDHOST_RADIUS);

EOLS(cfg_rlogin_trust_ruser_tacacs_eol, rlogin_trust_remoteuser_command,
     RLOGIN_TRUSTEDHOST_TACACS);

EOLS(cfg_rlogin_trust_ruser_local_eol, rlogin_trust_remoteuser_command,
     RLOGIN_TRUSTEDHOST_LOCAL);

EOLS(cfg_rlogin_trust_ruser_none_eol, rlogin_trust_remoteuser_command,
     RLOGIN_TRUSTEDHOST_DEFAULT);


KEYWORD(cfg_rlogin_trust_ruser_radius, cfg_rlogin_trust_ruser_radius_eol,
	no_alt,
	"radius", "Use radius returned username", PRIV_CONF|PRIV_HIDDEN);

KEYWORD(cfg_rlogin_trust_ruser_tacacs, cfg_rlogin_trust_ruser_tacacs_eol,
	cfg_rlogin_trust_ruser_radius,
	"tacacs", "Use tacacs+ returned username", PRIV_CONF|PRIV_HIDDEN);

KEYWORD(cfg_rlogin_trust_ruser_local, cfg_rlogin_trust_ruser_local_eol,
	cfg_rlogin_trust_ruser_tacacs,
	"local", "Use current line username", PRIV_CONF);

NOPREFIX(cfg_rlogin_trust_ruser_no, cfg_rlogin_trust_ruser_local,
	 cfg_rlogin_trust_ruser_none_eol);

KEYWORD(cfg_rlogin_trust_ruser, cfg_rlogin_trust_ruser_no,
	cfg_rlogin_trust_luser,
	"trusted-remoteuser-source",
	"Method used to get remote username",
	PRIV_CONF);

KEYWORD(cfg_rlogin, cfg_rlogin_trust_ruser, ALTERNATE,
	"rlogin", "Rlogin configuration commands",
	PRIV_CONF);


#undef	ALTERNATE
#define	ALTERNATE	cfg_rlogin
