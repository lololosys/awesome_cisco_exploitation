/* $Id: atm_arp.c,v 3.8.20.27 1996/09/13 16:34:51 mdavison Exp $
 * $Source: /release/112/cvs/Xsys/atm/atm_arp.c,v $
 *------------------------------------------------------------------
 * A T M _ A R P . C
 *
 * This module handles ATM ARP and InARP requests as specified in RFC1577
 *
 * January 1995, Ricky Li Fo Sjoe
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atm_arp.c,v $
 * Revision 3.8.20.27  1996/09/13  16:34:51  mdavison
 * CSCdi67952:  Clients fail to contact arp server using rfc1577
 * Branch: California_branch
 * A loop-control variable was incorrectly used in atm_arp_periodic().
 * This problem was fixed, thus allowing multiple physical interfaces
 * to initialize properly.
 *
 * Revision 3.8.20.26  1996/09/08  03:02:20  rzagst
 * CSCdi62883:  debug atm packet shows incorrect message for oam cells
 * Branch: California_branch
 * Use a status to prevent oam and inarp timers from being started when
 * running or stopped when not running.
 *
 * Revision 3.8.20.25  1996/08/16  02:13:07  lmercili
 * CSCdi65496:  rfc1577 failed ip routing
 * Branch: California_branch
 *
 * Revision 3.8.20.24  1996/08/08  05:05:27  lmercili
 * CSCdi63861:  rfc1577 stops working when router has more than one AIP
 * Branch: California_branch
 *
 * Revision 3.8.20.23  1996/08/06  08:27:02  gchristy
 * CSCdi64974:  IP Cache: delayed invalidation can create inconsistancies
 * Branch: California_branch
 *  - Back out changes made for CSCdi55725.
 *
 * Revision 3.8.20.22  1996/08/05  22:12:34  jwjang
 * CSCdi65057:  print out err message when sscop detects unmatch uni
 * version
 * Branch: California_branch
 *         CSCdi64992, changed malloc() to malloc_nsmae() in atm_arp.c
 *         fix sscop_rcvQ_cnt and sscop_ackQ_cnt, fixed  no timer running
 *         in  sscop outErrorRecovery state
 *
 * Revision 3.8.20.21  1996/08/05  17:26:41  rzagst
 * CSCdi63891:  Memory leak with AIP card
 * Insure if datagram_out(pak) is not called call datagram_done(pak) in
 * atm_arp_sendreq.
 * Branch: California_branch
 *
 * Revision 3.8.20.20  1996/08/01  20:20:24  awu
 * CSCdi62194:  %SYS-3-INVMEMINT:Invalid memory action; %SYS-2-MALLOCFAIL:
 * Memory al
 * Branch: California_branch
 *  -introduce reg_invoke_atm_vc_activated()/deactivated(), for the
 *   callback of activating/deactivating a PVC.
 *
 * Revision 3.8.20.19  1996/07/23  13:20:03  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.8.20.18  1996/07/10  22:19:53  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.8.20.17  1996/06/29  22:04:17  rzagst
 * CSCdi59683:  OAM cells are not transmitted after ATM int was shut/noshut
 * Branch: California_branch
 *
 * Revision 3.8.20.16  1996/06/28  23:05:05  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.8.20.15  1996/06/27  17:54:11  yanke
 * Remove mapgroup if no arp server left.
 *
 * CSCdi60385:  LECs cannot rejoin ELAN after changing ilmi prefix on a
 * port
 * Branch: California_branch
 *
 * Revision 3.8.20.14  1996/06/24  16:03:56  myeung
 * CSCdi55725:  OSPF CPU HOG
 * Branch: California_branch
 * - Modify IP cache invalidation code to allow delayed invalidation
 *
 * Revision 3.8.20.13  1996/06/19  23:19:09  rzagst
 * CSCdi60596:  ATM ARP fix to CSCdi56362 arp init called w/o atm interface
 * Branch: California_branch
 *
 * Revision 3.8.20.12  1996/06/09  06:05:46  achopra
 * CSCdi51530:  ATM ARP client misregisters if no address is configured
 * Branch: California_branch
 *
 * Revision 3.8.20.11  1996/05/21  09:42:57  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8.20.10  1996/05/13  22:56:35  mdavison
 * CSCdi56285:  potential minor memory leak when building atm arp map
 * Branch: California_branch
 *
 * Revision 3.8.20.9  1996/05/07  04:32:00  rzagst
 * CSCdi56362:  ATM ARP init can be called when no atm interface is present
 * backing out fix since it kills atm arp.
 * Branch: California_branch
 *
 * Revision 3.8.20.8  1996/05/07  00:37:41  rzagst
 * CSCdi56362:  ATM ARP init can be called when no atm interface is present
 * Branch: California_branch
 *
 * Revision 3.8.20.7  1996/05/05  17:08:31  rzagst
 * CSCdi54165:  Cant create pvcs during subinterface shutdown
 * Branch: California_branch
 *
 * Revision 3.8.20.6  1996/05/01  14:44:55  achopra
 * CSCdi52007:  shut down an ATM ARP client on subinterface doesnt clear
 * map entry
 * Branch: California_branch
 *
 * Revision 3.8.20.5  1996/04/25  23:01:46  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.8.20.4  1996/04/09  22:47:55  ronnie
 * CSCdi51737:  SYS-3-INVMEMINT: Invalid memory action (free) at interrupt
 * level
 * Branch: California_branch
 *
 * Revision 3.8.20.3  1996/04/08  20:51:26  ronnie
 * CSCdi51093:  no atm arp-server command causes crash
 * Branch: California_branch
 *
 * Revision 3.8.20.2  1996/03/29  23:41:53  schrupp
 * CSCdi53085:  ATMSIG  11.0, 11.1 bugfixes for 11.2
 * Branch: California_branch
 * Bugfixes from 11.0, 11.1 into 11.2
 *
 *  CSCdi35689 shut down an ATM ARP
 *  CSCdi46600 Concurrent 1577 and LANE crashes in ATMSIG upon reboot
 *  CSCdi47523 Crash in ATM Signalling code
 *  CSCdi48594 %SUBSYS-2-NOTFOUND: atmsig needs atmilmi on boothelper
 *  images
 *  CSCdi49728 bus error on 7000 running ATM,ATALK,OSPF,SNMP,IPX with 11.0
 *  CSCdi50591 ATMSIG:  release complete uses invalid cause value 80h
 *  CSCdi51037 IBM 25 Mbits ATM-ARP connectivity pb / rfc1755 support
 *  CSCdi52025 router crash at atmSig_callPresentState
 *
 * Revision 3.8.20.1  1996/03/18  18:59:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.6.2  1996/03/07  08:30:12  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.6.1  1996/02/20  00:18:35  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/01  05:59:32  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.7  1996/01/29  07:26:18  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.6  1996/01/09  20:14:45  ronnie
 * CSCdi44113:  Alignment error in ATM ARP code
 *
 * Revision 3.5  1995/12/17  18:24:40  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.4  1995/11/22  20:44:17  ronnie
 * CSCdi44212:  Many fixes done at Telecom Finland
 *
 * Revision 3.3  1995/11/17  08:45:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:06:58  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:59:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.18  1995/11/08  20:51:28  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.17  1995/10/23  02:54:04  ronnie
 * CSCdi42570:  Yet another crash from the ATM ARP Server
 *
 * Revision 2.16  1995/10/21  09:19:15  ronnie
 * CSCdi39635:  Connections to ARP server torn down too quickly
 *
 * Revision 2.15  1995/09/28  20:49:06  ronnie
 * CSCdi40510:  Too many ATM SVCs set up to same destination
 *              Also added some debugging to the ATM SigAPI
 *
 * Revision 2.14  1995/09/20  21:28:02  ronnie
 * CSCdi39998:  ATM ARP Server calls staticmap_enter() with a NULL maplist
 *
 * Revision 2.13  1995/09/16  03:19:28  ronnie
 * CSCdi40452:  ATM InARP replies are bogus
 *
 * Revision 2.12  1995/09/14  21:07:07  ronnie
 * CSCdi40212:  Declaring a new ARP Server leaves old servers map lying
 * around
 *
 * Revision 2.11  1995/09/11  23:21:39  ronnie
 * CSCdi39485:  ATM config magically changes
 *
 * Revision 2.10  1995/09/09  00:50:58  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.9  1995/08/11  20:58:18  ronnie
 * CSCdi38022:  ATM ARP client needs to check for valid NSAP before
 * creating map
 *
 * Revision 2.8  1995/08/08  16:45:28  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.7  1995/07/05 18:34:29  ronnie
 * CSCdi36248:  ARP server could not resolve NSAP for client
 *
 * Revision 2.6  1995/06/18  20:34:29  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.5  1995/06/18  20:31:36  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.4  1995/06/18  06:17:57  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.3  1995/06/13 23:33:18  ronnie
 * CSCdi35695:  debug atm arp only debugs ATM ARP server, not ARP clients
 *
 * Revision 2.2  1995/06/09  00:36:36  rlfs
 * CSCdi35552:  Memory leak with ATM RFC1577
 *
 * Revision 2.1  1995/06/07  20:11:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "../if/arp.h"
#include "../ip/ip_registry.h"
#include "../if/network.h"
#include "../if/atm_debug_flags.h"
#include "../if/atm.h"
#include "../if/static_map.h"
#include "../atm/atmsig_api.h"
#include "../atm/atmsig_private.h"
#include "../atm/atmsig_smap.h"
#include "../atm/atm_arp.h"
#include "ilmi_public.h"
#include "../atm/atm_registry.h"
#include "mgd_timers.h"


/* Max. size of Dynamic MAPLIST name */
#define	MAPL_NAME	40

