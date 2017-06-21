/* $Id: crypto_swepa_vip_output.c,v 1.1.68.9 1996/07/22 07:38:59 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/vip/crypto_swepa_vip_output.c,v $
 *------------------------------------------------------------------
 * crypto_swepa_vip_output.c
 * 
 * This file defines the software encryption and key mgt output routines. 
 *
 * 11, 1995-1997, xliu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_swepa_vip_output.c,v $
 * Revision 1.1.68.9  1996/07/22  07:38:59  xliu
 * CSCdi63492:  PPP does not work when switched from HDLC on EPA/VIP2
 * Branch: California_branch
 *
 * Revision 1.1.68.8  1996/07/15  09:59:32  xliu
 * CSCdi61651:  epa randome number seed is not initialized
 * Branch: California_branch
 *
 * Revision 1.1.68.7  1996/06/26  10:26:45  xliu
 * CSCdi61336:  vip2 crypto code clean up
 * Branch: California_branch
 *
 * Revision 1.1.68.6  1996/06/21  22:33:10  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 1.1.68.5  1996/06/01  01:15:14  xliu
 * CSCdi59234:  crypto rsp flow vector does not work correct for vip
 * encryption
 * Branch: California_branch
 *
 * Revision 1.1.68.4  1996/05/28  16:00:14  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 1.1.68.3  1996/05/21  09:44:43  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 1.1.68.2  1996/05/12  23:13:40  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 1.1.68.1  1996/04/19  14:57:22  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.8  1996/04/17  23:48:34  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 1.1.2.7  1996/04/09  20:03:31  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 1.1.2.6  1996/03/30  05:12:48  xliu
 * Branch: IosSec_branch
 * Commit before CA repository split.
 *
 * Revision 1.1.2.5  1996/03/25  23:19:43  xliu
 * Branch: IosSec_branch
 * Checkin VIP crypto engine code.
 *
 * Revision 1.1.2.4  1996/01/30  22:36:21  xliu
 * Branch: IosSec_branch
 * Checking in encryption RSP/VIP support code.
 *
 * Revision 1.1.2.3  1996/01/09  02:26:58  xliu
 * Branch: IosSec_branch
 * VIP software encryption.
 *
 * Revision 1.1.2.2  1995/12/10  09:30:10  xliu
 * Branch: IosSec_branch
 * Merge the code to replace the idb fastsend using ipc.
 *
 * Revision 1.1.2.1  1995/11/30  05:39:55  xliu
 * Branch: IosSec_branch
 *
 * Files created for crypto engine vip.
 *
 * Revision 1.1  1995/11/30  05:23:35  xliu
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
#include "registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "sched.h"
#include "interface_private.h"
#include "packet.h"
#include "ttysrv.h"
#include "../if/ether.h"
#include "../if/fddi.h"
#include "config.h"
#include "../wan/serial.h"
#include "../ip/ip.h"
#include "../ipmulticast/igmp.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "../ip/ipaccount.h"
#include "../if/tring.h"
#include "../wan/frame_relay.h"
#include "../wan/atm_dxi.h"
#include "../wan/smds.h"
#include "../wan/ppp.h"
#include "../if/network.h"

#include "../src-vip/dfs_vip_idb.h"

#include "../ip/ipfast_index.h"
#include "../ip/ipfast_ipc.h"
#include "../ip/ipfast_vip.h"
#include "../ip/ipfast_vip_ipc.h"
#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"

#include "sys_registry.h"

#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_tcpip.h"
#include "../ip/ip_registry.h"

#include "../pas/if_pas.h"       /* includes pas_pci.h & segpak.h */
#include "../pas/if_pas_amdp2.h"

#include "../src-vip/vip_memd.h"
#include "../src-vip/vip_debug.h"

#include "../crypto/crypto_registry.h"
#include "../crypto/crypto_public.h"
#include "../crypto/crypto_map.h"
#include "../crypto/crypto_engine/crypto_lib/lib_toolkit.h"
#include "../crypto/crypto_engine/crypto_lib/encrypt.h"
#include "../crypto/crypto.h"
#include "../crypto/crypto_engine_api.h"
#include "../crypto/crypto_engine/core/crypto_engine.h"
#include "../crypto/crypto_engine/core/crypto_swepa.h"
#include "../crypto/crypto_engine/vip/crypto_engine_vip.h"
#include "../crypto/crypto_sesmgmt.h"

