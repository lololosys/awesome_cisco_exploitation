/* $Id: exec_clear_ip_igrp2.h,v 3.2.60.2 1996/08/12 16:04:01 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/exec_clear_ip_igrp2.h,v $
 *------------------------------------------------------------------
 * IP-IGRP2 clear commands
 *
 * December 1993, Dino Farinacci
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_clear_ip_igrp2.h,v $
 * Revision 3.2.60.2  1996/08/12  16:04:01  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.60.1  1996/04/02  05:36:54  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.2  1995/11/17  17:33:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:06:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:16:10  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:16:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear ip eigrp [<as #>] events
 *
 * OBJ(int,1) = AS #, or zero
 */
EOLNS	(clear_ip_igrp2_log_eol, ipigrp2_clear_log);
KEYWORD (clear_ip_igrp2_log, clear_ip_igrp2_log_eol, no_alt,
 	 "events", NULL /*"Clear IP-EIGRP event logs"*/, PRIV_OPR|PRIV_HIDDEN);

/******************************************************************
 * clear ip eigrp [<as #>] logging 
 *
 * OBJ(int,1) = AS #, or zero
 */
EOLNS	(clear_ip_igrp2_evlog_eol, ipigrp2_clear_event_logging);
KEYWORD (clear_ip_igrp2_evlog, clear_ip_igrp2_evlog_eol, clear_ip_igrp2_log,
 	 "logging", NULL /*"Stop IP-EIGRP event logging"*/,
	 PRIV_OPR|PRIV_HIDDEN);

/******************************************************************
 * clear ip eigrp [<as #>] neighbors [<ip-address> | <interface-unit>]
 *
 * OBJ(paddr,1) = Neighbor IP address
 * OBJ(idb,1)   = Interface of neighbors to get cleared
 * OBJ(int,1)   = AS number, or zero
 */
EOLNS (clear_ip_igrp2_nbr_eol, ipigrp2_clear_neighbors);
INTERFACE (clear_ip_igrp2_nbr_int, clear_ip_igrp2_nbr_eol,
 	   clear_ip_igrp2_nbr_eol, OBJ(idb,1), IFTYPE_ANYSUB);
IPADDR	(clear_ip_igrp2_nbr_addr, clear_ip_igrp2_nbr_eol, 
 	 clear_ip_igrp2_nbr_int, OBJ(paddr,1), "IP-EIGRP neighbor address");
KEYWORD (clear_ip_igrp2_nbr, clear_ip_igrp2_nbr_addr, clear_ip_igrp2_evlog,
 	 "neighbors", "Clear IP-EIGRP neighbors", PRIV_OPR);

NUMBER  (clear_ip_igrp2_asn, clear_ip_igrp2_nbr, clear_ip_igrp2_nbr,
 	 OBJ(int,1), 1, 65535, "Autonomous system number");
KEYWORD (clear_ip_igrp2, clear_ip_igrp2_asn, ALTERNATE,
	 "eigrp", "Clear IP-EIGRP", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	clear_ip_igrp2
