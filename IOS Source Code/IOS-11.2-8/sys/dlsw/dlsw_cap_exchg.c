/* $Id: dlsw_cap_exchg.c,v 3.6.6.8 1996/09/11 18:37:01 akhanna Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_cap_exchg.c,v $
 *------------------------------------------------------------------
 * Implementation of DLSw Capabilities Exchange
 *
 * July 1994, Frank Bordonaro 
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Building and parsing of DLSw capabilities exchange general data 
 * stream variables and control vectors.
 *------------------------------------------------------------------
 * $Log: dlsw_cap_exchg.c,v $
 * Revision 3.6.6.8  1996/09/11  18:37:01  akhanna
 * CSCdi67926:  DLSW: ethernet direct encap causes crash
 * Branch: California_branch
 *
 * Revision 3.6.6.7  1996/07/23  13:21:16  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.6.6.6  1996/07/01  21:15:55  fbordona
 * CSCdi61790:  DLSw Error Msg - DLSWP-3-PGENERAL: DLSw: Not removing
 *              blocked peer. Race condition between receiving cap exchg
 *              message and opening of TCP write pipe. Allow cap exchg
 *              to time out.
 * Branch: California_branch
 *
 * Revision 3.6.6.5  1996/06/28  23:05:38  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.6.6.4  1996/06/27  15:13:25  fbordona
 * CSCdi61278:  DLSw: System restarted by bus error at PC 0xD0D0D0D
 *              action_b(). Do not free peers if blocked on TCP open.
 *              Connect timer not set properly for priority peers.
 * Branch: California_branch
 *
 * Revision 3.6.6.3  1996/06/11  15:00:44  ravip
 * CSCdi58441:  xid3 flooding in csna/dlsw environment cause access
 *              router to reboot
 *                  - Carry xid command response bit in the message
 *                  - Use Capex to exchange information about this
 *                    support
 * Branch: California_branch
 *
 * Revision 3.6.6.2  1996/05/17  10:44:31  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.2.7  1996/05/14  03:19:51  kpatel
 * CSCdi55075:  Cant define dlsw direct encapsulation for more that pvc
 *              dls frame-relay does not work over point to point
 *              sub-interface (CSCdi55085)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.2.6  1996/04/26  07:33:34  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.5.2.5  1996/04/24  21:09:02  ioakley
 * CSCdi48542:  Correct error in received run-time capabilities exchange
 *              processing.  Prevents DLSw ICANNOTREACH sap XX from
 *              disappearing in a running configuration.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.2.4  1996/04/20  02:07:51  fbordona
 * CSCdi55177:  DLSw phase I to convert peer to managed timers
 *              Required for scalability of large DLSw peer nets.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.2.3  1996/04/05  23:11:55  kpatel
 * CSCdi51479:  Memory leak when removing dlsw configuration.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.2.2  1996/04/03  14:00:42  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.2.1  1996/03/17  17:37:31  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.6.6.1  1996/03/18  19:30:56  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/16  06:38:13  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.26.2  1996/03/07  08:43:48  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  00:45:07  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/03/06  23:43:31  fbordona
 * CSCdi50868:  Dlsw Interop: Support vendor context cap exchg control
 *              vector.
 *
 * Revision 3.5  1996/02/20  22:08:02  fbordona
 * CSCdi47801:  SEgV exception, PC0x60637DFC tcpdriver_fhost.
 *              Use the tcpencaps handle now passed in the tcp
 *              closef vector (CSCdi48380) to determine if the
 *              DLSw read or write pipe has been closed.
 *
 * Revision 3.4  1996/02/01  06:01:01  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/11/17  09:02:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:21:00  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:22:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/10/09  20:18:37  rnaderi
 * CSCdi41641:  Add sub-interface option to the parser for DLSw+/FR
 *
 * Revision 2.4  1995/08/10  18:22:22  fbordona
 * CSCdi38575:  DLSw promiscuous peer doesnt learn remote config
 *
 * Revision 2.3  1995/06/28  18:48:28  fbordona
 * CSCdi36524:  CLS SDLC multidrop broken
 *              Clean up DLSw header files
 *
 * Revision 2.2  1995/06/18  21:21:05  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:25:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <ciscolib.h>
#include "interface_generic.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "mgd_timers.h"
#include "../dlsw/parser_defs_dlsw.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../tcp/tcpdriver.h"
#include "../tcp/tcp.h"
#include "../if/network.h"
#include "../if/rif_util.h"

#include "../ibm/netbios.h"
#include "../srt/rsrb.h"
#include "../dlsw/dlsw_dlx.h"
#include "../dlsw/dlsw_ssp.h"
#include "../dlsw/dlsw_peer_externs.h"
#include "../dlsw/dlsw_peer.h"
#include "../dlsw/dlsw_cap_exchg.h"
#include "../ui/debug.h"
#include "../dlsw/dlsw_debug.h"
#include "../h/mibconf.h"


/*
 * File Scope Function Prototypes
 */
static boolean cap_learn_from_partner (peerentry *);
static ulong build_cap_cvs (peerentry *, uchar *);
static ulong build_standard_cvs (uchar **);
static ulong build_cisco_cvs (peerentry *, uchar **);
static ulong build_runtime_cv (cv_hdr_t *);
static char *parse_required_cvs (paktype *, peerentry *, char *, 
                                 ushort *, ushort *);
static char *parse_optional_cvs (paktype *, peerentry *, char *, 
                                 ushort *, ushort *, ushort);
static char *parse_vendor_id_cv (paktype *, peerentry *, cv_vendor_id_t *, 
                                 ushort *, ushort *);
static char *parse_vers_num_cv (paktype *, peerentry *, cv_vers_num_t *,
                                 ushort *, ushort *);
static char *parse_ipw_cv (paktype *, peerentry *, cv_ipw_t *,
                                 ushort *, ushort *);
static char *parse_vstr_cv (paktype *, peerentry *, cv_vstr_t *,
                                 ushort *, ushort *);
static char *parse_sap_cv (paktype *, peerentry *, cv_sap_t *,
                                 ushort *, ushort *);
static char *parse_tcp_cv (paktype *, peerentry *, cv_tcp_t *,
                                 ushort *, ushort *);
static char *parse_excl_cv (paktype *, peerentry *, cv_excl_t *,
                                 ushort *, ushort *);
static char *parse_mac_cv (paktype *, peerentry *, cv_mac_t *,
                                 ushort *, ushort *);
static char *parse_nbname_cv (paktype *, peerentry *, cv_nbname_t *,
                                 ushort *, ushort *);
static char *parse_cisco_vers_cv (paktype *, peerentry *, cv_cisco_vers_t *,
                                 ushort *, ushort *, boolean *);
static char *parse_vend_ctxt_cv (paktype *, peerentry *, cv_vctxt_t *,
                                 ushort *, ushort *, boolean *);
static char *parse_lack_cv (paktype *, peerentry *, cv_lack_t *,
                                 ushort *, ushort *, boolean *);
static char *parse_pri_cv (paktype *, peerentry *, cv_pri_t *,
                                 ushort *, ushort *, boolean *);
static char *parse_pgrp_cv (paktype *, peerentry *, cv_pgrp_t *,
                                 ushort *, ushort *, boolean *);
static char *parse_border_cv (paktype *, peerentry *, cv_border_t *,
                                 ushort *, ushort *, boolean *);
static char *parse_cost_cv (paktype *, peerentry *, cv_cost_t *,
                                 ushort *, ushort *, boolean *);
static char *parse_spoof_and_seg_cv (paktype *, peerentry *, cv_biu_seg_t *,
                                 ushort *, ushort *, boolean *);
static char *parse_xid_op_cv (paktype *, peerentry *, cv_xid_op_t *,
                                 ushort *, ushort *, boolean *);
static char *parse_peer_type_cv (paktype *, peerentry *, cv_peer_type_t *,
                                 ushort *, ushort *, boolean *);
static void send_neg_rsp_id (paktype *, peerentry *, ushort, rsp_reason_t);
static boolean send_pos_rsp_id (paktype *pak, peerentry *peer);


/**
 ** --------------------------------------------------------------
 ** Capabilities Sending Functions
 ** --------------------------------------------------------------
 **/

/*
 * send_capabilities()
 */

