/* $Id: nhrp.c,v 3.9.10.4 1996/07/23 13:31:46 thille Exp $
 * $Source: /release/112/cvs/Xsys/wan/nhrp.c,v $
 *------------------------------------------------------------------
 * Next Hop Resolution Protocol.
 *
 * October 1995, Bruce Cole
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nhrp.c,v $
 * Revision 3.9.10.4  1996/07/23  13:31:46  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.9.10.3  1996/04/10  04:23:05  bcole
 * CSCdi54192:  IPX pings over a multipoint GRE tunnel fail intermittently
 * Branch: California_branch
 *
 * Fixup IPX encapsulation over multipoint tunnels.
 *
 * Revision 3.9.10.2  1996/03/28  03:31:26  bcole
 * CSCdi51494:  NHRP: Prefix entries not appearing in cache in many cases
 * Branch: California_branch
 *
 * Bring 11.1 fixes forward...
 *
 * Revision 3.9.10.1  1996/03/18  22:48:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.3  1996/03/16  07:55:54  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.2.2  1996/03/07  11:07:53  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/02/20  21:52:27  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/02/22  00:06:46  bcole
 * CSCdi47623:  NHRP overwrites page zero
 *
 * Add extra NULL pointer checks.  Avoid mallocing NHRP info when not
 * necessary.
 *
 * Revision 3.8  1996/02/01  06:12:22  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.7  1996/01/26  20:50:25  bcole
 * CSCdi47574: NHRP: Unconfiguring, then reconfiguring gives mgd timer
 * warning
 *
 * Stop our master managed timer when terminating process.
 * Only create an IP routing pdb for NHRP when necessary.
 *
 * Revision 3.6  1996/01/26  01:53:53  bcole
 * CSCdi47572: spurious alignment in nhrp when if link layer address not
 * configured
 *
 * Protect against configuration where NHRP is configured, but the
 * interface's link layer address is not.
 *
 * Revision 3.5  1996/01/22  07:45:50  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1996/01/18  15:55:48  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.3  1996/01/11  07:06:32  bcole
 * CSCdi46724:  NHRP: Provide for migration from RFC1577
 *
 * Glean NHRP information from ATM ARP server.
 *
 * Revision 3.2  1996/01/03  23:32:49  bcole
 * CSCdi46376:  NHRP: Finish post branch split file moves
 *
 * Revision 3.1  1995/11/09  13:50:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/06  21:55:02  bcole
 * Placeholder files for 11.1 NHRP work.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "config.h"
#include "logger.h"
#include "subsys.h"
#include "parser.h"
#include "packet.h"
#include "sys_registry.h"
#include "../os/signal.h"
#include "../if/network.h"
#include "../ip/ip.h"
#include "../ui/debug.h"
#include "../if/ether.h"
#include "../ip/ip_registry.h"
#include "../util/radix.h"
#include "nhrp_debug.h"
#include "nhrp.h"
#include "nhrp_public.h"
#include "nhrp_inlines.h"
#include "nhrp_vc.h"
#include "parser_defs_nhrp.h"
#include "../ip/tunnel.h"
#include "mgd_timers.h"
#include "../iprouting/ipnhrp.h"
#include "media_registry.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */

/*
 * Storage
 */
static ulong nhrp_req_id;		/* Next request ID to use */
static watched_queue *nhrp_packetQ;	/* Incoming packet queue */
static watched_queue *nhrp_eventQ;	/* Event queue */
static int nhrp_pid;			/* NHRP process id */
static mgd_timer nhrp_master_t;		/* Master managed timer */
static mgd_timer nhrp_periodic_t;	/* Periodic timer */
nhrpfuns *nhrp_funtab[NHRP_NETPROTOCNT];

/*
 * nhrp_funmap
 * Map a protocol type to its function table.
 */

nhrpfuns *nhrp_funmap (uchar idx)
{
    return(nhrp_funtab[idx]);
}

/*
 * nhrp_proto2idx
 * Map an IEEE protocol number to our internal enumerated type
 */

uchar nhrp_proto2idx (ushort protoid)
{
    switch (protoid) {
    default:
    case TYPE_IP10MB:
	return(0);
    case TYPE_ETHERTALK:
	return(1);
    case TYPE_NOVELL1:
	return(2);
    }
}

/*
 * nhrp_extractmask
 * Given opaque netmask information from the protocol dependent code,
 * determine what network mask to use in the NHRP packets.
 */

ulong nhrp_extractmask (nhrp_mask mask)
{
    if (mask)
	return(*((ulong *)mask));
    else
	return(MASK_HOST);
}

/*
 * nhrp_htoa
 * Convert a nibble of an NSAP address to ASCII.
 */

static uchar nhrp_htoa (char c)
{
   if ( (c >= 0) && (c <= 9) )
      return (c + '0');
   else
      return (c - 10 + 'A');
}

/*
 * nhrp_printnsap_subr
 * Convert a single byte of an NSAP address to ASCII.
 */

static inline void nhrp_printnsap_subr (uchar **in, uchar **outp)
{
    uchar *out;

    out = *outp;
    *(out++) = nhrp_htoa((**in & 0xf0) >> 4);
    *(out++) = nhrp_htoa(**in & 0x0f);
    *outp = out;
    ++*in;
}

/*
 * nhrp_printnsap
 * Print an NSAP address in ATM format
 */

void nhrp_printnsap (uchar *innsap, uchar *outnsap)
{
   ushort k;

   if (!innsap)
       return;
   /*
    * print AFI
    */
   nhrp_printnsap_subr(&innsap, &outnsap);
   *(outnsap++) = '.';
   /*
    * Print rest of prefix
    */
   for (k = 0; k < 12; k++)
       nhrp_printnsap_subr(&innsap, &outnsap);
   *(outnsap++) = '.';
   /*
    * Print the esi
    */
   for (k = 0; k < 6; k++)
       nhrp_printnsap_subr(&innsap, &outnsap);
   *(outnsap++) = '.';
   /*
    * And finally, the selector
    */
   nhrp_printnsap_subr(&innsap, &outnsap);
   *outnsap = '\0';
}

/*
 * nhrp_sprint_nbma
 * Print an NBMA address to a buffer
 */

uchar *nhrp_sprint_nbma (uchar *nbma_buf, uchar *nbma, uchar len, uchar type)
{
    nhrpaddrtype *nbma_addr;

    nbma_addr = (nhrpaddrtype *)nbma;
    switch (type) {
    case AF_RESERVED:
	sprintf(nbma_buf, "None");
	break;
    case AF_IP:
	sprintf(nbma_buf, "%i", nbma_addr->ip_address);
	break;
    case AF_E164:
	sprintf(nbma_buf, "%E", nbma_addr->smds_addr);
	break;
    case AF_NSAP:
	nhrp_printnsap(nbma_addr->nsap, nbma_buf);
	break;
    case AF_802:
	sprintf(nbma_buf, "%e", nbma_addr->mac_addr);
	break;
    default:
	sprintf(nbma_buf, "%x", nbma_addr->opaque);
	break;
    }
    return(nbma_buf);
}

/*
 * nhrp_find_nhs
 * Determine the NHS for a particular destination, or 0 if none.
 * This code should really use a tree based data structure.
 */

static nhrpaddr *nhrp_find_nhs (idbtype *idb, nhrpaddr *dest, nhrpfuns *fun,
				boolean recurse)
{
    nhrpnhstype *ptr, *best, *table;
    nhrpaddr def;
    nhrpinfotype *nhrpinfo;

    best = NULL;
    nhrpinfo = idb->nhrp_info;
    if (!nhrpinfo)
	return(NULL);
    table = nhrpinfo->pro[fun->protoidx].nhrp_nhstable;
    if (table == NULL)
	return(NULL);
    for (ptr = table; ptr; ptr = ptr->next) {
	if (!(*fun->nhrp_onnet)(&ptr->net, dest))
	    continue;
	/*
	 * Found a match.  Now determine if it is the longest match
	 */
	if (best) {
	    if ((*fun->nhrp_specific)(&ptr->net, &best->net))
		best = ptr;
	} else {
	    best = ptr;
	}
    }
    if (best)
	return(&best->nhs);
    /*
     * Recurse to find default NHSes
     */
    if (recurse) {
	memset(&def, 0, sizeof(def));
	return(nhrp_find_nhs(idb, &def, fun, FALSE));
    }
    return(NULL);
}

/*
 * nhrp_valid_nbma_len
 * Sanity check the NBMA length for a particular NBMA address type.
 */

static boolean nhrp_valid_nbma_len (uchar nbma_len, ushort addr_type)
{
    uchar maxbits;
    
    /*
     * Allow null NBMA address to pass sanity check
     */
    if ((addr_type == AF_RESERVED) && (nbma_len == 0))
	return(TRUE);
    if ((nbma_len == 0) || (nbma_len > NHRP_NBMA_MAXLEN)) {
	if (nhrp_debug)
	    buginf("\nNHRP: Illegal NBMA len %d", nbma_len);
	return(FALSE);
    }
    maxbits = NHRP_BYTESIZE * nhrp_af_len(addr_type);
    switch (addr_type) {
    case AF_IP:
    case AF_E164:
    case AF_NSAP:
    case AF_802:
	if (nbma_len == maxbits)
	    return(TRUE);
	break;
    default:
	if (nhrp_debug)
	    buginf("\nNHRP: Bad address type %d", addr_type);
	return(FALSE);
    }
    if (nhrp_debug)
	buginf("\nNHRP: Bad NBMA len %d for type %d", nbma_len, addr_type);
    return(FALSE);
}

/*
 * nhrp_copy_nbma
 * Copy an nbma address into cache
 */

static inline boolean nhrp_copy_nbma (void *src, uchar nbma_len,
				      ushort addr_type, nhrpcachetype *dst)
{
    uchar nbma_bytes;

    if (!nhrp_valid_nbma_len(nbma_len, addr_type)) {
	return(FALSE);
    }
    nbma_bytes = NHRP_NBMA_BYTES(nbma_len);
    bcopy(src, &dst->nbma.opaque, nbma_bytes);
    dst->nbma_len = nbma_len;
    dst->addr_type = addr_type;
    return(TRUE);
}

/*
 * nhrp_bugnet
 * Buginf a protocol independent nhrp network
 */

void nhrp_bugnet (uchar *str, nhrpaddrnet *net, nhrpfuns *fun)
{
    switch (fun->protoidx) {
    case NHRP_NETIP:
    default:
	buginf("%s%i%m", str, net->ip.target, net->ip.mask);
	break;
    case NHRP_NETAT:
    case NHRP_NETIPX:
	buginf("%s%x.%e", str, net->ipx.net, net->ipx.host);
	break;
    }
}

/*
 * nhrp_bugaddr
 * Buginf a protocol independent nhrp address
 */

void nhrp_bugaddr (uchar *str, nhrpaddr *addr, nhrpfuns *fun)
{
    switch (fun->protoidx) {
    case NHRP_NETIP:
    default:
	buginf("%s%i", str, addr->ip);
	break;
    case NHRP_NETAT:
    case NHRP_NETIPX:
	buginf("%s%x.%e", str, addr->ipx.net, addr->ipx.host);
	break;
    }
}

/*
 * nhrp_printaddr
 * printf a protocol independent nhrp address
 */

void nhrp_printaddr (uchar *str, nhrpaddr *addr, nhrpfuns *fun)
{
    switch (fun->protoidx) {
    case NHRP_NETIP:
    default:
	printf("%s%i", str, addr->ip);
	break;
    case NHRP_NETAT:
    case NHRP_NETIPX:
	printf("%s%x.%e", str, addr->ipx.net, addr->ipx.host);
	break;
    }
}

/*
 * nhrp_printpre
 * printf a protocol independent nhrp prefix (addr/net combo)
 */

void nhrp_printpre (uchar *str, nhrpaddrnet *addr, nhrpfuns *fun)
{
    switch (fun->protoidx) {
    case NHRP_NETIP:
    default:
	printf("%s%i%m", str, addr->ip.target, addr->ip.mask);
	break;
    case NHRP_NETAT:
    case NHRP_NETIPX:
	printf("%s%x.%e", str, addr->ipx.net, addr->ipx.host);
	break;
    }
}

/*
 * nhrp_cache_populate
 * Fill in cachetype structure information.
 */

static inline void nhrp_cache_populate (nhrpcachetype *ptr,
					idbtype *idb, ushort hold,
					nhrpaddr *target, nhrpaddr *nhop,
					uchar flags, nhrp_cachemodetype mode,
					nhrpfuns *fun)
{
    uchar nbma_buf[NHRP_NBMA_MAXSTR];

    GET_TIMESTAMP(ptr->created);
    TIMER_START(ptr->expire, hold * ONESEC);
    bcopy(target, &ptr->prefix.gen.opaque, fun->nhrp_addlen);
    bcopy(nhop, &ptr->nhop, fun->nhrp_addlen);
    ptr->flags = flags;
    ptr->mode = mode;
    if (nhrp_debug) {
	nhrp_bugaddr("\nNHRP: Cache update ", target, fun);
	buginf(" %s", nhrp_sprint_nbma(nbma_buf, ptr->nbma.opaque,
				       ptr->nbma_len, ptr->addr_type));
    }
}

/*
 * nhrp_cache_request
 * Revise an existing nhrp cache table entry with new information gleaned
 * from an NHRP request.
 */

