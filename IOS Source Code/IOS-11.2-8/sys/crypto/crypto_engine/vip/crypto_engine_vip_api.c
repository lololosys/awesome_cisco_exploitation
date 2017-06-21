/* $Id: crypto_engine_vip_api.c,v 1.1.68.17 1996/09/10 11:13:37 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/vip/crypto_engine_vip_api.c,v $
 *------------------------------------------------------------------
 * crypto_engine_vip_api.c
 * 
 * This file defines the crypto engine api specific for VIP. 
 *
 * 11, 1995-1997, xliu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_engine_vip_api.c,v $
 * Revision 1.1.68.17  1996/09/10  11:13:37  xliu
 * CSCdi68600:  ESA should not be given the tralier bytes to encrypt
 * Pass the total length (encap len + ip->tl) to ESA driver.
 * Branch: California_branch
 *
 * Revision 1.1.68.16  1996/08/15  19:16:46  xliu
 * CSCdi65915:  crypto clear-latch does not work
 * Branch: California_branch
 * Check for EPA_XTRACT status bit.
 *
 * Revision 1.1.68.15  1996/08/04  20:08:01  xliu
 * CSCdi64861:  EPA/4T performance drops significantly
 * Branch: California_branch
 *
 * Revision 1.1.68.14  1996/07/28  11:17:15  xliu
 * CSCdi64306:  crypto engine memory aligment error on vip2
 * Branch: California_branch
 *
 * Revision 1.1.68.13  1996/07/22  07:38:57  xliu
 * CSCdi63492:  PPP does not work when switched from HDLC on EPA/VIP2
 * Branch: California_branch
 *
 * Revision 1.1.68.12  1996/07/15  09:59:22  xliu
 * CSCdi61651:  epa randome number seed is not initialized
 * Branch: California_branch
 *
 * Revision 1.1.68.11  1996/07/06  22:15:37  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 1.1.68.10  1996/07/06  19:45:23  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 1.1.68.9  1996/07/01  12:26:51  xliu
 * CSCdi61743:  decryption does not use cached pak pointer correctly.
 * Branch: California_branch
 *
 * Revision 1.1.68.8  1996/06/30  18:11:09  ptran
 * CSCdi61257:  snmp and show diag support for encryption PA
 * Branch: California_branch
 *
 * Revision 1.1.68.7  1996/06/21  22:32:45  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 1.1.68.6  1996/06/01  01:15:04  xliu
 * CSCdi59234:  crypto rsp flow vector does not work correct for vip
 * encryption
 * Branch: California_branch
 *
 * Revision 1.1.68.5  1996/05/28  16:00:08  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 1.1.68.4  1996/05/21  09:44:39  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 1.1.68.3  1996/05/12  23:13:34  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 1.1.68.2  1996/04/22  18:08:46  ptran
 * CSCdi55272:  Port asm_4k_vip_epad.S to california branch
 * Branch: California_branch
 * Port asm_4k_vip_epad.S to california.  Remove stub function
 * epad_get_particle
 * from crypto_engine_vip_api.c.
 *
 * Revision 1.1.68.1  1996/04/19  14:57:14  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.11  1996/04/17  23:48:30  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 1.1.2.10  1996/04/09  20:03:24  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 1.1.2.9  1996/03/30  05:12:33  xliu
 * Branch: IosSec_branch
 * Commit before CA repository split.
 *
 * Revision 1.1.2.8  1996/03/25  23:19:24  xliu
 * Branch: IosSec_branch
 * Checkin VIP crypto engine code.
 *
 * Revision 1.1.2.7  1996/02/13  19:16:51  carrel
 * Branch: IosSec_branch
 * Crypto Map checkin (finally)
 *
 * Revision 1.1.2.6  1996/01/30  22:36:18  xliu
 * Branch: IosSec_branch
 * Checking in encryption RSP/VIP support code.
 *
 * Revision 1.1.2.5  1996/01/19  07:01:11  xliu
 * Branch: IosSec_branch
 * Commit before the second sync.
 *
 * Revision 1.1.2.4  1996/01/09  02:26:52  xliu
 * Branch: IosSec_branch
 * VIP software encryption.
 *
 * Revision 1.1.2.3  1995/12/10  09:30:08  xliu
 * Branch: IosSec_branch
 * Merge the code to replace the idb fastsend using ipc.
 *
 * Revision 1.1.2.2  1995/12/02  17:43:00  xliu
 * Branch: IosSec_branch
 *
 * Modified epa driver interface.
 *
 * Revision 1.1.2.1  1995/11/30  05:39:52  xliu
 * Branch: IosSec_branch
 *
 * Files created for crypto engine vip.
 *
 * Revision 1.1  1995/11/30  05:23:32  xliu
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
#include "../wan/ppp.h"
#include "../ip/ip.h"
#include "../ipmulticast/igmp.h"
#include "../ip/ipaccess.h"
#include "../ip/ipaccount.h"
#include "../if/tring.h"
#include "../wan/frame_relay.h"
#include "../wan/atm_dxi.h"
#include "../wan/smds.h"

#include "../if/ether.h"
#include "../if/network.h"
#include "../if/fddi.h"
#include "../util/random.h"

#include "../src-vip/dfs_vip_idb.h"

#include "../ip/ipfast_index.h"
#include "../ip/ipfast_vip.h"
#include "../ip/ipfast_vip_ipc.h"
#include "../ip/ipfast_ipc.h"

#include "sys_registry.h"

#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_tcpip.h"
#include "../ip/ip_registry.h"

#include "../pas/if_pas.h"       /* includes pas_pci.h & segpak.h */
#include "../src-vip/vip_memd.h"
#include "../src-vip/vip_debug.h"
#include "../crypto/crypto.h"
#include "../crypto/crypto_engine_api.h"
#include "../crypto/crypto_map.h"
#include "../crypto/crypto_sesmgmt.h"
#include "../crypto/crypto_public.h"
#include "../crypto/crypto_registry.h"
#include "../crypto/crypto_pubkey.h"
#include "../crypto/crypto_map.h"
#include "../crypto/crypto_engine/crypto_lib/lib_toolkit.h"
#include "../crypto/crypto_engine/core/crypto_engine.h"
#include "../crypto/crypto_engine/core/crypto_swepa.h"
#include "../crypto/crypto_engine/crypto_lib/encrypt.h"
#include "../crypto/crypto_engine/crypto_lib/dss.h"
#include "crypto_engine_vip.h"
#include "connstup.h"

