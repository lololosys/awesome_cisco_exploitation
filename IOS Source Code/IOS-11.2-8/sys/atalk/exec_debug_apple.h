/* $Id: exec_debug_apple.h,v 3.3.26.3 1996/06/29 00:07:23 thille Exp $
 * $Source: /release/112/cvs/Xsys/atalk/exec_debug_apple.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ A P P L E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_apple.h,v $
 * Revision 3.3.26.3  1996/06/29  00:07:23  thille
 * CSCdi61558:  Trim a little fat in the appletalk text strings
 * Clean up some literal strings and get back 580 bytes.
 *
 * Revision 3.3.26.2  1996/05/21  09:42:34  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3.26.1  1996/05/02  08:41:25  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.3  1996/01/18  02:12:06  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.2  1995/11/17  08:44:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:58:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:51:02  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/06/19  14:47:07  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/08  15:55:29  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  22:17:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(debug_atalk_extend_here, no_alt);

/*****************************************************************
 * debug apple load-balancing
 */
EOLS    (debug_apple_lb_eol, atalk_debug_command, DEBUG_ATALK_LB);
KEYWORD (debug_apple_lb,debug_apple_lb_eol, debug_atalk_extend_here,
	 "load-balancing", "AppleTalk load-balancing", PRIV_OPR);

/******************************************************************
 * debug apple redistribution
 *
 */
EOLS (debug_apple_redist_eol, atalk_debug_command, DEBUG_ATALKREDIST);
KEYWORD (debug_apple_redist, debug_apple_redist_eol, debug_apple_lb,
	 "redistribution", "Route Redistribution", PRIV_OPR);

/******************************************************************
 * debug apple zip [interface <interface>]
 *
 * OBJ(int, 1) = ATALK_DEBUG_INTERFACE if interface <interface>
 * OBJ(idb, 1) = <interface>
 */
EOLS	(debug_apple_zip_eol, atalk_debug_command, DEBUG_ZIP);
INTERFACE (debug_apple_zip_int_name, debug_apple_zip_eol,
	  no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_apple_zip_int, debug_apple_zip_int_name, debug_apple_zip_eol,
	    OBJ(int, 1), ATALK_DEBUG_INTERFACE,
	 common_str_interface, "Appletalk interface-specific debugging", PRIV_OPR);
KEYWORD (debug_apple_zip, debug_apple_zip_int, debug_apple_redist,
	 "zip", "Zone Information Protocol functions", PRIV_OPR);

/******************************************************************
 * debug apple routing [interface <interface>]
 *
 * OBJ(int, 1) = ATALK_DEBUG_INTERFACE if interface <interface>
 * OBJ(idb, 1) = <interface>
 */
EOLS	(debug_apple_routing_eol, atalk_debug_command, DEBUG_ATALKROUTE);
INTERFACE (debug_apple_routing_int_name, debug_apple_routing_eol,
	   no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_apple_routing_int, debug_apple_routing_int_name,
	    debug_apple_routing_eol, OBJ(int, 1), ATALK_DEBUG_INTERFACE,
	    common_str_interface, "Appletalk interface-specific debugging", PRIV_OPR);
KEYWORD (debug_apple_routing, debug_apple_routing_int, debug_apple_zip,
	 "routing", "(RTMP&EIGRP) functions", PRIV_OPR);
	    

/******************************************************************
 * debug apple rtmp [interface <interface>]
 *
 * OBJ(int, 1) = ATALK_DEBUG_INTERFACE if interface <interface>
 * OBJ(idb, 1) = <interface>
 *
 */
EOLS	(debug_apple_rtmp_eol, atalk_debug_command, DEBUG_ATALKRTMP);
INTERFACE (debug_apple_rtmp_int_name, debug_apple_rtmp_eol,
	   no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_apple_rtmp_int, debug_apple_rtmp_int_name,
	    debug_apple_rtmp_eol, OBJ(int, 1), ATALK_DEBUG_INTERFACE,
	    common_str_interface, "Appletalk interface-specific debugging", PRIV_OPR);
KEYWORD (debug_apple_rtmp, debug_apple_rtmp_int, debug_apple_routing,
	 "rtmp", "(RTMP) functions", PRIV_OPR);


/******************************************************************
 * debug apple responder [interface <interface>]
 *
 * OBJ(int, 1) = ATALK_DEBUG_INTERFACE if interface <interface>
 * OBJ(idb, 1) = <interface>
 */
EOLS	(debug_apple_responder_eol, atalk_debug_command, DEBUG_ATALKRSP );
INTERFACE (debug_apple_responder_int_name, debug_apple_responder_eol,
	   no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_apple_responder_int, debug_apple_responder_int_name,
	    debug_apple_responder_eol, OBJ(int, 1), ATALK_DEBUG_INTERFACE,
	 common_str_interface, "Appletalk interface-specific debugging", PRIV_OPR);
KEYWORD (debug_apple_responder, debug_apple_responder_int, debug_apple_rtmp,
	 "responder", "AppleTalk responder debugging", PRIV_OPR);

/******************************************************************
 * debug apple packet [interface <interface>] [level <number>]
 *
 * OBJ(int, 1) = ATALK_DEBUG_INTERFACE if interface <interface>
 * OBJ(idb, 1) = <interface>
 */
EOLS	(debug_apple_packet_eol, atalk_debug_command, DEBUG_ATALKPKT);
DECIMAL (debug_apple_packet_level_number, debug_apple_packet_eol, no_alt,
	 OBJ(int, 2), 0, 3, "Debugging level (higher for more info)");
