/* $Id: ipmfast.c,v 3.6.20.11 1996/08/07 23:50:29 dino Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/ipmfast.c,v $
 *------------------------------------------------------------------
 * ipmfast.c - IP multicast fastswitching support functions.
 *
 * December, 1994.
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ipmfast.c,v $
 * Revision 3.6.20.11  1996/08/07  23:50:29  dino
 * CSCdi65425:  Make SPARSE_FLAG setting more consistent amoung rtrs in a
 *              PIM domain
 * Branch: California_branch
 *
 * Revision 3.6.20.10  1996/07/23  23:38:13  mleelani
 * CSCdi61494:  Multicast fastswitching with incoming Dialer (ISDN) is
 * broken
 * Branch: California_branch
 * Support multicast fastswitching on incoming dialers.
 *
 * Revision 3.6.20.9  1996/06/27  07:59:50  mleelani
 * CSCdi61079:  Occasional traffic loss because of bogus DVMRP prunes
 * Branch: California_branch
 * o Dont send prune on non-RPF p2p link, if it does not appear in the OIF.
 * o Prevent bogus prune sent to RPF neighbor on tunnel when the dvmrp peer
 *   is a non-pruner.
 *
 * Revision 3.6.20.8  1996/06/27  03:27:44  mleelani
 * CSCdi60876:  Multicast fastswitching flag should be compatible with
 * unicast
 * Branch: California_branch
 * Reflect unicast fastswitching polarity
 *
 * Revision 3.6.20.7  1996/06/17  23:32:55  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.6.20.6  1996/06/12  03:56:06  mleelani
 * CSCdi58720:  Multicast fastswitching does not do fast drop.
 * Branch: California_branch
 * DO fast drop.
 *
 * Revision 3.6.20.5  1996/05/06  16:44:00  dino
 * CSCdi56689:  When a multipoint-VC goes away, the IP multicast cache is
 *              not fixed
 * Branch: California_branch
 *
 * Revision 3.6.20.4  1996/05/04  01:08:30  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Relocate the defintions for IGMP versions.
 *
 * Name Access List
 *
 * Revision 3.6.20.3  1996/03/27  22:17:41  mleelani
 * CSCdi51176:  Crash during show ip mcache
 * Branch: California_branch
 * Do not cache the OIF before blocking.
 *
 * Revision 3.6.20.2  1996/03/27  21:57:57  mleelani
 * CSCdi49709:  Broadcast/multicast translation using helpers not
 * functional.
 * Branch: California_branch
 * Use helper addresses to provide the translation
 *
 * Revision 3.6.20.1  1996/03/18  20:18:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.20.3  1996/03/13  01:38:37  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.20.2  1996/03/07  09:48:02  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.20.1  1996/02/20  01:01:24  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/01/26  20:03:31  dino
 * CSCdi47621:  IP multicast fastswitching doesnt work on dialerless ISDN
 *              interface. Also, packets are not process level switched
 *              when PPP compression is enabled in incoming interface.
 *
 * Revision 3.5  1996/01/23  22:09:41  mleelani
 * CSCdi47204:  Unexpected exception to CPU vector 2 in ip_mcache_update
 * Mcache for tunnels were getting corrupted.
 *
 * Revision 3.4  1995/11/28  22:37:50  mleelani
 * CSCdi44147:  Crash in free() during show ip mroute
 * Radix head getting freed while in use.
 *
 * Revision 3.3  1995/11/17  17:29:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:42:07  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:01:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:05:57  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/08/23  20:02:35  dino
 * CSCdi39186:  CMNS disables IP multicast fastswitching.
 *
 * Revision 2.3  1995/07/24  07:33:37  hampton
 * Transition IP Multicast support to use the passive timer macros for all
 * its timers.  It no longer references the system clock directly.
 * [CSCdi37539]
 *
 * Revision 2.2  1995/07/14 05:08:31  dino
 * CSCdi36055:  IP multicast doesnt check malloc return code
 *
 * Revision 2.1  1995/06/07  21:03:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "logger.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/fddi.h"
#include "../if/channel.h"
#include "../if/atm.h"
#include "../if/network.h"
#include "../util/radix.h"
#include "../wan/smds.h"
#include "../ui/debug.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "../ip/ip.h"
#include "../ip/gre.h"
#include "../ip/tunnel.h"
#include "../ip/ip_registry.h"
#include "../ipmulticast/mroute.h"
#include "../ipmulticast/ipmulticast_debug.h"
#include "../ipmulticast/ipmfast.h"
#include "../ipmulticast/igmp.h"
#include "../ipmulticast/pim.h"


/*
 * Fast drop routine on getting RPF failures.
 */
