/* $Id: cfg_ip_domain.h,v 3.1.2.2 1996/08/12 16:07:08 widmer Exp $
 * $Source: /release/112/cvs/Xsys/servers/cfg_ip_domain.h,v $
 *------------------------------------------------------------------
 * IP DNS commands.
 *
 * Januar 1996, Bruce Cole
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_ip_domain.h,v $
 * Revision 3.1.2.2  1996/08/12  16:07:08  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.1  1996/04/15  21:22:35  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/15  01:40:26  bcole
 * Placeholders for 11.2 features.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/****************************************************************************
 * ip domain-list <name>
 * no ip domain-list [<name>]
 *
 * OBJ(int,1)	= TRUE = <name> was given
 *		= FALSE = no <name>
 * OBJ(string,1) = <name>
 *
 * Only the named entity is deleted if 'no ip domain-list <name>'.
 * The full list is deleted if 'no ip domain-list'.
 */

EOLNS	(cip_domainlist_eol, domainlist_command);
SET	(cip_domainlist_clr, cip_domainlist_eol, OBJ(int,1), FALSE);

SET	(cip_domainlist_set, cip_domainlist_eol, OBJ(int,1), TRUE);
STRING	(cip_domainlist_name, cip_domainlist_set, cip_domainlist_eol,
	OBJ(string,1), "A domain name");
NOPREFIX (cip_domainlist_noprefix, cip_domainlist_name, cip_domainlist_eol);
NVGENNS	(cip_domainlist_nvgen, cip_domainlist_noprefix,
	 domainlist_command);
KEYWORD	(cip_domainlist, cip_domainlist_nvgen, ALTERNATE,
	"domain-list", "Domain name to complete unqualified host names.",
	 PRIV_CONF);
#undef	ALTERNATE
#define	ALTERNATE	cip_domainlist
