/* $Id: smds.c,v 3.15.12.12 1996/09/10 00:52:52 snyder Exp $
 * $Source: /release/112/cvs/Xsys/wan/smds.c,v $
 *------------------------------------------------------------------
 * SMDS
 *
 * August 6 1990, Vicki Ralls
 * 
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smds.c,v $
 * Revision 3.15.12.12  1996/09/10  00:52:52  snyder
 * CSCdi68568:  more constant opportunities, save memory
 *              156 out of image,
 *              116 out of data section
 * Branch: California_branch
 *
 * Revision 3.15.12.11  1996/08/28  13:22:41  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.15.12.10  1996/08/13  02:27:11  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.15.12.9  1996/07/23  13:32:10  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.15.12.8  1996/06/28  23:32:26  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.15.12.7  1996/06/27  09:45:48  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.15.12.6  1996/06/17  23:39:25  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.15.12.5  1996/05/17  12:16:53  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.15.2.3  1996/05/06  00:10:14  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.15.2.2  1996/04/26  07:59:58  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.15.2.1  1996/04/03  22:15:12  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.15.12.4  1996/05/09  14:49:24  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.15.12.3  1996/04/30  18:49:26  wmay
 * CSCdi54817:  Extra board encap added for ipx over frame-relay
 * Branch: California_branch
 *
 * Revision 3.15.12.2.16.1  1996/04/27  07:39:22  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.15.12.2.6.1  1996/04/08  02:43:34  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.15.12.2  1996/03/23  01:35:22  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.15.12.1  1996/03/18  22:48:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.15.16.1  1996/03/22  09:45:39  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.15  1996/02/13  08:17:46  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.14  1996/02/13  02:24:00  fred
 * CSCdi46765:  SDLC interface show fair-queue enabled
 *         Found in smds as well - turn it off
 *
 * Revision 3.13.6.1  1996/03/05  05:38:14  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.13.2.1  1996/02/27  21:21:08  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.13  1996/02/07  16:17:41  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.12  1996/02/05  10:26:41  amajumda
 * CSCdi48155:  problems with bridging on smds subinterfaces. The problem
 * is
 * that in the routine smds_process_bridge the check idb->tbridge_on_swidb
 * is
 * made at the interface level instead of the subinterface level. This is
 * fixed to check at the subinterface level.
 *
 * Revision 3.11  1996/02/01  06:12:34  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.10  1996/01/29  18:47:43  rharagan
 * CSCdi47442:  Router crashes  at address 0x4AFC4AFF
 * Fix crash in cbus_interrupt() if packet received while changing serial
 * encaps
 *
 * Revision 3.9  1996/01/29  07:31:26  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.8  1996/01/24  18:10:52  lmercili
 * CSCdi47361:  SMDS DXI does not work
 *
 * Revision 3.7  1996/01/12  23:27:05  lmercili
 * CSCdi45874:  Removal of subinterface causes router to reload
 *
 * Revision 3.6.2.1  1996/02/15  19:27:17  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.6  1995/12/14  21:00:24  wmay
 * CSCdi45297:  bridging does not work over smds subinterfaces - create
 * registry to convert from smds destination address to swidb that the
 * address is for.  Also some minor cleanups to get rid of global routines
 *
 * Revision 3.5  1995/11/20  22:36:43  wmay
 * CSCdi41555:  Spanning tree BPDUs being discarded by SMDS interface
 *
 * Revision 3.4  1995/11/20  22:28:42  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.3  1995/11/17  18:04:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:50:41  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:51:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:36:53  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/10/20  06:06:23  pst
 * CSCdi42495: Exorcise demons
 *
 * Revision 2.6  1995/09/15  19:22:42  lmercili
 * CSCdi34816:  add dynamic mapping for ipx/smds
 *
 * Revision 2.5  1995/09/09  00:55:00  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.4  1995/08/08  16:51:13  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.3  1995/06/21 14:39:57  lmercili
 * CSCdi35417:  SMDS addresses for subinterfaces shown wrong
 *
 * Revision 2.2  1995/06/20  20:38:28  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  23:19:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "../if/ether.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "../ui/parse_util.h"
#include "parser_defs_smds.h"
#include "serial.h"
#include "address.h"
#include "logger.h"
#include "mgd_timers.h"
#include "smds.h"
#include "../if/network.h"
#include "../if/arp.h"
#include "../ip/ip.h"
#include "../ip/ip_media.h"
#include "../ip/ip_registry.h"
#include "../atalk/atalk.h"
#include "../srt/span.h"
#include "../wan/serial_debug.h"
#include "../snmp/snmp_api.h"
#include "../snmp/ifmib_registry.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../if/tring.h"
#include "../if/priority_private.h"


/*
 * External Storage Declarations
 */
extern boolean serial_packet_debug;

static void smds_parse_map(parseinfo *, idbtype *);
static void smds_parse_bcast(parseinfo *, idbtype *);
static boolean smds_parse_proto_addr(parseinfo *, uint, addrtype *, uint);

static fuzzy smds_is_bcast(hwidbtype *hwidb, paktype *pak);


/*
 * Local Storage
 */
static const ulong smds_headerext[XHEL_SIZE] = {0,0,0};
static smdsmaptype *smdstable[SMDSHASH];
static ulong smds_counters[SMDS_MAX_COUNTERS];

/*
 * Managed Timer variables
 */
static mgd_timer master;


/*
 * smds_senddup:
 *    Send a duplicate packet to a multi-lis or broadcast addr.
 */
static void smds_senddup (
    idbtype *idb,
    paktype *pak,
    ipaddrtype dhost,
    uint link,
    uchar *smdshaddr)
{
    paktype *newpak;

    newpak = pak_duplicate(pak);
    if (newpak) {
        switch (link) {
        case LINK_IP:
            newpak->acct_proto = ACCT_PROTO_IP;
            break;
        case LINK_DECNET:
        case LINK_DECNET_ROUTER_L1:
        case LINK_DECNET_ROUTER_L2:
        case LINK_DECNET_PRIME_ROUTER:
        case LINK_DECNET_NODE:
            newpak->acct_proto = ACCT_PROTO_DECNET;
            break;
        case LINK_XNS:
            newpak->acct_proto = ACCT_PROTO_XNS;
            break;
        case LINK_APPLETALK:
            newpak->acct_proto = ACCT_PROTO_APPLE;
            break;
        case LINK_NOVELL:
            newpak->acct_proto = ACCT_PROTO_NOVELL;
            break;
        case LINK_APOLLO:
            newpak->acct_proto = ACCT_PROTO_APOLLO;
            break;
        case LINK_CLNS_BCAST:
        case LINK_CLNS_ALL_ES:
        case LINK_CLNS_ALL_IS:
	case LINK_ISIS_ALL_L1_IS:
	case LINK_ISIS_ALL_L2_IS:
            newpak->linktype = LINK_CLNS;
        case LINK_CLNS:
            newpak->acct_proto = ACCT_PROTO_CLNS;
            break;
        case LINK_BRIDGE:
            newpak->acct_proto = ACCT_PROTO_BRIDGE;
            break;
        case LINK_DEC_SPANNING:
        case LINK_IEEE_SPANNING:
            newpak->acct_proto = ACCT_PROTO_SPAN;
            break;
        case LINK_VINES:
            newpak->acct_proto = ACCT_PROTO_VINES;
            break;
        }
	/*
	 * We now know the E.164 destination address, and have filled in the
	 * the 802.2/SNAP header if we're using it.
	 * Now fill in the SMDS L3 Header & Trailer.  Its not a pretty sight.
	 */
        if (!smds_l3_encapsulate(newpak, newpak->datagramsize, idb, smdshaddr)){
            if (serial_debug) {
		smdstype *smds_hdr = (smdstype *)newpak->datagramstart;
		snap_hdr *snap = (snap_hdr *)smds_hdr->data;
		buginf("\nSMDS: Send, Encapsulation error, type=0x%x",
		       snap->type);
	    }
	    retbuffer(newpak);
	}
        show_smds_packet(idb, newpak, 1);

	if (idb->hwptr && idb->hwptr->board_encap) {
	    if (!(*idb->hwptr->board_encap)(pak,idb->hwptr)) {
		retbuffer(pak);
		return;
	    }
	}
	/*
	 * Make sure routing cache is populated by transit traffic.
	 */
	reg_invoke_fs_cache_update(newpak->linktype, newpak, 0L);

        datagram_out(newpak);
        smds_counters[SMDS_OUTPUT]++;
    } 
}

/*
 * smds_broadcast:
 *    Called to broadcast a packet to all multicast adresses that
 *    have a matching IP addr.  Only used in the IP/ARP case for
 *    multi-lis
 */
static uchar *smds_broadcast (
    idbtype *idb,
    paktype *pak,
    ipaddrtype dhost,
    uint link)
{
    smdsaddr *fmap = NULL, *bca;
    smds_pdb_t *spd = idb->hwptr->smds_pdb;
    smdsmaptype *bmt, *bmap = NULL;
    uint ndx, olink = link;

check_again:
    for (bca = spd->smds_bcast; bca; bca = bca->next) {
        if (bca->link == link && bca->idb == idb) {
            if (link == LINK_IP || link == LINK_ARP) {
		/*
		 * Do special checking for IP/ARP to find the proper multicast
		 */
		if (!IPMULTICAST(dhost) && olink == LINK_IP) {
		    continue;
		}
                if (fmap == NULL) {
                    fmap = bca;
                } else {
		    smds_senddup(idb, pak, dhost, link, bca->addr);
		}
            } else {    /* Not IP or ARP */
                fmap = bca;
		break;
            }
        }
    }

    if (!fmap) {
        for (ndx = 0; ndx < SMDSHASH; ndx++) {
            bmt = smdstable[ndx];
            while (bmt) {
                if (bmt->link == link && bmt->idb == idb && bmt->broadcast) {
                    if (bmap == NULL) {
                        bmap = bmt;
                    } else {
			smds_senddup(idb, pak, dhost, link, bmt->hardware);
		    }
                }
                bmt = bmt->next;
            }
        }
    }
    if (bmap)
        return(bmap->hardware);

    if (link == LINK_ARP && fmap == NULL) {    /* Look at the IP side now. */
        link = LINK_IP;
        goto check_again;
    }

    return(fmap ? fmap->addr : 0);
}

/*
 * smds_broadcast_lookup_by_addr
 */
