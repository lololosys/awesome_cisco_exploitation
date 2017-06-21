/* $Id: ipfast_flow_pas.c,v 3.1.4.7 1996/08/24 01:48:58 abaindur Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast_flow_pas.c,v $
 *------------------------------------------------------------------
 * ipfast_flow_pas.c : IP Flow Switching for Particle based boxes
 *
 * March 1996, Michael Beesley
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast_flow_pas.c,v $
 * Revision 3.1.4.7  1996/08/24  01:48:58  abaindur
 * CSCdi66979:  Traffic shaping broken on the c7200
 * Branch: California_branch
 *
 * Revision 3.1.4.6  1996/07/27  00:14:58  ssangiah
 * CSCdi64084: c7200: Optimum/Flow switching from non-ISL to ISL interface
 *             is broken.
 * Add routine to handle ISL encapsulation rewrites.
 * Branch: California_branch
 *
 * Revision 3.1.4.5  1996/07/17  04:59:45  rramacha
 * CSCdi63042:  Enable IP Flow/Optimum on 4T
 * Branch: California_branch
 *
 * Revision 3.1.4.4  1996/07/06  05:53:31  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.1.4.3  1996/05/23  00:52:49  sthormod
 * CSCdi57953:  Enable IP Flow/Optimum on token ring
 * Branch: California_branch
 *
 * Revision 3.1.4.2  1996/05/21  09:51:21  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.4.1  1996/05/07  00:14:13  mbeesley
 * CSCdi55679:  Add IP flow/optimum switching engine
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/29  18:42:28  mbeesley
 * Add placeholder files for new project
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "../ui/common_strings.h"
#include "registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "sys_registry.h"
#include "access.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"

#include "../if/ether.h"
#include "../if/fddi.h"
#include "../ip/ip.h"
#include "../ipmulticast/igmp.h"
#include "../ip/ipaccess.h"
#include "../ip/ipaccount.h"
#include "../ip/ipfast.h"
#include "../if/tring.h"

#include "../wan/frame_relay.h"
#include "../wan/atm_dxi.h"
#include "../wan/smds.h"
#include "../if/network.h"
#include "../if/atm.h"

#include "ipfast_flow_pas.h"
#include "ipfast_rsp_mtrie.h"
#include "ipfast_flow.h"

#include "../pas/if_pas.h"

#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_tcpip.h"
#include "ip_registry.h"
#include "../vlan/vlan.h"

/*
 * Define slop bytes so we do not get close to the start of a particle
 */
#define PARTICLE_SLOP_BYTES	8

/*
 * ip_pas_flow_get:
 * Look up a flow entry
 */
static inline ipflow_t *ip_pas_flow_get (paktype *pak, iphdrtype *ip, 
					 hwidbtype *input)
{
    uchar flags;
    ipflow_t *flow;
    ipflow_key_t flkey;
    ipcache_rntype *cptr, *ncptr;
    crypto_tag_t    crypto_tag = {0, 0};
    
    flags = ipflow_key_get(ip, &flkey, input);
    flow = ipflow_hash_lookup(&flkey);
    if (flow) {
	cptr = flow->cptr;
	if ((cptr->flags & IPCACHE_FLAG_VALID) == 0) {
	    ncptr = mnode_prefix_lookup(flkey.fl_dst);
	    if (!ncptr) {
		return(NULL);
	    }
	    if (ncptr->hwidb->ip_fast_flags) {
		if (!pak_flow_access_check(pak, input, ncptr->idb, 0, 
					   &crypto_tag)) {
		    return(NULL);
		}
	    }
	    flow->cptr = ncptr;
	    ipcache_adjust_refcount(cptr, -1);
	    ipcache_adjust_refcount(ncptr, 1);
	} else if ((flags & TCP_FLAGS_EST) == 0) {
	    if (cptr->hwidb->ip_fast_flags) {
		if (!pak_flow_access_check(pak, input, cptr->idb, 0, 
					   &crypto_tag)) {
		    return(NULL);
		}
	    }
	}
    } else {
	cptr = mnode_prefix_lookup(flkey.fl_dst);
	if (!cptr) {
	    return(NULL);
	}
	if (cptr->hwidb->ip_fast_flags) {
	    if (!pak_flow_access_check(pak, input, cptr->idb, 0, 
				       &crypto_tag)) {
		return(NULL);
	    }
	}
	flow = ipflow_add(ip, input, cptr->hwidb, &flkey, &crypto_tag);

	/*
	 * Only adjust prefix refcount if this is a real flow
	 * successfully inserted into flow cache.
	 */
	if (flow != &bogon_flow)
	  ipcache_adjust_refcount(cptr, 1);

	flow->cptr = cptr;
    }
    flow->tcp_flags |= flags;
    GET_TIMESTAMP32(flow->last_switched);
    return(flow);
}

