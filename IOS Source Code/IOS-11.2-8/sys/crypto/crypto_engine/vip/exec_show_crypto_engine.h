/* $Id: exec_show_crypto_engine.h,v 1.1.50.2 1996/05/28 16:00:17 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/vip/exec_show_crypto_engine.h,v $
 *------------------------------------------------------------------
 * Crypto Engine VIP show parser command defintions.
 *
 * Jan 1996, xxliu 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_crypto_engine.h,v $
 * Revision 1.1.50.2  1996/05/28  16:00:17  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 1.1.50.1  1996/04/19  14:57:24  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.4  1996/03/30  05:12:50  xliu
 * Branch: IosSec_branch
 * Commit before CA repository split.
 *
 * Revision 1.1.2.3  1996/03/25  23:19:53  xliu
 * Branch: IosSec_branch
 * Checkin VIP crypto engine code.
 *
 * Revision 1.1.2.2  1996/01/30  22:36:22  xliu
 * Branch: IosSec_branch
 * Checking in encryption RSP/VIP support code.
 *
 * Revision 1.1.2.1  1996/01/09  02:27:02  xliu
 * Branch: IosSec_branch
 * VIP software encryption.
 *
 * Revision 1.1  1996/01/09  02:12:02  xliu
 * Placeholder file.
 *
 * Revision 1.7  1995/06/07 12:03:03  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
 * show crypto engine vip
 */

EOLS	(exec_show_crypto_engine_eol, crypto_engine_show_cmd, 0);

/*
 * show crypto map
 */
KEYWORD_ID (exec_show_idb, exec_show_crypto_engine_eol, no_alt,
	    OBJ(int,1), PARSER_SHOW_IDB, "idb", 
	    "Show idb", PRIV_ROOT);

/*
 * show crypto map
 */
KEYWORD_ID (exec_show_map, exec_show_crypto_engine_eol, exec_show_idb,
	    OBJ(int,1), PARSER_SHOW_MAP, "crypto-map", 
	    "Show crypto map", PRIV_ROOT);
/*
 * show access control list
 */
NUMBER (acl_number, exec_show_crypto_engine_eol, exec_show_map,
        OBJ(int, 2), 0, 200, "acl-number");

KEYWORD_ID (exec_show_acl, acl_number, no_alt,
	    OBJ(int,1), PARSER_SHOW_ACL, "acl", 
	    "Show acl", PRIV_ROOT);

/*
 * show crypto engine statistics
 */

KEYWORD_ID (exec_show_crypto_engine_statis, exec_show_crypto_engine_eol, 
	    exec_show_acl, OBJ(int,1), 
            PARSER_SHOW_STATIS, "statistics", 
	    "Show crypto engine statistics", PRIV_ROOT);

/*
 * show crypto engine connnection table: <start_conn, end_conn>
 *
 */

KEYWORD_ID (exec_show_crypto_engine_conntbl, exec_show_crypto_engine_eol, 
	    exec_show_crypto_engine_statis, OBJ(int,1),
            PARSER_SHOW_CONN_TBL, 
            "connection table", 
	    "Show crypto engine connection table", PRIV_ROOT);

/*
 * show crypto engine configuration info 
 *
 */

KEYWORD (crypto_engine_show, exec_show_crypto_engine_conntbl, no_alt, "crypto-engine", 
         "Show crypto engine info", PRIV_ROOT);

#undef	ALTERNATE
#define	ALTERNATE	crypto_engine_show


