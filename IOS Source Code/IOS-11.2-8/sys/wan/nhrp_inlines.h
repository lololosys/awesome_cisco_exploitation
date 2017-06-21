/* $Id: nhrp_inlines.h,v 3.2.64.1 1996/04/25 23:24:28 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/wan/nhrp_inlines.h,v $
 *------------------------------------------------------------------
 * Next Hop Resolution Protocol.
 *
 * October 1995, Bruce Cole
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nhrp_inlines.h,v $
 * Revision 3.2.64.1  1996/04/25  23:24:28  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.2.68.1  1996/03/24  04:05:52  rzagst
 * ATM modularity: Remove AIP from all files that are not referring to the
 * AIP i/f
 * Branch: ATM_Cal_branch
 *
 * Revision 3.2  1995/11/17  18:03:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:50:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:36:22  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  21:55:04  bcole
 * Placeholder files for 11.1 NHRP work.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * nhrp_hash
 * Hash an NHRP next hop address, for cache lookup purposes.
 */

static inline ulong nhrp_hash (ipaddrtype addr)
{
    return(nethash(addr));
}

/*
 * nhrp_af_len
 * Map an address family to its (maximum) length, in bytes
 */

static inline uchar nhrp_af_len (ushort addr_type)
{
    switch (addr_type) {
    case AF_IP:
	return(sizeof(ipaddrtype));
    case AF_E164:
	return(STATIONLEN_SMDS64);
    case AF_NSAP:
	return(STATIONLEN_ATMNSAP);
    case AF_802:
	return(IEEEBYTES);
    default:
	return(0);
    }
}

/*
 * nhrp_netid
 * Determine the NHRP network identifier for this interface
 */

static inline ulong nhrp_netid (idbtype *idb, nhrpfuns *fun)
{
    nhrpinfotype *nhrpinfo;

    nhrpinfo = idb->nhrp_info;
    if (!nhrpinfo)
	return(NHRP_NETID_UNDEF);
    return(nhrpinfo->pro[fun->protoidx].nhrp_net_id);
}

/*
 * nhrp_if
 * Determine if the interface is configured for NHRP
 */

static inline boolean nhrp_if (idbtype *idb, nhrpfuns *fun)
{
    nhrpinfotype *nhrpinfo;

    nhrpinfo = idb->nhrp_info;
    if (!nhrpinfo)
	return(FALSE);
    return(nhrpinfo->pro[fun->protoidx].nhrp_net_id != NHRP_NETID_UNDEF);
}

/*
 * nhrp_on_if
 * Is NHRP minimally configured on this interface at all?
 */

static inline boolean nhrp_on_if (idbtype *idb)
{
    nhrpfuns *fun;
    ushort i;

    for (i = 0; i < NHRP_NETPROTOCNT; i++) {
	fun = nhrp_funmap(i);
	if (!fun)
	    continue;
	if (nhrp_if(idb, fun))
	    return(TRUE);
    }
    return(FALSE);
}

/*
 * nhrp_toomany
 * Determine if it is OK to send another NHRP packet out this interface.
 */

static inline boolean nhrp_toomany (idbtype *swidb, nhrpfuns *fun)
{
    nhrpinfotype *nhrpinfo;
    
    nhrpinfo = swidb->nhrp_info;
    if (!nhrpinfo)
	return(FALSE);
    if (nhrpinfo->pro[fun->protoidx].nhrp_quota < 1) {
	if (nhrp_debug_rate)
	    buginf("\nNHRP-RATE: %s: Quota reached", swidb->namestring);
	return(TRUE);
    }
    return(FALSE);
}

/*
 * nhrp_addr
 * Cast a pointer to an IP address into an NHRP address
 */

static inline nhrpaddr *nhrp_addr (ipaddrtype *addr)
{
    return((nhrpaddr *)addr);
}

/*
 * nhrpfixstart
 * Given a packet, determine the start of the NHRP fixed length header
 */

static inline void *nhrpfixstart (paktype *pak)
{
    if (pak->linktype == LINK_IP)
	return((void *)(ipdatastart(pak)));
    else
	return((void *)pak->network_start);
}

/*
 * nhrp_pdulen
 * Determine size of NHRP packet, ignoring its encapsulation
 */

static inline ushort nhrp_pdulen (paktype *pak)
{
    iphdrtype *ip;

    if (pak->linktype == LINK_IP) {
	ip = iphdrtype_start(pak);
	return(ip->tl - ipheadsize(ip));
    } else {
	return(pak->datagramsize - (pak->network_start - pak->datagramstart));
    }
}

/*
 * nhrp_eop
 * Calculate a pointer to the end of the NHRP packet.
 */

static inline uchar *nhrp_eop (paktype *pak)
{
    iphdrtype *ip;

    if (pak->linktype == LINK_IP) {
	ip = iphdrtype_start(pak);
	return((uchar *)ip + ip->tl);
    } else {
	return(pak->datagramstart + pak->datagramsize);
    }
}

/*
 * Various routines to find the proper location of fields within NHRP packets,
 * taking into account protocol dependent address fields (of varying length).
 */

/*
 * nhrp_rep2nbma
 * Find NBMA address in reply packet
 */

static inline void *nhrp_rep2nbma (nhrpreqreptype *req, nhrpfuns *fun)
{
    return(req->addrs + 3*NHRP_ALIGN(fun->nhrp_addlen));
}

/*
 * nhrp_req2nbma
 * Find NBMA address info in a request packet
 */

static inline nhrpnhoptype *nhrp_req2nbma (nhrpreqreptype *req, nhrpfuns *fun)
{
    return((nhrpnhoptype *)(req->addrs + 2*NHRP_ALIGN(fun->nhrp_addlen)));
}

/*
 * nhrp_repnhop
 * Find next hop info in request/reply packet
 */

static inline void *nhrp_repnhop (nhrpreqreptype *req, nhrpfuns *fun)
{
    return(req->addrs + 2*NHRP_ALIGN(fun->nhrp_addlen));
}

/*
 * nhrp_reg2nbma
 * Find NBMA address info in a register packet
 */

static inline void *nhrp_reg2nbma (nhrpregtype *reg, nhrpfuns *fun)
{
    return(reg->addr + NHRP_ALIGN(fun->nhrp_addlen));
}

/*
 * nhrp_regsrc
 * Find network layer src address in a register packet
 */

static inline void *nhrp_regsrc (nhrpregtype *reg, nhrpfuns *fun)
{
    return(reg->addr);
}

/*
 * nhrp_pursrc
 * Find network layer src address in a purge packet
 */

static inline void *nhrp_pursrc (nhrpreqreptype *req, nhrpfuns *fun)
{
    return(req->addrs + 2 * NHRP_ALIGN(fun->nhrp_addlen));
}

/*
 * nhrp_reqsrc
 * Find network layer src address in a request/reply packet
 */

static inline void *nhrp_reqsrc (nhrpreqreptype *req, nhrpfuns *fun)
{
    return(req->addrs + NHRP_ALIGN(fun->nhrp_addlen));
}

/*
 * nhrp_reqdst
 * Find network layer dst address in a request/reply packet
 */

static inline void *nhrp_reqdst (nhrpreqreptype *req, nhrpfuns *fun)
{
    return(req->addrs);
}

/*
 * nhrp_cache_keypos
 * Dynamically determine position of net mask length field
 */

static inline uchar *nhrp_cache_keypos (nhrpaddrnet *net, uchar len)
{
    return(((uchar *)&net->gen.network_key) + NHRP_ALIGN(len + 1));
}
