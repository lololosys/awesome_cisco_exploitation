/* $Id: epad_test_chain.h,v 3.1.64.5 1996/08/21 00:25:27 ptran Exp $
 * $Source: /release/112/cvs/Xsys/pas/epad_test_chain.h,v $
 *------------------------------------------------------------------
 * epad_test_chain.h - Encryption Port Adaptor driver test parser
 *			chains.
 *
 * August 1995, Paulina D. Tran
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: epad_test_chain.h,v $
 * Revision 3.1.64.5  1996/08/21  00:25:27  ptran
 * Check in ESA diag that test set Cipher key.
 * CSCdi66636:  ESA needs to support set Cipher key
 * Branch: California_branch
 *
 * Revision 3.1.64.4  1996/08/12  16:06:43  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.64.3  1996/08/08  16:56:13  ptran
 * Allocate memory pool to be use by diag.  Avoid to destroy session when
 * ESA
 * is timeout or hang.
 * CSCdi65487:  ESA diag failed to allocate buffer
 * Branch: California_branch
 *
 * Revision 3.1.64.2  1996/06/27  07:04:27  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Branch: California_branch
 *
 * Revision 3.1.2.5  1996/02/14  23:45:02  ptran
 * Changed the default of 4E to use I/O mapped I/O.  This change only
 * effect
 * IosSec_branch.
 * Added parser commands to test the encryption key management.
 * Fixed lost of pak and particles when descriptor error occur.
 * Added statistic counters and parser command to display these
 * statistic.
 *
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.4  1996/02/02  23:14:28  ptran
 * Commit first release of EPA firmware.
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.3  1996/01/31  04:48:13  xliu
 * Branch: IosSec_branch
 * Updated epa driver.
 *
 * Revision 3.1.2.2  1996/01/08  19:23:52  ptran
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.1  1995/12/06  21:48:54  rmontino
 * Branch: IosSec_branch
 *
 * Revision 3.1  1995/11/09  12:57:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/23  02:27:26  ptran
 * Placeholder file for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

pdecl(etest_pa);

/******************************************************************
 * Show epa statistic
 *   statistic
 */
EOLNS    (epad_show_statistic_eol, epad_show_statistic_cmd);
KEYWORD (epad_show_statistic, epad_show_statistic_eol, no_alt,
	 "statistic", "key and frame ring statistic", PRIV_ROOT);

/****************************************************************************
 * show EPA LED
 *   led
 */
EOLNS(epad_show_epa_led_eols, epad_led_cmd);
KEYWORD(epad_show_epa_led, epad_show_epa_led_eols, epad_show_statistic,
	"led", "display epa led", PRIV_ROOT);

/****************************************************************************
 * show epa status and time
 *   status 
 */
EOLNS(epad_showepa_status_eols, epad_statustime_cmd);
KEYWORD(epad_show_epa_status, epad_showepa_status_eols, epad_show_epa_led,
	"status", "Retrieve epa status and time", PRIV_ROOT);

/****************************************************************************
 * Read HMA memory
 *   memRead <HMA handle to read from> <size of HMA handle>
 */
EOLNS(epa_memRead_eols, epa_memRead_cmd);
NUMBER(epa_memRead_size, epa_memRead_eols, no_alt,
       OBJ(int, 2), 0, SIZE_64K, "size of HMA handle");
NUMBER(epa_memRead_handle, epa_memRead_size, no_alt,
       OBJ(int, 1), -SIZE_64K, SIZE_64K, "HMA handle to read from");
KEYWORD(epa_memRead, epa_memRead_handle, epad_show_epa_status,
	"memRead", "hma memory read", PRIV_ROOT);

/****************************************************************************
 * Allocate HMA memory
 *   memAlloc <size of HMA memory to allocate> <attribute>
 */
EOLNS(epa_memAlloc_eols, epa_memAlloc_cmd);
NUMBER(epa_memAlloc_attr, epa_memAlloc_eols, no_alt,
       OBJ(int, 2), 0, SIZE_64K, "attribute");
NUMBER(epa_memAlloc_size, epa_memAlloc_attr, no_alt,
       OBJ(int, 1), 0, SIZE_64K, "size of HMA memory to allocate");
