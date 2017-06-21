/* $Id: dlsw_peer_fst.c,v 3.10.10.11 1996/09/09 19:09:12 akhanna Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_peer_fst.c,v $
 *------------------------------------------------------------------
 * DLSw Peer Transport Routines for FST connected peers 
 *
 * July 1994, Frank Bordonaro 
 *
 * Copyright (c) 1993-1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Create, destroy, send, and receive routines for DLSw peer
 * connections using FST.
 *------------------------------------------------------------------
 * $Log: dlsw_peer_fst.c,v $
 * Revision 3.10.10.11  1996/09/09  19:09:12  akhanna
 * CSCdi65724:  Cannot stop keepalives on promiscous peer
 * Branch: California_branch
 *
 * Revision 3.10.10.10  1996/08/28  12:41:20  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.10.10.9  1996/08/26  15:05:19  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.10.10.8  1996/07/23  13:21:45  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.10.10.7  1996/07/18  20:27:28  ppearce
 * CSCdi63366:  Move SRB counters from hwidb into SRB subblock
 * Branch: California_branch
 *   - Decreases size of hwidb for non-SRB'd interfaces
 *   - Provides additional IOS-independence for SRB
 *   - Moves SRB a little closer to being completely swidb-based
 *
 * Revision 3.10.10.6  1996/07/09  18:08:50  kmoberg
 * CSCdi46177:  access-expression output capabilities needs to be
 *              added to DLSw. Clean up remote peer config.
 * Branch: California_branch
 *
 * Revision 3.10.10.5  1996/06/28  23:05:50  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.10.10.4  1996/06/18  01:43:28  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.10.10.3  1996/05/17  10:46:30  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.10.2.5  1996/05/10  15:13:31  fbordona
 * CSCdi57207:  DLSw FST broken over WAN token ring and FDDI
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.10.2.4  1996/05/05  22:55:17  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.10.2.3  1996/04/26  07:34:47  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.10.2.2  1996/04/20  02:08:30  fbordona
 * CSCdi55177:  DLSw phase I to convert peer to managed timers
 *              Required for scalability of large DLSw peer nets.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.10.2.1  1996/04/03  14:01:55  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.10.10.2  1996/04/17  13:37:04  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.10.10.1  1996/03/18  19:32:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.3  1996/03/16  06:39:32  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.2.2  1996/03/07  08:44:55  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.2.1  1996/02/20  00:46:34  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.10  1996/02/23  17:32:41  hampton
 * Clean up platform specific DLSW defines in commonly included header
 * files.  [CSCdi48844]
 *
 * Revision 3.9  1996/02/08  20:10:28  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.8  1996/02/01  06:01:42  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.7  1996/02/01  02:57:12  fbordona
 * CSCdi47930:  DLSw session does not reconnect after peer powered off.
 *              Only attempt one peer connection at at time to avoid
 *              starving the dlsw background process.
 *
 * Revision 3.6  1996/01/18  22:25:00  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1995/11/29  22:06:38  fbordona
 * CSCdi43961:  DLSw peer mysteriously shows up twice in write term.
 *              Simultaneous priority and border peers cause problems.
 *
 * Revision 3.4  1995/11/29  19:46:33  ravip
 * CSCdi40989:  crash: router tried to reference free memory in dlsw code
 *              use mem_lock() to lock memory, to avoid other routines
 *              from deleting the memory block in use.
 *              Use a flag to tag actions that would change the links.
 *
 * Revision 3.3  1995/11/20  23:25:57  fbordona
 * CSCdi42215:  dlsw backup peers broken
 *
 * Revision 3.2  1995/11/17  09:03:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:24:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  20:55:58  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/10/09  20:18:48  rnaderi
 * CSCdi41641:  Add sub-interface option to the parser for DLSw+/FR
 *
 * Revision 2.4  1995/07/31  18:59:03  ppearce
 * CSCdi37939:  Eliminate use of CLS_P_TYPE_NUMBERS from DLSw+
 *
 * Revision 2.3  1995/07/26  19:12:26  hampton
 * Convert DLSW to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37708]
 *
 * Revision 2.2  1995/06/18 21:21:34  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:27:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "logger.h"
#include "../dlsw/msg_dlsw_peer.c"	/* Not a typo, see logger.h */
#include <ciscolib.h>
#include "interface_generic.h"
#include "fastswitch_registry.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "mgd_timers.h"
#include "address.h"
#include "../cls/dlc_registry.h"
#include "../hes/if_mci.h"
#include "../ibm/netbios.h"
#include "../if/fddi.h"
#include "../if/ether.h"
#include "../if/network.h"
#include "../if/tring.h"
#include "../if/tring_common.h"
#include "../if/rif.h"
#include "../ip/ip.h"
#include "../ip/ipfast.h"
#include "../ip/ip_registry.h"
#include "../dlsw/parser_defs_dlsw.h"
#include "../ui/debug.h"
#include "../srt/srb_core.h"
#include "../dlsw/dlsw_dlx.h"
#include "../dlsw/dlsw_ssp.h"
#include "../dlsw/dlsw_peer_externs.h"
#include "../dlsw/dlsw_csm_externs.h"
#include "../dlsw/dlsw_csm_db.h"
#include "../dlsw/dlsw_csm.h"
#include "../dlsw/dlsw_debug.h"
#include "../dlsw/dlsw_peer.h"