static boolean nhrp_cache_request (nhrpcachetype *ptr, 
			    idbtype *idb, nhrpaddr *dest, nhrp_mask mask,
			    void *arg, nhrpfuns *fun)
{
    nhrpnhoptype *nbma;

    switch (ptr->mode) {
    case NHRP_STATIC:
	/*
	 * Don't overwrite static cache entries
	 */
	return(FALSE);
	break;
    case NHRP_DYNAMIC:
	/*
	 * Don't overwrite entries we learned explicitly
	 */
	if ((ptr->flags & NHRP_IFLAG) == 0)
	    return(FALSE);
	break;
    case NHRP_INCOMPLETE:
	break;
    }
    nbma = nhrp_req2nbma(arg, fun);
    if (!nhrp_copy_nbma(&nbma->nbma, nbma->nbma_len, nbma->addr_type, ptr))
	return(FALSE);
    nhrp_cache_populate(ptr, idb, nbma->hold, dest, dest,
			NHRP_AFLAG|NHRP_PFLAG|NHRP_IFLAG, NHRP_DYNAMIC, fun);
    return(TRUE);
}

/*
 * nhrp_cache_reply
 * Revise an existing nhrp cache table entry with new information
 * from an NHRP response.
 */

static boolean nhrp_cache_reply (nhrpcachetype *ptr, 
			  idbtype *idb, nhrpaddr *dest, nhrp_mask mask,
			  void *arg, nhrpfuns *fun)
{
    nhrpreqreptype *rep;
    nhrpnhoptype *nhop;

    /*
     * Don't overwrite static cache entries
     */
    if (ptr->mode == NHRP_STATIC)
	return(FALSE);
    rep = arg;
    nhop = nhrp_rep2nbma(rep, fun);
    if (!nhrp_copy_nbma(&nhop->nbma, nhop->nbma_len, nhop->addr_type, ptr))
	return(FALSE);
    nhrp_cache_populate(ptr, idb, nhop->hold, dest, nhrp_repnhop(rep, fun),
			rep->flags & NHRP_CACHED_FLAGS, NHRP_DYNAMIC, fun);
    /*
     * Get mask out of reply packet, and stick into cache.
     */
    if (mask)
	(*fun->nhrp_copymask)(mask, ptr);

    ptr->reqid = rep->reqid;
    return(TRUE);
}

/*
 * nhrp_cache_reg
 * Revise an existing nhrp cache table entry with new information
 * from an NHRP register packet.
 */

static boolean nhrp_cache_reg (nhrpcachetype *ptr, 
			idbtype *idb, nhrpaddr *dest, nhrp_mask mask,
			void *arg, nhrpfuns *fun)
{
    nhrpnhoptype *nhop;

    /*
     * Don't overwrite static cache entries
     */
    if (ptr->mode == NHRP_STATIC)
	return(FALSE);
    nhop = nhrp_reg2nbma(arg, fun);
    if (!nhrp_copy_nbma(&nhop->nbma, nhop->nbma_len, nhop->addr_type, ptr))
	return(FALSE);
    nhrp_cache_populate(ptr, idb, nhop->hold, dest, nhrp_regsrc(arg, fun),
			NHRP_AFLAG|NHRP_PFLAG|NHRP_RFLAG, NHRP_DYNAMIC, fun);
    return(TRUE);
}

/*
 * nrhp_ptype_string
 * Map an NHRP packet type to a string.
 */

uchar *nhrp_ptype_string (uchar type)
{
    switch (type) {
    case NHRP_REQUEST:
	return("request");
    case NHRP_REPLY:
	return("reply");
    case NHRP_REG:
	return("register");
    case NHRP_PURGE:
	return("purge");
    case NHRP_ERR:
	return("error");
    default:
	return("unknown");
    }
}

/*
 * nhrp_err_string
 * Map an NHRP error code to a string.
 */

static uchar *nhrp_err_string (ushort code)
{
    switch (code) {
    case NHRP_ERR_OPT:
	return("Unknown option");
    case NHRP_ERR_NETID:
	return("Network ID mismatch");
    case NHRP_ERR_LOOP:
	return("Loop detected");
    case NHRP_ERR_CANT:
	return("Can't serve this address");
    case NHRP_ERR_FULL:
	return("Cache is full");
    case NHRP_ERR_NHS_REACH:
	return("NHS unreachable");
    case NHRP_ERR_PROTO:
	return("Protocol failure");
    case NHRP_ERR_FRAG:
	return("Fragmentation failure");
    default:
	return("Unknown error code");
    }
}

/*
 * nrhp_stat
 * Gather NHRP packet statistics for an interface.
 */

static void nhrp_stat (idbtype *idb, ushort type, boolean recv, nhrpfuns *fun)
{
    nhrpinfotype *nhrpinfo;
    nhrpstattype *stats;

    if (idb == NULL)
	return;
    nhrpinfo = idb->nhrp_info;
    if (!nhrpinfo)
	return;
    if (type == 0 || type > NHRP_TYPES)
	return;
    type = (type - 1) * 2;
    if (recv)
	type++;
    stats = nhrpinfo->pro[fun->protoidx].nhrp_stats;
    if (!stats)
	return;
    stats->pkts[type]++;
}

/*
 * nhrp_nextidb
 * Find next hop idb for the provided destination
 * 'Modify' indicates whether or not the dest argument should be modified
 * to contain the next hop network layer address.
 */

static idbtype *nhrp_nextidb (paktype *pak, nhrpaddr *dest, boolean modify,
		       nhrpfuns *fun)
{
    return((*fun->nhrp_nhop)(pak, dest, modify));
}

/*
 * nhrp_opt_string
 * Map an NHRP option type to a string.
 */

static uchar *nhrp_opt_string (ushort type)
{
    switch (type) {
    case NHRP_OP_MASK:
	return("MASK");
    case NHRP_OP_NETID:
	return("NETID");
    case NHRP_OP_RESP:
	return("RESPONDER");
    case NHRP_OP_RECORD:
	return("RECORD");
    case NHRP_OP_RRECORD:
	return("RRECORD");
    case NHRP_OP_QOS:
	return("QOS");
    case NHRP_OP_AUTH:
	return("AUTH");
    default:
	return("unknown");
    }
}

/*
 * nhrp_show_record
 * Display debugging information regarding an NHRP record option.
 */

static void nhrp_show_record (uchar *opt, ushort len, nhrpfuns *fun)
{
    uchar *end;
    uchar reclen;
    nhrpnhoptype *rec;
    uchar nbma_buf[NHRP_NBMA_MAXSTR];
    nhrpaddr *addr;

    end = opt + len;
    while (opt < end) {
	addr = (nhrpaddr *)opt;
	rec = (nhrpnhoptype *)(opt + NHRP_ALIGN(fun->nhrp_addlen));
	nhrp_bugaddr("\nNHRP-OPT-REC: ", addr, fun);
	buginf(" %s", nhrp_sprint_nbma(nbma_buf, rec->nbma, rec->nbma_len,
				       rec->addr_type));
	reclen = NHRP_ALIGN(fun->nhrp_addlen) +
	    NHRP_ALIGN(NHRP_OPLEN_REC + NHRP_BYTE_LEN(rec->nbma_len));
	opt += reclen;
    }
}

/*
 * nhrp_show_options
 * Display debugging information on the NHRP options found within a packet.
 */

static void nhrp_show_options (paktype *pak, uchar *opt, nhrpfuns *fun)
{
    uchar *eop;
    nhrpopgentype *gopt;
    ushort optlen;

    eop = nhrp_eop(pak);
    while ((opt + NHRP_TL_LEN) <= eop) {
	gopt = (nhrpopgentype *)opt;
	buginf("\nNHRP-OPT: %s %d", nhrp_opt_string(gopt->type), gopt->len);
	optlen = NHRP_TL_LEN + gopt->len;
	if ((opt + optlen) > eop)
	    return;
	switch (gopt->type & ~NHRP_TLV_OPTBIT) {
	case NHRP_OP_RECORD:
	case NHRP_OP_RRECORD:
	    nhrp_show_record(gopt->opaque, gopt->len, fun);
	    break;
	case NHRP_OP_MASK:
	    buginf("\nNHRP-OPT-MASK: %x", ((nhrpopmasktype *)gopt)->mask);
	    break;
	default:
	    break;
	}
	opt += optlen;
    }
}

/*
 * nhrp_bugpak_mand
 * Print debugging information pertaining to the mandatory header.
 */

static void nhrp_bugpak_mand (nhrpreqreptype *req, paktype *pak, nhrpfuns *fun)
{
    nhrpaddr *src, *dst, *pursrc;
    nhrperrtype *err;

    switch (req->pkt_type) {
    case NHRP_REQUEST:
    case NHRP_REPLY:
	src = nhrp_reqsrc(req, fun);
	dst = nhrp_reqdst(req, fun);
	buginf("\nNHRP M: id %u", req->reqid);
	nhrp_bugaddr(" src ", src, fun);
	nhrp_bugaddr(" dst ", dst, fun);
	break;
    case NHRP_PURGE:
	src = nhrp_reqsrc(req, fun);
	dst = nhrp_reqdst(req, fun);
	pursrc = nhrp_pursrc(req, fun);
	buginf("\nNHRP M: %sid %u",
	       (req->flags & NHRP_PURGE_AFLAG) ? "Ack " : "",
	       req->reqid);
	nhrp_bugaddr(" reqsrc ", src, fun);
	nhrp_bugaddr(" reqdst ", dst, fun);
	nhrp_bugaddr(" pursrc ", pursrc, fun);
	break;
    case NHRP_ERR:
	err = (nhrperrtype *)req;
	buginf("\nNHRP: Error \"%s\" offset %d",
	       nhrp_err_string(err->errcode), err->offset);
	break;
    default:
	break;
    }
}

/*
 * nhrp_write
 * Note: dst is the network layer ultimate destination which we
 * are trying to reach.  First find the next hop for the destination.
 * Then map this network layer destination to an NBMA address.
 */

static void nhrp_write (paktype *pak, int paklen, nhrpaddr *dst,
		 idbtype *if_out, nhrpfuns *fun)
{
    hwidbtype *hwidb;
    boolean result;
    hwaddrtype hw;
    idbtype *if_out_nhop;
    boolean is_broad;
    nhrpaddr *nhop;
    uchar *addrp;
    ulong netid;

    hw.type = fun->linktype;
    hw.length = fun->nhrp_addlen;
    /*
     * NOVELL is a special case for hwidb->encap.  The passed in address is
     * a pointer instead of the address itself.
     */
    if (fun->linktype == LINK_NOVELL) {
	addrp = &hw.addr[sizeof(uchar *) + sizeof(long)];
	bcopy(&addrp, hw.addr, sizeof(uchar *));
	addrp = &hw.addr[sizeof(uchar *)];
    } else {
	addrp = hw.addr;
    }
    bcopy(dst, addrp, fun->nhrp_addlen);
    nhop = (nhrpaddr *)addrp;
    if_out_nhop = if_out;
    /*
     * Find next hop on the NBMA network
     */
    is_broad = (*fun->nhrp_isbroad)(nhop, if_out);
    netid = nhrp_netid(if_out, fun);
    if (netid == NHRP_NETID_UNDEF) {
	if (nhrp_debug)
	    buginf("\nNHRP: Pak out %s would leave logical nbma network",
		   if_out->namestring);
	retbuffer(pak);
	return;
    }
    if (!is_broad) {
	if_out_nhop = nhrp_nextidb(pak, nhop, TRUE, fun);
	if (!if_out_nhop) {
	    if (nhrp_debug)
		nhrp_bugaddr("\nNHRP: Cannot route packet for destination ",
			     nhop, fun);
	    retbuffer(pak);
	    return;
	}
	if (netid != nhrp_netid(if_out_nhop, fun)) {
	    if (nhrp_debug)
		buginf("\nNHRP: Pak out %s would leave logical nbma network",
		       if_out_nhop->namestring);
	    retbuffer(pak);
	    return;
	}
    }
    hwidb = if_out_nhop->hwptr;
    pak->linktype = LINK_NHRP;
    pak->if_output = if_out_nhop;
    pak->datagramsize = paklen;
    result = (*hwidb->vencap)(pak, (long)&hw);
    if (!result) {
	if (nhrp_debug) {
	    nhrp_bugaddr("\nNHRP: Encapsulation failed for dest ", nhop, fun);
	    buginf(" out %s", if_out_nhop->namestring);
	}
	datagram_done(pak);
    } else {
	if (nhrp_debug) {
	    buginf("\nNHRP: O %d bytes out %s ", pak->datagramsize,
		   if_out_nhop->namestring);
	    nhrp_bugaddr("dest ", nhop, fun);
	}
	datagram_out(pak);
    }
}

/*
 * nhrp_send
 * Compute final NHRP checksum for packet, and dump it on the lower layer
 * for transmission.
 */

static void nhrp_send (paktype *pak, nhrpfixedtype *fix, int nhrp_len,
		nhrpaddr *src, nhrpaddr *dest, idbtype *if_out, nhrpfuns *fun)
{
    nhrpinfotype *nhrpinfo;

    fix->cksum = ipcrc((ushort *)fix, nhrp_len);
    if (nhrp_debug) {
	buginf("\nNHRP: Sent %s ", nhrp_ptype_string(fix->pkt_type));
	nhrp_bugaddr("src ", src, fun);
	nhrp_bugaddr(" dst ", dest, fun);
	pak->datagramsize = nhrp_len;
	nhrp_bugpak_mand((nhrpreqreptype *)fix, pak, fun);
    }
    if (nhrp_debug_opt)
	nhrp_show_options(pak, nhrpopstart(fix, fix->mand_len), fun);
    if (bcmp(src, dest, fun->nhrp_addlen) == 0) {
	if (nhrp_debug) {
	    nhrp_bugaddr("\nNHRP: Discarding. src = dst = ", src, fun);
	}
	retbuffer(pak);
	return;
    }
    if (if_out == NULL) {
	if (nhrp_debug) {
	    buginf("\nNHRP: Discarding.  if_out is unknown.");
	    nhrp_bugaddr("dst is ", dest, fun);
	}
	retbuffer(pak);
	return;
    }
    nhrpinfo = if_out->nhrp_info;
    if (!nhrpinfo) {
	if (nhrp_debug)
	    buginf("\nNHRP: Pak out %s would leave logical nbma network",
		   if_out->namestring);
	retbuffer(pak);
	return;
    }
    if (nhrpinfo->pro[fun->protoidx].nhrp_quota)
	nhrpinfo->pro[fun->protoidx].nhrp_quota--;
    nhrp_stat(if_out, fix->pkt_type, FALSE, fun);
    nhrp_write(pak, nhrp_len, dest, if_out, fun);
}

