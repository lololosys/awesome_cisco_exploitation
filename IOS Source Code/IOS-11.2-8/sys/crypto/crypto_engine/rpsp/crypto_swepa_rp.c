/* $Id: crypto_swepa_rp.c,v 1.1.70.15 1996/09/10 14:14:42 sbelair Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/rpsp/crypto_swepa_rp.c,v $
 *------------------------------------------------------------------
 * crypto_swepa_rp.c 
 * 
 * This file defines crypto engine functions common to RP platform. 
 *
 * 11, 1995, xliu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_swepa_rp.c,v $
 * Revision 1.1.70.15  1996/09/10  14:14:42  sbelair
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
 * Revision 1.1.70.14  1996/09/06  23:57:45  bew
 * CSCdi62313:  Sourced fragments dont encrypt properly
 * Branch: California_branch
 * Add support for fragmented packets: always reassemble fragments
 * before either encryption or decryption.
 *
 * Revision 1.1.70.13  1996/08/07  09:21:59  xliu
 * CSCdi63274:  Wrong crypto engine name a show cr eng conf
 * Branch: California_branch
 *
 * Revision 1.1.70.12  1996/07/15  10:06:50  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 1.1.70.11  1996/07/03  01:29:20  xliu
 * CSCdi61913:  crypto engine doesnt check return value of pak_duplicate()
 * Branch: California_branch
 *
 * Revision 1.1.70.10  1996/06/21  22:32:07  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 1.1.70.9  1996/06/07  00:26:45  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Change iv offset and encryption point if the IP payload is too small
 * (e.g., a fragment).
 *
 * Revision 1.1.70.8  1996/06/06  19:37:30  xliu
 * CSCdi59758:  remove crypto map from interface does not reset fastsend
 * correctly
 * Branch: California_branch
 *
 * Revision 1.1.70.7  1996/06/06  19:31:51  xliu
 * CSCdi59758:  remove crypto map from interface does not reset fastsend
 * correctly
 * Branch: California_branch
 *
 * Revision 1.1.70.6  1996/06/05  23:11:56  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Fix crypto zeroize & key generation for 7500 with VIPs.
 * Fix crypto connection setup with traffic in both directions.
 * Fix encryption bugs.
 *
 * Revision 1.1.70.5  1996/05/28  15:59:59  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 1.1.70.4  1996/05/21  09:44:35  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 1.1.70.3  1996/05/12  23:13:16  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 1.1.70.2  1996/05/07  21:32:04  hampton
 * CSCdi52864:  Make 11.2 compile with the Cygnus progressive-96q1 release
 * Branch: California_branch
 *
 * Revision 1.1.70.1  1996/04/19  14:57:07  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.4.21  1996/04/17  23:48:17  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 1.1.4.20  1996/03/29  18:04:44  xliu
 * Branch: IosSec_branch
 * Fix naming problem.
 *
 * Revision 1.1.4.19  1996/03/27  01:52:52  xliu
 * Branch: IosSec_branch
 *
 * Revision 1.1.4.18  1996/03/25  23:31:27  xliu
 * Branch: IosSec_branch
 * Fix software encryption/decryption output problem.
 *
 * Revision 1.1.4.17  1996/03/21  10:08:44  xliu
 * Branch: IosSec_branch
 * Integrated with RSP changes.
 *
 * Revision 1.1.4.16  1996/03/05  02:01:10  xliu
 * Branch: IosSec_branch
 * Add the disable and enable of interrupts when removing vectors.
 *
 * Revision 1.1.4.15  1996/03/05  01:14:10  xliu
 * Branch: IosSec_branch
 * Fixed the pointer problem.
 *
 * Revision 1.1.4.14  1996/03/01  01:39:50  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 1.1.4.13  1996/02/27  06:42:19  xliu
 * Branch: IosSec_branch
 * Fixed compiler errors after sync.
 *
 * Revision 1.1.4.12  1996/02/13  19:16:41  carrel
 * Branch: IosSec_branch
 * Crypto Map checkin (finally)
 *
 * Revision 1.1.4.11  1996/02/13  17:00:01  xliu
 * Branch: IosSec_branch
 * Use vectors as encryption/decryption routine.
 *
 * Revision 1.1.4.10  1996/01/24  22:56:13  bew
 * Branch: IosSec_branch
 * Sync-related changes to sys/crypto.
 *
 * Revision 1.1.4.9  1996/01/20  16:51:32  xliu
 * Branch: IosSec_branch
 * Solve the 7000 problem with ip les fastswitch code.
 *
 * Revision 1.1.4.8  1996/01/19  07:01:05  xliu
 * Branch: IosSec_branch
 * Commit before the second sync.
 *
 * Revision 1.1.4.7  1996/01/13  05:46:59  bew
 * Branch: IosSec_branch
 * Cleanup. Lots of cleanup in the IosSec_branch crypto directory.
 *
 * Revision 1.1.4.6  1996/01/09  08:51:45  xliu
 * Branch: IosSec_branch
 * Moved from crypto/crypto_engine/rpsp dir to crypto dir.
 *
 * Revision 1.1.4.5  1996/01/09  01:12:13  xliu
 * Branch: IosSec_branch
 * RSP flow switch support.
 *
 * Revision 1.1.4.4  1996/01/04  18:56:03  xliu
 * Branch: IosSec_branch
 * After initial clean up.
 *
 * Revision 1.1.4.3  1995/12/19  03:41:52  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 1.1.4.2  1995/12/10  09:28:50  xliu
 * Branch: IosSec_branch
 * Moved encryption from datagram_out to ipsendnet, after fragmentation.
 *
 * Revision 1.1.4.1  1995/11/30  05:42:31  xliu
 * Branch: IosSec_branch
 *
 * Files created for crypto engine rsp.
 *
 * Revision 1.1  1995/11/30  05:13:09  xliu
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
#include "types.h"
#include "registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "string.h"
#include "interface_private.h"
#include "packet.h"
#include "../if/ether.h"
#include "../if/fddi.h"
#include "config.h"
#include "../wan/serial.h"
#include "../ip/ip.h"
#include "../ipmulticast/igmp.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "../ip/ipaccount.h"
#include "../ip/ipfast.h"
#include "../if/tring.h"
#include "../wan/frame_relay.h"
#include "../wan/atm_dxi.h"
#include "../wan/smds.h"
#include "../wan/dialer.h"
#include "../wan/dialer_private.h"
#include "../if/network.h"
#include "../if/atm.h"
#include "sched.h"
#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"

#include "sys_registry.h"
#include "../crypto/crypto_registry.h"

#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_tcpip.h"
#include "../ip/ip_registry.h"

#include "../crypto/crypto_public.h"
#include "../crypto/crypto_engine/crypto_lib/encrypt.h"
#include "../crypto/crypto_engine/crypto_lib/lib_toolkit.h"
#include "../crypto/crypto.h"
#include "../crypto/crypto_map.h"
#include "../crypto/crypto_engine_api.h"
#include "../crypto/crypto_engine/core/crypto_engine.h"
#include "../crypto/crypto_engine/core/crypto_swepa.h"
#include "../crypto/crypto_sesmgmt.h"
#include "../ui/debug.h"
#include "../crypto/crypto_debug.h"
#include "crypto_swepa_common.h"

/* Function prototypings */

