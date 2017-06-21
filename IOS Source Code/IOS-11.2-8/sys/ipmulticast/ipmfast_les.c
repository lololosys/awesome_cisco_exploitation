/* $Id: ipmfast_les.c,v 3.9.12.21 1996/08/27 18:38:58 abaindur Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/ipmfast_les.c,v $
 *------------------------------------------------------------------
 * ipmfast_les.c - IP multicast fastswitching for mid/low-end platforms.
 *
 * December, 1994.
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ipmfast_les.c,v $
 * Revision 3.9.12.21  1996/08/27  18:38:58  abaindur
 * CSCdi67226:  IP multicast fastswitching broken (regress)
 * Branch: California_branch
 *  Fix setting of pak->network_start and pak->flags after pak duplication.
 *  Also remove some duplicate pak field settings.
 *
 * Revision 3.9.12.20  1996/08/24  01:11:56  fred
 * CSCdi67059:  IP Multicast doesnt set up access list pointers
 * Branch: California_branch
 *              Set up certain pak->fields
 *
 * Revision 3.9.12.19  1996/08/23  18:04:23  abaindur
 * CSCdi66732:  Alignment error in IP multicast fastswitching
 * Branch: California_branch
 *
 * Revision 3.9.12.18  1996/08/13  23:22:00  rtio
 * CSCdi59352:  Show-frame-pvc does not accurately display the packets or
 * bytes
 * Branch: California_branch
 * Added code to put dlci/dfa in swidb->p2pmac for p2p subifs so ip mcast
 * cache can pass this number to ip_pak_write_mac_inline().
 *
 * Revision 3.9.12.17  1996/07/30  01:08:28  mbeesley
 * CSCdi55762:  Need to support fast IP multicasting on platforms
 * capable of scatter/gather IE c7200
 * Branch: California_branch
 *
 * Revision 3.9.12.16  1996/07/06  05:54:28  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.9.12.15  1996/06/28  00:08:06  mleelani
 * CSCdi61578:  Router with a local member, but with NULL OIF doesnt reply
 * to mping.
 * Branch: California_branch
 * Fast drop only if we dont have local members.
 *
 * Revision 3.9.12.14  1996/06/27  07:59:52  mleelani
 * CSCdi61079:  Occasional traffic loss because of bogus DVMRP prunes
 * Branch: California_branch
 * o Dont send prune on non-RPF p2p link, if it does not appear in the OIF.
 * o Prevent bogus prune sent to RPF neighbor on tunnel when the dvmrp peer
 *   is a non-pruner.
 *
 * Revision 3.9.12.13  1996/06/18  01:46:44  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.9.12.12  1996/06/12  03:56:08  mleelani
 * CSCdi58720:  Multicast fastswitching does not do fast drop.
 * Branch: California_branch
 * DO fast drop.
 *
 * Revision 3.9.12.11  1996/06/08  04:11:57  lwei
 * CSCdi59415:  mdb timer incorrectly shrank at data forwarding time
 * Branch: California_branch
 *
 * Revision 3.9.12.10  1996/05/28  17:39:50  fred
 * CSCdi58812:  ip multicast not correctly classified in WFQ on low end
 * Branch: California_branch
 *
 * Revision 3.9.12.9  1996/05/08  04:29:35  lwei
 * CSCdi54559:  IP multicast is sending larger than MTU sized packets over
 * GRE
 * Branch: California_branch
 *
 * Revision 3.9.12.8  1996/05/06  16:50:47  dino
 * CSCdi56717:  A POOLVALID() call is not required in
 *              ipmulticast_pak_duplicate()
 * Branch: California_branch
 *
 * Revision 3.9.12.7  1996/04/29  18:01:13  dino
 * CSCdi54902:  Dont replicate buffer, just bcopy ip->tl in
 *              ipmulticast_les_fast()
 * Branch: California_branch
 *
 * Revision 3.9.12.6  1996/04/27  05:52:34  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.7.10.3  1996/04/26  01:26:53  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.7.10.2  1996/04/17  03:02:49  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.7.10.1  1996/02/23  20:24:14  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.9.12.5  1996/04/19  15:19:32  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.7  1996/04/09  20:41:55  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.1.6.6  1996/02/21  03:31:42  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.1.6.5  1996/02/13  19:18:38  carrel
 * Branch: IosSec_branch
 * Crypto Map checkin (finally)
 *
 * Revision 2.1.6.4  1996/01/04  19:23:42  xliu
 * Branch: IosSec_branch
 * Replaced the direct call to crypto subsystem with the subsys registry
 * call.
 *
 * Revision 2.1.6.3  1995/12/19  03:57:34  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 2.1.6.2  1995/12/17  01:41:45  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.1  1995/11/17  17:03:18  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.9.12.4  1996/04/17  13:42:26  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.9.12.3  1996/04/09  00:10:16  mleelani
 * CSCdi53972:  Tunnel accounting incorrect for Mcast packets.
 * Branch: California_branch
 * Misc. tunnel accounting fixes.
 *
 * Revision 3.9.12.2  1996/04/04  04:55:12  dino
 * CSCdi53046:  DR doesnt register when on shared-tree and spt-threshold
 *              configured
 * Branch: California_branch
 *
 * Revision 3.9.12.1  1996/03/18  20:18:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.14.2  1996/03/13  01:38:39  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7.14.1  1996/02/20  01:01:25  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/02/13  09:36:22  dino
 * CSCdi48623:  Multicast mtraces are not processed by last hop router due
 *              to f/s
 *
 * Revision 3.8  1996/01/26  20:03:32  dino
 * CSCdi47621:  IP multicast fastswitching doesnt work on dialerless ISDN
 *              interface. Also, packets are not process level switched
 *              when PPP compression is enabled in incoming interface.
 *
 * Revision 3.7  1995/12/17  19:24:18  dino
 * CSCdi44742:  PIM should only switch to the SPT when traffic threshold
 *              is reached
 *
 * Revision 3.6  1995/12/15  09:19:14  ogrady
 * CSCdi39140:  Multicast fastswitching doesnt work on an RSP box.
 * Fix access alignment.
 *
 * Revision 3.5  1995/11/22  22:20:11  dino
 * CSCdi44336:  mtrace interface counters not properly counted when
 * partially fs
 *
 * Revision 3.4  1995/11/22  06:57:20  dino
 * CSCdi43574:  mtrace gets native interface instead of tunnel when native
 * not mcast
 *
 * Revision 3.3  1995/11/17  17:29:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:42:09  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:01:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/10/09  21:46:48  raj
 * CSCdi41279:  Multicast fastswitching doesnt work when ISDN is input
 * interface.
 * Use is_ddr() instead, so it works for the general DDR case.  Additional
 * changes needed for high-end fastswitch routines as well.
 *
 * Revision 2.4  1995/10/05  23:25:01  raj
 * CSCdi41279:  Multicast fastswitching doesnt work when ISDN is input
 * interface.
 * Need to associate the input packet with the D-Channel swidb instead of
 * the input B-channel swidb.
 *
 * Revision 2.3  1995/09/07  23:10:18  wmay
 * CSCdi39353:  MIP to XXX fast switching failures - use common inline
 * for congestion path on high end and ip multicast, and make it go a
 * little more efficiently.
 *
 * Revision 2.2  1995/07/29  01:56:01  dino
 * CSCdi36878:  Fast multicast bypasses access lists, accounting, policy
 *              routing
 *
 * Revision 2.1  1995/06/07  21:03:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "packet.h"
#include "subsys.h"
#include "access.h"
#include "logger.h"
#include "fastswitch.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/fddi.h"
#include "../wan/smds.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../ip/ipaccess_private.h"
#include "../ip/ipaccess.h"
#include "../ip/gre.h"
#include "../ip/tunnel.h"
#include "../ip/ipfast.h"
#include "../ipmulticast/mroute.h"
#include "../ipmulticast/igmp.h"
#include "../parser/parser_defs_access.h"
#include "../crypto/crypto_public.h"
#include "../crypto/crypto_registry.h"
#include "../ip/ipaccess_inline.h"
#include "../wan/frame_relay.h"
#include "../wan/atm_dxi.h"
#include "../wan/dialer.h"
#include "../if/atm.h"
#include "../ip/ipfast_pak.h"

extern ushort fastiphdrchk(ushort *, ulong);

/*
 * Local storage - cloning paktype header pointer
 */