/*
 * nhrp_hold
 * Determine the advertised holdtime for this interface
 */

static inline ulong nhrp_hold (idbtype *idb, boolean pos, nhrpfuns *fun)
{
    nhrpinfotype *nhrpinfo;
    nhrpproinfotype *proinfo;

    nhrpinfo = idb->nhrp_info;
    if (!nhrpinfo)
	return(NHRP_DEF_HOLDTIME);
    proinfo = &nhrpinfo->pro[fun->protoidx];
    if (!(proinfo->nhrp_flags & NHRP_CFG_HOLD))
	return(NHRP_DEF_HOLDTIME);
    if (pos)
	return(proinfo->nhrp_hold_pos);
    else
	return(proinfo->nhrp_hold_neg);
}

/*
 * nhrp_use_record
 * Return an indication as to whether or not it is OK to add record
 * extensions to the packet.
 */

static boolean nhrp_use_record (idbtype *swidb, nhrpfuns *fun)
{
    nhrpinfotype *nhrpinfo;

    nhrpinfo = swidb->nhrp_info;
    if (!nhrpinfo)
	return(TRUE);
    return((nhrpinfo->pro[fun->protoidx].nhrp_flags & NHRP_CFG_NOREC) == 0);
}

/*
 * nhrp_add_options
 * Add the default options to an NHRP request packet which we are locally
 * generating.
 */

static ulong nhrp_add_options (uchar *opt, idbtype *idb, nhrpfuns *fun)
{
    ulong len;
    nhrpopmasktype *mask;
    nhrpopnetidtype *net;
    nhrpoprestype *res;
    nhrpoprectype *rec;

    len = 0;
    /*
     * Mask option
     */
    mask = (nhrpopmasktype *)opt;
    mask->type = NHRP_OP_MASK;
    mask->len = NHRP_OPLEN_MASK;
    mask->mask = NHRP_OPDEF_MASK;
    len += NHRP_TL_LEN + NHRP_OPLEN_MASK;
    /*
     * Network ID option
     */
    net = (nhrpopnetidtype *)&opt[len];
    net->type = NHRP_OP_NETID;
    net->len = NHRP_OPLEN_NETID;
    net->netid = nhrp_netid(idb, fun);
    len += NHRP_TL_LEN + NHRP_OPLEN_NETID;
    /*
     * Responder address
     */
    res = (nhrpoprestype *)&opt[len];
    res->type = NHRP_OP_RESP;
    res->len = NHRP_OPLEN_RESP;
    res->resp = 0;
    len += NHRP_TL_LEN + NHRP_OPLEN_RESP;
    if (nhrp_use_record(idb, fun)) {
	/*
	 * Forward record option
	 */
	rec = (nhrpoprectype *)&opt[len];
	rec->type = NHRP_OP_RECORD;
	rec->len = 0;
	len += NHRP_TL_LEN;
	/*
	 * Reverse record option
	 */
	rec = (nhrpoprectype *)&opt[len];
	rec->type = NHRP_OP_RRECORD;
	rec->len = 0;
	len += NHRP_TL_LEN;
    }
    return(len);
}

static paktype *nhrp_getbuffer (ushort size)
{
    paktype *pak;
    
    pak = getbuffer(size);
    return(pak);
}

/*
 * nhrp_err
 * Generate an NHRP error packet in response to an incoming NHRP packet.
 */

static void nhrp_err (paktype *err_pak, ushort code, ushort offset, nhrpfuns *fun)
{
    nhrpreqreptype *errreq;
    ulong nhrp_len, head_len, err_pak_len;
    uchar *eop;
    nhrperrtype *err;
    paktype *pak;
    idbtype *if_in, *if_out;
    nhrpaddr *dest, *src;

    errreq = nhrpfixstart(err_pak);
    eop = nhrp_eop(err_pak);
    err_pak_len = eop - (uchar *)errreq;
    head_len = NHRP_FIXEDLEN + NHRP_ERRLEN;
    nhrp_len =  head_len + err_pak_len;
    if_in = err_pak->if_input;
    if (nhrp_toomany(if_in, fun))
	return;
    dest = (*fun->nhrp_ifaddr)(if_in);
    if_out = nhrp_nextidb(NULL, dest, FALSE, fun);
    if (if_out == NULL) {
	if (nhrp_debug) {
	    nhrp_bugaddr("\nNHRP: Cannot route error packet for destination ",
		   dest, fun);
	}
	return;
    }
    pak = nhrp_getbuffer(nhrp_len);
    if (!pak)
	return;
    err = (nhrperrtype *)nhrpfixstart(pak);
    memset(err, 0, head_len);
    err->version = NHRP_VERSION;
    err->hop = NHRP_MAXHOPS;
    err->pkt_type = NHRP_ERR;
    err->mand_len = NHRP_ERRLEN + err_pak_len;
    err->errcode = code;
    err->offset = offset;
    bcopy(errreq, &err->nhrp_pak, err_pak_len);
    src = nhrp_reqsrc(errreq, fun);
    nhrp_send(pak, (nhrpfixedtype *)err, nhrp_len, src, dest, if_out, fun);
}

/*
 * nhrp_our_nbma
 * Figure out our NBMA address, and the length of the address (in bits).
 */

static boolean nhrp_our_nbma (ushort addr_type, idbtype *swidb, nhrpaddrtype *nbma,
		       uchar *nbma_len)
{
    uchar addr_len;
    tunnel_info *tinfo;
    hwaddrtype *hwaddr;

    addr_len = nhrp_af_len(addr_type);
    switch (addr_type) {
    case AF_IP:
	tinfo = swidb->hwptr->tunnel;
	if (!tinfo)
	    return(FALSE);
	nbma->ip_address = TUNNEL_MYADDR(tinfo);
	*nbma_len = NHRP_BYTESIZE * addr_len;
	break;
    case AF_E164:
	hwaddr = swidb->vaddr;
	if (!hwaddr)
	    return(FALSE);
	bcopy(hwaddr->addr, nbma->smds_addr, addr_len);
	*nbma_len = NHRP_BYTESIZE * addr_len;
	break;
    case AF_NSAP:
	hwaddr = swidb->nsap;
	if (!hwaddr)
	    return(FALSE);
	bcopy(hwaddr->addr, nbma->nsap, addr_len);
	*nbma_len = NHRP_BYTESIZE * addr_len;
	break;
    case AF_802:
	ieee_copy(swidb->hwptr->hardware, nbma->mac_addr);
	*nbma_len = NHRP_BYTESIZE * addr_len;
	break;
    default:
	if (nhrp_debug) {
	    buginf("\nNHRP: Rejecting addr type %d", addr_type);
	}
	return(FALSE);
	break;
    }
    return(TRUE);
}

/*
 * nhrp_nbma_type
 * Determine the NBMA type for an interface.
 */

ushort nhrp_nbma_type (idbtype *swidb)
{
    hwidbtype *hwidb;

    hwidb = swidb->hwptr;
    switch (hwidb->enctype) {
    case ET_TUNNEL:
	return(AF_IP);
    case ET_X25:
	return(AF_X121);
    case ET_SMDS:
	return(AF_E164);
    case ET_ATM:
	if (swidb->arp_smds)
	    return(AF_E164);
	else
	    return(AF_NSAP);
    case ET_ARPA:
    case ET_SAP:
    case ET_SNAP:
	return(AF_802);
    default:
	return(AF_RESERVED);
    }
}

/*
 * nhrp_add_record
 * Add a record to the RECORD/RRECORD option of an NHRP packet.
 */

static ushort nhrp_add_record (uchar *pos, idbtype *if_in, ushort space,
			nhrpfuns *fun)
{
    ushort addr_type;
    nhrpaddrtype nbma;
    uchar nbma_len, nbma_byte_len;
    ushort len;
    nhrpnhoptype *rec;

    addr_type = nhrp_nbma_type(if_in);
    if (!nhrp_our_nbma(addr_type, if_in, &nbma, &nbma_len))
	return(0);
    nbma_byte_len = NHRP_BYTE_LEN(nbma_len);
    len = NHRP_ALIGN(fun->nhrp_addlen) +
	NHRP_ALIGN(NHRP_OPLEN_REC + nbma_byte_len);
    if (space < len)
	return(0);
    bcopy(&if_in->ip_address, pos, fun->nhrp_addlen);
    rec = (nhrpnhoptype *)(pos + NHRP_ALIGN(fun->nhrp_addlen));
    rec->hold = nhrp_hold(if_in, TRUE, fun);
    rec->addr_type = addr_type;
    rec->pref = 0;
    rec->nbma_len = nbma_len;
    bcopy(nbma.opaque, rec->nbma, nbma_byte_len);
    return(len);
}

/*
 * nhrp_ouraddress
 * Is 'addr' one of our addresses?  If netid is set, make sure 'addr' is
 * one of our addresses on that particular logical NBMA network.
 * Return pointer to the interface that 'addr' resides.
 */

static idbtype *nhrp_ouraddress (nhrpaddr *addr, nhrpfuns *fun, ulong netid)
{
    idbtype *swidb;

    if (netid != NHRP_NETID_UNDEF) {
	FOR_ALL_SWIDBS(swidb) {
	    if (nhrp_netid(swidb, fun) != netid)
		continue;
	    if ((*fun->nhrp_ourifadd)(swidb, addr))
		return(swidb);
	}
	return(NULL);
    }

    /*
     * If netid is not set, do not care the return idb.
     */
    return((idbtype *)((*fun->nhrp_ouradd)(addr)));
}

/*
 * nhrp_record_sanity
 * Do loop detection on the RECORD/RRECORD option
 */

static boolean nhrp_record_sanity (uchar *opt, ushort len, nhrpfuns *fun)
{
    uchar *end;
    uchar reclen;
    nhrpnhoptype *rec;
    nhrpaddr *addr;

    end = opt + len;
    while (opt < end) {
	addr = (nhrpaddr *)opt;
	rec = (nhrpnhoptype *)(opt + NHRP_ALIGN(fun->nhrp_addlen));
	if (nhrp_ouraddress(addr, fun, NHRP_NETID_UNDEF)) {
	    return(FALSE);
	}
	reclen = NHRP_ALIGN(fun->nhrp_addlen) +
	    NHRP_ALIGN(NHRP_OPLEN_REC + NHRP_BYTE_LEN(rec->nbma_len));
	opt += reclen;
    }
    return(TRUE);
}

/*
 * nhrp_respif
 * Determine what the configured responder interface is, or NULL if not
 * configured.
 */

idbtype *nhrp_respif (idbtype *swidb, nhrpfuns *fun)
{
    nhrpinfotype *nhrpinfo;

    nhrpinfo = swidb->nhrp_info;
    if (!nhrpinfo)
	return(NULL);
    return(nhrpinfo->pro[fun->protoidx].nhrp_respif);
}

/*
 * nhrp_parse_options
 * Parse options from an NHRP request/response packet.  Copy digested
 * options into packet which we hope to send (either forwarding or
 * locally generating).
 */

