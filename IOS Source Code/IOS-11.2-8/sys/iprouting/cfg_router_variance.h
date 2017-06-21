/* $Id: cfg_router_variance.h,v 3.2 1995/11/17 17:32:37 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/cfg_router_variance.h,v $
 *------------------------------------------------------------------
 * C F G _ R O U T E R _ V A R I A N C E . H
 * 
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_router_variance.h,v $
 * Revision 3.2  1995/11/17  17:32:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:06:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:16:09  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:13:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 *	variance <multiplier>
 *	no variance [<multiplier>]
 *
 * OBJ(int,1) = <multiplier>
 */
PARAMS_KEYONLY (router_variance_kw, NONE,
		"variance", OBJ(int,1), 1, 128,
		variance_command, 0,
		"Control load balancing variance",
		"Metric variance multiplier", PRIV_CONF);

ASSERT	(router_variance, router_variance_kw, ALTERNATE,
	 ((pdbtype *) csb->protocol)->family == PDB_IP &&
	 ((pdbtype *) csb->protocol)->proctype & (PROC_IGRP | PROC_IGRP2));

#undef	ALTERNATE
#define ALTERNATE	router_variance
