/* $Id: standby.c,v 3.13.10.16 1996/08/28 12:53:56 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/standby.c,v $
 *------------------------------------------------------------------
 * standby.c -- Hot standby protocol
 *
 * Sep 1993 Tony Li
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: standby.c,v $
 * Revision 3.13.10.16  1996/08/28  12:53:56  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.13.10.15  1996/08/10  16:45:55  dawnli
 * CSCdi65597:  Removing standby track statements blows away all HSRP for
 * interface
 * Branch: California_branch
 *
 * Revision 3.13.10.14  1996/07/26  20:35:00  weismann
 * CSCdi63104:  TB brakes HSRP on a 7500 TokenRing Interface. SMF needs
 * token ring functional address canonicalized for proper filtering.
 * Branch: California_branch
 *
 * Revision 3.13.10.13  1996/07/23  23:58:38  mleelani
 * CSCdi63822:  HSRP/PIM cannot operate concurrently
 * Branch: California_branch
 * Disable only for BSI FDDI.
 *
 * Revision 3.13.10.12  1996/07/23  13:25:05  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.13.10.11  1996/07/10  09:57:13  dawnli
 * CSCdi54969:  HSRP tracking does not track Frame Relay sub-interfaces
 * Branch: California_branch
 *
 * Revision 3.13.10.10  1996/06/28  01:21:22  snyder
 * CSCdi61601:  fix some char * that should have been char [] in iprouting
 * Branch: California_branch
 *
 *              You get space back for the pointer
 *              You get space back for each reference
 *              You get bytes out of data space
 *
 * Revision 3.13.10.9  1996/06/28  00:35:32  feisal
 * CSCdi51017:  HSRP confuses the Cat5000 when its used in a looped
 * environment
 * Branch: California_branch
 *
 * Revision 3.13.10.8  1996/06/18  18:26:09  dawnli
 * CSCdi60445:  reloading 4500 breaks HSRP - timers change to zero
 * Branch: California_branch
 *
 * Interface state changes in the time window that standby process has been
 * created but not registered. Standby will stay in init state.
 *
 * Register HSRP before standby process is created.
 *
 * Revision 3.13.10.7  1996/06/18  08:17:54  dawnli
 * CSCdi59054:  fddi behaves unstable when using HSRP on fddi
 * Branch: California_branch
 * Delay fddi reset to reduce state flap when multiple HSRP groups
 * are configured on fddi.
 *
 * Revision 3.13.10.6  1996/05/23  07:38:01  myeung
 * CSCdi58029:  OSPF crash in ospf_interface_up() when unconfiguring OSPF
 * Branch: California_branch
 * - Add boolean parameter to ip_address_change registry to indicate if
 *   the change is for secondary or primary address.
 *
 * Revision 3.13.10.5  1996/05/09  14:31:47  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.13.10.4.4.1  1996/04/27  07:05:40  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.13.10.4  1996/04/23  14:33:07  cshen
 * CSCdi52796:  HSRP broken on c1005
 * Branch: California_branch
 *
 * Revision 3.13.10.3  1996/04/22  17:45:13  gstovall
 * CSCdi54760:  Setting of variable through state_config vector can be
 * missed
 * Branch: California_branch
 * Reset the handled field when reusing an item.
 *
 * Revision 3.13.10.2.4.1  1996/04/08  01:56:07  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.13.10.2  1996/03/30  07:52:27  bcole
 * CSCdi48077:  Use of old standby group command after new config may
 * cause hang
 * Branch: California_branch
 *
 * Bring 11.1 fixes forward...
 *
 * Revision 3.13.10.1  1996/03/18  20:29:47  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.13.14.2  1996/04/01  23:50:39  cakyol
 * you do not need a lane client to shut down HSRP
 * on an atm subinterface.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.13.14.1  1996/03/22  22:53:01  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.13  1996/02/22  16:54:50  kramling
 * CSCdi48646:  System restarted by error - a SegV exception, PC 0x6049F09C
 *
 * Revision 3.12  1996/02/15  07:46:51  smackie
 * If it doesn't return a value, it's gotta be void. Fix numerous
 * examples of inlines that slipped through the net. (CSCdi49069)
 *
 * Revision 3.11.6.1  1996/03/05  06:15:00  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.11.2.1  1996/02/27  21:09:35  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.11  1996/02/07  16:13:30  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.10  1996/02/01  06:06:20  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.9  1996/01/24  20:16:45  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.8.2.1  1996/01/24  22:20:39  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.8  1996/01/22  06:15:05  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.7.6.2  1996/01/23  23:53:32  cakyol
 * HSRP over LANE Code review changes:
 * - "interface_up" now works properly with HSRP after
 *   Badri's network.c fix.
 * - additional HSRP mac addresses implemented much
 *   faster on the LANE client.
 * - starting an already running or stopping an already
 *   stopped LANE client now behaves properly.
 * - white space cleanup
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.7.6.1  1996/01/11  01:45:46  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.7  1995/12/06  20:24:02  mikel
 * CSCdi33290:  HSRP + PIM on low-end fddi if = packet replication
 * disable PIM and HSRP config on low end fddi due to the non ability
 * to program CAM.  Disallow for BSI chipset only.
 *
 * Revision 3.6.10.1  1995/12/29  01:06:12  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.6  1995/11/19  08:38:47  bcole
 * CSCdi44156:  HSRP: Should be configurable over 802.10 VLANs
 *
 * Allow configuration of HSRP over subinterface'd LAN media.  Add
 * more configuration sanity checking.
 *
 * Revision 3.5  1995/11/19  08:11:00  bcole
 * CSCdi29702:  HSRP: Secondary address support should be improved
 *
 * Abstract implementation of virtual IP address, so that multiple may
 * now be configured per HSRP group.  Add nob.
 *
 * Revision 3.4  1995/11/19  08:03:19  bcole
 * CSCdi26336:  reconfiguring a HSRP address will not install it in arp
 * cache
 *
 * Avoid leaving HSRP entries that are no longer valid in ARP
 * cache/dynamic IP alias table.  Revise virtual IP addr as appropriate.
 *
 * Revision 3.3  1995/11/17  17:37:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:48:56  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:10:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.12  1995/09/27  20:36:52  bcole
 * CSCdi41163: HSRP: Non-active router should still provide proxy ARP
 * responses
 *
 * HSRP should not conditionally cause proxy ARP responses to be
 * suppressed.
 *
 * Revision 2.11  1995/09/25  08:31:22  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.10  1995/09/11  23:51:02  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.9  1995/08/15 00:20:15  bcole
 * CSCdi38170:  Ping to HSRP IP address causes ICMP echo request storm
 *
 * Add notion of dynamic IP aliases, so that HSRP virtual IP address is
 * now pingable.  Eliminate packet forwarding problem for this virtual
 * IP address as a side effect.
 *
 * Revision 2.8  1995/08/09  00:20:55  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.7  1995/08/03  07:42:34  hampton
 * EIGRP stops processing input.  Convert the IP socket input queue into a
 * watched_queue.  This allows the scheduler to see if there is input
 * waiting when the process dismisses.  [CSCdi38156]
 *
 * Revision 2.6  1995/06/23 18:48:01  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.5  1995/06/19  06:44:01  bcole
 * CSCdi36059:  HSRP: 8 byte password truncated to 7 bytes
 *
 * Fix off by 1 error during command processing.
 *
 * Revision 2.4  1995/06/19  01:08:37  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.3  1995/06/16  17:08:31  dkatz
 * CSCdi35956:  Timer declarations should be in a separate file
 * Eliminate obsolete macro references.
 *
 * Revision 2.2  1995/06/09  13:06:51  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:11:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "config.h"
#include "logger.h"
#include "../iprouting/msg_standby.c"	/* Not a typo, see logger.h */
#include "parser.h"
#include "packet.h"
#include "bitlogic.h"
#include "sys_registry.h"
#include "../if/network.h"
#include "../ip/ip.h"
#include "../ui/debug.h"
#include "ieee.h"
#include "../if/tring.h"
#include "../ip/ip_media.h"
#include "../ip/ip_registry.h"
#include "media_registry.h"
#include "../hes/if_fci.h"
#include "../ip/ipaddress.h"

#include "iprouting_debug.h"
#include "standby.h"
#include "parser_defs_standby.h"
#include "../smf/smf_public.h"
#include "../util/itemlist.h"

#include "../if/atm.h"
#include "../lane/lane.h"
#include "../lane/lane_registry.h"

static const char notsameaddress[] = 
    "Hot standby address cannot equal interface IP address"; 
static const char alreadyconfigured[] = 
    "%i is configured as the %s address";
static char *sdefault_auth = SDEFAULT_AUTHSTRING;
static int standby_pid = 0;
static ipsocktype *socket;
static boolean standby_wakeup;
static boolean standby_registered = FALSE;
static sys_timestamp standby_nexthellotime;

static void standby_sendmulticast(int opcode, idbtype *idb, standbyinfotype *);
static void standby_state(int newstate, standbyinfotype *, idbtype *idb);
static forktype standby_process(void);
static void standby_bugpak(boolean received, paktype *pak, idbtype *idb);
static char *standby_statestring(int state);

/*
 * Router scalability enhancement
 *
 * In order not to have to look through all the interfaces and
 * all the groups, we can put IP addresses in a hash table
 */
static standbyaddrtype *standby_hash_table[NETHASHLEN];

static void standby_hash_add(standbyaddrtype *sa);
static standbyaddrtype *standby_hash_lookup(ipaddrtype addr);
static void standby_hash_remove(standbyaddrtype *sa);

/*
 * standby_get_group
 *
 * Chain through the standby groups to match by group number
 */

static standbyinfotype *
standby_get_group (idbtype *idb, uchar group)
{
    standbyinfotype *si;

    FOR_ALL_SBGROUPS(idb, si) {
	if (si->group == group) {
	    return(si);
	}
    }
    return(NULL);
}

/*
 * standby_hash_lookup
 *
 * Returns the standby group that contains the given IP address.
 * Hash table lookup
 */
 
static standbyaddrtype *
standby_hash_lookup (ipaddrtype address)
{
    uchar key;
    standbyaddrtype *sa;
	
    key = nethash(address);
    for (sa = standby_hash_table[key]; sa; sa = sa->next_in_hash) {
	if (sa->addr == address) {
	    break;
	}
    }
    return (sa);
}

