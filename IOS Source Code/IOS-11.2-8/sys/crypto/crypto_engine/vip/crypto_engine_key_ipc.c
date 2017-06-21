/* $Id: crypto_engine_key_ipc.c,v 1.1.50.12 1996/09/10 11:13:34 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/vip/crypto_engine_key_ipc.c,v $
 *------------------------------------------------------------------
 * Crypto engine VIP key request command.
 *
 * Jan 1996, xliu 
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_engine_key_ipc.c,v $
 * Revision 1.1.50.12  1996/09/10  11:13:34  xliu
 * CSCdi68600:  ESA should not be given the tralier bytes to encrypt
 * Pass the total length (encap len + ip->tl) to ESA driver.
 * Branch: California_branch
 *
 * Revision 1.1.50.11  1996/08/07  09:22:11  xliu
 * CSCdi63274:  Wrong crypto engine name a show cr eng conf
 * Branch: California_branch
 *
 * Revision 1.1.50.10  1996/07/28  07:44:32  xliu
 * CSCdi62947:  Add parser command to query epa status from RSP
 * This fix also solve the problems reported by CSCdi63488, CSCdi63852.
 * Branch: California_branch
 *
 * Revision 1.1.50.9  1996/07/22  18:57:46  bew
 * CSCdi62404:  password is not enabled when generating DSS key for EPA.
 * Branch: California_branch
 * When generating DSS keys, pass the password onto VIP if there is an
 * EPA on the VIP.
 *
 * Revision 1.1.50.8  1996/07/15  09:59:12  xliu
 * CSCdi61651:  epa randome number seed is not initialized
 * Branch: California_branch
 *
 * Revision 1.1.50.7  1996/07/06  19:45:19  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 1.1.50.6  1996/07/01  12:26:47  xliu
 * CSCdi61743:  decryption does not use cached pak pointer correctly.
 * Branch: California_branch
 *
 * Revision 1.1.50.5  1996/06/21  22:32:33  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 1.1.50.4  1996/06/01  01:15:00  xliu
 * CSCdi59234:  crypto rsp flow vector does not work correct for vip
 * encryption
 * Branch: California_branch
 *
 * Revision 1.1.50.3  1996/05/21  09:44:38  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 1.1.50.2  1996/05/12  23:13:31  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 1.1.50.1  1996/04/19  14:57:12  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.5  1996/04/09  20:03:22  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 1.1.2.4  1996/03/30  05:12:28  xliu
 * Branch: IosSec_branch
 * Commit before CA repository split.
 *
 * Revision 1.1.2.3  1996/03/25  23:19:15  xliu
 * Branch: IosSec_branch
 * Checkin VIP crypto engine code.
 *
 * Revision 1.1.2.2  1996/01/30  22:36:16  xliu
 * Branch: IosSec_branch
 * Checking in encryption RSP/VIP support code.
 *
 * Revision 1.1.2.1  1996/01/09  02:26:47  xliu
 * Branch: IosSec_branch
 * VIP software encryption.
 *
 * Revision 1.1  1996/01/09  02:16:17  xliu
 * Placeholder file.
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
#include "types.h"
#include "registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "sched.h"
#include "stacks.h"
#include "ttysrv.h"
#include "config.h"
#include "../ip/ip.h"
#include "../ipmulticast/igmp.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "../ip/ipaccount.h"
#include "../if/tring.h"
#include "../if/network.h"

#include "../src-vip/dfs_vip_idb.h"
#include "../src-vip/vip_debug.h"

#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../ipc/ipc_private.h"

#include "../ip/ipfast_index.h"
#include "../ip/ipfast_vip.h"
#include "../ip/ipfast_vip_ipc.h"
#include "../ip/ipfast_ipc.h"

#include "../os/chunk.h"

#include "sys_registry.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_tcpip.h"
#include "../ip/ip_registry.h"
#include "../pas/if_pas.h"       /* includes pas_pci.h & segpak.h */
#include "../src-vip/vip_memd.h"

#include "../crypto/crypto_registry.h"
#include "../crypto/crypto_public.h"
#include "../crypto/crypto_map.h"
#include "../crypto/crypto_engine/crypto_lib/lib_toolkit.h"
#include "../crypto/crypto.h"
#include "../crypto/crypto_engine_api.h"
#include "../crypto/crypto_engine/core/crypto_engine.h"
#include "../crypto/crypto_engine/core/crypto_swepa.h"
#include "../crypto/crypto_engine/vip/crypto_engine_vip.h"
#include "../crypto/crypto_sesmgmt.h"
#include "../crypto/crypto_engine/rpsp/crypto_engine_rsp_ipc.h"
#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"
#include "crypto_hwepa.h"
#include "../util/random.h"

/* Function prototype */
static crypto_vip_key_req_context* find_key_context (ushort);
static epa_status del_key_req (crypto_vip_key_req_context *);
static void send_sw_key_request (epa_crypto_blk * );

static uchar crypto_epa_rn_seed[SHA_LENGTH];
epa_passwd_t epa_passwd;

/*
 * crypto_engine_send_key_request
 *
 * Called by ipc callback to send the key request to EPA if the epa card 
 * is present, to the software key process if EPA is not present. This is
 * called at process level.
 */
