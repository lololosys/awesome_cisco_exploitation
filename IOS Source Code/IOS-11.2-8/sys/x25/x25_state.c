/* $Id: x25_state.c,v 3.2.60.4 1996/08/28 13:23:42 thille Exp $
 * $Source: /release/112/cvs/Xsys/x25/x25_state.c,v $
 *------------------------------------------------------------------
 * x25_state.c -- X25 State transition routines
 *
 * February 1987, Greg Satz
 *
 * Copyright (c) 1987-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x25_state.c,v $
 * Revision 3.2.60.4  1996/08/28  13:23:42  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.60.3  1996/05/10  21:54:46  swenson
 * CSCdi55124:  x25 pvc connection fails to re-establish
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/04/02  17:14:32  swenson
 * CSCdi38602:  x25-lat PT: X25 vc brought down too early (getting
 * sys-2-notq error)
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  22:50:37  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.3  1996/03/13  02:13:49  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.24.2  1996/03/07  11:19:57  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.24.1  1996/02/20  21:56:41  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:08:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:54:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/10/02  22:02:04  carvalho
 * CSCdi39783:  CMNS call to ethernet destination fails if LLC2 not open.
 *
 * Revision 2.3  1995/08/30  19:52:19  rbatz
 * CSCdi30085:  SDLLC over RSRB/FST over X.25 crashes router due to stack
 *              overflow. Made 'fakepak' static instead of automatic.
 *
 * Revision 2.2  1995/07/20  07:38:28  hampton
 * Convert X.25 to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37392]
 *
 * Revision 2.1  1995/06/07 23:22:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "address.h"
#include "packet.h"
#include "config.h"
#include "logger.h"
#include "../x25/msg_x25.c"		/* Not a typo, see logger.h */
#include "../ui/debug.h"
#include "x25.h"
#include "x25_proto.h"
#include "x25_bfe.h"
#include "x25_switch.h"
#include "x25_route.h"
#include "x25_debug.h"
#include "x25_registry.h"
#include "lapb.h"
#include "connect.h"
#include "pad.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "x25_event.h"

/*
 * x25_api_notify
 * Inform api when we get an interesting packet
 * At this point qllc is the only customer
 */

static void  x25_api_notify(paktype *pak, lcitype *lci)
{
    x25clearpaktype	*p;
    x25_event_t		event;
    idbtype		*idb;
    uchar 		cause, diag;
    hwidbtype		*hwidb;

    if (lci == NULL)
    	return;
    
    idb = pak->if_input;
    p = (x25clearpaktype *)pak->datagramstart;

    event = X25_BOGUS_EVENT;
    cause = 0;
    diag  = 0;
    
    switch (p->x25_pti) {

    case X25_CALL_REQUEST:
	event = X25_INCALL_EVENT;
	break;			/* this only goes through if lci != NULL */
    case X25_CALL_CONNECTED:
	event = X25_ACCEPT_EVENT;
	    break;
    case X25_CLEAR:
	cause = p->x25_cause;
	diag  = p->x25_diag;
	event = X25_CLEAR_IND_EVENT;
	break;
    case X25_CLEAR_CONFIRMATION:
	event = X25_CLEAR_CONF_EVENT;
	break;
    case X25_RESET:
	cause = p->x25_cause;
	diag  = p->x25_diag;
	event = X25_RESET_IND_EVENT;
	break;
    case X25_RESET_CONFIRMATION:
	event = X25_RESET_CONF_EVENT;
	break;
    case X25_INTERRUPT:
	break;
    case X25_INTERRUPT_CONFIRMATION:
	break;
    case X25_RR:		/* modulo 128 */
	event = X25_RR_EVENT;
	break;
    case X25_RNR:		/* modulo 128 */
	event = X25_RNR_EVENT;
	break;
    default:
	/*
	 *  we may still have a modulo 8 data, RR or RNR packet
	 */
	hwidb = hwidb_or_null(idb);
	if (hwidb == NULL) {
	    event = X25_BOGUS_EVENT;
	    break;
	}

	if (hwidb->x25_modulo == X25_MOD8) {
	    switch (p->x25_pti & X25_DATA_MASK) {
	    case X25_RR:
		event = X25_RNR_EVENT;
		break;
	    case X25_RNR:
		event = X25_RNR_EVENT;
		break;
	    default:
		break;
	    }
	}
			
    }
    if (event == X25_BOGUS_EVENT)
    	return;

    reg_invoke_media_x25_notify_event(lci->lci_linktype, lci, 
				      event, cause, diag, pak);

}

/*
 * x25_r1
 * Packet level ready
 */

void x25_r1 (
    register hwidbtype *idb,
    register paktype *pak,
    register lcitype *lci)
{
    register packettype *p = (packettype *)pak->datagramstart;
    uchar *c = (uchar *)p + X25HEADERBYTES;
    register considbtype *considb;

    considb = get_lci_considb(lci);

    if (ISCMNS_IEEE(idb)) {
	if (p->x25_pti == X25_RESTART && LCI(p) == 0) {

	    if (lci == NULL) {
		lci = cons_restart_lci(idb, pak);
		if (lci != NULL)
		    (*x25_state_table[lci->lci_state])(idb, pak, lci);
		else
		    datagram_done(pak);
		return;
	    }
	    considb->x25_restartsrcvd++;
	    idb->x25_restartsrcvd++;
	    if (x25_debug || x25event_debug)
		x25_prpack(idb, pak, NULL, 'I');
	    if (pak->datagramsize < X25HEADERBYTES + X25_SCAUSE_DATA ||
		pak->datagramsize > X25HEADERBYTES + X25_CAUSE_DATA) {
		datagram_done(pak);
		return;
	    }
	    if (ISDCE(idb, considb) &&
		!IS_X25_RESTART_DTE_ORIGINATED(c)) {
		x25_sendcause(idb, lci, X25_RESTART,
			X25_RESTART_LOCAL_PROC_ERR,
			X25_DIAG_ILLEGAL_DTE_CAUSE_CODE,NULL);
		datagram_done(pak);
		return;
	    }

            /* NmEvent Hook 5 */
            X25EventRx(idb, lci, p);

	    /*
	     *  set a valid configuration
	     */
	    (void) x25_valid_config(idb, TRUE);

	    TIMER_STOP(lci->lci_timer);
	    lci->lci_state = X25_R1;
	    cons_restart(idb, lci, pak, TRUE);
	    datagram_done(pak);
	} else if (p->x25_pti == X25_RESTART_CONFIRMATION && LCI(p) == 0) {
	    if (lci == NULL) {
		lci = cons_restart_lci(idb,pak);
		if (lci != NULL)
		    (*x25_state_table[lci->lci_state])(idb, pak, lci);
		else
		    datagram_done(pak);
		return;
	    }
	    if (x25_debug || x25event_debug)
		x25_prpack(idb, pak, NULL, 'I');
	    if (pak->datagramsize != X25HEADERBYTES) {
		datagram_done(pak);
		return;
	    }
	    considb->x25_upstate = FALSE;
	    x25_sendcause(idb, lci, X25_RESTART, X25_RESTART_LOCAL_PROC_ERR,
			  X25_DIAG_INVALID_PTI_R1,NULL);
	    datagram_done(pak);
	} else {
	    if (lci == NULL)
		lci = x25_lci_lookup(idb, LCI(p), pak->llc2_cb);
	    if (lci == NULL) {
		lci = x25_lci_enter(idb->firstsw, LCI(p), pak->llc2_cb);
		if (lci == NULL) {
		    datagram_done(pak);
		    return;
		}
		lci->lci_outgoing = FALSE;
		lci->lci_llc2 = pak->llc2_cb;
	   }
          /* NmEven Hook 6, check if Reset or Clear received */
           if (p->x25_pti == X25_CLEAR || p->x25_pti == X25_RESET)
                X25EventRx(idb, lci, p);

	   if (x25_debug || x25event_debug)
	       x25_prpack(idb, pak, lci, 'I');
	   (*x25_state_table[lci->lci_state])(idb, pak, lci);
	}
    } else { /* serial */
	if (p->x25_pti == X25_RESTART && LCI(p) == 0) {
	    idb->x25_restartsrcvd++;
	    idb->x25_unexp_restarts_rx++;
	    if (*c == X25_RESTART_LOCAL_PROC_ERR)
		idb->x25_errors_accused++;
	    if (x25_debug || x25event_debug)
		x25_prpack(idb, pak, NULL, 'I');
	    if (pak->datagramsize < X25HEADERBYTES +
		(ISDCE(idb, considb) ? X25_SCAUSE_DATA : X25_CAUSE_DATA)) {
		if (ISDCE(idb, considb))
		    x25_senddiag(idb, pak, X25_DIAG_PACKET_TOO_SHORT, NULL);
                else
                    DteProtocolViolation(lci, idb, X25_DIAG_PACKET_TOO_SHORT);

		datagram_done(pak);
		return;
	    }
            else {
            }
	    if (pak->datagramsize > X25HEADERBYTES + X25_CAUSE_DATA) {
		if (ISDCE(idb, considb))
		    x25_senddiag(idb, pak, X25_DIAG_PACKET_TOO_LONG, NULL);
                else
                    DteProtocolViolation(lci, idb, X25_DIAG_PACKET_TOO_LONG);

		datagram_done(pak);
		return;
	    }
	    if (ISDCE(idb, considb) &&
		!IS_X25_RESTART_DTE_ORIGINATED(c)) {
		if (ISDCE(idb, considb))
		    x25_senddiag(idb, pak, X25_DIAG_ILLEGAL_DTE_CAUSE_CODE,
				 NULL);
                else
                    DteProtocolViolation(lci, idb, X25_DIAG_ILLEGAL_DTE_CAUSE_CODE);
 
		datagram_done(pak);
		return;
	   }

	    TIMER_STOP(idb->x25_timer);
	    idb->x25_state = X25_R1;
	    idb->x25_upstate = TRUE;
	    reg_invoke_x25Restart_event(idb);
	    idb->x25_snmp_restart_tx = TRUE;
	    idb->x25_cause = X25_RESTART_NETWORK_OPERATIONAL;
	    idb->x25_diag = X25_DIAG_REMOTE_PROTOCOL;

            /* NmEvent Hook 5 */
            X25EventRx(idb, lci, p); 

	    x25_restart(idb, TRUE);
	    datagram_done(pak);
	} else if (p->x25_pti == X25_RESTART_CONFIRMATION && LCI(p) == 0) {
	    if (x25_debug || x25event_debug)
		x25_prpack(idb, pak, NULL, 'I');
	    x25_sendcause(idb, 0, X25_RESTART, X25_RESTART_LOCAL_PROC_ERR,
			  X25_DIAG_INVALID_PTI_R1, NULL);
	    idb->x25_state = ISDTE(idb, considb) ? X25_R2 : X25_R3;
	    datagram_done(pak);
	} else if (LCI(p) == 0) {

		/*
		 *  determine what diagnostic to issue
		 */
		switch (p->x25_pti) {
		  case X25_DATA:	/* packet too short or modulo 128 */
		    if (pak->datagramsize < X25HEADERBYTES) {
                      if (ISDCE(idb, considb))
			x25_senddiag(idb, pak, X25_DIAG_PACKET_TOO_SHORT, NULL);
                      else
                          DteProtocolViolation(lci, idb, X25_DIAG_PACKET_TOO_SHORT);

			break;
		    }
		    /* FALL THROUGH */
		  case X25_CALL_REQUEST:
		  case X25_CALL_CONNECTED:
		  case X25_CLEAR:
		  case X25_CLEAR_CONFIRMATION:
		  case X25_RESET:
		  case X25_RESET_CONFIRMATION:
		  case X25_INTERRUPT:
		  case X25_INTERRUPT_CONFIRMATION:
		  case X25_RR:		/* modulo 128 */
		  case X25_RNR:		/* modulo 128 */
		    x25_senddiag(idb, pak, X25_DIAG_PACKET_UNASS_LC, NULL);
		    break;
		  case X25_DIAGNOSTIC:
		    break;	/* we shouldn't get this */
		  case X25_REGISTRATION:
		  case X25_REGISTRATION_CONFIRMATION:
		  case X25_DTE_REJ:
		  default:
		    /*
		     *  we may still have a modulo 8 data, RR or RNR packet
		     */
		    if (idb->x25_modulo == X25_MOD8 &&
			(((p->x25_pti & X25_DATA_MASK) == X25_DATA) ||
			 ((p->x25_pti & X25_FLOW_MASK) == X25_RR) ||
			 ((p->x25_pti & X25_FLOW_MASK) == X25_RNR))) {
                      if (ISDCE(idb, considb))
			x25_senddiag(idb, pak, X25_DIAG_PACKET_UNASS_LC, NULL);
                      else
                          DteProtocolViolation(lci, idb, X25_DIAG_PACKET_UNASS_LC); 

                          
		    } else {
                      if (ISDCE(idb, considb))
			x25_senddiag(idb, pak, X25_DIAG_UNIDENT_PACKET, NULL);
                      else
                          DteProtocolViolation(lci, idb, X25_DIAG_UNIDENT_PACKET); 

		    }
		}
	    datagram_done(pak);
	} else {
	    if (lci == NULL)
		lci = x25_lci_lookup(idb, LCI(p), NULL);
	    if (lci == NULL) {
		lci = x25_lci_enter(idb->firstsw, LCI(p), NULL);
		if (lci == NULL) {
		    datagram_done(pak);
		    return;
		}
		lci->lci_outgoing = FALSE;
	    }

            /* NmEven Hook 6, check if Reset or Clear received */
            if (p->x25_pti == X25_CLEAR || p->x25_pti == X25_RESET)
                 X25EventRx(idb, lci, p);

	    if (x25_debug || x25event_debug)
		x25_prpack(idb, pak, lci, 'I');
	    if (ISBFE(idb)) {
		/*
		 *  BFE does not support the D or Q bit procedures
		 */
		p->x25_q = 0;
		p->x25_d = 0;
	    }
	    if (IS_LCI_PVC(idb, lci->lci_lci) &&
		(p->x25_pti == X25_CALL_REQUEST ||
		 p->x25_pti == X25_CALL_CONNECTED ||
		 p->x25_pti == X25_CLEAR ||
		 p->x25_pti == X25_CLEAR_CONFIRMATION)) {
		/*
		 *  per Table C-4/X.25, discard packet types that are invalid
		 *  for permanent virtual circuits; RESET the PVC if it's in
		 *  state D1
		 */
		if (x25_debug || x25event_debug)
		    x25_prpack(idb, pak, lci, 'I');
		datagram_done(pak);
		if (lci->lci_state == X25_D1)
		    x25_sendcause(idb, lci, X25_RESET,
				  X25_RESET_LOCAL_PROC_ERR,
				  X25_DIAG_INVALID_PTI_PVC, NULL);
	    } else {
		(*x25_state_table[lci->lci_state])(idb, pak, lci);
	    }
	}
    }
}

