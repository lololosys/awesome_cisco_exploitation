/* $Id: crypto_engine_api.c,v 1.1.68.15 1996/08/07 09:21:38 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/core/crypto_engine_api.c,v $
 *------------------------------------------------------------------
 * crypto_engine_api.c
 * 
 * This file defines the crypto engine api. 
 *
 * 11, 1995, xliu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_engine_api.c,v $
 * Revision 1.1.68.15  1996/08/07  09:21:38  xliu
 * CSCdi63274:  Wrong crypto engine name a show cr eng conf
 * Branch: California_branch
 *
 * Revision 1.1.68.14  1996/07/31  03:15:24  bew
 * CSCdi64649:  Router complains crypto zeroize failed
 * Branch: California_branch
 * Move the check for !context after the RP/RSP case where context is
 * not used.
 *
 * Revision 1.1.68.13  1996/07/28  07:44:02  xliu
 * CSCdi62947:  Add parser command to query epa status from RSP
 * This fix also solve the problems reported by CSCdi63488, CSCdi63852.
 * Branch: California_branch
 *
 * Revision 1.1.68.12  1996/07/22  18:55:41  bew
 * CSCdi62404:  password is not enabled when generating DSS key for EPA.
 * Branch: California_branch
 * When generating DSS keys, pass the password onto VIP if there is an
 * EPA on the VIP.
 *
 * Revision 1.1.68.11  1996/07/17  09:32:11  xliu
 * CSCdi63107:  Show crypto conn eng stat crashes router
 * Branch: California_branch
 *
 * Revision 1.1.68.10  1996/07/15  10:05:53  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 1.1.68.9  1996/07/06  19:44:57  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 1.1.68.8  1996/07/01  12:26:18  xliu
 * CSCdi61743:  decryption does not use cached pak pointer correctly.
 * Branch: California_branch
 *
 * Revision 1.1.68.7  1996/06/21  22:29:48  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 1.1.68.6  1996/06/11  08:48:56  xliu
 * CSCdi60097:  remove unused alg type from the case statement.
 * Branch: California_branch
 *
 * Revision 1.1.68.5  1996/06/07  00:26:25  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Change iv offset and encryption point if the IP payload is too small
 * (e.g., a fragment).
 *
 * Revision 1.1.68.4  1996/05/31  09:11:45  bew
 * CSCdi56084:  connection table
 * Branch: California_branch
 * An errant CRYPTO_ENGINE_ON_RP_RSP should have been
 * crypto_engine_rpsp_slot, correctly denoting the RP or RSP
 * crypto engine.
 *
 * Revision 1.1.68.3  1996/05/28  15:59:15  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 1.1.68.2  1996/05/12  23:12:44  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 1.1.68.1  1996/04/19  14:56:32  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.12  1996/04/17  23:47:58  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 1.1.2.11  1996/04/09  20:02:59  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 1.1.2.10  1996/03/21  10:07:18  xliu
 * Branch: IosSec_branch
 * Migrated to CA branch.
 *
 * Revision 1.1.2.6  1996/01/19  07:00:48  xliu
 * Branch: IosSec_branch
 * Commit before the second sync.
 *
 * Revision 1.1.2.5  1996/01/13  05:46:56  bew
 * Branch: IosSec_branch
 * Cleanup. Lots of cleanup in the IosSec_branch crypto directory.
 *
 * Revision 1.1.2.4  1996/01/04  18:35:43  xliu
 * Branch: IosSec_branch
 * After initial clean up.
 *
 * Revision 1.1.2.3  1995/12/10  09:27:15  xliu
 * Branch: IosSec_branch
 * Moved encryption from datagram_out to ipsendnet, after fragmentation.
 *
 * Revision 1.1.2.2  1995/12/02  17:42:45  xliu
 * Branch: IosSec_branch
 *
 * Modified epa driver interface.
 *
 * Revision 1.1.2.1  1995/11/30  04:59:44  xliu
 * Branch: IosSec_branch
 *
 * Moved the crypto engine core files into dir CORE.
 *
 * Revision 1.1  1995/11/30  04:20:50  xliu
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
#include "logger.h"
#include "types.h"
#include "interface_private.h"
#include "packet.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../util/random.h"
#include "../crypto/crypto_public.h"
#include "../crypto/crypto.h"
#include "../crypto/crypto_engine_api.h"
#include "../ui/debug.h"
#include "../crypto/crypto_debug.h"
#include "../crypto/crypto_registry.h"
#include "../crypto/crypto_map.h"
#include "../crypto/crypto_sesmgmt.h"
#include "../crypto/crypto_pubkey.h"
#include "../crypto/crypto_engine/crypto_lib/lib_toolkit.h"
#include "../crypto/crypto_engine_api.h"
#include "crypto_engine.h"
#include "crypto_swepa.h"
#include "../crypto/crypto_registry.h"
#include "../crypto/crypto_engine/crypto_lib/encrypt.h"
#include "../crypto/crypto_engine/crypto_lib/dss.h"

/* function prototypes */
epa_crypto_blk * get_crypto_req_blk (ushort, ulong); 
ulong crypto_swepa_error_pak_count =0;

