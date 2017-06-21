/* $Id: cfg_line_transport.h,v 3.2 1995/11/17 18:45:33 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/os/cfg_line_transport.h,v $
 *------------------------------------------------------------------
 * C F G _ L I N E _ T R A N S P O R T . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_line_transport.h,v $
 * Revision 3.2  1995/11/17  18:45:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/25  00:03:08  che
 * CSCdi28688:  Security tightening on product
 * connect.c:
 * Forces "transport input all" to be written out during a NVGEN if
 * tty->transport_allowedin = CONN_TRANSPORT_DEFMASK(default).
 *
 * cfg_line_transport.h:
 * Introduces new configuration command:
 * "transport { input | output } all"
 *
 * Revision 2.1  1995/07/02  10:51:02  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:10:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * terminal transport { telnet | lat | pad | none | rlogin }
 * [terminal] transport perferrred { telnet | lat | pad | none | rlogin }
 * transport { input | output } { telnet | lat | pad | none | all | rlogin } [...]
 */

#include <connect.h>

EOLS	(line_transport_eol, transport_cmd, PARSER_LINE_TRANSPORT);

KEYWORD_ID(line_transport_all, line_transport_eol, NONE,
	OBJ(int, 2), CONN_TRANSPORT_DEFMASK,
	"all", "All protocols", PRIV_USER);

KEYWORD_ID(line_transport_none, line_transport_eol, line_transport_all,
	 OBJ(int, 2), 0,
	 "none", "No protocols", PRIV_USER);
	   
/*
 * Parse a list of transports here
 */

TRANSPORT (line_transport_name, line_transport_eol,
  	   line_transport_none, OBJ(int, 2), FALSE);
  
/*
 * Test for the NO case here
 */

TESTVAR	(line_transport_no_test, line_transport_eol,
	 line_transport_name,
	 NONE, NONE, NONE,
	 sense, FALSE);

/*
 * Stop here if doing NV generation
 */

NVGENS	(line_transport_protocols, line_transport_no_test,
	 transport_cmd, PARSER_LINE_TRANSPORT);
  
/*
 * Check for the "output" keyword
 */
KEYWORD_ID(line_transport_out_kwd, line_transport_protocols, NONE,
	   OBJ(int,10), TELNET_TRANSPORT_OUT,
	   "output",
	   "Define which protocols to use for outgoing connections",
	   PRIV_USER);
	 
/*
 * Check for the "input" keyword
 */

KEYWORD_ID(line_transport_in_kwd, line_transport_protocols,
	   line_transport_out_kwd,
	   OBJ(int,10), TELNET_TRANSPORT_IN,
	   "input",
	   "Define which protocols to use when connecting "
	   "to the terminal server",
	   PRIV_USER);

/*
 * Handle the "none" keyword
 */
/*
 * Parse exactly one transport name and get out
 */

TRANSPORT (line_transport_one_protocol, line_transport_eol,
	   line_transport_none, OBJ(int, 2), TRUE);

/*
 * Test for the NO case here
 */

TESTVAR	(line_transport_one_notest, line_transport_eol,
	 line_transport_one_protocol,
	 NONE, NONE, NONE,
	 sense, FALSE);


/*
 * Stop here if doing NV generation
 */

NVGENS (line_transport_one_protocol_test, line_transport_one_notest,
	transport_cmd, PARSER_LINE_TRANSPORT);

/*
 * Mark that the "preferred" option was selected
 */

SET	(line_transport_prefer_set, line_transport_one_protocol_test,
	 OBJ(int,10), TELNET_TRANSPORT_PREFERRED);

/*
 * Check for the "preferred" keyword
 */

KEYWORD(line_transport_prefer, line_transport_prefer_set,
	line_transport_in_kwd,
	"preferred", "Specify the preferred protocol to use", PRIV_USER);

/*
 * If this is a "terminal" command, force the user into the "preferred"
 * suboption.
 */

TESTVAR (line_transport_permtest, line_transport_prefer_set, 
	 line_transport_prefer, NONE, NONE, NONE,
	 permanent, FALSE);

/*
 * Check to see if we are doing NV generation, and skip the "terminal
 * command" check if so
 */


TESTVAR (line_transport_nvtest, line_transport_permtest,
	 line_transport_prefer, NONE, NONE,
	 NONE, nvgen, FALSE);

/*
 * Match the transport command
 */

KEYWORD	(line_transport, line_transport_nvtest, ALTERNATE,
	 "transport", "Define transport protocols for line", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	line_transport