static boolean smds_bcast_lookup_by_addr (uchar *address, idbtype *idb)
{
    smds_pdb_t *spd = idb->hwptr->smds_pdb;
    smdsaddr *bca;
    
    for (bca = spd->smds_bcast; bca; bca = bca->next) {
	if (bca->idb == idb) {
	    if (bcmp(address, bca->addr, STATIONLEN_SMDS64) == 0)
	        return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * smds_addr_to_swidb
 * Find address
 */
static idbtype *smds_addr_to_swidb (void *addr, hwidbtype *hwidb)
{
    idbtype *idb;
    uchar *smdsdest;

    smdsdest = (uchar *) addr;
    if (SMDS_ISINDIV(smdsdest[0])) {
	FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
      	    if (idb->vaddr && smdsequal(smdsdest, idb->vaddr->addr)) {
		return(idb);
	    }
	}
    } else {
	if (SMDS_ISGROUP(smdsdest[0])) {
	    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
		if (smds_bcast_lookup_by_addr(smdsdest, idb)) {
		    return(idb);
		}
	    }
	}
    }
    return(NULL);
}

static boolean smds_address_check (paktype *pak, ulong link)
{
    smdstype *header = (smdstype *)pak->datagramstart;
    hwidbtype *hwidb;
    idbtype *swidb;

    if (pak->if_input == NULL)
	return(FALSE);

    hwidb = pak->if_input->hwptr;

    swidb = smds_addr_to_swidb(header->smdsdest, hwidb);
    if (swidb != NULL) {
	pak->if_input = swidb;
	return(TRUE);
    }
    
    if (serial_debug)
      buginf("\n SMDS: Packet %E - link %d, not addressed to us", 
	     header->smdsdest, link);
    smds_counters[SMDS_INVLD_ADDR]++;
    return(FALSE);
}
/*
 * get_bridge_entry
 * Function to determine if a mac level address exists in the bridge database.
 */
static btetype *smds_get_bridge_entry (
    idbtype *idb,
    uchar *mac_addr)
{
    ulong ultemp;
    spantype *span;
    btetype *mac;
    leveltype level;

    ultemp = mac_addr[4];
    ultemp ^= mac_addr[5];
    span = idb->span_ptr;
    if (span) {
	level = raise_interrupt_level(NETS_DISABLE);
	for (mac = span->bridge_tbl[ultemp]; mac; mac = mac->bte_next) {
	    if (ieee_equal(mac_addr, (uchar *) &mac->bte_mac_addr.sword[0])) {
		reset_interrupt_level(level);
		if ((mac->bte_action == BRIDGE_RECEIVE) && 
		    !(mac->bte_attributes & BTE_LAT))
		  return(NULL);
		return(mac);
	    }
	}
	reset_interrupt_level(level);
    }
    return(NULL);
}

/*
 * smds_bridge_datagram()
 *	Called to bridge an IP/ARP/LAT datagram.
 */
uchar *smds_bridge_datagram (
    paktype *pak,
    long address)
{
    btetype *mac;
    uchar *mac_dest;

    switch(pak->linktype) {
    case LINK_IP:
	mac_dest = reg_invoke_ip_transit_bridging(address, pak->if_output);
	if (!mac_dest)
	    return(NULL);
	break;
    case LINK_BRIDGE:
	if ((address != 0L) && (address != -1L)) {
    	    mac = smds_get_bridge_entry(pak->if_output, (uchar *)address);
	    return((mac) ? (uchar *)&mac->bte_proxy_addr : NULL);
	}
    case LINK_DEC_SPANNING:
    case LINK_IEEE_SPANNING:
	return(smds_broadcast(pak->if_output, pak, address, 
	        LINK_BRIDGE));
	break;
    default:
        if ((pak->enctype == ET_BRIDGE_ENCAPS) && is_ieee_bcast((uchar *)address)) {
           /*
            * ET_BRIDGE_ENCAPS will only be set by the arp routines when we
            * wish to force transit bridge encapsulation no matter if
            * mac_dest is in the bridge table or not.  The call to
            * ieee_equal() is only a sanity check.  The only difference
            * between this encapsulation and the following is the use of
            * the bridge multicast address instead of something from the
            * bridge table.
            */
	    pak->enctype = ET_SMDS;
	    if (!ether_vencap(pak, address))
		return(NULL);
	    if (pak->linktype == LINK_ARP) {
		/*
		 * ARP passes a MAC address, but the next couple of routines
		 * need to look at an IP address because they do special
		 * things.  Get that address for them now.
		 */
		smds_arp *arpptr = (smds_arp *)smds_arp_headstart(pak);
		address = arpptr->iptpro;
	    }
	    return(smds_broadcast(pak->if_output, pak, address, 
				  pak->linktype));
	}
        mac_dest = (uchar *) address;
	break;
    }

    /*
     * IP datagram AND we know the MAC level address of where to send it.
     * If the datagram is in the bridging database -- encapsulate it as
     * a transit bridging datagram, else return FALSE (FDDI encapsulation).
     */
    mac = smds_get_bridge_entry(pak->if_output, mac_dest);
    if (mac) {
	pak->enctype = ET_SMDS;
        if (ether_vencap(pak, address)) {
	    return((uchar *)&mac->bte_proxy_addr);
        }
    }
    return(NULL);

}


/*
 * smds_vencapsulate()
 */
static boolean smds_vencapsulate (
    paktype *pak,
    long address)
{

    idbtype *idb;
    hwidbtype *hwidb;
    int enctype = ET_SNAP;
    uchar const *daddr = NULL;
    sap_hdr *sap;
    snap_hdr *snap;
    uchar const *other_info;
    addrtype proto_addr;
    smdsmaptype *smdsmap = NULL;
    uchar *pad_ptr;

    idb = pak->if_output;
    hwidb = hwidb_or_null(idb);

    if (!interface_up(idb)) {
	if (serial_debug)
	    buginf("\nSMDS: %s: failed -- line down", idb->namestring);
	return(FALSE);
    }
    if ((hwidb->enctype != ET_SMDS) || (!hwidb->smds_lineup)) 
      return(FALSE);

    if (!idb->vaddr) {
	return(FALSE);
    }

    /*
     * Do address resolution.  We want an encapsulation method, a
     * destination address, and an optional pointer to any other
     * addressing information (presently any SNAP information).  We are
     * careful to reset pak->enctype with any new information.  The
     * destination address can be either a dstination MAC address (in
     * daddr) or a address structure (in proto_addr) which SMDS will use
     * to look up a static address map.
     */
    memset(&proto_addr, 0, sizeof(addrtype));
    if (!reg_invoke_media_smds_macaddr(pak->linktype, pak, address, &enctype, &daddr,
				   &other_info, &proto_addr)) {
	if (serial_debug) 
	    buginf("\nSMDS: %s: Bridge encap error type=%x, host=%x",
		   idb->namestring, pak->linktype, *(ulong *)daddr);
	if (!reg_used_media_smds_macaddr(pak->linktype))
	    errmsg(&msgsym(BADMACREG, LINK), idb->namestring,
		   pak->linktype);
	return(FALSE);
    }
    pak->enctype = enctype;

    switch (enctype) {
      case ET_SAP:
	pak->datagramstart -= SAP_HDRBYTES;
	pak->datagramsize += SAP_HDRBYTES;
	sap = (sap_hdr *)pak->datagramstart;
        sap->dsap    = other_info[0];
        sap->ssap    = other_info[1];
        sap->control = other_info[2];

	pak->info_start = pak->datagramstart;
	pak->encsize    = SAP_HDRBYTES;
	break;

      case ET_SNAP:
	pak->datagramstart -= SNAP_HDRBYTES;
	pak->datagramsize += SNAP_HDRBYTES;
	snap = (snap_hdr *)pak->datagramstart;
        snap->dsap    = SAP_SNAP;
        snap->ssap    = SAP_SNAP;
        snap->control = LLC1_UI;
	snap->oui[0]  = other_info[0];
	snap->oui[1]  = other_info[1];
	snap->oui[2]  = other_info[2];
	PUTSHORT(&snap->type,link2serial(pak->linktype));

	pak->info_start = pak->datagramstart;
	pak->encsize    = SNAP_HDRBYTES;
	break;

      case ET_BRIDGE:
      case ET_BRIDGE_ENCAPS:
	if ((pak->linktype == LINK_DEC_SPANNING) ||
	    (pak->linktype == LINK_IEEE_SPANNING)) {
	    pak->datagramstart -= (SNAP_HDRBYTES);
	    pak->encsize = SNAP_HDRBYTES;
	    pak->datagramsize += (SNAP_HDRBYTES);
	    snap = (snap_hdr *)pak->datagramstart;
	    snap->type = SMDS_BRIDGE_PID_BPDU;
	    pak->linktype = LINK_BRIDGE;
	} else {
	    pak->datagramstart -= (SNAP_HDRBYTES + SMDS_BRIDGE_PAD_SIZE);
	    pak->encsize = SNAP_HDRBYTES + SMDS_BRIDGE_PAD_SIZE;
	    pak->datagramsize += (SNAP_HDRBYTES + SMDS_BRIDGE_PAD_SIZE);
	    snap = (snap_hdr *)pak->datagramstart;
	    pad_ptr = pak->datagramstart + SNAP_HDRBYTES;
	    *pad_ptr++ = SMDS_BRIDGE_PAD_DATA;
	    *pad_ptr = SMDS_BRIDGE_PAD_DATA;
	    PUTSHORT(&snap->type, SMDS_BRIDGE_PID_8023);
	}

	pak->info_start = pak->datagramstart;
	snap->dsap    = SAP_SNAP;
	snap->ssap    = SAP_SNAP;
	snap->control = LLC1_UI;
	snap->oui[0]  = (SMDS_BRIDGE_OUI >> 16) & 0xFF;
	snap->oui[1]  = (SMDS_BRIDGE_OUI >>  8) & 0xFF;
	snap->oui[2]  = (SMDS_BRIDGE_OUI >>  0) & 0xFF;
	break;

      case ET_NULL:
        /* just add the mac layer */
	break;

      default:
	return(FALSE);
     }

    pak->rif_start = NULL;
    pak->riflen	   = 0;	


    /*
     * The registry call to the protocol specific routine may or may not
     * have returned a destination MAC address.  If it returned a
     * broadcast address, then look in the mapping tables to get the
     * multicast address for that protocol.  If the protocol specific
     * routine did not, it returned an address data structure that can be
     * used to look into the SMDS static mapping tables.
     *
     * Note: If this is a broadcast and there is no multicast address
     * configured for this protocol, then the smds_broadcast routine is
     * called.  This routine will walk the static mapping tables looking
     * for entries marked as broadcast, and will send a copy of this
     * packet to all but one of them.  That one is sent by letting this
     * routine complete.
     */
    if (daddr == baddr)  {
	if (pak->linktype == LINK_ARP) {
	    /*
	     * ARP passes a MAC address, but the next couple of routines
	     * need to look at an IP address because they do special
	     * things.  Get that address for them now.
	     */
	    smds_arp *arpptr = (smds_arp *)smds_arp_headstart(pak);
	    address = arpptr->iptpro;
	}
        daddr = smds_broadcast(idb, pak, (ipaddrtype)address, pak->linktype);
        if (!daddr)
            daddr = smds_bcast_lookup(pak->linktype, idb,
				      (ipaddrtype)address, TRUE);
    } else {
	smdsmap = smds_addr_lookup(&proto_addr, pak->linktype);
        if (smdsmap) {
	    daddr = smdsmap->hardware;
	}
    }
    if (!daddr) {
        if (serial_debug)
          buginf("\nSMDS send: Error in encapsulation, no hardware address, type = 0x%x", 
             link2serial(pak->linktype));
        return(FALSE);
    }
    /*
     * We now know the E.164 destination address, and have filled in the
     * the 802.2/SNAP header if we're using it.
     * Now fill in the SMDS L3 Header & Trailer.  Its not a pretty sight.
     */
    if (!smds_l3_encapsulate(pak, pak->datagramsize, idb, daddr)) {
        if (serial_debug)
           buginf("\nSMDS: Send, Error in encapsulation, type=0x%x",
		  link2serial(pak->linktype));
        return(FALSE);
    }
    show_smds_packet(idb, pak, 1);

    /*
     * Make sure routing cache is populated by transit traffic.
     */
    if (hwidb->board_encap) {
	if (!(*hwidb->board_encap)(pak,hwidb)) {
	    return(FALSE);
	}
    }
    if (!(pak->flags & PAK_ENCAPS_ONLY)) {
	reg_invoke_fs_cache_update(pak->linktype, pak, 0L);
	smds_counters[SMDS_OUTPUT]++;
    }
    return(TRUE);
}

boolean smds_l3_encapsulate (paktype *pak, int len, idbtype *idb,
			     uchar const *daddr)
{
    uchar *ptr;
    int pad, i, r;
    smdstype *header;
    ciscosmds *cs;
    dxismds *ds;
    smds_pdb_t *spd = idb->hwptr->smds_pdb;
    unsigned infolen = len;

    pak->datagramstart -= SMDS_SMDSBYTES;
    header = (smdstype *)pak->datagramstart;
    cs = (ciscosmds *)(pak->datagramstart - 2);
    ds = (dxismds *)(pak->datagramstart - SMDS_DXIBYTES);

/*
 * If DXI mode, prepend the DXI header.
 */
    if (spd->dximode) {
        r = SMDS_DXIBYTES;
        ds->dxi.addr = DXI_CMDTODSU;
        ds->dxi.control = DXI_UI;
        PUTSHORT(&ds->dxi.congest, 0);
	pak->enctype = ET_SMDS_DXI;
    } else {        /* Cisco mode */
        r = 2;
        PUTSHORT(&cs->cisco_id, 0);
	pak->enctype = ET_SMDS;
    }
    pak->datagramstart -= r;
    pak->encsize += SMDS_SMDSBYTES + r;
    pak->mac_start = pak->datagramstart;
    pak->addr_start = header->smdsdest;
    header->smds_rsvd = 0;
    len += SMDS_SMDSBYTES + r;
    header->smds_betag = idb->hwptr->smds_begin_tag++;
    bcopy(daddr, header->smdsdest, STATIONLEN_SMDS64);
    bcopy(idb->vaddr->addr, header->smdssrc, STATIONLEN_SMDS64);
    header->smds_xhel = XHEL_SIZE;
    PUTSHORT(&header->smds_x, 0x0000);
    bcopy((uchar *)smds_headerext, header->smds_hel, 12);

    /*
     * Calculate the pad size. Remember to subtract the pseudo hdlc
     * bytes off first (sizeof 2).
     */
    pad = 3 - ((infolen + 3) % 4);
    ptr = pak->datagramstart + len;
    PUTSHORT(&header->smds_basize,
	     (len - (SMDS_SMDSBYTES+r) + SMDS_HEADER + pad));
    /* 
     *The lower two bits of the higher level protocol
     * identifier field is used as a count of pad bytes
     *
     * All higher level protocols use LLC/SNAP headers
     */
    if (spd->necmode)
        header->smds_xhlpi = 1;  /* SMDS LLC indicator, unshifted */
    else
        header->smds_xhlpi = pad | SMDS_LLC;
    /* 
     * Now pad with zeros, put in the stupid @!**# trailer,
     * set the datagramsize, and check the length
     */
    for (i = 0; i < pad; i++)
        *ptr++ = 0;    
    bcopy(&header->smds_rsvd, ptr, SMDS_TRAILER);
    header->smds_hel[0] = 3;
    header->smds_hel[1] = 0;
    header->smds_hel[2] = 1;

    pak->datagramsize = len + pad + SMDS_TRAILER;
    if (pak->datagramsize > idb->hwptr->max_pak_size)
        return(FALSE);
    return(TRUE);
}

static boolean smds_process_bridge (paktype *pak,
				    hwidbtype *hwidb,
				    idbtype *idb)
{
    if ((!smds_address_check(pak, LINK_BRIDGE))) {
	retbuffer(pak);
	return(TRUE);
    }

    idb = pak->if_input;

    if (!idb->tbridge_on_swidb) {
	retbuffer(pak);
	return(TRUE);
    }
    pak->enctype = ET_ARPA;  /* Mark as bridge encapsulation */
    pak->flags |= PAK_TBRIDGE_IN;
    pak->datagramstart += ((hwidb->encsize -
			    hwidb->smds_pdb->fast_input_modelen) + 
			   SMDS_BRIDGE_PAD_SIZE);
    pak->datagramsize -= ((hwidb->encsize -
			   hwidb->smds_pdb->fast_input_modelen) + 
			  SMDS_BRIDGE_PAD_SIZE);
    net_bridge_copy(pak);
    if (pak->if_output) {       /* If !0 tbridge did its work */
	retbuffer(pak);
	return(TRUE);
    }
    pak->datagramstart -= ((hwidb->encsize -
			    hwidb->smds_pdb->fast_input_modelen) +
			   SMDS_BRIDGE_PAD_SIZE);
    pak->datagramsize += ((hwidb->encsize -
			   hwidb->smds_pdb->fast_input_modelen) +
			  SMDS_BRIDGE_PAD_SIZE);
    return(FALSE);
}

/*
 * smds_input
 * Handle input packets with SMDS format.
 * Basically:
 *    Pre-lim. qualify the packet header.
 *    Shift data over 6 bytes to the 'line' if packet is CLNS
 *    Call raw_enqueue() to deencapsulate as necessary the SMDS stuff.
 */

void smds_input (
    hwidbtype *hwidb,
    paktype *pak)
{
    smdstype    *header, *eheader;
    smdshetype  *smdshe;
    smds_pdb_t  *spd = hwidb->smds_pdb;
    uint	 modelen = SMDS_CISCOBYTES;	/* Default to ciscomode */
    snap_hdr    *snap;
    idbtype 	*idb = hwidb->firstsw;

    pak->if_input = idb;		/* Default for now. */
    show_smds_packet(idb, pak, 0);

    /*
     * If DXI mode, strip the DXI header off the packet.
     * Else Cisco mode so strip off the 2 byte(ZERO) header.
     */
    if (spd->dximode) {    /* DXI 3.2 mode */
	dxitype *dxihdr = (dxitype *)pak->datagramstart;
	/*
	 * Allow back-to-back routers to work if keepalive is off.
	 */
	if (hwidb->nokeepalive)
	    dxihdr->addr &= ~DXI_S_BIT;
	/*
	 * Test DXI header for consistency
	 */
	if ((dxihdr->addr & DXI_LL_BITS) == DXI_LM) {
	    /*
	     * DXI link management frame
	     */
	    smds_lm_frame(hwidb->firstsw, pak);
 	    smds_counters[SMDS_INPUT]++;
	    return;
	} else if (((dxihdr->addr & DXI_LL_BITS) != DXI_DATA) ||
		     ((dxihdr->addr & DXI_S_AE) != DXI_AE_BIT) ||
		       (dxihdr->control != DXI_UI)) {
	    /*
	     * Not a valid DXI data frame: discard
	     */
	    smds_counters[SMDS_DXI_BADFRAME]++;
	    retbuffer(pak);
	    return;
	}
	modelen = SMDS_DXIBYTES;
    }
    if (pak->datagramsize < SMDS_ENCAPBYTES1) {
        if (serial_debug) {
          buginf("\n SMDS(%s): BAD PACKET received, datagramsize=%d",
                hwidb->hw_namestring, pak->datagramsize);
        }
        retbuffer(pak);
        return;
    }
    pak->datagramstart += modelen;
    pak->datagramsize -= modelen;
    header = (smdstype *) pak->datagramstart;
    eheader = (smdstype *)((char *)header + header->smds_basize+4);
    smdshe = (smdshetype *) header->smds_hel;

    /*
     * Qualify the packet header before processing
     */
    if (header->smds_rsvd != 0 || eheader->smds_rsvd != 0) {
        if (serial_debug)
          buginf("\n SMDS: Invalid packet, Reserved NOT ZERO, %d,%d ",
		 header->smds_rsvd, eheader->smds_rsvd);
        retbuffer(pak);
        return;
    }
    if (eheader->smds_betag != header->smds_betag) {
        if (serial_debug)
          buginf("\n SMDS: Invalid packet, TAG mismatch %d,%d ",
		 header->smds_betag, eheader->smds_betag);
        smds_counters[SMDS_BAD_BETAG]++;
        retbuffer(pak);
        return;
    }
    if (GETSHORT(&eheader->smds_basize) != header->smds_basize) {
        if (serial_debug)
          buginf("\n SMDS: Invalid packet, Bad TRAILER length %d,%d ",
		 header->smds_basize, GETSHORT(&eheader->smds_basize));
        smds_counters[SMDS_BAD_BASIZE]++;
        retbuffer(pak);
        return;
    }
    if (header->smds_basize < SMDS_MINPAKLEN 
        || header->smds_basize > SMDS_MAXPAKLEN) {
        if (serial_debug)
          buginf("\n SMDS: Invalid packet, Bad BA length %d ",
		 header->smds_basize);
        smds_counters[SMDS_BAD_BASIZE]++;
        retbuffer(pak);
        return;
    }
    if ((header->smds_xhel & SMDS_XHEL_HEL) != XHEL_SIZE) {
        if (serial_debug)
          buginf("\n SMDS: Invalid packet, Bad header extension length %d ",
		 header->smds_xhel);
        smds_counters[SMDS_BAD_XHEL]++;
        retbuffer(pak);
        return;
    }

    /*
     * Verify the Header extension 'Version' values only
     */
    if (smdshe->smdshe_length != SMDSHE_VERLEN) {
        if (serial_debug)
          buginf("\n SMDS: Invalid packet, Bad header extension length %d ",
		 smdshe->smdshe_length);
        smds_counters[SMDS_BAD_XHEL]++;
        retbuffer(pak);
        return;
    }
    if (smdshe->smdshe_type != SMDSHE_VERTYP) {
        if (serial_debug)
          buginf("\n SMDS: Invalid packet, Bad header extension type %d ",
		 smdshe->smdshe_type);
        smds_counters[SMDS_BAD_XHEL]++;
        retbuffer(pak);
        return;
    }
    if (smdshe->smdshe_value != SMDSHE_VERVAL) {
        if (serial_debug)
          buginf("\n SMDS: Invalid packet, Bad header extension value %d ",
		 smdshe->smdshe_value);
        smds_counters[SMDS_BAD_XHEL]++;
        retbuffer(pak);
        return;
    }

    if (!SMDS_ISINDIV(header->smdsdest[0]) && 
      !SMDS_ISGROUP(header->smdsdest[0])) {
	if (serial_debug)
	    buginf("\n SMDS(%s): Invalid Destination E.164 address format %E",
		idb->namestring, header->smdsdest);
	smds_counters[SMDS_INVLD_ADDR]++;
	retbuffer(pak);
	return;
    }
    if (!SMDS_ISINDIV(header->smdssrc[0]) && 
      !SMDS_ISGROUP(header->smdssrc[0])) {
	if (serial_debug)
	    buginf("\n SMDS(%s): Invalid Source E.164 address format %E",
		idb->namestring, header->smdssrc);
	smds_counters[SMDS_INVLD_ADDR]++;
	retbuffer(pak);
	return;
    }
/*
 * Switch packet over if CLNS to align data with the 'line'
 */
    smds_counters[SMDS_INPUT]++;
    pak->flags &= ~PAK_TBRIDGE_IN;
    /*
     * Remove PAD & Trailer from datagramsize.  Protocols might see
     * cruft at the end.  TRAILER is *ALWAYS* 4 bytes in length.
     * PAD may be 0 - 3 bytes in length as specified in xhlpi.
     */
    pak->datagramsize -= ((header->smds_xhlpi & 0x3)+4); 

    snap = (snap_hdr *)header->data;

    if (snap->dsap == SAP_SNAP) {
	if (ieee_equal_oui(snap->oui, smds_bridge_oui)) {
	    switch (snap->type) {
	    case SMDS_BRIDGE_PID_8023FCS:
		pak->datagramsize -= sizeof(ulong);
		/* fall through */
	    case SMDS_BRIDGE_PID_8023:
		if (smds_process_bridge(pak,hwidb,idb))
		    return;
	    case SMDS_BRIDGE_PID_FDDIFCS:
		pak->datagramsize -= sizeof(ulong);
		/* fall through */
	    case SMDS_BRIDGE_PID_FDDI: {
		uchar *dgstart, *dgend;
		ushort ix;

		PUTSHORT(&snap->type, SMDS_BRIDGE_PID_8023);
		dgstart = snap->data + SMDS_BRIDGE_PAD_SIZE;
		dgend = dgstart + TR_ACFC_SIZE; /* should be 1 byte */
		for (ix = 0; ix < (IEEEBYTES * 2); ix++) {
		    *dgstart++ = BITSWAP(*dgend++);
		}
		PUTSHORT(dgstart,
			 pak->datagramsize -
			 ((hwidb->encsize -
			   hwidb->smds_pdb->fast_input_modelen) + 
			  SMDS_BRIDGE_PAD_FDDI_SIZE));
		pak->datagramstart += SMDS_BRIDGE_PAD_FDDI_EXTRA;
		if (smds_process_bridge(pak,hwidb,idb))
		    return;
	    }
	    case SMDS_BRIDGE_PID_8025FCS:
		pak->datagramsize -= sizeof(ulong);
		/* fall through */
	    case SMDS_BRIDGE_PID_8025: {
		uchar *dgstart, *dgend;
		ushort ix;

		PUTSHORT(&snap->type, SMDS_BRIDGE_PID_8023);
		dgstart = snap->data + SMDS_BRIDGE_PAD_SIZE;
		dgend = dgstart + TR_ACFC_SIZE; /* should be 1 byte */
		for (ix = 0; ix < (IEEEBYTES * 2); ix++) {
		    *dgstart++ = BITSWAP(*dgend++);
		}
		PUTSHORT(dgstart,
			 pak->datagramsize -
			 ((hwidb->encsize -
			   hwidb->smds_pdb->fast_input_modelen) + 
			  SMDS_BRIDGE_PAD_SIZE));
		if (smds_process_bridge(pak,hwidb,idb))
		    return;
	    }
	    case SMDS_BRIDGE_PID_BPDU:
		/*
		 * BPDU code needs the network start to be the
		 * start of the BPDU - we mistakenly set it in
		 * parse_packet
		 */
		pak->network_start = snap->data;
		break;
	    default:
		break;
	    }

	} else {
	    
	    switch (snap->type) {
	    case TYPE_BRIDGE:
		if (smds_process_bridge(pak,hwidb,idb))
		    return;
		break;

	    case TYPE_ETHERTALK:
		ieee_copy_oui(apple_snap, snap->oui);
		break;
	    }
	}
    } else if (snap->dsap == SAP_CLNS) {
	/*
	 * Handle bizarro CLNS case
	 *
	 * Because of the restrictions of the Sniff Buffer, we
	 * don't know if the frame is a CLNS one until now. And
	 * the frame's been aligned to a SNAP encapsulated packet.
	 * So, back the network start up to where it should be,
	 * set the encapsulation properly and shorten the size
	 * we thought the encapsulation was.
	 */
	pak->enctype = ET_SAP;
	pak->network_start -= (SNAP_HDRBYTES - SAP_HDRBYTES);
	pak->encsize       -= (SNAP_HDRBYTES - SAP_HDRBYTES);
    } else if (sap_get_entry(pak->if_input,snap->dsap)) {
        pak->enctype = ET_LLC2;
	pak->network_start -= (SNAP_HDRBYTES - SAP_HDRBYTES);
	pak->encsize       -= (SNAP_HDRBYTES - SAP_HDRBYTES);
    }

    raw_enqueue(hwidb, pak);
    return;
}

/*
 * show_smds_packet
 * buginf smds packet in raw form. Debugging tool.
 */

void show_smds_packet (
    idbtype *idb,
    paktype *pak,
    int outbound)
{
    int yu, ku, cnt = 0;
    char buff[150];
    uchar *tb, *pakc;
    snap_hdr   *snap;
    smdstype   *header, *trailer;
    dxismds *dxi = (dxismds *)pak->datagramstart;
    smds_pdb_t *spd = idb->hwptr->smds_pdb;

    if (!serial_debug && !serial_packet_debug) 
    return;
    
    buginf("\nInterface %s %s SMDS L3 packet:", idb->namestring,
    outbound ? "Sending": "Receiving");

    if (spd->dximode) {
        buginf("\nDXI: Addr=%-02x, Control=%-02x, Congestion=%-04x ",
        dxi->dxi.addr, dxi->dxi.control, dxi->dxi.congest);
	if (dxi->dxi.control== (DXI_PF | DXI_TEST)) {
	    if ((dxi->dxi.addr == (DXI_LM | DXI_CMDTODSU)) ||
   	         (dxi->dxi.addr == (DXI_LM | DXI_RSPFRDSU))) 
		buginf("\nRouter heartbeat poll, sequence number = %u",
                        ((dxiheartbeat *)pak->datagramstart)->dxi_hbseq);
	    else if ((dxi->dxi.addr == (DXI_LM | DXI_CMDFRDSU)) ||
                        (dxi->dxi.addr == (DXI_LM | DXI_RSPTODSU)))
		buginf("\nDSU heartbeat poll, size = %d",
			pak->datagramsize);
	    else
		buginf("\nUnknown test frame, size = %d", pak->datagramsize);
	    return;
	}
        cnt = SMDS_DXIBYTES;
    } else
        cnt = 2;

    header = (smdstype *)(pak->datagramstart + cnt);
    snap = (snap_hdr *)header->data;
    if (serial_debug) {
        if (snap->dsap == SAP_SNAP) 
            buginf("\nSMDS: dgsize:%d SNAP-type:0x%04x src:%E dst:%E",
		   pak->datagramsize, snap->type,
		   header->smdssrc, header->smdsdest);
        else
            buginf("\nSMDS: dgsize:%d SAP-type:0x%02x%02x src:%E dst:%E",
		   pak->datagramsize, snap->dsap, snap->ssap,
		   header->smdssrc, header->smdsdest);
    }

    if (!serial_packet_debug)
        return;

    buginf("\nSMDS Header  : ");    
    buginf("RSVD: %-02x BEtag: %-02x Basize: %-04x ",
    header->smds_rsvd, header->smds_betag, 
    header->smds_basize);
    buginf("\nDest:%-08x%-08x Src:%-08x%-08x Xh:", 
	   GETLONG(header->smdsdest), GETLONG(header->smdsdest + 4),
	   GETLONG(header->smdssrc), GETLONG(header->smdssrc + 4));
    tb = (uchar *)header + 20;
    pakc = (uchar *)header;
    for (ku = 0, yu = 0; yu < 16; ku += 2, yu++) 
        sprintf(buff+ku, "%-02x\0", tb[yu]);

    buginf("%s\nSMDS LLC   : ", buff);    
    for (ku=0, yu = SMDS_SMDSBYTES; yu < SMDS_SMDSBYTES+8; yu++, ku+=3) 
        sprintf(buff+ku, "%-02x \0", pakc[yu]);

    buginf("%s\nSMDS Data  : ", buff);    
    for (cnt=ku=0, yu = SMDS_SMDSBYTES+8; cnt < 22; yu++, ku+=3, cnt++) 
        sprintf(buff+ku, "%-02x \0", pakc[yu]);
    buginf("%s\nSMDS Data  : ", buff);    
    for (cnt=ku=0; cnt < 22; yu++, ku+=3, cnt++) 
        sprintf(buff+ku, "%-02x \0", pakc[yu]);

    buginf("%s\nSMDS Data  : ", buff);    
    for (cnt=ku=0; cnt < 22; yu++, ku+=3, cnt++) 
        sprintf(buff+ku, "%-02x \0", pakc[yu]);

    buginf("%s\nSMDS Data  : ", buff);    
    for (cnt=ku=0; cnt < 22; yu++, ku+=3, cnt++) 
        sprintf(buff+ku, "%-02x \0", pakc[yu]);

    buginf("%s\nSMDS Data  : ", buff);    
    for (cnt=ku=0; cnt < 22; yu++, ku+=3, cnt++) 
        sprintf(buff+ku, "%-02x \0", pakc[yu]);

    buginf("%s\nSMDS Data  : ", buff);    
    for (cnt=ku=0; cnt < 22; yu++, ku+=3, cnt++) 
        sprintf(buff+ku, "%-02x \0", pakc[yu]);

    yu = header->smds_basize+4-8;
    buginf("%s\nSMDS Last  : ", buff);
    for (cnt=ku=0; cnt < 8; yu++, ku+=3, cnt++) 
        sprintf(buff+ku, "%-02x \0", pakc[yu]);

    trailer = (smdstype *)((char *)header + header->smds_basize+4);
    buginf("%s\nSMDS Trailer  : ", buff);
    buginf("RSVD: %-02x BEtag: %-02x Length: %-04x\n", trailer->smds_rsvd,
    trailer->smds_betag, trailer->smds_basize);
}


/*
 * smds_getlink
 * Get link type for a SMDS datagram.
 */

long smds_getlink (paktype *pak)
{
    smdstype   *header;
    snap_hdr   *snap;
    ulong link, tlink;

    header = (smdstype *)pak->datagramstart;
    snap = (snap_hdr *)pak->info_start;
    if(ieee_equal_oui(snap->oui, smds_bridge_oui)) {
    	if ((snap->type == SMDS_BRIDGE_PID_BPDU)) {
	/*
	 * force IEEE bpdu, span_process will sort that out later
	 */
	    link = LINK_IEEE_SPANNING;
	    tlink = LINK_BRIDGE;
	} else {
	    tlink = link = LINK_BRIDGE;
	    
	}
    } else if (snap->dsap == SAP_CLNS)  {
        tlink = link = LINK_CLNS;
    } else if (pak->enctype == ET_LLC2) {
        tlink = link = LINK_LLC2;
    } else {
        tlink = link = serial2link(snap->type & 0xFFFF);
    }

    if (!smds_address_check(pak, tlink))
        link = LINK_ILLEGAL;

    return(link);
}

/*
 * smds_command
 * Parse the "smds" command
 */

void smds_command (parseinfo *csb)
{
    idbtype *idb;
    hwidbtype *hwidb;
    smds_pdb_t *spd;

    idb = csb->interface;
    hwidb = idb->hwptr;
    if (hwidb->enctype != ET_SMDS) {
        if (!csb->nvgen) {
            printf("\n%%SMDS not configured on this interface");
        }
        return;
    }

    spd = hwidb->smds_pdb;

    if (csb->nvgen) {
        int i;
        smdsmaptype *p;

        switch (csb->which) {
        case SMDS_HARDWARE:
            nv_write((uint)idb->vaddr, "%s %E",
                 csb->nv_command, idb->vaddr->addr);
            break;
        case SMDS_NEC:
            nv_write(spd->necmode && spd->smds_enabled, csb->nv_command);
            break;

        case SMDS_GLEAN:
            if (idb->smds_glean) {
               nv_write(TRUE, csb->nv_command);
               nv_add(idb->smds_glean->link, " %s", link2name(idb->smds_glean->link));
               nv_add(idb->smds_glean->timeout != GLEAN_TIMEOUT_DEF, " %d",
                      idb->smds_glean->timeout/ONEMIN);
               nv_add(idb->smds_glean->bcast, " broadcast");
            }
            break;

        case SMDS_DXI:
	    nv_write(!spd->dximode, "no %s", csb->nv_command);
            break;

        case SMDS_MAP:
            for (i = 0; i < SMDSHASH; i++) {
                for (p = smdstable[i]; p; p = p->next) {
                    if ( (idb == p->idb) && (p->permanent) ) {
                        nv_write(TRUE, csb->nv_command);
                        smds_write_map(idb,p);
                    }
                }
            }
            break;
        case SMDS_BA:
            nv_smds_bcast(idb,csb->nv_command);
            break;
        case SMDS_ARP_ON:
            nv_write(idb->arp_smds, "%s", csb->nv_command);
            break;
        default:
            bad_parser_subcommand(csb, csb->which);
            break;
        }
        return;
    }

    switch (csb->which) {
    case SMDS_HARDWARE:
        if (csb->sense) {
	    if(!(uint)idb->vaddr) {
		idb->vaddr = malloc(sizeof(hwaddrtype));
	    }
	    if(!(uint)idb->vaddr) {
		printf(nomemory);
		return;
	    }
            bcopy(GETOBJ(hwaddr,1)->addr, idb->vaddr->addr, 
		  STATIONLEN_SMDS64);
	    if (!SMDS_ISINDIV(idb->vaddr->addr[0])) {
                printf("\n%%SMDS hardware address cannot be group type!");
		free(idb->vaddr);
		idb->vaddr = NULL;
                return;
            } 
            spd->smds_enabled = TRUE;
	    idb->vaddr->length = STATIONLEN_SMDS64;
            bcopy(idb->vaddr->addr, hwidb->hardware, STATIONLEN_SMDS64);
	    if( !spd->smds_hardware[0])
            	bcopy(idb->vaddr->addr, spd->smds_hardware, STATIONLEN_SMDS64);
        } else {
	    if(idb->vaddr) 
		free(idb->vaddr);
	    idb->vaddr = NULL;
        }
        break;

    case SMDS_NEC:
        spd->necmode = csb->sense;
        break;

    case SMDS_GLEAN:
        if (csb->sense) {
            if (!idb->smds_glean) {
                idb->smds_glean = malloc(sizeof(smds_gleantype));
            }
            if (!idb->smds_glean) {
                printf(nomemory);
                return;
            }
            idb->smds_glean->link  = GETOBJ(int,3);
            if ( !GETOBJ(int,1) ) {
                idb->smds_glean->timeout = GLEAN_TIMEOUT_DEF;
            } else {
                idb->smds_glean->timeout = ( GETOBJ(int,1)*ONEMIN );
            }
            idb->smds_glean->bcast = GETOBJ(int,2); 
        } else {
            if (idb->smds_glean)
                free(idb->smds_glean);
            idb->smds_glean = NULL;   
        }
        break;

    case SMDS_DXI:
	if (csb->set_to_default)
	    csb->sense = TRUE;
	if (csb->sense == spd->dximode)
	    return; /* no change - do nothing */
	/*
	 * A little sublety here.  Subtract off the old type amount, and
	 * add the new amount.  This way, any board encapsulation
	 * is preserved (for the MIP).
	 */
        if (csb->sense) {
            hwidb->encsize += SMDS_DXIBYTES - SMDS_CISCOBYTES;
	    hwidb->smds_pdb->fast_input_modelen += SMDS_DXIBYTES 
		- SMDS_CISCOBYTES;
	    hwidb->smds_pdb->fast_output_modelen += SMDS_DXIBYTES 
		- SMDS_CISCOBYTES;
	} else {
            hwidb->encsize += SMDS_CISCOBYTES - SMDS_DXIBYTES;
	    hwidb->smds_pdb->fast_input_modelen += SMDS_CISCOBYTES 
		- SMDS_DXIBYTES;
	    hwidb->smds_pdb->fast_output_modelen += SMDS_CISCOBYTES 
		- SMDS_DXIBYTES;
	}
	hwidb->span_encapsize = hwidb->encsize + SMDS_BRIDGE_PAD_SIZE;
        spd->dximode = csb->sense;
	reg_invoke_media_set_rxoffset(hwidb->type, hwidb);
	/*
	 * Clear the arp tables for this idb - also clear fast cache
	 */
	FOR_ALL_SWIDBS_ON_HW(hwidb,idb) {
	    reg_invoke_ar_cache_clear(idb);
	}
	reg_invoke_bump_cache_version(hwidb, NULL, FALSE);
        break;

    case SMDS_MAP:
        smds_parse_map(csb, idb);
        break;

    case SMDS_BA:
        smds_parse_bcast(csb, idb);
        break;

    case SMDS_ARP_ON:
        idb->arp_smds = csb->sense;
        break;

    default:
        bad_parser_subcommand(csb, csb->which);
        break;
    }
}

/*
 * smds_parse_map:    Install a static map entry for the interface
 *        csb->val1 == 'BROADCAST' flag(TRUE or FALSE).
 */
static void smds_parse_map (parseinfo *csb, idbtype *idb)
{
    int link;
    addrtype p_addr;
    smdsmaptype *addr;
    uchar addr2[SMDSBYTES];
    int pbroad = csb->val1;
    char buffer[40];
    
    memset(&p_addr, 0,sizeof(addrtype));
    link = GETOBJ(int,10);
    if (!smds_parse_proto_addr(csb, NONULL, &p_addr, link)) {
        printf("\n%%Unable to parse map address");
        return;
    }
    bcopy(GETOBJ(hwaddr,1)->addr, addr2, STATIONLEN_SMDS64);
    addr = smds_addr_lookup(&p_addr, link);
    if (!csb->sense) {
	if (addr) {
            smds_map_delete(addr);
        } else {
	    printf("\n%%Can't find address map for ");
	    print_proto_addr(idb, &p_addr, link, buffer);
	    printf(buffer);
        }
    } else {
    /*
     * Check to see if this entry is in the map in its
     * entirety. If so then update with newer information
     * but don't complain. Otherwise check each part of the
     * map for redundancy.
     */
        if (addr == NULL) {
            if (!smds_addr_enter(&p_addr, TRUE, addr2, 
				 csb->interface, 
				 link, pbroad)) {
                printf("\n%%Error entering address map");
            }
        } else {
            if (bcmp(addr->hardware, addr2, SMDSBYTES)) { 
                bcopy(addr2, addr->hardware, SMDSBYTES);
		addr->broadcast = pbroad;
                printf("\nSMDS - address replaced");
            } else {
                printf("\n%%Address already in map");
            }
	    /* static map entry replaces a dynamic entry */
	    if (!addr->permanent) {
		/* stop timer associated with dynamic map */
		mgd_timer_stop(&addr->timer);
	        addr->permanent = TRUE;
                addr->broadcast = pbroad;
                printf("\nDynamic map replaced with static map");
            }
        }
    }
}

/*
 * smds_addr_enter
 */

smdsmaptype *smds_addr_enter (
    addrtype *addr,
    boolean permanent,
    uchar *addr2,
    idbtype *idb,
    uint link,
    uint pbroad)
{
    int hash;
    smdsmaptype *l;
    
    l = malloc(sizeof(smdsmaptype));
    if (l) {
	l->dest_addr = *addr;
        l->permanent = permanent;
        l->broadcast = pbroad;
        l->idb = idb;
        l->link = link;
        GET_TIMESTAMP(l->time);
        bcopy(addr2, &l->hardware, SMDSBYTES);
        hash = xsnethash(addr);
        l->next = smdstable[hash];
        smdstable[hash] = l;
    }
    return(l);
}


/*
 * smds_addr_lookup
 */

smdsmaptype *smds_addr_lookup (addrtype *addr, ulong link)
{
    int hash;
    smdsmaptype *l;
    
    hash = xsnethash(addr);
    l = smdstable[hash];
    while (l) {
        if (l->link == link)
          if (bcmp((uchar *)&l->dest_addr.addr, (uchar *)&addr->addr,
           l->dest_addr.length) == 0) 
            return(l);
        l = l->next;
    }
    return(NULL);
}

/*
 * smds_addr_lookup_wrap
 * Wrapper routine for registry interface to SMDS static mappings.
 */

static void smds_addr_lookup_wrap (addrtype *addr, ulong link, void *map)
{
    smdsmaptype *smdsmap;

    smdsmap = smds_addr_lookup(addr, link);
    if (smdsmap)
	*((smdsmaptype **)map) = smdsmap;
}

/*
 * smds_parse_bcast
 * Parse a broadcast table command
 */

static void smds_parse_bcast (parseinfo *csb, idbtype *idb)
{
    ulong link;
    uchar addr2[SMDSBYTES];
    ipaddrtype     ipnet, ipmask, ipnetmask;
    
    link = GETOBJ(int,10);
    bcopy(GETOBJ(hwaddr,1)->addr, addr2, SMDSBYTES);
    /*
     * CSCdi28764 - Fixed to allow E.164 UNI & MULTICAST addresses.
     * 	Error out if the address is neither of these.
     */
    if (!SMDS_ISGROUP(addr2[0]) && !SMDS_ISINDIV(addr2[0])) {
        printf("\n%%SMDS address is invalid!");
        return;
    }
    ipnet = csb->addr1.ip_addr;
    ipmask = csb->addr2.ip_addr;
    if (!ipnet && !ipmask) {
	ipnet = idb->ip_unnumbered ? idb->ip_unnumbered->ip_address :
	  idb->ip_address;
	ipmask = idb->ip_unnumbered ? idb->ip_unnumbered->ip_address :
	  idb->ip_nets_mask;
    }
    if (!smds_bcast_valid(idb, addr2)) {
	return;
    }
    ipnetmask = ipnet & ipmask;
    /*
     * Get ipnet & ipmask values for IP & ARP declarations
     */
    if (!csb->sense) {
        if (smds_bcast_lookup(link, idb, ipnetmask, FALSE)) {
            smds_bcast_delete(link, idb, ipnetmask);
        } else {
          printf("\n%%Can't find address map for %E", &addr2);
        }
    } else {
        if (smds_bcast_lookup(link, idb, ipnetmask, FALSE)) {
            smds_bcast_delete(link, idb, ipnetmask);
        }
	if ((link == LINK_IP || link == LINK_ARP) && (!ipnet || !ipmask))
	    printf("\n%%Error entering multicast address");
	else
            if (smds_bcast_enter(addr2, idb, link, ipnet, ipmask) == NULL)
                printf("\n%%Error entering multicast address");
    }
}

/*
 * smds_bcast_enter
 */

smdsaddr *smds_bcast_enter (
    uchar *addr2,
    idbtype *idb,
    uint link,
    ipaddrtype ipsub,
    ipaddrtype ipmask)
{
    smds_pdb_t *spd = idb->hwptr->smds_pdb;
    smdsaddr *bca, *last;
    queuetype *head;
    
    if ((link == LINK_IP || link == LINK_ARP) && !ipsub && !idb->ip_enabled) 
	return(NULL);
    bca = malloc(sizeof(smdsaddr));
    if (bca == NULL) 
        return(NULL);
    bca->link = link;
    bca->idb = idb;
    bcopy(addr2, (uchar *)&bca->addr, SMDSBYTES);
    bca->next = NULL;
    last = (smdsaddr *)&spd->smds_bcast;
    while (last->next) 
        last = last->next;
    last->next = bca;

    /* only one bridge multicast address for a if/subif */
    if (link == LINK_BRIDGE) {
       head = &idb->tbridge_circuitQ;
       head->qhead = bca;
       head->qtail = bca;
    }

    if (link == LINK_IP || link == LINK_ARP) {
	if (!ipsub) {
            bca->ipsubnet = (idb->ip_address) ? idb->ip_address : 
	      idb->ip_unnumbered->ip_address;;
            bca->ipmask =(idb->ip_address) ? idb->ip_nets_mask :
	      idb->ip_unnumbered->ip_nets_mask;;
	    bca->ipsubnet &= bca->ipmask;
	} else {
            bca->ipsubnet = ipsub;
            bca->ipmask =ipmask;
	    bca->ipsubnet &= bca->ipmask;
	}
    }
    return(bca);
}


/*
 * smds_broadcast_lookup
 */

uchar *smds_bcast_lookup (
    ulong link,
    idbtype *idb,
    ipaddrtype destip,
    boolean ipflag)
{
    smds_pdb_t *spd = idb->hwptr->smds_pdb;
    smdsaddr *bca;
    
check_again:
    for (bca = spd->smds_bcast; bca; bca = bca->next) {
        if (bca->link == link && idb == bca->idb) {
            if (link != LINK_IP && link != LINK_ARP)
                return(bca->addr);
	    /*
	     * Do special checking for IP/ARP to find the proper multicast
	     */
	    if (reg_invoke_ip_ifbroadcast(destip, bca->ipsubnet,bca->ipmask) ||
		((destip & bca->ipmask) == bca->ipsubnet))
		return(bca->addr);
        }
    }
    if ((link == LINK_ARP) && ipflag) {    /* Look at the IP side now. */
        link = LINK_IP;
        goto check_again;
    }
    return(NULL);
}

/*
 * smds_bcast_delete
 */

void smds_bcast_delete (
    ulong link,
    idbtype *idb,
    ipaddrtype ipnet)
{
    smds_pdb_t *spd = idb->hwptr->smds_pdb;
    smdsaddr *last, *addr;
    boolean found;

    last = (smdsaddr *) (&spd->smds_bcast);
    addr = spd->smds_bcast;
    /*
     * Find and delink the address entry from the address hash table.
     */
    while (addr) {
	found = FALSE;
        if (addr->link == link && addr->idb == idb) {
	    if (link == LINK_IP) {
		if (ipnet && (addr->ipsubnet == ipnet))
		    found = TRUE;
	    } else {
		found = TRUE;
	    }
	} 

	if (found) {
            if (last == addr)
              spd->smds_bcast = NULL;
            else
              last->next = addr->next;
            free(addr);
	    if (link == LINK_BRIDGE) {
		queue_init(&idb->tbridge_circuitQ, 0);
	    }
	    return;
        }

        last = addr;
        addr = addr->next;
    }
}

/*
 * smds_bcast_valid:	Check to see if other (sub)interfaces already
 *	have this E.164 MULTICAST address configured.
 */
boolean smds_bcast_valid(idbtype *idb, uchar *address)
{
    smdsaddr *bca;
    hwidbtype *hwidb;
    
    FOR_ALL_HWIDBS(hwidb) {
    	smds_pdb_t *spd = hwidb->smds_pdb;
	if(!spd) {
	    continue;
	}
    	for (bca = spd->smds_bcast; bca; bca = bca->next) {
	    if (!bcmp(address, bca->addr, STATIONLEN_SMDS64)) {
		if ((bca->idb->subif_state != SUBIF_STATE_DELETED) &&
		    (bca->idb->hwptr == idb->hwptr) && (bca->idb != idb)) {
		    printf("\n%%Multicast address %E is used at %s", 
			address, bca->idb->namestring);
		    return(FALSE);
	    	}
	    }
	}
    }
    return(TRUE);
}


/*
 * smds_map_delete
 */

void smds_map_delete (smdsmaptype *hp)
{
    uint hash;
    smdsmaptype **map;

    /*
     * Find and delink the address map from the address hash table.
     */
    hash = xsnethash(&hp->dest_addr);
    map = &smdstable[hash];
    while (*map) {
        if (*map == hp) {
            *map = hp->next;
            break;
        }
        map = &((*map)->next);
    }        
    free(hp);
}

/*
 *  smds_delete_map_on_swidb ()
 *     . remove all map entries on a swidb 
 *
 */

static void smds_delete_map_on_swidb(idbtype *idb)
{
    int i;
    smdsmaptype **pmap, *free_map = NULL;

    for (i=0; i < SMDSHASH; i++) {
        pmap = &smdstable[i];
        while (*pmap) {
            if ((*pmap)->idb == idb) {
                free_map = *pmap;
                *pmap = (*pmap)->next;
                free(free_map);
            }
            else {
                pmap = &((*pmap)->next);
            }
        }
    }
}


/*
 *   smds_delete_subif ()
 *       . called when "no int [i/f].x" command is executed to 
 *         remove a subinterface
 *       . deletes the smds address, the maps, and the multicast 
 *         address configured on the subinterface  
 */

void smds_delete_subif (idbtype *swidb)
{
    smdsaddr **maddr = NULL, *free_maddr = NULL;
    hwidbtype *hwidb = swidb->hwptr;
    smdsmaptype *map = NULL;
    int i;

    if (!is_smds(hwidb)) {
         return;
    }

    /* stop all timers for dynamic maps        */
    for (i=0; i < SMDSHASH; i++) {
        map = smdstable[i];
        while (map) {
            if ((map->idb == swidb)  && (!map->permanent)) {
                mgd_timer_stop(&map->timer);
            }
            map = map->next;
        }
    }

    smds_delete_map_on_swidb(swidb);
    maddr = &( hwidb->smds_pdb->smds_bcast);
    while (*maddr) {
        if ((*maddr)->idb == swidb) {
            free_maddr = *maddr;
            *maddr = (*maddr)->next;
            free(free_maddr);
        } else {
            maddr = &((*maddr)->next);
        }
    } 

    if (swidb->vaddr) {
        free(swidb->vaddr);
        swidb->vaddr = NULL;
    }

    swidb->arp_smds = FALSE;

    if (swidb->smds_glean) {
        free(swidb->smds_glean);
        swidb->smds_glean = NULL;
    }
    reg_invoke_bump_cache_version(hwidb, NULL, TRUE);
    reg_invoke_ar_cache_clear(swidb);

}


/*
 * smds_linestate
 * Return current line protocol status
 */

static boolean smds_linestate (hwidbtype *hwidb)
{
    return(hwidb->smds_lineup);
}

/*
 * smds_extract_hwaddr
 */
static boolean smds_extract_hwaddr (hwidbtype *hwidb, hwaddrtype *hwaddr)
{
    memset(hwaddr, 0, sizeof(hwaddrtype));
    hwaddr->type = STATION_SMDS64;
    hwaddr->length = STATIONLEN_SMDS64;
    hwaddr->target_type = TARGET_UNICAST;
    bcopy(hwidb->hardware, hwaddr->addr, STATIONLEN_SMDS64);
    return(TRUE);
}

/*
 * smds_extract_addr
 *
 * Given a SMDS packet, extract the requested 'MAC' level address
 * into a hwaddrtype structure.
 */
static boolean smds_extract_addr (
    paktype *pak,
    hwaddrtype *address,
    enum SRCDST which)
{
    smdstype *smh;

    smh = (smdstype *)pak->datagramstart;
    address->type = STATION_SMDS64;
    address->length = STATIONLEN_SMDS64;
    address->target_type = TARGET_UNICAST;
    switch (which) {
      case SOURCE:
	bcopy(smh->smdssrc, address->addr, STATIONLEN_SMDS64);
	break;
      case DESTINATION:
	bcopy(smh->smdsdest, address->addr, STATIONLEN_SMDS64);
	break;
    }

    if (is_ieee_bcast(address->addr))
	address->target_type = TARGET_BROADCAST;
    else if (ieee_equal(esis_8023_all_is_multi, address->addr))
	address->target_type = TARGET_OSI_ALL_IS;
    else if (ieee_equal(esis_8023_all_es_multi, address->addr))
	address->target_type = TARGET_OSI_ALL_ES;

    return(TRUE);
}

/*
 * smds_parse_packet
 *
 */
static iqueue_t smds_parse_packet (
    hwidbtype *idb,
    paktype *pak,
    uchar *data)
{
    smdstype *smds;
    snap_hdr *snap;
    ether_hdr *ether;
    charlong firstlong;

    /*
     * Grab the encapsulation type, and figure out the encapsulation size.
     */
    pak->enctype = ET_SMDS;

    /*
     * Check to see if building a frame in a virgin packet, or whether
     * we're just setting up media pointers into the frame
     */
    if (data) {
	/*
	 * We've got a virgin packet. 
	 */
	firstlong.d.lword = GETLONG(data);
    } else {
	/*
	 * Soiled packet. Start from datagramstart
	 */
	smds = (smdstype *)(pak->datagramstart +
			    (idb->smds_pdb->dximode ?
			    SMDS_DXIBYTES : SMDS_CISCOBYTES));
	snap = (snap_hdr *)smds->data;
	firstlong.d.lword = GETLONG(&snap->oui[1]);
    }

    if (firstlong.d.sword[0] == SMDS_BRIDGE_OUI) {
	/*
	 * If we're building a virgin frame, we have to set datagramstart
	 */
	if (data) {
	    pak->datagramstart = pak_center(pak) - idb->encsize -
		ETHER_ARPA_ENCAPBYTES - SMDS_BRIDGE_PAD_SIZE;
	}
	smds = (smdstype *)(pak->datagramstart + 
			    idb->smds_pdb->fast_input_modelen);
	snap = (snap_hdr *)smds->data;
	ether = (ether_hdr *)(snap->data + SMDS_BRIDGE_PAD_SIZE);
	pak->mac_start     = (uchar *)ether;
	pak->addr_start    = ether->daddr;
	pak->rif_start     = NULL;
	pak->riflen	   = 0;
    	pak->info_start    = (uchar *)snap;
	pak->network_start = ether->data;

    } else {
	/*
	 * If we're building a virgin frame, we have to set datagramstart
	 */
	if (data) {
	    pak->datagramstart = pak_center(pak) - idb->encsize;
	}
	smds = (smdstype *)(pak->datagramstart + 
			    idb->smds_pdb->fast_input_modelen);
	snap = (snap_hdr *)smds->data;
	pak->mac_start     = (uchar *)smds;
	pak->addr_start    = smds->smdsdest;
	pak->rif_start     = NULL;
	pak->riflen	   = 0;
	pak->info_start    = (uchar *)snap;

	pak->network_start = snap->data;
    }

    /*
     * Record just how big this encapsulation was rather than every protocol
     * have it's own fun and wacky way to work it out.
     */
    pak->encsize = pak->network_start - pak->datagramstart;

    return(smds_input);
}

/* smds_can_mcast
 * Return TRUE if interface supports and is configured with any sort of
 * physical multicast capability for the passed in linktype.
 */

static boolean smds_can_mcast (hwidbtype *hwidb, enum LINK link)
{
    smdsaddr *bca;
    
    for (bca = hwidb->smds_pdb->smds_bcast;
	 bca; bca = bca->next) {
        if (bca->link == link)
	    return(TRUE);
    }
    return(FALSE);
}

/*
 * smds_setup
 * Initialize smds encapsulation
 */

static boolean smds_setup (parseinfo *csb, hwidbtype *idb)
{

    int smds_maxdgram, smds_buffersize;
    smds_pdb_t    *spd;
    idbtype *swidb;
    ushort temp;

    if (!supports_maxdgram(idb))
        return(FALSE);

    spd = malloc(sizeof(smds_pdb_t));
    if (!spd)
	return(FALSE);

    smds_buffersize = idb->maxmtu + SMDS_ENCAPBYTES2 + ETHER_BAGGAGE;
    smds_maxdgram = smds_buffersize + HDLC_HEADER_SIZE;

    idb->smds_pdb = spd;

    /*
     * Set the fastswitch flavor
     */
    delete_fair_queue(idb);

    idb->fast_switch_type = FS_SMDS;

    idb->enctype = ET_SMDS;
    idb->encsize = SMDS_DXIBYTES + SMDS_SNAP_ENCAPBYTES;
    idb->span_encapsize = idb->encsize + SMDS_BRIDGE_PAD_SIZE;
    temp = reg_invoke_media_smds_encap_after_line(idb->type,idb);
    smds_buffersize += temp;
    smds_maxdgram += temp;
    if (!(*idb->set_maxdgram)(idb, smds_buffersize, smds_maxdgram))
        return(FALSE);
    idb->vencap = smds_vencapsulate;
    idb->bridge_vencap = smds_bridge_datagram;
    idb->deencap = smds_deencap;
    idb->getlink = smds_getlink;
    set_default_queueing(idb);
    idb->iqueue = smds_input;
    idb->parse_packet = smds_parse_packet;
    idb->periodic = smds_periodic;
    idb->extract_hwaddr = smds_extract_hwaddr;
    idb->extract_snpa = smds_extract_addr;
    idb->broadcast = smds_is_bcast;
    idb->can_mcast = smds_can_mcast;
    idb->show_support = smds_interface;
    idb->clear_support = clear_smds_counters;
    idb->lineaction = smds_cstate;
    idb->linestate = smds_linestate;
    idb->smds_lineup = TRUE;
    idb->smds_begin_tag = 1;
    spd->dximode = TRUE;
    spd->smds_heartbeat = FALSE;
    spd->necmode = FALSE;
    spd->smds_bcast = NULL;
    spd->fast_input_modelen = SMDS_DXIBYTES;
    spd->fast_output_modelen = SMDS_DXIBYTES;
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	queue_init(&swidb->tbridge_circuitQ, 0);
	swidb->ip_mtu = idb->maxmtu;
	swidb->arp_smds = FALSE;
	swidb->arp_timeout = ARP_MAXAGE;
	reg_invoke_media_enctype_setup(swidb, ET_SMDS);
	swidb->ip_enctype = ET_SMDS;
	swidb->tbridge_media = TBR_SMDS_TRANSIT;
	if (swidb->tbridge_on_swidb) {
	    idb->smds_bridge = TRUE;
	    /*
	     * To disable fast path bridging, add back the next line
	     * idb->span_bridge_process_force |= SPAN_BR_PROC_SMDS_BRIDGE;
	     */
	}
    }

    (*(idb->reset))(idb);
    return(TRUE);
}

