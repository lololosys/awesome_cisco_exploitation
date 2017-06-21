/* $Id: pim-autorp.c,v 3.1.2.3 1996/09/13 22:52:51 lwei Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/pim-autorp.c,v $
 *------------------------------------------------------------------
 * One line description here.
 *
 * April, 1996, Liming Wei
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pim-autorp.c,v $
 * Revision 3.1.2.3  1996/09/13  22:52:51  lwei
 * CSCdi69140:  Fix per code review comments to CSCdi68465
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/09/11  05:00:54  snyder
 * CSCdi68758:  spellink errors
 *              ourselve -> ourselves
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/09/10  20:58:37  lwei
 * CSCdi68465:  RP information is too difficult to change in last-hop
 * routers
 * Branch: California_branch
 *
 * Revision 3.1  1996/09/10  20:53:12  lwei
 * Placeholder file
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "stdarg.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "../ipmulticast/msg_pim.c"
#include "config.h"
#include "access.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "../iprouting/route.h"
#include "../ip/ipaccess.h"
#include "../ip/ipaccess_private.h"
#include "../ipmulticast/ipmulticast_debug.h"
#include "../ipmulticast/igmp.h"
#include "../ipmulticast/mroute.h"
#include "../ipmulticast/pim.h"

/*
 * Storage.
 */
queuetype pim_announced_rp;    /* RP announcement queue for Auto-RP */
rnhtype *pim_autorp_cache;     /* RP-mapping radix tree head */
queuetype pim_learned_rp;      /* RPs learned from Auto-RP */
queuetype pim_rp_announce_filter; /* RP configured announcement filter list */

extern queuetype pim_rp_filter;

/*
 * PIM auto-rp group prefix parameter block.
 */
typedef struct pim_glistparm_ {
     ipaddrtype    grp_prefix;  /* The multicast address prefix */
     ipaddrtype    grp_mask;    /* The address mask */
     int           idx;	        /* Index in the hashlist for non-wild items */
     boolean       non_wild;    /* Whether the current address is non-wild */
     fasttype      *grp_acl;    /* The Group access-list */
     fastitem      *item;	/* The wildcarded address item */
     fasthash      *acc;        /* The 'fasthash' part */
     boolean	   do_default;  /* G-RP mapping is for all multicast address */
} pim_glistparm;

/*
 * pim_init_grp_prefix_parm
 *
 * Initialize the parm block.
 */
static void pim_init_grp_prefix_parm (pim_glistparm *grp_parm,
			      pim_rp_announce_type *rp_ann)
{
    grp_parm->grp_prefix = 0;
    grp_parm->grp_mask = 0;
    grp_parm->idx = 0;
    grp_parm->non_wild = TRUE;
    grp_parm->grp_acl = (fasttype *) (rp_ann->group_acl
	? rp_ann->group_acl->list
	: NULL);
    if (grp_parm->grp_acl) {
	grp_parm->item = (fastitem *) grp_parm->grp_acl->items;
	grp_parm->acc = (fasthash *) grp_parm->grp_acl->hashlist[0];
	grp_parm->do_default = FALSE;
    } else {
	grp_parm->non_wild = FALSE;
	grp_parm->item = NULL;
	grp_parm->acc = NULL;
	grp_parm->do_default = TRUE;
    }
}

/*
 * pim_next_group_prefix
 *
 * Cycle through the items pointed to by the acess list number for the
 * configured group prefixes
 */
static boolean pim_next_group_prefix (pim_glistparm *grp_parm)
{
    int idx;
    fasttype *grp_fast = grp_parm->grp_acl;
    fasthash *acc;
    fastitem *item;

    while (TRUE) {
	if (!grp_parm->non_wild && !grp_parm->item) {
	    if (grp_parm->do_default) {
		grp_parm->grp_prefix = IPMCAST_DEFAULT;
		grp_parm->grp_mask = DEFAULT_MULTICAST_MASK;   
		grp_parm->do_default = FALSE;
		return(TRUE);
	    }
	    return (FALSE);
	} else if (grp_parm->non_wild) {
	    /*
	     * Scan the hash table of single addresses
	     */
	    acc = grp_parm->acc;
            if (acc) {
		grp_parm->acc = acc->next;
		if (acc) {
		    if (acc->grant) {
			grp_parm->grp_prefix = acc->address;
			grp_parm->grp_mask = MASK_HOST;
			return(TRUE);
		    }
		    continue;
		}
	    } else if (++grp_parm->idx < NETHASHLEN) {
		idx = grp_parm->idx;
		grp_parm->acc = grp_fast->hashlist[idx];
		continue;
	    }
	    grp_parm->non_wild = FALSE;
	} else {
	    /*
	     * Scan the table of wildcarded entries
	     */
	    item = grp_parm->item;
	    if (!item)
		 return (FALSE);
	    grp_parm->item = item->next;
	    if (item->grant) {
		grp_parm->grp_mask = ~item->wildmask;
		grp_parm->grp_prefix = item->permitmask & grp_parm->grp_mask;
		return (TRUE);
	    }
	}
    }
}

/*
 * pim_send_rp_announce_packet
 *
 * Fill in the igmp header and send the announce packet
 */
