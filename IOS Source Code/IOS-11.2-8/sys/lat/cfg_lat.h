/* $Id: cfg_lat.h,v 3.2 1995/11/17 17:32:16 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/cfg_lat.h,v $
 *------------------------------------------------------------------
 * C F G _ L A T . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_lat.h,v $
 * Revision 3.2  1995/11/17  17:32:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:53:44  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:08:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * lat group <name> <group-list>
 * no lat group <name> [<group-list>]
 */
EOLS	(lat_group_eol, lat_command, LAT_GROUPL );

SET	(lat_group_action_set, lat_group_eol, OBJ(int,4), LAT_GROUP_SET);
KEYWORD_ID(lat_group_disabled, lat_group_eol, lat_group_action_set,
	   OBJ(int,4), LAT_GROUP_DISABLED,
	   "disabled",
	   "Incrementally remove specified groups from list",
	   PRIV_CONF );
KEYWORD_ID(lat_group_enabled, lat_group_eol, lat_group_disabled,
	   OBJ(int,4), LAT_GROUP_ENABLED,
	   "enabled",
	   "Incrementally add specified groups to list",
	   PRIV_CONF );

SET	(lat_group_list_set, lat_group_enabled, OBJ(int,3), LAT_GROUP_LIST );
GROUPLIST(lat_group_list, lat_group_list_set, NONE,
	  OBJ(string,2), "Group numbers" );

KEYWORD_ID(lat_group_all, lat_group_enabled, lat_group_list,
	   OBJ(int,3), LAT_GROUP_ALL,
	   "all", "All group codes", PRIV_CONF );
TESTVAR	(lat_group_noprefix, lat_group_eol, NONE,
	 NONE, NONE, lat_group_all, sense, FALSE );
STRING	(lat_group_name, lat_group_noprefix, no_alt,
	 OBJ(string,1), "Name of the group" );
NVGENS	(lat_group_nvgen, lat_group_name, lat_command, LAT_GROUPL );
KEYWORD (lat_group, lat_group_nvgen, no_alt,
	 "group-list", "Create a group list", PRIV_CONF );

/***************************************************************/
KEYWORD	(glob_lat, lat_group, ALTERNATE,
	 "lat", "DEC Local Area Transport (LAT) transmission protocol",
	 PRIV_CONF  );

#undef	ALTERNATE
#define	ALTERNATE	glob_lat
