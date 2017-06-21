/* $Id: at_domain_chain.c,v 3.6.10.5 1996/07/23 18:34:27 dwong Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_domain_chain.c,v $
 *------------------------------------------------------------------
 * Parse chain file for AppleTalk Remapping
 *
 * September 1995, Steven Lin
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_domain_chain.c,v $
 * Revision 3.6.10.5  1996/07/23  18:34:27  dwong
 * CSCdi63707:  AT Domain-group missing after reload
 * Branch: California_branch
 *
 * Revision 3.6.10.4  1996/07/10  22:17:55  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.6.10.3  1996/05/21  09:42:25  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.10.2  1996/04/03  02:28:45  slin
 * CSCdi53350:  iptalk is broken
 * Branch: California_branch
 *
 * Revision 3.6.10.1  1996/03/23  01:26:05  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.6  1996/02/22  14:31:59  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  05:58:57  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:26:00  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:24:19  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  08:42:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:56:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:49:39  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/28  00:25:28  slin
 * Placeholders for AppleTalk Code Modularization
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "packet.h"
#include "config.h"
#include "parser.h"

#include "interface.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_atalk.h"
#include "atalk_private.h"
#include "at_domain_debug.h"

/*
 * Parse chains for DEBUG commands
 */
#define ALTERNATE NONE
/*****************************************************************
 * debug apple remap
 */
EOLS    (debug_apple_remap_eol, atdomain_debug_command, DEBUG_ATALK_REMAP);
KEYWORD (debug_apple_remap, debug_apple_remap_eol, no_alt,
	 "remap", "AppleTalk Remap function", PRIV_OPR);

/*****************************************************************
 * debug apple domain
 */
EOLS    (debug_apple_domain_eol, atdomain_debug_command, DEBUG_ATALK_DOMAIN);
KEYWORD (debug_apple_domain, debug_apple_domain_eol, debug_apple_remap,
	 "domain", "AppleTalk Domain function", PRIV_OPR);

