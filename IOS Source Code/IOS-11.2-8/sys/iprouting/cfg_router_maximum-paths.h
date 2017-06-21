/*$Id: cfg_router_maximum-paths.h,v 3.2 1995/11/17 17:32:11 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/iprouting/cfg_router_maximum-paths.h,v $
 *------------------------------------------------------------------
 *cfg_router_maximum-paths.h
 * 
 * <July> 1994 rchandra
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_router_maximum-paths.h,v $
 * Revision 3.2  1995/11/17  17:32:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:53  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:12:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
PARAMS_KEYONLY  (cfg_router_maxpaths, NONE,
                 "maximum-paths", OBJ(int, 1), 1, MAXROUTES,
                 maxpaths_command, 0,
                 "Forward packets over multiple paths",
                 "Number of paths", PRIV_CONF);

ASSERT  (router_maxpaths, cfg_router_maxpaths, ALTERNATE,
         (((pdbtype *) csb->protocol)->family == PDB_IP) ||
         ((((clns_pdbtype *) csb->protocol)->family == PDB_OSI) &&
          (((clns_pdbtype *) csb->protocol)->typeflag & ISO_ISISMASK)));

#undef  ALTERNATE
#define ALTERNATE       router_maxpaths
