/* $Id: ipnhrp.c,v 3.9.10.6 1996/08/28 12:52:46 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ipnhrp.c,v $
 *------------------------------------------------------------------
 * Next Hop Resolution Protocol.
 *
 * June 1995, Bruce Cole
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipnhrp.c,v $
 * Revision 3.9.10.6  1996/08/28  12:52:46  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.9.10.5  1996/06/17  23:33:53  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.9.10.4  1996/05/04  01:21:50  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.9.10.3  1996/04/10  04:23:02  bcole
 * CSCdi54192:  IPX pings over a multipoint GRE tunnel fail intermittently
 * Branch: California_branch
 *
 * Fixup IPX encapsulation over multipoint tunnels.
 *
 * Revision 3.9.10.2  1996/03/28  03:31:18  bcole
 * CSCdi51494:  NHRP: Prefix entries not appearing in cache in many cases
 * Branch: California_branch
 *
 * Bring 11.1 fixes forward...
 *
 * Revision 3.9.10.1  1996/03/18  20:28:10  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.16.3  1996/03/16  06:56:58  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.16.2  1996/03/07  09:49:32  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.16.1  1996/02/20  01:03:19  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/02/22  00:06:36  bcole
 * CSCdi47623:  NHRP overwrites page zero
 *
 * Add extra NULL pointer checks.  Avoid mallocing NHRP info when not
 * necessary.
 *
 * Revision 3.8  1996/02/20  21:18:16  bcole
 * CSCdi49357:  NHRP: Problems with debug ip nhrp, cache flushing
 *
 * Repair debug command backwards compatibility.  Fix ordering problem
 * when flushing cache during deconfiguration.
 *
 * Revision 3.7  1996/02/01  06:05:37  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/26  20:50:11  bcole
 * CSCdi47574: NHRP: Unconfiguring, then reconfiguring gives mgd timer
 * warning
 *
 * Stop our master managed timer when terminating process.
 * Only create an IP routing pdb for NHRP when necessary.
 *
 * Revision 3.5  1995/12/07  22:29:47  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.4  1995/11/22  17:48:40  albright
 * CSCdi43791:  Helper addresses dont work over unnumbered interfaces
 *
 * Revision 3.3  1995/11/17  17:34:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:48:24  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:08:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:06:24  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/24  22:34:28  bcole
 * CSCdi39279:  NHRP: Aggregation needed to provide purge notification
 *
 * Provide aggregated NHRP responses.
 * Cache now consists of radix tree(s) instead of hash table.
 * Allow NHRP resolution during encapsulation of NHRP purge messages.
 * For IP, NHRP now a routing process so as to receive callbacks.
 * Allow for radix tree walk where varargs have already been pre-collected.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "config.h"
#include "logger.h"
#include "subsys.h"
#include "parser.h"
#include "packet.h"
#include "sys_registry.h"
#include "../if/network.h"
#include "../ip/ip.h"
#include "../ui/debug.h"
#include "../if/ether.h"
#include "../ip/ip_registry.h"
#include "../util/radix.h"
#include "route.h"
#include "../wan/nhrp_debug.h"
#include "../wan/nhrp.h"
#include "../wan/nhrp_public.h"
#include "../wan/nhrp_inlines.h"
#include "../wan/parser_defs_nhrp.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "../wan/nhrp_vc.h"
#include "ipnhrp.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "media_registry.h"

/*
 * Storage
 */
static pdbtype *nhrp_pdb;

/*
 * nhrp_ip_redist
 * Receive callback notifications from the IP routing code concerning
 * changes to routes which NHRP has referenced.  If an NHRP cache table entry
 * has been derived from such routes, flush the cache entry as it is likely
 * to contain stale information.
 */

static void nhrp_ip_redist (pdbtype *pdb, ndbtype *ndb, int event, int flag)
{
    nhrpaddrnet net;
    
    if (nhrp_debug)
	buginf("\nNHRP: Redist callback: %i%m", ndb->netnumber, ndb->netsmask);
    net.ip.target = ndb->netnumber;
    net.ip.mask = ndb->netsmask;
    nhrp_cache_clear_prefix(&net, nhrp_funmap(NHRP_NETIP));
}

