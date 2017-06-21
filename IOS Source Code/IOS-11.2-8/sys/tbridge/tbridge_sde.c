/*-------------------------------------------------------------------------
 *
 * $Id: tbridge_sde.c,v 3.9.4.8 1996/09/10 02:38:27 hou Exp $
 *
 * tbridge/tbridge_sde.c
 *
 * October 1994, Martin McNealis.
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Definitions and functions for Transparently Bridged Virtual LANs.
 *
 * (a.k.a. IEEE 802.10 Secure Data Exchange (SDE)).
 * 
 *--------------------------------------------------------------------------
 * $Log: tbridge_sde.c,v $
 * Revision 3.9.4.8  1996/09/10  02:38:27  hou
 * CSCdi63727:  IP RIP routing doent work with 802.10 over FDDI
 * Branch: California_branch
 * - distinguish 802.10 vLAN routing packet from bridging packet so that
 *   the  multicast routing pkts can go to 802.10 vLAN routing stack.
 *
 * Revision 3.9.4.7  1996/08/21  20:37:32  hampton
 * Further isolate VLAN code from IPX internals.  [CSCdi66726]
 * Branch: California_branch
 *
 * Revision 3.9.4.6  1996/08/13  02:25:18  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.9.4.5  1996/06/24  23:36:42  hou
 * CSCdi61139:  vLAN 802.10 writes wrong ip mtu into nvram.
 * Branch: California_branch
 *
 * Revision 3.9.4.4  1996/06/01  01:32:46  hampton
 * Remove unnecessary cross module references.  [CSCdi59221]
 * Branch: California_branch
 *
 * Revision 3.9.4.3  1996/05/23  03:11:45  wilber
 * CSCdi58046:  Ill formatted 802.10(SDE) vLAN BPDUs
 * Remove the redundant code that lays down the SDE header
 * Branch: California_branch
 *
 * Revision 3.9.4.2  1996/04/11  17:05:02  hou
 * CSCdi53052:  multiple vlan id causing vlan tbridging and routing problem
 * Branch: California_branch
 *
 * Revision 3.9.4.1  1996/03/18  22:12:27  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.14.4  1996/03/16  07:39:39  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.14.3  1996/03/13  02:03:10  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.14.2  1996/03/07  10:52:06  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.14.1  1996/02/20  18:53:19  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/03/07  22:51:57  hou
 * CSCdi50237:  vLAN 802.10 failed to route IP packet with size 4455bytes
 * or larger
 * - set_802_10_sde_encap()
 *   donot change ip mtu size for vlan dot10 interface so that ip can
 *   fragment packet properly
 *
 * Revision 3.8  1996/02/05  03:35:18  mmcneali
 * CSCdi48150:  Cannot bridge between vLANs.
 *
 * Revision 3.7  1996/01/23  08:54:57  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 * Revision 3.6  1995/12/17  06:12:43  mmcneali
 * CSCdi43825:  No fastswitching for 802.10 vLAN packets.
 *
 * Revision 3.5  1995/12/04  21:14:58  mmcneali
 * CSCdi44882:  Failure to Dynamically Populate ARP cache breaks
 *              IP/ISL Links.
 *
 * Revision 3.4  1995/12/01  09:26:33  mmcneali
 * CSCdi44792:  Not allow same bridge-group to have different dot10 vLAN
 *              SAIDs.
 *
 *              In order to bridge different vLANs together, relax this
 *              restriction.
 *
 * Revision 3.3  1995/11/26  08:20:16  mmcneali
 * CSCdi43630:  ISL VLAN Multicast changed--IOS gs7 images are DEAF
 *
 *              When subinterface shutdown, do not remove it from the vLAN.
 *
 * Revision 3.2  1995/11/17  18:45:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:31:39  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/08/22  07:44:35  wilber
 * CSCdi38848:  disabling one SDE channel of many fails.
 * Remember to clear the ptr of SAID descriptor in the case of more than
 * one sub-interfaces have the same color(SAID) in the bridge-group
 *
 * Revision 2.2  1995/06/21  00:00:54  wilber
 * CSCdi36140:  tbridge code - set_802_10_sde_encap doesnt check malloc
 * return
 *
 * Revision 2.1  1995/06/07  23:03:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *--------------------------------------------------------------------------
 * $Endlog$
 *
 */


#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "ieee.h"
#include "packet.h"
#include "config.h"
#include "sys_registry.h"
#include "media_registry.h"

