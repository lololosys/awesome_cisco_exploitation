/* $Id: exec_show_lat.h,v 3.2.60.2 1996/08/12 16:04:23 widmer Exp $
 * $Source: /release/112/cvs/Xsys/lat/exec_show_lat.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ L A T . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_lat.h,v $
 * Revision 3.2.60.2  1996/08/12  16:04:23  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.60.1  1996/05/30  23:44:32  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:32:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:53:48  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:24:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show node [<node-name>|all] [counters|status|summary]
 * 
 */
EOLS	(show_node_eol, show_node, SHOW_NODE );

SET	(show_node_opts_set, show_node_eol, OBJ(int,2), TRUE );
KEYWORD_ID(show_node_summary, show_node_opts_set, show_node_eol,
	   OBJ(int,1), SHOW_NODE_SUMMARY,
	   "summary", "Show a summary for the selected node(s)", PRIV_USER );
KEYWORD_ID(show_node_status, show_node_opts_set, show_node_summary,
	   OBJ(int,1), SHOW_NODE_STATUS,
	   "status", "Show the status for the selected node(s)", PRIV_USER );
KEYWORD_ID(show_node_counters, show_node_opts_set, show_node_status,
	   OBJ(int,1), SHOW_NODE_COUNTERS,
	   "counters", "Show the counters for the selected node(s)", PRIV_USER );


SET	(show_node_name_set, show_node_counters,
	 OBJ(int,1), SHOW_NODE_STATUS );
STRING	(show_node_name, show_node_name_set, NONE,
	 OBJ(string,1), "Node name" );
TESTVAR	(show_node_name_test, show_node_name, NONE,
	 NONE, NONE, no_alt, OBJ(int,2), FALSE );


NOP	(show_node_nop, show_node_counters, show_node_name_test );

KEYWORD_ID(show_node_all, show_node_nop, show_node_nop,
	   OBJ(int,2), TRUE,
	   "all", "All nodes", PRIV_USER );

KEYWORD(show_node, show_node_all, ALTERNATE,
	"node", "Show known LAT nodes", PRIV_USER|PRIV_USER_HIDDEN);



/******************************************************************
 * show services <string>
 * 
 */

EOLNS	(show_services_eol, declat_show_ls);
STRING	(show_services_name, show_services_eol, show_services_eol,
	 OBJ(string,1), "Show a specific LAT learned service" );
KEYWORD	(show_services, show_services_name, show_node,
	 "services", "LAT learned services", PRIV_USER|PRIV_USER_HIDDEN);

/******************************************************************
 * show lat { access-list [<num>] | advertised | groups | nodes | services | 
 *		sessions <session-id> | traffic | <session-id> }
 * 
 * OBJ(int,1) = SHOW_LAT_AS, SHOW_LAT_GROUPS, SHOW_LAT_SERVERS,
 *		SHOW_LAT_NODES, SHOW_LAT_SESSIONS, SHOW_LAT_TRAFFIC
 * OBJ(int,2) = <session-id>
 */

EOLS	(show_lat_eol, show_lat, SHOW_LAT);

SET	(show_lat_defsession, show_lat_eol, OBJ(int,1), SHOW_LAT_SESSIONS);
SET	(show_lat_noline, show_lat_defsession, OBJ(int,2), -1);
NUMBER	(show_lat_line, show_lat_defsession, show_lat_noline,
	OBJ(int,2), 0, 256, "TTY line number");

KEYWORD_ID (show_lat_traffic, show_lat_eol, show_lat_line,
	OBJ(int,1), SHOW_LAT_TRAFFIC,
	"traffic", "Traffic statistics", PRIV_USER);
KEYWORD_ID (show_lat_sessions, show_lat_line, show_lat_traffic,
	OBJ(int,1), SHOW_LAT_SESSIONS,
	"sessions", "Session status", PRIV_USER);
KEYWORD_ID (show_lat_services, show_lat_eol, show_lat_sessions,
	OBJ(int,1), SHOW_LAT_SERVICES,
	"services", "Learned services", PRIV_USER);
KEYWORD_ID (show_lat_nodes, show_lat_eol, show_lat_services,
	OBJ(int,1), SHOW_LAT_NODES,
	"nodes", "Remote node status", PRIV_USER);
KEYWORD_ID (show_lat_groups, show_lat_eol, show_lat_nodes,
	OBJ(int,1), SHOW_LAT_GROUPS,
	"groups", "Defined Group names", PRIV_USER);
KEYWORD_ID (show_lat_as, show_lat_eol, show_lat_groups,
	OBJ(int,1), SHOW_LAT_AS,
	"advertised", "Advertised services", PRIV_USER);
NUMBER(show_lat_acc_list, show_lat_eol, show_lat_eol,
	OBJ(int,2), MINREGEXPACC, MAXREGEXPACC, "Access list number");
KEYWORD_ID (show_lat_acc, show_lat_acc_list, show_lat_as,
	OBJ(int,1), SHOW_LAT_ACCESS,
	"access-list", "Access lasts", PRIV_USER);
KEYWORD (show_lat_kwd, show_lat_acc, show_services,
	 "lat", "DEC LAT information", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_lat_kwd
