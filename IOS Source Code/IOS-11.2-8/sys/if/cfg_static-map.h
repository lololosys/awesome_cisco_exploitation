/* $Id: cfg_static-map.h,v 3.3 1995/12/07 17:45:01 widmer Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/if/cfg_static-map.h,v $
 *------------------------------------------------------------------
 * Static maps configuration commands
 *
 * October 1993, Lionel Bustini
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_static-map.h,v $
 * Revision 3.3  1995/12/07  17:45:01  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.2  1995/11/17  09:26:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:15  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:14:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * 
 * OBJ(int,1) = <protocol>
 * OBJ(paddr,1) = <protocol address>
 */
LINK_TRANS(cfg_static_map_here, no_alt);

NSAPADDR(cfg_smap_clns_addr, cfg_static_map_here, no_alt,
         OBJ(paddr,1), "CLNS address to map");
TESTVAR	(cfg_smap_proto_test, cfg_smap_clns_addr, cfg_static_map_here,
	 NONE, NONE, NONE, OBJ(int,1), LINK_CLNS);
PROTOADDR(cfg_smap_proto, cfg_smap_proto_test, ALTERNATE,
          OBJ(int,1), OBJ(paddr,1), PMATCH_ALL);

#undef  ALTERNATE
#define ALTERNATE       cfg_smap_proto
