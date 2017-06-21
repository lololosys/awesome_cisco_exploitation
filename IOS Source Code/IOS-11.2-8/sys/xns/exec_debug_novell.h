/* $Id: exec_debug_novell.h,v 3.2.62.1 1996/05/03 02:16:25 sluong Exp $
 * $Source: /release/112/cvs/Xsys/xns/exec_debug_novell.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ N O V E L L . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_novell.h,v $
 * Revision 3.2.62.1  1996/05/03  02:16:25  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.2  1995/11/17  19:21:33  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:38:22  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/06/19  14:49:47  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/08  15:56:26  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  23:27:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(debug_novell_extend_here, no_alt);

/******************************************************************
 * debug ipx spoof
 *
 */
EOLS	(debug_novell_spoof_eol, debug_command, DEBUG_NOVELLSPOOF);
KEYWORD (debug_novell_spoof, debug_novell_spoof_eol,
	 debug_novell_extend_here, "spoof",
	 "IPX and SPX Spoofing activity", PRIV_OPR);

/******************************************************************
 * debug ipx rip-update
 *
 * This is a hidden but permanent flag to assist in debugging
 * during development.
 *
 */
EOLS	(debug_novell_sap_upd_eol, debug_command, DEBUG_NOVELL_SAP_UPD);
KEYWORD_MM (debug_novell_sap_upd,
	    debug_novell_sap_upd_eol, debug_novell_spoof,
	    "sap-update", "", PRIV_OPR|PRIV_HIDDEN, 7);

/******************************************************************
 * debug ipx rip-update
 *
 * This is a hidden but permanent flag to assist in debugging
 * during development.
 *
 */
EOLS	(debug_novell_rip_upd_eol, debug_command, DEBUG_NOVELL_RIP_UPD);
KEYWORD_MM (debug_novell_rip_upd,
	    debug_novell_rip_upd_eol, debug_novell_sap_upd,
	    "rip-update", "", PRIV_OPR|PRIV_HIDDEN, 7);

/******************************************************************
 * debug ipx private
 *
 * This is a hidden but permanent flag to assist in debugging
 * during development.
 *
 */
EOLS	(debug_novell_priv_eol, debug_command, DEBUG_NOVELLPVT);
KEYWORD (debug_novell_private, debug_novell_priv_eol, debug_novell_rip_upd,
	 "private", "", PRIV_OPR|PRIV_HIDDEN);

/******************************************************************
 * debug ipx verbose
 *
 * This flag is used in conjunction with other permanent flags to
 * generate additional output which might be considered excessive
 * in the default case. Keep this hidden until and unless approved
 * for use by the masses.
 *
 */
EOLS	(debug_novell_vbose_eol, debug_command, DEBUG_NOVELLVERBOSE);
KEYWORD (debug_novell_verbose, debug_novell_vbose_eol, debug_novell_private,
	 "verbose", "", PRIV_OPR|PRIV_HIDDEN);

/******************************************************************
 * debug ipx redistribution
 *
 */
EOLS	(debug_novell_dist_eol, debug_command, DEBUG_NOVELLREDIST);
KEYWORD (debug_novell_redist, debug_novell_dist_eol, debug_novell_verbose,
	 "redistribution", "IPX route redistribution", PRIV_OPR);


/******************************************************************
 * debug ipx sap events
 *
 */
EOLS	(debug_novell_sap_events_eol, debug_command, DEBUG_NOVELLSAPEEVT);
KEYWORD (debug_novell_sap_events, debug_novell_sap_events_eol, 
	 no_alt, "events", 
	 "IPX Service Advertisement events", PRIV_OPR);

/******************************************************************
 * debug ipx sap activity
 *
 */
EOLS	(debug_novell_sap_activity_eol, debug_command, DEBUG_NOVELLSAP);
KEYWORD (debug_novell_sap_activity, debug_novell_sap_activity_eol,
	 debug_novell_sap_events,
	 "activity", "IPX Service Advertisement packets", PRIV_OPR);

/******************************************************************
 * debug ipx sap
 *
 */
KEYWORD (debug_novell_sap, debug_novell_sap_activity, debug_novell_redist,
	 "sap", "IPX Service Advertisement information", PRIV_OPR);

/******************************************************************
 * debug ipx routing events
 *
 */
EOLS	(debug_novell_routing_events_eol, debug_command, DEBUG_NOVELLRIPEVT);
KEYWORD (debug_novell_routing_events, debug_novell_routing_events_eol, no_alt,
	 "events", "IPX RIP routing events", PRIV_OPR);

/******************************************************************
 * debug ipx routing activity
 *
 */
EOLS	(debug_novell_routing_activity_eol, debug_command, DEBUG_NOVELLRIP);
KEYWORD (debug_novell_routing_activity, debug_novell_routing_activity_eol,
	 debug_novell_routing_events,
	 "activity", "IPX RIP routing activity", PRIV_OPR);

/******************************************************************
 * debug ipx routing
 *
 */
KEYWORD (debug_novell_routing, debug_novell_routing_activity, debug_novell_sap,
	 "routing", "IPX RIP routing information", PRIV_OPR);

/******************************************************************
 * debug ipx packet
 *
 */
EOLS	(debug_novell_packet_eol, debug_command, DEBUG_NOVELL);
KEYWORD (debug_novell_packet, debug_novell_packet_eol, debug_novell_routing,
	 "packet", "IPX activity", PRIV_OPR);

/******************************************************************
 * debug ipx
 *
 */
SET	(debug_novell_set, debug_novell_packet, OBJ(pdb,1), novell_debug_arr);
NOVELL_IPX_KEYWORD(debug_novell, debug_novell_set, ALTERNATE,
		   "Novell/IPX information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_novell
