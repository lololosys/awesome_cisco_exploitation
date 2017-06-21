/* $Id: cfg_int_encaps_x25.h,v 3.3.62.1 1996/04/27 06:45:49 syiu Exp $
 * $Source: /release/112/cvs/Xsys/x25/cfg_int_encaps_x25.h,v $
 *------------------------------------------------------------------
 * X25 interface encapsulation commands
 *
 * November 1995, Robert Widmer
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_encaps_x25.h,v $
 * Revision 3.3.62.1  1996/04/27  06:45:49  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.3.20.1  1996/02/06  20:32:21  perryl
 * Branch: Dial1_branch
 *
 * Revision 3.3  1995/12/07  17:45:41  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.2  1995/11/27  16:24:08  widmer
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


LINK_EXIT(cfg_encap_x25_exit, no_alt);

/**********************************************************************
 * [no] encapsulation x25 { dce | dte } { ddn | bfe } { ietf }
 *
 *   and the backwards-compatable forms:
 *	[no] encapsulation x25dce
 *	[no] encapsulation ddnx25
 *	[no] encapsulation ddnx25dce
 *	[no] encapsulation bfex25
 */

/*
 * (old) [no] encapsulation bfex25
 */
SET	(config_encap_bfex25_set, cfg_encap_x25_exit, OBJ(int,2), ENCAPS_X25_BFE);
KEYWORD_ID(config_encap_bfex25_kwd, config_encap_bfex25_set, NONE,
	   OBJ(int,1), ET_X25, "bfex25", "",
	   PRIV_CONF | PRIV_HIDDEN | PRIV_NONVGEN);
TEST_IDBSTATUS(config_encap_bfex25, config_encap_bfex25_kwd,
	       NONE, NONE, IDB_SYNC);

/*
 * (old) [no] encapsulation { ddnx25 | ddnx25dce }
 */
SET	(config_encap_ddnx25dce_set, cfg_encap_x25_exit,
	 OBJ(int,2), ENCAPS_LAPB_X25_DCE | ENCAPS_X25_DDN);
KEYWORD_ID(config_encap_ddnx25dce_kwd, config_encap_ddnx25dce_set, NONE,
	   OBJ(int,1), ET_X25, "ddnx25-dce", "",
	   PRIV_CONF | PRIV_HIDDEN | PRIV_NONVGEN);
TEST_IDBSTATUS(config_encap_ddnx25dce, config_encap_ddnx25dce_kwd, NONE,
	       config_encap_bfex25, IDB_SYNC);

TEST_MULTIPLE_FUNCS(config_encap_ddnx25dce_test, config_encap_ddnx25dce,
		    config_encap_bfex25, NONE);

SET	(config_encap_ddnx25_set, cfg_encap_x25_exit,
	 OBJ(int,2), ENCAPS_X25_DDN);
KEYWORD_ID(config_encap_ddnx25_kwd, config_encap_ddnx25_set, NONE,
	   OBJ(int,1), ET_X25, "ddnx25", "",
	   PRIV_CONF | PRIV_HIDDEN | PRIV_NONVGEN);
TEST_IDBSTATUS(config_encap_ddnx25, config_encap_ddnx25_kwd, NONE,
	       config_encap_ddnx25dce_test, IDB_SYNC);

/*
 * (old) [no] encapsulation x25-dce
 */
SET	(config_encap_x25dce_set, cfg_encap_x25_exit,
	 OBJ(int,2), ENCAPS_LAPB_X25_DCE);
KEYWORD_ID(config_encap_x25dce_kwd, config_encap_x25dce_set, NONE,
	   OBJ(int,1), ET_X25, "x25-dce", "",
	   PRIV_CONF | PRIV_HIDDEN | PRIV_NONVGEN);
TEST_IDBSTATUS(config_encap_x25dce, config_encap_x25dce_kwd, NONE,
	       config_encap_ddnx25, IDB_SYNC);

TEST_MULTIPLE_FUNCS(config_encap_x25dce_test, config_encap_x25dce,
		    config_encap_ddnx25, NONE);

/*
 * [no] encapsulation x25 { dce | dte } { ddn | bfe } { ietf }
 */

KEYWORD_OR(config_encap_x25_ietf, cfg_encap_x25_exit, cfg_encap_x25_exit,
	   OBJ(int,2), ENCAPS_X25_IETF,
	   "ietf", "Default to IETF's RFC-1356 encapsulation", PRIV_CONF);
KEYWORD_OR(config_encap_x25_ddn, cfg_encap_x25_exit, config_encap_x25_ietf,
	   OBJ(int,2), ENCAPS_X25_DDN,
	   "ddn", "Defense Data Network attachment", PRIV_CONF);
KEYWORD_OR(config_encap_x25_bfe, cfg_encap_x25_exit, config_encap_x25_ddn,
	   OBJ(int,2), ENCAPS_X25_BFE,
	   "bfe", "Blacker Front End attachment", PRIV_CONF);
IFELSE	(config_encap_x25_dce_test, config_encap_x25_ddn,
	config_encap_x25_bfe, (GETOBJ(int,2) & ENCAPS_LAPB_X25_DCE));
TEST_IDBSTATUS(config_encap_x25_isdn, config_encap_x25_ietf,
	       config_encap_x25_dce_test, NONE, IDB_ISDN);
KEYWORD_OR(config_encap_x25_dce, config_encap_x25_isdn,
	   config_encap_x25_isdn, OBJ(int,2), ENCAPS_LAPB_X25_DCE,
	   "dce", "DCE operation", PRIV_CONF);
KEYWORD (config_encap_x25_dte, config_encap_x25_isdn,
	 config_encap_x25_dce, "dte", "DTE operation", PRIV_CONF);
SET     (config_encap_x25_init_opt, config_encap_x25_dte, OBJ(int,2), 0);
NVGENS	(config_encap_x25_nvgen, config_encap_x25_init_opt,
	 encapsulation_command, ET_X25);
ENCAPS	(config_encap_x25, config_encap_x25_nvgen, config_encap_x25dce_test,
	 OBJ(int,1), ET_X25,
	 "x25", "X.25",
	 (IDB_SYNC | IDB_ISDN | IDB_DIALER | IDB_VTEMPLATE));
TEST_IDBSTATUS(config_encap_x25_test, ALTERNATE, config_encap_x25, NONE,
	       IDB_HSSI);

#undef	ALTERNATE
#define	ALTERNATE	config_encap_x25_test
