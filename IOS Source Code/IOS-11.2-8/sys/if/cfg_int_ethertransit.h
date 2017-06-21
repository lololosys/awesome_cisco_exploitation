/* $Id: cfg_int_ethertransit.h,v 3.2.60.1 1996/08/12 16:02:10 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_int_ethertransit.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ E T H E R T R A N S I T . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_ethertransit.h,v $
 * Revision 3.2.60.1  1996/08/12  16:02:10  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  09:26:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:08  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:04:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * ethernet-transit-oui { 90-compatible | standard | cisco }
 *
 */
EOLNS	(ci_ether_eol, tr_etheroui_command);

KEYWORD_ID(ci_ether_cisco, ci_ether_eol, no_alt,
	   OBJ(int,1), PARSER_ETHER_CISCO,
	   "cisco", "Cisco specific OUI", PRIV_CONF);
KEYWORD_ID(ci_ether_standard, ci_ether_eol, ci_ether_cisco,
	   OBJ(int,1), PARSER_ETHER_STANDARD,
	   "standard", "Standard OUI", PRIV_CONF);
KEYWORD_ID(ci_ether_early90, ci_ether_eol, ci_ether_standard,
	   OBJ(int,1), PARSER_ETHER_90COMPAT,
	   "90-compatible", "Cisco release 9.0 compatible", PRIV_CONF);

KEYWORD	(ci_ether, ci_ether_early90, NONE,
	 "ethernet-transit-oui", "Token-ring to Ethernet OUI handling",
	 PRIV_CONF);

TEST_BOOLEAN(ci_ether_enable, ci_ether, NONE, NONE, bridge_enable);
TEST_IDBSTATUS(ci_ether_test, ci_ether_enable, NONE, ALTERNATE, IDB_TR);

#undef	ALTERNATE
#define	ALTERNATE	ci_ether_test
