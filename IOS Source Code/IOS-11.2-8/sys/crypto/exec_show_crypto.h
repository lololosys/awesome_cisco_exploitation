/* $Id: exec_show_crypto.h,v 3.1.70.14 1996/08/27 06:30:56 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/exec_show_crypto.h,v $
 *------------------------------------------------------------------
 * exec_show_crypto.h - Show crypto status and public values.
 *
 * June 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_crypto.h,v $
 * Revision 3.1.70.14  1996/08/27  06:30:56  xliu
 * CSCdi67270:  crypto parser command clean up
 * Branch: California_branch
 * Incorporate comments from doc. review, as well as some syntax clean up.
 *
 * Revision 3.1.70.13  1996/08/15  07:11:11  xliu
 * CSCdi65751:  Inconsistent slot numbers in different crypto commands
 * Branch: California_branch
 *
 * Revision 3.1.70.12  1996/08/12  16:00:59  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.70.11  1996/08/07  09:21:14  xliu
 * CSCdi63274:  Wrong crypto engine name a show cr eng conf
 * Branch: California_branch
 *
 * Revision 3.1.70.10  1996/07/28  07:41:57  xliu
 * CSCdi62947:  Add parser command to query epa status from RSP
 * Branch: California_branch
 *
 * Revision 3.1.70.9  1996/07/15  21:22:57  xliu
 * CSCdi62990:  crypto engine parser command is broken
 * Branch: California_branch
 *
 * Revision 3.1.70.8  1996/07/15  10:05:24  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 3.1.70.7  1996/07/03  05:54:39  snijsure
 * CSCdi60711:  sh cr conn eng act does not show anything in vip2 crypto
 * router
 * Branch: California_branch
 *
 * Revision 3.1.70.6  1996/06/08  18:43:05  xliu
 * CSCdi59903:  show crypto eng brief is not yet functional
 * This parser command should only be show on RSP platform.
 * Branch: California_branch
 *
 * Revision 3.1.70.5  1996/06/07  00:24:31  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Various cleanups & addition of "crypto ping".
 *
 * Revision 3.1.70.4  1996/05/28  15:59:00  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 3.1.70.3  1996/05/21  09:44:26  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.70.2  1996/05/12  23:12:30  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.1.70.1  1996/04/19  14:56:08  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 3.1.2.9  1996/04/17  23:47:35  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 3.1.2.8  1996/04/16  21:52:53  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 3.1.2.7  1996/03/30  05:11:42  xliu
 * Branch: IosSec_branch
 * Commit before CA repository split.
 *
 * Revision 3.1.2.6  1996/03/13  22:45:16  bew
 * Branch: IosSec_branch
 * The RSP/VIP environment (multiple crypto engines) are now supported
 * in crypto parser routines and crypto session mgmt code.
 *
 * Revision 3.1.2.5  1996/03/01  01:39:17  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 3.1.2.4  1996/02/13  19:15:56  carrel
 * Branch: IosSec_branch
 * Crypto Map checkin (finally)
 *
 * Revision 3.1.2.3  1996/01/19  07:00:39  xliu
 * Branch: IosSec_branch
 * Commit before the second sync.
 *
 * Revision 3.1.2.2  1996/01/04  18:47:53  xliu
 * Branch: IosSec_branch
 * Crypto moduliztion.
 *
 * Revision 3.1.2.1  1995/11/28  01:46:50  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 3.1  1995/11/22  23:17:39  bew
 * Placeholders for new locations of crypto parser files
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * show crypto
 */

EOLNS	(exec_show_crypto_eol, crypto_show_command);

/*
 * show crypto card 
 * show crypto card status
 */
NUMBER  (exec_show_crypto_card_slot, exec_show_crypto_eol,
         no_alt, OBJ(int,2), 0, CRYPTO_MAX_SLOTS-1, "slot number");

KEYWORD_ID (exec_show_crypto_card, exec_show_crypto_card_slot, 
	    no_alt, OBJ(int,1), PARSER_CRYPTO_CARD, 
            "card", "Show crypto card status", PRIV_ROOT);

IFELSE (exec_show_crypto_card_if, exec_show_crypto_card, 
        no_alt,  crypto_chassis_has_slots());

/*
 * show crypto engine bri
 * Display all the crypto engines in the system
 */
KEYWORD_ID (exec_show_crypto_rsp_engines, exec_show_crypto_eol, 
	    no_alt, OBJ(int,1), PARSER_CRYPTO_ENGINES, 
            "brief", "Show all crypto engines in the system", PRIV_ROOT);

IFELSE (exec_show_crypto_engines, exec_show_crypto_rsp_engines, 
        no_alt,  crypto_chassis_has_slots());