/*
 * Public definitions start here.
 */

/*
 * nhrp_create_pdb
 * Create an IP routing pdb for NHRP.
 */

void nhrp_create_pdb (void)
{
    pdbtype *new;

    if (nhrp_pdb)
	return;
    new = malloc(sizeof(pdbtype));
    if (!new) {
	errmsg(&msgsym(NOMEMORY, SYS), "NHRP pdb", sizeof(pdbtype));
	return;
    }
    if (allocate_pdbindex(new) == -1) {
	free(new);
	return;
    }
    nhrp_pdb = new;
    nhrp_pdb->name = "nhrp";
    nhrp_pdb->family = PDB_IP;
    nhrp_pdb->pname = "NHRP Router";
    nhrp_pdb->running = TRUE;
    nhrp_pdb->proctype = PROC_NHRP;
    nhrp_pdb->redist_callback = nhrp_ip_redist;
    enqueue(&protoQ, nhrp_pdb);
}

/*
 * nhrp_delete_pdb
 * Delete the IP routing pdb which was previously created for NHRP.
 */

void nhrp_delete_pdb (void)
{
    if (!nhrp_pdb)
	return;
    unqueue(&protoQ, nhrp_pdb);
    free_pdbindex(nhrp_pdb);
    free(nhrp_pdb);
    nhrp_pdb = NULL;
}

/*
 * nhrp_ip_rtlookup
 * Perform a network layer routing table lookup on behalf of NHRP.
 */

idbtype *nhrp_ip_rtlookup (nhrpaddr *dest, nhrp_mask *mask, void **opaque,
			   boolean *static_route)
{
    static ndbtype *ndb;
    ipaddrtype nhop;
    ulong netid_out;
    idbtype *if_out;

    netid_out = 0;
    ndb = ip_rtlookup(dest->ip, 0);
    if (!ndb)
	return(NULL);
    nhop = dest->ip;
    if_out = route_lookup(ndb, &nhop, nhop, ROUTINGDEPTH, IP_PATH_DEFFLAGS,
			  NULL);
    if (if_out == NULL)
	return(NULL);
    if (NDB2PROCTYPE(ndb) == PROC_STATIC) {
	*static_route = TRUE;
    }
    *opaque = ndb;
    *mask = &ndb->netsmask;
    return(if_out);
}

/*
 * nhrp_ip_netsmask
 * Find longest match in the routing table, return subnet mask.
 */

static nhrp_mask nhrp_ip_netsmask (nhrpaddr *dest, boolean *static_route)
{
    ndbtype *ndb;

    ndb = ip_rtlookup(dest->ip, 0);
    if (!ndb)
	return(NULL);

    if (NDB2PROCTYPE(ndb) == PROC_STATIC) {
	*static_route = TRUE;
    }
    return(&ndb->netsmask);
}

/*
 * nhrp_ip_rtrevise
 * Mark an entry in the IP routing table as being redistributed into
 * NHRP.  This so that NHRP will receive a callback whenver this routing
 * table entry changes.
 */

void nhrp_ip_rtrevise (void *opaque)
{
    ndbtype *ndb;
    ulong pdbmask;

    ndb = opaque;
    pdbmask = INDEX2MASK(ndb->pdbindex);
    nhrp_pdb->distflag |= pdbmask;
    if (nhrp_debug)
	buginf("\nNHRP: Redist mask now %x", nhrp_pdb->distflag);
}

/*
 * nhrp_ip_onnet
 * Return TRUE if 'addr' is part of the network described by 'net'.
 */

static boolean nhrp_ip_onnet (nhrpnet *net, nhrpaddr *addr)
{
    return(net->ip.addr == (addr->ip & net->ip.mask));
}

/*
 * nhrp_ip_most_specific
 * Return TRUE if a is more specific than b
 */

static boolean nhrp_ip_most_specific (nhrpnet *a, nhrpnet *b)
{
    if (a->ip.mask == LONGEST_MASK(a->ip.mask, b->ip.mask))
	return(TRUE);
    return(FALSE);
}

