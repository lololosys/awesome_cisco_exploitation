/* $Id: crypto_swepa_rsp.c,v 1.1.70.20 1996/09/15 15:44:30 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/rpsp/crypto_swepa_rsp.c,v $
 *------------------------------------------------------------------
 * crypto_swepa_rsp.c 
 * 
 * This file defines crypto engine componet common to RSP platform. 
 *
 * 11, 1995, xliu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_swepa_rsp.c,v $
 * Revision 1.1.70.20  1996/09/15  15:44:30  xliu
 * CSCdi69200:  RSP crypto image  crashed when dram buffer allocation
 * return NULL
 * Branch: California_branch
 * Removed the duplicated memd buffer return from crypto_rsp_create_pak
 * when
 * the getbuffer return NULL.
 *
 * Revision 1.1.70.19  1996/09/10  14:14:44  sbelair
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
 * Revision 1.1.70.18  1996/09/09  17:29:01  xliu
 * CSCdi68379:  rsp_fs_free_memd_pak called twice on same pak in
 * crypto_swepa_rsp.c
 * Branch: California_branch
 *
 * Revision 1.1.70.17  1996/08/28  12:39:56  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 1.1.70.16  1996/08/22  01:25:04  xliu
 * CSCdi64300:  Need to investigate possible RSP memd buffer corruption
 * Branch: California_branch
 * Added comments. Opened a seperate bug to investigate why cached dram
 * copy
 * cause data corruption in crypto code path. Currently using uncached
 * dram copy
 * provided a temp. work around.
 *
 * Revision 1.1.70.15  1996/08/16  22:21:18  xliu
 * CSCdi64300:  Need to investigate possible RSP memd buffer corruption
 * Branch: California_branch
 * Use uncached copy to copy packets from memd to dram.
 *
 * Revision 1.1.70.14  1996/08/09  23:31:21  xliu
 * CSCdi63854:  Traffic is pass in clear text when distributed switching
 * is off
 * Branch: California_branch
 *
 * Revision 1.1.70.13  1996/07/30  19:00:03  bew
 * CSCdi62579:  clear crypto conn uses arbitrary slot# if slot# was not
 * entered
 * Branch: California_branch
 * Change method of determining slot.
 *
 * Revision 1.1.70.12  1996/07/28  07:44:19  xliu
 * CSCdi62947:  Add parser command to query epa status from RSP
 * This fix also solve the problems reported by CSCdi63488, CSCdi63852.
 * Branch: California_branch
 *
 * Revision 1.1.70.11  1996/07/15  10:06:54  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 1.1.70.10  1996/07/06  05:52:38  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 1.1.70.9  1996/07/03  05:55:03  snijsure
 * CSCdi60711:  sh cr conn eng act does not show anything in vip2 crypto
 * router
 * Branch: California_branch
 *
 * Revision 1.1.70.8  1996/07/01  12:26:39  xliu
 * CSCdi61743:  decryption does not use cached pak pointer correctly.
 * Branch: California_branch
 *
 * Revision 1.1.70.7  1996/06/17  22:44:39  xliu
 * CSCdi60489:  Crypto RSP crashed upon connection initiation
 * RSP crypto vector should be assinged after RP vector.
 * Branch: California_branch
 *
 * Revision 1.1.70.6  1996/06/07  00:26:47  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Change iv offset and encryption point if the IP payload is too small
 * (e.g., a fragment).
 *
 * Revision 1.1.70.5  1996/06/01  01:14:46  xliu
 * CSCdi59234:  crypto rsp flow vector does not work correct for vip
 * encryption
 * Branch: California_branch
 *
 * Revision 1.1.70.4  1996/05/28  16:00:01  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 1.1.70.3  1996/05/21  09:44:36  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 1.1.70.2  1996/05/12  23:13:17  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 1.1.70.1  1996/04/19  14:57:09  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.4.18  1996/04/17  23:48:19  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 1.1.4.17  1996/04/09  20:03:14  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 1.1.4.16  1996/03/30  05:12:20  xliu
 * Branch: IosSec_branch
 * Commit before CA repository split.
 *
 * Revision 1.1.4.15  1996/03/27  01:52:53  xliu
 * Branch: IosSec_branch
 *
 * Revision 1.1.4.14  1996/03/25  23:31:31  xliu
 * Branch: IosSec_branch
 * Fix software encryption/decryption output problem.
 *
 * Revision 1.1.4.13  1996/03/23  01:14:25  xliu
 * Branch: IosSec_branch
 * Return IPFLOWERR_NOCACHE, instead of FALSE when passing cs msg to
 * process
 * switch.
 *
 * Revision 1.1.4.12  1996/03/22  17:40:33  xliu
 * Branch: IosSec_branch
 * Add ipcache update to crypto map.
 *
 * Revision 1.1.4.11  1996/03/21  21:17:09  xliu
 * Branch: IosSec_branch
 * Add registry call to download crypto map.
 *
 * Revision 1.1.4.10  1996/03/21  10:08:45  xliu
 * Branch: IosSec_branch
 * Integrated with RSP changes.
 *
 * Revision 1.1.4.9  1996/02/27  06:42:21  xliu
 * Branch: IosSec_branch
 * Fixed compiler errors after sync.
 *
 * Revision 1.1.4.8  1996/02/13  19:16:42  carrel
 * Branch: IosSec_branch
 * Crypto Map checkin (finally)
 *
 * Revision 1.1.4.7  1996/01/30  22:36:09  xliu
 * Branch: IosSec_branch
 * Checking in encryption RSP/VIP support code.
 *
 * Revision 1.1.4.6  1996/01/19  07:01:06  xliu
 * Branch: IosSec_branch
 * Commit before the second sync.
 *
 * Revision 1.1.4.5  1996/01/09  08:51:46  xliu
 * Branch: IosSec_branch
 * Moved from crypto/crypto_engine/rpsp dir to crypto dir.
 *
 * Revision 1.1.4.4  1996/01/09  07:21:57  xliu
 * Branch: IosSec_branch
 * Crypto engine RSP.
 *
 * Revision 1.1.4.3  1996/01/09  01:12:16  xliu
 * Branch: IosSec_branch
 * RSP flow switch support.
 *
 * Revision 1.1.4.2  1995/12/10  09:28:52  xliu
 * Branch: IosSec_branch
 * Moved encryption from datagram_out to ipsendnet, after fragmentation.
 *
 * Revision 1.1.4.1  1995/11/30  05:42:32  xliu
 * Branch: IosSec_branch
 *
 * Files created for crypto engine rsp.
 *
 * Revision 1.1  1995/11/30  05:13:10  xliu
 * Place holder files.
 *
 * Revision 1.7  1995/06/07 12:03:03  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "logger.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "../h/fastswitch_registry.regc"
#include "media_registry.h"
#include "subsys.h"
#include "sched.h"
#include "packet.h"
#include "../if/ether.h"
#include "../if/fddi.h"
#include "config.h"
#include "ttysrv.h"
#include "parser.h"
#include "exception.h"
#include "../os/free.h"
#include "../os/pool.h"
#include "../os/buffers.h"
#include "../os/chunk.h"
#include "../wan/serial.h"
#include "../ip/ip.h"
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../ipmulticast/igmp.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "../ip/ipaccount.h"
#include "../ip/ipfast.h"
#include "../ip/ipfast_index.h"
#include "../if/tring.h"
#include "../les/if_les.h"
#include "../wan/frame_relay.h"
#include "../wan/atm_dxi.h"
#include "../wan/smds.h"
#include "../if/network.h"
#include "../if/atm.h"
#include "../util/radix.h"
#include "stdarg.h"

#include "../src-rsp/rsp_bufhdr.h"
#include "../src-rsp/rsp_if.h"
#include "../src-rsp/rsp_chasint.h"
#include "../src-rsp/rsp_init.h"
#include "../src-rsp/rsp_qasic.h"
#include "../src-rsp/qa_hw.h"
#include "../src-rsp/rsp_fast.h"
#include "../src-rsp/rsp_pcmap.h"
#include "../src-rsp/rsp_cache.h"
#include "../ip/ipfast.h"
#include "../ip/ipfast_flow.h"
#include "../ip/ipfast_rsp.h"
#include "../ip/ipfast_rsp_ipc.h"

#include "sys_registry.h"

#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_tcpip.h"
#include "../ip/ip_registry.h"

#include "../crypto/crypto_registry.h"
#include "../crypto/crypto_public.h"
#include "../crypto/crypto_engine/crypto_lib/lib_toolkit.h"
#include "../crypto/crypto.h"
#include "../crypto/crypto_map.h"
#include "../crypto/crypto_engine_api.h"
#include "../crypto/crypto_engine/core/crypto_engine.h"
#include "../crypto/crypto_engine/core/crypto_swepa.h"
#include "../crypto/crypto_sesmgmt.h"
#include "../crypto/crypto_ip.h"
#include "../crypto/crypto_map.h"
#include "../crypto/crypto_util.h"
#include "../ui/debug.h"
#include "../crypto/crypto_debug.h"
#include "crypto_swepa_common.h"
#include "crypto_engine_rsp.h"

#include "../hes/if_fci.h"
#include "../hes/ucode.h"
#include "../ip/ipfast_ipc.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"
#include "../hes/dbus.h"

/* Global for crypto engine on RSP */
chunk_type *crypto_conn_idb_chunk;

