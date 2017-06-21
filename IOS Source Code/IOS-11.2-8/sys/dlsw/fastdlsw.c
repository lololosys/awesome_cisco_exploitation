/* $Id: fastdlsw.c,v 3.6.6.13 1996/08/28 12:41:28 thille Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/fastdlsw.c,v $
 *------------------------------------------------------------------
 * Fast switching routines for DLSw+ - platform independent.
 *
 * September 1995, Kushal A. Patel
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fastdlsw.c,v $
 * Revision 3.6.6.13  1996/08/28  12:41:28  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.6.12  1996/08/16  04:55:49  fbordona
 * CSCdi66239:  DLSw: RSP fast switching to CIP causes crash at address 0x0
 *              Initialize fast_wan2lan vector for CIP LANs. Add check to
 *              prevent crash for unsupported media tyypes.
 * Branch: California_branch
 *
 * Revision 3.6.6.11  1996/08/09  17:30:26  fbordona
 * CSCdi65603:  DLSw: SYS-2-LINKED: Bad enqueue of 60AE6FC0 in queue
 *              Allow DLSw FST over CIP LAN interface.
 * Branch: California_branch
 *
 * Revision 3.6.6.10  1996/07/23  18:44:36  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.6.6.9  1996/07/11  14:17:16  fbordona
 * CSCdi62521:  DLSw circuit does not connect using FST over ATM peer
 *              Remove DLSw FST hooks from platform/media specific drivers.
 * Branch: California_branch
 *
 * Revision 3.6.6.8  1996/07/09  05:49:23  ppearce
 * CSCdi59527:  ascii string in inline wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.6.6.7  1996/06/28  15:37:39  fbordona
 * CSCdi61269:  25kb -- router crashed, msg = Local Timeout
 *              Traffic shaping depends on pak->if_output.
 * Branch: California_branch
 *
 * Revision 3.6.6.6  1996/06/12  00:14:47  fbordona
 * CSCdi60152:  DLSw merge of low-end and RSP fast switch code for
 *              modularity. Fix misaligned access in dlsw_wan2lan_common().
 * Branch: California_branch
 *
 * Revision 3.6.6.5  1996/05/24  15:11:57  fbordona
 * CSCdi58465:  misalignment & Spurious Accesses at dlsw_nim_fst2
 * Branch: California_branch
 *
 * Revision 3.6.6.4  1996/05/22  14:43:56  fbordona
 * CSCdi57997:  DLSw FST over WAN Frame Relay can not establish circuits
 *              Use reg_invoke_framerelay_fs() for frame-relay RXTYPE_DLSW
 *              Remove DLSw FST frame-relay code from interrupt routines.
 * Branch: California_branch
 *
 * Revision 3.6.6.3  1996/05/17  10:46:42  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.2.6  1996/05/10  15:13:38  fbordona
 * CSCdi57207:  DLSw FST broken over WAN token ring and FDDI
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.2.5  1996/05/05  22:55:19  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.5.2.4  1996/04/26  15:17:21  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.2.3  1996/04/19  20:25:49  fbordona
 * CSCdi55172:  DLSw FST peer-on-demand pass data before peer is connected
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.2.2  1996/04/03  14:02:02  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.2.1  1996/03/17  17:38:23  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.6.6.2  1996/04/17  13:37:06  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.6.6.1  1996/03/18  19:32:32  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.6.4  1996/03/16  06:39:45  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.6.3  1996/03/13  01:20:09  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.6.2  1996/03/07  08:45:02  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.6.1  1996/02/20  00:46:44  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/03/05  19:03:40  fbordona
 * CSCdi50574:  DLSw pod stuck in connect status.
 *              Don't foregt RSP.
 *
 * Revision 3.5  1996/02/23  17:32:43  hampton
 * Clean up platform specific DLSW defines in commonly included header
 * files.  [CSCdi48844]
 *
 * Revision 3.4  1996/02/01  06:01:48  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1996/01/11  22:23:44  kpatel
 * CSCdi46459:  Serial : Input counters dont increment for FST
 *
 * Revision 3.2  1995/11/22  22:16:36  kpatel
 * CSCdi44356:  DLSw+ RSP Fast Switching broken
 *
 * Revision 3.1  1995/11/09  11:25:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/27  17:10:45  kpatel
 * Placeholder for new file
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* 
 * Fast switching routines for DLSw+. These routines are currently
 * being used only on RSP and c7200 platforms until the low end
 * migrates to this code.
 *
 * Note: The c7200 platform supports particle based packets, but for
 *       DLSw currently only the FST path has supports for particles.
 *       The reason for this is that FST gets called from the IP
 *       fastswitching path which is particle based.  It doesn't make
 *       sense to add support for the rest until SRB supports particles.
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "packet.h"
#include "config.h"
#include "../ui/debug.h"
#include "../h/ttysrv.h"
#include "../wan/serial.h"
#include "../if/ether.h"
#include "../if/tring_common.h"
#include "../if/fddi.h"
#include "../if/network.h"
#include "logger.h"
#include "../dlsw/msg_dlsw_peer.c"	/* Not a typo, see logger.h */
#include "../srt/srb.h"
#include "../srt/srb_multiring.h"
#include "../srt/rsrb.h"
#include "../srt/srt_debug.h"
#include "../ip/ip.h"
#include "../ip/ipfast.h"
#include "../ibm/netbios.h"
#include "../if/rif.h"
#include "../if/rif_inline.h"
#include "../llc2/llc2.h"
#include "../dlsw/parser_defs_dlsw.h"
#include "../dlsw/dlsw_dlx.h"
#include "../dlsw/dlsw_ssp.h"
#include "../dlsw/dlsw_peer_externs.h"
#include "../dlsw/dlsw_peer.h"
#include "../dlsw/dlsw_debug.h"
#include "../os/pool.h" 
#include "../os/particle.h" 
#include "../os/buffers_inline.h" 
#include "../wan/smds.h"

/*
 * File Scope Function Prototypes
 */
static int dlsw_nim_to_fst (peerentry *, fast_cache_t *, paktype *,
                            ipcache_rntype *, hwidbtype *);