/*
 * x25_r2_r3
 * DTE restart request
 * DCE restart indication
 */

void x25_r2_r3 (
    register hwidbtype *idb,
    register paktype *pak,
    register lcitype *lci)
{
    register packettype *p = (packettype *)pak->datagramstart;
    uchar *c = (uchar *)p + X25HEADERBYTES;
    register considbtype *considb;

    considb = get_lci_considb(lci);

    if (x25_debug || x25event_debug)
	x25_prpack(idb, pak, lci, 'I');
    if (ISBFE(idb)) {
	/*
	 *  BFE does not support the D or Q bit procedures
	 */
	p->x25_q = 0;
	p->x25_d = 0;
    }
    if (ISCMNS_IEEE(idb)) {
	if (p->x25_pti == X25_RESTART && LCI(p) == 0) {
	    considb->x25_restartsrcvd++;
	    idb->x25_restartsrcvd++;
	    if (pak->datagramsize < X25HEADERBYTES + X25_SCAUSE_DATA) {
		if (ISDCE(idb, considb))
		    x25_sendcause(idb, lci, X25_RESTART,
			X25_RESTART_LOCAL_PROC_ERR, X25_DIAG_PACKET_TOO_SHORT,
			NULL);
		datagram_done(pak);
		return;
	    }
	    if (pak->datagramsize > X25HEADERBYTES + X25_CAUSE_DATA) {
		if (ISDCE(idb, considb))
		    x25_sendcause(idb, lci, X25_RESTART,
			X25_RESTART_LOCAL_PROC_ERR, X25_DIAG_PACKET_TOO_LONG,
			NULL);
		datagram_done(pak);
		return;
	    }

	    /*
	     *  set a valid configuration
	     */
	    (void) x25_valid_config(idb, TRUE);

            /* NmEvent Hook 5 */
            X25EventRx(idb, lci, p); 

	    lci->lci_state = X25_R1; 
	    TIMER_STOP(lci->lci_timer);
	    cons_restart(idb, lci, pak, TRUE);
	} else if (p->x25_pti == X25_RESTART_CONFIRMATION && LCI(p) == 0){
	    /*
	     *  set a valid configuration
	     */
	    (void) x25_valid_config(idb, TRUE);

	    lci->lci_state = X25_R1; 
	    TIMER_STOP(lci->lci_timer);
	    cons_restart(idb, lci, pak, FALSE);
	}
	datagram_done(pak);
    } else { /* serial */
	if (pak->datagramsize < X25HEADERBYTES) {
	    ;	/* EMPTY */
	} else if (p->x25_pti == X25_RESTART && LCI(p) == 0) {
	    idb->x25_restartsrcvd++;
	    if (*c == X25_RESTART_LOCAL_PROC_ERR)
		idb->x25_errors_accused++;
	    idb->x25_retransmit = 0;
	    if (pak->datagramsize < X25HEADERBYTES +
		    (ISDCE(idb, considb) ? X25_SCAUSE_DATA : X25_CAUSE_DATA)) {
		x25_sendcause(idb, 0, X25_RESTART,
			X25_RESTART_LOCAL_PROC_ERR, X25_DIAG_PACKET_TOO_SHORT,
			NULL);
		datagram_done(pak);
		return;
	    }
	    if (pak->datagramsize > X25HEADERBYTES + X25_CAUSE_DATA) {
		x25_sendcause(idb, 0, X25_RESTART,
			X25_RESTART_LOCAL_PROC_ERR, X25_DIAG_PACKET_TOO_LONG,
			NULL);
		datagram_done(pak);
		return;
	    }

            /* NmEvent Hook 5 */
            X25EventRx(idb, lci, p); 

	    idb->x25_state = X25_R1;
	    TIMER_STOP(idb->x25_timer);
	    idb->x25_upstate = TRUE;
	    idb->x25_cause = X25_RESTART_NETWORK_OPERATIONAL;
	    idb->x25_diag = X25_DIAG_NO_INFO;
	    x25_restart(idb, FALSE);
	} else if (p->x25_pti == X25_RESTART_CONFIRMATION && LCI(p) == 0) {
	    if (BFE_BROKEN(idb) &&
		((pak->datagramsize < X25HEADERBYTES) ||
		 (pak->datagramsize > X25HEADERBYTES))) {
		datagram_done(pak);
		return;
	    }
	    idb->x25_retransmit = 0;
	    if (pak->datagramsize < X25HEADERBYTES) {
		x25_sendcause(idb, 0, X25_RESTART,
			X25_RESTART_LOCAL_PROC_ERR, X25_DIAG_PACKET_TOO_SHORT,
			NULL);
		datagram_done(pak);
		return;
	    }
	    if (pak->datagramsize > X25HEADERBYTES) {
		x25_sendcause(idb, 0, X25_RESTART,
			X25_RESTART_LOCAL_PROC_ERR, X25_DIAG_PACKET_TOO_LONG,
			NULL);
		datagram_done(pak);
		return;
	    }

	    idb->x25_state = X25_R1;
	    TIMER_STOP(idb->x25_timer);
	    idb->x25_upstate = TRUE;
	    idb->x25_cause = X25_RESTART_NETWORK_OPERATIONAL;
	    idb->x25_diag = X25_DIAG_NO_INFO;
	    x25_restart(idb, FALSE);
	} else if (LCI(p) == 0) {

		/*
		 *  determine if a diagnostic should be issued
		 */
		switch (p->x25_pti) {
		  case X25_CALL_REQUEST:
		  case X25_CALL_CONNECTED:
		  case X25_CLEAR:
		  case X25_CLEAR_CONFIRMATION:
		  case X25_RESET:
		  case X25_RESET_CONFIRMATION:
		  case X25_INTERRUPT:
		  case X25_INTERRUPT_CONFIRMATION:
		  case X25_DATA:	/* modulo 128 */
		  case X25_RR:		/* modulo 128 */
		  case X25_RNR:		/* modulo 128 */
		    x25_senddiag(idb, pak, X25_DIAG_PACKET_UNASS_LC, NULL);
		    break;
		  case X25_DIAGNOSTIC:
		    break;	/* we shouldn't get this */
		  case X25_REGISTRATION:
		  case X25_REGISTRATION_CONFIRMATION:
		  case X25_DTE_REJ:
		    break;	/* unidentified packet type */
		  default:
		    /*
		     *  we may still have a modulo 8 data, RR or RNR packet
		     */
		    if (idb->x25_modulo == X25_MOD8 &&
			(((p->x25_pti & X25_DATA_MASK) == X25_DATA) ||
			 ((p->x25_pti & X25_FLOW_MASK) == X25_RR) ||
			 ((p->x25_pti & X25_FLOW_MASK) == X25_RNR))) {
                   if (ISDCE(idb, considb))
			x25_senddiag(idb, pak, X25_DIAG_PACKET_UNASS_LC, NULL);
                   else
                       DteProtocolViolation(lci, idb, X25_DIAG_PACKET_UNASS_LC); 

		    }
		}
	    }
	datagram_done(pak);
    }
}

/*
 * x25_p1
 * Ready
 */