#undef ALTERNATE
#define ALTERNATE	debug_apple_domain
LINK_POINT(atdomain_debug_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for AT/EIGRP interface commands
 */
#define ALTERNATE NONE
/*******************************************************************
 * <no> appletalk domain-group <number>
 */

EOLS    (ci_at_dgroup_eol, atdomain_if_command, ATALK_DOMAIN_GROUP);

NUMBER  (ci_at_set_dgroup, ci_at_dgroup_eol, no_alt,
	 OBJ(int,1), 1, 1000000, "AppleTalk Domain Number");

NVGENS  (ci_at_dgroupnvg, ci_at_set_dgroup, atdomain_if_command,
	 ATALK_DOMAIN_GROUP);
KEYWORD (ci_at_domain_group, ci_at_dgroupnvg, ALTERNATE, 
	 "domain-group", "Specify appletalk domain ", PRIV_CONF|PRIV_SUBIF);

ASSERT  (ci_at_domain_subif, ci_at_domain_group, ALTERNATE, 
	 main_if_or_vc(csb->interface));

#undef ALTERNATE
#define ALTERNATE	ci_at_domain_subif
LINK_POINT(atdomain_interface_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chains for AT/EIGRP top-level configuration commands
 */
#define ALTERNATE NONE
/************************************************************************/
/* appletalk domain <number> name <name>
 * appletalk domain <number> in remap-range <range> 
 * appletalk domain <number> out remap-range <range> 
 * appletalk domain <number> {hop-preservation | hop-reduction}
 */
EOLS    (conf_at_domain_eol, atdomain_command, ATALK_DOMAIN);

KEYWORD_ID(conf_at_domain_hop_preserve, conf_at_domain_eol, no_alt,
	   OBJ(int,2), ATALK_DOMAIN_HOP_PRESERVE,
	   "hop-preservation","Specify a hop preservation on the domain",
	   PRIV_CONF|PRIV_HIDDEN);

KEYWORD_ID(conf_at_domain_hop, conf_at_domain_eol, conf_at_domain_hop_preserve,
	   OBJ(int,2), ATALK_DOMAIN_HOP_REDUCTION, 
	   "hop-reduction", "Specify a hop reduction on the domain",PRIV_CONF);

CRANGE  (conf_at_domain_remaprange, conf_at_domain_eol, conf_at_domain_eol,
	 OBJ(int,3), OBJ(int,4));

KEYWORD_ID (conf_at_domain_remap_out, conf_at_domain_remaprange, no_alt,
	    OBJ(int,5), OUT, 
	    "out", "Remap on networks coming to domain", PRIV_CONF);

KEYWORD_ID (conf_at_domain_remap_in, conf_at_domain_remaprange,
	    conf_at_domain_remap_out,
	    OBJ(int,5), IN, 
	    "in", "Remap on networks coming from domain", PRIV_CONF);

KEYWORD_ID (conf_at_domain_remap, conf_at_domain_remap_in, conf_at_domain_hop,
	    OBJ(int,2),ATALK_DOMAIN_REMAP,
	    "remap-range", "Configure a Domain remapping range", PRIV_CONF);

NOPREFIX (conf_at_no_domain, conf_at_domain_eol, conf_at_domain_eol); 

TEXT    (conf_at_domain_set_name, conf_at_no_domain, no_alt,
	 OBJ(string,1), "Configure Domain Name");

KEYWORD_ID (conf_at_domain_name, conf_at_domain_set_name, conf_at_domain_remap,
	 OBJ(int,2),ATALK_DOMAIN_NAME,
	 "name", "Configure Domain name", PRIV_CONF);

NUMBER  (conf_at_domain_number, conf_at_domain_name, no_alt,
	 OBJ(int,1), 1, 1000000, "AppleTalk Domain Number");

NVGENS  (conf_at_domain_nvgen, conf_at_domain_number, atdomain_command,
	 ATALK_DOMAIN);

KEYWORD (conf_at_domain, conf_at_domain_nvgen, ALTERNATE,
	 "domain", "Configure AppleTalk Domain", PRIV_CONF);


#undef  ALTERNATE
#define ALTERNATE conf_at_domain
LINK_POINT(atdomain_config_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chain for SHOW commands.
 */
#define ALTERNATE NONE
/*****************************************************************
 * show apple domain
 *
 */
EOLS    (show_apple_domain_eol, atdomain_show_command, SHOW_APPLE_DOMAIN);

NUMBER  (show_apple_dm, show_apple_domain_eol, show_apple_domain_eol,
	 OBJ(int,1), 1, 1000000, "AppleTalk Domain Number");

KEYWORD (show_apple_domain, show_apple_dm, ALTERNATE,
	 "domain", "AppleTalk Domain(s) information", PRIV_USER);

/*****************************************************************
 * show apple remap
 *
 */
EOLS    (show_apple_remap_eol, atdomain_show_command, SHOW_APPLE_REMAP);

NUMBER  (show_apple_dm_net, show_apple_remap_eol, no_alt,
	 OBJ(int,4), 1, 65535, "AppleTalk network");

KEYWORD_ID (show_apple_dm_from, show_apple_dm_net, show_apple_remap_eol,
	    OBJ(int,3), 2, 
	    "from", "Find Original", PRIV_USER);

KEYWORD_ID (show_apple_dm_to, show_apple_dm_net, show_apple_dm_from,
	    OBJ(int,3), 1, 
	    "to", "Find Remap", PRIV_USER);


KEYWORD_ID (show_apple_dm_out, show_apple_dm_to, show_apple_remap_eol,
	    OBJ(int,2), 1, 
	    "out", "Remap on outbound", PRIV_USER);

KEYWORD_ID (show_apple_dm_in, show_apple_dm_to, show_apple_dm_out,
	    OBJ(int,2), 2, 
	    "in", "Remap on inbound", PRIV_USER);


NUMBER  (show_apple_remap_domain_number, show_apple_dm_in, no_alt,
	 OBJ(int,1), 1, 1000000, "AppleTalk Domain Number");
KEYWORD (show_apple_remap_domain, show_apple_remap_domain_number,show_apple_remap_eol,
	 "domain", "AppleTalk Domain Number",PRIV_USER);
KEYWORD (show_apple_remap, show_apple_remap_domain, show_apple_domain,
	 "remap", "AppleTalk remap table", PRIV_USER);


#undef  ALTERNATE
#define ALTERNATE	show_apple_remap
LINK_POINT(atdomain_show_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chain registration array for AT/Domain
 */
static parser_extension_request atdomain_chain_init_table[] = {
    { PARSE_ADD_DEBUG_ATALK_CMD, &pname(atdomain_debug_commands) },
    { PARSE_ADD_CFG_INT_ATALK_CMD, &pname(atdomain_interface_commands) },
    { PARSE_ADD_CFG_TOP_ATALK_CMD, &pname(atdomain_config_commands) },
    { PARSE_ADD_SHOW_ATALK_CMD, &pname(atdomain_show_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * atdomain_parser_init - Initialize AT/Domain parser support
 */
void
atdomain_parser_init (void)
{
    parser_add_command_list(atdomain_chain_init_table, "atdomain");
}

