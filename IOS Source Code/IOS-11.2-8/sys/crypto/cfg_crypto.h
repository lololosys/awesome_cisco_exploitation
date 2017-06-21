/* $Id: cfg_crypto.h,v 3.1.68.9 1996/08/15 04:52:25 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/cfg_crypto.h,v $
 *------------------------------------------------------------------
 * cfg_crypto.h - Config crypto parser macros
 *
 * June 1995, Brian Weis
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_crypto.h,v $
 * Revision 3.1.68.9  1996/08/15  04:52:25  xliu
 * CSCdi65990:  cr engine command is not functional
 * Branch: California_branch
 *
 * Revision 3.1.68.8  1996/08/12  16:00:55  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.68.7  1996/07/31  03:27:30  bew
 * CSCdi64375:  Invalid  function called in NVGEN of crypto clear-latch
 * Branch: California_branch
 * crypto clear-latch should be marked PRIV_NONVGEN. Also fixed the
 * error message in crypto_card_error_code to fully display.
 *
 * Revision 3.1.68.6  1996/07/28  07:41:47  xliu
 * CSCdi62947:  Add parser command to query epa status from RSP
 * Branch: California_branch
 *
 * Revision 3.1.68.5  1996/06/07  00:23:54  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Various cleanups & addition of "crypto ping".
 *
 * Revision 3.1.68.4  1996/06/05  23:11:13  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Fix crypto zeroize & key generation for 7500 with VIPs.
 * Fix crypto connection setup with traffic in both directions.
 * Fix encryption bugs.
 *
 * Revision 3.1.68.3  1996/05/31  06:53:13  bew
 * CSCdi51576:  no cr key-time...
 * Branch: California_branch
 * Fix no crypto key-timeout.
 *
 * Revision 3.1.68.2  1996/05/28  15:58:53  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 3.1.68.1  1996/04/19  14:55:21  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 3.1.2.13  1996/04/16  21:51:43  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 3.1.2.12  1996/03/25  18:43:33  bew
 * Branch: IosSec_branch
 * Added 40-bit DES and DES 8-bit CFB support. Implemented partial
 * challenge processing support, not yet compatible with SDU.
 * Updated the crypto methods vector per Cylink's new definitions.
 *
 * Revision 3.1.2.11  1996/03/21  10:06:59  xliu
 * Branch: IosSec_branch
 * Migrated to CA branch.
 *
 * Revision 3.1.2.10  1996/03/14  00:57:49  bew
 * Branch: IosSec_branch
 * Changed the crypto key-exchange protocol to be more intuitive.
 *
 * Revision 3.1.2.9  1996/03/13  22:44:43  bew
 * Branch: IosSec_branch
 * The RSP/VIP environment (multiple crypto engines) are now supported
 * in crypto parser routines and crypto session mgmt code.
 *
 * Revision 3.1.2.8  1996/03/01  01:38:40  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 3.1.2.7  1996/02/13  19:15:45  carrel
 * Branch: IosSec_branch
 * Crypto Map checkin (finally)
 *
 * Revision 3.1.2.6  1996/01/25  18:19:41  bew
 * Branch: IosSec_branch
 * Implemented a config option for automatic pre-genereration of
 * Diffie-Hellman key pairs. Also changed the use of timers so
 * that the the STUCKTMR message doesn't show up any more.
 *
 * Revision 3.1.2.5  1996/01/13  05:46:33  bew
 * Branch: IosSec_branch
 * Cleanup. Lots of cleanup in the IosSec_branch crypto directory.
 *
 * Revision 3.1.2.4  1995/12/20  23:30:39  bew
 * Branch: IosSec_branch
 * Public key exchange can now be two-way as well as one-way. Context is
 * now freed after a sign operation. Wrap-around of connection ids now
 * supported.  General cleanup.
 *
 * Revision 3.1.2.3  1995/12/02  17:44:27  xliu
 * Branch: IosSec_branch
 *
 * Add parser command to precompute dh number.
 *
 * Revision 3.1.2.2  1995/11/30  05:57:18  xliu
 * Branch: IosSec_branch
 *
 * Integrated with the new crypto engine code.
 *
 * Revision 3.1.2.1  1995/11/28  01:46:16  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 3.1  1995/11/22  23:17:35  bew
 * Placeholders for new locations of crypto parser files
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * crypto card clear-latch [slot]
 */