static paktype *ipmfast_cloned_pak;


/*
 * IP Multicast fastswitching subsystem header
 */
#define IPMULTICAST_FS_MAJVERSION  1
#define IPMULTICAST_FS_MINVERSION  0
#define IPMULTICAST_FS_EDITVERSION 1

SUBSYS_HEADER(ipmulticast_fs, IPMULTICAST_FS_MAJVERSION, 
	      IPMULTICAST_FS_MINVERSION, IPMULTICAST_FS_EDITVERSION, 
	      ipmulticast_fs_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost, "req: iphost, ipmulticast, fastswitch");


/*
 * ipmulticast_fs_init
 *
 * Register high-end fastswitching functions.
 */
void ipmulticast_fs_init (subsystype *subsys)
{
    reg_add_ipmulticast_les_fs(ipmulticast_les_fastswitch, 
				"ipmulticast_les_fastswitch");
}


/*
 * ipmulticast_fixup_smds:
 * Snip off the SMDDS trailer for scattered packets
 */
static inline void ipmulticast_fixup_smds (paktype *pak, paktype *newpak,
					   hwidbtype *input)
{
    /*
     * Strip off the inbound encaps
     */
    if (input->fast_switch_type == FS_SMDS) {
	int size, delta;
	particletype *mp;
	iphdrtype *ip;
	
	/*
	 * Adjust the various sizes in the packet. Note, 
	 * adjust the packet from the end
	 */
	ip = (iphdrtype *)ipheadstart(newpak);
	size = GETSHORT(&ip->tl) + pak->encsize;
	delta = newpak->datagramsize - size;
	mp = newpak->particlequeue.qtail;
	if (mp->data_bytes <= delta) {
	    delta -= mp->data_bytes;
	    unqueue_inline(&newpak->particlequeue, mp);
	    retparticle(mp);
	    mp = newpak->particlequeue.qtail;
	}
	mp->data_bytes -= delta;
    }
}