static int dlsw_srb_to_fst (peerentry *, fast_cache_t *, paktype *,
                              ipcache_rntype *, hwidbtype *);
static int dlsw_lan2fst (hwidbtype *, fast_cache_t *, paktype *);
static int dlsw_lan2dir (hwidbtype *, fast_cache_t *, paktype *);
static int dlsw_lan2frdir (hwidbtype *, fast_cache_t *, paktype *);
static boolean dlsw_wan2lan_common (hwidbtype *, paktype *, 
                                    dlx_fast_hdr_t *, ulong);


/**********************************************************************
 *
 *                     FAST FST DLSw OUTPUT
 *
 * This set of routines supports the fst encapsulation of DLSw packets
 * over the various types of interfaces.  This is the output (token ring
 * -> dlsw) path only.  The input path is elsewhere. 
 *
 **********************************************************************/
#define DLSW_FST_WRITE_TOKEN2_HEADER(data, cptr, rifptr, fst_hdr,       \
    size, fct)                                               \
{                                                                       \
    ushort *rif;                                                        \
    int i;                                                              \
    *data++ = (AC_PRI4 << 8) | FC_TYPE_LLC;                             \
    *data++ = cptr->m.macshort[0];                                      \
    *data++ = cptr->m.macshort[1];                                      \
    *data++ = cptr->m.macshort[2];                                      \
    if (rifptr && rifptr->length)                                       \
        *data++ = cptr->m.macshort[3] | (TR_RII << 8);                  \
    else                                                                \
        *data++ = cptr->m.macshort[3];                                  \
    *data++ = cptr->m.macshort[4];                                      \
    *data++ = cptr->m.macshort[5];                                      \
    if (rifptr && rifptr->length) {                                     \
        for (i=0, rif=(ushort *)rifptr->rif; i < rifptr->length; i+=2)  \
            *data++ = *rif++;                                           \
    }                                                                   \
    PUTLONG(((ulong *)data)++, ((SNAPSNAP << 16) | ((LLC1_UI << 8) | 0)));\
    PUTLONG(((ulong *)data)++, TYPE_IP10MB);                            \
    PUTLONG(((ulong *)data)++, fst_hdr[0]);                             \
    PUTLONG(((ulong *)data)++, fst_hdr[1]);                             \
    PUTLONG(((ulong *)data)++, fst_hdr[2]);                             \
    PUTLONG(((ulong *)data)++, fst_hdr[3]);                             \
    PUTLONG(((ulong *)data)++, fst_hdr[4]);                             \
    DLSW_HDR(data, fct, size);                               \
}


/**********************************************************************
 *
 *                     FAST DIRECT DLSw OUTPUT
 *
 * This set of routines supports the direct encapsulation of DLSw packets
 * over the various types of interfaces.  This is the output (token ring
 * -> dlsw) path only.  The input path is elsewhere. 
 *
 **********************************************************************/

#define DLSW_WRITE_SERIAL_HEADER(data)                                    \
    PUTLONG(((ulong *)data)++, HDLC_DLSW);

inline static ushort *srb_write_fddi_header(ushort *data,
                                            uchar *dst,
                                            uchar *src)
{
#define cp_byte  *((uchar *)data)++

    cp_byte  = FDDI_ASYNCHRONOUS_FRAME     /* fc */
        | FDDI_48BIT_ADDRESS
        | FDDI_LLC_FRAME
        | FDDI_PRIORITY_ZERO;

    cp_byte = bit_swaps[*dst++];          /* dst */
    cp_byte = bit_swaps[*dst++];
    cp_byte = bit_swaps[*dst++];
    cp_byte = bit_swaps[*dst++];
    cp_byte = bit_swaps[*dst++];
    cp_byte = bit_swaps[*dst++];

    cp_byte = bit_swaps[*src++];          /* src */
    cp_byte = bit_swaps[*src++];
    cp_byte = bit_swaps[*src++];
    cp_byte = bit_swaps[*src++];
    cp_byte = bit_swaps[*src++];
    cp_byte = bit_swaps[*src++];
    PUTLONG(((ulong *)data)++, ((SNAPSNAP << 16) | 0x0300));
    PUTLONG(((ulong *)data)++, TYPE_DLSW);
    return(data);
}

#define DLSW_HDR(data, fct, dgsize)                                       \
{                                                                         \
    PUTLONG(((ulong *)data)++, (DLX_FST_VERS_AND_OP | dgsize));           \
    PUTLONG(((ulong *)data)++, (fct->remote_id));                         \
    PUTLONG(((ulong *)data)++, (fct->local_id));                          \
}

#define FST_WRITE_ETHER_HEADER(data, cptr, fst_hdr)     \
{                                                       \
    PUTLONG(((ulong *)data)++, cptr->m.maclong[0]);     \
    PUTLONG(((ulong *)data)++, cptr->m.maclong[1]);     \
    PUTLONG(((ulong *)data)++, cptr->m.maclong[2]);     \
    PUTSHORT(data++, TYPE_IP10MB);                      \
    PUTLONG(((ulong *)data)++, fst_hdr[0]);             \
    PUTLONG(((ulong *)data)++, fst_hdr[1]);             \
    PUTLONG(((ulong *)data)++, fst_hdr[2]);             \
    PUTLONG(((ulong *)data)++, fst_hdr[3]);             \
    PUTLONG(((ulong *)data)++, fst_hdr[4]);             \
}

#define FST_WRITE_SERIAL_HEADER(data, cptr, fst_hdr)    \
{                                                       \
    PUTLONG(((ulong *)data)++, ((cptr->m.macshort[0] << 16) | TYPE_IP10MB)); \
    PUTLONG(((ulong *)data)++, fst_hdr[0]);             \
    PUTLONG(((ulong *)data)++, fst_hdr[1]);             \
    PUTLONG(((ulong *)data)++, fst_hdr[2]);             \
    PUTLONG(((ulong *)data)++, fst_hdr[3]);             \
    PUTLONG(((ulong *)data)++, fst_hdr[4]);             \
}

