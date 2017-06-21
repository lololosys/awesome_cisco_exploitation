/* $Id: exec_show_route-map.h,v 3.2.60.1 1996/08/12 16:04:13 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/exec_show_route-map.h,v $
 *------------------------------------------------------------------
 * exec_show_route-map.h
 *
 * Month Year, Your Name
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Command parsing chain for "show route-map".
 *------------------------------------------------------------------
 * $Log: exec_show_route-map.h,v $
 * Revision 3.2.60.1  1996/08/12  16:04:13  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:33:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:07:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:16:22  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:25:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 * show route-map [<map-name>]
 *
 * OBJ(string,1) = map name
 */
EOLNS (show_routemap_eols, show_route_map);
STRING  (show_routemap_string, show_routemap_eols, show_routemap_eols,
	 OBJ(string,1), "route-map name");
KEYWORD (show_routemap, show_routemap_string, ALTERNATE, 
	 "route-map", "route-map information", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_routemap

