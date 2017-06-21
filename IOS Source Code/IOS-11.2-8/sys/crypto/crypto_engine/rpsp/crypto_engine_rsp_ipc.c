/* $Id: crypto_engine_rsp_ipc.c,v 1.1.50.22 1996/09/15 20:29:22 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/rpsp/crypto_engine_rsp_ipc.c,v $
 *------------------------------------------------------------------
 * Crypto Engine RSP ipc requests.
 *
 * Jan 1996, xliu 
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_engine_rsp_ipc.c,v $
 * Revision 1.1.50.22  1996/09/15  20:29:22  xliu
 * CSCdi68755:  crypto maps not needed by vip2 should not be donwload to
 * vip2.
 * Branch: California_branch
 * Enforce the crypto map download as a complete transaction which inlcude
 * downloading the head, the map and peupe pair.
 *
 * Revision 1.1.50.21  1996/09/13  08:42:06  xliu
 * CSCdi68755:  crypto maps not needed by vip2 should not be donwload to
 * vip2.
 * Branch: California_branch
 * Only donwload the crypto maps which are assigned to vip interface.
 *
 * Revision 1.1.50.20  1996/08/15  19:16:36  xliu
 * CSCdi65915:  crypto clear-latch does not work
 * Branch: California_branch
 * Check for EPA_XTRACT status bit.
 *
 * Revision 1.1.50.19  1996/08/15  18:03:48  ptran
 * Register EPA's idb into VIP and RSP hwidb, swidb chain.
 * CSCdi64021:  show diag doesnt display EPA info when the next PA is empty
 * Branch: California_branch
 *
 * Revision 1.1.50.18  1996/08/15  07:56:36  xliu
 * CSCdi65871:  Wrong password to crypto clear-latch causes hang
 * Branch: California_branch
 * Set watched boolean to be TRUE when the crypto card returned with a
 * error
 * status.
 *
 * Revision 1.1.50.17  1996/08/14  15:57:50  bew
 * CSCdi65991:  crypto clear-latch asks for password if not a vip
 * Branch: California_branch
 * Added a missing return in crypto-latch processing.
 *
 * Revision 1.1.50.16  1996/08/09  23:31:15  xliu
 * CSCdi63854:  Traffic is pass in clear text when distributed switching
 * is off
 * Branch: California_branch
 *
 * Revision 1.1.50.15  1996/08/07  09:21:56  xliu
 * CSCdi63274:  Wrong crypto engine name a show cr eng conf
 * Branch: California_branch
 *
 * Revision 1.1.50.14  1996/08/07  08:26:34  snijsure
 * CSCdi65248:  encryption mib doesnt show correct status of ESA.
 * Branch: California_branch
 * Retrieve ESA status using crypto_show_card_status,
 * Also assign proper value to cieEngineCardIndex, using the chassis mib
 * interface.
 *
 * Revision 1.1.50.13  1996/07/31  03:29:09  bew
 * CSCdi64375:  Invalid  function called in NVGEN of crypto clear-latch
 * Branch: California_branch
 * Standardized a printf.
 *
 * Revision 1.1.50.12  1996/07/28  07:44:16  xliu
 * CSCdi62947:  Add parser command to query epa status from RSP
 * This fix also solve the problems reported by CSCdi63488, CSCdi63852.
 * Branch: California_branch
 *
 * Revision 1.1.50.11  1996/07/15  10:06:46  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 1.1.50.10  1996/07/06  19:45:13  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 1.1.50.9  1996/07/03  05:54:59  snijsure
 * CSCdi60711:  sh cr conn eng act does not show anything in vip2 crypto
 * router
 * Branch: California_branch
 *
 * Revision 1.1.50.8  1996/07/01  12:26:33  xliu
 * CSCdi61743:  decryption does not use cached pak pointer correctly.
 * Branch: California_branch
 *
 * Revision 1.1.50.7  1996/06/30  18:11:04  ptran
 * CSCdi61257:  snmp and show diag support for encryption PA
 * Branch: California_branch
 *
 * Revision 1.1.50.6  1996/06/28  23:40:48  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 1.1.50.5  1996/06/21  22:32:01  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 1.1.50.4  1996/06/05  23:11:51  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Fix crypto zeroize & key generation for 7500 with VIPs.
 * Fix crypto connection setup with traffic in both directions.
 * Fix encryption bugs.
 *
 * Revision 1.1.50.3  1996/05/28  15:59:56  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 1.1.50.2  1996/05/12  23:13:13  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 1.1.50.1  1996/04/19  14:57:05  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.9  1996/04/17  23:48:12  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and Removing wrong slot assumption.
 *
 * Revision 1.1.2.8  1996/04/09  20:03:10  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 1.1.2.7  1996/03/30  05:12:14  xliu
 * Branch: IosSec_branch
 * Commit before CA repository split.
 *
 * Revision 1.1.2.6  1996/03/22  17:40:31  xliu
 * Branch: IosSec_branch
 * Add ipcache update to crypto map.
 *
 * Revision 1.1.2.5  1996/03/21  21:17:07  xliu
 * Branch: IosSec_branch
 * Add registry call to download crypto map.
 *
 * Revision 1.1.2.4  1996/03/21  10:08:41  xliu
 * Branch: IosSec_branch
 * Integrated with RSP changes.
 *
 * Revision 1.1.2.3  1996/01/30  22:36:07  xliu
 * Branch: IosSec_branch
 * Checking in encryption RSP/VIP support code.
 *
 * Revision 1.1.2.2  1996/01/19  07:01:02  xliu
 * Branch: IosSec_branch
 * Commit before the second sync.
 *
 * Revision 1.1.2.1  1996/01/09  07:21:53  xliu
 * Branch: IosSec_branch
 * Crypto engine RSP.
 *
 * Revision 1.1  1996/01/09  05:32:08  xliu
 * Crypto engine RSP.
 *
 * Revision 1.7  1995/06/07 12:03:03  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include <ciscolib.h>
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "sched.h"
#include "stdlib.h"
#include "config.h"
#include "../ip/ip.h"
#include "../ip/ipfast.h"
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../ipc/ipc_private.h"
#include "access.h"
#include "../ip/ipfast_index.h"
#include "../ip/ipfast_ipc.h"
#include "../crypto/crypto_public.h"
#include "../crypto/crypto_registry.h"
#include "../crypto/crypto_map.h"
#include "../crypto/crypto.h"
#include "../crypto/crypto_engine_api.h"
#include "../crypto/crypto_pubkey.h"
#include "../crypto/crypto_engine/crypto_lib/lib_toolkit.h"
#include "../crypto/crypto_engine/crypto_lib/encrypt.h"
#include "../crypto/crypto_engine/core/crypto_engine.h"
#include "../crypto/crypto_engine/core/crypto_swepa.h"
#include "../crypto/crypto_engine/rpsp/crypto_engine_rsp_ipc.h"
#include "../ui/debug.h"
#include "../crypto/crypto_debug.h"
#include "../snmp/chassismib_api.h"
#include "../snmp/chassismib_registry.h"
#include "../hes/if_fci.h"
#include "../hes/if_vip.h"
#include "../hes/dgbus.h"
#include "ttysrv.h"
#include "../hes/ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"
#include "../ui/command.h"

/* function prototype */
uchar *generate_ds_ctrl_port_name(ushort, boolean);
extern slottype slots[];
extern sub_card_type_and_descr (int , int , int* , char** );
static void set_epa_config (uchar *);

