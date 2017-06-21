/* $Id: vinesfast_mci.c,v 3.5.8.7 1996/08/19 16:55:49 ehk Exp $
 * $Source: /release/112/cvs/Xsys/vines/vinesfast_mci.c,v $
 *------------------------------------------------------------------
 * vinesfast_mci.c -- VINES Fast Switching module providing MCI/CBUS
 *                    specific Support.
 *
 * July 1992, David R. Hampton
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vinesfast_mci.c,v $
 * Revision 3.5.8.7  1996/08/19  16:55:49  ehk
 * CSCdi66379:  vines fast switching fails to route Ether SNAP pkt if >
 * 1478 bytes
 * Branch: California_branch
 * do not include MAC header length in the SNAP packet lengt
 *
 * Revision 3.5.8.6  1996/08/14  14:11:34  dblair
 * CSCdi64968:  Implment outbound half-fastswitching for multilink PPP
 * Branch: California_branch
 * add multilink half-fastswitching support
 *
 * Revision 3.5.8.5  1996/08/13  14:34:59  rbadri
 * CSCdi64089:  Multiprotocol support fixes required
 * Branch: California_branch
 * Vines support for LANE
 *
 * Revision 3.5.8.4  1996/07/17  00:03:01  ehk
 * CSCdi59059:  Out of order packets when using vines route-cache
 * Branch: California_branch
 * invalidate a fast cache entry before passing to process switching
 * a packet with metric notification bit set and relearn a new cache
 * to maintain original packet ordering.
 *
 * Revision 3.5.8.3  1996/05/17  12:15:49  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.2.4  1996/05/06  00:06:23  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.4.2.3  1996/05/02  22:08:24  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.2.2  1996/04/03  22:10:07  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.2.1  1996/03/17  18:53:47  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.5.8.2  1996/04/27  06:30:29  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.3.16.3  1996/04/25  23:55:44  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.3.16.2  1996/04/16  23:56:37  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.3.16.1  1996/02/23  20:59:57  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.5.8.1  1996/03/18  22:32:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.20.3  1996/03/16  07:53:16  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.20.2  1996/03/13  02:05:21  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.20.1  1996/02/20  21:47:35  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/27  19:28:24  achopra
 * CSCdi45189:  "show atm traffic" shows different (wrong) packet counts
 *
 * Revision 3.4  1996/02/13  08:17:13  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.3  1995/12/01  21:39:41  slin
 * CSCdi44836:  Vines doesnt use idb subblocks
 *
 * Revision 3.2  1995/11/17  17:59:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:45:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:34:24  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/08/16  23:04:13  fred
 * CSCdi38896:  correct handling of return from v_same_mci()
 *
 * Revision 2.4  1995/08/12  07:44:10  fred
 * CSCdi38525:  make fast path able to process switch or discard packets
 * on congested interfaces
 *
 * Revision 2.3  1995/08/05  07:09:26  hampton
 * Fix VINES fast switching bits/sec counter on fddi interface.
 * [CSCdi30206]
 *
 * Revision 2.2  1995/07/10 05:32:43  fred
 * CSCdi36850:  commit fast path queuing for appletalk, decnet, etc.
 *
 * Revision 2.1  1995/06/07  23:15:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Routines Exported from this module:
 *
 * xxx_vines_fastswitch: Fast switches vines packets (called via the
 * various fast switching registries.)
 */

#include "master.h"
#include "subsys.h"
#include "fastswitch_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "../if/network.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/atm.h"
#include "../if/fddi.h"
#include "../wan/serial.h"
#include "../wan/frame_relay.h"
#include "../wan/atm_dxi.h"
#include "../wan/smds.h"
#include "../wan/ppp.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "vines_private.h"
#include "vinesfast.h"
#include "vinesfast_mci.h"
#include "../wan/dialer.h"

/*
 * Globals
 */
boolean (*vfs_samemci[V_FS_MAX])(hwidbtype *, vinesfastpathtype *);
boolean (*vfs_mci2mci[V_FS_MAX])(hwidbtype *, vinesfastpathtype *);


/**********************************************************************
 *
 *		   HIGH END SMDS PADDING ROUTINES
 *
 **********************************************************************/

static void v_mci2mci_clear_pad (
    mciregtype *outreg,
    ulong lastlong,
    ulong pad)
{
    ulong temp;

    temp = lastlong;
    switch (pad) {
      case 3:	temp &= 0xFF000000;	break;
      case 2:	temp &= 0xFFFF0000;	break;
      case 1:	temp &= 0xFFFFFF00;	break;
      default:	/* do nothing */	break;
    }
    outreg->write1long = temp;
}

static void v_samemci_read_pad (
    hwidbtype *input,
    mciregtype *inreg,
    ulong size_of_data,
    ulong pad)
{
    inreg->argreg = input->fast_net_start + btow(size_of_data + pad) - 2;
    inreg->cmdreg = MCI_CMD_RX_SELECT;
    input->smds_lastlong = inreg->readlong;
}

static void v_samemci_write_pad (
    hwidbtype *input,
    mciregtype *inreg,
    ulong size_of_data,
    ulong pad)
{
    inreg->argreg = input->fast_net_start + btow(size_of_data + pad) - 2;
    inreg->cmdreg = MCI_CMD_TX1_SELECT;
    v_mci2mci_clear_pad(inreg, input->smds_lastlong, pad);
}

/**********************************************************************
 *
 *		FAST SWITCHING BETWEEN CBUS/MCI CARDS
 *
 **********************************************************************/

/*
 * vines_mci2mci
 *
 * Fast switch VINES packets between two different MCI controllers.
 * Called from xxx_vines_fastswitch().
 */
#define V_M2M_START                                                    	\
    hwidbtype *output;							\
    mciregtype *inreg, *outreg;						\
    ulong size_to_copy, size_to_xmit;					\
									\
    /*									\
     * Set up some common variables					\
     */									\
    output = path->idb->hwptr;						\
    outreg = output->mci_regptr;					\
    inreg = input->mci_regptr;						\
									\
    /*									\
     * Select the destination interface and reserve a buffer in window 1\
     * of the destination MCI/FCI					\
     */									\
    outreg->argreg = output->mci_index;					\
    outreg->cmdreg = MCI_CMD_SELECT;					\
    outreg->argreg = output->buffer_pool;				\
    outreg->cmdreg = MCI_CMD_TX1_RESERVE;				\
    input->counters.inputs_fast[ACCT_PROTO_VINES]++;			\
    input->counters.rx_cumbytes_fast[ACCT_PROTO_VINES] += input->rxlength; \
    if (outreg->cmdreg != MCI_RSP_OKAY) {				\
        if (output->priority_list) {                                    \
            /* 							       	\
             * if sorting traffic and interface is congested, process switch \
             */							       	\
            return(FALSE);	       					\
        } else {					                \
            /*								\
             * Reserve failed on output -- flush the packet.		\
             */								\
            output->counters.output_total_drops++;		       	\
            inreg->cmdreg = MCI_CMD_RX_FLUSH;				\
            return(TRUE);						\
        }								\
    }									\
									\
    /*									\
     * Set up write pointer to base of destination transmit window.	\
     * Do some input accounting.  Figure out how to write new MAC	\
     * header.  Do not touch the read pointer, because it is pointing	\
     * exactly where it should be, at the first byte os the source 	\
     * network number. Compute number of bytes to be copied 		\
     * (in size_to_copy) and to be sent (in size_to_xmit).		\
     */									\
    size_to_xmit = input->length_only + path->reallength;		\
    size_to_copy = input->length_only - VINES_OFFSETB_SNET;		\
    output->counters.outputs_fast[ACCT_PROTO_VINES]++;			\
    output->counters.tx_cumbytes_fast[ACCT_PROTO_VINES] += size_to_xmit; \
    outreg->argreg = 0;							\
    outreg->cmdreg = MCI_CMD_TX1_SELECT;

