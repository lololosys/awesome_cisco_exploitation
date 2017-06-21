/* $Id: ipnat.c,v 3.1.6.20 1996/09/04 17:16:35 ahh Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipnat.c,v $
 *------------------------------------------------------------------
 * IP Network Address Translation
 *
 * October 1995, Andy Heffernan
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipnat.c,v $
 * Revision 3.1.6.20  1996/09/04  17:16:35  ahh
 * CSCdi67885:  NAT: sub-interfaces not supported properly
 * Allow interface commands to work in sub-interface configuration
 * mode, allow translation to be enabled on some sub-interfaces
 * but not others and still have the fast-path work.
 * Branch: California_branch
 *
 * Revision 3.1.6.19  1996/08/30  00:21:49  ahh
 * CSCdi67637:  NAT: removing non-existing memory dereferences NULL
 * Don't do that.
 * Branch: California_branch
 *
 * Revision 3.1.6.18  1996/08/28  22:57:47  ahh
 * CSCdi65898:  NAT: rsh not working when overloading
 * rsh wants rshd to connect back to it for stderr, so need to allow this
 * flow from a random privileged port in.
 * Branch: California_branch
 *
 * Revision 3.1.6.17  1996/08/22  00:27:49  ahh
 * CSCdi66753:  NAT: portlists not reaped when a mapping is deleted
 * As ye sow, so shall ye reap (or something like that).
 * Branch: California_branch
 *
 * Revision 3.1.6.16  1996/08/20  20:58:30  ahh
 * CSCdi66593:  NAT: can only have one mapping with a route-map
 * Fix the sorting callback to sort the right name strings.
 * Branch: California_branch
 *
 * Revision 3.1.6.15  1996/08/20  18:55:32  ahh
 * CSCdi66560:  NAT: cannot clear statistics
 * Branch: California_branch
 *
 * Revision 3.1.6.14  1996/08/14  05:37:21  ahh
 * CSCdi65971:  NAT: need static translations with ports
 * Allow user to configure protocol and ports in a static translation.
 * Branch: California_branch
 *
 * Revision 3.1.6.13  1996/08/13  21:27:35  ahh
 * CSCdi65510:  NAT: tftp does not work when overloading global addresses
 * Deal with the server sending packets back from a new port number.
 * Branch: California_branch
 *
 * Revision 3.1.6.12  1996/07/30  23:42:58  ahh
 * CSCdi64604:  NAT: need support for multi-homing
 * Allow the use of route maps for choosing a global address pool.
 * Branch: California_branch
 *
 * Revision 3.1.6.11  1996/06/21  00:22:23  ahh
 * CSCdi60792:  NAT: traceroute not working with overload
 * Extract flow information out of the embedded transport headers.
 * Branch: California_branch
 *
 * Revision 3.1.6.10  1996/06/18  00:31:52  ahh
 * CSCdi60565:  crash in show ip nat translations verbose
 * Don't try to dump the tables if they ain't there (boing).
 * Branch: California_branch
 *
 * Revision 3.1.6.9  1996/06/06  18:05:24  ahh
 * CSCdi59725:  ip nat static addresses fail to write out properly
 * Put the oddball translations on the right list.
 * Branch: California_branch
 *
 * Revision 3.1.6.8  1996/06/04  05:10:07  kao
 * CSCdi57759:  VIP2 system exception & DBUS-3-DBUSINTERR error when doing
 * eoir.
 * Remove the extern definition incidentally inserted in this bug fix.
 * Branch: California_branch
 *
 * Revision 3.1.6.7  1996/06/04  05:04:05  kao
 * CSCdi57759:  VIP2 system exception & DBUS-3-DBUSINTERR error when doing
 * eoir.
 * move all DFS/IPC related variables and routines to rsp_ipc_cbus.c
 * Branch: California_branch
 *
 * Revision 3.1.6.6  1996/06/03  23:23:11  ahh
 * CSCdi59119:  IP: NAT should support named access-lists
 * And extended access-lists and non-contiguous address pools.
 * Branch: California_branch
 *
 * Revision 3.1.6.5  1996/05/21  09:51:31  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.6.4  1996/05/13  23:20:58  ahh
 * CSCdi56426:  NAT: EFT bugs and feedback
 * Fix hash algorithm, fix address block coalescing, reduce up-front
 * memory allocation, drop untranslatable packets, clean up debugs.
 * Branch: California_branch
 *
 * Revision 3.1.6.3  1996/04/24  16:20:21  ahh
 * CSCdi55543:  NAT: modularity problems
 * Fix build failures (Doh!)
 * Branch: California_branch
 *
 * Revision 3.1.6.2  1996/04/24  00:06:24  ahh
 * CSCdi55483:  NAT: code review comments, various bug fixes
 * Fast-path fixup, cruft removal.
 * Branch: California_branch
 *
 * Revision 3.1.6.1  1996/04/16  18:53:19  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/07  05:59:03  ahh
 * Placeholder...Placeholder...
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "globs.h"
#include "types.h"
#include <ciscolib.h>
#include <stdlib.h>
#include "interface_private.h"
#include "ip.h"
#include "../ui/debug.h"
#include "ip_debug.h"
#include "ip_registry.h"
#include "logger.h"
#include "../os/msg_sched.c"            /* Not a typo, see logger.h */
#include "config.h"
#include "parser.h"
#include "../parser/macros.h"
#include "../parser/parser_actions.h"
#include "parser_defs_ipnat.h"
#include "../parser/parser_defs_access.h"
#include "../parser/parser_defs_parser.h"
#include "subsys.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "mgd_timers.h"
#include "access.h"		/* for MAXFASTACC */
#include "ipaccess.h"
#include "ipaccess_private.h"
#include "ipaccess2.h"
#include "../iprouting/route.h"
#include "fastswitch_registry.h"
#include "ipfast.h"
#include "ipnat_private.h"
#include "ipnat_pool.h"

mgd_timer ipnat_timer_master;
mgd_timer ipnat_entry_timer_master, ipnat_door_timer_master,
    ipnat_fragment_timer_master;
list_header ipnat_mappings[IPNAT_DOMAINS][IPNAT_LOOKUPS];
ipnat_statistics ipnat_stats;
pid_t ipnat_ager_pid;
int ipnat_max_entries;


/*
 * ipnat_get_ports
 *
 * Extract port and protocol information out of a packet.  For ICMP
 * packets with embedded headers, we need to reach into those headers
 * for the port information, because it will tell us which flow the
 * message is for.  If we're overloading, just the destination address
 * from the ICMP packet isn't enough to figure out which global->local
 * translation to use.
 */

void
ipnat_get_ports (iphdrtype *ip, ushort *sport, ushort *dport, uchar *proto,
		 ipaddrtype *srcadr, boolean *can_overload)
{
    iphdrtype *ip_embed;
    tcptype *tcp;
    udptype *udp;
    icmptype *icmp;
    ipnat_fragment *fragment;

    *proto = ip->prot;
    *srcadr = ip->srcadr;

    if (ip->fo) {
	/*
	 * See if we have saved info about this fragment.
	 */
	if (ipnat_debug)
	    buginf("\nNAT: fo %d, looking for fragment %i %i %d %d",
		   ip->fo, ip->srcadr, ip->dstadr, ip->id, ip->prot);
	fragment = ipnat_find_fragment(ip->srcadr, ip->dstadr, ip->id,
				       ip->prot);
	if (fragment) {
	    if (ipnat_debug)
		buginf("\nNAT: found fragment %i %i %d %d -- %d %d",
		       ip->srcadr, ip->dstadr, ip->id, ip->prot,
		       fragment->sport, fragment->dport);
	    *sport = fragment->sport;
	    *dport = fragment->dport;
	    *can_overload = TRUE;
	} else {
	    /* Not a fragment we know about, so we know nothing. */
	    *sport = *dport = 0;
	    *can_overload = FALSE;
	}
    } else {
	switch (ip->prot) {
	case TCP_PROT:
	    tcp = (tcptype *) ipdatastart_iph(ip);
	    *sport = tcp->sourceport;
	    *dport = tcp->destinationport;
	    *can_overload = TRUE;
	    break;
	case UDP_PROT:
	    udp = (udptype *) ipdatastart_iph(ip);
	    *sport = udp->sport;
	    *dport = udp->dport;
	    *can_overload = TRUE;
	    break;
	case ICMP_PROT:
	    icmp = (icmptype *) ipdatastart_iph(ip);
	    switch (icmp->type) {
	    case ICMP_UNREACHABLE:
	    case ICMP_QUENCH:
	    case ICMP_REDIRECT:
	    case ICMP_TIMEEXCEEDED:
	    case ICMP_PARAMETER:
		ip_embed = (iphdrtype *) icmp->data;
		*srcadr = ip_embed->dstadr;
		switch (ip_embed->prot) {
		case TCP_PROT:
		    tcp = (tcptype *) ipdatastart_iph(ip_embed);
		    *sport = tcp->destinationport;
		    *dport = tcp->sourceport;
		    *proto = ip_embed->prot;
		    break;
		case UDP_PROT:
		    udp = (udptype *) ipdatastart_iph(ip_embed);
		    *sport = udp->dport;
		    *dport = udp->sport;
		    *proto = ip_embed->prot;
		    break;
		case ICMP_PROT:
		    icmp = (icmptype *) ipdatastart_iph(ip_embed);
		    switch (icmp->type) {
		    case ICMP_ECHO:
		    case ICMP_ECHOREPLY:
			*sport = *dport = icmp->identifier;
			break;
		    default:
			*sport = *dport = 0;
			break;
		    }
		    break;
		default:
		    *sport = *dport = 0;
		    break;
		}
		*can_overload = FALSE;
		break;
	    case ICMP_ECHO:
	    case ICMP_ECHOREPLY:
		*sport = *dport = icmp->identifier;
		*can_overload = TRUE;
		break;
	    default:
		*sport = *dport = 0;
		*can_overload = FALSE;
		break;
	    }
	    break;
	default:
	    *sport = *dport = 0;
	    *can_overload = FALSE;
	    break;
	}
    }
}