inline boolean ipmulticast_rpf_fail_fast_drop_inline (idbtype *input,
						      mdbtype *mdb,
                                                      ipaddrtype source)
{
    midbtype *midb;

    if (input->pim_nbma_mode) {

	/*
	 * Fast drop, if NBMA mode.
	 */
	return(TRUE);
    }

    /*
     * Fast drop if we received packet on a non-rpf interface
     * not in the OIF.
     */
    midb = ip_get_midb_inline(&mdb->if_outputQ, input, 0, FALSE);
    if (!midb) {
	return(TRUE);
    }

    /*
     * On a P2P link with prune timer running, we can fast drop.
     */
    if (is_p2p(input)) {
	midb->send_prune = TRUE;
	
	if (TIMER_RUNNING_AND_SLEEPING(mdb->send_prune)) {
	    return(TRUE);
	}
    } else {

	/*
	 * Fast drop if we received the packet on an OIF where we
	 *              are ratelimiting asserts, and we already
	 *              have sent one in that period.
	 */
	if (ELAPSED_TIME(midb->last_assert) < PIM_ASSERT_FREQUENCY) {	
	    return(TRUE);
	}
    }

    return(FALSE);
}

/*
 * ip_mcache_update
 *
 * Get MAC header from packet and store in midb.
 */
void ip_mcache_update (paktype *pak)
{

    iphdrtype *ip;
    gdbtype   *gdb;
    mdbtype   *mdb;
    midbtype  *midb;
    hwidbtype *hwidb;
    idbtype   *if_output;
    trrif_t   *trh;
    uchar     *start;
    tunnel_info *tinfo;
    ulong     len;

    if (!pak->if_output) return;

    /*
     * Disable generic tunnel processing for now till
     * we add support for GRE fastswitching for high end
     * platforms. The mcache gets populated when the
     * media output vencap routine calls the cache update registry.
     * Return if tunnel vencap calls the cache update registry.
     */
    if (is_tunnel(pak->if_output->hwptr))
	return;

    /*
     * Don't bother doing lookup if not a multicast packet. Multicast packets
     * encapsulated in unicast tunnels will have the PAK_MULTICAST flag set.
     */
    if (!(pak->flags & PAK_MULTICAST)) return;

    /*
     * There could be incoming broadcast packets translated to
     * multicast packets because of the helper map feature.
     * Do not attempt to populate the header for such a packet.
     */
    if (pak->flags & PAK_BROADCAST) return;
    
    ip = (iphdrtype *) ipheadstart(pak);

    /*
     * If tunnel interface, get tunnel software idb and position the IP header
     * to the inner header (the multicast packet). Note the test for 
     * destination address since this packet is outbound.
     */
    if (ip->prot == GRE_PROT || ip->prot == IPINIP_PROT) {
	if_output = NULL;
	FOR_ALL_TUNNELS(tinfo) {
	    if (ip->dstadr != tinfo->destination) continue;
	    ip = (iphdrtype *) ipdatastart(pak);
	    if (tinfo->mode == TUN_MODE_GRE_IP) {
		ip = (iphdrtype *) ((char *) ip + GREHEADERBYTES(tinfo));
	    }
	    if_output = tinfo->hwidb->firstsw;
	    break;
	}
	if (!if_output) return;
    } else {
	if_output = pak->if_output;
    }

    /*
     * Interface has multicast fastswitching disabled, don't populate
     * MAC header in midb.
     */
    if (!if_output->ip_multicast_fs) return;

    /*
     * Get mutlicast routing table entry. If the gdb, mdb, or midb is not
     * found, return.
     */
    gdb = ip_get_gdb(ip->dstadr);
    if (!gdb) return;
    mdb = ip_get_best_mdb(gdb, ip->srcadr);
    if (!mdb) mdb = &gdb->mdb;
    midb = ip_get_midb(&mdb->if_outputQ, if_output, pak->desthost, 
		       IS_SPARSE_GROUP(gdb));
    if (!midb) return;

    /*
     * Already populated header.
     */
    if (midb->mac_header) return;

    /*	
     * Populate midb with MAC header.
     */
    len = pak->encsize;
    start = pak->datagramstart;

    /*
     * If interface is a tunnel, get real hwidb of next-hop to reach
     * tunnel endpoint.
     */
    hwidb = if_output->hwptr;
    if (is_tunnel(hwidb)) {
	hwidb = hwidb->tunnel->header->if_output->hwptr;
    }

    /*
     * Find out how much to copy and where to start from packet into midb.
     * This depends on encapsulation type.
     */
    if (hwidb->status & IDB_ETHER) {
	len = ETHER_ARPA_ENCAPBYTES;
    } else if (hwidb->status & IDB_FDDI) {
	len = FDDI_SNAP_ENCAPBYTES - 1;
	start++;
    } else if (hwidb->status & IDB_TR) {
	trh = (trrif_t *) start;
	len = TRING_SNAP_ENCAPBYTES - 2;
	if (trh->saddr[0] & TR_RII) {
	    len += (trh->rc_blth & RC_LTHMASK);
	}
	start += 2;
    } else if (hwidb->status & (IDB_SERIAL | IDB_ATM)) {

	/*
	 * Fall through. Use datagramstart and encsize.
         */
    } else if (hwidb->status & IDB_CHANNEL) {
	len = CHANNEL_ENCAPBYTES;
    } else if (hwidb->status & IDB_LEX) {

      /*
       * This should really be pak->encsize but PPP resets it to zero.
       */
       len = pak->network_start - pak->datagramstart;
    } else {
	return;
    }

    midb->mac_length = len;
    midb->mac_header = malloc(len);
    if (!midb->mac_header) return;

    bcopy(start, midb->mac_header, len);

    if (MCACHE_DEBUG(mdb->group)) {
	buginf("\nMRC: Build MAC header for (%i/%d, %i), %s",
	       ip_radix2addr(mdb->source), 
	       ip_bitsinmask(ip_radix2addr(mdb->mask)), mdb->group,
	       midb->idb->namestring);
    }
}