/*
 * v_m2m_finish
 *
 * Finish up this packet and send it.  This routine first copies out the
 * parts of the VINES IP packet that have already been sucked into local
 * memory, and then it copies the bulk of datagram from one MCI to
 * another.  Last, it transmits modified copy and flush the original from
 * the receive buffer.
 */
static inline void v_m2m_finish (
    hwidbtype *input,
    mciregtype *inreg,
    mciregtype *outreg,
    ulong size_to_copy,
    ulong size_to_xmit)
{
    outreg->write1long  = input->checksum_length;
    outreg->write1short = input->hops_ptype;
    outreg->write1long  = input->destination_net;
    outreg->write1short = input->destination_host;
    mci2mci(&inreg->readlong, &outreg->write1long, size_to_copy);
    outreg->argreg = size_to_xmit;
    outreg->cmdreg = MCI_CMD_TX1_START;
    inreg->cmdreg = MCI_CMD_RX_FLUSH;
}

/*
 * v_m2m_smds_finish
 *
 * This is essentially the same as the above routine, with a slight
 * modification to handle the SMDS padding and trailer.
 */
static void v_m2m_smds_finish (
    hwidbtype *input,
    mciregtype *inreg,
    mciregtype *outreg,
    ulong size_to_copy,
    ulong size_to_xmit,
    ulong pad,
    smdstype *sm)
{
    ulong lastlong;

    outreg->write1long  = input->checksum_length;
    outreg->write1short = input->hops_ptype;
    outreg->write1long  = input->destination_net;
    outreg->write1short = input->destination_host;

    /*
     * Copy the rest of the data across.  Make sure that the copy stops
     * just short of copying the last few bytes.  This is necessary
     * because we have to make sure that the padding is zero'd to pass
     * someone's bogus certification test.
     */
    mci2mci(&inreg->readlong, &outreg->write1long, size_to_copy - 4);
    lastlong = inreg->readlong;
    v_mci2mci_clear_pad(outreg, lastlong, pad);
    vines_write_smds_trailer(outreg, (ulong *)&sm->smds_rsvd);

    outreg->argreg = size_to_xmit;
    outreg->cmdreg = MCI_CMD_TX1_START;
    inreg->cmdreg = MCI_CMD_RX_FLUSH;
}

/*
 * vines_mci2mci_ether_snap
 *
 * Write a SNAP ethernet header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.  This routine
 * works between cbus/mci cards.
 */
static boolean vines_mci2mci_ether_snap (
    hwidbtype *input,
    vinesfastpathtype *path)
{
    V_M2M_START;
    vines_write_ether_snap_header(outreg, path, (input->length_only +
						 SNAP_HDRBYTES));
    if (size_to_xmit < MINETHERSIZE)
	size_to_xmit = MINETHERSIZE;
    v_m2m_finish(input, inreg, outreg, size_to_copy, size_to_xmit);
    return(TRUE);
}

/*
 * vines_mci2mci_ether_Arpa
 *
 * Write a ethernet ARPA header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.  This routine
 * works between cbus/mci cards.
 */
static boolean vines_mci2mci_ether_arpa (
    hwidbtype *input,
    vinesfastpathtype *path)
{
    V_M2M_START;
    vines_write_ether_header(outreg, path);
    if (size_to_xmit < MINETHERSIZE)
	size_to_xmit = MINETHERSIZE;
    v_m2m_finish(input, inreg, outreg, size_to_copy, size_to_xmit);
    return(TRUE);
}

/*
 * vines_mci2mci_fddi
 *
 * Write a FDDI SNAP header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.  This routine
 * works between cbus/mci cards.
 */
static boolean vines_mci2mci_fddi (
    hwidbtype *input,
    vinesfastpathtype *path)
{
    V_M2M_START;
    vines_write_fddi_header(outreg, path);
    size_to_xmit |= MCI_TX_ODDALIGN;
    v_m2m_finish(input, inreg, outreg, size_to_copy, size_to_xmit);
    return(TRUE);
}

/*
 * vines_mci2mci_token_snap
 *
 * Write a Token Ring SNAP header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.  This routine
 * works between cbus/mci cards.
 */
static boolean vines_mci2mci_token_snap (
    hwidbtype *input,
    vinesfastpathtype *path)
{
    V_M2M_START;
    vines_write_token_snap_header(outreg, path);
    v_m2m_finish(input, inreg, outreg, size_to_copy, size_to_xmit);
    return(TRUE);
}

/*
 * vines_mci2mci_token_vines
 *
 * Write a Banyan Token Ring header (SAP + demux byte) onto the front of
 * the supplied packet, and then hand the packet off to the transmit
 * code.  This routine works between cbus/mci cards.
 */
static boolean vines_mci2mci_token_vines (
    hwidbtype *input,
    vinesfastpathtype *path)
{
    V_M2M_START;
    vines_write_token_header(outreg, path);
    v_m2m_finish(input, inreg, outreg, size_to_copy, size_to_xmit);
    return(TRUE);
}

/*
 * vines_mci2mci_hdlc
 *
 * Write a HDLC header onto the front of the supplied packet, and then
 * hand the packet off to the transmit code.  This routine works between
 * cbus/mci cards.
 */
static boolean vines_mci2mci_hdlc (
    hwidbtype *input,
    vinesfastpathtype *path)
{
    V_M2M_START;
    vines_write_hdlc_header(outreg, path);
    v_m2m_finish(input, inreg, outreg, size_to_copy, size_to_xmit);
    return(TRUE);
}

/*
 * vines_mci2mci_ppp
 *
 * Write a PPP header onto the front of the supplied packet, and then
 * hand the packet off to the transmit code.  This routine works between
 * cbus/mci cards.
 */
static boolean vines_mci2mci_ppp (
    hwidbtype *input,
    vinesfastpathtype *path)
{
    V_M2M_START;
    vines_write_ppp_header(outreg, path);
    v_m2m_finish(input, inreg, outreg, size_to_copy, size_to_xmit);
    return(TRUE);
}

/*
 * vines_mci2mci_frame
 *
 * Write a Non-IETF Frame Relay header onto the front of the supplied
 * packet, and then hand the packet off to the transmit code.  This
 * routine works between cbus/mci cards.
 */
static boolean vines_mci2mci_frame (
    hwidbtype *input,
    vinesfastpathtype *path)
{
    V_M2M_START;
    vines_write_frame_header(outreg, path);
    INC_PVC_FAST_OUTPUT(HWIDB_TO_FR_PVC(output, path->subint_reference_num),
			size_to_xmit);
    v_m2m_finish(input, inreg, outreg, size_to_copy, size_to_xmit);
    return(TRUE);
}

/*
 * vines_mci2mci_frame_ietf
 *
 * Write a IETF Frame Relay header onto the front of the supplied
 * packet, and then hand the packet off to the transmit code.  This
 * routine works between cbus/mci cards.
 */