/*
 * smds_write_map
 * Write out a single map entry
 */

void smds_write_map (idbtype *idb, smdsmaptype *addr)
{
    char addrstr[64];
    addrtype paddr = addr->dest_addr;

    if (idb->subif_state == SUBIF_STATE_DELETED) {
	return;
    }
    print_proto_addr(idb, &paddr, (uint)addr->link, addrstr);
    nv_add(TRUE, " %s", addrstr);
    nv_add(TRUE, " %E", addr->hardware);
    nv_add(addr->broadcast, " broadcast");
}

/*
 * nv_smds_bcast
 * Scan hash table for map commands, then write them out
 */

void nv_smds_bcast (idbtype *idb, char *str)
{
    smds_pdb_t *spd = idb->hwptr->smds_pdb;
    smdsaddr *b;

    for (b = spd->smds_bcast; b; b = b->next) {
	if (b->idb->subif_state != SUBIF_STATE_DELETED && b->idb == idb) {
            nv_write(TRUE, "%s %s %E", str, link2name(b->link), b->addr);
            if (b->link == LINK_IP || b->link == LINK_ARP) {
            	nv_add(TRUE, " %i %i ", b->ipsubnet ,b->ipmask);
            }
	}
    }
}

/*
 * smds_hash_init
 * Initialize the hash table
 */

