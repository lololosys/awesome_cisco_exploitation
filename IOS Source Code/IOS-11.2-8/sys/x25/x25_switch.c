/* $Id: x25_switch.c,v 3.6.10.7 1996/08/28 13:23:46 thille Exp $
 * $Source: /release/112/cvs/Xsys/x25/x25_switch.c,v $
 *------------------------------------------------------------------
 * x25_switch.c -- X25 Packet Switch
 *
 * November 1988, Greg Satz
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x25_switch.c,v $
 * Revision 3.6.10.7  1996/08/28  13:23:46  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.10.6  1996/08/07  09:04:53  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.6.10.5  1996/07/12  23:02:39  swenson
 * CSCdi61933:  pvc-setup can be delayed indefinitely
 * save remaining time on pvc connect timer if interrupted
 * Branch: California_branch
 *
 * Revision 3.6.10.4  1996/06/28  23:32:59  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.6.10.3  1996/05/17  12:17:26  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.2.2  1996/04/03  22:18:27  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.6.2.1  1996/03/01  16:31:51  ppearce
 * CSCdi50498:  IBU modularity - Implement LLC subblock
 * Branch: IbuMod_Calif_branch
 *   Move LLC fields from idbtype struct to LLC subblock
 *
 * Revision 3.6.10.2  1996/05/11  01:04:01  atsao
 * CSCdi56322:  Some X.25 devices cannot handle our Call Connected packet
 * format
 * Branch: California_branch
 * While switching a x25 call connected packet, make sure the packet is
 * forwarded with the address block if there is one in the original call
 * connected packet.
 *
 * Revision 3.6.10.1  1996/03/18  22:50:40  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.3  1996/03/16  07:57:50  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.2.2  1996/03/13  02:13:52  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.2.1  1996/03/07  11:20:00  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6  1996/02/23  23:21:38  carvalho
 * CSCdi41580:  Calling Address Ext. facility modified while X25 switching
 * Save digit count/authority field for called and calling address
 * extension facilities for re-use when call is forwarded.
 *
 * Revision 3.5  1996/01/22  07:46:44  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1995/11/21  08:27:38  ahh
 * CSCdi42413:  XOT: cannot tune TCP keepalive behavior
 * Allow keepalive rate to be controlled for XOT connections.
 *
 * Revision 3.3  1995/11/17  18:08:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:52:17  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:54:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/10/14  01:14:45  carvalho
 * CSCdi42195:  Bus error in x25_flush_call.  Clear the pointers between
 * the lci for the incoming call and the lci for the outgoing call before
 * deleting the new lci when the outgoing call fails.
 *
 * Revision 2.5  1995/10/09  18:15:04  carvalho
 * CSCdi40830:  CMNS X25 CALL to down interface causes bus error @
 * x25_forward_clear.  Clear the pointers between the lci for the
 * incoming call and the lci for the new outgoing call before deleting
 * the new lci when the outgoing call fails.
 *
 * Revision 2.4  1995/10/02  22:02:06  carvalho
 * CSCdi39783:  CMNS call to ethernet destination fails if LLC2 not open.
 *
 * Revision 2.3  1995/09/02  08:34:55  pmorton
 * CSCdi38709:  CMNS generates X25 packets over ethernet 3 bytes too short
 *
 * x25_prpack.  Just use a char pointer instead of allocating a string on
 * the stack
 *
 * x25_encaps.  Use payload size for the packet length to pass in to llc2
 * _encapsulate
 *
 * x25_process_data/x25_forward_data.  Pass LINK_CMNS packets to
 * x25_forward_data.  Remove bogus pak->datagramstart/pak->datagramsize
 * computations from x25_forward_data.
 *
 * x25_llc2_clsproc. Remove naughty word from buginf.
 *
 * Revision 2.2  1995/07/20  07:38:31  hampton
 * Convert X.25 to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37392]
 *
 * Revision 2.1  1995/06/07 23:22:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "logger.h"
#include "../x25/msg_x25.c"		/* Not a typo, see logger.h */
#include "../ui/debug.h"
#include "address.h"
#include "config.h"
#include "packet.h"
#include "connect.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "../if/network.h"
#include "../ip/ip_registry.h"
#include "../tcp/tcpdriver.h"
#include "lapb.h"
#include "x25.h"
#include "x25_address.h"
#include "x25_switch.h"
#include "x25_route.h"
#include "x25_tcp.h"
#include "pad.h"
#include "x25_proto.h"
#include "x25_debug.h"
#include "x25_pbp.h"
#include "../llc2/llc2.h"
#include "../llc2/llc2_sb.h"
#include "ieee.h"

x25routeentrytype consrtp;	/* fake route for cons */

#define pvc_setup_data struct _pvc_setup_data

pvc_setup_data {
    uchar	version;
    uchar	msg_code;
    uchar	init_if_name_len;	/* initiator's interf. name length */
    uchar	init_vc_hi;		/* initiator's VC number */
    uchar	init_vc_lo;
    uchar	resp_if_name_len;	/* responder's interf. name length */
    uchar	resp_vc_hi;		/* responder's VC number */
    uchar	resp_vc_lo;
    uchar	win;			/* sender's flow control values */
    uchar	wout;
    uchar	pin;
    uchar	pout;
    uchar	names[1];		/* initiator & responder names */
};

char *const x25_pvc_err_str[X25_PVC_CODES] = {
					 "waiting to connect",
					 "1?", "2?", "3?", "4?",
					 "5?", "6?", "7?",
					 "dest. disconnected",
					 "PVC/TCP connection refused",
					 "PVC/TCP routing error",
					 "PVC/TCP connect timed out",
					 "12?", "13?", "14?", "15?",
					 "trying to connect via TCP",
					 "awaiting PVC-SETUP reply",
					 "connected",
					 "no such dest. interface",
					 "dest. interface is not up",
					 "non-X.25 dest. interface",
					 "no such dest. PVC",
					 "dest. PVC config. mismatch",
					 "mismatched flow control values",
					 "can't support flow control values",
					 "PVC setup protocol error"
					};


/*
 * x25_switching
 * Returns boolean determing whether we are doing X.25 switching
 */
boolean x25_switching (ulong sport, ulong dport, ipaddrtype fhost)
{
    return(x25_routing);
}

/*
 * x25_switch_packet
 * Handle forwarding of data from stream pipe (TCP).
 */
