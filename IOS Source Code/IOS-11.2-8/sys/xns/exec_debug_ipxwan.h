/* $Id: exec_debug_ipxwan.h,v 3.1.2.1 1996/05/03 02:16:21 sluong Exp $
 * $Source: /release/112/cvs/Xsys/xns/exec_debug_ipxwan.h,v $
 *------------------------------------------------------------------
 * IPX WAN debug commands.
 *
 * April, 1996, Steven Luong
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_ipxwan.h,v $
 * Revision 3.1.2.1  1996/05/03  02:16:21  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.1  1996/04/13  01:24:51  sluong
 * Add placeholders
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug ipx ipxwan
 *
 */
EOLS	(debug_novell_ipxwan_eol, debug_command, DEBUG_NOVELLIPXWAN);
KEYWORD (debug_novell_ipxwan, debug_novell_ipxwan_eol, no_alt,
	 "ipxwan", "Novell IPXWAN events", PRIV_OPR);

#undef ALTERNATE
#define	ALTERNATE	debug_novell_ipxwan
