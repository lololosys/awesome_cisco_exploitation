/* $Id: crypto_swepa_common.h,v 1.1.50.9 1996/09/10 14:14:41 sbelair Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/rpsp/crypto_swepa_common.h,v $
 *------------------------------------------------------------------
 * Crypto Engine RSP/RP common defintions. 
 *
 * Jan 1996, xliu 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_swepa_common.h,v $
 * Revision 1.1.50.9  1996/09/10  14:14:41  sbelair
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
 * Revision 1.1.50.8  1996/09/06  23:57:43  bew
 * CSCdi62313:  Sourced fragments dont encrypt properly
 * Branch: California_branch
 * Add support for fragmented packets: always reassemble fragments
 * before either encryption or decryption.
 *
 * Revision 1.1.50.7  1996/08/09  23:53:38  sbelair
 * CSCdi62311:  Encrypted tunnels dont work
 * Branch: California_branch
 * Don't update crypto_engine_tbl for tunnel interfaces, since these
 * don't have a valid slot number.
 *
 * Revision 1.1.50.6  1996/08/08  19:32:19  xliu
 * CSCdi63274:  Wrong crypto engine name a show cr eng conf
 * Branch: California_branch
 *
 * Revision 1.1.50.5  1996/06/07  00:26:43  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Change iv offset and encryption point if the IP payload is too small
 * (e.g., a fragment).
 *
 * Revision 1.1.50.4  1996/06/05  23:11:54  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Fix crypto zeroize & key generation for 7500 with VIPs.
 * Fix crypto connection setup with traffic in both directions.
 * Fix encryption bugs.
 *
 * Revision 1.1.50.3  1996/05/12  23:13:15  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 1.1.50.2  1996/04/23  22:04:50  bew
 * CSCdi55458:  fix usage of slot when registering crypto vectors
 * Branch: California_branch
 * Fixed a problem where the slot was used instead of a
 * crypto engine identifier.
 *
 * Revision 1.1.50.1  1996/04/19  14:57:07  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.6  1996/04/17  23:48:15  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 1.1.2.5  1996/04/09  20:03:12  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 1.1.2.4  1996/02/13  17:02:12  xliu
 * Branch: IosSec_branch
 *
 * Revision 1.1.2.3  1996/01/20  16:51:31  xliu
 * Branch: IosSec_branch
 * Solve the 7000 problem with ip les fastswitch code.
 *
 * Revision 1.1.2.2  1996/01/19  07:01:04  xliu
 * Branch: IosSec_branch
 * Commit before the second sync.
 *
 * Revision 1.1.2.1  1996/01/09  07:21:56  xliu
 * Branch: IosSec_branch
 * Crypto engine RSP.
 *
 * Revision 1.1  1996/01/09  05:32:11  xliu
 * Crypto engine RSP.
 *
 * Revision 1.7  1995/06/07 12:03:03  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * crypto_encrypt_fs_output
 *
 * Called by crypto engine after encryption to send the packet out in 
 * fastswitch mode. The packet has been encapsulated with the
 * output mac string. Assume pak->if_output->hwptr is defined.
 */
static inline void 
crypto_encrypt_fs_output(crypto_encrypt_header *header) 
{
    crypto_fs_pak_info *fs_pak_info;
    paktype *pak;
    leveltype level;
    hwidbtype *output_idb;

    fs_pak_info = (crypto_fs_pak_info *)&header->pak_info;
    pak = fs_pak_info->pak;
    output_idb = fs_pak_info->hwidb; 

    level = raise_interrupt_level(NETS_DISABLE);
    (* output_idb->fastsend) (output_idb, pak);
    reset_interrupt_level(level);
}

/*
 * crypto_encrypt_ps_output
 *
 * Called by the crypto engine after encryption to send the packet out
 * in process switch mode.
 */
static inline void 
crypto_encrypt_ps_output (crypto_encrypt_header *header)
{
    crypto_ps_pak_info *pak_info;
    paktype *pak;

    pak_info = (crypto_ps_pak_info *)&header->pak_info;
    pak = pak_info->pak;

    pak->cryptoflags &= ~CRYPTO_ENCRYPT;

    /* CRYPTO_ENCRYPT_DONE is needed by ipcache_update to update the
     * flow cache with connection id.
     */
    pak->cryptoflags |= CRYPTO_ENCRYPT_DONE;

    ipsendnet (pak, TRUE, pak_info->log); 
}

/*
 * crypto_decrypt_fs_output
 *
 * Called by the crypto engine after the decryption to send the packet 
 * through the fastswitch path again. 
 * NOTE: Here we assume the L4 header is kept clear and the input
 * access list, the ip policy routing flag etc have been checked. Therefore
 * we don't need to go back to the driver.
 */