static paktype  stuff_pak;
static inline void crypto_init_stuff_pak(paktype *);
static inline void crypto_cleanup_pak(paktype *);

boolean crypto_copy_pak_to_particles (paktype *pak, paktype *new_pak,
                                   hwidbtype *idb)
{
    particletype *mp;
    ushort length;
    uchar *data;

    crypto_init_stuff_pak(new_pak);
    length = pak->datagramsize;
    data = pak->datagramstart;

    while (length) {

        mp = pool_dequeue_cache(idb->pool);

        if (!mp) {
            mp = pool_getparticle(idb->pool->fallback);
        }

        if (!mp) {
            particle_retbuffer(new_pak);
            return FALSE;
        } 

        mp->data_start = particle_center(mp);
        mp->data_bytes = min(idb->pool->size, length);
        mp->next = NULL;
        bcopy(data, mp->data_start, mp->data_bytes);
        data += mp->data_bytes;
        length -= mp->data_bytes;

        particle_enqueue_inline(new_pak, mp);
    }

    /* Assign the network_start which is needed by vip_ip_flow_fs */
    switch (idb->type) {
    case IDBTYPE_FDDIT: 
        new_pak->network_start = new_pak->datagramstart + FDDI_SNAP_ENCAPBYTES;
        break;
    case IDBTYPE_IBM2692:
        new_pak->network_start = new_pak->datagramstart + TRING_SNAP_ENCAPBYTES;
        break;
    case IDBTYPE_AMDP2:
        new_pak->network_start =new_pak->datagramstart + ETHER_ARPA_ENCAPBYTES; 
        break;
    case IDBTYPE_S4T68360:
        if (idb->enctype == ET_HDLC) {
            new_pak->network_start = new_pak->datagramstart + HDLC_ENCAPBYTES;
        } else {
            new_pak->network_start = new_pak->datagramstart + PPP_ENCAPBYTES;
        }
        break;

    default:
        /* This routine should never receive a packet whose idb type is */
        /* included in the case statments. If this ever happen, it is an */
        /* error.                                                        */
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: packets not supported for encryption.");
        }
        particle_retbuffer(new_pak);
        return FALSE;
    }
    new_pak->enctype = pak->enctype;
    new_pak->if_output = pak->if_output;
    new_pak->if_input = pak->if_input;
    new_pak->mci_status = pak->mci_status;

    return TRUE;
}

/*
 * crypto_swepa_vip_outut
 *
 * Called by crypto engine sw encryption/key process
 * to send the packet and key response out. Called at process level.
 */
void crypto_swepa_vip_output (ushort type, void  *rsp)
{
    crypto_encrypt_header* header;
    paktype* pak;
    leveltype level;
    crypto_vip_pak_info  *pak_info = NULL;
    hwidbtype *output;
    crypto_vector_t *crypto_vec;

    header = (crypto_encrypt_header *)rsp;

    switch (type) {

        case CRYPTO_ENCRYPT_PACKET:

            pak_info = (crypto_vip_pak_info *)&header->pak_info;
            pak = pak_info->pak;

            /* For output encryption, the encrypted packet is directly
             * send out to the output interface.
             */
            output = pak_info->hwidb;
            crypto_vec = (crypto_vector_t *)output->crypto_vectors;
            if (!crypto_vec) {
                /*
                 * At this time, the crypto vector could be removed
                 * as the result of removing crypto map from the interface.
                 * In this case, drop the packet and return.
                 */
                datagram_done(pak);
                return;
            }

            level = raise_interrupt_level(NETS_DISABLE);

            if (!crypto_copy_pak_to_particles(pak, &stuff_pak, 
                 pak_info->hwidb)) {
                 if (crypto_engine_vip_debug) {
                     buginf("\nCRYPTO_ENGINE: can not copy pak to particles");
                 };
                 datagram_done(pak);
                 reset_interrupt_level(level);
                 return;
            }


            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE: send out encrypted packet %x",
                       " to idb %x\n", pak, crypto_vec->media_fastsend);
                crypto_dump_particles(&stuff_pak, stuff_pak.datagramsize);
            }

            (*crypto_vec->media_fastsend)(output, &stuff_pak);
            crypto_init_stuff_pak(&stuff_pak);
            datagram_done(pak);
            reset_interrupt_level(level);


            break;

        case CRYPTO_DECRYPT_PACKET:

            pak_info = (crypto_vip_pak_info *)&header->pak_info;
            pak = pak_info->pak;

            crypto_vec = (crypto_vector_t *)pak_info->hwidb->crypto_vectors;
            if (!crypto_vec)  {
                datagram_done(pak);
                return;
            }

            level = raise_interrupt_level(NETS_DISABLE);

            if (!crypto_copy_pak_to_particles(pak, &stuff_pak, 
                 pak_info->hwidb)) {
                 if (crypto_engine_vip_debug) {
                     buginf("\nCRYPTO_ENGINE: can not copy pak to particles");
                 };
                 datagram_done(pak);
                 reset_interrupt_level(level);
                 return;
            }

            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE: send back decrypted packet "
                       "%x to vip_ip_flow_fs", pak); 
                crypto_dump_particles(&stuff_pak, stuff_pak.datagramsize);
            }

            if (!(* (pak_dfs_t)crypto_vec->media_pak_dfs)
                                         (pak_info->hwidb, &stuff_pak)) {
                crypto_cleanup_pak(&stuff_pak);
            }
            crypto_init_stuff_pak(&stuff_pak);
            datagram_done(pak);

            reset_interrupt_level(level);


            break;

        case CRYPTO_EXP:
            /*
             * Send the response back to RSP.
             */
            crypto_engine_send_key_response ((epa_crypto_blk*)rsp);
            break;

        default:
	    break;
    }
};

