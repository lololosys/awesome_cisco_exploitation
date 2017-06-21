/* $Id: atip_chain.c,v 3.7.4.5 1996/07/10 22:19:19 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atalk/atip_chain.c,v $
 *------------------------------------------------------------------
 * Parse chains for Appletalk IP-services
 *
 * March 1994, Steven Lin
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atip_chain.c,v $
 * Revision 3.7.4.5  1996/07/10  22:19:19  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.7.4.4  1996/05/21  09:42:32  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.4.3  1996/05/02  08:41:05  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.7.4.2  1996/04/03  02:28:47  slin
 * CSCdi53350:  iptalk is broken
 * Branch: California_branch
 *
 * Revision 3.7.4.1  1996/03/23  01:28:11  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.7  1996/03/07  23:30:11  kleung
 * CSCdi45795:  Apple: Filter Statement Incorrectly Placed in Tunnel Config
 *
 * Revision 3.6  1996/02/22  14:32:02  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/02/01  05:59:19  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/29  07:26:14  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/17  18:24:22  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  08:44:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:58:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:50:50  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/02  01:43:34  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  20:10:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "parser.h"

#include "../ui/common_strings.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_atalk.h"
#include "../parser/parser_defs_parser.h"
#include "atalk_private.h"
#include "atip_debug.h"

/*
 * Parse chains for DEBUG commands
 */
#define ALTERNATE NONE
/******************************************************************
 * debug apple macip [interface <interface>]
 *
 * OBJ(int, 1) = ATALK_DEBUG_INTERFACE if interface <interface>
 * OBJ(idb, 1) = <interface>
 *
 */
EOLS	(debug_apple_macip_eol, atip_debug_command, DEBUG_ATALK_MACIP);
INTERFACE (debug_apple_macip_int_name, debug_apple_macip_eol,
	   no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_apple_macip_int, debug_apple_macip_int_name,
	    debug_apple_macip_eol, OBJ(int, 1), ATALK_DEBUG_INTERFACE,
	    common_str_interface, "Appletalk interface-specific debugging", PRIV_OPR);
KEYWORD (debug_apple_macip, debug_apple_macip_int, ALTERNATE,
	 "macip", "MacIP functions", PRIV_OPR);


/******************************************************************
 * debug apple iptalk [interface <interface>]
 *
 * OBJ(int, 1) = ATALK_DEBUG_INTERFACE if interface <interface>
 * OBJ(idb, 1) = <interface>
 */
EOLS	(debug_apple_iptalk_eol, atip_debug_command, DEBUG_ATALK_IPTALK);
INTERFACE (debug_apple_iptalk_int_name, debug_apple_iptalk_eol,
	  no_alt, OBJ(idb, 1), IFTYPE_HWIDB);
KEYWORD_ID (debug_apple_iptalk_int, debug_apple_iptalk_int_name,
	    debug_apple_iptalk_eol, OBJ(int, 1), ATALK_DEBUG_INTERFACE,
	 common_str_interface, "Appletalk interface-specific debugging", PRIV_OPR);
KEYWORD (debug_apple_iptalk, debug_apple_iptalk_int, debug_apple_macip,
	 "iptalk", "IPTalk encapsulation and functionality", PRIV_OPR);

