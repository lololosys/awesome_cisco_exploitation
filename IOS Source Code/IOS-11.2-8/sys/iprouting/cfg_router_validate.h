/* $Id: cfg_router_validate.h,v 3.2.60.1 1996/08/12 16:03:59 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_validate.h,v $
 *------------------------------------------------------------------
 * cfg_router_validate.h
 *
 * Nob to allow user to decide whether or not system should sanity check
 * source IP address of incoming routing updates.
 *
 * March 1994, Bruce Cole
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_router_validate.h,v $
 * Revision 3.2.60.1  1996/08/12  16:03:59  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:32:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:06:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:16:08  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:13:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLNS	(rtr_val_eol, iprouting_validate_command);
KEYWORD (rtr_val_kwd, rtr_val_eol, NONE,
	 "validate-update-source",
	 "Perform sanity checks against source address of routing updates",
	 PRIV_CONF);
ASSERT	(router_validate, rtr_val_kwd, ALTERNATE,
	 (((pdbtype *) csb->protocol)->family == PDB_IP &&
	  ((pdbtype *) csb->protocol)->proctype & (PROC_RIP|PROC_IGRP)));

#undef	ALTERNATE
#define	ALTERNATE	router_validate