static void pim_send_rp_announce_packet (paktype *pak, pim_rp_announce_type *rp_ann)
{
    int         size;
    struct udptype_ *udp;
    ipaddrtype      source;
    idbtype         *idb, *first_idb;
    paktype         *dup;
    iphdrtype    *ip;
    list_element *l_elt;
    
    idb = rp_ann->idb;
    size = pak->length;
    source = rp_ann->rp_address;

    udp = (struct udptype_ *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));

    udp->sport = PIM_RP_DISC;
    udp->dport = PIM_RP_DISC;
    udp->length = size - IPHEADERBYTES(NOPT);
    udp->checksum = 0;
    udp->checksum = CheckSum((uchar *)udp, udp->length, source,
			     CISCO_RP_ANNOUNCE, UDP_PROT);

    AUTORP_BUGINF("\nAuto-RP: Send RP-Announce packet, IP source %i, ttl %d",
		  source, rp_ann->ttl);

    /*
     * Send to IP. Duplicate the packet for all PIM enabled interfaces.
     */
    igmprouteparams.ttl = rp_ann->ttl;
    igmprouteparams.switching = FALSE;

    first_idb = NULL;
    FOR_ALL_IPMULTICAST_IDBS(idb, l_elt) {
	if (!idb->ip_enabled || !idb->pim_enabled) continue;

	/*
	 * Process multicast-threshold. If packet TTL is over threshold,
	 * send packet.
	 */
	if (rp_ann->ttl <= idb->ip_multicast_threshold) {
	    continue;
	}

	/*
	 * Check if an admin-scoped boundary is setup for the group
	 * CISCO-RP-ANNOUNCE.
	 */
	if (ipmulticast_test_boundary(idb, CISCO_RP_ANNOUNCE)) {
	    continue;
	}

	/*
	 * Remember the first idb, so we can send the original copy of
	 * packet to this idb later.
	 */
	if (!first_idb) {
	    first_idb = idb;
	    continue;
	}
	dup = pak_duplicate(pak);
	if (!dup) break;

	/*
	 * Send to the UDP socket
	 */
	ipwrite(dup, size, UDP_PROT, source, CISCO_RP_ANNOUNCE, idb,
	    NULL, &igmprouteparams);
	igmp_traffic.output_pim++;
    }

    /*
     * If we are the RP-mapping agent, make a copy of the packet to
     * be looped back locally.
     */
    dup = NULL;
    if (rp_agent_socket) {
	dup = pak_duplicate(pak);
    }
    if (!dup) {
	dup = pak;
	pak = NULL;
    }

    /*
     * Send packets if an idb is found.
     */
    if (first_idb) {
	ipwrite(dup, size, UDP_PROT, source, CISCO_RP_ANNOUNCE, first_idb,
		NULL, &igmprouteparams);
    } else {
	datagram_done(dup);
    }

    igmprouteparams.ttl = 1;
    igmprouteparams.switching = TRUE;

    /*
     * When acting as an RP-mapping agent, send a copy of the announce
     * packet to ourself. Set the ip source field so the displayed
     * debug message shows the correct source info.
     */
    if (pak) {
	ip = (iphdrtype *) ipheadstart(pak);
	ip->srcadr = source;
	pim_receive_autorp_packet(pak, udp, (pim_autorp_header *) udp->udpdata);
	datagram_done(pak);
    }
}

/*
 * pim_send_rp_announce
 *
 * Extract the group prefixes and masks from the configured group list
 *
 * ulong configured_group, the acl of the group list
 * ulong ttl, the ttl for RP-ANNOUNCE packets
 *
 * compose and send the CISCO-RP-ANNOUNCE packets.
 */
void pim_send_rp_announce (void)
{
    paktype *pak;
    pim_rp_announce_type *rp_ann;
    pim_glistparm grp_parm;
    pim_autorp_header *autorp;
    pim_autorp_block *rp_block;
    uchar	*grp_prefix;
    boolean	sent;
    int		mtu, init_len;


    /*
     * Go through the RP announcement chain and report each of them
     * in a separate RP announcement packet.
     */
    for (rp_ann = pim_announced_rp.qhead; rp_ann; rp_ann = rp_ann->next) {
	/*
	 * Initialize the parameter block for the group access list
	 */
	pim_init_grp_prefix_parm(&grp_parm, rp_ann);

        AUTORP_BUGINF("\nAuto-RP: Build RP-Announce packet for %i, PIM%s",
		      rp_ann->rp_address,
		      (pim_version & PIMV2) ? "v2 v1" : "v1");

	while (TRUE) {
	     /*
	      * Get a mtu sized buffer, and initialize the buffer;
	      */
	     mtu = rp_ann->idb->ip_mtu;
	     pak = getbuffer(mtu);
	     pak->length =  IPHEADERBYTES(NOPT) + UDPHEADERBYTES + 
			    sizeof(pim_autorp_header);
	     init_len = pak->length;
	     autorp = (pim_autorp_header *) (((char *) ipheadstart(pak)) +
		   IPHEADERBYTES(NOPT) + UDPHEADERBYTES);

	     autorp->version = PIM_AUTORP_V1;
	     autorp->type = PIM_RP_ANNOUNCE_TYPE;
	     autorp->rp_count = 1;
	     autorp->holdtime = rp_ann->holdtime;
	     autorp->reserved = 0;

	     rp_block = (pim_autorp_block *) autorp->data;
	     PUTLONG(rp_block->rp, rp_ann->rp_address);
	     rp_block->reserved_and_ver =
	         (pim_version == PIMV2) ? (PIMV1 | PIMV2) : PIMV1;
	     rp_block->grp_count = 0;
	     grp_prefix = rp_block->grp_prefix;
	     sent = FALSE;
   
	     /*
	      * Go through the group access list items and put the configured
	      * group prefixes in the packet.
	      */
	     while (pim_next_group_prefix(&grp_parm)) {
		rp_block->grp_count++;
		pak->length += sizeof(pim_autorp_block);

		/*
		 * Now fill in the encoded group address prefix: 
		 * I.e. mask length + group address prefix
		 */
		*grp_prefix++ = 0;
		*grp_prefix++ = ip_bitsinmask(grp_parm.grp_mask);
		PUTLONG(grp_prefix, grp_parm.grp_prefix);

		AUTORP_BUGINF("\nAuto-RP:  Build announce entry for (%i/%d)",
			      grp_parm.grp_prefix, *(grp_prefix - 1));

		grp_prefix += ADDRLEN_IP;
		if (pak->length + sizeof(pim_autorp_block) > mtu) {
		    pim_send_rp_announce_packet(pak, rp_ann);
		    sent = TRUE;
		    break;
	        }
	     }
	     /*
	     * Finished all items in the group access list
	     */
	    if (!grp_parm.non_wild && !grp_parm.item) {
		if (!sent && pak->length > init_len) {
		    pim_send_rp_announce_packet(pak, rp_ann);
		} else {
		   retbuffer(pak);
		}
		break;
	    }
	}
    }

    /*
     * Restart the timer, to send periodic RP-announcements
     */
    TIMER_START_JITTERED(pim_rp_announce_timer, PIM_RP_ANNOUNCE_INTVL,
			 PIM_JITTER_PCT);

}

