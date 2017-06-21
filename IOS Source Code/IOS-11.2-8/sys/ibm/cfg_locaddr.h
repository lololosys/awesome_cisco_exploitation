/* $Id: cfg_locaddr.h,v 3.2.60.1 1996/08/12 16:01:57 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ibm/cfg_locaddr.h,v $
 *------------------------------------------------------------------
 * C F G _ L O C A D D R . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_locaddr.h,v $
 * Revision 3.2.60.1  1996/08/12  16:01:57  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:22:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:51  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:11:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] locaddr-priority-list <num> <addr> {high|medium|normal|low}
 *						 [<dmac dm>] [<dsap ds>]
 * 
 */
EOLNS	(cfg_locaddr_eol, locaddr_pri_list_command);
pdecl (cfg_locaddr_nop);

HEXNUM(cfg_locaddr_sa, cfg_locaddr_nop, no_alt,
		OBJ(int, 6), "SDLC address");
KEYWORD_ID(cfg_locaddr_sdlc, cfg_locaddr_sa, cfg_locaddr_eol,
		OBJ(int, 5), TRUE,
		"sdlc", "SDLC address keyword", PRIV_CONF);
HEXNUM(cfg_locaddr_ssap, cfg_locaddr_nop, no_alt,
		OBJ(int, 10), "Source SAP address");
KEYWORD_ID(cfg_locaddr_ss, cfg_locaddr_ssap, cfg_locaddr_sdlc,
		OBJ(int,9), TRUE,
		"ssap", "Source SAP keyword", PRIV_CONF);
HEXNUM(cfg_locaddr_dsap, cfg_locaddr_nop, no_alt,
		OBJ(int, 8), "Destination SAP address");
KEYWORD_ID(cfg_locaddr_ds, cfg_locaddr_dsap, cfg_locaddr_ss,
		OBJ(int,7), TRUE,
		"dsap", "Destination SAP keyword", PRIV_CONF);
ENADDR(cfg_locaddr_smac, cfg_locaddr_nop, no_alt,
		OBJ(hwaddr, 2), "Source MAC address");
KEYWORD_ID(cfg_locaddr_sm, cfg_locaddr_smac, cfg_locaddr_ds,
		OBJ(int,6), TRUE,
		"smac", "Source MAC address keyword", PRIV_CONF);
ENADDR(cfg_locaddr_dmac, cfg_locaddr_nop, no_alt,
		OBJ(hwaddr, 1), "Destination MAC address");
KEYWORD_ID(cfg_locaddr_dm, cfg_locaddr_dmac, cfg_locaddr_sm,
		OBJ(int,4), TRUE,
		"dmac", "Destination MAC address keyword", PRIV_CONF);
NOP(cfg_locaddr_nop, cfg_locaddr_dm, no_alt);
KEYWORD_ID(cfg_locaddr_pri_low, cfg_locaddr_nop, no_alt,
	   OBJ(int,3), PRIORITY_LOW,
	   "low", "Low priority", PRIV_CONF );
KEYWORD_ID(cfg_locaddr_pri_normal, cfg_locaddr_nop, cfg_locaddr_pri_low,
	   OBJ(int,3), PRIORITY_NORMAL,
	   "normal", "Normal priority", PRIV_CONF );
KEYWORD_ID(cfg_locaddr_pri_medium, cfg_locaddr_nop, cfg_locaddr_pri_normal,
	   OBJ(int,3), PRIORITY_MEDIUM,
	   "medium", "Medium priority", PRIV_CONF );
KEYWORD_ID(cfg_locaddr_pri_high, cfg_locaddr_nop, cfg_locaddr_pri_medium,
	   OBJ(int,3), PRIORITY_HIGH,
	   "high", "High priority", PRIV_CONF );

HEXDIGIT (cfg_locaddr_addr, cfg_locaddr_pri_high, no_alt,
	 OBJ(int,2), 0x0, 0xff, "One-byte LU addr (in hex)" );
NUMBER(cfg_locaddr_num, cfg_locaddr_addr, no_alt,
	   OBJ(int,1), 1, 10, "LU address priority list");