KEYWORD(epa_memAlloc, epa_memAlloc_size, epa_memRead,
	"memAlloc", "hma memory allocation", PRIV_ROOT);

/****************************************************************************
 * Free HMA memory
 *   memFree <handle>
 */
EOLNS(epa_memFree_eols, epa_memFree_cmd);
NUMBER(epa_memFree_handle, epa_memFree_eols, no_alt,
       OBJ(int, 1), -SIZE_64K, SIZE_64K, "handle of HMA block to deallocate");
KEYWORD(epa_memFree, epa_memFree_handle, epa_memAlloc,
	"memFree", "hma memory free", PRIV_ROOT);

/****************************************************************************
 * Generate Random number
 *   genRandom <size of random number> <HMA handle to generate random number>
 */
EOLNS(epa_genRandom_eols, epa_genRandom_cmd);
NUMBER(epa_genRandom_handle, epa_genRandom_eols, no_alt,
       OBJ(int, 2), -SIZE_64K, SIZE_64K, "HMA handle to generate random number");
NUMBER(epa_genRandom_size, epa_genRandom_handle, no_alt,
       OBJ(int, 1), 0, SIZE_64K, "size of random number");
KEYWORD(epa_genRandom, epa_genRandom_size, epa_memFree,
	"genRandom", "generate random number", PRIV_ROOT);

/****************************************************************************
 * Generate Digital Standard Signature key
 *   genDSSkey <secret> <size of modulus P and G>
 */
EOLNS(epa_genDSSkey_eols, epa_genDSSkey_cmd);
NUMBER(epa_genDSSkey_size, epa_genDSSkey_eols, no_alt,
       OBJ(int, 2), -SIZE_64K, SIZE_64K, "size of modulus P and G");
NUMBER(epa_genDSSkey_secret, epa_genDSSkey_size, no_alt,
       OBJ(int, 1), -SIZE_64K, SIZE_64K, "HMA handle for generate secret number");
KEYWORD(epa_genDSSkey, epa_genDSSkey_secret, epa_genRandom,
	"genDSSkey", "generate DSS key", PRIV_ROOT);

/****************************************************************************
 * Generate Digital Standard Signature
 *   genDSSsig <secret handle> 
 */
EOLNS(epa_genDSSsig_eols, epa_genDSSsig_cmd);
STRING(epa_genDSSsig_hash, epa_genDSSsig_eols, no_alt,
       OBJ(string, 1), "hash");
NUMBER(epa_genDSSsig_size, epa_genDSSsig_hash, no_alt,
       OBJ(int, 2), 0, SIZE_64K, "size of modulus P and G");
NUMBER(epa_genDSSsig_secret, epa_genDSSsig_size, no_alt,
       OBJ(int, 1), -SIZE_64K, SIZE_64K, "secret handle");
KEYWORD(epa_genDSSsig, epa_genDSSsig_secret, epa_genDSSkey,
	"genDSSsig", "generate DSS signature", PRIV_ROOT);

/****************************************************************************
 * Generate Diffie Hellman pair
 *   genDHpair <secret handle> <size of base and modulus>
 */
EOLNS(epa_genDHpair_eols, epa_genDHpair_cmd);
NUMBER(epa_genDHpair_size, epa_genDHpair_eols, no_alt,
       OBJ(int, 2), 0, SIZE_64K, "size of base and modulus");
NUMBER(epa_genDHpair_secret, epa_genDHpair_size, no_alt,
       OBJ(int, 1), -SIZE_64K, SIZE_64K, "secret handle");
KEYWORD(epa_genDHpair, epa_genDHpair_secret, epa_genDSSsig,
	"genDHpair", "generate DH pair", PRIV_ROOT);

/**************************************************************************** 
 * Get Diffie Hellman Shared number
 *   getDHshared <secret handle> <shared handle> <size of base and modulus>
 */
EOLNS(epa_getDHshared_eols, epa_getDHshared_cmd);
NUMBER(epa_getDHshared_size, epa_getDHshared_eols, no_alt,
       OBJ(int, 3), 0, SIZE_64K, "size of base and modulus");
