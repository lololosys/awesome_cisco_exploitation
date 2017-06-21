/* $Id: exec_debug_smf.h,v 3.2 1995/11/17 18:52:14 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/smf/exec_debug_smf.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ S M F . H
 *
 * May 1995, Tony Speakman
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_smf.h,v $
 * Revision 3.2  1995/11/17  18:52:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/29  01:10:46  mordock
 * CSCdi37882:  currekt missed-spelled wurds
 * Corrected spelling mistakes in various messages.
 *
 * Revision 2.1  1995/07/04  01:56:53  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/19  14:48:55  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:20:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug smf
 *
 */
EOLS	(debug_smf_miss_eol, smf_debug_command, DEBUG_SMF_MISS);
KEYWORD (debug_smf_miss, debug_smf_miss_eol, no_alt,
	 "misses", "address misses", PRIV_OPR|PRIV_HIDDEN);
EOLS	(debug_smf_match_eol, smf_debug_command, DEBUG_SMF_MATCH);
KEYWORD (debug_smf_match, debug_smf_match_eol, debug_smf_miss,
	 "matches", "address matches", PRIV_OPR|PRIV_HIDDEN);
EOLS	(debug_smf_update_eol, smf_debug_command, DEBUG_SMF_UPDATE);
KEYWORD (debug_smf_update, debug_smf_update_eol, debug_smf_match,
	 "updates", "address insertions and deletions", PRIV_OPR);

KEYWORD_DEBUG(debug_smf, debug_smf_update, ALTERNATE,
	      OBJ(pdb,1), smf_debug_arr,
	      "smf", "Software MAC filter", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_smf