/*
 * ipflow_pas_disabled:
 * Flow switching is disabled, handle it.
 */
static int ipflow_pas_disabled (void)
{
    return(IPFLOWERR_NOTCONF);
}

/*
 * ipoptimum_pas_fs:
 * IP Optimum switching for particle based systems
 */
static int ipoptimum_pas_fs (paktype *pak, iphdrtype *ip, hwidbtype *input)
{
    ipcache_rntype 	*cptr;
    hwidbtype 		*output;
    idbtype 		*idb;
    ulong 		iplen;
    crypto_tag_t 	crypto_tag = {0, 0};

    /*
     * Check the TTL and Version/Length
     */
    if ((ip->ttl <= 1) || (*(uchar *)ip != IPV4_LEN5)) {
	return(IPFLOWERR_NOTCONF);
    }

    /*
     * Checksum the IP header
     */
    if (ipflow_pas_hdr_sum_unaligned(ip)) {
	return(IPFLOWERR_CKS);
    }

    /*
     * Look up the radix tree
     */
    cptr = mnode_prefix_lookup(GETLONG(&ip->dstadr));
    if (!cptr) {
	return(IPFLOWERR_NOCACHE);
    }

    /*
     * Check MTU
     */
    iplen = ip->tl;
    idb = cptr->idb;
    output = cptr->hwidb;
    if (iplen > idb->ip_mtu) {
	return(IPFLOWERR_MTU);
    }

    /*
     * Do access checks and ip accounting if configured to do so.
     */
    pak->network_start = (uchar *)ip;
    if (output->ip_fast_flags) {
	if (!pak_flow_access_check(pak, input, idb, iplen, &crypto_tag)) {
	    return(IPFLOWERR_ACCESS);
	}
    }

    /*
     * Now decrement IP TTL and adjust checksum.
     */
    ipflow_pas_ttl_adjust(ip);

    /*
     * Re-encapulate IP packet.
     */
    if ((*cptr->c.encaps)(pak, ip, cptr, iplen) != IPFLOWOK) {
	/*
	 * Failed to perform rewrite because of rewrite particle
	 * exhaustion. Drop the packet
	 */
	input->counters.input_drops += 1;
	datagram_done(pak);
	return(IPFLOWOK);
    }

    /*
     * Increment interface and traffic counters. Send the
     * frame to the outbound interface.
     */
    pak->acct_proto = ACCT_PROTO_IP;
    ipflow_pas_update_rx_counters(input, iplen + input->encsize, 1);
    ipflow_pas_update_tx_counters(output, pak->datagramsize, 1);
    pak->if_output = output->firstsw;
    (*output->fastsend)(output, pak);
    return(IPFLOWOK);
}

/*
 * ipflow_pas_fs:
 * IP Flow switching for particle based systems
 */