void x25_p1 (
    register hwidbtype *idb,
    register paktype *pak,
    register lcitype *lci)
{
    register packettype *p = (packettype *)pak->datagramstart;
    uchar *c, cause, diag;
    uchar caddr[2*X25_ADDRESS_LENGTH];
    int size, faclen;
    register considbtype *considb;

    considb = get_lci_considb(lci);
    
    c = (uchar *)p + X25HEADERBYTES;
    if (pak->datagramsize < X25HEADERBYTES) {
	x25_sendcause(idb, lci, X25_CLEAR,
		X25_CLEAR_LOCAL_PROC_ERR, X25_DIAG_PACKET_TOO_SHORT, NULL);
	datagram_done(pak);
    } else if (p->x25_pti == X25_CALL_REQUEST) {
	idb->x25_callsrcvd++;

	/*
	 *  parse the addresses
	 */
	lci->lci_src_addr.length = (*c >> 4) & 0x0f;
	lci->lci_dst_addr.length = *c & 0x0f;
	lci->lci_src_addr.type = ADDR_X121;
	lci->lci_dst_addr.type = ADDR_X121;
	size = bcdlen_inline(*c++);
	bcdtochar(c, caddr,
		  lci->lci_src_addr.length + lci->lci_dst_addr.length);
	bcopy(caddr, lci->lci_dst_addr.x121_addr, lci->lci_dst_addr.length);
	bcopy(&caddr[lci->lci_dst_addr.length],
	      lci->lci_src_addr.x121_addr, lci->lci_src_addr.length);

	faclen = *(c + size);

	/*
	 *  validate the Call packet
	 */
	if (ISDCE(idb, considb) ?
		((LCI(p) >= idb->x25_lic) && (LCI(p) <= idb->x25_hic)) :
		((LCI(p) >= idb->x25_loc) && (LCI(p) <= idb->x25_hoc))) {
	    x25_sendcause(idb, lci, X25_CLEAR, X25_CLEAR_LOCAL_PROC_ERR,
		X25_DIAG_CALL_ON_1WAY_LC, NULL);
	    idb->x25_callsrcvdfailed++;
	    datagram_done(pak);
	} else if ((pak->datagramsize < X25HEADERBYTES + X25_CALL_DATA) ||
	    (pak->datagramsize <
		X25HEADERBYTES + X25_CALL_DATA + size + faclen)) {
	    x25_sendcause(idb, lci, X25_CLEAR, X25_CLEAR_LOCAL_PROC_ERR,
		X25_DIAG_PACKET_TOO_SHORT, NULL);
	    idb->x25_callsrcvdfailed++;
	    if ((x25_debug || x25event_debug) &&
		(x25_vcnum == 0 || x25_vcnum == lci->lci_lci))
		buginf("\nCALL shorter than needed");
	    datagram_done(pak);
	} else if (!x25_parsefacility(idb, lci, c + size, X25_CALL_REQUEST,
				      &cause, &diag, X25_ROLE_CALLED_DTE)) {
	    x25_sendcause(idb, lci, X25_CLEAR, cause, diag, NULL);
	    idb->x25_callsrcvdfailed++;
	    if ((x25_debug || x25event_debug) &&
		(x25_vcnum == 0 || x25_vcnum == lci->lci_lci))
		buginf("\nFacility failure: cause %d diagnostic %d",
		    cause, diag);
	    datagram_done(pak);
	} else if (x25_fix_flow_fac(lci) && ISBFE(idb)) {
	    cause = X25_CLEAR_INCOMPAT_DEST;
	    diag = X25_DIAG_CALL_ERROR;
	    x25_sendcause(idb, lci, X25_CLEAR, cause, diag, NULL);
	    idb->x25_callsrcvdfailed++;
	    if ((x25_debug || x25event_debug) &&
		(x25_vcnum == 0 || x25_vcnum == lci->lci_lci))
		buginf("\nBad DDN flow control values");
	    datagram_done(pak);
	} else if (pak->datagramsize >
			X25HEADERBYTES + X25_CALL_DATA + size + faclen +
			(lci->lci_fast ?
			 X25_FAST_CALL_USER_DATA : X25_CALL_USER_DATA)) {
	    x25_sendcause(idb, lci, X25_CLEAR, X25_CLEAR_LOCAL_PROC_ERR,
		X25_DIAG_PACKET_TOO_LONG,NULL);
	    idb->x25_callsrcvdfailed++;
	    datagram_done(pak);
	} else {
	    if (badbcd(c, lci->lci_dst_addr.length)) {
		x25_sendcause(idb, lci, X25_CLEAR, X25_CLEAR_LOCAL_PROC_ERR,
		    X25_DIAG_INVALID_CALLED_ADDR, NULL);
		idb->x25_callsrcvdfailed++;
		datagram_done(pak);
	    } else if (badbcd(c, lci->lci_src_addr.length +
				 lci->lci_dst_addr.length)) {
		x25_sendcause(idb, lci, X25_CLEAR, X25_CLEAR_LOCAL_PROC_ERR,
		    X25_DIAG_INVALID_CALLING_ADDR, NULL);
		idb->x25_callsrcvdfailed++;
		datagram_done(pak);
	    } else
		x25_process_call(lci, pak, c + size + 1 + faclen,
			pak->datagramsize - (X25HEADERBYTES + X25_CALL_DATA +
			size + faclen));
	}
    } else if (p->x25_pti == X25_CALL_CONNECTED) {
	x25_sendcause(idb, lci, X25_CLEAR,
		      X25_CLEAR_LOCAL_PROC_ERR, X25_DIAG_INVALID_PTI_P1, NULL);
	datagram_done(pak);
    } else if (p->x25_pti == X25_CLEAR) {
	cause = X25_CLEAR_LOCAL_PROC_ERR;
	diag = 0;
	if (pak->datagramsize < X25HEADERBYTES + X25_SCAUSE_DATA)
	    diag = X25_DIAG_PACKET_TOO_SHORT;
	else if (ISDCE(idb, considb) && !IS_X25_CLEAR_DTE_ORIGINATED(c))
	    diag = X25_DIAG_ILLEGAL_DTE_CAUSE_CODE;
	else if ((*c > X25_CLEAR_DTE_ORIGINATED) &&
		 !(*c & X25_CLEAR_DTE_ORIGINATED_M)) {
	    idb->x25_provider_clears++;
	    if (*c == X25_CLEAR_LOCAL_PROC_ERR)
		idb->x25_errors_accused++;
	}
	if ((diag == 0) &&
	    (pak->datagramsize > X25HEADERBYTES + X25_CAUSE_DATA)) {
	    size = bcdlen_inline(*(c + X25_CAUSE_DATA));
	    faclen = *(c + X25_CAUSE_DATA + size + 1);
	    if (IS_1980(idb) && (!lci->lci_fast || faclen))
		diag = (faclen) ?
		    X25_DIAG_NONZERO_FACILITY_LEN : X25_DIAG_PACKET_TOO_LONG;
	    else if ((*(c + X25_CAUSE_DATA) & 0xf0) || (IS_1980(idb) && size))
		diag = X25_DIAG_NONZERO_ADDR_LEN;
	    else if ((pak->datagramsize <
			X25HEADERBYTES + X25_CAUSE_DATA + X25_CALL_DATA) ||
		     (pak->datagramsize <
			X25HEADERBYTES + X25_CAUSE_DATA + X25_CALL_DATA +
			size + faclen))
		diag = X25_DIAG_PACKET_TOO_SHORT;
	    else if (pak->datagramsize > X25HEADERBYTES + X25_CAUSE_DATA +
		X25_CALL_DATA + size + faclen)	/* can't be fast select */
		diag = X25_DIAG_PACKET_TOO_LONG;
	    else
		x25_parsefacility(idb, lci,
				  c + X25_CAUSE_DATA + size + 1,
				  X25_CLEAR, &cause, &diag, X25_ROLE_);
	}
	if (diag != 0) {
	    x25_sendcause(idb, lci, X25_CLEAR, cause, diag, NULL);
	    datagram_done(pak);
	} else
	    x25_process_clear(idb, lci, pak, c[0], c[1], TRUE);
	
    } else if (p->x25_pti == X25_CLEAR_CONFIRMATION) {
	x25_sendcause(idb, lci, X25_CLEAR,
		      X25_CLEAR_LOCAL_PROC_ERR, X25_DIAG_INVALID_PTI_P1, NULL);
	datagram_done(pak);
    } else if ((p->x25_pti == X25_RESTART ||
	p->x25_pti == X25_RESTART_CONFIRMATION) && LCI(p) != 0) {
	x25_sendcause(idb, lci, X25_CLEAR, X25_CLEAR_LOCAL_PROC_ERR,
	    X25_DIAG_ILLEGAL_LCI, NULL);
	datagram_done(pak);
    } else if (ISBFE(idb) &&
	       (p->x25_pti == X25_INTERRUPT ||
		p->x25_pti == X25_INTERRUPT_CONFIRMATION)) {
	x25_sendcause(idb, lci, X25_CLEAR,
		      X25_CLEAR_LOCAL_PROC_ERR, X25_DIAG_UNIDENT_PACKET, NULL);
	datagram_done(pak);
    } else if ((p->x25_pti & X25_DATA_MASK) == X25_DATA ||
	p->x25_pti == X25_INTERRUPT ||
	p->x25_pti == X25_INTERRUPT_CONFIRMATION ||
	p->x25_pti == X25_RESET || p->x25_pti == X25_RESET_CONFIRMATION ||
	(p->x25_pti & X25_FLOW_MASK) == X25_RNR ||
	(p->x25_pti & X25_FLOW_MASK) == X25_RR) {
	x25_sendcause(idb, lci, X25_CLEAR, X25_CLEAR_LOCAL_PROC_ERR,
	    X25_DIAG_INVALID_PTI_P1, NULL);
	datagram_done(pak);
    } else {
	x25_sendcause(idb, lci, X25_CLEAR, X25_CLEAR_LOCAL_PROC_ERR,
	    X25_DIAG_UNIDENT_PACKET, NULL);
	datagram_done(pak);
    }
}

/*
 * x25_process_call
 * Perform CALL REQUEST packet processing after all error conditions
 * have been checked for.
 */

void x25_process_call (
    lcitype *lci,
    paktype *pak,
    uchar *cud,
    int cud_len)
{
    hwidbtype *hwidb = lci->lci_idb->hwptr;
    considbtype *considb = get_lci_considb(lci);
    x25routeentrytype *route;
    uchar cause;
    uchar diag;
    uchar *reason;
    int pad_status;
    leveltype level;

    /*
     *	set the state of the VC D-bit flag 
     */
    lci->lci_dbit = ((packettype *)pak->datagramstart)->x25_d;

    /*
     *  determine if a Call matches a map, a route, or can't be handled
     */
    (void)x25_direct_call(lci, cud, cud_len, &cause, &diag, &reason);

    /*
     * notify api here, rather than at the entry to x25_p1, because
     * lci_linktype wasn't yet determined there.
     *
     * This is the earliest point which lci_linktype is known
     */
    if (!reg_invoke_x25_incoming_call(lci->lci_linktype, pak, lci, &cause,
				      &diag, &reason)) {
	/*
	 * X.25 client didn't want to accept the call.
	 * Make sure that there's a values for cause
	 * and reason.
	 */
	if (!cause)
	    cause = X25_CLEAR_NOT_OBTAINABLE;
	if (!reason)
	    reason = "\nCall refused by X.25 application";
    }

    switch (lci->lci_linktype) {
      case LINK_X25:
	/*
	 *  handle a switched Call
	 */
	route = lci->lci_rtp;

	if (!is_x25(route->idb->hwptr) &&
	    !ISCMNS_IEEE(route->idb->hwptr)) {
	    cause = X25_CLEAR_ACCESS_BARRED;
	    diag = X25_DIAG_INVALID_CALLED_ADDR;
	    reason = "\nCall routed to Null0";
	    break;
	}
	/* FALLTHROUGH */
      case LINK_X25TCP:
      case LINK_CMNS:
	switch (x25_cons_forward_call(lci, pak, cud, cud_len, &diag)) {
	  case X25_FORWARD_OK:
	  case X25_FORWARD_CMNS_IN_PROGRESS:
	    /*
	     *  set the VC state
	     */
	    lci->lci_state = ISDCE(hwidb, considb) ? X25_P2 : X25_P3;
	    break;
	  case X25_FORWARD_ALT:
	    /*
	     *  Alternate destinations need to be tried.
	     *  The packet has been re-queued to the end of the incoming
	     *  packet queue for x25_timer() to try again later, which gives
	     *  other x25_timer() functions a chance to run--note that X.25
	     *  has a single process thread for these process-level functions.
	     */
	    lci->lci_alt_diag = diag;	/*used above*/
	    return;
	  case X25_FORWARD_ERR:
	    /*
	     *  CONS/local/remote routing failed
	     */
	    lci->lci_idb->hwptr->x25_callsfrwdfailed++;
	    cause = X25_CLEAR_NOT_OBTAINABLE;
	    reason = "\nCall forwarding failure";
	    break;
	}

	pak = NULL;
	break;

      case LINK_X25SERVICE:
	/*
	 *  handle a service route
	 */
	if (!x25_service_accept(lci)) {
	    cause = X25_CLEAR_ACCESS_BARRED;
	    diag = X25_DIAG_INCOMING_CALL_BARRED;
	    reason = "\nX.25 services refused call";
	}
	break;

      case LINK_PAD:
	/*
	 *  handle a PAD Call
	 */
	level = set_interrupt_level(ALL_ENABLE);
	pad_status = pad_acceptcall(lci, cud, cud_len);
	reset_interrupt_level(level);

	switch (pad_status) {
	  case PAD_FAILED:
	    cause = X25_CLEAR_ACCESS_BARRED;
	    diag = X25_DIAG_INCOMING_CALL_BARRED;
	    reason = "\nPAD refused call";
	    break;

	  case PAD_IN_PROGRESS:
	    /*
	     *  the PAD will Clear or Confirm the call later;
	     *  put the VC in state P2/P3
	     */
	    lci->lci_state = ISDCE(hwidb, considb) ? X25_P2 : X25_P3;
	    break;

	  case PAD_OK:
	    break;
	}
	break;
      default:
	break;
    }

    if (pak)
	datagram_done(pak);
    /*
     *  the NUID references the (now free) packet
     */
    lci->lci_nuid = NULL;
    lci->lci_nuidlen = 0;

    /*
     *  either the VC needs to be cleared, accepted or its T11/T21 timer
     *  started
     */
    if (cause) {
	x25_sendcause(hwidb, lci, X25_CLEAR, cause, diag, NULL);
	if (cause != X25_CLEAR_LOCAL_PROC_ERR)
	    hwidb->x25_callsrcvdfailed++;
	if ((x25_debug || x25event_debug) &&
	    (x25_vcnum == 0 || x25_vcnum == lci->lci_lci))
	    buginf(reason);
    } else if (lci->lci_state == X25_P1) {
	lci->lci_state = X25_P4;
	sendpacket(lci->lci_idb->hwptr, lci,
		   X25_CALL_CONNECTED, lci->lci_dbit, NULL);
    } else {
	TIMER_START(lci->lci_timer, hwidb->x25_t1 * ONESEC);
	x25_addtimer(lci);
    }
}

