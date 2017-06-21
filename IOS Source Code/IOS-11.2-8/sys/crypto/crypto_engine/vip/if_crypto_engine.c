/* $Id: if_crypto_engine.c,v 1.1.68.11 1996/07/22 07:39:00 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/vip/if_crypto_engine.c,v $
 *------------------------------------------------------------------
 * if_crypto_engine.c 
 * 
 * This file defines the crypto engine vip interface routines. 
 *
 * 11, 1995, xliu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc. 
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_crypto_engine.c,v $
 * Revision 1.1.68.11  1996/07/22  07:39:00  xliu
 * CSCdi63492:  PPP does not work when switched from HDLC on EPA/VIP2
 * Branch: California_branch
 *
 * Revision 1.1.68.10  1996/07/15  09:59:34  xliu
 * CSCdi61651:  epa randome number seed is not initialized
 * Branch: California_branch
 *
 * Revision 1.1.68.9  1996/07/01  12:27:03  xliu
 * CSCdi61743:  decryption does not use cached pak pointer correctly.
 * Branch: California_branch
 *
 * Revision 1.1.68.8  1996/06/26  11:16:29  xliu
 * CSCdi61336:  vip2 crypto code clean up
 * Branch: California_branch
 *
 * Revision 1.1.68.7  1996/06/26  10:26:46  xliu
 * CSCdi61336:  vip2 crypto code clean up
 * Branch: California_branch
 *
 * Revision 1.1.68.6  1996/06/21  22:33:16  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 1.1.68.5  1996/06/01  01:15:16  xliu
 * CSCdi59234:  crypto rsp flow vector does not work correct for vip
 * encryption
 * Branch: California_branch
 *
 * Revision 1.1.68.4  1996/05/28  16:00:18  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 1.1.68.3  1996/05/12  23:13:42  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 1.1.68.2  1996/04/23  23:46:24  ptran
 * CSCdi55475:  need to include crypto engine in svip image.
 * Branch: California_branch
 * Remove unecessary include of rvip header files.
 *
 * Revision 1.1.68.1  1996/04/19  14:57:26  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.10  1996/04/09  20:03:35  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 1.1.2.9  1996/03/30  05:12:52  xliu
 * Branch: IosSec_branch
 * Commit before CA repository split.
 *
 * Revision 1.1.2.8  1996/03/25  23:19:59  xliu
 * Branch: IosSec_branch
 * Checkin VIP crypto engine code.
 *
 * Revision 1.1.2.7  1996/02/13  19:16:52  carrel
 * Branch: IosSec_branch
 * Crypto Map checkin (finally)
 *
 * Revision 1.1.2.6  1996/01/30  22:36:23  xliu
 * Branch: IosSec_branch
 * Checking in encryption RSP/VIP support code.
 *
 * Revision 1.1.2.5  1996/01/09  02:27:04  xliu
 * Branch: IosSec_branch
 * VIP software encryption.
 *
 * Revision 1.1.2.4  1995/12/10  09:30:12  xliu
 * Branch: IosSec_branch
 * Merge the code to replace the idb fastsend using ipc.
 *
 * Revision 1.1.2.3  1995/12/05  05:41:49  xliu
 * Branch: IosSec_branch
 * Add output vip packet analysis and debugging.
 *
 * Revision 1.1.2.2  1995/12/02  17:43:02  xliu
 * Branch: IosSec_branch
 *
 * Modified epa driver interface.
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
/*
 * This file defines the crypto engine fastsend routine which includes the 
 * following functionalities:
 *
 * 1. Identify the prepend bytes from the packet and check for the encryption
 *    flag. If it is on, go to step 2. Otherwise, remove the prepend and
 *    pass the packet to the media driver.
 * 2. create encryption control block for the given packet by allocating
 *    a particle and instantiating the control block. The connection id 
 *    is retrived from crypto map. Prepend the particle to the given packet.
 * 3. pass the packet to epa driver by calling epa fastsend routine. For
 *    encryption, the output interface is also passed to the driver. For
 *    decryption, the interface is set to be NULL. Expcect the driver to return
 *    the packet.
 *
 * Function/Procedures:
 * 2. static inline: find connection id from crypto map. Now, from acl. 
 * 3. static inline: allocate a particle and create the encrypt cont block.
 */
