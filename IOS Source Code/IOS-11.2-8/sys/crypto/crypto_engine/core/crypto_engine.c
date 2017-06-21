/* $Id: crypto_engine.c,v 1.1.68.5 1996/08/07 09:21:32 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/core/crypto_engine.c,v $
 *------------------------------------------------------------------
 * crypto_engine.c
 * 
 * This file defines the crypto engine dispatchers and other utiltiy 
 * routines. 
 * 11, 1995, xliu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_engine.c,v $
 * Revision 1.1.68.5  1996/08/07  09:21:32  xliu
 * CSCdi63274:  Wrong crypto engine name a show cr eng conf
 * Branch: California_branch
 *
 * Revision 1.1.68.4  1996/07/06  19:44:56  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 1.1.68.3  1996/06/28  23:40:45  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 1.1.68.2  1996/05/12  23:12:42  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 1.1.68.1  1996/04/19  14:56:30  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.16  1996/04/17  23:47:54  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 1.1.2.15  1996/04/16  21:53:32  bew
 * Branch: IosSec_branch
 * General code cleanup.
 *
 * Revision 1.1.2.14  1996/03/30  05:11:57  xliu
 * Branch: IosSec_branch
 * Commit before CA repository split.
 *
 * Revision 1.1.2.13  1996/03/26  18:19:26  xliu
 * Branch: IosSec_branch
 * Add vip support.
 *
 * Revision 1.1.2.12  1996/03/21  10:07:16  xliu
 * Branch: IosSec_branch
 * Migrated to CA branch.
 *
 * Revision 1.1.2.11  1996/03/01  01:39:25  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 1.1.2.10  1996/02/13  19:16:28  carrel
 * Branch: IosSec_branch
 * Crypto Map checkin (finally)
 *
 * Revision 1.1.2.9  1996/02/05  03:22:21  xliu
 * Branch: IosSec_branch
 * Add DH code; Add crypto_engine_encrypt/decrypt vectors.
 *
 * Revision 1.1.2.8  1996/01/30  22:35:56  xliu
 * Branch: IosSec_branch
 * Checking in encryption RSP/VIP support code.
 *
 * Revision 1.1.2.7  1996/01/24  22:56:00  bew
 * Branch: IosSec_branch
 * Sync-related changes to sys/crypto.
 *
 * Revision 1.1.2.6  1996/01/20  16:51:25  xliu
 * Branch: IosSec_branch
 * Solve the 7000 problem with ip les fastswitch code.
 *
 * Revision 1.1.2.5  1996/01/04  18:35:42  xliu
 * Branch: IosSec_branch
 * After initial clean up.
 *
 * Revision 1.1.2.4  1995/12/20  23:31:45  bew
 * Branch: IosSec_branch
 * Context is now freed after a sign operation. Before the callback is
 * called a pointer to the pak is taken so it can be used after the
 * callback returns.
 *
 * Revision 1.1.2.3  1995/12/10  09:27:12  xliu
 * Branch: IosSec_branch
 * Moved encryption from datagram_out to ipsendnet, after fragmentation.
 *
 * Revision 1.1.2.2  1995/12/02  17:42:43  xliu
 * Branch: IosSec_branch
 *
 * Modified epa driver interface.
 *
 * Revision 1.1.2.1  1995/11/30  04:59:43  xliu
 * Branch: IosSec_branch
 *
 * Moved the crypto engine core files into dir CORE.
 *
 * Revision 1.1  1995/11/30  04:20:48  xliu
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
#include "packet.h"
#include "mgd_timers.h"
#include "interface_private.h"
#include "../ip/ip.h"
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../crypto/crypto_public.h"
#include "../crypto/crypto.h"
#include "../crypto/crypto_engine_api.h"
#include "../crypto/crypto_engine/crypto_lib/lib_toolkit.h"
#include "../crypto/crypto_map.h"
#include "../crypto/crypto_sesmgmt.h"
#include "../crypto/crypto_pubkey.h"
#include "../crypto/crypto_registry.h"
#include "../crypto/crypto_engine_rsp_registry.h"
#include "../ui/debug.h"
#include "../crypto/crypto_debug.h"
#include "crypto_engine.h"
#include "crypto_swepa.h"

/* 
 * Global variable definition 
 *
 * Those veriables are available on all the platform.
 */
