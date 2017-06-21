/* $Id: cfg_router_passive-interface.h,v 3.2.60.1 1996/08/12 16:03:49 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_passive-interface.h,v $
 * ------------------------------------------------------------------
 * C F G _ R O U T E R _ P A S S I V E _ I N T . H
 * 
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_router_passive-interface.h,v $
 * Revision 3.2.60.1  1996/08/12  16:03:49  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:32:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:16:00  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:13:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 *	[no] passive-interface <interface>
 *
 * OBJ(idb,1) = <interface>
 */
EOLNS	(router_passint_eol, passive_interface_command);
INTERFACE(router_passint_int, router_passint_eol, no_alt, OBJ(idb,1), 
	  IFTYPE_ANYSUB);
NVGENNS	(router_passint_nvgen, router_passint_int,
	 passive_interface_command);

KEYWORD	(router_passint_kw, router_passint_nvgen, NONE,
	 "passive-interface", "Suppress routing updates on an interface", 
	 PRIV_CONF);

ASSERT	(router_passint, router_passint_kw, ALTERNATE,
	 (((pdbtype *) csb->protocol)->family == PDB_IP &&
	  ((pdbtype *) csb->protocol)->proctype != PROC_BGP &&
	  ((pdbtype *) csb->protocol)->proctype != PROC_EGP) ||
	 ((clns_pdbtype *) csb->protocol)->typeflag & ISO_ISISMASK);

#undef	ALTERNATE
#define	ALTERNATE	router_passint
