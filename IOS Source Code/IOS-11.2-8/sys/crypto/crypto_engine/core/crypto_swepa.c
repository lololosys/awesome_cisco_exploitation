/* $Id: crypto_swepa.c,v 1.1.68.18 1996/09/13 18:09:32 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/core/crypto_swepa.c,v $
 *------------------------------------------------------------------
 * crypto_swepa.c
 * 
 * This file defines the software encryption and key mgt processes.
 *
 * 11, 1995, xliu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_swepa.c,v $
 * Revision 1.1.68.18  1996/09/13  18:09:32  xliu
 * CSCdi68871:  Encryption process stops processing packets
 * Branch: California_branch
 * Empty queue is not correctly detected.
 *
 * Revision 1.1.68.17  1996/09/11  22:33:00  sbelair
 * CSCdi68871:  Encryption process stops processing packets
 * Branch: California_branch
 *
 * When the crypto engine encryption queue was getting full, it was getting
 * too full- makw sure only the queue size amount of elements get put on
 * the queue. Also, when the last element gets added to the queue, don't
 * increment top (in crypto_encrypt_enqueue), because that will make the
 * queue look empty in crypto_encrypt_deqeueue and no elements will be
 * dequeued.
 *
 * Revision 1.1.68.16  1996/09/10  14:14:19  sbelair
 * CSCdi67685:  Unable to fast-switch packets through encrypted tunnels
 * Branch: California_branch
 *
 * Implement new crypto vector for tunnel fast-switching (decryption
 * only),
 * and initialize it to crypto_engine_rp_tunnel_fs() for non-rsp
 * platforms, and crypto_engine_rsp_tunnel_fs() for rsp platforms. When
 * these routines are invoked, they will set the new switch mode
 * CRYPTO_ENGINE_TUNNLE_FASTS_SWITCH.  After decryption, this switch mode
 * will call crypto_decrypt_tunnel_fs_output which will set the
 * cryptoflags
 * appropriately and try to resume fast-switching the now decrypted packet
 * through the tunnel. If that fails it will enqueue the packet for
 * process-
 * level.
 *
 * Revision 1.1.68.15  1996/09/06  23:57:35  bew
 * CSCdi62313:  Sourced fragments dont encrypt properly
 * Branch: California_branch
 * Add support for fragmented packets: always reassemble fragments
 * before either encryption or decryption.
 *
 * Revision 1.1.68.14  1996/08/27  21:15:34  sbelair
 * CSCdi60324:  CPU hogs on 4000 (Encryption Process)
 * Branch: California_branch
 * Redo this fix to this problem- just use the time allotted to every
 * process when it runs, instead of a full second.  Thanks to Dave
 * Katz for pointing this out.
 *
 * Revision 1.1.68.13  1996/08/27  06:32:34  xliu
 * CSCdi67270:  crypto parser command clean up
 * Branch: California_branch
 * Clean up zeroize command.
 *
 * Revision 1.1.68.12  1996/08/09  23:37:48  sbelair
 * CSCdi60324:  CPU hogs on 4000 (Encryption Process)
 * Branch: California_branch
 * In crypto_encdec_packet(), give up the cpu after ONESEC, rather than
 * processing a set amount of packets.
 *
 * Revision 1.1.68.11  1996/07/17  09:32:16  xliu
 * CSCdi63107:  Show crypto conn eng stat crashes router
 * Branch: California_branch
 *
 * Revision 1.1.68.10  1996/07/08  07:47:48  xliu
 * CSCdi62016:  Software EPA does not timeout connection table
 * Branch: California_branch
 *
 * Revision 1.1.68.9  1996/07/06  19:45:01  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 1.1.68.8  1996/07/01  12:26:24  xliu
 * CSCdi61743:  decryption does not use cached pak pointer correctly.
 * Branch: California_branch
 *
 * Revision 1.1.68.7  1996/06/27  16:34:44  hampton
 * Keep your mitts off the scheduler private data structures.  [CSCdi61518]
 * Branch: California_branch
 *
 * Revision 1.1.68.6  1996/06/23  01:07:14  snijsure
 * CSCdi61118:  minor crypto cleanup
 * Branch: California_branch
 *
 * Revision 1.1.68.5  1996/06/21  22:30:01  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 1.1.68.4  1996/05/28  15:59:18  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 1.1.68.3  1996/05/12  23:12:47  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 1.1.68.2  1996/05/07  23:51:50  carrel
 * CSCdi56978:  Crypto Maps wont compile with named access lists
 * Branch: California_branch
 * Updated to use named access lists
 *
 * Revision 1.1.68.1  1996/04/19  14:56:36  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.19  1996/04/17  23:48:01  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 1.1.2.18  1996/04/16  21:53:34  bew
 * Branch: IosSec_branch
 * General code cleanup.
 *
 * Revision 1.1.2.17  1996/04/09  20:03:02  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 1.1.2.16  1996/03/30  05:12:04  xliu
 * Branch: IosSec_branch
 * Commit before CA repository split.
 *
 * Revision 1.1.2.15  1996/03/21  10:07:19  xliu
 * Branch: IosSec_branch
 * Migrated to CA branch.
 *
 * Revision 1.1.2.14  1996/03/05  02:00:14  xliu
 * Branch: IosSec_branch
 * Add ios random generation code and checking of uninit vectors.
 *
 * Revision 1.1.2.13  1996/03/01  01:39:31  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 1.1.2.12  1996/02/13  19:16:29  carrel
 * Branch: IosSec_branch
 * Crypto Map checkin (finally)
 *
 * Revision 1.1.2.11  1996/02/05  03:22:23  xliu
 * Branch: IosSec_branch
 * Add DH code; Add crypto_engine_encrypt/decrypt vectors.
 *
 * Revision 1.1.2.10  1996/01/30  22:35:59  xliu
 * Branch: IosSec_branch
 * Checking in encryption RSP/VIP support code.
 *
 * Revision 1.1.2.9  1996/01/24  22:56:03  bew
 * Branch: IosSec_branch
 * Sync-related changes to sys/crypto.
 *
 * Revision 1.1.2.8  1996/01/19  07:00:50  xliu
 * Branch: IosSec_branch
 * Commit before the second sync.
 *
 * Revision 1.1.2.6  1996/01/04  18:35:44  xliu
 * Branch: IosSec_branch
 * After initial clean up.
 *
 * Revision 1.1.2.5  1995/12/20  23:31:46  bew
 * Branch: IosSec_branch
 * Context is now freed after a sign operation. Before the callback is
 * called a pointer to the pak is taken so it can be used after the
 * callback returns.
 *
 * Revision 1.1.2.4  1995/12/17  06:23:00  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 1.1.2.3  1995/12/10  09:27:16  xliu
 * Branch: IosSec_branch
 * Moved encryption from datagram_out to ipsendnet, after fragmentation.
 *
 * Revision 1.1.2.2  1995/12/02  17:42:46  xliu
 * Branch: IosSec_branch
 *
 * Modified epa driver interface.
 *
 * Revision 1.1.2.1  1995/11/30  04:59:47  xliu
 * Branch: IosSec_branch
 *
 * Moved the crypto engine core files into dir CORE.
 *
 * Revision 1.1  1995/11/30  04:20:52  xliu
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
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "ttysrv.h"
#include "sched.h"
#include "mgd_timers.h"

#include "../ip/ip.h"
#include "../ip/ipfast.h"
#include "../ip/ipfast_index.h"
#include "../ip/ipfast_ipc.h"
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../util/random.h"
#include "../os/chunk.h"

#include "../crypto/crypto_public.h"
#include "../crypto/crypto.h"
#include "../crypto/crypto_engine_api.h"
#include "../ui/debug.h"
#include "../crypto/crypto_debug.h"
#include "../crypto/crypto_registry.h"
#include "crypto_engine.h"
#include "../crypto/crypto_engine/crypto_lib/encrypt.h"
#include "../crypto/crypto_engine/crypto_lib/lib_toolkit.h"
#include "crypto_swepa.h"
#include "../crypto/crypto_engine/crypto_lib/dss.h"
#include "../crypto/crypto_pubkey.h"
#include "../crypto/crypto_engine/rpsp/crypto_engine_rsp_ipc.h"
#include "../crypto/crypto_engine/vip/connstup.h"
#include "crypto_iv.h"


/*
 * encrypt_que_ready:
 * This is a watched boolean. It is set to be TRUE when a packet is enqueued
 * to the input queue of the encryption process. It is set to FALSE when the 
 * input queue for the encryption process is empty.
 */