boolean vip_pak_dfs(hwidbtype *, paktype *);
ushort ip_cks_and_ttl_adjust_unaligned (iphdrtype *);
uint crypto_engine_check_decryption (paktype * pak, hwidbtype *input);

/*
 * Retrive number of bytes, according to the different protocol, from
 * the packet to be used as the IV seed. This IV seed is to be XORed with
 * the IV key by the epa firmware bofer used for encryption/decryption.
 */
inline void crypto_get_epa_iv (iphdrtype *ip, uchar *iv)
{
    ushort index;
    uchar *start;
    uchar iphdrbuffer[20];
    ushort iv_seed;
    ulong x;
    ushort i;

    switch (ip->prot) {
    case TCP_PROT:
        index = CRYPTO_TCP_PROT;
        start = (uchar *)ip + ipheadsize(ip) + 
                         crypto_iv_gen_tbl[index].ivgen_ofs;
        bcopy(start, iv, CRYPTO_TCP_IV_SIZE);
        return;
    case UDP_PROT:
        index = CRYPTO_UDP_PROT;
        start = (uchar*)ip + ipheadsize(ip) + 
                         crypto_iv_gen_tbl[index].ivgen_ofs;
        break;
    default:
        index = CRYPTO_DEFAULT_PROT;
        bcopy((uchar *)ip, iphdrbuffer, ipheadsize(ip));
        start = &iphdrbuffer[CRYPTO_DEFAULT_IV_OFFSET];
        break;
    }

    bcopy (start, &iv_seed, crypto_iv_gen_tbl[index].ivgen_siz);

    /* Expand it into 8 bytes */
    x = iv_seed;                        /* load the two byte iv seed value */
    x = (x & 0x0000ffffL) | (~x << 16); /* expand it into 32 bit value */

    for (i=0; i<8; i+=4)
       *((ulong*)&iv[i]) = x;           /* expand it into 64 bit value */
}

