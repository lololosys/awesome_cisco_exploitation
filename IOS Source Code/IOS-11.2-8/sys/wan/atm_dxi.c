/* $Id: atm_dxi.c,v 3.10.12.9 1996/08/28 13:20:47 thille Exp $
 * $Source: /release/112/cvs/Xsys/wan/atm_dxi.c,v $
 *------------------------------------------------------------------
 * Support for ATM-DXI encapsulation and RFC1483 support
 *
 * October 1994, Shankar Natarajan
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atm_dxi.c,v $
 * Revision 3.10.12.9  1996/08/28  13:20:47  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.10.12.8  1996/08/13  23:22:49  rtio
 * CSCdi59352:  Show-frame-pvc does not accurately display the packets or
 * bytes
 * Branch: California_branch
 * Added code to put dlci/dfa in swidb->p2pmac for p2p subifs so ip mcast
 * cache can pass this number to ip_pak_write_mac_inline().
 *
 * Revision 3.10.12.7  1996/08/03  23:41:01  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.10.12.6  1996/07/23  13:30:55  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.10.12.5  1996/06/27  09:44:27  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.10.12.4  1996/05/09  14:48:59  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.10.12.3  1996/04/30  18:49:22  wmay
 * CSCdi54817:  Extra board encap added for ipx over frame-relay
 * Branch: California_branch
 *
 * Revision 3.10.12.2.16.1  1996/04/27  07:39:01  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.10.12.2.6.1  1996/04/08  02:43:12  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.10.12.2  1996/03/23  01:34:56  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.10.12.1  1996/03/18  22:46:49  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.10.16.1  1996/03/22  09:45:24  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.10  1996/02/13  04:22:02  shankar
 * CSCdi48025:  DXI ATM - DXI MAP DISSAPEARS AFTER RELOAD
 *
 * Revision 3.9.6.1  1996/03/05  05:37:56  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.9.2.1  1996/02/27  21:20:52  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.9  1996/02/07  16:17:03  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.8  1996/02/01  06:11:47  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.7  1996/01/29  07:31:03  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.6  1996/01/22  07:45:28  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5.2.1  1996/02/15  19:26:57  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.5  1996/01/05  04:57:25  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.4  1995/11/29  18:44:08  shankar
 * CSCdi41439:  ags crash during atm-dxi test. Part II.
 *
 * Revision 3.3  1995/11/17  18:00:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:48:58  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:47:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/23  22:52:08  ronnie
 * CSCdi38804:  InARP on ATM interfaces doesnt populate ARP table
 *         Many places were passing a complete addrtype to the hashing
 *         function, when only the first few bytes were set.  This made
 *         the hash function nondeterministic.
 *
 * Revision 2.2  1995/06/20  20:37:56  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  23:16:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "../srt/srt_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "ieee.h"
#include "../h/globs.h"
#include "../if/ether.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "serial.h"
#include "address.h"
#include "../wan/atm_dxi.h"
#include "../wan/atm_dxi_debug.h"
#include "serial_debug.h"
#include "logger.h"
#include "../srt/span.h"
#include "../if/arp.h"
#include "../if/network.h"
#include "../ui/parse_util.h"	
#include "parser.h"
#include "parser_defs_atm_dxi.h"
#include "frame_relay.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */

/*
 * Local Storage
 */
static dximaptype *dxitable[RELAYHASH];


/*
 * dxi_vencapsulate
 * dxi encapsulation assuming datagramstart & datagramsize initialized
 */
boolean dxi_vencapsulate (
    paktype *pak, 
    long dhost)
{
    if (pak->datagramstart == NULL)
	return(FALSE);
    return(dxi_encaps(pak, dhost, NULL));
}

/*
 * dxi_encaps
 * Determines protocol addr -> DFA mapping, handles replicated broadcasts,
 * and queues packet for output
 */
  
boolean dxi_encaps (
    paktype *pak,
    long address,
    dximaptype *recursive)
{
    dximaptype *map;
    register idbtype *idb;
    dxi_idb_struct_type *dxi_idb;
    addrtype proto_addr;
    boolean broadcast;
    uint linktype;
    

    broadcast = FALSE;
    idb = pak->if_output;
    dxi_idb =  idb->hwptr->atm_dxi_stuff;

    if (!interface_up(idb)) {
	if (atm_dxi_events_debug)
	    buginf("\n%s: encaps failed--line down", idb->namestring);
	return(FALSE);
    }

    if (!recursive) {
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
	if (!reg_invoke_media_frame_macaddr(pak->linktype, pak, address, 
					&proto_addr, &broadcast)) {
	    if (!reg_used_media_frame_macaddr(pak->linktype))
		errmsg(&msgsym(BADMACREG, LINK), idb->namestring,
		       pak->linktype);
	    return(FALSE);
	}
	
	if (broadcast) {
		map = dxi_broadcast(idb, pak, pak->linktype);
		if (map == NULL) {
		    if (atm_dxi_events_debug)
		      buginf("\n%s:encaps failed on broadcast for link %d(%s)",
			     idb->namestring, pak->linktype,
			     link2name(pak->linktype));
		    return(FALSE);
		}
	} else {
	    map = dxi_addr_lookup(idb, &proto_addr, pak->linktype);
	    if (map == NULL) {
	    	char buf[64];
		    if (atm_dxi_events_debug) {
			linktype = pak->linktype;
	    		print_proto_addr(idb, &proto_addr, linktype, buf);
		    	buginf("\n%s:Encaps failed-no map entry link %d(%s) %s",
			   idb->namestring, pak->linktype,
			   link2name(pak->linktype), buf);
		    }
		return(FALSE);
	    }
	}
    } else {
	map = recursive;
    }
    return(dxi_doencaps(idb, pak, map));
}

/*
 * dxi_doencaps
 * Fill in the packet headers. The default encapsulation is LLC/SNAP.
 * NLPID is supported for backward compatibility. Per-VC muxing as 
 * defined in RFC 1483 is also supported.
 */
boolean dxi_doencaps (
    idbtype *idb,
    paktype *pak,
    dximaptype *map)
{
    dxi_idb_struct_type *dxi_idb;
    dxi_pvc_info_type *dxi_pvc;
    atm_dxi_hdr *hdr;
    ushort num_dfa;

    num_dfa = map->dfa_num;
    dxi_idb =  idb->hwptr->atm_dxi_stuff;
    dxi_pvc = dxi_idb->dxi_dfa_tab[num_dfa];
    /*
     * check if the pvc is active or not
     */
    if (!dxi_pvc)
        return(FALSE);

    switch (dxi_pvc->encaps_type) {
	case DXI_MAP_SNAP:	
	    dxi_snap_encaps(pak, map->dfa);
	    break;
	case DXI_MAP_NLPID:
	    dxi_nlpid_encaps(pak, map->dfa);
	    break;
	case DXI_MAP_MUX:
	    pak->encsize = ATM_DXI_MUXENCAPBYTES;
	    pak->enctype = ET_ATM_DXI;
    	    pak->datagramstart -= (ATM_DXI_MUXENCAPBYTES);
	    pak->datagramsize += (ATM_DXI_MUXENCAPBYTES);
	    hdr = (atm_dxi_hdr *)pak->datagramstart;
	    PUTSHORT(&hdr->dfa, (map->dfa | ATM_DXI_EA_BITS));
	    if (atm_dxi_debug_packet)
	   	dxi_debug_pak(pak, "VC MUX header");
	    break;
	default:
	    if (atm_dxi_debug_packet)
	   	dxi_debug_pak(pak, "Output: Unknown Encaps on VC");
	    return(FALSE);
    }

    pak->mac_start = pak->datagramstart;
    if (idb->hwptr->board_encap) {
	if (!(*idb->hwptr->board_encap)(pak,idb->hwptr)) {
	    return(FALSE);
	}
    }

    if (!(pak->flags & PAK_ENCAPS_ONLY)) {
	reg_invoke_fs_cache_update(pak->linktype, pak, 0L);
	dxi_pvc->pvc_stats.num_output_pkts++;
	dxi_pvc->pvc_stats.num_bytes_send += pak->datagramsize;
    }
    	    
    return(TRUE);
}
/*
 * dxi_bridge1483_check
 * check packet and see if it's a 1483 bridge pdu. Note spanning
 * tree pdus are NOT classified as bridged packets internally and do not have
 * mac information encapsulated. While this is consistent with frame relay
 * implementation and more efficient use of bandwidth etc, it may want
 * to be reviewed, considered to encapsulate/decapsulate mac info for
 * spanning tree bpdus to be politically correct. Also, instead of this
 * implementation, may want sniff second word into sniff buffer so parse     
 * packet completely resolves alignment for frame relay AND atm dxi (as
 *  it really should). This should be done if process bridging ever needs
 * to speed up. With fast paths, may be non issue though. Also note to
 * this point spanning tree pdus classified as DXI NON ISO SNAP, and 
 * here no padding needs to be taken into account for network packet alignment
 * of spanning tree pdus.
*/

