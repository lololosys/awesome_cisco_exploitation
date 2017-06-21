/* $Id: at_input.c,v 3.5.26.9 1996/08/28 12:36:35 thille Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_input.c,v $
 *------------------------------------------------------------------
 * atalk_input.c -- handle Appletalk input
 *
 * August 1988, Bill Palmer
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_input.c,v $
 * Revision 3.5.26.9  1996/08/28  12:36:35  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.26.8  1996/08/13  00:26:36  dwong
 * CSCdi65598:  NBP filters outbound LkUp when received as FwdReq
 * Branch: California_branch
 *
 * Revision 3.5.26.7  1996/08/03  23:19:05  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.5.26.6  1996/07/10  22:18:20  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.5.26.5  1996/07/02  23:10:10  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.5.26.4  1996/05/19  01:46:37  dwong
 * CSCdi57975:  Bus Error, Router Crashes when switching apple over Serial
 * Branch: California_branch
 * Another idb nightmare
 *
 * Revision 3.5.26.3  1996/04/23  18:26:07  hampton
 * CSCdi55427:  Crash in etalk_enqueue
 * Branch: California_branch
 *
 * Revision 3.5.26.2  1996/03/23  01:26:47  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.5.26.1  1996/03/18  18:52:32  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.2  1996/03/13  01:06:48  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.2.1  1996/03/07  08:28:55  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5  1996/01/18  19:39:22  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1995/12/24  04:56:20  dwong
 * CSCdi44162:  Appletalk doesnt work between ISDN callers
 *
 * Revision 3.3  1995/11/17  08:42:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:05:34  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:57:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  20:50:04  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/09/29  22:32:31  dwong
 * CSCdi40454:  Wants NBP filters to take effect on outbound traffic
 * Make nbp packets (un)interesting packets when nbp filters are used
 * with dialer lists.
 *
 * Revision 2.5  1995/09/12  19:34:59  slin
 * CSCdi39096:  Inability to NBP-register AUFS Server (Unix Host).
 * - Add support for routing between local devices
 *
 * Revision 2.4  1995/07/05  22:24:34  kleung
 * CSCdi35438:  AURP update event did not send through tunnel properly
 *
 * Revision 2.3  1995/06/18  06:17:38  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.2  1995/06/14 22:50:56  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Change SMRP socket from 65 to 10.
 *
 * Revision 2.1  1995/06/07  20:08:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "../if/arp.h"
#include "../if/ether.h"
#include "sys_registry.h"
#include "../os/signal.h"
#include "atalk_private.h"
#include "at_registry.h"
#include "at_arp.h"
#include "at_domain.h"
#include "media_registry.h"


static watched_queue *atalkQ;


/*
 * etalk_validpacket
 *
 * Initialize AppleTalk-private packet data fields, determine link type, and
 * do related validation.
 *
 * WARNING!  This routine is called at interrupt level and must observe 
 *           coding restrictions.
 */