/*
 * ipnat_save_fragment0
 *
 * Save information about this fragment so that future fragments
 * can be translated correctly.
 */

static void
ipnat_save_fragment0 (iphdrtype *ip, ushort sport, ushort dport)
{
    ipnat_fragment *fragment;

    if (ipnat_debug)
	buginf("\nNAT: creating fragment %i %i %d %d -- %d %d",
	       ip->srcadr, ip->dstadr, ip->id, ip->prot, sport, dport);
    fragment = ipnat_create_fragment(ip->srcadr, ip->dstadr, ip->id,
				     ip->prot, sport, dport);
    if (fragment)
	ipnat_add_fragment(fragment);
}

/*
 * ipnat_translate_before_routing
 *
 * Do a global->local translation on a packet.
 * I.e., a packet is coming inside from the outside, so
 * fix up the source and destination addresses.
 */

static ipnat_entry *
ipnat_clone_outside_entry (ipnat_entry *nat, ipnat_entry *template)
{
    ipnat_entry *nat2;
    ipnat_mapping *mapping;

    nat2 = ipnat_create_entry(nat->address[IPNAT_INSIDE][IPNAT_LOCAL],
			      nat->address[IPNAT_INSIDE][IPNAT_GLOBAL],
			      0, 0,
			      template->address[IPNAT_OUTSIDE][IPNAT_LOCAL],
			      template->address[IPNAT_OUTSIDE][IPNAT_GLOBAL],
			      0, 0, 0, IPNF_OUTSIDE);
    if (nat2) {
	mapping = nat->mapping[IPNAT_INSIDE];
	nat2->mapping[IPNAT_INSIDE] = mapping;
	if (mapping) {
	    mapping->refcount++;
	    ipnat_addrpool_bump(mapping->poolname,
				nat2->address[IPNAT_INSIDE][IPNAT_LOCAL]);
	}
	mapping = template->mapping[IPNAT_OUTSIDE];
	nat2->mapping[IPNAT_OUTSIDE] = mapping;
	if (mapping) {
	    mapping->refcount++;
	    ipnat_addrpool_bump(mapping->poolname,
				nat2->address[IPNAT_OUTSIDE][IPNAT_LOCAL]);
	}
    }
    return(nat2);
}

static ipnat_entry *
ipnat_extend_static_entry (ipnat_entry *nat, ipnat_entry *template,
			   ipaddrtype srcadr, ushort sport, ushort dport,
			   uchar proto)
{
    ipnat_entry *nat2;
    ipnat_mapping *mapping;
    ipaddrtype outside_globaladdr;
    uint newport;
    ushort ports[IPNAT_DOMAINS][IPNAT_LOOKUPS];
    uchar flags;

    flags = IPNF_EXTENDED;
    if (template) {
	outside_globaladdr = template->address[IPNAT_OUTSIDE][IPNAT_GLOBAL];
	flags |= IPNF_OUTSIDE;
    } else {
	outside_globaladdr = srcadr;
    }

    if (nat->flags & IPNF_EXTENDED) {
	/*
	 * Use the port numbers supplied by the static translation.
	 */
	ports[IPNAT_INSIDE][IPNAT_LOCAL] =
	    nat->port[IPNAT_INSIDE][IPNAT_LOCAL];
	/*
	 * Allocate the port from the portlist that might be around
	 * for a dynamic mapping for this global address and protocol.
	 */
	newport = ipnat_allocate_port(NULL, TCP_PROT,
				      nat->address[IPNAT_INSIDE][IPNAT_LOCAL],
				      nat->address[IPNAT_INSIDE][IPNAT_GLOBAL],
				      nat->port[IPNAT_INSIDE][IPNAT_GLOBAL],
				      IPNAT_GLOBAL);
	if (newport == IPNAT_NO_PORT) {
	    if (ipnat_debug)
		buginf("\nNAT: no global port for %i",
		       nat->address[IPNAT_INSIDE][IPNAT_GLOBAL]);
	    return(NULL);
	}
	ports[IPNAT_INSIDE][IPNAT_GLOBAL] = newport;
	if (nat->flags & IPNF_OUTSIDE) {
	    ports[IPNAT_OUTSIDE][IPNAT_LOCAL] =
		nat->port[IPNAT_OUTSIDE][IPNAT_LOCAL];
	    ports[IPNAT_OUTSIDE][IPNAT_GLOBAL] =
		nat->port[IPNAT_OUTSIDE][IPNAT_GLOBAL];
	} else {
	    ports[IPNAT_OUTSIDE][IPNAT_LOCAL] =
		ports[IPNAT_OUTSIDE][IPNAT_GLOBAL] = sport;
	}
    } else {
	/*
	 * Use the port numbers from the packet.
	 */
	ports[IPNAT_INSIDE][IPNAT_LOCAL] =
	    ports[IPNAT_INSIDE][IPNAT_GLOBAL] = dport;
	ports[IPNAT_OUTSIDE][IPNAT_LOCAL] =
	    ports[IPNAT_OUTSIDE][IPNAT_GLOBAL] = sport;
    }
    nat2 = ipnat_create_entry(nat->address[IPNAT_INSIDE][IPNAT_LOCAL],
			      nat->address[IPNAT_INSIDE][IPNAT_GLOBAL],
			      ports[IPNAT_INSIDE][IPNAT_LOCAL],
			      ports[IPNAT_INSIDE][IPNAT_GLOBAL],
			      srcadr, outside_globaladdr,
			      ports[IPNAT_OUTSIDE][IPNAT_LOCAL],
			      ports[IPNAT_OUTSIDE][IPNAT_GLOBAL],
			      proto, flags);
    if (nat2) {
	if (template) {
	    mapping = template->mapping[IPNAT_OUTSIDE];
	    nat2->mapping[IPNAT_OUTSIDE] = mapping;
	    if (mapping) {
		mapping->refcount++;
		ipnat_addrpool_bump(mapping->poolname,
				    nat2->address[IPNAT_OUTSIDE][IPNAT_LOCAL]);
	    }
	}
    }
    return(nat2);
}

