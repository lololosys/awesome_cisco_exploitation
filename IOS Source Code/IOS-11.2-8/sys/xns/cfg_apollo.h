/* $Id: cfg_apollo.h,v 3.3 1995/11/27 21:25:12 widmer Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/cfg_apollo.h,v $
 *------------------------------------------------------------------
 * C F G _ A P O L L O . H
 *
 * Apollo global configuration commands.
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_apollo.h,v $
 * Revision 3.3  1995/11/27  21:25:12  widmer
 * CSCdi43607:  Add keyword_option macro to parse multiple keywords
 * Also create permit_deny macro using keyword_option macro to parse
 * permit/deny keywords.
 * Change "ip security multilevel" command to use keyword_option macro.
 *
 * Revision 3.2  1995/11/17  19:20:54  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:42:55  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.2  1995/06/21  18:16:18  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:01:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/**********************************************************************
 * apollo access-list <name> { permit | deny }
 *	[<firstnet>-]<lastnet.host> [<mask>]
 * no apollo access-list <name> [<any remaining arguments>]
 *
 * OBJ(string,1) = <name>
 * OBJ(int,1) = TRUE for permit, FALSE for deny
 * OBJ(int,2) = firstnet
 * OBJ(int,3) = TRUE if firstnet exists, FALSE otherwise
 * OBJ(int,4) = mask
 * OBJ(int,5) = TRUE if a mask exists, FALSE otherwise
 * (*OBJ(paddr,1)) = lastnet.host
 *
 *
 * We try to match <lastnet.host>.  If that fails, get <firstnet> and '-',
 * then <lastnet.host>.  This allows use of the apollo address parsing
 * functions and simplifies the overall parse.
 */

EOLS	(conf_apolloAL_eol, apollo_command, APOLLO_ACCESSL);

/* [<mask>] */
SET	(conf_apolloAL_maskset, conf_apolloAL_eol, OBJ(int,5), TRUE);
HEXNUM	(conf_apolloAL_mask, conf_apolloAL_maskset, conf_apolloAL_eol,
	 OBJ(int,4), "Mask");

/* If we didn't find <lastnet.host> below, then look
 * for <firstnet>-<lastnet.host>
 */
APOLLOADDR (conf_apolloAL_lastnet2, conf_apolloAL_mask, no_alt,
	    OBJ(paddr,1), "Network.host address to filter");
HELP	(conf_apolloAL_firstnethelp, NONE, "N-N.H\t");
SET	(conf_apolloAL_firstnetset, conf_apolloAL_lastnet2,
	 OBJ(int,3), TRUE);
CHARACTER (conf_apolloAL_hyphen, conf_apolloAL_firstnetset,
	   conf_apolloAL_firstnethelp, '-');
HEXADECIMAL (conf_apolloAL_firstnetnum, conf_apolloAL_hyphen,
	     no_alt, OBJ(int,2));
HELP	(conf_apolloAL_firstnet, conf_apolloAL_firstnetnum,
	 "  N-N.H\tNet1-net2.host network range filter\n");

/* <lastnet.host> */
APOLLOADDR (conf_apolloAL_lastnet1, conf_apolloAL_mask, conf_apolloAL_firstnet,
	    OBJ(paddr,1), "Network.host address to filter");

/* Permit & Deny keywords */
PERMIT_DENY(conf_apolloAL_permit_deny, conf_apolloAL_lastnet1, no_alt,
	    OBJ(int,1), PRIV_CONF);

/* Quit here if this is the 'no' version of the command */
NOPREFIX(conf_apolloAL_noprefix, conf_apolloAL_permit_deny, conf_apolloAL_eol);

STRING	(conf_apolloAL_name, conf_apolloAL_noprefix, no_alt, OBJ(string,1),
	 "Apollo access list name");
NVGENS	(conf_apolloAL_nvgen, conf_apolloAL_name, apollo_command,
	 APOLLO_ACCESSL);
KEYWORD (conf_apolloAL, conf_apolloAL_nvgen, no_alt,
	 "access-list", "Apollo access list", PRIV_CONF);

/**********************************************************************
 * apollo maximum-paths [<paths>]
 * no apollo maximum-paths [<paths>]
 *
 * OBJ(int,1) = <paths>
 */
EOLS	(conf_apollo_path_eol, apollo_command, APOLLO_PATHS);
NUMBER	(conf_apollo_path_num, conf_apollo_path_eol, conf_apollo_path_eol,
	 OBJ(int,1), 1, 512, "Number of parallel routing paths");
NVGENS	(conf_apollo_path_nvgen, conf_apollo_path_num, apollo_command,
	 APOLLO_PATHS);
KEYWORD (conf_apollo_path, conf_apollo_path_nvgen, conf_apolloAL,
	 "maximum-paths", "Set number of parallel routing paths", PRIV_CONF);

/**********************************************************************
 * apollo routing <address>
 * no apollo routing [<address>]
 *
 * OBJ(int,1) = <address>
 */

EOLS	(conf_apollo_routingeol, apollo_command, APOLLO_ROUTING);
HEXDIGIT(conf_apollo_routingaddr, conf_apollo_routingeol, no_alt,
	 OBJ(int,1), 1, APOLLOMAXADDR,
	 "This router's Apollo address");
NOPREFIX(conf_apollo_norouting, conf_apollo_routingaddr,
	 conf_apollo_routingeol);
NVGENS	(conf_apollo_routingnvgen, conf_apollo_norouting, apollo_command,
	 APOLLO_ROUTING);
KEYWORD (conf_apollo_routing, conf_apollo_routingnvgen, conf_apollo_path,
	 "routing", "Enable Apollo routing and set node address", PRIV_CONF);



KEYWORD	(configure_apollo, conf_apollo_routing, ALTERNATE,
	 "apollo", "Apollo global configuration commands", PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE	configure_apollo
