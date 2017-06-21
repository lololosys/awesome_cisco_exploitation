/* $Id: atmsig_smap.c,v 3.8.20.25 1996/09/11 01:10:39 rzagst Exp $
 * $Source: /release/112/cvs/Xsys/atm/atmsig_smap.c,v $
 *------------------------------------------------------------------
 * A T M S I G _ S M A P . C 
 *
 * January 1995,  Percy P. Khabardar 
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmsig_smap.c,v $
 * Revision 3.8.20.25  1996/09/11  01:10:39  rzagst
 * CSCdi67786:  %ATM-4-MTUCALLMISMATCH message can be confusing
 * Change message to indicate PDU mismatch and relationship between
 * interface
 * PDU and MTU.
 * Branch: California_branch
 *
 * Revision 3.8.20.24  1996/08/28  12:37:22  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.8.20.23  1996/08/16  02:13:15  lmercili
 * CSCdi65496:  rfc1577 failed ip routing
 * Branch: California_branch
 *
 * Revision 3.8.20.22  1996/08/15  20:01:31  schrupp
 * CSCdi65985:  ATM ESI: Dereg addresses when prefix is deleted
 * Branch: California_branch
 *
 * Revision 3.8.20.21  1996/07/29  21:02:01  jwjang
 * CSCdi64423:  atmsig_smap.c call atmsig_interface_up wrong
 * Branch: California_branch
 *
 * Revision 3.8.20.20  1996/07/10  22:20:27  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.8.20.19  1996/06/07  22:02:56  schrupp
 * CSCdi56175:  concurrent use of LANE and RFC1577, RFC1577 is not working
 * Branch: California_branch
 *
 * Revision 3.8.20.18  1996/06/04  19:32:42  schrupp
 * CSCdi59480:  ATMSMAP: Use registry to request prefix from ILMI
 * Branch: California_branch
 *
 * Revision 3.8.20.17  1996/06/04  16:05:25  schrupp
 * CSCdi59433:  Boot images wont build
 * Branch: California_branch
 *
 * Revision 3.8.20.16  1996/06/03  23:09:18  schrupp
 * CSCdi59328:  ATMSMAP: Request prefix from ILMI upon prefix deletion
 * Branch: California_branch
 *
 * Revision 3.8.20.15  1996/05/30  23:37:36  grobelch
 * - Use appropriate static-map params block to check for deleting
 * broadcast
 *   P2P VC in order to delete IP Multicast cache entry.
 * CSCdi57525:  Fix to static-map update for ATM broadcast P2P VC delete
 * Branch: California_branch
 *
 * Revision 3.8.20.14  1996/05/29  21:20:49  mdavison
 * CSCdi57676:  %ALIGN-3-SPURIOUS: Spurious memory access
 * Branch: California_branch
 *
 * Revision 3.8.20.13  1996/05/25  00:38:41  schrupp
 * CSCdi41888:  ILMI negotiation occurs with no ESI assigned
 * Branch: California_branch
 *
 * Revision 3.8.20.12  1996/05/24  23:32:50  achopra
 * CSCdi56238:  Linear search for glommable VCs in the vc HASH table has
 *         been changed to a binary search in a WAVL tree maintained per
 *         swidb, which is keyed on NSAPs, AAL5 encapsulation and
 *         protocol type if encapsulation is AAL5MUX.
 * Branch: California_branch
 *
 * Revision 3.8.20.11  1996/05/24  21:08:42  schrupp
 * CSCdi58635:  ATM: ESI cleanup
 * Branch: California_branch
 *
 *  - Deregister addresses when a new prefix is added by ILMI
 *  - Deregister addresses when ESI/NSAP is changed from command line
 *  - Reregister addresses even if the new prefix is the same as the old
 *  - Deregister addresses when subinterface is shut
 *  - Reregister addresses when subinterface is no shutD
 *  - Don't allow qsaal and ilmi pvc's on subinterfaces
 *  - Init atm_db->peeruni_ifname
 *
 * Revision 3.8.20.10  1996/05/15  21:38:10  mdavison
 * CSCdi56536:  Call mtu mismatch message needs to show calling party
 * address
 * Branch: California_branch
 *
 * Revision 3.8.20.9  1996/05/09  14:11:17  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.8.20.6.2.4  1996/05/03  21:40:31  kjadams
 * CSCdi56624:  Incorrect BLLI for Token Ring LANE Data Direct VCC
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 * back out last change - confine fix to /lane.
 *
 * Revision 3.8.20.6.2.3  1996/05/03  18:27:25  kjadams
 * CSCdi56624:  Incorrect BLLI for Token Ring LANE Data Direct VCC
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 * 2 additional blli's for tr lane.  Bump max_blli by 2 and initialize
 * the extra 2.
 *
 * Revision 3.8.20.6.2.2  1996/05/01  20:46:25  rbadri
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 * SDU sizes follow sub-interface MTU, rather than maxmtu
 *
 * Revision 3.8.20.6.2.1  1996/04/27  06:33:39  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.8.20.8  1996/05/08  02:43:51  grobelch
 * - Create system list registry functions invoked by static-map software
 *   when:
 *     a static-map is deleted, a map-group is deleted, and when a
 *     static-map is updated. A provider of static-map service (such
 *     as ATM signalling) can register for this, check for one of it's
 *     map's being the subject of the call, and act appropriately.
 *   - Provide ATM signalling routines for the static-map and group
 *     registries. Remove ARP table entries for IP and free up
 *     ATM signalling-specific data structures.
 *   - Improve vc "glomming" for point-to-point VCs to take in
 *     AAL5MUX case.
 *   - Fix mistaken use of an enumeration as a bit-value.
 * CSCdi53027:  ATMSIG: SVC static maps not updated properly in all cases
 * Branch: California_branch
 *
 * Revision 3.8.20.7  1996/05/06  16:43:47  dino
 * CSCdi56689:  When a multipoint-VC goes away, the IP multicast cache is
 *              not fixed
 * Branch: California_branch
 *
 * Revision 3.8.20.6  1996/04/25  23:02:30  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.8.20.5  1996/04/08  20:51:34  ronnie
 * CSCdi51093:  no atm arp-server command causes crash
 * Branch: California_branch
 *
 * Revision 3.8.20.4  1996/04/03  04:51:41  schrupp
 * CSCdi53125:  ATM signalling leaks memory
 * Branch: California_branch
 * Use malloc_named for ATMSIG to readily identify possible memory culprits.
 *
 * Revision 3.8.20.3.4.1  1996/04/24  21:40:15  rbadri
 * CSCdi55597:  SDU sizes in signalling do not follow sub-interface MTU
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.8.20.3  1996/03/29  23:42:42  schrupp
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
 * Revision 3.8.20.2  1996/03/29  19:28:25  grobelch
 *   Provide traffic shaping for SVCs when possible.
 *   Also:
 *   - Specify map-class rate parameters in kbps.
 *   - Provide a per-interface command to indicate
 *     whether SVC should be established if signalled
 *     parameters cannot be installed.
 *   - Don't attach ATM submaps to map-class block
 *   - Check for map-class params prior to using
 *     them (they can be null).
 *   - Fix help-strings for "clp1" parameters to
 *     indicate for clp0&1 flows.
 *   - Signal best effort if only pcr for clp0&1 are
 *     specified.
 *   - map-class parameters override map-list where
 *     they overlap.
 *   - Fix a couple of erroneous variable setting/usages
 *     in static-map handling.
 * CSCdi51550:  ATMSIG: SVCs should attempt to use traffic shaping
 * Branch: California_branch
 *
 * Revision 3.8.20.1  1996/03/18  18:59:41  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.12.2  1996/03/07  08:30:30  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.12.1  1996/02/20  00:18:58  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/03  22:16:51  ronnie
 * CSCdi46955:  ATM ARP Server table out of sync with map list
 *
 * Revision 3.7  1996/02/01  05:59:48  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/24  22:56:15  ronnie
 * CSCdi45540:  ATM interface causes memory corruption on 7k
 *
 * Revision 3.5  1995/12/20  01:07:57  ronnie
 * CSCdi45733:  atm crashes router if no PVCs declared
 *
 * Revision 3.4  1995/11/28  22:33:23  rzagst
 * CSCdi39434:  atm->vc_hash[] should be (vc_info_t *) not unsigned
 *
 * Revision 3.3  1995/11/22  20:44:39  ronnie
 * CSCdi44212:  Many fixes done at Telecom Finland
 *
 * Revision 3.2  1995/11/17  08:46:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.12  1995/10/03  23:40:43  schrupp
 * CSCdi40453:  ATM SDU size should be MTU + LLC/SNAP in Setup Msg
 *
 * Revision 2.11  1995/09/28  20:49:17  ronnie
 * CSCdi40510:  Too many ATM SVCs set up to same destination
 *              Also added some debugging to the ATM SigAPI
 *
 * Revision 2.10  1995/09/01  17:48:39  percyk
 * CSCdi39722:  set sdu sizes per connection - part 2
 *
 * Revision 2.9  1995/08/23  20:43:12  schrupp
 * CSCdi39215:  ATM ILMI always use latest prefix from switch
 *
 * Revision 2.8  1995/08/18  14:23:16  schrupp
 * CSCdi38181:  Add ATMSIG throttling and SSCOP changes
 *
 * Revision 2.7  1995/08/11  19:46:50  schrupp
 * CSCdi38620:  Prohibit SVC opens until ATMSIG processes active
 *
 * Revision 2.6  1995/08/08  05:33:29  schrupp
 * CSCdi38341:  Dont cancel ATM NSAP if never registered
 *
 * Revision 2.5  1995/08/03  20:21:35  schrupp
 * CSCdi38024:  Add interface string to show atm map command
 *
 * Revision 2.4  1995/07/31  23:50:33  schrupp
 * CSCdi37976:  Dont search ILMI table for new prefixes after prefix delete
 *
 * Revision 2.3  1995/07/26  15:13:17  schrupp
 * CSCdi37690:  Support ATM ESI addressing with ILMI prefix
 *
 * Revision 2.2  1995/06/18  20:31:42  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:12:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../atm/msg_atm.c"		/* Not a typo, see logger.h */
#include "sys_registry.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "../if/atm.h"
#include "../if/arp.h"
#include "atm_arp.h"
#include "atmsig_api.h"
#include "../if/static_map.h"
#include "atmsig_private.h"
#include "atmsig_public.h"
#include "atmsig_smap.h"
#include "ilmi_public.h"
#include "../atm/atm_registry.h"
#include "../ui/debug.h"
#include "../if/atm_debug_flags.h"
#include "../ip/ip_registry.h"
#include "media_registry.h"


static void atmSig_enterVcnumInMap(atmSvcType *, staticmap_type *);
static void atmSig_stuff_vcd_in_map(staticmap_type *, vcd_t);
vc_info_t *atm_glom_onto_vc(staticmap_type *, idbtype *);

static void atmSmap_rcvListen(sig_api_listen *, idbtype *);
static void atmSmap_rcvCancel(sig_api_cancel *, idbtype *);
static void atmSmap_rcvSetup(sig_api_setup *, idbtype *);
static void atmSmap_rcvConnect(sig_api_connect *, idbtype *);
static void atmSmap_rcvConnectAck(sig_api_connect_ack *, idbtype *);
static void atmSmap_rcvRelease(sig_api_release *, idbtype *);
static void atmSmap_rcvReleaseComplete(sig_api_release_comp *, idbtype *);

static void atmSmap_rcvAddParty(sig_api_add_party *, idbtype *);
static void atmSmap_rcvAddPartyAck(sig_api_add_party_ack *, idbtype *);
static void atmSmap_rcvAddPartyRej(sig_api_add_party_rej *, idbtype *);
static void atmSmap_rcvDropParty(sig_api_drop_party *, idbtype *);
static void atmSmap_rcvDropPartyAck(sig_api_drop_party_ack *, idbtype *);

/*
 * a t m S m a p _ r e g i s t e r A t m A d d r e s s ( )
 *
 *
 */
boolean atmSmap_registerAtmAddress (idbtype *idb)
{

   sig_api_msg *sig_msg;
   sig_api_listen *listen_msg;


   /* send request to sig driver to listen on this atm address */

   /* paranoia */
   if (!idb || (!idb->nsap) ) {
      if (atmSmap_debugAll || atmSmap_debugError)
         buginf("\nATMSM: no idb, register req failed");
      return (FALSE);
   }

   /* get memory to send request */
   sig_msg = malloc( sizeof(sig_api_msg) );
   if (sig_msg == NULL) {
     return (FALSE);
   }

   /* fill in the sig_api_listen block */

   sig_msg->opcode = ATM_LISTEN_NSAP;
   sig_msg->idb    = idb;

   listen_msg = (sig_api_listen *)&sig_msg->u.listen;
   listen_msg->sig_user_callback = atmSmap_callbackFunction;
   listen_msg->result = ATM_SIGAPI_OK;
   bcopy( idb->nsap, &listen_msg->nsap, sizeof(hwaddrtype) );

   /* send request to Signalling driver */
   atmSmap_sendToSigDriver(sig_msg);

   return (TRUE);

}



/*
 * a t m S m a p _ c a n c e l A t m A d d r e s s ( )
 *
 *
 */
boolean atmSmap_cancelAtmAddress (idbtype *idb)
{

   sig_api_msg *sig_msg;
   sig_api_cancel *cancel_msg;


   /* send request to sig driver to not listen on this atm address */

   /* paranoia */
   if (!idb || (!idb->nsap) ) {
      if (atmSmap_debugAll || atmSmap_debugError)
         buginf("\nATMSM: no idb, cancel req failed");
      return (FALSE);
   }

   /* get memory to send request */
   sig_msg = malloc( sizeof(sig_api_msg) );
   if (sig_msg == NULL) {
     return (FALSE);
   }

   /* fill in the sig_api_listen block */

   sig_msg->opcode = ATM_CANCEL_NSAP;
   sig_msg->idb    = idb;

   cancel_msg = (sig_api_cancel *)&sig_msg->u.cancel;
   cancel_msg->result = ATM_SIGAPI_OK;
   bcopy( idb->nsap, &cancel_msg->nsap, sizeof(hwaddrtype) );

   /* send request to Signalling driver */
   atmSmap_sendToSigDriver(sig_msg);

   return (TRUE);

}

/*
 * atm_addr_lookup: Called to find static mapping entries for use.
 * OUTPUT/RETURN:
 *	vc_info_t: Ptr. to a VC struct. that matches the address given.
 *		NULL if no VC was found to match that address.
 * INPUT:
 *	idb:	Pointer to the IDB we're interested in.  Must match with
 *		protocol address.
 *	atm:	Ptr. to the AIP struct for the above IDB.
 *	map:	Pointer to static map entry.
 */