static paktype *
ipnat_translate_before_routing (paktype *pak, boolean *drop)
{
    ipnat_entry *nat;
    ipnat_entry *template;
    iphdrtype *ip;
    ipaddrtype srcadr, outsideaddr;
    ushort sport, dport;
    boolean ddt, new_conversation, can_overload, new_template;
    enum ipnat_addrpool_type pool_type;
    ipnat_mapping *mapping;
    uchar proto;

    *drop = FALSE;

    ip = (iphdrtype *) ipheadstart(pak);

    /*
     * Don't bother if this is a broadcast.
     */
    if (ip->dstadr == IP_LIMITED_BROADCAST ||
	ip->dstadr == IP_BSD_LIMITED_BROADCAST)
	return(NULL);

    ipnat_get_ports(ip, &sport, &dport, &proto, &srcadr, &can_overload);

    if (ipnat_detailed_debug) {
	char *protstring;

	protstring = lookup_id_string(proto_items, ip->prot);
	buginf("\nNAT: o: %s (%i, %d) -> (%i, %d) [%d]", protstring ?
	       protstring : "?", ip->srcadr, sport, ip->dstadr, dport, ip->id);
    }

    new_conversation = ddt = FALSE;
    /*
     * Check to see if this is a connection attempt coming inside
     * that we need to dynamically create a translation for.
     */
    if (ip->prot == TCP_PROT) {
	mapping = ipnat_find_mapping(pak, ip->dstadr, IPNAT_GLOBAL,
				     IPNAT_INSIDE);
	if (mapping) {
	    pool_type = ipnat_addrpool_pooltype(mapping->poolname);
	    if (pool_type == IPNAT_POOL_ROTARY) {
		tcptype *tcp = (tcptype *) ipdatastart_iph(ip);

		ddt = TRUE;
		if (tcp->syn && !tcp->ack)
		    new_conversation = TRUE;
	    }
	}
    } else {
	mapping = NULL;
    }

    /*
     * First try to find the most specific translation entry possible.
     */
    nat = ipnat_find_entry(ip->dstadr, srcadr, dport, sport, proto,
			   IPNAT_GLOBAL);
    if (nat) {
	/* Ready to translate. */
	ipnat_stats.hits++;
    } else {
	if (ipnat_pending_doors()) {
	    nat = ipnat_open_door(proto, srcadr, ip->dstadr, sport, dport,
				  pak);
	    if (nat) {
		/* Ready to translate. */
		ipnat_stats.hits++;
		/*
		 * Yes, goto's are bad, but I want all translations
		 * to occur at the bottom of this routine, and the
		 * nesting here makes this difficult to arrange.
		 */
		goto translate;
	    }
	}

	/*
	 * See if the source address requires translation into local space.
	 */
	new_template = FALSE;
	template = ipnat_find_entry(IPADDR_ZERO, ip->srcadr, 0, 0, 0,
				    IPNAT_GLOBAL);
	if (template)
	    outsideaddr = ip->srcadr;
	else
	    outsideaddr = IPADDR_ZERO;

	/* Maybe try looking more generally. */
	if (!ddt) {
	    /*
	     * If a template wasn't found, go and try to create one.
	     * If it fails, no big deal.
	     */
	    if (template == NULL) {
		boolean dummy;

		template = ipnat_new_outside_entry(pak, ip->srcadr, &dummy);
		new_template = TRUE;
	    }
	    /*
	     * Now try looking for a more general entry.  Static entries
	     * will get matched here.
	     */
	    nat = ipnat_find_entry(ip->dstadr, outsideaddr, 0, 0, 0,
				   IPNAT_GLOBAL);
	    if (nat == NULL) {
		/* That didn't work, try with no outside address. */
		nat = ipnat_find_entry(ip->dstadr, IPADDR_ZERO, 0, 0, 0,
				       IPNAT_GLOBAL);
		if (nat == NULL) {
		    /* OK, how about inside address and port? */
		    nat = ipnat_find_entry(ip->dstadr, IPADDR_ZERO, dport, 0,
					   proto, IPNAT_GLOBAL);
		    if (nat == NULL) {
			/* We lose. */
			if (new_template && template)
			    ipnat_delete_entry(template);
			return(NULL);
		    }
		}
	    }

	    /*
	     * If this entry is one of a set of static entries
	     * that are ambiguous from the inside, create an
	     * extended entry for this conversation so that the
	     * inside->outside traffic gets translated correctly.
	     */
	    if (nat->flags & IPNF_EXTENDABLE) {
		nat = ipnat_extend_static_entry(nat, template, srcadr,
						sport, dport, proto);
		if (nat == NULL) {
		    /* We lose. */
		    if (new_template && template)
			ipnat_delete_entry(template);
		    return(NULL);
		}
	    } else if (!(nat->flags & IPNF_OUTSIDE) && template) {
		/*
		 * Make a full inside+outside translation from
		 * this one and the outside template if this one
		 * doesn't already have an outside translation.
		 */
		nat = ipnat_clone_outside_entry(nat, template);
		if (nat == NULL) {
		    /* We lose. */
		    *drop = TRUE;
		    if (new_template && template)
			ipnat_delete_entry(template);
		    return(NULL);
		}
	    }
	    ipnat_stats.hits++;
	} else if (new_conversation) {
	    /*
	     * This is a connection attempt to a rotary-style global
	     * address, so set up a new mapping for this conversation.
	     */
	    nat = ipnat_new_inside_dest_entry(template, pak, ip->dstadr,
					      ip->srcadr, sport, dport,
					      ip->prot, drop);
	    ipnat_stats.misses++;
	}

	if (nat == NULL) {
	    /* We lose. */
	    if (new_template && template)
		ipnat_delete_entry(template);
	    return(NULL);
	}
    }

translate:
    /*
     * If first fragment and overloading, save info for subsequent
     * fragments.
     */
    if (ip->morefragments && ip->fo == 0 && can_overload &&
	(nat->flags & IPNF_EXTENDED))
	ipnat_save_fragment0(ip, sport, dport);

    pak = ipnat_l3_fixup(pak, ip, nat, IPNAT_GLOBAL);
    if (pak == NULL)
	*drop = TRUE;
    return(pak);
}

/*
 * ipnat_translate_after_routing
 *
 * Do a local->global translation on a packet.
 * I.e., a packet is going outside, so fix up the source address.
 */

static paktype *
ipnat_translate_after_routing (paktype *pak, boolean *drop)
{
    ipnat_entry *nat;
    ipnat_entry *template;
    iphdrtype *ip;
    ushort sport, dport;
    ipaddrtype outsideaddr, srcadr;
    boolean can_overload, fix_nexthop;
    uchar proto;

    *drop = FALSE;

    ip = (iphdrtype *) ipheadstart(pak);

    ipnat_get_ports(ip, &sport, &dport, &proto, &srcadr, &can_overload);

    if (ipnat_detailed_debug) {
	char *protstring;

	protstring = lookup_id_string(proto_items, ip->prot);
	buginf("\nNAT: i: %s (%i, %d) -> (%i, %d) [%d]", protstring ?
	       protstring : "?", ip->srcadr, sport, ip->dstadr, dport, ip->id);
    }

    /*
     * First try to find the most specific translation entry possible.
     */
    nat = ipnat_find_entry(ip->srcadr, ip->dstadr, sport, dport, ip->prot,
			   IPNAT_LOCAL);
    if (nat) {
	/* Ready to translate. */
	ipnat_stats.hits++;
    } else {
	/* Try looking more generally. */
	/*
	 * See if the destination address requires translation
	 * into global space.
	 */
	template = ipnat_find_entry(IPADDR_ZERO, ip->dstadr, 0, 0, 0,
				    IPNAT_LOCAL);
	if (template)
	    outsideaddr = ip->dstadr;
	else
	    outsideaddr = IPADDR_ZERO;

	/*
	 * Now try looking for a more general entry.  Static entries
	 * will get matched here.
	 */
	nat = ipnat_find_entry(ip->srcadr, outsideaddr, 0, 0, 0, IPNAT_LOCAL);
	if (nat && !(nat->flags & IPNF_EXTENDABLE)) {
	    ipnat_mapping *mapping;

	    /*
	     * See if the simple entry found needs to be cloned into
	     * an extended entry.  We need to create extended entries
	     * for all TCP or UDP flows if overloading is enabled
	     * for this mapping so that we keep track of all ports
	     * that are in use for the inside global address.
	     */
	    if (can_overload &&
		(mapping =
		 ipnat_find_mapping(pak,
				    nat->address[IPNAT_INSIDE][IPNAT_LOCAL],
				    IPNAT_LOCAL, IPNAT_INSIDE)) &&
		(mapping->flags & IPNMF_OVERLOAD)) {
		/* Make a new entry from this one */
		nat = ipnat_clone_entry(nat, template, mapping, sport,
					ip->prot, ip->dstadr, dport);
		if (nat == NULL)
		    *drop = TRUE;
	    } else if (!(nat->flags & IPNF_OUTSIDE) && template) {
		/*
		 * Don't need to make an extended entry, but may need
		 * to clone the simple entry matched into one with
		 * an outside mapping, so do that here if needed.
		 */
		nat = ipnat_new_inside_entry(template, pak, ip->srcadr,
					     ip->dstadr, 0, 0, 0, FALSE, drop);
	    }
	    if (nat == NULL) {
		/* We lose. */
		return(NULL);
	    }

	    /* Ready to translate. */
	    ipnat_stats.hits++;
	} else {
	    /*
	     * Translation not found, so need to create one.  Supply
	     * all protocol and port information, and the routine will
	     * figure out whether it needs to be extended or not.
	     */
	    nat = ipnat_new_inside_entry(template, pak, ip->srcadr, ip->dstadr,
					 sport, dport, ip->prot, can_overload,
					 drop);
	    if (nat == NULL) {
		/* We lose. */
		return(NULL);
	    }
	    ipnat_stats.misses++;
	}
    }

    /*
     * If first fragment and overloading, save info for subsequent
     * fragments.
     */
    if (ip->morefragments && ip->fo == 0 && can_overload &&
	(nat->flags & IPNF_EXTENDED))
	ipnat_save_fragment0(ip, sport, dport);

    /*
     * Translation might change the destination address.  If this
     * address is also the next-hop that routing chose, then the
     * next-hop address may have to be changed, too.  Remember if
     * this is needed.
     */
    fix_nexthop = (pak->desthost == ip->dstadr);

    pak = ipnat_l3_fixup(pak, ip, nat, IPNAT_LOCAL);
    if (pak == NULL)
	*drop = TRUE;
    else if (fix_nexthop)
	pak->desthost = ip->dstadr;

    return(pak);
}