#include "../if/ether.h"
#include "../if/network.h"
#include "../srt/span.h"

#include "tbridge_sde.h"
#include "tbridge_sde_inline.h"

#include "../wan/serial.h"

#include "../vlan/vlan.h"

/*
 *  Table of pointers to the SAID descriptor structures
 *  hashed by Security Association ID.
 */

said_descriptor *said_table[MAX_SAIDS];


/*
 * Generic encapsulation routine which configures a subinterface for dot10
 * SDE encapsulation.
 *
 * Note currently only vLAN encapsulations are allowed on LAN interfaces.
 */

void set_802_10_sde_encap (parseinfo *csb)
{

hwidbtype        *hwidb;
idbtype          *swidb;
said_descriptor  *said_index, *previous_said;
encapstype	 encapsulation;
ulong            configured_said;
int              index;
int              subif_index;

    hwidb = csb->interface->hwptr;

    encapsulation = GETOBJ(int,1);

    configured_said = GETOBJ(int,2);

    /*
     * Supported on FDDI, Fast Ethernet, CBus Token Ring,
     * Ethernet and HDLC interfaces.
     *
     * Let's give decent error messages for this feature!
     */

    if (encapsulation != ET_SDE)
        return;

    if (!(is_sde_capable(csb->interface))) {
        printf("\n\nSDE encapsulation not supported on this interface.\n");
	return;
    }

    /*
     * To prevent packet duplication, only one subinterface per hardware
     * IDB can be configured to be within the same Virtual LAN.
     *
     * Nota Bene - this model breaks when we implement multiple vLAN
     *             membership per port.
     */

    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb)
    {
	if ((swidb->sde_said_db) 
            && ((swidb->sde_said_db->sdb_said == configured_said)
            && (swidb != csb->interface)))  {

            printf("\n\nConfiguration of multiple subinterfaces of the same "
                   "main\ninterface with the same SAID (%d) is not permitted."
                   "\nThis SAID is already configured on interface %s.\n", 
                   configured_said, swidb->namestring);
            return;
        }
    }

    if_maxdgram_adjust(hwidb, hwidb->max_buffer_size + SDE_ENCAPBYTES);

    /*
     * VLAN's can have larger than media size frames, take that into account.
     */
    if (hwidb->max_buffer_size > hwidb->max_pak_size) {
	hwidb->max_pak_size = hwidb->max_pak_size_def + SDE_ENCAPBYTES;
    }

    csb->interface->ip_routecache_same =
                                 csb->interface->ip_routecache_same_def = TRUE;

    /*
     * IPX can have subinterfaces independent of vLAN bridging/routing, so
     * if configured with a Novell network number then override any default
     * encapsulation.
     */
    reg_invoke_set_protocol_encaps(LINK_NOVELL, csb->interface, ET_SDE);

    /*
     * Write the SDE Security Association ID value into the SDE structure and
     * fill in the back pointer to the subinterface IDB it's configured on.
     *
     * Note we maintain an array, indexed by hardware interface number, of 
     * the software IDBs associated with this SAID value, so that when we
     * match a received 802.10 packet's SAID with those configured on the
     * received interface, we can then index the corresponding subinterface
     * IDB - which ultimately let's us get the packet to the correct bridge 
     * group!
     *
     * If SAID not already present then malloc the necessary space.
     * Assume entries are zeroed upon creation, so no need to initialize.
     */

    /*
     * Walk the linked list of Security Association IDs, typically there'll
     * only be one.
     *
     * Note we can do one of three things here:
     *
     * 1.  Add a new SAID to the hash table. Also add it to
     *     the subinterface software IDB.
     *
     * 2.  Link a new SAID structure to an existing chain.
     *
     * 3.  Add a subinterface to an existing SAID entry. i.e. another interface
     *     connecting to the Transit Backbone.
     *     Adding multiple subinterfaces to a virtual LAN is done via
     *     using the hardware IDBs interface index as a subscript into an
     *     array of MAX interfaces.
     */

    index = nethash(configured_said);          /* This should be an inline
						for the per-packet match */
    previous_said = NULL;
    for (said_index = said_table[index]; said_index; 
         said_index = said_index->next_said_db) {

    /* If we've found a SAID matching that entered */

	if (said_index->sdb_said == configured_said) {

            said_index->sdb_said = configured_said;
	    subif_index = csb->interface->hwptr->hw_if_index;
            said_index->sdb_sub_interface[subif_index] = csb->interface;
            csb->interface->sde_said_db = said_index;
	    said_index->sdb_number_subif_in_bgroup++;
	    return;
        }
        previous_said = said_index;
    }

    said_index = malloc(sizeof(said_descriptor));
    if (said_index == NULL)
	return;

    if (previous_said)
        previous_said->next_said_db = said_index;
    else 
        said_table[index] = said_index;        /* First on list */
    
    said_index->sdb_said = configured_said;
    subif_index = csb->interface->hwptr->hw_if_index;
    said_index->sdb_sub_interface[subif_index] = csb->interface;
    csb->interface->sde_said_db = said_index;
    said_index->sdb_number_subif_in_bgroup++;
}