static int ipflow_pas_fs (paktype *pak, iphdrtype *ip, hwidbtype *input)
{
    ipcache_rntype 	*cptr;
    hwidbtype 		*output;
    ipflow_t 		*flow;
    idbtype 		*idb;
    ulong 		iplen;

    /*
     * Check the TTL and Version/Length
     */
    if ((ip->ttl <= 1) || (*(uchar *)ip != IPV4_LEN5)) {
	return(IPFLOWERR_NOTCONF);
    }

    /*
     * Checksum the IP header
     */
    if (ipflow_pas_hdr_sum_unaligned(ip)) {
	return(IPFLOWERR_CKS);
    }

    /*
     * Lookup a flow
     */
    pak->network_start = (uchar *)ip;
    if (!(flow = ip_pas_flow_get(pak, ip, input))) {
	return(IPFLOWERR_NOCACHE);
    }
    cptr = flow->cptr;
    iplen = ip->tl;
    idb = cptr->idb;
    output = cptr->hwidb;
    flow->nexthop = cptr->nexthop;
    
    /*
     * Check MTU and update packet size distribution.
     */
    if (iplen > idb->ip_mtu) {
	return(IPFLOWERR_MTU);
    }
    ipflow_log_size_distribution(iplen);

    /*
     * Now decrement IP TTL and adjust checksum.
     */
    ipflow_pas_ttl_adjust(ip);

    /*
     * Re-encapulate IP packet.
     */
    if ((*cptr->c.encaps)(pak, ip, cptr, iplen) != IPFLOWOK) {
	/*
	 * Failed to perform rewrite because of rewrite particle
	 * exhaustion. Drop the packet
	 */
	input->counters.input_drops += 1;
	datagram_done(pak);
	return(IPFLOWOK);
    }
    flow->pkts++;
    flow->bytes += iplen;
    
    /*
     * Increment interface and traffic counters. Send the
     * frame to the outbound interface.
     */
    pak->acct_proto = ACCT_PROTO_IP;
    ipflow_pas_update_rx_counters(input, iplen + input->encsize, 1);
    ipflow_pas_update_tx_counters(output, pak->datagramsize, 1);
    pak->if_output = output->firstsw;
    (*output->fastsend)(output, pak);
    return(IPFLOWOK);
}

/*
 * ipflow_prep_rewrite:
 * Prepare for a rewrite requiring a rewrite particle
 */
static inline uchar *ipflow_prep_rewrite (paktype *pak, iphdrtype *ip, 
					  int bytes)
{
    particletype *rw_p;
    int 	 delta, encsize;

    /*
     * Allocate a rewrite particle
     */
    rw_p = allocate_fs_particle();
    if (!rw_p) {
	return(NULL);
    }

    /*
     * Snip off the inbound encaps and append new particle
     */
    encsize = ((uchar *)ip - pak->datagramstart);
    delta = bytes - encsize;
    particle_clip_inline(pak->particlequeue.qhead, -encsize);
    pak->datagramstart = rw_p->data_start;
    rw_p->data_bytes = bytes;
    pak->datagramsize += delta;
    requeue_inline(&pak->particlequeue, rw_p);
    return(rw_p->data_start + bytes);
}

/*
 * ipflow_pas_rewrite_short:
 * Rewrite encapsulation from cache struct to packet memory. Used
 * for rewrites that are guaranteed to fit over the inbound encapsulation.
 */
static inline void ipflow_pas_rewrite_short (paktype *pak, 
					     ipcache_rntype *cptr, 
					     uchar *dst, int bytes)
{
    int   delta, rewrite_bytes;
    ulong data, *src, *pak_dest;

    /*
     * Adjust packet and particle pointers
     */
    delta = bytes - (dst - pak->datagramstart);
    particle_clip_inline(pak->particlequeue.qhead, delta);
    pak->datagramstart -= delta;
    pak->datagramsize += delta;

    /*
     * Point src and pak_dest to the byte following the end of the rewrite.
     */
    rewrite_bytes = cptr->length;
    src = (ulong *)&cptr->m.macstring[rewrite_bytes];
    pak_dest = (ulong *)dst;

    /*
     * Copy the rewrite onto the packet backwards in 4 byte words
     */
    do {
	src -= 1;
	pak_dest -= 1;
	rewrite_bytes -= 4;

	data = GETLONG(src);
	PUTLONG(pak_dest, data);
    } while (rewrite_bytes > 0);
}
					   
