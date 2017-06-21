/* $Id: loopback.c,v 3.3.60.4 1996/07/23 13:23:50 thille Exp $
 * $Source: /release/112/cvs/Xsys/if/loopback.c,v $
 *------------------------------------------------------------------
 * Loopback Interface
 *
 * October, 1992 - Paul Traina
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Provide a virtual interface that acts like a an always up
 * loopback interface.  We can use this as the 'box' IP address.
 *------------------------------------------------------------------
 * $Log: loopback.c,v $
 * Revision 3.3.60.4  1996/07/23  13:23:50  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.3.60.3  1996/06/27  09:33:49  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.3.60.2  1996/05/21  09:49:58  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3.60.1  1996/05/21  06:31:19  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.3  1995/11/17  09:30:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:38:17  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:50:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/10/18  17:18:48  wmay
 * CSCdi39845:  show interface displays serials in a non-intuitive manner
 * Sort high end idb's by slot/slotunit/vc, low end by unit/vc.
 *
 * Default slot and slotunit are changed to MAXUINT.  Virtual interfaces
 * (tunnel, loop, async) shouldn't change them, as parser is now smart
 * enough to use unit on high end.
 *
 * Sort swidbs by subinterface number, rather than alphabetically
 * (CSCdi35897).
 *
 * Revision 2.4  1995/10/13  20:26:51  shj
 * CSCdi41281:  memory leak associated with loopback interface crashes
 * router
 * Define ET_LOOP to be used in hwidb->enctype on loopback interfaces.
 * Prevents set_default_queueing from changing hwidb->oqueue on loopback
 * interfaces, which led to packets being queued forever.
 *
 * Revision 2.3  1995/09/09  00:52:36  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.2  1995/06/21  03:28:09  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.1  1995/06/07  20:53:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "snmp_interface.h"
#include "../snmp/ifmib_registry.h"
#include "packet.h"
#include "config.h"
#include "../if/if_vidb.h"
#include "../if/network.h"
#include "../parser/parser_defs_parser.h"
#include "loopback.h"

/*
 * Definine an insanely fast bandwidth for the loopback device.
 */
#define	LOOP_VIS_BW	8000000		/* 8Gbps */

/*
 * loopback_vencap
 *
 * Called by the IDB dispatch routines
 */

static boolean loopback_vencap (paktype *pak, long address)
{
    pak->enctype = ET_NULL;
    pak->encsize = 0;
    pak->mac_start  = NULL;
    pak->addr_start = NULL;
    pak->rif_start  = NULL;
    pak->riflen     = 0;
    pak->info_start = NULL;
    return(TRUE);
}

/*
 * loopback_getlink
 *
 * Called by raw_enqueue via the IDB dispatch routines.
 */

static long loopback_getlink (paktype *pak)
{
    return(pak->linktype);	/* it was already set on the way out... */
}

/*
 * loopback_oqueue
 *
 * Called by the IDB dispatch routines -- if this were a normal interface
 * we would put the packet on the appropriate output hold queue.  However,
 * since we don't need an output hold queue, this is the most convinient
 * place to steal the packet away from the normal code path.
 *
 */

static boolean loopback_oqueue (hwidbtype *hwidb, paktype *pak,
				enum HEADTAIL which)
{
    paktype *copy;
    leveltype status;
    idbtype *idb = hwidb->firstsw;

    /*
     * If the packet we want to output originally came in this same loopback
     * interface, do not input it again, otherwise we will lock up the system
     * with an infinite loop.
     */
    if (pak->if_input != idb) {
	copy = pak_duplicate(pak);
	if (copy) {
	    copy->if_input = idb;
	    copy->if_output = NULL;
	    status = raise_interrupt_level(NETS_DISABLE);
	    raw_enqueue(hwidb, copy);
	    reset_interrupt_level(status);
	}
    }
    datagram_done(pak);
    return(TRUE);
}

/*
 * loopback_createidb
 * Create an IDB on the fly when it is first referenced by an
 * "interface loopback n" command.
 *
 */

static idbtype * loopback_createidb (uint unit)
{
    idbtype *idb;
    hwidbtype *hwidb;

    /*
     * don't put tunnel on normal in-use queue, but rater use special
     * vidb_inuseQ so we can clean up later.
     */
    idb = vidb_malloc();
    if (!idb) {
	printf(nomemory);
	return(NULL);
    }
    hwidb = idb->hwptr;

    hwidb->type     = IDBTYPE_LB;	/* fully defined by status bits */
    hwidb->state    = IDBS_UP;
    hwidb->name     = IFNAME_LOOPBACK;
    hwidb->typestring = "Loopback";
    hwidb->unit     = unit;
    set_default_interface_bandwidth(idb, LOOP_VIS_BW);
    idb_init_names(idb, TRUE);

    hwidb->iqueue   = raw_enqueue;
    hwidb->getlink  = loopback_getlink;
    hwidb->vencap   = loopback_vencap;
    hwidb->deencap  = NULL;
    hwidb->oqueue   = loopback_oqueue;
    hwidb->oqueue_dequeue = (oqueue_dequeue_t)return_null;
    hwidb->soutput  = (soutput_t) return_nothing;
    hwidb->extract_hwaddr = (extract_hwaddr_t)return_false;
    hwidb->extract_snpa = (extract_snpa_t)return_false;

    /*
     * No link level broadcasts
     */
    hwidb->broadcast= (broadcast_t) return_fuz_false;
    hwidb->reset    = (reset_t) return_nothing;
    hwidb->periodic = (periodic_t) return_nothing;

    /*
     * Number it for SNMP 
     */

    hwidb->hw_if_index = idb_get_next_hw_index();
    hwidb->firstsw->snmp_if_index =
    hwidb->snmp_if_index = reg_invoke_ifmib_register_hwidb(hwidb);
    hwidb->enctype = ET_LOOP;

    /*
     * Interface now exists, put it on the idb queue.
     * Do this last so idb->tunnel is always valid.
     */

    idb_enqueue(hwidb);
    nnets++;			/* increment total number of interfaces */

    return(idb);
}


boolean loopback_parser_createidb (uint64 *ifcode, parseinfo *csb)
{
    if (*ifcode == IFTYPE_LOOPBACK) {
	csb->createidb = loopback_createidb;
	return(TRUE);
    }
    return(FALSE);
}
