/* $Id: lack2.c,v 3.3.62.3 1996/07/23 13:29:03 thille Exp $
 * $Source: /release/112/cvs/Xsys/srt/lack2.c,v $
 *------------------------------------------------------------------
 * lack2.c -- Local Acknowledgement Finite State Machine Glue
 *
 * Interface between the Lak2 FSM and the rest of the system.
 *
 * March 4, 1993, Claude Cartee and Eric B. Decker
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lack2.c,v $
 * Revision 3.3.62.3  1996/07/23  13:29:03  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.3.62.2  1996/05/17  12:12:46  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.48.2  1996/04/26  07:58:06  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.3.48.1  1996/04/03  21:58:43  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.62.1  1996/03/18  22:10:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  10:50:36  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  18:46:54  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  18:54:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:41:53  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:31:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:30:11  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/08/08  16:49:14  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.2  1995/08/07 05:43:39  richl
 * CSCdi38315:  makefile rework
 *              header changes for rif_util.c and tring_inline.h
 *              move dlsw_work to its own file, create libibm_util.a for
 *              utility routines used by dlsw and rsrb
 *
 * Revision 2.1  1995/06/07  23:00:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_generic.h"
#include "packet.h"
#include "address.h"
#include "../if/tring.h"
#include "../srt/rsrb.h"
#include "../llc2/llc2.h"
#include "../if/rif.h"
#include "../if/rif_inline.h"
#include "../util/fsm.h"
#include "../srt/lack_externs.h"
#include "../srt/lack2fsm.h"
#include "../srt/lack2_externs.h"
#include "../srt/srt_registry.h"
#include "../ibm/libibm_externs.h"
#include "../if/rif_util.h"

static dlc_rtn_t la2_llc2_open(dlc_db_t *dlc_db, lak_t *lackie);
static dlc_rtn_t la2_llc2_close(dlc_db_t *dlc_db, lak_t *lackie);
static dlc_rtn_t la2_llc2_busy(dlc_db_t *dlc_db, boolean busyit);
static dlc_rtn_t la2_llc2_abort(dlc_db_t *dlc_db);

/*
 * l a k 2 _ p r s t a t e
 *
 * Return a string that describes the lak2 fsm state.
 */

char *lak2_prstate(lak_t *lackie, l2st_t state)
{
    if (lackie)
	state = lackie->lak2_state;
    switch(state) {
      case l2st_undefined:	return("NO_ONES_HOME");
      case l2st_disconnect:	return("DISC");
      case l2st_localdiscwait:	return("LOCAL_DISC_WAIT");
      case l2st_remdiscwait:	return("REM_DISC_WAIT");
      case l2st_remwait:	return("REM_WAIT");
      case l2st_localwait:	return("LOCAL_WAIT");
      case l2st_connectpending:	return("CONN_PENDING");
      case l2st_connect:	return("CONNECT");
      case l2st_remqonwait:	return("QON_ACK_WAIT");
      case l2st_remqoffwait:	return("QOFF_ACK_WAIT");
      case l2st_quench: 	return("QUENCHED");
      default:			return("UNKNOWN");
    }
}


/*
 * l a k 2 _ p r i n p u t
 *
 * Return a string that describes an input major, minor
 */

char *lak2_prinput(l2ma_t major, int minor)
{
    switch(major) {
      case l2ma_net:
	switch(minor) {
	  case l2mi_net_rx_connreq:		return("CR");
	  case l2mi_net_rx_connpollreq:		return("CPR");
	  case l2mi_net_rx_connpollack:		return("CPA");
	  case l2mi_net_rx_connpollackreq:	return("CPAR");
	  case l2mi_net_rx_connwaitack:		return("CWA");
	  case l2mi_net_rx_discreq:		return("DR");
	  case l2mi_net_rx_discack:		return("DA");
	  case l2mi_net_rx_la2_qoff:		return("QOFF");
	  case l2mi_net_rx_la2_qon:		return("QON");
	  case l2mi_net_rx_la2_qoff_ack:	return("QOFF_ACK");
	  case l2mi_net_rx_la2_qon_ack:		return("QON_ACK");
	  case l2mi_net_rx_data:		return("NET_DATA");
	  case l2mi_net_abort:			return("NET_ABORT");
	  case l2mi_net_bc_linkupreq:		return("LUR");
	  case l2mi_net_bc_linkupprsp:		return("LUPR");
	  case l2mi_net_bc_linkdnreq:		return("LDR");
	  case l2mi_net_bc_linkdnprsp:		return("LDPR");
	  case l2mi_net_bc_qon:			return("BC_QON");
	  case l2mi_net_bc_qoff:		return("BC_QOFF");
	  default:				return("NET_UNKNOWN");
	}
	break;

      case l2ma_dlc:
	switch (minor) {
	  case l2mi_dlc_ind_failure:		return("DLC_IND_FAIL");
	  case l2mi_dlc_ind_closed:		return("DLC_IND_CLOSED");
	  case l2mi_dlc_ind_ext_conn:		return("DLC_IND_EXT CONN");
	  case l2mi_dlc_ind_opened:		return("DLC_IND_OPEN");
          case l2mi_dlc_ind_txqhigh:		return("DLC_TXQ_HIGH");
          case l2mi_dlc_ind_txqlow:		return("DLC_TXQ_LOW");
	  default:				return("DLC_UNKNOWN");
	}
	break;

      case l2ma_timer:
	switch(minor) {
	  case l2mi_timer_expired:		return("TIMER_EXPIRED");
	  case l2mi_timer_abort:		return("TIMER_ABORT");
	  default:				return("TIMER_UNKNOWN");
	}
	break;

      default:					return("UNKNOWN_MAJOR");
    }
}