queuetype atm_arp_packetQ;       /* (In)ARP packets */
static	boolean	atm_arp_enabled = FALSE;

static mgd_timer master_timer;
static mgd_timer master_arp_timer;

/* forward declarations */
static void arpserv_nuke_client(idbtype *, atm_arpserv_entry *);
static void atm_arp_sendreq(paktype *, ipaddrtype, idbtype *, hwaddrtype *);
static staticmap_type *atm_arp_build_an_arpserv_map(maplist_type *, atm_arpserv_entry *, idbtype *);
static void atm_arp_reset(hwidbtype *);

/*
 * atm_arp_enqueue_packet
 *
 * Place the input packet on queue atm_arp_packetQ
 *
 */
static boolean
atm_arp_enqueue_packet(paktype *pak)
{
     if (atm_arp_enabled) {
    	 p_enqueue(&atm_arp_packetQ, pak);
     } else {
         datagram_done(pak);
     }
     return(TRUE);
}

/*
 *  atm_arp_build_mapl_name
 *
 *  Fill an array with the name of the ATM ARP map list for the passed
 *  interface.  It is the caller's responsibility to ensure that the passed
 *  array is large enough to hold the map list name.
 */
static inline void
atm_arp_build_mapl_name (idbtype *idb, char *mapl_name)
{
    sprintf(mapl_name, "%s_ATM_ARP", idb->namestring);
}
    
/*
 * atm_build_arp:
 *      Called to finish the ARP packet. This routine assumes the
 *      various length feilds have been properly set. The destination
 *      address, in the packet, is moved to the source address field.
 *      The supplied destination address is used.
 */
static void
atm_build_arp (paktype *pak, addrtype *destaddr, uchar *dest_atm_addr)
{
    uchar *sht, *sp, *tht, *tp;
    idbtype *idb;
    atm_rfc_arp *rfc;
 
    idb = pak->if_output;
    rfc = (atm_rfc_arp *)pak->datagramstart;
 
    /* Source ATM address. Zero length if not configured. */
    sht = rfc->vl_start;
    if (idb->nsap && idb->atm_nsap_valid) {
        bcopy(idb->nsap->addr, sht, idb->nsap->length);
        rfc->arshtl = (ATM_NSAPA_ADDR | idb->nsap->length);
    } else {
        rfc->arshtl = 0;
    }
 
    /* Source ATM subaddress is always zero length. */
    rfc->arsstl = 0;
 
    /* Source protocol address. */
    sp  = sht + rfc->arshtl + rfc->arsstl;
    rfc->arspln = ADDRLEN_IP;
    bcopy(&idb->ip_address, sp, rfc->arspln);
 
    /* Target ATM addres as was included in the request. */
    tht = sp + rfc->arspln;
    if ((rfc->arthtl & ~ATM_NSAPA_ADDR) > 0) {
        bcopy(dest_atm_addr, tht, rfc->arthtl & ~ATM_NSAPA_ADDR);
        rfc->arthtl = (ATM_NSAPA_ADDR | rfc->arthtl);
    }
 
    /* Target ATM subaddress is always zero length. */
    rfc->artstl = 0;
 
    /* Target protocol address. */
    tp = tht + rfc->arthtl + rfc->artstl;
    if (destaddr) {
        rfc->artpln = ADDRLEN_IP;
        bcopy(&destaddr->addr.ip_address, tp, rfc->artpln);
    }
 
    /* Adjust the packet size for the variable-length fields */
    pak->datagramsize += rfc->arspln + rfc->arshtl + rfc->arsstl
        + rfc->artpln + rfc->arthtl + rfc->artstl;
}

/*
 * Returns a MAP_LIST ptr. for the corresponding idb to use for the
 * DYNAMIC mapping stuff.
 */
static maplist_type *
atm_arp_getmapl (idbtype *idb)
{
    char mapl_name[MAPL_NAME];

    atm_arp_build_mapl_name(idb, mapl_name);
    return(maplist_lookup(mapl_name));
}

/*
 * atm_arp_stampvc:
 *	Called to timestamp a VC and give it another 15 mins. to live.
 */
static void
atm_arp_stampvc (vc_info_t *vc_info)
{
    if(!vc_info || !vc_info->inarp) {
	return;
    }
    mgd_timer_stop(&vc_info->arp_timer);
    mgd_timer_start(&vc_info->arp_timer, vc_info->inarp);
}

/*
 * atm_update_arp_mapl: Called to update the ARP Maplist entry with a
 *	new time stamp.
 */
static void
atm_update_arp_mapl (atm_arp_canon_t *canon, paktype *pak, vc_info_t *vc_info)
{
    atmSvcType *found_svc;
    staticmap_type *map;
    vcd_t vc;

    map = staticmap_lookup(pak->if_input, NULL, LINK_ARP, ET_ATM, FALSE);
    if (!map) {
	return;
    }

    vc = *(vcd_t *)map->hwaddr.addr;
    if (vc != vc_info->vc) {
	/*
	 * This is almost certainly a response from a connection to ourselves
	 * on the VC we are not using as the transmit vc.  So just ignore it.
	 */
	return;
    }
    if (!snpa_equ(&map->hwconf, &canon->s_aaddr)) {
	if (atm_arp_debug) {
	    buginf("\nATMARP(%s): New info from VCD#%d %i replacing NSAP",
		   pak->if_input->namestring, vc, map->addr.ip_addr);
	}
	/*
	 * ARP NSAP doesn't match MAP-LIST NSAP for the network address.
	 * In this case, we're forced to close down the 'known' 
	 * connection and re-establish it later.
	 */
	found_svc = atmSig_findSvcBlockByVC(map->sm_swidb,
					    *(vcd_t *)map->hwaddr.addr);
	if (found_svc)
	    atmSmap_closeSvcConnection(found_svc); 
    } else {
	GET_TIMESTAMP(map->update_time);
    }
}

/*
 * atm_update_mapl: Update a remote client MAP entry.  Refresh.
 */
static void
atm_update_mapl (maplist_type *mapl,
		 staticmap_type *map,
		 atm_arp_canon_t *canon,
		 paktype *pak)
{
    atmSvcType *found_svc;

    if (!snpa_equ(&map->hwconf, &canon->s_aaddr)) {
    	/*
    	 * ARP NSAP doesn't match MAP-LIST NSAP for the network address.
    	 * In this case, we're forced to close down the 'known' 
    	 * connection and re-establish it later.  Unless what we have
	 * is a PVC map.  Then we just populate the ARP table with it
	 * and throw out the new ARP info.  Kinda kludgy, I know.
    	 */
	if (map->hwconf.type == STATION_ATMVC) {
	    /*
	     *  Yeah, it's a PVC all right.  Just update the ARP table
	     */
	    if (atm_arp_debug) {
		buginf("\nATMARP(%s): adding pvc to %i to the ARP table",
		       pak->if_input->namestring, map->addr.ip_addr);
	    }
	    arp_table_add(canon->idb, (long)canon->s_naddr.addr.ip_address,
			  LINK_IP, &map->hwconf, ET_ATM, ARP_DYNAMIC);
	    return;
	}

	if (atm_arp_debug) {
	    buginf("\nATMARP(%s): ARP VCD#%d %i replacing NSAP",
		   pak->if_input->namestring, 
		   *(vcd_t *)map->hwaddr.addr,
		   map->addr.ip_addr);
	}
    	found_svc = atmSig_findSvcBlockByVC(map->sm_swidb,
					    *(vcd_t *)map->hwaddr.addr);
    	if (found_svc)
	     atmSmap_closeSvcConnection(found_svc); 
    	/*
    	 * We have a MAP entry.  Update the H/W address in it and force
    	 * the 'update_time' to be stamped.
    	 */
    	staticmap_replace_hw(mapl, map, &canon->s_aaddr);
    } else {
	if (atm_arp_debug) {
	    buginf("\nATMARP(%s): ARP Update from VCD#%d %i MAP VCD#%d",
		   pak->if_input->namestring, 
		   *(vcd_t *)canon->vcd.addr,
		   map->addr.ip_addr,
		   *(vcd_t *)map->hwaddr.addr);
	}
	GET_TIMESTAMP(map->update_time);
    }
    /*
     * If the connection is already up, update the ARP table
     */
    if (map->hwaddr.type != STATION_ILLEGAL) {
   	arp_table_add(canon->idb, (long)canon->s_naddr.addr.ip_address, 
		LINK_IP, &map->hwaddr, ET_ATM, ARP_DYNAMIC);
	reg_invoke_ipcache_invalidate_range((long)canon->s_naddr.addr.ip_address,
		MASK_HOST);
	return;
    }
}

