/* $Id: exec_debug_dspu.h,v 3.3 1996/02/01 23:33:09 widmer Exp $
 * $Source: /release/111/cvs/Xsys/dspu/exec_debug_dspu.h,v $
 *------------------------------------------------------------------
 * Dummy placeholder for Downstream PU (DSPU)
 *
 * April 94, Vivian Tseng
 *
 * Copyright (c) 1994-1996,1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_dspu.h,v $
 * Revision 3.3  1996/02/01  23:33:09  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  09:05:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:53:06  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.3  1995/06/19  14:47:56  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/08  15:55:41  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  22:18:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * debug dspu state <pu_name>
 *
 */
EOLS	(debug_dspu_state_eol, dspu_debug_command, DEBUG_DSPU_STATE);
SET	(debug_dspu_state_true, debug_dspu_state_eol, OBJ(int,1), TRUE);
STRING	(debug_dspu_state_name, debug_dspu_state_true, debug_dspu_state_eol,
	 OBJ(string, 1), "Host/Pu name");
KEYWORD	(debug_dspu_state, debug_dspu_state_name, no_alt,
	 "state", "DSPU state change", PRIV_OPR);


/*
 * debug dspu trace <pu_name>
 *
 */
EOLS	(debug_dspu_trace_eol, dspu_debug_command, DEBUG_DSPU_TRACE);
SET	(debug_dspu_trace_true, debug_dspu_trace_eol, OBJ(int,1), TRUE);
STRING	(debug_dspu_trace_name, debug_dspu_trace_true, debug_dspu_trace_eol,
	 OBJ(string, 1), "Host/Pu name");
KEYWORD (debug_dspu_trace, debug_dspu_trace_name, debug_dspu_state,
	 "trace", "DSPU FSM trace", PRIV_OPR);


/*
 * debug dspu packet <pu_name>
 *
 */
EOLS	(debug_dspu_packet_eol, dspu_debug_command, DEBUG_DSPU_PACKET);
SET	(debug_dspu_packet_true, debug_dspu_packet_eol, OBJ(int,1), TRUE);
STRING	(debug_dspu_packet_name, debug_dspu_packet_true,
	 debug_dspu_packet_eol,
	 OBJ(string, 1), "Host/Pu name");
KEYWORD	(debug_dspu_packet, debug_dspu_packet_name, debug_dspu_trace,
	 "packet", "DSPU packet", PRIV_OPR);


/*
 * debug dspu activation <pu_name>
 *
 */
EOLS	(debug_dspu_activation_eol, dspu_debug_command, DEBUG_DSPU_ACTIVATION);
SET	(debug_dspu_activation_true, debug_dspu_activation_eol,
	 OBJ(int,1), TRUE);
STRING	(debug_dspu_activation_name, debug_dspu_activation_true,
	 debug_dspu_activation_eol,
	 OBJ(string, 1), "Host/Pu name");
KEYWORD	(debug_dspu_activation, debug_dspu_activation_name,
	 debug_dspu_packet, "activation", "DSPU activation", PRIV_OPR);


/******************************************************************
 */

KEYWORD_DEBUG(debug_dspu, debug_dspu_activation, ALTERNATE,
	      OBJ(pdb,1), dspu_debug_arr,
	      "dspu", "DSPU Information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_dspu