/*
 * crypto_create_encrypt_blk
 *
 * Create encryption control block by instantiating the control block  
 * fields using the crypto engine connection table, indexed by the connection 
 * id.
 */
static inline boolean crypto_create_encrypt_blk (paktype *pak,
                                              crypto_vip_conn_entry *conn,
                                              particletype *particle)
{
    crypto_encrypt_control_blk blk;
    hwidbtype *hwidb = NULL;
    ushort enctype_size;
    iphdrtype *ip = (iphdrtype *)ipheadstart(pak);
    int offset;

    /* Currently only the connection id is needed. Therefore we don't
     * use the connection table. Later we may need other information 
     * from connection table.
     */
    blk.session = conn->hma_session_id;
    blk.status = 0x00;
    memset(blk.reserved1, 0x00, 8);
    blk.total_length = 0;


    switch (pak->cryptoflags) {

        case CRYPTO_ENCRYPT:
            blk.command = CRYPTO_EPA_ENCRYPT;
            hwidb = pak->if_output->hwptr;

            break;

        case CRYPTO_DECRYPT:
            blk.command = CRYPTO_EPA_DECRYPT;
            hwidb = pak->if_input->hwptr;
            break;

        default:
            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE: wrong pak cryptoflags");
            }
            return FALSE;
    }

    if (!hwidb) {
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: null hwidb.");
        }
        return FALSE;
    }

    switch (hwidb->type) {
    case IDBTYPE_FDDIT:
       enctype_size = FDDI_SNAP_ENCAPBYTES; 
       break;
    case IDBTYPE_IBM2692:
       enctype_size = TRING_SNAP_ENCAPBYTES; 
       break;
    case IDBTYPE_AMDP2:
       enctype_size = ETHER_ARPA_ENCAPBYTES; 
       break;
    case IDBTYPE_S4T68360:
       if (hwidb->enctype == ET_HDLC) {
           enctype_size =  HDLC_ENCAPBYTES;
       } else {
           enctype_size = PPP_ENCAPBYTES;
       }
       break;
    default:
        /* This routine should never receive a packet whose idb type is */
        /* included in the case statments. If this ever happen, it is an */
        /* error.                                                        */
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: packets not supported for encryption.");
        }
        return FALSE;
       break;
    }

    crypto_get_epa_iv(ip,  blk.iv);
    crypto_set_encrypt_start(ip, &offset);

    blk.offset = enctype_size + offset;


    /* Set the totoal length for ESA driver */
    blk.total_length = enctype_size + ip->tl;

    bcopy (&blk, particle->data_start, sizeof(crypto_encrypt_control_blk));
    particle->data_bytes = sizeof(crypto_encrypt_control_blk);

    return TRUE;
}

/*
 * epa_encrypt_decrypt
 *
 * Prepare the packet to send to epa driver for encryption or decryption.
 * The particle allocated for encryption block should be freed by driver.
 * NOTE: with the phase2 epa firmware update, the control block is no 
 * longer in particle. A staticly allocated buffer will be passed to
 * the driver.
 */
