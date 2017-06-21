/* $Id: novinput.c,v 3.14.4.42 1996/09/15 05:48:39 john Exp $
 * $Source: /release/112/cvs/Xsys/xns/novinput.c,v $
 *------------------------------------------------------------------
 * novinput.c -- Handle Novell Input and Switching
 *
 * June 1989, Bill Westfield.  Largely a copy of xnsinput.c
 *
 *     FOR GENERAL DOCUMENTATION OF FLOW, SEE THE BIG COMMENT SECTION
 *     AT THE BEGINNING OF novell.c!
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: novinput.c,v $
 * Revision 3.14.4.42  1996/09/15  05:48:39  john
 * CSCdi69212:  Filtered IPX NetBIOS packets are not freed
 * Branch: California_branch
 *
 * Revision 3.14.4.41  1996/09/14  20:21:30  john
 * CSCdi69062:  Tracebacks in nov_spx_spoof slows down the router
 * Branch: California_branch
 *
 * Revision 3.14.4.40  1996/09/10  18:11:10  sluong
 * CSCdi65486:  IPX EIGRP Updates not Propogated with MTU < EIGRP packet
 * Size
 * Branch: California_branch
 *
 * Revision 3.14.4.39  1996/09/05  18:40:59  akr
 * CSCdi66734:  ipx access-list 800 permit -1 nvgens wrongly
 * Branch: California_branch
 *
 * Revision 3.14.4.38  1996/08/28  13:25:16  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.14.4.37  1996/08/28  00:52:49  hampton
 * Migrate Desktop Protocols away from the one minute and one second
 * registries.  [CSCdi67383]
 * Branch: California_branch
 *
 * Revision 3.14.4.36  1996/08/21  20:38:32  hampton
 * Further isolate VLAN code from IPX internals.  [CSCdi66726]
 * Branch: California_branch
 *
 * Revision 3.14.4.35  1996/08/14  18:09:39  john
 * CSCdi64010:  IPX SPX spoofing stops when spoof table gets large -
 *              need to age out
 * Branch: California_branch
 *
 * Revision 3.14.4.34  1996/08/14  02:37:59  hampton
 * Use static inline routines to clean up references to the access list
 * array.  [CSCdi65925]
 * Branch: California_branch
 *
 * Revision 3.14.4.33  1996/08/07  09:05:37  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.14.4.32  1996/08/04  08:44:41  akr
 * CSCdi64779:  show access-list 900 doesnt show things properly
 * Branch: California_branch
 *
 * Revision 3.14.4.31  1996/08/03  23:44:34  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.14.4.30  1996/08/02  07:22:42  akr
 * CSCdi64780:  access-list 900 broken for plain-english-filters
 * Branch: California_branch
 *
 * Revision 3.14.4.29  1996/07/26  00:54:25  hampton
 * Fix the automore code in show access-list.  [CSCdi61084]
 * Branch: California_branch
 *
 * Revision 3.14.4.28  1996/07/26  00:09:16  hampton
 * Various minor IPX cleanups.  [CSCdi64087]
 * Branch: California_branch
 *
 * Revision 3.14.4.27  1996/07/23  20:20:32  john
 * CSCdi63741:  Misalignment at nov_fastswitch(0x6017aa4c)+0x40
 * Branch: California_branch
 *
 * Revision 3.14.4.26  1996/07/23  13:33:15  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.14.4.25  1996/07/21  23:04:37  john
 * CSCdi63412:  IPX data being corrupted across EIP on 7513
 * Branch: California_branch
 *
 * Revision 3.14.4.24  1996/07/13  11:42:57  fred
 * CSCdi62873:  NULL dereference in fair-queue routines when called by
 *         traffic shaping - make hash_mask independent of caller
 * Branch: California_branch
 *
 * Revision 3.14.4.23  1996/07/02  23:12:03  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.14.4.22  1996/06/28  23:33:46  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.14.4.21  1996/06/28  18:13:10  john
 * CSCdi61632:  distribute-list changes not properly propagated to IPX
 *              EIGRP
 * Branch: California_branch
 *
 * Revision 3.14.4.20  1996/06/27  01:10:27  mschaef
 * CSCdi61334:  Alignment error, building IPX route cache entry
 * Branch: California_branch
 *
 * Revision 3.14.4.19  1996/06/19  00:23:01  sluong
 * CSCdi42806:  spx-spoofing stops to remote PC, when RPRINTER being used.
 * Branch: California_branch
 *
 * Revision 3.14.4.18  1996/06/18  23:18:28  sluong
 * CSCdi53070:  IPX SPX-spoofing : Software forced crash, Watchdog.
 * The original patch were wiped out.
 * Branch: California_branch
 *
 * Revision 3.14.4.17  1996/06/01  01:33:35  hampton
 * Remove unnecessary cross module references.  [CSCdi59221]
 * Branch: California_branch
 *
 * Revision 3.14.4.16  1996/05/24  22:51:56  sluong
 * CSCdi51235:  Crash in show_novaccess
 * Branch: California_branch
 *
 * Revision 3.14.4.15  1996/05/17  21:05:15  akr
 * CSCdi57514:  logging is turned on by default for ipx access lists
 * Branch: California_branch
 *
 * Revision 3.14.4.14  1996/05/17  12:18:19  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.11.2.5  1996/05/06  00:12:56  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.11.2.4  1996/04/26  08:00:17  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.11.2.3  1996/04/03  22:22:14  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.11.2.2  1996/03/17  18:56:54  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.11.2.1  1996/03/16  12:10:38  ppearce
 * CSCdi50498:  IBU modularity - Implement NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Move NetBios fields from hwidbtype/idbtype structs to NetBios subblock
 *
 * Revision 3.14.4.13  1996/05/13  17:55:36  john
 * CSCdi35609:  IPX ping to local low end ethernet does not work
 *              make IPX ping to local interface do what IP does.
 * Branch: California_branch
 *
 * Revision 3.14.4.12  1996/05/10  18:27:47  john
 * CSCdi57227:  IPX access-list nvm as spurious log at end of command
 * Branch: California_branch
 *
 * Revision 3.14.4.11  1996/05/04  02:18:59  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Enqueue to routable packet received on bridged interface to the
 *   appropriate queue instead of bridgeQ.
 * - Restore input to the BVI if necessary when copying packet for
 *   process-level.
 *
 * Name Access List
 *
 * Revision 3.14.4.10  1996/05/03  02:19:54  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.14.4.9  1996/04/29  02:42:33  bcole
 * CSCdi55590:  configuring ipx routing  breaks IP ping
 * Branch: California_branch
 *
 * Add 'ipdiag' to the list of subsystem dependencies for subsystems which
 * support the ping command.
 *
 * Revision 3.14.4.8  1996/04/17  13:52:33  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.14.4.6  1996/04/08  22:07:56  john
 * CSCdi53685:  Allow encoded characters in parser string macro
 * Branch: California_branch
 *
 * Revision 3.14.4.5  1996/04/02  07:18:38  dkatz
 * CSCdi51966:  EIGRP SAP split horizon causes problems
 * Branch: California_branch
 * Default split horizon off until we get to the bottom of this.
 *
 * Revision 3.14.4.4  1996/04/02  05:38:29  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.14.4.3  1996/03/29  02:32:25  hou
 * CSCdi51527:  vLAN ISL does not fast switch ipx ping echo return packets
 * Branch: California_branch
 * and also check swidb instead of hwidb to determine input interface for
 * 7000 platform.
 * - isl_fastswitch():
 *   set input_hwidb->fast_rcvidb to input vlan interface after vlan
 *   demultiplex.
 * - internet_enqueue():
 *   pass packet to IP queue only when the input swidb is IP enabled.
 * - nov_set_nidb_idb():
 *   assign novell idb by checking swidb instead of hwidb
 *
 * Revision 3.14.4.2  1996/03/26  23:27:12  sluong
 * CSCdi51564:  show access-list doesnt display 1200 lists
 * Branch: California_branch
 *
 * Revision 3.14.4.1  1996/03/18  22:53:50  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.2.4  1996/03/16  08:00:40  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.8.2.3  1996/03/13  02:15:03  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.8.2.2  1996/03/07  11:22:27  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.8.2.1  1996/02/20  22:01:10  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.14  1996/03/07  06:47:20  shj
 * CSCdi50313:  IPX packets not evenized when sent via LANE
 * Treat ethernet LANE subinterfaces same as real ethernet.
 *
 * Revision 3.13  1996/03/02  17:20:01  john
 * CSCdi50505:  IPX ppp-client asynch interface delay value wrong
 *
 * Revision 3.12  1996/03/01  07:34:28  mschaef
 * CSCdi50473:  NLSP route aggregation conformance (phase 1)
 *
 * Fix assorted nits and bugs
 *
 * Revision 3.11  1996/02/25  03:53:35  mmcneali
 * CSCdi49686:  IPX non-functional in gs7 images on ISL encapsulating
 *              subinterfaces.
 *
 * Revision 3.10  1996/02/17  00:49:44  hou
 * CSCdi46043:  Only 50 percent of ping packets are fast switched on vLAN
 *              IPX routing, also, router doesn't learn all IPX networks
 *              because it drops some RIP packets coming from it's vlan
 *              subinterfaces.
 * - nov_set_nidb_idb():
 *   checking the swidb instead of hwidb to determine vLAN packets' src
 *   IPX network number.
 *
 * Revision 3.9  1996/01/31  02:13:29  snyder
 * CSCdi47864:  spellink errors
 *              progrss -> progress
 *
 * Revision 3.8  1996/01/17  03:33:59  boyle
 * CSCdi41676:  EIGRP not propagating sap when socket number changes
 *
 * Revision 3.7  1996/01/05  18:50:49  john
 * CSCdi43117:  Crash in routine process_handle_watchdog
 *
 * Revision 3.6  1996/01/01  00:04:12  john
 * CSCdi44144:  IPX/EIGRP does not always keep adjacencies over x.25 link
 *
 * Revision 3.5  1995/12/02  08:26:39  mschaef
 * CSCdi37520:  mac-address command not taken upon router reload or
 * initial config
 *
 * Revision 3.4  1995/12/01  06:26:20  mschaef
 * CSCdi37234:  Allow fastswitching of certain directed broadcast packets
 *
 * Revision 3.3  1995/11/17  19:24:11  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:54:09  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:59:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.21  1995/11/08  21:39:41  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.20  1995/10/26  00:53:55  mschaef
 * CSCdi42340:  ipx maximum-path should allow more than 4 if NLSP is not on
 *
 * Revision 2.19  1995/10/13  01:04:41  shj
 * CSCdi41571:  atm interface uses wrong ipx node address
 * Backout CSCdi38984, which caused wrong ipx address on non-LANE ATM
 * interfaces.  Instead, LANE now changes ipx address if ipx is already
 * configured when LANE Client configured.
 *
 * Revision 2.18  1995/09/28  18:01:45  mschaef
 * CSCdi41028:  Memory leak when configuring/Deconfiguring IPX
 *
 * Revision 2.17  1995/09/07  01:46:14  mikel
 * CSCdi38859:  When serial encap changes, eigrp hold and hello times dont
 * change
 * Make sure to change the eigrp hold and hello timers to the proper NBMA
 * or non-NBMA defaults if we have a encap change.  This change only takes
 * place if have the default timers set
 *
 * Revision 2.16  1995/09/01  23:06:52  asastry
 * CSCdi39698:  WFQ: Display info (in "show queue") for non-IP protocols
 * as well
 *
 * Revision 2.15  1995/08/25  14:11:54  hampton
 * Minor Novell IPX Cleanups.  Remove some unused variables and routines.
 * Eliminate direct references to the system clock.  [CSCdi39328]
 *
 * Revision 2.14  1995/08/03 22:20:43  john
 * CSCdi38175:  SPX spoofing sometimes fails to start on one side of link
 *
 * Revision 2.13  1995/07/31  21:03:51  john
 * CSCdi36543:  Rconsole is time-out with 2 netware clients (SPX-Spoof)
 *
 * Revision 2.12  1995/07/28  23:56:45  john
 * CSCdi37833:  IPX triggered rip/sap delays need a global default
 *
 * Revision 2.11  1995/07/20  20:22:24  john
 * CSCdi35867:  %ALIGN-3-SPURIOUS: Spurious memory access w/show align
 *              too! Align errors when a SAP queue max is configured.
 *
 * Revision 2.10  1995/07/18  22:33:47  john
 * CSCdi34411:  Large ipx output-rip/sap-delays may delay routing from
 *              running
 *
 * Revision 2.9  1995/07/09  21:10:56  speakman
 * CSCdi36848:  Configuration of CRB proves unpopular
 * Derive the CRB route/bridge behaviour from explicit, per-interface
 * route/bridge indicators rather than inferring it from protocol-
 * specific configuration.
 *
 * Revision 2.8  1995/07/01  03:39:16  hampton
 * Minor Novell IPX Cleanups.  Remove some unused variables and routines.
 * Eliminate direct references to the system clock.  [CSCdi36668]
 *
 * Revision 2.7  1995/06/20 21:54:13  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.6  1995/06/19  19:10:47  john
 * CSCdi34017:  "clear ipx spx-spoof" does not clear the table
 *
 * Revision 2.5  1995/06/19  04:10:10  speakman
 * CSCdi35697:  General TB and SRB regression failures
 * Make protocol-specific route/bridge test explicit rather than relying
 * on per-interface rxtypes indicators to provide backward compatible
 * SRB and TB behaviour.
 *
 * Revision 2.4  1995/06/18  06:20:08  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.3  1995/06/16 23:00:53  shaker
 * CSCdi35986:  Fix trivial typo in xns/novinput.c
 * Fix a trivial mis spelling
 *
 * Revision 2.2  1995/06/13  22:15:35  mschaef
 * CSCdi35794:  Problems with IPX default route
 *
 * Revision 2.1  1995/06/07  23:29:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#undef DEBUG_ENCAPS
#undef DEBUG_ITF
#undef DEBUG_LEN

#include "master.h"
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "../os/signal.h"
#include "mgd_timers.h"
#include "interface_private.h"

#include "packet.h"
#include "linktype.h"
#include "../ui/debug.h"
#include "../if/ether.h"
#include "config.h"
#include "xns.h"
#include "novell.h"
#include "novroute.h"
#include "novell_debug.h"
#include "novigrp.h"
#include "novnlsp.h"
#include "novfast.h"
#include "novell_public.h"
#include "config.h"
#include "access.h"
#include "logger.h"
#include "../xns/msg_ipx.c"		/* Not a typo, see logger.h */
#include "../if/network.h"
#include "../if/priority_private.h"
#include "sr_novell_nlsp.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../clns/isis_ipx.h"
#include "../os/free.h"
#include "../h/cs_registry.regh"
#include "../wan/dialer_registry.h"
#include "../clns/isis_externs.h"
#include "../ibm/netbios_sb.h"
#include "ipx_registry.h"
#include "../clns/clns_registry.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */

#ifdef DEBUG_ENCAPS
#include "../if/fddi.h"
#include "../if/tring.h"
#include "../wan/serial.h"
#endif

#include "../h/address.h"

#include "../vlan/vlan.h"

#include "../if/atm.h"
#include "../lane/lane.h"

void novell_setup_dynamic_int(setup_struct *sp);
void novell_setup_alllines(setup_struct *sp);

/*
 * Local Storage
 */
queuetype novpdbQ;
queuetype nov_router_initQ;
watched_queue *novQ;			/* Novell input queue */
queuetype novidbQ;			/* Novell virtual network list */
static uchar nov_trmulticast[IEEEBYTES] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
uchar novell_allhosts[IEEEBYTES] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static nov_logcache_entry logcache[NOVLOGCACHESIZE];
static ulong		  nov_logcache_version;


/*
 * The Novell subsystem header. If you change this, be prepared to spend
 * about 10-20 thousand dollars in Novell re-certification fees. It'll
 * come out of YOUR paycheck if you change this without talking to a whole
 * lot of people first. :-)
 */
#define IPX_MAJVERSION 2
#define IPX_MINVERSION 0
#define IPX_EDITVERSION  1

/*
 * Add lane in the seq: string to make sure the lane interface
 * config commands will be parsed before ipx's.
 */
SUBSYS_HEADER(ipx, IPX_MAJVERSION, IPX_MINVERSION, IPX_EDITVERSION,
	      nov_init, SUBSYS_CLASS_PROTOCOL,
 	      "seq: iphost, ipdiag, netbios_acl, lane",
	      "req: netbios_acl");


/*
 * fair conversation generator
 *
 *      level 2 protocol        1
 *      destination net         4
 *      destination host        6
 *      destination socket      2
 *      source net              4
 *      source host             6
 *      source socket           2
 */
static
short novell_fair_queue_flow_id (paktype *pak)
{
    ipxhdrtype *ipx;
    int         sum;

    pak->fair_weight = FAIR_QUEUE_BASE_WEIGHT;
    ipx = (ipxhdrtype *)ipxheadstart(pak);
    /* hash protocol, source, and destination */
    sum = xnethash((uchar *)&ipx->pt, 25);
    return(fair_queue_sum(pak, sum));
}
/*
 * ipx_print_network_header
 *
 * Print Apollo header.
 */
static void novell_print_network_header (paktype *pak, ushort **data)
{
    ipxhdrtype *ipx;

    ipx = (ipxhdrtype *)ipxheadstart(pak);
    printf("\n  source:%b socket %04x destination %b socket %04x protocol %02x",
	   ipx->ssnet, ipx->shost, ipx->ssock, 
	   ipx->ddnet, ipx->dhost, ipx->dsock, ipx->pt);
}

/*
 * Dump novell encap detail
 */
