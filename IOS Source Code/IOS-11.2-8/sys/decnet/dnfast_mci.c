/* $Id: dnfast_mci.c,v 3.3.12.5 1996/08/14 02:36:15 hampton Exp $
 * $Source: /release/112/cvs/Xsys/decnet/dnfast_mci.c,v $
 *------------------------------------------------------------------
 * dnfast_mci.c -- DECnet fast switching support for MCI/CBUS interfaces.
 *
 * 25-December-1988, Kirk Lougheed
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dnfast_mci.c,v $
 * Revision 3.3.12.5  1996/08/14  02:36:15  hampton
 * Use static inline routines to clean up references to the access list
 * array.  [CSCdi65925]
 * Branch: California_branch
 *
 * Revision 3.3.12.4  1996/07/22  23:07:09  asastry
 * CSCdi62165:  DECnet fast-switching should be able to use DECnet default
 * route
 * Branch: California_branch
 *
 * Revision 3.3.12.3  1996/05/17  23:12:22  rbadri
 * CSCdi57499:  DECnet fastswitching from serial->lane broken
 * Branch: California_branch
 * Issue the select command before writing the LANE encapsulation.
 * Adjust the datagram size to account for the encapsulation.
 *
 * Revision 3.3.12.2  1996/05/09  14:12:29  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.12.1.18.1  1996/04/27  06:34:41  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.12.1.8.1  1996/04/08  01:45:17  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.3.12.1  1996/03/18  19:11:48  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.16.1  1996/03/22  09:35:27  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3  1996/02/13  19:44:20  hampton
 * Reorder routines to remove platform specific extern statements in
 * fastswitching code.  [CSCdi48844]
 *
 * Revision 3.2.48.1  1996/03/05  05:49:21  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.40.1  1996/02/27  20:41:02  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.14.1  1996/02/13  00:17:25  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * DECnet routing for LANE
 *
 * Revision 3.2  1995/11/17  08:59:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:16:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/23  18:53:59  asastry
 * CSCdi38977:  DECnet static routes breaks decnet fast switching
 *
 * Revision 2.2  1995/07/10  05:32:28  fred
 * CSCdi36850:  commit fast path queuing for appletalk, decnet, etc.
 *
 * Revision 2.1  1995/06/07  20:23:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "packet.h"
#include "access.h"
#include "../if/ether.h"
#include "config.h"
#include "../wan/serial.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../if/fddi.h"
#include "decnet_externs.h"
#include "dnconversion.h"
#include "access.h"
#include "../if/network.h"
#include "../if/atm.h"
#include "../lane/lane.h"
#include "../hes/if_aip.h"

/*
 * Forward definition(s)
 */
  
/*
 * This structure will fool the compiler and force it to keep *data* and
 * *dnptr* in the same register, a performance consideration for high speed
 * switching.
 */
typedef struct reg_ {
    union {
	uchar *data;
	dndatapacket *dnptr;
    } u;
} reg;

/*
 * Bit position -- performance consideration.
 */
#define DECNET_MAPPED	1


/*
 * Small inline functions to force the compiler to put code where we want it.
 */
inline static void dn_write_ether_header (
    mciregtype *regptr,
    ushort gateway,
    hwidbtype *output)
{
    regptr->write1long = DN_MAC_CANONICAL;
    regptr->write1short = gateway;
    regptr->write1long = DN_MAC_CANONICAL;
    regptr->write1short = GETSHORT(&output->hardware[4]);
}
inline static void dn_write_fddi_header (
    mciregtype *regptr,
    ushort gateway,
    hwidbtype *output)
{
    regptr->write1short = FDDI_LLC_FC_BYTE;
    if (output->type == IDBTYPE_FDDIT) {
	regptr->write1long = DN_MAC_CANONICAL;
	regptr->write1short = gateway;
	regptr->write1long = *((ulong *) &output->hardware[0]);
	regptr->write1short = GETSHORT(&output->hardware[4]);
    } else {
	regptr->write1long = DN_MAC_SWAPPED;
	regptr->write1short = bit_swaps[gateway & 0xFF] |
	    (bit_swaps[(gateway >> 8)] << 8);
	regptr->write1long = *((ulong *) &output->bit_swapped_hardware[0]);
	regptr->write1short = GETSHORT(&output->bit_swapped_hardware[4]);
    }
    regptr->write1long = 0xAAAA0300;
    regptr->write1short = 0;
}
inline static void dn_write_serial_header (
    mciregtype *regptr,
    hwidbtype *output)
{

    if (output->cache_board_encap) {
	ushort tempbuff;
	(*output->cache_board_encap)(output,&tempbuff,sizeof(ushort));
	regptr->write1short = tempbuff;
    }
    regptr->write1short = HDLC_STATION;
}

    
/*
 * |-------|-------|-------|-------|
 * | flags | -     | -     | AA    |
 * | 00    | 04    | 00    | dst   |
 * | dst   | -     | -     | AA    |
 * | 00    | 04    | 00    | src   |
 * | src   | ce    | visit |       |
 * |       |       |       |       | 
 * 
 * |-------|-------|-------|-------|
 * | pad   | flags | -     | -     | 
 * | AA    | 00    | 04    | 00    | 
 * | dst   | dst   | -     | -     | 
 * | AA    | 00    | 04    | 00    | 
 * | src   | src   | ce    | visit |
 * |       |       |       |       | 
 * 
 * The field farthest into the datagram we normally look at is the 
 * visits field. This means we have to read in 20 bytes (with 1 byte 
 * of DECnet padding) or 19 bytes, if the 1 byte is not present.
 * If we're in interim path split mode we have to read 2 more
 * longwords.
 * 
 * The following ASSUMPTIONS are made
 *
 *   1) MCI_OFFSETSIZE is where the DECnet input data starts, if
 *      the input media is FDDI, then DECnet input data starts at
 *      MCI_OFFSETSIZE + ISO2_FDDI_OVERHEAD.
 *
 *   2) DECnet data consists of 2 bytes of length field, followed by
 *      an optional 1 byte PAD and then the FLAGS. We don't care about
 *      these two (2) bytes i.e. we don't read them into memory.
 *
 *   3) We read the DECnet data into memory starting at SNIFFBASE+0.
 *
 *   4) Given these assumptions, the FLAGS field is in MCI_OFFSETSIZE + 1,
 *      even byte boundary, if the optional PAD is not present, or
 *      odd byte boundary if the PAD is not present.
 *
 *   5) The visits field is in MCI_OFFSETSIZE + 1 + 9, even byte boundary
 *      if the optional PAD is NOT present, odd byte boundary if the
 *      the PAD is present.
 *
 *   6) If PAD is present
 *             FLAGS is at SNIFFBASE+0.d.byte[1].
 *		   DEST is at SNIFFBASE+2.d.byte[0 and 1]
 *		   SRC is at SNIFFBASE+4.d.byte[0 and 1]
 *             visits is at SNIFFBASE+4.d.byte[3].
 *
 *   7) If PAD is NOT present
 *             FLAGS is at SNIFFBASE+0.d.byte[0].
 *		   DEST is at SNIFFBASE+1.d.byte[3]
 *		          and SNIFFBASE+2.d.byte[0]
 *		   SRC is at SNIFFBASE+3.d.byte[3]
 *		         and SNIFFBASE+4.d.byte[0]
 *             visits is at SNIFFBASE+4.d.byte[2].
 *
 *   8) SNIFFBASE+0.d.byte[0] and SNIFFBASE+0.d.byte[1] are in the
 *      same MCI short word, so the offset is MCI_OFFSETSIZE + 1 (short
 *      word) to read or write the FLAGS.
 *   
 *   9) SNIFFBASE+4.d.byte[3] and SNIFFBASE+4.d.byte[2] are in the
 *      same MCI short word, so the OFFSET is MCI_OFFSETSIZE + 10 to
 *      read or write the visit field.
 * 
 *   9.1) Since we now sometimes update the byte which contains
 *      the congestion encountered (ce) bit and since we always 
 *      write the short that contains the visits byte, now
 *      we will always write the longword that contains both the
 *      cf byte and the visits byte.
 *
 *  10) When address mapping we have to rewrite SRC and DEST.
 *      These are unfortunately NOT in the same short word when
 *      the pads are different.  
 *
 * Since the DEC implementations use only one byte of padding, we
 * optimize our code by switching only datagrams with a single pad byte.
 */

    