watched_boolean *encrypt_que_ready;

/*
 * Variables for the dropped packet chunk lists
 */
chunk_type *crypto_dropped_packets_chunks;
crypto_drop_pak_t *crypto_dropped_packets_list;

/*
 * global variables  
 *
 * Those variables are available on all the platforms, including the
 * crypto engine on vip. Later the variable names will be modified to
 * be more precise.
 */
epa_swcrypto_connection epa_swcrypto_conn_tbl[CRYPTO_CONNTBL_SIZE];
crypto_dh_pair          crypto_dh_pairs[CRYPTO_CONNTBL_SIZE];
uchar                   crypto_encrypt_pad[EPA_DES_ENCDEC_BLOCK_LENGTH];
uchar                   crypto_pri_key[SHA_LENGTH];
uchar                   crypto_pub_key[DSS_LENGTH_MIN];
uchar                   crypto_ran_num[CRYPTO_RANDOM_NUM_SIZE];
ulong                   crypto_release_counter;
crypto_engine_encrypt_proc   crypto_engine_encrypt;
crypto_engine_decrypt_proc   crypto_engine_decrypt;
crypto_engine_reassemble_proc crypto_engine_reassemble;
void*                        ip_les_fastswitch;
mgd_timer                    crypto_engine_master_timer;

