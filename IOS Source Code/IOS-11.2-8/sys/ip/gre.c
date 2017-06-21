/* $Id: gre.c,v 3.3.60.5 1996/07/06 05:53:18 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/gre.c,v $
 *------------------------------------------------------------------
 * Generic Route Encapsulation
 *
 * April 1992, Paul Traina
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: gre.c,v $
 * Revision 3.3.60.5  1996/07/06  05:53:18  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.3.60.4  1996/06/27  09:36:37  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.3.60.3  1996/06/18  01:44:34  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.3.60.2  1996/04/29  05:22:48  pst
 * CSCdi55097:  Unaligned access in memd - possible IPX/tunneling
 * interaction
 * Branch: California_branch
 *
 * Revision 3.3.60.1  1996/03/18  20:15:50  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  09:45:26  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  14:25:05  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  09:32:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:40:00  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:54:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:03:59  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/28  09:26:42  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:56:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "subsys.h"
#include "packet.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "ip.h"
#include "ip_registry.h"
#include "../clns/clns.h"
#include "../ui/debug.h"
#include "../if/network.h"
#include "tunnel.h"
#include "gre.h"

/*
 * gre_link2type
 *
 * Determine what we're going to put in our GRE PTYPE field.
 * If this is something other than a ARPA-encapsulatable protocol,
 * we need to assign an arbitrary value for PTYPE.  These values should
 * be published as extensions of the GRE RFC.
 *
 */

static ushort gre_link2type (long link)
{
    switch (link) {
    case LINK_CLNS:
	return GRE_PTYPE_OSI;
    case LINK_PUP:
	return GRE_PTYPE_PUP;
    case LINK_XNS:
	return GRE_PTYPE_XNS;
    case LINK_IP:
	return GRE_PTYPE_IP;
    case LINK_VINES:
	return GRE_PTYPE_VINES;
    case LINK_VINES_ECHO:
	return GRE_PTYPE_VINES_ECHO;
    case LINK_VINES_LOOP:
	return GRE_PTYPE_VINES_LOOP;
    case LINK_NHRP:
	return GRE_PTYPE_NHRP;
    case LINK_BRIDGE:
	return GRE_PTYPE_BRIDGE;
    case LINK_DECNET:
	return GRE_PTYPE_DECNET;
    case LINK_DECNET_ROUTER_L1:
    case LINK_DECNET_ROUTER_L2:
	return GRE_PTYPE_DECNET;
    case LINK_DECNET_NODE:
	return GRE_PTYPE_DECNET;
    case LINK_FR_SWITCH:
	return GRE_PTYPE_FR_SWITCH;
    case LINK_APPLETALK:
	return GRE_PTYPE_APPLETALK;
    case LINK_APOLLO:
	return GRE_PTYPE_APOLLO;
    case LINK_NOVELL:
	return GRE_PTYPE_NOVELL;
    case LINK_IEEE_SPANNING:
	return GRE_PTYPE_IEEE_SPANNING;
    case LINK_DEC_SPANNING:
	return GRE_PTYPE_DEC_SPANNING;
    default:
	return GRE_PTYPE_UNKNOWN;
    }
}

/*
 * gre_type2link
 *
 * Called by raw_enqueue to determine where to send the packet we just
 * decapsulated for us.  The GRE spec says that the PTYPE field should
 * be the same as the ARPA encapsulated PTYPE field if one is available
 * for that particular protocol, otherwise it may be arbitrary.
 */