void x25_switch_packet (
    tcp_encaps *t,
    paktype *pak,
    lcitype *lci)
{
    packettype *p;
    uchar *c, *cudptr, cause, diag, caddr[2*X25_ADDRESS_LENGTH];
    int pr, mbit, ps, len, cudlen, flen, tlen, faclen;
    uchar size;
    addrtype dst;
    x25routeentrytype *pt;
    hwidbtype *hwidb;
    hwidbtype *lciidb;
    llctype *llc2_cb;
    pvc_setup_data *pvc;
    boolean pbp_local_ack = FALSE;

    p = (packettype *)pak->datagramstart;

    /*
     * discard if we don't have a valid packet type for a null lci
     */
    if (lci == NULL &&
	p->x25_pti != X25_CALL_REQUEST && p->x25_pti != X25_PVCSETUP) {
	datagram_done(pak);
	return;
    }

    c = (uchar *)p + X25HEADERBYTES;
    switch (p->x25_pti) {
    case X25_PVCSETUP:
	pvc = (pvc_setup_data *)c;
	if (pvc->msg_code >= X25_PVC_CODES)
	    pvc->msg_code = X25_PVC_UNKNOWN_ERR;
	if (x25_debug || x25event_debug)
	    x25_prpack(NULL, pak, lci, 'I');

	/*
	 *  sanity check
	 */
	if ((pak->datagramsize < (X25HEADERBYTES + X25_PVCSETUP_BYTES +
		pvc->init_if_name_len + pvc->resp_if_name_len)) ||
	    (pvc->version != X25_PVCSETUP_VER) ||
	    ((lci == NULL) && (pvc->msg_code != X25_PVC_DOWN))) {
	    if (!lci ||
		((t != lci->lci_swdata) &&
		 (t != lci->lci_pvc_swdata))) {
		x25_switch_tcp_close(NULL, t, NULL);
	    }

	    if (lci) {
		x25_switch_tcp_close(lci, NULL, NULL);
		lci->lci_pvc_status = X25_PVC_UNKNOWN_ERR;
		if (TIMER_RUNNING(lci->lci_timer) &&
		    lci->lci_state != X25_D2 && lci->lci_state != X25_D3) {
		    p_unqueue(&x25tQ, lci);
		    TIMER_STOP(lci->lci_timer);
		}
		errmsg(&msgsym(PVCBAD, X25),
			lci->lci_idb->hwptr->hw_namestring,
			lci->lci_lci,
			x25_pvc_err_str[lci->lci_pvc_status]);
	    }
	    datagram_done(pak);
	    break;
	}

	if (lci == NULL) {
	    /*
	     *  we have a PVC setup request
	     */

	    /*
	     *  find the PVC specified
	     */
	    FOR_ALL_HWIDBS(hwidb) {
		/*
		 * do a case-insensitive string match on the interface name
		 */
		if ((pvc->resp_if_name_len == strlen(hwidb->hw_namestring)) &&
		    (strncasecmp(hwidb->hw_namestring,
				 &pvc->names[pvc->init_if_name_len],
				 pvc->resp_if_name_len) == 0))
		    break;
	    }

	    if (hwidb == NULL)
		pvc->msg_code = X25_PVC_BAD_IF;
	    else if (!hw_interface_up(hwidb))
		pvc->msg_code = X25_PVC_IF_NOT_UP;
	    else if (!is_x25(hwidb))
		pvc->msg_code = X25_PVC_IF_NOT_X25;
	    else if (!IS_LCI_PVC(hwidb,
				 (pvc->resp_vc_hi << 8) | pvc->resp_vc_lo) ||
		     (lci = x25_lci_lookup(hwidb,
				(pvc->resp_vc_hi << 8) | pvc->resp_vc_lo,
				NULL)) == NULL)
		pvc->msg_code = X25_PVC_NO_PVC;
	    else if (lci->lci_swtype != X25_ROUTE_IPADDR ||
		     (pt = lci->lci_rtp,
		      pt->pvc_remote_if_len != pvc->init_if_name_len) ||
		     (strncasecmp(pt->pvc_remote_if, pvc->names,
				  pt->pvc_remote_if_len) != 0) ||
		     (int)pt->pvc_dest_vc !=
			((pvc->init_vc_hi << 8) | pvc->init_vc_lo) ||
		     lci->lci_swdata)
		pvc->msg_code = X25_PVC_CROSS_CON;

	    /*
	     *  if the PVC exists, compare its configuration with the message
	     */
	    if (pvc->msg_code != X25_PVC_DOWN) {
		lci = NULL;
	    } else if (lci->lci_win != pvc->wout ||
		       lci->lci_wout != pvc->win ||
		       lci->lci_pin != pvc->pout ||
		       lci->lci_pout != pvc->pin) {
		pvc->msg_code = X25_PVC_FLOW_MISMATCH;

		/*
		 *  return our flow control values
		 */
		pvc->win = lci->lci_win;
		pvc->wout = lci->lci_wout;
		pvc->pin = lci->lci_pin;
		pvc->pout = lci->lci_pout;
	    } else if ((lci->lci_win > lci->lci_wout ?
			lci->lci_win : lci->lci_wout) >= hwidb->x25_modulo ||
		       (1 << (lci->lci_pin > lci->lci_pout ?
			lci->lci_pin : lci->lci_pout)) > x25_payload(hwidb)) {
		pvc->msg_code = X25_PVC_BAD_FLOW;
	    } else {
		/*
		 *  we can connect the PVC
		 */
		pvc->msg_code = X25_PVC_UP;
	    }

	    if (lci) {
		lci->lci_pvc_status = pvc->msg_code;

		/*
		 *  cancel a connect timer
		 */
		if (TIMER_RUNNING(lci->lci_timer) &&
		    lci->lci_state != X25_D2 && lci->lci_state != X25_D3) {
		    p_unqueue(&x25tQ, lci);
		    TIMER_STOP(lci->lci_timer);
		}
	    }

	    /*
	     *  deal with the incoming TCP connection and packet
	     */
	    if (x25_debug || x25event_debug)
		x25_prpack(NULL, pak, lci, 'O');

	    if (pvc->msg_code != X25_PVC_UP) {
		if (pvc->msg_code != X25_PVC_IF_NOT_UP) {
		    pvc->names[pvc->init_if_name_len + pvc->resp_if_name_len] =
			'\0';
		    errmsg(&msgsym(PVCBAD, X25),
			   &pvc->names[pvc->init_if_name_len],
			   (pvc->resp_vc_hi << 8) | pvc->resp_vc_lo,
			   x25_pvc_err_str[pvc->msg_code]);
		}
		x25_switch_tcp_close(NULL, t, pak);
	    } else {
		/*
		 *  connect the PVC
		 */
		lci->lci_swdata = t;
		lci->lci_tcp_reset_sent = FALSE;
		x25_switch_tcp_send(lci, pak);
		reg_invoke_ip_tcpdriver_update(lci->lci_swdata, NULL, 0, lci);

		/*
		 *  we need to RESET the PVC with a 'network operational' cause
		 */
		if (lci->lci_state != X25_D2 && lci->lci_state != X25_D3) {
		    x25_sendcause(hwidb, lci, X25_RESET,
			X25_RESET_NETWORK_OPERATIONAL, X25_DIAG_NO_INFO, NULL);
		} else {
		    lci->lci_pvc_con_d2_d3 = TRUE;
		}
	    }
	} else {
	    /*
	     *  we have a PVC setup reply
	     */

	    hwidb = lci->lci_idb->hwptr;

	    if (pvc->msg_code == X25_PVC_UP) {
		/*
		 *  connect the PVC
		 */
		lci->lci_pvc_status = X25_PVC_UP;

		/*
		 *  this (outgoing) connection is in lci_pvc_swdata
		 *  and any colliding (incoming) connection in lci_swdata
		 */
		lci->lci_pvc_swdata = lci->lci_swdata;
		lci->lci_swdata = t;

		if (lci->lci_state != X25_D2 && lci->lci_state != X25_D3) {
		    x25_sendcause(hwidb, lci, X25_RESET,
			X25_RESET_NETWORK_OPERATIONAL, X25_DIAG_NO_INFO, NULL);
		} else {
		    lci->lci_pvc_con_d2_d3 = TRUE;
		}
	    } else {
		/*
		 *  kill the connection
		 */
		x25_switch_tcp_close(lci, NULL, NULL);

		switch (pvc->msg_code) {
		  case X25_PVC_BAD_IF:
		  case X25_PVC_IF_NOT_UP:
		  case X25_PVC_IF_NOT_X25:
		  case X25_PVC_NO_PVC:
		  case X25_PVC_CROSS_CON:
		  case X25_PVC_FLOW_MISMATCH:
		  case X25_PVC_BAD_FLOW:
		    lci->lci_pvc_status = pvc->msg_code;
		    break;
		  default:
		    lci->lci_pvc_status = X25_PVC_UNKNOWN_ERR;
		    break;
		}

		if (pvc->msg_code != X25_PVC_IF_NOT_UP)
		    errmsg(&msgsym(PVCBAD, X25), hwidb->hw_namestring,
			   lci->lci_lci, x25_pvc_err_str[pvc->msg_code]);

		/*
		 *  stop any connect timer and restart, if applicable
		 */

		if (TIMER_RUNNING(lci->lci_timer) &&
		    lci->lci_state != X25_D2 && lci->lci_state != X25_D3) {
		    p_unqueue(&x25tQ, lci);
		    TIMER_STOP(lci->lci_timer);
		}

		if (!TIMER_RUNNING(lci->lci_timer)) {
		    if (x25_set_pvc_con_time(lci))
			x25_addtimer(lci);
		}
	    }

	    datagram_done(pak);
	}

	break;
    case X25_CALL_REQUEST:
	if (x25_debug || x25event_debug)
	    x25_prpack(NULL, pak, NULL, 'I');

	if (lci) {
	    errmsg(&msgsym(XOTPROTOCOL, X25),
		   reg_invoke_ip_tcpdriver_fhost(t), "Call Collision");
	    x25_sendcause(lci->lci_idb->hwptr, lci, X25_CLEAR,
			  X25_CLEAR_REMOTE_PROC_ERR, X25_DIAG_INVALID_PTI,NULL);
	    datagram_done(pak);
	    return;
	}

	/*
	 *  get the destination address
	 */
	flen = (*c >> 4) & 0xf;
	tlen = *c & 0xf;
	size = bcdlen_inline(*c++);
	faclen = *(c + size);
	bcdtochar(c, caddr, flen+tlen);
	memset(&dst, 0, sizeof(addrtype));
	dst.type = ADDR_X121;
	dst.length = tlen;
	bcopy(caddr, dst.x121_addr, dst.length);

	cudptr = NULL;
	cudlen = 0;
	if (pak->datagramsize > X25HEADERBYTES + X25_CALL_DATA + size +
	    faclen) {
	    cudptr = c + size + faclen + 1;
	    cudlen = pak->datagramsize - (cudptr -
		(uchar *)pak->datagramstart);

	    /*
	     *  do packet by packet compression if the first byte of
	     *  the CUD is X25_PBP_PROTOCOL_ID
	     */
	    if (cudlen && (*cudptr == X25_PBP_PROTOCOL_ID)) {
		if (x25_switching_pbp_compress) {
		    pbp_local_ack = TRUE;
		}
	    }
	}

	/*
	 * Can we get there from here?
	 */
	pt = x25_forus_stream(&dst, cudptr, cudlen);
	if (pt == NULL) {
	    if (x25_debug || x25event_debug)
		buginf("\nX25-Switch: No route for %*s from %*s", tlen, caddr,
		    flen, caddr+tlen);
	    x25_switch_tcp_close(NULL, t, NULL);
	    datagram_done(pak);
	    break;
	}

	lci = x25_lci_enter(pt->idb, X25_NO_VC_ASSIGNED, NULL);
	if (lci == NULL) {
	    x25_switch_tcp_close(NULL, t, NULL);
	    datagram_done(pak);
	    break;
	}

	if (pbp_local_ack)
	    lci->lci_pbp_local_ack = TRUE;

	lci->lci_linktype = LINK_X25TCP;
	lciidb = lci->lci_idb->hwptr;
	lci->lci_rtp = pt;
	lci->lci_swtype = X25_ROUTE_IPADDR;
	lci->lci_swdata = t;
	bcopy(&dst, &lci->lci_dst_addr, sizeof(addrtype));
	lci->lci_src_addr.type = ADDR_X121;
	lci->lci_src_addr.length = flen;
	bcopy(&caddr[tlen], &lci->lci_src_addr.x121_addr, flen);
	lci->lci_src_addr.x121_addr[flen] = '\0';
	lci->lci_dbit = p->x25_d;

	/*
	 *  At this point we must parse the CALL facilities;
	 *  older code may not indicate the expected window and packet
	 *  sizes for the call, so we need to detect if these flow
	 *  control facilities are not present and, if not present,
	 *  force uniformly acceptable values on the CALL CONFIRM
	 *  to guarantee matching values on either end of the TCP
	 *  connection.  It is important to note, however, that BFE
	 *  interfaces cannot indicate any facilities on a CALL CONFIRM.
	 */
	lci->lci_win = 0;
	lci->lci_wout = 0;
	lci->lci_pin = 0;
	lci->lci_pout = 0;
	if (!x25_parsefacility(lciidb, lci, c + size, X25_CALL_REQUEST,
				&cause, &diag, X25_ROLE_CALLING_DTE))
	    goto failure;
	if (lci->lci_win == 0 || lci->lci_pin == 0) {
	    if (ISBFE(lciidb))
		goto failure;
	    /*
	     *  if the TCP-use-interface-defaults flag is set (see bug
	     *  report CSCdi13759) force the interface defaults otherwise
	     *	force universally acceptable values; these forced value are
	     *  indicated on the CALL CONFIRM
	     */
	    if (lci->lci_win == 0) {
		lci->lci_win = x25_tcp_use_if_defs ?
			lci->lci_idb->hwptr->x25_win : X25_DEFAULT_WIN;
		lci->lci_wout = x25_tcp_use_if_defs ?
			lci->lci_idb->hwptr->x25_wout : X25_DEFAULT_WOUT;
		lci->lci_force_winsize = TRUE;
	    }
	    if (lci->lci_pin == 0) {
		lci->lci_pin = x25_tcp_use_if_defs ?
			lci->lci_idb->hwptr->x25_pin : X25_DEFAULT_IPS;
		lci->lci_pout = x25_tcp_use_if_defs ?
			lci->lci_idb->hwptr->x25_pout : X25_DEFAULT_OPS;
		lci->lci_force_pksize = TRUE;
	    }
	}
	if (x25_fix_flow_fac(lci) && ISBFE(lciidb))
	    goto failure;
	llc2_cb = pak->llc2_cb;
	if (x25_forward_call(lci, pak, cudptr, cudlen, &size) !=
	    X25_FORWARD_OK) {
	    pak = NULL;
	  failure:
	    if (pak)
		datagram_done(pak);
	    lciidb->x25_callsrcvdfailed++;
	    lciidb->x25_callsfrwdfailed++;
	    x25_switch_tcp_close(lci, NULL, NULL);
	    x25_lci_delete(lci);
	} else {
	    reg_invoke_ip_tcpdriver_update(t, NULL, 0, lci);
	    x25_switch_tcp_setparms(t, pt->tcp_keepalive_period,
				    pt->tcp_keepalive_tries);
	}
	break;
    case X25_CALL_CONNECTED:
	if (p->x25_q) {	/* PBP code */
	    p->x25_q = 0;	/* restore to standard x25 */
	    if (x25_switching_pbp_compress)
		lci->lci_pbp_local_ack = TRUE; /* remote end wants local ack */
	}

	if (x25_debug || x25event_debug)
	    x25_prpack(NULL, pak, lci, 'I');
	if (pak->datagramsize > X25HEADERBYTES) {
	    size = bcdlen_inline(*c++);
	    (void) x25_parsefacility(lci->lci_idb->hwptr, lci,
				     c + size, X25_CALL_CONNECTED, &cause,
				     &diag, X25_ROLE_CALLED_DTE);
	}
	x25_forward_call_connected(lci, pak);
	break;
    case X25_CLEAR:
	if (x25_debug || x25event_debug)
	    x25_prpack(NULL, pak, lci, 'I');
	x25_forward_clear(lci, pak, c[0], c[1]);
	break;
    case X25_RESET:
	if (x25_debug || x25event_debug)
	    x25_prpack(NULL, pak, lci, 'I');
	if (lci->lci_tcp_reset_sent) {
	    /*
	     *  we have a RESET collision over the TCP connection
	     */
	    lci->lci_tcp_reset_sent = FALSE;
	    datagram_done(pak);
	    if (lci->lci_state == X25_P4 || lci->lci_state == X25_D1)
		x25_reenter_d1(lci);
	} else
	    x25_forward_reset(lci, pak, c[0], c[1]);
	break;
    case X25_RESET_CONFIRMATION:
	if (x25_debug || x25event_debug)
	    x25_prpack(NULL, pak, lci, 'I');
	lci->lci_tcp_reset_sent = FALSE;
	datagram_done(pak);
	if (lci->lci_state == X25_P4 || lci->lci_state == X25_D1)
	    x25_reenter_d1(lci);
	break;
    case X25_INTERRUPT:
	if (x25_debug || x25event_debug)
	    x25_prpack(NULL, pak, lci, 'I');
	if (lci->lci_tcp_reset_sent)
	    datagram_done(pak);
	else
	    x25_forward_interrupt(lci, pak);
	break;
    case X25_INTERRUPT_CONFIRMATION:
	if (x25_debug || x25event_debug)
	    x25_prpack(NULL, pak, lci, 'I');
	if (lci->lci_tcp_reset_sent)
	    datagram_done(pak);
	else
	    x25_forward_interrupt_confirm(lci, pak);
	break;
    default:
	if (x25_debug)
	    x25_prpack(NULL, pak, lci, 'I');

	/* discard data in the pipe during RESET or martians */
	if (lci->lci_tcp_reset_sent || !x25_data(pak, &pr, &mbit, &ps, &len))
	    datagram_done(pak);
	else if ((p->x25_pti & X25_DATA_MASK) == X25_DATA) {
	    pak->datagramsize -= len;
	    pak->datagramstart += len;
	    if (mbit)
		pak->flags |= PAK_MBIT;
	    x25_forward_data(lci, pak, p->x25_sn, len, pr);
	} else {
	    if (lci->lci_pbp_local_ack) {	/* PBP near inner node rx */
		if (!x25_pbp_inner_node_receive(lci, p)) {
		    x25_pbp_flush_call(lci);
		    datagram_done(pak);
		    return;
		}
	    }

	    x25_forward_ack(lci, pak, p->x25_sn, pr);
	}
	break;
    }
}