/*
 * Globals
 */
queuetype fast_hash_table[DLSW_FCT_HASH_SIZE];

/*
 * Some shared string consts
 */
const char DLSW_Revcd_seq_d_expected_d[] = 
                      "\nDLSw: Recvd seq# %d from peer %i expected %d";
const char peer_get_pri_by_cat_msg[] = "peer_get_pri_by_cat()";

/*
 * File scope data
 */
static sys_timestamp next_cleanup_time;
static ipparamtype dlswipparams;

/*
 * File scope functions
 */
static void dlsw_fast_cache_init (void);
static uchar *get_rifptr_from_clsi (TCLSIMsg *clsi_msg);
static ulong clsi_hdr_bytes (TCLSIMsg *clsi_msg);

/*
 * Forward statements for IP fast switch 
 */
static void dlsw_fst_input (paktype *pak);


/*
 * peer_fst_init()
 */

void peer_fst_init ()
{
    dlsw_fast_cache_init();
    reg_add_ip_enqueue(FST_DLSW_PROT, dlsw_fst_input, "dlsw_fst_input");
}


/*
 * dlsw_fst_connect()
 */

boolean dlsw_fst_connect (peerentry *peer)
{
    return(TRUE); 
}


/*
 * dlsw_fst_abort()
 */

void dlsw_fst_abort (peerentry *peer)
{
    PDEBUG("\nDLSw: FST aborting connection for peer %i", peer->ipaddr);
}

/*
 * dlsw_fst_sender()
 *
 * datagramstart points at dlx/ssp header.
 * datagramsize includes dlx/ssp header and payload.
 */