#ifdef DEBUG_ENCAPS
void
print_novell_encap (paktype *pak,
		    idbtype *swidb,
		    char *kind)
{
    ulong *dataptr;
    hwidbtype *hwidb;
    ieee_addrs *addrs;
    ether_hdr *ether;
    vhdlctype *hdlc;
    sap_hdr *sap;
    snap_hdr *snap;
    fddi_novell *fddi;

    if (!novell_debug)
	return;
    /*
     * Screen out bogus packets:
     */
    if ((pak == NULL) || !validmem(pak)) {
	buginf("print_novell_encap: pak NULL or invalid");
	return;
    }

    if ((swidb == NULL) || !validmem(swidb)) {
	buginf("print_novell_encap: swidb NULL or invalid");
	return;
    }

    hwidb = swidb->hwptr;

    if ((hwidb == NULL) || !validmem(hwidb)) {
	buginf("print_novell_encap: hwidb NULL or invalid");
	return;
    }
    
    dataptr = (ulong *) (pak->network_start);
    switch (pak->enctype) {
      case ET_ARPA:
	ether = (ether_hdr *)pak->datagramstart;
	buginf("\n%s: %s %s, src %e, dst %e, type 0x%-04x"
	       "\n%-04x%-08x%-08x%-08x:"
	       "\n%-08x%-08x%-08x%-08x%-08x%-08x%-08x%-08x"
	       ", hwlen %d",
	       swidb->short_namestring, kind, encstring(pak->enctype),
	       ether->saddr, ether->daddr, ether->type_or_len,
	       (ushort)dataptr[-4], dataptr[-3], dataptr[-2], dataptr[-1],
	       dataptr[0], dataptr[1], dataptr[2], dataptr[3], dataptr[4],
	       dataptr[5], dataptr[6], dataptr[7], pak->datagramsize);
	break;
      case ET_SAP:
	addrs = (ieee_addrs *)pak->addr_start;
	sap = (sap_hdr *)pak->info_start;
	buginf("\n%s: %s %s, src %e, dst %e, type 0x%-02x%-02x, ctl 0x%-02x"
	       "\n%-02x%-08x%-08x%-08x%-08x:"
	       "\n%-08x%-08x%-08x%-08x%-08x%-08x%-08x%-08x"
	       ", hwlen %d",
	       swidb->short_namestring, kind, encstring(pak->enctype),
	       addrs->saddr, addrs->daddr,
	       sap->dsap, sap->ssap, sap->control,
	       (uchar)dataptr[-5], dataptr[-4], dataptr[-3],
	       dataptr[-2], dataptr[-1], dataptr[0],
	       dataptr[1], dataptr[2], dataptr[3], dataptr[4],
	       dataptr[5], dataptr[6], dataptr[7], pak->datagramsize);
	break;
      case ET_SNAP:
	addrs = (ieee_addrs *)pak->addr_start;
	snap = (snap_hdr *)pak->info_start;
	buginf("\n%s: %s %s, src %e, dst %e, type 0x%-04x"
	       "\n%-04x%-08x%-08x%-08x%-08x%-08x:"
	       "\n%-08x%-08x%-08x%-08x%-08x%-08x%-08x%-08x"
	       ", hwlen %d",
	       swidb->short_namestring, kind, encstring(pak->enctype),
	       addrs->saddr, addrs->daddr, snap->type,
	       (ushort)dataptr[-6], dataptr[-5], dataptr[-4],
	       dataptr[-3], dataptr[-2], dataptr[-1],
	       dataptr[0], dataptr[1], dataptr[2], dataptr[3], dataptr[4],
	       dataptr[5], dataptr[6], dataptr[7], pak->datagramsize);
	break;
      case ET_NOVELL_ETHER:
	ether = (ether_hdr *)pak->datagramstart;
	buginf("\n%s: %s %s, src %e, dst %e, len %d"
	       "\n%-04x%-08x%-08x%-08x:"
	       "\n%-08x%-08x%-08x%-08x%-08x%-08x%-08x%-08x"
	       ", hwlen %d",
	       swidb->short_namestring, kind, encstring(pak->enctype),
	       ether->saddr, ether->daddr, ether->type_or_len,
	       (ushort)dataptr[-4],dataptr[-3], dataptr[-2], dataptr[-1],
	       dataptr[0], dataptr[1], dataptr[2], dataptr[3], dataptr[4],
	       dataptr[5], dataptr[6], dataptr[7], pak->datagramsize);
	break;
      case ET_NOVELL_FDDI:
	fddi = (fddi_novell *)pak->datagramstart;
	buginf("\n%s: %s %s, fc %-02x src %e, dst %e"
	       "\n%-08x%-08x%-08x%-08x:"
	       "\n%-08x%-08x%-08x%-08x:%-08x%-08x%-08x%-08x"
	       ", hwlen %d",
	       swidb->short_namestring, kind, encstring(pak->enctype),
	       fddi->fc, fddi->saddr, fddi->daddr,
	       dataptr[0], dataptr[1], dataptr[2], dataptr[3],
	       dataptr[4], dataptr[5], dataptr[6], dataptr[7],
	       dataptr[8], dataptr[9], dataptr[10], dataptr[11],
	       pak->datagramsize);
	break;
      case ET_HDLC:
	hdlc = (vhdlctype *)pak->datagramstart;
	buginf("\n%s: %s %s, size %d, type %#x, flags %#x"
	       "\n%-08x:"
	       "\n%-08x%-08x%-08x%-08x%-08x%-08x%-08x%-08x"
	       ", hwlen %d",
	       swidb->short_namestring, kind, encstring(pak->enctype),
	       pak->datagramsize, hdlc->var_hdlctype, hdlc->var_hdlcflags,
	       dataptr[-1], dataptr[0], dataptr[1], dataptr[2], dataptr[3],
	       dataptr[4],dataptr[5], dataptr[6], dataptr[7],
	       pak->datagramsize);
	break;
      default:
	buginf("\n%s -> %s: %s %s, size %d",
	       (pak->if_input ? pak->if_input->short_namestring : "NULL"),
	       (pak->if_output ? pak->if_output->short_namestring : "NULL"),
	       kind, encstring(pak->enctype), pak->datagramsize);
	break;
    }
}

#endif


/*
 * Common packet display routine
 */
static void
print_packet (
	      paktype *pak,
	      char *desc)
{
    ipxhdrtype *ipx;
#ifdef DEBUG_LEN
    ushort dgsize = 0;
    ushort typelen = 0;
    ushort ipxlen = 0;
    ushort paklen = 0;
#endif

    if (!novell_debug && !novell_spoof_debug)
	return;

    ipx = (ipxhdrtype *)ipxheadstart(pak);

    buginf("\nIPX: ");

#ifdef DEBUG_LEN
    if (pak->if_input) {
	dgsize = pak->datagramsize;
	ipxlen = GETSHORT(&ipx->len);
	if (pak->if_input->hwptr->status & (IDB_ETHER|IDB_LEX)) {
	    ether_hdr *ether;

	    ether = (ether_hdr *)pak->datagramstart;

	    switch (pak->nif_input->enctype) {
	      case ET_NOVELL_ETHER:
	      case ET_ARPA:
	      case ET_SAP:
	      case ET_SNAP:
		typelen = ether->type_or_len;
		break;
	      default:
		break;
	    }
	}
    }

    paklen = pak_get_pool_size((paktype*)pak);

    if (dgsize || typelen || ipxlen)
	buginf("[%-04x/%-04x/%-04x/%-04x] ", dgsize, typelen, ipxlen, paklen);
#endif

    buginf("%s:%x.%e->%x.%e",
	   (pak->if_input ? pak->if_input->short_namestring : "local"),
	   GET2WD(ipx->ssnet), ipx->shost,
	   GET2WD(ipx->ddnet), ipx->dhost);

    buginf(" ln=%-3d tc=%-02x", GETSHORT(&ipx->len), ipx->tc);

    if (novell_debug1)
	buginf(" pt=%-02x ds=%-04x ss=%-04x", ipx->pt, GETSHORT(&ipx->dsock),
	       GETSHORT(&ipx->ssock));
    
    buginf(", %s", desc);

}

/*
 * novell_enable_test
 * Return TRUE if we're running Novell
 * Called by SERVICE_PROTO_RUNNING.
 */

static boolean novell_enable_test (void)
{
    return(novell_running);
}

/*
 * novell_routing_idb
 * Return TRUE iff IPX packets inbound on this interface should be
 * routed.
 */

static boolean novell_routing_idb (idbtype *swidb)
{
    if ((global_crb_enable || global_irb_enable) && swidb->tbridge_on_swidb) {
	/*
	 * For CRB, just return the novell route/bridge indicator
	 * if this interface is in a bridge group.  This is
	 * set by the "bridge <n> route ipx" command.
	 */
	return(swidb->tbridge_route_linktypes & LINK_NOVELL_RBI);
    } else {
	/*
	 * Otherwise, just verify that novell routing is enabled,
	 * and this interface is enabled for novell.  This latter
	 * is set by the "ipx network" command.
	 */
	return(novell_running && swidb->novellnets);
    }
}

/*
 * novell_bridging_idb
 * Return TRUE iff NOVELL packets inbound on this interface should be
 * bridged.
 */

static boolean novell_bridging_idb (idbtype *swidb)
{
    if (global_crb_enable) {
	/*
	 * For CRB, just return the complement of the novell
	 * route/bridge indicator if this interface is in
	 * a bridge group.  This is cleared by the
	 * "no bridge <n> route novell" command (the default).
	 */
	return(swidb->tbridge_on_swidb &&
	       (!(swidb->tbridge_route_linktypes & LINK_NOVELL_RBI)));
    } else if (global_irb_enable) {
	return(swidb->tbridge_on_swidb &&
	       !(swidb->tbridge_bridge_linktypes & LINK_NOVELL_RBI));
    } else {
	/*
	 * To support SRB and the archaic transparent bridging
	 * behaviour just check for the absence of novell routing.
	 */
	return(!novell_running);
    }
}
/*
 * novell_rxtypes_update
 *
 * Maintain the route/bridge state of a protocol's rxtypes.
 * Called whenever protocol-specific configuration changes
 * or transparent-bridging-specific configuration changes
 * are made.  Updates a swidb's rxtype_flags to reflect the
 * new configuration.
 *
 * Determines whether the protocol should be routed and sets/
 * clears rxtypes_flags appropriately.  If Concurrent Routing
 * and Bridging is enabled, the decision is based on the
 * protocol configuration for the given swidb.  Otherwise,
 * the decision is based on the protocol configuration for
 * the router as a whole.
 */

void novell_rxtypes_update (idbtype *swidb)
{
    boolean rte_nv, br_nv;

    if (global_crb_enable || global_irb_enable) {
	rte_nv = novell_routing_idb(swidb);
	br_nv = novell_bridging_idb(swidb);
    } else {
	rte_nv = novell_running;
	br_nv = !rte_nv;
    }

    if (rte_nv) {
	SET_ROUTING_RXTYPE(swidb, RXTYPE_NOVELL1);
	SET_ROUTING_RXTYPE(swidb, RXTYPE_NOVELL2);
	SET_ROUTING_RXTYPE(swidb, RXTYPE_SNAP_NOV1);
	SET_ROUTING_RXTYPE(swidb, RXTYPE_SAP_NOVELL);
    } else {
	CLEAR_ROUTING_RXTYPE(swidb, RXTYPE_NOVELL1);
	CLEAR_ROUTING_RXTYPE(swidb, RXTYPE_NOVELL2);
	CLEAR_ROUTING_RXTYPE(swidb, RXTYPE_SNAP_NOV1);
	CLEAR_ROUTING_RXTYPE(swidb, RXTYPE_SAP_NOVELL);
    }

    if (br_nv) {
	CLEAR_BRIDGING_RXTYPE(swidb, RXTYPE_NOVELL1);
	CLEAR_BRIDGING_RXTYPE(swidb, RXTYPE_NOVELL2);
	CLEAR_BRIDGING_RXTYPE(swidb, RXTYPE_SNAP_NOV1);
	CLEAR_BRIDGING_RXTYPE(swidb, RXTYPE_SAP_NOVELL);
    } else {
	SET_BRIDGING_RXTYPE(swidb, RXTYPE_NOVELL1);
	SET_BRIDGING_RXTYPE(swidb, RXTYPE_NOVELL2);
	SET_BRIDGING_RXTYPE(swidb, RXTYPE_SNAP_NOV1);
	SET_BRIDGING_RXTYPE(swidb, RXTYPE_SAP_NOVELL);
    }

    if (swidb->hwptr->setup_bridging)
	(*(swidb->hwptr->setup_bridging))(swidb->hwptr);

    reg_invoke_bridge_setup();	/* sse_bridging_setup_enqueue() */
}

/*
 * novell_showproto
 * Interface specific portion of "show protocols" commands
 * Called by SERVICE_SHOWPROTO
 */

static void novell_showproto (idbtype *idb)
{
    nidbtype *nidb;

    if (idb == NULL) {
	if (RUNNING(LINK_NOVELL))
	    printf("\n  IPX routing is enabled");
	return;
    }
    if (RUNNING_IF(LINK_NOVELL, idb)) {
	FOR_ALL_NIDBS(nidb) {
	    if (nidb->idb == idb) {
		printf("\n  IPX address is %x.%e", nidb->novellnet,
		       idb->novell_node);
		if (idb->novellnets > 1)
		    printf(" (%s)", encstring(nidb->enctype));
	    }
	}
    }
}

/*
 * Initialize an IDB for novell (extracted so that we can call it for
 * dynamicly created interfaces like the tunnel)
 *
 */

void nov_init_idb (idbtype *idb, boolean setup_hwidb)
{
    netbios_sb_t *netb_sb;

    idb->novellnets = 0;
    novell_rxtypes_update(idb);
    IPX_CLEAR_PRIMARY_NIDB(idb);
    idb->nov_update_interval = NOVELL_RIP_DEFAULT;
    if (setup_hwidb) {
	idb->hwptr->novell_fastokay = FALSE;
	idb->hwptr->novell_input_acl_checks = FALSE;
	idb->hwptr->novell_routecache = NOVELL_RCACHE;
	idb->hwptr->novell_ether_ok = FALSE;
	idb->hwptr->novell_arpa_ok = FALSE;
	idb->hwptr->novell_iso1_ok = FALSE;
	idb->hwptr->novell_iso2_ok = FALSE;
    }
    idb->novell_type20 = FALSE;
    idb->nov_rip_multiplier = idb->nov_sap_multiplier =
      NOVELL_RIP_INVALIDTIME;
    idb->nov_rip_maxpacketsize = NOVELL_RIP_MAXSIZE;
    idb->nov_sap_maxpacketsize = NOVELL_SAP_MAXSIZE;
    idb->nov_respond_to_gns = TRUE;
    idb->nov_gns_delay = novell_gns_response_delay;
    idb->novell_output_acl_checks = FALSE;
    /*
     * By default, pad odd-length, slow-switched packets only on ethernet
     * (ethernet LANE is same as real ethernet).
     */
    idb->novell_evenize = (idb->hwptr->status & (IDB_ETHER|IDB_LEX))  ||
                          lec_enabled(idb);
    idb->novell_sap_delay = novell_def_output_sap_dly;
    idb->novell_rip_delay = novell_def_output_rip_dly;

    netb_sb = netbios_get_or_create_sb(idb);
    if (!netb_sb) {
      return; /* No memory -- ppearce*/
    }
    netb_sb->nov_netbios_h_iacf = NULL;
    netb_sb->nov_netbios_h_oacf = NULL;
    netb_sb->nov_netbios_b_iacf = NULL;
    netb_sb->nov_netbios_b_oacf = NULL;

    /*
     * The following are not used but are initialized to NULL
     * for safety since the data structures are still referenced
     * by XNS and APOLLO.
     */
    idb->novell_delay = 0;
    idb->novell_interval = 1;
    idb->novell_deleted_nidb = FALSE;
    idb->nov_ipxwan_enabled = FALSE;
}

/*
 * novell_idb_delay_values
 * Return IDB delay values associated with this IDB.
 *
 * Returns TRUE if successful, FALSE if null IDB or NIDB pointers were
 * detected.  Returns all parameters zero if NULL pointers were detected.
 */
static boolean novell_idb_delay_values (idbtype *idb, ulong *link_delay,
				 ulong *link_throughput, ulong *ipx_ticks)
{
    nidbtype *nidb;

    *link_delay = 0;
    *link_throughput = 0;
    *ipx_ticks = 0;

    if (idb == NULL)
        return(FALSE);
    nidb = IPX_PRIMARY_NIDB(idb);
    if (nidb == NULL)
        return(FALSE);
    if (nov_ipxwan_on_nidb(nidb)) {
	*link_delay = NOVELL_LINK_DELAY(nidb);
	*link_throughput  = NOVELL_LINK_THROUGHPUT(nidb);
	*ipx_ticks = NOVELL_IDB_TICKS(nidb);
    } else {
	*link_delay = nidb->novell_link_delay;
	*link_throughput = nidb->novell_link_throughput;
	*ipx_ticks = NOVELL_IDB_IPXWAN_TICKS(nidb);
    }
    if (*link_delay == 0)
	*link_delay = idb->delay * 10;	/* It's in tens of microseconds! */
    if (*link_throughput == 0)
	*link_throughput = idb->visible_bandwidth * 1000; /* In bits */
    return(TRUE);
}

/*
 * novell_bw_delay_changed
 *
 * Called by registry when the global interface bandwidth or delay changes.
 *
 * We in turn invoke the IPX delay/bandwidth change registry, since the
 * global parameters may impact the IPX parameters.
 */
static void novell_bw_delay_changed (idbtype *idb)
{
    if (idb && idb->primary_nidb)
	reg_invoke_ipx_nidb_bw_delay_changed(idb->primary_nidb);
}

/*
 * ipx_default_pkt_type_handler
 * Returns false
 */
static boolean ipx_default_pkt_type_handler (paktype *pak)
{
    return (FALSE);
}

/*
 * novell_test_fastswitch_bcast
 *
 */
static boolean novell_test_fastswitch_bcast (void)
{
    return(novell_fastswitch_bcast);
}

void ipx_set_encaps (idbtype *swidb, int encaps)
{
    nidbtype *nidb;

    if (!novell_on_idb(swidb))
      return;
    nidb = swidb->primary_nidb;
    if (!nidb)
	return;
    if (encaps == ET_NULL)
	nidb->enctype = novell_get_default_encapsulation(swidb);
    else
	nidb->enctype = encaps;
}

/*
 * novell_register_thyself
 */
static void novell_register_thyself (void)
{
    novell_parser_init();
    novell_media_registry();

    /*
     * Register some functions
     */
    reg_add_bump_cache_version(novcache_external_increment_version,
		      "novcache_external_increment_version");
    reg_add_fast_setup(nov_fastsetup, "nov_fastsetup");
    reg_add_media_hwaddr_notify(nov_hwaddr_notify, "nov_hwaddr_notify");
    reg_add_mac_address_change(nov_mac_address_change,
			       "nov_mac_address_change");
    reg_add_snapshot_start_aging_routes(LINK_NOVELL, novell_start_aging,
					"novell_start_aging");
    reg_add_route_adjust(novrip_route_adjust, "novrip_route_adjust");
    reg_add_sw_idb_init(nov_init_idb, "nov_init_idb");
    reg_add_proto_on_swidb(LINK_NOVELL, novell_on_idb, "novell_on_idb");
    reg_add_routed_proto_on_swidb(LINK_NOVELL, novell_on_idb, "novell_on_idb");
    reg_add_ping_run(LINK_NOVELL, nov_ping, "nov_ping");
    reg_add_ping_parse(novell_ping_parse, "novell_ping_parse");
    reg_add_proto_running(LINK_NOVELL, novell_enable_test,
			  "novell_enable_test");
    reg_add_proto_bridging_on_swidb(LINK_NOVELL, novell_bridging_idb,
				    "novell_bridging_idb"); 
    reg_add_proto_routing_on_swidb(LINK_NOVELL, novell_routing_idb,
				   "novell_routing_idb");
    reg_add_determine_fair_queue_flow_id(LINK_NOVELL,
					 novell_fair_queue_flow_id,
					 "novell_fair_queue_flow_id");
    reg_add_print_network_header(LINK_NOVELL, novell_print_network_header,
				 "novell_print_network_header");
    reg_add_rxtypes_update(novell_rxtypes_update,"novell_rxtypes_update");
    reg_add_access_show(show_novaccess, "show_novaccess");
    reg_add_showproto(novell_showproto, "novell_showproto");
    reg_add_setup_global(novell_setup_global, "novell_setup_global");
    reg_add_setup_global2(novell_setup_global2, "novell_setup_global2");
    reg_add_setup_int(novell_setup_int, "novell_setup_int");
    reg_add_setup_end(novell_setup_end, "novell_setup_end");
    reg_add_setup_async_int(novell_setup_dynamic_int, "novell_async_int");
    reg_add_alllines_setup(novell_setup_alllines, "novell_setup_allines");
    reg_add_swif_erase(novell_swif_erase, "novell_swif_erase");
    reg_add_ipx_return_static_counts(novell_return_static_counts,
				     "novell_return_static_counts");
    reg_add_ipx_idb_delay_values(novell_idb_delay_values,
				 "novell_idb_delay_values");
    reg_add_bandwidth_changed(novell_bw_delay_changed, 
			      "novell_bw_delay_changed");
    reg_add_ipx_novell_fastswitch_bcast(novell_test_fastswitch_bcast,
					"novell_test_fastswitch_bcast");
    reg_add_delay_changed(novell_bw_delay_changed,
			  "novell_bw_delay_changed");
    reg_add_pt_novell_pppclient_configure(novell_pppclient_configure,
			  "novell_pppclient_configure");
    reg_add_pt_novell_get_owner_idb(novell_get_owner_idb,
			  "novell_get_owner_idb");
    reg_add_default_ipx_input_paktype(ipx_default_pkt_type_handler,
				      "IPX pkt type handler default");
    reg_add_ipx_configure_for_lane(ipx_configure_for_lane,
                                   "ipx_configure_for_lane");
    reg_add_set_protocol_encaps(LINK_NOVELL, ipx_set_encaps, "ipx_set_encaps");
    novell_test_init();
    register_snmp_ipx_echo();	/* register snmp ipx ping functions */
}

