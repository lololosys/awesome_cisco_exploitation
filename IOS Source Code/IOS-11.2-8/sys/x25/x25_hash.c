/* $Id: x25_hash.c,v 3.3.10.4 1996/08/28 13:23:34 thille Exp $
 * $Source: /release/112/cvs/Xsys/x25/x25_hash.c,v $
 *------------------------------------------------------------------
 * x25_hash.c -- X25 Logical Channel Indentifier Hashing support
 *
 * February 1987, Greg Satz
 *
 * Copyright (c) 1987-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x25_hash.c,v $
 * Revision 3.3.10.4  1996/08/28  13:23:34  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.10.3  1996/06/28  23:32:45  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3.10.2  1996/06/01  07:19:50  tylin
 * CSCdi53715:  CDP is not working over Fr-Rel, X25 and SMDS interfaces
 *    Add new CUD (0xDA) for CDP
 * Branch: California_branch
 *
 * Revision 3.3.10.1  1996/03/18  22:50:20  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.4  1996/03/16  07:57:41  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.26.3  1996/03/13  02:13:34  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.2  1996/03/07  11:19:41  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:56:11  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/23  23:21:36  carvalho
 * CSCdi41580:  Calling Address Ext. facility modified while X25 switching
 * Save digit count/authority field for called and calling address
 * extension facilities for re-use when call is forwarded.
 *
 * Revision 3.2  1995/11/17  18:07:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:53:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/10/02  22:01:57  carvalho
 * CSCdi39783:  CMNS call to ethernet destination fails if LLC2 not open.
 *
 * Revision 2.4  1995/09/05  01:02:36  gglick
 * CSCdi37627:  parser error convert svc to pvc after ltc change
 *
 * Revision 2.3  1995/07/20  07:38:20  hampton
 * Convert X.25 to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37392]
 *
 * Revision 2.2  1995/07/12 19:19:03  gglick
 * CSCdi35754:  Incoming x25 call is accepted as PAD call i.s.o being
 * routed
 *
 * Revision 2.1  1995/06/07  23:22:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "address.h"
#include "config.h"
#include "packet.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "connect.h"
#include "logger.h"
#include "../x25/msg_x25.c"		/* Not a typo, see logger.h */
#include "config.h"
#include "../ui/debug.h"
#include "../ui/parse_util.h"
#include "../clns/clns_externs.h"
#include "../if/network.h"	       /* for xnethash */
#include "../ip/ip_registry.h"
#include "x25.h"
#include "x25_address.h"
#include "x25_proto.h"
#include "x25_switch.h"
#include "x25_route.h"
#include "x25_debug.h"
#include "lapb.h"
#include "pad.h"
#include "x25_pbp.h"
#include "../llc2/llc2.h"


#define X25_LCI_HASH(lci)	(lci & (X25_LCI_HASH_SIZE - 1))
lcitype *x25_lci_hash_table[X25_LCI_HASH_SIZE];


/*
 * Initialize LCI hash table
 */

void x25_lci_hash_init (void)
{
    register int i;

    for (i = 0; i < X25_LCI_HASH_SIZE; i++)
	x25_lci_hash_table[i] = (lcitype *)NULL;
}

/*
 * Initialize LCI hash table after Restart
 */

