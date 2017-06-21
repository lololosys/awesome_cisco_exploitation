/* $Id: exec_clear_host.h,v 3.2.62.1 1996/08/12 16:06:02 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/exec_clear_host.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L E A R _ H O S T . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_clear_host.h,v $
 * Revision 3.2.62.1  1996/08/12  16:06:02  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:45:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:15:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear host { <name> | * }
 *
 * OBJ(string,1) = <name> or "*"
 */
EOLNS	(clear_host_eol, name_clear_host);
SET	(clear_host_name_set, clear_host_eol, OBJ(int,1), TRUE);
STRING	(clear_host_name, clear_host_name_set, no_alt,
	 OBJ(string,1), "Hostname to delete (* for all entries)");
KEYWORD_ID (name_clear_host, clear_host_name, ALTERNATE,
	 OBJ(int,1), FALSE,
	 "host", "Delete host table entries", PRIV_OPR);


#undef	ALTERNATE
#define	ALTERNATE	name_clear_host