void smds_hash_init (void)
{
    int i;
    
    for (i = 0; i < SMDSHASH; i++)
        smdstable[i] = NULL;
}

/*
 *  smds_no_encap_after_line
 */
static ushort smds_no_encap_after_line (hwidbtype *idb)
{
    return(0);
}

/*
 * smds_glean_source_address
 * Glean the source address from the packet
 */
static void  smds_glean_source_address (paktype *pak)
{
    hwaddrtype hwaddr; 
    addrtype saddr;
    smdsmaptype *map = NULL;
    idbtype *swidb = NULL;
    smds_gleantype *gleaninfo = NULL;

    swidb = pak->if_input;
    gleaninfo = swidb->smds_glean;

    if (!gleaninfo)
	return;

    if ( smds_extract_addr (pak, &hwaddr, SOURCE) != TRUE) {
	if (serial_debug) 
	    buginf("\nSMDS: Error gleaning source address map."); 
        return;
    }

    reg_invoke_build_packet_address(pak->linktype, pak,
		              &saddr, SOURCE);

    /* check to see if the source address is already in the  */
    /* map table                                             */
      
    map = smds_addr_lookup( &saddr, pak->linktype);
    if (map == NULL) {
	map = smds_addr_enter( &saddr, FALSE, hwaddr.addr,   
                               swidb,  pak->linktype,
                               gleaninfo->bcast ); 
        if (!map) {
	    if (serial_debug)
                buginf("\nSMDS: Error adding gleaned address map.");
	    return;
        }

	/* add a map timer for this map entry */
        mgd_timer_init_leaf(&map->timer, &master, MAP, map, FALSE);  
        mgd_timer_start(&map->timer, gleaninfo->timeout );
    } else {
        /* if this is a static map do nothing */
        if (map->permanent) {
            return;
        } else {
            bcopy(hwaddr.addr, map->hardware, SMDSBYTES);

	    /* update the broadcast field, it may have changed */
            map->broadcast = gleaninfo->bcast;

	    /* update the map timer for this map entry */
            mgd_timer_stop(&map->timer);
            mgd_timer_start(&map->timer, gleaninfo->timeout );
        }
    }
}