/*
 * x25_process_pad_accept_call
 * Send a CALL CONNECTED on behalf of the PAD for reliable Protocol
 * Translator X25->TCP and X25->LAT connections ("printer" option).
 */
void x25_process_pad_accept_call (lcitype *lci)
{
    /*
     *  stop the CALL CONFIRM timer T11/T21 and, if needed
     *  start the idle timer
     */
    if (checkqueue(&x25tQ, lci)){
	p_unqueue(&x25tQ, lci);       /* stop timer T11/T21 */
	TIMER_STOP(lci->lci_timer);
	}
    
    lci->lci_state = X25_P4;
    sendpacket(lci->lci_idb->hwptr, lci,
	       X25_CALL_CONNECTED, lci->lci_dbit, NULL);
}

/*
 * x25_process_clear
 * Perform clear processing
 */

void x25_process_clear (
    hwidbtype *idb,
    lcitype *lci,
    paktype *pak,
    uchar cause,
    uchar diag,
    boolean conf)
{
    register packettype *p = (packettype *)pak->datagramstart;

    if (ISBFE(idb) &&
	x25_process_atif(idb, lci, cause, diag, conf)) {
	datagram_done(pak);
	return;
    }

    if (lci->lci_state == X25_P2 || lci->lci_state == X25_P3)
	idb->x25_callssentfailed++;

    /*
     *  the VC state is now P6/P7, but since the former state is needed
     *  to determine if an encapsulation Call should be put into hold-down
     *  and we send an immediate Clear Confirm (deleting the VC unless
     *  held-down), the new state is not set
     */

    if (lci->lci_swdata && p->x25_pti == X25_CLEAR)
	x25_forward_clear(lci, pak, cause, diag);
    else
	datagram_done(pak);

    if (conf)
 	sendpacket(idb, lci, X25_CLEAR_CONFIRMATION, 0, NULL);

    reg_invoke_media_x25_notify_event(lci->lci_linktype,
    	lci, X25_CLEAR_IND_EVENT, cause, diag, NULL);

    x25_call_remove(lci, cause, diag);
}

/*
 * x25_p2_p3
 * DTE call request
 * DCE incoming call
 */

void x25_p2_p3 (
    register hwidbtype *idb,
    register paktype *pak,
    register lcitype *lci)
{
    register packettype *p = (packettype *)pak->datagramstart;
    register uchar *c;
    uchar cause, diag;
    int size, faclen;
    register considbtype *considb;

    considb = get_lci_considb(lci);
    x25_api_notify(pak, lci);

    c = (uchar *)p + X25HEADERBYTES;
    if (pak->datagramsize < X25HEADERBYTES) {
	x25_sendcause(idb, lci, X25_CLEAR,
		X25_CLEAR_LOCAL_PROC_ERR, X25_DIAG_PACKET_TOO_SHORT, NULL);
	datagram_done(pak);
    } else if (p->x25_pti == X25_CALL_REQUEST) {
	if (ISDCE(idb, considb) ?
		((LCI(p) >= idb->x25_lic) && (LCI(p) <= idb->x25_hic)) :
		((LCI(p) >= idb->x25_loc) && (LCI(p) <= idb->x25_hoc))) {
	    idb->x25_callsrcvd++;
	    x25_sendcause(idb, lci, X25_CLEAR, X25_CLEAR_LOCAL_PROC_ERR,
		X25_DIAG_CALL_ON_1WAY_LC, NULL);
	    idb->x25_callsrcvdfailed++;
	    datagram_done(pak);
	} else
	    x25_process_call_collision(idb, lci, pak);
    } else if (p->x25_pti == X25_CALL_CONNECTED) {
	cause = X25_CLEAR_LOCAL_PROC_ERR;
	diag = 0;
	if (pak->datagramsize < X25HEADERBYTES)
	    diag = X25_DIAG_PACKET_TOO_SHORT;
	else if (pak->datagramsize > X25HEADERBYTES) {
	    size = bcdlen_inline(*c) + 1;	/* include addr. length byte */
	    if (pak->datagramsize <= X25HEADERBYTES + size)
		faclen = 0;
	    else
		faclen = *(c + size) + 1; /* include facility length byte */
#ifdef WIERD_BFE_CODE
	    if (ISBFE(idb) && (faclen > 1))
		diag = X25_DIAG_PACKET_TOO_LONG; /* stupid BFE conformance */
	    else 
#endif
	    if (pak->datagramsize < X25HEADERBYTES + size + faclen)
		diag = X25_DIAG_PACKET_TOO_SHORT;
	    else if (pak->datagramsize >
			X25HEADERBYTES + size + faclen +
			(lci->lci_fast ? X25_FAST_CALL_USER_DATA : 0))
		diag = X25_DIAG_PACKET_TOO_LONG;
	    else if (lci->lci_fast == X25_FACILITY_FAST_SELECT_RES)
		diag = X25_DIAG_PACK_INCOMPAT_FACILITY;
	    else if (faclen > 0)
		(void)x25_parsefacility(idb, lci, c + size,
					X25_CALL_CONNECTED, &cause, &diag,
					X25_ROLE_CALLING_DTE);
	}

	if (lci->lci_prec_connected == X25_UNASSIGNED_PREC) {
	    lci->lci_prec_connected = lci->lci_prec;
	}
	if ((diag == 0) && (lci->lci_prec != lci->lci_prec_connected)) {
	    diag = X25_DIAG_FACILITY_PARAM_ILLEGAL;
	}
	if (diag != 0) {
	    x25_sendcause(idb, lci, X25_CLEAR, cause, diag, NULL);
	    datagram_done(pak);
	} else
	    x25_process_call_connected(idb, lci, pak);
    } else if (p->x25_pti == X25_CLEAR) {
	cause = X25_CLEAR_LOCAL_PROC_ERR;
	diag = 0;
	if (pak->datagramsize < X25HEADERBYTES + X25_SCAUSE_DATA)
	    diag = X25_DIAG_PACKET_TOO_SHORT;
	else if (ISDCE(idb, considb) && !IS_X25_CLEAR_DTE_ORIGINATED(c))
	    diag = X25_DIAG_ILLEGAL_DTE_CAUSE_CODE;
	else if ((*c > X25_CLEAR_DTE_ORIGINATED) &&
		 !(*c & X25_CLEAR_DTE_ORIGINATED_M)) {
	    idb->x25_provider_clears++;
	    if (*c == X25_CLEAR_LOCAL_PROC_ERR)
		idb->x25_errors_accused++;
	}
	if ((diag == 0) &&
	    (pak->datagramsize > X25HEADERBYTES + X25_CAUSE_DATA)) {
	    size = bcdlen_inline(*(c + X25_CAUSE_DATA));
	    faclen = *(c + X25_CAUSE_DATA + size + 1);
	    if (IS_1980(idb) && (!lci->lci_fast || faclen))
		diag = (faclen) ?
		    X25_DIAG_NONZERO_FACILITY_LEN : X25_DIAG_PACKET_TOO_LONG;
	    else if ((*(c + X25_CAUSE_DATA) & 0xf0) || (IS_1980(idb) && size))
		diag = X25_DIAG_NONZERO_ADDR_LEN;
	    else if ((pak->datagramsize <
			X25HEADERBYTES + X25_CAUSE_DATA + X25_CALL_DATA) ||
		     (pak->datagramsize <
			X25HEADERBYTES + X25_CAUSE_DATA + X25_CALL_DATA +
			size + faclen))
		diag = X25_DIAG_PACKET_TOO_SHORT;
	    else if (pak->datagramsize >
			X25HEADERBYTES + X25_CAUSE_DATA +
			X25_CALL_DATA + size + faclen +
			(lci->lci_fast ? X25_FAST_CALL_USER_DATA : 0))
		diag = X25_DIAG_PACKET_TOO_LONG;
	    else
		x25_parsefacility(idb, lci,
				  c + X25_CAUSE_DATA + size + 1,
				  X25_CLEAR, &cause, &diag, X25_ROLE_);
	}
	if (diag != 0) {
	    x25_sendcause(idb, lci, X25_CLEAR, cause, diag, NULL);
	    datagram_done(pak);
	} else
	    x25_process_clear(idb, lci, pak, c[0], c[1], TRUE);
    } else if (p->x25_pti == X25_CLEAR_CONFIRMATION) {
	x25_sendcause(idb, lci, X25_CLEAR,
		      X25_CLEAR_LOCAL_PROC_ERR, ISDCE(idb, considb) ?
			X25_DIAG_INVALID_PTI_P3 : X25_DIAG_INVALID_PTI_P2,
		      NULL);
	datagram_done(pak);
    } else if ((p->x25_pti == X25_RESTART ||
	p->x25_pti == X25_RESTART_CONFIRMATION) && LCI(p) != 0) {
	x25_sendcause(idb, lci, X25_CLEAR, X25_CLEAR_LOCAL_PROC_ERR,
	    X25_DIAG_ILLEGAL_LCI, NULL);
	datagram_done(pak);
    } else if (ISBFE(idb) &&
	       (p->x25_pti == X25_INTERRUPT ||
		p->x25_pti == X25_INTERRUPT_CONFIRMATION)) {
	x25_sendcause(idb, lci, X25_CLEAR,
		      X25_CLEAR_LOCAL_PROC_ERR, X25_DIAG_UNIDENT_PACKET, NULL);
	datagram_done(pak);
    } else if ((p->x25_pti & X25_DATA_MASK) == X25_DATA ||
	p->x25_pti == X25_INTERRUPT ||
	p->x25_pti == X25_INTERRUPT_CONFIRMATION ||
	p->x25_pti == X25_RESET || p->x25_pti == X25_RESET_CONFIRMATION ||
	(p->x25_pti & X25_FLOW_MASK) == X25_RNR ||
	(p->x25_pti & X25_FLOW_MASK) == X25_RR) {
	if (p->x25_pti == X25_RESET) {
	    if ((*c == X25_RESET_DTE_ORIGINATED) ||
		(*c & X25_RESET_DTE_ORIGINATED_M)) {
		idb->x25_dte_resets++;
		lci->lci_dte_resets++;
	    } else {
		idb->x25_dce_resets++;
		lci->lci_dce_resets++;
		if (*c == X25_RESET_LOCAL_PROC_ERR)
		    idb->x25_errors_accused++;
	    }
	}
	x25_sendcause(idb, lci, X25_CLEAR, X25_CLEAR_LOCAL_PROC_ERR,
		ISDCE(idb, considb) ?
			X25_DIAG_INVALID_PTI_P3 : X25_DIAG_INVALID_PTI_P2,NULL);
	datagram_done(pak);
    } else {
	x25_sendcause(idb, lci, X25_CLEAR,
		X25_CLEAR_LOCAL_PROC_ERR, X25_DIAG_UNIDENT_PACKET, NULL);
	datagram_done(pak);
    }
}

