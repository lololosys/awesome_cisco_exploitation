/* $Id: crypto_hwepa_key.c,v 1.1.4.8 1996/07/23 13:20:57 thille Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/vip/crypto_hwepa_key.c,v $
 *------------------------------------------------------------------
 * crypto_hwepa_key.c - EPA key commands. 
 *
 * Mar 1996, xliu
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_hwepa_key.c,v $
 * Revision 1.1.4.8  1996/07/23  13:20:57  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 1.1.4.7  1996/07/01  12:27:00  xliu
 * CSCdi61743:  decryption does not use cached pak pointer correctly.
 * Branch: California_branch
 *
 * Revision 1.1.4.6  1996/06/30  18:11:12  ptran
 * CSCdi61257:  snmp and show diag support for encryption PA
 * Branch: California_branch
 *
 * Revision 1.1.4.5  1996/06/21  22:33:05  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 1.1.4.4  1996/06/01  01:15:11  xliu
 * CSCdi59234:  crypto rsp flow vector does not work correct for vip
 * encryption
 * Branch: California_branch
 *
 * Revision 1.1.4.3  1996/05/21  09:44:43  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 1.1.4.2  1996/05/12  23:13:39  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/04/19  14:57:21  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.3  1996/04/09  20:03:30  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 1.1.2.2  1996/03/30  05:12:46  xliu
 * Branch: IosSec_branch
 * Commit before CA repository split.
 *
 * Revision 1.1.2.1  1996/03/26  08:55:11  xliu
 * Branch: IosSec_branch
 * Check in EPA support code.
 *
 * Revision 1.1  1996/03/26  08:49:03  xliu
 * Place holder files.
 *
 * Revision 1.7  1995/06/07 12:03:03  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "logger.h"
#include "types.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "access.h"
#include "ttysrv.h"
#include "sched.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"

#include "../wan/serial.h"
#include "../ip/ip.h"
#include "../ipmulticast/igmp.h"
#include "../ip/ipaccess.h"
#include "../ip/ipaccount.h"
#include "../if/tring.h"
#include "../wan/frame_relay.h"
#include "../wan/atm_dxi.h"
#include "../wan/smds.h"
#include "../if/network.h"
#include "../if/atm.h"
#include "../util/random.h"

#include "../src-vip/dfs_vip_idb.h"

#include "../ip/ipfast_index.h"
#include "../ip/ipfast_ipc.h"
#include "../ip/ipfast_vip.h"
#include "../ip/ipfast_vip_ipc.h"

#include "sys_registry.h"

#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_tcpip.h"
#include "../ip/ip_registry.h"

#include "../pas/if_pas.h"       /* includes pas_pci.h & segpak.h */
#include "../src-vip/vip_memd.h"
#include "../src-vip/vip_debug.h"
#include "../crypto/crypto_public.h"
#include "../crypto/crypto_map.h"
#include "../crypto/crypto.h"
#include "../crypto/crypto_engine_api.h"
#include "../crypto/crypto_sesmgmt.h"
#include "../crypto/crypto_registry.h"
#include "../crypto/crypto_pubkey.h"
#include "../crypto/crypto_engine/crypto_lib/lib_toolkit.h"
#include "../crypto/crypto_engine/core/crypto_engine.h"
#include "../crypto/crypto_engine/core/crypto_swepa.h"
#include "../crypto/crypto_engine/crypto_lib/encrypt.h"
#include "../crypto/crypto_engine/crypto_lib/dss.h"
#include "crypto_engine_vip.h"
#include "crypto_hwepa.h"

static void copy_command_to_pak (uchar *,ushort,uchar *,ushort, paktype *);

/*
 * Register the key request into the request pool.
 *
 * Currently the simple indexing and sequential search alg is used to 
 * implement the pool. Later, hash alg can be used.
 */
epa_status reg_key_req (epa_crypto_blk *blk, ulong req_id)
{
   crypto_vip_key_req_context *context;
   ushort index;

   context = &crypto_vip_key_req_context_pool[0];

   for (index=0; index< CRYPTO_VIP_KEY_CONTEXT_POOL_SIZE; index++)
       if (context->state == CRYPTO_VIP_KEY_REQ_ENTRY_UNSET) {
           context->req_id = req_id;
           context->command = blk->header.command_id;
           context->conn_id = blk->header.status;
           context->state = CRYPTO_VIP_KEY_REQ_ENTRY_SET;
           context->blk = blk;
           return EPA_OP_OK;
       } else {
           context++;
       }
       
   return EPA_OP_FAIL;
}