/*
 * crypto_engine_scan
 *
 * Return crypto_engine_tbl. 
 */
void crypto_engine_scan (crypto_engine_config_t *engine_list)
{
   engine_list = &crypto_engine_tbl[0];
};

/*
 * epa_init    
 *
 * Initialize the selected crypto unit. Currently, the only thing done
 * by initialization is to copy the DSS signature key from NVRAM if the 
 * keys have been generated before. Otherwise, do nothing.
 *
 * input: epa_engine_id 
 * output: EPA_OP_OK[sync], EPA_INIT_SUCC [asyn]. 
 *         EPA_OP_FAIL[sync], EPA_INIT_FAIL [asyn]. 
 */

epa_status epa_init (ushort  crypto_engine_id,
                     epa_callback_context *context,
                     epa_callback_handle   callback)
{
    crypto_mykey *dss_keys = NULL;
    ushort status = EPA_OP_OK;
    long serial;

    /* 
     * On rpsp, the dsskey pair is retrived from crypto_my_key_list.
     * For crypto engine on vip with software crypto processes, the dss
     * key pair is send to vip through IPC. 
     */
    if (crypto_engine_id == ((int) crypto_engine_rpsp_slot)) {
        /* 
         * Get the platform serial number, and use that to find
	 * the right key pair in our key list.
	 */
	serial = crypto_processor_serial; /* Determined at boot time */

	if (!serial) {
	    status = EPA_OP_FAIL;
	    return status;
	}

	dss_keys = crypto_pubkey_get_keypair_by_serial(serial);
        if (dss_keys) {
            bcopy(dss_keys->private_key.key, crypto_pri_key,
                  dss_keys->private_key.keylen);
            bcopy(dss_keys->public_key.key, crypto_pub_key,
                  dss_keys->public_key.keylen);
        } else {
	    status = EPA_OP_FAIL;
        }
     } else {
         /*
          * Find dss key pairs for crypto engine on VIP.
          */
     }

   return status;

};

/*
 * crypto_engine_get_epa_status 
 */

epa_status crypto_get_epa_status (ushort crypto_engine_id,
                                  epa_callback_context *context,
                                  epa_callback_handle  callback)
{
    ushort status = EPA_OP_OK;
    epa_crypto_blk *blk;

    /* check the input sanity */
    if (!context) {
        if (crypto_engine_debug) {
            buginf("\nCrypto Engine: invalid input to crypto_get_info.");
        }
        return EPA_OP_FAIL;
    }

    epa_req_id++;

    /* Create the request block.  */
    blk = get_crypto_req_blk (CRYPTO_GET_EPA_STATUS, epa_req_id);
    if (!blk) {
        return EPA_OP_FAIL;
    }

    status = epa_engine_req_dspt(crypto_engine_id, 
                                 epa_req_id,
                                 CRYPTO_GET_EPA_STATUS,
                                 blk, 
                                 callback,
                                 context);
    
    return status;
}

/*
 * epa_zeroize
 *
 * Zeroize the protected memory. This command can only be called at 
 * PRIV_ROOT mode.
 */