/*
 * nhrp_ip_most_specific2
 * Return TRUE if a is more specific than b
 */

static boolean nhrp_ip_most_specific2 (uchar *a, uchar *b)
{
    ipaddrtype a_mask;
    ipaddrtype b_mask;

    a_mask = *(ipaddrtype *)a;
    b_mask = *(ipaddrtype *)b;

    return(a_mask == LONGEST_MASK(a_mask, b_mask));
}

/*
 * nhrp_ip_cmpaddr
 * Return non-zero if network address different, zero if same
 */

static boolean nhrp_ip_cmpaddr (nhrpaddr *a, nhrpaddr *b)
{
    if (a->ip == b->ip)
	return(0);
    return(1);
}

/*
 * nhrp_ip_cmpnet
 * Return non-zero if address/mask pairs are different, zero if same
 */

static int nhrp_ip_cmpnet (nhrpnet *a, nhrpnet *b)
{
    if ((a->ip.addr == b->ip.addr) && (a->ip.mask == b->ip.mask))
	return(0);
    return(1);
}

/*
 * nhrp_ip_nhop
 * Determine the next hop if for a particular destination.
 * 'Modify' indicates whether or not the dest argument should be modified
 * to contain the next hop network layer address.
 */

static idbtype *nhrp_ip_nhop (paktype *pak, nhrpaddr *dest, boolean modify)
{
    ipaddrtype dummy;
    
    if (modify)
	return(reg_invoke_ip_first_hop(&dest->ip, dest->ip,
				       IP_PATH_USE_PROXY|IP_PATH_ROUNDROBIN));
    dummy = dest->ip;
    return(reg_invoke_ip_first_hop(&dummy, dummy,
				   IP_PATH_USE_PROXY|IP_PATH_ROUNDROBIN));
}

/*
 * nhrp_ip_ifaddr
 * Determine our IP address for use in the NHRP header of packets which
 * are locally generated.
 */

static nhrpaddr *nhrp_ip_ifaddr (idbtype *idb)
{
    if (idb->ip_unnumbered)
	idb = idb->ip_unnumbered;
    return((nhrpaddr *)&idb->ip_address);
}

/*
 * nhrp_ip_copymask
 * Fill in the network mask field of the network prefix that is stored
 * in a cache entry.  Make sure that the prefix is truly a prefix.
 */

static void nhrp_ip_copymask (nhrp_mask mask, nhrpcachetype *ptr)
{
    ipaddrtype ipmask;

    ipmask = nhrp_extractmask(mask);
    ptr->prefix.ip.mask = ipmask;
    ptr->prefix.ip.target &= ipmask;
}

/*
 * nhrp_ip_isbroad
 * Determine if the network layer destination address can be considered
 * a broadcast on this interface.
 */

static boolean nhrp_ip_isbroad (nhrpaddr *dst, idbtype *swidb)
{
    if (dst->ip == -1 || ip_broadcast(dst->ip, swidb)) {
	dst->ip = -1;
	return(TRUE);
    }
    return(FALSE);
}

/*
 * nhrp_ip_ouradd
 * Determine if the network layer address is one of the locally assigned
 * addresses.
 */

static boolean nhrp_ip_ouradd (nhrpaddr *addr)
{
    return(ip_ouraddress(addr->ip));
}

/*
 * nhrp_ip_ourifadd
 * Determine if the network layer address is one of the locally assigned
 * addresses - on a particular interface.
 */

static boolean nhrp_ip_ourifadd (idbtype *swidb, nhrpaddr *addr)
{
    return(ip_ifaddress(swidb, addr->ip));
}

/*
 * nhrp_ip_macaddr
 * Attempt to resolve nhop's NBMA address.  Called by link layer specific
 * encapsulation code.
 */

