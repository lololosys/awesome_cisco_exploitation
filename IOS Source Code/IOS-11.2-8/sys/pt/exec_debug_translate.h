/* $Id: exec_debug_translate.h,v 3.2 1995/11/17 18:59:02 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/pt/exec_debug_translate.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ T R A N S L A T E . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_translate.h,v $
 * Revision 3.2  1995/11/17  18:59:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:00:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:56:30  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/19  14:49:20  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:20:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * debug translate
 *
 */
EOLS	(debug_translate_eol, debug_command, DEBUG_TRANSLATE);
KEYWORD_DEBUG(debug_translate, debug_translate_eol, ALTERNATE,
	      OBJ(pdb,1), pt_debug_arr,
	      "translate", "Protocol translation events", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_translate
