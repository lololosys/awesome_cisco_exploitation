/* $Id: cfg_ip_bootp.h,v 3.1.2.2 1996/08/12 16:07:07 widmer Exp $
 * $Source: /release/112/cvs/Xsys/servers/cfg_ip_bootp.h,v $
 *------------------------------------------------------------------
 * IP BOOTP commands.
 *
 * Januar 1996, Bruce Cole
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_ip_bootp.h,v $
 * Revision 3.1.2.2  1996/08/12  16:07:07  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.1  1996/04/15  21:22:33  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/15  01:40:25  bcole
 * Placeholders for 11.2 features.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 *	ip bootp server 
 *	no ip bootp server 
 */

EOLNS	(cip_bootp_server_eol, bootp_server_command);
KEYWORD	(cip_bootp_server, cip_bootp_server_eol, no_alt, 
	 "server", "Enable BOOTP server", PRIV_CONF);
KEYWORD	(cip_bootp, cip_bootp_server, ALTERNATE,
	 "bootp", "Config BOOTP services", PRIV_CONF);
#undef	ALTERNATE
#define	ALTERNATE	cip_bootp