/*
 * ipflow_pas_rewrite_generic:
 * Given a pointer to a paktype and a cache entry and the length of
 * the rewrite, perform the rewrite into a virgin particle.
 * Note, if the length is not an integral of 8, then some bogus 
 * pad bytes will overwrite up to 7 bytes before the dst pointer. Also
 * the dst pointer needs to point to the last byte after the rewrite.
 * This routine assumes no particular alignment restrictions.
 */
static inline void ipflow_pas_rewrite_generic (paktype *pak,
					       ipcache_rntype *cptr, 
					       uchar *dst,
					       int bytes)
{
    uchar *src = &cptr->m.macstring[bytes];
    ulong data;

    do {
	src -= 8;
	dst -= 8;
	bytes -= 8;

	asm volatile (".set mips3");
	asm volatile (".set noreorder");

	/*
	 * read 8 bytes of data from possibly unaligned source
	 * address into a 64-bit register.
	 */
	asm volatile ("ldl %0, 0(%1); ldr %0, 7(%1);"  
		      : "=&r" (data) : "r" (src) );

	/*
	 * write out 8 bytes of data to a potentially unaligned
	 * destination.
	 */
	asm volatile ("sdl %0, 0(%1); sdr %0, 7(%1);" 
		      : "=&r" (data) : "r" (dst) );

	asm volatile (".set reorder");
	asm volatile (".set mips2");
	
    } while (bytes > 0);
}

/*
 * ipflow_pas_rewrite_ether:
 * Rewrite Ethernet ARPA encapsulation from cache struct to
 * packet memory.
 */
static inline void ipflow_pas_rewrite_ether (paktype *pak, 
					     ipcache_rntype *cptr, 
					     uchar *dst)
{
    int   delta;
    ulong data;
    uchar *src;

    /*
     * Adjust packet and particle pointers
     */
    delta = ETHER_ARPA_ENCAPBYTES - (dst - pak->datagramstart);
    particle_clip_inline(pak->particlequeue.qhead, delta);
    pak->datagramstart -= delta;
    pak->datagramsize += delta;

    /*
     * Point src and dst to the byte following the end of the rewrite.
     */
    src = &cptr->m.macstring[ETHER_ARPA_ENCAPBYTES];

    asm volatile (".set mips3");
    asm volatile (".set noreorder");

    /*
     * One thing to note is that we copy
     * an extra 2 bytes prepending the MAC, since it's easier to
     * copy 16 bytes than it is to copy 14.
     */
    asm volatile ("ldl %0, -8(%1); ldr %0, -1(%1);" 
		  : "=&r" (data) : "r" (src) );
    asm volatile ("sdl %0, -8(%1); sdr %0, -1(%1);" 
		  :  : "r" (data), "r" (dst) );
    asm volatile ("ldl %0, -16(%1); ldr %0, -9(%1);"
		  : "=&r" (data) : "r" (src) );
    asm volatile ("sdl %0, -16(%1); sdr %0, -9(%1);"
		  :  : "r" (data), "r" (dst) );
    
    asm volatile (".set reorder");
    asm volatile (".set mips2");
}

/*
 * ipflow_encaps_short:
 * Rewrite any encapsulation whose complete rewrite string is
 * completely included in cptr->m.macstring and is guaranteed
 * to fit over the inbound encapsulation IE no rewrite particle needed.
 * This is used for PPP, HDLC and SLIP.
 */