/*
 * ipmulticast_pak_duplicate
 *
 * For traditional platforms (contigous buffers), do a fast copy of the data 
 * packet only (not the ENCAPBYTES area of paktype). For scatter/gather 
 * platforms (like Predator) clone the packet and snip off inbound 
 * encapsulations.
 */
static inline paktype *ipmulticast_pak_duplicate (paktype *pak, ulong tl,
						  hwidbtype *input,
						  boolean *copied)
{
    paktype  *newpak;

    if (pak_has_particles(pak)) {

	/*
	 * If the packet is scattered (on a box such as Predator), we
	 * can clone the packet. If not, we have to duplicate it.
	 */
	if (!ipmfast_cloned_pak) {
	    /*
	     * First time thru, try and allocate one
	     */
	    ipmfast_cloned_pak = pool_getbuffer(fspak_pool);
	    if (!ipmfast_cloned_pak) {
		return(NULL);
	    }
	}
	if (!*copied) {
	    newpak = pak_clone(pak, ipmfast_cloned_pak);
	} else {
	    newpak = NULL;
	}

	/*
	 * If we were unable to clone or there is no rewrite particle left,
	 * simply copy the packet into another system buffer
	 */
	if (!newpak || !pool_cache_available_items(fs_particle_pool)) {
	    if (newpak) {
	        datagram_done(newpak);
	    }
	    newpak = getbuffer(pak->datagramsize);
	    if (!newpak) {
		return(NULL);
	    }
	    
	    /*
	     * Copy the data. Note: datagramstart and network_start
	     * point to the same place in the new packet. Copy the original
	     * encaps infront of datagramstart (it will get overwritten).
	     */
	    particle_copy_to_buffer(pak, newpak->datagramstart - pak->encsize);
	    *copied = TRUE;
	    return(newpak);
	}

	/*
	 * Strip off the inbound encaps if SMDS
	 */
	ipmulticast_fixup_smds(pak, newpak, input);
	newpak->datagramsize -= pak->encsize;
	particle_clip_inline(newpak->particlequeue.qhead, -pak->encsize);
	return(newpak);

    } else { 

	/*
	 * Have to make copy (sigh). Hopefully chained buffers or some RSP
	 * niftiness will help this in the future. Doing the last midb
	 * check is a small price to pay to avoid an extra copy.
	 */
	newpak = platform_fastsend_getbuffer(pak->datagramsize);
	if (!newpak) {
	    return(NULL);
	}
	newpak->if_input = NULL; /* Clear cruft for RSP packets. */
	
	/*
	 * Do fast copy.
	 */
	platform_fs_pak_data_copy(pak, newpak, pak->network_start, 
				  newpak->network_start, tl);
	return(newpak);
    }
}

/*
 * ipmulticast_fast_drop -
 *
 * Accounting and misc. stuff for fast drops.
 */