static void dxi_bridge1483_check (paktype *pak)
{
    atm_dxi_bridge_hdr *frame;
    snap_hdr *snap;
    ether_hdr *ether;   

    frame = (atm_dxi_bridge_hdr *)pak->datagramstart;
    snap = (snap_hdr *)((uchar *)frame->atm_dxi_bridge.snap_start);
    if (bcmp((uchar *)&snap->oui[0],tb1490_snap,SNAP_OUIBYTES) != 0)
        return;
    if (GETSHORT(&snap->type) == DXI_IEEE_DEC_BPDU) {
        return;
    }
    pak->enctype = ET_BRIDGE;   /* Mark as bridge encapsulation */
    pak->flags |= PAK_TBRIDGE_IN;
    ether              = (ether_hdr *)((uchar *)frame->bridge_data_start);
    pak->mac_start     = pak->datagramstart + ATM_DXI_BRIDGE_ENCAPBYTES;
    pak->addr_start    = ether->daddr;
    pak->rif_start     = NULL;
    pak->riflen        = 0;
    pak->info_start    = NULL;
    pak->network_start = ether->data;
    pak->encsize       = pak->mac_start - pak->datagramstart;
}
 
    

/*
 * dxi_input
 * Handle input packets with ATM-DXI format.
 */

void dxi_input (
    hwidbtype *idb, 
    paktype *pak)
{
    dxi_idb_struct_type *dxi_idb;
    dxi_pvc_info_type *dxi_pvc;
    ushort temp_dfa, dfa;

    dxi_pvc = NULL;
    dxi_idb = idb->atm_dxi_stuff;
    if (!dxi_idb) {
	datagram_done(pak);
        return;   /* paranoia check, should never really happen */
    }

    temp_dfa = GETSHORT(pak->datagramstart);
    dfa = DFA_TO_NUM(temp_dfa);
    dxi_pvc = dxi_idb->dxi_dfa_tab[dfa];

	if (!dxi_pvc) {
	    dxi_idb->dxi_err_dfa = dfa;
	    datagram_done(pak);
	    return;
	}
	dxi_pvc->pvc_stats.num_input_pkts++;
	dxi_pvc->pvc_stats.num_bytes_rcv += pak->datagramsize;

/*
 * We may have a 1483 snap encapsulated atm dxi bridge packet not dealt
 * with by parse packet. Check and set pak pointers accordingly if necessary.
 */

    dxi_bridge1483_check(pak);
 
/*
 * Overwrite pak->if_input with the IDB pointer from the PVC table
 * in order to associate the packet with the appropriate subinterface.
 */
	if (dxi_pvc->idb)
	    pak->if_input = dxi_pvc->idb;

	if (!(pak->flags & PAK_TBRIDGE_IN)) {
	    switch (GETSHORT((uchar *)pak->datagramstart + ADDRLEN_DXI)) {
	    case ATM_DXI_ISIS:
	    case ATM_DXI_ESIS:
	    case ATM_DXI_CLNP:
	    case DXI_SNAP_NLPID:
	    case ATM_DXI_IETF_IP:
		if (atm_dxi_debug_packet)
		    dxi_debug_pak(pak, "Incoming NLPID type");
		break;
	    case DXI_NON_ISO_LLC_SNAP:
		if (atm_dxi_debug_packet)
		    dxi_debug_pak(pak, "Incoming NON ISO LLC type");
		break;
	    case DXI_ISO_LLC:
		if (atm_dxi_debug_packet)
		    dxi_debug_pak(pak, "Incoming ISO LLC type");
		break;
	    default:
		if (atm_dxi_debug_packet) {
		    dxi_debug_pak(pak, "Incoming PER VC MUX type");
		    buginf("\n %x Enqueueing on %d queue", dfa, 
						dxi_pvc->linktype);
		}
	        if (dxi_pvc->linktype) {
                    pak->linktype = dxi_pvc->linktype;
                    reg_invoke_raw_enqueue(dxi_pvc->linktype, pak);
                } else {
                    dxi_pvc->pvc_stats.num_dropped_pkts++;
                    datagram_done(pak);
                }
                return;
           }
       pak->linktype = (*idb->getlink)(pak);
       if (pak->linktype != LINK_ILLEGAL ) {
           reg_invoke_raw_enqueue(pak->linktype, pak);
       } else {
           dxi_pvc->pvc_stats.num_dropped_pkts++;
           datagram_done(pak);
       }
       return;
	
     }
	pak->datagramsize -= pak->encsize;
	pak->datagramstart = pak->mac_start;
	net_bridge_enqueue(pak);
        return;
}

/*
 * dxi_getlink
 * Get link type for a ATM-DXI datagram.
 */

long dxi_getlink (paktype *pak)
{
    ushort type;
    snap_hdr *snap;
    atm_dxi_hdr *atm_dxi;
    atm_dxi_llc_hdr *llc_frame;
    ether_hdr *ether;
    long result;

    if (pak->enctype != ET_ATM_DXI)
	return(LINK_ILLEGAL);

    if (pak->flags & PAK_TBRIDGE_IN) {
        ether = (ether_hdr *)pak->datagramstart;
	return(type2link(ether->type_or_len));
    } 
    atm_dxi = (atm_dxi_hdr *)pak->datagramstart;
    switch (GETSHORT((uchar *)pak->datagramstart + ADDRLEN_DXI)) {
    	case ATM_DXI_IETF_IP:
	case ATM_DXI_ISIS:
	case ATM_DXI_ESIS:
	case ATM_DXI_CLNP:
	case DXI_SNAP_NLPID:
	    type = GETSHORT(&atm_dxi->type);
	    if (type == DXI_SNAP_NLPID) {
	   	snap = (snap_hdr *)pak->info_start;
	    	return(type2link(GETSHORT(&snap->type)));
	    }
	    return(nlpid2link(GETSHORT(&atm_dxi->type)));
	default:
    	    llc_frame = (atm_dxi_llc_hdr *)pak->datagramstart;
	    type = GETSHORT(&llc_frame->llc);
	    switch (type) {
		case DXI_NON_ISO_LLC_SNAP:
	    	    if(GETSHORT(&llc_frame->etype) == DXI_IEEE_DEC_BPDU) { 
                        return(LINK_IEEE_SPANNING);
                    }
                    return(type2link(GETSHORT(&llc_frame->etype)));
		case DXI_ISO_LLC:
	    	    result  = nlpid2link(GETSHORT(&llc_frame->cntrl));
	    	    return(result);
		default:
	    	    return(LINK_ILLEGAL);
	    }
    }
}

/*
 * Parse the user input 
 */
void dxi_parse_pvc_command(
    parseinfo *csb,
    idbtype *idb,
    ushort vpi,
    ushort vci,
    ushort encaps_type,
    boolean enable)
{
    dxi_idb_struct_type *dxi_idb;
    dxi_pvc_info_type *dxi_pvc;
    ushort dfa;
    int i;
    dximaptype *p, *ptemp;

    dxi_idb =  idb->hwptr->atm_dxi_stuff;
    dfa = vpivci_to_dfa_conversion(vpi, vci);
    dxi_pvc = dxi_idb->dxi_dfa_tab[dfa];

    if (!enable) {
	if (!dxi_pvc)
	    return;
	    for (i=0; i<RELAYHASH; i++) {
		p = dxitable[i];
	    	while (p) {
	    	    ptemp = p->next;
		    if (p && (p->idb == idb) && (p->dfa_num == dfa)) {
		   	dxi_map_delete(p);	    
		    }
		    p = ptemp;
		}
	    }
	    free(dxi_pvc);
	    dxi_idb->dxi_dfa_tab[dfa] = NULL;
	return;
    }

    if (!dxi_pvc) {
	dxi_pvc = dxi_pvc_init(idb, dfa, TRUE);
	if (!dxi_pvc)
	    return;
    } else {
	/* 
	 * Remove all maps configured on this pvc because pvc
	 * is being redefined.
	 */
	for (i=0; i<RELAYHASH; i++) {
	    p = dxitable[i];
	    while (p) {
	    	ptemp = p->next;
		if ((p->dfa_num == dfa) && (idb == p->idb)) {
		    dxi_map_delete(p);	    
		}
		p = ptemp;
	    }
	}
    }
    if (encaps_type == DXI_MAP_MUX) {
	dxi_pvc->dedicated_pvc = TRUE;
    } else {
	dxi_pvc->dedicated_pvc = FALSE;
    }
    dxi_pvc->encaps_type = encaps_type;
    dxi_pvc->configured = TRUE;
}

/*
 * Given a VPI and a VCI, return a DXI Frame Address (DFA)
 * The mumbo-jumbo follows the ATM-DXI spec. Please get a 
 * hold of one. 
 */
ushort vpivci_to_dfa_conversion(
	ushort vpi,
	ushort vci)
{
    ushort dfa, temp;
    vpi = vpi << 4; /* Move the VPI to the correct position within the DFA*/
    vpi &= 0x0F0;   /* Clear the other DFA bits */
    temp = vci;     /* save the VCI. We need it later. 
		     * bits 5 and 6 need to be separated from 
		     * bits 3-1.   
		     */
    temp &= 0x0F;   /* isolate bits 1-3 of the VCI. They will       */
		    /* occupy the LS bit positions within the DFA   */
    vci = vci >> 4; /* Now work on bits 5 and 6 of the vci          */
    vci = vci << 8; /* Move them to their new positions within DFA */
    vci = vci | temp;       /* put the lowest 4 bits into the DFA   */
    dfa = vci | vpi;       /* put in the VPI bits and voila!       */
			    /* We have the DFA!!		    */
return(dfa);
}

/*
 * dxi_parse_map
 * parse an atm_dxi map command
 *
 * The command format is <atm-dxi> <map> <protocol> <addr> <vpi> <vci>
 *
 * vpi : Virtual Path Identifier, range 1-15.
 * vci : Virtual circuit identifier, range 1-63.
 */
