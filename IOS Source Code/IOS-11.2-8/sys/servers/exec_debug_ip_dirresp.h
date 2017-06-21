/* $Id: exec_debug_ip_dirresp.h,v 3.1.18.1 1996/07/16 08:01:27 raj Exp $
 * $Source: /release/112/cvs/Xsys/servers/exec_debug_ip_dirresp.h,v $
 *------------------------------------------------------------------
 * director-responder debug commands
 *
 * Feburary 1996, Paul Traina
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_debug_ip_dirresp.h,v $
 * Revision 3.1.18.1  1996/07/16  08:01:27  raj
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
 * debug ip director-responder
 *
 */
EOLS	(debug_ip_dirresp_eol, dirresp_debug_command, DEBUG_DIRRESP);
KEYWORD (debug_ip_dirresp, debug_ip_dirresp_eol, ALTERNATE,
	 "drp", "Director response protocol", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE debug_ip_dirresp
