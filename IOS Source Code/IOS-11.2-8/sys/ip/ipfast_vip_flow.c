/* $Id: ipfast_vip_flow.c,v 3.6.6.21 1996/09/11 20:09:36 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast_vip_flow.c,v $
 *------------------------------------------------------------------
 * VIP resident IP flow-cache fastswitching routines
 *
 * Oct 1995, Darren Kerr
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast_vip_flow.c,v $
 * Revision 3.6.6.21  1996/09/11  20:09:36  snyder
 * CSCdi68837:  more things const
 *              156 out of data, 64 image
 * Branch: California_branch
 *
 * Revision 3.6.6.20  1996/09/05  02:52:24  dlobete
 * CSCdi67904:  Small hole in ACL check for distributed flow switching
 * Branch: California_branch
 *
 * Revision 3.6.6.19  1996/08/12  06:12:26  dlobete
 * CSCdi59677:  distributed switching broken if MIP queuing fixed
 * Branch: California_branch
 * Eliminate mci_index from DFS scope and replace with the more
 * suitable hw_if_index. This not only fixes DFS to MIP channel-groups
 * but also paves the way for input from the new channelized port-adaptors
 * into DFS.
 *
 * Revision 3.6.6.18  1996/06/06  12:03:28  dlobete
 * CSCdi57944:  DFS confused about rx ignores and tx drops
 * Branch: California_branch
 * Also fix up an error in NACL processing at RSP.
 *
 * Revision 3.6.6.17  1996/06/06  09:39:56  dlobete
 * CSCdi54871:  faulty local switching accounting
 * Branch: California_branch
 * Turn off local-switching if we're overwhelming the local-output
 * interface. This way we can take advantage of MEMD's queueing
 * capability and basically equalize packet queueing from multiple
 * sources. This will also will fix the accounting hole.
 *
 * Revision 3.6.6.16  1996/06/06  09:24:57  dlobete
 * CSCdi57425:  VIP2 max cache entries wt. 32MB & 64MB are the same
 * Branch: California_branch
 *
 * Revision 3.6.6.15  1996/06/06  08:58:49  dlobete
 * CSCdi55797:  DACL ignores access-lists when switching to subinterfaces
 * Branch: California_branch
 * To have DFS handle IP output access-lists to multiple subinterfaces
 * (eg on a MIP) and to save the baggage of swidbs down at VIP,
 * the output-access-list is stashed in the DFS cache prefix entry.
 * Also don't enable DFS if multiple input ACLs exist on a VIP interface.
 *
 * Revision 3.6.6.14  1996/06/05  05:09:00  kao
 * CSCdi53846:  EOIR of EIP wipes route cache of entries for that interface
 * No cache invalidation if reset during EOIR
 * Populate txq and txacc down to all IPs after memd recarve for those
 * DFS caches pending on new txq and txacc
 * Branch: California_branch
 *
 * Revision 3.6.6.13  1996/06/03  18:22:11  getchell
 * CSCdi59268:  TTL handling on inbound DFS flow switching still munged.
 * Branch: California_branch
 *
 * Revision 3.6.6.12  1996/06/01  00:56:29  xliu
 * CSCdi59234: crypto rsp flow vector does not work correct for vip
 * encryption
 * Branch: California_branch
 *
 * Revision 3.6.6.9  1996/05/12  23:09:40  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.6.6.8  1996/05/08  01:09:53  gchristy
 * CSCdi56944:  Compilation errors from IP named access list commit
 * Branch: California_branch
 * - Modularize new ACL code for inclusion in VIP images.
 *
 * Revision 3.6.6.7  1996/04/30  23:52:21  dlobete
 * CSCdi56090:  Distributed switching on VIP2s breaks ICMP to Processor
 * Branch: California_branch
 * 4k cache side-effect breaks IP checksum calculation.
 *
 * Revision 3.6.6.6  1996/04/29  04:46:43  dlobete
 * CSCdi54163:  Distributed-switched packets discarded on ATM interface
 * Branch: California_branch
 *
 * Revision 3.6.6.5  1996/04/19  15:42:23  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.1.2.9  1996/04/11  23:15:11  carrel
 * Branch: IosSec_branch
 * IP code review comments #1
 *
 * Revision 3.1.2.8  1996/04/09  20:38:19  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 3.1.2.7  1996/03/30  05:39:36  xliu
 * Branch: IosSec_branch
 * Commit before CA split.
 *
 * Revision 3.1.2.6  1996/03/30  05:19:25  carrel
 * cvs fixes
 *
 * Revision 3.1.2.5  1996/03/19  08:09:32  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 3.1.2.4  1996/01/31  04:45:28  xliu
 * Branch: IosSec_branch
 * Add RSP/VIP encryption feature code.
 *
 * Revision 3.1.2.3  1996/01/23  05:43:36  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 3.1.2.2  1996/01/04  19:03:17  xliu
 * Branch: IosSec_branch
 * Replaced the direct crypto system call as the crypto subsys registry
 * call.
 *
 * Revision 3.1.2.1  1995/12/19  03:56:35  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 3.6.6.4  1996/04/12  17:42:42  kao
 * CSCdi53527:  Need EOIR support on VIP2
 * Branch: California_branch
 *
 * Revision 3.6.6.3  1996/04/10  03:38:34  dlobete
 * CSCdi53121:  Distributed IP Access Lists need to run at interrupt-level
 * Branch: California_branch
 *
 * Revision 3.6.6.2  1996/04/08  13:44:47  getchell
 * CSCdi52361:  DFS broken using unlike media
 * Branch: California_branch
 *
 * Revision 3.6.6.1  1996/03/21  22:50:28  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.4  1996/03/18  23:26:52  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.3  1996/03/07  17:28:28  getchell
 * CSCdi50953:  viper dfs mods
 * Branch: ELC_branch
 *
 * Revision 3.2.2.2  1996/02/08  07:48:18  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.1  1996/01/15  23:00:25  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.6  1996/03/04  07:12:42  kao
 * CSCdi49674:  While VIP is cofigured with DFS, OIR event will crash the
 * VIP;
 *
 * Revision 3.5  1996/02/24  02:14:41  dkerr
 * CSCdi49787:  IPC rolls over and dies under load
 * Make IPC more robust in an RSP->VIP distributed switching environment
 * (and increase performance from the miserly 100 msgs per minute to
 * something respectable, like 300K msgs per second).
 *
 * Revision 3.4  1996/01/19  03:02:58  dlobete
 * CSCdi44954:  Local-switching for DFS required
 * But that's not all! Also part of this jumbo Distributed IP
 * Switching commit:
 * - CSCdi43302: Illegal Access to Low Address crashes RVIP/4E (RSP2)
 * - DFS config which mirrors RSP route-cache setting
 *         (ie configurable distributed flow or optimal switching)
 * - Interrupt level IPC updates and invalidates for DFS cache operations
 * - Stay in closer synch with RSP cache for prefix invalidations
 *
 * Revision 3.3  1995/12/17  18:29:49  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:35:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:57:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:05:02  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/07  00:05:23  dkerr
 * Initial checkin.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "sys_registry.h"
#include "parser.h"
#include "ip_registry.h"
#include "sched.h"
#include "stacks.h"
#include "ttysrv.h"
#include "logger.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"
#include "../os/chunk.h"

#include "../if/ether.h"
#include "../if/fddi.h"
#include "../if/network.h"
#include "../if/atm.h"
#include "../if/tring.h"

#include "../src-vip/dfs_vip_idb.h"

#include "../ip/ip.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "../ip/ipaccount.h"
#include "../ip/ipfast_index.h"
#include "../ipmulticast/igmp.h"
#include "../ip/ipfast_vip.h"
#include "../ip/ipfast_vip_ipc.h"

#include "../wan/serial.h"
#include "../wan/frame_relay.h"
#include "../wan/atm_dxi.h"
#include "../wan/smds.h"


#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_tcpip.h"

#include "../pas/if_pas.h"

#include "../src-vip/vip_memd.h"
#include "../src-vip/rsp_memd.h"

#include "../crypto/crypto_public.h"
#include "../crypto/crypto_registry.h"

extern ushort fastiphdrchk(ushort *, ulong);

void ipflow_free(ipflow_t *);
ipflow_t *ipflow_create(ipflow_key_t *, paktype *, hwidbtype *);
ipflowtbl_t ipflowtbl;
static chunk_type *ipfast_vip_flow_chunk;

/*
 * IP Checksum and TTL/CKS update using only 16-bit reads and
 *   writes to guard against potential odd word alignment
 *   caused by annoying encapsulations.
 *
 * The TTL and CKS are modified first to allow time for rest
 *   of line to fill the Orion's d-cache.  If the IP header
 *   starts at offset 8 in a cache line, then the Orion will
 *   fill it's d-cache in the following order: 2,3,0,1
 *   (see sub-block ordering in manual).  Therefore, after
 *   updated the ttl/cks, we should checksum the IP source
 *   address, then destination address, then version & length.
 */
