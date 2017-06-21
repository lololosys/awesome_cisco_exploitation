/* $Id: exec_test_novigrp.h,v 3.1.2.3 1996/08/16 00:14:34 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/exec_test_novigrp.h,v $
 *------------------------------------------------------------------
 * IPX EIGRP test commands.
 *
 * April, 1996, Steven Luong
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_test_novigrp.h,v $
 * Revision 3.1.2.3  1996/08/16  00:14:34  hampton
 * Change parser comments to reflect ipx command usage.  [CSCdi66208]
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/08/12  16:10:08  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.2.1  1996/05/03  02:16:43  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.1  1996/04/13  01:24:46  sluong
 * Add placeholders
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * test ipx eigrp <AS> ack|Nlocal|Nsuccessor|delete <novell-network>
 *
 * where N is a a test number, as in 2local or 1successor, etc.
 *
 * OBJ(int,1) test type
 * OBJ(int,2) AS number
 * OBJ(int,3) ipx network number
 * OBJ(int,4) network mask
 */
EOLNS (priv_test_nov_igrp2_eol, novigrp_test_command);
HEXDIGIT (priv_test_nov_igrp2_mask, priv_test_nov_igrp2_eol,
	  priv_test_nov_igrp2_eol, OBJ(int,4), 0x80000000, 0xffffffff, 
	  "IPX network mask");
HEXDIGIT (priv_test_nov_igrp2_dest, priv_test_nov_igrp2_mask, no_alt,
	  OBJ(int,3), NOV_MIN_NET, NOV_RIP_DEFAULT_ROUTE, "IPX address");
KEYWORD_ID (priv_test_nov_igrp2_del, priv_test_nov_igrp2_dest, no_alt,
	    OBJ(int,1), IGRP2_TEST_DELETE,
	    "delete", "Delete a phoney entry in the topology table", PRIV_OPR);
KEYWORD_ID (priv_test_nov_igrp2_succ2, priv_test_nov_igrp2_dest,
	    priv_test_nov_igrp2_del, OBJ(int,1), IGRP2_TEST_SUCC2,
	    "2successor", "Neighbor states 3 - 2", PRIV_OPR);
KEYWORD_ID (priv_test_nov_igrp2_succ1, priv_test_nov_igrp2_dest,
	    priv_test_nov_igrp2_succ2, OBJ(int,1), IGRP2_TEST_SUCC1,
	    "1successor", "Neighbor states 3", PRIV_OPR);
KEYWORD_ID (priv_test_nov_igrp2_local6, priv_test_nov_igrp2_dest,
	    priv_test_nov_igrp2_succ1, OBJ(int,1), IGRP2_TEST_LOCAL6,
	    "6local", "Neighbor states 1 - 2 - FC fail - 3", PRIV_OPR);
KEYWORD_ID (priv_test_nov_igrp2_local5, priv_test_nov_igrp2_dest,
	    priv_test_nov_igrp2_local6, OBJ(int,1), IGRP2_TEST_LOCAL5,
	    "5local", "Neighbor states 1 - 0 - FC fail - 1", PRIV_OPR);
KEYWORD_ID (priv_test_nov_igrp2_local4, priv_test_nov_igrp2_dest,
	    priv_test_nov_igrp2_local5, OBJ(int,1), IGRP2_TEST_LOCAL4,
	    "4local", "Neighbor states 1 - 0 - 2", PRIV_OPR);
KEYWORD_ID (priv_test_nov_igrp2_local3, priv_test_nov_igrp2_dest,
	    priv_test_nov_igrp2_local4, OBJ(int,1), IGRP2_TEST_LOCAL3,
	    "3local", "Neighbor states 1 - 0", PRIV_OPR);
KEYWORD_ID (priv_test_nov_igrp2_local2, priv_test_nov_igrp2_dest,
	    priv_test_nov_igrp2_local3, OBJ(int,1), IGRP2_TEST_LOCAL2,
	    "2local", "Neighbor states 1 - 2", PRIV_OPR);
KEYWORD_ID (priv_test_nov_igrp2_local1, priv_test_nov_igrp2_dest,
	    priv_test_nov_igrp2_local2, OBJ(int,1), IGRP2_TEST_LOCAL1,
	    "1local", "Neighbor states 1", PRIV_OPR);
KEYWORD_ID (priv_test_nov_igrp2_ack, priv_test_nov_igrp2_eol,
	    priv_test_nov_igrp2_local1, OBJ(int,1), IGRP2_TEST_ACK,
	    "ack", "Toggle EIGRP fast acking", PRIV_OPR);
NUMBER (priv_test_nov_igrp2_as, priv_test_nov_igrp2_ack, no_alt,
	OBJ(int,2), 1, 0xffff, "AS number");
KEYWORD (priv_test_nov_igrp2, priv_test_nov_igrp2_as, ALTERNATE,
	 "eigrp", "IPX EIGRP test commands", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	priv_test_nov_igrp2
