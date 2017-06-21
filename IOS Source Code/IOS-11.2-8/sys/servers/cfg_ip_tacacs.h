
/* $Id: cfg_ip_tacacs.h,v 3.1.68.1 1996/06/19 23:19:17 irfan Exp $
 * $Source: /release/112/cvs/Xsys/servers/cfg_ip_tacacs.h,v $
 *------------------------------------------------------------------
 * CFG_IP_TACACS:  Contains parser chains for "ip tacacs ..."
 *
 * Dec 1995, Syed Mohammad Irfan Ashraf
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_ip_tacacs.h,v $
 * Revision 3.1.68.1  1996/06/19  23:19:17  irfan
 * CSCdi60721:  tacacs source-interface doesnt use FORWARD_INTERFACE macro
 * Branch: California_branch
 *
 * Revision 3.1  1995/12/08  00:54:51  irfan
 * CSCdi45224:  tacacs could use a specified source address
 * allow tacacs (TA, XTA, TA+) to use a specific (up) interface's IP
 * address.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * parser chain for ip tacacs commands
 */

/******************************************************************************
 *	[no] ip tacacs source-interface <sub-interface>
 *
 * OBJ(idb,1) -- IDB to pull source IP address from
 */

EOLS	(cip_tacacs_source_eol, tacacs_service, TACACS_SOURCE_INTERFACE);

FORWARD_INTERFACE (cip_tacacs_source_name, cip_tacacs_source_eol, no_alt,
		   OBJ(idb,1), IFTYPE_ANYSUB);

NOPREFIX (cip_tacacs_source_noprefix, cip_tacacs_source_name,
	  cip_tacacs_source_eol);

NVGENS	(cip_tacacs_source_nvgen, cip_tacacs_source_noprefix,
	 tacacs_service, TACACS_SOURCE_INTERFACE);

KEYWORD	(cip_tacacs_source, cip_tacacs_source_nvgen, no_alt,
	 "source-interface",
	 "Specify interface for source address in TACACS packets",
	 PRIV_CONF);

KEYWORD	(cip_tacacs, cip_tacacs_source, ALTERNATE,
	 "tacacs", "TACACS configuration commands",
	 PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cip_tacacs