/*
 * nov_init
 * Initialize Novell protocol machine.
 * Allow bridging of Novell by MCI hardware.
 */

void nov_init (subsystype *subsys)
{
    int i;
    idbtype *idb;

    novell_register_thyself();
    novell_debug_init();
    /*
     * Build initial timer tree:
     */
    mgd_timer_init_parent(&novell_rip_delay_timer, NULL);
    mgd_timer_init_parent(&novell_sap_delay_timer, NULL);
    mgd_timer_init_parent(&novell_super_root_timer, NULL);

    novell_rip_part_wb = create_watched_boolean("IPX RIP PART", 0);
    novell_sap_part_wb = create_watched_boolean("IPX SAP PART", 0);
    novell_link_down_wb = create_watched_boolean("IPX LINKDOWN", 0);
    novell_sent_down_wb = create_watched_boolean("IPX SENTDOWN", 0);

    novell_def_output_rip_dly = NOV_OUTPUT_DELAY_DEFAULT;
    novell_def_output_sap_dly = NOV_OUTPUT_DELAY_DEFAULT;
    novell_def_trig_rip_dly = NOV_OUTPUT_DELAY_DEFAULT;
    novell_def_trig_sap_dly = NOV_OUTPUT_DELAY_DEFAULT;
    FOR_ALL_SWIDBS(idb) 
	nov_init_idb(idb, TRUE);
    for (i = 0; i < NOVELL_MAXTRAFFIC; i++)
	novell_stat[i] = 0;

    novQ = NULL;
    queue_init(&novbgQ, 0);
    queue_init(&novell_genericQ, 0);
    queue_init(&novidbQ,0);
    queue_init(&novell_sapQ,0);
    queue_init(&novell_gnsQ,0);
    queue_init(&nov_router_initQ,0);
    queue_init(&novpdbQ,0);
    queue_init(&nov_freed_ndbQ,0);
    queue_init(&nov_free_nidbQ,0);
    queue_init(&novell_ipxwanQ,0);
    novell_snmpQ = NULL;

    ipx_internal_network_nidb = NULL;
    ipx_kludge_enabled_flag = TRUE;
    novell_set_max_hops(NOVELL_DEFAULT_MAX_HOPS);
    novell_backup_servers_needed = FALSE;
    TIMER_STOP(novell_backup_servers_timer);
    novell_next_output_nidb = NULL;
    novell_freed_ndbq_wakeup = 0;
    novell_update_proc_throttle = 0;
    novell_update_sched_throttle = 0;
    novell_running = FALSE;
    novell_shutting_down = FALSE;
    novell_started = FALSE;
    novell_route_count = 0;
    ipx_server_split_on_server_paths = FALSE;
    ipx_eigrp_sap_split_horizon = IPX_EIGRP_SAP_SPLIT_HORIZON_DEFAULT;
    novell_server_count = 0;
    nov_static_route_count = 0;
    nov_static_server_count = 0;
    novell_table = NULL;
    novell_table2 = NULL;
    novell_table3 = NULL;
    nov_urd_table = NULL;
    novell_urd_insert_count = 0;
    novell_urd_callback_count = 0;
    novell_urd_scrub_count = 0;
    nov_pessimal_route_list = NULL;
    nov_pessimal_sap_list = NULL;
    novell_sap_route_hashtable = NULL;
    novell_backup_server_interval = NOVELL_DEFAULT_BACKUP_SERVER_INTERVAL;
    FOR_ALL_SWIDBS(idb)
	novell_rxtypes_update(idb);
    novell_maxpaths = novell_cfg_maxpaths = RIP_MAXPATHS;
    novell_sapqueuemax = 0;
    novell_gns_response_delay = NOVELL_DEFAULT_GNS_RESPONSE_DELAY;
    novell_ping_deflt = IPX_ECHO_TYPE_CISCO;
    novell_auto_sw_supported = FALSE;
    nov_cache_init();
    nov_gns_roundrobin = FALSE;
    nov_load_share_per_host = FALSE;
    novell_default_route = TRUE;
    novell_default_ndb = NULL;
    novell_algorithms = TRUE;
    novell_sap_uses_routing = TRUE;
    novell_potential_pseudonode = TRUE;
    novell_rip_running = FALSE;
    novell_nlsp_running = 0;
    novell_type20_in_checks = FALSE;
    novell_type20_out_checks = FALSE;
    novell_netbios_in_checks = FALSE;
    novell_type20_helpered = FALSE;
    novell_fastswitch_bcast = FALSE;
    novell_global_acl_checks = FALSE;
    novell_newest_servers = NULL;
    novell_newest_routes = NULL;
    ipx_snmp_init();
    novell_init_atlr_list();
    ipx_acct_init();
    ipx_checkpointvalue = 0;
    nov_spx_initialize = TRUE;
    nov_spx_idle_time = SPX_DEFAULT_IDLE_TIME * ONEMIN;
    nov_spx_clear_time = SPX_DEFAULT_CLEAR_TIME * ONEHOUR;
    rn_inithead((void **)&nov_rnhead, IPXRADIXDATAOFF);
    /*
     * Create a null nidb for "drop" routes.
     */
    if (nullidb->primary_nidb == NULL) {
	nullnidb = malloc_named(sizeof(nidbtype), "IPX IDB");
	if (nullnidb) {
	    nullnidb->idb = nullidb;
	    nullidb->primary_nidb = nullnidb;
	    nullnidb->state = NOVELL_NIDB_UP;
	}
    } else {
	nullnidb->idb = nullidb;
	nullnidb->state = NOVELL_NIDB_UP;
    }
}
/*
 * nov_spx_hashaddress - this routine hashes the net/node/socket/connection
 * a given spx session for use in the hash table
 *
 * 4 pointers to the net/node/socket/and connectionID data
 *
 * returns a hash index
 */

static uchar nov_spx_hashaddress (uchar  *pt1, uchar *pt2, uchar *pt3, uchar *pt4)
{
    uchar result;

    result = ( *pt1 ^ *(pt1+1) ^ *(pt1+2) ^ *(pt1+3) ^ *pt2 ^
	   *(pt2+1) ^ *(pt2+2) ^ *(pt2+3) ^ *(pt2+4) ^ *(pt2+5) ^
	   *pt3 ^ *(pt3+1) ^ *pt4 ^ *(pt4+1) ) / SPOOF_HASH_TABLE_ENTRIES;

    return(result);

}

/* nov_spx_getspoofrecord
 *
 * this routine allocates a new spoof record, getting more records
 *   if it runs out
 */

static nov_spx_spoof_record   *nov_spx_getspoofrecord (void)
{
   nov_spx_spoof_record  *spoofrecordp;
   int           i;

   /* function to pass out a nov_spx_spoof_record to the calling routine
    *  and to allocate more spoof records when needed 
    */

   if (spoofheadpointer == NULL ) {
       spoofheadpointer = malloc(MAX_SPOOF_ENTRIES * sizeof(nov_spx_spoof_record));
       if (spoofheadpointer != NULL) {
	   for (i = 0 ; i < MAX_SPOOF_ENTRIES-1 ; i++)
	       spoofheadpointer[i].chain = &(spoofheadpointer[i+1]);
	   spoofheadpointer[MAX_SPOOF_ENTRIES-1].chain = NULL;
	   spooftailpointer = &(spoofheadpointer[MAX_SPOOF_ENTRIES-1]);
       } else {
	   /* unable to get memory for spoof entry panic!! */
	   /* warn with console message */
	   return NULL;
       }
   }
   /* now have memory, give calling procedure one record */
   /* and increment head pointer */
   
   spoofrecordp = spoofheadpointer;
   spoofheadpointer = spoofrecordp->chain;
   spoofrecordp->chain = NULL;
   return spoofrecordp;
}/* nov_spx_getspoofrecord */

/* novell_returnspoofrecord
 *
 * this routine returns a spoof record to the available pool
 */

void  novell_returnspoofrecord (nov_spx_spoof_record  *spoofrecordp)
{
    /* routine to return an unused spoof record to the list of available
     * records, careful when taking records out of the hash table to
     *  update the table before returning the record!
     */    
    spoofrecordp->chain = NULL;
    if (spoofheadpointer == NULL) {
	spoofheadpointer = spooftailpointer = spoofrecordp;
    } else {
        spoofrecordp->destnet = 0;
        spoofrecordp->destsocket = 0;
	TIMER_STOP(spoofrecordp->timelastchanged);
	TIMER_STOP(spoofrecordp->timelastspoofed);
	TIMER_STOP(spoofrecordp->timeadded);
	spooftailpointer->chain = spoofrecordp;
	spooftailpointer = spoofrecordp;
    }
    return;
    
}/* novell_returnspoofrecord */

/* novell_spx_initializepointers
 *
 * Initialize spx spoof table pointers.
 */
static void  novell_spx_initializepointers (void)
{

    int i;

    spoofheadpointer = spooftailpointer = NULL;

    for (i = 0 ; i < SPOOF_HASH_TABLE_ENTRIES; i++ ) {
      spoofhashtable[i] = NULL;
    }
    return;
} /* novell_spx_initializepointers */


/*
 * novell_clear_spx_spoof
 * Clear out Novell spx spoof table data
 *
 */

void novell_clear_spx_spoof (void)
{
    uchar hashindex;
    nov_spx_spoof_record *spoofrecordp;
    nov_spx_spoof_record *nextp;

    for (hashindex = 0; hashindex < SPOOF_HASH_TABLE_ENTRIES; hashindex++) {
	spoofrecordp = spoofhashtable[hashindex];
	spoofhashtable[hashindex] = NULL;
	while (spoofrecordp != NULL ) {
	    nextp = spoofrecordp->chain;
	    novell_returnspoofrecord(spoofrecordp);
	    spoofrecordp = nextp;
	}
    } 
}


/*
 * nov_spx_spoof
 * Handle a spx spoofing and spx session tracking.
 *
 * return TRUE if we did spoof and the packet should be dropped by caller
 * return FALSE if we did not spoof and the packet should continue on thru
 *    normal send process
 */

static boolean
nov_spx_spoof (paktype *pak, uchar *dhost)
{
    uchar hashindex;
    nov_spx_spoof_record *spoofrecordp, *prevrecordp=NULL;
    ushort foundit=FALSE;
    ipxhdrtype *ipx, *newipx;
    nov_spx_header *ipxspx, *newspx;
    paktype *newpak;
    addrtype proto_addr;
    boolean link_status;

    if (nov_spx_initialize) {
	novell_spx_initializepointers();
	GET_TIMESTAMP(novell_spxagertimer);
	nov_spx_initialize = FALSE;
    }

    ipx = (ipxhdrtype *)ipxheadstart(pak);
    ipxspx = (nov_spx_header *)ipxheadstart(pak);

    hashindex = (nov_spx_hashaddress((uchar *) (ipxspx->ddnet),
				     (uchar *) (ipxspx->dhost),
				     (uchar *) &(ipxspx->dsock),
				     (uchar *) &(ipxspx->destconnectid)) ^
		 nov_spx_hashaddress((uchar *) (ipxspx->ssnet),
				     (uchar *) (ipxspx->shost),
				     (uchar *) &(ipxspx->ssock),
				     (uchar *) &(ipxspx->sourceconnectid)));
    
    spoofrecordp = spoofhashtable[hashindex];
    /* At this point we have an SPX packet arriving or going to an interface
     * with spx spoofing enabled. First print out a debug message
     */
    if (novell_spoof_debug) {
	print_packet(pak, "SPX:");
	buginf(" %x %x %x %x %x %x %x", ipxspx->connectionctl,
	       ipxspx->datastreamtype,ipxspx->sourceconnectid,
	       ipxspx->destconnectid, ipxspx->sequencenumber,
	       ipxspx->acknumber,ipxspx->allocnumber);
    }
	    
    if (pak->nif_output->novell_spoof_flag & NOVELL_S_SPOOF) {
 	if ((ipx->len == NOVELL_SPX_KEEPALIVE_LEN) || 
 	    ((ipx->len == NOVELL_SPX2_KEEPALIVE_LEN) && 
 	     (ipxspx->sequencenumber == 0) && 
 	     (ipxspx->connectionctl & NOVELL_SPX_CCID))) {
	    while ((spoofrecordp != NULL ) && !foundit ) {
		/* try to find a match */
		if((spoofrecordp->destnet == GET2WD(ipxspx->ssnet)) &&
		   ieee_equal(spoofrecordp->destnode, ipxspx->shost) &&
		   (spoofrecordp->destsocket == ipxspx->ssock) &&
		   (spoofrecordp->destconnectid == ipxspx->sourceconnectid) &&
		   (spoofrecordp->sourcenet == GET2WD(ipxspx->ddnet)) &&
		   ieee_equal(spoofrecordp->sourcenode, ipxspx->dhost) &&
		   (spoofrecordp->sourcesocket == ipxspx->dsock) &&
		   (spoofrecordp->sourceconnectid == ipxspx->destconnectid))
		    foundit = TRUE;
		else
		    spoofrecordp = spoofrecordp->chain;
	    }
	    if (!foundit) {	/* send packet */
	        if (novell_spoof_debug) buginf(" (not in table)");
		return(FALSE);
	    }
	    if ((spoofrecordp->rcvcount <= 1) ||
		(spoofrecordp->sndcount <= 1)) {
	        if (novell_spoof_debug) buginf(" (early)");
		/* not enough connectivity yet allow packet thru */
		spoofrecordp->sndcount++;
		return(FALSE);
	    }
	    if (CLOCK_OUTSIDE_INTERVAL(spoofrecordp->timelastchanged, 
				       pak->nif_output->novell_spxidletime
				       * ONESEC)) {
	        if (novell_spoof_debug)
		  buginf(" (Last Changed %d sec)",
			 ELAPSED_TIME(spoofrecordp->timelastchanged)/ONESEC);
		/* Don't spoof more than once per second per session */
		/* This is to prevent a spoofing flood/meltdown      */
		if (CLOCK_IN_INTERVAL(spoofrecordp->timelastspoofed, ONESEC)) {
		    if (novell_spoof_debug)
			buginf(" skip "); 
		    return(TRUE);
		}
		/* If this is DDR and the link is up send the packet */
		/* but tell the dialer it is uninteresting. */
		if (is_ddr(pak->if_output->hwptr)) {
		  reg_invoke_proto_address(LINK_NOVELL, pak->if_output, pak, 
					   &proto_addr);
		  ieee_copy(dhost, proto_addr.nov_addr.host);
		  proto_addr.nov_addr.net = ipx_nexthop_network(pak);
		  link_status = reg_invoke_dialer_connected(pak->if_output->hwptr,
						  &proto_addr, LINK_NOVELL);
		  if (link_status) {
		    pak->dialer_flags |= PAK_DDR_UNINTERESTING;
		    if (novell_spoof_debug) buginf(" (let through)");
		    return(FALSE);		  
		  }
		  if (novell_spoof_debug) buginf(" (link status %d)", 
						 link_status);
		}
		/* Now build a packet to spoof the sender */
		newpak = pak_duplicate(pak);
		if (!(newpak == NULL)) {
		    newipx = (ipxhdrtype *)ipxheadstart(newpak);
		    newpak->if_output = pak->if_input;
		    newpak->nif_output = pak->nif_input;
		    PUT2WD(newipx->ssnet, GET2WD(ipx->ddnet));
		    ieee_copy(ipx->shost, newipx->dhost);
		    newipx->ssock = ipx->dsock;
		    newipx->dsock = ipx->ssock;
		    ieee_copy(ipx->dhost, newipx->shost);
		    PUT2WD(newipx->ddnet, GET2WD(ipx->ssnet));
			    
		    newipx->tc = spoofrecordp->transportctl;
		    newspx = (nov_spx_header *) newpak->network_start;
		    newspx->connectionctl = (uchar) 0x80;
		    newspx->sourceconnectid = ipxspx->destconnectid;
		    newspx->destconnectid = ipxspx->sourceconnectid;
		    if (spoofrecordp->sequencenumber)
		      newspx->sequencenumber = spoofrecordp->sequencenumber;
		    newspx->acknumber = spoofrecordp->acknumber;
		    newspx->allocnumber = spoofrecordp->allocnumber;
		    newpak->nif_output = NULL;
		    clear_if_input(newpak);
		    if (novell_spoof_debug) {
			print_packet(newpak, "spx keepalive sent");
			buginf(" %x %x %x %x %x %x %x", newspx->connectionctl,
			       newspx->datastreamtype,newspx->sourceconnectid,
			       newspx->destconnectid, newspx->sequencenumber,
			       newspx->acknumber,newspx->allocnumber);
		    }
		    if (nov_send(newpak)) {
			novell_stat[NOVELL_WATCHDOG_OUT]++;
			spoofrecordp->sndcount++;
			GET_TIMESTAMP(spoofrecordp->timelastspoofed);
			return(TRUE);
		    } else {
			if (novell_spoof_debug)
			    buginf("\n%%IPX: SPX could not send keepalive");
			spoofrecordp->sndcount++;
			return(FALSE);
		    }
		}
	    } else {		/* clock hasn't expired yet */
	      if ((ipxspx->sequencenumber) &&
		  ((spoofrecordp->sequencenumber != ipxspx->acknumber) ||
		   (spoofrecordp->acknumber != ipxspx->sequencenumber))) {
		spoofrecordp->sequencenumber = ipxspx->acknumber;
		if (spoofrecordp->sequencenumber)
		  spoofrecordp->acknumber = ipxspx->sequencenumber;
		GET_TIMESTAMP(spoofrecordp->timelastchanged);
	      }
	      if (novell_spoof_debug) 
		buginf(" Last Changed %d sec",
		       ELAPSED_TIME(spoofrecordp->timelastchanged)/ONESEC);
	      spoofrecordp->sndcount++;
	      return(FALSE);
	    }
	}
	return(FALSE);	/* Pak will be returned by caller */
    } else if (pak->nif_input->novell_spoof_flag & NOVELL_S_SPOOF) {
	if (ipxspx->destconnectid != SPX_HANDSHAKE_CONNECT_ID) {
	    while((spoofrecordp != NULL ) && !foundit ) {
		/* try to find a match */
		if((spoofrecordp->destnet == GET2WD(ipxspx->ddnet)) &&
		   ieee_equal(spoofrecordp->destnode, ipxspx->dhost) &&
		   (spoofrecordp->destsocket == ipxspx->dsock) &&
		   (spoofrecordp->destconnectid == ipxspx->destconnectid) &&
		   (spoofrecordp->sourcenet == GET2WD(ipxspx->ssnet)) &&
		   ieee_equal(spoofrecordp->sourcenode, ipxspx->shost) &&
		   (spoofrecordp->sourcesocket == ipxspx->ssock) &&
		   (spoofrecordp->sourceconnectid == ipxspx->sourceconnectid))
		    foundit = TRUE;
		else {
		    prevrecordp = spoofrecordp;
		    spoofrecordp = spoofrecordp->chain;
		}
	    }
	    if ((ipxspx->datastreamtype == SPX_END_OF_CONNECTION) ||
		(ipxspx->datastreamtype == SPX_END_OF_CONNECTION_ACK)) {
		if (foundit) {
		    if (novell_spoof_debug) buginf(" (end of session)");
		    if (prevrecordp == NULL) {
			spoofhashtable[hashindex] = spoofhashtable[hashindex]->chain;
		    } else {
			prevrecordp->chain = spoofrecordp->chain;
		    }
		    novell_returnspoofrecord(spoofrecordp);
		}
	    } else {
		if (!foundit) {  /* Create a new record */
		    if (novell_spoof_debug) buginf(" (new)");
		    spoofrecordp = nov_spx_getspoofrecord();
		    if (spoofrecordp == NULL) { /* Failed to get memory */
			return(FALSE);
		    }
		    spoofrecordp->chain = spoofhashtable[hashindex];
		    spoofhashtable[hashindex] = spoofrecordp;
		    /* Add data to record */
		    spoofrecordp->rcvcount = 0;
		    spoofrecordp->sndcount = 0;
		    spoofrecordp->transportctl = ipxspx->tc;
		    spoofrecordp->destnet = GET2WD(ipxspx->ddnet);
		    ieee_copy(ipxspx->dhost, spoofrecordp->destnode);
		    spoofrecordp->destsocket = ipxspx->dsock;
		    spoofrecordp->destconnectid = ipxspx->destconnectid;
		    spoofrecordp->sourcenet = GET2WD(ipxspx->ssnet);
		    ieee_copy(ipxspx->shost, spoofrecordp->sourcenode);
		    spoofrecordp->sourcesocket = ipxspx->ssock;
		    spoofrecordp->sourceconnectid = ipxspx->sourceconnectid;
		    GET_TIMESTAMP(spoofrecordp->timeadded);
		    GET_TIMESTAMP(spoofrecordp->timelastchanged);
		    GET_TIMESTAMP(spoofrecordp->timelastspoofed);
		}
	        if (novell_spoof_debug) buginf(" (changed:");
		if ((spoofrecordp->sequencenumber != ipxspx->sequencenumber) ||
		    (spoofrecordp->acknumber != ipxspx->acknumber)) {
		    spoofrecordp->sequencenumber = ipxspx->sequencenumber;
		    spoofrecordp->acknumber = ipxspx->acknumber;
		    GET_TIMESTAMP(spoofrecordp->timelastchanged);
	            if (novell_spoof_debug) buginf("yes)");
		} else {
		  /*
		   * Got an SPX spoof packet after we've been spoofing
		   * someone must have cleared the table on the other side
		   * reset the timestamp so we stop spoofing for a while.
		   * Unless this is DDR as we let packets thru when the
		   * link is up, but classify them UNINTERESTING
		   */
		   if (!is_ddr(pak->if_input->hwptr)) {
		       if (CLOCK_OUTSIDE_INTERVAL(spoofrecordp->timelastchanged,
						  2 *
						  pak->nif_input->novell_spxidletime
						  * ONESEC)) {
			   GET_TIMESTAMP(spoofrecordp->timelastchanged);
			   if (novell_spoof_debug) buginf("clear)");
		       } else {
			   if ((ipx->len > NOVELL_SPX_KEEPALIVE_LEN) ||
			       (((ipx->len > NOVELL_SPX2_KEEPALIVE_LEN) ||
				 (ipxspx->sequencenumber != 0)) &&
				(ipxspx->connectionctl & NOVELL_SPX_CCID))) {
			       GET_TIMESTAMP(spoofrecordp->timelastchanged);
			       if (novell_spoof_debug) buginf("SYS:yes)");
			   } else {
			       if (novell_spoof_debug) buginf("no)");
			   }
		       }
		   } else {
		       if ((ipx->len > NOVELL_SPX_KEEPALIVE_LEN) ||
			   (((ipx->len > NOVELL_SPX2_KEEPALIVE_LEN) ||
			     (ipxspx->sequencenumber != 0)) &&
			    (ipxspx->connectionctl & NOVELL_SPX_CCID))) {
			   GET_TIMESTAMP(spoofrecordp->timelastchanged);
			   if (novell_spoof_debug) buginf("SYS:yes (up))");
		       } else {
			   if (novell_spoof_debug) buginf("no (up))");
		       }
		   }
	       }
		spoofrecordp->allocnumber = ipxspx->allocnumber;
		spoofrecordp->rcvcount++;
 	        if (novell_spoof_debug) 
		    buginf(" Last Changed %d",
			   ELAPSED_TIME(spoofrecordp->timelastchanged)/ONESEC);
	    }
	}
    }
    return(FALSE);
}



