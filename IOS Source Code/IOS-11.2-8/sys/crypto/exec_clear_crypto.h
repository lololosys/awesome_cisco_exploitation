/* $Id: exec_clear_crypto.h,v 3.1.70.3 1996/08/12 16:00:58 widmer Exp $
 * $Source: /release/112/cvs/Xsys/crypto/exec_clear_crypto.h,v $
 *------------------------------------------------------------------
 * exec_clear_crypto.h - Clear crypto commands
 *
 * June 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_clear_crypto.h,v $
 * Revision 3.1.70.3  1996/08/12  16:00:58  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.70.2  1996/06/07  00:24:28  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Various cleanups & addition of "crypto ping".
 *
 * Revision 3.1.70.1  1996/04/19  14:56:07  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 3.1.2.3  1996/04/16  21:52:49  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 3.1.2.2  1996/03/13  22:45:16  bew
 * Branch: IosSec_branch
 * The RSP/VIP environment (multiple crypto engines) are now supported
 * in crypto parser routines and crypto session mgmt code.
 *
 * Revision 3.1.2.1  1995/11/28  01:46:49  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 3.1  1995/11/22  23:17:37  bew
 * Placeholders for new locations of crypto parser files
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * clear crypto
 */

EOLNS	(exec_clear_crypto_eol, crypto_clear_command);

/*
 * clear crypto connections
 * OBJ(int,1) = Command
 * OBJ(int,2) = Connection id
 * OBJ(int,3) = Slot
 */

NUMBER (exec_clear_crypto_conn_slot, exec_clear_crypto_eol,
	exec_clear_crypto_eol, OBJ(int,3), 0, CRYPTO_MAX_SLOTS,
	"Chassis slot number");

IFELSE (exec_clear_crypto_conn_if, exec_clear_crypto_conn_slot,
	exec_clear_crypto_eol, crypto_chassis_has_slots());

SET     (exec_clear_crypto_conn_set, exec_clear_crypto_conn_if, 
	 OBJ(int,3), CRYPTO_INVALID_SLOT);

SIGNED_DEC  (exec_clear_crypto_conn_number, exec_clear_crypto_conn_set, 
	 no_alt, OBJ(int,2), CRYPTO_CONN_MINIMUM, CRYPTO_CONN_MAXIMUM, 
	 "Connection or Pending Connection Id");

KEYWORD_ID (exec_clear_crypto_conn, exec_clear_crypto_conn_number, 
	    no_alt, OBJ(int,1), PARSER_CRYPTO_CLEAR_CONN, "connection", 
	    "Clear a connection id", PRIV_ROOT);

KEYWORD	(exec_clear_crypto_base, exec_clear_crypto_conn, ALTERNATE,
	 "crypto", "Encryption subsystem", PRIV_ROOT);

#undef	ALTERNATE
#define	ALTERNATE	exec_clear_crypto_base
