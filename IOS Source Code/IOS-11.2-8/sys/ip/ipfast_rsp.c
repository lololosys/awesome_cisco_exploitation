/* $Id: ipfast_rsp.c,v 3.9.6.28 1996/09/15 15:46:31 xliu Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast_rsp.c,v $
 *------------------------------------------------------------------
 * IP Flow Switching
 *
 * Feb 1995, Darren Kerr
 *
 * Copyright (c) 1995-1996,1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast_rsp.c,v $
 * Revision 3.9.6.28  1996/09/15  15:46:31  xliu
 * CSCdi69200:  RSP crypto image  crashed when dram buffer allocation
 * return NULL
 * Branch: California_branch
 * Add macro definiton.
 *
 * Revision 3.9.6.27  1996/09/07  02:17:07  kao
 * CSCdi54108:  Optimum and Flow switching performance degradation
 * Performance degraded with more and more new supported features
 * This fix actually optimizes several places in various routines to
 * increase
 * the optimum/flow switching performance for RSP
 * Branch: California_branch
 *
 * Revision 3.9.6.26  1996/09/03  06:37:23  xliu
 * CSCdi67091:  encrypiton code should check ip_fast_flags in optimum
 * switching
 * Branch: California_branch
 *
 * Revision 3.9.6.25  1996/09/03  05:49:06  xliu
 * CSCdi67091:  encrypiton code should check ip_fast_flags in optimum
 * switching
 * Branch: California_branch
 * When the packet is output through vip, don't call crypto vectors.
 *
 * Revision 3.9.6.24  1996/08/27  02:57:02  xliu
 * CSCdi67091:  encrypiton code should check ip_fast_flags in optimum
 * switching
 * Branch: California_branch
 * Check crypto flag only when ip_fast_flag is on, in optimum switching.
 *
 * Revision 3.9.6.23  1996/08/22  01:07:37  xliu
 * CSCdi64300:  Need to investigate possible RSP memd buffer corruption
 * Branch: California_branch
 * Let the crypto vector handle the cache writethrough before return
 * IPFLOWOK.
 *
 * Revision 3.9.6.22  1996/08/16  22:19:54  xliu
 * CSCdi64300:  Need to investigate possible RSP memd buffer corruption
 * Branch: California_branch
 * Invalidate write through cache line before return.
 *
 * Revision 3.9.6.21  1996/07/22  18:52:12  hou
 * CSCdi59675:  Fast-switching is broken from isl to non-isl network.
 * Branch: California_branch
 * - Add an optimum switching vector for FastEthernet ISL packet.
 *
 * Revision 3.9.6.20  1996/07/08  22:34:25  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.9.6.19  1996/07/06  05:53:46  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.9.6.18  1996/06/01  16:03:08  kao
 * CSCdi57759:  VIP2 system exception & DBUS-3-DBUSINTERR error when doing
 * eoir.
 * Flush DFS/IPC message before memd recarve to have a consistent DFS
 * cache after EOIR.
 * Branch: California_branch
 *
 * Revision 3.9.6.17  1996/06/01  09:30:51  xliu
 * CSCdi59258:  crypto rsp fastswitch vector should check vip interface.
 * Branch: California_branch
 *
 * Revision 3.9.6.16  1996/06/01  00:56:24  xliu
 * CSCdi59234: crypto rsp flow vector does not work correct for vip
 * encryption
 * Branch: California_branch
 *
 * Revision 3.9.6.15  1996/06/01  00:25:00  ahh
 * CSCdi59212:  NAT: unsupported switching paths can be disabled more se...
 * Branch: California_branch
 *
 * Revision 3.9.6.14  1996/05/28  16:13:48  xliu
 * CSCdi58788:  code changes for vip named access list and encryption perf
 * Branch: California_branch
 *
 * Revision 3.9.6.13  1996/05/21  09:51:23  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.9.6.12  1996/05/13  23:20:49  ahh
 * CSCdi56426:  NAT: EFT bugs and feedback
 * Fix hash algorithm, fix address block coalescing, reduce up-front
 * memory allocation, drop untranslatable packets, clean up debugs.
 * Branch: California_branch
 *
 * Revision 3.9.6.11  1996/05/07  20:00:51  xliu
 * CSCdi56777:  memd buffers are queued to process level for
 * encryption/decryption
 * Branch: California_branch
 *
 * Revision 3.9.6.10  1996/05/04  20:49:25  fred
 * CSCdi56013:  alignment problems with moving data to paks
 * Branch: California_branch
 *
 * Revision 3.9.6.9  1996/05/04  01:04:05  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - ARP, IP protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on CRB/IRB is enabled or not.
 *
 * Constrained Multicast Flooding
 * - Convert tbridge flood array to a linked list of tbifd's.
 *
 * Name Access List
 *
 * Revision 3.9.6.8  1996/04/30  21:18:03  dkerr
 * CSCdi43050:  IP lookup bottleneck for RSP in ISP test setup
 * Branch: California_branch
 *
 * Revision 3.9.6.7  1996/04/30  19:53:40  dkerr
 * CSCdi54321:  RSP - Cbus complex QAerror at 80kpps with two FDDIs on VIP2
 * Branch: California_branch
 *
 * Revision 3.9.6.6  1996/04/29  15:48:49  fred
 * CSCdi40287:  fsip is dropping large # of packets when fair-queue is on
 * Branch: California_branch
 *
 * Revision 3.9.6.5  1996/04/26  00:13:48  xliu
 * CSCdi55822:  Variable crypto_tag should be initialized before being
 * used.
 * Branch: California_branch
 *
 * Revision 3.9.6.4  1996/04/24  00:06:12  ahh
 * CSCdi55483:  NAT: code review comments, various bug fixes
 * Fast-path fixup, cruft removal.
 * Branch: California_branch
 *
 * Revision 3.9.6.3  1996/04/19  15:42:06  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.17  1996/04/11  23:15:07  carrel
 * Branch: IosSec_branch
 * IP code review comments #1
 *
 * Revision 2.1.6.16  1996/04/09  20:37:50  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.1.6.15  1996/03/30  05:39:22  xliu
 * Branch: IosSec_branch
 * Commit before CA split.
 *
 * Revision 2.1.6.14  1996/03/21  10:12:41  xliu
 * Branch: IosSec_branch
 * Modified comments.
 *
 * Revision 2.1.6.13  1996/03/21  10:04:15  xliu
 * Branch: IosSec_branch
 * Migrated to V111_1_3.
 *
 * Revision 2.1.6.12  1996/03/19  08:09:19  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 3.9.6.2  1996/04/17  13:41:51  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.9.6.1  1996/03/18  20:17:03  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.2  1996/03/16  06:58:52  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.7.2.1  1996/03/13  01:37:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.9  1996/03/04  15:31:07  dkerr
 * CSCdi50623:  IP flow cache needs better accounting granularity
 * Better show output.  Also allow flow stats to be exported.
 *
 * Revision 3.8  1996/02/24  02:14:28  dkerr
 * CSCdi49787:  IPC rolls over and dies under load
 * Make IPC more robust in an RSP->VIP distributed switching environment
 * (and increase performance from the miserly 100 msgs per minute to
 * something respectable, like 300K msgs per second).
 *
 * Revision 3.7  1996/01/20  19:11:04  lbustini
 * CSCdi38705:  DDR optimum switching for RSP doesnt reset idle timer.
 * Disable RSP fastswitching on DDR interfaces.
 *
 * Revision 3.6  1996/01/19  03:02:52  dlobete
 * CSCdi44954:  Local-switching for DFS required
 * But that's not all! Also part of this jumbo Distributed IP
 * Switching commit:
 * - CSCdi43302: Illegal Access to Low Address crashes RVIP/4E (RSP2)
 * - DFS config which mirrors RSP route-cache setting
 *         (ie configurable distributed flow or optimal switching)
 * - Interrupt level IPC updates and invalidates for DFS cache operations
 * - Stay in closer synch with RSP cache for prefix invalidations
 *
 * Revision 3.5  1995/12/17  18:29:44  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.4  1995/11/22  22:32:57  mcieslak
 * CSCdi42620:  rsp crashed w/SegV exception  (ipflow_tx_congestion)
 * Remove the use of "ipflow_encaps_unsupported" as the default because
 * it was not being setup properly.
 *
 * Revision 3.3  1995/11/17  09:35:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:40:49  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:57:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/11/08  21:04:55  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.9  1995/10/21  19:21:12  fred
 * CSCdi40287:  fsip is dropping large # of packets when fair-queue is on
 *         mci: set up pak->if_output in congestion case (CQ/PQ/RSVP)
 *         rsp: correct use of queuing routines - must use pak->oqueue()
 *              and pak->oqueue_dequeue(), was using
 *              holdq_enqueue_fast_reserve
 *              and pak->oqueue_dequeue()
 *
 * Revision 2.8  1995/09/25  08:32:12  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.7  1995/08/09  00:20:33  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.6  1995/07/20  17:02:21  shaker
 * CSCdi37384:  Optimum and Flow switching can output packets out of order
 * Make optimum and flow switching always defer to any output on the output
 * hold queue.
 *
 * Revision 2.5  1995/07/19  06:47:33  shaker
 * CSCdi36264:  fast and optimum switching dont prime returnq
 * Bring 11.0 code up to date with solution in 10.3 code.
 *
 * Revision 2.4  1995/07/10  05:39:48  fred
 * CSCdi36851:  disable fancy queuing if using autonomous, sse, flow, or
 * optimal switching - ie, only use it if process switching or fast
 * switching. Applies to AGS/7000/7500 platforms.
 *
 * Revision 2.3  1995/06/21  08:55:00  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.2  1995/06/19  01:06:35  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:59:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "registry.h"
#include "logger.h"
#include "../os/free.h"
#include "../ui/common_strings.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "../if/ether.h"
#include "../if/fddi.h"
#include "config.h"
#include "../ip/ip.h"
#include "../ipmulticast/igmp.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "../iprouting/route.h"
#include "../ip/ipaccount.h"
#include "../ip/ipfast.h"
#include "../if/tring.h"
#include "../les/if_les.h"
#include "../wan/frame_relay.h"
#include "../wan/atm_dxi.h"
#include "../wan/smds.h"
#include "../wan/dialer.h"
#include "../if/network.h"
#include "../if/atm.h"

#include "../src-rsp/rsp_bufhdr.h"
#include "../src-rsp/rsp_if.h"
#include "../src-rsp/rsp_qasic.h"
#include "../src-rsp/qa_hw.h"
#include "../src-rsp/rsp_fast.h"
#include "../src-rsp/rsp_pcmap.h"
#include "../src-rsp/rsp_cache.h"
#include "../ip/ipfast_flow.h"
#include "../ip/ipfast_rsp.h"
#include "../ip/ipfast_rsp_mtrie.h"
#include "../ip/ipfast_rsp_ipc.h"
#include "../ip/udp_flood_fs.h"

#include "sys_registry.h"

#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_tcpip.h"
#include "ip_registry.h"

#include "../crypto/crypto_registry.h"
#include "../crypto/crypto_public.h"
#include "../vlan/vlan.h"

static ipencaps_t ipcache_encaps(ipcache_rntype *cptr);
boolean rsp_holdq_enqueue(bufhdr_t *bufptr, hwidbtype *output, 
                          void *ip, ushort tql);
paktype *rsp_oqueue_getbuffer(ushort size);

/*
 * RSP IP Fastswitching Routine
 * This is written for (almost) optimal performance on the RSP platform.
 */