static inline ushort *fst_write_header (ushort *data,
                                        ipcache_rntype *cptr,
                                        ulong *fst_hdr)
{
    ushort i;

    for (i = 0; i < cptr->length/2; i++) {
        PUTSHORT(((ushort *)data)++, cptr->m.macshort[i]);
    }

    PUTLONG(((ulong *)data)++, fst_hdr[0]);
    PUTLONG(((ulong *)data)++, fst_hdr[1]);
    PUTLONG(((ulong *)data)++, fst_hdr[2]);
    PUTLONG(((ulong *)data)++, fst_hdr[3]);
    PUTLONG(((ulong *)data)++, fst_hdr[4]);
    return(data);
}

static inline ushort *fst_write_fddi_header (ushort *data,
                                             ipcache_rntype *cptr,
                                             ulong *fst_hdr)
{
    ushort i;

    *((uchar *)data)++ = FDDI_ASYNCHRONOUS_FRAME     /* fc */
        | FDDI_48BIT_ADDRESS
            | FDDI_LLC_FRAME
                | FDDI_PRIORITY_ZERO;

    for (i = 0; i < cptr->length/2; i++) {
	PUTSHORT(((ushort *)data)++, cptr->m.macshort[i]);
    }

    PUTLONG(((ulong *)data)++, fst_hdr[0]);
    PUTLONG(((ulong *)data)++, fst_hdr[1]);
    PUTLONG(((ulong *)data)++, fst_hdr[2]);
    PUTLONG(((ulong *)data)++, fst_hdr[3]);
    PUTLONG(((ulong *)data)++, fst_hdr[4]);
    return(data);
}


static inline ushort *fst_write_smds_header (paktype *pak,
                                             ipcache_rntype *cptr,
                                             ulong *fst_hdr)
{
    ulong *macptr, *cache_macptr, *padptr;
    smds_pdb_t *spd;
    smdstype *sm;
    ushort pad, *data;

    /*
     * Write smds header from ip fast cache and DLSw fast header.
     */
    data = fst_write_header((ushort *) pak->datagramstart, cptr, fst_hdr);

    /*
     * Then adjust smds header for this packet...
     */
    spd = cptr->idb->hwptr->smds_pdb;
    sm = (smdstype *)(pak->datagramstart + spd->fast_output_modelen);
    pad = 3 - ((pak->datagramsize - cptr->length + 3) % 4);
    sm->smds_xhlpi &= ~3;
    sm->smds_xhlpi |= pad;
    sm->smds_betag = cptr->idb->hwptr->smds_begin_tag++;
    sm->smds_basize = pak->datagramsize - (4 + spd->fast_output_modelen) + pad;

    /*
     * ...and add smds trailer...yuk!
     */
    padptr = (ulong *)((uchar *)pak->datagramstart + pak->datagramsize);
    PUTLONG (padptr, 0);
    pak->datagramsize += (4 + pad);
    cache_macptr = (ulong *)&sm->smds_rsvd;
    macptr = (ulong *)((uchar *)pak->datagramstart + pak->datagramsize - 4);
    PUTLONG(macptr, GETLONG(cache_macptr));
    return(data);
}


static inline void srb_rx_counters (hwidbtype *idb, int length)
{
    GET_TIMESTAMP(idb->lastinput);
    idb->counters.inputs_fast[ACCT_PROTO_SRB]++; 
    idb->counters.rx_cumbytes_fast[ACCT_PROTO_SRB] += length;
}

/**********************************************************************
 *
 * DLSw token ring to FST
 *
 **********************************************************************/


/*
 * d l s w _ s r b _ i n p u t ()
 *
 * Token ring to virtual ring transport function.
 * Returns TRUE if DLSw can fast switch the lan packet to a remote
 * peer, FALSE otherwise.
 *
 * This function does not have to concern itself with local bridging
 * and since all filtering is performed at process level, this functions
 * does not have to consider
 *  -- LAN input access list filters (since session is established)
 *  -- WAN output filtering (since session is established)
 *
 * srb_rx_counters are incremented by fs_srb_to_vring() before
 * calling this function.
 */

boolean dlsw_srb_input (hwidbtype *idb, paktype *pak)
{
    trrif_t *trh = (trrif_t *) pak->datagramstart;
    fast_cache_t *fct;
    char buff[DLSW_MAX_PEER_ID];
    int rc;
    int riflen;
    sap_hdr *saph_ptr;
    uchar control;

    riflen = pak->riflen;

    /*
     * Does the DLSw fast switch cache have an entry for this
     * packet (base on mac/sap pairs). If so, then this packet can
     * be sent to a remote peer via fst or direct encaps.
     * Pass pointers to the circuit entry lookup function so no
     * data copying is necessary.
     */
    fct = dlsw_tr2fct(trh->daddr, (ushort *) (&trh->rc_blth + riflen));
    if (!fct) {
        return(FALSE);
    } else if (((peerentry *) fct->peer_handle)->conn_state != PEER_CONN) {
        if (((peerentry *) fct->peer_handle)->debug_fast_err)
            buginf("\nDLSw: dlsw_srb_input: peer not yet connected");
        return(FALSE);
    }

    /*
     * Do not fast switch NULL XID P, SABME, NetBIOS NQ or NR.
     * This allows CSM to learn new reachability info.
     * Give CSM a chance to filter/firewall NetBIOS UI frames.
     */ 
    saph_ptr = (sap_hdr *) (&trh->rc_blth + riflen);
    control = saph_ptr->control & ~P_BIT;
    if ((control == SABME) || (control == LLC1_XID) ||
        (control == LLC1_TEST)) {
        return(FALSE);
    } else if ((saph_ptr->dsap == SAP_NETBIOS) && (control == LLC1_UI)) {
        return(FALSE);
    }

    /*
     * This may not be entirely necessary but play it safe. For dlsw
     * signal that packet data beyond network hdr is potentially
     * in CPU's cache and needs flushed (used on RSP).
     */
    pak_set_dirty(pak);

    if (((peerentry *) fct->peer_handle)->fast_lan2wan == NULL) {
        /*
         * Chances are the peer-on-demand-defaults are not in sync
         * on the two peers. Do not pop this packet up to the core
         * since the session will never come up.
         */
        if (((peerentry *) fct->peer_handle)->debug) {
            buginf("\nDLSw: %s: fast switching not enabled for %s",
                   idb->hw_namestring,
                   peer_str((peerentry *) fct->peer_handle, buff));
        }
        datagram_done(pak);
        return(TRUE);
    }

    /*
     * Fast switch the packet. 
     * Since DLSw must alter the token ring packet by inserting a new
     * rif, the Frame Check Sequence is useless, so remove it from
     * the packet.
     *
     * fast_lan2wan vectored functions consume pak.
     * They ruturn TRUE is pak was fast switched, FALSE otherwise.
     */
    rc = (*((peerentry *)fct->peer_handle)->fast_lan2wan)(idb, fct, pak);

    if (rc == TRUE) {
        ((peerentry *) fct->peer_handle)->pkts_tx++;
        if (((peerentry *) fct->peer_handle)->debug_fast_pak)
            buginf("\nDLSw: dlsw_srb_input() %s to %s\n",
                   idb->hw_namestring,
                   peer_str((peerentry *) fct->peer_handle, buff));
    } else {
        ((peerentry *) fct->peer_handle)->drops++;
        if (((peerentry *) fct->peer_handle)->debug_fast_err ||
            ((peerentry *) fct->peer_handle)->debug_fast_err) 
            buginf("\nDLSw: dlsw_srb_input(): dropped - %s to %s",
                   idb->hw_namestring,
                   peer_str((peerentry *) fct->peer_handle, buff));
    }

    return(TRUE);
}