/*
 * pim_get_prm
 *
 * Find the PIM Auto_RP mapping descriptor from the radix tree.
 * The keys are the group prefix and the group mask.
 * Perform a longest match lookup, if mask is 0. Otherwise try
 * to find the exact match.
 */
pim_rp_mapping *pim_get_prm (ipaddrtype group_prefix, ipaddrtype mask)
{
   pim_rp_mapping *prm;
   ipradixtype    address, radix_mask;
   ipaddrtype     pprefix, pmask;

   if (!pim_autorp_cache) return (NULL);

   ip_addr2radix(group_prefix, address);

   if (mask) {
	ip_addr2radix(mask, radix_mask);
	prm = (pim_rp_mapping *) rn_lookup(address, radix_mask,
					   pim_autorp_cache);
   } else {
	prm = (pim_rp_mapping *) rn_match(address, pim_autorp_cache);
   }

   /*
    * Verify the match.
    */
   if (prm) {
       pprefix = ip_radix2addr(prm->group_prefix);
       pmask = ip_radix2addr(prm->group_mask);
       if ((group_prefix & pmask) != pprefix) return (NULL);
   }

   return (prm);
}

/*
 * Add to PIM RP-mapping cache. Do arbitration when updating an entry
 * learned from another source. Returns TRUE if cache has changed.
 *
 * expire_time is in second.
 */
boolean pim_add_prm (ipaddrtype group_prefix, ipaddrtype mask,
		  ipaddrtype rp_address, ipaddrtype source,
		  ulong expire_time, boolean replace, uchar rp_ver)
{
    pim_rp_mapping *prm;
    leveltype	   level;
    rntype         *rn;
    ipaddrtype	   group, old_prefix, old_mask;
    boolean	   changed;

    if (!IPMULTICAST(group_prefix) || martian(rp_address, TRUE)) {
	AUTORP_BUGINF("\nAuto-RP: Illegal RP-mapping (%i/%d, %i) not added",
		       group_prefix, ip_bitsinmask(mask), rp_address);
	return(FALSE);
    }

    changed = FALSE;
    prm = pim_get_prm(group_prefix, mask);

    if (!prm) {
	prm = malloc(sizeof(pim_rp_mapping));
	if (!prm) return (FALSE);
	prm->rp_addr = rp_address;
	prm->info_source = source;
	GET_TIMESTAMP(prm->uptime);
	ip_addr2radix(group_prefix, prm->group_prefix);
	ip_addr2radix(mask, prm->group_mask);

	if (!pim_autorp_cache) {
	    if (!rn_inithead((void **) &pim_autorp_cache, IPRADIXDATAOFFSET)) {
		errmsg(&msgsym(RADIXINIT, AUTORP), 0);
		return(FALSE);
	    }
	}

	level = raise_interrupt_level(NETS_DISABLE);
	rn = rn_addroute(prm->group_prefix, prm->group_mask,
			 pim_autorp_cache, prm->radix_nodes);
	reset_interrupt_level(level);

	if (rn != (rntype *) prm) {
	    errmsg(&msgsym(RADIXINSERT, AUTORP),
		   ip_radix2addr(prm->group_prefix),
		   ip_bitsinmask(ip_radix2addr(prm->group_mask)),
		   prm->rp_addr,
		   prm, rn);
	    free(prm);
	    return(FALSE);
	}

	changed = TRUE;
	AUTORP_BUGINF("\nAuto-RP:  create");
    } else {

	/*
	 * An entry already exists, we replace the existing one when the
	 * new RP address is larger than what we already have, or when we
	 * are told to replace the existing entry.
	 */
	if (!replace && rp_address < prm->rp_addr)  return(FALSE);

	old_prefix = ip_radix2addr(prm->group_prefix);
	old_mask   = ip_radix2addr(prm->group_mask);

	if (rp_address == prm->rp_addr && group_prefix == old_prefix) {
	   AUTORP_BUGINF("\nAuto-RP:  update");
	   prm->info_source = source;
	} else {
	   prm->rp_addr = rp_address;
	   GET_TIMESTAMP(prm->uptime);

	   prm->info_source = source;

	   if (old_prefix != group_prefix || old_mask != mask) {
	       AUTORP_BUGINF("\nAuto-RP: Not exact match: trying to replace"
			     "(%i/%d) with (%i/%d)",
			     old_prefix, ip_bitsinmask(old_mask),
			     group_prefix, ip_bitsinmask(mask));
	   }

	   changed = TRUE;
	   AUTORP_BUGINF("\nAuto-RP:  replaced with");
	}
    }
    prm->rp_version = rp_ver;

    group = group_prefix & mask;
    if (expire_time && group != CISCO_RP_ANNOUNCE &&
	group != CISCO_RP_DISCOVERY) {
	TIMER_START_JITTERED(prm->expires, expire_time * ONESEC,
			     PRM_JITTER_PCT);
    } else {
	TIMER_STOP(prm->expires);
    }

    AUTORP_BUGINF(" (%i/%d, RP:%i)",
	       group_prefix, ip_bitsinmask(mask), prm->rp_addr);
    if (rp_ver) {
        AUTORP_BUGINF(", PIM%s%s", PIM_RP_IS_V2(rp_ver) ? "v2 " : "",
		      PIM_RP_IS_V1(rp_ver) ? "v1" : "");
    }

    if (changed) {
	pim_has_new_rp = TRUE;
	igmp_check_new_rp(prm->rp_addr, 0);
	errmsg(&msgsym(MAPPING, AUTORP), group_prefix, ip_bitsinmask(mask),
	       prm->rp_addr);
    }
    return(changed);
}