/* static veriable */
static watched_boolean *rsp_parser_ready_boolean;
boolean rsp_parser_waiting;

/* 
 * ipc global variable. 
 */
ipc_port_id crypto_engine_rsp_crnt_port;

/*
 * At the end of    
 */
ushort  crypto_vip_no_connections=0;
epa_vip_connection_tbl *crypto_vip_connection_tbl;

void crypto_engine_extract_vip_conn_tbl (ipc_message_header *msg)
{
    ushort  nbytes;

    epa_crypto_blk_header *crypto_blk_hdr;

    crypto_blk_hdr = (epa_crypto_blk_header *)msg->data;
    if ( !crypto_blk_hdr )
       return;

    if (crypto_engine_debug) {
       buginf("\nCRYPTO ENGINE:Number of connection entries received from "
              "VIP %d\n", crypto_blk_hdr->data_size);
    }

    if ( crypto_blk_hdr->data_size > 0 ) {
       nbytes =  crypto_blk_hdr->data_size* sizeof(epa_vip_connection_tbl);
       crypto_vip_no_connections = crypto_blk_hdr->data_size;
       crypto_vip_connection_tbl = malloc(nbytes); 
       if ( !crypto_vip_connection_tbl ) {
          buginf("\nCould not allocate enough memory (%d bytes) \n",nbytes);
          return;
       }
       bcopy((msg->data+sizeof(epa_crypto_blk_header)),crypto_vip_connection_tbl,
           nbytes);
    }
}

static void set_epa_config (uchar *msg)
{
    epa_rtn_get_status rtn_status;
    epa_crypto_blk_header* blk_hdr;

    blk_hdr = (epa_crypto_blk_header *)msg;

    if (blk_hdr->status != 0) {
        printf("\n%s", crypto_card_error_code(blk_hdr->status));
        if (rsp_parser_waiting) {
            process_set_boolean(rsp_parser_ready_boolean, TRUE);
            rsp_parser_waiting = FALSE;
        }
        return;
    }

    bcopy(msg + sizeof(epa_crypto_blk_header), &rtn_status, 
          sizeof(rtn_status));

    if (crypto_engine_tbl[rtn_status.slot].crypto_unit_type != CRYPTO_HWEPA) {
        if (crypto_engine_debug) {
            buginf("\nCRYPTO_ENGINE: wrong epa type.");
        }
        return;
    }


    /* Update the crypto engine table */
    crypto_set_card_status(rtn_status.slot, rtn_status.status);  

    if (rsp_parser_waiting) {
        process_set_boolean(rsp_parser_ready_boolean, TRUE);
        rsp_parser_waiting = FALSE;
    }
}

