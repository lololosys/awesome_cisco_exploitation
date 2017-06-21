/* $Id: if_les_tms380.c,v 3.11.6.19 1996/08/28 12:59:00 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les_tms380.c,v $
 *------------------------------------------------------------------
 * if_les_tms380.c -- Pancake Network Tokenring interface driver
 * 
 * June 1991, Chris Shaker
 *
 * Copyright (c) 1991-1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Extensive use made of code from David Hampton's SBE16 card monitor
 * code in /csc/sbe16/madge, and of his system code in
 * /csc/sys/if/if_sbe16.[ch]
 *
 * August 5, 1991 - Updated to agree with /csc/sbe16/madge version 1008,
 * as of July 24, 1991.
 *------------------------------------------------------------------
 * $Log: if_les_tms380.c,v $
 * Revision 3.11.6.19  1996/08/28  12:59:00  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.11.6.18  1996/08/26  15:10:26  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.11.6.17  1996/08/22  05:15:41  echeng
 * CSCdi62388:  Many bugs in the code handling TokenRing error counters
 * Branch: California_branch
 *
 * Revision 3.11.6.16  1996/08/13  02:20:41  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.11.6.15  1996/08/11  23:34:06  ppearce
 * CSCdi61195:  RSRB performance degraded 40% from 11.1(1)
 * Branch: California_branch
 *   Erroneous RSRB debug processing -- missing check for debug enabled
 *
 * Revision 3.11.6.14  1996/07/23  18:47:38  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.11.6.13  1996/07/22  01:33:10  psandova
 * CSCdi59090:  Non-explorer packets are being flushed with explorers
 * Branch: California_branch
 * Valid multicast packets that should be handed to the protocol stack
 * are instead being diverted to the srb module and are flushed by the
 * explorer control mechanism.
 *
 * Revision 3.11.6.12  1996/07/20  03:20:03  fred
 * CSCdi63607:  add access list to traffic shaping per customer request
 *              requires change of parameters on function called by
 *              fastsend routines in drivers
 * Branch: California_branch
 *
 * Revision 3.11.6.11  1996/07/18  20:28:59  ppearce
 * CSCdi63366:  Move SRB counters from hwidb into SRB subblock
 * Branch: California_branch
 *   - Decreases size of hwidb for non-SRB'd interfaces
 *   - Provides additional IOS-independence for SRB
 *   - Moves SRB a little closer to being completely swidb-based
 *
 * Revision 3.11.6.10  1996/07/16  09:36:34  hampton
 * Centralize knowledge of Decnet/Token Ring interaction.  [CSCdi63066]
 * Branch: California_branch
 *
 * Revision 3.11.6.9  1996/07/11  14:20:32  fbordona
 * CSCdi62521:  DLSw circuit does not connect using FST over ATM peer
 *              Remove DLSw FST hooks from platform/media specific drivers.
 * Branch: California_branch
 *
 * Revision 3.11.6.8  1996/07/09  06:00:39  ppearce
 * CSCdi59527:  inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.11.6.7  1996/06/01  06:15:48  hampton
 * Remove obsolete media_tring_supports_etr registry call.  [CSCdi59254]
 * Branch: California_branch
 *
 * Revision 3.11.6.6  1996/06/01  01:32:10  hampton
 * Remove unnecessary cross module references.  [CSCdi59221]
 * Branch: California_branch
 *
 * Revision 3.11.6.5  1996/05/17  11:26:38  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.10.2.6  1996/05/10  15:14:50  fbordona
 * CSCdi57207:  DLSw FST broken over WAN token ring and FDDI
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.10.2.5  1996/05/05  23:32:13  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.10.2.4  1996/04/26  15:22:11  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.10.2.3  1996/04/07  18:40:12  rhubbard
 * CSCdi50344:  invalid rif entries if multiring configured or possible
 * dlsw
 * loops .
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.10.2.2  1996/04/03  19:59:45  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.10.2.1  1996/03/17  18:06:44  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.11.6.4  1996/05/09  14:34:08  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.11.6.3  1996/05/04  01:24:48  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Restore input to the BVI if necessary when copying packet for
 *   process-level.
 * - Supress reflection and keep physical broadcast in bridging fast-path
 *   for tms380.
 *
 * Revision 3.11.6.2.4.1  1996/04/27  07:15:27  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.11.6.2  1996/04/17  13:44:07  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.11.6.1.8.1  1996/04/08  01:58:57  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.11.6.1  1996/03/18  20:42:42  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.11.10.1  1996/03/28  17:35:13  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.11  1996/03/06  12:47:35  fetemadi
 * CSCdi47379:  Mallocfail - Memory Allocation problems on cisco4700
 *
 * Revision 3.10  1996/02/14  23:22:48  jcchan
 * CSCdi48756:  Race condition for TMS380 input driver
 *
 * Revision 3.9  1996/02/13  05:50:58  ogrady
 * CSCdi45887:  Router crash in rsp_fastsend
 *              Use common routine for token ring output debug.
 *
 * Revision 3.8.2.1  1996/01/24  22:24:34  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.8  1996/01/22  06:37:48  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.7  1996/01/18  20:22:54  jacobt
 * CSCdi47137:  printing of rif info is not filtered out when debug
 * filtering is ON
 *
 * Revision 3.6  1995/12/19  00:40:46  hampton
 * Obsolete the GET_NONZERO_TIMESTAMP routine.  It is no longer necessary
 * with 64bit timestamps.  [CSCdi45208]
 *
 * Revision 3.5  1995/12/07  22:30:37  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.4  1995/12/06  18:31:13  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.3  1995/11/17  17:41:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:00:07  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:33:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.14  1995/11/08  21:14:58  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.13  1995/10/12  21:17:29  jcchan
 * CSCdi41499:  Router fails to reinsert into token hub
 *
 * Revision 2.12  1995/09/28  20:24:24  rbatz
 * CSCdi38486:  CSCdi38486:  Fast-switched SRB can pad 4 bytes of data on
 *              frames.  The extra data is the Token Ring FCS.
 *              We pass it while Madge needs to recalculate it.
 *
 * Revision 2.11  1995/09/13  01:04:16  hcwoo
 * CSCdi40240:  Process switching from TR nosedives at high i/p rates on
 * 25xx, 4x00
 *
 * Revision 2.10  1995/08/25  03:31:36  speakman
 * CSCdi38884:  IP ARP over token/serial broken
 * SNAP over HDLC, who woulda thunk it.  Do the GETSHORT thing.
 *
 * Revision 2.9  1995/08/07  05:38:55  richl
 * CSCdi38315:  makefile rework
 *              move inlines to tring_inline and utilities to rif_util.c
 *
 * Revision 2.8  1995/07/29  01:10:34  mordock
 * CSCdi37882:  currekt missed-spelled wurds
 * Corrected spelling mistakes in various messages.
 *
 * Revision 2.7  1995/07/05  20:49:39  hampton
 * Consolidate common token ring variables into the idb, instead of having
 * them in the private data structure for each type of token ring
 * interface.  Remove overloading of the delay variable by token rings.
 * Eliminate direct references to the system clock.  [CSCdi36721]
 *
 * Revision 2.6  1995/06/21 08:55:40  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.5  1995/06/21  03:01:21  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.4  1995/06/21  02:16:08  hcwoo
 * CSCdi35529:  source-bridge x x x, crashes the router while there is
 * traffic.
 *
 * Revision 2.3  1995/06/19  06:47:56  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:11:40  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:37:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * ------------------------------------------------------------------------
 *
 * This module contains the device driver for TMS380 style token ring
 * interfaces in the IGS family of cisco routers/bridges.
 * The generic module, "tring.c", contains low level_init code used to
 * interface to the token ring 802.5/IBM media.
 *
 * This module contains all of the idb vector routines implemented,
 * initialization code, interrupt routines, and support routines.
 *
 * These routines are organized in the following order:
 *
 * 	idb vector functions
 * 	interrupt handler(s)
 * 	support routines for vector functions and interrupts
 * 	low level interface functions.
 * 	initialization routines
 *
 * idb vectors supported
 *
 *     encap		tr_encapsulate		(tring.c)
 *     deencap					not supported
 *     soutput		tms380_madge_output
 *     oqueue		holdq_enqueue		(network.c)
 *     oqueue_dequeue	holdq_dequeue		(network.c)
 *     iqueue		raw_enqueue		(network.c)
 *     reset		tms380_restart
 *     shutdown		tms380_shutdown
 *     linktype		get10mblink		(ether.c)
 *     broadcast	tr_bcast		(tring.c)
 *     setencap		tr_setencaps		(tring.c)
 *     periodic		tr_periodic		(tring.c)
 *     device_periodic	tms380_periodic
 *
 * tms380_madge_output, tms380_restart, tms380_shutdown, and tms380_periodic
 * are the only vector functions that are interface specific.
 * tr_encapsulate, tr_bcase, tr_setencaps, and tr_periodic are media
 * dependent.  Others are network level dependencies.
 *
 * Token Ring interface states:
 *
 * IDBS_DOWN:	something failed
 * IDBS_INIT:	coming up.  In initialization sequence.  Waiting for
 * 		completion of the start command. 
 * IDBS_UP:	fully functional
 * IDBS_RESET:	something failed and the board was reset.  In this
 * 		state the board should not be touched, since another
 *		context is accessing the board.
 * IDBS_ADMINDOWN: The board has been shutdown on purpose.
 *
 * Flapping and Queue effects: The Token Ring interface can flap
 * depending on what is happening on the ring.  State changes are delayed
 * using the net_cstate/cstate_check combination.  When the interface
 * goes down we need to prevent additional packets from being started up.
 * We want the interface to become quiesient.  We can NOT flush the
 * output queue because this has nasty effects on overall reliablity of
 * packet arrival if flapping occurs.
 *
 * The holdq is flushed anytime we go into the INIT state.  Anytime we
 * are in INIT the queue should be empty.  We also flush the queue when the
 * interface is detected to be hard down.  This occurs after a suitable
 * length of time (cstate_check).  This is done to avoid the ring from
 * holding packet buffers that could be better used by another part of the
 * system.
 *
 * INTRS:
 * Upon a token ring interrupt, we will check all interrupt sources and
 * process everything that can be processed at that time.
 * ------------------------------------------------------------------
 * $EndStuff$
 */

#include "master.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "interface_private.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "../srt/srb_core.h"
#include "../srt/srb_core_registry.h"
#include "../srt/srb_multiring.h"
#include "../srt/srt_registry.h"
#include "../partner/partner_registry.h"
#include "subsys.h"
#include "logger.h"
#include "../if/msg_tr.c"		/* Not a typo, see logger.h */
#include "packet.h"
#include "../if/tring.h"
#include "../if/tring_private.h"
#include "../if/tring_common.h"
#include "../os/pool.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"
#include "../if/network.h"
#include "../if/rif.h"
#include "../ip/ip.h"
#include "if_les.h"
#include "if_les_tms380.h"

#include "../srt/trsrb.h"
#include "../tbridge/tbridge_sde.h"
#include "../if/rif_inline.h"
#include "../if/tring_inline.h"

#include "../ip/remif_proto.h"  
#include "../smf/smf_public.h"
#include "../ui/debug.h"
#include "../tbridge/tbridge_debug.h"
#include "../if/rif_util.h"
#include "../if/traffic_shape.h"

/*
 * Local variables:
 */
char tms380_rif_string[(SRB_MAXRIF * 2) + (SRB_MAXRIF / 2)];

ulong dbgflags = 0;
TIME_BASE;
DBG_BASE(DBG_ARB);

/*
 * TMS380 subsystem header
 */

#define TMS380_MAJVERSION   1
#define TMS380_MINVERSION   0
#define TMS380_EDITVERSION  1

SUBSYS_HEADER(tms380, TMS380_MAJVERSION, TMS380_MINVERSION, TMS380_EDITVERSION,
	      tms380_subsys_init, SUBSYS_CLASS_DRIVER,
	      NULL,
	      NULL);

/* command interface */

/*
 * tms380_get_smt_stats
 * Get internal token ring station management statistics.
 *
 * This routine checks to see if the interface is alive and if so
 * gets an update of its dynamic cells.
 */
static void tms380_get_smt_stats (hwidbtype *idb)
{
    tms380_instance_t *ds;
    sbe_smt_stats_t *stats;
    idbtype *swidb = firstsw_or_null(idb);
    ushort srb_actualring, srb_thisring;
    srb_vector_table_t *srbV_p;
    
    ds = INSTANCE;
    
    if (ds->if_state != IF_UP) {
	return;
    }
    
    /*
     * Call the low level routine that does the work:
     */
    grab_380_smt_stats(idb);
 
    srbV_p = reg_invoke_srb_get_vector_table(); 
    if (!srbV_p) {
        return;
    }
   
    if (srbV_p->srb_sb_get_actualring(swidb) == 0) {
	stats = &ds->frozen_stats;
	ieee_copy(ds->bia, idb->bia);
	reg_invoke_media_bia_discovered(idb);
        srb_thisring = srbV_p->srb_sb_get_thisring(swidb);
        srb_actualring = stats->local_ring;
	srbV_p->srb_sb_set_actualring(swidb, srb_actualring);
	if (srb_thisring && srb_actualring &&
	       (srb_actualring != srb_thisring)) {
	    errmsg(&msgsym(BADRNGNUM, TR),
		   idb->unit, srb_thisring, srb_actualring);
	}
	reg_invoke_media_rps_startup(idb, srb_actualring);
    }
}


/*
 * tms380_get_static_stats: get status info that doesn't change.
 *
 * We assume that this is only called in initialization so it is
 * okay to muck with the idb->state cell.  This allows us to use
 * tms380_get_stats.
 */
static void tms380_get_static_stats (hwidbtype *idb)
{
    tms380_instance_t *ds;
    
    ds = INSTANCE;
    if (!ds)
	return;
    
    (void)tms380_get_stats(idb);
    ds->madge_kernel_version = ds->madge_kernel;
    ds->madge_fastmac_version = ds->madge_fastmac;

}

/*
 * tms380_update_prev_stats: update our accumulation of previous stats
 * 	from the card with their current values.
 *
 * This gives us running totals and is useful for debugging.  This also
 * nails the cells we use for keeping track of ring status messages.
 */
static void tms380_update_prev_stats (hwidbtype *idb)
{
    if (!tms380_get_stats(idb)) {
	return;
    }
    tms380_add_counters(idb);
}


void tms380_add_counters (hwidbtype *idb)
{
    tms380_instance_t *ds;
    madge_smt_errors_t *errors;
    
    sbe_smt_stats_t *stats;
    ulong *src, *dst;
    int i;
    tokensb *tsb;
    
    ds = INSTANCE;
    tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);
    
    stats = &ds->frozen_stats;
    
    tring_update_counters(tsb);

    /*
     * Add counters.  We use the exact same structure, so this can be a
     * simple loop instead of listing each cell.
     */
    src = (ulong *)&ds->frozen_counts;
    dst = (ulong *)&ds->prev_counts;
    for (i = 0; i < sizeof(sbe_counters_t)/4; i++)
	dst[i] += src[i];

    /* Needed to generalize TR mib code */
    tsb->prev_bad_acbits += ds->prev_counts.bad_acbits;
    ieee_copy(stats->una, tsb->naun);
    ieee_copy(stats->beacon_una, tsb->beacon_naun);
    tsb->last_beacon_type = stats->last_beacon_type;

    /*
     * Add errors
     * from ds->log380_errors instead of ds->active_errors
     */
    errors = &ds->log380_errors;
    tsb->prev_err_line += errors->err_line;
    tsb->prev_err_internal += errors->err_internal;
    tsb->prev_err_burst += errors->err_burst;
    tsb->prev_err_arifci += errors->err_arifci;
    tsb->prev_err_abort += errors->err_abort;
    
    tsb->prev_err_lostframe += errors->err_lostframe;
    tsb->prev_err_rcvcongestion += errors->err_rcvcongestion;
    tsb->prev_err_freq += errors->err_freq;
    tsb->prev_err_token += errors->err_token;

    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
}


/****** bridging/protocol support ******/

/*************************************************************
 *
 * Reset anything associated with bridging.
 *
 *************************************************************/

static void nail_bridge_stuff (hwidbtype *idb)
{
    tms380_instance_t *ds;
    
    ds = INSTANCE;
    
    ds->max_rd = 0;
    ds->bridge_enabled = FALSE;
    ds->tbridge_enabled = FALSE;
    ds->max_rif = 0;
    ds->rng_br_rng_f = 0L;
    ds->rng_br_rng_r = 0L;
}