/*
 * Deencapsulation routine for 802.10 SDE encapsulated interfaces.
 */

void deencap_802_10_sde (idbtype *swidb, boolean sense)
{

said_descriptor  *said_ptr, *previous_said;
spantype         *span;
int              index;
int              subif_index;

    span = swidb->span_ptr;

    /*
     * If no other vLAN encapsulation or dot10 SAID descriptor
     * on the interface, then we have nothing to do.
     */

    /* If other vLAN encapsulations is on the interface, we
     * have to do more clear up than just change the vLAN ID
     */ 
    if (swidb->isl_vlan != NULL)
        reg_invoke_deencap_isl_vlan(swidb, FALSE);

    if (swidb->sde_said_db == NULL)
        return;

    /* If dot10 vLAN interface configured for routing then tidy up ...
     *
     * If deleting 802.10 SDE encapsulation and subinterface bridging
     * present, remove the interface from the Bridge Group.
     * Cannot have subinterface bridging without SDE encapsulation.
     * Also if other interfaces in the same Bridge Group are using the
     * same SAID (i.e `colour') remove the interface; because we currently
     * do not support multiple colours in a single Bridge Group.
     */

    if (!sense)
        reg_invoke_deconfigure_vlan_switching(swidb);

    /*
     * In deencapsulating an SDE subinterface we have three scenarios:
     *
     * 1.  If the SAID descriptor is the first entry in the hash chain and it
     *     is only applied to this interface then we can blow it away; afterall
     *     each SAID descriptor requires > 1K. This is the case where the
     *     Network Administrator is deconfiguring the feature.
     *
     * 2.  If it's not the first entry in the hash chain then we need to tidy
     *     up the linked list.
     *
     * 3.  If the SAID is applied on other interfaces then just remove this
     *     software IDB from the array of interfaces.
     */

    index = nethash(swidb->sde_said_db->sdb_said);

    previous_said = NULL;

    for (said_ptr = said_table[index]; said_ptr; 
	 said_ptr = said_ptr->next_said_db) {

        if (said_ptr == swidb->sde_said_db) {

            if (said_ptr->sdb_number_subif_in_bgroup == 1) {

                /* 
                 * If not the initial entry in the hash chain then link
		 * previous and next, otherwise remove the whole said_table
		 * entry (if only one entry in the chain), or change the 2nd
		 * entry in the chain to the 1st one (if more than one entry
		 * in the chain).
                 */

	        if (previous_said != NULL)
		    previous_said->next_said_db = said_ptr->next_said_db;

		else 
		    said_table[index] = said_ptr->next_said_db;

                free(swidb->sde_said_db);
                swidb->sde_said_db = NULL;

                return;

	    /*
	     * There exist other interfaces belonging to this virtual
	     * LAN therefore just delete the reference to this 
	     * SWIDB in the SAID descriptor structure.
	     */

            } else {

                subif_index = swidb->hwptr->hw_if_index;
	        said_ptr->sdb_sub_interface[subif_index] = NULL;
		said_ptr->sdb_number_subif_in_bgroup--;
                swidb->sde_said_db = NULL;
		return;
            }
        }

        previous_said = said_ptr;
    }
}

/*
 * Write the 802.10 SDE encapsulation SAID to NV Memory.
 */
void dot10_sde_vlan_encaps_nvram_write (parseinfo *csb)
{
    if (csb->interface->sde_said_db) {
        nv_write(TRUE, "encapsulation sde %d",
                 csb->interface->sde_said_db->sdb_said);
    }
}