core2peer_ret_t dlsw_fst_sender (peerentry  *peer, paktype *pak, 
                                 boolean force, peer_pak_cat category, 
                                 peer_put_t *dontcare)
{
    iphdrtype *iphead;
    ipcache_rntype *cptr = NULL;
    hwidbtype *hwidb;

    /*
     * ip requires that pak->network_start points to start of
     * the ip header.
     */
    pak->datagramstart -= IPHEADERBYTES(NOPT);
    pak->network_start = pak->datagramstart;
    pak->datagramsize += IPHEADERBYTES(NOPT);
    pak->linktype = LINK_IP;
    pak->desthost = peer->ipaddr;

    /*
     * Guarantee a reasonable likelihood of sending out the same interface
     */
    cptr = ipcache_lookup_fast(peer->ipaddr);
    if (cptr) {

        /* copy in the ip header from the peer structure */
        iphead = (iphdrtype *) pak->datagramstart;
        bcopy((uchar *)peer->ip_header, (uchar *)iphead, IPHEADERBYTES(NOPT));
        iphead->id = 0; /* process level fst paks are not sequenced */
        iphead->tl = pak->datagramsize;
        iphead->checksum = 0;                       /* clear checksum field */
        iphead->checksum = ipcrc((ushort *) iphead, ltob(iphead->ihl));
        clear_if_input(pak);

        hwidb = cptr->idb->hwptr;
        if ((hwidb->status & (IDB_ETHER | IDB_TR | IDB_FDDI)) ||
            ((hwidb->status & IDB_SERIAL) && (hwidb->enctype == ET_HDLC))) {
            if (hwidb->status & IDB_FDDI) {
                pak->datagramstart -= cptr->length + 1;
                pak->datagramsize += cptr->length + 1;
                pak->datagramstart[0] = (FDDI_ASYNCHRONOUS_FRAME |
                                         FDDI_48BIT_ADDRESS |
                                         FDDI_LLC_FRAME |
                                         FDDI_PRIORITY_ZERO);
                bcopy(cptr->m.macstring, pak->datagramstart + 1, cptr->length);
            } else if (hwidb->status & IDB_TR) {
                pak->datagramstart -= cptr->length + 2;
                pak->datagramsize += cptr->length + 2;
                pak->datagramstart[0] = AC_PRI4;
                pak->datagramstart[1] = FC_TYPE_LLC;
                bcopy(cptr->m.macstring, pak->datagramstart + 2, cptr->length);
            } else {
                pak->datagramstart -= cptr->length;
                pak->datagramsize += cptr->length;
                bcopy(cptr->m.macstring, pak->datagramstart, cptr->length);
                if (pak->datagramsize < MINETHERSIZE)
                    pak->datagramsize = MINETHERSIZE;
            }
            ip_traffic.outputs++;
            pak->acct_proto = ACCT_PROTO_IP;
            pak->if_output = cptr->idb;
            PDEBUG_FPAK("\nDLSw: fst sender() fast switch to %i out %s", 
                        pak->desthost, pak->if_output->hwptr->hw_namestring);
            datagram_out(pak);
            peer->pkts_tx++;
            return(PEER_SUCCESS);
        }
    }

    /*
     * Can't fast switch the packet. Must slow switch. 
     */

    PDEBUG_FPAK("\nDLSw: fst sender() pak %x to %i size %d", 
                pak, pak->desthost, pak->datagramsize);

    dlswipparams.ident = 0;
    ipwrite(pak, pak->datagramsize, FST_DLSW_PROT, lpeer->ipaddr,
            peer->ipaddr, NULL, NULL, &dlswipparams);

    peer->pkts_tx++;
    return(PEER_SUCCESS);
}


/*
 * dlsw_fst_input()
 *
 * Process input from a FST Peer. 
 * datagramstart points to start of IP header
 *
 * Reassembly performed by ip code before calling DLSw.
 */

static void dlsw_fst_input (paktype *pak)
{
    iphdrtype *ip;
    fast_cache_t *fct = NULL;
    dlx_header_t *dlxh;
    peerentry *peer = NULL;;

    if ((!lpeer) || (dlsw_disable) || (!lpeer->ipaddr)) {
        datagram_done(pak);
        return;
    }

    pak->acct_proto = ACCT_PROTO_SRB;
    ip = (iphdrtype *)ipheadstart(pak);
    dlxh = (dlx_header_t *) (ipdatastart(pak));

    if (dlxh->proto_ver_id == DLX_PROTO_VERSION) {
        /*
         * Fastest way to find an FST peer is through the circuit entry.
         */ 
        fct = dlsw_id2fct(dlxh->session_id);
        if (fct)
            peer = (peerentry *) fct->peer_handle;
    }

    if (!peer)
        peer = (peerentry *) peer_ip_to_handle(ip->srcadr); 

    if (!peer) {
        /*
         * A non-configured remote peer is connecting in. Accept the
         * connection if peer-on-demmand is enabled.
         */
        if (!lpeer->cap.pod_enabled) {
            if (dlsw_peer_debug)
                buginf("\nDLSw FST: pod disabled - dropped packet from %i",
                       ip->srcadr);
            datagram_done(pak);
            return;
        }

        peer = peer_create_remote(0, ip->srcadr, PROM_PEER, NULL, DLSW_FST,
                         	FALSE, FALSE, lpeer->prompeer_defaults.cost,
                                lpeer->prompeer_defaults.largest_frame,
                                lpeer->prompeer_defaults.keepalive_int,
                                lpeer->prompeer_defaults.dlsw_lsap_out,
                                lpeer->prompeer_defaults.netbios_h_oacf,
                                lpeer->prompeer_defaults.netbios_b_oacf,
                                DLSW_MAX_TCP_QLEN, NULL, NULL, NULL,
                                0, 0, 0, 0, lpeer->prompeer_defaults.dlsw_dmac_outlist,
                                lpeer->prompeer_defaults.dlsw_dmac_out, NULL,
			        NORMAL_MODE);

        if (!peer) {
            if (dlsw_peer_debug)
                buginf("\nDLSw: FST create p-o-d failure for %i", ip->srcadr);
            datagram_done(pak);
            return;
        }
    } else if (peer->xport_type != DLSW_FST) {
        datagram_done(pak);
        return;
    }

    /* Count it for LNM if we can. */
    if (fct) {
        idbtype *fct_swidb = firstsw_or_null(fct->lanhwidb);
        srbV.srb_sb_incr_rx_counters(fct_swidb, pak->datagramsize);
    }
    peer->pkts_rx++;
    pak->datagramstart = (uchar *) dlxh;

    if (dlxh->proto_ver_id == DLX_PROTO_VERSION) {
        pak->datagramsize = dlxh->packet_length;
    } else {
        pak->datagramsize = ((ssp_hdr_start_t *) dlxh)->header_len +
                            ((ssp_hdr_start_t *) dlxh)->message_len;
    }

    peer_input(peer, pak, FALSE);
    return;
}