NUMBER(epa_getDHshared_shared, epa_getDHshared_size, no_alt,
       OBJ(int, 2), -SIZE_64K, SIZE_64K, "shared handle");
NUMBER(epa_getDHshared_secret, epa_getDHshared_shared, no_alt,
       OBJ(int, 1), -SIZE_64K, SIZE_64K, "secret handle");
KEYWORD(epa_getDHshared, epa_getDHshared_secret, epa_genDHpair,
	"getDHshared", "get DH shared number", PRIV_ROOT);

/****************************************************************************
 * Set Cipher key
 *   setCipherkey <size> <shared> <key>
 */
EOLNS(epa_setCipherkey_eols, epa_setCipherkey_cmd);
NUMBER(epa_setCipherkey_key, epa_setCipherkey_eols, no_alt,
       OBJ(int, 3), -SIZE_64K, SIZE_64K, "HMA handle of DES and KAPPA key");
NUMBER(epa_setCipherkey_shared, epa_setCipherkey_key, no_alt,
       OBJ(int, 2), -SIZE_64K, SIZE_64K, "shared handle");
NUMBER(epa_setCipherkey_size, epa_setCipherkey_shared, no_alt,
       OBJ(int, 1), 0, SIZE_64K, "number of bytes in shared number");
KEYWORD(epa_setCipherkey, epa_setCipherkey_size, epa_getDHshared,
	"setCipherkey", "set DES KAPPA key", PRIV_ROOT);

/****************************************************************************
 * Create new session
 *   newSession <iv> <key> <method>
 */
EOLNS(epa_newSession_eols, epa_newSession_cmd);
NUMBER(epa_newSession_method, epa_newSession_eols, no_alt,
       OBJ(int, 3), -SIZE_64K, SIZE_64K, "encryption method");
NUMBER(epa_newSession_key, epa_newSession_method, no_alt,
       OBJ(int, 2), -SIZE_64K, SIZE_64K, "HMA handle to 8-byte session key");
NUMBER(epa_newSession_iv, epa_newSession_key, no_alt,
       OBJ(int, 1), -SIZE_64K, SIZE_64K, "HMA handle to 8-byte iv value");
KEYWORD(epa_newSession, epa_newSession_iv, epa_setCipherkey,
	"newSession", "Create new session control block", PRIV_ROOT);

/****************************************************************************
 * Free established session
 *   freeSession <handle>
 */
EOLNS(epa_freeSession_eols, epa_freeSession_cmd);
NUMBER(epa_freeSession_handle, epa_freeSession_eols, no_alt,
       OBJ(int, 1), -SIZE_64K, SIZE_64K, "HMA handle to session control block");
KEYWORD(epa_freeSession, epa_freeSession_handle, epa_newSession,
	"freeSession", "Release session control block", PRIV_ROOT);

/****************************************************************************
 * Set EPA password
 *   setPassword <size of passwd> <password>
 */
EOLNS(epa_setPasswd_eols, epa_setPasswd_cmd);
STRING(epa_setPasswd_passwd, epa_setPasswd_eols, no_alt,
       OBJ(string, 1), "password");
NUMBER(epa_setPasswd_size, epa_setPasswd_passwd, no_alt,
       OBJ(int, 1), 0, 20, "size of passwd");
KEYWORD(epa_setPasswd, epa_setPasswd_size, epa_freeSession,
	"setPassword", "set epa password", PRIV_ROOT);

/****************************************************************************
 * Set EPA secure hash value
 *  sha <size> <message>
 */
EOLNS(epa_sha_eols, epa_sha_cmd);
STRING(epa_sha_message, epa_sha_eols, no_alt,
       OBJ(string, 1), "message from which to compute secure hash");
NUMBER(epa_sha_size, epa_sha_message, no_alt,
       OBJ(int, 1), 0, SIZE_64K, "size of message");
KEYWORD(epa_sha, epa_sha_size, epa_setPasswd, 
	"sha", "secure hash", PRIV_ROOT);

/****************************************************************************
 * Verify Digital Standard Signature
 *   verDSSsig <size of modulus P and G> <hash> <R> <S> <public number>
 */
