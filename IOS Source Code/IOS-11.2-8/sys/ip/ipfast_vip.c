/* $Id: ipfast_vip.c,v 3.8.6.17 1996/08/16 08:02:10 dlobete Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast_vip.c,v $
 *------------------------------------------------------------------
 * VIP resident IP Fastswitching routines
 *
 * Aug 1995, Darren Kerr and Don Lobete
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipfast_vip.c,v $
 * Revision 3.8.6.17  1996/08/16  08:02:10  dlobete
 * CSCdi59971:  DACL needs modularity work
 * Branch: California_branch
 * For the sake of future maintenance of IP access-lists, share as much
 * source code as possible between VIP and RSP.
 *
 * Revision 3.8.6.16  1996/08/12  06:12:23  dlobete
 * CSCdi59677:  distributed switching broken if MIP queuing fixed
 * Branch: California_branch
 * Eliminate mci_index from DFS scope and replace with the more
 * suitable hw_if_index. This not only fixes DFS to MIP channel-groups
 * but also paves the way for input from the new channelized port-adaptors
 * into DFS.
 *
 * Revision 3.8.6.15  1996/06/06  12:03:26  dlobete
 * CSCdi57944:  DFS confused about rx ignores and tx drops
 * Branch: California_branch
 * Also fix up an error in NACL processing at RSP.
 *
 * Revision 3.8.6.14  1996/06/06  09:39:54  dlobete
 * CSCdi54871:  faulty local switching accounting
 * Branch: California_branch
 * Turn off local-switching if we're overwhelming the local-output
 * interface. This way we can take advantage of MEMD's queueing
 * capability and basically equalize packet queueing from multiple
 * sources. This will also will fix the accounting hole.
 *
 * Revision 3.8.6.13  1996/06/06  09:24:52  dlobete
 * CSCdi57425:  VIP2 max cache entries wt. 32MB & 64MB are the same
 * Branch: California_branch
 *
 * Revision 3.8.6.12  1996/06/06  08:58:47  dlobete
 * CSCdi55797:  DACL ignores access-lists when switching to subinterfaces
 * Branch: California_branch
 * To have DFS handle IP output access-lists to multiple subinterfaces
 * (eg on a MIP) and to save the baggage of swidbs down at VIP,
 * the output-access-list is stashed in the DFS cache prefix entry.
 * Also don't enable DFS if multiple input ACLs exist on a VIP interface.
 *
 * Revision 3.8.6.11  1996/06/05  05:08:59  kao
 * CSCdi53846:  EOIR of EIP wipes route cache of entries for that interface
 * No cache invalidation if reset during EOIR
 * Populate txq and txacc down to all IPs after memd recarve for those
 * DFS caches pending on new txq and txacc
 * Branch: California_branch
 *
 * Revision 3.8.6.10  1996/05/28  16:13:50  xliu
 * CSCdi58788:  code changes for vip named access list and encryption perf
 * Branch: California_branch
 *
 * Revision 3.8.6.9  1996/05/21  09:51:26  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8.6.8  1996/05/12  23:09:39  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.8.6.7  1996/05/08  01:09:51  gchristy
 * CSCdi56944:  Compilation errors from IP named access list commit
 * Branch: California_branch
 * - Modularize new ACL code for inclusion in VIP images.
 *
 * Revision 3.8.6.6  1996/04/19  15:42:17  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.8.6.5  1996/04/12  23:05:08  kao
 * CSCdi53527:  Need EOIR support on VIP2 (remove lines brought in during
 * "cvs update")
 * Branch: California_branch
 *
 * Revision 3.8.6.4  1996/04/12  17:42:40  kao
 * CSCdi53527:  Need EOIR support on VIP2
 * Branch: California_branch
 *
 * Revision 3.8.6.3  1996/04/10  03:38:29  dlobete
 * CSCdi53121:  Distributed IP Access Lists need to run at interrupt-level
 * Branch: California_branch
 *
 * Revision 3.8.6.2  1996/04/08  13:44:44  getchell
 * CSCdi52361:  DFS broken using unlike media
 * Branch: California_branch
 *
 * Revision 3.8.6.1  1996/03/21  22:50:23  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.4  1996/03/18  23:26:47  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.3  1996/03/07  17:28:23  getchell
 * CSCdi50953:  viper dfs mods
 * Branch: ELC_branch
 *
 * Revision 3.2.2.2  1996/02/08  07:48:14  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.1  1996/01/15  23:00:21  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.8  1996/03/04  07:12:40  kao
 * CSCdi49674:  While VIP is cofigured with DFS, OIR event will crash the
 * VIP;
 *
 * Revision 3.7  1996/02/24  02:14:37  dkerr
 * CSCdi49787:  IPC rolls over and dies under load
 * Make IPC more robust in an RSP->VIP distributed switching environment
 * (and increase performance from the miserly 100 msgs per minute to
 * something respectable, like 300K msgs per second).
 *
 * Revision 3.6  1996/01/24  00:27:37  dlobete
 * CSCdi47213:  IP DFS statistics non-functional under traffic load
 * Don't rely on processes running on VIP - send up DFS statistics
 * when interface statistics are passed up (i.e. at interrupt level).
 *
 * Revision 3.5  1996/01/19  03:02:56  dlobete
 * CSCdi44954:  Local-switching for DFS required
 * But that's not all! Also part of this jumbo Distributed IP
 * Switching commit:
 * - CSCdi43302: Illegal Access to Low Address crashes RVIP/4E (RSP2)
 * - DFS config which mirrors RSP route-cache setting
 *         (ie configurable distributed flow or optimal switching)
 * - Interrupt level IPC updates and invalidates for DFS cache operations
 * - Stay in closer synch with RSP cache for prefix invalidations
 *
 * Revision 3.4  1996/01/18  15:48:27  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.3  1995/12/17  18:29:47  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:35:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:57:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:04:59  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/26  05:50:06  dlobete
 * Placeholders for VIP_branch development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
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
#include "access.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"

#include "../if/ether.h"
#include "../if/fddi.h"
#include "../if/tring.h"
#include "../if/network.h"
#include "../if/atm.h"

#include "../wan/serial.h"
#include "../wan/frame_relay.h"
#include "../wan/atm_dxi.h"
#include "../wan/smds.h"

#include "../src-vip/dfs_vip_idb.h"

#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_tcpip.h"
#include "../parser/parser_defs_access.h"
#include "ip_registry.h"

#include "../ip/ip.h"
#include "../ip/ipaccount.h"
#include "../ip/ipfast_index.h"
#include "../ipmulticast/igmp.h"
#include "../ip/ipfast_vip.h"
#include "../ip/ipfast_vip_ipc.h"
#include "../ip/ipaccess_vip.h"
#include "../h/access.h"
#include "../ip/ipaccess.h"
#include "../ip/ipaccess_private.h"
#include "../crypto/crypto_public.h"
#include "../ip/ipaccess_inline.h"

#include "../pas/if_pas.h"

#include "../src-vip/vip_memd.h"
#include "../src-vip/rsp_memd.h"

#include "../dev/monitor1.h"


/*
 * Perform access checks as per config.
 */