/*********************************************************************
 *
 * Fast Switching Cache Functions
 *
 ********************************************************************/

static void dlsw_fast_cache_init ()
{
    ulong ii;

    TIMER_START(next_cleanup_time, DLSW_FCT_CLEAN_INT);

    for (ii = 0; ii < DLSW_FCT_HASH_SIZE; ii++)
        queue_init(&fast_hash_table[ii], 0);

    dlswipparams.ident = 0;               /* default id */
    dlswipparams.tos = TOS_PREC_ROUTINE;  /* default type of service */
    dlswipparams.ttl = TTL_DEFAULT;       /* default time to live */
    dlswipparams.dontfragment = FRAGMENTS_ALLOWED;  /* can fragment */
    dlswipparams.log = TRUE;              /* okay to log packet */
    dlswipparams.switching = TRUE;
}


void dlsw_fct_clear_cache ()
{
    ulong ii;
    fast_cache_t *fct;
    fast_cache_t *next_fct;
    leveltype status;

    if (!confirm("All DLSw fast switched user sessions will bounce"))
        return;

    status = raise_interrupt_level(NETS_DISABLE);
    for (ii = 0; ii < DLSW_FCT_HASH_SIZE; ii++) {
        fct = fast_hash_table[ii].qhead;
        while (fct) {
            next_fct = fct->next;
            unqueue(&fast_hash_table[ii], fct);
            fct->local_id = 0;
            fct->next = NULL;
            fct->change_flag = TRUE;
            free(fct);
            fct = next_fct;
        }
    }
    reset_interrupt_level(status);
}


void dlsw_fct_clear_peer (peerhandle_t peer_handle)
{
    ulong ii;
    fast_cache_t *fct;
    fast_cache_t *next_fct;
    leveltype status;

    status = raise_interrupt_level(NETS_DISABLE);
    for (ii = 0; ii < DLSW_FCT_HASH_SIZE; ii++) {
        fct = fast_hash_table[ii].qhead;
        while (fct) {
            next_fct = fct->next;
            if (fct->peer_handle == peer_handle) {
                unqueue(&fast_hash_table[ii], fct);
                fct->local_id = 0;
                fct->next = NULL;
                fct->change_flag = TRUE;
                free(fct);
            }
            fct = next_fct;
        }
    }
    reset_interrupt_level(status);
}


/*
 * dlsw_fast_hash_macaddr
 *
 * Generate hash table index for dmac/smac pair
 */
static inline int dlsw_fast_hash_macaddr (uchar *lmac, uchar *rmac)
{
    register ulong n = 0;
    register ulong i = 0;

    /*
     * Add 12 bytes of dmac/smac. Must use byte adds because
     * the macs will be reversed depending on direction.
     */
    for (i = 0; i < IEEEBYTES; i++) {
        n += lmac[i];
        n += rmac[i];
    }
    return(n & DLSW_FCT_HASH_MASK);
}