static ushort nhrp_parse_options (paktype *req_pak, uchar *src_opt, uchar *dest,
			   ushort space, uchar pkt_type, boolean responder,
			   nhrp_mask *mask, nhrpfuns *fun)
{
    uchar *eop;
    ushort tot_space;
    boolean netid_included, netid_match;
    ulong our_netid, their_netid;
    nhrpopgentype *gopt, *gdest;
    nhrpoprestype *resopt;
    ushort optlen, reclen, type;
    idbtype *if_in, *respif;
    boolean process_record;
    nhrpaddr *ifaddr;
    ulong *reqmask;

    reqmask = NULL;
    netid_included = netid_match = FALSE;
    if_in = req_pak->if_input;
    our_netid = nhrp_netid(if_in, fun);
    their_netid = 0;
    eop = nhrp_eop(req_pak);
    tot_space = space;
    /*
     * Gdest is null when we are only reading options, not copying them.
     */
    gdest = (nhrpopgentype *)dest;
    while ((src_opt + NHRP_TL_LEN) <= eop) {
	gopt = (nhrpopgentype *)src_opt;
	if (gopt->len & 0x3) {
	    if (nhrp_debug_opt)
		buginf("\nNHRP-OPT: Option not filled to 32 bit boundary");
	    return(0);
	}
	optlen = NHRP_TL_LEN + gopt->len;
	if (gdest && (space < optlen)) {
	    if (nhrp_debug_opt)
		buginf("\nNHRP-OPT: Out of room for options");
	    return(0);
	}
	src_opt += optlen;
	if (src_opt > eop) {
	    if (nhrp_debug_opt)
		buginf("\nNHRP-OPT: Options runs past end of packet");
	    return(0);
	}
	type = gopt->type & ~NHRP_TLV_OPTBIT;
	switch (type) {
	case NHRP_OP_MASK:
	    if (gopt->len != NHRP_OPLEN_MASK) {
		if (nhrp_debug_opt)
		    buginf("\nNHRP-OPT: Mask option is wrong length");
		return(0);
	    }
	    reqmask = &((nhrpopmasktype *)gopt)->mask;
	    break;
	case NHRP_OP_NETID:
	    netid_included = TRUE;
	    if (gopt->len != NHRP_OPLEN_NETID) {
		if (nhrp_debug_opt)
		    buginf("\nNHRP-OPT: Netid option is wrong length");
		return(0);
	    }
	    their_netid = ((nhrpopnetidtype *)gopt)->netid;
	    if (their_netid == our_netid)
		netid_match = TRUE;
	    break;
	case NHRP_OP_RESP:
	    if (gopt->len != NHRP_OPLEN_RESP) {
		if (nhrp_debug_opt)
		    buginf("\nNHRP-OPT: Responder option is wrong length");
		return(0);
	    }
	    break;
	default:
	    break;
	}
	if (!gdest) {
	    /*
	     * Pass the mask from the option (if present) back to the caller
	     */
	    *mask = reqmask;
	    continue;
	}
	/*
	 * Copy the option.
	 */
	gdest->type = gopt->type;
	switch (type) {
	case NHRP_OP_MASK:
	    gdest->len = NHRP_OPLEN_MASK;
	    if (responder) {
		/*
		 * If we are the responder, fill in the mask we determined
		 * earlier.
		 */
		((nhrpopmasktype *)gdest)->mask = nhrp_extractmask(*mask);
	    } else {
		/*
		 * Preserve the mask from the request
		 */
		((nhrpopmasktype *)gdest)->mask = *reqmask;
	    }
	    break;
	case NHRP_OP_NETID:
	    gdest->len = NHRP_OPLEN_NETID;
	    ((nhrpopnetidtype *)gdest)->netid = their_netid;
	    break;
	case NHRP_OP_RESP:
	    gdest->len = NHRP_OPLEN_RESP;
	    resopt = (nhrpoprestype *)gdest;
	    if (responder) {
		/*
		 * If we are the responder, fill in the responder option
		 * info.
		 */
		respif = nhrp_respif(if_in, fun);
		if (!respif)
		    respif = if_in;
		ifaddr = (*fun->nhrp_ifaddr)(respif);
		resopt->resp = ifaddr->ip;
	    }
	    break;
	case NHRP_OP_RECORD:
	case NHRP_OP_RRECORD:
	    /*
	     * First determine if we need to process this option in addition
	     * to copying.
	     */
	    switch (pkt_type) {
	    case NHRP_REQUEST:
		process_record = (type == NHRP_OP_RECORD);
		break;
	    case NHRP_REPLY:
		process_record = (type == NHRP_OP_RRECORD);
		break;
	    default:
		process_record = FALSE;
		break;
	    }
	    if (process_record) {
		if (!nhrp_record_sanity(gopt->opaque, gopt->len, fun)) {
		    if (nhrp_debug_opt)
			buginf("\nNHRP-OPT: Loop detected while processing"
			       " %srecord option",
			       type == NHRP_OP_RRECORD ? "reverse " : "");
		    nhrp_err(req_pak, NHRP_ERR_LOOP, 0, fun);
		    return(0);
		}
	    }
	    gdest->len = gopt->len;
	    bcopy(gopt->opaque, gdest->opaque, gopt->len);
	    if (process_record) {
		/*
		 * Add our option
		 */
		reclen = nhrp_add_record(gdest->opaque + gdest->len, if_in,
					 space, fun);
		if (reclen == 0) {
		    if (nhrp_debug_opt)
			buginf("\nNHRP-OPT: Attempt to amend record option"
			       " failed");
		    return(0);
		}
		optlen += reclen;
		gdest->len += reclen;
	    }
	    break;
	default:
	    /*
	     * Unknown TLV
	     */
	    if (!(gopt->type & NHRP_TLV_OPTBIT)) {
		/*
		 * Option cannot be ignored.  Abort.
		 */
		if (nhrp_debug_opt)
		    buginf("\nNHRP-OPT: Unknown option %d", gopt->type);
		nhrp_err(req_pak, NHRP_ERR_CANT, 0, fun);
		return(0);
	    }
	    /*
	     * Ignore, but copy
	     */
	    gdest->len = gopt->len;
	    bcopy(gopt->opaque, gdest->opaque, gopt->len);
	    break;
	}
	gdest = (nhrpopgentype *)(((uchar *)gdest) + optlen);
	space -= optlen;
    }
    if (netid_included && !netid_match) {
	if (nhrp_debug_opt)
	    buginf("\nNHRP-OPT: Network id of %d not found.  %d instead",
		   our_netid, their_netid);
	nhrp_err(req_pak, NHRP_ERR_NETID, 0, fun);
	return(0);
    }
    if (!gdest)
        return(tot_space);
    tot_space -= space;
    if (tot_space == 0) {
	if (nhrp_debug_opt)
	    buginf("\nNHRP-OPT: No options found");
    }
    return(tot_space);
}

/*
 * nhrp_cache_inc
 * Fill in cache information pertaining to an incomplete cache table entry.
 */

static boolean nhrp_cache_inc (nhrpcachetype *ptr, 
			idbtype *idb, nhrpaddr *dest, nhrp_mask mask,
			void *arg, nhrpfuns *fun)
{
    nhrp_cache_populate(ptr, idb, NHRP_PERIODIC, dest, dest,
			NHRP_AFLAG|NHRP_PFLAG, NHRP_INCOMPLETE, fun);
    ptr->hits = 1;
    ptr->ret = NHRP_RETRANS_RATE;
    TIMER_START(ptr->retrans, ptr->ret*ONESEC);
    return(TRUE);
}

/*
 * nhrp_use
 * Determine the configured usage count (which should trigger an NHRP request)
 */

ushort nhrp_use (idbtype *swidb, nhrpfuns *fun)
{
    nhrpinfotype *nhrpinfo;
    
    nhrpinfo = swidb->nhrp_info;
    if (!nhrpinfo)
	return(NHRP_USE_DEF);
    return(nhrpinfo->pro[fun->protoidx].nhrp_use);
}

/*
 * nhrp_revise_inc
 * Revise an incomplete cache table entry.
 * Return TRUE if it is time to send actual request, FALSE otherwise.
 */

static boolean nhrp_revise_inc (idbtype *swidb, nhrpaddr *dest, nhrpfuns *fun)
{
    nhrpcachetype *ptr;
    ushort use;
    uchar *route_mask, *cache_mask;
    nhrpaddrnet net;
    boolean static_route;

    /*
     * Try longest match first, if there is a match then check if route
     * entry is more specific then NHRP cache. If yes and it's not a
     * static route then we still need to send a request.
     */
    memset(&net, 0, sizeof(nhrpaddrnet));
    bcopy(dest, net.gen.opaque, fun->nhrp_addlen);
    ptr = nhrp_cache_lookup_rnmask(swidb, &net, fun, TRUE);
    if (ptr) {
	static_route = FALSE;
	route_mask = (*fun->nhrp_netsmask)(dest, &static_route);
	if (route_mask) {
	    cache_mask = nhrp_cache_keypos(&ptr->prefix, fun->nhrp_addlen)
			 + 1;
	    if (!(*fun->nhrp_specific2)(cache_mask, route_mask) &&
		!static_route) {
		ptr = NULL;
	    }
	}
    }

    if (!ptr) {
	/*
	 * Create new cache table entry.
	 */
	nhrp_cache_add(swidb, dest, NULL, nhrp_cache_inc, NULL, fun);
	if (nhrp_use(swidb, fun) != 1)
	    return(FALSE);
	if (nhrp_debug_rate)
	    buginf("\nNHRP-RATE: Sending initial request");
	return(TRUE);
    }
    if (ptr->mode != NHRP_INCOMPLETE) {
	/*
	 * An entry is now there.  No need to send request.
	 */
	return(FALSE);
    }
    ptr->hits++;
    use = nhrp_use(swidb, fun);
    /*
     * Return if we haven't gotten enough requests to transmit request.
     */
    if (ptr->hits < use)
	return(FALSE);
    /*
     * Time to send initial request?
     */
    if (ptr->hits == use) {
	if (nhrp_debug_rate)
	    buginf("\nNHRP-RATE: Sending initial request");
	TIMER_START(ptr->retrans, ptr->ret*ONESEC);
	return(TRUE);
    }
    /*
     * Time to retransmit?
     */
    if (SLEEPING(ptr->retrans))
	return(FALSE);
    if (nhrp_debug_rate)
	buginf("\nNHRP-RATE: Retransmitting request (retrans ivl %u)",
	       ptr->ret);
    /*
     * Do exponential backoff
     */
    ptr->ret *= 2;
    TIMER_START(ptr->retrans, ptr->ret*ONESEC);
    return(TRUE);
}

/*
 * nhrp_tap_req
 * Update or add NBMA information for the originator of the NHRP request
 * packet.
 */

static void nhrp_tap_req (idbtype *if_in, nhrpreqreptype *req, nhrpfuns *fun)
{
    nhrpcachetype *ptr;
    nhrpaddr *src;

    /*
     * Update our cache with the requestor's NBMA address information
     */
    src = nhrp_reqsrc(req, fun);
    ptr = nhrp_cache_lookup(if_in, src, fun);
    if (ptr) {
	nhrp_cache_request(ptr, if_in, src, NULL, req, fun);
    } else {
	nhrp_cache_add(if_in, src, NULL, nhrp_cache_request, req, fun);
    }
}

/*
 * nhrp_cache_ourself
 * Revise an existing nhrp cache table entry with new information which
 * we derived from our routing table or interface addresses.
 */

static boolean nhrp_cache_ourself (nhrpcachetype *ptr, 
			    idbtype *swidb, nhrpaddr *dest, nhrp_mask mask,
			    void *arg, nhrpfuns *fun)
{
    nhrpnbmatype *hw;

    hw = arg;
    if (!nhrp_copy_nbma(hw->nbmaptr, hw->nbma_len, hw->addr_type, ptr))
	return(FALSE);
    nhrp_cache_populate(ptr, swidb, nhrp_hold(swidb, TRUE, fun), dest, dest,
			NHRP_AFLAG|NHRP_PFLAG|NHRP_GFLAG, NHRP_DYNAMIC, fun);
    /*
     * Fetch the network mask we derived earlier.
     */
    if (mask)
	(*fun->nhrp_copymask)(mask, ptr);
    return(TRUE);
}

/*
 * nhrp_tap_ourself
 * Update or add NBMA information for a response that we derived from
 * our routing table or interface addresses.
 * Return a pointer to the cache table entry (which may be NULL).
 */

static nhrpcachetype *nhrp_tap_ourself (idbtype *if_in, nhrpaddr *dest,
				 nhrp_mask mask,
				 nhrpnbmatype *hw, nhrpfuns *fun)
{
    nhrpcachetype *ptr;

    ptr = nhrp_cache_lookup_mask(if_in, dest, mask, fun);
    if (ptr) {
	nhrp_cache_ourself(ptr, if_in, dest, mask, hw, fun);
    } else {
	ptr = nhrp_cache_add(if_in, dest, mask, nhrp_cache_ourself, hw, fun);
    }
    return(ptr);
}

/*
 * nhrp_send_rep
 * Send an NHRP reply packet in response to a request.
 */

static void nhrp_send_rep (paktype *req_pak, nhrpreqreptype *req, uchar cache_flags,
		    uchar nbma_len, uchar *nbma, nhrpaddr *nhop_addr,
		    nhrp_mask mask, nhrpfuns *fun)
{
    paktype *pak;
    nhrpreqreptype *rep;
    ulong nhoplen, nhrp_len;
    ushort optlen;
    nhrpnhoptype *nhop;
    uchar nbma_byte_len;
    ushort mand_len;
    boolean positive;
    idbtype *if_in;
    uchar nbma_buf[NHRP_NBMA_MAXSTR];
    nhrpnhoptype *pnbma;
    nhrpaddr *srcp, *dstp;

    if_in = req_pak->if_input;
    nhrp_tap_req(if_in, req, fun);
    if (nbma_len)
	nbma_byte_len = NHRP_BYTE_LEN(nbma_len);
    else
	nbma_byte_len = 0;
    nhoplen = NHRP_ALIGN(fun->nhrp_addlen) +
	NHRP_ALIGN(NHRP_NHOPLEN + nbma_byte_len);
    mand_len = NHRP_REQLEN + NHRP_ALIGN(fun->nhrp_addlen) * 2 + nhoplen;
    nhrp_len = NHRP_FIXEDLEN + mand_len + NHRP_OPLEN_MAXTOTAL;
    if (nhrp_toomany(if_in, fun))
	return;
    pak = nhrp_getbuffer(nhrp_len);
    if (!pak)
	return;
    rep = (nhrpreqreptype *)nhrpfixstart(pak);
    memset(rep, 0, nhrp_len);
    rep->version = NHRP_VERSION;
    rep->hop = NHRP_MAXHOPS;
    rep->pkt_type = NHRP_REPLY;
    rep->mand_len = mand_len;
    rep->flags = (req->flags & NHRP_QFLAG);
    rep->flags |= cache_flags;
    rep->protoid = fun->protoid;
    rep->reqid = req->reqid;
    srcp = nhrp_reqsrc(rep, fun);
    dstp = nhrp_reqdst(rep, fun);
    bcopy(nhrp_reqsrc(req, fun), srcp, fun->nhrp_addlen);
    bcopy(nhrp_reqdst(req, fun), dstp, fun->nhrp_addlen);
    nhop = nhrp_rep2nbma(rep, fun);
    bcopy(nhop_addr, nhrp_repnhop(rep, fun), fun->nhrp_addlen);
    positive = (cache_flags & NHRP_PFLAG);
    nhop->hold = nhrp_hold(if_in, positive, fun);
    nhop->nbma_len = nbma_len;
    if (nbma) {
	pnbma = nhrp_req2nbma(req, fun);
	nhop->addr_type = pnbma->addr_type;
    } else {
	nhop->addr_type = 0;
    }
    bcopy(nbma, &nhop->nbma, nbma_byte_len);
    if (nhrp_debug)
	buginf("\nNHRP: Send %sreply %s",
	       (cache_flags & NHRP_PFLAG) ? "" : "negative ",
	       nhrp_sprint_nbma(nbma_buf, nbma, nbma_len, nhop->addr_type));
    optlen = nhrp_parse_options(req_pak, nhrpopstart(req, req->mand_len),
				nhrpopstart(rep, mand_len),
				NHRP_OPLEN_MAXTOTAL, 0, TRUE, &mask, fun);
    if (optlen == 0) {
	retbuffer(pak);
	return;
    }
    nhrp_len = NHRP_FIXEDLEN + mand_len + optlen;
    /*
     * Find next hop on the NBMA network
     */
    if ((*fun->nhrp_isbroad)(srcp, if_in)) {
	if (nhrp_debug)
	    buginf("\nNHRP: Suppressing reply to broadcast address");
	retbuffer(pak);
	return;
    }
    nhrp_send(pak, (nhrpfixedtype *)rep, nhrp_len, (*fun->nhrp_ifaddr)(if_in),
	      (nhrpaddr *)srcp, if_in, fun);
}

