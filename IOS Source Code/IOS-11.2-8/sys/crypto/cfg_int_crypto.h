/* $Id: cfg_int_crypto.h,v 3.1.22.2 1996/08/15 01:12:57 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/cfg_int_crypto.h,v $
 *------------------------------------------------------------------
 * Parser gook for Crypto subsystem interface commands
 *
 * Sept 1995, David Carrel
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_crypto.h,v $
 * Revision 3.1.22.2  1996/08/15  01:12:57  xliu
 * CSCdi65622:  Cannnot apply crypto map to frame-relay subinterface
 * Branch: California_branch
 *
 * Revision 3.1.22.1  1996/04/19  14:55:25  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 3.1.2.1  1996/02/14  23:46:06  carrel
 * Initial Crypto Map commit
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * crypto map <tag>
 */
EOLS	(cfg_int_crypto_map_eol, crypto_map_command, PARSER_CRYPTO_MAP_INT);
STRING	(cfg_int_crypto_map_tag, cfg_int_crypto_map_eol, no_alt,
	 OBJ(string,1), "Crypto Map tag");
NOPREFIX(cfg_int_crypto_map_nopre, cfg_int_crypto_map_tag,
	 cfg_int_crypto_map_eol);
NVGENS	(cfg_int_crypto_map_nvgen, cfg_int_crypto_map_nopre,
	 crypto_map_command, PARSER_CRYPTO_MAP_INT);
KEYWORD	(cfg_int_crypto_map, cfg_int_crypto_map_nvgen, no_alt,
	 "map", "Assign a Crypto Map", PRIV_CONF|PRIV_SUBIF);

KEYWORD	(cfg_int_crypto, cfg_int_crypto_map, no_alt,
	 "crypto", "Encryption/Decryption commands", PRIV_CONF|PRIV_SUBIF);

ASSERT  (cfg_int_crypto_if, cfg_int_crypto, ALTERNATE, 
	 main_if_or_vc(csb->interface));

#undef  ALTERNATE
#define ALTERNATE	cfg_int_crypto_if