/*
 * x25_process_call_collision
 * Process the call collision state
 */

void x25_process_call_collision (
    hwidbtype *idb,
    lcitype *lci,
    paktype *pak)
{
    packettype *p;
    register considbtype *considb;
    int flen, tlen, size, faclen;
    uchar cause, diag;
    uchar *c;
    uchar caddr[2*X25_ADDRESS_LENGTH];
    ushort save_pin, save_pout;
    uchar save_win, save_wout, save_prec, save_fast, save_reverse;

    considb = get_lci_considb(lci);

    p = (packettype *)pak->datagramstart;

    /*
     *  If DCE, we clear our call indication and process the DTE's call request
     *  If DTE, we ignore the call indication
     */
    if (ISDCE(idb, considb)) {
	/*
	 *  get rid of the incoming part of a switched call
	 */
	x25_flush_call(lci, X25_CLEAR_DTE_ORIGINATED, X25_DIAG_CALL_COLLISION,
		       FALSE);

	/*
	 *  the current LCI has a set of information that may not be used for
	 *  the incoming CALL, so delete the current LCI and create a new lci
	 */
	if (TIMER_RUNNING(lci->lci_timer)) {
	    p_unqueue(&x25tQ, lci);
	    TIMER_STOP(lci->lci_timer);
	}
	x25_lci_delete(lci);
	lci = x25_lci_enter(idb->firstsw, LCI(p), pak->llc2_cb);

	if (lci == NULL) {
	    datagram_done(pak);
	    return;
	}
	lci->lci_outgoing = FALSE;

	/*
	 *  process the CALL
	 */
	x25_p1(idb, pak, lci);

	/*
	 *  if we're waiting for a CALL CONFIRM, the state is P5
	 */
	if (lci->lci_state == X25_P2 || lci->lci_state == X25_P3)
	    lci->lci_state = X25_P5;
    } else if (!BFE_BROKEN(idb)) {
	/*
	 *  this is a rather obscure area--
	 *
	 *  in the event of a call collision, the action a DTE takes is
	 *  to DISCARD the incoming CALL
	 *
	 *  *but* ISO 8208 requires that the format of the incoming CALL
	 *  packet be validated(!); BFE requires this behavior
	 *
	 *  the CCITT only addresses the operation of a DCE, so naturally
	 *  its tables only show that a call collision requires NORMAL
	 *  processing from a DCE
	 *
	 *  I've chosen to discard the packet because this is only an
	 *  area that certification suites care about and validating
	 *  the incoming CALL will screw up the negotiated facilities
	 *  in the lci structure
	 */
	idb->x25_callsrcvd++;
	idb->x25_callsrcvdfailed++;
	datagram_done(pak);
	lci->lci_state = X25_P5;
    } else {
	idb->x25_callsrcvd++;
	idb->x25_callsrcvdfailed++;

	/*
	 *  save those VC attributes that can be changed by the following
	 *  validation checks--note that this is tailored to the parameters
	 *  that x25_parsefacility() may change under a BFE encapsulation
	 */
	save_pin = lci->lci_pin;
	save_pout = lci->lci_pout;
	save_win = lci->lci_win;
	save_wout = lci->lci_wout;
	save_prec = lci->lci_prec;
	save_fast = lci->lci_fast;
	save_reverse = lci->lci_reverse;
	lci->lci_fast = 0;

	c = (uchar *)p + X25HEADERBYTES;
	flen = (*c >> 4) & 0xf;
	tlen = *c & 0xf;
	size = bcdlen_inline(*c++);
	faclen = *(c + size);
	if (ISDCE(idb, considb) ?
		((LCI(p) >= idb->x25_lic) && (LCI(p) <= idb->x25_hic)) :
		((LCI(p) >= idb->x25_loc) && (LCI(p) <= idb->x25_hoc))) {
	    x25_sendcause(idb, lci, X25_CLEAR, X25_CLEAR_LOCAL_PROC_ERR,
		X25_DIAG_CALL_ON_1WAY_LC, NULL);
	    datagram_done(pak);
	} else if ((pak->datagramsize < X25HEADERBYTES + X25_CALL_DATA) ||
	    (pak->datagramsize <
		X25HEADERBYTES + X25_CALL_DATA + size + faclen)) {
	    x25_sendcause(idb, lci, X25_CLEAR, X25_CLEAR_LOCAL_PROC_ERR,
		X25_DIAG_PACKET_TOO_SHORT, NULL);
	    if ((x25_debug || x25event_debug) &&
		(x25_vcnum == 0 || x25_vcnum == lci->lci_lci))
		buginf("\nCALL shorter than needed");
	    datagram_done(pak);
	} else if (!x25_parsefacility(idb, lci, c + size, X25_CALL_REQUEST,
				      &cause, &diag, X25_ROLE_CALLED_DTE)) {
	    x25_sendcause(idb, lci, X25_CLEAR, cause, diag, NULL);
	    if ((x25_debug || x25event_debug) &&
		(x25_vcnum == 0 || x25_vcnum == lci->lci_lci))
		buginf("\nFacility failure: cause %d diagnostic %d",
		    cause, diag);
	    datagram_done(pak);
	} else if (pak->datagramsize >
			X25HEADERBYTES + X25_CALL_DATA + size + faclen +
			(lci->lci_fast ?
			 X25_FAST_CALL_USER_DATA : X25_CALL_USER_DATA)) {
	    x25_sendcause(idb, lci, X25_CLEAR, X25_CLEAR_LOCAL_PROC_ERR,
		X25_DIAG_PACKET_TOO_LONG,NULL);
	    datagram_done(pak);
	} else {
	    bcdtochar(c, caddr, flen+tlen);
	    if (badbcd(c, tlen)) {
		x25_sendcause(idb, lci, X25_CLEAR, X25_CLEAR_LOCAL_PROC_ERR,
		    X25_DIAG_INVALID_CALLED_ADDR, NULL);
		datagram_done(pak);
	    } else if (badbcd(c, tlen + flen)) {
		x25_sendcause(idb, lci, X25_CLEAR, X25_CLEAR_LOCAL_PROC_ERR,
		    X25_DIAG_INVALID_CALLING_ADDR, NULL);
		datagram_done(pak);
	    } else {
		datagram_done(pak);
		lci->lci_pin = save_pin;
		lci->lci_pout = save_pout;
		lci->lci_win = save_win;
		lci->lci_wout = save_wout;
		lci->lci_prec = save_prec;
		lci->lci_fast = save_fast;
		lci->lci_reverse = save_reverse;
		lci->lci_state = X25_P5;
	    }
	}
    }
}

/*
 * x25_process_call_connected
 * Process the Call Connected packet
 */

void x25_process_call_connected (
    hwidbtype *idb,
    lcitype *lci,
    paktype *pak)
{
    static paktype fakepak;

    lci->lci_dbit |= ((packettype *)pak->datagramstart)->x25_d;

    p_unqueue(&x25tQ, lci);
    if (lci->lci_idle) {
	TIMER_START(lci->lci_timer, lci->lci_idle);
	x25_addtimer(lci);
    } else {
        TIMER_STOP(lci->lci_timer);
    }

    lci->lci_state = X25_P4;

    if (lci->lci_linktype == LINK_PAD)
	pad_completecall(lci);
    else {
	if (lci->lci_swdata) {
	    x25_forward_call_connected(lci, pak);
	    pak = NULL;
	}
    }
    if (!QUEUEEMPTY(&lci->lci_holdQ)) {
	fakepak.datagramsize = 0;
	fakepak.x25_cb = lci;
	x25_output(idb, &fakepak, TAIL);
    }

    if (pak)
	datagram_done(pak);
}

/*
 * x25_p4
 * Data transfer
 */

void x25_p4 (
    register hwidbtype *idb,
    register paktype *pak,
    register lcitype *lci)
{
    register packettype *p = (packettype *)pak->datagramstart;
    register uchar *c;
    register considbtype *considb;
    uchar cause, diag;
    int size, faclen;

    considb = get_lci_considb(lci);
    x25_api_notify(pak, lci);

    c = (uchar *)p + X25HEADERBYTES;
    if (p->x25_pti == X25_CALL_REQUEST) {
	idb->x25_callsrcvd++;
	lci->lci_retransmit = 0;
	x25_sendcause(idb, lci, X25_CLEAR,
		      X25_CLEAR_LOCAL_PROC_ERR, X25_DIAG_INVALID_PTI_P4, NULL);
	idb->x25_callsrcvdfailed++;
	datagram_done(pak);
    } else if (p->x25_pti == X25_CALL_CONNECTED) {
	x25_sendcause(idb, lci, X25_CLEAR,
		      X25_CLEAR_LOCAL_PROC_ERR, X25_DIAG_INVALID_PTI_P4, NULL);
	datagram_done(pak);
    } else if (p->x25_pti == X25_CLEAR) {
	cause = X25_CLEAR_LOCAL_PROC_ERR;
	diag = 0;
	if (pak->datagramsize < X25HEADERBYTES + X25_SCAUSE_DATA)
	    diag = X25_DIAG_PACKET_TOO_SHORT;
	else if (ISDCE(idb, considb) && !IS_X25_CLEAR_DTE_ORIGINATED(c))
	    diag = X25_DIAG_ILLEGAL_DTE_CAUSE_CODE;
	else if ((*c > X25_CLEAR_DTE_ORIGINATED) &&
		 !(*c & X25_CLEAR_DTE_ORIGINATED_M)) {
	    idb->x25_provider_clears++;
	    if (*c == X25_CLEAR_LOCAL_PROC_ERR)
		idb->x25_errors_accused++;
	}
	if ((diag == 0) &&
	    (pak->datagramsize > X25HEADERBYTES + X25_CAUSE_DATA)) {
	    size = bcdlen_inline(*(c + X25_CAUSE_DATA));
	    faclen = *(c + X25_CAUSE_DATA + size + 1);
	    if (IS_1980(idb) && (!lci->lci_fast || faclen))
		diag = (faclen) ?
		    X25_DIAG_NONZERO_FACILITY_LEN : X25_DIAG_PACKET_TOO_LONG;
	    else if ((*(c + X25_CAUSE_DATA) & 0xf0) || (IS_1980(idb) && size))
		diag = X25_DIAG_NONZERO_ADDR_LEN;
	    else if ((pak->datagramsize <
			X25HEADERBYTES + X25_CAUSE_DATA + X25_CALL_DATA) ||
		     (pak->datagramsize <
			X25HEADERBYTES + X25_CAUSE_DATA + X25_CALL_DATA +
			size + faclen))
		diag = X25_DIAG_PACKET_TOO_SHORT;
	    else if (pak->datagramsize > X25HEADERBYTES + X25_CAUSE_DATA +
		X25_CALL_DATA + size + faclen +
		(lci->lci_fast ? X25_FAST_CALL_USER_DATA : 0))
		diag = X25_DIAG_PACKET_TOO_LONG;
	    else
		x25_parsefacility(idb, lci,
				  c + X25_CAUSE_DATA + size + 1,
				  X25_CLEAR, &cause, &diag, X25_ROLE_);
	}
	lci->lci_retransmit = 0;
	if (diag != 0) {
	    x25_sendcause(idb, lci, X25_CLEAR, cause, diag, NULL);
	    datagram_done(pak);
	} else
	    x25_process_clear(idb, lci, pak, c[0], c[1], TRUE);
    } else if (p->x25_pti == X25_CLEAR_CONFIRMATION) {
	lci->lci_retransmit = 0;
	x25_sendcause(idb, lci, X25_CLEAR,
		      X25_CLEAR_LOCAL_PROC_ERR, X25_DIAG_INVALID_PTI_P4, NULL);
	datagram_done(pak);
    } else {
	switch (lci->lci_state) {
	case X25_P4:
	case X25_D1:
	    lci->lci_state = X25_D1;
	    if (lci->lci_idle) {
		if (TIMER_RUNNING(lci->lci_timer))
		    p_unqueue(&x25tQ, lci);
		TIMER_START(lci->lci_timer, lci->lci_idle);
		x25_addtimer(lci);
	    }
	    x25_d1(idb, pak, lci);
	    break;
	case X25_D2:
	case X25_D3:
	    x25_d2_d3(idb, pak, lci);
	    break;
	default:
	    errmsg(&msgsym(ILLP4, X25), idb->hw_namestring,
		   x25_prstate(idb, lci));
	    datagram_done(pak);
	    break;
	}
    }
}