void crypto_swepa_rp_output(ushort, void*);
void crypto_engine_req_timer_update(void);
void crypto_swepa_initialization(void);
void crypto_ip_decrypt(paktype *, boolean, boolean);
void crypto_ip_encrypt(paktype *, boolean, boolean);
void crypto_engine_rp_fastsend(hwidbtype *, paktype *);
void crypto_engine_rp_tunnel_fs(hwidbtype *, paktype *);
static void assign_crypto_vectors(hwidbtype *);
void crypto_dialer_propagate_map(hwidbtype *, hwidbtype *);
void crypto_dialer_remove_map(hwidbtype *);

void crypto_swepa_rp_output (ushort type, void  *rsp)
{
    crypto_encrypt_header *header = (crypto_encrypt_header *)rsp;

    switch (type) {
    case CRYPTO_ENCRYPT_PACKET:
	switch (header->switch_mode) {
	case CRYPTO_PROCESS_SWITCH:
	    crypto_encrypt_ps_output(header);
	    break;

	case CRYPTO_FAST_SWITCH:
	    crypto_encrypt_fs_output(header);
	    break;
        default:
            crypto_drop_packet(header);
            if (crypto_engine_debug) {
                buginf("\nCRYPTO_ENGINE: packet dropped because of "
                       "wrong switching mode");
            }
	};
	break;

    case CRYPTO_DECRYPT_PACKET:
	switch (header->switch_mode) {
	case CRYPTO_PROCESS_SWITCH:
	    /* For process switch, the packet is decrypted and send out. */
	    crypto_decrypt_ps_output(header);
	    break;

	case CRYPTO_FAST_SWITCH:
	    /* FS packet has the output interface identified.*/
	    crypto_decrypt_fs_output(header);
	    break;

	case CRYPTO_TUNNEL_FAST_SWITCH:
	    /* Send packet right back into tunnel FS routine */
	    crypto_decrypt_tunnel_fs_output(header);
	    break;
        default:
            crypto_drop_packet(header);
            if (crypto_engine_debug) {
                buginf("\nCRYPTO_ENGINE: packet dropped because of "
                       "wrong switching mode");
            }
	}
	break;

    case CRYPTO_EXP:
	/* Call epa_engine_rsp_disp */
	epa_engine_rsp_dspt ((epa_crypto_blk*)rsp);
	break;

    default:
        if (crypto_engine_debug) {
            buginf("\nCRYPTO_ENGINE: unknown command id.");
        }
	break;
    }
};