/*
 * ipnat_accesscheck
 *
 *
 */

boolean
ipnat_accesscheck (ipnat_entry *nat, int list)
{
    enum ipnat_lookup lookup;
    enum ipnat_domain domain;

    for (domain = IPNAT_INSIDE; domain < IPNAT_DOMAINS; domain++)
	for (lookup = IPNAT_LOCAL; lookup < IPNAT_LOOKUPS; lookup++)
	    if (fast_check(list, nat->address[domain][lookup]))
		return(TRUE);
    return(FALSE);
}

/*
 * ipnat_ager
 *
 * ...and its little helper(s).
 * Age out dynamic translations that haven't been used in a while.
 */

static void
ipnat_ager_timers (void)
{
    mgd_timer *leaf_timer;
    ipnat_entry *nat;
    ipnat_door *door;
    ipnat_fragment *fragment;
    char *protstring;

    /*
     * Go through all the expired translation entry timers first.
     */
    while (mgd_timer_expired(&ipnat_entry_timer_master)) {
	leaf_timer = mgd_timer_first_expired(&ipnat_entry_timer_master);
	if (leaf_timer) {
	    nat = mgd_timer_context(leaf_timer);
	    if (ipnat_debug && (!ipdebug_acl ||
				ipnat_accesscheck(nat, ipdebug_acl))) {
		buginf("\nNAT: expiring %i (%i)",
		       nat->address[IPNAT_INSIDE][IPNAT_GLOBAL],
		       nat->address[IPNAT_INSIDE][IPNAT_LOCAL]);
		if (nat->flags & IPNF_EXTENDED) {
		    protstring = lookup_id_string(proto_items, nat->proto);
		    buginf(" %s %d (%d)", protstring ? protstring : "?",
			   nat->port[IPNAT_INSIDE][IPNAT_GLOBAL],
			   nat->port[IPNAT_INSIDE][IPNAT_LOCAL]);
		}
	    }
	    ipnat_delete_entry(nat);
	    ipnat_stats.entry_timeouts++;
	}
    }

    /*
     * Next go through all the expired door timers.
     */
    while (mgd_timer_expired(&ipnat_door_timer_master)) {
	leaf_timer = mgd_timer_first_expired(&ipnat_door_timer_master);
	if (leaf_timer) {
	    door = mgd_timer_context(leaf_timer);
	    if (ipnat_debug) {
		protstring = lookup_id_string(proto_items, door->proto);
		buginf("\nNAT: expiring door %s (%i/*, %d/*) -> (%i, %d)",
		       protstring ? protstring : "?", door->outsidehost,
		       door->serverport, door->insidehost,
		       door->insideport[IPNAT_LOCAL]);
	    }
	    ipnat_remove_door(door);
	    if (door->flags & IPNDF_PORT_ALLOCATED)
		ipnat_free_port(door->proto, door->insidehost,
				door->insideport[IPNAT_GLOBAL], IPNAT_GLOBAL);
	    ipnat_destroy_door(door);
	}
    }

    /*
     * Next go through all the expired fragment timers.
     */
    while (mgd_timer_expired(&ipnat_fragment_timer_master)) {
	leaf_timer = mgd_timer_first_expired(&ipnat_fragment_timer_master);
	if (leaf_timer) {
	    fragment = mgd_timer_context(leaf_timer);
	    if (ipnat_debug)
		buginf("\nNAT: expiring fragment %i %i %d %d",
		       fragment->srcadr, fragment->dstadr, fragment->id,
		       fragment->prot);
	    ipnat_remove_fragment(fragment);
	    ipnat_destroy_fragment(fragment);
	}
    }
}

static void
ipnat_ager_messages (void)
{
    ulong message, value;
    void *pointer;

    /*
     * Drain the queue of messages.
     */
    while (process_get_message(&message, &pointer, &value)) {
	switch (message) {
	case IPNAT_MSG_INSTALL_ALIAS:
	    (void) ipnat_establish_alias(value, FALSE);
	    break;
	default:
	    errmsg(&msgsym(UNEXPECTEDMESSAGE, SCHED), message, pointer, value);
	    break;
	}
    }
}

static process
ipnat_ager (void)
{
    ulong major, minor;

    /*
     * NAT configuration may send messages with work that needs
     * to be done after initialization is complete, so wait for this
     * before continuing.
     */
    process_wait_on_system_init();

    process_watch_mgd_timer(&ipnat_timer_master, ENABLE);
    while (TRUE) {
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	    case TIMER_EVENT:
		ipnat_ager_timers();
		break;
	    case MESSAGE_EVENT:
		ipnat_ager_messages();
		break;
	    default:
		errmsg(&msgsym(UNEXPECTEDEVENT, SCHED), major, minor);
		break;
	    }
	}
    }
}

static void
ipnat_start_ager (void)
{
    if (ipnat_ager_pid == 0)
	ipnat_ager_pid = process_create(ipnat_ager, "IP NAT Ager",
					NORMAL_STACK, PRIO_NORMAL);
}

/*
 * User interface
 */

/*
 * ipnat_clear_command
 *
 * Clear a dynamic translation.
 */

