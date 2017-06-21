/* $Id: fastsrb_cbus.c,v 3.13.4.14 1996/08/26 15:06:28 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/hes/fastsrb_cbus.c,v $
 *------------------------------------------------------------------
 * fastsrb_cbus.c -- Source Route Bridging on cBus systems
 *
 * September 1991, David R. Hampton
 *
 * Copyright (c) 1991-1996, 97  by cisco Systems, Inc.
 * All rights reserved.
 *
 * Includes support for multibus CSC-R16 and cBus CTR Token Ring Interfaces.
 *
 *------------------------------------------------------------------
 * $Log: fastsrb_cbus.c,v $
 * Revision 3.13.4.14  1996/08/26  15:06:28  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.13.4.13  1996/08/11  23:33:20  ppearce
 * CSCdi61195:  RSRB performance degraded 40% from 11.1(1)
 * Branch: California_branch
 *   Erroneous RSRB debug processing -- missing check for debug enabled
 *
 * Revision 3.13.4.12  1996/07/25  23:53:06  motto
 * CSCdi46834:  SRB explorer counters are incr. on CIP I/F for non CIP
 * destined expl
 * Branch: California_branch
 *
 * Revision 3.13.4.11  1996/07/18  20:27:59  ppearce
 * CSCdi63366:  Move SRB counters from hwidb into SRB subblock
 * Branch: California_branch
 *   - Decreases size of hwidb for non-SRB'd interfaces
 *   - Provides additional IOS-independence for SRB
 *   - Moves SRB a little closer to being completely swidb-based
 *
 * Revision 3.13.4.10  1996/07/09  05:50:46  ppearce
 * CSCdi59527:  inlines wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.13.4.9  1996/05/29  18:25:20  motto
 * CSCdi46832:  vencap failed debug messages displayed for non cip
 * explorers
 * Branch: California_branch
 *
 * Revision 3.13.4.8  1996/05/28  15:36:04  ppearce
 * CSCdi58319:  SR/TLB fastswitch fails with RSRB/FST config
 * Branch: California_branch
 *   Code review comments -- better description
 *
 * Revision 3.13.4.7  1996/05/23  14:17:51  ppearce
 * CSCdi58319:  SR/TLB fastswitch fails with RSRB/FST config
 * Branch: California_branch
 *   Use buffer2mci() rather than mci2mci() to copy SR/TLB data
 *
 * Revision 3.13.4.6  1996/05/17  11:17:10  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.11.2.10  1996/05/05  23:06:36  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.11.2.9  1996/05/02  22:02:55  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.11.2.8  1996/04/26  15:18:19  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.11.2.7  1996/04/14  19:30:32  motto
 * Update interface counters for SR/TLB fastswitching accounting.
 * CSCdi53896:  CSNA: Channel is process switching on input
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.11.2.6  1996/04/03  14:28:19  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.11.2.5  1996/03/21  06:07:20  ppearce
 * IBU modularity: Channel subblock
 * Branch: IbuMod_Calif_branch
 *   Move Channel fields from hwidbtype/idbtype structs to Channel subblock
 *
 * Revision 3.11.2.4  1996/03/20  17:53:59  ppearce
 * IBU modularity - NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Code review comment - name changes only
 *
 * Revision 3.11.2.3  1996/03/19  02:04:16  ppearce
 * IBU modularity - NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Incorporate NetBios session access lists into NetBios subblock
 *
 * Revision 3.11.2.2  1996/03/17  17:48:47  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.11.2.1  1996/03/16  12:09:06  ppearce
 * CSCdi50498:  IBU modularity - Implement NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Move NetBios fields from hwidbtype/idbtype structs to NetBios subblock
 *
 * Revision 3.13.4.5  1996/04/04  08:19:39  fred
 * CSCdi53201:  CSCdi49802 incorrectly committed
 * Branch: California_branch
 *         Assessment was incorrect. It was not CBUS Timing, it was an IP
 *         Checksum Error
 *
 * Revision 3.13.4.4  1996/03/30  01:28:26  hwang
 * CSCdi51725:  4r driver need to provide lan manager support
 * Branch: California_branch
 * Enable mac frame receive if srb configured, don't directly
 * call ctr_modify use idb->modify_srb vector instead, now we
 * have 2 cbus token ring driver (trip and vip_4r).
 *
 * Revision 3.13.4.3  1996/03/27  17:44:42  fred
 * CSCdi49802:  rsrb with fst connection fails on 7k and ags+
 * Branch: California_branch
 * Move INCREMENT macro to adjust timing of CBUS RX_MOVE operation
 *
 * Revision 3.13.4.2  1996/03/23  22:48:51  ppearce
 * SR/TLB fastswitching feature
 * Branch: California_branch
 *
 * Revision 3.13.4.1  1996/03/18  19:40:20  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.9.2.5  1996/03/16  06:49:45  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.9.2.4  1996/03/14  00:56:22  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.9.2.3  1996/03/13  01:21:13  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.9.2.2  1996/02/20  00:51:01  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.13  1996/03/07  06:33:55  kchiu
 * CSCdi40165:  Add netbios session level byte filtering support
 *
 * Revision 3.12  1996/02/27  19:27:02  achopra
 * CSCdi45189:  "show atm traffic" shows different (wrong) packet counts
 *
 * Revision 3.11  1996/02/21  02:43:25  hampton
 * Clean up platform specific SRB defines in commonly included header
 * files.  [CSCdi48844]
 *
 * Revision 3.10  1996/02/13  22:08:55  fred
 * CSCdi47956:  WFQ severely degrades RSRB performance
 *              install congestion path in forwarding routine
 *
 * Revision 3.9.2.1  1996/01/24  21:45:21  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.9  1996/01/18  22:46:54  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.8  1996/01/12  04:26:35  motto
 * CSCdi46474:  Unable to send TEST to CIP from local SDLLC - Bad VCN
 *
 * Revision 3.7  1995/12/13  04:19:44  ahh
 * CSCdi44230:  Remove dead code
 * Bring out your dead... *clang* (RSRB/TCP fast-switching)
 *
 * Revision 3.6  1995/12/11  10:18:54  ppearce
 * CSCdi41891:  Both FS and Process counters are updated on the same packet
 *   Decrement FS counters when packet booted to process level
 *
 * Revision 3.5  1995/12/03  23:46:02  psandova
 * CSCdi44673:  Multibus crash in fs_explorer2cbus (1110C0C2)
 *
 * Use 128 byte instead of 256 byte chunks when doing a cbus copy.
 *
 * Revision 3.4  1995/12/03  08:11:45  ppearce
 * CSCdi44430:  Packets not forwarded when RSRB-direct with custom-queueing
 *   Don't fast-switch packet if priority/custom queueing is enabled
 *     on the output interface
 *
 * Revision 3.3  1995/11/29  06:47:01  ppearce
 * CSCdi44617:  CIP-to-TRIP fastswitching broken with TRIP10-3 microcode
 *   Reset ACFC bits correctly before output to TRIP
 *
 * Revision 3.2  1995/11/17  09:16:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:38:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.25  1995/11/08  20:59:52  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.24  1995/10/19  13:26:03  ppearce
 * CSCdi42462:  DLSw+ cannot fast switch packets to/from CIP
 *   Add CIP-specific fast switch code for DLSw+
 *
 * Revision 2.23  1995/09/25  08:33:33  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.22  1995/09/01  05:51:55  ppearce
 * CSCdi39630:  RSRB FST/direct fast-switching to CIP not working
 *   cip_to_vring must include FCS_SIZE in packet length
 *
 * Revision 2.21  1995/08/10  21:12:19  jacobt
 * CSCdi38323:  SRB in 11.0 causes interfaces to reset, ac byte is not set
 * correctly  (two port bridge case)
 *
 * Revision 2.20  1995/08/10  16:06:21  jacobt
 * CSCdi38323:  SRB in 11.0 causes interfaces to reset, ac byte is not set
 * correctly
 *
 * Revision 2.19  1995/08/08  20:22:04  motto
 * CSCdi37835:  SRB fastswitching from FDDI to CIP internal LANs does not
 * work
 *
 * Revision 2.18  1995/08/07  05:33:39  richl
 * CSCdi38315:  makefile rework
 *              move code to tring_inline and to rif_util in libutil.a
 *
 * Revision 2.17  1995/08/04  14:09:57  motto
 * Changed CIP LAN interface into SRB to only have one entry per LAN
 * instead of per adapter.
 * CSCdi37535:  CIP LAN SRB explorers multiplying by # of adapters defined
 * per LAN
 *
 * Revision 2.16  1995/08/01  21:56:41  motto
 * Needed to add support in the srb-explorerfastswitching code for
 * the CIP interanl LAN interfaces.
 * CSCdi37528:  CIP internal LAN SRB fastswitch explorer does not work
 *
 * Revision 2.15  1995/08/01  09:21:21  ogrady
 * CSCdi32342:  update TR_MAXPORTS.
 * Remove constant TR_MAXPORTS.  Determine max number of possible SRB
 * ports dynamically.
 *
 * Revision 2.14  1995/07/25  22:18:29  motto
 * CSCdi37365:  CSNA does not work with 2 CIPs in a C7000
 *
 * Revision 2.13  1995/07/24  23:44:45  ahh
 * CSCdi37573:  RSRB/FTCP broken on high-end
 * Recognize MCI cards properly.
 *
 * Revision 2.12  1995/07/23  16:26:00  motto
 * Modify CIP SRB fastswitch to allow CIPxLANa-2-CIPyLANa
 * and CIPxLANa-2-CIPxLANb.
 * CSCdi37365:  CSNA does not work with 2 CIPs in a C7000
 *
 * Revision 2.11  1995/07/20  19:52:54  ppearce
 * CSCdi37405:  CIP-to-RSRB fastswitching broken for direct/fst/ftcp encaps
 *
 * Revision 2.10  1995/07/12  23:44:50  hampton
 * Convert TCP to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37015]
 *
 * Revision 2.9  1995/07/05 20:48:13  hampton
 * Consolidate common token ring variables into the idb, instead of having
 * them in the private data structure for each type of token ring
 * interface.  Remove overloading of the delay variable by token rings.
 * Eliminate direct references to the system clock.  [CSCdi36721]
 *
 * Revision 2.8  1995/06/26 20:42:34  rbatz
 * CSCdi30326:  Bytes and Packet information INVALID(sho int token ring)
 * When fast srb explorers were transmitted, outputs_fast[ACCT_PROTO_SRB]
 * and tx_cumbytes_fast[ACCT_PROTO_SRB] were not being incremented.  This
 * resulted in incorrect reporting of output stats for "show int tokX".
 *
 * Revision 2.7  1995/06/23  21:40:29  motto
 * CSCdi36373:  CIP SRB fastswitching breaks RSP and AGS images
 *
 * Revision 2.6  1995/06/23  01:30:23  motto
 * Code review cleanup
 *   - Remove debug code.
 *   - Restore explorer fastswitch for non CIP interfaces.
 * CSCdi36240:  SRB fastswitching and process switching broken for CIP
 * v-interface
 *
 * Revision 2.5  1995/06/22  15:31:30  motto
 * CSCdi36240:  SRB fastswitching and process switching broken for CIP
 * v-interface
 *
 * Revision 2.4  1995/06/17  06:25:38  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.3  1995/06/09  19:39:56  alam
 * CSCdi35614:  fast srb fddi<->fddi over vring does not pass netbios
 * traffic
 *
 * Revision 2.2  1995/06/08  23:36:57  alam
 * CSCdi35548:  changes per code review comments
 *
 * Revision 2.1  1995/06/07  20:39:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "../srt/srt_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/channel.h"
#include "channel_sb.h"
#include "../if/rif.h"
#include "../srt/srb.h"
#include "../srt/rsrb.h"
#include "../srt/srb_core.h"
#include "../srt/srb_error.h"
#include "../if/rif_inline.h"
#include "if_mci.h"
#include "if_cmd.h"
#include "if_trip.h"
#include "if_ags_c7000_trip.h"
#include "if_hes_common.h"
#include "../if/fddi.h"
#include "../if/atm.h"
#include "../ibm/netbios.h"
#include "../ibm/netbios_sb.h"
#include "../wan/frame_relay.h"
#include "../ip/ipfast.h"
#include "fastsrb_cbus.h"


extern ipaddrtype rsrb_fst_name;
extern boolean channel_vencap(paktype *, long);

/*
 * Global Storage
 */

charlong *srb_encap;
 

/*
 * srb_incr_vring_counters()
 *   Utility routine to increment SRB counters
 *
 * NOTE: This exact routine exists in three different places
 *           ../srt/fastsrb.c
 *           ../hes/fastsrb_les.c
 *           ../les/fastsrb_cbus.c
 *
 *       Plan on centralizing the routine when time permits --ppearce
 *
 */
static void
srb_incr_vring_counters (hwidbtype *idb, tr_ventrytype *vre, int dgsize)
{
    srbV.srb_sb_incr_tx_nb_counters(firstsw_or_null(idb->vidb_link), dgsize);
    srbV.srb_sb_incr_rx_counters(firstsw_or_null(vre->virtual_idb), dgsize);
    vre->forwards++;
}


/************************************************************************
 *
 *                    FAST SWITCH EXPLORERS
 *
 * Functions for fast switching explorer packets to various media.
 * The process level explorer handling function stores the structure
 * pointers of the output interfaces that the explorer packets are
 * forwarded to.  In this way the interrupt routine learns where
 * explorers are headed and attempts to forward them at interrupt.
 * The packet is then sent to the schedualed process where other time
 * consuming operations can be done out of the critical path.
 *
 ************************************************************************/

#ifdef C7000
/*
 * Fast switch explorers to local TOKENRING and FDDI interfaces.
 * Make use of the SP's CBUS-copy facilities to minimize MBUS-copies.
 * Note that FDDI packets are canonicalized to TOKENRING on input
 * and are odd-aligned ... (sigh).  Since the interface enforces word 
 * alignment when accessing it is not possible to MBUS-copy the FDDI
 * header and then CBUS-copy the remainder of the packet.
 */

static boolean fs_explorer2cbus (
    paktype *pak,
    fs_hdrtype *hcb,
    hwidbtype *oidb)
{
    mciregtype *dstreg = oidb->mci_regptr;
    uchar *newpakhdr = (uchar *)hcb->newtrh;
    int hdrsize = TRING_ENCAPBYTES + hcb->newriflen;
    uchar fddi_hdr[hdrsize];
    ushort chunks = 64;
    uchar *fptr;
    int i, j;
    int encsize;

    /* Calculate encapsulation size of input interface */

    if (hcb->iidb->status & IDB_TR)
        encsize = CTR_RECEIVE_OFFSET + btow(TRING_ENCAPBYTES);
    else if (hcb->iidb->status & IDB_FDDI)
        encsize = FDDI_RECEIVE_OFFSET + btow(FDDI_ENCAPBYTES);
    else if (hcb->iidb->type == IDBTYPE_CIP_LAN_ADAPTER)
        encsize = CHANNEL_TOKEN_RIF_OFFSET;
    else
        encsize = CTR_RECEIVE_OFFSET + btow(TRING_ENCAPBYTES);
    
    /* Reserve a transmit buffer in window 1 of the cbus controller */
    
    dstreg->argreg = oidb->mci_index;
    dstreg->cmdreg = MCI_CMD_SELECT;

    if (dstreg->cmdreg != MCI_RSP_OKAY) {
	hcb->iidb->fs_expl_input->out_drops++;
        return(FALSE);
    }

    dstreg->argreg = oidb->buffer_pool;
    dstreg->cmdreg = MCI_CMD_TX1_RESERVE;

    if (dstreg->cmdreg != MCI_RSP_OKAY) {
	hcb->iidb->fs_expl_input->out_drops++;
        return(FALSE);
    }

    /* Select transmit buffer. */

    dstreg->argreg = 0;
    dstreg->cmdreg = MCI_CMD_TX1_SELECT;

    if (oidb->status & IDB_TR) {

	/* Update the AC|FC bytes and copy out header. */

	*((ushort *)(hcb->newtrh)) = (AC_PRI4 << 8) | FC_TYPE_LLC;

        /* MBUS-copy the header.  */

        buffer2mcis(hcb->newtrh, &dstreg->write1short, TRING_ENCAPBYTES);

        /* MBUS-copy the new rif. */

        buffer2mcis(&hcb->newtrh->rc_blth,&dstreg->write1short,hcb->newriflen);

        /* CBUS-copy the rest of the packet.  */

        dstreg->rx_select = encsize + btow(hcb->pakriflen);

	/* Avoid multibus timeout by copying 128 byte chunks at a time. */

	for(i=hcb->pakdatasize, j=0; i; i -= j)
	    dstreg->rx2tx1bcopy = j = min(i, chunks);

        dstreg->argreg = hcb->dgsize;

    } else if (oidb->status & IDB_FDDI) {

        /* Fddi packets are odd aligned (sigh) ... 
	 * fudge pointers to get it into memd correctly. */

        fptr = &fddi_hdr[0];

	*fptr++ = FDDI_LLC_FC_BYTE;

	/* Bitswap the dst and src mac address. */

        for (i = 2; i < TRING_ENCAPBYTES; i++)
            *fptr++ = BITSWAP(newpakhdr[i]);

        /* Copy new rif into tmp buffer ... it's easier this way. */

        for (; i < hdrsize; i++)
            *fptr++ = newpakhdr[i];

	/* Include the first byte of the original packet data. */

	*fptr = hcb->pakdataptr[0];

        /* Now MBUS-copy the new header. */

	buffer2mcis(fddi_hdr, &dstreg->write1short, hdrsize);

        /* MBUS-copy data from original packet. Start on second byte. */

        buffer2mci(&hcb->pakdataptr[1], &dstreg->write1long, hcb->pakdatasize);
        dstreg->argreg = hcb->dgsize-1;

    } else {
	hcb->iidb->fs_expl_input->out_drops++;
	return(FALSE);
    }

    /* Update the output stats for the idb */
    oidb->counters.outputs_fast[ACCT_PROTO_SRB]++;
    oidb->counters.tx_cumbytes_fast[ACCT_PROTO_SRB]
	+= hcb->pakdatasize + hdrsize;

    dstreg->cmdreg = MCI_CMD_TX1_START;
    return(TRUE);
}

/*
 * Fast switch explorers to internal virtual CHANNEL interfaces.
 */

static boolean fs_cip_explorer2cbus (
    paktype *pak,
    fs_hdrtype *hcb,
    hwidbtype *oidb)
{
    mciregtype *dstreg = oidb->mci_regptr;
    uchar *newpakhdr = (uchar *)hcb->newtrh;
    ushort vc;

    /* Does CIP internal MAC adapter exist on this idb */
    channel_sb_t *chan_sb = channel_get_sb(oidb);
    if (chan_sb) {
        vc = chan_sb->get_vc_from_macaddr(oidb, (uchar *)&newpakhdr[2],
                                                (uchar *)&newpakhdr[8]);
        if (vc == NO_CHANNEL_VC) {
            srbV.srb_sb_incr_tx_drops(oidb->firstsw);
            return(FALSE);
        }
    } else {
        srbV.srb_sb_incr_tx_drops(oidb->firstsw);
        return(FALSE);
    }

    /* Reserve a transmit buffer in window 1 of the cbus controller */
    if (!fci_select_if( oidb, dstreg) || !srb_mci_cmd_tx1_reserve(oidb)) {
	hcb->iidb->fs_expl_input->out_drops++;
        return(FALSE);
    }

    /* Select transmit buffer. */
    dstreg->argreg = 0;
    dstreg->cmdreg = MCI_CMD_TX1_SELECT;

    dstreg->write1long = vc << 16 | 0x2001;
    if (chan_sb->emulated_media == CIP_VLAN_TOKEN) {
        dstreg->write1short = 0x0001;
        *((ushort *)(hcb->newtrh)) = (AC_PRI4 << 8) | FC_TYPE_LLC;
        buffer2mcis(hcb->newtrh, &dstreg->write1short, TRING_ENCAPBYTES);
        buffer2mcis(&hcb->newtrh->rc_blth, &dstreg->write1short,
		    hcb->newriflen);
        buffer2mci(&hcb->pakdataptr[0], &dstreg->write1long, hcb->pakdatasize);
    } else if (chan_sb->emulated_media == CIP_VLAN_FDDI) {
        dstreg->write1short = 0x0002;
	*((ushort *)(hcb->newtrh)) = FDDI_LLC_FC_BYTE;
        /* XXXX Do we need to byteswap FDDI macaddresses? */
        buffer2mcis(hcb->newtrh, &dstreg->write1short, TRING_ENCAPBYTES);
        buffer2mcis(&hcb->newtrh->rc_blth, &dstreg->write1short,
		    hcb->newriflen);
        buffer2mci(&hcb->pakdataptr[0], &dstreg->write1long, hcb->pakdatasize);
    } else {
	hcb->iidb->fs_expl_input->out_drops++;
        return(FALSE);
    }

    /* Update the output stats for the idb */
    oidb->counters.outputs_fast[ACCT_PROTO_SRB]++;
    oidb->counters.tx_cumbytes_fast[ACCT_PROTO_SRB] += hcb->dgsize +
	CHANNEL_TOKEN_ENCAPBYTES;

    dstreg->argreg = hcb->dgsize + CHANNEL_TOKEN_ENCAPBYTES;
    dstreg->cmdreg = MCI_CMD_TX1_START;
    return(TRUE);
}