/*
 * x25_p5
 * Call collision
 */

void x25_p5 (
    register hwidbtype *idb,
    register paktype *pak,
    register lcitype *lci)
{
    register packettype *p = (packettype *)pak->datagramstart;
    register uchar *c;
    uchar cause, diag;
    int size, faclen;
    register considbtype *considb;

    considb = get_lci_considb(lci);
    x25_api_notify(pak, lci);

    c = (uchar *)p + X25HEADERBYTES;
    if (pak->datagramsize < X25HEADERBYTES) {
	x25_sendcause(idb, lci, X25_CLEAR,
		X25_CLEAR_LOCAL_PROC_ERR, X25_DIAG_PACKET_TOO_SHORT, NULL);
	datagram_done(pak);
    } else if (p->x25_pti == X25_CALL_REQUEST) {
	idb->x25_callsrcvd++;
	x25_sendcause(idb, lci, X25_CLEAR,
		      X25_CLEAR_LOCAL_PROC_ERR, X25_DIAG_INVALID_PTI_P5, NULL);
	idb->x25_callsrcvdfailed++;
	datagram_done(pak);
    } else if (p->x25_pti == X25_CALL_CONNECTED) {
	cause = X25_CLEAR_LOCAL_PROC_ERR;
	diag = 0;
	if (ISDCE(idb, considb))
	    diag = X25_DIAG_INVALID_PTI_P5;
	else if (pak->datagramsize < X25HEADERBYTES)
	    diag = X25_DIAG_PACKET_TOO_SHORT;
	else if (pak->datagramsize > X25HEADERBYTES) {
	    size = bcdlen_inline(*c) + 1;	/* include addr. length byte */
	    if (pak->datagramsize <= X25HEADERBYTES + size)
		faclen = 0;
	    else
		faclen = *(c + size) + 1; /* include facility length byte */
#ifdef WIERD_BFE_CODE
	    if (ISBFE(idb) && (faclen > 1))
		diag = X25_DIAG_PACKET_TOO_LONG; /* stupid BFE conformance */
	    else 
#endif
	    if (pak->datagramsize < X25HEADERBYTES + size + faclen)
		diag = X25_DIAG_PACKET_TOO_SHORT;
	    else if (pak->datagramsize > X25HEADERBYTES + size + faclen +
		(lci->lci_fast ? X25_FAST_CALL_USER_DATA : 0))
		diag = X25_DIAG_PACKET_TOO_LONG;
	    else if (faclen > 0)
		(void)x25_parsefacility(idb, lci, c + size,
					X25_CALL_CONNECTED, &cause, &diag,
					X25_ROLE_CALLING_DTE);
	}

	if (lci->lci_prec_connected == X25_UNASSIGNED_PREC) {
	    lci->lci_prec_connected = lci->lci_prec;
	}
	if ((diag == 0) && (lci->lci_prec != lci->lci_prec_connected)) {
	    diag = X25_DIAG_FACILITY_PARAM_ILLEGAL;
	}
	if (diag != 0) {
	    x25_sendcause(idb, lci, X25_CLEAR, cause, diag, NULL);
	    datagram_done(pak);
	} else
	    x25_process_call_connected(idb, lci, pak);
    } else if (p->x25_pti == X25_CLEAR) {
	cause = X25_CLEAR_LOCAL_PROC_ERR;
	diag = 0;
	if (pak->datagramsize < X25HEADERBYTES + X25_SCAUSE_DATA)
	    diag = X25_DIAG_PACKET_TOO_SHORT;
	else if (ISDCE(idb, considb) && !IS_X25_CLEAR_DTE_ORIGINATED(c))
	    diag = X25_DIAG_ILLEGAL_DTE_CAUSE_CODE;
	else if ((*c > X25_CLEAR_DTE_ORIGINATED) &&
		 !(*c & X25_CLEAR_DTE_ORIGINATED_M)) {
	    idb->x25_provider_clears++;
	    if (*c == X25_CLEAR_LOCAL_PROC_ERR)
		idb->x25_errors_accused++;
	}
	if ((diag == 0) &&
	    (pak->datagramsize > X25HEADERBYTES + X25_CAUSE_DATA)) {
	    size = bcdlen_inline(*(c + X25_CAUSE_DATA));
	    faclen = *(c + X25_CAUSE_DATA + size + 1);
	    if (IS_1980(idb) && (!lci->lci_fast || faclen))
		diag = (faclen) ?
		    X25_DIAG_NONZERO_FACILITY_LEN : X25_DIAG_PACKET_TOO_LONG;
	    else if ((*(c + X25_CAUSE_DATA) & 0xf0) || (IS_1980(idb) && size))
		diag = X25_DIAG_NONZERO_ADDR_LEN;
	    else if ((pak->datagramsize <
			X25HEADERBYTES + X25_CAUSE_DATA + X25_CALL_DATA) ||
		     (pak->datagramsize <
			X25HEADERBYTES + X25_CAUSE_DATA + X25_CALL_DATA +
			size + faclen))
		diag = X25_DIAG_PACKET_TOO_SHORT;
	    else if (pak->datagramsize >
		     X25HEADERBYTES + X25_CAUSE_DATA + X25_CALL_DATA +
		     size + faclen +
		     (lci->lci_fast ? X25_FAST_CALL_USER_DATA : 0))
		diag = X25_DIAG_PACKET_TOO_LONG;
	    else
		x25_parsefacility(idb, lci,
				  c + X25_CAUSE_DATA + size + 1,
				  X25_CLEAR, &cause, &diag, X25_ROLE_);
	}
	if (diag != 0) {
	    x25_sendcause(idb, lci, X25_CLEAR, cause, diag, NULL);
	    datagram_done(pak);
	} else
	    x25_process_clear(idb, lci, pak, c[0], c[1], TRUE);
    } else if (p->x25_pti == X25_CLEAR_CONFIRMATION) {
	x25_sendcause(idb, lci, X25_CLEAR,
		      X25_CLEAR_LOCAL_PROC_ERR, X25_DIAG_INVALID_PTI_P5, NULL);
	datagram_done(pak);
    } else if ((p->x25_pti == X25_RESTART ||
	p->x25_pti == X25_RESTART_CONFIRMATION) && LCI(p) != 0) {
	x25_sendcause(idb, lci, X25_CLEAR, X25_CLEAR_LOCAL_PROC_ERR,
	    X25_DIAG_ILLEGAL_LCI, NULL);
	datagram_done(pak);
    } else if (ISBFE(idb) &&
	       (p->x25_pti == X25_INTERRUPT ||
		p->x25_pti == X25_INTERRUPT_CONFIRMATION)) {
	x25_sendcause(idb, lci, X25_CLEAR,
		      X25_CLEAR_LOCAL_PROC_ERR, X25_DIAG_UNIDENT_PACKET, NULL);
	datagram_done(pak);
    } else if ((p->x25_pti & X25_DATA_MASK) == X25_DATA ||
	p->x25_pti == X25_INTERRUPT ||
	p->x25_pti == X25_INTERRUPT_CONFIRMATION ||
	p->x25_pti == X25_RESET || p->x25_pti == X25_RESET_CONFIRMATION ||
	(p->x25_pti & X25_FLOW_MASK) == X25_RNR ||
	(p->x25_pti & X25_FLOW_MASK) == X25_RR) {
	if (p->x25_pti == X25_RESET) {
	    if ((*c == X25_RESET_DTE_ORIGINATED) ||
		(*c & X25_RESET_DTE_ORIGINATED_M)) {
		idb->x25_dte_resets++;
		lci->lci_dte_resets++;
	    } else {
		idb->x25_dce_resets++;
		lci->lci_dce_resets++;
		if (*c == X25_RESET_LOCAL_PROC_ERR)
		    idb->x25_errors_accused++;
	    }
	}
	x25_sendcause(idb, lci, X25_CLEAR, X25_CLEAR_LOCAL_PROC_ERR,
	    X25_DIAG_INVALID_PTI_P5, NULL);
	datagram_done(pak);
    } else {
	x25_sendcause(idb, lci, X25_CLEAR, X25_CLEAR_LOCAL_PROC_ERR,
	    X25_DIAG_UNIDENT_PACKET, NULL);
	datagram_done(pak);
    }
}

/*
 * x25_p6_p7
 * DTE clear request
 * DCE clear indication
 */

