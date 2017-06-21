/* $Id: exec_xremote.h,v 3.2.62.1 1996/08/12 16:10:11 widmer Exp $
 * $Source: /release/112/cvs/Xsys/xremote/exec_xremote.h,v $
 *------------------------------------------------------------------
 * E X E C _ X R E M O T E . H
 * 
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_xremote.h,v $
 * Revision 3.2.62.1  1996/08/12  16:10:11  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  19:26:33  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:01:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:44:44  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:28:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 *
 */
EOLNS	(exec_xremote_eol, xremote_exec_command);
SET	(exec_xremote_set, exec_xremote_eol,
	 OBJ(int,1), PARSER_XREMOTE_DEFAULT);

IPADDR_NAME(exec_xremote_xtrek_host, exec_xremote_eol, no_alt,
	    OBJ(paddr,1), "Xtrek server hostname or IP address");
KEYWORD_ID(exec_xremote_xtrek, exec_xremote_xtrek_host, exec_xremote_set,
	   OBJ(int,1), PARSER_XREMOTE_XTREK,
	   "xtrek", NULL, PRIV_USER | PRIV_HIDDEN);

IPADDR_NAME(exec_xremote_xdm_host, exec_xremote_eol, exec_xremote_eol,
	    OBJ(paddr,1), "XDM server hostname or IP address");
KEYWORD_ID(exec_xremote_xdm, exec_xremote_xdm_host, exec_xremote_xtrek,
	 OBJ(int,1), PARSER_XREMOTE_XDM,
	 "xdm", "Start up a XDM session", PRIV_USER);

STRING	(exec_xremote_lat_service, exec_xremote_eol, no_alt,
	 OBJ(string,1), "LAT service"); 
KEYWORD_ID(exec_xremote_lat, exec_xremote_lat_service, exec_xremote_xdm,
	   OBJ(int,1), PARSER_XREMOTE_LAT,
	   "lat", "Start up a LAT session", PRIV_USER);

KEYWORD_ID(exec_xremote_font, exec_xremote_eol, exec_xremote_lat,
	   OBJ(int,1), PARSER_XREMOTE_FONT,
	   "font", NULL, PRIV_ROOT | PRIV_HIDDEN);

KEYWORD_ID(exec_xremote_disable, exec_xremote_eol, NONE,
	   OBJ(int, 1), PARSER_XREMOTE_DISABLE,
	   "disable", "Disable XRemote mode", PRIV_USER);

ASSERT (exec_xremote_disable_test, exec_xremote_disable, exec_xremote_font,
	(stdio->xrcb != NULL));

KEYWORD	(exec_xremote, exec_xremote_disable_test, NONE,
	 "xremote", "Enter XRemote mode", PRIV_USER);


TEST_BOOLEAN(exec_xremote_test, exec_xremote, NONE, ALTERNATE,
	     xremote_enable);

#undef	ALTERNATE
#define	ALTERNATE	exec_xremote_test