static void fs_mci_expl_vectors (hwidbtype *idb)
{
    if (is_cip_lan(idb))
        idb->fs_expl_func = fs_cip_explorer2cbus;
    else
        idb->fs_expl_func = fs_explorer2cbus;
}

#else

/*
 * Fast switch explorers to local Cbus token ring.
 */

static boolean fs_explorer2cbus (
    paktype *pak,
    fs_hdrtype *hcb,
    hwidbtype *oidb)
{
    mciregtype *dstreg = oidb->mci_regptr;
    uchar *newpakhdr = (uchar *)hcb->newtrh;
    int hdrsize   = TRING_ENCAPBYTES + hcb->newriflen;
    uchar fddi_hdr[hdrsize];
    uchar *fptr;
    int i;

    /* Reserve a buffer in window 1 of the cbus controller */

    dstreg->argreg = oidb->mci_index;
    dstreg->cmdreg = MCI_CMD_SELECT;
    dstreg->argreg = oidb->buffer_pool;
    dstreg->cmdreg = MCI_CMD_TX1_RESERVE;

    if (dstreg->cmdreg != MCI_RSP_OKAY) {
	hcb->iidb->fs_expl_input->out_drops++;
	return(FALSE);
    }

    dstreg->argreg = 0;
    dstreg->cmdreg = MCI_CMD_TX1_SELECT;

    if (oidb->status & IDB_TR) {

	/* Update the AC|FC bytes and copy out header. */

	*((ushort *)(hcb->newtrh)) = (AC_PRI4 << 8) | FC_TYPE_LLC;

        buffer2mcis(hcb->newtrh, &dstreg->write1short, hdrsize);
  
        /* Copy original packet after the rif. */

        buffer2mci(hcb->pakdataptr, &dstreg->write1long, hcb->pakdatasize);
        dstreg->argreg = hcb->dgsize;
	}

    else if (oidb->status & IDB_FDDI) {

        /* Fddi packets are odd aligned (sigh) ... 
	 * fudge pointers to get it into memd correctly. */

        fptr = &fddi_hdr[0];

	*fptr++ = FDDI_LLC_FC_BYTE;

        /* If interface is FDDIT bitswap dst and src mac address. */

        for (i = 2; i < TRING_ENCAPBYTES; i++) {

	    if (oidb->type == IDBTYPE_FDDIT)
                *fptr++ = BITSWAP(newpakhdr[i]);
            else
                *fptr++ = newpakhdr[i];
	    }

        /* Copy new rif into fddi buffer ... it's easier this way. */

        for (; i < hdrsize; i++)
            *fptr++ = newpakhdr[i];

	/* Include the first byte of the original packet data. */

	*fptr = hcb->pakdataptr[0];

        /* Now MBUS-copy the new fddi header. */

	buffer2mcis(fddi_hdr, &dstreg->write1short, hdrsize);

        /* MBUS-copy data from original packet. Start on second byte. */

        buffer2mci(&hcb->pakdataptr[1], &dstreg->write1long, hcb->pakdatasize);
        dstreg->argreg = hcb->dgsize-1;
        }
    else {
	hcb->iidb->fs_expl_input->out_drops++;
	return(FALSE);
    }

    /* Update the output stats for the idb */
    oidb->counters.outputs_fast[ACCT_PROTO_SRB]++;
    oidb->counters.tx_cumbytes_fast[ACCT_PROTO_SRB]
	+= hcb->pakdatasize + hdrsize;

    dstreg->cmdreg = MCI_CMD_TX1_START;

    return(TRUE);
}

static void fs_mci_expl_vectors (hwidbtype *idb)
{
    idb->fs_expl_func = fs_explorer2cbus;
}

#endif C7000


/**********************************************************************
 * 
 *		       FAST DIRECT RSRB OUTPUT
 *
 * This set of routines supports the direct encapsulation of RSRB packets
 * over the various types of interfaces.  This is the output (token ring
 * -> rsrb) path only.  The input path is elsewhere.  These routines do
 * not have to perform any assess checking as the interfaces involved are
 * transit interfaces, no destination interfaces.
 *
 * Fast direct RSRB output routines which are shared by DLSw can be
 * found in fastsrb_cbus.h
 * 
 **********************************************************************/

inline static void fst_write_token2_header (
    uchar *odgstart,
    ipcache_rntype *cptr,
    riftype *rifptr,
    ushort *fst_ptr,
    int size,
    int rn,
    tr_vpeertype *vrp)
    
{
    ushort *data, *rif;
    int i;

    /*
     * Write the Mac header
     */
    data = (ushort *)odgstart;
    *data++ = (AC_PRI4 << 8) | FC_TYPE_LLC;
    *data++ = cptr->m.macshort[0];
    *data++ = cptr->m.macshort[1];
    *data++ = cptr->m.macshort[2];
    if (rifptr && rifptr->length)
	*data++ = cptr->m.macshort[3] | (TR_RII << 8);
    else
	*data++ = cptr->m.macshort[3];
    *data++ = cptr->m.macshort[4];
    *data++ = cptr->m.macshort[5];
    if (rifptr && rifptr->length) {
	for (i=0, rif=(ushort *)rifptr->rif; i < rifptr->length; i+=2)
	    *data++ = *rif++;
    }
    *data++ = SNAPSNAP;
    *data++ = (LLC1_UI << 8) | 0;
    *data++ = 0x0000;
    *data++ = TYPE_IP10MB;

    /* writing ip header */
    *data++ = fst_ptr[0];
    *data++ = fst_ptr[1];
    *data++ = fst_ptr[2];
    *data++ = fst_ptr[3];
    *data++ = fst_ptr[4];
    *data++ = fst_ptr[5];
    *data++ = fst_ptr[6];
    *data++ = fst_ptr[7];
    *data++ = fst_ptr[8];
    *data++ = fst_ptr[9];


    *data++ = vrp->vrp_version;		/* version */
    *data++ = RSRB_OP_FORWARDFST;		/* op      */
    *data++ = rn;			/* trn     */
    *data++ = vrp->vrg->vrn;		/* vrn     */
    *data++ = 0;			/* offset  */
    /* Note that Len and Param are *not* placed in the frame! */
}

inline static void srb_write_serial_header (mciregtype *regptr,
			hwidbtype *out, tr_vpeertype *vrp)
{
     
    if (out->cache_board_encap) {
	ushort temp;
	(*out->cache_board_encap)(out,&temp,sizeof(ushort));
	regptr->write1short = temp;
    }
    if (vrp->type == RSRB_PT_FR) {
        fr_idb_struct_type *fr_idb;
	ushort dlci;
        long ldlci;
        fr_idb = out->frame_relay_stuff;
	dlci = NUM_TO_DLCI(vrp->dlci);
	ldlci = (long) (((dlci|FRAME_RELAY_EA_BITS) <<16) | TYPE_RSRB);
        regptr->write1long = ldlci;
    } else if (vrp->type == RSRB_PT_IF) {
    /*
     * Write the Mac header
     */
        regptr->write1long = HDLC_RSRB;
    }
}

inline static void srb_write_ether_header (
    mciregtype *regptr,
    uchar *dst,
    uchar *src,
    int length)
{
    /*
     * Write the Mac header
     */
    regptr->write1short = *(ushort *)(dst+0);
    regptr->write1short = *(ushort *)(dst+2);
    regptr->write1short = *(ushort *)(dst+4);
    regptr->write1short = *(ushort *)(src+0);
    regptr->write1short = *(ushort *)(src+2);
    regptr->write1short = *(ushort *)(src+4);
    regptr->write1short = length;
    regptr->write1short = SNAPSNAP;
    regptr->write1short = (LLC1_UI << 8) | 0;
    regptr->write1short = 0x0000;
    regptr->write1short = TYPE_RSRB;
}

inline static void srb_write_fddi_header (
    mciregtype *regptr,
    uchar *dst,
    uchar *src)
{
    /*
     * Write the Mac header
     */
    regptr->write1short = FDDI_LLC_FC_BYTE;
    regptr->write1short = *(ushort *)(dst+0);
    regptr->write1short = *(ushort *)(dst+2);
    regptr->write1short = *(ushort *)(dst+4);
    regptr->write1short = *(ushort *)(src+0);
    regptr->write1short = *(ushort *)(src+2);
    regptr->write1short = *(ushort *)(src+4);
    regptr->write1short = SNAPSNAP;
    regptr->write1short = (LLC1_UI << 8) | 0;
    regptr->write1short = 0x0000;
    regptr->write1short = TYPE_RSRB;
}

inline static void srb_write_token_header (
    mciregtype *regptr,
    uchar *dst,
    uchar *src,
    riftype *rifptr)
{
    int i;
    ushort *rif;

    /*
     * Write the Mac header
     */
    regptr->write1short = (AC_PRI4 << 8) | FC_TYPE_LLC;
    regptr->write1short = *(ushort *)(dst+0);
    regptr->write1short = *(ushort *)(dst+2);
    regptr->write1short = *(ushort *)(dst+4);
    if (rifptr && rifptr->length)
	regptr->write1short = *(ushort *)(src+0) | (TR_RII << 8);
    else
	regptr->write1short = *(ushort *)(src+0);
    regptr->write1short = *(ushort *)(src+2);
    regptr->write1short = *(ushort *)(src+4);
    if (rifptr && rifptr->length) {
	for (i=0, rif=(ushort *)rifptr->rif; i < rifptr->length; i+=2)
	    regptr->write1short = *rif++;
    }
    regptr->write1short = SNAPSNAP;
    regptr->write1short = (LLC1_UI << 8) | 0;
    regptr->write1short = 0x0000;
    regptr->write1short = TYPE_RSRB;
}

inline static void srb_write_token2_header (
    uchar *odgstart,
    int odgsize,
    tr_vpeertype *vrp,
    uchar *src,
    ushort rn,
    riftype *rifptr)
{
    ushort *data, *rif;
    int i;

    /*
     * Write the Mac header
     */
    data = (ushort *)odgstart;
    *data++ = (AC_PRI4 << 8) | FC_TYPE_LLC;
    *data++ = *(ushort *)(vrp->macaddr+0);
    *data++ = *(ushort *)(vrp->macaddr+2);
    *data++ = *(ushort *)(vrp->macaddr+4);
    if (rifptr && rifptr->length)
	*data++ = *(ushort *)(src+0) | (TR_RII << 8);
    else
	*data++ = *(ushort *)(src+0);
    *data++ = *(ushort *)(src+2);
    *data++ = *(ushort *)(src+4);
    if (rifptr && rifptr->length) {
	for (i=0, rif=(ushort *)rifptr->rif; i < rifptr->length; i+=2)
	    *data++ = *rif++;
    }
    *data++ = SNAPSNAP;
    *data++ = (LLC1_UI << 8) | 0;
    *data++ = 0x0000;
    *data++ = TYPE_RSRB;
    /*
     * Write the RSRB header.
     */
    *data++ = vrp->vrp_version;		/* version */
    *data++ = RSRB_OP_FORWARD;		/* op      */
    *data++ = rn;			/* trn     */
    *data++ = vrp->vrg->vrn;		/* vrn     */
    *data++ = 0;			/* offset  */
    *data++ = odgsize;			/* len     */
    *data++ = 0;			/* param   */
}

/***********************************************************************
 *
 * Common pieces of code written as inline functions so that they can be
 * mixed and matched.  They cannot be written as inline functions because
 * of the number of parameters they generate that are used later.
 *
 **********************************************************************/

#define MAC_ACL_BYTES (TR_MAXENCSIZE + sizeof(netbios_header))
static uchar mac_acl[MAC_ACL_BYTES];

static inline boolean fs_srb_access_check (
    hwidbtype *idb,
    hwidbtype *oidb,
    uchar *dgstart,
    int dgsize,
    int riflen)
{
    /*
     * Check the access lists.
     */
    if (idb->srb_access_in && !srb_in_check(dgstart, dgsize, idb, riflen))
	return(FALSE);
    if (oidb->srb_access_out && !srb_out_check(dgstart, dgsize, oidb, riflen))
	return(FALSE);
    return(TRUE);
}

static inline boolean fs_netbios_input_acl_check (hwidbtype *hwidb)
{
    return(netbios_sb_ibm_b_iacf_is_enabled(hwidb->firstsw) ||
           (netbios_bytes_accQ.qhead && hwidb->access_expression_in));
}

static inline boolean fs_netbios_output_acl_check (hwidbtype *hwidb)
{
    return(netbios_sb_ibm_b_oacf_is_enabled(hwidb->firstsw) ||
           (netbios_bytes_accQ.qhead && hwidb->access_expression_out));
}


/**********************************************************************
 *
 * These routines handle all possible two port bridges in a router.
 *
 **********************************************************************/

/*
 * srb_tr_fastswitch
 *
 * From a CTR to token ring or FDDI
 */

#define SRB_FAST_SETUP \
    mciregtype *regptr = idb->mci_regptr; \
    uchar *dgstart     = (uchar *)&regptr->readlong; \
    int dgsize         = idb->rxlength + FCS_SIZE; \
    int riflen         = idb->rxrifwordlen * 2; 

static boolean srb_tr_fastswitch (
    hwidbtype *idb,
    void *dummy,
    hwidbtype *oidb)
{
    SRB_FAST_SETUP
    boolean mac_read;
    boolean write_addr;
    ushort vc;

    mac_read = write_addr = FALSE;
    srb_encap = (charlong *)(mac_acl + TR_ACFC_SIZE);

    srb_in_counters(idb, dgsize);
    if (idb->srb_access_in || oidb->srb_access_out) {
	srb_mci_rx_select_offset_inline(idb, CTR_RECEIVE_OFFSET);
	mci2buffer(&regptr->readlong, mac_acl, MAC_ACL_BYTES);
	if (!fs_srb_access_check(idb, oidb, mac_acl, dgsize, riflen)) {
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;
	    return(TRUE);
	}
	mac_read = TRUE;
    }
    if (oidb->state != IDBS_UP) {
	regptr->cmdreg = MCI_CMD_RX_FLUSH;
        srbV.srb_sb_incr_tx_drops(oidb->firstsw);
	return(TRUE);
    }

    if (regptr == oidb->mci_regptr) {
	if (is_fddi(oidb)) {
	    if (oidb->type == IDBTYPE_FDDIT) {
		if (!mac_read) {
		    srb_mci_read_mac_header_inline(idb,
						   CTR_RECEIVE_OFFSET
						   + 1);
		}
		srb_mci_swap_mac_address_inline();
		write_addr = TRUE;
	    }
	    regptr->argreg = oidb->mci_index;
	    regptr->cmdreg = MCI_CMD_RX_MOVE;
	    srb_out_counters(oidb, dgsize);
            if (srb_debug) {
                SrbDebug_FastForward(idb);
            }
	    if (regptr->cmdreg == MCI_RSP_OKAY) {
		srb_mci_tx1_select_offset_inline(idb, CTR_RECEIVE_OFFSET);
		if (write_addr)
		    srb_mci_fddi_mac_inline(idb);
		else
		    regptr->write1short = FDDI_LLC_FC_BYTE;

		dgsize -= (FCS_SIZE + TRING_ENCAPBYTES - FDDI_ENCAPBYTES);
				       
		srb_mci_tx1_start_inline(idb, dgsize | MCI_TX_ODDALIGN);
	    } else {
		regptr->cmdreg = MCI_CMD_RX_FLUSH;
                oidb->counters.output_total_drops++;
	    }
	} else if (is_tokenring(oidb)) {
	    /* Output is tring */
            if (srb_mci_rx_move_select_inline(idb, oidb, 
					      CTR_RECEIVE_OFFSET, dgsize)) {
		regptr->write1short = AC_PRI4 << 8 | FC_TYPE_LLC;
                if (srb_debug) {
		    SrbDebug_FastForward(idb);
                }
	        dgsize -= FCS_SIZE;
	        srb_mci_tx1_start_inline(idb, dgsize);
	    }
	} else if (is_cip_lan(oidb)) {
            channel_sb_t *chan_sb = channel_get_sb(oidb);
            if (!mac_read) {
		srb_mci_read_mac_header_inline(idb, CTR_RECEIVE_OFFSET + 1);
            }
            if (chan_sb) {
	        vc = chan_sb->get_vc_from_macaddr(oidb, &SRB_DA_BYTES, &SRB_SA_BYTES);
            } else {
                vc = NO_CHANNEL_VC;
            }
            if (vc != NO_CHANNEL_VC) {
		if (chan_sb->emulated_media == CIP_VLAN_TOKEN) {
		    dgsize -= (FCS_SIZE - CHANNEL_TOKEN_ENCAPBYTES); 
	            if (srb_mci_rx_move_select_inline(idb, oidb, 
						      CTR_RECEIVE_OFFSET -
						      btow(CHANNEL_TOKEN_ENCAPBYTES),
						      dgsize)) {
		        srb_mci_cip_token_encap_inline(idb, vc); 
		        srb_mci_tx1_start_inline(idb, dgsize);
                        if (srb_debug) {
                            SrbDebug_FastForward(idb);
                        }
		    }
		} else if (chan_sb->emulated_media == CIP_VLAN_FDDI) { 
		    dgsize -= (FCS_SIZE - CHANNEL_TOKEN_ENCAPBYTES); /* XXX */
	            if (srb_mci_rx_move_select_inline(idb, oidb,
						      CTR_RECEIVE_OFFSET -
						      btow(CHANNEL_TOKEN_ENCAPBYTES),
						      dgsize)) {
			/* XXXbitswap needed?*/ 
		        srb_mci_cip_fddi_mac_inline(idb, vc);
		        srb_mci_tx1_start_inline(idb, dgsize);
                        if (srb_debug) {
                            SrbDebug_FastForward(idb);
                        }
		    }
		} else {
	            regptr->cmdreg = MCI_CMD_RX_FLUSH;
                    srbV.srb_sb_incr_tx_drops(oidb->firstsw);
                }
	    } else {
	        regptr->cmdreg = MCI_CMD_RX_FLUSH;
                srbV.srb_sb_incr_tx_drops(oidb->firstsw);
	    }
	} else {
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;
            srbV.srb_sb_incr_tx_drops(oidb->firstsw);
	}
    } else {
	srb_mci_rx_select_offset_inline(idb, CTR_RECEIVE_OFFSET);
	if ((*oidb->if_srb_hes_fastout)(idb, oidb, dgstart, dgsize, NULL, 0,
			      idb)) {
	    srb_out_counters(oidb, dgsize);
            if (srb_debug) {
                SrbDebug_FastForward(idb);
            }
	}
        regptr->cmdreg = MCI_CMD_RX_FLUSH;
    }
    return(TRUE);
}