vc_info_t *
atm_addr_lookup (idbtype *idb, atm_db_t *atm, staticmap_type *map)
{
    vcd_t vcnum = -1;
    vc_info_t *vc;

    if (map) {
	if (map->hwconf.type == STATION_ATMVC)
	    bcopy(&map->hwconf.addr, &vcnum, STATIONLEN_ATMVC);
	else if (map->hwaddr.type == STATION_ATMVC)
	    bcopy(&map->hwaddr.addr, &vcnum, STATIONLEN_ATMVC);
        else if (map->hwconf.type == STATION_ATMNSAP) {
	    /*
	     *  It is possible that we can glom onto an existing VC, so check
	     *  this before we decide to force a new call to be placed.
	     */
	    vc = atm_glom_onto_vc(map,idb);
	    if (vc != NULL) {
		/* We glommed.  Return the resulting VC */
		return(vc);
	    }
	    if (atm->sig_vcnum)
		vcnum = atm->sig_vcnum;
	    else {
		if (atm_errors_debug)
		    buginf("\nATM: sig vc not configured for %s", 
			   idb->namestring);
            }
        } else if (map->hwconf.type == STATION_SMDS64) {
	    vcnum = atm_get_aal34vcnum(idb);
	}
	if (!ATM_BAD_VC(atm, vcnum)) {
	    vc = atm_getvc(atm, vcnum);
	    if (vc) {
		if ( (vc->swidb == idb) ||
		    (atm && (vcnum == atm->sig_vcnum)) ) {
		    return(vc);
		}
	    }
	}
    }
    return(NULL);
}

/*
 * vc_for_physaddr
 *
 * return a vc struct pointer if we have an existing VC on the passed IDB
 * which can run the passed protocol as well.
 *
 * right now we ignore the protocol address, but we probably should not.  We
 * also really need some way of ensuring that the QOS match.
 */
static vc_info_t *
vc_for_physaddr (staticmap_type *map, idbtype *idb)
{
    vc_info_t salt, *vc;
    atm_params_type *params;

    if (idb == NULL || idb->atm_glommable_vc_tree == NULL || map == NULL)
	return(NULL);

    /* Get the correct params structure - static map or class */
    if (map->class && map->class->params)
	params = map->class->params;
    else
	params = map->params;

    /*
     * Salt the node according to the key i.e. with NSAP address, AAL5 
     * encapsulation and protocol.
     */
    salt.calledparty = map->hwconf;
    salt.linktype = map->protocol;
    if (params->aal5mux)
	salt.flags = ATM_VC_AAL5MUX;
    else 
	salt.flags = ATM_VC_AAL5SNAP;

    /* Search for an existing glommable VC in idb's WAVL tree. */
    vc = wavl_to_vcinfo(wavl_search(idb->atm_glommable_vc_tree, &salt.wavl, 0));
    return(vc);
}

/*
 * atm_glom_onto_vc
 *
 * attempt to find an existing connection to use.  this should always be
 * done before attempting to place a call.
 */
vc_info_t *
atm_glom_onto_vc (staticmap_type *map, idbtype *idb)
{
    vc_info_t *vc;
    atmSvcType *svc;

    vc = vc_for_physaddr(map, idb);

    if (vc == NULL)
	return(NULL);
    
    if (IS_ATMVC_SVC(vc)) {
       svc = atmSig_findSvcBlockByVC(vc->swidb, vc->vc);
       if (svc == NULL)
          return(NULL);
       if (svc->state != ATMSIG_ST_ACTIVE)
          return(NULL);
    }

    /* Great!  We'll just glom onto it */
    if (atm_arp_debug || atm_event_debug)
	buginf("\nATMSM(%s): Attaching to VC #%d for type %d traffic",
	       idb->namestring, vc->vc, map->addr.type);

    if (!map->sm_swidb)
       map->sm_swidb = idb;

    atmSig_stuff_vcd_in_map(map, vc->vc);

    /*
     *  If the atm arp server is running, it needs to know that we are now
     *  using this connection.
     */
    reg_invoke_atm_arp_glommed_onto_vc(map->sm_swidb, vc->vc,
					    map->protocol,&map->addr);
    return(vc);
}

/*
 *   atmSmap_get_params
 *
 *       returns the appropriate params structure
 */
static atm_params_type *
atmSmap_get_params (idbtype *swidb, staticmap_type *map)
{
    atm_db_t *atm;
    atm_params_type *params; 

    if ((swidb == NULL) || (map == NULL))
	return(NULL);

    atm = swidb->hwptr->atm_db;
    if (atm == NULL)
	return(NULL);

    /* use the configured map class if it exists */
    if (map->class && map->class->params) {
        params = map->class->params;
        if (atmSmap_debugAll || atmSmap_debugEvent)
            buginf("\nATMSM: Map class used: %s", map->class->name);

    } else if (atm->def_class && atm->def_class->params) {
        /* default map class for the interface */
        params = atm->def_class->params; 
        if (atmSmap_debugAll || atmSmap_debugEvent)
            buginf("\nATMSM: Map class used: %s (default)", atm->def_class->name);

    } else {
        /* best effort traffic parameters */
        params = map->params;
        if (atmSmap_debugAll || atmSmap_debugEvent)
            buginf("\nATMSM: Map class used: best effort");
    }

    return(params);
}

/* 
 * a t m S m a p _  o p e n S v c C o n n e c t i o n ( )
 *
 * Issues a request to the Sig driver to open a svc connection to the 
 * requested destination.
 *
 * Before a new request is issued a check is made to see if a previous   
 * request to the same destination exists.  If a previous request is 
 * pending then the current request is dropped.
 *
 * If no previous request exists then a new one is issued to the Sig 
 * driver.
 *
 */
boolean
atmSmap_openSvcConnection (idbtype *idb, staticmap_type *map, int flags)
{

   atmSvcType *call_id;
   atm_params_type *params, *ptr_to_smap_params;
   sig_api_msg *sig_msg;
   sig_api_setup *setup_msg;
   atmSig_smap *smap;
   mapgroup_type *map_group;
   staticmap_type *map_ptr;
   int i;
   atm_params_type *first_params = NULL;
   atmSig_smap *first_smap = NULL;
   atm_db_t *atm;


   /*
    *  We are going to be clever.  (Uh oh!  Better take cover!)  If we
    *  already have a VC up to this destination NSAP, and it can take
    *  this protocol traffic also, we'll just use that one also.
    */
   if (atm_glom_onto_vc(map,idb) != NULL) {
       /* we glommed on--no need to do anything else */
       return(TRUE);
   }
   
    
   /* valid output ATM port? */
   if (!idb || !(idb->hwptr->status & IDB_ATM) ) {
      if (atmSmap_debugAll || atmSmap_debugError)
         buginf("\nATMSM: no output port, open req failed");
      return (FALSE);
   }

   /* ATM Signalling subsystem initialized? */
   if (!atmsig_interface_up(idb)) {
      if (atmSmap_debugAll || atmSmap_debugError) 
         buginf("\nATMSM: subsystem init not complete, open req failed");
      return (FALSE);
   }

   /* valid nsap prefix? find the nsap info on the atm db */
   atm = idb->hwptr->atm_db;
   if (!atm) {
     if (atmSmap_debugAll || atmSmap_debugError)
       buginf("\nATMSM: no atm_db, open req failed");
     return (FALSE);
   }

   /* valid registered nsap?
    * idb->atm_nsap_valid set when registering with signalling
    */
   if ( (!idb->nsap) || (!idb->atm_nsap_valid) ) {
      if (atmSmap_debugAll || atmSmap_debugError)
         buginf("\nATMSM: nsap not valid");
      return (FALSE);
   }

   /* valid map block? */
   if (!map) {
      if (atmSmap_debugAll || atmSmap_debugError)
         buginf("\nATMSM: null map control block, open req failed");
      return (FALSE);
   }

   /*
    * Get the correct params structure - if no TOS configured get it directly
    * off static map struct, else get it off class struct or the default
    * map class for the interface.
    */
   params = atmSmap_get_params(idb, map);

   if (!params) {
      if (atmSmap_debugAll || atmSmap_debugError)
         buginf("\nATMSM: params block not found, open req failed");
      return (FALSE);
   }

   ptr_to_smap_params = map->params;
   if (!ptr_to_smap_params) {
      if (atmSmap_debugAll || atmSmap_debugError)
         buginf("\nATMSM: map params block not found, open req failed");
      return (FALSE);
   }

   /*
    * A smap exists?  A presence of smap in the static map means 
    * that a request to establish call is already pending.  Let the previous
    * request complete before issuing another one.
    */
   if (ptr_to_smap_params->smap) {
     if (atmSmap_debugAll || atmSmap_debugError)
       buginf("\nATMSM: smap exists, %x open req failed", 
	ptr_to_smap_params->smap);
     return (FALSE);
   }
   else {
     smap = malloc_named( sizeof(atmSig_smap), "ATMSIG-SMAP");
     if (smap)
       ptr_to_smap_params->smap = smap;
     else {
       return (FALSE);
     }
   }

   /* get a call_id */
   call_id = atmSig_api_allocate_call_id();
   if (call_id == NULL) {
      if (atmSmap_debugAll || atmSmap_debugError)
         buginf("\nATMSM: no mem to alloc call_id block, open req failed");
      ptr_to_smap_params->smap = NULL;
      free(smap);
      return (FALSE);
   }

   /* save destination atm address in smap block */
   bcopy(map->hwconf.addr, smap->destAtmAddr.addr, STATIONLEN_ATMNSAP);
   smap->destAtmAddr.type = STATION_ATMNSAP;
   smap->destAtmAddr.length = STATIONLEN_ATMNSAP;
   smap->destAtmAddr.target_type = TARGET_UNICAST;

   /* save source atm address in smap block */
   bcopy(idb->nsap->addr, smap->srcAtmAddr.addr, STATIONLEN_ATMNSAP);
   smap->srcAtmAddr.type = STATION_ATMNSAP;
   smap->srcAtmAddr.length = STATIONLEN_ATMNSAP;
   smap->srcAtmAddr.target_type = TARGET_UNICAST;

   /* Save first_params before checking map entries below */
   smap->call_id = call_id;
   smap->locallyInitiatedCall = TRUE;
   first_params = params; 
   first_smap = smap; 

   /* Get a sig_msg block for the API. If none available, exit now */
   sig_msg = malloc_named( sizeof(sig_api_msg),"ATMSIG_SMAP" );
   if (sig_msg == NULL) {
     free(smap);
     ptr_to_smap_params->smap = NULL;
     free(call_id->rootParty);
     free(call_id);
     return (FALSE);
   }

   /* If aal5mux, only 1 smap on this call, smap block already filled
    * in, add protocol, fill in msg, stuff idb in map->idb.
    */
   if (params->aal5mux) {
     smap->linktype = map->protocol;
     call_id->aal5mux = TRUE;
     call_id->linktype = map->protocol;
     map->sm_swidb = idb;
     goto tx_setup;
   }         

   /* enter call_id in all static maps pointing to the dest atm address */
   FOR_ALL_MAPGROUPS_ON_IDB(idb, map_group) {
     FOR_ALL_MAPS_IN_LIST(i, map_group->maplist, map_ptr) {

       /*
	* get correct qos structure - if no qos configured get it directly
	* off static map, otherwise get it off the class struct or the
	* default map class for the interface.
	*/
       params = atmSmap_get_params(idb, map_ptr);

       if (!params)
	   continue;

       /* proceed only if address match */
       if ( bcmp(map_ptr->hwconf.addr, first_smap->destAtmAddr.addr,
		                          STATIONLEN_ATMNSAP) != 0 )
	   continue;

       /* don't overwrite existing aal5mux map */
       if (params->aal5mux)
	 continue;

       ptr_to_smap_params = map_ptr->params;

       /* stuff idb in map->idb */
       map_ptr->sm_swidb = idb;

       /* address match, not aal5mux, could share VC for multiple map entries
	* if so, need to malloc smap here, fill in smap block
	*/
       smap = ptr_to_smap_params->smap;

       if (!smap) {
	 smap = malloc_named( sizeof(atmSig_smap), "ATMSIG-SMAP");

	 if (smap)
	   ptr_to_smap_params->smap = smap;
	 else {
	   return (FALSE); 
	 }
       }
       /* save destination atm address in smap block */
       bcopy(map->hwconf.addr, smap->destAtmAddr.addr, STATIONLEN_ATMNSAP);
       smap->destAtmAddr.type = STATION_ATMNSAP;
       smap->destAtmAddr.length = STATIONLEN_ATMNSAP;
       smap->destAtmAddr.target_type = TARGET_UNICAST;

       /* save source atm address in smap block */
       bcopy(idb->nsap->addr, smap->srcAtmAddr.addr, STATIONLEN_ATMNSAP);
       smap->srcAtmAddr.type = STATION_ATMNSAP;
       smap->srcAtmAddr.length = STATIONLEN_ATMNSAP;
       smap->srcAtmAddr.target_type = TARGET_UNICAST;

       smap->call_id = call_id;
       smap->locallyInitiatedCall = TRUE;

     }
   }	   

   /* send request to the Signalling driver to open svc */
tx_setup:
   /* fill in sig_api_setup block */

   sig_msg->opcode = ATM_SETUP;
   sig_msg->idb    = idb; 

   setup_msg = (sig_api_setup *)&sig_msg->u.setup;
   setup_msg->call_id = call_id;

   /* 
    * provide a call back function for Sig driver to notify client of
    * incoming events.
    */ 
   setup_msg->sig_user_callback = atmSmap_callbackFunction;

   /* copy Type of Service parameters in message */ 
   /* use first params found */
   bcopy( first_params, &setup_msg->params, sizeof(atm_params_type) ); 

   /* fill in destination ATM address in message */
   bcopy(first_smap->destAtmAddr.addr, setup_msg->called_party.addr, 
                                                    STATIONLEN_ATMNSAP);
   setup_msg->called_party.type = STATION_ATMNSAP;
   setup_msg->called_party.length = STATIONLEN_ATMNSAP;
   setup_msg->called_party.target_type = TARGET_UNICAST;

   /* fill in source ATM address in message */
   bcopy(idb->nsap->addr, setup_msg->calling_party.addr, STATIONLEN_ATMNSAP);
   setup_msg->calling_party.type = STATION_ATMNSAP;
   setup_msg->calling_party.length = STATIONLEN_ATMNSAP;
   setup_msg->calling_party.target_type = TARGET_UNICAST;

   /* fill in sdu size */
   if (first_params->aal5mux) 
     setup_msg->fwd_sdu_size = setup_msg->bwd_sdu_size = idb->sub_mtu;
   else
     setup_msg->fwd_sdu_size = setup_msg->bwd_sdu_size = idb->sub_mtu +
       ATM_SNAP_HDRBYTES;

   /* fill in Broadband Lower Layer info */
   /* snap vc? */
   if (!first_params->aal5mux) {
      setup_msg->blli[0].length = 1;
      setup_msg->blli[0].string[0] = 0xCC;  

      setup_msg->blli[1].length = 0; 
      setup_msg->blli[2].length = 0;
     
      setup_msg->flags = ATM_VC_AAL5SNAP; 
   }
   else {
     atmSig_fillBroadbandLowLayerInfo(map, setup_msg);
     setup_msg->flags = ATM_VC_AAL5MUX; 
   }
   setup_msg->flags |= flags;

   /* set multipoint flag in setup msg */
   setup_msg->multipoint = FALSE;

   /* send to Signalling driver */
   atmSmap_sendToSigDriver(sig_msg);

   return (TRUE);

}