boolean nhrp_ip_macaddr (paktype *pak, ipaddrtype nhop,
			 uchar const **macaddr, boolean checkfin)
{
    iphdrtype *ip;
    idbtype *if_out, *if_in;
    ipaddrtype dest;
    ushort addr_type;
    uchar netid_in, netid_out;
    boolean nhop_success, is_nhrp;
    nhrpfuns *fun;
    nhrpinfotype *nhrpinfo;
    acl_headertype *acl;

    if (IPMULTICAST(nhop) || (nhop == 0L) || (nhop == -1L)) {
	return(FALSE);
    }
    fun = nhrp_funmap(NHRP_NETIP);
    if_out = pak->if_output;
    netid_out = nhrp_netid(if_out, fun);
    if (netid_out == NHRP_NETID_UNDEF)
	return(FALSE);
    addr_type = nhrp_nbma_type(if_out);
    if (addr_type == AF_RESERVED) {
	if (nhrp_debug)
	    buginf("\nNHRP: Don't know how to resolve NBMA address for %s",
		   if_out->namestring);
	return(FALSE);
    }
    is_nhrp = (pak->linktype == LINK_NHRP);
    if (is_nhrp || !checkfin) {
	/*
	 * Only attempt to resolve the next hop.
	 */
	dest = nhop;
    } else {
	ip = iphdrtype_start(pak);
	dest = ip->dstadr;
    }
    /*
     * First attempt to resolve final IP destination
     */
    nhop_success = nhrp_resolve_nhop(if_out, nhrp_addr(&dest),
				     macaddr, addr_type, fun);
    if (nhop_success)
	return(TRUE);
    if (dest != nhop) {
	/*
	 * Cannot resolve final destination.  At least try to resolve next
	 * hop.
	 */
	nhop_success = nhrp_resolve_nhop(if_out, nhrp_addr(&nhop),
					 macaddr, addr_type, fun);
    }
    /*
     * Looks like we might want to send an NHRP request.
     * Only forwarding from outside the fabric to within the fabric
     * should cause NHRP requests.  (Locally generated traffic should
     * be considered to come from outside the fabric).
     * Also, don't send NHRP requests based upon NHRP packets, to
     * avoid recursive encapsulation attempts
     * ... unless this is a locally generated purge packet
     */
    if (is_nhrp) {
	if (!nhrp_our_purge_pak(pak))
	    return(nhop_success);
	/*
	 * Allow NHRP address resolution to be performed on NHRP purge
	 * packets, as a special case.
	 */
    }
    if_in = pak->if_input;
    if (if_in) {
	netid_in = nhrp_netid(if_in, fun);
	if (netid_in == netid_out) {
	    if (nhrp_debug)
		buginf("\nNHRP: Forwarding packet within same fabric "
		       "%s -> %s", if_in->namestring, if_out->namestring);
	    return(nhop_success);
	}
    }
    nhrpinfo = if_out->nhrp_info;
    if (nhrpinfo && nhrpinfo->pro[NHRP_NETIP].nhrp_req_acl) {
	acl = acl_array[nhrpinfo->pro[NHRP_NETIP].nhrp_req_acl];
	if (!ip_accesscheck(pak, acl)) {
	    if (nhrp_debug)
		buginf("\nNHRP: Packet rejected by acl");
	    return(nhop_success);
	}
    }
    if (fun == NULL)
	return(nhop_success);
    nhrp_req_enq(if_out, nhrp_addr(&dest), FALSE, fun);
    return(nhop_success);
}

/*
 * nhrp_ip_macaddr_wrap
 */

static boolean nhrp_ip_macaddr_wrap (paktype *pak, ipaddrtype nhop,
				     uchar const **macaddr)
{
    return(nhrp_ip_macaddr(pak, nhop, macaddr, TRUE));
}

/*
 * nhrp_ip_lookup
 * Protocol independent interface to determine next hop link layer address
 * for use in forwarding of NHRP packets.
 */

static boolean nhrp_ip_lookup (paktype *pak, void *address, uchar const **macaddr)
{
    return(nhrp_ip_macaddr(pak, *((ipaddrtype *)address), macaddr, FALSE));
}

/*
 * nhrp_ip_checkcache
 * invalidate protocol cache entry for the given destination and mask
 */
static void nhrp_ip_checkcache (nhrpaddr *dest)
{
    ipaddrtype mask, c_mask;
    ipaddrtype ipaddr;

    if (iprouting_cache_mask(dest->ip, &mask, &c_mask)) {
	ipaddr = dest->ip & mask;
	reg_invoke_ipcache_invalidate_subrange(ipaddr, mask, MASK_HOST);
    }
}

