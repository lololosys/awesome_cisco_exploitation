/* $Id: cfg_arap.h,v 3.2.60.1 1996/09/10 18:51:25 asb Exp $
 * $Source: /release/112/cvs/Xsys/arap/cfg_arap.h,v $
 *------------------------------------------------------------------
 * ARAP Config commands
 *
 * January 1994, Tim Kolar
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * $Log: cfg_arap.h,v $
 * Revision 3.2.60.1  1996/09/10  18:51:25  asb
 * CSCdi68276:  ARAP:  arap logging command requires debugging extensions
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:39:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  09:34:51  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize arap, atm, bstun
 *
 * Revision 2.1  1995/06/07  22:01:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

/***************************************************************
 * arap 
 *
 * 
 */


/*************************************************************************
 * arap network <net-number> <zone-name>
 * no arap network <net-number> [<zone-name>]
 *
 * OBJ(int,1) = net-number
 * OBJ(string,1) = zone-name
 */
EOLS (cl_arap_network_eol, arap_command, PARSER_ARAP_NETWORK);
TEXT (cl_arap_network_zone, cl_arap_network_eol, no_alt,
      OBJ(string,1), "Zone name");
NOPREFIX (cl_arap_nonetwork, cl_arap_network_zone, cl_arap_network_eol);
NUMBER (cl_arap_network_net, cl_arap_nonetwork, no_alt,
        OBJ(int,1), 1, 65279, "Network number");
NVGENS (cl_arap_networknvgen, cl_arap_network_net,
        arap_command, PARSER_ARAP_NETWORK);
KEYWORD (cl_arap_network, cl_arap_networknvgen, no_alt,
        "network", "Internal Appletalk Network For Arap Clients", PRIV_CONF);


EOLS	(cl_arap_logging_eol, arap_command, PARSER_ARAP_LOGGING);

EOLS	(cl_arap_logging_mnp4_eol, arap_command, PARSER_MNP4_LOGGING);

KEYWORD	(cl_arap_logging_mnp4, cl_arap_logging_mnp4_eol,
	 cl_arap_logging_eol, 
	 "mnp4", "Show MNP4 trace log on disconnect",
	 PRIV_CONF);

EOLS	(cl_arap_logging_debugextensions_eol, arap_command, PARSER_ARAP_LOGGING_DEBUGEXTENSIONS);

KEYWORD	(cl_arap_logging_debugextensions, cl_arap_logging_debugextensions_eol,
	 cl_arap_logging_mnp4, 
	 "debug-extensions", "Turn on verbose logging of ARAP events",
	 PRIV_CONF);

KEYWORD	(cl_arap_logging, cl_arap_logging_debugextensions,
	 cl_arap_network,
	 "logging", "Turn on logging of ARAP connections" , PRIV_CONF);

EOLS	(cl_arap_callback_eol, arap_command, PARSER_ARAP_CALLBACK);
KEYWORD	(cl_arap_callback, cl_arap_callback_eol, cl_arap_logging,
	 "callback", "Enable callback of ARAP connections" , PRIV_CONF);

KEYWORD	(cl_arap, cl_arap_callback, ALTERNATE,
	 "arap", "Appletalk Remote Access Protocol", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cl_arap