void dxi_parse_map (parseinfo *csb, idbtype *idb)
{
    addrtype proto_addr;
    ushort dfa, vpi, vci;
    boolean broadcast;
    dximaptype *addr;
    dxi_idb_struct_type *dxi_idb;
    dxi_pvc_info_type *dxi_pvc;
    boolean parse_rtn;
    uint linktype;

    parse_rtn = FALSE;
    broadcast = FALSE;
    dxi_idb =  idb->hwptr->atm_dxi_stuff;

    memset(&proto_addr, 0,sizeof(addrtype));
    vpi = GETOBJ(int,1);
    vci = GETOBJ(int,2);
    linktype = GETOBJ(int,10);

    if (csb->sense && !vci && !vpi) {
	printf("\n%%\007 Either one of VPI or VCI must be non-zero. ");
	return;
    }

    dfa = vpivci_to_dfa_conversion(vpi, vci);
    /* 
     * parse_proto_addr expects the DFA to be in OBJ(int,1)
     */
    SETOBJ(int,1) = dfa;    

    dxi_pvc = dxi_idb->dxi_dfa_tab[dfa];
    if (dxi_pvc && dxi_pvc->idb && dxi_pvc->idb != idb ) {
	printf("\n%% PVC %d already assigned to interface %s", dfa,
		dxi_pvc->idb->namestring);
	return;
    }

    /* Treat LINK_CLNS and LINK_BRIDGE separately   */
    if (!LINK_LEARN_OK(linktype))
	parse_rtn = parse_proto_addr(csb, NONULL, &proto_addr, GETOBJ(paddr,1),
					linktype);
    else
	parse_rtn = parse_proto_addr(csb, NONULL, &proto_addr,
					(addrtype *) &dfa,
					linktype);

    if (!parse_rtn) {
	printf("\n%% [ATM-DXI]Unable to parse map address");
	return;
    }

    /* If addr is NULL, address doesn't exist */
    addr = dxi_addr_lookup(idb, &proto_addr, linktype);
    if (!csb->sense) {
	if (addr) {
	    reg_invoke_bump_cache_version(idb->hwptr, NULL, FALSE);
	    dxi_map_delete(addr);
	    return;
	} else {
	    char buf[64];
	    print_proto_addr(idb, &proto_addr, linktype, buf);
	    printf("\n%% [ATM-DXI]Can't find address map for %s", buf);
	    return;
	}
    }

    /* Broadcast feature must be supported for ATM-DXI */
    broadcast = GETOBJ(int,3);

    if (dxi_pvc && dxi_pvc->dedicated_pvc && dxi_pvc->linktype && 
				(dxi_pvc->linktype != linktype)) {
	printf("\nDXI VC is dedicated to %s, %s cannot be configured",
		link2name(dxi_pvc->linktype), link2name(linktype));
	return;
    }

    /*
     * Check to see if this entry is in the map in its
     * entirety. If so then update with newer information
     * but don't complain. Otherwise check each part of the
     * map for redundancy.
     */
     if (addr == NULL) {
	if (!(addr = dxi_addr_enter(idb, &proto_addr, dfa, broadcast,
			linktype))) {
	    printf("\n%%Error entering address map");
	} else {
    	    dxi_pvc = dxi_idb->dxi_dfa_tab[dfa];
	    if (!dxi_pvc)
		return;
	    if (dxi_pvc->dedicated_pvc) {
		dxi_pvc->linktype = linktype;
		dxi_set_rxtype(dxi_pvc);
	    } else {
		dxi_pvc->linktype = LINK_ILLEGAL;
	    }

	    addr->broadcast = broadcast;
	}
    } else {
	if (bcmp(&addr->proto_addr, &proto_addr,
		    sizeof(addrtype)) == 0 &&
	    addr->link ==  linktype &&
	    NUM_TO_DFA(dfa) == addr->dfa) {
	    addr->broadcast = broadcast;
	    if (dxi_pvc->dedicated_pvc) {
		dxi_pvc->linktype = linktype;
		dxi_set_rxtype(dxi_pvc);
	    } else {
		dxi_pvc->linktype = LINK_ILLEGAL;
	    }
	} else {
	    printf("\n%% Address already in map");
	}
    }
}
void dxi_set_rxtype (dxi_pvc_info_type *dxi_pvc)
{
    switch (dxi_pvc->linktype) {
	case LINK_IP: 		dxi_pvc->rxtype = RXTYPE_DODIP; break;
	case LINK_APPLETALK: 	dxi_pvc->rxtype = RXTYPE_APPLE; break;
	case LINK_NOVELL:	dxi_pvc->rxtype = RXTYPE_NOVELL1; break;
	case LINK_VINES:	dxi_pvc->rxtype = RXTYPE_VINES; break;
	default:		dxi_pvc->rxtype = RXTYPE_UNKNOWN; break;
    }
}


/*
 * atm_dxi_command
 * Parse the "atm-dxi" command
 */