static boolean vines_mci2mci_frame_ietf (
    hwidbtype *input,
    vinesfastpathtype *path)
{
    V_M2M_START;
    vines_write_frame_ietf_header(outreg, path);
    INC_PVC_FAST_OUTPUT(HWIDB_TO_FR_PVC(output, path->subint_reference_num),
			size_to_xmit);
    v_m2m_finish(input, inreg, outreg, size_to_copy, size_to_xmit);
    return(TRUE);
}

/*
 * vines_mci2mci_smds_cisco
 *
 * Write a "cisco" SMDS header onto the front of the supplied packet, the
 * trailer and padding onto the end of the packet, and then hand the
 * packet off to the transmit code.
 *
 * Remember that the last select performed doubles as the starting point
 * for transmission.  In this case it doesn't matter that the trailer is
 * written last because the code always explicitly selects zero ase the
 * first byte to be tranmitted.
 */
static boolean vines_mci2mci_smds_cisco (
    hwidbtype *input,
    vinesfastpathtype *path)
{
    ulong pad, basize;
    smdstype *sm;

    V_M2M_START;
    V_SMDS_SETUP;
    vines_write_smds_header(outreg, path);
    v_m2m_smds_finish(input, inreg, outreg, size_to_copy, size_to_xmit, pad, sm);
    return(TRUE);
}

/*
 * vines_mci2mci_smds_dxi
 *
 * Write a DXI SMDS header onto the front of the supplied packet, the
 * trailer and padding onto the end of the packet, and then hand the
 * packet off to the transmit code.
 *
 * Remember that the last select performed doubles as the starting point
 * for transmission.  In this case it doesn't matter that the trailer is
 * written last because the code always explicitly selects zero ase the
 * first byte to be tranmitted.
 */
static boolean vines_mci2mci_smds_dxi (
    hwidbtype *input,
    vinesfastpathtype *path)
{
    ulong pad, basize;
    smdstype *sm;

    V_M2M_START;
    V_SMDS_SETUP;
    vines_write_smds_dxi_header(outreg, path);
    v_m2m_smds_finish(input, inreg, outreg, size_to_copy, size_to_xmit, pad, sm);
    return(TRUE);
}
/*
 * vines_mci2mci_atm_dxi
 *
 * Write a atm-dxi header onto the front of the supplied
 * packet, and then hand the packet off to the transmit code.  This
 * routine works between cbus/mci cards.
 */
static boolean vines_mci2mci_atm_dxi (
    hwidbtype *input,
    vinesfastpathtype *path)
{
    V_M2M_START;
    vines_write_atm_dxi_header(outreg, path);
    INC_DXI_PVC_FAST_OUTPUT(HWIDB_TO_DXI_PVC(output,
					     path->subint_reference_num),
			    size_to_xmit);
    v_m2m_finish(input, inreg, outreg, size_to_copy, size_to_xmit);
    return(TRUE);
}


/**********************************************************************
 *
 *		FAST SWITCHING WITHIN AN CBUS/MCI CARD
 *
 **********************************************************************/

/*
 * v_same_start
 *
 * Move the buffer to the output interface.  This routine signals if its
 * parent should continue running.
 */
enum priority_options {FAST_SWITCH, HALF_FAST_SWITCH, DROP};
static inline enum priority_options v_same_start (
    hwidbtype *input,
    mciregtype *inreg,
    hwidbtype *output)
{
    /*
     * RX buffer from receive window to transmit window 1. This may
     * fail if interface output queue limit has been exceeded.
     */
    inreg->argreg = (ushort)output->mci_index;
    inreg->cmdreg = MCI_CMD_RX_MOVE;
    input->counters.inputs_fast[ACCT_PROTO_VINES]++;
    input->counters.rx_cumbytes_fast[ACCT_PROTO_VINES] += input->rxlength;
    if (inreg->cmdreg == MCI_RSP_OKAY) 
	return(FAST_SWITCH);
    else if (output->priority_list)
	return(HALF_FAST_SWITCH);
    else {
	output->counters.output_total_drops++;
	inreg->cmdreg = MCI_CMD_RX_FLUSH;
	return(DROP);
    }
}

/*
 * v_same_finish
 *
 * Finish up this packet and send it.  This routine first copies out the
 * parts of the VINES IP packet that have already been sucked into local
 * memory, and then kicks the cbus/mci to start it transmitting the
 * frame.
 */
static inline void v_same_finish (
    hwidbtype *input,
    mciregtype *inreg,
    hwidbtype *output,
    ulong size_to_xmit,
    ulong mciarg)
{
    /*
     * Don't waste time writing the DNET:DHOST values.  They haven't
     * changed so there's no need to waste the time.
     */
    inreg->write1long  = input->checksum_length;
    inreg->write1short = input->hops_ptype;
    inreg->argreg = mciarg;
    inreg->cmdreg = MCI_CMD_TX1_START;
    output->counters.tx_cumbytes_fast[ACCT_PROTO_VINES] += size_to_xmit;
    output->counters.outputs_fast[ACCT_PROTO_VINES]++;
}

static boolean vines_samemci_atm (
    hwidbtype *input,
    vinesfastpathtype *path)
{
    mciregtype *inreg = input->mci_regptr;
    hwidbtype *output = path->idb->hwptr;
    ulong size_of_data = input->length_only;
    ulong size_to_xmit = size_of_data + path->reallength;

    switch (v_same_start(input, inreg, output)) {
      case FAST_SWITCH:
	break;
      case HALF_FAST_SWITCH:
	return(FALSE);
      case DROP:
	return(TRUE);
    }

    /*
     * FASTswitch VINES packets, same MCI ONLY
     */
    inreg->argreg = input->fast_net_start - ATM_HDR_WORDS_OUT;
    inreg->cmdreg = MCI_CMD_TX1_SELECT;
    vines_write_atm_header(inreg, path);
    v_same_finish(input, inreg, output, size_to_xmit, size_to_xmit);
    ATM_VCOUTFAST(output, path->vinesp_mh.mac_shorts[0]);
    ATM_OUTFAST(output);
    return(TRUE);
}

/*
 * vines_samemci_ether_snap
 *
 * Write a SNAP ethernet header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.  This routine
 * works within a cbus/mci card.
 */
static boolean vines_samemci_ether_snap (
    hwidbtype *input,
    vinesfastpathtype *path)
{
    mciregtype *inreg = input->mci_regptr;
    hwidbtype *output = path->idb->hwptr;
    ulong size_of_data = input->length_only;
    ulong size_to_xmit = size_of_data + path->reallength;

    switch (v_same_start(input, inreg, output)) {
      case FAST_SWITCH:
	break;
      case HALF_FAST_SWITCH:
	return(FALSE);
      case DROP:
	return(TRUE);
    }
    inreg->argreg = input->fast_net_start - E_SNAP_HDR_WORDS_OUT;
    inreg->cmdreg = MCI_CMD_TX1_SELECT;
    vines_write_ether_snap_header(inreg, path, (size_of_data+SNAP_HDRBYTES));
    if (size_to_xmit < MINETHERSIZE)
	size_to_xmit = MINETHERSIZE;
    v_same_finish(input, inreg, output, size_to_xmit, size_to_xmit);
    return(TRUE);
}

/*
 * vines_samemci_ether_arpa
 *
 * Write a ethernet ARPA header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.  This routine
 * works within a cbus/mci card.
 */