void crypto_engine_fs_decrypt (paktype *pak)
{
    crypto_pak_info    pak_info;
    iphdrtype *ip = (iphdrtype *)ipheadstart(pak);
    crypto_fs_pak_info *fs_pak_info = (crypto_fs_pak_info *)&pak_info;
    uchar iv_seed[CRYPTO_MAX_IV_SIZE];
    ushort iv_seed_size;
    int crypto_offset;
    
    crypto_set_encrypt_start(ip, &crypto_offset);

    fs_pak_info->pak = pak;
    fs_pak_info->ip = ip;
    fs_pak_info->hwidb = pak->if_output->hwptr;
    fs_pak_info->switch_mode = CRYPTO_FAST_SWITCH;
    crypto_get_iv_seed(ip, iv_seed, &iv_seed_size);
    epa_decrypt_packet(ipheadstart(pak) + crypto_offset,
		       ip->tl - crypto_offset,
	               pak->crypto_dec_conn_id, &iv_seed[0], iv_seed_size, 
		       &pak_info);
}

/*
 * crypto_engine_register_crypto_vectors
 *
 * Called when a crypto map is assigned to an interface. The 
 * media vectors are saved in idb->crypto_vectors and being
 * replaced by crypto vectors. Here the output encryption vector
 * at both process and fastswitch level are replaced.
 */
void crypto_engine_register_crypto_vectors (hwidbtype *idb, void *head) 
{

    assign_crypto_vectors(idb);
    upd_config_tbl (idb, CRYPTO_ENGINE_REGISTER);
}

static void assign_crypto_vectors (hwidbtype *idb)
{ 

    leveltype level;
    crypto_vector_t* crypto_vec;

    if (idb->crypto_vectors) {
        return;
    }

    crypto_vec = malloc (sizeof(crypto_vector_t));
    if (!crypto_vec) {
        return;
    }

    level = raise_interrupt_level(NETS_DISABLE);
    idb->status |= IDB_ENCRYPT;
    idb->crypto_vectors = (void *)crypto_vec;
    ((crypto_vector_t *)idb->crypto_vectors)->fs_vec =
                                            crypto_engine_rp_fastsend;
    ((crypto_vector_t *)idb->crypto_vectors)->ps_vec = crypto_engine_ps_vec; 
    ((crypto_vector_t *)idb->crypto_vectors)->tunnel_fs_vec =
					    crypto_engine_rp_tunnel_fs;
    reset_interrupt_level(level);

}

/*
 * crypto_engine_restore_media_vectors
 * 
 * Remove the crypto vectors from the given idb. This is called when
 * a crypto map is removed from an interface. 
 */
void crypto_engine_restore_media_vectors (hwidbtype *idb, void *head)
{
    leveltype level;


    level = raise_interrupt_level(NETS_DISABLE);
    idb->status &= ~IDB_ENCRYPT;
    reset_interrupt_level(level);

    if (idb->crypto_vectors) {
        free (idb->crypto_vectors);
        idb->crypto_vectors = NULL;
    }

    upd_config_tbl(idb, CRYPTO_ENGINE_DEREGISTER);
}