/*
 * dlsw_lan2fst()
 *
 * Fast switch token ring packet to a dlsw FST remote peer using
 * IP encapsulation.
 *
 * Called by dlsw_srb_input() with peer->fast_lan2wan vector.
 *
 * Fast switches token ring packets out to an FST peer by calling
 * dlsw_nim_to_fst() or dlsw_srb_to_fst().
 */

static boolean dlsw_lan2fst (hwidbtype *input, fast_cache_t *fct,
                                                    paktype *pak)
{
    peerentry *peer = (peerentry *) fct->peer_handle;
    hwidbtype *idb;
    ipcache_rntype *cptr;
    dlx_fast_hdr_t *fast_hdr;
    iphdrtype *iphead;
    ulong totsize;
    paktype *newpak;

    /*
     * Lookup ip cache to see if we have valid entry.
     * If found, update it's last used field since ipcache_lookup_fast()
     * doesn't do this for us.
     */
    cptr = ipcache_lookup_fast(peer->ipaddr);
    if (cptr) {
        idb = cptr->idb->hwptr;

        if (idb->status & IDB_TR)
            return(dlsw_srb_to_fst(peer, fct, pak, cptr, input));
        else
            return(dlsw_nim_to_fst(peer, fct, pak, cptr, input));
    } else {
        /*
         * Since fct is valid for the macs/saps, the ip fast cache entry 
         * has timed out, send this one to ip input. It will repopulate the 
         * ip fast cache.
         *
         * Must use ip_simple_enqueue() because the ip fast cache
         * can only be populated at process level :-( Using ip_write()
         * would have been the better choice if we were at process level.
         */
        PDEBUG_FPAK("\ndlsw_lan2fst(): ip cache MISS for %i", peer->ipaddr);

        totsize = IPHEADERBYTES(NOPT) + sizeof(dlx_fast_hdr_t) 
                                              + pak->datagramsize;
        /*
	 * There are several reasons why we may have to dupe the packet :
	 *
         * For RSP platform, can not queue fast switch packets. RSP fast
         * switch packets will already be in a list. If packet is already in 
         * list, dupe it before sending it to ip. Additionally, we must return
         * the original fast switch pak to the buffer pool.
	 *
	 * Header must be long word aligned. A misaligned ip header causes
	 * major alignment errors in ip code. This pak was originally an srb
	 * header with a 3 byte sap header
         */
	if (pak->next || (((ulong) pak->datagramstart) & 0x03)) {
            newpak = input_getbuffer(totsize, input);
            if (!newpak) {
                errmsg(PEER_NOMEM, "get fst packet due to ip cache miss");
                datagram_done(pak);
                return(FALSE);
            }
            bcopy(pak->datagramstart, newpak->datagramstart, pak->datagramsize);
            newpak->if_input = pak->if_input;
            datagram_done(pak);
            pak = newpak;
        }

        pak->datagramsize = totsize;
        pak->linktype = LINK_IP;
        pak->desthost = peer->ipaddr;
        /*
         * Copy in the ip header from the peer structure
         * Even though pak is being sent through ip, it still must be
         * sequenced.
         */
        pak->datagramstart -= (IPHEADERBYTES(NOPT) + sizeof(dlx_fast_hdr_t));
        pak->network_start = pak->datagramstart;
        iphead = (iphdrtype *)pak->datagramstart; 
        bcopy((uchar *)peer->ip_header, (uchar *)iphead, IPHEADERBYTES(NOPT));
        dlsw_fst_incr_send(peer, &iphead->id);

        iphead->tl = pak->datagramsize;
        iphead->checksum = 0;                       /* clear checksum field */
        iphead->checksum = ipcrc((ushort *) iphead, ltob(iphead->ihl));

        /* 
         * Write Fast DLX header and original end station packet.
         * Leave room for ip header.
         */
        fast_hdr = (dlx_fast_hdr_t *) 
                   (pak->datagramstart + IPHEADERBYTES(NOPT));
        fast_hdr->packet_length = totsize - IPHEADERBYTES(NOPT);
        fast_hdr->proto_ver_id = DLX_PROTO_VERSION;
        fast_hdr->message_type = DLX_FAST_PASS;
        fast_hdr->session_id = fct->remote_id;
        fast_hdr->src_session_id = fct->local_id;
        /*
         * Send it to ip for forwarding.
         * This will populate the ip fast cache.
         */
        PDEBUG_FPAK("\nDLSw: dlsw_lan2fst(): to %i via ip - size %d",
               pak->desthost, pak->datagramsize);
        ip_simple_enqueue(pak);
        return(TRUE);
    }
}