/*
 * crypto_replicate_pak
 *
 * This is the same routine as rsp_replicate_pak, except instead of 
 * allocate buffer from rsp buffer pool, the buffer is allocated from
 * system buffer.
 */
paktype *crypto_replicate_pak (paktype *pak) 
{
    paktype	*newpak;
    uchar	*newstart;
    int		size;
    idbtype     *output;
    hwidbtype   *hwidb;
    int		 offset;

    /*
     * If a packet has been decrypted, and then now is
     * being re-encrypted the pak could already have been
     * replicated to a DRAM pak. If so, just return the
     * same pak pointer.
     */
    if (!pak->buffer_header) {
	return pak;
    }

    output = pak->if_output;
    hwidb = hwidb_or_null(output);
    size = pak->datagramsize;

    newpak = getbuffer(size);
    if (!newpak) {
	rsp_fs_free_memd_pak(pak);
	hwidb->counters.output_total_drops++;
	return NULL;
    }

    /*
     * Set up the new packet structure:
     */
    offset = pak->network_start - pak->datagramstart;
    newstart = newpak->datagramstart - offset;
    newpak->datagramstart = newstart;
    newpak->datagramsize = size;
    newpak->rxtype = pak->rxtype;
    newpak->linktype = pak->linktype;
    newpak->if_input = pak->if_input;
    newpak->if_output = pak->if_output;
    newpak->flags = pak->flags;
    newpak->traffic_shape = pak->traffic_shape;
    newpak->cryptoflags = pak->cryptoflags;
    newpak->crypto_enc_conn_id = pak->crypto_enc_conn_id;
    newpak->crypto_dec_conn_id = pak->crypto_dec_conn_id;
    newpak->mac_start = 
	newstart + (pak->mac_start - pak->datagramstart);

    newpak->buffer_flags |= pak->buffer_flags & PAK_BUF_ACCT_DONE;
    pak->buffer_flags &= ~PAK_BUF_ACCT_DONE;

    bcopy(pak->datagramstart, newstart, size);

    /*
     * Free the input MEMD buffer:
     */
    rsp_fs_free_memd_pak(pak);

    return newpak;
}


