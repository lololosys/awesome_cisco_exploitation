/* $Id: crypto_engine_extern.h,v 1.1.68.1 1996/04/19 14:56:34 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/core/crypto_engine_extern.h,v $
 *------------------------------------------------------------------
 * crypto_engine_extern.h
 * 
 * This file defines the externs exported by crypto engine. 
 *
 * 11, 1995, xliu 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_engine_extern.h,v $
 * Revision 1.1.68.1  1996/04/19  14:56:34  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.1  1995/11/30  04:59:45  xliu
 * Branch: IosSec_branch
 *
 * Moved the crypto engine core files into dir CORE.
 *
 * Revision 1.1  1995/11/30  04:20:51  xliu
 * Place holder files.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
extern epa_engine_entry epa_engine_tbl[];
extern epa_idb_type epa_idb;
extern epa_engine_ident_struct epa_engine_ident;
extern ulong epa_req_id; 
extern ushort epa_test_flag;
extern ushort epa_conn_crypto_pair_tbl[]; 
extern ulong *epa_des_time;
extern ulong epa_encrypt_count;
extern ulong epa_decrypt_count;