/*
 * d l s w _ n i m _ t o _ f s t ()
 *
 * Fast switch SRB packet to dlsw peer using IP encapsulation.
 * Called from dlsw_lan2fst().
 *
 * Return TRUE if packet is fast switched, FALSE otherwise.
 * In either case, this function consumes pak.
 */

static boolean dlsw_nim_to_fst (peerentry *peer, fast_cache_t *fct,
                                paktype *pak, ipcache_rntype *cptr, 
                                                   hwidbtype *input)
{
    hwidbtype *oidb;
    ulong oidb_status;
    iphdrtype *iphead;
    ushort *data;
    int maxmtu;
    ulong payload_size;

    oidb = cptr->idb->hwptr;
    oidb_status = oidb->status;
    payload_size = pak->datagramsize;
    pak->datagramsize += (FST_IP_HDR_SIZE + sizeof(dlx_fast_hdr_t));

    if (oidb_status & IDB_ATM) {
        maxmtu = reg_invoke_media_atm_mtu(LINK_IP, cptr->idb, FALSE);
        if (maxmtu == 0)
            maxmtu = cptr->idb->sub_mtu;
    } else
        maxmtu = oidb->maxmtu;

    if (pak->datagramsize  > maxmtu) {
        PDEBUG_FERR("\nDLSw: pak size %d > mtu %d for FST out %s",
                    pak->datagramsize, maxmtu, oidb->hw_namestring);
        datagram_done(pak);
        return(FALSE);
    }

    /*
     * Now do necessary modifications to ip/udp header.
     */
    iphead = peer->ip_header;
    dlsw_fst_incr_send(peer, &iphead->id);
    iphead->tl = pak->datagramsize ;
    iphead->checksum = 0;
    iphead->checksum = ipcrc((ushort *) iphead, ltob(iphead->ihl));

    /*
     * Set up write pointer to base of destination transmit window.
     * Do some input accounting.  Figure out how to write new MAC header,
     * write new mac header and IP header, and copy the received
     * datagram after that.
     */
    if (oidb_status & IDB_ETHER) {
        pak->datagramsize += ETHER_ARPA_ENCAPBYTES;
	data = (ushort *)(pak->datagramstart - 
            (ETHER_ARPA_ENCAPBYTES + FST_IP_HDR_SIZE + sizeof(dlx_fast_hdr_t)));
        pak->datagramstart = (uchar *)data;
        FST_WRITE_ETHER_HEADER(data, cptr, ((ulong *)iphead));

    } else if (oidb_status & IDB_SERIAL) {
        if (oidb->enctype == ET_HDLC) {
            pak->datagramsize += HDLC_HEADER_SIZE;
            data = (ushort *)(pak->datagramstart - 
                (HDLC_HEADER_SIZE + FST_IP_HDR_SIZE + sizeof(dlx_fast_hdr_t)));
            pak->datagramstart = (uchar *)data;
            FST_WRITE_SERIAL_HEADER(data, cptr, ((ulong *)iphead));
        } else if ((oidb->enctype == ET_FRAME_RELAY) ||
                   (oidb->enctype == ET_ATM_DXI) ||
                   (oidb->enctype == ET_PPP)) {
            pak->datagramsize += cptr->length;
            data = (ushort *)(pak->datagramstart - 
                (cptr->length + FST_IP_HDR_SIZE + sizeof(dlx_fast_hdr_t)));
            pak->datagramstart = (uchar *)data;
            data = fst_write_header(data, cptr, ((ulong *)iphead));
        } else if (oidb->enctype == ET_SMDS) {
            pak->datagramsize += cptr->length;
            pak->datagramstart -= (FST_IP_HDR_SIZE + sizeof(dlx_fast_hdr_t) +
                                   cptr->length);
            /*
             * fst_write_smds_header() changes the size of the packet
             * when it adds the smds trailer.
             */
            data = fst_write_smds_header(pak, cptr, ((ulong *)iphead));
        } else {
            datagram_done(pak);
            return(FALSE);
        }

    } else if (oidb_status & IDB_FDDI) {
        pak->datagramsize += FDDI_SNAP_ENCAPBYTES;
	data = (ushort *)(pak->datagramstart - 
            (FDDI_SNAP_ENCAPBYTES + FST_IP_HDR_SIZE + sizeof(dlx_fast_hdr_t)));
        pak->datagramstart = (uchar *)data;
        data = fst_write_fddi_header(data, cptr, (ulong *)iphead);

    } else if (oidb_status & IDB_ATM) {
        pak->atm_vcid = cptr->m.macshort[0];
        pak->atm_enctype = cptr->m.macshort[1];
        pak->datagramsize += cptr->length;
        data = (ushort *)(pak->datagramstart -
            (cptr->length + FST_IP_HDR_SIZE + sizeof(dlx_fast_hdr_t)));
        pak->datagramstart = (uchar *)data;
        data = fst_write_header(data, cptr, (ulong *)iphead);

    } else {
        datagram_done(pak);
        return(FALSE);
    }

    DLSW_HDR(data, fct, (payload_size + sizeof(dlx_fast_hdr_t)));
    pak->if_output = cptr->idb;
    (*oidb->fastsend)(oidb, pak);
    return(TRUE);
}


/*
 * d l s w  _ s r b _ t o _ f s t ()
 *
 * Fast switch SRB packet to dlsw peer using IP encapsulation
 * over a WAN token ring.
 *
 * Return TRUE if packet is fast switched, FALSE otherwise.
 * In either case, this function consumes pak.
 */

