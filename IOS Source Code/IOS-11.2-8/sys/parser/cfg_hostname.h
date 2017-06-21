/* $Id: cfg_hostname.h,v 3.2.62.1 1996/08/12 16:05:34 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_hostname.h,v $
 *------------------------------------------------------------------
 * C F G _ H O S T N A M E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_hostname.h,v $
 * Revision 3.2.62.1  1996/08/12  16:05:34  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:42:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:50:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:03:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * hostname <name>
 */
EOLNS	(hostname_eol, hostname_command);

STRING	(hostname_name, hostname_eol, no_alt, OBJ(string,1),
	 "This system's network name");

NOPREFIX (hostname_noprefix, hostname_name, hostname_eol);
NVGENNS	(hostname_nvgen, hostname_noprefix, hostname_command);
KEYWORD	(sys_hostname, hostname_nvgen, ALTERNATE, "hostname",
	 "Set system's network name", PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE	sys_hostname
