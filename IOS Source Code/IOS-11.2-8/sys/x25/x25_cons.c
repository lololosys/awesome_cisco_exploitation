/* $Id: x25_cons.c,v 3.3.62.3 1996/08/28 13:23:31 thille Exp $
 * $Source: /release/112/cvs/Xsys/x25/x25_cons.c,v $
 *------------------------------------------------------------------
 * x25_cons.c -- CONS Packet level (3)
 *
 * May 1991, Babak Nader
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x25_cons.c,v $
 * Revision 3.3.62.3  1996/08/28  13:23:31  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.62.2  1996/05/17  12:17:19  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.48.2  1996/04/03  22:18:00  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.48.1  1996/03/01  16:31:49  ppearce
 * CSCdi50498:  IBU modularity - Implement LLC subblock
 * Branch: IbuMod_Calif_branch
 *   Move LLC fields from idbtype struct to LLC subblock
 *
 * Revision 3.3.62.1  1996/03/18  22:50:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  11:19:35  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  21:56:03  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  18:07:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:52:03  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:53:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/10/02  22:01:54  carvalho
 * CSCdi39783:  CMNS call to ethernet destination fails if LLC2 not open.
 *
 * Revision 2.5  1995/09/02  08:34:53  pmorton
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
 * Revision 2.4  1995/08/16  17:30:43  gglick
 * CSCdi30978:  CMNS maps deleted on call setup failure
 *
 * Revision 2.3  1995/07/20  07:38:17  hampton
 * Convert X.25 to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37392]
 *
 * Revision 2.2  1995/06/09 13:25:32  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:22:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "../os/buffers.h"
#include "address.h"
#include "../if/network.h"
#include "packet.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "../llc2/llc2.h"
#include "../llc2/llc2_sb.h"
#include "lapb.h"
#include "x25.h"
#include "x25_address.h"
#include "x25_proto.h"
#include "x25_switch.h"
#include "x25_debug.h"
#include "logger.h"
#include "../x25/msg_x25.c"		/* Not a typo, see logger.h */
#include "subsys.h"
#include "x25_pbp.h"
#include "ieee.h"


static void cons_init (subsystype *subsys)
{
    cons_parser_init();
}

#define	CONS_MAJVERSION 1
#define	CONS_MINVERSION 0
#define	CONS_EDITVERSION  1

SUBSYS_HEADER(cmns, CONS_MAJVERSION, CONS_MINVERSION, CONS_EDITVERSION,
	      cons_init, SUBSYS_CLASS_PROTOCOL,
	      "req: llc2",
	      NULL);


/* get_lci_considb
 * return considb of the llc2
 * in the given lci
 */
considbtype *get_lci_considb (lcitype *lci)
{
    llctype *llc2_cb;

    if (lci != NULL) {
	llc2_cb = lci->lci_llc2;
	if (llc2_cb != NULL && validmem(llc2_cb)) {
	    considbtype *considb;

	    considb = llc2_cb->considb;
	    if (considb != NULL && validmem(considb))
		return(considb);
	}
    }
    return(NULL);
}

/*
 * getconsidb
 */
static considbtype *getconsidb (
    hwidbtype *hwidb)
{
    considbtype *considb = NULL;

    if (ISCMNS_IEEE(hwidb)) {
	/*
	 *  allocate and initialize the CMNS service strcture
	 */
	considb = malloc(sizeof(considbtype));
	if (considb != NULL) {
	    considb->cmns_dce = LAPBX25_DXE;
	    considb->x25_restart = FALSE;
	    considb->x25_upstate = FALSE;
	    considb->x25_restartssent = 0;
	    considb->x25_restartsrcvd = 0;
	    considb->x25_cause = X25_RESTART_DTE_ORIGINATED;
	    considb->x25_diag = X25_DIAG_NO_INFO;
	}
    }

    return(considb);
}

/* set_lci_considb
 * set considb of the llc2
 * in the givenlci
 */
boolean set_lci_considb (
    hwidbtype *hwidb,
    lcitype *lci)
{
    llctype *llc2_cb = NULL;

    if (lci != NULL) {
	considbtype *considb;

	llc2_cb = lci->lci_llc2;
	if ((llc2_cb->considb == NULL) && (considb = getconsidb(hwidb))) {
	    llc2_cb->considb = considb;
	    return(llc2_cb->considb != NULL);
	}
    }

    return(FALSE);
}

