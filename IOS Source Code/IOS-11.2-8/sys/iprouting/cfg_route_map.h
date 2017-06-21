/* $Id: cfg_route_map.h,v 3.2.60.1 1996/08/12 16:03:34 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_route_map.h,v $
 *------------------------------------------------------------------
 * cfg_route_map.h
 *
 * Enter route-map subcommand mode
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_route_map.h,v $
 * Revision 3.2.60.1  1996/08/12  16:03:34  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:31:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:43  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:12:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * [no] route-map <map-tag> [permit|deny] [<seq-num>]
 *
 * OBJ(string,1) == route_map tag.
 * OBJ(int,1)    == TRUE for "permit", FALSE for "deny", default it TRUE.
 * OBJ(int,2)    == sequence number (< 0 means non specify)
 */
EOLNS (config_routemap_eols, route_map_command);
NUMBER (config_routemap_setseq, config_routemap_eols, config_routemap_eols,
	OBJ(int,2), 0, 0xffff, 
	"Sequence to insert to/delete from existing route-map entry");
KEYWORD_ID (config_routemap_deny, config_routemap_setseq, 
	    config_routemap_setseq, OBJ(int,1), FALSE, "deny", 
	    "Route map denies set operations", PRIV_CONF);
KEYWORD_ID (config_routemap_permit, config_routemap_setseq, 
	    config_routemap_deny, OBJ(int,1), TRUE, "permit", 
	    "Route map permits set operations", PRIV_CONF);
SET (config_routemap_setnoseq, config_routemap_permit, OBJ(int,2), -1);
SET (config_routemap_setpermit, config_routemap_setnoseq, OBJ(int,1), TRUE);
STRING (config_routemap_tag, config_routemap_setpermit, no_alt, OBJ(string,1),
	"Route map tag");
NVGENNS (config_routemap_nvgen, config_routemap_tag, route_map_nvgen);

KEYWORD_ID(config_routemap, config_routemap_nvgen, NONE,
	   leave_subcommand, TRUE,
	   "route-map", "Create route-map or enter route-map command mode", 
	   PRIV_CONF);

TEST_BOOLEAN(config_routemap_test, config_routemap, NONE, ALTERNATE,
	     router_enable);

#undef	ALTERNATE
#define	ALTERNATE	config_routemap_test