/*
 * ip_invalidate_mcache
 *
 * Invalidate a olist cache entry from the mdb when an ATM VC gets cleared.
 */
void ip_invalidate_mcache (idbtype *idb)
{

    gdbtype  *gdb;
    midbtype *midb;
    int      i;

    if (!idb->ip_multicast_fs) return;
    if (!(idb->hwptr->status & IDB_ATM)) return;

    for (i = 0; i < NETHASHLEN; i++) {
	for (gdb = ip_mroute_cache[i]; gdb; gdb = gdb->next) {
	    midb = ip_get_midb(&gdb->mdb.if_outputQ, idb, 0, FALSE);
	    if (!midb) continue;

	    /*
	     * The ATM interface is in the (*,G). That means it has a very
	     * good chance being in the (S,G)s. Scan through them as well as
	     * the (*,G) entry.
	     */
	    ip_invalidate_each_mcache((rntype *) &gdb->mdb, (va_list) &idb);
	    rn_walktree(gdb->radix_head->rnh_treetop, 
			ip_invalidate_each_mcache, idb);
	}
    }
}

/*
 * ip_invalidate_each_mcache
 */
int ip_invalidate_each_mcache (rntype *node, va_list args)
{
    
    mdbtype  *mdb;
    idbtype  *idb;
    midbtype *midb;
    uchar    *header;

    mdb = (mdbtype *) node;
    idb = va_arg(args, idbtype *);

    /* 
     * Find ATM interface in olist. If not found or there is no MAC header
     * populated, go to next entry.
     */
    midb = ip_get_midb(&mdb->if_outputQ, idb, 0, FALSE);
    if (!midb || !midb->mac_header) return(0);

    /*
     * Flag this entry so process level can populate a new ATM header. Get
     * rid of the old one.
     */
    mdb->fastswitch = FALSE;
    header = midb->mac_header;
    midb->mac_header = NULL;
    midb->mac_length = 0;
    free(header);
    return(0);
}