/*
 * crypto_engine_rsp_ipc_callback
 *
 * Called when the crypto engine on VIP send back the response to
 * crypto engine on RSP. This callback is registered with 
 * crypto_engine_rsp_crnt_port. 
 */
void crypto_engine_rsp_ipc_callback (void *msg)
{
    epa_crypto_blk *blk = NULL;
    ipc_message_header *header = (ipc_message_header *)msg;
    epa_crypto_blk_header *crypto_blk_hdr;
    uchar *data = header->data;
    extern watched_boolean *crypto_vip_update_conn_tbl;

    crypto_blk_hdr = (epa_crypto_blk_header *)data;

    switch (crypto_blk_hdr->command_id) {
    case CRYPTO_ENGINE_VIP_GET_CFG:
        crypto_vip_init_maps(crypto_blk_hdr);
        return;

    case  CRYPTO_ENGINE_VIP_SEND_TBL:
       crypto_engine_extract_vip_conn_tbl (header);
       process_set_boolean(crypto_vip_update_conn_tbl, TRUE); 
       return;

    case CRYPTO_GET_EPA_STATUS:
    case CRYPTO_CLEAR_LATCH:
       /* This is the callback after RSP query VIP for epa status */
       set_epa_config(header->data);
       return;

    default:
        /* By default, we assume this is the key response message */
        break;
    }

    /* 
     * Forward the response to the Session Manager.
     */

    blk = malloc(sizeof(epa_crypto_blk));
    if (!blk) {
        if (crypto_engine_debug) {
            buginf("\nCRYPTO_ENGINE: can not allocate blk.");
        }
        return;
    }
    blk->next = NULL;
        
    /* 
     * Assume the data is in the format: 
     *       <crypto header, crypto data> 
     */
    bcopy(data, &blk->header, sizeof(epa_crypto_blk_header));
    bcopy(data + sizeof (epa_crypto_blk_header), blk->data, 
          blk->header.data_size);

    /* Pass the response to Session Manager. */
    epa_engine_rsp_dspt (blk);
    return;
}

/*
 * crypto_engine_req_remote_dspt
 *
 * Send out the key related request to a crypto engine on vip. This is
 * called by epa_engine_req_dspt when the given crypto_engine_id indicates
 * a crypto engine on vip.   
 * Note: name should be changed to be more meaningful.
 */

void crypto_engine_req_remote_dspt (ushort crypto_engine_id, void *req)
{
    crypto_engine_vip_ipc_msg *msg;
    epa_crypto_blk *blk = req;
    ushort size;
    int slot = crypto_engine_id;

    /* Create crypto engine vip ipc message */
    size = sizeof(ipc_ipfast_msg_hdr) + sizeof(epa_crypto_blk_header) +
           blk->header.data_size + sizeof(int);
    msg = ip_ipc_get_msg(size);
    msg->hdr.ipfast_type = IPFAST_CRYPTO_ENGINE_VIP_KEY_REQUEST;
    msg->hdr.msg_size = size;
    msg->slot = slot;
    msg->data = (uchar *)msg + sizeof(ipc_ipfast_msg_hdr) + sizeof(int);
    bcopy((uchar *)blk + sizeof(blk->next), msg->data,
           sizeof(epa_crypto_blk_header) + blk->header.data_size);

    ip_ipc_send_msg(size, IPC_TYPE_IP_CACHE, TRUE);

    free(req);
}

/*
 * crypto_vip_map_head_add
 *
 * Called to add a crypto map head to vip.
 */

void crypto_vip_map_head_add (void *head)
{
    crypto_vip_map_tag_msg head_msg;
    crypto_map_headtype *new_head = (crypto_map_headtype *)head;
    crypto_engine_vip_ipc_msg *msg;
    ushort size;
    boolean find = FALSE;
    hwidbtype *hwidb;
    int slot;

    /*
     * Download the head to the vip only in the case when there are
     * vip hwidb referenced the head and the head has been removed before.
     */
    FOR_ALL_HWIDBS(hwidb) {
        if (is_vip(hwidb) && hwidb->firstsw->crypto_map &&
              !strncmp(hwidb->firstsw->crypto_map, new_head->map_tag, 
                       strlen(new_head->map_tag))) {
            find = TRUE;
            break;
        }
    }

    if (!find)
        return;

    head_msg.command = CRYPTO_VIP_ADD_MAP_HEAD;
    bcopy(new_head->map_tag, head_msg.tag, strlen(new_head->map_tag));
    head_msg.tag_size = strlen(new_head->map_tag); 

    size = sizeof(ipc_ipfast_msg_hdr) + sizeof(head_msg) + sizeof(int);
    slot = hwidb->slot;

    msg = ip_ipc_get_msg(size);
    msg->hdr.ipfast_type = IPFAST_CRYPTO_ENGINE_VIP_UPD_MAP; 
    msg->hdr.msg_size = size; 
    msg->slot = slot;
    msg->data = (uchar *)msg + sizeof(ipc_ipfast_msg_hdr) + sizeof(int);
    bcopy((uchar*)&head_msg, msg->data, sizeof(head_msg));

    ip_ipc_send_msg(size, IPC_TYPE_IP_CACHE, TRUE);
}

