/* $Id: cfg_lat_routing.h,v 3.2 1995/11/17 17:32:18 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/cfg_lat_routing.h,v $
 *------------------------------------------------------------------
 * C F G _ L A T _ R O U T I N G . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_lat_routing.h,v $
 * Revision 3.2  1995/11/17  17:32:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:53:45  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:08:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] lat service <name> enabled
 * [no] lat service <name> identifaction <string>
 * [no] lat service <name> rating <num>
 * [no] lat service <name> autocommand [<string>]
 * [no] lat service <name> rotary <num>
 * [no] lat service <name> password [<string>]
 * no lat service <name>
 */
EOLS	(lat_service_eol, lat_command, LAT_SVC );
STRING	(lat_service_pw_string, lat_service_eol, lat_service_eol,
	 OBJ(string,2), "LAT password for the service" );
KEYWORD_ID(lat_service_password, lat_service_pw_string, lat_service_eol,
	   OBJ(int,1), LAT_PASSWORD,
	   "password", "Set up a LAT password for the service", PRIV_CONF );
NUMBER	(lat_service_rotary_num, lat_service_eol, no_alt,
	 OBJ(int,2), 0, -1, "Rotary group number" );
NOPREFIX(lat_service_rotary_noprefix, lat_service_rotary_num, lat_service_eol );
KEYWORD_ID(lat_service_rotary, lat_service_rotary_noprefix, lat_service_password,
	   OBJ(int,1), LAT_ROTARY,
	   "rotary", "Associate a rotary group with a service", PRIV_CONF );
TEXT	(lat_service_auto_string, lat_service_eol, lat_service_eol,
	 OBJ(string,2), "Command to be associated with service" );
KEYWORD_ID(lat_service_auto, lat_service_auto_string, lat_service_rotary,
	   OBJ(int,1), LAT_AUTOCOMMAND,
	   "autocommand", "Associate a command with a service", PRIV_CONF );
NUMBER	(lat_service_rating_num, lat_service_eol, no_alt,
	 OBJ(int,2), 1, 255, "Static service rating for specified service" );
NOPREFIX(lat_service_rating_noprefix, lat_service_rating_num, lat_service_eol );
KEYWORD_ID(lat_service_rating, lat_service_rating_noprefix, lat_service_auto,
	   OBJ(int,1), LAT_RATING,
	   "rating", "Set the static service rating for specified service", PRIV_CONF );
TEXT	(lat_service_ident_name, lat_service_eol, no_alt,
	 OBJ(string,2), "Text that describes the service" );
NOPREFIX(lat_service_ident_noprefix, lat_service_ident_name, lat_service_eol );
KEYWORD_ID(lat_service_ident, lat_service_ident_noprefix, lat_service_rating,
	   OBJ(int,1), LAT_IDENT,
	   "identification",
	   "Set LAT service identification for specified service",
	   PRIV_CONF );
KEYWORD_ID(lat_service_enabled, lat_service_eol, lat_service_ident,
	   OBJ(int,1), LAT_ENABLE_CMD,
	   "enabled", "Enable inbound connections", PRIV_CONF );
TESTVAR	(lat_service_noprefix, lat_service_enabled, lat_service_eol,
	 NONE, NONE, NONE, sense, TRUE );
STRING	(lat_service_name, lat_service_enabled, no_alt,
	 OBJ(string,1), "Service name" );
NVGENS	(lat_service_nvgen, lat_service_name,
	 lat_command, LAT_SVC );
KEYWORD (lat_service, lat_service_nvgen, no_alt,
	 "service", "Enable inbound services", PRIV_CONF );

/*
 * [no] lat server-buffers n
 */

PARAMS_KEYONLY(lat_extra_server, lat_service, "server-buffers",
	       OBJ(int,1), 1, 128,
	       lat_command, LAT_EXTRA_SERVER,
	       "Set the number of receive buffers "
	       "for server connections",
	       "Receive buffers",
	       PRIV_CONF );