/*
 * crypto_rsp_create_pak
 *
 * Allocate a pak in the system buffer and copy a packet from MEMD
 * to the pak. This is called by crypto_rsp_flow_encrypt_decrypt at
 * interrupt level, before the pak is enqueued to the encryption 
 * process.
 */
paktype *crypto_rsp_create_pak(bufhdr_t *bufptr, void *ip, 
                               ipcache_rntype *cptr)
{
    paktype     *newpak;
    ulong       size = bufptr->bf_tx_bytes;
    hwidbtype   *hwidb = cptr->hwidb; 
    uchar       *memdstart;
    int         offset;
    uchar       *ip_header = ip;

    /*
     * Packet in MEMD.
     * Copy packet out of MEMD into a system buffer in DRAM.
     */
    newpak = getbuffer(size);
    if (!newpak) {
        hwidb->counters.output_total_drops++;
        return NULL;
    }
    newpak->next = NULL;

    /*
     * Set up the new packet structure:
     */
    offset = ADDR_TO_MEMD_OFFSET_24(ip_header) - bufptr->bf_tx_ptr;
    newpak->datagramstart -= offset;
    newpak->datagramsize = size;
    memdstart = MEMD_OFFSET_TO_UNCACHED_ADDR(bufptr->bf_tx_ptr);
    newpak->network_start = newpak->datagramstart + offset;
    bcopy(memdstart, newpak->datagramstart, size);
    invalidate_data_inline(memdstart, size);

    /* 
     * Because the crypto vector is called before the cache writethrough 
     * are invalidated by the rsp flow/optimum switch routine,
     * we need to invalidate two cache lines in the writethrough here.
     */
    invalidate_data_inline((uchar *)ip, CRYPTO_WRITETHROUGH_CACHE_LINES);
    retbufhdr_inline(bufptr);

    newpak->refcount = 1;
    newpak->if_output = cptr->hwidb->firstsw; 
    newpak->rxtype = RXTYPE_DODIP;
    newpak->linktype = LINK_IP;

    return newpak;
}

/*
 * crypto_encrypt_rsp_flow_output
 *
 * Called by the crypto engine after the software encryption is done. The 
 * packet is in memd buffer. Call media output encapsulation vector to encap 
 * and send out. The media encap vector is saved in idb->crypto_vectors.
 */

static inline void
crypto_encrypt_rsp_flow_output (crypto_encrypt_header *header)
{
    crypto_rsp_flow_info *flow_info;
   
    flow_info = (crypto_rsp_flow_info *)&header->pak_info;
    datagram_out(flow_info->pak);
}

/*
 * crypto_decrypt_rsp_flow_output
 */
static inline void
crypto_decrypt_rsp_flow_output (crypto_encrypt_header *header)
{
    crypto_rsp_flow_info *flow_info;
    
    flow_info = (crypto_rsp_flow_info *)&header->pak_info;
    datagram_out(flow_info->pak);
}

/*
 * crypto_swepa_rsp_output
 *
 * Called by encryption process to send out a encrypted/decrypted
 * packet.
 */

void crypto_swepa_rsp_output (ushort type, void  *rsp)
{
    crypto_encrypt_header* header;

    header = (crypto_encrypt_header *)rsp;

    switch (type) {
    case CRYPTO_ENCRYPT_PACKET:
	switch (header->switch_mode) {
	case CRYPTO_PROCESS_SWITCH:
	    crypto_encrypt_ps_output(header);
	    break;

	case CRYPTO_FAST_SWITCH:
	    crypto_encrypt_fs_output(header);
	    break;

	case CRYPTO_RSP_FLOW_SWITCH:
	    crypto_encrypt_rsp_flow_output(header);
	    break;
 
        default:
            crypto_drop_packet(header);
            if (crypto_engine_debug) {
                buginf("\nCRYPTO_ENGINE: packet dropped because of "
                       "wrong switching mode");
            }
            break;
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

	case CRYPTO_RSP_FLOW_SWITCH:
	    /*
	     * The flow should pass fs cache again to find the
	     * output interface.
	     */
	    crypto_decrypt_rsp_flow_output(header);
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
            break;
	}
	break;

    case CRYPTO_EXP:
	/* Call epa_engine_rsp_disp */
	epa_engine_rsp_dspt ((epa_crypto_blk*)rsp);
	break;

    default:
        if (crypto_engine_debug) {
	    buginf("\nCRYPTO ENGINE: unknown command id.");
        }
	break;
    }
}

/*
 * crypto_encrypt_rsp_flow
 *
 * First copy the packet from MEMD to DRAM. Then enqueue the packet to
 * the encryption process.
 */