/*
 * Filter this packet based upon protocol type.
 */

static boolean tms380_filter (tms380_instance_t *ds,
			      sap_hdr *llc1)
{
    ushort *ptr;
    
    if (llc1->dsap == SAP_SNAP) {
	snap_hdr *snap = (snap_hdr *)llc1;

	for (ptr = ds->snapcodes; *ptr; ptr++)
	    if (snap->type == *ptr)
		return(TRUE);
    } else {
	for (ptr = ds->llc1codes; *ptr; ptr++)
	    if (*(ushort *)llc1 == *ptr)
		return(TRUE);
    }
    return(FALSE);
}


/*************************************************************
 *
 * Classify this packet.
 *
 *************************************************************/
 
static void tms380_classify (sap_hdr *llc1,
			     ushort *pak_type,
			     ushort *buf_flags)
{
    /*
     * SNAP?
     */
    if (llc1->dsap == SAP_SNAP) {
        snap_hdr *snap = (snap_hdr *)llc1;
 
        switch (snap->type) {
          case TYPE_IP10MB:
	    *pak_type = RXTYPE_SNAP_DODIP;
            break;
          case TYPE_RFC826_ARP:
            *pak_type = RXTYPE_RFC826_ARP;
            break;
          case TYPE_DECNET:
            *pak_type = RXTYPE_SNAP_DECNET;
            break;
          case TYPE_XNS:
            *pak_type = RXTYPE_SNAP_XNS;
            break;
          case TYPE_ETHERTALK:
            *pak_type = RXTYPE_SNAP_APPLE;
            break;
          case TYPE_NOVELL1:
            *pak_type = RXTYPE_SNAP_NOV1;
            break;
          case TYPE_APOLLO:
            *pak_type = RXTYPE_SNAP_APOLLO;
            break;
          case TYPE_VINES:
          case TYPE_VINES2:
          case TYPE_VINES_LOOP:
          case TYPE_VINES_ECHO:
          case TYPE_VINES_ECHO2:
            *pak_type = RXTYPE_SNAP_VINES;
            break;
          default:
            *pak_type = RXTYPE_UNKNOWN;
            break;
        }
    } else {
        switch (llc1->dsap) {
          case SAP_IP:
	    *pak_type = RXTYPE_SAP_IP;
            break;
          case SAP_CLNS:
            *pak_type = RXTYPE_SAP_CLNS;
            break;
          case SAP_NOVELL:
            *pak_type = RXTYPE_SAP_NOVELL;
            break;
          case SAP_VINES:
            *pak_type = RXTYPE_SAP_VINES;
            break;
          default:
            *pak_type = RXTYPE_SAP_UNKN;
            break;
        }
    }
}


/*
 * tms380_lacceptor
 *
 * The Local Acceptor determines if a packet should be processed locally.
 * It looks for the following and sets the appropriate buffer flags:
 *
 * 	MAC address	BF_MYMAC	this interface's mac address
 * 	group address	BF_MYGROUP	the group address of this interface
 * 	functional	BF_MYFUNC	one of the funcs of this interf. hit
 * 	broadcast	BF_BROAD	one of the two broadcast addresses
 * 	multicast       BF_MYMULTI	or of MYGROUP, MYFUNC, and BROAD.
 *					Used to see if a multicast is for us.
 *
 * If any of these hit BF_LOCAL is set.  In addition if we are a REM server
 * and the frame is a MAC packet then pass it up as BF_LOCAL.  If we see
 * any MAC packets we assume we are a REM server.  No explicit check is
 * made.  Note that if we aren't a REM server we wouldn't have told the
 * chipset to give us mac packets.
 *
 */
static boolean tms380_lacceptor (hwidbtype *idb, fm_receive_t *rx,
				 paktype *pak)
{
    tms380_instance_t *ds = INSTANCE;
    srbroute_t *rif;
    tring_hdr *mac;
    sap_hdr *llc1;
    ushort buf_flags = 0;
    ushort pak_type  = 0;
    
    mac = (tring_hdr *)pak->datagramstart;

    if (mac->saddr[0] & TR_RII) {
        rif = (srbroute_t *)mac->data;
        rx->rif_length = rif->rc_blth & RC_LTHMASK;
        rx->rd_offset = 0;
        rx->llc_offset = TRING_ENCAPBYTES + rx->rif_length;
    } else {
        rx->rif_length = 0;
        rx->rd_offset = 0;
        rx->llc_offset = TRING_ENCAPBYTES;
    }
 
    if ((mac->daddr[0] & TR_GROUP) == 0) {
        /*
         * Specific address.  Must be ours.
 
         */
        buf_flags |= (BF_LOCAL | BF_MYMAC);
 
    } else if (mac->daddr[2] & TR_FAI) {
        /*
         * It's a group address.  If it is a broadcast, we do a few
         * more special checks.  If the packet is an all rings
         * broadcast (as determined by the RIF field), we insure that
         * the direction bit is not on, and that we have room for
         * another routing descriptor.
         */
        if (GETLONG(mac->daddr+2) != 0xffffffff)
            buf_flags |= (BF_LOCAL | BF_MYGROUP);
        else {
            buf_flags |= (BF_LOCAL | BF_BROAD);
        }
    } else {
        /*
         * It's a functional address
         */
        buf_flags |= (BF_LOCAL | BF_MYFUNC);
    }
 
    /*
     * Is it a MAC packet?  We always take MAC packets.
     */
    if ((mac->fc & FC_TYPE_MASK) == FC_TYPE_MAC) {
        buf_flags |= BF_MAC_PKT;
        rx->buf_flags = buf_flags;
        if((buf_flags & BF_MYMAC) == BF_MYMAC &&
	            ((mac->data[2] & MVID_DEST_CLASS_MASK) == RNG_STATION))
            return (FALSE);
        else return(TRUE);
    }
 
 
    /*
     * It's a LLC packet.  If it is a broadcast from us, junk it.
     */
    if ((mac->daddr[0] & TR_GROUP) &&
        ieee_equal_sans_rii(mac->saddr, ds->node_addr)) {
        return(FALSE);
    }
 
    /*
     * Did we accept this packet because we are in promiscuous mode?
     * Packets with our address will not match this criteria.
     */
    if ((rx->status & AC_AM_BOTH) == 0) {
        if (ieee_equal_sans_rii(mac->saddr, ds->node_addr))
            return(FALSE);
        buf_flags |= BF_EAVES;
    }
 
    llc1 = (sap_hdr *)((uchar *)mac + rx->llc_offset);
    if (llc1->control == LLC1_UI) {
        if (llc1->ssap == SAP_SNAP && llc1->dsap == SAP_SNAP)
            rx->data_offset = rx->llc_offset + SNAP_HDRBYTES;
        else
            rx->data_offset = rx->llc_offset + SAP_HDRBYTES;
    } else {
        rx->data_offset = 0;
    }
 
    /*
     * Determine what protocol this packet contains.
     */
    tms380_classify(llc1, &pak_type , &buf_flags);
    rx->buf_flags = buf_flags;
    rx->type = pak_type;

 
    return(TRUE);
}


/*
 * tms380_acceptor: determine how this packet should be accepted if any.
 *
 * The Acceptor determines how a packet should be processed.  It also
 * does some preprocessing for the main system by computing offsets and
 * storing them in known locations in the header.
 *
 * The LLC operation code is examined to see if it is an XID or TEST, and
 * if so we will set the BF_XIDTEST bit.  Note this bit is valid only if
 * BF_BRIDGE is also set.
 *
 * There also exists a filter table that contains a list of protocols
 * that Ma is currently routing.  We will never set the BRIDGE bit
 * for any of these packets.
 *
 * We also assume that any multicast packet will be completely examined
 * by the main system.  That is the system will determine whether a
 * multicast packet is truely eligible for bridging.  This is based on
 * the packets protocol type and other information.  Because of this
 * assumption there is no need for the bridge acceptor to examine the
 * filter table when we have a multicast for ourselves.  Other multicasts
 * (not for us) must still be examined.
 *
 * Buffer Flags:  These are returned by the Acceptor to indicate what it
 * found.
 *
 * 	BF_NONE		Neither acceptor tripped.  The packet will be discarded
 *
 * 	BF_LOCAL	The local acceptor tripped.  MYMAC, MYGROUP, REM_OTHER,
 * 			MYFUNC, BROAD, or EAVES.
 * 	BF_BRIDGE	The bridge acceptor tripped.  Can also have MYGROUP,
 * 			MYFUNC, or BROAD.
 * 	BF_MAC_PKT	packet is a mac packet
 * 	BF_MYMAC	Local match on node address.  Cannot be bridged.
 * 	BF_MYGROUP	Group hit.  Can also be bridged.
 * 	BF_MYFUNC	Functional hit.  More than FUNC_LOCAL can be bridged.
 * 	BF_BROAD	Either kind of broadcast.  Can be bridged.
 * 	BF_EXPLORER	Set on route explorer frames.
 * 	BF_EAVES	Not implemented.
 * 	BF_XIDTEST	an XID/TEST packet, only valid with BF_BRIDGE set.
 * 	BF_MYMULTI	or of MYGROUP, MYFUNC, and BROAD.  Used to see if a
 * 			multicast is for us.
 */

static boolean tms380_acceptor (hwidbtype *idb, fm_receive_t *rx, paktype *pak)
{
    tring_hdr *mac;
    srbroute_t *rif,*rifroute;
    sap_hdr *llc1;
    uchar *ptr;
    ushort buf_flags = 0;
    ushort pak_type = 0;
    int i, length = 0;
    ulong rd_info;
    tms380_instance_t *ds = INSTANCE;
 
    mac = (tring_hdr *)pak->datagramstart;
    if (mac->saddr[0] & TR_RII) {
        rif = (srbroute_t *)mac->data;
        rx->rif_length = length =  rif->rc_blth & RC_LTHMASK;
        rx->rd_offset = 0;
        rx->llc_offset = TRING_ENCAPBYTES + length;
    } else {
        rif = NULL;
        rx->rif_length = 0;
        rx->rd_offset = 0;
        rx->llc_offset = TRING_ENCAPBYTES;
    }
 
    /*
     * Did the source route accelerator chip zinged this one.  If so,
     * mark it as a bridged packet and go find the ring-bridge-ring
     * triplet that matched.  This saves the main system from having to
     * do the work.
     */
    if (rx->status & AC_AM_EXTMATCH) {
        buf_flags |= BF_BRIDGE;
 
        if (length == 2) {             /* RifLen = 2 (no Ring/Bridge info) */
            if (rif->rc_dlf & RC_D)
                rif->rc_dlf ^= RC_D;  
        }
        else if (length > 2) {
            ptr = (uchar *)rif->rd;
            if (rif->rc_dlf & RC_D) {
                /*
                 * reverse
                 */
                for (i = 2; i < length/2; i++, ptr += 2) {
                    rd_info = GETLONG(ptr);
                    if (!RD_MISMATCH(ds->rng_br_rng_r, rd_info)) {
                        rx->rd_offset = ptr - (uchar *)mac;
                        break;
                    }
                }
            } else {
                /*
                 * forward
                 */
                for (i = 2; i < length/2; i++, ptr += 2) {
                    rd_info = GETLONG(ptr);
                    if (!RD_MISMATCH(ds->rng_br_rng_f, rd_info)) {
                        rx->rd_offset = ptr - (uchar *)mac;
                        break;
                    }
                }
            }
        }
 
        if (rif && (rif->rc_blth & RC_BROADBIT)) {
            if (rif->rc_dlf & RC_D)
                return(FALSE);
 
            if ((length != 2) &&
                (length < 6 || length > ds->max_rif - 2))
                return(FALSE);
 
            if (rx->rd_offset) {
                /*
                 * Filter out any broadcast packets that have
                 * already traversed this bridge.  This will
                 * prevent looping of explorer packets.  The
                 * simple fact that we found our routing
                 * descriptor indicates that the packet has
                 * already crossed us.
                 */
                return(FALSE);
            }
            buf_flags |= BF_EXPLORER;
        }
    }
 
    /*
     * Did one of the internal addresses match?  If so, the possibilities
     * are 1) our MAC address, 2) a functional address, 3) a group
 
     * address, or 4) the broadcast address.  It *is* possible for both
     * internal and external matches to occur, in fact, it will happen
     * often.
     */
    if (rx->status & AC_AM_ADDRMATCH) {
        if ((mac->daddr[0] & TR_GROUP) == 0) {
            /*
             * Specific address.  Must be ours.
             */
            buf_flags |= (BF_LOCAL | BF_MYMAC);
 
        } else if (mac->daddr[2] & TR_FAI) {
            /*
             * It's a group address.  If it is a broadcast, we do a few
             * more special checks.  If the packet is an all rings
             * broadcast (as determined by the RIF field), we insure that
             * the direction bit is not on, and that we have room for
             * another routing descriptor.
             */
            if (GETLONG(mac->daddr+2) != 0xffffffff)
                buf_flags |= (BF_LOCAL | BF_MYGROUP);
            else {
                buf_flags |= (BF_LOCAL | BF_BROAD);
            }
        } else {
            /*
             * It's a functional address
             */
            buf_flags |= (BF_LOCAL | BF_MYFUNC);
        }
    }
 
    /*
     * Is it a MAC packet?  We always take MAC packets.
     */
    if ((mac->fc & FC_TYPE_MASK) == FC_TYPE_MAC) {
        buf_flags |= BF_MAC_PKT;
	rx->buf_flags = buf_flags;
        if((buf_flags & BF_MYMAC) == BF_MYMAC &&
	            ((mac->data[2] & MVID_DEST_CLASS_MASK) == RNG_STATION))
            return (FALSE);
        else return(TRUE);
    }
 
    /*
     * It's a LLC packet.  If it is a broadcast from us, junk it.
     */
    if ((mac->daddr[0] & TR_GROUP) &&
        ieee_equal_sans_rii(mac->saddr, ds->node_addr))
        return(FALSE);

    /* Remove multicast frames that have already traversed our
     * router . If {targetring | bridge | thisring} match then discard theses
     * frames . Note we are not checking unicasts as these are not
     * sent back from the madge/tms380 chip set .
     */
    if( (rif) && (buf_flags & (BF_BROAD | BF_MYFUNC))  &&
       (rif->rc_blth & RC_BROADBIT) && (!(rx->rif_length < 6)  )  )  {
            
	ptr = (uchar *)rif->rd;
 
         /* search forward only */
        
	for (i = 2; i < length/2; i++, ptr += 2) {
	    rd_info = GETLONG(ptr);
            if (!RD_MISMATCH(ds->rng_br_rng_r, rd_info)) {  /* targetring| bridge | thisring */
            	if (tokenevent_debug) {
                rifroute = (srbroute_t *)mac->data;
                    buginf("\nTMS380: Info-removing %s,  s: %e d: %e  rif: %s bf: %x",    
                           (buf_flags & BF_BROAD) ? "Broadcast" : "Funct.addr",
                           mac->saddr, mac->daddr,
                           get_rif_str((uchar *)rifroute, tms380_rif_string),
                           buf_flags);
	        }
	    return(FALSE);
	    }
	}
	 	
    }
    /*
     * Did we accept this packet because we are in promiscuous mode?
     * Packets with our of our addresses or a RIF will not match this
     * criteria.
     *
     * If we do match, see if it is suitable for transparent bridging or
     * if we are just eavesdropping on the wire.
     */
    if ((rx->status & AC_AM_BOTH) == 0) {
        if (ieee_equal_sans_rii(mac->saddr, ds->node_addr))
            return(FALSE);
        if (ds->tbridge_enabled) {         /* Bridging enabled */
 
           /*
             * Transparent Bridging enabled. We want to see everything
             * except source routed packets that should have passed
             * through another bridge.
             */
            if (length)
                return(FALSE);
	    /*
	     * Suppress reflections.
	     */
            for (i = 0; i < MAX_ADDRS; i++)
                if (ieee_equal_sans_rii(mac->saddr, ds->tbridge_addrs[i]))
                    return(FALSE);
 
            buf_flags |= BF_TBRIDGE;    /* Candidate for bridging */
        } else {
            buf_flags |= BF_EAVES;
        }
    }

    /*
     * Suppress reflections of physical broadcasts.
     */
    if ((ds->tbridge_enabled) && (is_ieee_bcast(mac->daddr))) {
	for (i = 0; i < MAX_ADDRS; i++)
	    if (ieee_equal_sans_rii(mac->saddr, ds->tbridge_addrs[i]))
		return(FALSE);
    }
 
    /*
     * Look at the LLC header
     */
    llc1 = (sap_hdr *)((uchar *)mac + rx->llc_offset);
 
    if (llc1->control == LLC1_UI) {
        if (llc1->ssap == SAP_SNAP && llc1->dsap == SAP_SNAP)
            rx->data_offset = rx->llc_offset + SNAP_HDRBYTES;
        else
            rx->data_offset = rx->llc_offset + SAP_HDRBYTES;
    } else {
            rx->data_offset = rx->llc_offset;
    }
 
    /*
     * Determine what protocol this packet contains.  For bridged
     * packets, we must also filter out any packets for protocols that
     * should be routed.
     *
     * If IRB(Integrated Routing and Bridging) is configured, we may
     * both route and transparent-bridge a specific protocol. Bypass
     * the filter hense we don't drop the routable packet and let
     * transparent-bridging code handle it.
     */
    tms380_classify(llc1, &pak_type, &buf_flags);
    if ((buf_flags & (BF_BRIDGE|(global_irb_enable ? 0 : BF_TBRIDGE))) &&

        !(buf_flags & BF_LOCAL) &&
        tms380_filter(ds, llc1))
            return(FALSE);
 
    /*
     * Everything passed.  Give this packet to the main system.
     */
    rx->buf_flags = buf_flags;
    rx->type = pak_type;
    return(TRUE);
}