static int ipflow_encaps_short (void *pak, iphdrtype *ip, 
				ipcache_rntype *cptr, ulong iplen)
{
    ipflow_pas_rewrite_short(pak, cptr, (uchar *)ip, cptr->length);
    return(IPFLOWOK);
}

/*
 * ipflow_encaps_generic:
 * Rewrite any encapsulation whose complete rewrite string is
 * completely included in cptr->m.macstring.
 * This is used for Channel, and ATM.
 */
static int ipflow_encaps_generic (void *pak, iphdrtype *ip, 
				  ipcache_rntype *cptr, ulong iplen)
{
    int encap_len = cptr->length;
    uchar *dst;

    /*
     * Prepare for the rewrite
     */
    dst = ipflow_prep_rewrite(pak, ip, encap_len);
    if (!dst) {
	return(IPFLOWERR_RESOURCE);
    }

    /*
     * Perform rewrite
     */
    ipflow_pas_rewrite_generic(pak, cptr, dst, encap_len);
    return(IPFLOWOK);
}

/*
 * ipflow_encaps_ether:
 * Handle ARPA ethernet rewrites
 */
static int ipflow_encaps_ether (void *pak, iphdrtype *ip, 
				ipcache_rntype *cptr, ulong iplen)
{
    ipflow_pas_rewrite_ether(pak, cptr, (uchar *)ip);
    return(IPFLOWOK);
}

/*
 * ipflow_encaps_ether_isl:
 * Handle ISL encapsulation on a fast-ethernet interface.
 */
static int ipflow_encaps_ether_isl (void *pak, iphdrtype *ip,
                                    ipcache_rntype *cptr, ulong iplen)
{
    idbtype *swidb = cptr->idb;
    ulong encap_len = ETHER_8023_ENCAPBYTES + ISL_HEADER_LENGTH;
    uchar *dst;

    /*
     * Prepare for the rewrite.
     */
    dst = ipflow_prep_rewrite(pak, ip, encap_len);
    if (!dst) {
        return(IPFLOWERR_RESOURCE);
    }

    /*
     * Perform the rewrite.
     */
    ipflow_pas_rewrite_generic(pak, cptr, dst, encap_len);
    ((paktype *)pak)->flags |= PAK_DO_ISL_CRC;
    swidb->isl_vlan->egressing_isl_packets[VLAN_ROUTE_IP]++;

    return(IPFLOWOK);
}

/*
 * ipflow_encaps_frame_relay:
 * Rewrite Frame Relay mac encapsulation from radix cache struct.
 */
static int ipflow_encaps_frame_relay (void *pak, iphdrtype *ip, 
				      ipcache_rntype *cptr, ulong iplen)
{
    int encap_len = cptr->length;

    /*
     * Do accounting
     */
    INC_PVC_FAST_OUTPUT(HWIDB_TO_FR_PVC(cptr->hwidb,
					cptr->subint_reference_num),
			iplen + encap_len);
    /*
     * Perform rewrite
     */
    ipflow_pas_rewrite_short(pak, cptr, (uchar *)ip, encap_len);
    return(IPFLOWOK);
}

/*
 * ipflow_encaps_tokenring:
 * Write out variable length tokenring encapsulation.
 */
static int ipflow_encaps_tokenring (void *pak, iphdrtype *ip,
				    ipcache_rntype *cptr, ulong iplen)
{
    int encap_len = cptr->length + IPCACHE_HARDCODED_ACFC_BYTES;
    uchar *dst;

    /*
     * If we can fit over the inbound encapsulation, do not 
     * allocate a rewrite particle.
     */
    if (encap_len < (PARTICLE_ENCAPBYTES-PARTICLE_SLOP_BYTES)) {
	/*
	 * Rewrite within the first particle
	 */
	ipflow_pas_rewrite_short(pak, cptr, (uchar *)ip, encap_len);
    } else {
	/*
	 * Prepare for the rewrite
	 */
	dst = ipflow_prep_rewrite(pak, ip, encap_len);
	if (!dst) {
	    return(IPFLOWERR_RESOURCE);
	}
	/*
	 * Perform rewrite
	 */
	ipflow_pas_rewrite_generic(pak, cptr, dst, cptr->length);
    }
    *(ushort *)(((paktype *)pak)->datagramstart) = (AC_PRI0 << 8)|FC_TYPE_LLC;
    return(IPFLOWOK);
}

