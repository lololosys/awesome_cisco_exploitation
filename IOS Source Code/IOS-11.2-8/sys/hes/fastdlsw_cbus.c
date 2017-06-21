/* $Id: fastdlsw_cbus.c,v 3.9.6.14 1996/08/28 12:43:56 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/fastdlsw_cbus.c,v $
 *------------------------------------------------------------------
 * Fast DLSw for cBus systems
 *
 * September 1994, Frank Bordonaro 
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: fastdlsw_cbus.c,v $
 * Revision 3.9.6.14  1996/08/28  12:43:56  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.9.6.13  1996/08/26  15:06:20  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.9.6.12  1996/08/11  23:33:10  ppearce
 * CSCdi61195:  RSRB performance degraded 40% from 11.1(1)
 * Branch: California_branch
 *   Erroneous RSRB debug processing -- missing check for debug enabled
 *
 * Revision 3.9.6.11  1996/07/23  13:22:34  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.9.6.10  1996/07/18  20:27:53  ppearce
 * CSCdi63366:  Move SRB counters from hwidb into SRB subblock
 * Branch: California_branch
 *   - Decreases size of hwidb for non-SRB'd interfaces
 *   - Provides additional IOS-independence for SRB
 *   - Moves SRB a little closer to being completely swidb-based
 *
 * Revision 3.9.6.9  1996/07/11  14:18:45  fbordona
 * CSCdi62521:  DLSw circuit does not connect using FST over ATM peer
 *              Remove DLSw FST hooks from platform/media specific drivers.
 * Branch: California_branch
 *
 * Revision 3.9.6.8  1996/07/09  05:50:39  ppearce
 * CSCdi59527:  inlines wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.9.6.7  1996/05/29  18:25:08  motto
 * CSCdi46832:  vencap failed debug messages displayed for non cip
 * explorers
 * Branch: California_branch
 *
 * Revision 3.9.6.6  1996/05/24  15:12:45  fbordona
 * CSCdi58465:  misalignment & Spurious Accesses at dlsw_nim_fst2
 * Branch: California_branch
 *
 * Revision 3.9.6.5  1996/05/23  14:13:41  fbordona
 * CSCdi57997:  DLSw FST over WAN Frame Relay can not establish circuits
 *              Fix typo that causes cygnus progressive-96q1 compiler
 *              error.
 * Branch: California_branch
 *
 * Revision 3.9.6.4  1996/05/23  03:21:41  kpatel
 * CSCdi52568:  SNA session cant establish when using dlsw direct
 *              encap/framerelay
 * Branch: California_branch
 *
 * Revision 3.9.6.3  1996/05/22  14:44:53  fbordona
 * CSCdi57997:  DLSw FST over WAN Frame Relay can not establish circuits
 *              Use reg_invoke_framerelay_fs() for frame-relay RXTYPE_DLSW
 *              Remove DLSw FST frame-relay code from interrupt routines.
 * Branch: California_branch
 *
 * Revision 3.9.6.2  1996/05/17  11:17:04  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.8.2.7  1996/05/10  15:14:18  fbordona
 * CSCdi57207:  DLSw FST broken over WAN token ring and FDDI
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.8.2.6  1996/05/02  22:02:44  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.8.2.5  1996/04/26  15:18:15  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.8.2.4  1996/04/19  20:25:17  fbordona
 * CSCdi55172:  DLSw FST peer-on-demand pass data before peer is connected
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.8.2.3  1996/04/03  14:28:13  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.8.2.2  1996/03/21  06:07:16  ppearce
 * IBU modularity: Channel subblock
 * Branch: IbuMod_Calif_branch
 *   Move Channel fields from hwidbtype/idbtype structs to Channel subblock
 *
 * Revision 3.8.2.1  1996/03/17  17:48:41  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.9.6.1  1996/03/18  19:40:16  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.6.4  1996/03/16  06:49:39  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.6.3  1996/03/13  01:21:07  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.6.2  1996/02/20  00:50:54  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/03/05  18:50:02  fbordona
 * CSCdi50574:  DLSw pod stuck in connect status.
 *              Use number of circuit count to disconnect pods.
 *
 * Revision 3.8  1996/02/27  03:15:07  kpatel
 * CSCdi46309:  DLSw+ fast switching on 7000 broken
 *
 * Revision 3.7  1996/02/23  17:32:53  hampton
 * Clean up platform specific DLSW defines in commonly included header
 * files.  [CSCdi48844]
 *
 * Revision 3.6  1996/02/01  06:02:21  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5.6.1  1996/01/24  21:45:17  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.5  1996/01/11  22:25:03  kpatel
 * CSCdi46459:  Serial : Input counters dont increment for FST
 *
 * Revision 3.4  1995/11/20  22:43:35  fbordona
 * CSCdi43047:  show dlsw fastcache crashes when displaying large rifs
 *              Do not lock down ip cache entry for FST to take
 *              advantage of ip re-route.
 *
 * Revision 3.3  1995/11/17  09:16:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:26:16  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:38:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/10/19  13:25:59  ppearce
 * CSCdi42462:  DLSw+ cannot fast switch packets to/from CIP
 *   Add CIP-specific fast switch code for DLSw+
 *
 * Revision 2.4  1995/09/25  08:33:28  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.3  1995/08/07  05:33:33  richl
 * CSCdi38315:  makefile rework
 *              move code to tring_inline and to rif_util in libutil.a
 *
 * Revision 2.2  1995/06/09  01:07:02  hampton
 * Remove more useless includes of obsolete files.  [CSCdi35520]
 *
 * Revision 2.1  1995/06/07 20:39:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "../if/channel.h"
#include "channel_sb.h"
#include "../if/ether.h"
#include "if_hes_common.h"
#include "../if/rif.h"
#include "../if/rif_inline.h"
#include "../llc2/llc2.h"
#include "../srt/srb.h"
#include "../srt/srb_core.h"
#include "../srt/rsrb.h"
#include "../srt/srb_error.h"
#include "if_mci.h"
#include "../if/fddi.h"
#include "../if/atm.h"
#include "../ip/ipfast.h"
#include "../wan/frame_relay.h"
#include "fastsrb_cbus.h"

/*
 * Include files common to fast and slow switching code
 */
#include "logger.h"
#include "../dlsw/msg_dlsw_peer.c"	/* Not a typo, see logger.h */
#include "../dlsw/parser_defs_dlsw.h"
#include "../dlsw/dlsw_dlx.h"
#include "../dlsw/dlsw_ssp.h"
#include "../dlsw/dlsw_peer_externs.h"
#include "../dlsw/dlsw_peer.h"

/*
 * File Scope Function Prototypes
 */
static boolean dlsw_lan_to_mci_fst(peerentry *, fast_cache_t *, uchar *,
				   int, ipcache_rntype *, hwidbtype *);
static boolean dlsw_mci_wan2lan_common(hwidbtype *, ulong, ulong);


/***********************************************************************
 *
 * DLSw specific macros.
 *
 **********************************************************************/

inline static void fst_write_token2_header (uchar *odgstart, 
                   ipcache_rntype *cptr, riftype *rifptr, ushort *fst_ptr,
                   int size, fast_cache_t *fct)

{
    ushort *data, *rif;
    int i;
    ulong *lptr;

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

    /*
     * Write DLX header
     */
    lptr = (ulong *) data;
    *lptr++ = (DLX_FST_VERS_AND_OP | size);
    *lptr++ = fct->remote_id;
    *lptr++ = fct->local_id;
}

inline static void dlsw_write_serial_header (mciregtype *regptr,hwidbtype *out)
{
    if (out->cache_board_encap) {
        ushort temp;
        (*out->cache_board_encap)(out,&temp,sizeof(ushort));
        regptr->write1short = temp;
    }
    /*
     * Write the Mac header
     */
    regptr->write1long = HDLC_DLSW;
}