/*
 * tms380_set_bridge_filters
 *
 * This routine is responsible for building the filter block that
 * tells a source route bridge to ignore certain routable protocols.
 */
static void tms380_set_bridge_filters (hwidbtype *idb)
{
    tms380_instance_t *ds;
    ushort *snap, *llc1, *llc2;
    idbtype *swidb = idb->firstsw;
    
    if ((idb->state != IDBS_UP) && (idb->state != IDBS_INIT))
	return;
    
    ds = INSTANCE;
    if (!ds)
	return;
    
    snap = ds->snapcodes;
    llc1 = ds->llc1codes;
    llc2 = ds->llc2codes;
    
    if (ROUTING_IF(swidb, LINK_CLNS)) {
	*llc1++ = (ushort) SAP_CLNS << 8 | SAP_CLNS;
    }
    if (ROUTING_IF(swidb, LINK_NOVELL))  {
	*llc1++ = (ushort) SAP_NOVELL << 8 | SAP_NOVELL;
	*snap++ = TYPE_NOVELL1;
    }
    if (ROUTING_IF(swidb, LINK_XNS)) {
	*llc1++ = (ushort) SAP_3COM << 8 | SAP_3COM;
	*snap++ = TYPE_XNS;
    }
    if (ROUTING_IF(swidb, LINK_VINES)) {
	*llc1++ = (ushort) SAP_VINES << 8 | SAP_VINES;
	*snap++ = TYPE_VINES2;
	*snap++ = TYPE_VINES_ECHO2;
    }
    if (ROUTING_IF(swidb, LINK_IP))  {
	*snap++ = TYPE_IP10MB;
    }
    if (ROUTING_IF(swidb, LINK_DECNET))  {
	*snap++ = TYPE_DECNET;
    }
    if (ROUTING_IF(swidb, LINK_APPLETALK))  {
	*snap++ = TYPE_ETHERTALK;
    }
    *snap = 0;
    *llc1 = 0;
    *llc2 = 0;
}


/*
 * Common code for SRB packets:
 */
boolean tms380_srb_common (hwidbtype *idb,
			   tring_hdr *trh,
			   paktype *pak,
			   uint buf_flags,
			   int riflen,
			   int rdoffset,
			   sap_hdr *llc1)
{
    if (tokenring_debug) {
	if (tr_print_packet(idb, " br:", trh, (ulong *)llc1,
			buf_flags, (pak->datagramsize - TR_FCS_SIZE))) {
	    buginf("\nTR%d:  br:      riflen %d, rd_offset %d, llc_offset %d", 
		   idb->unit, riflen, rdoffset, (uchar *)llc1 - (uchar *)trh);
	}
    }
    
    if (idb->tr_bridge_idb == NULL && idb->tr_vring_blk == NULL) {
	if (srb_debug || srbevent_debug) {
	    buginf("\nSRB%d: no path, s: %e d: %e rif: %s",
		   idb->unit, trh->saddr, trh->daddr,
		   ((trh->saddr[0] & TR_RII) ?
		    get_rif_str(trh->data, tms380_rif_string) : ""));
	}
        srbV.srb_sb_incr_rx_drops(idb->firstsw);
	/*
	### CSCdi47379: Must drop the pak
	*/
	datagram_done(pak);
	return(TRUE);
    }
    
    pak->network_start = pak->datagramstart;
    pak->datagramsize -= TR_FCS_SIZE;        /* hide FCS */
    pak->enctype = ET_NULL;
    pak->flags |= PAK_SRB_IN;
    GET_TIMESTAMP(idb->lastinput);    /* remember time of last input */
 
    reg_invoke_srt_srb_forward(pak, idb, riflen);
    idb->counters.rx_cumbytes += pak->datagramsize;
    idb->counters.inputs++;
    return(TRUE);
}


/*
 * Process packets destined for one of our interfaces:
 */
static void tms380_local_common (hwidbtype *idb,
				 tring_hdr *trh,
				 paktype *pak,
				 uint buf_flags,
				 int riflen,
				 int rdoffset,
				 sap_hdr *llc1)
{
    tms380_instance_t *ds;    /* Driver instance structure */
    boolean ui;
    int dgsize   = pak->datagramsize;
    idbtype *swidb = firstsw_or_null(idb);
   
    ds = INSTANCE;
 
    /*
     * This is for one of our addresses, be it our MAC, functional,
     * group, or the broadcast.
     */
    if (tokenring_debug)
       tr_print_packet(idb, " in:", trh, (ulong *)llc1,
            buf_flags, dgsize);
 
    /*
     * Is it a MAC frame?  If not, is it an unnumbered information
     * frame. If not, do the special hacks.  If so, determine which
     * network level protocol it is.
     */
    if (buf_flags & BF_MAC_PKT) {
       if (dgsize - riflen < TRING_SAP_ENCAPBYTES) {
           idb->counters.runts++;
           retbuffer(pak);
           return;
        }
        pak->network_start = pak->datagramstart + TRING_8025_ENCAPBYTES
                             + riflen;
        pak->enctype = ET_MAC;
        if (trh->daddr[0] & TR_GROUP)
            idb->counters.input_broad++;
        idb->counters.rx_cumbytes += dgsize;
        idb->counters.inputs++;
        (*idb->iqueue)(idb,pak);
        return;
    }
 
    /*
     * No, it's a LLC frame.
     */
    if (dgsize - riflen < TRING_SAP_ENCAPBYTES) {
       idb->counters.runts++;
       retbuffer(pak);
       return;
    }
 
    if (dgsize > idb->max_pak_size) {
       idb->counters.giants++;
       retbuffer(pak);
       return;
    }
 
    idb->rxrifwordlen = riflen/2;
    (*idb->parse_packet)(idb, pak, (uchar *)NULL);
    trh = (tring_hdr *) pak->datagramstart;
    llc1 = (sap_hdr *) (&trh->data[riflen]);
 
    if (buf_flags & BF_BRIDGE) {
        pak->flags |= PAK_SRB_IN;
        srbV.srb_sb_incr_rx_counters(idb->firstsw, dgsize);
    }
    if (buf_flags & BF_SBECHECKED)
        pak->flags |= PAK_MCICHECKED;
    if (trh->daddr[0] & TR_GROUP)
        idb->counters.input_broad++;
    idb->counters.rx_cumbytes += dgsize;
    idb->counters.inputs++;
 
    ui = ((llc1->control & ~LLC1_P) == LLC1_UI);
    if (!ui || srb_multiring_any_enabled(swidb))
	rif_update_llc(idb, 0, trh->saddr, NULL,
		   (srbroute_t *)(riflen ? trh->data : NULL), llc1, RIF_AGED);
    switch (llc1->control & ~LLC1_P) {
      case LLC1_UI:
        (*idb->iqueue)(idb,pak);
        return;
 
      case LLC1_XID:
        tr_process_xid(idb, pak, FALSE, FALSE);
        break;
 
      case LLC1_TEST:
        tr_process_test(idb, pak, FALSE);
        break;
 
      default:
        reg_invoke_llc2_input(pak);
        return;
    }
 
    /*
     * Frames which have BF_BRIDGE and are destined locally,
     * are locally processed, then bridged.
     * srb_common() will return the passed buffer into the free pool
     */
    if (buf_flags & BF_BRIDGE) {
        /*
         * Verify there _is_ a target local or virtual ring
         * before committing to forwarding the packet...
         */
        if (idb->tr_bridge_idb == NULL && idb->tr_vring_blk == NULL) {
            if (srb_debug || srbevent_debug) {
                buginf("\nSRB%d: no path, s: %e d: %e rif: %s",
                   idb->unit, trh->saddr, trh->daddr,
                   ((trh->saddr[0] & TR_RII) ?
		    get_rif_str(trh->data, tms380_rif_string) : ""));
             }
             srbV.srb_sb_incr_rx_drops(idb->firstsw);
         } else {
             /* target ring exists, forward and consume. */
             pak->flags |= PAK_SRB_IN;
	     reg_invoke_srt_srb_forward(pak, idb, riflen);
             return;
         }
    }
    retbuffer(pak);
}


/***** driver interface *****/

/*
 * tramon_init
 *
 * This routine is responsible for initilizing the main system
 * structures of the monitor.
 *
 *	1) initilize the main monitor global structure Sbe_globs.
 *	2) initilize pointers and other cells in global area that deal
 *	   with these things.
 *	3) Initilize the memory allocation mechanism.  Allocate MIN_MALLOC
 *	   plus any remaining change from Segment allocation.
 */
static void tramon_init (hwidbtype *idb)
{
    tms380_instance_t *ds;
    
    ds = INSTANCE;
    /*
     * Set up the shared system structure
     */
    ds->brd_flags = FLAG_PRESENT;
    ds->sca_fail = SCA_FAIL_NONE;
    ds->madge_kernel = VERNUM_SMTMAC_LOAD_PARMS;
    ds->madge_fastmac = VERNUM_FASTMAC_MODULE;
    
    /*
     * Set up the local data structure
     */
    ds->if_state = IF_DOWN;
    ds->adapter_reset = FALSE;
    idb_mark_unthrottled(idb);
    nail_bridge_stuff(idb);
}

/*
 * tms380_hard_reset
 * This function forces the token ring into a fully reset state.
 */
static boolean tms380_hard_reset (hwidbtype *idb)
{
    tms380_instance_t *ds;
    leveltype level;
    ulong dummy;
    
    ds = INSTANCE;
    level = raise_interrupt_level(NETS_DISABLE);
    
    tms380_update_prev_stats(idb);
    reg_invoke_media_rif_reset(idb);
    
    /*
     * Initialize the low level software:
     */
    tramon_init(idb);
    
    /*
     * Do a hard reset of the TMS380 and download its firmware:
     */
    if (!hardreset380(idb, &dummy)) {
	ds->brd_flags |= FLAG_HARD_FAIL;
	ds->sca_fail = dummy;
	reset_interrupt_level(level);
	if (idb->state != IDBS_ADMINDOWN)
	    errmsg(&msgsym(RESETFAIL, TR), idb->unit, dummy);
	return(FALSE);
    }
    /* force idb state to down? or done somewhere else? */
    ds->if_state = IF_DOWN;
    
    reset_interrupt_level(level);
    return(TRUE);
}


/*
 * tms380_soft_reset
 * This function forces the SBE 16/4 token ring card to reset it's
 * internal data structures and throw a soft reset at the TI chipset.
 * This is the equivalent of yanking the reset line on the TI processor,
 * except the TI code does not have to be downloaded into TI local memory
 * again.  Diagnostics are still performed.  Diagnostics may take 3
 * seconds to complete, and you should attempt them three times before
 * giving up.
 */
static boolean tms380_soft_reset (hwidbtype *idb)
{
    tms380_instance_t *ds;
    boolean result;
    ulong dummy;
    leveltype level;
    
    ds = INSTANCE;
    level = raise_interrupt_level(NETS_DISABLE);
    tms380_update_prev_stats(idb);
    reg_invoke_media_rif_reset(idb);
    
    ds->if_state = IF_CLOSING;
    nail_bridge_stuff(idb);
    idb_mark_unthrottled(idb);
    
    result = softreset380(idb, &dummy);
    ds->adapter_reset = TRUE;
    ds->if_state = IF_DOWN;
    if (result == FALSE) {
	ds->sca_fail = dummy;
	errmsg(&msgsym(RESETFAIL, TR), idb->unit, dummy);
    }
    
    reset_interrupt_level(level);
    return(result);
}


/*
 * tms380_try_autofix: try to bring the interface up automatically
 */
static forktype tms380_try_autofix (void)
{
    ulong  trash;
    ushort retries;
    tms380_instance_t *ds = NULL;
    hwidbtype *idb;
    tokensb *tsb;
    list_element *l_elt;
    
    FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_TMS380, l_elt, idb) {
	ds = INSTANCE;
	
	if (ds->pid && (ds->if_state == IF_AUTOFIX)) {
	    break;
	}
    }
 
    if (idb == NULL) {
        ds->pid = 0;
        process_kill(THIS_PROCESS);
        return;
    }

    /*
     * If we are administratively shut down, shut down the
     */
    if ((idb->state == IDBS_ADMINDOWN) || (idb->state == IDBS_INIT)) {
        ds->pid = 0;
        process_kill(THIS_PROCESS);
	return;
    }
    
    while (1) {
        if (idb->state != IDBS_ADMINDOWN)
            net_cstate(idb, IDBS_INIT);
        else
            break;

        trebuginf("\nTR%d: autofix", idb->unit);
    
        ds->if_state = IF_OPENING;
        for (retries = 3; retries > 0; retries--) {
	    if (softreset380(idb, &trash) && init380(idb, &trash)) {
	        trash = 0;
	        break;
	    }
	    trash = 1;
        }
        
        if (!trash && open380(idb, &trash))
	    trash = 0;
        else
	    trash = 1;
        
        if (!trash)
	    if (ds->bridge_enabled) {
	        if (cfgbridge380(idb, TRUE, &trash))
		    trash = 0;
	        else
		    trash = 1;
	    }
        
       if (trash || !chgmode380(idb, &trash)) {
	    softreset380(idb, &trash);
	    if (idb->state != IDBS_ADMINDOWN) {
	        ds->if_state = IF_DOWN;
	        net_cstate(idb, IDBS_DOWN);
	    }
	    trebuginf("failed");
            break;
        }
	tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);
        ds->if_state = IF_UP;
        net_cstate(idb, IDBS_UP);
        tsb->tr_lineup = TR_KEEP_COUNT;
        GET_TIMESTAMP(tsb->tr_lineuptimer);
        ds->brd_flags = FLAG_PRESENT | FLAG_OPERATE;
        idb->counters.transitions++;
        trebuginf("\nTR%d: Interface is alive, phys. addr %e",
	          idb->unit, idb->hardware);
        tms380_enable_host_interrupts(idb);
        (*idb->soutput)(idb);	/* See if there is anything to output */
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
        break;
    }
    ds->pid = 0;
    process_kill(THIS_PROCESS);
}

static forktype tms380_init_process (void)
{
    hwidbtype *idb;
    list_element *l_elt;
 
    FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_TMS380, l_elt, idb) {
	tms380_instance_t *ds = INSTANCE;
	
	if (ds->pid && (ds->if_state == IF_DOWN)) {
	    do {
		leveltype status;
		
		status = set_interrupt_level(ALL_ENABLE);
		process_sleep_for(ONESEC);
		reset_interrupt_level(status);
		
		(*idb->reset)(idb);
	    } while (idb->state == IDBS_DOWN);
	    
	    ds->pid = 0;
	}
    }
    process_kill(THIS_PROCESS);
}
 
/*
 * Certain_Death - Lock_up and Die gracefully?
 *
 * This routine stuffs a failure code and associated information into
 * the sca failure cells.  It also indicates the board is not operating
 * and HARD_FAIL indicates Certain_Death did it.
 *
 * Certain_Death is only called after the main system globals and initial
 * interrupt system have been set up.  It is never called prior to test0
 * completing, Hard_Death is what is used in that situation.  We never get
 * to SCA initialization under that circumstance.  We also assume that the
 * shadow cells have been kept reasonable.
 */

