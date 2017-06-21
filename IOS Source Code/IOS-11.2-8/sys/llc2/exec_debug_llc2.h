/* $Id: exec_debug_llc2.h,v 3.3 1996/02/28 01:02:43 kchiu Exp $
 * $Source: /release/111/cvs/Xsys/llc2/exec_debug_llc2.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ L L C 2 . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_llc2.h,v $
 * Revision 3.3  1996/02/28  01:02:43  kchiu
 * CSCdi48822:  llc2 windowing mechanism doesn't work properly under
 * congestion
 *
 * Revision 3.2  1995/11/17  17:43:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:35:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:54:37  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.3  1995/06/19  14:48:21  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.2  1995/06/08  15:55:54  widmer
 * CSCdi35497:  Remove old style debugging
 *
 * Revision 2.1  1995/06/07  22:19:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * debug llc2 dynwind
 *
 */
EOLS	(debug_llc2_dynwind_eol, debug_command, DEBUG_LLC2_DYNWIND);
KEYWORD (debug_llc2_dynwind, debug_llc2_dynwind_eol, no_alt,
	 "dynwind", "LLC2 (FRAS) dynamic window", PRIV_OPR);

/*
 * debug llc2 packets
 *
 */
EOLS	(debug_llc2_packets_eol, debug_command, DEBUG_LLC2_PACKET);
KEYWORD (debug_llc2_packets, debug_llc2_packets_eol, debug_llc2_dynwind,
	 "packets", "LLC2 type II packets", PRIV_OPR);

/*
 * debug llc2 errors
 *
 */
EOLS	(debug_llc2_err_eol, debug_command, DEBUG_LLC2_ERROR);
KEYWORD (debug_llc2_err, debug_llc2_err_eol, debug_llc2_packets,
	 "errors", "LLC2 type II errors", PRIV_OPR);

/*
 * debug llc2 state
 *
 */
EOLS	(debug_llc2_state_eol, debug_command, DEBUG_LLC2_STATE);
KEYWORD (debug_llc2_state, debug_llc2_state_eol, debug_llc2_err,
	 "state", "LLC2 type II state changes", PRIV_OPR);

/*
 * debug llc2
 *
 */
KEYWORD_DEBUG(debug_llc2, debug_llc2_state, ALTERNATE,
	      OBJ(pdb,1), llc2_debug_arr,
	      "llc2", "LLC2 type II Information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_llc2