inline static void dlsw_write_ether_header (mciregtype *regptr, uchar *dst,
                                            uchar *src, int length)
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
    regptr->write1short = TYPE_DLSW;
}

inline static void dlsw_write_fddi_header (mciregtype *regptr, uchar *dst, 
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
    regptr->write1short = TYPE_DLSW;
}

static inline void fst_write_fr_header (mciregtype *regptr,
                                        ipcache_rntype *cptr, ulong *fst_hdr)
{
    regptr->write1long = cptr->m.maclong[0];
    write_fst_header(regptr, fst_hdr);
}

inline static void dlsw_write_token_header (mciregtype *regptr, uchar *dst,
                                           uchar *src, riftype *rifptr)
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
    regptr->write1short = TYPE_DLSW;
}


inline static void dlsw_write_token2_header (uchar *odgstart, int odgsize,
                   peerentry *peer, uchar *src, 
                   fast_cache_t *fct, riftype *rifptr)
{
    ushort *data, *rif;
    int i;
    ulong *lptr;

    /*
     * Write the Mac header
     */
    data = (ushort *)odgstart;
    *data++ = (AC_PRI4 << 8) | FC_TYPE_LLC;
    *data++ = *(ushort *)(peer->macaddr+0);
    *data++ = *(ushort *)(peer->macaddr+2);
    *data++ = *(ushort *)(peer->macaddr+4);
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
    *data++ = TYPE_DLSW;

    /*
     * Write the DLSW header.
     */
    lptr = (ulong *) data;
    *lptr++ = (DLX_FST_VERS_AND_OP | odgsize);
    *lptr++ = fct->remote_id;
    *lptr++ = fct->local_id;
}

static inline void fst_write_header (mciregtype *regptr,
                                     ipcache_rntype *cptr, ulong *fst_hdr)
{
    ushort i;

    for (i = 0; i < cptr->length/2; i++)
        regptr->write1short = cptr->m.macshort[i];

    write_fst_header(regptr, fst_hdr);
}

/***********************************************************************
 *
 * DLSw FST routines.
 *
 **********************************************************************/

/*
 * srb_mci_to_dlsw()
 *
 * Do not have to consider
 *  1. local bridging
 *  2. LAN input access list filters (since session is established)
 *  3. WAN output filtering (since session is established)
 *
 * srb_in_counters() incremented by caller.
 */

static boolean srb_mci_to_dlsw (hwidbtype *idb, uchar *dgstart,
				int dgsize, int riflen)
{
    ushort macs[IEEEBYTES]; /* to hold daddr and saddr */
    union {
        sap_hdr saph;
        ushort lsap;
        ulong long_read;
    } sapu;
    int ii;
    fast_cache_t *fct;
    int rc;
    char buff[DLSW_MAX_PEER_ID];
    ushort mci_receive_offset;

    if (is_tokenring(idb)) {
        mci_receive_offset = CTR_RECEIVE_OFFSET;
    } else if (is_cip_lan(idb)) {
        mci_receive_offset = CHANNEL_TOKEN_ACFC_OFFSET;
    } else {
        mci_receive_offset = CTR_RECEIVE_OFFSET;
    }

    /*
     * Try to find circuit entry for this packet.
     * Read in the daddr, saddr, dsap, ssap to find circuit entry.
     * CTR_RECEIVE_OFFSET points to start of TR header.
     */
    idb->mci_regptr->argreg = mci_receive_offset + (TR_DA_OFFSET >> 1);
    idb->mci_regptr->cmdreg = MCI_CMD_RX_SELECT;
    for (ii = 0; ii < IEEEBYTES; ii++)
        macs[ii] = idb->mci_regptr->readshort;

    idb->mci_regptr->argreg = mci_receive_offset + (TRING_ENCAPBYTES >> 1) + 
                     (riflen >> 1);
    idb->mci_regptr->cmdreg = MCI_CMD_RX_SELECT;
    sapu.long_read = idb->mci_regptr->readlong;
 
    fct = dlsw_tr2fct((uchar *) macs, &sapu.lsap);
    if (!fct) {
        return(FALSE);
    } else if (((peerentry *) fct->peer_handle)->conn_state != PEER_CONN) {
        if (((peerentry *) fct->peer_handle)->debug_fast_err)
            buginf("\nDLSw: srb_madge_to_dlsw: peer not yet connected");
        return(FALSE);
    }
 
    /*
     * Do not fast switch NULL XID P, SABME, NetBIOS NQ or NR.
     * This allows CSM to learn new reachability info.
     * Give CSM a chance to filter/firewall NetBIOS UI frames.
     */
    sapu.saph.control &= ~P_BIT;
    if ((sapu.saph.control == SABME) || (sapu.saph.control == LLC1_XID) ||
        (sapu.saph.control == LLC1_TEST)) {
        return(FALSE);
    } else if ((sapu.saph.dsap == SAP_NETBIOS) &&
        (sapu.saph.control == LLC1_UI)) {
        return(FALSE);
    }

    if (((peerentry *) fct->peer_handle)->fast_hes_lan2wan == NULL) {
        /*
         * Chances are the peer-on-demand-defaults are not in sync
         * on the two peers. Do not pop this packet up to the core
         * since the session will never come up.
         */
        if (((peerentry *) fct->peer_handle)->debug_fast_err) {
            buginf("\nDLSw: %s: fast switching not enabled for %s",
                   idb->hw_namestring,
                   peer_str((peerentry *) fct->peer_handle, buff));
        }
        idb->mci_regptr->cmdreg = MCI_CMD_RX_FLUSH;
        return(TRUE);
    }

    /*
     * Fast switch the packet.
     * Since DLSw must alter the token ring packet by inserting a new
     * rif, the Frame Check Sequence is useless, so remove it from
     * the packet.
     *
     * fast_lan2wan points to dlsw_lan2fst() for FST peers.
     */
    rc = (*((peerentry *) fct->peer_handle)->fast_hes_lan2wan)(idb, fct,
                         dgstart, (ulong) (dgsize - TR_FCS_SIZE));
    if (rc == TRUE) {
        ((peerentry *)fct->peer_handle)->pkts_tx++;
        if (((peerentry *) fct->peer_handle)->debug_fast_pak)
            buginf("\nDLSw: srb_mci_to_dlsw() %s to %s",
                   idb->hw_namestring,
                   peer_str((peerentry *) fct->peer_handle, buff));

        /* count vring transmit and receive for LNM */
        srbV.srb_sb_incr_tx_nb_counters(idb->vidb_link->firstsw, dgsize);
    } else {
        ((peerentry *) fct->peer_handle)->drops++;
        if (((peerentry *) fct->peer_handle)->debug_fast_pak)
            buginf("\nDLSw: srb_mci_to_dlsw(): dropped - %s to %s",
                   idb->hw_namestring,
                   peer_str((peerentry *) fct->peer_handle, buff));
    }
    return(TRUE);
}


/*
 * dlsw_lan2fst()
 *
 * Fast switch token ring packet to a dlsw FST remote peer using
 * UDP/IP encapsulation.
 *
 * Called by srb_mci_to_dlsw() with peer->fast_lan2wan vector.
 *
 * Fast switches token ring packets out to an FST peer by calling
 * dlsw_nim_to_fst().
 */