/*
 * l a k 2 _ n e t _ c a t e g o r i z e ( )
 *
 * Given a raw network input code return the appropriate
 * lak2_fsm network minor.
 */

l2mi_net_t lak2_net_categorize(int opcode)
{
    switch (opcode) {
      case RSRB_OP_ConnectRequest:		return(l2mi_net_rx_connreq);
      case RSRB_OP_ConnectPollReq:		return(l2mi_net_rx_connpollreq);
      case RSRB_OP_ConnectPollAck:		return(l2mi_net_rx_connpollack);
      case RSRB_OP_ConnectPollAckAndReq:	return(l2mi_net_rx_connpollackreq);
      case RSRB_OP_ConnectWaitAck:		return(l2mi_net_rx_connwaitack);
      case RSRB_OP_DisconnectRequest:		return(l2mi_net_rx_discreq);
      case RSRB_OP_DisconnectAck:		return(l2mi_net_rx_discack);
      case RSRB_OP_LLC2DATA:			return(l2mi_net_rx_data);
      case RSRB_OP_SDLLC:			return(l2mi_net_rx_data);
      case RSRB_OP_QLLC:			return(l2mi_net_rx_data);
      case RSRB_OP_LA2_QON:			return(l2mi_net_rx_la2_qon);
      case RSRB_OP_LA2_QON_ACK:		       	return(l2mi_net_rx_la2_qon_ack);
      case RSRB_OP_LA2_QOFF:			return(l2mi_net_rx_la2_qoff);
      case RSRB_OP_LA2_QOFF_ACK:		return(l2mi_net_rx_la2_qoff_ack);
      case RSRB_OP_LINKUP_REQ:			return(l2mi_net_bc_linkupreq);
      case RSRB_OP_LINKDOWN_REQ:		return(l2mi_net_bc_linkdnreq);
      case RSRB_OP_LINKUP_PRSP:			return(l2mi_net_bc_linkupprsp);
      case RSRB_OP_LINKDOWN_PRSP:		return(l2mi_net_bc_linkdnprsp);
      case RSRB_OP_QUENCH_ON:			return(l2mi_net_bc_qon);
      case RSRB_OP_QUENCH_OFF:			return(l2mi_net_bc_qoff);
      default:					return(0);
    }
}


/*************************************************************************
 *
 * lak2 action primatives
 *
 * These primatives are used by the lak2 fsm action procedures to cause
 * certain kinds of things to happen in the rest of the system code.
 *
 *************************************************************************/