void crypto_engine_rp_fastsend (hwidbtype *hwidb, paktype *pak)
{
    crypto_pak_info    pak_info;
    crypto_fs_pak_info *fs_pak_info = (crypto_fs_pak_info *)&pak_info;
    uchar iv_seed[CRYPTO_MAX_IV_SIZE];
    ushort iv_seed_size;
    iphdrtype *ip = (iphdrtype *)ipheadstart(pak);
    int crypto_offset;


    if (pak->cryptoflags & CRYPTO_ENCRYPT ||
         pak->cryptoflags & CRYPTO_DECRYPT) {
        
        fs_pak_info->pak = pak;
        fs_pak_info->ip = ip; 
        fs_pak_info->hwidb = hwidb;
        fs_pak_info->switch_mode = CRYPTO_FAST_SWITCH;
        crypto_set_encrypt_start(ip, &crypto_offset);
        crypto_get_iv_seed(ip, iv_seed, &iv_seed_size);

        if (pak->cryptoflags & CRYPTO_DECRYPT) {
            epa_decrypt_packet( ipheadstart(pak) + crypto_offset,
               ip->tl - crypto_offset,
               pak->crypto_dec_conn_id, &iv_seed[0], iv_seed_size, &pak_info);
        } else {
            epa_encrypt_packet( ipheadstart(pak) + crypto_offset,
               ip->tl - crypto_offset,
	       pak->crypto_enc_conn_id, &iv_seed[0], iv_seed_size, &pak_info);
        }
        return;
    } else {
        (*hwidb->fastsend)(hwidb,pak);
        return;
    }
}

void crypto_engine_rp_tunnel_fs (hwidbtype *idb, paktype *pak)
{
    crypto_pak_info    pak_info;
    crypto_fs_pak_info *fs_pak_info = (crypto_fs_pak_info *)&pak_info;
    uchar iv_seed[CRYPTO_MAX_IV_SIZE];
    ushort iv_seed_size;
    iphdrtype *ip = (iphdrtype *)ipheadstart(pak);
    int crypto_offset;

    if (pak->cryptoflags & CRYPTO_DECRYPT) {
        fs_pak_info->pak = pak;
        fs_pak_info->ip = ip; 
        fs_pak_info->hwidb = pak->if_input->hwptr;
        fs_pak_info->switch_mode = CRYPTO_TUNNEL_FAST_SWITCH;
        crypto_set_encrypt_start(ip, &crypto_offset);
        crypto_get_iv_seed(ip, iv_seed, &iv_seed_size);
	epa_decrypt_packet(ipheadstart(pak) + crypto_offset,
               		   ip->tl - crypto_offset,
               		   pak->crypto_dec_conn_id, 
			   &iv_seed[0], 
			   iv_seed_size, 
			   &pak_info);
    }
}

/*
 * crypto_ip_encrypt
 */
inline void
crypto_ip_encrypt(paktype *pak, boolean disposeflag, boolean log)
{
    iphdrtype *ip;
    ushort encryptlength;
    epa_status ret;
    uchar *encrypt_start;
    crypto_pak_info pak_info;
    crypto_ps_pak_info *ps_pak_info;
    uchar iv_seed[CRYPTO_MAX_IV_SIZE];
    ushort iv_seed_size;
    int crypto_offset;

    ip = (iphdrtype *)ipheadstart(pak);
    crypto_set_encrypt_start(ip, &crypto_offset);

    /* Convert to crypto_fs_pak to use the pak field. */
    ps_pak_info = (crypto_ps_pak_info *)&pak_info;
    ps_pak_info->pak = pak;
    ps_pak_info->ip = ip;
    ps_pak_info->hwidb = pak->if_output->hwptr;
    ps_pak_info->switch_mode = CRYPTO_PROCESS_SWITCH;
    ps_pak_info->disposeflag = disposeflag;
    ps_pak_info->log = log;

    /*
     * Call crypto engine to encrypt. The switch mode is set to process
     * switch since this routine is only called at process level. Encrypt
     * starts at 8 bytes offset the transport header (Cylink definition).
     * The iv is to be generated by crypto engine.
     */
    encrypt_start = ipheadstart(pak) + crypto_offset;
    encryptlength = ip->tl - crypto_offset;
    crypto_get_iv_seed(ip, iv_seed, &iv_seed_size);

    ret = epa_encrypt_packet(encrypt_start, encryptlength,
                             pak->crypto_enc_conn_id, &iv_seed[0], 
			     iv_seed_size, &pak_info);
    if (ret != EPA_OP_OK) {
        retbuffer(pak);
    } 
}