static boolean dlsw_lan2fst (hwidbtype *input, fast_cache_t *fct,
			     uchar *dgstart, ulong dgsize)
{
    peerentry *peer = (peerentry *) fct->peer_handle;
    ipcache_rntype *cptr;
    paktype *pak;
    dlx_fast_hdr_t *fast_hdr;
    iphdrtype *iphead;
    ulong totsize;

    /*
     * Lookup ip cache to see if we have valid entry.
     * If found, update it's last used field since ipcache_lookup_fast()
     * doesn't do this for us.
     */
    cptr = ipcache_lookup_fast(peer->ipaddr);
    if (cptr) {
        if (cptr->idb->hwptr->status & (IDB_MCI | IDB_CBUS)) {
            return(dlsw_lan_to_mci_fst(peer, fct, dgstart,
                                       dgsize, cptr, input));
        } else {
            PDEBUG_FERR("\nDLSw: lan2fst: wan interface is not MCI");
            return(FALSE);
        }
    } else {

	input->counters.cache_miss[ACCT_PROTO_IP]++;

        /*
         * Since fct is valid for the macs/saps, the ip fast cache entry
         * has timed out, send this one to ip input. It will repopulate the
         * ip fast cache.
         */
        PDEBUG_FPAK("\ndlsw_lan2fst(): ip cache MISS for %i", peer->ipaddr);

        /*
         * Start by reading the datagram into memory.
         * Set pointer to start of packet and read it into packet buffer.
         * The arguments to mci2buffer need to be fudged because the cbus
         * can only be accessed in shortwords.
         */
        totsize = IPHEADERBYTES(NOPT) + sizeof(dlx_fast_hdr_t) + dgsize;
        pak = input_getbuffer(totsize, input);
        if (!pak) {
            if (input->status & (IDB_MCI | IDB_CBUS)) {
                input->mci_regptr->cmdreg = MCI_CMD_RX_FLUSH;
                cbus_mci_idb_throttle(input);
            }
            PDEBUG_FERR("\nDLSw: lan2fst: input_getbuffer() failed tr->%i",
                   pak->desthost);
            return(FALSE);
        }
        pak->datagramsize = totsize;

        /*
         * Copy in the ip header from the peer structure
         * Even though pak is being sent through ip, it still must be
         * sequenced.
         */
        iphead = (iphdrtype *) pak->datagramstart;
        bcopy((uchar *)peer->ip_header, (uchar *)iphead, IPHEADERBYTES(NOPT));
        dlsw_fst_incr_send(peer, &iphead->id);

        iphead->tl = pak->datagramsize;
        iphead->checksum = 0;                       /* clear checksum field */
        iphead->checksum = ipcrc((ushort *) iphead, ltob(iphead->ihl));

        pak->linktype = LINK_IP;
        pak->desthost = iphead->dstadr;

        /*
         * Write Fast DLX header and original end station packet.
         */
        fast_hdr = (dlx_fast_hdr_t *)
                   (pak->datagramstart + IPHEADERBYTES(NOPT));
        fast_hdr->packet_length = sizeof(dlx_fast_hdr_t) + dgsize;
        fast_hdr->proto_ver_id = DLX_PROTO_VERSION;
        fast_hdr->message_type = DLX_FAST_PASS;
        fast_hdr->session_id = fct->remote_id;
        fast_hdr->src_session_id = fct->local_id;

        if (input->status & (IDB_MCI | IDB_CBUS)) {
            input->mci_regptr->argreg = CTR_RECEIVE_OFFSET;
            input->mci_regptr->cmdreg = MCI_CMD_RX_SELECT;
            mci2buffer(&input->mci_regptr->readlong, fast_hdr + 1, dgsize+1);
            input->mci_regptr->cmdreg = MCI_CMD_RX_FLUSH;
        } else {
            scopy(dgstart, fast_hdr + 1, dgsize);
        }

        /*
         * Send it to ip for forwarding.
         * This will populate the ip fast cache.
         * Forced to use ip_simple_enqueue() instead of ipwrite()
         * because ip fast cache entries can not be made at interrupt
         * level.
         */
        PDEBUG_FPAK("\nDLSw: dlsw_lan2fst(): to %i via ip - size %d seq %d",
                    pak->desthost, pak->datagramsize, iphead->id);
        ip_simple_enqueue(pak);
        return(TRUE);
    }
}


/*
 * dlsw_lan_to_mci_fst()
 *
 * Fast switch SRB packet to dlsw peer using ip encapsulation.
 * Input token ring interface could be mci.
 * Output FST interface is an MCI style interface (mbus or cbus).
 */

