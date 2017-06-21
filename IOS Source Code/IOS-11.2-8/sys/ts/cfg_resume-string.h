/* $Id: cfg_resume-string.h,v 3.2.62.1 1996/08/12 16:08:26 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ts/cfg_resume-string.h,v $
 *------------------------------------------------------------------
 * resume-string command
 *
 * Feb 1995, Bill Westfield
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_resume-string.h,v $
 * Revision 3.2.62.1  1996/08/12  16:08:26  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:54:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:58:52  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:12:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * resume-string <hostname> <char> <text> <char>
 * no resume-string <hostname> [<char> <text> <char>]
 * 
 * OBJ(string,1) = <char> <text> <char>
 * OBJ(string,2) = <hostname>
 */

EOLNS	(cfg_resumestr_eol, set_resumestring);
TEXT	(cfg_resumestr_char, cfg_resumestr_eol, no_alt,
	 OBJ(string,1), "c resume-string c, where 'c' is a delimiting character");

NOPREFIX (cfg_resumestr_noprefix, cfg_resumestr_char, cfg_resumestr_eol);

STRING	(cfg_resumestr_host, cfg_resumestr_noprefix, no_alt,
	 OBJ(string,2), "Name of the host for this resume string");
NVGENNS	(cfg_resumestr_nvgen, cfg_resumestr_host, set_resumestring);

/******************************************************************/
KEYWORD	(cfg_resumestr, cfg_resumestr_nvgen, ALTERNATE,
	 "resume-string", "Define a host-specific resume string", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cfg_resumestr