/*
 * Decrypt the entire IP packet EXCEPT the IP header. That
 * is, from the transport header until the end of the packet.
 */
inline void
crypto_ip_decrypt (paktype *pak, boolean disposeflag, boolean log)
{
    iphdrtype *ip;
    ushort decryptlength;
    epa_status ret;
    uchar *decrypt_start;
    crypto_pak_info pak_info;
    crypto_ps_pak_info *ps_pak_info;
    uchar iv_seed[CRYPTO_MAX_IV_SIZE];
    ushort iv_seed_size;
    int crypto_offset;

    ip = (iphdrtype *)ipheadstart(pak);
    crypto_set_encrypt_start(ip, &crypto_offset);

    /* Convert to crypto_fs_pak to use the pak field. */
    ps_pak_info = (crypto_ps_pak_info *)&pak_info;
    ps_pak_info->pak = pak;
    ps_pak_info->ip = ip;
    ps_pak_info->hwidb = pak->if_input->hwptr;
    ps_pak_info->switch_mode = CRYPTO_PROCESS_SWITCH;
    ps_pak_info->disposeflag = disposeflag;
    ps_pak_info->log = log;

    /*
     * Make API call
     */
    decrypt_start = ipheadstart(pak) + crypto_offset;
    decryptlength = ip->tl - crypto_offset;

    crypto_get_iv_seed(ip, iv_seed, &iv_seed_size);

    ret = epa_decrypt_packet(decrypt_start, decryptlength, 
                             pak->crypto_dec_conn_id, &iv_seed[0], iv_seed_size,
                             &pak_info);
    if (ret != EPA_OP_OK) {
        /* report error */
        retbuffer(pak);
    } 
}

void crypto_engine_ps_vec (ushort command, paktype *pak, boolean disposeflag,
                           boolean log)
{
    paktype *copy;

    switch (command) {
    case CRYPTO_ENCRYPT:
        /* 
         * If this packet is referenced more than once, we need to duplicate a 
         * packet, encrypt it and send it out. At the same time, the original 
         * packet is indicated as done.
         */
	if (pak->refcount > 1) {
	    copy = pak_duplicate(pak);
            if (!copy) {
                /* Drop the packet */
                datagram_done(pak);
                return;
            }
	    copy->refcount = 1;
            pak->cryptoflags = 0;
            pak->crypto_enc_conn_id = 0;
            pak->crypto_dec_conn_id = 0;
	    datagram_done(pak);
	} else {
	    copy = pak;
	}
	crypto_ip_encrypt(copy, disposeflag, log);
	break;

    case CRYPTO_DECRYPT:
	crypto_ip_decrypt(pak, disposeflag, log);
	break;
    default:
	if(crypto_engine_debug) {
	    buginf("\nCRYPTO_ENGINE: invalid command.");
	}
	break;
    }
}

/*
 * crypto_engine_rpsp_encrypt_proc
 *
 * Encrypt routine called by encryption process running on rpsp. On rpsp,
 * the data is in continues memory.
 */

void crypto_engine_rpsp_encrypt_decrypt (ushort act,crypto_encrypt_header *header)
{
    ushort i, block_num, trailing_len, type;
    uchar *data, *key;
    uchar encrypt_queue[EPA_DES_ENCDEC_BLOCK_LENGTH];
    ushort conn_id = header->conn_id;
    crypto_encrypt_decrypt_proc    action;

    data = header->data;
    block_num = (header->data_size)/EPA_DES_ENCDEC_BLOCK_LENGTH;
    trailing_len = (header->data_size) & 7;
    key = epa_swcrypto_conn_tbl[conn_id].expanded_encrypt_key;
    type = epa_swcrypto_conn_tbl[conn_id].alg_type;

    if (act == CRYPTO_ENCRYPT) {
        action = epa_des_encrypt;
    } else {
        action = epa_des_decrypt;
    }
        
    /*
     * Generate IV to initialize the encryption queue.
     */
    crypto_iv_gen(encrypt_queue, header->iv_seed, header->iv_seed_size,
		  epa_swcrypto_conn_tbl[conn_id].iv_key);

    /* First, encrypt the blocks which are 8 byte long. */
    for (i=0; i<block_num; i++) {
        action(type, key, data, encrypt_queue,
                        EPA_DES_ENCDEC_BLOCK_LENGTH); 
        data += EPA_DES_ENCDEC_BLOCK_LENGTH;
    }
    /*
     * Second, encrypt the last block which is less than 
     * 8 bytes, if exist.
     */
    if (trailing_len) {
        action(type, key, data, encrypt_queue, trailing_len); 
    }
}

