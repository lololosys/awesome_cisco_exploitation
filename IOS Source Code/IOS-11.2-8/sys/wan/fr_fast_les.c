/* $Id: fr_fast_les.c,v 3.4.12.6 1996/08/13 02:26:54 gstovall Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_fast_les.c,v $
 *------------------------------------------------------------------
 * fr_fast_les.c -- c3000/c4000 Frame Relay Fast Switching Support
 *
 * 22-May-1990, Chris Shaker
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fr_fast_les.c,v $
 * Revision 3.4.12.6  1996/08/13  02:26:54  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.4.12.5  1996/05/22  23:57:00  pbettink
 * CSCdi57783:  Restoring Frame Relay minimum packet size to 4
 * Branch: California_branch
 *
 * Revision 3.4.12.4  1996/05/06  07:15:50  schaefer
 * CSCdi49868: crash at interrupt level on rsp2 (7513)
 * Branch: California_branch
 * ensure rxtype is set when deciding whether to fastswitch
 *
 * Revision 3.4.12.3  1996/04/25  23:23:19  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.4.12.2  1996/04/17  13:51:10  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.4.12.1  1996/03/18  22:47:24  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.2  1996/03/13  02:11:52  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.2.1  1996/02/20  21:50:53  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/13  08:17:34  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.3  1996/01/19  22:16:56  wmay
 * CSCdi45344:  ACL: input access lists are ignored on SMDS subinterfaces
 * Add include file for serial_inline; add sniff buffer for dest address
 *
 * Revision 3.2  1995/11/17  18:02:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:49:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:35:18  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/09/26  17:32:01  ccpoe
 * CSCdi40819:  Spurious access in fr_fast_pvc_switch_or_fs. Should check
 * pvc_usage before accessing framerelay routing data structure.
 *
 * Revision 2.2  1995/08/01  16:59:39  ccpoe
 * CSCdi37323:  Frame relay not counting FECN/BECN/DE incoming when fast
 * switching. Since we don't set FECN/BECN bit unless the packet is
 * frame-relay switched, counting incoming FECN/BECN/DE suffices the
 * requirement.
 *
 * Revision 2.1  1995/06/07  23:17:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "subsys.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "../hes/if_mci.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "frame_relay.h"
#include "fr_switch.h"
#include "serial.h"
#include "ppp.h"
#include "smds.h"
#include "atm_dxi.h"
#include "../ip/ip.h"
#include "../ip/ipfast.h"
#include "../if/network.h"
#include "media_registry.h"
#include "../if/serial_inline.h"
#include "../if/traffic_shape.h"


/*
 * Initial fast switch routine that sets up the proper subinterface
 * pointer, and looks for PVC switching.  This routine is called for all
 * rxtypes prior to calling a network layer protocol specific routine.
 */

