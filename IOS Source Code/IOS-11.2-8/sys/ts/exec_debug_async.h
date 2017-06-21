/* $Id: exec_debug_async.h,v 3.2 1995/11/17 18:54:27 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ts/exec_debug_async.h,v $
 *------------------------------------------------------------------
 * exec_debug_async.h - Parse chain for "debug async"
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_async.h,v $
 * Revision 3.2  1995/11/17  18:54:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:39:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:55  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/19  14:47:13  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:17:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



/************************************************************
 * debug async state
 */

EOLS    (debug_async_state_eol, debug_command, DEBUG_ASYNC_STATE);
KEYWORD (debug_async_state, debug_async_state_eol, no_alt,
	 "state", "Interface state changes", PRIV_OPR);

/************************************************************
 * debug async packet
 */

EOLS    (debug_async_packet_eol, debug_command, DEBUG_ASYNC_PACKET);
KEYWORD (debug_async_packet, debug_async_packet_eol, debug_async_state,
	 "packet", "Packet I/O", PRIV_OPR);


/************************************************************
 * debug async framing
 */

EOLS    (debug_async_framing_eol, debug_command, DEBUG_ASYNC_FRAMING);
KEYWORD (debug_async_framing, debug_async_framing_eol, debug_async_packet,
	 "framing", "Packet framing", PRIV_OPR);


/************************************************************
 * debug async
 */

KEYWORD_DEBUG(debug_async, debug_async_framing, ALTERNATE,
	      OBJ(pdb,1), async_debug_arr,
	      "async", "Async interface information", PRIV_OPR);

#undef ALTERNATE
#define ALTERNATE	debug_async