crypto_engine_config_t       crypto_engine_tbl[CRYPTO_MAX_SLOTS];
ulong			     epa_req_id = 0;
void			     crypto_engine_req_timer_update(void);
ushort                       crypto_engine_rpsp_slot = 0;
ushort                       crypto_engine_platform = CRYPTO_PLATFORM_RP;

static inline epa_crypto_blk* create_vip_verify_blk(epa_crypto_blk *blk);
static inline epa_crypto_blk* create_vip_sign_blk(epa_crypto_blk *blk);
epa_engine_req         epa_engine_req_pool[CRYPTO_ENGINE_REQPOOL_SIZE]; 

/*
 * crypto_engine_del_req_pool
 *
 * Delete the request from the request pool either because of the
 * completion of the request, or because of the timeout.
 */

epa_status  crypto_engine_del_req_pool (ushort req_id)
{
    ushort   index;
    epa_engine_req *req = NULL;

    /*
     * Later if it turns out there are larget amount of upstanding
     * requests, the efficient alg, like hashing alg, will be used to
     * increase the speed of indexing.
     */
    req = &epa_engine_req_pool[0];

    for (index=0; index< CRYPTO_ENGINE_REQPOOL_SIZE; index++) {
        if (req->req_id == req_id) {
	    req->req_id = CRYPTO_ENGINE_REQ_ENTRY_UNUSED;
	    return EPA_OP_OK;
        } else {
	    req++;
        }
    }

    if (crypto_engine_debug) {
       buginf("\nCRYPTO_ENGINE: can not find req %d from the req_pool.",
	      req_id);
    }
    return EPA_OP_FAIL;

};

/*
 *  crypto_engine_add_req_pool
 *
 *  Add the request to the request pool.
 */
epa_status crypto_engine_add_req_pool (ushort req_id,
                                    ushort command_id,
				    ushort crypto_unit_id,
				    ushort timeout_flag,
				    epa_callback_context *context,
				    epa_callback_handle callback)
{
    ushort     index;
    epa_engine_req *req = NULL;

    if (!context || !callback) {
        return EPA_OP_OK;
    }

    /*
     * Later if it turns out there are large amount of upstanding
     * requests, the efficient alg, like hashing alg, will be used to
     * increase the speed of indexing.
     */
    req = &epa_engine_req_pool[0];

    for (index=0; index< CRYPTO_ENGINE_REQPOOL_SIZE; index++) 
        if (req->req_id == CRYPTO_ENGINE_REQ_ENTRY_UNUSED) {
            req->req_id = req_id;
            req->command_id = command_id;
            req->crypto_unit_id = crypto_unit_id;
            req->proc_context = context;
            req->proc_callback = callback;
            if (timeout_flag) {
                 /*
                  * Save the req pointer to identify the request when
                  * the timer expired.
                  */
                 mgd_timer_init_leaf(&req->timer,&crypto_engine_master_timer,
                                     CRYPTO_KEY_COMM_TIMER, NULL, FALSE);
                 req->timer.mt_additional_context[0] = req;
                 mgd_timer_start(&req->timer, context->req_timeout);
            }
            return EPA_OP_OK;
        } else {
	    req++;
        }

    if (crypto_engine_debug) {
        buginf("\nCRYPTO_ENGINE: can not add req %d to the req_pool.", req_id);
    }
    return EPA_OP_FAIL;
};

/*
 * crypto_engine_find_req_info
 *
 * Return the entry as selected by the req_id.
 * This routine should use the more efficient indexing algorithm to search
 * the request pool. 
 */

epa_engine_req* crypto_engine_find_req_info (ushort  req_id)
{
    ushort index;
    epa_engine_req* req = NULL;

    req = &epa_engine_req_pool[0];
 
    for (index = 0; index < CRYPTO_ENGINE_REQPOOL_SIZE; index++)
        if (req->req_id == req_id) 
            return req;
        else
            req++;

    return NULL;
};