/*
 * atm_send_inarp_req
 * Send an inverse ARP request
 */
void
atm_send_inarp_req (
    vc_info_t *vc_info,
    uint link,
    paktype *pak)
{
    atm_rfc_arp *rfc;
    atm_db_t *atm;
    idbtype *idb;
    vcd_t vcnum;

    if (vc_info == NULL) {
	/* This should *NEVER* happen */
	if (atm_errors_debug) {
	    buginf("\nATM InARP:  Caller passed a NULL vc_info ptr.");
	}
	if (pak) datagram_done(pak);
	return;
    }
    
    idb = vc_info->swidb;
    vcnum = vc_info->vc;
    atm = idb->hwptr->atm_db;
    if (atm_arp_debug) {
	buginf("\nATMARP(%s)O: INARP_REQ to VCD#%d for link %d(%s)",
	       idb->namestring, vcnum, link,link2name(link));
    }

    if(!pak) {
    	pak = getbuffer(ATM_ARP_MAX_SIZE);
    	if ( !pak ) {
	    return;
	}
    	pak->datagramstart = atm_rfc_arp_headstart(pak);
    	pak->if_output = idb;
    	pak->datagramsize = 0;
    	memset(pak->datagramstart, 0, ATM_ARP_MAX_SIZE); /* Clean slate */
    }

    switch (link) {
    case LINK_IP:
	rfc = (atm_rfc_arp *)atm_rfc_arp_headstart(pak);
	PUTSHORT(rfc->arhrd, ATM_ARHRD);
	PUTSHORT(rfc->arpro, PR_IP);
	rfc->arshtl = rfc->arthtl = 0;
	rfc->artpln = rfc->arspln = PL_IP;
	PUTSHORT(rfc->opcode, ATM_INARP_REQ);
	pak->datagramsize += ATM_ARP_HDRBYTES;
	break;
    default:
	datagram_done(pak);
	return;
    }

    atm_build_arp(pak, NULL, NULL);

    pak->linktype = LINK_ARP; 
    pak->enctype = ET_ATM;
    pak->acct_proto = ACCT_PROTO_ARP;
    atm_cpcs_encap(pak, atm, vc_info, zerosnap, NULL, 0);
    atm_show_packet(pak->if_output->hwptr, pak, TRUE);
    datagram_out(pak);
}

/*
 * atm_arp_build_maps
 *
 * Build everything we need to return a static map entry for the passed ATM ARP
 * Server, or all our ATM ARP Servers if we get passed a NULL.
 *
 * We return a pointer to the map for the passed ARP Server, or a NULL if we
 * were passed a NULL for the ARP Server.
 */
staticmap_type *
atm_arp_build_maps (idbtype *swidb, atm_arpserv_entry *arpserv)
{
    maplist_type *mapl;
    char mapl_name[MAPL_NAME];

    /*
     *  Are we running 1577 at all?
     */
    if (swidb->atm_arp == NULL) {
	/* nope.  no NSAP to give a new map, so don't create one */
	return(NULL);
    }
    
    /*
     *  If the map group has not yet been created, go create it.
     */
    atm_arp_build_mapl_name(swidb, mapl_name);
    if (mapgroup_lookup(swidb, mapl_name) == NULL)
	mapgroup_create(mapl_name, mapl_name, swidb);

    mapl = maplist_lookup(mapl_name);
    if (mapl == NULL)
	return(NULL);

    /*
     *  Do we already have a map to our target NSAP?
     */
    if (arpserv == NULL) {
	FOR_ALL_ARP_SERVERS(swidb->atm_arp, arpserv)
	    atm_arp_build_an_arpserv_map(mapl, arpserv, swidb);
	return NULL;
    } else
	return atm_arp_build_an_arpserv_map(mapl, arpserv, swidb);
}

/*
 * atm_arp_build_an_arpserv_map
 *
 * build a map to a specific arp server
 */
static staticmap_type *
atm_arp_build_an_arpserv_map (maplist_type	*mapl,
			      atm_arpserv_entry	*arpserv,
			      idbtype		*swidb)
{
    staticmap_type *map;
    hwaddrtype *nsap;
    atm_params_type *params;

    nsap = &arpserv->nsap;
    map = staticmap_hwlookup_list(mapl, nsap, ET_ATM, TRUE, LINK_ARP);
    if (map != NULL)
	return map;

    /*
     *  Build a new map
     */
    params = malloc_named(sizeof(atm_params_type),"ATM-ARP");
    if (!params)
    	return(NULL);
    atm_params_init(params);
    params->broadcast = FALSE;
    map = staticmap_enter(mapl, NULL, &arpserv->nsap, LINK_ARP, ET_ATM,
			  MAP_DYNAMIC, NULL, params);
    if (map != NULL)
	map->sm_swidb = swidb;
    return map;
}

/*
 * atm_arp_build_map
 *
 * Build a map entry for a specific IP to NSAP mapping
 */
static staticmap_type *
atm_arp_build_a_map (maplist_type *mapl,
		     addrtype *prot_addr,
		     hwaddrtype *atm_addr,
		     idbtype *idb)
{
    atm_params_type *atm_parms;
    staticmap_type *result;

    atm_parms = malloc_named(sizeof(atm_params_type),"ATM-ARP");
    if (!atm_parms) {
	return NULL;
    }
    atm_params_init(atm_parms);
    atm_parms->broadcast = TRUE;	/* Always for these */

    result = staticmap_enter(mapl, prot_addr, atm_addr, 
			     LINK_IP, ET_ATM, MAP_DYNAMIC, NULL, atm_parms);
    if (result == NULL) {
        free(atm_parms);
        return(NULL);
    }

    result->sm_swidb = idb;
    return(result);
}

/*
 * atm_arp_call_srvr:	Called from within, to establsh a connection to
 *	the ATM ARP server.
 * 2 big cases:
 * 	A: We have a STATIC-MAP for the ARP server
 *		If there is no connection, we open it.
 *		If there's ~1 min. before expiration, in InARP the server
 *		If the entry expired, we close the connection.
 *	B: We have no STATIC-MAP for the ARP server
 *		Create a map-group and map-list for the server
 *		Open a connection to the ARP server.
 */
static void
atm_arp_call_srvr (idbtype 		*swidb,
		   staticmap_type 	*smap,
		   atm_arpserv_entry	*arpserv)
{
    staticmap_type *map;

    /*
     * If we have a MAP entry, check to see if the VCD is up.  If not,
     * Bring it up and exchange ARPs again.
     */
    if (!swidb || !interface_up_simple(swidb))
	return;
    
    /* Do not try to register yourself if you don't have an address */
    if (!swidb->nsap || !swidb->ip_address)
	return;

    if (smap) {
	if (smap->hwaddr.type == STATION_ILLEGAL) {
    	    (void)atmSmap_openSvcConnection(swidb, smap, VC_FLAG_NOIDLETIME);
	} else {
	    vc_info_t *vc_info;
	    uint time_left;
	    sys_timestamp expiration_time;

	    vc_info = atm_getvc_inline(swidb->hwptr->atm_db, 
				       *(vcd_t *)smap->hwaddr.addr);
	    if (!vc_info) {
    	    	(void)atmSmap_openSvcConnection(swidb,smap,VC_FLAG_NOIDLETIME);
		return;
	    }
	    expiration_time = TIMER_ADD_DELTA(smap->update_time,
					      swidb->arp_timeout);
	    time_left = TIME_LEFT_SLEEPING(expiration_time);
	    if (time_left <= ONEMIN) {
	    	atm_send_inarp_req(vc_info, LINK_IP, NULL);
	    }
	}
	return;
    }

    /*
     *  Build all our map lists, if we haven't already.  Actually, we will have
     *  always built our maps by now, unless there was a malloc error or
     *  something.
     */
    map = atm_arp_build_maps(swidb, arpserv);
    if (map == NULL)
	return;

    /* 
     * We need a connection to this ARP server.
     */
    if (atm_arp_debug) {
	char nsapstr[MAX_ATM_NSAP_ASCII_LEN];

    	atm_printnsap(arpserv->nsap.addr, nsapstr);
    	buginf("\nATMARP(%s): Opening ARP server VCC to %s",
	       swidb->namestring, nsapstr);
    }

    (void)atmSmap_openSvcConnection(swidb, map, VC_FLAG_NOIDLETIME);
}