static inline ushort ip_cks_and_ttl_adjust_unaligned (iphdrtype *ip)
{
    ushort *ptr = (ushort *)ip;
    ushort cks, ttl, ssum;
    long lsum;

    cks = ptr[5];

    ttl = ptr[4];
    if (ttl <= 1) {
	return 1;
    }

    lsum = ttl;
    lsum += cks;

    if (cks >= 0xFEFF)
	cks++;
    ptr[5] = cks + 0x100;
    ptr[4] = ttl - 0x100;

    /*
     * Don't change the order of these ops without first checking
     * the sub-block ordering (for our fastest encapsulation)
     * with a logic analyzer (or oscilliscope, though I doubt any
     * of our sw weenies knows how to operate one :-).
     */
    lsum += ptr[6];
    lsum += ptr[7];
    lsum += ptr[8];
    lsum += ptr[9];
    lsum += ptr[0];
    lsum += ptr[1];
    lsum += ptr[2];
    lsum += ptr[3];
    ssum = lsum + (lsum >> 16);
    return(~ssum);
}

/*
 * VIP IP Fastswitching using flow cache lookup.
 *
 */
boolean vip_ip_flow_fs (hwidbtype *input, paktype *pak)
{
    iphdrtype *ip = (iphdrtype *)ipheadstart(pak);
    ipflow_key_t flkey;
    ipflow_t *flow;
    uchar tcp_flags;
    encapinfotype *eptr;
    ulong iplen;
    dfsidbtype *output;
    pas_cbtype *pascb = INSTANCE(input);
    pas_cbtype *pascb_out;
    void *bhptr;
    int input_encap_len, diff;
    particletype *mp;
    boolean retval;

    /*
     * Pull in IP src/dst addresses, protocol, tos, and port numbers.
     */
    tcp_flags = ipflow_key_get(ip, &flkey);
    flkey.fl_input = input->mci_index;

    if ((flow = ipflow_hash_lookup(&flkey, tcp_flags)) == NULL) 
      if ((flow = ipflow_create(&flkey, pak, input)) == NULL) {
	/*
         * Just punt this packet up to the host
	 */
	  retval = (*pascb->pak_to_host)(input, pascb, pak);
	  if (!retval)
	      input->counters.input_drops++;
	  return(retval);
      }


    /*
     * LATER: if ((tcp_flags & TCP_FLAGS_EST) == 0) and
     * (flow->tcp_flags_needed & TCP_FLAGS_EST) then do access checks.
     */

    /*
     * Save last set of tcp flags in flow entry so that accounting
     * code can accelerate decommission of TCP flows when FIN seen.
     */
    flow->tcp_flags_last = tcp_flags;

    /*
     * Get the encapsulation rewrite info struct from flow entry.
     */
    eptr = flow->eptr;
    output = eptr->idb;

    iplen = ip->tl;
	/*
     * EOIR only invalidates those cache entries whose output interface
     * is removed. VIP still need to nullify both tx_acc_ptr and tx_q_ptr
     * of all output interfaces since MEMD recarve will assign new ones
     * for each interface. tx_acc_ptr will remain NULL briefly after
     * UNQUIESCE until IPFAST_UPDATE_IDB DFS/IPC message sent by RSP
     * for each interface is received. IPFAST_UPDATE_IDB messages are sent 
     * right after MEMD recarve.
     */
    if (iplen > eptr->net_mtu || output->tx_acc_ptr == NULL) {
	/*
         * Let host handle fragmentation
	 */
	retval = (*pascb->pak_to_host)(input, pascb, pak);
	if (!retval)
	    input->counters.input_drops++;
	return(retval);
    }

    input_encap_len = pak->network_start - pak->datagramstart;
    diff = pak->datagramsize - input_encap_len;
    if (iplen != diff) {
	if (iplen < diff) {
	    /*
	     * Extra bytes prepended to packet end - strip them off in order
	     * to mimic what RSP does
	     */
	    ipfast_prefix_badiplen++;
	    diff -= iplen;
	    while (diff){
		mp = pak->particlequeue.qtail;
  
		if (!mp) {
		    /*
		     * REALLY bogus length!
		     */
		    input->counters.input_drops++;
		    return(FALSE);
		}
  
		if (mp->data_bytes <= diff) {
		    /*
		     * We need to throw away this particle
		     */
		    diff -= mp->data_bytes;
		    mp = pak->particlequeue.qtail;
		    particle_unqueue_inline(pak,mp);
		    retparticle_inline(mp);
		    
		} else {
		    /* Adjust byte count of last particle */
		    mp->data_bytes -= diff;
		    diff = 0;
		}
	    }
	} else {
	    /* iplen > diff - runt packet */
	    input->counters.input_frame++;
	    return(FALSE);
	}
    }

    /*
     * Check IP header
     */
    if (ip_cks_and_ttl_adjust_unaligned(ip)) {
        /*
         * Let host handle bad checksum accounting.
         */
        retval = (*pascb->pak_to_host)(input, pascb, pak);
        if (!retval)
            input->counters.input_drops++;
        return(retval);
    }

    /*
     * We need to read the memd tx accumulator and buffer
     * header before we load up the reno's with writes.  It's
     * not pretty, but necessary.
     */

    if (!output->local_sw_vector) {
	/*
	 * Spill it if outbound tql is too low.
	 */
	if (!vip_rsp_check_tql((volatile ushort *)output->tx_acc_ptr)) {
	    output->outputdrops++;
	    return(FALSE);
	}
 
	/*
	 * Grab a MEMD buffer.
	 */
	if ((bhptr = (void *)vip_dfs_bh_allocate(pascb, pak->datagramsize)) == 0) {
	    /*
	     * Need to increment the tql, drop the packet,
	     * and return.
	     */
	    *(volatile ushort *)output->tx_acc_ptr = -1;
	    input->counters.input_drops++;
	    return(FALSE);
	}
    } else {
        /*
         * Check for congestion on the outbound txq (this check should
         * mimmick the check used by vip_dtq_consumer()). This condition
         * will usually happen when we have faster local input
         * interface feeding a slower local output interface. To avoid
         * unfairly dropping this local-swiched stream or overwhelming
         * the txq (causes output-stuck condition in RSP), let's turn off
         * local-switching and queue to MEMD - can try to turn on later.
         */
        if (vsys_vec.viper) {
            pascb_out = INSTANCE(output->hwidb);
            if (*pascb_out->txring_cnt_ptr >= pascb_out->tx_polling_high) {
                output->local_sw_vector = NULL;
		output->dfs_throttled = TRUE;
		output->outputdrops++;
                return(FALSE);
            }
        } else {
            if (output->hwidb->output_qcount) {
                output->local_sw_vector = NULL;
		output->dfs_throttled = TRUE;
		output->outputdrops++;
                return(FALSE);
            }
        }
	bhptr = NULL;
    }

    /*
     * Call encapsulation rewrite vectore.
     */
    if ((*eptr->evector)(pak, eptr, input, input_encap_len, iplen, bhptr)) {
	flow->pkts++;
	flow->bytes += iplen;
	ipflowtbl.switched++; /* temporary */
	GET_TIMESTAMP(flow->last_switched);
	return(TRUE);
    }
    return(FALSE);
}