/* 
 * crypto_engine_timer_update
 *
 * This routine is called by the timer_process when crypto_engine_req_timer
 * expired.
 */
void crypto_engine_key_timeout (mgd_timer *expired_timer)
{
    epa_engine_req *req;
    epa_engine_req *timeout_req;
    unsigned short index;

    timeout_req = (epa_engine_req *)expired_timer->mt_additional_context[0];
    req = &epa_engine_req_pool[0];

    for (index = 0; index < CRYPTO_ENGINE_REQPOOL_SIZE; index++) 
        if (req->req_id == timeout_req->req_id ) {
            mgd_timer_stop(expired_timer);
            req->req_id = CRYPTO_ENGINE_REQ_ENTRY_UNUSED;
            if (req->proc_callback) {
                (* req ->proc_callback)(req->proc_context,
                                      req->command_id,
                                      EPA_TIMEOUT,
                                      NULL,
                                      0,
                                      NULL);
            }
            return;
        } else {
	    req++;
        }
};

/*
 * epa_get_blk_buffer
 *
 * Allocate a memory buffer for the request. Return the pointer to the
 * buffer.
 */

inline epa_crypto_blk* epa_get_blk_buffer (void) 
{
   epa_crypto_blk  *blk_ptr = NULL;

    blk_ptr = malloc(sizeof(epa_crypto_blk));
    if (!blk_ptr) {
	return NULL;
    }   

    blk_ptr->next = NULL;
    return blk_ptr;
};

/*
 * crypto_engine_req_dspt
 * 
 * Dispatch the key requests to 1) key process if the requests are for the
 * crypto engine on RP/RSP; 2) vip crypto engine otherwise. This only run
 * on RP/RSP.         
 */
epa_status epa_engine_req_dspt (ushort id,
				ulong  req_id,
				ushort command_type,
				epa_crypto_blk *blk,
				epa_callback_handle callback,
				epa_callback_context* context)
{
    ushort status = EPA_OP_OK;
    epa_crypto_blk *new_blk;

    if (!blk) {
        return EPA_OP_FAIL;
    }

    if (context && callback) {
        status = crypto_engine_add_req_pool (req_id,
                                         command_type,
                                         id,
                                         context->req_timeout,
                                         context,
                                         callback);

        if (status != EPA_OP_OK) {
            if (crypto_engine_debug) {
                buginf ("\nCRYPTO_ENGINE: can not add the req %d to the "
                        "req pool.", req_id);
            }
            return status;
        }
    }

    /* 
     * If the request is for the crypto engine on RP/RSP, enqueue the request
     * to the key process; if the request is for the crypto engine on vip,
     * send the request through IPC.
     */

    if ((crypto_engine_tbl[id].id == crypto_engine_rpsp_slot)
        || crypto_engine_tbl[id].id == CRYPTO_ENGINE_NOT_PRESENT) {
	crypto_key_enqueue(&crypto_engine_idb, blk);   
    } else {
       /* 
        * By default, we send the request to vip. If the crypto_engine
        * id does not represent a valid slot, returrn error there.
        */

       switch (blk->header.command_id) {
       case CRYPTO_VERIFY:
           new_blk = create_vip_verify_blk(blk);
           if (!new_blk) {
               if (crypto_engine_debug) {
                   buginf("\nCRYPTO_ENGINE: can not allocate blk.");
               }
               free(blk);
               return EPA_OP_FAIL;
           }
           free(blk);
           blk = new_blk;
           break;
       case CRYPTO_SIGN:
           new_blk = create_vip_sign_blk(blk);
           if (!new_blk) {
               if (crypto_engine_debug) {
                   buginf("\nCRYPTO_ENGINE: can not allocate blk.");
               }
               free(blk);
               return EPA_OP_FAIL;
           }
           free(blk);
           blk = new_blk;
           break;
       default:
           break;
       }
       reg_invoke_crypto_engine_req_remote_dspt(id,blk);
   }
   return status;
};