/*
 * atm_arp_onesec
 *
 * per-hwinterface once-a-second periodic handling
 */
static void
atm_arp_onesec (hwidbtype *hwidb)
{
    if (hwidb->atm_db == NULL)
	return;			/* can never happen */
    if (hwidb->atm_db->flags & ATM_FLAG_ARP_RESET)
	atm_arp_reset(hwidb);
}

/*
 * atm_arp_periodic()
 *	Handle periodic functions for the VCCs.  In particular, send out
 *	periodic InARPs if necessary.
 *	Check to see if we have an ARP server connection and it's up.
 *	Called once per minute to perform it's duties.
 */
static void
atm_arp_periodic (void)
{
    uint ndx1, ndx2;
    atm_db_t *atm;
    idbtype *swidb;
    mgd_timer *expired_timer;
    sys_timestamp expiration_time;

    /*
     *	Aging of ARP table stuff.  RFC1577 says that the client *MUST*
     *	age out all InARP entries after 15 minutes.  Before aging them
     *  out, we *HAVE* to send out an InARP request.  If we don't see the
     *  reply, the entry *MUST* be aged out of the ARP table.
     */
    while (mgd_timer_expired(&master_timer)) {
	vc_info_t *vc;
	
	expired_timer = mgd_timer_first_expired(&master_timer);
	vc = mgd_timer_context(expired_timer);
	if (!vc) {
	    continue;
	}
	mgd_timer_stop(&vc->arp_timer);
	/*
	 * If PVC ARP ages, we resend an InARP.  We should get a 
	 * reply in 1 min.  If not, we delete this entry from the
	 * ARP table.
	 */
	atm_send_inarp_req(vc, LINK_IP, NULL);
    	mgd_timer_start(&vc->arp_timer, vc->inarp);
    }

    /*
     * Weed thru the ATM descriptor blocks and find the ones that are
     * actually being used.  Much faster going this route than running
     * through all the HWIDBs.  Also makes this module a more stand-alone
     * system.
     */
    for(ndx1 = 0; ndx1 < MAX_ATM_INTFC; ndx1++) {
	atm = &atm_db[ndx1];
	if (!(atm->flags & ATM_FLAG_DB_INUSE)) {
	    continue;		/* Skip those not in use */
	}
	if (!atm_interface_up(atm->hwidb->firstsw)) {
	    continue;		/* Skip those that are shutdown */
	}

	/*
	 * Got through each map-list and check.  If the connection
	 * is closed, delete the map entry.  If the entry is over ARP
	 * timeout minutes, then close the connection.  
	 *
	 */
	FOR_ALL_SWIDBS_ON_HW(atm->hwidb, swidb) {
	    maplist_type *map_l;
	    addrtype addr;
	    staticmap_type *map;

	    /*
	     * Do not bring up svc on a subinterface
	     * if it's shut down.
	     */
	    if (!interface_up_simple(swidb))
		continue;

	    memset(&addr, 0, sizeof(addrtype));
	    if(is_atm_arp_client(swidb)) {
		map_l = atm_arp_getmapl(swidb);

		/*
		 * Now check to see if ARP map entries need to be cleaned
		 */
		if (!map_l) {
		    atm_arpserv_entry *entry;
		    
		    /*
		     * OOPS! No map list.  Must set it up & call
		     * ARP servers.
		     */
		    FOR_ALL_ARP_SERVERS(swidb->atm_arp, entry)
			atm_arp_call_srvr(swidb, NULL, entry);
		    continue;
		}
		for (ndx2 = 0; ndx2 < STATIC_MAP_HASH; ndx2++) {
		    uint time_left;

		    map = map_l->hash[ndx2];
		    while (map) {
			/*
			 * All LINK_ARP maps point to an ARP server.
			 * We handle those separately.
			 */
			if (map->protocol == LINK_ARP) {
			    atm_arpserv_entry *entry;
			    
			    entry = locate_arpserv_entry(swidb, &map->hwaddr);
	    	    	    atm_arp_call_srvr(swidb, map, entry);
			    map = map->next;
			    continue;
			}
			/*
			 * If ARP time expired, close the connection and keep
			 * the static map entry.  If no arp in 1 min., delete
			 * the map entry.
			 */
			expiration_time = TIMER_ADD_DELTA(map->update_time, 
							  swidb->arp_timeout);
	    		time_left = TIME_LEFT_SLEEPING(expiration_time);
			if (time_left == 0) {
    			    atmSvcType *found_svc;
			    staticmap_type *newmap = map->next;

			    if (map->hwaddr.type != STATION_ILLEGAL) {
				found_svc = atmSig_findSvcBlockByVC(map->sm_swidb, *(vcd_t *)map->hwaddr.addr);
	    			if (found_svc)
	  	    		    atmSmap_closeSvcConnection(found_svc); 
			    } else {
			        /*
			         * If no connection, then delete this map entry
			         */
			        staticmap_delete(map_l, map);
			    }
			    map = newmap;
			    continue;
	    		} else {
			    /*
			     * Time is left.  Do simple housekeeping.
			     * Update ARP table if it was flushes somehow.
			     */
			    arptype *arp;

			    if (map->hwaddr.type != STATION_ILLEGAL) {
			    	arp = arp_table_lookup(map->protocol,
				    (long)map->addr.addr.ip_address, swidb);
			    	if (!arp) {
				   arp_table_add(swidb, 
					    (long)map->addr.addr.ip_address,
					    map->protocol, &map->hwaddr, 
					    ET_ATM, ARP_DYNAMIC);
			    	} else {
				    if (map->hwaddr.type != arp->hwtype) {
				    	arp_table_add(swidb, 
					    (long)map->addr.addr.ip_address,
					    map->protocol, &map->hwaddr, 
					    ET_ATM, ARP_DYNAMIC);
				    	reg_invoke_ipcache_invalidate_range(
					    (long)map->addr.addr.ip_address, 
					    MASK_HOST);
				    }
			    	}
			    }
			}
			
			map = map->next;
		    }
		}
	    }
	}
    }

}

/*
 * atm_arp_canon:	Canonicalize the ARP packet into some
 *	generically useable thing for other functions
 */
static void
atm_arp_canon (paktype *pak, idbtype *idb, atm_arp_canon_t *canon)
{
    atm_rfc_arp *rfc;
    uchar *sht, *sst, *sp, *tht, *tst, *tp;

    rfc = (atm_rfc_arp *)pak->network_start;
    sht = rfc->vl_start;
    sst = sht + ATM_ADDRLEN(rfc->arshtl);	/* ATM Source Sub-Address */
    sp = sst + ATM_ADDRLEN(rfc->arsstl);/* ATM Source Network Address */
    tht = sp + rfc->arspln;	/* ATM Destinatiion Address */
    tst = tht + ATM_ADDRLEN(rfc->arthtl);/* ATM Destination Sub-Address */
    tp = tst + ATM_ADDRLEN(rfc->artstl);/* ATM Destination Network Address */

    memset(canon, 0, sizeof(atm_arp_canon_t));
    canon->idb = idb;
    canon->s_naddr.type = canon->d_naddr.type = ADDR_IP; /* Always IP */
    canon->s_naddr.length = rfc->arspln;
    canon->d_naddr.length = rfc->artpln;
    canon->s_naddr.addr.ip_address = GETLONG(sp);
    canon->d_naddr.addr.ip_address = GETLONG(tp);
    canon->s_aaddr.type = canon->d_aaddr.type = STATION_ATMNSAP;
    canon->s_aaddr.length = ATM_ADDRLEN(rfc->arshtl);
    bcopy(sht, canon->s_aaddr.addr, rfc->arshtl);
    canon->d_aaddr.length = ATM_ADDRLEN(rfc->arthtl);
    bcopy(sht, canon->d_aaddr.addr, rfc->arthtl);

    canon->vcd.type = STATION_ATMVC;
    canon->vcd.length = STATIONLEN_ATMVC;
    bcopy(pak->mac_start, canon->vcd.addr, canon->vcd.length);
}

/*
 * atm_arp_rcvdreq:	Received an ARP request
 */