void crypto_engine_recv_key_request (uchar *msg) 
{
    epa_crypto_blk *blk;
    int slot;

    /* 
     * Malloc memory buffer for the key reuqest blk.
     */
    blk = (epa_crypto_blk *)chunk_malloc(crypto_key_blk_chunk);
    blk->next = NULL;
    if (!blk) {
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: can not allocate blk buffer.");
        }
        return;
    }

    msg += sizeof(ipc_ipfast_msg_hdr);
    slot = *(int *)msg;
    if ((slot != vip_logical_slot()) && (slot != -1)) {
	/*
	 * We received message that is not intended for us
	 * and is not broadcast- ignore it.
	 */
	chunk_free(crypto_key_blk_chunk, blk);
	return;
    }

    msg += sizeof(int);
    bcopy(msg, &blk->header, sizeof(epa_crypto_blk_header));
    bcopy(msg + sizeof(epa_crypto_blk_header), blk->data, 
          blk->header.data_size);
    if (crypto_engine_vip_debug) {
        buginf("\nCRYPTO_ENGINE: req_id: %d, comm_id: %d, size: %d", 
             blk->header.req_id, blk->header.command_id, blk->header.data_size);
    }

    if (crypto_engine_vip_config.epa_type == CRYPTO_SWEPA) {
        send_sw_key_request ((epa_crypto_blk *)blk);
    } else {
        crypto_send_epa_key_request ((epa_crypto_blk *)blk);
    }
}

/*
 * crypto_engine_rsp_key_rsp_callback
 *
 * Called when the key process completes the execution. Forward the response
 * to the RSP Session Manager.
 */
void crypto_engine_send_key_response (epa_crypto_blk *blk)
{
    ipc_message *ipc_message;
    uchar *data;

    if (crypto_engine_vip_debug) {
        buginf("\nCRYPTO_ENGINE: received key response for req %d",
               blk->header.req_id);
    }

    /*
     * Take command-specific actions here.
     */
    switch (blk->header.command_id) {
    case CRYPTO_CLEAR_CONN_ENTRY:
    case CRYPTO_CLEAR_DH_NUMBER:
        /* There is no waiting process for the response */
        chunk_free(crypto_key_blk_chunk, blk);
        return;
    default:
        break;
    }

    ipc_message = ipc_get_message (sizeof(epa_crypto_blk_header) +
                                   blk->header.data_size,0,0);

    if (!ipc_message) {
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: could not get message of size: %d",
                     sizeof(epa_crypto_blk_header) + blk->header.data_size);
        }
        return;
    }

    data = ipc_message->header->data;
    bcopy(&blk->header, data, sizeof(epa_crypto_blk_header));
    bcopy (blk->data, data + sizeof(epa_crypto_blk_header),
           blk->header.data_size);

    ipc_send_message_permanent_port(ipc_message, IPC_TYPE_CRYPTO);
    chunk_free(crypto_key_blk_chunk, blk);
}

/*
 * send_sw_key_request
 * Regist the key request into the key request pool and enqueue the request
 * to the key process.
 */

void send_sw_key_request (epa_crypto_blk *blk) 
{
    /*
     * Should bring in the code which regists the requests in the pool,
     * etc.
     */

    if (blk->header.command_id == CRYPTO_INIT) {
        /* copy the dss key pair */
        bcopy(blk->data, crypto_pri_key, SHA_LENGTH);
        bcopy(blk->data+SHA_LENGTH, crypto_pub_key, DSS_LENGTH_MIN);
        return;
    }
  
    if (crypto_engine_vip_debug) {
        buginf("\nCRYPTO_ENGINE: enqueue the key request to sw key proc.");
    }
 
    crypto_key_enqueue (&crypto_engine_idb, blk);

}
  
/*
 * crypto_send_epa_key_request
 * 
 * Send key request to EPA. The response is recevied by callback which will
 * pass the result back to Session Mgr. Here the command id is crypto api
 * command id, which is different as cylink crypto api command id. For each
 * crypto api command, one or more than one cylink commands are send to
 * epa to complete the task.
 * This is called at process level.
 */ 
