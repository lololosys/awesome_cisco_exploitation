/* $Id: ipmfast_c7000.c,v 3.7.12.17 1996/07/30 01:09:25 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/src-68-c7000/ipmfast_c7000.c,v $
 *------------------------------------------------------------------
 * ipmfast_c7000.c -- IP multicast fastswitching routines that are 7000 
 *                    specific.
 *
 * December 1994
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipmfast_c7000.c,v $
 * Revision 3.7.12.17  1996/07/30  01:09:25  mbeesley
 * CSCdi55762:  Need to support fast IP multicasting on platforms
 * capable of scatter/gather IE c7200
 * Branch: California_branch
 *
 * Revision 3.7.12.16  1996/07/06  05:56:43  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.7.12.15  1996/06/28  00:08:23  mleelani
 * CSCdi61578:  Router with a local member, but with NULL OIF doesnt reply
 * to mping.
 * Branch: California_branch
 * Fast drop only if we dont have local members.
 *
 * Revision 3.7.12.14  1996/06/27  08:00:15  mleelani
 * CSCdi61079:  Occasional traffic loss because of bogus DVMRP prunes
 * Branch: California_branch
 * o Dont send prune on non-RPF p2p link, if it does not appear in the OIF.
 * o Prevent bogus prune sent to RPF neighbor on tunnel when the dvmrp peer
 *   is a non-pruner.
 *
 * Revision 3.7.12.13  1996/06/18  01:48:42  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.7.12.12  1996/06/12  03:56:29  mleelani
 * CSCdi58720:  Multicast fastswitching does not do fast drop.
 * Branch: California_branch
 * DO fast drop.
 *
 * Revision 3.7.12.11  1996/06/08  04:12:14  lwei
 * CSCdi59415:  mdb timer incorrectly shrank at data forwarding time
 * Branch: California_branch
 *
 * Revision 3.7.12.10  1996/05/08  04:29:24  lwei
 * CSCdi54559:  IP multicast is sending larger than MTU sized packets over
 * GRE
 * Branch: California_branch
 *
 * Revision 3.7.12.9  1996/05/04  01:47:06  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Convert tbridge flood array to a linked list of tbifd's.
 *
 * Name Access List
 *
 * Revision 3.7.12.8  1996/04/27  06:28:11  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.7.12.7  1996/04/24  02:21:23  mleelani
 * CSCdi55507:  IP multicast fastswitching failure on priority queueing
 * path.
 * Branch: California_branch
 * Allow priority queueing in multicast fastswitch path.
 *
 * Revision 3.7.12.6  1996/04/22  21:41:12  mleelani
 * CSCdi54259:  Multicast fast switching confuses RIF cache
 * Branch: California_branch
 * Add code review changes. Make sure prunes are sent if the outgoing
 * interface list is empty.
 *
 * Revision 3.7.12.5  1996/04/21  21:57:30  mleelani
 * CSCdi54259:  Multicast fast switching confuses RIF cache
 * Branch: California_branch
 * Incorrect fallthrough to process level when we have a fully pruned
 * list of output interfaces.
 *
 * Revision 3.6.10.3  1996/04/26  00:35:55  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.6.10.2  1996/04/17  04:18:33  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.6.10.1  1996/02/23  20:58:40  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.7.12.4  1996/04/19  15:30:50  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.11  1996/04/17  23:01:54  carrel
 * Branch: IosSec_branch
 * Final commit from IP code review comments
 *
 * Revision 2.1.6.10  1996/04/09  21:56:42  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.1.6.9  1996/04/02  19:49:37  carrel
 * header cleanup
 *
 * Revision 2.1.6.8  1996/03/05  04:42:33  che
 * Branch: IosSec_branch
 * Removed reference to crypto_engine_rsp_registry.h.
 *
 * Revision 2.1.6.7  1996/02/21  04:06:49  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.1.6.6  1996/02/13  19:18:44  carrel
 * Branch: IosSec_branch
 * Crypto Map checkin (finally)
 *
 * Revision 2.1.6.5  1996/01/20  17:02:32  xliu
 * Branch: IosSec_branch
 * Solve 7000 problem with ip les fs.
 *
 * Revision 2.1.6.4  1996/01/19  20:13:13  xliu
 * Branch: IosSec_branch
 * Add crypto registry include file.
 *
 * Revision 2.1.6.3  1995/12/19  04:16:04  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 2.1.6.2  1995/12/17  02:20:18  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.1  1995/11/17  18:13:39  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.7.12.3  1996/04/04  04:55:29  dino
 * CSCdi53046:  DR doesnt register when on shared-tree and spt-threshold
 *              configured
 * Branch: California_branch
 *
 * Revision 3.7.12.2  1996/03/27  22:29:05  mleelani
 * CSCdi51178:  ATM subinterface multicast fastswitching is not functional.
 * Branch: California_branch
 *  o Incorrect VC decode.
 *  o Missing RX FLUSH in some cases causing CBUS INIT errors.
 *
 * Revision 3.7.12.1  1996/03/18  22:00:08  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.14.2  1996/03/13  02:00:27  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.14.1  1996/02/20  17:57:09  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/13  09:32:40  dino
 * CSCdi48623:  Multicast mtraces are not processed by last hop router due
 *              to f/s
 *
 * Revision 3.6  1995/12/17  19:25:25  dino
 * CSCdi44742:  PIM should only switch to the SPT when traffic threshold
 *              is reached
 *
 * Revision 3.5  1995/11/22  22:20:34  dino
 * CSCdi44336:  mtrace interface counters not properly counted when
 * partially fs
 *
 * Revision 3.4  1995/11/22  06:57:33  dino
 * CSCdi43574:  mtrace gets native interface instead of tunnel when native
 * not mcast
 *
 * Revision 3.3  1995/11/17  18:44:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:31:45  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:26:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/09  21:45:52  raj
 * CSCdi41279:  Multicast fastswitching doesnt work when ISDN is input
 * interface.
 * Use is_ddr() instead, so it works for the general DDR case.  Additional
 * changes needed for high-end fastswitch routines as well.
 *
 * Revision 2.2  1995/07/29  01:56:12  dino
 * CSCdi36878:  Fast multicast bypasses access lists, accounting, policy
 *              routing
 *
 * Revision 2.1  1995/06/07  22:55:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "media_registry.h"
#include "packet.h"
#include "address.h"
#include "access.h"
#include "subsys.h"

#include "logger.h"

#include "../os/pool.h"
#include "../os/particle.h"

#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../hes/cbus_bcopy_inline.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/fddi.h"
#include "../if/channel.h"
#include "../if/network.h"
#include "../if/atm.h"
#include "../wan/serial.h"
#include "../wan/smds.h"
#include "../wan/atm_dxi.h"
#include "../wan/frame_relay.h"
#include "../wan/dialer.h"
#include "../util/radix.h"
#include "../ip/ipaccess_private.h"
#include "../ip/ipaccess.h"
#include "../ip/ip.h"
#include "../ip/ipfast.h"
#include "../ip/ipfast_hes.h"
#include "../ip/ip_registry.h"
#include "../ip/tunnel.h"
#include "../ip/gre.h"
#include "../ip/ipfast_pak.h"
#include "../ipmulticast/mroute.h"
#include "../ipmulticast/ipmfast.h"
#include "../ipmulticast/igmp.h"
#include "../parser/parser_defs_access.h"
#include "../wan/dialer.h"


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
    reg_add_ipmulticast_hes_fs(ipmulticast_hes_fastswitch, 
			       "ipmulticast_hes_fastswitch");
}

/*
 * ipmulticast_fs_input_stat
 *
 * Do input interface statistics.
 */
