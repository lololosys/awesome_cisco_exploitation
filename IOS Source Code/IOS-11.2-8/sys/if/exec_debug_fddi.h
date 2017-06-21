/* $Id: exec_debug_fddi.h,v 3.2 1995/11/17 09:27:14 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/exec_debug_fddi.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ F D D I . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_fddi.h,v $
 * Revision 3.2  1995/11/17  09:27:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:49:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:53:21  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.4  1995/06/19  14:48:02  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.3  1995/06/13  03:29:16  alam
 * CSCdi35570:  Remove extraneous functions
 * Use llc_process_test and llc_process_xid for FDDI as well as ethernet
 * and FR.
 * Remove FDDI packet debug.
 *
 * Revision 2.2  1995/06/08  15:55:43  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  22:18:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug fddi-smt-packets
 *
 */
EOLS	(odebug_fddi_smt_pak_eol, debug_command,
	 DEBUG_FDDI_SMT_PACKET);

KEYWORD (odebug_fddi_smt_pak, odebug_fddi_smt_pak_eol, no_alt,
	 "fddi-smt-packets", "", PRIV_OPR|PRIV_HIDDEN);

/******************************************************************
 * debug fddi-events
 *
 */
EOLS	(odebug_fddi_events_eol, debug_command, DEBUG_FDDI);
KEYWORD (odebug_fddi_events, odebug_fddi_events_eol, odebug_fddi_smt_pak,
	 "fddi-events", "", PRIV_OPR|PRIV_HIDDEN);

/******************************************************************
 * debug fddi-cmt-events
 *
 */
EOLS	(odebug_fddi_cmt_events_eol, debug_command, DEBUG_FDDI_CMT_EVENTS);
KEYWORD (odebug_fddi_cmt_events, odebug_fddi_cmt_events_eol,odebug_fddi_events,
	 "fddi-cmt-events", "", PRIV_OPR|PRIV_HIDDEN);

/*======================================================================
 =
 =			      New Style
 =
 =======================================================================*/
/******************************************************************
 * debug fddi smt-packets
 *
 */
EOLS	(debug_fddi_smt_packets_eol, debug_command, DEBUG_FDDI_SMT_PACKET);
KEYWORD (debug_fddi_smt_packets, debug_fddi_smt_packets_eol, no_alt,
	 "smt-packets", "Log SMT packets", PRIV_OPR);

/******************************************************************
 * debug fddi events
 *
 */
EOLS	(debug_fddi_events_eol, debug_command, DEBUG_FDDI);
KEYWORD (debug_fddi_events, debug_fddi_events_eol, debug_fddi_smt_packets,
	 "events", "Other FDDI events", PRIV_OPR);

/******************************************************************
 * debug fddi cmt-events
 *
 */
EOLS	(debug_fddi_cmt_events_eol, debug_command, DEBUG_FDDI_CMT_EVENTS);
KEYWORD (debug_fddi_cmt_events, debug_fddi_cmt_events_eol, debug_fddi_events,
	 "cmt-events", "Log CMT transactions", PRIV_OPR);

/******************************************************************
 * debug fddi
 *
 */
KEYWORD_DEBUG(debug_fddi_kwd, debug_fddi_cmt_events, NONE,
	      OBJ(pdb,1), fddi_debug_arr,
	      "fddi", "FDDI information", PRIV_OPR);

TEST_INT(debug_fddi, debug_fddi_kwd, NONE, ALTERNATE, nfddinets);

#undef	ALTERNATE
#define	ALTERNATE	debug_fddi
