/* $Id: exec_test_crypto.h,v 3.1.70.7 1996/08/27 06:30:58 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/exec_test_crypto.h,v $
 *------------------------------------------------------------------
 * exec_test_crypto.h - Test commands for crypto
 *
 * November 1995, Brian Weis
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_test_crypto.h,v $
 * Revision 3.1.70.7  1996/08/27  06:30:58  xliu
 * CSCdi67270:  crypto parser command clean up
 * Branch: California_branch
 * Incorporate comments from doc. review, as well as some syntax clean up.
 *
 * Revision 3.1.70.4  1996/07/15  10:05:26  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 3.1.70.3  1996/06/07  00:24:32  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Various cleanups & addition of "crypto ping".
 *
 * Revision 3.1.70.2  1996/05/12  23:12:31  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.1.70.1  1996/04/19  14:56:09  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 3.1.2.5  1996/04/17  23:47:37  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 3.1.2.4  1996/04/16  21:52:56  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 3.1.2.3  1996/03/26  20:26:02  xliu
 * Branch: IosSec_branch
 * Add new test command.
 *
 * Revision 3.1.2.2  1995/12/12  23:09:43  bew
 * Branch: IosSec_branch
 * Fixes for CS & better formatting of debug messages.
 *
 * Revision 3.1.2.1  1995/11/28  01:46:51  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 3.1  1995/11/22  23:17:43  bew
 * Placeholders for new locations of crypto parser files
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

EOLS	(priv_test_crypto_eol, test_crypto, 0);

/*
 * crypto nist-tests cfb8 [21|22|23|24|25|26]
 * crypto nist-tests cfb64 [21|22|23|24|25|26]
 */

NUMBER	(priv_test_nist_testnumber, priv_test_crypto_eol, priv_test_crypto_eol, 
	 OBJ(int,2), 21, 26, "Test number");

KEYWORD_ID (priv_test_nist_cfb64, priv_test_nist_testnumber, 
	    no_alt, OBJ(int,1), PARSER_TEST_NIST_CFB64,
	    "cfb64", "Execute DES CFB-64 tests message", PRIV_OPR);

KEYWORD_ID (priv_test_nist_cfb8, priv_test_nist_testnumber, 
	    priv_test_nist_cfb64, OBJ(int,1), PARSER_TEST_NIST_CFB8,
	    "cfb8", "Execute DES CFB-8 tests message", PRIV_OPR);

KEYWORD (priv_test_nist, priv_test_nist_cfb8, no_alt,
	 "nist-tests", "Execute NIST FIPS tests", PRIV_OPR|PRIV_HIDDEN);


/*
 * crypto des-test [encrypt|decrypt] [8|64] key <8-hex-bytes> \
 *        iv <8-hex-bytes> text <8-hex-bytes>
 */

HEXDATA (priv_test_crypto_des_plaintext_data, priv_test_crypto_eol,
	 no_alt, OBJ(string,4), OBJ(int,4), "Text (data) in Hex",
	 PARSEBUF);

KEYWORD	(priv_test_crypto_des_plaintext, priv_test_crypto_des_plaintext_data,
	no_alt, "text", "DES IV", PRIV_OPR);

HEXDATA (priv_test_crypto_des_iv_data, priv_test_crypto_des_plaintext,
	 no_alt, OBJ(string,3), OBJ(int,3), "IV in Hex",
	 PARSEBUF);

KEYWORD	(priv_test_crypto_des_iv, priv_test_crypto_des_iv_data, no_alt,
	 "iv", "DES IV", PRIV_OPR);

HEXDATA (priv_test_crypto_des_key_data, priv_test_crypto_des_iv,
	 no_alt, OBJ(string,2), OBJ(int,2), "DES key data (in hex)",
	 PARSEBUF);

KEYWORD	(priv_test_crypto_des_key, priv_test_crypto_des_key_data, no_alt,
	 "key", "DES key", PRIV_OPR);

NUMBER	(priv_test_crypto_des_mode, priv_test_crypto_des_key, no_alt, 
	 OBJ(int,5), 8, 64, "Number of CFB bits (8 or 64)");

KEYWORD_ID (priv_test_crypto_des_dec, priv_test_crypto_des_mode, 
	    no_alt, OBJ(int,1), PARSER_TEST_DES_DECRYPT,
	    "decrypt", "Test decryption mode", PRIV_OPR);

KEYWORD_ID (priv_test_crypto_des_enc, priv_test_crypto_des_mode, 
	    priv_test_crypto_des_dec, OBJ(int,1), PARSER_TEST_DES_ENCRYPT,
	    "encrypt", "Test encryption mode", PRIV_OPR);

KEYWORD (priv_test_crypto_des, priv_test_crypto_des_enc, priv_test_nist,
	    "des-test", "Test with known DES key, IV, and plaintext", 
	    PRIV_OPR|PRIV_HIDDEN);

/*
 * crypto ping
 * crypto ping passive
 */

KEYWORD_ID (priv_test_crypto_ping_passive, priv_test_crypto_eol, 
	    priv_test_crypto_eol, OBJ(int,1), PARSER_TEST_CRYPTO_PING_PASSIVE,
	    "passive", "Ping loopback test (passive)", PRIV_OPR|PRIV_HIDDEN);

KEYWORD_ID (priv_test_crypto_ping, priv_test_crypto_ping_passive, 
	    priv_test_crypto_des, OBJ(int,1), PARSER_TEST_CRYPTO_PING,
	    "ping", "Ping loopback test (active)", PRIV_OPR|PRIV_HIDDEN);

/*
 * crypto initiate-session S.R.C.X D.S.T.X
 */

NUMBER	(priv_test_crypto_map_num, priv_test_crypto_eol, no_alt,
	 OBJ(int,2), 0, CRYPTO_MAP_SEQ_MAX, 
	 "Crypto map statement number to use");

STRING	(priv_test_crypto_map, priv_test_crypto_map_num, no_alt,
	 OBJ(string,1), "Crypto map name to use");

IPADDR	(priv_test_crypto_initses_src, priv_test_crypto_map, 
	 no_alt, OBJ(paddr,1), "Destination IP address");

IPADDR	(priv_test_crypto_initses_dst, priv_test_crypto_initses_src, 
	 no_alt, OBJ(paddr,2), "Source IP address");

KEYWORD_ID (priv_test_crypto_initses, priv_test_crypto_initses_dst, 
	    priv_test_crypto_ping, OBJ(int,1), PARSER_TEST_INITIATE_SESSION,
	    "initiate-session", "Send a CIM connection message", PRIV_OPR);

KEYWORD	(priv_test_crypto, priv_test_crypto_initses, ALTERNATE,
	 "crypto", "Test crypto functions", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	priv_test_crypto
