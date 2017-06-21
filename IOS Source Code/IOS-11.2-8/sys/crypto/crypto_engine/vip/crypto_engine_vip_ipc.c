/* $Id: crypto_engine_vip_ipc.c,v 1.1.50.12 1996/09/15 20:19:38 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/vip/crypto_engine_vip_ipc.c,v $
 *------------------------------------------------------------------
 * crypto engine vip ipc requests.
 *
 * Jan 1996, xliu 
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_engine_vip_ipc.c,v $
 * Revision 1.1.50.12  1996/09/15  20:19:38  xliu
 * CSCdi68755:  crypto maps not needed by vip2 should not be donwload to
 * vip2.
 * Branch: California_branch
 * Enforce the crypto map download as a complete transaction which inlcude
 * downloading the head, the map and peupe pair.
 *
 * Revision 1.1.50.11  1996/08/18  17:35:29  xliu
 * CSCdi66415:  Use hw_if_index instead of mic_index, following VIP2 code
 * change
 * Branch: California_branch
 * When sending IPC message from RSP to VIP2, use hw_if_index to identify
 * the
 * interface on vip2.
 *
 * Revision 1.1.50.10  1996/08/15  19:16:47  xliu
 * CSCdi65915:  crypto clear-latch does not work
 * Branch: California_branch
 * Check for EPA_XTRACT status bit.
 *
 * Revision 1.1.50.9  1996/08/05  07:03:47  xliu
 * CSCdi62337:  Can not remove a single crypto map sequence from one
 * interface
 * Branch: California_branch
 *
 * Revision 1.1.50.8  1996/07/28  11:17:17  xliu
 * CSCdi64306:  crypto engine memory aligment error on vip2
 * Branch: California_branch
 *
 * Revision 1.1.50.7  1996/07/15  09:59:26  xliu
 * CSCdi61651:  epa randome number seed is not initialized
 * Branch: California_branch
 *
 * Revision 1.1.50.6  1996/07/03  05:55:09  snijsure
 * CSCdi60711:  sh cr conn eng act does not show anything in vip2 crypto
 * router
 * Branch: California_branch
 *
 * Revision 1.1.50.5  1996/07/01  12:26:57  xliu
 * CSCdi61743:  decryption does not use cached pak pointer correctly.
 * Branch: California_branch
 *
 * Revision 1.1.50.4  1996/05/28  16:00:12  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 1.1.50.3  1996/05/21  09:44:42  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 1.1.50.2  1996/05/12  23:13:37  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 1.1.50.1  1996/04/19  14:57:18  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.6  1996/04/17  23:48:33  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 1.1.2.5  1996/04/09  20:03:28  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 1.1.2.4  1996/03/30  05:12:42  xliu
 * Branch: IosSec_branch
 * Commit before CA repository split.
 *
 * Revision 1.1.2.3  1996/03/25  23:19:39  xliu
 * Branch: IosSec_branch
 * Checkin VIP crypto engine code.
 *
 * Revision 1.1.2.2  1996/01/30  22:36:20  xliu
 * Branch: IosSec_branch
 * Checking in encryption RSP/VIP support code.
 *
 * Revision 1.1.2.1  1996/01/09  02:26:57  xliu
 * Branch: IosSec_branch
 * VIP software encryption.
 *
 * Revision 1.1  1996/01/09  02:11:59  xliu
 * Placeholder file.
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
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "logger.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "sched.h"
#include "access.h"
#include "../ip/ip.h"
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../ipc/ipc_private.h"
#include "../ipmulticast/igmp.h"
#include "../ip/ipaccess.h"
#include "../ip/ipaccount.h"
#include "../if/tring.h"
#include "../if/network.h"

#include "../src-vip/dfs_vip_idb.h"
#include "../src-vip/vip_debug.h"

#include "../ip/ipfast_index.h"
#include "../ip/ipfast_vip.h"
#include "../ip/ipfast_vip_ipc.h"
#include "../ip/ipfast_ipc.h"

#include "sys_registry.h"

#include "../os/chunk.h"

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
#include "../crypto/crypto.h"
#include "../crypto/crypto_engine/crypto_lib/lib_toolkit.h"
#include "../crypto/crypto_engine_api.h"
#include "../crypto/crypto_engine/core/crypto_engine.h"
#include "../crypto/crypto_engine/core/crypto_swepa.h"
#include "../crypto/crypto_engine/vip/crypto_engine_vip.h"
#include "../crypto/crypto_engine/rpsp/crypto_engine_rsp_ipc.h"

static void assign_crypto_vectors_to_idb(hwidbtype*, dfsidbtype *);
static void restore_media_vectors(hwidbtype *, dfsidbtype*);
static epa_vip_connection_tbl transfer_data[CRYPTO_CONNTBL_SIZE];

/*
 * assign_crypto_vectos_to_idb
 *
 * Called  to assign crypto_pak_dfs to pascb->crypto_pak_dfs, assign
 * crypto_engine_vip_fastsend to idb->fastsend. 
 */
