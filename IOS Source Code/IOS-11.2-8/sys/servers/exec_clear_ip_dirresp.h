/* $Id: exec_clear_ip_dirresp.h,v 3.1.2.1 1996/08/15 06:49:51 raj Exp $
 * $Source: /release/112/cvs/Xsys/servers/exec_clear_ip_dirresp.h,v $
 *------------------------------------------------------------------
 * Director Responder clear commands
 *
 * July 1996, Richard Johnson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: exec_clear_ip_dirresp.h,v $
 * Revision 3.1.2.1  1996/08/15  06:49:51  raj
 * CSCdi61197:  Part of  CSCdi49395 patches missing from 11.2
 * Branch: California_branch
 * Update Director Responder Agent to the latest version.
 *
 * Revision 3.1  1996/08/15  05:40:05  raj
 * CSCdi61197:  Part of  CSCdi49395 patches missing from 11.2
 * Updating Director Responder code to agree with current version.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear ip drp
 *
 */
EOLS	(clear_ip_drp_eol, dirresp_clear, 0);
KEYWORD	(clear_ip_dirresp, clear_ip_drp_eol, ALTERNATE,
	 "drp", "Clear director responder counters", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	clear_ip_dirresp