/*
 * nov_input_teardown
 *
 * This process is being killed.  Remove any data structures that this
 * process set up when it started.
 */

static void nov_input_teardown (int signal, int dummy1, void *dummy2, char *dummy3)
{
    paktype *pak;

    reg_delete_raw_enqueue(LINK_NOVELL);
    process_watch_queue(novQ, DISABLE, RECURRING);
    while ((pak = process_dequeue(novQ)))
	retbuffer(pak);
    delete_watched_queue(&novQ);
    novell_inp_pid = 0;
}


/*
 * nov_input
 * All of Novell begins here
 */

process nov_input (void)
{
    int error, which, pak_count, len;
    paktype *pak;
    ipxhdrtype *ipx;
    nidbtype *nidb;
    novell_sappacket *sapp;

    process_wait_on_system_init();

    /*
     * Set up this process' world.
     */
    signal_permanent(SIGEXIT, nov_input_teardown);
    novQ = create_watched_queue("IPX Input", 0, 0);
    process_watch_queue(novQ, ENABLE, RECURRING);
    reg_add_raw_enqueue(LINK_NOVELL, novell_enqueue, "novell_enqueue");

    while (TRUE) {
	process_wait_for_event();

	pak_count = PAK_SWITCHCOUNT;

	while (TRUE) {
	    /*
 	     * Switch only a limited number of packets per pass
	     */
	    if (pak_count-- == 0) {
		break;
	    }

	    /*
	     * Dequeue a Novell datagram
	     */
	    pak = process_dequeue(novQ);
	    if (!pak)
		break;
	    ipx = (ipxhdrtype *)ipxheadstart(pak);

 	    /*
 	     * Possibly decompress header of packet. We need a nif_input for
 	     * this code, but we don't want one later in this function, so
 	     * we set it, and if set, clear it, in this code.
 	     */
 	    if (pak->if_input != NULL) {
 		pak->nif_input = IPX_PRIMARY_NIDB(pak->if_input);
 		if (pak->nif_input != NULL) {
 		    pak = reg_invoke_ipx_compression_decompress_header(pak);
 		    if (pak == NULL) {
 		        continue;
 		    }
 		    pak->nif_input = NULL;
 		}
 	    }
	    /*
	     * Discard up front if we are going down:
	     */
	    if (!novell_running || novell_shutting_down) {
#ifdef DEBUG_ENCAPS
		print_novell_encap(pak, pak->if_input, "not running");
#endif
		nov_dispose(NOVELL_FORMATERROR, pak);
		continue;		
	    }

	    if (!nov_set_nidb_idb(pak))
	      continue;

	    nidb = (nidbtype *) pak->nif_input;

	    if (novell_debug) {
		print_packet(pak, "rcvd");
#ifdef DEBUG_ENCAPS
		print_novell_encap(pak, pak->if_input, "  encap");
#endif
	    }
	    novell_stat[NOVELL_INPUT]++;
	    idb_increment_rx_proto_counts(pak, ACCT_PROTO_NOVELL);

	    len = GETSHORT(&ipx->len);
	    if (len & 1)
		len++;			/* odd packets preserve garbage */

	    /*
	     * Sanity check length 
	     */
	    if ((len < XNS_MINSIZE) ||
		(len > pak->if_input->hwptr->maxmtu) ||
		(GETSHORT(&ipx->len) > ((ulong) pak->datagramsize - 
		    ((ulong) ipx - (ulong) pak->datagramstart)))) {
		if (novell_debug) {
		    print_packet(pak, "bad len");
		}
		nov_format_dispose(pak);
		continue;
	    }

	    /*
	     * Check checksum for validity
	     */
	    if (GETSHORT(&ipx->cksum) != NOVELL_NO_CHECKSUM) {
		if (nov_forme(pak) && ((GETSHORT(&ipx->dsock) != NOVELL_SOCK_ECHO) &&
				       (GETSHORT(&ipx->dsock) != NOVELL_SOCK_ECHO_STND))) {
		    error = XNS_ERR_BADSUM;
		    nov_error(pak, error, 0, NOVELL_CKSUMERR);
		    continue;
		}
	    }

	    /*
	     * Check input access list to see if we should drop it:
	     */
 	    if (pak->if_input->novell_inaccesslist) {
 		if (!nov_accesscheck
 		    (pak, pak->if_input->novell_inaccesslist)) {
 		    if (novell_debug)
 			print_packet(pak, "*denied (in)*");
 		    nov_dispose(NOVELL_FILTER_IN, pak);
 		    continue;
 		}
 	    }

	    /*
	     * Is this packet aimed at us, or should it be forwarded, or
	     * pitched?
	     */
	    which = nov_forus(pak);

	    /*
	     * If the hopcount indicates the packet was originated on this
	     * particular input network (i.e., no hops), then we can 'glean'
	     * the source media level address for the source of the packet
	     * from the media header in the packet. Do so.
             */
	    if ((ipx->tc == 0) && (which != NOVELL_PITCHIT)) {
		reg_invoke_media_glean_source_address(pak->enctype, pak);
	    }
	    
	    /*
	     * If state is IPXWAN_STARTING, only accept packets to the
	     * IPXWAN port and packets that are meant for US.
	     */
	    if (NOVELL_NIDB_ISIPXWAN_STARTING(nidb)) {
		if ((GETSHORT(&ipx->dsock) != NOVELL_SOCK_IPXWAN) ||
		    (which != NOVELL_CONSUMEIT)) {
		    if (novell_debug)
		      print_packet(pak, "IPXWAN not started");
		    nov_dispose(NOVELL_FORMATERROR, pak);
		    continue;
		}
	    }

 	    /*
 	     * Send away SPX packets. If not consumed, we will fall
	     * through!
 	     */
 	    if ((NOVELL_TYPE_SPX == ipx->pt) &&
 		(NOVELL_CONSUMEIT == which)) {
 		if (reg_invoke_ipx_input_paktype(ipx->pt, pak)) {
		    novell_stat[NOVELL_LOCALDEST]++;
		    continue;
		}
 	    }
 
	    switch (which) {
	      case NOVELL_CONSUMEIT:
		  if (novell_debug)
		      print_packet(pak, "local");
		  novell_stat[NOVELL_LOCALDEST]++;

		  switch (GETSHORT(&ipx->dsock)) {
		    case NOVELL_SOCK_SAP:
		      reg_invoke_snapshot_update_received(pak->if_input,
							  (paktype *) pak);
		      sapp = (novell_sappacket *)(XNSDATASTART(pak));
		      if (GETSHORT(&sapp->response_type) ==
			  NOVELL_SAP_NEARQUERY) {
			  /* Don't respond if we shouldn't */
			  if (!pak->if_input->nov_respond_to_gns) {
			      if (novell_debug)
				  print_packet(pak, "GNS disabled, toss");
			      datagram_done(pak);
			      novell_stat[NOVELL_PITCHED]++;
			  } else {
			      enqueue(&novell_gnsQ, pak);
			  }
		      } else {
			  p_enqueue(&novbgQ, pak);
		      }
		      break;

                    case NOVELL_SOCK_IGRP:
		      if (novigrp_enabled) {
			  reg_invoke_novigrp_process(pak);
		      } else {
			  if (novell_debug)
			      print_packet(pak, "IPX EIGRP disabled, toss");
			  datagram_done(pak);
			  novell_stat[NOVELL_PITCHED]++;
		      }
		      break;

		    case NOVELL_SOCK_RIP:
		      reg_invoke_snapshot_update_received(pak->if_input,
							  (paktype *) pak);
		      if (novell_rip_running) {
			  p_enqueue(&novrip_pdb.inputq, pak);
		      } else {
			  if (novell_debug)
			      print_packet(pak, "RIP disabled, toss");
			  datagram_done(pak);
			  novell_stat[NOVELL_PITCHED]++;
		      }
		      break;

		    case NOVELL_SOCK_IPXWAN:
		      if (ipxwan_enabled) {
			  p_enqueue(&novell_ipxwanQ, pak);
		      } else {
			  if (novell_debug)
			      print_packet(pak, "IPXWAN not present, toss");
			  datagram_done(pak);
			  novell_stat[NOVELL_PITCHED]++;
		      }
		      break;

		    case NOVELL_SOCK_SNMP:
		      if (novell_snmp_pid != 0) {
			  process_enqueue_pak(novell_snmpQ, pak);
		      } else {
			  if (novell_debug)
			      print_packet(pak, "SNMP disabled, toss");
			  datagram_done(pak);
			  novell_stat[NOVELL_PITCHED]++;
		      }
		      break;

  		    case NOVELL_SOCK_NLSP:
		      if (novell_nlsp_enabled) {
			  reg_invoke_ipx_nlsp_process_packet(pak);
		      } else {
			  if (novell_debug)
			      print_packet(pak, "NLSP disabled, toss");
			  datagram_done(pak);
			  novell_stat[NOVELL_PITCHED]++;
		      }
		      break;

		    default:
		      p_enqueue(&novell_genericQ, pak);
		      break;

		  }
		  break;
	      case NOVELL_PITCHIT:
		  if (novell_debug)
		      print_packet(pak, "bad pkt");
		  novell_stat[NOVELL_PITCHED]++;
		  retbuffer(pak);
		  break;
	      case NOVELL_ROUTEIT:
		  nov_forward(pak);
		  break;
	      }
	}
    }
}

/*
 * nov_forme
 * Determine if a packet is meant for one this hosts interfaces.
 */

boolean nov_forme (paktype *pak)
{
    ipxhdrtype *ipx;
    ulong dnet;
    nidbtype *nidb;

    ipx = (ipxhdrtype *)ipxheadstart(pak);

    dnet = GET2WD(ipx->ddnet);
    FOR_ALL_NIDBS(nidb) {
      if (dnet != 0L && dnet != -1L && dnet != nidb->novellnet) 
          continue; 
      if (ieee_equal(ipx->dhost, nidb->idb->novell_node)) 
          return(TRUE); 
    } 
    nidb = pak->nif_input;
    if (((dnet == 0L) || (dnet == -1L) || (dnet == nidb->novellnet)) &&
	ieee_equal(ipx->dhost, nidb->idb->novell_node))
	return(TRUE);
    else
	return(FALSE);
}

/*
 * nov_netbios_buginf
 *
 * Print a buginf summary of a NETBIOS Type 20 Propagation Packet
 *
 */
void nov_netbios_buginf (paktype *pak, char *reason)
{
    ipxhdrtype *ipx;
    novellnetbiosheader *netbios;
    int hops = 0;
    int i;

    if (!novell_debug)
	return;

    ipx = (ipxhdrtype *)ipxheadstart(pak);
    netbios = (novellnetbiosheader *)ipxdatastart(pak);

    buginf("\nIPX: src=%x.%e, dst=%x.%e, tc=%-02x, NETBIOS pkt (%s)",
	   GET2WD(ipx->ssnet),
	   ipx->shost,
	   GET2WD(ipx->ddnet),
	   ipx->dhost,
	   ipx->tc,
	   reason);
    hops = ipx->tc;
    buginf("\nNETBIOS net list:");
    for (i = 0; (i < min(hops, NOVELL_NETBIOS_NUM_NETS)); i++)
	buginf("->%x", GETLONG(&netbios->nov_netbios_nets[i]));
}


/*
 * nov_forus
 * Determine if a packet is meant for this host
 */

