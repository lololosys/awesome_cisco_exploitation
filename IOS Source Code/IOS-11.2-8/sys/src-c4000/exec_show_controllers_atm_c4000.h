/* $Id: exec_show_controllers_atm_c4000.h,v 3.2.62.1 1996/08/12 16:07:28 widmer Exp $
 * $Source: /release/112/cvs/Xsys/src-c4000/exec_show_controllers_atm_c4000.h,v $
 *------------------------------------------------------------------
 * ATMizer show controllers commands
 *
 * January 1995, Walter Friedrich
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: exec_show_controllers_atm_c4000.h,v $
 * Revision 3.2.62.1  1996/08/12  16:07:28  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:45:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:26:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:57:33  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:22:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * show controllers atm [[<unit number>] [memory <addr> <length>]
 *
 */
EOLNS	(show_control_atm_eol, show_controllers);
NUMBER  (show_control_atm_mem_lth, show_control_atm_eol, no_alt,
	 OBJ(int,5), 1, 1024,
	 NULL /*"ATMizer Private Memory Dump size (bytes)"*/);
NUMBER  (show_control_atm_mem_addr, show_control_atm_mem_lth, no_alt,
	 OBJ(int,4), 0, MAXUINT-1,
	 NULL /*"Interface private Memory dump start address"*/);
KEYWORD (show_control_atm_mem, show_control_atm_mem_addr, show_control_atm_eol,
        "memory", NULL /*"Interface private memory dump"*/,
	 PRIV_USER | PRIV_HIDDEN);
UNIT_NUMBER (show_control_atm_unit, show_control_atm_mem, show_control_atm_eol,
        OBJ(idb,1),  IFTYPE_ATM);
KEYWORD_ID(show_control_atm_kwd, show_control_atm_unit, no_alt,
        OBJ(int,1), IDB_ATM, "atm", "Show ATM controllers", PRIV_USER);
TEST_INT (show_control_atm, show_control_atm_kwd, NONE, ALTERNATE,
        natmnets);

#undef	ALTERNATE
#define	ALTERNATE	show_control_atm