/*
 * nhrp_send_rep_cache
 * Wrapper routine for sending an NHRP reply.  Pull the NBMA information
 * we intend to reply with out of a cache table entry.
 */

static void nhrp_send_rep_cache (paktype *req_pak, nhrpreqreptype *req,
			  uchar cache_flags, nhrpcachetype *ptr, nhrpfuns *fun)
{
    if (!nhrp_cache_nei(nhrp_reqsrc(req, fun), req->reqid, ptr, fun))
	return;
    nhrp_send_rep(req_pak, req, cache_flags, ptr->nbma_len,
		  ptr->nbma.opaque, &ptr->nhop, &ptr->prefix.ip.mask, fun);
}

/*
 * nhrp_rep_ouraddr
 * Reply to an NHRP request using our interface's NBMA address, and our
 * network layer address as the next hop.
 */

static void nhrp_rep_ouraddr (paktype *pak, nhrpreqreptype *req, ushort addr_type,
		       idbtype *if_in, nhrp_mask mask, nhrpfuns *fun)
{
    nhrpnbmatype hw;
    nhrpaddrtype nbma;
    nhrpcachetype *ptr;
    nhrpaddr *addr;

    if (!nhrp_our_nbma(addr_type, if_in, &nbma, &hw.nbma_len))
	return;
    hw.addr_type = addr_type;
    hw.nbmaptr = &nbma;
    addr = nhrp_reqdst(req, fun);
    ptr = nhrp_tap_ourself(if_in, addr, mask, &hw, fun);
    if (!ptr)
	return;
    nhrp_send_rep_cache(pak, req, NHRP_AFLAG|NHRP_PFLAG|NHRP_SFLAG, ptr, fun);
}

/*
 * nhrp_get_ev
 * Get some memory in which to queue an NHRP event.  Complain if not
 * available.
 */

static inline nhrp_eventtype *nhrp_get_ev (void)
{
    nhrp_eventtype *ev;

    ev = malloc(sizeof(nhrp_eventtype));
    return(ev);
}

/*
 * nhrp_req_enq
 * Enqueue a request event.
 * Throttling should happen here...
 */

void nhrp_req_enq (idbtype *if_out, nhrpaddr *dest,
		   boolean refresh, nhrpfuns *fun)
{
    nhrp_eventtype *ev;

    /*
     * Check if there exists a VC for the target, if yes, no need to
     * send resolution request.
     */
    if (nhrp_vc_exists(if_out, dest, fun))
	return;

    ev = nhrp_get_ev();
    if (!ev)
	return;
    ev->type = NHRP_EV_REQ;
    ev->ev.req.if_out = if_out;
    bcopy(dest, &ev->ev.req.dest, fun->nhrp_addlen);
    ev->ev.req.refresh = refresh;
    ev->fun = fun;
    process_enqueue(nhrp_eventQ, ev);
}

/*
 * nhrp_send_req
 * Send an NHRP request packet.
 */

static void nhrp_send_req (idbtype *if_out, nhrpaddr *dest,
		    boolean refresh, nhrpfuns *fun)
{
    paktype *pak;
    nhrpreqreptype *req;
    ulong nhrp_len, optlen;
    nhrpaddr *src, *nhs, *srcp, *dstp;
    ushort addr_type;
    nhrpaddrtype nbma;
    uchar nbma_len, nbma_byte_len;
    ushort mand_len, nhoplen;
    nhrpnhoptype *pnbma;

    addr_type = nhrp_nbma_type(if_out);
    if (!nhrp_our_nbma(addr_type, if_out, &nbma, &nbma_len))
	return;
    nbma_byte_len = NHRP_BYTE_LEN(nbma_len);
    nhoplen = NHRP_NHOPLEN + NHRP_ALIGN(fun->nhrp_addlen);
    mand_len = 2*NHRP_ALIGN(fun->nhrp_addlen) +
	NHRP_ALIGN(NHRP_REQLEN + nbma_byte_len + nhoplen);
    nhrp_len = NHRP_FIXEDLEN + mand_len + NHRP_OPLEN_DEF;
    /*
     * See if we want to try to send a request.  If we are attempting to
     * refresh an existing entry, skip check.
     */
    if (!refresh && !nhrp_revise_inc(if_out, dest, fun))
	return;
    /*
     * Now, see if we are allowed to send it.
     */
    if (nhrp_toomany(if_out, fun))
	return;
    pak = nhrp_getbuffer(nhrp_len);
    if (!pak)
	return;
    req = (nhrpreqreptype *)nhrpfixstart(pak);
    memset(req, 0, nhrp_len);
    req->version = NHRP_VERSION;
    req->hop = NHRP_MAXHOPS;
    req->pkt_type = NHRP_REQUEST;
    req->mand_len = mand_len;
    req->flags = NHRP_QFLAG;
    req->protoid = fun->protoid;
    req->reqid = nhrp_req_id++;
    src = (*fun->nhrp_ifaddr)(if_out);
    srcp = nhrp_reqsrc(req, fun);
    dstp = nhrp_reqdst(req, fun);
    bcopy(src, srcp, fun->nhrp_addlen);
    bcopy(dest, dstp, fun->nhrp_addlen);

    pnbma = nhrp_req2nbma(req, fun);
    pnbma->hold = nhrp_hold(if_out, TRUE, fun);
    pnbma->addr_type = addr_type;
    pnbma->nbma_len = nbma_len;
    bcopy(nbma.opaque, &pnbma->nbma, nbma_byte_len);
    nhs = nhrp_find_nhs(if_out, dest, fun, TRUE);
    if (!nhs) {
	/*
	 * If we are not in "server" mode, set IP destination of NHRP
	 * request to the ultimate destination.
	 */
	nhs = dest;
    }
    optlen = nhrp_add_options(nhrpopstart(req, mand_len), if_out, fun);
    nhrp_len = NHRP_FIXEDLEN + mand_len + optlen;
    nhrp_send(pak, (nhrpfixedtype *)req, nhrp_len, src, nhs, if_out, fun);
}

/*
 * nhrp_forward
 * Forward an NHRP packet.  Packet may either have been received locally,
 * or intercepted via router alert.
 */

static void nhrp_forward (paktype *pak_in, idbtype *if_out,
		   nhrpreqreptype *req_in, nhrpaddr *dest, nhrpfuns *fun)
{
    nhrpreqreptype *req;
    ulong nhrp_len;
    nhrpaddr *nhs;
    ushort mand_len, optlen;
    paktype *pak;
    nhrpnhoptype *nbma;
    boolean is_broad;

    if (nhrp_debug)
	buginf("\nNHRP: Forwarding %s to %s",
	       nhrp_ptype_string(req_in->pkt_type), if_out->namestring);
    mand_len = req_in->mand_len;
    nhrp_len = NHRP_FIXEDLEN + mand_len + NHRP_OPLEN_MAXTOTAL;
    if (nhrp_toomany(if_out, fun))
	return;
    pak = nhrp_getbuffer(nhrp_len);
    if (!pak)
	return;
    req = (nhrpreqreptype *)nhrpfixstart(pak);
    memset(req, 0, nhrp_len);
    bcopy(req_in, req, NHRP_FIXEDLEN);
    req->unused0 = 0;
    req->cksum = 0;
    bcopy(&req_in->flags, &req->flags, mand_len);
    req->unused1 = 0;
    if (req_in->pkt_type == NHRP_REQUEST) {
	nbma = nhrp_req2nbma(req, fun);
	nbma->pref = 0;
    }
    nhs = nhrp_find_nhs(if_out, dest, fun, TRUE);
    if (!nhs) {
	/*
	 * If we are not in "server" mode, set IP destination of NHRP
	 * request to the ultimate destination.
	 */
	nhs = dest;
    }
    is_broad = (*fun->nhrp_isbroad)(nhs, if_out);
    if (is_broad) {
	if ((pak_in->flags & PAK_BROADCAST) && (pak_in->if_input == if_out)) {
	    if (nhrp_debug)
		buginf("\nNHRP: Suppressing broadcast forwarding -"
		       " same interface: %s", if_out->namestring);
	    retbuffer(pak);
	    return;
	}
    }
    optlen = nhrp_parse_options(pak_in, nhrpopstart(req_in, mand_len),
				nhrpopstart(req, mand_len),
				NHRP_OPLEN_MAXTOTAL, req_in->pkt_type, FALSE,
				NULL, fun);
    if (optlen == 0) {
	retbuffer(pak);
	return;
    }
    nhrp_len = NHRP_FIXEDLEN + mand_len + optlen;
    nhrp_send(pak, (nhrpfixedtype *)req, nhrp_len,
	      (*fun->nhrp_ifaddr)(if_out), nhs, if_out, fun);
}

/*
 * nhrp_recv_request
 * Handle receipt of an NHRP request packet.
 */

static void nhrp_recv_request (paktype *pak, nhrpreqreptype *req, nhrpfuns *fun)
{
    nhrpcachetype *ptr;
    nhrpaddr *dest;
    uchar nbma_len, *nbma_ptr;
    idbtype *if_in, *if_out, *if_addr;
    ulong netid_in, netid_out;
    ushort addr_type;
    nhrpnbmatype hw;
    void *opaque;
    nhrp_mask mask;
    nhrpnhoptype *nbma;
    boolean static_route;

    if_in = pak->if_input;
    netid_in = nhrp_netid(if_in, fun);
    dest = nhrp_reqdst(req, fun);
    nbma = nhrp_req2nbma(req, fun);
    addr_type = nhrp_nbma_type(if_in);
    if (nbma->addr_type != addr_type) {
	/*
	 * Packet came in interface which isn't actually
	 * part of the fabric.  
	 */
	if (nhrp_debug)
	    buginf("\nNHRP: Request for addr type %d received"
		   " on interface %s, should be %d", nbma->addr_type,
		   if_in->namestring, addr_type);
	return;
    }
    if_addr = nhrp_ouraddress(dest, fun, netid_in);
    if (if_addr) {
	nhrp_rep_ouraddr(pak, req, addr_type, if_addr, NULL, fun);
	return;
    }
    if_out = (*fun->nhrp_rtlookup)(dest, &mask, &opaque, &static_route);
    if (if_out) {
	netid_out = nhrp_netid(if_out, fun);
	if (netid_in != netid_out) {
	    if (nhrp_debug) {
		nhrp_bugaddr("\nNHRP: We are egress router for ",
			     dest, fun);
		buginf(", received via %s", if_in->namestring);
	    }
	    /*
	     * We are the egress router
	     */
	    if (fun->linktype == LINK_IP)
	        nhrp_ip_rtrevise(opaque);
	    nhrp_rep_ouraddr(pak, req, addr_type, if_in, mask, fun);
	    return;
	}
    }
    nbma_ptr = nhrp_proto_resolve(if_in, addr_type, dest, &nbma_len, fun);
    if (nbma_ptr) {
	if (nhrp_debug) {
	    nhrp_bugaddr("\nNHRP: Found map entry for ", dest, fun);
	    buginf(" in media specific map for %s", if_in->namestring);
	}
        /*
	 * Send reply
	 */
	hw.nbmaptr = (nhrpaddrtype *)nbma_ptr;
	hw.addr_type = addr_type;
	hw.nbma_len = nbma_len;
	ptr = nhrp_tap_ourself(if_in, dest, NULL, &hw, fun);
	if (!ptr)
	    return;
	nhrp_send_rep_cache(pak, req, NHRP_AFLAG|NHRP_PFLAG|NHRP_SFLAG,
			    ptr, fun);
	return;
    }
    ptr = nhrp_cache_lookup_comp(if_in, dest, fun);
    if (ptr) {
        /*
	 * Send reply
	 */
	nhrp_send_rep_cache(pak, req, ptr->flags, ptr, fun);
	return;
    }
    if (if_out) {
	/*
	 * Forward request
	 */
	nhrp_forward(pak, if_out, req, dest, fun);
	return;
    } else {
	/*
	 * No route, return negative reply
	 */
	nhrp_send_rep(pak, req, 0, 0, NULL, (*fun->nhrp_ifaddr)(if_in),
		      NULL, fun);
	return;
    }
}

/*
 * nhrp_recv_reply
 * Process an NHRP reply packet.  Update our cache with the NBMA information
 * contained within the response.
 */