KEYWORD_ID (exec_show_crypto_engine_rp, exec_show_crypto_eol,
	    exec_show_crypto_engines, OBJ(int,1), 
            PARSER_CRYPTO_ENGINE_RP_CONFIG, "configuration", 
	    "Show RP crypto engine config", PRIV_ROOT);

KEYWORD_ID (exec_show_crypto_engine_rsp, exec_show_crypto_eol,
	    exec_show_crypto_engines, OBJ(int,1), 
            PARSER_CRYPTO_ENGINE_RSP_CONFIG, "configuration", 
	    "Show RSP crypto engine config", PRIV_ROOT);
/*
 * show crypto engine configuration 
 */

IFELSE (exec_show_crypto_engine_config, exec_show_crypto_engine_rsp, 
        exec_show_crypto_engine_rp,  crypto_chassis_has_slots());

/*
 * show crypto engine dropped packets 
 */

KEYWORD_ID (exec_show_crypto_engine_drop_pak, exec_show_crypto_eol, 
	    no_alt, OBJ(int,1), 
            PARSER_CRYPTO_ENGINE_DROP_PAK, "dropped-packet", 
	    "Show crypto engine dropped packets", PRIV_ROOT);

/*
 * show crypto engine connnection table: <begin, end>
 */
NUMBER  (exec_show_crypto_rsp_slot_num, exec_show_crypto_eol, 
	   exec_show_crypto_eol, OBJ(int,2), 0, 19, "slot number");

IFELSE  (exec_show_crypto_engine_active_conn_if, 
         exec_show_crypto_rsp_slot_num,
         exec_show_crypto_eol,
	 crypto_chassis_has_slots());

KEYWORD_ID (exec_show_crypto_engine_active_conn, 
            exec_show_crypto_engine_active_conn_if,
	    exec_show_crypto_engine_drop_pak, OBJ(int,1), 
            PARSER_CRYPTO_ENGINE_ACT_CONN, "active", 
	    "Show all crypto engine active connections", PRIV_ROOT);

SET     (exec_show_crypto_engine_conns, exec_show_crypto_engine_active_conn, 
         OBJ(int, 1), PARSER_CRYPTO_ENGINE_CONN_ALL);

KEYWORD_ID (exec_show_crypto_engine_conntbl, exec_show_crypto_engine_conns, 
	    exec_show_crypto_engine_config, OBJ(int,1), 
            PARSER_CRYPTO_ENGINE_CONNTBL, "connections", 
	    "Show connection information", PRIV_ROOT);

/*
 * show crypto engine info 
 *
 */

KEYWORD_ID (exec_show_crypto_engine, exec_show_crypto_engine_conntbl, 
	    exec_show_crypto_card_if, OBJ(int,1), 
            PARSER_CRYPTO_ENGINE, "engine", 
	    "Show crypto engine info", PRIV_ROOT);
/*
 * show crypto pregen-dh-pairs [<slot>]
 */

NUMBER (exec_show_crypto_pregen_slot, exec_show_crypto_eol,
	exec_show_crypto_eol, OBJ(int,2), 0, CRYPTO_MAX_SLOTS-1,
	"Chassis slot number");

SET     (exec_show_crypto_pregen_set, exec_show_crypto_pregen_slot, 
	 OBJ(int,2), CRYPTO_INVALID_SLOT);

IFELSE  (exec_show_crypto_pregen_if, exec_show_crypto_pregen_set,
	 exec_show_crypto_eol, crypto_chassis_has_slots());

KEYWORD_ID (exec_show_crypto_pregen, exec_show_crypto_pregen_if, 
	    exec_show_crypto_engine, OBJ(int,1), 
            PARSER_CRYPTO_GET_PREGEN, "pregen-dh-pairs", 
	    "Show how many pairs are currently pregenerated", PRIV_ROOT);

/*
 * show crypto key-timeout
 */

KEYWORD_ID (exec_show_crypto_session_key_timeout, exec_show_crypto_eol, 
	    exec_show_crypto_pregen, OBJ(int,1), 
            PARSER_CRYPTO_GET_KEY_TIMEOUT, "key-timeout", 
	    "Show this router's session key timeout value", PRIV_ROOT);

/*
 * show crypto connections
 */

KEYWORD_ID (exec_show_crypto_conn, exec_show_crypto_eol, 
	    exec_show_crypto_session_key_timeout, OBJ(int,1), 
	    PARSER_CRYPTO_GET_CONN, "connections", 
	    "Show this router's pending & completed encrypted connections", PRIV_ROOT);

/*
 * show crypto alg
 */