int crypto_engine_key_proc_yield (int);
YIELD_context           crypto_yield_handle;

/*
 * function prototyping 
 */
static inline void crypto_encdec_packet(crypto_engine_idb_t *);
static inline void crypto_key_process_req(void);
boolean copy_cert_to_buffer(paktype *, uchar *, uchar *, ushort);

/*
 * crypto_key_enqueue
 *
 * Enqueue a request to the key request queue. This will only be called
 * at process level.
 */
epa_status crypto_key_enqueue (crypto_engine_idb_t *idb, void *blk)
{
    process_enqueue(idb->expq, blk);
    return EPA_OP_OK;
}

/*
 * crypto_drop_packet
 *
 * Return the pak to the sys buffer. This is called by encryption process
 * when the input queue is full, or when the connection has not been set.
 * Record the number of dropped packets, if possible.
 */
void crypto_drop_packet (crypto_encrypt_header *header) 
{
    crypto_pak_info *pak_info = &header->pak_info;
    boolean found = FALSE;
    crypto_drop_pak_t *list_element, *new_element;
    leveltype level;

    /*
     * See if a chunk has already been allocated for this hwidb.
     */
    list_element = crypto_dropped_packets_list;
    while (list_element) {
	if (list_element->hwidb == pak_info->hwidb) {
	    found = TRUE;
	    break;
	}
	list_element = list_element->next;
    }

    /*
     * If hwidb found, update the count. Otherwise allocate a new chunk and
     * add it to the head of the list.
     */
    if (found) {
	list_element->count++;
    } else {
   	if ((new_element=chunk_malloc(crypto_dropped_packets_chunks)) != NULL) {
	    new_element->count = 1; 
	    new_element->hwidb = pak_info->hwidb;
	    level = raise_interrupt_level(ALL_DISABLE);
	    new_element->next = crypto_dropped_packets_list;
	    crypto_dropped_packets_list = new_element;
    	    reset_interrupt_level(level); 
	}
    }

    datagram_done(pak_info->pak);
}

/*
 * crypto_key_dequeue
 *
 * Dequeue a request from the key request queue. This will only be called
 * at process level.
 */
inline void * crypto_key_dequeue (crypto_engine_idb_t *idb)
{
    return(process_dequeue(idb->expq));
}

/*
 * crypto_encrypt_enqueue
 *
 * This procedure enqueue the packet to the input queue of encryption
 * process. It assumes it is ether called at the interrupt level or
 * the interrupt is disabled. 
 */

inline void crypto_encrypt_enqueue (crypto_engine_idb_t *idb, 
		                    crypto_encrypt_header *header)
{

    /* 
     * First, check if the queue is overflow.
     */
    if (idb->encrypt_que_count >= EPA_ENCRYPT_QUE_SIZE - 1) {
        if (crypto_engine_debug) {
            buginf("\nCRYPTO_ENGINE: packet dropped since the queue is full.");
        }
        /* Drop the packet by returning the packet to the poll */
        crypto_drop_packet(header);
    } else {
        bcopy(header, &idb->encrypt_que[idb->encrypt_que_top],
              sizeof(crypto_encrypt_header));

        idb->encrypt_que_count++;

        if (idb->encrypt_que_top == EPA_ENCRYPT_QUE_SIZE -1 ) {
            idb->encrypt_que_top = 0;
        } else {
            /* point to the next available slot */
            idb->encrypt_que_top++; 
        }

	if (idb->encrypt_que_count == 1)
            process_set_boolean(encrypt_que_ready, TRUE);
    }
}

/*
 * crypto_encrypt_dequeue
 * 
 * This procedure is called to dequeue a packet from the encryption queue.
 * It is only called at process level since sw encryption is only done at 
 * process level.
 */

