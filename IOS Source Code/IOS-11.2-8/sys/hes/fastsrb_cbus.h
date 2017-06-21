/* $Id: fastsrb_cbus.h,v 3.3.46.5 1996/08/11 23:33:29 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/hes/fastsrb_cbus.h,v $
 *------------------------------------------------------------------
 * fastsrb_cbus.h -- prototypes for fastsrb_cbus.c
 *
 * January 1993, Dave katz
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: fastsrb_cbus.h,v $
 * Revision 3.3.46.5  1996/08/11  23:33:29  ppearce
 * CSCdi61195:  RSRB performance degraded 40% from 11.1(1)
 * Branch: California_branch
 *   Erroneous RSRB debug processing -- missing check for debug enabled
 *
 * Revision 3.3.46.4  1996/07/18  20:28:04  ppearce
 * CSCdi63366:  Move SRB counters from hwidb into SRB subblock
 * Branch: California_branch
 *   - Decreases size of hwidb for non-SRB'd interfaces
 *   - Provides additional IOS-independence for SRB
 *   - Moves SRB a little closer to being completely swidb-based
 *
 * Revision 3.3.46.3  1996/05/29  18:25:31  motto
 * CSCdi46832:  vencap failed debug messages displayed for non cip
 * explorers
 * Branch: California_branch
 *
 * Revision 3.3.46.2  1996/05/17  11:17:16  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.34.3  1996/05/02  22:03:05  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.34.2  1996/04/26  15:18:23  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.34.1  1996/04/03  14:28:23  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.46.1  1996/03/18  19:40:28  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.16.2  1996/03/13  01:21:18  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.16.1  1996/01/24  21:45:27  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/12/11  10:18:57  ppearce
 * CSCdi41891:  Both FS and Process counters are updated on the same packet
 *   Decrement FS counters when packet booted to process level
 *
 * Revision 3.2  1995/11/17  09:16:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:38:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/08/04  14:09:59  motto
 * Changed CIP LAN interface into SRB to only have one entry per LAN
 * instead of per adapter.
 * CSCdi37535:  CIP LAN SRB explorers multiplying by # of adapters defined
 * per LAN
 *
 * Revision 2.5  1995/08/01  21:57:12  motto
 * Needed to add support in the srb-explorerfastswitching code for
 * the CIP interanl LAN interfaces.
 * CSCdi37528:  CIP internal LAN SRB fastswitch explorer does not work
 *
 * Revision 2.4  1995/07/24  06:02:27  motto
 * Code review cleanup
 * CSCdi37365:  CSNA does not work with 2 CIPs in a C7000
 *
 * Revision 2.3  1995/07/23  16:25:57  motto
 * Modify CIP SRB fastswitch to allow CIPxLANa-2-CIPyLANa
 * and CIPxLANa-2-CIPxLANb.
 * CSCdi37365:  CSNA does not work with 2 CIPs in a C7000
 *
 * Revision 2.2  1995/06/08  23:37:00  alam
 * CSCdi35548:  changes per code review comments
 *
 * Revision 2.1  1995/06/07  20:39:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _FAST_SRB_CBUS_H_
#define _FAST_SRB_CBUS_H_

#include "../hes/ctrlr_cmds.h"

/**********************************************************************
 *
 * Speed things up a bit by doing local searches for vrg, vrp and vre.
 * Caller must define vrg, vrp and vre. Results are stored in these
 * variables
 *
 **********************************************************************/
#define FAST_SRB_GET_VRG(ringtofind) \
    for (vrg = (tr_vgrouptype *) vringQ.qhead; vrg; vrg = vrg->next) { \
	if (vrg->vrn == (ringtofind)) \
	    break; \
    }

#define FAST_SRB_GET_VRP_FST(srcipaddress) \
    for (vrp = (tr_vpeertype *) vrg->peers.qhead; vrp; vrp = vrp->next) { \
	if ((vrp->ipaddr == (ipaddrtype) (srcipaddress)) && \
	    (vrp->type == RSRB_PT_FST)) \
	    break; \
    }

#define FAST_SRB_GET_VRE_LOCAL(vrestart, vrern) \
    for (vre = ((tr_ventrytype *) vrestart)->next; vre; vre = vre->next) { \
	if (vre->rn == vrern) { \
	    if (vre->type & 	\
		(RSRB_ET_LOCAL | \
		 RSRB_ET_SDLLC | \
		 RSRB_ET_QLLC  | \
		 RSRB_ET_LOCAL_VIRTUAL)) \
		    break; \
        } \
    }

extern charlong *srb_encap;

#define SRB_DADADADA	srb_encap[0].d.lword
#define SRB_DADASASA	srb_encap[1].d.lword
#define SRB_SASASASA	srb_encap[2].d.lword