/*
 * dlsw_find_fct()
 *
 * rsap must have command/response bit set to 0.
 */
static fast_cache_t *dlsw_find_fct (fast_cache_t *head, uchar *lmac,
                                    uchar *rmac, uchar lsap, uchar rsap)
{
    fast_cache_t *fct;

    for (fct = head; fct; fct = fct->next) {
        if ((ieee_equal(fct->lmac, lmac)) && (ieee_equal(fct->rmac, rmac))) {
            if ((lsap == fct->lsap) && (rsap == fct->rsap))
                return(fct);
        }
    }
    return(NULL);
}


/*
 * dlsw_fct_init()
 *
 * -- CMS/RSP bit must be 0 in lsap/rsap.
 * -- rif includs RC - from router to LAN end station.
 */

static void dlsw_init_fct (fast_cache_t *fct, peerhandle_t peer_handle,
                    hwidbtype *lanhwidb, uchar *lmac, uchar *rmac,
                    uchar lsap, uchar rsap, uchar *rif, ulong remote_id)
{
    fct->local_id = (ulong) fct;
    fct->remote_id = remote_id;
    fct->peer_handle = peer_handle;
    GET_TIMESTAMP(fct->last_used);
    GET_TIMESTAMP(((peerentry *) peer_handle)->zero_ckt_time);
    fct->lanhwidb = lanhwidb;
    ieee_copy(lmac, fct->lmac);
    ieee_copy(rmac, fct->rmac);
    fct->lmac[0] |= TR_RII;         /* now it can be compared to raw paks */
    fct->rsap = rsap;
    fct->lsap = lsap;
    fct->riflen = rif[0] & RC_LTHMASK;
    bcopy(rif, fct->rif, fct->riflen);
    fct->fast_wan2lan = NULL;
    fct->fast_hes_wan2lan = NULL;
    fct->change_flag = FALSE;
}


/*
 * dlsw_tr2fct ()
 *
 * macs : source mac follows dest mac.
 * lsap : source sap follows by dest sap.
 */

fast_cache_t *dlsw_tr2fct (uchar *macs, ushort *lsap)
{
   ulong index;
   fast_cache_t *fct;
   uchar *sap = (uchar *) lsap;

   index = dlsw_fast_hash_macaddr(macs, macs + IEEEBYTES);

   fct = dlsw_find_fct(fast_hash_table[index].qhead, macs + IEEEBYTES,
                       macs, (sap[1] & ~SAP_RESPONSE), sap[0]);
   if (fct) {
       if (((peerentry *) fct->peer_handle)->debug_fast_pak)
           buginf("\nDLSw: dlsw_tr2fct() lmac:%e rmac:%e ls:%x rs:%x i:%d",
                  macs+6, macs, sap[1], sap[0], index);
       GET_TIMESTAMP(fct->last_used);
       GET_TIMESTAMP(((peerentry *) fct->peer_handle)->zero_ckt_time);
   }
   return(fct);
}


static fast_cache_t *dlsw_fct_add (fast_cache_t *fct)
{
   ulong index;
   fast_cache_t *oldfct;

   index = dlsw_fast_hash_macaddr(fct->lmac, fct->rmac);
   oldfct = dlsw_find_fct(fast_hash_table[index].qhead, fct->lmac,
                          fct->rmac, fct->lsap, fct->rsap);
   if (oldfct) {
       if (((peerentry *) fct->peer_handle)->debug_fast_pak)
           buginf("\nDLSw: duplicate fast cache entry found");

       oldfct->remote_id = fct->remote_id;
       oldfct->peer_handle = fct->peer_handle;
       oldfct->lanhwidb = fct->lanhwidb;
       oldfct->riflen = fct->riflen;
       bcopy(fct->rif, oldfct->rif, fct->riflen);

       GET_TIMESTAMP(oldfct->last_used);
       GET_TIMESTAMP(((peerentry *) oldfct->peer_handle)->zero_ckt_time);

       fct->next = NULL;
       fct->change_flag = TRUE;
       fct->local_id = 0;
       free(fct);
       return(oldfct);
   } else {
       if (((peerentry *) fct->peer_handle)->debug_fast_pak)
           buginf("\nDLSw: Adding fast entry lm:%e rm:%e ls:%x rs:%x i:%d",
                  fct->lmac,  fct->rmac,  fct->lsap, fct->rsap, index);
       enqueue(&fast_hash_table[index], fct);
       return(fct);
   }
}