void 
crypto_ip_recompute_icmp_checksum(paktype *pak)
{
    iphdrtype *ip;
    icmptype *icmp;

    uchar *sum;

    ip = (iphdrtype *)ipheadstart(pak);
    icmp = (icmptype *)((char *)ip + ltob(ip->ihl));

    icmp->checksum=0; /* Get rid of old checksum! */
    icmp->checksum=ipcrc((ushort *)icmp, (n2h(ip->tl) - ltob(ip->ihl)));

    sum = (uchar *)&icmp->checksum;
}

/*
 * get_next_particle_fill_buffer
 *
 * Copy the rest of bytes (less than 8 bytes) from the current 
 * particle. Get the next particle from the particle queue of the given 
 * pak. Copy the number of bytes from the new particle to the rest of
 * the buffer. Return the new particle pointer, the data pointer which 
 * points to the end of data to be encrypt/decrypted, and also the
 * buffer with the clear data copied. 
 */ 
static inline epa_status get_next_particle_fill_buffer (
                                                  uchar *data1, ushort count1,
                                                  uchar *data2, ushort count2,
                                                  uchar *buffer)
{
    if ((count1 + count2) != EPA_DES_ENCDEC_BLOCK_LENGTH) {
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: block size error: %d", count1+count2);
        }
        return EPA_OP_FAIL;
    }

    bcopy(data1, buffer, count1);
    bcopy(data2, buffer+count1, count2);
    return EPA_OP_OK;
} 