/*
 * pim_accept_rp_announce
 *
 * See if we should accept this announcement for an RP-to-group mapping.
 */
static boolean pim_accept_rp_announce (ipaddrtype rp_address, ipaddrtype group)
{
    
    pim_rp_afilter_type *filter;

    for (filter = pim_rp_announce_filter.qhead; filter; 
	 filter = filter->next) {
	if (nfast_check(filter->rp_acl, rp_address)) {
	    if (nfast_check(filter->group_acl, group)) return(FALSE);
	}
    }

    /*
     * Also automatically refuse advertisement for CISCO-RP-ANNOUNCE
     * and CISCO-RP-DISCOVERY.
     */
    if (group == CISCO_RP_ANNOUNCE || group == CISCO_RP_DISCOVERY) {
	return(FALSE);
    }
    return(TRUE);
}

/*
 * pim_receive_autorp_packet
 */
void pim_receive_autorp_packet (paktype *pak, udptype *udp,
				pim_autorp_header *autorp)
{
   uchar   *pkt;
   int	   length, rp_count, group_count;
   int     holdtime;
   pim_autorp_block    *rp_block;
   pim_encoded_group   *grp_encoded;
   ipaddrtype          gaddr, mask, source, rp_address;
   iphdrtype           *ip;
   pim_rp_mapping      *prm;
   boolean	       is_discovery, cache_changed;

   ip = (iphdrtype *) ipheadstart(pak);
   source = ip->srcadr;
   is_discovery = FALSE;
   cache_changed = FALSE;

   /*
    * This recognizes version 1 Auto_RP messages.
    */
   if (autorp->version != PIM_AUTORP_V1) {
	AUTORP_BUGINF("\nAuto-RP: version (%d) unknown from (%i)",
		       autorp->version, ip->srcadr);
	return;
   }

   if (autorp->type != PIM_RP_ANNOUNCE_TYPE &&
       autorp->type != PIM_RP_DISCOVERY_TYPE) {
	AUTORP_BUGINF("\nAuto-RP: Type (%d) unknown from (%i)",
		       autorp->type, ip->srcadr);
	return;
   }

   if (autorp->type == PIM_RP_DISCOVERY_TYPE) {
       is_discovery = TRUE;
   }

   AUTORP_BUGINF("\nAuto-RP: Received RP-%s",
		 is_discovery ? "discovery," : "announce,");

   /*
    * Make sure that the socket layer does not pass an RP announce
    * packet up when we are not an RP-mapping agent.
    */
   if (ip->dstadr == CISCO_RP_ANNOUNCE && !rp_agent_socket) {
	AUTORP_BUGINF(" ignored. We are not an RP-mapping agent");
	return;
   }

   /*
    * If the RP_Discovery packet was originated by ourself, or
    * by another RP-mapping agent while we are an RP-mapping agent ourself,
    * ignore it.
    */
   if (ip->dstadr == CISCO_RP_DISCOVERY) {
	if (ip_ouraddress(source)) {
	    AUTORP_BUGINF(" from ourselves (%i), ignored", source);
	    return;
	}
	if (rp_agent_socket) {
	    AUTORP_BUGINF(" from %i, ignored", source);
	    return;
	}
   }

   rp_block = (pim_autorp_block *) autorp->data;
   length = udp->length - UDPHEADERBYTES - sizeof(pim_autorp_header);

   rp_count = autorp->rp_count;
   holdtime = autorp->holdtime;
   pkt = NULL;

   AUTORP_BUGINF(" from %i, RP_cnt %d, holdtime %d secs", source, rp_count,
		   holdtime);

   /*
    * Parse the packet, search the radix tree, update/create new radix node
    * in tree.
    */
   while (rp_count-- > 0 && length >= 0) {
	length -= sizeof(pim_autorp_block) - PIM_ADDRTYPE_SIZE;
	rp_address = GETLONG(rp_block->rp);
	group_count = rp_block->grp_count;
	grp_encoded = (pim_encoded_group *) rp_block->grp_prefix;

	/*
	 * For each group prefix, search the RP-mapping cache,
	 * and do the arbitration.
	 */
	while (group_count-- > 0 && length >= sizeof(pim_encoded_group)) {
	     mask = ip_bitcount2mask(grp_encoded->mask_len);
	     gaddr = GETLONG(grp_encoded->address);

	     /*
	      * Check RP announce filter. If passed, add to database. 
	      * Otherwise, issue buginf() if enabled.
	      */	
	     if (is_discovery || pim_accept_rp_announce(rp_address, gaddr)) {

		 /*
		  * Only update the RP-mapping database when the holdtime
		  * in the received packet is none-zero.
		  * When holdtime in packet is zero, it really means the
		  * RP-mapping should be removed immediately.
		  */
		 if (holdtime) {
		     cache_changed |= pim_add_prm(gaddr, mask, rp_address,
			     source, holdtime, is_discovery,
			     AUTORP_GET_VER(rp_block->reserved_and_ver));
		 } else {
		     prm = pim_get_prm(gaddr, mask);
		     if (prm && prm->rp_addr == rp_address) {
			 pim_delete_prm(prm);
		     }
		 }
	     } else {
		 AUTORP_BUGINF("\nAuto-RP: Filtered %i/%d for RP %i", 
			       gaddr, grp_encoded->mask_len, rp_address);
	     }
	     pkt = ((char *) grp_encoded) + sizeof(pim_encoded_group);
	     grp_encoded = (pim_encoded_group *) pkt;
	     length -= sizeof(pim_encoded_group);
	}
	rp_block = (pim_autorp_block *) pkt;
   }

   /*
    * If the autorp cache is change on a mapping agent, we need
    * to send triggered updates.
    */
   if (cache_changed && !is_discovery)
	pim_send_rp_mapping();
}