/* x25_cons_route_failed
 * send message route failed
 *
 */
void x25_cons_route_failed (
    lcitype *lci,
    int diag)
{
    lci->lci_idb->hwptr->x25_callsrcvdfailed++;
    lci->lci_idb->hwptr->x25_callsfrwdfailed++;
    x25_sendcause(lci->lci_idb->hwptr, lci, X25_CLEAR,
		 X25_CLEAR_LOCAL_PROC_ERR, diag, NULL);

    if ((x25_debug || x25event_debug) &&
	(x25_vcnum == 0 || x25_vcnum == lci->lci_lci))
	 buginf("\nCannot route call");
}

/*
 * cons_enqueue
 * Called from interrupt level to enqueue an ISO CONS datagram to the
 * X25 input routine
 */
void cons_enqueue (paktype *pak)
{
    hwidbtype *idb = pak->if_input->hwptr;

    if (idb->cmns_intfc_enabled) {
	/*
	 * The pak->datagramstart for X.25 should be
	 * already set correctly by LLC2 therefore process
	 * incoming packet by calling x25_input.
	 *
	 */
	x25_input(idb, pak);
    } else {
	netinput_enqueue(pak);
    }
}

/*
 * cons_restart_lci
 * This routine check for HB lci
 * for restart if not create and set
 * its correct states.
 */
lcitype *cons_restart_lci (
   hwidbtype *hwidb,
   paktype *pak)
{
   lcitype *lci;
   llctype *llc2_cb = pak->llc2_cb;

   /*
    *  change to a valid configuration
    */
   (void) x25_valid_config(hwidb, TRUE);

   /*
    * Check for an unconnected lci with the given llc2_cb
    */
   lci = x25_lci_llc2_lookup(hwidb, llc2_cb, TRUE);

   if (lci == NULL) {
	/*
	 * HB lci not found therefore
	 * get lci for restart
	 */
	lci = x25_lci_enter(hwidb->firstsw, X25_NO_VC_ASSIGNED, llc2_cb);

	if (!lci)
	   return(lci);

	lci->lci_linktype = LINK_X25;

	/*
	 *  a host has opened an llc2 connection to us
	 *  attach a call-back routine and save the llc2_cb
	 */
	if (!llc2_attach(pak->llc2_cb, LINK_LLC2, 
                         llc_sb_get_wind(hwidb->firstsw),
			 (void(*)(llctype *,int))x25_llc2_clsproc)) {
	     /* llc2 failed */
	     x25_lci_delete(lci);
	     return(NULL);
	}

	lci->lci_llc2 = pak->llc2_cb;
	if (!set_lci_considb(hwidb, lci)) {
	    /* no memory */
	    x25_lci_delete(lci);
	    return(NULL);
	}
        lci->lci_state = X25_R2;	/* DXE */
        TIMER_STOP(lci->lci_timer);
    }

    return(lci);
}

/*
 * x25_process_held_lci
 * called when packet level on a CMNS interface enters R1 state (sending or
 * receiving a restart confirmation).  If we deferred forwarding any call
 * requests, send them now.
 */