/*
 * a t m S m a p _ c l o s e S v c C o n n e c t i o n ( )
 *
 * The only reason someone would want to close an svc connection on a router
 * is when a period of inactivity is detected on the svc.  Usually a background
 * process would trigger this event on detection of an idle svc.
 *
 * NOTE - currently I don't expect any vc to be idle as periodic routing 
 * updates will be constantly taking place on all the vc's.
 */
void atmSmap_closeSvcConnection (void *call_id)
{

   sig_api_msg *sig_msg;
   sig_api_release *release;


   /* send request to the Signalling driver to close the svc */

   /* get memory */
   sig_msg = malloc( sizeof(sig_api_msg) );
   if (sig_msg == NULL) {
     return;
   }

   /* fill in sig_api_release block */

   sig_msg->opcode = ATM_RELEASE;

   release = (sig_api_release *)&sig_msg->u.release;
   release->call_id = call_id;
   release->cause = ATM_NORMAL_UNSPECIFIED;

   /* send to Signalling driver */
   atmSmap_sendToSigDriver(sig_msg);

}



/*
 * a t m S m a p _ f i n d S v c S t a t u s ( )
 *
 * Find the status (state) of the svc by sending a Status Enquiry message
 * to the switch.
 */
boolean atmSmap_findSvcStatus (void *call_id)
{

   sig_api_msg *sig_msg;
   sig_api_status_enq *status_enq;


   /* send request to the Signalling driver to send Status Enquiry req  */

   /* get memory */
   sig_msg = malloc( sizeof(sig_api_msg) );
   if (sig_msg == NULL) {
     return (FALSE);
   }

   /* fill in sig_api_status_enq block */

   sig_msg->opcode = ATM_STATUS_ENQUIRY;

   status_enq = (sig_api_status_enq *)&sig_msg->u.status_enq;
   status_enq->call_id = call_id;

   /* send to Signalling driver */
   atmSmap_sendToSigDriver(sig_msg);

   return (TRUE);
}



/*
 * a t m S m a p _ c a l l b a c k F u n c ( )
 *
 * This callback function is used by the Signalling driver to report
 * events to the Static Map client.
 * 
 */
void atmSmap_callbackFunction (void *ptr)
{

   idbtype *idb;
   sig_api_msg *sig_msg;
   sig_api_listen *listen;
   sig_api_cancel *cancel;
   sig_api_connect *connect;
   sig_api_connect_ack *connect_ack;
   sig_api_setup *setup;
   sig_api_release *release;
   sig_api_release_comp *release_complete;
   sig_api_add_party *add_party;
   sig_api_add_party_ack *add_party_ack;
   sig_api_add_party_rej *add_party_rej;
   sig_api_drop_party *drop_party;
   sig_api_drop_party_ack *drop_party_ack;


   sig_msg = (sig_api_msg *)ptr;

   /* sanity check */
   if (!sig_msg) {
      if (atmSmap_debugAll || atmSmap_debugError)
         buginf("\nATMSM: callbackFunction, no sig_msg");
      return;
   }

   idb = sig_msg->idb;
 
   switch (sig_msg->opcode) {

      case ATM_LISTEN_NSAP:
         listen = &sig_msg->u.listen;
         atmSmap_rcvListen(listen, idb);
         break;

      case ATM_CANCEL_NSAP:
         cancel = &sig_msg->u.cancel;
         atmSmap_rcvCancel(cancel, idb);
         break; 

      case ATM_SETUP:
         setup = &sig_msg->u.setup;
         atmSmap_rcvSetup(setup, idb);
         break;

      case ATM_CONNECT:
         connect = &sig_msg->u.connect;
         atmSmap_rcvConnect(connect, idb);
         /* transmit a Connect Ack req to the Sig driver */
         break;

      case ATM_CONNECT_ACK:
         connect_ack = &sig_msg->u.connect_ack; 
         atmSmap_rcvConnectAck(connect_ack, idb); 
         break;

      case ATM_RELEASE:
         release = &sig_msg->u.release;
         atmSmap_rcvRelease(release, idb);
         break;

      case ATM_RELEASE_COMPLETE:
         release_complete = &sig_msg->u.release_comp;
         atmSmap_rcvReleaseComplete(release_complete, idb);
         break;

      case ATM_ADD_PARTY:
         add_party = &sig_msg->u.add_party;
         atmSmap_rcvAddParty(add_party, idb);
         break;

      case ATM_ADD_PARTY_ACK:
         add_party_ack = &sig_msg->u.add_party_ack;
         atmSmap_rcvAddPartyAck(add_party_ack, idb);
         break;

      case ATM_ADD_PARTY_REJ:
         add_party_rej = &sig_msg->u.add_party_rej;
         atmSmap_rcvAddPartyRej(add_party_rej, idb);
         break;

      case ATM_DROP_PARTY:
         drop_party = &sig_msg->u.drop_party;
         atmSmap_rcvDropParty(drop_party, idb);
         break;

      case ATM_DROP_PARTY_ACK:
         drop_party_ack = &sig_msg->u.drop_party_ack;
         atmSmap_rcvDropPartyAck(drop_party_ack, idb);
         break;


      default:
	 if (atmSmap_debugAll || atmSmap_debugError)
	   buginf("\nATMSM: callbackFunction, invalid event");
         break;

   }

   /* DON'T free sig_msg - it's being called off Sig driver's stack */

}


/*
 * a t m S m a p _ r c v L i s t e n 
 *
 * The Signalling driver returns Listen result to client 
 * via a callback function.
 */
void atmSmap_rcvListen (sig_api_listen *listen, idbtype *msg_idb)
{



  if (listen->result == ATM_SIGAPI_OK) {
    msg_idb->atm_nsap_valid = TRUE;

    if (atmSmap_debugAll || atmSmap_debugEvent)
      buginf("\nATMSM: listen success");

  } else {

    /* don't touch atm_nsap_valid flag */
    if (atmSmap_debugAll || atmSmap_debugEvent)
      buginf("\nATMSM: listen failed");
  }
  
}


/*
 * a t m S m a p _ r c v C a n c e l
 *
 * The Signalling driver returns Cancel result to client 
 * via a callback function. Set atm_nsap_valid to FALSE regardless 
 * of result.
 */
void atmSmap_rcvCancel (sig_api_cancel *cancel, idbtype *msg_idb)
{


  if (cancel->result == ATM_SIGAPI_OK) {
    msg_idb->atm_nsap_valid = FALSE;

    if (atmSmap_debugAll || atmSmap_debugEvent)
      buginf("\nATMSM: cancel success");

  } else {

    /* don't touch atm_nsap_valid flag */
    if (atmSmap_debugAll || atmSmap_debugEvent)
      buginf("\nATMSM: cancel failed");
  }
  
}

/*
 * atm_dynmap_on_if
 * Return an indication as to whether or not we are running a dynamic
 * address resolution protocol on this interface.
 */

boolean atm_dynmap_on_if (idbtype *swidb)
{
    if (is_atm_arp(swidb))
	return(TRUE);
    if (reg_invoke_nhrp_on_if(swidb))
	return(TRUE);
    return(FALSE);
}



/*
 * a t m S m a p _ r c v S e t u p
 *
 * The Signalling driver indicates an incoming Setup message to the Static 
 * Map client via a callback function.
 */
void atmSmap_rcvSetup (sig_api_setup *setup, idbtype *msg_idb)
{
   hwaddrtype temp;
   idbtype *idb;
   mapgroup_type *map_group;
   staticmap_type *map_ptr;
   sig_api_msg *sig_msg;
   sig_api_connect *connect;
   sig_api_release *release;
   atmSvcType *svc;
   boolean found;
   atm_params_type *params;
   boolean acceptCall;
   boolean aal5mux;
   int i;
   sig_api_cause_code cause;
   char nsapstr[MAX_ATM_NSAP_ASCII_LEN];
   char mtu_string[20];


   found      = FALSE;
   map_ptr    = NULL;
   params     = NULL;
   acceptCall = TRUE;
   aal5mux    = FALSE;

   cause = ATM_INCOMPAT_DEST;

   svc = (atmSvcType *)setup->call_id;

   /* extract partner ATM address */ 
   temp.type = STATION_ATMNSAP;
   temp.length = STATIONLEN_ATMNSAP;
   temp.target_type = TARGET_UNICAST;

   if (setup->multipoint) {
       if (atmSmap_debugAll || atmSmap_debugEvent)
	   buginf("\nATMMTP:rcvSETUP -- Multipoint");
   }
   bcopy(&setup->calling_party.addr, &temp.addr, STATIONLEN_ATMNSAP);

   /*
    * To accept this call, we must either be running a dynamic address
    * resolution protocol, or we must
    * have a static map entry for the caller.
    */
   if (atm_dynmap_on_if(msg_idb)) {
       if (svc->tos.aalType != ATM_AALIE_TYPE5 ||
	   svc->tos.sscsType != ATM_AALIE_SSCS_NULL) {
	   /*
	    * Aal5 Param ie has invalid content or we can not
	    * support, reject the call and send proper cause code.
	    */
	   cause = ATM_AAL_PARAMS_NOT_SUPP;
	   acceptCall = FALSE;
	   /* aal5 params ok so check mtu sizes */
       } else {
	   /* Check if the incoming call's forward and backward pdu sizes
	    * are the same as our interface mtu size.
	    * If not check whether we are configured to reject calls or
	    * merely to print a warning and accept the call.
	    */
	   if ((svc->tos.fwdSduSize != (msg_idb->sub_mtu +
					ATM_SNAP_HDRBYTES)) ||
	       (svc->tos.bwdSduSize != (msg_idb->sub_mtu +
					ATM_SNAP_HDRBYTES))) {
	       /* configured to reject the call */
	       if (msg_idb->hwptr->atm_db->mtu_reject_call) {
		   atm_printnsap(temp.addr, nsapstr);
  		   sprintf (mtu_string,"(MTU + %d) is %d",ATM_SNAP_HDRBYTES,
  			  (ATM_SNAP_HDRBYTES + msg_idb->sub_mtu));
  		   errmsg(&msgsym(MTUCALLMISMATCH,ATM), nsapstr,
   			  svc->tos.fwdSduSize, svc->tos.bwdSduSize,
			  msg_idb->namestring,
 			  mtu_string,
  			  " -  Call Rejected");
		   cause = ATM_AAL_PARAMS_NOT_SUPP;
		   acceptCall = FALSE;
		   /* ignoring mtu mismatches */
	       } else {
		   acceptCall = TRUE;
		   atm_printnsap(temp.addr, nsapstr);
  		   sprintf (mtu_string,"(MTU + %d) is %d",ATM_SNAP_HDRBYTES,
  			  (ATM_SNAP_HDRBYTES + msg_idb->sub_mtu));
  		   errmsg(&msgsym(MTUCALLMISMATCH,ATM), nsapstr,
  			  svc->tos.fwdSduSize,
 			  svc->tos.bwdSduSize,
 			  msg_idb->namestring,
  			  mtu_string,
  			  " - Call Accepted");
	       }
	       /* mtu sizes match */
	   } else {
	       acceptCall = TRUE;
	   }
       }
       /* not arp static maps */
   } else {

       /* if aal5mux, blli parsed in notifySetup, set aal5mux boolean here */
       if (setup->blli[0].length >= ATMSIG_LLAYER3_LENGTH) {
	   aal5mux = TRUE;
       }

       /* destination ATM address configured on static maps? */       
       FOR_ALL_SWIDBS_ON_HW(msg_idb->hwptr, idb) {
	   FOR_ALL_MAPGROUPS_ON_IDB(idb, map_group) {
	       FOR_ALL_MAPS_IN_LIST(i, map_group->maplist, map_ptr) {

		   /* proceed only if address match */
		   if ( bcmp(map_ptr->hwconf.addr, temp.addr,
			     STATIONLEN_ATMNSAP) != 0)
		       continue;
		   /*
		    * get params from static map or class block or
		    * default class
		    */
		   params = atmSmap_get_params(msg_idb, map_ptr);

		   if (!params) {
		       if (atmSmap_debugAll || atmSmap_debugError)
			   buginf("\nATMSM: rcvSetup, params block not found");
		       acceptCall = FALSE;
		       goto result;
		   }

		   /* verify map encap matches call */
		   if ( (!svc->multiPoint) && (params->aal5mux != aal5mux) )
		       continue;

		   /* if aal5mux, test protocol */
		   if ( (!svc->multiPoint) && (aal5mux) ) {
		       if (map_ptr->protocol != svc->linktype) {
			   continue;
		       }
		   }
		   found = TRUE;
		   break;

	       } /* swidbs */
	   } /* mapgroups */
       } /* maps */

       /* static map block found? */
       if (!found) {
	   if (atmSmap_debugAll || atmSmap_debugError) {
	       buginf("\nATMSM: rcvSetup: map block not found");
	       buginf("\nATMSM: ");
	       for (i = 0; i < STATIONLEN_ATMNSAP; ++i)
		   buginf("%x ", temp.addr[i]);
	   }
	   acceptCall = FALSE;
	   goto result; 
       }

       if (svc->tos.aalType != ATM_AALIE_TYPE5)
	 goto result;

       /* aal5mux has a different pdu size than snap */
       if (aal5mux) {
	   if ((svc->tos.fwdSduSize != msg_idb->sub_mtu) ||
	       (svc->tos.bwdSduSize != msg_idb->sub_mtu)) {
	       /* are we configured to reject calls */
	       if (msg_idb->hwptr->atm_db->mtu_reject_call) {
		   atm_printnsap(temp.addr, nsapstr);
 		   sprintf (mtu_string,"(MTU) is %d",
 			    msg_idb->sub_mtu);
		   errmsg(&msgsym(MTUCALLMISMATCH,ATM), nsapstr,
			  svc->tos.fwdSduSize,
			  svc->tos.bwdSduSize,
			  msg_idb->namestring,
			  mtu_string,
			  " - Call Rejected");
		   acceptCall = FALSE;
	       } else {
		   acceptCall = TRUE;
		   atm_printnsap(temp.addr, nsapstr);
		   sprintf (mtu_string,"(MTU) is %d",
 			    msg_idb->sub_mtu);
		   errmsg(&msgsym(MTUCALLMISMATCH,ATM), nsapstr,
			  svc->tos.fwdSduSize,
			  svc->tos.bwdSduSize,
			  msg_idb->namestring,
			  mtu_string,
			  " - Call Accepted");
	       }
	   }
       } else {

	   /* aal5snap header */
	   if ((svc->tos.fwdSduSize != (msg_idb->sub_mtu +
					ATM_SNAP_HDRBYTES))
	       || (svc->tos.bwdSduSize !=
		   (msg_idb->sub_mtu + ATM_SNAP_HDRBYTES))) {
	       /* are configured to reject calls or merely warn */
	       if (msg_idb->hwptr->atm_db->mtu_reject_call) {
		   atm_printnsap(temp.addr, nsapstr);
  		   sprintf (mtu_string,"(MTU + %d) is %d",ATM_SNAP_HDRBYTES,
  			  (ATM_SNAP_HDRBYTES + msg_idb->sub_mtu));
  		   errmsg(&msgsym(MTUCALLMISMATCH,ATM), nsapstr,
  			  svc->tos.fwdSduSize,
 			  svc->tos.bwdSduSize,
 			  msg_idb->namestring,
 			  mtu_string,
  			  " - Call Rejected");
		   acceptCall = FALSE;
	       } else {
		   acceptCall = TRUE;
		   atm_printnsap(temp.addr, nsapstr);
  		   sprintf (mtu_string,"(MTU + %d) is %d",ATM_SNAP_HDRBYTES,
  			  (ATM_SNAP_HDRBYTES + msg_idb->sub_mtu));
  		   errmsg(&msgsym(MTUCALLMISMATCH,ATM), nsapstr,
  			  svc->tos.fwdSduSize,
 			  svc->tos.bwdSduSize,
 			  msg_idb->namestring,
 			  mtu_string,
  			  " - Call Accepted");
	       }
	   }
       }
   }

result:
   /* get memory to respond to Setup message */ 
   sig_msg = malloc( sizeof(sig_api_msg) );

   /*
    * If we are out of memory then the Signalling state machine will timeout
    * and send a Release message to it's partner.   We should clean out the
    * call_id from the params block.
    */
   if (sig_msg == NULL) {
     return;
   }

   /* tx Connect request to Signalling driver IF dest ATM addr found */
   if (acceptCall) {
      sig_msg->opcode = ATM_CONNECT;
      connect = (sig_api_connect *)&(sig_msg->u.connect);
      connect->call_id = setup->call_id;
      if (aal5mux)
	connect->flags = ATM_VC_AAL5MUX; 
      else
	connect->flags = ATM_VC_AAL5SNAP; 
   } else {
      /* dest ATM address not found in static map - send a Release request */
      sig_msg->opcode = ATM_RELEASE;
      release = (sig_api_release *)&(sig_msg->u.release);
      release->call_id = setup->call_id;
      release->cause = cause;
   }

   /* send to Signalling driver */
   atmSmap_sendToSigDriver(sig_msg);
}

 
 