epa_status epa_zeroize (ushort crypto_engine_id,
                        epa_callback_context *context,
                        epa_callback_handle callback)
{
    ushort       status = EPA_OP_OK;
    epa_crypto_blk* blk;
    ushort i;
    leveltype level;

    if (crypto_engine_id == crypto_engine_rpsp_slot) {
        memset(crypto_pub_key, 0x00, DSS_LENGTH_MIN);
        memset(crypto_pri_key, 0x00, SHA_LENGTH);
        level = raise_interrupt_level(NETS_DISABLE);
        for (i=0; i< CRYPTO_CONNTBL_SIZE; i++) {
            epa_swcrypto_conn_tbl[i].state = EPA_SWCRYPTO_CONN_UNSET;
        }
        reset_interrupt_level(level);
        return EPA_OP_OK;
    }
    
    if (!context) {
        return EPA_INVALID_INPUT;
    }

    epa_req_id++;

    /* Create the request block.  */
    blk = get_crypto_req_blk (CRYPTO_ZEROIZE, epa_req_id);
    if (!blk) {
        return EPA_OP_FAIL;
    }

    status = epa_engine_req_dspt(crypto_engine_id, 
                                 epa_req_id,
                                 CRYPTO_ZEROIZE,
                                 blk, 
                                 callback,
                                 context);

    return status;            
}

/*
 * epa_create_session_key
 *
 * Create a session key using the selected algorithm. Add the key to the
 * connection table.
 */

epa_status epa_create_session_key (ushort               crypto_engine_id,
                                   epa_alg_obj          *encrypt_alg,
                                   short                connection_id,
                                   epa_callback_context *context,
                                   epa_callback_handle  callback)
{
    ushort                           status = EPA_OP_OK;
    epa_crypto_blk*                  blk = NULL;
    epa_crypto_session_key_data_blk* data_blk = NULL;

    if (crypto_engine_debug) {
        buginf("\nCrypto engine %d: create session for conn id %d",
                 crypto_engine_id, connection_id);
    }

    /* check the input sanity */
    if (!context || connection_id < 0 || !encrypt_alg) {
        if (crypto_engine_debug) {
            buginf("\nCrypto Engine: invalid input ");
        }
        return EPA_INVALID_INPUT;
    }

    epa_req_id ++;

    /* Create the request block.  */
    blk = get_crypto_req_blk (CRYPTO_CREATE_SESSION_KEY, epa_req_id);
    if (!blk) {
        return EPA_OP_FAIL;
    }

    /* Create the data block.    */
    data_blk = (epa_crypto_session_key_data_blk *)blk->data;
    data_blk->connection_id = connection_id;
    data_blk->encrypt_alg_type = encrypt_alg->algorithm_type;

    blk->header.data_size = sizeof(epa_crypto_session_key_data_blk);

    status = epa_engine_req_dspt(crypto_engine_id, 
                                 epa_req_id,
                                 CRYPTO_CREATE_SESSION_KEY,
                                 blk, 
                                 callback,
                                 context);
    
    return status;
}

            
/*
 * epa_clear_connection_entry
 *
 * Clear the connection entry by setting the state to CRYPTO_CONN_CLEAR.
 */

epa_status epa_clear_connection_entry ( ushort crypto_engine_id,
                                        ushort connection_id,
                                        epa_callback_context *context,
                                        epa_callback_handle  callback)
{
    ushort status = EPA_OP_OK;
    epa_crypto_blk *blk;
    crypto_clear_conn_entry_data_blk *data_blk;
    leveltype level;
       
    if (crypto_engine_debug) {
        buginf("\nCrypto engine %d: delete connection %d",
                crypto_engine_id, connection_id);
    }

    if (connection_id >= CRYPTO_CONNTBL_SIZE) {
        if (crypto_engine_debug) {
            buginf("\nCrypto Engine: maximum connection number exceeded.");
        }
        return EPA_OP_FAIL;
    }

    /* 
     * If the connection is created on RP/RPS, clear the connection and
     * return. If the connection is create on VIP, send command to VIP.
     */
    if (crypto_engine_id == crypto_engine_rpsp_slot) {
        level = raise_interrupt_level(NETS_DISABLE);
        epa_swcrypto_conn_tbl[connection_id].state = EPA_SWCRYPTO_CONN_UNSET;
        reset_interrupt_level(level);
    } else {
        epa_req_id++;
        blk = get_crypto_req_blk (CRYPTO_CLEAR_CONN_ENTRY, epa_req_id);
        if (!blk) {
            return EPA_OP_FAIL;
        }
        data_blk = (crypto_clear_conn_entry_data_blk *)blk->data;
        data_blk->conn_id = connection_id;
        blk->header.data_size = sizeof(crypto_clear_conn_entry_data_blk);

        status = epa_engine_req_dspt(crypto_engine_id, 
                                     epa_req_id,
                                     CRYPTO_CLEAR_CONN_ENTRY,
                                     blk, 
                                     callback,
                                     context);
    }

     /*
      * If this is RSP platform, invalidate the ip cache entries associated
      * with the given connection so that the flow cache can get flushed out.
      */
    reg_invoke_crypto_invalidate_ip_cache(connection_id);

    return status;
}