/*
 * This routine determines whether a Bridge Group contains any 802.10 SDE
 * encapsulating subinterfaces. If delete is TRUE these interfaces will
 * be removed from the Group - this results from configuration changes
 * which preclude SDE encapsulated bridging.
 */

boolean check_802_10_subif_bridging (spantype *span, boolean delete)
{

idbtype  *bridge_port;
int      index;
boolean  sde_bridging = FALSE;
boolean  once_flag = FALSE;

    for (index = 0; index < span->port_count; index++) {

        bridge_port = span->port[index];

        if (bridge_port->sde_said_db) {

            sde_bridging = TRUE;

            if (delete) {

                if (!once_flag) {

                    printf("\n\nTransparent Bridging configuration now"
                           " incompatible with interfaces within\n Bridge"
                           " Group %d which are configured for 802.10 SDE"
                           " encapsulation;\n therefore these interfaces"
                           " removed from the Bridge Group.\n",
                           span->spanindex);

                    once_flag = TRUE;
                }

                if (span->running)
                    span_port_disable(span, bridge_port);
                span_delete_port(bridge_port);

            }
        }
    }

    return (sde_bridging);
}


/*
 * sde_enqueue
 *
 * Enqueue a SDE packet to an approprate queue after parsing the SDE header.
 * This is for process-switched packets such as broadcast and BPDU.
 */

void sde_enqueue (paktype *pak)
{
    snap_hdr *snap;
    int info_size;
    ulong sde_result;

    /*
     * We get here because 'pak->linktype == LINK_SDE'.
     */
    sde_result = tbridge_parse_sde_header_inline(pak);
    if (SDE_DISCARD == sde_result) {
        retbuffer(pak);
        return;
    }

    /* 
     * Since tbridge_parse_sde_header_inline is called
     * twice(one in  tbridge_crb_inline, one in sde_enqueue)
     * for sde routing broadcast pkts, we have to change
     * pak->infor_start here, but not in tbridge_parse_sde_header
     * to avoid changing pak->infor_start twice.
     */

    if (SDE_SUBIF_ROUTE == sde_result)
        pak->info_start += SDE_ENCAPBYTES;

    snap = (snap_hdr *)pak->info_start;
    
    /*
     * We may get a SDE packet without adjusting the pak->info_start
     * to point to the native LLC after parsing the SDE header because we 
     * want to simply bridge it on major swidb. That means bridge itself 
     * won't have any interest in the SDE header - it's llc data in this
     * case.
     * So instead of trying to look into the lsap of the native LLC
     * to figure out what queue this packet should enqueue to,
     * set link type to LINK_BRIDGE explicitly in order to enqueue it to 
     * bridgeQ and bridge it.
     */

    if (SAP_SDE == snap->dsap) {
	pak->linktype = LINK_BRIDGE;
	info_size = SDE_ENCAPBYTES;
    } else {
	/*
	 * Move the MAC portion down by SDE_ENCAPBYTES bytes to 'extract' 
	 * the SDE header.
	 * pak->info_start is already adjusted by SDE-header parsing 
	 * routine to point to native LLC.
	 * Adjust pak->datagramstart, pak->addr_start.
	 */
         sde_extract_header_inline(pak);
	/*
	 * Use switch for future expansion of specific sap.
	 */
	switch (snap->dsap) {
	case SAP_SNAP:
	    pak->enctype = ET_SNAP;
	    info_size = SNAP_HDRBYTES;
	    pak->linktype = type2link(snap->type);
	    break;
	    
	default:
	    pak->enctype = ET_SAP;
	    info_size = SAP_HDRBYTES;
	    pak->linktype = sap2link(snap->dsap);
	    break;
	}
    }

    /*
     * What STP process wants is pak->network_start.
     */
    pak->network_start = pak->info_start + info_size;

    reg_invoke_raw_enqueue(pak->linktype, pak);
}


/*
 * Construct necessary header for IEEE BPDU going out on hdlc-sde swidb.
 * Called when tx BPDU onto interface with swidb->tbridge_media =
 * TBR_HDLC_TRANSIT_SDE.
 *
 * Normal HDLC-encap BPDU's format:
 *
 * 8F00 4242 BPDU
 *
 * where hdlc-type is 4242(TYPE_IEEE_SPANNING).
 * It doesn't have any MAC header since it intends to talk to the box
 * at the end of HDLC link without any consideration it will go beyond
 * that box.
 * With the introduction of the 802.10 SDE encapsulation, it is possible
 * we want to bridge a SDE-encaped(colored) BPDU instead of receiving it. 
 * Without MAC header, it's not possible to bridge it. So we will construct 
 * a BPDU with MAC header,
 *
 * 8F00 6558 STP-multi SA(MAC of bridgeID) len SDE-hdr 424203 BPDU
 *
 * hdlc-type is 6558(TYPE_BRIDGE) to indicate that it's a bridging packet
 * with hdlc encapsulation. 
 */