/*
 * pim_receive_rp_announce
 */
void pim_receive_rp_announce (void)
{
   paktype *pak;
   udptype *udp;
   pim_autorp_header *autorp;

   while ((pak = read_ipsocket(rp_agent_socket, 0, 0, &udp, &autorp))) {
	pim_receive_autorp_packet(pak, udp, autorp);
	datagram_done(pak);
   }
}

/*
 * pim_init_rp_discovery_pak
 */
static boolean pim_init_rp_discovery_pak (paktype **pak)
{
    paktype *packet;
    pim_autorp_header *autorp;

    /*
     * No need to send RP discovery packet if don't have a cache.
     */
    if (!pim_autorp_cache || !pim_autorp_min_mtu) return(FALSE);

    /*
     * Allocate RP-mapping buffer.
     */
    packet = getbuffer(pim_autorp_min_mtu);
    if (!packet) return(FALSE);

    packet->length = IPHEADERBYTES(NOPT) + UDPHEADERBYTES
		     + sizeof(pim_autorp_header);
    autorp = (pim_autorp_header *) (ipheadstart(packet) + IPHEADERBYTES(NOPT)
		+ UDPHEADERBYTES);
    autorp->version = PIM_AUTORP_V1;
    autorp->type = PIM_RP_DISCOVERY_TYPE;
    autorp->rp_count = 0;
    autorp->holdtime = PIM_RP_DISCOVERY_HOLDTIME;
    autorp->reserved = 0;
    packet->dataptr = NULL;
    *pak = packet;

    return(TRUE);
}

/*
 * pim_autorp_send_discovery
 *
 * Extract the autorp blocks from the packet buffer chain and form
 * the autorp discovery packet. Send the packet to address
 * CISCO_RP_DISCOVERY
 */
static void pim_autorp_send_discovery (paktype *pak)
{
   paktype *pakbuf, *tmpbuf;
   udptype *udp;
   pim_autorp_header *autorp;
   char *pkt;
   idbtype *first_idb, *idb;
   ipaddrtype source;
   list_element *l_elt;
   
   udp = (udptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));
   autorp = (pim_autorp_header *) udp->udpdata;

   pakbuf = (paktype *) pak->dataptr;
   pkt = (char *) (((char *) ipheadstart(pak)) + pak->length);

   /*
    * Go through the packet buffer chain and copy each block into the first
    * packet buffer, to the area pointed by pkt.
    */
   while (pakbuf) {

	/*
	 * The total buffer size was checked before this function was called,
	 * the following condition should never become true. It is included
	 * just in case something very rare happens.
	 */
	if (pak->length + pakbuf->length > pim_autorp_min_mtu) {
	   paktype *tmp_buf;
	   AUTORP_BUGINF("\nAuto-RP: RP discovery packet buffer too large\n");

	   tmp_buf = (paktype *) pakbuf->dataptr;
	   retbuffer(pakbuf);
	   pakbuf = tmp_buf;
	   break;
	}

	bcopy(pakbuf->data_area, pkt, pakbuf->length);
	pak->length += pakbuf->length;
	autorp->rp_count++;

	pkt += pakbuf->length;

	tmpbuf = (paktype *) pakbuf->dataptr;
	pakbuf->dataptr = NULL;

	/*
	 * Don't need the buffer anymore.
	 */
	retbuffer(pakbuf);
	pakbuf = tmpbuf;
   }


   /*
    * If we have no RP-mappings to send, stop here.
    */
   if (!autorp->rp_count) {
      AUTORP_BUGINF(("\nAuto-RP: No RP mappings to send"));
      retbuffer(pak);
      return;
   }

   /*
    * Now fill in the UDP header.
    */
   udp->dport = PIM_RP_DISC;
   udp->sport = PIM_RP_DISC;
   udp->length = pak->length - IPHEADERBYTES(NOPT);

   AUTORP_BUGINF("\nAuto-RP: Send RP-discovery packet (%d RP entries)",
		  autorp->rp_count);

   /*
    * Send a copy of the RP discovery packet to each multicast capable
    * interface
    */
   first_idb = NULL;
   igmprouteparams.ttl = pim_rp_mapping_ttl;
   igmprouteparams.switching = FALSE;
   FOR_ALL_IPMULTICAST_IDBS(idb, l_elt) {
	paktype *dup;

	if (!idb->ip_enabled || !idb->pim_enabled) continue;

        /*
         * Process multicast-threshold. If packet TTL is over threshold,
         * send packet.
         */
        if (pim_rp_mapping_ttl <= idb->ip_multicast_threshold) {
            continue;
        }

	/*
	 * Check if an admin-scoped boundary is setup for the group
	 * CISCO-RP-DISCOVERY.
	 */
	if (ipmulticast_test_boundary(idb, CISCO_RP_DISCOVERY)) {
	    continue;
	}

	/*
	 * Remember the first idb, so we can send the original copy of
	 * packet to this idb later.
	 */
	if (!first_idb) {
	    first_idb = idb;
	    continue;
	}
	dup = pak_duplicate(pak);
	if (!dup) break;

	udp = (udptype *) (((char *) ipheadstart(dup)) + IPHEADERBYTES(NOPT));
	source = idb->ip_address;
	udp->checksum = 0;
        udp->checksum = CheckSum((uchar *)udp, udp->length, source,
				 CISCO_RP_DISCOVERY, UDP_PROT);

	/*
	 * Send to the UDP socket
	 */
	ipwrite(dup, dup->length, UDP_PROT, source, CISCO_RP_DISCOVERY, idb,
	    NULL, &igmprouteparams);
	igmp_traffic.output_pim++;
    }

    udp = (udptype *) (((char *) ipheadstart(pak)) + IPHEADERBYTES(NOPT));
    source = first_idb->ip_address;
    udp->checksum = 0;
    udp->checksum = CheckSum((uchar *)udp, udp->length, source,
			      CISCO_RP_DISCOVERY, UDP_PROT);

    /*
     * Send the RP Discovery packet to the well-known address.
     */
    if (first_idb) {
	ipwrite(pak, pak->length, UDP_PROT, source, CISCO_RP_DISCOVERY,
		first_idb, NULL, &igmprouteparams);
    } else {
	datagram_done(pak);
    }
    igmprouteparams.ttl = 1;
    igmprouteparams.switching = TRUE;
    igmp_traffic.output_pim++;
}