/*
 * send_key_req
 *
 * This is the transportation routine called to send the defined 
 * command buffer to EPA. The command buffer is defined as data in
 * the particle carried by the pak. pak->crypto_dec_conn_id,
 * pak->crypto_enc_conn_id, and pal->cryptoflags 
 * fields are used to save command identification information. 
 * If the request don't need to be registered in the request pool, the
 * input parameter blk is NULL.
 */
static void send_key_req ( paktype *pak, epa_crypto_blk *blk, ulong req_id) 
{
    ushort status = EPA_OP_OK;
    leveltype level;

    /* 
     * Certian fields from paktype are used to carry information needed
     * either by the driver or by the crypto engine when processing the 
     * callback. 
     *
     * Update both the encryption and decryption connection id. Actually,
     * only one should be necessary for the callback to regain context.
     */
    if (blk) {
        pak->crypto_dec_conn_id = req_id;
        pak->crypto_enc_conn_id = req_id;
        pak->cryptoflags = blk->header.seq_number; 

        /* 
         * blk->header.status saves the flag to regist the request in the 
         * pool. For some requests, there is no need to regist. 
         */
        if ( blk->header.status) {
            status = reg_key_req (blk, req_id);
            if (status != EPA_OP_OK) {
                if (crypto_engine_vip_debug) {
                   buginf("\nCRYPTO_ENGINE: can not regist key req.\n");
                }
                return;
            }
        }
    }

    /* 
     * Send the request to EPA driver. Disable interrupt!
     */
    level = raise_interrupt_level(NETS_DISABLE);
    (* (crypto_engine_vip_config.epa_key_idb)->fastsend)(
                         crypto_engine_vip_config.epa_key_idb, pak);
    reset_interrupt_level(level);

    /*
     * Since epa driver will copy the pak into desc ring, and a 
     * static pak will be used to pass the saved content back, here we
     * return the pak to the drvier header pool.
     */
    retbuffer(pak);
} 

/*
 * get_comm_pak
 *
 * Allocate a pak for the key command send to the epa driver. 
 */
paktype * get_comm_pak (ushort size) 
{
    paktype *pak;
    particletype *mp = NULL;
    hwidbtype *idb = crypto_engine_vip_config.epa_key_idb;

    pak = pool_getbuffer(idb->headerpool);
    if (!pak) {
        return NULL;
    }
    pak->cryptoflags = 0;
    pak->crypto_enc_conn_id = 0;
    pak->crypto_dec_conn_id = 0;
    pak->datagramstart = 0;
    pak->datagramsize = 0;

    mp = getparticle_inline(idb->pool, idb->pool->size, TRUE);
    if (!mp) {
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: can not allocate particle.");
        }
        return NULL;
    }
    mp->data_bytes = size; 

    /* Clear out the data area */
    memset(mp->data_start, 0x00, size);
    particle_enqueue(pak, mp);

    return pak;
}

static void copy_command_to_pak (uchar *command, ushort comm_size,
                          uchar *data, ushort data_size,
                          paktype *pak)
{
    particletype *mp;
    uchar *bufptr;

    mp = pak->particlequeue.qhead;
    if (!mp) {
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE:no particle allocated for the command.");
        }
        return;
    }

    bcopy(command, mp->data_start, comm_size);
    bufptr = mp->data_start + comm_size;

    if (data_size != 0) {
        bcopy(data, bufptr, data_size);
    }
}

/*
 * create_comm_pak_
 *
 * Create command buffer according to the given command id and the input.
 *
 * Input:
 *    comm_id:  2 byte command id;
 *    input:    pointer to the input parameters;
 *    data:     pointer to the input data [not always present]
 * Output:
 *    pointer to the created buffer.
 * Note:
 *    After the command buffer is read by the wrapper functins, the memory
 *    should be freed.
 */