void
ipnat_clear_command (parseinfo *csb)
{
    ipnat_entry *nat;
    ipaddrtype inside_globaladdr, inside_localaddr;
    ipaddrtype outside_globaladdr, outside_localaddr;
    uchar proto;
    ushort inside_globalport, inside_localport;
    ushort outside_globalport, outside_localport;
    boolean star, extended, inside, outside;

    if (!ipnat_stats.initialized)
	return;

    switch (csb->which) {
    case IPNAT_DYNAMIC:
	star = GETOBJ(int,1);
	if (star) {
	    /* Go through entire hash table, freeing everything. */
	    ipnat_free_all_hash();
	} else {
	    extended = GETOBJ(int,2);
	    inside = GETOBJ(int,3);
	    outside = GETOBJ(int,4);
	    inside_globaladdr = GETOBJ(paddr,1)->ip_addr;
	    inside_localaddr = GETOBJ(paddr,2)->ip_addr;
	    if (outside) {
		outside_localaddr = GETOBJ(paddr,3)->ip_addr;
		outside_globaladdr = GETOBJ(paddr,4)->ip_addr;
	    } else {
		outside_localaddr = outside_globaladdr = IPADDR_ZERO;
	    }
	    if (extended) {
		proto = GETOBJ(int,5);
		inside_globalport = GETOBJ(int,6);
		inside_localport = GETOBJ(int,7);
		if (outside) {
		    outside_localport = GETOBJ(int,8);
		    outside_globalport = GETOBJ(int,9);
		} else {
		    outside_localport = outside_globalport = 0;
		}
	    } else {
		proto = 0;
		inside_globalport = inside_localport = 0;
		outside_localport = outside_globalport = 0;
	    }
	    nat = ipnat_find_entry(inside_globaladdr, outside_globaladdr,
				   inside_globalport, outside_globalport,
				   proto, IPNAT_GLOBAL);
	    if (nat == NULL)
		printf("\n%%Translation not found");
	    else if (nat->flags & IPNF_STATIC)
		printf("\n%%Translation not dynamic");
	    else
		ipnat_delete_entry(nat);
	}
	break;

    case IPNAT_STATS:
	ipnat_stats.entry_timeouts = 0;
	ipnat_stats.hits = 0;
	ipnat_stats.misses = 0;
	break;

    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * ipnat_add_dynamic_cfg
 *
 * Add a mapping to the global list.
 */

static int
mappingsort (void *vmapping1, void *vmapping2)
{
    ipnat_mapping *mapping1, *mapping2;
    acl_headertype *acl1, *acl2;
    ulong aclnum1, aclnum2;

    mapping1 = vmapping1;
    mapping2 = vmapping2;

    if (mapping1->match_type == mapping2->match_type) {
	switch (mapping1->match_type) {
	case IPNAT_MATCH_ACL:
	    acl1 = mapping1->acl;
	    acl2 = mapping2->acl;
	    /*
	     * Sort ACLs numerically if possible.
	     */
	    if (ipaccess_numeric_acl(acl1) && ipaccess_numeric_acl(acl2)) {
		aclnum1 = atoi(acl1->name);
		aclnum2 = atoi(acl2->name);
		return(aclnum1 - aclnum2);
	    } else {
		return(strcmp(acl1->name, acl2->name));
	    }
	case IPNAT_MATCH_ROUTE_MAP:
	    return(strcmp(mapping1->mapname, mapping2->mapname));
	}
    }

    switch (mapping1->match_type) {
    case IPNAT_MATCH_ACL:
	return(-1);	/* ACL's come first */
    case IPNAT_MATCH_ROUTE_MAP:
	return(1);	/* then route maps */
    }

    return(0);
}

static boolean
ipnat_add_dynamic_cfg (parseinfo *csb, list_header *mapping_list,
		       acl_headertype *acl, char *mapname,
		       enum ipnat_lookup lookup, enum ipnat_domain domain)
{
    boolean mapping_NULL;
    ipnat_mapping *mapping;
    char *poolname;
    list_element *el;
    boolean overload;
    enum ipnat_addrpool_type pool_type;
    idbtype *idb;
    enum ipnat_match_type match_type;
    enum ipnat_source_type source_type;
    boolean found;

    if (csb->which == IPNAT_ISOURCE)
	overload = GETOBJ(int,2) & IPNAT_OVERLOAD_SET;
    else
	overload = FALSE;
    poolname = GETOBJ(string,1);
    mapname = GETOBJ(string,3);
    idb = GETOBJ(idb,1);

    /* Determine where the address(es) will come from. */
    if (acl)
	match_type = IPNAT_MATCH_ACL;
    else
	match_type = IPNAT_MATCH_ROUTE_MAP;

    /* Determine where the address(es) will come from. */
    if (poolname[0] != '\0')
	source_type = IPNAT_SOURCE_POOL;
    else
	source_type = IPNAT_SOURCE_INTERFACE;

    /*
     * Search for an existing mapping.
     */
    found = FALSE;
    FOR_ALL_DATA_IN_LIST(mapping_list, el, mapping) {
	switch (mapping->match_type) {
	case IPNAT_MATCH_ACL:
	    if (mapping->acl == acl)
		found = TRUE;	/* ding */
	    break;
	case IPNAT_MATCH_ROUTE_MAP:
	    if (strcmp(mapping->mapname, mapname) == 0)
		found = TRUE;	/* ding */
	    break;
	}
	if (found)		/* look, no goto */
	    break;
    }

    mapping_NULL = (mapping) ? FALSE : TRUE; /* so we can back out cleanly */

    if (mapping) {
	if (mapping->refcount > 0) {
	    printf("\n%%Dynamic mapping in use, cannot change");
	    return(FALSE);
	}
    } else {
	/*
	 * Create a new mapping.
	 */
	mapping = malloc(sizeof(ipnat_mapping));
	if (mapping == NULL) {
	    printf(nomemory);
	    return(FALSE);
	}
    }

    /*
     * Do some sanity-checking on the config.
     */
    if (source_type == IPNAT_SOURCE_POOL) {
	pool_type = ipnat_addrpool_pooltype(poolname);
	if (pool_type == IPNAT_POOL_ROTARY) {
	    if (csb->which == IPNAT_ISOURCE || csb->which == IPNAT_OSOURCE)
		printf("\n%% Pool %s is a rotary-type pool, "
		       "unexpected behavior may result.", poolname);
	} else if (csb->which == IPNAT_IDESTINATION) {
	    printf("\n%% Pool %s is not a rotary-type pool, "
		   "unexpected behavior may result.", poolname);
	}
    }

    /*
     * Initialize the mapping.
     */

    /* Set up the matching function. */
    mapping->match_type = match_type;
    switch (match_type) {
    case IPNAT_MATCH_ACL:
	mapping->acl = acl;
	mapping->mapname = NULL;
	break;
    case IPNAT_MATCH_ROUTE_MAP:
	mapping->acl = NULL;
	setstring(&mapping->mapname, mapname);
	break;
    }

    /* Set up the address source. */
    mapping->source_type = source_type;
    switch (source_type) {
    case IPNAT_SOURCE_POOL:
	if (ipnat_addrpool_inuse(poolname)) {
	    printf("\n%%Pool %s in use, cannot replace", poolname);
	    if (mapping_NULL)
		free(mapping); /* Free the newly allocated structure. */
	    return(FALSE);
	}
	setstring(&mapping->poolname, poolname);
	mapping->idb = NULL;
	break;
    case IPNAT_SOURCE_INTERFACE:
	mapping->idb = idb;
	if (idb)
	    overload = TRUE;
	if (mapping->poolname)
	    setstring(&mapping->poolname, NULL);
	break;
    }
    if (overload)
	mapping->flags |= IPNMF_OVERLOAD;
    else
	mapping->flags &= ~IPNMF_OVERLOAD;

    /*
     * Add mapping to list of dynamic mappings if it is new.
     */
    if (mapping_NULL)
	list_insert(mapping_list, &mapping->element, mapping, mappingsort);

    return(TRUE);
}

/*
 * ipnat_remove_dynamic_cfg
 *
 * Remove a mapping from the global list.
 */

static boolean
ipnat_remove_dynamic_cfg (parseinfo *csb, list_header *mapping_list,
			  acl_headertype *acl, char *mapname)
{
    ipnat_mapping *mapping;
    list_element *el;

    FOR_ALL_DATA_IN_LIST(mapping_list, el, mapping) {
	if ((acl && mapping->acl == acl) ||
	    (mapping->mapname && strcmp(mapname, mapping->mapname) == 0)) {
	    if (mapping->refcount > 0) {
		printf("\n%%Dynamic mapping in use, cannot remove");
		return(FALSE);
	    }
	    list_remove(mapping_list, &mapping->element, NULL);
	    break;
	}
    }

    if (mapping == NULL) {
	printf("\n%%Dynamic mapping not found");
	return(FALSE);
    }

    if (mapping->poolname)
	setstring(&mapping->poolname, NULL);
    if (mapping->mapname)
	setstring(&mapping->mapname, NULL);

    /* Reap any portlists that reference this mapping (sigh). */
    ipnat_reap_portlists(mapping);

    free(mapping);

    return(TRUE);
}

/*
 * ipnat_add_static_aliases
 *
 * Insert any required aliases.
 * This function is meant to be executed interactively (synchronously
 * with the user), so check for ARP collisions to give the poor user
 * a break.
 */

static boolean
ipnat_add_static_aliases (ipaddrtype addr[IPNAT_DOMAINS][IPNAT_LOOKUPS],
			  boolean verbose)
{
    ipaddrtype address;
    const char emsg[] = "\n%% Address %i already in use on attached network";

    address = addr[IPNAT_INSIDE][IPNAT_GLOBAL];
    if (address && !ipnat_establish_alias(address, TRUE)) {
	if (verbose)
	    printf(emsg, address);
	return(FALSE);
    }

    address = addr[IPNAT_OUTSIDE][IPNAT_LOCAL];
    if (address && !ipnat_establish_alias(address, TRUE)) {
	if (verbose)
	    printf(emsg, address);
	return(FALSE);
    }

    return(TRUE);
}

/*
 * ipnat_remove_static_aliases
 *
 * Remove any installed aliases.
 */

static void
ipnat_remove_static_aliases (ipaddrtype addr[IPNAT_DOMAINS][IPNAT_LOOKUPS])
{
    ipaddrtype address;

    address = addr[IPNAT_INSIDE][IPNAT_GLOBAL];
    if (address && samecable(address)) {
	if (ipnat_detailed_debug)
	    buginf("\nNAT: deleting alias for %i", address);
	reg_invoke_ipalias_delete_dynamic(address);
    }
    address = addr[IPNAT_OUTSIDE][IPNAT_GLOBAL];
    if (address && samecable(address)) {
	if (ipnat_detailed_debug)
	    buginf("\nNAT: deleting alias for %i", address);
	reg_invoke_ipalias_delete_dynamic(address);
    }
}

/*
 * ipnat_add_static_cfg
 *
 * Add a static translation to the global list.
 */

static boolean
ipnat_add_static_cfg (parseinfo *csb, enum ipnat_lookup lookup,
		      enum ipnat_domain domain)
{
    ipnat_entry *nat;
    ushort flags;
    ipaddrtype addr[IPNAT_DOMAINS][IPNAT_LOOKUPS];
    ushort port[IPNAT_DOMAINS][IPNAT_LOOKUPS];
    enum ipnat_domain opposite_domain;
    enum ipnat_lookup tlookup, opposite_lookup, opposite_tlookup;
    uchar proto;

    opposite_lookup = ipnat_opposite_lookup(lookup);
    opposite_domain = ipnat_opposite_domain(domain);
    addr[domain][lookup] = GETOBJ(paddr,1)->ip_addr;
    addr[domain][opposite_lookup] = GETOBJ(paddr,2)->ip_addr;
    addr[opposite_domain][IPNAT_LOCAL] =
	addr[opposite_domain][IPNAT_GLOBAL] = IPADDR_ZERO;
    proto = GETOBJ(int,5);
    port[domain][lookup] = GETOBJ(int,6);
    port[domain][opposite_lookup] = GETOBJ(int,7);
    port[opposite_domain][IPNAT_LOCAL] =
	port[opposite_domain][IPNAT_GLOBAL] = 0;

    flags = IPNF_STATIC;
    /*
     * A static entry is extendable if the keyword is specified
     * or if it specifies ports with the addresses.
     */
    if ((GETOBJ(int,2) & IPNAT_EXTEND_SET) || (proto != 0))
	flags |= IPNF_EXTENDABLE;
    if (domain == IPNAT_OUTSIDE)
	flags |= IPNF_OUTSIDE;
    if (csb->which == IPNAT_ODESTINATION || csb->which == IPNAT_IDESTINATION)
	flags |= IPNF_DEST;
    if (proto)
	flags |= IPNF_EXTENDED;
    if (GETOBJ(int,2) & IPNAT_NOALIAS_SET)
	flags |= IPNF_NOALIAS;

    /*
     * See if a translation is already configured
     * for one of the entered addresses.
     */
    for (tlookup = IPNAT_LOCAL; tlookup < IPNAT_LOOKUPS; tlookup++) {
	nat = ipnat_find_entry(addr[IPNAT_INSIDE][tlookup],
			       addr[IPNAT_OUTSIDE][tlookup],
			       port[IPNAT_INSIDE][tlookup],
			       port[IPNAT_OUTSIDE][tlookup], proto, tlookup);
	if (nat) {
	    opposite_tlookup = ipnat_opposite_lookup(tlookup);
	    if (nat->address[domain][opposite_tlookup] ==
		addr[domain][opposite_tlookup]) {
		/*
		 * Exactly the same entry, just update the flags.
		 * If the status of the no-alias flag is changing,
		 * either add or remove the alias as appropriate.
		 */
		if ((nat->flags & IPNF_NOALIAS) && !(flags & IPNF_NOALIAS)) {
		    if (!ipnat_add_static_aliases(addr, TRUE))
			return(FALSE);
		} else if (!(nat->flags & IPNF_NOALIAS) &&
			   (flags & IPNF_NOALIAS))
		    ipnat_remove_static_aliases(addr);
		nat->flags = flags;
		return(TRUE);
	    }

	    if ((nat->address[IPNAT_INSIDE][IPNAT_LOCAL] ==
		 addr[IPNAT_INSIDE][IPNAT_LOCAL]) &&
		(nat->flags & IPNF_EXTENDABLE) && (flags & IPNF_EXTENDABLE))
		continue;

	    printf("\n%% %i already mapped (%i -> %i)", addr[domain][tlookup],
		   nat->address[domain][IPNAT_LOCAL],
		   nat->address[domain][IPNAT_GLOBAL]);
	    return(FALSE);
	}
    }

    /*
     * Set up aliases for the global address(es) if needed.
     */
    if (!(flags & IPNF_NOALIAS))
	if (!ipnat_add_static_aliases(addr, TRUE))
	    return(FALSE);

    /*
     * Create the translation entry.
     */
    nat = ipnat_create_entry(addr[IPNAT_INSIDE][IPNAT_LOCAL],
			     addr[IPNAT_INSIDE][IPNAT_GLOBAL],
			     port[IPNAT_INSIDE][IPNAT_LOCAL],
			     port[IPNAT_INSIDE][IPNAT_GLOBAL],
			     addr[IPNAT_OUTSIDE][IPNAT_LOCAL],
			     addr[IPNAT_OUTSIDE][IPNAT_GLOBAL],
			     port[IPNAT_OUTSIDE][IPNAT_LOCAL],
			     port[IPNAT_OUTSIDE][IPNAT_GLOBAL], proto, flags);
    if (nat == NULL) {
	printf(nomemory);
	return(FALSE);
    }

    ipnat_stats.statics++;
    return(TRUE);
}

/*
 * ipnat_remove_static_cfg
 *
 * Remove a static translation from the global list.
 */

static boolean
ipnat_remove_static_cfg (parseinfo *csb, enum ipnat_lookup lookup,
			 enum ipnat_domain domain)
{
    ipnat_entry *nat;
    ipaddrtype address;
    ushort port;
    uchar proto;

    address = GETOBJ(paddr,1)->ip_addr;
    proto = GETOBJ(int,5);
    port = GETOBJ(int,6);

    switch (csb->which) {
    case IPNAT_ISOURCE:
    case IPNAT_IDESTINATION:
	nat = ipnat_find_entry(address, IPADDR_ZERO, port, 0, proto, lookup);
	break;
    case IPNAT_ODESTINATION:
    case IPNAT_OSOURCE:
	nat = ipnat_find_entry(IPADDR_ZERO, address, 0, port, proto, lookup);
	break;
    default:
	nat = NULL;
	break;
    }
    if (nat == NULL || !(nat->flags & IPNF_STATIC)) {
	printf("\n%%Translation not found");
	return(FALSE);
    }

    /*
     * Remove the aliases if they were needed.
     */
    if (!(nat->flags & IPNF_NOALIAS))
	ipnat_remove_static_aliases(nat->address);

    ipnat_delete_entry(nat);

    ipnat_stats.statics--;
    return(TRUE);
}


/*
 * ipnat_change_idb_in_mapping
 * Replace the IDB in a NAT mapping if a match is found. 
 */
static void
ipnat_change_idb_in_mapping(list_header *mapping_list, idbtype *old_idb, 
			    idbtype *new_idb)
{
    list_element *el;
    ipnat_mapping *mapping;

    FOR_ALL_DATA_IN_LIST(mapping_list , el, mapping) {
	if (mapping->idb == old_idb) {
	    mapping->idb = new_idb;
	    break;
	}
    }
}


/*
 * ipnat_change_idb
 * An IDB has dynamically changed during interface configuration.
 * Check all NAT mappings to see whether we need to replace an old
 * IDB with the new one.
 */
static void
ipnat_change_idb (idbtype *old_idb, idbtype *new_idb)
{
    list_header *mapping_list;
    enum ipnat_lookup lookup;
    enum ipnat_domain domain;

    for (domain = IPNAT_INSIDE; domain < IPNAT_DOMAINS; domain++) {
	for (lookup = IPNAT_LOCAL; lookup < IPNAT_LOOKUPS; lookup++) {
	    mapping_list = &ipnat_mappings[domain][lookup]; 
	    ipnat_change_idb_in_mapping(mapping_list, old_idb, new_idb);
	}
    }
}


/*
 * ipnat_initialize
 *
 * Initialize everything now that NAT has actually been configured.
 */

static void
ipnat_initialize (void)
{
    int i;

    /*
     * Initialize the timer tree.
     */
    mgd_timer_init_parent(&ipnat_timer_master, NULL);
    mgd_timer_init_parent(&ipnat_entry_timer_master, &ipnat_timer_master);
    mgd_timer_init_parent(&ipnat_door_timer_master, &ipnat_timer_master);
    mgd_timer_init_parent(&ipnat_fragment_timer_master, &ipnat_timer_master);

    /*
     * Initialize the timeout defaults.
     */
    ipnat_timeouts[IPNAT_TIMEOUT].default_timeout = IPNAT_TIMEOUT_DEFAULT;
    ipnat_timeouts[IPNAT_FINRST_TIMEOUT].default_timeout =
	IPNAT_FINRST_TIMEOUT_DEFAULT;
    ipnat_timeouts[IPNAT_TCP_TIMEOUT].default_timeout =
	IPNAT_TCP_TIMEOUT_DEFAULT;
    ipnat_timeouts[IPNAT_UDP_TIMEOUT].default_timeout =
	IPNAT_UDP_TIMEOUT_DEFAULT;
    ipnat_timeouts[IPNAT_DNS_TIMEOUT].default_timeout =
	IPNAT_DNS_TIMEOUT_DEFAULT;
    ipnat_timeouts[IPNAT_ICMP_TIMEOUT].default_timeout =
	IPNAT_ICMP_TIMEOUT_DEFAULT;
    ipnat_timeouts[IPNAT_SYN_TIMEOUT].default_timeout =
	IPNAT_SYN_TIMEOUT_DEFAULT;
    for (i = 0; i < IPNAT_NUM_TIMEOUTS; i++) {
	ipnat_timeouts[i].timeout = ipnat_timeouts[i].default_timeout;
	ipnat_timeouts[i].enabled = TRUE;
    }

    /*
     * Initialize the other defaults.
     */
    ipnat_max_entries = IPNAT_MAX_ENTRIES_DEFAULT;

    /*
     * Initialize the translation database.
     */
    ipnat_db_init();

    /*
     * Initialize the address pool code.
     */
    ipnat_addrpool_init();

    /*
     * Register the translation hooks.
     */
    reg_add_translate_after_routing(LINK_IP, ipnat_translate_after_routing,
				    "ipnat_translate_after_routing");
    reg_add_translate_before_routing(LINK_IP, ipnat_translate_before_routing,
				     "ipnat_translate_before_routing");
    reg_add_media_change_idb(ipnat_change_idb, "ipnat_change_idb");

    /*
     * Start the helper process.
     */
    ipnat_start_ager();
}

/*
 * ip_nat_command
 *
 * Global IP configuration commands.
 */

void
ip_nat_command (parseinfo *csb)
{
    enum ipnat_lookup lookup;
    enum ipnat_domain domain;
    ipnat_mapping *mapping;
    ipnat_entry *nat;
    list_element *el;
    int aclnum;
    char *aclname, *mapname;
    acl_headertype *acl;

    if (!ipnat_stats.initialized)
	ipnat_initialize();

    domain = GETOBJ(int,3);
    lookup = GETOBJ(int,4);

    if (csb->nvgen) {
	switch (csb->which) {
	case IPNAT_ISOURCE:
	case IPNAT_IDESTINATION:
	case IPNAT_OSOURCE:
	case IPNAT_ODESTINATION:
	    /* Dump all dynamic mappings. */
	    FOR_ALL_DATA_IN_LIST(&ipnat_mappings[domain][lookup], el,
				 mapping) {
		switch (mapping->match_type) {
		case IPNAT_MATCH_ACL:
		    nv_write(TRUE, "%s list %s", csb->nv_command,
			     mapping->acl->name);
		    break;
		case IPNAT_MATCH_ROUTE_MAP:
		    nv_write(TRUE, "%s route-map %s", csb->nv_command,
			     mapping->mapname);
		    break;
		}
		switch (mapping->source_type) {
		case IPNAT_SOURCE_POOL:
		    nv_add(TRUE, " pool %s", mapping->poolname);
		    break;
		case IPNAT_SOURCE_INTERFACE:
		    nv_add(TRUE, " interface %s", mapping->idb->namestring);
		    break;
		}
		if (csb->which == IPNAT_ISOURCE &&
		    (mapping->flags & IPNMF_OVERLOAD))
		    nv_add(TRUE, " overload");
	    }

	    /* Dump all static entries. */
	    for (nat = ipnat_statics[domain][lookup]; nat;
		 nat = nat->static_next) {
		if (nat->flags & IPNF_EXTENDED) 
		    nv_write(TRUE, "%s static %s %i %d %i %d", csb->nv_command,
			     lookup_id_string(proto_items, nat->proto),
			     nat->address[domain][lookup],
			     nat->port[domain][lookup],
			     nat->address[domain][ipnat_opposite_lookup(lookup)],
			     nat->port[domain][ipnat_opposite_lookup(lookup)]);
		else
		    nv_write(TRUE, "%s static %i %i", csb->nv_command,
			     nat->address[domain][lookup],
			     nat->address[domain][ipnat_opposite_lookup(lookup)]);
		if (nat->flags & IPNF_EXTENDABLE)
		    nv_add(TRUE, " extendable");
		if (nat->flags & IPNF_NOALIAS)
		    nv_add(TRUE, " no-alias");
	    }
	    break;

	case IPNAT_TIMEOUT_CMD:
	    /*
	     * If the timeout is enabled and zero, write out "immediate";
	     * if the timeout is enabled and not default, write out number,
	     * if the timeout is disabled, write out "never".
	     */
	    if (ipnat_timeouts[GETOBJ(int,2)].enabled) {
		if (ipnat_timeouts[GETOBJ(int,2)].timeout == 0)
		    nv_write(TRUE, "%s immediate", csb->nv_command);
		else if (ipnat_timeouts[GETOBJ(int,2)].timeout !=
			 ipnat_timeouts[GETOBJ(int,2)].default_timeout)
		    nv_write(TRUE, "%s %d", csb->nv_command,
			     ipnat_timeouts[GETOBJ(int,2)].timeout / ONESEC);
	    } else {
		nv_write(TRUE, "%s never", csb->nv_command);
	    }
	    break;

	case IPNAT_MAX_ENTRIES:
	    if (ipnat_max_entries != IPNAT_MAX_ENTRIES_DEFAULT)
		nv_write(TRUE, "%s %d", csb->nv_command, ipnat_max_entries);
	    break;

	case IPNAT_POOL:
	    ipnat_pool_command(csb);
	    break;

	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
    case IPNAT_ISOURCE:
    case IPNAT_IDESTINATION:
    case IPNAT_OSOURCE:
    case IPNAT_ODESTINATION:
	aclnum = GETOBJ(int,1);
	aclname = GETOBJ(string,2);
	mapname = GETOBJ(string,3);
	if (aclnum || aclname[0] != '\0' || mapname[0] != '\0') {
	    if (aclnum || aclname[0] != '\0') {
		acl = access_parse_acl(aclnum, aclname, (aclnum == 0),
				       ACL_UNDEFINED);
		if (acl == NULL)
		    return;
	    } else {
		acl = NULL;
	    }
	    if (csb->sense)
		ipnat_add_dynamic_cfg(csb, &ipnat_mappings[domain][lookup],
				      acl, mapname, lookup, domain);
	    else
		ipnat_remove_dynamic_cfg(csb, &ipnat_mappings[domain][lookup],
					 acl, mapname);
	} else {
	    if (csb->sense)
		ipnat_add_static_cfg(csb, lookup, domain);
	    else
		ipnat_remove_static_cfg(csb, lookup, domain);
	}
	break;

    case IPNAT_TIMEOUT_CMD:
	/*
	 * If 0 timeout entered or "never" keyword specified, disable
	 * the timeout; if "immediate" specified store 0 and enable
	 * the timeout.
	 * If "no" or "default" prefix specified, restore default.
	 */
	if (csb->sense) {
	    if ((GETOBJ(int,3) == IPNAT_TIMEOUT_VALUE && GETOBJ(int,1) == 0) ||
		GETOBJ(int,3) == IPNAT_TIMEOUT_NEVER) {
		ipnat_timeouts[GETOBJ(int,2)].enabled = FALSE;
	    } else if (GETOBJ(int,3) == IPNAT_TIMEOUT_IMMEDIATE) {
		ipnat_timeouts[GETOBJ(int,2)].enabled = TRUE;
		ipnat_timeouts[GETOBJ(int,2)].timeout = 0;
	    } else {
		ipnat_timeouts[GETOBJ(int,2)].enabled = TRUE;
		ipnat_timeouts[GETOBJ(int,2)].timeout = GETOBJ(int,1)*ONESEC;
	    }
	} else {
	    ipnat_timeouts[GETOBJ(int,2)].enabled = TRUE;
	    ipnat_timeouts[GETOBJ(int,2)].timeout =
		ipnat_timeouts[GETOBJ(int,2)].default_timeout;
	}
	break;

    case IPNAT_MAX_ENTRIES:
	if (csb->sense)
	    ipnat_max_entries = GETOBJ(int,1);
	else
	    ipnat_max_entries = IPNAT_MAX_ENTRIES_DEFAULT;
	break;

    case IPNAT_POOL:
	ipnat_pool_command(csb);
	break;

    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * ipnat_if_command
 *
 * Interface commands.
 */

void
ipnat_if_command (parseinfo *csb)
{
    idbtype *idb, *tmp_idb;
    hwidbtype *hwidb;
    ipinfotype *ip_info;
    boolean nat_set;

    if (!ipnat_stats.initialized)
	ipnat_initialize();

    idb = csb->interface;
    hwidb = idb->hwptr;
    ip_info = idb->ip_info;

    if (csb->nvgen) {
	switch (csb->which) {
	case IPNAT_IF_OUTSIDE:
	    if (ip_info && ip_info->nat_enabled && ip_info->nat_outside)
		nv_write(TRUE, "%s", csb->nv_command);
	    break;

	case IPNAT_IF_INSIDE:
	    if (ip_info && ip_info->nat_enabled && !ip_info->nat_outside)
		nv_write(TRUE, "%s", csb->nv_command);
	    break;

	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
    case IPNAT_IF_OUTSIDE:
	if (ip_info->nat_enabled == csb->sense &&
	    ip_info->nat_outside == csb->sense) {
	    /* No change, so do nothing. */
	    return;
	}
	ip_info->nat_enabled = ip_info->nat_outside = csb->sense;
	if (csb->sense) {
	    ipnat_stats.outsides++;
	    if (csb->flags & CONFIG_TERM) {
		if (hwidb->ip_routecache & IP_OPTIMUM_ROUTECACHE)
		    printf("\n%% Optimum switching will be disabled");
		else if (hwidb->ip_routecache & IP_FLOW_ROUTECACHE)
		    printf("\n%% Flow switching will be disabled");
	    }
	} else {
	    ipnat_stats.outsides--;
	}
	break;

    case IPNAT_IF_INSIDE:
	if (ip_info->nat_enabled == csb->sense &&
	    ip_info->nat_outside == FALSE) {
	    /* No change, so do nothing. */
	    return;
	}
	ip_info->nat_enabled = csb->sense;
	ip_info->nat_outside = FALSE;
	if (csb->sense) {
	    ipnat_stats.insides++;
	    if (csb->flags & CONFIG_TERM) {
		if (hwidb->ip_routecache & IP_OPTIMUM_ROUTECACHE)
		    printf("\n%% Optimum switching will be disabled");
		else if (hwidb->ip_routecache & IP_FLOW_ROUTECACHE)
		    printf("\n%% Flow switching will be disabled");
	    }
	} else {
	    ipnat_stats.insides--;
	}
	break;

    default:
	bad_parser_subcommand(csb, csb->which);
	return;
    }

    if (ipnat_stats.insides || ipnat_stats.outsides)
	ip_nat_anywhere = TRUE;
    else
	ip_nat_anywhere = FALSE;

    /*
     * Check to see if NAT is enabled on any of our subinterfaces.
     * If so, set the flag for the entire hwidb.
     */
    nat_set = FALSE;
    FOR_ALL_SWIDBS_ON_HW(idb->hwptr, tmp_idb) {
	if (tmp_idb->ip_info && tmp_idb->ip_info->nat_enabled) {
	    nat_set = TRUE;
	    break;
	}
    }
    if (nat_set)
	hwidb->ip_fast_flags |= IP_FAST_NAT;
    else
	hwidb->ip_fast_flags &= ~IP_FAST_NAT;

    /*
     * Now that ip_nat_anywhere and ip_fast_flags are set
     * appropriately, call the fast-switch driver routines
     * to set up the right fast-switch routines.
     */
    reg_invoke_register_drivers(FSF_DODIP, hwidb);
}

/*
 * Still more user interface...
 */


/*
 * ipnat_show
 *
 * Show commands.
 */

void
ipnat_show (parseinfo *csb)
{
    if (!ipnat_stats.initialized)
	return;

    switch (csb->which) {
    case IPNAT_DYNAMIC:
	ipnat_show_entries(GETOBJ(int,1));
	break;

    case IPNAT_STATS:
	ipnat_show_stats();
	break;

    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * Parse chains for top-level IP configuration commands
 */
#define ALTERNATE	NONE
#include "cfg_ip_nat.h"
LINK_POINT(ipnat_cfg_ip, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for "debug ip" debug command
 */
#define ALTERNATE	NONE
EOLS  (debug_ip_nat_eol, ip_debug_command, DEBUG_NAT);
EOLS  (debug_ip_nat_detailed_eol, ip_debug_command, DEBUG_NAT_DETAILED);
KEYWORD (debug_ip_nat_detailed, debug_ip_nat_detailed_eol, debug_ip_nat_eol,
	 "detailed", "NAT detailed events", PRIV_OPR);
NUMBER	(debug_ip_nat_list, debug_ip_nat_detailed, debug_ip_nat_detailed,
	 OBJ(int,1), MINFASTACC, MAXFASTACC, "Access list");
KEYWORD (debug_ip_nat, debug_ip_nat_list, ALTERNATE,
	 "nat", "NAT events", PRIV_OPR);
#undef ALTERNATE
#define ALTERNATE	debug_ip_nat
LINK_POINT(ipnat_debug, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for NAT interface commands
 */
#define ALTERNATE       NONE
#include "cfg_int_ip_nat.h"
LINK_POINT(ipnat_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for NAT show commands
 */
#define	ALTERNATE	NONE
#include "exec_show_ip_nat.h"
LINK_POINT(ipnat_show_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chains for NAT clear commands
 */
#define	ALTERNATE	NONE
#include "exec_clear_ip_nat.h"
LINK_POINT(ipnat_clear_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain registration array for nat
 */
static parser_extension_request ipnat_chain_table[] = {
    { PARSE_ADD_CFG_TOP_IP_CMD, &pname(ipnat_cfg_ip) },
    { PARSE_ADD_DEBUG_IP_CMD, &pname(ipnat_debug) },
    { PARSE_ADD_CFG_INT_IP_CMD, &pname(ipnat_interface_commands) },
    { PARSE_ADD_SHOW_IP_CMD, &pname(ipnat_show_commands) },
    { PARSE_ADD_CLEAR_IP_CMD, &pname(ipnat_clear_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * ipnat_init
 * Initialize NAT support 
 */

static void
ipnat_init (subsystype *subsys)
{
    parser_add_command_list(ipnat_chain_table, "ipnat");
    ipnat_pool_parser_init();

    ipnat_ager_pid = 0;
    bzero(&ipnat_stats, sizeof(ipnat_stats));
    /*
     * Rest of initialization and allocation will happen when
     * first command is executed.
     */
}

/*
 * NAT subsystem header
 */

#define IPNAT_MAJVERSION	1
#define IPNAT_MINVERSION	0
#define IPNAT_EDITVERSION	1

SUBSYS_HEADER(ipnat, IPNAT_MAJVERSION, IPNAT_MINVERSION,
	      IPNAT_EDITVERSION, ipnat_init, SUBSYS_CLASS_PROTOCOL,
	      NULL, NULL);