void x25_p6_p7 (
    register hwidbtype *idb,
    register paktype *pak,
    register lcitype *lci)
{
    register packettype *p = (packettype *)pak->datagramstart;
    register uchar *c;
    uchar cause, diag;
    int size, faclen;
    register considbtype *considb;

    considb = get_lci_considb(lci);
    x25_api_notify(pak, lci);

    c = (uchar *)p + X25HEADERBYTES;
    if (p->x25_pti == X25_CALL_REQUEST) {
	idb->x25_callsrcvd++;
	idb->x25_callsrcvdfailed++;
	datagram_done(pak);
    } else if (p->x25_pti == X25_CLEAR) {
	cause = X25_CLEAR_LOCAL_PROC_ERR;
	diag = 0;
	if (pak->datagramsize < X25HEADERBYTES + X25_SCAUSE_DATA)
	    diag = X25_DIAG_PACKET_TOO_SHORT;
	else if (ISDCE(idb, considb) && !IS_X25_CLEAR_DTE_ORIGINATED(c))
	    diag = X25_DIAG_ILLEGAL_DTE_CAUSE_CODE;
	else if ((*c > X25_CLEAR_DTE_ORIGINATED) &&
		 !(*c & X25_CLEAR_DTE_ORIGINATED_M)) {
	    idb->x25_provider_clears++;
	    if (*c == X25_CLEAR_LOCAL_PROC_ERR)
		idb->x25_errors_accused++;
	}
	if ((diag == 0) &&
	    (pak->datagramsize > X25HEADERBYTES + X25_CAUSE_DATA)) {
	    size = bcdlen_inline(*(c + X25_CAUSE_DATA));
	    faclen = *(c + X25_CAUSE_DATA + size + 1);
	    if (IS_1980(idb) && (!lci->lci_fast || faclen))
		diag = (faclen) ?
		    X25_DIAG_NONZERO_FACILITY_LEN : X25_DIAG_PACKET_TOO_LONG;
	    else if ((*(c + X25_CAUSE_DATA) & 0xf0) || (IS_1980(idb) && size))
		diag = X25_DIAG_NONZERO_ADDR_LEN;
	    else if ((pak->datagramsize <
			X25HEADERBYTES + X25_CAUSE_DATA + X25_CALL_DATA) ||
		     (pak->datagramsize <
			X25HEADERBYTES + X25_CAUSE_DATA + X25_CALL_DATA +
			size + faclen))
		diag = X25_DIAG_PACKET_TOO_SHORT;
	    else if (pak->datagramsize >
			X25HEADERBYTES + X25_CAUSE_DATA + X25_CALL_DATA +
			size + faclen +
			(lci->lci_fast ? X25_FAST_CALL_USER_DATA : 0))
		diag = X25_DIAG_PACKET_TOO_LONG;
	    else
		x25_parsefacility(idb, lci, c + X25_CAUSE_DATA + size + 1,
				  X25_CLEAR, &cause, &diag, X25_ROLE_);
	}
	if (diag != 0) {
	    lci->lci_retransmit = 0;
	    x25_sendcause(idb, lci, X25_CLEAR, cause, diag, NULL);
	    datagram_done(pak);
	} else
	    x25_process_clear(idb, lci, pak, c[0], c[1], FALSE);
    } else if (p->x25_pti == X25_CLEAR_CONFIRMATION) {
	cause = X25_CLEAR_LOCAL_PROC_ERR;
	diag = 0;
	if (pak->datagramsize < X25HEADERBYTES) {
	    diag = X25_DIAG_PACKET_TOO_SHORT;
	} else if (pak->datagramsize > X25HEADERBYTES) {
	    size = bcdlen_inline(*c);
	    faclen = *(c + size + 1);
	    if (IS_1980(idb))
		diag = X25_DIAG_PACKET_TOO_LONG;
	    else if ((pak->datagramsize <
			X25HEADERBYTES + X25_CALL_DATA) ||
		     (pak->datagramsize <
			X25HEADERBYTES + X25_CALL_DATA + size + faclen))
		diag = X25_DIAG_PACKET_TOO_SHORT;
	    else if (pak->datagramsize >
			X25HEADERBYTES + X25_CALL_DATA + size + faclen)
		diag = X25_DIAG_PACKET_TOO_LONG;
	    else if (size != 0)
		diag = X25_DIAG_NONZERO_ADDR_LEN;
	    else
		x25_parsefacility(idb, lci, c + size + 1,
				  X25_CLEAR_CONFIRMATION, &cause, &diag,
				  X25_ROLE_);
	}
	if (diag != 0) {
	    lci->lci_retransmit = 0;
	    x25_sendcause(idb, lci, X25_CLEAR, cause, diag, NULL);
	    datagram_done(pak);
	} else
	    x25_process_clear(idb, lci, pak, 0, 0, FALSE);
    } else {
	datagram_done(pak);
    }
}

/*
 * x25_d1
 * Flow control ready
 */

void x25_d1 (
    register hwidbtype *idb,
    register paktype *pak,
    register lcitype *lci)
{
    register packettype *p = (packettype *)pak->datagramstart;
    uchar *c = (uchar *)p + X25HEADERBYTES;

    if (pak->datagramsize < X25HEADERBYTES) {
	x25_sendcause(idb, lci, X25_RESET,
		X25_RESET_LOCAL_PROC_ERR, X25_DIAG_PACKET_TOO_SHORT, NULL);
	datagram_done(pak);
    } else if (p->x25_pti == X25_RESET) {
	if ((*c == X25_RESET_DTE_ORIGINATED) ||
	    (*c & X25_RESET_DTE_ORIGINATED_M)) {
	    idb->x25_dte_resets++;
	    lci->lci_dte_resets++;
	} else {
	    idb->x25_dce_resets++;
	    lci->lci_dce_resets++;
	    if (*c == X25_RESET_LOCAL_PROC_ERR)
		idb->x25_errors_accused++;
	}
	if (pak->datagramsize < X25HEADERBYTES + X25_SCAUSE_DATA) {
	    x25_sendcause(idb, lci, X25_RESET, X25_RESET_LOCAL_PROC_ERR,
		X25_DIAG_PACKET_TOO_SHORT, NULL);
	    datagram_done(pak);
	} else if (pak->datagramsize > X25HEADERBYTES + X25_CAUSE_DATA) {
	    x25_sendcause(idb, lci, X25_RESET, X25_RESET_LOCAL_PROC_ERR,
		X25_DIAG_PACKET_TOO_LONG, NULL);
	    datagram_done(pak);
	} else
	    x25_process_reset(lci, pak, c[0], c[1]);
    } else if (p->x25_pti == X25_RESET_CONFIRMATION) {
	if (pak->datagramsize < X25HEADERBYTES)
	    x25_sendcause(idb, lci, X25_RESET, X25_RESET_LOCAL_PROC_ERR,
		X25_DIAG_PACKET_TOO_SHORT, NULL);
	else if (pak->datagramsize > X25HEADERBYTES)
	    x25_sendcause(idb, lci, X25_RESET, X25_RESET_LOCAL_PROC_ERR,
		X25_DIAG_PACKET_TOO_LONG, NULL);
	else
	    x25_sendcause(idb, lci, X25_RESET, X25_RESET_LOCAL_PROC_ERR,
		X25_DIAG_INVALID_PTI_D1, NULL);
	datagram_done(pak);
    } else if (p->x25_pti == X25_RESTART ||
	       p->x25_pti == X25_RESTART_CONFIRMATION) {
	x25_sendcause(idb, lci, X25_RESET, X25_RESET_LOCAL_PROC_ERR,
	    X25_DIAG_ILLEGAL_LCI, NULL);
	datagram_done(pak);
    } else if (ISBFE(idb) &&
	       (p->x25_pti == X25_INTERRUPT ||
		p->x25_pti == X25_INTERRUPT_CONFIRMATION)) {
	x25_sendcause(idb, lci, X25_RESET,
		      X25_RESET_LOCAL_PROC_ERR, X25_DIAG_UNIDENT_PACKET, NULL);
	datagram_done(pak);
    } else if (p->x25_pti == X25_INTERRUPT) {
	idb->x25_interrupt_rx++;
	if (pak->datagramsize <= X25HEADERBYTES) {
	    x25_sendcause(idb, lci, X25_RESET, X25_RESET_LOCAL_PROC_ERR,
		X25_DIAG_PACKET_TOO_SHORT, NULL);
	    datagram_done(pak);
	} else if (pak->datagramsize >
		   X25HEADERBYTES +
		   (IS_1980(idb) ?
			X25_INT_DATA_MAX_1980 : X25_INT_DATA_MAX_1984)) {
	    x25_sendcause(idb, lci, X25_RESET, X25_RESET_LOCAL_PROC_ERR,
		X25_DIAG_PACKET_TOO_LONG, NULL);
	    datagram_done(pak);
	} else if (!lci->lci_allow_int) {
	    x25_sendcause(idb, lci, X25_RESET, 
			  X25_RESET_LOCAL_PROC_ERR, X25_DIAG_UNAUTH_INTR, NULL);
	    datagram_done(pak);
	} else {
	    if (lci->lci_swdata) {
		x25_forward_interrupt(lci, pak);
	    } else {
		sendpacket(lci->lci_idb->hwptr, lci, X25_INTERRUPT_CONFIRMATION,
			   0, NULL);
		datagram_done(pak);
	    }
	    lci->lci_intsrcvd++;
	}
    } else if (p->x25_pti == X25_INTERRUPT_CONFIRMATION) {
	if (pak->datagramsize < X25HEADERBYTES) {
	    x25_sendcause(idb, lci, X25_RESET, X25_RESET_LOCAL_PROC_ERR,
		X25_DIAG_PACKET_TOO_SHORT, NULL);
	    datagram_done(pak);
	} else if (pak->datagramsize > X25HEADERBYTES) {
	    x25_sendcause(idb, lci, X25_RESET, X25_RESET_LOCAL_PROC_ERR,
		X25_DIAG_PACKET_TOO_LONG, NULL);
	    datagram_done(pak);
	} else if (!lci->lci_allow_int_cnf) {
	    /*must have sent INT on this interface*/
	    x25_sendcause(idb, lci, X25_RESET, 
			  X25_RESET_LOCAL_PROC_ERR, X25_DIAG_UNAUTH_INTR_CONF,
			  NULL);
	    datagram_done(pak);
	} else {
	    if (lci->lci_swdata)
		x25_forward_interrupt_confirm(lci, pak);
	    else {
		datagram_done(pak);
		lci->lci_allow_int_cnf = FALSE;
	    }
	}
    } else {
	x25_sendcause(idb, lci, X25_RESET, X25_RESET_LOCAL_PROC_ERR,
	    X25_DIAG_UNIDENT_PACKET, NULL);
	datagram_done(pak);
    }
}

/*
 *  x25_reenter_d1
 *  Process a VC that has entered state D1 from state D2/D3.
 *  If a switched VC is finished with an internally generated RESET, we may
 *  have packets queued for output on this VC once it enters state D1.
 */

void x25_reenter_d1 (lcitype *lci)
{
    register paktype *pak;
    register packettype *p;
    lcitype *ilci;
    int pr, mbit, ps, len;

    /*
     *  leave a PVC connect timer ticking, but
     *  cancel a RESET retransmit timer or an idle timer
     */
    if (TIMER_RUNNING(lci->lci_timer) &&
	(!IS_LCI_PVC(lci->lci_idb->hwptr, lci->lci_lci) ||
	 lci->lci_state == X25_D2 || lci->lci_state == X25_D3)) {
	p_unqueue(&x25tQ, lci);
	TIMER_STOP(lci->lci_timer);
    }

    lci->lci_state = X25_D1;
    lci->lci_retransmit = 0;

    if (lci->lci_swtype == X25_ROUTE_IF) {
	ilci = lci->lci_swdata;
    } else {
	ilci = lci;
    }

    while ((pak = pak_dequeue(&lci->lci_resetQ))) {
	p = (packettype *)pak->datagramstart;

	if (p->x25_pti == X25_INTERRUPT) {
	    x25_forward_interrupt(ilci, pak);
	} else if ((p->x25_pti & X25_DATA_MASK) == 0) {
	    x25_data(pak, &pr, &mbit, &ps, &len);
	    pak->datagramsize -= len;		/* blech */
	    pak->datagramstart += len;
	    x25_forward_data(ilci, pak, p->x25_sn, len, pr);
	} else {
	    x25_data(pak, &pr, &mbit, &ps, &len);
	    x25_forward_ack(ilci, pak, p->x25_sn, pr);
	}
    }

    /*
     *  the VC timer is ticking if it's a PVC waiting to connect (reset in D1);
     *  if not, check if we should start a PVC connect timer (exited D2/D3);
     *  and check if we should start an SVC idle timer
     */
    if (TIMER_RUNNING(lci->lci_timer)) {
	; /* EMPTY */
    } else if (IS_LCI_PVC(lci->lci_idb->hwptr, lci->lci_lci)) {
	if (lci->lci_swdata == NULL &&
	    lci->lci_linktype == LINK_X25TCP &&
	    (x25_set_pvc_con_time(lci)))
	    x25_addtimer(lci);
    } else if (lci->lci_idle != 0) {
	TIMER_START(lci->lci_timer, lci->lci_idle);
	x25_addtimer(lci);
    }
}