static void x25_process_held_lci (
   hwidbtype *hwidb,
   lcitype *rlci)
{
    register x25map *map;
    register llctype *llc2 = rlci->lci_llc2;
    addrtype mac_addr;
    uint rc;
    boolean found_rlci;
    leveltype level;

    found_rlci = FALSE;

    /* strip DSAP/LSAP off from the end */
    memset(&mac_addr, 0, sizeof(addrtype));
    mac_addr.type = ADDR_MAC_KLUDGE;
    mac_addr.length = ADDRLEN_MAC_KLUDGE;
    ieee_copy(&llc2->llc2_dte[RMAC_DTE], mac_addr.mac_kludge_addr);

    level = raise_interrupt_level(NETS_DISABLE);

    map = x25_map_find(hwidb, &mac_addr, X25_MATCH_EXACT, X25_MAPS_CMNS);

    while (map && map->map_nvc != 0) {
	lcitype *lci;

	/* Remove a pending call from the list and forward it.  */
	--map->map_nvc;
	lci = map->map_lci[ map->map_nvc ];
	map->map_lci[ map->map_nvc ] = NULL;

	rc = cons_forward_call( lci,	/* lci of incoming call */
				llc2,	/* outgoing llc2 session */
				lci->lci_cudptr,	/* call user data */
				lci->lci_cudlen, 
				TRUE );	/* forward the call now */

	/* if we were unable to forward the call, clear it. */
	if (rc == X25_FORWARD_ERR)
	    x25_cons_route_failed(lci, X25_DIAG_NO_INFO);
    }

    /*
     * unqueue from timer queue and
     * Remove HB lci used for Restart
     * since restart phase completed
     */
    if (checkqueue(&x25tQ, rlci)) {
	p_unqueue(&x25tQ, rlci);
	TIMER_STOP(rlci->lci_timer);
    }

    if (rlci->lci_map != NULL && !validmem(rlci->lci_map))
	rlci->lci_map = NULL;
    x25_lci_delete(rlci);
    reset_interrupt_level(level);
}

/*
 * x25_flush_held_lci
 * check all lci's associated with
 * given llc2 and flush them
 */

static void x25_flush_held_lci (
   hwidbtype *hwidb,
   llctype *llc2)
{
    register x25map *map;
    leveltype level;
    addrtype mac_addr;
    lcitype *lci;

    memset(&mac_addr, 0, sizeof(addrtype));
    mac_addr.type = ADDR_MAC_KLUDGE;
    mac_addr.length = ADDRLEN_MAC_KLUDGE;
    /* strip DSAP/LSAP off from the end */
    ieee_copy(&llc2->llc2_dte[RMAC_DTE], mac_addr.mac_kludge_addr);

    level = raise_interrupt_level(NETS_DISABLE);

    map = x25_map_find(hwidb, &mac_addr, X25_MATCH_EXACT, X25_MAPS_CMNS);

    if (map) {
	while (map->map_nvc > 0) {

	    /* Remove a pending call from the list and clear it.  */
	    --map->map_nvc;
	    lci = map->map_lci[ map->map_nvc ];
	    map->map_lci[ map->map_nvc ] = NULL;

	    x25_cons_route_failed(lci, X25_DIAG_MAINTENANCE_ACTION);
	}
    }

    reset_interrupt_level(level);
}

/*
 * cons_cstate
 * Called by the parser when
 * a MAC entry is place for CONS.
 */

void cons_cstate (
   register hwidbtype *hwidb,
   register llctype *llc2,
   boolean upstate)
{
    lcitype *lci;
    considbtype *considb;

    /*
     *  change to a valid configuration
     */
    (void) x25_valid_config(hwidb, TRUE);

    if (hwidb->state == IDBS_UP && upstate) {
	if ((considb = getconsidb(hwidb)) == NULL) {
	    /* no mem */
	    cons_cstate(hwidb, llc2, FALSE);
	    return;
	}

	/* create a half baked lci that will be deleted after Restart confirm */
	lci = x25_lci_enter(hwidb->firstsw, X25_NO_VC_ASSIGNED, llc2);

	if (lci == NULL) {
	    /* no mem */
	    cons_cstate(hwidb, llc2, FALSE);
	    return;
	}

	lci->lci_linktype = LINK_X25;
	lci->lci_state = X25_R1;
	llc2->considb = considb;
	lci->lci_llc2 = llc2;
	x25_sendcause(hwidb, lci, X25_RESTART,
		      considb->x25_cause, considb->x25_diag, NULL);
    } else {
        /* flush all Held lci's */
        x25_flush_held_lci(hwidb, llc2);

	/*
	 * Clear all VC and address
	 * map in use by that Control Block
	 */
	considb = llc2->considb;
	x25_lci_hash_restart(hwidb, llc2);
	x25_map_hash_restart(hwidb, llc2);

	if (considb != NULL && validmem((void *)considb->x25_upstate)) {
	    free(considb);
	    considb = NULL;
	} else
	    considb = NULL;

	llc2_close(llc2);
    }
}

/*
 * x25_retransmit_restart
 * In case of Restart collision
 * when DTE originated restart
 * is recieved and restart is sent
 * with cause code of DTE originated
 */