/*
 * ipflow_encaps_fddi:
 * Allows variable length mac encaps to account for RIF.
 */
static int ipflow_encaps_fddi (void *pak, iphdrtype *ip,
			       ipcache_rntype *cptr, ulong iplen)
{
    int encap_len = cptr->length + IPCACHE_HARDCODED_FC_BYTES;
    uchar *dst;

    /*
     * If we can fit over the inbound encapsulation, do not 
     * allocate a rewrite particle.
     */
    if (encap_len < (PARTICLE_ENCAPBYTES-PARTICLE_SLOP_BYTES)) {
	/*
	 * Rewrite within the first particle
	 */
	ipflow_pas_rewrite_short(pak, cptr, (uchar *)ip, encap_len);
    } else {
	/*
	 * Prepare for the rewrite
	 */
	dst = ipflow_prep_rewrite(pak, ip, encap_len);
	if (!dst) {
	    return(IPFLOWERR_RESOURCE);
	}
	
	/*
	 * Perform the rewrite
	 */
	ipflow_pas_rewrite_generic(pak, cptr, dst, encap_len);
    }
    *((paktype *)pak)->datagramstart = FDDI_LLC_FC_BYTE;
    return(IPFLOWOK);
}

/*
 * ipflow_encaps_smds:
 * Rewrite SMDS mac encapsulation.
 */
static int ipflow_encaps_smds (void *pak, iphdrtype *ip, 
			       ipcache_rntype *cptr, ulong iplen)  
{
    ulong *macptr, *cache_macptr, *padptr;
    paktype *outpak = (paktype *)pak;
    particletype *tail_p;
    smdstype *sm;
    hwidbtype *output;
    uchar *dst;
    uint modelen, encaps_plus_ip_len, pad;

    /*
     * Prepare for the rewrite
     */
    output = cptr->hwidb;
    modelen = output->smds_pdb->fast_output_modelen;
    encaps_plus_ip_len = outpak->datagramsize - ((uchar *)ip - 
						 outpak->datagramstart);
    encaps_plus_ip_len += cptr->length;
    dst = ipflow_prep_rewrite(pak, ip, cptr->length);
    if (!dst) {
	return(IPFLOWERR_RESOURCE);
    }

    /*
     * Perform the rewrite
     */
    ipflow_pas_rewrite_generic(pak, cptr, dst, cptr->length);

    /*
     * Handle PAD and TRAILER
     */
    sm = (smdstype *)(outpak->datagramstart + modelen);
    pad = 3 - ((encaps_plus_ip_len + 3) % 4);
    sm->smds_xhlpi = (sm->smds_xhlpi & ~3) | pad;
    sm->smds_betag = output->smds_begin_tag++;
    sm->smds_basize = encaps_plus_ip_len - (4 + modelen) + pad;

    /*
     * SMDS pad
     */
    tail_p = outpak->particlequeue.qtail;
    padptr = (ulong *)(tail_p->data_start + tail_p->data_bytes);
    PUTLONG(padptr, 0);

    /*
     * SMDS trailer
     */
    outpak->datagramsize = encaps_plus_ip_len + pad;
    cache_macptr = (ulong *)&sm->smds_rsvd;
    macptr = (ulong *)(tail_p->data_start + tail_p->data_bytes + pad);

    /*
     * Copy the trailer and fix up the tail particle. We are assuming that
     * there is always room at the end of a particle for up to three bytes
     * of pad and four bytes of trailer.
     */
    PUTLONG(macptr, GETLONG(cache_macptr));
    tail_p->data_bytes += (pad + 4);
    return(IPFLOWOK);
}