static boolean vines_samemci_ether_arpa (
    hwidbtype *input,
    vinesfastpathtype *path)
{
    mciregtype *inreg = input->mci_regptr;
    hwidbtype *output = path->idb->hwptr;
    ulong size_of_data = input->length_only;
    ulong size_to_xmit = size_of_data + path->reallength;

    switch (v_same_start(input, inreg, output)) {
      case FAST_SWITCH:
	break;
      case HALF_FAST_SWITCH:
	return(FALSE);
      case DROP:
	return(TRUE);
    }
	
    inreg->argreg = input->fast_net_start - E_ARPA_HDR_WORDS_OUT;
    inreg->cmdreg = MCI_CMD_TX1_SELECT;
    vines_write_ether_header(inreg, path);
    if (size_to_xmit < MINETHERSIZE)
	size_to_xmit = MINETHERSIZE;
    v_same_finish(input, inreg, output, size_to_xmit, size_to_xmit);
    return(TRUE);
}

/*
 * vines_samemci_fddi
 *
 * Write a FDDI SNAP header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.  This routine
 * works within a cbus/mci card.
 */
static boolean vines_samemci_fddi (
    hwidbtype *input,
    vinesfastpathtype *path)
{
    mciregtype *inreg = input->mci_regptr;
    hwidbtype *output = path->idb->hwptr;
    ulong size_of_data = input->length_only;
    ulong size_to_xmit = size_of_data + path->reallength;
    ulong mciarg = size_to_xmit;

    switch (v_same_start(input, inreg, output)) {
      case FAST_SWITCH:
	break;
      case HALF_FAST_SWITCH:
	return(FALSE);
      case DROP:
	return(TRUE);
    }

    inreg->argreg = input->fast_net_start - F_SNAP_HDR_WORDS_OUT;
    inreg->cmdreg = MCI_CMD_TX1_SELECT;
    vines_write_fddi_header(inreg, path);
    mciarg |= MCI_TX_ODDALIGN;
    v_same_finish(input, inreg, output, size_to_xmit, mciarg);
    return(TRUE);
}

/*
 * vines_samemci_token_snap
 *
 * Write a Token Ring SNAP header onto the front of the supplied packet,
 * and then hand the packet off to the transmit code.  This routine works
 * within a cbus/mci card.
 */
static boolean vines_samemci_token_snap (
    hwidbtype *input,
    vinesfastpathtype *path)
{
    mciregtype *inreg = input->mci_regptr;
    hwidbtype *output = path->idb->hwptr;
    ulong size_of_data = input->length_only;
    ulong size_to_xmit = size_of_data + path->reallength;

    switch (v_same_start(input, inreg, output)) {
      case FAST_SWITCH:
	break;
      case HALF_FAST_SWITCH:
	return(FALSE);
      case DROP:
	return(TRUE);
    }

    inreg->argreg = input->fast_net_start - T_SNAP_HDR_WORDS_OUT;
    inreg->cmdreg = MCI_CMD_TX1_SELECT;
    vines_write_token_snap_header(inreg, path);
    v_same_finish(input, inreg, output, size_to_xmit, size_to_xmit);
    return(TRUE);
}

/*
 * vines_samemci_token_vines
 *
 * Write a Banyan Token Ring header (SAP + demux byte) onto the front of
 * the supplied packet, and then hand the packet off to the transmit
 * code.  This routine works within a cbus/mci card.
 */
static boolean vines_samemci_token_vines (
    hwidbtype *input,
    vinesfastpathtype *path)
{
    mciregtype *inreg = input->mci_regptr;
    hwidbtype *output = path->idb->hwptr;
    ulong size_of_data = input->length_only;
    ulong size_to_xmit = size_of_data + path->reallength;

    switch (v_same_start(input, inreg, output)) {
      case FAST_SWITCH:
	break;
      case HALF_FAST_SWITCH:
	return(FALSE);
      case DROP:
	return(TRUE);
    }

    inreg->argreg = input->fast_net_start - T_VINES_HDR_WORDS_OUT;
    inreg->cmdreg = MCI_CMD_TX1_SELECT;
    vines_write_token_header(inreg, path);
    v_same_finish(input, inreg, output, size_to_xmit, size_to_xmit);
    return(TRUE);
}

/*
 * vines_samemci_hdlc
 *
 * Write a HDLC header onto the front of the supplied packet, and then
 * hand the packet off to the transmit code.  This routine works within a
 * cbus/mci card.
 */
static boolean vines_samemci_hdlc (
    hwidbtype *input,
    vinesfastpathtype *path)
{
    mciregtype *inreg = input->mci_regptr;
    hwidbtype *output = path->idb->hwptr;
    ulong size_of_data = input->length_only;
    ulong size_to_xmit = size_of_data + path->reallength;

    switch (v_same_start(input, inreg, output)) {
      case FAST_SWITCH:
	break;
      case HALF_FAST_SWITCH:
	return(FALSE);
      case DROP:
	return(TRUE);
    }

    inreg->argreg = input->fast_net_start - path->wordlength;
    inreg->cmdreg = MCI_CMD_TX1_SELECT;
    vines_write_hdlc_header(inreg, path);
    v_same_finish(input, inreg, output, size_to_xmit, size_to_xmit);
    return(TRUE);
}

/*
 * vines_samemci_ppp
 *
 * Write a PPP header onto the front of the supplied packet, and then
 * hand the packet off to the transmit code.  This routine works within a
 * cbus/mci card.
 */
static boolean vines_samemci_ppp (
    hwidbtype *input,
    vinesfastpathtype *path)
{
    mciregtype *inreg = input->mci_regptr;
    hwidbtype *output = path->idb->hwptr;
    ulong size_of_data = input->length_only;
    ulong size_to_xmit = size_of_data + path->reallength;

    switch (v_same_start(input, inreg, output)) {
      case FAST_SWITCH:
	break;
      case HALF_FAST_SWITCH:
	return(FALSE);
      case DROP:
	return(TRUE);
    }

    inreg->argreg = input->fast_net_start - path->wordlength;
    inreg->cmdreg = MCI_CMD_TX1_SELECT;
    vines_write_ppp_header(inreg, path);
    v_same_finish(input, inreg, output, size_to_xmit, size_to_xmit);
    return(TRUE);
}

/*
 * vines_samemci_frame
 *
 * Write a Non-IETF header onto the front of the supplied packet, and
 * then hand the packet off to the transmit code.  This routine works
 * within a cbus/mci card.
 */
static boolean vines_samemci_frame (
    hwidbtype *input,
    vinesfastpathtype *path)
{
    mciregtype *inreg = input->mci_regptr;
    hwidbtype *output = path->idb->hwptr;
    ulong size_of_data = input->length_only;
    ulong size_to_xmit = size_of_data + path->reallength;

    switch (v_same_start(input, inreg, output)) {
      case FAST_SWITCH:
	break;
      case HALF_FAST_SWITCH:
	return(FALSE);
      case DROP:
	return(TRUE);
    }

    inreg->argreg = input->fast_net_start - path->wordlength;
    inreg->cmdreg = MCI_CMD_TX1_SELECT;
    vines_write_frame_header(inreg, path);
    INC_PVC_FAST_OUTPUT(HWIDB_TO_FR_PVC(output, path->subint_reference_num),
			size_to_xmit);
    v_same_finish(input, inreg, output, size_to_xmit, size_to_xmit);
    return(TRUE);
}