#define SRB_DA_BYTES    srb_encap[0].d.byte[0]
#define SRB_SA_BYTES    srb_encap[1].d.byte[2]

/**********************************************************************
 *
 * Inline functions which are common to both RSRB and DLSw fast switching.
 * 
 **********************************************************************/

/*
 * General SRB Accounting Routines
 */

static inline void srb_in_counters (hwidbtype *idb, int dgsize)
{
    GET_TIMESTAMP(idb->lastinput);
    idb->counters.inputs_fast[ACCT_PROTO_SRB]++;
    idb->counters.rx_cumbytes_fast[ACCT_PROTO_SRB] += dgsize;
}

static inline void srb_in_backout_counters (hwidbtype *idb, int dgsize)
{
    idb->counters.inputs_fast[ACCT_PROTO_SRB]--;
    idb->counters.rx_cumbytes_fast[ACCT_PROTO_SRB] -= dgsize;
}

static inline void srb_out_counters (hwidbtype *idb, int dgsize)
{
    idb->counters.outputs_fast[ACCT_PROTO_SRB]++;
    idb->counters.tx_cumbytes_fast[ACCT_PROTO_SRB] += dgsize;
    GET_TIMESTAMP(idb->lastoutput);
}

static inline void srb_out_backout_counters (hwidbtype *idb, int dgsize)
{
    idb->counters.outputs_fast[ACCT_PROTO_SRB]--;
    idb->counters.tx_cumbytes_fast[ACCT_PROTO_SRB] -= dgsize;
    if (srb_debug) {
        SrbDebug_DroppedFastForward(idb);
    }
}

static inline tr_ventrytype *
fast_srb_get_vre (tr_ventrytype *vrestart,
		 ushort vrern, ushort vrebn)
{
    tr_ventrytype *vre_tmp;

    for (vre_tmp = ((tr_ventrytype *) vrestart)->next; vre_tmp; 
	 vre_tmp = vre_tmp->next) { 
	if ((vre_tmp->rn == vrern) && (vre_tmp->bn == vrebn)) 
	    return(vre_tmp);
    }
    return(NULL);
}

static inline boolean srb_mci_cmd_rx_move (hwidbtype *oidb,
                                           int dgsize)
{
    mciregtype *mcireg = oidb->mci_regptr;

    mcireg->argreg = oidb->mci_index;
    mcireg->cmdreg = MCI_CMD_RX_MOVE;
    srb_out_counters(oidb, dgsize);
    if (mcireg->cmdreg != MCI_RSP_OKAY) {
        srb_out_backout_counters(oidb, dgsize);
        oidb->counters.output_total_drops++;
	mcireg->cmdreg = MCI_CMD_RX_FLUSH;
        return FALSE;
    }
    return TRUE;
}

static inline boolean srb_mci_cmd_tx1_reserve ( hwidbtype *oidb)
{
    mciregtype *mcireg = oidb->mci_regptr;

    mcireg->argreg = oidb->buffer_pool;
    mcireg->cmdreg = MCI_CMD_TX1_RESERVE;
    if (mcireg->cmdreg != MCI_RSP_OKAY) {
        oidb->counters.output_total_drops++;
        return FALSE;
    }
    return TRUE;
}

static inline boolean
srb_mci_rx_move_select_inline (hwidbtype *inputhw,
			       hwidbtype *outputhw,
			       ushort tx_offset,
			       int dgsize)
{
    mciregtype *mcireg = inputhw->mci_regptr;

    if (!srb_mci_cmd_rx_move(outputhw, dgsize))
	return(FALSE);

    mcireg->argreg = tx_offset;
    mcireg->cmdreg = MCI_CMD_TX1_SELECT;

    return(TRUE);
}

static inline 
void srb_mci_tx1_select_offset_inline (hwidbtype *hwidb,
				       ushort tx_offset)
{
    mciregtype *mcireg = hwidb->mci_regptr;

    mcireg->argreg = tx_offset;
    mcireg->cmdreg = MCI_CMD_TX1_SELECT;

    return;
}

static inline
void srb_mci_rx_select_offset_inline (hwidbtype *hwidb,
				      ushort rx_offset)
{
    mciregtype *mcireg = hwidb->mci_regptr;

    mcireg->argreg = rx_offset;
    mcireg->cmdreg = MCI_CMD_RX_SELECT;

    return;
}

static inline
void srb_mci_tx1_start_inline (hwidbtype *hwidb,
			      ushort dgsize)
{
    mciregtype *mcireg = hwidb->mci_regptr;

    mcireg->argreg = dgsize;
    mcireg->cmdreg = MCI_CMD_TX1_START;

    return;
}