/*
 * [no] lat host-buffers n
 */

PARAMS_KEYONLY(lat_extra_host, lat_extra_server, "host-buffers",
	       OBJ(int,1), 1, 128,
	       lat_command, LAT_EXTRA_HOST,
	       "Set the number of receive buffers for host connections",
	       "Receive buffers",
	       PRIV_CONF );

/*
 * [no] lat vc-sessions n
 */

PARAMS_KEYONLY(lat_vc_sessions, lat_extra_host, "vc-sessions",
	       OBJ(int,1), 1, 255,
	       lat_command, LAT_VC_SESSIONS,
	       "Set the maximum sessions per LAT virtual circuit",
	       "Maximum sessions",
	       PRIV_CONF );

/*
 * [no] lat service-announcements
 */


EOLS	(lat_service_ann_eol, lat_command, LAT_SERVICE_ANNOUNCEMENTS);

KEYWORD_MM(lat_service_ann, lat_service_ann_eol, lat_vc_sessions,
	   "service-announcements",
	   "Control generation of multicast service announcements",
	   PRIV_CONF, 9);

/*
 * [no] lat service-responder
 */

EOLS	(lat_service_responder_eol, lat_command, LAT_SERVICE_RESPONDER);

KEYWORD_MM(lat_service_responder, lat_service_responder_eol, lat_service_ann,
	   "service-responder",
	   "Respond to service solicitation requests for other nodes",
	   PRIV_CONF, 9);

/***************************************************************
 * lat service-group <group-list>
 * no lat service-group [<group-list>]
 */
EOLS	(lat_sgroup_eol, lat_command, LAT_SERVICEG );

SET	(lat_sgroup_action_set, lat_sgroup_eol, OBJ(int,4), LAT_GROUP_SET );
KEYWORD_ID(lat_sgroup_disabled, lat_sgroup_eol, lat_sgroup_action_set,
	   OBJ(int,4), LAT_GROUP_DISABLED,
	   "disabled", "Remove group code to group code mask", PRIV_CONF );
KEYWORD_ID(lat_sgroup_enabled, lat_sgroup_eol, lat_sgroup_disabled,
	   OBJ(int,4), LAT_GROUP_ENABLED,
	   "enabled", "Add group code to group code mask", PRIV_CONF );

SET	(lat_sgroup_list_set, lat_sgroup_enabled, OBJ(int,3), LAT_GROUP_LIST );
GROUPLIST(lat_sgroup_list, lat_sgroup_list_set, NONE,
	  OBJ(string,2), "Group code mask" );

KEYWORD_ID(lat_sgroup_all, lat_sgroup_enabled, lat_sgroup_list,
	   OBJ(int,3), LAT_GROUP_ALL,
	   "all", "All group codes", PRIV_CONF );
TESTVAR	(lat_sgroup_noprefix, lat_sgroup_eol, NONE,
	 NONE, NONE, lat_sgroup_all, sense, FALSE );
NVGENS	(lat_sgroup_nvgen, lat_sgroup_noprefix, lat_command, LAT_SERVICEG );
KEYWORD_MM(lat_sgroup, lat_sgroup_nvgen, lat_service_responder,
	   "service-group",
	   "Specify a group code mask to use when advertising all services for this node",
	   PRIV_CONF, 9 );

/***************************************************************
 * lat retransmit-limit <num>
 * no lat retransmit-limit
 */
PARAMS_KEYONLY(lat_retrans, lat_sgroup, "retransmit-limit",
	       OBJ(int,1), 4, 120,
	       lat_command, LAT_RETRANSMIT,
	       "Set the number of LAT retransmits before declaring system unreachable",
	       "LAT retransmit limit", PRIV_CONF );

/***************************************************************
 * lat service-timer <num>
 * no lat service-timer
 */
EOLS	(lat_servicet_eol, lat_command, LAT_SERVICET);
NUMBER	(lat_servicet_number, lat_servicet_eol, no_alt,
	 OBJ(int,1), 10, 180,
	 "Number of seconds between service advertisements");
