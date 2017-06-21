/* $Id: exec_test_ip_eigrp.h,v 3.2 1996/11/12 04:35:49 dkatz Exp $
 * $Source: /release/111/cvs/Xsys/iprouting/exec_test_ip_eigrp.h,v $
 *------------------------------------------------------------------
 * IP EIGRP test commands
 *
 * November 1996, Dave Katz
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_test_ip_eigrp.h,v $
 * Revision 3.2  1996/11/12 04:35:49  dkatz
 * CSCdi76003:  EIGRP isnt modular
 * Clean up the tendrils reaching into EIGRP from the generic IP routing
 * code.
 *
 * Revision 3.1  1996/11/12 03:46:56  dkatz
 * Add placeholders.
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

EOLS    (priv_test_ip_igrp2_error_eol, ipigrp2_error_cmd, 0);
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

EOLS    (priv_test_ip_igrp2_probe_eol, ipigrp2_probe_command, 0);
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
EOLS (priv_test_ip_igrp2_eol, ipigrp2_test_command, 0);
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