void tms380_certain_death (hwidbtype *idb,
			   uint fail,
			   ulong fail_code)
{
    tms380_instance_t *ds;
    struct _sifreg_t *sif_regs;
    tokensb *tsb;
    
    ds = INSTANCE;
    
    sif_regs = SIF_REGS;
    tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);
    ds->brd_flags |= FLAG_HARD_FAIL;	/* do it in this order */
    ds->brd_flags &= ~FLAG_OPERATE;	/* 'course this all happens */
    ds->sca_fail = fail;			/* in jam */
    ds->sca_fail_code = fail_code;
    
    errmsg(&msgsym(INTFAIL, TR), idb->unit, ds->brd_flags, ds->sca_fail,
	   ds->sca_fail_code, idb->state);
    
    ds->brd_flags &= ~FLAG_HARD_FAIL;
    if (idb->state != IDBS_ADMINDOWN) {
	net_cstate(idb, IDBS_DOWN);
	ds->if_state = IF_DOWN;
    }
    idb->counters.transitions++;
    tsb->tr_fail_count++;
    tsb->tr_last_fail = ds->sca_fail;
    tsb->tr_last_fail_code = ds->sca_fail_code;
    GET_TIMESTAMP(tsb->tr_last_fail_time);
    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
}

/*
 * Perform background tasks for the TMS380 interface
 */
static void tms380_periodic (hwidbtype *idb)
{
    tms380_instance_t *ds;
    
    ds = INSTANCE;
    
    /* If not shut down, do periodic functions */
    if ((idb->state != IDBS_ADMINDOWN) && (idb->state != IDBS_RESET)) {
        if ((idb->state == IDBS_DOWN) && (ds->if_state == IF_DOWN)) {
            if (!ds->pid)
                ds->pid = process_create(tms380_init_process, "TR Init", 
					 NORMAL_STACK, PRIO_NORMAL);
 
        } else if (ds->if_state == IF_AUTOFIX) {
            if (!ds->pid)
                ds->pid = process_create(tms380_try_autofix, "TR AutoFix",
					 NORMAL_STACK, PRIO_NORMAL);
        }
    }
}


static void tms380_tbridge_init (hwidbtype *idb,
				 boolean bridging)
{
    tms380_instance_t *ds = INSTANCE;
    idbtype *swidb;
    uchar bitswapped_address[IEEEBYTES];
    tokensb *tsb; 

    tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);

    /*
     * Initialize functional addresses
     */
    tr_functional_init(idb);
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {

	/*
	 * check if transparent bridging configured on any interface.
	 * if so, set up bridging for us.
	 * bridging_on_this_interface() checks for
	 *   a) bridging is active on this idb
	 *   b) bridge_enable
	 *   c) !(system_loading | bootstrap_enable)
	 */
	if (bridging_on_this_interface(swidb)) {
	    
            idb->tr_ring_mode |= RINGMODE_TBRIDGE;
            idb->tr_ring_mode |= RINGMODE_ALL_FRAMES;
            ds->tbridge_enabled = TRUE;
            if (!bridging)
                tms380_set_bridge_filters(idb);
	} else {
	    idb->tr_ring_mode &= ~RINGMODE_TBRIDGE;
	    idb->tr_ring_mode &= ~RINGMODE_ALL_FRAMES;
	    ds->tbridge_enabled = FALSE;
	}

	/*
	 * Insert/remove MAC addresses of interest in the SMF.
	 */
	ieee_swap(idb->hardware, bitswapped_address);
	reg_invoke_tbridge_smf_update(bitswapped_address,
				      swidb,
				      SMF_MACTYP_OWN_UNICAST |
				      SMF_ACTION_RECEIVE);

	reg_invoke_media_interesting_addresses(swidb);

	/*
	 * Setup source route bridging
	 */
	if (bridging) {
	    idb->tr_ring_mode |= RINGMODE_BRIDGE;
	    tsb->tr_functional |= TRFUNC_BRIDGE;
	} else {
	    idb->tr_ring_mode &= ~RINGMODE_BRIDGE;
	}
    }

    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
}


static boolean tms380_start (hwidbtype *idb,
			     boolean bridging)
{
    tms380_instance_t *ds;
    ulong error_code;
    ushort retries;
    ushort abort;
    tokensb  *tsb;
    idbtype  *swidb = firstsw_or_null(idb);
    
    ds = INSTANCE;
    trebuginf("starting.");

    /*
     * Do sanity checks on buffer size:
     */
    if ((idb->max_buffer_size < RC_LF_MIN) ||
	(idb->max_buffer_size > RC_LF_MAX)) {
	errmsg(&msgsym(BADBUFFSIZE, TR), idb->unit, idb->max_buffer_size);
	return(FALSE);
    }
    
    tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);

    /*
     * Check the node, group, and functional addresses:
     */
    tsb->tr_group |= ((ulong) TR_FAI << 24);
    
    /*
     * Check functional addressing to see if it conforms.  Basically
     * if someone tries to turn on one of the restricted addresses
     * with out the associated ring_mode we punt them.
     *
     * NETMGR is the old name.  It really is the configuration report
     * server.
     */

    tsb->tr_functional &= ~((ulong) TR_FAI << 24);
    if ((tsb->tr_functional & TRFUNC_MON)
	 || (!(idb->tr_ring_mode & RINGMODE_RPS)
	     && (tsb->tr_functional & TRFUNC_RPS))
	 || (!(idb->tr_ring_mode & RINGMODE_REM)
	     && (tsb->tr_functional & TRFUNC_REM))
	 || (!(idb->tr_ring_mode & RINGMODE_NETMGR)
	    && (tsb->tr_functional & TRFUNC_CRS))) {
	errmsg(&msgsym(BADFUNC, TR),
	       idb->unit, tsb->tr_functional, idb->tr_ring_mode);
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	return(FALSE);
    }

    if (bridging) {
        srb_triplet_t srb_triplet;
        srb_vector_table_t *srbV_p = reg_invoke_srb_get_vector_table();
        if (srbV_p) {
            srbV_p->srb_sb_get_triplet(swidb, &srb_triplet);
        } else {
            srb_triplet.localRing  = SRB_INVALID_RING_NUM;
            srb_triplet.bridgeNum  = SRB_INVALID_BRIDGE_NUM;
            srb_triplet.remoteRing = SRB_INVALID_RING_NUM;
        }
	/*
	 * Fix up ring_mode and Functional addresses so everything is
	 * consistent for bridging because we do a few more things:
	 */
	if ((srb_triplet.bridgeNum > SRB_MAX_BRIDGE_NUM) ||
	    (srb_triplet.localRing > SRB_MAX_RING_NUM) ||
	    (srb_triplet.remoteRing > SRB_MAX_RING_NUM) ||
	    (ds->max_rd < SRB_MIN_MAXRD) ||
	    (ds->max_rd > SRB_MAX_MAXRD)) {
	    errmsg(&msgsym(BADBRDGPARMS, TR),
		   idb->unit, 
                   srb_triplet.bridgeNum,
                   srb_triplet.localRing,
		   srb_triplet.remoteRing);
	    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	    return(FALSE);
	}
	idb->tr_ring_mode  |= RINGMODE_BRIDGE;
	tsb->tr_functional |= TRFUNC_BRIDGE;
	ds->bridge_enabled = TRUE; 
	ds->rng_br_rng_f = (   (srb_triplet.localRing  << 20) 
			     | (srb_triplet.bridgeNum  << 16) 
			     | (srb_triplet.remoteRing << 4) );
	ds->rng_br_rng_r = (   (srb_triplet.remoteRing << 20) 
			     | (srb_triplet.bridgeNum  << 16) 
			     | (srb_triplet.localRing  << 4) );
	ds->max_rif = SRB_MAXRIF;
	
    } else {
	/*
	 * Fix up ring_mode and Functional addresses so everything is
	 * consistent with no bridging:
	 */
	idb->tr_ring_mode &= ~RINGMODE_TBRIDGE;
	tsb->tr_functional &= ~TRFUNC_BRIDGE;
	ds->bridge_enabled = FALSE;
	ds->rng_br_rng_f = 0L;
	ds->rng_br_rng_r = 0L;
	ds->max_rif =  0;
    }
    /*
     * Align server_rem so we can also use it when seeing
     * if the dest was for a REM server and we are one.  REM
     * packets may be generated by this interface and need to
     * be passed upstairs.
     */
    if (idb->tr_ring_mode & RINGMODE_REM) {
	tsb->tr_functional |= TRFUNC_REM;
	ds->server_rem = TRFUNC_REM;
    } else
	ds->server_rem = FALSE;
    if (idb->tr_ring_mode & RINGMODE_RPS) {
	tsb->tr_functional |= TRFUNC_RPS;
	ds->server_rps = TRUE;
    } else
	ds->server_rps = FALSE;
    if (idb->tr_ring_mode & RINGMODE_NETMGR) {
	tsb->tr_functional |= TRFUNC_CRS;
	ds->server_netmgr = TRUE;
    } else
	ds->server_netmgr = FALSE;
    
    /*
     * Incoming data appears to be okay.  Now do the real
     * open.
     */
    abort = 0;
    ds->if_state = IF_OPENING;
    for (retries = 3; retries > 0; retries--) {
	if (softreset380(idb, &error_code)) {
	    if (init380(idb, &error_code)) {
		abort = 0;
		break;
	    }
	    if (tokenevent_debug)
		errmsg(&msgsym(INITFAIL, TR), idb->unit, retries, error_code);
	    abort = RSLT_START_FAIL_INIT;
	} else {
	    if (tokenevent_debug)
		errmsg(&msgsym(INITFAIL, TR), idb->unit, retries, error_code);
	    abort = RSLT_START_FAIL_RESET;
	}
    }
    if (abort) {
	if (tokenevent_debug)
	    errmsg(&msgsym(INITFAIL, TR), idb->unit, abort, error_code);
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	return(FALSE);
    }
    
    /*
     * Simple insertion.  Don't take a chance on being swamped.
     */
    if (!open380(idb, &error_code)) {
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	return(FALSE);
    }
    
    /*
     * Set the rest of the open options.
     */
    if (!chgmode380(idb, &error_code)) {
	errmsg(&msgsym(MODEFAIL, TR), idb->unit, RSLT_MODE_FAILED,
	       error_code);
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	return(FALSE);
    }
    
    /* configure the SRA chip */

    if (bridging) {                               /* Enable the SRA chip */
	if (!cfgbridge380(idb, TRUE, &error_code)) {
	    close380(idb);
	    errmsg(&msgsym(MODEFAIL, TR), idb->unit, RSLT_CFGBR_FAILED,
		   error_code);
	    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	    return(FALSE);
	}
    } else {                                     /* Disable the SRA */
	if (!cfgbridge380(idb, FALSE, &error_code)) {
	    close380(idb);
	    errmsg(&msgsym(MODEFAIL, TR), idb->unit, RSLT_CFGBR_FAILED,
		   error_code);
	    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	    return(FALSE);
	}
    }

    /*
     * The startup took.
     */
    tms380_enable_host_interrupts(idb);
    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
    return(TRUE);
}


/*
 * Setup tunneled bridging
 */
static boolean tunnel_setup_tring_bridge (hwidbtype *hwidb)
{
    ulong error_code;
    uint temp;
    tokensb *tsb;

    tsb = idb_use_hwsb_inline(hwidb, HWIDB_SB_TOKEN);

    tsb->tr_functional |= TRFUNC_BRIDGE;

    if (!set_address_380(hwidb, DIR_SET_FUNCTIONAL_ADDRESS,
			 tsb->tr_functional, &error_code)) {
    }

    tsb->tr_group |= ((ulong) TR_FAI << 24);
    if (!set_address_380(hwidb, DIR_SET_GROUP_ADDRESS,
			 tsb->tr_group, &error_code)) {
    }

    temp = hwidb->tr_ring_mode;
    hwidb->tr_ring_mode |= RINGMODE_ALL_FRAMES;
    if (!chgmode380(hwidb, &error_code)) {
    }
    hwidb->tr_ring_mode = temp;

    tms380_enable_host_interrupts(hwidb);

    idb_release_hwsb_inline(hwidb, HWIDB_SB_TOKEN);

    return(TRUE);
}


/*
 * tms380_restart
 *
 * Reset the token ring interface.  This routine checks for the
 * administratively down state.
 */
static void tms380_restart (hwidbtype *idb)
{
    tms380_instance_t *ds;
    rx_slot_ptr *rxptr;
    tx_slot_ptr *txptr;
    int i;
    leveltype status;

    SAVE_CALLER();

    /*
     * Check to see if we're tunneling to a UNIX router, if we are, just
     * set up the the bridging vector and jump out.
     */
    if (idb->remif != NULL) {
	idb->remif->tring_bridge_setup = tunnel_setup_tring_bridge;
	return;
    } 

    /*
     * If we are administratively shut down, shut down the
     */
    if ((idb->state == IDBS_ADMINDOWN) || (idb->state == IDBS_INIT))
	return;
    
    ds = INSTANCE;
    
    if (tokenevent_debug) {
	buginf("\nTR%d: reset from %x", idb->unit, caller());
	buginf("\nTR%d: txtmr: %#Ta, msclk: %#Tn, qt: %x (%dms)\n", idb->unit,
	       idb->xmittimer, idb->lastoutput, ELAPSED_TIME(idb->lastoutput));
    }
    
    /*
     * Are resets allowed yet?
     */
    if (nvsize && !system_configured) 
	return;
    
    /*
     * Housekeeping
     */
    status = raise_interrupt_level(NETS_DISABLE);
   
    tms380_update_prev_stats(idb);
    reg_invoke_media_rif_reset(idb);
    tms380_vendor_init(idb);        /* vendor specific setups */

    if (!ds->reset_tms380_on_fail) {
	net_cstate(idb, IDBS_DOWN);
	reset_interrupt_level(status);
	return;
    }

    /*
     * Flush packets in hold queues
     */
    holdq_reset(idb);

    /*
     * flush the packets in the tx and rx rings
     */
    if (ds->rx_p) {
	rxptr = ds->rx_p;
        for (i = 0; i < ds->rx_size; i++) {
            if (rxptr->pak)
                datagram_done(rxptr->pak);
            rxptr->pak = NULL;
            rxptr->fm_rx_space = 0;
            rxptr++;
        }
        free(ds->rx_p);
	ds->rx_p = NULL;
    }
 
    if (ds->tx_p) {
        txptr = ds->tx_p; 
        for (i = 0; i < ds->tx_size; i++) {
            if (txptr->pak) 
                datagram_done(txptr->pak); 
            txptr->pak = NULL;
            txptr->fm_tx_space = 0; 
            txptr++; 
        }   
        free(ds->tx_p);
        ds->tx_p = NULL; 
    }

    tms380_set_ringspeed(idb);  /* set the ring speed */

    reg_invoke_local_lnm_reset_count(idb);

    /*
     * If interface is administratively down, don't re-enable
     */
    if (idb->state != IDBS_ADMINDOWN)
	(void) tms380_initialize(idb);
    
    reset_interrupt_level(status);
}


/*
 * tms380_shutdown
 * 
 * Cause the interface to completely shut up.
 * Newstate is advisory information on what state to mark the idb.
 *
 * On exit the only allowable state is either ADMINDOWN or RESET.
 * Interrupts are disabled because we do the hard reset prior to
 * closing the gate.  Has to be this way.
 */
static void tms380_shutdown (hwidbtype *idb,
			     int newstate)
{
    tms380_instance_t *ds;
    rx_slot_ptr *rxptr;
    tx_slot_ptr *txptr;
    int i;
    leveltype level;

    SAVE_CALLER();
    
    ds = INSTANCE;
    level = raise_interrupt_level(NETS_DISABLE);
    idb->counters.transitions++;
    
    holdq_reset(idb);
    
    /*
     * flush the packets in the tx and rx rings
     */
    if (ds->rx_p) {
        rxptr = ds->rx_p;
        for (i = 0; i < ds->rx_size; i++) {
            if (rxptr->pak)
                datagram_done(rxptr->pak);
            rxptr->pak = NULL;
            rxptr->fm_rx_space = 0;
            rxptr++;
        }
        free(ds->rx_p);
        ds->rx_p = NULL;
    }

    if (ds->tx_p) {
        txptr = ds->tx_p;
        for (i = 0; i < ds->tx_size; i++) {
            if (txptr->pak)
                datagram_done(txptr->pak);
            txptr->pak = NULL;
            txptr->fm_tx_space = 0;
            txptr++;
        }
        free(ds->tx_p);
        ds->tx_p = NULL;
    }

    /* 
     * when shutting down the interface, delete the interface buffer  
     * pool cache in an attempt to save some packet memory 
     */

    if (idb->pool->flags & POOL_CACHE)
        pool_destroy_cache(idb->pool);

    trebuginf("\nTR%d: interface shutdown, from %#x\n",
	      idb->unit, caller());
    
    tms380_reset(idb);
    
    if (idb->state != IDBS_ADMINDOWN) {
	if (newstate != IDBS_ADMINDOWN)
	    net_cstate(idb, IDBS_RESET);
	else
	    net_cstate(idb, IDBS_ADMINDOWN);
    }
    
    reset_interrupt_level(level);
}