void x25_lci_hash_restart (
    register hwidbtype *hwidb,
    register llctype *llc2)
{
    register int hash_bucket;
    register lcitype *lci;
    leveltype level;
    uchar pvc_cause;
    uchar svc_cause;

    if (!hwidb->x25_upstate) {
	pvc_cause = X25_RESET_NETWORK_OUT_OF_ORDER;
	svc_cause = X25_CLEAR_OUT_OF_ORDER;
    } else if (hwidb->x25_cause == X25_RESTART_LOCAL_PROC_ERR) {
	pvc_cause = X25_RESET_REMOTE_PROC_ERR;
	svc_cause = X25_CLEAR_REMOTE_PROC_ERR;
    } else {
	pvc_cause = X25_RESET_NETWORK_OPERATIONAL;
	svc_cause = X25_CLEAR_OUT_OF_ORDER;
    }

    level = raise_interrupt_level(NETS_DISABLE);
    for (hash_bucket = 0; hash_bucket < X25_LCI_HASH_SIZE; hash_bucket++) {
restart:
	for (lci = x25_lci_hash_table[hash_bucket]; lci;) {
	    if ((lci->lci_idb->hwptr != hwidb) ||
		(lci->lci_llc2 != llc2)) {
		lci = lci->lci_hash;
		continue;
	    }

	    if (IS_LCI_PVC(hwidb, lci->lci_lci) )
		reg_invoke_media_x25_notify_event(lci->lci_linktype, 
		    lci, X25_RESET_IND_EVENT, pvc_cause, 
		    X25_RESET_NETWORK_OUT_OF_ORDER, NULL);
	    else
		reg_invoke_media_x25_notify_event(lci->lci_linktype, 
		    lci, X25_CLEAR_IND_EVENT, svc_cause, X25_DIAG_NO_INFO, NULL);

	    /*
	     *  flush all held buffers
	     */
	    x25_call_reset(lci, TRUE);

	    if (IS_LCI_PVC(hwidb, lci->lci_lci)) {
		lci->lci_state = X25_D1;
		lci->lci_retransmit = 0;
		if (!hwidb->x25_upstate) {
		    /*
		     * break the connection
		     */
		    x25_flush_call(lci, pvc_cause, hwidb->x25_diag, FALSE);
		} else {
		    /*
		     *  handle switched and XOT PVCs
		     */

		    /*
		     *  if a connection exists, reset the other end of the PVC
		     */
		    if ((lci->lci_swdata) &&
			(lci->lci_swtype == X25_ROUTE_IPADDR)) {
			paktype *pak;
			packettype *p;
			uchar *c;

			/*
			 *  Tunnelled PVC
			 *  format a RESET packet to send over TCP
			 */
			if (lci->lci_tcp_reset_sent) {
			    ;   /* EMPTY--we've already RESET the con */
			} else {
			    pak = getx25(hwidb, lci, X25_CAUSE_DATA, FALSE);
			    if (pak) {
				pak->linktype = LINK_X25;
				p = (packettype *)pak->datagramstart;
				p->x25_q = 0;
				p->x25_d = 0;
				p->x25_sn =
					(hwidb->x25_modulo == X25_MOD8) ?
					X25_MODULO_8 : X25_MODULO_128;
				p->x25_lcg = LCG(lci->lci_lci);
				p->x25_lcn = LCN(lci->lci_lci);
				p->x25_pti = X25_RESET;
				c = (uchar *)p + X25HEADERBYTES;
				*c++ = pvc_cause;
				*c = hwidb->x25_diag;
				x25_forward_reset(lci, pak,
						  pvc_cause, hwidb->x25_diag);
			    } else {
				/*
				 *  we can't RESET the connection
				 *  flush it now and re-connect later
				 */
				x25_flush_call(lci,
						X25_RESET_NETWORK_OUT_OF_ORDER,
						hwidb->x25_diag, FALSE);
			    }
			}
		    } else if ((lci->lci_swdata) &&
			       (lci->lci_swtype == X25_ROUTE_IF)) {
			lcitype *other_lci;

			/*
			 *  locally switched PVC
			 */

			other_lci = (lcitype *)lci->lci_swdata;
			if (other_lci) {
			    /*
			     *  reset the other end
			     *  doesn't invoke x25_forward_reset()
			     *  because we don't have (or need) a
			     *  RESET packet
			     */
			    if (other_lci->lci_state == X25_D2 ||
				other_lci->lci_state == X25_D3) {
				/*
				 *  flush any packets on the resetQ
				 */
				x25_call_reset(other_lci, FALSE);
			    } else {
				other_lci->lci_swdata = NULL;
				x25_sendcause(other_lci->lci_idb->hwptr,
						other_lci,
						X25_RESET,
						pvc_cause, hwidb->x25_diag,
						NULL);
				other_lci->lci_swdata = lci;
			    }
			}
		    }

		    /*
		     *  if no connection exists, try to establish one
		     */
		    if (!lci->lci_swdata) {
			if (lci->lci_swtype == X25_ROUTE_IF)
			    x25_pvc_retry(lci);
			else {
			    /*
			     *  clear any error condition on a RESTART
			     */
			    lci->lci_pvc_status = X25_PVC_DOWN;

			    /*
			     *  check if we should restart the connect timer
			     */
			    if (!TIMER_RUNNING(lci->lci_timer)) {
				if (x25_set_pvc_con_time(lci))
				    x25_addtimer(lci);
			    }
			}
		    }
		}

		lci = lci->lci_hash;
		continue;
	    }

	    /*
	     *  SVCs
	     */
	    if (lci->lci_linktype == LINK_PAD)
		pad_restart(lci);
	    if (lci->lci_swdata)
		x25_flush_call(lci, svc_cause, hwidb->x25_diag, FALSE);
	    x25_lci_delete(lci);
	    goto restart;
	}
    }
    reset_interrupt_level(level);
}

/*
 * Lookup LCI in hash table corresponding to llc2.
 * Return pointer to LCI or null.
 * if nconnected is TRUE, only look for not-yet-connected lcis
 */

lcitype *x25_lci_llc2_lookup (
    register hwidbtype *idb,
    register llctype *llc2,
    boolean nconnected)
{
    register int i;
    register lcitype *lci;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    for (i = 0; i < X25_LCI_HASH_SIZE ; i++) {
	for (lci = x25_lci_hash_table[i]; lci; lci = lci->lci_hash) {
	    if ((lci->lci_idb->hwptr == idb) &&
		(lci->lci_llc2 == llc2) &&
		(!nconnected || (lci->lci_map == NULL))) {
		reset_interrupt_level(level);
		return(lci);
	    }
	}
    }

    reset_interrupt_level(level);
    return(NULL);
}

/*
 * Lookup LCI in hash table.
 * Return pointer to LCI or null.
 */

lcitype *x25_lci_lookup (
    register hwidbtype *hwidb,
    register int vc,
    register llctype *llc2)
{
    register lcitype *lci;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    for (lci = x25_lci_hash_table[X25_LCI_HASH(vc)]; lci; lci = lci->lci_hash) {
	if ((lci->lci_lci == vc) &&
	    (lci->lci_idb->hwptr == hwidb) &&
	    (lci->lci_llc2 == llc2))
		break;
    }

    reset_interrupt_level(level);
    return(lci);
}

/*
 * Lookup next LCI in hash table.
 */

lcitype *x25_lci_lookup_next (
    register hwidbtype *idb,
    register int vc,
    register llctype *llc2)
{
    register lcitype *lci;
    leveltype level;
    int last_vc;

    last_vc = (idb->x25_hoc > idb->x25_htc) ? idb->x25_hoc :
	      ((idb->x25_htc > idb->x25_hic) ? idb->x25_htc :
	       (idb->x25_hic ? idb->x25_hic : X25_MAX_LCI));

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     *  search for the next VC
     */
    for (vc++; vc <= last_vc; vc++) {
	/*
	 *  skim through the hash bucket looking for this VC
	 */
	for (lci = x25_lci_hash_table[X25_LCI_HASH(vc)];
	     lci; lci = lci->lci_hash) {
	    if ((lci->lci_lci == vc) &&
		(lci->lci_idb->hwptr == idb) &&
		(lci->lci_llc2 == llc2)) {
		reset_interrupt_level(level);
		return(lci);
	    }
	}
    }

    reset_interrupt_level(level);
    return(NULL);
}