paktype* create_comm_pak (ushort comm_id, void *input, uchar *data)
{
    paktype * pak = NULL;
    ushort input_size = 0, input_data_size = 0, output_data_size = 0;
    ushort total_size;

    switch (comm_id) {

        case EPA_HMA_ALLOC:
            /* The output handle is included in the input */
            input_size = sizeof(crypto_hma_alloc_t);
            break;

        case EPA_HMA_FREE:
            input_size = sizeof (crypto_hma_free_t);
            break;

        case EPA_HMA_WRITE:
            input_size = sizeof(crypto_hma_write_t);
            input_data_size = ((crypto_hma_write_t *)input)->byte_to_write;
            break;

        case EPA_HMA_READ:
            input_size = sizeof (crypto_hma_read_t);
            output_data_size = ((crypto_hma_read_t*)input)->byte_to_read;
            break;

        case EPA_HMA_COPY:
            input_size = sizeof(crypto_hma_copy_t);
            break;

        case EPA_READ_RANDOM:
            input_size = sizeof(crypto_read_random_t);
            output_data_size = ((crypto_read_random_t*)input)->rand_num_size;
            break;

        case EPA_GEN_RANDOM:
            input_size = sizeof(crypto_gen_random_t);
            break;

        case EPA_SHA:
            input_size = sizeof (crypto_sha_t);
            input_data_size = ((crypto_sha_t *)input)->msg_size;
            output_data_size = 20;
            break;

        case EPA_GEN_DSS_KEY:
            /* the public number is included in the input */
            input_size = sizeof(crypto_gen_dss_key_t);
            break;

        case EPA_GEN_DSS_SIGNATURE:
            input_size = sizeof (crypto_gen_dss_signature_t);
            output_data_size = sizeof(crypto_sig_t);
            break;

        case EPA_VER_DSS_SIGNATURE:
            input_size = sizeof (crypto_verify_dss_signature_t);
            break;

        case EPA_GEN_DH_PAIR:
            /* Here we assume the input->modulus_size specifies the size
             * of the output DH public number.
             */
            input_size = sizeof(crypto_gen_dh_pair_t);
            break;

        case EPA_GET_DH_SHARED:
            input_size = sizeof(crypto_get_dh_shared_t );
            break;

        case EPA_SET_DESKAPPA_KEY:
            input_size = sizeof (crypto_set_deskappa_key_t);
            break;

        case EPA_SET_CIPHER_KEY:
            input_size = sizeof (crypto_set_cipher_key_t);
            break;

        case EPA_GET_EPA_STATUS:
            input_size = sizeof (crypto_get_epa_status_t);
            break;

        case EPA_SET_EPA_STATUS:
            input_size = sizeof (crypto_set_epa_status_t);
            input_data_size = ((crypto_set_epa_status_t*)input)->passwd_size;
            break;

        case EPA_SET_EPA_PASSWD:
            input_size = sizeof (crypto_set_epa_passwd_t);
            input_data_size = ((crypto_set_epa_passwd_t*)input)->passwd_size;
            break;

        case EPA_ZEROIZE_EPA:
            input_size = sizeof(crypto_zeroize_t);
            break;

        case EPA_READ_FLASH:
            break;

        case EPA_WRITE_FLASH:
            break;

        case EPA_SET_SESSION:
            input_size = sizeof (crypto_set_new_session_t);
            break;

        case EPA_FREE_SESSION:
            input_size = sizeof(crypto_free_session_t);
            break;

        case EPA_GET_TIME:
            break;

        case EPA_SET_TIME:
            break;

        case EPA_SET_RN_SEED:
            input_size = sizeof(crypto_set_rn_seed_t);
            input_data_size = ((crypto_set_rn_seed_t *)input)->seed_size;
            break;

        case EPA_MEM_SYNDROME:
            input_size = sizeof(crypto_gen_syndrome_t);
            output_data_size = ((crypto_gen_syndrome_t *)input)->syndrome_size;
            break;

        default:
            if (crypto_engine_vip_debug) {
                ttyprintf(CONTTY, "CRYPTO_ENGINE: invalid command id. %d\n",
                        comm_id);
            }
            break;
    }

    /*
     * Calculate the pak size.
     */
    total_size = input_size;
    if (input_data_size && data) {
        total_size += input_data_size;
    }
    if (output_data_size) {
        total_size += output_data_size;
    }

    pak = get_comm_pak(total_size);
    if (!pak) {
         if (crypto_engine_vip_debug) {
             ttyprintf(CONTTY,"EPA_KEY: can not get pak.\n");
         }
         return NULL;
    }
   
    copy_command_to_pak(input, input_size, data, input_data_size, pak);

    return pak;
}