/*
 * tms380_disable
 *
 * Disable an interface to throttle it because of excessive inputs.
 */
static void tms380_disable (hwidbtype *idb)
{
    tms380_instance_t *ds;
    
    ds = INSTANCE;
    
    tms380_disable_host_interrupts(idb);
    idb_mark_throttled(idb);
}

/*
 * tms380_enable
 *
 * Enable an interface after it has been throttled for excessive inputs.
 */
static void tms380_enable (hwidbtype *idb)
{
    tms380_instance_t *ds;
    
    ds = INSTANCE;
    
    idb_mark_unthrottled(idb);
    tms380_enable_host_interrupts(idb);
}


/***** interrupt handlers, transmit/receive ring handling *****/

static inline paktype *tms380_getbuf_inline (hwidbtype *idb, boolean cache_ok)
{
  paktype *pak = NULL;
 
  if (idb->input_qcount > idb->input_defqcount)
    return(NULL);
 
  /*
   * Try to get a cache buffer first
   */
  if (cache_ok) {
      pak = pool_dequeue_cache(idb->pool);
  }
  /*
   * the token ring driver did keep any buffer on its own free list so
   * instead of going thro' private_getbuf, we go directly to the pool.
   */
  if (!pak) 
     pak = pool_getbuffer(idb->pool);
  
  if (pak) {
#ifdef DEBUG
        if (!validmem(pak)) {
            buginf("\ntms380_getbuf: Invalid p=%x\n", pak);
            return(NULL);
        }
        if (pak->next != NULL) {
            buginf("\ntms380_getbuf: buffer in list, p=%x\n", pak);
            return(NULL);
        }
        if (pak->refcount != 1) {
            buginf("\ntms380_getbuf: refcount not one, p=%x\n", pak);
            return(NULL);
        }
#endif
       pak->datagramstart = pak->network_start = pak_center(pak);

       return(pak);
  } else
    idb->counters.output_nobuffers++;
 
  return(NULL);
}

paktype *tms380_getbuf (hwidbtype *idb, boolean cache_ok)
{
    return(tms380_getbuf_inline(idb,FALSE));
}

static boolean tms380_set_maxdgram (hwidbtype *idb, int buffersize,
				    int maxdgram)
{
    if (!(idb->type == IDBTYPE_TMS380)) {
	printf(set_maxdgram_failed, maxdgram, idb->hw_namestring);
        return(FALSE);
    }
 
    if_maxdgram_adjust(idb, maxdgram);

    /*
     * Initialize the interface.
     */
    tms380_restart(idb);
    return(TRUE);
}


static void tms380_copy_src_addrs (tms380_instance_t *ds,
				   paktype *pak)
{
    tring_hdr *mac;
    int index;
    
    /*
     * We need to copy source addresses when transparent bridging is
     * enabled.  This allows us to discard these packets when they come
     * back in on the interface.
     */
    index = ds->tbridge_index;
    mac = (tring_hdr *)pak->datagramstart;
	
    ieee_copy(mac->saddr, ds->tbridge_addrs[ds->tbridge_index++]);
    if (ds->tbridge_index >= MAX_ADDRS)
	ds->tbridge_index = 0;
}


/*
 * tms380_tx_pak
 *
 * put pak on transmit ring if there's room.
 * return TXRV_DROPPED - packet too large
 *	  TXRV_QUEUE_IT - no room on Tx ring
 *	  TXRV_SENT - Tx done
 * does not put packets on holdqueue.
 */
static int tms380_tx_pak (hwidbtype *idb,
			  paktype *pak)
{
  tms380_instance_t *ds = INSTANCE;
  struct _sifreg_t *sif_regs = SIF_REGS;
  ushort small_buf_len = 0, large_buf_len = 0;
  volatile tx_slot_ptr *tx_slot;
  ushort tx_slot_space;
  uchar  *dgstart;
  int    dgsize;
 
  dgstart = (uchar *)pak->datagramstart;
  dgsize  = pak->datagramsize;
 
  /* Prevent too large packets from being sent  */
  if (dgsize  > idb->max_pak_size) { /* too big, dump it */
    ds->send_toobig++;
    datagram_done(pak);
    return(TXRV_DROPPED);  /* disposed of if not sent! */
  }
 
  tx_slot = &ds->tx_p[ds->tms380_tx_slot];
  tx_slot_space = tx_slot->fm_tx_space;
 
  /*
   * Go thro' DIO and see if slot is ready to transmit.
   * If both the small buffer length and large buffer length is zero,
   * it is ready to transmit.
   * Just the large buffer is used for transmit.
   */
 
  sif_regs->adrx = ADDR_CHPT1;
  sif_regs->adr  = tx_slot_space + FM_TX_SMALL_BUF_LEN;
  small_buf_len  = sif_regs->datainc;
  large_buf_len  = sif_regs->datainc;
 
  /*
   * Both small and large buffer length has to be zero.
   */
  if ((small_buf_len != 0)||(large_buf_len != 0)) {
    ds->tx_buffer_full++;
    return(TXRV_QUEUE_IT);
  }
 
  /* setup the ac byte */
  if (pak->flags & PAK_SRB_OUT) {
    /* force the recalculation of the crc */
    *dgstart = AC_PRI4;
  } else if (pak->flags & PAK_SRB_EXPLORER) {
    *dgstart = AC_PRI4;
  } else
    *dgstart = AC_PRI0;
 
 
  /*
   * There are two output code path with in the Fastmac plus firmware,
   * if frames that are lesser than 1016 byte (internal buffer size within
   * the firmware) are sent using the small buffer path, we can have a smaller
   * latency for transmission.
   */
 
  if (dgsize < TMS380_LARGE_BUF_SIZE) {
      sif_regs->adr = tx_slot_space + FM_TX_SMALL_BUF_ADDR_MSW;
      sif_regs->datainc = ((ulong)dgstart >> 16);
      sif_regs->datainc = ((ulong)dgstart & 0xFFFF);
 
      sif_regs->adr = tx_slot_space + FM_TX_LARGE_BUF_LEN;
      sif_regs->data = 0;
      sif_regs->adr = tx_slot_space + FM_TX_SMALL_BUF_LEN;
      sif_regs->data = dgsize;
  } else {
 
      sif_regs->adr = tx_slot_space + FM_TX_LARGE_BUF_ADDR_MSW;
      sif_regs->datainc = ((ulong)dgstart >> 16);
      sif_regs->datainc = ((ulong)dgstart & 0xFFFF);
 
      /*
       * Inform madge that a buffer exists
       * KEEP THE ORDER OF ACCESS TO REGISTERS
       */
 
      sif_regs->adr = tx_slot_space + FM_TX_LARGE_BUF_LEN;
      sif_regs->data = dgsize;
      sif_regs->adr = tx_slot_space + FM_TX_SMALL_BUF_LEN;
      sif_regs->data = TMS380_NO_SMALL_BUFFER_FOR_TX;
  }
  if (tx_slot->pak) 
    datagram_done(tx_slot->pak);
  tx_slot->pak = pak;
 
  /*
   * Update the ring pointer
   */
  ds->tms380_tx_slot++;
  if (ds->tms380_tx_slot == ds->tx_size)
	ds->tms380_tx_slot = 0;
 
 
  if (ds->tbridge_enabled)
    tms380_copy_src_addrs(ds, pak);
 
  ds->active_counts.tx_sent++;
  ds->active_counts.tx_bytes_sent += dgsize;
  if (dgsize <= MAX_FRAME/2)
    ds->active_counts.tx_small++;
  else
    ds->active_counts.tx_large++;
 
  token_output_debug(idb, pak);
 
  if (dgstart[0] & TR_GROUP)
    idb->counters.output_broad++;
 
  GET_TIMESTAMP(idb->lastoutput);
  return(TXRV_SENT);
}


/*
 * tms380_tx_free - free paks from tx->adapter_ptr to adapter_ptr
 *
 * madge chases host pointer.  
 * we move ds->tx_adapter_ptr to catch up with adapter_ptr(madge)
 * freeing paks along the way.
 */
static void tms380_tx_free (hwidbtype *idb)
{
    tms380_instance_t *ds = INSTANCE;
    struct _sifreg_t *sif_regs;
    tx_slot_ptr * tx_ptr;
    ushort large_buf_len;
    ushort small_buf_len;
    int    end;
 
    sif_regs = SIF_REGS;
    if (ds->tms380_tx_slot == 0)
        end = ds->tx_size - 1;
    else
        end = ds->tms380_tx_slot - 1;


    while (ds->tms380_tx_free_slot != end) {

        tx_ptr = &ds->tx_p[ds->tms380_tx_free_slot];
        
        sif_regs->adrx = ADDR_CHPT1;
        sif_regs->adr = tx_ptr->fm_tx_space + FM_TX_LARGE_BUF_LEN;
        large_buf_len = sif_regs->data;
        sif_regs->adr = tx_ptr->fm_tx_space + FM_TX_SMALL_BUF_LEN;
        small_buf_len = sif_regs->data;

        if (large_buf_len == 0 && small_buf_len == 0) {
            if (tx_ptr->pak) {
                datagram_done(tx_ptr->pak);
                tx_ptr->pak = NULL;
            } else
                break;
	    ds->tms380_tx_free_slot++;
            if (ds->tms380_tx_free_slot == ds->tx_size)
            ds->tms380_tx_free_slot = 0;
        } else
            break;
    }
}

static inline boolean
tms380_multicast_srb_explorer (
    ushort buf_flags,
    idbtype *idb,
    ushort rxtype,
    sap_hdr* llc1)
{
    if ((buf_flags & (BF_BRIDGE | BF_EXPLORER | BF_BROAD))
                  == (BF_BRIDGE | BF_EXPLORER | BF_BROAD)) 
        if (multicast_explorer_can_be_bridged(idb, rxtype, llc1))
            return(TRUE);

    return(FALSE);
}


/*****************************************************************************
 * Created by combining David Hampton's advance_rcvr() and madge_input()
 *****************************************************************************
 *
 * tms380_madge_input
 *
 * Receiver processing now takes the form of an acceptor procedure that has
 * two components.  These components look and result in flags that signify
 * how this packet should be processed if at all.  These acceptors are further
 * documented in the acceptor routine itself.
 *
 * When bridging we also must pass the CRC undisturbed.  This is included
 * in the packet and the size adjusted accordingly. When bridging the
 * chipset must always be running with PASS_CRC so we can get the
 * bits we need if we need to bridge a packet.  The checks get simpler if
 * we always run with pass_crc on even if bridging isn't officially turned
 * on.
 *
 * To solve this problem, the bridge code is responsible for adjusting any
 * packet's size if it isn't bridged.  This turns out to actually be quite
 * simple.  Once a packet has been accepted a final check is done. If the
 * bridge code is turned on (SRBRIDGE defined) and the BF_BRIDGE flag is off
 * then we should adjust.
 *
 * Then, we need to check the nab flags. If we are considering
 * this packet for enqueuing locally and the BF_BRIDGE flag is set then
 * adjust the packet size by 4 (CRC size).  In other words:
 *
 * 	no flags		enqueue locally, no adjustment
 * 	BF_LOCAL		enqueue locally, no adjustment
 * 	BF_LOCAL & BF_BRIDGE	enqueue locally, adjust len
 * 	BF_BRIDGE		no local, bridge adjust len
 *
 * Bridged packets are allocated enough space to include the CRC but
 * the datagram length is actually 4 less (doesn't include the CRC).
 * A packet flag is set to indicate that this is true.  On output, a
 * special encapsulation type is used to indicate that this packet
 * is special.  We then take care of restoring the proper
 * length before it is handed to the interface.
 *
 * Start input on a TMS380 token ring interface.  This routine reads
 * directly from IO memory that is shared between the system and the
 * TMS380 chipset.  See if_xx_tms380.h for a complete description
 * of how buffers are passed between the Madge firmware and this driver.
 *
 * WARNING: We rely on bridge_enable (source routing allowed) gating in
 * higher levels of the code from kicking the interface into bridge mode.
 * This prevents the BF_BRIDGE flag from ever appearing that would result
 * in the packet from potentially being enqueued on the srbQ which might
 * not exist (terminal server, etc.)  If packets got put on this queue and
 * the srb_explorer process wasn't running it would be a memory leak.
 ***********************************************************************/