#define MCI_DEC_FLAGS (MCI_OFFSETSIZE + 1)
#define MCI_DEC_VISITS (MCI_OFFSETSIZE + 9)
#define MCI_DEC_DEST (MCI_OFFSETSIZE + 4)
#define MCI_DEC_SRC (MCI_OFFSETSIZE + 8)
#define ISO2_FDDI_OVERHEAD 	3
#define MCI_DEC_LANE_OVERHEAD   8 /* btow (LECID + ARPA encaps) */

/*
 * dn_mci2mci
 * Fast switch DECnet packets between two different MCI controllers.
 * Called from dn_fastswitch().
 */
static boolean dn_mci2mci (
    hwidbtype *input,
    hwidbtype *output,
    ulong gateway,
    dndatapacket *dnptr,
    ushort mapped)
{
    mciregtype *dstreg, *srcreg;
    ushort temp, bytes;
    ulong encap_words;
    ulong input_interface, output_interface;
    
    dstreg = output->mci_regptr;
    
    /*
     * Reserve a buffer in window 1 of the destination MCI
     */
    dstreg->argreg = output->mci_index;
    dstreg->cmdreg = MCI_CMD_SELECT;
    dstreg->argreg = output->buffer_pool;
    dstreg->cmdreg = MCI_CMD_TX1_RESERVE;
    if (dstreg->cmdreg != MCI_RSP_OKAY)
	return(FALSE);
    
    /*
     * Set up write pointer to base of destination transmit window.
     */
    dstreg->argreg = 0;
    dstreg->cmdreg = MCI_CMD_TX1_SELECT;
    
    /*
     * Do some accounting of packets and bytes in and out.
     */
    dn_stat[DNST_RECEIVED]++;
    dn_stat[DNST_DATAS]++;
    dn_stat[DNST_FORWARD]++;
    srcreg = input->mci_regptr;
    bytes = input->rxlength;
    input->counters.rx_cumbytes_fast[ACCT_PROTO_DECNET] += bytes;
    input_interface = input->status;
    input->counters.inputs_fast[ACCT_PROTO_DECNET]++;
    output->counters.tx_cumbytes_fast[ACCT_PROTO_DECNET] += bytes;
    output_interface = output->status;
    output->counters.outputs_fast[ACCT_PROTO_DECNET]++;
    temp = bytes;

    if (output_interface & IDB_ETHER) {
	encap_words = ETHER_ARPA_ENCAPBYTES >> 1;
	if (input_interface & IDB_ETHER) { 
	    
            /*
             * Input was ETHERNET, adjust the input offset to point at the
             * protocol type and adjust the number of bytes to transfer.
	     */
	    dn_write_ether_header(dstreg, gateway, output);
	    srcreg->argreg = MCI_ETHER_OFFSET + ((ETHER_ARPA_ENCAPBYTES-2) >> 1);
	    temp -= (ETHER_ARPA_ENCAPBYTES-2);
	} else if (input_interface & IDB_FDDI) {
	    
	    /*
	     * Input was FDDI.
	     */
	    srcreg->argreg = (FDDI_RECEIVE_OFFSET + 
			      (((FDDI_SNAP_ENCAPBYTES - 2) >> 1) + 1));
	    temp -= (FDDI_SNAP_ENCAPBYTES - 2);
	    dn_write_ether_header(dstreg, gateway, output);
	    bytes -= (FDDI_SNAP_ENCAPBYTES - ETHER_ARPA_ENCAPBYTES);
	    if (bytes < MINETHERSIZE)
		bytes = MINETHERSIZE;
	} else { /* input_interface & IDB_SERIAL */
	    /*
	     * Input was HDLC, adjust the input offset to point at the protocol
	     * type and adjust the number of bytes to transfer and the number
	     * of bytes in the output datagram.
	     */
	    dn_write_ether_header(dstreg, gateway, output);
	    temp -= (HDLC_ENCAPBYTES - 2);
	    bytes += (ETHER_ARPA_ENCAPBYTES - HDLC_ENCAPBYTES);
	    if (bytes < MINETHERSIZE)
	        bytes = MINETHERSIZE;
	    srcreg->argreg = MCI_SERIAL_OFFSET + ((HDLC_ENCAPBYTES - 2) >> 1);
	}

    } else if (output_interface & IDB_FDDI) {
	encap_words = ((FDDI_SNAP_ENCAPBYTES >> 1) + 1);

	/*
	 * Output FDDI.
	 */
	if (input_interface & IDB_ETHER) {
	    srcreg->argreg = MCI_ETHER_OFFSET + ((ETHER_ARPA_ENCAPBYTES - 2) >> 1);
	    bytes += (FDDI_SNAP_ENCAPBYTES - ETHER_ARPA_ENCAPBYTES);
	    temp -= (ETHER_ARPA_ENCAPBYTES - 2);
	    dn_write_fddi_header(dstreg, gateway, output);
	    bytes |= 0x4000;	/* starts on odd byte boundary */
	} else if (input_interface & IDB_FDDI) {
	    dn_write_fddi_header(dstreg, gateway, output);
	    srcreg->argreg = FDDI_RECEIVE_OFFSET + (((FDDI_SNAP_ENCAPBYTES - 2)
						    >> 1) + 1);
	    bytes |= 0x4000;	/* starts on odd byte boundary */
	    temp -= (FDDI_SNAP_ENCAPBYTES - 2);
	} else { /* input_interface & idb_serial */
	    srcreg->argreg = MCI_SERIAL_OFFSET + ((HDLC_ENCAPBYTES - 2) >> 1);
	    bytes += (FDDI_SNAP_ENCAPBYTES - HDLC_ENCAPBYTES);
	    dn_write_fddi_header(dstreg, gateway, output);
	    temp -= (HDLC_ENCAPBYTES - 2);
	    bytes |= 0x4000;	/* starts on odd byte boundary */
	    
	}
    } else { /* output_interface & IDB_SERIAL */
	encap_words = HDLC_ENCAPBYTES >> 1;
	
	/*
	 * Output is Serial, advance RX pointer and maybe write HDLC header
	 */
	if (input_interface & IDB_ETHER) {
	    srcreg->argreg = MCI_ETHER_OFFSET + (ETHER_ARPA_ENCAPBYTES >> 1);
	    dstreg->write1long = HDLC_DECCODE;
	    temp -= ETHER_ARPA_ENCAPBYTES;
	    bytes -= (ETHER_ARPA_ENCAPBYTES - HDLC_ENCAPBYTES);
	} else if (input_interface & IDB_FDDI) {
	    srcreg->argreg = FDDI_RECEIVE_OFFSET + ((FDDI_SNAP_ENCAPBYTES >> 1)
						    + 1);
	    dstreg->write1long = HDLC_DECCODE;
	    temp -= FDDI_SNAP_ENCAPBYTES;
	    bytes -= (FDDI_SNAP_ENCAPBYTES - HDLC_ENCAPBYTES);
	} else { /* input_interface & IDB_SERIAL */
	    srcreg->argreg = MCI_SERIAL_OFFSET;
	}
    }
    srcreg->cmdreg = MCI_CMD_RX_SELECT;
    
    /*
     * Copy bulk of datagram from one MCI to another.
     */
    mci2mci(&srcreg->readlong, &dstreg->write1long, temp);
    srcreg->cmdreg = MCI_CMD_RX_FLUSH;
    
    /*
     * Now update the long that contains the visits byte and congestion
     * encountered bit.
     */
    dstreg->argreg = encap_words + (MCI_DEC_VISITS - MCI_OFFSETSIZE);
    dstreg->cmdreg = MCI_CMD_TX1_SELECT;
    dstreg->write1long = *((ulong *)&input->sniff[SNIFFBASE+4].d.sword[0]);

    /*
     * Always clear the local bit unless packet is going back out
     * the same interface.
     */
    if (dnptr->typecode & DN_LOCAL)
	goto dn_local;
 return_dn_local:
    
    /*
     * If mapped we have to rewrite the SRC and DEST address bytes.
     */
    if (mapped)
	goto dn_mapped;
 return_dn_mapped:


    /*
     * Finally -- dispatch the datagram.
     */
    dstreg->argreg = 0;
    dstreg->cmdreg = MCI_CMD_TX1_SELECT;
    dstreg->argreg = bytes;
    dstreg->cmdreg = MCI_CMD_TX1_START;
    return(TRUE);

    /*
     * ========================================================================
     *
     * This function has been organized so that the EXPECTED mainline path will
     * will never take a branch. The code is not particularily readable but
     * this sometime happens when performance is the main concern.
     *
     * =======================================================================
     */

 dn_local:
    if (input != output) {
	dnptr->typecode &= ~DN_LOCAL;
        dstreg->argreg = encap_words + (MCI_DEC_FLAGS - MCI_OFFSETSIZE);
	dstreg->cmdreg = MCI_CMD_TX1_SELECT;
	dstreg->write1short = input->sniff[SNIFFBASE+0].d.sword[0];
    }	
    goto return_dn_local;

 dn_mapped:	
    dstreg->argreg = encap_words + (MCI_DEC_DEST - MCI_OFFSETSIZE);
    dstreg->cmdreg = MCI_CMD_TX1_SELECT;
    dstreg->write1long = *((ulong *) &input->sniff[SNIFFBASE+1].d.sword[1]);
    dstreg->argreg = encap_words + (MCI_DEC_SRC - MCI_OFFSETSIZE);
    dstreg->cmdreg = MCI_CMD_TX1_SELECT;
    dstreg->write1long = *((ulong *) &input->sniff[SNIFFBASE+3].d.sword[1]);
    goto return_dn_mapped;
}

