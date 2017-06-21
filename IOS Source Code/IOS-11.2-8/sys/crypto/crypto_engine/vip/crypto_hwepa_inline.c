/* $Id: crypto_hwepa_inline.c,v 1.1.4.7 1996/07/28 07:44:35 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/vip/crypto_hwepa_inline.c,v $
 *------------------------------------------------------------------
 * crypto_hwepa_inline.c - inline functions to send command to EPA. 
 *
 * Mar, 1996, xliu 
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_hwepa_inline.c,v $
 * Revision 1.1.4.7  1996/07/28  07:44:35  xliu
 * CSCdi62947:  Add parser command to query epa status from RSP
 * This fix also solve the problems reported by CSCdi63488, CSCdi63852.
 * Branch: California_branch
 *
 * Revision 1.1.4.6  1996/07/15  09:59:30  xliu
 * CSCdi61651:  epa randome number seed is not initialized
 * Branch: California_branch
 *
 * Revision 1.1.4.5  1996/06/21  22:33:01  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 1.1.4.4  1996/05/28  16:00:13  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 1.1.4.3  1996/05/21  09:44:42  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 1.1.4.2  1996/05/12  23:13:38  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/04/19  14:57:20  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.2  1996/03/30  05:12:45  xliu
 * Branch: IosSec_branch
 * Commit before CA repository split.
 *
 * Revision 1.1.2.1  1996/03/26  08:55:10  xliu
 * Branch: IosSec_branch
 * Check in EPA support code.
 *
 * Revision 1.1  1996/03/26  08:49:02  xliu
 * Place holder files.
 *
 * Revision 1.7  1995/06/07 12:03:03  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "../ui/common_strings.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "sched.h"
#include "stacks.h"
#include "ttysrv.h"
#include "config.h"
#include "access.h"
#include "../ip/ip.h"
#include "../ipmulticast/igmp.h"
#include "../ip/ipaccess.h"
#include "../ip/ipaccount.h"
#include "../if/tring.h"
#include "../if/network.h"

#include "../src-vip/dfs_vip_idb.h"
#include "../src-vip/vip_debug.h"

#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_tcpip.h"
#include "../ip/ip_registry.h"
#include "../ip/ipfast_index.h"
#include "../ip/ipfast_ipc.h"
#include "../ip/ipfast_vip.h"

#include "../pas/if_pas.h"       /* includes pas_pci.h & segpak.h */
#include "../src-vip/vip_memd.h"

#include "../crypto/crypto_registry.h"
#include "../crypto/crypto_public.h"
#include "../crypto/crypto_map.h"
#include "../crypto/crypto.h"
#include "../crypto/crypto_engine/crypto_lib/lib_toolkit.h"
#include "../crypto/crypto_engine/crypto_lib/encrypt.h"
#include "../crypto/crypto_engine_api.h"
#include "../crypto/crypto_engine/core/crypto_engine.h"
#include "../crypto/crypto_engine/core/crypto_swepa.h"
#include "../crypto/crypto_engine/vip/crypto_engine_vip.h"
#include "../crypto/crypto_engine/rpsp/crypto_engine_rsp_ipc.h"

#include "crypto_hwepa.h"

inline void send_set_session_key_comm (ushort method, epa_crypto_blk *blk)
{
    epa_crypto_session_key_data_blk *data_blk = 
                                (epa_crypto_session_key_data_blk*)blk->data;

    /* Send the first command to set encrypt key. This command
     * will be registed in the pool
     */
    blk->header.seq_number = EPA_SET_ENCRYPT_KEY;
    blk->header.status = CRYPTO_ENGINE_REGIST_KEY_REQ;

    hSetCipherKey(
         crypto_vip_conntbl[data_blk->connection_id].hma_dh_shared,
         DH_LENGTH_MIN,
         crypto_vip_conntbl[data_blk->connection_id].hma_encrypt_key,
         SKEY_OFFSET,
         method,
         blk);
}

inline  void send_set_iv_key_comm (ushort method, epa_crypto_blk *blk)
{
    epa_crypto_session_key_data_blk *data_blk = 
                                (epa_crypto_session_key_data_blk*)blk->data;

    blk->header.seq_number = EPA_SET_IV_KEY;
    blk->header.status = CRYPTO_ENGINE_REGIST_KEY_REQ;

    hSetCipherKey(
        crypto_vip_conntbl[data_blk->connection_id].hma_dh_shared,
        DH_LENGTH_MIN,
        crypto_vip_conntbl[data_blk->connection_id].hma_iv_key,
        IKEY_OFFSET,
        method,
        blk);
}

inline void send_sha_comm (ushort next_comm, uchar *data, 
                           ushort data_length,
                           epa_crypto_blk *blk)
{
    
    blk->header.seq_number = next_comm;
    blk->header.status = CRYPTO_ENGINE_REGIST_KEY_REQ;

    hSHA (data, data_length, blk);
}

inline void send_hma_alloc_comm (ushort next_comm,
                                 ushort size,
                                 ushort attribute,
                                 epa_crypto_blk *blk) 
{
    blk->header.seq_number = next_comm;
    blk->header.status = CRYPTO_ENGINE_REGIST_KEY_REQ;

    hMemAlloc (size, attribute, blk);
}