static boolean srb_fddi_fastswitch (
    hwidbtype *idb,
    void *dummy,
    hwidbtype *oidb)
{
    SRB_FAST_SETUP
    boolean mac_read, write_addr;
    ushort vc;

    mac_read = write_addr = FALSE;
    srb_encap = (charlong *)(mac_acl + TR_ACFC_SIZE);

    dgsize &= ~(MCI_TX_ODDALIGN | MCI_RSP_ERR_UNK);
    srb_in_counters(idb, dgsize);
    if (idb->srb_access_in || oidb->srb_access_out) {
	srb_mci_rx_select_offset_inline(idb, FDDI_RECEIVE_OFFSET);
	mci2buffer(&regptr->readlong, mac_acl, MAC_ACL_BYTES);
	if (!fs_srb_access_check(idb, oidb, mac_acl, dgsize, riflen)) {
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;
	    return(TRUE);
	}
	mac_read = TRUE;
    }
    if (oidb->state != IDBS_UP) {
	regptr->cmdreg = MCI_CMD_RX_FLUSH;
        srbV.srb_sb_incr_tx_drops(oidb->firstsw);
	return(TRUE);
    }

    if (regptr == oidb->mci_regptr) {
	if (((idb->type == IDBTYPE_FDDIT) &&
	     (oidb->type != IDBTYPE_FDDIT)) ||
	    ((idb->type != IDBTYPE_FDDIT) &&
	     (oidb->type == IDBTYPE_FDDIT))) {
	    if (!mac_read) {
		srb_mci_read_mac_header_inline(idb,
					       FDDI_RECEIVE_OFFSET
					       + 1);
	        mac_read = TRUE;
            }
	    srb_mci_swap_mac_address_inline();
	    write_addr = TRUE;
	}
	if (is_fddi(oidb)) {
	    regptr->argreg = oidb->mci_index;
	    regptr->cmdreg = MCI_CMD_RX_MOVE;
	    srb_out_counters(oidb, dgsize);
            if (srb_debug) {
                SrbDebug_FastForward(idb);
            }
	    if (regptr->cmdreg == MCI_RSP_OKAY) {
	        srb_mci_tx1_select_offset_inline(idb,
			    		         FDDI_RECEIVE_OFFSET);
		if (write_addr)
		    srb_mci_fddi_mac_inline(idb);
		else
		    regptr->write1short = FDDI_LLC_FC_BYTE;
		dgsize -= FCS_SIZE;
		dgsize |= MCI_TX_ODDALIGN;
	    } else {
	        regptr->cmdreg = MCI_CMD_RX_FLUSH;
                oidb->counters.output_total_drops++;
                return(TRUE);
	    }
	} else if (is_tokenring(oidb)) {
	    regptr->argreg = oidb->mci_index;
	    regptr->cmdreg = MCI_CMD_RX_MOVE;
	    srb_out_counters(oidb, dgsize);
            if (srb_debug) {
                SrbDebug_FastForward(idb);
            }
	    if (regptr->cmdreg == MCI_RSP_OKAY) {
	        srb_mci_tx1_select_offset_inline(idb,
					         CTR_RECEIVE_OFFSET);
		if (write_addr)
		    srb_mci_tring_mac_inline(idb);
		else
		    regptr->write1short = AC_PRI4 << 8 | FC_TYPE_LLC;
		dgsize -= (FCS_SIZE + FDDI_ENCAPBYTES - TRING_ENCAPBYTES);
		dgsize &= ~MCI_TX_ODDALIGN;
	    } else {
	        regptr->cmdreg = MCI_CMD_RX_FLUSH;
                oidb->counters.output_total_drops++;
                return(TRUE);
	    }
	} else if (is_cip_lan(oidb)) {
            channel_sb_t *chan_sb = channel_get_sb(oidb);
            if (!mac_read) {
		srb_mci_read_mac_header_inline(idb, FDDI_RECEIVE_OFFSET+1);
            }
            if (chan_sb) {
	        vc = chan_sb->get_vc_from_macaddr(oidb, &SRB_DA_BYTES, &SRB_SA_BYTES);
            } else {
                vc = NO_CHANNEL_VC;
            }
	    if (vc != NO_CHANNEL_VC) {
		dgsize -= (FCS_SIZE + FDDI_ENCAPBYTES - TRING_ENCAPBYTES);
		dgsize += CHANNEL_TOKEN_ENCAPBYTES;
		dgsize &= ~MCI_TX_ODDALIGN;
                if (chan_sb->emulated_media == CIP_VLAN_TOKEN) {
	            if (srb_mci_rx_move_select_inline(idb, oidb,
						      FDDI_RECEIVE_OFFSET -
						      btow(CHANNEL_TOKEN_ENCAPBYTES),
						      dgsize)) {
    			srb_mci_cip_token_mac_inline(idb, vc); 
		    }
		} else if (chan_sb->emulated_media == CIP_VLAN_FDDI) {
	            if (srb_mci_rx_move_select_inline(idb, oidb,
						      FDDI_RECEIVE_OFFSET -
						      btow(CHANNEL_TOKEN_ENCAPBYTES),
						      dgsize)) {
			/* XXX bitswap needed? */
			srb_mci_cip_fddi_mac_inline(idb, vc);
		    }
		} else {
	            regptr->cmdreg = MCI_CMD_RX_FLUSH;
                    srbV.srb_sb_incr_tx_drops(oidb->firstsw);
                    return(TRUE);
		}
                if (srb_debug) {
                    SrbDebug_FastForward(idb);
                }
	    } else {
	        regptr->cmdreg = MCI_CMD_RX_FLUSH;
                srbV.srb_sb_incr_tx_drops(oidb->firstsw);
                return(TRUE);
	    }
	} else {
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;
            oidb->counters.output_total_drops++;
            return(TRUE);
	}
	srb_mci_tx1_start_inline(idb, dgsize);
    } else { 
	if (idb->type == IDBTYPE_FDDIT) {
	    if (!mac_read) {
		srb_mci_read_mac_header_inline(idb,
					       FDDI_RECEIVE_OFFSET
					       + 1);
	    }
	    srb_mci_swap_mac_address_inline();
	}
	srb_mci_rx_select_offset_inline(idb, FDDI_RECEIVE_OFFSET);
	if ((*oidb->if_srb_hes_fastout)(idb, oidb, dgstart, dgsize, NULL, 0,
			      idb)) {
	    srb_out_counters(oidb, dgsize);
            if (srb_debug) {
                SrbDebug_FastForward(idb);
            }
	}
	regptr->cmdreg = MCI_CMD_RX_FLUSH;
    }
    return(TRUE);
}

/*
 * input channel, emulating token or fddi
 * output token, fddi
 */
static boolean srb_cip_fastswitch (
    hwidbtype *idb,
    void *dummy,
    hwidbtype *oidb)
{
    SRB_FAST_SETUP
    boolean mac_read;
    boolean write_addr;

    mac_read = write_addr = FALSE;
    dgsize -= FCS_SIZE;
    srb_in_counters(idb, dgsize);
    dgsize -= sizeof(channel_hdr);
    dgsize += 2;	     /* adjust by media size - 2 for TR, 1 for fddi */

    srb_encap = (charlong *)(mac_acl + 2);

    if (oidb->state != IDBS_UP) {
	regptr->cmdreg = MCI_CMD_RX_FLUSH;
        srbV.srb_sb_incr_tx_drops(oidb->firstsw);
	return(TRUE);
    }

    if (regptr == oidb->mci_regptr) {
	if (is_fddi(oidb)) {
	    if (oidb->type == IDBTYPE_FDDIT) {
		srb_mci_read_mac_header_inline(idb,
						CHANNEL_TOKEN_ACFC_OFFSET
						+ 1);
		/*
		 * XXX only need this if we are playing ctr, not fddi, on
		 * input
		 */
		srb_mci_swap_mac_address_inline();
		write_addr = TRUE;
	    }
	    dgsize -= (TRING_ENCAPBYTES - FDDI_ENCAPBYTES);
            if (srb_mci_rx_move_select_inline(idb, oidb, 
                                              CHANNEL_TOKEN_ACFC_OFFSET,
					      dgsize)) {
		if (write_addr)
		    srb_mci_fddi_mac_inline(idb);
		else
		    regptr->write1short = FDDI_LLC_FC_BYTE;
		srb_mci_tx1_start_inline(idb, dgsize | MCI_TX_ODDALIGN);
                if (srb_debug) {
                    SrbDebug_FastForward(idb);
                }
	    }
	} else if (is_tokenring(oidb)) {
	    if (srb_mci_rx_move_select_inline(idb, oidb,
					      CHANNEL_TOKEN_ACFC_OFFSET,
					      dgsize)) {
                regptr->write1short = (AC_PRI4 << 8) | FC_TYPE_LLC;
		srb_mci_tx1_start_inline(idb, dgsize);
                if (srb_debug) {
                    SrbDebug_FastForward(idb);
                }
	    }
	} else if (is_cip_lan(oidb)) {
            ushort vc = NO_CHANNEL_VC;
            channel_sb_t *chan_sb = channel_get_sb(oidb);
	    srb_mci_read_mac_header_inline(idb, CHANNEL_TOKEN_ACFC_OFFSET+1);
            if (chan_sb) {
	        vc = chan_sb->get_vc_from_macaddr(oidb, &SRB_DA_BYTES, &SRB_SA_BYTES);
            }
            if (vc != NO_CHANNEL_VC) {
		if (chan_sb->emulated_media == CIP_VLAN_TOKEN) {
		    dgsize += CHANNEL_TOKEN_ENCAPBYTES;
                    if (srb_mci_rx_move_select_inline(idb, oidb, 
                                                      CHANNEL_RECEIVE_OFFSET,
						      dgsize)) {
			srb_mci_cip_token_encap_inline(idb, vc);
	                srb_mci_tx1_start_inline(idb, dgsize);
                        if (srb_debug) {
                            SrbDebug_FastForward(idb);
                        }
                    } 
		} else if (chan_sb->emulated_media == CIP_VLAN_FDDI) {
		    dgsize += CHANNEL_TOKEN_ENCAPBYTES;
                    if (srb_mci_cmd_rx_move(oidb, dgsize)) {
	                srb_mci_tx1_select_offset_inline(idb,
			                                 CHANNEL_RECEIVE_OFFSET);
			srb_mci_cip_fddi_mac_inline(idb, vc); /*XXX bitswap?*/
	                srb_mci_tx1_start_inline(idb, dgsize);
                        if (srb_debug) {
                            SrbDebug_FastForward(idb);
                        }
		    }
                } else {
	            regptr->cmdreg = MCI_CMD_RX_FLUSH;
                    srbV.srb_sb_incr_tx_drops(oidb->firstsw);
                }
	    } else {
	        regptr->cmdreg = MCI_CMD_RX_FLUSH;
                srbV.srb_sb_incr_tx_drops(oidb->firstsw);
            }
	} else {
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;
            srbV.srb_sb_incr_tx_drops(oidb->firstsw);
        }
    } else {
	dgstart++;		/* keep compiler happy */
        riflen++;		/* keep compiler happy */
	buginf("\nsrb_cip_fastswitch reached impossible case!");
	regptr->cmdreg = MCI_CMD_RX_FLUSH;
    }
    return(TRUE);
}

/*
 * These routines handle all token ring to virtual ring bridges.  Thes
 * routines only handle the direction of TR -> VRING.  Another set of
 * routines handles the opposite direction.
 *
 **********************************************************************/

/*
 *
 */

static boolean srb_ctr_to_vring (
    hwidbtype *idb,
    void *dummy2,
    hwidbtype *dummy)
{
    SRB_FAST_SETUP
    hwidbtype *oidb;
    ushort rc, dest;
    ulong temp;
    ushort bn;
    ushort rn = 0;
    tr_vpeertype *vrp;
    tr_ventrytype *vre;
    paktype *pak;
    boolean mac_read, write_addr;
    ushort vc;

    mac_read = write_addr = FALSE;
    srb_encap = (charlong *)(mac_acl + TR_ACFC_SIZE);
    srb_in_counters(idb, dgsize);

    /*
     * Gather all the information we will need to forward this frame.
     * Putting all the bail checks at the end means less if statements to
     * slow down the code.
     */
    regptr->argreg = CTR_RECEIVE_OFFSET + (TRING_ENCAPBYTES >> 1);
    regptr->cmdreg = MCI_CMD_RX_SELECT;
    rc = regptr->readshort;
    if (idb->rxsrbinfo & FCI_RXSRB_DIR) {
	regptr->argreg = CTR_RECEIVE_OFFSET + (TRING_ENCAPBYTES >> 1) + 1;
	regptr->cmdreg = MCI_CMD_RX_SELECT;
	dest = regptr->readshort >> 4;
	regptr->argreg = CTR_RECEIVE_OFFSET + (TRING_ENCAPBYTES >> 1) +
	    ((idb->rxsrbinfo & FCI_RXSRB_RDOFFSET) >> 8) - 1;
	regptr->cmdreg = MCI_CMD_RX_SELECT;
	temp = regptr->readshort;
	rn = (temp >> 4) & 0x0FFF;
	bn = (temp >> 0) & 0x000F;
    } else {
	regptr->argreg = CTR_RECEIVE_OFFSET + (TRING_ENCAPBYTES >> 1) +
	    (idb->rxsrbinfo & FCI_RXSRB_LENGTH) - 1;
	regptr->cmdreg = MCI_CMD_RX_SELECT;
	dest = regptr->readshort >> 4;
	regptr->argreg = CTR_RECEIVE_OFFSET + (TRING_ENCAPBYTES >> 1) +
	    ((idb->rxsrbinfo & FCI_RXSRB_RDOFFSET) >> 8) + 1;
	regptr->cmdreg = MCI_CMD_RX_SELECT;
	temp = regptr->readlong;
	bn = (temp >> 16) & 0x000F;
	rn = (temp >>  4) & 0x0FFF;
    }

    /*
     * Bail checks go here.  'Traced' frames must be handed by the system
     * so that the appropriate response may be generated.
     */
    if (rc & ((RC_BROADBIT | RC_TRACE_BOTH) << 8)) {
        srb_in_backout_counters(idb, dgsize);
	return(FALSE);
    }

    /*
     * ------------------------------------------------------------------
     * DLSw Fast Remote Peer Demux.
     * If rif terminates on the virtual ring, packet may be destined for
     * a DLSw peer (DLSw performs rif termination). Could also be for LNM
     * so fall thhrough if DLSw fast switch fails.
     *
     * ------------------------------------------------------------------
     */
    if (dest == srbV.srb_sb_get_targetring(idb->firstsw)) {
        if (reg_invoke_dlsw_srb_mci_input(idb, dgstart, dgsize, riflen)) {
            return(TRUE);
        } 
        srb_in_backout_counters(idb, dgsize);
        return(FALSE);
    }

    /*
     * Now find the table entry for the destination cisco on the other
     * side of the virtual ring.
     */
    vre = fast_srb_get_vre(idb->tr_vring_blk, rn, bn);

    /*
     * If there is not a forward target, return FALSE now, because this
     * frame needs to be handled by the processor to attempt to reopen
     * previous dead remote peers (proxy explorer forces this...)
     */
    if (!vre) {
        srb_in_backout_counters(idb, dgsize);
	return(FALSE);
    }

    if (vre->type == RSRB_ET_LOCAL) {
	oidb = vre->outhole.idb;
	if (!oidb->srb_routecache) {
            srb_in_backout_counters(idb, dgsize);
	    return(FALSE);
        }
	if (idb->srb_access_in || oidb->srb_access_out) {
	    mci2buffer(&regptr->readlong, mac_acl, MAC_ACL_BYTES);
	    if (!fs_srb_access_check(idb, oidb, mac_acl, dgsize, riflen)) {
		regptr->cmdreg = MCI_CMD_RX_FLUSH;
		return(TRUE);
	    }
	    mac_read = TRUE;
	}
	if (oidb->state != IDBS_UP) {
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;
            srbV.srb_sb_incr_tx_drops(oidb->firstsw);
	    return(TRUE);
	}
	if (regptr == oidb->mci_regptr) {
	    if (is_fddi(oidb)) {
		if (oidb->type == IDBTYPE_FDDIT) {
		    if (!mac_read) {
			srb_mci_read_mac_header_inline(idb,
						       CTR_RECEIVE_OFFSET
						       + 1);
		    }
		    srb_mci_swap_mac_address_inline();
		    write_addr = TRUE;
		}
		regptr->argreg = oidb->mci_index;
		regptr->cmdreg = MCI_CMD_RX_MOVE;
		srb_out_counters(oidb, dgsize);
                srb_incr_vring_counters(idb, vre, dgsize);
		if (regptr->cmdreg == MCI_RSP_OKAY) {
		    srb_mci_tx1_select_offset_inline(idb, CTR_RECEIVE_OFFSET);
		    if (write_addr)
			srb_mci_fddi_mac_inline(idb);
		    else
			regptr->write1short = FDDI_LLC_FC_BYTE;
		    
		    dgsize -= (FCS_SIZE + TRING_ENCAPBYTES - FDDI_ENCAPBYTES);
			       
		    srb_mci_tx1_start_inline(idb, dgsize |
					     MCI_TX_ODDALIGN);
                    if (srb_debug) {
                        SrbDebug_VrnFastForward(idb, bn, rn);
                    }
		} else {
		    regptr->cmdreg = MCI_CMD_RX_FLUSH;
                    oidb->counters.output_total_drops++;
		}
	    } else if (is_tokenring(oidb)) { 
		/* Output is tring */
		dgsize -= FCS_SIZE;
		if (srb_mci_rx_move_select_inline(idb, oidb, 
						  CTR_RECEIVE_OFFSET,
						  dgsize)) {
		    regptr->write1short = AC_PRI4 << 8 | FC_TYPE_LLC;
                    srb_incr_vring_counters(idb, vre, dgsize);
		    srb_mci_tx1_start_inline(idb, dgsize);
                    if (srb_debug) {
		        SrbDebug_VrnFastForward(idb, bn, rn);
		    }
		}
	    } else if (is_cip_lan(oidb)) {
                channel_sb_t *chan_sb = channel_get_sb(oidb);
                if (!mac_read) {
		    srb_mci_read_mac_header_inline(idb, CTR_RECEIVE_OFFSET+1);
                }
                if (chan_sb) {
		  vc = chan_sb->get_vc_from_macaddr(oidb, &SRB_DA_BYTES, &SRB_SA_BYTES);
		} else {
		  vc = NO_CHANNEL_VC;
		}
                if (vc != NO_CHANNEL_VC) {
		    if (chan_sb->emulated_media == CIP_VLAN_TOKEN) {
		        dgsize -= (FCS_SIZE-CHANNEL_TOKEN_ENCAPBYTES);
                        if (srb_mci_rx_move_select_inline(idb, oidb, 
                                      CTR_RECEIVE_OFFSET-btow(CHANNEL_TOKEN_ENCAPBYTES),
                                                                    dgsize)) {
			    srb_mci_cip_token_encap_inline(oidb, vc);
                            srb_incr_vring_counters(idb, vre, dgsize);
		            srb_mci_tx1_start_inline(idb, dgsize);
                            if (srb_debug) {
                                SrbDebug_VrnFastForward(idb, bn, rn);
                            }
		    	}
		    } else if (chan_sb->emulated_media == CIP_VLAN_FDDI) {
		        dgsize -= (FCS_SIZE-CHANNEL_TOKEN_ENCAPBYTES);
                        if (srb_mci_rx_move_select_inline(idb, oidb, 
                                      CTR_RECEIVE_OFFSET-btow(CHANNEL_TOKEN_ENCAPBYTES),
                                                                    dgsize)) {
			    srb_mci_cip_fddi_mac_inline(oidb, vc);
                            srb_incr_vring_counters(idb, vre, dgsize);
		            srb_mci_tx1_start_inline(idb, dgsize);
                            if (srb_debug) {
                                SrbDebug_VrnFastForward(idb, bn, rn);
                            }
		    	}
		    } else {
		        regptr->cmdreg = MCI_CMD_RX_FLUSH;
                        srbV.srb_sb_incr_tx_drops(oidb->firstsw);
		    }
		} else {
		    regptr->cmdreg = MCI_CMD_RX_FLUSH;
                    srbV.srb_sb_incr_tx_drops(oidb->firstsw);
		}
	    } else {
		regptr->cmdreg = MCI_CMD_RX_FLUSH;
                srbV.srb_sb_incr_tx_drops(oidb->firstsw);
	    }
        } else {
	    srb_mci_rx_select_offset_inline(idb, CTR_RECEIVE_OFFSET);
	    if ((*oidb->if_srb_hes_fastout)(idb, oidb, dgstart, dgsize,
					NULL, 0, idb)) {
		srb_out_counters(oidb, dgsize);
		srb_incr_vring_counters(idb, vre, dgsize);
                if (srb_debug) {
                    SrbDebug_VrnFastForward(idb, bn, rn);
                }
            }
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;
	}
	return(TRUE);
    }

    if (vre->type == RSRB_ET_LOCAL_VIRTUAL) {
        oidb = vre->outhole.idb;

        if (oidb->span_bridge_process_force)
            return(FALSE);

        if (oidb->state != IDBS_UP) {
            regptr->cmdreg = MCI_CMD_RX_FLUSH;
            return(TRUE);
        }

        if (idb->srb_access_in) {
            regptr->argreg = CTR_RECEIVE_OFFSET;
            regptr->cmdreg = MCI_CMD_RX_SELECT;
            mci2buffer(dgstart, mac_acl, MAC_ACL_BYTES);
            if (!srb_in_check(mac_acl, dgsize, idb, riflen)) {
                regptr->cmdreg = MCI_CMD_RX_FLUSH;
                return(TRUE);
            }
        }

        pak = input_getbuffer(dgsize, idb);
        if(!pak) {
            regptr->cmdreg = MCI_CMD_RX_FLUSH;
            idb->counters.output_nobuffers++;
            return(TRUE);
        }

        pak->datagramstart -= (TRING_ENCAPBYTES + riflen);
        pak->datagramsize   = dgsize - TR_FCS_SIZE;
        pak->if_input       = idb->firstsw;
        pak->if_output      = oidb->firstsw;
        pak->linktype       = LINK_RSRB;
        pak->rxtype         = RXTYPE_UNKNOWN;
        pak->riflen         = riflen;

        regptr->argreg = CTR_RECEIVE_OFFSET;
        regptr->cmdreg = MCI_CMD_RX_SELECT;
        mci2buffer(&regptr->readlong, pak->datagramstart, dgsize);
        regptr->cmdreg = MCI_CMD_RX_FLUSH;

        if ((*oidb->vbridge_forward)(pak, oidb)) {
            srb_incr_vring_counters(idb, vre, dgsize);
        }

        return(TRUE);
    }

    if (vre->type & (RSRB_ET_REMOTE | RSRB_ET_REMSTATIC)) {
	vrp = vre->outhole.vp;
	/*
         * sanity check due to vr_purge_remote_rings queuing vre's to srb background
         * to be removed . This could result in a valid vre but an invalid vrp.
         */
        if(!vrp) {
            if(srberr_debug)
                buginf("\nRSRB%d:srb_ctr_to_vring: Unhooked vrp to vre ,  srn %d bn %d\n",
                       idb->unit, rn, bn);
        srb_in_backout_counters(idb, dgsize);
        return(FALSE);
	}

	if (idb->srb_access_in || vrp->rsrb_lsap_out || vrp->rsrb_netb_out) {
            regptr->argreg = CTR_RECEIVE_OFFSET;
            regptr->cmdreg = MCI_CMD_RX_SELECT;
	    mci2buffer(&regptr->readlong, mac_acl, MAC_ACL_BYTES);
	    if ((idb->srb_access_in && !srb_in_check(mac_acl, dgsize, idb,
						     riflen)) ||
		!(rsrb_out_check(mac_acl, dgsize, idb, riflen, vrp))) {
		regptr->cmdreg = MCI_CMD_RX_FLUSH;
		return(TRUE);
	    }
	}
	regptr->argreg = CTR_RECEIVE_OFFSET;
	regptr->cmdreg = MCI_CMD_RX_SELECT;
	if ((vrp->type == RSRB_PT_IF) || (vrp->type == RSRB_PT_LAN) ||
	    (vrp->type == RSRB_PT_FST) || (vrp->type == RSRB_PT_FR)) {
            if (vrp->vrp_hes_fast_lanout) {
                switch ((*vrp->vrp_hes_fast_lanout)(idb, vrp, vre, rn, dgstart,
					       dgsize)) {
                    case TRUE:
                      vrp->pkts_tx++;
                      srb_incr_vring_counters(idb, vre, dgsize);
                      if (srb_debug) {
                          RsrbDebug_ForwardToPeer(idb, vrp);
                      }
                      return(TRUE);
                    case RSRB_DROP:
                      return(TRUE);
                    case FALSE:
                    default:
                      srb_in_backout_counters(idb, dgsize);
                      return(FALSE);
                }
            }
	}
    }
    srb_in_backout_counters(idb, dgsize);
    return(FALSE);
}