/* 
 * epa_encdec_packet
 * 
 * Enqueue the packet to the encryption process queue. This is a common 
 * routine called by both encrypt/decrypt api.
 */
epa_status epa_encdec_packet ( ushort command_id,
                                      ushort  switch_mode,
                                      uchar*  data_buffer,
                                      ushort  data_size,
                                      short  connection_id,
                                      uchar*  iv_seed,
                                      ushort  iv_seed_size,
                                      crypto_pak_info* pakinfo)
{   
    crypto_encrypt_header   header;
    ushort  status = EPA_OP_OK;

    /* check the input sanity */
    if (!data_buffer) {
        if (crypto_engine_debug) {
            buginf("Crypto Engine: invalid input to crypto_encdec_packet.\n");
        }
        crypto_swepa_error_pak_count++;
        return EPA_INVALID_INPUT;
    };

    header.command = command_id;
    header.conn_id = connection_id;
    header.data = data_buffer;
    header.data_size = data_size;
    header.status = 0;
    header.iv_seed_size = iv_seed_size;
    bcopy(iv_seed, header.iv_seed, iv_seed_size);
    header.switch_mode = switch_mode;
    
    bcopy(pakinfo, &header.pak_info, sizeof(crypto_pak_info));

    /* 
     * If this is called from interrupt level by the fast/flow switch code,
     * call crypto_engine_fast_encrypt_enqueue, otherwise, call the 
     * routine which enqueue at the process level.
     */
    if (switch_mode == CRYPTO_PROCESS_SWITCH) { 
        crypto_engine_proc_encrypt_enqueue(&crypto_engine_idb, &header);
    } else {
        crypto_engine_fast_encrypt_enqueue(&crypto_engine_idb, &header);
    }

    return status; 
}

/*
 * epa_encrypt_packet
 *
 * Encrypt the packet and send the packet out to the output interface.
 * There is no asynchronous response to the caller. The crypto engine keeps
 * track of the dropped packets and any other errors returned from encryption.
 */

epa_status epa_encrypt_packet (uchar *encrypt_data_buffer,
                               ushort encrypt_data_size,
                               short  connection_id,
                               uchar* iv_seed,
                               ushort iv_seed_size,
                               crypto_pak_info *pakinfo)
{

    return (epa_encdec_packet(CRYPTO_ENCRYPT_PACKET,
                              pakinfo->switch_mode,
                              encrypt_data_buffer,
                              encrypt_data_size,
                              connection_id,
                              iv_seed,
                              iv_seed_size,
                              pakinfo));
}

/*
 * epa_decrypt_packet
 *
 * Decrypt the packet from the given offset with the key as selected by
 * the connection id.There is no asynchronous response to the caller. The 
 * crypto engine keeps track of the dropped packets and any other errors 
 * returned from decryption.
 */

epa_status epa_decrypt_packet (uchar *decrypt_data_buffer,
                               ushort decrypt_data_size,
                               short  connection_id,
                               uchar* iv_seed,
                               ushort iv_seed_size,
                               crypto_pak_info *pakinfo)
{

    return (epa_encdec_packet(CRYPTO_DECRYPT_PACKET,
                              pakinfo->switch_mode,
                              decrypt_data_buffer,
                              decrypt_data_size,
                              connection_id,
                              iv_seed,
                              iv_seed_size,
                              pakinfo));
}

/*
 * epa_generate_alg_parameters
 *
 * Generate parameters as needed by certain algorithm and initialize the 
 * connection table (if needed).
 */