static inline boolean epa_encrypt_decrypt (crypto_vip_pak_info *pak_info,
                                   ushort *status)
{
    particletype *mp = NULL;
    paktype *pak = NULL;
    short conn_id;

    if (!pak_info) {
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE_VIP: invalid input.\n");
        }
        return (FALSE);
    }

    pak = pak_info->pak;
    if (!pak) {
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: null pak pointer.");
        }
        return (FALSE);
    }

    if (pak->cryptoflags & CRYPTO_DECRYPT) {
	conn_id = pak->crypto_dec_conn_id;
    } else {
	conn_id = pak->crypto_enc_conn_id;
    }
	
    /* Drop the packet if the connection is pending. */
    if (conn_id <=0 || crypto_vip_conntbl[conn_id].state !=
                              CRYPTO_EPA_SESSION_SET) {
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: conn pending. pak %x dropped", pak);
            crypto_dump_particles(pak, pak->datagramsize);
        }
        if (pak_has_particles(pak)) {
            while ((mp = dequeue_inline(&pak->particlequeue))) {
                retparticle(mp);
            }
        } else {
            datagram_done(pak);
        }
        return FALSE;
    }

    /* 
     * If this is the packet for decryption, convert the pak pointers to
     * its uncached address since the pak pointer is to be saved by the driver
     * and reused after the packet is decrypted.
     */
    if (pak->cryptoflags & CRYPTO_DECRYPT) {
        pak->datagramstart = (uchar *)((ulong)(pak->datagramstart) & 
                         ~PCIMEM_CWT_OFS); 
        pak->network_start = (uchar *)((ulong)(pak->network_start) & 
                         ~PCIMEM_CWT_OFS); 
    }
        
    mp = getparticle_inline(crypto_engine_vip_config.epa_encrypt_idb->pool, 
                            crypto_engine_vip_config.epa_encrypt_idb->pool->size,
                                  TRUE);
    if (!mp) {
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE_VIP:  can not allocate mp.\n");
        }
        if (pak_has_particles(pak)) {
            particle_retbuffer(pak);
        } else {
            datagram_done(pak);
        }
        return (FALSE);
    }
  
    /* Create encryption control block */
    if (!crypto_create_encrypt_blk (pak, &crypto_vip_conntbl[conn_id], mp)) {
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: can not create control blk.");
        }
        if (pak_has_particles(pak)) {
            particle_retbuffer(pak);
            retparticle_inline(mp);
        } else {
            datagram_done(pak);
        }
        return FALSE;
    }

    /* Note:
     * Currently we use pak->unspecifiedfield[0] to pass the pointer 
     * between crypto engine and the driver. Should find a better way
     * later.
     */
    pak_info->pak->unspecified[0] =(ulong)mp;

    if (crypto_engine_vip_debug) {
        buginf("\nBEFORE %s ", 
              pak->cryptoflags&CRYPTO_ENCRYPT?"encrypt":"decrypt");
        crypto_dump_particles(pak, pak->datagramsize);
    }

    /*
     * Update the connection table.
     */
    if (pak->cryptoflags & CRYPTO_DECRYPT)
        crypto_vip_conntbl[pak->crypto_dec_conn_id].decrypt_count++;
    else
        crypto_vip_conntbl[pak->crypto_enc_conn_id].encrypt_count++;

    /* Pass the packet to the driver */
    (*crypto_engine_vip_config.epa_encrypt_idb->fastsend)
         (crypto_engine_vip_config.epa_encrypt_idb, pak_info->pak);
   
    return TRUE;
}

/*
 * crypto_reparent_pak
 *
 * This is called by sw encryption when enqueuing the paks to the
 * encryption process.
 */
static inline paktype* crypto_reparent_pak (paktype *old, hwidbtype *idb)
{
    paktype *new;

    new = particle_reparent(old, idb->headerpool); 
    if (!new) {
        buginf("CRYPTO_ENGINE: could not reparent pak.\n");
        /*here we should add code to clean particles and add drop counts.*/
        return new;
    }

    /* Move in the fields which are not moved by particle_reparent */
    new->if_output = old->if_output;
    new->if_input = old->if_input;
    new->enctype = old->enctype;
    new->network_start = old->network_start;
    new->cryptoflags = old->cryptoflags;
    new->crypto_enc_conn_id = old->crypto_enc_conn_id;
    new->crypto_dec_conn_id = old->crypto_dec_conn_id;

    return new;
}

extern void bugtrace(void);

/*
 * sw_encrypt_decrypt
 *
 * This is the software encrypiton vector. Called at interrupt level.
 */
