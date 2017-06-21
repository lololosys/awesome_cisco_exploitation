/* $Id: cfg_router_synchronization.h,v 3.2.60.1 1996/08/12 16:03:52 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_synchronization.h,v $
 *------------------------------------------------------------------
 * cfg_router_synchronization.h
 * 
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_router_synchronization.h,v $
 * Revision 3.2.60.1  1996/08/12  16:03:52  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:32:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:06:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:16:02  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:13:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 *      [no] synchronization
 *
 */
EOLNS	(router_synchronization_eol, synchronization_command);
KEYWORD	(router_synchronization_kw, router_synchronization_eol, NONE,
	 "synchronization", "Perform IGP synchronization", PRIV_CONF);
ASSERT (router_synchronization, router_synchronization_kw, ALTERNATE,
	((pdbtype *) csb->protocol)->family == PDB_IP &&
	((pdbtype *) csb->protocol)->proctype == PROC_BGP);

#undef	ALTERNATE
#define ALTERNATE	router_synchronization