/*
 * dlsw_fct_house_keeping()
 *
 * Cleans up aged out fct entries.
 */
void dlsw_fct_house_keeping (void)
{
    ulong ii;
    fast_cache_t *fct;
    fast_cache_t *next_fct;

    if (SLEEPING(next_cleanup_time))
        return;

    for (ii = 0; ii < DLSW_FCT_HASH_SIZE; ii++) {
        fct = fast_hash_table[ii].qhead;
        while (fct) {
            next_fct = fct->next;
            if (CLOCK_OUTSIDE_INTERVAL(fct->last_used, DLSW_FCT_LIFE)) {
                p_unqueue(&fast_hash_table[ii], fct);
                fct->local_id = 0;
                fct->next = NULL;
                fct->change_flag = TRUE;
                free(fct);
            }
            fct = next_fct;
        }
    }
}


/*
 * dlsw_fct_lan2wan_start()
 *
 * Does not consume clsi_msg.
 */

void dlsw_fct_lan2wan_start (TCLSIMsg *clsi_msg, peerhandle_t peer_handle,
                             uchar *macs, uchar dsap, uchar ssap)
{
    peerentry *peer = (peerentry *) peer_handle;
    fast_cache_t *fct;
    paktype *ipak;
    paktype *opak;
    uchar saps[2];
    uchar *rif;
    ulong clsi_hdr_len;
    ulong payload_len;
    dlx_fast_hdr_t *fast_hdr;

    if (peer->peer_type == DEMAND_PEER) { 
        if (peer->conn_state == PEER_DISC) {
            if (dlsw_peer_debug)
                buginf("\nDLSw: start conn timer for demand peer");
            mgd_timer_start(&peer->conn_timer, 0);
            peer->conn_retries = 0;
            return;
        } else if (peer->conn_state != PEER_CONN) {
            if (dlsw_peer_debug)
                buginf("\nDLSw: demand peer not yet connected");
            return;
        }
    } else if (peer->conn_state != PEER_CONN) {
        PDEBUG_FERR("\nDLSw: peer not yet connected");
        return;
    }

    rif = get_rifptr_from_clsi(clsi_msg);
    if (!rif) {
        PDEBUG_FERR("\ndlsw_fct_lan2wan_start(): can't find rif");
        return;
    }

    ipak = CLSIMsgGetPak(clsi_msg);
    saps[0] = dsap;
    saps[1] = ssap;
    fct =  dlsw_tr2fct(macs, (ushort *) saps);
    if (!fct) {
        fct = malloc(sizeof(fast_cache_t));
        if (!fct) {
            return;
        }

        /*
         * Reverse macs/saps - store in cache so local end station is
         * always the destination.
         */
        dlsw_init_fct(fct, peer_handle, ipak->if_input->hwptr,
                      macs + IEEEBYTES, macs, (ssap & ~SAP_RESPONSE),
                      dsap, rif, 0);
        fct = dlsw_fct_add(fct);
    } else {
        /*
         * Update port, rif, and peer_handle to allow fct to 
         * adjust to new routes found by CSM.
         */
        fct->peer_handle = peer_handle;
        fct->remote_id = 0; /* learn new remote correlator */
        fct->lanhwidb = ipak->if_input->hwptr;
        fct->riflen = rif[0] & RC_LTHMASK;
        bcopy(rif, fct->rif, fct->riflen);
    }

    /*
     * Send DLX_FAST_PASS to remote peer.
     * Get pak with DLX fast header at datagramstart.
     */
    clsi_hdr_len = clsi_hdr_bytes(clsi_msg);
    payload_len = ipak->datagramsize - clsi_hdr_len;
    opak = getbuffer(sizeof(dlx_fast_hdr_t) + payload_len);
    if (!opak) {
        return;
    }
    opak->if_input = ipak->if_input;
    opak->datagramsize = sizeof(dlx_fast_hdr_t) + payload_len;
    fast_hdr = (dlx_fast_hdr_t *) opak->datagramstart;
    fast_hdr->packet_length = opak->datagramsize;
    fast_hdr->proto_ver_id = DLX_PROTO_VERSION;
    fast_hdr->message_type = DLX_FAST_PASS;
    fast_hdr->session_id = 0;
    fast_hdr->src_session_id = 0;

    /*
     * Add raw token ring packet payload after DLX header.
     */
    reg_invoke_dlsw_enable_fs(ipak->if_input->hwptr, (void *) fct);
    bcopy(ipak->datagramstart + clsi_hdr_len, fast_hdr + 1, payload_len);
    csm_peer_put(fct->peer_handle, opak, fct->rmac, fct->lmac,
                 fct->rsap, fct->lsap, 0, NULL);
}


