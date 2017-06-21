/* $Id: exec_debug_ppp.h,v 3.3.32.2 1996/07/30 21:01:41 dblair Exp $
 * $Source: /release/112/cvs/Xsys/wan/exec_debug_ppp.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ P P P . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_ppp.h,v $
 * Revision 3.3.32.2  1996/07/30  21:01:41  dblair
 * CSCdi57980:  debug ppp multilink is too busy
 * Branch: California_branch
 * Clean up multilink debugs
 *
 * Revision 3.3.32.1  1996/04/27  06:36:25  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.3.2.1  1996/01/16  15:45:15  dblair
 * CSCdi42375:  Move Multilink PPP into PPP subsystem
 * Branch: Dial1_branch
 *
 * Revision 3.3  1996/01/11  10:18:33  billw
 * CSCdi46705:  debug ppp chap should be debug ppp authentication
 * Just do it.  Retain "hidden" "debug ppp chap" for now.
 *
 * Revision 3.2  1995/11/17  18:01:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/20  22:39:00  dblair
 * CSCdi40745:  PPP Multilink feature commit
 *
 * Revision 2.1  1995/07/02  01:46:06  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/19  14:48:40  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:19:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * debug ppp chap
 */

EOLS    (debug_ppp_chap_eol, debug_command, DEBUG_PPP_CHAP);
KEYWORD (debug_ppp_chap, debug_ppp_chap_eol, no_alt,
	 "chap", "chap", PRIV_OPR|PRIV_HIDDEN);
KEYWORD (debug_ppp_auth, debug_ppp_chap_eol, debug_ppp_chap,
	 "authentication", "CHAP and PAP authentication", PRIV_OPR);
/*
 * debug ppp error
 */

EOLS    (debug_ppp_error_eol, debug_command, DEBUG_PPP_ERROR);
KEYWORD (debug_ppp_error, debug_ppp_error_eol, debug_ppp_auth,
	 "error", "Protocol errors and error statistics", PRIV_OPR);

/*
 * debug ppp negotiation
 */

EOLS    (debug_ppp_negotiation_eol, debug_command, DEBUG_PPP_NEGOTIATION);
KEYWORD (debug_ppp_negotiation, debug_ppp_negotiation_eol, debug_ppp_error,
	 "negotiation", "Protocol parameter negotiation", PRIV_OPR);

/*
 * debug ppp packet
 */

EOLS    (debug_ppp_packet_eol, debug_command, DEBUG_PPP_PACKET);
KEYWORD (debug_ppp_packet, debug_ppp_packet_eol, debug_ppp_negotiation,
	 "packet", "Low-level PPP packet dump", PRIV_OPR);

/*
 * Multilink debugs
 */

EOLS    (debug_multilink_eol, debug_command, DEBUG_MULTILINK);
EOLS    (debug_multilink_events_eol, debug_command,
	 DEBUG_MULTILINK_EVENTS);
KEYWORD (debug_multilink_events, debug_multilink_events_eol, 
	 debug_multilink_eol,
	 "events", "Multilink events", PRIV_OPR);
EOLS    (debug_multilink_data_eol, debug_command,
	 DEBUG_MULTILINK_DATA);
KEYWORD (debug_multilink_data, debug_multilink_data_eol, 
	 debug_multilink_events,
	 "data", "First bytes of multilink packet", PRIV_OPR | PRIV_HIDDEN);
EOLS    (debug_multilink_detailed_eol, debug_command,
	 DEBUG_MULTILINK_DETAILED);
KEYWORD (debug_multilink_detailed, debug_multilink_detailed_eol, 
	 debug_multilink_data,
	 "detailed", "Detailed multilink activity", PRIV_OPR | PRIV_HIDDEN);

/*
 * debug ppp multilink
 */

KEYWORD (debug_multilink, debug_multilink_detailed, debug_ppp_packet,
	 "multilink", "Multilink activity", PRIV_OPR);

/******************************************************************
 * debug ppp
 */

KEYWORD_DEBUG(debug_ppp, debug_multilink, ALTERNATE,
	      OBJ(pdb,1), ppp_debug_arr,
	      "ppp", "PPP (Point to Point Protocol) information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_ppp