void atm_dxi_command (parseinfo *csb)
{
    idbtype *swidb;
    hwidbtype *idb;
    dxi_idb_struct_type *dxi_idb;

    swidb = csb->interface;
    idb = swidb->hwptr;
    dxi_idb =  idb->atm_dxi_stuff;

    if (idb->enctype != ET_ATM_DXI) {
	if (!csb->nvgen) {
		printf("\n%%ATM-DXI not configured on this interface");
	}
	return;
    }

    if (csb->nvgen) {
	if (GETOBJ(int,5) == TRUE)
	    return;
	switch (csb->which) {
	case ATM_DXI_PVC:
	{
    	    dxi_pvc_info_type *dxi_pvc;
	    int     vpi, vci, temp1, dfa;
	    char *str1 = " ";
	    int i, maptype;
	    for ( i= 0; i < MAXDFA_VALUES; i++) {
    		dxi_pvc = dxi_idb->dxi_dfa_tab[i];
		if (!dxi_pvc)
		    continue;
		if (swidb != dxi_pvc->idb)
		    continue;
		if (dxi_pvc && dxi_pvc->configured) {
		    /* 
		     * See the ATM-DXI spec to decode the 
		     * following mumbo jumbo!
		     */
		    dfa = i;
		    vpi = ((dfa & 0x0F0) >> 4);
		    temp1 = (dfa >> 4) & 0x030;
		    vci = (dfa & 0x00F) | temp1;
		    maptype = dxi_pvc->encaps_type;
		    switch(maptype) {
			case DXI_MAP_SNAP:
			    str1 = "snap";
			    break;
			case DXI_MAP_NLPID:
			    str1 = "nlpid";
			    break;
			case DXI_MAP_MUX:
			    str1 = "mux";
			    break;
			default:
			    str1 = "snap";
			    break;
		    }
	    	    nv_write(TRUE, "%s %d %d %s", csb->nv_command, vpi, 
						vci, str1);
		}
	    }
	}
	break;
	case ATM_DXI_DFA:
	{
	    int i, maptype;
	    dximaptype *p;
    	    dxi_pvc_info_type *dxi_pvc;
	    int     vpi, vci, temp1, dfa;
	    char *str1, *str2;

	    for (i=0; i<RELAYHASH; i++) {
		for (p = dxitable[i]; p; p = p->next) {
		    if ((p->link == LINK_ILLEGAL) && (swidb == p->idb)) {
		    	dfa = p->dfa_num;
		    	/* 
		     	 * See the ATM-DXI spec to decode the 
		     	 * following mumbo jumbo!
		     	 */
		    	vpi = ((dfa & 0x0F0) >> 4);
		    	temp1 = (dfa >> 4) & 0x030;
		    	vci = (dfa & 0x00F) | temp1;
    			dxi_pvc = dxi_idb->dxi_dfa_tab[dfa];
			if (!dxi_pvc)
		    	    continue;
		    	maptype = dxi_pvc->encaps_type;
		   	str1 = (p->broadcast ? " broadcast" : "");
		    	switch(maptype) {
			case DXI_MAP_SNAP:
			    str2 = "snap";
			    break;
			case DXI_MAP_NLPID:
			    str2 = "nlpid";
			    break;
			case DXI_MAP_MUX:
			    str2 = "mux";
			    break;
			default:
			    str2 = "snap";
			    break;
		    	}
		      	nv_write(TRUE, "%s %d %d%s %s", csb->nv_command,
			       vpi, vci, str1, str2);
		    }
	      	}
	    }   
	}
	break; 
	case ATM_DXI_MAP:
	{
	    int i;
	    char *str, *str1;
	    dximaptype *p;
	    char    mapaddr[64];
	    int     vpi, vci, temp1, dfa;

	    for (i=0; i<RELAYHASH; i++) {
		for (p = dxitable[i]; p; p = p->next) {
		    if((p->link != LINK_ILLEGAL) && (swidb == p->idb)) {
			str = (p->broadcast ? "broadcast" : "");
			/* 
			 * Go through the motions of retrieving
			 * the VPI and the VCI from the DFA.
			 * see comments under atm_dxi_parse_atm_dxi
			 * for more details.
			 */
			dfa = DFA_TO_NUM(p->dfa);
			vpi = ((dfa & 0x0F0) >> 4);
			temp1 = (dfa >> 4) & 0x030;
			vci = (dfa & 0x00F) | temp1;
		      	str1 = "";
			print_proto_addr(swidb, &p->proto_addr, p->link,
							mapaddr);
			nv_write(TRUE,"%s %s %d %d %s%s",
				csb->nv_command, mapaddr, vpi, vci, str, str1);
		    }
		}
	    }
	}
	break;
	  case ATM_DXI_ZAP:
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
    	case ATM_DXI_PVC:
	    dxi_parse_pvc_command(csb, swidb, GETOBJ(int,1), GETOBJ(int,2),
				GETOBJ(int,3), csb->sense);
	    break;
        case ATM_DXI_DFA:
	    if (is_p2p(swidb))
	    	dxi_parse_int_dfa(swidb, GETOBJ(int,1), GETOBJ(int,2), 
			csb->sense, GETOBJ(int,3), GETOBJ(int,4));
	    else
	    	printf("\n%%%s is not a point-to-point interface",
		   swidb->namestring);
	    break;
      	case ATM_DXI_MAP:
	    if ( is_p2p(swidb) ) {
	    	printf("\nATM-DXI INTERFACE-DFA command should be used on "
	    				"point-to-point interfaces");
	     	break;
	    }
	    dxi_parse_map(csb, swidb);
	    break;
      	case ATM_DXI_ZAP:
	    if (csb->sense) {
	    	dxi_clear_map(swidb);
	    }
	    break;
      	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
    }
}


/*
 * dxi_set_p2pvc: Called to set the current VCC as the p2p VCC on
 *      the specified ATM interface.
 */
static boolean
dxi_set_p2pvc (idbtype *idb, ushort dfa)
{
    if (idb->p2pmac) {
        return(FALSE);
    }
    idb->p2pmac = malloc(sizeof(hwaddrtype));
    if (!idb->p2pmac) {
        return(FALSE);
    }
    idb->p2pmac->type = STATION_FR10;
    idb->p2pmac->length = STATIONLEN_FR10;
    bcopy(&dfa, idb->p2pmac->addr, STATIONLEN_FR10);
    return (TRUE);
}

/*
 * fr_clear_p2pvc:     Called to remove a VCC from a p2p interface
 */
static void
dxi_clear_p2pvc (dxi_pvc_info_type* dxi_vc)
{
    idbtype *idb;

    idb = dxi_vc->idb;
    if(!idb->p2pmac) {
        return;
    }
    free(idb->p2pmac);
    idb->p2pmac = NULL;
}


/*
 * dxi_pvc_init
 * Create and initialize a PVC structure
 */
dxi_pvc_info_type *dxi_pvc_init (
    idbtype *idb,
    ushort dfa,
    boolean map_flag)
{
    dxi_idb_struct_type *dxi_idb;
    dxi_pvc_info_type *dxi_pvc;

    dxi_pvc = NULL;
    dxi_idb =  idb->hwptr->atm_dxi_stuff;
    dxi_pvc = dxi_idb->dxi_dfa_tab[dfa];
    if (!dxi_pvc) {
	dxi_pvc = create_dxi_pvc();
	if (!dxi_pvc)
	    return(NULL);
	dxi_pvc->pvc_type = DXI_PVC_STATIC;
	dxi_pvc->pvc_status = DFA_ACTIVE;
	dxi_pvc->map_flag = map_flag;
	dxi_pvc->idb = idb;   
	dxi_pvc->dfa = dfa;
	dxi_pvc->use_count = 0;
	dxi_pvc->encaps_type = DXI_MAP_SNAP;
	dxi_idb->dxi_dfa_tab[dfa] = dxi_pvc;
    }
    if ( is_p2p(idb) ) {
	dxi_set_p2pvc(idb, dfa);
    }
    return(dxi_pvc);
}

/*
 * Create a DXI pvc if it doesn't exist. 
 */
dxi_pvc_info_type * create_dxi_pvc (void)
{
    dxi_pvc_info_type *dxi_pvc;

    dxi_pvc =  malloc(sizeof(dxi_pvc_info_type));
    if (!dxi_pvc) {
	printf (nomemory);
	return(NULL);
    }
    /*
     * init stats
     */
    clear_dxi_pvc_stats(&dxi_pvc->pvc_stats);
    return(dxi_pvc);
}

/*
 * dxi_pvc_clear
 * Free the PVC struct and stats info
*/
void dxi_pvc_clear (
    hwidbtype *idb, 
    ushort dfa)
{
    dxi_idb_struct_type *dxi_idb;
    dxi_pvc_info_type *dxi_pvc;

    dxi_pvc = NULL;

    dxi_idb =  idb->atm_dxi_stuff;
    dxi_pvc = dxi_idb->dxi_dfa_tab[dfa];
    if (dxi_pvc) {
	if (dxi_pvc->use_count >= 0)
	    return;
	if (is_p2p(dxi_pvc->idb)) {
	    dxi_clear_p2pvc(dxi_pvc);
	}
	free(dxi_pvc);
	dxi_idb->dxi_dfa_tab[dfa] = NULL;
    }
}
/*
 * dxi_addr_enter
 * Enter a DXI map as specified by the user. 
 * Without a map, encapsulation failures will result
 * For DXI, all VCs are LOCAL and STATIC.
 */
dximaptype *dxi_addr_enter (
    idbtype *idb, 
    addrtype *proto_addr, 
    ushort dfa, 
    boolean broadcast, 
    uint link)
{
    int hash;
    dximaptype *l;
    leveltype level;
    dxi_pvc_info_type *dxi_pvc;
    dxi_idb_struct_type *dxi_idb;
    
    dxi_idb = idb->hwptr->atm_dxi_stuff;
    dxi_pvc = dxi_idb->dxi_dfa_tab[dfa];
    if (!dxi_pvc) {
	dxi_pvc = dxi_pvc_init(idb, dfa, TRUE);
	if (!dxi_pvc)
	    return(NULL);
	if (is_p2p(idb)) {
	    idb->subif_state = SUBIF_STATE_UP;
	    reg_invoke_route_adjust_msg(idb);
	    reg_invoke_route_adjust(idb);
	}
    }
    else {
	dxi_pvc->pvc_type = DXI_PVC_STATIC;
	dxi_pvc->idb = idb; 
    }
    dxi_pvc->use_count++;
    level = raise_interrupt_level(NETS_DISABLE);
    l = malloc(sizeof(dximaptype));
    if (l) {
	memset(&l->proto_addr, 0, sizeof(addrtype));
	bcopy(proto_addr, &l->proto_addr, sizeof(addrtype));
	l->idb = idb;
	l->broadcast = broadcast;
	l->link = link;
	l->dfa = NUM_TO_DFA(dfa);
	l->dfa_num = dfa;
	hash = xnethash((uchar *)&proto_addr->addr, proto_addr->length)
	    & (RELAYHASH-1);
	l->next = dxitable[hash];
	dxitable[hash] = l;
    }

    reset_interrupt_level(level);
    return(l);
}

/*
 * dxi_addr_lookup
 * Search the DXI map tables to find a specified map.
 */

dximaptype *dxi_addr_lookup (
    idbtype *idb,
    addrtype *proto_addr,
    register uint link)
{
    register dximaptype *l;
    leveltype level;

    if (is_p2p(idb)) {
	memset((uchar *)proto_addr, 0, sizeof(addrtype));
	bcopy((uchar *)&idb, (uchar *)&proto_addr->addr, sizeof(idb)); 
	proto_addr->length = sizeof(idb);
    }
    switch (link) {
    	case LINK_DECNET_ROUTER_L1:
    	case LINK_DECNET_ROUTER_L2:
    	case LINK_DECNET_PRIME_ROUTER:
    	case LINK_DECNET_NODE:
	    link = LINK_DECNET;
	    break;
    	default:
	    break;
    }
    level = raise_interrupt_level(NETS_DISABLE);
    l = dxitable[xnethash((uchar *)&proto_addr->addr, 
				proto_addr->length) & (RELAYHASH-1)];
    while (l) {
  	if (((l->link == link) || (l->link == LINK_ILLEGAL)) 
		&& l->idb == idb &&
	    	((bcmp((uchar *)&l->proto_addr, (uchar *)proto_addr,
		 sizeof(addrtype)) == 0) ||
	     (link == LINK_ILLEGAL))) {
	    reset_interrupt_level(level);
	    return(l);
	}
	l = l->next;
    }
    reset_interrupt_level(level);
    return(NULL);
}

/*
 * dxi_map_delete
 */

void dxi_map_delete (register dximaptype *hp)
{
    register uint hash;
    register dximaptype **map;
    leveltype level;
    dxi_idb_struct_type *dxi_idb;
    dxi_pvc_info_type *dxi_pvc;
    boolean map_err;

    map_err = TRUE;
    
    level = raise_interrupt_level(NETS_DISABLE);
    hash = xnethash((uchar *)&hp->proto_addr.addr, hp->proto_addr.length) &
	(RELAYHASH-1);
    map = &dxitable[hash];
    while (*map) {
	if (*map == hp) {
	    *map = hp->next;
	    hp->next = NULL;
	    map_err = FALSE;
	    break;
	}
	map = &((*map)->next);
    }		
    reset_interrupt_level(level);
    if (map_err == TRUE)
	return;
    dxi_idb = hp->idb->hwptr->atm_dxi_stuff;
    dxi_pvc = dxi_idb->dxi_dfa_tab[hp->dfa_num];
    if (--dxi_pvc->use_count <= 0) {
	dxi_pvc_clear(hp->idb->hwptr, hp->dfa_num);
    }
    free(hp);
}

/*
 * atm_dxi_deencapsulate
 * Remove atm-dxi parameters from the idb when changing encapsulations
 * on the interface.
 */

static void atm_dxi_deencapsulate (
    hwidbtype *idb)
{
    dxi_idb_struct_type *dxi_idb;
    dxi_pvc_info_type *dxi_pvc;
    int i;
    idbtype *swidb;

    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	if (is_first_swidb(swidb))
	    swidb->ip_split_disabled = swidb->ip_split_disabled_def = FALSE;
	dxi_clear_map(swidb);
	if (is_subinterface(swidb))
	    shutdown_subif(swidb, SUBIF_STATE_DELETED);
    }
    dxi_idb =  idb->atm_dxi_stuff;
    if (!dxi_idb)
        return;
    /*
     * clean up stats and pvc tables
     */
    for (i = 0; i < MAX_DFA; i++) {
	if (!(dxi_pvc = dxi_idb->dxi_dfa_tab[i]))
	    continue;
	free(dxi_pvc);
	dxi_idb->dxi_dfa_tab[i] = NULL;
    }
    free(dxi_idb);
    idb->atm_dxi_stuff = NULL;
}

