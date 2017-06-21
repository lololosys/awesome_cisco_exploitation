/* $Id: cfg_state-mach.h,v 3.2.62.1 1996/08/12 16:08:27 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ts/cfg_state-mach.h,v $
 *------------------------------------------------------------------
 * C F G _ S T A T E - M A C H . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_state-mach.h,v $
 * Revision 3.2.62.1  1996/08/12  16:08:27  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:54:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:52  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:13:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * state-machine <name> <state> <firstchar> <lastchar>
 *		{{ <nextstate> | transmit } | [delay] }
 *
 * OBJ(string,1) = <name>
 * OBJ(int,1) = <state>
 * OBJ(int,2) = <firstchar>
 * OBJ(int,3) = <lastchar>
 * OBJ(int,4) = <nextstate>
 * OBJ(int,5) = TRUE if transmit, FALSE if not
 * OBJ(int,6) = TRUE if [delay], FALSE if not
 */
EOLNS	(cfg_statem_eol, statem_command);
KEYWORD_ID (cfg_statem_delay, cfg_statem_eol, cfg_statem_eol,
	 OBJ(int,6), TRUE,
	 "delay", "Transmit if no more input in 100ms", PRIV_CONF);
KEYWORD_ID (cfg_statem_transmit, cfg_statem_eol, no_alt,
	 OBJ(int,5), TRUE,
	 "transmit", "Transmit immediately", PRIV_CONF);
NUMBER	(cfg_statem_nextstate, cfg_statem_delay, cfg_statem_transmit,
	 OBJ(int,4), 0, MAXSTATE, "Next state");
NUMBER	(cfg_statem_lastchar, cfg_statem_nextstate, no_alt,
	 OBJ(int,3), 0, MAXBYTE, "Last character");
NUMBER	(cfg_statem_firstchar, cfg_statem_lastchar, no_alt,
	 OBJ(int,2), 0, MAXBYTE, "First character");
NUMBER	(cfg_statem_state, cfg_statem_firstchar, no_alt,
	 OBJ(int,1), 0, MAXSTATE, "Current state number");
NOPREFIX (cfg_statem_noprefix, cfg_statem_state, cfg_statem_eol);
STRING	(cfg_statem_name, cfg_statem_noprefix, no_alt,
	 OBJ(string,1), "Name of this state machine");
NVGENNS	(cfg_statem_nvgen, cfg_statem_name, statem_command);
KEYWORD	(cfg_statem_kwd, cfg_statem_nvgen, NONE,
	 "state-machine", "Define a TCP dispatch state machine", PRIV_CONF);

ASSERT	(cfg_statem, cfg_statem_kwd, ALTERNATE,
	 tty_statemQ);

#undef	ALTERNATE
#define	ALTERNATE	cfg_statem
