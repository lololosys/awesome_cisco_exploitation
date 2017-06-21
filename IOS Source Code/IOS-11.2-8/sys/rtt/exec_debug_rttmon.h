/* $Id: exec_debug_rttmon.h,v 1.1.4.2 1996/06/07 22:25:48 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/rtt/exec_debug_rttmon.h,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor parse chain header file.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This header file contains the  definitions for
 * the RTT Monitor debug parse chain.
 *------------------------------------------------------------------
 * $Log: exec_debug_rttmon.h,v $
 * Revision 1.1.4.2  1996/06/07  22:25:48  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/17  11:40:45  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.2  1996/04/19  02:41:15  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 1.1.2.1  1996/03/22  18:41:28  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:21  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * [no] debug rtr { error | trace } [ <entry-number> ]
 */

LINK_TRANS(debug_rttmon_extend_here, NONE);

/******************************************************************
 * [no] debug rtr error [ <entry-number> ]
 * 
 * OBJ(int,1) = <entry-number>
 * OBJ(int,2) = 1 = entry number exists
 *              0 = no entry number
 */
EOLS(debug_rttmon_error_eol, rttmon_debug_command,
     DEBUG_RTTMON_ERROR);

SET(debug_rttmon_error_entry_number,
    debug_rttmon_error_eol, OBJ(int, 2), 1);

SET(debug_rttmon_error_entry_no_number,
    debug_rttmon_error_eol, OBJ(int, 2), 0);

NUMBER(debug_rttmon_error_entry,
       debug_rttmon_error_entry_number,
       debug_rttmon_error_entry_no_number, OBJ(int, 1), 0,
       MAX_rttMonApplNumCtrlAdminEntry, "Entry Number");

KEYWORD(debug_rttmon_error, debug_rttmon_error_entry,
	debug_rttmon_extend_here,
	D_rttMonDebugError_string,
	"Output RTR Error Messages", PRIV_OPR);

/******************************************************************
 * [no] debug rtr trace [ <entry-number> ]
 *
 * NOTE:  The above error debug is required to trace, thus when you 
 *          turn on trace for all, or a single entry the same
 *          value will be turned on for error debug.  But,
 *          the opposite is not true, turning off trace does not
 *          turn on the error debug for that entry (or all).
 *        The value of Zero traces the probe control inside the
 *          base libraries for all probes.
 *
 * OBJ(int,1) = <entry-number>
 * OBJ(int,2) = 1 = entry number exists
 *              0 = no entry number
 */
EOLS(debug_rttmon_trace_eol, rttmon_debug_command,
     DEBUG_RTTMON_TRACE);

SET(debug_rttmon_trace_entry_number,
    debug_rttmon_trace_eol, OBJ(int, 2), 1);

SET(debug_rttmon_trace_entry_no_number,
    debug_rttmon_trace_eol, OBJ(int, 2), 0);

NUMBER(debug_rttmon_trace_entry,
       debug_rttmon_trace_entry_number,
       debug_rttmon_trace_entry_no_number, OBJ(int, 1), 0,
       MAX_rttMonApplNumCtrlAdminEntry, "Entry Number");

KEYWORD(debug_rttmon_trace, debug_rttmon_trace_entry,
	debug_rttmon_error,
	D_rttMonDebugTrace_string,
	"Trace RTR", PRIV_OPR);

/******************************************************************
 * [no] (debug) rtr ...
 */

KEYWORD_DEBUG(debug_rttmon, debug_rttmon_trace, ALTERNATE,
	      OBJ(pdb, 1), rttmon_debug_arr,
	      D_rttMon_string,
	      "RTR Monitor Information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_rttmon