epa_status hMemAlloc (ushort size, uchar attribute, epa_crypto_blk *blk)
{
    crypto_hma_alloc_t input;
    paktype *comm_pak;
   
    input.command = EPA_HMA_ALLOC;
    input.byte_to_alloc = size;
    input.attribute = attribute;
    input.reserved = 0x00;
    input.handle = 0x0000;
    input.status = 0x00;
  
    comm_pak = create_comm_pak(EPA_HMA_ALLOC, &input, NULL);
    if (!comm_pak) {
        if (crypto_engine_vip_debug) {
           buginf("\nCRYPTO_ENGINE: can not get command pak, abort.");
        }
        return EPA_OP_FAIL;
    }

    send_key_req (comm_pak, blk, crypto_engine_vip_key_req_id++);

    return EPA_OP_OK;

}

epa_status hMemFree ( ushort handle, epa_crypto_blk *blk)
{
    crypto_hma_free_t input;
    paktype *comm_pak;
   
    input.command = EPA_HMA_FREE;
    input.handle = handle;
   
    comm_pak = create_comm_pak (EPA_HMA_FREE, &input, NULL);
    if (!comm_pak) {
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: can not get command pak, abort.");
        }
        return EPA_OP_FAIL;
    }
    send_key_req (comm_pak, blk, crypto_engine_vip_key_req_id++);

    return EPA_OP_OK;
 
}

epa_status hMemRead (ushort handle, short size,  epa_crypto_blk *blk)
{
    crypto_hma_read_t input;
    paktype *comm_pak;

   
    input.command = EPA_HMA_READ;
    input.handle = handle;
    input.byte_to_read = size;
    input.status = 0x00;
    input.reserved = 0x0000;
   
    comm_pak = create_comm_pak (EPA_HMA_READ, &input, NULL);
    if (!comm_pak) {
        if (crypto_engine_vip_debug) {
             buginf("\nCRYPTO_ENGINE: can not get command pak, abort.");
        }
        return EPA_OP_FAIL;
    }
    send_key_req (comm_pak, blk, crypto_engine_vip_key_req_id++);

    return EPA_OP_OK;
}

epa_status hMemWrite (ushort handle, short size, void *data, epa_crypto_blk *blk)
{
    crypto_hma_write_t input;
    paktype *comm_pak;
   
    input.command = EPA_HMA_WRITE;
    input.handle = handle;
    input.byte_to_write = size;
    input.status = 0x00;
   
    comm_pak = create_comm_pak (EPA_HMA_WRITE, &input, data);
    if (!comm_pak) {
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: can not get command pak, abort.\n");
        }
        return EPA_OP_FAIL;
    }
    send_key_req (comm_pak, blk, crypto_engine_vip_key_req_id++);

    return EPA_OP_OK;
}

epa_status hGenRandom ( ushort hdl, ushort size , epa_crypto_blk *blk)
{
    crypto_gen_random_t input;
    paktype *comm_pak;
   
    input.command = EPA_GEN_RANDOM;
    input.rand_num_size = size;
    input.handle = hdl;
    input.status = 0x00;
   
    comm_pak = create_comm_pak (EPA_GEN_RANDOM, &input, NULL);
    if (!comm_pak) {
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: can not get command pak, abort.");
        }
        return EPA_OP_FAIL;
    }
    send_key_req (comm_pak, blk, crypto_engine_vip_key_req_id++);

    return EPA_OP_OK;

}

epa_status hReadRandom	( ushort size, uchar *random, epa_crypto_blk *blk )
{
    crypto_read_random_t input;
    paktype *comm_pak;
   
    input.command = EPA_READ_RANDOM;
    input.rand_num_size = size;
    input.status = 0x00;
   
    comm_pak = create_comm_pak (EPA_READ_RANDOM, &input, NULL);
    if (!comm_pak) {
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: can not get command pak, abort.");
        }
        return EPA_OP_FAIL;
    }
    send_key_req (comm_pak, blk, crypto_engine_vip_key_req_id++);

    return EPA_OP_OK;

}