static inline boolean sw_encrypt_decrypt (ushort command_id,
                                       crypto_vip_pak_info *pak_info,
                                       ushort *status)
{
    paktype *pak = pak_info->pak;
    iphdrtype *ip;
    crypto_encrypt_header  header;
    paktype  *new_pak;
    int      crypto_offset;
    int      conn_id;
   
    if (pak->cryptoflags & CRYPTO_DECRYPT) {
    	conn_id = pak->crypto_dec_conn_id;
    } else {
    	conn_id = pak->crypto_enc_conn_id;
    }

    if (crypto_engine_vip_debug) {
        buginf ("\nCRYPTO_ENGINE_VIP: %s\n",
                pak->cryptoflags & CRYPTO_ENCRYPT? "encrypt":"decrypt");
        crypto_dump_particles(pak, pak->datagramsize);
    }

    if (conn_id <= 0 || epa_swcrypto_conn_tbl[conn_id].state != 
                                            EPA_SWCRYPTO_CONN_SET) {
        datagram_done(pak);
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE: packets dropped: state = %d",
                   epa_swcrypto_conn_tbl[conn_id].state);
         }
        return (FALSE);
    }

    /*
     * Convert the pak pointers to its uncached value.
     */
    if (pak->cryptoflags & CRYPTO_DECRYPT) {
        pak->datagramstart = (uchar *)((ulong)(pak->datagramstart) & 
                             ~PCIMEM_CWT_OFS); 
        pak->network_start = (uchar *)((ulong)(pak->network_start) & 
                             ~PCIMEM_CWT_OFS); 
    }

    /*
     * For VIP2, only scattered packets are there.
     */
    if (pak_has_particles(pak)) {
        new_pak = getbuffer(pak->datagramsize);
        if (!new_pak) {
           if (crypto_engine_vip_debug) {
               buginf("\nCRYPTO_ENGINE: could not allocate pak.");
           }
           datagram_done(pak);
           return (FALSE);
        }
        new_pak = pak_coalesce(pak, new_pak);
        if (!new_pak) {
            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE: can no copy the scatter pak to dram");
            }
            datagram_done(new_pak);
            return(FALSE);
        }
        new_pak->refcount = 1; 
        new_pak->enctype = pak->enctype;
        new_pak->linktype = pak->linktype;
        new_pak->if_input = pak->if_input;
        new_pak->if_output = pak->if_output;
	new_pak->flags = pak->flags;
	new_pak->traffic_shape = pak->traffic_shape;
        new_pak->network_start = new_pak->datagramstart + 
                                 (pak->network_start - pak->datagramstart);
        new_pak->mci_status = pak->mci_status;
        datagram_done(pak);
        pak = new_pak;
    } 

    /* 
     * Need to release the static pak passed here when porting to other
     * paltform.
     */

    pak_info->pak = pak;

    ip = (iphdrtype *)ipheadstart(pak);
    header.command = command_id; 
    header.conn_id = conn_id;
    crypto_get_iv_seed(ip, header.iv_seed, &header.iv_seed_size);
    crypto_set_encrypt_start(ip, &crypto_offset);

    header.data = ipheadstart(pak) + crypto_offset;
    header.data_size = ip->tl - crypto_offset;
    header.switch_mode = CRYPTO_VIP_FAST_SWITCH;
    header.status = 0;

    if ((header.data + header.data_size) > (pak->datagramstart + 
         pak->datagramsize)) {
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_ENGINE_VIP: invalid data size. Pakcet dropped.");
        }
        datagram_done(pak);
        return TRUE;
    }

    bcopy(pak_info, &header.pak_info, sizeof(crypto_vip_pak_info));

    crypto_engine_fast_encrypt_enqueue(&crypto_engine_idb, &header);

    return TRUE;
}

/*
 * crypto_vip_sw_encrypt
 *
 * Called by crypto_engine_fastsend to pass the packet to software 
 * encryption process. After encryption is done, the crypto engine will
 * pass the packet to the media fastsend vector to send the packet out.
 */
boolean crypto_vip_sw_encrypt (void *pak_info,
			    ushort *status)
{
    return(sw_encrypt_decrypt(CRYPTO_ENCRYPT_PACKET,
                       (crypto_vip_pak_info*)pak_info, status));
}

/*
 * crypto_vip_epa_encrypt
 *
 * Called by crypto_engine_fastsend to pass the packet to epa driver
 * for encryption. After encryption, the driver will pass the packet
 * to the media driver.
 */

boolean crypto_vip_epa_encrypt (void *pak_info, ushort *status)
{
   return(epa_encrypt_decrypt((crypto_vip_pak_info *)pak_info, status));
}