/*
 * l a k 2 _ s e n d _ n e t m s g
 *
 * Send a particular message to the remote end.  The message is assumed to be
 * one that doesn't require other rsrb header fields to be filled in (such as
 * trn, vrn, and offset).
 *
 * We assume that packets that are handed to us do not include the FCS.
 * Further prior to sending the message on its way we have to add TR_FCS_SIZE
 * to the packet because the other side assumes that all incoming frames will
 * have this added in.  We simply bump the datagram size appropriately assuming
 * that the other side doesn't care if the FCS is valid.
 *
 * If we are handed a packet via piggie_pak (god that's a fun name) we assume
 * that is headed in the right direction, has all the fields properly filled
 * in including the rif, and that we intercepted it and that is how it got
 * here.  In other words it was heading to the other side and it is no problem
 * for us to piggie back on it (gee I wonder where piggie_pak got its name).
 *
 * If we don't have a piggie_pak to piggie back on we must build one.  We
 * first assume that all op codes that we could possibly be interested in
 * sending are protocol units that will be consumed by the far end and won't
 * make it out on to any network media.  We use our lackie structure entity id
 * (which points at the local end) and reverse it.  The lackie entity id
 * points at our local host who we are partnering with.  We want to send a
 * packet pointed at the remote side, sourced by our local host.  We must play
 * games with the RIF we look up too because the remote side code does funny
 * things if we don't have a RIF (Lan Manager).  We must look up the RIF using
 * Local_Addr, Rem_Addr even though we are sending a packet to Rem_Addr from
 * Local_Addr.  This is because of the way rifs are stored for Local Ack (to
 * avoid dual lookups).
 */

void	lak2_send_netmsg (lak_t *lackie, int opcode)
{
    boolean	success;
    int		riflen, newriflen;
    tr_vhdrtype lhdr;
    trrif_t	*trh;
    riftype	*rif;
    paktype	*pak;
    tr_vpeertype *vrp;
    char peerid[MAX_PEER_NAME], *p;
    uchar *da, *sa;

    if (!lackie || !lackie->vrp) {
	lak2_butcher_piggie(lackie);
	if (lack_debug_err)
	    buginf("\nLOCACK: null lackie or vrp in send_netmsg");
	return;
    }
    vrp = lackie->vrp;
    if (!validmem(vrp) || !validmem(vrp->vrg)) {
	lak2_butcher_piggie(lackie);
	if (lack_debug_err)
	    buginf("\nLOCACK: vrp or vrp->vrg points to invalid memory.");
	return;
    }
    switch(lackie->dlc_db.type) {
      case DLC_LLC2:
	sa = (uchar *) &lackie->LACK_LOCAL_ADDR;
	da = (uchar *) &lackie->LACK_REM_ADDR;
	break;
      case DLC_CLS:
      case DLC_SDLC:
      case DLC_QLLC:
	da = (uchar *) &lackie->LACK_LOCAL_ADDR;
	sa = (uchar *) &lackie->LACK_REM_ADDR;
	break;
      default:
	lak2_butcher_piggie(lackie);
	if (lack_debug_err)
	    buginf("\nLOCACK: lak2_send_netmsg, unexpected dlc type %d", 
		   lackie->dlc_db.type);
	return;
    }
    rif = rif_lookup(sa, NULL, da, 0);
    /*
     * Do we have to take care of bit flipping?
     */
    if (!rif) {
	lak2_butcher_piggie(lackie);
	if (lack_debug_err)
	    buginf("\nLOCACK: no rif.  %s to peer %s/%s %s",
		   vr_op2str(opcode), vrpeer_id(vrp, peerid),
		   lack_id(lackie), lak2_prstate(lackie, 0));
	return;
    }
    newriflen = rif->length;
    pak = lackie->piggie_pak;
    if (pak) {
	/*
	 * preexisting packet.  Fudge things so we lay things down
	 * correctly
	 */
	lackie->piggie_pak = NULL;
	trh = (trrif_t *) pak->datagramstart;
	riflen = (trh->saddr[0] & TR_RII) ? (trh->rc_blth & RC_LTHMASK) : 0;
	p = (char *) (pak->datagramstart + TRING_ENCAPBYTES + riflen);
	pak->datagramstart += (riflen - newriflen);
	pak->datagramsize += (newriflen - riflen) + TR_FCS_SIZE;
    } else {
	pak = getbuffer(TRING_ENCAPBYTES + newriflen + SAP_HDRBYTES + TR_FCS_SIZE);
	if (!pak) {
	    return;
	}
	pak->datagramstart = pak->network_start;
	pak->datagramsize = TRING_ENCAPBYTES + newriflen + SAP_HDRBYTES + TR_FCS_SIZE;
	p = (char *) (pak->datagramstart + TRING_ENCAPBYTES + newriflen);
	((llc1_hdr_t *) p)->control = LLC1_UI;
    }
    trh = (trrif_t *) pak->datagramstart;
    trh->ac = AC_PRI0;
    trh->fc = FC_TYPE_LLC;
    PUTSHORT(&trh->daddr[0], GETSHORT(&lackie->LACK_REM_ADDR));
    PUTSHORT(&trh->daddr[2], GETSHORT(&lackie->address[8]));
    PUTSHORT(&trh->daddr[4], GETSHORT(&lackie->address[10]));
    PUTSHORT(&trh->saddr[0], GETSHORT(&lackie->LACK_LOCAL_ADDR));
    PUTSHORT(&trh->saddr[2], GETSHORT(&lackie->address[2]));
    PUTSHORT(&trh->saddr[4], GETSHORT(&lackie->address[4]));
    ((llc1_hdr_t *) p)->dsap = lackie->LACK_REM_SAP;
    ((llc1_hdr_t *) p)->ssap = lackie->LACK_LOCAL_SAP;
    if (newriflen) {
	bcopy((uchar *) rif->rif, (uchar *) &trh->rc_blth, newriflen);
	trh->saddr[0] |= TR_RII;
 	if ( (lackie->dlc_db.type == DLC_LLC2) || 
             (lackie->dlc_db.type == DLC_CLS) ) 
        {
	    trh->rc_dlf ^= RC_D;
        }
    }
    lhdr.rhdr_version = vrp->vrp_version;
    lhdr.op = opcode;
    lhdr.trn = lackie->remote_ring;
    lhdr.vrn = vrp->vrg->vrn;
    lhdr.len = pak->datagramsize;
    success = (*vrp->sender)(vrp, &lhdr, pak);
    if (!success) {
	p = "failed ";
    } else
	p = "";
    if ((lack_debug_packet || lack_debug_state || 
	 (lack_debug_err && !success)) &&
	  (opcode != RSRB_OP_LLC2DATA && opcode != RSRB_OP_SDLLC &&
	   opcode != RSRB_OP_QLLC))
 	buginf("\nLOCACK: %s%s to peer %s/%s [%s]", p, vr_op2str(opcode),
 	       vrpeer_id(vrp, peerid), lack_id(lackie),
 	       get_rif_str((uchar *) &trh->rc_blth, trprint_rifstr));
}