boolean send_capabilities (peerentry *peer)
{
    paktype *pak;
    cap_gds_t *gds_hdr;
    ssp_hdr_t *ssp_ptr;
    char buff[DLSW_MAX_PEER_ID];

    /* don't know the pak size or ssp_msg_len yet - fake it */
    pak = get_ssp_pak(1024);
    if (!pak) {
        errmsg(PEER_NOMEM, "send start-up capabilities exchange");
        return(FALSE);
    }
    ssp_ptr = (ssp_hdr_t *) pak->datagramstart;
    gds_hdr = (cap_gds_t *) (pak->datagramstart + SSP_CNTL_HDR_LEN);
    gds_hdr->gds_id = SSP_CAP_EX_ID; 
    gds_hdr->gds_len = sizeof(cap_gds_t);
    gds_hdr->gds_len += build_cap_cvs(peer, ((uchar *)gds_hdr) + 
                                      sizeof(cap_gds_t));
    ssp_ptr->ssp_msg_len  = gds_hdr->gds_len; 
    pak->datagramsize = SSP_CNTL_HDR_LEN + gds_hdr->gds_len;
    ssp_ptr->ssp_dirn = SSP_DIRN_FORWARD;
    if ((*peer->sender)(peer,pak,TRUE,PEER_CONN_SETUP,NULL)==PEER_SUCCESS) {
        PDEBUG("\nDLSw: CapExId Msg sent to %s", peer_str(peer, buff));
        return(TRUE);
    } else {
        dlsw_peer_fsm(peer, CLOSE_CONNECTION);
        PDEBUG("\nDLSw: Capabilities send failed to %s", peer_str(peer, buff));
        return(FALSE);
    } 
}


/*
 * send_runtime_capabilities()
 */

boolean send_runtime_capabilities ()
{
    paktype *pak;
    paktype *newpak;
    cap_gds_t *gds_hdr;
    ssp_hdr_t *ssp_ptr;
    peerentry *peer;
    ulong ii;
    char buff[DLSW_MAX_PEER_ID];

    /* don't know the pak size or ssp_msg_len yet - fake it */
    pak = get_ssp_pak(1024);
    if (!pak) {
        errmsg(PEER_NOMEM, "get pak to send runtime capabilities exchange");
        return(FALSE);
    }
    ssp_ptr = (ssp_hdr_t *) pak->datagramstart;
    gds_hdr = (cap_gds_t *) (pak->datagramstart + SSP_CNTL_HDR_LEN);
    gds_hdr->gds_id = SSP_CAP_EX_ID;
    gds_hdr->gds_len = sizeof(cap_gds_t);
    gds_hdr->gds_len += build_runtime_cv((cv_hdr_t *)
                        (((uchar *)gds_hdr) + sizeof(cap_gds_t)));
    if (!gds_hdr->gds_len)
        return(FALSE);

    ssp_ptr->ssp_dirn = SSP_DIRN_FORWARD;
    ssp_ptr->ssp_msg_len  = gds_hdr->gds_len;
    pak->datagramsize = SSP_CNTL_HDR_LEN + gds_hdr->gds_len;

    for (ii = 0; ii < DLSW_MAX_PGROUP; ii++) {
        if ((peer = (peerentry *) conn_dlx_peerQ[ii].qhead)) {
            for (; peer; peer = peer->next) {
                if (peer->conn_state != PEER_CONN)
                    continue;
                newpak = pak_duplicate(pak);
                if (!newpak) {
                    return(TRUE);
                }

                if ((*peer->sender) (peer, newpak, TRUE, PEER_CONN_ADMIN, 
                                     NULL) == PEER_SUCCESS) {
                    PDEBUG("\nDLSw: Run-time capabilities sent to %s",
                           peer_str(peer, buff));
                } else {
                    PDEBUG("\nDLSw: Run-time capabilities send failed to %s",
                           peer_str(peer, buff));
                    dlsw_peer_fsm(peer, CLOSE_CONNECTION);
                }
            }
        }
    }

    if ((peer = (peerentry *) conn_dlsw_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            if (peer->conn_state != PEER_CONN)
                continue;
            newpak = pak_duplicate(pak);
            if (!newpak) {
                return(TRUE);
            }

            if ((*peer->sender) (peer, newpak, TRUE, PEER_CONN_ADMIN, 
                                 NULL) == PEER_SUCCESS) {
                PDEBUG("\nDLSw: Run-time capabilities sent to %s", 
                       peer_str(peer, buff));
            } else {
                PDEBUG("\nDLSw: Run-time capabilities send failed to %s", 
                       peer_str(peer, buff));
                dlsw_peer_fsm(peer, CLOSE_CONNECTION);
            }
        }
    }
    datagram_done(pak);
    return(TRUE);
}


/*
 * send_neg_rsp_id()
 */

static void send_neg_rsp_id (paktype *pak, peerentry *peer, 
                         ushort offset, rsp_reason_t reason)
{
    ssp_hdr_t *ssp_ptr;
    cap_neg_rsp_t *cap_neg_rsp;
    char buff[DLSW_MAX_PEER_ID];

    if ((!pak) || (!peer)) {
        if (pak)
            datagram_done(pak);
        errmsg(PEER_GENERAL, "send_neg_rsp_id(): pak or peer is NULL");
        return;
    }

    pak->datagramsize = SSP_CNTL_HDR_LEN + sizeof(cap_neg_rsp_t);
    ssp_ptr = (ssp_hdr_t *) pak->datagramstart;
    ssp_ptr->ssp_vers  = SSP_VERS; 
    ssp_ptr->ssp_hdr_len  = SSP_CNTL_HDR_LEN;
    ssp_ptr->ssp_msg_len  = sizeof(cap_neg_rsp_t);
    ssp_ptr->ssp_msg_type  = SSP_OP_capability_xchg;
    ssp_ptr->ssp_dirn = SSP_DIRN_BACKWARD;

    cap_neg_rsp = (cap_neg_rsp_t *)
                  (pak->datagramstart + SSP_CNTL_HDR_LEN);
    cap_neg_rsp->gds_hdr.gds_id = SSP_CAP_NEG_RSP_ID;
    cap_neg_rsp->gds_hdr.gds_len = sizeof(cap_neg_rsp_t);
    PUTSHORT(&cap_neg_rsp->offset, offset);
    PUTSHORT(&cap_neg_rsp->reason, reason); 

    if ((*peer->sender)(peer,pak,TRUE,PEER_CONN_SETUP,NULL)==PEER_SUCCESS) {
        PDEBUG("\nDLSw: Neg CapExResp (r:%d o:%d) sent to %s",
               reason, offset, peer_str(peer, buff));
    } else {
        PDEBUG("\nDLSw: Neg CapExResp (r:%d o:%d) send failed to %s",
           reason, offset, peer_str(peer, buff));
    }

    /*
     * Close the connection after sending the negative response.
     */
    dlsw_peer_fsm(peer, CLOSE_CONNECTION);
}


/*
 * send_pos_rsp_id()
 */

static boolean send_pos_rsp_id (paktype *pak, peerentry *peer)
{
    ssp_hdr_t *ssp_ptr;
    cap_pos_rsp_t *cap_pos_rsp;
    char buff[DLSW_MAX_PEER_ID];

    if ((!pak) || (!peer)) {
        if (pak)
            datagram_done(pak);
        errmsg(PEER_GENERAL, "send_pos_rsp_id(): pak or peer is NULL");
        dlsw_peer_fsm(peer, CLOSE_CONNECTION);
        return(FALSE);
    }

    pak->datagramsize = SSP_CNTL_HDR_LEN + sizeof(cap_pos_rsp_t);
    ssp_ptr = (ssp_hdr_t *) pak->datagramstart;
    ssp_ptr->ssp_vers  = SSP_VERS;
    ssp_ptr->ssp_hdr_len  = SSP_CNTL_HDR_LEN;
    ssp_ptr->ssp_msg_len  = sizeof(cap_pos_rsp_t);
    ssp_ptr->ssp_msg_type  = SSP_OP_capability_xchg;
    ssp_ptr->ssp_dirn = SSP_DIRN_BACKWARD;

    cap_pos_rsp = (cap_pos_rsp_t *)
                  (pak->datagramstart + SSP_CNTL_HDR_LEN);
    cap_pos_rsp->gds_id = SSP_CAP_POS_RSP_ID;
    cap_pos_rsp->gds_len = sizeof(cap_pos_rsp_t);

    if ((*peer->sender)(peer,pak,TRUE,PEER_CONN_SETUP,NULL)==PEER_SUCCESS) {
        PDEBUG("\nDLSw: Pos CapExResp sent to %s", peer_str(peer, buff));
        return(TRUE);
    } else {
        PDEBUG("\nDLSw: Pos CapExResp send failed to %s", peer_str(peer, buff));
        dlsw_peer_fsm(peer, CLOSE_CONNECTION);
        return(FALSE);
    }
}


/*
 * build_cap_cvs()
 */

static ulong build_cap_cvs (peerentry *peer, uchar *start)
{
    ulong tot_cv_len = 0;
    uchar *cv_ptr = start;

    tot_cv_len = build_standard_cvs(&cv_ptr);
    tot_cv_len += build_cisco_cvs(peer, &cv_ptr);
    return(tot_cv_len);
}