/*
 * ip_mroute_cache_command
 *
 * Enable/Disable IP multicast fastswitching on an interface. This command
 * is different than the "ip route-cache" command in that unicast routing
 * enables/disables fastswitching on an output interface basis. Multicast
 * routing does it on both input interface and each interface in the
 * outgoing interface list.
 */
void ip_mroute_cache_command (parseinfo *csb)
{

    idbtype   *idb;
    
    idb = csb->interface;
    if (!idb) return;

    /*
     * NV generation.
     */
    if (csb->nvgen) {	
	if (!idb->ip_multicast_fs) {
	    nv_write(TRUE, "no %s", csb->nv_command);
	}
	return;
    }

    /*
     * Don't allow multicast fastswitching on X.25 interfaces.
     */
    if (is_x25(idb->hwptr)) {
	printf("\nIP multicast fastswitching not allowed on X.25 interfaces");
	return;
    }

    /*
     * Don't allow multicast fastswitching on interfaces with compression
     * enabled.
     */
    if (csb->set_to_default)
	csb->sense = TRUE;
    if (csb->sense && (idb->hwptr->comp_db || idb->hwptr->ppp_compression)) {
	printf("\nIP multicast fastswitching not allowed on interfaces with compression enabled");
	return;
    }

    /*
     * Enable fastswitching only if the h/w interface
     * has unicast fastswitching turned on. 
     */
    if (csb->sense && (!idb->hwptr->ip_routecache)) {
	printf("\nIP unicast fastswitching has to be enabled on the physical interface");
	return;
    }
    /*
     * Checking time is over...  
     * Enable or disable multicast fast switching
     *
     * Signals periodic process to do checking.
     */
    ip_mroute_fs_change = (idb->ip_multicast_fs ^ csb->sense);

    /*
     * Set sense from command.
     */
    idb->ip_multicast_fs = csb->sense;
}

/*
 * ipmulticast_fs_setup_interface
 * 
 * Disable multicast fastswitching on all swidbs, when unicast
 * fastswitching is turned off on the hwidb.
 */
void ipmulticast_fs_setup_interface (hwidbtype *hwidb)
{
    idbtype *idb;
    
    if (!hwidb->ip_routecache) {
	FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	    idb->ip_multicast_fs = FALSE;
	}
	ip_mroute_fs_change = TRUE;
    }
}

/*
 * ip_show_mroute_cache
 *
 * Display the IP multicast routing fastswitching cache. The fastswitching
 * cache and the IP multicast routing table are one table. This function
 * displays the mrouting cache relevant information.
 *
 * show ip mcache [<group>]
 */
void ip_show_mcache (parseinfo *csb)
{

    gdbtype    *gdb, *next;
    mdbtype    *mdb;
    ipaddrtype group, source;
    int        i;
    rnhtype    *head;

    gdb = NULL;
    mdb = NULL;
    group = GETOBJ(paddr,1)->ip_addr;
    source = GETOBJ(paddr,2)->ip_addr;

    /*
     * Display single group's cache entries.
     */
    if (group) {
	gdb = ip_get_gdb(group);
	if (!gdb) {
	    printf("\nGroup %i not found", group);
	    return;
	}

	/*
	 * Display single (S,G).
	 */
	if (source) {
	    mdb = ip_get_best_mdb(gdb, source);
	    if (!mdb) {
		printf("\nSource %i not found", source);
		return;
	    }
	}
    }

    automore_enable("IP Multicast Fast-Switching Cache");
    automore_header(NULL);

    if (gdb) {
	if (mdb) {
	    ip_display_mcache_entry(mdb, gdb);
	} else {
	    mem_lock(gdb);
            if (gdb->radix_head)  {
		head = gdb->radix_head;
		mem_lock(head);
		rn_walktree_blocking(gdb->radix_head->rnh_treetop, 
				     ip_display_mcache, gdb);
		free(head);
	    }
	    free(gdb);
	}
    } else {
    
	/*
	 * Display all entries for all groups.
	 */
	for (i = 0; i < NETHASHLEN; i++) {
	    for (gdb = ip_mroute_cache[i]; gdb; gdb = next) {
		mem_lock(gdb);
		if (IS_SPARSE_GROUP(gdb)) {
		    ip_display_mcache_entry(&gdb->mdb, gdb);
		}
		if (gdb->radix_head)  {
		    head = gdb->radix_head;
		    mem_lock(head);
		    rn_walktree_blocking(gdb->radix_head->rnh_treetop,
					 ip_display_mcache, gdb);
		    free(head);
		}
		next = gdb->next;
		free(gdb);
		if (automore_quit()) break;
	    }
	    if (automore_quit()) break;
	}
    }
    automore_disable();
}