/*
 * crypto_vip_get_conn_tbl
 *
 * Retrieve connection table information from VIP. 
 * This code gets executed on RSP.
 */

void crypto_initiate_vip_get_conn_tbl (int slot)
{
    /* For time being ride along on MAP messages till we have a generic
     * ipfast_type message defined for crypto subsystem to transfer data
     * from vip to rsp. 
     */

    crypto_vip_get_conn_tbl_msg rsp_to_vip_msg;
    crypto_engine_vip_ipc_msg *msg;
    ushort size;

    rsp_to_vip_msg.command = CRYPTO_VIP_GET_CONN_TBL;
    rsp_to_vip_msg.crypto_engine_id = slot;

    size = sizeof(ipc_ipfast_msg_hdr) + sizeof(crypto_vip_get_conn_tbl_msg)
						+ sizeof(int);

    msg = ip_ipc_get_msg(size);
    msg->hdr.ipfast_type = IPFAST_CRYPTO_ENGINE_VIP_UPD_MAP; 
    msg->hdr.msg_size = size; 
    msg->slot = slot;
    msg->data = (uchar *)msg + sizeof(ipc_ipfast_msg_hdr) + sizeof(int);
    bcopy((uchar*)&rsp_to_vip_msg, msg->data, 
				sizeof(crypto_vip_get_conn_tbl_msg));

    ip_ipc_send_msg(size, IPC_TYPE_IP_CACHE, TRUE);
}

/*
 * crypto_vip_map_add
 *
 * Add a crypto map to a crypto map sequences with the given tag, to VIP.
 */

void crypto_vip_map_add (char *map)
{
    crypto_vip_upd_map_msg map_msg;
    crypto_maptype *new_map = (crypto_maptype *)map;
    crypto_map_headtype *head = (crypto_map_headtype *)new_map->head;
    crypto_engine_vip_ipc_msg *msg;
    ushort size;
    int slot;

    if (!head) {
        if (crypto_engine_debug) {
            buginf("\nCRYPTO_ENGINE: add map to the sequence with null head.");
        }
        return;
    }

    /* Always download the head as part of the transaction */
    crypto_vip_map_head_add(head);

    map_msg.command = CRYPTO_VIP_ADD_MAP;
    map_msg.seq_no = new_map->seq_no;
    map_msg.conn_id = new_map->conn_id;
    map_msg.mode_flags = new_map->mode_flags;
    map_msg.algorithms = new_map->algorithms;
    map_msg.num_conn = new_map->num_conn;
    bcopy(head->map_tag, map_msg.tag, strlen(head->map_tag));
    map_msg.tag_size = strlen(head->map_tag);

    size = sizeof(ipc_ipfast_msg_hdr) + sizeof(map_msg) + sizeof(int);
    slot = -1;

    msg = ip_ipc_get_msg(size);
    msg->hdr.ipfast_type = IPFAST_CRYPTO_ENGINE_VIP_UPD_MAP; 
    msg->hdr.msg_size = size; 
    msg->slot = slot;
    msg->data = (uchar *)msg + sizeof(ipc_ipfast_msg_hdr) + sizeof(int);
    bcopy((uchar*)&map_msg, msg->data, sizeof(map_msg));

    ip_ipc_send_msg(size, IPC_TYPE_IP_CACHE, TRUE);
}

/*
 * crypto_vip_map_set_peupe
 *
 * Set pe/upe pair to a given map.
 */
void crypto_vip_map_set_peupe (char *map)
{
    crypto_vip_upd_map_msg map_msg;
    crypto_maptype *new_map = (crypto_maptype *)map;
    crypto_map_headtype *head = (crypto_map_headtype *)new_map->head;
    crypto_engine_vip_ipc_msg *msg;
    ushort size;
    int slot;

    if (!head) {
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: emptry crypto map head.\n");
        }
        return;
    } 

    /* Always download the map first */
    crypto_vip_map_add(map);

    map_msg.command = CRYPTO_VIP_MAP_SET_PEUPE;
    map_msg.seq_no = new_map->seq_no;
    map_msg.conn_id = new_map->conn_id;
    map_msg.pe_addr = new_map->pe.ip_addr;
    map_msg.upe_addr = new_map->upe.ip_addr;
    map_msg.algorithms = new_map->algorithms;
    map_msg.acl_number = atoi(new_map->acl_list->name);
    map_msg.num_conn = new_map->num_conn;
    bcopy(head->map_tag, map_msg.tag, strlen(head->map_tag));
    map_msg.tag_size = strlen(head->map_tag);

    size = sizeof(ipc_ipfast_msg_hdr) + sizeof(map_msg) + sizeof(int);

    /* indicate all vips */
    slot = -1;

    msg = ip_ipc_get_msg(size);
    msg->hdr.ipfast_type = IPFAST_CRYPTO_ENGINE_VIP_UPD_MAP; 
    msg->hdr.msg_size = size; 
    msg->slot = slot;
    msg->data = (uchar *)msg + sizeof(ipc_ipfast_msg_hdr) + sizeof(int);
    bcopy((uchar*)&map_msg, msg->data, sizeof(map_msg));

    ip_ipc_send_msg(size, IPC_TYPE_IP_CACHE, TRUE);
}