/*
 * vines_samemci_frame_ietf
 *
 * Write a IETF header onto the front of the supplied packet, and
 * then hand the packet off to the transmit code.  This routine works
 * within a cbus/mci card.
 */
static boolean vines_samemci_frame_ietf (
    hwidbtype *input,
    vinesfastpathtype *path)
{
    mciregtype *inreg = input->mci_regptr;
    hwidbtype *output = path->idb->hwptr;
    ulong size_of_data = input->length_only;
    ulong size_to_xmit = size_of_data + path->reallength;

    switch (v_same_start(input, inreg, output)) {
      case FAST_SWITCH:
	break;
      case HALF_FAST_SWITCH:
	return(FALSE);
      case DROP:
	return(TRUE);
    }

    inreg->argreg = input->fast_net_start - path->wordlength;
    inreg->cmdreg = MCI_CMD_TX1_SELECT;
    vines_write_frame_ietf_header(inreg, path);
    INC_PVC_FAST_OUTPUT(HWIDB_TO_FR_PVC(output, path->subint_reference_num),
			size_to_xmit);
    v_same_finish(input, inreg, output, size_to_xmit, size_to_xmit);
    return(TRUE);
}

/*
 * vines_samemci_smds_cisco 
 *
 * Write a "Cisco" SMDS header onto the front of the supplied packet, the
 * trailer and padding onto the end of the packet, and then hand the
 * packet off to the transmit code.
 *
 * Note that the trailer is written first.  This is because the last
 * select performed doubles as the starting point for transmission.  In
 * this case the start of the header is the last select, and thus the
 * first byte tranmitted.
 */
static boolean vines_samemci_smds_cisco (
    hwidbtype *input,
    vinesfastpathtype *path)
{
    mciregtype *inreg = input->mci_regptr;
    hwidbtype *output = path->idb->hwptr;
    ulong size_of_data = input->length_only;
    ulong size_to_xmit = size_of_data + path->reallength;
    ulong pad, basize;
    smdstype *sm;

    V_SMDS_SETUP;
    v_samemci_read_pad(input, inreg, size_of_data, pad);
    switch (v_same_start(input, inreg, output)) {
      case FAST_SWITCH:
	break;
      case HALF_FAST_SWITCH:
	return(FALSE);
      case DROP:
	return(TRUE);
    }

    v_samemci_write_pad(input, inreg, size_of_data, pad);
    vines_write_smds_trailer(inreg, (ulong *)&sm->smds_rsvd);
    inreg->argreg = input->fast_net_start - SMDS_HDR_WORDS_OUT;
    inreg->cmdreg = MCI_CMD_TX1_SELECT;
    vines_write_smds_header(inreg, path);
    v_same_finish(input, inreg, output, size_to_xmit, size_to_xmit);
    return(TRUE);
}

/*
 * vines_samemci_smds_dxi 
 *
 * Write a DXI SMDS header onto the front of the supplied packet, the
 * trailer and padding onto the end of the packet, and then hand the
 * packet off to the transmit code.
 *
 * Note that the trailer is written first.  This is because the last
 * select performed doubles as the starting point for transmission.  In
 * this case the start of the header is the last select, and thus the
 * first byte tranmitted.
 */
static boolean vines_samemci_smds_dxi (
    hwidbtype *input,
    vinesfastpathtype *path)
{
    mciregtype *inreg = input->mci_regptr;
    hwidbtype *output = path->idb->hwptr;
    ulong size_of_data = input->length_only;
    ulong size_to_xmit = size_of_data + path->reallength;
    ulong pad, basize;
    smdstype *sm;

    V_SMDS_SETUP;
    v_samemci_read_pad(input, inreg, size_of_data, pad);
    switch (v_same_start(input, inreg, output)) {
      case FAST_SWITCH:
	break;
      case HALF_FAST_SWITCH:
	return(FALSE);
      case DROP:
	return(TRUE);
    }

    v_samemci_write_pad(input, inreg, size_of_data, pad);
    vines_write_smds_trailer(inreg, (ulong *)&sm->smds_rsvd);
    inreg->argreg = input->fast_net_start - SMDS_HDR_WORDS_OUT;
    inreg->cmdreg = MCI_CMD_TX1_SELECT;
    vines_write_smds_dxi_header(inreg, path);
    v_same_finish(input, inreg, output, size_to_xmit, size_to_xmit);
    return(TRUE);
}
/*
 * vines_samemci_atm_dxi
 *
 * Write a atm-dxi header onto the front of the supplied packet, and
 * then hand the packet off to the transmit code.  This routine works
 * within a cbus/mci card.
 */
static boolean vines_samemci_atm_dxi (
    hwidbtype *input,
    vinesfastpathtype *path)
{
    mciregtype *inreg = input->mci_regptr;
    hwidbtype *output = path->idb->hwptr;
    ulong size_of_data = input->length_only;
    ulong size_to_xmit = size_of_data + path->reallength;

    switch (v_same_start(input, inreg, output)) {
      case FAST_SWITCH:
	break;
      case HALF_FAST_SWITCH:
	return(FALSE);
      case DROP:
	return(TRUE);
    }

    inreg->argreg = input->fast_net_start - path->wordlength;
    inreg->cmdreg = MCI_CMD_TX1_SELECT;
    vines_write_atm_dxi_header(inreg, path);
    INC_DXI_PVC_FAST_OUTPUT(HWIDB_TO_DXI_PVC(output,
					     path->subint_reference_num),
			    size_to_xmit);
    v_same_finish(input, inreg, output, size_to_xmit, size_to_xmit);
    return(TRUE);
}


/**********************************************************************
 *
 *		      HIGH END INPUT ROUTINES
 *
 **********************************************************************/

/*
 * vines_access_check_mci
 *
 * Determine if we are allowed to forward the passed datagram, based on
 * the VINES access pointer.  Return FALSE if output access is denied.
 * MCI read pointer must already be positioned to the source netrowk
 * addres.
 */
boolean vines_access_check_mci (
    hwidbtype *input,
    ulong list)
{
    mciregtype *regptr;
    vinesacctype *item;
    ulong srcnet;
    ushort srchost;
    shortlong tmp;

    regptr = input->mci_regptr;
    item = (vinesacctype *) vines_access_lists[list];
    srcnet = regptr->readlong;
    srchost = regptr->readshort;
    do {
        if ((input->destination_host & item->dmaskhost) != item->dsthost)
            continue;
        if ((input->destination_net & item->dmasknet) != item->dstnet)
            continue;
        if ((srchost & item->smaskhost) != item->srchost)
            continue;
        if ((srcnet & item->smasknet) != item->srcnet)
            continue;
        if (item->protocol) {
            /* it is an extended access list */
            if (item->protocol != (input->hops_ptype & 0xFF))
                continue;
	    if ((item->protocol == VINES_IPC) || (item->protocol == VINES_SPP)) {
		tmp.d.lword = regptr->readlong;
		if ((tmp.d.sword[0] & item->smasksock) != item->srcsocket)
		    continue;
		if ((tmp.d.sword[1] & item->dmasksock) != item->dstsocket)
		    continue;
	    }
	}
        return(item->grant);
    } while ((item = item->next));
    return(FALSE);
}