KEYWORD_ID (exec_show_crypto_algs, exec_show_crypto_eol, 
	    exec_show_crypto_conn, OBJ(int,1), PARSER_CRYPTO_GET_ALG, "algorithms", 
	    "Show this router's supported algorithms", PRIV_ROOT);

/*
 * For 7000/7500:
 *  show crypto mypubkey rsp
 * For all routers:
 * show crypto mypubkey
 */

KEYWORD_ID (exec_show_crypto_mykey_rsp, exec_show_crypto_eol,
	    exec_show_crypto_eol, OBJ(int,1), 
	    PARSER_CRYPTO_GETPUBKEY_RP, "rsp", "Select the RSP public key",
	    PRIV_USER);

KEYWORD_ID (exec_show_crypto_mykey_slot, exec_show_crypto_mykey_rsp, 
	    exec_show_crypto_algs,  OBJ(int,1), PARSER_CRYPTO_GETPUBKEY_ALLMY,
	    "mypubkey", 
	    "Show the router public key(s)", PRIV_USER);

KEYWORD_ID (exec_show_crypto_mykey_noslot, exec_show_crypto_eol, 
	    exec_show_crypto_algs,  OBJ(int,1), PARSER_CRYPTO_GETPUBKEY_ALLMY,
	    "mypubkey", 
	    "Show the router public key", PRIV_USER);

IFELSE	(exec_show_crypto_mykey, exec_show_crypto_mykey_slot, 
	 exec_show_crypto_mykey_noslot, crypto_chassis_has_slots());

/*
 * show crypto pubkey serial <peer-serial-number>
 * show crypto pubkey name <peer-name>
 * show crypto pubkey 
 */

STRING  (exec_show_crypto_pubkey_serial_str, exec_show_crypto_eol, no_alt,
	 OBJ(string,1), "Peer serial number");

KEYWORD_ID (exec_show_crypto_pubkey_serial, exec_show_crypto_pubkey_serial_str, 
	    exec_show_crypto_eol, OBJ(int,1),
	    PARSER_CRYPTO_GETPUBKEY_SERIAL, "serial", 
	    "Select key by serial number", PRIV_USER);

STRING  (exec_show_crypto_pubkey_name_str, exec_show_crypto_eol, no_alt,
	 OBJ(string,1), "Peer name");

KEYWORD_ID (exec_show_crypto_pubkey_name, exec_show_crypto_pubkey_name_str, 
	    exec_show_crypto_pubkey_serial, OBJ(int,1), 
	    PARSER_CRYPTO_GETPUBKEY_NAME, "name", "Select key by name", 
	    PRIV_USER);

KEYWORD_ID (exec_show_crypto_pubkey, exec_show_crypto_pubkey_name, 
	    exec_show_crypto_mykey, OBJ(int,1), PARSER_CRYPTO_GETPUBKEY_ALL,
	    "pubkey", "Show peer public key", PRIV_USER);

/*
 * show crypto map
 * show crypto map interface <interface>
 * show crypto map tag <tag>
 *
 * OBJ(int,2)	 = <action>
 * OBJ(idb,1) 	 = <interface>
 * OBJ(string,1) = <tag>
 */
EOLNS	(exec_show_crypto_map_eol, show_cryptomaps);
STRING	(exec_show_crypto_map_tag_tag, exec_show_crypto_map_eol, no_alt,
	 OBJ(string,1), "Crypto map tag");
KEYWORD_ID (exec_show_crypto_map_tag, exec_show_crypto_map_tag_tag,
	    exec_show_crypto_map_eol,
	    OBJ(int,2), PARSER_CRYPTO_MAP_TAG,
	    "tag", "Crypto maps with a given tag.", PRIV_CONF);
INTERFACE (exec_show_crypto_map_if_if, exec_show_crypto_map_eol, no_alt,
	   OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD_ID (exec_show_crypto_map_if, exec_show_crypto_map_if_if,
	    exec_show_crypto_map_tag,
	    OBJ(int,2), PARSER_CRYPTO_MAP_IF,
	    common_str_interface, "Crypto maps for an interface.", PRIV_CONF);
SET	(exec_show_crypto_map_set, exec_show_crypto_map_if,
	 OBJ(int,2), PARSER_CRYPTO_MAP_ALL);
KEYWORD	(exec_show_crypto_map, exec_show_crypto_map_set,exec_show_crypto_pubkey,
	 "map", "Crypto maps", PRIV_ROOT);



KEYWORD	(exec_show_crypto_base, exec_show_crypto_map, ALTERNATE,
	 "crypto", "Encryption module", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	exec_show_crypto_base