static void tms380_madge_input (hwidbtype *idb,
				rx_slot_ptr *rx_slot)
{
    tms380_instance_t *ds = INSTANCE;
    struct _sifreg_t *sif_regs;
    fm_receive_t receive;
    int size;
    int count = 0;
    paktype *newpak;
    iphdrtype *ip;

    sif_regs = SIF_REGS;
 
    /* Get packet length */
    sif_regs->adrx = ADDR_CHPT1;
    sif_regs->adr  = rx_slot->fm_rx_space;
    size           = sif_regs->data;
 
    sif_regs->adr   = rx_slot->fm_rx_space + FM_RX_STATUS;
    receive.status = sif_regs->data;
 
    while (size && (count++ < ds->rx_size)) {
        uchar *dgstart;
        tring_hdr *trh;
        sap_hdr *llc1 = NULL;
        snap_hdr *snap;
        paktype *pak, *lpak;
        boolean keep;
        boolean multicast_to_srb_common = FALSE;
        ushort packet_type = 0, buf_flags = 0;
        ushort snaptype;
        int riflen;
        uchar status;

        if ((status  = ((receive.status & FM_RX_FRAME_STATUS) >> 8)) != 0) {
            if (status & FM_RX_DELIMITER_ERROR)
                ds->rx_delimiter_error++;
            if (status & FM_RX_IMPLICIT_ERROR)
                ds->rx_imp_error++;
            if (status & FM_RX_EXPLICIT_ERROR)
                ds->rx_exp_error++;
            if (status & FM_RX_DMA_OVERRUN)
                ds->rx_dma_overrun++;
            if (status & FM_RX_BUFFER_RUNOUT)
                ds->rx_buf_runout++;
 
            TMS380_RX_NEXT_PACKET
            continue;
        }
 
 
        if ((size > MAX_FRAME) || (size < MIN_FRAME)) {
            if (size > MAX_FRAME)
                ds->active_counts.rx_giants++;
            else
                ds->active_counts.rx_runts++;
            TMS380_RX_NEXT_PACKET
            continue;
        }

        lpak = (paktype *)tms380_getbuf_inline(idb, TRUE);
        if (!lpak) {
            tms380_disable(idb);
            idb->counters.input_drops++;
            TMS380_RX_NEXT_PACKET
            continue;
        }
 
        dgstart = lpak->datagramstart;
 
        sif_regs->adr = rx_slot->fm_rx_space + FM_RX_MSW_BUFFER_ADDR;
        sif_regs->datainc = ((ulong)dgstart >> 16);
        sif_regs->datainc = ((ulong)dgstart & 0xFFFF);
 
        pak = rx_slot->pak;
        rx_slot->pak = lpak;
 
        dgstart = pak->datagramstart;
        pak->datagramsize = size;
        pak->if_input     = (idbtype *)idb;
        receive.pak  = pak;
        receive.data = pak->datagramstart;
        receive.data_length = size;
 
        set_if_input_inline(pak, idb->firstsw);
 
        if (ds->bridge_enabled || ds->tbridge_enabled)
            keep = tms380_acceptor(idb, &receive, pak);
        else
            keep = tms380_lacceptor(idb, &receive,pak);
 
        if (idb->remif != NULL) {
 
            pak->riflen =  receive.rif_length;
            pak->enctype = ET_MAC;
            pak->network_start = pak->datagramstart + TRING_8025_ENCAPBYTES
                                + pak->riflen;
            pak->datagramsize = size - TR_FCS_SIZE; /* remove FCS */
 
            TMS380_RX_NEXT_PACKET
            (*idb->iqueue)(idb,pak);
            continue;
        }

#ifdef TBRIDGE_DEBUG	/* see the monument in tbridge_debug_flags.h !!! */
	if (tbridge_debug_pak_0) {

	    uchar	*type_string;

	    type_string = (receive.type <= RXTYPE_LAST) ?
		rxtype_string(receive.type) :
		    "unkown";

	    buginf("\nTB0: %04x flags %04x %s %s on %s"
		   "\n     %e to %e %d bytes",
		   receive.status, receive.buf_flags,
		   (keep ? "" : "FLUSH"),
		   type_string,
		   idb->firstsw->namestring,
		   ((tring_hdr *)(pak->datagramstart))->saddr,
		   ((tring_hdr *)(pak->datagramstart))->daddr, size);
	}
#endif	/* TBRIDGE_DEBUG */

        if (!keep) {
            /*
             * This packet was rejected by either the bridging acceptor
             * or the local acceptor.  If the local acceptor was called,
             * this is because the destination address was not our local,
             * multicast, or broadcast address.  If the bridging acceptor
             * was called this is because the same address check failed
             * and our ring-bride-ring triplet was not found in the RIF
             * field of the packet.  Note: Either acceptor will fail on a
             * broadcast packet from this interface.  This saves hassle
             * upstairs in the main system.  Nuke the packet.
             */
            retbuffer(pak);
            TMS380_RX_NEXT_PACKET
            continue;
        }
 
        buf_flags = receive.buf_flags;
        if (buf_flags & BF_LOCAL)
            ds->active_counts.rx_local++;
        else if (buf_flags & (BF_BRIDGE | BF_TBRIDGE))
            ds->active_counts.rx_bridge++;
 
        if (size <= MAX_FRAME/2)
            ds->active_counts.rx_small++;
        else
            ds->active_counts.rx_large++;
 
        riflen = receive.rif_length;
        packet_type = receive.type;

	/*
	 * Very high speed switching of routed protocols. Call the IP unicast
	 * fastswitching path for IP multicast packets. It will in turn
	 * call the IP multicast fastswitching path.
	 */
	if (((buf_flags & 
	     (BF_BRIDGE|BF_MYMULTI|BF_MYMAC|BF_EXPLORER|BF_MAC_PKT))
	      == BF_MYMAC) || 
	    ((buf_flags & BF_MYMULTI) && packet_type == RXTYPE_SNAP_DODIP)) {
 
            /*
             * This attempts to be somewhat more general than the previous
             * comparison against just SNAP_DODIP and SAP_NOVELL, but still
             * allow this code to be ignored if the most common definition
             * for the 'disabled' element in the fast switching matrix is
 
             * in use (that being 'return_false'). Also sanity check against
             * a NULL fast switch matrix element.
             */
            idb->rxtype = packet_type;
 
            pak->encsize = receive.data_offset;
            pak->riflen  = riflen;
            pak->network_start = pak->datagramstart +
                                     receive.data_offset + riflen;
            pak->datagramsize = size - TR_FCS_SIZE; /* remove FCS */
 
            token_decode_encapsulation_inline(pak,
                     packet_type,(tring_hdr *)pak->datagramstart);
 
            if (reg_invoke_token_fs(packet_type, pak)) {
                ds->active_counts.rx_sent++;
                ds->active_counts.rx_bytes_sent += size;
                TMS380_RX_NEXT_PACKET
                continue; /* next packet */
	    }

	    /*
	     * Check for IP tunneled packets.
	     */
	    if (packet_type == RXTYPE_SNAP_DODIP) {
		ip = (iphdrtype *) pak->network_start;
		if (ip->prot == GRE_PROT || ip->prot == IPINIP_PROT) {
		    if (reg_invoke_tunnel_ip_les_fs(pak))
			continue;
		}
	    }

	    pak->datagramsize = size; /*replace FCS */
        }
 
        trh = (tring_hdr *)dgstart;
        if (buf_flags & BF_MAC_PKT)
            llc1 = (sap_hdr *)(dgstart + TR_MAXENCSIZE);
        else
            llc1 = (sap_hdr *)(dgstart + receive.llc_offset);
 
        /* See if we can fast srb this packet */
        if ((buf_flags
             & (BF_BRIDGE|BF_MYMULTI|BF_MYMAC|BF_EXPLORER|BF_MAC_PKT))
            == BF_BRIDGE) {
            if (tokenring_debug) {
                int rif_len;
 
                if (trh->saddr[0] & TR_RII)    /* have a rif get its length */
                    rif_len = trh->data[0] & RC_LTHMASK;
                else
                    rif_len = 0;
                llc1 = (sap_hdr *) (&trh->data[rif_len]);
                tr_print_packet(idb,"srb/fs:", trh, (ulong *) llc1,
                    buf_flags, size);
            }
            if ((*idb->tr_srb_fastswitch)(idb, &receive,idb->tr_bridge_idb)) {
                ds->active_counts.rx_sent++;
                ds->active_counts.rx_bytes_sent += size;
                TMS380_RX_NEXT_PACKET
                continue;
            }
        }
 
        snap = (snap_hdr *)(dgstart + receive.llc_offset);
        snaptype = snap->type;

        /*
         * Flush explorers ASAP if the router is congested.
         */
        if (((buf_flags & (BF_MYMULTI|BF_MYMAC|BF_EXPLORER)) == BF_EXPLORER)
        || (multicast_to_srb_common = tms380_multicast_srb_explorer(buf_flags, 
                                          idb->firstsw, packet_type, llc1))) {
	    if (registry_populated(REG_SRT)) {
                if (reg_invoke_srt_flush_explorer(idb, (ushort)size)) {
                    datagram_done(pak);
                    TMS380_RX_NEXT_PACKET
                    continue;
                }
            }
        }
 
	/*
	 * Added this because srb now checks the rxtype to prevent bridging 
	 * any protocol which we are routing.
	 */
	pak->rxtype  = packet_type;

        /*
         * Look for RSRB frames.  Remember to reference the data in short
         * words since it is still on the adapter card at this point.
         */
        if (snap->dsap == SAP_SNAP) { 
	   if (snaptype == TYPE_RSRB) {
               receive.data = snap->data;
               if (reg_invoke_tms380_vring_input(idb, &receive)) {
                   ds->active_counts.rx_sent++;
                   ds->active_counts.rx_bytes_sent += size;
                   TMS380_RX_NEXT_PACKET
                   continue;
               }
            }
        }

        /*
         * Handle packets classified for transparent bridging
         * Keep physical broadcasts in the tbridge fast path.
         * Any that are routeable will be returned for reception.
	 */
	if ( (buf_flags & BF_TBRIDGE ||
           ((ds->tbridge_enabled) && (is_ieee_bcast(trh->daddr)))) &&
	   !(buf_flags & BF_EXPLORER) ) {
            if (tokenring_debug)
                 tr_print_packet(idb,"tbr:", trh, (ulong *) llc1,
                        buf_flags, size);
 
            pak->riflen = 0;
            pak->encsize = receive.data_offset;
            pak->network_start = pak->datagramstart + receive.data_offset;
            pak->datagramsize = size - TR_FCS_SIZE; /* remove FCS */
 
            token_decode_encapsulation_inline(pak, packet_type,
                              (tring_hdr *)pak->datagramstart);

            if (reg_invoke_tbridge_forward(pak)) {
                ds->active_counts.rx_sent++;
                ds->active_counts.rx_bytes_sent += size;
                TMS380_RX_NEXT_PACKET
                continue;
            }
            pak->datagramsize = size;
        }

	/* 
         * take the private pool buffer and clip it into a (possibly) 
         * smaller public pool buffer and send the clipped buffer to 
	 * process level, so that private pool buffers are not held   
	 * outstanding for long periods of time                       
         */

	newpak = pak_clip(pak, pak->datagramsize);

	if (newpak) {
	    /*
	     * If the input interface has been overwritten to the BVI
	     * (Bridge-group Virtual Inteface), copy the real received input
	     * interface as well.
	     * For IRB(Integrated Routing and Bridging)
	     */
	    if (is_bvi(hwidb_or_null(pak->if_input)) && pak->irb_rcvd_input)
		newpak->irb_rcvd_input = pak->irb_rcvd_input;

	    retbuffer(pak);
	} else {
	    retbuffer(pak);
	    idb->counters.input_drops++;
	    tms380_disable(idb);
	    TMS380_RX_NEXT_PACKET
	    continue;
	}

        /*
         * Bridge SRB packets the slow way, ie, packet was copied
         * into a system memory buffer before forwarding.
         */
        if ((buf_flags & (BF_BRIDGE | BF_MYMULTI | BF_MYMAC)) == BF_BRIDGE) {
            if (tms380_srb_common(idb, trh, newpak, buf_flags,
                      riflen, receive.rd_offset, llc1)) {
                ds->active_counts.rx_sent++;
                ds->active_counts.rx_bytes_sent += size;
                TMS380_RX_NEXT_PACKET
                continue;
            }
        }
 
        /*
         * Bridge selected Multicast Explorers as common SRB packets.
         * These include illegal linktypes that are bridged by default.
         * This is to prevent storms of these packets from filling up the
         * process level queues and causing interface throttling.
         * See multicast_explorer_can_be_bridged().
         */
        if (multicast_to_srb_common) {
            if (tms380_srb_common(idb, trh, newpak, buf_flags,
                      riflen, receive.rd_offset, llc1)) {
                ds->active_counts.rx_sent++;
                ds->active_counts.rx_bytes_sent += size;
                TMS380_RX_NEXT_PACKET
                continue;
            }
        }

        /*
         * Process a locally destined frame.  Madge always provides the
         * FCS, so we must knock off that number of bytes from the size.
         */
 
        size -= TR_FCS_SIZE;
        newpak->datagramsize -= TR_FCS_SIZE;
        tms380_local_common(idb, trh, newpak, buf_flags,
                    riflen, receive.rd_offset, llc1);
        ds->active_counts.rx_sent++;
        ds->active_counts.rx_bytes_sent += size;
        TMS380_RX_NEXT_PACKET
    }
 
    if (sched_run_interval && AWAKE(sched_run_time))
        tms380_disable(idb);
}

/************************************************************************
 *
 * tms380_sif_handler: handle a TI chip set interrupt
 *
 * This routine is responsible for dealing with all of the possible
 * interrupt conditions coming from the sif chip.  The Madge code has
 * single bit indicators for what has happened.  The lack of any
 * bits indicates a transmit or receive interrupt.
 *
 * Adapter Check: (bit 3) Death.  A h/w or s/w error in the chip set
 * 	was detected.  Shut the interface down, rummage around to see
 * 	if we can detect why it happened and punt generally.  Mother
 * 	will have to shoot us in the head to get us going again.  Note
 * 	that we keep the Mother side running so she can find out what's
 * 	going on.
 *
 * SSB_Response: (bit 2) An asynchronous command has completed.  THe
 * 	FASTMAC code does not support asynchronous completion of
 * 	commands so this bit should never be set.
 *
 * SRB_Free: (bit 1) The current SRB command has completed.  The response
 * 	block in the command header must be checked to see if the
 * 	command completed successfully, will complete later (not
 * 	supported), or failed miserably.
 *
 * ARB_Command: (bit 0) The Madge code has sent us an asynchronous
 * 	qcommand.  The only command currently supported is a Ring
 * 	Status command.
 *
 * Rx Pending: (no bits) New receive packets are available.
 *
 * Note that we don't have to dork with the interrupt level, since the
 * interrupt hardware on the 68030 disables network interrupts when we're
 * in the interrupt routine.
 *
 ************************************************************************/

void tms380_sif_handler (hwidbtype *idb,
			 volatile ushort *sifint_p,
			 ushort intval)
{
    tms380_instance_t *ds;
    struct _sifreg_t *sif_regs;
    rx_slot_ptr *rx_slot;
    int i = 0;

    ds = INSTANCE;
    sif_regs = SIF_REGS;
    WRITE_SIF(sifint_p, CLEAR_INTERRUPT);
    
    if (intval & MADGE_CHECK) {
        tms380_adapter_check(idb, intval, 3);
        return;
    }

    if ((intval & MADGE_ARB_MASK) == MADGE_ARB_REQ) 
        arb_req(idb, ds, sif_regs, sifint_p, intval);

    rx_slot = &ds->rx_p[ds->tms380_rx_slot];
 
    /*
     * Grab the pointers we need to do processing.
     */
    sif_regs->adrx = ADDR_CHPT1;
    sif_regs->adr  = rx_slot->fm_rx_space;
   
    if (sif_regs->data) {
        if (!idb_is_throttled(idb))
            tms380_madge_input(idb, rx_slot);
        else {
            do {
                /*
                 * Mark the slot as read by putting a zero in the length field.
                 */
                sif_regs->data = 0;
                sif_regs->adr  = rx_slot->fm_rx_space;
                ds->tms380_rx_slot++;
                if (ds->tms380_rx_slot == ds->rx_size)
			ds->tms380_rx_slot = 0;
                rx_slot = &ds->rx_p[ds->tms380_rx_slot];
                i++;
		if (i > ds->rx_size)
		    break;
            } while (sif_regs->data);
        }
    }
 
    /* Free the buffers on the transmit ring */
 
    tms380_tx_free(idb);
    
    (*idb->soutput)(idb);
}


/*
 * tms380_fastsend
 *
 * fast send this packet out a tms380 interface.
 * if there is no more room on the transmit frame
 * this packet will get queued onto the holdq.
 */
static void tms380_fastsend (hwidbtype *idb,
			     paktype *pak)
{
    traffic_shape_t *shape;
    int rval;
    ulong acct_proto;
    
    if (idb->state != IDBS_UP) {
	idb->counters.output_total_drops++;
	datagram_done(pak);
	return;
    }

    /*
     * determine whether there is window available to send the message with
     * in the rate control bucket
     */
    shape = traffic_shape_required_inline(pak->if_output->traffic_shape, pak);
    if (shape) {
	traffic_shape_defer_sending(shape, pak);
	return;
    }

    /*
     * If there are already packets on the output queue,
     * or there is no room in the output ring,
     * add this packet to the end of the queue to minimize
     * out of order situations:
     */
    if ( idb->output_qcount || 
	((rval = tms380_tx_pak(idb, pak)) == TXRV_QUEUE_IT)) {
	holdq_enqueue_fast(idb, pak);
    } else if (rval == TXRV_SENT) {
	acct_proto = (ulong)pak->acct_proto;
	idb->counters.tx_cumbytes_fast[acct_proto] += pak->datagramsize;
	idb->counters.outputs_fast[acct_proto]++;	
    }

    tms380_tx_free(idb);
}


/*
 * tms380_madge_output
 * Start or restart output on an tr interface.
 */
static void tms380_madge_output (hwidbtype *idb)
{
    tms380_instance_t *ds = INSTANCE;
    paktype *pak;		/* Current packet being output */
    int rval;
    leveltype level;
    /* Keep filling tx ring as long as: 
     * - we have more packets to transmit, 
     * - the interface is up, 
     * - and we don't try and overfill the tx buffer.
     */

    level = raise_interrupt_level(NETS_DISABLE);

    /* NOTE:  The check for idb->remif is here to support the SGI router
     * token ring tunneling.
     */
    while ((((ds->if_state == IF_UP) && (idb->state == IDBS_UP))
	    || (idb->remif != NULL)) && (idb->output_qcount > 0)
            && ((pak = (*idb->oqueue_dequeue)(idb)) != NULL) ) {
	    if ( (rval = tms380_tx_pak (idb, pak)) == TXRV_QUEUE_IT ) {
		pak_requeue_head(idb, pak);
		DBG2(DBG_PAK, "tr(%d): pak %x -> tx holdq\n", 
		     idb->unit, pak);
		break;
	    }
	    else if (rval == TXRV_SENT) {
		idb->counters.outputs++; /* count total output */
		idb->counters.tx_cumbytes += 
		    pak->datagramsize;	/* count total bytes sent */
	    }	    
    }

    tms380_tx_free(idb);
    
    reset_interrupt_level(level);
}	


/*
 * tms380_modify: modify certain selected control cells.
 *
 * The only cells that can be modified are defined in sca.h under the
 * ASYNC_MODIFY command.  MODIFY was implemented coincident with GET_CONFIG.
 * We check to see if this is an early implementation and only do our stuff
 * if the command exists.  If it doesn't we don't care anyway.
 */