/*
 * build_standard_cvs()
 */

static ulong build_standard_cvs (uchar **start)
{
    ulong tot_cv_len;
    cv_hdr_t *cv_hdr;
    ulong len;
    macentry_t *macentry;
    nbentry_t *nbentry;

    /* vendor_id CV */
    cv_hdr = (cv_hdr_t *) *start;
    cv_hdr->cv_len = sizeof(cv_vendor_id_t);
    cv_hdr->cv_id = CAP_VENDOR_ID;
    bcopy(lpeer->cap.vendor_id, 
          ((cv_vendor_id_t *)cv_hdr)->vendor_id, SNAP_OUIBYTES);
    tot_cv_len = cv_hdr->cv_len;

    /* version number CV */
    cv_hdr = (cv_hdr_t *) (((char *)cv_hdr) + cv_hdr->cv_len);
    cv_hdr->cv_len = sizeof(cv_vers_num_t);
    cv_hdr->cv_id = CAP_VERS_NUM;
    ((cv_vers_num_t *)cv_hdr)->vers_num = lpeer->cap.version_number;
    ((cv_vers_num_t *)cv_hdr)->rel_num  = lpeer->cap.release_number;
    tot_cv_len += cv_hdr->cv_len;

    /* initial pacing window CV */
    cv_hdr = (cv_hdr_t *) (((char *)cv_hdr) + cv_hdr->cv_len);
    cv_hdr->cv_len = sizeof(cv_ipw_t);
    cv_hdr->cv_id = CAP_IPW;
    PUTSHORT(&((cv_ipw_t *)cv_hdr)->window, lpeer->cap.initial_pacing_window);
    tot_cv_len += cv_hdr->cv_len;

    /* supported saps CV */
    cv_hdr = (cv_hdr_t *) (((char *)cv_hdr) + cv_hdr->cv_len);
    cv_hdr->cv_len = sizeof(cv_sap_t);
    cv_hdr->cv_id = CAP_SAP_LIST;
    bcopy(lpeer->cap.supp_saps, ((cv_sap_t *)cv_hdr)->sap_vector, SAP_CV_BYTES);
    tot_cv_len += cv_hdr->cv_len;

    /* version string CV */
    len = strlen(lpeer->cap.version_string); 
    cv_hdr = (cv_hdr_t *) (((char *)cv_hdr) + cv_hdr->cv_len);
    cv_hdr->cv_len = sizeof(cv_hdr_t) + len;
    cv_hdr->cv_id = CAP_VERS_STRING;
    bcopy(lpeer->cap.version_string, 
          ((cv_vstr_t *)cv_hdr)->vers_string, len); 
    tot_cv_len += cv_hdr->cv_len;

    /* number of TCP session supported CV */
    cv_hdr = (cv_hdr_t *) (((char *)cv_hdr) + cv_hdr->cv_len);
    cv_hdr->cv_len = sizeof(cv_tcp_t);
    cv_hdr->cv_id = CAP_NUM_TCP;
    ((cv_tcp_t *)cv_hdr)->num_tcp_sup = lpeer->cap.num_tcp_sessions;
    tot_cv_len += cv_hdr->cv_len;

    /* mac exclusivity CV */
    cv_hdr = (cv_hdr_t *) (((char *)cv_hdr) + cv_hdr->cv_len);
    cv_hdr->cv_len = sizeof(cv_excl_t);
    cv_hdr->cv_id = CAP_MAC_EXCL;
    ((cv_excl_t *)cv_hdr)->excl = lpeer->cap.mac_exclusive;
    tot_cv_len += cv_hdr->cv_len;

    /* netbios exclusivity CV */
    cv_hdr = (cv_hdr_t *) (((char *)cv_hdr) + cv_hdr->cv_len);
    cv_hdr->cv_len = sizeof(cv_excl_t);
    cv_hdr->cv_id = CAP_NETBIOS_EXCL;
    ((cv_excl_t *)cv_hdr)->excl = lpeer->cap.nb_exclusive;
    tot_cv_len += cv_hdr->cv_len;

    /* mac-address CV - one for each mac-addr */
    macentry = (macentry_t *) lpeer->cap.macQ.qhead;
    if (macentry) {
        for (; macentry; macentry = macentry->next) {
            cv_hdr = (cv_hdr_t *) (((char *)cv_hdr) + cv_hdr->cv_len);
            cv_hdr->cv_len = sizeof(cv_mac_t);
            cv_hdr->cv_id = CAP_MAC_ADDR;
            ieee_copy(macentry->macaddr, ((cv_mac_t *)cv_hdr)->macaddr); 
            ieee_copy(macentry->macmask, ((cv_mac_t *)cv_hdr)->macmask); 
            tot_cv_len += cv_hdr->cv_len;
        }
    }

    /* netbios-name CV - one for each name */
    nbentry = (nbentry_t *) lpeer->cap.nbnameQ.qhead;
    if (nbentry) {
        for (; nbentry; nbentry = nbentry->next) {
            len = strlen(nbentry->nbname);
            cv_hdr = (cv_hdr_t *) (((char *)cv_hdr) + cv_hdr->cv_len);
            cv_hdr->cv_len = sizeof(cv_hdr_t) + len;
            cv_hdr->cv_id = CAP_NB_NAME;
            bcopy(nbentry->nbname, ((cv_nbname_t *)cv_hdr)->nbname, len);
            tot_cv_len += cv_hdr->cv_len;
        }
    }

    /* *start = ((uchar *) cv_hdr) + cv_hdr->cv_len; */
    *start = *start + tot_cv_len;
    return(tot_cv_len);
}


/*
 * build_cisco_cvs()
 */

static ulong build_cisco_cvs (peerentry *peer, uchar **start)
{
    ulong tot_cv_len;
    cv_hdr_t *cv_hdr;

    /* vendor context CV must preceed cisco CVs */
    cv_hdr = (cv_hdr_t *) *start;
    cv_hdr->cv_len = sizeof(cv_vctxt_t);
    cv_hdr->cv_id = CAP_VENDOR_CNTXT;
    bcopy(lpeer->cap.vendor_id, 
          ((cv_vctxt_t *)cv_hdr)->vendor_id, SNAP_OUIBYTES);
    tot_cv_len = cv_hdr->cv_len;

    /* cisco version CV */
    cv_hdr = (cv_hdr_t *) (((char *)cv_hdr) + cv_hdr->cv_len);
    cv_hdr->cv_len = sizeof(cv_cisco_vers_t);
    cv_hdr->cv_id = CAP_CISCO_VERS;
    PUTSHORT(&((cv_cisco_vers_t *)cv_hdr)->cisco_version, CISCO_VERSION_1);
    tot_cv_len += cv_hdr->cv_len;

    /* local-ack capable CV */
    cv_hdr = (cv_hdr_t *) (((char *)cv_hdr) + cv_hdr->cv_len);
    cv_hdr->cv_len = sizeof(cv_lack_t);
    cv_hdr->cv_id = CAP_LACK;
    ((cv_lack_t *)cv_hdr)->lack = peer->local_ack;
    tot_cv_len += cv_hdr->cv_len;

    /* priority CV */
    cv_hdr = (cv_hdr_t *) (((char *)cv_hdr) + cv_hdr->cv_len);
    cv_hdr->cv_len = sizeof(cv_pri_t);
    cv_hdr->cv_id = CAP_PRIORITY;
    ((cv_pri_t *)cv_hdr)->priority = peer->priority;
    tot_cv_len += cv_hdr->cv_len;

    /* peer group CV */
    cv_hdr = (cv_hdr_t *) (((char *)cv_hdr) + cv_hdr->cv_len);
    cv_hdr->cv_len = sizeof(cv_pgrp_t);
    cv_hdr->cv_id = CAP_PEER_GRP;
    PUTSHORT(&((cv_pgrp_t *)cv_hdr)->group, lpeer->cap.group);
    tot_cv_len += cv_hdr->cv_len;

    /* border peer CV */
    cv_hdr = (cv_hdr_t *) (((char *)cv_hdr) + cv_hdr->cv_len);
    cv_hdr->cv_len = sizeof(cv_border_t);
    cv_hdr->cv_id = CAP_BORDER;
    ((cv_border_t *)cv_hdr)->border = lpeer->cap.border;
    tot_cv_len += cv_hdr->cv_len;

    /* peer cost CV */
    cv_hdr = (cv_hdr_t *) (((char *)cv_hdr) + cv_hdr->cv_len);
    cv_hdr->cv_len = sizeof(cv_cost_t);
    cv_hdr->cv_id = CAP_COST;
    ((cv_cost_t *)cv_hdr)->cost = lpeer->cap.cost;
    tot_cv_len += cv_hdr->cv_len;

    /*
     * There is a bug in 10.3 version previous to 10.3(4) that
     * cause a router to hang if it receives a cap exchg CV that
     * it doesn't recognize. This bug was fixed by CSCdi35292.
     * To minimize the risk of this happening, only send new CVs
     * if we need to override the deault.
     */

    /* peer max recievable i-field spoofing and BIU segmenting CV */
    if (lpeer->cap.spoof_and_seg) {
        cv_hdr = (cv_hdr_t *) (((char *)cv_hdr) + cv_hdr->cv_len);
        cv_hdr->cv_len = sizeof(cv_biu_seg_t);
        cv_hdr->cv_id = CAP_SPOOF_AND_FRAG;
        ((cv_biu_seg_t *)cv_hdr)->spoof_and_seg = lpeer->cap.spoof_and_seg;
        tot_cv_len += cv_hdr->cv_len;
    }

    /* xid op support cv */
    cv_hdr = (cv_hdr_t *) (((char *)cv_hdr) + cv_hdr->cv_len);
    cv_hdr->cv_len = sizeof(cv_xid_op_t);
    cv_hdr->cv_id = CAP_XID_OP_SUPPORT;
    ((cv_xid_op_t *)cv_hdr)->xid_op_support = TRUE;
    tot_cv_len += cv_hdr->cv_len;

    /* peer type cv */
    cv_hdr = (cv_hdr_t *) (((char *)cv_hdr) + cv_hdr->cv_len);
    cv_hdr->cv_len = sizeof(cv_peer_type_t);
    cv_hdr->cv_id = CAP_PEER_TYPE;
    ((cv_peer_type_t *)cv_hdr)->peer_type = peer->peer_type;
    tot_cv_len += cv_hdr->cv_len;

    return(tot_cv_len);
}