/*
 * vines_hes_fastswitch()
 *
 * This routine is common to all of the cbus/mci interfaces.  It is
 * called after a media specific routine has set tup variables describing
 * the just received packet.
 *
 * The code below has been lovingly hand-crafted to provide a lifetime of
 * reliable use and happiness. ;-)
 *
 * Don't change this stuff unless you're prepared to spend time poring
 * over hot and sweaty assembler listings and HP analyzer timing output...
 */
static inline boolean vines_hes_fastswitch (
    hwidbtype *input,
    mciregtype *inreg)
{
    vinescachetype *cache;
    vinesfastpathtype *path;
    hwidbtype *output;
    idbtype *swoutput;
    ushort indx;

    input->checksum_length = inreg->readlong;
    input->hops_ptype = inreg->readshort;
    input->destination_net = inreg->readlong;
    input->destination_host = inreg->readshort;
    if (input->hops_ptype & (VINES_METRIC << 8)) {
	vines_invalidate_cache_entry(input->destination_net, 
				     input->destination_host);
        return(FALSE);
    }
    if ((input->hops_ptype & (VINES_HOPS << 8)) == 0)
        return(FALSE);

    indx = input->destination_host;
    indx ^= input->destination_net_h;
    indx ^= input->destination_net_l;
    indx ^= (indx >> 8);
    indx &= 0xFF;

    for (cache = vinescache[indx]; cache; cache = cache->next_entry) {
	if (!(cache->flags & V_FC_VALID) ||
	    (cache->host != input->destination_host) ||
	    (cache->net != input->destination_net))
            continue;
        path = cache->next_path;
        swoutput = path->idb;
        if (swoutput == input->fast_rcvidb)
            return(FALSE);

        /*
         * Check if access list are enabled on this interface.
         */
        if (!(ushort)vines_getidb(swoutput)->vines_accesslist) {
	  vines_fs_yes_doit:
            output = swoutput->hwptr;
	    if (output->output_qcount && output->priority_list) {
		/*
		 * if interface is congested and we are sorting traffic,
		 * do this in the process path
		 */
		return(FALSE);
	    }
            cache->next_path = path->next_path;
            input->hops_ptype -= 256;/* decrement hop count */
            if (inreg == output->mci_regptr) {
                /*
                 * Output interface is on the same MCI.
                 */
		return((*vfs_samemci[path->encaptype])(input, path));
            } else if (output->mci_regptr) {
                /*
                 * Output is another MCI.
                 */
		return((*vfs_mci2mci[path->encaptype])(input, path));
            } else {
                /*
                 * Oops.  Shouldn't be here.
                 */
		return(FALSE);
	    }
        } else {
            if (vines_access_lists[vines_getidb(swoutput)->vines_accesslist])
                if (!(vines_access_check_mci(input,
					     vines_getidb(swoutput)->vines_accesslist)))
                    return(FALSE);
            goto vines_fs_yes_doit;
        }
    }
    return(FALSE);
}

/*******************************************************************
 *
 *******************************************************************/

/*
 * ether_snap_vines_fastswitch
 *
 * Very high speed switching of SNAP encapsulated Ethernet VINES
 * datagram. This routine has bee heavily optimized for speed, since it
 * is in the inner loop of the code that handles back-to-back datagrams.
 */
static boolean ether_snap_vines_fastswitch (
    hwidbtype *input)
{
    mciregtype *inreg;
 
    /*
     * Setup vines data read pointer to check the hop count.
     */
    inreg = input->mci_regptr;
    input->fast_net_start = MCI_ETHER_OFFSET + E_SNAP_HDR_WORDS_IN;
    inreg->argreg = MCI_ETHER_OFFSET + E_SNAP_HDR_WORDS_IN;
    inreg->cmdreg = MCI_CMD_RX_SELECT;
    return(vines_hes_fastswitch(input, inreg));
}

/*
 * ether_vines_fastswitch
 *
 * Very high speed switching of SNAP or ARPA encapsulated Ethernet VINES
 * datagram. This routine has been heavily optimized for speed, since it
 * is in the inner loop of the code that handles back-to-back datagrams.
 */
boolean ether_vines_fastswitch (hwidbtype *input)
{
    mciregtype *inreg;

    /*
     * Setup vines data read pointer to check the hop count.
     */
    inreg = input->mci_regptr;
    inreg->argreg = MCI_ETHER_OFFSET + 2*IEEEWORDS;
    inreg->cmdreg = MCI_CMD_RX_SELECT;
    if (inreg->readshort <= MAXETHERSIZE) {
	input->fast_net_start = MCI_ETHER_OFFSET + E_SNAP_HDR_WORDS_IN;
        inreg->argreg = MCI_ETHER_OFFSET + E_SNAP_HDR_WORDS_IN;
        inreg->cmdreg = MCI_CMD_RX_SELECT;
	return(vines_hes_fastswitch(input, inreg));
    } else {
	input->fast_net_start = MCI_ETHER_OFFSET + E_ARPA_HDR_WORDS_IN;
	return(vines_hes_fastswitch(input, inreg));
    }
}

/*******************************************************************
 *
 *******************************************************************/

/*
 * fddi_vines_fastswitch
 *
 * Very high speed switching of SNAP encapsulated FDDI VINES datagram.
 * This routine has been optimized for speed, since it is in the inner
 * loop of the code that handles back-to-back datagrams.
 */
boolean fddi_vines_fastswitch (hwidbtype *input)
{
    mciregtype *inreg;

    /*
     * Setup vines data read pointer to check the hop count.
     */
    inreg = input->mci_regptr;
    input->fast_net_start = FDDI_RECEIVE_OFFSET + F_SNAP_HDR_WORDS_IN;
    inreg->argreg = FDDI_RECEIVE_OFFSET + F_SNAP_HDR_WORDS_IN;
    inreg->cmdreg = MCI_CMD_RX_SELECT;
    return(vines_hes_fastswitch(input, inreg));
}

/*******************************************************************
 *
 *******************************************************************/

/*
 * token_snap_vines_fastswitch
 *
 * Very high speed switching of SNAP encapsulated Token Ring VINES
 * datagram. This routine has been optimized for speed, since it is in
 * the inner loop of the code that handles back-to-back datagrams.
 */
boolean token_snap_vines_fastswitch (hwidbtype *input)
{
    mciregtype *inreg;

    /*
     * Setup vines data read pointer to check the hop count.
     */
    inreg = input->mci_regptr;
    input->fast_net_start = CTR_RECEIVE_OFFSET + T_SNAP_HDR_WORDS_IN;
    inreg->argreg = CTR_RECEIVE_OFFSET + T_SNAP_HDR_WORDS_IN;
    inreg->cmdreg = MCI_CMD_RX_SELECT;
    return(vines_hes_fastswitch(input, inreg));
}

/*******************************************************************
 *
 *******************************************************************/

/*
 * token_vines_fastswitch
 *
 * Very high speed switching of Banyan' SAP encapsulated Token Ring VINES
 * datagram. This routine has been optimized for speed, since it is in
 * the inner loop of the code that handles back-to-back datagrams.
 */
boolean token_vines_fastswitch (hwidbtype *input)
{
    mciregtype *inreg;

    /*
     * Setup vines data read pointer to check the hop count.
     */
    inreg = input->mci_regptr;
    input->fast_net_start = CTR_RECEIVE_OFFSET + T_VINES_HDR_WORDS_IN;
    inreg->argreg = CTR_RECEIVE_OFFSET + T_VINES_HDR_WORDS_IN;
    inreg->cmdreg = MCI_CMD_RX_SELECT;
    return(vines_hes_fastswitch(input, inreg));
}