void  crypto_send_epa_key_request (epa_crypto_blk *blk)
{

    /* 
     * EPA key ring requires the paktype. The pak is deallocated 
     * when the crypto engine received the callback from the driver
     * after the request is servied by the EPA firmware.
     * In the following, the blk->header.seq_number is to link the
     * next command into the transaction. 
     */ 

    switch (blk->header.command_id) {

    case  CRYPTO_CREATE_SESSION_KEY:
        {
        epa_crypto_session_key_data_blk *data_blk = 
                       (epa_crypto_session_key_data_blk *)blk->data;
        ushort conn_id = data_blk->connection_id;

        if (crypto_engine_vip_debug) {
            buginf("\nEPA_REQ: start create session key transaction.");
        }

        crypto_vip_conntbl[conn_id].encrypt_method = 
            epa_encrypt_method(data_blk->encrypt_alg_type);

        send_hma_alloc_comm (EPA_SET_ENCRYPT_KEY,
                             EPA_SESSION_KEY_SIZE,
                             EPA_SESSION_KEY_HMA_ATTRI, blk);
        }
        break;

    case CRYPTO_VIP_SIGN:
        {
        crypto_vip_sign_data_blk *data_blk = 
                   (crypto_vip_sign_data_blk*)blk->data;
        uchar *data = blk->data + sizeof(crypto_vip_sign_data_blk);

        if (crypto_engine_vip_debug) {
            buginf( "\nEPA_REQ: create sha to sign.");
            crypto_dump_data(data, data_blk->data_length);
        }
        send_sha_comm(EPA_GEN_DSS_SIGNATURE, data, 
                      data_blk->data_length, blk);
        }
        break; 

    case CRYPTO_VIP_VERIFY:
        { 
        crypto_vip_verify_data_blk *data_blk = 
                  (crypto_vip_verify_data_blk *)blk->data;

        if (crypto_engine_vip_debug) {
            buginf("\nVerify command buffer.\n");
            crypto_dump_data(blk->data + 
                             sizeof(crypto_vip_verify_data_blk),
                             data_blk->clear_data_length + SHA_LENGTH*2);
        }

        send_sha_comm(EPA_VER_DSS_SIGNATURE, 
                   blk->data + sizeof(crypto_vip_verify_data_blk), 
                   data_blk->clear_data_length, blk);
        }
        break;

    case CRYPTO_GEN_DSS_KEY_PAIR:
        {
        crypto_gen_dss_key_data_blk *passwd = 
                                (crypto_gen_dss_key_data_blk *)blk->data;
        epa_passwd.size = passwd->passwd_size;
        bcopy (passwd->passwd, epa_passwd.passwd, epa_passwd.size); 
        
        send_set_epa_status_comm(EPA_GEN_DSS_KEY,
                                 EPA_PRIVMOD, EPA_PRIVMOD, 
                                 blk);
        }
        break; 

    case CRYPTO_GET_EPA_STATUS:
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: send command to get epa status.");
        }
        send_get_epa_status_comm (EPA_CHECK_STATUS, blk);
        break;

    case CRYPTO_GEN_DH_ALG_PHASE1_PARAM:
        /* Allocate hma handle for the private DH number; 
         * gen DH number pair. 
         */
        if (crypto_engine_vip_debug) {
            buginf( "\nEPA_REQ: alloc hma to generate DH pair.");
        }

        send_hma_alloc_comm (EPA_GEN_DH_PAIR,
                             EPA_DH_PRIVATE_SIZE,
                             EPA_DH_PRIVATE_HMA_ATTRI,
                             blk);
        break;

    case CRYPTO_GEN_DH_ALG_PHASE2_PARAM:
        /* Allocate hma handle for the shared number. */ 
        if (crypto_engine_vip_debug) {
            buginf( "\nEPA_REQ: alloc hma to generate DH shared.");
        }
        send_hma_alloc_comm (EPA_GET_DH_SHARED,
                             EPA_DH_SHARED_SIZE,
                             EPA_DH_SHARED_HMA_ATTRI,
                             blk);
        break;

    case CRYPTO_SET_EPA_PRIV_MODE:
        send_set_epa_status_comm (EPA_TRANS_END,
                             EPA_PRIVMOD,
                             EPA_PRIVMOD,
                             blk);
        break;

    case CRYPTO_INIT_RANDOM:
        send_hma_alloc_comm (EPA_GEN_RANDOM,
                             EPA_RANDOM_SIZE,
                             EPA_RANDOM_HMA_ATTRI,
                             blk);
        break;

    case CRYPTO_CLEAR_CONN_ENTRY:
        {
        crypto_clear_conn_entry_data_blk *data_blk = 
                             (crypto_clear_conn_entry_data_blk *)blk->data;
        ushort conn_id = data_blk->conn_id;
        ushort session_id = crypto_vip_conntbl[conn_id].hma_session_id;
        leveltype level;

        if (crypto_engine_vip_debug) {
            buginf( "\nEPA_REQ: clear conn %d with handle: %d", 
                       conn_id, crypto_vip_conntbl[conn_id].hma_session_id);
        }

        level = raise_interrupt_level(NETS_DISABLE);
        crypto_vip_conntbl[conn_id].hma_dh_shared = 0;
        crypto_vip_conntbl[conn_id].hma_session_id = 0;
        crypto_vip_conntbl[conn_id].state = CRYPTO_EPA_SESSION_UNSET;
        reset_interrupt_level(level);

        send_free_session_comm (EPA_HMA_FREE,session_id, blk); 

        }
        break;

    case CRYPTO_CONFIG_EPA:
        {
        crypto_config_epa_data_blk *data_blk =
                    (crypto_config_epa_data_blk *)blk->data;
        epa_passwd.size = data_blk->passwd_size;
        bcopy(data_blk->passwd, epa_passwd.passwd, data_blk->passwd_size);
        send_set_epa_passwd_comm(EPA_SET_EPA_STATUS,
                                 data_blk->passwd,
                                 data_blk->passwd_size,
                                 blk);
                                 
        }
        break;

    case CRYPTO_ZEROIZE:
        send_zeroize(EPA_GET_EPA_STATUS,
                     blk);
        break;

    case CRYPTO_CLEAR_LATCH:
        {
        crypto_config_epa_data_blk *data_blk =
                    (crypto_config_epa_data_blk *)blk->data;
        epa_passwd.size = data_blk->passwd_size;
        bcopy(data_blk->passwd, epa_passwd.passwd, data_blk->passwd_size);
        send_set_epa_status_comm(EPA_GET_EPA_STATUS,
                                 EPA_XTRACT,
                                 0x00000000,
                                 blk);
        }
        break;

    case CRYPTO_GEN_SYNDROME:
        {
        crypto_gen_syndrome_data_blk *data_blk = 
                   (crypto_gen_syndrome_data_blk *)blk->data;

        send_gen_syndrome(EPA_TRANS_END,
             crypto_engine_vip_dh_tbl[data_blk->connection_id].dh_shr_handle,
             data_blk->challenge_length,   
             data_blk->offset,
             blk);
        }
        break;
        
    default:
        break;
    }
}

