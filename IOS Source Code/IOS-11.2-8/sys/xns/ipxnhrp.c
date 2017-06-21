/* $Id: ipxnhrp.c,v 3.5.10.4 1996/06/17 23:39:35 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xns/ipxnhrp.c,v $
 *------------------------------------------------------------------
 * Next Hop Resolution Protocol.
 *
 * October 1995, Bruce Cole
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipxnhrp.c,v $
 * Revision 3.5.10.4  1996/06/17  23:39:35  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.5.10.3  1996/04/10  04:23:09  bcole
 * CSCdi54192:  IPX pings over a multipoint GRE tunnel fail intermittently
 * Branch: California_branch
 *
 * Fixup IPX encapsulation over multipoint tunnels.
 *
 * Revision 3.5.10.2  1996/03/28  03:31:34  bcole
 * CSCdi51494:  NHRP: Prefix entries not appearing in cache in many cases
 * Branch: California_branch
 *
 * Bring 11.1 fixes forward...
 *
 * Revision 3.5.10.1  1996/03/18  22:52:46  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/16  07:59:11  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.26.2  1996/03/07  11:21:38  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:59:47  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/22  00:06:48  bcole
 * CSCdi47623:  NHRP overwrites page zero
 *
 * Add extra NULL pointer checks.  Avoid mallocing NHRP info when not
 * necessary.
 *
 * Revision 3.4  1996/02/20  21:18:23  bcole
 * CSCdi49357:  NHRP: Problems with debug ip nhrp, cache flushing
 *
 * Repair debug command backwards compatibility.  Fix ordering problem
 * when flushing cache during deconfiguration.
 *
 * Revision 3.3  1996/02/01  06:13:26  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.2  1995/11/17  19:22:06  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:58:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:38:39  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  21:55:07  bcole
 * Placeholder files for 11.1 NHRP work.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
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
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../clns/isis_ipx.h"
#include "ipx_registry.h"
#include "../util/radix.h"
#include "../wan/nhrp_debug.h"
#include "../wan/nhrp.h"
#include "../wan/nhrp_public.h"
#include "../wan/nhrp_inlines.h"
#include "../wan/parser_defs_nhrp.h"
#include "../wan/nhrp_vc.h"
#include "../iprouting/ipnhrp.h"
#include "novell.h"
#include <ciscolib.h>
#include "media_registry.h"

/*
 * nhrp_ipx_rtlookup
 * Perform a network layer routing table lookup on behalf of NHRP.
 */

static idbtype *nhrp_ipx_rtlookup (nhrpaddr *dest, nhrp_mask *mask,
				   void **opaque, boolean * static_route)
{
    nidbtype *nif_output;
    uchar *dest_out;
    ulong flags;
    novndbtype *p;

    p = (*novell_table)[NOV_NETHASH(dest->ipx.net)];
    dest_out = novrip_first_hop(dest->ipx.net, dest->ipx.host,
				&nif_output, &flags, p, IPX_ROUND_ROBIN_YES,
				IPX_DEFAULT_ROUTE_YES);
    *static_route = FALSE;
    *opaque = NULL;
    *mask = NULL;
    return(nif_output->idb);
}

/*
 * nhrp_ipx_netsmask
 * Return NULL to use hostmask.
 */

static nhrp_mask nhrp_ipx_netsmask (nhrpaddr *dest, boolean *static_route)
{
    return(NULL);
}

/*
 * nhrp_ipx_onnet
 * Return TRUE if 'addr' is part of the network described by 'net'.
 */

static boolean nhrp_ipx_onnet (nhrpnet *net, nhrpaddr *addr)
{
    return(net->ipx == addr->ipx.net);
}

/*
 * nhrp_ipx_most_specific
 * Return TRUE if a is more specific than b
 * For IPX, we don't consider the addresses to be hierarchical, so the
 * answer is always false.
 */

static boolean nhrp_ipx_most_specific (nhrpnet *a, nhrpnet *b)
{
    return(FALSE);
}

/*
 * nhrp_ipx_most_specific2
 * Return TRUE if a is more specific than b
 * For IPX, we don't consider the addresses to be hierarchival, so the
 * answer is always false.
 */

static boolean nhrp_ipx_most_specific2 (uchar *a, uchar *b)
{
    return(FALSE);
}

/*
 * nhrp_ipx_cmpaddr
 * Return non-zero if network address different, zero if same
 */

static boolean nhrp_ipx_cmpaddr (nhrpaddr *a, nhrpaddr *b)
{
    if ((a->ipx.net == b->ipx.net) && ieee_equal(a->ipx.host, b->ipx.host))
	return(0);
    return(1);
}