#include "master.h"
#include <ciscolib.h>
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
#include "../if/atm.h"

#include "../src-vip/dfs_vip_idb.h"

#include "../ip/ipfast_index.h"
#include "../ip/ipfast_ipc.h"
#include "../ip/ipfast_vip.h"
#include "../ip/ipfast_vip_ipc.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"

#include "../if/ether.h"
#include "../if/pci.h"
#include "../if/network.h"
#include "../if/fddi.h"

#include "../pas/if_pas.h"
#include "../pas/if_pas_amdp2.h"

#include "../src-vip/vip_dma.h"
#include "../src-vip/vip_memd.h"
#include "../src-vip/vip_debug.h"

#include "../ip/ip.h"
#include "../crypto/crypto_registry.h"
#include "../crypto/crypto_public.h"
#include "../crypto/crypto_map.h"
#include "../crypto/crypto.h"
#include "../crypto/crypto_engine_api.h"
#include "../crypto/crypto_engine/core/crypto_engine.h"
#include "../crypto/crypto_engine/crypto_lib/lib_toolkit.h"
#include "../crypto/crypto_engine/core/crypto_swepa.h"
#include "crypto_engine_vip.h"
#include "../crypto/crypto_sesmgmt.h"
#include "../crypto/crypto_util.h"
#include "../crypto/crypto_ip.h"
#include "../crypto/crypto_conntable.h"

extern epa_swcrypto_connection epa_swcrypto_conn_tbl[];
void check_crypto_map (paktype *, uint, ushort);

/* 
 * Callback from epa driver after driver is initialized.
 * Assign the epa encrypt/decrypt vectors to crypto_engine_vip_config.
 * Assign epa idb.
 */
void crypto_register_hwidb (int device, hwidbtype *idb)
{
    if (!device) {
         crypto_engine_vip_config.epa_type = CRYPTO_HWEPA;
         crypto_engine_vip_config.hwepa_status = CRYPTO_HWEPA_NORMAL;
         crypto_engine_vip_config.epa_encrypt_idb = idb;
         crypto_engine_idb.vip_encrypt = crypto_vip_epa_encrypt;
         crypto_engine_idb.vip_decrypt = crypto_vip_epa_decrypt;
    } else {
         crypto_engine_vip_config.epa_key_idb = idb;
    }
}


/*
 * Callback from epa driver during the normal operation.
 *
 */
void crypto_engine_callback (ushort type, paktype *pak)
{
    hwidbtype *input_idb;
    crypto_vector_t *crypto_vec;

    switch (type) {
    
        case CRYPTO_DECRYPT:
           
            input_idb = (hwidbtype*)pak->if_input->hwptr;
            if (!input_idb) {
                if (crypto_engine_vip_debug) {
                    buginf(  "\nCRYPTO ENGINE: input idb is null.");
                }
                return;
            }
            crypto_vec = (crypto_vector_t *)input_idb->crypto_vectors;
            if (!crypto_vec)
                return;

            if (crypto_engine_vip_debug) {
                buginf("\nCRYPTO_ENGINE: send back decrypted packet %x",pak);
                crypto_dump_particles(pak, pak->datagramsize);
            }

            (* (pak_dfs_t)crypto_vec->media_pak_dfs)(input_idb, pak);

            break;

        case CRYPTO_EPA_ENCRYPTION_ERROR:
            /* error handling */
            if (crypto_engine_vip_debug) {
               buginf( "CRYPTO_ENGINE_VIP: encryption error.\n");
            }
            break;

        default:
            /* error handling */
            break;
    }
}