/*
 * crypto_vip_map_set_modeflags
 *
 * Set special mode flags in a given map.
 */
void crypto_vip_map_set_modeflags (char *map)
{
    crypto_vip_upd_map_msg map_msg;
    crypto_maptype *new_map = (crypto_maptype *)map;
    crypto_map_headtype *head = (crypto_map_headtype *)new_map->head;
    crypto_engine_vip_ipc_msg *msg;
    ushort size;
    int slot;
 
    if (!head) {
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: emptry crypto map head.\n");
        }
        return;
    } 

    /* Always download the map first */
    crypto_vip_map_add(map);

    map_msg.command = CRYPTO_VIP_MAP_SET_MODE;
    map_msg.seq_no = new_map->seq_no;
    map_msg.mode_flags = new_map->mode_flags;
    bcopy(head->map_tag, map_msg.tag, strlen(head->map_tag));
    map_msg.tag_size = strlen(head->map_tag);

    size = sizeof(ipc_ipfast_msg_hdr) + sizeof(map_msg) + sizeof(int);

    /* indicate all vips */
    slot = -1;

    msg = ip_ipc_get_msg(size);
    msg->hdr.ipfast_type = IPFAST_CRYPTO_ENGINE_VIP_UPD_MAP; 
    msg->hdr.msg_size = size; 
    msg->slot = slot;
    msg->data = (uchar *)msg + sizeof(ipc_ipfast_msg_hdr) + sizeof(int);
    bcopy((uchar*)&map_msg, msg->data, sizeof(map_msg));

    ip_ipc_send_msg(size, IPC_TYPE_IP_CACHE, TRUE);
}

/*
 * crypto_vip_map_delete 
 */
void crypto_vip_map_delete (char *map)
{
    crypto_vip_upd_map_msg map_msg;
    crypto_maptype *new_map = (crypto_maptype *)map;
    crypto_map_headtype *head = (crypto_map_headtype *)new_map->head;
    crypto_engine_vip_ipc_msg *msg;
    ushort size;
    int slot;

    if (!head) {
        if (crypto_engine_debug) {
            buginf("\nCRYPTO_ENGINE: emptry crypto map head.\n");
        }
        return;
    } 

    map_msg.command = CRYPTO_VIP_DEL_MAP;
    map_msg.seq_no = new_map->seq_no;
    bcopy(head->map_tag, map_msg.tag, strlen(head->map_tag));
    map_msg.tag_size = strlen(head->map_tag);
 
    size = sizeof(ipc_ipfast_msg_hdr) + sizeof(map_msg) + sizeof(int);

    /* indicate all vips */
    slot = -1;

    msg = ip_ipc_get_msg(size);
    msg->hdr.ipfast_type = IPFAST_CRYPTO_ENGINE_VIP_UPD_MAP; 
    msg->hdr.msg_size = size; 
    msg->slot = slot;
    msg->data = (uchar *)msg + sizeof(ipc_ipfast_msg_hdr) + sizeof(int);
    bcopy((uchar*)&map_msg, msg->data, sizeof(map_msg));

    ip_ipc_send_msg(size, IPC_TYPE_IP_CACHE, TRUE);
}


/*
 * crypto_vip_map_head_delete
 */
void crypto_vip_map_head_delete (uchar *tag)
{
    crypto_vip_map_tag_msg head_msg;
    crypto_engine_vip_ipc_msg *msg;
    ushort size;
    int slot;

    head_msg.command = CRYPTO_VIP_DEL_MAP_HEAD;
    bcopy(tag, head_msg.tag, strlen(tag));
    head_msg.tag_size = strlen(tag); 

    size = sizeof(ipc_ipfast_msg_hdr) + sizeof(head_msg) + sizeof(int);

    /* indicate all vips */
    slot = -1;

    msg = ip_ipc_get_msg(size);
    msg->hdr.ipfast_type = IPFAST_CRYPTO_ENGINE_VIP_UPD_MAP; 
    msg->hdr.msg_size = size; 
    msg->slot = slot;
    msg->data = (uchar *)msg + sizeof(ipc_ipfast_msg_hdr) + sizeof(int);
    bcopy((uchar*)&head_msg, msg->data, sizeof(head_msg));

    ip_ipc_send_msg(size, IPC_TYPE_IP_CACHE, TRUE);
}

/*
 * crypto_vip_assing_map_to_int
 *
 * Assign all the maps, included by a given tag, to an output interface.
 */