/*
 * build_runtime_cv()
 */

static ulong build_runtime_cv (cv_hdr_t *cv_hdr)
{
    macentry_t *macentry;
    nbentry_t *nbentry;
    ulong tot_cv_len;
    ulong len;

    /* supported SAPs cv */
    cv_hdr->cv_len = sizeof(cv_sap_t);
    cv_hdr->cv_id = CAP_SAP_LIST;
    bcopy(lpeer->cap.supp_saps,
          ((cv_sap_t *)cv_hdr)->sap_vector, SAP_CV_BYTES);
    tot_cv_len = cv_hdr->cv_len;

    /* mac exclusivity CV */
    cv_hdr = (cv_hdr_t *) (((char *)cv_hdr) + cv_hdr->cv_len);
    cv_hdr->cv_len = sizeof(cv_excl_t);
    cv_hdr->cv_id = CAP_MAC_EXCL;
    ((cv_excl_t *)cv_hdr)->excl = lpeer->cap.mac_exclusive;
    tot_cv_len += cv_hdr->cv_len;

    /* netbios exclusivity CV */
    cv_hdr = (cv_hdr_t *) (((char *)cv_hdr) + cv_hdr->cv_len);
    cv_hdr->cv_len = sizeof(cv_excl_t);
    cv_hdr->cv_id = CAP_NETBIOS_EXCL;
    ((cv_excl_t *)cv_hdr)->excl = lpeer->cap.nb_exclusive;
    tot_cv_len += cv_hdr->cv_len;

    /* mac-address CV - one for each mac-addr */
    macentry = (macentry_t *) lpeer->cap.macQ.qhead;
    if (macentry) {
        for (; macentry; macentry = macentry->next) {
            cv_hdr = (cv_hdr_t *) (((char *)cv_hdr) + cv_hdr->cv_len);
            cv_hdr->cv_len = sizeof(cv_mac_t);
            cv_hdr->cv_id = CAP_MAC_ADDR;
            ieee_copy(macentry->macaddr, ((cv_mac_t *)cv_hdr)->macaddr);
            ieee_copy(macentry->macmask, ((cv_mac_t *)cv_hdr)->macmask);
            tot_cv_len += cv_hdr->cv_len;
          }
    }

    nbentry = (nbentry_t *) lpeer->cap.nbnameQ.qhead;
    if (nbentry) {
        for (; nbentry; nbentry = nbentry->next) {
            len = strlen(nbentry->nbname);
            cv_hdr = (cv_hdr_t *) (((char *)cv_hdr) + cv_hdr->cv_len);
            cv_hdr->cv_len = sizeof(cv_hdr_t) + len;
            cv_hdr->cv_id = CAP_NB_NAME;
            bcopy(nbentry->nbname, ((cv_nbname_t *)cv_hdr)->nbname, len);
            tot_cv_len += cv_hdr->cv_len;
        }
    }
    return(tot_cv_len);
}


/**
 ** --------------------------------------------------------------
 ** Capabilities Receiving Functions
 ** --------------------------------------------------------------
 **/


/*
 * peer_cap_exchg_input()
 *
 */
 
void peer_cap_exchg_input (peerentry *peer, paktype *pak)
{
    cap_gds_t *gds_hdr;
    char buff[DLSW_MAX_PEER_ID];

    gds_hdr = (cap_gds_t *) (pak->datagramstart + SSP_CNTL_HDR_LEN);
    switch (gds_hdr->gds_id) {
      case SSP_CAP_EX_ID:
        /*
         * Direct forces us to process CapEx msgs at process level
         * since we can't malloc at interrupt level.
         */
        PDEBUG("\nDLSw: Recv CapExId Msg from %s", peer_str(peer, buff));
        /*
         * TCP peers can not process incoming CapEx Msgs until
         * the write pipe is open (read pipe is already open).
         */
        if ((peer->xport_type == DLSW_TCP) && (!peer->tcp_wr_t)) {
            PDEBUG("\nDLSw: can't process CapExId until write pipe open");
            datagram_done(pak);
        } else if (peer->conn_state != PEER_CONN) {
            /*
             * If partner's Capabilities are acceptable and we are
             * able to send a positive response, then tell peer fsm.
             * parse_cap_ex_id() consumes the pak.
             */
            if (parse_cap_ex_id(pak, peer))
                dlsw_peer_fsm(peer, CAP_RCVD);
        } else {
            parse_runtime_cap_ex_id(pak, peer); /* consumes pak */
        }
        break;

      case SSP_CAP_POS_RSP_ID:
        PDEBUG("\nDLSw: Recv CapExPosRsp Msg from %s", peer_str(peer, buff));
        datagram_done(pak);
        peer->cap.cap_ack = TRUE;
        dlsw_peer_fsm(peer, CAP_RSP_RCVD);
        break;

      case SSP_CAP_NEG_RSP_ID:
        PDEBUG("\nDLSw: Recv CapExNegRsp Msg from %s", peer_str(peer, buff));
        datagram_done(pak);
        dlsw_peer_fsm(peer, CLOSE_CONNECTION);
        break;

      default:
        /*
         * send_neg_rsp_id() is responsible for pak
         */
        PDEBUG("\nDLSw: Recv CapEx Unknown Msg %x from %s", 
               gds_hdr->gds_id, peer_str(peer, buff));
        send_neg_rsp_id(pak, peer, 2, INVALID_GDS_ID);
        break;
    }
}


/*
 * parse_runtime_cap_ex_id()
 *
 * Run-time CapEx is a replacement operation, so clear previously 
 * learned CV info before parsing new runtime CV.
 *
 * Run capabilities exchange CV errors will take the session down.
 */

