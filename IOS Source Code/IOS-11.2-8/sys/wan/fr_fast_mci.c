/* $Id: fr_fast_mci.c,v 3.3.28.4 1996/08/13 02:26:57 gstovall Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_fast_mci.c,v $
 *------------------------------------------------------------------
 * fr_fast_mci.c -- MCI and cBus IP Fast Switching Support
 *
 * 20-March-1988, Kirk Lougheed
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fr_fast_mci.c,v $
 * Revision 3.3.28.4  1996/08/13  02:26:57  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.3.28.3  1996/05/22  14:45:52  fbordona
 * CSCdi57997:  DLSw FST over WAN Frame Relay can not establish circuits
 *              Use reg_invoke_framerelay_fs() for frame-relay RXTYPE_DLSW
 *              Remove DLSw FST frame-relay code from interrupt routines.
 * Branch: California_branch
 *
 * Revision 3.3.28.2  1996/05/21  16:51:10  wmay
 * CSCdi58156:  frame relay ip fast switching tried on unknown snap paks
 * add missing break statement
 * Branch: California_branch
 *
 * Revision 3.3.28.1  1996/03/18  22:47:26  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.1  1996/03/13  02:11:54  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3  1996/01/19  22:16:58  wmay
 * CSCdi45344:  ACL: input access lists are ignored on SMDS subinterfaces
 * Add include file for serial_inline; add sniff buffer for dest address
 *
 * Revision 3.2  1995/11/17  18:02:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:49:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:35:20  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/09/26  17:32:02  ccpoe
 * CSCdi40819:  Spurious access in fr_fast_pvc_switch_or_fs. Should check
 * pvc_usage before accessing framerelay routing data structure.
 *
 * Revision 2.3  1995/08/01  16:59:40  ccpoe
 * CSCdi37323:  Frame relay not counting FECN/BECN/DE incoming when fast
 * switching. Since we don't set FECN/BECN bit unless the packet is
 * frame-relay switched, counting incoming FECN/BECN/DE suffices the
 * requirement.
 *
 * Revision 2.2  1995/06/26  17:13:09  wmay
 * CSCdi35439:  FR-IETF over AppleTalk broke - make sure sniff buffer is
 * restored after fast-switching calls.
 *
 * Revision 2.1  1995/06/07  23:17:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "subsys.h"
#include "fastswitch_registry.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/fddi.h"
#include "frame_relay.h"
#include "fr_switch.h"
#include "serial.h"
#include "ppp.h"
#include "smds.h"
#include "atm_dxi.h"
#include "../ip/ip.h"
#include "../ip/ipfast.h"
#include "../ip/ipfast_hes.h"
#include "../if/network.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "../if/serial_inline.h"


/*
 * fr_mci2mci
 * Fast switch FR packets between two different MCI controllers.
 * Called from fr_fast_set_subif_pvc_switch().
 * This is now inline, so it has a lot more  variables that were
 * set up in fr_fast_pvc_switch_or_fs
 */

static inline void fr_mci2mci (
			       hwidbtype *input,
			       hwidbtype *output,
			       mciregtype *srcreg,
			       fr_pvc_info_type *cptr,
			       ushort out_dlci,
			       ushort bytes,
			       fr_idb_struct_type *out_fr_idb,
			       fr_pvc_info_type *out_fr_pvc)
{
    mciregtype  *dstreg;

    /*
     * Set up the dstreg
     * Reserve a buffer in window 1 of the destination MCI
     */
    dstreg = (mciregtype *) output->mci_regptr;
    dstreg->argreg = output->mci_index;
    dstreg->cmdreg = MCI_CMD_SELECT;
    
    /*
     * The MCI doesn't care what the argreg value is -- the FCI requires that
     * it be the buffer pool number i.e. which pool to get buffer from.
     */
    dstreg->argreg = output->buffer_pool;
    dstreg->cmdreg = MCI_CMD_TX1_RESERVE;
    if (dstreg->cmdreg == MCI_RSP_OKAY) {
	dstreg->argreg = 0;
	dstreg->cmdreg = MCI_CMD_TX1_SELECT;
	dstreg->write1short = out_dlci;
	/*
	 * Copy bulk of datagram from one MCI to another.
	 * Transmit modified copy and flush original in receive buffer.
	 */
	srcreg->argreg = MCI_SERIAL_OFFSET + 1;
	srcreg->cmdreg = MCI_CMD_RX_SELECT;
	dstreg->argreg = bytes;
	mci2mci(&srcreg->readlong, &dstreg->write1long, (bytes - 2));
	dstreg->cmdreg = MCI_CMD_TX1_START;

        INC_PVC_SWITCHED(cptr);
	INC_PVC_FAST_INPUT(cptr, bytes, out_dlci);
	INC_PVC_FAST_OUTPUT(out_fr_pvc, bytes);

	input->counters.inputs_fast[ACCT_PROTO_FRS]++;
	input->counters.rx_cumbytes_fast[ACCT_PROTO_FRS] += bytes;
	output->counters.outputs_fast[ACCT_PROTO_FRS]++;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_FRS] += bytes;

    } else
	output->counters.output_total_drops++;

    /*
     * Flush the datagram.  We come here when we can't do the TX1_RESERVE
     * OR [fall through] after we have successfully copied the datagram
     * over the multibus and issued the start.
     */
    srcreg->cmdreg = MCI_CMD_RX_FLUSH;
    return;
}