/*
 * Lookup LCI in hash table.
 * Return pointer to LCI or null.
 */

lcitype *x25_lci_lookup_by_x121 (
    register hwidbtype *hwidb,
    register uchar *addr,
    register int addr_length)
{
    register lcitype *lci;
    addrtype *x121;
    leveltype level;
    int i;

    level = raise_interrupt_level(NETS_DISABLE);

    for (i = 0; i < X25_LCI_HASH_SIZE ; i++) {
	for (lci = x25_lci_hash_table[i]; lci; lci = lci->lci_hash) {
	    if (lci->lci_idb->hwptr != hwidb)
		continue;
	    x121 = (lci->lci_outgoing) ? &lci->lci_dst_addr :
		&lci->lci_src_addr;
	    if (bcmp(x121->x121_addr, addr, addr_length) != 0)
		continue;
	    reset_interrupt_level(level);
	    return(lci);
	}
    }

    reset_interrupt_level(level);
    return(lci);
}

boolean x25_lci_get_info (
    lcitype *lci,
    int *lcn,
    hwaddrtype *address,
    ulong *packetsneedack)
{
    hwidbtype *hwidb;
    addrtype *x121;

    if (!validlink(lci))
	return(FALSE);
    if (lcn)
	*lcn = lci->lci_lci;

    if (address) {
	address->type = STATION_X121;
	address->target_type = TARGET_UNICAST;
	memset(address->addr, 0, STATIONLEN_X121);
	x121 = (lci->lci_outgoing) ? &lci->lci_dst_addr : &lci->lci_src_addr;
	address->length = x121->length;
	bcopy(x121->x121_addr, address->addr, address->length);
    }

    if (packetsneedack) {
	hwidb = lci->lci_idb->hwptr;
	*packetsneedack = ((lci->lci_pr + hwidb->x25_modulo) - lci->lci_ack) %
	    hwidb->x25_modulo;
    }
    return(TRUE);
}

boolean x25_lci_get_stats (
    lcitype *lci,
    ulong *bytessent,
    ulong *bytesrcvd,
    ulong *packetssent,
    ulong *packetsrcvd,
    ulong *resetssent,
    ulong *resetsrcvd)
{
    if (!validlink(lci))
	return(FALSE);
    *bytessent   = lci->lci_bytessent;
    *bytesrcvd   = lci->lci_bytesrcvd;
    *packetssent = lci->lci_packetssent;
    *packetsrcvd = lci->lci_packetsrcvd;
    *resetssent  = lci->lci_resetssent;
    *resetsrcvd  = lci->lci_resetsrcvd;
    return(TRUE);
}

/*
 *  Convert an LCI to a held-down LCI.
 *  The VC must be re-located in the hash table so that it doesn't
 *  occupy a legal VC on the interface.
 */

void x25_lci_holddown (
    register lcitype *lci)
{
    leveltype level;
    register int hash;
    register lcitype *target, *before;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     *  find the lci in the hash table
     */
    hash = X25_LCI_HASH(lci->lci_lci);
    target = x25_lci_hash_table[hash];
    before = target;

    while (target && (target != lci)) {
	before = target;
	target = target->lci_hash;
    }

    if (!target) {
	reset_interrupt_level(level);
	errmsg(&msgsym(NOLCI, X25), lci->lci_idb->namestring,
		lci->lci_lci, "LCI");
    } else {
	/*
	 *  unlink it
	 */
	if (target == before)
	    x25_lci_hash_table[hash] = target->lci_hash;
	else
	    before->lci_hash = target->lci_hash;

	/*
	 *  reassign the VC number to an illegal value
	 */
	lci->lci_lci = X25_MAX_LCI + 1;
	hash = X25_LCI_HASH(lci->lci_lci);
	lci->lci_hash = x25_lci_hash_table[hash];
	x25_lci_hash_table[hash] = lci;

	/*
	 *  put the VC in hold-down (a non-standard state)
	 */
	lci->lci_state = X25_X1;
	TIMER_START(lci->lci_timer, lci->lci_idb->hwptr->x25_holdtime * ONEMIN);
	x25_addtimer(lci);

	reset_interrupt_level(level);
    }
}

/*
 * x25_free_lci
 * Return next free LCI number for given interface (and, for interfaces that
 * support LLC2, for a given LLC2 session on the interface)
 */

uint x25_free_lci (
    hwidbtype *hwidb,
    llctype *llc2_cb)
{
    register int	lcn, done;
    considbtype *considb = NULL;

    if (llc2_cb)
	considb = llc2_cb->considb;
    /*
     *	this procedure needs to find an available logical channel
     *	on which to place a call; reference X.25 Annex A.
     *
     *	    A DCE searches, in ascending numerical order, through the
     *	    incoming then the two-way logical channel ranges.
     *
     *	    A DTE searches, in descending numerical order, through the
     *	    outgoing then the two-way logical channel ranges.
     */
    if (ISDCE(hwidb, considb)) {
	/*
	 *	search the incoming logical channel range
	 */
	if (hwidb->x25_lic) {
	    lcn = hwidb->x25_lic;
	    done = hwidb->x25_hic + 1;

	    while (lcn != done) {
		if (!x25_lci_lookup(hwidb, lcn, llc2_cb))
		    return(lcn);
		lcn++;
	    }
	}

	/*
	 *	search the two-way logical channel range
	 */
	if (hwidb->x25_ltc) {
	    lcn = hwidb->x25_ltc;
	    done = hwidb->x25_htc + 1;

	    while (lcn != done) {
		if (!x25_lci_lookup(hwidb, lcn, llc2_cb))
		    return(lcn);
		lcn++;
	    }
	}
    } else {
	/*
	 *	search the outgoing logical channel range
	 */
	if (hwidb->x25_loc) {
	    lcn = hwidb->x25_hoc;
	    done = hwidb->x25_loc - 1;

	    while (lcn != done) {
		if (!x25_lci_lookup(hwidb, lcn, NULL))
		    return(lcn);
		lcn--;
	    }
	}

	/*
	 *	search the two-way logical channel range
	 */
	if (hwidb->x25_ltc) {
	    lcn = hwidb->x25_htc;
	    done = hwidb->x25_ltc - 1;

	    while (lcn != done) {
		if (!x25_lci_lookup(hwidb, lcn, llc2_cb))
		    return(lcn);
		lcn--;
	    }
	}
    }

    /*
     *	nada, nyet, nill
     */
    return(X25_NO_VC_ASSIGNED);
}