/*
 * standby_hash_add
 *
 * add a new address unless the address is already there
 * but we avoid that - don't we
 */
 
static void
standby_hash_add (standbyaddrtype *sa)
{
    ipaddrtype address;
    uchar key;

    address = sa->addr;
    if (standby_hash_lookup(address) != NULL) {
	return;
    }
    key = nethash(address);
    sa->next_in_hash = standby_hash_table[key];
    standby_hash_table[key] = sa;
    return;
}

/*
 * standby_hash_remove
 *
 * Remove standbyaddrtype from hash table
 */

static void
standby_hash_remove (standbyaddrtype *sa)
{
    standbyaddrtype *current, **prev_ptr;
    uchar key;

    key = nethash(sa->addr);
    prev_ptr = &standby_hash_table[key];

    while ((current = *prev_ptr) != NULL) {
	if (current == sa) {
	    *prev_ptr = current->next_in_hash;
	    break;
	}
	prev_ptr = &(current->next_in_hash);
    }
    return;
}

/*
 * standby_priocmp
 * Compare the priorities of the incoming packet with our own.  Return 1
 * for higher, -1 for lower.
 */

static int standby_priocmp (paktype *pak, idbtype *idb, uchar pri)
{
    standbytype *standby;
    udptype *udp;
    iphdrtype *ip;

    ip  = (iphdrtype *)ipheadstart(pak);
    udp = (udptype *) ipdatastart(pak);
    standby = (standbytype *) ((char *)udp + UDPHEADERBYTES);

    if (pri > standby->priority)
	return(-1);
    if (pri < standby->priority)
	return(1);
    if (idb->ip_address > ip->srcadr)
	return(-1);
    if (idb->ip_address < ip->srcadr)
	return(1);
    return(1);
}

/*
 * standby_rcv_hello
 * Receive a HELLO packet on an interface.
 */

static void standby_rcv_hello (paktype *pak, standbyinfotype *li,
			       idbtype *idb, standbytype *standby,
			       boolean authok)
{
    iphdrtype *ip;

    ip = (iphdrtype *)ipheadstart(pak);
    switch (standby->state) {
    case SSTATE_SPEAK:

	/*
	 * If the remote is a higher priority speaker, and we're either
	 * speaking or playing standby, we should go silent and let him
	 * play.
	 */
	if ((standby_priocmp(pak, idb, li->priority) == 1) &&
	    ((li->state == SSTATE_SPEAK) ||
	     (li->state == SSTATE_STANDBY))) {
	    if (li->state == SSTATE_STANDBY)
		RESTART_STANDBYTIMER;
	    standby_state(SSTATE_LISTEN, li, idb);
	}
	break;
    case SSTATE_ACTIVE:

	/*
	 * If we've heard from an active router, remember who it is.  If we're
	 * learning, remember the information that is being broadcast,
	 * and then listen.  
	 */
	li->active = ip->srcadr;
	if ((li->state != SSTATE_VIRGIN) &&
	    (li->state != SSTATE_ACTIVE)) {
	    RESTART_ACTIVETIMER;
	    if (authok) {
		/*
		 * Don't learn dynamic information from foreign active router
		 * unless their authentication passed our check.
		 */
		li->learned_hellotime = standby->hellotime;
		li->learned_holdtime = standby->holdtime;
		if (!(li->flags & SFLAG_CFG_IP_ADDR))
		    standby_primary_addr(li) = standby->address;
	    }
	}
	if ((li->state == SSTATE_LEARN) && standby_have_addr(li)) {
	    RESTART_STANDBYTIMER;
	    standby_state(SSTATE_LISTEN, li, idb);
	    return;
	}

	/*
	 * If the remote has lower priority, or if we're the active router,
	 * cause a coup.  If we listening, start speaking.  If we're
	 * standby, become active.
	 */
	if (standby_priocmp(pak, idb, li->priority) == -1) {
	    if ((li->flags & SFLAG_CFG_PREEMPT) ||
		(li->state == SSTATE_ACTIVE)) {
		/*
		 * Someone else thinks they are the designated router, and we
		 * already are or are about to be.  This may lead to two 
		 * boxes on the same network segment listening to the same
		 * hot standby group MAC address.  As a unicast packet
		 * may fail to get transmitted due to ARP failure,
		 * multicast the coup to expedite things.
		 * We could alternately use ip_arp_glean()...
		 * If our state is pre SSTATE_LISTEN, we are not ready
		 * to assume active roll so don't send a coup in that case.
		 */
		if (li->state & ~(SSTATE_VIRGIN|SSTATE_LEARN))
		    standby_sendmulticast(LOPCODE_COUP, idb, li);
		switch (li->state) {
		case SSTATE_LISTEN:
		case SSTATE_SPEAK:
		    RESTART_STANDBYTIMER;
		    /* Fall through */
		case SSTATE_STANDBY:
		    standby_state(SSTATE_ACTIVE, li, idb);
		    break;
		default:
		    break;
		}
	    }
	} else {
	    /*
	     * Remote has higher priority and is active.  If we
	     * are also, remote should have sent us a coup.  But just
	     * in case the remote's address filter is messed up and they
	     * don't see our multicasts (it happened), resign anyways.
	     */
	    if (li->state == SSTATE_ACTIVE) {
		RESTART_ACTIVETIMER;
		RESTART_STANDBYTIMER;
		standby_state(SSTATE_SPEAK, li, idb);
	    }
	}
	break;
    case SSTATE_STANDBY:
	li->standby = ip->srcadr;
	if (standby_priocmp(pak, idb, li->priority) == 1) {

	    /*
	     * If the standby has a higher priority, and we're speaking,
	     * but not active, start listening.
	     */
	    switch (li->state) {
	    case SSTATE_SPEAK:
	    case SSTATE_STANDBY:
		standby_state(SSTATE_LISTEN, li, idb);
		/* Fall through */
	    case SSTATE_LISTEN:
	    case SSTATE_ACTIVE:
		RESTART_STANDBYTIMER;
		break;
	    default:
		break;
	    }
	} else {

	    /*
	     * If the standby has lower priority, start to take over for
	     * him.
	     */
	    switch (li->state) {
	    case SSTATE_LISTEN:
		standby_state(SSTATE_SPEAK, li, idb);
		/* Fall Through */
	    case SSTATE_ACTIVE:
		RESTART_STANDBYTIMER;
		break;
	    case SSTATE_SPEAK:
		standby_state(SSTATE_STANDBY, li, idb);
		break;
	    default:
		break;
	    }
	}
	break;
    default:
	break;
    }
}

/*
 * standby_rcv_coup
 * Receive a COUP packet on an interface.
 */

static void standby_rcv_coup (paktype *pak, standbyinfotype *li,
			      idbtype *idb,
			      standbytype *standby)
{
    /*
     * If we receieve a coup from a higher priority router, then resign.
     */
    if ((li->state == SSTATE_ACTIVE) &&
	(standby_priocmp(pak, idb, li->priority) == 1)) {
	RESTART_ACTIVETIMER;
	RESTART_STANDBYTIMER;
	standby_state(SSTATE_SPEAK, li, idb);
    }
}

/*
 * standby_rcv_resign
 * Receive a RESIGN packet on an interface.
 */

static void standby_rcv_resign (paktype *pak, standbyinfotype *li,
				idbtype *idb,
				standbytype *standby)
{
    /*
     * If we received a resignation, the standby becomes active.
     * Everyone else starts to speak.
     */
    switch (li->state) {
    case SSTATE_STANDBY:
	standby_state(SSTATE_ACTIVE, li, idb);
	break;
    case SSTATE_LISTEN:
	standby_state(SSTATE_SPEAK, li, idb);
	RESTART_STANDBYTIMER;
	/* fall through */
    case SSTATE_SPEAK:
	RESTART_ACTIVETIMER;
	break;
    default:
	break;
    }
}

/*
 * standby_rcvpacket
 * Receive a standby protocol packet on an interface.
 */

static void standby_rcvpacket (paktype *pak, idbtype *idb)
{
    standbytype *standby;
    iphdrtype *ip;
    udptype *udp;
    standbyinfotype *li;
    boolean authok;

    ip  = (iphdrtype *)ipheadstart(pak);
    udp = (udptype *) ipdatastart(pak);
    standby = (standbytype *) ((char *)udp + UDPHEADERBYTES);
    if (!idb->standbyidb)
	goto done;
    li = standby_get_group(idb, standby->group);

    if (pak->refcount != 1) {
	buginf("Standby: bad packet refcount, input interface %s",
	       pak->if_input ? pak->if_input->namestring : "internal");
#ifdef DEBUG
	bugpak("Standby:", pak);
#endif
    }

    if (!standby_mincfg(li))
	goto done;
    if (standby->version != STANDBY_VERSION)
	goto done;
    if (bcmp(standby->auth, li->auth, STANDBY_AUTH_LEN)) {
	errmsg(&msgsym(BADAUTH, STANDBY), ip->srcadr,
	       standby_statestring(standby->state));
	authok = FALSE;
    } else
	authok = TRUE;

    if (standby_debug)
	standby_bugpak(TRUE, pak, idb);

    switch (standby->opcode) {
    case LOPCODE_HELLO:
	standby_rcv_hello(pak, li, idb, standby, authok);
	break;
    case LOPCODE_COUP:
	standby_rcv_coup(pak, li, idb, standby);
	break;
    case LOPCODE_RESIGN:
	standby_rcv_resign(pak, li, idb, standby);
	break;
    default:
	break;
    }

 done:
    retbuffer(pak);
}

/*
 * standby_usebia
 * Should we use the interface's BIA for HSRP?
 */

static inline boolean standby_usebia (idbtype *swidb)
{
    if (!swidb->standbyidb)
        return(FALSE);
    return(swidb->standbyidb->flags & SFLAG_CFG_USEBIA);
}

/*
 * standby_pak_macaddr
 * Prepare packet to be encapsulated with a source MAC address of the
 * group's MAC address.  All this to help make learning bridges wake up and
 * notice when&if the MAC address moves from one network segment to another.
 */

