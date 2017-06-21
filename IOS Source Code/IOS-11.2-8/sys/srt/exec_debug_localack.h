/* $Id: exec_debug_localack.h,v 3.2 1995/11/17 18:53:31 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/srt/exec_debug_localack.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ L O C A L A C K . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_localack.h,v $
 * Revision 3.2  1995/11/17  18:53:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:04  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.3  1995/06/19  14:48:22  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/08  15:55:56  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  22:19:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug local-ack state
 */
EOLS	(debug_local_ack_state_eol, srt_debug_command, DEBUG_LACK_STATE);
KEYWORD (debug_local_ack_state, debug_local_ack_state_eol, no_alt,
	 "state", "Local ACKnowledgement state changes", PRIV_OPR);

/******************************************************************
 * debug local-ack packets
 */
EOLS	(debug_local_ack_packets_eol, srt_debug_command, DEBUG_LACK_PACKET);
KEYWORD (debug_local_ack_packets, debug_local_ack_packets_eol,
	 debug_local_ack_state,
	 "packets", "Local ACKnowledgement packets", PRIV_OPR);

/******************************************************************
 * debug local-ack errors
 */
EOLS	(debug_local_ack_errors_eol, srt_debug_command, DEBUG_LACK_ERR);
KEYWORD (debug_local_ack_errors, debug_local_ack_errors_eol,
	 debug_local_ack_packets,
	 "errors", "Local ACKnowledgement errors", PRIV_OPR);

/******************************************************************
 * debug local-ack
 */
KEYWORD_DEBUG(debug_local_ack, debug_local_ack_errors, ALTERNATE,
	      OBJ(pdb,1), srt_debug_arr,
	      "local-ack", "Local ACKnowledgement information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_local_ack