/*
 * l a k 2 _ b u t c h e r _ p i g g i e
 *
 * Kill off the piggie pak if any.
 */

void	lak2_butcher_piggie (lak_t *lackie)
{
    if (!lackie || !lackie->piggie_pak)
	return;
    vr_nukepak(lackie->piggie_pak);
    lackie->piggie_pak = NULL;
}


/*
 * l a 2 _ l l c 2 _ o p e n
 *
 * Cause LLC2 to do an open
 */

static dlc_rtn_t la2_llc2_open (dlc_db_t *dlc_db, lak_t *lackie)
{
    trrif_t *trh;
    int riflen;
    int dsap, ssap;
    llctype *llc2;
    paktype *pak;
    uchar *frameptr;
    hwaddrtype destMac, srcMac;

    if (!lackie || !dlc_db) {
	buginf("\nLOCACK: la2_llc2_open, null dlc_db or null lackie");
	return(DLC_RS_FAILED);
    }
    if (dlc_db->type != DLC_LLC2) {
	buginf("\nLOCACK: la2_llc2_open called with wrong dlc_db type");
	return(DLC_RS_FAILED);
    }	
    pak = lackie->piggie_pak;
    if (!pak) {
	buginf("\nLOCACK: la2_llc2_open with no piggie pak");
	return(DLC_RS_FAILED);
    }
    trh = (trrif_t *) pak->datagramstart;
    riflen = (trh->saddr[0] & TR_RII) ? (trh->rc_blth & RC_LTHMASK) : 0;
    frameptr = (uchar *) trh + TRING_ENCAPBYTES + riflen;
    dsap = ((llc1_hdr_t *) frameptr)->dsap;
    ssap = ((llc1_hdr_t *) frameptr)->ssap;

    snpa_init(&destMac, STATION_IEEE48, TARGET_UNICAST, trh->daddr);
    snpa_init(&srcMac, STATION_IEEE48, TARGET_UNICAST, trh->saddr);

    lackie->llc2_open_pak = pak;
    lackie->llc2_open_ok = TRUE;
    llc2 = llc2_open1
	(dsap, ssap, &destMac, &srcMac, pak->if_output,
	 LINK_LLC2, 8, NULL, TRUE, TRUE, FALSE, NULL_L3_PID,NULL_DLCI );
    if (!llc2) {
	lackie->llc2_open_pak = NULL;
	lackie->llc2_open_ok = FALSE;
	return(DLC_RS_FAILED);
    }
    lackie->llc = dlc_db->ptr.llc = llc2;
    llc2->l2_lakptr = lackie;
    pak->flags |= PAK_LACK_OUT;
    pak_enqueue(&lack_linkupQ, pak);
    lackie->piggie_pak = NULL;
    return(DLC_RS_PENDING);
}