inline void send_hma_free_comm (ushort next_comm,
                                 ushort handle,
                                 epa_crypto_blk *blk) 
{
    blk->header.seq_number = next_comm;
    blk->header.status = CRYPTO_ENGINE_REGIST_KEY_REQ;

    hMemFree (handle, blk);
}

inline void send_gen_dss_pair_comm (ushort handle, epa_crypto_blk *blk )
{
    blk->header.status = CRYPTO_ENGINE_REGIST_KEY_REQ;
    
    hGenDSSKey(handle, DSS_LENGTH_MIN, blk);
}

inline void send_gen_dh_pair_comm (ushort handle, epa_crypto_blk *blk)
{
    blk->header.status = CRYPTO_ENGINE_REGIST_KEY_REQ;
    
    hGenDHPair(handle, DH_LENGTH_MIN, blk);
}

inline void send_get_epa_status_comm (ushort next_comm, epa_crypto_blk *blk)
{
    blk->header.seq_number = next_comm;
    blk->header.status = CRYPTO_ENGINE_REGIST_KEY_REQ;


    hGetEPAStatus(blk);
}


inline void send_set_epa_passwd_comm (ushort next_comm, 
                                      uchar *passwd,
                                      ushort passwd_size,
                                      epa_crypto_blk *blk)
{
    blk->header.seq_number = next_comm;
    blk->header.status = CRYPTO_ENGINE_REGIST_KEY_REQ;

    hSetEPAPasswd(passwd, passwd_size, blk);
};

inline void send_set_epa_status_comm (ushort next_comm,
                                      ulong mask,
                                      ulong flag,
                                      epa_crypto_blk *blk)
{
    blk->header.seq_number = next_comm;
    blk->header.status = CRYPTO_ENGINE_REGIST_KEY_REQ;

    hSetEPAStatus(epa_passwd.size, mask, flag, epa_passwd.passwd, blk);
}

inline void send_set_new_session_comm (ushort ehandle,
                                  ushort ihandle,
                                  ushort method,
                                  epa_crypto_blk *blk)
{
    blk->header.status = CRYPTO_ENGINE_REGIST_KEY_REQ;

    hSetSession(ihandle, ehandle, method, blk);
}

inline void send_free_session_comm (ushort next_comm,
                                    ushort session_hdl,
                                    epa_crypto_blk *blk)
{
    blk->header.seq_number = next_comm;
    blk->header.status = CRYPTO_ENGINE_REGIST_KEY_REQ;

    hFreeSession(session_hdl, blk);
}

inline void send_gen_dss_sig_comm (ushort dss_p_handle,
                              uchar *sha,
                              ushort module_size,
                              epa_crypto_blk *blk)
{
    blk->header.status = CRYPTO_ENGINE_REGIST_KEY_REQ;
    hGenDSSSignature(dss_p_handle, sha, DSS_LENGTH_MIN, blk);
}

inline void send_ver_dss_sig_comm (uchar *sha, ushort module_size,
                              uchar *sig, uchar *public_key,
                              epa_crypto_blk *blk)
{
    blk->header.status = CRYPTO_ENGINE_REGIST_KEY_REQ;
    hVerDSSSignature(sha, module_size, (crypto_sig_t *)sig, public_key, blk);
}

inline void send_init_random_comm(ushort handle, ushort size,
                             epa_crypto_blk *blk)
{
    blk->header.status = CRYPTO_ENGINE_REGIST_KEY_REQ;
    hGenRandom (handle, size, blk);
}

inline void send_gen_dh_shr_comm (ushort dh_p_handle,
                                  ushort dh_s_handle,
                                  ushort module_size,
                                  uchar *pub_number,
                                  epa_crypto_blk *blk)
{
    blk->header.status = CRYPTO_ENGINE_REGIST_KEY_REQ;
    hGetDHShared(dh_p_handle,
                 dh_s_handle,
                 module_size,
                 pub_number,
                 blk);
}

inline void send_set_rn_seed (ushort next_comm,
                              uchar *seed,
                              ushort seed_size,
                              epa_crypto_blk *blk)
{
    blk->header.seq_number = next_comm;
    blk->header.status = CRYPTO_ENGINE_REGIST_KEY_REQ;

    hSetRNSeed (seed, seed_size, blk);
}

inline void send_zeroize (ushort next_comm,
                          epa_crypto_blk *blk)
{
    blk->header.seq_number = next_comm;
    blk->header.status = CRYPTO_ENGINE_REGIST_KEY_REQ;

    hZeroize(blk);
}

inline void send_gen_syndrome (ushort next_comm,
                               ushort dh_shared_handle,
                               ushort syndrome_len,
                               ushort offset,
                               epa_crypto_blk *blk)
{
    blk->header.seq_number = next_comm;
    blk->header.status = CRYPTO_ENGINE_REGIST_KEY_REQ;

    hGenSyndrome(dh_shared_handle, syndrome_len, offset, blk);

}

inline void send_hma_read (ushort next_comm,
                           ushort handle,
                           ushort size,
                           epa_crypto_blk *blk)
{

    if (crypto_engine_vip_debug) {
        buginf(  "\nCRYPTO_ENGINE: send hma read command.\n");
    }

    blk->header.status = CRYPTO_ENGINE_REGIST_KEY_REQ;
    blk->header.seq_number = next_comm;

    hMemRead (handle, size, blk);
}








    

    



 
 