/*
 *
 */
static boolean srb_fddi_to_vring (
    hwidbtype *idb,
    void *dummy2,
    hwidbtype *dummy)
{
    SRB_FAST_SETUP
    hwidbtype *oidb;
    ushort rc, dest;
    ulong temp;
    ushort bn;
    ushort rn = 0;
    paktype *pak;
    tr_ventrytype *vre;
    boolean mac_read;
    boolean write_addr;
    ushort vc;

    mac_read = write_addr = FALSE;
    srb_encap = (charlong *)(mac_acl + TR_ACFC_SIZE);
    
    dgsize &= ~(MCI_TX_ODDALIGN | MCI_RSP_ERR_UNK);
    srb_in_counters(idb, dgsize);

    /*
     * Gather all the information we will need to forward this frame.
     * Putting all the bail checks at the end means less if statements to
     * slow down the code.
     */
    srb_mci_rx_select_offset_inline(idb, FDDI_RECEIVE_OFFSET + 
				    (FDDI_ENCAPBYTES >> 1) + 1);
    rc = regptr->readshort;

    if (idb->rxsrbinfo & FCI_RXSRB_DIR) {
	srb_mci_rx_select_offset_inline(idb, FDDI_RECEIVE_OFFSET + 
					(FDDI_ENCAPBYTES >> 1) + 2);
	dest = regptr->readshort >> 4;
	srb_mci_rx_select_offset_inline(idb, FDDI_RECEIVE_OFFSET + 
					(FDDI_ENCAPBYTES >> 1) +
					((idb->rxsrbinfo &
					  FCI_RXSRB_RDOFFSET) >> 8));
	temp = regptr->readshort;
	rn = (temp >> 4) & 0x0FFF;
	bn = (temp >> 0) & 0x000F;
    } else {
	srb_mci_rx_select_offset_inline(idb, FDDI_RECEIVE_OFFSET + 
					(FDDI_ENCAPBYTES >> 1) +
					(idb->rxsrbinfo &
					 FCI_RXSRB_LENGTH));
	dest = regptr->readshort >> 4;
	srb_mci_rx_select_offset_inline(idb, FDDI_RECEIVE_OFFSET + 
					(FDDI_ENCAPBYTES >> 1) +
					((idb->rxsrbinfo &
					 FCI_RXSRB_RDOFFSET) >> 8) + 2);
	temp = regptr->readlong;
	bn = (temp >> 16) & 0x000F;
	rn = (temp >>  4) & 0x0FFF;
    }

    /*
     * Bail checks go here.  'Traced' frames must be handed by the system
     * so that the appropriate response may be generated.
     */
    if (rc & ((RC_BROADBIT | RC_TRACE_BOTH) << 8)) {
        srb_in_backout_counters(idb, dgsize);
	return(FALSE);
    }

    /*
     * ------------------------------------------------------------------
     * DLSw Fast Remote Peer Demux.
     * If rif terminates on the virtual ring, packet may be destined for
     * a DLSw peer (DLSw performs rif termination). Could also be for LNM
     * so fall thhrough if DLSw fast switch fails.
     *
     * ------------------------------------------------------------------
     */
    if (dest == srbV.srb_sb_get_targetring(idb->firstsw)) {
        if (reg_invoke_dlsw_srb_mci_input(idb, dgstart, dgsize, riflen)) {
            return(TRUE);
        } 
        srb_in_backout_counters(idb, dgsize);
        return(FALSE);
    }

    srb_mci_rx_select_offset_inline(idb, FDDI_RECEIVE_OFFSET);

    /*
     * Now find the table entry for the destination cisco on the other
     * side of the virtual ring.
     */
    vre = fast_srb_get_vre(idb->tr_vring_blk, rn, bn);

    /*
     * If there is not a forward target, return FALSE now, because this
     * frame needs to be handled by the processor to attempt to reopen
     * previous dead remote peers (proxy explorer forces this...)
     */
    if (!vre) {
	if (srberr_debug)
	    buginf("\nSRB%d: SRB/FDDI does not forward to remote peer",
		   idb->unit);
	regptr->cmdreg = MCI_CMD_RX_FLUSH;
        srbV.srb_sb_incr_rx_drops(idb->firstsw);
	return(TRUE);
    }

    if (vre->type == RSRB_ET_LOCAL) {
	oidb = vre->outhole.idb;
	if (!oidb->srb_routecache) {
            srb_in_backout_counters(idb, dgsize);
	    return(FALSE);
        }
	if (idb->srb_access_in || oidb->srb_access_out) {
	    mci2buffer(&regptr->readlong, mac_acl, MAC_ACL_BYTES);
	    if (!fs_srb_access_check(idb, oidb, mac_acl, dgsize, riflen)) {
		regptr->cmdreg = MCI_CMD_RX_FLUSH;
		return(TRUE);
	    }
	    mac_read = TRUE;
	}
	if (oidb->state != IDBS_UP) {
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;
            srbV.srb_sb_incr_tx_drops(oidb->firstsw);
	    return(TRUE);
	}

	if (regptr == oidb->mci_regptr) {
	    if (((idb->type == IDBTYPE_FDDIT) &&
		 (oidb->type != IDBTYPE_FDDIT)) ||
		((idb->type != IDBTYPE_FDDIT) &&
		 (oidb->type == IDBTYPE_FDDIT))) {
		if (!mac_read) {
		    srb_mci_read_mac_header_inline(idb,
						   FDDI_RECEIVE_OFFSET
						   + 1);
		    mac_read = TRUE;
                }
		srb_mci_swap_mac_address_inline();
		write_addr = TRUE;
	    }
	    if (is_fddi(oidb)) {
		dgsize -= FCS_SIZE;
                dgsize |= MCI_TX_ODDALIGN;
		regptr->argreg = oidb->mci_index;
		regptr->cmdreg = MCI_CMD_RX_MOVE;
		srb_out_counters(oidb, dgsize);
		if (regptr->cmdreg == MCI_RSP_OKAY) {
		    srb_mci_tx1_select_offset_inline(idb, FDDI_RECEIVE_OFFSET);
		    if (write_addr)
			srb_mci_fddi_mac_inline(idb);
		    else
			regptr->write1short = FDDI_LLC_FC_BYTE;
                    srb_incr_vring_counters(idb, vre, dgsize);
		    srb_mci_tx1_start_inline(idb, dgsize);
                    if (srb_debug) {
                        SrbDebug_VrnFastForward(idb, bn, rn);
                    }
		} else {
		    regptr->cmdreg = MCI_CMD_RX_FLUSH;
                    oidb->counters.output_total_drops++;
		}
	    } else if (is_tokenring(oidb)) { 
		dgsize -= (FCS_SIZE + FDDI_ENCAPBYTES - TRING_ENCAPBYTES);
		dgsize &= ~MCI_TX_ODDALIGN;
		regptr->argreg = oidb->mci_index;
		regptr->cmdreg = MCI_CMD_RX_MOVE;
		srb_out_counters(oidb, dgsize);
		if (regptr->cmdreg == MCI_RSP_OKAY) {
		    srb_mci_tx1_select_offset_inline(idb, FDDI_RECEIVE_OFFSET);
		    if (write_addr)
		        srb_mci_tring_mac_inline(idb);
		    else
		        regptr->write1short = AC_PRI4 << 8 | FC_TYPE_LLC;
                    srb_incr_vring_counters(idb, vre, dgsize);
		    srb_mci_tx1_start_inline(idb, dgsize);
                    if (srb_debug) {
		        SrbDebug_VrnFastForward(idb, bn, rn);
		    }
		} else {
		    regptr->cmdreg = MCI_CMD_RX_FLUSH;
                    oidb->counters.output_total_drops++;
		}
	    } else if (is_cip_lan(oidb)) {
                channel_sb_t *chan_sb = channel_get_sb(oidb);
                if (!mac_read) {
		    srb_mci_read_mac_header_inline(idb, FDDI_RECEIVE_OFFSET+1);
                }
                if (chan_sb) {
		    vc = chan_sb->get_vc_from_macaddr(oidb, &SRB_DA_BYTES, &SRB_SA_BYTES);
		} else {
		    vc = NO_CHANNEL_VC;
		}
                if (vc != NO_CHANNEL_VC) {
		    dgsize -= (FCS_SIZE + FDDI_ENCAPBYTES - TRING_ENCAPBYTES);
                    dgsize += CHANNEL_TOKEN_ENCAPBYTES;
		    dgsize &= ~MCI_TX_ODDALIGN;
		    if (chan_sb->emulated_media == CIP_VLAN_TOKEN) {
                        if (srb_mci_rx_move_select_inline(idb, oidb, 
                                      FDDI_RECEIVE_OFFSET-btow(CHANNEL_TOKEN_ENCAPBYTES),
                                                                    dgsize)) {
			    srb_mci_cip_token_mac_inline(oidb, vc);
                            srb_incr_vring_counters(idb, vre, dgsize);
		            srb_mci_tx1_start_inline(idb, dgsize);
                            if (srb_debug) {
                                SrbDebug_VrnFastForward( idb, bn, rn);
                            }
		    	}
		    } else if (chan_sb->emulated_media == CIP_VLAN_FDDI) {
		        dgsize -= (FCS_SIZE-CHANNEL_TOKEN_ENCAPBYTES);
                        if (srb_mci_rx_move_select_inline(idb, oidb, 
                                      FDDI_RECEIVE_OFFSET-btow(CHANNEL_TOKEN_ENCAPBYTES),
                                                                    dgsize)) {
			    srb_mci_cip_fddi_mac_inline(oidb, vc);
                            srb_incr_vring_counters(idb, vre, dgsize);
		            srb_mci_tx1_start_inline(idb, dgsize);
                            if (srb_debug) {
                                SrbDebug_VrnFastForward( idb, bn, rn);
                            }
		    	}
		    } else {
		        regptr->cmdreg = MCI_CMD_RX_FLUSH;
                        srbV.srb_sb_incr_tx_drops(oidb->firstsw);
		    }
		} else {
		    regptr->cmdreg = MCI_CMD_RX_FLUSH;
                    srbV.srb_sb_incr_tx_drops(oidb->firstsw);
		}
	    } else {
		regptr->cmdreg = MCI_CMD_RX_FLUSH;
                srbV.srb_sb_incr_tx_drops(oidb->firstsw);
	    }
	} else {
	    if (idb->type == IDBTYPE_FDDIT) {
		if (!mac_read) {
		    srb_mci_read_mac_header_inline(idb,
						   FDDI_RECEIVE_OFFSET
						   + 1);
		}
		srb_mci_swap_mac_address_inline();
	    }
	    srb_mci_rx_select_offset_inline(idb, FDDI_RECEIVE_OFFSET);
	    if ((*oidb->if_srb_hes_fastout)(idb, oidb, dgstart, dgsize, NULL, 0,
					idb)) {
		srb_out_counters(oidb, dgsize);
		srb_incr_vring_counters(idb, vre, dgsize);
                if (srb_debug) {
                    SrbDebug_VrnFastForward(idb, bn, rn);
                }
	    }
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;
	}
	return(TRUE);
    }

    if (vre->type == RSRB_ET_LOCAL_VIRTUAL) {
        oidb = vre->outhole.idb;

        if (oidb->span_bridge_process_force)
            return(FALSE);

        if (oidb->state != IDBS_UP) {
            regptr->cmdreg = MCI_CMD_RX_FLUSH;
            return(TRUE);
        }

	if (idb->srb_access_in) {
	    mci2buffer(dgstart, mac_acl, MAC_ACL_BYTES);
            if (!srb_in_check(mac_acl, dgsize, idb, riflen)) {
	        regptr->cmdreg = MCI_CMD_RX_FLUSH;
	        return(TRUE);
	    }
            srb_mci_rx_select_offset_inline(idb, FDDI_RECEIVE_OFFSET);
	}

        pak = input_getbuffer(dgsize, idb);
	if(!pak) {
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;
	    idb->counters.output_nobuffers++;
	    return(TRUE);
	}

        pak->datagramstart -= (TRING_ENCAPBYTES + riflen);
        pak->datagramsize   = dgsize - TR_FCS_SIZE - FDDI_ENCAPBYTES 
						   + TRING_ENCAPBYTES;
        pak->if_input       = idb->firstsw;
        pak->if_output      = oidb->firstsw;
        pak->linktype       = LINK_RSRB;
	pak->rxtype         = RXTYPE_UNKNOWN;
	pak->riflen         = riflen;

	mci2buffer(&regptr->readlong, pak->datagramstart, dgsize);
        regptr->cmdreg = MCI_CMD_RX_FLUSH;

        if ((*oidb->vbridge_forward)(pak, oidb)) {
	    srb_incr_vring_counters(idb, vre, dgsize);
        }

	return(TRUE);
    }

    if (vre->type & (RSRB_ET_REMOTE | RSRB_ET_REMSTATIC)) {
	if (srberr_debug)
	    buginf("\nSRB%d: SRB/FDDI does not forward to remote peer",
		   idb->unit);
	regptr->cmdreg = MCI_CMD_RX_FLUSH;
	return(TRUE);
    }
    srb_in_backout_counters(idb, dgsize);
    return(FALSE);
}