static void tms380_modify (hwidbtype *idb)
{
    tms380_instance_t *ds;
    ulong error_code;
    leveltype level;
    idbtype *swidb = firstsw_or_null(idb);
    srb_triplet_t srb_triplet;

    ds = INSTANCE;
    
    /*
     * If we are initializing, we cannot execute this command yet.  The
     * system will automatically re-execute this command as soon as the
     * adapter is opened.
     */
    if (idb->state != IDBS_UP) {
	if (tokenevent_debug)
	    buginf("Not done initializing yet\n");
	return;
    }

    ds->max_rd = 8; /* magic number */

    if (!ds->bridge_enabled) {
	if (tokenevent_debug)
	    errmsg(&msgsym(MODEFAIL, TR), idb->unit, 
		   RSLT_MODIFY_BRIDGE_OFF, 0);
	return;
    }
    
    srbV.srb_sb_get_triplet(swidb, &srb_triplet);

    if (ds->max_rd < SRB_MIN_MAXRD ||
	ds->max_rd > SRB_MAX_MAXRD ||
	srb_triplet.bridgeNum  > SRB_MAX_BRIDGE_NUM ||
	srb_triplet.localRing  > SRB_MAX_RING_NUM ||
	srb_triplet.remoteRing > SRB_MAX_RING_NUM) {
	if (tokenevent_debug)
	    errmsg(&msgsym(MODEFAIL, TR), idb->unit, RSLT_MODIFY_BAD_DATA, 0);
	return;
    }
    
    level = raise_interrupt_level(NETS_DISABLE);
    
    if (!cfgbridge380(idb, TRUE, &error_code)) {
	errmsg(&msgsym(MODEFAIL, TR), idb->unit, RSLT_CFGBR_FAILED,
	       error_code);
	reset_interrupt_level(level);
	return;
    }
    ds->rng_br_rng_f = (    (srb_triplet.localRing  << 20) 
		          | (srb_triplet.bridgeNum  << 16) 
			  | (srb_triplet.remoteRing << 4) );
    ds->rng_br_rng_r = (    (srb_triplet.remoteRing << 20) 
			  | (srb_triplet.bridgeNum  << 16) 
			  | (srb_triplet.localRing  << 4) );
    ds->max_rif = ds->max_rd * 2 + 2;
    
    tms380_enable_host_interrupts(idb);

    reset_interrupt_level(level);
}

/*
 * tms380_set_addresses: set new functional and group addresses
 *
 * This routine instructs the adapter to use new functional and group
 * addresses, without having to reset the adapter and go through the ring
 * insertion process.
 */
static boolean tms380_set_addresses (hwidbtype *idb)
{
    tms380_instance_t *ds;
    ulong error_code;
    idbtype *swidb;
    leveltype level;
    tokensb *tsb;
    srb_vector_table_t *srbV_p;
    
    level = raise_interrupt_level(NETS_DISABLE);
	
    ds = INSTANCE;

    if ((idb->state != IDBS_UP) && (idb->state != IDBS_INIT)) {
	DBG1(DBG_MADGE,
	     "Unexpected idb->state=%#x\n", 
	     idb->state);
        reset_interrupt_level(level);
	return(FALSE);
    }
    
    /* If we are initializing, we cannot execute this command yet.  The
     * system will automatically re-execute this command as soon as the
     * adapter is opened.*/
    if (idb->state == IDBS_INIT) {
	DBG0(DBG_MADGE,
	     "Not done initializing yet\n");
        reset_interrupt_level(level);
	return(TRUE);
    }
    
    tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);

    /* Setup the ring mode.  If the ring is running at 4M, force the ETR
     * bit off because it is not defined for a 4M ring. */
    if (idb->tr_ring_speed == 4) {
	idb->tr_ring_mode &= ~RINGMODE_ETR;
    }
    /*
     * Reinitialize functional addresses for non-bridging stuff
     */
    tr_functional_init(idb);
    srbV_p = reg_invoke_srb_get_vector_table();
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
        ushort srb_bridging;
        if (srbV_p) {
            srb_bridging = srbV_p->srb_sb_get_bridging_enabled(swidb);
        } else {
            srb_bridging = FALSE;
        }
	if (srb_bridging || RUNNING_IF(LINK_BRIDGE,swidb)) {
	    tsb->tr_functional |= TRFUNC_BRIDGE;
        }
    }

    if (!set_address_380(idb, DIR_SET_FUNCTIONAL_ADDRESS,
			 tsb->tr_functional, &error_code)) {
	if (tokenevent_debug)
	    errmsg(&msgsym(SETFUNFAIL, TR), idb->unit, error_code);
	tms380_enable_host_interrupts(idb);
        reset_interrupt_level(level);
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	return(FALSE);
    }
    
    /* Set the group address */
    tsb->tr_group |= ((ulong) TR_FAI << 24);
    if (!set_address_380(idb, DIR_SET_GROUP_ADDRESS,
			 tsb->tr_group, &error_code)) {
	if (tokenevent_debug)
	    errmsg(&msgsym(SETGRPFAIL, TR), idb->unit, error_code);
	tms380_enable_host_interrupts(idb);
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
        reset_interrupt_level(level);
	return(FALSE);
    }

    /*
     * Set the mode options.
     */
    if (!chgmode380(idb, &error_code)) {
	if (tokenevent_debug)
	    errmsg(&msgsym(MODEFAIL, TR), idb->unit, RSLT_MODE_FAILED,
	       error_code);
        tms380_enable_host_interrupts(idb);
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
        reset_interrupt_level(level);
	return(FALSE);
    }

    tms380_enable_host_interrupts(idb);
    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
    reset_interrupt_level(level);
    return(TRUE);
}


/***** presentation layer *****/

void tms380_pnt_ptrs (hwidbtype *idb)
{
    tms380_instance_t *ds = INSTANCE;
    rx_slot_ptr       *rxptr;
    tx_slot_ptr       *txptr;
    int i;
    ushort level;
    
    /*
     * Grab the pointers we need to do processing.
     */
    level = raise_interrupt_level(NETS_DISABLE);

    buginf("\nrx:    host       ti380");
    for (i = 0,  rxptr = ds->rx_p; i < ds->rx_size ; i++, rxptr++) 
        buginf("\n       %0#8x       %0#8x    pak: %0#8x",
            rxptr, rxptr->fm_rx_space, rxptr->pak);
    buginf("\n");

    buginf("\ntx:    host       ti380");
    for (i = 0,  txptr = ds->tx_p; i < ds->tx_size ; i++, txptr++) 
        buginf("\n       %0#8x       %0#8x    pak: %0#8x",
            txptr, txptr->fm_tx_space, txptr->pak);
    buginf("\n");

    reset_interrupt_level(level);
}

/*
 * tms380_show_controller
 * support for show controller
 */
static boolean tms380_show_controller (hwidbtype *idb, parseinfo *csb)
{
    tms380_instance_t *ds;
    struct _sifreg_t *sif_regs;
    sbe_counters_t *counters;
    sbe_smt_stats_t *stats = NULL;
    madge_smt_errors_t *errors;
    boolean alive;
    uchar version_string[MAC_VERS_LEN+1];
    tokensb  *tsb;
    idbtype  *swidb = firstsw_or_null(idb);
    srb_triplet_t srb_triplet;
    leveltype level;
    
    ds = INSTANCE;
    if (!ds) {
	printf(not_initialized);
	return(FALSE);
    }

    tsb = idb_get_hwsb_inline(idb, HWIDB_SB_TOKEN);

    sif_regs = SIF_REGS;
    /*
     * Kick off a command to read the smt stats.  This should complete
     * before we get around to printing the numbers.
     */
    tms380_get_smt_stats(idb);
    
    printf("\nTMS380 unit %d: ", idb->unit);
    tms380_print_nim_id(idb);

    printf("%d Kb RAM, state %d, idb %#x, ds %#x",
           ds->memory_size/1024, idb->state, idb, (ulong)ds);

    printf("\n  current address: %e, burned in address: %e",
	   idb->hardware, idb->bia);


    printf("\n\n  ssb_ptr %#x, srb_ptr %#x, arb_ptr %#x, stb_ptr %#x, ipb_ptr %#x",
	   ds->ssb_ptr, ds->srb_ptr, ds->arb_ptr, ds->stb_ptr, ds->ipb_ptr);
    printf("\n  bia_addr %#x, swlev_addr %#x, address_addr %#x, parm_addr %#x",
	   ds->bia_addr, ds->swlev_addr, ds->address_addr, ds->parm_addr);
    printf("\n  mac_buff %#x, ti_ring_speed_ptr %#x, adapter_ram_ptr %#x",
	   ds->mac_buff, ds->ti_ring_speed_ptr, ds->adapter_ram_ptr);
    printf("\n  adapter_ram %d, ti_ring_speed %d, memory paragraphs %d",
	   ds->adapter_ram, ds->ti_ring_speed, ds->paragraphs);
    printf("\n  sifsts 0x%04x, sifacl 0x%04x, sifadr 0x%04x, sifadrx 0x%04x",
	   sif_regs->intreg, sif_regs->actl, sif_regs->adr, sif_regs->adrx);

    if (idb->state == IDBS_UP) { 
        sif_regs->adr  = ds->stb_ptr + OFFSET_RX_INTERNAL_BUF_SIZ;
        printf("\n  rx internal buf size %d, rx total buffers avail %d,",
	   sif_regs->datainc, sif_regs->datainc);
        printf("\n  rx buffers in use %d, rx frames lost %d,",
	   sif_regs->datainc, sif_regs->datainc);
    } else {
        printf("\n  rx internal buf size 0, rx total buffers avail 0,");
        printf("\n  rx buffers in use 0, rx frames lost 0,");
    }
 
    print_ring_status(idb);

    /*
     * Print out monitor state.  This includes both static state which
     * is gotten on the way up and dynamic state which is gotten from a
     * running controller.  If the controller isn't running the dynamic
     * information we would have gotten is not interesting.  This structure
     * lends itself to the get_status (dynamic) and get_config (static)
     * scheme of things but alas get_config wasn't built early enough
     * so for backward compatitbility reasons the static stuff that is
     * already on the status block will stay there.
     *
     * note that 'get statistics' should not bzero any counters here
     * hence I substitute tms380_get_stats_copy() for tms380_get_stats()
     */
    stats = &ds->frozen_stats;
    counters = &ds->frozen_counts;

    alive = tms380_get_stats_copy(idb);
    printf("\n  Monitor state: (%sactive)",(alive ? "" : "not "));
    printf("\n    flags %#x, code %#x, reason %#x\n",
           ds->brd_flags, ds->sca_fail, ds->sca_fail_code);

    bcopy(ds->frozen_stats.ti380_version, version_string, MAC_VERS_LEN);
    version_string[MAC_VERS_LEN] = '\0';
    sys_ebcdic_to_ascii(version_string, MAC_VERS_LEN);
    printf("    chip f/w: %s, [bridge capable]", version_string);
    printf("\n    SMT versions: %d.%02d kernel, %d.%02d fastmac",
           ds->madge_kernel_version/256, ds->madge_kernel_version&0xff, 
           ds->madge_fastmac_version/256, ds->madge_fastmac_version&0xff);
    printf("\n    ring mode: %x", idb->tr_ring_mode);
    if (tsb->tr_functional) {
        printf(", internal enables: ");
        if (tsb->tr_functional & TRFUNC_BRIDGE)
	    printf(" SRB");
        if (tsb->tr_functional & TRFUNC_REM)
	    printf(" REM");
        if (tsb->tr_functional & TRFUNC_RPS)
	    printf(" RPS");
        if (tsb->tr_functional & TRFUNC_CRS)
	    printf(" CRS/NetMgr");
    }
    printf("\n    internal functional: %08x, group: %08x",
           tsb->tr_functional, tsb->tr_group & ~0x80000000);
    printf("\n    if_state: %d, ints: %d/%d, ghosts: %d/%d, bad_states: %d/%d",
           ds->if_state, counters->sca_ints, ds->prev_counts.sca_ints,
           counters->sca_ghosts, ds->prev_counts.sca_ghosts,
           counters->bad_state, ds->prev_counts.bad_state);	   

    srbV.srb_sb_get_triplet(swidb, &srb_triplet);
    printf("\n    ring: %d, bridge num: %d, target: %d, max hops: %d",
           srb_triplet.localRing,
           srb_triplet.bridgeNum,
           srb_triplet.remoteRing,
           srbV.srb_sb_get_max_hops(swidb));
    printf("\n    last open options: (%08x)\n",ds->last_open_options);

    printf("\n\n  error log reads %d, error log failures %d",
           ds->error_log_counter, ds->error_log_failures);
    printf("\n  too big packets %d, full tx buffer errors %d",
           ds->send_toobig, ds->tx_buffer_full);
    printf("\n  receive delimiter error %d, receive implicit errors %d",
           ds->rx_delimiter_error, ds->rx_imp_error);
    printf("\n  receive explicit error %d, receive dma overrun %d",
           ds->rx_exp_error, ds->rx_dma_overrun);
    printf("\n  receive buffer runout %d",ds->rx_buf_runout);

    printf("\n    Packet counts:");
    printf("\n      receive total:  %d/%d, small: %d/%d, large %d/%d",
	   counters->rx_sent, ds->prev_counts.rx_sent,
	   counters->rx_small,ds->prev_counts.rx_small,
	   counters->rx_large,ds->prev_counts.rx_large);
    printf("\n              runts: %d/%d, giants: %d/%d",
	   counters->rx_runts,    ds->prev_counts.rx_runts,
	   counters->rx_giants,   ds->prev_counts.rx_giants);
    printf("\n              local: %d/%d, bridged: %d/%d, promis: %d/%d",
	   counters->rx_local, ds->prev_counts.rx_local,
	   counters->rx_bridge,ds->prev_counts.rx_bridge,
	   counters->rx_promiscuous, ds->prev_counts.rx_promiscuous);
    printf("\n            bad rif: %d/%d, multiframe: %d/%d",
	   counters->rx_rif_toolarge, ds->prev_counts.rx_rif_toolarge,
	   counters->rx_multiframe,ds->prev_counts.rx_multiframe);
    printf("\n      ring num mismatch %d/%d, spanning violations %d",
	   counters->rx_srb_rnum_mismatch,
	   ds->prev_counts.rx_srb_rnum_mismatch,
	   ds->spanning_violations);
    
    printf("\n      transmit total: %d/%d, small: %d/%d, large %d/%d",
	   counters->tx_sent,  ds->prev_counts.tx_sent,
	   counters->tx_small, ds->prev_counts.tx_small,
	   counters->tx_large, ds->prev_counts.tx_large);
    printf("\n               runts: %d/%d, giants: %d/%d, errors %d/%d",
	   counters->tx_toosmall,  ds->prev_counts.tx_toosmall,
	   counters->tx_toolarge,  ds->prev_counts.tx_toolarge,
	   counters->tx_siferr,    ds->prev_counts.tx_siferr);
    printf("\n              bad fs: %d/%d, bad ac: %d/%d",
	   counters->tx_bad_fs,  ds->prev_counts.tx_bad_fs,
	   counters->bad_acbits, ds->prev_counts.bad_acbits);
    printf("\n           congested: %d/%d, not present: %d/%d",
	   counters->tx_congested,  ds->prev_counts.tx_congested,
	   counters->tx_nobodyhome, ds->prev_counts.tx_nobodyhome);
    printf("\n    Unexpected interrupts: %d/%d,  last unexp. int: %d\n",
	   counters->bozo_ints, ds->prev_counts.bozo_ints,
	   counters->last_bozo);
    
    errors = &ds->active_errors;
    printf("\n    Internal controller counts:");
    printf("\n      line errors: %d/%d,  internal errors: %d/%d",
	   errors->err_line, tsb->prev_err_line,
	   errors->err_internal, tsb->prev_err_internal);
    printf("\n      burst errors: %d/%d,  ari/fci errors: %d/%d",
	   errors->err_burst, tsb->prev_err_burst,
	   errors->err_arifci, tsb->prev_err_arifci);
    printf("\n      abort errors: %d/%d, lost frame: %d/%d",
	   errors->err_abort, tsb->prev_err_abort,
	   errors->err_lostframe, tsb->prev_err_lostframe);
    printf("\n      copy errors: %d/%d, rcvr congestion: %d/%d",
	   errors->err_copy, tsb->prev_err_copy,
	   errors->err_rcvcongestion, tsb->prev_err_rcvcongestion);
    printf("\n      token errors: %d/%d, frequency errors: %d/%d",
	   errors->err_token, tsb->prev_err_token,
	   errors->err_freq, tsb->prev_err_freq);
    printf("\n      dma bus errors: -/-, dma parity errors: -/-");
    printf("\n      adapter checks %d, info from last adapter check: ",
	   ds->adapter_checks);
    printf("\n      status %#x, parm0 %#x, parm1 %#x, parm2 %#x, intval %#x",
	   ds->adapter_check_status, ds->parm0,
	   ds->parm1, ds->parm2, ds->intval);

    /*
     * Now that active_errors have been displayed so they can be reset
     */
    level = raise_interrupt_level(ETHER_DISABLE);
    memset(&ds->active_counts, 0, sizeof(sbe_counters_t));
    memset(&ds->active_errors, 0, sizeof(madge_smt_errors_t));
    reset_interrupt_level(level);

    printf("\n    Internal controller smt state:"
	   "\n      Adapter MAC:     %e, Physical drop:     %08x"
	   "\n      NAUN Address:    %e, NAUN drop:         %08x"
	   "\n      Last source:     %e, Last poll:         %e",
	   stats->mac, GETLONG(&stats->physdrop),
	   stats->una, stats->una_physdrop,
	   stats->last_src, stats->last_poll);

    printf("\n      Last MVID:       %04x,           Last attn code:    %04x"
	   "\n      Txmit priority:  %04x,           Auth Class:        %04x"
	   "\n      Monitor Error:   %04x,           Interface Errors:  %04x",
	   stats->last_major_vector, stats->last_attn, stats->txmt_pri,
	   stats->auth_class, stats->monitor_error_code,
	   stats->ring_xfc_errors);
    printf("\n      Correlator:      %04x,           Soft Error Timer:  %04x"
	   "\n      Local Ring:      %04x,           Ring Status:       %04x"
	   "\n      Beacon rcv type: %04x,           Beacon txmit type: %04x",
	   stats->frame_corr, stats->soft_err_timer, stats->local_ring,
	   stats->ring_status, stats->beacon_rcv_type,
	   stats->beacon_txmt_type);
    printf("\n      Beacon type:     %04x,           Beacon NAUN:       %e"
	   "\n      Beacon drop:     %04x,           Reserved:          %04x"
	   "\n      Reserved2:       %04x\n", stats->last_beacon_type,
	   stats->beacon_una, GETLONG(&stats->beacon_una_physdrop),
	   *(ushort *)stats->rsrvd, *(ushort *)stats->rsrvd2);

    return automore_quit();
}