/*
 * crypto_engine_rpsp_reassemble
 *
 * Reassmeble a fragmented packet before encryption or decryption. After
 * the packet has been reassembled, some pak fields need to be recalculated.
 */
paktype *crypto_engine_rpsp_reassemble (paktype *pak)
{
    iphdrtype *ip;
    idbtype *saved_if_output;
    ipaddrtype saved_desthost;

    /*
     * Need to save the original pak values before reassemble() is called
     * since reassemble() returns the original pak to the pool.
     */
    saved_desthost = pak->desthost;
    saved_if_output = pak->if_output;

    pak = reassemble(pak);
    if (pak == NULL) {
    	return(NULL);
    }

    /*
     * Now we have a reassembled pak. The IV and data values in header 
     * need to be re-calculated to correspond with the new pak.
     *
     * Note that the reassembled packet is forced to process switch.
     * It is very possible that fragmentation will be necessary before
     * it can be sent out, and it is necessary to do the fragmentation
     * at process switch.
     */
    ip = (iphdrtype *)ipheadstart(pak);

    /*
     * Reset the hwptr in the header so we can find the right output
     * interface.
     */
    pak->if_output = saved_if_output;
    pak->desthost = saved_desthost;
    
    /*
     * Fix the ip header checksum -- ip->tl has changed from the
     * original value.
     */
    ip->checksum = 0;
    ip->checksum = ipcrc((ushort *)ip, ip->ihl << 2);

    return(pak);
}

/*
 * crypto_engine_init
 *
 * This procedure initialize the crypto engine. On RP/RSP, it 
 * should be called when the crypto subsystem starts. It is the major
 * initialization routine for the crypto engine on both RP and RSP.
 */
void crypto_engine_init (ushort id, ushort type)
{
    ulong  tm;
    idbtype *idb;
    char *processor;
    ushort i;

    GetRTClock(&tm);

    /* Initialize the crypto engine slot. */
    crypto_engine_tbl[id].id = id; 
    crypto_engine_tbl[id].state = CRYPTO_ENGINE_INSTALLED;
    crypto_engine_tbl[id].name = NULL;
    crypto_engine_tbl[id].crypto_unit_type = type;
    crypto_engine_tbl[id].serial = crypto_processor_serial;
    crypto_engine_tbl[id].time_start = tm;
    crypto_engine_tbl[id].platform = crypto_engine_platform; 

    /* Initializa the crypto engine encrypt/decrypt routine */
    crypto_engine_encrypt = crypto_engine_rpsp_encrypt_decrypt;
    crypto_engine_decrypt = crypto_engine_rpsp_encrypt_decrypt;
    crypto_engine_reassemble = crypto_engine_rpsp_reassemble;

    /* Initialize the software encryption and key process */
    crypto_swepa_initialization();

    if (crypto_engine_platform == CRYPTO_PLATFORM_RP) {
        crypto_engine_idb.swepa_output = crypto_swepa_rp_output;
        reg_add_crypto_engine_register_crypto_vectors (
		          crypto_engine_register_crypto_vectors,
		          "crypto_engine_register_crypto_vectors");
        reg_add_crypto_engine_restore_media_vectors(
                          crypto_engine_restore_media_vectors,
			 "crypto_engine_restore_media_vectors");
        /*
         * Reinitialize the idb crypto vectors by checking each idb and its 
         * crypto maps. 
         */
        FOR_ALL_SWIDBS(idb) {
	    if (idb->crypto_map) {
	        crypto_engine_register_crypto_vectors(idb->hwptr, NULL);
            }
        }
    } else {
        /* Initialize the rsp required configuration. */
        reg_invoke_crypto_engine_rsp_conf_init((void*)&crypto_engine_tbl[id], 
        					   (void*)&crypto_engine_idb);
    }

    /*
     * Initialize the variable crypto_release_counter to an
     * architecture-dependent value. This count reflects how often
     * we give up the cpu in more compute-intensive routines in
     * the crypto toolkit which are used by the key process for
     * key generation and DSS verification. Initialize
     * crypto_release_counter in case we miss a processor type.
     */
    crypto_release_counter = CRYPTO_RELEASE_CNT_DEFAULT;
    processor = platform_get_string(PLATFORM_STRING_PROCESSOR);
    if ((strcmp(processor,"68000") == 0) ||
         (strcmp(processor,"68360") == 0) ||
         (strcmp(processor,"68020") == 0))
        crypto_release_counter = CRYPTO_RELEASE_CNT_HIGH;
    else if ((strcmp(processor,"68030") == 0) ||
             (strcmp(processor,"68ec030") == 0))
        crypto_release_counter = CRYPTO_RELEASE_CNT_MID;
    else if ((strcmp(processor,"68040") == 0) ||
             (strcmp(processor,"R4K") == 0)   ||
             (strcmp(processor,"R4600") == 0))
        crypto_release_counter = CRYPTO_RELEASE_CNT_LOW;
 
    crypto_yield_handle.yield_proc = crypto_engine_key_proc_yield;
    crypto_yield_handle.handle = &crypto_release_counter;

    /* Initialize the les fast switch vector if the ip les is supported.*/
    reg_invoke_ip_les_fs_query(&ip_les_fastswitch);

    /* register crypto engine key timeout */
    reg_add_crypto_engine_key_timeout(crypto_engine_key_timeout,
                                      "crypto_engine_key_timeout");

    /* register dialer hunt idbs config update call */
    reg_add_crypto_dialer_propagate_map(crypto_dialer_propagate_map,
                                       "crypto_dialer_propagate_map");

    /* register dialer to remove crypto map flags from the hunt idbs */
    reg_add_crypto_dialer_remove_map(crypto_dialer_remove_map,
                                     "crypto_dialer_remove_map");

    /* Initialize the crypto engine req pool */
    for (i=0; i<CRYPTO_ENGINE_REQPOOL_SIZE; i++) {
        epa_engine_req_pool[i].req_id = CRYPTO_ENGINE_REQ_ENTRY_UNUSED;
    }
}

