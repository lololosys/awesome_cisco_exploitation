/* $Id: exec_debug_crypto.h,v 3.1.70.2 1996/06/07 00:24:29 bew Exp $
 * $Source: /release/112/cvs/Xsys/crypto/exec_debug_crypto.h,v $
 *------------------------------------------------------------------
 * exec_debug_crypto.h - Crypto debug commands
 *
 * June 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_crypto.h,v $
 * Revision 3.1.70.2  1996/06/07  00:24:29  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Various cleanups & addition of "crypto ping".
 *
 * Revision 3.1.70.1  1996/04/19  14:56:08  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 3.1.2.1  1995/11/28  01:46:49  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 3.1  1995/11/22  23:17:38  bew
 * Placeholders for new locations of crypto parser files
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug crypto key-exchange
 *
 */
EOLS	(debug_crypto_ke_eol, debug_command, DEBUG_CRYPTO_EXCHANGE);
KEYWORD (debug_crypto_ke, debug_crypto_ke_eol, no_alt,
	 "key-exchange", "Key Exchanger", PRIV_OPR);

/******************************************************************
 * debug crypto engine
 *
 */
EOLS	(debug_crypto_engine_eol, debug_command, DEBUG_CRYPTO_ENGINE);
KEYWORD (debug_crypto_engine, debug_crypto_engine_eol, debug_crypto_ke,
         "engine", "Crypto Engine Debug", PRIV_OPR);


/******************************************************************
 * debug crypto sessmgmt
 *
 */
EOLS	(debug_crypto_sm_eol, debug_command, DEBUG_CRYPTO_SM);
KEYWORD (debug_crypto_sm, debug_crypto_sm_eol, debug_crypto_engine,
	 "sessmgmt", "Session Management", PRIV_OPR);

/******************************************************************
 * debug crypto 
 *
 */
KEYWORD (debug_crypto_kwd, debug_crypto_sm, ALTERNATE,
	 "crypto", "Cryptographic subsysem", PRIV_OPR);

SET     (debug_crypto, debug_crypto_kwd, OBJ(pdb,1), crypto_debug_arr);

#undef	ALTERNATE
#define	ALTERNATE	debug_crypto
