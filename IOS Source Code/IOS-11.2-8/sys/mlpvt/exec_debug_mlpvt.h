/* $Id: exec_debug_mlpvt.h,v 1.1.42.2 1996/05/18 22:34:42 perryl Exp $
 * $Source: /release/112/cvs/Xsys/mlpvt/exec_debug_mlpvt.h,v $
 *------------------------------------------------------------------
 * exec_debug_mlpvt.h -- MLPVT debug commands
 *
 * January 1996, Ashwin Baindur
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_mlpvt.h,v $
 * Revision 1.1.42.2  1996/05/18  22:34:42  perryl
 * CSCdi57939:  MLPVT is a bad name for Stack Group Bidding Protocol
 * Branch: California_branch
 * Rename it to SGBP, Stack Group Bidding Protocol
 *
 * Revision 1.1.42.1  1996/04/27  06:34:15  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.1.2.1  1996/01/26  20:28:35  agh
 * Branch: Dial1_branch
 * Initial Code Commit for MLPVT.
 *
 * Revision 1.1  1996/01/26  19:22:29  abaindur
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug mlpvt group operations
 *
 */
EOLS	(debug_mlpvt_errors_eol, debug_command, DEBUG_MLPVT_ERRORS);
KEYWORD (debug_mlpvt_errors, debug_mlpvt_errors_eol, no_alt,
	 "errors", "SGBP errors", PRIV_OPR);

EOLS	(debug_mlpvt_msgs_eol, debug_command, DEBUG_MLPVT_MSGS);
KEYWORD (debug_mlpvt_msgs, debug_mlpvt_msgs_eol, debug_mlpvt_errors,
	 "messages", "SGBP messages", PRIV_OPR);

EOLS	(debug_mlpvt_queries_eol, debug_command, DEBUG_MLPVT_QUERIES);
KEYWORD (debug_mlpvt_queries, debug_mlpvt_queries_eol, debug_mlpvt_msgs,
	 "queries", "SGBP mastership queries", PRIV_OPR);

EOLS	(debug_mlpvt_events_eol, debug_command, DEBUG_MLPVT_EVENTS);
KEYWORD (debug_mlpvt_events, debug_mlpvt_events_eol,
	 debug_mlpvt_queries,
	 "events", "SGBP events", PRIV_OPR);

EOLS	(debug_mlpvt_hello_eol, debug_command, DEBUG_MLPVT_HELLOS);
KEYWORD (debug_mlpvt_hello, debug_mlpvt_hello_eol, debug_mlpvt_events,
	 "hellos", "SGBP connection hellos", PRIV_OPR);

KEYWORD_DEBUG(debug_mlpvt, debug_mlpvt_hello, ALTERNATE,
	      OBJ(pdb,1), mlpvt_debug_arr,
	      "sgbp", "SGBP debugging",
	      PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_mlpvt