/*
 * Provide a handle for all the protocol dependent functions found above,
 * and the protocol dependent constants.
 */

static nhrpfuns nhrp_ipfuns = {nhrp_ip_onnet, nhrp_ip_most_specific,
		    nhrp_ip_most_specific2, nhrp_ip_cmpaddr, nhrp_ip_cmpnet,
		    nhrp_ip_nhop, nhrp_ip_rtlookup,
		    nhrp_ip_ifaddr, nhrp_ip_copymask, nhrp_ip_isbroad,
		    nhrp_ip_ouradd, nhrp_ip_ourifadd, nhrp_ip_lookup,
		    nhrp_ip_checkcache, nhrp_ip_netsmask,
		    ADDRLEN_IP,
		    2*ADDRLEN_IP,
		    ADDR_IP,
		    LINK_IP,
		    TYPE_IP10MB,
		    NHRP_NETIP,
		    TRUE};

/*
 * nhrp_ip_init
 * Perform any protocol dependent initialization.
 */

static void nhrp_ip_init (subsystype *subsys)
{
    nhrp_funtab[NHRP_NETIP] = &nhrp_ipfuns;
    nhrp_ipfuns.protoid = link2ether(nhrp_ipfuns.linktype);
    reg_add_iprouting_macaddr(nhrp_ip_macaddr_wrap, "nhrp_ip_macaddr_wrap");
    reg_add_media_tunnel_bcast(LINK_IP, nhrp_tunnel_bcast,
			       "nhrp_tunnel_bcast");
    reg_add_ip_enqueue(NHRP_PROT, nhrp_enqueue, "nhrp_enqueue");
    nhrp_ip_parser_init();
}

/*
 * Parser support and command processing:
 */

/*
 * nhrp_clear
 * Flush entire cache - dynamic entries only.
 */

void nhrp_ip_clear (parseinfo *csb)
{
    ipaddrtype addr, mask;
    nhrpfuns *fun;

    fun = nhrp_funmap(NHRP_NETIP);
    if (!fun)
	return;
    addr = GETOBJ(paddr,1)->ip_addr;
    mask = GETOBJ(paddr,2)->ip_addr;
    if (!addr)
	nhrp_cache_walk(nhrp_cache_age_subr, fun, NULL, TRUE);
    else
	nhrp_cache_clear_addr(nhrp_addr(&addr), fun);
}

/*
 * show_ip_nhrp
 * NHRP show command processing.
 */

void show_ip_nhrp (parseinfo *csb)
{
    nhrpfuns *fun;

    fun = nhrp_funmap(NHRP_NETIP);
    if (!fun)
	return;
    switch (csb->which) {
    case NHRP_SHOW_TRAFFIC:
	nhrp_show_traffic(GETOBJ(idb,1), fun);
	break;
    case NHRP_SHOW_CACHE:
	nhrp_show_cache(NHRP_NETIP,
			GETOBJ(idb,1), GETOBJ(int,1), GETOBJ(int,2));
	break;
    case NHRP_SHOW_PURGE:
	nhrp_show_purge(fun);
	break;
    default:
	break;
    }
}

/*
 * nhrp_ip_nhs_nvgen
 * Handle NVGENeration of the NHS table.
 */

static void nhrp_ip_nhs_nvgen (uchar *cmd, nhrpnhstype *table)
{
    nhrpnhstype *ptr;

    for (ptr = table; ptr; ptr = ptr->next) {
	nv_write(TRUE, "%s %i", cmd, ptr->nhs.ip);
	if (ptr->net.ip.addr)
	    nv_add(TRUE, " %i %i", ptr->net.ip.addr, ptr->net.ip.mask);
    }
}

/*
 * nhrp_ip_map_nvgen_subr
 * NVGEN an NHRP static mapping based upon a static cache table entry.
 */

