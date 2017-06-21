/* $Id: exec_debug_lex.h,v 3.2 1995/11/17 17:42:47 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lex/exec_debug_lex.h,v $
 *------------------------------------------------------------------
 * EXEC_DEBUG_LEX.H
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_lex.h,v $
 * Revision 3.2  1995/11/17  17:42:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:34:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:54:31  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/19  14:48:18  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:19:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * debug lex rcmd
 */

EOLS    (debug_lex_rcmd_eol, debug_command, DEBUG_LEX_RCMD);
KEYWORD (debug_lex_rcmd, debug_lex_rcmd_eol, no_alt,
	 "rcmd", "LEX Remote Commands", PRIV_OPR);

/******************************************************************
 * debug lex
 */

KEYWORD_DEBUG(debug_lex, debug_lex_rcmd, ALTERNATE,
	      OBJ(pdb,1), lex_debug_arr,
	      "lex", "LAN Extender protocol", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_lex