/*
 * atm_dxi_linestate
 * Return current line protocol status
 */

static boolean atm_dxi_linestate (hwidbtype *hwidb)
{
    return(TRUE);
}

/*
 * atm_dxi_extract_addr
 *
 * Given a atm-dxi packet, extract the requested 'MAC' level address
 * into a hwaddrtype structure.
 */
static boolean atm_dxi_extract_addr (
    paktype *pak,
    hwaddrtype *address,
    enum SRCDST which)
{
    dxi_idb_struct_type *dxi_idb;
    atm_dxi_hdr *hdr;
    ushort dfa;

    hdr = (atm_dxi_hdr *)pak->datagramstart;
    address->type = STATION_FR10;
    address->length = STATIONLEN_FR10;
    address->target_type = TARGET_UNICAST;
    switch (which) {
      case GET_SOURCE:
	dxi_idb = pak->if_input->hwptr->atm_dxi_stuff;
	dfa = DFA_TO_NUM(GETSHORT(&hdr->dfa));
	bcopy(&dfa, address->addr, STATIONLEN_FR10);
	break;
      case GET_DEST:
	return(FALSE);
    }

    return(TRUE);
}

/*
 * atm_dxi_parse_packet
 *
 * Given a packet buffer and the first four bytes of a serial interface
 * datagram, determine the value of pak->datagramstart so the datagram's
 * network level protocols are aligned on a long word boundary.
 * Returns pointer to function to enqueue the datagram, NULL if datagram
 * should be discarded.
 */
static iqueue_t atm_dxi_parse_packet (
    hwidbtype *idb,
    paktype *pak,
    uchar *data)
{
    dxi_idb_struct_type *dxi_idb;
    atm_dxi_hdr *frame;
    atm_dxi_llc_hdr *llc_frame;
    snap_hdr *snap; 
    ether_hdr *ether;
    charlong firstlong;

    /*
     * Check to see if building a frame in a virgin packet, or whether
     * we're just setting up media pointers into the frame. Note           
     * dxi_bridge1483_check in dxi_input takes care of 1483 bridge 
     * alignments we might have missed here.      
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
	firstlong.d.lword = GETLONG(pak->datagramstart);
    }

    dxi_idb = idb->atm_dxi_stuff;


    if (firstlong.d.sword[1] == TYPE_BRIDGE) { 

	pak->enctype = ET_BRIDGE;

	switch (firstlong.d.sword[1]) {
	case TYPE_BRIDGE:
	    pak->flags |= PAK_TBRIDGE_IN;
	    if (data) {
	    	frame = (atm_dxi_hdr *)(pak_center(pak) -
					ATM_DXI_ENCAPBYTES -
					ETHER_ARPA_ENCAPBYTES);
	    	pak->datagramstart = (uchar *)frame;
	    } else {
	    	frame = (atm_dxi_hdr *)pak->datagramstart;
	    }
	    ether              = (ether_hdr *)frame->data;
	    pak->mac_start     = (uchar *)ether;
	    pak->addr_start    = ether->daddr;
            pak->info_start    = NULL;
            break;
	default:
	    return(NULL);	/* should never happen */
	}
	pak->rif_start     = NULL;
	pak->riflen        = 0;
	pak->network_start = ether->data;
	pak->encsize = pak->network_start - pak->datagramstart;
	return(dxi_input);
    }

    pak->enctype = ET_ATM_DXI;
    pak->flags &= ~PAK_TBRIDGE_IN;

    switch (firstlong.d.sword[1]) {
    case DXI_NON_ISO_LLC_SNAP:
	if (data) {
	    llc_frame = (atm_dxi_llc_hdr *)(pak_center(pak) -
				    ATM_DXI_SNAPENCAPBYTES);
	    pak->datagramstart = (uchar *)llc_frame;
	} else {
	    llc_frame = (atm_dxi_llc_hdr *)pak->datagramstart;
	}
	pak->info_start    = (uchar *)llc_frame->snap_start;
	pak->network_start = llc_frame->snap_data_start;
	break;

    case DXI_ISO_LLC:
	if (data) {
	    llc_frame = (atm_dxi_llc_hdr *)(pak_center(pak) -
				    ATM_DXI_ISOPDU_HDRBYTES);
	    pak->datagramstart = (uchar *)llc_frame;
	} else {
	    llc_frame = (atm_dxi_llc_hdr *)pak->datagramstart;
	}
	pak->info_start    = (uchar *)llc_frame->llc_start;
	pak->network_start = llc_frame->iso_pdu_start;
	break;

    case DXI_SNAP_NLPID:
	if (data) {
	    frame = (atm_dxi_hdr *)(pak_center(pak) - ATM_DXI_SNAPENCAPBYTES);
	    pak->datagramstart = (uchar *)frame;
	} else {
	    frame = (atm_dxi_hdr *)pak->datagramstart;
	}
	snap = (snap_hdr *)frame->snap_start;
	pak->info_start    = (uchar *)snap;
	pak->network_start = snap->data;
	break;

    case ATM_DXI_IETF_IP:
	if (data) {
	    frame = (atm_dxi_hdr *)(pak_center(pak) - ATM_DXI_ENCAPBYTES);
	    pak->datagramstart = (uchar *)frame;
	} else {
	    frame = (atm_dxi_hdr *)pak->datagramstart;
	}
	pak->info_start    = frame->snap_start;
	pak->network_start = frame->data;
	break;

    case ATM_DXI_CLNP:
    case ATM_DXI_ESIS:
    case ATM_DXI_ISIS:
	if (data) {
	    frame = (atm_dxi_hdr *)(pak_center(pak) - ATM_DXI_ENCAPBYTES);
	    pak->datagramstart = (uchar *)frame;
	} else {
	    frame = (atm_dxi_hdr *)pak->datagramstart;
	}
	pak->info_start    = frame->snap_start;
	pak->network_start = frame->data;
	break;

    default:
	if (data) {
	    frame = (atm_dxi_hdr *)(pak_center(pak) - ADDRLEN_DXI);
	    pak->datagramstart = (uchar *)frame;
	} else {
	    frame = (atm_dxi_hdr *)pak->datagramstart;
	}
	pak->info_start    = frame->snap_start;
	pak->network_start = frame->snap_start;
	break;
    }
    pak->mac_start     = NULL;
    pak->addr_start    = NULL;
    pak->rif_start     = NULL;
    pak->riflen        = 0;
    pak->encsize = pak->network_start - pak->datagramstart;
    return(dxi_input);
}

/*
 * atm_dxi_setup
 * Initialize atm-dxi encapsulation
 */
static boolean atm_dxi_setup (parseinfo *csb, hwidbtype *idb)
{
    dxi_idb_struct_type *atm_dxi;
    int i;
    idbtype *swidb;

    atm_dxi = malloc(sizeof(dxi_idb_struct_type));
    if (!atm_dxi)
        return(FALSE);

    idb->nokeepalive = TRUE;
	
    /*
     * initialize the pvc tab
     */
     for (i = 0; i < MAX_DFA; i++)
          atm_dxi->dxi_dfa_tab[i] = NULL;

    /*
     * Set the fastswitch flavor
     */
    idb->fast_switch_type = FS_ATM_DXI;

    idb->inloopback = FALSE;
    idb->deencap = atm_dxi_deencapsulate;
    idb->vencap = dxi_vencapsulate;
    idb->bridge_vencap = (bridge_vencap_t)return_null;
    idb->getlink = dxi_getlink;
    idb->oqueue = holdq_enqueue;
    idb->oqueue_dequeue = holdq_dequeue;
    idb->iqueue = dxi_input;
    idb->parse_packet = atm_dxi_parse_packet;
    idb->broadcast = (broadcast_t) return_fuz_false;
    idb->keep_count = 0;
    idb->extract_hwaddr = invalidate_hwaddr;
    idb->extract_snpa = atm_dxi_extract_addr;
    idb->encsize = ATM_DXI_ENCAPBYTES;
    idb->span_encapsize = idb->encsize;
    idb->lineaction = dxi_cstate;
    idb->linestate = atm_dxi_linestate;
    idb->atm_dxi_stuff = atm_dxi;
    idb->enctype = ET_ATM_DXI;	/* default encapsulation */
    idb->span_bridge_process_force |= SPAN_BR_PROC_FR_BRIDGE;
    idb->show_support = NULL; 
  
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	reg_invoke_media_enctype_setup(swidb, ET_ATM_DXI);
	swidb->ip_enctype = ET_ATM_DXI;
	if (is_first_swidb(swidb))
	    swidb->ip_split_disabled = swidb->ip_split_disabled_def = TRUE;
	swidb->ip_routecache_same = swidb->ip_routecache_same_def = TRUE;
    }
    if (supports_maxdgram(idb)) {
        if (!(idb->status & IDB_HSSI))
            (*idb->set_maxdgram)(idb, idb->maxmtu + ETHER_BAGGAGE, 
	        idb->maxmtu + ETHER_BAGGAGE + HDLC_ENCAPBYTES);
        else /* HSSI */
          (*idb->set_maxdgram)(idb, 
	    idb->maxmtu + ETHER_BAGGAGE + DS3_OVERHEAD_BYTES, 
	    idb->maxmtu + ETHER_BAGGAGE + DS3_BAGGAGE);
    }
    (*(idb->reset))(idb);

    return(TRUE);
}

