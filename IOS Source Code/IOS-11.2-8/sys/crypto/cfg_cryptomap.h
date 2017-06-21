/* $Id: cfg_cryptomap.h,v 3.1.22.3 1996/08/12 16:00:56 widmer Exp $
 * $Source: /release/112/cvs/Xsys/crypto/cfg_cryptomap.h,v $
 *------------------------------------------------------------------
 * Crypto map mode parse chains
 *
 * Sept 1995, David Carrel
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_cryptomap.h,v $
 * Revision 3.1.22.3  1996/08/12  16:00:56  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.22.2  1996/05/07  23:51:25  carrel
 * CSCdi56978:  Crypto Maps wont compile with named access lists
 * Branch: California_branch
 * Updated to use named access lists
 *
 * Revision 3.1.22.1  1996/04/19  14:55:24  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 3.1.2.4  1996/03/25  18:43:34  bew
 * Branch: IosSec_branch
 * Added 40-bit DES and DES 8-bit CFB support. Implemented partial
 * challenge processing support, not yet compatible with SDU.
 * Updated the crypto methods vector per Cylink's new definitions.
 *
 * Revision 3.1.2.3  1996/03/01  18:14:18  carrel
 * Branch: IosSec_branch
 * Fixed crypto map indenting problem.  (Thanks Rob!)
 *
 * Revision 3.1.2.2  1996/03/01  01:38:42  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 3.1.2.1  1996/02/14  23:16:59  carrel
 * Branch: IosSec_branch
 * New for crypto maps
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * exit from cryptomap mode
 */
EOLNS	(cm_exit_eol, exit_config_submode);
KEYWORD	(cm_exit_kwd, cm_exit_eol, NONE,
	 "exit", "Exit from crypto map configuration mode", PRIV_MIN);
TESTVAR (cm_exit, cm_exit_kwd, NONE, NONE, NONE, ALTERNATE, sense, TRUE);

/*
 * match mode
 *	match address <number>
 */
EOLS	(cm_match_addr_eol, crypto_map_eols, PARSER_CRYPTO_MAP_MATCH_ADDR);
GENERAL_STRING (cm_match_addr_grpname, cm_match_addr_eol, no_alt,
                OBJ(string,1), "Access-list name", STRING_HELP_CHECK);
TEST_MULTIPLE_FUNCS(cm_match_addr_str, cm_match_addr_grpname, NONE, no_alt);
NUMBER	(cm_match_addr_num, cm_match_addr_eol, cm_match_addr_str,
	 OBJ(int,2), MINSLOWACC, MAXSLOWACC, "IP access-list number");
NOPREFIX(cm_match_addr_nopre, cm_match_addr_num, cm_match_addr_eol);
NVGENS	(cm_match_addr_nvgen, cm_match_addr_nopre,
	 crypto_map_eols, PARSER_CRYPTO_MAP_MATCH_ADDR);
KEYWORD	(cm_match_addr, cm_match_addr_nvgen, no_alt,
	 "address", "Match address of packets to encrypt.", PRIV_CONF);

KEYWORD	(cm_match, cm_match_addr, cm_exit,
	 "match", "Match values.", PRIV_CONF);

/*
 * set mode
 *	set algorithm des|40-bit-des
 *	set peer <string>
 *	set mode sdu_compatibility
 * TBA:
 *	set key-timeout ...
 *	set key-max-data ...
 *	set ...
 */
EOLS	(cm_set_mode_eol, crypto_map_eols, PARSER_CRYPTO_MAP_SET_MODE);
KEYWORD_ID (cm_set_mode_sdu, cm_set_mode_eol, no_alt,
	    OBJ(int,2), PARSER_CRYPTO_MAP_SDU_MODE,
	    "sdu-compatibility", "Cylink SDU compatiblitity mode", PRIV_CONF);
NVGENS	(cm_set_mode_nvgen, cm_set_mode_sdu,
	 crypto_map_eols, PARSER_CRYPTO_MAP_SET_MODE);
KEYWORD	(cm_set_mode, cm_set_mode_nvgen, no_alt,
	 "mode", "Special mode characteristics.", PRIV_CONF);

EOLS	(cm_set_peer_eol, crypto_map_eols, PARSER_CRYPTO_MAP_SET_PEER);
STRING	(cm_set_peer_name, cm_set_peer_eol, no_alt,
	 OBJ(string,1), "Name of peer");
NVGENS	(cm_set_peer_nvgen, cm_set_peer_name,
	 crypto_map_eols, PARSER_CRYPTO_MAP_SET_PEER);
KEYWORD	(cm_set_peer, cm_set_peer_nvgen, cm_set_mode,
	 "peer", "Allowed Encryption/Decryption peer.", PRIV_CONF);

EOLS	(cm_set_alg_eol, crypto_map_eols, PARSER_CRYPTO_MAP_SET_ALG);
KEYWORD_ID (cm_set_alg_40_8, cm_set_alg_eol, cm_set_alg_eol,
	    OBJ(int,2), CRYPTO_DES_40_8,
	    CRYPTO_CFB8_STR, CRYPTO_CFB8_DESC, PRIV_CONF);
KEYWORD_ID (cm_set_alg_40_64, cm_set_alg_eol, cm_set_alg_40_8,
	    OBJ(int,2), CRYPTO_DES_40,
	    CRYPTO_CFB64_STR, CRYPTO_CFB64_DESC, PRIV_CONF);
KEYWORD_ID (cm_set_alg_40, cm_set_alg_40_64, no_alt,
	    OBJ(int,2), CRYPTO_DES_40,
	    CRYPTO_DES_40_STR, CRYPTO_DES_40_DESC, PRIV_CONF);
KEYWORD_ID (cm_set_alg_des_8, cm_set_alg_eol, cm_set_alg_eol,
	    OBJ(int,2), CRYPTO_DES_56_8,
	    CRYPTO_CFB8_STR, CRYPTO_CFB8_DESC, PRIV_CONF);
KEYWORD_ID (cm_set_alg_des_64, cm_set_alg_eol, cm_set_alg_des_8,
	    OBJ(int,2), CRYPTO_DES_56,
	    CRYPTO_CFB64_STR, CRYPTO_CFB64_DESC, PRIV_CONF);
KEYWORD_ID (cm_set_alg_des, cm_set_alg_des_64, cm_set_alg_40,
	    OBJ(int,2), CRYPTO_DES_56,
	    CRYPTO_DES_56_STR, CRYPTO_DES_56_DESC, PRIV_CONF);
IFELSE	(cm_set_alg_export_chk, cm_set_alg_des, cm_set_alg_40,
	 !crypto_is_exportable_system());
NVGENS	(cm_set_alg_nvgen, cm_set_alg_export_chk,
	 crypto_map_eols, PARSER_CRYPTO_MAP_SET_ALG);
KEYWORD	(cm_set_alg, cm_set_alg_nvgen, cm_set_peer,
	 "algorithm", "Encryption/Decryption algorithms to use.", PRIV_CONF);

KEYWORD	(cm_set, cm_set_alg, cm_match,
	 "set", "Set values for encryption/decryption", PRIV_CONF);


#undef	ALTERNATE
#define	ALTERNATE	cm_set