void parse_runtime_cap_ex_id (paktype *pak, peerentry *peer)
{
    ssp_hdr_t *ssp_ptr;
    cap_gds_t *gds_hdr;
    char *cv;
    ushort offset;
    ushort r;
    ulong ii;
    macentry_t *macentry;
    nbentry_t *nbentry;
    char buff[DLSW_MAX_PEER_ID];

    /*
     * Clear previous runtime info.
     */
    peer->cap.mac_exclusive = FALSE;
    peer->cap.nb_exclusive = FALSE;
    for (ii = 0; ii < SAP_CV_BYTES; ii++)
         peer->cap.supp_saps[ii] = ~0;

    if (peer->cap.macQ.qhead) {
        while ((macentry = p_dequeue(&peer->cap.macQ)))
            free(macentry);
    }
    if (peer->cap.nbnameQ.qhead) {
        while ((nbentry= p_dequeue(&peer->cap.nbnameQ)))
            free(nbentry);
    }

    ssp_ptr = (ssp_hdr_t *) pak->datagramstart;
    gds_hdr = (cap_gds_t *) (pak->datagramstart + SSP_CNTL_HDR_LEN);
    cv =  ((uchar *)gds_hdr) + sizeof(cap_gds_t);
    offset = sizeof(cap_gds_t);

    while ((cv) && (offset < gds_hdr->gds_len)) {
        switch (((cv_hdr_t *)cv)->cv_id) {
          case CAP_SAP_LIST:
            cv = parse_sap_cv(pak, peer,(cv_sap_t *) cv, &offset, &r);
            break;

          case CAP_MAC_EXCL:
            cv = parse_excl_cv(pak, peer,(cv_excl_t *) cv, &offset, &r);
            break;
            
          case CAP_NETBIOS_EXCL:
            cv = parse_excl_cv(pak, peer,(cv_excl_t *) cv, &offset, &r);
            break;

          case CAP_MAC_ADDR:
            cv = parse_mac_cv(pak, peer,(cv_mac_t *) cv, &offset, &r);
            break;

          case CAP_NB_NAME:
            cv = parse_nbname_cv(pak, peer,(cv_nbname_t *) cv, &offset, &r);
            break;

          default:
            PDEBUG("\nDLSw: Unknown run-time CV %x with length %d from %s",
                   ((cv_hdr_t *)cv)->cv_id, ((cv_hdr_t *)cv)->cv_len,
                   peer_str(peer,buff));
            /*
             * If the unknown control vector is the Vendor Id CV, then
             * it's a safe bet that the other station is disconnected, so
             * close the connection and let a new connection sequence
             * bring us back.
             */
            if (((cv_hdr_t *)cv)->cv_id == CAP_VENDOR_ID) {
                dlsw_peer_fsm(peer, CLOSE_CONNECTION);
                return;
            }

            /*
             * If cv_len is zero, this function will loop and keep
             * checking the same CV.
             */
            if (((cv_hdr_t *)cv)->cv_len == 0)
                return;

            offset = offset + ((cv_hdr_t *)cv)->cv_len;
            cv = cv + ((cv_hdr_t *)cv)->cv_len;
            break;

        } 
    }

    if (cv) {
        /*
         * Check ssp message length and gds header message length
         */
        if (gds_hdr->gds_len != offset) {
            PDEBUG("\nDLSw: gds_hdr->gds_len(%d) != offset(%d)",
                       gds_hdr->gds_len, offset);
            cv = NULL;
            offset = 0;
            r = INVALID_GDS_LEN;
        } else if (gds_hdr->gds_len != ssp_ptr->ssp_msg_len) {
            /* sanity check during development */
            PDEBUG("\nDLSw: gds_hdr->gds_len(%d) != ssp_ptr->msg_len(%d)",
                   gds_hdr->gds_len, ssp_ptr->ssp_msg_len);
        } 
    }

    if (cv)
        send_pos_rsp_id(pak, peer);
    else
        send_neg_rsp_id(pak, peer, offset, r);
    update_peer_cap (peer);
}


/*
 * parse_cap_ex_id()
 *
 * Returns TRUE if able to send CapEx Rsp
 *         FALSE if unable to send CapEx Rsp
 */

boolean parse_cap_ex_id (paktype *pak, peerentry *peer)
{
    ssp_hdr_t *ssp_ptr;
    cap_gds_t *gds_hdr;
    char *cv;
    ushort offset;
    ushort r;
    boolean rc = FALSE;

    ssp_ptr = (ssp_hdr_t *) pak->datagramstart;
    gds_hdr = (cap_gds_t *) (pak->datagramstart + SSP_CNTL_HDR_LEN);
    cv =  ((uchar *)gds_hdr) + sizeof(cap_gds_t);
    offset = sizeof(cap_gds_t); 
    cv = parse_required_cvs(pak, peer, cv, &offset, &r);
    if (cv)
        cv = parse_optional_cvs(pak, peer, cv, &offset, &r, gds_hdr->gds_len);

    if (cv) {
        /*
         * Check ssp message length and gds header message length
         */
        if (gds_hdr->gds_len != offset) {
            PDEBUG("\nDLSw: gds_hdr->gds_len(%d) != offset(%d)", 
                       gds_hdr->gds_len, offset);  
            cv = NULL;
            offset = 0;
            r = INVALID_GDS_LEN;
            send_neg_rsp_id(pak, peer, offset, r);
        } else if (gds_hdr->gds_len != ssp_ptr->ssp_msg_len) {
            /* sanity check during development */
            PDEBUG("\nDLSw: gds_hdr->gds_len(%d) != ssp_ptr->msg_len(%d)",
                   gds_hdr->gds_len, ssp_ptr->ssp_msg_len);
            datagram_done(pak);
        } else if ((!peer->cap.cap_sent) &&
                (peer->peer_type == PROM_PEER)) {
            rc = cap_learn_from_partner(peer);
            if (rc) {
                rc = send_pos_rsp_id(pak, peer);
                if (rc) {
                    peer->cap.cap_known = TRUE;
                    rc = send_capabilities(peer);
                    if (rc)
                        peer->cap.cap_sent = TRUE;
                }
            }
        } else {
            rc = send_pos_rsp_id(pak, peer);
            if (rc) {
                peer->cap.cap_known = TRUE;
		if (!peer->cap.cap_sent) {
		     rc = send_capabilities(peer);
		     if (rc)
		          peer->cap.cap_sent = TRUE;
		}
	    }
        }
    } else {
        send_neg_rsp_id(pak, peer, offset, r);
    }
    return(rc);
}


/*
 * parse_required_cvs()
 */

static char *parse_required_cvs (paktype *pak, peerentry *peer,
                                 char *cv, ushort *offset, ushort *r)
{
    cv = parse_vendor_id_cv(pak, peer, (cv_vendor_id_t *)cv, offset, r);
    if (cv)
        cv = parse_vers_num_cv(pak, peer, (cv_vers_num_t *)cv, offset, r);
    if (cv)
        cv = parse_ipw_cv(pak, peer, (cv_ipw_t *) cv, offset, r);
    if (cv)
        cv = parse_sap_cv(pak, peer,(cv_sap_t *) cv, offset, r);
    return(cv);
}


/*
 * parse_optional_cvs()
 */

static char *parse_optional_cvs (paktype *pak, peerentry *peer,
                                 char *cv, ushort *offset,
                                 ushort *r, ushort gds_len)
{
    char buff[DLSW_MAX_PEER_ID]; 
    boolean cisco_ctxt = TRUE; /* assume TRUE for backwards compat. */

    /*
     * Process optional CVs in order of appearance.
     */
    while ((cv) && (*offset < gds_len)) {
        switch (((cv_hdr_t *)cv)->cv_id) {
          case CAP_VERS_STRING:
            cv = parse_vstr_cv(pak, peer, (cv_vstr_t *) cv, offset, r);
            break;

          case CAP_NUM_TCP:
            cv = parse_tcp_cv(pak, peer,(cv_tcp_t *) cv, offset, r);
            break;

          case CAP_MAC_EXCL:
          case CAP_NETBIOS_EXCL:
            cv = parse_excl_cv(pak, peer,(cv_excl_t *) cv, offset, r);
            break;

          case CAP_MAC_ADDR:
            cv = parse_mac_cv(pak, peer,(cv_mac_t *) cv, offset, r);
            break;

          case CAP_NB_NAME:
            cv = parse_nbname_cv(pak, peer,(cv_nbname_t *) cv, offset, r);
            break;

          case CAP_LACK:
            cv = parse_lack_cv(pak, peer,(cv_lack_t *) cv,
                               offset, r, &cisco_ctxt);
            break;

          case CAP_PRIORITY:
            cv = parse_pri_cv(pak, peer,(cv_pri_t *) cv,
                              offset, r, &cisco_ctxt);
            break;

          case CAP_PEER_GRP:
            cv = parse_pgrp_cv(pak, peer,(cv_pgrp_t *) cv,
                               offset, r, &cisco_ctxt);
            break;

          case CAP_BORDER:
            cv = parse_border_cv(pak, peer,(cv_border_t *) cv,
                                 offset, r, &cisco_ctxt);
            break;

          case CAP_COST:
            cv = parse_cost_cv(pak, peer,(cv_cost_t *) cv,
                               offset, r, &cisco_ctxt);
            break;

          case CAP_CISCO_VERS:
            cv = parse_cisco_vers_cv(pak, peer, (cv_cisco_vers_t *) cv,
                                     offset, r, &cisco_ctxt);
            break;

          case CAP_VENDOR_CNTXT:
            cv = parse_vend_ctxt_cv(pak, peer, (cv_vctxt_t *) cv,
                                    offset, r, &cisco_ctxt);
            break;

          case CAP_SPOOF_AND_FRAG:
            cv = parse_spoof_and_seg_cv(pak, peer, (cv_biu_seg_t *) cv,
                                        offset, r, &cisco_ctxt);
            break;

          case CAP_XID_OP_SUPPORT:
            cv = parse_xid_op_cv(pak, peer, (cv_xid_op_t *) cv, 
                                 offset, r, &cisco_ctxt);
            break;

	  case CAP_PEER_TYPE:
	    cv = parse_peer_type_cv(pak, peer, (cv_peer_type_t *) cv,
				    offset, r, &cisco_ctxt);
	    break;

          default:
            PDEBUG("\nDLSw: Unknown CV %x with length %d from %s",
                   ((cv_hdr_t *)cv)->cv_id, ((cv_hdr_t *)cv)->cv_len,
                   peer_str(peer,buff));

            /*
             * If cv_len is zero, this function will loop and keep
             * checking the same CV.
             */
            if (((cv_hdr_t *)cv)->cv_len == 0)
                return(NULL);

            *offset = *offset + ((cv_hdr_t *)cv)->cv_len;
            cv = cv + ((cv_hdr_t *)cv)->cv_len;
            break;
        }
    }
    return(cv);
}