int rsp_ipfastswitch (bufhdr_t *bufptr, iphdrtype *ip, hwidbtype *input)
{
    ipcache_rntype *cptr;
    hwidbtype *output;
    idbtype *idb;
    ushort tql;
    ulong iplen;

    /*
     * ip_hdr_sum_dstadr is same as ip_hdr_sum except it returns
     * the value of ip->dstadr. When IP header checksum shows data corruption,
     * ip_hdr_sum_dstadr returns 0 which is a reserved address has no mnode
     * associated with it.
     */
    cptr = mnode_prefix_lookup(ip_hdr_sum_dstadr(ip));
    if (!cptr) {
	if (IPMULTICAST(ip->dstadr)) {
	    paktype *pak = fs_pak;

	    stuff_fs_pak(bufptr, input, pak);
	    if (reg_invoke_ipmulticast_les_fs(pak))
		return(IPFLOWOK);
	}
        return(IPFLOWERR_NOCACHE);
    }

    iplen = ip->tl;
    idb = cptr->idb;
    output = cptr->hwidb;
    if (iplen > idb->ip_mtu)
	return(IPFLOWERR_MTU);

    /*
     * Do access checks and ip accounting if configured to do so.
     * encaps, output_qcount and dec_tql_and_inval are duplicated for
     * fast path performance (save few instructions)
     */
    if (output->ip_fast_flags) {
	crypto_tag_t crypto_tag = {0, 0};

	if (!flow_access_check(ip, input, idb, iplen, &crypto_tag))
	    return(IPFLOWERR_ACCESS);
	
	/*
	 * Now decrement IP TTL and adjust checksum.
	 */
	ip_ttl_adjust(ip);

	/*
	 * Re-encapulate IP packet, setting txstart address and output
	 * length in bufptr fields.
	 */
	if (crypto_tag.cryptoflags && 
            crypto_rsp_host_encrypt(input, output, cptr)) { 
	    (*cptr->crypto_encap)(bufptr, ip, cptr, iplen, &crypto_tag);
	    update_rx_counters(input, iplen + input->encsize, 1);
	    return (IPFLOWOK);
	}

	(*cptr->c.encaps)(bufptr, ip, cptr, iplen);

	/*
	 * Count the packet as received.
	 */
	update_rx_counters(input, iplen + input->encsize, 1);

	/*
	 * If anything is pending on the holdq, send it out now.
	 * Read and decrement tql, while invalidating the R4600 cache
	 * line that contains the IP header in shared memory.
	 * After this invalidation, do *NOT* reference IP header again!!!
	 */
	if (output->output_qcount)
	    (*output->soutput)(output);
	/*
	 * data beyond IP header is accessed, need to invalidate 2 cache lines
	 */
	tql = dec_tql_and_inval((uchar *)ip, output->tx_acc_ptr, 1);
    } else {
	/*
	 * Now decrement IP TTL and adjust checksum.
	 */
	ip_ttl_adjust(ip);

        (*cptr->c.encaps)(bufptr, ip, cptr, iplen);

	/*
	 * Count the packet as received.
	 */
	update_rx_counters(input, iplen + input->encsize, 1);

	/*
	 * If anything is pending on the holdq, send it out now.
	 * Read and decrement tql, while invalidating the R4600 cache
	 * line that contains the IP header in shared memory.
	 * After this invalidation, do *NOT* reference IP header again!!!
	 */
	if (output->output_qcount)
	    (*output->soutput)(output);
	/*
	 * only IP header is accessed, invalidate IP header cache line only
	 */
	tql = dec_tql_and_inval((uchar *)ip, output->tx_acc_ptr, 0);
    }

    if (tql <= RSP_RES_PROC_TX) {
	if (rsp_holdq_enqueue(bufptr, output, ip, tql))
	    update_tx_counters(output, bufptr->bf_tx_bytes, 1);
	return(IPFLOWOK);
    }

    /*
     * Count beans. LATER: Try to figure out an clean and easy way
     * to include outbound fddi FC byte and tokenring ACFC bytes.
     * update_global_ip_counts_cache keeps count in a global variable
     * which will be updated to ip_traffic by rsp_process_rawq to improve
     * performance
     */
    update_global_ip_counts_cache(1);
    update_tx_counters(output, bufptr->bf_tx_bytes, 1);
    hwq_enqueue_inline(output->tx_q_ptr, bufptr);
    return(IPFLOWOK);
}