/*
 * dlsw_fct_wan2lan_start()
 *
 * Consumes pak.
 */

void dlsw_fct_wan2lan_start (paktype *pak, peerhandle_t peer_handle,
                             dlsw_ring_t *port_ptr, uchar *rif)
{
    fast_cache_t *fct;
    dlx_fast_hdr_t *fast_hdr;
    trrif_t *trh;
    boolean tr_mac_init = FALSE;
    uchar tr_order_macs[IEEEBYTES * 2];
    uchar tr_order_saps[2];
    uchar *saps;
    uchar rem_rif_len;
    int rifdiff;
    ushort *dest, *src;
    idbtype *swidb;
    int ii;
    leveltype status;

    if ((!port_ptr) || (!rif)) {
        datagram_done(pak);
        return;
    }

    swidb = port_ptr->swidb;
    if (swidb == NULL) {
        datagram_done(pak);
        return;
    }

    /*
     * Demand peer connections are trigger from dlsw_lan2wan_start().
     * All peers must be connected before establishing a fast cache
     * entry due to receiving a dlx packet.
     */
    if (((peerentry *) peer_handle)->conn_state != PEER_CONN) {
        if (((peerentry *) peer_handle)->debug_fast_err)
            buginf("\nDLSw: wan2lan_start: peer not yet connected");
        datagram_done(pak);
        return;
    }

    /*
     * Sanity check dlx op.
     */
    fast_hdr = (dlx_fast_hdr_t *) pak->datagramstart;
    trh  = (trrif_t *) (pak->datagramstart + sizeof(dlx_fast_hdr_t));
    rem_rif_len = trh->rc_blth & RC_LTHMASK;
    saps = &trh->rc_blth + rem_rif_len;
    tr_order_saps[0] = saps[1] & ~SAP_RESPONSE;
    tr_order_saps[1] = saps[0];

    /*
     * Does an fct already exist for this session?
     */
    if (fast_hdr->session_id) {
        if (((peerentry *) peer_handle)->debug_fast_pak)
            buginf("\nDLSw: wan2lan_start session_id %x", 
                   fast_hdr->session_id);
        fct = dlsw_id2fct(fast_hdr->session_id);
    } else {
        if (((peerentry *) peer_handle)->debug_fast_pak)
            buginf("\nDLSw: wan2lan_start session_id not yet available");
        ieee_copy(trh->saddr, tr_order_macs);
        tr_order_macs[0] &= ~TR_RII;
        ieee_copy(trh->daddr, tr_order_macs + IEEEBYTES);
        tr_order_macs[IEEEBYTES] |= TR_RII;
        tr_mac_init = TRUE;
        fct = dlsw_tr2fct(tr_order_macs, (ushort *) tr_order_saps);
    }

    status = raise_interrupt_level(NETS_DISABLE);
    if (!fct) {
        fct = malloc(sizeof(fast_cache_t));
        if (!fct) {
            datagram_done(pak);
            reset_interrupt_level(status);
            return;
        }

        if (tr_mac_init == FALSE) {
            ieee_copy(trh->saddr, tr_order_macs);
            tr_order_macs[0] &= ~TR_RII;
        }
        dlsw_init_fct(fct, peer_handle, swidb->hwptr, trh->daddr, tr_order_macs,
                      tr_order_saps[1], tr_order_saps[0], rif,
                      fast_hdr->src_session_id);
        fct = dlsw_fct_add(fct);
    } else {
        fct->remote_id = fast_hdr->src_session_id;
        fct->peer_handle = peer_handle;
        fct->lanhwidb = swidb->hwptr;
        fct->riflen = rif[0] & RC_LTHMASK;
        bcopy(rif, fct->rif, fct->riflen);
    }

    /*
     * Enable fast switching for this circuit.
     */
    reg_invoke_dlsw_enable_fs(fct->lanhwidb, (void *) fct);

    /*
     * Shift the token ring header for the new local rif size.
     */
    rifdiff = rem_rif_len - fct->riflen;
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
        pak->datagramstart = (uchar *) dest;
    } else {
        pak->datagramstart = (uchar *) trh;
    }

    /*
     * Copy in local rif.
     */
    bcopy(fct->rif, &trh->rc_blth, fct->riflen);
    trh->ac = AC_PRI4;

    /*
     * Figure out new size of lan packet.
     *
     * NOTE: For some reason that I haven't figured out yet, with
     *       ethernet WAN encap, pak->datagramsize is ARPA_ENCAP (14)
     *       bytes larger than it should be. So, don't use it.
     */
    pak->datagramsize = fast_hdr->packet_length - sizeof(dlx_fast_hdr_t) -
                        rifdiff;

    if (((peerentry *) fct->peer_handle)->debug_fast_pak)
        buginf("\nDLSw: dlsw_fct_wan2lan_start() sending lan pak of size %d",
               pak->datagramsize);

    pak->flags &= ~PAK_SRB_OUT;           /* re-calc checksum */
    pak->flags |= PAK_SRB_EXPLORER;

    pak->if_output = swidb;
    if (swidb->hwptr->fastsend) {
        (*swidb->hwptr->fastsend)(swidb->hwptr, pak);
    } else {
        datagram_out(pak);
    }
    reset_interrupt_level(status);
}


