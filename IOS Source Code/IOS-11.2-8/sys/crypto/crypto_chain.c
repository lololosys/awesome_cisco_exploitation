/* $Id: crypto_chain.c,v 3.2.60.4 1996/08/15 01:12:55 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_chain.c,v $
 *------------------------------------------------------------------
 * crypto_chain.c - Crypto parse chains
 *
 * May 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_chain.c,v $
 * Revision 3.2.60.4  1996/08/15  01:12:55  xliu
 * CSCdi65622:  Cannnot apply crypto map to frame-relay subinterface
 * Branch: California_branch
 *
 * Revision 3.2.60.3  1996/06/17  23:29:46  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/05/21  09:44:25  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2.60.1  1996/04/19  14:55:31  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 2.1.6.15  1996/03/19  07:48:34  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 2.1.6.14  1996/03/13  22:44:54  bew
 * Branch: IosSec_branch
 * The RSP/VIP environment (multiple crypto engines) are now supported
 * in crypto parser routines and crypto session mgmt code.
 *
 * Revision 2.1.6.13  1996/03/01  18:14:20  carrel
 * Branch: IosSec_branch
 * Fixed crypto map indenting problem.  (Thanks Rob!)
 *
 * Revision 2.1.6.12  1996/03/01  01:38:48  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 2.1.6.11  1996/02/13  19:15:47  carrel
 * Branch: IosSec_branch
 * Crypto Map checkin (finally)
 *
 * Revision 2.1.6.10  1995/12/19  03:41:24  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 2.1.6.9  1995/12/17  06:22:51  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.8  1995/11/28  01:46:21  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 2.1.6.7  1995/11/17  16:33:13  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.6  1995/10/16  04:04:13  xliu
 * Branch: IosSec_branch
 * Add crypto engine parser commands.
 *
 * Revision 2.1.6.4  1995/10/10  18:19:29  bew
 * Branch: IosSec_branch
 * Implementation of connection timeouts with managed timers & the clear
 * crypto connection command.  Introduced the concept of "current
 * connection" and "new connection" for use during connection setup.
 * Also oodles of cleanup work.
 *
 * Revision 2.1.6.3  1995/09/06  00:55:45  bew
 * Branch: IosSec_branch
 * epa_init() is now called whenever new DSS keys are created or read in
 * from the configuration. Also added a crypto config command to initialize
 * the EPA to replace the exec epa command. The epa command was removed
 * altogether.
 *
 * Revision 2.1.6.2  1995/08/02  19:24:28  bew
 * Branch: IosSec_branch
 * Addition of Connection Table manipulation routines for use by ACL code.
 * Cleanup of sign & verify code.
 * Storage of DSS key pair in the configuration (for now).
 *
 * Revision 2.1.6.1  1995/07/01  05:31:17  carrel
 * Branch: IosSec_branch
 * Initial merge of the 10.3 IosSec_branch to Arizona.
 *
 * Revision 3.1  1995/11/09  11:13:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:21:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1.2.3  1995/06/23  02:57:01  bew
 * Branch: IosSec_branch
 * Support the storing of public keys in the configuration.
 *
 * Revision 1.1.2.2  1995/06/06  21:33:52  bew
 * Branch: IosSec_branch
 * Cleanup of crypto subsystem debugging. Crypto debug is now accessed
 * through the standard parser "debug" command.
 *
 * Revision 1.1.2.1  1995/06/02  00:19:56  bew
 * Branch: IosSec_branch
 * New crypto files -- Session Management files and cleanup of the
 * subsystem.
 *
 * Revision 1.1  1995/06/01 23:16:22  bew
 * Placeholders for crypto/crypto* files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "interface.h"
#include "../if/network.h"
#include "config.h"
#include "parser.h"
#include "access.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "../crypto/parser_defs_crypto.h"
#include "../ui/debug.h"
#include "crypto.h"
#include "crypto_debug.h"
#include "crypto_conntable.h"
#include "crypto_map.h"

/*
 * Start of crypto map subcommands
 */

#undef  ALTERNATE
#define ALTERNATE       no_alt
#include "cfg_cryptomap.h"
NO_OR_DEFAULT(cm_top_no, ALTERNATE, PRIV_MIN | PRIV_NONVGEN);
HELP	(top_config_cryptomap, cm_top_no,
	 "Crypto Map configuration commands:\n");
#undef  ALTERNATE

/*
 * Parse chains for crypto interface commands
 */
#define ALTERNATE	NONE
#include "../crypto/cfg_int_crypto.h"
LINK_POINT(crypto_interface_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for crypto debug commands
 */
#define	ALTERNATE	NONE
#include "../crypto/exec_debug_crypto.h"
LINK_POINT(crypto_debug_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for config commands
 */
#define	ALTERNATE	NONE
#include "../crypto/cfg_crypto.h"
LINK_POINT(crypto_config_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for show commands
 */
#define	ALTERNATE	NONE
#include "../crypto/exec_show_crypto.h"
LINK_POINT(crypto_show_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for clear commands
 */
#define	ALTERNATE	NONE
#include "../crypto/exec_clear_crypto.h"
LINK_POINT(crypto_clear_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for test commands
 */
#define	ALTERNATE	NONE
#include "../crypto/exec_test_crypto.h"
LINK_POINT(crypto_test_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain for hex data input mode...
 */
#define ALTERNATE no_alt
#include "../crypto/cfg_crypto_hex.h"
HELP (crypto_config_keylist, ALTERNATE, "Hex input mode:\n");
#undef  ALTERNATE
#define ALTERNATE no_alt
#include "../crypto/cfg_crypto_keypair_hex.h"
HELP (crypto_config_keypair_keylist, ALTERNATE, "Hex input mode:\n");
#undef  ALTERNATE

/*
 *  Parse chain registration array for static map commands
 */
parser_extension_request static_crypto_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(crypto_debug_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(crypto_config_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(crypto_show_commands) },
    { PARSE_ADD_CLEAR_CMD, &pname(crypto_clear_commands) },
    { PARSE_ADD_TEST_CMD, &pname(crypto_test_commands) },
    { PARSE_ADD_CFG_INT_AS_CMD, &pname(crypto_interface_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * Local static functions for crypto maps
 */
static void *cryptomap_save_var (parseinfo *csb)
{
    crypto_maptype *cryptomap = csb->userDataBlock[0];

    csb->userDataBlock[0] = NULL;
    return(cryptomap);
}

static void cryptomap_reset_var (parseinfo *csb, void *var)
{
    csb->userDataBlock[0] = var;
}


/*
 * crypto_parser_init - Initialize crypto parser support
 */
void crypto_parser_init (void)
{
    parser_add_command_list(static_crypto_chain_init_table, "crypto");
    parser_add_mode("crypto-map", "config-crypto-map",
                    "Crypto map config mode",
                    TRUE, TRUE, "configure",
                    cryptomap_save_var, cryptomap_reset_var,
                    &pname(top_config_cryptomap),
		    NULL);
    parser_add_mode("crypto-pubkey", "config-pubkey", 
		    "Crypto subsystem public key entry mode",
		    TRUE, FALSE, NULL, NULL, NULL,
		    &pname(crypto_config_keylist),
		    NULL);
    parser_add_mode("crypto-pubkey-keypair", "config-keypair", 
		    "Crypto subsystem key pair entry mode",
		    TRUE, FALSE, NULL, NULL, NULL,
		    &pname(crypto_config_keypair_keylist),
		    NULL);
}