epa_status hGenDSSKey(ushort handle, ushort modulus_size, epa_crypto_blk *blk)
{
    crypto_gen_dss_key_t input;
    paktype *comm_pak;
    ushort i;
   
    input.command = EPA_GEN_DSS_KEY;
    input.handle = handle;
    input.modulus_size = modulus_size;
    input.status = 0x00;
    input.reserved = 0x0000;

    for (i=0; i<modulus_size; i++) input.public_number[i] = 0x00;
   
    comm_pak = create_comm_pak (EPA_GEN_DSS_KEY, &input, NULL);
    if (!comm_pak) {
        if (crypto_engine_vip_debug) {
            buginf("CRYPTO_ENGINE: can not get command pak, abort.");
        }
        return EPA_OP_FAIL;
    }
    send_key_req (comm_pak, blk, crypto_engine_vip_key_req_id++);

    return EPA_OP_OK;

}            

epa_status hGenDSSSignature (ushort handle, uchar *hash, 
                             ushort modulus_size, epa_crypto_blk *blk)	
{
    crypto_gen_dss_signature_t input;
    paktype *comm_pak;
   
    input.command = EPA_GEN_DSS_SIGNATURE;
    input.private_key_handle = handle;
    input.modulus_size = modulus_size;
    bcopy(hash, input.sha_hash, CRYPTO_SHA_HASH_SIZE);
    input.status = 0x00;
    input.reserved = 0x0000;
   
    comm_pak = create_comm_pak (EPA_GEN_DSS_SIGNATURE, &input, NULL);
    if (!comm_pak) {
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: can not get command pak, abort.");
        }
        return EPA_OP_FAIL;
    }
    send_key_req (comm_pak, blk, crypto_engine_vip_key_req_id++);

    return EPA_OP_OK;

}            

epa_status hVerDSSSignature (uchar* hash, ushort modulus_size, 
                             crypto_sig_t *sig, uchar *public_number, 
                             epa_crypto_blk *blk)
{
    crypto_verify_dss_signature_t input;
    paktype *comm_pak;
   
    input.command = EPA_VER_DSS_SIGNATURE;
    input.modulus_size = modulus_size;
    bcopy(hash, input.sha_hash, CRYPTO_SHA_HASH_SIZE);
    bcopy(sig->sig_r, input.sig.sig_r, CRYPTO_SIG_R_SIZE);
    bcopy(sig->sig_s, input.sig.sig_s, CRYPTO_SIG_S_SIZE);
    bcopy(public_number, input.public_number, DH_LENGTH_MIN);
    input.status = 0x00;

    comm_pak=create_comm_pak (EPA_VER_DSS_SIGNATURE, &input, NULL);
    if (!comm_pak) {
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: can not get command pak.");
        }
        return EPA_OP_FAIL;
    }
    send_key_req (comm_pak, blk, crypto_engine_vip_key_req_id++);

    return EPA_OP_OK;
}

epa_status hGenDHPair (ushort handle, ushort modulus_size, epa_crypto_blk *blk)
{
    crypto_gen_dh_pair_t input;
    paktype *comm_pak;
    ushort i;
   
    input.command = EPA_GEN_DH_PAIR;
    input.private_number_handle = handle;
    input.modulus_size = modulus_size;
    input.status = 0x00;

    for (i=0; i<modulus_size; i++) input.public_number[i] = 0x00;
   
    comm_pak = create_comm_pak (EPA_GEN_DH_PAIR, &input, NULL);
    if (!comm_pak) {
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: can not get command pak, abort.");
        }
        return EPA_OP_FAIL;
    }
    send_key_req (comm_pak, blk, crypto_engine_vip_key_req_id++);

    return EPA_OP_OK;

}        