/*
 * x25_forward_call
 * This routine must forward the CALL REQUEST packet onward towards its
 * final destination. Depending on its source contributes towards its
 * destination. Packets input from streams can only go out over serial
 * interfaces. Packets input from serial interfaces can go out over another
 * serial interface or over a stream. Note we must preserve the origlci LCN
 * as our caller uses it if we fail.
 */
uint x25_forward_call (
    lcitype *lci,
    paktype *pak,
    uchar *cud,
    uint cud_len,
    uchar *diag)
{
    x25routeentrytype *pt;
    lcitype *lci2;
    addrtype src;
    addrtype dst;

    lci->lci_idb->hwptr->x25_callsfrwd++;
    pt = lci->lci_rtp;
    if (pt == NULL || pt->type == X25_ROUTE_ALIAS || !pak) {
	if (pak)
	    datagram_done(pak);
	return(X25_FORWARD_ERR);
    }

    /*
     * We do not support incoming stream routed to outgoing stream.
     */
    if (pt->type == X25_ROUTE_IPADDR && pak->linktype == LINK_X25TCP) {
	datagram_done(pak);
	return(X25_FORWARD_ERR);
    }

    /*
     *	handle replacement of source and destination addresses as needed--
     *  allow the maximum number of X.121 digits plus 1 for the null character
     */
    if (pt->repsrc == NULL) {
	src = lci->lci_src_addr;
    } else {
	if (regsub(pt->x121pattern, pt->repsrc,
		   src.x121_addr, X121_MAX_DIGITS + 1) < 0) {
            buginf("\nX25-Switch: Error rewriting calling address");
	    datagram_done(pak);
            return(X25_FORWARD_ERR);
        }
	src.type = ADDR_X121;
	src.length = strlen(src.x121_addr);
    }

    if (pt->repdst == NULL) {
	dst = lci->lci_dst_addr;
    } else {
	if (regsub(pt->x121pattern, pt->repdst,
		   dst.x121_addr, X121_MAX_DIGITS + 1) < 0) {
            buginf("\nX25-Switch: Error rewriting called address");
	    datagram_done(pak);
            return(X25_FORWARD_ERR);
        }
	dst.type = ADDR_X121;
	dst.length = strlen(dst.x121_addr);
    }

    /*
     *	handle routing to a TCP connection
     */
    if (pt->type == X25_ROUTE_IPADDR) {
	ipaddrtype lhost;

	/*
	 *  dispose of the CALL if we can't insert the flow control facilities
	 */
	if ((pak =
	     x25_insert_flow_facil(pak, lci->lci_wout, lci->lci_win,
				   lci->lci_pout, lci->lci_pin)) == NULL) {
	    return(X25_FORWARD_ERR);
	}

	if (pt->source_idb && pt->source_idb->ip_address &&
	    interface_up(pt->source_idb))
	    lhost = pt->source_idb->ip_address;
	else
	    lhost = 0;

	/* inputtime is used to detect CLEAR/CALL seq in x25_timer alt routing*/
	COPY_TIMESTAMP(lci->lci_starttime, pak->inputtime);

	if ((lci->lci_swdata = 
	    x25_switch_tcp_open(pt->ipaddr[lci->lci_alt_index], lhost,
				lci, diag)) == NULL) {
	    /*
	     *  remote route failed; try any alternates
	     */
	    if (++(lci->lci_alt_index) >= pt->ipaddr_max) {
		/*
		 *  no alternates remain
		 */
		lci->lci_alt_index = 0;
		datagram_done(pak);
		return(X25_FORWARD_ERR);
	    } else {
		/*
		 *  re-queue the packet to allow other timer task events
		 *  to be processed before re-trying this Call
		 */
		p_enqueue(&x25aQ, pak);
		return(X25_FORWARD_ALT);
	    }
	}
	x25_switch_tcp_setparms(lci->lci_swdata, pt->tcp_keepalive_period,
				pt->tcp_keepalive_tries);

	if (x25_debug || x25event_debug)
	    x25_prpack(NULL, pak, lci, 'O');

	x25_switch_tcp_send(lci, pak);
	return(X25_FORWARD_OK);
    }

    /*
     *	handle routing to an X.25 interface
     */

    if (pak->linktype != LINK_X25) {
	/*
	 *  we have an incoming remotely routed CALL
	 */
	lci2 = lci;
    } else {
	/*
	 *  we have an locally routed CALL
	 */
	lci2 = x25_lci_enter(pt->idb, X25_NO_VC_ASSIGNED, NULL);

	if (lci2 == NULL) {
	    datagram_done(pak);
	    return(X25_FORWARD_ERR);
	}

	lci2->lci_linktype = LINK_X25;

	/*
	 *  note that the incoming interface VC has the role of "called DTE"
	 *  while the outgoing interface VC has the role of "calling DTE"--
	 *  swap the flow control values to reflect this
	 */
	lci2->lci_pin = lci->lci_pout;
	lci2->lci_pout = lci->lci_pin;
	lci2->lci_win = lci->lci_wout;
	lci2->lci_wout = lci->lci_win;
	lci2->lci_tput_in = lci->lci_tput_out;
	lci2->lci_tput_out = lci->lci_tput_in;

	/*
	 *  lower any flow control values that are too large and mark
	 *  the incoming LCI if the new value(s) need to be indicated
	 *  on the CALL CONFIRM
	 */
	(void) x25_fix_flow_fac(lci2);
	lci->lci_force_winsize |= lci2->lci_force_winsize;
	lci->lci_force_pksize |= lci2->lci_force_pksize;

	lci2->lci_dbit = lci->lci_dbit;
	lci2->lci_fast = lci->lci_fast;
	lci2->lci_confirm_addr = lci->lci_confirm_addr;
	lci2->lci_reverse = lci->lci_reverse;
	lci2->lci_cug = lci->lci_cug;
	lci2->lci_lam = lci->lci_lam;
	lci2->lci_tdelay = lci->lci_tdelay;
	bcopy(&lci->lci_src_nsap, &lci2->lci_src_nsap, sizeof(addrtype));
	lci2->lci_src_nsap_digits = lci->lci_src_nsap_digits;
	bcopy(&lci->lci_dst_nsap, &lci2->lci_dst_nsap, sizeof(addrtype));
	lci2->lci_dst_nsap_digits = lci->lci_dst_nsap_digits;
	lci2->lci_rtp = lci->lci_rtp;
        lci2->lci_prec = lci->lci_prec;
	lci->lci_swtype = X25_ROUTE_IF;
	lci->lci_swdata = lci2;
	lci2->lci_swtype = X25_ROUTE_IF;
	lci2->lci_swdata = lci;

	/*
	 *  we need to allow forwarding of an NUID facility, so copy the
	 *  original VC's pointer and length into the outgoing VC structure;
	 *  note, however, that the pointer references the packet so after
	 *  sending the Call, these fields should be cleared (unless the VC
	 *  gets deleted) to avoid retaining an invalid pointer
	 */
	lci2->lci_nuid = lci->lci_nuid;
	lci2->lci_nuidlen = lci->lci_nuidlen;
    }

    /*
     *  set the (possibly modified) X.121 addresses of the outgoing VC
     */
    bcopy(&src, &lci2->lci_src_addr, sizeof(addrtype));
    bcopy(&dst, &lci2->lci_dst_addr, sizeof(addrtype));

    if (x25_sendcall(lci2, cud, cud_len)) {
	lci2->lci_nuid = NULL;
	lci2->lci_nuidlen = 0;
	lci->lci_nuid = NULL;
	lci->lci_nuidlen = 0;
	datagram_done(pak);
	return(X25_FORWARD_OK);
    }

    /*
     * Since lci2 was added to x25tQ
     * in x25_sendcall make sure it is
     * unqueued first before deleting.
     */
    if (checkqueue(&x25tQ, lci2)) {
	p_unqueue(&x25tQ, lci2);
	TIMER_STOP(lci2->lci_timer);
    }
    /*
     * break the association between the two lcis, since we're about
     * to delete one of them.  Also clear the pointer to the network
     * user ID, as it points into the pak we're discarding.
     */
    lci->lci_swtype = -1;
    lci->lci_swdata = NULL;
    lci2->lci_swtype = -1;
    lci2->lci_swdata = NULL;
    lci2->lci_nuid = NULL;
    lci2->lci_nuidlen = 0;
    lci->lci_nuid = NULL;
    lci->lci_nuidlen = 0;

    x25_lci_delete(lci2);

    datagram_done(pak);
    return(X25_FORWARD_ERR);
}