/*
 * a t m S m a p _ r c v C o n n e c t
 *
 * The Signalling driver automatically sends a Conenct Ack message upon
 * receipt of a Connect message (assuming it could open a vc with the
 * ATM driver).  So we only need to update our map lists.
 */
void atmSmap_rcvConnect (sig_api_connect *connect, idbtype *idb)
{

   atmSvcType *svc;
   boolean result;

   svc = (atmSvcType *) connect->call_id;

   if (!svc->multiPoint) 
     result = atmSmap_staticMapUpdate(connect->call_id, idb);
   else 
     result = atmSmap_staticMapMtpUpdate(connect->call_id, idb);

   if (is_atm_arp_server(idb))
       reg_invoke_atm_arp_serv_connect_reg(idb, connect->vcd);
}

 
 
/*
 * a t m S m a p _ r c v C o n n e c t A c k
 *
 * Simply update the static map list.
 */
void atmSmap_rcvConnectAck (sig_api_connect_ack *connect_ack, idbtype *idb)
{

   atmSvcType *svc;
   boolean result;

   svc = (atmSvcType *) connect_ack->call_id;

   if (!svc->multiPoint)
     result = atmSmap_staticMapUpdate(connect_ack->call_id, idb);
   else 
     result = atmSmap_staticMapMtpUpdate(connect_ack->call_id, idb);

   if (is_atm_arp_server(idb))
       reg_invoke_atm_arp_serv_connect_ack_reg(idb, svc->vcnum);
}

 
 
/*
 * a t m S m a p _ r c v R e l e a s e
 *
 * Received a Release notification from the Signalling driver.  Clean up
 * the static map list entry and send a Release Complete request to the
 * Sig driver.
 */
void atmSmap_rcvRelease (sig_api_release *release, idbtype *idb)
{
   sig_api_msg *sig_msg;
   sig_api_release_comp *rel_comp;
   atmSvcType *svc;

   svc = (atmSvcType *) release->call_id;

   if (is_atm_arp_server(idb))
       reg_invoke_atm_arp_serv_disconnect_reg(idb, svc->vcnum);

   /* free static map entry list */
   if (!svc->multiPoint)
     atmSmap_staticMapDelete(release->call_id, idb);
   else
     atmSmap_staticMapMtpDelete(release->call_id, idb);

   /* get memory to respond to Release indication */
   sig_msg = malloc( sizeof(sig_api_msg) );

   /*
    * If we are out of memory then the Signalling state machine will timeout
    * and send a Release Complete message to it's partner.
    */
   if (sig_msg == NULL) {
     return;
   }

   /* form Release Complete request */
   sig_msg->opcode = ATM_RELEASE_COMPLETE;
   rel_comp = (sig_api_release_comp *)&(sig_msg->u.release_comp);
   rel_comp->call_id = release->call_id;

   /* send to Signalling driver */
   atmSmap_sendToSigDriver(sig_msg);
   
}

 
 
/*
 * a t m S m a p _ r c v R e l e a s e C o m p l e t e
 *
 * Simply free static map entry list.
 */
void atmSmap_rcvReleaseComplete (sig_api_release_comp *release_comp,
				 idbtype *idb)
{
   atmSvcType *svc;

   svc = (atmSvcType *) release_comp->call_id;

   if (is_atm_arp_server(idb))
       reg_invoke_atm_arp_serv_disconnect_reg(idb, svc->vcnum);

   /* free static map entry list */
   if (!svc->multiPoint)
     atmSmap_staticMapDelete(release_comp->call_id, idb);
   else
     atmSmap_staticMapMtpDelete(release_comp->call_id, idb);

}


/* Multipoint messages start here */

/*
 * a t m S m a p _ r c v A d d P a r t y
 *
 * Received an AddParty notification from the Signalling driver.
 * Add new party to call if call active and NSAP valid. 
 */
void atmSmap_rcvAddParty (sig_api_add_party *add_party, idbtype *msg_idb)
{


   hwaddrtype temp;
   idbtype *idb;
   staticmap_type *map_ptr;
   sig_api_msg *sig_msg;
   sig_api_add_party_ack *add_party_ack;
   sig_api_add_party_rej *add_party_rej;
   atmSvcType *call_id;
   boolean found;
   boolean acceptCall;
   int i;


   found = FALSE;
   acceptCall = FALSE;
   map_ptr = NULL;

   /* extract ATM address of partner */ 
   /* called_party is actually calling party when we receive add_party */
   temp.type = STATION_ATMNSAP;
   temp.length = STATIONLEN_ATMNSAP;
   temp.target_type = TARGET_UNICAST;
   bcopy(&add_party->called_party.addr, &temp.addr, STATIONLEN_ATMNSAP);

   /* AddParty checks already done. At this point we should have an active,
    * remotely initiated, multipoint call. Verify we have a static map entry 
    * for this party, but don't update static map. Use same VC as existing 
    * call. Don't compare addr, only hwaddr in map entry.
    */
  
   call_id = add_party->call_id;

   /* destination ATM address configured on static maps? */
   FOR_ALL_SWIDBS_ON_HW(msg_idb->hwptr, idb) {
 
      if (!idb->mapgroup)
         continue;

      /* scan the static map table for the nsap address */
      map_ptr = staticmap_hwlookup(idb, &temp, ET_ATM, FALSE, LINK_ILLEGAL);
 
      if (map_ptr) {
         found = TRUE;
         break;
      }

   }

   /* static map block found? */
   if (!found) {
      if (atmSmap_debugAll || atmSmap_debugEvent) {
         buginf("\nATMSM: rcvAddParty: map block not found");
         buginf("\nATMSM: ");
         for (i = 0; i < STATIONLEN_ATMNSAP; ++i)
            buginf("%x ", temp.addr[i]);
      }
   }

   /* Get memory to respond to AddParty message */ 
   sig_msg = malloc( sizeof(sig_api_msg) );

   /*
    * If we are out of memory then the Signalling state machine will timeout
    * and send a AddPartyRej message to it's partner.
    */
   if (sig_msg == NULL) {
     return;
    }

   /* tx AddPartyAck request to Signalling driver IF dest ATM addr found */
   if (found) {
      sig_msg->opcode = ATM_ADD_PARTY_ACK;
      add_party_ack = (sig_api_add_party_ack *)&(sig_msg->u.add_party_ack);
      add_party_ack->call_id = add_party->call_id;
      add_party_ack->endpoint_id = add_party->endpoint_id;
   }
   else {
      /* dest ATM address not found in Static map - send a AddPartyRej req */
      sig_msg->opcode = ATM_ADD_PARTY_REJ;
      add_party_rej = (sig_api_add_party_rej *)&(sig_msg->u.add_party_rej);
      add_party_rej->call_id = add_party->call_id;
      add_party_rej->endpoint_id = add_party->endpoint_id;
      add_party_rej->cause = ATM_INCOMPAT_DEST;
   }

   /* send it to the Signalling driver */
   process_enqueue(atmSig_outputQ, sig_msg);

   return;

 
}



/*
 * a t m S m a p _ r c v A d d P a r t y A c k 
 *
 * Received an AddPartyAck notification from the Signalling driver.
 * Add new party to call. Update party static map.
 */
void atmSmap_rcvAddPartyAck (sig_api_add_party_ack *add_party_ack, 
			     idbtype *idb)
{

  atmSmap_staticMapPartyUpdate(add_party_ack->call_id, 
					 add_party_ack->endpoint_id, idb);
 
}



/*
 * a t m S m a p _ r c v A d d P a r t y R e j
 *
 * Received an AddPartyRej notification from the Signalling driver.
 * Party not added to call. Nothing to do.
 */
void atmSmap_rcvAddPartyRej (sig_api_add_party_rej *add_party_rej, 
			     idbtype *idb)
{

   /* remove party from map entry */
   atmSmap_staticMapPartyDelete(add_party_rej->call_id, 
				add_party_rej->endpoint_id, idb);
 
}



/*
 * a t m S m a p _ r c v D r o p P a r t y
 *
 * Received an DropParty notification from the Signalling driver.
 * Drop Party from call and send DropPartyAck.
 */
void atmSmap_rcvDropParty (sig_api_drop_party *drop_party, idbtype *idb)
{

   sig_api_msg *sig_msg;
   sig_api_drop_party_ack *drop_party_ack;


   /* Get memory to respond to DropParty indication */
   sig_msg = malloc( sizeof(sig_api_msg) );

   /*
    * If we are out of memory then the Signalling state machine will timeout
    * and send a DropPartyAck message to it's partner.
    */
   if (sig_msg == NULL) {
     return;
   }

   /* remove party from map entry */
   atmSmap_staticMapPartyDelete(drop_party->call_id, drop_party->endpoint_id, idb);

   /* form DropPartyAck request */
   sig_msg->opcode = ATM_DROP_PARTY_ACK;
   drop_party_ack = (sig_api_drop_party_ack *)&(sig_msg->u.drop_party_ack);
   drop_party_ack->call_id = drop_party->call_id;
   drop_party_ack->endpoint_id = drop_party->endpoint_id;

   /* send to Signalling driver */
   atmSmap_sendToSigDriver(sig_msg);

 
}



/*
 * a t m S m a p _ r c v D r o p P a r t y A c k
 *
 * Received an DropPartyAck notification from the Signalling driver.
 * Delete dpParty from party static map
 */
void atmSmap_rcvDropPartyAck (sig_api_drop_party_ack *drop_party_ack, 
			      idbtype *idb)
{

   /* remove party from map entry */
   atmSmap_staticMapPartyDelete(drop_party_ack->call_id, drop_party_ack->endpoint_id, idb);

 
}



/*
 * a t m S m a p _ s t a t i c M a p U p d a t e
 *
 * Enter vcnum in all static map blocks which point to the destination
 * ATM address.  Returns TRUE if vcnum entered, FALSE otherwise.
 * This is for PTP calls only
 */
boolean atmSmap_staticMapUpdate (void *call_id, idbtype *idb)
{

   staticmap_type *map_ptr;
   mapgroup_type *map_group;
   atmSvcType *svc;
   hwaddrtype temp;
   atm_params_type *params;
   int i;
   atmSvcPartyType *party;
   boolean result = FALSE;

   svc = (atmSvcType *)call_id;
   party = svc->rootParty;

   map_ptr = NULL;

   /* extract dest ATM address */
   temp.type = STATION_ATMNSAP;
   temp.length = STATIONLEN_ATMNSAP;
   temp.target_type = TARGET_UNICAST;
   bcopy(&party->destNsap.addr, &temp.addr, STATIONLEN_ATMNSAP); 

   /* enter vcnum in all static maps pointing to the nsap address */
   FOR_ALL_MAPGROUPS_ON_IDB(idb, map_group) {
      FOR_ALL_MAPS_IN_LIST(i, map_group->maplist, map_ptr) {

	/* proceed only if address match */
	if ( bcmp(map_ptr->hwconf.addr, temp.addr, STATIONLEN_ATMNSAP) != 0 )
	  continue;

	/* get params from static map or class block or default class for i/f  */
         params = atmSmap_get_params(idb, map_ptr);

         if (atmSmap_debugAll || atmSmap_debugEvent)
            buginf("\nsvc->linktype = %x, map_ptr->protocol = %x",
                                   svc->linktype, map_ptr->protocol);

	/* proceed only if this params matches the svc */
	if (params->aal5mux != svc->aal5mux)
	  continue;

	if (svc->aal5mux) {

	  if (svc->linktype == map_ptr->protocol) {
	    atmSig_enterVcnumInMap(svc, map_ptr);
	    return (TRUE);
	  }
	  else
	    continue;
	} else {
	  if ((map_ptr->protocol != LINK_ARP) || (map_ptr->protocol == LINK_ARP
			&& map_ptr->hwaddr.type == STATION_ILLEGAL)) {
	      atmSig_enterVcnumInMap(svc, map_ptr);
	      result = TRUE;
	  }
	}
      }
    }

   return (result);

}



/*
 * a t m S i g _ e n t e r V c n u m I n M a p ( )
 *
 * PTP only
 */