/*******************************************************************
 *
 *******************************************************************/

/*
 * hdlc_vines_fastswitch
 *
 * Very high speed switching of HDLC encapsulated Vines datagram. This
 * routine has been optimized for speed, since it is in the inner loop of
 * the code that handles back-to-back datagrams.
 *
 * This routine doubles as the PPP input routine.
 */
boolean hdlc_vines_fastswitch (hwidbtype *input)
{
    mciregtype *inreg;

    /*
     * Setup vines data read pointer to check the hop count.
     * Use encsize rather than the equate value so we subtract
     * the correct number of bytes for the MIP.
     */
    inreg = input->mci_regptr;
    input->fast_net_start = MCI_SERIAL_OFFSET + HDLC_HDR_WORDS_IN;
    inreg->argreg = MCI_SERIAL_OFFSET + HDLC_HDR_WORDS_IN;
    inreg->cmdreg = MCI_CMD_RX_SELECT;
    return(vines_hes_fastswitch(input, inreg));
}

/*******************************************************************
 *
 *******************************************************************/

/*
 * frame_vines_fastswitch
 *
 * Very high speed switching of Frame Relay encapsulated VINES datagram.
 */
static boolean frame_vines_fastswitch (
    hwidbtype *input)
{
    mciregtype *inreg;

    /*
     * Setup vines data read pointer to check the hop count.
     */
    inreg = input->mci_regptr;
    input->fast_net_start = MCI_SERIAL_OFFSET + FRAMEC_HDR_WORDS_IN;
    return(vines_hes_fastswitch(input, inreg));
}

/*
 * frame_ietf_vines_fastswitch
 *
 * Very high speed switching of Frame Relay encapsulated VINES datagram.
 */
static boolean frame_ietf_vines_fastswitch (
    hwidbtype *input)
{
    mciregtype *inreg;

    /*
     * Setup vines data read pointer to check the hop count.
     */
    inreg = input->mci_regptr;
    input->fast_net_start = MCI_SERIAL_OFFSET + FRAMEI_HDR_WORDS_IN;
    return(vines_hes_fastswitch(input, inreg));
}
/*
 * atm_dxi_vines_fastswitch
 *
 * Very high speed switching of Frame Relay encapsulated VINES datagram.
 */
static boolean atm_dxi_vines_fastswitch (
    hwidbtype *input)
{
    mciregtype *inreg;

    /*
     * Setup vines data read pointer to check the hop count.
     */
    inreg = input->mci_regptr;
    input->fast_net_start = input->rxoffset +
	btow(input->atm_dxi_stuff->fast_input_encsize);
    return(vines_hes_fastswitch(input, inreg));
}

/*
 * atm_vines_fastswitch
 *
 * Very high speed switching of ATM encapsulated Vines datagram. This
 * routine has been optimized for speed, since it is in the inner loop of
 * the code that handles back-to-back datagrams.
 */
boolean atm_vines_fastswitch (hwidbtype *input)
{
    register mciregtype *inreg;
    ushort offset=0;
    vc_info_t *vc;

    /*
     * Setup vines data read pointer to check the hop count.
     * Use encsize rather than the equate value so we subtract
     * the correct number of bytes for the MIP.
     */
    inreg = input->mci_regptr;
    vc = input->atm_db->fast_vc_info;
    if(vc->etype == ATM_ENCTYPE_NLPID) {
	offset = btow(ATM_NLPID_SNAP_ENCAPBYTES);
    }
    if (vc->etype == ATM_ENCTYPE_LANE) {
	/*
	 * For LANE, if it is a SNAP encapsulation, then set the offset
	 * to reflect that (4 words). Otherwise, assume ARPA encapsulation.
	 */
	inreg = input->mci_regptr;
	inreg->argreg = (input->rxoffset + btow(ATM_AAL5_LANE_ENCAPBYTES) +
			 2*IEEEWORDS);
	inreg->cmdreg = MCI_CMD_RX_SELECT;
	if (inreg->readshort <= MAXETHERSIZE) {
	    offset = btow(ATM_SNAP_HDRBYTES);
	} 
    }

    vc->infast++;
    ATM_INFAST(input);
    input->fast_net_start = input->rxoffset + ATM_HDR_WORDS_IN + offset;
    inreg->argreg = input->fast_net_start;
    inreg->cmdreg = MCI_CMD_RX_SELECT;
    return(vines_hes_fastswitch(input, inreg));
}

/*
 * smds_vines_fastswitch
 *
 * Very high speed switching of SMDS encapsulated Vines datagram. This
 * routine has been optimized for speed, since it is in the inner loop of
 * the code that handles back-to-back datagrams.
 */
static boolean smds_vines_fastswitch (
    hwidbtype *input)
{
    mciregtype *inreg;

    /*
     * Setup vines data read pointer to check the hop count.
     */
    inreg = input->mci_regptr;
    input->fast_net_start = MCI_SERIAL_SMDS_OFFSET + 1 + SMDS_HDR_WORDS_IN;
    inreg->argreg = MCI_SERIAL_SMDS_OFFSET + 1 + SMDS_HDR_WORDS_IN;
    inreg->cmdreg = MCI_CMD_RX_SELECT;
    return(vines_hes_fastswitch(input, inreg));
}

/**********************************************************************
 *
 *		   MISCELLANEOUS HIGH END ROUTINES
 *
 **********************************************************************/

/*
 * vines_clear_fs_tables
 *
 * Vines has been disabled.  Clear out the FS table entries.
 */
void vines_clear_fs_tables (void)
{
    fs_table_fill((fastswitch_type)return_false, "return_false",
		  RXTYPE_VINES);
    fs_table_fill((fastswitch_type)return_false, "return_false",
		  RXTYPE_SNAP_VINES);
    fs_table_fill((fastswitch_type)return_false, "return_false",
		  RXTYPE_SAP_VINES);
}

/*
 * vines_setup_fs_tables
 *
 * Vines has been enabled.  Fill in the FS table entries.
 */
void vines_setup_fs_tables (void)
{
    reg_add_ether_fs(RXTYPE_VINES,
		     (service_ether_fs_type)ether_vines_fastswitch,
			 "ether_vines_fastswitch");
    reg_add_ether_fs(RXTYPE_SNAP_VINES,
		     (service_ether_fs_type)ether_snap_vines_fastswitch,
			 "ether_snap_vines_fastswitch");
    reg_add_hdlc_fs(RXTYPE_VINES,
		    (service_hdlc_fs_type)hdlc_vines_fastswitch,
			"hdlc_vines_fastswitch");
    reg_add_fddi_fs(RXTYPE_VINES,
		    (service_fddi_fs_type)fddi_vines_fastswitch,
			"fddi_vines_fastswitch");
    reg_add_token_fs(RXTYPE_VINES,
		     (service_token_fs_type)token_snap_vines_fastswitch,
			 "token_snap_vines_fastswitch");
    reg_add_token_fs(RXTYPE_SNAP_VINES,
		     (service_token_fs_type)token_vines_fastswitch,
			 "token_vines_fastswitch");
    reg_add_ppp_fs(RXTYPE_VINES, (service_ppp_fs_type)hdlc_vines_fastswitch,
		    "hdlc_vines_fastswitch");
    reg_add_smds_fs(RXTYPE_VINES,
		    (service_smds_fs_type)smds_vines_fastswitch,
		    "smds_vines_fastswitch");
    reg_add_framerelay_fs(RXTYPE_VINES,
			  (service_framerelay_fs_type)frame_vines_fastswitch,
			  "frame_vines_fastswitch");
    reg_add_framerelay_fs(RXTYPE_SNAP_VINES,
			  (service_framerelay_fs_type)frame_ietf_vines_fastswitch,
			  "frame_ietf_vines_fastswitch");
    reg_add_atm_dxi_fs(RXTYPE_VINES,
		       (service_atm_dxi_fs_type)atm_dxi_vines_fastswitch,
		       "atm_dxi_vines_fastswitch");
    reg_add_atm_dxi_fs(RXTYPE_SNAP_VINES,
		       (service_atm_dxi_fs_type)atm_dxi_vines_fastswitch,
		       "atm_dxi_vines_fastswitch");
    reg_add_aip_fs(RXTYPE_VINES,
		    (service_aip_fs_type)atm_vines_fastswitch,
			"atm_vines_fastswitch");
    reg_add_aip_fs(RXTYPE_SNAP_VINES,
		    (service_aip_fs_type)atm_vines_fastswitch,
			"atm_vines_fastswitch");

}