/*
 * x25_pvc_retry
 * Continue trying to establish a PVC
 */
void x25_pvc_retry (lcitype *lci)
{
    int otherpvc;
    uchar diag;
    lcitype *lci2;
    paktype *pak;
    packettype *p;
    x25routeentrytype *pt;
    hwidbtype *lciidb = lci->lci_idb->hwptr;
    pvc_setup_data *pvc;
    leveltype level;

    /*
     * get the route entry
     */
    pt = lci->lci_rtp;
    otherpvc = (int)pt->pvc_dest_vc;

    /* make sure any remainder time is zero'd */
    lci->lci_pvc_remainder = 0;

    if (!lciidb->x25_upstate) {
	;	/* EMPTY */
    } else if (lci->lci_swtype == X25_ROUTE_IF) {
	/*
	 * in order to connect, a switched PVC must:
	 *	go to a destination interface that is up,
	 *	go to an X.25 interface,
	 *	the destination VC must exist,
	 *	the destination VC must be a PVC,
	 *	go to a complimentarily configured PVC,
	 *	match the four flow control parameters
	 *	and be capable of operating with the flow control parameters
	 */
	if (!interface_up(pt->idb)) {
	    lci->lci_pvc_status = X25_PVC_IF_NOT_UP;
	} else if (!is_x25(pt->idb->hwptr)) {
	    lci->lci_pvc_status = X25_PVC_IF_NOT_X25;
	} else if (!IS_LCI_PVC(pt->idb->hwptr, otherpvc) ||
		 (lci2 = x25_lci_lookup(pt->idb->hwptr,
					otherpvc, NULL)) == NULL) {
	    lci->lci_pvc_status = X25_PVC_NO_PVC;
	} else if (lci2->lci_swtype != X25_ROUTE_IF ||
		   lci2->lci_rtp->idb != lci->lci_idb ||
		   (uint)lci2->lci_rtp->pvc_dest_vc != lci->lci_lci) {
	    lci->lci_pvc_status = X25_PVC_CROSS_CON;
	} else if (lci2->lci_pin != lci->lci_pout ||
		   lci2->lci_pout != lci->lci_pin ||
		   lci2->lci_win != lci->lci_wout ||
		   lci2->lci_wout != lci->lci_win) {
	    lci->lci_pvc_status = lci2->lci_pvc_status = X25_PVC_FLOW_MISMATCH;
	} else if ((lci->lci_win > lci->lci_wout ?
		    lci->lci_win : lci->lci_wout) >= lciidb->x25_modulo ||
		   (lci->lci_win > lci->lci_wout ?
		    lci->lci_win : lci->lci_wout) >=
			lci2->lci_idb->hwptr->x25_modulo ||
		   (1 << (lci->lci_pin > lci->lci_pout ?
		    lci->lci_pin : lci->lci_pout)) > x25_payload(lciidb) ||
		   (1 << (lci->lci_pin > lci->lci_pout ?
		    lci->lci_pin : lci->lci_pout)) >
			x25_payload(lci2->lci_idb->hwptr)) {
	    lci->lci_pvc_status = lci2->lci_pvc_status = X25_PVC_BAD_FLOW;
	} else {
	    /*
	     * connect the PVCs
	     */
	    lci->lci_pvc_status = X25_PVC_UP;
	    lci2->lci_pvc_status = X25_PVC_UP;

	    lci->lci_swdata = lci2;
	    lci2->lci_swdata = lci;

	    /*
	     * if a PVC is waiting for a RESET CONFIRM we can't send out
	     * a 'network operational' RESET; flag the VC to send a
	     * 'network operational' RESET on re-entry to state D1
	     */
	    if (lci->lci_state == X25_D2 || lci->lci_state == X25_D3) {
		lci->lci_pvc_con_d2_d3 = TRUE;
	    } else if (TIMER_RUNNING(lci->lci_timer)) {
		p_unqueue(&x25tQ, lci);
		TIMER_STOP(lci->lci_timer);
	    }
	    if (lci2->lci_state == X25_D2 || lci2->lci_state == X25_D3) {
		lci2->lci_pvc_con_d2_d3 = TRUE;
	    } else if (TIMER_RUNNING(lci2->lci_timer)) {
		p_unqueue(&x25tQ, lci2);
		TIMER_STOP(lci2->lci_timer);
	    }

	    x25_sendcause(lciidb, lci, X25_RESET,
			  X25_RESET_NETWORK_OPERATIONAL, X25_DIAG_NO_INFO,NULL);
	}

	if (lci->lci_pvc_status != X25_PVC_DOWN &&
	    lci->lci_pvc_status != X25_PVC_IF_NOT_UP &&
	    lci->lci_pvc_status != X25_PVC_UP) {
	    errmsg(&msgsym(PVCBAD, X25), lciidb->hw_namestring,
		   lci->lci_lci, x25_pvc_err_str[lci->lci_pvc_status]);
	}
    } else if (lci->lci_swtype == X25_ROUTE_IPADDR) {
	if ((lci->lci_win > lci->lci_wout ? lci->lci_win : lci->lci_wout) >=
	    lciidb->x25_modulo ||
	    (1 << (lci->lci_pin > lci->lci_pout ?
		lci->lci_pin : lci->lci_pout)) > x25_payload(lciidb)) {
	    lci->lci_pvc_status = X25_PVC_BAD_FLOW;
	} else {
	    ipaddrtype lhost;

	    diag = X25_DIAG_NO_INFO;
	    lci->lci_tcp_reset_sent = FALSE;
	    lci->lci_pvc_status = X25_PVC_TCP_TRYING;

	    if (pt->source_idb && pt->source_idb->ip_address &&
		interface_up(pt->source_idb))
		lhost = pt->source_idb->ip_address;
	    else
		lhost = 0;

	    /*
	     *  open a connection and send a PVC setup packet
	     */
	    if ((pak = (paktype *)getx25(lciidb, lci,
			X25_PVCSETUP_BYTES + strlen(lciidb->hw_namestring) +
			pt->pvc_remote_if_len, FALSE)) &&
		(lci->lci_pvc_swdata = 
		 x25_switch_tcp_open(pt->ipaddr[0], lhost, lci, &diag))) {
		x25_switch_tcp_setparms(lci->lci_pvc_swdata,
					pt->tcp_keepalive_period,
					pt->tcp_keepalive_tries);
		/*
		 *  swap lci_swdata and lci_pvc_swdata so _tcp_send will work;
		 *  don't allow any incoming packets because a non-NULL
		 *  lci_swdata indicates that the VC is connected
		 */
		level = raise_interrupt_level(NETS_DISABLE);
		otherpvc = (int)lci->lci_swdata;
		lci->lci_swdata = lci->lci_pvc_swdata;
		lci->lci_pvc_swdata = (tcp_encaps *)otherpvc;

		lci->lci_pvc_status = X25_PVC_TCP_CONNECTED;
		p = (packettype *)pak->datagramstart;
		p->x25_q = 0;
		p->x25_d = 0;
		p->x25_sn = (lciidb->x25_modulo == X25_MOD8) ?
			X25_MODULO_8 : X25_MODULO_128;
		p->x25_lcg = 0;
		p->x25_lcn = 0;
		p->x25_pti = X25_PVCSETUP;
		pvc = (pvc_setup_data *)((uchar *)p + X25HEADERBYTES);
		pvc->version = X25_PVCSETUP_VER;
		pvc->msg_code = X25_PVC_DOWN;
		pvc->init_if_name_len = strlen(lciidb->hw_namestring);
		pvc->init_vc_hi = LCG(lci->lci_lci);
		pvc->init_vc_lo = LCN(lci->lci_lci);
		pvc->resp_if_name_len = pt->pvc_remote_if_len;
		pvc->resp_vc_hi = LCG((int)pt->pvc_dest_vc);
		pvc->resp_vc_lo = LCN((int)pt->pvc_dest_vc);
		pvc->win = lci->lci_win;
		pvc->wout = lci->lci_wout;
		pvc->pin = lci->lci_pin;
		pvc->pout = lci->lci_pout;
		sprintf(pvc->names, "%s", lciidb->hw_namestring);
		sprintf(&pvc->names[pvc->init_if_name_len], "%*s",
			pt->pvc_remote_if_len, pt->pvc_remote_if);

		if (x25_debug || x25event_debug)
		    x25_prpack(NULL, pak, lci, 'O');
		x25_switch_tcp_send(lci, pak);

		reg_invoke_ip_tcpdriver_update(lci->lci_swdata, NULL, 0, lci);

		/*
		 *  move the connection id back into lci_pvc_swdata since the
		 *  connection isn't up yet
		 */
		lci->lci_swdata = lci->lci_pvc_swdata;
		lci->lci_pvc_swdata = (tcp_encaps *)otherpvc;
		reset_interrupt_level(level);
	    } else {
		/*
		 *  if we couldn't get a packet to send, we'll put the PVC
		 *  in state X25_PVC_DOWN to retry in a few seconds
		 */
		if (pak)
		    datagram_done(pak);

		if (diag == X25_DIAG_INCOMING_CALL_BARRED)
		    lci->lci_pvc_status = X25_PVC_TCP_REFUSED;
		else if (diag == X25_DIAG_TIMER_EXPIRED)
		    lci->lci_pvc_status = X25_PVC_TCP_TOUT;
		else if (diag == X25_DIAG_TEMP_ROUTING_ERROR)
		    lci->lci_pvc_status = X25_PVC_TCP_ROUTE_ERR;
		else
		    lci->lci_pvc_status = X25_PVC_DOWN;

		if (!TIMER_RUNNING(lci->lci_timer)) {
		    if (x25_set_pvc_con_time(lci))
			x25_addtimer(lci);
		}
	    }
	}

	if (lci->lci_pvc_status != X25_PVC_DOWN &&
	    lci->lci_pvc_status != X25_PVC_TCP_CONNECTED)
	    errmsg(&msgsym(PVCBAD, X25), lciidb->hw_namestring,
		   lci->lci_lci, x25_pvc_err_str[lci->lci_pvc_status]);
    }
}