static void x25_retransmit_restart(
   hwidbtype *hwidb,
   lcitype *lci)
{
   considbtype *considb = get_lci_considb(lci);

   /*
    * Pause for a random time and then retransmit.
    */
   process_sleep_for(0);
   x25_sendcause(hwidb, lci, X25_RESTART,
		 considb->x25_cause, considb->x25_diag, NULL);
}

/*
 * cons_restart
 * restart negotiation or confirm.
 *
 */
void cons_restart (
    hwidbtype *hwidb,
    lcitype *lci,
    paktype *pak,
    boolean ack)
{
    register packettype *p = (packettype *)pak->datagramstart;
    uchar *c = (uchar *)p + X25HEADERBYTES;
    considbtype *considb = get_lci_considb(lci);

    if (ack) {
	/*
	 * Restart Indication
	 * find role as DTE or DCE
	 */
	if (IS_DXE(considb, cmns_dce)) {
	    if (IS_X25_RESTART_DTE_ORIGINATED(c)) {
		if (considb->x25_restartssent) {
		    considb->x25_restartssent = 0; /* from begining */
		    /*
		     * Restart collision, start
		     * back-off timer retransmit and
		     * consider this restart finished
		     * case (c)
		     */
		     x25_retransmit_restart(hwidb, lci);
		     return;
		} else {
		    /* from another DTE case (b) */
		    considb->cmns_dce = LAPBX25_DCE;
		}
	    } else {
		/* from another DCE  case (a) */
		considb->cmns_dce = LAPBX25_DTE;
	    }
	}

	sendpacket(hwidb, lci, X25_RESTART_CONFIRMATION, 0, NULL);
	x25_process_held_lci(hwidb, lci);
	considb->x25_upstate = TRUE;
	/*
	 * block configuration until
	 * the interface is down.
	 */
	if (!hwidb->x25_upstate)
	    hwidb->x25_upstate = TRUE;
    } else {
	/*
	 * Restart confirm
	 */
	if (IS_DXE(considb, cmns_dce)) {
	    /*
	     * if not DCE then it is DTE/DTE
	     * case (d)
	     */
	    considb->cmns_dce = LAPBX25_DTE;
	}
	x25_process_held_lci(hwidb, lci);
        considb->x25_upstate = TRUE;
	/*
	 * block configuration until
	 * the interface is down.
	 */
	if (!hwidb->x25_upstate)
	    hwidb->x25_upstate = TRUE;
    }
}

/*
 * x25_llc2output
 * Actually output an X25 data packet; return FALSE if unable to
 * send because of a buffer failure when fragmenting.
 */

boolean x25_llc2output (register paktype *pak)
{
    register hwidbtype *hwidb = pak->if_output->hwptr;
    register lcitype *lci = pak->x25_cb;
    register packettype *p;
    uchar *c, mbit;
    ushort encsize;
    ushort cons_encsize;

    cons_encsize = pak->llc2_sapoffset + LLC2HEADERBYTES;
    encsize = (hwidb->x25_modulo == X25_MOD8) ?
		X25HEADERBYTES : X25EHEADERBYTES;

    /*
     *  we only do routing (never encapsulation) between CONS connections,
     *  so we never do fragmentation
     */

    /*
     *  we'll set the packet's M bit if it's specified in the buffer flags
     *  and it has the maximum number of data octets (required for GOSIP)
     */
    mbit = (pak->flags & PAK_MBIT) &&
	   ((pak->datagramsize - encsize - cons_encsize) ==
	    (1 << lci->lci_pout));
    /*
     *  form the DATA packet header
     */
    p = (packettype *)(pak->datagramstart + cons_encsize);
    p->x25_q = (pak->x25_flags & PAK_X25_QBIT) ? 1 : 0;
    p->x25_d = (pak->x25_flags & PAK_X25_DBIT) ? 1 : 0;
    p->x25_sn = (hwidb->x25_modulo == X25_MOD8) ?
                X25_MODULO_8 : X25_MODULO_128;
    p->x25_lcg = LCG(lci->lci_lci);
    p->x25_lcn = LCN(lci->lci_lci);

    if (hwidb->x25_modulo == X25_MOD8) {
	p->x25_pti = (lci->lci_ack << 5) | (mbit ? (1 << 4) : 0) |
		     (lci->lci_ps << 1) | X25_DATA;
    } else {
	c = (uchar *)p + X25HEADERBYTES;
	p->x25_pti = (lci->lci_ps << 1) | X25_DATA;
	*c = (lci->lci_ack << 1) | (mbit ? 1 : 0);
    }

    lci->lci_bytessent += (pak->datagramsize - encsize - cons_encsize);
    if (x25_debug) {
	pak->datagramstart += cons_encsize;
	pak->datagramsize -= cons_encsize;
	x25_prpack(hwidb, pak, lci, 'O');
	pak->datagramstart -= cons_encsize;
	pak->datagramsize += cons_encsize;
    }
    pak->llc2_cb = lci->lci_llc2;
    if (llc_get_oqueue_status(lci->lci_llc2) == DLC_OQUEUE_FULL) {
        datagram_done(pak);
    } else {
        llc_output(pak, LLC_IFRAME);
    }
    GET_TIMESTAMP(lci->lci_lastoutput);
    lci->lci_packetssent++;
    lci->lci_ps = ++lci->lci_ps % hwidb->x25_modulo;

    return(TRUE);
}