EOLNS(epa_verDSSsig_eols, epa_verDSSsig_cmd);
STRING(epa_verDSSsig_public, epa_verDSSsig_eols, no_alt,
       OBJ(string, 4), "public number");
STRING(epa_verDSSsig_S, epa_verDSSsig_public, no_alt,
       OBJ(string, 3), "S");
STRING(epa_verDSSsig_R, epa_verDSSsig_S, no_alt,
       OBJ(string, 2), "R");
STRING(epa_verDSSsig_hash, epa_verDSSsig_R, no_alt,
       OBJ(string, 1), "secure hash result");
NUMBER(epa_verDSSsig_size, epa_verDSSsig_hash, no_alt,
       OBJ(int, 1), 0, SIZE_64K, "size of modulus P and G");
KEYWORD(epa_verDSSsig, epa_verDSSsig_size, epa_sha,
	"verDSSsig", "Verify DSS signature", PRIV_ROOT); 

/****************************************************************************
 * Set EPA status
 *  setStatus <size of passwd> <mask bit> <flag> <password>
 */
EOLNS(epa_setStatus_eols, epa_setStatus_cmd);
STRING(epa_setStatus_passwd, epa_setStatus_eols, no_alt,
       OBJ(string, 1), "passwd");
NUMBER(epa_setStatus_flag, epa_setStatus_passwd, no_alt,
       OBJ(int, 3), 0, 1, "flag");
NUMBER(epa_setStatus_mask, epa_setStatus_flag, no_alt,
       OBJ(int, 2), 0, 3, "mask: 0=tamper, 1=xtract, 2=testing, 3=privmod");
NUMBER(epa_setStatus_size, epa_setStatus_mask, no_alt, 
       OBJ(int, 1), 0, SIZE_64K, "size of passwd");
KEYWORD(epa_setStatus, epa_setStatus_size, epa_verDSSsig,
	"setStatus", "Set EPA Status", PRIV_ROOT);

/****************************************************************************
 * Read EPA flash 
 * readflash <offset> <size> <verbosity level>
 */
EOLNS(etest_key_rf_eols, epad_key_rf_cmd);
NUMBER(etest_key_rf_verbose, etest_key_rf_eols, no_alt,
       OBJ(int,3), 0, 2, "verbose 0=quiet 1=verbose 2=detail");
NUMBER(etest_key_rf_size, etest_key_rf_verbose, no_alt,
       OBJ(int, 2), 0, MAX_FRAME_SIZE, "number of bytes to read from flash");
NUMBER(etest_key_rf_offset, etest_key_rf_size, no_alt,
       OBJ(int, 1), 0, MAX_FRAME_SIZE, 
       "offset from beginning of flash to begin reading");
KEYWORD(etest_key_readflash, etest_key_rf_offset, epa_setStatus,
	"readflash", "Read EPA flash", PRIV_ROOT);

/****************************************************************************
 * Write to EPA flash memory
 *   writeflash <offset> <size> <verbosity level>
 */
EOLNS(etest_key_wf_eols, epad_key_wf_cmd);
NUMBER(etest_key_wf_verbose, etest_key_wf_eols, no_alt,
       OBJ(int, 3), 0, 2, "verbose 0=quiet 1=verbose 2=detail");
NUMBER(etest_key_wf_size, etest_key_wf_verbose, no_alt,
       OBJ(int, 2), 0, MAX_FRAME_SIZE, "number of bytes to write to flash");
NUMBER(etest_key_wf_offset, etest_key_wf_size, no_alt,
       OBJ(int, 1), 0, MAX_FRAME_SIZE, 
       "offset from beginning of flash to begin writing");
KEYWORD(etest_key_writeflash, etest_key_wf_offset, etest_key_readflash,
	"writeflash", "Write to EPA flash", PRIV_ROOT);

/****************************************************************************
 * Clear all of EPA memory (including HMA non-volatile parameters)
 *   zeroize <vebosity level>
 */
EOLNS(etest_key_zeroize_eols, epad_key_zeroize_cmd);
NUMBER(etest_key_zeroize_verbose, etest_key_zeroize_eols, no_alt,
       OBJ(int, 1), 0, 2, "verbose 0=quiet 1=verbose 2=detail");