/*
 * x25_forward_call_connected
 * Forward a Call Connect packet
 */
void x25_forward_call_connected (
    lcitype *lci,
    paktype *pak)
{
    lcitype *olci;
    packettype *p = (packettype *)pak->datagramstart;
    uchar from_called_w, from_calling_w, from_called_p, from_calling_p;

    /*
     *	handle remote routing over a TCP connection
     */
    if (lci->lci_swtype == X25_ROUTE_IPADDR && pak->linktype == LINK_X25) {
	/*
	 *  indicate any required flow values--
	 *  this interface plays the "calling DTE" role
	 */
	if (lci->lci_force_winsize) {
	    from_called_w = lci->lci_win;
	    from_calling_w = lci->lci_wout;
	} else {
	    from_called_w = 0;
	    from_calling_w = 0;
	}
	if (lci->lci_force_pksize) {
	    from_called_p = lci->lci_pin;
	    from_calling_p = lci->lci_pout;
	} else {
	    from_called_p = 0;
	    from_calling_p = 0;
	}

	if ((lci->lci_force_winsize || lci->lci_force_pksize) &&
	    ((pak = x25_insert_flow_facil(pak,
			from_called_w, from_calling_w,
			from_called_p, from_calling_p)) == NULL)) {
	    x25_sendcause(lci->lci_idb->hwptr, lci, X25_CLEAR,
			X25_CLEAR_NETWORK_CONGESTION, X25_DIAG_NO_INFO, NULL);
	    return;
	}

	if (x25_debug || x25event_debug)
	    x25_prpack(NULL, pak, lci, 'O');

	if (lci->lci_pbp_compress && x25_switching_pbp_compress) {  /* PBP */
	    /*tell remote tcp end to set local ack*/
	    p->x25_q = 1;
	}

	x25_switch_tcp_send(lci, pak);
	return;
    }

    olci = lci;

    /*
     *	copy facility values if we're performing local routing to a
     *	second X.25 interface
     */
    if (pak->linktype == LINK_X25) {
	olci = lci->lci_swdata;

	/*
	 *  the incoming interface VC has the role of "calling DTE" and
	 *  the outgoing interface VC has the role of "called DTE"--
	 *  swap the flow control values to reflect this
	 */
	olci->lci_wout = lci->lci_win;
	olci->lci_win = lci->lci_wout;
	olci->lci_pout = lci->lci_pin;
	olci->lci_pin = lci->lci_pout;
	olci->lci_force_winsize |= lci->lci_force_winsize;
	olci->lci_force_pksize |= lci->lci_force_pksize;
	olci->lci_tput_out = lci->lci_tput_in;
	olci->lci_tput_in = lci->lci_tput_out;

	olci->lci_dbit = lci->lci_dbit;
	olci->lci_fast = lci->lci_fast;
	olci->lci_confirm_addr = lci->lci_confirm_addr;
	olci->lci_reverse = lci->lci_reverse;
	olci->lci_cug = lci->lci_cug;
	olci->lci_lam = lci->lci_lam;
	olci->lci_tdelay = lci->lci_tdelay;
	olci->lci_prec = lci->lci_prec;
	bcopy(&lci->lci_src_nsap, &olci->lci_src_nsap, sizeof(addrtype));
	olci->lci_src_nsap_digits = lci->lci_src_nsap_digits;
	bcopy(&lci->lci_dst_nsap, &olci->lci_dst_nsap, sizeof(addrtype));
	olci->lci_dst_nsap_digits = lci->lci_dst_nsap_digits;
    } else {
	olci->lci_dbit |= p->x25_d;
    }

    if ((pak->datagramsize > X25HEADERBYTES) && 
	(pak->datagramstart[X25HEADERBYTES] != 0)) { 
	olci->lci_confirm_addr = TRUE;
    }

    p_unqueue(&x25tQ, olci);
    TIMER_STOP(olci->lci_timer);
    olci->lci_state = X25_P4;

    sendpacket(olci->lci_idb->hwptr, olci, X25_CALL_CONNECTED, p->x25_d, pak);
    datagram_done(pak);
}