inline 
crypto_encrypt_header * crypto_encrypt_dequeue (crypto_engine_idb_t *idb)
{ 
    crypto_encrypt_header* header = NULL;
    leveltype level;

    /* 
     * Here we disable the interrupt to prevent the watched boolean
     * encrypt_que_ready be modified by the fastswitch. 
     */
    level = raise_interrupt_level(ALL_DISABLE);

    /* Check if the queue is already empty */
    if (idb->encrypt_que_top == idb->encrypt_que_bot) {
        process_set_boolean(encrypt_que_ready, FALSE);
        reset_interrupt_level(level); 
        return NULL;
    }
 
    header = &idb->encrypt_que[idb->encrypt_que_bot]; 

    idb->encrypt_que_count --;

    if (idb->encrypt_que_bot == EPA_ENCRYPT_QUE_SIZE - 1) {
        idb->encrypt_que_bot = 0;
    } else 
        idb->encrypt_que_bot ++; 

    reset_interrupt_level(level); 

    return header;
}

/*
 * crypto_encrypt_proc 
 *
 * The process which encrypts/decrypts the packets. 
 */
forktype crypto_encrypt_proc (void)
{
    ulong major, minor;
    mgd_timer *crypto_engine_expired_timer;

    encrypt_que_ready = create_watched_boolean("ENCRYPT BOOLEAN", 0);
    process_watch_boolean(encrypt_que_ready, ENABLE, RECURRING);

    /* Initialize the crypto engine master timer. */
    mgd_timer_init_parent(&crypto_engine_master_timer, NULL);
    process_watch_mgd_timer(&crypto_engine_master_timer, ENABLE);

    while (TRUE) {
        /*
	 * Set the global blocking disable flag to FALSE such that
         * the process will block on SWEPA_des_reqQ.
         */
        process_safe_wait_for_event();

        while (process_get_wakeup(&major, &minor)) {
            switch (major) {
	    case BOOLEAN_EVENT:
		/*
		 * Since the des_queue_ready is a flag which
		 * can be updated at the interrupt level after
		 * the des process drains the queue, we check again
		 * here of the flag to make sure the queue is drained 
		 * out before going back to sleep.
		 */
		while (process_get_boolean(encrypt_que_ready)) {
		    crypto_encdec_packet(&crypto_engine_idb);
		}
		break;

            case TIMER_EVENT:
                while (mgd_timer_expired(&crypto_engine_master_timer)) {
                    crypto_engine_expired_timer = mgd_timer_first_expired(
                                        &crypto_engine_master_timer);
                    reg_invoke_crypto_engine_key_timeout(
                                     crypto_engine_expired_timer);
                }
                break;
	    default:
		break;
            }
        }
    }
}