static long gre_type2link (paktype *pak)
{
    iphdrtype *ip;
    long link;

    ip = (iphdrtype *)ipheadstart(pak);

    switch (pak->tunnelled_type) {
    case GRE_PTYPE_OSI:
	link = LINK_CLNS;		break;
    case GRE_PTYPE_PUP:
	link = LINK_PUP;		break;
    case GRE_PTYPE_XNS:
	link = LINK_XNS;		break;
    case GRE_PTYPE_IP:
	link = LINK_IP;			break;
    case GRE_PTYPE_VINES:
	link = LINK_VINES;		break;
    case GRE_PTYPE_VINES_ECHO:
	link = LINK_VINES_ECHO;		break;
    case GRE_PTYPE_VINES_LOOP:
	link = LINK_VINES_LOOP;		break;
    case GRE_PTYPE_NHRP:
	link = LINK_NHRP;		break;
    case GRE_PTYPE_DECNET:
	link = LINK_DECNET;		break;
    case GRE_PTYPE_BRIDGE:
	link = LINK_BRIDGE;		break;
    case GRE_PTYPE_FR_SWITCH:
	link = LINK_FR_SWITCH;		break;
    case GRE_PTYPE_APPLETALK:
	link = LINK_APPLETALK;		break;
    case GRE_PTYPE_APOLLO:
	link = LINK_APOLLO;		break;
    case GRE_PTYPE_NOVELL:
	link = LINK_NOVELL;		break;
    case GRE_PTYPE_IEEE_SPANNING:
	link = LINK_IEEE_SPANNING;	break;
    case GRE_PTYPE_DEC_SPANNING:
	link = LINK_DEC_SPANNING;	break;
    default:
	link = LINK_ILLEGAL;		break;
    }

    if (tunnel_debug) {
	switch (link) {
	  case LINK_IP:
	    buginf("\n%s: GRE decapsulated IP %i->%i (len=%d, ttl=%d)",
		   pak->if_input->namestring,
		   ip->srcadr, ip->dstadr, pak->datagramsize, ip->ttl);
	    break;
	  case LINK_CLNS:
	    buginf("\n%s: GRE decapsulated CLNS packet",
		   pak->if_input->namestring);
	    break;
	  default:
	    buginf("\n%s: GRE decapsulated (linktype=%d, len=%d)",
		   pak->if_input->namestring, link, pak->datagramsize);
	}
    }
    return(link);
}

/*
 * gre_ip_encaps
 *
 * Called by the tunnel_oqueue routine.  This spoofs the output queue
 * on the tunnel interface.  Instead of putting packet on output hold
 * queue,  build a new packet from scratch create a GRE header, and copy
 * the existing packet data into the new packet after the GRE header.
 *
 * We are not responsible for releasing the old packet -- our caller
 * will do that (caller may need old packet for statistics).
 * 
 */

static paktype *gre_ip_encaps (tunnel_info *tinfo, paktype *pak)
{
    paktype *new_pak;
    iphdrtype *ip;
    gretype *gre;
    ulong *grenext;
    ulong bytes;

    bytes = tinfo->header->datagramsize + GREHEADERBYTES(tinfo) +
	    pak->datagramsize;

    new_pak = getbuffer(bytes);
    if (!new_pak)
        goto punt;

    ip = (iphdrtype *)ipheadstart(new_pak);
        
    gre = (gretype *) ((char *)ip + tinfo->header->datagramsize);
    *(ushort *)gre = 0;			/* clear flags & version */

    if (!(gre->ptype = gre_link2type(pak->linktype)))
	goto punt;			/* couldn't classify packet */

    if (tinfo->checksum || tinfo->srclen)
	grenext = (ulong *) (gre + 1);	/* point to end of data structure */
    else
	grenext = (ulong *) &gre->checksum; /* no checksum field */

    if (tinfo->enable_key) {
	gre->have_key = TRUE;
	*grenext++ = tinfo->key;
    }

    if (tinfo->enable_seq) {
	gre->have_sequence = TRUE;
        *grenext++ = tinfo->t_sequence++;
    }

    if (tinfo->srclen) {
	gre->have_routing = TRUE;
	gre->offset = 0;
	bcopy(tinfo->source_route, grenext, tinfo->srclen);
	grenext += (tinfo->srclen / sizeof(ulong));	/* danger */
    }

    /* copy old packet into place -- tunnel_encaps set DG start & size */
    (void) bcopy(pak->datagramstart, (uchar *)grenext, pak->datagramsize);

    if (tinfo->checksum) {
	gre->have_checksum = TRUE;
	gre->checksum = 0;
	gre->checksum = ipcrc((ushort *) gre,
			      bytes - tinfo->header->datagramsize);
    }

    /* sneak some information upstairs... */
    new_pak->datagramsize = bytes;

    return(new_pak);

  punt:
    if (new_pak)
	datagram_done(new_pak);
    return(NULL);
}