/*
 * l a 2 _ l l c 2 _ c l o s e
 *
 * close down an llc2 connection
 */

static dlc_rtn_t la2_llc2_close (dlc_db_t *dlc_db, lak_t *lackie)
{
    dlc_rtn_t rc;

    if (!dlc_db || !lackie) {
	buginf("\nLOCACK: la2_llc2_close, null dlc_db or null lackie");
	return(DLC_RS_FAILED);
    }
    /*
     * We are forcing the call.  Nil the back pointer so LLC2 doesn't
     * think its doing an async close and call the state machine again.
     */
    if (dlc_db->type != DLC_LLC2) {
	if (lack_debug_err)
	    buginf("\nLOCACK: la2_llc2_close called with wrong dlc_db type.");
	return(DLC_RS_FAILED);
    }
    if (!dlc_db->ptr.llc) {
	if (lack_debug_err)
	    buginf("\nLOCACK: la2_llc2_close called with NULL llc2 ");
 	return(DLC_RS_FAILED);
    }
    if (!dlc_db->ptr.llc->l2_lakptr) {
	if (lack_debug_err)
	    buginf("\nLOCACK: la2_llc2_close called with llc2 pointing back to NULL lackie");
	return(DLC_RS_FAILED);
    }
    /*
     * llc2_close primitive modified to return one of:
     *     DLC_RS_OK - close issued during adm state, no network activity
     *                 required to close connection
     *     DLC_RS_PENDING - close requires network activity, status returned later
     *                 (i.e. DISC/UA exchange)
     *     DLC_RS_FAILED - NULL or invalid llc ptr
     */
    rc = llc2_close(dlc_db->ptr.llc);
    if (rc != DLC_RS_PENDING) {
	if (dlc_db->ptr.llc)
	    dlc_db->ptr.llc->l2_lakptr = NULL;
	dlc_db->ptr.llc = lackie->llc = NULL;
    }
    return(rc);
}


/*
 * l a 2 _ l l c 2 _ b u s y
 *
 * busy or unbusy the llc2 connection
 */

static dlc_rtn_t la2_llc2_busy (dlc_db_t *dlc_db, boolean busyit)
{
    if (!dlc_db) {
	buginf("\nLOCACK: la2_llc2_busy, null dlc_db");
	return(DLC_RS_FAILED);
    }
    if (dlc_db->type != DLC_LLC2) {
	buginf("\nLOCACK: la2_llc2_busy called with wrong dlc_db type.");
	return(DLC_RS_FAILED);
    }
    if (!dlc_db->ptr.llc)
	return(DLC_RS_FAILED);
    if (busyit)
	llc2_stateswitch(dlc_db->ptr.llc, SET_LOCAL_BUSY);
     else {
 	if ( (dlc_db->ptr.llc->rnr_disable)  &&
 	     (dlc_db->ptr.llc->state == LLS_ADM) )
 		llc2_release_UA(dlc_db->ptr.llc);
 	else
 	    llc2_stateswitch(dlc_db->ptr.llc, CLEAR_LOCAL_BUSY);
     }
    return(DLC_RS_OK);
}


/*
 * l a 2 _ l l c 2 _ a b o r t
 *
 * abort the llc2 connection
 */

static dlc_rtn_t la2_llc2_abort (dlc_db_t *dlc_db)
{
    if (!dlc_db) {
	buginf("\nLOCACK: la2_llc2_abort, null dlc_db");
	return(DLC_RS_OK);
    }
    if (dlc_db->type != DLC_LLC2) {
	buginf("\nLOCACK: la2_llc2_abort called with wrong dlc_db type.");
	return(DLC_RS_FAILED);
    }
    if (!dlc_db->ptr.llc)
	return(DLC_RS_FAILED);
    send_control(dlc_db->ptr.llc, DM_IND);
    llc2_goto_adm(dlc_db->ptr.llc, LC_ABORTED);
    return(DLC_RS_OK);
}


/*************************************************************************
 *
 * Timer Code.
 *
 *************************************************************************/

/*
 * l a k 2 _ S t a r t A c k T i m e r
 *
 * Start the Ack Timer for a given lack entity
 */

