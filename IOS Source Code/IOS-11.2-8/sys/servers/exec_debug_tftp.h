/* $Id: exec_debug_tftp.h,v 3.2 1995/11/17 18:51:21 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/servers/exec_debug_tftp.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ T F T P . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_debug_tftp.h,v $
 * Revision 3.2  1995/11/17  18:51:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:56:46  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.2  1995/06/19  14:49:16  widmer
 * CSCdi36074:  Clean up debug parse chain alternates
 * Add keyword_debug macro to assign debug array
 *
 * Revision 2.1  1995/06/07  22:20:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * debug tftp
 *
 */
EOLS	(debug_tftp_eol, debug_command, DEBUG_TFTP);
KEYWORD_DEBUG(debug_tftp, debug_tftp_eol, ALTERNATE,
	      OBJ(pdb,1), tftp_debug_arr,
	      "tftp", "TFTP packets", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_tftp