/*
 * x25_flush_call
 * Remove other circuit when its partner goes away.
 */
void x25_flush_call (
    lcitype *lci,
    uchar cause,
    uchar diag,
    boolean time_pvc)
{
    lcitype *olci;

    lci->lci_pvc_status = X25_PVC_DOWN;

    if (lci->lci_swdata) {
	if (lci->lci_swtype == X25_ROUTE_IPADDR) {
	    x25_switch_tcp_close(lci, NULL, NULL);
	} else {
	    olci = lci->lci_swdata;
	    if (olci->lci_swdata != NULL) {
		olci->lci_swdata = NULL;
		if ((olci->lci_idb->hwptr != lci->lci_idb->hwptr) &&
		    validmem(olci->lci_idb)) {
		    if (IS_LCI_PVC(olci->lci_idb->hwptr, olci->lci_lci)) {
			olci->lci_pvc_status = X25_PVC_DOWN;
			x25_sendcause(olci->lci_idb->hwptr, olci, X25_RESET,
				      cause, diag, NULL);
		    } else
			x25_sendcause(olci->lci_idb->hwptr, olci, X25_CLEAR,
				      cause, diag, NULL);
		}
	    }
	}
	lci->lci_swdata = NULL;
    }

    /*
     *  if applicable, start a PVC's connect timer
     */
    if (time_pvc &&
	((lci->lci_state == X25_P4) || (lci->lci_state == X25_D1)) &&
	IS_LCI_PVC(lci->lci_idb->hwptr, lci->lci_lci)) {
	if (TIMER_RUNNING(lci->lci_timer))
	    p_unqueue(&x25tQ, lci);
	/*
	 *  corner case--normally locally switched PVCs don't have a
	 *  connect timer, but we may be out of packets to connect it now
	 */
	if (!x25_set_pvc_con_time(lci))
	    TIMER_START(lci->lci_timer, ONESEC);
	x25_addtimer(lci);
    }
}

/*
 * x25_forward_clear
 * Handle forwarding and closing of connections.
 */
void x25_forward_clear (
    lcitype *lci,
    paktype *pak,
    uchar cause,
    uchar diag)
{
    uchar *c = ((uchar *)pak->datagramstart) + X25HEADERBYTES;
    lcitype *olci;

    /*
     *  forward a local procedure error as a remote procedure error
     */
    if (cause == X25_CLEAR_LOCAL_PROC_ERR)
	cause = X25_CLEAR_REMOTE_PROC_ERR;

    *c = cause;

    if (lci->lci_swtype == X25_ROUTE_IPADDR && pak->linktype == LINK_X25) {
	if (x25_debug || x25event_debug)
	    x25_prpack(NULL, pak, lci, 'O');
	x25_switch_tcp_send(lci, pak);
	x25_switch_tcp_close(lci, NULL, NULL);
	return;
    }

    if (pak->linktype == LINK_X25) {
	olci = lci->lci_swdata;
	lci->lci_swdata = NULL;
    } else {
	olci = lci;
	x25_switch_tcp_close(olci, NULL, NULL);
    }

    olci->lci_swdata = NULL;
    x25_sendcause(olci->lci_idb->hwptr, olci, X25_CLEAR, cause, diag, pak);
    datagram_done(pak);
}

/*
 *  x25_forward_reset
 *  forward a Reset packet
 *
 *  The VC RESET procedure is rendered simple by our choice to implement
 *  it as a 'ripple' RESET of all the LCs that make up the connection with
 *  local RESET CONFIRMs issued on receipt of a RESET.  This works as long
 *  as the RESET of both ends of an X.25 connection guarantee that any
 *  data between the two ends gets freed in the process.  This is done
 *  for local connections by having interrupts disabled and forwarding an
 *  incoming RESET or, for an internally generated RESET procedure, processing
 *  both outgoing RESETs at the same time.  The procedure for a remote
 *  connection, however, requires a RESET CONFIRM handshake over the TCP
 *  connection in order to guarantee the destruction of any data/interrupt/
 *  flow control packets in the TCP pipe.
 */
void x25_forward_reset (
    lcitype *lci,
    paktype *pak,
    uchar cause,
    uchar diag)
{
    packettype *p;
    uchar *c = ((uchar *)pak->datagramstart) + X25HEADERBYTES;
    lcitype *olci;

    /*
     *  forward a local procedure error as a remote procedure error
     */
    if (cause == X25_RESET_LOCAL_PROC_ERR)
	cause = X25_RESET_REMOTE_PROC_ERR;

    *c = cause;

    /*
     *	handle a RESET going out a TCP connection
     */
    if (lci->lci_swtype == X25_ROUTE_IPADDR && pak->linktype == LINK_X25) {
	if (lci->lci_tcp_reset_sent) {
	    datagram_done(pak);
	} else {
	    if (x25_debug || x25event_debug)
		x25_prpack(NULL, pak, lci, 'O');
	    x25_switch_tcp_send(lci, pak);

	    /*
	     *  flag the TCP connection to discard data/flow control/interrupt
	     *  packets until a RESET CONFIRM handshake *only* if the router
	     *  on the other end of this TCP connection does end-to-end reset
	     *  handling
	     */
	    lci->lci_tcp_reset_sent = lci->lci_confirm_tcp_reset;
	}
	return;
    }

    /*
     *  determine the outgoing VC for
     *  a locally routed VC or a packet that came in from a TCP connection
     */
    if (pak->linktype == LINK_X25) {
	olci = lci->lci_swdata;
	datagram_done(pak);
    } else {
	if (lci->lci_confirm_tcp_reset) {
	    /*
	     *  we do end-to-end RESET handling over this TCP connection;
	     *  acknowledge the receipt of the RESET with a RESET CONFIRM--
	     *  RESET collisions over the TCP pipe are handled
	     *  by x25_switch_packet
	     */
	    p = (packettype *)pak->datagramstart;
	    p->x25_q = 0;
	    p->x25_d = 0;
	    p->x25_sn = (lci->lci_idb->hwptr->x25_modulo == X25_MOD8) ?
			X25_MODULO_8 : X25_MODULO_128;
	    p->x25_pti = X25_RESET_CONFIRMATION;
	    p->x25_lcg = LCG(lci->lci_lci);
	    p->x25_lcn = LCN(lci->lci_lci);
	    pak->datagramsize = X25HEADERBYTES;

	    if (x25_debug || x25event_debug)
		x25_prpack(NULL, pak, lci, 'O');
	    x25_switch_tcp_send(lci, pak);
	} else {
	    datagram_done(pak);
	}

	olci = lci;

	/*
	 *  plunk 'lci->lci_swdata' into 'lci' for restoring the loop after
	 *  forwarding the RESET
	 */
	lci = olci->lci_swdata;
    }

    x25_call_reset(olci, FALSE);

    if (olci->lci_state != X25_D2 && olci->lci_state != X25_D3) {
	/*
	 *  forward the RESET packet
	 */
	olci->lci_swdata = NULL;		/* block the loop */
	x25_sendcause(olci->lci_idb->hwptr, olci, X25_RESET, cause, diag, NULL);
	olci->lci_swdata = lci;			/* restore the loop */
    }
}

/*
 * x25_forward_interrupt
 * Forward an Interrupt packet in one of three cases:
 *	to a remote VC over a TCP connection,
 *	to a local VC from a TCP connection, or
 *	to a local VC from a local VC
 */
void x25_forward_interrupt (lcitype *lci, paktype *pak)
{
    lcitype *olci;

    if (lci->lci_swtype == X25_ROUTE_IPADDR && pak->linktype == LINK_X25) {
	lci->lci_allow_int = FALSE;
	if (lci->lci_tcp_reset_sent) {
	    if (!pak_requeue(&lci->lci_resetQ, pak)) {
		datagram_done(pak);
		if (IS_LCI_PVC(lci->lci_idb->hwptr, lci->lci_lci)) {
		    x25_flush_call(lci,
			X25_RESET_NETWORK_OUT_OF_ORDER, X25_DIAG_NO_INFO, TRUE);
		    x25_sendcause(lci->lci_idb->hwptr, lci, X25_RESET,
			X25_RESET_NETWORK_OUT_OF_ORDER, X25_DIAG_NO_INFO, NULL);
		} else {
		    x25_sendcause(lci->lci_idb->hwptr, lci, X25_CLEAR,
			X25_CLEAR_NETWORK_CONGESTION, X25_DIAG_NO_INFO, NULL);
		}
	    }
	} else {
	    if (x25_debug || x25event_debug)
		x25_prpack(NULL, pak, lci, 'O');
	    x25_switch_tcp_send(lci, pak);
	}
	return;
    }

    if (pak->linktype == LINK_X25) {
	lci->lci_allow_int = FALSE;
	olci = lci->lci_swdata;
    } else {
	olci = lci;
    }
    pak->if_output = olci->lci_idb;

    if (olci->lci_state == X25_D2 || olci->lci_state == X25_D3) {
	if (!pak_requeue(&olci->lci_resetQ, pak)) {
	    /*
	     *  no elements available--give up on the VC
	     */
	    datagram_done(pak);
	    if (IS_LCI_PVC(olci->lci_idb->hwptr, olci->lci_lci)) {
		x25_flush_call(olci,
			X25_RESET_NETWORK_OUT_OF_ORDER, X25_DIAG_NO_INFO, TRUE);
		x25_sendcause(olci->lci_idb->hwptr, olci, X25_RESET,
			X25_RESET_NETWORK_OUT_OF_ORDER, X25_DIAG_NO_INFO, NULL);
	    } else {
		x25_sendcause(olci->lci_idb->hwptr, olci, X25_CLEAR,
			X25_CLEAR_NETWORK_CONGESTION, X25_DIAG_NO_INFO, NULL);
	    }
	}
    } else {
	x25_sendint(olci, pak, NULL, 0);
    }
}