/*
 * gre_ip_decaps
 *
 * Decapsulate an packet passed down from IP processing
 *
 * Determine which tunnel packet is destined for, check packet key
 * and sequence numbers.
 *
 * If GRE source routing on packet, process packet or send it on its next
 * hop (currently we only support IP in the SR entries).
 *
 * Finally, if packet is good, move the data following the GRE header
 * down to the start of the packet data area and enqueue the packet for
 * native processing.
 *
 */

static void gre_ip_decaps (paktype *pak)
{
    iphdrtype   *ip;
    hwidbtype	*idb;
    idbtype     *swidb;
    tunnel_info	*tinfo;
    gretype	*gre;
    ulong	*grenext, *keyptr;
    sretype	*sre;
    ipaddrtype	*sre_data;
    int		 delta;

    ip = (iphdrtype *)ipheadstart(pak);
    gre = (gretype *) ipdatastart(pak);

    ip_traffic.gre_inputs++;	/* GRE */
    FOR_ALL_TUNNELS(tinfo)
	if ((tinfo->mode == TUN_MODE_GRE_IP) &&
	    (ip->srcadr == tinfo->destination) &&
	    (tinfo->hwidb->state == IDBS_UP))
	    break;

    if (!tinfo) {
	/*
	 * Didn't match P2P tunnel.  Maybe this packet is for a multipoint
	 * tunnel, which requires matching on the GRE key.
	 */
	if (!gre->have_key)
	    goto punt;
	if (gre->have_checksum || gre->have_routing)
	    keyptr = (ulong *)(gre + 1);
	else
	    keyptr = (ulong *)&gre->checksum;
	FOR_ALL_TUNNELS(tinfo) {
	    if (tinfo->mode != TUN_MODE_MGRE_IP)
		continue;
	    if (tinfo->key != *keyptr)
		continue;
	    if (ip->dstadr != TUNNEL_MYADDR(tinfo))
		continue;
	    if (tinfo->hwidb->state == IDBS_UP)
		break;
	}
	if (!tinfo)
	    goto punt;
    }

    idb = tinfo->hwidb;
    swidb = idb->firstsw;

    if (tunnel_debug)
	buginf("\n%s: GRE/IP to decaps %i->%i (len=%d ttl=%d)",
	       swidb->namestring, ip->srcadr, ip->dstadr,
	       ip->tl, ip->ttl);

    if (*((ushort *) gre)) {	/* if flags or non-zero version number,
				   we have to process things by hand */

	if (gre->have_checksum || gre->have_routing)
	    grenext = (ulong *) (gre + 1);
	else
	    grenext = (ulong *) &gre->checksum;

	if (gre->have_checksum &&
	    ipcrc((ushort *) gre, ip->tl - (ip->ihl<<2)))
	    goto punt;

	if (tinfo->enable_key && !gre->have_key)
	    goto punt;

	if (gre->have_key && *grenext++ != tinfo->key)
	    goto punt;

	if (gre->have_sequence) {
	    delta = *grenext++ - tinfo->r_sequence;
	    if (delta < 0 && delta > -GRE_SEQUENCE_WINDOW)
		goto punt;	/* out of order packet, punt it */
	    tinfo->r_sequence += delta;     /* update to accept next packet */
	    if (delta > GRE_SEQUENCE_WINDOW || delta < 0)
		goto punt;
	}

	if (gre->have_routing) {
	    sre = (sretype *) ((uchar *) grenext + gre->offset);
	    
	    if (sre->length == 0) /* if last entry in chain, it's for us */
		grenext = (ulong *) (sre + 1);

	    /*   if an IP address, fixup offset to point to next in chain
		 and send it along its merry way */
	    
	    else {
		switch (sre->address_family) {
		case GRE_PTYPE_IP:
		    sre_data = (ipaddrtype *) (sre+1) +
			(sre->offset / sizeof(ipaddrtype));
		    sre->offset += sizeof(ipaddrtype);
		    if (sre->offset == sre->length)
			gre->offset += sizeof(sre) + sre->length;
		    ipwrite(pak, 0, GRE_PROT,
			    swidb->ip_unnumbered ?
			    swidb->ip_unnumbered->ip_address :
			    swidb->ip_address,
			    *sre_data, NULL, NOPT, &ipdefaultparams);
	            return;
		default:
		    goto punt;
		}
	    }
	}
    } else {	/* "fast path" .. skip all the other checks */
	if (tinfo->enable_key)
	    goto punt;

	grenext = (ulong *) &gre->checksum;
    }

    /*
     * Realign the packet pointers for the de-encapsulated frame
     */
    pak->datagramsize   = ip->tl - ((uchar *)grenext - (uchar *)ip);
    pak->network_start  = (uchar *)grenext;
    pak->datagramstart  = pak->network_start;
    pak->enctype        = ET_TUNNEL;
    pak->encsize        = 0;
    pak->tunnelled_type = gre->ptype;

    reg_invoke_tunnel_raw_enqueue(idb, pak);
    return;

  punt:
    datagram_done(pak);
}