/*
 *  Create a new LCI structure with an assigned virtual circuit number (for
 *  an incoming Call or PVC) or without an assigned virtual circuit number
 *  (for an outgoing Call)
 */

lcitype *x25_lci_enter (
    idbtype *idb,
    register int vc,
    llctype *llc2_cb)
{
    register int hash;
    register lcitype *lci;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     *  if needed, get a VC number
     */
    if ((vc == X25_NO_VC_ASSIGNED) &&
	(vc = x25_free_lci(idb->hwptr, llc2_cb)) == X25_NO_VC_ASSIGNED) {
	reset_interrupt_level(level);
	return(NULL);
    }

    /*
     *  create a VC structure
     */
    if ((lci = malloc(sizeof(lcitype))) == NULL) {
	reset_interrupt_level(level);
	return(NULL);
    }

    /*
     *  insert the VC in the hash table
     */
    hash = X25_LCI_HASH(vc);
    lci->lci_hash = x25_lci_hash_table[hash];
    x25_lci_hash_table[hash] = lci;

    /*
     *  initialize the remaining VC fields
     */

    lci->lci_next = NULL;		/* VC timer queue link */
    queue_init(&lci->lci_holdQ, idb->hwptr->x25_holdmax);
    queue_init(&lci->lci_reassmQ, 0);
    queue_init(&lci->lci_resetQ, 0);

    lci->lci_state = (IS_LCI_PVC(idb->hwptr, vc)) ? X25_D1 : X25_P1;
    lci->lci_ps = 0;
    lci->lci_pr = 0;
    lci->lci_ack = 0;
    lci->lci_rpr = 0;
    lci->lci_busy = FALSE;
    lci->lci_allow_int = TRUE;
    lci->lci_allow_int_cnf = FALSE;
    lci->lci_tcp_reset_sent = FALSE;
    lci->lci_confirm_tcp_reset = x25_xot_confirm_svc_reset;
    lci->lci_pvc_con_d2_d3 = FALSE;
    lci->lci_pvc_status = X25_PVC_DOWN;
    lci->lci_pvc_swdata = NULL;
    lci->lci_cud_pid_method = X25_CUD_PID_NONE;
    lci->lci_data_pid_method = X25_DATA_PID_NONE;
    lci->lci_retransmit = 0;
    TIMER_STOP(lci->lci_timer);
    lci->lci_lci = vc;
    lci->lci_data_flags = 0;
    lci->lci_tx_first_frag = FALSE;
    lci->lci_tx_frag_cnt = 0;
    lci->lci_reassmbytes = 0;
    lci->lci_linktype = LINK_ILLEGAL;
    lci->lci_idle = 0;
    GET_TIMESTAMP(lci->lci_starttime);
    TIMER_STOP(lci->lci_lastinput);
    TIMER_STOP(lci->lci_lastoutput);

    lci->lci_src_addr.type = ADDR_ILLEGAL;
    lci->lci_src_addr.length = 0;
    lci->lci_dst_addr.type = ADDR_ILLEGAL;
    lci->lci_dst_addr.length = 0;
    lci->lci_orig_dst_addr.type = ADDR_ILLEGAL;
    lci->lci_orig_dst_addr.length = 0;

    lci->x25_dooutput = (ISCMNS_IEEE(idb->hwptr)) ?
			x25_llc2output : x25_lapboutput;

    lci->lci_dbit = IS_LCI_PVC(idb->hwptr, vc);
    lci->lci_wout = idb->hwptr->x25_wout;
    lci->lci_win = idb->hwptr->x25_win;
    lci->lci_pout = idb->hwptr->x25_pout;
    lci->lci_pin = idb->hwptr->x25_pin;
    lci->lci_force_winsize = FALSE;
    lci->lci_force_pksize = FALSE;
    lci->lci_prec = X25_UNASSIGNED_PREC;
    lci->lci_prec_connected = X25_UNASSIGNED_PREC;
    lci->lci_fast = 0;
    lci->lci_confirm_addr = FALSE;
    lci->lci_reverse = FALSE;
    lci->lci_cug = 0;
    lci->lci_lam = 0;
    lci->lci_tput_out = 0;
    lci->lci_tput_in = 0;
    lci->lci_tdelay = 0;
    lci->lci_dst_addr_fac_reason = 0;
    lci->lci_nuidlen = 0;
    lci->lci_nuid = NULL;

    lci->lci_bytessent = 0;
    lci->lci_bytesrcvd = 0;
    lci->lci_packetssent = 0;
    lci->lci_packetsrcvd = 0;
    lci->lci_resetssent = 0;
    lci->lci_resetsrcvd = 0;
    lci->lci_rnrrcvd = 0;
    lci->lci_intssent = 0;
    lci->lci_intsrcvd = 0;
    lci->lci_rejrcvd = 0;

    lci->lci_map = NULL;
    lci->lci_idb = idb;
    lci->lci_pcb = NULL;
    lci->lci_rtp = NULL;
    lci->lci_alt_index = 0;
    lci->lci_alt_diag = X25_DIAG_NO_INFO;

    /* CONS Defaults */
    lci->lci_src_nsap.type = ADDR_ILLEGAL;
    lci->lci_src_nsap.length = 0;
    lci->lci_dst_nsap.type = ADDR_ILLEGAL;
    lci->lci_dst_nsap.length = 0;
    lci->lci_cudptr = NULL;
    lci->lci_cudlen = 0;

    lci->lci_swtype = -1;
    lci->lci_swdata = NULL;
    lci->lci_llc2 = llc2_cb;

    /*
     *  X.25 MIB defaults
     */
    lci->lci_dte_resets = 0;
    lci->lci_dce_resets = 0;
    lci->lci_t2_count = 0;
    lci->lci_outgoing = TRUE;

/* PHIL _ REGISTRY  ! */
    lci->qllc = NULL;

    /*
     *  packet-by-packet compression defaults
     */
    lci->lci_pbp_enabled = FALSE;
    lci->lci_pbp_send_id = FALSE;
    lci->lci_pbp_compress = FALSE;
    lci->lci_pbp_local_ack = FALSE;
    lci->lci_thc_applied = FALSE;
    lci->lci_pbp_resets = 0;
    lci->lci_pbp_xmt_in_bytes = 0;
    lci->lci_pbp_rcv_out_bytes = 0;
    lci->lci_pbp_xmt_sync = 0;
    lci->lci_pbp_rcv_sync = 0;
    lci->lci_pbp_depak = NULL;
    lci->lci_pbp_ackpak = NULL;
    lci->lci_pbp_comp_stac = NULL;
    lci->lci_pbp_rnr = FALSE;
    lci->lci_pbp_rnr_ticks = 0;
    lci->lci_pbp_need_pak_size = 0;
    lci->lci_pbp_pr = 0;
    lci->lci_pbp_ps = 0;
    lci->lci_pbp_ack = 0;
    lci->lci_pbp_rpr = 0;
    lci->lci_pbp_holdQ_count = 0;
    x25_pbp_clearcounters(lci);
    queue_init(&lci->lci_pbp_rnr_deferredQ, 0);

    reset_interrupt_level(level);
    return(lci);
}

