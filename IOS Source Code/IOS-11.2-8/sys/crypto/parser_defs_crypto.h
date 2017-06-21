/* $Id: parser_defs_crypto.h,v 3.1.70.8 1996/08/27 06:31:01 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/parser_defs_crypto.h,v $
 *------------------------------------------------------------------
 * parser_defs_crypto.h - Definitions for the crypto subsystem
 *
 * June 1995, Brian Weis
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_crypto.h,v $
 * Revision 3.1.70.8  1996/08/27  06:31:01  xliu
 * CSCdi67270:  crypto parser command clean up
 * Branch: California_branch
 * Incorporate comments from doc. review, as well as some syntax clean up.
 *
 * Revision 3.1.70.7  1996/07/28  07:41:59  xliu
 * CSCdi62947:  Add parser command to query epa status from RSP
 * Branch: California_branch
 *
 * Revision 3.1.70.6  1996/07/15  10:05:29  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 3.1.70.5  1996/07/03  05:54:40  snijsure
 * CSCdi60711:  sh cr conn eng act does not show anything in vip2 crypto
 * router
 * Branch: California_branch
 *
 * Revision 3.1.70.4  1996/06/07  00:24:34  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Various cleanups & addition of "crypto ping".
 *
 * Revision 3.1.70.3  1996/05/28  15:59:01  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 3.1.70.2  1996/05/12  23:12:32  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.1.70.1  1996/04/19  14:56:13  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 3.1.2.13  1996/04/17  23:47:39  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 3.1.2.12  1996/04/16  21:52:59  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 3.1.2.11  1996/03/30  05:11:44  xliu
 * Branch: IosSec_branch
 * Commit before CA repository split.
 *
 * Revision 3.1.2.10  1996/03/26  20:34:25  xliu
 * Branch: IosSec_branch
 * Add new testing command.
 *
 * Revision 3.1.2.9  1996/03/25  18:43:45  bew
 * Branch: IosSec_branch
 * Added 40-bit DES and DES 8-bit CFB support. Implemented partial
 * challenge processing support, not yet compatible with SDU.
 * Updated the crypto methods vector per Cylink's new definitions.
 *
 * Revision 3.1.2.8  1996/03/14  00:57:57  bew
 * Branch: IosSec_branch
 * Changed the crypto key-exchange protocol to be more intuitive.
 *
 * Revision 3.1.2.7  1996/03/01  01:39:18  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 3.1.2.6  1996/02/13  19:15:57  carrel
 * Branch: IosSec_branch
 * Crypto Map checkin (finally)
 *
 * Revision 3.1.2.5  1996/01/19  07:00:40  xliu
 * Branch: IosSec_branch
 * Commit before the second sync.
 *
 * Revision 3.1.2.4  1996/01/04  18:47:54  xliu
 * Branch: IosSec_branch
 * Crypto moduliztion.
 *
 * Revision 3.1.2.3  1995/12/20  23:30:51  bew
 * Branch: IosSec_branch
 * Public key exchange can now be two-way as well as one-way. Context is
 * now freed after a sign operation. Wrap-around of connection ids now
 * supported.  General cleanup.
 *
 * Revision 3.1.2.2  1995/12/02  17:44:34  xliu
 * Branch: IosSec_branch
 *
 * Add parser command to precompute dh number.
 *
 * Revision 3.1.2.1  1995/11/28  01:46:53  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 3.1  1995/11/22  23:17:40  bew
 * Placeholders for new locations of crypto parser files
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *  Parser cfg commands
 */
typedef
enum crypto_cfg_types_ {
    PARSER_CRYPTO_SIGKEYS,
    PARSER_CRYPTO_PUBKEY,
    PARSER_CRYPTO_DSS_KEYPAIR,
    PARSER_CRYPTO_INIT_EPA,
    PARSER_CRYPTO_ALG_SET,
    PARSER_CRYPTO_KEY_TIMEOUT,
    PARSER_CRYPTO_ZEROIZE,
    PARSER_CRYPTO_KEY_EXCHANGE,
    PARSER_CRYPTO_KEY_EXCHANGE_PASSIVE,
    PARSER_CRYPTO_PREGEN_DH,
    PARSER_CRYPTO_CARD_LATCH,
    PARSER_CRYPTO_SDU_PET_SIZE,
    PARSER_CRYPTO_SDU_COT_SIZE
} crypto_cfg_types;