static inline void ipmulticast_fs_input_stat(hwidbtype *input, ulong rxlength,
					     mdbtype *mdb)
{
    if (input->rxtype == RXTYPE_IP_TUNNEL) 
        input->firstsw->ip_multicast_in++;
    else
        input->fast_rcvidb->ip_multicast_in++;
    input->counters.rx_cumbytes_fast[ACCT_PROTO_IP] += rxlength;
    input->counters.inputs_fast[ACCT_PROTO_IP]++;
    ip_traffic.inputs++;
    mdb->total_received++;
}

/*
 * If we have priority queing enabled, this function re-enqueues 
 * pak into the holdQ.
 */
static inline void ipmulticast_low_tql_c7000 (hwidbtype  *input,
					      hwidbtype  *hwidb,
					      midbtype   *midb,
					      mciregtype *regptr,
					      ulong      bytes)
{
    paktype *pak;
    
    /*
     * can't copy from input to output, so we'd better store in a pak,
     * and queue, or discard
     */
    pak = getbuffer(input->rxlength);
    if (pak) {
	
	/*
	 * Copy packet into RP memory.
	 */
	pak->linktype = LINK_IP;
	regptr->argreg = input->rxoffset;
	regptr->cmdreg = MCI_CMD_RX_SELECT;
	mci2buffer(&regptr->readlong, pak->datagramstart, 
		   input->rxlength);
	
	/*
	 * Grab the IP tl for the datagramsize.
	 */
	pak->network_start = pak->datagramstart + 
	    (input->rxlength - bytes);
	pak->datagramsize = ((ushort *)(pak->network_start))[1];
	pak->if_output = midb->idb;
	ip_pak_write_mac_inline(pak, hwidb,
				(ulong *)midb->mac_header,
				midb->mac_length,
				reg_invoke_get_subint_ref_num_from_pak
				(hwidb->enctype, hwidb, pak));
	/*
	 * Invalidate protocol accouting field in the packet to 
	 * prevent duplicate accounting in holdq_enqueue_fast() since
	 * accounting is already done in ipmulticast_hes_fastswitch()
	 */
	pak->acct_proto = ACCT_PROTO_LAST;
	holdq_enqueue_fast(hwidb, pak);
	midb->uses++;
    } else {
	hwidb->counters.output_total_drops++;
    }
}

