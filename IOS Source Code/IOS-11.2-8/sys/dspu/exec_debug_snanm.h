/* $Id: exec_debug_snanm.h,v 3.3 1996/02/01 23:33:10 widmer Exp $
 * $Source: /release/111/cvs/Xsys/dspu/exec_debug_snanm.h,v $
 *------------------------------------------------------------------
 * SNA Network Management Debug 
  *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_snanm.h,v $
 * Revision 3.3  1996/02/01  23:33:10  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  09:05:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:53:07  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/19  14:48:56  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:20:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * debug sna state <pu_name>
 *
 */
EOLS	(debug_sna_state_eol, sna_debug_command, DEBUG_SNA_STATE);
SET	(debug_sna_state_true, debug_sna_state_eol, OBJ(int,1), TRUE);
STRING	(debug_sna_state_name, debug_sna_state_true, debug_sna_state_eol,
	 OBJ(string, 1), "Host/Pu name");
KEYWORD	(debug_sna_state, debug_sna_state_name, no_alt,
	 "state", "SNA state change", PRIV_OPR);


/*
 * debug sna trace <pu_name>
 *
 */
EOLS	(debug_sna_trace_eol, sna_debug_command, DEBUG_SNA_TRACE);
SET	(debug_sna_trace_true, debug_sna_trace_eol, OBJ(int,1), TRUE);
STRING	(debug_sna_trace_name, debug_sna_trace_true, debug_sna_trace_eol,
	 OBJ(string, 1), "Host/Pu name");
KEYWORD	(debug_sna_trace, debug_sna_trace_name, debug_sna_state,
	 "trace", "SNA FSM trace", PRIV_OPR);


/*
 * debug sna packet <pu_name>
 *
 */
EOLS	(debug_sna_packet_eol, sna_debug_command, DEBUG_SNA_PACKET);
SET	(debug_sna_packet_true, debug_sna_packet_eol, OBJ(int,1), TRUE);
STRING	(debug_sna_packet_name, debug_sna_packet_true, debug_sna_packet_eol,
	 OBJ(string, 1), "Host/Pu name");
KEYWORD	(debug_sna_packet, debug_sna_packet_name, debug_sna_trace,
	 "packet", "SNA packet", PRIV_OPR);


/*
 * debug sna activation <pu_name>
 *
 */
EOLS	(debug_sna_activation_eol, sna_debug_command, DEBUG_SNA_ACTIVATION);
SET	(debug_sna_activation_true, debug_sna_activation_eol,
	 OBJ(int,1), TRUE);
STRING	(debug_sna_activation_name, debug_sna_activation_true,
	 debug_sna_activation_eol,
	 OBJ(string, 1), "Host/Pu name");
KEYWORD	(debug_sna_activation, debug_sna_activation_name, debug_sna_packet,
	 "activation", "SNA activation", PRIV_OPR);

/******************************************************************
 */

KEYWORD	(debug_sna, debug_sna_activation, ALTERNATE,
	 "sna", "SNA Information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_sna