static inline boolean crypto_ip_sanity_check(paktype *pak,
					     hwidbtype *idb)
{
    iphdrtype  *ip;
    icmptype *icmp;

    /* Sanity check */
    if (!idb || !pak || !pak->datagramstart) {
        if (crypto_engine_vip_debug) {
            buginf( "CRYPTO_ENGINE: null datagramstart.\n");
        }
        return FALSE;
    } 
                
    if (!idb->firstsw) {
        if (crypto_engine_vip_debug) {
            buginf( "CRYPTO_ENGINE: null firstsw.\n");
        }
        return FALSE;
    }

    /*
     * If the linktype field is set in the pak,
     * then we only do crypto if it is LINK_IP.
     */
    if (pak->linktype && (pak->linktype != LINK_IP)) {
        if (crypto_engine_vip_debug)
	    buginf("CRYPTO_ENGINE: wrong linktype\n");
	return FALSE;
    }

    ip = (iphdrtype *)ipheadstart(pak);

    /*
     * Sanity checks on this ip packet, since it is possible
     * to get here without going through ip code (vip only).
     */
    if ((ulong)ip & 3) {
	/*
	 * Unaligned ip header- have to be careful here.
	 */
	
	uchar ihl, version, prot;
	uchar type, code;
	ushort tl;
	ulong temp;

	temp = GETLONG(ip);
	version = temp >> 28;
	ihl = (temp >> 24) & 0xF;
	tl = temp & 0xFFFF;

	if ((version != IPVERSION)  ||
	    (tl < MINIPHEADERBYTES) ||
	    (tl > MAXDATAGRAMSIZE)  ||
	    (ihl < btol(MINIPHEADERBYTES))) {
	    if (crypto_engine_vip_debug)
		buginf("CRYPTO_ENGINE: bad ip header\n");
	    return FALSE;
	}

	temp = GETSHORT((char *)ip + (2 * sizeof(ulong)));
	prot = temp & 0xFF;

	if (prot == 0) {
	    if (crypto_engine_vip_debug)
		buginf("CRYPTO_ENGINE: prot = 0\n");
	    return FALSE;
	}

	if (prot == ICMP_PROT) {
	    temp = GETSHORT((char *)ip + ltob(ihl));
	    type = (temp >> 8) & 0xFF;
	    code = temp & 0xFF;

	    if (((type == ICMP_ECHOREPLY) || (type == ICMP_ECHO)) &&
		 (code == CRYPTO_ICMP_CODE)) {
		if (crypto_engine_vip_debug)
		    buginf("CRYPTO_ENGINE: icmp connection msg\n");
		return FALSE;
	    }
	}
    } else {
	if ((ip->version != IPVERSION) ||
	    (ip->tl > MAXDATAGRAMSIZE) ||
	    (ip->tl < MINIPHEADERBYTES) ||
	    (ip->ihl < btol(MINIPHEADERBYTES)) ||
	    (ip->prot == 0)) {
	    if (crypto_engine_vip_debug)
		buginf("CRYPTO_ENGINE: bad ip header\n");
	    return FALSE;        
	}

	if (ip->prot == ICMP_PROT) {
	    icmp = (icmptype *)((char *)ip + ltob(ip->ihl));
	    if (((icmp->type == ICMP_ECHOREPLY) || 
		 (icmp->type == ICMP_ECHO)) &&
		 icmp->code == CRYPTO_ICMP_CODE) {
		if (crypto_engine_vip_debug)
		    buginf("CRYPTO_ENGINE: icmp connection msg\n");
		return FALSE;
	    }
	}
    }
    return TRUE;
}

/*
 * crypto_check_pak_for_encryption
 *
 * Lookup the ipcache using the destination address. Retrieve the 
 * crypto tag contained in encapinfotype. Return the conn id if the
 * packet should be encrypted.
 */
 boolean crypto_check_pak_for_encryption(paktype *pak,
                                         hwidbtype *idb)
{

    /* perform sanity checks first */
    if (!crypto_ip_sanity_check(pak, idb))
	return FALSE;

    pak->cryptoflags = 0;

    return crypto_map_check(pak,idb->firstsw->crypto_map, CRYPTO_ENCRYPT, TRUE);
}

