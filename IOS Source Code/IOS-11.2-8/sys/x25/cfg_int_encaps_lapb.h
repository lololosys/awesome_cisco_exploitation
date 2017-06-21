/* $Id: cfg_int_encaps_lapb.h,v 3.3 1995/12/07 17:45:41 widmer Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/x25/cfg_int_encaps_lapb.h,v $
 *------------------------------------------------------------------
 * LAPB interface encapsulation commands
 *
 * November 1995, Robert Widmer
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_encaps_lapb.h,v $
 * Revision 3.3  1995/12/07  17:45:41  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.2  1995/11/27  16:24:07  widmer
 * CSCdi33598:  WAN commands (eg X.25) in parser even when Subsystem not
 * included
 * Create parser link for interface encapsulation types
 *
 * Revision 3.1  1995/11/22  22:21:20  widmer
 * CSCdi33598:  WAN commands (eg X.25) in parser even when Subsystem not
 * included
 * Initial version
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_EXIT(cfg_encap_lapb_exit, no_alt);

/**********************************************************************
 * [no] encapsulation lapb { dce | dte } { multi | <protocol> }
 *
 *   and the backwards-compatable forms:
 *	[no] encapsulation lapb-dce
 *	[no] encapsulation multi-lapb
 *	[no] encapsulation multi-lapb-dce
 */
SET	(config_encap_mlapbdce_set_proto, cfg_encap_lapb_exit,
	 OBJ(int,3), LINK_ILLEGAL);
SET	(config_encap_mlapbdce_set, config_encap_mlapbdce_set_proto,
	 OBJ(int,2), ENCAPS_LAPB_X25_DCE);
KEYWORD_ID(config_encap_mlapbdce_kwd, config_encap_mlapbdce_set, NONE,
	   OBJ(int,1), ET_LAPB, "multi-lapb-dce", "",
	   PRIV_CONF | PRIV_HIDDEN | PRIV_NONVGEN);
TEST_IDBSTATUS(config_encap_mlapbdce, config_encap_mlapbdce_kwd,
	       NONE, NONE, IDB_SYNC);

TEST_MULTIPLE_FUNCS(config_encap_mlapbdce_test, config_encap_mlapbdce,
		    ALTERNATE, NONE);

SET	(config_encap_mlapb_set_proto, cfg_encap_lapb_exit,
	 OBJ(int,3), LINK_ILLEGAL);
SET	(config_encap_mlapb_set, config_encap_mlapb_set_proto,
	 OBJ(int,2), 0);
KEYWORD_ID(config_encap_mlapb_kwd, config_encap_mlapb_set, NONE,
	   OBJ(int,1), ET_LAPB, "multi-lapb", "",
	   PRIV_CONF | PRIV_HIDDEN | PRIV_NONVGEN);
TEST_IDBSTATUS(config_encap_mlapb, config_encap_mlapb_kwd, NONE,
	       config_encap_mlapbdce_test, IDB_SYNC);

/*
 * [no] encapsulation { lapb | lapb-dce }
 */
SET	(config_encap_lapbdce_set_proto, cfg_encap_lapb_exit,
	 OBJ(int,3), LINK_IP);
SET	(config_encap_lapbdce_set, config_encap_lapbdce_set_proto,
	 OBJ(int,2), ENCAPS_LAPB_X25_DCE);
KEYWORD_ID(config_encap_lapbdce_kwd, config_encap_lapbdce_set, NONE,
	   OBJ(int,1), ET_LAPB, "lapb-dce", "",
	   PRIV_CONF | PRIV_HIDDEN | PRIV_NONVGEN);
TEST_IDBSTATUS(config_encap_lapbdce, config_encap_lapbdce_kwd, NONE,
	       config_encap_mlapb, IDB_SYNC);

TEST_MULTIPLE_FUNCS(config_encap_lapbdce_test, config_encap_lapbdce,
		    config_encap_mlapb, NONE);

/*
 * [no] encapsulation lapb { dce | dte } { multi | <protocol> }
 */

PROTONAME(config_encap_lapb_proto, cfg_encap_lapb_exit, cfg_encap_lapb_exit,
	  OBJ(int,3),
	  PMATCH_DGRAM PMATCH_EXCLUDE PMATCH_BRIDGE PMATCH_ARP PMATCH_AARP);
KEYWORD_ID(config_encap_lapb_multi, cfg_encap_lapb_exit, config_encap_lapb_proto,
	   OBJ(int,3), LINK_ILLEGAL,
	   "multi", "Multiprotocol operation", PRIV_CONF);
KEYWORD_OR(config_encap_lapb_dce, config_encap_lapb_multi,
	   config_encap_lapb_multi, OBJ(int,2), ENCAPS_LAPB_X25_DCE,
	   "dce", "DCE operation", PRIV_CONF);
KEYWORD (config_encap_lapb_dte, config_encap_lapb_multi,
	 config_encap_lapb_dce, "dte", "DTE operation", PRIV_CONF);
SET     (config_encap_lapb_init_proto, config_encap_lapb_dte,
	 OBJ(int,3), LINK_IP);
SET     (config_encap_lapb_init_opt, config_encap_lapb_init_proto,
	 OBJ(int,2), 0);
NVGENS	(config_encap_lapb_nvgen, config_encap_lapb_init_opt,
	 encapsulation_command, ET_LAPB);

ENCAPS	(config_encap_lapb, config_encap_lapb_nvgen, config_encap_lapbdce_test,
	 OBJ(int,1), ET_LAPB,
	 "lapb", "LAPB (X.25 Level 2)",
         (IDB_SYNC | IDB_ISDN | IDB_DIALER));

TEST_IDBSTATUS(config_encap_lapb_test, ALTERNATE, config_encap_lapb, NONE,
	       IDB_HSSI);

#undef	ALTERNATE
#define	ALTERNATE	config_encap_lapb_test