epa_status hGetDHShared(ushort p_handle, ushort s_handle, ushort modulus_size,
                       uchar* public_number, epa_crypto_blk *blk)
{
    crypto_get_dh_shared_t input;
    paktype *comm_pak;
   
    input.command = EPA_GET_DH_SHARED;
    input.private_number_handle = p_handle;
    input.shared_number_handle = s_handle;
    input.modulus_size = modulus_size;
    bcopy(public_number, input.public_number, CRYPTO_DH_PUBLIC_SIZE);
    input.status = 0x00;

    comm_pak = create_comm_pak (EPA_GET_DH_SHARED, &input, NULL);
    if (!comm_pak) {
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: can not get command pak, abort.");
        }
        return EPA_OP_FAIL;
    }
    send_key_req (comm_pak, blk, crypto_engine_vip_key_req_id++);

    return EPA_OP_OK;

}            

epa_status hSetCipherKey (ushort s_handle,ushort number_of_bytes, 
                          ushort k_handle, ushort offset,
                          ushort method, epa_crypto_blk *blk) 
{
    crypto_set_cipher_key_t input;
    paktype *comm_pak;
   
    input.command = EPA_SET_CIPHER_KEY;
    input.shared_number_handle = s_handle;
    input.number_of_bytes = number_of_bytes;
    input.key_handle = k_handle;
    input.status = 0x00;
    input.offset = offset;
    input.method = method;
   
    comm_pak = create_comm_pak (EPA_SET_CIPHER_KEY, &input, NULL);
    if (!comm_pak) {
        if (crypto_engine_vip_debug) {
            ttyprintf(CONTTY, "CRYPTO_ENGINE: can not get command pak, abort.\n");
        }
        return EPA_OP_FAIL;
    }
    send_key_req (comm_pak, blk, crypto_engine_vip_key_req_id++);

    return EPA_OP_OK;

}        

epa_status hSHA (uchar *data, ushort size, epa_crypto_blk *blk)
{
    crypto_sha_t input;
    paktype *comm_pak;
   
    input.command = EPA_SHA;
    input.msg_size = size;
    input.status = 0x00;
   
    comm_pak = create_comm_pak (EPA_SHA, &input, data);
    if (!comm_pak) {
        if (crypto_engine_vip_debug) {
            ttyprintf(CONTTY, "CRYPTO_ENGINE: can not get command pak, abort.\n");
        }
        return EPA_OP_FAIL;
    }
    send_key_req (comm_pak, blk, crypto_engine_vip_key_req_id++);

    return EPA_OP_OK;

}

epa_status hSetSession (ushort iv_key_handle, ushort encrypt_key_handle,
                    ushort encrypt_method, epa_crypto_blk *blk)
{
    crypto_set_new_session_t input;
    paktype *comm_pak;
   
    input.command = EPA_SET_SESSION;
    input.iv_key_handle = iv_key_handle;
    input.encrypt_key_handle = encrypt_key_handle;
    input.encrypt_method = encrypt_method;
    input.status = 0x00;
   
    comm_pak = create_comm_pak (EPA_SET_SESSION, &input, NULL);
    if (!comm_pak) {
        if (crypto_engine_vip_debug) {
            ttyprintf(CONTTY, "CRYPTO_ENGINE: can not get command pak, abort.\n");
        }
        return EPA_OP_FAIL;
    }
    send_key_req (comm_pak, blk, crypto_engine_vip_key_req_id++);

    return EPA_OP_OK;
}

epa_status hFreeSession (ushort handle, epa_crypto_blk *blk)
{
    crypto_free_session_t input;
    paktype *comm_pak;
   
    input.command = EPA_FREE_SESSION;
    input.session_handle = handle;
    input.status = 0x00;
   
    comm_pak = create_comm_pak (EPA_FREE_SESSION, &input, NULL);
    if (!comm_pak) {
        if (crypto_engine_vip_debug) {
            ttyprintf(CONTTY, "CRYPTO_ENGINE: can not get command pak, abort.\n");
        }
        return EPA_OP_FAIL;
    }
    send_key_req (comm_pak, blk, crypto_engine_vip_key_req_id++);

    return EPA_OP_OK;
}

epa_status hSetEPAStatus(ushort size, ulong mask, ulong flag, uchar* passwd,
                         epa_crypto_blk *blk)
{
    crypto_set_epa_status_t input;
    paktype *comm_pak;
   
    input.command = EPA_SET_EPA_STATUS; 
    input.passwd_size = size,
    input.mask = mask;
    input.flag = flag;
    input.status = 0x00;
   
    comm_pak = create_comm_pak (EPA_SET_EPA_STATUS, &input, passwd);
    if (!comm_pak) {
        if (crypto_engine_vip_debug) {
            ttyprintf(CONTTY, "CRYPTO_ENGINE: can not get command pak, abort.\n");
        }
        return EPA_OP_FAIL;
    }
    send_key_req (comm_pak, blk, crypto_engine_vip_key_req_id++);

    return EPA_OP_OK;
}
    