static void
atm_arp_rcvdreq(paktype *pak, vc_info_t *vc_info)
{
    atm_arp_canon_t canon;
    maplist_type *mapl;
    staticmap_type *map;
    char nsapstr[MAX_ATM_NSAP_ASCII_LEN];

    atm_arp_canon(pak, pak->if_input, &canon);
    atm_printnsap(canon.s_aaddr.addr, nsapstr);

    if (atm_arp_debug) {
	buginf("\nATMARP:(%s): ARP Request from %i -> %s", 
		pak->if_input->namestring, canon.s_naddr.addr.ip_address,
		nsapstr);
    }
    mapl = atm_arp_getmapl(pak->if_input);
    if (!mapl) {
	if(atm_errors_debug || atm_arp_debug) {
	    buginf("\nATMARP(%s): ARP request from %i, MAP-LIST is missing",
		pak->if_input->namestring, canon.s_naddr.addr.ip_address);
	}
	return;
    }
    /*
     * Ok... We have the reply and now we have the appropriate map-list.
     * Plug this entry into the map-list then initiate a connection to
     * this host
     */
    map = staticmap_lookup(pak->if_input, &canon.s_naddr, LINK_IP, 
	ET_ATM, FALSE);
    if (map) {
	atm_update_mapl(mapl, map, &canon, pak);
    }
}

/*
 * atm_arp_rcvdreply:	Received an ARP reply from either a remote 
 *	Connection, or the ARP server.  Either way, we need to update
 *	our tables.  If from the ARP server, we'll need to open a 
 *	connection to the target NSAP for the user.
 */
static void
atm_arp_rcvdreply (paktype *pak)
{
    atm_arp_canon_t canon;
    atm_db_t *atm;
    char mapl_name[MAPL_NAME];
    maplist_type *mapl;
    staticmap_type *map;
    char nsapstr[MAX_ATM_NSAP_ASCII_LEN];

    if (!interface_up(pak->if_input))
	return;
    
    atm_arp_canon(pak, pak->if_input, &canon);
    atm_printnsap(canon.s_aaddr.addr, nsapstr);

    if (atm_arp_debug) {
	buginf("\nATMARP:(%s): ARP reply from %i -> %s", 
		pak->if_input->namestring, canon.s_naddr.addr.ip_address,
		nsapstr);
    }
    atm = pak->if_input->hwptr->atm_db;
    mapl = atm_arp_getmapl(pak->if_input);
    if (!mapl) {
	if(atm_errors_debug || atm_arp_debug) {
	    buginf("\nATMARP(%s): ARP reply from %i, MAP-LIST %s is missing",
		pak->if_input->namestring, 
		canon.s_naddr.addr.ip_address, mapl_name);
	}
	return;
    }
    /*
     * Ok... We have the reply and now we have the appropriate map-list.
     * Plug this entry into the map-list then initiate a connection to
     * this host
     */
    map = staticmap_lookup(pak->if_input, &canon.s_naddr,LINK_IP,ET_ATM,FALSE);
    if (map) {
	atm_update_mapl(mapl, map, &canon, pak);
	return;
    }

    /*
     * We didn't have a map.  Build one.
     */
    map = atm_arp_build_a_map(mapl, &canon.s_naddr, &canon.s_aaddr,
			      pak->if_input);
    if (!map) {
    	if(atm_errors_debug || atm_arp_debug) {
	    buginf("\nATMARP(%s): ARP reply from %i, MAP-LIST %s enter failed",
		    pak->if_input->namestring, 
		    canon.s_naddr.addr.ip_address, mapl_name);
    	}
	return;
    }

    if (atm_arp_debug) {
	buginf("\nATMARP(%s): Opening VCC to %s",
	       pak->if_input->namestring, nsapstr);
    }

    (void)atmSmap_openSvcConnection(pak->if_input, map, 0);

}

/*
 * atm_inarp_rcvdreq:	Received an InARP Request from the other side
 *	Update our ARP information and reply to the InARP
 */
static void
atm_inarp_rcvdreq (paktype *pak, vc_info_t *vc_info)
{
    atm_arp_canon_t canon;
    atm_db_t *atm;
    atm_rfc_arp *atm_rfc;
    paktype *newpak;

    atm_arp_canon(pak, pak->if_input, &canon);
    if (atm_arp_debug) {
	buginf("\nATMARP(%s)I: INARP Request VCD#%d from %i",
		pak->if_input->namestring,
		*(vcd_t *)canon.vcd.addr, canon.s_naddr.ip_addr);
    }

    newpak = pak_duplicate(pak);
    if (!newpak) {
	return;
    }
    atm = newpak->if_input->hwptr->atm_db;

    /*
     * For INARP PVCs, we update the ARP table.  Otherwise, it's an
     * InARP request from the ARP server.
     */
    if (vc_info->inarp) {
    	arp_table_add(canon.idb, (long)canon.s_naddr.ip_addr, LINK_IP,
		&canon.vcd, ET_ATM, ARP_DYNAMIC);
    	reg_invoke_ipcache_invalidate_range((long)canon.s_naddr.ip_addr, 
		MASK_HOST);
    } else {
	maplist_type *mapl;
	staticmap_type *map;

	/*
	 *  Now update the IP Map list.  This will also populate the
	 *  ARP cache.  The map list should have already been created,
	 *  bug it doesn't hurt to be safe.
	 */
	atm_arp_build_maps(pak->if_input, NULL);
	atm_update_arp_mapl(&canon, pak, vc_info);
	mapl = atm_arp_getmapl(pak->if_input);
	if (!mapl) {
	    if(atm_errors_debug || atm_arp_debug) {
		buginf("\nATMARP(%s): InARP request from %i, "
		       "MAP-LIST is missing",
		       pak->if_input->namestring,
		       canon.s_naddr.addr.ip_address);
	    }
	} else {
	    /*
	     *  Update or create the map for this puppy
	     */
	    map = staticmap_lookup(pak->if_input, &canon.s_naddr, LINK_IP, 
				   ET_ATM, FALSE);
	    if (map != NULL)
		atm_update_mapl(mapl, map, &canon, pak);
	    else
		map = atm_arp_build_a_map(mapl, &canon.s_naddr, &canon.s_aaddr,
					  pak->if_input);
	}
    }

    atm_rfc = (atm_rfc_arp *)newpak->network_start;
    PUTSHORT(atm_rfc->opcode, ATM_INARP_REPLY);
    newpak->if_output = pak->if_input;
    newpak->datagramsize = ATM_ARP_HDRBYTES;
    newpak->datagramstart = newpak->network_start;
    atm_rfc->arthtl = canon.s_aaddr.length;
    atm_build_arp(newpak, &canon.s_naddr, canon.s_aaddr.addr);
    atm_cpcs_encap(newpak, atm, vc_info, zerosnap, NULL, 0);
    if (atm_arp_debug) {
	buginf("\nATMARP(%s)O: INARP Response VCD#%d to %i",
		pak->if_input->namestring,
		*(vcd_t *)canon.vcd.addr, canon.s_naddr.ip_addr);
    }

    atm_show_packet(newpak->if_output->hwptr, newpak, TRUE);
    datagram_out(newpak);
    atm_arp_stampvc(vc_info);
}

/*
 * atm_inarp_rcvdreply:	Remote end replied to our InARP request
 *	Update our ARP information
 *	TIMESTAMP our VC to show a new InARP time & address.
 */
static void
atm_inarp_rcvdreply (paktype *pak, vc_info_t *vc_info)
{
    atm_arp_canon_t canon;

    atm_arp_canon(pak, pak->if_input, &canon);
    if (atm_arp_debug) {
	buginf("\nATMARP(%s)I: INARP Reply VCD#%d from %i",
		pak->if_input->namestring,
		*(vcd_t *)canon.vcd.addr, canon.s_naddr.ip_addr);
    }

    /*
     * For INARP PVCs, we update the ARP table.  Otherwise, it's an
     * InARP reply from the ARP server.
     */
    if (vc_info->inarp) {
    	arp_table_add(canon.idb, (long)canon.s_naddr.addr.ip_address, LINK_IP,
		&canon.vcd, ET_ATM, ARP_DYNAMIC);
    	reg_invoke_ipcache_invalidate_range((long)canon.s_naddr.addr.ip_address,
		MASK_HOST);
    } else {
	atm_update_arp_mapl(&canon, pak, vc_info);
    }
    atm_arp_stampvc(vc_info);
}

/*
 * atm_arp_process
 * 	handle incoming ARP & InARP packets
 *	Update ARP tables as necessary when the replies arrive.
 */