#undef ALTERNATE
#define ALTERNATE	debug_apple_iptalk
LINK_POINT(atip_debug_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chain for iptalk interface commands
 */
#define ALTERNATE NONE
/******************************************************************
 * appletalk iptalk <net> <zone-name>
 * no appletalk iptalk
 *
 * OBJ(string,1) = zone name
 * OBJ(int,1) = AppleTalk network
 */
EOLS	(ci_at_iptalk_eol, atipif_command, ATALK_IPTALK);

TEXT	(ci_at_iptalk_zone, ci_at_iptalk_eol, no_alt,
	 OBJ(string,1), "Appletalk zone");

NUMBER	(ci_at_iptalk_net, ci_at_iptalk_zone, no_alt,
	 OBJ(int,1), 1, 65279, "Appletalk network for this interface");

NOPREFIX (ci_at_noiptalk, ci_at_iptalk_net, ci_at_iptalk_eol);
NVGENS	(ci_at_iptalk_nvgen, ci_at_noiptalk, atipif_command, ATALK_IPTALK);
KEYWORD	(ci_at_iptalkword, ci_at_iptalk_nvgen, NONE,
	"iptalk", "CAP and KIP encapsulation of AT in IP", PRIV_CONF);
KEYWORD (ci_at_iptalk, ci_at_iptalkword, NONE,
	 "appletalk", "", PRIV_CONF | PRIV_DUPLICATE);
TEST_IDBSTATUS(ci_at_iptalk_assert, ci_at_iptalk, NONE, no_alt, IDB_TUNNEL);

#undef ALTERNATE
#define ALTERNATE  ci_at_iptalk_assert
LINK_POINT(atiptalk_interface_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for ATIP top-level configuration commands
 */
#define ALTERNATE NONE
/*************************************************************************
 * appletalk iptalk-baseport <number>
 * no appletalk iptalk-baseport [<number>]
 *
 * OBJ(int,1) = number
 */
PARAMS_KEYONLY (cr_at_iptalkbase, no_alt, "iptalk-baseport",
		OBJ(int,1), 0, -1, appleip_command, ATALK_IPTALK_BASEPORT,
		"iptalk-baseport",
		"Set starting UDP port for mapping AT DDP sockets", PRIV_CONF);


/*************************************************************************
 * appletalk macip server <ipaddr> zone <server-zone>
 * no appletalk macip server <ipaddr> zone <server-zone>
 * appletalk macip { dynamic | static } <ipaddr> [<ipaddr>] zone <server-zone>
 * no appletalk macip
 *
 * OBJ(int,1) = MACIP_SERVER, MACIP_DYNAMIC, or MACIP_STATIC
 * OBJ(string,1) = server-zone
 * (*OBJ(paddr,1)) = ipaddr
 * (*OBJ(paddr,2)) = ipaddr (optional address)
 */

EOLS (cr_at_macip_eol, appleip_command, ATALK_MACIP);

/* <ipaddr> [<ipaddr>] zone <server-zone> */
TEXT (cr_at_macip_zonename, cr_at_macip_eol, no_alt,
      OBJ(string,1), "Appletalk zone name");
KEYWORD	(cr_at_macip_zone, cr_at_macip_zonename, no_alt,
	"zone", "Keyword 'zone' to specify a zone", PRIV_CONF);
IPADDR (cr_at_macip_ipaddr2, cr_at_macip_zone, cr_at_macip_zone,
	OBJ(paddr,2), "End of IP address range");
IPADDR (cr_at_macip_ipaddr1, cr_at_macip_ipaddr2, no_alt,
	OBJ(paddr,1), "Beginning of IP address range");

/* static */
KEYWORD_ID (cr_at_macip_static, cr_at_macip_ipaddr1, no_alt,
	 OBJ(int,1), MACIP_STATIC,
	"static", "Use static IP client address assignment", PRIV_CONF);

/* dynamic */
KEYWORD_ID (cr_at_macip_dynamic, cr_at_macip_ipaddr1, cr_at_macip_static,
	 OBJ(int,1), MACIP_DYNAMIC,
	"dynamic", "Use dynamic IP client address assignment", PRIV_CONF);

/* server <ipaddr> zone <server-zone> */
TEXT (cr_at_macip_servzonename, cr_at_macip_eol, no_alt,
	OBJ(string,1), "Appletalk zone name");
KEYWORD	(cr_at_macip_servzone, cr_at_macip_servzonename, no_alt,
	"zone", "Appletalk server zone name", PRIV_CONF);
IPADDR (cr_at_macip_servipaddr, cr_at_macip_servzone, no_alt,
	OBJ(paddr,1), "Server's IP address");
KEYWORD_ID (cr_at_macip_server, cr_at_macip_servipaddr, cr_at_macip_dynamic,
	 OBJ(int,1), MACIP_SERVER,
	"server", "Set the server for dynamic IP address assignment", PRIV_CONF);

/* no appletalk macip */
SET (cr_at_macip_noset, cr_at_macip_eol, OBJ(int,1), MACIP_DISABLE);
TESTVAR(cr_at_macip_noprefix, cr_at_macip_noset, NONE,
	NONE, NONE, cr_at_macip_server, sense, FALSE);
NVGENS (cr_at_macip_nvgen, cr_at_macip_noprefix,
	appleip_command, ATALK_MACIP);
KEYWORD	(cr_at_macip, cr_at_macip_nvgen, cr_at_iptalkbase,
	"macip", "Enable MAC-IP", PRIV_CONF);

KEYWORD (cr_atip, cr_at_macip, ALTERNATE, 
	 "appletalk", "AppleTalk global configuration commands",
	 PRIV_CONF | PRIV_DUPLICATE);

#undef  ALTERNATE
#define ALTERNATE	cr_atip
LINK_POINT(atip_config_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chain for SHOW commands.
 */
#define ALTERNATE NONE
/******************************************************************
 * show apple macip-traffic
 *
 */
EOLS	(show_apple_macip_tr_eol, show_appleip, SHOW_APPLE_MACIP_TRAFFIC);
KEYWORD (show_apple_macip_tr, show_apple_macip_tr_eol, ALTERNATE,
	 "macip-traffic", "Mac IP traffic", PRIV_USER);

/******************************************************************
 * show apple macip-servers
 *
 */
EOLS	(show_apple_macip_serv_eol, show_appleip, SHOW_APPLE_MACIP_SERVERS);
KEYWORD (show_apple_macip_serv, show_apple_macip_serv_eol, show_apple_macip_tr,
	 "macip-servers", "Mac IP servers", PRIV_USER);

/******************************************************************
 * show apple macip-clients
 *
 */
EOLS	(show_apple_macip_cl_eol, show_appleip, SHOW_APPLE_MACIP_CLIENTS);
KEYWORD (show_apple_macip_cl, show_apple_macip_cl_eol, show_apple_macip_serv,
	 "macip-clients", "Mac IP clients", PRIV_USER);

#undef  ALTERNATE
#define ALTERNATE	show_apple_macip_cl
LINK_POINT(atip_show_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chain registration array for ATIP
 */
static parser_extension_request atip_chain_init_table[] = {
    { PARSE_ADD_DEBUG_ATALK_CMD, &pname(atip_debug_commands) },
    { PARSE_ADD_CFG_INT_AS2_CMD, &pname(atiptalk_interface_commands) },
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(atip_config_commands) },
    { PARSE_ADD_SHOW_ATALK_CMD, &pname(atip_show_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * atip_parser_init - Initialize ATIP parser support
 */
void
atip_parser_init (void)
{
    parser_add_command_list(atip_chain_init_table, "atip");
}