int nov_forus (paktype *pak)
{
    idbtype *idb; 
    hwidbtype *hwidb;
    nidbtype *nidb; 
    novndbtype *p;
    int i;
    ulong dnet, snet;
    int hops;
    ipxhdrtype *ipx;
    novellnetbiosheader *netbios;
    novell_sappacket *sapp;
    boolean type20;
    boolean netbios_socket;
    ushort dsock, rsptyp;

    ipx = (ipxhdrtype *)ipxheadstart(pak);

    idb = pak->if_input;
    hwidb = idb->hwptr;
    nidb = pak->nif_input;

    
    dnet = GET2WD(ipx->ddnet);
    snet = GET2WD(ipx->ssnet);
    dsock = GETSHORT(&ipx->dsock);
    type20 = ((ipx->pt == NOVELL_TYPE_NETBIOS) &&
	      ieee_equal(ipx->dhost, novell_allhosts));
    netbios_socket = (novell_netbios_in_checks && !type20 &&
		      (dsock == NOVELL_SOCK_NETBIOS) &&
		      ieee_equal(ipx->dhost, novell_allhosts));
    /*
     * Look for all-nets, NETBIOS socket bc, and type 20 bc:
     */
    if ((dnet == -1L) || type20 || netbios_socket) {
	/*
	 * Only hear all nets on the primary interface. Apply to type 20
	 * or other netbios only if enabled.
	 */
	if ((dnet == -1) ||
	    (type20 && (novell_type20_in_checks || novell_type20_helpered)) ||
	    netbios_socket) {

	    p = novrip_find_rt(snet, NOV_NETMASK_ALL, IPX_ROUTE_MATCH_FUZZ);

	    if ((p != NULL) &&
		(p->path_list != NULL) &&
		(p->path_list->nidb != NULL) &&
		(p->path_list->nidb->idb != NULL) &&
		interface_up(p->path_list->nidb->idb) && 
		(nidb != p->path_list->nidb))
		return(NOVELL_PITCHIT);
	}
	/*
	 * IPX type 20 propagation packet special handling:
	 */
	if (type20 && !novell_type20_helpered) {
	    if (!idb->novell_type20)
		return(NOVELL_PITCHIT);
            hops = ipx->tc;
	    if (hops >= NOVELL_NETBIOS_NUM_NETS) {
		if (novell_debug)
		    nov_netbios_buginf( pak, "hop count exceeded");
		return(NOVELL_PITCHIT);
	    } else {
		if (novell_debug)
		    nov_netbios_buginf( pak, "inbound");
	    }
	    /*
	     * A well-formed type 20 packet must include space for
	     * 8 network number entries:
	     */
	    if (GETSHORT(&ipx->len) - XNSHEADERBYTES - 
		(NOVELL_NETBIOS_NUM_NETS*4) < 0) {
		if (novell_debug)
		    nov_netbios_buginf( pak, "packet not big enough");
		return(NOVELL_PITCHIT);
	    }
	    netbios = (novellnetbiosheader *)ipxdatastart(pak);
	    /*
	     * A well-formed type 20 packet has unused network number
	     * entries set to 0. However, do not fail if a violation
	     * is found.
	     */
	    for (i = hops; (i < NOVELL_NETBIOS_NUM_NETS); i++)
		if (GETLONG(&netbios->nov_netbios_nets[i])) {
		    if (novell_debug)
			nov_netbios_buginf( pak, "network entry non-zero");
		    break;
		}
	    /*
	     * Input net loop check. Discard packet if it has
	     * already traversed this network:
	     */
	    for (i=0; (i < hops); i++)
		if (nidb->novellnet == 
 		    GETLONG(&netbios->nov_netbios_nets[i])) {
		    if (novell_debug)
			nov_netbios_buginf( pak, "loop detected on input");
		    return(NOVELL_PITCHIT);
		}
	    /* Check for input filters */
	    if ((type20) && (nidb != NULL) && (nidb->idb != NULL)) {
	      /* Filter according to any NetBIOS lists */
	      if (!novell_netbios_acfcheck(pak,
					   IPX_ACCESS_DIRECTION_INCOMING,
					   nidb)) {
		novell_stat[NOVELL_FILTER_IN]++;
		if (novell_debug)
		  nov_netbios_buginf(pak, "inbound filtered");
		return(NOVELL_PITCHIT);
	      }
	    }
	    /*
	     * Record input net:
	     */
	    PUTLONG(&netbios->nov_netbios_nets[hops],(nidb->novellnet ?
						      nidb->novellnet :
						      ipx_internal_net));
	}
    }

    if (nov_multicast_addr(hwidb, ipx->dhost[0])) {
	/*
	 * Forward broadcast if the destination network is not
	 * broadcast or unspecified and the source network is not
	 * unspecified and the destination network does not
	 * match the input interface network.
	 * and does not match our Internal IPX network.
	 */
	if ((dnet != -1L) && (dnet != 0L) && (snet != 0L)
	    && (nidb->novellnet != dnet) && (dnet != ipx_internal_net)) {
	    /*
	     * Don't ever forward RIP, SAP or EIGRP packets
	     */
	    if ((dsock == NOVELL_SOCK_RIP) ||
		(dsock == NOVELL_SOCK_SAP) ||
		(dsock == NOVELL_SOCK_IGRP)) {
		return(NOVELL_PITCHIT);
	    }
	    return(NOVELL_ROUTEIT);
	}
	novell_stat[NOVELL_BCASTIN]++;
	/*
	 * Ignore EIGRP updates with a hopcount > 0.
	 * Allows queries with tc > 0.
	 * Allow RIP/SAP updates with tc of 1  (Lotus Notes OS/2 Server).
	 * Ignore RIP/SAP broadcasts with tc > 1.
	 */
	hops = ipx->tc;
	sapp = (novell_sappacket *)(XNSDATASTART(pak));
	rsptyp = GETSHORT(&sapp->response_type);

	if (((hops > 1) &&
	    (((dsock == NOVELL_SOCK_RIP) &&
	      (rsptyp == RIP_REPLY)) ||
	     ((dsock == NOVELL_SOCK_SAP) &&
	      (rsptyp == NOVELL_SAP_GENRESPONSE)))) ||
	     ((hops != 0) && (dsock == NOVELL_SOCK_IGRP))) {
	    return(NOVELL_PITCHIT);
	} else {
	  if ((hops <= 1) && ((snet != 0L) && (snet != nidb->novellnet)) &&
	      (((dsock == NOVELL_SOCK_RIP) &&
		(rsptyp == RIP_REPLY)) ||
	       ((dsock == NOVELL_SOCK_SAP) &&
		(rsptyp == NOVELL_SAP_GENRESPONSE)))) {
	    return(NOVELL_PITCHIT);
	  }
	}
        /* Pitch helpered packets if helpering is not enabled */
	if (!idb->novell_helperlist) {
	    if (type20) {
		if (novell_type20_helpered) {
		    if (novell_debug)
			buginf("\nIPX: unable to forward, no helper %x.%e(%x) to %x.%e(%x) typ %x",
			       GET2WD(ipx->ssnet), ipx->shost, GETSHORT(&ipx->ssock),
			       GET2WD(ipx->ddnet), ipx->dhost, dsock,
			       ipx->pt);
		    return(NOVELL_PITCHIT);
		}
	    } else if ((dsock != NOVELL_SOCK_RIP) &&
		     (dsock != NOVELL_SOCK_SAP) &&
		     (dsock != NOVELL_SOCK_IGRP) &&
		     (dsock != NOVELL_SOCK_NLSP) &&
		     (dsock != NOVELL_SOCK_IPXWAN) &&
		     (dsock != NOVELL_SOCK_SNMP) &&
		     (dsock != NOVELL_SOCK_NCS_SRV) &&
		     (dsock != NOVELL_SOCK_NLSP) &&
		     (dsock != NOVELL_SOCK_ECHO_STND) &&
		     (dsock != NOVELL_SOCK_ECHO)) {
		if (novell_debug)
		    buginf("\nIPX: unable to forward, no helper %x.%e(%x) to %x.%e(%x) typ %x",
			   GET2WD(ipx->ssnet), ipx->shost, GETSHORT(&ipx->ssock),
			   GET2WD(ipx->ddnet), ipx->dhost, dsock,
			   ipx->pt);
		return(NOVELL_PITCHIT);
	    }
	}
	/*
	 * Ignore our own broadcasts
	 */
	if (ieee_equal(idb->novell_node, ipx->shost))
	    return(NOVELL_PITCHIT);
        /*
         * Ignore packets from misconfigured clients/servers
         * broadcasts must be to network 0, or nidb network number.
         */
/* %%%        if ((hops == 0) && ((snet != 0L) && (snet != nidb->novellnet)))
          return(NOVELL_PITCHIT); */

	pak->flags |= PAK_BROADCAST;
	return(NOVELL_CONSUMEIT);
    }
    if (nov_forme(pak))
	return(NOVELL_CONSUMEIT);
    return(NOVELL_ROUTEIT);
}

/*
 * nov_type20_output_loop
 */
boolean nov_type20_output_loop (paktype *pak, ulong dnet)
{
    ipxhdrtype *ipx;
    novellnetbiosheader *netbios;
    int i;
    ushort hops;
    
    ipx = (ipxhdrtype *)ipxheadstart(pak);
    netbios = (novellnetbiosheader *)ipxdatastart(pak);

    /*
     * IPX Type 20 Propagation Packet (NETBIOS) output screening.
     * Check output network list for possible loop.
     */
    hops = ipx->tc;
    hops += 1;
    for (i=0; (i < min(hops, NOVELL_NETBIOS_NUM_NETS)); i++)
	if (dnet == GETLONG(&netbios->nov_netbios_nets[i])) {
	    nov_netbios_buginf(pak,"loop detected on output");
	    return(TRUE);
	}
    return(FALSE);
}

/*
 * nov_forward
 * Forward a Novell datagram
 */

void nov_forward (paktype *pak)
{
    ipxhdrtype *ipx;
    idbtype *idb;
    nidbtype *nidb;
    ushort hops;

    ipx = (ipxhdrtype *)ipxheadstart(pak);
    idb = pak->if_input;
    nidb = pak->nif_input;

    /*
     * Check inbound hop count. If old style, drop the packet after
     * 15 hops (at the 16th router). If novell algorithms is in use, drop
     * packets after either 16 hops or 255 hops. Note that as a result
     * of ecentricities in the hop count algorithm, ultimately derived
     * from the requirements of Novell certification, any hop count in
     * the range 16 .. 31 will be dropped. Also note that type 20 hop
     * count screening has already been done by forus().
     */
    hops = ipx->tc;
    if (NOVELL_IGRP_HOPCOUNT_ALGORITHM_ALLOWED) {
	if ((hops <= XNS_MAX_HOPS) ||
	    ((hops >= XNS_IGRP_MIN_HOPS) && (hops < XNS_IGRP_MAX_HOPS))) {
	    ;
	} else {
	    if (novell_debug) {
		print_packet(pak, "max tc");
		buginf(" (tc=%-02x)", ipx->tc);
	    }
	    nov_error(pak, XNS_ERR_2OLD, 0, NOVELL_HOPCOUNT);
	    return;
	}
    } else {
	if (hops > novell_max_hops) {
	    if (novell_debug) {
		print_packet(pak, "max tc");
		buginf(" (tc=%-02x)", ipx->tc);
	    }
	    nov_error(pak, XNS_ERR_2OLD, 0, NOVELL_HOPCOUNT);
	    return;
	}
    }
    
    /*
     * Adjust hop count of helpered packets here. There are 3 different
     * cases ... type 20 propagation packets, other flooded packets,
     * and other helpered packets. Note that in all cases, pak->if_input
     * is NULL, while pak->ddnet has been set. Type 20 (and other flooded
     * packets) also have pak->nif_output set.
     *
     * Type 20 helpered packets have already been output screened by
     * nov_flood(). Note that at the present time, type 20 packets
     * for remote destination networks are routed, not helpered.
     *
     */
    if ((ipx->pt == NOVELL_TYPE_NETBIOS) &&
	ieee_equal(ipx->dhost, novell_allhosts) &&
	(!novell_type20_helpered)) {
	novell_stat[NOVELL_NETBIOSIN]++;
	/*
	 * IPX Type 20 Propagation Packet (NETBIOS) special handling.
	 * Check output network for possible loop and adjust hop count.
	 */
	if (idb != NULL)
	    if (nov_type20_output_loop(pak, GET2WD(ipx->ddnet))) {
		nov_error(pak, XNS_ERR_2OLD, 0, NOVELL_HOPCOUNT);
		return;
	    }
	hops = ipx->tc;
	hops += 1;
	ipx->tc = hops;
	if (novell_debug)
	    nov_netbios_buginf(pak, "outbound");
    } else if (idb == NULL) {
	/*
	 * Handle other helpered packets:
	 */
	hops = ipx->tc;
	hops += 1;
	ipx->tc = hops;
    }
    /*
     * Fix up source net if necessary:
     */
    if (idb && (GET2WD(ipx->ssnet) == 0))
	PUT2WD(ipx->ssnet, nidb->novellnet);
    
    if (nov_send(pak))
	novell_stat[NOVELL_FORWARDED]++;
}
/*
 * nov_presend
 * Send a packet out a single interface
 */
static boolean nov_presend (paktype *pak, uchar *dhost, ulong flags)
{
    ipxhdrtype *ipx = (ipxhdrtype *)ipxheadstart(pak);
    ushort hops;
    ipx_watchdog_packet *ipxwpak;
    addrtype proto_addr;

    /*
     * Pass the checksum through with no modification, unless the output
     * encapsulation is novell-ether:
     */
    if ((pak->nif_output->enctype == ET_NOVELL_ETHER) ||
	(pak->nif_output->enctype == ET_NOVELL_FDDI))
	ipx->cksum = NOVELL_NO_CHECKSUM;
    
    if ((pak->if_input) && (!((ipx->pt == NOVELL_TYPE_NETBIOS) &&
			    ieee_equal(ipx->dhost, novell_allhosts)))) {
	/*
	 * Adjust outbound hop count (on non type 20, transit packets ONLY).
	 * When novell algorithms is enabled, the algorithm used allows
	 * packets to transit both RIP and IGRP clouds with a "logically"
	 * increasing hop count. RIP allows 16 hops, IGRP allows as many as
	 * 224 hops. In mixed environments, fewer hops are possible.
	 *
	 * Note: this is here because the route table lookup gives us the
	 * routing protocol by which the next hop was learned.
	 *
	 * Algorithm:
	 *
	 * Bit width of TC    Action
	 * ---------------    ------
	 *     IN     OUT
	 *      4      4      tc++;
	 *      8      8      tc++
	 *      4      8      if (tc==0) tc=2 else tc++;if (tc <= 15) tc=tc*16;
	 *      8      4      tc=tc/16;
	 */
	hops = ipx->tc;
	if (novell_debug) buginf(", tc:%-02x->",hops);
	if (NOVELL_IGRP_HOPCOUNT_ALGORITHM_ALLOWED) {
	    if (hops <= XNS_MAX_HOPS) {
		if (flags & NET_IGRP) {
		    /* 4 -> 8 */
		    if (hops == 0) {
			hops = 2;
		    } else {
			hops +=1;
			}
		    if (hops <= XNS_MAX_HOPS)
			hops = (hops * 16);
		} else {
		    /* 4 -> 4 */
		    hops += 1;
		}
	    } else {
		if (flags & NET_IGRP) {
		    /* 8 -> 8 */
		    hops += 1;
		} else {
		    /* 8 -> 4 */
		    hops = (hops/16);
		}
	    }
	} else {
	    hops += 1;
	}
	if (novell_debug) buginf("%-02x",hops);
	ipx->tc = hops;
    } else {
      if ((ipx->pt == NOVELL_TYPE_NETBIOS) &&
	  ieee_equal(ipx->dhost, novell_allhosts)) {
	/* Here we have a Type20 "broadcast" should it be filtered? */
	if (!novell_netbios_acfcheck(pak,
				     IPX_ACCESS_DIRECTION_OUTGOING,
				     (nidbtype *) pak->nif_output)) {
	  novell_stat[NOVELL_FILTER_OUT]++;
	  if (novell_debug)
	    nov_netbios_buginf(pak, "outbound filtered");
	  retbuffer(pak);
	  return(FALSE);
	}
      }
    }
    /* Check to see if the packet is bigger than the maximum mtu on the 
     * output interface. If it is, pitch it. 
     */
    if (GETSHORT(&ipx->len) > pak->if_output->hwptr->maxmtu) {
	if (novell_debug) {
	    print_packet(pak, "dropped - too big, size=");
	    buginf("%d, %s", GETSHORT(&ipx->len),
		   pak->if_output->short_namestring);
	}
	nov_dispose(NOVELL_FORMATERROR, pak);
	return(FALSE);
    }	

    /*
     * Before sending the packet, see if access lists are defined.
     * Only do access checks for routed traffic (if_input != NULL).
     */
    if (pak->if_input) {
	if (pak->if_output->novell_accesslist) {
	    if (!nov_accesscheck(pak, 
				 pak->if_output->novell_accesslist)) {
	        novell_stat[NOVELL_FILTER_OUT]++;
		retbuffer(pak);
		if (novell_debug) buginf(", *denied (out)*");
		return(FALSE);
	    }
	}
    }

    /*
     * If the packet is a watchdog packet and the outgoing DDR interface
     * has watchdog spoofing enabled then we answer this packet and
     * don't route it.
     * Watchdog is a 32 byte IPX packet with a "?" in the 32nd byte.
     */
    ipxwpak = (ipx_watchdog_packet *) (pak->network_start);

    if ((pak->if_output != NULL) && (pak->if_input != NULL)) {
      if (pak->nif_output->novell_spoof_flag & NOVELL_W_SPOOF) {
	if ((GETSHORT(&ipxwpak->len) == NOVELL_WATCHDOG_LEN) &&
	    (ipxwpak->watchdog_byte == NOVELL_WATCHDOG_QUERY)) {
	  novell_stat[NOVELL_WATCHDOG_IN]++;
	  if (novell_spoof_debug) print_packet(pak, "watchdog");
	  reg_invoke_proto_address(LINK_NOVELL, pak->if_output, pak, 
				   &proto_addr);
	  proto_addr.nov_addr.net = ipx_nexthop_network(pak);
	  ieee_copy(dhost, proto_addr.nov_addr.host);
	  if (!is_ddr(pak->if_output->hwptr) ||
	      (is_ddr(pak->if_output->hwptr) &&
	       !reg_invoke_dialer_connected(pak->if_output->hwptr,
					    &proto_addr, LINK_NOVELL))) {
	    if (!nov_watchdogspoof(pak)) {
	      retbuffer(pak);
	      if (novell_spoof_debug) buginf(" *failed*");
	      return(FALSE);
	    }
	    retbuffer(pak);
	    return(TRUE);
	  } else {
	    /* DDR and the line is up, allow the packet thru */
	      pak->dialer_flags |= PAK_DDR_UNINTERESTING;
	      if (novell_spoof_debug) buginf(" (up: let through)");
	  }
	}
      }
      if (((pak->nif_input->novell_spoof_flag & NOVELL_S_SPOOF) || 
	   (pak->nif_output->novell_spoof_flag & NOVELL_S_SPOOF)) &&
	  (ipxwpak->pt == NOVELL_TYPE_SPX)) {
	if (nov_spx_spoof(pak, dhost)) {
	  retbuffer(pak);
	  return(TRUE);
	}
      }
    }
    if (novell_debug) {
	print_packet(pak, "gw=");
	if (pak->if_output) {
	    buginf("%s:%x.%e", pak->if_output->short_namestring,
		    ipx_nexthop_network(pak), dhost);
	}
    }
    return(nov_send1(pak, dhost, GETSHORT(&ipx->len)));
}

/*
 * nov_send
 * Send a Novell gram
 */
boolean nov_send (paktype *pak)
{
    ipxhdrtype *ipx;
    uchar *dhost;
    novndbtype *p;
    nidbtype *t; /* Workaround compiler recast problem */
    ulong flags;
    nidbtype *nidb;
    boolean sent_once;
    ulong nidbcount;
    ulong dnet;
    paktype *newpak;
    boolean success;

    ipx = (ipxhdrtype *)ipxheadstart(pak);

    /* Handle responses to net 0 (should only be for unnumbered interfaces) */
    dnet = GET2WD(ipx->ddnet);
    if (dnet == 0) {
	flags = NET_CONNECTED;
	/* Use output nidb if set. Else, try input nidb */
	if (pak->nif_output == NULL) {
	    pak->nif_output = pak->nif_input;
	}
	dhost = &(ipx->dhost[0]);
    } else {
	p = (*novell_table)[NOV_NETHASH(dnet)];
	dhost = novrip_first_hop(dnet, &(ipx->dhost[0]),
				 &t, &flags, p, IPX_ROUND_ROBIN_YES,
				 IPX_DEFAULT_ROUTE_YES);
	pak->nif_output = t;
    }

    if ((dhost == NULL) || (pak->nif_output == NULL) ||
	(pak->nif_output == nullnidb)) {	/* no route */
	if (novell_debug)
	    print_packet(pak, "no route");
	nov_error(pak, XNS_ERR_UNREACH, 0, NOVELL_NOROUTE);
	return(FALSE);
    }
    /* Internal networks never have packets sent out them... */
    if (IPX_NIDB_IS_INTERNAL(pak->nif_output)) {
	datagram_done(pak);
	return(TRUE);
    }
    if (0 /* flags & NET_CLIENT */ ) {
	sent_once = FALSE;
	nidbcount = 0;
	FOR_ALL_NIDBS(nidb) {
	    if (nidb->novellnet == dnet) {
		nidbcount++;
	    }
	}
	if (nidbcount == 0) {
	    datagram_done(pak);
	    return(FALSE);
	}
	success = TRUE;
	FOR_ALL_NIDBS(nidb) {
	    if (nidb->novellnet == dnet) {
		/* Don't copy the last time, just fall through */
		if (--nidbcount <= 0) {
		    pak->nif_output = nidb;
		    break;
		}
		newpak = pak_duplicate(pak);
		if (newpak != NULL) {
		    newpak->nif_output = nidb;
		    newpak->if_output = newpak->nif_output->idb;
		    success &= nov_presend((paktype *) newpak, dhost, flags);
		}
		
	    }
	}
    }
    pak->if_output = pak->nif_output->idb;
    return(nov_presend(pak, dhost, flags));
}

/*
 * nov_send1
 * Other half of sending XNS grams
 */