/*
 * x25_forward_interrupt_confirm
 * Forward X.25 interrupt confirmation in one of three cases:
 *	to a remote VC over a TCP connection,
 *	to a local VC from a TCP connection, or
 *	to a local VC from a local VC
 */
void x25_forward_interrupt_confirm (lcitype *lci, paktype *pak)
{
    lcitype *olci;

    if (lci->lci_swtype == X25_ROUTE_IPADDR && pak->linktype == LINK_X25) {
	lci->lci_allow_int_cnf = FALSE;
	if (x25_debug || x25event_debug)
	    x25_prpack(NULL, pak, lci, 'O');
	x25_switch_tcp_send(lci, pak);
	return;
    }

    if (pak->linktype == LINK_X25) {
	lci->lci_allow_int_cnf = FALSE;
	olci = lci->lci_swdata;
    } else {
	olci = lci;
    }

    /*
     *  we can't get an interrupt confirm if the outgoing interface is
     *  in state D2/D3, so we won't need to queue the packet
     */
    sendpacket(olci->lci_idb->hwptr, olci, X25_INTERRUPT_CONFIRMATION,
	       0, NULL);
    olci->lci_allow_int = TRUE;
    datagram_done(pak);
}

/*
 * x25_forward_data
 * Forward X.25 Data packets
 */
void x25_forward_data (
    lcitype *lci,
    paktype *pak,
    int sn,
    uint len,
    int pr)
{
    lcitype *olci;
    packettype *p;

    if (lci->lci_swtype == X25_ROUTE_IPADDR && pak->linktype == LINK_X25) {
	/*
	 *  the X.25 preamble has been removed, but must be sent,
	 *  so re-adjust the packet
	 */
	pak->datagramsize += len;
	pak->datagramstart -= len;

	if (lci->lci_tcp_reset_sent) {
	    /*
	     *  we're waiting for a RESET CONFIRM from the far end--
	     *  hold the packet
	     */
	    if (!pak_enqueue(&lci->lci_resetQ, pak)) {
		datagram_done(pak);
		if (IS_LCI_PVC(lci->lci_idb->hwptr, lci->lci_lci)) {
		    x25_flush_call(lci,
			X25_RESET_NETWORK_OUT_OF_ORDER, X25_DIAG_NO_INFO, TRUE);
		    x25_sendcause(lci->lci_idb->hwptr, lci, X25_RESET,
			X25_RESET_NETWORK_OUT_OF_ORDER, X25_DIAG_NO_INFO, NULL);
		} else {
		    x25_sendcause(lci->lci_idb->hwptr, lci, X25_CLEAR,
			X25_CLEAR_NETWORK_CONGESTION, X25_DIAG_NO_INFO, NULL);
		}
	    }
	} else {
	    if (lci->lci_pbp_local_ack) {	/* PBP near inner function */
		if (lci->lci_pbp_ackpak != NULL) {
		    /* sending DATA don't need RR. removed queued local ack's*/
		    p_unqueue(&x25LocalAckQ, lci->lci_pbp_ackpak);
		    datagram_done(lci->lci_pbp_ackpak);
		    lci->lci_pbp_ackpak = NULL;
		}
		p = (packettype *)pak->datagramstart;
		x25_pbp_stuff_pr_ps(lci, p);
	    }

	    if (x25_debug)
		x25_prpack(NULL, pak, lci, 'O');
	    x25_switch_tcp_send(lci, pak);
	}
	return;
    }

    if (pak->linktype == LINK_X25 || pak->linktype == LINK_CMNS) /* from X.25, CMNS or TCP? */
	olci = lci->lci_swdata;
    else
	olci = lci;

    pak->if_output = olci->lci_idb;

    /*
     *  hold off sending the packet if the VC is RESETting
     */
    if (olci->lci_state == X25_D2 || olci->lci_state == X25_D3) {
	/*
	 *  the X.25 preamble has been removed, but is needed for reset
	 *  processing, so re-adjust the packet
	 */
	pak->datagramsize += len;
	pak->datagramstart -= len;

	if (pak_enqueue(&olci->lci_resetQ, pak))
	    pak = NULL;
    } else {
	if (x25_encaps(pak, (long)olci, NULL)) {
	    /*if doing local ack don't update lci_ack*/
	    if (!olci->lci_pbp_local_ack) {
		/*
		 *  adjust the acknowledged packets value according to
		 *  the modulo of the incoming and outgoing interfaces
		 */
		if (olci->lci_idb->hwptr->x25_modulo ==
		    (sn == X25_MODULO_128 ? X25_MOD128 : X25_MOD8))
		    olci->lci_ack = pr;
		else if (sn == X25_MODULO_128) {
		    /*
		     *  we're switching a modulo 128 packet out as modulo 8
		     */
		    olci->lci_ack = pr & (X25_MOD8 - 1);
		} else {
		    /*
		     *  we're switching a modulo 8 packet out as modulo 128--
		     *  note that the maximum window size is 7
		     */
		    if (pr < (olci->lci_ack & (X25_MOD8 - 1)))
			olci->lci_ack =
			   ((olci->lci_ack & ~(X25_MOD8 - 1)) + X25_MOD8) | pr;
		    else
			olci->lci_ack = (olci->lci_ack & ~(X25_MOD8 - 1)) | pr;
		    olci->lci_ack &= X25_MOD128 - 1;
		}
	    } else {/*both near & far inner node functions*/
		p = (packettype *) (pak->datagramstart +
					LAPB_OVERHEAD(olci->lci_idb->hwptr));
		if (pak->linktype == LINK_X25 || pak->linktype == LINK_CMNS) {
		    x25_pbp_stuff_pr_ps(lci, p); /*near inner lci*/
		}
		if (!x25_pbp_inner_node_receive(olci, p)) /*far inner olci*/
		    goto FAILED;
	    }
	    x25_output(olci->lci_idb->hwptr, pak, TAIL);
	    pak = NULL;
	}
    }

FAILED:
    if (pak) {
	/*
	 *  we have an encapsulation failure or can't queue it--
	 *  give up on the VC
	 */
	datagram_done(pak);
	if (IS_LCI_PVC(lci->lci_idb->hwptr, lci->lci_lci)) {
	    x25_flush_call(olci,
			X25_RESET_NETWORK_OUT_OF_ORDER, X25_DIAG_NO_INFO, TRUE);
	    x25_sendcause(olci->lci_idb->hwptr, olci, X25_RESET,
			X25_RESET_NETWORK_OUT_OF_ORDER, X25_DIAG_NO_INFO, NULL);
	} else {
	    x25_sendcause(olci->lci_idb->hwptr, olci, X25_CLEAR,
			X25_CLEAR_NETWORK_CONGESTION, X25_DIAG_NO_INFO, NULL);
	}
    }
}

/*
 *  x25_forward_ack
 *  Handle updating of windows for switched connections
 */