boolean crypto_vip_epa_decrypt (void *pak_info, ushort *status)
{
   return(epa_encrypt_decrypt((crypto_vip_pak_info *)pak_info, status));
}

/*
 * crypto_vip_sw_decrypt
 *
 * Pass the packet for software decryption. After the decryption, the packet is
 * passed to vip_ip_flow_fs to continue the flow switching. This is called by
 * crypto_engine_vip_decrypt which is the vector called by vip_ip_flow_fs. 
 */

boolean crypto_vip_sw_decrypt(void *pak_info, ushort *status)
{
    return(sw_encrypt_decrypt(CRYPTO_DECRYPT_PACKET, 
                       (crypto_vip_pak_info *)pak_info, status));
}

/*
 * crypto_engine_vip_pak_dfs
 *
 * This is the routine assinged to idb->crypto_vectors->vip_flow_decrypt.
 * It dispatches the packet either to sw decryption or to epa driver for
 * hw decryption according to the configuration.
 */ 
boolean crypto_engine_vip_pak_dfs (hwidbtype *input, paktype *pak) 
{
    crypto_vector_t *crypto_vec;
    iphdrtype *ip;
    ushort status;
    pas_cbtype *pascb = INSTANCE(input);
    icmptype *icmp;
    crypto_pak_info pak_info;

    if (crypto_engine_vip_debug) {
        buginf("\nCRYPTO_ENGINE: received packet: %x", pak);
        crypto_dump_particles(pak, pak->datagramsize);
    }

    crypto_vec = (crypto_vector_t *)input->crypto_vectors;
    if (!crypto_vec) {
        return (FALSE);
    }

    /*
     * If this is connection set up message, send the message back to
     * RSP.
     */
    ip = (iphdrtype *)ipheadstart(pak);
    icmp = (icmptype *)((char *)ip + ltob(ip->ihl));
    if (((icmp->type == ICMP_ECHOREPLY) || (icmp->type == ICMP_ECHO)) &&
	icmp->code == CRYPTO_ICMP_CODE) {
        return((*pascb->pak_to_host)(input, pascb, pak));
    }

    status = crypto_engine_check_decryption(pak, input);
    switch( status) {
    case CRYPTO_VIP_DECRYPT: 
        break;

    case CRYPTO_VIP_DECRYPT_PENDING:
    case CRYPTO_VIP_CONN_SETUP:
        return((*pascb->pak_to_host)(input, pascb, pak));
        return TRUE;

    case CRYPTO_VIP_FRAGMENT:
        /*
         * Input fragment needs decryption. Until we
         * support fragment reassembly on the vip for
         * crypto, have to pass this to the rsp.
         */
        if (crypto_engine_vip_debug)
            buginf("\nCRYPTO_ENGINE: input fragment -> rsp");
        return((*pascb->pak_to_host)(input, pascb, pak));

    default:
        /* 
         * For any other case it will fall through the flow switching
         * code.
         */
        if (crypto_engine_vip_debug) {
            buginf("\nFLOW:pass the packet %x to flow switch", pak);
        }
        crypto_vec = (crypto_vector_t*)input->crypto_vectors;
        if (!crypto_vec || !crypto_vec->media_pak_dfs) {
           datagram_done(pak);
           return (FALSE);
        }
        return((* ((pak_dfs_t)crypto_vec->media_pak_dfs))(input, pak));
    }

    if (crypto_engine_vip_debug) {
        buginf("\nCRYPTO_ENGINE: decrypt the packet: %x\n", pak);
    }

    /* We need to set input hwidb since it is used to interface with
     * flow switch after driver return the decrypted packet.
     */
    pak->if_input = input->firstsw;
    pak_info.pak  = pak;
    pak_info.hwidb = input; 
   
    /*
     * Here we return TRUE/FALSE to the driver. Remember, return TRUE
     * means the packets are switched by pak_dfs. Return FALSE means
     * the packet is not resumed by the switch routine and the driver 
     * will release all the particles.
     */ 
    (*crypto_engine_idb.vip_decrypt)((void *)&pak_info, &status);

    return TRUE;
}

/* 
 * crypto_fastsend_lookup
 * 
 * Needed by epa driver. 
 */