boolean nov_send1 (paktype *pak, uchar const *dhost, uint len)
{
    ipxhdrtype *ipx;
    ipxhdrtype ipx_copy;
    idbtype *idb;
    hwidbtype *hwidb;
    nidbtype *nidb;
    paktype *pak2;
    int Qsize, count;
    novell_client *client_record;
    leveltype status;

    pak2 = NULL;
    nidb = pak->nif_output;

    /* Internal networks never have packets sent out them... */
    if (IPX_NIDB_IS_INTERNAL(nidb)) {
	datagram_done(pak);
	return(TRUE);
    }

    ipx = (ipxhdrtype *)ipxheadstart(pak);
    idb = pak->if_output;
    pak->acct_proto = ACCT_PROTO_NOVELL;
    hwidb = hwidb_or_null(idb);

    /*
     *  Step in and check if this is for a client of ours:
     */

     if ((Qsize = QUEUESIZE(&nidb->clientQ))) {
	/*
	 * If this is a broadcast, duplicate it and fan it out to
	 * all of the client idbs.
	 */
	if (ieee_equal(ipx->dhost, novell_allhosts)) {
	    for (count = 0; count < Qsize; count++) {
		client_record = dequeue(&nidb->clientQ);
		enqueue(&nidb->clientQ, client_record);
		pak2 = pak_duplicate(pak);
		if (!pak2) {
		    continue;
		}
		pak2->if_output = client_record->idb;
    		hwidb = hwidb_or_null(pak2->if_output);
		pak2->nif_output = client_record->idb->owner_nidb;
		if (!interface_up(pak2->if_output)) {
		    datagram_done(pak2);
 		    continue;
		}
    		if (pak_encap(hwidb, pak2, len, (long)dhost)) {
		    datagram_out(pak2);
		} else {
		    datagram_done(pak2);
		    if (novell_debug)
			buginf("\nUnable to encapsulate BROADCAST for client");
		}
	    }
	    datagram_done(pak);
	    return(TRUE);
	} else {
	    for (count = 0; count < Qsize; count++) {
		client_record = dequeue(&nidb->clientQ);
		enqueue(&nidb->clientQ, client_record);
		/* 
		 * Check the next hop host address, against the
		 * client's host address. If there is a match, send
		 * it to the client interface.
		 */
		if (interface_up(client_record->idb)   
		    && (ieee_equal(client_record->idb->novell_node, dhost))){
		    pak->if_output = client_record->idb;
    		    hwidb = hwidb_or_null(pak->if_output);
    		    if (pak_encap(hwidb, pak, len, (long)dhost)) {
		    	datagram_out(pak);
		        return(TRUE);
		    } else {
			if (novell_debug) {
			    buginf("\n%s: Unable to encapsulate for client",
				   client_record->idb->short_namestring);
			}
			datagram_done(pak);
			return(FALSE);
		    }
		}
	    }
	}
    }

    if ((idb == NULL) || (nidb == NULL)) {
	errmsg(&msgsym(NULLIDB, SYS), "nov_send1");
	retbuffer(pak);
	return(FALSE);
    }
    if (!interface_up(idb) || (!NOVELL_NIDB_SENDS_ALLOWED(nidb))) {
	retbuffer(pak);
	return(FALSE);
    }
    if (nov_multicast_addr(hwidb, dhost[0])) {
	novell_stat[NOVELL_BCASTOUT]++;
	if (hwidb->status & IDB_TR)
	    dhost = nov_trmulticast;
    }
    /*
     * Possibly evenize outbound packet. The reasons for this are
     * historical, ultimately deriving from the fact that some Novell
     * certified ethernet drivers will not accept odd-length packets on
     * this media. See CSCdi09754 and CSCdi11734 for background info.
     */
    pak->enctype = nidb->enctype;

    /* 
     * This doesn't seem right.
     */
    if (QUEUESIZE(&nidb->clientQ)) {
	pak->enctype = ET_PPP;
    }
    if (idb->novell_evenize) {
	if (len & 1) {
	    if ((((hwidb->status & (IDB_ETHER|IDB_LEX)) || lec_enabled(idb))
                 && (pak->enctype != ET_SAP)) ||
		((hwidb->status & IDB_TR)    && (pak->enctype == ET_SNAP)) ||
		((hwidb->status & IDB_FDDI)  && (pak->enctype == ET_SAP)) ||
		((hwidb->status & IDB_SERIAL)))
		len++;
	} else {
	    if ((((hwidb->status & (IDB_ETHER|IDB_LEX)) || lec_enabled(idb))
                 && (pak->enctype == ET_SAP)) ||
		((hwidb->status & IDB_TR) && (pak->enctype == ET_SAP)) ||
		((hwidb->status & IDB_FDDI)  && (pak->enctype == ET_SNAP)))
		len++;
	}
    }
    /*
     * Encapsulation can involve compression, best to keep a copy of the
     * original header so accounting doesn't get bad data
     */
    ipx = (ipxhdrtype *) ipxheadstart(pak);
    bcopy(ipx, &ipx_copy, sizeof(ipxhdrtype));

    if (pak_encap(hwidb, pak, len, (long)dhost)) {
#ifdef DEBUG_ENCAPS
	print_novell_encap(pak,
			   pak->if_output,
			   "  encap");
#endif
	/* Check to see if the packet is bigger than the max size packet
	 * on the output interface. If it is, pitch it. 
	 */
	if (pak->datagramsize > pak->if_output->hwptr->max_pak_size) {
	    if (novell_debug) {
		print_packet(pak, "dropped - too big, size=");
		buginf("%d, %s", GETSHORT(&ipx->len),
		       pak->if_output->short_namestring);
	    }
	    nov_dispose(NOVELL_FORMATERROR, pak);
	    return(FALSE);
	}	
	if ((nidb->novellnet == GET2WD(ipx->ddnet)) &&
	    ieee_equal(ipx->dhost, idb->novell_node)) {
	  if (hwidb->status & IDB_HEARSELF) {
	    pak->flags |= PAK_HEARSELF;
	  } else {
	    pak2 = pak_duplicate(pak);
	    if (pak2) {
	      pak2->if_input = idb;    
	      pak2->if_output = NULL;
	      pak2->flags |= PAK_SENDSELF;
	      status = raise_interrupt_level(NETS_DISABLE);
	      raw_enqueue(pak2->if_input->hwptr, pak2);
	      reset_interrupt_level(status);
	      /* Don't actually send the packet out to ourself */
	      datagram_done(pak);
	      return(TRUE);
	    }
	  }
	}
	novell_stat[NOVELL_OUTPUT]++;
	if (pak->nif_input != NULL && idb->ipx_accounting)
	  ipx_accumulate_acctg(&ipx_copy);
	datagram_out(pak);		/* queue it for output */
	return(TRUE);
    }
    if (novell_debug) {
	print_packet(pak, "encap failed");
#ifdef DEBUG_ENCAPS
	print_novell_encap(pak,
			   pak->if_output,
			   "post-mortem" );
#endif
    }
    nov_error(pak, XNS_ERR_UNREACH, 0, NOVELL_ENCAPSFAILED);
    return(FALSE);
}

/*
 * nov_watchdogspoof
 * Handle a watchdog request for remote Client. Answer the server.
 */

boolean
nov_watchdogspoof (paktype *pak)
{
    ipxhdrtype *ipx, *newipx;
    ipx_watchdog_packet *ipxwpak;
    paktype *newpak;

    ipx = (ipxhdrtype *)ipxheadstart(pak);

    newpak = pak_duplicate(pak);
    if (!(newpak == NULL)) {
	newipx = (ipxhdrtype *)ipxheadstart(newpak);
	newpak->if_output = pak->if_input;
	newpak->nif_output = pak->nif_input;
	PUT2WD(newipx->ssnet, GET2WD(ipx->ddnet));
	ieee_copy(ipx->shost, newipx->dhost);
	newipx->ssock = ipx->dsock;
	newipx->dsock = ipx->ssock;
	ieee_copy(ipx->dhost, newipx->shost);
	PUT2WD(newipx->ddnet, GET2WD(ipx->ssnet));
	
	newipx->cksum = NOVELL_NO_CHECKSUM;
	newipx->len = NOVELL_WATCHDOG_LEN;
	newipx->tc = 0;
	ipxwpak = (ipx_watchdog_packet *) newpak->network_start;
	/* Change the "?" query into a "Y" connection valid */
	ipxwpak->watchdog_byte = NOVELL_WATCHDOG_YES;
	
	newpak->nif_output = NULL;  /* nov_send will fill this in */
	clear_if_input(newpak);     /* clear copied input parameters */
	if (nov_send(newpak)) {
	    novell_stat[NOVELL_WATCHDOG_OUT]++;
	    if (novell_spoof_debug) print_packet(newpak, "watchdog sent");
	    return(TRUE);
	}
    }
    return(FALSE);
    /* Pak will be returned by caller */
}



/*
 * nov_error
 * Handler sending an XNS error packet
 */

void nov_error (
    paktype *pak,
    int type,
    int code,
    int local)
{
    if (local >= 0)
	nov_dispose(local, pak);
    else
	datagram_done(pak);
}

/*
 * nov_log_stats
 *
 * Log selected IPX header info when stats are updated.
 *
 */
void nov_log_stats (int code, paktype *pak)
{
    ipxhdrtype *ipx;

    ipx = (ipxhdrtype *)ipxheadstart(pak);
    switch(code) {
      case NOVELL_RIPFMTERROR:
	nov_last_bad_rip_net = GET2WD(ipx->ssnet);
	ieee_copy(ipx->shost, nov_last_bad_rip_host);
	break;
      case NOVELL_SAPFMTERROR:
	nov_last_bad_sap_net = GET2WD(ipx->ssnet);
	ieee_copy(ipx->shost, nov_last_bad_sap_host);
	break;
      default:
	break;
    }
}


/*
 * nov_format_dispose
 * Discard an XNS input packet ... called if we wish to distinguish
 * RIP/SAP from other format errors.
 */

void nov_format_dispose (paktype *pak)
{
    ipxhdrtype *ipx;

    ipx = (ipxhdrtype *)ipxheadstart(pak);

    if (ipx->dsock == NOVELL_SOCK_RIP)
	nov_dispose(NOVELL_RIPFMTERROR, pak);
    else if (ipx->dsock == NOVELL_SOCK_SAP)
	nov_dispose(NOVELL_SAPFMTERROR, pak);
    else
	nov_dispose(NOVELL_FORMATERROR, pak);
}


/*
 * nov_dispose
 * Discard an XNS input packet
 */

void nov_dispose (int code, paktype *pak)
{
    if (code >= NOVELL_MAXTRAFFIC)
      errmsg(&msgsym(BADDISP, SYS), code, "nov_dispose");
    else {
	nov_log_stats(code, pak);
	novell_stat[code]++;
    }
    datagram_done(pak);
}


/*
 * novnaccess_command
 * Parse nov normal access list command.
 * access-list <number> <permit | deny> <source network | -1><.><source addr>
 *             <source mask> <destination network | -1><.><destination addr>
 *             <destination mask>
 *
 * The only required parmeter is the source network, which may be -1 -- 
 * indicates ALL networks. A value of zero (0) is disallowed
 * 		access-list 800 deny -1
 * The source address parameter is an optional parmeter. If specified, it
 * be separated from the source network by a period (.).
 *              access-list 800 deny -1.0000.0c00.1111
 * The source mask is an optional parameter, but it may only be specified if
 * a source address was specfied.
 *              access-list 800 deny -1.0000.0c00.1111 0000.00ff.ffff
 *              access-list 800 deny -1 0000.00ff.ffff (is illegal)
 * The rules for  the destination parameters are identical -- except that
 * the destination network is optional.
 *              access-list 800 deny -1 2 
 *              access-list 800 deny -1 2.0000.0c00.1111
 *              access-list 800 deny -1 2.0000.0c00.1111 0000.00ff.ffff
 * The maximum definition would result in
 *              access-list 800 deny 1.1111.1111.1111 0000.00ff.ffff
 *                                   2.2222.2222.2222 0000.00ff.ffff
 */
void novnaccess_command (parseinfo *csb)
{
    int number;
    boolean grant;
    xnsnacctype *item, *wrk_item;
    xnsnacctype *entry;
    novpdbtype *pdb;
    
    if (csb->nvgen) {
	nv_novaccess(csb);
	return;
    }

    number = GETOBJ(int,1);
    grant = (boolean) GETOBJ(int,2);

    if (csb->sense == FALSE) {
	nov_access_delete(number);
    } else {
	entry = malloc(sizeof(xnsnacctype));
	if (!entry)
	    return;
	entry->grant = grant;
	entry->flags = GETOBJ(int,3);
	/*
	 * Network/host masks are always applied. Init to 0 in case
	 * they are not specified:
	 */
	entry->srcnetmask = entry->dstnetmask = 0;
	entry->srchostmask.u.lword[0] = entry->dsthostmask.u.lword[0] = 0;
	entry->srchostmask.u.sword[2] = entry->dsthostmask.u.sword[2] = 0;

	if ((entry->flags & (XNS_SRCNET + XNS_SRCHOST)) ==
	    (XNS_SRCNET + XNS_SRCHOST)) {
	    entry->srcnet = GETOBJ(paddr,1)->nov_addr.net;
	    ieee_copy(GETOBJ(paddr,1)->nov_addr.host,
		      (uchar *)&entry->srchost);
	} else if (entry->flags & XNS_SRCNET) {
	    entry->srcnet = GETOBJ(int,4);
	} else {
	    printf("\nIPX network address required - %s", csb->line);
	    return;
	}

	if (entry->flags & XNS_SRCHOSTMASK)
	    ieee_copy(GETOBJ(paddr,2)->nov_addr.host,
		      (uchar *)&entry->srchostmask);

	if ((entry->flags & (XNS_DSTNET + XNS_DSTHOST)) ==
	    (XNS_DSTNET + XNS_DSTHOST)) {
	    entry->dstnet = GETOBJ(paddr,3)->nov_addr.net;
	    ieee_copy(GETOBJ(paddr,3)->nov_addr.host,
		      (uchar *)&entry->dsthost);
	} else if (entry->flags & XNS_DSTNET) {
	    entry->dstnet = GETOBJ(int,5);
	}

	if (entry->flags & XNS_DSTHOSTMASK)
	    ieee_copy(GETOBJ(paddr,4)->nov_addr.host,
		      (uchar *)&entry->dsthostmask);
	/*
	 * Store our masks the way we will use them, speeds the
	 * fastpath checking (not having to flip the bits).
	 */
	entry->srcnetmask = ~entry->srcnetmask;
	entry->dstnetmask = ~entry->dstnetmask;
	entry->dsthostmask.u.sword[2] = ~entry->dsthostmask.u.sword[2];
	entry->dsthostmask.u.lword[0] = ~entry->dsthostmask.u.lword[0];
	entry->srchostmask.u.sword[2] = ~entry->srchostmask.u.sword[2];
	entry->srchostmask.u.lword[0] = ~entry->srchostmask.u.lword[0];

	/*
	 * Check to ensure that we don't add a duplicate entry.
	 */
	wrk_item = ipx_get_std_acl(number);
	for (item = wrk_item; item; wrk_item = item, item = item->next) {
	    if (bcmp((uchar *)&entry->grant, (uchar *)&item->grant,
		     sizeof(xnsnacctype) - sizeof(item->next)) == 0) {
		free(entry);
		return;
	    }
	}
	
	/*
	 * Finally add the new item to the list.
	 */
	if (wrk_item == NULL)
	    ipx_set_std_acl(number, entry);
	else
	    wrk_item->next = entry;
    }

    for (pdb = (novpdbtype *)novpdbQ.qhead; pdb; pdb= pdb->next) {
	if (pdb->proctype == NET_NLSP) {
	    reg_invoke_ipx_mark_dr_info_change(pdb, NULL);
	    continue;
	}
        if (pdb->proctype != NET_IGRP)
            continue;
	access_mark_list_changed(number);
    }

}

/*
 * nv_novsumaccess
 * Function to write out the NOV access list(s).
 */
static void nv_novsumaccess (parseinfo *csb)
{
    int i;
    nov_sumlisttype *item;
    
    for (i = MINNOVSUM; i <= MAXNOVSUM; i++) {
	for (item = ipx_get_sum_acl(i); item; item = item->next) {
	    nv_write(TRUE,"%s %d %s", csb->nv_command, i,
		     item->grant ? "permit" : "deny");
	    if (item->net == -1L)
		nv_add(TRUE, " -1");
	    else 
		nv_add(TRUE, " %x %x", item->net, item->mask);
	    if (item->idb != NULL) {
		nv_add(TRUE, " %s", item->idb->namestring);
	    }
	    nv_add(item->ticks != 1, " ticks %d", item->ticks);
	    nv_add(item->area_count != NOVELL_DEFAULT_AREA_COUNT,
		   " area-count %d", item->area_count);
	}
    }
}

/*
 * novsumaccess_command
 *
 * Novell summary access list:
 *
 * access-list <1200-1299> { permit | deny } <net> <mask> [ticks] <number>
 *             [area-count] <number>
 *
 * OBJ(int,1) = access list number
 * OBJ(int,2) = TRUE for "permit", FALSE for "deny".
 * OBJ(int,3) = IPX network number
 * OBJ(int,4) = IPX network mask
 * OBJ(int,5) = ticks
 * OBJ(int,6) = area count
 * OBJ(int,7) = TRUE => OBJ(idb,1) is interface qualifier
 */
void novsumaccess_command (parseinfo *csb)
{
    int number;
    boolean grant;
    nov_sumlisttype *item, *wrk_item, *entry;
    novpdbtype *pdb;
    ulong address, mask;    
    uchar area_count;
    ushort ticks;
    novndbtype *ndb;
    idbtype *idb = NULL;
     
    if (csb->nvgen) {
	nv_novsumaccess(csb);
	return;
    }

    number = GETOBJ(int,1);
    grant = (boolean) GETOBJ(int,2);

    if (csb->sense == FALSE) {
	item = ipx_get_sum_acl(number);
	ipx_set_sum_acl(number, NULL);
	while (item) {
	    wrk_item = item->next;
	    if (!item->grant && (ndb = novrip_find_rt(item->net, item->mask,
						      IPX_ROUTE_MATCH_EXACT)))
		if (ndb->path_list && ndb->path_list->nidb == nullnidb)
		    novrip_del_rt(item->net, item->mask, 
				  IPX_ROUTE_DELETE_RETAIN_CONNECTED);
	    free(item);
	    item = wrk_item;
	}
    } else {
	address = GETOBJ(int,3);
	mask = GETOBJ(int,4) ? GETOBJ(int,4) : NOV_NETMASK_ALL;
	ticks = GETOBJ(int,5);
	area_count = GETOBJ(int,6);
	if (GETOBJ(int,7)) {
	    idb = GETOBJ(idb,1);
	}
	
	if ((address & mask) != address) {
	    printf("\n%%%s: Mask inconsistent with network");
	    return;
	}

	if (!ipx_validate_contiguous_mask(mask)) {
	    printf("\n%%: Non-contiguous mask %8x is not supported", mask);
	    return;
	}
	entry = malloc(sizeof(nov_sumlisttype));
	if (!entry)
	    return;

	entry->net = address;
	entry->mask = mask;
	entry->grant = grant;
	entry->area_count = area_count ? area_count : 
	    NOVELL_DEFAULT_AREA_COUNT;
	entry->ticks = ticks ? ticks : 1;
	entry->idb = idb;

	/*
	 * Check to ensure that we don't add a duplicate entry. A duplicate
	 * is considered to have the same net, mask, and idb (if any).
	 */
	wrk_item = ipx_get_sum_acl(number);
	for (item = wrk_item; item; wrk_item = item, item = item->next) {
	    if ((entry->net == item->net) &&
		(entry->mask == item->mask) &&
		(entry->idb == item->idb)) {
		item->grant = entry->grant;
		item->area_count = entry->area_count;
		item->ticks = entry->ticks;
		free(entry);
		for (pdb = (novpdbtype *) novpdbQ.qhead; pdb; 
		     pdb = pdb->next) {
		    if (pdb->proctype == NET_NLSP && pdb->osi_pdb) {
			reg_invoke_ipx_mark_dr_info_change(pdb, NULL);
		    }
		}
		return;
	    }
	}
	
	/*
	 * Finally add the new item to the list.
	 */
	if (wrk_item == NULL)
	    ipx_set_sum_acl(number, entry);
	else
	    wrk_item->next = entry;
    }
    for (pdb = (novpdbtype *) novpdbQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->proctype == NET_NLSP && pdb->osi_pdb) {
	    reg_invoke_ipx_mark_dr_info_change(pdb, NULL);
	    reg_invoke_isis_mark_L1_changed(pdb->osi_pdb);
	}
    }
}

