/* $Id: cfg_router_table-map.h,v 3.2.60.1 1996/08/12 16:03:53 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_router_table-map.h,v $
 *------------------------------------------------------------------
 * C F G _ R O U T E R _ T A B L E - M A P . H
 * 
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_router_table-map.h,v $
 * Revision 3.2.60.1  1996/08/12  16:03:53  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:32:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:06:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:16:03  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:13:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*********************************************************
 * table-prefix <string>
 * no table-prefix
 *
 * OBJ(string,1) = prefix name
 */

EOLNS	(rtr_table_map_eol, table_map_command);

STRING(rtr_table_map_string, rtr_table_map_eol, no_alt,
	OBJ(string,1), "route-map name");

NOPREFIX(rtr_table_map_noprefix, rtr_table_map_string, rtr_table_map_eol);
NVGENNS(rtr_table_map_nvgen, rtr_table_map_noprefix, table_map_command);

KEYWORD(rtr_table_map_kw, rtr_table_map_nvgen, NONE,
	 "table-map", "Map external entry attributes into routing table", 
	 PRIV_CONF);

ASSERT	(router_table_map, rtr_table_map_kw, ALTERNATE,
	 ((pdbtype *) csb->protocol)->family == PDB_IP &&
	 ((pdbtype *) csb->protocol)->proctype == PROC_BGP);

#undef	ALTERNATE
#define ALTERNATE router_table_map