/*
 * crypto_engine_fastsend 
 *
 * Define pak->network_start; Create encrypt blk; Prepend to the pak;
 * Call epa driver to encrypt and send.
 * Assume at this time, the pak->cryptoflags has been set in the following
 * way:
 *  1) called by vip_ipflow_fs for decryption: 
 *         pak->crypto_flag is set to CRYPTO_DECRYPT.
 *  2) called by dma tx poll routine:
 *         pak->crypto_flag is set to CRYPTO_ENCRYPT.
 * In either case, the packet is presented in the format of paktype.
 */

void crypto_engine_vip_fastsend (hwidbtype *idb, paktype *pak)
{
   crypto_vip_pak_info pak_info;
   crypto_vector_t *vectors;
   ushort       status;
   iphdrtype 	*ip;
   boolean      crypto_bypass = FALSE;

   /*
    * set pak->network_start
    *
    * Note: we should have been able to use idb->encsize to find the
    * ip header start position. But, there are some problems with the
    * fddi and token ring card whose idb->encsize is not correct. Currently
    * this is a workaround. We should investigate the problem.
    *
    * Note: the FR and other encap type are not included here. The encsize
    * problem should be solved before the FR and other encap encryption
    * support is added. WIth encsize correctly reflecting the encap size,
    * there is no need of the switching statement here.
    */
   switch(idb->type) {
       case IDBTYPE_FDDIT: 
           pak->network_start = pak->datagramstart + FDDI_SNAP_ENCAPBYTES; 
           break;
       case IDBTYPE_IBM2692:
           pak->network_start = pak->datagramstart + TRING_SNAP_ENCAPBYTES;
           break;
       case IDBTYPE_AMDP2:
           pak->network_start = pak->datagramstart + ETHER_ARPA_ENCAPBYTES; 
           break;
       case IDBTYPE_S4T68360:
	   switch (idb->enctype) {
	   case ET_HDLC:
               pak->network_start = pak->datagramstart + HDLC_ENCAPBYTES;
	       break;
           case ET_PPP:
               pak->network_start = pak->datagramstart + PPP_ENCAPBYTES;
	       break;
	   default:
	       crypto_bypass = TRUE;
	       break;
           }
           break;
       default:
           crypto_bypass = TRUE;
           break;
   }

   /* Check if the pak is intended for encryption. */
   if (crypto_bypass || !crypto_check_pak_for_encryption(pak, idb) ||
       !(pak->cryptoflags & CRYPTO_ENCRYPT)) {
       /* Forward the pak to the media interface vector */
       crypto_bypass = FALSE;
       vectors = (crypto_vector_t *)idb->crypto_vectors;
       if (!vectors) {
           if (crypto_engine_vip_debug) {
               buginf( "CRYPTO_ENGINE_VIP: null crypto vectors.\n");
           }
           datagram_done(pak);
           return;
       }
       (*vectors->media_fastsend)(idb, pak);
       return;
   }

   if (crypto_deny_packet(pak)) {
       if (crypto_engine_vip_debug) {
           buginf("\nCRYPTO_ENGINE: deny packets");
       }
       datagram_done(pak);
       return;
   }

   ip = (iphdrtype *)ipheadstart(pak);
   if (get_ip_morefragments(ip) || (getfo(ip) != 0)) {
       /*
        * Fragments need to be reassembled before they
        * can be encrypted properly. Until we have
        * this capability on the vip crypto engine,
        * have to drop the packet.
        */
       if (crypto_engine_vip_debug) {
           buginf("\nCRYPTO_ENGINE: dropping outgoing fragment");
           datagram_done(pak);
           return;
        }
    }

   pak_info.ip = (iphdrtype *)ipheadstart(pak);
   pak_info.pak = pak;
   pak_info.hwidb = idb;
   pak_info.switch_mode = CRYPTO_VIP_FAST_SWITCH;
   pak->if_output = idb->firstsw;
   (*crypto_engine_idb.vip_encrypt)((void*)&pak_info, &status);
}