/*
 * pim_autorp_build_entry
 *
 * For each RP-mapping entry:
 *
 *    1) Find the buffer for the RP;
 *    2) Put in the RP-mapping entry in the autorp_block
 *
 * Once the sum of the lengths of all buffers exceeds the pim_autorp_min_mtu,
 * an RP discovery packet is composed and sent.
 *
 * Similar to pim_build_entry, this allows the scanning of the autorp
 * cache for a single pass.
 */
static int pim_autorp_build_entry (rntype *node, va_list args)
{
   pim_rp_mapping *prm;
   paktype        **pak, *packet, *pakbuf, *prevbuf;
   int            *total_length, mask_len;
   pim_autorp_header *autorp_header;
   pim_autorp_block  *block;
   pim_encoded_group *grp_prefix;
   ipaddrtype        grp_addr, grp_mask, *rp_addr;

   prevbuf = NULL;

   prm = (pim_rp_mapping *) node;

   /*
    * Don't advertise semi-static entries.
    */
   if (!TIMER_RUNNING(prm->expires)) return(0);

   pak = va_arg(args, paktype **);
   total_length = va_arg(args, int *);

   packet = *pak;
   grp_addr = ip_radix2addr(prm->group_prefix);
   grp_mask = ip_radix2addr(prm->group_mask);
   mask_len = ip_bitsinmask(grp_mask);

   autorp_header = (pim_autorp_header *) (ipheadstart(packet) +
	        IPHEADERBYTES(NOPT) + UDPHEADERBYTES);
   block = (pim_autorp_block *) autorp_header->data;

   AUTORP_BUGINF("\nAuto-RP:  Build mapping (%i/%d, RP:%i)",
		  grp_addr, mask_len, prm->rp_addr);
   if (prm->rp_version) {
       AUTORP_BUGINF(", PIM%s%s", PIM_RP_IS_V2(prm->rp_version) ? "v2 " : "",
		     PIM_RP_IS_V1(prm->rp_version) ? "v1" : "");
   }
   /*
    * This is the first RP-mapping entry.
    */
   if (!autorp_header->rp_count) {
	autorp_header->rp_count++;
	packet->length = IPHEADERBYTES(NOPT) + UDPHEADERBYTES +
			 sizeof(pim_autorp_header) +
			 sizeof(pim_autorp_block);
	packet->dataptr = NULL;

        /*
         * Put in the RP address
         */
	PUTLONG(block->rp, prm->rp_addr);
	block->reserved_and_ver = prm->rp_version;
	block->grp_count = 1;

	/*
	 * Put in the group address prefix, in encoded group address format.
	 */
        grp_prefix = (pim_encoded_group *) block->grp_prefix;
        grp_prefix->reserved = 0;
	grp_prefix->mask_len = mask_len;
	PUTLONG(grp_prefix->address, grp_addr);

	AUTORP_BUGINF(",");

	*total_length = packet->length;
	return(0);
   }

   /*
    * Search the packet buffer chain, for the RP.
    */
   rp_addr = (ipaddrtype *) block->rp;
   pakbuf =  packet;
   while (pakbuf && *rp_addr != prm->rp_addr) {
	prevbuf = pakbuf;
	pakbuf = (paktype *) pakbuf->dataptr;
	if (pakbuf) {
	    block = (pim_autorp_block *) pakbuf->data_area;
	    rp_addr = (ipaddrtype *) block->rp;
	}
   }

   /*
    * If not found in existing buffer chain, create new buffer,
    * put in chain, initialize the fixed part of the autorp block.
    */
   if (!pakbuf) {
       pakbuf = getbuffer(pim_autorp_min_mtu);
       if (!pakbuf) return(1);
       prevbuf->dataptr = (char *) pakbuf;
       pakbuf->dataptr = NULL;

       /*
        * Other buffers except the first one don't have packet headers
        */
	pakbuf->length = sizeof(pim_autorp_block) - PIM_ADDRTYPE_SIZE;
	*total_length += pakbuf->length;

	block = (pim_autorp_block *) pakbuf->data_area;
	block->reserved_and_ver = prm->rp_version;
	rp_addr = (ipaddrtype *) block->rp;
	*rp_addr = prm->rp_addr;
	block->grp_count = 0;

   }

   AUTORP_BUGINF(".");

   pakbuf->length += PIM_ADDRTYPE_SIZE;
   *total_length += PIM_ADDRTYPE_SIZE;

   /*
    *  Put the group prefix in encoded group address format.
    */
   grp_prefix = (pim_encoded_group *) (((char *)
		 block->grp_prefix) + (block->grp_count * PIM_ADDRTYPE_SIZE));
   grp_prefix->reserved = 0;
   grp_prefix->mask_len = mask_len;
   PUTLONG(grp_prefix->address, grp_addr);
   block->grp_count++;
   if (*total_length > pim_autorp_min_mtu) {
	pim_autorp_send_discovery(*pak);
	pim_init_rp_discovery_pak(pak);
   }
   return(0);
}

/*
 * pim_send_rp_mapping
 *
 * Send PIM RP-mapping packets
 */
