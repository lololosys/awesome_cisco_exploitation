/* $Id: ipmfast_ags.c,v 3.7.12.15 1996/08/13 23:22:25 rtio Exp $
 * $Source: /release/112/cvs/Xsys/src-68-ags/ipmfast_ags.c,v $
 *------------------------------------------------------------------
 * ipmfast_ags.c -- IP multicast fastswitching routines that are AGS+
 *                  specific.
 *
 * December 1994
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipmfast_ags.c,v $
 * Revision 3.7.12.15  1996/08/13  23:22:25  rtio
 * CSCdi59352:  Show-frame-pvc does not accurately display the packets or
 * bytes
 * Branch: California_branch
 * Added code to put dlci/dfa in swidb->p2pmac for p2p subifs so ip mcast
 * cache can pass this number to ip_pak_write_mac_inline().
 *
 * Revision 3.7.12.14  1996/07/30  01:09:22  mbeesley
 * CSCdi55762:  Need to support fast IP multicasting on platforms
 * capable of scatter/gather IE c7200
 * Branch: California_branch
 *
 * Revision 3.7.12.13  1996/07/23  13:28:18  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.7.12.12  1996/07/06  05:56:39  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.7.12.11  1996/06/28  00:08:26  mleelani
 * CSCdi61578:  Router with a local member, but with NULL OIF doesnt reply
 * to mping.
 * Branch: California_branch
 * Fast drop only if we dont have local members.
 *
 * Revision 3.7.12.10  1996/06/27  08:00:19  mleelani
 * CSCdi61079:  Occasional traffic loss because of bogus DVMRP prunes
 * Branch: California_branch
 * o Dont send prune on non-RPF p2p link, if it does not appear in the OIF.
 * o Prevent bogus prune sent to RPF neighbor on tunnel when the dvmrp peer
 *   is a non-pruner.
 *
 * Revision 3.7.12.9  1996/06/18  01:48:36  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.7.12.8  1996/06/12  03:56:25  mleelani
 * CSCdi58720:  Multicast fastswitching does not do fast drop.
 * Branch: California_branch
 * DO fast drop.
 *
 * Revision 3.7.12.7  1996/06/08  04:12:19  lwei
 * CSCdi59415:  mdb timer incorrectly shrank at data forwarding time
 * Branch: California_branch
 *
 * Revision 3.7.12.6  1996/05/08  04:29:29  lwei
 * CSCdi54559:  IP multicast is sending larger than MTU sized packets over
 * GRE
 * Branch: California_branch
 *
 * Revision 3.7.12.5  1996/04/27  06:47:48  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.7.12.4  1996/04/22  21:14:05  che
 * CSCdi55307:  AGS+ gs3-i-m does not compile
 * Branch: California_branch
 * Needs to include crypto_public.h
 *
 * Revision 3.6.10.3  1996/04/26  00:34:22  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.6.10.2  1996/04/17  04:17:35  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.6.10.1  1996/02/23  20:58:34  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.7.12.3  1996/04/09  00:10:23  mleelani
 * CSCdi53972:  Tunnel accounting incorrect for Mcast packets.
 * Branch: California_branch
 * Misc. tunnel accounting fixes.
 *
 * Revision 3.7.12.2  1996/04/04  04:55:22  dino
 * CSCdi53046:  DR doesnt register when on shared-tree and spt-threshold
 *              configured
 * Branch: California_branch
 *
 * Revision 3.7.12.1  1996/03/18  21:58:56  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.14.2  1996/03/13  02:00:21  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.14.1  1996/02/20  17:54:42  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/13  09:32:33  dino
 * CSCdi48623:  Multicast mtraces are not processed by last hop router due
 *              to f/s
 *
 * Revision 3.6  1995/12/17  19:25:32  dino
 * CSCdi44742:  PIM should only switch to the SPT when traffic threshold
 *              is reached
 *
 * Revision 3.5  1995/11/22  22:20:29  dino
 * CSCdi44336:  mtrace interface counters not properly counted when
 * partially fs
 *
 * Revision 3.4  1995/11/22  06:57:39  dino
 * CSCdi43574:  mtrace gets native interface instead of tunnel when native
 * not mcast
 *
 * Revision 3.3  1995/11/17  18:42:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:30:41  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:24:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/10/09  21:46:47  raj
 * CSCdi41279:  Multicast fastswitching doesnt work when ISDN is input
 * interface.
 * Use is_ddr() instead, so it works for the general DDR case.  Additional
 * changes needed for high-end fastswitch routines as well.
 *
 * Revision 2.4  1995/09/08  21:19:13  wmay
 * CSCdi39353:  MIP to XXX fast switching failures - go to common inline
 * for ipmulticast and congestion for high end.
 *
 * Revision 2.3  1995/07/29  01:56:08  dino
 * CSCdi36878:  Fast multicast bypasses access lists, accounting, policy
 *              routing
 *
 * Revision 2.2  1995/06/09  03:27:38  fred
 * Change parameters to ip_pak_mac_rewrite to support IP accounting
 * in the fast path
 *
 * Revision 2.1  1995/06/07  22:54:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "subsys.h"
#include "access.h"
#include "media_registry.h"

#include "logger.h"

#include "../os/pool.h"
#include "../os/particle.h"

#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/fddi.h"
#include "../if/channel.h"
#include "../if/network.h"
#include "../if/atm.h"
#include "../wan/smds.h"
#include "../util/radix.h"
#include "../ip/ipaccess_private.h"
#include "../ip/ipaccess.h"
#include "../ip/tunnel.h"
#include "../ip/gre.h"
#include "../ip/ip_registry.h"
#include "../ipmulticast/mroute.h"
#include "../ipmulticast/ipmfast.h"
#include "../ip/ipfast.h"
#include "../ipmulticast/igmp.h"
#include "../parser/parser_defs_access.h"
#include "../crypto/crypto_public.h"
#include "../ip/ipaccess_inline.h"
#include "../wan/frame_relay.h"
#include "../wan/atm_dxi.h"
#include "../wan/dialer.h"
#include "../if/atm.h"
#include "../ip/ipfast_pak.h"

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
 * ipmulticast_hes_fastswitch
 *
 * Interrupt level IP multicast fastswitching function. This function has
 * been optimized for speed.
 */