ulong x25_lci_onintstack_delete_cnt = 0;

/*
 * Remove LCI from hash table.
 */

void x25_lci_delete (
    register lcitype *lci)
{
    register int hlci;
    register lcitype *hl, *pl;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * Let the PAD code know that it's lci is going to be deleted
     */

    if (lci->lci_linktype == LINK_PAD)
	pad_lci_delete(lci);

    if (onintstack()) {
         errmsg(&msgsym(DEBUG_LCI, X25));
	 x25_lci_onintstack_delete_cnt++;
    }

    /*
     *  remove the VC from any address mapping
     */
    if (lci->lci_map)
	x25_map_remove_lci(lci);

    /*
     *  remove the VC from the lci structure hash table
     */

    hlci = X25_LCI_HASH(lci->lci_lci);
    hl = x25_lci_hash_table[hlci];
    pl = hl;
    while (hl && (hl != lci)) {
	pl = hl;
	hl = hl->lci_hash;
    }
    if (!hl) {
	errmsg(&msgsym(NOLCI, X25), lci->lci_idb->namestring,
	       lci->lci_lci, "LCI");
    } else {
	if (hl == pl)
	    x25_lci_hash_table[hlci] = hl->lci_hash;
	else
	    pl->lci_hash = hl->lci_hash;
    }

    if (IS_LCI_PVC(lci->lci_idb->hwptr, lci->lci_lci) && lci->lci_rtp) {
	free(lci->lci_rtp);
	lci->lci_rtp = NULL;
    }

    if (lci->lci_cudptr) {
	free(lci->lci_cudptr);
	lci->lci_cudptr = NULL;
    }

    no_x25_pbp_compression (lci);

    if (checkqueue(&x25tQ, lci)) {
	/*
	 * Ick; this thing is not supposed to be in the timer queue
	 * by the time it gets to us.  Complain and nuke.
	 */
	errmsg(&msgsym(INTIMEQ, X25), lci->lci_idb->namestring,
		lci->lci_lci, 0);
	p_unqueue(&x25tQ, lci);
    }

    reg_invoke_media_x25_notify_event(lci->lci_linktype, 
    	lci, X25_DELETE_EVENT, 0, 0, NULL);

    free(lci);
    reset_interrupt_level(level);
}

/*
 *  LCI - hash pick routine.
 *  p-free avioding... so walk the
 *  hash table by indexing.
 */
lcitype *lci_hashpick (
    lcitype *hp,
    int index)
{
    lcitype *ptr;

    for (ptr = hp; ptr; ptr = ptr->lci_hash)
	 if (index-- == 0)
	     break;
    return(ptr);
}

/*
 * Display information about each LCI
 */

/*
 * Note that show_x25status expects automore_enable to be called before
 * calling it.
 * automore_disable should be called after returning from this function.
 */