void crypto_restore_media_vector (void *head)
{
    hwidbtype *hwidb;
    crypto_map_headtype *head_ptr = (crypto_map_headtype *)head;

    FOR_ALL_HWIDBS(hwidb) {
        if (!strncmp(hwidb->firstsw->crypto_map, head_ptr->map_tag, 
             strlen(head_ptr->map_tag))) { 
            crypto_engine_restore_media_vectors (hwidb, head);
        }
    }
}

void crypto_dialer_propagate_map (hwidbtype *idb1, hwidbtype *idb2) 
{
    leveltype level;
    
    if (!IS_ENCRYPTION(idb1)) {
        return;
    }
     
    level = raise_interrupt_level(NETS_DISABLE);
    idb2->ip_fast_flags |= IP_FAST_CRYPTO_MAP;
    idb2->firstsw->crypto_map = idb1->firstsw->crypto_map;
    idb2->crypto_vectors = idb1->crypto_vectors;
    idb2->status |= IDB_ENCRYPT;
    reset_interrupt_level(level);
}

/*
 * The basic function to remove the crypto map and flag from
 * the hunt idbs. We actually don't need ddb1 in this case. But to
 * use the existing macro, we need ddb1 as input parameter.
 */
void remove_map (dialerdbtype *ddb1, dialerdbtype *ddb2)
{
    hwidbtype *idb;
    leveltype level;
    dialerdbtype *tempddb;

    idb = ddb2->ownidb;

    level = raise_interrupt_level(NETS_DISABLE);
    idb->ip_fast_flags &= ~IP_FAST_CRYPTO_MAP;
    idb->firstsw->crypto_map = NULL;
    idb->crypto_vectors = NULL;
    idb->status &= ~IDB_ENCRYPT;
    reset_interrupt_level(level);

    if (HUNTGROUP_LEADER(ddb2)) {
        FOR_ALL_HUNTDDBS_ON_MASTER(ddb2, tempddb)
            remove_map(ddb2, tempddb);
    }
}

void crypto_dialer_remove_map (hwidbtype *idb)
{
    dialerdbtype *ddb, *hunt_ddb;

    if (!if_is_ddr(idb))
        return;

    ddb = get_dialerdb(idb);
    if (!HUNTGROUP_LEADER(ddb))
        return;

    FOR_ALL_HUNTDDBS_ON_MASTER(ddb, hunt_ddb) {
        remove_map(ddb, hunt_ddb);
    }
}