static boolean srb_cip_to_vring (
    hwidbtype *idb,
    void *dummy2,
    hwidbtype *dummy)
{
    SRB_FAST_SETUP
    hwidbtype *oidb;
    ushort dest;
    ulong temp;
    ushort bn;
    ushort rn = 0;
    paktype *pak;
    tr_vpeertype *vrp;
    tr_ventrytype *vre;
    boolean mac_read, write_addr;

    mac_read = write_addr = FALSE;
    srb_encap = (charlong *)(mac_acl + 2);
    dgsize -= FCS_SIZE;
    srb_in_counters(idb, dgsize);

    /*
     * Gather all the information we will need to forward this frame.
     * Putting all the bail checks at the end means less if statements to
     * slow down the code.
     */
    regptr->argreg = CHANNEL_TOKEN_RIF_OFFSET;
    regptr->cmdreg = MCI_CMD_RX_SELECT;
    if (idb->rxsrbinfo & FCI_RXSRB_DIR) {
	regptr->argreg = CHANNEL_TOKEN_RIF_OFFSET + 1; /* skip dir, length */
	regptr->cmdreg = MCI_CMD_RX_SELECT;
	dest = regptr->readshort >> 4;
	regptr->argreg = CHANNEL_TOKEN_RIF_OFFSET +
	    ((idb->rxsrbinfo & FCI_RXSRB_RDOFFSET) >> 8) - 1;
	regptr->cmdreg = MCI_CMD_RX_SELECT;
	temp = regptr->readshort;
	rn = (temp >> 4) & 0x0FFF;
	bn = (temp >> 0) & 0x000F;
    } else {
	regptr->argreg = CHANNEL_TOKEN_RIF_OFFSET +
	    (idb->rxsrbinfo & FCI_RXSRB_LENGTH) - 1;
	regptr->cmdreg = MCI_CMD_RX_SELECT;
	dest = regptr->readshort >> 4;
	regptr->argreg = CHANNEL_TOKEN_RIF_OFFSET + 
	    ((idb->rxsrbinfo & FCI_RXSRB_RDOFFSET) >> 8) + 1;
	regptr->cmdreg = MCI_CMD_RX_SELECT;
	temp = regptr->readlong;
	bn = (temp >> 16) & 0x000F;
	rn = (temp >>  4) & 0x0FFF;
    }

    /*
     * ------------------------------------------------------------------
     * DLSw Fast Remote Peer Demux.
     * If rif terminates on the virtual ring, packet may be destined for
     * a DLSw peer (DLSw performs rif termination). Could also be for LNM
     * so fall thhrough if DLSw fast switch fails.
     *
     * ------------------------------------------------------------------
     */
    if (dest == srbV.srb_sb_get_targetring(idb->firstsw)) {
        /* 
         * Reset dgsize before invoking DLSw+
         *
         *   Add FCS_SIZE back in (DLSw+ will decrement FCS_SIZE later)
         *     but don't include the CSNA header length
         *
         */
        dgsize += (FCS_SIZE - CHANNEL_TOKEN_ENCAPBYTES);
        if (reg_invoke_dlsw_srb_mci_input(idb, dgstart, dgsize, riflen)) {
            return(TRUE);
        } 
        srb_in_backout_counters(idb, dgsize);
        return(FALSE);
    }
    regptr->argreg = CHANNEL_TOKEN_ACFC_OFFSET;
    regptr->cmdreg = MCI_CMD_RX_SELECT;

    /*
     * Now find the table entry for the destination cisco on the other
     * side of the virtual ring.
     */
    vre = fast_srb_get_vre(idb->tr_vring_blk, rn, bn);

    /*
     * If there is not a forward target, return FALSE now, because this
     * frame needs to be handled by the processor to attempt to reopen
     * previous dead remote peers (proxy explorer forces this...)
     */
    if (!vre) {
        srb_in_backout_counters(idb, dgsize);
	return(FALSE);
    }
    if (vre->type == RSRB_ET_LOCAL) {
	oidb = vre->outhole.idb;
	if (!oidb->srb_routecache) {
            srb_in_backout_counters(idb, dgsize);
	    return(FALSE);
        }
	if (idb->srb_access_in || oidb->srb_access_out) {
	    mci2buffer(&regptr->readlong, mac_acl, MAC_ACL_BYTES);
	    if (!fs_srb_access_check(idb, oidb, mac_acl, dgsize, riflen)) {
		regptr->cmdreg = MCI_CMD_RX_FLUSH;
		return(TRUE);
	    }
	    mac_read = TRUE;
	}
	if (oidb->state != IDBS_UP) {
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;
            srbV.srb_sb_incr_tx_drops(oidb->firstsw);
	    return(TRUE);
	}
	if (regptr == oidb->mci_regptr) {
	    if (is_fddi(oidb)) {
		if (oidb->type == IDBTYPE_FDDIT) {
		    if (!mac_read) {
			srb_mci_read_mac_header_inline(idb,
						       CHANNEL_TOKEN_ACFC_OFFSET
						       + 1);
		    }
		    srb_mci_swap_mac_address_inline();
		    write_addr = TRUE;
		}
		dgsize -= ((TRING_ENCAPBYTES - FDDI_ENCAPBYTES) + 
                           CHANNEL_TOKEN_ENCAPBYTES);
                if (srb_mci_cmd_rx_move(oidb, dgsize)) {
                    srb_incr_vring_counters(idb, vre, dgsize);
		    srb_mci_tx1_select_offset_inline(idb, CHANNEL_TOKEN_ACFC_OFFSET);
		    if (write_addr)
			srb_mci_fddi_mac_inline(idb);
		    else
			regptr->write1short = FDDI_LLC_FC_BYTE;
		    srb_mci_tx1_start_inline(idb, dgsize | MCI_TX_ODDALIGN);
                    if (srb_debug) {
                        SrbDebug_VrnFastForward(idb, bn, rn);
                    }
		}
	    } else if (is_tokenring(oidb)) { 
		/* Output is tring */
		dgsize -= CHANNEL_TOKEN_ENCAPBYTES;
		if (srb_mci_rx_move_select_inline(idb, oidb, 
				   	          CHANNEL_TOKEN_ACFC_OFFSET,
						  dgsize)) {
		    regptr->write1short = AC_PRI4 << 8 | FC_TYPE_LLC;
                    srb_incr_vring_counters(idb, vre, dgsize);
		    srb_mci_tx1_start_inline(idb, dgsize);
                    if (srb_debug) {
                        SrbDebug_VrnFastForward(idb, bn, rn);
                    }
		}
	    } else if (is_cip_lan(oidb) && (oidb != idb)) {
                ushort vc = NO_CHANNEL_VC;
                channel_sb_t *chan_sb = channel_get_sb(oidb);
                if (!mac_read) {
                    srb_mci_read_mac_header_inline(idb, CHANNEL_TOKEN_ACFC_OFFSET+1);
                }
                if (chan_sb) {
                    vc = chan_sb->get_vc_from_macaddr(oidb, &SRB_DA_BYTES, &SRB_SA_BYTES);
                }
                if (vc != NO_CHANNEL_VC) {
                    if (chan_sb->emulated_media == CIP_VLAN_TOKEN) {
                        if (srb_mci_cmd_rx_move(oidb, dgsize)) {
                            srb_incr_vring_counters(idb, vre, dgsize);
		            srb_mci_tx1_select_offset_inline(oidb, 
                                                             CHANNEL_RECEIVE_OFFSET);
                            srb_mci_cip_token_encap_inline(oidb, vc);
                            srb_mci_tx1_start_inline(idb, dgsize);
                            if (srb_debug) {
		                SrbDebug_VrnFastForward(idb, bn, rn);
		            }
			}
                    } else if (chan_sb->emulated_media == CIP_VLAN_FDDI) {
                        if (srb_mci_cmd_rx_move(oidb, dgsize)) {
                            srb_incr_vring_counters(idb, vre, dgsize);
		            srb_mci_tx1_select_offset_inline(oidb,
                                                             CHANNEL_RECEIVE_OFFSET);
		            srb_mci_cip_fddi_mac_inline(oidb, vc);
                            srb_mci_tx1_start_inline(idb, dgsize);
                            if (srb_debug) {
		                SrbDebug_VrnFastForward(idb, bn, rn);
		            }
			}
		    } else {
	                regptr->cmdreg = MCI_CMD_RX_FLUSH;
                        srbV.srb_sb_incr_tx_drops(oidb->firstsw);
                    }
	        } else {
                    /* 
                     * Dest MACADDR not configured on CIP
                     * or Cbus failure
                     */
	            regptr->cmdreg = MCI_CMD_RX_FLUSH;
                    srbV.srb_sb_incr_tx_drops(oidb->firstsw);
                }
	    } else {
	        oidb->counters.output_total_drops++;
	        regptr->cmdreg = MCI_CMD_RX_FLUSH;
            }
	} else {
	    buginf("\nsrb_cip_to_vring - impossible case, regptr != oidb->mci_regptr!");
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;
            srbV.srb_sb_incr_tx_drops(oidb->firstsw);
	}
	return(TRUE);	
    }

    if (vre->type == RSRB_ET_LOCAL_VIRTUAL) {
        oidb = vre->outhole.idb;

        if (oidb->span_bridge_process_force)
            return(FALSE);

        if (oidb->state != IDBS_UP) {
            regptr->cmdreg = MCI_CMD_RX_FLUSH;
            return(TRUE);
        }

        if (idb->srb_access_in) {
            regptr->argreg = CHANNEL_TOKEN_ACFC_OFFSET;
            regptr->cmdreg = MCI_CMD_RX_SELECT;
            mci2buffer(dgstart, mac_acl, MAC_ACL_BYTES);
            if (!srb_in_check(mac_acl, dgsize, idb, riflen)) {
                regptr->cmdreg = MCI_CMD_RX_FLUSH;
                return(TRUE);
            }
        }

        pak = input_getbuffer(dgsize, idb);
        if(!pak) {
            regptr->cmdreg = MCI_CMD_RX_FLUSH;
            idb->counters.output_nobuffers++;
            return(TRUE);
        }

        pak->datagramstart -= (TRING_ENCAPBYTES + riflen);
        pak->datagramsize   = dgsize - CHANNEL_TOKEN_ENCAPBYTES;
        pak->if_input       = idb->firstsw;
        pak->if_output      = oidb->firstsw;
        pak->linktype       = LINK_RSRB;
        pak->rxtype         = RXTYPE_UNKNOWN;
        pak->riflen         = riflen;

        regptr->argreg = CHANNEL_RECEIVE_OFFSET;
        regptr->cmdreg = MCI_CMD_RX_SELECT;
        mci2buffer(&regptr->readlong, pak->datagramstart
                                    - CHANNEL_TOKEN_ENCAPBYTES, dgsize);
        regptr->cmdreg = MCI_CMD_RX_FLUSH;

        if ((*oidb->vbridge_forward)(pak, oidb)) {
            srb_incr_vring_counters(idb, vre, pak->datagramsize);
        }

        return(TRUE);
    }

    if (vre->type & (RSRB_ET_REMOTE | RSRB_ET_REMSTATIC)) {
	vrp = vre->outhole.vp;

	/*
         * sanity check due to vr_purge_remote_rings queuing vre's to srb background
         * to be removed . This could result in a valid vre but an invalid vrp.
         */
        if(!vrp) {
            if(srberr_debug)
                buginf("\nRSRB%d:srb_cip_to_vring: Unhooked vrp to vre ,  srn %d bn %d\n",
                       idb->unit, rn, bn);
        srb_in_backout_counters(idb, dgsize);
        return(FALSE);
        }

	if (idb->srb_access_in || vrp->rsrb_lsap_out || vrp->rsrb_netb_out) {
	    mci2buffer(&regptr->readlong, mac_acl, MAC_ACL_BYTES);
	    if ((idb->srb_access_in && !srb_in_check(mac_acl, dgsize, idb,
						     riflen)) ||
		!(rsrb_out_check(mac_acl, dgsize, idb, riflen, vrp))) {
		regptr->cmdreg = MCI_CMD_RX_FLUSH;
		return(TRUE);
	    }
	}
	regptr->argreg = CHANNEL_TOKEN_ACFC_OFFSET;
	regptr->cmdreg = MCI_CMD_RX_SELECT;
	if ((vrp->type == RSRB_PT_IF) || (vrp->type == RSRB_PT_LAN) ||
	    (vrp->type == RSRB_PT_FST) || (vrp->type == RSRB_PT_FR)) {
            if (vrp->vrp_hes_fast_lanout) {
                dgsize -= (CHANNEL_TOKEN_ENCAPBYTES - FCS_SIZE);
                switch ((*vrp->vrp_hes_fast_lanout)(idb, vrp, vre, rn, dgstart,
					       dgsize)) {
                    case TRUE:
                      vrp->pkts_tx++;
                      srb_incr_vring_counters(idb, vre, dgsize);
                      if (srb_debug) {
                          RsrbDebug_ForwardToPeer(idb, vrp);
                      }
                      return(TRUE);
                    case RSRB_DROP:
                      return(TRUE);
                    case FALSE:
                    default:
                      srb_in_backout_counters(idb, dgsize);
                      return(FALSE);
                }
            }
	}
    }
    srb_in_backout_counters(idb, dgsize);
    return(FALSE);
}

boolean srb_vbridge_to_vring (
    hwidbtype *vidb,
    void *pakptr,
    hwidbtype *dummy)
{
    paktype *pak     = (paktype *)pakptr;
    uchar *dgstart   = pak->datagramstart;
    ushort dgsize    = pak->datagramsize + TR_FCS_SIZE;
    uchar *rif_start = pak->rif_start;
    ushort riflen    = pak->riflen;
    uchar *rif_match = pak->rif_match;
    tr_ventrytype *vre;
    tr_vpeertype *vrp;
    hwidbtype *oidb = NULL;
    srbrd_t *rd;
    ushort rc, rn, bn;

    /* Parse the ring and bridge numbers for the next hop. */

    rc = *(ushort *)rif_start;
    if (rc & RC_D) {
        rd = (srbrd_t *)(rif_start + 2);
        rn = ((ushort *) rif_match)[-1] >> 4;
        bn = ((ushort *) rif_match)[-1] & 0xf;
    } else {
        rd = (srbrd_t *)(rif_start + riflen - 2);
        bn = ((ushort *) rif_match)[1] & 0xf;
        rn = ((ushort *) rif_match)[2] >> 4;
    }

    /* Bail checks go here.  'Traced' frames must be handed by the system
     * so that the appropriate response may be generated.  Also reject
     * frames that end on the virtual ring because of all the various
     * places in the system code where they could be processed. */

    if ((rc & ((RC_BROADBIT | RC_TRACE_BOTH) << 8))
    || rd->ring_num == srbV.srb_sb_get_targetring(firstsw_or_null(vidb)))
       return(FALSE);

    /* Find the destination virtual ring entry.  */

    vre = fast_srb_get_vre(vidb->tr_vring_blk, rn, bn);

    if (!vre)
	return(FALSE);

    if (vre->type == RSRB_ET_LOCAL) {
	oidb = vre->outhole.idb;

	if (!oidb->srb_routecache)
	    return(FALSE);

	if (oidb->state != IDBS_UP) {
	    datagram_done(pak);
	    return(TRUE);
	}

	if (oidb->srb_access_out 
	&& !srb_out_check(dgstart, dgsize, oidb, pak->riflen)) {
	    datagram_done(pak);
	    return(TRUE);
	}

	if ((*oidb->if_srb_hes_fastout)(vidb,oidb,dgstart,dgsize,NULL,0,vidb)) {
	    srb_out_counters(oidb, dgsize);
 	    srb_incr_vring_counters (vidb, vre, dgsize);
	}

	datagram_done(pak);
	return(TRUE);
    }

    if (vre->type & (RSRB_ET_REMOTE | RSRB_ET_REMSTATIC)) {
	vrp = vre->outhole.vp;
	/*
         * sanity check due to vr_purge_remote_rings queuing vre's to srb background
         * to be removed . This could result in a valid vre but an invalid vrp.
         */
        if(!vrp) {
            if(srberr_debug)
                buginf("\nRSRB%d:srb_vbridge_to_vring: Unhooked vrp to vre ,  srn %d bn %d\n",
                       oidb->unit, rn, bn);
	    datagram_done(pak);        
	    return(TRUE);
        }

	if (vrp->rsrb_lsap_out || vrp->rsrb_netb_out)
	    if (!rsrb_out_check(dgstart, dgsize, vidb, riflen, vrp)) {
		datagram_done(pak);
		return(TRUE);
	    }

	if((vrp->type == RSRB_PT_IF   || vrp->type == RSRB_PT_LAN
	||  vrp->type == RSRB_PT_FST  || vrp->type == RSRB_PT_FR)
	&&  vrp->vrp_hes_fast_lanout) {
            switch ((*vrp->vrp_hes_fast_lanout)(vidb, vrp, vre, rn, dgstart, dgsize)) {
                case TRUE:
                  vrp->pkts_tx++;
                  srb_incr_vring_counters(vidb, vre, dgsize);
                  if (srb_debug) {
                      RsrbDebug_ForwardToPeer(vidb, vrp);
                  }
		  datagram_done(pak);
                  return(TRUE);

                case RSRB_DROP:
		  datagram_done(pak);
                  return(TRUE);

                case FALSE:
                default:
                  break;
            }
	}
    }
    return(FALSE);
}


/***********************************************************************
 *
 *
 *
 **********************************************************************/

/*
 * fs_to_mci_fst_vring
 * Fast switch SRB packet to virtual
 * ring using IP encapsulation.
 */

static int fs_to_mci_fst_vring (
    tr_vpeertype *vrp,
    tr_ventrytype *vre,
    ushort rn,
    uchar *dgstart,
    int dgsize,
    ipcache_rntype *cptr,
    hwidbtype *input)
{
    hwidbtype *output;
    mciregtype *dstreg;
    ulong output_status;
    int totalsize;
    riftype *rifptr;
    iphdrtype *iphead;

    output = cptr->idb->hwptr;
    output_status = output->status;
    totalsize = dgsize + FST_IP_HDR_SIZE +(sizeof(tr_vhdrtype) - 6);
    if (totalsize >= output->maxmtu)
	return(FALSE);
    if ((output_status & IDB_SERIAL) && (output->enctype != ET_HDLC))
        return(FALSE);
    dstreg = output->mci_regptr;
    iphead = vrp->ip_header;
    if (output->priority_list && output->output_qcount) {
	/* traffic already in hold queue, divert to hold queue */
	return FALSE;			/* let slow path handle congestion */
    }
    if (dstreg == input->mci_regptr) {
	/*
	 * The output interface is on the same cBus complex as the intput
	 * interface.  Don't do any data moves at all, only rewrite the
	 * headers.
	 */
	dstreg->argreg = output->mci_index;
	dstreg->cmdreg = MCI_CMD_RX_MOVE;

	/*
	 * RX_MOVE is slow.  Assume it will be sucessful and count totals
	 * while waiting for it to complete.
	 */
	output->counters.outputs_fast[ACCT_PROTO_SRB]++;
	output->counters.tx_cumbytes_fast[ACCT_PROTO_SRB] += dgsize;
	if (dstreg->cmdreg == MCI_RSP_OKAY) {
	    /* 
	     * Now do necessary modifications to
	     * ip header.
	     */

	    RSRB_FST_INCREMENT_SEND(vrp, TRUE, iphead->id);
	    iphead->tl = dgsize + FST_IP_HDR_SIZE + (sizeof(tr_vhdrtype) - 6);
	    iphead->checksum = 0;			
	    iphead->checksum = ipcrc((ushort *) iphead, ltob(iphead->ihl));
    
	    /*
	     * Lay in the MAC/IP/VHD header.
	     */
	    if (output_status & IDB_SERIAL) {
		dstreg->argreg = input->rxoffset - btow(output->encsize) -
		    (sizeof(tr_vhdrtype) - 6)/2 - FST_IP_HDR_SIZE/2 ;
		dstreg->cmdreg = MCI_CMD_TX1_SELECT;
		totalsize = dgsize + output->encsize + (sizeof(tr_vhdrtype) -6)
		    + FST_IP_HDR_SIZE;
		fst_write_serial_header(dstreg, cptr, (ulong *)iphead);
	    } else if (output_status & IDB_ETHER) {
		dstreg->argreg = input->rxoffset - ETHER_ARPA_ENCAPBYTES/2 -
		    (sizeof(tr_vhdrtype) - 6)/2 - FST_IP_HDR_SIZE/2;
		dstreg->cmdreg = MCI_CMD_TX1_SELECT;
		totalsize = dgsize + ETHER_ARPA_ENCAPBYTES +
		    (sizeof(tr_vhdrtype) -6)
		    + FST_IP_HDR_SIZE;
		fst_write_ether_header(dstreg, cptr, (ulong *)iphead);
	    } else if (output_status & IDB_FDDI) {
		rifptr = get_rif_inline(output, cptr->m.macstring);
		if (rifptr) {
		    dstreg->argreg = input->rxoffset - FDDI_SNAP_ENCAPWORDS
			- (sizeof(tr_vhdrtype) - 6)/2 - rifptr->length/2 
			    - FST_IP_HDR_SIZE/2;
		    totalsize = dgsize + FDDI_SNAP_ENCAPBYTES 
			+ (sizeof(tr_vhdrtype) - 6) + FST_IP_HDR_SIZE +
			    rifptr->length;
		} else {
		    dstreg->argreg = input->rxoffset - FDDI_SNAP_ENCAPWORDS
			- (sizeof(tr_vhdrtype) - 6)/2 - FST_IP_HDR_SIZE/2;
		    totalsize = dgsize + FDDI_SNAP_ENCAPBYTES 
			+ (sizeof(tr_vhdrtype) - 6) + FST_IP_HDR_SIZE;
		}
		dstreg->cmdreg = MCI_CMD_TX1_SELECT;
		totalsize |= MCI_TX_ODDALIGN;
		fst_write_fddi_header(dstreg, cptr, rifptr, (ulong *)iphead);
	    } else if (output_status & IDB_TR) {
		rifptr = get_rif_inline(output, cptr->m.macstring);
		if (rifptr) {
		    dstreg->argreg = input->rxoffset - TRING_SNAP_ENCAPBYTES/2
			- (sizeof(tr_vhdrtype) - 6)/2 - rifptr->length/2 
			    - FST_IP_HDR_SIZE/2;
		    totalsize = dgsize + TRING_SNAP_ENCAPBYTES 
			+ (sizeof(tr_vhdrtype) - 6) + FST_IP_HDR_SIZE +
			    rifptr->length;
		} else {
		    dstreg->argreg = input->rxoffset - TRING_SNAP_ENCAPBYTES/2
			- (sizeof(tr_vhdrtype) - 6)/2 - FST_IP_HDR_SIZE/2;
		    totalsize = dgsize + TRING_SNAP_ENCAPBYTES 
			+ (sizeof(tr_vhdrtype) - 6) + FST_IP_HDR_SIZE;
		}
		dstreg->cmdreg = MCI_CMD_TX1_SELECT;
		fst_write_token_header(dstreg, cptr, rifptr, (ulong *)iphead);
 	    } else if (output_status & IDB_ATM) {
		ATM_VCOUTFAST(output, cptr->m.macshort[0]);
		ATM_OUTFAST(output);
		dstreg->argreg = input->rxoffset - btow(cptr->length)
		    - (sizeof(tr_vhdrtype) - 6)/2 - FST_IP_HDR_SIZE/2;
		totalsize = dgsize + TRING_SNAP_ENCAPBYTES 
		    + (sizeof(tr_vhdrtype) - 6) + FST_IP_HDR_SIZE;
		dstreg->cmdreg = MCI_CMD_TX1_SELECT;
		fst_write_atm_header(dstreg, cptr, (ulong *)iphead);
	    } else {
		if (srb_debug || srberr_debug)
		    buginf("\nRSRB: FST fast-switching not supported on interface type 0x%x", output_status);
		return(FALSE);
	    }

	    dstreg->write1short = vrp->vrp_version;		/* version */
	    dstreg->write1short = RSRB_OP_FORWARDFST;	/* op      */
	    dstreg->write1short = rn;			/* trn     */
	    dstreg->write1short = vrp->vrg->vrn;	/* vrn     */
	    dstreg->write1short = 0;			/* offset  */
	    
	    /*
	     * Send it.
	     */
	    dstreg->argreg = totalsize;
	    dstreg->cmdreg = MCI_CMD_TX1_START;
	    return(TRUE);
	} else {

	    /*
	     * un-increment the statistics
	     */
	    output->counters.outputs_fast[ACCT_PROTO_SRB]--;
	    output->counters.tx_cumbytes_fast[ACCT_PROTO_SRB] -= dgsize;

	    /*
	     * RX_MOVE Failed.
	     */
	    if (output->priority_list)
		return FALSE;		/* let slow path handle congestion */
	    /*
	     * Drop the frame.
	     */
	    dstreg->cmdreg = MCI_CMD_RX_FLUSH;
	    output->counters.output_total_drops++;
	    return(RSRB_DROP);
	}
    }

    /*
     * Reserve a buffer in window 1 of the destination MCI
     */
    dstreg->argreg = output->mci_index;
    dstreg->cmdreg = MCI_CMD_SELECT;
    
    /*
     * The MCI doesn't care what the argreg value is -- the FCI requires that
     * it be the buffer pool number i.e. which pool to get buffer from.
     */
    dstreg->argreg = output->buffer_pool;
    dstreg->cmdreg = MCI_CMD_TX1_RESERVE;
    if (dstreg->cmdreg != MCI_RSP_OKAY) {
	/*
	 * TX_RESERVE Failed.
	 */
	if (output->priority_list)
	    return FALSE;		/* let slow path handle congestion */
        /*
         * drop the frame since we don't want this packet to get
         * switched via process level and then getting packets
         * out of order
         */
	input->mci_regptr->cmdreg = MCI_CMD_RX_FLUSH;
        output->counters.output_total_drops++;
        return(RSRB_DROP);
    }

    /* 
     * Now do necessary modifications to
     * ip header.
     */
    RSRB_FST_INCREMENT_SEND(vrp, TRUE, iphead->id)

    iphead->tl = dgsize + FST_IP_HDR_SIZE + (sizeof(tr_vhdrtype) - 6);
    iphead->checksum = 0;			
    iphead->checksum = ipcrc((ushort *) iphead, ltob(iphead->ihl));
    
    /*
     * Set up write pointer to base of destination transmit window.
     * Do some input accounting.  Figure out how to write new MAC header
     * and write new mac header and UDP/IP header and copy the received
     * datagram after that.
     */
    output->counters.outputs_fast[ACCT_PROTO_SRB]++;
    output->counters.tx_cumbytes_fast[ACCT_PROTO_SRB] += dgsize;
    dstreg->argreg = 0;
    dstreg->cmdreg = MCI_CMD_TX1_SELECT;
    if (output_status & IDB_ETHER) {
	totalsize = dgsize + ETHER_ARPA_ENCAPBYTES + FST_IP_HDR_SIZE +
	    (sizeof(tr_vhdrtype) - 6);
	fst_write_ether_header(dstreg, cptr, (ulong *)iphead);
    } else if (output_status & IDB_SERIAL) {
	totalsize = dgsize + output->encsize + FST_IP_HDR_SIZE +
	    (sizeof(tr_vhdrtype) - 6);
	fst_write_serial_header(dstreg, cptr, (ulong *)iphead);
    } else if (output_status & IDB_FDDI) {
	rifptr = get_rif_inline(output, cptr->m.macstring);
	if (rifptr) {
	    totalsize = dgsize + FDDI_SNAP_ENCAPBYTES 
		+ (sizeof(tr_vhdrtype) - 6) + FST_IP_HDR_SIZE +
		    rifptr->length;
	} else {
	    totalsize = dgsize + FDDI_SNAP_ENCAPBYTES 
		+ (sizeof(tr_vhdrtype) - 6) + FST_IP_HDR_SIZE;
	}
	totalsize |= MCI_TX_ODDALIGN;
	fst_write_fddi_header(dstreg, cptr, rifptr, (ulong *)iphead);
    } else {
	rifptr = get_rif_inline(output, cptr->m.macstring);
	if (rifptr) {
	    totalsize = dgsize + TRING_SNAP_ENCAPBYTES 
		+ (sizeof(tr_vhdrtype) - 6) + FST_IP_HDR_SIZE +
		    rifptr->length;
	} else {
	    totalsize = dgsize + TRING_SNAP_ENCAPBYTES 
		+ (sizeof(tr_vhdrtype) - 6) + FST_IP_HDR_SIZE;
	}
	fst_write_token_header(dstreg, cptr, rifptr, (ulong *)iphead);
    }

    /*
     * Write the RSRB header.
     */
    dstreg->write1short = vrp->vrp_version;		/* version */
    dstreg->write1short = RSRB_OP_FORWARDFST;	/* op      */
    dstreg->write1short = rn;			/* trn     */
    dstreg->write1short = vrp->vrg->vrn;	/* vrn     */
    dstreg->write1short = 0;			/* offset  */


    /*
     * copy the data from CTR card (or buffer) to MCI buffer
     *
     *   NOTE: The only types of interface cards in the 7000
     *           are MCI or CBUS so the use of buffer2mci is 
     *           is required only when the interface is virtual
     *             (e.g. SR/TLB use a vidb)
     *
     */
    if (input->status & (IDB_MCI | IDB_CBUS)) {
        mci2mci(dgstart, &dstreg->write1long, dgsize);    /* includes FCS */
        input->mci_regptr->cmdreg = MCI_CMD_RX_FLUSH;
    } else {
        buffer2mci(dgstart, &dstreg->write1long, dgsize); /* includes FCS */
    }

    /*
     * send the packet out
     */
    dstreg->argreg = totalsize;
    dstreg->cmdreg = MCI_CMD_TX1_START;

    /*
     * Keep statistics.
     */
    output->counters.tx_cumbytes_fast[ACCT_PROTO_SRB] += dgsize;
    output->counters.outputs_fast[ACCT_PROTO_SRB]++;
    GET_TIMESTAMP(output->lastoutput);
    return(TRUE);

} /* end of fs_to_mci_udp_vring */