static void assign_crypto_vectors_to_idb (hwidbtype *idb, dfsidbtype* idb_dfs)
{
    pas_cbtype *pascb=NULL;
    leveltype level;
    void* *crypto_vectors;

    pascb = INSTANCE(idb);

    idb->status |= IDB_ENCRYPT;
    if (crypto_engine_vip_debug) {
        buginf("\nCRYPTO_ENGINE_VIP: find idb to replace vector: %x\n",idb);
    }

    crypto_vectors = chunk_malloc(crypto_vector_chunk);
    if (!crypto_vectors) {
       if (crypto_engine_vip_debug) {
           buginf("CRYPTO_ENGINE_VIP: can not malloc.\n");
       }
       return;
    }
    memset(crypto_vectors, 0x00, sizeof(crypto_vector_t));

    level = raise_interrupt_level(NETS_DISABLE);
    idb->crypto_vectors = crypto_vectors;
    if (!((crypto_vector_t*)idb->crypto_vectors)->media_fastsend) {
        ((crypto_vector_t *)idb->crypto_vectors)->media_fastsend = 
                                                       idb->fastsend;
        idb->fastsend = crypto_engine_vip_fastsend;
        idb_dfs->local_sw_vector = crypto_engine_vip_fastsend;
       
        ((crypto_vector_t *)idb->crypto_vectors)->media_pak_dfs =
                                                  pascb->pak_dfs;
        pascb->pak_dfs = crypto_engine_vip_pak_dfs;

        if (crypto_engine_vip_debug) {
            buginf("CRYPTO_VIP: replaced fastsend: %x by new fastsend: %x"
                   " pak_dfs %x by crypto_pak_dfs %x\n", 
                   ((crypto_vector_t *)idb->crypto_vectors)->media_fastsend, 
                   idb->fastsend, 
                   ((crypto_vector_t *)idb->crypto_vectors)->media_pak_dfs, 
                   crypto_engine_vip_pak_dfs);
        }
    }
    reset_interrupt_level(level);
}

static void restore_media_vectors (hwidbtype *idb, dfsidbtype* idb_dfs)
{
    crypto_vector_t *vectors;
    pas_cbtype *pascb=NULL;
    leveltype level;

    if (crypto_engine_vip_debug) {
        buginf("\nCRYPOT_ENGINE: restore media vectors to %x", idb);
    }

    if (!idb) {
        return;
    }

    vectors = (crypto_vector_t *)idb->crypto_vectors;
    if (!vectors) {
        return;
    }

    if(!vectors->media_fastsend || !vectors->media_pak_dfs) {
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: null media vectors.");
        }
        return;
    }

    if (crypto_engine_vip_debug) {
        buginf("\nCRYPTO_ENGINE: restore media vectors: %x, %x",
                 vectors->media_fastsend, vectors->media_pak_dfs);
    }

    level = raise_interrupt_level(NETS_DISABLE);
    pascb = INSTANCE(idb); 
    idb->fastsend = vectors->media_fastsend;
    idb_dfs->local_sw_vector = vectors->media_fastsend; 
    pascb->pak_dfs = vectors->media_pak_dfs; 
    chunk_free(crypto_vector_chunk, vectors);            
    idb->crypto_vectors = NULL;
    reset_interrupt_level(level);
}