/*
 * gre_multipoint_cstate
 * Update interface link type to reflect that it is actually multipoint.
 */

static void gre_multipoint_cstate (idbtype *idb, int enable)
{
    if (enable)
	idb->subif_link_type = SUBIF_TYPE_MULTI;
    /*
     * Routing in & out of a multipoint tunnel interface is a normal
     * occurrence, so disable IP redirects.
     */
    idb->ip_redirect = !enable;
}

/*
 * gre_ip_setmode
 *
 * Provide tunnel initialization with information for the
 * tinfo structure 
 */

static void gre_ip_setmode (tunnel_info *tinfo,
			    linktype_t *getlink, int *enctype)
{
    tinfo->encaps = gre_ip_encaps;
    tinfo->resync = (void (*)(tunnel_info *))return_nothing;

    tinfo->protocol = GRE_PROT;

    if (tinfo->mode == TUN_MODE_MGRE_IP) {
	tinfo->cstate     = gre_multipoint_cstate;
	tinfo->transport  = TUN_TRANS_IP_MULTI;
	tinfo->modestring = "multi-GRE/IP";
    } else {
	tinfo->cstate     = NULL;
	tinfo->transport  = TUN_TRANS_IP;
	tinfo->modestring = "GRE/IP";
    }

    *getlink = gre_type2link;
    *enctype = ET_TUNNEL;
}

/*
 * gre_init
 *
 * Perform gre registration
 *
 */

static void gre_init (subsystype *subsys)
{
    reg_add_tunnel_setmode(TUN_MODE_GRE_IP,  gre_ip_setmode, "gre_ip_setmode");
    reg_add_tunnel_setmode(TUN_MODE_MGRE_IP, gre_ip_setmode, "gre_ip_setmode");
    reg_add_ip_enqueue(GRE_PROT, gre_ip_decaps, "gre_ip_decaps");
}

/*
 * GRE subsystem header
 */

#define GRE_MAJVERSION 1
#define GRE_MINVERSION 0
#define GRE_EDITVERSION  1

SUBSYS_HEADER(gre, GRE_MAJVERSION, GRE_MINVERSION, GRE_EDITVERSION,
	      gre_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      "req: iphost, tunnel");