static void crypto_encdec_packet (crypto_engine_idb_t *idb)
{ 
    crypto_encrypt_header *header = NULL;
    short           conn_id;
    paktype	    *pak;
    iphdrtype	    *ip;
    int 	    crypto_offset;
    uchar 	    iv_seed[CRYPTO_MAX_IV_SIZE];
    ushort	    iv_seed_size;
    boolean	    send;

    while (TRUE) {

        if (process_time_exceeded()) {
	    blocking_disabled = FALSE;
            process_suspend();
	    blocking_disabled = TRUE;
            break;
        }

        /* Retrieve from the queue and process the request. */
        header = crypto_encrypt_dequeue(&crypto_engine_idb); 
        if (!header) {
            /* The queue has been drained out. Go back to scheduler. */
	    break;
        }
        conn_id = header->conn_id;

        /* Check the sanity of the selected session. */
        if (conn_id <= 0 || (epa_swcrypto_conn_tbl[conn_id].state != 
			    EPA_SWCRYPTO_CONN_SET)) {
            header->status = EPA_OP_FAIL;
            crypto_drop_packet(header);
            if (crypto_engine_debug) {
                buginf("\nCRYPTO_ENGINE: packets dropped: State = %d",
		        epa_swcrypto_conn_tbl[conn_id].state);
            }
            continue;
        }

    	/*
	 * Fragments must be reassembled before either encryption or decryption
	 * so that we are guarenteed to use the same IV and data values. If 
	 * the crypto_engine_reassemble routine returns NULL, the full packet 
	 * hasn't been found, so we ignore the pak. If there is no reassemble 
	 * vector and the packet is a fragment, then we have no choice but to 
	 * drop the packet.
     	 */
	pak = header->pak_info.pak;
	ip = (iphdrtype *)ipheadstart(pak);
	if ((get_ip_morefragments(ip)) || (getfo(ip) != 0)) {
	    if (crypto_engine_reassemble) {
		/* 
		 * The reassemble function will change the value of pak to 
		 * either NULL indicating that reassembly is not complete, or 
		 * to a new pak which contains the entire reassembled packet.
		 */
		pak = (*crypto_engine_reassemble)(pak);
		if (pak == NULL) {
		    continue;
		}
	
		/*
		 * Now we have a reassembled pak. The IV and data values in 
		 * header need to be re-calculated to correspond with the new 
		 * pak.
		 *
		 * Note that the reassembled packet is forced to process switch.
		 * It is very possible that fragmentation will be necessary 
		 * before it can be sent out, and it is necessary to do the 
		 * fragmentation at process switch.
		 *
		 * Have to re-compute ip for the new pak! 
		 */
		ip = (iphdrtype *)ipheadstart(pak);
		header->pak_info.pak = pak;
		header->pak_info.ip = ip;
		crypto_set_encrypt_start(ip, &crypto_offset);
		crypto_get_iv_seed(ip, iv_seed, &iv_seed_size);
		header->data_size = ip->tl - crypto_offset;
		header->data = ipheadstart(pak) + crypto_offset;
		header->iv_seed_size = iv_seed_size;
		bcopy(iv_seed, header->iv_seed, iv_seed_size);
	    } else {
		/* No reassemble vector- drop the packet. */
		if (crypto_engine_debug)
		    buginf("\nCRYPTO_ENGINE: can't reassemble packet");
		crypto_drop_packet(header);
		continue;
	    }
	}

	/* 
	 * We should improve the way the idb is assigned to conn. Here we
	 * check the idb value for every packet.
	 */
	if (!epa_swcrypto_conn_tbl[conn_id].swidb) {
	    epa_swcrypto_conn_tbl[conn_id].swidb =
		header->pak_info.hwidb->firstsw;
	}

	send = TRUE;
            
        /* Process the command */
        switch (header->command) {
	case CRYPTO_ENCRYPT_PACKET:
            if (!crypto_engine_encrypt) {
                if(crypto_engine_debug) {
                    buginf("\nCRYPTO_ENGINE: encrypt vector is not" 
                           " initialized");
                }
                crypto_drop_packet(header);
                return;
            }
	    (*crypto_engine_encrypt)(CRYPTO_ENCRYPT, header);
	    /* Increase the count */
	    epa_swcrypto_conn_tbl[conn_id].encrypt_count++;

	    break;

	case CRYPTO_DECRYPT_PACKET:
            if (!crypto_engine_encrypt) {
                if(crypto_engine_debug) {
                    buginf("\nCRYPTO_ENGINE: decrypt vector is not" 
                           " initialized");
                }
                crypto_drop_packet(header);
                return;
            }
	    (*crypto_engine_decrypt)(CRYPTO_DECRYPT, header);
	    /* Increase the count */
	    epa_swcrypto_conn_tbl[conn_id].decrypt_count++;

	    /*
	     * If the packet needs encryption under a different connection id,
	     * re-calcuate the encryption offset/iv/etc and requeue the packet 
	     * for encryption.
	     */
	    pak = header->pak_info.pak;
    	    if (pak->cryptoflags & CRYPTO_ENCRYPT) {

    	    	pak->cryptoflags &= ~CRYPTO_DECRYPT;
    	    	pak->cryptoflags |= CRYPTO_DECRYPT_DONE;
    	    	/*
    	     	 * Re-queue depending on the mode
    	     	 */
    	    	switch (header->switch_mode) {
    	    	case CRYPTO_PROCESS_SWITCH:
       		    (*((crypto_vector_t *)pak->
			if_input->hwptr->crypto_vectors)->ps_vec)
    		    	(CRYPTO_ENCRYPT, pak, TRUE, TRUE);
    		    break;
    	    	case CRYPTO_FAST_SWITCH:
       		    (*((crypto_vector_t *)pak->
			if_input->hwptr->crypto_vectors)->fs_vec)
    	            	(pak->if_output->hwptr, pak);
    		    break;
    	    	case CRYPTO_TUNNEL_FAST_SWITCH:
       		    (*((crypto_vector_t *)pak->
			if_input->hwptr->crypto_vectors)->tunnel_fs_vec)
    	            	(pak->if_output->hwptr, pak);
    		    break;
    	    	case CRYPTO_RSP_FLOW_SWITCH:
    	    	case CRYPTO_VIP_FAST_SWITCH:
    		    /* BEW - TBD */
    		    break;
		}
    	    	send = FALSE;
 	    }
	    break;

	default:
	    break;
	}

	header->status = EPA_OP_OK;
   
	/* Send the packet out */ 
	if (send) {
	    (*crypto_engine_idb.swepa_output)(header->command, (void *)header);
	}
    }
} 