/*
 *  x25_process_reset
 *  Process a Reset packet
 *
 *  The VC RESET procedure has end-to-end significance such that all
 *  of the logical channels that make up the connections will be RESET
 *  and all data/flow control/interrupt packets in transit will be discarded.
 *
 *  The handling of a RESET to ensure the destruction of all in-the-pipe
 *  data, flow control and interrupt packets can be much trickier than
 *  it would seem.  The following is an attempt at a complete set of
 *  scenarios for handling RESETs on VCs.
 *
 *  1. RESET of Encapsulation VCs
 *    For encapsulation VCs, the router is one of the connection's two
 *    endpoints, so the RESET handling is straightforward.
 *
 *    1.1 Reception of a RESET in state D1
 *	Reset the VC and return a RESET CONFIRM.
 *
 *    1.2 Reception of a RESET or RESET CONFIRM in state D2/D3
 *	Reset the VC and place it in state D1.
 *
 *  2. RESET of Switched VCs
 *    The most direct approach to handling a RESET of switched VCs would
 *    be to forward the RESET out the destination VC and hold the two VCs in
 *    the D2/D3 state until a RESET CONFIRM reply is received and forwarded.
 *
 *    An alternate approach is to forward the RESET out the destination VC
 *    (placing it in state D2/D3) and sending a RESET CONFIRM reply on the
 *    incoming VC (placing it in state D1); this approach maintains the
 *    end-to-end significance of the RESET event as long as the reset
 *    of both the incoming and outgoing VCs is an atomic process.
 *
 *    In either case we must be able to queue packets for a VC in state
 *    D2/D3 if the other VC is in state D1, meaning that it can receive data,
 *    flow control or interrupt packets (the first method may have this
 *    happen when, in response to an error, generates an outgoing RESET on
 *    both of the switched VCs, placing them both in state D2/D3).  The
 *    code implements the second method of RESET handling.
 *
 *
 *    2.1 RESET of a Locally Switched Connection
 *
 *	2.1.1 Reception of a RESET in state D1
 *
 *	  2.1.1.1 Reception of a RESET with destination VC in state D1
 *	    The internal queues and counters of both VCs are flushed,
 *	    the RESET is forwarded to the connecting VC, and a RESET CONFIRM
 *	    is returned on the incoming VC, putting it in state D1.
 *
 *	  2.1.1.2 Reception of a RESET with destination VC in state D2/D3
 *	    The internal queues and counters of both VCs are flushed (although
 *	    the retransmit counter of the outgoing VC is not zeroed), nothing
 *	    is sent to the connecting VC (leaving it in state D2/D3), and a
 *	    RESET CONFIRM is returned on the incoming VC, putting it in state
 *	    D1.
 *
 *	2.1.2 Reception of a RESET or RESET CONFIRM in state D2/D3
 *	  The incoming VC is put in state D1 and any data/flow control/
 *	  interrupt packets on the reset queue (from the connecting VC) are
 *	  output.
 *
 *	2.1.3 Reception of data/flow control/interrupt, dest. in state D2/D3
 *	  The packet is queued to the connecting VC's reset hold queue.
 *
 *    2.2 RESET of a Remotely Switched Connection
 *	The RESET of a remotely switched connection is more complicated
 *	because the data pipe between the two routers, a TCP connection,
 *	is essentially a VC but does not carry a VC data structure.  The
 *	data needed are 1) a state indication to differentiate between
 *	state D1 and D2/D3 and 2) a queue for packets to be forwarded
 *	once the TCP connection exits state D2/D3.
 *
 *	The state is determined using the lci_tcp_reset_sent flag, which is
 *	FALSE for state D1 and TRUE for state D2/D3.
 *
 *	*IMPORTANT*
 *	The queue used is the *same* lci_resetQ structured used to queue
 *	packets to be forwarded out the X.25 VC once it exits state D2/D3;
 *	we're able to do this because the direction the packets are
 *	forwarded can be determined by which end (the X.25 VC or the TCP
 *	connection) exited state D2/D3.
 *
 *	In addition to these data, we must also be able to properly handle
 *	a path to a router that does not confirm a RESET across the TCP
 *	connection (all 9.1 code and prior).  This is flagged using
 *	lci_confirm_tcp_reset, which defaults to FALSE for backwards
 *	compatibility.
 *
 *	2.2.1 Reception of a RESET in state D1
 *
 *	  2.2.1.1 RESET, far end will not ack. RESET over TCP
 *	    This scenario is handled so as to be compatible with our existing
 *	    code base.  The internal queues and counters of the VC are flushed,
 *	    the RESET is forwarded over the TCP connection, and a RESET CONFIRM
 *	    is returned on the VC, putting it in state D1.
 *
 *	  2.2.1.2 RESET, far end will ack. RESET over TCP
 *	    The internal queues and counters of the VC are flushed, the RESET
 *	    is forwarded over the TCP connection, the VC is marked to wait
 *	    for a RESET confirmation from the TCP connection, and a RESET
 *	    CONFIRM is returned on the VC, putting it in state D1.
 *
 *	  2.2.1.3 RESET, waiting for RESET ack. from far end
 *	    The internal queues and counters of the VC are flushed, the RESET
 *	    packet is discarded, and a RESET CONFIRM is returned on the VC,
 *	    putting it in state D1.
 *
 *	2.2.2 Reception of a RESET or RESET CONFIRM in state D2/D3
 *	  The incoming VC is put in state D1 and any data/flow control/interrupt
 *	  packets on the reset queue (from the TCP connection) are output.
 *
 *	2.2.3 Reception of a RESET over TCP
 *
 *	  2.2.3.1 TCP RESET in state D1, TCP far end doesn't expect ack.
 *		  of RESET
 *	    This scenario is handled so as to be compatible with our existing
 *	    code base.  The RESET is forwarded putting it in state D2/D3.
 *
 *	  2.2.3.2 TCP RESET in state D2/D3, TCP far end doesn't expect ack.
 *		  of RESET
 *	    This scenario is handled so as to be compatible with our existing
 *	    code base.  The RESET is discarded and the VC's internal queues and
 *	    counters are flushed (although the retransmit counter of the VC is
 *	    not zeroed) leaving the VC in state D2/D3.
 *
 *	  2.2.3.3 TCP RESET in state D1, TCP far end expects ack. of RESET
 *	    A RESET CONFIRM is returned over the TCP connection and the RESET
 *	    is forwarded out the VC putting it in state D2/D3.
 *
 *	  2.2.3.4 TCP RESET in state D2/D3, TCP far end expects ack. of RESET
 *	    A RESET CONFIRM is returned over the TCP connection, the RESET
 *	    is discarded and the internal queues and counters of the VC are
 *	    flushed (although the retransmit counter of the VC is not zeroed),
 *	    leaving the VC in state D2/D3.
 *
 *	  2.2.3.5 TCP RESET or RESET CONFIRM while waiting for a TCP RESET ack.
 *	    The VC's flag to wait for a TCP RESET confirmation is cleared and
 *	    any packets on the reset hold queue are forwarded over the TCP
 *	    connection.
 *
 *	2.2.4 Reception of data/flow control/interrupt over TCP while waiting
 *	      for a TCP RESET ack.
 *	  The incoming packet is discarded.
 *
 *	2.2.5 Reception of data/flow control/interrupt over TCP while in
 *	      state D2/D3
 *	  The packet is queued to the VC's reset hold queue.
 */

void x25_process_reset (
    lcitype *lci,
    paktype *pak,
    uchar cause,
    uchar diag)
{
    boolean conf = TRUE;

    lci->lci_resetsrcvd++;

    /*
     * reset the VC
     */
    x25_call_reset(lci, FALSE);

    /*
     *  forward the RESET over a locally or remotely switched connection
     */
    if (lci->lci_swdata) {
	x25_forward_reset(lci, pak, cause, diag);
    } else {
	datagram_done(pak);

	/*
	 *  special handling for unconnected switched PVCs
	 */
	if (lci->lci_linktype == LINK_X25 &&
	    IS_LCI_PVC(lci->lci_idb->hwptr, lci->lci_lci)) {
	    /*
	     *  force an "out of order" RESET collision
	     */
	    x25_sendcause(lci->lci_idb->hwptr, lci, X25_RESET,
			X25_RESET_NETWORK_OUT_OF_ORDER, X25_DIAG_NO_INFO, NULL);
	    conf = FALSE;
	}
    }

    /*
     *  confirm the incoming RESET
     */
    if (conf)
 	sendpacket(lci->lci_idb->hwptr, lci, X25_RESET_CONFIRMATION, 0, NULL);

    reg_invoke_media_x25_notify_event(lci->lci_linktype, 
	    lci, X25_RESET_IND_EVENT, cause, diag,  NULL);

    x25_reenter_d1(lci);

    if (!ISIEEEMSK(lci->lci_idb->hwptr)) {
	reg_invoke_x25Reset_event(lci);
    }
}

/*
 * x25_d2_d3
 * DTE reset request
 */

void x25_d2_d3 (
    register hwidbtype *idb,
    register paktype *pak,
    register lcitype *lci)
{
    register packettype *p = (packettype *)pak->datagramstart;
    uchar *c = (uchar *)p + X25HEADERBYTES;

    /*
     *  we don't need to worry about state D3 as a DCE or state D2 as a DTE
     *  because we confirm a RESET locally and 'ripple' a RESET through the
     *  X.25 connection instead of holding the state for a far-end RESET
     *  CONFIRMATION (note that the RESET still has end-to-end significance).
     */

    if (p->x25_pti == X25_RESET) {
	if ((*c == X25_RESET_DTE_ORIGINATED) ||
	    (*c & X25_RESET_DTE_ORIGINATED_M)) {
	    idb->x25_dte_resets++;
	    lci->lci_dte_resets++;
	} else {
	    idb->x25_dce_resets++;
	    lci->lci_dce_resets++;
	    if (*c == X25_RESET_LOCAL_PROC_ERR)
		idb->x25_errors_accused++;
	}
	if (pak->datagramsize < X25HEADERBYTES + X25_SCAUSE_DATA) {
	    x25_sendcause(idb, lci, X25_RESET, X25_RESET_LOCAL_PROC_ERR,
		X25_DIAG_PACKET_TOO_SHORT, NULL);
	} else if (pak->datagramsize > X25HEADERBYTES + X25_CAUSE_DATA) {
	    x25_sendcause(idb, lci, X25_RESET, X25_RESET_LOCAL_PROC_ERR,
		X25_DIAG_PACKET_TOO_LONG, NULL);
	} else {
	    /*
	     *  handle a RESET collision
	     */
	    if (lci->lci_pvc_con_d2_d3) {
		/*
		 *  the PVC was connected while we were in the D2/D3 state--
		 *  generate the 'network operational' RESET
		 */
		lci->lci_pvc_con_d2_d3 = FALSE;
		lci->lci_state = X25_D1;
		x25_sendcause(idb, lci, X25_RESET,
			X25_RESET_NETWORK_OPERATIONAL, X25_DIAG_NO_INFO, NULL);
	    } else {
		lci->lci_resetsrcvd++;
		x25_reenter_d1(lci);
	    }
	}
	datagram_done(pak);
    } else if (p->x25_pti == X25_RESET_CONFIRMATION) {
	if (pak->datagramsize < X25HEADERBYTES) {
	    x25_sendcause(idb, lci, X25_RESET, X25_RESET_LOCAL_PROC_ERR,
		X25_DIAG_PACKET_TOO_SHORT, NULL);
	} else if (pak->datagramsize > X25HEADERBYTES) {
	    x25_sendcause(idb, lci, X25_RESET, X25_RESET_LOCAL_PROC_ERR,
		X25_DIAG_PACKET_TOO_LONG, NULL);
	} else {
	    if (lci->lci_pvc_con_d2_d3) {
		/*
		 *  the PVC was connected while we were in the D2/D3 state--
		 *  generate the 'network operational' RESET
		 */
		lci->lci_pvc_con_d2_d3 = FALSE;
		lci->lci_state = X25_D1;
		x25_sendcause(idb, lci, X25_RESET,
			X25_RESET_NETWORK_OPERATIONAL, X25_DIAG_NO_INFO, NULL);
	    } else {
		x25_reenter_d1(lci);
	    }
	}
	datagram_done(pak);
    } else {
	datagram_done(pak);
    }
}