/*
 * crypto_engine_key_callback
 *
 * Callback from epa driver after key mgt operations. The response is
 * in the scattered pak which is allocated when the request is created.
 * Here we copy the response back to epa_crypto_blk to be send back to RSP.
 * When sending out using IPC, the response has to be copied into ipc message
 * buffer. We should save the extra copy, later. 
 * 
 * The command id here is cylink command id, which is directly read from
 * the particle which carries the response.
 */
void crypto_engine_key_callback (ushort status, paktype *pak)
{
    crypto_engine_key_comm_buffer_t  *recv_comm;
    crypto_vip_key_req_context* context;
    particletype *mp;
    paktype *newpak;

    mp = pak->particlequeue.qhead;
    if (!mp) {
        return;
    }

    newpak = getbuffer(mp->data_bytes);
    if (!newpak) {
        return;
    }

    bcopy(mp->data_start, newpak->datagramstart, mp->data_bytes);
    newpak->datagramsize = pak->datagramsize; 
    particle_retbuffer(pak);

    recv_comm = (crypto_engine_key_comm_buffer_t *)newpak->datagramstart;
    newpak->cryptoflags = pak->cryptoflags;
    newpak->crypto_enc_conn_id = pak->crypto_enc_conn_id;
    newpak->crypto_dec_conn_id = pak->crypto_dec_conn_id;

    /*
     * The pak connection_id fields are used to save the req_id for the given 
     * key request. There could be an encryption and a decryption connection
     * id, so choose the decryption one first. If it can't be found try the
     * encryption one.
     *
     * Find the context of the given request and either send
     * back the output through IPC or send another key command, based on
     * the Session Mgr command, saved in the context.
     */
    context = NULL;
    if (newpak->crypto_dec_conn_id > 0) {
        context = find_key_context(newpak->crypto_dec_conn_id);
    }
    if (!context) {
	context = find_key_context(newpak->crypto_enc_conn_id);
    }
    if (!context) {
    	if (crypto_engine_vip_debug) {
            buginf ("CRYPTO_ENGINE_VIP: can not find context: encryption %d "
		    "decryption %d\n", 
		    newpak->crypto_enc_conn_id, newpak->crypto_dec_conn_id);
        }
        return;
    }

    del_key_req (context);
    if (crypto_engine_vip_debug) {
        buginf( "\nCRYPTO_ENGINE: key rsp status: %x", 
                recv_comm->status);
    }

    /* Check the status of the returned command. */
    if (recv_comm->status != 0) {
        if (crypto_engine_vip_debug) {
            buginf("\nEPA_RSP: request %d failed. Status: %d\n", 
                          recv_comm->command, recv_comm->status);
        }
        context->blk->header.status = recv_comm->status;
        crypto_engine_send_key_response(context->blk);
        retbuffer(newpak);
        return;
    } else {
        context->blk->header.status = EPA_OP_OK;
    }

    switch (recv_comm->command) {

    case EPA_SET_CIPHER_KEY:
        {
        crypto_set_cipher_key_t *output = 
                            (crypto_set_cipher_key_t *)recv_comm;
        epa_crypto_session_key_data_blk *data_blk = 
                       (epa_crypto_session_key_data_blk *)context->blk->data;
        ushort conn_id = data_blk->connection_id;

        if (context->blk->header.seq_number == EPA_SET_ENCRYPT_KEY) {
            if (crypto_engine_vip_debug) {
               buginf( "\nEPA_RSP: set up session key with handle %d\n",
                    output->key_handle);
            }
            crypto_vip_conntbl[conn_id].hma_encrypt_key_state = 
                                           CRYPTO_EPA_ENCRYPT_KEY_SET;
            /*
             * We have the session key created. Now send out command to
             * create IV key.
             */
            send_hma_alloc_comm (EPA_SET_IV_KEY,
                                 EPA_IV_KEY_SIZE,
                                 EPA_IV_KEY_HMA_ATTRI, context->blk);
        } else {
            if (crypto_engine_vip_debug) {
               buginf( "\nEPA_RSP: set up iv key with handle %d\n",
                    output->key_handle);
            }
            crypto_vip_conntbl[conn_id].hma_iv_key_state = 
                                          CRYPTO_EPA_IV_KEY_SET;

            /* 
             * Now we have IV key created. Check the state of both key
             * and send out the command to create session.
             */
            if (crypto_vip_conntbl[conn_id].hma_encrypt_key_state == 
                CRYPTO_EPA_ENCRYPT_KEY_SET &&
                crypto_vip_conntbl[conn_id].hma_iv_key_state == 
                CRYPTO_EPA_IV_KEY_SET) {

                send_set_new_session_comm(crypto_vip_conntbl[conn_id].
                                         hma_encrypt_key,
                                         crypto_vip_conntbl[conn_id].
                                         hma_iv_key,
                                         crypto_vip_conntbl[conn_id].
                                                        encrypt_method,
                                         context->blk);
            }
        }
        }
        break;

    case EPA_SET_SESSION:
        {
        crypto_set_new_session_t *output = 
                (crypto_set_new_session_t *)recv_comm;        
        epa_crypto_session_key_data_blk* data_blk = 
                      (epa_crypto_session_key_data_blk *)context->blk->data;
        ushort conn_id = data_blk->connection_id;
        leveltype level;

        if (crypto_engine_vip_debug) {
            buginf( "\nEPA_RSP: set up a new session. %d\n",
                    output->new_session_handle);
        }

        level = raise_interrupt_level(NETS_DISABLE);
        crypto_vip_conntbl[conn_id].hma_session_id = 
                                        output->new_session_handle;
        crypto_vip_conntbl[conn_id].state = CRYPTO_EPA_SESSION_SET;
        reset_interrupt_level(level);

        context->blk->header.seq_number = CRYPTO_REQ_SEQ_END;
        }
        break;

    case EPA_FREE_SESSION:
        {
        crypto_clear_conn_entry_data_blk *data_blk =
                       (crypto_clear_conn_entry_data_blk *)context->blk->data;
        crypto_vip_conn_entry *conn = &crypto_vip_conntbl[data_blk->conn_id];
        
        if (crypto_engine_vip_debug) {
            buginf("\nEPA_RSP: free a session.\n");
        }

        if (conn->hma_encrypt_key_state == CRYPTO_EPA_ENCRYPT_KEY_SET) {
            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE:free encrypt key handle %d for conn %d",
                      conn->hma_encrypt_key, data_blk->conn_id);
            }
            send_hma_free_comm(EPA_FREE_IV_KEY,
                               conn->hma_encrypt_key,
                               context->blk);
            conn->hma_encrypt_key_state = CRYPTO_EPA_ENCRYPT_KEY_UNSET;
            conn->hma_encrypt_key = 0;
        }
        }
        break;

    case EPA_HMA_ALLOC:
        {
        crypto_hma_alloc_t *output = (crypto_hma_alloc_t *)recv_comm;

        if (crypto_engine_vip_debug) {
            buginf("\nEPA_RSP: alloc hma block %d of %d bytes\n", 
                    output->handle, output->byte_to_alloc);
        }
      
        switch (context->blk->header.command_id) {
        case CRYPTO_GEN_DSS_KEY_PAIR:
        case CRYPTO_INIT:
            send_set_epa_status_comm(EPA_GEN_DSS_KEY,
                                     EPA_PRIVMOD, EPA_PRIVMOD, 
                                     context->blk);
            break;
        case CRYPTO_GEN_DH_ALG_PHASE1_PARAM:
            {
            crypto_gen_dh_param_data_blk *data_blk = 
                           (crypto_gen_dh_param_data_blk *)context->blk->data;
            ushort  conn_id = data_blk->connection_id;
            crypto_engine_vip_dh_tbl[conn_id].dh_pri_handle = output->handle;
            if (crypto_engine_vip_debug) {
                buginf( "\nEPA_RSP: geneate DH pair use handle:%d for conn %d.",
                        output->handle, conn_id);
            }
            send_gen_dh_pair_comm (output->handle, context->blk);
            }
            break;

        case CRYPTO_GEN_DH_ALG_PHASE2_PARAM:
            {
            crypto_gen_dh_param_data_blk *data_blk = 
                           (crypto_gen_dh_param_data_blk *)context->blk->data;
            ushort  conn_id = data_blk->connection_id;
            crypto_engine_vip_dh_tbl[conn_id].dh_shr_handle = output->handle;
            if (crypto_engine_vip_debug) {
                buginf( "\nEPA_RSP: geneate DH shared %d for conn %d, ",
                        output->handle, conn_id);
            }
            crypto_vip_conntbl[conn_id].hma_dh_shared = 
                                                         output->handle;
            send_gen_dh_shr_comm(
                crypto_engine_vip_dh_tbl[conn_id].dh_pri_handle,
                output->handle,
                DH_LENGTH_MIN,
                data_blk->peer_public_number,
                context->blk);
            }
            break;

        case CRYPTO_INIT_RANDOM:
            send_init_random_comm(output->handle, SHA_LENGTH, context->blk);
            context->blk->header.seq_number = CRYPTO_REQ_SEQ_END;
            break;

        case CRYPTO_CREATE_SESSION_KEY:
            {
            epa_crypto_session_key_data_blk* data_blk = 
                        (epa_crypto_session_key_data_blk *)context->blk->data;
            ushort conn_id = data_blk->connection_id;
           
            if (context->blk->header.seq_number == 
                      EPA_SET_ENCRYPT_KEY) {
                crypto_vip_conntbl[conn_id].hma_encrypt_key =
                                                              output->handle;
                send_set_session_key_comm(
                        crypto_vip_conntbl[conn_id].encrypt_method, 
                        context->blk);
            } else {
                crypto_vip_conntbl[conn_id].hma_iv_key = output->handle;
                send_set_iv_key_comm(
                    crypto_vip_conntbl[conn_id].encrypt_method,
                    context->blk);
            }
            }
            break;

         default:
            break;
         }
         }
         break;

    case EPA_HMA_FREE:
        {
        crypto_hma_free_t *output = (crypto_hma_free_t *)recv_comm;
        if (crypto_engine_vip_debug) {
            buginf("\nEPA_RSP: freed hma block %d\n", output->handle);
        }
        switch (context->blk->header.seq_number) {
        case EPA_FREE_IV_KEY: 
            {
            crypto_clear_conn_entry_data_blk *data_blk =
                   (crypto_clear_conn_entry_data_blk *)context->blk->data;
            crypto_vip_conn_entry *conn = 
                             &crypto_vip_conntbl[data_blk->conn_id];
       
            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE:free iv key handle %d for conn %d",
                         conn->hma_iv_key, data_blk->conn_id);
            }
            if (conn->hma_iv_key_state == CRYPTO_EPA_IV_KEY_SET) {
                send_hma_free_comm(EPA_FREE_DH_SHARED,
                                   conn->hma_iv_key,
                                   context->blk);
                conn->hma_iv_key_state = CRYPTO_EPA_IV_KEY_UNSET;
                conn->hma_iv_key = 0;
            } else {
                context->blk->header.seq_number = CRYPTO_REQ_SEQ_END;
            }
            }
            break;
        case EPA_FREE_DH_SHARED:
            {
            crypto_clear_conn_entry_data_blk *data_blk =
                   (crypto_clear_conn_entry_data_blk *)context->blk->data;
            ushort conn_id = data_blk->conn_id;
            if (crypto_engine_vip_dh_tbl[conn_id].dh_shr_handle) {
                if (crypto_engine_vip_debug) {
                    buginf("\nCRYPTO_ENGINE: free dh shr handle %d for conn %d",
                      crypto_engine_vip_dh_tbl[conn_id].dh_shr_handle, conn_id);
                }
                send_hma_free_comm(EPA_FREE_DH_PRIVATE,
                           crypto_engine_vip_dh_tbl[conn_id].dh_pri_handle,
                           context->blk);
                crypto_engine_vip_dh_tbl[conn_id].dh_pri_handle = 0;
            } else {
                context->blk->header.seq_number = CRYPTO_REQ_SEQ_END;
            }
            }
            break;

        case EPA_FREE_DH_PRIVATE:
            {
            crypto_clear_conn_entry_data_blk *data_blk =
                   (crypto_clear_conn_entry_data_blk *)context->blk->data;
            ushort conn_id = data_blk->conn_id;
            if (crypto_engine_vip_dh_tbl[conn_id].dh_pri_handle) {
                if (crypto_engine_vip_debug) {
                    buginf("\nCRYPTO_ENGINE: free dh pri handle %d for conn %d",
                      crypto_engine_vip_dh_tbl[conn_id].dh_pri_handle, conn_id);
                }
                send_hma_free_comm(EPA_TRANS_END,
                           crypto_engine_vip_dh_tbl[conn_id].dh_pri_handle,
                           context->blk);
                crypto_engine_vip_dh_tbl[conn_id].dh_pri_handle = 0;
            } else {
                context->blk->header.seq_number = CRYPTO_REQ_SEQ_END;
            }
            }
            break;


        case EPA_TRANS_END:
            context->blk->header.seq_number = CRYPTO_REQ_SEQ_END;
            break;

        default:
            break;
        }
        }
        break;

    case EPA_HMA_WRITE:
        {
        crypto_hma_write_t *output = (crypto_hma_write_t *)recv_comm;
        if (crypto_engine_vip_debug) {
            buginf("\nEPA_RSP: write hma block %d %d bytes\n", 
                    output->handle, output->byte_to_write);
        }
        output += sizeof(crypto_hma_write_t);
        }
        break;

    case EPA_HMA_READ:
        /*
         * Currently we don't use this command. The case is still
         * preserved for the case of extension later.
         */
        break;

    case EPA_SHA:
        {
        crypto_sha_t *output = (crypto_sha_t *)recv_comm;
        switch (context->blk->header.command_id) {
        case CRYPTO_VIP_SIGN:
            if (crypto_engine_vip_debug) {
                buginf( "\nEPA_RSP: sign using %d",
                        crypto_engine_vip_dss_p_handle);
                crypto_dump_data((uchar *)output + sizeof(crypto_sha_t) +
                                  output->msg_size, 20);
            }
            send_gen_dss_sig_comm(crypto_engine_vip_dss_p_handle,
                              (uchar *)(output)+sizeof(crypto_sha_t) + 
                                        output->msg_size,
                              DSS_LENGTH_MIN,
                              context->blk);
            break;
        case CRYPTO_VIP_VERIFY: 
            {
            crypto_vip_verify_data_blk* data_blk = 
                            (crypto_vip_verify_data_blk*)context->blk->data;
            uchar *clear_data, *signature;

            if (crypto_engine_vip_debug) {
                buginf( "\nEPA_RSP: verify");
                crypto_dump_data((uchar *)output + sizeof(crypto_sha_t) +
                                  output->msg_size, 20);
            }
            clear_data = context->blk->data + 
                         sizeof(crypto_vip_verify_data_blk);
            signature = clear_data + data_blk->clear_data_length;

            send_ver_dss_sig_comm((uchar *)output+sizeof(crypto_sha_t)+
                                  output->msg_size,
                                  DSS_LENGTH_MIN,
                                  signature,
                                  data_blk->public_key,
                                  context->blk);
            }
            break;
        default:
            break;
        }
        }
        break;

    case EPA_GEN_DSS_KEY:
        {
            crypto_gen_dss_key_t *output = (crypto_gen_dss_key_t*)recv_comm;
            epa_rtn_sig_key *rtn_pub_key_data_blk = NULL;

            if (crypto_engine_vip_debug) {
                buginf("\nEPA_RSP: dss key pair gennerated.\n");
            }

            rtn_pub_key_data_blk = (epa_rtn_sig_key *)context->blk->data;
            rtn_pub_key_data_blk->alg_type = CRYPTO_GEN_KEY_PAIR;
            rtn_pub_key_data_blk->key_tag.dss_keys.pub_key_size = 
                                                               DSS_LENGTH_MIN;
            rtn_pub_key_data_blk->key_tag.dss_keys.pri_key_size = 
                                        sizeof(crypto_engine_vip_dss_p_handle);
            bcopy(output->public_number, 
                  rtn_pub_key_data_blk->key_tag.dss_keys.pub_key,
                  DSS_LENGTH_MIN);
            bcopy(&crypto_engine_vip_dss_p_handle, 
                  rtn_pub_key_data_blk->key_tag.dss_keys.pri_key,
                  sizeof(crypto_engine_vip_dss_p_handle));
            bcopy(&crypto_engine_vip_config.epa_serial_number,
                  &rtn_pub_key_data_blk->serial_number,
                  sizeof(crypto_engine_vip_config.epa_serial_number));

            send_get_epa_status_comm(EPA_TRANS_END, context->blk);
        }

        break; 

    case EPA_GEN_DSS_SIGNATURE:
        {
        crypto_gen_dss_signature_t *output =
                                (crypto_gen_dss_signature_t*)recv_comm;
        uchar *sig = (uchar *)output + sizeof(crypto_gen_dss_signature_t);

        if (crypto_engine_vip_debug) {
            ushort i;
            buginf("\nEPS_RSP: complet signing.\n");
            for (i=0; i<40; i++) 
                buginf("%02x ", sig[i]);
        }

        bcopy(sig, context->blk->data, SHA_LENGTH * 2);
        context->blk->header.data_size = SHA_LENGTH * 2; 
        context->blk->header.seq_number = CRYPTO_REQ_SEQ_END;
        }
        break;

    case EPA_VER_DSS_SIGNATURE:
        {
        ushort result;

        if (crypto_engine_vip_debug) {
            buginf("\nEPA_RSP: complet verifying.Result: %d\n",
                                            recv_comm->status);
        }
        if (recv_comm->status != 0) {
            result = CRYPTO_INVALID_SIGNATURE;
        } else {
            result = CRYPTO_VALID_SIGNATURE;
        }

        bcopy(&result, context->blk->data, sizeof(result));
        context->blk->header.seq_number = CRYPTO_REQ_SEQ_END;
        context->blk->header.data_size = sizeof(result);
        }
        break;

    case EPA_GEN_RANDOM:
        buginf("\nEPA_RSP: complet random number generation\n");
        break;

    case EPA_SET_RN_SEED:
        if (crypto_engine_vip_debug) {
                buginf("\nEPA_RSP: set random number seed\n");
        }
        send_get_epa_status_comm(EPA_TRANS_END, context->blk);
        break;

    case EPA_ZEROIZE_EPA:
        if (crypto_engine_vip_debug) {
            buginf("\nEPA_RSP: zeroize completed.\n");
        }
        send_get_epa_status_comm(EPA_TRANS_END, context->blk);
        break;

    case EPA_GEN_DH_PAIR:
        {
        crypto_gen_dh_pair_t *output = (crypto_gen_dh_pair_t*)recv_comm;
        crypto_gen_dh_param_data_blk *data_blk = 
                          (crypto_gen_dh_param_data_blk *)context->blk->data;
        ushort  conn_id = data_blk->connection_id;
        epa_rtn_alg_param *rtn_data_blk = NULL;

        if (crypto_engine_vip_debug) {
            buginf("\nEPA_RSP: dh pair gennerated.\n");
        }

        crypto_engine_vip_dh_tbl[conn_id].state = CRYPTO_DH_PAIR_SET;
        rtn_data_blk = (epa_rtn_alg_param *)context->blk->data;
        rtn_data_blk->alg_type = DH_ALG_PHASE1;
        bcopy(output->public_number, rtn_data_blk->alg_tag.dh_pub.number,
              DH_LENGTH_MIN);
        context->blk->header.data_size = sizeof(epa_rtn_alg_param);
        context->blk->header.seq_number = CRYPTO_REQ_SEQ_END;
        }
        break;

    case EPA_GET_DH_SHARED:
        
        if (crypto_engine_vip_debug) {
            buginf("\nEPA_RSP: complete DH shared generation.\n");
        }
        context->blk->header.seq_number = CRYPTO_REQ_SEQ_END;
        break;

    case EPA_SET_EPA_PASSWD:
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: epa password set.");
        }
        switch(context->blk->header.seq_number) {
        case EPA_SET_EPA_STATUS:
            send_set_epa_status_comm(EPA_SET_EPA_STATUS,
                                     EPA_XTRACT,
                                     0x00000000,
                                     context->blk);
            break;
        }

        break;

    case EPA_SET_EPA_STATUS:
        if (crypto_engine_vip_debug) {
            buginf("\nEPA_RSP: complete setting epa status.\n");
        }
        switch (context->blk->header.command_id) {
        case CRYPTO_GEN_DSS_KEY_PAIR:
        case CRYPTO_INIT:
            if (context->blk->header.seq_number == EPA_GEN_DSS_KEY) {
                if (crypto_engine_vip_debug) {
                    buginf("\nCRYPTO_ENGINE: send command to gen dss.");
                }
                send_gen_dss_pair_comm (crypto_engine_vip_dss_p_handle,
                                        context->blk);
            } else {
                /* the initializaton is done. */
               context->blk->header.seq_number = CRYPTO_REQ_SEQ_END;
            }
            break;

        case CRYPTO_CONFIG_EPA:
            switch (context->blk->header.seq_number) {
            case EPA_SET_EPA_STATUS:
                 /* to set privilege mode */
                send_set_epa_status_comm (EPA_SET_RN_SEED,
                                          EPA_PRIVMOD,
                                          EPA_PRIVMOD,
                                          context->blk);
                break;
            case EPA_SET_RN_SEED:
                random_fill(crypto_epa_rn_seed, SHA_LENGTH, FALSE);
                send_set_rn_seed(EPA_TRANS_END,
                                 crypto_epa_rn_seed,
                                 SHA_LENGTH,
                                 context->blk);
            }
            break;

        case CRYPTO_CLEAR_LATCH: 
            send_get_epa_status_comm (EPA_CHECK_STATUS, context->blk);
            break;
            
        default:
            break;
        }
        break;
 
    case EPA_GET_EPA_STATUS:
        {
        crypto_get_epa_status_t *output =(crypto_get_epa_status_t *)recv_comm;
        epa_rtn_get_status *rtn_status = 
                                     (epa_rtn_get_status *)context->blk->data;
        if (crypto_engine_vip_debug) {
            buginf("\nEPA_RSP: status: %04x, version: %04x\n",
                   output->status, output->version);
            buginf("\nflags: %x, reset_cr: %x, sys_error: %x",
                   output->flags, output->reset_cr, output->sys_error);
        }

        switch (context->blk->header.command_id) {
        case CRYPTO_GEN_DSS_KEY_PAIR:
            rtn_status = (epa_rtn_get_status *)(context->blk->data + 
                         sizeof(epa_rtn_sig_key) - sizeof(epa_rtn_get_status));
            context->blk->header.data_size =  sizeof(epa_rtn_sig_key);
            break;
        default:
            rtn_status = (epa_rtn_get_status *)context->blk->data;
            context->blk->header.data_size = sizeof(epa_rtn_get_status);
            break;
        }
     
        rtn_status->slot = vip_logical_slot();
        bcopy((uchar *)&output->flags, &rtn_status->status, 
               sizeof(output->flags));
        context->blk->header.seq_number = CRYPTO_REQ_SEQ_END;
        }
        break;

    case EPA_MEM_SYNDROME:
        {
        crypto_gen_syndrome_t *output =
                                (crypto_gen_syndrome_t*)recv_comm;
        crypto_gen_syndrome_data_blk *data_blk = 
                 (crypto_gen_syndrome_data_blk *)context->blk->data;

        uchar *syndrome = (uchar *)output + sizeof(crypto_gen_syndrome_t);

        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE:complete generating syndrome of %d bytes",
                    data_blk->challenge_length);
            crypto_dump_data(syndrome, 20);
        }

        context->blk->header.data_size = data_blk->challenge_length; 
        context->blk->header.seq_number = CRYPTO_REQ_SEQ_END;
        bcopy(syndrome, context->blk->data, data_blk->challenge_length);
        }
        break;


    default:
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: invalid epa response. %d",
                    recv_comm->command);
        }
        break;
    }

    retbuffer(newpak);

    /* Pass the blk to key process to send the response back to rsp */
    if (context->blk->header.command_id != EPA_TEST_ONLY &&
        context->blk->header.seq_number == CRYPTO_REQ_SEQ_END) {
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: enqueue to sw key proc.");
        }
        crypto_engine_send_key_response(context->blk);
    }
}