/*
 * crypto_key_proc
 *
 * This is the process which executes key related requests. 
 */
forktype crypto_key_proc (void)
{
    ulong major, minor;

    /* Initialize the random number generator */
    random_fill (crypto_ran_num, CRYPTO_RANDOM_NUM_SIZE, TRUE);

    process_watch_queue(crypto_engine_idb.expq, ENABLE, RECURRING);

    while (TRUE) {
        process_safe_wait_for_event();
        while (process_get_wakeup(&major, &minor)) {
            switch (major) {
	    case QUEUE_EVENT:
                blocking_disabled = FALSE;
		crypto_key_process_req();
                blocking_disabled = TRUE;
		break;
	    default:
		break;
            }
        }
    }
 }

void crypto_key_process_req (void)
{
    epa_crypto_blk *blk;
    epa_status     status = EPA_OP_OK;

    /* Retrive from the queue and process the request. */
    blk = crypto_key_dequeue(&crypto_engine_idb);
    if (!blk) {
        if (crypto_engine_debug) {
            buginf("\nCRYPTO_ENGINE: key proc got notified with the"
                   " null key request");
        }
	return;
    }

    /* Start executing the command. */
    switch (blk->header.command_id) {
    case CRYPTO_CREATE_SESSION_KEY:
	{ 
            epa_crypto_session_key_data_blk* data_blk = NULL;
            ushort conn_id;

            data_blk = (epa_crypto_session_key_data_blk*)blk->data;
            conn_id = data_blk->connection_id;

            status=epa_swcrypto_gen_session_key(data_blk->encrypt_alg_type,
                                                conn_id, DH_LENGTH_MIN,
                                    crypto_dh_pairs[conn_id].shared_number);

            /*
             * Reset the DH tbl entry by throwing out both private and public
             * DH number, as well as the shared number.
             */
	}
	break;

    case CRYPTO_CLEAR_DH_NUMBER:
        {
            crypto_clear_dh_number_data_blk* data_blk = NULL;
            ushort conn_id;

            data_blk = (crypto_clear_dh_number_data_blk*)blk->data;
            conn_id = data_blk->connection_id;

            if (crypto_dh_pairs[conn_id].status == CRYPTO_DH_PAIR_SET) {
                crypto_dh_pairs[conn_id].status = CRYPTO_DH_PAIR_UNSET;
                memset(crypto_dh_pairs[conn_id].secret_number,0x00,
                       DH_LENGTH_MIN);
                memset(crypto_dh_pairs[conn_id].public_number,0x00,
                       DH_LENGTH_MIN);
                memset(crypto_dh_pairs[conn_id].shared_number,0x00,
                       DH_LENGTH_MIN); 
            }
         }
         break;

    case CRYPTO_SIGN:
	{
            crypto_sign_data_blk* data_blk = NULL;
            uchar *data_buf;
            uchar sig_r[SHA_LENGTH];
            uchar sig_s[SHA_LENGTH];

            data_blk = (crypto_sign_data_blk *)blk->data;
            data_buf = data_blk->data;

            /*The signature should be generated in the order sig_r, sig_s.*/
            status = crypto_dss_sign (sig_r, sig_s, 
                                      crypto_pri_key,
                                      crypto_ran_num,
                                      data_buf,
                                      data_blk->data_length);

            bcopy(sig_r, blk->data, SHA_LENGTH);
            bcopy(sig_s, blk->data + SHA_LENGTH, SHA_LENGTH);
            blk->header.data_size = SHA_LENGTH * 2; 
        }
	break;

    case CRYPTO_VIP_SIGN:
	{
            crypto_vip_sign_data_blk* data_blk = NULL;
            uchar *data_buf;
            uchar sig_r[SHA_LENGTH];
            uchar sig_s[SHA_LENGTH];

            data_blk = (crypto_vip_sign_data_blk *)blk->data;
            data_buf = blk->data + sizeof(crypto_vip_sign_data_blk);

            /*The signature should be generated in the order sig_r, sig_s.*/
            status = crypto_dss_sign (sig_r, sig_s, 
                                      crypto_pri_key,
                                      crypto_ran_num,
                                      data_buf,
                                      data_blk->data_length);

            bcopy(sig_r, blk->data, SHA_LENGTH);
            bcopy(sig_s, blk->data + SHA_LENGTH, SHA_LENGTH);
            blk->header.data_size = SHA_LENGTH * 2; 
        }
        break;
                
    case CRYPTO_VERIFY:
	{
            crypto_verify_data_blk*   data_blk = NULL;
            ushort                    result;

            data_blk = (crypto_verify_data_blk *)blk->data;
            status = crypto_dss_verify (&result,
                                    data_blk->clear_data,
                                    data_blk->clear_data_length,
                                    data_blk->signature,
                                    data_blk->signature+SHA_LENGTH, 
                                    data_blk->public_key);
            if (status == EPA_OP_OK) {
	        bcopy(&result, blk->data, sizeof(result));
            }
	}
	break;

    case CRYPTO_VIP_VERIFY:
        {
            crypto_vip_verify_data_blk *data_blk = NULL;
            uchar *clear_data = NULL, *signature = NULL;
            ushort                    result;

            data_blk = (crypto_vip_verify_data_blk *)blk->data;
            clear_data = blk->data + sizeof(crypto_vip_verify_data_blk);
            signature = clear_data + data_blk->clear_data_length;

            status = crypto_dss_verify (&result, clear_data,
                                        data_blk->clear_data_length,
                                        signature,
                                        signature+SHA_LENGTH, 
                                        data_blk->public_key);
            if (status == EPA_OP_OK) {
                bcopy(&result, blk->data, sizeof(result));
            }
        }
        break;

    case CRYPTO_GEN_DSS_KEY_PAIR:
	{ 
            epa_rtn_sig_key *rtn_pub_key_data_blk = NULL;

            status = crypto_dss_gen_key(crypto_pub_key,
                                        crypto_pri_key,
                                        crypto_ran_num);
            if (status == EPA_OP_OK) {
                rtn_pub_key_data_blk = (epa_rtn_sig_key *)blk->data;
                rtn_pub_key_data_blk->alg_type = CRYPTO_GEN_KEY_PAIR;
                rtn_pub_key_data_blk->key_tag.dss_keys.pub_key_size = 
                                                        DSS_LENGTH_MIN;
                bcopy(&crypto_pub_key[0], 
                      rtn_pub_key_data_blk->key_tag.dss_keys.pub_key,
                      DSS_LENGTH_MIN);
                rtn_pub_key_data_blk->key_tag.dss_keys.pri_key_size = 
                                                            SHA_LENGTH;
                bcopy(&crypto_pri_key[0], 
	            rtn_pub_key_data_blk->key_tag.dss_keys.pri_key,
		    SHA_LENGTH);
		
		rtn_pub_key_data_blk->serial_number = crypto_processor_serial;

                blk->header.data_size = sizeof(epa_rtn_sig_key);
            }
	}
	break;

    case CRYPTO_GEN_DH_ALG_PHASE1_PARAM:
	{
	    epa_rtn_alg_param *rtn_alg_param_data_blk = NULL;
	    crypto_gen_dh_param_data_blk *data_blk = NULL;
	    ushort  conn_id;

	    data_blk = (crypto_gen_dh_param_data_blk *)blk->data;
	    conn_id = data_blk->connection_id;

	    /* 
             * Check if we have DH pairs precomputed. If it is, use
	     * the precomputed one. Otherwise, generate DH pairs on the
	     * fly.
	     */

	    if (crypto_dh_pairs[conn_id].status!= CRYPTO_DH_PAIR_SET) {
	        status = crypto_gen_dh_pair(
                         crypto_dh_pairs[conn_id].secret_number,
                         crypto_dh_pairs[conn_id].public_number,
                         crypto_ran_num);
            }

            if (status == EPA_OP_OK) {
                crypto_dh_pairs[conn_id].status = CRYPTO_DH_PAIR_SET;
                rtn_alg_param_data_blk = (epa_rtn_alg_param *)blk->data;
                rtn_alg_param_data_blk->alg_type = DH_ALG_PHASE1;

                bcopy(crypto_dh_pairs[conn_id].public_number, 
                      rtn_alg_param_data_blk->alg_tag.dh_pub.number,
                      DH_LENGTH_MIN);
                blk->header.data_size = sizeof(epa_rtn_alg_param);
            }
	}
	break;

    case CRYPTO_GEN_DH_ALG_PHASE2_PARAM:
	{
            crypto_gen_dh_param_data_blk *data_blk = NULL;
	    ushort conn_id;
	
	    data_blk = (crypto_gen_dh_param_data_blk *)blk->data;
	    conn_id = data_blk->connection_id;

            status = crypto_get_dh_shared (
                       crypto_dh_pairs[conn_id].secret_number,
                       data_blk->peer_public_number,
                       crypto_dh_pairs[conn_id].shared_number,
                       NULL );
	}
	break;

    case CRYPTO_CLEAR_CONN_ENTRY:
        {
            crypto_clear_conn_entry_data_blk *data_blk = NULL;
            ushort conn_id;
            leveltype level;
   
            data_blk = (crypto_clear_conn_entry_data_blk *)blk->data;
            conn_id = data_blk->conn_id;
            level = raise_interrupt_level(NETS_DISABLE);
            epa_swcrypto_conn_tbl[conn_id].state = EPA_SWCRYPTO_CONN_UNSET;
            reset_interrupt_level(level);

            /*
             * Free the memory.
             */
            if (epa_swcrypto_conn_tbl[conn_id].expanded_encrypt_key) {
                free(epa_swcrypto_conn_tbl[conn_id].expanded_encrypt_key);
            }
        }
        break;

    case CRYPTO_ZEROIZE:
        memset(crypto_pub_key, 0x00, DSS_LENGTH_MIN);
        memset(crypto_pri_key, 0x00, SHA_LENGTH);
        status = EPA_OP_OK;
        break;

    case CRYPTO_GEN_SYNDROME:
        {
            crypto_gen_syndrome_data_blk *data_blk = NULL;
            ushort length;
            uchar buffer[DH_LENGTH_MIN];
   
            data_blk = (crypto_gen_syndrome_data_blk *)blk->data;
            length = data_blk->challenge_length;
            memset(buffer, 0x00, DH_LENGTH_MIN); 

            status = crypto_gen_syndrome(data_blk->connection_id,
                                         data_blk->challenge_length,
                                         data_blk->offset,
                                         buffer);
            bcopy(buffer, blk->data, length); 
            blk->header.data_size = length; 
        }
        break;

    default:
	status = EPA_OP_FAIL;
	break;
    }      

    blk->header.status = status;

    (*crypto_engine_idb.swepa_output)(CRYPTO_EXP, (void *)blk);
}

