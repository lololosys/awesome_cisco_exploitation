/* $Id: exec_test_ip_routing.h,v 3.2.60.2 1996/08/12 16:04:15 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/exec_test_ip_routing.h,v $
 *------------------------------------------------------------------
 * IP routing test commands
 *
 * February 1994, Bob Albrightson
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: exec_test_ip_routing.h,v $
 * Revision 3.2.60.2  1996/08/12  16:04:15  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.60.1  1996/04/02  05:36:57  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.2  1995/11/17  17:33:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:16:23  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:27:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * test ip eigrp <as> error-percent <n> [<interface>]
 *
 * OBJ(int,2) = AS number
 * OBJ(int,3) = error percentage (0-100)
 * OBJ(idb,1) = interface
 */

EOLNS    (priv_test_ip_igrp2_error_eol, ipigrp2_error_cmd);
INTERFACE(priv_test_ip_igrp2_error_int, priv_test_ip_igrp2_error_eol,
 	  priv_test_ip_igrp2_error_eol, OBJ(idb,1), IFTYPE_ANYSUB);
NUMBER  (priv_test_ip_igrp2_error_pct, priv_test_ip_igrp2_error_int,
 	 no_alt, OBJ(int,3), 0, 100, "Error percentage");
KEYWORD (priv_test_ip_igrp2_error, priv_test_ip_igrp2_error_pct, no_alt,
 	 "error-percent", "Set induced error percentage", PRIV_OPR);

/******************************************************************
 * test ip eigrp <as> probe {<interface> | <IP address>} [stop]
 *
 * OBJ(int,2) = AS number
 * OBJ(idb,1) = interface
 * OBJ(paddr,1) = address
 * OBJ(int,3) = TRUE if "stop" selected
 */

EOLNS    (priv_test_ip_igrp2_probe_eol, ipigrp2_probe_command);
KEYWORD_ID(priv_test_ip_igrp2_probe_stop, priv_test_ip_igrp2_probe_eol,
 	   priv_test_ip_igrp2_probe_eol, OBJ(int,3), TRUE, "stop",
 	   "Stop the current probe", PRIV_OPR);
IPADDR  (priv_test_ip_igrp2_probe_addr, priv_test_ip_igrp2_probe_stop,
 	 no_alt, OBJ(paddr,1), "Peer to probe");
INTERFACE(priv_test_ip_igrp2_probe_int, priv_test_ip_igrp2_probe_stop,
 	  priv_test_ip_igrp2_probe_addr, OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD (priv_test_ip_igrp2_probe, priv_test_ip_igrp2_probe_int,
 	 priv_test_ip_igrp2_error, "probe", "Send probe packets", PRIV_OPR);
 
/******************************************************************
 * test ip eigrp <AS> ack|Nlocal|Nsuccessor|delete <ip-address> [<mask>]
 *
 * where N is a a test number, as in 2local or 1successor, etc.
 *
 * OBJ(int,1) test type
 * OBJ(int,2) AS number
 * OBJ(paddr,1), OBJ(paddr,2) ip address, mask
 */
EOLNS (priv_test_ip_igrp2_eol, ipigrp2_test_command);
IPADDR (priv_test_ip_igrp2_mask, priv_test_ip_igrp2_eol,
	priv_test_ip_igrp2_eol, OBJ(paddr, 2), "IP mask");
IPADDR (priv_test_ip_igrp2_dest, priv_test_ip_igrp2_mask,
	priv_test_ip_igrp2_eol, OBJ(paddr, 1), "IP address");
KEYWORD_ID (priv_test_ip_igrp2_del, priv_test_ip_igrp2_dest,
 	    priv_test_ip_igrp2_probe, OBJ(int,1), IGRP2_TEST_DELETE,
	    "delete", "Delete a phoney entry in the topology table", PRIV_OPR);
KEYWORD_ID (priv_test_ip_igrp2_succ2, priv_test_ip_igrp2_dest,
	    priv_test_ip_igrp2_del, OBJ(int,1), IGRP2_TEST_SUCC2,
	    "2successor", "Neighbor states 3 - 2", PRIV_OPR);
KEYWORD_ID (priv_test_ip_igrp2_succ1, priv_test_ip_igrp2_dest,
	    priv_test_ip_igrp2_succ2, OBJ(int,1), IGRP2_TEST_SUCC1,
	    "1successor", "Neighbor states 3", PRIV_OPR);
KEYWORD_ID (priv_test_ip_igrp2_local6, priv_test_ip_igrp2_dest,
	    priv_test_ip_igrp2_succ1, OBJ(int,1), IGRP2_TEST_LOCAL6,
	    "6local", "Neighbor states 1 - 2 - FC fail - 3", PRIV_OPR);
KEYWORD_ID (priv_test_ip_igrp2_local5, priv_test_ip_igrp2_dest,
	    priv_test_ip_igrp2_local6, OBJ(int,1), IGRP2_TEST_LOCAL5,
	    "5local", "Neighbor states 1 - 0 - FC fail - 1", PRIV_OPR);
KEYWORD_ID (priv_test_ip_igrp2_local4, priv_test_ip_igrp2_dest,
	    priv_test_ip_igrp2_local5, OBJ(int,1), IGRP2_TEST_LOCAL4,
	    "4local", "Neighbor states 1 - 0 - 2", PRIV_OPR);
KEYWORD_ID (priv_test_ip_igrp2_local3, priv_test_ip_igrp2_dest,
	    priv_test_ip_igrp2_local4, OBJ(int,1), IGRP2_TEST_LOCAL3,
	    "3local", "Neighbor states 1 - 0", PRIV_OPR);
KEYWORD_ID (priv_test_ip_igrp2_local2, priv_test_ip_igrp2_dest,
	    priv_test_ip_igrp2_local3, OBJ(int,1), IGRP2_TEST_LOCAL2,
	    "2local", "Neighbor states 1 - 2", PRIV_OPR);
KEYWORD_ID (priv_test_ip_igrp2_local1, priv_test_ip_igrp2_dest,
	    priv_test_ip_igrp2_local2, OBJ(int,1), IGRP2_TEST_LOCAL1,
	    "1local", "Neighbor states 1", PRIV_OPR);
KEYWORD_ID (priv_test_ip_igrp2_ack, priv_test_ip_igrp2_eol,
	    priv_test_ip_igrp2_local1, OBJ(int,1), IGRP2_TEST_ACK,
	    "ack", "Toggle EIGRP fast acking", PRIV_OPR);
NUMBER (priv_test_ip_igrp2_as, priv_test_ip_igrp2_ack, no_alt,
	OBJ(int,2), 1, 0xffff, "AS number");
KEYWORD (priv_test_ip_igrp2, priv_test_ip_igrp2_as, ALTERNATE,
	 "eigrp", "IP EIGRP test commands", PRIV_OPR);

#undef  ALTERNATE
#define ALTERNATE priv_test_ip_igrp2