void tbridge_construct_hdlc_sde_bpdu_header (paktype *pak)

{
    uchar* datagram_start;
    vhdlctype *hdlc;
    ether_hdr* ether;
    bpduhdrtype* bpdu;
    sap_hdr* sap;

    datagram_start = pak->datagramstart;
    pak->datagramstart -= (ETHER_SAP_ENCAPBYTES + SDE_ENCAPBYTES);
    pak->datagramsize += (ETHER_SAP_ENCAPBYTES + SDE_ENCAPBYTES);

    /* Move HDLC header */

    bcopy(datagram_start, pak->datagramstart, sizeof(vhdlctype));

    hdlc = (vhdlctype *) pak->datagramstart;

    /* 
     * Note that we Overwrite hdlc-type TYPE_IEEE_SPANNING with
     * TYPE_BRIDGE since now we want to treat it as a normal bridging
     * packet.
     */
    PUTLONG((uchar *)&hdlc->var_hdlcflags, HDLC_BRIDGECODE);

    /* STP multicast as DA */
    ether = (ether_hdr *)hdlc->data;
    ieee_copy(ieee_span_address, ether->daddr);

    /* MAC portion of BridgeID as SA */
    bpdu = (bpduhdrtype*)(datagram_start + sizeof(vhdlctype));
    ieee_copy((uchar *)&bpdu->bridge_id[2], ether->saddr);

    PUTSHORT(&ether->type_or_len, pak->datagramsize - (sizeof(vhdlctype) +
						       ETHER_8023_ENCAPBYTES));

    tbridge_bfr_sde_header_wr_inline(pak->if_output, ether->data, ether->saddr);
    
    /* STP reserved lsap */
    sap = (sap_hdr *)(ether->data + SDE_ENCAPBYTES);
    PUTSHORT((uchar *)sap, link2serial(pak->linktype));
    sap->control = LLC1_UI;
    return;
}


/*
 * show_transparently_bridged_vlans
 *
 * Display those transparently bridging interfaces which are configured as
 * part of a Virtual LAN - i.e. 802.10 SDE encapsulation containing a Virtual
 * LAN `colour' (or SAID (Security Association ID) as dot10 calls it); or
 * Inter Switch Link (ISL) encapsulating subinterface(s) ...
 */

void show_transparently_bridged_vlans (parseinfo *csb)
{

spantype         *span;
idbtype          *vlan_idb;
int              index, found_vlan_intf, bridge_int, transit_int, vlan_int;
boolean          onceflag = FALSE;

    automore_enable(NULL);

    for (index = 1; index < SPAN_MAXINDEX; index++) {

        span = spanarray[index];

	/*
	 * If we have a Transparent Bridging Group, check to see if it contains
	 * any SDE or ISL Virtual LAN encapsulated interfaces.
	 *
	 * To facilitate display, run through all interfaces in the Bridge
	 * Group pulling out vLAN Transit Backbone encapsulating subinterfaces,
	 * then show the End Subnet interfaces assigned to this Group. 
	 */

        if (span != NULL) {
	    
	    found_vlan_intf = FALSE;

            for (bridge_int = 0; bridge_int < span->port_count; bridge_int++) {

		if (IS_VLAN_CONFIGURED_SUBINTERFACE(span->port[bridge_int])) {

		    found_vlan_intf = TRUE;
		    break;
		}
	    }

	    if (found_vlan_intf == TRUE) {

		onceflag = TRUE;

		/* 
	         * Found a bridging interface configured for vLAN
		 * encapsulation so display its member Bridge Group.
		 */

                printf("\n\nBridge Group:  %d", span->spanindex);

                printf("\n\n   Virtual LAN Trunking Interface(s):    vLAN "
		       "Protocol:    vLAN ID:    State:\n");

		for (transit_int = 0; transit_int < span->port_count;
		     transit_int++) {


		    if (IS_DOT10_VLAN_SUBINTERFACE(span->port[transit_int])) {

		        vlan_idb = span->port[transit_int];

			printf("\n          %29s", vlan_idb->namestring);

			printf("   IEEE 802.10          ");
			printf("%6d ", vlan_idb->sde_said_db->sdb_said);

			printf("%s", (interface_up(vlan_idb) ?
			bstate2string(vlan_idb->port_state) : "  down"));

		    } else if (IS_ISL_VLAN_SUBINTERFACE(span->port[transit_int])) {

		        vlan_idb = span->port[transit_int];

			printf("\n          %29s", vlan_idb->namestring);

			printf(" Inter Switch Link     ");
			printf("%6d ", vlan_idb->isl_vlan->colour);

			printf("%s", (interface_up(vlan_idb) ?
			bstate2string(vlan_idb->port_state) : "   down"));
		    }
		}

                printf("\n\n   Virtual LAN Native Interface(s):      State:\n");

                for (vlan_int = 0; vlan_int<span->port_count; vlan_int++) {

		    if (IS_VLAN_CONFIGURED_SUBINTERFACE(span->port[vlan_int])
			== FALSE) {

                        vlan_idb = span->port[vlan_int];

                        printf("\n          %29s", vlan_idb->namestring);
                        printf("%s", (interface_up(vlan_idb) ?
                        bstate2string(vlan_idb->port_state) : "   down"));
		    }
		}
            }
	}
    }

    if (onceflag == FALSE)
        printf("\n\nNo Transparently Bridged Virtual LANs configured.\n");
    else
	printf("\n");

    automore_disable();
}