/*
 * fs_to_fst_vring
 * Fast switch SRB packet to virtual
 * ring using UDP/IP encapsulation.
 */

static int fs_to_fst_vring (
    hwidbtype *input,
    tr_vpeertype *vrp,
    tr_ventrytype *vre,
    ushort rn,
    uchar *dgstart,
    int dgsize)
{
    ipcache_rntype *cptr;
    ulong address;

    if (vrp->ip_header == NULL)
	return(FALSE);
    /*
     * Set up some common variables
     */
    address = vrp->ipaddr;
    
    /*
     * lookup ip cache to see if we have valid
     * entry.
     */
    cptr = ipcache_lookup_fast(address);
    if (cptr) {
	if (cptr->idb->hwptr->status & (IDB_MCI|IDB_CBUS)) {
	    return(fs_to_mci_fst_vring(vrp, vre, rn, dgstart, dgsize,
				       cptr, input));
	}
    }
    input->counters.cache_miss[ACCT_PROTO_IP]++;
    return(FALSE);
} /* end of fs_to_fst_vring */


/***********************************************************************
 *
 *
 *
 **********************************************************************/

/*
 * mci_vring_output
 *
 * Fast switch an RSRB encapsulated packet out onto an MCI style
 * interface.  This routine is called via a vector from one of the
 * fs_xxx_to_vring() routines.
 *
 * If the intermediate media is a token ring, look to see if any of the
 * multiring bits was on.  This is a good indication of whether or not
 * there are source route bridges in the ring, and that the router should
 * be using RIFs on these frames.  Since these frames only ever go to
 * other cisco routers, this should not cause any problems.
 */

static int mci_vring_output (
    hwidbtype *input,
    tr_vpeertype *vrp,
    tr_ventrytype *vre,
    ushort rn,
    uchar *dgstart,
    int dgsize)
{
    hwidbtype *output;
    mciregtype *dstreg;
    ulong output_interface;
    riftype *rifptr;
    int i, totalsize;
    boolean canonical;
    uchar swapspace[IEEEBYTES];

    /*
     * Set up some common variables
     */
    output = vrp->outidb;
    output_interface = output->status;

    if (output->priority_list && output->output_qcount) {
	/* traffic already in hold queue, divert to hold queue */
	return FALSE;			/* let slow path handle congestion */
    }

    dstreg = output->mci_regptr;

    if (dstreg == input->mci_regptr) {
	/*
	 * The output interface is on the same cBus complex as the intput
	 * interface.  Don't do any data moves at all, only rewrite the
	 * headers.
	 */
	dstreg->argreg = output->mci_index;
	dstreg->cmdreg = MCI_CMD_RX_MOVE;

	/*
	 * RX_MOVE is slow.  Assume it will be sucessful and count totals
	 * while waiting for it to complete.
	 */
	srb_out_counters(output, dgsize);
        if (srb_debug) {
            SrbDebug_FastForward(input);
        }
	if (dstreg->cmdreg == MCI_RSP_OKAY) {
	    /*
	     * Lay in the RSRB header.
	     */
	    if (output->status & IDB_SERIAL) {
		dstreg->argreg = input->rxoffset - btow(output->encsize) -
		    sizeof(tr_vhdrtype)/2;
		dstreg->cmdreg = MCI_CMD_TX1_SELECT;
		totalsize = dgsize + output->encsize + sizeof(tr_vhdrtype);
		srb_write_serial_header(dstreg,output,vrp);
	    } else if (output->status & IDB_ETHER) {
		dstreg->argreg = input->rxoffset - ETHER_SNAP_ENCAPBYTES/2 -
		    sizeof(tr_vhdrtype)/2;
		dstreg->cmdreg = MCI_CMD_TX1_SELECT;
		totalsize = dgsize + ETHER_SNAP_ENCAPBYTES +
		    sizeof(tr_vhdrtype);
		srb_write_ether_header(dstreg, vrp->macaddr, output->hardware,
				       totalsize);
	    } else if (output->status & IDB_FDDI) {
		rifptr = get_rif_inline(output, vrp->macaddr);
		if (rifptr) {
		    dstreg->argreg = input->rxoffset -
			FDDI_SNAP_ENCAPWORDS - sizeof(tr_vhdrtype)/2 -
			    rifptr->length/2; 
		    totalsize = dgsize + FDDI_SNAP_ENCAPBYTES +
			sizeof(tr_vhdrtype) + rifptr->length;
		} else {
		    dstreg->argreg = input->rxoffset -
			FDDI_SNAP_ENCAPWORDS - sizeof(tr_vhdrtype)/2;
		    totalsize = dgsize + FDDI_SNAP_ENCAPBYTES +
			sizeof(tr_vhdrtype);
		}
		dstreg->cmdreg = MCI_CMD_TX1_SELECT;
		canonical = (output->type == IDBTYPE_FDDIT);
		totalsize = dgsize + FDDI_SNAP_ENCAPBYTES +
		    sizeof(tr_vhdrtype);
		totalsize |= MCI_TX_ODDALIGN;
		if (canonical) {
		    srb_write_fddi_header(dstreg, vrp->macaddr,
					  output->hardware);
		} else {
		    for (i = 0; i < IEEEBYTES; i++)
			swapspace[i] = BITSWAP(vrp->macaddr[i]);
		    srb_write_fddi_header(dstreg, swapspace,
					  output->bit_swapped_hardware);
		}
	    } else if (output->status & IDB_TR) {
		rifptr = get_rif_inline(output, vrp->macaddr);
		if (rifptr) {
		    dstreg->argreg = input->rxoffset -
			TRING_SNAP_ENCAPBYTES/2 - sizeof(tr_vhdrtype)/2 -
			    rifptr->length/2; 
		    totalsize = dgsize + TRING_SNAP_ENCAPBYTES +
			sizeof(tr_vhdrtype) + rifptr->length;
		} else {
		    dstreg->argreg = input->rxoffset -
			TRING_SNAP_ENCAPBYTES/2 - sizeof(tr_vhdrtype)/2;
		    totalsize = dgsize + TRING_SNAP_ENCAPBYTES +
			sizeof(tr_vhdrtype);
		}

		dstreg->cmdreg = MCI_CMD_TX1_SELECT;
		srb_write_token_header(dstreg, vrp->macaddr, output->hardware,
				       rifptr);
 	    } else {
		/* channel emulating token or fddi - not useful */
		if (srb_debug)
		    buginf("\nmci_vring_output called for outbound CIP!");
		dstreg->cmdreg = MCI_CMD_RX_FLUSH;
		output->counters.output_total_drops++;
		return(RSRB_DROP);
	    }

	    dstreg->write1short = vrp->vrp_version;		/* version */
	    dstreg->write1short = RSRB_OP_FORWARD;	/* op      */
	    dstreg->write1short = rn;			/* trn     */
	    dstreg->write1short = vrp->vrg->vrn;	/* vrn     */
	    dstreg->write1short = 0;			/* offset  */
	    dstreg->write1short = dgsize; 		/* len     */
	    dstreg->write1long  = 0;			/* param   */
	    
	    /*
	     * Send it.
	     */
	    dstreg->argreg = totalsize;
	    dstreg->cmdreg = MCI_CMD_TX1_START;
	    return(TRUE);
	} else {
	    /*
	     * RX_MOVE Failed. If fancy queuing active, do that.
	     */
	    if (output->priority_list) {
		output->counters.outputs_fast[ACCT_PROTO_SRB]--;
		output->counters.tx_cumbytes_fast[ACCT_PROTO_SRB] -= dgsize;
		return FALSE;		/* let slow path handle congestion */
	    }

	    /*
	     * Drop the frame.
	     */
	    dstreg->cmdreg = MCI_CMD_RX_FLUSH;
	    output->counters.output_total_drops++;
	    return(RSRB_DROP);
	}
    }

    /*
     * Reserve a buffer in window 1 of the destination MCI
     */
    dstreg->argreg = output->mci_index;
    dstreg->cmdreg = MCI_CMD_SELECT;
    
    /*
     * The MCI doesn't care what the argreg value is -- the FCI requires that
     * it be the buffer pool number i.e. which pool to get buffer from.
     */
    dstreg->argreg = output->buffer_pool;
    dstreg->cmdreg = MCI_CMD_TX1_RESERVE;
    if (dstreg->cmdreg != MCI_RSP_OKAY) {
	/*
	 * TX_RESERVE Failed
	 */
	if (output->priority_list)
	    return FALSE;		/* let slow path handle congestion */ 

	/*
	 * actually, drop the packet so we don't slow switch
	 * this packet, and fast switch following packets
	 * causing out of order packets.
	 */
	input->mci_regptr->cmdreg = MCI_CMD_RX_FLUSH;
        output->counters.output_total_drops++;
        return(RSRB_DROP);
    }
    
    /*
     * Set up write pointer to base of destination transmit window.
     * Do some input accounting.  Figure out how to write new MAC header.
     * Then adjust the read pointer of the receiver window to point after
     * old MAC header.  Compute number of bytes to be copied (in temp) and
     * to be sent (in bytes).
     */
    dstreg->argreg = 0;
    dstreg->cmdreg = MCI_CMD_TX1_SELECT;
    if (output_interface & IDB_ETHER) {
	totalsize = dgsize + ETHER_SNAP_ENCAPBYTES + sizeof(tr_vhdrtype);
	srb_write_ether_header(dstreg, vrp->macaddr, output->hardware,
			       totalsize);
    } else if (output_interface & IDB_SERIAL) {
	totalsize = dgsize + output->encsize + sizeof(tr_vhdrtype);
	srb_write_serial_header(dstreg,output,vrp);
    } else if (output_interface & IDB_FDDI) {
	rifptr = get_rif_inline(output, vrp->macaddr);
	canonical = (output->type == IDBTYPE_FDDIT);
	totalsize = dgsize + FDDI_SNAP_ENCAPBYTES + sizeof(tr_vhdrtype);
	totalsize |= MCI_TX_ODDALIGN;
	if (canonical) {
	    srb_write_fddi_header(dstreg, vrp->macaddr, output->hardware);
	} else {
	    for (i = 0; i < IEEEBYTES; i++)
		swapspace[i] = BITSWAP(vrp->macaddr[i]);
	    srb_write_fddi_header(dstreg, swapspace,
				  output->bit_swapped_hardware);
	}
    } else {
	rifptr = get_rif_inline(output, vrp->macaddr);
	totalsize = dgsize + TRING_SNAP_ENCAPBYTES + sizeof(tr_vhdrtype);
	if (rifptr)
	    totalsize += rifptr->length;
	srb_write_token_header(dstreg, vrp->macaddr, output->hardware, rifptr);
    } 
    /* no cip case needed here */

    /*
     * Write the RSRB header.
     */
    dstreg->write1short = vrp->vrp_version;		/* version */
    dstreg->write1short = RSRB_OP_FORWARD;	/* op      */
    dstreg->write1short = rn;			/* trn     */
    dstreg->write1short = vrp->vrg->vrn;	/* vrn     */
    dstreg->write1short = 0;			/* offset  */
    dstreg->write1short = dgsize;		/* len     */
    dstreg->write1long  = 0;			/* param   */

    if (input->status & (IDB_MCI | IDB_CBUS)) {
	mci2mci(dgstart, &dstreg->write1long, dgsize);	  /* includes FCS */
	input->mci_regptr->cmdreg = MCI_CMD_RX_FLUSH;
    } else {
	buffer2mci(dgstart, &dstreg->write1long, dgsize); /* includes FCS */
    }

    dstreg->argreg = totalsize;
    dstreg->cmdreg = MCI_CMD_TX1_START;

    /*
     * Keep statistics.
     */
    srb_out_counters(output, dgsize);
    if (srb_debug) {
        SrbDebug_FastForward(input);
    }
    return(TRUE);

}

/**********************************************************************
 * 
 *		       FAST DIRECT RSRB INPUT
 *
 * These routines accept a direct RSRB packet from an interface, and
 * send it out onto a locally attached token ring.
 * 
 **********************************************************************/

/*
 * mci_vring_input
 *
 * Receive an RSRB packet from an MCI style interface, and forward the
 * extracted frame out onto a token ring.  The RSRB frame must be a
 * 'FORWARD' to be handled by this routine.  Anything not understood is
 * thrown up to the system level code.  The 'fastout' will point to a
 * routine that will check access lists (where configured), output the
 * frame, and keep statistics.  Interfaces with Netbios 'Byte' access
 * lists cannot be handled fast.
 */