static void nhrp_recv_reply (paktype *pak, nhrpreqreptype *rep, nhrpfuns *fun)
{
    nhrpcachetype *ptr;
    nhrpaddr *dest, *src;
    idbtype *swidb, *if_out;
    ushort optlen;
    nhrp_mask mask;
    nhrpnhoptype *nhop;
    addrtype proto_addr;

    /*
     * Sanity check our options, and extract the mask
     */
    optlen = nhrp_parse_options(pak, nhrpopstart(rep, rep->mand_len), NULL,
                                NHRP_OPLEN_MAXTOTAL, 0, FALSE, &mask,
                                fun);
    if (optlen != NHRP_OPLEN_MAXTOTAL) {
        /*
         * Options didn't parse.
         */
        return;
    }
    dest = nhrp_reqdst(rep, fun);
    src = nhrp_reqsrc(rep, fun);
    swidb = pak->if_input;
    ptr = nhrp_cache_lookup_mask(swidb, dest, mask, fun);
    if (ptr) {
	/*
	 * Update cache
	 */
	nhrp_cache_reply(ptr, swidb, dest, mask, rep, fun);
    } else {
	nhrp_cache_add(swidb, dest, mask, nhrp_cache_reply, rep, fun);
    }
    if (nhrp_extractmask(mask) != MASK_HOST) {
	/*
	 * Cleanup any incomplete cache entries with a host mask.
	 */
	ptr = nhrp_cache_lookup(swidb, dest, fun);
	if (ptr && ptr->mode == NHRP_INCOMPLETE)
	    nhrp_cache_delete_subr(ptr);
    }

    /*
     * Create static map and blown out route cache. This is to make sure
     * that the following data would take new built-up cut-through path
     * instead of taking the normal route path.
     */
    memset(&proto_addr, 0, sizeof(addrtype));
    bcopy(dest, &proto_addr.addr, fun->nhrp_addlen);
    proto_addr.type = fun->addrtype;
    proto_addr.length = fun->nhrp_addlen;
    nhop = nhrp_rep2nbma(rep, fun);
    nhrp_add_map(swidb, &proto_addr, nhop->nbma, fun);

    if (!nhrp_ouraddress(src, fun, NHRP_NETID_UNDEF)) {
	/*
	 * Reply is not for us.  Try to forward it.
	 */
	if_out = nhrp_nextidb(NULL, src, FALSE, fun);
	if (if_out == NULL) {
	    if (nhrp_debug)
		nhrp_bugaddr("\nNHRP: Cannot route reply packet for"
			     " destination ", src, fun);
	    return;
	}
	nhrp_forward(pak, if_out, rep, src, fun);
    }
}

/*
 * nhrp_queue_retrans
 * Prepare a timer to manage retransmission of a purge message.
 */

void nhrp_queue_retrans (nhrpcacherequestor *nbr, sys_timestamp expire)
{
    if (TIMER_RUNNING_AND_AWAKE(expire))
	return;
    nbr->ret = 1;
    nbr->expire = expire;
    mgd_timer_init_leaf(&nbr->retrans, &nhrp_master_t, NHRP_TIM_PURGE,
			nbr, FALSE);
    mgd_timer_start_jittered(&nbr->retrans, nbr->ret * ONESEC, 25);
}

/*
 * nhrp_send_purge_subr
 * Send an NHRP purge packet.
 */

static void nhrp_send_purge_subr (nhrpaddr *dest, nhrpaddr *purgedest,
			   ulong reqid, boolean ack, nhrpfuns *fun)
{
    paktype *pak;
    idbtype *if_out;
    nhrpreqreptype *pur;
    ushort mand_len;
    ulong nhrp_len;
    nhrpaddr *srcp, *oursrc;

    if_out = nhrp_nextidb(NULL, dest, FALSE, fun);
    if (if_out == NULL) {
	if (nhrp_debug)
	    nhrp_bugaddr("\nNHRP: Cannot route purge packet for destination ",
		   dest, fun);
	return;
    }
    mand_len = NHRP_ALIGN(fun->nhrp_addlen) * 3 + NHRP_REQLEN;
    nhrp_len = NHRP_FIXEDLEN + mand_len;
    pak = nhrp_getbuffer(nhrp_len);
    if (!pak)
	return;
    pur = (nhrpreqreptype *)nhrpfixstart(pak);
    memset(pur, 0, nhrp_len);
    pur->version = NHRP_VERSION;
    pur->hop = NHRP_MAXHOPS;
    pur->pkt_type = NHRP_PURGE;
    pur->mand_len = mand_len;
    pur->protoid = fun->protoid;
    pur->reqid = reqid;
    srcp = nhrp_reqsrc(pur, fun);
    bcopy(purgedest, srcp, fun->nhrp_addlen);
    oursrc = (*fun->nhrp_ifaddr)(if_out);
    bcopy(oursrc, nhrp_pursrc(pur, fun), fun->nhrp_addlen);
    if (ack)
	pur->flags = NHRP_PURGE_AFLAG;
    nhrp_send(pak, (nhrpfixedtype *)pur, nhrp_len, oursrc, dest, if_out, fun);
}

/*
 * nhrp_send_purge
 * Send an NHRP purge packet.
 */

void nhrp_send_purge (nhrpcacherequestor *nbr)
{
    nhrp_send_purge_subr(&nbr->src, &nbr->src, nbr->reqid, FALSE,
			 nhrp_funmap(nbr->protoidx));
}

/*
 * nhrp_send_purge_all
 * Send NHRP purge packets to all the configured NHSs. This is called
 * when NHRP is turned off.
 */

void nhrp_send_purge_all (nhrpproinfotype *pro, nhrpfuns *fun)
{
    nhrpnhstype *nhs;

    /*
     * Is NHS configured ?
     */
    nhs = pro->nhrp_nhstable;
    if (!nhs)
	return;

    /*
     * Send to each configured NHS exactly one purge request packet.
     * Request ID is set to 0.
     */
    for (; nhs; nhs = nhs->next) {
	nhrp_send_purge_subr(&nhs->nhs, &nhs->nhs, 0, FALSE, fun);
    }
}

/*
 * nhrp_recv_purge_ack
 * Receive a purge acknowledgment.  Disarm the corresponding timer.
 */

static void nhrp_recv_purge_ack (paktype *pak, nhrpreqreptype *pur, nhrpfuns *fun)
{
    mgd_timer *leaf;
    nhrpcacherequestor *nbr;
    nhrpaddr *src;

    /*
     * Wade through timer queue searching for the entry which matches
     * the request ID from the purge message
     */
    src = nhrp_reqsrc(pur, fun);
    for (leaf = mgd_timer_first_child(&nhrp_master_t);
	 leaf; leaf = mgd_timer_next_running(leaf)) {
	if (mgd_timer_type(leaf) != NHRP_TIM_PURGE)
	    continue;
	nbr = mgd_timer_context(leaf);
	if (nbr->reqid != pur->reqid)
	    continue;
	if (bcmp(&nbr->src, src, fun->nhrp_addlen) != 0)
	    continue;
	if (nhrp_debug)
	    nhrp_bugaddr("\nNHRP: Accepted ACK for purge of ", src, fun);
	mgd_timer_stop(leaf);
	free(nbr);
	return;
    }
    if (nhrp_debug) {
	buginf("\nNHRP: Rejected Purge ACK %u ", pur->reqid);
	nhrp_bugaddr("for ", src, fun);
    }
}

/*
 * nhrp_recv_purge
 * Porcess an NHRP purge packet.  Flush the corresponding cache information.
 * Acknowledge the packet.
 */

static void nhrp_recv_purge (paktype *pak, nhrpreqreptype *pur, nhrpfuns *fun)
{
    nhrpaddr *src;

    if (pur->flags & NHRP_PURGE_AFLAG) {
	nhrp_recv_purge_ack(pak, pur, fun);
	return;
    }
    /*
     * Send acknowledgment
     */
    src = nhrp_pursrc(pur, fun);
    nhrp_send_purge_subr(src, nhrp_reqsrc(pur, fun), pur->reqid, TRUE, fun);
    /*
     * Purge our cache entry
     */
    nhrp_cache_purge_id(pak->if_input, pur->reqid, fun);
}

/*
 * nhrp_recv_reg
 * Process an NHRP register packet.  Update our cache with the NBMA information
 * contained within the packet.
 */

static void nhrp_recv_reg (paktype *pak, nhrpregtype *reg, nhrpfuns *fun)
{
    nhrpcachetype *ptr;
    nhrpaddr *dest;
    idbtype *swidb;

    dest = nhrp_regsrc(reg, fun);
    swidb = pak->if_input;
    ptr = nhrp_cache_lookup(swidb, dest, fun);
    if (ptr) {
	/*
	 * Update cache
	 */
	nhrp_cache_reg(ptr, swidb, dest, NULL, reg, fun);
    } else {
	nhrp_cache_add(swidb, dest, NULL, nhrp_cache_reg, reg, fun);
    }
}

/*
 * nhrp_cache_map
 * Update cache information based upon static mapping.
 */

static boolean nhrp_cache_map (nhrpcachetype *ptr, 
			idbtype *idb, nhrpaddr *dest, nhrp_mask mask,
			void *arg, nhrpfuns *fun)
{
    nhrphwaddrtype *hw;

    hw = arg;
    if (!nhrp_valid_nbma_len(NHRP_BYTESIZE * hw->nbma_len, hw->addr_type)) {
	return(FALSE);
    }
    bcopy(hw->nbma.opaque, ptr->nbma.opaque, hw->nbma_len);
    ptr->nbma_len = NHRP_BYTESIZE * hw->nbma_len;
    ptr->addr_type = hw->addr_type;
    nhrp_cache_populate(ptr, idb, NHRP_DEF_HOLDTIME, dest, dest, hw->flags,
			NHRP_STATIC, fun);
    if (mask)
	(*fun->nhrp_copymask)(mask, ptr);
    return(TRUE);
}

/*
 * nhrp_map_mcast
 * Add or delete static mapping table information regarding the
 * broadcast/multicast address.
 */

void nhrp_map_mcast (idbtype *swidb, nhrphwaddrtype *hw, boolean sense,
		     nhrpfuns *fun)
{
    nhrpmcasttype *pred, *ptr;
    nhrpinfotype *nhrpinfo;

    nhrpinfo = swidb->nhrp_info;
    if (!nhrpinfo)
	return;
    pred = (nhrpmcasttype *)&nhrpinfo->pro[fun->protoidx].nhrp_mcast;
    for (ptr = pred->next; ptr; pred = ptr, ptr = ptr->next) {
	if (hw->nbma_len != ptr->nbma_len)
	    continue;
	if (bcmp(ptr->nbma.opaque, hw->nbma.opaque, ptr->nbma_len) != 0)
	    continue;
	/*
	 * Found match.  If adding, then nothing needs to be done.
	 */
	if (sense)
	    return;
	/*
	 * Remove entry.
	 */
	pred->next = ptr->next;
	free(ptr);
	return;
    }
    if (!sense)
	return;
    /*
     * Need to create new entry.
     */
    ptr = malloc(sizeof(nhrpmcasttype));
    if (ptr == NULL) {
	printf(nomemory);
	return;
    }
    ptr->nbma_len = hw->nbma_len;
    ptr->addr_type = hw->addr_type;
    bcopy(hw->nbma.opaque, ptr->nbma.opaque, ptr->nbma_len);
    pred->next = ptr;
}

/*
 * nhrp_map_command
 * Add/revise/delete static mapping table information from the cache.
 */

void nhrp_map_command (idbtype *swidb, nhrpaddr *dest, nhrp_mask mask,
		       nhrphwaddrtype *hw,
		       boolean sense, nhrpfuns *fun)
{
    nhrpcachetype *ptr;

    ptr = nhrp_cache_lookup_mask(swidb, dest, mask, fun);
    if (sense) {
	if (ptr) {
	    /*
	     * Revise existing entry
	     */
	    nhrp_cache_map(ptr, swidb, dest, mask, hw, fun);
	} else {
	    /*
	     * Create new entry
	     */
	    nhrp_cache_add(swidb, dest, mask, nhrp_cache_map, hw, fun);
	}
    } else {
	if (!ptr)
	    return;
	/*
	 * Delete entry
	 */
	nhrp_cache_delete(swidb, ptr);
    }
}

/*
 * nhrp_pak_sanity
 * Sanity check incoming NHRP packet.  Check protocol version, checksum,
 * network identifier.
 */

static boolean nhrp_pak_sanity (paktype *pak, nhrpfixedtype *fix, nhrpfuns *fun)
{
    ushort pdulen;
    nhrpaddr *src, *dst;
    nhrpreqreptype *req;

    if (fix->version != NHRP_VERSION) {
	if (nhrp_debug) {
	    req = (nhrpreqreptype *)fix;
	    src = nhrp_reqsrc(req, fun);
	    dst = nhrp_reqdst(req, fun);
	    buginf("\nNHRP: Wrong proto version (%d)", fix->version);
	    nhrp_bugaddr(" src ", src, fun);
	    nhrp_bugaddr(" dst ", dst, fun);
	}
	return(FALSE);
    }
    pdulen = nhrp_pdulen(pak);
    if (ipcrc((ushort *)fix, pdulen)) {
	if (nhrp_debug) {
	    req = (nhrpreqreptype *)fix;
	    src = nhrp_reqsrc(req, fun);
	    dst = nhrp_reqdst(req, fun);
	    buginf("\nNHRP: Bad checksum");
	    nhrp_bugaddr(" src ", src, fun);
	    nhrp_bugaddr(" dst ", dst, fun);
	}
	return(FALSE);
    }
    if (!nhrp_if(pak->if_input, fun)) {
	if (nhrp_debug)
	    buginf("\nNHRP: NHRP packet via %s, not configured for NHRP",
		   pak->if_input->namestring);
	return(FALSE);
    }
    return(TRUE);
}

