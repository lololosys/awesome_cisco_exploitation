/* $Id: cfg_router_is-type.h,v 3.2.60.1 1996/08/12 16:00:40 widmer Exp $
 * $Source: /release/112/cvs/Xsys/clns/cfg_router_is-type.h,v $
 *------------------------------------------------------------------
 * C F G _ R O U T E R _ I S - T Y P E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_router_is-type.h,v $
 * Revision 3.2.60.1  1996/08/12  16:00:40  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  08:51:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:07:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:58  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:12:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * is-type { level-1 | level-2-only | level-1-2 }
 * no is-type
 */

EOLNS	(router_istype_eol, clns_istype_cmd);

KEYWORD_ID(router_istype_level12_kwd, router_istype_eol, NONE,
	   OBJ(int,10), ISIS_CIRCUIT_L1L2,
	   "level-1-2", "Act as both a station router and an area router", 
	   PRIV_CONF);
TESTVAR	(router_istype_level12, router_istype_level12_kwd, no_alt,
	 NONE, NONE, NONE, OBJ(int,10), 0);
KEYWORD_ID(router_istype_level2, router_istype_eol, router_istype_level12,
	   OBJ(int,10), ISIS_CIRCUIT_L2,
	   "level-2-only", "Act as an area router only", PRIV_CONF);
KEYWORD_ID(router_istype_level1, router_istype_eol, router_istype_level2,
	   OBJ(int,10), ISIS_CIRCUIT_L1,
	   "level-1", "Act as a station router only", PRIV_CONF);
NOPREFIX(router_istype_noprefix, router_istype_level1, router_istype_eol);

KEYWORD	(router_istype_kw, router_istype_noprefix, NONE,
	 "is-type", "IS Level for this routing process (OSI only)", 
	 PRIV_CONF);

ASSERT	(router_istype, router_istype_kw, ALTERNATE,
	 ((clns_pdbtype *) csb->protocol)->family == PDB_OSI &&
	 ((clns_pdbtype *) csb->protocol)->typeflag & ISO_ISISMASK);

#undef	ALTERNATE
#define	ALTERNATE	router_istype