boolean fr_fast_pvc_switch_or_fs (paktype *pak)
{
    hwidbtype *output, *input;
    ushort in_dlci, in_type, bytes, save_dlci;
    fr_idb_struct_type *fr_idb;
    fr_pvc_info_type *frpvc, *out_frpvc;
    fr_idb_struct_type *out_fridb;
    frame_relay_hdr * frame;
    tring_hdr *trh;

    input = pak->if_input->hwptr;
    fr_idb = input->frame_relay_stuff;
    /*
     * initialize this so early error return doesn't inherit the value
     * from the previous use of this paktype
     */
    pak->rxtype = RXTYPE_UNKNOWN;

    /*
     * Be sure we have a valid struct before starting
     */

    if (!fr_idb)
	return(FALSE);

    /*
     * Lookup the DLCI to check the action to take
     */
    bytes = pak->datagramsize;
    frame = (frame_relay_hdr *)pak->datagramstart;
    in_dlci = frame->dlci;
    in_type = frame->type;
    save_dlci = DLCI_TO_NUM(in_dlci);
    if (save_dlci < MIN_DLCI || save_dlci > MAX_DLCI)
	return(FALSE);
    frpvc = fr_idb->fr_dlci_tab[save_dlci];
    if (!frpvc)
	return(FALSE);
    if ((!input->nokeepalive &&!IS_PVC_ACTIVE_BIT_SET(frpvc->pvc_status)) ||
      frpvc->pvc_usage == PVC_SWITCHED_IP_TUNNEL)
	return(FALSE);

    /* Overwrite pak->if_input with the correct subinterface pointer. */

    if (frpvc->idb)
	pak->if_input = frpvc->idb;
    
    /*
     * The rest of this routine handles PVC switching.  Bail out now if
     * this PVC is used locally by the router.
     */
    if (frpvc->pvc_usage == PVC_LOCAL) {
	snap_hdr *snap;
	ushort type;
	fr_q933_hdr *q933;
	
	pak->mac_start  = (uchar *)frame;
	pak->addr_start = (uchar *)frame;
	/*
	 * Push short packets out of the FS path
	 */ 
	if (pak->datagramsize < FR_MIN_PKT_SIZE) {
	    pak->rxtype = RXTYPE_UNKNOWN;
	    return(FALSE); 
	}
	
	/*
	 * Have we got a frame with a Q933 NLPID?
	 * 
	 * If we do, then we can assume that there are 2 bytes of L2_PID
	 * and 2 bytes of L3_PID. Currently, the only L2 protocol
	 * supported is 802.2. (L2 pid value of 0x4c).
	 *
	 * We set network_start in here, but the llc code may need to
	 * adjust it later when it knows if the control field is 
	 * 1 byte or 2.
	 *
	 */
	if (frame->type == FR_Q933_NLPID) {
	    q933 = (fr_q933_hdr *)frame;
	    pak->info_start    = q933->data;
	    pak->network_start = pak->info_start + SAP_LONGHDRBYTES;
	    pak->encsize       = FRAME_RELAY_Q933_NLPID_SIZE;
	    type = RXTYPE_SNAP_UNK;
	} else if ((frame->type == FR_SNAP1PAD) ||
		   (frame->type == FR_SNAP0PAD)) {
	    int pad_adjust;
	    pad_adjust = 0;
	    if (frame->type == FR_SNAP0PAD) pad_adjust = 1;
	    snap = (snap_hdr *)((uchar *)frame->snap_start - pad_adjust);
	    pak->info_start    = frame->snap_start;
	    pak->network_start = snap->data;
	    pak->encsize       = FRAME_RELAY_SNAPENCAPBYTES - pad_adjust;

	    if (GETSHORT((ushort *)&snap->oui[1]) == FR_OUI23_BRIDGE) {
		/*
		 * Sniff buffer could be corrupted - don't use it again
		 * We pass the input dlci number using the sniff buffer.
		 * Saves us a parameter so we can use the same bridging
		 * entry routines as all the other interfaces
		 */
		input->sniff[SNIFFBASE+0].d.sword[0] = save_dlci;

                if ((GETSHORT(&snap->type) == FR1490_802_5) ||
                    (GETSHORT(&snap->type) == FR1490_802_5_FCS)) { 
 		    /* History for following piece of code: IBU added
 		     *  check for FR1490_802_5 and FR1490_802_5_FCS during 
 		     *  FRAS BAN feature commit in 11.1. Logic was, 
 		     *  if the frame format is 802_5, its for FRAS BAN.
 		     *  This broke transparent bridging as it also uses
 		     *  same format (CSCdi70653). Additional checking added
 		     *  to fix this problem. If transparent bridging is not
 		     *  configured on (sub)interface, send pak to process
 		     *  level (FRAS). Otherwise, check for RIF bit in
 		     *  source mac address. If set, again send pak to
 		     *  process level else send it for transparent
 		     *  bridging  processing. 
 		     */
 
 		    trh = (tring_hdr *) pak->network_start; 
 		    if ((!pak->if_input->tbridge_on_swidb) ||
 			(trh->saddr[0] & TR_RII)) {
			return(FALSE); 
		    } 
		}
		/*
		 * Protect against bridging killer packet fragments
		 * smaller than the encapsulation size
		 */
		if (bytes >= pak->encsize) {
		    if (reg_invoke_tbridge_forward(pak)) {
			INC_PVC_FAST_INPUT(frpvc, bytes, in_dlci);
			return(TRUE);
		    }
		}
		return(FALSE);
	    }
	    switch (GETSHORT(&snap->type)) {
		SERIAL_SNAP_TYPE_CASES;	/* FR also uses these ! */
	    default:
		type = RXTYPE_SNAP_UNK;
		break;
	    }
	} else {
	    pak->info_start    = (char *)NULL;
	    pak->network_start = frame->data;
	    pak->encsize       = FRAME_RELAY_ENCAPBYTES;
 
	    switch(frame->type) {
		SERIAL_ARPA_TYPE_CASES;	/* FR also uses these ! */
	    case FR_IETF_IP:
		type = RXTYPE_DODIP;
		break;
	    case TYPE_BRIDGE:
		input->sniff[SNIFFBASE+0].d.sword[0] = save_dlci;

		/*
		 * Protect against bridging killer packet fragments
		 * smaller than the encapsulation size
		 */
		if (bytes >= pak->encsize) {
		    if (reg_invoke_tbridge_forward(pak)) {
			INC_PVC_FAST_INPUT(frpvc, bytes, in_dlci);
			return(TRUE);
		    }
		}
		return(FALSE);
	    default:
		type = RXTYPE_UNKNOWN;
		break;
	    }

	    /*
	     * Fudgebyte revenge. We have to skip a packing byte between
	     * the end of the type field and the start of the CLNS fixed
	     * header. This is required for backwards compatibility.
	     */
	    if (type == RXTYPE_CLNS) {
		pak->network_start++;
		pak->encsize++;
	    }
	    pak->rxtype = type;
	}
	pak->rxtype = type;

	/*
	 * Protect against fast switching killer packet fragments
	 * smaller than the encapsulation size
	 */
	if (bytes > pak->encsize) {
	    if (reg_invoke_framerelay_fs(type, pak)) {
		INC_PVC_FAST_INPUT(frpvc, bytes, in_dlci);
		return(TRUE);
	    }
	}
	return(FALSE);
    }

    if (frpvc->pvc_usage != PVC_SWITCHED_SERIAL)
	return(FALSE);
    output = frpvc->fr_route.out_idb;
    out_fridb = output->frame_relay_stuff;
    if (!output || output->state != IDBS_UP || !is_frame_relay(output) ||
	!out_fridb)
	return(FALSE);

    /*
     * Ok.... We've got a legit Frame_Relay packet.  Let's see if we can
     * fastswitch this packet
     * Don't do the RX_MOVE if the datagram exceeds the
     * maximum output interface datagram size.
     * Search for next hop, if any.
     */
    if (bytes > output->max_pak_size)
	return(FALSE);

    if (!(out_frpvc = out_fridb->fr_dlci_tab[frpvc->fr_route.out_dlci]))
        return(FALSE);
    if (!IS_PVC_ACTIVE_BIT_SET(out_frpvc->pvc_status))
	return(FALSE);
    in_dlci = (in_dlci & FR_NON_DLCI_BITS);
    in_dlci |= NUM_TO_DLCI(frpvc->fr_route.out_dlci);

    frame = (frame_relay_hdr *)pak->datagramstart;
    /* Add the board encap before actually changing anything */
    if (output->board_encap) {
	if ((*output->board_encap)(pak, output) == FALSE) {
	    return(FALSE);
	}
    }
    frame->dlci = in_dlci;

    pak->acct_proto = ACCT_PROTO_FRS;

    pak->if_output = out_frpvc->idb? out_frpvc->idb: output->firstsw;
    pak->linktype = LINK_FR_SWITCH_SERIAL;
    pak->mac_start = pak->datagramstart;	/* set up for WFQ */
    (*output->fastsend)(output, (paktype *)pak);
     
    INC_PVC_SWITCHED(frpvc);
    INC_PVC_FAST_INPUT(frpvc, bytes, in_dlci);
    INC_PVC_FAST_OUTPUT(out_frpvc, bytes);

    input->counters.inputs_fast[ACCT_PROTO_FRS]++;
    input->counters.rx_cumbytes_fast[ACCT_PROTO_FRS] += bytes;

    return(TRUE);		
}

/*
 * fr_les_register_drivers
 *
 * This routine initializes the fast switching table for PVC switching.
 * The fast switching table for routed protocols is initialized in
 * fastswitch_init.c.
 */
static void fr_les_register_drivers (ulong flags, hwidbtype *hwidb)
{
    if (hwidb->enctype != ET_FRAME_RELAY)
	return;
    reg_add_fr_pvc_switch_or_fs((service_fr_pvc_switch_or_fs_type)
				fr_fast_pvc_switch_or_fs,
				"fr_fast_pvc_switch_or_fs");
}

/*
 * fr_les_init
 * Initialize Frame Relay fastswitching support
 */

static void fr_les_init (subsystype* subsys)
{
    reg_add_register_drivers(fr_les_register_drivers,
			     "fr_les_register_drivers");
}

/*
 * LES FR subsystem header
 */

#define FR_LES_MAJVERSION 1
#define FR_LES_MINVERSION 0
#define FR_LES_EDITVERSION  1

SUBSYS_HEADER(frame_relay_les,
	      FR_LES_MAJVERSION, FR_LES_MINVERSION, FR_LES_EDITVERSION,
	      fr_les_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      "req: fastswitch, frame_relay, iphost");

