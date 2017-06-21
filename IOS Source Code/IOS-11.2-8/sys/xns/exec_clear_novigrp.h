/* $Id: exec_clear_novigrp.h,v 3.1.2.2 1996/08/12 16:10:02 widmer Exp $
 * $Source: /release/112/cvs/Xsys/xns/exec_clear_novigrp.h,v $
 *------------------------------------------------------------------
 * IPX EIGRP clear commands.
 *
 * April, 1996, Steven Luong
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_clear_novigrp.h,v $
 * Revision 3.1.2.2  1996/08/12  16:10:02  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.1  1996/05/03  02:16:18  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.1  1996/04/13  01:24:48  sluong
 * Add placeholders
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear ipx eigrp events
 */
EOLNS	(clear_ipx_igrp2_log_eol, novigrp_clear_log);
KEYWORD (clear_ipx_igrp2_log, clear_ipx_igrp2_log_eol, no_alt,
 	 "events", NULL /*"Clear IPX-EIGRP event logs"*/,
	 PRIV_OPR|PRIV_HIDDEN);

/******************************************************************
 * clear ipx eigrp logging 
 */
EOLNS	(clear_ipx_igrp2_evlog_eol, novigrp_clear_event_logging);
KEYWORD (clear_ipx_igrp2_evlog, clear_ipx_igrp2_evlog_eol,
	 clear_ipx_igrp2_log,
	 "logging", NULL /*"Stop IPX-EIGRP event logging"*/,
	 PRIV_OPR|PRIV_HIDDEN);

/******************************************************************
 * clear ipx eigrp neighbors [<novell-address> | <interface-unit>]
 *
 * OBJ(paddr,1) = Neighbor Novell address
 * OBJ(idb,1)   = Interface of neighbors to get cleared
 * OBJ(int,1) = net
 * OBJ(int,2) = TRUE if * is entered
 * OBJ(int,3) = mask if present
 */
EOLNS (clear_novell_igrp2_nbr_eol, novigrp_clear_neighbors);

INTERFACE (clear_novell_igrp2_nbr_int, clear_novell_igrp2_nbr_eol,
	   clear_novell_igrp2_nbr_eol, OBJ(idb,1), IFTYPE_ANYSUB);

NOVADDR	(clear_novell_igrp2_nbr_addr, clear_novell_igrp2_nbr_eol, 
	 clear_novell_igrp2_nbr_int, OBJ(paddr,1), "NOVELL-EIGRP neighbor address");

KEYWORD (clear_novell_igrp2_nbr, clear_novell_igrp2_nbr_addr,
	 clear_ipx_igrp2_evlog, "neighbors", "Clear NOVELL-EIGRP neighbors",
	 PRIV_OPR);

KEYWORD (clear_novell_igrp2, clear_novell_igrp2_nbr, ALTERNATE,
	 "eigrp", "Clear NOVELL-EIGRP", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	clear_novell_igrp2