void tms380_possibly_display_ringstat (hwidbtype *idb,
				       uint ring_stat)
{
    tms380_instance_t *ds;
    boolean display;
    tokensb *tsb;
    
    ds = INSTANCE;
    tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);
    
    /*
     * Do what sbe16_t2m_ringstatus does here:
     */
    display = FALSE;
    if (ring_stat & RNG_WIRE_FAULT) {
	if (!(tsb->tr_ring_status & RNG_WIRE_FAULT))
	    display = TRUE;
	else if (AWAKE(tsb->tr_next_buginf_time))
	    display = TRUE;
    }
    if (tr_ringstat_common(idb, ring_stat, display))
	TIMER_START(tsb->tr_next_buginf_time, TR_BUGINF_INTERVAL);

    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
}


/*
 * tms380_txqlength
 *
 * Returns the number of frames in the TX ring
 */
static int tms380_txqlength (hwidbtype *idb)
{
    tms380_instance_t *ds = INSTANCE;

    return(ds->tx_pak_count);
}


/***** initialization *****/

/*
 * Initialize a tokenring interface
 */
boolean tms380_initialize (hwidbtype *idb)
{
    tms380_instance_t *ds;
    struct _sifreg_t *sif_regs;
    boolean bridging;
    leveltype level;
    idbtype  *swidb = firstsw_or_null(idb);
    tokensb  *tsb;
    boolean srb_bridging;
    srb_vector_table_t *srbV_p;
    
    ds = INSTANCE;
    sif_regs = SIF_REGS;
    tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);

    /* clear last time tr int was up */
    TIMER_STOP(tsb->tr_lineuptimer);

    /* 
     * Copy in the hw_address no matter what state we are in
     */	
    tms380_get_default_hw_address (idb, idb->bia); /* save default */
    ieee_copy(idb->bia, ds->bia);
    ieee_copy(idb->hardware, ds->node_addr);
    if (is_ieee_zero(ds->node_addr)) {
	tms380_get_default_hw_address (idb, ds->node_addr);
	ieee_copy(ds->node_addr, idb->hardware);
    }
    
    /*
     * If we are administratively shut down, shut down the
     */
    if (idb->state == IDBS_ADMINDOWN) {
	END_TIME_380("tms380_initialize");
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	return(TRUE);
    }
    
    idb->counters.resets++;		/* Count the resets */
    
    /*
     * If we are already initializing, return:
     */
    if (idb->state == IDBS_INIT) {
	END_TIME_380("tms380_initialize");
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	return(TRUE);
    }
    
    /*
     * Return if we have not yet parsed a ring speed command for this
     * interface. This is so that we don't mess up a tokenring
     * by inserting onto it with the wrong ring speed.
     */
    if ((idb->tr_ring_speed != 4) && (idb->tr_ring_speed != 16)) {
	if (tokenevent_debug)
	    buginf("\nTR%d: ring-speed has not been set", idb->unit);
	net_cstate(idb,IDBS_RESET);
	ds->if_state = IF_DOWN;
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	return(TRUE);
    }
    
    level = raise_interrupt_level(NETS_DISABLE);
    net_cstate(idb, IDBS_INIT);
    ds->if_state = IF_DOWN;
    
    idb->counters.transitions++;

    /*
     * Leave the idb->state set to IDBS_INIT.
     * We initialize the hardware address for the TMS380. Use the cookie
     * specified address if idb->hardware is NULL:
     */
    if (is_ieee_zero(ds->node_addr)) {
	tms380_get_default_hw_address (idb, ds->node_addr);
	if (is_ieee_bcast(ds->node_addr)) {
	    ds->if_state = IF_DOWN;
	    net_cstate(idb, IDBS_DOWN);
	    if (tokenevent_debug)
		errmsg(&msgsym(BADMUL, TR), idb->unit, ds->node_addr);
	    reset_interrupt_level(level);
	    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	    return(FALSE);
	}
	ieee_copy(ds->node_addr, idb->hardware);
    }
    
    /*
     * Validate the hardware address.  If it is invalid, use the
     * universally administered address for the interface.
     */
    if (idb->hardware[0] & TR_GROUP) {
	/*
	 * Some protocols like to change the MAC addresses of all the
	 * interfaces.  Unfortunately, the bit indicating a multicast is
	 * different on token ring than on ethernet, so setting a token ring
	 * and an ethernet to the same address can be dangerous.  Here, we
	 * make sure that we don't set the address to a multicast, printing a
	 * nasty message (which includes the process running) instead.  There
	 * is a potential problem with people manually setting a funny XNS or
	 * Novell adddress.
	 */
	if (tokenevent_debug)
	    errmsg(&msgsym(BADMUL, TR), idb->unit, idb->hardware);
	*((ulong *) ds->node_addr) = 0L;
	*((ushort *) ds->node_addr+4) = 0;
    }
   
    /*
     * Clean up any remaining buffers in the transmission ring before reseting
     * the pointers
     */
    if (!tms380_hard_reset(idb)) {
	if (idb->state != IDBS_ADMINDOWN) {
	    net_cstate(idb, IDBS_DOWN);
 	    if (tokenevent_debug)
		errmsg(&msgsym(INITFAIL, TR), idb->unit, idb->state);
	    ds->if_state = IF_DOWN;
	}
	reset_interrupt_level(level);
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	return(FALSE);
    }
    
    ds->tx_pak_count_reset = ds->tx_pak_count; /* remember for bugs */
    ds->tx_pak_count = 0;
    /*
     * Set the buffer size and pointers.
     */

    ds->buffersize = idb->max_buffer_size;

    /*
     * Check for bridging so that we can do the appropriate start:
     */
    
    bridging = FALSE;
    srb_bridging = FALSE;
    srbV_p = reg_invoke_srb_get_vector_table();
    if (srbV_p) {
        if (srbV_p->srb_sb_get_bridging_enabled(swidb)) {
            srb_bridging = TRUE;
        }
    }

    if (srb_bridging) {
        /*
         * Continue if it is not a part of the virtual
         * bridge of the local lnm feature.
         */
        if (!reg_invoke_local_lnm_vr_bridge(idb)) {

	    /*
	     * if we have the capability to bridge, whether
	     * SRB or transparent bridging, set up bridging
	     * for us.
    	     */
	    ds->max_rd = 8; /* magic number */
	    if (bridge_enable) {
	        tms380_set_bridge_filters(idb);
	        bridging = TRUE;
	    }
        } else {
	    ds->max_rd = 0;
        }
    } else {
        ds->max_rd = 0;
    }

    tms380_tbridge_init(idb, bridging);  /* set up bridging */

    /*
     * start the tms380.  Note that this is historically
     * a rather convoluted and involuted processes,
     * gradually getting clearer.  Needs to be ripped out
     * with a wooden spoon, tossed out, and rewritten.
     */
    if (!tms380_start(idb, bridging)) {
	/* start failed */
	if (idb->state != IDBS_ADMINDOWN) {
	    ds->if_state = IF_DOWN;
	    net_cstate(idb, IDBS_DOWN);
	    if (tokenevent_debug)
		errmsg(&msgsym(BADSTART, TR), idb->unit, idb->state);
	}
	reset_interrupt_level(level);
	tms380_enable_host_interrupts(idb);
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	return(FALSE);
    }

    /*
     * Start was successfull:
     * We rely on keep alives to reset the interface periodically if it
     * is wedged.  All Token Ring interfaces can get into a state where
     * they will continuously fail (for example, if the lobe cable is
     * disconnected at the DB-9 connector).  If we don't reset the lineup
     * and lineuptimer cells, then the tr_periodic routine won't detect
     * that something is wrong and reset us.  This would mean that in the
     * example, when the user finally notices that the cable is off and
     * reconnects it, the router won't restart the ring.  By bumping
     * these keepalive cells here we guarentee that the box will
     * periodically attempt to reset the interface if it isn't behaving.
     * This can be disabled by turning off keepalives or shutting down
     * the interface.
     */
    tsb->tr_lineup = TR_KEEP_COUNT;
    GET_TIMESTAMP(tsb->tr_lineuptimer);
    ds->if_state = IF_UP;
    net_cstate(idb, IDBS_UP);
    
    /*
     * Get the hardware address the interface board is using, and mark
     * the interface up.  Then initialize lineup and lineuptimer.
     */
    ieee_copy(ds->node_addr, idb->hardware);
    
    trebuginf("\nTR%d: Interface is alive, phys. addr %e",
	      idb->unit, idb->hardware);
    
    /*
     * The silly slow Token Ring interface has now been able to set its
     * hardware address.  The XNS and Novell protocols rely on this for
     * their node addresses.
     */
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	reg_invoke_media_hwaddr_notify(swidb, idb->hardware, IEEEBYTES);
    }
    rif_update(idb, 0, idb->hardware, NULL, NULL, RIF_INTERFACE);
    ds->brd_flags = FLAG_PRESENT | FLAG_OPERATE;
    tsb->tr_ring_status = 0;
    GET_TIMESTAMP(tsb->tr_status_time);
    idb->counters.transitions++;
    GET_TIMESTAMP(tsb->tr_next_buginf_time);	/* enable future ones */
    
    tms380_get_static_stats(idb);
    tms380_get_smt_stats(idb);
    
    tms380_set_addresses(idb);
    
    if (srb_bridging) {
	tms380_modify(idb);
    }
    
    reset_interrupt_level(level);
    
    (*idb->soutput)(idb);	/* kick start if necessary */
    tms380_enable_host_interrupts(idb);
    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
    return(TRUE);
}


/*
 * init_tms380_idb_common
 *
 * Set up the idb for a TMS380 token ring interface.
 * Note that malloc return zero'd memory, so we only initilize those
 * entries that have non-zero values.
 */
boolean init_tms380_idb_common (hwidbtype *idb)
{
    tms380_instance_t *ds;
    pooltype *tms380_buffer_pool = NULL;
    ulong idblist_type;
    
    tring_idb_init(idb, idb->tr_ring_speed);

    idb->status = (IDB_TR|IDB_HEARSELF|IDB_BCAST|IDB_BRIDGE);
    idb->type = IDBTYPE_TMS380;
    idb->typestring = "TMS380";
    idb->state = IDBS_RESET;
    idb->reason = "first initialization";
    idb->set_maxdgram = tms380_set_maxdgram; 
    if_maxdgram_adjust(idb, TMS380_MTUTOBUFSIZ + TMS380_MTU4);
    idb->maxmtu_def = TMS380_MTU4;
    idb->firstsw->sub_mtu = idb->maxmtu_def;

    idb->maxmtu = TMS380_MTU4;
    idb->pool = NULL;
    idb->pool_group = POOL_GROUP_INVALID;
    idb->fast_switch_flag = 0;
    idb->firstsw->tbridge_media = TBR_TR;

    /*
     * Set up function assignments that are unique to this driver
     */
    idb->soutput = tms380_madge_output;
    idb->reset = tms380_restart;
    idb->system_configured = tms380_restart;
    idb->shutdown = tms380_shutdown;
    idb->enable = tms380_enable;
    idb->setup_bridging = tms380_set_bridge_filters;
    idb->device_periodic = tms380_periodic;
    idb->set_extra_addrs = tms380_set_addresses;

    idb->show_controller = tms380_show_controller;
    idb->fastsend = tms380_fastsend;
    idb->txqlength = tms380_txqlength;
    
    idb->dot5_get_smt_stats = tms380_get_smt_stats;
    idb->dot5_add_counters = tms380_add_counters;
    idb->dot5_soft_reset = (void *)tms380_soft_reset;
   
    reg_add_setup_srb(idb->type, tms380_restart, "tms380_restart");
    reg_add_modify_srb(idb->type, (srbcore_modify_srb_t *) tms380_modify, 
						 	  "tms380_modify");

    idb->srb_routecache = TRUE;

    idb->fs_expl_allQ  =  (fs_expl_que *) NULL;
    idb->fs_expl_spanQ =  (fs_expl_que *) NULL;
    idb->fs_expl_input =  (fs_expl_que *) NULL;

    /*
     * Allocate this instance of the driver:
     */
    ds = malloc_named_fast(sizeof(tms380_instance_t), "TMS380 Instance");
    if (!ds) {
	crashdump(0);
    }
    INSTANCE = ds;
    ds->reset_tms380_on_fail = TRUE; /* Hard reset next time we fail */
 
    /*
     * Allocate and initilize the 380's system command and status blocks
     */
    ds->scb380 = malloc_named_iomem(sizeof(scbssb_t), "TMS380 Cmd/Stat");
    if (!ds->scb380) {
	crashdump(0);
    }
    
    /* set up 380 dev registers, interrupt wrappers */
    init_tms380_dev(idb);
    
    tring_idb_enqueue(idb);
    idb_enqueue(idb);

    /*
     * Check for TMS380 hwidb list if not there, create it.
     */
    if (idb_get_listhead(HWIDBLIST_TMS380) == NULL) {
	if (!idb_create_list(HWIDBLIST_TMS380, &idblist_type) ||
	    idblist_type != HWIDBLIST_TMS380) {
	    if (tokenring_debug) {
		idb_report_create_error("TMS380");
	    }
	    return(FALSE);
	}
    }

    /*
     * Add the idb to the TMS380 idb list
     */
    if (!idb_add_hwidb_to_list(idb, HWIDBLIST_TMS380)) {
	if (tokenring_debug) {
		idb_report_add_error("TMS380");
	}
    }    

    ds->tx_pak_count = 0;          /* number of paks in tx ring */
    ds->tx_pak_count_reset = 0;    /* number of paks in tx ring at reset */

    ds->buffersize = idb->max_buffer_size;
    ds->rx_size = make_ring_size(TMS380_MAX_RX_SLOTS);

    if (!idb->hw_namestring)
        idb_init_names(idb->firstsw, FALSE);

    idb->pool_group = pool_create_group();

    tms380_buffer_pool = pak_pool_create(idb->hw_namestring,
					 idb->pool_group,
					 idb->max_buffer_size, POOL_SANITY,
					 NULL);
    if (!tms380_buffer_pool) {
        crashdump(0);
    }
    idb->pool = tms380_buffer_pool;
    idb->pool->fallback = pak_pool_find_by_size(idb->max_buffer_size);
    pool_adjust(idb->pool, 0, TMS380_NUM_BUF(ds->rx_size),
		TMS380_NUM_BUF(ds->rx_size), TRUE);
    pak_pool_create_cache(idb->pool, TMS380_NUM_CACHE(ds->rx_size));
    pool_adjust_cache(idb->pool, TMS380_NUM_CACHE(ds->rx_size));
    return(TRUE);
}