static boolean dlsw_srb_to_fst (peerentry *peer, fast_cache_t *fct,
                                paktype *pak, ipcache_rntype *cptr, 
                                hwidbtype *input)
{
    hwidbtype *oidb;
    ushort *odata;
    riftype *rifptr = NULL;
    int hdrsize;
    int totalsize;
    iphdrtype *iphead;

    oidb = cptr->idb->hwptr;
    hdrsize = TRING_SNAP_ENCAPBYTES + FST_IP_HDR_SIZE + sizeof(dlx_fast_hdr_t);

    if (srb_multiring_any_enabled(cptr->idb)) {
	rifptr = get_rif_inline(oidb, cptr->m.macstring);
        if (!rifptr) {
            PDEBUG_FERR("\nDLSw: rif_lookup failed for FST out %s", 
                        oidb->hw_namestring);
            datagram_done(pak);
            return(FALSE);
        }
        hdrsize += rifptr->length;
    }
    totalsize = hdrsize + pak->datagramsize;
    if (totalsize  > oidb->maxmtu) {
        PDEBUG_FERR("\nDLSw: pak size %d > mtu %d for FST out %s",
                    totalsize, oidb->maxmtu, oidb->hw_namestring);
        datagram_done(pak);
        return(FALSE);
    }

    /* Now do necessary modifications to ip header */
    iphead = peer->ip_header;
    dlsw_fst_incr_send(peer, &iphead->id);
    iphead->tl = FST_IP_HDR_SIZE + sizeof(dlx_fast_hdr_t) + pak->datagramsize;
    iphead->checksum = 0;
    iphead->checksum = ipcrc((ushort *) iphead, ltob(iphead->ihl));

    /* set up transmit descriptor, append header to packet */
    pak->datagramstart -= hdrsize;
    odata = (ushort *)pak->datagramstart;
    DLSW_FST_WRITE_TOKEN2_HEADER(odata, cptr, rifptr, ((ulong *)iphead),
                            (pak->datagramsize + sizeof(dlx_fast_hdr_t)), fct);

    /* packet and count include FCS */
    pak->datagramsize = totalsize;
    pak->if_output = cptr->idb;
    (*oidb->fastsend)(oidb, pak);
    return(TRUE);
}


/*
 * f s _ d l s w _ s r b _ o u t p u t ()
 *
 * Forward a token ring frame onto a local token ring.
 * Used for FST and Direct Encaps.
 *  -- Always recompute CRC
 *  -- Access list filtering is not required.
 */

boolean fs_dlsw_srb_output (hwidbtype *idb, fast_cache_t *fct, paktype *pak)
{

    if (!fct->lanhwidb->firstsw) {
        if (((peerentry *)fct->peer_handle)->debug)
            buginf("\nfs_dlsw_srb_output(): No swidb to send to %s%d",
                   fct->lanhwidb->name, fct->lanhwidb->unit);
        datagram_done(pak);
    }

    pak->flags &= ~PAK_SRB_OUT;           /* re-calc checksum */
    pak->flags |= PAK_SRB_EXPLORER;
    pak->if_output = fct->lanhwidb->firstsw;

    (*fct->lanhwidb->fastsend)(fct->lanhwidb, pak); /* consumes pak */
    return(TRUE);
}


/**********************************************************************
 *
 *                     FAST DIRECT DLSw ENCAPS
 *
 **********************************************************************/

/*
 * d l s w _  l a n 2 d i r ()
 *
 * Send a token ring packet out a direct encapsulation serial interface.
 * called by dlsw_srb_input() through peer->fast_lan2wan vector.
 *
 * Return TRUE if packet is fast switched, FALSE otherwise.
 * In either case, this function consumes pak.
 */

static boolean dlsw_lan2dir (hwidbtype *input, fast_cache_t *fct, paktype *pak)
{
    peerentry *peer = (peerentry *) fct->peer_handle;
    int totalsize = 0;
    ushort *data = NULL;

    if ((peer->outidb->status & IDB_SERIAL) &&
        (peer->outidb->enctype == ET_HDLC)) {
        totalsize = pak->datagramsize + HDLC_HEADER_SIZE + sizeof(dlx_fast_hdr_t);
        data = (ushort *)(pak->datagramstart - (HDLC_HEADER_SIZE + 
            sizeof(dlx_fast_hdr_t)));
        pak->datagramstart = (uchar *)data;
        DLSW_WRITE_SERIAL_HEADER(data);

    } else {
        datagram_done(pak);
        return(FALSE);
    }

    if (!peer->outidb->firstsw) {
        if (((peerentry *)fct->peer_handle)->debug)
            buginf("\ndlsw_lan2dir(): No swidb to send to %s%d",
                   peer->outidb->name, peer->outidb->unit);
        datagram_done(pak);
    }

    /*
     * Write the DLSw (dlx protocol) header and original tr pak
     * payload and send the packet.
     */
    DLSW_HDR(data, fct, (pak->datagramsize + sizeof(dlx_fast_hdr_t)));
    pak->datagramsize = totalsize;
    pak->if_output = peer->outidb->firstsw;
    (*peer->outidb->fastsend)(peer->outidb, pak);
    return(TRUE);
}

/*
 * d l s w _  l a n 2 f r d i r ()
 *
 * Send a token ring packet out a FR direct encapsulation serial interface.
 * called by dlsw_srb_input() through peer->fast_lan2wan vector.
 *
 * Return TRUE if packet is fast switched, FALSE otherwise.
 * In either case, this function consumes pak.
 */

static int dlsw_lan2frdir (hwidbtype *idb, fast_cache_t *fct, paktype *pak)
{
    peerentry *peer = (peerentry *) fct->peer_handle;
    int totalsize = 0;
    char buff[DLSW_MAX_PEER_ID];
    ushort fr_dlci;
    dlx_fast_hdr_t *dlxh_p;

    if ((peer->outidb->status & IDB_SERIAL) &&
        (peer->outidb->enctype == ET_FRAME_RELAY)) {
        totalsize = pak->datagramsize + sizeof(dlx_fast_hdr_t);

        /*
         * Write the DLSw (dlx protocol) header and original tr pak
         * payload and send the packet.
         */
        pak->datagramstart -= sizeof(dlx_fast_hdr_t);
        dlxh_p = (dlx_fast_hdr_t *) pak->datagramstart; 
        dlxh_p -> proto_ver_id = DLX_PROTO_VERSION;
        dlxh_p -> message_type = DLX_FAST_PASS;
        dlxh_p -> packet_length = totalsize;
        dlxh_p -> session_id = fct->remote_id;
        dlxh_p -> src_session_id = fct->local_id;

        pak->datagramsize = totalsize;

        pak->linktype = LINK_DLSW;
        pak->acct_proto = ACCT_PROTO_SRB;
        pak->if_output = peer->outidb->firstsw;
        fr_dlci = peer->fr_dlci;
        if ((peer->outidb->vencap) &&
            (*peer->outidb->vencap)(pak, (ulong) fr_dlci)) {
                (*peer->outidb->fastsend)(peer->outidb, pak);
        } else {
                PDEBUG_FERR("\nDLSW Direct vencap failed - drop pak for %s",
                            peer_str(peer, buff));
                datagram_done(pak);
            return(FALSE);
        }
        return(TRUE);
    } else {
        datagram_done(pak);
        return(FALSE);
}
}


