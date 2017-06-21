/* $Id: exec_debug_novigrp.h,v 3.1.2.2 1996/08/16 00:14:18 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/exec_debug_novigrp.h,v $
 *------------------------------------------------------------------
 * IPX EIGRP debug commands.
 *
 * April, 1996, Steven Luong
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_novigrp.h,v $
 * Revision 3.1.2.2  1996/08/16  00:14:18  hampton
 * Change parser comments to reflect ipx command usage.  [CSCdi66208]
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/03  02:16:28  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.1  1996/04/13  01:24:49  sluong
 * Add placeholders
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug ipx eigrp [events]
 * debug ipx eigrp <AS> [<ipx-network-number>] [<ipx-network-mask>]
 *
 */
EOLS    (debug_novell_igrp_events_eol, debug_command, DEBUG_NOVELLIGRPEVT);
EOLS    (debug_novell_igrp_eol, debug_command, DEBUG_NOVELLIGRP);
EOLS    (debug_novell_igrp_target_eol, novigrp_debug_command, IPXIGRP_DEB_FSM);
HEXDIGIT (debug_novell_igrp_mask, debug_novell_igrp_target_eol,
	  debug_novell_igrp_target_eol,
	  OBJ(int, 3), 0x80000000, 0xFFFFFFFF, "IPX network mask");
HEXDIGIT (debug_novell_igrp_dest, debug_novell_igrp_target_eol,
	  debug_novell_igrp_target_eol,
	  OBJ(int, 2), NOV_MIN_NET, NOV_RIP_DEFAULT_ROUTE, "IPX address");
NUMBER (debug_novell_igrp_as, debug_novell_igrp_dest, debug_novell_igrp_eol,
	OBJ(int,1), 1, 0xffff, "AS number");
KEYWORD (debug_novell_igrp_events, debug_novell_igrp_events_eol,
	debug_novell_igrp_as, "events", "IPX EIGRP events", PRIV_OPR);

/******************************************************************
 * debug ipx eigrp neighbor <AS> <ipx-address>
 *
 */
EOLS    (debug_novell_igrp_neigh_eol, novigrp_debug_command, IPXIGRP_DEB_NEI);
NOVADDR	(debug_novell_igrp_neigh_addr, debug_novell_igrp_neigh_eol,
	 no_alt, OBJ(paddr,1),
	 "NOVELL-EIGRP neighbor address");
NUMBER (debug_novell_igrp_neigh_as, debug_novell_igrp_neigh_addr,
	no_alt, OBJ(int,1), 1, 0xffff, "AS number");
KEYWORD (debug_novell_igrp_neigh, debug_novell_igrp_neigh_as,
	 debug_novell_igrp_events, "neighbor", "IPX neighbor", PRIV_OPR);
KEYWORD (debug_novell_igrp, debug_novell_igrp_neigh, no_alt,
	"eigrp", "IPX EIGRP packets", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_novell_igrp