void crypto_vip_assign_map_to_int (hwidbtype *idb, void *head)
{
    crypto_vip_assign_map_to_int_msg int_msg;
    crypto_map_headtype *new_head = (crypto_map_headtype *)head;
    crypto_engine_vip_ipc_msg *msg;
    ushort size;
    crypto_maptype *map;
    int slot;

    /* First make sure the idb is a VIP interface */
    if (!is_vip(idb)) {
  	return;
    }

    /* Always download the head and map, as part of the transaction. */
    if (!new_head) {
        if (crypto_engine_debug) {
            buginf("\nCRYPTO_ENGINE: add map to the sequence with null head.");
        }
        return;
    }
    crypto_vip_map_head_add (new_head);

    for (map= new_head->top; map; map = map->next) {
        crypto_vip_map_add((char *)map);
        if (map->pe.type != 0 &&  map->upe.type != 0) {
            crypto_vip_map_set_peupe((char *)map);
        }
    }

    int_msg.command = CRYPTO_VIP_ASSIGN_MAP;
    int_msg.hw_if_index = idb->hw_if_index;
    bcopy(new_head->map_tag,int_msg.map_tag, strlen(new_head->map_tag));
    int_msg.tag_size = strlen(new_head->map_tag);

    size = sizeof(ipc_ipfast_msg_hdr) + sizeof(int_msg) + sizeof(int);
    slot = idb->slot;
    msg = ip_ipc_get_msg(size);
    msg->hdr.ipfast_type = IPFAST_CRYPTO_ENGINE_VIP_UPD_MAP; 
    msg->hdr.msg_size = size; 
    msg->slot = slot;
    msg->data = (uchar *)msg + sizeof(ipc_ipfast_msg_hdr) + sizeof(int);
    bcopy((uchar*)&int_msg, msg->data, sizeof(int_msg));

    ip_ipc_send_msg(size, IPC_TYPE_IP_CACHE, TRUE);
}

/*
 * crypto_vip_remove_map_from_int
 *
 * Assign all the maps, included by a given tag, to an output interface.
 */

void crypto_vip_remove_map_from_int (hwidbtype *idb, void *head)
{
    crypto_vip_assign_map_to_int_msg int_msg;
    crypto_map_headtype *new_head = (crypto_map_headtype *)head;
    crypto_engine_vip_ipc_msg *msg;
    ushort size;
    int slot;

    if (!head || !(is_vip(idb))) {
        return;
    }

    int_msg.command = CRYPTO_VIP_REMOVE_MAP;
    int_msg.hw_if_index = idb->hw_if_index;
    bcopy(new_head->map_tag,int_msg.map_tag, strlen(new_head->map_tag));
    int_msg.tag_size = strlen(new_head->map_tag);

    size = sizeof(ipc_ipfast_msg_hdr) + sizeof(int_msg) + sizeof(int);
    slot = idb->slot;

    msg = ip_ipc_get_msg(size);
    msg->hdr.ipfast_type = IPFAST_CRYPTO_ENGINE_VIP_UPD_MAP; 
    msg->hdr.msg_size = size; 
    msg->slot = slot;
    msg->data = (uchar *)msg + sizeof(ipc_ipfast_msg_hdr) + sizeof(int);
    bcopy((uchar*)&int_msg, msg->data, sizeof(int_msg));

    ip_ipc_send_msg(size, IPC_TYPE_IP_CACHE, TRUE);
}

/*
 * crypto_vip_upd_map_conn_id
 *
 * Update the connection id for all the maps matched with the 
 * given tag.
 */ 
void crypto_vip_upd_map_conn_id (char *map_ptr, int old_id)
{
    crypto_vip_map_upd_conn_msg conn_msg;
    crypto_engine_vip_ipc_msg *msg;
    crypto_maptype *map = (crypto_maptype *)map_ptr;
    crypto_map_headtype *head = (crypto_map_headtype*)map->head;
    ushort size;
    int slot;

    if (!head) {
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: map is not defined with map head.");
        }
    }
 
    conn_msg.command = CRYPTO_VIP_UPD_CONN_ID;
    conn_msg.new_conn_id = map->conn_id;
    conn_msg.seq_no = map->seq_no;
    conn_msg.tag_size = strlen(head->map_tag);
    bcopy(head->map_tag, conn_msg.map_tag, conn_msg.tag_size);
    conn_msg.old_conn_id  = old_id;

    size = sizeof(ipc_ipfast_msg_hdr) + sizeof(conn_msg) + sizeof(int);

    /* indicate all vips */
    slot = -1;

    msg = ip_ipc_get_msg(size);
    msg->hdr.ipfast_type = IPFAST_CRYPTO_ENGINE_VIP_UPD_MAP; 
    msg->hdr.msg_size = size; 
    msg->slot = slot;
    msg->data = (uchar *)msg + sizeof(ipc_ipfast_msg_hdr) + sizeof(int);
    bcopy((uchar*)&conn_msg, msg->data, sizeof(conn_msg));
 
    ip_ipc_send_msg(size, IPC_TYPE_IP_CACHE, TRUE);
}

/*
 * init_vip_dss_key
 *
 * Download dss key to vip.
 */
static void init_vip_dss_key (ushort slot, crypto_mykey *key)
{
    epa_crypto_blk *blk;

    if (!key) {
        return;
    }

    blk = malloc (sizeof(epa_crypto_blk));
    if (!blk) {
        return;
    }

    blk->header.command_id = CRYPTO_INIT;
    bcopy(key->private_key.key, blk->data, key->private_key.keylen);
    bcopy(key->public_key.key, blk->data+key->private_key.keylen,
          key->public_key.keylen);
    blk->header.data_size = key->private_key.keylen + key->public_key.keylen; 

    crypto_engine_req_remote_dspt (slot, (void *)blk);
}