/*
 * d l s w _ n i m _ d i r 2 l a n ()
 *
 * Receive a DLSw packet from a serial nim, and forward the
 * extracted frame out onto a token ring.
 */

boolean dlsw_nim_dir2lan (hwidbtype *idb, paktype *pak)
{
    dlx_fast_hdr_t *dlxh;

    /*
     * Find start of DLX header.
     */
    if ((idb->status & IDB_SERIAL) &&  (idb->enctype == ET_HDLC)) {
	dlxh = (dlx_fast_hdr_t *)&(pak->datagramstart[HDLC_ENCAPBYTES]);
    } else if ((idb->status & IDB_SERIAL) &&
              (idb->enctype == ET_FRAME_RELAY)) {
        dlxh = (dlx_fast_hdr_t *)
                &(pak->datagramstart[pak->encsize]);
    } else {
	return(FALSE);
    }

    return(dlsw_wan2lan_common(idb, pak, dlxh, 0));
}

/*
 * dlsw_nim_fst2lan
 * 
 * Used to forward FST datagrams on to a local token Ring.
 */

boolean dlsw_nim_fst2lan (hwidbtype *in_idb, paktype *pak)
{
    iphdrtype *iph;
    dlx_fast_hdr_t *dlxh;

    if (!lpeer)
        return(FALSE);

    /*
     * Assume that IP fastswitching code did the
     * necessary checks on this packets.
     * 1) IP Checksum is correct.
     * 2) This packet is not an IP fragment.
     * 3) There are no IP options.
     *
     * since this is called after trying to fastswitch it as
     * an ip packet, it fell into here.
     */
    if (in_idb->status & IDB_SERIAL) {
	if ((in_idb->enctype == ET_HDLC) ||
	    (in_idb->enctype == ET_FRAME_RELAY) ||
            (in_idb->enctype == ET_ATM_DXI) ||
            (in_idb->enctype == ET_PPP) ||
            (in_idb->enctype == ET_SMDS))
	    iph = (iphdrtype *) pak->network_start;
        else
            return(FALSE);
    } else if (in_idb->status & IDB_ETHER) {
        iph = (iphdrtype *)&(pak->datagramstart[ETHER_ARPA_ENCAPBYTES]);
    } else if ((in_idb->status & IDB_FDDI) ||
               (in_idb->status & IDB_TR) ||
               (in_idb->status & IDB_ATM)) {
        iph = (iphdrtype *) pak->network_start;
    } else {
        return(FALSE);
    }

    if (GETLONG(&iph->dstadr) != lpeer->ipaddr)
        return(FALSE);

    dlxh = (dlx_fast_hdr_t *) (iph + 1);
    return(dlsw_wan2lan_common(in_idb, pak, dlxh, iph->id));
}


/*
 * dlsw_wan2lan_common()
 *
 * Used by FST and Direct to forward datagrams on to a local token Ring.
 * If frame has no dlsw fast switch context, bounc it upstairs.
 * Do not have to consider LAN output access list filters since the
 * context-less get slow switched.
 *
 */