/*
 * copy_conn_tbl
 *
 * Copy connection table to the given memory. Currently we have different
 * connection tables maintained for swepa and hwepa. It will be merged
 * into one table later.
 * Here the input size must be greater or equal to the size of connection
 * table.
 */
static void copy_conn_tbl(epa_vip_connection_tbl *tbl, ushort *count)
{
    ushort i, j;

    if (crypto_engine_vip_config.epa_type != CRYPTO_HWEPA) {
        for ( j= 0 , i = 0 ; i < CRYPTO_CONNTBL_SIZE; i++ ) {
            if ( epa_swcrypto_conn_tbl[i].state == EPA_SWCRYPTO_CONN_UNSET )
                continue;
            tbl[j].connection_id = epa_swcrypto_conn_tbl[i].connection_id;
            tbl[j].alg_type = epa_swcrypto_conn_tbl[i].alg_type;
            tbl[j].state = epa_swcrypto_conn_tbl[i].state;
            tbl[j].encrypt_count = epa_swcrypto_conn_tbl[i].encrypt_count;

            tbl[j].decrypt_count = epa_swcrypto_conn_tbl[i].decrypt_count;
            if ( epa_swcrypto_conn_tbl[i].swidb )
                tbl[j].if_index =
                       epa_swcrypto_conn_tbl[i].swidb->hwptr->hw_if_index;
            j++;
        }
    } else {
        for ( j= 0 , i = 0 ; i < CRYPTO_CONNTBL_SIZE; i++ ) {
            if ( crypto_vip_conntbl[i].state ==  CRYPTO_EPA_SESSION_UNSET )
                continue;
            tbl[j].connection_id = crypto_vip_conntbl[i].connection_id;
            tbl[j].alg_type = crypto_vip_conntbl[i].encrypt_method;
            tbl[j].state = EPA_SWCRYPTO_CONN_SET; 
            tbl[j].encrypt_count = crypto_vip_conntbl[i].encrypt_count;
            tbl[j].decrypt_count = crypto_vip_conntbl[i].decrypt_count;
            if ( crypto_vip_conntbl[i].hwidb )
                tbl[j].if_index = crypto_vip_conntbl[i].hwidb->hw_if_index;
            j++;
        }
    }

    *count = j;
}

/*
 * This is the routine that  does the bulk transfer of
 * connection table data from VIP to RSP
 */
void crypto_engine_vip_send_conn_tbl (void)
{
    ushort count;
    ushort nbytes;
    ipc_message *message;
    extern epa_swcrypto_connection epa_swcrypto_conn_tbl[];
    epa_crypto_blk_header  crypto_blk_hdr;


    copy_conn_tbl(transfer_data, &count);

    /* 
     * Each entry in epa_vip_connection_tbl is 24 bytes long 
     * and we have 'j' valid entries.
     */
    if (crypto_engine_vip_debug) 
        buginf("\nCRYPTO: There are %d valid connections on vip\n",count);

    nbytes = count * sizeof(epa_vip_connection_tbl);
    message = ipc_get_message(sizeof(epa_crypto_blk_header)+nbytes, 0, 0);

    if (!message && count > 0 ) {
	/* Try and get simple message size, data is too large? */
        if (crypto_engine_vip_debug) {
             buginf("\nCRYPTO: Could not get data buffer of size %d\n",nbytes);
        }
        message = ipc_get_message(sizeof(epa_crypto_blk_header), 0, 0);
	nbytes=0;
    }
    if ( !message ) {
	/* 
	 * Oh well we can't get enough space even to send "fail"  message
	 * to vip.
	 */
        if (crypto_engine_vip_debug) {
             buginf("\nCRYPTO: Could not get even the ipc buffer \n");
        }
        return;
    }

    crypto_blk_hdr.command_id =  CRYPTO_ENGINE_VIP_SEND_TBL;
    crypto_blk_hdr.req_id = vip_logical_slot();
    crypto_blk_hdr.data_size = count; 

    if (crypto_engine_vip_debug)
            buginf("\nGot IPC buffer %d long \n",nbytes);

    bcopy(&crypto_blk_hdr, message->header->data, 
          sizeof(epa_crypto_blk_header));

    /* 
     * Copy the actual connection table if we found any legit entries
     */

    if ( nbytes > 0 )
       bcopy(transfer_data,message->header->data+sizeof(epa_crypto_blk_header),
           nbytes);
    ipc_send_message_permanent_port(message, IPC_TYPE_CRYPTO);
}