EOLNS    (cfg_crypto_card_eol, crypto_command);

NUMBER  (cfg_crypto_card_slot, cfg_crypto_card_eol, no_alt,
         OBJ(int,2), 0, CRYPTO_MAX_SLOTS-1, "Chassis slot number");

KEYWORD_ID (cfg_card_slot, cfg_crypto_card_slot, no_alt, 
	    OBJ(int,1), PARSER_CRYPTO_CARD_LATCH, 
	    "clear-latch", "Clear card extraction latch", 
	    PRIV_ROOT|PRIV_NONVGEN);

IFELSE	(cfg_crypto_card_if, cfg_card_slot, 
	 no_alt, crypto_chassis_has_slots());

/* crypto sdu connections <size>
 * crypto sdu entities <size>
 * OBJ(int,1) = Command value
 * OBJ(int,2) = Number of COT/PET Connections

 */
EOLNS    (cfg_crypto_sdu_pet_eol, crypto_command);

NUMBER   (cfg_crypto_sdu_pet_size, cfg_crypto_sdu_pet_eol, no_alt,
          OBJ(int,2),2,500, "Number of Entities <2-500> ");

NOPREFIX (cfg_crypto_sdu_pet_no, cfg_crypto_sdu_pet_size, 
          cfg_crypto_sdu_pet_eol);

NVGENS   (cfg_crypto_sdu_pet_nvgen, cfg_crypto_sdu_pet_no,
           crypto_command, PARSER_CRYPTO_SDU_PET_SIZE); 

KEYWORD_ID (cfg_crypto_sdu_pet, cfg_crypto_sdu_pet_nvgen, no_alt,
            OBJ(int,1), PARSER_CRYPTO_SDU_PET_SIZE,
            "entities", "set the Protected Entity Table size", PRIV_ROOT);

EOLNS      (cfg_crypto_sdu_cot_eol, crypto_command);

NUMBER     (cfg_crypto_sdu_cot_size, cfg_crypto_sdu_cot_eol, no_alt,
            OBJ(int,2),3,500, "Number of Connections <3-500> "); 

NOPREFIX (cfg_crypto_sdu_cot_no, cfg_crypto_sdu_cot_size, 
          cfg_crypto_sdu_cot_eol);
 
NVGENS     (cfg_crypto_sdu_cot_nvgen, cfg_crypto_sdu_cot_no,
            crypto_command, PARSER_CRYPTO_SDU_COT_SIZE);

KEYWORD_ID (cfg_crypto_sdu_cot, cfg_crypto_sdu_cot_nvgen, 
            cfg_crypto_sdu_pet, OBJ(int,1), PARSER_CRYPTO_SDU_COT_SIZE,
             "connections", "set the Connection Table size", PRIV_ROOT);

KEYWORD   (cfg_crypto_sdu, cfg_crypto_sdu_cot, cfg_crypto_card_if,
            "sdu", "Set the Number Connections/Entities size", PRIV_ROOT);

/*
 * crypto engine encrypt-queue <size>
 */
EOLNS    (cfg_crypto_engine_eol, crypto_engine_command);
NUMBER  (cfg_crypto_engine_encrypt_queue_size, cfg_crypto_engine_eol,
         cfg_crypto_engine_eol, OBJ(int,2), 500, 2000,
         "size of encryption input queue");
NVGENS   (cfg_crypto_engine_nvgen, cfg_crypto_engine_encrypt_queue_size,
          crypto_engine_command, PARSER_CRYPTO_ENGINE_ENCRYPT_QUEUE);