/*
 * smds_encaps_show_protocol_attr
 *
 */
static void smds_encaps_show_protocol_attr (idbtype *swidb)
{
    printf("\n  SMDS hardware address is ");
    if (swidb->vaddr && swidb->vaddr->addr != NULL) {
        printf("%E", swidb->vaddr->addr);
    } else {
        printf("not set.");
    }
    printf("\n  Encapsulation SMDS");
}

/*
 * smds_onemin_tasks
 * Perform once-a-minute SMDS tasks.
 *
 * Right now, these are:
 *  1) Age 'dynamic' entries in the table
 */
static void smds_onemin_tasks (void)
{
}

/*
 * smds_sw_idb_encaps_init
 * Initialize smds related data structures in the swidb
 */
static void smds_sw_idb_encaps_init ( idbtype *swidb, boolean setup_hwidb)
{

     swidb->smds_glean = NULL;   /* ensure this field is set to NULL */
}

/*
 * smds_init
 */

static void smds_init (subsystype *subsys)
{
    smds_hash_init();
    smds_parser_init();

    reg_add_media_map_lookup(ET_SMDS, smds_addr_lookup_wrap,
			     "smds_addr_lookup_wrap");
    reg_add_media_serial_setup(ET_SMDS, smds_setup, "smds_setup");
    reg_add_default_media_smds_encap_after_line(
	 smds_no_encap_after_line, "smds_no_encap_after_line");
    reg_add_swif_erase(smds_delete_subif, "smds_delete_subif");
    reg_add_media_glean_source_address(ET_SMDS, smds_glean_source_address,
				       "smds_glean_source_address");
    reg_add_onemin(smds_onemin_tasks, "smds_onemin_tasks");
    mgd_timer_init_parent(&master, NULL);
    reg_add_encaps_show_protocol_attr(ET_SMDS,
				      smds_encaps_show_protocol_attr,
                                      "sdms_encaps_show_protocol_attr");
    reg_add_sw_idb_encaps_init(ET_SMDS, smds_sw_idb_encaps_init,
                                      "smds_sw_idb_encaps_init");
    reg_add_media_addr_to_swidb(ET_SMDS, smds_addr_to_swidb,
				"smds_add_to_swidb");
}