boolean dlsw_wan2lan_common (hwidbtype *in_idb, paktype *pak, 
                             dlx_fast_hdr_t *dlxh, ulong ip_seq_num)
{
    trrif_t *trh;
    peerentry *peer;
    int remote_rif_len;
    int rifdiff;
    ushort *dest, *src;
    fast_cache_t *fct;
    int ii, delta;
    iphdrtype *iph;

    if ((dlxh->message_type != DLX_FAST_PASS) ||
        (GETLONG(&dlxh->src_session_id) == 0))
        return(FALSE);

    /*
     * Is DLSw set-up to fast switch it?
     * Don't fastswitch if the lan interface is not up or the user has
     * turned off fast switching.
     */
    fct = dlsw_id2fct(GETLONG(&dlxh->session_id));
    if ((!fct) || (fct->lanhwidb->state != IDBS_UP) ||
        (fct->fast_wan2lan == NULL) ||
        (((peerentry *) fct->peer_handle)->conn_state != PEER_CONN)) {
        if (fct && 
            ((peerentry *) fct->peer_handle)->debug_fast_err) {
            if (fct->lanhwidb->state != IDBS_UP)
                buginf("\nDLSw: dlsw_wan2lan_common(): IDB state not up");
            else if (fct->fast_wan2lan == NULL)
                buginf("\nDLSw: dlsw_wan2lan_common(): fct not enabled");
            else
                buginf("\nDLSw: dlsw_wan2lan_common(): peer not connected");
        }
        return(FALSE);
    }

    /*
     * This may not be entirely necessary but play it safe. For dlsw
     * signal that packet data beyond network hdr is potentially
     * in CPU's cache and needs flushed (used on RSP).
     */
    pak_set_dirty(pak);

    /*
     * FST uses iph->id for sequence number and it must be checked
     * to keep the FST packet order.
     */
    fct->remote_id = GETLONG(&dlxh->src_session_id);
    peer = (peerentry *) fct->peer_handle;

    if (peer->xport_type == DLSW_FST) {
        /*
         * Due to the fast cache's ability to dynamically change peers
         * per entry, ensure that this packet was sent by the peer
         * described in the fast cache entry.
         */
        iph = (iphdrtype *) pak->network_start;
        if (peer->ipaddr != GETLONG(&iph->srcadr)) {
            PDEBUG_FERR("\nDLSw: peer mismatch: (%d) source %i - cached %i",
                        ip_seq_num, GETLONG(&iph->srcadr), peer->ipaddr);
            datagram_done(pak);
            return(TRUE);
        }

        if (!dlsw_fst_acceptor(peer, ip_seq_num)) {
            peer->drops++;
            datagram_done(pak);
            return(TRUE);
        }
        PDEBUG_FPAK("\nDLSw: peer %i: recv seq# %d dlx size %d - out to %s",
                    peer->ipaddr, ip_seq_num, dlxh->packet_length,
                    fct->lanhwidb->hw_namestring);
    } else {
        PDEBUG_FPAK("\nDLSw: wan2lan: dlx size %d - out to %s",
                    dlxh->packet_length, fct->lanhwidb->hw_namestring);
    }

    /* Do packet accouting prior to modifying the packet. */
    srb_rx_counters(in_idb, pak->datagramsize);

    /*
     * Shift packet for new local rif.
     */
    trh = (trrif_t *)(dlxh + 1);
    pak->datagramsize = dlxh->packet_length - sizeof(dlx_fast_hdr_t);
    remote_rif_len = trh->rc_blth & RC_LTHMASK;
    rifdiff = remote_rif_len - fct->riflen;
    if (rifdiff != 0) {
        src = (ushort *) trh;
        dest = (ushort *) (((uchar *)src) + rifdiff);
        if (rifdiff < 0) {
            for (ii = 0; ii < TR_RC_SHORTS; ii++)
                dest[ii] = src[ii];
        } else {
            for (ii = TR_RC_SHORTS - 1; ii >= 0; ii--)
                dest[ii] = src[ii];
        }
        trh = (trrif_t *) dest;
	delta = (uchar *)trh - (uchar *)pak->datagramstart;
        pak->datagramstart += delta;
        pak->datagramsize -= rifdiff;
    } else {
	delta = (uchar *)trh - (uchar *)pak->datagramstart;
        pak->datagramstart += delta;
    }

    if (pak_has_particles(pak)) {
	/*
	 * Here we are adjusting the particle pointers to reflect the
	 * changes to pak->datagramstart.  We are basically stripping
	 * off the media encap + 20 bytes of IP header + 12
	 * bytes of DLSw header + the delta of the 2 rifs.  Even at
	 * worst case with 84 bytes encap & 18 bytes of RIF, that is
	 * still well within the 512 particle size used by c7200.
	 */
	particle_clip_inline(pak->particlequeue.qhead, -delta);
    }

    /*
     * Copy the local rif into the packet.
     */
    bcopy(fct->rif, &trh->rc_blth, fct->riflen);
    trh->ac = AC_PRI4;

    /*
     * fct->fast_wan2lan points to fs_dlsw_srb_output().
     * fs_dlsw_srb_output() consumes pak.
     */
    peer->pkts_rx++;
    clear_if_input(pak);
    (*fct->fast_wan2lan)(in_idb, fct, pak);
    return(TRUE);
}


/*******************************************************************
 * 
 *       General fast switching admin functions
 *
 *******************************************************************/

void dlsw_fct_enable (hwidbtype *idb, void *fct_parm)
{
    fast_cache_t *fct = (fast_cache_t *) fct_parm;

    if (idb && fct && ((idb->status & IDB_TR) || is_cip_lan(idb))) {
        fct->fast_wan2lan = fs_dlsw_srb_output;
    } else {
        if (dlsw_peer_debug)
            buginf("\ndlsw_fct_enable() not IDB_TR");
        fct->fast_wan2lan = NULL;
    }

    /* Clear the other variant for sanity sake. */
    fct->fast_hes_wan2lan = NULL;
}


void dlsw_fct_disable (void *fct_parm)
{
    fast_cache_t *fct = (fast_cache_t *) fct_parm;

    if (fct)
        fct->fast_wan2lan = NULL;
}


void dlsw_init_fast_peer (void *pptr)
{
    peerentry *peer = (peerentry *) pptr;

    peer->fast_lan2wan = NULL;

    if (peer->xport_type == DLSW_IF) {
        peer->fast_lan2wan = dlsw_lan2dir;
    } else if (peer->xport_type == DLSW_FR) {
        peer->fast_lan2wan = dlsw_lan2frdir;
    } else if (peer->xport_type == DLSW_IF_LAN) {
        /*
         * Not suported. Placeholer if it is ever added.
         */
        peer->fast_lan2wan = NULL;
    } else if (peer->xport_type == DLSW_FST) {
        peer->fast_lan2wan = dlsw_lan2fst;
    } else
        peer->fast_lan2wan = NULL;

    /* Clear the other two variants for sanity sake. */
    peer->fast_hes_lan2wan = NULL;
    peer->fast_les_lan2wan = NULL;
}


static void fs_dlsw_init (subsystype* subsys)
{
    reg_add_dlsw_srb_input((service_dlsw_srb_input_type)
        dlsw_srb_input, "dlsw_srb_input");
    reg_add_nim_dlsw_fst_forward(dlsw_nim_fst2lan, "dlsw_nim_fst2lan");
    reg_add_nim_dlsw_input(dlsw_nim_dir2lan, "dlsw_nim_dir2lan");
    reg_add_dlsw_enable_fs(dlsw_fct_enable, "dlsw_fct_enable");
    reg_add_dlsw_disable_fs(dlsw_fct_disable, "dlsw_fct_disable");
    reg_add_dlsw_init_fast_peer(dlsw_init_fast_peer, "dlsw_init_fast_peer");
}

/*
 * Fast SRB subsystem header
 */

#define FS_DLSW_MAJVERSION 1
#define FS_DLSW_MINVERSION 0
#define FS_DLSW_EDITVERSION 1

/*
 * After ../les/fastsrb_les.c is obsoleted, add fs_srb as a req after srb.
 */
SUBSYS_HEADER(fs_dlsw, 
              FS_DLSW_MAJVERSION, 
              FS_DLSW_MINVERSION, 
              FS_DLSW_EDITVERSION,
              fs_dlsw_init, 
              SUBSYS_CLASS_PROTOCOL,
              "seq: srb",
              "req: fastswitch, srb");