/*
 *
 */
ipflow_t *ipflow_create (ipflow_key_t *key, paktype *pak, hwidbtype *input)
{
    ipflow_t *flow;
    ipcachetype *cptr;
    encapinfotype *eptr;
    dfsidbtype *output;


    cptr = ipfast_prefix_lookup(key->fl_dst);
    if (!cptr || !(cptr->flags & IPCACHE_FLAG_VALID))
      return(NULL);

    /*
     * Do IP access checks.
     * This would include encryption/decryption check, and subsequent
     * creation of special mac rewrite.
     */
    eptr = cptr->eptr;
    output = eptr->idb;
    if (output->ip_fast_flags)
	if (!vip_ip_access_check(pak, output->ip_fast_flags,
				 input->fast_rcvidb->ip_input_accesslist,
				 output->ip_output_accesslist))
	    return(NULL);

    pak->cryptoflags = 0;
    pak->crypto_enc_conn_id = 0;
    pak->crypto_dec_conn_id = 0;
    if (input->firstsw->crypto_map) {
        if (!reg_invoke_crypto_map_check(pak, input->firstsw->crypto_map,
                                         CRYPTO_DECRYPT, TRUE) ||
             crypto_deny_packet(pak)) {
            return (NULL);
        }
        if (pak->cryptoflags && (pak->crypto_dec_conn_id <=0)) {
            return (NULL);
        }
    }
    
    flow = ipflow_alloc();
    if (!flow)
      return(NULL); /* later: we could still do normal switching! */

    
    flow->key = *key;
    flow->cptr = cptr;
    cptr->refcount++;

    flow->crypto_tag.cryptoflags = pak->cryptoflags;
    flow->crypto_tag.crypto_conn_id = pak->crypto_dec_conn_id;

    /*
     * LATER: check for encryption access list.
     * If the encryption access list requires additional encapsulation,
     * then we need to allocate an encapinfo struct and point flow->eptr
     * at it.
     */
    flow->eptr = cptr->eptr;
    
    GET_TIMESTAMP(flow->first_switched);
    
    ipflow_hash_insert(flow);
    return(flow);
}