static inline void 
crypto_decrypt_fs_output(crypto_encrypt_header *header) 
{
    crypto_fs_pak_info *fs_pak_info;
    paktype *pak;
    leveltype level;
    hwidbtype *output_idb;

    fs_pak_info = (crypto_fs_pak_info *)&header->pak_info;
    pak = fs_pak_info->pak;
    output_idb = fs_pak_info->hwidb; 

    level = raise_interrupt_level(NETS_DISABLE);
    (* output_idb->fastsend) (output_idb, pak);
    reset_interrupt_level(level);
}

/*
 * crypto_decrypt_ps_output
 *
 * Called by the crypto engine after the decryption. The packet is enqueued
 * to the IPpacketQ again. 
 */

static inline void 
crypto_decrypt_ps_output(crypto_encrypt_header *header)
{
    crypto_ps_pak_info *pak_info;
    paktype *pak;
    iphdrtype *ip;

    pak_info = (crypto_ps_pak_info *)&header->pak_info;
    pak = pak_info->pak;

    pak->cryptoflags &= ~CRYPTO_DECRYPT;
    pak->cryptoflags |= CRYPTO_DECRYPT_DONE;

    /*
     * Send the packet out to different direction:
     * 1) Forwarding: pass the packet to ipsendnet
     *)2) Forus:      pass the packet to ipenqueue;
     *)3) Tunneling:  pass the packet to ipenqueue;
     */

    if (ip_forus(pak) || (is_tunnel(pak->if_input->hwptr))) {
        ip = (iphdrtype *)ipheadstart(pak);
        ip_traffic.localdest++;
        reg_invoke_ip_enqueue(ip->prot,pak);
    } else {
        ipsendnet(pak, TRUE, pak_info->log);
    }
}

/*
 * crypto_decrypt_tunnel_fs_output
 *
 * Called by the crypto engine after the decryption. The packet is 
 * sent right back into the tunnel fast-switch routine which had
 * the packet decrypted. Now that the gre header is decrypted
 * the packet can be processed.
 */
static inline void
crypto_decrypt_tunnel_fs_output(crypto_encrypt_header *header)
{
    crypto_fs_pak_info *fs_pak_info;
    paktype *pak;
    leveltype level;

    fs_pak_info = (crypto_fs_pak_info *)&header->pak_info;
    pak = fs_pak_info->pak;
    pak->cryptoflags &= ~CRYPTO_DECRYPT;
    pak->cryptoflags |= CRYPTO_DECRYPT_DONE;

    level = raise_interrupt_level(NETS_DISABLE);
    if (!reg_invoke_tunnel_ip_les_fs(pak)) {
	/*
	 * Couldn't fast-switch the packet, enqueue it
	 * it for process-level. Re-set the cryptoflags
	 * since fast-switch routines can clear them.
	 */
	pak->cryptoflags |= CRYPTO_DECRYPT_DONE;
	ip_simple_enqueue(pak);
    }
    reset_interrupt_level(level);
}

/*
 * crypto_engine_rsp_upd_config_tbl
 */
static inline void upd_config_tbl (hwidbtype *idb, ushort action)
{
    ulong tm;
    crypto_maptype *map;
    crypto_map_headtype *head;

    /*
     * On the platform which does not have slot number, return
     */
    if (NO_EXTENDED_SLOTS_ON_CHASSIS || (idb->slot == 0xffffffff)) {
        return;
    }

    switch (action) {
    case CRYPTO_ENGINE_REGISTER:

        GetRTClock(&tm);

        if (IS_VIP(idb)) {
            /*
             * crypto engine id is same as the slot number for the
             * vip interface.
             */
            crypto_engine_tbl[idb->slot].id = idb->slot;
	 } else {
            /*
             * For all the slots which don't have vip installed, use the
             * crypto engine on rsp.
             */
            crypto_engine_tbl[idb->slot].id = crypto_engine_rpsp_slot;
        }
        crypto_engine_tbl[idb->slot].time_start = tm;
        crypto_engine_tbl[idb->slot].state = CRYPTO_ENGINE_INSTALLED;
        break;

    case CRYPTO_ENGINE_DEREGISTER:
        /* Leave this case here for potential usage */ 
        break;
    default:
        break;
    }

    /* Update crypto map->crypto_engine_id */
    for (head = crypto_maps; head; head = head->next) 
        for (map = head->top; map; map = map->next) {
           if (map->slot == idb->slot) {
               map->crypto_engine_id = crypto_engine_tbl[idb->slot].id;
           }
        }
}

void crypto_engine_ps_vec(ushort, paktype*, boolean, boolean);