static void
atmSig_enterVcnumInMap (atmSvcType *svc, staticmap_type *map_ptr)
{

   atm_params_type *params, *ptr_to_smap_params;
   atmSig_smap *smap;
   short cur_vcnum;

   /*
    * get correct qos structure - if no qos configured get if directly
    * off static map, otherwise get it off the class struct or the default class
    * for the interface.
    */
    params = atmSmap_get_params(svc->idb, map_ptr);

   /*
    * if local side trying to establish connection, don't overwrite
    * the map block.  The UNI spec does not mention how to handle
    * call collision - let both sides open their connections and
    * let them send data on their respective svc's.
    */

   ptr_to_smap_params = map_ptr->params;
   smap = (atmSig_smap *)ptr_to_smap_params->smap;
 
   if ( smap && (svc->locallyInitiatedCall == FALSE) ) {
     if (atmSmap_debugAll || atmSmap_debugEvent)
         buginf("\nATMSM: enterVcnumInMap: map blk not updated, vc %d", svc->vcnum);
     reg_invoke_ip_atm_staticmap_connect(map_ptr);
     return;
   }

   /* no smap here means remotely initiated PTP call, no collision */
   if(!smap) {
     smap = malloc_named( sizeof(atmSig_smap), "ATMSIG-SMAP");

     if (smap) {
       ptr_to_smap_params->smap = smap;
       smap->locallyInitiatedCall = svc->locallyInitiatedCall;
       smap->call_id = svc;
     }

     else {
       return;
     } 
   }

   if (atmSmap_debugAll || atmSmap_debugEvent) 
     buginf("\nenterVcnumInMap, map_ptr %x, params %x, smap %x, call_id %x", 
	  map_ptr, params, smap, smap->call_id);

   if (atmSmap_debugAll || atmSmap_debugEvent) 
     buginf("\nATMSM: updating map block with vc %d", svc->vcnum);

   /* check map ptr for existing VC, if so, return */
   cur_vcnum = (short) map_ptr->hwaddr.addr[0];
   if (cur_vcnum)
     return;

   if (!map_ptr->sm_swidb)
       map_ptr->sm_swidb = svc->idb;
   atmSig_stuff_vcd_in_map(map_ptr, svc->vcnum);
   reg_invoke_ip_atm_staticmap_connect(map_ptr);
}

/*
 * atmSig_stuff_vcd_in_map
 *
 * Enter the VCD into the passed map, and populate the ARP Table
 */
static void
atmSig_stuff_vcd_in_map (staticmap_type *map_ptr, vcd_t vcd)
{
    map_ptr->hwaddr.type = STATION_ATMVC;
    map_ptr->hwaddr.length = STATIONLEN_ATMVC;
    map_ptr->hwaddr.addr[0] = (uchar)(vcd >> 8);
    map_ptr->hwaddr.addr[1] = (uchar)vcd;

    /*
     * Add this into the ARP table...
     */
    if (map_ptr->protocol == LINK_IP) {
    	arp_table_add(map_ptr->sm_swidb, 
		      (long)map_ptr->addr.addr.ip_address, map_ptr->protocol, 
		      &map_ptr->hwaddr, ET_ATM, ARP_DYNAMIC);
	GET_TIMESTAMP(map_ptr->update_time);
    }
}



/*
 * a t m S i g _ s t a t i c M a p D e l e t e ( )
 * 
 * Delete vcd from from static map entry.
 * PTP only
 */
void atmSmap_staticMapDelete (void *call_id, idbtype *idb)
{

   atmSvcType *svc;
   hwaddrtype temp;
   staticmap_type *map_ptr;
   mapgroup_type *map_group;
   atm_params_type *params, *ptr_to_smap_params;
   int i;
   atmSig_smap *smap;
   atmSvcPartyType *party;


   svc = (atmSvcType *)call_id;
   party = svc->rootParty;

   temp.type = STATION_ATMNSAP;
   temp.length = STATIONLEN_ATMNSAP;
   temp.target_type = TARGET_UNICAST;
   bcopy(&party->destNsap.addr, &temp.addr, STATIONLEN_ATMNSAP);

   if (!svc->idb) {
      if (atmSmap_debugAll || atmSmap_debugError)
         buginf("\nATMSM: staticMapDelete - no idb for svc");
   }

   /* remove vc number and free smap from all static maps pointing to the 
    *dest nsap addr 
    */
   FOR_ALL_MAPGROUPS_ON_IDB(idb, map_group) {
      FOR_ALL_MAPS_IN_LIST(i, map_group->maplist, map_ptr) {

	 /* proceed only if the VCDs match */
	 if (*(vcd_t *)map_ptr->hwaddr.addr != svc->vcnum)
	     continue;
	 
   	if (atmSmap_debugAll || atmSmap_debugEvent) 
     	    buginf("\nstaticMapDelete, map_ptr %x call_id %x"
		" VCD %d Protocol %d Addr %i", 
	  	map_ptr, call_id, *(vcd_t *)map_ptr->hwaddr.addr, 
		map_ptr->protocol, map_ptr->addr.ip_addr);

	 /*
          * get correct qos structure - if no qos configured get if directly
          * off static map, otherwise get it off the class struct or the
	  * default map class for the i/f.
          */
         params = atmSmap_get_params(idb, map_ptr);

	 /*
	  * Remove from ARP table
	  */
	 switch (map_ptr->protocol) {
	 case LINK_IP:
	     arp_table_remove(map_ptr->addr.addr.ip_address, map_ptr->protocol);
	     if (params->broadcast) {
		 reg_invoke_ip_invalidate_mcache(idb);
	     }
	     break;
	 case LINK_ARP:		
	     /*
	      * For LINK_ARP, we must flush all entries, because the ARP
	      * server is gone.  Entries are not reliable.
	      */
	     arp_flush_cache(map_ptr->sm_swidb);
	     break;
	 }

	 /* proceed only if this params matches the svc */
	 if (params->aal5mux != svc->aal5mux)
	   continue;

          ptr_to_smap_params = map_ptr->params;

         if ( svc->aal5mux ) {
	   if (svc->linktype == map_ptr->protocol) {
	     map_ptr->hwaddr.type    = STATION_ILLEGAL;
	     map_ptr->hwaddr.length  =
	     map_ptr->hwaddr.addr[0] =
	     map_ptr->hwaddr.addr[1] = 0;
	     map_ptr->sm_swidb = NULL;

	     /* remove smap block from static map */
	     if (!ptr_to_smap_params)
	       return;

	     smap = ptr_to_smap_params->smap;
	     if (smap)
	       free(smap);
	     ptr_to_smap_params->smap = NULL;
	     return;
	   }
	   else 
	     continue;
	 }

	 else {
	   map_ptr->hwaddr.type    = STATION_ILLEGAL;
	   map_ptr->hwaddr.length  =
	   map_ptr->hwaddr.addr[0] =
	   map_ptr->hwaddr.addr[1] = 0;

	   /* remove smap block from static map */
	   if (!ptr_to_smap_params)
	     continue;

	   smap = ptr_to_smap_params->smap;
	   if (smap)
	     free(smap);
	   ptr_to_smap_params->smap = NULL;
	 }
       }
    }

   return;

}



/*
 * a t m S m a p _ s e n d T o S i g D r i v e r 
 *
 * Sends request to the Signalling Driver.
 */
void atmSmap_sendToSigDriver (sig_api_msg *msg) 
{

   /* sanity check */
   if (!msg) {
      if (atmSmap_debugAll || atmSmap_debugError)
         buginf("\nATMSM: sendToSigDriver: null message");
      return;
   }

   process_enqueue(atmSig_outputQ, msg);

}



/* Multipoint routines start here */
/* 
 * a t m S m a p _  o p e n S v c M t p C o n n e c t i o n ( )
 *
 * Issues a request to the Sig driver to open a svc multipoint 
 * connection to the requested destination.
 *
 */
boolean atmSmap_openSvcMtpConnection (idbtype *idb, staticmap_type *map)
{

   atmSvcType *call_id;
   atm_params_type *params, *ptr_to_smap_params;
   sig_api_msg *sig_msg;
   sig_api_setup *setup_msg;
   atmSig_smap *bc_smap;




   /* valid output ATM port? */
   if (!idb || !(idb->hwptr->status & IDB_ATM) ) {
      if (atmSmap_debugAll || atmSmap_debugError)
         buginf("\nATMSM: no output port, mtp open req failed");
      return (FALSE);
   }

   /* ATM Signalling subsystem initialized? */
   if (!atmsig_interface_up(idb)) {
      if (atmSmap_debugAll || atmSmap_debugError) 
         buginf("\nATMSM: subsystem init not complete, mtp open req failed");
      return (FALSE);
   }

   /* valid nsap prefix? */
   if (!idb->nsap || (atmSig_nullPrefix(idb->nsap->addr)) ) {
      if (atmSmap_debugAll || atmSmap_debugError)
         buginf("\nATMSM: nullPrefix, mtp open req failed");
      return (FALSE);
   }

   /* valid map block? */
   if (!map) {
      if (atmSmap_debugAll || atmSmap_debugError)
         buginf("\nATMSM: null map control block, mtp open req failed");
      return (FALSE);
   }

   /* Get the correct params structure - static map or class or default class */
   params = atmSmap_get_params(idb, map);

   if (!params) {
      if (atmSmap_debugAll || atmSmap_debugError)
         buginf("\nATMMTP: params block not found, open req failed");
      return (FALSE);
   }

   ptr_to_smap_params = map->params;

   if (!ptr_to_smap_params) {
      if (atmSmap_debugAll || atmSmap_debugError)
         buginf("\nATMMTP: map params block not found, open req failed");
      return (FALSE);
   }

   /*
    * A bc_smap exists?  A presence of bc_smap in the static map means 
    * that a request to establish MTP call is already pending.  
    * Let the previous request complete before issuing another one.
    */
   if (ptr_to_smap_params->bc_smap) {
     if (atmSmap_debugAll || atmSmap_debugEvent)
        buginf("\nATMMTP: bc_smap exists");    
     return (FALSE);
   }
   else {
     bc_smap = malloc_named( sizeof(atmSig_smap), "ATMSIG-SMAP");
     if (bc_smap == NULL) {
       return (FALSE);
     }
   }

   /* get a call_id */
   call_id = atmSig_api_allocate_call_id();
   if (!call_id) {
      if (atmSmap_debugAll || atmSmap_debugError)
         buginf("\nATMMTP: no mem to alloc call_id block");
      ptr_to_smap_params->bc_smap = NULL;
      free(bc_smap);
      return (FALSE);
   }

   /* stuff idb in map->idb */
   map->sm_swidb = idb;

   ptr_to_smap_params->bc_smap = bc_smap;
   bc_smap->call_id = call_id;   
   bc_smap->locallyInitiatedCall = TRUE;
   
   /* save destination atm address in bc_smap block */
   bcopy(map->hwconf.addr, bc_smap->destAtmAddr.addr, STATIONLEN_ATMNSAP);
   bc_smap->destAtmAddr.type = STATION_ATMNSAP;
   bc_smap->destAtmAddr.length = STATIONLEN_ATMNSAP;
   bc_smap->destAtmAddr.target_type = TARGET_UNICAST;

   /* save source atm address in bc_smap block */
   bcopy(idb->nsap->addr, bc_smap->srcAtmAddr.addr, STATIONLEN_ATMNSAP);
   bc_smap->srcAtmAddr.type = STATION_ATMNSAP;
   bc_smap->srcAtmAddr.length = STATIONLEN_ATMNSAP;
   bc_smap->srcAtmAddr.target_type = TARGET_UNICAST;

   /* send request to the Signalling driver to open MTP svc */

   /* get memory */
   sig_msg = malloc( sizeof(sig_api_msg) );
   if (sig_msg == NULL) {
     ptr_to_smap_params->bc_smap = NULL;
     free(bc_smap);
     free(call_id->rootParty); 
     free(call_id);
     return (FALSE);
   }

   /* Check aal5mux */
   if (params->aal5mux) {
     bc_smap->linktype = map->protocol;
     call_id->aal5mux = TRUE;
     call_id->linktype = map->protocol;
   }         

   /* fill in sig_api_setup block */

   sig_msg->opcode = ATM_SETUP;
   sig_msg->idb    = idb; 

   setup_msg = (sig_api_setup *)&sig_msg->u.setup;
   setup_msg->call_id = call_id;

   /* 
    * provide a call back function for Sig driver to notify client of
    * incoming events.
    */ 
   setup_msg->sig_user_callback = atmSmap_callbackFunction;

   /* copy Type of Service parameters in message */ 
   bcopy( params, &setup_msg->params, sizeof(atm_params_type) ); 

   /* fill in destination ATM address in message */
   bcopy(bc_smap->destAtmAddr.addr, setup_msg->called_party.addr, 
                                                    STATIONLEN_ATMNSAP);
   setup_msg->called_party.type = STATION_ATMNSAP;
   setup_msg->called_party.length = STATIONLEN_ATMNSAP;
   setup_msg->called_party.target_type = TARGET_UNICAST;

   /* fill in source ATM address in message */
   bcopy(idb->nsap->addr, setup_msg->calling_party.addr, STATIONLEN_ATMNSAP);
   setup_msg->calling_party.type = STATION_ATMNSAP;
   setup_msg->calling_party.length = STATIONLEN_ATMNSAP;
   setup_msg->calling_party.target_type = TARGET_UNICAST;

   /* fill in Broadband Lower Layer info */
   /* snap vc? */
   if (!params->aal5mux) {
      setup_msg->blli[0].length = 1;
      setup_msg->blli[0].string[0] = 0xCC;  

      setup_msg->blli[1].length = 0; 
      setup_msg->blli[2].length = 0;
     
      setup_msg->flags = ATM_VC_AAL5SNAP; 
   }
   else {
     atmSig_fillBroadbandLowLayerInfo(map, setup_msg);
     setup_msg->flags = ATM_VC_AAL5MUX; 
   }

   /* set multipoint flag in setup msg */
   setup_msg->multipoint = TRUE;

   /* send to Signalling driver */
   atmSmap_sendToSigDriver(sig_msg);

   return (TRUE);

}



/*
 * a t m S m a p _ s t a t i c M a p M t p U p d a t e
 *
 * Enter vcnum in the static map block for this call_id.
 * Returns TRUE if vcnum entered, FALSE otherwise.
 * This is for MTP calls only
 */