/*
 * vines_fast_supported
 *
 * Has code been written to support this specific high end interface?  If
 * the interface is a serial interface, then we have to also check the
 * link layer protocol.  Some are supported, some aren't.
 */
static boolean vines_fast_supported (int linktype, hwidbtype *hwidb)
{
    mcitype* mci;

    if (!hwidb || (linktype != LINK_VINES))
	return(FALSE);

    /* Serial must be running HDLC */
    if (hwidb->status & IDB_SERIAL) {
	if ((hwidb->enctype != ET_HDLC) &&
	    (hwidb->enctype != ET_PPP) &&
	    (hwidb->enctype != ET_FRAME_RELAY) &&
	    (hwidb->enctype != ET_SMDS) &&
	    (hwidb->enctype != ET_ATM_DXI))
	    return(FALSE);
	if (is_ddr(hwidb))
	    return(FALSE);
	if (is_multilink(hwidb))
	    return(FALSE);
    }

    /* Looks for old MCI/SCI software */
    if (hwidb->status & IDB_MCI) {
	if (!mci_supports_fastswitching(hwidb))
	    return(FALSE);
	mci = hwidb->devctl;
#ifdef C7000
	/*
	 * If its a 7000, this must be a SIP.
	 */
	return(mci->sw_version >= 0x0102);
#else
	/*
	 * Its a CSC/x.  Lots of things to check.
	 */
	if ((mci->hw_version >> 8) == MCI_HARDWARE) {
	    if (mci->sw_version & 0x8000)
		return(mci->sw_version >= 0x8013);
	    return(mci->sw_version >= 0x010A);
	}
	if ((mci->hw_version >> 8) == SCI_HARDWARE) {
	    if (mci->sw_version & 0x8000)
		return(mci->sw_version >= 0x8004);
	    return(mci->sw_version >= 0x0104);
	}
	return(FALSE);
#endif
    }

    /*  Look for Ultra interfaces or old CbusI software */
    if (hwidb->status & IDB_CBUS) {
	mci = hwidb->devctl;
	if ((mci->hw_version >> 8) == CBUS_HARDWARE)
	    return(mci->sw_version >= 0x0202);
	/* CbusII supported in first release. */
	return(TRUE);
    }
    
    /* Allow fast switching on ATM interfaces */
    if (is_atm(hwidb))
	return (TRUE);
    
    return(FALSE);
}

/*
 * vines_mci_fs_register_drivers
 *
 * Fast switching has been enabled or disabled on an interface.
 * Set/clear the fast switching registries appropriately.
 */
static void vines_mci_fs_register_drivers (ulong flags, hwidbtype* idb)
{
    /*
     * Check to see if we're configuring Vines
     */
    if (!(flags & FSF_VINES))
	return;

    if (idb->fast_switch_flag & FSF_VINES)
	vines_setup_fs_tables();
    else 
	vines_clear_fs_tables();
}

/*
 * vines_mci_fs_init
 *
 * Hook the VINES fast switching code into the rest of the router.
 */
static void vines_mci_fs_init (subsystype* subsys)
{
    vfs_mci2mci[V_FS_ETHER_ARPA]  = vines_mci2mci_ether_arpa;
    vfs_mci2mci[V_FS_ETHER_SNAP]  = vines_mci2mci_ether_snap;
    vfs_mci2mci[V_FS_TOKEN_SNAP]  = vines_mci2mci_token_snap;
    vfs_mci2mci[V_FS_TOKEN_VINES] = vines_mci2mci_token_vines;
    vfs_mci2mci[V_FS_FDDI]        = vines_mci2mci_fddi;
    vfs_mci2mci[V_FS_HDLC]        = vines_mci2mci_hdlc;
    vfs_mci2mci[V_FS_FRAME]       = vines_mci2mci_frame;
    vfs_mci2mci[V_FS_FRAME_IETF]  = vines_mci2mci_frame_ietf;
    vfs_mci2mci[V_FS_SMDS_CISCO]  = vines_mci2mci_smds_cisco;
    vfs_mci2mci[V_FS_SMDS_DXI]    = vines_mci2mci_smds_dxi;
    vfs_mci2mci[V_FS_PPP]         = vines_mci2mci_ppp;
    vfs_mci2mci[V_FS_ATM_DXI]	  = vines_mci2mci_atm_dxi;

    vfs_samemci[V_FS_ETHER_ARPA]  = vines_samemci_ether_arpa;
    vfs_samemci[V_FS_ETHER_SNAP]  = vines_samemci_ether_snap;
    vfs_samemci[V_FS_TOKEN_SNAP]  = vines_samemci_token_snap;
    vfs_samemci[V_FS_TOKEN_VINES] = vines_samemci_token_vines;
    vfs_samemci[V_FS_FDDI]        = vines_samemci_fddi;
    vfs_samemci[V_FS_HDLC]        = vines_samemci_hdlc;
    vfs_samemci[V_FS_ATM]	  = vines_samemci_atm;
    vfs_samemci[V_FS_FRAME]       = vines_samemci_frame;
    vfs_samemci[V_FS_FRAME_IETF]  = vines_samemci_frame_ietf;
    vfs_samemci[V_FS_SMDS_CISCO]  = vines_samemci_smds_cisco;
    vfs_samemci[V_FS_SMDS_DXI]    = vines_samemci_smds_dxi;
    vfs_samemci[V_FS_PPP]         = vines_samemci_ppp;
    vfs_samemci[V_FS_ATM_DXI]	  = vines_samemci_atm_dxi;

    reg_add_register_drivers(vines_mci_fs_register_drivers,
			     "vines_mci_fs_register_drivers");
    reg_add_fast_supported(vines_fast_supported,
			   "vines_fast_supported");
}

/*
 * MCI FS subsystem header
 */

#define VINESMCIFS_MAJVERSION 1
#define VINESMCIFS_MINVERSION 0
#define VINESMCIFS_EDITVERSION  1

SUBSYS_HEADER(vines_mci_fs, VINESMCIFS_MAJVERSION, VINESMCIFS_MINVERSION, VINESMCIFS_EDITVERSION,
	      vines_mci_fs_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: vines",
	      "req: fastswitch, vines");