KEYWORD (debug_apple_packet_level, debug_apple_packet_level_number,
	 debug_apple_packet_eol,
	 "level", "Debugging level", PRIV_OPR);
/*
INTERFACE (debug_apple_packet_int_name, debug_apple_packet_level,
	   no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD (debug_apple_packet_int, debug_apple_packet_int_name,
	 debug_apple_packet_level,
	 common_str_interface, "Appletalk interface-specific debugging", PRIV_OPR);
*/
INTERFACE_KEYWORD(debug_apple_packet_int, debug_apple_packet_level,
		  debug_apple_packet_level,
		  OBJ(idb, 1), IFTYPE_HWIDB,
		  common_str_interface, "Appletalk interface-specific debugging");

KEYWORD (debug_apple_packet, debug_apple_packet_int, debug_apple_responder,
	 "packet", "Per-packet debugging", PRIV_OPR);

/******************************************************************
 * debug apple nbp [interface <interface>]
 *
 * OBJ(int, 1) = ATALK_DEBUG_INTERFACE if interface <interface>
 * OBJ(idb, 1) = <interface>
 */
EOLS	(debug_apple_nbp_eol, atalk_debug_command, DEBUG_NBP);
DECIMAL (debug_apple_nbp_level_number, debug_apple_nbp_eol, no_alt,
	 OBJ(int, 2), 0, 3, "Debugging level (higher for more info)");
KEYWORD (debug_apple_nbp_level, debug_apple_nbp_level_number,
	 debug_apple_nbp_eol,
	 "level", "Debugging level", PRIV_OPR);
INTERFACE (debug_apple_nbp_int_name, debug_apple_nbp_level,
	   no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD (debug_apple_nbp_int, debug_apple_nbp_int_name,
	 debug_apple_nbp_level,
	 common_str_interface, "Appletalk interface-specific debugging", PRIV_OPR);
KEYWORD (debug_apple_nbp, debug_apple_nbp_int, debug_apple_packet,
	 "nbp", "Name Binding Protocol (NBP) functions", PRIV_OPR);

/******************************************************************
 * debug apple fs interface <interface>
 *
 * OBJ(int, 1) = ATALK_DEBUG_INTERFACE interface <interface>
 * OBJ(idb, 1) = <interface>
 *
 * The interface is not optional.
 */
EOLS	(debug_apple_fs_eol, atalk_debug_command, DEBUG_ATALKFS);
INTERFACE(debug_apple_fs_int_name, debug_apple_fs_eol,
	  no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_apple_fs_int, debug_apple_fs_int_name, no_alt,
	    OBJ(int, 1), ATALK_DEBUG_INTERFACE,
	    common_str_interface, "Appletalk interface-specific debugging", PRIV_OPR);
KEYWORD (debug_apple_fs, debug_apple_fs_int, debug_apple_nbp,
	"fs", "Appletalk fast-switching", PRIV_OPR);


/******************************************************************
 * debug apple events [interface <interface>]
 *
 * OBJ(int, 1) = ATALK_DEBUG_INTERFACE if interface <interface>
 * OBJ(idb, 1) = <interface>
 */
EOLS	(debug_apple_event_eol, atalk_debug_command, DEBUG_APPLEEVT);
INTERFACE(debug_apple_event_int_name, debug_apple_event_eol,
	  no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_apple_event_int, debug_apple_event_int_name,
	    debug_apple_event_eol, OBJ(int, 1), ATALK_DEBUG_INTERFACE,
	    common_str_interface, "Appletalk interface-specific debugging", PRIV_OPR);
KEYWORD (debug_apple_event, debug_apple_event_int, debug_apple_fs,
	 "events", "Appletalk special events", PRIV_OPR);

/******************************************************************
 * debug apple errors [interface <interface>]
 *
 * OBJ(int, 1) = ATALK_DEBUG_INTERFACE if interface <interface>
 * OBJ(idb, 1) = <interface>
 */
EOLS	(debug_apple_errors_eol, atalk_debug_command, DEBUG_APPLEERRS);
INTERFACE (debug_apple_errors_int_name, debug_apple_errors_eol,
	   no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_apple_errors_int, debug_apple_errors_int_name,
	    debug_apple_errors_eol, OBJ(int, 1), ATALK_DEBUG_INTERFACE,
	    common_str_interface, "Appletalk interface-specific debugging", PRIV_OPR);
KEYWORD (debug_apple_errors, debug_apple_errors_int, debug_apple_event,
	 "errors", "Information about errors", PRIV_OPR);

/******************************************************************
 * debug apple arp [interface <interface>]
 *
 * OBJ(int, 1) = ATALK_DEBUG_INTERFACE if interface <interface>
 * OBJ(idb, 1) = <interface>
 */
EOLS	(debug_apple_arp_eol, atalk_debug_command, DEBUG_APPLEARP);
INTERFACE(debug_apple_arp_int_name, debug_apple_arp_eol,
	  no_alt, OBJ(idb, 1), IFTYPE_HWIDB   );
KEYWORD_ID (debug_apple_arp_int, debug_apple_arp_int_name, debug_apple_arp_eol,
	    OBJ(int, 1), ATALK_DEBUG_INTERFACE,
	    common_str_interface, "Appletalk interface-specific debugging", PRIV_OPR);
KEYWORD (debug_apple_arp, debug_apple_arp_int, debug_apple_errors, 
	 "arp", "Appletalk address resolution protocol", PRIV_OPR);


/******************************************************************
 * debug apple
 *
 */
KEYWORD (debug_apple, debug_apple_arp, ALTERNATE,
	 "apple", "Appletalk information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_apple