/*
 * nhrp_ipx_cmpnet
 * Return non-zero if IPX network #s are different, zero if same
 */

static int nhrp_ipx_cmpnet (nhrpnet *a, nhrpnet *b)
{
    if (a->ipx == b->ipx)
	return(0);
    return(1);
}

/*
 * nhrp_ipx_nhop
 * Determine the next hop if for a particular destination.
 * 'Modify' indicates whether or not the dest argument should be modified
 * to contain the next hop network layer address.
 */

static idbtype *nhrp_ipx_nhop (paktype *pak, nhrpaddr *dest, boolean modify)
{
    nidbtype *nif_output;
    uchar *dest_out;
    ulong flags;
    novndbtype *p;

    p = (*novell_table)[NOV_NETHASH(dest->ipx.net)];
    dest_out = novrip_first_hop(dest->ipx.net, dest->ipx.host,
				&nif_output, &flags, p, IPX_ROUND_ROBIN_YES,
				IPX_DEFAULT_ROUTE_YES);
    if (!nif_output || !dest_out)
	return(NULL);
    if (modify) {
	bcopy(dest_out, dest->ipx.host, IPX_HOST_BYTES);
	pak->nif_output = nif_output;
    }
    return(nif_output->idb);
}

/*
 * nhrp_ipx_ifaddr
 * Determine our IPX address for use in the NHRP header of packets which
 * are locally generated.
 */

static nhrpaddr *nhrp_ipx_ifaddr (idbtype *idb)
{
    static nhrpaddr tmp;
    nidbtype *nidb;

    nidb = IPX_PRIMARY_NIDB(idb);
    tmp.ipx.net = nidb ? nidb->novellnet : 0;
    ieee_copy(idb->novell_node, tmp.ipx.host);
    return(&tmp);
}

/*
 * nhrp_ipx_copymask
 * For IPX, this is a noop, as we do not store netmask information
 * in the cache (flat address space).
 */

static void nhrp_ipx_copymask (nhrp_mask mask, nhrpcachetype *ptr)
{
}

/*
 * nhrp_ipx_isbroad
 * Determine if the network layer destination address can be considered
 * a broadcast on this interface.
 */

static boolean nhrp_ipx_isbroad (nhrpaddr *dst, idbtype *swidb)
{
    if (ieee_equal(dst->ipx.host, novell_allhosts))
	return(TRUE);
    return(FALSE);
}

/*
 * nhrp_ipx_ourifadd_subr
 * Determine if the given address is a locally assigned address for a
 * particular nidb.
 */

static boolean nhrp_ipx_ourifadd_subr (nidbtype *nidb, ulong dnet, uchar *host)
{
    if (dnet != 0L && dnet != -1L && dnet != nidb->novellnet)
	return(FALSE);
    return(ieee_equal(host, nidb->idb->novell_node));
}

/*
 * nhrp_ipx_ouradd
 * Determine if the network layer address is one of the locally assigned
 * addresses.
 */

static boolean nhrp_ipx_ouradd (nhrpaddr *addr)
{
    ulong dnet;
    nidbtype *nidb;

    dnet = addr->ipx.net;
    FOR_ALL_NIDBS(nidb) {
	if (nhrp_ipx_ourifadd_subr(nidb, dnet, addr->ipx.host))
	    return(TRUE);
    }
    return(FALSE);
}

/*
 * nhrp_ipx_ourifadd
 * Determine if the network layer address is one of the locally assigned
 * addresses - on a particular interface.
 */

static boolean nhrp_ipx_ourifadd (idbtype *swidb, nhrpaddr *addr)
{
    ulong dnet;
    nidbtype *nidb;

    dnet = addr->ipx.net;
    nidb = IPX_PRIMARY_NIDB(swidb);
    return(nhrp_ipx_ourifadd_subr(nidb, dnet, addr->ipx.host));
}

/*
 * nhrp_ipx_macaddr
 * Attempt to resolve nhop's NBMA address.  Called by link layer specific
 * encapsulation code.
 */