static inline boolean ipmulticast_fast_drop (idbtype *input, mdbtype *mdb,
					     ulong   bytes,  paktype *pak)
{
    hwidbtype *in_hwidb = input->hwptr;
    
    input->ip_multicast_in++;
    in_hwidb->counters.rx_cumbytes_fast[ACCT_PROTO_IP] += bytes;
    in_hwidb->counters.inputs_fast[ACCT_PROTO_IP]++;
    ip_traffic.inputs++;
    mdb->total_received++;
    retbuffer(pak);
    return(TRUE);
}


/*	
 * ipmulticast_les_fastswitch
 *
 * Used for mid/low-end platforms.
 *
 * Interrupt level IP multicast fastswitching function. This function has
 * been optimized for speed.
 */
boolean ipmulticast_les_fastswitch (paktype *pak)
{
    paktype    *pak_copy;
    iphdrtype  *ip;
    idbtype    *idb, *input_swidb;
    hwidbtype  *hwidb, *input_hwidb;
    gdbtype    *gdb;
    mdbtype    *mdb;
    midbtype   *midb;
    ipaddrtype group, source;
    boolean    sent, copied;
    ulong      bytes, input_length;
    ushort     subint_ref_num;
    boolean    fast_drop_ok;
    
    if (!ip_mrouter_running) {
	return(FALSE);
    }
    
    ip = (iphdrtype *) ipheadstart(pak);

    /*
     * Cache header if enabled.
     */
    ipmulticast_cache_header_inline(ip);
   
    /*
     * IGMP packets are passed to process level.
     */
    if (ip->prot == IGMP_PROT) {
	return(FALSE);
    }

    /*
     * Get software idb.
     */
    idb = pak->if_input;
    input_swidb = idb;
    input_hwidb = idb->hwptr;
    
    /*	
     * Not fastswitching on this interface?  Bail, unless just maybe
     * this is ISDN.
     */
    if (!idb->ip_multicast_fs) {

	/*
	 * If the packet came in on an ISDN B-channel or a multilink 
         * virtual template, we need to find
	 * the D-channel's swidb because that's the one entered into
	 * the mroute information.
	 */
	hwidb = idb->hwptr;
	if (is_ddr(hwidb) || is_isdn(hwidb)) {
	    idb = get_netcfg_idb(idb);
	    if (!idb->ip_multicast_fs) return(FALSE);
	} else {
            return(FALSE);
        }
    }
    
    /*
     * Find group in multicast routing table. If there is none, we have to
     * build state. Do that at process level. No need to sniff anything.
     */
    group = GETLONG(&ip->dstadr);
    gdb = ip_get_gdb_inline(group);
    if (!gdb) {
	return(FALSE);
    }

    /*
     * Get (S,G) entry. Check to see if this entry is "in the cache" put
     * there by process level. This is achieved by the mdb->fastswitch flag.
     */
    source = GETLONG(&ip->srcadr);
    mdb = ip_get_best_mdb_inline(gdb, source);
    if (!mdb) {

	/*
	 * (*,G) threshold has exceeded, go build (S,G) at process level. Go
	 * to process level to build (S,G) if we are first-hop router to
	 * the source (and we are DR).
	 */
	if (gdb->join_spt) {
	    return(FALSE);
	}
	if (idb->ip_multicast_dr && 
	    ip_address_on_idb(GETLONG(&ip->srcadr), idb)) {
	    return(FALSE);
	}
	mdb = &gdb->mdb;
    }
   
    if (!mdb->fastswitch) {
	return(FALSE);
    }
    
    bytes = GETSHORT(&ip->tl);

    /*
     * Do RPF check. If fails, check if we should bail to process level.
     */
    if (mdb->if_input != idb) {

	/*
	 *
	 * If the SPT flag is set for the (S,G) or (*,G) entry (true
	 * if we have SPT threshold configured), check if we want to do
	 * a fast drop.
	 *
	 * If the SPT flag is not set, see if an RPF failure occured on
	 * the (*,G). If so, try to fast drop. 
	 */
	if ((mdb->flags & SPT_FLAG) || (gdb->mdb.if_input != idb)) {
	    if (ipmulticast_rpf_fail_fast_drop_inline(idb, mdb, source)) {
		mdb->rpf_failed++;
		return(ipmulticast_fast_drop(input_swidb, mdb, bytes, pak));
	    } else {
		return(FALSE);
	    }
	}

	/*
	 * (*,G) threshold has exceeded, go build (S,G) at process level.
	 */
	if (gdb->join_spt) {
	    return(FALSE);
	}
	mdb = &gdb->mdb;
    }
   
    /*
     * If the register flag is set, we may have to send registers.
     * Do not fast drop or forward using the fast path.
     */
    if (mdb->register_flag)
	return(FALSE);
     
    /*
     * The output list is empty or the MDB is in prune state.
     * Fast drop if we dont have local memebers.
     */
    if ((MDB_OUTPUTQ_NULL(mdb)) && (!(mdb->flags & LOCAL_FLAG))) {

	/*
	 * On P2P links, punt to process level unless the prune 
	 * timer is running.
	 */
	if (is_p2p(idb) && !TIMER_RUNNING(mdb->send_prune))
	    return(FALSE);
	
	return(ipmulticast_fast_drop(input_swidb, mdb, bytes, pak));    
    }
   
    /*
     * Do IP header check since ip_fastswitch() only does it when there
     * is a unicast cache hit.
     */
    if (fastiphdrchk((ushort *)ip, group)) return(FALSE);
    pak->flags |= PAK_MCICHECKED;
    input_length = bytes + pak->encsize;
    
    /*
     * Should we rate-limit on input?
     */
    if (ip_rate_limit_check(ip->prot, ((udptype *)ipdatastart_iph(ip))->dport,
			    mdb->bits_sec, bytes, mdb->in_limit,
			    mdb->port_number)) {
	return(ipmulticast_fast_drop(input_swidb, mdb, bytes, pak));    
    }

    /*
     * Check input access-lists.
     * (Do not use input_hwidb here to take care of ISDN D-channel case)
     */
    if (idb->hwptr->ip_fast_flags & IP_FAST_INPUTACCESSLISTS) {
	if (!ip_accesscheck_inline(pak, idb->ip_input_accesslist,
				   NULL, TRUE)) {
	    return(ipmulticast_fast_drop(input_swidb, mdb, bytes, pak));
	}
    }
    
    /*
     * If the packet is bigger than the minimal IP MTU of interfaces on
     * mdb olist, let the process level fragment it and do the forwarding.
     */
    if (bytes > mdb->ip_min_mtu) return(FALSE);

    /*
     * Assume that we may not drop the packet at interrupt level if
     * replication fails (i.e. sent is false).  This will be set to true
     * if there is no reason to punt to process level.
     */
    fast_drop_ok = FALSE;
    
    /*
     * Do replication.
     */
    sent = copied = FALSE;
    for (midb = (midbtype *) mdb->if_outputQ.qhead; midb; midb = midb->next) {
	if (midb->state == MIDB_PRUNE_STATE) continue;
	idb = midb->idb;

	/*	
	 * TTL and access-list checks.
	 */
	if (ip->ttl < idb->ip_multicast_threshold) {
	    fast_drop_ok = TRUE;
	    continue;
	}
	
	if (idb->hwptr->ip_fast_flags & IP_FAST_ACCESSLISTS) {
	    if (!ip_accesscheck_inline(pak, idb->ip_output_accesslist,
				       NULL, TRUE)) {
		accessfailcnt++;
		if (idb->ip_output_accesslist->type == ACL_IP_SIMPLE ||
		    idb->ip_output_accesslist->type == ACL_IP_NAMED_SIMPLE) {
		    fast_drop_ok = TRUE;
		}
		continue;
	    }
	}

	/*
	 * Should we rate-limit on output?
	 */
	if (ip_rate_limit_check(ip->prot,
				((udptype *)ipdatastart_iph(ip))->dport,
				mdb->bits_sec, bytes, midb->out_limit,
				midb->port_number)) {
	    fast_drop_ok = TRUE;
	    continue;
	}
	
	/*
	 * MAC header not set up by process level. This could have happened
	 * based on a TTL or access-list denial in process switching and
	 * the mdb was put in the cache. Process level will transmit packet
	 * on the rest of the interfaces from this point on in the list.
	 */
	if (!midb->mac_header) break;

	/*
	 * Do IP accounting for interface, if enabled.
	 */
	if (idb->hwptr->ip_fast_flags & IP_FAST_ACCOUNTING) {
	    if (!ip_fast_accumulate_acctg(source, group, bytes, 1)) {
		return(FALSE);
	    }
	}

	/*
	 * Clone the packet. On traditional boxes, this requires copying
	 * the frame. On boxes that support scatter/gather IE Predator,
	 * it involves cloning the data structures for the packet 
	 */
	if (copied || midb->next) {
	    pak_copy = ipmulticast_pak_duplicate(pak, bytes, 
						 input_hwidb, &copied);
	    if (!pak_copy) {
		return(FALSE);
	    }

	    pak_copy->flags = pak->flags & ~PAK_INPUTQ;

	} else {
	    pak_copy = pak;
	    pak = NULL;
	    if (pak_has_particles(pak_copy)) {
		ipmulticast_fixup_smds(pak_copy, pak_copy, input_hwidb);
		pak_copy->datagramsize -= pak_copy->encsize;
		particle_clip_inline(pak_copy->particlequeue.qhead, 
				     -pak_copy->encsize);
	    }
	}

	hwidb = idb->hwptr;
	if (hwidb->tunnel) {

	    /*
	     * Do tunnel accounting.
	     */
	    hwidb->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += bytes;
	    hwidb->counters.outputs_fast[ACCT_PROTO_IP]++;
	    hwidb = hwidb->tunnel->header->if_output->hwptr;
	}

	/*
	 * Get subinterface reference number (usually a circuit or multiplexing
	 * identifier) from the idb if it exists. Unicast IP keeps the ref# in
	 * the cache pointer.
	 */
	if (idb->p2pmac) {
	    subint_ref_num = *((ushort*) idb->p2pmac->addr);
	} else {
	    subint_ref_num = 0;
	}

	if (pak_has_particles(pak_copy)) {
	    /*
	     * Scatter/gather packets
	     */
	    if (!ip_pak_write_mac_scattered_inline(pak_copy, idb->hwptr,
						   (ulong *)midb->mac_header, 
						   midb->mac_length, subint_ref_num)) {
		if (pak_copy == ipmfast_cloned_pak) {
		    datagram_done(pak_copy);
		} else {
		    /*
		     * Failed to do the rewrite on the last output interface.
		     * So we must restore the original state of the inbound pak
		     */
		    pak_copy->datagramsize += pak_copy->encsize;
		    particle_clip_inline(pak_copy->particlequeue.qhead, 
					 pak_copy->encsize);
		}
		return(FALSE);
	    }
	} else {
	    /*
	     * Traditional packets, contiguous
	     */
	    pak_copy->datagramsize = bytes;
	    ip_pak_write_mac_inline(pak_copy, idb->hwptr,
				    (ulong *)midb->mac_header, 
				    midb->mac_length, subint_ref_num);
	}
	pak_copy->if_output = idb;
	pak_copy->linktype = LINK_IP;
	pak_copy->acct_proto = ACCT_PROTO_IP;
	(*hwidb->fastsend)(hwidb, pak_copy);
	sent = TRUE;
	midb->uses++;
	idb->ip_multicast_out++;
    }

    /*
     * If we have sent the packet out at least one OIF, flag that
     * information in the packet.
     */
    if (sent && pak) {
	pak->flags |= PAK_MFS_SWITCHED;
    }

    /*
     * We didn't finish the list. The rest of list is done at process level.
     */
    if (midb) {
	return(FALSE);
    }
    
    /*
     * Do stats.
     */
    if (sent) {
	mdb->total_received++;
	mdb->uses++;
	mdb->uses_sec++;
	mdb->bytes += bytes;
	mdb->bits_sec += bytes << 3;
	GET_TIMESTAMP(mdb->lastinput_fs);
	TIMER_START(mdb->expires, mdb->expiration_time);

	/* 
	 * Do input interface stats.
	 */
	input_hwidb->counters.rx_cumbytes_fast[ACCT_PROTO_IP] += input_length;
	input_hwidb->counters.inputs_fast[ACCT_PROTO_IP]++;
	ip_traffic.inputs++;
	
	/*
	 * There might be an extra copy if last midb was denied transmission.
	 * If we didn't process the entire list, keep original buffer so
	 * it can be queued for process level.
	 */
	if (pak) retbuffer(pak);

	/*
	 * Bump input counter. The entire list was fastswitched. Otherwise,
	 * the counter is bumped at process level.
	 */
	input_swidb->ip_multicast_in++;
    } else if (fast_drop_ok) {
	return(ipmulticast_fast_drop(input_swidb, mdb, bytes, pak));
    }
    return(sent);
}