static boolean mci_vring_input (hwidbtype* idb)
{
    tr_vgrouptype *vrg;
    tr_ventrytype *vre;
    mciregtype *regptr;
    charlong data;
    ushort vrn, trn, length, offset;
    int riflen;
    hwidbtype *outidb;
    llc1_hdr_t *llc1;
    tr_vpeertype *vrp;
    paktype *pak;
    trrif_t *trh;

    regptr = idb->mci_regptr;
    offset = idb->rsrb_vringoffset;
    if (idb->type == IDBTYPE_CTR)
	offset += idb->rxrifwordlen;

    /*
     * Is this frame traced? 
     */
    regptr->argreg = offset + (TRING_ENCAPBYTES >> 1) +
	(sizeof(tr_vhdrtype)/2);
    regptr->cmdreg = MCI_CMD_RX_SELECT;
    data.d.sword[0] = regptr->readshort;
    if (data.d.sword[0] & ((RC_BROADBIT | RC_TRACE_BOTH) << 8))
	return(FALSE);

    /*
     * Ignore the version number.  This must be a FORWARD command.  This
     * is the same for all versions
     */
    regptr->argreg = offset;
    regptr->cmdreg = MCI_CMD_RX_SELECT;
    data.d.lword = regptr->readlong;
    if (data.d.sword[1] != RSRB_OP_FORWARD)
	return(FALSE);

    /*
     * If the target ring is zero, or the target ring is the same as the
     * virtual ring, then the packet is for this router.  Otherwise, find
     * the ring group.
     */
    data.d.lword = regptr->readlong;
    trn = data.d.sword[0];
    vrn = data.d.sword[1];
    if (!trn || (trn == vrn))
	return(FALSE);

    FAST_SRB_GET_VRG(vrn)

    if (!vrg) {
	idb->counters.input_drops++;
	regptr->cmdreg = MCI_CMD_RX_FLUSH;
	return(TRUE);
    }
 
    FAST_SRB_GET_VRE_LOCAL(&vrg->rings.qhead, trn)
    vrp = idb->tr_ifpeer;
    if ((!vre) || (!vrp)) {
	regptr->cmdreg = MCI_CMD_RX_FLUSH;
	return(TRUE);
    }

    outidb = vre->outhole.idb;
    
    /*
     * Don't fastswitch the frame if the user doesn't want it done.
     */
    if (outidb->state != IDBS_UP)
        return(FALSE);
    if (!outidb->srb_routecache) {
	return(FALSE);
    }

    /*
     * Read length and param entries.  This should leave the MCI pointing
     * at the first byte of the frame data.
     */
    data.d.lword = regptr->readlong;
    length = data.d.sword[1];
    data.d.lword = regptr->readlong;

    if (vre->type == RSRB_ET_LOCAL_VIRTUAL) {

        if (outidb->span_bridge_process_force)
            return(FALSE);

	mci2buffer(&regptr->readlong, mac_acl, MAC_ACL_BYTES);
	trh = (trrif_t *)mac_acl;
	riflen = trh->rc_blth & RC_LTHMASK;

	if (idb->srb_access_in
        && !srb_in_check(mac_acl, length, idb, riflen)) {
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;
	    return(TRUE);
	}

        pak = input_getbuffer(length, idb);
	if(!pak) {
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;
	    idb->counters.output_nobuffers++;
	    return(TRUE);
	}

        pak->datagramstart -= (TRING_ENCAPBYTES + riflen);
        pak->datagramsize   = length - TR_FCS_SIZE;
        pak->if_input       = idb->firstsw;
        pak->if_output      = outidb->firstsw;
        pak->linktype       = LINK_RSRB;
	pak->rxtype         = RXTYPE_UNKNOWN;
	pak->riflen         = riflen;

	regptr->argreg = offset + (sizeof(tr_vhdrtype) >> 1);
	regptr->cmdreg = MCI_CMD_RX_SELECT;
	mci2buffer(&regptr->readlong, pak->datagramstart, length);
        regptr->cmdreg = MCI_CMD_RX_FLUSH;

        if ((*outidb->vbridge_forward)(pak, outidb)) {
            vrp->pkts_rx++;
	    vre->forwards++;
	}

        srb_in_counters(idb, length);
	return(TRUE);
    }

    if (vre->type != RSRB_ET_LOCAL)
	return(FALSE);

    /*
     * Time for access lists.  What fun.  Don't forget to go back to the
     * start of the embedded frame once the access list garbage is done.
     */
    if (outidb->srb_access_out || global_proxy_explorer_enable ||
	global_netbios_name_cache_enable) {
	trrif_t *trh;

	if (fs_netbios_output_acl_check(outidb)) {
	    return(FALSE);
        }
	mci2buffer(&regptr->readlong, mac_acl, MAC_ACL_BYTES);
	trh = (trrif_t *)mac_acl;

	/*
	 * Do proxy explorers, if needed
	 */
	if (global_proxy_explorer_enable) {
	    riflen = trh->rc_blth & RC_LTHMASK;
	    llc1 = (llc1_hdr_t *)((uchar *)&trh->rc_blth + riflen);
	    if (((llc1->control & ~LLC1_P) == LLC1_XID) ||
		((llc1->control & ~LLC1_P) == LLC1_TEST)) {
		rif_update(NULL, vrg->vrn, trh->saddr, NULL,
			   (srbroute_t *) &trh->rc_blth, RIF_AGED_REM);
	    }
	}

	/*
	 * Do NetBIOS name caching, if needed
	 */
	if (global_netbios_name_cache_enable) {
	    netbios_name_cache_update_pak((uchar *) trh, NETBIOS_NAME_AGED_REM,
					  NULL, vrg->vrn);
	}

	if (outidb->srb_access_out &&
	    (!srb_out_check(mac_acl, length, outidb,
			   trh->rc_blth & RC_LTHMASK))) {
	    regptr->cmdreg = MCI_CMD_RX_FLUSH;
	    return(TRUE);
	}
	regptr->argreg = offset + (sizeof(tr_vhdrtype) >> 1);
	regptr->cmdreg = MCI_CMD_RX_SELECT;
    }
    srb_in_counters(idb, length);
    vrp->pkts_rx++;
    if (regptr == outidb->mci_regptr) {

        if (is_tokenring(outidb)) {	/* Output interface is tring */

	    regptr->argreg = outidb->mci_index;
	    regptr->cmdreg = MCI_CMD_RX_MOVE;
	    /*
	     * The cBus rx_move is extremely slow -- assume that it is
	     * going to be successfull, count some totals while waiting.
	     */
	    srb_out_counters(outidb, length);
            if (srb_debug) {
                SrbDebug_FastForward(idb);
            }
	    if (regptr->cmdreg != MCI_RSP_OKAY) {
	        /*
	         * RX_MOVE failed -- drop the datagram.
	         */
	        regptr->cmdreg = MCI_CMD_RX_FLUSH;
	        outidb->counters.output_total_drops++;
	        return(TRUE);
	    }
	    regptr->argreg = offset + sizeof(tr_vhdrtype)/2;
	    regptr->cmdreg = MCI_CMD_TX1_SELECT;
	    regptr->write1short = (AC_PRI4 << 8) | FC_TYPE_LLC;
	    regptr->argreg = length - TR_FCS_SIZE;	/* Drop the FCS */
	    regptr->cmdreg = MCI_CMD_TX1_START;
	    return(TRUE);
 
        } else if (is_cip_lan(outidb)) {    /* Output interface is CIP LAN */
            ushort vc = NO_CHANNEL_VC;
            channel_sb_t *chan_sb = channel_get_sb(outidb);
            ushort mci_token_acfc_offset = offset+(sizeof(tr_vhdrtype)/2);
            srb_mci_read_mac_header_inline(idb, mci_token_acfc_offset+1);
            if (chan_sb) {
                vc = chan_sb->get_vc_from_macaddr(outidb, &SRB_DA_BYTES, &SRB_SA_BYTES);
            }
            if (vc != NO_CHANNEL_VC) {
                length -= (FCS_SIZE - CHANNEL_TOKEN_ENCAPBYTES);
                if (chan_sb->emulated_media == CIP_VLAN_TOKEN) {
                    if (srb_mci_cmd_rx_move( outidb, length)) {
                        srb_mci_tx1_select_offset_inline(idb, 
                              (mci_token_acfc_offset -
			       btow(CHANNEL_TOKEN_ENCAPBYTES)));
                        srb_mci_cip_token_mac_inline(outidb, vc);
                        srb_mci_tx1_start_inline(idb, length);
	                if (srb_debug) {
	                    buginf("\nSRB%d: CIP fast forward (vrn %d trn %d)",
		                   outidb->unit, vrn, trn);
                        }
		    }
                } else if (chan_sb->emulated_media == CIP_VLAN_FDDI) {
                    if (srb_mci_cmd_rx_move( outidb, length)) {
                        srb_mci_tx1_select_offset_inline(idb, 
                              (mci_token_acfc_offset -
			       btow(CHANNEL_TOKEN_ENCAPBYTES)));
                        srb_mci_cip_fddi_mac_inline(outidb, vc);
                        srb_mci_tx1_start_inline(idb, length);
	                if (srb_debug) {
	                    buginf("\nSRB%d: CIP fast forward (vrn %d trn %d)",
		                   outidb->unit, vrn, trn);
                        }
                    }
                } else {
	            regptr->cmdreg = MCI_CMD_RX_FLUSH;
                    srbV.srb_sb_incr_tx_drops(outidb->firstsw);
                }
            } else {
                /* 
                 * Dest macaddr is not configured on CIP internal LAN
                 */
	        regptr->cmdreg = MCI_CMD_RX_FLUSH;
                srbV.srb_sb_incr_tx_drops(outidb->firstsw);
            }
            return(TRUE);
        } else {
            /* 
             * Output interface not supported for fastswitching
             *   boot packet up to process level
             */
            srb_in_backout_counters(idb, length);
            return(FALSE);
        }
    }
    
    if ((*vre->vre_hes_fastout)(vre->virtual_idb, outidb, (uchar *)
			    &regptr->readlong, length, NULL, 0, idb)) {  
	vre->forwards++;
	srb_out_counters(outidb, regptr->rxlength);
	if (srb_debug) {
	    buginf("\nRSRB%d: fast forward (vrn %d trn %d)",
		   outidb->unit, vrn, trn);
        }
    }
    regptr->cmdreg = MCI_CMD_RX_FLUSH;
    return(TRUE);
}

static boolean fs_srb_ctr_output (
    hwidbtype *idb,
    hwidbtype *oidb,
    uchar *dgstart,  
    int dgsize, 
    ulong *lpdu,
    int lpdu_offset,
    hwidbtype *realidb)
{
    mciregtype *dstreg = oidb->mci_regptr;
    mciregtype *srcreg;
    ushort throwaway;

    /*
     * Reserve a buffer in window 1 of the destination MCI
     */
    dstreg->argreg = oidb->mci_index;
    dstreg->cmdreg = MCI_CMD_SELECT;

    /*
     * The MCI doesn't care what the argreg value is -- the FCI requires that
     * it be the buffer pool number i.e. which pool to get buffer from.
     */
    dstreg->argreg = oidb->buffer_pool;
    dstreg->cmdreg = MCI_CMD_TX1_RESERVE;
    if (dstreg->cmdreg != MCI_RSP_OKAY)
        return(FALSE);

    dstreg->argreg = 0;
    dstreg->cmdreg = MCI_CMD_TX1_SELECT;

    /* Drop the FCS. */
    dgsize -= TR_FCS_SIZE;

    if (realidb->status & (IDB_MCI | IDB_CBUS)) {
	srcreg =  realidb->mci_regptr;
	throwaway = srcreg->readshort;
	dstreg->write1short = (AC_PRI4 << 8) | FC_TYPE_LLC;
	mci2mci(dgstart, &dstreg->write1long, dgsize - 2);
    } else {
	dstreg->write1short = (AC_PRI4 << 8) | FC_TYPE_LLC;
	buffer2mci(dgstart + 2, &dstreg->write1long, dgsize - 2);
    }

    if (lpdu) {
	/* Rewrite LPDU. */
	dstreg->argreg = lpdu_offset;
	dstreg->cmdreg = MCI_CMD_TX1_SELECT;
	dstreg->write1long = *lpdu;
	/* Reset the transmit start pointer (sigh). */
	dstreg->argreg = 0;
	dstreg->cmdreg = MCI_CMD_TX1_SELECT;
    }

    dstreg->argreg = dgsize;
    dstreg->cmdreg = MCI_CMD_TX1_START;

    return(TRUE);
}

static boolean fs_srb_fddi_output (
    hwidbtype *idb,
    hwidbtype *oidb,
    uchar *dgstart,
    int dgsize,
    ulong *lpdu,
    int lpdu_offset,
    hwidbtype *realidb)
{
    mciregtype *dstreg = oidb->mci_regptr;

    srb_encap = (charlong *)(dgstart + 2);
    /*
     * Reserve a buffer in window 1 of the destination MCI
     */
    dstreg->argreg = oidb->mci_index;
    dstreg->cmdreg = MCI_CMD_SELECT;

    /*
     * The MCI doesn't care what the argreg value is -- the FCI requires that
     * it be the buffer pool number i.e. which pool to get buffer from.
     */
    dstreg->argreg = oidb->buffer_pool;
    dstreg->cmdreg = MCI_CMD_TX1_RESERVE;
    if (dstreg->cmdreg != MCI_RSP_OKAY)
        return(FALSE);

    dstreg->argreg = 0;
    dstreg->cmdreg = MCI_CMD_TX1_SELECT;

    /* Drop the FCS. */
    dgsize -= (FCS_SIZE + TRING_ENCAPBYTES - FDDI_ENCAPBYTES);

    if (oidb->type == IDBTYPE_FDDIT) {
	/* need to swap addresses 
	 * assume packet came in as token ring packets 
	 */
	srb_mci_swap_mac_address_inline();
	srb_mci_fddi_mac_inline(oidb);
	buffer2mci(dgstart+TRING_ENCAPBYTES, &dstreg->write1long, 
		   dgsize-TRING_ENCAPBYTES);
    } else {
	dstreg->write1short = FDDI_LLC_FC_BYTE;
	buffer2mci(dgstart+TR_ACFC_SIZE, &dstreg->write1long, 
		   dgsize-TR_ACFC_SIZE);
    }

    dstreg->argreg = dgsize | MCI_TX_ODDALIGN;
    dstreg->cmdreg = MCI_CMD_TX1_START;

    return(TRUE);
}


/*
 * Write CIP header into CBUS buffer then copy packet from system buffer.
 * 
 * Does not handle MCI to MCI or ON_COMPLEX CBUS transfers.
 */
static inline boolean fs_srb_cip_output (
    hwidbtype *idb,
    hwidbtype *oidb,
    uchar *dgstart,  
    int dgsize, 
    ulong *lpdu,
    int lpdu_offset,
    hwidbtype *realidb)
{
    mciregtype *dstreg = oidb->mci_regptr;
    ushort vc;
    channel_sb_t *chan_sb;

    if (realidb->status & (IDB_MCI | IDB_CBUS)) {
	if(srberr_debug) {
	    buginf("\n%%%SRB%d - CIP invalid hardware config", idb->unit);
	    bugtrace();
	}
	return(FALSE);
    }

    /* Reserve a buffer in window 1 of the destination MCI */

    if (!fci_select_if(oidb, dstreg)) {
	oidb->counters.output_total_drops++;
        return FALSE;
    }

    if (!srb_mci_cmd_tx1_reserve(oidb)) {
	oidb->counters.output_total_drops++;
        return FALSE;
    }

    chan_sb = channel_get_sb(oidb);
    if (!chan_sb) {
        srbV.srb_sb_incr_tx_drops(oidb->firstsw);
	return(FALSE);
    }
  
    vc = chan_sb->get_vc_from_macaddr(oidb, dgstart+TR_DA_OFFSET, dgstart+TR_SA_OFFSET);
    if (vc == NO_CHANNEL_VC) {
        srbV.srb_sb_incr_tx_drops(oidb->firstsw);
	return(FALSE);
    }

    /* Select the MEMD buffer and write the CIP header. */

    dstreg->argreg = 0;
    dstreg->cmdreg = MCI_CMD_TX1_SELECT;

    dstreg->write1short = vc;
    dstreg->write1short = 0x2001;

    switch(chan_sb->emulated_media) {
	case CIP_VLAN_TOKEN:
	    dstreg->write1short = 0x0001;
	    dstreg->write1short = (AC_PRI4<<8) | FC_TYPE_LLC;
	    break;

	case CIP_VLAN_FDDI:
	    dstreg->write1short = 0x0002;
	    dstreg->write1short = FDDI_LLC_FC_BYTE; /* add lc byte */
	    dgsize &= ~(MCI_TX_ODDALIGN | MCI_RSP_ERR_UNK);
	    break;

	default:
	    if(srberr_debug)
	        buginf("\n%%%SRB%d - CIP SRB encap media type invalid(%d)",
		   oidb->unit, chan_sb->emulated_media);
            return(FALSE);
    }

    /* Adjust the packet size and write the packet behind the CIP header. */

    dgsize -= TR_FCS_SIZE;

    buffer2mci(dgstart+TR_ACFC_SIZE, &dstreg->write1long, dgsize-TR_ACFC_SIZE);

    /* Rewrite LPDU? */

    if (lpdu) {
	dstreg->argreg = lpdu_offset + CHANNEL_TOKEN_ENCAPBYTES;
	dstreg->cmdreg = MCI_CMD_TX1_SELECT;
	dstreg->write1long = *lpdu;
	dstreg->argreg = 0;                 /* Reset TX buf pointer. */
	dstreg->cmdreg = MCI_CMD_TX1_SELECT;
    }

    dstreg->argreg = dgsize + CHANNEL_TOKEN_ENCAPBYTES;
    dstreg->cmdreg = MCI_CMD_TX1_START;

    return(TRUE);
}

/**********************************************************************
 * 
 * 
 * 
 **********************************************************************/

/*
 *
 */
void fs_srb_setup (void)
{
    hwidbtype *idb, *oidb;
    tr_ventrytype *vring;
    tr_vgrouptype *vrg;

    FOR_ALL_HWIDBS(idb) {
	if (!(is_srb_hw(idb)))
	    continue;

	idb->tr_srb_fastswitch = (tr_srb_fastswitch_type)return_false;
	idb->if_srb_hes_fastout = (if_srb_hes_fastout_type)return_false;
	/* Set the other two variants for sanity sake */
	idb->if_srb_les_fastout = (if_srb_les_fastout_type)return_false;
	idb->if_srb_fastout = (if_srb_fastout_type)return_false;

	/*
	 * Set up the output side if_srb_hes_fastout.
	 */
	if (idb->srb_routecache && !fs_netbios_output_acl_check(idb)) {
	    if (idb->type == IDBTYPE_CTR) {
		idb->if_srb_hes_fastout = fs_srb_ctr_output;
	    } else if (idb->status & IDB_FDDI) {
		idb->if_srb_hes_fastout = fs_srb_fddi_output; 
	    } else if (idb->type == IDBTYPE_CIP_LAN_ADAPTER) {
		idb->if_srb_hes_fastout = fs_srb_cip_output; 
	    }
        }


	/*
	 * Don't touch cards with input side netbios offset lists or
	 * acess expressions.  This is because they need to look at
	 * random offsets into the frame and the frame is not local to
	 * this processor.
	 */
	if (idb->srb_access_in && fs_netbios_input_acl_check(idb)) {
	    continue;
        }

	/*
	 * Target is a virtual ring.
	 */
	vring = idb->tr_vring_blk;
	if (vring) {
	    if (!idb->srb_routecache)
		continue;

	    if (idb->type == IDBTYPE_CTR) {
		idb->tr_srb_fastswitch = 
                    (tr_srb_fastswitch_type)srb_ctr_to_vring;
	    } else if (idb->status & IDB_FDDI) {
		idb->tr_srb_fastswitch = 
                    (tr_srb_fastswitch_type)srb_fddi_to_vring;
	    } else if (idb->type == IDBTYPE_CIP_LAN_ADAPTER) {
		idb->tr_srb_fastswitch =
		    (tr_srb_fastswitch_type)srb_cip_to_vring;
	    }
	    continue;	
	}
	
	/*
	 * Target must be a real ring.  Check for fast switching enabled
	 * on the output interface.  Cannot function if netbios offset
	 * lists or acess expressions are enabled on the output
	 * interface, so check for those as well.
	 */
	oidb = idb->tr_bridge_idb;
	if (oidb) {
	    if ((!oidb->srb_routecache) ||
		(oidb->srb_access_out && fs_netbios_output_acl_check(oidb))) {
	            continue;
            }
	    if (idb->type == IDBTYPE_CTR) {
		idb->tr_srb_fastswitch =
		    (tr_srb_fastswitch_type)srb_tr_fastswitch;
	    } else if (idb->status & IDB_FDDI) {
		idb->tr_srb_fastswitch =
		    (tr_srb_fastswitch_type)srb_fddi_fastswitch;
	    } else if (idb->type == IDBTYPE_CIP_LAN_ADAPTER) {
		idb->tr_srb_fastswitch =
		    (tr_srb_fastswitch_type)srb_cip_fastswitch;
	    }
	}
    }

    /* Initialize virtual bridge fast switch vectors. */

    for (vrg = (tr_vgrouptype *)vringQ.qhead; vrg; vrg = vrg->next) {
	if (vrg->virtual_bridge == NULL)
	    continue;

	if (vrg->virtual_bridge->tbridge_media == TBR_VBRIDGE) {
	    vrg->virtual_bridge->hwptr->tr_srb_fastswitch = 
			(tr_srb_fastswitch_type)srb_vbridge_to_vring;
	    continue;
	}
    }
}

void set_vring_fast_entry (
    hwidbtype *idb)
{
    tr_ventrytype *vre;

    vre = idb->tr_vring_blk;
    for (; vre; vre = vre->next) {
	if (((vre->type == RSRB_ET_LOCAL) ||
	     (vre->type == RSRB_ET_LOCAL_VIRTUAL)) &&
	    (vre->outhole.idb == idb)) {
	    break;
	}
    }

    if (!vre)
        return;
    /* For sanity sake, set the other variant. */
    vre->vre_les_fastout = (vre_les_fastout_type)vre_fastout_err;

    if (idb->type == IDBTYPE_CTR) {
	vre->vre_hes_fastout = fs_srb_ctr_output;
	return;
    }

    if (idb->status & IDB_FDDI) {
	vre->vre_hes_fastout = fs_srb_fddi_output;
	return;
    }

    if (idb->type == IDBTYPE_CIP_LAN_ADAPTER) {
	vre->vre_hes_fastout = fs_srb_cip_output;
	return;
    }
    vre->vre_hes_fastout = (vre_hes_fastout_type)vre_fastout_err;
}

void set_vring_fast_peer (
    tr_vpeertype *vrp,
    hwidbtype *idb)
{
    switch (vrp->type) {
    case RSRB_PT_IF:
    case RSRB_PT_FR:
	vrp->vrp_hes_fast_lanout = mci_vring_output;
	break;

    case RSRB_PT_LAN:
	vrp->vrp_hes_fast_lanout = mci_vring_output;
	break;

    case RSRB_PT_FST:
	vrp->vrp_hes_fast_lanout = fs_to_fst_vring;
	FOR_ALL_HWIDBS(idb) {
	    idb->rsrb_fst_fastout = (rsrb_fst_fastout_type)fs_to_mci_fst_vring;
	}
	break;

    default:
	vrp->vrp_hes_fast_lanout = NULL;
	break;
    }

    /* Clear the other two variants for sanity sake. */
    vrp->vrp_fast_lanout = NULL;
    vrp->vrp_les_fast_lanout = NULL;
}


/*
 * mci_rsrb_fst_fast_forward
 * This routine is the combined parts of vrfst_input()
 * 
 * It is used to forward FST datagrams on to the local token Ring.
 * We will bump up to process level if:
 *  - mci/cbus hasn't checked the packet.
 */

