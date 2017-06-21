/* $Id: exec_clear_interface.h,v 3.3.28.3 1996/08/12 16:08:45 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ui/exec_clear_interface.h,v $
 *------------------------------------------------------------------
 * E X E C _ C L E A R _ I N T E R F A C E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_clear_interface.h,v $
 * Revision 3.3.28.3  1996/08/12  16:08:45  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.28.2  1996/05/21  10:07:46  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3.28.1  1996/04/27  06:29:20  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.2.20.2  1996/04/16  23:49:55  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.2.20.1  1996/03/01  22:27:09  perryl
 * Branch: Dial1_branch
 * Disallow UI configuration for virtual access interface.
 * Allow show and clear command for virtual access interface.
 * Disallow clear command for virtual template interface.
 *
 * Revision 3.3  1996/01/18  02:15:36  tli
 * CSCdi47063:  show ip cache cannot be restricted to subinterfaces
 * Change IFTYPE_ANY to IFTYPE_HWIDB
 *
 * Revision 3.2  1995/11/17  17:47:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:40:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:59:22  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:15:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * clear interface <interface-name>
 *
 * csb->if_name = <interface-name>
 */
EOLNS	(clear_if_eol, clear_intcommand);
INTERFACE (clear_if_name, clear_if_eol, no_alt,
	   OBJ(idb,1), ((IFTYPE_HWIDB | IFTYPE_VACCESS) & ~IFTYPE_VTEMPLATE));
KEYWORD (clear_interface, clear_if_name, ALTERNATE,
	 common_str_interface, "Clear the hardware logic on an interface",
	 PRIV_OPR);


#undef	ALTERNATE
#define	ALTERNATE	clear_interface