static inline void
crypto_engine_encrypt_rsp_flow (bufhdr_t *bufptr,
                                iphdrtype *ip,
                                ipcache_rntype *cptr,
                                ulong iplen,
                                short conn_id)
{
    crypto_rsp_flow_info *flow_info;
    crypto_pak_info   pak_info;
    uchar iv_seed[CRYPTO_MAX_IV_SIZE];
    ushort iv_seed_size;
    iphdrtype *newip = NULL;
    paktype *pak;
    int crypto_offset;

    (* cptr->c.encaps)(bufptr, ip, cptr, iplen);
    pak = crypto_rsp_create_pak(bufptr, ip, cptr);
    if (!pak) {
        if (crypto_engine_debug) {
            buginf("\nCRYPTO_ENGINE: can not allocate pak in dram.");
        }
        retbufhdr_inline(bufptr);
        return;
    }

    newip = (iphdrtype *)ipheadstart(pak);
    pak->cryptoflags = CRYPTO_ENCRYPT;

    flow_info = (crypto_rsp_flow_info *)&pak_info;
    flow_info->switch_mode = CRYPTO_RSP_FLOW_SWITCH;
    flow_info->pak = pak;
    flow_info->hwidb = cptr->hwidb;
    flow_info->ip = newip;
    crypto_get_iv_seed(newip, &iv_seed[0], &iv_seed_size);
    crypto_set_encrypt_start(newip, &crypto_offset);

    epa_encrypt_packet(
         (uchar*)newip + crypto_offset,
          iplen - crypto_offset,
          conn_id, &iv_seed[0], iv_seed_size, &pak_info);
}

/*
 * crypto_engine_decrypt_rsp_flow
 *
 * First copy the packet from MEMD to DRAM. Then enqueue the packet to
 * the encryption process.
 */
static inline 
void crypto_engine_decrypt_rsp_flow (bufhdr_t *bufptr,
                                     iphdrtype *ip,
                                     ipcache_rntype *cptr,
                                     ulong iplen,
                                     short  conn_id)
{
    crypto_rsp_flow_info *flow_info;
    crypto_pak_info      pak_info;
    uchar iv_seed[CRYPTO_MAX_IV_SIZE];
    ushort iv_seed_size;
    paktype *pak;
    iphdrtype *newip = NULL;
    int crypto_offset;

    (* cptr->c.encaps)(bufptr, ip, cptr, iplen);

    pak = crypto_rsp_create_pak(bufptr, ip, cptr);
    if (!pak) {
        if (crypto_engine_debug) {
            buginf("\nCRYPTO_ENGINE: can not create rsp pak.");
        }
        retbufhdr_inline(bufptr);
        return;
    }

    newip = (iphdrtype *)ipheadstart(pak);

    pak->cryptoflags = CRYPTO_DECRYPT;

    flow_info = (crypto_rsp_flow_info *)&pak_info;
    flow_info->switch_mode = CRYPTO_RSP_FLOW_SWITCH;
    flow_info->pak = pak;
    flow_info->hwidb = cptr->hwidb;
    flow_info->ip = newip;
    crypto_get_iv_seed(newip, &iv_seed[0], &iv_seed_size);
    crypto_set_encrypt_start(newip, &crypto_offset);

    epa_decrypt_packet(
         (uchar*)newip + crypto_offset,
          iplen - crypto_offset,
          conn_id, &iv_seed[0], iv_seed_size, &pak_info);

}

/*
 * crypto_rsp_flow_encrypt_decrypt
 *
 * This is the crypto encap vector assigned to ip cache entry.
 */

void crypto_rsp_flow_encrypt_decrypt (void *bp, iphdrtype *ip,
                                      ipcache_rntype *cptr,
                                      ulong iplen,
                                      void *crypto_tag) 
{
    crypto_tag_t *tag = (crypto_tag_t *)crypto_tag;

    if (tag->cryptoflags == CRYPTO_ENCRYPT) { 
            crypto_engine_encrypt_rsp_flow((bufhdr_t *)bp, ip, cptr, iplen,
                                            tag->crypto_conn_id); 
    } else {
        if (tag->cryptoflags == CRYPTO_DECRYPT) {
            crypto_engine_decrypt_rsp_flow((bufhdr_t *)bp, ip, cptr, iplen,
                                        tag->crypto_conn_id);
        } else {
            /* Drop the flow */
            crypto_swepa_error_pak_count++;
            cptr->hwidb->counters.output_total_drops++;
            retbufhdr_inline(bp);
        }
    }
}

/*
 * crypto_ipcache_add_callback
 *
 * Called when an ip cache entry is added to the cache. 
 */

void crypto_ipcache_add_callback (ipcache_rntype *entry)
{
    if (entry) {
        entry->crypto_encap = crypto_rsp_flow_encrypt_decrypt;
    }
}