/*
 * ip_display_mcache
 *
 * Called from the radix walker function for each (S,G).
 */
int ip_display_mcache (rntype *node, va_list args)
{

    mdbtype *mdb;
    gdbtype *gdb;

    if (automore_quit()) return(1);

    mdb = (mdbtype *) node;
    gdb = va_arg(args, gdbtype *);

    if (mdb != &gdb->mdb) mem_lock(mdb);
    ip_display_mcache_entry(mdb, gdb);
    if (mdb != &gdb->mdb) free(mdb);
    return(0);
}

/*
 * ip_display_mcache_entry
 *
 * Display a single multicast cache entry.
 */
void ip_display_mcache_entry (mdbtype *mdb, gdbtype *gdb)
{

    midbtype *midb, *next;
    idbtype  *idb;
    uchar    lastinput[20];
    int      i;

    /*
     * If not in cache, don't display.
     */
    if (!mdb->fastswitch) return;

    if (mdb == &gdb->mdb) {
	printf("\n(*, %i)", mdb->group);
    } else {
	printf("\n(%i/%d, %i)", ip_radix2addr(mdb->source), 
	       ip_bitsinmask(ip_radix2addr(mdb->mask)), mdb->group);
    }

    if (!TIMER_RUNNING(mdb->lastinput_fs)) {
	sprintf(lastinput, "never");
    } else {
	sprint_ticks_in_dhms(lastinput, ELAPSED_TIME(mdb->lastinput_fs));
    }

    /*
     * Print incoming interface and last used time.
     */
    printf(", %s, Last used: %s%s", 
	   (mdb->if_input) ? mdb->if_input->namestring : "Null", lastinput,
	   !(mdb->flags & FAST_FLAG) ? ", Semi-fast" : " ");

    /*
     * Print a line for each outgoing interface used for fastswitching.
     */
    for (midb = (midbtype *) mdb->if_outputQ.qhead; midb; midb = next) {
	if (midb->state == MIDB_PRUNE_STATE || !midb->mac_header) {
	    next = midb->next;
	    continue;
	}
	mem_lock(midb);
	mem_lock(midb->mac_header);
	printf("\n  %15s MAC Header: ", midb->idb->namestring);
	for (i = 0; i < midb->mac_length; i++) {
	    printf("%-02x", midb->mac_header[i]);
	}

	/*
	 * If a tunnel interface, print the next-hop interface after MAC
	 * header.
	 */
	if (midb->idb->hwptr->tunnel && midb->idb->hwptr->tunnel->header) {
	    idb = midb->idb->hwptr->tunnel->header->if_output;
	    if (idb) printf(" (%s)", idb->namestring);
	}
	next = midb->next;  
	free(midb->mac_header);
	free(midb);
    }
}

/*
 * ip_mdb_toggle_fs
 *
 * Set or clear mdb->fastswitch flag. Also, set last used timer to 0.
 */
void ip_mdb_toggle_fs (mdbtype *mdb, boolean flag, uchar *caller_string)
{


    if (MCACHE_DEBUG(mdb->group)) {
	buginf("\nMRC: Fast-switch flag for (%i/%d, %i), %s -> %s, caller %s",
	       ip_radix2addr(mdb->source), 
	       ip_bitsinmask(ip_radix2addr(mdb->mask)), mdb->group, 
	       (mdb->fastswitch) ? "on" : "off", (flag) ? "on" : "off",
	       caller_string);
    }

    if (!mdb->fastswitch) TIMER_STOP(mdb->lastinput_fs);
    mdb->fastswitch = flag;
}