static boolean dlsw_lan_to_mci_fst (peerentry *peer, fast_cache_t *fct,
                                    uchar *dgstart, int dgsize,
                                    ipcache_rntype *cptr, hwidbtype *input)
{
    hwidbtype *output;
    mciregtype *dstreg;
    ulong totalsize;
    riftype *rifptr;
    iphdrtype *iphead;

    output = cptr->idb->hwptr;
    totalsize = dgsize + FST_IP_HDR_SIZE + sizeof(dlx_fast_hdr_t);
    if (totalsize > output->maxmtu) {
        PDEBUG_FERR("\ndlsw_mci_to_fst(): pak size %d exceeds mtu %d",
                    totalsize, output->maxmtu);
        input->mci_regptr->cmdreg = MCI_CMD_RX_FLUSH;
        output->counters.output_total_drops++;
        return(FALSE);
    }

    /*
     * m-bus token ring interfaces are not MCI style so their regptr
     * will be zero and the check below will be false.
     */
    dstreg = output->mci_regptr;
    iphead = peer->ip_header;
    if (dstreg == input->mci_regptr) {
        /*
         * The input and output interfaces are on the SP.
         * Since m-bus MCI cards do not have token ring, we know the
         * input interface is an SP interface.
         */
        dstreg->argreg = output->mci_index;
        dstreg->cmdreg = MCI_CMD_RX_MOVE;

        /*
         * RX_MOVE is slow.  Assume it will be sucessful and count totals
         * while waiting for it to complete.
         * Now do necessary modifications to the ip header.
         */
        iphead->tl = totalsize;
        iphead->checksum = 0;

        if (dstreg->cmdreg == MCI_RSP_OKAY) {
            /* only increment send count if RX_MOVE successful */
            dlsw_fst_incr_send(peer, &iphead->id);
            iphead->checksum = ipcrc((ushort *) iphead, ltob(iphead->ihl));

            /*
             * Lay in the MAC/IP/DLX header.
             */
            if (output->status & IDB_SERIAL) {
                if (output->enctype == ET_HDLC) {
                    dstreg->argreg = input->rxoffset - btow(output->encsize) -
                        (sizeof(dlx_fast_hdr_t)/2) - FST_IP_HDR_SIZE/2 ;
                    dstreg->cmdreg = MCI_CMD_TX1_SELECT;
                    totalsize += output->encsize;
                    fst_write_serial_header(dstreg, cptr, (ulong *)iphead);
                } else if (output->enctype == ET_FRAME_RELAY) {
                    dstreg->argreg = input->rxoffset - btow(output->encsize) -
                        (sizeof(dlx_fast_hdr_t)/2) - FST_IP_HDR_SIZE/2 ;
                    dstreg->cmdreg = MCI_CMD_TX1_SELECT;
                    totalsize += output->encsize;
                    fst_write_fr_header(dstreg, cptr, (ulong *) iphead);
                } else if (output->enctype == ET_ATM_DXI) {
                    ATM_VCOUTFAST(output, cptr->m.macshort[0]);
                    ATM_OUTFAST(output);
                    dstreg->argreg = input->rxoffset - btow(cptr->length)
                        - (sizeof(dlx_fast_hdr_t)/2) - FST_IP_HDR_SIZE/2;
                    totalsize += cptr->length;
                    dstreg->cmdreg = MCI_CMD_TX1_SELECT;
                    fst_write_header(dstreg, cptr, (ulong *)iphead);
                } else {
                    return(FALSE);
                }
            } else if (output->status & IDB_ETHER) {
                dstreg->argreg = input->rxoffset - ETHER_ARPA_ENCAPBYTES/2 -
                    (sizeof(dlx_fast_hdr_t)/2) - FST_IP_HDR_SIZE/2;
                dstreg->cmdreg = MCI_CMD_TX1_SELECT;
                totalsize += ETHER_ARPA_ENCAPBYTES;
                fst_write_ether_header(dstreg, cptr, (ulong *)iphead);
            } else if (output->status & IDB_FDDI) {
		rifptr = get_rif_inline(output, cptr->m.macstring);
                if (rifptr) {
                    dstreg->argreg = input->rxoffset - FDDI_SNAP_ENCAPWORDS
                        - (sizeof(dlx_fast_hdr_t)/2) - rifptr->length/2
                            - FST_IP_HDR_SIZE/2;
                    totalsize += (FDDI_SNAP_ENCAPBYTES + rifptr->length);
                } else {
                    dstreg->argreg = input->rxoffset - FDDI_SNAP_ENCAPWORDS
                        - (sizeof(dlx_fast_hdr_t)/2) - FST_IP_HDR_SIZE/2;
                    totalsize += FDDI_SNAP_ENCAPBYTES;
                }
                dstreg->cmdreg = MCI_CMD_TX1_SELECT;
                totalsize |= MCI_TX_ODDALIGN;
                fst_write_fddi_header(dstreg, cptr, rifptr, (ulong *)iphead);
            } else if (output->status & IDB_TR) {
		rifptr = get_rif_inline(output, cptr->m.macstring);
                if (rifptr) {
                    dstreg->argreg = input->rxoffset - TRING_SNAP_ENCAPBYTES/2
                        - (sizeof(dlx_fast_hdr_t)/2) - rifptr->length/2
                            - FST_IP_HDR_SIZE/2;
                    totalsize += (TRING_SNAP_ENCAPBYTES + rifptr->length);
                } else {
                    dstreg->argreg = input->rxoffset - TRING_SNAP_ENCAPBYTES/2
                        - (sizeof(dlx_fast_hdr_t)/2) - FST_IP_HDR_SIZE/2;
                    totalsize += TRING_SNAP_ENCAPBYTES;
                }
                dstreg->cmdreg = MCI_CMD_TX1_SELECT;
                fst_write_token_header(dstreg, cptr, rifptr, (ulong *)iphead);
            } else if (output->status & IDB_ATM) {
                ATM_VCOUTFAST(output, cptr->m.macshort[0]);
                ATM_OUTFAST(output);
                dstreg->argreg = input->rxoffset - btow(cptr->length)
                    - (sizeof(dlx_fast_hdr_t)/2) - FST_IP_HDR_SIZE/2;
                totalsize += cptr->length;
                dstreg->cmdreg = MCI_CMD_TX1_SELECT;
                fst_write_header(dstreg, cptr, (ulong *)iphead);
            } else {
                PDEBUG_FERR("\nDLSw: FST fast-switching not supported on %s",
                            output->hw_namestring);
                output->counters.output_total_drops++;
                return(FALSE);
            }

            dstreg->write1short = DLX_FST_SHORT_VERS_AND_OP;
            dstreg->write1short = dgsize + sizeof(dlx_fast_hdr_t);
            dstreg->write1long = fct->remote_id;
            dstreg->write1long = fct->local_id;

            /*
             * Send it.
             */
            dstreg->argreg = totalsize;
            dstreg->cmdreg = MCI_CMD_TX1_START;
            output->counters.outputs_fast[ACCT_PROTO_SRB]++;
            output->counters.tx_cumbytes_fast[ACCT_PROTO_SRB] += totalsize;
            return(TRUE);
        } else {
            /*
             * RX_MOVE Failed.  Drop the frame.
             * Most likely cause is that the output interface's
             * tx-queue-limit (see show controller cbus) has been
             * reached. This is indicated by an error code of 0x800E.
             * Increase interface "tx-queue-limit" as a fix but
             * lan input rate probably exceeds wan output capabilities.
             */
            PDEBUG("\ndlsw_mci_to_fst(): RX_MOVE Failed : %x", dstreg->cmdreg);
            dstreg->cmdreg = MCI_CMD_RX_FLUSH;
            output->counters.output_total_drops++;
            return(FALSE);
        }
    }

    /*
     * Welcome to the wonderful world of the AGS+. 
     * Output interface is an m-bus MCI interface.
     * Reserve a buffer in window 1 of the destination MCI style interface.
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
         * drop the frame since we don't want this packet to get
         * switched via process level and then getting packets
         * out of order
         */
        input->mci_regptr->cmdreg = MCI_CMD_RX_FLUSH;
        output->counters.output_total_drops++;
        PDEBUG("\ndlsw_mci_to_fst(): dstreg->cmdreg != MCI_RSP_OKAY");
        return(TRUE);
    }

    /*
     * Now do necessary modifications to ip header.
     */
    iphead->tl = totalsize;
    iphead->checksum = 0;
    dlsw_fst_incr_send(peer, &iphead->id);
    iphead->checksum = ipcrc((ushort *) iphead, ltob(iphead->ihl));

    /*
     * Set up write pointer to base of destination transmit window.
     * Do some input accounting.  Figure out how to write new MAC header
     * and write new mac header and UDP/IP header and copy the received
     * datagram after that.
     */
    dstreg->argreg = 0;
    dstreg->cmdreg = MCI_CMD_TX1_SELECT;
    if (output->status & IDB_ETHER) {
        totalsize += ETHER_ARPA_ENCAPBYTES;
        fst_write_ether_header(dstreg, cptr, (ulong *)iphead);
    } else if (output->status & IDB_SERIAL) {
        totalsize += output->encsize;
        fst_write_serial_header(dstreg, cptr, (ulong *)iphead);
    } else if (output->status & IDB_FDDI) {
	rifptr = get_rif_inline(output, cptr->m.macstring);
	if (rifptr) {
            totalsize += (FDDI_SNAP_ENCAPBYTES + rifptr->length);
	} else {
	    totalsize = FDDI_SNAP_ENCAPBYTES;
	}
        fst_write_fddi_header(dstreg, cptr, rifptr, (ulong *)iphead);
    } else {
	rifptr = get_rif_inline(output, cptr->m.macstring);
        if (rifptr) {
            totalsize += (TRING_SNAP_ENCAPBYTES + rifptr->length);
        } else {
            totalsize = TRING_SNAP_ENCAPBYTES;
        }
        fst_write_token_header(dstreg, cptr, rifptr, (ulong *)iphead);
    }

    /*
     * Write the Fast DLX header.
     */
    dstreg->write1short = DLX_FST_SHORT_VERS_AND_OP;
    dstreg->write1short = dgsize + sizeof(dlx_fast_hdr_t);
    dstreg->write1long = fct->remote_id;
    dstreg->write1long = fct->local_id;

    /*
     * Copy the token ring packet from MCI to MCI
     */
    if (input->status & (IDB_MCI | IDB_CBUS)) {
        input->mci_regptr->argreg = CTR_RECEIVE_OFFSET;
        input->mci_regptr->cmdreg = MCI_CMD_RX_SELECT;
        mci2mci(&input->mci_regptr->readlong, &dstreg->write1long, dgsize);
        input->mci_regptr->cmdreg = MCI_CMD_RX_FLUSH;
    } else {
        buffer2mci(dgstart, &dstreg->write1long, dgsize);
    }

    /*
     * send the packet out
     */
    dstreg->argreg = totalsize;
    dstreg->cmdreg = MCI_CMD_TX1_START;

    /*
     * Keep statistics.
     */
    output->counters.outputs_fast[ACCT_PROTO_SRB]++;
    output->counters.tx_cumbytes_fast[ACCT_PROTO_SRB] += totalsize;
    return(TRUE);
}


/*******************************************************************
 * 
 * FST Encaps functions to receive FST frames from remote peers
 * and forward them onto local token rings.
 *
 *******************************************************************/


/*
 * dlsw_mci_fst2lan()
 *
 * It is used to forward FST datagrams on to the local token Ring.
 * We will bump up to process level if:
 *  - mci/cbus hasn't checked the packet.
 *
 * Assume the following for the packet:
 * 1) IP Checksum is correct.
 * 2) This packet is not an IP fragment.
 * 3) There are no IP options.
 */