boolean ipmulticast_hes_fastswitch (ipaddrtype group, hwidbtype *hwidb)
{

    gdbtype    *gdb;
    mdbtype    *mdb;
    hwidbtype  *input_hwidb;
    idbtype    *swidb;
    midbtype   *midb;
    mciregtype *regptr;
    paktype    *pak, *copy_of_pak;
    iphdrtype  *ip;
    ipaddrtype source;
    ulong      bytes, ttl, length;
    boolean    sent;
    boolean    fast_drop_ok;
    
    if (!ip_mrouter_running) return(FALSE);

    /*
     * Get software idb. If packet came in on a tunnel, use tunnel's
     * swidb and the real interface's hwidb.
     */
    swidb = (hwidb->rxtype == RXTYPE_IP_TUNNEL) ? hwidb->tunnel_swidb :
	hwidb->firstsw;

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
	    hwidbtype *netidb;

	    netidb = get_netidb(hwidb);
	    if(netidb && netidb->firstsw)
		swidb = netidb->firstsw;
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
    ipmulticast_hes_rxselect(hwidb, regptr);

    /*
     * Get (S,G) entry. Check to see if this entry is "in the cache" put
     * there by process level. This is achieved by the mdb->fastswitch flag.
     */
    source = hwidb->sniff[SNIFFBASE+6].d.lword;
    mdb = ip_get_best_mdb_inline(gdb, source);
    if (!mdb) {

	/*
	 * (*,G) threshold has exceeded, go build (S,G) at process level.
	 */
	if (gdb->join_spt) return(FALSE);
	mdb = &gdb->mdb;
    }
    if (!mdb->fastswitch) return(FALSE);

    /*
     * Do RPF check. If fails, bail to process level if we
     * cannot fast drop.
     */
    if (mdb->if_input != swidb) {

	if ((mdb->flags & SPT_FLAG) || (gdb->mdb.if_input != swidb)) {
	    if (ipmulticast_rpf_fail_fast_drop_inline(swidb, mdb, source)) {

		/*
		 * Flush Cbus buffer, don't need it anymore.
		 */
		regptr->argreg = hwidb->mci_index;
		regptr->cmdreg = MCI_CMD_SELECT;
		regptr->cmdreg = MCI_CMD_RX_FLUSH;

		return(TRUE);
	    } else {
		return(FALSE);
	    }
	}
	
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
 
    /*
     * If the register flag is set, we may have to send registers.
     * Do not fast drop or forward using the fast path.
     */
    if (mdb->register_flag)
	return(FALSE);

    /*
     * The output list is empty or the MDB is in prune state.
     * Fast drop if there is no local member.
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
	regptr->argreg = hwidb->mci_index;
	regptr->cmdreg = MCI_CMD_SELECT;
	regptr->cmdreg = MCI_CMD_RX_FLUSH;
	
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
	
	/*
	 * Flush Cbus buffer, don't need it anymore.
	 */
	regptr->argreg = hwidb->mci_index;
	regptr->cmdreg = MCI_CMD_SELECT;
	regptr->cmdreg = MCI_CMD_RX_FLUSH;
	return(TRUE);
    }

    input_hwidb = hwidb;

    /*
     * Copy packet into RP memory.
     */
    pak = getbuffer(hwidb->rxlength);
    if (!pak) return(FALSE);
    pak->datagramsize = hwidb->rxlength;
    regptr->argreg = hwidb->rxoffset;
    regptr->cmdreg = MCI_CMD_RX_SELECT;
    mci2buffer(&regptr->readlong, pak->datagramstart, pak->datagramsize);

    pak->network_start = pak->datagramstart + (hwidb->rxlength - bytes);

    /*
     * IGMP packets are passed to process level.
     */
    ip = (iphdrtype *) ipheadstart(pak);
    if (ip->prot == IGMP_PROT) {
	retbuffer(pak);
	return(FALSE);
    }

    /*
     * Account for odd aligned start for FDDI frames. mci2buffer() reads an 
     * extra byte in the front of the MAC header.
     */
    if (hwidb->fast_switch_type == FS_FDDI) pak->network_start++;

    /*
     * Check input access-lists.
     */
    if (input_hwidb->ip_fast_flags & IP_FAST_INPUTACCESSLISTS) {
	if (!ip_accesscheck_inline(pak, swidb->ip_input_accesslist,
				   NULL, TRUE)) {
	    retbuffer(pak);
	    return(FALSE);
	}
    }

    /*
     * If the packet is bigger than the minimal IP MTU of itnerfaces on
     * mdb olist, let the process level fragment it and do the forwarding
     */
    if (bytes > mdb->ip_min_mtu) {
	retbuffer(pak);
	return(FALSE);
    }

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
	    if (!ip_accesscheck(pak, swidb->ip_output_accesslist)) {
		accessfailcnt++;
		if (swidb->ip_output_accesslist->type == ACL_IP_SIMPLE ||
		    swidb->ip_output_accesslist->type == ACL_IP_NAMED_SIMPLE) {
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
	 * Use this interface.
	 */
	hwidb = swidb->hwptr;
	regptr = (mciregtype *) hwidb->mci_regptr;
        regptr->argreg = hwidb->mci_index;
	regptr->cmdreg = MCI_CMD_SELECT;
	    
	regptr->argreg = hwidb->buffer_pool;
	regptr->cmdreg = MCI_CMD_TX1_RESERVE;
	    
	if (regptr->cmdreg != MCI_RSP_OKAY || hwidb->output_qcount) {

	    /*
	     * can't copy from input to output, so we'd better store in a pak,
	     * and queue, or discard
	     */
	    copy_of_pak = pak_duplicate(pak);
	    if (copy_of_pak) {
		copy_of_pak->linktype = LINK_IP;
		copy_of_pak->acct_proto = ACCT_PROTO_IP;
		ip_pak_write_mac_inline(copy_of_pak, hwidb,
					(ulong *)midb->mac_header,
					midb->mac_length,
					reg_invoke_get_subint_ref_num_from_pak(hwidb->enctype, 
									       hwidb, copy_of_pak));
		/*
		 * Invalidate the accounting protocol field to prevent
		 * duplicate accounting in holdq_enqueue_fast.
		 */
		copy_of_pak->acct_proto = ACCT_PROTO_LAST;
		holdq_enqueue_fast(hwidb, copy_of_pak);
	    } else
		hwidb->counters.output_total_drops++;
	    continue;
	}

	/*
	 * Do IP accounting for interface, if enabled.
	 */
	if (hwidb->ip_fast_flags & IP_FAST_ACCOUNTING) {
	    if (!ip_fast_accumulate_acctg(source, group, bytes, 1)) {
		retbuffer(pak);
		return(FALSE);
	    }
	}

	/*
	 * Write new MAC header.
	 */
	swidb->ip_multicast_out++;
	length = ipmulticast_hes_mac_rewrite_inline(hwidb, regptr, 
	    (ulong *) midb->mac_header, midb->mac_length, MCI_OFFSETSIZE,
            bytes);
	
	buffer2mci(pak->network_start, &regptr->write1long, bytes);
	regptr->argreg = length + bytes;
	regptr->cmdreg = MCI_CMD_TX1_START;

	hwidb->counters.tx_cumbytes_fast[ACCT_PROTO_IP] += 
	    (length & ~MCI_TX_ODDALIGN) + bytes;
	hwidb->counters.outputs_fast[ACCT_PROTO_IP]++;
	sent = TRUE;
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
	sent = fast_drop_ok = FALSE;
    }

    /*
     * Do multicast routing stats.
     */
    if (sent) {
	mdb->uses++;
	mdb->uses_sec++;
	mdb->bytes += bytes;
	mdb->bits_sec += bytes << 3;
	GET_TIMESTAMP(mdb->lastinput_fs);
	TIMER_START(mdb->expires, mdb->expiration_time);
 	
 	/*
 	 * Flush Cbus buffer, don't need it anymore.
 	 */
 	regptr = input_hwidb->mci_regptr;
 	regptr->argreg = input_hwidb->mci_index;
 	regptr->cmdreg = MCI_CMD_SELECT;
 	regptr->cmdreg = MCI_CMD_RX_FLUSH;
  
	/* 
	 * Do input interface stats.
	 */
 	if (input_hwidb->rxtype == RXTYPE_IP_TUNNEL) {
 	    input_hwidb = input_hwidb->tunnel_swidb->hwptr;
 	}
	input_hwidb->counters.rx_cumbytes_fast[ACCT_PROTO_IP]
	    += input_hwidb->rxlength;
	input_hwidb->counters.inputs_fast[ACCT_PROTO_IP]++;
	ip_traffic.inputs++;

	/*
	 * Bump input counter. The entire list was fastswitched. Otherwise,
	 * the counter is bumped at process level.
	 */
	input_hwidb->firstsw->ip_multicast_in++;
       
        /*
	 * Reset the flag in case we are done with the buffer.
         */
        input_hwidb->ipmfast_switched = FALSE;
    } else if (fast_drop_ok) {
	sent = TRUE;
    }

    /*
     * Return RP buffer.
     */
    retbuffer(pak);
    return(sent);
}