KEYWORD  (cfg_crypto_engine, cfg_crypto_engine_nvgen, cfg_crypto_sdu, 
          "engine", "Config crypto engine", PRIV_CONF);
/*
 * crypto map <tag> <seq-no>
 * no crypto map <tag> [<seq-no>]
 */
EOLS	(cfg_crypto_map_eol, crypto_map_command, PARSER_CRYPTO_MAP_GLOBAL);
NUMBER	(cfg_crypto_map_seqno_insert, cfg_crypto_map_eol, no_alt,
	 OBJ(int,2), 0, CRYPTO_MAP_SEQ_MAX,
	 "Sequence to insert to existing crypto map entry");
NUMBER	(cfg_crypto_map_seqno_delete, cfg_crypto_map_eol, cfg_crypto_map_eol,
	 OBJ(int,2), 0, CRYPTO_MAP_SEQ_MAX,
	 "Sequence to delete from existing crypto map entry");
IFELSE  (cfg_crypto_map_if, cfg_crypto_map_seqno_insert, 
	 cfg_crypto_map_seqno_delete, csb->sense);
SET	(cfg_crypto_map_set, cfg_crypto_map_if, OBJ(int,2), -1);
STRING	(cfg_crypto_map_tag, cfg_crypto_map_set, no_alt,
	 OBJ(string,1), "Crypto map tag");
NVGENS	(cfg_crypto_map_nvgen, cfg_crypto_map_tag, crypto_map_command,
	 PARSER_CRYPTO_MAP_GLOBAL);
KEYWORD	(cfg_crypto_map, cfg_crypto_map_nvgen, cfg_crypto_sdu,
	 "map", "Enter a crypto map", PRIV_CONF);



EOLNS	(cfg_crypto_eol, crypto_command);

/*
 * For 7500:
 *  crypto pregen-dh-pairs <number> [<slot>]
 * For others:
 *  crypto pregen-dh-pairs <number>
 * For everyone:
 *  no crypto preg-dh-pairs
 * OBJ(int,1) = Command value
 * OBJ(int,2) = Number of pairs to pregen
 * OBJ(int,3) = Chassis slot
 */

NUMBER  (cfg_crypto_pregen_dh_slot, cfg_crypto_eol,
         cfg_crypto_eol, OBJ(int,3), 0, CRYPTO_MAX_SLOTS-1, 
	 "Chassis slot number");

IFELSE	(cfg_crypto_pregen_dh_if, cfg_crypto_pregen_dh_slot, 
	 cfg_crypto_eol, crypto_chassis_has_slots());

SET     (cfg_crypto_pregen_set, cfg_crypto_pregen_dh_if, OBJ(int,3), 
	 CRYPTO_INVALID_SLOT); 

NUMBER  (cfg_crypto_pregen_dh_conn_number, cfg_crypto_pregen_set,
         no_alt, OBJ(int,2), 0, 10, "Number of pairs to pregenerate");

NOPREFIX (cfg_crypto_pregen_no, cfg_crypto_pregen_dh_conn_number, 
	  cfg_crypto_eol);

NVGENS	(cfg_crypto_pregen_dh_nvgens, cfg_crypto_pregen_no, 
	 crypto_command, PARSER_CRYPTO_PREGEN_DH);

KEYWORD_ID (cfg_crypto_pregen_dh, cfg_crypto_pregen_dh_nvgens, cfg_crypto_map,
            OBJ(int,1), PARSER_CRYPTO_PREGEN_DH,
            "pregen-dh-pairs", "Precompute DH Pair", PRIV_ROOT);

/*
 * crypto zeroize [<slot>]
 * This command delete DSS keys and connections.
 *
 * OBJ(int,1) = Command value
 * OBJ(int,2) = slot
 */