/*
 * ipcache_encaps:
 * Assign output encapsulation vector to route cache struct.
 * Later: move this to registry call from ipcache_add().
 */
static ipencaps_t ipcache_encaps (ipcache_rntype *cptr)
{
    hwidbtype *output = cptr->hwidb;

    /*
     * Check for DDR interfaces
     */
    if (output->ip_fast_flags & IP_FAST_RSP_DDR) {
	return(NULL);
    }

    /*
     * Return the encap vector 
     */
    switch (output->fast_switch_type) {

    case FS_FDDI:
	return(ipflow_encaps_fddi);
    case FS_TOKEN:
	return(ipflow_encaps_tokenring);
    case FS_SMDS:
	return(ipflow_encaps_smds);
    case FS_FR:
	return(ipflow_encaps_frame_relay);
    case FS_ETHER:
        if (cptr->idb->isl_vlan)
            return(ipflow_encaps_ether_isl);
        else
            return(ipflow_encaps_ether);

    /*
     * For the following encapsulations, we only need to prefix
     * cptr->length bytes of cptr->macstring to the IP header, and 
     * we are guaranteed that this will fit over the inbound encapsulation.
     */
    case FS_HDLC:
    case FS_PPP:
    case FS_SLIP:
	return(ipflow_encaps_short);

    /*
     * For the following encapsulations, we only need to prefix
     * cptr->length bytes of cptr->macstring to the IP header.
     */
    case FS_AIP:
    case FS_CHANNEL:
	return(ipflow_encaps_generic);
	
    /*
     * Let the normal f/s routine have a shot at it...
     */
    default:
	break;
    }
    return(NULL);
}

/*
 * ipflow_pas_prefix_add:
 * Add the encap vector to the cache entry
 */
static void ipflow_pas_prefix_add (ipcache_rntype *cptr)
{
    cptr->c.encaps = ipcache_encaps(cptr);
    if (cptr->c.encaps) {
	mnode_prefix_add(cptr);
    }
}

/*
 * ipflow_pas_prefix_delete:
 * Delete the encap vector for a cache entry
 */
static void ipflow_pas_prefix_delete (ipcache_rntype *cptr)
{
    /*
     * Remove reference from mtrie.
     */
    if (cptr->c.encaps) {
	mnode_prefix_delete(cptr);
    }

    /*
     * Signal to flow switching code that this entry is now invalid.
     * We can use this rather than flags to avoid thrashing d-cache.
     */
    cptr->c.encaps = NULL;
}

/*
 * ipflow_pas_fs_register_drivers:
 * Add the registrys
 */
static void ipflow_pas_fs_register_drivers (ulong flags, hwidbtype* idb)
{
    /*
     * Check to see if we're configuring IP
     */
    if (!(flags & FSF_DODIP)) {
	return;
    }

    /*
     * Enable IP flow switching if enabled
     */
    if (idb->fast_switch_flag & FSF_DODIP) {

	/*
	 * No Flow/Optimum fastswitching to or from DDR interfaces.
	 */
	if (is_ddr(idb)) {
	    idb->ip_fast_flags |= IP_FAST_RSP_DDR;
	    idb->ip_routecache &= ~IP_OPTIMUM_ROUTECACHE;
	    idb->ip_routecache &= ~IP_FLOW_ROUTECACHE;
	} else {
	    idb->ip_fast_flags &= ~IP_FAST_RSP_DDR;
	}

	/*
	 * No optimum or flow switching if NAT is on (yet).
	 */
	if (idb->ip_fast_flags & IP_FAST_NAT) {
	    idb->ip_routecache &= ~IP_OPTIMUM_ROUTECACHE;
	    idb->ip_routecache &= ~IP_FLOW_ROUTECACHE;
	}

	if (idb->ip_routecache & IP_FLOW_ROUTECACHE) {
	    if (ipflow_table_init())
	      idb->ip_turbo_fs = (ip_turbo_fs_t) ipflow_pas_fs;
	} else {
	    if (idb->ip_routecache & IP_OPTIMUM_ROUTECACHE)
	      idb->ip_turbo_fs = (ip_turbo_fs_t) ipoptimum_pas_fs;
	    else
	      idb->ip_turbo_fs = (ip_turbo_fs_t) ipflow_pas_disabled;
	    ipflow_check_free_resources();
  	}
    }
}