void flush_total_flow_stats (ipflow_t *flow)
{
    /*
     * LATER:
     * _ add this flow's total bytes/pkts to cumulative flow stat table
     * _ update extended ip accounting if configured
     * _ update rommon2 flow stats if configured
     */
    flow->bytes = 0;
    flow->pkts = 0;
}

/*
 * Flush flow's lifetime stats, free all resources, and decrement
 * refcounts associated with the flow.
 */
void decommission_flow (ipflow_t *flow)
{
    ipcachetype *cptr = flow->cptr;
    
    flush_total_flow_stats(flow);
    flow->cptr = NULL;
    cptr->refcount--;
    free_ipcache_entry(cptr);
    ipflow_free(flow);
}

/*
 * We don't want to have network interrupts disabled for an extended
 * period of time, yet we also don't want to keep enabling and disabling
 * interrupts either.
 * Let's only disable network interrupts for *roughly* 10 microseconds
 */
#define NET_DISABLE_MAX_NANOSECS       20000 /* 20uS max disable */
#define NANOSECS_PER_EMPTY_BUCKET_POLL   100 /* ~100nS to poll bucket */
#define NANOSECS_PER_FLOW_FLUSH         2000 /* ~2uS to process valid flow */ 

void flow_periodic (void)
{
    ipflow_t *next, *prev, *flow;
    leveltype level;
    int bkt, net_disable_counter = NET_DISABLE_MAX_NANOSECS;
    
    if (ipflowtbl.active_flows) {

	level = raise_interrupt_level(NETS_DISABLE);
	
	for (bkt=0; bkt<IPFLOWTBL_SIZE; bkt++) {
	    prev = (ipflow_t *)&ipflowtbl.bucket[bkt];
	    for (flow = prev->next; flow; flow = next) {
		next = flow->next;

		net_disable_counter -= NANOSECS_PER_FLOW_FLUSH;
		
		if (!(flow->cptr->flags & IPCACHE_FLAG_VALID)) {
		    ipflowtbl.invalidated++;
		} else if (flow->tcp_flags_last & TCP_FLAGS_FIN) {
		    /*
		     * We've seen a FIN on this connection, so the chances
		     * are very high that we won't see any more packets
		     * on this flow.
		     */
		    ipflowtbl.tcp_fin_seen++;
		} else if (ELAPSED_TIME(flow->last_switched) >
			 IPFLOW_IDLE_TIMEOUT) {
		    ipflowtbl.timed_out++;
		} else if (flow->bytes > 0x80000000) {
		    /*
		     * This is just to swap out 32-bit flow counters
		     * to the 62-bit global counters during benchmarks.
		     */
		    ipflowtbl.timed_out++;
		} else {
		    continue;  /* leave valid flow in hash table */
		}
		/*
		 * One of above conditions indicate that we should
		 * remove this flow from the hash table, so flush stats,
		 * decrement refcounts, and de-alloc flow.
		 * Also remove flow from hash chain.
		 */
		decommission_flow(flow);
		prev->next = next;
		flow = prev;
	    }
	    
	    if ((net_disable_counter -= NANOSECS_PER_EMPTY_BUCKET_POLL) < 0) {
		reset_interrupt_level(level);
		level = raise_interrupt_level(NETS_DISABLE);
		net_disable_counter = NET_DISABLE_MAX_NANOSECS;	
	    }
	}
	reset_interrupt_level(level);
    }
}