static inline
void srb_mci_fddi_mac_inline (hwidbtype *hwidb)
{
    mciregtype *mci_regptr = hwidb->mci_regptr;

    mci_regptr->write1short = FDDI_LLC_FC_BYTE;
    mci_regptr->write1long = SRB_DADADADA;
    mci_regptr->write1long = SRB_DADASASA;
    mci_regptr->write1long = SRB_SASASASA;
}

static inline
void srb_mci_tring_mac_inline (hwidbtype *hwidb)
{
    mciregtype *mci_regptr = hwidb->mci_regptr;

    mci_regptr->write1short = (AC_PRI4 << 8) | FC_TYPE_LLC;
    mci_regptr->write1long = SRB_DADADADA;
    mci_regptr->write1long = SRB_DADASASA;
    mci_regptr->write1long = SRB_SASASASA;
}

static inline
void srb_madge_tring_mac_inline (void *p)
{
    ushort *ptr = p;
    *ptr++ = (AC_PRI4 << 8) | FC_TYPE_LLC;
    *ptr++ = srb_encap[0].d.sword[0];
    *ptr++ = srb_encap[0].d.sword[1];
    *ptr++ = srb_encap[1].d.sword[0];
    *ptr++ = srb_encap[1].d.sword[1];
    *ptr++ = srb_encap[2].d.sword[0];
    *ptr = srb_encap[2].d.sword[1];
}

static inline
void srb_mci_swap_mac_address_inline (void)
{
    srb_encap[0].d.byte[0] = BITSWAP(srb_encap[0].d.byte[0]);
    srb_encap[0].d.byte[1] = BITSWAP(srb_encap[0].d.byte[1]);
    srb_encap[0].d.byte[2] = BITSWAP(srb_encap[0].d.byte[2]);
    srb_encap[0].d.byte[3] = BITSWAP(srb_encap[0].d.byte[3]);
    srb_encap[1].d.byte[0] = BITSWAP(srb_encap[1].d.byte[0]);
    srb_encap[1].d.byte[1] = BITSWAP(srb_encap[1].d.byte[1]);
    srb_encap[1].d.byte[2] = BITSWAP(srb_encap[1].d.byte[2]);
    srb_encap[1].d.byte[3] = BITSWAP(srb_encap[1].d.byte[3]);
    srb_encap[2].d.byte[0] = BITSWAP(srb_encap[2].d.byte[0]);
    srb_encap[2].d.byte[1] = BITSWAP(srb_encap[2].d.byte[1]);
    srb_encap[2].d.byte[2] = BITSWAP(srb_encap[2].d.byte[2]);
    srb_encap[2].d.byte[3] = BITSWAP(srb_encap[2].d.byte[3]);
}

static inline
void srb_mci_read_mac_header_inline (hwidbtype *hwidb, 
				     ushort rx_offset)
{
    mciregtype *mci_regptr;

    mci_regptr = hwidb->mci_regptr;
    mci_regptr->argreg = rx_offset;
    mci_regptr->cmdreg = MCI_CMD_RX_SELECT;

    /*
     * Read the destination and source MAC addresses.
     */
    SRB_DADADADA = mci_regptr->readlong;
    SRB_DADASASA = mci_regptr->readlong;
    SRB_SASASASA = mci_regptr->readlong;
}

static inline
void srb_mci_cip_fddi_mac_inline (hwidbtype *hwidb, ushort vc)
{
    mciregtype *mci_regptr = hwidb->mci_regptr;

    mci_regptr->write1long = vc << 16 | 0x2001; 
    mci_regptr->write1long = 0x00020000 | FDDI_LLC_FC_BYTE;
    mci_regptr->write1long = SRB_DADADADA;
    mci_regptr->write1long = SRB_DADASASA;
    mci_regptr->write1long = SRB_SASASASA;
}

static inline
void srb_mci_cip_token_mac_inline (hwidbtype *hwidb, ushort vc)
{
    mciregtype *mci_regptr = hwidb->mci_regptr;

    mci_regptr->write1long = vc << 16 | 0x2001;
    mci_regptr->write1long = 0x00010000 | (AC_PRI4<<8) | FC_TYPE_LLC;
    mci_regptr->write1long = SRB_DADADADA;
    mci_regptr->write1long = SRB_DADASASA;
    mci_regptr->write1long = SRB_SASASASA;
}

static inline
void srb_mci_cip_token_encap_inline (hwidbtype *hwidb, ushort vc)
{
    mciregtype *mci_regptr = hwidb->mci_regptr;

    mci_regptr->write1long  = vc << 16 | 0x2001;
    mci_regptr->write1short = 0x0001;
}


