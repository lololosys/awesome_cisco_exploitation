/* $Id: cfg_modemcap.h,v 3.1.40.4 1996/08/08 17:56:40 sjackson Exp $
 * $Source: /release/112/cvs/Xsys/ts/cfg_modemcap.h,v $
 *------------------------------------------------------------------
 * cfg_modemcap.h  -- parser stuff for global modemcap command
 *
 * June 1995, Tim Kolar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_modemcap.h,v $
 * Revision 3.1.40.4  1996/08/08  17:56:40  sjackson
 * CSCdi65288:  Modem autoconfig should always redetect and attempt to
 * lock speed
 * Branch: California_branch
 * Modem locking speed is now configureable which prevents changing of the
 * desired line speed. Configuration is also attempted after every hangup.
 *
 * Revision 3.1.40.3  1996/07/03  18:17:59  sjackson
 * CSCdi61825:  Modemcap edit should allow a no configuration
 * Branch: California_branch
 *
 * Revision 3.1.40.2  1996/06/16  21:20:01  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1.40.1  1996/05/07  18:07:38  snyder
 * CSCdi56844:  spellink errors
 * Branch: California_branch
 *
 * miscllaneous -> miscellaneous
 * Capabilties -> Capabilities
 *
 * Revision 3.1  1996/02/01  04:59:34  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:44:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:20:15  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/26  01:07:43  tkolar
 * *** empty log message ***
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*************************************************************************
 *
 * modemcap edit <modem name> <which attribute> <new value>
 *
 */

EOLS	(cl_modemcap_edit_eol, modemcap_command, PARSER_MODEMCAP_EDIT);
STRING (cl_modemcap_edit_string, cl_modemcap_edit_eol, no_alt,
	OBJ(string,2), "Modem string for this attribute");

NOPREFIX (cl_modemcap_edit_no, cl_modemcap_edit_string,
	cl_modemcap_edit_eol);

KEYWORD_ID (cl_modemcap_edit_tpl, cl_modemcap_edit_no, 
	no_alt, OBJ(int, 1), MODEMCAP_TEMPLATE,
	"template", "Specify modemcap entry to use as a template", 
								PRIV_CONF);

KEYWORD_ID (cl_modemcap_edit_fd, cl_modemcap_edit_no, 
	cl_modemcap_edit_tpl, OBJ(int,1), MODEMCAP_FACDEFAULTS,
	"factory-default", "Edit entry for factory default", PRIV_CONF);

KEYWORD_ID (cl_modemcap_edit_aa, cl_modemcap_edit_no, 
	cl_modemcap_edit_fd, OBJ(int,1), MODEMCAP_AUTOANSWER,
	"autoanswer", "Edit entry for autoanswer", PRIV_CONF);

KEYWORD_ID (cl_modemcap_edit_cd, cl_modemcap_edit_no, 
	cl_modemcap_edit_aa, OBJ(int,1), MODEMCAP_CARDETECT,
	"carrier-detect", "Edit entry for carrier-detect", PRIV_CONF);

KEYWORD_ID (cl_modemcap_edit_dtr, cl_modemcap_edit_no, 
	cl_modemcap_edit_cd, OBJ(int,1), MODEMCAP_DTR,
	"dtr", "Edit entry for DTR", PRIV_CONF);

KEYWORD_ID (cl_modemcap_edit_hfl, cl_modemcap_edit_no, 
	cl_modemcap_edit_dtr, OBJ(int,1), MODEMCAP_HARDFLOW,
	"hardware-flowcontrol", "Edit entry for hardware flowcontrol", 
								PRIV_CONF);

STRING (cl_modemcap_edit_spd_string, cl_modemcap_edit_eol, no_alt,
	OBJ(string,2), "String to lock modem DTE speed");
NUMBER (cl_modemcap_edit_spd_number, cl_modemcap_edit_spd_string, no_alt,
	OBJ(int,2), 300, 115200, "Speed modem DTE will be locked at");
NOPREFIX (cl_modemcap_edit_spd_no, cl_modemcap_edit_spd_number,
	cl_modemcap_edit_eol);