epa_status epa_generate_alg_parameter (ushort        crypto_engine_id,
                                       epa_alg_obj   *alg,
                                       ushort        connection_id,
                                       epa_callback_context *context,
                                       epa_callback_handle  callback)
{
    ushort                         status;
    epa_crypto_blk*                blk = NULL;
    crypto_gen_dh_param_data_blk * data_blk = NULL;

    if (crypto_engine_debug) {
        buginf("\nCrypto engine %d: generate alg param\n",crypto_engine_id);
    } 

    /* check the input sanity */
    if (!context || !alg) {
        if (crypto_engine_debug) {
            buginf("\nCryptoEngine:invalid input");
        }
        return EPA_INVALID_INPUT;
    };

    epa_req_id ++;

    /* Create the request block.  */
    blk = get_crypto_req_blk (CRYPTO_GEN_ALG_PARAM, epa_req_id);
    if (!blk) {
        return EPA_OP_FAIL;
    }

    data_blk = (crypto_gen_dh_param_data_blk *)blk->data;
    data_blk->connection_id = connection_id;

    switch (alg->algorithm_type)  {
    case DH_ALG_PHASE1:
        blk->header.command_id = CRYPTO_GEN_DH_ALG_PHASE1_PARAM;
        break;

    case DH_ALG_PHASE2:
        blk->header.command_id = CRYPTO_GEN_DH_ALG_PHASE2_PARAM;
        bcopy(alg->type_tag.dh_alg_phase2.peer_public_number,
              data_blk->peer_public_number,
              DH_LENGTH_MIN);
        break;

    default:
        if (crypto_engine_debug) {
            buginf("Crypto Engine: invalid algorithm type.\n");
        }
        return EPA_INVALID_INPUT;
    };

    blk->header.data_size = sizeof(crypto_gen_dh_param_data_blk);

    status = epa_engine_req_dspt(crypto_engine_id, 
                                 epa_req_id,
                                 CRYPTO_GEN_DH_ALG_PHASE1_PARAM,
                                 blk, 
                                 callback,
                                 context);

    return EPA_OP_OK;
}

/*
 * epa_generate_key_pair
 *
 * Generate key pair using the given algorithm. 
 */

epa_status epa_generate_key_pair (ushort crypto_engine_id,
                                  epa_alg_obj *alg,
                                  uchar *passwd,
                                  ushort passwd_size,
                                  epa_callback_context *context,
                                  epa_callback_handle  callback)
{
    ushort                         status;
    epa_crypto_blk*                blk = NULL;
    crypto_gen_dss_key_data_blk*   data_blk;

    if (crypto_engine_debug) {
        buginf("\nCrypto engine %d: create key pairs.\n", crypto_engine_id);
    }

    /* check the input sanity */
    if (!context || !alg) {
        if (crypto_engine_debug) {
            buginf("\nCrypto Engine: invalid input to crypto_gen_key_pair.");
        }
        return EPA_INVALID_INPUT;
    };

    epa_req_id ++;

    /* Create the request block.  */
    blk = get_crypto_req_blk (CRYPTO_GEN_DSS_KEY_PAIR, epa_req_id);
    if (!blk) {
        return EPA_OP_FAIL;
    }

    switch (alg->algorithm_type) {
    case DSS_KEY_GEN:
        blk->header.command_id = CRYPTO_GEN_DSS_KEY_PAIR;
        blk->header.data_size = 0;
        break;

    default:
        if (crypto_engine_debug) {
            buginf("\nCrypto Engine: invalid algorithm type. %d\n",
                   alg->algorithm_type);
        }
        return EPA_INVALID_INPUT;
    };

    if (passwd) {
        data_blk = (crypto_gen_dss_key_data_blk *)blk->data;
        bcopy(passwd, data_blk->passwd, passwd_size);
        data_blk->passwd_size = passwd_size;
        blk->header.data_size = sizeof(crypto_gen_dss_key_data_blk);
    }

    status = epa_engine_req_dspt(crypto_engine_id, 
                                 epa_req_id,
                                 CRYPTO_GEN_KEY_PAIR,
                                 blk, 
                                 callback,
                                 context);

    return status;
}

/*
 * epa_sign
 *
 * Sign the given data using the private key and the given hash alg. 
 * Return the data and the signature back to SM.
 */