static boolean dlsw_mci_fst2lan (hwidbtype *input)
{
    ipaddrtype srcadr, dstadr;
    ushort garbage;
    ushort seq_num;
    ulong trh_offset;

    if (!lpeer)
        return(FALSE);

    /*
     * Note: this code relies on the rx pointer to be sitting at the IP
     * checksum.
     *
     * Short read is ip checksum.  Long reads are ip SA, DA.
     */
    garbage = input->mci_regptr->readshort;
    srcadr = input->mci_regptr->readlong;
    dstadr = input->mci_regptr->readlong;

    /*
     * If it isn't for us, then forward the thing on...
     */
    if (dstadr != lpeer->ipaddr)
        return(FALSE);

    /*
     * If the input was on an FDDI or CTR style interface, then
     * SNIFFBASE+3 has our ID. If not, then get it from SNIFFBASE+1.
     * Do this before overwriting SNIFFBASE+3.
     */
    if ((input->status & IDB_CBUS) &&
        ((input->status & IDB_TR) || (input->status & IDB_FDDI)
         || (input->status & IDB_ATM))) {
        seq_num = input->sniff[SNIFFBASE+3].d.sword[1];
    } else {
        seq_num = input->sniff[SNIFFBASE+1].d.sword[1];
    }

    trh_offset = input->rsrb_vringoffset +
                 (IPHEADERBYTES(NOPT) >> 1) + (sizeof(dlx_fast_hdr_t) >> 1);
    if (input->status & IDB_ETHER)
        trh_offset -= ((ETHER_SNAP_ENCAPBYTES - ETHER_ARPA_ENCAPBYTES) >> 1);

    return(dlsw_mci_wan2lan_common(input, seq_num, trh_offset));
}


/*
 * dlsw_mci_wan2lan_common()
 *
 * Used by FST and Direct mci wan input.
 * MCI card read offset must be pointing to start of DLX header.
 */

static boolean dlsw_mci_wan2lan_common (hwidbtype *input, ulong fst_seq_num,
                                        ulong trh_offset)
{
    mciregtype *inreg;
    dlx_fast_hdr_t dlxh;
    ulong *lptr;
    trrif_t trh;
    fast_cache_t *fct;
    ushort encap_bytes;
    ulong payload_bytes;
    ulong rem_rif_len;
    int rifdiff;
    peerentry *peer;
    ushort *sptr;
    ushort garbage;
    ulong ii, jj;
    char buff[DLSW_MAX_PEER_ID];

    /*
     * Read in the DLX fast header.
     */
    inreg = input->mci_regptr;
    lptr = (ulong *) &dlxh;
    *lptr++ = inreg->readlong;
    *lptr++ = inreg->readlong;
    *lptr++ = inreg->readlong;

    if ((dlxh.message_type != DLX_FAST_PASS) ||
        (dlxh.src_session_id == 0))
        return(FALSE);

    /*
     * Read in token ring header including RC.
     */
    lptr = (ulong *) &trh;
    *lptr++ = inreg->readlong;
    *lptr++ = inreg->readlong;
    *lptr++ = inreg->readlong;
    *lptr++ = inreg->readlong;
    
    /*
     * Don't fast switch if
     * 1. No DlSw fast cache entry for this packet
     * 2. lan interface is not up
     * 3. the peer is not connected
     */
    fct = dlsw_id2fct(dlxh.session_id);
    if ((!fct) || (fct->lanhwidb->state != IDBS_UP) ||
        (((peerentry *) fct->peer_handle)->conn_state != PEER_CONN)) {
        if (fct &&
            ((peerentry *) fct->peer_handle)->debug_fast_err) {
            if (fct->lanhwidb->state != IDBS_UP)
                buginf("\nDLSw: mci_wan2lan_common IDB state not up");
            else
                buginf("\nDLSw: mci_wan2lan_common peer not connected");
        }
        return(FALSE);
    }

    fct->remote_id = dlxh.src_session_id;
    rem_rif_len = trh.rc_blth & RC_LTHMASK;
    rifdiff = rem_rif_len - fct->riflen;
    payload_bytes = dlxh.packet_length - sizeof(dlx_fast_hdr_t) - 
                    TRING_ENCAPBYTES - rem_rif_len;

    peer = (peerentry *) fct->peer_handle;
    peer->pkts_rx++;
    if (peer->xport_type == DLSW_FST) {
        if (!dlsw_fst_acceptor(peer, fst_seq_num)) {
            peer->drops++;
            inreg->cmdreg = MCI_CMD_RX_FLUSH;
            return(FALSE);
        }
        PDEBUG_FPAK("\nDLSw: peer %I: recv seq# %d dlx size %d - out to %s",
                    peer->ipaddr, fst_seq_num, dlxh.packet_length,
                    fct->lanhwidb->hw_namestring);
    } else {
        PDEBUG_FPAK("\nDLSw: wan2lan: dlx size %d - out to %s",
                    dlxh.packet_length, fct->lanhwidb->hw_namestring);
    }

    /*
     * If on the same controller, just move the token ring portion
     * of the wan fst packet to a TX buffer and send it dude.
     */
    srb_in_counters(input, dlxh.packet_length);
    if (inreg == fct->lanhwidb->mci_regptr) {
        /* 
         * Copy original pak (just trh, rif, and payload) from wan input
         * RX buff to Tx buff. Does not include FCS.
         * Ethernet receive interrupt code assumes SNAP encaps, 
         * so correct this.
         */
        inreg->argreg = trh_offset;
        inreg->cmdreg = MCI_CMD_RX_SELECT;
        inreg->argreg = fct->lanhwidb->mci_index;
        inreg->cmdreg = MCI_CMD_RX_MOVE;
        /*
         * The cBus rx_move is extremely slow -- assume that it is
         * going to be successful, count some totals while waiting.
         */
        srb_out_counters(fct->lanhwidb, dlxh.packet_length - 
                                        (sizeof(dlx_fast_hdr_t)));

        if (inreg->cmdreg != MCI_RSP_OKAY) {
            /*
             * RX_MOVE failed -- drop the datagram.
             */
            inreg->cmdreg = MCI_CMD_RX_FLUSH;
            fct->lanhwidb->counters.output_total_drops++;
            peer->drops++;
            PDEBUG_FERR("\nDLSw: mci_wan2lan_common RX_MOVE failed");
            return(FALSE);
        }

        encap_bytes = 0;
        if (is_cip_lan(fct->lanhwidb)) { 
            ushort vc = NO_CHANNEL_VC;
            hwidbtype *oidb = fct->lanhwidb;
            channel_sb_t *chan_sb = channel_get_sb(oidb);
            if (chan_sb) {
                vc = chan_sb->get_vc_from_macaddr(oidb, trh.daddr, trh.saddr);
            }
            if (vc != NO_CHANNEL_VC) {
                inreg->argreg = trh_offset + (rifdiff >> 1)
                                  - btow(CHANNEL_TOKEN_ENCAPBYTES);
                inreg->cmdreg = MCI_CMD_TX1_SELECT;
                if (chan_sb->emulated_media == CIP_VLAN_TOKEN) {
                    srb_mci_cip_token_encap_inline(oidb, vc);
                    encap_bytes += CHANNEL_TOKEN_ENCAPBYTES;
                } else {
                    inreg->cmdreg = MCI_CMD_RX_FLUSH;
                    fct->lanhwidb->counters.output_total_drops++;
                    peer->drops++;
                    PDEBUG_FERR("\nDLSw: mci_wan2lan_common No CIP ILAN");
                    return(FALSE);
                }
            } else {
                inreg->cmdreg = MCI_CMD_RX_FLUSH;
                fct->lanhwidb->counters.output_total_drops++;
                peer->drops++;
                PDEBUG_FERR("\nDLSw: mci_wan2lan_common No CIP vc");
                return(FALSE);
            }

        } else {

            /*
             * Re-write the trh and the rif.
             * May have to shift trh for local rif to fit.
             */
            inreg->argreg = trh_offset + (rifdiff >> 1); 
            inreg->cmdreg = MCI_CMD_TX1_SELECT;
        }

        /*
         * Re-write token ring header.
         */
        *(ushort *)&trh = (AC_PRI4 << 8) | FC_TYPE_LLC;
        lptr = (ulong *) &trh;
        inreg->write1long = (ulong) *lptr++; /* ac, fc, daddr bytes 0-1 */
        inreg->write1long = (ulong) *lptr++; /* daddr bytes 2-5 */
        inreg->write1long = (ulong) *lptr++; /* saddr bytes 0-3 */
        sptr = (ushort *) lptr;
        inreg->write1short = (ushort) *sptr; /* saddr bytes 4-5 */

        /*
         * Insert local rif.
         */
        sptr = (ushort *) fct->rif;
        for (ii = 0; ii < (fct->riflen >> 1); ii++)
            inreg->write1short = *sptr++;
 
        /*
         * Send the packet out.
         */
        encap_bytes += TRING_ENCAPBYTES;
        inreg->argreg = encap_bytes + fct->riflen + payload_bytes;
        inreg->cmdreg = MCI_CMD_TX1_START;

        /*
         * Free receive buffer
         */
        inreg->cmdreg = MCI_CMD_RX_FLUSH;
        return(TRUE);
    }

    /*
     * Increment read pointer past original rif.
     */
    for (jj = 0; jj < ((rem_rif_len - 2) >> 1); jj++)
        garbage = inreg->readshort;

    /*
     * wan2lanout points to dlsw_mci_output() or to dlsw_madge_output().
     */
    if ((*fct->fast_hes_wan2lan)(input, fct, &trh, (uchar *)
                               &inreg->readlong, payload_bytes)) {
        srb_out_counters(fct->lanhwidb, payload_bytes);
    } else {
        PDEBUG_FERR("\nDLSw: mci_wan2lan_common %s failed to %s",
                    peer_str((peerentry *) fct->peer_handle, buff),
                    fct->lanhwidb->hw_namestring);
        peer->drops++;
    }
    inreg->cmdreg = MCI_CMD_RX_FLUSH;
    return(TRUE);
}


