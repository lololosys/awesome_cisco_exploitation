/* $Id: cfg_printer.h,v 3.1.40.1 1996/08/12 16:08:25 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ts/cfg_printer.h,v $
 *------------------------------------------------------------------
 * cfg_printer.h  Parser structures for lpd printers
 *
 * October 1994, Bill Westfield
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_printer.h,v $
 * Revision 3.1.40.1  1996/08/12  16:08:25  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1  1996/02/01  04:59:35  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1996/01/09  09:09:42  billw
 * CSCdi44091:  lpd should be able to output formfeed between jobs
 *
 * Revision 3.2  1995/11/17  18:44:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/21  18:17:11  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:11:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/***************************************************************
 * printer <name> LINE <n>|ROTARY <n> [JOBTYPES <word>]
 * no printer <name>
 *
 * OBJ(string,1) = <name>
 * OBJ(int,1) = line (or 0)
 * OBJ(int,2) = rotary (or 0)
 * OBJ(int,3) = 1 for newline conversion mode, 0 otherwise.
 * OBJ(string,2) = <word> (or null)
 * OBJ(int,4) = formfeed or 0
 * OBJ(int,5) = jobtimeout or 0
 * OBJ(int,6) = 1-3600 [timeout] or 0
 *
 */
pdecl(cfg_printer_types);

EOLNS	(cfg_printer_eol, printer_cfgcommand);

NUMBER (cfg_printer_timeout, cfg_printer_types, cfg_printer_eol,
	OBJ(int,6), 1, 3600, "timeout in seconds");

KEYWORD_ORTEST	(cfg_printer_jobtimeout,  cfg_printer_timeout, cfg_printer_eol, 
	 OBJ(int,5), TRUE,
	 "jobtimeout", "Change the default time for line acquisition",
	 PRIV_CONF);

KEYWORD_ORTEST (cfg_printer_ffeed, cfg_printer_types, cfg_printer_jobtimeout,
	    OBJ(int,4), TRUE,
	    "formfeed", "Add formfeed at end of job", PRIV_CONF);

KEYWORD_ORTEST (cfg_printer_transp,  cfg_printer_types, cfg_printer_ffeed,
	    OBJ(int,3), TRUE,
	    "newline-convert", "Convert unix newlines to CRLFs", PRIV_CONF);

STRING (cfg_printer_jobtypes, cfg_printer_eol, no_alt,
	OBJ(string,2), "Types of print job to accept");
KEYWORD (cfg_printer_types, cfg_printer_jobtypes, cfg_printer_transp,
	 "jobtypes", "Specify allowed job types", PRIV_CONF);

NUMBER (cfg_printer_rotno, cfg_printer_types, no_alt,
	OBJ(int,2), 1, 99, "rotary group number");
TTYLINE (cfg_printer_lineno, cfg_printer_types, no_alt,
	OBJ(int,1), "TTY number");

KEYWORD (cfg_printer_rotary, cfg_printer_rotno, no_alt,
	 "rotary", "Set rotary group of lines for printer", PRIV_CONF);
KEYWORD (cfg_printer_line, cfg_printer_lineno, cfg_printer_rotary,
	 "line", "Set TTY line for printer", PRIV_CONF);

NOPREFIX (cfg_printer_noprefix, cfg_printer_line, cfg_printer_eol);
STRING	(cfg_printer_name, cfg_printer_noprefix, no_alt,
	 OBJ(string,1), "Name of this printer");
NVGENNS	(cfg_printer_nvgen, cfg_printer_name, printer_cfgcommand);
KEYWORD	(cfg_printer_kwd, cfg_printer_nvgen, ALTERNATE,
	 "printer", "Define an LPD printer", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cfg_printer_kwd
