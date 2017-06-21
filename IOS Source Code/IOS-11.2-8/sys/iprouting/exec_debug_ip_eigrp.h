/* $Id: exec_debug_ip_eigrp.h,v 3.2 1996/11/12 04:35:44 dkatz Exp $
 * $Source: /release/111/cvs/Xsys/iprouting/exec_debug_ip_eigrp.h,v $
 *------------------------------------------------------------------
 * IP EIGRP debug command chains
 *
 * November 1996, Dave Katz
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_ip_eigrp.h,v $
 * Revision 3.2  1996/11/12 04:35:44  dkatz
 * CSCdi76003:  EIGRP isnt modular
 * Clean up the tendrils reaching into EIGRP from the generic IP routing
 * code.
 *
 * Revision 3.1  1996/11/12 03:46:55  dkatz
 * Add placeholders.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug ip eigrp [<AS> [<ip-address> [<mask>]]]
 *
 * OBJ(int,1) AS number
 * OBJ(paddr,1), OBJ(paddr,2) ip address, mask
 */
EOLS	(debug_ip_igrp2_eol, iprouting_debug_command, DEBUG_IPIGRP2);
EOLS    (debug_ip_igrp2_target_eol, ipigrp2_debug_command,
	 DEBUG_IPIGRP2_AS_TARGET);
IPADDR (debug_ip_igrp2_mask, debug_ip_igrp2_target_eol,
	debug_ip_igrp2_target_eol, OBJ(paddr, 2), "IP mask");
IPADDR (debug_ip_igrp2_dest, debug_ip_igrp2_mask, no_alt,
	OBJ(paddr, 1), "IP address");
NUMBER (debug_ip_igrp2_as, debug_ip_igrp2_dest, debug_ip_igrp2_eol,
	OBJ(int,1), 1, 0xffff, "AS number");

/******************************************************************
 * debug ip eigrp neighbor <AS> <ip-address>
 *
 * OBJ(int,1) AS number
 * OBJ(paddr,1) ip address
 */
EOLS    (debug_ip_igrp2_nei_eol, ipigrp2_debug_command, 
	 DEBUG_IPIGRP2_NEI_TARGET);
IPADDR (debug_ip_igrp2_nei_ad, debug_ip_igrp2_nei_eol, no_alt,
	OBJ(paddr, 1), "IP address");
NUMBER (debug_ip_igrp2_nei_as, debug_ip_igrp2_nei_ad, no_alt,
	OBJ(int,1), 1, 0xffff, "AS number");
KEYWORD	 (debug_ip_igrp2_nei, debug_ip_igrp2_nei_as, debug_ip_igrp2_as,
	  "neighbor", "IP-EIGRP neighbor debugging", PRIV_OPR);

/******************************************************************
 * debug ip eigrp summary
 *
 * OBJ(int,1) AS number
 * OBJ(paddr,1), OBJ(paddr,2) ip address, mask
 */
EOLS    (debug_ip_igrp2_sum_eol, iprouting_debug_command,
	 DEBUG_IPIGRP2_SUMMARY);
KEYWORD	 (debug_ip_igrp2_sum, debug_ip_igrp2_sum_eol, debug_ip_igrp2_nei,
	  "summary", "IP-EIGRP summary route processing", PRIV_OPR);

/******************************************************************
 * debug ip eigrp notifications
 *
 * OBJ(int,1) AS number
 * OBJ(paddr,1), OBJ(paddr,2) ip address, mask
 */
EOLS    (debug_ip_igrp2_not_eol, iprouting_debug_command,
	 DEBUG_IPIGRP2_NOTIFICATIONS);
KEYWORD	 (debug_ip_igrp2_not, debug_ip_igrp2_not_eol, debug_ip_igrp2_sum,
	  "notifications", "IP-EIGRP event notifications", PRIV_OPR);
KEYWORD	 (debug_ip_igrp2, debug_ip_igrp2_not, no_alt,
	  "eigrp", "IP-EIGRP information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_ip_igrp2