boolean vip_ip_access_check (paktype *pak, ulong flags,
			     acl_headertype *acl_in, acl_headertype *acl_out)
{
    if (flags & IP_FAST_ACCESSLISTS)
        if (!ip_accesscheck_inline(pak, acl_out, NULL, TRUE))
            return(FALSE);
 
    if (flags & IP_FAST_INPUTACCESSLISTS)
        if (!ip_accesscheck_inline(pak, acl_in, NULL, TRUE))
            return(FALSE);
    
    return(TRUE);
}

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
inline ushort ip_cks_and_ttl_adjust_unaligned (iphdrtype *ip)
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
 * VIP Fastswitching using IP prefix lookup.
 *
 * Look-up cache entry.
 * Check MTU.
 * Check outbound tql.
 * Check access lists.
 * Hand over to outbound encapsulation vector.
 */
boolean vip_ip_prefix_fs (hwidbtype *input, paktype *pak)
{
    iphdrtype *ip = (iphdrtype *)ipheadstart(pak);
    charlong destaddr;
    ipcachetype *cptr;
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
     * Perform radix lookup of destination IP address.
     */
    destaddr.d.lword = GETLONG(&ip->dstadr);

    /*
     * Perform prefix lookup to see if the destination network is cached.
     */
    cptr = ipfast_prefix_lookup(destaddr.d.lword);
    if (!cptr) {
	ipfast_prefix_misses++;
	/*
         * Just punt this packet up to the host
	 */
	retval = (*pascb->pak_to_host)(input, pascb, pak);
	if (!retval)
	    input->counters.input_drops++;
	return(retval);
    }
    ipfast_prefix_switched++;

    /*
     * Get the encapsulation rewrite info struct from prefix entry.
     */
    eptr = cptr->eptr;
    output = eptr->idb;
    
    /*
     * We stashed the mtu into the cache struct to avoid baggage of
     * swidb in vip.
     */
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
     * Access list checks.
     */
    if (output->ip_fast_flags)
      if (!vip_ip_access_check(pak, output->ip_fast_flags,
			       input->fast_rcvidb->ip_input_accesslist,
			       output->ip_output_accesslist)) {
	  /*
	   * Let host handle ICMP reply and accounting of violation.
	   */
	  retval = (*pascb->pak_to_host)(input, pascb, pak);
	  if (!retval)
	      input->counters.input_drops++;
	  return(retval);
      }


    if (!output->local_sw_vector) {
	/*
	 * We need to read the memd tx accumulator and buffer
	 * header before we load up the reno's with writes.  It's
         * not pretty, but necessary.
	 */

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
	ipfast_prefix_local_switched++;
    }

    /*
     * Re-encapsulate the packet and send to transmit routine.
     */
    return((*eptr->evector)(pak, eptr, input, input_encap_len, iplen, bhptr));
}