void pim_send_rp_mapping (void)
{
   paktype *pak;
   int total_length;

   pak = NULL;
   total_length = 0;

   /*
    * Initialize the first packet buffer. 
    */
   if (!pim_init_rp_discovery_pak(&pak)) return;

   AUTORP_BUGINF("\nAuto-RP: Build RP-Discovery packet");

   /*
    * Walk through the RP-mapping cache and compose the RP-mapping blocks.
    * in the buffer chain. The autorp blocks for different RPs are stored
    * in separate buffers, off of the pak->dataptr chain. This way we
    * only need to go through the autorp radix tree once.
    */
   rn_walktree(pim_autorp_cache->rnh_treetop, pim_autorp_build_entry,
	       &pak, &total_length);

   if (pak) {
      pim_autorp_send_discovery(pak);
   }

   TIMER_START_JITTERED(pim_rp_mapping_timer, PIM_RP_MAPPING_INTVL,
			 PIM_JITTER_PCT);
}

/*
 * pim_receive_rp_discovery
 */
void pim_receive_rp_discovery (void)
{
   paktype *pak;
   udptype *udp;
   pim_autorp_header *autorp;

   while ((pak = read_ipsocket(pim_rp_discovery, 0, 0, &udp, &autorp))) {
	pim_receive_autorp_packet(pak, udp, autorp);
	datagram_done(pak);
   }
}
/*
 * pim_get_best_rp_mapping
 *
 * Find the longest match on the group prefix.
 */
pim_rp_mapping *pim_get_best_rp_mapping (ipaddrtype group)
{
   pim_rp_mapping *prm;
   ipradixtype    address;
   ipaddrtype     addr, mask;

   if (!pim_autorp_cache) return(NULL);

   /*
    * Convert the address into radixtype and search the radix tree
    */
   ip_addr2radix(group, address);
   prm = (pim_rp_mapping *) rn_match(address, pim_autorp_cache);

   if (!prm) return(NULL);

   /*
    * Verify that the entry found matches the group prefix
    */
   addr = ip_radix2addr(prm->group_prefix);
   mask = ip_radix2addr(prm->group_mask);
   if ((group & mask) != addr) return(NULL);

   return(prm);
}

/*
 * pim_delete_prm
 */
void pim_delete_prm (pim_rp_mapping *prm)
{
   rntype *rn;
   pim_rptype *rp;

   SAVE_CALLER();
   rn = rn_delete(prm->group_prefix, prm->group_mask, pim_autorp_cache);

   if (rn == (rntype *) prm) {

       /*
	* Search the learned_rp for state created based on this prm.
	*/
       for (rp = (pim_rptype *) pim_learned_rp.qhead; rp; rp = rp->next) {
	    if (rp->prm == prm) break;
       };
       if (rp && rp->prm == prm) {

	   /*
	    * Don't need the rp state any more now that the learned
	    * RP-mapping has already timed out. Also clear the multicast
	    * routing entry using this rp.
	    */
	   ip_clear_mroute_cache(rp->rp_address, NULL);
	   unqueue(&pim_learned_rp, rp);
	   free(rp);
       }
       free(prm);
       AUTORP_BUGINF("deleted.");
   } else {
       AUTORP_BUGINF("\nAuto-RP: Couldn't delete mapping (%i/%d, %i)"
		     "(expected %#x, got %#x), PC 0x%x",
	        ip_radix2addr(prm->group_prefix),
		ip_bitsinmask(ip_radix2addr(prm->group_mask)), prm->rp_addr,
		prm, rn, caller());
   }
}

/*
 * ip_age_one_prm
 *
 * Age one RP-mapping entry
 */
static int ip_age_one_prm (rntype *node, va_list args)
{
   pim_rp_mapping *prm;
   ipaddrtype group, mask;

   prm = (pim_rp_mapping *) node;

   if (TIMER_RUNNING_AND_AWAKE(prm->expires)) {
	group = ip_radix2addr(prm->group_prefix);
	mask = ip_radix2addr(prm->group_mask);

	AUTORP_BUGINF("\nAuto-RP: mapping (%i/%d, RP:%i) expired, ",
			group, ip_bitsinmask(mask), prm->rp_addr);

	/*
	 * Semi-static RP-mapping entry should never be timed out.
	 * they can only be removed on-demand.
	 */
	if ((group & mask) == CISCO_RP_ANNOUNCE ||
	    (group & mask) == CISCO_RP_DISCOVERY) {
	    errmsg(&msgsym(PRM_DEL, AUTORP), prm->rp_addr,
		   ip_bitsinmask(mask), group);
	}
	pim_delete_prm(prm);
   }
   return(0);
}

/*
 * ip_pim_autorp_ager
 *
 * Go through the learned RP mapping cache and age them one by one.
 */
void ip_pim_autorp_ager (void)
{

   if (!pim_autorp_cache) return;

   if (pim_autorp_cache->rnh_treetop) {
       rn_walktree(pim_autorp_cache->rnh_treetop, ip_age_one_prm);
   }
}

/*
 * pim_update_gdb_rp
 *
 * The RP information has changed.  Update the RP information in gdb.
 * This function puts the supplied RP in gdb->rp.  Note, this routine
 * was originally written to support the two well-known Auto-RP groups.
 *
 * When no RP exists for the group, this routine turns the group into
 * dense-mode.
 */
static void pim_update_gdb_rp (gdbtype *gdb, ipaddrtype rp)
{
    if (!gdb) return;

    if (rp) {
	if (gdb->rp.address != rp) {
	    gdb->rp.address = 0;
	    pim_store_rp_info(gdb, rp, RP_UP, TRUE, FALSE);
	}
    } else {

	/*
	 * No RP exists for the group. Assume dense mode operation.
	 */
	pim_clear_rp_state(gdb);
	gdb->mdb.flags &=~SPARSE_FLAG;
    }
}

/*
 * pim_set_rp_ann_holdtime
 *
 * Set holdtime and send out a PIM RP Announce message
 */
void pim_set_rp_ann_holdtime (pim_rp_announce_type *rp_ann, ulong holdtime)
{
    rp_ann->holdtime = holdtime;
    pim_send_rp_announce();
}