/*
 * crypto_engine_rsp_dspt
 *
 * Dispatch the response to the Session Manager. This only run on RP/RSP.
 * Need to add timeout management later. 
 */
void epa_engine_rsp_dspt (epa_crypto_blk *blk)
{
    epa_pak_info pak_info;
    ushort       status;
    epa_engine_req *entry = NULL;

    if (!blk) {
        return;
    }

    /* find the entry from the epa_req_pool */
    entry = crypto_engine_find_req_info(blk->header.req_id);
    if (!entry) {
        /* There is no need to send back the response. Simply return. */
        return;
    }

    /* Stop the timer if there is a timer assoicated with this req */
    if (mgd_timer_initialized(&entry->timer)) {
        mgd_timer_stop(&entry->timer);
    }

    /* Invoke the process callback. */
    if (entry->proc_callback) {
        (* entry->proc_callback)(entry->proc_context,
                                 blk->header.command_id,
                                 blk->header.status,
                                 blk->data,
                                 blk->header.data_size,
  	                         &pak_info); 
    }
      
    /* Delete the request from the request pool. */
    status = crypto_engine_del_req_pool(blk->header.req_id);
    if (status != EPA_OP_OK) {
       return;
    }

   free(blk);
}

static inline epa_crypto_blk * create_vip_verify_blk (epa_crypto_blk *blk)
{
    crypto_verify_data_blk  *data;
    epa_crypto_blk *new_blk = NULL;
    crypto_vip_verify_data_blk *new_data;
    new_blk = malloc(sizeof(epa_crypto_blk));
    if (!new_blk) {
        if (crypto_engine_debug) {
            buginf("\nCRYPTO_ENGINE: can not allocate blk.");
        }
        return NULL;
    }
    new_blk->next = NULL;

    bcopy(&blk->header, &new_blk->header, sizeof(epa_crypto_blk_header));

    data = (crypto_verify_data_blk *)blk->data;
    new_data = (crypto_vip_verify_data_blk *)new_blk->data;

    new_data->clear_data_length = data->clear_data_length;
    new_data->signature_length = data->signature_length;
    new_data->alg_type = data->alg_type;
    bcopy(data->public_key, new_data->public_key, DSS_LENGTH_MIN);
    bcopy(data->clear_data,
          new_blk->data + sizeof(crypto_vip_verify_data_blk),
          data->clear_data_length);
    bcopy(data->signature,
          new_blk->data + sizeof(crypto_vip_verify_data_blk)
          + data->clear_data_length, data->signature_length);
    new_blk->header.data_size = sizeof(crypto_vip_verify_data_blk) +
                                new_data->clear_data_length +
                                new_data->signature_length;
    new_blk->header.command_id = CRYPTO_VIP_VERIFY;

    return new_blk;
}
/*
 * Copy the data to be signed into epa_crypto_blk data buffer since the
 * request is to be send to vip.
 */
static inline epa_crypto_blk * create_vip_sign_blk (epa_crypto_blk *blk)
{
    crypto_sign_data_blk  *data_blk;
    epa_crypto_blk *new_blk = NULL;
    crypto_vip_sign_data_blk *new_data;

    new_blk = malloc(sizeof(epa_crypto_blk));
    if (!new_blk) {
        if (crypto_engine_debug) {
            buginf("\nCRYPTO_ENGINE: can not allocate blk.");
        }
        return NULL;
    }
    new_blk->next = NULL;

    bcopy(&blk->header, &new_blk->header, sizeof(epa_crypto_blk_header));

    data_blk = (crypto_sign_data_blk *)blk->data;
    new_data = (crypto_vip_sign_data_blk *)new_blk->data;

    new_data->data_length = data_blk->data_length;
    new_data->alg_type = data_blk->alg_type;

    bcopy(data_blk->data, new_blk->data + sizeof(crypto_vip_sign_data_blk), 
          data_blk->data_length);

    new_blk->header.data_size = sizeof(crypto_vip_sign_data_blk) + 
                                new_data->data_length; 
    new_blk->header.command_id = CRYPTO_VIP_SIGN;

    return new_blk;
}