static inline ipflow_t *
ipflow_get (iphdrtype *ip, hwidbtype *input)
{
    uchar flags;
    ipflow_t *flow;
    ipflow_key_t flkey;
    ipcache_rntype *cptr, *ncptr;
    crypto_tag_t    crypto_tag;

    flags = ipflow_key_get(ip, &flkey, input);
    flow = ipflow_hash_lookup(&flkey);
    if (flow) {
	/*
	 * crypto_tag are not used here, no need to initialize them
	 * and could be removed later
	 */
	cptr = flow->cptr;
	if ((cptr->flags & IPCACHE_FLAG_VALID) == 0) {
            crypto_tag.cryptoflags = 0;
            crypto_tag.crypto_conn_id = 0;
	    ncptr = mnode_prefix_lookup(flkey.fl_dst);
	    if (!ncptr)
	      return(NULL);
	    if (ncptr->hwidb->ip_fast_flags)
	      if (!flow_access_check(ip, input, ncptr->idb, 0, &crypto_tag))
		return(NULL);
	    flow->cptr = ncptr;
            flow->crypto_tag = crypto_tag;
	    ipcache_adjust_refcount(cptr, -1);
	    ipcache_adjust_refcount(ncptr, 1);
	} else if ((flags & TCP_FLAGS_EST) == 0) {
	    if (cptr->hwidb->ip_fast_flags)
	      if (!flow_access_check(ip, input, cptr->idb, 0, &crypto_tag))
		return(NULL);
	}
    } else {
	cptr = mnode_prefix_lookup(flkey.fl_dst);
	if (!cptr)
	  return(NULL);
	crypto_tag.cryptoflags = 0;
	crypto_tag.crypto_conn_id = 0;
	if (cptr->hwidb->ip_fast_flags)
	  if (!flow_access_check(ip, input, cptr->idb, 0, &crypto_tag))
	    return(NULL);
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
 * RSP IP FLOW Switching Routine
 */
int rsp_ipflowswitch (bufhdr_t *bufptr, iphdrtype *ip, hwidbtype *input)
{
    ipcache_rntype *cptr;
    hwidbtype *output;
    ipflow_t *flow;
    idbtype *idb;
    ulong iplen;
    ushort tql;

    /*
     * ip_hdr_sum is same as ip_hdr_sum_dstadr except it returns checksum 0
     * when good returns other value when bad
     */
    if (ip_hdr_sum(ip))
	return(IPFLOWERR_CKS);

    if (!(flow = ipflow_get(ip, input))) {
	if (IPMULTICAST(ip->dstadr)) {
	    paktype *pak = fs_pak;

	    stuff_fs_pak(bufptr, input, pak);
	    if (reg_invoke_ipmulticast_les_fs(pak))
		return(IPFLOWOK);
	}
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
    if (iplen > idb->ip_mtu) 
	return(IPFLOWERR_MTU);
    ipflow_log_size_distribution(iplen);

    /*
     * Now decrement IP TTL and adjust checksum.
     */
    ip_ttl_adjust(ip);

    /*
     * Re-encapulate IP packet, setting txstart address and output
     * length in bufptr fields.  Increment flow counters.
     */
    if (flow->crypto_tag.cryptoflags && 
        crypto_rsp_host_encrypt(input, output, cptr)) { 
        (*cptr->crypto_encap)(bufptr, ip, cptr, iplen, &flow->crypto_tag);
	update_rx_counters(input, iplen + input->encsize, 1);
        return (IPFLOWOK);
    } else {
        (*cptr->c.encaps)(bufptr, ip, cptr, iplen);
    }

    flow->pkts++;
    flow->bytes += iplen;

    /*
     * Count the packet as received.
     */
    update_rx_counters(input, iplen + input->encsize, 1);

    /*
     * If anything is pending on the holdq, send it out now.
     * Then read and decrement tql, while invalidating the R4600 cache
     * line that contains the IP header in shared memory.
     * After this invalidation, do *NOT* reference IP header again!!!
     */
    if (output->output_qcount)
      (*output->soutput)(output);
    /*
     * data beyond IP header is accessed, need to invalidate 2 cache lines
     */
    tql = dec_tql_and_inval((uchar *)ip, output->tx_acc_ptr, 1);
    if (tql <= RSP_RES_PROC_TX) {
	if (!rsp_holdq_enqueue(bufptr, output, ip,  tql)) {
	    flow->drops++;
	    flow->pkts--;
	    flow->bytes -= iplen;
	} else
	    update_tx_counters(output, bufptr->bf_tx_bytes, 1);
	return(IPFLOWOK);
    }

    /*
     * Increment interface and traffic counters.
     * Put buffer on hardware transmit queue.
     * update_global_ip_counts_cache keeps count in a global variable
     * which will be updated to ip_traffic by rsp_process_rawq to improve
     * performance
     */
    update_global_ip_counts_cache(1);
    update_tx_counters(output, bufptr->bf_tx_bytes, 1);
    hwq_enqueue_inline(output->tx_q_ptr, bufptr);
    return(IPFLOWOK);
}

/*
 * Transmit Queue Limit accumulator too low.
 * Here with data cache invalidated throughout
 * (that's why we're passing iplen on stack).
 */
boolean rsp_holdq_enqueue (bufhdr_t *bufptr, hwidbtype *output,
				  void *ip, ushort tql)
{
    paktype *pak;
    uchar *memdstart;
    uchar *ip_header = ip;
    int offset;
    ulong bytes = bufptr->bf_tx_bytes;

    if (tql)
      hwacc_inc_inline(output->tx_acc_ptr);
    
    if ((!output->pak_backing_store && !output->priority_list) ||
	(!(pak = rsp_oqueue_getbuffer(bytes)))) {
	output->counters.output_total_drops++;
	retbufhdr_inline(bufptr);
	return(FALSE);
    }
    
    /*
     * Copy packet from MEMD to dram, invalidate the MEMD portion's
     * cache lines, and return the MEMD buffer to its free queue.
     */
    offset = ADDR_TO_MEMD_OFFSET_24(ip_header) - bufptr->bf_tx_ptr;
    pak->datagramstart -= offset;
    pak->datagramsize = bytes;
    memdstart = MEMD_OFFSET_TO_ADDR(bufptr->bf_tx_ptr);
    bcopy(memdstart, pak->datagramstart, bytes);
    invalidate_data_inline (memdstart, bytes);
    retbufhdr_inline(bufptr);
    
    pak->if_output = output->firstsw;
    pak->rxtype = RXTYPE_DODIP;
    pak->linktype = LINK_IP;
    
    /*
     * Put the new packet on the TX hold queue now:
     *
     * queuing routine counts drop and
     * frees input buffer on enqueue failure.
     */
    if (!output->oqueue(output, pak, TAIL))
      return(FALSE);
    if (1 == output->output_qcount)
        output->soutput(output);
    output->pak_swapped_out++;
    return(TRUE);
}

/*
 * Rewrite any encapsulation whose complete rewrite string is
 * completely included in cptr->m.macstring.
 * This should work for HDLC, FR, PPP, Channel, Ethernet, and ATM.
 */
static int ipflow_encaps_generic (void *bp, iphdrtype *ip, 
				  ipcache_rntype *cptr, ulong iplen)
{
    hwidbtype *hwidb = cptr->hwidb;
    ulong encap_len = cptr->length;
    
    rewrite_generic(cptr, ip, encap_len);
    setup_tx_bufptr(bp, hwidb, (ulong)ip - encap_len, iplen + encap_len);
    return(IPFLOWOK);
}

/*
 * Rewrite Frame Relay mac encapsulation from radix cache struct.
 */
static int ipflow_encaps_frame_relay (void *bp, iphdrtype *ip, 
				      ipcache_rntype *cptr, ulong iplen)
{
    INC_PVC_FAST_OUTPUT(HWIDB_TO_FR_PVC(cptr->hwidb,
					cptr->subint_reference_num),
			    iplen + cptr->length);
    
    return(ipflow_encaps_generic(bp, ip, cptr, iplen));
}

/*
 * Write out variable length tokenring encapsulation.
 */
static int ipflow_encaps_tokenring (void *bp, iphdrtype *ip,
				    ipcache_rntype *cptr, ulong iplen)
{
    uchar *txptr;
    ulong encap_len = cptr->length;

    rewrite_generic(cptr, ip, encap_len);
    encap_len += IPCACHE_HARDCODED_ACFC_BYTES;
    txptr = (uchar *)ip - encap_len;
    *(ushort *)txptr = (AC_PRI0 << 8) | FC_TYPE_LLC;
    setup_tx_bufptr(bp, cptr->hwidb, (ulong)txptr, iplen + encap_len);
    
    return(IPFLOWOK);
}

/*
 * Allows variable length mac encaps to account for RIF.
 */
static int ipflow_encaps_fddi_with_rif (void *bp, iphdrtype *ip,
					ipcache_rntype *cptr, ulong iplen)
{
    uchar *txptr;
    ulong encap_len = cptr->length;

    rewrite_generic(cptr, ip, encap_len);
    encap_len += IPCACHE_HARDCODED_FC_BYTES;
    txptr = (uchar *)ip - encap_len;
    *txptr = FDDI_LLC_FC_BYTE;
    setup_tx_bufptr(bp, cptr->hwidb, (ulong)txptr, iplen + encap_len);

    return(IPFLOWOK);
}

/*
 * Allows variable length mac encaps to account for RIF.
 */
static int ipflow_encaps_fddi (void *bp, iphdrtype *ip,
			       ipcache_rntype *cptr, ulong iplen)
{
    rewrite_fddi(cptr, ip);
    setup_tx_bufptr(bp, cptr->hwidb, (ulong)ip - FDDI_SNAP_ENCAPBYTES,
		    iplen + FDDI_SNAP_ENCAPBYTES);

    return(IPFLOWOK);
}


/*
 * Rewrite SMDS mac encapsulation from 8-byte aligned iphash struct,
 * then set MEMD buffer header transmit fields appropriately.
 * This routine differs from the more generic ip_fastswitch in that
 *   we modify the rewrite header in the cache before copying to
 *   packet memory.  This keeps us from reading packet memory and
 *   requiring a resultant cache invalidation.
 */
static int ipflow_encaps_smds (void *bp, iphdrtype *ip, 
			       ipcache_rntype *cptr, ulong iplen)  
{
    hwidbtype *output;
    smdstype *sm;
    uint modelen, encaps_plus_ip_len, len, pad, txstart;
    
    output = cptr->hwidb;
    modelen = output->smds_pdb->fast_output_modelen;
    sm = (smdstype *)(cptr->m.macstring + modelen);
    encaps_plus_ip_len = iplen + cptr->length;
    /*
     * Calculate the pad size. remember to subtract the psdeo hdlc
     * bytes off first (sizeof 2).
     */
    pad = 3 - ((iplen + 3) % 4);
    sm->smds_xhlpi = (sm->smds_xhlpi & ~3) | pad;
    sm->smds_betag = output->smds_begin_tag++;
    sm->smds_basize = encaps_plus_ip_len - (4 + modelen) + pad;
    
    txstart = (ulong)ip - cptr->length;
    bcopy(cptr->m.maclong, (uchar *)txstart, cptr->length);
    /*
     * SMDS pad
     */
    PUTLONG((ulong *) (txstart + encaps_plus_ip_len), 0);
    
    /*
     * SMDS trailer
     */
    len = encaps_plus_ip_len + pad;
    PUTLONG((ulong *) (txstart+len), GETLONG((ulong *)&sm->smds_rsvd));
    
    setup_tx_bufptr(bp, output, txstart, len + 4);
    return(IPFLOWOK);
}

  /*
  * Ethernet ISL encapsulation.
  */
static int ipflow_encaps_ether_isl (void *bp, iphdrtype *ip, 
 				    ipcache_rntype *cptr, ulong iplen)
{
    idbtype *swidb = cptr->idb;
    ulong encap_len = cptr->length;
    ushort txlen; 
   
    rewrite_generic(cptr, ip, encap_len);
    swidb->isl_vlan->egressing_isl_packets[VLAN_ROUTE_IP]++;
    txlen = iplen + encap_len;
    setup_tx_bufptr_isl(bp, cptr->hwidb, (ulong)ip - encap_len, txlen);
    return(IPFLOWOK);
} 

/*
 * Assign output encapsulation vector to route cache struct.
 * Later: move this to registry call from ipcache_add().
 */
static ipencaps_t ipcache_encaps (ipcache_rntype *cptr)
{
    hwidbtype *output = cptr->hwidb;
    
    if (output->ip_fast_flags & IP_FAST_RSP_DDR)
      return(NULL);

    switch (output->fast_switch_type) {
      case FS_FDDI:
	if (cptr->length > FDDI_SNAP_ENCAPBYTES)
	  return(ipflow_encaps_fddi_with_rif);
	else
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
	  return(ipflow_encaps_generic);
	/*
	 * For the following encapsulations, we only need to prefix
	 * cptr->length bytes of cptr->macstring to the IP header.
	 */
      case FS_AIP:
      case FS_CHANNEL:
      case FS_HDLC:
      case FS_PPP:
      case FS_SLIP:
	return(ipflow_encaps_generic);
	
	/*
	 * Let the lowend f/s routine have a shot at it...
	 */
      default:
	break;
    }
    return(NULL);
}

void rsp_optimum_prefix_add (ipcache_rntype *cptr)
{
    cptr->c.encaps = ipcache_encaps(cptr);
    if (cptr->c.encaps)
      mnode_prefix_add(cptr);
}

void rsp_optimum_prefix_delete (ipcache_rntype *cptr)
{
    /*
     * Remove reference from mtrie.
     */
    if (cptr->c.encaps)
      mnode_prefix_delete(cptr);

    /*
     * Signal to flow switching code that this entry is now invalid.
     * We can use this rather than flags to avoid thrashing d-cache.
     */
    cptr->c.encaps = NULL;

    /*
     * Send invalidate to distributed cache as well.
     */
    ipc_ipfast_rsp_send_invalid(cptr);
}


/*
 * ip_rsp_fs_register_drivers
 */

void ip_rsp_fs_register_drivers (ulong flags, hwidbtype* idb)
{
    /*
     * Check to see if we're configuring IP
     */
    if (!(flags & FSF_DODIP))
	return;

    /*
     * Enable IP flow switching or IP RSP switching if enabled
     */
    if (idb->fast_switch_flag & FSF_DODIP) {
	/*
	 * No RSP fastswitching to or from DDR interfaces.
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
	    if (ipflow_table_init()) {
                if (IS_ENCRYPTION(idb)) {
                    reg_invoke_crypto_rsp_register_ip_turbo_fs(idb);
                } else {
	            idb->ip_turbo_fs = (ip_turbo_fs_t) rsp_ipflowswitch;
                }
	    } else {
		idb->ip_turbo_fs = (ip_turbo_fs_t) turbo_disabled;
	    }
	} else {
	    if (idb->ip_routecache & IP_OPTIMUM_ROUTECACHE) {
                if (IS_ENCRYPTION(idb)) {
                    reg_invoke_crypto_rsp_register_ip_turbo_fs(idb);
                } else {
		    idb->ip_turbo_fs = (ip_turbo_fs_t) rsp_ipfastswitch;
                }
	    } else {
		idb->ip_turbo_fs = (ip_turbo_fs_t) turbo_disabled;
	    }
	    ipflow_check_free_resources();
	}
    }
}

boolean rsp_ip_turbo_cache_idb_capable (hwidbtype *hwidb)
{
    if (hwidb && !is_ddr(hwidb)) {

	/*
	 * Do we support turbo switching on this interface,
	 * ie - 'ip route-cache flow' and 'ip route-cache rsp'?
	 */
	switch(hwidb->type) {
	case IDBTYPE_CBUSETHER:
	case IDBTYPE_FDDIT:
	case IDBTYPE_CTR:
	case IDBTYPE_AIP:
	case IDBTYPE_CBUSSERIAL:
	case IDBTYPE_HSSI:
	case IDBTYPE_T1:
	case IDBTYPE_E1:
	case IDBTYPE_FEIP:
	    return(TRUE);
	default:
	    return(FALSE);
	}
    }
    return(FALSE);
}

/*
 * Default optimum switching 'on' for all supported interfaces
 */

void default_optimum_fast_on (hwidbtype *hwidb, boolean ip_on)
{
    if (ip_on && rsp_ip_turbo_cache_idb_capable(hwidb)) {
	hwidb->ip_routecache |= IP_OPTIMUM_ROUTECACHE;
    }
}

void rsp_ip_periodic (void)
{
    ip_ipc_flush_msg();
    ipflow_periodic();
}

void ip_rsp_fs_init (subsystype* subsys)
{
    reg_add_ip_platform_default(default_optimum_fast_on,
				"default_optimum_fast_on");

    reg_add_ip_turbo_idb_capable(rsp_ip_turbo_cache_idb_capable,
				"rsp_ip_turbo_cache_idb_capable");

    reg_add_ip_flow_command_parse(ip_flow_cache_commands,
				  "ip_flow_cache_commands");

    reg_add_ip_cache_flow_show(ipflow_show, "ipflow_show");

    reg_add_auto_sw_ip_cache_show(optimum_ipcache_show,
				  "optimum_ipcache_show");

    reg_add_flush_ip_stats((service_flush_ip_stats_type)
			   rsp_ip_periodic,
			   "rsp_ip_periodic");

    reg_add_register_drivers(ip_rsp_fs_register_drivers,
			     "ip_rsp_fs_register_drivers");

    reg_add_auto_sw_ip_cache_update(ipc_ipfast_rsp_send_add, 
				    "ipc_ipfast_rsp_send_add");

    reg_add_ipcache_add_callback(rsp_optimum_prefix_add,
 				 "rsp_mnode_prefix_add");
    
    reg_add_ipcache_delete_callback(rsp_optimum_prefix_delete,
				    "rsp_optimum_prefix_delete");

    reg_add_ip_enable_disable_dfs_ipc(ipc_send_enable_ds, 
				      "ipc_send_enable_ds");

    reg_add_ip_send_acl_change_ipc(ipc_ipfast_rsp_access_change,
				   "ipc_ipfast_rsp_access_change");

    reg_add_ip_access_group(ipc_access_group_callback,
			    "ipc_access_group_callback");

    /*
     * note this registry has nothing to do with flow/optimum switching.
     * however it is an rsp specific ip fastswitching feature.  For now
     * this is probably the best place to init this.
     */
    reg_add_udp_flood_fs(udp_flood_fs, "udp_flood_fs");
 
    ipc_ipfast_rsp_init();
}


/*
 * IP RSP FS subsystem header
 */

#define IPRSPFS_MAJVERSION 1
#define IPRSPFS_MINVERSION 0
#define IPRSPFS_EDITVERSION  1

SUBSYS_HEADER(ip_rsp_fs, IPRSPFS_MAJVERSION, IPRSPFS_MINVERSION, IPRSPFS_EDITVERSION,
	      ip_rsp_fs_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      "req: fastswitch, iphost");