boolean nhrp_ipx_macaddr (paktype *pak, long address, uchar const **macaddr,
			  boolean checkfin)
{
    ipxhdrtype *ipx;
    idbtype *if_out, *if_in;
    nhrpaddr dest, realdest, *destp;
    ushort addr_type;
    uchar netid_in, netid_out;
    boolean nhop_success, is_nhrp;
    nhrpfuns *fun;
    nhrpinfotype *nhrpinfo;
    uchar const *nhop_host;

    nhop_host = (uchar const *)address;
    if (is_ieee_bcast(nhop_host))
	return(FALSE);
    fun = nhrp_funmap(NHRP_NETIPX);
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
    /*
     * Detect recursive encapsulation attempts.
     */
    if (pak->linktype == LINK_NHRP) {
	dest.ipx.net = IPX_PRIMARY_NIDB(if_out)->novellnet;
	ieee_copy(nhop_host, dest.ipx.host);
	is_nhrp = TRUE;
	destp = &dest;
    } else {
	ipx = (ipxhdrtype *)ipxheadstart(pak);
	realdest.ipx.net = GET2WD(ipx->ddnet);
	ieee_copy(ipx->dhost, realdest.ipx.host);
	is_nhrp = FALSE;
	destp = &realdest;
    }
    nhop_success = FALSE;
    if (checkfin) {
	/*
	 * First attempt to resolve final network layer destination
	 */
	nhop_success = nhrp_resolve_nhop(if_out, destp,
					 macaddr, addr_type, fun);
	if (nhop_success)
	    return(TRUE);
    }
    if ((destp != &dest) || !checkfin) {
	if ((destp->ipx.net != IPX_PRIMARY_NIDB(if_out)->novellnet) ||
	    !ieee_equal(destp->ipx.host, nhop_host)) {
	    dest.ipx.net = IPX_PRIMARY_NIDB(if_out)->novellnet;
	    ieee_copy(nhop_host, dest.ipx.host);
	    /*
	     * Cannot resolve final destination.  At least try to resolve next
	     * hop.
	     */
	    nhop_success = nhrp_resolve_nhop(if_out, &dest,
					     macaddr, addr_type, fun);
	}
    }
    /*
     * Looks like we might want to send an NHRP request.
     * Only forwarding from outside the fabric to within the fabric
     * should cause NHRP requests.  (Locally generated traffic should
     * be considered to come from outside the fabric).
     * Also, don't send NHRP requests based upon NHRP packets
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
    if (nhrpinfo && nhrpinfo->pro[NHRP_NETIPX].nhrp_req_acl) {
	if (!nov_accesscheck(pak, nhrpinfo->pro[NHRP_NETIPX].nhrp_req_acl)) {
	    if (nhrp_debug)
		buginf("\nNHRP: Packet rejected by acl");
	    return(nhop_success);
	}
    }
    if (fun == NULL)
	return(nhop_success);
    nhrp_req_enq(if_out, destp, FALSE, fun);
    return(nhop_success);
}

/*
 * nhrp_ipx_macaddr_wrap
 */

static boolean nhrp_ipx_macaddr_wrap (paktype *pak, long address,
				      uchar const **macaddr)
{
    return(nhrp_ipx_macaddr(pak, address, macaddr, TRUE));
}

/*
 * nhrp_ipx_lookup
 * Protocol independent interface to determine next hop link layer address
 * for use in forwarding of NHRP packets.
 */

static boolean nhrp_ipx_lookup (paktype *pak, void *address, uchar const **macaddr)
{
    uchar *ipxhost;
    
    /*
     * Skip over IPX network #
     */
    ipxhost = (uchar *)address + sizeof(ulong);
    return(nhrp_ipx_macaddr(pak, (ulong)ipxhost, macaddr, FALSE));
}

/*
 * nhrp_ipx_checkcache
 * invalidate protocol cache entry for the given destination and mask
 */

static void nhrp_ipx_checkcache (nhrpaddr *dest)
{
    /* placeholder */
}

/*
 * Provide a handle for all the protocol dependent functions found above,
 * and the protocol dependent constants.
 */

static nhrpfuns nhrp_ipxfuns = {nhrp_ipx_onnet, nhrp_ipx_most_specific,
		    nhrp_ipx_most_specific2, nhrp_ipx_cmpaddr, nhrp_ipx_cmpnet,
		    nhrp_ipx_nhop, nhrp_ipx_rtlookup,
		    nhrp_ipx_ifaddr, nhrp_ipx_copymask, nhrp_ipx_isbroad,
		    nhrp_ipx_ouradd, nhrp_ipx_ourifadd, nhrp_ipx_lookup,
		    nhrp_ipx_checkcache, nhrp_ipx_netsmask,
		    ADDRLEN_NOVELL,
		    ADDRLEN_NOVELL,
		    ADDR_NOVELL,
		    LINK_NOVELL,
		    TYPE_NOVELL1,
		    NHRP_NETIPX,
		    FALSE};

/*
 * nhrp_ipx_init
 * Perform any protocol dependent initialization.
 */

static void nhrp_ipx_init (subsystype *subsys)
{
    nhrp_funtab[NHRP_NETIPX] = &nhrp_ipxfuns;
    nhrp_ipxfuns.protoid = link2ether(nhrp_ipxfuns.linktype);
    reg_add_ipxnhrp_macaddr(nhrp_ipx_macaddr_wrap, "nhrp_ipx_macaddr_wrap");
    reg_add_ipxnhrp_macaddr_map(nhrp_macaddr_map, "nhrp_macaddr_map");
    reg_add_media_tunnel_bcast(LINK_NOVELL, nhrp_tunnel_bcast,
			       "nhrp_tunnel_bcast");
    nhrp_ipx_parser_init();
}
/*
 * Parser support and command processing:
 */

/*
 * nhrp_clear
 * Flush entire cache - dynamic entries only.
 */

void nhrp_ipx_clear (parseinfo *csb)
{
    nhrpaddr addr;
    nhrpfuns *fun;

    fun = nhrp_funmap(NHRP_NETIPX);
    if (!fun)
	return;
    if (GETOBJ(paddr,1)->length == 0) {
	nhrp_cache_walk(nhrp_cache_age_subr, fun, NULL, TRUE);
    } else {
	ieee_copy(GETOBJ(paddr,1)->nov_addr.host, addr.ipx.host);
	addr.ipx.net = GETOBJ(paddr,1)->nov_addr.net;
	nhrp_cache_clear_addr(&addr, fun);
    }
}

/*
 * show_ipx_nhrp
 * NHRP show command processing.
 */

void show_ipx_nhrp (parseinfo *csb)
{
    nhrpfuns *fun;

    fun = nhrp_funmap(NHRP_NETIPX);
    if (!fun)
	return;
    switch (csb->which) {
    case NHRP_SHOW_TRAFFIC:
	nhrp_show_traffic(GETOBJ(idb,1), fun);
	break;
    case NHRP_SHOW_CACHE:
	nhrp_show_cache(NHRP_NETIPX,
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
 * nhrp_ipx_nhs_nvgen
 * Handle NVGENeration of the NHS table.
 */

static void nhrp_ipx_nhs_nvgen (uchar *cmd, nhrpnhstype *table)
{
    nhrpnhstype *ptr;

    for (ptr = table; ptr; ptr = ptr->next) {
	nv_write(TRUE, "%s %i", cmd, ptr->nhs.ip);
	if (ptr->net.ip.addr)
	    nv_add(TRUE, " %i %i", ptr->net.ip.addr, ptr->net.ip.mask);
    }
}

/*
 * nhrp_map_nvgen_subr
 * NVGEN an NHRP static mapping based upon a static cache table entry.
 */

static int nhrp_ipx_map_nvgen_subr (rntype *rn, va_list ap)
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
    nv_write(TRUE, "%s %x.%e %s", cmd, ptr->prefix.ipx.net,
	     ptr->prefix.ipx.host,
	     nhrp_sprint_nbma(nbma_buf, ptr->nbma.opaque,
			      ptr->nbma_len, ptr->addr_type));
    return(0);
}

/*
 * nhrp_ipx_map_mcast_nvgen
 * nvgen the static map for a particular interface.
 */

static void nhrp_ipx_map_mcast_nvgen (idbtype *swidb, uchar *cmd,
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
 * nhrp_ipx_if_command
 * Command processing for interface specific NHRP commands.
 */

void nhrp_ipx_if_command (parseinfo *csb)
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
    ipinfo = &nhrpinfo->pro[NHRP_NETIPX];
    fun = nhrp_funmap(NHRP_NETIPX);
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
	    nhrp_ipx_nhs_nvgen(csb->nv_command, ipinfo->nhrp_nhstable);
	    break;
	case NHRP_CMD_MAP:
	    addr_type = nhrp_nbma_type(idb);
	    if (addr_type == AF_RESERVED)
		return;
	    nhrp_cache_walk(nhrp_ipx_map_nvgen_subr, fun,
			    idb, csb->nv_command);
	    nhrp_ipx_map_mcast_nvgen(idb, csb->nv_command, ipinfo);
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
	    ieee_copy(GETOBJ(paddr,1)->nov_addr.host, addr.ipx.host);
	    addr.ipx.net = GETOBJ(paddr,1)->nov_addr.net;
	    nhrp_map_command(idb, &addr, NULL, &hw, csb->sense, fun);
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
 * ipxnhrp subsystem header
 */

#define IPXNHRP_MAJVERSION 1
#define IPXNHRP_MINVERSION 0
#define IPXNHRP_EDITVERSION 1

SUBSYS_HEADER(ipxnhrp, IPXNHRP_MAJVERSION, IPXNHRP_MINVERSION,
	      IPXNHRP_EDITVERSION,
	      nhrp_ipx_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: nhrp",
	      "req: nhrp");
