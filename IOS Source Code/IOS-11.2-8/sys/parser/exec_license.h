/* $Id: exec_license.h,v 3.2.62.1 1996/08/12 16:06:09 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/exec_license.h,v $
 *------------------------------------------------------------------
 * E X E C _ L I C E N S E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_license.h,v $
 * Revision 3.2.62.1  1996/08/12  16:06:09  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:46:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:21:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * license [<host>]
 */

EOLNS	(exec_license_eol, license_command);
IPADDR_NAME(exec_license_host, exec_license_eol, exec_license_eol,
	    OBJ(paddr,1), NULL /*"License host"*/);
KEYWORD	(exec_license, exec_license_eol, ALTERNATE,
	 "license", NULL, PRIV_OPR | PRIV_HIDDEN );


#undef	ALTERNATE
#define	ALTERNATE	exec_license