/*
 * dn_fastswitch
 * High speed switching of Ethernet, HDLC , ATM-LANE or FDDI DECnet datagrams.
 * This routine has been very heavily optimized for speed, since it is in
 * the inner loop of the code that handles back-to-back datagrams.  Do not
 * add permanent debugging code, subroutines, or much of anything else.
 * The GNU CC compiler will generate the best code.
 * Speedups *must* be debugged with an oscilloscope.
 */

boolean dn_fastswitch (hwidbtype *input)
{
    mciregtype *regptr;
    mciregtype *mci_regptr;	/*identical to regptr -- performance kludge */
    hwidbtype *output;
    idbtype *swoutput;
    ulong ultemp;
    uchar firstbyte, uctemp;
    register dnroutetype *thisroute;
    dnroutetype **routeptr;
    reg p;
    dnnet *net;
    ushort mapped, gateway, ustemp;
    ulong output_interface, input_interface;
    dnacctype *acl;
    dnaccitemtype *list;
    vc_info_t *vc;
    ushort    *encap_info;

    vc = NULL;
    
    if (!(net = dn_nets[input->fast_rcvidb->dn_network]))
	return(FALSE);
    
    mci_regptr = input->mci_regptr;
    input_interface = input->status;
    if (input_interface & IDB_FDDI)
	mci_regptr->argreg = MCI_OFFSETSIZE + 1 + ISO2_FDDI_OVERHEAD + input->rxrifwordlen;
    else if (input_interface & IDB_ATM) {
	    vc = input->atm_db->fast_vc_info; 
	    if (is_atm_lane(vc->swidb)) {
		mci_regptr->argreg = (MCI_OFFSETSIZE + 1 + 
				      MCI_DEC_LANE_OVERHEAD);
	    } else {
                return (FALSE);
	    }
    } else {
	mci_regptr->argreg = MCI_OFFSETSIZE + 1;
    }
    mci_regptr->cmdreg = MCI_CMD_RX_SELECT;
    
    /*
     * WARNING.  If we're in "path-split-mode interim" we will need
     * to read 1 more long into the sniff buffer.  The assumption
     * is that we will be able to read them without setting up the 
     * MCI to do the read again.  Be careful if you change the code 
     * between here and there.
     */
    input->sniff[SNIFFBASE+0].d.lword = mci_regptr->readlong;	/* FLAGS */
    input->sniff[SNIFFBASE+1].d.lword = mci_regptr->readlong;
    input->sniff[SNIFFBASE+2].d.lword = mci_regptr->readlong;
    input->sniff[SNIFFBASE+3].d.lword = mci_regptr->readlong;
    input->sniff[SNIFFBASE+4].d.lword = mci_regptr->readlong;	/* visits */
    input->sniff[SNIFFBASE+5].d.lword = mci_regptr->readlong;	/* databyte */
    
    /*
     * Set up pointer to base of datagram.
     * Ensure that we have zero or one byte of DECnet padding.
     */
    p.u.data = (uchar *) &input->sniff[SNIFFBASE+0].d.byte[0];
    firstbyte = *p.u.data;
    if (firstbyte & DN_PAD) {
        if (firstbyte != (DN_PAD + 1))
	    return(FALSE);
	firstbyte = *(++p.u.data);
    }
    
    /*
     * Don't fast switch control or not-long packets.
     */
    firstbyte ^= DN_LONG;
    if (firstbyte & (DN_CTL+DN_LONG))
	return(FALSE);
    
    /*
     * Determine destination route. This code exists in dn_get_route() --
     * placing it inline is a performance consideration.
     */
    ultemp = dn_area(p.u.dnptr->dest);
    if (net->my_dn_area == ultemp) {
	ultemp = dn_node(p.u.dnptr->dest);
	routeptr = &net->dnroutes[ultemp];
    } else
	if (net->doareas)
	    routeptr = &net->dnareas[ultemp];
	else
	    routeptr = &net->dnroutes[0];
    
    
    /*
     * If no route, send to process level if return is requested. If return
     * is NOT requested, drop the datagram and exit.
     */
    if (!(thisroute = *routeptr)) {
	/*
	 * Check for bridging DECNet on this interface before
	 * consigning the packet for possible return, conversion,
	 * or discarding.
	 */
	if (BRIDGING_RXTYPE(input->firstsw, RXTYPE_DECNET))
	    return(FALSE);
	input->counters.inputs_fast[ACCT_PROTO_DECNET]++;
	input->counters.rx_cumbytes_fast[ACCT_PROTO_DECNET] += input->rxlength;
	goto dn_check_retreq;
    }

    /*
     * If the next hop type is "PhaseV", send to process level so that it
     * can be sent to CLNS.
     */
    if (thisroute->type == PHASEV)
	goto dn_check_retreq;
	
    /*
     * If routing to another network/area -- find the correct network entry.
     */
    mapped = 0;
    uctemp = thisroute->flags;
    if (uctemp & DN_RT_NET)
	goto dn_check_atg;
    
 return_dn_check_atg:
    if (uctemp & DN_RT_ME)	/* if addressed to us */
	return(FALSE);

    
    /*
     * Check hop count.
     */
    uctemp = p.u.dnptr->visits;
    if (uctemp >= (uchar)net->dn_max_visits)
	goto dn_check_retreq;
    uctemp++;
    p.u.dnptr->visits = uctemp;
    
    /*
     * Select possible multipath route 
     */
    if (thisroute->nextsplit)
	goto dn_nextsplit;

 return_dn_nextsplit:
    if (!(swoutput = thisroute->interface)) {
	/*
	 * If we have a static route that is not an interface static
	 * route (i.e. of the form 'dec route a.b x.y'), send packet
	 * to process level for processing.
	 */
	if (thisroute->type == PHASEIV_STATIC)
	    return(FALSE);

	goto dn_check_retreq;
    }
    output = swoutput->hwptr;

    /*
     * For ATM interfaces, send to process level if LANE is not
     * configured on this sub-interface.
     */
    if ((output->status & IDB_ATM) ) {
	if ( !is_atm_lane(thisroute->interface)) {
	    return (FALSE);
	}
        if (thisroute->cache_version != 
	    swoutput->cache_version)
            return (FALSE);
    }
    /*
     * If HELLO for router, drop the datagram if return
     * not requested, else send to process level.
     */
    uctemp = thisroute->flags;
    if (uctemp  & (DN_RT_ROUTER | DN_RT_NET))
	goto dn_check_retreq;
    
    /*
     * If switching disabled -- send to process level.
     */

    if (!output->dn_fastokay )
	return(FALSE);
    
    /*
     * Check for congestion encountered.  For DECnet this means
     * we set the congestion encountered bit if there are any packets
     * in the queue ahead of us. If mci_txqloc is set, we know
     * how to read the transmit queue length for this particular
     * hardware. If not, we can't check for congestion.
     */
    if (output->mci_txqloc) {
	regptr = output->mci_regptr;
	if (output->status & IDB_MCI) {
	    regptr->argreg = output->mci_txqloc;
	    regptr->cmdreg = MCI_CMD_MEMA_SELECT;
	    regptr->cmdreg = MCI_CMD_MEMA_READ;
	    if ((int)regptr->argreg > (int)output->dn_congest_limit) {
		p.u.dnptr->next_level_2 |= DN_RTFL2_CON;
		dn_stat[DNST_CONGEST]++;   /* DEBUG ONLY. ??? REMOVE LATER. */
	    }
	} 
#ifdef notdef			    /* can't select memd at interrupt level */
	else if (output->status & IDB_CBUS) {
	    regptr->argreg = output->mci_txqloc;
	    regptr->cmdreg = FCI_CMD_MEMD_SELECT;
	    if (regptr->readshort < output->mci_txcount) {
		p.u.dnptr->next_level_2 |= DN_RTFL2_CON;
		dn_stat[DNST_CONGEST]++;   /* DEBUG ONLY. ??? REMOVE LATER. */
	    }
	}
#endif
    }

    /*
     * If access list(s) are present, run datagram by them.
     */
    if (thisroute->interface->dn_accesslist)
	goto dn_access_check;
    
 return_dn_access_check:
    if (output->output_qcount && output->priority_list) {
	/* if sorting traffic and interface is congested, process switch */
	return FALSE;
    }
    regptr = input->mci_regptr;
    if (regptr == output->mci_regptr) {

	
	/*
	 * Output interface is on this MCI.
	 * Move RX buffer from receive window to transmit window 1.
	 * This may fail if interface output queue limit has been exceeded.
	 */
	ustemp = input->rxlength;
	regptr->argreg = output->mci_index;
	regptr->cmdreg = MCI_CMD_RX_MOVE;
	
	/*
	 * The RX_MOVE command is slow -- do some accounting before
	 * checking the results of the command.
	 */
	input->counters.inputs_fast[ACCT_PROTO_DECNET]++;
	input->counters.rx_cumbytes_fast[ACCT_PROTO_DECNET] += ustemp;
	dn_stat[DNST_RECEIVED]++;
	dn_stat[DNST_DATAS]++;
	if (regptr->cmdreg == MCI_RSP_OKAY) {
	    dn_stat[DNST_FORWARD]++;
	    output->counters.outputs_fast[ACCT_PROTO_DECNET]++;
	    output->counters.tx_cumbytes_fast[ACCT_PROTO_DECNET] += ustemp;
	    
	    /*
	     * Always clear the local bit unless packet is going back out
	     * the same interface.
	     */
	    if (p.u.dnptr->typecode & DN_LOCAL)
		goto dn_check_local;
	    
	    /*
	     * If mapped, we have to rewrite SRC and DEST address bytes.
	     */
	return_dn_check_local:
	    if (mapped)
		goto dn_mapped;
	    
	return_dn_mapped:
	    output_interface = output->status;
	    gateway = *(ushort *) thisroute->gateway;
	    
	    /*
	     * Select offset and write new MAC header to MCI buffer.
	     */
	    if (output_interface & IDB_ETHER) {
		if (input_interface & IDB_ETHER) {
		    regptr->argreg = MCI_DEC_VISITS;
		    regptr->cmdreg = MCI_CMD_TX1_SELECT;
		    regptr->write1long = 
			     *((ulong *)&input->sniff[SNIFFBASE+4].d.sword[0]);
		    regptr->argreg = MCI_ETHER_OFFSET;
		    regptr->cmdreg = MCI_CMD_TX1_SELECT;
		    dn_write_ether_header (regptr, gateway, output);
		    regptr->argreg = ustemp;	
		    regptr->cmdreg = MCI_CMD_TX1_START;
		    return(TRUE);
		} else if (input_interface & IDB_FDDI) {
		    ustemp -= (FDDI_SNAP_ENCAPBYTES - ETHER_ARPA_ENCAPBYTES);
		    regptr->argreg = MCI_DEC_VISITS + ISO2_FDDI_OVERHEAD;
		    regptr->cmdreg = MCI_CMD_TX1_SELECT;
		    regptr->write1long = 
			     *((ulong *)&input->sniff[SNIFFBASE+4].d.sword[0]);
		    regptr->argreg = MCI_ETHER_OFFSET + ISO2_FDDI_OVERHEAD;

		    regptr->cmdreg = MCI_CMD_TX1_SELECT;
		    dn_write_ether_header (regptr, gateway, output);
		    if (ustemp < MINETHERSIZE)
			ustemp = MINETHERSIZE;
		    regptr->argreg = ustemp;
		    regptr->cmdreg = MCI_CMD_TX1_START;
		    return(TRUE);
		} else if (input_interface & IDB_ATM) {
		    if (is_atm_lane(vc->swidb)) {
			regptr->argreg = (MCI_DEC_VISITS +
					  MCI_DEC_LANE_OVERHEAD);
			regptr->cmdreg = MCI_CMD_TX1_SELECT;
			regptr->write1long = 
			    *((ulong *)&input->sniff[SNIFFBASE+4].d.sword[0]);
			/*
			 * For LANE DA starts at MCI_OFFSETSIZE + 1
			 */
			regptr->argreg = (MCI_ETHER_OFFSET +
					  MCI_DEC_LANE_OVERHEAD);
			regptr->cmdreg = MCI_CMD_TX1_SELECT;
			dn_write_ether_header (regptr, gateway, output);
			ustemp -= ATM_AAL5_LANE_ENCAPBYTES;
			if (ustemp < MINETHERSIZE)
			    ustemp = MINETHERSIZE;
			regptr->argreg = ustemp;	
			regptr->cmdreg = MCI_CMD_TX1_START;
		        return(TRUE);
		    }
		    return (FALSE);

		} else {	/* input interface & idb_serial */
		    ustemp += ETHER_ARPA_ENCAPBYTES - input->encsize;
		    regptr->argreg = MCI_DEC_VISITS;
		    regptr->cmdreg = MCI_CMD_TX1_SELECT;
		    regptr->write1long = 
			     *((ulong *)&input->sniff[SNIFFBASE+4].d.sword[0]);
		    regptr->argreg = MCI_ETHER_OFFSET;
		    regptr->cmdreg = MCI_CMD_TX1_SELECT;
		    dn_write_ether_header(regptr, gateway, output);
		    if (ustemp < MINETHERSIZE)
			ustemp = MINETHERSIZE;
		    regptr->argreg = ustemp;
		    regptr->cmdreg = MCI_CMD_TX1_START;
		    return(TRUE);
		}
	    } else if (output_interface & IDB_FDDI) {
		if (input_interface & IDB_ETHER) {
		    ustemp += (FDDI_SNAP_ENCAPBYTES - ETHER_ARPA_ENCAPBYTES);
		    regptr->argreg = MCI_DEC_VISITS;
		    regptr->cmdreg = MCI_CMD_TX1_SELECT;
		    regptr->write1long = 
			     *((ulong *)&input->sniff[SNIFFBASE+4].d.sword[0]);
		    regptr->argreg = (FDDI_RECEIVE_OFFSET -
				      ISO2_FDDI_OVERHEAD);
		    regptr->cmdreg = MCI_CMD_TX1_SELECT;
		    dn_write_fddi_header(regptr, gateway, output);
		    regptr->argreg = ustemp | 0x4000;
		    regptr->cmdreg = MCI_CMD_TX1_START;
		    return(TRUE);
		} else if (input_interface & IDB_FDDI) {
		    regptr->argreg = MCI_DEC_VISITS + ISO2_FDDI_OVERHEAD;
		    regptr->cmdreg = MCI_CMD_TX1_SELECT;
		    regptr->write1long = 
			     *((ulong *)&input->sniff[SNIFFBASE+4].d.sword[0]);
		    regptr->argreg = FDDI_RECEIVE_OFFSET;
		    regptr->cmdreg = MCI_CMD_TX1_SELECT;
		    dn_write_fddi_header(regptr, gateway, output);
		    regptr->argreg = ustemp | 0x4000;
		    regptr->cmdreg = MCI_CMD_TX1_START;
		    return(TRUE);
		} else if (input_interface & IDB_ATM) { 
		    if (is_atm_lane(vc->swidb)) {
			ustemp += (FDDI_SNAP_ENCAPBYTES - 
				   ETHER_ARPA_ENCAPBYTES -
				    ATM_AAL5_LANE_ENCAPBYTES);
			regptr->argreg = (MCI_DEC_VISITS + 
					  MCI_DEC_LANE_OVERHEAD);
			regptr->cmdreg = MCI_CMD_TX1_SELECT;
			regptr->write1long = 
			    *((ulong *)&input->sniff[SNIFFBASE+4].d.sword[0]);
			/*
			 * For lane, DA/SA starts at MCI_OFFSETSIZE +1
			 * So, choose the argreg far enough to write
			 * FC(1 short word) DA (3) SA (3) oui (0xAAAA0300)
			 * oui (0x00)
			 */
			regptr->argreg = (MCI_OFFSETSIZE -
					  ISO2_FDDI_OVERHEAD);
			regptr->cmdreg = MCI_CMD_TX1_SELECT;
			dn_write_fddi_header(regptr, gateway, output);
			regptr->argreg = ustemp | 0x4000;
			regptr->cmdreg = MCI_CMD_TX1_START;
			return(TRUE);
		    }
		    else {
		      return (FALSE);
		    }
		} else { /* input_interface & idb_serial */
		    ustemp += (FDDI_SNAP_ENCAPBYTES - input->encsize);
		    regptr->argreg = MCI_DEC_VISITS;
		    regptr->cmdreg = MCI_CMD_TX1_SELECT;
		    regptr->write1long = 
			     *((ulong *)&input->sniff[SNIFFBASE+4].d.sword[0]);
		    regptr->argreg = FDDI_RECEIVE_OFFSET -
			ISO2_FDDI_OVERHEAD;
		    regptr->cmdreg = MCI_CMD_TX1_SELECT;
		    dn_write_fddi_header(regptr, gateway, output);
		    regptr->argreg = ustemp | 0x4000;
		    regptr->cmdreg = MCI_CMD_TX1_START;
		    return(TRUE);
		}
  	    } else if (output_interface & IDB_ATM) {
		if (is_atm_lane(thisroute->interface)) {
		    if (input_interface & IDB_ETHER) {
			regptr->argreg = MCI_DEC_VISITS;
			regptr->cmdreg = MCI_CMD_TX1_SELECT;
			regptr->write1long = 
			    *((ulong *)&input->sniff[SNIFFBASE+4].d.sword[0]);
			regptr->argreg = MCI_ETHER_OFFSET;
			regptr->cmdreg = MCI_CMD_TX1_SELECT;
			dn_write_ether_header (regptr, gateway, output);
			regptr->argreg = (MCI_ETHER_OFFSET - 
					  ATM_AAL5_LANE_ENCAPWORDS);
			regptr->cmdreg = MCI_CMD_TX1_SELECT;
		    } else if (input_interface & IDB_FDDI) {
			ustemp -= (FDDI_SNAP_ENCAPBYTES - 
				   ETHER_ARPA_ENCAPBYTES);
			regptr->argreg = MCI_DEC_VISITS + ISO2_FDDI_OVERHEAD;
			regptr->cmdreg = MCI_CMD_TX1_SELECT;
			regptr->write1long = 
			    *((ulong *)&input->sniff[SNIFFBASE+4].d.sword[0]);
			regptr->argreg = MCI_ETHER_OFFSET+ISO2_FDDI_OVERHEAD;
			regptr->cmdreg = MCI_CMD_TX1_SELECT;
			dn_write_ether_header (regptr, gateway, output);
			regptr->argreg = (MCI_ETHER_OFFSET +
					  ISO2_FDDI_OVERHEAD -
					  ATM_AAL5_LANE_ENCAPWORDS);
			regptr->cmdreg = MCI_CMD_TX1_SELECT;


		    } else if (input_interface & IDB_ATM) {
			if (is_atm_lane(vc->swidb)) {
			    ustemp -= ATM_AAL5_LANE_ENCAPBYTES;
			    regptr->argreg = (MCI_DEC_VISITS +
					      MCI_DEC_LANE_OVERHEAD);
			    regptr->cmdreg = MCI_CMD_TX1_SELECT;
			    regptr->write1long = 
				*((ulong *)&input->sniff[SNIFFBASE+4].d.sword[0]);
			    regptr->argreg = (MCI_ETHER_OFFSET +
					      MCI_DEC_LANE_OVERHEAD);
			    regptr->cmdreg = MCI_CMD_TX1_SELECT;
			    dn_write_ether_header (regptr, gateway, output);
			    regptr->argreg = output->rxoffset;
			    regptr->cmdreg = MCI_CMD_TX1_SELECT;
			    
			}
			else {
			    return (FALSE);
			}
		    } else {
			ustemp += (ETHER_ARPA_ENCAPBYTES - 
				   input->encsize +
				   ATM_AAL5_LANE_ENCAPBYTES);
			regptr->argreg = MCI_DEC_VISITS;
			regptr->cmdreg = MCI_CMD_TX1_SELECT;
			regptr->write1long = 
			    *((ulong *)&input->sniff[SNIFFBASE+4].d.sword[0]);
			regptr->argreg = MCI_ETHER_OFFSET;
			regptr->cmdreg = MCI_CMD_TX1_SELECT;
			dn_write_ether_header(regptr, gateway, output);
			regptr->argreg = (MCI_ETHER_OFFSET - 
					  ATM_AAL5_LANE_ENCAPWORDS);
			regptr->cmdreg = MCI_CMD_TX1_SELECT;
		    }
		    /*
		     * Write the ATM header + LECID. For each of the input
		     * interface, we would have written the DA/SA and selected
		     * the point at which VCD should be written. 
		     */
		    encap_info = ((ushort *)thisroute->encap_info);
		    regptr->write1short = *encap_info++;
		    regptr->write1short = *encap_info++;
		    regptr->write1short = *encap_info;
		    ustemp += ATM_AAL5_LANE_ENCAPBYTES;
		    if (ustemp < MINETHERSIZE + ATM_AAL5_LANE_ENCAPBYTES)
			ustemp = MINETHERSIZE + ATM_AAL5_LANE_ENCAPBYTES;
		    regptr->argreg = ustemp;
		    regptr->cmdreg = MCI_CMD_TX1_START;
		    return (TRUE);
		} else {
                    return (FALSE);
		}
		
	    } else {    /* output_interface & IDB_SERIAL */
		if (input_interface & IDB_ETHER) {
		    ustemp -= ETHER_ARPA_ENCAPBYTES;
		    ustemp += output->encsize;
		    regptr->argreg = MCI_DEC_VISITS;
		    regptr->cmdreg = MCI_CMD_TX1_SELECT;
		    regptr->write1long = 
			*((ulong *)&input->sniff[SNIFFBASE+4].d.sword[0]);

		    regptr->argreg = MCI_OFFSETSIZE - btow(output->encsize);
		    regptr->cmdreg = MCI_CMD_TX1_SELECT;
		    dn_write_serial_header(regptr,output);
		    regptr->argreg = ustemp;
		    regptr->cmdreg = MCI_CMD_TX1_START;
		    return(TRUE);
		} else if (input_interface & IDB_FDDI) {
		    ustemp -= (FDDI_SNAP_ENCAPBYTES - output->encsize);
		    regptr->argreg = MCI_DEC_VISITS + ISO2_FDDI_OVERHEAD;
		    regptr->cmdreg = MCI_CMD_TX1_SELECT;
		    regptr->write1long = 
			     *((ulong *)&input->sniff[SNIFFBASE+4].d.sword[0]);
		    regptr->argreg = MCI_OFFSETSIZE - btow(output->encsize) + 
			ISO2_FDDI_OVERHEAD;
		    regptr->cmdreg = MCI_CMD_TX1_SELECT;
		    dn_write_serial_header(regptr,output);
		    regptr->argreg = ustemp;
		    regptr->cmdreg = MCI_CMD_TX1_START;
		    return(TRUE);
		} else if (input_interface & IDB_ATM) {
		    if (is_atm_lane(vc->swidb)) {
			regptr->argreg = (MCI_DEC_VISITS +
					  MCI_DEC_LANE_OVERHEAD);
			regptr->cmdreg = MCI_CMD_TX1_SELECT;
			regptr->write1long = 
			    *((ulong *)&input->sniff[SNIFFBASE+4].d.sword[0]);
			/*
			 * For LANE DA starts at MCI_OFFSETSIZE + 1
			 */
			regptr->argreg = (MCI_OFFSETSIZE + 
					  MCI_DEC_LANE_OVERHEAD -
					  btow(output->encsize));
			regptr->cmdreg = MCI_CMD_TX1_SELECT;
			dn_write_serial_header (regptr, output);
			ustemp -= (ATM_AAL5_LANE_ENCAPBYTES +
				   ETHER_ARPA_ENCAPBYTES -
				   output->encsize);
			regptr->argreg = ustemp;	
			regptr->cmdreg = MCI_CMD_TX1_START;
			return (TRUE);
		    }
		    else {
			return (FALSE);
		    }

		} else {	/* (input_interface & IDB_SERIAL) */
		    ustemp += output->encsize - input->encsize;
		    regptr->argreg = MCI_DEC_VISITS;
		    regptr->cmdreg = MCI_CMD_TX1_SELECT;
		    regptr->write1long = 
			     *((ulong *)&input->sniff[SNIFFBASE+4].d.sword[0]);
		    regptr->argreg = MCI_OFFSETSIZE - btow(output->encsize);
		    regptr->cmdreg = MCI_CMD_TX1_SELECT;
		    dn_write_serial_header(regptr,output);
		    regptr->argreg = ustemp;
		    regptr->cmdreg = MCI_CMD_TX1_START;
		    return(TRUE);	    
		}
	    }
	} else if (output->priority_list) {
	    /* if sorting traffic and interface is congested, process switch */
	    return FALSE;
	} else {
	    
	    /*
	     * RX_MOVE failed -- discard the datagram and bump our counter.
	     */
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;
	    output->counters.output_total_drops++;
	    return(TRUE);
	}
    } else {
	
	/* 
	 * The output interface is across the multibus -- call dn_mci2mci().
	 */
	if (((mcitype *)output->devctl)->tx1bug == FALSE) {
	    gateway = *(ushort *) thisroute->gateway;
	    if (dn_mci2mci(input, output, gateway, p.u.dnptr, mapped))
		return(TRUE);
	}
	return(FALSE);
    }
    /*
     * ========================================================================
     *
     * This function has been organized so that the EXPECTED mainline path will
     * will never take a branch. The code is not particularily readable but
     * this sometime happens when performance is the main concern.
     *
     * =======================================================================
     */
    
 dn_access_check:
    
    /*
     * Access lists present -- evaluate them.  Pass NSP (re-)connect 
     * initiate packets to process level.
     */
    if ((p.u.dnptr->databyte == NSP_CONNECT) ||
	(p.u.dnptr->databyte == NSP_RECONNECT))
        return(FALSE);

    ultemp = thisroute->interface->dn_accesslist;
    if ((acl = dn_get_acl(ultemp)) != NULL) {
	list = acl->normal;
    } else {
	list = NULL;
    }
    if (list && !dn_accesscheck(p.u.dnptr, list))
	goto dn_check_retreq;

    goto return_dn_access_check;
    
    
 dn_mapped:
    
    /*
     * We have changed the DECnet source and destination address,
     * update the memory  in the MCI/cBus board.
     */
    if (input_interface & IDB_FDDI) {
	regptr->argreg = MCI_DEC_DEST + ISO2_FDDI_OVERHEAD;
	regptr->cmdreg = MCI_CMD_TX1_SELECT;
	regptr->write1long = 
	    *((ulong *) &input->sniff[SNIFFBASE+1].d.sword[1]);
	regptr->argreg = MCI_DEC_SRC + ISO2_FDDI_OVERHEAD;
    } else if (input_interface & IDB_ATM) {
	if (is_atm_lane(vc->swidb)) {
	    regptr->argreg = MCI_DEC_DEST + MCI_DEC_LANE_OVERHEAD;
	    regptr->cmdreg = MCI_CMD_TX1_SELECT;
	    regptr->write1long = 
		*((ulong *) &input->sniff[SNIFFBASE+1].d.sword[1]);
	    regptr->argreg = MCI_DEC_SRC + MCI_DEC_LANE_OVERHEAD;
	}
    } else {
	regptr->argreg = MCI_DEC_DEST;
	regptr->cmdreg = MCI_CMD_TX1_SELECT;
	regptr->write1long = 
	    *((ulong *) &input->sniff[SNIFFBASE+1].d.sword[1]);
	regptr->argreg = MCI_DEC_SRC;
    }
    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    regptr->write1long = *((ulong *) &input->sniff[SNIFFBASE+3].d.sword[1]);
    
    goto return_dn_mapped;
    
 dn_check_local:
    
    /*
     * Clear the DN_LOCAL bit in the DECnet packet.
     */
    if (input_interface & IDB_ATM) {
	regptr->argreg = MCI_DEC_FLAGS + MCI_DEC_LANE_OVERHEAD;
	p.u.dnptr->typecode &= ~DN_LOCAL;		
    } else {
	if (input != output) {	
	    p.u.dnptr->typecode &= ~DN_LOCAL;		
	    if (input_interface & IDB_FDDI) {
		/*
		 * Is this like COOL?  The rif shit?
		 */
		regptr->argreg = (MCI_DEC_FLAGS + ISO2_FDDI_OVERHEAD +
				  input->rxrifwordlen);
	    } 
	    else {
		regptr->argreg = MCI_DEC_FLAGS;
	    }
	}
    }

    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    regptr->write1short = input->sniff[SNIFFBASE+0].d.sword[0];

    goto return_dn_check_local;
    
 dn_nextsplit:
    mci_regptr = input->mci_regptr;
    input->sniff[SNIFFBASE+6].d.lword = mci_regptr->readlong;
    if (!(thisroute = (dnroutetype *) dn_multipath(p.u.dnptr, net, thisroute)))
	return(FALSE);
    goto return_dn_nextsplit;
    
 dn_check_atg:
    if (!(net = dn_address_translation(p.u.dnptr, net)))
	goto dn_check_retreq;
    else {
	
	/*
	 * Network/area entry found -- try to determine route.
	 */
	routeptr = dn_get_route(p.u.dnptr, net);
	thisroute = *routeptr;
	if (!thisroute)
	    goto dn_check_retreq;
    	mapped = DECNET_MAPPED;
    }
    uctemp = thisroute->flags;
    goto return_dn_check_atg;
    
 dn_check_retreq:
    if ((p.u.dnptr->typecode & DN_RETREQ) || reg_invoke_dnconversion_report()||
	decnet_default_route)
	return(FALSE);                           /* send to process level */
    else {
	input->mci_regptr->cmdreg = MCI_CMD_RX_FLUSH;
	return(TRUE); 	      
    }
}