/*
 * nhrp_bugpak
 * Display debugging information for an NHRP packet.
 */

static inline void nhrp_bugpak (paktype *pak, uchar *string, nhrpfuns *fun)
{
    iphdrtype *ip;
    nhrpreqreptype *req;

    req = nhrpfixstart(pak);
    if (pak->linktype == LINK_IP) {
	ip = iphdrtype_start(pak);
	buginf("\nNHRP: %s %s ipsrc %i ipdst %i Size %d", string,
	       nhrp_ptype_string(req->pkt_type), ip->srcadr, ip->dstadr,
	       nhrp_pdulen(pak));
    } else {
	buginf("\nNHRP: %s %s Size %d", string,
	       nhrp_ptype_string(req->pkt_type), nhrp_pdulen(pak));
    }
}

/*
 * nhrp_recv_err
 * Process an incoming error packet.
 */

static void nhrp_recv_err (paktype *pak, nhrperrtype *err)
{
}

/*
 * nhrp_enqueue
 * Enqueue a received NHRP packet.
 */

void nhrp_enqueue (paktype *pak)
{
    if (!nhrp_pid)
	retbuffer(pak);
    else
	process_enqueue(nhrp_packetQ, pak);
}

/*
 * nhrp_recv
 * Receive an NHRP packet, sanity check it, and demultiplex based upon
 * packet type.
 */

static void nhrp_recv (paktype *pak)
{
    nhrpreqreptype *req;
    nhrpfixedtype *fix;
    nhrpfuns *fun;

    fix = nhrpfixstart(pak);
    req = (nhrpreqreptype *)fix;
    switch (fix->pkt_type) {
    case NHRP_REQUEST:
    case NHRP_REPLY:
    case NHRP_PURGE:
    case NHRP_REG:
	fun = nhrp_funmap(nhrp_proto2idx(req->protoid));
	break;
    case NHRP_ERR:
	/*
	 * XXX protocol bug.  Must assume IP for now.
	 */
	fun = nhrp_funmap(NHRP_NETIP);
	break;
    default:
	if (nhrp_debug)
	    printf("\nNHRP: Unknown NHRP packet type %d", fix->pkt_type);
	retbuffer(pak);
	return;
    }
    if (nhrp_debug) {
	nhrp_bugpak(pak, "Recv", fun);
	nhrp_bugpak_mand(req, pak, fun);
    }
    if (nhrp_debug_opt)
	nhrp_show_options(pak, nhrpopstart(req, req->mand_len), fun);
    if (nhrp_pak_sanity(pak, fix, fun)) {
	nhrp_stat(pak->if_input, fix->pkt_type, TRUE, fun);
	switch (fix->pkt_type) {
	case NHRP_REQUEST:
	    nhrp_recv_request(pak, req, fun);
	    break;
	case NHRP_REPLY:
	    nhrp_recv_reply(pak, req, fun);
	    break;
	case NHRP_PURGE:
	    nhrp_recv_purge(pak, req, fun);
	    break;
	case NHRP_ERR:
	    nhrp_recv_err(pak, (nhrperrtype *)fix);
	    break;
	case NHRP_REG:
	    nhrp_recv_reg(pak, (nhrpregtype *)fix, fun);
	    break;
	}
    }
    retbuffer(pak);
}

/*
 * nhrp_send_reg_subr
 * Send an NHRP register packet to a specific destination.
 */

static void nhrp_send_reg_subr (idbtype *if_out, nhrpaddr *src, nhrpaddr *nhs,
			 uchar nbma_len, uchar *nbma, ushort addr_type,
			 nhrpfuns *fun)
{
    paktype *pak;
    ushort nhrp_len, mand_len, nbma_byte_len, optlen;
    nhrpregtype *reg;
    nhrpnhoptype *nhop;

    nbma_byte_len = NHRP_BYTE_LEN(nbma_len);
    mand_len = NHRP_ALIGN(fun->nhrp_addlen) +
	NHRP_ALIGN(NHRP_REGLEN + nbma_byte_len);
    nhrp_len = NHRP_FIXEDLEN + mand_len + NHRP_OPLEN_DEF;
    pak = nhrp_getbuffer(nhrp_len);
    if (!pak)
	return;
    reg = (nhrpregtype *)nhrpfixstart(pak);
    memset(reg, 0, nhrp_len);
    reg->version = NHRP_VERSION;
    reg->hop = NHRP_MAXHOPS;
    reg->pkt_type = NHRP_REG;
    reg->mand_len = mand_len;
    reg->protoid = fun->protoid;
    bcopy(src, nhrp_regsrc(reg, fun), fun->nhrp_addlen);
    nhop = nhrp_reg2nbma(reg, fun);
    nhop->hold = nhrp_hold(if_out, TRUE, fun);
    nhop->addr_type = addr_type;
    nhop->nbma_len = nbma_len;
    bcopy(nbma, &nhop->nbma, nbma_byte_len);
    if (nhrp_debug)
	nhrp_bugaddr("\nNHRP: Send register to ", nhs, fun);
    optlen = nhrp_add_options(nhrpopstart(reg, mand_len), if_out, fun);
    nhrp_len = NHRP_FIXEDLEN + mand_len + optlen;
    nhrp_send(pak, (nhrpfixedtype *)reg, nhrp_len, src, nhs, if_out, fun);
}

/*
 * nhrp_send_reg
 * Send an NHRP register packet to all of our configured NHSes.
 */

static void nhrp_send_reg (idbtype *swidb, nhrpnhstype *nhs, nhrpfuns *fun)
{
    nhrpnhstype *ptr;
    uchar nbma_len;
    nhrpaddrtype nbma;
    ushort addr_type;
    nhrpaddr *src;
    nhrpinfotype *nhrpinfo;

    nhrpinfo = swidb->nhrp_info;
    if (!nhrpinfo)
	return;
    ptr = nhrpinfo->pro[fun->protoidx].nhrp_nhstable;
    if (ptr == NULL)
	return;
    addr_type = nhrp_nbma_type(swidb);
    if (addr_type == AF_RESERVED)
	return;
    if (!nhrp_our_nbma(addr_type, swidb, &nbma, &nbma_len))
	return;
    src = (*fun->nhrp_ifaddr)(swidb);

    /*
     * If send to a specific NHS, do not reset the registration timer.
     */
    if (nhs) {
	nhrp_send_reg_subr(swidb, src, &nhs->nhs, nbma_len, nbma.opaque,
			   addr_type, fun);
	return;
    }

    /*
     * Send to each configured NHS and reset the transmission timer.
     */
    for (; ptr; ptr = ptr->next) {
	nhrp_send_reg_subr(swidb, src, &ptr->nhs, nbma_len, nbma.opaque,
			   addr_type, fun);
    }
    nhrpinfo->pro[fun->protoidx].nhrp_reg_hold = 0;
}

/*
 * nhrp_if_up
 * Interface coming up.  Send out register packet.
 */

static void nhrp_if_up (idbtype *swidb)
{
    nhrpfuns *fun;
    ushort i;

    for (i = 0; i < NHRP_NETPROTOCNT; i++) {
	fun = nhrp_funmap(i);
	if (!fun)
	    continue;
	if (!nhrp_if(swidb, fun))
	    continue;
	if (nhrp_debug)
	    buginf("\nNHRP: if_up: %s proto %d", swidb->namestring,
		   i);
	nhrp_send_reg(swidb, NULL, fun);
    }
}

/*
 * nhrp_if_down
 * Interface going down.  Flush dynamic cache information.
 */

static void nhrp_if_down (idbtype *swidb)
{
    nhrpfuns *fun;
    ushort i;

    for (i = 0; i < NHRP_NETPROTOCNT; i++) {
	fun = nhrp_funmap(i);
	if (!fun)
	    continue;
	if (!nhrp_if(swidb, fun))
	    continue;
	if (nhrp_debug)
	    buginf("\nNHRP: if_down: %s proto %d", swidb->namestring, i);
	nhrp_cache_clear(swidb, fun);
    }
}

/*
 * nhrp_announce
 * Send register packets to everybody...
 */

static void nhrp_announce (void)
{
    idbtype *swidb;
    nhrpfuns *fun;
    ushort i;
    nhrpinfotype *nhrpinfo;

    for (i = 0; i < NHRP_NETPROTOCNT; i++) {
	fun = nhrp_funmap(i);
	if (!fun)
	    continue;
	FOR_ALL_SWIDBS(swidb) {
	    nhrpinfo = swidb->nhrp_info;
	    if (!nhrpinfo)
		continue;
	    if (!nhrpinfo->pro[i].nhrp_nhstable)
		continue;
	    if (!nhrp_if(swidb, fun))
		continue;
	    nhrp_send_reg(swidb, NULL, fun);
	}
    }
}

/*
 * nhrp_init
 * Initialize global NHRP data structure, register our functions.
 * Called at sytem initialization time.
 */

static void nhrp_init (subsystype *subsys)
{
    nhrp_req_id = 0;
    nhrp_pid = 0;

    nhrp_media_registry();
    nhrp_parser_init();
    nhrp_vc_init();

    mgd_timer_init_parent(&nhrp_master_t, NULL);
    mgd_timer_init_leaf(&nhrp_periodic_t, &nhrp_master_t,
			NHRP_TIM_PERIODIC, NULL, FALSE);

    reg_add_raw_enqueue(LINK_NHRP, nhrp_enqueue, "nhrp_enqueue");
    reg_add_onemin(nhrp_cache_age, "nhrp_cache_age");
    reg_add_default_media_tunnel_bcast(nhrp_tunnel_bcast, "nhrp_tunnel_bcast");
    reg_add_ar_cache_clear(nhrp_cache_clear_wrap, "nhrp_cache_clear_wrap");
    reg_add_swif_erase(nhrp_cache_if_delete, "nhrp_cache_if_delete");
    reg_add_swif_comingup(nhrp_if_up, "nhrp_if_up");
    reg_add_swif_goingdown(nhrp_if_down, "nhrp_if_down");
    reg_add_restarted(nhrp_announce, "nhrp_announce");
    reg_add_nhrp_on_if(nhrp_on_if, "nhrp_on_if");
}

/*
 * nhrp_resolve_nhop
 * Determine the next hop NBMA address for a particular IP destination.
 */

boolean nhrp_resolve_nhop (idbtype *if_out, nhrpaddr *nhop,
			   uchar const **macaddr, ushort addr_type,
			   nhrpfuns *fun)
{
    nhrpcachetype *ptr;
    uchar *transport_dest;

    ptr = nhrp_cache_lookup_comp(if_out, nhop, fun);
    if (ptr == NULL)
	return(FALSE);
    if (ptr->addr_type == AF_RESERVED) {
	if (nhrp_debug)
	    nhrp_bugaddr("\nNHRP: Cache lookup returned negative response on ",
			 nhop, fun);
	return(FALSE);
    }
    if (ptr->addr_type != addr_type) {
	if (nhrp_debug) {
	    nhrp_bugaddr("\nNHRP: Cache lookup on ", nhop, fun);
	    buginf(" returned addr type %d, expected addr type %d",
		   ptr->addr_type, addr_type);
	}
	return(FALSE);
    }
    /*
     * Mark cache entry as used
     */
    ptr->flags |= NHRP_UFLAG;
    transport_dest = ptr->nbma.opaque;
    if (!transport_dest)
	return(FALSE);
    *macaddr = transport_dest;
    return(TRUE);
}

/*
 * nhrp_our_purge_pak
 * Return an indication as to whether or not this is a locally generated
 * purge packet.
 */

boolean nhrp_our_purge_pak (paktype *pak)
{
    nhrpreqreptype *pur;

    if (pak->if_input)
	return(FALSE);
    pur = nhrpfixstart(pak);
    return(pur->pkt_type == NHRP_PURGE);
}

/*
 * nhrp_update_nhs
 * Add/delete an NHS from the table of configured NHSes.
 */

void nhrp_update_nhs (idbtype *idb, nhrpaddr *nhs, nhrpnet *net,
		      boolean sense, nhrpfuns *fun)
{
    nhrpnhstype *ptr, *pred;
    nhrpinfotype *nhrpinfo;
    nhrpproinfotype *nhrppro;
    nhrpcacherequestor *req;

    nhrpinfo = idb->nhrp_info;
    if (!nhrpinfo)
	return;
    nhrppro = &nhrpinfo->pro[fun->protoidx];
    pred = (nhrpnhstype *)&nhrppro->nhrp_nhstable;
    for (ptr = pred->next; ptr; pred = ptr, ptr = ptr->next) {
	if ((*fun->nhrp_cmpnet)(&ptr->net, net))
	    continue;
	if ((*fun->nhrp_cmpaddr)(&ptr->nhs, nhs))
	    continue;
	if (sense) {
	    /*
	     * Entry already exists
	     */
	    return;
	}

	/*
	 * Trigger purge request.
	 */
	if (nhrp_if(idb, fun)) {
	    req = malloc(sizeof(nhrpcacherequestor));
	    if (req) {
		req->protoidx = fun->protoidx;
		bcopy(nhs, &req->src, fun->nhrp_addlen);
		TIMER_START(req->expire, NHRP_RETRANS_EXP_TIME);
		nhrp_queue_retrans(req, req->expire);
		nhrp_send_purge(req);
	    }
	}
	/*
	 * Delete entry
	 */
	pred->next = ptr->next;
	free(ptr);
	return;
    }
    if (sense) {
	/*
	 * Add new entry
	 */
	ptr = malloc(sizeof(nhrpnhstype));
	if (ptr == NULL) {
	    printf(nomemory);
	    return;
	}
	bcopy(net, &ptr->net, fun->nhrp_netlen);
	bcopy(nhs, &ptr->nhs, fun->nhrp_addlen);
	ptr->next = pred->next;
	pred->next = ptr;

	/*
	 * Trigger a registration request.
	 */
	if (nhrp_if(idb, fun) &&
	    ((nhrppro->nhrp_reg_hold + NHRP_PERIODIC) < 
	     (nhrppro->nhrp_hold_pos / 3))) {
	    nhrp_send_reg(idb, ptr, fun);
	}
    }
}