/*
 * dxi_hash_init
 * Initialize the hash table
 */

void dxi_hash_init (void)
{
    int i;
    
    for (i = 0; i < RELAYHASH; i++)
	dxitable[i] = NULL;
}
/*
 * dxi_init_pvc_stats
 */
static void dxi_init_pvc_stats (dxi_idb_struct_type *dxi_idb)
{
    dxi_pvc_info_type *dxi_pvc;
    int i;

    for (i = 0; i < MAX_DFA; i++) {
	if (!(dxi_pvc = dxi_idb->dxi_dfa_tab[i]))
	    continue;

	clear_dxi_pvc_stats(&dxi_pvc->pvc_stats);
    }
}
/* 
 * Clear any applicable counters
 */
static void dxi_clear_counters (hwidbtype *idb)
{
    dxi_idb_struct_type *dxi_idb;

    if (is_atm_dxi(idb) && (dxi_idb = idb->atm_dxi_stuff))
	dxi_init_pvc_stats(dxi_idb);
}
static ushort dxi_get_subint_ref_num_from_pak (hwidbtype *hwidb, paktype *pak)
{
    ushort dfa;

    dfa = GETSHORT(pak->mac_start);
    return(DFA_TO_NUM(dfa));
}
/*
 * atm_dxi_init
 */
static void atm_dxi_init (subsystype *subsys)
{
    atm_dxi_debug_init();
    dxi_hash_init();

    /*
     * Register some functions
     */
    reg_add_clear_counters(dxi_clear_counters, "dxi_clear_counters");
    reg_add_media_serial_setup(ET_ATM_DXI, atm_dxi_setup, "atm_dxi_setup");
    reg_add_swif_erase(dxi_clear_map, "dxi_clear_map");
    reg_add_encaps_nvram_write(ET_ATM_DXI, dxi_encaps_nvram_write,
			       "dxi_encaps_nvram_write");
    reg_add_encaps_show_protocol_attr(ET_ATM_DXI, 
				      dxi_encaps_show_protocol_attr, 
		      		      "dxi_encaps_show_protocol_attr");
    reg_add_encaps_set_protocol_attr(ET_ATM_DXI, 
				     dxi_encaps_set_protocol_attr, 
		      		     "dxi_encaps_set_protocol_attr");
    reg_add_get_subint_ref_num_from_pak(ET_ATM_DXI,
					dxi_get_subint_ref_num_from_pak,
					"dxi_get_subint_ref_num_from_pak");
				       
    /*
     * Initialize atm-dxi support
     */
    atm_dxi_parser_init();

}

/*
 * show_atm_dxi options
 */