/*
 * nv_novaccess
 * Function to write out the NOV access list(s).
 */
void nv_novaccess (parseinfo *csb)
{
    int i;
    mac_address_type shost,dhost;
    xnseacctype *item;
    char *str;
    
    for (i = MINNOVACC; i <= MAXENOVACC; i++) {
	for (item = ipx_get_ext_acl(i); item; item = item->next) {
    	    str = NULL;
	    nv_write(TRUE,"%s %d %s", csb->nv_command, i,
		     item->grant ? "permit" : "deny");
	    switch (item->protocol) {
	      case NOVELL_TYPE_SPX:
		str = "spx";
		break;
	      case NOVELL_TYPE_SAPRESP:
		str = "sap";
		break;
	      case NOVELL_TYPE_RIP:
		str = "rip";
		break;
	      case NOVELL_TYPE_NCP:
		str = "ncp";
		break;
	      case NOVELL_TYPE_NETBIOS:
		str = "netbios";
		break;
	      case NOVELL_TYPE_ANY_PROT:
		str = "any";
		break;
	      default:
		break;
	    }
	    if (str != NULL)
		nv_add((item->flags & XNS_PROTOCOL)," %s", str);
	    else
	        nv_add((item->flags & XNS_PROTOCOL)," %d", 
			(short) item->protocol);

	    /* "any" keyword currently supported for extended lists only */
	    if ((i >= MINENOVACC) && (i <= MAXENOVACC) && 
		(item->srcnet == NOVELL_TYPE_ANY_NET)) {
	        nv_add((item->flags & XNS_SRCNET)," any");
	    } else {
	        nv_add((item->flags & XNS_SRCNET)," %x", item->srcnet);
	        nv_add((item->flags & XNS_SRCHOST),".%e", &item->srchost);
	    }
	    
	    shost.u.lword[0] = ~item->srchostmask.u.lword[0];
	    shost.u.sword[2] = ~item->srchostmask.u.sword[2];
	    if (item->flags & XNS_SRCNETMASK) {
		nv_add((item->flags & XNS_SRCNETMASK)," %x", ~item->srcnetmask);
		nv_add((item->flags & XNS_SRCHOSTMASK),".%e", &shost);
	    } else {
		nv_add((item->flags & XNS_SRCHOSTMASK)," %e", &shost);
	    }
	    str = NULL;
	    switch (item->srcsocket) {
	      case NOVELL_SOCK_SAP:
		str = "sap";
		break;
	      case NOVELL_SOCK_RIP:
		str = "rip";
		break;
	      case NOVELL_SOCK_NLSP:
		str = "nlsp";
		break;
	      case NOVELL_SOCK_NCP:
		str = "ncp";
		break;
	      case NOVELL_SOCK_NETBIOS:
		str = "netbios";
		break;
	      case NOVELL_SOCK_IGRP:
		str = "eigrp";
		break;
	      case NOVELL_SOCK_DIAG:
		str = "diagnostic";
		break;
	      case NOVELL_SOCK_ECHO:
		str = "cping";
		break;
	      case NOVELL_SOCK_ECHO_STND:
		str = "nping";
		break;
	      case NOVELL_SOCK_ALL:
		str = "all";
		break;
	      default:
		break;
	    }
	    if (str != NULL) 
		nv_add((item->flags & XNS_SRCSOCKET)," %s", str);
	    else
		nv_add((item->flags & XNS_SRCSOCKET)," %x", item->srcsocket);

	    /* "any" keyword currently supported for extended lists only */
	    if ((i >= MINENOVACC) && (i <= MAXENOVACC) &&
		(item->dstnet == NOVELL_TYPE_ANY_NET)) {
		nv_add((item->flags & XNS_DSTNET), " any");
	    } else {
	        nv_add((item->flags & XNS_DSTNET)," %x", item->dstnet);
	        nv_add((item->flags & XNS_DSTHOST),".%e", &item->dsthost);
	    }

	    dhost.u.lword[0] = ~item->dsthostmask.u.lword[0];
	    dhost.u.sword[2] = ~item->dsthostmask.u.sword[2];
	    if (item->flags & XNS_DSTNETMASK) {
	      nv_add((item->flags & XNS_DSTNETMASK)," %x", ~item->dstnetmask);
	      nv_add((item->flags & XNS_DSTHOSTMASK),".%e", &dhost);
	    }
	    else {
	      nv_add((item->flags & XNS_DSTHOSTMASK)," %e", &dhost);
	    }
            str = NULL;
            switch (item->dstsocket) {
              case NOVELL_SOCK_SAP:
                str = "sap";
                break;
              case NOVELL_SOCK_RIP:
                str = "rip";
                break;
              case NOVELL_SOCK_NLSP:
                str = "nlsp";
                break;
              case NOVELL_SOCK_NCP:
                str = "ncp";
                break;
              case NOVELL_SOCK_NETBIOS:
                str = "netbios";
                break;
              case NOVELL_SOCK_IGRP:
                str = "eigrp";
                break;
              case NOVELL_SOCK_DIAG:
                str = "diagnostic";
                break;
              case NOVELL_SOCK_ECHO:
                str = "cping";
                break;
              case NOVELL_SOCK_ECHO_STND:
                str = "nping";
                break;
              case NOVELL_SOCK_ALL:
                str = "all";
                break;
              default:
                break;
            }
            if (str != NULL)
                nv_add((item->flags & XNS_DSTSOCKET)," %s", str);
            else
	        nv_add((item->flags & XNS_DSTSOCKET), " %x", item->dstsocket);
	    if (((i >= MINENOVACC) && (i <= MAXENOVACC)) && item->log)
		nv_add(TRUE, " log");
	}
    }
}

/*
 * show_novaccess
 * Function to display the NOV access lists.
 */
void show_novaccess (int acl)
{
    int i;
    mac_address_type shost,dhost;
    xnseacctype *item;
    char *str;
    nov_sumlisttype *sum;
    
    for (i = MINNOVACC; i <= MAXENOVACC; i++) {
	if (acl && (acl != i))
	    continue;
	item = ipx_get_ext_acl(i);
	if (item) {
	    if (automore_quit())
		return;
	    printf("\nIPX access list %d", i);
	    while (validmem(item)) {
		inline_mem_lock(item);
		printf("\n    %s", item->grant ?
		       "permit" : "deny");
		str = NULL;
		if (item->flags & XNS_PROTOCOL) {
		    switch (item->protocol) {
            	      case NOVELL_TYPE_SPX:
                        str = "spx";
                        break;
                      case NOVELL_TYPE_SAPRESP:
                        str = "sap";
                        break;
                      case NOVELL_TYPE_RIP:
                        str = "rip";
                        break;
                      case NOVELL_TYPE_NCP:
                        str = "ncp";
                        break;
                      case NOVELL_TYPE_NETBIOS:
                        str = "netbios";
                        break;
                      case NOVELL_TYPE_ANY_PROT:
                        str = "any";
                        break;
                      default:
                        break;
                    }
		    if (str != NULL)
                        printf(" %s", str);
                    else
		        printf(" %d", (short) item->protocol);
		}

		/* "any" keyword currently supported for extended lists only */
		if (item->flags & XNS_SRCNET) {
		    if ((i >= MINENOVACC) && (i <= MAXENOVACC) &&
			(item->srcnet == NOVELL_TYPE_ANY_NET)) {
			printf(" any");
		    } else {
			printf(" %x", item->srcnet);
			if (item->flags & XNS_SRCHOST)
			    printf(".%e", &item->srchost);
		    }
		}

		shost.u.lword[0] = ~item->srchostmask.u.lword[0];
		shost.u.sword[2] = ~item->srchostmask.u.sword[2];
		if (item->flags & XNS_SRCNETMASK) {
		    printf(" %x", ~item->srcnetmask);
		    if (item->flags & XNS_SRCHOSTMASK)
		      printf(".%e", &shost);
		  }
		else {
		    if (item->flags & XNS_SRCHOSTMASK)
		      printf(" %e", &shost);
		  }
		if (item->flags & XNS_SRCSOCKET) {
		    str = NULL;
		    switch (item->srcsocket) {
		      case NOVELL_SOCK_SAP:
			str = "sap";
			break;
		      case NOVELL_SOCK_RIP:
			str = "rip";
			break;
		      case NOVELL_SOCK_NLSP:
			str = "nlsp";
			break;
		      case NOVELL_SOCK_NCP:
			str = "ncp";
			break;
		      case NOVELL_SOCK_NETBIOS:
			str = "netbios";
			break;
		      case NOVELL_SOCK_IGRP:
			str = "eigrp";
			break;
		      case NOVELL_SOCK_DIAG:
			str = "diagnostic";
			break;
		      case NOVELL_SOCK_ALL:
			str = "all";
			break;
		      default:
			break;
		    }
		    if (str != NULL)
			printf(" %s", str);
		    else
		        printf(" %x", item->srcsocket);
		}

		/* "any" keyword currently supported for extended lists only */
		if (item->flags & XNS_DSTNET) {
		    if ((i >= MINENOVACC) && (i <= MAXENOVACC) &&
			(item->dstnet == NOVELL_TYPE_ANY_NET)) {
			printf(" any");
		    } else {
			printf(" %x", item->dstnet);
			if (item->flags & XNS_DSTHOST)
			    printf(".%e", &item->dsthost);
		    }
	        }
		
		dhost.u.lword[0] = ~item->dsthostmask.u.lword[0];
		dhost.u.sword[2] = ~item->dsthostmask.u.sword[2];
		if (item->flags & XNS_DSTNETMASK) {
		  printf(" %x", ~item->dstnetmask);
		  if (item->flags & XNS_DSTHOSTMASK)
		    printf(".%e", &dhost);
		}
		else {
		  if (item->flags & XNS_DSTHOSTMASK)
		    printf(" %e", &dhost);
		}
		if (item->flags & XNS_DSTSOCKET) {
                    str = NULL;
                    switch (item->dstsocket) {
                      case NOVELL_SOCK_SAP:
                        str = "sap";
                        break;
                      case NOVELL_SOCK_RIP:
                        str = "rip";
                        break;
                      case NOVELL_SOCK_NLSP:
                        str = "nlsp";
                        break;
                      case NOVELL_SOCK_NCP:
                        str = "ncp";
                        break;
                      case NOVELL_SOCK_NETBIOS:
                        str = "netbios";
                        break;
                      case NOVELL_SOCK_IGRP:
                        str = "eigrp";
                        break;
                      case NOVELL_SOCK_DIAG:
                        str = "diagnostic";
                        break;
                      case NOVELL_SOCK_ALL:
                        str = "all";
                        break;
                      default:
                        break;
                    }
                    if (str != NULL)
                        printf(" %s", str);
                    else
		        printf(" %x", item->dstsocket);
		}
		if (((i >= MINENOVACC) && (i <= MAXENOVACC)) && item->log)
		    printf(" log");
 		inline_mem_unlock_or_free(item);
		item = item->next;
	    }	    
	}
    }

    for (i = MINNOVSAPACC; i <= MAXNOVSAPACC; i++) {
	if (acl && (acl != i))
	    continue;
	item = ipx_get_ext_acl(i);
	if (item) {
	    if (automore_quit())
		return;
	    printf("\nIPX SAP access list %d", i);
	    while (validmem(item)) {
		inline_mem_lock(item);
		printf("\n    %s", item->grant ?
		       "permit" : "deny");
		if (item->flags & XNS_SRCNET)
		    printf(" %x", item->srcnet);
		if (item->flags & XNS_SRCHOST)
		    printf(".%e", &item->srchost);
		if (item->flags & XNS_SRCNETMASK)
		    printf(" %x", ~item->srcnetmask);
		if (item->flags & XNS_SRCHOSTMASK) {
		  shost.u.lword[0] = ~item->srchostmask.u.lword[0];
		  shost.u.sword[2] = ~item->srchostmask.u.sword[2];
		  printf(".%e", &shost);
		}
		if (item->flags & XNS_SERVER_TYPE)
		    printf(" %x", item->protocol);
		if (item->flags & XNS_SERVER_NAME)
		    printf(" %S", item->server_name);
		inline_mem_unlock_or_free(item);
		item = item->next;
	    }	    
	}
    }

    for (i = MINNOVSUM; i <= MAXNOVSUM; i++) {
	if (acl && (acl != i))
	    continue;
	sum = ipx_get_sum_acl(i);
	if (sum) {
	    if (automore_quit())
		return;
	    printf("\nIPX summary access list %d", i);
	    while (validmem(sum)) {
		inline_mem_lock(sum);
		printf("\n    %s", sum->grant ? "permit" : "deny");
		if (sum->net == -1L)
		    printf(" -1");
		else 
		    printf(" %x %x", sum->net, sum->mask);
		if (sum->idb != NULL) {
		    printf(" %s", sum->idb->namestring);
		}
		if (sum->ticks != 1)
		    printf(" ticks %d", sum->ticks);
		if (sum->area_count != NOVELL_DEFAULT_AREA_COUNT)
		    printf(" area-count %d", sum->area_count);
		inline_mem_unlock_or_free(sum);
		sum = sum->next;
	    }	    
	}
    }    
}

 

/*
 * noveaccess_command
 * Parse novell extended access list command.
 * access-list [number] [permit | deny] [protocol] [source network]<.>
 *             <source address> <source address mask> <source socket>
 *             <destination network><.><destination address.> 
 *	       (destination address mask> <destination socket>
 *
 * The only required parmeter is the protocol.
 * 		access-list 900 deny 1
 */

void noveaccess_command (parseinfo *csb)
{
    int number;
    boolean grant;
    xnseacctype *item, *wrk_item;
    xnseacctype *entry;
    
    if (csb->nvgen) {
	/* The NVGEN for Novell extended access lists is done
	 * concurrently with the normal access lists.
	 */
	return;
    }

    number = GETOBJ(int,1);
    grant = (boolean) GETOBJ(int,2);

    if (csb->sense == FALSE) {
	nov_access_delete(number);
	return;
    }
    entry = malloc(sizeof(xnseacctype));
    if (!entry)
	return;
    
    entry->grant = grant;
    entry->flags = GETOBJ(int,3);

    /* Protocol is required. */
    entry->protocol = GETOBJ(int,6);

    /* Source net & host */
    if ((entry->flags & (XNS_SRCNET + XNS_SRCHOST)) ==
	(XNS_SRCNET + XNS_SRCHOST)) {
	entry->srcnet = GETOBJ(paddr,1)->nov_addr.net;
	ieee_copy(GETOBJ(paddr,1)->nov_addr.host, (uchar *)&entry->srchost);
    } else if (entry->flags & XNS_SRCNET) {
	entry->srcnet = GETOBJ(int,4);
    }

    /* Source mask */
    if (entry->flags & XNS_SRCHOSTMASK) {
	if (entry->flags & XNS_SRCNETMASK) {
	    entry->srcnetmask = GETOBJ(paddr,2)->nov_addr.net;
	    ieee_copy(GETOBJ(paddr,2)->nov_addr.host,
		  (uchar *)&entry->srchostmask);
	} else {
	    entry->srcnetmask = (ulong) 0x0000;
	    ieee_copy(GETOBJ(hwaddr,2)->addr, (uchar *)&entry->srchostmask);
	}
    }

    /* Source socket */
    if (entry->flags & XNS_SRCSOCKET)
	entry->srcsocket = GETOBJ(int,7);

    /* Destination net & host */
    if ((entry->flags & (XNS_DSTNET + XNS_DSTHOST)) ==
	(XNS_DSTNET + XNS_DSTHOST)) {
	entry->dstnet = GETOBJ(paddr,3)->nov_addr.net;
	ieee_copy(GETOBJ(paddr,3)->nov_addr.host, (uchar *)&entry->dsthost);
    } else if (entry->flags & XNS_DSTNET) {
	entry->dstnet = GETOBJ(int,5);
    }
    
    /* Destination mask */
    if (entry->flags & XNS_DSTHOSTMASK) {
	if (entry->flags & XNS_DSTNETMASK) {
	    entry->dstnetmask = GETOBJ(paddr,4)->nov_addr.net;
	    ieee_copy(GETOBJ(paddr,4)->nov_addr.host,
		  (uchar *)&entry->dsthostmask);
	} else {
	    entry->dstnetmask = (ulong) 0x0000;
	    ieee_copy(GETOBJ(hwaddr,4)->addr, (uchar *)&entry->dsthostmask);
	}
    }

    /* Destination socket */
    if (entry->flags & XNS_DSTSOCKET)
	entry->dstsocket = GETOBJ(int,8);

    /* Logging */
    entry->log = (boolean) GETOBJ(int,9);

    /*
     * Store our masks the way we will use them, speeds the
     * fastpath checking (not having to flip the bits).
     */
    entry->srcnetmask = ~entry->srcnetmask;
    entry->dstnetmask = ~entry->dstnetmask;
    entry->dsthostmask.u.sword[2] = ~entry->dsthostmask.u.sword[2];
    entry->dsthostmask.u.lword[0] = ~entry->dsthostmask.u.lword[0];
    entry->srchostmask.u.sword[2] = ~entry->srchostmask.u.sword[2];
    entry->srchostmask.u.lword[0] = ~entry->srchostmask.u.lword[0];
    /*
     * Check to ensure that we don't add a duplicate entry.
     */
    wrk_item = ipx_get_ext_acl(number);
    for (item = wrk_item; item; wrk_item = item, item = item->next) {
	if (bcmp((uchar *)&entry->grant, (uchar *)&item->grant,
		 sizeof(xnseacctype) - sizeof(item->next)) == 0) {
	    free(entry);
	    return;
	}
    }
    
    /*
     * Finally add the new item to the list.
     */
    if (wrk_item == NULL)
	ipx_set_ext_acl(number, entry);
    else
	wrk_item->next = entry;	    
    access_mark_list_changed(number);
}

/*
 * novsaccess_command
 * Parse novell SAP access list command.
 * [no] access-list [number] [permit | deny] [source network]<.>
 *             <source address> [server type [server name]]
 *
 */