/*
 * ipflow_pas_turbo_cache_idb_capable: 
 * Can we do flow swtiching on this interface
 */
static boolean ipflow_pas_turbo_cache_idb_capable (hwidbtype *hwidb)
{
    if (hwidb && !is_ddr(hwidb)) {

	/*
	 * Do we support turbo switching on this interface,
	 * ie - 'ip route-cache [flow|optimum]'
	 */
	switch(hwidb->type) {

	case IDBTYPE_AMDP2:
	case IDBTYPE_FEIP:
	case IDBTYPE_FDDIT:
	case IDBTYPE_IBM2692:
	case IDBTYPE_S4T68360:
	    return(TRUE);

	default:
	    return(FALSE);
	}
    }
    return(FALSE);
}

/*
 * ipflow_pas_default_optimum_on:
 * Default optimum switching 'on' for all supported interfaces.
 * Presnetly it is defaulted off. When it is fully regression tested
 * it should be defaulted on.
 */
static void ipflow_pas_default_optimum_on (hwidbtype *hwidb, boolean ip_on)
{
    if (ip_on && ipflow_pas_turbo_cache_idb_capable(hwidb)) {
	if (FALSE) {
	    hwidb->ip_routecache |= IP_OPTIMUM_ROUTECACHE;
	}
    }
}

/*
 * ipflow_pas_periodic: 
 * Perform periodic functionality.
 */
static void ipflow_pas_periodic (void)
{
    ipflow_periodic();
}

/*
 * ipflow_pas_fs_init:
 * Initialize IP flow switching subsystem
 */
static void ipflow_pas_fs_init (subsystype* subsys)
{
    reg_add_ip_platform_default(ipflow_pas_default_optimum_on,
				"ipflow_pas_default_optimum_on");
    reg_add_ip_turbo_idb_capable(ipflow_pas_turbo_cache_idb_capable,
				 "ipflow_pas_turbo_cache_idb_capable");
    reg_add_ip_flow_command_parse(ip_flow_cache_commands,
				  "ip_flow_cache_commands");
    reg_add_ip_cache_flow_show(ipflow_show, "ipflow_show");
    reg_add_auto_sw_ip_cache_show(optimum_ipcache_show,
				  "optimum_ipcache_show");
    reg_add_flush_ip_stats((service_flush_ip_stats_type)
			   ipflow_pas_periodic, "ipflow_pas_periodic");
    reg_add_register_drivers(ipflow_pas_fs_register_drivers,
			     "ipflow_pas_fs_register_drivers");
    reg_add_ipcache_add_callback(ipflow_pas_prefix_add,
 				 "ipflow_pas_prefix_add");
    reg_add_ipcache_delete_callback(ipflow_pas_prefix_delete,
				    "ipflow_pas_prefix_delete");
}

/*
 * IP PAS FS subsystem header
 */

#define IP_FLOW_PASFS_MAJVERSION 	1
#define IP_FLOW_PASFS_MINVERSION 	0
#define IP_FLOW_PASFS_EDITVERSION  	1

SUBSYS_HEADER(ipflow_pas_fs, IP_FLOW_PASFS_MAJVERSION, 
	      IP_FLOW_PASFS_MINVERSION, IP_FLOW_PASFS_EDITVERSION,
	      ipflow_pas_fs_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      "req: fastswitch, iphost");

/* end of file */