epa_status epa_sign (ushort  crypto_engine_id,
                     char *  data,
                     ushort  data_length,
                     epa_alg_obj *alg,
                     epa_callback_context *context,
                     epa_callback_handle  callback)
{
    ushort                         status;
    epa_crypto_blk*                blk = NULL;
    crypto_sign_data_blk*          data_blk = NULL;

    if (crypto_engine_debug) {
        buginf("\nCrypto engine %d: sign message using crypto engine",
                crypto_engine_id);
    }

    /* check the input sanity */
    if (!context || !data || !alg) {
        if (crypto_engine_debug) {
            buginf("\nCrypto Engine: invalid input to crypto_sign.");
        }
        return EPA_INVALID_INPUT;
    };

    epa_req_id ++;

    /* Create the request block.  */
    blk = get_crypto_req_blk (CRYPTO_SIGN, epa_req_id);
    if (!blk) {
        return EPA_OP_FAIL;
    }

    /*
     * In the following the signature and data are pointers 
     * defined for a given pak. Those pointers are valid until
     * the packet is send out from the output interface.
     */
    data_blk = (crypto_sign_data_blk *)blk->data;
    data_blk->data = data;
    data_blk->data_length = data_length;
    data_blk->alg_type = alg->algorithm_type;

    blk->header.data_size = sizeof(crypto_sign_data_blk);  

    status = epa_engine_req_dspt(crypto_engine_id, 
                                 epa_req_id,
                                 CRYPTO_SIGN,
                                 blk, 
                                 callback,
                                 context);

    return EPA_OP_OK;

}

/*
 * epa_verify
 *
 * Verify the data using the given public key and the given hash algorithm.
 * The result is return through callback.
 */

epa_status epa_verify (ushort      crypto_engine_id,
                       char        *clear_data,
                       ushort      clear_data_length,
                       char *      signature,
                       ushort      signature_length,
                       epa_alg_obj *alg,
                       epa_key_obj *public_key,
                       epa_callback_context *context,
                       epa_callback_handle  callback)
{

    ushort                         status;
    epa_crypto_blk*                blk = NULL;
    crypto_verify_data_blk*        data_blk = NULL;

   
    if (crypto_engine_debug) {
        buginf("\nCrypto engine %d: verify signature",
                crypto_engine_id);
    }
 
    /* check the input sanity */
    if (!context || !clear_data || !signature ||!alg || !public_key) {
        if (crypto_engine_debug) {
            buginf("\nCrypto Engine: invalid input to crypto_verify.");
        }
        return EPA_INVALID_INPUT;
    };

    epa_req_id ++;

    /* Create the request block.  */
    blk = get_crypto_req_blk (CRYPTO_VERIFY, epa_req_id);
    if (!blk) {
        return EPA_OP_FAIL;
    }

    /* 
     * In the following, the clear data and the signature are 
     * retrived from pak which is not valid until the
     * result is send back to the session manager.
     */

    data_blk = (crypto_verify_data_blk *)blk->data;
    data_blk->clear_data = clear_data;
    data_blk->clear_data_length = clear_data_length;
    data_blk->signature = signature;
    data_blk->signature_length = signature_length;
    data_blk->alg_type = alg->algorithm_type;
    bcopy(public_key->key_tag.dss_public_key.key_value, data_blk->public_key, 
          public_key->key_tag.dss_public_key.size);

    blk->header.data_size = sizeof(crypto_verify_data_blk) + 
                                     public_key->key_tag.dss_public_key.size; 

    status = epa_engine_req_dspt(crypto_engine_id, 
                                 epa_req_id,
                                 CRYPTO_VERIFY,
                                 blk, 
                                 callback,
                                 context);

    return EPA_OP_OK;
};

/*
 * crypto_create_digest
 *
 * Create the message digest using the given hasing algorithm.
 * This request is used by session manager to create finger prints
 * for manual key distribution. Therefore, it is only executed on
 * RP/RSP. The result is return in the synchronous way. 
 */

epa_status crypto_create_digest (epa_alg_obj* alg,
                                 uchar*       message, 
                                 ushort       message_bytes,
                                 uchar*       digest)
{

   switch (alg->algorithm_type) {
   case SHA_DIGEST:
       return (crypto_sha_digest (message, message_bytes, digest));
   default:
       /* Report error. Currently we only use sha. */
       return EPA_INVALID_INPUT;
   }
}

/*
 * crypto_pregen_dh_pairs
 *
 * Precompute DH pairs. Called by parser or Session mgt. 
 */ 