void crypto_engine_rsp_fastsend (hwidbtype *hwidb, paktype *pak)
{
    crypto_pak_info    pak_info;
    crypto_fs_pak_info *fs_pak_info = (crypto_fs_pak_info *)&pak_info;
    uchar iv_seed[CRYPTO_MAX_IV_SIZE];
    ushort iv_seed_size;
    paktype *newpak;
    iphdrtype *ip;
    int crypto_offset;

    if (((pak->cryptoflags & CRYPTO_ENCRYPT) ||
         (pak->cryptoflags & CRYPTO_DECRYPT)) && 
         ! (is_vip(hwidb) && IS_ENCRYPTION(hwidb))) {

        newpak = crypto_replicate_pak(pak);
        if (!newpak) {
           return;
        }

        fs_pak_info->pak = newpak;
        ip = (iphdrtype *)newpak->network_start;
        fs_pak_info->hwidb = hwidb;
        fs_pak_info->switch_mode = CRYPTO_FAST_SWITCH;
        crypto_get_iv_seed(ip, &iv_seed[0], &iv_seed_size);
        crypto_set_encrypt_start(ip, &crypto_offset);

        if (newpak->cryptoflags & CRYPTO_DECRYPT) {
            epa_decrypt_packet(
               ipheadstart(newpak) + crypto_offset,
               ip->tl - crypto_offset,
               newpak->crypto_dec_conn_id, iv_seed, iv_seed_size, &pak_info);
        } else {
            epa_encrypt_packet(
               ipheadstart(newpak) + crypto_offset,
               ip->tl - crypto_offset,
               newpak->crypto_enc_conn_id, iv_seed, iv_seed_size, &pak_info);
        }
        return;
    } else {
        (*hwidb->fastsend) (hwidb, pak);
        return;
    }
}

void crypto_engine_rsp_tunnel_fs (hwidbtype *idb, paktype *pak)
{
    crypto_pak_info    pak_info;
    crypto_fs_pak_info *fs_pak_info = (crypto_fs_pak_info *)&pak_info;
    uchar iv_seed[CRYPTO_MAX_IV_SIZE];
    ushort iv_seed_size;
    iphdrtype *ip;
    int crypto_offset;
    paktype *newpak;
    int cryptoflags = pak->cryptoflags;
    hwidbtype *hwptr = NULL;

    if (pak->cryptoflags & CRYPTO_DECRYPT) {

    	if (pak->if_input) {
	    hwptr = pak->if_input->hwptr;
	}
        newpak = crypto_replicate_pak(pak);
        if (!newpak)
           return;

        newpak->cryptoflags = cryptoflags;
        ip = (iphdrtype *)newpak->network_start;
        fs_pak_info->pak = newpak;
        fs_pak_info->ip = ip; 
        fs_pak_info->hwidb = hwptr;
        fs_pak_info->switch_mode = CRYPTO_TUNNEL_FAST_SWITCH;
        crypto_set_encrypt_start(ip, &crypto_offset);
        crypto_get_iv_seed(ip, iv_seed, &iv_seed_size);
	epa_decrypt_packet(ipheadstart(newpak) + crypto_offset,
               		   ip->tl - crypto_offset,
               		   newpak->crypto_dec_conn_id, 
			   &iv_seed[0], 
			   iv_seed_size, 
			   &pak_info);
    }
}

/*
 * crypto_rsp_turbo_fs
 *
 * This is the ip turbo fs vector assinged to an interface when the
 * interface is configured for encryption. All it does is to 
 * check if the incoming packet is an ICMP connection set up packet.
 * If it is, punt the packet to the process switch. If it is not,
 * forward to the media ip_turbo_fs.
 */
int crypto_rsp_ip_turbo_fs (bufhdr_t* bufptr, iphdrtype*ip, hwidbtype *input)
{
    if (ip->prot == ICMP_PROT) {
        icmptype *icmp;
        icmp = (icmptype *)((char *)ip + ltob(ip->ihl));
        if (((icmp->type == ICMP_ECHOREPLY) || (icmp->type == ICMP_ECHO)) &&
            icmp->code == CRYPTO_ICMP_CODE) {
            return(IPFLOWERR_NOCACHE);
        }
    }

    if (is_vip(input)) {
        /* Let process switch handle the packet */
        return(IPFLOWERR_NOCACHE);
    }

    if (input->ip_routecache & IP_FLOW_ROUTECACHE) {
        return(rsp_ipflowswitch(bufptr,ip,input));
    } else {
        return(rsp_ipfastswitch(bufptr,ip,input));
    }
}

/*
 * crypto_rsp_register_crypto_vectors
 *
 * Called when a crypto map is assigned to an interface. Both input
 * and output vectors are replaced by the crypto vectors. If the interface
 * is on vip, an ipc command is send to vip.
 */
void crypto_rsp_register_crypto_vectors (hwidbtype *hwidb, 
                                         void *head) 
{
    crypto_vector_t *crypto_vec;
    leveltype  level;

    crypto_vec = (crypto_vector_t*)hwidb->crypto_vectors;
    if (!crypto_vec) {
        crypto_vec = malloc(sizeof(crypto_vector_t));
        if (!crypto_vec) {
            return;
        }
        hwidb->crypto_vectors = (void *)crypto_vec;
    }
    level = raise_interrupt_level(NETS_DISABLE);
    hwidb->status |= IDB_ENCRYPT;
    crypto_vec->fs_vec = crypto_engine_rsp_fastsend;
    crypto_vec->ps_vec = crypto_engine_ps_vec; 
    crypto_vec->tunnel_fs_vec = crypto_engine_rsp_tunnel_fs;
    if ((hwidb->ip_routecache &(IP_OPTIMUM_ROUTECACHE|IP_FLOW_ROUTECACHE)) &&
         !crypto_vec->media_ip_turbo_fs) {
        crypto_vec->media_ip_turbo_fs = hwidb->ip_turbo_fs;
        hwidb->ip_turbo_fs = (ip_turbo_fs_t)crypto_rsp_ip_turbo_fs;
    }
    reset_interrupt_level(level);

    if (is_vip(hwidb)) {
        crypto_vip_assign_map_to_int(hwidb, head);
    }

    upd_config_tbl (hwidb, CRYPTO_ENGINE_REGISTER);
}