/*
 * dlsw_mci_output()
 *
 * MCIs are not on the same controller.
 * input mciregtype read offset points to the token ring packet's payload.
 * dgstart is the address of input mciregtype readlong register.
 *
 * Called by dlsw_mci_wan2lan_common() using the fct->fast_hes_wan2lan vector.
 */

static boolean dlsw_mci_output (hwidbtype *input, fast_cache_t *fct,
				trrif_t *trh, uchar *dgstart,
				int payload_bytes)
{
    mciregtype *dstreg = fct->lanhwidb->mci_regptr;
    ushort *sptr;
    ulong *lptr;
    int ii;

    /*
     * Reserve a buffer in window 1 of the destination MCI
     */
    dstreg->argreg = fct->lanhwidb->mci_index;
    dstreg->cmdreg = MCI_CMD_SELECT;

    /*
     * The MCI doesn't care what the argreg value is -- the FCI requires that
     * it be the buffer pool number i.e. which pool to get buffer from.
     */
    dstreg->argreg = fct->lanhwidb->buffer_pool;
    dstreg->cmdreg = MCI_CMD_TX1_RESERVE;
    if (dstreg->cmdreg != MCI_RSP_OKAY)
        return(FALSE);

    dstreg->argreg = 0;
    dstreg->cmdreg = MCI_CMD_TX1_SELECT;

    /*
     * Write token ring header with local rif.
     */
    lptr = (ulong *) trh;
    dstreg->write1long = (ulong) *lptr++; /* ac, fc, daddr bytes 0-1 */
    dstreg->write1long = (ulong) *lptr++; /* daddr bytes 2-5 */
    dstreg->write1long = (ulong) *lptr++; /* saddr bytes 0-3 */
    sptr = (ushort *) lptr;
    dstreg->write1short = (ushort) *sptr; /* saddr bytes 4-5 */

    sptr = (ushort *) fct->rif;
    for (ii = 0; ii < (fct->riflen >> 1); ii++)
        dstreg->write1short = *sptr++;

    /*
     * Write the payload.
     * Does not include FCS
     */
    if (input->status & (IDB_MCI | IDB_CBUS)) {
        mci2mci(dgstart, &dstreg->write1long, payload_bytes);
    } else {
        buffer2mci(dgstart, &dstreg->write1long, payload_bytes);
    }

    /*
     * Send it.
     * CTRs always regenerate the FCS. 
     */
    dstreg->argreg = payload_bytes + TRING_ENCAPBYTES + fct->riflen;
    dstreg->cmdreg = MCI_CMD_TX1_START;

    return(TRUE);
}


/**********************************************************************
 *
 *                     FAST DIRECT DLSW LAN INPUT
 *
 * Direct Encaps functions to send token ring frames to 
 * remote peers.
 *
 **********************************************************************/


/*
 * dlsw_lan2mcidir()
 *
 * Fast switch token ring packet to a dlsw Direct remote peer using
 * HDLC encapsulation.
 *
 * Called by srb_mci_to_dlsw() with peer->fast_lan2wan vector.
 *
 * Do not have to consider
 *  1. local bridging
 *  2. LAN input access list filters (since session is established)
 *  3. WAN output filtering (since session is established)
 */