/*
 * crypto_engine_proc_encrypt_enqueue
 * 
 * This procedure enqueue the packet to the queue drained by the 
 * crypto_engine_encrypt_proc.  It is called at the process level. 
 * The interrupt has to be disabled since the encryption input queue is 
 * also used by fastswitch code to enqueue the packet.
 */

inline void crypto_engine_proc_encrypt_enqueue (crypto_engine_idb_t *idb,
						void *blk)
{
    leveltype    level;

    level = raise_interrupt_level(ALL_DISABLE);
    crypto_encrypt_enqueue(idb,(crypto_encrypt_header* )blk);
    reset_interrupt_level(level);
}

/*
 * crypto_engine_fast_encrypt_enqueue
 * 
 * This procedue is called at interrpt level by fastswitch code and 
 * enqueue the packet to the input queue drained by the 
 * crypto_engine_encrypt_proc.
 */

inline void crypto_engine_fast_encrypt_enqueue (crypto_engine_idb_t *idb,
						void *blk)
{
   crypto_encrypt_enqueue(idb,(crypto_encrypt_header* )blk);
}

/*
 * crypto_engine_key_proc_yield
 *
 * Check if the calling process should be suspended. It is called by
 * the crypto lib. 
 */

int crypto_engine_key_proc_yield (int flag)
{
    if (process_time_exceeded()) {
	process_safe_suspend();
    }

    return CRYPTO_NO_CANCEL;
}

