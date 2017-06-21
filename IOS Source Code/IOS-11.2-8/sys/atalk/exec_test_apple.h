/* $Id: exec_test_apple.h,v 3.1.40.1 1996/08/12 16:00:28 widmer Exp $
 * $Source: /release/112/cvs/Xsys/atalk/exec_test_apple.h,v $
 *------------------------------------------------------------------
 * Appletalk test commands
 *
 * February 1994, Bob Albrightson
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Longer description here.
 *------------------------------------------------------------------
 * $Log: exec_test_apple.h,v $
 * Revision 3.1.40.1  1996/08/12  16:00:28  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1  1996/02/01  04:57:07  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1996/01/22  06:57:54  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  18:47:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:53:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:20:25  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:27:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

enum {
    ATALK_TEST_MIN,
    ATALK_TEST_ARP,
    ATALK_TEST_EIGRP,
    ATALK_TEST_NBP,
    ATALK_TEST_END,
    ATALK_TEST_MAX
};

/**************************************************
 *
 * Exit AppleTalk test mode.
 */
EOLS	(priv_test_apple_end_eol, atalk_test_command, ATALK_TEST_END);
KEYWORD	(priv_test_apple_end, priv_test_apple_end_eol, no_alt,
	 "end", "Exit AppleTalk test mode", PRIV_MIN);

/******************************************************************
 * test apple nbp { lookup <object>:<type>@<zone> |
 *                  confirm
 *                }
 */
EOLS    (priv_test_apple_nbp_eol, atalk_test_command, ATALK_TEST_NBP);

KEYWORD_ID(priv_test_apple_nbp_poll, priv_test_apple_nbp_eol, no_alt,
	   OBJ(int, 1), ATNBP_TEST_POLL,
	   "poll", "Poll all devices in internet", PRIV_OPR);

DECIMAL (priv_test_apple_nbp_parms_intv, priv_test_apple_nbp_eol, no_alt,
	 OBJ(int, 4), 1, 60, "Retransmission interval");
DECIMAL (priv_test_apple_nbp_parms_rep, priv_test_apple_nbp_parms_intv, no_alt,
	 OBJ(int, 3), 1, 1000, "Maximum replies");
DECIMAL (priv_test_apple_nbp_parms_req, priv_test_apple_nbp_parms_rep, no_alt,
	 OBJ(int, 2), 1, 1000, "Maximum retransmissions");
KEYWORD_ID(priv_test_apple_nbp_parms, priv_test_apple_nbp_parms_req,
	   priv_test_apple_nbp_poll,
	   OBJ(int, 1), ATNBP_TEST_PARAMETERS,
	   "parameters", "Set NBP timeouts", PRIV_OPR);

STRING  (priv_test_apple_nbp_confirm_nve, priv_test_apple_nbp_eol, no_alt,
	 OBJ(string, 2), "<object>:<type>@<zone>");
STRING  (priv_test_apple_nbp_confirm_addr, priv_test_apple_nbp_confirm_nve, no_alt,
	 OBJ(string, 1), "<net>.<node>[:<skt>]");
KEYWORD_ID (priv_test_apple_nbp_confirm, priv_test_apple_nbp_confirm_addr,
	    priv_test_apple_nbp_parms,
	    OBJ(int, 1), ATNBP_TEST_CONFIRM,
	    "confirm", "Confirm NBP", PRIV_OPR);

TEXT    (priv_test_apple_nbp_lkup_tuple, priv_test_apple_nbp_eol, no_alt,
	 OBJ(string, 1), "<object>:<type>@<zone>");
KEYWORD_ID (priv_test_apple_nbp_lkup, priv_test_apple_nbp_lkup_tuple,
	    priv_test_apple_nbp_confirm,
	    OBJ(int, 1), ATNBP_TEST_LOOKUP,
	    "lookup", "Send NBP Lookup", PRIV_OPR);

KEYWORD (priv_test_apple_nbp, priv_test_apple_nbp_lkup, priv_test_apple_end,
	 "nbp", "AppleTalk NBP test commands", PRIV_OPR);

/******************************************************************
 * test appletalk arp <interface> <ddp address> <mac-address>
 *
 * Inserts an AARP entry with the specified DDP and MAC address into
 * the AppleTalk ARP table. Can be removed by "clear apple arp" just
 * as a real entry could be.
 *
 * OBJ(idb,1)		interface
 * OBJ(paddr,1)		appletalk address
 * OBJ(hwaddr,1)	hardware address
 */
EOLS	(priv_test_apple_arp_eol, atalk_test_command, ATALK_TEST_ARP);
ENADDR	(priv_test_apple_arp_macaddr, priv_test_apple_arp_eol,
	 no_alt, OBJ(hwaddr,1), "Apple ARP MAC address");