static boolean dlsw_lan2mcidir (hwidbtype *input, fast_cache_t *fct,
                                uchar *dgstart, ulong dgsize)
{
    peerentry *peer = (peerentry *) fct->peer_handle;
    mciregtype *dstreg;
    ulong totalsize;

    /*
     * m-bus token ring interfaces are not MCI style so their regptr
     * will be zero and the check below will be false.
     */
    dstreg = peer->outidb->mci_regptr;
    if (dstreg == input->mci_regptr) {
        /*
         * The input and output interfaces are on the SP.
         * Since m-bus MCI cards do not have token ring, we know the
         * input interface is an SP interface.
         */
        dstreg->argreg = peer->outidb->mci_index;
        dstreg->cmdreg = MCI_CMD_RX_MOVE;

        /*
         * RX_MOVE is slow.  Assume it will be sucessful and do some
         * accounting while waiting for it to complete. Just setting
         * totalsize alone doesn't allow enough time for the RX_MOVE
         * to complete. Counters are incremented prematurely but this
         * is better than doing useless work.
         */
        totalsize = dgsize + sizeof(dlx_fast_hdr_t);
        peer->outidb->counters.outputs_fast[ACCT_PROTO_SRB]++;
        peer->outidb->counters.tx_cumbytes_fast[ACCT_PROTO_SRB] += totalsize;
        if (dstreg->cmdreg == MCI_RSP_OKAY) {
            /*
             * Lay in the Media header.
             */
            if (peer->outidb->status & IDB_SERIAL) {
                dstreg->argreg = input->rxoffset - 
                                 btow(peer->outidb->encsize) -
                                 (sizeof(dlx_fast_hdr_t)/2);
                dstreg->cmdreg = MCI_CMD_TX1_SELECT;
                totalsize += peer->outidb->encsize;
                dstreg->write1long = HDLC_DLSW;
            } else {
                PDEBUG_FERR("\nDLSw: Direct fast-switching not supported on %s",
                            peer->outidb->hw_namestring);
                peer->outidb->counters.output_total_drops++;
                return(FALSE);
            }

            dstreg->write1short = DLX_FST_SHORT_VERS_AND_OP;
            dstreg->write1short = dgsize + sizeof(dlx_fast_hdr_t);
            dstreg->write1long = fct->remote_id;
            dstreg->write1long = fct->local_id;

            /*
             * Send it.
             */
            dstreg->argreg = totalsize;
            dstreg->cmdreg = MCI_CMD_TX1_START;
            return(TRUE);
        } else {
            /*
             * RX_MOVE Failed.  Drop the frame.
             * Most likely cause is that the output interface's
             * tx-queue-limit (see show controller cbus) has been
             * reached. This is indicated by an error code of 0x800E.
             * Increase interface "tx-queue-limit" as a fix but
             * lan input rate probably exceeds wan output capabilities.
             */
            PDEBUG_FERR("\nDLSw: lan2mcidir RX_MOVE Failed : %x",
                        dstreg->cmdreg);
            dstreg->cmdreg = MCI_CMD_RX_FLUSH;
            peer->outidb->counters.output_total_drops++;
            return(FALSE);
        }
    }

    /*
     * Welcome to the wonderful world of the AGS+. Output interface
     * is an m-bus MCI interface.
     * Reserve a buffer in window 1 of the destination MCI style interface.
     */
    dstreg->argreg = peer->outidb->mci_index;
    dstreg->cmdreg = MCI_CMD_SELECT;

    /*
     * The MCI doesn't care what the argreg value is -- the FCI requires that
     * it be the buffer pool number i.e. which pool to get buffer from.
     */
    dstreg->argreg = peer->outidb->buffer_pool;
    dstreg->cmdreg = MCI_CMD_TX1_RESERVE;

    if (dstreg->cmdreg != MCI_RSP_OKAY) {
        /*
         * drop the frame since we don't want this packet to get
         * switched via process level and then getting packets
         * out of order
         */
        input->mci_regptr->cmdreg = MCI_CMD_RX_FLUSH;
        peer->outidb->counters.output_total_drops++;
        PDEBUG_FERR("\nDLSw: mci_to_fst dstreg->cmdreg != MCI_RSP_OKAY");
        return(FALSE);
    }

    /*
     * Set up write pointer to base of destination transmit window.
     */
    totalsize = dgsize + sizeof(dlx_fast_hdr_t);
    dstreg->argreg = 0;
    dstreg->cmdreg = MCI_CMD_TX1_SELECT;
    if (peer->outidb->status & IDB_SERIAL) {
        totalsize += peer->outidb->encsize;
        dstreg->write1long = HDLC_DLSW;
    } else {
        PDEBUG_FERR("\nDLSw: Direct fast-switching not supported on %s",
                    peer->outidb->hw_namestring);
        input->mci_regptr->cmdreg = MCI_CMD_RX_FLUSH;
        peer->outidb->counters.output_total_drops++;
        return(FALSE);
    }

    /*
     * Write the Fast DLX header.
     */
    dstreg->write1short = DLX_FST_SHORT_VERS_AND_OP;
    dstreg->write1short = dgsize + sizeof(dlx_fast_hdr_t);
    dstreg->write1long = fct->remote_id;
    dstreg->write1long = fct->local_id;

    /*
     * Copy the data from MCI to MCI
     */
    if (input->status & (IDB_MCI | IDB_CBUS)) {
        mci2mci(dgstart, &dstreg->write1long, dgsize);
        ((mciregtype *) input->mci_regptr)->cmdreg = MCI_CMD_RX_FLUSH;
    } else {
        buffer2mci(dgstart, &dstreg->write1long, dgsize);
    }

    /*
     * send the packet out
     */
    dstreg->argreg = totalsize;
    dstreg->cmdreg = MCI_CMD_TX1_START;

    /*
     * Keep statistics.
     */
    peer->outidb->counters.outputs_fast[ACCT_PROTO_SRB]++;
    peer->outidb->counters.tx_cumbytes_fast[ACCT_PROTO_SRB] += totalsize;
    return(TRUE);
}


/*
 * dlsw_lan2mcifrdir()
 *
 * Fast switch token ring packet to a dlsw FR Direct remote peer
 *
 * Called by srb_mci_to_dlsw() with peer->fast_lan2wan vector.
 *
 * Do not have to consider
 *  1. local bridging
 *  2. LAN input access list filters (since session is established)
 *  3. WAN output filtering (since session is established)
 */

static int dlsw_lan2mcifrdir (hwidbtype *input, fast_cache_t *fct,
			      uchar *dgstart, ulong dgsize)
{
    peerentry *peer = (peerentry *) fct->peer_handle;
    mciregtype *dstreg;
    ulong totalsize;
    fr_idb_struct_type *fr_idb;
    ushort fr_dlci;
    
    fr_idb = peer->outidb->frame_relay_stuff;
    fr_dlci = NUM_TO_DLCI(peer->fr_dlci);

    /*
     * m-bus token ring interfaces are not MCI style so their regptr
     * will be zero and the check below will be false.
     */
    dstreg = (mciregtype *) peer->outidb->mci_regptr;
    if (dstreg == (mciregtype *) input->mci_regptr) {
        /*
         * The input and output interfaces are on the SP.
         * Since m-bus MCI cards do not have token ring, we know the
         * input interface is an SP interface.
         */
        dstreg->argreg = peer->outidb->mci_index;
        dstreg->cmdreg = MCI_CMD_RX_MOVE;

        /*
         * RX_MOVE is slow.  Assume it will be sucessful and do some
         * accounting while waiting for it to complete. Just setting
         * totalsize alone doesn't allow enough time for the RX_MOVE
         * to complete. Counters are incremented prematurely but this
         * is better than doing useless work.
         */
        totalsize = dgsize + sizeof(dlx_fast_hdr_t);
        peer->outidb->counters.outputs_fast[ACCT_PROTO_SRB]++;
        peer->outidb->counters.tx_cumbytes_fast[ACCT_PROTO_SRB] += totalsize;
        if (dstreg->cmdreg == MCI_RSP_OKAY) {
            /*
             * Lay in the Media header.
             */
            if ((peer->outidb->status & IDB_SERIAL) &&
                (peer->outidb->enctype == ET_FRAME_RELAY)) {
                dstreg->argreg = input->rxoffset - 
                                 btow(peer->outidb->encsize) -
                                 (sizeof(dlx_fast_hdr_t)/2);
                dstreg->cmdreg = MCI_CMD_TX1_SELECT;
                totalsize += peer->outidb->encsize;
            /*  if(fr_idb->fr_ietf_encaps) FR_ENCAPS_IETF */
                dstreg->write1short = (fr_dlci | FRAME_RELAY_EA_BITS);
                dstreg->write1short = link2serial(LINK_DLSW);
            } else {
                PDEBUG("\nDLSw: Direct FR fast-switching not supported on %s",
                       peer->outidb->hw_namestring);
                return(FALSE);
            }

            dstreg->write1short = DLX_FST_SHORT_VERS_AND_OP;
            dstreg->write1short = dgsize + sizeof(dlx_fast_hdr_t);
            dstreg->write1long = fct->remote_id;
            dstreg->write1long = fct->local_id;

            /*
             * Send it.
             */
            dstreg->argreg = totalsize;
            dstreg->cmdreg = MCI_CMD_TX1_START;
            return(TRUE);
        } else {
            /*
             * RX_MOVE Failed.  Drop the frame.
             * Most likely cause is that the output interface's
             * tx-queue-limit (see show controller cbus) has been
             * reached. This is indicated by an error code of 0x800E.
             * Increase interface "tx-queue-limit" as a fix but
             * lan input rate probably exceeds wan output capabilities.
             */
            PDEBUG_FERR("\nDLSw: lan2mcifrdir RX_MOVE Failed : %x",
                        dstreg->cmdreg);
            dstreg->cmdreg = MCI_CMD_RX_FLUSH;
            peer->outidb->counters.output_total_drops++;
            return(RSRB_DROP);
        }
    }

    /*
     * Welcome to the wonderful world of the AGS+. Output interface
     * is an m-bus MCI interface.
     * Reserve a buffer in window 1 of the destination MCI style interface.
     */
    dstreg->argreg = peer->outidb->mci_index;
    dstreg->cmdreg = MCI_CMD_SELECT;

    /*
     * The MCI doesn't care what the argreg value is -- the FCI requires that
     * it be the buffer pool number i.e. which pool to get buffer from.
     */
    dstreg->argreg = peer->outidb->buffer_pool;
    dstreg->cmdreg = MCI_CMD_TX1_RESERVE;

    if (dstreg->cmdreg != MCI_RSP_OKAY) {
        /*
         * drop the frame since we don't want this packet to get
         * switched via process level and then getting packets
         * out of order
         */
        ((mciregtype *) input->mci_regptr)->cmdreg = MCI_CMD_RX_FLUSH;
        peer->outidb->counters.output_total_drops++;
        PDEBUG_FERR("\nDLSw: mci_to_fst dstreg->cmdreg != MCI_RSP_OKAY");
        return(RSRB_DROP);
    }

    /*
     * Set up write pointer to base of destination transmit window.
     */
    totalsize = dgsize + sizeof(dlx_fast_hdr_t);
    dstreg->argreg = 0;
    dstreg->cmdreg = MCI_CMD_TX1_SELECT;
    if ((peer->outidb->status & IDB_SERIAL) &&
        (peer->outidb->enctype == ET_FRAME_RELAY)) {
        totalsize += peer->outidb->encsize;
        dstreg->write1short = (fr_dlci | FRAME_RELAY_EA_BITS);
        dstreg->write1short = link2serial(LINK_DLSW);
    } else {
        PDEBUG_FERR("\nDLSw: Direct FR fast-switching not supported on %s",
                    peer->outidb->hw_namestring);
        return(FALSE);
    }

    /*
     * Write the Fast DLX header.
     */
    dstreg->write1short = DLX_FST_SHORT_VERS_AND_OP;
    dstreg->write1short = dgsize + sizeof(dlx_fast_hdr_t);
    dstreg->write1long = fct->remote_id;
    dstreg->write1long = fct->local_id;

    /*
     * Copy the data from MCI to MCI
     */
    if (input->status & (IDB_MCI | IDB_CBUS)) {
        input->mci_regptr->argreg = CTR_RECEIVE_OFFSET;
        input->mci_regptr->cmdreg = MCI_CMD_RX_SELECT;
        mci2mci(&input->mci_regptr->readlong, &dstreg->write1long, dgsize);
        input->mci_regptr->cmdreg = MCI_CMD_RX_FLUSH;
    } else {
        buffer2mci(dgstart, &dstreg->write1long, dgsize);
    }

    /*
     * send the packet out
     */
    dstreg->argreg = totalsize;
    dstreg->cmdreg = MCI_CMD_TX1_START;

    /*
     * Keep statistics.
     */
    peer->outidb->counters.outputs_fast[ACCT_PROTO_SRB]++;
    peer->outidb->counters.tx_cumbytes_fast[ACCT_PROTO_SRB] += totalsize;
    return(TRUE);
}