/*
 * copy_cert_to_buffer
 *
 * copy the clear data from particles to the buffer to create
 * signature
 * When check in, recode this.
 * Here we assume this is a icmp message; all the data after the icmp 
 * header should be signed.
 */
boolean copy_cert_to_buffer (paktype *pak, uchar *buf, uchar *data,
                                ushort size)
{
    particletype *mp;
    ushort count;
    uchar *cert;
    ushort left;
    iphdrtype* ip = (iphdrtype *)ipheadstart(pak);

    if (!pak || !data ||!buf) {
        return FALSE;
    }

    cert = ipheadstart(pak) + ipheadsize(ip) + ICMP_HEADER_SIZE;
    mp = pak->particlequeue.qhead;
    if (!mp) {
        return FALSE;
    }

    left = mp->data_bytes - (cert - mp->data_start); 
 
    while (size) {
        count = min(size, left);
        bcopy(cert, buf, count); 
        size -= count; 
        buf += count; 
        mp = mp->next;
        if (!mp && size) {
            return FALSE;
        } 
        if (size == 0) {
            return TRUE;
        }
        cert = mp->data_start;
        left = mp->data_bytes;
    }
    return TRUE;
}


void crypto_dump_data (uchar *data, ushort length)
{
    ushort i, j=0;

    buginf("\n");
    for (i=0; i<length; i++) {
        buginf("%02x ", data[i]);
        j++;
        if (j>=20) {
            j = 0;
            buginf("\n");
        }
    }
}

               