/*
 * Called if configuring a bridging subinterface for IEEE 802.10 Virtual LANs.
 *
 * Check that media is LAN or HDLC Serial Interface, 802dot10 VLANs currently
 * only supported on Ethernet/Fast Ethernet, Token Ring, FDDI and HDLC links.
 *  
 * Also subinterface vLAN bridging only works in conjunction with the
 * IEEE 802.1d version of our friend the Spanning Tree.
 *
 * If SDE encapsulation already configured on interfaces within an
 * existing Bridge Group then a new interface can only be added to that
 * same Group if is has the same vLAN ID (Colour). In other words each
 * Bridge Group can only have a single colour associated with it.
 *
 * A subinterface can only support one kind of vLAN interface.
 */
	    
boolean dot10_capable_subinterface (idbtype *input_swidb, spantype *span,
                                    int index)
{
    int spanindex;
    idbtype *swidb;

    if (input_swidb->sde_said_db == NULL) {

       printf("\n\nBridging on a LAN or HDLC subinterface requires"
              " that the interface be already\nconfigured for 802.10"
              " SDE Encapsulation with a Security AssociationID.\n");
       return (FALSE);
    }

    else if (span->protocol != SPAN_PROTO_IEEE) {

        printf("\n\nBridging on a subinterface only supported"
               "\nin conjunction with the IEEE Spanning Tree.\n");
        return (FALSE);
    }

    else {

        FOR_ALL_SWIDBS(swidb)
	{
	    spanindex = idb_sw_get_spanindex(swidb);
            if ((swidb->sde_said_db != NULL)
	        && (swidb->sde_said_db == input_swidb->sde_said_db)
                && (spanindex != 0) && (spanindex != index)) {

                   printf("\n\n802.10 Security Association ID %d already"
                          " in use within Bridge Group %d.\n",
                          swidb->sde_said_db->sdb_said, spanindex);
                   return (FALSE);
	    }
        }
    }

    /*
     *  Now set the tbridge_media on the input subinterface SWIDB  - used
     *  by the switching logic to build the SDE encapsulated packet.
     */

    switch (input_swidb->hwptr->firstsw->tbridge_media) {

	case TBR_ETHER:
            input_swidb->tbridge_media = TBR_ETHER_SDE; break;

        case TBR_FDDI:
            input_swidb->tbridge_media = TBR_FDDI_SDE; break;

        case TBR_CTR:
            input_swidb->tbridge_media = TBR_CTR_SDE; break;

	case TBR_TR:
            input_swidb->tbridge_media = TBR_TR_SDE; break;

        case TBR_HDLC_TRANSIT:
            input_swidb->tbridge_media = TBR_HDLC_TRANSIT_SDE; break;

        default:
   	    input_swidb->tbridge_media = TBR_UNKNOWN; break;
    }

    return (TRUE);
}