NOPREFIX(lat_servicet_noprefix, lat_servicet_number, lat_servicet_eol);
NVGENS	(lat_servicet_nvgen, lat_servicet_noprefix,
	 lat_command, LAT_SERVICET);
KEYWORD_MM(lat_servicet, lat_servicet_nvgen, lat_retrans,
	   "service-timer", "Set the time between LAT service advertisements",
	   PRIV_CONF, 9);

/***************************************************************
 * lat ka-timer <num>
 * no lat ka-timer
 */
PARAMS_KEYONLY(lat_katimer, lat_servicet, "ka-timer",
	       OBJ(int,1), 10, 255,
	       lat_command, LAT_KATIMER,
	       "Set the LAT keepalive timer",
	       "LAT keepalive timer in seconds", PRIV_CONF );

/***************************************************************
 * lat host-delay <num>
 * no lat host-delay
 */
PARAMS_KEYONLY(lat_hdtimer, lat_katimer, "host-delay",
	       OBJ(int,1), 10, 2550,
	       lat_command, LAT_HDTIMER,
	       "Set the LAT host delay",
	       "LAT host delay timer in milliseconds", PRIV_CONF );

/***************************************************************
 * lat vc-timer <num>
 * no lat vc-timer
 */
PARAMS_KEYONLY(lat_vctimer, lat_hdtimer, "vc-timer",
	       OBJ(int,1), 10, 1000,
	       lat_command, LAT_VCTIMER,
	       "Set the virtual-circuit timer",
	       "Virtual-circuit timer in milliseconds", PRIV_CONF );

/******************************************************************
 * lat node-name <name>
 *
 * OBJ(string,1) = <name>
 */
EOLS	(lat_nodename_eol, lat_command, LAT_NODENAME);
STRING	(lat_nodename_name, lat_nodename_eol, no_alt,
	 OBJ(string,1), "The nodename");
NOPREFIX (lat_nodename_noprefix, lat_nodename_name, lat_nodename_eol);
NVGENS	(lat_nodename_nvgens, lat_nodename_noprefix, lat_command, LAT_NODENAME);
KEYWORD (lat_nodename, lat_nodename_nvgens, lat_vctimer,
	 "node-name", "Set LAT nodename different than hostname", PRIV_CONF);


/***************************************************************
 * lat access-list <num> { permit | deny } <string>
 * no lat access-list <num>
 *
 * note that there are 200 rergular expression access lists, since they
 * must cover the same range as both regular and complex IP accesslists.
 */
EOLS	(lat_access_eol, lat_command, LAT_ACCESS );
STRING	(lat_access_pattern, lat_access_eol, no_alt,
	 OBJ(string,1), "LAT node name, with or without regular expression pattern matching" );
KEYWORD_ID(lat_access_deny, lat_access_pattern, no_alt,
	   OBJ(int,2), FALSE,
	   "deny", "Deny access to matching node names", PRIV_CONF );
KEYWORD_ID(lat_access_permit, lat_access_pattern, lat_access_deny,
	   OBJ(int,2), TRUE,
	   "permit", "Allow access to matching node names", PRIV_CONF );
NOPREFIX(lat_access_noprefix, lat_access_permit, lat_access_eol);
NUMBER	(lat_access_num, lat_access_noprefix, no_alt,
	 OBJ(int,1), MINREGEXPACC+1, MAXREGEXPACC, "LAT access-list number" );
NVGENS	(lat_access_nvgen, lat_access_num,
	 lat_command, LAT_ACCESS );
KEYWORD	(lat_access, lat_access_nvgen, lat_nodename,
	 "access-list", "Specify a LAT access condition", PRIV_CONF );


KEYWORD	(lat_routing_commands, lat_access, ALTERNATE,
	 "lat", "", PRIV_CONF | PRIV_DUPLICATE );

#undef	ALTERNATE
#define	ALTERNATE	lat_routing_commands