/*
 * ipmulticast_flush_buffer
 *
 * FLush CBUS buffer.
 */
static inline void ipmulticast_flush_buffer (hwidbtype *hwidb, 
					     mciregtype *regptr) 
{
    
    /*
     * Flush Cbus buffer, don't need it anymore.
     */
    regptr->argreg = hwidb->mci_index;
    regptr->cmdreg = MCI_CMD_SELECT;
    regptr->cmdreg = MCI_CMD_RX_FLUSH;
}

/*
 * ipmulticast_hes_fastswitch
 *
 * Interrupt level IP multicast fastswitching function. This function has
 * been optimized for speed.
 */
boolean ipmulticast_hes_fastswitch (ipaddrtype group, hwidbtype *hwidb)
{

    gdbtype    *gdb;
    mdbtype    *mdb;
    hwidbtype  *input_hwidb, *stat_hwidb;
    idbtype    *swidb;
    midbtype   *midb, *save_midb;
    mciregtype *regptr;
    ipaddrtype source;
    ulong      bytes, ttl, length, ipoffset;
    boolean    sent;
    boolean    fast_drop_ok;
    
    if (!ip_mrouter_running) return(FALSE);

    /*
     * Get software idb. If packet came in on a tunnel, use tunnel's
     * swidb and the real interface's hwidb throughout function.
     */
    if (hwidb->rxtype == RXTYPE_IP_TUNNEL) {
	swidb = hwidb->tunnel_swidb;
	stat_hwidb = swidb->hwptr;
    } else {
        swidb = hwidb->fast_rcvidb;
        
        /*
         * Should never be the case; extreme paranoia.
         */
	if (!swidb) {
	    return(FALSE);
	}
	stat_hwidb = hwidb;
    }

    /*	
     * Not fastswitching on this interface?  Bail, unless just maybe
     * this is from a DDR type interface.
     */
    if (!swidb->ip_multicast_fs) {

	/*
	 * If the packet came in on an DDR interface, we need to find
	 * the master swidb for the interface because that's the one
	 * entered  into the mroute information.
	 */
	if (is_ddr(hwidb) || is_isdn(hwidb)) {
	    swidb = get_netcfg_idb(swidb);
	    if (!swidb->ip_multicast_fs) return(FALSE);
	} else {
            return(FALSE);
        }
    }

    /*
     * Find group in multicast routing table. If there is none, we have to
     * build state. Do that at process level. No need to sniff anything.
     */
    gdb = ip_get_gdb_inline(group);
    if (!gdb) return(FALSE);

    /*
     * Sniff IP header so we can get source IP address and other fields. 
     */
    regptr = (mciregtype *) hwidb->mci_regptr;
    ipoffset = ipmulticast_hes_rxselect(hwidb, regptr);
    if (!ipoffset) return(FALSE);

    /*
     * IGMP packets are passed to process level. Check ip->prot.
     */
    if (hwidb->sniff[SNIFFBASE+5].d.byte[1] == IGMP_PROT) return(FALSE);

    /*
     * Get (S,G) entry. Check to see if this entry is "in the cache" put
     * there by process level. This is achieved by the mdb->fastswitch flag.
     */
    source = hwidb->sniff[SNIFFBASE+6].d.lword;
    mdb = ip_get_best_mdb_inline(gdb, source);
    if (!mdb) {

	/*
	 * (*,G) threshold has exceeded, go build (S,G) at process level. Go
	 * to process level to build (S,G) if we are first-hop router to
	 * the source (and we are DR).
	 */
	if (gdb->join_spt) return(FALSE);
	if (swidb->ip_multicast_dr && ip_address_on_idb(source, swidb)) {
	    return(FALSE);
	}
	mdb = &gdb->mdb;
    }
    if (!mdb->fastswitch) return(FALSE);

    /*
     * Do RPF check. If fails, bail to process level.
     */
    if (mdb->if_input != swidb) {

	if ((mdb->flags & SPT_FLAG) || (gdb->mdb.if_input != swidb)) {
	    if (ipmulticast_rpf_fail_fast_drop_inline(swidb, mdb, source)) {
		
		mdb->rpf_failed++;
		
		/*
		 * Flush Cbus buffer, don't need it anymore.
		 */
		ipmulticast_flush_buffer(hwidb, regptr);

		/*
		 * Do input interface stats.
		 */
		ipmulticast_fs_input_stat(stat_hwidb, hwidb->rxlength, mdb);

		return(TRUE);
	    } else {
		return(FALSE);
	    }
	}

	/*
	 * (*,G) threshold has exceeded, go build (S,G) at process level.
	 */
	if (gdb->join_spt) return(FALSE);
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
	if (is_p2p(swidb) && !TIMER_RUNNING_AND_SLEEPING(mdb->send_prune))
	    return(FALSE);

	/*
	 * Flush Cbus buffer, don't need it anymore.
	 */
	ipmulticast_flush_buffer(hwidb, regptr);

	/*
	 * Do input interface stats.
	 */
	ipmulticast_fs_input_stat(stat_hwidb, hwidb->rxlength, mdb);
	
	return(TRUE);
    }

    bytes = hwidb->sniff[SNIFFBASE+3].d.sword[1];
    ttl = hwidb->sniff[SNIFFBASE+5].d.byte[0];

    /*
     * Should we rate-limit on input?
     */
    if (ip_rate_limit_check(hwidb->sniff[SNIFFBASE+5].d.byte[1],
			    hwidb->sniff[SNIFFBASE+8].d.sword[1],
			    mdb->bits_sec, bytes, mdb->in_limit,
			    mdb->port_number)) {
	
	ipmulticast_flush_buffer(hwidb, regptr);
	ipmulticast_fs_input_stat(stat_hwidb, hwidb->rxlength, mdb);
	return(TRUE);
    }

    input_hwidb = hwidb;

    /*
     * Check input access-lists.
     */
    if (swidb->ip_input_accesslist) {
	switch (swidb->ip_input_accesslist->type) {
	case ACL_IP_SIMPLE:
	case ACL_IP_NAMED_SIMPLE:
	    if (!nfast_check(swidb->ip_input_accesslist, source))
		return(FALSE);
	    break;

	case ACL_IP_EXTENDED:
	case ACL_IP_NAMED_EXTENDED:
	    if (!slow_check(swidb->ip_input_accesslist, TRUE, swidb,
			    hwidb->sniff[SNIFFBASE+5].d.byte[1], source,
			    group, hwidb->sniff[SNIFFBASE+8].d.sword[0],
			    hwidb->sniff[SNIFFBASE+8].d.sword[1], FALSE,
			    hwidb->sniff[SNIFFBASE+3].d.byte[1])) {
		return(FALSE);
	    }
	    break;

	default:
	    break;
	}
    }

    /*
     * If the packet is bigger than the minimal IP MTU of interfaces on
     * the mdb olist, let the process level do fragmentation & forwarding.
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
    sent = FALSE;
    save_midb = NULL;
    for (midb = (midbtype *) mdb->if_outputQ.qhead; midb; midb = midb->next) {
	if (midb->state == MIDB_PRUNE_STATE) continue;
	swidb = midb->idb;

	/*	
	 * TTL and access-list checks.
	 */
	if (ttl < swidb->ip_multicast_threshold) {
	    fast_drop_ok = TRUE;
	    continue;
	}
	
	if (swidb->ip_output_accesslist) {
	    switch (swidb->ip_output_accesslist->type) {

	    case ACL_IP_SIMPLE:
	    case ACL_IP_NAMED_SIMPLE:
		if (!nfast_check(swidb->ip_output_accesslist, source)) {
		    accessfailcnt++;
		    fast_drop_ok = TRUE;
		    continue;
		}
		break;

	    case ACL_IP_EXTENDED:
	    case ACL_IP_NAMED_EXTENDED:
		if (!slow_check(swidb->ip_output_accesslist, TRUE, swidb,
				hwidb->sniff[SNIFFBASE+5].d.byte[1], source,
				group, hwidb->sniff[SNIFFBASE+8].d.sword[0],
				hwidb->sniff[SNIFFBASE+8].d.sword[1], FALSE,
				hwidb->sniff[SNIFFBASE+3].d.byte[1])) {
		    accessfailcnt++;
		    continue;
		}
		break;

	    default:
		break;
	    }
	}

	/*
	 * Should we rate-limit on output?
	 */
	if (ip_rate_limit_check(hwidb->sniff[SNIFFBASE+5].d.byte[1],
				hwidb->sniff[SNIFFBASE+8].d.sword[1],
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
	 * If this is the first interface to send on, save this one for the
	 * RX_MOVE. Don't RX_MOVE if part of the outgoing interface list
	 * is process switched.
	 */
	if (!save_midb && (mdb->flags & FAST_FLAG)) {
	    save_midb = midb;
	    continue;
	}

	/*
	 * Use this interface. A tunnel hwidb has the real interface's 
	 * mci_index stored in it.
	 */
	hwidb = swidb->hwptr;

	sent = TRUE;
 	
 	/*
 	 * If we have priority queing enabled, re-enqueue
 	 * into the holdQ.
 	 */
 	if (hwidb->priority_list && hwidb->output_qcount) {
 	    ipmulticast_low_tql_c7000(input_hwidb, hwidb, midb, regptr, bytes);
   	    continue;
 	}
 
 	/*
 	 * Reserve a CBUS buffer. If not, bail.
 	 */
        regptr->argreg = hwidb->mci_index;
	regptr->cmdreg = MCI_CMD_SELECT;
	    
	regptr->argreg = hwidb->buffer_pool;
	regptr->cmdreg = MCI_CMD_TX1_RESERVE;
	    
	if (regptr->cmdreg != MCI_RSP_OKAY) {
	    if (hwidb->priority_list) {
 		ipmulticast_low_tql_c7000(input_hwidb, hwidb,
 					  midb, regptr, bytes);
 	    } else {
		hwidb->counters.output_total_drops++;
	    }
	    continue;
	}

	/*
	 * Do IP accounting for interface, if enabled.
	 */
	if (hwidb->ip_fast_flags & IP_FAST_ACCOUNTING) {
	    if (!ip_fast_accumulate_acctg(source, group, bytes, 1)) {
		return(FALSE);
	    }
	}

	/*
	 * Write MAC header to new Cbus buffer.
	 */
	midb->uses++;
	swidb->ip_multicast_out++;
	length = ipmulticast_hes_mac_rewrite_inline(hwidb, regptr, 
	    (ulong *) midb->mac_header, midb->mac_length, MCI_OFFSETSIZE,
            bytes);

	/*
	 * Tell the Cbus to make a copy.
	 */
	regptr->rx_select = ipoffset;
	cbus_bcopy(regptr, bytes);

	/*
	 * Tell the Cbus to send it.
	 */
	regptr->argreg = length + bytes;
	regptr->cmdreg = MCI_CMD_TX1_START;

	hwidb->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += 
	    (length & ~MCI_TX_ODDALIGN) + bytes;
	hwidb->counters.outputs_fast[ACCT_PROTO_IP]++;
	ip_traffic.forwarded++;
    }

    /* 
     * RX_MOVE the saved interface, if any.
     */
    if (save_midb) {
	sent = TRUE;
	hwidb = save_midb->idb->hwptr;

	/*
	 * If we have priority queuing enabled, re-enqueue
	 * into the holdQ.
	 */
	if (hwidb->priority_list && hwidb->output_qcount) {
	    ipmulticast_low_tql_c7000(input_hwidb, hwidb, save_midb, regptr,
				      bytes);

	    /*
	     * Data either queued or dropped, flush cbus buffer.
	     */
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;
	} else {
	    regptr->argreg = hwidb->mci_index;
	    regptr->cmdreg = MCI_CMD_RX_MOVE;

	    if (regptr->cmdreg != MCI_RSP_OKAY) {
		if (hwidb->priority_list) {
		    ipmulticast_low_tql_c7000(input_hwidb, hwidb, save_midb,
					      regptr, bytes);
		} else {
		    hwidb->counters.output_total_drops++;
		}

		/*
		 * Data either queued or dropped, flush cbus buffer.
		 */
		regptr->cmdreg = MCI_CMD_RX_FLUSH;
	    } else {
		/*
		 * Do IP accounting for interface, if enabled.
		 */
		if (hwidb->ip_fast_flags & IP_FAST_ACCOUNTING) {
		    if (!ip_fast_accumulate_acctg(source, group, bytes, 1)) {
			return(FALSE);
		    }
		}

		save_midb->uses++;
		save_midb->idb->ip_multicast_out++;
		length = ipmulticast_hes_mac_rewrite_inline(hwidb, regptr, 
		    (ulong *) save_midb->mac_header, save_midb->mac_length, 
		    ipoffset, bytes);

		regptr->argreg = length + bytes;
		regptr->cmdreg = MCI_CMD_TX1_START;

		hwidb->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += 
		    (length & ~MCI_TX_ODDALIGN) + bytes;
		hwidb->counters.outputs_fast[ACCT_PROTO_IP]++;
		ip_traffic.forwarded++;
	    }
	}
    } else if (!midb) {

	/*
	 * The midb is NULL because we have fully traversed the outgoing 
	 * interface list. When there is no save_midb, we haven't fastswitched
	 * any interface in the outgoing interface list or the outgoing 
	 * interface is partially fastswitched (the other part process level 
	 * switched and we need to copy the input cbus buffer into a paktype).
	 * 
	 * We need to free the cbus buffer when there is at least one interface
	 * in the outgoing interface list that the packet was sent on. 
	 *
	 * The case where all interfaces don't get the packet, we need to
	 * send to process level for Prune processing. We need to copy the
	 * cbus buffer into a paktype (the calling functions do this).
	 */
        if (sent) {
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;
        }
    }
    
    /*
     * If we have sent the packet out at least one OIF, flag that
     * information in the packet through the idb variable.
     */
    if (sent) {
	input_hwidb->ipmfast_switched = TRUE;
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
	mdb->uses++;
	mdb->uses_sec++;
	mdb->bytes += bytes;
	mdb->bits_sec += bytes << 3;
	GET_TIMESTAMP(mdb->lastinput_fs);
	TIMER_START(mdb->expires, mdb->expiration_time);

	/* 
	 * Do input interface stats.
	 */
	ipmulticast_fs_input_stat(stat_hwidb, input_hwidb->rxlength, mdb);

        /*
	 * Reset the flag in case we are done with the buffer.
         */
        input_hwidb->ipmfast_switched = FALSE; 
    } else if (fast_drop_ok) {
	return(TRUE);
    }

    return(sent);
}