void show_x25status (int vc)
{
    register lcitype *lci;
    lcitype *lci2;
    x25map *map;
    int hash_bucket;
    int hash_inx;
    int inx;
    int first = TRUE;
    x25routeentrytype *pt;
    uint modulo;
    uint window;
    uint services;
    char buffer[80];

    for (hash_bucket = 0; hash_bucket < X25_LCI_HASH_SIZE; hash_bucket++) {
	/*
	 * hash pick mem lock
	 */
        for (hash_inx = 0;
	     (lci = lci_hashpick(x25_lci_hash_table[hash_bucket], hash_inx));
	     hash_inx++) {

	    if (vc > 0 && vc != lci->lci_lci)
		continue;
	    /* lock lci */
            mem_lock(lci);
	    /*
             * check for HB lci if HB lci
             * do not lock
             * lock address map for that lci
             */
            if (lci->lci_map)
                mem_lock(lci->lci_map);
	    if (!first) {
		automore_conditional(0);
		printf("\n");
	    }

	    printf("\n%cVC %d,  State: %s,  Interface: %s",
		   IS_LCI_PVC(lci->lci_idb->hwptr, lci->lci_lci) ? 'P' : 'S',
		   lci->lci_lci, x25_prstate(lci->lci_idb->hwptr, lci),
		   lci->lci_idb->namestring);
	    printf("\n Started ");
	    print_dhms(lci->lci_starttime);
	    printf(", last input ");
	    print_dhms(lci->lci_lastinput);
	    printf(", output ");
	    print_dhms(lci->lci_lastoutput);

	    switch (lci->lci_linktype) {
	      case LINK_X25:
	      case LINK_X25TCP:
	      case LINK_X25SERVICE:
		if (IS_LCI_PVC(lci->lci_idb->hwptr, lci->lci_lci)) {
		    /*
		     *  print the information on a local or remote PVC
		     */

		    if ((pt = lci->lci_rtp) == NULL) {
			printf("\n PVC <-->");
		    } else if (lci->lci_swtype == X25_ROUTE_IF) {
			printf("\n PVC <--> %s PVC %d, %s%s",
			    pt->idb->namestring,
			    pt->pvc_dest_vc,
			    lci->lci_swdata ? "connected" : "not connected, ",
			    lci->lci_swdata ?
				"" : x25_pvc_err_str[lci->lci_pvc_status]);
		    } else {
			printf("\n PVC <--> [%i] %*s PVC %d, %s%s",
			    pt->ipaddr[0],
			    pt->pvc_remote_if_len, pt->pvc_remote_if,
			    pt->pvc_dest_vc,
			    (lci->lci_pvc_status == X25_PVC_UP) ?
				"connected" : "not connected, ",
			    (lci->lci_pvc_status == X25_PVC_UP) ?
				"" : x25_pvc_err_str[lci->lci_pvc_status]);
		    }
		} else {
		    /*
		     *  print the routed SVC information
		     */
		    if ((lci->lci_swtype != X25_ROUTE_IF) ||
			((lci2 = lci->lci_swdata) == NULL) ||
			!validmem(lci2) ||
			!validmem(lci2->lci_idb))
			lci2 = NULL;

		    printf("\n Connects %s", lci->lci_src_addr.x121_addr);
		    if (lci->lci_src_nsap.length) {
			x25_format_nsap(buffer,
			    &lci->lci_src_nsap.cmns_addr[1],
			    lci->lci_src_nsap_digits & 0x3f);
			printf("/%s", buffer);
			if (lci->lci_outgoing)
			    map = lci2 ? lci2->lci_map : NULL;
			else
			    map = lci->lci_map;

			if (map && (map->map_addr_count > 1) &&
			    (map->map_address[X25_MAP_SECONDARY_ADDR_INX].
			     addr.type == ADDR_MAC_KLUDGE)) {
			    sprintf(buffer, "%e",
				map->map_address[X25_MAP_SECONDARY_ADDR_INX].
				addr.mac_kludge_addr);
			    printf(" [%s]", buffer);
			}
		    }

		    printf(" <--> %s", lci->lci_dst_addr.x121_addr);
		    if (lci->lci_dst_nsap.length) {
			x25_format_nsap(buffer,
			    &lci->lci_dst_nsap.cmns_addr[1],
			    lci->lci_dst_nsap_digits & 0x3f);
			printf("/%s", buffer);
			if (lci->lci_outgoing)
			    map = lci->lci_map;
			else
			    map = lci2 ? lci2->lci_map : NULL;

			if (map && (map->map_addr_count > 1) &&
			    (map->map_address[X25_MAP_SECONDARY_ADDR_INX].
			     addr.type == ADDR_MAC_KLUDGE)) {
			    sprintf(buffer, "%e",
				map->map_address[X25_MAP_SECONDARY_ADDR_INX].
				addr.mac_kludge_addr);
			    printf(" [%s]", buffer);
			}
		    }

		    printf(" %s", (lci->lci_outgoing) ? "from" : "to");

		    /*
		     *  the "other end" is either an X.25 service,
		     *  an X.25 VC, a CMNS VC or an  XOT session
		     */
		    if (lci->lci_linktype == LINK_X25SERVICE) {
			if ((pt = lci->lci_rtp) &&
			    (pt->type == X25_ROUTE_FUNC))
			    printf(" [%s]", x25_funcs[(int)pt->data]);
			else
			    printf(" [?]");
		    } else if (lci->lci_swtype == X25_ROUTE_IPADDR) {
			/*
			 *  "EMPTY"
			 *  XOT information is common for SVCs and PVCs,
			 */
			buffer[0] = '\0';	/* keep compiler happy */
		    } else if (lci2 == NULL) {
			    printf(" nowhere");
		    } else {
			printf(" %s VC %d",
			    lci2->lci_idb->namestring, lci2->lci_lci);
		    }
		}

		if ((lci->lci_swtype == X25_ROUTE_IPADDR) && lci->lci_swdata) {
		    printf("\n XOT between %i, %d and %i, %d",
			   reg_invoke_ip_tcpdriver_lhost(lci->lci_swdata),
			   reg_invoke_ip_tcpdriver_lport(lci->lci_swdata),
			   reg_invoke_ip_tcpdriver_fhost(lci->lci_swdata),
			   reg_invoke_ip_tcpdriver_fport(lci->lci_swdata));
		}

		break;

	      case LINK_PAD:
		printf("\n Connects %s <--> PAD",
		    lci->lci_outgoing ?
			lci->lci_dst_addr.x121_addr :
			lci->lci_src_addr.x121_addr);
		break;

	      default:
		if ((map = lci->lci_map) == NULL) {
		    printf("\n Connects ? <-->");
		    break;
		}

		printf("\n Connects %s <-->",
		  map->map_address[X25_MAP_PRIMARY_ADDR_INX].addr.x121_addr);

		services = map->map_services &
			x25_map_link2service(lci->lci_linktype);

		for (inx = 1; inx < map->map_addr_count; inx++) {
		    /*
		     *  an IP address may support both IP and compressed TCP
		     */
		    if (map->map_address[inx].addr.type == ADDR_IP) {
			if (services & X25_MAPS_COMP_TCP) {
			    print_proto_addr(lci->lci_idb,
					&map->map_address[inx].addr,
					LINK_COMPRESSED_TCP, buffer);
			    printf("\n    %s", buffer);
			    if ((services & X25_MAPS_IP) == 0)
				continue;
			}
		    }

		    /*
		     *  print_proto_addr takes a LINK_ value, not an
		     *  ADDR_ value (*sigh*)
		     */
		    print_proto_addr(
				lci->lci_idb, &map->map_address[inx].addr,
				addr_linktype[map->map_address[inx].addr.type],
				buffer);
		    printf("\n    %s", buffer);
		}

		if (services & X25_MAPS_CLNS)
		    printf("\n    clns");
		if (services & X25_MAPS_BRIDGE)
		    printf("\n    bridge");
		if (services & X25_MAPS_CDP)
		    printf("\n    cdp");

		printf("\n ");

		if (! IS_LCI_PVC(lci->lci_idb->hwptr, lci->lci_lci)) {
		    switch (lci->lci_cud_pid_method) {
		      case X25_CUD_PID_CISCO:
			printf("cisco cud pid, ");
			break;
		      case X25_CUD_PID_IETF:
			printf("ietf cud pid, ");
			break;
		      case X25_CUD_PID_SNAP:
			printf("snap cud pid, ");
			break;
		      case X25_CUD_PID_MULTI:
			printf("multiprotocol cud pid, ");
			break;
		      default:
			printf("no cud pid, ");
			break;
		    }
		}

		switch (lci->lci_data_pid_method) {
		  case X25_DATA_PID_NONE:
		    printf("no Tx data PID");
		    break;
		  case X25_DATA_PID_MULTI:
		    printf("standard Tx data PID");
		    break;
		  case X25_DATA_PID_SNAP:
		    printf("SNAP Tx data PID");
		    break;
		}
	    }

	    if ((lci->lci_linktype == LINK_IP) &&
		(lci->lci_prec != X25_UNASSIGNED_PREC))
		printf(", Precedence %d", lci->lci_prec);
	    if (lci->lci_reverse)
		printf(", Reverse charged");
	    if (lci->lci_dbit)
		printf(", D-bit allowed");
	    printf("\n Window size input: %d, output: %d"
		   "\n Packet size input: %d, output: %d",
		lci->lci_win, lci->lci_wout,
		1 << lci->lci_pin, 1 << lci->lci_pout);
	    modulo = lci->lci_idb->hwptr->x25_modulo;
	    printf("\n PS: %d  PR: %d  ACK: %d  Remote PR: %d  RCNT: %d"
		   "  RNR: %s%s",
		lci->lci_ps, lci->lci_pr, lci->lci_ack, lci->lci_rpr,
		((lci->lci_pr +  modulo) - lci->lci_ack) % modulo,
		lci->lci_busy ? "TRUE" : "FALSE",
		(lci->lci_pbp_compress && lci->lci_pbp_rnr) ?
			"  RNR sent" : "");
	    window = (lci->lci_ps - lci->lci_rpr + modulo) % modulo;
	    if (window >= lci->lci_wout)
		printf("\n Window is closed");
	    printf("\n Retransmits: %d  Timer (secs): %u"
		   "  Reassembly (bytes): %d"
		   "\n Held Fragments/Packets: %d/%d",
		lci->lci_retransmit,
		TIME_LEFT_SLEEPING(lci->lci_timer) / ONESEC,
		lci->lci_reassmbytes, lci->lci_tx_frag_cnt,
		lci->lci_holdQ.count);
	    printf("\n Bytes %d/%d Packets %d/%d Resets %d/%d"
		   " RNRs 0/%d REJs 0/%d INTs %d/%d",
		lci->lci_bytessent, lci->lci_bytesrcvd,
		lci->lci_packetssent, lci->lci_packetsrcvd,
		lci->lci_resetssent, lci->lci_resetsrcvd,
		lci->lci_rnrrcvd,
		lci->lci_rejrcvd,
		lci->lci_intssent, lci->lci_intsrcvd);
	    if (lci->lci_pbp_compress)
		x25_pbp_show_stats(lci);
	    first = FALSE;

	    /*
	     *  free our locked references
	     */
	    if (lci->lci_map)
                free(lci->lci_map);
	    free(lci);
	}
    }
}