/*
 * parse_vendor_id_cv()
 */

static char *parse_vendor_id_cv (paktype *pak, peerentry *peer, 
                                 cv_vendor_id_t *cv, ushort *cv_offset, 
                                 ushort *reason)
{
    char buff[DLSW_MAX_PEER_ID];

    if (cv->cv_hdr.cv_id != CAP_VENDOR_ID) {
        PDEBUG("\nDLSw: Cap vendor_id cv_id %x invalid from %s",
                   cv->cv_hdr.cv_id, peer_str(peer, buff)); 
        *reason = NO_VENDOR_ID;
        return(NULL);
    }
    if (cv->cv_hdr.cv_len != sizeof(cv_vendor_id_t)) {
        PDEBUG("\nDLSw: Cap vendor_id cv_len %d invalid from %s",
                   cv->cv_hdr.cv_len, peer_str(peer, buff));
        *reason = INVALID_CV_LEN;
        return(NULL);
    }
    bcopy(cv->vendor_id, peer->cap.vendor_id, SNAP_OUIBYTES);
    *cv_offset = *cv_offset + sizeof(cv_vendor_id_t);
    return(((char *) cv) + sizeof(cv_vendor_id_t));
}


/*
 * parse_vers_num_cv()
 */

static char *parse_vers_num_cv (paktype *pak, peerentry *peer,
                                cv_vers_num_t *cv, ushort *cv_offset,
                                ushort *reason)
{
    char buff[DLSW_MAX_PEER_ID];

    if (cv->cv_hdr.cv_id != CAP_VERS_NUM) {
        PDEBUG("\nDLSw: Cap vers num cv_id %x invalid from %s",
                   cv->cv_hdr.cv_id, peer_str(peer, buff)); 
        *reason = NO_VERS_NUM;
        return(NULL);
    }
    if (cv->cv_hdr.cv_len != sizeof(cv_vers_num_t)) {
        PDEBUG("\nDLSw: Cap vers num cv_len %d invalid from %s",
                   cv->cv_hdr.cv_len, peer_str(peer, buff));
        *reason = INVALID_CV_LEN;
        return(NULL);
    }
    peer->cap.version_number = cv->vers_num;
    peer->cap.release_number = cv->rel_num;
    *cv_offset = *cv_offset + sizeof(cv_vers_num_t);
    return(((char *) cv) + sizeof(cv_vers_num_t));
}


/*
 * parse_ipw_cv()
 */

static char *parse_ipw_cv (paktype *pak, peerentry *peer,
                           cv_ipw_t *cv, ushort *cv_offset,
                           ushort *reason)
{
    char buff[DLSW_MAX_PEER_ID];

    if (cv->cv_hdr.cv_id != CAP_IPW) {
        PDEBUG("\nDLSw: Cap ipw cv_id %x invalid from %s",
                   cv->cv_hdr.cv_id, peer_str(peer, buff)); 
        *reason = NO_IPW;
        return(NULL);
    }
    if (cv->cv_hdr.cv_len != sizeof(cv_ipw_t)) {
        PDEBUG("\nDLSw: Cap ipw cv_len %d invalid from %s",
                   cv->cv_hdr.cv_len, peer_str(peer, buff));
        *reason = INVALID_CV_LEN;
        return(NULL);
    }
    peer->cap.initial_pacing_window = GETSHORT(&cv->window);
    *cv_offset = *cv_offset + sizeof(cv_ipw_t);
    return(((char *) cv) + sizeof(cv_ipw_t));
}


/*
 * parse_sap_cv()
 */

static char *parse_sap_cv (paktype *pak, peerentry *peer,
                           cv_sap_t *cv, ushort *cv_offset,
                           ushort *reason)
{
    char buff[DLSW_MAX_PEER_ID];

    if (cv->cv_hdr.cv_id != CAP_SAP_LIST) {
        PDEBUG("\nDLSw: Cap sap list cv_id %x invalid from %s",
                   cv->cv_hdr.cv_id, peer_str(peer, buff)); 
        *reason = NO_SAP_LIST;
        return(NULL);
    }
    if (cv->cv_hdr.cv_len != sizeof(cv_sap_t)) {
        PDEBUG("\nDLSw: Cap sap list cv_len %d invalid from %s",
                   cv->cv_hdr.cv_len, peer_str(peer, buff));
        *reason = INVALID_CV_LEN;
        return(NULL);
    }
    bcopy(cv->sap_vector, peer->cap.supp_saps, SAP_CV_BYTES); 
    *cv_offset = *cv_offset + sizeof(cv_sap_t);
    return((char *) cv + sizeof(cv_sap_t));
}


/*
 * parse_vstr_cv()
 */

static char *parse_vstr_cv (paktype *pak, peerentry *peer,
                           cv_vstr_t *cv, ushort *cv_offset,
                           ushort *reason)
{
    char buff[DLSW_MAX_PEER_ID];
    ulong string_len;

    if (cv->cv_hdr.cv_len < sizeof(cv_vstr_t)) {
        PDEBUG("\nDLSw: Cap vers str cv_len %d invalid from %s",
                   cv->cv_hdr.cv_len, peer_str(peer, buff));
        *reason = INVALID_CV_LEN;
        return(NULL);
    }
    string_len = cv->cv_hdr.cv_len - sizeof(cv_hdr_t);
    peer->cap.version_string = malloc(string_len + 1);
    if (!peer->cap.version_string) {
        *reason = INVALID_CV_LEN; /* pick'em */
        return(NULL);
    }
  
    bcopy(cv->vers_string, peer->cap.version_string, string_len);
    peer->cap.version_string[string_len] = '\0';
    *cv_offset = *cv_offset + cv->cv_hdr.cv_len;
    return(((char *) cv) + cv->cv_hdr.cv_len);
}


/*
 * parse_cisco_vers_cv()
 */

static char *parse_cisco_vers_cv (paktype *pak, peerentry *peer,
                                  cv_cisco_vers_t *cv, ushort *cv_offset,
                                  ushort *reason, boolean *cisco_ctxt)
{
    char buff[DLSW_MAX_PEER_ID];

    if (*cisco_ctxt == TRUE) {
    if (cv->cv_hdr.cv_len != sizeof(cv_cisco_vers_t)) {
            PDEBUG("\nDLSw: Cap cisco vers cv_len %d invalid from %s",
                   cv->cv_hdr.cv_len, peer_str(peer, buff));
        *reason = INVALID_CV_LEN;
        return(NULL);
    }
    peer->cap.cisco_version = GETSHORT(&cv->cisco_version);
}
    *cv_offset = *cv_offset + cv->cv_hdr.cv_len;
    return(((char *) cv) + cv->cv_hdr.cv_len);
}



/*
 * parse_vend_ctxt_cv()
 */

static char *parse_vend_ctxt_cv (paktype *pak, peerentry *peer,
                                 cv_vctxt_t *cv, ushort *cv_offset,
                                 ushort *reason, boolean *cisco_ctxt)
{
    char buff[DLSW_MAX_PEER_ID];

    if (cv->cv_hdr.cv_len != sizeof(cv_vctxt_t)) {
        PDEBUG("\nDLSw: Cap vendor context cv_len %d invalid from %s",
                   cv->cv_hdr.cv_len, peer_str(peer, buff));
        *reason = INVALID_CV_LEN;
        return(NULL);
    }
    if (!bcmp(lpeer->cap.vendor_id, cv->vendor_id, SNAP_OUIBYTES))
        *cisco_ctxt = TRUE;
    else
        *cisco_ctxt = FALSE;

    *cv_offset = *cv_offset + sizeof(cv_vctxt_t);
    return(((char *) cv) + sizeof(cv_vctxt_t));
}


/*
 * parse_tcp_cv()
 */