void x25_forward_ack (
    lcitype *lci,
    paktype *pak,
    int sn,
    int pr)
{
    lcitype *olci;
    packettype *p;

    if (lci->lci_swtype == X25_ROUTE_IPADDR && pak->linktype == LINK_X25) {
	if (lci->lci_tcp_reset_sent) {
	    if (!pak_enqueue(&lci->lci_resetQ, pak)) {
		datagram_done(pak);
		if (IS_LCI_PVC(lci->lci_idb->hwptr, lci->lci_lci)) {
		    x25_flush_call(lci,
			X25_RESET_NETWORK_OUT_OF_ORDER, X25_DIAG_NO_INFO, TRUE);
		    x25_sendcause(lci->lci_idb->hwptr, lci, X25_RESET,
			X25_RESET_NETWORK_OUT_OF_ORDER, X25_DIAG_NO_INFO, NULL);
		} else {
		    x25_sendcause(lci->lci_idb->hwptr, lci, X25_CLEAR,
			X25_CLEAR_NETWORK_CONGESTION, X25_DIAG_NO_INFO, NULL);
		}
	    }
	} else {
	    if (x25_debug)
		x25_prpack(NULL, pak, lci, 'O');
	    x25_switch_tcp_send(lci, pak);
	}
	return;
    }

    if (pak->linktype == LINK_X25)
	olci = lci->lci_swdata;
    else
	olci = lci;

    if (olci->lci_state == X25_D2 || olci->lci_state == X25_D3) {
	if (!pak_enqueue(&olci->lci_resetQ, pak)) {
	    datagram_done(pak);
	    if (IS_LCI_PVC(lci->lci_idb->hwptr, lci->lci_lci)) {
		x25_flush_call(olci,
			X25_RESET_NETWORK_OUT_OF_ORDER, X25_DIAG_NO_INFO, TRUE);
		x25_sendcause(olci->lci_idb->hwptr, olci, X25_RESET,
			X25_RESET_NETWORK_OUT_OF_ORDER, X25_DIAG_NO_INFO, NULL);
	    } else {
		x25_sendcause(olci->lci_idb->hwptr, olci, X25_CLEAR,
			X25_CLEAR_NETWORK_CONGESTION, X25_DIAG_NO_INFO, NULL);
	    }
	}
    } else {
	/*
	 *  adjust the acknowledged packets value according to
	 *  the modulo of the incoming and outgoing interfaces
	 */
	/*if doing local ack don't update lci_ack*/
	if (!olci->lci_pbp_local_ack) {
	    if (olci->lci_idb->hwptr->x25_modulo ==
		(sn == X25_MODULO_128 ? X25_MOD128 : X25_MOD8))
		    olci->lci_ack = pr;
	    else if (sn == X25_MODULO_128) {
		/*
		 *  we're switching a modulo 128 packet out as modulo 8
		 */
		    olci->lci_ack = pr & (X25_MOD8 - 1);
		} else {
		    /*
		     *  we're switching a modulo 8 packet out as modulo 128--
		     *  note that the maximum window size is 7
		     */
		    if (pr < (olci->lci_ack & (X25_MOD8 - 1)))
			olci->lci_ack = ((olci->lci_ack & ~(X25_MOD8 - 1)) + 
					X25_MOD8) | pr;
		    else
			olci->lci_ack = (olci->lci_ack & ~(X25_MOD8 - 1)) | pr;
		    olci->lci_ack &= X25_MOD128 - 1;
		}
	} else {/*near inner node functions*/
	    p = (packettype *)pak->datagramstart;
	    if (!x25_pbp_inner_node_receive(olci, p)) {
		x25_pbp_flush_call(olci);
		datagram_done(pak);
		return;
	    }
	    if (!x25_pbp_unacked(olci->lci_idb->hwptr, olci)) {
		datagram_done(pak);
		return;
	    }
	}
	x25_sendflow(olci->lci_idb->hwptr, olci, X25_RR);
	datagram_done(pak);
    }
}

/*
 *  cons_forward_call
 *  Forward a CMNS Call; destination may be serial or llc2
 */
uint cons_forward_call (
    lcitype *lci,
    llctype *llc2,
    uchar *cud,
    uint cud_len,
    boolean fwd)
{
    x25map *map;
    lcitype *lci2;

    map = lci->lci_map;

    if (map == NULL)
	return(X25_FORWARD_ERR);

    /*
     *  if the Call can't be forwarded immediately, store the
     *  incoming lci in the map while awaiting completion of
     *  the LLC2 establishment and service RESTART.  If we've
     *  already stored the maximum, abandon the call.
     *  Allocate memory to hold the call user data in the incoming
     *  lci; if that fails, abandon the call.
     */
    if (!fwd) {
	if (map->map_nvc >= map->map_maxnvc)
	    return(X25_FORWARD_ERR);

	if ((lci->lci_cudptr = malloc(cud_len + 1)) == NULL)
	    return(X25_FORWARD_ERR);

	lci->lci_cudlen = cud_len;
	bcopy(cud, lci->lci_cudptr, cud_len);
	map->map_lci[map->map_nvc++] = lci;
	return(X25_FORWARD_CMNS_IN_PROGRESS);
    }

    /*
     *  create the outgoing VC
     */
    lci2 = x25_lci_enter(map->map_idb, X25_NO_VC_ASSIGNED, llc2);

    if (lci2 == NULL)
	return(X25_FORWARD_ERR);

    lci2->lci_linktype = LINK_X25;

    /*
     *  set the destination X.121 address if specified for a serial interface,
     *  otherwise it's left empty; the source address is always left empty
     */
    if (is_x25(map->map_idb->hwptr) &&
	(map->map_addr_count > 1)) {
	bcopy(&map->map_address[X25_MAP_SECONDARY_ADDR_INX].addr,
	      &lci2->lci_dst_addr, sizeof(addrtype));
    }

    lci2->lci_wout = lci->lci_win;
    lci2->lci_win = lci->lci_wout;
    lci2->lci_pout = lci->lci_pin;
    lci2->lci_pin = lci->lci_pout;
    lci2->lci_force_winsize = lci->lci_force_winsize;
    lci2->lci_force_pksize = lci->lci_force_pksize;
    lci2->lci_tput_out = lci->lci_tput_in;
    lci2->lci_tput_in = lci->lci_tput_out;

    lci2->lci_fast = lci->lci_fast;
    lci2->lci_confirm_addr = lci->lci_confirm_addr;
    lci2->lci_reverse = lci->lci_reverse;
    lci2->lci_cug = lci->lci_cug;
    lci2->lci_lam = lci->lci_lam;
    lci2->lci_tdelay = lci->lci_tdelay;
    bcopy(&lci->lci_src_nsap, &lci2->lci_src_nsap, sizeof(addrtype));
    lci2->lci_src_nsap_digits = lci->lci_src_nsap_digits;
    bcopy(&lci->lci_dst_nsap, &lci2->lci_dst_nsap, sizeof(addrtype));
    lci2->lci_dst_nsap_digits = lci->lci_dst_nsap_digits;
    lci2->lci_rtp = lci->lci_rtp;
    lci2->lci_llc2 = llc2;

    lci->lci_swtype = X25_ROUTE_IF;
    lci->lci_swdata = lci2;
    lci2->lci_swtype = X25_ROUTE_IF;
    lci2->lci_swdata = lci;

    /*
     *  LLC2 connection exits--forward the Call
     */
    lci->lci_idb->hwptr->x25_callsfrwd++;
    if (x25_sendcall(lci2, cud, cud_len)) {
	return(X25_FORWARD_OK);
    }

    /*
     *  Error--since lci2 was added to x25tQ in x25_sendcall, make sure
     *  it is unqueued first before deleting.
     */
    if (checkqueue(&x25tQ, lci2)) {
	p_unqueue(&x25tQ, lci2);
	TIMER_STOP(lci2->lci_timer);
    }
    /*
     * break the association between the two lcis, since we're about
     * to delete one of them.
     */
    lci->lci_swtype = -1;
    lci->lci_swdata = NULL;
    lci2->lci_swtype = -1;
    lci2->lci_swdata = NULL;

    x25_lci_delete(lci2);

    return(X25_FORWARD_ERR);
}

/*
 *  x25_cons_forward_call
 *  Forward a CONS Call to the appropraite interface
 */
uint x25_cons_forward_call (
    lcitype *lci,
    paktype *pak,
    uchar *cud,
    uint cud_len,
    uchar *diag)
{
    uint rc = X25_FORWARD_ERR;

    if (lci->lci_linktype == LINK_CMNS) {
	x25map *map;

	map = lci->lci_map;

	if (map == NULL) {
	    if (pak)
	        datagram_done(pak);
	    return(X25_FORWARD_ERR);
        }

	/*
	 *  set up the fake routeentry
	 */
	consrtp.type = X25_ROUTE_CMNS;
	consrtp.idb = map->map_idb;
	lci->lci_rtp = &consrtp;

	/*
	 *  determine the destination interface
	 */
	if (is_x25(map->map_idb->hwptr)) {
	    rc = cons_forward_call(lci, NULL, cud, cud_len, TRUE);
	} else if (ISCMNS_IEEE(map->map_idb->hwptr)) {
	    uchar *mac;
	    llctype *llc2_cb;
	    considbtype *considb;
	    uchar wsize;
	    hwaddrtype macAddr, srcAddr;

	    mac = map->map_address[X25_MAP_SECONDARY_ADDR_INX].
			addr.mac_kludge_addr;

	    snpa_init(&macAddr, STATION_IEEE48, TARGET_UNICAST, mac );
	    snpa_clear(&srcAddr);
	    
	    llc2_cb = llc2_find_cb(mac, SAP_CMNS, SAP_CMNS, map->map_idb);

	    if (llc2_cb == NULL) {
		/*
		 *  there is no existing LLC2 connection--
		 *  try to create one using a non-blocking open
		 */
                wsize = llc_sb_get_wind(map->map_idb);
		llc2_cb =
		    llc2_open(SAP_CMNS, SAP_CMNS, &macAddr, &srcAddr,
			      map->map_idb, LINK_LLC2,
			      wsize, x25_llc2_clsproc, FALSE, FALSE);

		/*
		 *  process Call to be forwarded once the link is up
		 */
		if (llc2_cb)
		    rc = cons_forward_call(lci, llc2_cb, cud, cud_len, FALSE);
		else
		    rc = X25_FORWARD_ERR;
	    } else {
		/*
		 *  an LLC2 connection exists; process the Call, but don't
		 *  forward it unless the service is up
		 */
		considb = llc2_cb->considb;

		rc = cons_forward_call(lci, llc2_cb, cud, cud_len,
				       (considb && considb->x25_upstate));
	    }
	} else {
	    rc = X25_FORWARD_ERR;
	}

	if (pak)
	    datagram_done(pak);
    } else {
	/*
	 *  this Call is not going out via CONS
	 */
	rc = x25_forward_call(lci, pak, cud, cud_len, diag);
    }

    return(rc); /* result */
}