static forktype 
atm_arp_process (void)
{
    hwidbtype *hwidb;
    atm_db_t *atm;
    idbtype *idb;
    paktype *pak;
    atm_rfc_arp *atm_arp;
    vc_info_t *in_vc;
    atm_encap_t *atm_encap;
    
    process_set_priority(PRIO_LOW);
    while (TRUE) {
	edisms(queueBLOCK, (long)&atm_arp_packetQ);
	while (TRUE) {
            pak = p_dequeue(&atm_arp_packetQ);
            if ( !pak )
	    	break;
	    idb = pak->if_input;
            hwidb = idb->hwptr;
            if (!hwidb) {
		if (atm_errors_debug) {
	    	    buginf("\nATM_ARP_PROCESS(): source idb not set");
		}
	    	datagram_done(pak);
	    	continue;
            }
	    atm = hwidb->atm_db;
	    atm_arp = (atm_rfc_arp *)pak->network_start;
	    atm_encap = (atm_encap_t *)pak->datagramstart;
	    in_vc = atm_getvc_inline(atm, atm_encap->vcnum);
	    if (!in_vc) {
		if (atm_errors_debug || atm_arp_debug) {
		    buginf("\nATMARP(%s): Received ARP packet for missing VCD#%d",
			pak->if_input->namestring, atm_encap->vcnum);
		}
		datagram_done(pak);
		continue;
	    }
	    if (GETSHORT(atm_arp->arhrd) != ATM_ARHRD) {
		if (atm_errors_debug || atm_arp_debug)
		    buginf ("\nATMARP(%s): Received non RFC-1577 compliant ARP"
			    " packet on VCD#%d",pak->if_input->namestring,
			    atm_encap->vcnum);
		datagram_done(pak);
		continue;
	    }
	    
	    /*
	     * In each case below, where atmarp_rxpak() is called, the
	     * ATM ARP server is required to dispose of the packet.
	     */
	    switch(GETSHORT(atm_arp->opcode)) {
	    case ATM_ARP_REQ: 
		atm_arp_rcvdreq(pak, in_vc);
		/*
		 * Give this to the ATM ARP Server.  Nothing more to do.
		 */
		atmarp_rxpak(pak, in_vc);
		break;
	    case ATM_ARP_REPLY:
		atm_arp_rcvdreply(pak);
		atmarp_rxpak(pak, in_vc);	
		break;
	    case ATM_INARP_REQ:
		atm_inarp_rcvdreq(pak, in_vc);
		atmarp_rxpak(pak, in_vc);	
		break;
	    case ATM_INARP_REPLY:
		atm_inarp_rcvdreply(pak, in_vc);
		/*
		 * And now give this to the ATM ARP Server.
		 */
		atmarp_rxpak(pak, in_vc);
		break;
	    case ATM_ARP_NAK:
		if (atm_errors_debug || atm_arp_debug) {
		    buginf("\nATMARP(%s): ARP_NAK received on VCD#%d",
		       hwidb->hw_namestring, atm_encap->vcnum);
		}
	    	datagram_done(pak);
		break;
	    default:
		if (atm_errors_debug || atm_arp_debug) {
		    buginf("\nATMARP(%s): Bad ARP code %d received on VCD#%d",
			   hwidb->hw_namestring, GETSHORT(atm_arp->opcode), 
			   atm_encap->vcnum);
		}
	    	datagram_done(pak);
		break;
	    }
	}
    }
}

/*
 * atm_arp_sendreq:
 * Called through the reg_invoke_arp_sendreq() function from the
 * ARP server.  It initializes the ARP packet as necessary for an
 * ATM ARP request, per RFC1577
 * Basically, this is an ARP encapsulation function specific to ATM.
 * NOTE:  Only works for IP... RFC1577 only specifies IP.
 */
static void
atm_arp_sendreq (
    paktype *pak,
    ipaddrtype naddr,
    idbtype *idb,
    hwaddrtype *destaddr)
{
    atm_db_t *atm;
    atm_rfc_arp *rfc;
    atm_arpserv_entry *arpserv;
    vcd_t vcd;
    vc_info_t *arp_vc, *vc_info;
    staticmap_type *map_ptr;
    paktype *duppak;
    addrtype addr;
    boolean free_pak_buffer_needed = TRUE;

    atm = idb->hwptr->atm_db;
    if (atm_arp_debug) {
	buginf("\nATMARP(%s): Sending ARP to %i", 
		idb->namestring, naddr);
    }
    addr.length = ADDRLEN_IP;
    addr.type = ADDR_IP;
    bcopy(&naddr, &addr.addr, addr.length);

    vc_info = NULL;
    /*
     * Ok... If we have a VCD to ARP, then make sure it still
     * exists.  If we don't have a VCD to ARP, then make sure we have
     * an ARP server to ARP.  If not, we drop this ARP request on the
     * floor.
     */
    if (destaddr && destaddr->type == STATION_ATMVC) {
	bcopy(destaddr->addr, &vcd, destaddr->length);
    	vc_info = atm_getvc(atm, vcd);
    	if (!vc_info) {
	    datagram_done(pak);
	    return;
    	}
    }

    pak->datagramstart = atm_rfc_arp_headstart(pak);
    pak->network_start = pak->datagramstart;
    pak->if_output = idb;
    pak->datagramsize = 0;
    pak->linktype = LINK_IP;

    if (destaddr && !IS_ATMVC_SVC(vc_info)) {
	/*
	 * For PVCs, we InARP the VCD and make sure we have the
	 * same IP address on the remote end
	 */
	atm_send_inarp_req(vc_info, pak->linktype, pak);
	return;
    }

    /*
     * Look to see if we already have map entry.  If so, we send an 
     * InARP to the connection for a quick refresh.
     */
    map_ptr = staticmap_lookup(idb, &addr, LINK_ARP, ET_ATM, FALSE);
    if (map_ptr) {
	if (map_ptr->hwaddr.type != STATION_ILLEGAL) {
	    vc_info = atm_getvc_inline(atm, *(vcd_t *)map_ptr->hwaddr.addr);
	    if (vc_info) {
		atm_send_inarp_req(vc_info, pak->linktype, pak);
		return;
	    }
	}
    }

    /*
     * check to see that an ARP server is configured
     * If not, dump this ARP
     */
    if (!idb->atm_arp) {
	datagram_done(pak);
	return;
    }

    /*
     *  Build and ARP packet
     */
    rfc = (atm_rfc_arp *)pak->network_start;
    switch (pak->linktype) {
    case LINK_IP:
	PUTSHORT(rfc->arhrd, ATM_ARHRD);
	PUTSHORT(rfc->opcode, ATM_ARP_REQ);
	rfc->artpln = rfc->arspln = PL_IP;
	PUTSHORT(rfc->arpro, PR_IP);
	rfc->arshtl = (ATM_NSAPA_ADDR | STATIONLEN_ATMNSAP);
	rfc->arthtl = 0;		/* Assume no target NSAP yet. */
	if(destaddr)
	    rfc->arthtl = (ATM_NSAPA_ADDR | STATIONLEN_ATMNSAP);
	rfc->arsstl = rfc->artstl = 0;	/* No ATM Subaddress */
	pak->datagramsize += ATM_ARP_HDRBYTES;
	break;
    default:
	datagram_done(pak);
	return;
    }

    atm_build_arp(pak, &addr, destaddr->addr);

    pak->linktype = LINK_ARP; 
    pak->enctype = ET_ATM;
    pak->acct_proto = ACCT_PROTO_ARP;

    /*
     *  Now blast our packet out to all the arp servers
     */
    FOR_ALL_ARP_SERVERS(idb->atm_arp, arpserv) {
	map_ptr = staticmap_hwlookup(idb, &arpserv->nsap, ET_ATM, TRUE,
				     LINK_ARP);
	if (!map_ptr) {
	    atm_arp_call_srvr(idb, NULL, arpserv);
	    continue;
	}

	arp_vc = atm_getvc_inline(atm, *(vcd_t *)map_ptr->hwaddr.addr);
	if (!arp_vc) {
	    atm_arp_call_srvr(idb, map_ptr, arpserv);
	    continue;
	}

	/*
	 *  We must send a copy of our packet to all but the last ARP
	 *  Server.
	 */
	if (arpserv->next != NULL) {
	    duppak = pak_duplicate(pak);
	    if (duppak == NULL)
		continue;
	} else {
	    duppak = pak;	/* fake out a duplicate */
	    /*
	     * indicate that we have used the pak struct else we will
	     * free it
	     */
	    free_pak_buffer_needed = FALSE;
	}
	atm_cpcs_encap(duppak, atm, arp_vc, zerosnap, NULL,0);
	atm_show_packet(duppak->if_output->hwptr, duppak, TRUE);
	datagram_out(duppak);
	atm_arp_stampvc(arp_vc);
    }
    /*
     *  due to a vc not being up or a map not existing we haven't used
     *  the pac struct so do now to avoid memory leak
     */  
    if (free_pak_buffer_needed) {
	datagram_done(pak);
    }
}

/*
 * remove_arp_maps: 	* removes all ATM_ARP map groups for the SWIDB.
 */
static void
remove_arp_maps (idbtype *idb)
{
    char mapl_name[MAPL_NAME];
    mapgroup_type *mapg;

    atm_arp_build_mapl_name(idb, mapl_name);
    mapg = mapgroup_lookup(idb, mapl_name);
    if (mapg) {
        /*
         *  We will delete the existing map group, and create a fresh
         *  new one for us to use.
         */
        mapgroup_delete(idb, mapg);
	arp_flush_cache(idb);
        atm_arp_build_maps(idb, NULL);
    }
}