/*
 * show_smds
 */
void show_smds (parseinfo *csb)
{
    switch (csb->which) {
      case SHOW_SMDS_ADDRESSES:
        display_smds();
        break;
      case SHOW_SMDS_MAP:
        show_smds_map();
        break;
      case SHOW_SMDS_TRAFFIC:
        show_smds_counters();
        break;
      default:
        bad_parser_subcommand(csb, csb->which);
        break;
    }
}

/*
 * show_smds_counters
 */

void show_smds_counters (void)
{

    printf("\n %u Input packets", smds_counters[SMDS_INPUT]);
    printf("\n %u Output packets", smds_counters[SMDS_OUTPUT]);
    printf("\n %u DXI heartbeat sent", smds_counters[SMDS_DXI_CMDTODSU]);
    printf("\n %u DXI heartbeat received", smds_counters[SMDS_DXI_RSPFRDSU]);
    printf("\n %u DXI DSU polls received", smds_counters[SMDS_DXI_CMDFRDSU]);
    printf("\n %u DXI DSU polls sent", smds_counters[SMDS_DXI_RSPTODSU]);
    printf("\n %u Invalid DXI frames", smds_counters[SMDS_DXI_BADFRAME]);
    printf("\n %u Bad BA size errors", smds_counters[SMDS_BAD_BASIZE]);
    printf("\n %u Bad Header extension errors", smds_counters[SMDS_BAD_XHEL]);
    printf("\n %u Invalid address errors", smds_counters[SMDS_INVLD_ADDR]);
    printf("\n %u Bad tag errors", smds_counters[SMDS_BAD_BETAG]);

}