/*
 * Periodic routine to manage flow memory and flush flow stats.
 */
void ipflow_tensec (void)
{
    ipflow_t *flow;
    int free_flows;
    leveltype level;

    flow_periodic();

    free_flows = ipflowtbl.alloced_flows - ipflowtbl.active_flows;
    if (free_flows >= 0) {
	while ((free_flows++ < IPFLOW_FREE_RESERVE) &&
	       (ipflowtbl.alloced_flows < IPFLOW_ALLOC_MAX)) {
	    
	    flow = chunk_malloc(ipfast_vip_flow_chunk);
	    if (!flow)
	      break;
	    
	    /*
	     * Now put malloc'd flow onto interrupt-level free queue.
	     */
	    level = raise_interrupt_level(NETS_DISABLE);
	    flow->next = ipflowtbl.flow_pool;
	    ipflowtbl.flow_pool = flow;
	    reset_interrupt_level(level);
	    ipflowtbl.alloced_flows++;
	}
    }
}

void print_ip_15 (ipaddrtype prefix)
{
    uchar c[4];
    int len;

    *(ulong *)c = prefix;
    len = printf("%d.%d.%d.%d", c[0], c[1], c[2], c[3]);
    printf("%*c", 15-len, ' ');
}

    
static const char rstring[] =
  "\nSrcIf SrcIPaddress    DstIf DstIPaddress    Pr DstP SrcP Pkts B/Pkt Active Idle";