/*
 * Rewrite simple mac encapsulations for switching to interface
 * on another CyBus interface controller.
 */
boolean vip_ip_encaps_generic (paktype *pak, encapinfotype *eptr,
			       hwidbtype *input, int input_encap_len,
			       ulong iplen, void *bhptr)
{
    dfsidbtype *output = eptr->idb;
    ulong macsize = eptr->length;
    ulong txsize = iplen + macsize;
    ulong rxsize = pak->datagramsize;
    boolean status = FALSE;
    particletype *mp;

    /*
     * Rewrite encapsulation in place.
     * NB - this assumes that there is enough room in any inbound
     * packet for any outbound encaps supported by rewrite_generic.
     * If the outbound encaps is bigger than this, then we need a
     * different encaps routine to allocate a separate rewrite mb.
     * Also, this copies in 8-byte chunks, so make sure that there
     * is appropriate slop.
     */
    encap_rewrite_generic(eptr, pak->network_start, macsize);

    mp = pak->particlequeue.qhead;
    particle_clip_inline(pak->particlequeue.qhead, macsize - input_encap_len);

    pak->datagramsize = txsize;
    pak->datagramstart = pak->network_start - macsize;
    
    /*
     * If we're able to enqueue the packet onto the appropriate
     * transmit queue, count beans and return success.
     * If the outbound interface is on a different cybus controller,
     * this involves checking hw accumulator and allocating a
     * memd buffer.
     * LATER: _ vectorize this when local switching is supported.
     *        _ remove 'input' parameter when we change memd buffer
     *          allocation to use global best-fit buffers.
     * Vectorization gets trickier with the use of dfsidbtype
     * because the fastsend routines of the pa drivers are expecting
     * real hwidbtypes as parameters.
     */
    if (output->local_sw_vector) /* local switch posssible */ {
	output->local_sw_vector(output->hwidb, pak);
	status = TRUE;
    } else if (vip_rsp_memd_fastsend(output->tx_q_ptr, pak, input, (vip_bhdr_t *)bhptr)) {
	status = TRUE;
    }

    if (status == TRUE) {
	/*
	 * Count beans
	 */
	update_rx_ip_cntrs(input, rxsize, 1);
	update_tx_ip_cntrs(output, txsize, 1);
    } else {
	input->counters.input_drops++;
    }
    return(status);
}

/*
 * Temporarily unsupported encapsulations should be shuttled
 * to the RSP for switching.
 *
 * This is broken in that we've already decremented the TTL
 * and adjusted the checksum.  We need to convey this info
 * to the RSP so that it doesn't decrement it again.
 */
boolean vip_ip_encaps_unsupported (paktype *pak, encapinfotype *eptr,
				   hwidbtype *input, int input_encap_len,
				   ulong iplen, void *bhptr)
{
    return(FALSE);
}

void vip_ip_dfs_onesec (void)
{
}

/*
 * VIP IP fastswitching initialization.
 */
void vip_ip_fs_init (subsystype* subsys)
{
    ulong mem_size;

    /*
     * Only meaningful for VIPs in c75xx.
     */
    if (vsys_vec.rpsp_host)
	return;

    /*
     * Ask the ROM monitor how much DRAM we have on board.
     */
    mem_size = mon_getmemsize();

    /*
     * Service to poll hwidbs and flow entries for IP switching
     * stats to be IPC'd to the RSP.
     */
    reg_add_onesec(vip_ip_dfs_onesec, "vip_ip_dfs_onesec");
    reg_add_access_named_show(show_ip1access, "show_ip1access");
    reg_add_access_named_show(show_ip2access, "show_ip2access");
    reg_add_access_named_show(show_acl_intface_settings, 
			      "show_acl_intface_settings");

    ipfast_parser_init();

    /*
     * Ready IPC to call our init routine.
     */
    ipc_ipfast_vip_init();

    /*
     * Initialize the lists required by DFS.
     */
    ipfast_vip_init_lists(mem_size);

    /*
     * Initialize anything required for Distributed Flow Switching.
     */
    ipfast_vip_flow_init(mem_size);

    /*
     * Initialize anything required for Distributed Access Lists. 
     */
    ipaccess_vip_init(mem_size);
}

/*
 * VIP IP fastswitching subsystem header
 */
#define VIP_IP_FS_MAJVERSION 1
#define VIP_IP_FS_MINVERSION 0
#define VIP_IP_FS_EDITVERSION 1

/*
 * Shouldn't this be a driver, since we're really not running
 * the routing protocols?
 */

SUBSYS_HEADER(vip_ip_fs, VIP_IP_FS_MAJVERSION, VIP_IP_FS_MINVERSION,
	      VIP_IP_FS_EDITVERSION, vip_ip_fs_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      NULL);