static char *parse_tcp_cv (paktype *pak, peerentry *peer,
                           cv_tcp_t *cv, ushort *cv_offset,
                           ushort *reason)
{
    char buff[DLSW_MAX_PEER_ID];

    if (cv->cv_hdr.cv_len != sizeof(cv_tcp_t)) {
        PDEBUG("\nDLSw: Cap num tcp supp cv_len %d invalid from %s",
                   cv->cv_hdr.cv_len, peer_str(peer, buff));
        *reason = INVALID_CV_LEN;
        return(NULL);
    }
    peer->cap.num_tcp_sessions = cv->num_tcp_sup;
    *cv_offset = *cv_offset + sizeof(cv_tcp_t);
    return((char *) cv + sizeof(cv_tcp_t));
}


/*
 * parse_excl_cv()
 */

static char *parse_excl_cv (paktype *pak, peerentry *peer,
                            cv_excl_t *cv, ushort *cv_offset,
                            ushort *reason)
{
    char buff[DLSW_MAX_PEER_ID];

    if (cv->cv_hdr.cv_len != sizeof(cv_excl_t)) {
        PDEBUG("\nDLSw: Cap exclusive supp cv_len %d invalid from %s",
                   cv->cv_hdr.cv_len, peer_str(peer, buff));
        *reason = INVALID_CV_LEN;
        return(NULL);
    }
    if (cv->cv_hdr.cv_id == CAP_MAC_EXCL)
        peer->cap.mac_exclusive = cv->excl;
    else
        peer->cap.nb_exclusive = cv->excl;
    *cv_offset = *cv_offset + sizeof(cv_excl_t);
    return((char *) cv + sizeof(cv_excl_t));
}


/*
 * parse_mac_cv()
 */

static char *parse_mac_cv (paktype *pak, peerentry *peer,
                           cv_mac_t *cv, ushort *cv_offset,
                           ushort *reason)
{
    macentry_t *macentry;
    char buff[DLSW_MAX_PEER_ID];

    if (cv->cv_hdr.cv_len != sizeof(cv_mac_t)) {
        PDEBUG("\nDLSw: Cap mac-addr cv_len %d invalid from %s",
                   cv->cv_hdr.cv_len, peer_str(peer, buff));
        *reason = INVALID_CV_LEN;
        return(NULL);
    }
    macentry = malloc(sizeof(macentry_t));
    if (!macentry) {
        return((char *) cv);
    }
    ieee_copy(cv->macaddr, macentry->macaddr);
    ieee_copy(cv->macmask, macentry->macmask);
    p_enqueue(&peer->cap.macQ, macentry);
    *cv_offset = *cv_offset + sizeof(cv_mac_t);
    return((char *) cv + sizeof(cv_mac_t));
}


/*
 * parse_nbname_cv()
 */

static char *parse_nbname_cv (paktype *pak, peerentry *peer,
                              cv_nbname_t *cv, ushort *cv_offset,
                              ushort *reason)
{
    nbentry_t *nbentry;
    char buff[DLSW_MAX_PEER_ID];

    if ((cv->cv_hdr.cv_len < sizeof(cv_hdr_t)) ||
        (cv->cv_hdr.cv_len > (sizeof(cv_hdr_t) + NETBIOS_MAX_NAME_LEN))) {
        PDEBUG("\nDLSw: Cap netbios name cv_len %d invalid from %s",
                   cv->cv_hdr.cv_len, peer_str(peer, buff));
        *reason = INVALID_CV_LEN;
        return(NULL);
    }
    nbentry = malloc(sizeof(nbentry_t));
    if (!nbentry) {
        return((char *) cv);
    }
    nbentry->len = cv->cv_hdr.cv_len - sizeof(cv_hdr_t);
    bcopy(cv->nbname, nbentry->nbname, nbentry->len);
    nbentry->nbname[nbentry->len] = '\0';
    peer_build_netbios_mask(nbentry);
    p_enqueue(&peer->cap.nbnameQ, nbentry);
    *cv_offset = *cv_offset + cv->cv_hdr.cv_len;
    return((char *) cv + cv->cv_hdr.cv_len);
}


/*
 * parse_lack_cv()
 */

static char *parse_lack_cv (paktype *pak, peerentry *peer,
                            cv_lack_t *cv, ushort *cv_offset,
                            ushort *reason, boolean *cisco_ctxt)
{
    char buff[DLSW_MAX_PEER_ID];

    if (*cisco_ctxt == TRUE) {
    if (cv->cv_hdr.cv_len != sizeof(cv_lack_t)) {
        PDEBUG("\nDLSw: Cap lack capable cv_len %d invalid from %s",
                   cv->cv_hdr.cv_len, peer_str(peer, buff));
        *reason = INVALID_CV_LEN;
        return(NULL);
    }
    peer->cap.local_ack = cv->lack;
}
    *cv_offset = *cv_offset + cv->cv_hdr.cv_len;
    return(((char *) cv) + cv->cv_hdr.cv_len);
}


/*
 * parse_pri_cv()
 */

static char *parse_pri_cv (paktype *pak, peerentry *peer,
                            cv_pri_t *cv, ushort *cv_offset,
                           ushort *reason, boolean *cisco_ctxt)
{
    char buff[DLSW_MAX_PEER_ID];

    if (*cisco_ctxt == TRUE) {
    if (cv->cv_hdr.cv_len != sizeof(cv_pri_t)) {
            PDEBUG("\nDLSw: Cap priority cv_len %d invalid from %s",
                   cv->cv_hdr.cv_len, peer_str(peer, buff));
        *reason = INVALID_CV_LEN;
        return(NULL);
    }
    peer->cap.priority = cv->priority;
}
    *cv_offset = *cv_offset + cv->cv_hdr.cv_len;
    return(((char *) cv) + cv->cv_hdr.cv_len);
}


/*
 * parse_pgrp_cv()
 */

static char *parse_pgrp_cv (paktype *pak, peerentry *peer,
                            cv_pgrp_t *cv, ushort *cv_offset,
                            ushort *reason, boolean *cisco_ctxt)
{
    int ii;
    char buff[DLSW_MAX_PEER_ID];

    if (*cisco_ctxt == TRUE) {
    if (cv->cv_hdr.cv_len != sizeof(cv_pgrp_t)) {
            PDEBUG("\nDLSw: Cap group cv_len %d invalid from %s",
                   cv->cv_hdr.cv_len, peer_str(peer, buff));
        *reason = INVALID_CV_LEN;
        return(NULL);
    }
    peer->cap.group = GETSHORT(&cv->group);
    if (peer->priority) {
        for (ii = 1; ii <= DLSW_PRIORITY_COUNT; ii++)
            peer->priority_peers[ii]->cap.group = GETSHORT(&cv->group);
    }
}
    *cv_offset = *cv_offset + cv->cv_hdr.cv_len;
    return(((char *) cv) + cv->cv_hdr.cv_len);
}


/*
 * parse_border_cv()
 */

static char *parse_border_cv (paktype *pak, peerentry *peer,
                              cv_border_t *cv, ushort *cv_offset,
                              ushort *reason, boolean *cisco_ctxt)
{
    int ii;
    char buff[DLSW_MAX_PEER_ID];

    if (*cisco_ctxt == TRUE) {
    if (cv->cv_hdr.cv_len != sizeof(cv_border_t)) {
            PDEBUG("\nDLSw: Cap border cv_len %d invalid from %s",
                   cv->cv_hdr.cv_len, peer_str(peer, buff));
        *reason = INVALID_CV_LEN;
        return(NULL);
    }
    peer->cap.border = cv->border;
    if (peer->priority) {
        for (ii = 1; ii <= DLSW_PRIORITY_COUNT; ii++)
            peer->priority_peers[ii]->cap.border = cv->border;
    }
}
    *cv_offset = *cv_offset + cv->cv_hdr.cv_len;
    return(((char *) cv) + cv->cv_hdr.cv_len);
}


/*
 * parse_cost_t()
 */

static char *parse_cost_cv (paktype *pak, peerentry *peer,
                            cv_cost_t *cv, ushort *cv_offset,
                            ushort *reason, boolean *cisco_ctxt)
{
    char buff[DLSW_MAX_PEER_ID];

    if (*cisco_ctxt == TRUE) {
    if (cv->cv_hdr.cv_len != sizeof(cv_cost_t)) {
        PDEBUG("\nDLSw: Cap peer cost cv_len %d invalid from %s",
                   cv->cv_hdr.cv_len, peer_str(peer, buff));
        *reason = INVALID_CV_LEN;
        return(NULL);
    }
    peer->cap.cost = cv->cost;
    }
    *cv_offset = *cv_offset + cv->cv_hdr.cv_len;
    return(((char *) cv) + cv->cv_hdr.cv_len);
}


/*
 * parse_spoof_and_seg_cv()
 */