void crypto_sw_vip_encrypt_decrypt (ushort act, crypto_encrypt_header *header)
{
    uchar buffer[EPA_DES_ENCDEC_BLOCK_LENGTH];
    crypto_vip_pak_info *pak_info=(crypto_vip_pak_info *)&header->pak_info;
    paktype *pak =pak_info->pak;
    ushort i, block_num, trailing_len, type;
    uchar  *data, *key;
    uchar encrypt_queue[EPA_DES_ENCDEC_BLOCK_LENGTH];
    ushort conn_id = header->conn_id;
    crypto_encrypt_decrypt_proc  action;

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
    crypto_iv_gen (encrypt_queue, header->iv_seed,header->iv_seed_size,
                   epa_swcrypto_conn_tbl[conn_id].iv_key);

    if (!pak_has_particles(pak)) {

        for (i=0; i<block_num; i++) {
             action(type, key, data, encrypt_queue,
                             EPA_DES_ENCDEC_BLOCK_LENGTH); 
             data += EPA_DES_ENCDEC_BLOCK_LENGTH;
        }
        /* Second, encrypt the last block which is less than 
         * 8 bytes, if exist.
         */
        if (trailing_len) {
            action(type,key,data,encrypt_queue,trailing_len); 
        }
    } else {
        /* This is a scattered pak. */
        particletype *particle, *new_particle=NULL;
        uchar *particle_data_end, *particle_data_start; 
        uchar *new_particle_data_start = NULL;
        uchar *new_mapped_data_start, *mapped_data_start;
        ushort count1=0, count2=0, status=0;
        boolean new_particle_flag = FALSE;

        /*
         * There are two pointers which always point to the current particle
         * and the data to be encrypted, in the current particle. First, the
         * two pointers about the first particle are initialized.
         */

        particle = pak->particlequeue.qhead;
        mapped_data_start = particle->data_start;
        CONVERT_PARTICLE_ADDR(mapped_data_start);
        particle_data_start = data;
        particle_data_end = mapped_data_start + particle->data_bytes - 1;

        for (i=0; i<block_num; i++) {
            if (particle_data_start + EPA_DES_ENCDEC_BLOCK_LENGTH -1 >
                particle_data_end) {
                count1 = particle_data_end - particle_data_start + 1;
                count2 = EPA_DES_ENCDEC_BLOCK_LENGTH - count1;
                new_particle = particle->next;
                new_mapped_data_start = new_particle->data_start;
                CONVERT_PARTICLE_ADDR(new_mapped_data_start);
                new_particle_data_start = new_mapped_data_start;
                status = get_next_particle_fill_buffer(particle_data_start,
                                                       count1,
                                                       new_particle_data_start,
                                                       count2,
                                                       buffer); 
                if (status != EPA_OP_OK)
                    return;

                data = &buffer[0];
                new_particle_flag = TRUE;
            }
            action(type, key, data, encrypt_queue,
                            EPA_DES_ENCDEC_BLOCK_LENGTH);

            if (new_particle_flag) {
                new_particle_flag = FALSE;
                bcopy(data, particle_data_start, count1);
                bcopy(data+count1, new_particle_data_start, count2);
                particle_data_start = new_particle_data_start + count2; 
                new_mapped_data_start = new_particle->data_start;
                CONVERT_PARTICLE_ADDR(new_mapped_data_start);
                particle_data_end = new_mapped_data_start + 
                                    new_particle->data_bytes - 1;
                particle = new_particle;
            } else {
                particle_data_start+= EPA_DES_ENCDEC_BLOCK_LENGTH;
            }
            data = particle_data_start;
        }

        if (trailing_len) {
            /* 
             * Either all the bytes are in the same particle, or 
             * in two seperate particles.
             */
            if (particle_data_start + trailing_len - 1 > particle_data_end) {
                count1 = particle_data_end - particle_data_start + 1;
                count2 = trailing_len - count1;
                new_particle = particle->next;
                new_mapped_data_start = new_particle->data_start;
                CONVERT_PARTICLE_ADDR(new_mapped_data_start);
                new_particle_data_start = new_mapped_data_start;
                status = get_next_particle_fill_buffer(particle_data_start,
                                                       count1,
                                                       new_particle_data_start,
                                                       count2,
                                                       buffer); 
                if (status != EPA_OP_OK)
                    return;

                data = &buffer[0];
                new_particle_flag = TRUE;
            }

            action(type, key, data, encrypt_queue, trailing_len);

            if (new_particle_flag) {
                bcopy(data, particle_data_start, count1);
                bcopy(data+count1, new_particle_data_start, count2);
            }
        }
    }
}

/*
 * crypto_engine_vip_free_pak
 *
 * Free the packet. Called by sw cryption process to drop the packet.
 * The interrupt is disabled when free particles and return the pak header.
 */
inline void crypto_engine_vip_free_pak (paktype *pak)
{

    if (crypto_engine_vip_debug) {
        buginf("\nCRYPTO_ENGINE: free the packet: %x", pak);
    }

    /* Should increase the counter here. */
    if (pak_has_particles(pak)) {
        particle_retbuffer(pak);
    } else {
        datagram_done (pak);
    }
}

static inline void crypto_init_stuff_pak (paktype *pak)
{
    pak->datagramstart = NULL;
    pak->datagramsize = 0;
    pak->network_start = NULL;
    pak->if_input = NULL;
    pak->if_output = NULL;
}

static inline void crypto_cleanup_pak (paktype *pak)
{
    particletype *mp;

    while (mp = particle_dequeue_inline(pak), mp) {
        retparticle(mp);
    }
    pak->datagramstart = NULL;
    pak->datagramsize = 0;
}