void	lak2_StartAckTimer(lak_t *lackie, int retries)
{
    lak_t *cur;
    leveltype level;
    
    for (cur = lack_timer_queue; cur; cur = cur->timer_queue_next)
	if (cur == lackie)
	    break;
    if (!cur) {			/* record not found , insertinto head */
	level = raise_interrupt_level(NETS_DISABLE);
	lackie->timer_queue_next = lack_timer_queue; /* point new next into old head */
	lack_timer_queue = lackie; /* point to new head record */
	reset_interrupt_level(level);
    }
    if (retries < 1)
	retries = 1;
    lackie->retry_count = retries;
    TIMER_START(lackie->timer_start, LAK2_ACK_TIME * ONESEC);
}


/*
 * l a k 2 _ R e s u m e A c k T i m e r
 *
 * Resume the Ack Timer for the given lack entity
 * using an exisitng retry count.
 */

void	lak2_ResumeAckTimer(lak_t *lackie)
{
    lak2_StartAckTimer(lackie, lackie->retry_count);
}


/*
 * l a k 2 _ S t o p A c k T i m e r
 *
 * Stop the Ack Timer for a given lack entity
 */

void	lak2_StopAckTimer(lak_t *lackie)
{
    lak_t *prev, *cur;
    leveltype level;

    if (!lack_timer_queue || !lackie)
	return;
    level = raise_interrupt_level(NETS_DISABLE);
    if (lack_timer_queue == lackie) {
	/*
	 * Entry at front of list
	 */
	lack_timer_queue = lackie->timer_queue_next;
	lackie->timer_queue_next = NULL;
    } else {
	/*
	 * Entry somewhere in the rest of the list
	 */
	for (prev = lack_timer_queue, cur = lack_timer_queue->timer_queue_next;
	      cur; prev = cur, cur = cur->timer_queue_next) {
	    if (cur == lackie) {
		prev->timer_queue_next = lackie->timer_queue_next;
		lackie->timer_queue_next = NULL;
		break;
	    }
	}
    }
    reset_interrupt_level(level);
}


/*
 * l a k 2 _ f s m
 *
 * Interface to the lak2 finite state machine
 *
 * major and minor indicate the event that has occured that we are
 * calling the fsm with.
 */

ushort lak2_fsm (lak_t *lackie, l2ma_t major, int minor)
{
    int     	fsm_rc;
    l2st_t	old_state;
    uchar addr[14];
    leveltype   level;

    if (lackie->fsm_ptr != lak2bc_fsm_table && lackie->fsm_ptr != lak2_fsm_table) {
	buginf("\nLOCACK: Internal error, fsm_ptr corrupted.");
	return(RSRB_OP_ERR);
    }
    if (lackie->lak2_fsm_level > LAK2_MAX_FSM_LEVELS) {
	buginf("\nLOCACK: Internal error, too many fsm levels.");
	buginf("\nLOCACK: %s state %s, event: %s", lack_id(lackie),
	       lak2_prstate(lackie, 0), lak2_prinput(major, minor) );
	return(RSRB_OP_ERR);
    }
    lackie->lak2_fsm_level++;
    old_state = lackie->lak2_state;
    if (lack_debug_packet || lack_debug_state)
	buginf("\nLOCACK: %s event: %s", lack_id(lackie),
	       lak2_prinput(major, minor) );
    fsm_rc = (int) fsm_execute ("LOCACK", lackie->fsm_ptr,
                        &((int)(lackie->lak2_state)), l2st_fsm_max,
                        lackie, major, minor,
			lack_debug_packet || lack_debug_state);
    if (fsm_rc < 0) {
	switch (fsm_rc) {
	  case FSM_RC_ERROR_INPUT:
	  case FSM_RC_INVALID_INPUT:
	    if (lack_debug_state || lack_debug_err)
		buginf("\nLOCACK: Invalid input: %s %s %s",
		       lack_id(lackie), lak2_prstate(NULL, old_state),
		       lak2_prinput(major, minor));
	    break;
	  case FSM_RC_UNKNOWN_STATE:
	    if (lack_debug_state || lack_debug_err)
		buginf("\nLOCACK: illegal state: %s %s %s states: %d %d",
		       lack_id(lackie), lak2_prstate(NULL, old_state),
		       lak2_prinput(major, minor), old_state, lackie->lak2_state);
	    break;
	  case FSM_RC_UNKNOWN_INPUT:
	    if (lack_debug_state || lack_debug_err)
		buginf("\nLOCACK: illegal input: %s %s %s input: %d %d",
		       lack_id(lackie), lak2_prstate(NULL, old_state),
		       lak2_prinput(major, minor), major, minor);
	    break;
	  case FSM_RC_UNEXPECTED_INPUT:
	    if (lack_debug_state || lack_debug_err)
		buginf("\nLOCACK: unexpected input: %s %s %s input: %d %d",
		       lack_id(lackie), lak2_prstate(NULL, old_state),
		       lak2_prinput(major, minor), major, minor);
	    break;
	  default:
	    if (lack_debug_state || lack_debug_err)
		buginf("\nLOCACK: unknown error rtn code %d: %s %s %s input: %d %d  states: %d %d",
		       fsm_rc, lack_id(lackie), lak2_prstate(NULL, old_state),
		       lak2_prinput(major, minor), major, minor, old_state, lackie->lak2_state);
	    break;
	}
	fsm_rc = RSRB_OP_ERR;
    } 

    lackie->lak2_fsm_level--;
    if (lackie->piggie_pak)
	lak2_butcher_piggie(lackie);
    level = raise_interrupt_level(NETS_DISABLE);
    if (lackie->lak2_fsm_level == 0 && lackie->lak2_state == l2st_disconnect) {
	char *lackid;

	lak2_StopAckTimer(lackie);
	bcopy(&(lackie->address[0]), &addr[0], 14);
	if (lack_debug_state)
	    lackid = lack_id(lackie);	/* get now for later debug */
	else
	    lackid = NULL;
	lack_remove_hash(lackie);
	if (!lack_addr_in_use(addr)) {
	    if (lack_debug_state)
		buginf("\nLOCACK: %s addr no longer in use, removing rif.", lackid);
	    unset_rif(NULL, 0, &addr[0], &addr[6], FALSE);
	}
    }
    reset_interrupt_level(level);
    return (fsm_rc);
}