static char *parse_spoof_and_seg_cv (paktype *pak, peerentry *peer,
                                      cv_biu_seg_t *cv, ushort *cv_offset,
                                      ushort *reason, boolean *cisco_ctxt)
{
    char buff[DLSW_MAX_PEER_ID];

    if (*cisco_ctxt == TRUE) {
        if (cv->cv_hdr.cv_len != sizeof(cv_biu_seg_t)) {
            PDEBUG("\nDLSw: Cap peer biu segment cv_len %d invalid from %s",
                       cv->cv_hdr.cv_len, peer_str(peer, buff));
            *reason = INVALID_CV_LEN;
            return(NULL);
        }
        peer->cap.spoof_and_seg = cv->spoof_and_seg;
    }
    *cv_offset = *cv_offset + cv->cv_hdr.cv_len;
    return(((char *) cv) + cv->cv_hdr.cv_len);
}


/*
 * parse_xid_op_cv()
 */

static char *parse_xid_op_cv (paktype *pak, peerentry *peer,
                              cv_xid_op_t *cv, ushort *cv_offset,
                              ushort *reason, boolean *cisco_ctxt)
{
    char buff[DLSW_MAX_PEER_ID];

    if (*cisco_ctxt == TRUE) {
        if (cv->cv_hdr.cv_len != sizeof(cv_xid_op_t)) {
            PDEBUG("\nDLSw: Cap peer xid op cv_len %d invalid from %s",
                       cv->cv_hdr.cv_len, peer_str(peer, buff));
            *reason = INVALID_CV_LEN;
            return(NULL);
        }
        peer->cap.xid_op_support = cv->xid_op_support;
    }
    *cv_offset = *cv_offset + cv->cv_hdr.cv_len;
    return(((char *) cv) + cv->cv_hdr.cv_len);
}

/*
 * parse_peer_type_cv()
 */

static char *parse_peer_type_cv (paktype *pak, peerentry *peer,
                                cv_peer_type_t *cv, ushort *cv_offset,
                                ushort *reason, boolean *cisco_ctxt)
{
   char buff[DLSW_MAX_PEER_ID];
   int media_lf;

   if (*cisco_ctxt == TRUE) {
       if (cv->cv_hdr.cv_len != sizeof(cv_peer_type_t)) {
           PDEBUG("\nDLSw: Cap peer type cv_len %d invalid from %s",
                      cv->cv_hdr.cv_len, peer_str(peer, buff));
           *reason = INVALID_CV_LEN;
           return(NULL);
    }

    peer->cap.peer_type = cv->peer_type;;

    if ((cv->peer_type == DEMAND_PEER) && (peer->peer_type == PROM_PEER)) {
    /*
     * Set the pod defaults.
     *
     * This is done because of a specific scenario related to
     * PROM and POD peers. When a peer receives a CUR_ex through
     * the border peer network, it creates a fake peer structure
     * to pass the CUR to the core for processing. This pod peer
     * structure is removed after 15 seconds, if no peer connect
     * request is received within that time. Now if we receive a
     * connect request, we consider it to be promiscuous and accept
     * the incoming request as PROM peer.
     *
     * Using the capabilities exchange, this pod information is
     * sent to the remote peer and the peer type and default parameters
     * are changed to that of pod-defaults.
     */
         PDEBUG("\nDLSw: Changing %s type from PROM_PEER to DEMAND_PEER (CAP_EX)",
                peer_str(peer, buff));
         peer->ple = lpeer->pod_defaults.ple;
         peer->peer_type = DEMAND_PEER;
         peer->cost = lpeer->pod_defaults.cost;
         peer->local_ack = lpeer->pod_defaults.local_ack;
         peer->max_tcp_qlen = lpeer->pod_defaults.max_tcp_qlen;
         peer->netbios_h_oacf = lpeer->pod_defaults.netbios_h_oacf;
         peer->netbios_b_oacf = lpeer->pod_defaults.netbios_b_oacf;
         peer->dlsw_lsap_out = lpeer->pod_defaults.dlsw_lsap_out;
         peer->keepalive_int = lpeer->pod_defaults.keepalive_int;
         peer->dlsw_dmac_outlist = lpeer->pod_defaults.dlsw_dmac_outlist;
         if (lpeer->pod_defaults.dlsw_dmac_out)
             ieee_copy(lpeer->pod_defaults.dlsw_dmac_out, peer->dlsw_dmac_out);
         else
             ieee_copy(zeromac, peer->dlsw_dmac_out);

         /* Update the POD largest frame size */
         if (peer->xport_type == DLSW_TCP)
                     media_lf = RC_LFINITIAL;
         else
                     media_lf = RC_LF516;

         if (lpeer->pod_defaults.largest_frame == RC_LFINITIAL) {
             peer->largest_frame = media_lf;
         } else if ((lpeer->pod_defaults.largest_frame > media_lf) &&
                    (peer->xport_type != DLSW_FST)) {
                     PDEBUG("%%Media only supports a largest frame of %d.  Lowering lf.\n",
                                    rc_code_to_bytes(media_lf));
                     peer->largest_frame = media_lf;
         } else {
              peer->largest_frame = lpeer->pod_defaults.largest_frame;
         }
         peer->largest_frame_def = lpeer->pod_defaults.largest_frame;

         /* If priority defined, create the priority peers */
         peer->priority = lpeer->pod_defaults.priority;
         if (!cap_learn_from_partner(peer)) {
             *reason = POD_FAIL;
             return NULL;
         }
      }
   }
   *cv_offset = *cv_offset + cv->cv_hdr.cv_len;
   return(((char *) cv) + cv->cv_hdr.cv_len);
}

/*
 * cap_learn_from_partner()
 *
 * Promiscuous peers must learn certain config info from
 * their partner's cap exchg message.
 */

static boolean cap_learn_from_partner (peerentry *peer)
{
    ulong ii;
    char buff[DLSW_MAX_PEER_ID];

    if (peer->peer_type == PROM_PEER)
        peer->priority = peer->cap.priority;
    if (peer->priority) {
        for (ii = 1; ii <= DLSW_PRIORITY_COUNT; ii++) {
            peer->priority_peers[ii] = create_peer(FALSE, 0, peer->ipaddr,
                       peer->peer_type, peer->ple, peer->xport_type,
                       peer->local_ack, peer->cost, peer->largest_frame,
                       peer->max_tcp_qlen, NULL, NULL, NULL, FALSE, NORMAL_MODE);
            if (!peer->priority_peers[ii]) {
                errmsg(PEER_NOMEM, "create promiscuous priority peer");
                return(FALSE);
            }
            peer->priority_peers[ii]->keepalive_int = peer->keepalive_int;
            peer->priority_peers[ii]->keepalive_count = 0;
            peer->priority_peers[ii]->took_def_keep = peer->took_def_keep;
            peer->priority_peers[ii]->priority_top = peer;
            peer->priority_peers[ii]->priority = FALSE;
            peer->priority_peers[ii]->netbios_h_oacf = peer->netbios_h_oacf;
            peer->priority_peers[ii]->netbios_b_oacf = peer->netbios_b_oacf;
            peer->priority_peers[ii]->dlsw_lsap_out = peer->dlsw_lsap_out;
            if (!peer_init_remote_xport(peer->priority_peers[ii], NULL,
                                        peer->largest_frame)) {
                PDEBUG("\nDLSw: peer_init_remote_xport failed for %s",
                       peer_str(peer, buff));
		mgd_timer_stop(&peer->conn_timer);
                return(FALSE);
            }
            peer->priority_peers[ii]->cap.group = peer->cap.group;
            peer->priority_peers[ii]->cap.border = peer->cap.border;
        }

        peer->priority_peers[1]->port = DLSW_P1_PORT;
        peer->priority_peers[2]->port = DLSW_P2_PORT;
        peer->priority_peers[3]->port = DLSW_P3_PORT;

    }
    return(TRUE);
}


/**
 ** --------------------------------------------------------------
 ** Miscellaneous Functions
 ** --------------------------------------------------------------
 **/


/*
 * get_ssp_pak()
 *
 * Doesn't matter if it's an Info or Control ssp header for peer.
 *
 */

paktype *get_ssp_pak (ulong pak_size)
{
    paktype *pak;
    ssp_hdr_t *ssp_ptr;

    pak = getbuffer(pak_size);
    if (!pak) {
        return(NULL);
    }
    pak->datagramsize = pak_size; /* may need to be adjusted later */
    ssp_ptr = (ssp_hdr_t *) pak->datagramstart;
    ssp_ptr->ssp_vers  = SSP_VERS; 
    ssp_ptr->ssp_hdr_len = SSP_CNTL_HDR_LEN;
    ssp_ptr->ssp_msg_type  = SSP_OP_capability_xchg;
    ssp_ptr->ssp_prot_id = SSP_PROTOCOL_ID;
    ssp_ptr->ssp_hdr_no = SSP_HDR_NUM;
    ssp_ptr->ssp_dlc_hdr_len = SSP_SNA_DLC_HDR_LEN;
    return(pak);
}