/*
 * atm_arp_reset:	Called when an interface is RESET, or put into
 *	SHUTDOWN.  Cleanup is required to flush out stale data
 *	Delete all ATM_ARP map groups for all SWIDBS on a given HWIDB.
 */
static void
atm_arp_reset (hwidbtype *hwidb)
{
    idbtype *idb;

    if (hwidb->atm_db == NULL)
	return;		/* can never happen */

    /*
     *  Doing this requires malloc/free.  If we are in an interrupt routine,
     *  we can't do it now, so we'll just flag process level.  Note that we
     *  are avoiding a critical region by clearing the arp reset flag before
     *  doing any work.
     */
    if (onintstack()) {
	hwidb->atm_db->flags |= ATM_FLAG_ARP_RESET;
	return;
    }
    hwidb->atm_db->flags &= ~ATM_FLAG_ARP_RESET;

    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
        remove_arp_maps(idb);
    }
}

/*      
 * remove_atm_arp_subif:  Do arp server and client cleanup on this 
 *                        interface. 
 */     
static void
remove_atm_arp_subif (idbtype *idb)
{       
    hwidbtype *hwidb;
    atm_db_t *atm;

    /* Verify this idb is running atm arp */  
    if (! is_atm_arp(idb)) {
        return;
    }

    hwidb = idb->hwptr;
    if (!is_atm(hwidb)) {
        return;
    } 
    atm = hwidb->atm_db;
    if (atm == NULL) {
       return;
    }

    /* Turn off arp server on this sub-int */
    if (is_atm_arp_server(idb)) {
        atm_arp_serv_off(idb);
    } else {
        atm_arp_client_off(idb, 
        &idb->atm_arp->as_data.client.arp_serv_list->nsap);
      }
}
/*
 * atm_inarp_enable_timer:	Enable managed timer for this VCC
 */
void
atm_inarp_enable_timer (vc_info_t *vc_info)
{
    if (vc_info && vc_info->inarp && !vc_info->inarp_timer_running) {
	
    	mgd_timer_init_leaf(&vc_info->arp_timer, &master_arp_timer,
			0, vc_info, TRUE);
	/*
	 * Force first timer in 1 second, to make the first INARP go out
	 * at the next periodic interval. 
	 */
    	mgd_timer_start(&vc_info->arp_timer, ONESEC);
	vc_info->inarp_timer_running = TRUE;
    }
}

/* 
 * atm_inarp_disable_timer:	Disable managed timer for this VCC
 */
void
atm_inarp_disable_timer(vc_info_t *vc_info)
{
    if (vc_info && vc_info->inarp && vc_info->inarp_timer_running) {
    	mgd_timer_stop(&vc_info->arp_timer);
	vc_info->inarp_timer_running = FALSE;
    }
}

/*
 * atm_arp_service_addr_change
 *
 * If an address(IP/NSAP/ESI) is configured/changed/deleted on the interface, 
 * inform the ARP servers accordingly.
 * 
 * boolean added is TRUE if an IP or ATM address is configured or changed 
 * and FALSE if deleted on the idb
 */
static void atm_arp_service_addr_change (idbtype *idb, boolean added)
{
    atmSvcType *svc;
    vc_info_t *vc_info;
    staticmap_type *smap;
    atm_arpserv_entry *entry;

    if (is_atm_arp_client(idb)) {
	/* 
	 * Act on address change for all ARP servers configured on the client 
	 */
	FOR_ALL_ARP_SERVERS(idb->atm_arp, entry) {
	    smap = staticmap_hwlookup(idb,&entry->nsap,ET_ATM,TRUE,LINK_ARP);
	    if (smap) {
		/* 
		 * If address is changed, and a map exists to the ARP server,
		 * send InARP_REQ to the ARP server to update its tables if
		 * there is an open SVC else open a SVC to it.
		 */
		if (added) {
		    vc_info = atm_getvc_inline(idb->hwptr->atm_db,
			*(vcd_t *)smap->hwaddr.addr);
		    if (vc_info)
			atm_send_inarp_req(vc_info, LINK_IP, NULL);
		    else
			atm_arp_call_srvr(idb, NULL, entry);
		} else {
		    /* 
		     * If an address is deleted and there is an open SVC to 
		     * the ARP server, close it.
		     */
		    svc = atmSig_findSvcBlockByVC(idb, *(vcd_t *)smap->hwaddr.addr);
		    if (svc)
			atmSmap_closeSvcConnection(svc);
		}
	    } else {
		/* 
		 * If an address is configured and no map exists, 
		 * setup SVCs to the ARP servers 
		 */
		if (added)
		    atm_arp_call_srvr(idb, NULL, entry);
	    }
	}
    }
}

/*
 * atm_arp_service_ipaddr_change
 *
 * Called everytime an IP address changes on the interface. We do not care
 * if a secondary address changes. We only need to know when a primary IP 
 * address is configured, deleted or changed.
 */
static void atm_arp_service_ipaddr_change (idbtype *idb, ipaddrtype address, 
			ipaddrtype mask, boolean secondary, boolean adding)
{
    if (secondary || !is_atm_arp_client(idb))
	return;

    atm_arp_service_addr_change(idb, adding);
}

/*
 * atm_arp_init
 * Initialize the ARP queue
 */
static void
atm_arp_init (subsystype *subsys)
{
atm_arp_start_func = atm_arp_setup;
}

/*
 * atm_arp_setup
 *
 * Perform actual ATM ARP initialization function here
 */
void
atm_arp_setup(void)
{
    if(!atm_arp_enabled) {
    	queue_init(&atm_arp_packetQ, 40);
	(void) cfork(atm_arp_process, 0L, 0, "ATM ARP Input", CONSOLE_LINENUM);
	reg_add_onemin(atm_arp_periodic, "ATM ARP periodic");
	reg_add_if_onesec(atm_arp_onesec, "ATM ARP one second periodic");
	reg_add_atm_vc_activated(atm_inarp_enable_timer,
				 "ATM inarp timer enabler");
	reg_add_atm_vc_deactivated(atm_inarp_disable_timer,
				   "ATM inarp timer disabler");
	reg_add_atm_vc_removed(atm_inarp_disable_timer,
			       "ATM inarp timer disabler");
	reg_add_if_onesec(atm_arp_serv_periodic,
			  "ATM arp server periodic function");
	reg_add_atm_arp_glommed_onto_vc(atm_arp_serv_glommed_onto_vc,
					"ATM ARP find existing vc to use");
	reg_add_atm_arp_serv_connect_reg(atm_arp_serv_connect,
				   "ATM ARP server call connection function");
	reg_add_atm_arp_serv_connect_ack_reg (atm_arp_serv_connect_ack,
				   "ATM ARP server call connect ack function");
	reg_add_atm_arp_serv_disconnect_reg (atm_arp_serv_disconnect,
				   "ATM_ARP server disconnect function");
	reg_add_arp_sendreq(ET_ATM, atm_arp_sendreq, "ATM ARP Send Request");
	reg_add_atm_addr_change(atm_arp_service_addr_change, 
	    "Service ATM address change");
	reg_add_ip_address_change(atm_arp_service_ipaddr_change, 
	    "Service IP address change");
    	reg_add_media_hw_reset(atm_arp_reset, "atm_arp_reset");
    	reg_add_swif_goingdown(remove_arp_maps, "remove_arp_maps");
    	reg_add_swif_erase(remove_atm_arp_subif, "remove_atm_arp_subif");
	mgd_timer_init_parent(&master_timer, NULL);
    	mgd_timer_init_parent(&master_arp_timer, &master_timer);
	reg_add_atm_arp_enqueue(atm_arp_enqueue_packet, 
				       "atm_arp_enqueue_packet");
	atm_arp_parser_init();
	atm_arp_enabled = TRUE;
    }
}    

/*
 * atm_arp_client_init
 *
 * Start an ATM ARP client running on an interface
 */
void
atm_arp_client_init (idbtype *idb, hwaddrtype *nsap)
{
    if (atm_arp_debug)
    	buginf("\nATMARP: Starting ARP client on %s", idb->namestring);

    /*
     * If we are not already configured we must allocate a new structure.
     */
    if (idb->atm_arp == NULL) {
	idb->atm_arp = malloc_named(sizeof(*idb->atm_arp),"ATM-ARP");
	if (idb->atm_arp == NULL)
	    return;
	idb->atm_arp->arp_flags &= ~ATM_ARP_SERVER;
    }

    atm_arp_client_init_common(idb, nsap);
}

/*
 *  atm_arp_client_init_common
 *
 *  Common initalization for ATM Clients and ATM Servers which also are
 *  running as clients
 */
