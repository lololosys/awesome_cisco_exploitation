/* $Id: cfg_line_lat.h,v 3.2 1995/11/17 17:32:19 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/cfg_line_lat.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ L A T . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_lat.h,v $
 * Revision 3.2  1995/11/17  17:32:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:53:46  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/21  18:17:07  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:09:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/***************************************************************
 * lat remote-modification
 *
 */
EOLS	(lat_remote_mod_eol, lat_remote_modification , LAT_REMOTE_MODIFY);
KEYWORD	(lat_remote_mod, lat_remote_mod_eol, no_alt,
	 "remote-modification", "Set line to be remote modifiable", PRIV_CONF);

/***************************************************************
 * lat out-group <group-list>
 * no lat out-group [<group-list>]
 */
EOLS	(lat_outgroup_eol, lat_outgroup_command, LAT_OGROUP );

SET	(lat_outgroup_action_set, lat_outgroup_eol, OBJ(int,4), LAT_GROUP_SET );
KEYWORD_ID(lat_outgroup_disabled, lat_outgroup_eol, lat_outgroup_action_set,
	   OBJ(int,4), LAT_GROUP_DISABLED,
	   "disabled",
	   "Incrementally remove specified groups from list",
	   PRIV_USER );
KEYWORD_ID(lat_outgroup_enabled, lat_outgroup_eol, lat_outgroup_disabled,
	   OBJ(int,4), LAT_GROUP_ENABLED,
	   "enabled",
	   "Incrementally add specified groups to list", PRIV_USER );

SET	(lat_outgroup_list_set, lat_outgroup_enabled, OBJ(int,3), LAT_GROUP_LIST );
GROUPLIST(lat_outgroup_list, lat_outgroup_list_set, NONE,
	  OBJ(string,2), "Group list" );

KEYWORD_ID(lat_outgroup_all, lat_outgroup_enabled, lat_outgroup_list,
	   OBJ(int,3), LAT_GROUP_ALL,
	   "all", "All group codes", PRIV_USER );
TESTVAR	(lat_outgroup_noprefix, lat_outgroup_eol, NONE,
	 NONE, NONE, lat_outgroup_all, sense, FALSE );
NVGENS	(lat_outgroup_nvgen, lat_outgroup_noprefix,
	 lat_outgroup_command, LAT_OGROUP );
KEYWORD	(lat_outgroup, lat_outgroup_nvgen, lat_remote_mod,
	 "out-group",
	 "Define group list for outgoing user-initiated connections",
	 PRIV_USER );

KEYWORD	(line_lat, lat_outgroup, ALTERNATE,
	 "lat", "DEC Local Area Transport (LAT) protocol-specific configuration",
	 PRIV_USER );

#undef	ALTERNATE
#define	ALTERNATE	line_lat