NVGENNS	(cfg_locaddr_nvgen, cfg_locaddr_num, locaddr_pri_list_command);
KEYWORD	(cfg_locaddr, cfg_locaddr_nvgen, NONE,
	 "locaddr-priority-list", 
	 "Establish queueing priorities based on LU address", PRIV_CONF );

TEST_BOOLEAN(cfg_locaddr_test, cfg_locaddr, NONE, ALTERNATE, bridge_enable);


/***************************************************************
 * [no] sap-priority-list <list> {high|medium|normal|low} [<dsap ds>]
 *					[<ssap ss>] [<dmac dm>] [<smac sm>]
 * 
 */
EOLNS	(cfg_sappri_eol, sap_pri_list_command);
pdecl (cfg_sappri_nop);

ENADDR(cfg_sappri_smac, cfg_sappri_nop, no_alt,
		OBJ(hwaddr,2), "Destination MAC address");
KEYWORD_ID(cfg_sappri_sm, cfg_sappri_smac, cfg_sappri_eol,
		OBJ(int,6), TRUE,
		"smac", "Source MAC address keyword", PRIV_CONF);
ENADDR(cfg_sappri_dmac, cfg_sappri_nop, no_alt,
		OBJ(hwaddr,1), "Destination MAC address");
KEYWORD_ID(cfg_sappri_dm, cfg_sappri_dmac, cfg_sappri_sm,
		OBJ(int,5), TRUE,
		"dmac", "Destination MAC address keyword", PRIV_CONF);
HEXNUM(cfg_sappri_ssap, cfg_sappri_nop, no_alt,
		OBJ(int,4), "Source SAP address");
KEYWORD_ID(cfg_sappri_ss, cfg_sappri_ssap, cfg_sappri_dm,
		OBJ(int, 8), TRUE,
		"ssap", "Source SAP address keyword", PRIV_CONF);
HEXNUM(cfg_sappri_dsap, cfg_sappri_nop, no_alt,
		OBJ(int,3), "Destination SAP address");
KEYWORD_ID(cfg_sappri_ds, cfg_sappri_dsap, cfg_sappri_ss,
		OBJ(int, 7), TRUE,
		"dsap", "Destination SAP address keyword",PRIV_CONF);
NOP(cfg_sappri_nop, cfg_sappri_ds, no_alt);
KEYWORD_ID(cfg_sappri_pri_low, cfg_sappri_nop, no_alt,
	   OBJ(int,2), PRIORITY_LOW,
	   "low", "Low priority", PRIV_CONF );
KEYWORD_ID(cfg_sappri_pri_normal, cfg_sappri_nop, cfg_sappri_pri_low,
	   OBJ(int,2), PRIORITY_NORMAL,
	   "normal", "Normal priority", PRIV_CONF );
KEYWORD_ID(cfg_sappri_pri_medium, cfg_sappri_nop, cfg_sappri_pri_normal,
	   OBJ(int,2), PRIORITY_MEDIUM,
	   "medium", "Medium priority", PRIV_CONF );
KEYWORD_ID(cfg_sappri_pri_high, cfg_sappri_nop, cfg_sappri_pri_medium,
	   OBJ(int,2), PRIORITY_HIGH,
	   "high", "High priority", PRIV_CONF );


NUMBER(cfg_sappri_num, cfg_sappri_pri_high, no_alt,
	   OBJ(int,1), 1, 10, "SAP priority list");

NVGENNS(cfg_sappri_nvgen, cfg_sappri_num, sap_pri_list_command);

KEYWORD	(cfg_sappri, cfg_sappri_nvgen, NONE,
	 "sap-priority-list", 
	 "Establish queueing priorities based on SAP and/or MAC address(es)",
	 PRIV_CONF);

TEST_BOOLEAN(cfg_sappri_test, cfg_sappri, NONE, cfg_locaddr_test,
	     bridge_enable);

#undef	ALTERNATE
#define	ALTERNATE	cfg_sappri_test