epa_status crypto_pregen_dh_pairs (ushort    crypto_engine_id,
                                   ushort    start_conn_id,
                                   ushort    end_conn_id,
                                   epa_callback_context *context,
                                   epa_callback_handle  callback)
                                 
{
    ushort i, status = EPA_OP_OK;
    epa_alg_obj alg;

    /* Check the sanity of the input */
    if ((start_conn_id >= CRYPTO_CONNTBL_SIZE) ||
        (end_conn_id >= CRYPTO_CONNTBL_SIZE )) {
        return EPA_INVALID_INPUT;
    }

    alg.algorithm_type = DH_ALG_PHASE1;

    for ( i = start_conn_id; i<= end_conn_id; i++) {
    
        status = epa_generate_alg_parameter (crypto_engine_id,
                                             &alg, i, context, callback);
        if (status != EPA_OP_OK) {
            if (crypto_engine_debug) {
                buginf("\nCRYPTO_ENGINE: fail to generate DH pair for "
                       "connection %d", i);
            }
            break;
        }
    }

    return status;
}

/*
 * crypto_set_alg
 * 
 * This api is not used now.
 */
    
epa_status crypto_set_alg (ushort      crypto_engine_id,
                           epa_alg_obj * alg,
                           short       connection_id)
{

    /* Check the sanity of the input */
    if (!alg || connection_id > CRYPTO_CONNTBL_SIZE || connection_id <= 0 ) {
        /* report error */
    }

    /* Update the algorithm for the given session. Now we only have
     * the crypto engine connection table udpated. Later, we need to 
     * update the connection table maintained by epa.
     */

    switch (alg->algorithm_type) {

        case DES_CFB64_IV1:
        case DES_CFB8_IV1:
            epa_swcrypto_conn_tbl[connection_id].alg_type=alg->algorithm_type;
            break;
        default:
            return EPA_OP_FAIL;
    }
    return EPA_OP_OK;
}

/*
 * get_crypto_req_blk
 * 
 * Malloc memory needed to create epa_crypto_blk. Initialize the
 * header. Called only at process level. The memory is freed when
 * 1) the response is received; 2) the timeout is expired.
 */
epa_crypto_blk * get_crypto_req_blk (ushort command,
                                                   ulong  req_id) 
{
    epa_crypto_blk *blk;

    /* request epa_crypto_req_blk */
    blk = epa_get_blk_buffer();
    if (!blk) {
        if (crypto_engine_debug) {
            buginf("\nCrypto Engine: can not allocate memory for key req.\n");
        }
        return NULL;
    };

    /* Initialize the request header.  */
    blk->header.req_id = req_id;
    blk->header.seq_number = CRYPTO_REQ_SEQ_END;
    blk->header.command_id = command;

    return blk;
}

/*
 * crypto_engine_dss_key_init
 *
 * Called to initialize the DSS key paris if they have been generated
 * before. 
 */
void crypto_engine_init_dss_keys(void)
{
    ushort i;

    for (i=0; i<CRYPTO_MAX_SLOTS; i++) {
       if ((crypto_engine_tbl[i].state == CRYPTO_ENGINE_INSTALLED)&&
           (crypto_engine_tbl[i].id != crypto_engine_rpsp_slot)) {
           epa_init(crypto_engine_tbl[i].id, NULL, NULL);
        }
    }
    epa_init(crypto_engine_rpsp_slot, NULL, NULL);
}

epa_status crypto_config_epa_card (ushort crypto_engine_id,
                                   uchar *passwd,
                                   ushort passwd_size,
                       		   epa_callback_context *context,
                       		   epa_callback_handle  callback)
{
    epa_crypto_blk *blk;
    ushort status;
    crypto_config_epa_data_blk*        data_blk = NULL;

    if (!passwd) {
        return EPA_INVALID_INPUT;
    }

    epa_req_id ++;

    /* Create the request block.  */
    blk = get_crypto_req_blk (CRYPTO_CONFIG_EPA, epa_req_id);
    if (!blk) {
        return EPA_OP_FAIL;
    }

    data_blk = (crypto_config_epa_data_blk *)blk->data;
    bcopy(passwd, data_blk->passwd, passwd_size);
    data_blk->passwd_size = passwd_size;
    blk->header.data_size = sizeof(crypto_config_epa_data_blk);

    status = epa_engine_req_dspt(crypto_engine_id, 
                                 epa_req_id,
                                 CRYPTO_CONFIG_EPA,
                                 blk, 
                                 callback,
                                 context);

    return status;
}
    