void novsaccess_command (parseinfo *csb)
{
    int number, i;
    boolean grant;
    mac_address_type shost;
    xnseacctype *item, *wrk_item;
    xnseacctype *entry;

    if (csb->nvgen) {
	for (i = MINNOVSAPACC; i <= MAXNOVSAPACC; i++) {
	    for (item = ipx_get_ext_acl(i); item; item = item->next) {
		nv_write(TRUE,"%s %d %s", csb->nv_command, i,
			 item->grant ? "permit" : "deny");
		nv_add((item->flags & XNS_SRCNET)," %x", item->srcnet);
		nv_add((item->flags & XNS_SRCHOST),".%e", &item->srchost);
		nv_add((item->flags & XNS_SRCNETMASK)," %x", ~item->srcnetmask);
		shost.u.lword[0] = ~item->srchostmask.u.lword[0];
		shost.u.sword[2] = ~item->srchostmask.u.sword[2];
		nv_add((item->flags & XNS_SRCHOSTMASK),".%e", &shost);
		nv_add((item->flags & XNS_SERVER_TYPE)," %x", item->protocol);
		nv_add((item->flags & XNS_SERVER_NAME)," %S",
		       item->server_name);
	    }
	}
	return;
    }

    entry = malloc(sizeof(xnseacctype));
    if (!entry)
	return;
    
    number = GETOBJ(int,1);
    grant = (boolean)GETOBJ(int,2);

    if (csb->sense == FALSE) {
	nov_access_delete(number);
	return;
    }
    entry->grant = grant;
    entry->flags = GETOBJ(int,3);

    /* Source net & host */
    if ((entry->flags & (XNS_SRCNET + XNS_SRCHOST)) ==
	(XNS_SRCNET + XNS_SRCHOST)) {
	entry->srcnet = GETOBJ(paddr,1)->nov_addr.net;
	ieee_copy(GETOBJ(paddr,1)->nov_addr.host, (uchar *)&entry->srchost);
    } else if (entry->flags & XNS_SRCNET) {
	entry->srcnet = GETOBJ(int,4);
    }

    /*
     * Source network/host masks are always applied. Init to 0
     * in case they are not specified:
     */
    entry->srcnetmask = 0;
    entry->srchostmask.u.lword[0] = 0;
    entry->srchostmask.u.sword[2] = 0;

    if ((entry->flags & (XNS_SRCNETMASK + XNS_SRCHOSTMASK)) ==
	(XNS_SRCNETMASK + XNS_SRCHOSTMASK)) {
	entry->srcnetmask = GETOBJ(paddr,2)->nov_addr.net;
	ieee_copy(GETOBJ(paddr,2)->nov_addr.host,
	      (uchar *)&entry->srchostmask);
    }

    if (entry->flags & XNS_SERVER_TYPE)
	entry->protocol = GETOBJ(int,5);

    memset(entry->server_name, 0, NOVELL_SERVER_NAME_LEN);
    if (entry->flags & XNS_SERVER_NAME) {
	sstrncpy(entry->server_name, GETOBJ(string,1),
		NOVELL_SERVER_NAME_LEN);
    }

    /*
     * Store our masks the way we will use them.
     */
    entry->srcnetmask = ~entry->srcnetmask;
    entry->srchostmask.u.sword[2] = ~entry->srchostmask.u.sword[2];
    entry->srchostmask.u.lword[0] = ~entry->srchostmask.u.lword[0];
    /*
     * Check to ensure that we don't add a duplicate entry.
     */
    wrk_item = ipx_get_ext_acl(number);
    for (item = wrk_item; item; wrk_item = item, item = item->next) {
	if (bcmp((uchar *)&entry->grant, (uchar *)&item->grant,
		 sizeof(xnseacctype) - sizeof(item->next)) == 0) {
	    free(entry);
	    return;
	}
    }
    
    /*
     * Finally add the new item to the list.
     */
    if (wrk_item == NULL)
	ipx_set_ext_acl(number, entry);
    else
	wrk_item->next = entry;

    access_mark_list_changed(number);
}

static char protstr_buff[8];
static char ssockstr_buff[8];
static char dsockstr_buff[8];
static void
nov_accesslog_msg (nov_logcache_entry *entry)
{
    unsigned count;
    char *plural, *protstr, *ssockstr, *dsockstr;
    static const char *const mode[] = { "deny", "permit" };

    count = entry->count;
    if (count == 0)
        count = 1;

    plural = (count == 1) ? "" : "s";

    switch (entry->protocol) {
      case NOVELL_TYPE_ECHO:
	/*
	 * Cisco ping reply
	 */
	protstr = "ping";
	break;
      case NOVELL_TYPE_SPX:
        protstr = "spx";
        break;
      case NOVELL_TYPE_SAPRESP:
	/*
	 * Novell standard ping uses same protocol type as sap 
	 */
	if (entry->ssock == NOVELL_SOCK_ECHO_STND)
	    protstr = "ping";
	else
            protstr = "sap";
        break;
      case NOVELL_TYPE_RIP:
	/*
	 * Cisco pings requests use same protocol type as rip 
	 */
	if (entry->ssock == NOVELL_SOCK_ECHO)
	    protstr = "ping";
	else
            protstr = "rip";
        break;
      case NOVELL_TYPE_NCP:
        protstr = "ncp";
        break;
      case NOVELL_TYPE_NETBIOS:
        protstr = "netbios";
        break;
      default:
	protstr = protstr_buff;
	sprintf(protstr, "%d", entry->protocol);
        break;
    }

    switch (entry->ssock) {
      case NOVELL_SOCK_SAP:
        ssockstr = "sap";
        break;
      case NOVELL_SOCK_RIP:
        ssockstr = "rip";
        break;
      case NOVELL_SOCK_NLSP:
        ssockstr = "nlsp";
        break;
      case NOVELL_SOCK_NCP:
        ssockstr = "ncp";
        break;
      case NOVELL_SOCK_NETBIOS:
        ssockstr = "netbios";
        break;
      case NOVELL_SOCK_IGRP:
        ssockstr = "eigrp";
        break;
      case NOVELL_SOCK_DIAG:
        ssockstr = "diagnostic";
        break;
      case NOVELL_SOCK_ECHO:
	ssockstr = "cping";
	break;
      case NOVELL_SOCK_ECHO_STND:
	ssockstr = "nping";
	break;
      default:
	ssockstr = ssockstr_buff;
	sprintf(ssockstr, "%x", entry->ssock);
        break;
    }

    switch (entry->dsock) {
      case NOVELL_SOCK_SAP:
        dsockstr = "sap";
        break;
      case NOVELL_SOCK_RIP:
        dsockstr = "rip";
        break;
      case NOVELL_SOCK_NLSP:
        dsockstr = "nlsp";
        break;
      case NOVELL_SOCK_NCP:
        dsockstr = "ncp";
        break;
      case NOVELL_SOCK_NETBIOS:
        dsockstr = "netbios";
        break;
      case NOVELL_SOCK_IGRP:
        dsockstr = "eigrp";
        break;
      case NOVELL_SOCK_DIAG:
        dsockstr = "diagnostic";
        break;
      case NOVELL_SOCK_ECHO:
	dsockstr = "cping";
	break;
      case NOVELL_SOCK_ECHO_STND:
	dsockstr = "nping";
	break;
      default:
	dsockstr = dsockstr_buff;
	sprintf(dsockstr, "%x", entry->dsock);
        break;
    }

    errmsg(&msgsym(NOVACCESSLOG,IPX), entry->list,
               mode[entry->grant != 0], protstr,
	       ssockstr, entry->srcnet, entry->srchost,
	       dsockstr, entry->dstnet, entry->dsthost,
               count, plural);
}

/* nov_accesslog_cache
 *
 *
 */
static void nov_accesslog_cache(int list, boolean grant, ipxhdrtype *ipx)
{
    int i;
    nov_logcache_entry *entry, *oldest;

    entry = oldest = logcache;

    for (i = 0; i < NOVLOGCACHESIZE; i++, entry++) {
        if ((long)(entry->age - oldest->age) < 0)       /* if entry < oldest */            oldest = entry;

        if (entry->protocol != ipx->pt)
            continue;
        if (entry->srcnet != GET2WD(ipx->ssnet))
            continue;
	if (!(ieee_equal(entry->srchost, ipx->shost)))
	    continue;
        if (entry->ssock != ipx->ssock)
            continue;
        if (entry->dstnet != GET2WD(ipx->ddnet))
            continue;
	if (!(ieee_equal(entry->dsthost, ipx->dhost)))
	    continue;
        if (entry->dsock != ipx->dsock)
            continue;
        if (entry->list != list)
            continue;
        if (entry->grant != grant)
            continue;
        entry->age = nov_logcache_version++;
        entry->count++;

        return;
    }

    /* generate summary log record if we delayed anything */
    if (oldest->count)
        nov_accesslog_msg(oldest);

    oldest->protocol = ipx->pt;
    oldest->srcnet = GET2WD(ipx->ssnet);
    ieee_copy(ipx->shost, oldest->srchost);
    oldest->dstnet = GET2WD(ipx->ddnet);
    ieee_copy(ipx->dhost, oldest->dsthost);
    oldest->dsock = ipx->dsock;
    oldest->ssock = ipx->ssock;
    oldest->list  = list;
    oldest->grant = grant;
    oldest->age   = nov_logcache_version++;
    oldest->count = 0;
    GET_TIMESTAMP(oldest->logtime);

    nov_accesslog_msg(oldest);
}

/*
 * nov_accesslog_cache_print
 * Print old cache information
 */

void nov_accesslog_cache_print (void)
{
    int i;
    nov_logcache_entry *entry;

    entry = logcache;

    for (i = 0; i < NOVLOGCACHESIZE; i++) {
        if (entry->count &&
            CLOCK_OUTSIDE_INTERVAL(entry->logtime, NOVLOGPRINTDELAY)) {
            nov_accesslog_msg(entry);
            GET_TIMESTAMP(entry->logtime);
            entry->count = 0;
        }
        entry++;
    }
}

/*
 * nov_accesscheck
 * Apply a NOVELL access list to a packet.
 */

boolean nov_accesscheck (paktype *pak, int accesslist)
{
    ipxhdrtype *ipx;
    xnseacctype *item;
    ulong flags;
    xnspephdrtype *pep;
    ushort host_us;
    ulong  host_ul;
    
    ipx = (ipxhdrtype *)ipxheadstart(pak);

    if (!accesslist)
	return(TRUE);
    item = ipx_get_ext_acl(accesslist);
    if (item == NULL) 
	return(TRUE);

    do {
	flags = item->flags;
	if (flags & XNS_PROTOCOL) {
	    if ((item->protocol != (ushort) -1) && 
		(item->protocol != ipx->pt)) {
		continue;
	    } else {
		if (flags & XNS_PEPCLIENT) {
		    if (ipx->pt == XNS_TYPE_PEP) { 
			pep = (xnspephdrtype *)ipxdatastart(pak);
			if (pep->pep_client_type != item->pep_clienttype)
			  continue;
		    }
		}
	    }
	}
	
	/*
	 * A network of -1 indicates all XNS networks.
	 * A socket of 0 indicates all XNS sockets.
	 */
	if (flags & XNS_SRCNET) {
	   if ((item->srcnet != -1L) && 
	       (item->srcnet != (GET2WD(ipx->ssnet) & item->srcnetmask))) 
		continue;
	}
	if (flags & XNS_SRCHOST) {
	    
	    /*
	     * Check for match on source host address
	     */

	    host_ul = GETLONG(ipx->shost);
	    if ((host_ul & item->srchostmask.u.lword[0])
		!= item->srchost.u.lword[0])
		continue;
	    host_us = GETSHORT(&ipx->shost[4]); 
	    if ((host_us & item->srchostmask.u.sword[2])
		!= item->srchost.u.sword[2])
		continue;
	}
	if (flags & XNS_SRCSOCKET) {
	    if (item->srcsocket && (item->srcsocket != ipx->ssock))
		continue;
	}
	if (flags & XNS_DSTNET) {
	    if ((item->dstnet != -1L) && 
		(item->dstnet != (GET2WD(ipx->ddnet) & item->dstnetmask)))
		continue;
	}
	if (flags & XNS_DSTHOST) {
	    /*
	     * Check for match on destination host address
	     */

	    host_ul = GETLONG(ipx->dhost);
	    if ((host_ul & item->dsthostmask.u.lword[0])
		!= (item->dsthost.u.lword[0]))
		continue;
	    host_us = GETSHORT(&ipx->dhost[4]); 
	    if ((host_us & item->dsthostmask.u.sword[2]) 
		!= item->dsthost.u.sword[2])
		continue;
	}
	if (flags & XNS_DSTSOCKET) {
	    if (item->dstsocket && (item->dstsocket != ipx->dsock))
		continue;
	}
	if (((accesslist >= MINENOVACC) && (accesslist <= MAXENOVACC)) &&
	    item->log) {
	  if (onintstack())
	    return(FALSE);  /* drop down to process level */
	  nov_accesslog_cache(accesslist, item->grant, ipx);
	} 
	return(item->grant);
    } while ((item = item->next));
    return(FALSE);
}

/*
 * novrip_out_filter
 * Check if the specified network number is permitted. Check both
 * distribute-lists and output-network-filter lists.
 */
boolean novrip_out_filter (novndbtype *ndb, ulong net, idbtype *idb)
{
    int list;
    novpdbtype *src_pdb;
    ulong target_net;

    /*
     * The target net to filter is from the route table entry, unless this
     * is the default route. In that case, filter on the actual target net.
     */

    target_net = ndb->dstnet;
    if ((ndb->dstnet == NOVELL_RIP_DEFAULT_ROUTE) && novell_default_route) {
	target_net = net;
    }

    /*
     * Check the old style output-network-filter.
     */

    if (!nov_filtercheck(target_net, idb->nov_out_filterlist))
	return (FALSE);
    /* 
     * Check if this destination is being filtered via the 
     * "distribute-list 800 out"
     */

    list = novrip_pdb.ofltr_all;
    if (!list)
        list = IPX_PRIMARY_NIDB(idb)->oprotofltr[novrip_pdb.index];
    if (list)
        if (!nov_filtercheck(target_net, list))
	    return (FALSE);
    /*
     * Check if this destination from this source protocol is 
     * being filtered via "distribute-list 800 out [src_protocol]".
     */

    if (!(src_pdb = NOV_INDEX2PDB(ndb->novpdbindex)))
	return(FALSE); 
    list = novrip_pdb.ofltr_proto[src_pdb->index];
    if (list && !nov_filtercheck(target_net, list))
	return(FALSE);

    return(TRUE);
}
    

/*
 * nov_set_nidb_idb
 *
 * set the pak->nif_input and pak->if_input
 * This routine was made from code remove from nov_input it
 * is called by nov_input and by the SAP queue checker.
 */
boolean nov_set_nidb_idb (pak)
    paktype *pak;
{
  ipxhdrtype *ipx;
  nidbtype *nidb, *default_interface_nidb, *tnidb;
  ulong dnet;

  ipx = (ipxhdrtype *)ipxheadstart(pak);

  /* 
   * Discover the correct logical input network descriptor
   * based upon input interface and (if IEEE medium) the
   * encapsulation. Discard the packet if a descriptor is
   * not found.
   *
   * Note that if the packet is received on a frame relay
   * interface, pak->if_input is valid. Scan the nidb list
   * looking for a network defined on the input swidb.
   *
   * On IEEE interfaces, Scan the nidb list looking for a
   * network on the same physical interface with the correct
   * encapsulation. 
   * If the nidb list comprises vLAN encapsulating IEEE type
   * subinterfaces then the encapsulation type will be a vLAN
   * encapsulation.
   *
   * On all other interfaces (i.e., serial) scan to the first
   * network with the input swidb.
   */

  default_interface_nidb = NULL;
  pak->nif_input = NULL;           /* Force legitimate assignment */

  tnidb = NULL;
  dnet = GET2WD(ipx->ddnet);

  if (pak->if_input->owner_nidb) {
    /*  if the input idb is a client, point to its owner */  
    nidb = pak->if_input->owner_nidb;
    change_if_input(pak, pak->if_input->owner_nidb->idb);
  } else if (is_wan(pak->if_input->hwptr) &&
	     (is_subinterface(pak->if_input) || 
	      (pak->enctype !=	ET_HDLC))) {
      FOR_ALL_NIDBS(nidb) {
	  if (nidb->idb == pak->if_input) {
	      if (is_atm_lane(pak->if_input)) {
		  ether_hdr *ether = (ether_hdr *)(pak->datagramstart);
		  ushort type_or_len = GETSHORT(&ether->type_or_len);
		  if (type_or_len <= MAXETHERSIZE) {
		      snap_hdr *snap = (snap_hdr *)ether->data;

		      switch (snap->dsap) {
			case SAP_SNAP:
			  if (nidb->enctype == ET_SNAP)
			      tnidb = nidb;
			  break;
			case SAP_NOVELL_ETHER:
			  if (nidb->enctype == ET_NOVELL_ETHER)
			      tnidb = nidb;
			  break;
			default:
			  /* ET_SAP */
			  if (nidb->enctype == ET_SAP)
			      tnidb = nidb;
			  break;
		      }
		  } else {
		      if (type_or_len == TYPE_NOVELL1) {
			  if (nidb->enctype == ET_ARPA)
			      tnidb = nidb;
		      }
		  }
		  if (tnidb != NULL) {
		      break;
		  }
	      } else {
		  tnidb = nidb;
	      }
	  }
      }
      nidb = tnidb;
  } else if (IS_VLAN_CONFIGURED_SUBINTERFACE(pak->if_input)) {

      FOR_ALL_NIDBS(nidb) {

  	  /* Note one hwidb may have several swidb vLAN subinterfaces
	   * with different IPX Network numbers, we have to use swidb
	   * instead of hwidb to search for corresponding nidb.
	   */

	   if (nidb->idb == pak->if_input)
	      break;
      }

  } else {

      FOR_ALL_NIDBS(nidb) {

	if (nidb->idb->hwptr == pak->if_input->hwptr) {

	     if ((pak->if_input->hwptr->status & IDB_IEEEMASK) &&
	             (nidb->enctype != pak->enctype)) {

	      continue;

	  } else {

	    /*
	     * Set the correct swidb.
	     *
             * We have a match on physical interface and
             * encapsulation (if IEEE interface). This is
             * good enough in all cases, except HDLC, where
             * it is good enough as a default (for forwarded
             * frames). However, if a packet is destined to
             * one of our directly connected network numbers,
             * for HDLC where multiple instances of the same
             * encapsulation are allowed, we want to use the
             * closest match.
	     */

	    if ((pak->enctype != ET_HDLC) ||
		(nidb->novellnet == dnet)) {
	      break;
	    } else if ((default_interface_nidb == NULL) &&
		       (NOVELL_NIDB_RECEIVES_ALLOWED(nidb))) {
	      default_interface_nidb = nidb;
	    }
	  }
	}
      }
    }

  /* Grab the default_interface_nidb, if it was set */

  if (nidb == NULL) {
    nidb = default_interface_nidb;
  }

  if (nidb != NULL) {
    pak->if_input = nidb->idb;

  } else {
#ifdef DEBUG_ENCAPS
    print_novell_encap((paktype *)pak, pak->if_input, "discard");
#endif
    nov_dispose(NOVELL_FORMATERROR, pak);
    return(FALSE);
  }
  
  /*
   * If the interface is not up don't accept any packets. Recall
   * that the physical interface may be up and receiving packets
   * while the network's subinterface may be administratively down.
   */
  if (!interface_up(nidb->idb)) {
    if (novell_debug)
      print_packet(pak, "interface not up");
    nov_dispose(NOVELL_FORMATERROR, pak);
    return(FALSE);
  }		
  
  /*
   * If the NIDB is 'going down' (or 'is down')
   * don't accept any packets
   */
  if (!NOVELL_NIDB_RECEIVES_ALLOWED(nidb)) {
    if (novell_debug)
      print_packet(pak, "network not up");
    nov_dispose(NOVELL_FORMATERROR, pak);
    return(FALSE);
  }		
  
  /*
   * Set logical input descriptor. This is important to
   * everything that follows.
   */

  pak->nif_input = nidb;
  return(TRUE);

}


/*
 * nov_access_delete
 * Delete a Novell style access list.
 */

void nov_access_delete (int number)
{
    xnseacctype *item, *wrk_item;

    item = ipx_get_ext_acl(number);
    ipx_set_ext_acl(number, NULL);
    while (item) {
	wrk_item = item->next;
	free(item);
	item = wrk_item;
    }
    access_mark_list_changed(number);
    return;
}

/*
 * nov_filtercheck
 * Check if the specified network number is permitted.
 */
boolean nov_filtercheck (ulong net, int accesslist)
{
    xnseacctype *item;
    ulong flags;

    if (!accesslist)
	return(TRUE);

    item = ipx_get_ext_acl(accesslist);

    if (item == NULL)
	return(TRUE);

    do {
	flags = item->flags;
	if (flags & XNS_SRCNET) {
	    if ((item->srcnet != -1L) &&
		(item->srcnet != (net & item->srcnetmask)))
		continue;
	}
	return(item->grant);
    } while ((item = item->next));
    return(FALSE);
}