ATADDR	(priv_test_apple_arp_ddp, priv_test_apple_arp_macaddr,
	 no_alt, OBJ(paddr,1), "Appletalk address of AARP entry");

INTERFACE (priv_test_apple_arp_idb, priv_test_apple_arp_ddp,
	   no_alt, OBJ(idb,1), IFTYPE_ANYSUB);

KEYWORD (priv_test_apple_arp, priv_test_apple_arp_idb, priv_test_apple_nbp,
	 "arp", "APPLETALK ARP test commands", PRIV_OPR);


/******************************************************************
 * test appletalk eigrp ack|Nlocal|Nsuccessor|delete <cable-range>
 *
 * where N is a a test number, as in 2local or 1successor, etc.
 *
 * OBJ(int,1) test type
 * OBJ(int,2), OBJ(int,3) appletlak cable range
 */
EOLS (priv_test_apple_igrp2_eol, atalk_test_command, ATALK_TEST_EIGRP);
CRANGE (priv_test_apple_igrp2_dest, priv_test_apple_igrp2_eol, no_alt,
	OBJ(int,2), OBJ(int,3));
KEYWORD_ID (priv_test_apple_igrp2_del, priv_test_apple_igrp2_dest, no_alt,
	    OBJ(int,1), IGRP2_TEST_DELETE,
	    "delete", "Delete a phoney entry in the topology table", PRIV_OPR);
KEYWORD_ID (priv_test_apple_igrp2_succ2, priv_test_apple_igrp2_dest,
	    priv_test_apple_igrp2_del, OBJ(int,1), IGRP2_TEST_SUCC2,
	    "2successor", "Neighbor states 3 - 2", PRIV_OPR);
KEYWORD_ID (priv_test_apple_igrp2_succ1, priv_test_apple_igrp2_dest,
	    priv_test_apple_igrp2_succ2, OBJ(int,1), IGRP2_TEST_SUCC1,
	    "1successor", "Neighbor states 3", PRIV_OPR);
KEYWORD_ID (priv_test_apple_igrp2_local6, priv_test_apple_igrp2_dest,
	    priv_test_apple_igrp2_succ1, OBJ(int,1), IGRP2_TEST_LOCAL6,
	    "6local", "Neighbor states 1 - 2 - FC fail - 3", PRIV_OPR);
KEYWORD_ID (priv_test_apple_igrp2_local5, priv_test_apple_igrp2_dest,
	    priv_test_apple_igrp2_local6, OBJ(int,1), IGRP2_TEST_LOCAL5,
	    "5local", "Neighbor states 1 - 0 - FC fail - 1", PRIV_OPR);
KEYWORD_ID (priv_test_apple_igrp2_local4, priv_test_apple_igrp2_dest,
	    priv_test_apple_igrp2_local5, OBJ(int,1), IGRP2_TEST_LOCAL4,
	    "4local", "Neighbor states 1 - 0 - 2", PRIV_OPR);
KEYWORD_ID (priv_test_apple_igrp2_local3, priv_test_apple_igrp2_dest,
	    priv_test_apple_igrp2_local4, OBJ(int,1), IGRP2_TEST_LOCAL3,
	    "3local", "Neighbor states 1 - 0", PRIV_OPR);
KEYWORD_ID (priv_test_apple_igrp2_local2, priv_test_apple_igrp2_dest,
	    priv_test_apple_igrp2_local3, OBJ(int,1), IGRP2_TEST_LOCAL2,
	    "2local", "Neighbor states 1 - 2", PRIV_OPR);
KEYWORD_ID (priv_test_apple_igrp2_local1, priv_test_apple_igrp2_dest,
	    priv_test_apple_igrp2_local2, OBJ(int,1), IGRP2_TEST_LOCAL1,
	    "1local", "Neighbor states 1", PRIV_OPR);
KEYWORD_ID (priv_test_apple_igrp2_ack, priv_test_apple_igrp2_eol,
 	    priv_test_apple_igrp2_local1, OBJ(int,1), IGRP2_TEST_ACK,
 	    "ack", "Toggle EIGRP fast acking", PRIV_OPR);

KEYWORD (priv_test_apple_igrp2, priv_test_apple_igrp2_ack,
	 priv_test_apple_arp,
	 "eigrp", "APPLETALK EIGRP test commands", PRIV_OPR);


/*
 * Top level for AppleTalk test code. This is where the actual test code
 * forces re-entry into the parse chain when coming back from a sub-node.
 */
NOP	(top_test_atalk, priv_test_apple_igrp2, NONE);

/*
 * Test AppleTalk. Apple AppleTalk tests will be grouped under this command.
 */
EOLNS	(priv_test_apple_eol, atalk_test);
KEYWORD (priv_test_apple, priv_test_apple_eol, ALTERNATE,
	 "appletalk", "APPLETALK diagnostic code", PRIV_OPR);

#undef  ALTERNATE
#define ALTERNATE priv_test_apple