epa_status crypto_set_card_status (ushort slot,
                                   ulong  status)
{
    if (slot > CRYPTO_MAX_SLOTS) {
        return (EPA_INVALID_INPUT);
    }

    if (crypto_engine_tbl[slot].crypto_unit_type != CRYPTO_HWEPA) {
        return (EPA_INVALID_INPUT);
    }

    crypto_engine_tbl[slot].epa_status = status;

    return EPA_OP_OK;
}

epa_status crypto_get_card_status (ushort slot,
                                   ulong*  status)
{
    if (slot > CRYPTO_MAX_SLOTS) {
        return (EPA_INVALID_INPUT);
    }

    if (crypto_engine_tbl[slot].crypto_unit_type != CRYPTO_HWEPA) {
        return (EPA_INVALID_INPUT);
    }

    *status = crypto_engine_tbl[slot].epa_status;

    return EPA_OP_OK;
}
 
 /*
  * epa_generate_syndrome
  *
  * Create and return a syndrome for XORing with the Cylink challenge value.
  */
 
 epa_status epa_generate_syndrome (ushort         crypto_engine_id,
                                   ushort         connection_id,
                                   ushort         challenge_length,
                                   ushort         offset,
                                   epa_callback_context *context,
                                   epa_callback_handle  callback)
 {
     ushort                           status = EPA_OP_OK;
     epa_crypto_blk*                  blk = NULL;
     crypto_gen_syndrome_data_blk *data_blk = NULL;
 
     if (crypto_engine_debug) {
         buginf("\nCRYPTO ENGINE %d: get syndrome for conn id %d",
                  crypto_engine_id, connection_id);
     }
 
     /* check the input sanity */
     if (!context ||!challenge_length < 0) {
         if (crypto_engine_debug) {
             buginf("\nCRYPTO ENGINE: invalid input");
         }
         return EPA_INVALID_INPUT;
     }
 
     epa_req_id ++;
 
     /* Create the request block.  */
     blk = get_crypto_req_blk (CRYPTO_GEN_SYNDROME, epa_req_id);
     if (!blk) {
         return EPA_OP_FAIL;
     }
 
     /* Create the data block.    */
     data_blk = (crypto_gen_syndrome_data_blk *)blk->data;
     data_blk->connection_id = connection_id;
     data_blk->challenge_length = challenge_length;
     data_blk->offset = offset;
 
     blk->header.data_size = sizeof(crypto_gen_syndrome_data_blk);
 
     status = epa_engine_req_dspt(crypto_engine_id, 
                                  epa_req_id,
                                  CRYPTO_GEN_SYNDROME,
                                  blk, 
                                  callback,
                                  context);
     
     return status;
 }

/*
 * This is a synchronous command. No callback is enabled. 
 */
int crypto_clear_dh_numbers (int crypto_engine_id, int conn_id)
{
     ushort                           status = EPA_OP_OK;
     epa_crypto_blk*                  blk = NULL;
     crypto_clear_dh_number_data_blk *data_blk = NULL;
 
     if (crypto_engine_debug) {
         buginf("\nCRYPTO ENGINE %d: clear dh number for conn id %d",
                  crypto_engine_id, conn_id);
     }
 
     /* check the input sanity */
     if (conn_id < 0) {
         if (crypto_engine_debug) {
             buginf("\nCRYPTO ENGINE: invalid input");
         }
         return EPA_OP_FAIL;
     }

     epa_req_id++;
 
     /* Create the request block.  */
     blk = get_crypto_req_blk (CRYPTO_CLEAR_DH_NUMBER, epa_req_id);
     if (!blk) {
         return EPA_OP_FAIL;
     }
 
     /* Create the data block.    */
     data_blk = (crypto_clear_dh_number_data_blk *)blk->data;
     data_blk->connection_id = conn_id;
 
     blk->header.data_size = sizeof(crypto_clear_dh_number_data_blk);
 
     status = epa_engine_req_dspt(crypto_engine_id, 
                                  epa_req_id,
                                  CRYPTO_CLEAR_DH_NUMBER,
                                  blk, 
                                  NULL,
                                  NULL);

     return status;
     
}
 