NUMBER  (cfg_crypto_zeroize_slot, cfg_crypto_eol,
         cfg_crypto_eol, OBJ(int,2), 0, CRYPTO_MAX_SLOTS-1, 
	 "Chassis slot number");

IFELSE	(cfg_crypto_zeroize_if, cfg_crypto_zeroize_slot, 
	 cfg_crypto_eol, crypto_chassis_has_slots());

SET     (cfg_crypto_zeroize_set, cfg_crypto_zeroize_if, OBJ(int,2), 
	 CRYPTO_INVALID_SLOT); 

KEYWORD_ID (cfg_crypto_zeroize, cfg_crypto_zeroize_set, cfg_crypto_pregen_dh, 
	    OBJ(int,1), PARSER_CRYPTO_ZEROIZE, 
	    "zeroize", "Zeroize all sensitive data", 
	    PRIV_ROOT|PRIV_NONVGEN);

/*
 * crypto key-exchange <peer-address> <peer-name> [<port>]
 * crypto key-exchange passive [<port>]
 *
 * OBJ(int,1) = Command value
 * OBJ(int,2) = Port
 * OBJ(string,1) = Key name
 * OBJ(paddr,1) = Peer IP address
 */

NUMBER  (cfg_crypto_key_exchange_port, cfg_crypto_eol, cfg_crypto_eol,
	 OBJ(int,2), 1025, 65536, "Port number for connection");

/*
 * The default port is set to 0. If later code finds a 0 then it picks
 * the default transport layer port number.
 */
SET     (cfg_crypto_key_exchange_set, cfg_crypto_key_exchange_port, 
	 OBJ(int,2), 0); 

STRING	(cfg_crypto_key_exchange_name, cfg_crypto_key_exchange_set, 
	 no_alt, OBJ(string,1), "Crypto engine name to send");

IPADDR  (cfg_crypto_key_exchange_addr, cfg_crypto_key_exchange_name,
	 no_alt, OBJ(paddr,1), "Peer IP address");

KEYWORD_ID (cfg_crypto_key_exchange_passive, cfg_crypto_key_exchange_set, 
	    cfg_crypto_key_exchange_addr, OBJ(int,1), 
	    PARSER_CRYPTO_KEY_EXCHANGE_PASSIVE, 
	    "passive", "Wait for a peer to contact this router",
	    PRIV_ROOT);

KEYWORD_ID (cfg_crypto_key_exchange, cfg_crypto_key_exchange_passive, 
	    cfg_crypto_zeroize, OBJ(int,1), PARSER_CRYPTO_KEY_EXCHANGE, 
	    "key-exchange", "Exchange DSS public keys with a peer", 
	    PRIV_ROOT|PRIV_NONVGEN);

/*
 * crypto key-timeout <minutes>
 *
 * OBJ(int,1) = Command value
 * OBJ(int,2) = Timeout number
 */

NUMBER  (cfg_crypto_key_timeout_value, cfg_crypto_eol, no_alt,
	 OBJ(int,2), 1, 1440, "Timeout in minutes");

NOPREFIX (cfg_crypto_key_timeout_no, cfg_crypto_key_timeout_value, 
	  cfg_crypto_eol);

NVGENS	(cfg_crypto_key_timeout_nvgens, cfg_crypto_key_timeout_no, 
	 crypto_command, PARSER_CRYPTO_KEY_TIMEOUT);

KEYWORD_ID (cfg_crypto_key_timeout, cfg_crypto_key_timeout_nvgens, 
	    cfg_crypto_key_exchange, OBJ(int,1), PARSER_CRYPTO_KEY_TIMEOUT, 
	    "key-timeout", 
	    "Set the session key timeout value", 
	    PRIV_ROOT);

/*
 * [no] crypto algorithm des [cfb-64]
 * [no] crypto algorithm des cfb-8
 * [no] crypto algorithm 40-bit-des [cfb-64]
 * [no] crypto algorithm 40-bit-des cfb-8
 *
 * OBJ(int,1) = Command value
 * OBJ(int,2) = Type of des
 */