/**********************************************************************
 *
 *                     FAST DIRECT RSRB OUTPUT
 *
 * This set of routines supports the direct encapsulation of RSRB packets
 * over the various types of interfaces.  This is the output (token ring
 * -> rsrb) path only.  The input path is elsewhere.  These routines do
 * not have to perform any assess checking as the interfaces involved are
 * transit interfaces, no destination interfaces.
 *
 **********************************************************************/
static inline void write_fst_header (
    mciregtype *regptr,
    ulong *fst_hdr)
{
    /* writing ip  header */
    regptr->write1long = fst_hdr[0];
    regptr->write1long = fst_hdr[1];
    regptr->write1long = fst_hdr[2];
    regptr->write1long = fst_hdr[3];
    regptr->write1long = fst_hdr[4];
}

static inline void fst_write_ether_header (
    mciregtype *regptr,
    ipcache_rntype *cptr,
    ulong *fst_hdr)
{
    regptr->write1long = cptr->m.maclong[0];
    regptr->write1long = cptr->m.maclong[1];
    regptr->write1long = cptr->m.maclong[2];
    regptr->write1short = TYPE_IP10MB;

    /* write ip header */
    write_fst_header(regptr, fst_hdr);
}


static inline void fst_write_fddi_header (
    mciregtype *regptr,
    ipcache_rntype *cptr,
    riftype *rifptr,
    ulong *fst_hdr)
{
    int i;
    ushort *rif;

    regptr->write1short = FDDI_LLC_FC_BYTE;
    regptr->write1short = cptr->m.macshort[0];
    regptr->write1short = cptr->m.macshort[1];
    regptr->write1short = cptr->m.macshort[2];
    if (rifptr && rifptr->length)
        regptr->write1short = cptr->m.macshort[3] | (FDDIT_RII << 8);
    else
        regptr->write1short = cptr->m.macshort[3];
    regptr->write1short = cptr->m.macshort[4];
    regptr->write1short = cptr->m.macshort[5];
    if (rifptr && rifptr->length) {
        for (i=0, rif=(ushort *)rifptr->rif; i < rifptr->length; i+=2)
            regptr->write1short = *rif++;
    }
    regptr->write1short = SNAPSNAP;
    regptr->write1short = (LLC1_UI << 8) | 0;
    regptr->write1long = TYPE_IP10MB;

    /* write ip header */
    write_fst_header(regptr, fst_hdr);
}

static inline void fst_write_serial_header (
    mciregtype *regptr,
    ipcache_rntype *cptr,
    ulong *fst_hdr)
{
    if (cptr->length == 6 )
        regptr->write1long = cptr->m.maclong[0];
    else
        regptr->write1short =  cptr->m.macshort[0];
    regptr->write1short = TYPE_IP10MB;

    /* write ip header */
    write_fst_header(regptr, fst_hdr);
}

static inline void fst_write_token_header (
    mciregtype *regptr,
    ipcache_rntype *cptr,
    riftype *rifptr,
    ulong *fst_hdr)
{
    int i;
    ushort *rif;

    /*
     * Write the Mac header
     */
    regptr->write1short = (AC_PRI4 << 8) | FC_TYPE_LLC;
    regptr->write1short = cptr->m.macshort[0];
    regptr->write1short = cptr->m.macshort[1];
    regptr->write1short = cptr->m.macshort[2];
    if (rifptr && rifptr->length)
        regptr->write1short = cptr->m.macshort[3] | (TR_RII << 8);
    else
        regptr->write1short = cptr->m.macshort[3];
    regptr->write1short = cptr->m.macshort[4];
    regptr->write1short = cptr->m.macshort[5];
    if (rifptr && rifptr->length) {
        for (i=0, rif=(ushort *)rifptr->rif; i < rifptr->length; i+=2)
            regptr->write1short = *rif++;
    }
    regptr->write1short = SNAPSNAP;
    regptr->write1short = (LLC1_UI << 8) | 0;
    regptr->write1short = 0x0000;
    regptr->write1short = TYPE_IP10MB;

    /* write ip header */
    write_fst_header(regptr, fst_hdr);
}

static inline void fst_write_atm_header (
    mciregtype *regptr,
    ipcache_rntype *cptr,
    ulong *fst_hdr)
{

    ushort *src = cptr->m.macshort;
    ushort enclen = cptr->length;

    /*
     * atm encapsulation for outgoing vc is already in cache, so just
     * copy it.  how convenient.
     */

    while(enclen) {
        regptr->write1short = *src++;
        enclen -= sizeof(short);
    }

    /* write ip header */
    write_fst_header(regptr, fst_hdr);
}

#endif /* _FAST_SRB_CBUS_H_ */