/*
 * show_smds_map
 */

void show_smds_map (void)
{
    smdsmaptype *p;
    hwidbtype *hwidb;
    int i;
    smds_pdb_t *spd;
    idbtype *idb;
    
    FOR_ALL_HWIDBS(hwidb) {
        spd = hwidb->smds_pdb;
	FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	    if (idb->subif_state == SUBIF_STATE_DELETED) {
		continue;
	    }
    	    for (i = 0; i < SMDSHASH; i++) {
                for (p = smdstable[i]; p; p = p->next)
                    if (p && p->idb == idb)
                        smds_display_map(p);
	    }

            if (spd && spd->smds_bcast)
                smds_display_bcast(idb);
    	}
    }
}


/*
 * smds_display_map
 */

void smds_display_map (smdsmaptype *p)
{
    char buf[64];

    printf("\n%s:  ", p->idb->namestring);

    print_proto_addr(p->idb, &p->dest_addr, p->link, buf);
    printf("%s ", buf);
    printf("maps to %E", p->hardware);
    if (p->permanent) {
        printf(" static");
    } else {
	printf("\n                                 --   dynamic, TTL: %d min ",
			       mgd_timer_left_sleeping(&p->timer) / ONEMIN);
    }
    if (p->broadcast)
        printf(",broadcast");
}


/*
 * smds_display_bcast
 */

void smds_display_bcast (idbtype *idb)
{
    smdsaddr *p;
    smds_pdb_t *spd = idb->hwptr->smds_pdb;
    
    for (p = spd->smds_bcast; p; p = p->next) {
	if (idb->subif_state != SUBIF_STATE_DELETED && p->idb == idb) {
            printf("\n%s:  %s ", idb->namestring, link2name(p->link));
	    if ((p->link == LINK_IP) || (p->link == LINK_ARP))
                printf("%i %i ", p->ipsubnet, p->ipmask);
            printf("maps to %E multicast", p->addr);
	}
    }
}

/*
 * smds_cstate
 */

void smds_cstate (hwidbtype *idb)
{
    smds_pdb_t * spd = idb->smds_pdb;

    if (idb->state != IDBS_UP) {
        idb->smds_lineup = FALSE;
        spd->smds_myseq = 0;
        spd->smds_lastseen = 0;
    }
    if (idb->state == IDBS_UP) {
	/*
	 * Line goes up, set smds_lineup to TRUE and restart heartbeat counters
	 */
	idb->smds_lineup = TRUE;
	spd->smds_myseq = 0;
	spd->smds_lastseen = 0;
    }
}


/*
 * smds_parse_proto_addr
 */
static boolean smds_parse_proto_addr (parseinfo *csb, uint flag,
			       addrtype *addr, uint link)
{
    addrtype *paddr = GETOBJ(paddr, 1);
    return(parse_proto_addr(csb, flag, addr, paddr, link));
}