static inline void standby_pak_macaddr (paktype *pak, idbtype *swidb,
					uchar group)
{
    if (swidb->hwptr->status & IDB_TR)
	return;
    pak->enc_flags |= ENC_SRCMAC_OVERRIDE;
    ieee_copy(standby_groups, pak->data_area);
    pak->data_area[IEEEBYTES-1] = group;
}

/*
 * standby_sendpacket
 * Send a packet to an address on an interface.
 */

static void standby_sendpacket (int opcode, ipaddrtype address, idbtype *idb,
				standbyinfotype *li)
{
    paktype *pak;
    standbytype *standby;
    int nbytes;
    iphdrtype *ip;
    udptype *udp;

    if (!standby_mincfg(li))
	return;
    pak = getbuffer(IPHEADERBYTES(NOPT) + UDPHEADERBYTES + 
		    STANDBY_MAXPACKETSIZE);
    if (!pak) {
	return;
    }
    
    ip = (iphdrtype *)ipheadstart(pak);

    pak->if_output = idb;
    udp = (udptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));
    standby = (standbytype *) ((char *)udp + UDPHEADERBYTES);
    udp->sport = STANDBY_PORT;
    udp->dport = STANDBY_PORT;
    udp->length = UDPHEADERBYTES + STANDBY_MAXPACKETSIZE;
    nbytes = STANDBY_MAXPACKETSIZE + UDPHEADERBYTES + IPHEADERBYTES(NOPT);
    ip->srcadr = idb->ip_address;
    if (li->state == SSTATE_ACTIVE) {
        /*
         * No need to override packet's MAC address on interface where
         * we have simply changed the primary MAC address, or where we
         * are using the interface's BIA.
         */
        if (!(li->flags & SFLAG_HWADDR_COERCE) && !standby_usebia(idb))
            standby_pak_macaddr(pak, idb, li->group);
    }

    standby->version = STANDBY_VERSION;
    standby->opcode = opcode;
    standby->state = li->state;
    standby->priority = li->priority;
    standby->hellotime = li->learned_hellotime;
    standby->holdtime = li->learned_holdtime;
    standby->group = li->group;
    standby->pad = 0;
    bcopy(li->auth, standby->auth, STANDBY_AUTH_LEN);
    standby->address = standby_primary_addr(li);

    udp->checksum = 0;
    udp->checksum = CheckSum((uchar *)udp, udp->length, idb->ip_address,
			     address, UDP_PROT);

    if (standby_debug)
	standby_bugpak(FALSE, pak, idb);

    ipwrite(pak, nbytes, UDP_PROT, idb->ip_address,
	    address, idb, NULL, &iprouteparams);
}

/*
 * standby_sendmulticast
 * Send a multicast on an interface.
 */

static void standby_sendmulticast (int opcode, idbtype *idb,
				   standbyinfotype *si)
{
    standby_sendpacket(opcode, ALLROUTERS, idb, si);
}

/*
 * standby_protocol_macaddr
 * Compute MAC address to use for the supplied group #.  Return result in dst.
 */

static inline void standby_protocol_macaddr (idbtype *swidb, uchar group,
					     uchar *dst)
{
    hwidbtype   *hwidb;

    hwidb = swidb->hwptr;
    if (standby_usebia(swidb)) {
        ieee_copy(hwidb->hardware, dst);
        return;
    }
    if (hwidb->status & IDB_TR) {
	ieee_zero(dst);
	dst[0] = TR_GROUP|TR_LOCAL;
	dst[3] = (1 << (group & 0x3));
    } else {
	ieee_copy(standby_groups, dst);
	dst[IEEEBYTES-1] = group;
    }
}

/*
 * standby_ip_forus
 * Determine if the given IP address is the active standby protocol
 * IP address.
 */