KEYWORD(etest_key_zeroize, etest_key_zeroize_verbose, etest_key_writeflash,
	"zeroize", "Zero out the EPA memeory", PRIV_ROOT);

/****************************************************************************
 * Run all the test available to test the Cylink 124 Public key processor
 * chip.
 *   all
 */
EOLNS(etest_key_all_eols, epad_key_all_cmd);
NUMBER(etest_key_all_verbose, etest_key_all_eols, no_alt,
       OBJ(int, 1), 0, 2, "verbose 0=quiet 1=verbose 2=detail");
KEYWORD(etest_key_all, etest_key_all_verbose, etest_key_zeroize,
	"all", "Run all key command tests", PRIV_ROOT);

/****************************************************************************
 * Key management test suite to test the Cylink 124 Public key processor chip
 *   key
 */
KEYWORD(etest_key, etest_key_all, ALTERNATE,
	"key", "Key Management Test Suite", PRIV_ROOT);

/****************************************************************************
 * Generate packets start from size 1 to maximum packet size allow.  
 * Send these packet to EPA for encryption, decryption or bitflip.
 *   frame <encryption/decryption/bitflip> <verbosity level>
 */
EOLNS(etest_frame_eol, epad_frame_cmd);
NUMBER(etest_frame_verbose, etest_frame_eol, no_alt,
       OBJ(int, 2), 0, 2, "verbose 0=quiet 1=verbose  2=detail");
NUMBER(etest_frame_num, etest_frame_verbose, no_alt,
       OBJ(int, 3), 0, MAXPACKET, "number of time to execute this test");
NUMBER(etest_frame_type, etest_frame_num, no_alt,
       OBJ(int, 1), 0, 1, "type 0=encryption, 1=bitflip");
KEYWORD(etest_frame, etest_frame_type, etest_key,
	"frame", "Frame Test Suite", PRIV_ROOT);

EOLNS(etest_encrypt_eol, epad_data_session_cmd);
NUMBER(etest_encrypt_verbose, etest_encrypt_eol, no_alt,
       OBJ(int, 4), 0, 2, "verbose 0=quiet 1=verbose  2=detail");
NUMBER(etest_encrypt_size, etest_encrypt_verbose, no_alt,
       OBJ(int, 3), 0, MAX_FRAME_SIZE, "packet size (0=random)");
NUMBER(etest_encrypt_numpack, etest_encrypt_size, no_alt,
       OBJ(int, 2), 1, MAXPACKET, "number of data packet to send");
NUMBER(etest_encrypt_numloop, etest_encrypt_numpack, no_alt,
       OBJ(int, 1), 1, MAXPACKET, "number of times to repeat this test");
KEYWORD(etest_encrypt, etest_encrypt_numloop, etest_frame,
	"encrypt", "set up session then encrypt/decrypt packet", PRIV_ROOT);

/****************************************************************************
 * Internally generate packet with a long word sequence and request the
 * EPA to encrypt, decrypt or just bitflip the data.
 *   traffic <form of packet scatter/contiguous> <num packet> <packet size>
 *           <offset> <encrypt/decrypt/bitflip> <verbosity level>
 */
EOLNS(etest_traffic_eol, epad_traffic_cmd);
NUMBER(etest_traffic_verbose, etest_traffic_eol, no_alt,
       OBJ(int, 6), 0, 2, "verbose 0=quiet 1=verbose 2=detail");
NUMBER(etest_traffic_mode, etest_traffic_verbose, no_alt,
       OBJ(int, 5), 0, 4, "mode 0=encrypt/decrypt 1=bitflips throughput 2=holdq 3=holdq is full 4=don't care");
NUMBER(etest_traffic_offset, etest_traffic_mode, no_alt,
       OBJ(int, 4), 0, MAX_OFFSET+1, "offset (100000=random)");
NUMBER(etest_traffic_size, etest_traffic_offset, no_alt,
       OBJ(int, 3), 0, MAX_FRAME_SIZE, "packet size (0=random)");
NUMBER(etest_traffic_num, etest_traffic_size, no_alt,
       OBJ(int, 2), 0, MAXPACKET, "num packets");