/**********************************************************************
 *
 *                     FAST DIRECT DLSW WAN INPUT
 *
 * These routines accept a direct DLSW packet from an interface, and
 * send it out onto a locally attached token ring.
 *
 **********************************************************************/


/*
 * dlsw_mci_dir2lan()
 *
 * Receive a DLSw Direct packet from an MCI style interface, and forward the
 * extraced frame out onto a token ring.
 *
 * Do not have to consider
 *  1. local bridging
 *  2. LAN input access list filters (since session is established)
 *  3. WAN output filtering (since session is established)
 */

static boolean dlsw_mci_dir2lan (hwidbtype* input)
{
    /*
     * set the Rx buffer offset to the DLX fast header.
     */
    input->mci_regptr->argreg = input->rsrb_vringoffset;
    input->mci_regptr->cmdreg = MCI_CMD_RX_SELECT;
    return(dlsw_mci_wan2lan_common(input, 0, 
           input->rsrb_vringoffset + (sizeof(dlx_fast_hdr_t) >> 1)));
}


/*******************************************************************
 *
 *       General fast switching admin functions
 *
 *******************************************************************/

static void dlsw_fct_enable (hwidbtype *idb, void *fct_parm)
{
    fast_cache_t *fct = (fast_cache_t *) fct_parm;

    if ((!idb) || (!fct)) {
        if (((peerentry *) fct->peer_handle)->debug_fast_pak)
            buginf("\nDLSw: dlsw_fct_enable() idb %x fct %x", idb, fct);
        return;
    }

    if (idb->status & (IDB_MCI | IDB_CBUS))
        fct->fast_hes_wan2lan = dlsw_mci_output;

    /* Clear the other variant for sanity sake. */
    fct->fast_wan2lan = NULL;
}


static void dlsw_fct_disable (void *fct_parm)
{
    fast_cache_t *fct = (fast_cache_t *) fct_parm;

    if (fct)
        fct->fast_hes_wan2lan = NULL;
}


static void dlsw_init_fast_peer (void *pptr)
{
    peerentry *peer = (peerentry *) pptr;

    if (peer->xport_type == DLSW_IF) {
        if (peer->outidb->status & (IDB_MCI | IDB_CBUS))
            peer->fast_hes_lan2wan = dlsw_lan2mcidir;
        else
            peer->fast_hes_lan2wan = NULL;
    } else if (peer->xport_type == DLSW_FR) {
        if (peer->outidb->status & (IDB_MCI | IDB_CBUS))
            peer->fast_hes_lan2wan = dlsw_lan2mcifrdir;
        else
            peer->fast_hes_lan2wan = NULL;
    } else if (peer->xport_type == DLSW_IF_LAN) {
        peer->fast_hes_lan2wan = NULL;
    } else if (peer->xport_type == DLSW_FST) {
        peer->fast_hes_lan2wan = dlsw_lan2fst;
    } else
        peer->fast_hes_lan2wan = NULL;

    /* Clear the other two variants for sanity sake. */
    peer->fast_les_lan2wan = NULL;
    peer->fast_lan2wan = NULL;
}


static void dlsw_fast_switch_init (void)
{
    reg_add_dlsw_enable_fs(dlsw_fct_enable, "dlsw_fct_enable");
    reg_add_dlsw_disable_fs(dlsw_fct_disable, "dlsw_fct_disable");
    reg_add_dlsw_init_fast_peer(dlsw_init_fast_peer, "dlsw_init_fast_peer");
}

static void mci_fdlsw_init (subsystype* subsys)
{
    dlsw_fast_switch_init();
    reg_add_dlsw_srb_mci_input(srb_mci_to_dlsw, "srb_mci_to_dlsw");
    reg_add_mci_dlsw_fst_forward(dlsw_mci_fst2lan, "dlsw_mci_fst2lan");
    reg_add_mci_dlsw_input(dlsw_mci_dir2lan, "dlsw_mci_dir2lan");
    reg_add_framerelay_fs(RXTYPE_DLSW,
                              (service_framerelay_fs_type) dlsw_mci_dir2lan,
                              "dlsw_mci_dir2lan");
}


/*
 * Fast DLSw subsystem headers
 */

#define FDLSW_CBUS_MAJVERSION 1
#define FDLSW_CBUS_MINVERSION 0
#define FDLSW_CBUS_EDITVERSION 1

SUBSYS_HEADER(mci_fdlsw, FDLSW_CBUS_MAJVERSION,
              FDLSW_CBUS_MINVERSION, FDLSW_CBUS_EDITVERSION,
              mci_fdlsw_init, SUBSYS_CLASS_PROTOCOL,
              "seq: srb",
              "req: fastswitch, srb mci_fsrb");