static boolean standby_ip_forus (idbtype *idb, ipaddrtype addr)
{
    standbyinfotype *si;
    standbyaddrtype *siaddr;

    FOR_ALL_SBGROUPS(idb, si) {
	if (!standby_have_addr(si))
	    continue;
        if (si->state != SSTATE_ACTIVE)
            continue;
	FOR_ALL_SBADDRS(si, siaddr) {
	    if (siaddr->addr == addr)
		return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * standby_update_arp
 * Add or remove an ARP table entry for the standby protocol MAC/IP
 * address.  Also send out a gratuitous ARP response when adding
 * an ARP table entry.
 */

static void standby_update_arp (idbtype *idb, standbyinfotype *si, boolean up)
{
    uchar standby_mac[IEEEBYTES];
    hwidbtype *hwidb;
    standbyaddrtype *siaddr;
    
    if (si == NULL)
	return;
    if (up) {
	if (si->state != SSTATE_ACTIVE)
	    return;
	standby_protocol_macaddr(idb, si->group, standby_mac);
	hwidb = idb->hwptr;
	FOR_ALL_SBADDRS(si, siaddr) {
	    ip_arp_table_add(idb, siaddr->addr, standby_mac,
			     hwidb->enctype, ARP_INTERFACE);
	    ip_arp_unsolicited(idb, siaddr->addr, NULL, standby_mac);
	}
    } else {
	FOR_ALL_SBADDRS(si, siaddr)
	    arp_table_remove(siaddr->addr, LINK_IP);
    }
}

/*
 * standby_arp_reload
 * IP arp table has been reloaded.  Re-establish hot standby ARP table entry.
 */

static void standby_arp_reload (idbtype *idb)
{
    standbyinfotype *si;

    FOR_ALL_SBGROUPS(idb, si)
	standby_update_arp(idb, si, TRUE);
}

/*
 * standby_ip_dup
 * Update the arp entry if it is detected duplicated.
 */

static boolean standby_ip_dup (idbtype *idb, ipaddrtype target)
{
    uchar standby_mac[IEEEBYTES];
    standbyinfotype *si;
    standbyaddrtype *siaddr;
    hwidbtype *hwidb;

    FOR_ALL_SBGROUPS(idb, si) {
        if (si->state != SSTATE_ACTIVE)
            continue;

        FOR_ALL_SBADDRS(si, siaddr) {
            if (siaddr->addr == target) {
                standby_sendmulticast(LOPCODE_HELLO, idb, si);
                standby_protocol_macaddr(idb, si->group, standby_mac);
                hwidb = idb->hwptr;
                if (standby_debug)
                    buginf("\nSB%d: Update arp cache of %i with %e",
                           si->group, target, standby_mac);
                ip_arp_table_add(idb, target, standby_mac,
                                 hwidb->enctype, ARP_INTERFACE);
                ip_arp_unsolicited(idb, target, NULL, standby_mac);
                return(TRUE);
            }
        }
    }
    return (FALSE);
}

/*
 * standby_proxyarp
 * Determine if system should answer proxy ARP request.  If not
 * configured to run hot standby on this interface, ignore the ARP.
 *
 * We used to only send proxy ARP responses when the interface is in active
 * state, but the active router may neglect to send such a response if
 * the best routed path happens to be through us (the non-active router).
 * If we are using HSRP in the mode where it doesn't use a virtual MAC
 * address, proxy ARP responses from non-active routers could cause
 * problems, so suppress them. Otherwise, send a proxy ARP response
 * regardless of HSRP state, using the HSRP group's MAC address.
 */

static boolean standby_proxyarp (idbtype *idb, ipaddrtype source,
				 ipaddrtype target, boolean *allow, uchar
				 *mac_addr)
{
    standbyinfotype *si;

    FOR_ALL_SBGROUPS(idb, si) {
	if (!standby_have_addr(si))
	    continue;
	*allow = TRUE;
        if (standby_usebia(idb)) {
            /*
             * Suppress proxy ARP responses if we are not the active
             * router, and our HSRP address is different from the
             * active router's. Active router should provide the proxy
             * ARP response in this case.
             */
            *allow = (si->state == SSTATE_ACTIVE);
        }
        if (*allow)
            standby_protocol_macaddr(idb, si->group, mac_addr);
	return(TRUE);
    }
    return(FALSE);
}

/*
 * standby_tr_functional_change
 * Map a group number to a TR functional address, and revise the interface's
 * functional address to include/exclude the mapped address.
 */

static inline void standby_tr_functional_change (hwidbtype *hwidb,
						 uchar group, boolean sense)
{
    ulong functional;
    ulong tr_functional;
    ONE_ITEM_LIST(tmp, TOKEN_HW_FUNCTIONAL, sizeof(ulong), 0);

    switch (group) {
    case 0:
    default:
	functional = TRFUNC_STANDBY_0;
	break;
    case 1:
	functional = TRFUNC_STANDBY_1;
	break;
    case 2:
	functional = TRFUNC_STANDBY_2;
	break;
    }

    /*
     * Get current value
     */
    (*hwidb->state_config)(hwidb, IDB_CONTROL_READ_CONFIG, &tmp_list);
    tr_functional = tmp_item.u.l_value;

    if (sense)
	tr_functional |= functional;
    else
	tr_functional &= ~functional;

    /*
     * Write the new value
     */
    tmp_item.u.l_value = tr_functional;
    tmp_item.handled = FALSE;
    itemlist_reset(&tmp_list);
    (*hwidb->state_config)(hwidb, IDB_CONTROL_SET_CONFIG, &tmp_list);
}

/*
 * standby_if_deactivate
 * Stop acting as the active router on the designated interface.
 */

static void standby_if_deactivate (idbtype *idb, standbyinfotype *si)
{
    hwidbtype *hwidb;
    standbyaddrtype *siaddr;

    hwidb = idb->hwptr;
    FOR_ALL_SBADDRS(si, siaddr)
	ipaddr_table_delete(siaddr->addr, idb);
    if (si->flags & SFLAG_HWADDR_COERCE) {
	coerce_hwaddr(hwidb->bia, hwidb);
	si->flags &= ~SFLAG_HWADDR_COERCE;
    } else if (standby_usebia(idb)) {
        /*
         * Nothing needs to be done
         */
    } else {
	/*
         * Need to deconfigure the HSRP virtual MAC.
         *
	 * if this is an ATM interface, a LANE Client
	 * MAY be running here.  If so, do the right
	 * thing by calling the client routine.
	 */
	if (is_atm(hwidb)) {
	    reg_invoke_lec_hsrp_action(idb, FALSE, si->group);
	} else {
	    /*
	     * Should really deconfigure MAC without 
	     * resetting interface.  For token ring, we can.
	     */
	    if (hwidb->set_extra_addrs != NULL) {
		standby_tr_functional_change(hwidb, si->group, FALSE);
		(*hwidb->set_extra_addrs)(hwidb);
            } else if (is_ethernet(hwidb)) {
		if (hwidb->afilter_reset)
		    (*hwidb->afilter_reset)(hwidb);
		else
		    (*hwidb->reset)(hwidb);
	    } else {
                hwidb->reset_request = TRUE;
	    }
	}
    }
    standby_update_arp(idb, si, FALSE);
}

/*
 * standby_can_add_hwaddr
 * Determine if interface supports addition of a second unicast MAC
 * address.
 * Inerface hardware may be unable to support this, or the user may
 * have disabled this capability in software.
 */

static inline boolean standby_can_add_hwaddr (idbtype *swidb)
{
    hwidbtype   *hwidb;

    hwidb = swidb->hwptr;
    if ((hwidb->type == IDBTYPE_LANCE) ||
      (hwidb->type == IDBTYPE_QUICC_ETHER))
	return(FALSE);
    if (standby_usebia(swidb))
        return(FALSE);
    return(TRUE);
}

/*
 * standby_update_afilter
 * Update an interface's MAC address filter so that it will listen
 * to multiple unicast MAC addresses.
 */

static inline void standby_update_afilter (idbtype *idb, hwidbtype *hwidb,
					   uchar *mac, boolean dynamic)
{
    uchar bitswapped_address[IEEEBYTES];

    if (standby_debug)
	buginf("\nSB: %s Adding %e to address filter", idb->namestring,
	       mac);

    /* on an ATM i/f, nothing needs to be done */
    if (! is_atm(hwidb)) {
	if (dynamic && idb->hwptr->afilter_reset) {
	    (*idb->hwptr->afilter_reset)(idb->hwptr);
	} else {
	    (*hwidb->listen)(idb, mac, FCI_HSRP_STATION_ADDR);
	    /*
	     * Need to update DA field of address filter with FSM bits for
	     * IP.  Only necessary when an extended address filter is being 
	     * used.
	     */
	    if (hwidb->status & IDB_CBUS)
		(*hwidb->listen)(idb, mac, FCI_DOD_IP_PROTOCOL);
	}
    }
    /* CSCdi63104 If token ring mac, make canonical for SMF entry */
    if (hwidb->set_extra_addrs != NULL) {
        ieee_swap(mac, bitswapped_address);
        mac = bitswapped_address;
    }
    reg_invoke_tbridge_smf_update(mac,
				  idb,
				  SMF_MACTYP_STANDBY_UNICAST |
				  SMF_ACTION_RECEIVE);
}


/*
 * standby_if_activate
 * Begin acting as the active router on the designated interface.
 */

static void standby_if_activate (idbtype *idb, standbyinfotype *li)
{
    uchar standby_mac[IEEEBYTES];
    hwidbtype *hwidb;
    standbyaddrtype *siaddr;

    /*
     * If we haven't learned the hot standby pseudo IP address
     */
    if (!standby_have_addr(li))
	return;		
    standby_protocol_macaddr(idb, li->group, standby_mac);
    FOR_ALL_SBADDRS(li, siaddr)
	ipaddr_table_insert(siaddr->addr, idb, IPADDR_DYN_ALIAS,
			    IPADDRFLAG_NO_ARP_RELOAD, 0, NULL);
    hwidb = idb->hwptr;
    if (standby_can_add_hwaddr(idb)) {
	standby_update_afilter(idb, hwidb, standby_mac, TRUE);

	/* if a LANE Client is running, do this */
	if (is_atm(hwidb))
	    reg_invoke_lec_hsrp_action(idb, TRUE, li->group);
	else
	    /*
	     * If this is token ring, need to set up 
	     * the hot standby functional address.
	     */
	    if (hwidb->set_extra_addrs != NULL)
		(*hwidb->set_extra_addrs)(hwidb);
	/*
	 * Must generate a gratuitous packet using the standby_mac address
	 * as the source address so as to wake up any learning bridges
	 * which might otherwise not notice that the hot standby MAC address
	 * has moved to a different network segment.
	 */
	standby_update_arp(idb, li, TRUE);
    } else if (standby_usebia(idb)) {
        /*
         * Only need to gratuitous arp in this case.
         */
        standby_update_arp(idb, li, TRUE);
    } else {
	if (standby_debug)
	    buginf("\nSB: %s changing MAC address to %e", idb->namestring,
		   standby_mac);
	if (RUNNING_IF(LINK_DECNET, idb)) {
	    errmsg(&msgsym(MISCONFIG, STANDBY), idb->namestring, standby_mac);
	    return;
	}
	/*
	 * Interface cannot add a second unicast MAC address.  We must
	 * resort to clobbering the existing MAC address with the
	 * hot standby protocol MAC address.
	 * Gratuitous ARPing happens as a side effect of the following
	 * operation.
	 */
	coerce_hwaddr(standby_mac, hwidb);
	li->flags |= SFLAG_HWADDR_COERCE;
    }
}

/*
 * standby_state
 * Change state to the given state.
 */

static void standby_state (int newstate, standbyinfotype *li, idbtype *idb)
{
    boolean resigning;

    if ((li->state == SSTATE_ACTIVE) &&
	(newstate != li->state))
	resigning = TRUE;
    else
	resigning = FALSE;
    switch (newstate) {
    case SSTATE_VIRGIN:
	TIMER_STOP(li->activetimer);
	TIMER_STOP(li->standbytimer);
	li->active = 0;
	li->standby = 0;
	break;
    case SSTATE_LEARN:
	li->learned_holdtime = standby_ms2sec(li->holdtime);
	li->learned_hellotime = standby_ms2sec(li->hellotime);
	if (standby_have_addr(li))
	    newstate = SSTATE_LISTEN;
	break;
    case SSTATE_LISTEN:
    case SSTATE_SPEAK:
	break;
    case SSTATE_STANDBY:
	TIMER_STOP(li->standbytimer);
	break;
    case SSTATE_ACTIVE:
	if (li->flags & SFLAG_CFG_TIMERS) {
	    li->learned_hellotime = standby_ms2sec(li->hellotime);
	    li->learned_holdtime = standby_ms2sec(li->holdtime);
	}
        li->re_arp = STANDBY_ARP_RESENT;
	TIMER_STOP(li->activetimer);
	break;
    default:
	break;
    }

    if (standby_debug)
	buginf("\nSB%d: %s state %s -> %s", li->group, idb->namestring,
	       standby_statestring(li->state), standby_statestring(newstate));

    li->state = newstate;
    /*
     * Need to change state before resigning or becoming active
     * so that ARP related code knows whether or not to gratuitous ARP for
     * the hot standby protocol address during the interface's MAC address
     * adjustment.
     */
    if (resigning) {
	standby_sendmulticast(LOPCODE_RESIGN, idb, li);
	standby_if_deactivate(idb, li);
    } else if (li->state == SSTATE_ACTIVE) {
	standby_if_activate(idb, li);
    }
}

/*
 * standby_next_activate_time
 * Figure out which of our non-expired timers is going to go off first.
 */

static sys_timestamp standby_next_activate_time (
    standbyinfotype *li,
    sys_timestamp nexttime)
{
    if (TIMER_RUNNING_AND_SLEEPING(li->activetimer))
	nexttime = TIMER_SOONEST(nexttime, li->activetimer);
    if (TIMER_RUNNING_AND_SLEEPING(li->standbytimer))
	nexttime = TIMER_SOONEST(nexttime, li->standbytimer);
    nexttime = TIMER_SOONEST(nexttime, li->nexthello);
    return(nexttime);
}

/*
 * standby_service_interface
 * Send HELLOs on an interface
 * Possibly age out current active router.
 * Return the time to wake up again
 */

static void standby_service_interface (idbtype *idb, standbyinfotype *li)
{
    /*
     * Check for active timer to expire.
     */
    if (TIMER_RUNNING_AND_AWAKE(li->activetimer)) {
	li->active = 0;
	switch (li->state) {
	case SSTATE_LISTEN:
	    RESTART_ACTIVETIMER;
	    RESTART_STANDBYTIMER;
	    standby_state(SSTATE_SPEAK, li, idb);
	    break;
	case SSTATE_STANDBY:
	    standby_state(SSTATE_ACTIVE, li, idb);
	    break;
	default:
	    break;
	}
    }

    /*
     * Check for standby timer to expire.
     */
    if (TIMER_RUNNING_AND_AWAKE(li->standbytimer)) {
	li->standby = 0;
	switch (li->state) {
	case SSTATE_LISTEN:
	    RESTART_STANDBYTIMER;
	    standby_state(SSTATE_SPEAK, li, idb);
	    break;
	case SSTATE_SPEAK:
	    standby_state(SSTATE_STANDBY, li, idb);
	default:
	    break;
	}
    }

    /*
     * Now, send hellos if necessary.
     */
    if (AWAKE(li->nexthello)) {
	if (li->state & SSTATE_SPEAKER) {
	    if (li->state == SSTATE_ACTIVE) {
                if (li->flags & SFLAG_CFG_TIMERS) {
                    li->learned_hellotime = standby_ms2sec(li->hellotime);
                    li->learned_holdtime = standby_ms2sec(li->holdtime);
                }
                if (li->re_arp > 0) {
                    li->re_arp--;
                    standby_update_arp(idb, li, TRUE);
                }
	    }
	    standby_sendmulticast(LOPCODE_HELLO, idb, li);
	}
	TIMER_START(li->nexthello, (li->learned_hellotime ?
                                    standby_sec2ms(li->learned_hellotime) :
                                    li->hellotime));
    }
}

/*
 * standbyBLOCK
 * Block if the timer is still running and if the input queue is empty.
 */

static boolean standbyBLOCK (void)
{
    if (standby_wakeup) {
	standby_wakeup = FALSE;
	return(FALSE);
    }

    if (!process_is_queue_empty(socket->inq))
	return(FALSE);

    if (AWAKE(standby_nexthellotime))
	return(FALSE);

    return(TRUE);
}

/*
 * standby_shutdown_swif
 * Shut down a software interface.
 * Change state to INIT no matter what the current state is.
 * standby_state() will take actions to resign if needed.
 */

static void standby_shutdown_swif (idbtype *idb)
{
    standbyinfotype *si;

    FOR_ALL_SBGROUPS(idb, si) {
        standby_state(SSTATE_VIRGIN, si, idb);
    }
}

/*
 * standby_resign_all_ifs
 * Resign from all interfaces, when reloading.
 */

static void standby_resign_all_ifs (void)
{
    idbtype *idb;
    standbyinfotype *si;

    FOR_ALL_SWIDBS(idb) 
        FOR_ALL_SBGROUPS(idb, si) {
            if (si->state == SSTATE_ACTIVE)
                standby_state(SSTATE_VIRGIN, si, idb);
        }
}

/*
 * standby_pick_priority
 * Determine hot standby priority.  Normally just the configured/default
 * value, but is dynamically decreased if an interface which is being
 * tracked is down.
 */

static uchar standby_pick_priority (standbyinfotype *si)
{
    int if_index;
    int pri_dec;

    if (si->upif_cnt == si->trackif_cnt)
	return(si->cfg_priority);
    pri_dec = 0;
    for (if_index = 0; if_index < MAX_INTERFACES; if_index++) {
	if (!CHECKBIT(if_index, si->trackifs))
	    continue;
	/*
	 * This if_index is being tracked.  Is it up?  If not, consider
	 * its track priority...
	 */
	if (CHECKBIT(if_index, si->upifs))
	    continue;
	pri_dec += si->if_pri[if_index];
    }
    if (!pri_dec) {
	/*
	 * Track priorities were not explicitly configured.  Use the default
	 * decrement.
	 */
	 pri_dec = STANDBY_PRI_DEC;
    }
    /*
     * Avoid wrap
     */
    if (si->cfg_priority < pri_dec)
	return(0);
    return(si->cfg_priority - pri_dec);
}

/*
 * standby_if_state
 * Revise bit table if interesting or up interfaces with new information.
 */

static void standby_if_state (int index, boolean sense, uchar *table,
			      ushort *cnt)
{
    if (table == NULL)
	return;
    if (sense) {
	if (!CHECKBIT(index, table)) {
	    SETBIT(index, table);
	    (*cnt)++;
	}
    } else {
	if (CHECKBIT(index, table)) {
	    (*cnt)--;
	    CLEARBIT(index, table);
	}
    }
}

/*
 * standby_track_if
 * Loop through all interfaces running hot standby.  See if we are tracking
 * interface state changes for the passed in interface.  If so, revise
 * our up/down counts.  If there is an interface which is down that we are
 * tracking, adjust the hot standby priority.
 */

static void standby_track_if (idbtype *idb, boolean ifup)
{
    int index;
    uchar new_priority;
    standbyinfotype *si;
    idbtype *sbidb;

    index = idb->if_number;
    FOR_ALL_SWIDBS(sbidb) {
	FOR_ALL_SBGROUPS(sbidb, si) {
	    /*
	     * Not tracking any interfaces
	     */
	    if (!si->upifs)
		continue;
	    /*
	     * Are we tracking this interface?
	     */
	    if (!CHECKBIT(index, si->trackifs))
		continue;
	    /*
	     * Revise our notion of this interface's state.  Update our count
	     * of up interfaces.
	     */
	    standby_if_state(idb->if_number, ifup, si->upifs, &si->upif_cnt);
	    if (standby_debug)
		buginf("\nSB%d: %s Now %d/%d tracked interfaces up", si->group,
		       sbidb->namestring, si->upif_cnt, si->trackif_cnt);
	    new_priority = standby_pick_priority(si);
	    if (new_priority == si->priority)
		continue;
	    if (standby_debug)
		buginf("\nSB%d: %s Priority was %d now %d, configured as %d",
		       si->group, sbidb->namestring, si->priority,
		       new_priority, si->cfg_priority);
	    si->priority = new_priority;
	    /*
	     * Our priority has changed dynamically.  Tell others right
	     * away.
	     */
	    if (si->state & SSTATE_SPEAKER)
		standby_sendmulticast(LOPCODE_HELLO, sbidb, si);
	}
    }
}

/*
 * standby_swif_goingdown
 * Interface is going down.  Attempt to resign from swidb, and keep
 * track of interface state if necessary.
 */

static void standby_swif_goingdown (idbtype *idb)
{
    standby_shutdown_swif(idb);
    standby_track_if(idb, FALSE);
}

/*
 * standby_macaddr_update
 * An interface's MAC address filter has been reset.  Refresh our entry
 * in the address filter, if we had one there before the reset.
 */

static void standby_macaddr_update (idbtype *idb)
{
    standbyinfotype *si;
    uchar standby_mac[IEEEBYTES];
    hwidbtype *hwidb;

    if (!standby_can_add_hwaddr(idb))
	return;
    hwidb = idb->hwptr;
    FOR_ALL_SBGROUPS(idb, si) {
	standby_protocol_macaddr(idb, si->group, standby_mac);
	if (si->state == SSTATE_ACTIVE)
            standby_update_afilter(idb, hwidb, standby_mac, FALSE);
        else
            reg_invoke_tbridge_smf_delete(standby_mac,
                                          idb,
                                          SMF_MACTYP_STANDBY_UNICAST |
                                          SMF_ACTION_RECEIVE);
    }
}

/*
 * standby_ip_startup_inline
 * Even though hot standby protocol is already configured, we may have
 * been unable to obtain a UDP socket when trying to initialize.
 * Now if the interface is up, it is time to complete initialization.
 */

static inline void standby_ip_startup_inline (idbtype *idb)
{
    if (!standby_pid && interface_up(idb)) {
	standby_process_init();
    }
}

/*
 * standby_ip_startup
 * An interface's IP address was just configured.  Perhaps we don't
 * already have a UDP socket?
 */

static void standby_ip_startup (
    idbtype *idb,
    ipaddrtype address,
    ipaddrtype mask,
    boolean secondary,			 
    boolean adding)
{
    standbyinfotype *si;

    FOR_ALL_SBGROUPS(idb, si) {
	if (standby_mincfg(si)) {
	    standby_ip_startup_inline(idb);
	    return;
	}
    }
}

/*
 * standby_learn_swif
 * Keep track of interface state.
 * Move an interface to learn state if it's ready.
 */

static void standby_learn_swif (idbtype *idb)
{
    standbyinfotype *li;
    boolean ifup;

    ifup = interface_up(idb);
    standby_track_if(idb, ifup);
    FOR_ALL_SBGROUPS(idb, li) {
	if (!standby_mincfg(li))
	    continue;
	standby_ip_startup_inline(idb);
	if ((li->state == SSTATE_VIRGIN) && ifup) {
	    RESTART_ACTIVETIMER;
	    RESTART_STANDBYTIMER;
	    standby_state(SSTATE_LEARN, li, idb);
	}
    }
}

/*
 * standby_state_change
 * Handle a state change for a particular group
 */

static void standby_state_change (idbtype *idb, standbyinfotype *li)
{
    if ((li->state != SSTATE_VIRGIN) &&
	(!interface_up(idb) || !standby_mincfg(li)))
	standby_state(SSTATE_VIRGIN, li, idb);
    else if (((li->state == SSTATE_VIRGIN) ||
	      ((li->state == SSTATE_LEARN) && standby_have_addr(li)))) {
	if (interface_up(idb) && standby_mincfg(li)) {
	    RESTART_ACTIVETIMER;
	    RESTART_STANDBYTIMER;
	    standby_state(SSTATE_LEARN, li, idb);
	}
    }
}

/*
 * standby_state_change_hwif
 * Handle an interface state change.
 */

static void standby_state_change_hwif (hwidbtype *hwidb)
{
    standbyinfotype *si;
    idbtype *idb;

    FOR_ALL_SWIDBS_ON_HW(hwidb, idb)
	FOR_ALL_SBGROUPS(idb, si)
	    standby_state_change(idb, si);
}

/*
 * standby_process_state_changes
 * Handle hot standby protocol configuration state changes which may affect
 * the hot standby process.
 */

static void standby_process_state_change (standbyinfotype *li)
{
    idbtype *idb;
    
    if (standby_mincfg(li)) {
	if (!standby_pid) {
	    /*
	     * Start the process if it hasn't been started already.
	     */
	    standby_process_init();
	} else {
	    /*
	     * Wake up sleeping process, to prevent it from possibly
	     * sleeping too long.
	     */
	    standby_wakeup = TRUE;
	}
	return;
    }
    /*
     * May be time to shut down hot standby process
     * If
     *   hot standby process is running and 
     *   no interfaces remain configured with
     *   minimal hot standby protocol information
     * then
     *   shutdown hot standby process
     */
    if (!standby_pid)
	return;
    FOR_ALL_SWIDBS(idb) {
        if (standby_if_mincfg(idb))
            return;
    }
    if (standby_debug)
	buginf("\nSB: Shutting down hot standby process");

    /*
     * No need to continue running hot standby protocol process
     */
    close_ipsocket(socket);
    socket = NULL;
    process_kill(standby_pid);
    standby_pid = 0;
}

/*
 * standby_set_tr_functional
 * Setup token ring functional address for hot standby protocol.
 */

static void standby_set_tr_functional (hwidbtype *hwidb, idbtype *swidb)
{
    standbyinfotype *si;

    FOR_ALL_SBGROUPS(swidb, si) {
	if (!standby_mincfg(si))
	    continue;
	standby_tr_functional_change(hwidb, si->group,
				     (si->state == SSTATE_ACTIVE));
    }
}

/*
 * standby_check_if_reset
 * Check for interface reset requests.
 * Called by periodic_activity() via reg_invoke_if_onesec().
 */
static void standby_check_if_reset (hwidbtype *hwidb)
{
    if (hwidb->reset_request){
        hwidb->reset(hwidb);
        hwidb->reset_request = FALSE;
    }
    return;
}

/*
 * standby_find_group
 *
 * Use the hash table to find the standbyaddrtype that
 * matches and hence the standbyinfotype.
 *
 * Return NULL if no match
 */

static standbyinfotype *
standby_find_group (ipaddrtype address)
{
    standbyaddrtype *sa;
    
    sa = standby_hash_lookup(address);
    if (sa == NULL) {
	return(NULL);
    } else {
	return(sa->sbgroup);
    }
}

/*
 * standby_ip_address_reserved
 *
 * Check if the supplied address is the primary standby ip
 * or one of the secondary standby ip on the supplied interface.
 */
static boolean standby_ip_address_reserved (idbtype *idb, ipaddrtype address)
{
    standbyinfotype *si;

    si = standby_find_group(address);
    if (si && si->idb == idb)
        return(TRUE);
    return(FALSE);
}

/*
 * standby_register
 * Add registrations.
 * Assume that standby_registered is FALSE when it is called.
 */
static void standby_register(void)
{
    reg_add_will_reload(standby_resign_all_ifs, "standby_resign_all_ifs");
    reg_add_swif_goingdown(standby_swif_goingdown,
                           "standby_swif_goingdown");
    reg_add_hwif_state_change(standby_state_change_hwif,
                              "standby_state_change_hwif");
    reg_add_swif_comingup(standby_learn_swif, "standby_learn_swif");
    reg_add_ip_address_change(standby_ip_startup, "standby_ip_startup");
    reg_add_ip_address_reserved(standby_ip_address_reserved,
                                "standby_ip_address_reserved");
    reg_add_media_tring_add_functional(standby_set_tr_functional,
                                       "standby_set_tr_functional");
    reg_add_arp_reload(standby_arp_reload, "standby_arp_reload");
    reg_add_ip_dupaddr_action(standby_ip_dup, "standby_ip_dup");
    reg_add_ip_arp_for_proto(standby_ip_forus, "standby_ip_forus");
    reg_add_ip_proxyarp_proto_action(standby_proxyarp, "standby_proxyarp");
    reg_add_media_interesting_addresses(standby_macaddr_update,
                                        "standby_macaddr_update");
    reg_add_if_onesec(standby_check_if_reset, "standby_check_reset");
    standby_registered = TRUE;
}

/*
 * standby_process_init
 * Assume that standby process has not been created yet.
 * Add registration, open UDP socket, and create standby process.
 * This was extracted from standby_process() to fix CSCdi60445.
 * Register HSRP before standby process is created to take care of
 * interface state change before the first time new process is scheduled.
 */
void standby_process_init (void)
{
    if (standby_debug)
	buginf("\nSB: Add registration.");

    if (!standby_registered) {
        standby_register();
    }
    if (standby_debug)
	buginf("\nSB: Open UDP socket.");

    socket = open_ipsocket(UDP_PROT, 0, 0, STANDBY_PORT,
			   STANDBY_PORT, SOCK_ANYSRC);
    if (socket) {
        if (standby_debug)
            buginf("\nSB: Create hot standby process.");

        standby_pid = cfork(standby_process, 0, 0, "Hot standby", 0);
    } else {
	errmsg(&msgsym(NOSOCKET, STANDBY));
    }
    return;
}

/*
 * standby_process
 * Fork for implementing the hot standby protocol.
 */

forktype standby_process (void)
{
    idbtype *idb;
    standbyinfotype *si;
    paktype *pak;
    udptype *udp;
    int dummy;

    if (standby_debug)
	buginf("\nSB: Starting up hot standby process");

    standby_wakeup = FALSE;
    TIMER_START(standby_nexthellotime, standby_sec2ms(SDEFAULT_HELLOTIME));
    while (TRUE) {
	edisms((blockproc *) standbyBLOCK, 0);
	TIMER_START(standby_nexthellotime,
                    standby_sec2ms(STANDBY_MAXHELLOTIME));

	/*
	 * First, empty the input queue
	 */
	
	while ((pak = read_ipsocket(socket, FALSE, 0, &udp, &dummy))) {
	    standby_rcvpacket(pak, pak->if_input);
	}

	/*
	 * Now, send out hellos as necesary.
	 */
	FOR_ALL_SWIDBS(idb) {
	    FOR_ALL_SBGROUPS(idb, si) {
		if (!standby_mincfg(si))
		    continue;
		standby_service_interface(idb, si);
		standby_nexthellotime =
		    standby_next_activate_time(si, standby_nexthellotime);
	    }
	}
    }
}

/*
 * standby_opcodestring
 * Return the string corresponding to an opcode.
 */

static char *standby_opcodestring (int opcode)
{
    switch (opcode) {
    case LOPCODE_HELLO: return("Hello");
    case LOPCODE_COUP:  return("Coup");
    case LOPCODE_RESIGN:return("Resign");
    default:
	break;
    }
    return("NOP");
}

/*
 * standby_bugpak
 * Decode a hello and print it out.
 */

static void standby_bugpak (boolean received, paktype *pak, idbtype *idb)
{
    iphdrtype *ip;
    udptype *udp;
    standbytype *standby;

    ip = (iphdrtype *)ipheadstart(pak);

    if (received)
	udp = (udptype *) ipdatastart(pak);
    else
	udp = (udptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));

    standby = (standbytype *) ((char *)udp + UDPHEADERBYTES);
    buginf("\nSB%d:%s %s %s %i %s pri %d hel %d hol %d ip %i", standby->group,
	   idb->namestring, standby_opcodestring(standby->opcode), received ?
	   "in" : "out", ip->srcadr, standby_statestring(standby->state),
	   standby->priority, standby->hellotime, standby->holdtime,
	   standby->address); 
}

/*
 * standby_statestring
 * Return the string associated with a particular state.
 */

static char *standby_statestring (int state)
{
    switch (state) {
    case SSTATE_VIRGIN: return("Init");
    case SSTATE_LEARN:  return("Learn");
    case SSTATE_LISTEN: return("Listen");
    case SSTATE_SPEAK:  return("Speak");
    case SSTATE_STANDBY: return("Standby");
    case SSTATE_ACTIVE: return("Active");
    default:
	break;
    }
    return("Confused");
}

/*
 * standby_track_command
 * Parse the track subcommand.  Allocate/deallocate tables for tracking
 * interface state changes.
 */

static void standby_track_command (standbyinfotype *si, idbtype *idb,
				   int pri_dec, boolean sense)
{
    boolean ifup;
    
    if (si->trackifs == NULL) {
	if (sense == FALSE)
	    return;
	si->trackifs = malloc(MAX_INTERFACES / 8);
	if (si->trackifs == NULL) {
	    printf(nomemory);
	    return;
	}
	si->upifs = malloc(MAX_INTERFACES / 8);
	if (si->upifs == NULL) {
	    printf(nomemory);
	    free(si->trackifs);
	    si->trackifs = NULL;
	    return;
	}
	si->if_pri = malloc(MAX_INTERFACES);
	if (si->if_pri == NULL) {
	    printf(nomemory);
	    free(si->trackifs);
	    free(si->upifs);
	    si->trackifs = si->upifs = NULL;
	    return;
	}
    }
    /*
     * Mark/unmark this particular interface for tracking
     */
    standby_if_state(idb->if_number, sense, si->trackifs, &si->trackif_cnt);
    if (!sense)
	pri_dec = 0;
    si->if_pri[idb->if_number] = pri_dec;
    if (sense == FALSE && si->trackif_cnt == 0) {
	/*
	 * Not tracking anything anymore.  Deallocate resources
	 */
	free(si->trackifs);
	free(si->upifs);
	free(si->if_pri);
	si->trackifs = si->upifs = si->if_pri = NULL;
	si->upif_cnt = 0;
	si->priority = standby_pick_priority(si);
	return;
    }
    /*
     * Initialize state for this tracked/untracked interface,
     * and possibly change our priority right away.
     */
    ifup = interface_up(idb) && sense;
    standby_if_state(idb->if_number, ifup, si->upifs, &si->upif_cnt);
    si->priority = standby_pick_priority(si);
}

/*
 * standby_nvgen
 * Handle command generation of all the hot standby interface commands.
 */

void standby_nvgen (idbtype *idb, standbyinfotype *si)
{
    idbtype *trackidb;
    uchar group;
    uchar nv_command[12];
    standbyaddrtype *siaddr;

    group = si->group;
    if (group)
	sprintf(nv_command, "standby %d", group);
    else
	sprintf(nv_command, "standby");
    if (si->flags & SFLAG_CFG_TIMERS) {
	nv_write(TRUE, "%s timers ", nv_command);
        if (si->hellotime < ONESEC) {
            nv_add(TRUE, "msec %d ", si->hellotime);
        } else {
            nv_add(TRUE, "%d ", standby_ms2sec(si->hellotime));
        }
        if (si->holdtime < ONESEC) {
            nv_add(TRUE, "msec %d", si->holdtime);
        } else {
            nv_add(TRUE, "%d", standby_ms2sec(si->holdtime));
        }
    }
    if (si->flags & SFLAG_CFG_PRIORITY)
	nv_write(TRUE, "%s priority %d", nv_command, si->cfg_priority);
    if (si->flags & SFLAG_CFG_PREEMPT)
	nv_write(TRUE, "%s preempt", nv_command);
    if (si->flags & SFLAG_CFG_AUTH)
	nv_write(TRUE, "%s authentication %s", nv_command, si->auth);
    if (si->flags & SFLAG_CFG_IP) {
	if (si->flags & SFLAG_CFG_IP_ADDR)
	    nv_write(TRUE, "%s ip %i", nv_command, standby_primary_addr(si));
	else
	    nv_write(TRUE, "%s ip", nv_command);
	for (siaddr = si->addrs.next_in_group; 
	     siaddr; siaddr = siaddr->next_in_group)
	    nv_write(TRUE, "%s ip %i secondary", nv_command, siaddr->addr);
    }
    if ((si->flags & SFLAG_CFG_TRACK) && si->trackifs) {
	FOR_ALL_SWIDBS(trackidb) {
	    if (CHECKBIT(trackidb->if_number, si->trackifs)) {
		nv_write(TRUE, "%s track %s", nv_command,
			 trackidb->namestring);
		nv_add(si->if_pri[trackidb->if_number],
		       " %d", si->if_pri[trackidb->if_number]);
	    }
	}
    }
}

/*
 * standby_new_group
 *
 * Allocate the memory and 
 * initialize standbyinfo fields to their default.
 */

static standbyinfotype *
standby_new_group (uchar group, idbtype *idb)
{
    standbyinfotype *si;

    si = malloc(sizeof(standbyinfotype));
    if (si != NULL) {
	si->flags = 0;
	si->state = SSTATE_VIRGIN;
	standby_primary_addr(si) = SDEFAULT_IP_ADDR;
	memset(si->auth, 0, STANDBY_AUTH_LEN);
	sstrncpy(si->auth, sdefault_auth, STANDBY_AUTH_LEN);
	si->priority = si->cfg_priority = SDEFAULT_PRIORITY;
	si->group = group;
	si->hellotime = standby_sec2ms(SDEFAULT_HELLOTIME);
	si->holdtime = standby_sec2ms(SDEFAULT_HOLDTIME);
	TIMER_STOP(si->activetimer);
	TIMER_STOP(si->standbytimer);
	GET_TIMESTAMP(si->nexthello);
	si->idb = idb;
    }
    return(si);
}

/*
 * standby_revise_chain
 * Add new group to chain, and keep them sorted by group number for
 * deterministic NVGENeration.
 */

static void standby_revise_chain (idbtype *idb, standbyinfotype *li)
{
    standbyinfotype *pred, *cur;

    pred = (standbyinfotype *)&idb->standbyidb->chain;
    for (cur = pred->next; cur; pred = cur, cur = cur->next) {
	if (cur->group > li->group) {
	    li->next = cur;
	    pred->next = li;
	    return;
	}
    }
    li->next = NULL;
    pred->next = li;
}

/*
 * standby_delete_address
 * Delete a secondary address from the queue of virtual IP addresses
 * associated with this standby group.
 */

static void standby_delete_address (standbyinfotype *si, ipaddrtype addr)
{
    standbyaddrtype *siaddr, *prev;
    
    prev = &si->addrs;
    /*
     * Start looking after first (primary) entry.
     */
    for (siaddr = prev->next_in_group; siaddr;
	 prev = siaddr, siaddr = siaddr->next_in_group) {
	if (siaddr->addr == addr)
	    break;
    }
    /*
     * If not found
     */
    if (siaddr == NULL)
	return;
    /*
     * Delete element from linked list
     */
    prev->next_in_group = siaddr->next_in_group;
    siaddr->next_in_group = NULL;
}

/*
 * standby_add_address
 * Add an address to the queue of virtual IP addresses associated with
 * this standby group.  Try to avoid duplicates.
 */

static standbyaddrtype*
standby_add_address (standbyinfotype *si, ipaddrtype addr)
{
    standbyaddrtype *siaddr, *prev;
    
    prev = NULL;
    for (siaddr = &si->addrs; siaddr;
 	 prev = siaddr, siaddr = siaddr->next_in_group) {
	if (siaddr->addr == addr)
 	    return(NULL);
    }
    /*
     * For sanity sake...
     */
    if (prev == NULL)
	return(NULL);
    /*
     * Add element to end of linked list
     */
    siaddr = malloc(sizeof(standbyaddrtype));
    if (siaddr == NULL) {
	printf(nomemory);
	return(NULL);
    }
    prev->next_in_group = siaddr;
    siaddr->next_in_group = NULL;
    siaddr->addr = addr;
    return(siaddr);
}

/*
 * standby_claim_addr
 * Assume control of a new virtual IP address.
 */

static void 
standby_claim_addr (idbtype *idb, standbyinfotype *si, standbyaddrtype *sa)
{
    ipaddrtype addr;

    addr = sa->addr;
    sa->sbgroup = si;
    standby_hash_add(sa);

    if (si->state != SSTATE_ACTIVE)
	return;
    ipaddr_table_insert(addr, idb, IPADDR_DYN_ALIAS, IPADDRFLAG_NO_ARP_RELOAD,
			0, NULL);
    standby_update_arp(idb, si, TRUE);
}

/*
 * standby_relinquish_addr
 * Relinquish control of a virtual IP address.
 */

static void 
standby_relinquish_addr (idbtype *idb, standbyinfotype *si, ipaddrtype addr)
{
     standbyaddrtype *sa;

     sa = standby_hash_lookup(addr);
     if (sa != NULL) {
	 standby_hash_remove(sa);
	 if (sa != &si->addrs) {
	     free(sa);
	 }
     }
     if (si->state != SSTATE_ACTIVE)
	 return;
     ipaddr_table_delete(addr, idb);
     standby_update_arp(idb, si, FALSE);
}

/*
 * standby_on_any
 * Determine if HSRP is configured on any swidbs associated with this
 * physical interface.
 */

static boolean standby_on_any (hwidbtype *hwidb)
{
    idbtype *swidb;

    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	if (swidb->standbyidb && swidb->standbyidb->chain)
	    return(TRUE);
    }
    return(FALSE);
}

/*
 * standby_group_unique
 * Insure uniqueness of hot standby group numbers, when multiple swidbs
 * exist under a single hot standby capable interface.  (Uniqueness required
 * as there is only a single address filter available for the physical
 * interface).
 */

static boolean standby_group_unique (idbtype *swidb, uchar newgroup)
{
    idbtype *subif;
    standbyinfotype *si;

    FOR_ALL_SWIDBS_ON_HW(swidb->hwptr, subif) {
	/*
	 * Only check other swidbs
	 */
	if (subif == swidb)
	    continue;
	FOR_ALL_SBGROUPS(subif, si)
	    if (si->group == newgroup)
		return(FALSE);
    }
    return(TRUE);
}

/*
 * standby_command
 * Parse hot standby protocol commands
 */

void standby_command (parseinfo *csb)
{
    idbtype *idb;
    standbyinfotype *li, *cur, *pred;
    standbyaddrtype *sa;
    ipaddrtype address;
    uchar group;
    boolean new_group;
    hwidbtype *hwidb;
    standbyidbtype *sbidb;

    idb = csb->interface;
    if (csb->nvgen) {
        if (!idb->standbyidb)
            return;
        if (standby_usebia(idb))
            nv_write(TRUE, "standby use-bia");
	FOR_ALL_SBGROUPS(idb, li)
	    standby_nvgen(idb, li);
	return;
    }

    /*
     * an ATM interface can run HSRP if and only if
     * a LANE client is configured on the subinterface
     */
    if (is_atm(idb->hwptr)) {
	if (csb->sense && !lec_enabled(idb)) {
	    printf("\nStandby: a LANE client must be enabled here for HSRP to run");
	    return;
	}
    }
    else if ((idb->hwptr->status & IDB_IEEEMASK) == 0) {
	printf("\nStandby: Interface %s cannot support hot standby protocol",
	       idb->namestring);
	return;
    }

    /* 
     * Due to the CAM on lowend FDDI (bsi chipset), it is not possible
     * to support both HSRP and PIM at the same time.
     */
    if ((idb->hwptr->status & IDB_FDDI) && 
	(idb->hwptr->type == IDBTYPE_BSI) && 
	(idb->ip_mrouting)) {
	printf("\nStandby: Interface %s cannot simultaneously support both\n"
	       "Hot Standby and IP Multicast routing.", idb->namestring);
	return;
    }

    /*
     * Parsing of configuration commands.
     */

    if (!idb->standbyidb) {
	/*
	 * For media whose address filter cannot support multiple unicast
	 * MAC addresses, HSRP can only be configured on 1 swidb per hwidb.
	 */
	if (!is_only_swidb(idb) && !standby_can_add_hwaddr(idb)) {
	    if (standby_on_any(idb->hwptr)) {
		printf("\nStandby: Interface hardware cannot support multiple"
		       " HSRP groups per interface.");
		return;
	    }
	}
	idb->standbyidb = malloc(sizeof(standbyidbtype));
	if (!idb->standbyidb) {
            printf(nomemory);
	    return;
	}
	hwidb = idb->hwptr;
	if (hwidb->type == IDBTYPE_FDDIT) {
	    /*
	     * Sigh, we have to worry about enabling FDDI stripping,
	     * which needs to be enabled before we possibly
	     * send packets whose source MAC address is not our own.
	     */
	    hwidb->reset_request = TRUE;
	}
    }
    sbidb = idb->standbyidb;
    /*
     * Process global interface commands now
     */
    switch (csb->which) {
    case STANDBY_CMD_USEBIA:
        if (csb->sense) {
            if (sbidb->chain && sbidb->chain->next) {
                printf("\nStandby: Must deconfigure multiple groups first");
                return;
            }
            if ((sbidb->flags & SFLAG_CFG_USEBIA) == 0) {
                li = sbidb->chain;
                if (li && li->state == SSTATE_ACTIVE) {
                    RESTART_ACTIVETIMER;
                    RESTART_STANDBYTIMER;
                    standby_state(SSTATE_LEARN, li, idb);
                }
                sbidb->flags |= SFLAG_CFG_USEBIA;
            }
        } else {
            if (sbidb->flags & SFLAG_CFG_USEBIA) {
                li = sbidb->chain;
                if (li && li->state == SSTATE_ACTIVE) {
                    RESTART_ACTIVETIMER;
                    RESTART_STANDBYTIMER;
                    standby_state(SSTATE_LEARN, li, idb);
                }
                sbidb->flags &= ~SFLAG_CFG_USEBIA;
            }
        }
        /*
         * Global interface commands don't require any per group
         * processing, so finish now.
         */
        return;

    default:
        break;
    }

    if (GETOBJ(int,4)) {
	group = GETOBJ(int,3);
    } else {
	/*
	 * Group was not specified.  Try defaulting it.
         * Commands which do not take a group # argument always default
         * to whatever group has been configured.
	 */
	if (sbidb->chain && (sbidb->chain->flags & SFLAG_CFG_GROUP))
	    group = sbidb->chain->group;
	else
	    group = SDEFAULT_GROUP;
    }
    /*
     * If interface is subinterfaced, make sure that each swidb has a
     * unique group number.
     */
    if (!is_only_swidb(idb)) {
	if (!standby_group_unique(idb, group)) {
	    printf("\nStandby: %s: Must use unique standby group number for"
		   "\n  each logical interface that is a member of the same"
		   " physical interface.", idb->namestring);
	    return;
	}
    }
    li = standby_get_group(idb, group);
    if (li == NULL) {
	/*
	 * Group isn't configured, so no command is a noop.
	 */
	if (csb->sense == FALSE)
	    return;
	if (sbidb->chain && !standby_can_add_hwaddr(idb)) {
	    printf("\nStandby: Interface hardware cannot support multiple"
		   " groups.");
	    return;
	}
	new_group = TRUE;
	li = standby_new_group(group, idb);
	if (li == NULL) {
	    printf(nomemory);
	    return;
	}
    } else {
	new_group = FALSE;
    }
    switch (csb->which) {
    case STANDBY_CMD_IP:
	address = GETOBJ(paddr,1)->ip_addr;
	if (address) {
	    /*
	     * If the address is in a standby group
	     * make sure it's the right one
	     */
	    cur = standby_find_group(address);
	    
	    if (cur != NULL) {
		if (cur != li) {
		    printf("\nStandby: Address %i in group %d", 
			   address, cur->group);
		    if (idb != cur->idb) {
			printf(" - interface %s", cur->idb->namestring);
		    }
		    return;
		} else {
		    /* 	
		     * can't config primary as secondary or vice versa
		     */
		    if (GETOBJ(int,1)) {
			if (address == standby_primary_addr(li)) {
			    printf(alreadyconfigured,  address, "primary");
			    return;
			}
		    } else {		/* primary */
			if ((address != SDEFAULT_IP_ADDR) &&
			    (address != standby_primary_addr(li)) && 
			    (standby_primary_addr(li) != SDEFAULT_IP_ADDR)) {
			    printf(alreadyconfigured, address, "secondary");
			    return;
			}
		    }
		}
	    }
	} else {
	    address = SDEFAULT_IP_ADDR;
	    cur = NULL;
	}

	if (csb->sense) {

	    if (address) {
		/*
		 * If the address is already in the standby group
		 * there's nothing to do.
		 */
		if (cur != NULL) {
		    return;
		}
		if (martian(address,TRUE)) {
		    printf("\nStandby: Not a valid host address - %i", address);
			return;
		    }
		if (ip_ouraddress(address)) {
		    printf(notsameaddress);
		    return;
		}

		if (GETOBJ(int,1)) {
		    /*
		     * Configuring a secondary
		     */
		    sa = standby_add_address(li, address);
		    if (sa == NULL) {
			return;
		    }
		} else {
		    /*
		     * if (standby_primary_addr(li) != address)
		     *
		     * This is always true.  If it was the same
		     * address, then it would have to be the same
		     * group and we never get to this point
		     */
		    standby_relinquish_addr(idb, li, standby_primary_addr(li));
		    standby_primary_addr(li) = address;
		    sa = &li->addrs;

		    li->flags |= SFLAG_CFG_IP_ADDR;
		}
		standby_claim_addr(idb, li, sa);
	    }

	    if ((li->flags & SFLAG_CFG_IP) == 0) {
		/*
		 * Enable IP support
		 */
		li->flags |= SFLAG_CFG_IP;
		ip_multicast_ctl(idb, ALLROUTERS, TRUE);
	    }
	} else {
	    if (GETOBJ(int,1)) {
		/*
		 * Deconfiguring a secondary
		 */
		address = GETOBJ(paddr,1)->ip_addr;
		standby_delete_address(li, address);
		standby_relinquish_addr(idb, li, address);
	    } else {
		if (li->flags & SFLAG_CFG_IP_ADDR) {
		    li->flags &= ~SFLAG_CFG_IP_ADDR;
		    standby_relinquish_addr(idb, li, standby_primary_addr(li));
		    standby_primary_addr(li) = SDEFAULT_IP_ADDR;
		}
	    }
	    if (li->flags & SFLAG_CFG_IP) {
		/*
		 * Make sure that no IP addresses remain configured
		 */
		if (!(li->flags & SFLAG_CFG_IP_ADDR) &&
		    (li->addrs.next_in_group == NULL)) {
		    /*
		     * Disable IP support
		     */
		    li->flags &= ~SFLAG_CFG_IP;
		    ip_multicast_ctl(idb, ALLROUTERS, FALSE);
		}
	    }
	}
	/*
	 * Don't want ICMP redirects if hot standby protocol is running.
	 */
	idb->ip_redirect = !csb->sense;
	break;
    case STANDBY_CMD_AUTH:
	memset(li->auth, 0, STANDBY_AUTH_LEN);
	if (csb->sense) {
	    li->flags |= SFLAG_CFG_AUTH;
	    sstrncpy(li->auth, GETOBJ(string,1), STANDBY_AUTH_LEN + 1);
	} else {
	    li->flags &= ~SFLAG_CFG_AUTH;
	    sstrncpy(li->auth, sdefault_auth, STANDBY_AUTH_LEN);
	}
	break;
    case STANDBY_CMD_PREEMPT:
	if (csb->sense)
	    li->flags |= SFLAG_CFG_PREEMPT;
	else
	    li->flags &= ~SFLAG_CFG_PREEMPT;
	break;
    case STANDBY_CMD_PRIORITY:
	if (csb->sense) {
	    li->flags |= SFLAG_CFG_PRIORITY;
	    li->cfg_priority = GETOBJ(int,1);
	} else {
	    li->flags &= ~SFLAG_CFG_PRIORITY;
	    li->cfg_priority = SDEFAULT_PRIORITY;
	}
	li->priority = standby_pick_priority(li);
	break;
    case STANDBY_CMD_TIMERS:
	if (csb->sense) {
	    li->flags |= SFLAG_CFG_TIMERS;
            li->hellotime = GETOBJ(int,1);
            li->holdtime = GETOBJ(int,2);
            if (!GETOBJ(int,5)) {
                li->hellotime *= ONESEC;
            }
            if (!GETOBJ(int,6)) {
                li->holdtime *= ONESEC;
            }
	} else {
	    li->flags &= ~SFLAG_CFG_TIMERS;
	    li->hellotime = standby_sec2ms(SDEFAULT_HELLOTIME);
	    li->holdtime = standby_sec2ms(SDEFAULT_HOLDTIME);
	}
	break;
    case STANDBY_CMD_TRACK:
        /*
         * Standby track interface command must be configured after
         * host standby registrations have been added.
         * This ensures that standby process will be notified about all
         * interface state change, and not leave the tracked interface
         * in a wrong state.
         */
        if (!standby_registered) {
            standby_register();
        }
	standby_track_command(li, GETOBJ(idb,1), GETOBJ(int,1), csb->sense);
	if (csb->sense)
	    li->flags |= SFLAG_CFG_TRACK;
	else if (li->trackifs == NULL)
	    li->flags &= ~SFLAG_CFG_TRACK;
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	return;
    }
    if (new_group)
	standby_revise_chain(idb, li);
    standby_state_change(idb, li);
    standby_process_state_change(li);
    if ((new_group == FALSE) && (li->flags == 0)) {
	/*
	 * Unlink deconfigured group from chain
	 */
	pred = (standbyinfotype *)&sbidb->chain;
	for (cur = pred->next; cur; pred = cur, cur = cur->next) {
	    if (cur == li) {
		pred->next = li->next;
		break;
	    }
	}
	memset(li, 0, sizeof(standbyinfotype));
    }
}

/*
 * show_standby_idb
 * Show a particular interface stats.
 */

static void show_standby_idb (idbtype *idb, standbyinfotype *li)
{
    idbtype *trackidb;

    if (!standby_mincfg(li))
	return;
    printf("\n%s - Group %d", idb->namestring, li->group);
    printf("\n  Local state is %s, priority %d",
	   standby_statestring(li->state), li->priority); 
    if (li->flags & SFLAG_CFG_PREEMPT)
	printf(", may preempt");
    if (standby_usebia(idb))
        printf(", use bia");
    printf("\n  Hellotime %d holdtime %d", li->learned_hellotime,
	   li->learned_holdtime);
    if (li->flags & SFLAG_CFG_TIMERS) {
        printf(" configured hellotime ");
        if (li->hellotime < ONESEC) {
            printf("%d msec holdtime ", li->hellotime);
        } else {
            printf("%d sec holdtime ", standby_ms2sec(li->hellotime));
        }
        if (li->holdtime < ONESEC) {
            printf("%d msec", li->holdtime);
        } else {
            printf("%d sec", standby_ms2sec(li->holdtime));
        }
    }
    if (li->state & SSTATE_SPEAKER) {
	printf("\n  Next hello sent in %lTF", li->nexthello);
    }
    printf("\n  Hot standby IP address is ");
    if (standby_have_addr(li))
	printf("%i%s", standby_primary_addr(li),
	       (li->flags & SFLAG_CFG_IP_ADDR) ? " configured" : "");
    else
	printf("unknown");
    
    printf("\n  Active router is ");
    if (li->state == SSTATE_ACTIVE)
	printf("local");
    else if (li->active == 0)
	printf("unknown");
    else {
	printf("%i expire", li->active);
	if (SLEEPING(li->activetimer)) {
	    printf("s in %TF", li->activetimer);
	} else
	    printf("d");
    }
    printf("\n  Standby router is ");
    if (li->state == SSTATE_STANDBY)
	printf("local");
    else if (li->standby == 0)
	printf("unknown");
    else {
	printf("%i expire", li->standby);
	if (SLEEPING(li->standbytimer)) {
	    printf("s in %TF", li->standbytimer);
	} else
	    printf("d");
    }
    if (li->trackifs) {
	printf("\n  Tracking interface states for %d interface%s, %d up:",
	       li->trackif_cnt, (li->trackif_cnt == 1) ? "" : "s",
	       li->upif_cnt);
	FOR_ALL_SWIDBS(trackidb) {
	    if (!CHECKBIT(trackidb->if_number, li->trackifs))
		continue;
	    printf("\n    %s %s",
		   CHECKBIT(trackidb->if_number, li->upifs) ? "Up  " : "Down",
		   trackidb->namestring);
	    if (li->if_pri[trackidb->if_number])
		printf(" Priority decrement: %d",
		       li->if_pri[trackidb->if_number]);
	}
    }
}

/*
 * show_standby
 * Show hot standby information.
 */

void show_standby (parseinfo *csb)
{
    idbtype *target, *idb;
    standbyinfotype *si;
    
    target = GETOBJ(idb, 1);
    automore_enable(NULL);
    /*
     * Should be able to use show_swidb_chain, but that routine's more
     * processing assumes that we generate output for each interface which
     * is not true.
     */
    FOR_ALL_SWIDBS(idb) {
	if (idb->hwptr->status & IDB_DELETED)
	    continue;
	if (target && (target != idb))
	    continue;
	FOR_ALL_SBGROUPS(idb, si)
	    show_standby_idb(idb, si);
	if (automore_quit())
	    break;
    }
    automore_disable();
}