/*
 * l a k 2 _ x i d _ o k
 *
 * MUST HAVE A PAK.  (pak of what?)
 */

boolean lak2_xid_ok (paktype *pak)
{
    lak_t *lackie;
    trrif_t	*trh;
    llc1_hdr_t *llc1;              /* pointer to llc-1 header */
    xid3_t     *xid3;		   /* pointer to xid information */
    int		riflen;
    boolean     xid_rc = TRUE;
    uchar       addr[14];

    lackie = trmac2lak(pak, addr, TRMAC2LAK_FROM_NET);
    if (!lackie)
	return(TRUE);
    /* If disconnecting, go ahead and pass XID thru */
    if (lackie->lak2_state < l2st_remwait) {
        if (lack_debug_state) {
	    buginf("\nLOCACK: %s, state %s",lack_id(lackie), lak2_prstate(lackie,0));
	    buginf("\n\t passing XID");
	}
	return(TRUE);
    }

    trh = (trrif_t *) pak->datagramstart;
    riflen = (trh->saddr[0] & TR_RII) ? trh->rc_blth & RC_LTHMASK : 0;
    llc1 = (llc1_hdr_t *) (pak->datagramstart + TRING_ENCAPBYTES + riflen);
    xid3 = (xid3_t *) ((uchar *) llc1 + SAP_HDRBYTES);
    /*
     * Verify that we have a packet big enough to be a Fmt 3
     * so we can check to see if it is a non-activation XID.
     */
    if ((pak->datagramsize - ((uchar *) xid3 - pak->datagramstart)) < sizeof(xid3_t))
	xid_rc = FALSE;
    else if ((xid3->xid3_fmt & XID_FMT_MASK) != XID_FMT_3)
	xid_rc = FALSE;
    else if ((xid3->xid3_esi & XID3_ESI_MASK) != XID3_ESI_NONACT)
	xid_rc = FALSE;
    if (xid_rc == FALSE) {
	if (lack_debug_state)
	    buginf("\nLOCACK: %s, state %s",lack_id(lackie), lak2_prstate(lackie,0));
	return(FALSE);
    }
    else
	return(TRUE);
}


/*
 * l a k 2 _ p r o c e s s _ n e t m s g
 *
 * Process an incoming network protocol message.
 */