/*
 * crypto_rsp_restore_media_vectors
 *
 * Called when a crypto map head is removed from an interface. Here only
 * rsp or vip vectors are handled. The ip fastsend is handeled by rp 
 * restore vector routine.
 */
void crypto_rsp_restore_media_vectors (hwidbtype *hwidb, void *head)
{
    crypto_vector_t *crypto_vec;
    leveltype level;
    crypto_map_headtype *map_head = (crypto_map_headtype *)head;
    
    crypto_vec = (crypto_vector_t *)hwidb->crypto_vectors;
    if (!crypto_vec)
        return;

    level = raise_interrupt_level(NETS_DISABLE);
    hwidb->status &= ~IDB_ENCRYPT;
    if ((hwidb->ip_routecache & (IP_OPTIMUM_ROUTECACHE|IP_FLOW_ROUTECACHE)) &&
        crypto_vec->media_ip_turbo_fs) {
        hwidb->ip_turbo_fs = (ip_turbo_fs_t)(crypto_vec->media_ip_turbo_fs);
    }
    reset_interrupt_level(level);

    if (is_vip(hwidb)) {
        crypto_vip_remove_map_from_int(hwidb, map_head);
    }

    free (hwidb->crypto_vectors);
    hwidb->crypto_vectors = NULL;

    upd_config_tbl(hwidb, CRYPTO_ENGINE_DEREGISTER);
}

static char *rstring =
  "\nSrcIf    SrcIPaddress    DstIf    DstIPaddress    Pr SrcP DstP Flag Conn";

/*
 * crypto_show_flow_info
 */
void crypto_show_flow (ipaddrtype prefix, ipaddrtype mask, idbtype *idb)
{
    ipaddrtype dest, src;
    hwidbtype *hwidb = (idb) ? idb->hwptr : NULL;
    ipflow_t *flow, *show, *showbuf;
    ipflowtbl_t *tp = &ipflowtbl;
    leveltype level;
    int bkt, idx;
    
    if (!(showbuf = malloc (sizeof(ipflow_t) * IPFLOWSHOWMAX)))
        return;
    
    automore_enable(NULL);
    
    automore_header(rstring);
    for (bkt=0; bkt<tp->hash_size; bkt++) {
	level = raise_interrupt_level(NETS_DISABLE);
	flow = (ipflow_t *)ipflowtbl.hashtbl->bucket[bkt];
	for (idx=0; flow && (idx<IPFLOWSHOWMAX); flow = flow->hash_next) {
	    if (!flow->pkts)
		continue; /* avoid div0 error, though shouldn't happen */
	    dest = flow->key.fl_dst;
	    src = flow->key.fl_src;
	    if (((dest & mask) != prefix) && ((src & mask) != prefix))
	      continue;
	    if (hwidb && hwidb != flow->fl_output && hwidb != flow->fl_input)
	      continue;
	
	    showbuf[idx++] = *flow;
	}
	reset_interrupt_level(level);

	show = showbuf;
	for ( ;idx-- > 0; show++) {
	    printf("\n%8s %15i %8s %15i %02x %04x %04x",
		   show->fl_input->hw_short_namestring, show->key.fl_src,
		   show->fl_output->hw_short_namestring, show->key.fl_dst,
		   show->key.fl_prot, show->key.fl_sport, show->key.fl_dport);
	    printf(" %-4d%-4d",
		   show->crypto_tag.cryptoflags,
		   show->crypto_tag.crypto_conn_id);
	}
 	if (automore_quit())
	    break;
    }
    free(showbuf);
    automore_disable();
}

watched_boolean *crypto_vip_update_conn_tbl;
/* 
 * This routing does following
 * A. It initiates IPC message to VIP to transfer connection table maintained
 *    by VIP.
 * B. The IPC callback function, crypto_engine_extract_vip_conn_tbl extracts
 *    the data from IPC data structure and copies them over to
 *    global pointer crypto_vip_connection_tbl and also initialized an
 *    integer crypto_vip_no_connections.
 * 
 */
void crypto_vip_get_conn_tbl (int slot)
{
    ulong major, minor; 

    /* 
     * We are now sure that slot is a VIP 
     */
    crypto_vip_update_conn_tbl = create_watched_boolean("GET CRYPTO TABLE BOOLEAN", 0);
    process_watch_boolean(crypto_vip_update_conn_tbl, ENABLE, ONE_SHOT);
    process_set_boolean(crypto_vip_update_conn_tbl, FALSE);

    crypto_initiate_vip_get_conn_tbl(slot);

    /* We probably need to set a timer. */
    process_wait_for_event(); 
    while ( process_get_wakeup(&major, &minor) ) {
	if ( major == BOOLEAN_EVENT ) {
	     break;
	}
    }
    delete_watched_boolean(&crypto_vip_update_conn_tbl);
}