/*
 * 40-bit-des macros
 */

KEYWORD_ID (cfg_crypto_alg_des40_8 , cfg_crypto_eol, 
	    cfg_crypto_eol, OBJ(int,2), CRYPTO_DES_40_8,
	    CRYPTO_CFB8_STR, CRYPTO_CFB8_DESC, PRIV_ROOT);

KEYWORD_ID (cfg_crypto_alg_des40_64 , cfg_crypto_eol, 
	    cfg_crypto_alg_des40_8, OBJ(int,2), CRYPTO_DES_40,
	    CRYPTO_CFB64_STR, CRYPTO_CFB64_DESC, PRIV_ROOT);

KEYWORD_ID (cfg_crypto_alg_des40, cfg_crypto_alg_des40_64, 
	    no_alt, OBJ(int,2), CRYPTO_DES_40,
	    CRYPTO_DES_40_STR, CRYPTO_DES_40_DESC, PRIV_ROOT);
/*
 * des macros
 */

KEYWORD_ID (cfg_crypto_alg_des56_8 , cfg_crypto_eol, 
	    cfg_crypto_eol, OBJ(int,2), CRYPTO_DES_56_8,
	    CRYPTO_CFB8_STR, CRYPTO_CFB8_DESC, PRIV_ROOT);

KEYWORD_ID (cfg_crypto_alg_des56_64 , cfg_crypto_eol, 
	    cfg_crypto_alg_des56_8, OBJ(int,2), CRYPTO_DES_56,
	    CRYPTO_CFB64_STR, CRYPTO_CFB64_DESC, PRIV_ROOT);
KEYWORD_ID (cfg_crypto_alg_des56, cfg_crypto_alg_des56_64,
	    cfg_crypto_alg_des40, OBJ(int,2), CRYPTO_DES_56,
	    CRYPTO_DES_56_STR, CRYPTO_DES_56_DESC, PRIV_ROOT);

IFELSE	(cfg_crypto_alg_export_chk, cfg_crypto_alg_des56, cfg_crypto_alg_des40,
	 !crypto_is_exportable_system());

NVGENS	(cfg_crypto_alg_nvgens, cfg_crypto_alg_export_chk, crypto_command, PARSER_CRYPTO_ALG_SET);

KEYWORD_ID (cfg_crypto_alg, cfg_crypto_alg_nvgens, cfg_crypto_key_timeout, 
	    OBJ(int,1), PARSER_CRYPTO_ALG_SET, /* for nvgens */
	    "algorithm", "Allow crypto algorithms", PRIV_ROOT);

/*
 * crypto init-epa
 *
 * There is no nvgen rule - this is intended to only be invoked manually; EPA's
 * are automatically initialized when their keys are found in the configuration.
 *
 * OBJ(int,1) = Command value
 */

KEYWORD_ID (cfg_crypto_init_epa, cfg_crypto_eol, 
	    cfg_crypto_alg, OBJ(int,1), PARSER_CRYPTO_INIT_EPA,
	    "epa-init", "Initialize an active EPA", PRIV_ROOT|PRIV_HIDDEN);

/*
 * crypto  DSS-key-pair <name> <serial>
 *
 * OBJ(int,1) = Command value
 * OBJ(int,2) = Serial number (for conn msgs)
 * OBJ(int,3) = Slot (crypto engine) keys are to be used
 * OBJ(string,1) = crypto engine name (for humans)
 */

NUMBER  (cfg_crypto_dss_keypair_slot, cfg_crypto_eol, no_alt,
	 OBJ(int,3), 0, CRYPTO_MAX_SLOTS-1, "Chassis slot number");

IFELSE	(cfg_crypto_dss_keypair_if, cfg_crypto_dss_keypair_slot, 
	 cfg_crypto_eol, crypto_chassis_has_slots());