/*
 * smds_is_bcast
 * Return FUZ_TRUE if multicast packet, FUZ_FALSE otherwise
 */

static fuzzy
smds_is_bcast (hwidbtype *hwidb, paktype *pak)
{
    smdstype *header;

    header = (smdstype *) pak->datagramstart;
    return(SMDS_ISGROUP(header->smdsdest[0]) ? FUZ_TRUE : FUZ_FALSE);
}

void display_smds (void)
{
    idbtype *idb;
    hwidbtype *hwidb;
    smds_pdb_t *spd;

    FOR_ALL_HWIDBS(hwidb) {
	if (is_smds(hwidb)) {
            spd = hwidb->smds_pdb;
	    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
            	if (spd && idb->vaddr)
            	    printf("\nSMDS address - %s   %E",idb->namestring, 
	      	      idb->vaddr->addr);
	    }
	}
    }
    
}       


/*
 * xsnethash
 */

uint xsnethash (addrtype *addr)
{
    register uint i, sum, len = addr->length;
    uchar *adr = (uchar *)&addr->addr;

    sum = 0;
    for (i = 0; i < len; i++)
        sum += adr[i];

    sum &= (SMDSHASH-1);
    return sum;
}

/*
 * smds_deencap() -
 * Called when switching encapsulations away from SMDS.
 *  Must clean out all addresses, address mappings & special modes.
 */
void smds_deencap (hwidbtype *idb)
{
    idbtype *swidb;
    smds_pdb_t *spd = idb->smds_pdb;
    smdsaddr *nsm, *saddr = spd->smds_bcast;

    mgd_timer_stop(&master);          /* stop all map timers */
    idb->smds_lineup = FALSE;
    idb->vencap = NULL;
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	swidb->ip_split_disabled = swidb->ip_split_disabled_def = FALSE;
	swidb->arp_smds = FALSE;
        if (swidb->vaddr) {
            free(swidb->vaddr);
            swidb->vaddr = NULL;
        }
        /* delete the maps configured on the interface */
        smds_delete_map_on_swidb(swidb);

        if (swidb->smds_glean) {
            free(swidb->smds_glean);
            swidb->smds_glean = NULL;
        }
    }

    /* SMDS f/s scribbles over fst_rcvidb vector - restore it */
    idb->fast_rcvidb = idb->firstsw;

    while (saddr) {
        nsm = saddr->next;
        free(saddr);
        saddr = nsm;
    }

    idb->enctype = idb->enctype_def;    /* Set it to DEFAULT enctype */
    idb->encsize = 0;
    free(spd);				/* Throw away spd */
    idb->smds_pdb = NULL;
    idb->can_mcast = NULL;
    ieee_copy(default_mac_addr, idb->hardware);
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	queue_init(&swidb->tbridge_circuitQ, 0);
	if (swidb->tbridge_on_swidb) {
	    idb->smds_bridge = FALSE;
	    idb->span_bridge_process_force &= ~SPAN_BR_PROC_SMDS_BRIDGE;
	}
    }
}


/*
 * smdsequal
 * Given two ethernet 48 bit addresses, return equality predicate.
 *
 * This will also have to change if all 60 bits of address are used.
 */

boolean smdsequal (register uchar *addr1, register uchar *addr2)
{
    if ((addr1 == NULL) || (addr2 == NULL))
        return(FALSE);
    if (bcmp(addr1, addr2, STATIONLEN_SMDS64))
	return(FALSE);
    return(TRUE);
}

/*
 * smds_interface
 *
 * Support for the "show interface" command.
 */
void smds_interface (
    hwidbtype *hwidb)
{
    smds_pdb_t *spd = hwidb->smds_pdb;

    printf("\n  Mode(s):  D15 compatibility");
    if (spd->dximode) {
        printf(", DXI 3.2");
	if (spd->smds_heartbeat) {
	    printf("\n  DXI heartbeat sent %u,  DXI heartbeat received %u",
		   spd->smds_dxihbsent, spd->smds_dxihbrcvd);
	}
    }
}

/*
 * clear_smds_counters
 *
 * Support for the "clear counters" command.
 */
void clear_smds_counters (hwidbtype *idb)
{
    smds_pdb_t *spd;

    if (is_smds(idb) && (spd = idb->smds_pdb)) {
	spd->smds_dxihbsent = 0;
	spd->smds_dxihbrcvd = 0;
    }
}

/*
 * smds_periodic
 *
 * Heartbeat process when DXI3.2 enabled.
 */
void smds_periodic (hwidbtype *idb)
{
    smds_pdb_t *spd;
    paktype *pak;
    dxiheartbeat *hb_pak;
    ulong sn_diff;
    boolean heartbeat;
    tinybool send_heartbeat_now;

    if ((idb->state != IDBS_UP) || (idb->enctype != ET_SMDS))
        return;

    while (mgd_timer_expired(&master))  {
        mgd_timer *expired_timer;
        smdsmaptype *map;

        expired_timer = mgd_timer_first_expired(&master);
        switch (mgd_timer_type(expired_timer)) {
            case MAP:
                map = mgd_timer_context(expired_timer);
                mgd_timer_stop(expired_timer);
                smds_map_delete(map);
		break;
            default:
                mgd_timer_stop(expired_timer);
		break;
        }
    }

    send_heartbeat_now = FALSE;
    spd = idb->smds_pdb;
    heartbeat = spd->smds_heartbeat;
    spd->smds_heartbeat = !idb->nokeepalive && spd->smds_enabled && spd->dximode;
    if (!spd->smds_heartbeat) {
	/*
	 * If heartbeat disabled return
	 */
	if(heartbeat && !idb->smds_lineup) {
            /*
             * Turn off heartbeat : reset line state if necessary
             */
            idb->smds_lineup = TRUE;
            idb->reason = "Heartbeat disabled";
	    reg_invoke_ifmib_link_trap(LINK_UP_TRAP, idb);
	}
	return;
    } else if (!heartbeat) {
	/*
	 * Turn on heartbeat : initialize data structure
	 */
	spd->smds_myseq = 0;
	spd->smds_lastseen = 0;
	GET_TIMESTAMP(spd->smds_timesent);
	send_heartbeat_now = TRUE;
    }

    /*
     * Verifies the keepalive period is still in the valid range as it
     * may have changed with the "keepalive" command
     */
    if (idb->keep_period < DXI_MIN_BEAT) {
	idb->keep_period = DXI_MIN_BEAT;
	idb->keep_count = DXI_MIN_BEAT;
    }
    else if (idb->keep_period > DXI_MAX_BEAT) {
	idb->keep_period = DXI_MAX_BEAT;
	idb->keep_count = DXI_MAX_BEAT;
    }

    /*
     * Verifies that we comply with the minimum heartbeat timer (which is
     * equal to the No Acknoledgement Timer)
     */
    if (!send_heartbeat_now  &&
	(ELAPSED_TIME(spd->smds_timesent) < DXI_NO_ACK_TMR)) {
	return;
    }

    /*
     * Compare last received sequence number to last sent one
     */
    sn_diff= spd->smds_myseq - spd->smds_lastseen;
    if (!idb->smds_lineup && !sn_diff) {
	/*
	 * Line down and valid Test frame exchange: change state to up
	 */
	idb->smds_lineup = TRUE;
	idb->reason = "Heartbeat OK";
	reg_invoke_ifmib_link_trap(LINK_UP_TRAP, idb);
    } else if (idb->smds_lineup && (sn_diff >= 3)) {
        /*
         * Line up and >=3 invalid Test frame exchanges: change state to down
         */
	idb->smds_lineup = FALSE;
        idb->reason = "Heartbeat failed";
	reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
    }
    if (serial_debug) {
	buginf("\n%s: SMDS DXI myseq %u, last received %u, line %s",
		idb->hw_namestring, spd->smds_myseq, spd->smds_lastseen,
		idb->smds_lineup ? "up" : "down");
    }

    /*
     * Now increment sequence number and send a Test Command frame
     */
    pak = getbuffer(DXI_HBEAT_SIZE);
    if (pak) {
	pak->if_output = idb->firstsw;
	pak->linktype = LINK_ADDRESS;
	pak->datagramsize = DXI_HBEAT_SIZE; 
	pak->datagramstart = pak_center(pak) - SMDS_DXIBYTES;
	pak->flags |= PAK_PRIORITY; /* Put at head of queue */
	memset(pak->datagramstart, 0, DXI_HBEAT_SIZE);
	hb_pak = (dxiheartbeat *) pak->datagramstart;
	hb_pak->dxi_header.addr = DXI_LM | DXI_CMDTODSU;
	hb_pak->dxi_header.control = DXI_TEST | DXI_PF;
	/* hb_pak->dxi_header.congest = 0;		already zeroed */
	hb_pak->dxi_hbseq = ++(spd->smds_myseq);
        show_smds_packet(idb->firstsw, pak, 1);
	if (idb->board_encap) {
	    if (!(*idb->board_encap)(pak,idb)) {
		retbuffer(pak);
		return;
	    }
	}
	GET_TIMESTAMP(spd->smds_timesent);
	datagram_out(pak);
        smds_counters[SMDS_OUTPUT]++;
        smds_counters[SMDS_DXI_CMDTODSU]++;
	spd->smds_dxihbsent++;
    }
}

/*
 * smds_lm_frame
 *
 * Handles Link Management frames from the SDSU.
 * For now only heartbeat frames are processed.
 */
void smds_lm_frame (idbtype *idb, paktype *pak)
{
    dxiheartbeat *hb_pak = (dxiheartbeat *)pak->datagramstart;
    uchar test_fr_src = hb_pak->dxi_header.addr & DXI_S_CR_AE;
    hwidbtype *hwidb = idb->hwptr;
    smds_pdb_t *spd = hwidb->smds_pdb;

    /*
     * If not Test frame: discard packet
     */
    if ((hb_pak->dxi_header.control & DXI_PF_MASK) != DXI_TEST)
    {
        smds_counters[SMDS_DXI_BADFRAME]++;
        retbuffer(pak);
        return;
    }

    if (test_fr_src == DXI_CMDFRDSU)
    {
	/*
	 * If Test Command from SDSU: echo back to SDSU
	 */
	hb_pak->dxi_header.addr = DXI_LM | DXI_RSPTODSU;
	pak->if_output = idb;
        pak->linktype = LINK_ADDRESS;
	pak->flags |= PAK_PRIORITY;
        show_smds_packet(idb, pak, 1);
	if (hwidb->board_encap) {
	    if (!(*hwidb->board_encap)(pak,hwidb)) {
		retbuffer(pak);
		return;
	    }
	}
	datagram_out(pak);
        smds_counters[SMDS_OUTPUT]++;
        smds_counters[SMDS_DXI_RSPTODSU]++;
        smds_counters[SMDS_DXI_CMDFRDSU]++;
    } else if (test_fr_src == DXI_RSPFRDSU) {
        /*
         * Else if Test Response from SDSU: store sequence number only if
	 * sequence numbers match
         */
        smds_counters[SMDS_DXI_RSPFRDSU]++;
	spd->smds_dxihbrcvd++;
	if (hb_pak->dxi_hbseq == spd->smds_myseq) {
	    spd->smds_lastseen = hb_pak->dxi_hbseq;
	}
        retbuffer(pak);
    } else {
        /*
         * Else bad Test frame
         */
        smds_counters[SMDS_DXI_BADFRAME]++;
	retbuffer(pak);
    }
}

/*
 * SMDS subsystem header
 */

#define SMDS_MAJVERSION 1
#define SMDS_MINVERSION 0
#define SMDS_EDITVERSION  1

SUBSYS_HEADER(smds, SMDS_MAJVERSION, SMDS_MINVERSION, SMDS_EDITVERSION,
	      smds_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      NULL);
      