/*
 * get_epa_status 
 *
 * Get epa status. 
 */
static void get_epa_status (ushort slot, crypto_mykey *key)
{
    epa_crypto_blk *blk;

    blk = malloc (sizeof(epa_crypto_blk));
    if (!blk) {
        return;
    }

    blk->header.command_id = CRYPTO_GET_EPA_STATUS;

    if (key) {
        bcopy(key->private_key.key, blk->data, key->private_key.keylen);
        blk->header.data_size = key->private_key.keylen;
    } else {
        blk->header.data_size = 0;
    }
    crypto_engine_req_remote_dspt (slot, (void *)blk);
}


/*
 * crypto_vip_init_maps
 *
 * Called when crypto_engine_rsp_ipc_callback receive vip request
 * to get crypto maps from rsp at initialization time.
 */
void crypto_vip_init_maps (epa_crypto_blk_header *crypto_blk_hdr)
{
    crypto_map_headtype *head, *tmp_head;
    crypto_maptype *map;
    crypto_mykey *mykey;
    idbtype *idb, *tmp_idb;
    hwidbtype *tmp_hwidb;
    boolean crypto_map_set = FALSE;


    /* 
     * Update crypto_engine_tbl which has been initialize when
     * crypto subsys is initialized.
     * <req_id:slot, status: type>
     */
    if (crypto_blk_hdr->status == CRYPTO_TYPE_UNKNOWN) {
        printf("\nCRYPTO: No crypto engine is installed on vip.");
        crypto_engine_tbl[crypto_blk_hdr->req_id].crypto_unit_type =
                                                CRYPTO_TYPE_UNKNOWN;
        /*
         * Remove crypto maps assigned to any vip interface.
         */
        FOR_ALL_SWIDBS(idb) {
            if (is_vip(idb->hwptr) && (idb->crypto_map)) {
                tmp_head = find_crypto_map_head(idb->crypto_map);
                free(idb->crypto_map);
                idb->crypto_map = NULL;
                idb->hwptr->ip_fast_flags &= ~IP_FAST_CRYPTO_MAP;

                FOR_ALL_SWIDBS(tmp_idb) {
                    if (tmp_idb->crypto_map) {
                        crypto_map_set = TRUE;
                        break;
                    }
                }

                if (!crypto_map_set) {
                    FOR_ALL_HWIDBS(tmp_hwidb) {
                        tmp_hwidb->ip_fast_flags &= !IP_FAST_INPUT_CRYPTO_MAP;
                    }
                }
                reg_invoke_crypto_engine_restore_media_vectors(idb->hwptr,
                                                               tmp_head);
            }
         }
        return;
    }

    crypto_engine_tbl[crypto_blk_hdr->req_id].id = crypto_blk_hdr->req_id;
    crypto_engine_tbl[crypto_blk_hdr->req_id].crypto_unit_type =
                                                   crypto_blk_hdr->status;
    crypto_engine_tbl[crypto_blk_hdr->req_id].state =
                                               CRYPTO_ENGINE_INSTALLED;

    for (head = crypto_maps; head; head = head->next) {
        if (head->top->slot == crypto_blk_hdr->req_id) {
            reg_invoke_crypto_vip_map_head_add((void *)head);
        } else {
            continue;
        }
        for (map = head->top; map; map = map->next) {
            map->crypto_engine_id = crypto_get_engine_id(map->slot);
            reg_invoke_crypto_vip_map_add((char *)map);
            reg_invoke_crypto_vip_map_set_peupe((char *)map);
            FOR_ALL_SWIDBS(idb) {
                if (is_vip(idb->hwptr) &&
		     idb->crypto_map && 
                     !strcmp(idb->crypto_map, head->map_tag)) {
                     reg_invoke_crypto_vip_assign_map_to_int(
                                                    idb->hwptr, head);
                }
            }
        }
    }

    mykey = crypto_mykey_get_by_slot(crypto_blk_hdr->req_id);

    if (crypto_blk_hdr->status != CRYPTO_HWEPA) {
        init_vip_dss_key(crypto_blk_hdr->req_id, mykey);
    } else {
        get_epa_status(crypto_blk_hdr->req_id, mykey);
    }
}

void crypto_engine_rsp_ipc_init (void)
{
    ipc_error_code error;

    if (!crypto_engine_rsp_crnt_port)
        ipc_create_named_port(CRYPTO_ENGINE_RSP_CRNT_PORT, 
	&crypto_engine_rsp_crnt_port, IPC_PORT_OOB);

    error = ipc_set_receive_callback(crypto_engine_rsp_crnt_port, 0,
                                     NULL);
 
    if (error != IPC_OK) {
	if (crypto_engine_debug)
            buginf("\nCRYPTO_ENGINE: can not set port for %s\n", 
		   CRYPTO_ENGINE_RSP_CRNT_PORT);
    }

}