void show_atm_dxi(parseinfo *csb)
{
    switch (csb->which) {

      case SHOW_ATM_DXI_MAP:
	show_atm_dxi_map();
	break;

      case SHOW_ATM_DXI_PVC:
	show_dxi_pvc_stats(csb);
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}
/*
 * show pvc statistics
 */

void show_dxi_pvc_stats (parseinfo *csb)
{
    hwidbtype *idb;
    ushort vpi, vci, dfa = 0;

    automore_enable(NULL);
    if (GETOBJ(idb,1)) {
	idb = GETOBJ(idb,1)->hwptr;
	if ((GETOBJ(int,1) != -1) && (GETOBJ(int,2) != -1)) {
	    vpi = (ushort) GETOBJ(int,1);
	    vci = (ushort) GETOBJ(int,2);
	    dfa = vpivci_to_dfa_conversion(vpi, vci);
	    if (is_atm_dxi(idb))
		show_dxi_idb_stats(idb, dfa);
	    else
		printf("\nInvalid interface specified");
	}
    } else {
	FOR_ALL_HWIDBS(idb) {
	    if (is_atm_dxi(idb))
	   	 show_dxi_idb_stats(idb, 0);
	    }
    }
    automore_disable();
}
/*
 * show pvc per idb stats
 */

void show_dxi_idb_stats (hwidbtype *idb, ushort dfa)
{
    dxi_idb_struct_type *dxi_idb;
    dxi_pvc_info_type *dxi_pvc;
    int i;

    if (!(dxi_idb = idb->atm_dxi_stuff))
	return;

    if (!dfa) {
	printf ("\n\nPVC Statistics for interface %s (ATM DXI)",
		idb->hw_namestring);
	for (i = 0; i < MAX_DFA; i++) {
	    if (!(dxi_pvc = dxi_idb->dxi_dfa_tab[i]))
		continue;
	    display_dxi_pvc_stats(dxi_pvc, i, TRUE);
	    automore_conditional(0);
	}
    } else {          /* show stats for a particular dfa*/
	if ((dxi_pvc = dxi_idb->dxi_dfa_tab[dfa])) {
	    if (!dxi_pvc)
		return;
	    printf ("\n\nPVC Statistics for interface %s (ATM DXI)",
		idb->hw_namestring);
	    display_dxi_pvc_stats(dxi_pvc, dfa, TRUE);
	} else
		printf ("\nInvalid DFA specified");
    }
}
/*
 * display pvc stats
 *
 * Display particular pvc stats
 */

void display_dxi_pvc_stats (
    	dxi_pvc_info_type *dxi_pvc,
	ushort dfa,
	boolean static_pvc)
{
    dxi_pvc_stats_type pvc_stats;
    uchar  pvc_status;
    ushort vpi, vci, tmp;

    pvc_stats = dxi_pvc->pvc_stats;

    /* 
     * to decode this mumbo jumbo, please see a DXI spec.
     */
    vpi = ((dfa & 0x0F0) >> 4);
    tmp = (dfa >> 4) & 0x030;
    vci = (dfa & 0x00F) | tmp;
    printf ("\n\nDFA = %d, VPI = %d, VCI = %d, PVC STATUS = ", dfa, vpi, vci);
    if (static_pvc)
	printf ("STATIC");
    else {
	pvc_status = dxi_pvc->pvc_status;
	if (IS_DFA_DELETED(pvc_status))
	    printf ("DELETED");
	else
	    printf ("%s", IS_DFA_ACTIVE(pvc_status) ? "ACTIVE" : "INACTIVE");
    }
    printf(", INTERFACE = %s", dxi_pvc->idb->namestring);
    printf("\n\n  input pkts %10lu    output pkts %10lu   in bytes %10lu",
	   dxi_pvc_input_pkts(&pvc_stats), dxi_pvc_output_pkts(&pvc_stats),
	   dxi_pvc_input_bytes(&pvc_stats));
    printf("\n  out bytes %10lu     dropped pkts %10lu  ",
	   dxi_pvc_output_bytes(&pvc_stats), pvc_stats.num_dropped_pkts);
}

/*
 * show_atm_dxi_map
 * Lock the memory before attempting display. This prevents
 * memory from being freed from under us. 
 */
void show_atm_dxi_map (void)
{
    dximaptype *p, *ptemp;
    int i;
    hwidbtype *idb;
    
    automore_enable(NULL);
    FOR_ALL_HWIDBS(idb) {
	if (!is_atm_dxi(idb))
	    continue;
	for (i = 0; i < RELAYHASH; i++) {
	    p = dxitable[i];
	    while (p != NULL) {
		if ( p->link == LINK_ILLEGAL) {
		     p = p->next;
		     continue;
		}
		if (idb != p->idb->hwptr) {
		     p = p->next;
		    continue;
		}
		mem_lock(p);
		dxi_display_map(p);
		ptemp = p->next;
		free(p);
		p = ptemp;
	    }
	}
	for (i = 0; i < RELAYHASH; i++) {
	    p = dxitable[i];
	    while (p != NULL) {
		if (idb != p->idb->hwptr) {
		     p = p->next;
		    continue;
		}
		mem_lock(p);
		if ( p->link == LINK_ILLEGAL )
		    dxi_display_int_dfa(p);
		ptemp = p->next;
		free(p);
		p = ptemp;
	    }
	}
    }
    automore_disable();
}

/*
 * dxi_display_map
*/
void dxi_display_map (dximaptype *p)
{
    dxi_idb_struct_type *dxi_idb;
    dxi_pvc_info_type *dxi_pvc;
    char *str;
    ushort vpi, vci, tmp;
    
    dxi_idb =  p->idb->hwptr->atm_dxi_stuff;

    printf("\n%s (%s): ", p->idb->namestring, print_idbstate(p->idb));
    if (p->link == LINK_CLNS) {
	printf("CLNS ");
    } else {
	char buf[64];
	if (p->link == LINK_COMPRESSED_TCP)
	    print_proto_addr(p->idb, &p->proto_addr, LINK_IP, buf);
	else
	    print_proto_addr(p->idb, &p->proto_addr, p->link, buf);
	printf("%s ", buf);
    }
    if (is_subinterface(p->idb)) {
    	if (p->idb->subif_state == SUBIF_STATE_ADMINDOWN)
	    printf("\n              ");
    } else {
	if (p->idb->hwptr->state == IDBS_ADMINDOWN)
	    if (!p->idb->hwptr->standby)
	        printf("\n              ");
    }
    printf("DFA %d(0x%x,0x%x)",p->dfa_num, p->dfa_num, p->dfa);
    printf(", static,");
    if (p->broadcast)
	printf("\n              broadcast,");
    dxi_pvc = dxi_idb->dxi_dfa_tab[p->dfa_num];
    /* 
     * to decode this mumbo jumbo, please see a DXI spec.
     */
    vpi = ((p->dfa_num & 0x0F0) >> 4);
    tmp = (p->dfa_num >> 4) & 0x030;
    vci = (p->dfa_num & 0x00F) | tmp;
    printf(" vpi = %d, vci = %d, ", vpi, vci);
    switch (dxi_pvc->encaps_type) {
	case ATM_DXI_VC_NLPID:
	    str = " NLPID";
	    break;
	case ATM_DXI_VC_MUX:
	    str = " VC based MUX";
	    break;
	case ATM_DXI_VC_SNAP:
	    str = " SNAP";
	    break;
	default:
	    str = " SNAP (default)";
	    break;
    }
    printf("\n              encapsulation:%s", str);
    if (dxi_pvc->dedicated_pvc) {
	printf(", ");
	printf("\n              Linktype %s", link2name(dxi_pvc->linktype));
    }

}

/*
 * dxi_display_int_dfa
 * Display DFAs attached to a subinterface
*/
void dxi_display_int_dfa (dximaptype *p)
{
    dxi_idb_struct_type *dxi_idb;
    dxi_pvc_info_type *dxi_pvc;
    char *str;
    ushort vpi, vci, tmp;
    
    dxi_idb =  p->idb->hwptr->atm_dxi_stuff;
    printf("\n%s (%s): ", p->idb->namestring, print_idbstate(p->idb));
    printf("point-to-point dfa, ");
    printf("DFA %d(0x%x,0x%x)",p->dfa_num, p->dfa_num, p->dfa);
    if (p->broadcast)
	printf(", broadcast");
    dxi_pvc = dxi_idb->dxi_dfa_tab[p->dfa_num];
    /* 
     * to decode this mumbo jumbo, please see a DXI spec.
     */
    vpi = ((p->dfa_num & 0x0F0) >> 4);
    tmp = (p->dfa_num >> 4) & 0x030;
    vci = (p->dfa_num & 0x00F) | tmp;
    printf(" vpi = %d, vci = %d, ", vpi, vci);
    switch (dxi_pvc->encaps_type) {
	case ATM_DXI_VC_NLPID:
	    str = " NLPID";
	    break;
	case ATM_DXI_VC_MUX:
	    str = " VC based MUX";
	    break;
	case ATM_DXI_VC_SNAP:
	    str = " SNAP";
	    break;
	default:
	    str = " SNAP (default)";
	    break;
    }
    printf("\n              encapsulation:%s", str);
}

/*
 * Support Higher level broadcast over DFAs.
 * dxi_broadcast
 */
dximaptype *dxi_broadcast (
    register idbtype *idb,
    register paktype *pak,
    uint link)
{
    int i;
    dximaptype *first_addr;
    register paktype *newpak;
    register dximaptype *addr;
    boolean result;
    dxi_idb_struct_type *dxi_idb;

    dxi_idb = idb->hwptr->atm_dxi_stuff;
    first_addr = NULL;
/*
 * Handle sets of link types that should match an FR map
 * e.g., LINK_DECNET_ROUTER_L1 -> LINK_DECNET
 */
    switch (link) {
    case LINK_DECNET_ROUTER_L1:
    case LINK_DECNET_ROUTER_L2:
    case LINK_DECNET_PRIME_ROUTER:
    case LINK_DECNET_NODE:
	link = LINK_DECNET;
	break;
    case LINK_DEC_SPANNING:
    case LINK_IEEE_SPANNING:
	link = LINK_BRIDGE;
	break;
    default:
	break;
    }
    if (atm_dxi_events_debug)
	buginf("\n%s: broadcast search", idb->namestring);
    for (i = 0; i < RELAYHASH; i++) {
	for (addr = dxitable[i]; addr; addr = addr->next) {
	    if (!(addr->broadcast) && !is_p2p(addr->idb))
		continue;
	    if (addr->idb != idb)
		continue;
	    if ((addr->link != link) && (!((link == LINK_IP) &&
				(addr->link == LINK_COMPRESSED_TCP)))
				&& (addr->link != LINK_ILLEGAL))
		continue;
	    if (first_addr == NULL) {
		first_addr = addr;
		continue;
	    }
	    newpak = pak_duplicate(pak);
	    if (newpak) {
		if (atm_dxi_events_debug)
		    buginf("\n%s: Broadcast on DFA %d  link %d",
			   idb->namestring, DFA_TO_NUM(addr->dfa),
			   addr->link);
		result = dxi_encaps(newpak, 0L, addr);
		if (result && idb->hwptr->board_encap != NULL)
		    result = (*idb->hwptr->board_encap)(newpak,
							idb->hwptr);
		if (result)
		    datagram_out(newpak);
		else
		    retbuffer(newpak);
	    }
	}
    }
    return(first_addr);
}

/*
 * dxi_clear_map
 * Re-initialize the atm-dxi map
 */

void dxi_clear_map (idbtype *idb)
{
    dximaptype *p, *ptemp;
    int i;
    dxi_idb_struct_type *dxi_idb;

    dxi_idb = idb->hwptr->atm_dxi_stuff;

    if (idb->hwptr->enctype_cfg != ET_ATM_DXI)
	return;

    for (i = 0; i < RELAYHASH; i++) {
	p = dxitable[i];
	while (p) {
	    ptemp = p->next;
	    if (p->idb == idb) {
		dxi_map_delete(p);
	    }
	    p = ptemp;
	}
    }
}

/*
 * dxi_cstate
 * Handle interface state change
*/
void dxi_cstate (hwidbtype *idb)
{
    int i;
    dxi_idb_struct_type *dxi_idb;
    dxi_pvc_info_type *dxi_pvc;
    
    dxi_idb = idb->atm_dxi_stuff;

    if (idb->state != IDBS_UP) {
	for (i = 0; i < MAX_DFA; i++) {
	    if (!(dxi_pvc = dxi_idb->dxi_dfa_tab[i]))
		continue;
	    dxi_pvc->pvc_status = DFA_DELETED;
	    if (is_p2p(dxi_pvc->idb)) {
		dxi_pvc->idb->subif_state = SUBIF_STATE_DOWN;
		reg_invoke_route_adjust_msg(dxi_pvc->idb);
		reg_invoke_route_adjust(dxi_pvc->idb);
	    }
	}
    } else {
	for (i = 0; i < MAX_DFA; i++) {
	    if (!(dxi_pvc = dxi_idb->dxi_dfa_tab[i]))
		continue;
	    dxi_pvc->pvc_status = DFA_ACTIVE;
	    if (is_p2p(dxi_pvc->idb)) {
		dxi_pvc->idb->subif_state = SUBIF_STATE_UP;
		reg_invoke_route_adjust_msg(dxi_pvc->idb);
		reg_invoke_route_adjust(dxi_pvc->idb);
	    }
    	}
    }
}

/*
 * dxi_parse_int_dfa
 * Add a DFA to the current subinterface
 */
void dxi_parse_int_dfa (
    idbtype *idb,
    ushort vpi,
    ushort vci,
    boolean enable,
    ushort encaps,
    boolean broadcast)
{
    dximaptype *map;
    addrtype proto_addr;
    dxi_pvc_info_type *dxi_pvc;
    dxi_idb_struct_type *dxi_idb;
    ushort dfa;
    
    memset(&proto_addr, 0, sizeof(addrtype));
    bcopy((uchar *)&idb, &proto_addr.addr, sizeof(idb)); 
    proto_addr.length = sizeof(idb);
    
    dfa = vpivci_to_dfa_conversion(vpi, vci);
    dxi_idb = idb->hwptr->atm_dxi_stuff;
    dxi_pvc = dxi_idb->dxi_dfa_tab[dfa];
    map = dxi_addr_lookup(idb, &proto_addr, LINK_ILLEGAL);

    if ( !enable ) {
	if (dxi_pvc)
	    dxi_pvc->idb = dxi_pvc->idb->hwptr->firstsw;
	if ( map ) {
	    dxi_map_delete(map);
	}
    } else {
	/* 
	 * First make sure that the DFA isn't assigned to
	 * another subinterface. 
	 */
	if (dxi_pvc) {
	    if (dxi_pvc->idb && (dxi_pvc->idb != idb) &&
		(dxi_pvc->idb != dxi_pvc->idb->hwptr->firstsw)) {
		printf("\n%%PVC already assigned to interface %s", 
		       dxi_pvc->idb->namestring);
		return;
	    }
	}
	
	if ( !map ) {
	    /* Don't allow two PVCs on a point-to-point subinterface. */
		if (dxi_addr_enter(idb, &proto_addr, dfa, broadcast, 
				LINK_ILLEGAL)) {
		    /* Set PVC value initialized by the map creation */
		    dxi_pvc = dxi_idb->dxi_dfa_tab[dfa];
		    dxi_pvc->encaps_type = encaps;
		}
	} else
	    printf("\n%%%s:Duplicate map entry", idb->namestring);
    }
}
/* 
 * dxi_encaps_nvram_write
 * Used by the "show interface" command
 */
void dxi_encaps_nvram_write (parseinfo *csb)
{
    nv_write(TRUE, "encapsulation atm-dxi");
}
/* 
 * dxi_encaps_show_protocol_attr
 * Used by the "show interface" command
 */
void dxi_encaps_show_protocol_attr (idbtype *swidb)
{
    printf("\n  Encapsulation ATM-DXI");
}
/*
 * dxi_encaps_set_protocol_attr
 * Used by the "show interface" command
 */
void dxi_encaps_set_protocol_attr (idbtype *swidb)
{
    swidb->hwptr->nokeepalive = TRUE;
}

/*
 * dxi_snap_encaps: Called to do SNAP encapsulation.
 */
void dxi_snap_encaps(paktype *pak, ushort dfa)
{
    atm_dxi_hdr *dxi;
    snap_hdr *snap;
    sap_hdr *sap;

    switch(pak->linktype) {
    case LINK_CLNS:
    case LINK_CLNS_BCAST:
    case LINK_CLNS_ALL_ES:
    case LINK_CLNS_ALL_IS:
    case LINK_ISIS_ALL_L1_IS:
    case LINK_ISIS_ALL_L2_IS:
	pak->enctype = ET_ATM_DXI;
	pak->encsize = ATM_DXI_ISOPDU_HDRBYTES;
	pak->datagramstart -= ATM_DXI_ISOPDU_HDRBYTES;
	pak->datagramsize += ATM_DXI_ISOPDU_HDRBYTES;
	dxi = (atm_dxi_hdr *)pak->datagramstart;
	PUTSHORT(&dxi->dfa, (dfa | ATM_DXI_EA_BITS));
    	sap = (sap_hdr *)(pak->datagramstart + sizeof(ushort));
	sap->dsap = sap->ssap = SAP_CLNS;
	sap->control = LLC1_UI;
	if (atm_dxi_debug_packet)
	    dxi_debug_pak(pak, "Outgoing OSI LLC header");
   	break;
   case LINK_DEC_SPANNING:
   case LINK_IEEE_SPANNING:
        pak->enctype = ET_ATM_DXI;
        pak->encsize = ATM_DXI_SNAPENCAPBYTES;
        pak->datagramstart -= (ATM_DXI_SNAPENCAPBYTES);
        pak->datagramsize += (ATM_DXI_SNAPENCAPBYTES);
        memset(pak->datagramstart, 0, ATM_DXI_SNAPENCAPBYTES);
        dxi = (atm_dxi_hdr *)pak->datagramstart;
        PUTSHORT(&dxi->dfa, (dfa | ATM_DXI_EA_BITS));
        snap = (snap_hdr *)(pak->datagramstart + sizeof(ushort));
        snap->dsap = snap->ssap = SAP_SNAP;
        snap->control = LLC1_UI;
        ieee_copy_oui(tb1490_snap, snap->oui);
        snap->type = DXI_IEEE_DEC_BPDU;
        if (atm_dxi_debug_packet)
            dxi_debug_pak(pak, "Outgoing Bridge BPDU header");
        break; 
   case LINK_BRIDGE:
	pak->enctype = ET_ATM_DXI;
	pak->encsize = ATM_DXI_BRIDGE_ENCAPBYTES;
    	pak->datagramstart -= (ATM_DXI_BRIDGE_ENCAPBYTES);
	pak->datagramsize += (ATM_DXI_BRIDGE_ENCAPBYTES);
	memset(pak->datagramstart, 0, ATM_DXI_BRIDGE_ENCAPBYTES);
	dxi = (atm_dxi_hdr *)pak->datagramstart;
	PUTSHORT(&dxi->dfa, (dfa | ATM_DXI_EA_BITS));
	snap = (snap_hdr *)(pak->datagramstart + sizeof(ushort));
	snap->dsap = snap->ssap = SAP_SNAP;
	snap->control = LLC1_UI;
	ieee_copy_oui(tb1490_snap, snap->oui);
        snap->type = DXI_1483_ETHER_BRIDGE;
        if (atm_dxi_debug_packet)
	    dxi_debug_pak(pak, "Outgoing Bridge header");
	break;
    default:
	pak->enctype = ET_ATM_DXI;
	pak->encsize = ATM_DXI_SNAPENCAPBYTES;
    	pak->datagramstart -= (ATM_DXI_SNAPENCAPBYTES);
	pak->datagramsize += (ATM_DXI_SNAPENCAPBYTES);
	memset(pak->datagramstart, 0, ATM_DXI_SNAPENCAPBYTES);
	dxi = (atm_dxi_hdr *)pak->datagramstart;
	PUTSHORT(&dxi->dfa, (dfa | ATM_DXI_EA_BITS));
	snap = (snap_hdr *)(pak->datagramstart + sizeof(ushort));
	snap->dsap = snap->ssap = SAP_SNAP;
	snap->control = LLC1_UI;
	snap->type = link2serial(pak->linktype);
	if (atm_dxi_debug_packet)
	    dxi_debug_pak(pak, "Outgoing Non OSI LLC/SNAP header");
	break;
    }
}

/*
 * dxi_nlpid_encaps: Called to do NLPID encapsulation.
 *      Follows RFC1483/1294 for encapsulations.  ISO & IP have a
 *      straight encapsulation scheme.  Simple 2 byte NLPID is all that
 *      is necessary.  All other protocols require a SNAP frame after
 *      the NLPID with a real protocol PID as defined with SNAP.
 * OUTPUT/RETURN:
 * INPUT:
 * ASSUMPTIONS:
 */
void dxi_nlpid_encaps(paktype *pak, ushort dfa)
{
    atm_dxi_hdr *hdr;
    snap_hdr *snap;

	if (NLPID_DEF(pak->linktype)) {
	    pak->enctype = ET_ATM_DXI;
	    pak->encsize = ATM_DXI_ENCAPBYTES;
	    pak->datagramstart -= ATM_DXI_ENCAPBYTES;
	    pak->datagramsize += ATM_DXI_ENCAPBYTES;
	    hdr = (atm_dxi_hdr *)pak->datagramstart;
	    PUTSHORT(&hdr->dfa, (dfa | ATM_DXI_EA_BITS));
	    PUTSHORT(&hdr->type, link2nlpid(pak->linktype));
	    if (atm_dxi_debug_packet)
	    	dxi_debug_pak(pak, "NLPID header");
	} else {
	    pak->enctype = ET_ATM_DXI;
	    pak->encsize = ATM_DXI_SNAPENCAPBYTES;
	    pak->datagramstart -= ATM_DXI_SNAPENCAPBYTES;
	    pak->datagramsize += ATM_DXI_SNAPENCAPBYTES;
	    hdr = (atm_dxi_hdr *)pak->datagramstart;
	    PUTSHORT(&hdr->dfa, (dfa | ATM_DXI_EA_BITS));
	    PUTSHORT(&hdr->type, DXI_SNAP_NLPID);
	    snap = (snap_hdr *)hdr->snap_start;
	    snap->control = DXI_SNAP_CONTROL;
	    if (pak->linktype == LINK_APPLETALK)
	    	ieee_copy_oui(apple_snap, snap->oui);
	    else 
	    	ieee_copy_oui(zerosnap, snap->oui);
	    PUTSHORT(&snap->type, link2serial(pak->linktype));
	    if (atm_dxi_debug_packet)
	    	dxi_debug_pak(pak, "NLPID/SNAP header");
	}
}

/* 
 * A hastily thrown together debug routine that 
 * should eventually get removed. shankar 09/94
 */
void dxi_debug_pak (paktype *pak, char *string)
{
    short *temp1;
    int i, size;
    buginf("\n\n\n %s", string);
    buginf("\n datagram start %x, size %d, network %x refcount %d link %d, ",
	pak->datagramstart, pak->datagramsize, pak->network_start, 
	 pak->refcount, pak->linktype);
    buginf("\n");
    size = pak->datagramsize;
    temp1 = (short *)pak->datagramstart;
    /* convert short to char */
    if (size > 128)
    size = 128;
    size = size >> 1;
    for (i = 0; i < size; i++, temp1++)
    {
	buginf(" 0x%04x", GETSHORT(temp1));
	if ( (i>0) && !((i+1) % 8))
		buginf("\n");
    }
}

/*
 * ATM-DXI subsystem header
 */

#define ATMDXI_MAJVERSION 1
#define ATMDXI_MINVERSION 0
#define ATMDXI_EDITVERSION  1

SUBSYS_HEADER(atm_dxi, ATMDXI_MAJVERSION, ATMDXI_MINVERSION, ATMDXI_EDITVERSION,
	      atm_dxi_init, SUBSYS_CLASS_PROTOCOL, NULL, NULL);
