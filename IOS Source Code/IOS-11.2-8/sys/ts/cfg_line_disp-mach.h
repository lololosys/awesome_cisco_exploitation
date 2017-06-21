/* $Id: cfg_line_disp-mach.h,v 3.2.62.1 1996/08/12 16:08:23 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ts/cfg_line_disp-mach.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ D I S P - M A C H . H
 *
 * dispatch-machine
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_disp-mach.h,v $
 * Revision 3.2.62.1  1996/08/12  16:08:23  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:53:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:44  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:09:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * dispatch-machine <machine-name>
 *
 * OBJ(string,1) = <machine-name>
 */
EOLNS	(cfg_dispmach_eol, tty_disstate_command);
STRING	(cfg_dispmach_name, cfg_dispmach_eol, no_alt,
	 OBJ(string,1), "Name of a defined state machine");
NOPREFIX (cfg_dispmach_noprefix, cfg_dispmach_name, cfg_dispmach_eol);
NVGENNS	(cfg_dispmach_nvgen, cfg_dispmach_noprefix, tty_disstate_command);
KEYWORD	(cfg_dispmach_kwd, cfg_dispmach_nvgen, NONE,
	 "dispatch-machine", "Reference a TCP dispatch state machine",
	 PRIV_CONF);

ASSERT	(cfg_dispmach, cfg_dispmach_kwd, ALTERNATE,
	 tty_statemQ);

#undef	ALTERNATE
#define	ALTERNATE	cfg_dispmach