void crypto_show_vip_conn_tbl (int slot)
{
    ushort i=0;
    extern ushort crypto_vip_no_connections;
    extern epa_vip_connection_tbl *crypto_vip_connection_tbl;
    epa_vip_connection_tbl *conn;   
    boolean found = FALSE;
    hwidbtype *idb;


    if (slot == crypto_engine_rpsp_slot) {
        crypto_engine_show_active_conn(slot);
        return;
    }

    FOR_ALL_HWIDBS(idb){
        if (idb->slot == slot && is_vip(idb)) {
            found = TRUE;
            break;
	}
    }
    if ( !found ) { 
         printf("Slot %d, does not have a encryption engine.\n",slot);
         return;
    }

    crypto_vip_get_conn_tbl(slot);
    /* Check if there is atleast one connection on VIP */

    if ( crypto_vip_no_connections == 0 || !crypto_vip_connection_tbl ) {
	printf("\nNo connections.\n");
	return;
    }
    printf("\n%5s %15s %11s %6s %15s %8s %8s\n","ID", "Interface",
           "IP-Address","State","Algorithm", "Encrypt","Decrypt");

    for (i = 0; i< crypto_vip_no_connections; i++) {
        conn = crypto_vip_connection_tbl+i;
        if (conn->state == EPA_SWCRYPTO_CONN_SET) {
            printf("%10d %15s %11i %6s %15s %8d %8d\n",conn->connection_id,
                  "Interface",
                  conn->if_index,
                  conn->state == 1? "set":"unset",
                  crypto_xlate_algorithm_name(conn->alg_type),
                  conn->encrypt_count,
                  conn->decrypt_count);

            }

        } 
    free(crypto_vip_connection_tbl);
    crypto_vip_no_connections=0;
}

static inline void crypto_engine_rsp_conf_init_inline (
                                  crypto_engine_config_t * crypto_conf,
                                  crypto_engine_idb_t * crypto_idb)
{
    crypto_conf->platform = CRYPTO_PLATFORM_RSP;
    crypto_idb->swepa_output = crypto_swepa_rsp_output;
}

void crypto_engine_rsp_conf_init (void *conf, void * idb)
{
    crypto_engine_rsp_conf_init_inline(conf, idb);
}

/*
 * Return the serial number for a VIP2 if a VIP2 is in the given slot.
 * NOTE: crypto_sigkeys() currently depends on this return of this function 
 *       being 0x0 if the slot does not contain a  VIP. If this semantic 
 *       changes, crypto_sigkeys() must be changed as well.
 */
ulong get_vip_slot_serial_number (ushort slot)
{
    slottype *slotp;

    FOR_ALL_NONEMPTY_SLOTS(slotp) {
        if ( slotp->slot_num == slot 
	    &&  slotp->ctrlr_type == FCI_SVIP_CONTROLLER  ) {
            return slotp->serial_number;
	}
    }

    return 0x0;
}

/*
 * Find the serial number for the processor board if there is a processor
 * board in the given slot.
 */
ulong get_processor_serial_number (void)
{
    dgtype *dbus;

    dbus = dbus_rpslot_to_dbus();
    if (!dbus) {
	return(0x0);
    }

    return(dbus_serial_number(dbus));
}

void crypto_add_connection_idb (hwidbtype *idb, ushort conn_id)
{
    epa_swcrypto_connection *conn;
    crypto_conn_idb_t *new_idb, *idb_ptr;


    if (conn_id <= 0 || conn_id >= CRYPTO_CONNTBL_SIZE) {
        return;
    }

    conn = &epa_swcrypto_conn_tbl[conn_id];

    new_idb = chunk_malloc(crypto_conn_idb_chunk);
    if (!new_idb) {
        return;
    }

    new_idb->next = NULL;
    new_idb->hwidb = idb;

    if (!conn->idbs) {
        conn->idbs = new_idb;
        return;
    }

    idb_ptr = conn->idbs;
    while (idb_ptr->next)  {
        if (idb_ptr->hwidb == idb) {
            chunk_free(crypto_conn_idb_chunk, new_idb);
            return;
        }
        idb_ptr = idb_ptr->next;
    }

    idb_ptr->next = new_idb;

    return;
}


void crypto_invalidate_ip_cache (ushort conn_id)
{
    crypto_conn_idb_t  *conn_idb, *temp_idb;

    /*
     * Invalidate all the ip cache entries related to this connection.
     * After the cache entry is invalidated, free the idb.
     */

    conn_idb = epa_swcrypto_conn_tbl[conn_id].idbs;
    while (conn_idb) {
        reg_invoke_ipcache_invalidate(conn_idb->hwidb,
                                      conn_idb->hwidb->firstsw, TRUE);
        temp_idb = conn_idb;
        conn_idb = conn_idb->next;
        chunk_free(crypto_conn_idb_chunk, temp_idb);
    }

    epa_swcrypto_conn_tbl[conn_id].idbs = NULL;
}

/*
 * This is called to register the crypto ip_turbo_fs vector. It is called
 * when a interface is configued for flow or optimum switching.
 */
void crypto_rsp_register_ip_turbo_fs (hwidbtype  *idb)
{
    crypto_vector_t *crypto_vec;

    crypto_vec = (crypto_vector_t *)idb->crypto_vectors;

    if (!crypto_vec)
        return;
    if (idb->ip_routecache & IP_FLOW_ROUTECACHE){
        crypto_vec->media_ip_turbo_fs = rsp_ipflowswitch;
    } else {
        if (idb->ip_routecache & IP_OPTIMUM_ROUTECACHE){
            crypto_vec->media_ip_turbo_fs = rsp_ipfastswitch;
        }
    }

    idb->ip_turbo_fs = (ip_turbo_fs_t)crypto_rsp_ip_turbo_fs;
}