static void decnet_mci_fs_register_drivers (ulong flags, hwidbtype* idb)
{
    /*
     * Check to see if we're configuring DECnet
     */
    if (!(flags & FSF_DECNET))
	return;

    if (idb->fast_switch_flag & FSF_DECNET) {
	reg_add_ether_fs(RXTYPE_DECNET,
			 (service_ether_fs_type)dn_fastswitch,
			 "dn_fastswitch");
	reg_add_fddi_fs(RXTYPE_DECNET,
			(service_fddi_fs_type)dn_fastswitch,
			"dn_fastswitch");
	reg_add_aip_fs(RXTYPE_DECNET,
			(service_aip_fs_type)dn_fastswitch,
			"dn_fastswitch");

	reg_add_hdlc_fs(RXTYPE_DECNET,
			(service_hdlc_fs_type)dn_fastswitch,
			"dn_fastswitch");
    } else {
	fs_table_fill((fastswitch_type)return_false, "return_false",
		      RXTYPE_DECNET);
    }
}

static void decnet_mci_fs_init (subsystype* subsys)
{
    reg_add_register_drivers(decnet_mci_fs_register_drivers,
			     "decnet_mci_fs_register_drivers");
}

/*
 * MCI FS subsystem header
 */

#define DECNETMCIFS_MAJVERSION 1
#define DECNETMCIFS_MINVERSION 0
#define DECNETMCIFS_EDITVERSION  1

SUBSYS_HEADER(decnet_mci_fs, DECNETMCIFS_MAJVERSION, DECNETMCIFS_MINVERSION,
	      DECNETMCIFS_EDITVERSION,
	      decnet_mci_fs_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: decnet",
	      "req: fastswitch, decnet");