#ifdef ANNA
/*
 * llc2_prstatus
 * Return string for given LLC2 state
 */

char *llc2_prstatus (
    int status)
{

    switch (status) {
    case LC_CONNECT:
	return "LC_CONNECT";
    case LC_CONCNF:
	return "LC_CONCNF";
    case LC_DATA:
	return "LC_DATA";
    case LC_DATACNF:
	return "LC_DATACNF";
    case LC_DISC:
	return "LC_DISC";
    case LC_DISCNF:
	return "LC_DISCNF";
    case LC_RESET:
	return "LC_RESET";
    case LC_RSTCNF:
	return "LC_RSTCNF";
    case LC_REPORT:
	return "LC_REPORT";
    case LC_FLOWCONTROL:
	return "LC_FLOWCONTROL";
    case LC_TIMEOUT:
	return "LC_TIMEOUT";
    case LC_INTDOWN:
	return "LC_INTDOWN";
    default:
	return "UNKNOWN";
    }
}
#endif


/*
 * x25_llc2_clsproc
 * X25 (ISO CONS ) status process being called
 * from LLC2. This routine is called to let us
 * know that LLC2 link is down or call is
 * disconnected.
 */
void x25_llc2_clsproc (
    llctype *handle,
    int status)
{
    llctype *llc2 = (llctype *)handle;
   register hwidbtype *idb;
   register leveltype level;

   if (llc2 == NULL )  /* no cookie */
	return;

   idb = llc2->idb->hwptr;

   if (x25_debug || x25event_debug)
	buginf("\n%s: CMNS [ LLC2 DTE=%02x%02x.%02x%02x.%02x%02x ]"
	       "  Status %s", llc2->idb->namestring,
		llc2->llc2_dte[0], llc2->llc2_dte[1], llc2->llc2_dte[2],
		llc2->llc2_dte[3], llc2->llc2_dte[4], llc2->llc2_dte[5],
		llc2_prreason(status));

   level = raise_interrupt_level(NETS_DISABLE);

   switch (status) {
     case LC_CONNECT:
	cons_cstate(idb, llc2, TRUE);
	break;
     case LC_INTDOWN:
	  idb->x25_upstate = FALSE;
	  /* Fall through */
     case LC_TIMEOUT:
     case LC_DISC_TIMEOUT:
     case LC_RESET:
     case LC_DISC:
     case LC_DISC_LAN:
     case LC_DISC_CNF:
	/*
	 * LLC2 link down
	 */
	cons_cstate(idb, llc2, FALSE);
	break;
     default:
	errmsg(&msgsym(BADUPCALL, X25), llc2->idb->namestring,
		llc2_prstatus(status),
		llc2->llc2_dte[0], llc2->llc2_dte[1], llc2->llc2_dte[2],
		llc2->llc2_dte[3], llc2->llc2_dte[4], llc2->llc2_dte[5]);
        break;

   }

   reset_interrupt_level(level);
}