/*
 * crypto_engine_rsp_init
 *
 */
void crypto_engine_rsp_init (subsystype* subsys)
{
    /*
     * Find the the RSP slot on which we're running. If we get MAX_SLOTS, the
     * image couldn't figure out which slot. Then we just pick the slot for
     * the 7513. Crude, but there isn't a modular method of determining which
     * 75xx backplane we have. Oh by the way, jacks-hacks assures us that
     * compute_rsp_slot isn't reliable. In fact, it returns MAX_SLOTS.
     */
    crypto_engine_rpsp_slot = (ushort) get_rsp_self_slot();
    if (crypto_engine_rpsp_slot == MAX_SLOTS) {
	crypto_engine_rpsp_slot = RSPSLOT_7513_PROC1;
        printf("%% Cannot determine RSP slot. Assuming slot %d.\n", 
		crypto_engine_rpsp_slot);
    }
    crypto_engine_platform = CRYPTO_PLATFORM_RSP;

    crypto_conn_idb_chunk = chunk_create(sizeof(crypto_conn_idb_t),
                                         CRYPTO_CONN_IDB_CHUNK_SIZE,
                                         CHUNK_FLAGS_NONE, NULL, 0,
                                         "crypto conn idb");

    reg_add_crypto_engine_rsp_conf_init(crypto_engine_rsp_conf_init,
                                      "crypto_engine_rsp_conf_init");

    reg_add_crypto_engine_req_remote_dspt (crypto_engine_req_remote_dspt,
					   "crypto_engine_req_remote_dspt");

    reg_add_crypto_engine_rsp_ipc_callback(crypto_engine_rsp_ipc_callback,
                                          "crypto_engine_rsp_ipc_callback");

    reg_add_crypto_show_flow_info(crypto_show_flow, "crypto_show_flow");

    reg_add_crypto_ipcache_add_callback(crypto_ipcache_add_callback,
                               "crypto_ipcache_add_callback");       

    reg_add_crypto_engine_register_crypto_vectors(
                                   crypto_rsp_register_crypto_vectors,
                                  "crypto_rsp_register_crypto_vectors");

    reg_add_crypto_engine_restore_media_vectors(
                                   crypto_rsp_restore_media_vectors,
                                  "crypto_rsp_restore_media_vectors");

    reg_add_crypto_vip_map_head_add(crypto_vip_map_head_add,
                                    "crypto_vip_map_head_add");
    reg_add_crypto_vip_map_add(crypto_vip_map_add,
                               "crypto_vip_map_add");

    reg_add_crypto_show_vip_conn_tbl(crypto_show_vip_conn_tbl,
                               "crypto_show_vip_conn_tbl");

    reg_add_crypto_vip_map_delete(crypto_vip_map_delete,
                                 "crypto_vip_map_delete");
    reg_add_crypto_vip_map_head_delete(crypto_vip_map_head_delete,
                                       "crypto_vip_map_head_delete");
    reg_add_crypto_vip_assign_map_to_int(crypto_vip_assign_map_to_int,
                                       "crypto_vip_assign_map_to_int");
    reg_add_crypto_vip_map_set_peupe(crypto_vip_map_set_peupe,
                                     "crypto_vip_map_set_peupe");
    
    reg_add_crypto_vip_map_set_modeflags(crypto_vip_map_set_modeflags,
                                     "crypto_vip_map_set_modeflags");

    reg_add_crypto_vip_upd_map_conn_id(crypto_vip_upd_map_conn_id,
                                     "crypto_vip_upd_map_conn_id");

    reg_add_crypto_config_epa(crypto_config_epa, "crypto_config_epa");

    reg_add_crypto_show_card_status(crypto_engine_show_card_status,
                                    "crypto_engine_show_card_status");

    reg_add_crypto_card_cfg_comm(crypto_card_clear_latch,
                                    "crypto_card_clear_latch");

    reg_add_crypto_remove_dfs(crypto_remove_dfs,"crypto_remove_dfs");

    reg_add_get_slot_serial_number(get_vip_slot_serial_number,
				   "get_slot_serial_number");

    reg_add_get_processor_serial_number(get_processor_serial_number,
					"get_processor_serial_number");

    reg_add_crypto_add_connection_idb(crypto_add_connection_idb,
                                     "crypto_add_connection_idb");

    reg_add_crypto_invalidate_ip_cache(crypto_invalidate_ip_cache,
                                       "crypto_invalidate_ip_cache");

    reg_add_crypto_rsp_register_ip_turbo_fs(crypto_rsp_register_ip_turbo_fs,
                                            "crypto_rsp_register_ip_turbo_fs");


    crypto_engine_rsp_ipc_init();
}

/*
 * CRYPTO_ENGINE_RSP subsystem header
 */
#define CRYPTO_ENGINE_RSP_MAJVERSION 1
#define CRYPTO_ENGINE_RSP_MINVERSION 0
#define CRYPTO_ENGINE_RSP_EDITVERSION 1

SUBSYS_HEADER(crypto_engine_rsp, CRYPTO_ENGINE_RSP_MAJVERSION, 
              CRYPTO_ENGINE_RSP_MINVERSION,
              CRYPTO_ENGINE_RSP_EDITVERSION,
	      crypto_engine_rsp_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      NULL);