boolean atmSmap_staticMapMtpUpdate (void *call_id, idbtype *idb)
{

   staticmap_type *map_ptr;
   mapgroup_type *map_group;
   atmSvcType *svc;
   hwaddrtype temp;
   atm_params_type *params,*ptr_to_smap_params;
   int i;
   atmSvcPartyType *party;
   atmSig_smap *bc_smap;
   boolean result = FALSE;

   svc = (atmSvcType *)call_id;
   party = svc->rootParty;

   map_ptr = NULL;

   /* already have VC, don't overwrite map for remote MTP call */
   if (!svc->locallyInitiatedCall)
     return (FALSE);

   /* extract dest ATM address */
   temp.type = STATION_ATMNSAP;
   temp.length = STATIONLEN_ATMNSAP;
   temp.target_type = TARGET_UNICAST;
   bcopy(&party->destNsap.addr, &temp.addr, STATIONLEN_ATMNSAP); 

   /* enter vcnum in static map bc_hwaddr pointing to the nsap address
    * for this MTP call only. verify nsap, protocol, call_id
    */
   FOR_ALL_MAPGROUPS_ON_IDB(idb, map_group) {
      FOR_ALL_MAPS_IN_LIST(i, map_group->maplist, map_ptr) {

	/* verify nsap address match */
	if ( bcmp(map_ptr->hwconf.addr, temp.addr, STATIONLEN_ATMNSAP) != 0 ) 
	  continue;

	/* get correct params structure - static map or class or default class */
        params = atmSmap_get_params(idb, map_ptr);

	ptr_to_smap_params = map_ptr->params;
        
	/* verify bc_smap */
	bc_smap = ptr_to_smap_params->bc_smap;
	
	if (!bc_smap) 
	  continue;

	/* verify call_id */
	if (bc_smap->call_id != call_id) 
	  continue;

	if (atmSmap_debugAll || atmSmap_debugEvent) 
	  buginf("\nstaticMapMTPUpdate: map_ptr %x, params %x, bc_smap %x, 
                   bc_call_id %x", map_ptr, params, bc_smap, bc_smap->call_id);

	if (atmSmap_debugAll || atmSmap_debugEvent) 
	  buginf("\nATMMTP: updating MTP map block with vc %d", svc->vcnum);
	
	/* here's the equivalent of enterVcnumInMap */
	map_ptr->atm_map->bc_hwaddr.type = STATION_ATMVC;
	map_ptr->atm_map->bc_hwaddr.length = STATIONLEN_ATMVC;
	map_ptr->atm_map->bc_hwaddr.addr[0] = (uchar)(svc->vcnum >> 8);
	map_ptr->atm_map->bc_hwaddr.addr[1] = (uchar)svc->vcnum;
	if (!map_ptr->sm_swidb)
	  map_ptr->sm_swidb = svc->idb;

	/* set state in map */
	map_ptr->atm_map->map_linkstate = LINK_ACTIVE;
	map_ptr->atm_map->map_partystate = PARTY_ACTIVE;

	reg_invoke_ip_atm_staticmap_connect(map_ptr);
	result = TRUE;
      }
    }
   return (result);

 }


/*
 * a t m S m a p _  o p e n P a r t y C o n n e c t i o n ( )
 *
 * Link up, request new party connection to same VC
 * 
 */
boolean atmSmap_openPartyConnection (idbtype *idb, staticmap_type *map,
				     void *call_id)
{
   
   atmSvcType *svc;
   atmSvcPartyType *endpoint_id;
   atm_params_type *params, *ptr_to_smap_params;
   sig_api_msg *sig_msg;
   sig_api_add_party *add_party_msg;
   atmSig_smap *bc_smap = NULL;


  svc = (atmSvcType *)call_id;

   /* valid svc block? */
   if (svc && !atmSig_validateSvc(svc) ) {
      if (atmSmap_debugAll || atmSmap_debugError)
         buginf("\nATMMTP: openPartyConnection: svc invalid");
      return (FALSE);
    }

   /* call active? */
   if (svc->state != ATMSIG_ST_ACTIVE) {
      if (atmSmap_debugEvent)
         buginf("\nATMMTP: openPartyConnection: svc not active");
      return (FALSE);
    }

   /* valid map block? */
   if (!map) {
      if (atmSmap_debugAll || atmSmap_debugError)
         buginf("\nATMMTP: openPartyConnection: null map control block");
      return (FALSE);
   }

   /* Get the correct params structure - static map or class or default class */
   params = atmSmap_get_params(idb, map);

   if (!params) {
      if (atmSmap_debugAll || atmSmap_debugError)
         buginf("\nATMMTP: openPartyConnection: params block not found");
      return (FALSE);
   }

   ptr_to_smap_params = map->params;
 
   if (!ptr_to_smap_params) {
      if (atmSmap_debugAll || atmSmap_debugError)
         buginf("\nATMMTP: openPartyConnection: map params block not found");
      return (FALSE);
   }

   /*
    * A bc_smap exists?  A presence of bc_smap in the params struct means 
    * that a request to establish call or add this party is already pending.
    * Let the previous request complete before issuing another one.
    */
   if (ptr_to_smap_params->bc_smap) {
     if (atmSmap_debugAll || atmSmap_debugEvent)
        buginf("\nATMMTP: openParty, bc_smap exists");    
     return (FALSE);
   }
   else {
     bc_smap = malloc_named( sizeof(atmSig_smap), "ATMSIG-SMAP");
     if (bc_smap == NULL) {
       return (FALSE);
     }
   }

   /* allocate endpoint ref for this member of this call */
   endpoint_id = atmSig_api_allocate_endpoint_id(svc);

   if (!endpoint_id) {
      return (FALSE);
    }

   ptr_to_smap_params->bc_smap = bc_smap;
   bc_smap->call_id = svc;   
   bc_smap->locallyInitiatedCall = TRUE;
   
   /* save destination atm address in bc_smap block */
   bcopy(map->hwconf.addr, bc_smap->destAtmAddr.addr, STATIONLEN_ATMNSAP);
   bc_smap->destAtmAddr.type = STATION_ATMNSAP;
   bc_smap->destAtmAddr.length = STATIONLEN_ATMNSAP;
   bc_smap->destAtmAddr.target_type = TARGET_UNICAST;

   /* save source atm address in bc_smap block */
   bcopy(idb->nsap->addr, bc_smap->srcAtmAddr.addr, STATIONLEN_ATMNSAP);
   bc_smap->srcAtmAddr.type = STATION_ATMNSAP;
   bc_smap->srcAtmAddr.length = STATIONLEN_ATMNSAP;
   bc_smap->srcAtmAddr.target_type = TARGET_UNICAST;

   /* send request to the Signalling driver to add the party */

   /* get memory */
   sig_msg = malloc( sizeof(sig_api_msg) );
   if (sig_msg == NULL) {
     ptr_to_smap_params->bc_smap = NULL;
     free(endpoint_id);
     free(bc_smap);
     return (FALSE);
    }

   /* fill in sig_api_add_party block */

   sig_msg->opcode = ATM_ADD_PARTY;
   sig_msg->idb    = idb; 

   add_party_msg = (sig_api_add_party *)&sig_msg->u.add_party;
   add_party_msg->call_id = svc;
   add_party_msg->endpoint_id = endpoint_id;

   /* fill in destination ATM address in message  */
   bcopy(bc_smap->destAtmAddr.addr, add_party_msg->called_party.addr, 
                                                    STATIONLEN_ATMNSAP);
   add_party_msg->called_party.type = STATION_ATMNSAP;
   add_party_msg->called_party.length = STATIONLEN_ATMNSAP;
   add_party_msg->called_party.target_type = TARGET_UNICAST;

   /* send to Signalling driver */
   atmSmap_sendToSigDriver(sig_msg);

   return (TRUE);

 }




/*
 * a t m S m a p _ s t a t i c M a p P a r t y U p d a t e
 *
 * Enter vcnum in the static map block for this call_id.
 * Returns TRUE if vcnum entered, FALSE otherwise.
 * This is for MTP calls only
 */
boolean atmSmap_staticMapPartyUpdate (void *call_id, void *endpoint_id, 
				      idbtype *idb)
{

  atmSvcType *svc;
  atmSvcPartyType *party;
  staticmap_type *map_ptr;
  mapgroup_type *map_group;
  atm_params_type *ptr_to_smap_params;
  hwaddrtype temp;
  int i;
  atmSig_smap *bc_smap;
  boolean result = FALSE;

  svc = (atmSvcType *)call_id;
  party = (atmSvcPartyType *)endpoint_id;


  /* sanity */
  if (!party ) {
    if (atmSmap_debugEvent)
      buginf("\nATMMTP: staticMapPartyUpdate, no party");
    return (FALSE);
  }

  /* already have VC, don't overwrite map for remote MTP call */
  if (svc->locallyInitiatedCall == FALSE)
    return (FALSE);

  if (party->partyState != ATMSIG_PST_ACTIVE) {
    if (atmSmap_debugEvent)
      buginf("\nATMMTP: staticMapPartyUpdate, party not active");
    return (FALSE);
  }

   map_ptr = NULL;

   /* extract dest ATM address */
   temp.type = STATION_ATMNSAP;
   temp.length = STATIONLEN_ATMNSAP;
   temp.target_type = TARGET_UNICAST;
   bcopy(&party->destNsap.addr, &temp.addr, STATIONLEN_ATMNSAP); 

     if (atmSmap_debugAll || atmSmap_debugEvent)
       buginf("\nATMMTP: staticMapPartyUpdate");

   /* enter vcnum in static map pointing to the nsap address for 
    * this endpoint, for this protocol
    */
   FOR_ALL_MAPGROUPS_ON_IDB(idb, map_group) {
      FOR_ALL_MAPS_IN_LIST(i, map_group->maplist, map_ptr) {

	/* proceed only if address match */
	if ( bcmp(map_ptr->hwconf.addr, temp.addr, STATIONLEN_ATMNSAP) != 0 ) 
	  continue;

	/* verify bc_smap and call_id */
        ptr_to_smap_params = map_ptr->params;

        if (!ptr_to_smap_params)
          continue;

	bc_smap = ptr_to_smap_params->bc_smap;

	if (!bc_smap) 
	  continue;

	/* verify call_id */
	if (bc_smap->call_id != call_id)
	  continue;

	if (atmSmap_debugAll || atmSmap_debugEvent) 
	  buginf("\nstaticMapPartyUpdate: map_ptr %x, bc_smap %x, 
                   bc_call_id %x", map_ptr, bc_smap, bc_smap->call_id);

	if (atmSmap_debugAll || atmSmap_debugEvent) 
	  buginf("\nATMMTP: updating MTP map block with vc %d", svc->vcnum);
	
	/* here's the equivalent of enterVcnumInMap */
	map_ptr->atm_map->bc_hwaddr.type = STATION_ATMVC;
	map_ptr->atm_map->bc_hwaddr.length = STATIONLEN_ATMVC;
	map_ptr->atm_map->bc_hwaddr.addr[0] = (uchar)(svc->vcnum >> 8);
	map_ptr->atm_map->bc_hwaddr.addr[1] = (uchar)svc->vcnum;
	if (!map_ptr->sm_swidb)
	  map_ptr->sm_swidb = svc->idb;

	/* set state in map */
	map_ptr->atm_map->map_linkstate = LINK_ACTIVE;
	map_ptr->atm_map->map_partystate = PARTY_ACTIVE;

	result = TRUE;

	if (atmSmap_debugAll || atmSmap_debugEvent)
	  buginf("\nsvc->linktype = %x, map_ptr->protocol = %x",
		                 svc->linktype, map_ptr->protocol);

      }
    }

  return (result);

}



/*
 * a t m S i g _ s t a t i c M a p M t p D e l e t e ( )
 * 
 * Delete vcd from from all static map entries remaining for this call.
 * MTP only
 */
boolean atmSmap_staticMapMtpDelete (void *call_id, idbtype *idb)
{

   atmSvcType *svc;
   atmSvcPartyType *party;
   hwaddrtype temp;
   staticmap_type *map_ptr;
   mapgroup_type *map_group;
   atm_params_type *ptr_to_smap_params;
   int i;
   atmSig_smap *bc_smap;
   boolean result = FALSE;

   svc = (atmSvcType *)call_id;
   party = (atmSvcPartyType *)svc->rootParty;

   /* remotely initiated MTP doesn't have map entry, nothing to delete */
   if (svc->locallyInitiatedCall == FALSE)
     return(FALSE);

   temp.type = STATION_ATMNSAP;
   temp.length = STATIONLEN_ATMNSAP;
   temp.target_type = TARGET_UNICAST;
   bcopy(&party->destNsap.addr, &temp.addr, STATIONLEN_ATMNSAP);

   if (atmSmap_debugAll || atmSmap_debugEvent)
     buginf("\nATMSM: staticMapMtpDelete");

   /* at this point, should only have 1 map entry with VCD in 
    * bc_hwaddr, check them all anyway
    * clear bc_hwaddr = bc_vcd and free bc_smap from all static maps on 
    * this idb that are in this call
    */
   FOR_ALL_MAPGROUPS_ON_IDB(idb, map_group) {
      FOR_ALL_MAPS_IN_LIST(i, map_group->maplist, map_ptr) {

         /* if no bc_smap, not active in MTP call */
         ptr_to_smap_params = map_ptr->params;
         if (!ptr_to_smap_params)
           continue;

	 bc_smap = ptr_to_smap_params->bc_smap;
	 if (!bc_smap)
	   continue;

	 /* should have same call_id, check it */
	 if (call_id != bc_smap->call_id)
	   continue;

	 /* remove bc_smap block */
	 free(bc_smap);
	 ptr_to_smap_params->bc_smap = NULL;

	 /* clear bc_hwaddr entry in map */
	 map_ptr->atm_map->bc_hwaddr.type    = STATION_ILLEGAL;
	 map_ptr->atm_map->bc_hwaddr.length  =
	 map_ptr->atm_map->bc_hwaddr.addr[0] =
	 map_ptr->atm_map->bc_hwaddr.addr[1] = 0;

	/* set state in map */
	map_ptr->atm_map->map_linkstate = LINK_FAILED;
	map_ptr->atm_map->map_partystate = PARTY_NULL;
	
	result = TRUE;

	if (atmSmap_debugAll || atmSmap_debugEvent) {
	    buginf("\nATMMTP: mtpDelete, linkstate = FAILED, partystate = NULL");
	}
	/*
	 * Call back IP multicast if multipoint VC has gone away.
	*/
	if (map_ptr->protocol == LINK_IP && 
	    ptr_to_smap_params->broadcast) {
	   reg_invoke_ip_invalidate_mcache(idb);
	}
     }
  }

   return(result);

}



/*
 * a t m S m a p _ c l o s e P a r t y C o n n e c t i o n ( )
 *
 * Remove this party from the call.
 */
void atmSmap_closePartyConnection (void *call_id, void *endpoint_id)
{

   sig_api_msg *sig_msg;
   sig_api_drop_party *drop_party;


   /* send request to the Signalling driver to drop the party */

   /* get memory */
   sig_msg = malloc( sizeof(sig_api_msg) );
   if (sig_msg == NULL) {
     return;
   }

   /* fill in sig_api_release block */

   sig_msg->opcode = ATM_DROP_PARTY;

   drop_party = (sig_api_drop_party *)&sig_msg->u.drop_party;
   drop_party->call_id = call_id;
   drop_party->endpoint_id = endpoint_id;
   drop_party->cause = ATM_NORMAL_UNSPECIFIED;

   /* send to Signalling driver */
   atmSmap_sendToSigDriver(sig_msg);

}


/*
 * a t m S i g _ s t a t i c M a p P a r t y D e l e t e ( )
 * 
 * Delete vcd from static map entry.
 * This is only called when individual parties of a multipoint
 * call are dropped.
 */
boolean atmSmap_staticMapPartyDelete (void *call_id, void *endpoint_id, 
				      idbtype *idb)
{

   atmSvcType *svc;
   atmSvcPartyType *party;
   hwaddrtype temp;
   staticmap_type *map_ptr;
   mapgroup_type *map_group;
   atm_params_type *ptr_to_smap_params;
   int i;
   atmSig_smap *bc_smap;
   boolean result = FALSE;


   svc = (atmSvcType *)call_id;
   party = (atmSvcPartyType *)endpoint_id;

   /* if no party, error */
   if (!party) {
     if (atmSmap_debugAll || atmSmap_debugEvent)
       buginf("\n partyDelete: no party block");
     return(FALSE);
   }

   /* remotely initiated MTP doesn't have map entry, nothing to delete */
   if (svc->locallyInitiatedCall == FALSE)
     return(FALSE);

   temp.type = STATION_ATMNSAP;
   temp.length = STATIONLEN_ATMNSAP;
   temp.target_type = TARGET_UNICAST;
   bcopy(&party->destNsap.addr, &temp.addr, STATIONLEN_ATMNSAP);

   if (atmSmap_debugAll || atmSmap_debugEvent)
     buginf("\nATMSM: staticMapPartyDelete");

   /* remove vc number from static map, should be only 1 match */
   FOR_ALL_MAPGROUPS_ON_IDB(idb, map_group) {
      FOR_ALL_MAPS_IN_LIST(i, map_group->maplist, map_ptr) {

	/* proceed only if address match */
	if ( bcmp(map_ptr->hwconf.addr, temp.addr, STATIONLEN_ATMNSAP) != 0 )
	  continue;

   	if (atmSmap_debugAll || atmSmap_debugEvent) 
     	    buginf("\nstaticMapPartyDelete, map_ptr %x call_id %x", 
		   map_ptr, call_id);

	/* verify bc_smap and call_id */
	ptr_to_smap_params = map_ptr->params;
        if (!ptr_to_smap_params)
          continue;

	bc_smap = ptr_to_smap_params->bc_smap;
	if (!bc_smap)
	  continue;

	if (call_id != bc_smap->call_id)
	  continue;

	/* clear vc from bc map entry */
	map_ptr->atm_map->bc_hwaddr.type    = STATION_ILLEGAL;
	map_ptr->atm_map->bc_hwaddr.length  =
	map_ptr->atm_map->bc_hwaddr.addr[0] =
	map_ptr->atm_map->bc_hwaddr.addr[1] = 0;
	map_ptr->sm_swidb = NULL;

	/* set state in map */
	map_ptr->atm_map->map_linkstate = LINK_FAILED;
	map_ptr->atm_map->map_partystate = PARTY_NULL;

	/* remove smap block from static map */
	bc_smap = ptr_to_smap_params->bc_smap;
	free(bc_smap);
	ptr_to_smap_params->bc_smap = NULL;

	result = TRUE;
      }
    }

   return(result);

}


/** ILMI start here **/
ilmiClient atmSmap_ilmiClient;

/*
 * a t m S m a p _ r e g i s t e r I l m i ( )
 * 
 * Register with ILMI.
 * Called (atmSig_init) at initialization.
 */
boolean atmSmap_registerIlmi (void)
{

  static boolean registered = FALSE;

  extern ilmiClient atmSmap_ilmiClient;    /* declared extern */
  ilmiClientReq crq;

  if (registered) {
    if (atmSmap_debugAll || atmSmap_debugEvent)
      buginf("\registerIlmi, already registered");
    return(TRUE);
  }

  atmSmap_ilmiClient = reg_invoke_atm_ilmi_register_client("atmSmap");
	
  if (!atmSmap_ilmiClient) {
    if (atmSmap_debugAll || atmSmap_debugError)
      buginf("\registerIlmi, no ilmiClient");
    return(FALSE);
  }

  crq.client_id = atmSmap_ilmiClient;
  crq.notifyCB = atmSmap_ilmiCallback;

  crq.notify_type = ilmiAddressAdd;
  reg_invoke_atm_ilmi_register_service(&crq);

  crq.notify_type = ilmiResponseNotify;
  reg_invoke_atm_ilmi_register_service(&crq);

  crq.notify_type = ilmiAddressDel;
  reg_invoke_atm_ilmi_register_service(&crq);

  if (atmSmap_debugAll || atmSmap_debugEvent) 
    buginf("\nATMSM: registerIlmi, OK");

  return(TRUE);

}


/*
 * a t m S m a p _ i l m i C a l l b a c k ( )
 * 
 * ILMI callback function 
 * Only handle the services that we registered for
 */
void atmSmap_ilmiCallback (void *rq)
{

  hwidbtype *hwptr;
  idbtype *idb;
  atm_db_t *atm_db;
  ilmiClientReq *crq;
  atmSig_nsap_info *nsap_ptr;
  char nsapstr[MAX_ATM_NSAP_ASCII_LEN];
  char ilmiPrefix[ILMI_PREFIX_SIZE];
  esiReqType *esi_req;
  crq = rq;
  
  FOR_ALL_HWIDBS(hwptr) {
    if (hwptr->hw_if_index == crq->intf_id) 
      break;
  }

  /* paranoia */
  if ( (!hwptr) || (!(hwptr->status & IDB_ATM)) ) {
    if (atmSmap_debugAll || atmSmap_debugError)
      buginf("\nATMSM: ilmiCallback, not ATM port, callback failed");
    return;
  }

  atm_db = hwptr->atm_db;

  /* paranoia */
  if (!atm_db) {
    if (atmSmap_debugAll || atmSmap_debugError)
      buginf("\nATMSM: ilmiCallback, no atm_db");
    return;
}

  switch(crq->notify_type) {

  case ilmiAddressAdd:
      if (atmSmap_debugAll || atmSmap_debugEvent) 
	buginf("\nATMSM: ilmiCallback, ilmiAddressAdd");
    /* 
     * ILMI prefix added
     * Use it for all ESI addresses, overwrite existing prefix, log it
     */
    if (atm_db->ilmiPrefixValid) {
      if (atmSmap_debugAll || atmSmap_debugEvent) 
	buginf("\nATMSM: ilmiAddressAdd, ILMI Prefix already valid");
    }

    if (atm_db->address_reg) {
      if (crq->address->length != ILMI_PREFIX_SIZE) {

	if (atmSmap_debugAll || atmSmap_debugEvent) 
	buginf("\nATMSM: ilmiAddressAdd, incorrect length = %d ", 
	       crq->address->length);

	return;
      }


      /* 
       * New prefix. If new is different than current prefix,
       * need to cancel all esi.
       * If new is same, then just reregister, dont' want to
       * confuse the switch
       */
      if ( bcmp(atm_db->ilmiPrefix, crq->address->octet_ptr, 
		ILMI_PREFIX_SIZE) != 0 ) {

	/* not our prefix, dereg any registered addresses */
	if (atmSmap_debugAll || atmSmap_debugEvent) 
	  buginf("\nATMSM: ilmiAddressAdd, new ILMI Prefix");

	nsap_ptr = atm_db->nsap_list_head;
	while (nsap_ptr) {
	  if (nsap_ptr->useIlmiPrefix) {
	    if (nsap_ptr->ilmiAddrRegistered) {
	      atmSmap_sendNsapToIlmi(nsap_ptr, reqAddressInValid);
	    }
	    nsap_ptr->ilmiPrefixSet = FALSE;
	    nsap_ptr->ilmiAddrRegistered = FALSE;
	    nsap_ptr->idb->atm_nsap_valid = FALSE;
	  }
	  nsap_ptr = nsap_ptr->next;
	}
      }

      /* Get new prefix */
      bcopy (crq->address->octet_ptr, atm_db->ilmiPrefix, 
	     crq->address->length);
      
      atm_db->ilmiPrefixValid = TRUE;

      if (atmSmap_debugAll || atmSmap_debugEvent) {
	atm_printprefix(atm_db->ilmiPrefix, nsapstr);
	buginf("\nATMSM: ILMI Prefix added - %s", nsapstr);
      }

    }
    
    /* Have prefix, loop through all nsap's on atm_db.
     * If using ESI, then overwrite prefix, else leave it alone.
     * Set correct flags and send request to ilmi for switch verification.
     * Set ilmiAddrReg to FALSE so we can reregister.
     * Set atm_nsap_valid to FALSE until reregistered.
     */
    nsap_ptr = atm_db->nsap_list_head;
    while (nsap_ptr) {
      if (nsap_ptr->useIlmiPrefix) {
	nsap_ptr->ilmiAddrRegistered = FALSE;
	  nsap_ptr->idb->atm_nsap_valid = FALSE;
	if (atm_interface_up(nsap_ptr->idb)) {
	  if (atmSmap_getIlmiPrefix(atm_db, nsap_ptr)) {
	    atmSmap_sendNsapToIlmi(nsap_ptr, reqAddressValid);
	  }
	}
      }
      nsap_ptr = nsap_ptr->next;
    }
    break;


    case ilmiResponseNotify:
      if (atmSmap_debugAll || atmSmap_debugEvent) 
	buginf("\nATMSM: ilmiCallback, ilmiResponseNotify");
      /*
     * Ilmi notifies address registers and deregisters.
     * If dereg, free esi_req then exit. If reg, then 
       * ESI configured NSAP has been validated by switch
       * Copy from nsap_info to idb->nsap
       * Register with signalling
       */
    
    /* Sanity checks */
    esi_req = (esiReqType *)crq->transid; 

    if (!esi_req) {
      if (atmSmap_debugAll || atmSmap_debugEvent) 
	buginf("\nATMSM: ilmiResponseNotify, no esi_req from ilmi");
      return;
    }

    idb = esi_req->idb;

    if (!idb) {
      if (atmSmap_debugAll || atmSmap_debugEvent) 
	buginf("\nATMSM: ilmiResponseNotify, no idb from ilmi");
      return;
    }

    /* Validate ilmi message */
      if ( (crq->error == ilmiResponseReceived) && 
	  (crq->response == ilmiRespNoErr) ) {

      /* Verify request */
      if ( (esi_req->ilmiReq != reqAddressValid) && 
	   (esi_req->ilmiReq != reqAddressInValid) ) {
	  if (atmSmap_debugAll || atmSmap_debugEvent) 
	  buginf("\nATMSM: ilmiResponseNotify, invalid request %d", esi_req->ilmiReq);
	free(esi_req);
	  return;
	}

      /* If dereg, just free esi_req and return */
      if (esi_req->ilmiReq == reqAddressInValid) {
	if (atmSmap_debugAll || atmSmap_debugEvent) 
	  buginf("\nATMSM: ilmiResponseNotify, addr dereg ok");
	free(esi_req);
	  return;
      }
	  
      /* Not dereg, must be registration */

	/* find the idb in the nsap_info list */
      nsap_ptr = atm_db->nsap_list_head;
	while (nsap_ptr) {
	if ( (nsap_ptr->idb == idb) && (nsap_ptr->useIlmiPrefix) ) {
	    break;
	  }
	  nsap_ptr = nsap_ptr->next;
	}

	/* addr not found */
      if (!nsap_ptr) {
	  if (atmSmap_debugAll || atmSmap_debugEvent) 
	  buginf("\nATMSM: ilmiResponseNotify, nsap_ptr not found");
	free(esi_req);	  
	  return;
	}

      /* Nsap_ptr found, verify still have idb->nsap
       * May have been deleted already due to race conditions.
       * If idb->nsap found, use it. Else just free esi_req and exit.
       */
      if (idb->nsap) {
	bcopy (nsap_ptr->nsap.addr, idb->nsap->addr, ATM_NSAP_ADDR_LNGTH);
	if (atmSmap_debugAll || atmSmap_debugEvent) 
	  buginf("\nATMSM: ilmiResponseNotify, register ok");
	atmSmap_registerAtmAddress(idb);
      }
      free(esi_req);
      return;
    }

      if ( (crq->error == ilmiResponseReceived) && 
	  (crq->response == ilmiRespBadValueErr) ) {
	
      /* ESI rejected, no action taken, just buginf and free esi_req */
	if (atmSmap_debugAll || atmSmap_debugEvent) 
	  buginf("\nATMSM: ILMI ESI rejected, %s", hwptr->hw_namestring);
      free(esi_req);
	return;
      }

      /* ILMI error response */
      if (atmSmap_debugAll || atmSmap_debugEvent) 
	buginf("\nATMSM: ILMI Error Response, %d", crq->response);
    free(esi_req);
    return;

    case ilmiAddressDel:	
      if (atmSmap_debugAll || atmSmap_debugEvent) 
	buginf("\nATMSM: ilmiCallback, ilmiAddressDel");
      /*
       * A prefix is being deleted 
       * If it's our's, then cancel all valid addresses, but keep same ESI in 
       * nsap_info (don't free nsap_info in api_cancel) and idb->nsap
       * wait around for callback with new prefix.
       * This assumes that all ESI configured subinterfaces have the same
       * prefix.
       */

    if ( (!atm_db->address_reg) || 
	  (!atm_db->ilmiPrefixValid) || 
	  (crq->address->length != ILMI_PREFIX_SIZE) ) {

	if (atmSmap_debugAll || atmSmap_debugEvent) 
	  buginf("\nATMSM: ILMI Prefix NOT deleted");

	return;
      }

      /* check prefix against my prefix */
      if ( bcmp(atm_db->ilmiPrefix, crq->address->octet_ptr, 
		ILMI_PREFIX_SIZE) != 0 ) {

	/* not our prefix, ignore it */
	if (atmSmap_debugAll || atmSmap_debugEvent) 
	  buginf("\nATMSM: ILMI Prefix not deleted, not ours");
      
	return;
      }

    /* our prefix */
    if (atmSmap_debugAll || atmSmap_debugEvent) 
      buginf("\nATMSM: our ILMI Prefix");
    
    /* Our prefix is being deleted. If nsap_info address is valid,
     * cancel the address, clear the prefix, set the flags.
     * Don't free the nsap_info struct, null the prefixes.
     * Don't bother deregistering with ILMI because it already knows
     * that this prefix is gone.
     */

    /* search the nsap_info list */    
    nsap_ptr = atm_db->nsap_list_head;
    while (nsap_ptr) {
      if (nsap_ptr->useIlmiPrefix) {
	atmSmap_sendNsapToIlmi(nsap_ptr, reqAddressInValid);
	nsap_ptr->ilmiPrefixSet = FALSE;
	nsap_ptr->ilmiAddrRegistered = FALSE;
	nsap_ptr->idb->atm_nsap_valid = FALSE;
	bzero(nsap_ptr->idb->nsap->addr, ILMI_PREFIX_SIZE);
	bzero(&nsap_ptr->nsap.addr, ILMI_PREFIX_SIZE);
      }
      nsap_ptr = nsap_ptr->next;
    }

    bzero(atm_db->ilmiPrefix, ILMI_PREFIX_SIZE);
    atm_db->ilmiPrefixValid = FALSE;

    /* 
     * Now check for any prefixes that ILMI is hoarding.
     * If prefix returned, register with this prefix
     */
    if (atmSmap_debugAll || atmSmap_debugEvent) 
      buginf("\nATMSM: check ILMI for Prefix");
    
    if (atmSmap_ilmiPrefix(hwptr, ilmiPrefix) ) {
      bcopy(ilmiPrefix, atm_db->ilmiPrefix, ILMI_PREFIX_SIZE);
      atm_db->ilmiPrefixValid = TRUE;

      /* Got a prefix from ILMI. Now register */
      nsap_ptr = atm_db->nsap_list_head;
      while (nsap_ptr) {
	if (nsap_ptr->useIlmiPrefix) {
	  nsap_ptr->ilmiAddrRegistered = FALSE;
	  nsap_ptr->idb->atm_nsap_valid = FALSE;
	  if (atm_interface_up(nsap_ptr->idb)) {
	    if (atmSmap_getIlmiPrefix(atm_db, nsap_ptr)) {
	      atmSmap_sendNsapToIlmi(nsap_ptr, reqAddressValid);
	    }
	  }
	}
	nsap_ptr = nsap_ptr->next;
      }
    }

    break;
    

  default:
	
      if (atmSmap_debugAll || atmSmap_debugEvent) 
	buginf("\nATMSM: atmSmap_ilmiCallback, unknown callback");
      
    }

  return;

}



/*
 * a t m S m a p _ f i l l E s i I n N s a p I n f o ( )
 * 
 * Fill Esi In Nsap, calls getIlmiPrefix, sendNsapToIlmi 
 * Called from the atm cmd line. Use printf's, not buginf's.
 */
boolean atmSmap_fillEsiInNsapInfo(idbtype *idb, atm_db_t *atm_db)
{

  atmSig_nsap_info *nsap_ptr;

  /* alloc an nsap struct and fill in info */
  nsap_ptr = malloc_named( sizeof(atmSig_nsap_info), "ATMSIG-NSAP");
  if (nsap_ptr == NULL) {
    return(FALSE);
  }

  /* copy full NSAP from idb->nsap to nsap_ptr, overwrite the prefix below */
  bcopy(idb->nsap->addr, &nsap_ptr->nsap.addr, ATM_NSAP_ADDR_LNGTH);

  /* set nsap_ptr fields */
  nsap_ptr->idb = idb;
  nsap_ptr->useIlmiPrefix = TRUE;

  /* add this nsap_info struct to linked list */
  nsap_ptr->next = idb->hwptr->atm_db->nsap_list_head;
  idb->hwptr->atm_db->nsap_list_head = nsap_ptr;

  /* call getIlmiPrefix here, exit if no prefix yet */
  if ( !atmSmap_getIlmiPrefix(atm_db, nsap_ptr) ) {
    if (atmSmap_debugAll || atmSmap_debugEvent) 
      printf("\nATMSM: fillEsiInNsapInfo, getIlmiPrefix failed");
    return(FALSE);
  }

  /* send to ilmi here */
  if (!atmSmap_sendNsapToIlmi(nsap_ptr, reqAddressValid) ) {
    if (atmSmap_debugAll || atmSmap_debugEvent) 
      printf("\nATMSM: fillEsiInNsapInfo, sendNsapToIlmi failed");
    return(FALSE);
  }

  return(TRUE);

}


/*
 * a t m S m a p _ g e t I l m i P r e f i x ( )
 *
 * Get prefix found by Ilmi. Build NSAP and set flags.
 */
boolean atmSmap_getIlmiPrefix(atm_db_t *atm_db, atmSig_nsap_info *nsap_ptr)
{

  if (!atm_db->ilmiPrefixValid) {
    if (atmSmap_debugAll || atmSmap_debugError) 
      buginf("\nATMSM: getIlmiPrefix, Prefix not valid");
    return(FALSE);
  }

  /* copy prefix from atm_db->ilmiPrefix */
   bcopy(&atm_db->ilmiPrefix, &nsap_ptr->nsap.addr, ILMI_PREFIX_SIZE); 

  /* set ILMI fields in nsap_ptr */
  nsap_ptr->ilmiPrefixSet = TRUE;

  if (atmSmap_debugAll || atmSmap_debugEvent) 
    buginf("\nATMSM:getIlmiPrefix OK");

  return(TRUE);


}


/*
 * a t m S m a p _ i l m i P r e f i x ( )
 *
 * Get a prefix from ILMI. 
 * If prefix found return TRUE, else return FALSE.
 *
 */
boolean atmSmap_ilmiPrefix (hwidbtype *hwidb, uchar *ilmiPrefix)
{
    OctetString *os;

    os = reg_invoke_atm_ilmi_addrtraverse(hwidb->hw_if_index, NULL);

    if (os == NULL) {
      if (atmSmap_debugAll || atmSmap_debugEvent) 
	buginf("\nATMSM: IlmiPrefix: octet string is NULL");
      return FALSE;
    }

    if (os->length != ILMI_PREFIX_SIZE) {
      if (atmSmap_debugAll || atmSmap_debugEvent) 
	buginf("\nATMSM: IlmiPrefix: invalid octet string len");
      return FALSE;
    }

    if (atmSmap_debugAll || atmSmap_debugEvent) {
      buginf("\nATMSM: IlmiPrefix: prefix found");
    }

    bcopy (os->octet_ptr, ilmiPrefix, ILMI_PREFIX_SIZE);
    return TRUE;
}


#define OID_HDR_SIZE 14

/*
 * a t m S m a p _ s e n d N s a p T o I l m i ( )
 *
 * Convert NSAP to SNMP format. Send it to ILMI for registration 
 * with ATM switch.
 */
boolean 
atmSmap_sendNsapToIlmi (atmSig_nsap_info *nsap_ptr, ilmiReqType request)
{

  idbtype *idb;
  hwidbtype *hwptr;
  atm_db_t *atm_db;

  ilmiClientReq crq;
  VarBind vb;
  OID oid_name;
  int ii;
  char nsapstr[MAX_ATM_NSAP_ASCII_LEN];
  esiReqType *esi_req;
  static long oid_ptr[] = {1, 3, 6, 1, 4, 1, 353, 2, 6, 1, 1, 3, 0, 20,
			   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			   0, 0, 0, 0, 0, 0, 0};

  idb = nsap_ptr->idb;
  hwptr = idb->hwptr;

  /* paranoia */
  if (!hwptr->status & IDB_ATM) {
    if (atmSmap_debugAll || atmSmap_debugEvent)
      buginf("\nATMSM: sendNsapToIlmi, not ATM port, callback failed");
    return(FALSE);
  }

  atm_db = hwptr->atm_db;

  /* more paranoia */
  if (!atm_db) {
    if (atmSmap_debugAll || atmSmap_debugEvent)
      buginf("\nATMSM: sendNsapToIlmi, no atm_db, callback failed");
    return(FALSE);
  }

  /* more paranoia, verify ILMI prefix and ESI set for atm_db  */
  if ( (atm_db->address_reg   == FALSE) ||
      (atm_db->ilmiPrefixValid == FALSE) ) {
    if (atmSmap_debugAll || atmSmap_debugEvent) 
      buginf("\nATMSM: sendNsapToIlmi, atm_db not set for ILMI");
    return(FALSE);
  }

  /* more paranoia, verify ILMI prefix and ESI set for nsap_ptr  */
  if ( (nsap_ptr->useIlmiPrefix   == FALSE) ||
       (nsap_ptr->ilmiPrefixSet == FALSE) ) {
    if (atmSmap_debugAll || atmSmap_debugEvent) 
      buginf("\nATMSM: sendNsapToIlmi, nsap_ptr not set for ILMI");
    return(FALSE);
  }

  /* Verify request */
  if ( (request != reqAddressValid) && (request != reqAddressInValid) ) {
    if (atmSmap_debugAll || atmSmap_debugEvent) 
      buginf("\nATMSM: sendNsapToIlmi, invalid request %d", request);
    return(FALSE);
  }
    
  /* Malloc esiReq for ILMI *cookie* */
  esi_req = malloc_named( sizeof(esiReqType), "ESI-REQ");
  if (esi_req == NULL)
    return (FALSE);

  esi_req->idb = idb;
  esi_req->ilmiReq = request;
  


  /* full NSAP  stored in nsap_ptr, register when verified by ILMI
   * copy nsap to idb->nsap when registered
   */
   if (atmSmap_debugAll || atmSmap_debugEvent) {
     atm_printnsap(nsap_ptr->nsap.addr, nsapstr);
     buginf("\nATMSM: ILMI Address Formed - %s", nsapstr);
   }

  /* 
   *  set atmaddress for ILMI
   */
  /* OID_HDR initialized in array, now fill in the nsap */
  for(ii=0; ii<ATM_NSAP_ADDR_LNGTH; ii++) 
    oid_ptr[(OID_HDR_SIZE + ii)] = nsap_ptr->nsap.addr[ii];
  
  /* clear snmp structures */
  memset(&crq, 0, (sizeof (ilmiClientReq)) );
  memset(&vb, 0, (sizeof (VarBind)) );
  memset(&oid_name, 0, (sizeof (OID)) );

  /* set snmp structures */
  vb.name = &oid_name;
  vb.name->oid_ptr = oid_ptr;
  vb.name->length = ILMI_ADDR_SET_LEN;
  vb.value.type = INTEGER_TYPE;
  vb.value.sl_value = request; 

  (esiReqType *)crq.transid = esi_req;
  crq.vblist = &vb;
  crq.oper_type = ilmiSet;
  crq.client_id = atmSmap_ilmiClient; /* make this global */
  crq.intf_id = hwptr->hw_if_index;

  /* send to ilmi */
  if (atmSmap_debugAll || atmSmap_debugEvent) 
    buginf("\nATMSM: sendNsapToIlmi %s", 
	   request == reqAddressValid ? "Register" : "DeRegister");

  reg_invoke_atm_ilmi_client_request(&crq);

  return (TRUE);

}



/*
 * a t m S m a p _ e x t D e l e t e S m a p ( )
 *
 * When:
 *      - a map-list is deleted,
 *      - a static map is deleted from a map list.
 *
 * What:
 *      - remove arp-table  entry.
 *	- Free our params structures.
 */
static void atmSmap_extDeleteSmap(staticmap_type * map_ptr)
{
    atm_params_type *ptr_to_smap_params;

    /* check for ATM signalling SMAP. */
    if (map_ptr->hwconf.type != STATION_ATMNSAP) 
	return;

    if (atmSmap_debugAll || atmSmap_debugEvent)
         buginf("\nATMSM: extended delete of static-map");

    /* if ip map, invalidate arp-cache. */
    if (map_ptr->protocol == LINK_IP)
        arp_table_remove((long)map_ptr->addr.addr.ip_address,
                LINK_IP);

    ptr_to_smap_params = map_ptr->params;
 
    if (!ptr_to_smap_params)
	return;

    if (ptr_to_smap_params->bc_smap) {
	free(ptr_to_smap_params->bc_smap);
	ptr_to_smap_params->bc_smap = NULL;
    }

    if (ptr_to_smap_params->smap) {
        free(ptr_to_smap_params->smap);
	ptr_to_smap_params->smap = NULL;
    }
}

/*
 * a t m S m a p _ e x t D e l e t e M a p G r o u p ( )
 *
 * When:
 *      - a map-group is deleted,
 *
 * What:
 *      - remove arp-table  entry.
 */
static void atmSmap_extDeleteMapGroup(idbtype *idb, maplist_type * list_ptr)
{
    staticmap_type * map_ptr;
    atm_params_type *ptr_to_smap_params;
    long ipaddr;
    boolean didGroup=FALSE;
    int i;

    FOR_ALL_MAPS_IN_LIST(i, list_ptr, map_ptr) {
        if ((map_ptr->map_type == MAP_STATIC) &&
            (map_ptr->hwconf.type == STATION_ATMNSAP)) {
	    if ((atmSmap_debugAll || atmSmap_debugEvent) && !didGroup) {
                buginf("\nATMSM: extended delete of map group");
		didGroup = TRUE;
	    }

            if (map_ptr->protocol == LINK_IP) {
                ipaddr = (long)map_ptr->addr.addr.ip_address;
                arp_table_remove(ipaddr, LINK_IP);
            }
	    map_ptr->hwaddr.type = STATION_ILLEGAL;
	    map_ptr->hwaddr.length  =
             map_ptr->hwaddr.addr[0] =
             map_ptr->hwaddr.addr[1] = 0;
            map_ptr->sm_swidb = NULL;

            if (map_ptr->atm_map) {
		map_ptr->atm_map->next_svc   = NULL; 
		map_ptr->atm_map->bc_vc      = NULL;
		map_ptr->atm_map->bc_call_id = NULL;
		map_ptr->atm_map->bc_linkstate = LINK_NULL;
	    }

            ptr_to_smap_params = map_ptr->params;
	    if (ptr_to_smap_params->smap) {
		free(ptr_to_smap_params->smap);
		ptr_to_smap_params->smap = NULL;
	    }
            if (ptr_to_smap_params->bc_smap) {
                free(ptr_to_smap_params->bc_smap);
                ptr_to_smap_params->bc_smap = NULL;
            }
        }
    } 
}

/*
 * a t m S m a p _ e x t U p d a t e S m a p ( )
 *
 * When:
 *      - a map-list is deleted via CLI,
 *      - a static map is deleted from a map list via CLI.
 *
 * What:
 *      - remove arp-table  entry.
 *      - Free our params structures.
 *      - in future, think about if NSAP same/broadcast same/ etc.
 */
static void atmSmap_extUpdateSmap(
    maplist_type *list_ptr,
    staticmap_type *map_ptr,
    hwaddrtype *hwconf_ptr,
    char *name)
{
    atm_params_type *ptr_to_smap_params;
 
    /* check for ATM signalling SMAP. */
    if (map_ptr->hwconf.type != STATION_ATMNSAP)
        return;

    if (atmSmap_debugAll || atmSmap_debugEvent)
         buginf("\nATMSM: extended update of static-map");

    /* if ip map, invalidate arp-cache. */
    if (map_ptr->protocol == LINK_IP)
        arp_table_remove((long)map_ptr->addr.addr.ip_address,
                LINK_IP);

    map_ptr->hwaddr.type = STATION_ILLEGAL;
    map_ptr->hwaddr.length  =
     map_ptr->hwaddr.addr[0] =
     map_ptr->hwaddr.addr[1] = 0;
    map_ptr->sm_swidb = NULL;
 
    ptr_to_smap_params = map_ptr->params;
 
    if (!ptr_to_smap_params)
        return;
 
    if (ptr_to_smap_params->bc_smap) {
        free(ptr_to_smap_params->bc_smap);
        ptr_to_smap_params->bc_smap = NULL;
    }
 
    if (ptr_to_smap_params->smap) {
        free(ptr_to_smap_params->smap);
        ptr_to_smap_params->smap = NULL;
    }
}

/*
 * atmsmap_reqvc
 * Attempt to open an SVC connection for this map table entry.
 * Return TRUE if we fail to already have a VC associated with the map
 * table entry.
 */

static boolean atmsmap_reqvc (idbtype *swidb, staticmap_type *map)
{
    atm_db_t *atm;

    if (atm_glom_onto_vc(map, swidb))
	return(FALSE);
    atm = swidb->hwptr->atm_db;
    if (!atm)
	return(TRUE);
    if (!atm->sig_vcnum)
	return(TRUE);
    atmSmap_openSvcConnection(swidb, map, 0);
    return(TRUE);
}

/*
 * a t m S m a p _ I n i t
 *
 * At startup, initialize atm static-maps.
 *
 */
void atmSmap_Init (void)
{
    /* register static-map deletion-extension functions. */
    reg_add_ext_staticmap_delete(atmSmap_extDeleteSmap,
                        "extended static-map delete:ATM SVC");
 
    reg_add_ext_mapgroup_delete(atmSmap_extDeleteMapGroup,
                        "extended map-group delete:ATM SVC");
 
    reg_add_ext_staticmap_update(atmSmap_extUpdateSmap,
                        "extended static-map update:ATM SVC");
    reg_add_ip_atm_map_request_vc(atmsmap_reqvc, "atmsmap_reqvc");
}