void crypto_config_epa (ushort slot,
                        uchar *passwd,
                        ushort passwd_size)
{
    epa_crypto_blk *blk;

    blk = malloc (sizeof(epa_crypto_blk));
    if (!blk) {
        return;
    }

    blk->header.command_id = CRYPTO_CONFIG_EPA;
    bcopy(passwd, blk->data, passwd_size);
    blk->header.data_size = passwd_size ;
    crypto_engine_req_remote_dspt (slot, (void *)blk);
}

void crypto_engine_show_card_status (ushort slot, boolean print_status)
{
    epa_crypto_blk *blk;
    ulong status;

    if (crypto_engine_tbl[slot].crypto_unit_type != CRYPTO_HWEPA) {
	if ( print_status )
            printf("%% There is no crypto card installed in slot %d", slot);
        return;
    }

    rsp_parser_ready_boolean = create_watched_boolean("PARSER_READY", 0);

    blk = malloc (sizeof(epa_crypto_blk));
    if (!blk) {
        return;
    }

    blk->header.command_id = CRYPTO_GET_EPA_STATUS;
    blk->header.data_size = 0;

    rsp_parser_waiting = TRUE;

    process_watch_boolean(rsp_parser_ready_boolean, ENABLE, ONE_SHOT);
    process_set_boolean(rsp_parser_ready_boolean, FALSE);

    crypto_engine_req_remote_dspt (slot, (void *)blk);

    /* Wait for the response to be sendback. */
    process_wait_for_event();

    /* Assume the status has been updated */
    status = crypto_engine_tbl[slot].epa_status;
    if ( print_status ) {
        printf("\nCrypto card in slot: %d\n", slot);
        printf("\nTampered:        %s",  EPA_TAMPERED(status)? "Yes":"No");
        printf("\nXtracted:        %s",  EPA_XTRACTED(status)? "Yes":"No");
        printf("\nPassword set:    %s",  EPA_PASSWD_SET(status)? "Yes":"No");
        printf("\nDSS Key set:     %s",  EPA_DSS_SET(status)? "Yes":"No");
    }
    delete_watched_boolean(&rsp_parser_ready_boolean);
}

void crypto_card_clear_latch (ushort slot)
{
    epa_crypto_blk *blk;
    uchar *password_buffer;
    boolean password_status;
    crypto_config_epa_data_blk *data_blk;

    if (crypto_engine_tbl[slot].crypto_unit_type != CRYPTO_HWEPA) {
        printf("\nThere is no crypto card installed in this slot.");
        return;
    }

    if (!EPA_XTRACTED(crypto_engine_tbl[slot].epa_status)) {
        printf("\n The extraction latch on crypto card in slot %d is not on.", 
               slot);
        return;
    }

    password_buffer = malloc(CRYPTO_EPA_PASSWD_MAX_SIZE);
    if (!password_buffer ) {
        printf(nomemory);
        return;
    }
    printf("%% Enter the crypto card password.");

    password_status = askstring(stdio, password_buffer, 
                   CRYPTO_EPA_PASSWD_MAX_SIZE,"Password: ",
		   30 * ONESEC, 3, TRUE);
    if (!password_status) { /* error during entry */
        printf("%% Aborted.\n");
        free(password_buffer);
        return;
    }

    blk = malloc (sizeof(epa_crypto_blk));
    if (!blk) {
        return;
    }

    blk->header.command_id = CRYPTO_CLEAR_LATCH;
    data_blk = (crypto_config_epa_data_blk *)blk->data;
    bcopy(password_buffer, data_blk->passwd, strlen(password_buffer));
    data_blk->passwd_size = strlen(password_buffer);
    blk->header.data_size = sizeof(crypto_config_epa_data_blk); 

    rsp_parser_waiting = TRUE;

    rsp_parser_ready_boolean = create_watched_boolean("PARSER_READY", 0);
    process_watch_boolean(rsp_parser_ready_boolean, ENABLE, ONE_SHOT);
    process_set_boolean(rsp_parser_ready_boolean, FALSE);

    crypto_engine_req_remote_dspt (slot, (void *)blk);

    /* Wait for the response to be sendback. */
    process_wait_for_event();

    delete_watched_boolean(&rsp_parser_ready_boolean);
}

boolean crypto_remove_dfs (idbtype *idb)
{
    hwidbtype *hwidb = idb->hwptr;
    crypto_map_headtype *head;
    uchar *tmp_ptr;

    /* Confirm from user */
    printf("Warning! Remove distributed switching will disenable encryption"
           "on this interface.");
    if (!yes_or_no("Do you want to continue", FALSE, FALSE)) {
        return FALSE;
    }
   
    /* If confimred, remove the crypto map from the interface */
    head = find_crypto_map_head(idb->crypto_map);
    if (head) {
        tmp_ptr = idb->crypto_map; 
        idb->crypto_map = NULL;
        free(tmp_ptr);
        hwidb->ip_fast_flags &= ~IP_FAST_CRYPTO_MAP;
        
        /* 
         * need to check out why crypto map code removes all
         * the ip_fast_flags.
         */
        reg_invoke_crypto_engine_restore_media_vectors(hwidb, head);    
    }

    return TRUE;
}