/*
 * Dump information for each PVC
 */

void x25_pvc_nvgen (parseinfo *csb)
{
    hwidbtype *hwidb = csb->interface->hwptr;
    int vc;
    lcitype *lci;
    int first_svc;
    x25routeentrytype *p;

    /*
     *  CONS doesn't have PVCs
     */
    if (ISCMNS_IEEE(hwidb))
	return;

    /*
     *  determine the lowest SVC number from the administrative limits
     */
    first_svc = hwidb->x25_admin_lic ? hwidb->x25_admin_lic :
		(hwidb->x25_admin_ltc ? hwidb->x25_admin_ltc :
		 (hwidb->x25_admin_loc ? hwidb->x25_admin_loc :
		  X25_MAX_LCI + 1));

    /*
     *  determine the lowest SVC number from the operational limits
     */
    vc = hwidb->x25_lic ? hwidb->x25_lic :
	 (hwidb->x25_ltc ? hwidb->x25_ltc :
	  (hwidb->x25_loc ? hwidb->x25_loc :
	   X25_MAX_LCI + 1));

    /*
     *  take the lowest permissible SVC number
     */
    if (vc < first_svc) {
	first_svc = vc;
    }

    /*
     *  check if no PVCs are allowed
     */
    if (first_svc == 1) {
	return;
    }

    /*
     *  loop through all allowable PVCs in canonical order
     */
    for (vc = 0; (lci = x25_lci_lookup_next(hwidb, vc, NULL)); ) {
	vc = lci->lci_lci;
	if (vc >= first_svc)
	    break;
	if (csb->interface != lci->lci_idb)
	    continue;	/* skip PVCs defined for other subinterfaces */
	if (lci->lci_linktype == LINK_PAD)
	    continue;	/* PAD PVCs are created by "translate"  */

	/*
	 *  start the configuration command
	 */
	nv_write(TRUE, "%s %d", csb->nv_command, lci->lci_lci);

	/*
	 *  encapsulation PVCs share code with map commands
	 */
	if (lci->lci_map) {
	    x25_map_entry_nvgen(lci->lci_map, NULL);
	    continue;
	}

	/*
	 *  this is a locally or remotely switched PVC
	 */

	p = lci->lci_rtp;

	nv_add(lci->lci_swtype == X25_ROUTE_IF,
	       " interface %s pvc %d", p->idb->namestring, p->pvc_dest_vc);
	nv_add(lci->lci_swtype == X25_ROUTE_IPADDR,
	       " tunnel %i interface %*s %*s pvc %d", p->ipaddr[0],
		p->pvc_remote_if_type_len, p->pvc_remote_if,
		p->pvc_remote_if_len - p->pvc_remote_if_type_len,
		&p->pvc_remote_if[p->pvc_remote_if_type_len],
		p->pvc_dest_vc);

	nv_add((lci->lci_pin != hwidb->x25_admin_pin) ||
	       (lci->lci_pout != hwidb->x25_admin_pout),
	       " packetsize %d %d", 1 << lci->lci_pin, 1 << lci->lci_pout);
	nv_add((lci->lci_win != hwidb->x25_admin_win) ||
	       (lci->lci_wout != hwidb->x25_admin_wout),
	       " windowsize %d %d", lci->lci_win, lci->lci_wout);
    }
}