void	lak2_process_netmsg(paktype *pak, int opcode, tr_ventrytype *vre, dlc_t type)
{
    trrif_t	*trh;
    lak_t	*lackie;
    int		offset, riflen;
    uchar	addr[14];
    ushort	rem_rn, rem_bn;
    char *why;
    leveltype level;

    if (!(pak->peer_ptr)) {
	if (lack_debug_err)
	    buginf("\nLOCACK: no peer for this netmsg");
	vr_nukepak(pak);
	return;
    }
    lackie = trmac2lak(pak, addr, TRMAC2LAK_FROM_NET);
    if (!lackie) {
	/*
	 * element doesn't exist.  Create one and pass to
	 * the state machine.
	 */
	trh = (trrif_t *)pak->datagramstart;
	riflen = (trh->saddr[0] & TR_RII) ? trh->rc_blth & RC_LTHMASK : 0;
	if (!riflen) {
	    if (lack_debug_err)
		buginf("\nLOCACK: no rif on LOCACK netmsg");
	    vr_nukepak(pak);
	    return;
	}
	/*
	 * This packet is coming in from the network heading towards the media.
	 * The rif searcher is built to scan rifs coming in from the media going
	 * towards other media or the network.  So we have to futz the RIF to
	 * make it appear like it is going out the network side.
	 */
	trh->rc_dlf ^= RC_D;
	offset = rif_match_offset((srbroute_t *) &trh->rc_blth, vre->rn, vre->bn,
		((tr_vpeertype *) pak->peer_ptr)->vrg->vrn, &rem_rn, &rem_bn, &why);
	trh->rc_dlf ^= RC_D;

	if (!offset) {
	    /*
	     * shouldn't ever happen
	     */
	    if (lack_debug_err) {
 		buginf("\nLOCACK: couldn't find remote ring on LOCACK netmsg1, %s, %s, ring group = %d, lrn = %d, lbn = %d, [%s]",
 		       vr_op2str(opcode), why, 
 		       ((tr_vpeertype *) pak->peer_ptr)->vrg->vrn,
 		       vre->rn, vre->bn, 
 		       get_rif_str((uchar *) &trh->rc_blth, trprint_rifstr));
	    }
	    vr_nukepak(pak);
	    return;
	}
 	lack2_pak2addr(pak, addr, TRMAC2LAK_FROM_NET);
	lackie = lack_create_entry(addr, pak->peer_ptr, vre->rn, rem_rn);
	if (!lackie) {
	    if (lack_debug_err)
		buginf("\nLOCACK: No memory for NetMsg lackie!");
	    vr_nukepak(pak);
	    return;
	}
	if (lack_debug_state)
	    buginf("\nLOCACK: (remote) created entity for %s", lack_id(lackie));
	/*
	 * copy the rif if any in too.
	 */
	lackie->rif.len = riflen;
	bcopy((uchar *) &trh->rc_blth, lackie->rif.data, riflen);
	trh->rc_dlf ^= RC_D;
	rif_update(NULL, 0, trh->daddr, trh->saddr, (srbroute_t *) &trh->rc_blth, RIF_AGED);
	lackie->dlc_db.ptr.control_block = NULL;
	lackie->dlc_db.type = type;
	    lackie->llc = NULL;
	lackie->dlc_handle = reg_invoke_lack_get_dlc_handle(type);
    }
    level = raise_interrupt_level(NETS_DISABLE);
    lackie->piggie_pak = pak;
    lak2_fsm(lackie, l2ma_net, lak2_net_categorize(opcode));
    reset_interrupt_level(level);
}


/* DLC HANDLES
 *
 * THESE REALLY BELONG IN THEIR OWN PROPER MODULES
 *
 * PHIL - 4/10/95
 */

static dlc_handle_t default_dlc_handle = {
    DLC_NONE,
    NULL, NULL, NULL, NULL
} ;

dlc_handle_t llc2_dlc_handle = {
    DLC_LLC2,
    la2_llc2_open,	la2_llc2_close,	la2_llc2_busy,	la2_llc2_abort
};

/* lack2_get_default_handle
 *
 * provide a default handle for lack_get_dlc_handle
 */
static dlc_handle_t *lack2_get_default_handle (void)
{
    buginf("\nLOCACK: lak2_process_netmsg, bad dlc type.");
    return(&default_dlc_handle);
}

/* llc2_get_dlc_handle
 *
 * provide the dlc handle for lack_get_dlc_handle
     */
static dlc_handle_t *llc2_get_dlc_handle (void)
{
    return(&llc2_dlc_handle);
}

void lack2_init (void)
{
    /*
     * Register LACK2 dlc "handles". 
     *
     * This really belongs with each media type, but that's
     * more time to figure out, if they really belong
     *
     * PHIL 4/10/95
     */
    reg_add_default_lack_get_dlc_handle(lack2_get_default_handle,
					"lack2_default_handle");
    reg_add_lack_get_dlc_handle(DLC_LLC2, llc2_get_dlc_handle, 
				"llc2_get_dlc_handle");
}
