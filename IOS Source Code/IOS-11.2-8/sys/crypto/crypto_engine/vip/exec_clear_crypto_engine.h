/* $Id: exec_clear_crypto_engine.h,v 1.1.50.1 1996/04/19 14:57:23 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/vip/exec_clear_crypto_engine.h,v $
 *------------------------------------------------------------------
 * Crypto Engine VIP clear parser command defintions.
 *
 * Jan, 1996,xliu 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_clear_crypto_engine.h,v $
 * Revision 1.1.50.1  1996/04/19  14:57:23  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.2  1996/03/25  23:19:48  xliu
 * Branch: IosSec_branch
 * Checkin VIP crypto engine code.
 *
 * Revision 1.1.2.1  1996/01/09  02:27:00  xliu
 * Branch: IosSec_branch
 * VIP software encryption.
 *
 * Revision 1.1  1996/01/09  02:12:01  xliu
 * Placeholder file.
 *
 * Revision 1.7  1995/06/07 12:03:03  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
EOLS	(exec_clear_crypto_eol, crypto_engine_clear_cmd, 0);

KEYWORD	(crypto_engine_clear, exec_clear_crypto_eol, ALTERNATE,
	 "crypto", "Encryption subsystem", PRIV_ROOT);

#undef	ALTERNATE
#define	ALTERNATE	crypto_engine_clear