static int nhrp_ip_map_nvgen_subr (rntype *rn, va_list ap)
{
    nhrpcachetype *ptr;
    idbtype *swidb;
    uchar *cmd;
    uchar nbma_buf[NHRP_NBMA_MAXSTR];
    nhrpfuns *fun;

    ptr = (nhrpcachetype *)rn;
    fun = va_arg(ap, nhrpfuns *);
    swidb = va_arg(ap, idbtype *);
    cmd = va_arg(ap, uchar *);
    if (ptr->mode != NHRP_STATIC)
	return(0);
    if (ptr->idb != swidb)
	return(0);
    nv_write(TRUE, "%s %i", cmd, ptr->prefix.ip.target);
    if (ptr->prefix.ip.mask != MASK_HOST)
	nv_add(TRUE, " %i", ptr->prefix.ip.mask);
    nv_add(TRUE, " %s", nhrp_sprint_nbma(nbma_buf, ptr->nbma.opaque,
				   ptr->nbma_len, ptr->addr_type));
    return(0);
}

/*
 * nhrp_ip_map_mcast_nvgen
 * nvgen the static map for a particular interface.
 */

static void nhrp_ip_map_mcast_nvgen (idbtype *swidb, uchar *cmd,
			      nhrpproinfotype *ipinfo)
{
    uchar nbma_buf[NHRP_NBMA_MAXSTR];
    nhrpmcasttype *mptr;

    for (mptr = ipinfo->nhrp_mcast; mptr; mptr = mptr->next) {
	nv_write(TRUE, "%s multicast %s", cmd,
		 nhrp_sprint_nbma(nbma_buf, mptr->nbma.opaque,
				  mptr->nbma_len, mptr->addr_type));

    }
}

/*
 * nhrp_ip_if_command
 * Command processing for interface specific NHRP commands.
 */