/*
 * Initial fast switch routine that sets up the proper subinterface
 * pointer, and looks for PVC switching.  This routine is called for all
 * rxtypes prior to calling a network layer protocol specific routine.
 */

boolean fr_fast_pvc_switch_or_fs (hwidbtype *input)
{
    mciregtype *srcptr = input->mci_regptr;
    hwidbtype *output;
    ushort in_dlci, save_dlci, bytes, in_type, type;
    fr_idb_struct_type *fr_idb = input->frame_relay_stuff;
    fr_pvc_info_type *frpvc, *out_frpvc;
    fr_idb_struct_type *out_fridb;
    charlong sniff[3];
    charlong rif_sniff[3];
    boolean have_snap;
    /*
     * Be sure we have a valid struct before starting
     */
    if (!fr_idb)
	return(FALSE);

    /*
     * Lookup the DLCI to check the action to take
     */
    bytes = input->rxlength;
    srcptr->argreg = MCI_SERIAL_OFFSET;
    srcptr->cmdreg = MCI_CMD_RX_SELECT;
    sniff[0].d.lword = input->sniff[SNIFFBASE+0].d.lword = srcptr->readlong;
    in_dlci = input->sniff[SNIFFBASE+0].d.sword[0];
    in_type = input->sniff[SNIFFBASE+0].d.sword[1];

    save_dlci = DLCI_TO_NUM(in_dlci);
    if (save_dlci < MIN_DLCI || save_dlci > MAX_DLCI)
	goto READ_SNIFF_AND_FAIL;
    frpvc = fr_idb->fr_dlci_tab[save_dlci];
    if (!frpvc)
	goto READ_SNIFF_AND_FAIL;
    if ((!input->nokeepalive && !IS_PVC_ACTIVE_BIT_SET(frpvc->pvc_status)) || 
      frpvc->pvc_usage == PVC_SWITCHED_IP_TUNNEL)
	goto READ_SNIFF_AND_FAIL;

    if (frpvc->idb)
	input->fast_rcvidb = frpvc->idb; /* Note the receiving subinterface */
    else
	input->fast_rcvidb = input->firstsw;
    
    /*
     * The rest of this routine handles PVC switching.  Bail out now if
     * this PVC is used locally by the router.
     */
    if (frpvc->pvc_usage == PVC_LOCAL) {
	have_snap = FALSE;
	/*
	 * This can be removed if the MCI/SCI/Cbus is ever taught to
	 * classify Frame Relay packets.
	 */
        switch (in_type) {
	case FR_IETF_IP:
	    type = RXTYPE_IPUNC;
	    break;
	SERIAL_ARPA_TYPE_CASES;
	case FR_SNAP_NLPID:
	    sniff[1].d.lword =
		input->sniff[SNIFFBASE+1].d.lword =
		    srcptr->readlong;
	    sniff[2].d.sword[0] =
		input->sniff[SNIFFBASE+2].d.sword[0] =
		    in_type = srcptr->readshort;
	    if (input->sniff[SNIFFBASE+1].d.sword[1] == FR_OUI23_BRIDGE) {
                if ((in_type == FR1490_802_5) ||
                    (in_type == FR1490_802_5_FCS)) { 

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
 		    
 		    if (!input->fast_rcvidb->tbridge_on_swidb) {
 			return(FALSE);
 		    } else { 
 			
 			rif_sniff[0].d.lword = srcptr->readlong; 
 			/* pad, fc , first two octates of destination mac */
 
 			rif_sniff[1].d.lword = srcptr->readlong;
 			/* next four octates of destination mac */
 		
 			rif_sniff[2].d.sword[0] = srcptr->readshort;
 			/* first two octates of source mac */
 
 			if (rif_sniff[2].d.byte[0] & TR_RII) 
			    return(FALSE);
		    }
		}

		/*
		 * pass the input dlci number to the tbridge code
		 */
		input->sniff[SNIFFBASE+3].d.sword[0] = save_dlci;
		input->rxtype = LINK_BRIDGE;
		if (reg_invoke_tbridge_mci_forward(frpvc->idb)) {
		    INC_PVC_FAST_INPUT(frpvc, bytes, in_dlci);
		    return(TRUE);
		}
		return(FALSE);
	    }
	    have_snap = TRUE;
	    switch (in_type) {
	    default:
		type = RXTYPE_SNAP_UNK;
		break;
	    SERIAL_SNAP_TYPE_CASES;
	    }
	    break;
	case TYPE_BRIDGE:
	    /*
	     * pass the input dlci number to the tbridge code
	     */
	    input->sniff[SNIFFBASE+3].d.sword[0] = save_dlci;
	    if (reg_invoke_tbridge_mci_forward(frpvc->idb)) {
		INC_PVC_FAST_INPUT(frpvc, bytes, in_dlci);
		return(TRUE);
	    }
	    return(FALSE);
	    break;
	case FR_SNAP0PAD:
	    /* Fill in the rest of the sniff buffer for parse_packet */
	READ_SNIFF_AND_FAIL:
	    input->sniff[SNIFFBASE+1].d.lword = srcptr->readlong;
	    input->sniff[SNIFFBASE+2].d.sword[0] = srcptr->readshort;
	case FR_Q933_NLPID:
	default:
	    input->rxtype = RXTYPE_UNKNOWN;
	    return(FALSE);
	}
	/*
	 * For RXTYPE_DODIP, the next function, hdlc_ip_fastswitch(),
	 * expects RX_SELECT to be pointing at the IP Destination address
	 * for the IP route-cache lookup.  We've reset RX_SELECT in this
	 * code, so we have to put it back!
	 *
	 * In all cases, IP_FAST_STARTUP *EXPECTS* RX_SELECT to be set
	 * to the IP DSTADDR field of the IP header!!!!!!
	 */
	if (type == RXTYPE_DODIP) {
	    srcptr->argreg = MCI_OFFSETSIZE + MCI_IP_DSTADR_OFFSET;
	    srcptr->cmdreg = MCI_CMD_RX_SELECT;
	}
	input->rxtype = type;
	if (reg_invoke_framerelay_fs(type, input)) {
	    INC_PVC_FAST_INPUT(frpvc, bytes, in_dlci);
	    return(TRUE);
	}

	/*
	 * For RXTYPE_DODIP/TYPE_IP10MB, attempt to do special case ip
	 * fast switching. For now, only DLSw FST case is checked here, 
	 * but rsrb, udp flooding, etc... can be moved here from platform
	 * specific interrupt code. If special case ip fast switching 
	 * fails, reset the read offset to the start of the ip header.
	 */
	if ((type == RXTYPE_DODIP) && (in_type == TYPE_IP10MB)) {
	    srcptr->argreg = MCI_SERIAL_OFFSET + (sizeof(long)/2);
	    srcptr->cmdreg = MCI_CMD_RX_SELECT;
	    input->sniff[SNIFFBASE+1].d.sword[1] = srcptr->readshort;
	    input->sniff[SNIFFBASE+1].d.lword = srcptr->readlong;
	    input->sniff[SNIFFBASE+2].d.lword = srcptr->readlong;
	    if (input->sniff[SNIFFBASE+2].d.byte[3] == FST_DLSW_PROT) {
		if (reg_invoke_mci_dlsw_fst_forward(input)) {
		    INC_PVC_FAST_INPUT(frpvc, bytes, in_dlci);
		    return(TRUE);
		}
	    }

	    srcptr->argreg = MCI_SERIAL_OFFSET + (sizeof(long)/2);
	    srcptr->cmdreg = MCI_CMD_RX_SELECT;
	}

        input->sniff[SNIFFBASE+0].d.lword = sniff[0].d.lword;
        if (have_snap) {
            input->sniff[SNIFFBASE+1].d.lword = sniff[1].d.lword;
            input->sniff[SNIFFBASE+2].d.sword[0] = sniff[2].d.sword[0];
        }
	return(FALSE);
    }

    /*
     * Frame relay PVC switching
     */
    if (frpvc->pvc_usage != PVC_SWITCHED_SERIAL)
	goto READ_SNIFF_AND_FAIL;
    output = frpvc->fr_route.out_idb;
    out_fridb = output->frame_relay_stuff;
    if (!output || output->state != IDBS_UP || !is_frame_relay(output) ||
	!out_fridb)
	goto READ_SNIFF_AND_FAIL;

    /*
     * Ok.... We've got a legit Frame_Relay packet.  Let's see if we can
     * fastswitch this packet
     * Don't do the RX_MOVE if the datagram exceeds the
     * maximum output interface datagram size.
     * Search for next hop, if any.
     */
    if (bytes > output->max_pak_size)
	goto READ_SNIFF_AND_FAIL;

    if (!(out_frpvc = out_fridb->fr_dlci_tab[frpvc->fr_route.out_dlci]))
        goto READ_SNIFF_AND_FAIL;
    if (!IS_PVC_ACTIVE_BIT_SET(out_frpvc->pvc_status))
        goto READ_SNIFF_AND_FAIL;
    in_dlci = (in_dlci & FR_NON_DLCI_BITS);
    in_dlci |= NUM_TO_DLCI(frpvc->fr_route.out_dlci);

    /* 
     * If input MCI != output MCI then must move the buffer to the
     * new MCI before transmitting.
     */
    if (srcptr != output->mci_regptr) {
	fr_mci2mci(input, output, srcptr, frpvc,
		   in_dlci, bytes, out_fridb, out_frpvc);
	return(TRUE);
    }
    /*
     * We've got an output interface.... Let's switch this packet over.
     */
    srcptr->argreg = output->mci_index;
    srcptr->cmdreg = MCI_CMD_RX_MOVE;		/* Get transmit window */
    if (srcptr->cmdreg != MCI_RSP_OKAY) {
	srcptr->cmdreg = MCI_CMD_RX_FLUSH;
	output->counters.output_total_drops++;
	return(TRUE);		/* Couldn't get buf. */
    }

    srcptr->argreg = output->rxoffset;
    srcptr->cmdreg = MCI_CMD_TX1_SELECT;
    /* Check for a board encapsulation */
    if (output->cache_board_encap){
	ushort tempbuf;
	(*output->cache_board_encap)(output,&tempbuf,sizeof(ushort));
	srcptr->write1short = tempbuf;
    }
    srcptr->write1short = in_dlci;

    INC_PVC_SWITCHED(frpvc);
    INC_PVC_FAST_INPUT(frpvc, bytes, in_dlci);
    input->counters.inputs_fast[ACCT_PROTO_FRS]++;
    input->counters.rx_cumbytes_fast[ACCT_PROTO_FRS] += bytes;
    
    bytes += output->encsize - input->encsize;
    srcptr->argreg = bytes;
    srcptr->cmdreg = MCI_CMD_TX1_START;		/* Send it out */

    INC_PVC_FAST_OUTPUT(out_frpvc, bytes);
    output->counters.outputs_fast[ACCT_PROTO_FRS]++;
    output->counters.tx_cumbytes_fast[ACCT_PROTO_FRS] += bytes;

    return(TRUE);
}

/*
 * fr_mci_register_drivers
 *
 * This routine initializes the fast switching table for PVC switching.
 * The fast switching table for routed protocols is initialized in
 * fastswitch_init.c.
 */
static void fr_mci_register_drivers (ulong flags, hwidbtype *hwidb)
{
    if (hwidb->enctype != ET_FRAME_RELAY)
	return;
    reg_add_fr_pvc_switch_or_fs((service_fr_pvc_switch_or_fs_type)
				fr_fast_pvc_switch_or_fs,
				"fr_fast_pvc_switch_or_fs");
}

/*
 * fr_mci_init
 * Initialize Frame Relay fastswitching support
 */

static void fr_mci_init (subsystype* subsys)
{
    reg_add_register_drivers(fr_mci_register_drivers,
			     "fr_mci_register_drivers");
}

/*
 * MCI FR subsystem header
 */

#define FR_MCI_MAJVERSION 1
#define FR_MCI_MINVERSION 0
#define FR_MCI_EDITVERSION  1

SUBSYS_HEADER(frame_relay_mci,
	      FR_MCI_MAJVERSION, FR_MCI_MINVERSION, FR_MCI_EDITVERSION,
	      fr_mci_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      "req: fastswitch, frame_relay, iphost");