inline void* crypto_fastsend_lookup (hwidbtype *idb)
{
    if (!idb->crypto_vectors) {
        if (crypto_engine_vip_debug) {
            buginf( "CRYPTO_ENGINE_VIP: crypto vectors not initialized.\n");
        }
        return NULL;
    }

    return ((void *)(((crypto_vector_t *)idb->crypto_vectors)->media_fastsend));
}

/*
 * crypto_engine_check_decryption
 *
 * Check if the packet should be decrypted using crypto_map. Return 
 * CRYPTO_VIP_DECRYPT_FLOW if the packet should be decrypted and the 
 * connection has been set up. Return CRYPTO_VIP_DECRYPT_PENDING is the
 * packet should be decrypted and the connection has not been set up. 
 * Return CRYPTO_VIP_NO_DECRYPT if the packet should not be decrypted.
 * 
 */
uint crypto_engine_check_decryption (paktype * pak, hwidbtype *input)
{
    iphdrtype *ip;
    uchar tcp_flags;
    ipflow_key_t flkey;
    ipflow_t *flow;

    ip = (iphdrtype *)ipheadstart(pak);
    tcp_flags = ipflow_key_get(ip, &flkey);
    flkey.fl_input = input->hw_if_index;
  
    flow = ipflow_hash_lookup(&flkey, tcp_flags);
    if (flow ) {
       if ( flow->crypto_tag.cryptoflags == CRYPTO_DECRYPT) {
            pak->cryptoflags = flow->crypto_tag.cryptoflags; 
            pak->crypto_dec_conn_id = flow->crypto_tag.crypto_conn_id;
            if (get_ip_morefragments(ip) || (getfo(ip) != 0)) {
                /* special handling for fragments */
                return(CRYPTO_VIP_FRAGMENT);
            }
            return(CRYPTO_VIP_DECRYPT);
       } else {
            return (CRYPTO_VIP_NO_DECRYPT);
       }
    }

    pak->cryptoflags = 0;
    pak->crypto_dec_conn_id = 0;
    crypto_map_check(pak, input->firstsw->crypto_map, CRYPTO_DECRYPT, TRUE);
  
    if (crypto_engine_vip_debug) {
        buginf("\nCRYPTO_ENGINE: after map check: flags = %x, dec conn_id %d", 
		pak->cryptoflags, pak->crypto_dec_conn_id);
    }

    if (pak->cryptoflags & CRYPTO_DECRYPT) {
        if  (pak->crypto_dec_conn_id > 0) {
            if (get_ip_morefragments(ip) || (getfo(ip) != 0)) {
                /* special handling for fragments */
                return(CRYPTO_VIP_FRAGMENT);
            }
            return CRYPTO_VIP_DECRYPT;
        } else {
            return CRYPTO_VIP_DECRYPT_PENDING;
        }
    }
    return CRYPTO_VIP_NO_DECRYPT;
}

boolean crypto_vip_ip_encaps_generic (paktype *pak, encapinfotype *eptr,
                                      hwidbtype *input, void *output,
                                      int input_encap_len,
                                      ulong iplen, void *flow)
{

    crypto_vip_pak_info pak_info;
    ushort status;
    
    pak_info.hwidb = input;
    pak_info.pak = pak;
    pak_info.eptr = eptr;
    pak_info.encap_len = input_encap_len;
    pak_info.iplen = iplen;
    pak_info.output = output;
    pak_info.flow = (ipflow_t *)flow;

    return((*crypto_engine_idb.vip_decrypt)((void *)&pak_info, &status));
}
    
void crypto_dump_particles (paktype *pak, ushort size)
{
    particletype *mp;
    ushort count, i, j = 0;
    uchar *data;

    mp = pak->particlequeue.qhead;

    while (mp && size > 0) {
        data = mp->data_start;
        i = j = 0;
        count = 0; 
        buginf("\n");
        while (count < mp->data_bytes && count < size) {
            buginf("%02x ", data[i]);
            i++; count ++; j++;
            if (j>= 20) {
                j = 0;
                buginf("\n");
            }
        }
        size -= count;
        if (size == 0)
            break;
        mp = mp->next;
    }
}