static uchar *get_rifptr_from_clsi (TCLSIMsg *clsi_msg)
{
    uchar *rif;

    switch (CLSIMsgGetPrimitiveCode(clsi_msg))
    {
      case (CLS_TEST_STN | CLS_INDICATION):     /* Test Frame      */
        rif = (uchar *)clsi_msg + ((TestStnInd_t *)clsi_msg)->fRoutingOffset;
        break;
      case (CLS_ID_STN | CLS_INDICATION):       /* Contextless XID */
        rif = (uchar *)clsi_msg + ((IdStnInd_t *)clsi_msg)->fRoutingOffset;
        break;
      case (CLS_UDATA_STN | CLS_INDICATION):    /* UIDATA          */
        rif = (uchar *)clsi_msg + ((UDataStnInd_t *)clsi_msg)->fRoutingOffset;
        break;
      case (CLS_CONNECT_STN | CLS_INDICATION):  /* Contextless SABME */
        rif = (uchar *)clsi_msg +
                                ((ConnectStnInd_t *)clsi_msg)->fRoutingOffset;
        break;
      default:
        return(NULL);
    }

    /*
     * Since this rif is from end station to router, we need to reverse the
     * direction before storing it.
     */
    if (rif[1] & RC_D)
        rif[1] &= ~RC_D;
    else
        rif[1] |= RC_D;
    return(rif);
}

/*
 * clsi_hdr_bytes()
 *
 * True header padding, not necessarily sizeof header.
 */

static ulong clsi_hdr_bytes (TCLSIMsg *clsi_msg)
{
    switch(clsi_msg->fPrimitiveCode) {
      case CLS_ID_STN | CLS_INDICATION:
          return(offsetof(IdStnInd_t, fXIDData) +
                    ((IdStnInd_t *) clsi_msg)->fReserveLength);
      case CLS_UDATA_STN | CLS_INDICATION:
          return(offsetof(UDataStnInd_t, fUData) +
                 ((UDataStnInd_t *) clsi_msg)->fReserveLength);
      case CLS_CONNECT_STN | CLS_INDICATION:
          return(offsetof(ConnectStnInd_t, fCONNECTLength) +
                    ((ConnectStnInd_t *) clsi_msg)->fReserveLength);
      default:
          /* errmsg() */
          buginf("\nclsi_hdr_bytes() Invalid primitive %x",
                 clsi_msg->fPrimitiveCode);
          return(0);
    }
}