NUMBER(etest_traffic_type, etest_traffic_num, no_alt,
       OBJ(int, 1), 0, 2, "type 0=scattered 1=contiguous 2=random");
KEYWORD(etest_traffic, etest_traffic_type, etest_encrypt,
	"traffic", "start encrypt then decrypt packet", PRIV_ROOT);

/****************************************************************************
 * Process the descriptor ring on VIP side
 *   processing <frame/key ring>
 */
EOLNS(etest_process_eol, epad_process_rings_cmd);
NUMBER(etest_process_ring, etest_process_eol, no_alt,
        OBJ(int, 1), 0, 1, "0=frame 1=key");
KEYWORD(etest_intr, etest_process_ring, etest_traffic,
        "processing", "start processing packets on ring", PRIV_ROOT); 

/****************************************************************************
 * Download code to EPA
 *   download
 */
EOLNS(etest_download_eol, epad_download_cmd);
KEYWORD(etest_download, etest_download_eol, etest_intr,
        "download", "start download testing", PRIV_ROOT);
 
/****************************************************************************
 * Send init block to EPA which contain the description information
 *   sendinit
 */
EOLNS(etest_sendinit_eol, epad_sendinit_cmd);
KEYWORD(etest_sendinit, etest_sendinit_eol, etest_download,
        "sendinit", "send init block", PRIV_ROOT);
 
/****************************************************************************
 * Bring up test include mailbox test, interrupt test and buffer transfer test.
 * bring-up test  <pa_num> <test num>
 */
EOLNS(etest_bringup_eol, epad_bringup_test_cmd);
NUMBER(etest_bringup_X, etest_bringup_eol, no_alt,
        OBJ(int, 4), 1, 100000, "initial X---test 3 only");
NUMBER(etest_bringup_bufsize, etest_bringup_X, no_alt,
        OBJ(int, 3), 1, 100000, "bufsize in longwords---test 3 only");
NUMBER(etest_bringup_numtries, etest_bringup_bufsize, no_alt,
        OBJ(int, 2), 1, 100000, "Number of retries");
NUMBER(etest_bringup_testnum, etest_bringup_numtries, no_alt,
       OBJ(int, 1), 1, 3,
       "Bringup test number: 1=mailbox, 2=interrupt, 3=buffer transfer");
KEYWORD(etest_bringup, etest_bringup_testnum, etest_sendinit,
        "bringup", "start bringup testing", PRIV_ROOT);
 
/****************************************************************************
 * Test amcc chip functionality
 *   amcc
 */
EOLNS(etest_amcc_eol, epad_test_amcc_cmd);
KEYWORD(etest_amcc, etest_amcc_eol, etest_bringup,
        "amcc", "test AMCC functionality", PRIV_ROOT);

/****************************************************************************
 * Use to measure esa performance
 */
EOLS(epad_diags_eol, epad_diags_cmd,0);
NUMBER(epad_diags_part, epad_diags_eol, no_alt,
        OBJ(int, 6), 0, 1, "test particle #");
NUMBER(epad_diags_verbose, epad_diags_part, no_alt,
        OBJ(int, 5), 0, 1, "verbose");
NUMBER(epad_diags_delay, epad_diags_verbose, no_alt,
        OBJ(int, 4), 0, 1000000, "delay in microsecs");
NUMBER(epad_diags_size, epad_diags_delay, no_alt,
        OBJ(int, 3), 0, 2, "0=small(64) 1=large(1536) 2=larger(4096)");
NUMBER(epad_diags_numtries, epad_diags_size, no_alt,
        OBJ(int, 2), 1, 1000000000, "Number of retries");
NUMBER(epad_diags_bay, epad_diags_numtries, no_alt,
        OBJ(int, 1), 0, 6, "Bay number");
KEYWORD(epad_perf, epad_diags_bay, etest_amcc,
        "fastsend", "fastsend", PRIV_ROOT);
/****************************************************************************
 * test epa
 */
KEYWORD(test_epad, epad_perf, ALTERNATE,
        "epa", "Encryption port adapter test system", PRIV_ROOT);
#undef ALTERNATE
#define ALTERNATE test_epad
 