/*
SrcIf SrcIPaddress    DstIf DstIPaddress    Pr DstP SrcP Pkts B/Pkt Active Idle
Fd4/0 15.1.100.41     Et3/0 171.168.132.111 06 5608 0017   10   100   10.0 64.2
Fd4/0 15.1.100.41     Et3/0 15.1.16.1       06 5608 0017   22    60    2.3 13.3
*/

/*
 * vip_ipflow_show
 */
void vip_ipflow_show (ipaddrtype prefix, ipaddrtype mask)
{
    ipflow_t *flow;
    ipflowshow_t *show, *showbuf;
    leveltype level;
    int bkt, idx;
    ulong pkts, bytes;
    char *pktstr;
    
    automore_enable(NULL);
    
    printf("\nIP Flow Cache, %d active, %d alloced, %lu opened",
	   ipflowtbl.active_flows, ipflowtbl.alloced_flows, ipflowtbl.added);
    printf("\n  %d closed: %d tcp-fin, %d idle-time, %d prefix-inval",
	   (ipflowtbl.tcp_fin_seen +
	    ipflowtbl.timed_out +
	    ipflowtbl.invalidated),
	   ipflowtbl.tcp_fin_seen, ipflowtbl.timed_out, ipflowtbl.invalidated);
    printf("\n  %d create failures, %lu switched",
	   ipflowtbl.create_failures, ipflowtbl.switched);

    /*
     * Don't print header if no flows are active!
     */
    if (!ipflowtbl.active_flows)
	return;

    automore_header(rstring);

    showbuf = malloc(sizeof(ipflowshow_t) * IPFLOWSHOWMAX);
    if (!showbuf) {
	printf(nomemory);
	return;
    }

    for (bkt=0; bkt<IPFLOWTBL_SIZE; bkt++) {
	level = raise_interrupt_level(NETS_DISABLE);
	flow = (ipflow_t *)ipflowtbl.bucket[bkt];
	for (idx=0; flow && (idx<IPFLOWSHOWMAX); flow = flow->next) {
	    if ((flow->key.fl_dst & mask) != prefix)
	      continue;
	    if (!(flow->cptr->flags & IPCACHE_FLAG_VALID))
	      continue;
	    showbuf[idx].key = flow->key;
	    if (!(showbuf[idx].pkts = flow->pkts))
	      continue; /* avoid divide by zero */
	    showbuf[idx].bytes = flow->bytes + flow->pkts * flow->eptr->length;
	    showbuf[idx].duration = CLOCK_DIFF_UNSIGNED(flow->first_switched,
							flow->last_switched);
	    showbuf[idx].idle = ELAPSED_TIME(flow->last_switched);
	    showbuf[idx].instr = "vip";
	    showbuf[idx].input_index = flow->key.fl_input;
	    showbuf[idx].outstr = dfs_get_namestring(flow->eptr->idb);
	    showbuf[idx].output_index = dfs_get_slotunit(flow->eptr->idb);
	    idx++;
	}
	reset_interrupt_level(level);

	show = showbuf;
	for ( ;idx-- > 0; show++) {
	    ulong active, idle;
	    
	    pkts = show->pkts;
	    bytes = show->bytes / pkts;
	    active = show->duration / 100;
	    idle = show->idle / 100;

	    if (pkts > 9999999) {
		pkts /= 1000000;
		pktstr = "M";
	    } else if (pkts > 9999) {
		pkts /= 1000;
		pktstr = "K";
	    } else
	      pktstr = " ";
	    
	    printf("\n%3s%02x ", show->instr, show->input_index);
	    print_ip_15(show->key.fl_src);
	    printf(" %3s%02x ", show->outstr, show->output_index);
	    print_ip_15(show->key.fl_dst);
	    printf(" %02x %04x %04x",
		   show->key.fl_prot, show->key.fl_dport, show->key.fl_sport);
	    printf(" %-4d%s%-5d %-4d.%d %-2d.%d",
		   pkts, pktstr, bytes, active / 10, active % 10,
		   idle / 10, idle % 10);
	}
	if (automore_quit())
	  break;
    }
    free(showbuf);
    automore_disable();
}
    
void ipfast_vip_flow_init (ulong mem_size) 
{
    ulong flow_entries;

    /*
     * Set up our chunk for the flow cache.
     */
    switch (mem_size) {
      case DFS_64MB_SIZE:
	flow_entries = IPFLOW_ALLOC_MAX_HUGE;
	break;

      case DFS_32MB_SIZE:
	flow_entries = IPFLOW_ALLOC_MAX_LARGE;
        break;
 
      case DFS_16MB_SIZE:
	flow_entries = IPFLOW_ALLOC_MAX_MEDIUM;
        break;
 
      case DFS_8MB_SIZE:
      default:
	flow_entries = IPFLOW_ALLOC_MAX_SMALL;
        break;
    }

	ipfast_vip_flow_chunk = chunk_create(sizeof(ipflow_t), 
					 flow_entries,
                                             CHUNK_FLAGS_NONE, NULL,
                                             0, "DFS Flow Cache");
}