void
atm_arp_client_init_common (idbtype *idb, hwaddrtype *nsap)
{
    atmSvcType *found_svc;
    staticmap_type *map = NULL;
    atm_arpserv_entry *entry;

    /*
     * First, build a new entry if we don't already have an entry for this
     * guy
     */
    entry = locate_arpserv_entry(idb, nsap);
    if (entry == NULL) {
	entry = malloc_named(sizeof(*entry),"ATM-ARP");
	if (entry == NULL)
	    return;
	entry->nsap = *nsap;	/* Yes, it's a structure copy */
	entry->nsap_valid = TRUE;

	/*
	 *  Now add it to the head of the list
	 */
	if (idb->atm_arp->as_data.client.arp_serv_list == NULL)
	    idb->atm_arp->as_data.client.arp_serv_list = entry;
	else {
	    idb->atm_arp->as_data.client.arp_serv_list->prev = entry;
	    entry->next = idb->atm_arp->as_data.client.arp_serv_list;
	    idb->atm_arp->as_data.client.arp_serv_list = entry;
	}
    }

    /*
     * First check if we already have a connection up.  Have to clean out
     * the old entry before setting up the new one.
     */
    map = staticmap_lookup(idb, NULL, LINK_ARP, ET_ATM, FALSE);
    if (map) {
    	maplist_type *mapl;
	found_svc = atmSig_findSvcBlockByVC(idb, *(vcd_t *)map->hwaddr.addr);
	if (found_svc)
	    atmSmap_staticMapDelete(found_svc, idb);
    	mapl = atm_arp_getmapl(idb);
	staticmap_delete(mapl, map);
    }

   /*
    * Call the ARP server right away....
    */
   atm_arp_call_srvr(idb, NULL, entry);
}

/*
 * atm_arp_client_off
 *
 * Stop running the ATM ARP client on the passed interface
 */
void
atm_arp_client_off (idbtype *idb, hwaddrtype *hwaddr)
{
    atm_arpserv_entry *entry;
    maplist_type *mapl;
    char mapl_name[MAPL_NAME];
    mapgroup_type *mapg;

    if (atm_arp_debug) {
	char nsapstr[MAX_ATM_NSAP_ASCII_LEN];

	if (hwaddr != NULL) {
	    atm_printnsap(hwaddr->addr, nsapstr);
	    buginf("\nATMARP(%s): Stop ATM ARP client to %s",
		   idb->namestring, nsapstr);
	} else
	    buginf("\nATMARP(%s): Stop all ATM ARP clients", idb->namestring);
    }

    /*
     *  If we got passed a specific NSAP, search for it and only free it.
     */
    if (hwaddr != NULL) {
	atm_arpserv_entry *entry;

	FOR_ALL_ARP_SERVERS(idb->atm_arp, entry) {
	    if (snpa_equ(&entry->nsap, hwaddr)) {
		arpserv_nuke_client(idb, entry);
		break;
	    }
	}
        /*
         * if no more server left, remove the arp map group
         * on this interface.
         */
        if (idb->atm_arp->as_data.client.arp_serv_list == NULL) {
            atm_arp_build_mapl_name(idb, mapl_name);
            mapg = mapgroup_lookup(idb, mapl_name);
            if (mapg) {
                mapgroup_delete(idb, mapg);
            }
        }
	return;
    }

    /*
     *  We got passed a NULL: loop through all the entries, nuking each
     *  one.  We cannot use FOR_ALL_ARP_SERVERS here, because we free() the
     *  entry before the entry->next field would be dereferenced.
     */
    for (entry = idb->atm_arp->as_data.client.arp_serv_list; entry != NULL; ) {
        atm_arpserv_entry *tmp;

	tmp = entry->next;
	arpserv_nuke_client(idb, entry);
	entry = tmp;
    }

    /*
     * Flush out the maplist
     */
    mapl = atm_arp_getmapl(idb);
    if (mapl) {
	uint ndx;
	for (ndx = 0; ndx < STATIC_MAP_HASH; ndx++) {
	    staticmap_type *map;
	    atmSvcType *found_svc;

	    for (map = mapl->hash[ndx] ; map != NULL ; ) {
	    	staticmap_type *newmap = map->next;
		vcd_t vcd;

		vcd = *(vcd_t *)map->hwaddr.addr;
		found_svc = atmSig_findSvcBlockByVC(idb, vcd);
		if (found_svc) {
		    vc_info_t *vc_info;
		    
		    /*
		     * Try to hang this sucker up, now that we're done with it
		     */
		    vc_info = atm_getvc(idb->hwptr->atm_db, vcd);
		    if (vc_info != NULL)
			atm_hangup_vc(vc_info);
	    	    atmSmap_staticMapDelete(found_svc, idb);
		}
		map = newmap;
	    }
	} 
    }
    atm_arp_build_mapl_name(idb, mapl_name);
    mapg = mapgroup_lookup(idb, mapl_name);
    if (mapg) {
	mapgroup_delete(idb, mapg);
    }

    /*
     * Flush out all ARP entries for this IDB
     */
    reg_invoke_ar_cache_clear(idb);
}

/*
 * arpserv_entry_from_map
 *
 * Given a static map, return the ARP Server entry which reflects the ARP
 * server it points to.
 */
atm_arpserv_entry *
locate_arpserv_entry (idbtype *idb, hwaddrtype *nsap)
{
    atm_arpserv_entry *entry;

    FOR_ALL_ARP_SERVERS(idb->atm_arp, entry) {
	if (snpa_equ(&entry->nsap, nsap))
	    return(entry);
    }
    return(NULL);
}

/*
 *  Remove a specific arp server from our list of ARP servers we have clients
 *  for.
 */
static void
arpserv_nuke_client (idbtype *idb, atm_arpserv_entry *entry)
{
    staticmap_type  *map;
    atmSvcType      *svc;
   
    if ((NULL != idb) && (NULL != entry)) {
        /*
         *  Destroy the map entry
         */
        map = staticmap_hwlookup(idb, &entry->nsap, ET_ATM, TRUE, LINK_ARP);
        if (NULL != map) {
            svc = atmSig_findSvcBlockByVC(idb, *(vcd_t *)map->hwaddr.addr);
            if (NULL != svc) {
                atmSmap_closeSvcConnection(svc); /* hang up on the server */
                atmSmap_staticMapDelete(svc, idb);
            }
            staticmap_delete(atm_arp_getmapl(idb), map);
        }
 
        /*
         *  And remove the ARP Server entry
         */
        if (entry->prev != NULL) {
            entry->prev->next = entry->next;
        }
        if (entry->next != NULL) {
            entry->next->prev = entry->prev;
        }
        if (idb->atm_arp->as_data.client.arp_serv_list == entry) {
            idb->atm_arp->as_data.client.arp_serv_list = entry->next;
        }
        free(entry);
    }
}


/*
 * atm_arp_glom_by_serv
 *
 * Try to glom onto to a VC to the same destination NSAP  - this
 * will create a static map if one does not already exists and broadcast
 * traffic i.e. route updates can then be sent to the remote client.
 *
 */
boolean 
atm_arp_glom_by_serv (idbtype *idb, uchar atm_addr_len, uchar *atm_addr,
 		      ipaddrtype ip_address)
{
    addrtype addr;
    staticmap_type *map;
    hwaddrtype hwaddr;

    /* first check to see if a map entry exists      */
 
    addr.length = ADDRLEN_IP;
    addr.type = ADDR_IP;
    bcopy(&ip_address, &addr.addr, addr.length);
    map = staticmap_lookup(idb, &addr, LINK_IP, ET_ATM, FALSE);

    if (map) {
        if (map->hwaddr.type != STATION_ILLEGAL) 
            return(FALSE);      /* map is already used for a VC, do nothing */
    } else {

        /* create a static map  */ 
 	
        hwaddr.type = STATION_ATMNSAP;
        hwaddr.length = atm_addr_len & ATM_ADDR_LEN_MASK;
        hwaddr.target_type = TARGET_UNICAST;
        bcopy(atm_addr, &hwaddr.addr, hwaddr.length);
 	  
        map = atm_arp_build_a_map(atm_arp_getmapl(idb), &addr, &hwaddr, idb);
        if (!map) {
            return(FALSE);
        }
 
    }
 
    /* now try to glom onto a VC to the same destination */
 
    if (atm_glom_onto_vc(map, idb) != NULL)  {
         return(TRUE);        
    }
 
    return(FALSE);
}

/*
 * ATM_ARP subsystem header
 */

#define ATM_ARP_MAJVERSION 1
#define ATM_ARP_MINVERSION 0
#define ATM_ARP_EDITVERSION  1

/*
 * ATM ARP subsystem must be initialized.
 */
SUBSYS_HEADER(atm_arp, ATM_ARP_MAJVERSION, ATM_ARP_MINVERSION,
	      ATM_ARP_EDITVERSION,
	      atm_arp_init, SUBSYS_CLASS_KERNEL,
	      NULL,
	      "req:atm,atmsig");