void nhrp_ip_if_command (parseinfo *csb)
{
    idbtype *idb;
    nhrpinfotype *nhrpinfo;
    nhrpproinfotype *ipinfo;
    nhrpnet net;
    nhrpaddr addr, nhs;
    ushort addr_type, auth_len;
    hwaddrtype *hwaddr;
    nhrphwaddrtype hw;
    uchar addr_len;
    nhrpfuns *fun;
    ipaddrtype mask;

    idb = csb->interface;
    nhrpinfo = idb->nhrp_info;
    if (!nhrpinfo) {
	if (csb->nvgen)
	    return;
	nhrpinfo = malloc_named(NHRP_NETPROTOCNT * sizeof(nhrpproinfotype),
				"NHRP Info");
	if (!nhrpinfo) {
	    printf(nomemory);
	    return;
	}
	idb->nhrp_info = nhrpinfo;
    }
    ipinfo = &nhrpinfo->pro[NHRP_NETIP];
    fun = nhrp_funmap(NHRP_NETIP);
    if (csb->nvgen) {
	switch (csb->which) {
	case NHRP_CMD_AUTH:
	    nv_write((ipinfo->nhrp_auth != NULL), "%s %s",
		     csb->nv_command, ipinfo->nhrp_auth);
	    break;
	case NHRP_CMD_RESP:
	    if (ipinfo->nhrp_respif != NULL)
		nv_write(TRUE, "%s %s",
			 csb->nv_command, ipinfo->nhrp_respif->namestring);
	    break;
	case NHRP_CMD_HOLD:
	    if (ipinfo->nhrp_flags & NHRP_CFG_HOLD)
		nv_write(TRUE, "%s %d %d",
			 csb->nv_command, ipinfo->nhrp_hold_pos,
			 ipinfo->nhrp_hold_neg);
	    break;
	case NHRP_CMD_RECORD:
	    if (ipinfo->nhrp_flags & NHRP_CFG_NOREC)
		nv_write(TRUE, "no %s", csb->nv_command);
	    break;
	case NHRP_CMD_INTEREST:
	    nv_write(ipinfo->nhrp_req_acl,
		     "%s %d", csb->nv_command, ipinfo->nhrp_req_acl);
	    break;
	case NHRP_CMD_NET:
	    nv_write(ipinfo->nhrp_net_id != NHRP_NETID_UNDEF,
		     "%s %d", csb->nv_command, ipinfo->nhrp_net_id);
	    break;
	case NHRP_CMD_NHS:
	    nhrp_ip_nhs_nvgen(csb->nv_command, ipinfo->nhrp_nhstable);
	    break;
	case NHRP_CMD_MAP:
	    addr_type = nhrp_nbma_type(idb);
	    if (addr_type == AF_RESERVED)
		return;
	    nhrp_cache_walk(nhrp_ip_map_nvgen_subr, fun,
			    idb, csb->nv_command);
	    nhrp_ip_map_mcast_nvgen(idb, csb->nv_command, ipinfo);
	    break;
	case NHRP_CMD_USE:
	    if (!(ipinfo->nhrp_flags & NHRP_IF_INIT))
		return;
	    nv_write(ipinfo->nhrp_use != NHRP_USE_DEF,
		     "%s %d", csb->nv_command, ipinfo->nhrp_use);
	    break;
	case NHRP_CMD_MAX:
	    if (!(ipinfo->nhrp_flags & NHRP_IF_INIT))
		return;
	    nv_write((ipinfo->nhrp_interval != NHRP_IF_RATE_IVL) ||
		     (ipinfo->nhrp_pkt_cnt != NHRP_MAX_PKT), "%s %d every %d",
		     csb->nv_command, ipinfo->nhrp_pkt_cnt,
		     ipinfo->nhrp_interval);
	    break;
	}
	return;
    }
    if (csb->sense && !(ipinfo->nhrp_flags & NHRP_IF_INIT))
	nhrp_if_defaults(idb, ipinfo);
    switch (csb->which) {
    case NHRP_CMD_AUTH:
	if (csb->sense) {
	    if (ipinfo->nhrp_auth)
		free(ipinfo->nhrp_auth);
	    auth_len = strlen(GETOBJ(string,1));
	    if (auth_len > NHRP_AUTHLEN_MAX) {
		printf(nomemory);
		return;
	    }
	    ipinfo->nhrp_auth = malloc(auth_len + 1);
	    if (ipinfo->nhrp_auth == NULL) {
		printf(nomemory);
		return;
	    }
	    sstrncpy(ipinfo->nhrp_auth, GETOBJ(string,1), auth_len + 1);
	} else {
	    if (ipinfo->nhrp_auth)
		free(ipinfo->nhrp_auth);
	    ipinfo->nhrp_auth = NULL;
	}
	break;
    case NHRP_CMD_RESP:
	if (csb->sense) {
	    ipinfo->nhrp_respif = GETOBJ(idb,1);
	} else {
	    ipinfo->nhrp_respif = NULL;
	}
	break;
    case NHRP_CMD_HOLD:
	if (csb->sense) {
	    ipinfo->nhrp_flags |= NHRP_CFG_HOLD;
	    ipinfo->nhrp_hold_pos = GETOBJ(int,1);
	    ipinfo->nhrp_hold_neg = GETOBJ(int,2);
	    if (ipinfo->nhrp_hold_neg == 0)
		ipinfo->nhrp_hold_neg = NHRP_DEF_HOLDTIME;
	} else {
	    ipinfo->nhrp_flags &= ~NHRP_CFG_HOLD;
	    ipinfo->nhrp_hold_pos = NHRP_DEF_HOLDTIME;
	    ipinfo->nhrp_hold_neg = NHRP_DEF_HOLDTIME;
	}
	break;
    case NHRP_CMD_RECORD:
	if (csb->set_to_default || csb->sense) {
	    ipinfo->nhrp_flags &= ~NHRP_CFG_NOREC;
	} else {
	    ipinfo->nhrp_flags |= NHRP_CFG_NOREC;
	}
	break;
    case NHRP_CMD_INTEREST:
	if (csb->sense)
	    ipinfo->nhrp_req_acl = GETOBJ(int,1);
	else
	    ipinfo->nhrp_req_acl = 0;
	break;
    case NHRP_CMD_NET:
	if (csb->sense) {
	    if (ipinfo->nhrp_net_id != GETOBJ(int,1)) {
		/*
		 * Set time to send registration for next period.
		 */
		ipinfo->nhrp_reg_hold = ipinfo->nhrp_hold_pos;
	    }
	    ipinfo->nhrp_net_id = GETOBJ(int,1);
	    nhrp_cache_init(&ipinfo->nhrp_head);
	    nhrp_process_start();
	    nhrp_create_pdb();
	} else {
	    nhrp_cache_if_delete(idb);
	    if (ipinfo->nhrp_net_id != NHRP_NETID_UNDEF) {
		/*
		 * Send purge request for previous registration.
		 */
		nhrp_send_purge_all(ipinfo, fun);
	    }
	    ipinfo->nhrp_net_id = NHRP_NETID_UNDEF;
	    nhrp_process_kill();
	}
	break;
    case NHRP_CMD_NHS:
	net.ip.addr = GETOBJ(paddr,2)->ip_addr;
	net.ip.mask = GETOBJ(paddr,3)->ip_addr;
	if (net.ip.addr && !good_route_mask(net.ip.addr, net.ip.mask,
					    subnetzero_enable)) {
	    printf("\nNHRP: Network %i mask %i - inconsistent",
		   net.ip.addr, net.ip.mask);
	    return;
	}
	net.ip.addr = net.ip.addr & net.ip.mask;
	nhs.ip = GETOBJ(paddr,1)->ip_addr;
	nhrp_update_nhs(idb, &nhs, &net, csb->sense, fun);
	break;
    case NHRP_CMD_MAP:
	addr_type = nhrp_nbma_type(idb);
	if (addr_type == AF_RESERVED)
	    return;
	addr_len = nhrp_af_len(addr_type);
	switch (addr_type) {
	case AF_IP:
	    hw.nbma.ip_address = GETOBJ(paddr,2)->ip_addr;
	    hw.nbma_len = addr_len;
	    break;
	case AF_E164:
	case AF_NSAP:
	case AF_802:
	    hwaddr = GETOBJ(hwaddr,1);
	    bcopy(hwaddr->addr, hw.nbma.opaque, addr_len);
	    hw.nbma_len = addr_len;
	    break;
	default:
	    printf("\nNHRP not yet supported for media type on %s",
		   idb->namestring);
	    return;
	}
	hw.addr_type = addr_type;
	hw.flags = NHRP_AFLAG|NHRP_PFLAG;
	if (GETOBJ(int,1)) {
	    nhrp_map_mcast(idb, &hw, csb->sense, fun);
	} else {
	    nhrp_cache_init(&ipinfo->nhrp_head);
	    addr.ip = GETOBJ(paddr,1)->ip_addr;
	    if (GETOBJ(int,10)) {
		mask = MASK_HOST;
	    } else {
		mask = GETOBJ(paddr,3)->ip_addr;
                /*
		 * Sanity check mask against address
		 */
		if (addr.ip && 
		    !good_route_mask(addr.ip, mask, subnetzero_enable)) {
		    printf("\nNHRP: Network %i mask %i - inconsistent",
			   addr.ip, mask);
		    return;
		}
	    }
	    nhrp_map_command(idb, &addr, &mask, &hw, csb->sense, fun);
	}
	break;
    case NHRP_CMD_USE:
	if (csb->sense)
	    ipinfo->nhrp_use = GETOBJ(int,1);
	else
	    ipinfo->nhrp_use = NHRP_USE_DEF;
	break;
    case NHRP_CMD_MAX:
	if (csb->sense) {
	    ipinfo->nhrp_pkt_cnt = GETOBJ(int,1);
	    ipinfo->nhrp_interval = GETOBJ(int,2);
	} else {
	    ipinfo->nhrp_pkt_cnt = NHRP_MAX_PKT;
	    ipinfo->nhrp_interval = NHRP_IF_RATE_IVL;
	}
	break;
    }
}

/* 
 * ipnhrp subsystem header
 */

#define IPNHRP_MAJVERSION 1
#define IPNHRP_MINVERSION 0
#define IPNHRP_EDITVERSION 1

SUBSYS_HEADER(ipnhrp, IPNHRP_MAJVERSION, IPNHRP_MINVERSION, IPNHRP_EDITVERSION,
	      nhrp_ip_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: nhrp",
	      "req: nhrp");