epa_status hGetEPAStatus(epa_crypto_blk *blk)
{
    crypto_get_epa_status_t input;
    paktype *comm_pak;
   
    input.command = EPA_GET_EPA_STATUS; 
    input.status = 0x00;
   
    comm_pak = create_comm_pak (EPA_GET_EPA_STATUS, &input, NULL);
    if (!comm_pak) {
        if (crypto_engine_vip_debug) {
            ttyprintf(CONTTY, "CRYPTO_ENGINE: can not get command pak, abort.\n");
        }
        return EPA_OP_FAIL;
    }
    send_key_req (comm_pak, blk, crypto_engine_vip_key_req_id++);

    return EPA_OP_OK;
}

epa_status hSetEPAPasswd(uchar * passwd, ushort size, epa_crypto_blk *blk)
{
    crypto_set_epa_passwd_t input;
    paktype *comm_pak;
   
    input.command = EPA_SET_EPA_PASSWD; 
    input.passwd_size = size;
    input.status = 0x00;
   
    comm_pak = create_comm_pak (EPA_SET_EPA_PASSWD, &input, passwd);
    if (!comm_pak) {
        if (crypto_engine_vip_debug) {
            ttyprintf(CONTTY, "CRYPTO_ENGINE: can not get command pak, abort.\n");
        }
        return EPA_OP_FAIL;
    }
    send_key_req (comm_pak, blk, crypto_engine_vip_key_req_id++);

    return EPA_OP_OK;
}

epa_status hSetRNSeed(uchar * seed, ushort size, epa_crypto_blk *blk)
{
    crypto_set_rn_seed_t input;
    paktype *comm_pak;
   
    input.command = EPA_SET_RN_SEED; 
    input.seed_size = size;
    input.status = 0x00;
   
    comm_pak = create_comm_pak (EPA_SET_RN_SEED, &input, seed);
    if (!comm_pak) {
        if (crypto_engine_vip_debug) {
            ttyprintf(CONTTY, "CRYPTO_ENGINE: can not get command pak, abort.\n");
        }
        return EPA_OP_FAIL;
    }
    send_key_req (comm_pak, blk, crypto_engine_vip_key_req_id++);

    return EPA_OP_OK;
}

epa_status hZeroize(epa_crypto_blk *blk)
{
    crypto_zeroize_t input;
    paktype *comm_pak;
   
    input.command = EPA_ZEROIZE_EPA; 
    input.status = 0x00;
   
    comm_pak = create_comm_pak (EPA_ZEROIZE_EPA, &input, NULL);
    if (!comm_pak) {
        if (crypto_engine_vip_debug) {
            ttyprintf(CONTTY, "CRYPTO_ENGINE: can not get command pak, abort.\n");
        }
        return EPA_OP_FAIL;
    }
    send_key_req (comm_pak, blk, crypto_engine_vip_key_req_id++);

    return EPA_OP_OK;
}

epa_status hGenSyndrome (ushort dh_shared_handle,
                         ushort syndrome_size,
                         ushort offset,
                         epa_crypto_blk *blk)
{
    crypto_gen_syndrome_t input;
    paktype *comm_pak;
   
    input.command = EPA_MEM_SYNDROME; 
    input.status = 0x00;
    input.handle = dh_shared_handle;
    input.offset = offset;
    input.hma_buffer_size = DH_LENGTH_MIN;
    input.syndrome_size = syndrome_size;
   
    comm_pak = create_comm_pak (EPA_MEM_SYNDROME, &input, NULL);
    if (!comm_pak) {
        if (crypto_engine_vip_debug) {
            ttyprintf(CONTTY, "CRYPTO_ENGINE: can not get command pak, ",
                      "abort.\n");
        }
        return EPA_OP_FAIL;
    }
    send_key_req (comm_pak, blk, crypto_engine_vip_key_req_id++);

    return EPA_OP_OK;
}







