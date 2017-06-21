/* $Id: cfg_router_version.h,v 3.2 1995/11/17 17:32:38 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/cfg_router_version.h,v $
 *------------------------------------------------------------------
 * cfg_router_version.h
 * 
 * August 1995, Paul Traina
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_router_version.h,v $
 * Revision 3.2  1995/11/17  17:32:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:06:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:06:16  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/11  19:37:58  pst
 * Add placeholders
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

PARAMS_KEYONLY  (cfg_router_version, NONE,
                 "version", OBJ(int, 1), 1, 2,
                 iprouting_version_command, 0,
                 "Set routing protocol version",
                 "version", PRIV_CONF);

ASSERT  (router_version, cfg_router_version, ALTERNATE,
         (((pdbtype *) csb->protocol)->family == PDB_IP) &&
	 (((pdbtype *) csb->protocol)->proctype & PROC_RIP));
	 
#undef  ALTERNATE
#define ALTERNATE       router_version