KEYWORD_ID (cl_modemcap_edit_speed, cl_modemcap_edit_spd_no, 
	cl_modemcap_edit_hfl, OBJ(int,1), MODEMCAP_SPEED,
	 "speed", "Edit entries for locking modem speed", PRIV_CONF);

KEYWORD_ID (cl_modemcap_edit_ber, cl_modemcap_edit_no, 
	cl_modemcap_edit_speed, OBJ(int,1), MODEMCAP_BESTERR,
	"best-error-control", "Edit entry for best error control", PRIV_CONF);

KEYWORD_ID (cl_modemcap_edit_bcp, cl_modemcap_edit_no, 
	cl_modemcap_edit_ber, OBJ(int,1), MODEMCAP_BESTCOMP,
	"best-compression", "Edit entry for best compression", PRIV_CONF);

KEYWORD_ID (cl_modemcap_edit_ner, cl_modemcap_edit_no, 
	cl_modemcap_edit_bcp, OBJ(int,1), MODEMCAP_NOERR,
	"no-error-control", "Edit entry for no error control", PRIV_CONF);

KEYWORD_ID (cl_modemcap_edit_ncp, cl_modemcap_edit_no, 
	cl_modemcap_edit_ner, OBJ(int,1), MODEMCAP_NOCOMP,
	"no-compression", "Edit entry for no compression", PRIV_CONF);

KEYWORD_ID (cl_modemcap_edit_nec, cl_modemcap_edit_no, 
	cl_modemcap_edit_ncp, OBJ(int,1), MODEMCAP_NOECHO,
	"no-echo", "Edit entry for no echo", PRIV_CONF);

KEYWORD_ID (cl_modemcap_edit_nrs, cl_modemcap_edit_no, 
	cl_modemcap_edit_nec, OBJ(int,1), MODEMCAP_NORESULT,
	"no-results", "Edit entry for no results (quiet mode)", PRIV_CONF);

KEYWORD_ID (cl_modemcap_edit_sfl, cl_modemcap_edit_no, 
	cl_modemcap_edit_nrs, OBJ(int,1), MODEMCAP_SOFTFLOW,
	"software-flowcontrol", "Edit entry for software flowcontrol", PRIV_CONF);

KEYWORD_ID (cl_modemcap_edit_cid, cl_modemcap_edit_no, 
	cl_modemcap_edit_sfl, OBJ(int,1), MODEMCAP_CALLID,
	"caller-id", "Edit entry for Caller ID", PRIV_CONF);

KEYWORD_ID (cl_modemcap_edit_msc, cl_modemcap_edit_no, 
	cl_modemcap_edit_cid, OBJ(int,1), MODEMCAP_MISC,
	"miscellaneous", "Edit entry for miscellaneous commands", PRIV_CONF);

STRING (cl_modemcap_edit_name, cl_modemcap_edit_msc, no_alt,
	OBJ(string,1), "Modem type");

NVGENS (cl_modemcap_edit_nvgens, cl_modemcap_edit_name,
	modemcap_command, PARSER_MODEMCAP_EDIT);

KEYWORD	(cl_modemcap_edit, cl_modemcap_edit_nvgens, no_alt,
	 "edit", "Edit a modemcap entry", PRIV_CONF);


/*************************************************************************
 *
 * modemcap entry <modemcap string>
 *
 */

EOLS	(cl_modemcap_entry_eol, modemcap_command, PARSER_MODEMCAP_ENTRY);

TEXT (cl_modemcap_entry_text, cl_modemcap_entry_eol, no_alt,
	OBJ(string,2), "Modemcap entry for this modem");

NVGENS (cl_modemcap_entry_nvgens, cl_modemcap_entry_text,
	modemcap_command, PARSER_MODEMCAP_ENTRY);

KEYWORD	(cl_modemcap_entry, cl_modemcap_entry_nvgens, cl_modemcap_edit,
	 "entry", "Give a full modemcap entry", PRIV_CONF);

/*************************************************************************/

KEYWORD_MM (cl_modemcap, cl_modemcap_entry, ALTERNATE,
	    "modemcap", "Modem Capabilities database", PRIV_CONF, 6);

#undef	ALTERNATE
#define	ALTERNATE	cl_modemcap
