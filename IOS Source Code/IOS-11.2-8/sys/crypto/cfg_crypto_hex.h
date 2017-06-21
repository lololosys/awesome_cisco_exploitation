/* $Id: cfg_crypto_hex.h,v 3.1.70.1 1996/04/19 14:55:22 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/cfg_crypto_hex.h,v $
 *------------------------------------------------------------------
 * cfg_crypto_hex.h - Hex mode for entering crypto keys
 *
 * June 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_crypto_hex.h,v $
 * Revision 3.1.70.1  1996/04/19  14:55:22  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 3.1.2.1  1995/11/28  01:46:16  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 3.1  1995/11/22  23:17:36  bew
 * Placeholders for new locations of crypto parser files
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Public key hexmode
 */

/* There are two "commands": lines of data and the "quit" command. */
EOLS (crypto_hex_quit_eol, crypto_pubkey_hexmode, PARSER_CRYPTO_PUBKEY_QUIT);
KEYWORD (crypto_hex_quit, crypto_hex_quit_eol, ALTERNATE,
	 "quit", "Exit data entry mode", PRIV_USER);

/* Hex data are returned in string 1, with a length in int 1 */
EOLS (crypto_hex_data_eol, crypto_pubkey_hexmode, PARSER_CRYPTO_PUBKEY_DATA);
HEXDATA (crypto_hex_data, crypto_hex_data_eol, crypto_hex_quit,
	 OBJ(string,1), OBJ(int,1), "Hex data to append to parse buffer",
	 PARSEBUF);

#undef ALTERNATE
#define ALTERNATE crypto_hex_data