/*
 * pim_check_rp_lists
 *
 * The access-list "list" is changed. Check if it is related to the
 * RP configurations.
 */
void pim_check_rp_lists(acl_headertype *acl)
{
   pim_rptype *rp;
   pim_rp_mapping *prm;
   boolean rp_ann_deleted, rp_disc_deleted;

   rp_ann_deleted = FALSE;
   rp_disc_deleted = FALSE;

   /*
    * Check configured RPs, if the list was used by a configured RP
    */
   for (rp = (pim_rptype *) pim_configured_rp.qhead; rp; rp = rp->next) {
	if (rp->group_acl == acl) {

	    /*
	     * This acl has been used by an configured RP, which might
	     * have been used as the RP for the two Auto-RP well-known
	     * groups. We need to update the RP mappings.
	     */
	    prm = pim_get_prm(CISCO_RP_ANNOUNCE, MASK_HOST);
	    if (prm) {
		pim_delete_prm(prm);
		rp_ann_deleted = TRUE;
	    }
	    prm = pim_get_prm(CISCO_RP_DISCOVERY, MASK_HOST);
	    if (prm) {
		pim_delete_prm(prm);
		rp_disc_deleted = TRUE;
	    }
	}
   }

   pim_autorp_reset_known_groups(rp_ann_deleted, rp_disc_deleted);
}

/*
 * pim_autorp_reset_known_groups
 *
 * The semi-static entries for the two well-known groups were deleted
 * Re-establish the correct semi-static RP mappings for the
 * two well-known groups.
 */
void pim_autorp_reset_known_groups(boolean rp_ann_deleted,
				   boolean rp_disc_deleted)
{
   ipaddrtype rp_ann, rp_disc;
   gdbtype *gdb;
   pim_rptype *rp;
   
   rp_ann = IPADDR_ZERO;
   rp_disc = IPADDR_ZERO;

   /*
    * Search the configured RP queue and see if there are other
    * RPs that can serve these two groups.
    */
   for (rp = (pim_rptype *) pim_configured_rp.qhead; rp; rp = rp->next) {
	if (rp_ann_deleted && nfast_check(rp->group_acl, CISCO_RP_ANNOUNCE)) {
	   pim_add_prm(CISCO_RP_ANNOUNCE, MASK_HOST, rp->rp_address, 0, 0,
		       0, 0);
	   rp_ann = rp->rp_address;
	}
	if (rp_disc_deleted && nfast_check(rp->group_acl, CISCO_RP_DISCOVERY)) {
	   pim_add_prm(CISCO_RP_DISCOVERY, MASK_HOST, rp->rp_address, 0,
		       0, 0, 0);
	   rp_disc = rp->rp_address;
	}
   }
 
   /*
    * Find the group entries for the Auto-RP well-known groups, and
    * update the RP state.
    */
   gdb = ip_get_gdb(CISCO_RP_ANNOUNCE);
   pim_update_gdb_rp(gdb, rp_ann);
   gdb = ip_get_gdb(CISCO_RP_DISCOVERY);
   pim_update_gdb_rp(gdb, rp_disc);
}

/*
 * pim_enable_rp_discovery
 *
 * Enable the interface to receive RP-discovery packets.
 * Returns TRUE on sucess.
 */
boolean pim_enable_rp_discovery(idbtype *idb)
{
    addrtype laddr;

    igmp_add_group(CISCO_RP_DISCOVERY, idb, 0, 
		   IGMP_LOCAL_FLAG | IGMP_USER_FLAG, 0);

    if (!pim_rp_discovery) {
	laddr.type = ADDR_IP;
	laddr.ip_addr = CISCO_RP_DISCOVERY;
	pim_rp_discovery = open_ipsocket(UDP_PROT, &laddr, NULL, PIM_RP_DISC,
					 0, SOCK_ANYSRC);
	if (!pim_rp_discovery) {
	   AUTORP_BUGINF("\nAuto-RP: Can't open RP_discovery UDP socket");
	   return (FALSE);
	}
    }
    return(TRUE);
}
/*
 * ip_clear_one_autorp_entry
 *
 * clear a single Auto-RP cache entry in the radix tree
 */
static int ip_clear_one_autorp_entry(rntype *node, va_list args)
{
    pim_rp_mapping *prm;
    ipaddrtype     rp;

    rp = va_arg(args, ipaddrtype);

    prm = (pim_rp_mapping *) node;

    if (!rp || rp == prm->rp_addr) {
	pim_delete_prm(prm);
    }
    return(0);
}

/*
 * ip_clear_autorp_cache
 *
 * clear the entire autorp cache
 */
void ip_clear_autorp_cache(void)
{
   pim_rptype *rp;

   if (!pim_autorp_cache) return;

   /*
    * Zero as last parameter indicates unconditional deletion
    */
   rn_walktree(pim_autorp_cache->rnh_treetop,
	       ip_clear_one_autorp_entry, 0);

   free(pim_autorp_cache);
   pim_autorp_cache = NULL;

   /*
    * Also need to release the pim_learned_rp queue.
    */
   rp = (pim_rptype *) dequeue(&pim_learned_rp);
   for (; rp; rp = (pim_rptype *) dequeue(&pim_learned_rp)) {
	free(rp);
   }
}

/*
 * clear_autorp_command
 *
 * Selectively (given rp address) or comprehensively (given "*")
 * removes an rp from the AutoRP mapping cache. This command
 * has no effect on statically configured RPs.
 * 
 */
void clear_autorp_command (parseinfo *csb)
{

    ipaddrtype rp;

    rp = GETOBJ(paddr,1)->ip_addr;

    if (!pim_autorp_cache) return;

    /*
     * Process 'clear ip pim rp <addr>'
     */
    if (rp) {
	rn_walktree(pim_autorp_cache->rnh_treetop,
		    ip_clear_one_autorp_entry, rp);
    } else {
	ip_clear_autorp_cache();
    }

    return;
}

    