static boolean
etalk_validpacket (paktype* pak)
{
    atalkidbtype 	*idb = atalk_getidb(pak->if_input);
    uchar* hdr = NULL;

    if (pak->enctype != idb->atalk_enctype) {
	if (at_debug(atalkpkt_debug, ATALK_INTERFACE_MASK, idb->phys_idb)
	    && at_debug(atalkerr_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    atalk_pak_printbad(pak,"wrong encapsulation",idb);
	atalk_stat[ATALK_ROUTER_BADVERS]++;
	return (FALSE);
    }
    hdr = pak->network_start - LLAP_ENCAPBYTES;
    if ((hdr[LLAP_SRCNODE] == ATALK_BROAD_ADDR) ||
	(hdr[LLAP_SRCNODE] == ATALK_ANY_ROUTER)) {
	atalk_stat[ATALK_MARTIAN]++;
	idb->phys_idb->hwptr->atalk_inputerrs++;
	atalk_pak_printbad(pak,"bad llap source",idb);
	return (FALSE);
    }
    if ((hdr[LLAP_DSTNODE] != idb->atalknode) &&
	(hdr[LLAP_DSTNODE] != ATALK_ANY_ROUTER) &&
	(hdr[LLAP_DSTNODE] != ATALK_BROAD_ADDR)) {
	if (at_debug(atalkpkt_debug, ATALK_INTERFACE_MASK, idb->phys_idb)
	    || at_debug(atalkerr_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    atalk_pak_printbad(pak,"llap dest not for us",idb);
	atalk_stat[ATALK_IGNORED]++;
	return (FALSE);
    }
    if ((hdr[LLAP_PAKTYPE] != ALAP_DDP_SHORT) &&
	(hdr[LLAP_PAKTYPE] != ALAP_DDP_LONG)) {
	atalk_stat[ATALK_MARTIAN]++;
	idb->phys_idb->hwptr->atalk_inputerrs++;
	atalk_pak_printbad(pak,"bad llap type",idb);
	return (FALSE);
    }

    if (hdr[LLAP_PAKTYPE] == ALAP_DDP_SHORT)
	pak->linktype = LINK_ATALK_SHORT;
    else 
	pak->linktype = LINK_APPLETALK;
    return (TRUE);
}

/*
 * atalk_validpacket
 *
 * Initialize AppleTalk-private packet data fields, and do encapsulation
 * validation.
 *
 * WARNING!  This routine is called at interrupt level and must observe 
 *           coding restrictions.
 */
static boolean
atalk_validpacket (paktype* pak)
{
    atalkidbtype* idb = atalk_getidb(pak->if_input);

    if (pak->enctype != idb->atalk_enctype) {
	if (at_debug(atalkpkt_debug, ATALK_INTERFACE_MASK, idb->phys_idb)
	    && at_debug(atalkerr_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
	    atalk_pak_printbad(pak,"wrong encapsulation",idb);
	atalk_stat[ATALK_ROUTER_BADVERS]++;
	return (FALSE);
    }
    if (pak->enctype == ET_SNAP) {
	snap_hdr *snap = (snap_hdr *)pak->info_start;

	if (!ieee_equal_oui(snap->oui, apple_snap)) {
	    atalk_stat[ATALK_BAD_ISO2OUI]++;
	    idb->phys_idb->hwptr->atalk_inputerrs++;
	    atalk_pak_printbad(pak,"incorrect SNAP OUI",idb);
	    return (FALSE);
	}
    }
    pak->linktype = LINK_APPLETALK;
    return (TRUE);
}

/* 
 * etalk_enqueue
 *
 * Prepare a packet for AppleTalk processing after checking global 
 * state and do minimal validation.
 *
 * WARNING!  This routine is called at interrupt level and must observe 
 *           coding restrictions.
 */
void 
etalk_enqueue (paktype* pak)
{
    atalkidbtype *idb;

    boolean valid = FALSE;

    idb = atalk_getidb(pak->if_input);
    
    if (idb && atalkif_InterfaceUp(idb)) {
	pak->transport_start = NULL;
	atalk_pak_inithdrptr(pak);
	if (idb->atalk_enctype == ET_ETHERTALK)
	    valid = etalk_validpacket(pak);
	else 
	    valid = atalk_validpacket(pak);
	if (valid) {
	    process_enqueue_pak(atalkQ, pak);
	    return;
	}
    }
    protocol_discard(pak, atalk_running);
}

#ifdef DEAD
/*
 * llap_allocencaps
 * adjust the pak information to include LLAP in encapsulation
 */
long
llap_allocencaps (paktype* pak)
{
    if ((pak == NULL) || (pak->datagramstart == NULL))
    return (FALSE);
    pak->datagramstart -= LLAP_ENCAPBYTES;
    pak->datagramsize += LLAP_ENCAPBYTES;
    return (TRUE);
}

/*
 * llap_overencaps
 * overwrite an existing llap encapsulation
 */
boolean
llap_overencaps (paktype* pak, int address, uchar* daddr)
{
    uchar* addr = daddr - LLAP_ENCAPBYTES;
    pak->datagramsize += LLAP_ENCAPBYTES;
    switch (pak->linktype) {
    default:
    case LINK_AARP:
	return (FALSE);
    case LINK_ATALK_SHORT:
	addr[LLAP_PAKTYPE] = ALAP_DDP_SHORT;
	break;
    case LINK_APPLETALK:
	addr[LLAP_PAKTYPE] = ALAP_DDP_LONG;
	break;
    }

    addr[LLAP_DSTNODE] = atalk_nodeonly(address);
    addr[LLAP_SRCNODE] = atalkif_MyNode(pak->if_output);
    return (TRUE);
}
#endif

/*
 * llap_encapsulate
 * add localtalk encaps for pre-8.2 serials
 */
long 
llap_encapsulate (paktype* pak, int length, ulong address)
{
    if (pak->if_output == NULL)
	return (FALSE);
    pak->datagramsize = length;
    pak->datagramstart = pak->network_start;
    switch (pak->linktype) {
    default:
    case LINK_AARP:
	return (FALSE);
    case LINK_ATALK_SHORT:
	pak->datagramsize += LLAP_ENCAPBYTES;
	pak->datagramstart -= LLAP_ENCAPBYTES;
	pak->datagramstart[LLAP_PAKTYPE] = ALAP_DDP_SHORT;
	break;
    case LINK_APPLETALK:
	pak->datagramsize += LLAP_ENCAPBYTES;
	pak->datagramstart -= LLAP_ENCAPBYTES;
	pak->datagramstart[LLAP_PAKTYPE] = ALAP_DDP_LONG;
	break;
    }

    pak->datagramstart[LLAP_DSTNODE] = atalk_nodeonly(address);
    pak->datagramstart[LLAP_SRCNODE] = atalkif_MyNode(atalk_getidb(pak->if_output));
    return (TRUE);
}

/*
 * llap_vencapsulate
 * add localtalk encaps for pre-8.2 serials, assumes datagramstart is setup
 */
long
llap_vencapsulate (paktype* pak, ulong address)
{
    if ((pak->if_output == NULL)||(pak->datagramstart == NULL))
	return (FALSE);
    switch (pak->linktype) {
    default:
    case LINK_AARP:
	return (FALSE);
    case LINK_ATALK_SHORT:
	pak->datagramsize += LLAP_ENCAPBYTES;
	pak->datagramstart -= LLAP_ENCAPBYTES;
	pak->datagramstart[LLAP_PAKTYPE] = ALAP_DDP_SHORT;
	break;
    case LINK_APPLETALK:
	pak->datagramsize += LLAP_ENCAPBYTES;
	pak->datagramstart -= LLAP_ENCAPBYTES;
	pak->datagramstart[LLAP_PAKTYPE] = ALAP_DDP_LONG;
	break;
    }

    pak->datagramstart[LLAP_DSTNODE] = atalk_nodeonly(address);
    pak->datagramstart[LLAP_SRCNODE] = atalkif_MyNode(atalk_getidb(pak->if_output));
    return (pak->datagramstart[LLAP_SRCNODE] != 0);
}

/*
 * atalk_pak_initdataptr
 *
 * This routine handles the two forms of ddp headers, namely short and
 * extended.  In order to allow the bulk of the code to function
 * regardless of headers used, we peel this layer of encapsulation off
 * and provide bottleneck routines to access data in the header.
 * Although it is slower for some accesses, most of the higher level
 * AT protocols can perform their tasks much more quickly and with
 * fewer opportunities for mistakes.  
 *
 * This routine sets the atalk_datalen and transport_start fields in the
 * packet overhead area.  These fields provide access information
 * about the ddp data portion of the packet.
 *

 * NOTE: The datagramstart field should point to the first ddp header
 * byte.  The packet linktype should be one of LINK_APPLETALK or
 * LINK_ATALK_SHORT.

 * FURTHER: Short style packets must be sourced or sinked by this node.
 * they are never transit packets.
 * 
 * WARNING: Not all output interfaces can support short packets so when
 * in doubt, extended should be used.  However, it is considered good
 * network citizenship to use short packets when possible (primarily
 * with localtalk and serial interfaces when generating packets for
 * routing.)
 *
 * Routine returns either zero if successful or a reason the packet
 * should be discarded.  By adding tests here, we attempt to minimize
 * the number of times we recalc the packet info.
 */
int
atalk_pak_initdataptr (paktype* pak)
{
    /* Obtain the length as specified in the header */
    ushort size = atalk_pak_rawlength(pak);
    /* hsize will be the header size for this linktype */
    ushort hsize;			
    
    atalkidbtype* idb = atalk_getidb(pak->if_input);
    ddptype* ddp = (ddptype*)pak->network_start;
    uchar*    hdr;			/* for LLAP bytes on DDP shorts */


    /*
     * After adjusting (if this is a transit packet) or initializing (if a
     * source packet) the generic datagram pointers, that information is
     * used to determine the ddp header type, determine data pointers and
     * to cache some frequently used information.
     */
    if (pak->linktype == LINK_APPLETALK) {
	/* For extended packets, all data is obtained from the packet */
	hsize = DDP_OVERHEADBYTES;
	pak->atalk_dstfqa = atutil_SetPakAddress(GETSHORT(ddp->l.dnet),
						ddp->l.dnode,
						ddp->l.dsock);
	pak->atalk_srcfqa = atutil_SetPakAddress(GETSHORT(ddp->l.snet),
						ddp->l.snode,
						ddp->l.ssock);
	atalk_stat[ATALK_DDP_LONG]++;
    } else {
	hsize = DDPS_OVERHEADBYTES;
	hdr = (uchar *)ddp - LLAP_ENCAPBYTES;
	/*
	 * For short headers, the net # information is obtained from the idb 
	 * and the node numbers are obtained by sniffing back into this
	 * LLAP bytes.
	 *
	 * I love DDP short packets. They should be banned like asbestos.
	 */
	pak->atalk_dstfqa = 
	    atutil_SetPakAddress(atalkif_MyNet(idb),
				hdr[LLAP_DSTNODE],
				ddp->s.dsock);
	pak->atalk_srcfqa = 
	    atutil_SetPakAddress(atalkif_MyNet(idb),
				hdr[LLAP_SRCNODE],
				ddp->s.ssock);
	atalk_stat[ATALK_DDP_SHORT]++;
    }
    /* Finally, finish adjusting the atalk data info */
    pak->transport_start = ((uchar*) ddp) + hsize;
    pak->atalk_datalen = size - hsize;
    if ((pak->datagramsize < size) || (pak->datagramsize < hsize))
	atalk_stat[ATALK_DDP_2SHORT_ERR]++;
    if (   (pak->datagramsize > size)
	|| (pak->datagramsize > (DDP_MAX_DATABYTES + hsize)))
	atalk_stat[ATALK_DDP_2LONG_ERR]++;
    if ((size < hsize) || (size > pak->datagramsize) || 
	(pak->atalk_datalen > DDP_MAX_DATABYTES)) {
	if (at_debug(atalkerr_debug, ATALK_NULL_MASK)) {
	    buginf("\natalk_pak_initdataptr: int %s size = %d hsize = %d"
		   " datagramsize = %d encsize = %d\n",
		   idb_get_namestring(idb->phys_idb),
		   size, hsize, pak->datagramsize, pak->encsize);
	}
	return (ATALK_DDP_WRONGSIZE);
    }
    return (0);
}

/*
 * atalk_glean
 *
 * Glean the source addresses (DDP and MAC) for this packet from
 * the frame and stuff 'em into the AARP cache.
 *
 * At this point, we assume that datagramstart is pointing to the
 * beginning of the DDP frame, after the encapsulation (both media
 * encaps and any possible LLAP bytes).
 */
static inline void
atalk_glean (paktype *pak)
{
    uchar       *llap_start;
    hwaddrtype  src_snpa;
    atalkidbtype *idb = atalk_getidb(pak->if_input);
    hwidbtype   *hwidb = hwidb_or_null(pak->if_input);
    encapstype	aarp_enctype;
    
    /* Abort if input interface is unknown or if address extraction
     * routine is unavailable or address pointers are unavailable or bogus.
     */
    if (!idb || !hwidb || !hwidb->extract_snpa || !pak->addr_start ||
	!validmem(pak->addr_start))
	return;

    /* Abort if AARP not supported on input interface. */
    if ((aarp_enctype = idb->aarp_enctype) == ET_NULL)
	return;

    /*
     * Extract the source address from the packet.  Abort if it fails.
     */
    if (!(*hwidb->extract_snpa)(pak, &src_snpa, SOURCE))
	return;

    if (idb->atalk_enctype == pak->enctype) {

	if (idb->atalk_enctype == ET_ETHERTALK) { /* Nonextended AT pkt? */
	    if (hwidb->enctype == ET_ARPA) {
		llap_start = pak->network_start - ATALK_LAP_EXTRABYTES;
		aarp_insert(idb,
			    atalk_address(idb->atalknet,
					  llap_start[LLAP_SRCNODE]),
			    src_snpa.addr, src_snpa.length,
			    aarp_enctype, ARP_DYNAMIC);
	    }
	} else if (idb->atalk_enctype == ET_SNAP) { /* Extended AT pkt? */

	    /* Unlike non-extended AppleTalk, extended AppleTalk */
	    /* has to use the destination net from the incoming */
	    /* packet since wide cable ranges are possible. */
	    if (atalkif_dstnettype(idb, atalk_pak_snet(pak)) == netLocalNet) {
		aarp_insert(idb,
			    atalk_pak_srcaddr(pak),
			    src_snpa.addr, src_snpa.length,
			    aarp_enctype, ARP_DYNAMIC);
	    }
	}
    }
}

/*
 * atalk_enqueue -
 * 	enqueue a packet to be handled by one of our internal processes
 *	NOTE:  due to MacTCP bug, have to special-case ddpIP pkts.
 */
void
atalk_enqueue (paktype* pak)
{
    watched_queue *qq;
    atalkidbtype *idb;
    int skt;
    boolean dodebug;
    
    idb = atalk_getidb(pak->if_input);

    dodebug = at_debug(atalkpkt_debug,
		       ATALK_INTERFACE_MASK | ATALK_LOGLEVEL_MASK,
		       idb->phys_idb, 3);
    
    if (dodebug)
	buginf("\nAT: atalk_enqueue to (%dn,%da,%dk) cksum=%d (%d)",
		atalk_pak_dnet(pak), atalk_pak_dnode(pak),
		atalk_pak_dsock(pak), atalk_pak_cksum(pak),
		atalk_compute_crc(pak));

    atalk_stat[ATALK_LOCALDEST]++;

    skt = atalk_pak_ssock(pak);
    if (skt < ATALK_MIN_SOCKET || skt > ATALK_MAX_SOCKET) {
	atalk_pak_printbad(pak, "source socket invalid", idb);
	atalk_dispose(ATALK_MARTIAN, pak);
	return;
    }
	
    /* good ole MacTCP directs ddpIP to random skts: */
    if (atalk_pak_getpt(pak) == DDP_TYPE_MACIP)
	skt = ATALK_SOCK_MACIP;
    else
	skt = atalk_pak_dsock(pak);

    atalk_SocketMarkInput(skt);
    if (!atalk_SocketPreQueue(skt, pak)) {
	qq = atalk_SocketQueue(skt);
	if (qq == NULL) {
	    atalk_pak_printbad(pak,"socket inactive",idb);
	    atalk_dispose(ATALK_NO_HANDLER, pak);
	    return;
	}
	
	/* add it to the queue... */
	process_enqueue(qq, pak);
	
	if (dodebug)
	    buginf("\nAT: atalk_enqueue, packet queued on %lx\n", qq);
    }
}

/*
 * atalk_check -
 *	It is here that check-summing is done.
 *
 *	If <myddp> is not set to DDP_TYPE_Invalid, then we also ensure
 *	that the pkt is the right type for the socket handler.  Otherwise,
 *	socket handler is responsible for it's own check.
 *
 *	If the pkt fails one of the checks, and <errstat> is non-negative,
 *	then atalk_stats[<errstat>] is incremented.
 */

boolean
atalk_check (paktype *pak, ddpTypes myddp, int errstat, boolean ignore_if)
{
    atalkidbtype *idb = atalk_getidb(pak->if_input);
    
    if (!ignore_if && !atalkif_InterfaceUp(idb)) {
	atalk_pak_printbad(pak, "port down", idb);
	atalk_dispose(-1, pak);
	return (FALSE);
    }

    /* if ddptype checking requested... */
    if (myddp != DDP_TYPE_Invalid) {
	if (atalk_pak_getpt(pak) != myddp) {
	    atalk_dispose(errstat, pak);
	    return (FALSE);		/* wrong DDP type for skt */
	}
    }

    /* if checksumming enabled... */
    if (atalk_checksumming) {
	if (atalk_valid_DDPcksum(pak)) {
	    atalk_pak_printgood(pak, "atalk_dequeue", idb);
	    return (TRUE);	/* checksum okay */
	}

	/* checksum error: */
	pak->if_input->hwptr->atalk_inputerrs++;
	atalk_pak_printbad(pak,"bad checksum", idb);
	atalk_dispose(errstat, pak); 
	atalk_stat[ATALK_CKSUMERR]++;
	return (FALSE);
    } else {
	atalk_pak_printgood(pak, "atalk_dequeue", idb);
	return (TRUE);		/* checksumming disabled */
    }
}

/*
 * atalk_forus
 * determine whether packet is meant for this host
 */
static inline forusTypes
atalk_forus (paktype* pak)
{
    ataddrtype daddr, saddr;
    addrTypes satype, datype;
    atalkidbtype *ifidb = atalk_getidb(pak->if_input);

    daddr = atutil_DestAddr(pak);
    saddr = atutil_SrcAddr(pak);
    satype = atalkif_SrcAddrType(ifidb,saddr);
    datype = atalkif_DstAddrType(ifidb,daddr);

#ifdef ATYPE_DEBUG
    (void)atalk_PrintAddrType(ifidb,daddr,satype);
    (void)atalk_PrintAddrType(ifidb,daddr,datype);
#endif

    switch (datype) {
    case dstNotForCable:
	if (!atalkif_operational(ifidb)) {
	    atalk_stat[ATALK_NOT_OPERATIONAL]++;
	    return ATALK_PITCHIT;
	}
	if ((satype == srcIsForeign) || (satype == srcOnThisCable))
	    return ATALK_ROUTEIT;
	atalk_stat[ATALK_SADDR_INVALID]++;
	return ATALK_PITCHIT;
    case dstCableBroadcast:
	if (atalkif_BroadcastForMe(ifidb,daddr))
	    return ATALK_CONSUMEIT;
	atalk_stat[ATALK_BCAST_NOTMINE]++;
	return ATALK_PITCHIT;
    case dstIsMyAddress:
	return ATALK_CONSUMEIT;
    case addrIsInvalid:
	if (atalkif_CaymanTunnel(ifidb)) {
	    if ((daddr == 0L) && (saddr == 0L))
		return ATALK_CONSUMEIT;
	}
	atalk_stat[ATALK_DADDR_INVALID]++;
	return ATALK_PITCHIT;
    case dstForCableRouter:
	return ATALK_CONSUMEIT;
    case dstForCableMarshal:
				/* This is not implemented in this release */
	atalk_stat[ATALK_DADDR_INVALID]++;
	return ATALK_PITCHIT;
    case dstForThisCable:
	if (atalk_localrouting || !ifidb->atalk_rtmp_split_horizon)
	    return (ATALK_ROUTEIT);
    case cableUnavailable:
    case addrIndeterminate:
    default:
	atalk_stat[ATALK_DADDR_IGNORED]++;
	return ATALK_PITCHIT;
    }
}

static void
atalk_input_teardown (int signal, int dummy1, void *dummy2, char *dummy3)
{
    paktype *pak;

    reg_delete_raw_enqueue(LINK_APPLETALK);
    reg_delete_raw_enqueue(LINK_ATALK_SHORT);
    process_watch_queue(atalkQ, DISABLE, RECURRING);
    while ((pak = process_dequeue(atalkQ)) != NULL)
	retbuffer(pak);
    delete_watched_queue(&atalkQ);
    atalk_pid = 0;
}

/* 
 * atalk_input 
 * all incoming appletalk packets initially go through here
 */
process 
atalk_input (void)
{
    paktype *pak;
    hwidbtype	       *hwidb;
    atalkidbtype	       *idb;
    int err;
    int which, pak_count;
    boolean intercepted;

    process_wait_on_system_init();

    /*
     * Set up this process' world.
     */
    signal_permanent(SIGEXIT, atalk_input_teardown);
    atalkQ = create_watched_queue("AT packets", 0, 0);
    process_watch_queue(atalkQ, ENABLE, RECURRING);
    reg_add_raw_enqueue(LINK_APPLETALK, etalk_enqueue, "etalk_enqueue");
    reg_add_raw_enqueue(LINK_ATALK_SHORT, etalk_enqueue, "etalk_enqueue");
    
    while (TRUE) {
	process_wait_for_event();

	pak_count = PAK_SWITCHCOUNT;

	while (TRUE) {
	    /*
	     * Process only a limited number of packets per pass.
	     */
	    if (pak_count-- == 0)
		break;
	    
	    pak = process_dequeue(atalkQ);
	    if (!pak)
		break;

	    atalk_stat[ATALK_INPUT]++; /* count a packet seen */
	    idb   = atalk_getidb(pak->if_input);
	    hwidb = hwidb_or_null(idb->phys_idb);
	    if (hwidb == NULL) {
		atalk_pak_printbad(pak, "no HW idb", idb);
		datagram_done((paktype *)pak);
		continue;
	    }
	    idb_increment_rx_proto_counts(pak, ACCT_PROTO_APPLE);
	    
	    err = atalk_pak_initdataptr(pak);
	    if (err != 0) {
		atalk_pak_printbad(pak, "failed consistency checks",
				   idb);
		hwidb->atalk_inputerrs++;
		atalk_dispose(err, pak);
		continue;
	    }

	    /*
	     * Glean the MAC address of an incoming frame if gleaning
	     * is enabled. Glean only those frames which have not
	     * yet been through a router and non-NBP frames.
	     */
	    
	    if ((atalk_pak_hops(pak) == 0)
		&& (atalk_pak_getpt(pak) != DDP_TYPE_NBP)) {
		reg_invoke_media_glean_source_address(pak->enctype, pak);
		if (idb->atalk_glean) {
		    atalk_glean(pak);
		}
	    }
	    if (at_debug(atalkpkt_debug, 0))
		atalk_pak_printgood(pak, "Packet rec'd", idb);
	    intercepted = FALSE;
	    reg_invoke_atalk_intercept(pak, &intercepted) ;
	    if (intercepted) {
		continue;
	    }

	    /*
	     * Now, if it came from the AT domain 
	     * then do all the domain processing stuff
	     */
	    if (atalk_DomainRouter_running) {
		if (atdomain_NBPLoop(pak)) {
		    datagram_done(pak);
		    continue;
		}
		atdomain_DDPinProcess(pak);
	    }

	    /* 
	     * is this packet for us?
	     */
	    which = atalk_forus(pak);
	    if (atalkpkt_debug->flag != 0
		&& at_debug(atalkpkt_debug, ATALK_INTERFACE_MASK, idb->phys_idb))
		atalk_pak_printforus(pak,which);
	    switch (which) {
	    case ATALK_PITCHIT:
		datagram_done((paktype *)pak);
		break;
	    case ATALK_ROUTEIT:
		if ((pak = atalk_nbpcheck(pak, 0)) == NULL)
		    break;
		atalk_forward(pak);
		break;
	    case ATALK_CONSUMEIT:
		if ((pak = atalk_nbpcheck(pak, 0)) == NULL)
		    break;
		atalk_enqueue(pak);
		break;
#ifdef NOTDEF
	    case ATALK_COPYAND_ROUTEIT:
		if ((pak = atalk_nbpcheck(pak), 0)) == NULL)
		    break;
		ours = atalk_pak_duplicate(pak);
		if (ours)
		    atalk_enqueue(ours);
		else
		    atalk_stat[ATALK_NO_BUFFERS]++;
		atalk_forward(pak);
		break;
#endif
	    default:
		datagram_done((paktype*)pak);
		break;
	    }
	}
    }
}

/*
 * atalk_dispose
 * dispose of an AppleTalk packet and tally error type
 *
 * if <code> is negative, then don't bump error count.
 * (maybe we'll think up another stat when we're feeling more creative)
 *
 * note - negative arg. feature relied on by atalk_check().
 */

void
atalk_dispose (int code, paktype *pak)
{
    /* return buffer before possible error msg: */
    datagram_done(pak);

    if (code < 0) {
	;		/* no stat update requested */
    } else if (code < ATALK_MAXTRAFFIC)
	atalk_stat[code]++;
    else /* code is out of range */
	errmsg(&msgsym(BADDISP, SYS), code, "atalk_dispose");
}

/*
 * atalk_loopback
 *
 *	loopback driver for appletalk.
 *	if it's for us, then requeue on handler's queue, return TRUE.
 */
static boolean
atalk_loopback (paktype *pak)
{
	idbtype *idb = pak->if_output;

	/* it's for us, put it on the background queue: */
	pak->if_output = NULL;
	if (!change_if_input(pak, idb)) {
	    atalk_dispose(ATALK_NO_HANDLER, pak);
	    return (FALSE);
	}
	pak->enctype = ET_NULL;
	pak->flags |= PAK_SENDSELF;
	atalk_stat[ATALK_LOOPBACK]++;
	atalk_stat[ATALK_INPUT]++;
	atalk_stat[((pak->linktype==LINK_APPLETALK) ? ATALK_DDP_LONG
		    : ATALK_DDP_SHORT)]++;

	atalk_pak_initdataptr(pak);
	if (at_debug(atalkpkt_debug,
		     ATALK_INTERFACE_MASK | ATALK_LOGLEVEL_MASK,
		     idb, 3))
	    buginf("\nAT: atalk_loopback to (%a, %dk) cksum=%d (%d)",
		   atalk_pak_dstaddr(pak), atalk_pak_dsock(pak),
		   atalk_pak_cksum(pak), atalk_compute_crc(pak));
	
	process_enqueue_pak(atalkQ, pak);
	return(TRUE);
}

/*
 * atalk_encaps
 * Encapsulate a packet for appletalk
 */
static inline boolean 
atalk_encaps (paktype *pak, int length, long address)
{
    atalkidbtype	*idb_o;
    hwidbtype	*hwidb_o;
    
    if (pak->if_output == NULL)
	return (FALSE);
    idb_o = atalk_getidb(pak->if_output);
    pak->enctype = idb_o->atalk_enctype;
    pak->acct_proto = ACCT_PROTO_APPLE;
    hwidb_o = pak->if_output->hwptr;
    if (hwidb_o->vencap != NULL) {
	if (idb_o->atalk_enctype == ET_ETHERTALK) {
	    if (pak->if_input != NULL) {
		pak->datagramsize = length;
		if (!llap_vencapsulate(pak, address))
		    return (FALSE);
	    } else {
		if (!llap_encapsulate(pak, length, address))
		    return (FALSE);
	    }
	    pak->enctype = hwidb_o->enctype;
	}

	return ((*hwidb_o->vencap)(pak, address));
    } else {
	return (FALSE);
    }
    return (TRUE);
}

/*
 * atalk_send1
 * lower level appletalk packet sending machinery
 */
boolean 
atalk_send1 (paktype *pak, long dhost)
{
    atalkidbtype  *idb;
    hwidbtype	  *hwidb;
    ataddrtype	   daddr;
    paktype	  *new_pak;
    int		   oldsize;
    uchar	  *oldstart;
    int		   len;
    boolean	   dumping_output;

    idb = atalk_getidb(pak->if_output);	/* get output interface for packet */
    hwidb = hwidb_or_null(pak->if_output);
    if (idb == NULL || hwidb == NULL) {	/* didn't figure out where to go! */
	errmsg(&msgsym(NULLIDB, SYS), "atalk_send1");
	datagram_done(pak); 
	return(FALSE);
    }

    daddr = atalk_pak_dstaddr(pak);

    if ((hwidb->status & IDB_HEARSELF) == 0)
	pak->flags &= ~PAK_HEARSELF;


    if (at_debug(atalkpkt_debug,
		 ATALK_INTERFACE_MASK | ATALK_LOGLEVEL_MASK,
		 idb->phys_idb, 3))
	buginf("\nAT: atalk_send1 to (%a,%dk) cksum=%d (%d)",
	       daddr, atalk_pak_dsock(pak), atalk_pak_cksum(pak),
	       atalk_compute_crc(pak));

    dumping_output = at_debug(atalkpkt_debug, ATALK_INTERFACE_MASK,
			      idb->phys_idb);
    if (dhost == -1L) {
	/*
	 * If this is a broadcast, we need to check if we need a
	 * copy for ourselves. If so, we give ourself a copy as if
	 * we received it from the interface but
	 * we only count errors and continue processing.
	 */
	atalk_stat[ATALK_BCASTOUT]++;
	atalk_pak_setbroadcast(pak);
	if (atalk_getidb(pak->if_input) != idb) {
	    if (atalkif_BroadcastForMe(idb,daddr)) {
		new_pak = atalk_pak_duplicate(pak);
		if (new_pak != NULL) {
		    if (atalk_loopback(new_pak))
			atalk_stat[ATALK_BCASTIN]++;
		} else
		    atalk_stat[ATALK_NO_BUFFERS]++;
	    }
	}
    } else if (dhost == 0) {
	if (atalkif_CaymanTunnel(idb) || atalkif_AURPTunnel(idb))
	    dhost = -1;
	/* If this is for us and we will not hear ourselves, 
	 * then we just simulate a reception. 
	 */
	else if ((pak->flags & PAK_HEARSELF) == 0)
	    return atalk_loopback(pak);
	else 
	    dhost = daddr;
    }

    ATASSERT(dhost != 0);

    len = atalk_pak_rawlength(pak);
    oldsize = pak->datagramsize;
    oldstart = pak->datagramstart;
    if (atalk_encaps(pak, len, dhost)) {

	/* Now update any route-caches */
	if (dhost == -1L) {
	    if (atalk_pak_dsock(pak) == ATALK_SOCK_SMRP)
		reg_invoke_atalk_smrp_fs_update_mcache(pak);
	} else if (dhost != atalkif_MyAddress(idb))
	    atalk_UpdateCache(pak);
	
	/* Now if it  to the domain
	 * make sure that we get all the domain
	 * stuff properly (like remapping and so on)
	 */
	if(atalk_DomainRouter_running)
	    atdomain_DDPoutProcess(pak);
	
	atalk_stat[ATALK_OUTPUT]++;
	if (dumping_output)
	    at_pak_dumphdr(pak, pak->if_output, "encap'ed output packet");

	if (at_debug(atalkpkt_debug,
		     ATALK_INTERFACE_MASK | ATALK_LOGLEVEL_MASK,
		     idb->phys_idb, 3))
	    buginf("\nAT: atalk_send1, post-encap: to (%a,%dk) cksum=%d (%x)",
		   dhost, atalk_pak_dsock(pak),
		   atalk_pak_cksum(pak), atalk_compute_crc(pak));

	datagram_out(pak);	/* queue for output */
	return (TRUE);		/* a job well done */
    } else {			/* Failed encapsulation */
	pak->datagramstart = oldstart;
	pak->datagramsize = oldsize;
	if ((pak->enc_flags & ENC_NOARPENTRY) == 0) {
	    if (dumping_output)
		at_pak_dumphdr(pak, pak->if_output, "encapsulation failed");
	    hwidb->atalk_outputerrs++;
	    atalk_pak_printbad(pak, "encapsulation failed", idb);
	    atalk_dispose(ATALK_ENCAPSFAILED, pak);
	}
	return (FALSE);
    }
}

/* 
 * atalk_send
 * send an appletalk packet
 *
 * NOTE:  this routine assumes pak->if_output is NULL in the standard case,
 *	  which will force a route lookup.  If pak->if_output is set,
 *	  we assume the packet has a destination local to the
 *	  named interface cable.  It is an error to set the output
 *	  interface if the destination is not known to be local.
 */
boolean 
atalk_send (paktype *pak)
{
    long dhost;
    atalkidbtype	*idb_i;
    atalkidbtype	*idb_o;
    hwidbtype	*hwidb;
    boolean 	intercepted;


    /* if this is going to get intercepted by us, we don't need to route
	it.  In fact, trying to route it will cause problems due to the 
	NULL idb for the route. */
    intercepted = FALSE;
    reg_invoke_atalk_intercept(pak, &intercepted);
    if (intercepted) {
	return(TRUE); 
    }

    dhost = atroute_FirstHop(pak);
    idb_i = atalk_getidb(pak->if_input);
    idb_o = atalk_getidb(pak->if_output);
    hwidb = (idb_i ? hwidb_or_null(idb_i->phys_idb) : NULL);

    if (idb_i && at_debug(atalkpkt_debug,
		 ATALK_INTERFACE_MASK | ATALK_LOGLEVEL_MASK,
		 idb_i->phys_idb, 3))
	buginf("\nAT: atalk_send to (%dn,%da,%dk) cksum=%d (%d)",
		atalk_pak_dnet(pak), atalk_pak_dnode(pak),
		atalk_pak_dsock(pak), atalk_pak_cksum(pak),
		atalk_compute_crc(pak));

    if ((dhost <= 0) && (idb_o == NULL)) {	/* no route to addr */
	atalk_pak_printbad(pak,"no route",idb_i);
	if (idb_i)
	    hwidb->atalk_inputerrs++;
	atalk_dispose(ATALK_NOROUTE, pak);
	return(FALSE);
    }
    if (idb_o != NULL) {
	if (!atalkif_InterfaceUp(idb_o)) {
	    atalk_pak_printbad(pak,"port down",idb_o);
	    atalk_dispose(ATALK_NOT_OPERATIONAL, pak);
	    return (FALSE);
	}
	if (idb_o->atalk_accesslist != 0)
	    if (!atalk_accesscheck(atalk_pak_snet(pak),
				   idb_o->atalk_accesslist,
				   atutil_NodeIsBroadcast(atalk_pak_dnode(pak)))
		|| !atalk_sendcheck(atalk_pak_snet(pak),
				    idb_o->atalk_accesslist)) {
		atalk_pak_printbad(pak,"access denied",idb_o);
		atalk_dispose(ATALK_NOACCESS, pak);
		return (FALSE);
	    }
    }
    return(atalk_send1(pak, dhost));
}

/*
 * atalk_compute_crc
 * compute 16-bit ddp checksum for a ddp
 * should probably be recoded in assembler for speed
 */
ushort 
atalk_compute_crc (paktype* pak)
{
    ddplong *ddp = (ddplong *)pak->network_start;
    ushort cksum;
    uchar *p;
    int cnt;

    cksum = 0;
    cnt = GETSHORT(ddp->length) & DDPLENGTHMASK;
    cnt -= (uchar *)&ddp->dnet[0] - (uchar *)&ddp->length[0]; /* don't count these */
    p = (uchar *)&ddp->dnet[0];	/* begin checksumming here */
    while (cnt > 0) {
	cksum += *p;
	if (cksum & 0x8000) {	/* msb set? */
	    cksum <<= 1;	/* rotate left one bit */
	    cksum |= 1;		/* put msb into lsb */
	} else			/* msb not set */
	    cksum <<= 1;	/* so just rotate left one bit */
	p++;
	cnt--;	
    }
    if (cksum == 0)
	cksum = 0xffff;
    return(cksum);
}


/*
 * atalk_DDPcksum
 * calculate and store DDP checksum for an appletalk packet
 */
void
atalk_DDPcksum (paktype *pak)
{
    ushort cksum;

    if (atalk_pak_needscksum(pak))
	cksum = atalk_compute_crc(pak);
    else
	cksum = 0;
    atalk_pak_setcksum(pak, cksum);
}

/*
 * atalk_valid_DDPcksum
 * verify ddp checksum for a ddp is valid
 * valid iff checksum == 0 or computed checksum matches ddp->cksum
 */
boolean 
atalk_valid_DDPcksum (paktype* pak)
{
    ushort	pak_cksum;
    ushort	computed_cksum;

    pak_cksum = atalk_pak_cksum(pak);
    if (pak_cksum == 0)
	return (TRUE);
	
    computed_cksum = atalk_compute_crc(pak);
    if (computed_cksum != pak_cksum) {
	if (at_debug(atalkerr_debug, ATALK_NULL_MASK)) {
	    buginf("\nAT: invalid cksum, pak: %04x, computed: %04x",
		   pak_cksum, computed_cksum);
	    at_pak_dumphdr(pak, pak->if_input, "Cksum Err");
	}
	return (FALSE);
    }
    return (TRUE);
}

/*
 * atalk_forward
 * forward an appletalk packet
 */
void
atalk_forward (paktype *pak)
{
    int hc;
    idbtype *outidb;
    rbTree *tree;
    atalk_rdb_t *r;
    at_Domain *domain_in, *domain_out = NULL;
    
    hc = atalk_pak_hops(pak) + 1;

    if (atalk_DomainRouter_running) {
	tree = atroute_GetTree();
	r = atroute_LookupRoute(tree, atalk_pak_dnet(pak), FALSE);

	if (r && r->pathcount > 0) {
	    atalkidbtype *idb_out;
	    idb_out = r->rpath.idb;
	    if (idb_out) {
		domain_out = idb_out->atalk_dminfo;
	    }
	}
	domain_in = atalk_getidb(pak->if_input)->atalk_dminfo;

	if ((domain_in != domain_out) && (  (domain_in && domain_in->hop_rdctn)
					  || (domain_out && domain_out->hop_rdctn))) {
	    hc = 1;
	}
    }
    if (hc >= DDP_MAX_HOPCOUNT) {
	atalk_pak_printbad(pak,"hop count exceeded", atalk_getidb(pak->if_input));
	pak->if_input->hwptr->atalk_inputerrs++;
	atalk_dispose(ATALK_HOPCOUNT, pak);
	return;
    }

    atalk_pak_sethops(pak,hc);
    outidb = pak->if_output;
    if (atalk_send(pak)) 
	atalk_stat[ATALK_FORWARDED]++;
    else if (outidb != NULL)
        outidb->hwptr->atalk_outputerrs++;
}