/*
 * send_key_rsp
 *
 * Send the key request output to Session Mgr on RSP through IPC.
 * Delete the context from the context pool.
 */

epa_status send_key_rsp (crypto_vip_key_req_context *context)
{
    return EPA_OP_OK;
}

/*
 * find_key_context
 *
 * Find the context of a key request with the given req_id. Return the
 * context to the caller.
 */

crypto_vip_key_req_context * find_key_context (ushort req_id)

{
    ushort index;
    crypto_vip_key_req_context* context = NULL;

    context = &crypto_vip_key_req_context_pool[0];

    /* Should improve with hash indexing */
    for (index=0; index<CRYPTO_VIP_KEY_CONTEXT_POOL_SIZE; index++)
        if (context->req_id == req_id)
            return context;
        else
            context++;
    return NULL;
}

/*
 * del_key_req 
 *
 * Delete the key requests from the request pool.
 */
static epa_status del_key_req (crypto_vip_key_req_context *context)
{
    ushort index;
    crypto_vip_key_req_context *contexts;

    contexts = &crypto_vip_key_req_context_pool[0];

    for (index = 0; index < CRYPTO_VIP_KEY_CONTEXT_POOL_SIZE; index++) 
        if (context->req_id == contexts->req_id) {
            contexts->state = CRYPTO_VIP_KEY_REQ_ENTRY_UNSET;
            return EPA_OP_OK; 
        } else {
            contexts++;
        }
           
    return EPA_OP_FAIL;
} 