/*
 * nhrp_link2idx
 * Map between a linktype an our internal enumerated type
 */

ushort nhrp_link2idx (ushort linktype)
{
    switch (linktype) {
    case LINK_IP:
    default:
	return(NHRP_NETIP);
    case LINK_APPLETALK:
	return(NHRP_NETAT);
    case LINK_NOVELL:
	return(NHRP_NETIPX);
    }
}

/*
 * nhrp_tunnel_bcast
 * Use NHRP map database to determine where to send multicast/broadcast
 * packet over a multipoint tunnel.  If multiple destinations exist,
 * perform packet replication (yuck).
 */

boolean nhrp_tunnel_bcast (paktype *pak)
{
    idbtype *if_out;
    nhrpmcasttype *ptr;
    boolean found;
    paktype *copy;
    nhrpinfotype *nhrpinfo;
    ushort protoidx;

    if_out = pak->if_output;
    found = FALSE;
    nhrpinfo = if_out->nhrp_info;
    if (!nhrpinfo)
	return(FALSE);
    protoidx = nhrp_link2idx(pak->linktype);
    /*
     * Default to IP
     */
    if (!nhrpinfo->pro[protoidx].nhrp_mcast)
        protoidx = NHRP_NETIP;
    for (ptr = nhrpinfo->pro[protoidx].nhrp_mcast; ptr; ptr = ptr->next) {
	if (!found) {
	    found = TRUE;
	    pak->desthost = ptr->nbma.ip_address;
	} else {
	    copy = pak_duplicate(pak);
	    if (copy == NULL) {
		return(TRUE);
	    }
	    copy->desthost = ptr->nbma.ip_address;
	    /*
	     * Fragmentation?  Accounting?  Local receipt of packet?
	     */
	    datagram_out(copy);
	}
    }
    return(found);
}

/*
 * nhrp_events
 * Go process the event queue
 */

static void nhrp_events (void)
{
    nhrp_eventtype *ev;
    paktype *pak;
    int count;

    count = PAK_SWITCHCOUNT;
    while (count-- > 0) {
	pak = process_dequeue(nhrp_packetQ);
	if (pak == NULL)
	    break;
	nhrp_recv(pak);
    }

    while ((ev = process_dequeue(nhrp_eventQ))) {
	switch (ev->type) {
	case NHRP_EV_REQ:
	    nhrp_send_req(ev->ev.req.if_out, &ev->ev.req.dest,
			  ev->ev.req.refresh, ev->fun);
	    break;
	default:
	    break;
	}
	free(ev);
    }
}

/*
 * nhrp_periodic
 * Called every NHRP_PERIODIC seconds.
 * Replenish per interface grant quota for transmission of NHRP traffic.
 * Note: Individual interface timers must go off no faster than
 * NHRP_PERIODIC.  
 */

static void nhrp_periodic (void)
{
    nhrpfuns *fun;
    idbtype *swidb;
    nhrpinfotype *nhrpinfo;
    nhrpproinfotype *nhrppro;
    ushort i;

    for (i = 0; i < NHRP_NETPROTOCNT; i++) {
	fun = nhrp_funmap(i);
	if (!fun)
	    continue;

	FOR_ALL_SWIDBS(swidb) {
	    nhrpinfo = swidb->nhrp_info;
	    if (!nhrpinfo)
		continue;
	    nhrppro = &nhrpinfo->pro[i];
	    if (nhrppro->nhrp_net_id == NHRP_NETID_UNDEF)
		continue;
	    if (nhrppro->nhrp_secs > NHRP_PERIODIC) {
		/*
		 * when the quota is used up, replenish some of
		 * the quota instead of waiting for nhrp_secs.
		 * otherwise, SVC cut-through may not happen
		 * for the rest of interval in the worst case.
		 */
		ushort ratio;

		nhrppro->nhrp_secs -= NHRP_PERIODIC;
		ratio = nhrppro->nhrp_pkt_cnt * NHRP_PERIODIC;
		ratio /= nhrppro->nhrp_interval;
		nhrppro->nhrp_quota = min(nhrppro->nhrp_pkt_cnt,
					  nhrppro->nhrp_quota + ratio);
		continue;
	    }
	    /*
	     * Timer has gone off
	     */
	    nhrppro->nhrp_secs += (nhrppro->nhrp_interval - NHRP_PERIODIC);
	    /*
	     * Reset grant quota
	     */
	    if (nhrppro->nhrp_pkt_cnt != nhrppro->nhrp_quota) {
		if (nhrp_debug_rate)
		    buginf("\nNHRP-RATE: %s: Used %d", swidb->namestring,
			   nhrppro->nhrp_pkt_cnt - nhrppro->nhrp_quota);
		nhrppro->nhrp_quota = nhrppro->nhrp_pkt_cnt;
	    }

	    /*
	     * Time to send registration request ?
	     */
	    nhrppro->nhrp_reg_hold += NHRP_PERIODIC;
	    if (nhrppro->nhrp_reg_hold > (nhrppro->nhrp_hold_pos / 3)) {
		nhrp_send_reg(swidb, NULL, fun);
		nhrppro->nhrp_reg_hold = 0;
	    }
	}
    }
}

/*
 * nhrp_timer_check
 * Demux timer events.
 */

static void nhrp_timer_check (void)
{
    mgd_timer *expired;
    nhrpcacherequestor *nbr;

    while (mgd_timer_expired(&nhrp_master_t)) {
	expired = mgd_timer_first_expired(&nhrp_master_t);
	switch (mgd_timer_type(expired)) {
	case NHRP_TIM_PURGE:
	    nbr = mgd_timer_context(expired);
	    nhrp_send_purge(nbr);
	    if (nbr->ret < NHRP_MAX_RET_DELAY)
		nbr->ret *= 2;
	    if (TIMER_RUNNING_AND_AWAKE(nbr->expire)) {
		if (nhrp_debug) {
		    nhrp_bugaddr("\nNHRP: Expired purge request for ",
				 &nbr->src, nhrp_funmap(nbr->protoidx));
		    buginf(" %u", nbr->reqid);
		}
		mgd_timer_stop(expired);
		free(nbr);
	    } else {
		mgd_timer_start_jittered(&nbr->retrans, nbr->ret * ONESEC, 20);
	    }
	    break;
	case NHRP_TIM_PERIODIC:
	    nhrp_periodic();
	    mgd_timer_start(&nhrp_periodic_t, ONESEC * NHRP_PERIODIC);
	    break;
	default:
	    mgd_timer_stop(expired);
	    break;
	}
    }
}

/*
 * nhrp_teardown 
 *
 * This process is being killed.  Clean up any allocated 
 * resources that NHRP process process set up when it started.
 */

static void nhrp_teardown (int signal, int dummy1, void *dummy2, 
			   char *dummy3)
{
    paktype *pak;
    nhrp_eventtype *ev;

    process_watch_mgd_timer(&nhrp_master_t, DISABLE);
    mgd_timer_stop(&nhrp_master_t);

    process_watch_queue(nhrp_eventQ, DISABLE, RECURRING);
    while ((ev = process_dequeue(nhrp_eventQ)) != NULL)
	free(ev);
    delete_watched_queue(&nhrp_eventQ);

    process_watch_queue(nhrp_packetQ, DISABLE, RECURRING);
    while ((pak = process_dequeue(nhrp_packetQ)) != NULL)
	retbuffer(pak);
    delete_watched_queue(&nhrp_packetQ);

    nhrp_delete_pdb();
    nhrp_pid = 0;
}

/*
 * nhrp_process
 * Process to handle incoming NHRP traffic, and to generate/retransmit
 * NHRP traffic.
 */

static process nhrp_process (void)
{
    ulong major, minor;

    /*
     * initialization
     */
    process_wait_on_system_init();
    signal_permanent(SIGEXIT, nhrp_teardown);

    /*
     * set up the event predicates.
     */
    nhrp_packetQ = create_watched_queue("NHRP packetQ", 0, 0);
    process_watch_queue(nhrp_packetQ, ENABLE, RECURRING);
    nhrp_eventQ = create_watched_queue("NHRP eventQ", 0, 0);
    process_watch_queue(nhrp_eventQ, ENABLE, RECURRING);
    process_watch_mgd_timer(&nhrp_master_t, ENABLE);

    while (TRUE) {
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	    case TIMER_EVENT:
		nhrp_timer_check();
		break;
	    case QUEUE_EVENT:
		nhrp_events();
		break;
	    default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}

/*
 * nhrp_process_start
 * Start up NHRP process, if it is not started already.
 */

void nhrp_process_start (void)
{
    if (!nhrp_pid) {
	nhrp_pid = process_create(nhrp_process, "NHRP", NORMAL_STACK, 
				  PRIO_NORMAL);
	if (nhrp_pid == NO_PROCESS) {
	    nhrp_pid = 0;
	    return;
	}
	mgd_timer_start(&nhrp_periodic_t, ONESEC * NHRP_PERIODIC);
    }
}

/*
 * nhrp_process_kill
 * Kill off NHRP process, provided that NHRP is not minimally configured
 * on at least some interface.
 */

void nhrp_process_kill (void)
{
    idbtype *swidb;
    ushort i;
    nhrpinfotype *nhrpinfo;

    FOR_ALL_SWIDBS(swidb) {
	nhrpinfo = swidb->nhrp_info;
	if (!nhrpinfo)
	    continue;
	for (i = 0; i < NHRP_NETPROTOCNT; i++) {
	    if (nhrpinfo->pro[i].nhrp_net_id != NHRP_NETID_UNDEF)
		return;
	}
    }
    /*
     * NHRP is not configured to run on any interfaces.  Shut down
     * process.
     */
    process_kill(nhrp_pid);
}

/*
 * nhrp_show_purge
 * Display our purge retransmission queue
 */

void nhrp_show_purge (nhrpfuns *fun)
{
    mgd_timer *leaf;
    nhrpcacherequestor *nbr;
    ulong expire;

    for (leaf = mgd_timer_first_child(&nhrp_master_t);
	 leaf; leaf = mgd_timer_next_running(leaf)) {
	if (mgd_timer_type(leaf) != NHRP_TIM_PURGE)
	    continue;
	nbr = mgd_timer_context(leaf);
	printf("\nReqid %u Ret %d ", nbr->reqid, nbr->ret);
	nhrp_printaddr("src ", &nbr->src, fun);
	expire = TIME_LEFT_SLEEPING(nbr->expire);
	if (expire) {
	    printf(" expire ");
	    print_ticks_in_dhms(expire);
	} else {
	    printf("expired ");
	}
    }
}

/*
 * nhrp_show_traffic_idb
 * Display traffic information for a particular interface
 */

static void nhrp_show_traffic_idb (idbtype *swidb, nhrpstattype *stats)
{
    int i;
    uchar *ptype;

    printf("\n%s", swidb->namestring);
    for (i = 0; i < NHRP_TYPES; i++) {
	ptype = nhrp_ptype_string(i + 1);
	printf("\n  %s packets sent: %u", ptype, stats->pkts[i * 2]);
	printf("\n  %s packets received: %u", ptype, stats->pkts[i * 2 + 1]);
    }
}

/*
 * nhrp_show_traffic
 * Display traffic information for NHRP.
 */

void nhrp_show_traffic (idbtype *target, nhrpfuns *fun)
{
    idbtype *swidb;
    nhrpstattype *stats;
    nhrpinfotype *nhrpinfo;

    automore_enable(NULL);
    /*
     * Should be able to use show_swidb_chain, but that routine's more
     * processing assumes that we generate output for each interface, which
     * is not true.
     */
    FOR_ALL_SWIDBS(swidb) {
	if (swidb->hwptr->status & IDB_DELETED)
	    continue;
	if (target && (target != swidb))
	    continue;
	nhrpinfo = swidb->nhrp_info;
	if (!nhrpinfo)
	    continue;
	stats = nhrpinfo->pro[fun->protoidx].nhrp_stats;
	if (stats == NULL)
	    continue;
	nhrp_show_traffic_idb(swidb, stats);
	if (automore_quit())
	    break;
    }
    automore_disable();
}

/*
 * nhrp_if_defaults
 * Initialize interface specific defaults.
 */

void nhrp_if_defaults (idbtype *swidb, nhrpproinfotype *proinfo)
{
    proinfo->nhrp_hold_pos = proinfo->nhrp_hold_neg = NHRP_DEF_HOLDTIME;
    proinfo->nhrp_interval = NHRP_IF_RATE_IVL;
    proinfo->nhrp_use = NHRP_USE_DEF;
    proinfo->nhrp_pkt_cnt = NHRP_MAX_PKT;
    proinfo->nhrp_quota = NHRP_MAX_PKT;
    if (proinfo->nhrp_stats == NULL) {
	proinfo->nhrp_stats = malloc_named(sizeof(nhrpstattype), "NHRP stats");
	if (proinfo->nhrp_stats == NULL) {
	    printf(nomemory);
	    return;
	}
    }
    proinfo->nhrp_flags |= NHRP_IF_INIT;
}

/* 
 * nhrp subsystem header
 */

#define NHRP_MAJVERSION 1
#define NHRP_MINVERSION 0
#define NHRP_EDITVERSION 1

SUBSYS_HEADER(nhrp, NHRP_MAJVERSION, NHRP_MINVERSION, NHRP_EDITVERSION,
	      nhrp_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      NULL);