SET     (cfg_crypto_dss_keypair_set, cfg_crypto_dss_keypair_if, OBJ(int,3), 
	 CRYPTO_INVALID_SLOT); 

HEXNUM	(cfg_crypto_dss_keypair_serial, cfg_crypto_dss_keypair_set, no_alt,
	 OBJ(int,2), "Serial number");

STRING	(cfg_crypto_dss_keypair_string, cfg_crypto_dss_keypair_serial, no_alt,
	 OBJ(string,1), "Crypto engine name");

NVGENS	(cfg_crypto_dss_keypair_nvgens, cfg_crypto_dss_keypair_string, 
	 crypto_command, PARSER_CRYPTO_DSS_KEYPAIR);

KEYWORD_ID (cfg_crypto_dss_keypair, cfg_crypto_dss_keypair_nvgens, 
	    cfg_crypto_init_epa, OBJ(int,1), PARSER_CRYPTO_DSS_KEYPAIR,
	    "DSS-key-pair", "Software EPA key pair", PRIV_ROOT|PRIV_HIDDEN);

/*
 *  crypto public-key <name> <serialnumber>
 *      < hex data > 
 *      < hex data >
 *      < ... >
 *      quit
 *
 *  no crypto public-key <name>
 *
 * OBJ(int,1) = Command value
 * OBJ(int,2) = Serial number
 * OBJ(string,2) = Peer name
 */
HEXNUM	(cfg_crypto_pub_serial, cfg_crypto_eol, no_alt,
	 OBJ(int,2), "Serial number");

NOPREFIX (cfg_crypto_pub_no, cfg_crypto_pub_serial, cfg_crypto_eol);

STRING	(cfg_crypto_pub_name, cfg_crypto_pub_no, no_alt,
	 OBJ(string,1), "Peer name");

NVGENS	(cfg_crypto_pub_nvgens, cfg_crypto_pub_name, crypto_command, 
	 PARSER_CRYPTO_PUBKEY);

KEYWORD_ID (cfg_crypto_pubkey, cfg_crypto_pub_nvgens, 
	    cfg_crypto_dss_keypair, OBJ(int,1), PARSER_CRYPTO_PUBKEY,
	    "public-key", "Peer public key", PRIV_ROOT);

/*
 * For 7500:
 *  crypto gen-signature-keys <crypto_engine_name> [<slot_number>]
 * All others:
 *  crypto gen-signature-keys <crypto_engine_name>
 *
 * OBJ(int,1) = Command value
 * OBJ(int,2) = Slot
 * OBJ(string,1) = Crypto engine name
 */

NUMBER  (cfg_crypto_sigkeys_slot, cfg_crypto_eol, cfg_crypto_eol,
	 OBJ(int,2), 0, CRYPTO_MAX_SLOTS-1, "Chassis slot number");

IFELSE	(cfg_crypto_sigkeys_if, cfg_crypto_sigkeys_slot, cfg_crypto_eol,
	 crypto_chassis_has_slots());

SET     (cfg_crypto_sigkeys_set_unit, cfg_crypto_sigkeys_if,
	 OBJ(int,2), CRYPTO_INVALID_SLOT);

STRING	(cfg_crypto_sigkeys_name, cfg_crypto_sigkeys_set_unit, no_alt,
	 OBJ(string,1), "Crypto subsystem name");

KEYWORD_ID (cfg_crypto_sigkeys, cfg_crypto_sigkeys_name, 
	    cfg_crypto_pubkey, OBJ(int,1), PARSER_CRYPTO_SIGKEYS, 
	    "gen-signature-keys", 
	    "Generate Signature keys for a crypto subsystem", 
	    PRIV_ROOT|PRIV_NONVGEN);
KEYWORD	(cfg_crypto_base, cfg_crypto_sigkeys, ALTERNATE,
	 "crypto", "Encryption module", PRIV_ROOT);

#undef	ALTERNATE
#define	ALTERNATE	cfg_crypto_base