static boolean mci_rsrb_fst_fast_forward (
     hwidbtype *input)
{
    mciregtype *inreg;
    hwidbtype *output;
    tr_vgrouptype *vrg;
    tr_ventrytype *vre;
    tr_vpeertype *vrp;
    paktype *pak;
    ushort seq_num;
    ushort data_offset;
    ushort vrn, trn, length, offset;
    ipaddrtype srcadr, dstadr;
    boolean fst_result;
    int riflen;
    llc1_hdr_t *llc1;
    charlong data;

    /*
     * Assume the following for the packet:
     * 1) IP Checksum is correct.
     * 2) This packet is not an IP fragment.
     * 3) There are no IP options.
     */
    inreg = input->mci_regptr;
    data_offset = input->rsrb_vringoffset;
    /*
     * The SRB and FST data headers are different from each other on Ethernet
     * only. Note the difference here...
     */
    if (input->status & IDB_ETHER) {
	data_offset -= (ETHER_SNAP_ENCAPBYTES - ETHER_ARPA_ENCAPBYTES)/2;
    } else if (is_srb_hw(input))
	data_offset += input->rxrifwordlen;
    data_offset += (FST_IP_HDR_SIZE + (sizeof(tr_vhdrtype) - 6))/2;

    /*
     * Note: this code relies on the rx pointer to be sitting at the IP
     * checksum.
     *
     * Short read is ip checksum.  Long reads are SA, DA.
     */
    offset = inreg->readshort;
    srcadr = inreg->readlong;
    dstadr = inreg->readlong;
    /*
     * If it isn't for us, then forward the thing on...
     */
    if (dstadr != rsrb_fst_name)
	return(FALSE);
    
    /*
     * If the mci passed the RX_CHECK then the header will always be
     * MINIPHEADERBYTES long.
     *
     *
     * RSRB VHD
     */

    /*
     * If the input was on an FDDI or CTR style interface, then
     * SNIFFBASE+3 has our ID. If not, then get it from SNIFFBASE+1.
     * Do this before overwriting SNIFFBASE+3.
     */
    if ((input->status & IDB_CBUS) &&
	((input->status & IDB_TR) || (input->status & IDB_FDDI)
	 || (input->status & IDB_ATM))) {
	length = input->sniff[SNIFFBASE+3].d.sword[0];
	seq_num = input->sniff[SNIFFBASE+3].d.sword[1];
    } else {
	length = input->sniff[SNIFFBASE+1].d.sword[0];
	seq_num = input->sniff[SNIFFBASE+1].d.sword[1];
    }

    /*
     * Modify the total length to remove the IP header and vhdr.
     */
    length -= (FST_IP_HDR_SIZE + (sizeof(tr_vhdrtype) - 6));

    input->sniff[SNIFFBASE+3].d.lword = inreg->readlong;
    input->sniff[SNIFFBASE+4].d.lword = inreg->readlong;
    
    if (input->sniff[SNIFFBASE+3].d.sword[1] != RSRB_OP_FORWARDFST)
        return(FALSE);
    
    trn = input->sniff[SNIFFBASE+4].d.sword[0];
    vrn = input->sniff[SNIFFBASE+4].d.sword[1];
    
    if (!trn || (trn == vrn))
	return(FALSE);

    FAST_SRB_GET_VRG(vrn)

    if (!vrg) {
	input->counters.input_drops++;
	inreg->cmdreg = MCI_CMD_RX_FLUSH;
	return(TRUE);
    }

    FAST_SRB_GET_VRP_FST(srcadr)

    FAST_SRB_GET_VRE_LOCAL(&vrg->rings.qhead, trn)

    if (!vre || !vrp) {
	input->counters.input_drops++;
	inreg->cmdreg = MCI_CMD_RX_FLUSH;
	return(TRUE);
    }

    if (vre->type != RSRB_ET_LOCAL || vre->type != RSRB_ET_LOCAL_VIRTUAL)
	return(FALSE);

    output = vre->outhole.idb;

    /*
     * Don't fastswitch the frame if the user doesn't want it done.
     */
    if (output->state != IDBS_UP)
	return(FALSE);
    if (!output->srb_routecache) {
	return(FALSE);
    }

    /*
     * Is this frame traced? 
     */
    inreg->argreg = data_offset + (TRING_ENCAPBYTES >> 1);
    inreg->cmdreg = MCI_CMD_RX_SELECT;
    data.d.sword[0] = inreg->readshort;
    if (data.d.sword[0] & ((RC_BROADBIT | RC_TRACE_BOTH) << 8))
	return(FALSE);

    inreg->argreg = data_offset;
    inreg->cmdreg = MCI_CMD_RX_SELECT;
    
    if (vre->type == RSRB_ET_LOCAL_VIRTUAL) {
	trrif_t *trh;

        if (output->span_bridge_process_force)
            return(FALSE);

	mci2buffer(&inreg->readlong, mac_acl, MAC_ACL_BYTES);
	trh    = (trrif_t *)mac_acl;
	riflen = trh->rc_blth & RC_LTHMASK;

	if (input->srb_access_in)
            if (!srb_in_check(mac_acl, length, input, riflen)) {
	        inreg->cmdreg = MCI_CMD_RX_FLUSH;
	        return(TRUE);
	    }

        /* Check sequencing. FUNCTION CANNOT RETURN FALSE FROM HERE ON!! */

        RSRB_FST_ACCEPTOR(vrp, seq_num, fst_result)
        if (!fst_result) {
	    input->counters.input_drops++;
	    vrp->drops++;
	    if (srbevent_debug)
	        buginf("\nRSRB: FST acceptor failed");
	    inreg->cmdreg = MCI_CMD_RX_FLUSH;
	    return(TRUE);
        }

        pak = input_getbuffer(length, input);
	if(!pak) {
	    inreg->cmdreg = MCI_CMD_RX_FLUSH;
	    input->counters.output_nobuffers++;
	    return(TRUE);
	}

        pak->datagramstart -= (TRING_ENCAPBYTES + riflen);
        pak->datagramsize   = length - TR_FCS_SIZE;
        pak->if_input       = input->firstsw;
        pak->if_output      = output->firstsw;
        pak->linktype       = LINK_RSRB;
	pak->rxtype         = RXTYPE_UNKNOWN;
	pak->riflen         = riflen;

        inreg->argreg = data_offset;
        inreg->cmdreg = MCI_CMD_RX_SELECT;
	mci2buffer(&inreg->readlong, pak->datagramstart, length);
        inreg->cmdreg = MCI_CMD_RX_FLUSH;

        if ((*output->vbridge_forward)(pak, output)) {
            srb_in_counters(input, length);
	    srb_incr_vring_counters(input, vre, length);
	}

	return(TRUE);
    }

    srb_in_counters(input, length);
    /*
     * Time for access lists.  What fun.  Don't forget to go back to the
     * start of the embedded frame once the access list garbage is done.
     */
    if (output->srb_access_out || global_proxy_explorer_enable ||
	global_netbios_name_cache_enable) {
	trrif_t *trh;

	if (fs_netbios_output_acl_check(output)) {
            srb_in_backout_counters(input, length);
	    return(FALSE);
        }
	mci2buffer(&inreg->readlong, mac_acl, MAC_ACL_BYTES);
	trh = (trrif_t *)mac_acl;
	/*
	 * Do proxy explorers, if needed
	 */
	if (global_proxy_explorer_enable) {
	    riflen = trh->rc_blth & RC_LTHMASK;
	    llc1 = (llc1_hdr_t *)((uchar *)&trh->rc_blth + riflen);
	    if (((llc1->control & ~LLC1_P) == LLC1_XID) ||
		((llc1->control & ~LLC1_P) == LLC1_TEST)) {
		rif_update(NULL, vrg->vrn, trh->saddr, NULL,
			   (srbroute_t *) &trh->rc_blth, RIF_AGED_REM);
	    }
	}

	/*
	 * Do NetBIOS name caching, if needed
	 */
	if (global_netbios_name_cache_enable) {
	    netbios_name_cache_update_pak((uchar *) trh, NETBIOS_NAME_AGED_REM,
					  NULL, vrg->vrn);
	}

	if (output->srb_access_out &&
	    (!srb_out_check(mac_acl, length, output,
			   trh->rc_blth & RC_LTHMASK))) {
	    inreg->cmdreg = MCI_CMD_RX_FLUSH;
	    return(TRUE);
	}
	inreg->argreg = data_offset;
	inreg->cmdreg = MCI_CMD_RX_SELECT;
    }


    /*
     * Finally, check the thing out for proper sequencing. THIS FUNCTION CANNOT
     * RETURN FALSE AFTER THIS POINT!!
     */
    RSRB_FST_ACCEPTOR(vrp, seq_num, fst_result)

    if (!fst_result) {
	input->counters.input_drops++;
	vrp->drops++;
	if (srbevent_debug)
	    buginf("\nRSRB: FST acceptor failed");
	inreg->cmdreg = MCI_CMD_RX_FLUSH;
	return(TRUE);
    }
    /*
     * Okay. We got a frame.
     */
    vrp->pkts_rx++;

    if (inreg == output->mci_regptr) {

        if (is_tokenring(output)) {	/* Output interface is tring */

	    inreg->argreg = output->mci_index;
	    inreg->cmdreg = MCI_CMD_RX_MOVE;
	    /*
	     * The cBus rx_move is extremely slow -- assume that it is
	     * going to be successfull, count some totals while waiting.
	     */
	    if (inreg->cmdreg == MCI_RSP_OKAY) {
	        srb_out_counters(output, length);
                srbV.srb_sb_incr_tx_nb_counters(output->firstsw, length);
	    } else {
	        /*
	         * RX_MOVE failed -- drop the datagram.
	         */
	        inreg->cmdreg = MCI_CMD_RX_FLUSH;
	        output->counters.output_total_drops++;
	        return(TRUE);
	    }
	    inreg->argreg = data_offset;
	    inreg->cmdreg = MCI_CMD_TX1_SELECT;
	    inreg->write1short = (AC_PRI4 << 8) | FC_TYPE_LLC;
	    inreg->argreg = (length - 4);
	    inreg->cmdreg = MCI_CMD_TX1_START;
	    /* fastout routine does output accounting */
	    if (srb_debug)
	        buginf("\nRSRB%d: FST faster forward input (vrn %d trn %d), seq %d",
	    	   output->unit, vrn, trn, seq_num);
	    return(TRUE);

        } else if (is_cip_lan(output)) {    /* Output interface is CIP LAN */
            ushort vc = NO_CHANNEL_VC;
            channel_sb_t *chan_sb = channel_get_sb(output);
            srb_mci_read_mac_header_inline(input, data_offset+1);
            if (chan_sb) {
                vc = chan_sb->get_vc_from_macaddr(output, &SRB_DA_BYTES, &SRB_SA_BYTES);
            }
            if (vc != NO_CHANNEL_VC) {
                length -= (FCS_SIZE - CHANNEL_TOKEN_ENCAPBYTES);
                if (chan_sb->emulated_media == CIP_VLAN_TOKEN) {
                    if (srb_mci_cmd_rx_move(output, length)) {
                        srb_mci_tx1_select_offset_inline(input,
                             (data_offset - btow(CHANNEL_TOKEN_ENCAPBYTES)));
                        srbV.srb_sb_incr_tx_nb_counters(output->firstsw, length);
                        srb_mci_cip_token_mac_inline(output, vc);
                        srb_mci_tx1_start_inline(input, length);
                        if (srb_debug)
                            buginf("\nRSRB%d: CIP FST faster forward input (vrn %d trn %d), seq %d",
                                   output->unit, vrn, trn, seq_num);
                    }
                } else if (chan_sb->emulated_media == CIP_VLAN_FDDI) {
                    if (srb_mci_cmd_rx_move(output, length)) {
                        srb_mci_tx1_select_offset_inline(input,
                             (data_offset - btow(CHANNEL_TOKEN_ENCAPBYTES)));
                        srbV.srb_sb_incr_tx_nb_counters(output->firstsw, length);
                        srb_mci_cip_fddi_mac_inline(output, vc);
                        srb_mci_tx1_start_inline(input, length);
                        if (srb_debug)
                            buginf("\nRSRB%d: CIP FST faster forward input (vrn %d trn %d), seq %d",
                                   output->unit, vrn, trn, seq_num);
                    }
                } else {
                    inreg->cmdreg = MCI_CMD_RX_FLUSH;
                    srbV.srb_sb_incr_tx_drops(output->firstsw);
                }
            } else {
                /*
                 * Dest macaddr not configured on CIP internal LAN
                 */
                inreg->cmdreg = MCI_CMD_RX_FLUSH;
                srbV.srb_sb_incr_tx_drops(output->firstsw);
            }
            return(TRUE);
        } else {
            /*
             * Output interface not supported for fastswitching
             *   boot packet up to process level
             */
            srb_in_backout_counters(input, length);
            return(FALSE);
        }
    }

    if ((*vre->vre_hes_fastout)(vre->virtual_idb, output, (uchar *)
			    &inreg->readlong, length, NULL, 0, input)) {
	vre->forwards++;
	srb_out_counters(output, length);
	if (srb_debug)
	    buginf("\nRSRB%d: FST fast forward input (vrn %d trn %d), seq %d",
		   output->unit, vrn, trn, seq_num);
    }
    inreg->cmdreg = MCI_CMD_RX_FLUSH;
    return(TRUE);
} 

/**********************************************************************
 *
 *		     Autonomous SRB/RSRB Support
 *
 **********************************************************************/

static void setup_cbus_srb (hwidbtype *configured)
{
    leveltype level;
    hwidbtype *idb, *target;
    mcitype *cbus;
    mciregtype *regptr;
    tr_vgrouptype *vrg;
    tr_ventrytype *vre, *vp;
    tr_ventrytype *local_vre_list, **vp_addr;
    srb_triplet_t srb_triplet;
    srb_triplet_t target_srb_triplet;
    ulong rbr, cbus_index;
    int i, j, ring;
    boolean trans, srb;

    /*
     * Kick the interface to go out and set all the parameters that hte
     * Spyglass/PH need in order to receive source routed frames.
     */
    if (configured && (configured->status & IDB_CBUS) 
                    && (configured->status & IDB_TR)) {
        srbcore_modify_srb(configured, 0, 0);
    }

    /*
     * Find all Cbus based token rings and FDDI, and indicate their bridging 
     * state to the appropriate cbus controller.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    FOR_ALL_HWIDBS(idb) {
	if (!((idb->type == IDBTYPE_CTR) ||
	      (idb->type == IDBTYPE_FDDIT)))
	    continue;
	regptr = idb->mci_regptr;
	cbus_command_a(regptr, MCI_CMD_SELECT, idb->mci_index);

	/*
	 * Don't configure rings that are down.
	 */
	if (idb->state == IDBS_ADMINDOWN) {
	    cbus_command_la(regptr, FCI_SRB_RING_BRIDGE_RING, -1L);
	    cbus_command_a(regptr, FCI_SRB_TARGET_INDEX, FCI_SRB_NO_TARGET);
	    cbus_command_a(regptr, FCI_SRB_BRIDGING_TYPE, FCI_BRIDGE_NONE);
	    continue;
	}

	/*
	 * Part of a Transparent bridge?
	 */
	trans = RUNNING_IF(LINK_BRIDGE,idb->firstsw) && bridge_enable &&
	    !(system_loading | bootstrap_enable);
	
	/*
	 * Part of a Source routing bridge?
	 */
        srbV.srb_sb_get_triplet(idb->firstsw, &srb_triplet);
	srb = (srb_triplet.bridgeNum <= SRB_MAX_BRIDGE_NUM);
	if (srb) {
	    rbr = ((srb_triplet.localRing & 0xFFF) << 20) |
		  ((srb_triplet.bridgeNum & 0xF) << 16) |
		  ((srb_triplet.remoteRing & 0xFFF) << 4);
	    cbus_command_la(regptr, FCI_SRB_RING_BRIDGE_RING, rbr);

	    /*
	     * Local Bridging?
	     */
	    target = idb->tr_bridge_idb;
            srbV.srb_sb_get_triplet(firstsw_or_null(target), &target_srb_triplet);
	    if (target && !(target->state == IDBS_ADMINDOWN) &&
		((srb_triplet.localRing  == target_srb_triplet.remoteRing) &&
		 (srb_triplet.bridgeNum  == target_srb_triplet.bridgeNum)  &&
		 (srb_triplet.remoteRing == target_srb_triplet.localRing)  &&
		 (target->srb_routecache & SRB_CBUS_ROUTECACHE))) {
		cbus_command_a(regptr, FCI_SRB_TARGET_INDEX,
			       target->mci_index);
	    } else {
		cbus_command_a(regptr, FCI_SRB_TARGET_INDEX,
			       FCI_SRB_NO_TARGET);
	    }
	} else {
	    cbus_command_la(regptr, FCI_SRB_RING_BRIDGE_RING, -1L);
	}

	/*
	 * Now set the bridge type
	 */
	if (srb & trans) {
	    cbus_command_a(regptr, FCI_SRB_BRIDGING_TYPE,
			   FCI_BRIDGE_SRT_STRICT);
	} else if (srb) {
	    cbus_command_a(regptr, FCI_SRB_BRIDGING_TYPE, FCI_BRIDGE_SRB);
	} else if (trans) {
	    cbus_command_a(regptr, FCI_SRB_BRIDGING_TYPE, FCI_BRIDGE_TRANS);
	} else {
	    cbus_command_a(regptr, FCI_SRB_BRIDGING_TYPE, FCI_BRIDGE_NONE);
	}
    }
    reset_interrupt_level(level);

    /*
     * Set up autonomous virtual rings structures.  Delete all current
     * structures first.  The tables must then be handed down in sorted
     * order within each virtual ring.
     */
    for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	if (cbus->flags & CBUS)
	    continue;
	ring = 0;
	regptr = cbus->regptr;
	cbus_command_a(regptr, FCI_SRB_VRING_DESTROY, 0);
	cbus_command_a(regptr, FCI_SRB_VRING_DESTROY, 1);
	for (vrg = (tr_vgrouptype *)vringQ.qhead; vrg; vrg = vrg->next) {
	    local_vre_list = NULL;
	    i = 0; j = 0;
	    for (vre = (tr_ventrytype *) vrg->rings.qhead; vre; vre = vre->next) {
		if (vre->type == RSRB_ET_LOCAL) {
		    idb = vre->outhole.idb;
		    if (!idb || (idb->state == IDBS_ADMINDOWN))
			continue;
		    if (!((idb->type == IDBTYPE_CTR) ||
			  (idb->type == IDBTYPE_FDDIT)))
			continue;
		    if (!(idb->srb_routecache & SRB_CBUS_ROUTECACHE))
			continue;
		    if (idb->mci_regptr != regptr) /* different cbus? */
			continue;

		    cbus_index = ((vre->rn & 0xFFF) << 20) |
			((vre->bn & 0xF) << 16) |
			    idb->mci_index;
		    vre->cbus_index = cbus_index;

                    /*
		     * add vre to local list in sorted order
		     */
                    for(vp_addr = &local_vre_list; *vp_addr; ) {
			 vp = *vp_addr;
			 if(vp->cbus_index > cbus_index)
			     break;
			 vp_addr = &vp->tmp_next;
		    }
                    vre->tmp_next = *vp_addr;
                    *vp_addr = vre;
		}
	    }
	    /*
	     * Anything there?
	     */
	    if (!local_vre_list)
		continue;
	    
	    level = raise_interrupt_level(NETS_DISABLE);
	    cbus_command_a(regptr, FCI_SRB_VRING_CREATE,
			   (vrg->vrn << 4) | ring);
	    for (vp = local_vre_list; vp; vp = vp->tmp_next) {
		cbus_command_la(regptr, FCI_SRB_KEY_INDEX, vp->cbus_index);
	    }
	    reset_interrupt_level(level);
	    if (++ring == 2)
		break;
	}
    }
}


static void mci_fsrb_init (subsystype* subsys)
{
    reg_add_mci_rsrb_fst_forward(mci_rsrb_fst_fast_forward,
				 "mci_rsrb_fst_fast_forward");
    reg_add_mci_vring_input(mci_vring_input, "mci_vring_input");
    reg_add_srt_setup_auto_sw_srb(setup_cbus_srb, "setup_cbus_srb");
    reg_add_mci_expl_vectors(fs_mci_expl_vectors, "fs_mci_expl_vectors");
}


/*
 * Fast SRB subsystem headers
 */

#define FSRB_CBUS_MAJVERSION 1
#define FSRB_CBUS_MINVERSION 0
#define FSRB_CBUS_EDITVERSION 1

SUBSYS_HEADER(mci_fsrb, FSRB_CBUS_MAJVERSION, FSRB_CBUS_MINVERSION, FSRB_CBUS_EDITVERSION,
	      mci_fsrb_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: srb",
	      "req: fastswitch, srb");