/*
 *  Parser crypto map commands
 */
typedef
enum crypto_map_cfg_types_ {
    PARSER_CRYPTO_MAP_GLOBAL,
    PARSER_CRYPTO_MAP_INT,
    PARSER_CRYPTO_MAP_MATCH_ADDR,
    PARSER_CRYPTO_MAP_SET_PEER,
    PARSER_CRYPTO_MAP_SET_ALG,
    PARSER_CRYPTO_MAP_SET_MODE,
    PARSER_CRYPTO_MAP_SDU_MODE,
} crypto_int_cfg_types;

/*
 *  Parser pubkey cfg commands
 */
typedef
enum crypto_cfg_pubkey_types_ {
    PARSER_CRYPTO_PUBKEY_QUIT,
    PARSER_CRYPTO_PUBKEY_DATA
} crypto_cfg_pubkey_types;

/*
 *  Parser show commands
 */
typedef
enum crypto_show_types_ {
    PARSER_CRYPTO_GETINFO,
    PARSER_CRYPTO_GETINFO_ALL,
    PARSER_CRYPTO_GETPUBKEY_NAME,
    PARSER_CRYPTO_GETPUBKEY_SERIAL,
    PARSER_CRYPTO_GETPUBKEY_ALL,
    PARSER_CRYPTO_GETPUBKEY_RP,
    PARSER_CRYPTO_GETPUBKEY_RSP,
    PARSER_CRYPTO_GETPUBKEY_ALLMY,
    PARSER_CRYPTO_GET_ALG,
    PARSER_CRYPTO_GET_CONN,
    PARSER_CRYPTO_GET_VIP_CONN,
    PARSER_CRYPTO_GET_KEY_TIMEOUT,
    PARSER_CRYPTO_GET_PREGEN,
    PARSER_CRYPTO_ENGINE,
    PARSER_CRYPTO_ENGINE_CONNTBL,
    PARSER_CRYPTO_ENGINE_ACT_CONN,
    PARSER_CRYPTO_ENGINE_DEAD_CONN,
    PARSER_CRYPTO_MAP_ALL,
    PARSER_CRYPTO_MAP_IF,
    PARSER_CRYPTO_MAP_TAG,
    PARSER_CRYPTO_ENGINE_CONFIG,
    PARSER_CRYPTO_ENGINE_DROP_PAK,
    PARSER_CRYPTO_ENGINES,
    PARSER_CRYPTO_ENGINE_RP_CONFIG,
    PARSER_CRYPTO_ENGINE_RSP_CONFIG,
    PARSER_CRYPTO_ENGINE_VIP_CONFIG,
    PARSER_CRYPTO_ENGINE_ENCRYPT_QUEUE,
    PARSER_CRYPTO_ENGINE_CONN_ALL,
    PARSER_CRYPTO_CARD,
} crypto_show_types;

/*
 * Parser clear commands
 */
typedef
enum crypto_clear_types_ {
    PARSER_CRYPTO_CLEAR_CONN,
} crypto_clear_types;

/*
 * Parser test commands
 */
typedef
enum crypto_test_types_ {
    PARSER_TEST_INITIATE_SESSION,
    PARSER_TEST_CRYPTO_PING,
    PARSER_TEST_CRYPTO_PING_PASSIVE,
    PARSER_TEST_VIP_GEN_DSS,
    PARSER_TEST_VIP_INIT_DSS,
    PARSER_TEST_VIP_SET_EPA_PRIV_MODE,
    PARSER_TEST_VIP_GEN_EPA_DSS_KEY,
    PARSER_TEST_DES_ENCRYPT,
    PARSER_TEST_DES_DECRYPT,
    PARSER_TEST_NIST_CFB8,
    PARSER_TEST_NIST_CFB64,
} crypto_test_types;