/*
 * x25_clear_vc
 * Clear a virtual circuit from clear command
 */

void x25_clear_vc (parseinfo *csb)
{
    hwidbtype *idb;
    int vc;
    lcitype *lci;
    leveltype level;

    idb = GETOBJ(idb,1)->hwptr;
    vc = GETOBJ(int,1);

    if (!is_x25(idb)) {
	printf("\nInterface is not an X.25 interface");
	return;
    }
    if (!ISCMNS_IEEE(idb) && !idb->lapb_upstate) {
	printf("\nLower protocol is not up");
	return;
    }

    /*
     *  if no VC was specified, confirm a RESTART of the service
     */
    if (vc == -1) {
	printf("\nForce interface RESTART%s",
		idb->x25_state == X25_R1 ?
		"" : " (interface is currently RESTARTing)");
	if (!confirm(" "))
	    return;

	level = raise_interrupt_level(NETS_DISABLE);
	idb->x25_retransmit = 0;
	x25_sendcause(idb, (lcitype *)NULL, X25_RESTART,
			X25_RESTART_NETWORK_OPERATIONAL,
			X25_DIAG_MAINTENANCE_ACTION, NULL);
	reset_interrupt_level(level);
	return;
    }

    /*
     *  a particular VC was given; CLEAR an SVC, RESET a PVC
     */

    lci = x25_lci_lookup(idb, vc, NULL);
    if (lci == (lcitype *)NULL) {
	printf("\nLogical channel %d not found", vc);
	return;
    }

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     *  we cannot RESET a PVC if the service is not up
     *  (if the service isn't up, there are no SVCs)
     */
    if (idb->x25_state != X25_R1) {
	reset_interrupt_level(level);
	printf("\nCannot RESET a VC with interface in state %s",
		x25_prstate(idb, NULL));
	return;
    }

    if (IS_LCI_PVC(idb, vc)) {
	if (lci->lci_state == X25_D1) {
	    x25_sendcause(idb, lci, X25_RESET,
			  X25_RESET_NETWORK_OPERATIONAL,
			  X25_DIAG_MAINTENANCE_ACTION, NULL);
	    reset_interrupt_level(level);
	} else {
	    reset_interrupt_level(level);
	    printf("\nCannot RESET a PVC in state %s", x25_prstate(idb,lci));
	}
    } else {
	/*
	 *  allow a CLEAR from states P2/P3, P5 and P4/D1/D2/D3
	 */
	if (((lci->lci_state >= X25_P2) &&
	     (lci->lci_state <= X25_P5)) ||
	    ((lci->lci_state >= X25_D1) &&
	     (lci->lci_state <= X25_D3))) {
	    x25_sendcause(idb, lci, X25_CLEAR,
			  X25_CLEAR_OUT_OF_ORDER,
			  X25_DIAG_MAINTENANCE_ACTION, NULL);
	    reset_interrupt_level(level);
	} else {
	    reset_interrupt_level(level);
	    printf("\nCannot CLEAR an SVC in state %s", x25_prstate(idb, lci));
	}
    }
}