/*
 * crypto_engine_vip_upd_map
 *
 * Update the crypto map. 
 */

void crypto_engine_vip_upd_map (uchar *msg)
{
    crypto_vip_fast_hdr *new_msg = (crypto_vip_fast_hdr *)msg;
    crypto_vip_map_tag_msg *head_msg = NULL;
    crypto_vip_upd_map_msg *map_msg;
    crypto_map_headtype *head = NULL;
    crypto_maptype *map = NULL;
    leveltype level;

    if ((new_msg->target != vip_logical_slot()) && (new_msg->target != -1)) {
	/*
	 * We received a message that is not intended
	 * intended for us and is not broadcast- ignore it.
	 */
	return;
    }

    switch (new_msg->command) {
    case CRYPTO_VIP_ADD_MAP_HEAD:
        head_msg = (crypto_vip_map_tag_msg *)
                   (msg + sizeof(ipc_ipfast_msg_hdr) + sizeof(int));

        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: add map head : %s", head_msg->tag);
        }

        head = vip_find_crypto_map_head(head_msg->tag, head_msg->tag_size);
        if (head) {
            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE: crypto map head already exists.");
            }
            return;
        }

        head = crypto_map_head_add(head_msg->tag, head_msg->tag_size);
        if (!head) {
            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE: can not add map head.");
            } 
        }
        break;

    case CRYPTO_VIP_ADD_MAP:
        map_msg = (crypto_vip_upd_map_msg *)(msg + 
			sizeof(ipc_ipfast_msg_hdr) + sizeof(int));

        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: add map with seq %d", map_msg->seq_no);
        }

        head = vip_find_crypto_map_head(map_msg->tag, map_msg->tag_size);
        if (!head) {
            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE: can not find head.");
            }
            return;
        }

        /* Check if the map exists */
        map = find_crypto_map(head, map_msg->seq_no);
        if (map) {
            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE: map exists.");
            }
            return;
        }
 
        map = crypto_map_add(head, map_msg->seq_no);
        if (!map) {
            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE: can not add map.");
            }
            return;
        }
	map->mode_flags = map_msg->mode_flags;
        break;

    case CRYPTO_VIP_DEL_MAP:
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: delete map");
        }

        map_msg = (crypto_vip_upd_map_msg *)(msg + 
			sizeof(ipc_ipfast_msg_hdr) + sizeof(int));
        head = vip_find_crypto_map_head(map_msg->tag, map_msg->tag_size);
        if (!head) {
            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE: can not delete from null sequence.");
            }
            return;
        }

        map = find_crypto_map(head, map_msg->seq_no);
        if (!map) {
            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE: can not find map.");
            }
        }

        /*
         * Delete the crypto map .
         */
        if (!crypto_map_delete(head, map)) {
            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE: failed to delete map.");
            }
        }
        break;

    case CRYPTO_VIP_DEL_MAP_HEAD:
        {
        dfsidbtype *hwidb_dfs;
        leveltype level;
        hwidbtype *hwidb;
        
        head_msg = (crypto_vip_map_tag_msg *)
                   (msg + sizeof(ipc_ipfast_msg_hdr) + sizeof(int));

        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: del map head : %s", head_msg->tag);
        }

        head = vip_find_crypto_map_head(head_msg->tag, head_msg->tag_size);
        if (!head) {
            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE: can not find the crypto map head.");
            }
            return;
        }

        /* 
         * Restore the media vectors on all the idb which has the map
         * removed.
         */
        level = raise_interrupt_level(NETS_DISABLE);
        FOR_ALL_HWIDBS(hwidb) {
            if ((hwidb->status & IDB_ENCRYPT) &&
		!strncmp(hwidb->firstsw->crypto_map, head_msg->tag,
                         			head_msg->tag_size)) {
                hwidb_dfs = dfs_vip_get_hwidb(hwidb->hw_if_index);
                if (!hwidb_dfs) {
                    break;
                }

                restore_media_vectors(hwidb, hwidb_dfs);
                chunk_free(crypto_map_tag_chunk, hwidb->firstsw->crypto_map);
                hwidb->firstsw->crypto_map = NULL;
            }
        }
        reset_interrupt_level(level);

        /* Remove the head, as well as all the map sequences */
        crypto_map_head_delete(head_msg->tag, head_msg->tag_size);

        }
        break;

    case CRYPTO_VIP_ASSIGN_MAP:
        {
        crypto_vip_assign_map_to_int_msg *int_msg;
        hwidbtype *idb;
        crypto_maptype *map = NULL;
        dfsidbtype *hwidb_dfs;

        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: assign map to int");
        }
        int_msg = (crypto_vip_assign_map_to_int_msg *)(msg + 
                          sizeof(ipc_ipfast_msg_hdr) + sizeof(int));
        hwidb_dfs = dfs_vip_get_hwidb(int_msg->hw_if_index);
        if (!hwidb_dfs) {
            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE_VIP: null hwidb_dfs for index: %d", 
                        int_msg->hw_if_index);
            }
            return;
        }

        /* This pointer should never be NULL */
        idb = hwidb_dfs->hwidb;
	if (!idb) {
            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE_VIP: null hwidb for hwidb_dfs: %x", 
                        			hwidb_dfs);
            }
            return;
	}

        if (idb->firstsw && !idb->firstsw->crypto_map) {
            idb->firstsw->crypto_map = chunk_malloc(crypto_map_tag_chunk);
	    if (!idb->firstsw->crypto_map) {
		if (crypto_engine_vip_debug) {
		    buginf("\nCRYPTO_ENGINE: can not allocate mem for map.");
		}
		return;
	    }
        }
        bcopy(int_msg->map_tag, idb->firstsw->crypto_map, int_msg->tag_size);
        idb->firstsw->crypto_map[int_msg->tag_size] = '\0';

        head = vip_find_crypto_map_head(int_msg->map_tag,int_msg->tag_size); 
        if (!head) {
            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE: can not find crypto map head.");
            }
            return;
        }

        map = head->top;
        for (; map; map = map->next) map->hwidb = idb;

        if (!idb->crypto_vectors)
            assign_crypto_vectors_to_idb(idb, hwidb_dfs);
        }
        break;
    
    case CRYPTO_VIP_REMOVE_MAP:
        {
        crypto_vip_assign_map_to_int_msg *int_msg;
        hwidbtype *idb;
        leveltype level;
        crypto_map_headtype *head;
        dfsidbtype *hwidb_dfs;
        boolean find = FALSE;

        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: remove map from int");
        }
        int_msg = (crypto_vip_assign_map_to_int_msg *)(msg + 
                            sizeof(ipc_ipfast_msg_hdr) + sizeof(int));

        hwidb_dfs = dfs_vip_get_hwidb(int_msg->hw_if_index);
        if (!hwidb_dfs) {
            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE_VIP: null hwidb_dfs for index: %d", 
                        int_msg->hw_if_index);
            }
            return;
        }

        head = vip_find_crypto_map_head(int_msg->map_tag,int_msg->tag_size); 
        if (!head) {
            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE: can not find crypto map head.");
            }
            return;
        }

        level = raise_interrupt_level(NETS_DISABLE);
        /* This pointer should never be NULL */
        idb = hwidb_dfs->hwidb;
	if (!idb) {
            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE_VIP: null hwidb for hwidb_dfs: %x", 
                        			hwidb_dfs);
            }
            return;
	}
        restore_media_vectors(idb, hwidb_dfs);
        chunk_free(crypto_map_tag_chunk, idb->firstsw->crypto_map);
        idb->firstsw->crypto_map = NULL;
        reset_interrupt_level(level);

        /* 
         * If there are no interface reference this crypto map, 
         * free the memory used by the head and the associated map.
         */
        FOR_ALL_HWIDBS(idb) {
            if ((idb->status & IDB_ENCRYPT) &&
		!strncmp(idb->firstsw->crypto_map, head_msg->tag,
                         		int_msg->tag_size)) {
                find = TRUE;
                break;
            }
        }

        if (!find) {
            crypto_map_head_delete(int_msg->map_tag, int_msg->tag_size);
        }

        }
        break;

    case CRYPTO_VIP_MAP_SET_PEUPE:
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: set pe/upe.");
        }
        map_msg = (crypto_vip_upd_map_msg *)(msg + 
			sizeof(ipc_ipfast_msg_hdr) + sizeof(int));
        head = vip_find_crypto_map_head(map_msg->tag, map_msg->tag_size);
        if (!head) {
            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE: can not find head.");
            }
            return;
        }
        map = find_crypto_map(head, map_msg->seq_no);
        if (!map) {
            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE: can not find map.");
            }
            return;
        }
        map->pe.ip_addr = map_msg->pe_addr;
        map->upe.ip_addr = map_msg->upe_addr;
        map->acl_list = acl_array[map_msg->acl_number];
        map->acl_list_number = map_msg->acl_number;

        if (crypto_engine_vip_debug) {
            buginf("\nassign pe.ip_addr: %x upe.ip_addr: %x, ",
                 map->pe.ip_addr, map->upe.ip_addr);
	    if (map->acl_list) {
	    	buginf("acl: %s, list :%x", 
			map->acl_list->name, map->acl_list->list);
	    } else {
		buginf("acl not yet sent to VIP");
	    }
        }

        break;

    case CRYPTO_VIP_MAP_SET_MODE:
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: set mode.");
        }
        map_msg = (crypto_vip_upd_map_msg *)(msg + 
			sizeof(ipc_ipfast_msg_hdr) + sizeof(int));
        head = vip_find_crypto_map_head(map_msg->tag, map_msg->tag_size);
        if (!head) {
            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE: can not find head.");
            }
            return;
        }
        map = find_crypto_map(head, map_msg->seq_no);
        if (!map) {
            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE: can not find map.");
            }
            return;
        }
        map->mode_flags = map_msg->mode_flags;

        break;

    case CRYPTO_VIP_UPD_CONN_ID:
        {
        crypto_vip_map_upd_conn_msg *conn_msg = 
           (crypto_vip_map_upd_conn_msg *)(msg + 
			sizeof(ipc_ipfast_msg_hdr) + sizeof(int));

        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: replace conn %d with %d for map %s", 
                   conn_msg->old_conn_id, conn_msg->new_conn_id,
                   conn_msg->map_tag);
        }
        head = vip_find_crypto_map_head(conn_msg->map_tag, conn_msg->tag_size);
        if (!head) {
            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE: can not find map head.");
            }
            return;
        }
        map = head->top;
        
        level = raise_interrupt_level(NETS_DISABLE);
        for (; map; map = map->next) 
            if (map->seq_no == conn_msg->seq_no) {
                map->conn_id = conn_msg->new_conn_id; 
            }
        reset_interrupt_level(level);
        }
        break;

    case CRYPTO_VIP_GET_CONN_TBL:
	{
        crypto_vip_get_conn_tbl_msg *conn_msg = 
          		(crypto_vip_get_conn_tbl_msg *)(msg + 
			sizeof(ipc_ipfast_msg_hdr) + sizeof(int));
        if ( vip_logical_slot() != conn_msg->crypto_engine_id) {
             if (crypto_engine_vip_debug) 
                buginf("\nCRYPTO_ENGINE: my logical slot = %d, message for %d, not serviced.\n", 
		   vip_logical_slot(), conn_msg->crypto_engine_id);
	    return;
	}
        if (crypto_engine_vip_debug) 
             buginf("\nCRYPTO_ENGINE: get vip conn tbl from crypto engine # %d.", conn_msg->crypto_engine_id);
         crypto_engine_vip_send_conn_tbl(); 
		/* Send the ACK back to RSP */
	}
        break;
    default:
        if (crypto_engine_vip_debug) {
           buginf("\nCRYPTO_ENGINE: invalid map command %d.", new_msg->command);
        }
        break;
    }
}
