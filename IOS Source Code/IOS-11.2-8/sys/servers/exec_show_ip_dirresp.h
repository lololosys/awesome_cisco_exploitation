/* $Id: exec_show_ip_dirresp.h,v 3.1.18.2 1996/08/12 16:07:10 widmer Exp $
 * $Source: /release/112/cvs/Xsys/servers/exec_show_ip_dirresp.h,v $
 *------------------------------------------------------------------
 * Director-Responder show commands
 *
 * Feburary 1996, Paul Traina
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_show_ip_dirresp.h,v $
 * Revision 3.1.18.2  1996/08/12  16:07:10  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.18.1  1996/07/16  08:01:28  raj
 * CSCdi61197:  Part of  CSCdi49395 patches missing from 11.2
 * Branch: California_branch
 * Inserted patches which were missing and update dirresp code as well.
 *
 * Revision 3.1  1996/02/20  23:50:13  pst
 * Add rest of client files...
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show ip drp statistics
 */
EOLNS	(show_ip_dirresp_eol, dirresp_show);
KEYWORD (show_ip_dirresp, show_ip_dirresp_eol, ALTERNATE,
	 "drp", "Director response protocol", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_ip_dirresp
