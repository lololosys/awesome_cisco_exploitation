/* $Id: decnet.c,v 3.13.18.31 1996/09/11 00:15:05 asastry Exp $
 * $Source: /release/112/cvs/Xsys/decnet/decnet.c,v $
 *------------------------------------------------------------------
 * decnet.c -- DECnet routing and gateway
 *
 * 5-Jun-87, C. Hedrick
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: decnet.c,v $
 * Revision 3.13.18.31  1996/09/11  00:15:05  asastry
 * CSCdi68358:  Changing DECnet L2 to L1 breaks L1 adj when Phase IV Prime
 * is used
 * Branch: California_branch
 *
 * Revision 3.13.18.30  1996/08/28  12:40:07  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.13.18.29  1996/08/27  02:35:27  rbadri
 * CSCdi67259:  LANE: Configuring DECnet changes MAC address only if LEC
 * is present
 * Branch: California_branch
 * Coerce the MAC address for ATM interfaces, not just for LANE.
 *
 * Revision 3.13.18.28  1996/08/14  02:36:04  hampton
 * Use static inline routines to clean up references to the access list
 * array.  [CSCdi65925]
 * Branch: California_branch
 *
 * Revision 3.13.18.27  1996/08/14  01:20:44  asastry
 * CSCdi60379:  VAX Routing phase IV node not seen in CLNS neighbor table.
 * Branch: California_branch
 *
 * Revision 3.13.18.26  1996/08/07  08:57:46  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.13.18.25  1996/08/03  23:21:41  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.13.18.24  1996/07/30  14:41:12  michellw
 * CSCdi64226:  Support larger max_frame_size (>1516) for LANE
 * Branch: California_branch
 * - The max_frame_size will be set to 1516 for ethernet and 4544 for TR
 *   if mtu is not configured on the sub-interface; otherwise, it will be
 *   set to based on the configured mtu.
 * - The lane module will not come up if the configured mtu is not valid.
 * - Add one more parameter (swidb) to the reg_invoke_media_change_mtu
 *   registry function.
 * - 1577 will come up with the default AIP mtu size (4470) if mtu is not
 *   configured on the sub-interface.
 *
 * Revision 3.13.18.23  1996/07/26  00:53:51  hampton
 * Fix the automore code in show access-list.  [CSCdi61084]
 * Branch: California_branch
 *
 * Revision 3.13.18.22  1996/07/16  09:36:08  hampton
 * Centralize knowledge of Decnet/Token Ring interaction.  [CSCdi63066]
 * Branch: California_branch
 *
 * Revision 3.13.18.21  1996/07/13  11:38:13  fred
 * CSCdi62873:  NULL dereference in fair-queue routines when called by
 *         traffic shaping - make hash_mask independent of caller
 * Branch: California_branch
 *
 * Revision 3.13.18.20  1996/07/02  23:10:36  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.13.18.19  1996/06/26  18:18:01  tylin
 * CSCdi60716:  ALIGN-3-SPURIOUS at 0x602E468C reading 0x391.
 * decnet/ip/clns
 *    Resolve null pointer reference.
 * Branch: California_branch
 *
 * Revision 3.13.18.18  1996/06/24  09:19:22  fox
 * CSCdi59550:  PPP causes router to crash
 * Branch: California_branch
 * Slim down and speed up linktype to NCP index to PPP protocol lookups.
 * Extend NCP protocheck functions to pass original IDB as well as config
 * IDB.  Change NCP names to upper case.  Display NCP name in trace of
 * PPP negotiation.
 *
 * Revision 3.13.18.17  1996/06/17  23:30:01  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.13.18.16  1996/06/12  19:08:43  pst
 * CSCdi60173:  Eliminate cisco definition of ntohl in favor of
 * swap_xx_bits (partial commit part 1)
 * Branch: California_branch
 *
 * Revision 3.13.18.15  1996/06/08  23:24:00  cheilek
 * CSCdi56786:  Interface Description Truncated
 * Branch: California_branch
 * Increase namestring display field width by 8.
 *
 * Revision 3.13.18.14  1996/06/01  00:26:31  rbadri
 * CSCdi58822:  show bri 0 causes crash in dncache_update
 * Branch: California_branch
 * Validate the thisroute->interface before using.
 *
 * Revision 3.13.18.13  1996/05/30  23:42:39  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.13.18.12  1996/05/30  01:19:22  asastry
 * CSCdi58810:  DEC 'advertise' route is wiped out by "dec routing"
 * command.
 * Branch: California_branch
 *
 * Revision 3.13.18.11  1996/05/29  19:07:45  asastry
 * CSCdi46634:  Misc DECnet code cleanup - get rid of unwanted registry
 * call.
 * Branch: California_branch
 *
 * Revision 3.13.18.10  1996/05/22  20:54:29  raj
 * CSCdi56721:  Debug ip mpacket doesnt show ttl and other important fields
 * Branch: California_branch
 * Added fields for "detail", and specifying acl as well as group.
 *
 * Revision 3.13.18.9  1996/05/17  10:43:55  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.13.4.3  1996/05/05  22:54:31  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.13.4.2  1996/04/26  07:33:25  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.13.4.1  1996/04/03  13:58:12  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.13.18.8  1996/05/10  01:37:04  wmay
 * CPP and ppp half bridge commit
 * Branch: California_branch
 *
 * Revision 3.13.18.7  1996/05/09  14:12:20  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.13.18.6  1996/05/07  18:47:10  widmer
 * CSCdi56253:  decnet access-group does not cause parser error without
 * decnet cost
 * Branch: California_branch
 * Print warning message for decnet commands if decnet is not
 * configured on interface.
 *
 * Revision 3.13.18.5  1996/05/04  00:36:14  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Bridge-group Virtual Interface smf support.
 * - Set up bridge/route indicators based on CRB/IRB is enabled or not.
 *
 * Revision 3.13.18.4  1996/04/29  09:36:35  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.13.18.3  1996/04/29  02:42:14  bcole
 * CSCdi55590:  configuring ipx routing  breaks IP ping
 * Branch: California_branch
 *
 * Add 'ipdiag' to the list of subsystem dependencies for subsystems which
 * support the ping command.
 *
 * Revision 3.13.18.2.4.1  1996/04/27  06:34:32  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.13.18.2  1996/04/17  13:36:31  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.13.18.1.8.1  1996/04/08  01:45:06  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.13.18.1  1996/03/18  19:11:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.13.22.1  1996/03/22  09:35:18  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.13.8.1  1996/03/05  05:49:10  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.13.2.1  1996/02/27  20:40:51  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.13  1996/02/08  03:19:02  anke
 * CSCdi48405:  Grammar are bad in output
 *
 * Revision 3.12  1996/02/07  16:10:41  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.11  1996/02/01  06:00:43  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.10  1996/01/29  07:27:07  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.9  1996/01/23  02:38:24  asastry
 * CSCdi47315:  DECnet IV/V conversion makes bogus entries in ISO-IGRP L2
 * routing table
 *
 * Revision 3.8  1996/01/22  23:20:10  asastry
 * CSCdi46327:  'show decnet static' shows bad SNPA for non-ethernet
 * static routes
 *
 * Revision 3.7  1996/01/18  20:52:13  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.6  1996/01/16  19:40:20  rchandra
 * CSCdi46063:  show decnet route shows high cost route as prefered route
 *  - do not display adjacency entries until we have heard routing update
 *
 * Revision 3.5  1996/01/11  23:09:39  hou
 * Change the counter name from "congestion" to "non-empty queue" in
 * "SHOW DECNET TRAFFIC" display.[CSCdi46602}
 *
 * Revision 3.4.6.2  1996/02/15  19:30:53  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Introduce a new routine to invalidate cache based on cache_version
 * in swidb (applies to ATM-LANE only).
 *
 * Revision 3.4.6.1  1996/02/13  00:17:15  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * DECnet routing for LANE
 *
 * Revision 3.4  1995/12/07  22:28:01  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.3  1995/11/17  08:58:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:14:46  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:15:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.17  1995/10/20  19:03:11  asastry
 * CSCdi41453:  Request for new knob: no decnet split-horizon (for frame
 * relay)
 *
 * Revision 2.16  1995/10/09  21:16:26  asastry
 * CSCdi39043:  DECnet route entry can have corrupted next-hop SNPA
 * address.
 *
 * Revision 2.15  1995/09/01  23:05:43  asastry
 * CSCdi39698:  WFQ: Display info (in "show queue") for non-IP protocols
 * as well
 *
 * Revision 2.14  1995/08/07  19:10:57  asastry
 * CSCdi33368:  Router shouldn't send DECnet Node Unreachable when dialing
 * on DDR
 *
 * Revision 2.13  1995/07/24  06:17:33  asastry
 * CSCdi36662:  Add a knob to override the behavior of the "L2 attached
 * bit". This is needed to allow L2 routers that are also doing IV/V
 * conversion to advertise the route to the "nearest L2 router" even when
 * they have no Phase IV area reachability.
 *
 * Revision 2.12  1995/07/17  23:16:25  hampton
 * Miscellaneous timer cleanups for DECNET.  [CSCdi37215]
 *
 * Revision 2.11  1995/07/09 21:09:18  speakman
 * CSCdi36848:  Configuration of CRB proves unpopular
 * Derive the CRB route/bridge behaviour from explicit, per-interface
 * route/bridge indicators rather than inferring it from protocol-
 * specific configuration.
 *
 * Revision 2.10  1995/07/07  23:04:31  asastry
 * CSCdi33292:  Access list for DECnet DDR dialer map does not function
 * properly when padding is present.
 *
 * Revision 2.9  1995/07/07  22:46:24  hampton
 * Convert DECNET to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi36830]
 *
 * Revision 2.8  1995/06/28 19:49:34  asastry
 * CSCdi36048:  router crashes after sending a DECnet ping.
 *
 * Revision 2.7  1995/06/28  19:42:31  asastry
 * CSCdi35322:  DECnet should convert to Phase V if next hop is in init
 * state
 *
 * Revision 2.6  1995/06/19  04:04:43  speakman
 * CSCdi35697:  General TB and SRB regression failures
 * Make protocol-specific route/bridge test explicit rather than relying
 * on per-interface rxtypes indicators to provide backward compatible
 * SRB and TB behaviour.
 *
 * Revision 2.5  1995/06/18  19:40:17  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.4  1995/06/18  06:18:15  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.3  1995/06/16 17:06:40  dkatz
 * CSCdi35956:  Timer declarations should be in a separate file
 * Eliminate obsolete macro references.
 *
 * Revision 2.2  1995/06/09  12:59:37  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:22:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*******************************************************************\
  
  The following license applies to institutions to which this software
  is given without charge by Rutgers University.  If you have received
  it from cisco Systems in for use in supporting cisco products, its use
  is covered by your license with cisco System, and not this license.
  
  You are permitted to use this software for your own internal,
  non-commercial purposes.  Copies of the software may be made only as
  necessary for such use, and for purposes of backup.  This software, or
  any copies thereof, may not be made available to or distributed to any
  person or installation outside your institution without the written
  consent of cisco Systems, Inc., or Rutgers University
  
  NO WARRANTY
  
  BECAUSE THIS SOFTWARE IS LICENSED FREE OF CHARGE, WE PROVIDE
  ABSOLUTELY NO WARRANTY, TO THE EXTENT PERMITTED BY APPLICABLE STATE
  LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING, RUTGERS UNIVERSITY AND
  CISCO SYSS PROVIDE THIS SOFTWARE "AS IS" WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
  PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME
  THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
  
  IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW WILL RUTGERS UNIVERSITY
  OR CISCO SYSS, INC. BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY
  LOST PROFITS, LOST MONIES, OR OTHER SPECIAL, INCIDENTAL OR
  CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE
  (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR DATA BEING RENDERED
  INACCURATE OR LOSSES SUSTAINED BY THIRD PARTIES OR A FAILURE OF THE
  PROGRAM TO OPERATE WITH PROGRAMS NOT DISTRIBUTED BY CISCO SYSS,
  INC.) THE PROGRAM, EVEN IF YOU HAVE BEEN ADVISED OF THE POSSIBILITY OF
  SUCH DAMAGES, OR FOR ANY CLAIM BY ANY OTHER PARTY.
  
  \*********************************************************************/

/*
  
  This is a really minimal DECnet router.  It is intended to use the
  same basic protocol as real DEC code, however a lot of details that
  we seem to be able to get away without have not been implemented:
  
  - We do not check the Ethernet addresses of arriving packets.
  Apparently when the source is an NI endnode and the destination
  is not, we are supposed to validate the first 4 bytes of the
  address.  Our routing is based entirely upon the 2-byte DECnet
  address, which we pull from the appropriate fields in the packet.
  
  - We do not use the official DECnet way of computing when to throw
  away packets due to congestion.  That is handled by the drivers,
  so we can't do much.  If we get a packet, we route it.
  
  - We ignore the maximum block size parameters. We are supposed to
  keep track of the largest packet each destination will accept,
  and drop packets that are too big.  We let the destination worry.
  As far as I know, DECnet does not require the gateway to fragment.
  If they did, we would of course have to implement that.
  
  - We do not send back error messages for undeliverable packets.
  
  - When routes change, we send a full routing update.  DEC sends 
  only some of them routes, including all of those that changed.
  Of course we can accept partial updates.
  
  - We do not check the test data sent by an end node in the hello.
  (We do, however, check the checksums used in routing updates.)
  
  - We do not limit the number of active adjacencies.  Our data
  structures do not require any static limits.
  
  - Our point to point support looks like Ethernet, not DECnet.
  Since cisco doesn't give us a DDCMP link layer, there's no
  way we could interoperate with a DEC router anyway.  So it's
  easier not to worry about their separate protocol for point 
  to point.
  
  - Update timers are per/interface.  DEC has separate update
  timers for broadcast and point to point as two classes.
  
  - Notes about split paths:
  The interrupt code assumes that if route->nextsplit is nonzero
  then the pointer will be valid and route->numroutes will be
  correct.  Always clear route->nextsplit before changing the
  route table.  In general we unsplit a route before changing
  it. New routing updates will add equal cost paths back in if they
  exist.  This is much less complicated than trying to make 
  dn_recomp() find split paths.
  */


#include "master.h"
#include "logger.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../decnet/msg_decnet.c"	/* Not a typo, see logger.h */
#include <string.h>
#include <ciscolib.h>
#include <stdarg.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "ttysrv.h"
#include "packet.h"
#include "linktype.h"
#include "ieee.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "../clns/clns_externs.h"
#include "../clns/clns.h"
#include "access.h"
#include "parser_defs_dec.h"
#include "../if/priority.h"
#include "decnet_parser.h"
#include "decnet_echo.h"
#include "dnconversion.h"
#include "decnet_externs.h"
#include "decnet_debug.h"
#include "../if/network.h"
#include "../if/priority_private.h"
#include "../ip/ip.h"
#include "../util/regexp.h"
#include "../if/tring.h"
#include "../if/atm.h"
#include "../lane/lane.h"
#include "name.h"
#include "../wan/ppp.h"
#include "../wan/ppp_registry.h"

#ifndef FIXLSWAP
#define FIXLSWAP(x) x
#endif

/*
 * The DECnet subsystem header
 */

#define DEC_MAJVERSION 1
#define DEC_MINVERSION 0
#define DEC_EDITVERSION  1

SUBSYS_HEADER(decnet, DEC_MAJVERSION, DEC_MINVERSION, DEC_EDITVERSION,
	      dn_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: iphost, ipdiag, ipservice, ppp",
	      NULL);

/*
 * Storage declarations
 */

watched_queue *DNpacketQ; /* DECnet input queue */
queuetype DNechoQ;		/* DECnet echo queue */
queuetype decnet_staticQ;       /* Queue of DECnet static routes */
queuetype decnet_advQ   ;       /* Queue of DECnet 'advertise' routes */
dnroutetype *decnet_default_route; /* pointer to a DECnet default  route */

dnnet *dn_nets[DN_MAXNETS];	/* pointers to each network structure */	
static dnnet *dn_netsx[DN_MAXNETS]; /* incomplete network structures */

static uchar dn_e_gwaddr[IEEEBYTES]; /* idb->encaps variable for Ethernets  */

/* only uses 4 bytes, last 2 are passed */
uchar *dn_etheraddr = NULL;	/* points to a valid decnetaddr */
int dn_stat[DNST_MAX];
sys_timestamp dn_special_L1_routing; /* time for triggered L1 update */
sys_timestamp dn_special_L2_routing; /* time for triggered L2 update */
sys_timestamp dn_special_hellos;     /* time for triggered hello update */
static sys_timestamp dn_next_routing; /* next regular routing update */
static sys_timestamp dn_next_hello;   /* next hello update */
static sys_timestamp dn_next_activation; /* next scheduled activing of router */
static pid_t dn_input_pid;	    /* process id for DECnet Input */
static pid_t dn_route_pid;	    /* process id for DECnet Routing */
static boolean dn_router_running; /* router process running */
boolean dn_running;	   	/* process decnet - OK to toggle this */
static boolean dn_atg;			
idbtype *dn_echo_interface;
boolean dn_conversion;		/* TRUE if DECnet conversion is enabled */
boolean dn_attach_override;     /* TRUE if configured to behave as 'attached'
				   regardless of connectivity to L2 areas */

static void decnet_enqueue (paktype *);


/*
 * fair conversation generator
 */
static
short decnet_fair_queue_flow_id (paktype *pak)
{
    dnhdrtype *dn;
    uchar     *data;
    int        flags;
    int	       sum;

    dn = (dnhdrtype *)dnheadstart(pak);
    data = dn->pakstart;
    flags = *data;
    if (DN_PAD & flags) {
	/* turn off PAD flag, rest is "length of pad" ;
	   step past the pad byte */
	data += flags - DN_PAD;
	flags = *data;
    }
    if (DN_CTL & flags) {
	return(linktype_is_fq_conversation(pak));
    } else {
	dndatapacket *hdr = (dndatapacket *) data;
	pak->fair_weight = FAIR_QUEUE_BASE_WEIGHT;
	sum = hdr->dest[0];
	sum += hdr->dest[1];
	sum += hdr->source[0];
	sum += hdr->source[1];
	return(fair_queue_sum(pak, sum));
    }
}

/*
 * decnet_print_network_header
 *
 * Print DECnet header.
 */
static void decnet_print_network_header (paktype *pak, ushort **data)
{
    dnhdrtype *dn;
    uchar *pad_byte;
    uchar flags;
    dn = (dnhdrtype *)dnheadstart(pak);
    pad_byte = dn->pakstart;
    flags = *pad_byte;
    if (DN_PAD & flags) {
	/* turn off PAD flag, rest is "length of pad" ;
	   step past the pad byte */
	pad_byte += flags - DN_PAD;
	flags = *pad_byte;
    }
    *data = (ushort *)pad_byte;
    if (!(DN_CTL & flags)) {
	dndatapacket *hdr = (dndatapacket *) pad_byte;
	printf("\n  source %v destination %v",
	       (hdr->source[1]*256) + hdr->source[0],
	       (hdr->dest[1]*256) + hdr->dest[0]);
    }
}

/*
 * Decnet keyword ID and STRings
 */
static const id_string dnconnect_items[] = {
    { DNID_EQ, DNSTR_EQ},
    { DNID_NEQ, DNSTR_NEQ},
    { DNID_LT, DNSTR_LT},
    { DNID_GT, DNSTR_GT},
    {0, NULL}
};

/*
 * Return the correct broadcast string based on link type. 
 */
static uchar *dn_return_bcast_str (uchar type)
{
    switch (type) {
        case LINK_DECNET_NODE: return ("end nodes");
        case LINK_DECNET_ROUTER_L1: return ("PhaseIV routers");
        case LINK_DECNET_ROUTER_L2: return ("PhaseIV+ routers");
        case LINK_DECNET_PRIME_ROUTER: return ("PhaseIV prime routers");
	default: return ("");
    }
}

/*
 * Lookup a Decnet connect item name.
 */
static char *dn_connect_str (ushort id)
{
    return(lookup_id_string(dnconnect_items, id));
}


/*
 * decnet_enable_test
 * Return TRUE if we're running DECnet
 * Called by SERVICE_PROTO_RUNNING.
 */

static boolean decnet_enable_test (void)
{
    return(dn_running);
}

/*
 * decnet_routing_idb
 * Return TRUE iff DECNET packets inbound on this interface should be
 * routed.
 */

static boolean decnet_routing_idb (idbtype *swidb)
{
    if ((global_crb_enable || global_irb_enable)
	&& swidb->tbridge_on_swidb) {
	/*
	 * For CRB, just return the decnet route/bridge indicator
	 * if this interface is in a bridge group.  This is
	 * set by the "bridge <n> route decnet" command.
	 */
	return(swidb->tbridge_route_linktypes & LINK_DECNET_RBI);
    } else {
	/*
	 * Otherwise, just verify that decnet routing is enabled,
	 * and this interface is enabled for decnet.  This latter
	 * is set by the "decnet cost" command.
	 */
	return(dn_running && (swidb->dn_cost > 0));
    }
}

/*
 * decnet_bridging_idb
 * Return TRUE iff DECNET packets inbound on this interface should be
 * bridged.
 */

static boolean decnet_bridging_idb (idbtype *swidb)
{
    if (global_crb_enable) {
	/*
	 * For CRB, just return the complement of the decnet
	 * route/bridge indicator if this interface is in
	 * a bridge group.  This is cleared by the
	 * "no bridge <n> route decnet" command (the default).
	 */
	return(swidb->tbridge_on_swidb &&
	       (!(swidb->tbridge_route_linktypes & LINK_DECNET_RBI)));
    } else if (global_irb_enable) {
	return(swidb->tbridge_on_swidb &&
	       !(swidb->tbridge_bridge_linktypes & LINK_DECNET_RBI));
    } else {
	/*
	 * To support SRB and the archaic transparent bridging
	 * behaviour just check for the absence of decnet routing.
	 */
	return(!dn_running);
    }
}

/*
 * decnet_rxtypes_update
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

static void decnet_rxtypes_update (idbtype *swidb)
{
    boolean rte_dn, br_dn;

    if (global_crb_enable || global_irb_enable) {
	rte_dn = decnet_routing_idb(swidb);
	br_dn = decnet_bridging_idb(swidb);
    } else {
	rte_dn = dn_running;
	br_dn = !rte_dn;
    }

    if (rte_dn) {
	SET_ROUTING_RXTYPE(swidb, RXTYPE_DECNET);
	SET_ROUTING_RXTYPE(swidb, RXTYPE_SNAP_DECNET);
    } else {
	CLEAR_ROUTING_RXTYPE(swidb, RXTYPE_DECNET);
	CLEAR_ROUTING_RXTYPE(swidb, RXTYPE_SNAP_DECNET);
    }

    if (br_dn) {
	CLEAR_BRIDGING_RXTYPE(swidb, RXTYPE_DECNET);
	CLEAR_BRIDGING_RXTYPE(swidb, RXTYPE_SNAP_DECNET);
    } else {
	SET_BRIDGING_RXTYPE(swidb, RXTYPE_DECNET);
	SET_BRIDGING_RXTYPE(swidb, RXTYPE_SNAP_DECNET);
    }

    if (swidb->hwptr->setup_bridging)
	(*(swidb->hwptr->setup_bridging))(swidb->hwptr);

    reg_invoke_bridge_setup();	/* sse_bridging_setup_enqueue() */
}

/*
 * decnet_showproto
 * Interface specific portion of "show protocols" commands
 * Called by SERVICE_SHOWPROTO
 */

static void decnet_showproto (idbtype *idb)
{
    if (idb == NULL) {
	if (RUNNING(LINK_DECNET)) {
	    printf("\n  DECNET routing is enabled");
	}
	return;
    }
    if (RUNNING_IF(LINK_DECNET, idb)) {
	printf("\n  Decnet cost is %d", idb->dn_cost);
    }
}

/*
 * Currently this function is useful to update the cache info 
 * For LANE interfaces only. But, it is generic enough to locate the
 * route entry and so updates for any particular link can be done based
 * on the interface off the route entry.
 */
static void dncache_update (paktype *pak, ulong address)
{
    dnroutetype *thisroute = NULL;
    dnhdrtype *dn;
    uchar *pad_byte;
    uchar flags;
    ushort index;
    dnnet *net;
    dnroutetype **table;
    int hisnode, hisarea;
    dndatapacket *hdr ; 

    dn = (dnhdrtype *)dnheadstart(pak);
    pad_byte = dn->pakstart;
    flags = *pad_byte;
	
    if (DN_PAD & flags) {
	/* 
	 * turn off PAD flag, rest is "length of pad" ;
	 * step past the pad byte 
	 */
	pad_byte += flags - DN_PAD;
	flags = *pad_byte;
    }
    hdr = (dndatapacket *) pad_byte;
    /*
     * Now that you have the decnet data packet, it should be
     * easy to decode, find the route entry corresponding to
     * to the destination and then write the cache info
     */
    net = dn_nets[pak->if_output->dn_network];
    if (net == NULL) {
	dn_dispose(DN_NOTNET, pak);
	return;
    }
    hisarea = dn_area(hdr->dest);
    hisnode = dn_node(hdr->dest);
    if (hisarea == net->my_dn_area) {
	table = net->dnroutes;
	index = hisnode;
    } else {
	table = net->dnareas;
	index = hisarea;
    }
    thisroute = table[index];

    /*
     * For LANE, the encapsulation info has to be cached so that
     * fast switching can be done based on the route entry.
     * We store the VCD, Enc_type and LEC_ID in the encap_info
     * The validity of the cache is inferred from the swidb's
     * cache_version field
     */
    if (thisroute && thisroute->interface &&
	is_atm_lane(thisroute->interface)) {
	ushort *encap_info;
	ushort *cache_info;
	encap_info = (ushort *)thisroute->encap_info;
	cache_info = (ushort *)pak->datagramstart;
	*encap_info++ = *cache_info++;
	*encap_info++ = *cache_info++;
	*encap_info   = *cache_info;
	thisroute->cache_version = thisroute->interface->cache_version;
    }
}

static void
dncache_invalidate (hwidbtype *hwidb, idbtype *swidb, boolean immediately)
{
    /* 
     * We would like to validate cache based on the swidb's cache_version.
     * This is used by ATM-LANE to effectively fast switch DECnet packets.
     */
    if (swidb) {
	swidb->cache_version++;
    }
}

/*
 * dn_init_idb
 * Initialize an interface for DECnet (split out so we can add interfaces
 * after decnet has already started (e.g. the tunnel)).
 */

void dn_init_idb (idbtype *idb, boolean setup_hwidb)
{
    dn_info *dn_tr;

    idb->dn_cost = -1;
    decnet_rxtypes_update(idb);
    idb->dn_prio = 0;		/* no priority set yet */
    idb->dn_dr_prio = 0;	/* DR's priority */
    idb->dn_network = 0;
    idb->dn_route_time = DN_ROUTE_TIME;
    idb->dn_hello_time = DN_HELLO_TIME;
    idb->dn_maxblksize = DN_MAXPACKET;
    idb->dn_hismtu = DN_MAXPACKET;
    idb->dn_split_horizon = TRUE;

    if (setup_hwidb) {
	idb->hwptr->dn_routecache = TRUE;
 	if (idb->hwptr->status & IDB_TR) {
	    idb->hwptr->dn_func = malloc_named(sizeof(dn_info),
					       "Decnet Address");
	    if (!idb->hwptr->dn_func) {
	        return;
	    }
	    dn_tr = idb->hwptr->dn_func;
 	    idb->hwptr->dn_congest_limit = DN_DEF_CATHRESH;
 	    dn_tr->dn_tr_iv_routers.upper = TRFUNC_UPPER;
 	    PUTLONG(&dn_tr->dn_tr_iv_routers.lower, TRFUNC_D_DN_ROUTE);
 	    dn_tr->dn_tr_iv_prime_routers.upper = TRFUNC_UPPER;
 	    PUTLONG(&dn_tr->dn_tr_iv_prime_routers.lower, TRFUNC_D_DN_ROUTE);
 	    dn_tr->dn_tr_iv_endnodes.upper = TRFUNC_UPPER;
 	    PUTLONG(&dn_tr->dn_tr_iv_endnodes.lower, TRFUNC_D_DN_NODE);
 	}
	idb->hwptr->dn_congest_limit = DN_DEF_CATHRESH;
	idb->dn_L1_adj_count = 0;
	idb->dn_old_phaseIV_adj_count = 0;
	idb->dn_phaseIV_plus_adj_count = 0;
    }
    
    dn_fastsetup(idb->hwptr);
}

/*
 * dn_register_thyself
 */
static void dn_register_thyself (void)
{
    decnet_parser_init();
    decnet_media_registry();
    register_dec_route_routines();

    reg_add_fast_setup(dn_fastsetup, "dn_fastsetup");
    reg_add_fs_cache_update(LINK_DECNET, dncache_update, "dncache_update");
    reg_add_bump_cache_version(dncache_invalidate, "dncache_invalidate");
    reg_add_sw_idb_init(dn_init_idb, "dn_init_idb");
    reg_add_proto_on_swidb(LINK_DECNET, decnet_on_idb, "decnet_on_idb");
    reg_add_routed_proto_on_swidb(LINK_DECNET, decnet_on_idb, "decnet_on_idb");
    reg_add_printf((long) 'v', dn_printf, "dn_printf");
    reg_add_printf((long) 'V', dn_printf, "dn_printf");
    reg_add_ping_run(LINK_DECNET, dn_ping, "dn_ping"); 
    reg_add_ping_parse(decnet_ping_parse, "decnet_ping_parse"); 
    reg_add_proto_running(LINK_DECNET,
		      decnet_enable_test, "decnet_enable_test");
    reg_add_proto_bridging_on_swidb(LINK_DECNET, decnet_bridging_idb,
				    "decnet_bridging_idb"); 
    reg_add_proto_routing_on_swidb(LINK_DECNET, decnet_routing_idb,
				   "decnet_routing_idb");
    reg_add_rxtypes_update(decnet_rxtypes_update,"decnet_rxtypes_update");
    reg_add_access_show(show_dnaccess, "show_dnaccess");
    reg_add_showproto(decnet_showproto, "decnet_showproto");
    reg_add_setup_global(decnet_setup_global, "decnet_setup_global");
    reg_add_setup_int(decnet_setup_int, "decnet_setup_int");
    reg_add_setup_end(decnet_setup_end, "decnet_setup_end");
    reg_add_determine_fair_queue_flow_id(LINK_DECNET,
					 decnet_fair_queue_flow_id,
					 "decnet_fair_queue_flow_id");
    reg_add_print_network_header(LINK_DECNET, decnet_print_network_header,
					 "decnet_print_network_header");
    register_snmp_decnet_echo();
}
/*
 * dn_init
 * Initialize the DECnet protocol handling.
 * Allow bridging by hardware until routing is turned on.
 */

void dn_init (subsystype *subsys)
{
    idbtype *idb;
    int i;

    TIMER_STOP(dn_special_L1_routing);
    TIMER_STOP(dn_special_L2_routing);
    TIMER_STOP(dn_special_hellos);
    TIMER_STOP(dn_next_routing);
    TIMER_STOP(dn_next_hello);
    TIMER_STOP(dn_next_activation);

    dn_register_thyself();
    decnet_debug_init();

    FOR_ALL_SWIDBS(idb)
	decnet_rxtypes_update(idb);
    DNpacketQ = NULL;
    queue_init(&DNechoQ, 0);
    queue_init(&decnet_staticQ, 0);
    queue_init(&decnet_advQ, 0);
    
    for (i = 0; i < DN_MAXNETS; i++) {
        dn_nets[i] = NULL;
	dn_netsx[i] = NULL;
    }
    
    ieee_copy(dn_e_baseaddr, dn_e_gwaddr);
    ieee_copy(dn_t_baseaddr, dn_t_gwaddr);
    
    dn_router_running = FALSE;
    dn_running = FALSE;
    dn_atg = FALSE;
    dn_conversion = FALSE;
    dn_attach_override = FALSE;
    decnet_default_route = NULL;

    FOR_ALL_SWIDBS(idb) {
	dn_init_idb(idb, TRUE);
    }

    /*
     * Register DECCP with PPP
     */
    reg_invoke_ncp_register(LINK_DECNET, TYPE_PPP_DECCP,
			    NULL, NULL, "DECCP");
    reg_invoke_ncp_register_link(LINK_DECNET_ROUTER_L1, 0, LINK_DECNET);
    reg_invoke_ncp_register_link(LINK_DECNET_ROUTER_L2, 0, LINK_DECNET);
    reg_invoke_ncp_register_link(LINK_DECNET_PRIME_ROUTER, 0, LINK_DECNET);
    reg_invoke_ncp_register_link(LINK_DECNET_NODE, 0, LINK_DECNET);
}

/*
 * dn_initnet
 * Initialize a DECnet network.
 */

dnnet *dn_initnet (int n)
{
    dnnet *net;
    int i;
    
    net = dn_netsx[n];
    if (net) {		/* if already initialized */
        return(net);
    }
    
    /*
     * Check to see if we will have more than one DECnet.
     */
    for (i = 0; i < DN_MAXNETS; i++) {
	if (dn_netsx[i] != NULL) {
	    dn_atg = TRUE;		/* have multiple nets now */
	    break;
	}
    }
    
    net = malloc_named(sizeof(dnnet), "DECnet Network");
    if (net == NULL) {
        return(NULL);
    }
    
    /*
     * At this point, we may be creating a dnnet structure so we can build
     * a translation map, or are setting parameters other than the decnet
     * address.  To be safe, we don't allow using the struct until the
     * decnet address is set.  dn_setaddr() will do this.
     */
    dn_netsx[n] = net;
    net->netn = n;
    dn_var_init(net);
    
    return(net);
}

/*
 * dn_cleanup
 *
 * Stop DECnet routing, kill processes and stop all the timers.
 */
static void dn_cleanup (void)
{
    paktype *pak;

    if (dn_router_running) {
	TIMER_STOP(dn_special_L1_routing);
	TIMER_STOP(dn_special_L2_routing);
	TIMER_STOP(dn_special_hellos);
	TIMER_STOP(dn_next_routing);
	TIMER_STOP(dn_next_hello);
	TIMER_STOP(dn_next_activation);

	reg_delete_raw_enqueue(LINK_DECNET);
	process_watch_queue(DNpacketQ, DISABLE, RECURRING);
	while ((pak = process_dequeue(DNpacketQ)) != NULL)
	    datagram_done(pak);
	delete_watched_queue(&DNpacketQ);

	process_kill(dn_input_pid);
	process_kill(dn_route_pid);

	dn_router_running = FALSE;
    }
}

/*
 * dn_input
 * The process that redistributes DECnet datagrams
 */

static process dn_input (void)
{
    paktype *pak;
    dnhdrtype *dn;
    uchar *data;
    int size, firstbyte, pak_count;
    
    process_wait_on_system_init();

    /*
     * Set up this process' world.
     */
    DNpacketQ = create_watched_queue("DECnet Input", 0, 0);
    process_watch_queue(DNpacketQ, ENABLE, RECURRING);
    reg_add_raw_enqueue(LINK_DECNET, decnet_enqueue, "decnet_enqueue");

    while (TRUE) {
	process_wait_for_event();

	pak_count = PAK_SWITCHCOUNT;

	while (TRUE) {
	    /*
 	     * Process only a limited number of packets per pass
	     */
	    if (pak_count-- == 0) {
		break;
	    }

	    pak = process_dequeue(DNpacketQ);
	    if (pak == NULL) {
		break;
	    }

	    dn_stat[DNST_RECEIVED]++;
	    idb_increment_rx_proto_counts(pak, ACCT_PROTO_DECNET);

	    dn = (dnhdrtype *)dnheadstart(pak);

	    size = get16(dn->paksize);
	    data = dn->pakstart;
	    
	    if ((size + DN_PAKSIZE_FIELD_LEN) >
		(pak->datagramsize -
		 (pak->network_start - pak->datagramstart)) ||
		(size > DN_MAXPACKET)) {
		dnet_pabuginf("\nDNET: bad length: hdw=%d, sft=%d",
			   pak->datagramsize - 
			        (pak->network_start - pak->datagramstart),
			   size + DN_PAKSIZE_FIELD_LEN);
		dn_dispose(DN_FORMATERR,pak);
		continue;
	    }	   

	    /* skip padding bytes */
	    firstbyte = 0;
	    if ((size--) > 0 && (firstbyte = *(data++)) & DN_PAD) {
	        int padcount = firstbyte & 0x7f;
		dnet_pabuginf("\nPadding bit is on in this packet!"
	                      " Pad byte is %x, length is %d",
			        firstbyte, padcount);

		data += padcount - 1;  /* already skipped itself */
		size -= padcount;  /* no -1 because includes firstbyte */
		if (size <= 0) {
		    dnet_pabuginf("\nDNET: Format error (size) "
				  "disposing of packet");
		    dn_dispose(DN_FORMATERR,pak);
		    continue;
		}
		firstbyte = *(data++);
	    }
	    if (firstbyte & DN_CTL) {
		dn_control(pak, data, size, firstbyte);
	    } else {
		dn_packet_forward(pak, (dndatapacket *)(data-1), firstbyte);
	    }
	}
    }
}

/*
 * pick_dn_802addr
 * Perform MAC level address selection for DECnet
 */
static uchar *pick_dn_802addr (idbtype *idb)
{
    int i;
    hwidbtype *hwidb;
    dnnet *net;
    uchar *addr = NULL;
    static uchar tring_addr[IEEEBYTES];
    
    /*
     * If the mac address has been forced, always use that. Assume 
     * other checking has insured that this is a "legal" forced
     * mac address (for the iv_prime cases).
     */
    hwidb = idb->hwptr;
    if (hwidb &&
	(((ushort *)hwidb->forced_mac)[0] |
	 ((ushort *)hwidb->forced_mac)[1] |
	 ((ushort *)hwidb->forced_mac)[2]))
	return hwidb->forced_mac;

    /*
     * If DECnet is up, try to pick an appropriate address even if
     * DECnet hasn't been fully configured yet.
     */
    net = dn_nets[idb->dn_network];
    if (net) {
	addr = net->decnetaddr;
    } else {
	for (i = 0; i < DN_MAXNETS; i++) {
	    net = dn_nets[i];
	    if (net) {
		addr = net->decnetaddr;
		break;
	    }
	}
    }
    if (addr == NULL)
	return(NULL);
    if (!idb_is_tokenring(idb))
	return(addr);

    /*
     * KLUDGE ALERT -- KLUDGE ALERT.
     *
     * The configuration says to turn decnet on for this interface.
     * Since the Decnet always uses ethernet addresses, and an
     * ethernet address is a multicast on token ring we bit swap the
     * entire address.  Write the address a byte at a time to prevent
     * big/little endian problems.
     */
    tring_addr[0] = (DN_MAC_TR >> 24) & 0xFF;
    tring_addr[1] = (DN_MAC_TR >> 16) & 0xFF;
    tring_addr[2] = (DN_MAC_TR >> 8) & 0xFF;
    tring_addr[3] = (DN_MAC_TR) & 0xFF;
    tring_addr[4] = BITSWAP(addr[4]);
    tring_addr[5] = BITSWAP(addr[5]);
    return(tring_addr);
}

/*
 * dn_setaddr
 * Set up DECnet addressing and routing tables
 */

void dn_setaddr (int area, int node, dnnet *net, boolean iv_prime)
{
    idbtype *idb;
    
    /*
     * DECnet always runs canonically.  That is it always uses an
     * ethernet address.  When it hits a token ring or a fddi the mac
     * addresses are bitswapped.
     */
    
    ieee_copy(dn_e_baseaddr, net->decnetaddr);
    dn_addr(net->decnetaddr+4,area,node);	/* build address */
	
    if (dn_etheraddr == NULL) {
	dn_etheraddr = net->decnetaddr;
    }

    net->my_dn_area = area;
    net->my_dn_node = node;
    net->my_dn_address = swap_16bit_word((area << 10) | node);  /* XXX swapped! 68K dependent! */
    net->iv_prime = iv_prime;

    dn_flushroutes(net);		/* rebuild the route tables */
    
    dn_running = TRUE;			/* TR reset uses this */
    
    FOR_ALL_SWIDBS(idb) {
	decnet_rxtypes_update(idb);	/* enable routing */
	if (idb->dn_cost != -1) {
	    dn_enable_if(idb);
	}
	idb->dn_L1_adj_count = 0;
        idb->dn_old_phaseIV_adj_count = 0;
        idb->dn_phaseIV_plus_adj_count = 0;

    }
    
    /*
     * Start DECnet processes if none already running.
     */
    if (dn_router_running == FALSE) {
	dn_input_pid = process_create(dn_input, "DECnet Input", NORMAL_STACK,
				      PRIO_HIGH);
	dn_route_pid = cfork(dn_route, 0, 900, "DECnet Routing",
			     CONSOLE_LINENUM);
	dn_router_running = TRUE;
    }
}

/*
 *  dn_flushroutes -- flush all the routes
 */

void dn_flushroutes (dnnet *net)
{
    dnroutetype *route,**routeptr;
    
    /*
     * Clear out all the routes, being careful of the pointer to 
     * the area router which may be a copy.
     */
    for (routeptr = net->dnareas; routeptr < &net->dnroutes[DN_MAXNODES];
	 routeptr++) {
	if (routeptr == &net->dnroutes[0]) {
	    continue;
	}
	if (*routeptr == net->dnroutes[0]) { /* if duplicate pointer */
	    net->dnroutes[0] = NULL;
	}
	dn_free_rt(routeptr);
    }
    if (net->dnroutes[0]) {
	dn_free_rt(&net->dnroutes[0]);
    }
    net->dn_maxroute = NULL;
    
    /*
     * Here to set up our own route.
     * All we set is DN_RT_ME.  We don't make a valid entry because we
     * don't want ourselves to show up in our list of adjacent nodes.
     * Sendroutes just uses DN_RT_ME.  This should be improved.
     */
    routeptr = &net->dnroutes[net->my_dn_node];
    if (*routeptr == NULL) {
	route = malloc(sizeof(dnroutetype));
	if (route == NULL) {
	    return;
	}
	if (routeptr > net->dn_maxroute) {
	    net->dn_maxroute = routeptr;
	}
	*routeptr = route;
    } else {
	route = *routeptr;
    }
    
    if (net->doareas) {
	net->attached = TRUE;
	net->dnroutes[0] = route;	/* we are nearest area router */
    }
    
    dn_clear_rt(route);
    route->interface = NULL;
    route->hops = 0;
    route->cost = 0;
    route->flags = DN_RT_ME;
    route->prio = 0;
    route->type = PHASEIV;
    /*
     * Ditto for area table
     */
    routeptr = &net->dnareas[net->my_dn_area];
    if (*routeptr == NULL) {
	route = malloc(sizeof(dnroutetype));
	if (route == NULL) {
	    return;
	}
	*routeptr = route;
    } else {
	route = *routeptr;
    }
    
    dn_clear_rt(route);
    route->interface = NULL;
    route->hops = 0;
    route->cost = 0;
    route->flags = DN_RT_ME;
    route->prio = 0;
    
    dn_maproutes(net);			/* build routes to mapped nodes */
}

/*
 * dn_maproutes - build routes to mapped nodes
 */

void dn_maproutes (dnnet *net)
{
    int i, area, node;
    dnmap *map;
    
    for (i = 0; i < DNNHASH; i++) {
	map = net->nethash[i];
	while (map) {
	    area = map->fromaddr >> 10;
	    node = map->fromaddr & 0x3FF;
	    dn_addmaproute(area, node, net, map);
	    
	    map = map->next;
	}
    }
}


/*
 * dn_addmaproute
 *
 * Add a map route to our routing table.
 */

void dn_addmaproute (int area, int node, dnnet *net, dnmap *map)
{
    dnroutetype **routeptr;
    dnroutetype *route,**table, *thisroute;
    int index, cost, hops;
    
    if (net == NULL) {
	return;
    }
    
    /*
     * Set static metrics using the default or value in the map.
     *
     * I had planned to make the metrics advertised for mapped routes reflect
     * the actual metrics in the destination network.  I didn't becuase:
     * 1. When map entries create nodes in an area that is not my area, many
     *    metrics in another network might map into one route on this network.
     *    There is no good way to pick the correct metric.
     * 2. Keeping the advertised metrics up to date would be expensive.
     * So, until some real need is demonstrated, I'm going to leave
     * the static metrics.
     *
     * The administrative metrics should allow you to have more than one
     * translating gateway into another network; this hasn't really been
     * tried yet as of this writing.
     */
    hops = cost = 1;
    if (map->cost) {			/* if set administratively */
	hops = map->hops;
	cost = map->cost;
    }
    
    if (area == net->my_dn_area) {
        table = net->dnroutes;
        index = node;
    } else {
        table = net->dnareas;
        index = area;
    }
    
    /*
     * Look for an existing mapped route and remove it.  We remove it 
     * because if the cost has changed it may go back in somewhere else.  
     * Routeptr points to the previous element so we can do the deletion.
     * There can only be one mapped route, although there may be another
     * better route to this destination.  Hopefully, its a route to another
     * mapping router, else there's a config problem.
     */
    
    routeptr = &table[index];
    thisroute = *routeptr;
    while (thisroute) {
	if (thisroute->flags & DN_RT_NET) {
	    *routeptr = thisroute->nextadj;
	    break;
	}
	routeptr = &thisroute->nextadj;
	thisroute = thisroute->nextadj;
    }
    
    /*
     * Create new adjacency if needed.
     */
    
    if (thisroute == NULL) {  /* need new adjacency */
	dnet_robuginf("\nDNET-RT: creating mapped route to %d.%d", area, node);
	thisroute = malloc(sizeof(dnroutetype));
	if (!thisroute) {
	    return;
	}
    }
    
    /*
     * Now put it in the right place.
     */
    
    routeptr = &table[index];
    route = *routeptr;
    if (routeptr > net->dn_maxroute) {
	net->dn_maxroute = routeptr;
    }
    
    /*
     * 1. If its an indirect route and the cost is better then skip it else
     *    remove the indirect route.
     * 2. If there's a HELLO, we have a configuration problem -- the user
     *    has entered a mapping entry that conflicts with a real node.
     *    (There might be a problem with an indirect route but we can't 
     *    tell.)
     * 3. If there's a NET entry, we have a code bug.
     */
    
    while (route && ((route->flags & DN_RT_HELLO) == 0)) {
	if (route->cost >= cost) {
	    dnet_robuginf("\nDNET-RT: route to %d.%d superceded by map",
		       dn_area(route->gateway), dn_node(route->gateway));
	    *routeptr = route->nextadj;
	    free(route);
	} else {
	    routeptr = &route->nextadj;
	}
	route = *routeptr;
    }
    
    if (route) {
	errmsg(&msgsym(MAPCON, DNET), area, node,
	       dn_area(route->gateway), dn_node(route->gateway));
    }
    
    thisroute->nextadj = route;
    *routeptr = thisroute;
    
    thisroute->interface = NULL;
    thisroute->flags = DN_RT_NET;
    thisroute->hops = hops;
    thisroute->cost = cost;
    thisroute->type = PHASEIV;
}

void dn_free_rt (dnroutetype **routeptr)
{
    dnroutetype *route,*nextroute;

    nextroute = *routeptr;
    if (nextroute == NULL) {
	return;
    }
    *routeptr = NULL;
    while (nextroute) {
	route = nextroute;
	nextroute = route->nextadj;
	if ((route->type != PHASEIV) && (route->type != PHASEIV_PLUS)) {
	    reg_invoke_decnet_dn2clns_neighbor(NULL, dn_area(route->gateway),
					       dn_node(route->gateway), 0L);
	}
	free(route->vector);
	free(route->areavector);
	free(route);
    }
}

void dn_clear_rt (dnroutetype *route)
{
    free(route->vector);
    route->vector = NULL;
    free(route->areavector);
    route->areavector = NULL;
    dn_free_rt(&route->nextadj);
    route->numroutes = 0;
    route->nextsplit = NULL;
}

/*
 * dn_dispose
 * Discard an DECnet input packet
 */

void dn_dispose (int code, paktype *pak)
{
    if (code >= DNST_MAX) {
	errmsg(&msgsym(BADDISP, SYS), code, "dn_dispose");
    } else {
	dn_stat[code]++;
    }
    retbuffer(pak);
}

/*
 * Encodings for NSP messages MSGFLG field.
 *
 * These are the bit encodings for the MSGFLG field of a NSP message.
 * Of these known message types, all have valid source and destination
 * link ids except for connect acknowledge and nop which only have
 * a destination id.  All other bit combinations are reserved.
 * 
 * When we have a source and destination id, we hash them together to
 * use as the path selector.  This has the advantage that different
 * sessions between a single a pair of nodes may use different
 * paths.  Otherwise, we'll use the node ids as the selector.
 *
 *   bits    decimal offset
 * --------  ---
 * 76543210
 *
 * 00000000   0 - data
 * 00100000  32 - data, begin of msg
 * 01000000  64 - data, end of msg
 * 01100000  96 - data, begin & end of msg
 * 00010000  16 - link service
 * 00110000  48 - interrupt
 * 00000100   4 - ack
 * 00010100  20 - other ack
 * 00100100  36 - connect ack, dstaddr only
 * 00001000   8 - nop, dstaddr only
 * 00011000  24 - connect initiate
 * 00101000  40 - connect confirm
 * 00111000  56 - disconnect initiate
 * 01001000  72 - disconnect confirm
 * 01101000 104 - retransmitted connect initiate
 */

const uchar nsptype[] = { 
    SDID, RSVD, RSVD, RSVD, SDID, RSVD, RSVD, RSVD, /*   0 */
    DSID, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /*   8 */
    SDID, RSVD, RSVD, RSVD, SDID, RSVD, RSVD, RSVD, /*  16 */
    SDID, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /*  24 */
    SDID, RSVD, RSVD, RSVD, DSID, RSVD, RSVD, RSVD, /*  32 */
    SDID, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /*  40 */
    SDID, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /*  48 */
    SDID, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /*  56 */
    SDID, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /*  64 */
    SDID, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /*  72 */
    RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /*  80 */
    RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /*  88 */
    SDID, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /*  96 */
    SDID, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /* 104 */
    RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /* 112 */
    RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /* 120 */
    RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /* 128 */
    RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /* 136 */
    RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /* 144 */
    RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /* 152 */
    RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /* 160 */
    RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /* 168 */
    RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /* 176 */
    RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /* 184 */
    RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /* 192 */
    RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /* 200 */
    RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /* 208 */
    RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /* 216 */
    RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /* 224 */
    RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /* 232 */
    RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, /* 240 */
    RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD, RSVD  /* 248 */
    };

/*
 * dn_packet_forward
 * Forward a DECnet datagram. Calls dn_forward, if the packet's input
 * interface is not NULL. If it is NULL, it is discarded.
 */
void dn_packet_forward (paktype *pak, dndatapacket *data, int firstbyte)
{
    dnnet *net;

    if ((pak->if_input == NULL) ||
	((net = dn_nets[pak->if_input->dn_network]) == NULL)) {
	dn_dispose(DN_NOTNET, pak);
	return;
    }

    dn_forward(pak, data, firstbyte);
}

/*
 * dn_forward
 * Forward a DECnet datagram.  Must always dispose of the datagram.
 * Unlike most, this takes a pointer to the type code.  (Most of the
 * routines take a pointer to the next byte.)
 */

void dn_forward (paktype *pak, dndatapacket *data, int firstbyte)
{
    dnroutetype *thisroute, **routeptr, *next_hop_route;
    dnhdrtype *dn;
    int node, srcnode, area, next_hop_node, next_hop_area;
    dnnet *net;
    idbtype *idb;
    int n, mapped, returned = FALSE;
    boolean next_hop_adj_not_up;
    uchar route_was_static;

    dn = (dnhdrtype *)dnheadstart(pak);

    net = NULL;
    /*
     * If input interface is NULL (which is the case for a DECnet ping packet),
     * find the first non-NULL network number
     */
    if (pak->if_input)
	net = dn_nets[pak->if_input->dn_network];
    else {
	for (n = 0; n < DN_MAXNETS; n++) {
	    if (dn_nets[n]) {
		net = dn_nets[n];
		break;
	    }
	}
    }

    if (!net) {
	retbuffer(pak);
	return;
    }

    /*
     * For the moment, we don't bother to do any parsing of the message
     * See decnet.h for message format.
     */
    dn_stat[DNST_DATAS]++;
    if ((firstbyte & DN_LONG) == 0) {
	dn_dispose(DN_NOTLONG, pak);
	return;
    }

    /*
     * Come back here if we've decided to return the packet.
     * 
     * Be a little careful in here.  A packet can go through address
     * translation, be returned due to no route, and go back through the
     * address translation. Make sure there is no chance of infinite loops.
     */
 sendreturn:
    mapped = 0;
    
    firstbyte = data->typecode;
    
    node = dn_node(data->dest);
    area = dn_area(data->dest);

    /* find the route */
    
 reroute:

    /*
     * If destination is in a different area, we use "dnareas[area]"  only
     * if we are doing Level-2 and are "attached"
     */
    if (area != net->my_dn_area)  {  		
	if ((net->doareas) && (net->attached || dn_attach_override)) {
	    routeptr = &net->dnareas[area];
	} else {
	    routeptr = &net->dnroutes[0];
	}
    } else {
	routeptr = &net->dnroutes[node];
    }
    thisroute = *routeptr;
    
    if (data->visits >= 
	((firstbyte & DN_RET) ? (net->dn_max_visits << 1) : net->dn_max_visits)) {
	dn_dispose(DN_TOOMANYVISITS,pak);
	return;
    }
    
    /*
     * This is the guts of the address translation.
     * NOTE: This code also exists in dnfast.c.
     *
     * If the route is to a mapped network, we look in the map to see if this
     * packet should be mapped.  If there is no mapping, drop the packet.  We
     * must have a map for the destination address and a map from the
     * destination network back to the source address in order to complete
     * the mapping. 
     *
     * Only make 1 pass through the mapping code in case the user has
     * made mapping loops with his configuration.
     */
    if (!mapped && (thisroute != NULL) &&
	(thisroute->flags & DN_RT_NET)) { 	/* if route to another net */
	dnmap *map;
	ushort dstaddr, srcaddr;
	int rarea, rnode;
	
	dstaddr = (area << 10) + node;
	srcaddr = (dn_area(data->source) << 10) + dn_node(data->source);
	
	map = net->nethash[dnhash(dstaddr)];
	while (map) {
	    dnnet *newnet;
	    
	    if (map->fromaddr == dstaddr) { /* if destination match */
		dnmap *rmap;
		
		newnet = dn_nets[map->tonetn];
		if (newnet == NULL) {	/* if bad destination */
		    if (dn_return(DN_NOTNET, pak, data)) {
			returned = TRUE;
			goto sendreturn;
		    }
		    else return;
		}
		
		/* find reverse path */
	        rmap = newnet->reverse[dnhash(srcaddr)];
	        while (rmap) {
		    if ((rmap->toaddr == srcaddr) && 
			(rmap->tonetn == net->netn)) { /* if reverse found */
		        rnode = rmap->fromaddr & 0x3FF;
		        rarea = rmap->fromaddr >> 10;
		        dn_addr(data->source,rarea,rnode);
			map->use++;
			rmap->use++;
		        break;
		    }
		    rmap = rmap->reverse;
	        }
                if (!rmap) {		/* if no reverse map */
		    dnet_pktbuginf("\nDNET-PKT: reverse map not found");
		    if (dn_return(DN_NOMAP, pak, data)) {
			returned = TRUE;
			goto sendreturn;
		    } else {
			return;
		    }
	        }
	        node = map->toaddr & 0x3FF;
	        area = map->toaddr >> 10;
	        dn_addr(data->dest,area,node);
		net = newnet;
		mapped++;
	        break;
	    }
            map = map->next;
        }
	if (!mapped) {			/* if mapping failed */
	    if (dn_return(DN_NOMAP, pak, data)) {
		returned = TRUE;
		goto sendreturn;
	    } else {
		return;
	    }
	}
	goto reroute;
    }
    
    if (thisroute && (thisroute->flags & DN_RT_ME)) { /* if addressed to us */
	dnet_robuginf("\nDNET-RT: Received a packet addressed to us!");

	dn_stat[DNST_LOCAL]++;
	if (dn_disconnect(pak, data, net)) {
	    returned = TRUE;
	    goto sendreturn;
	}
	return;
    }
    /*
     * Handle static routes here
     */
    if (thisroute && (thisroute->type == PHASEIV_STATIC)) { 
	thisroute = decnet_handle_static_route(net, thisroute, ROUTINGDEPTH);
    }   

    route_was_static = thisroute &&
	               (thisroute->flags & DN_RT_WAS_STATIC);

    /*
     * DN_RT_ROUTER means hello for a router, which isn't legal for routing.
     * DN_RT_NET means have a map loop which should imply a user config bug.
     */

    if (thisroute) {
        next_hop_area = dn_area(thisroute->gateway);
	next_hop_node = dn_node(thisroute->gateway);

	if (next_hop_area != net->my_dn_area)  {  		
	    if ((net->doareas) && (net->attached || dn_attach_override)) {
		next_hop_route = net->dnareas[next_hop_area];
	    } else {
		next_hop_route = net->dnroutes[0];
	    }
	} else {
	    next_hop_route = net->dnroutes[next_hop_node];
	}

	next_hop_adj_not_up = (next_hop_route->flags & DN_RT_ROUTER) &&
		               (!route_was_static) && 
	                       ((next_hop_route->flags & DN_RT_STATE_MASK)
			                        != DN_RT_STATE_UP);
    }
    else {
	next_hop_adj_not_up = TRUE;
    }

    if (thisroute == NULL || 
	((thisroute->flags & (DN_RT_ROUTER|DN_RT_NET)) && !route_was_static) ||
	next_hop_adj_not_up || (thisroute->interface == NULL)) {

	/*
	 * See if Phase IV to Phase V conversion code wants it.
	 */
	if (!reg_invoke_decnet_convert(LINK_DECNET, pak,
				      (uchar *) data, FALSE)) {
	    /*
	     * Last chance - check if there is a default route configured...
	     */
	    if (decnet_default_route) {
		thisroute = decnet_handle_static_route(net, decnet_default_route,
						       ROUTINGDEPTH);

		if (decnet_pkt_debug && thisroute) {
		    buginf("\nDNET-PKT: Using default route to route packet, "
			       "source %d.%d dest %d.%d, ",
			       dn_area(data->source), dn_node(data->source),
			       dn_area(data->dest), dn_node(data->dest));
		    if (decnet_default_route->interface == NULL)
		        buginf("via %d.%d", dn_area(thisroute->gateway),
  		               dn_node(thisroute->gateway));
		    else {
		        buginf("via interface %s",
				   (decnet_default_route->interface)->namestring);
			if (decnet_default_route->snpa.length != 0)
			    buginf(", snpa %e", decnet_default_route->snpa.addr);
                    }
	        }

		if (thisroute)
		    goto nosplit;
	    }

	    if (dn_return(DN_NOROUTE, pak, data)) {
	        returned = TRUE;
	        goto sendreturn;
	    } 
        }
        return;
    }
    
    /*  Select possible multipath route */
    
    if (thisroute->nextsplit) {
	dnroutetype *rt = thisroute;
	
	if (net->dn_interim_mode) {
	    int i;
	    ushort n;
	    dnnspdispkt *nsp = (dnnspdispkt *) &data->databyte;
	    
	    /*
	     * For NSP messages with both source and destination ids
	     * we use those ids for creating the hash.
	     * Otherwise, use the node ids for the hash.
	     */
	    if (nsptype[nsp->msgflg]) {
		n = dn_node(data->dest) ^ dn_node(data->source);
	    } else {
		n = dn_id(nsp->dstaddr) ^ dn_id(nsp->srcaddr);
	    }
	    n = n % thisroute->numroutes;
	    
	    /*
	     * Find the n'th path and use it.
	     */
	    rt = thisroute;
	    for (i = 0; i < n; i++) {
		rt = rt->nextadj;
		if (!rt || (rt->flags & (DN_RT_ROUTER|DN_RT_NET))) {
		    buginf("\nDNET: broken numroutes: %d n=%d node=%d", 
			   thisroute->numroutes, n, dn_node(data->source));
		    goto nosplit;
		}
	    }
	    thisroute = rt;
	} else {
	    leveltype level = raise_interrupt_level(NETS_DISABLE);
	    
	    /*
	     * thisroute->nextsplit points to the route we want to use
	     * this time. Point nextsplit to the next indirect route in the
	     * route list.  Wrap back to the beginning of the list if we 
	     * encounter an adjacency or the end of the list.
	     */
	    rt = thisroute->nextsplit->nextadj;
	    if (!rt || (rt->flags & (DN_RT_HELLO|DN_RT_NET))) {
		rt = *routeptr;
	    }
	    thisroute = thisroute->nextsplit;
	    (*routeptr)->nextsplit = rt;
	    reset_interrupt_level(level);
	}
    }
 nosplit:
    
    srcnode = dn_node(data->source);
    
    /*
     * Always clear the local bit unless we're switching back out
     * the input interface.  We only need to set the bit for locally
     * generated packets and don't need to worry about if the destination
     * is an endnode or not, etc.
     */
    if (thisroute->interface != pak->if_input)  {
	data->typecode &= ~DN_LOCAL;
    } else if ((thisroute->flags & DN_RT_HELLO) && 
	       (thisroute->interface->hwptr->status & IDB_TR)) {
        /*
	 * If the interfaces are the same and are token-ring and the 
	 * destination is an adjacency, we have to check to see if 
	 * the source is an adjacency whose iv_prime status is opposite
	 * the destination. In that case we clear the local
	 * bit so that the 2 nodes can continue to communicate
	 * thru the router.
	 */
	int snode, sarea;
	dnnet *snet;
	dnroutetype **srouteptr, *sthisroute;

	/* find the source route */
	snet = (pak->if_input) ? dn_nets[pak->if_input->dn_network]:dn_nets[0];
	snode = dn_node(data->source);
	sarea = dn_area(data->source);
	if (sarea != snet->my_dn_area) { /* if for a different area */
	    if ((snet->doareas) && (snet->attached || dn_attach_override))
		srouteptr = &snet->dnareas[sarea];
	    else
		srouteptr = &snet->dnroutes[0];
	} else
	    srouteptr = &snet->dnroutes[snode];
	sthisroute = *srouteptr;
	
	if (sthisroute && (sthisroute->flags & DN_RT_HELLO) &&
	    ((thisroute->flags ^ sthisroute->flags) & DN_RT_PRIME))
	    data->typecode &= ~DN_LOCAL;
    }
    
    data->visits++;
    
    if (thisroute->interface->dn_accesslist) {
	dnnspconpkt *nsp = (dnnspconpkt *) &data->databyte;
	
	if (!decnet_accesscheck(nsp, dn_area(data->source), srcnode, 
				area, node, 
				thisroute->interface->dn_accesslist,
				thisroute->interface)) {
	    if (dn_return(DN_NOACCESS, pak, data)) {
		returned = TRUE;
		goto sendreturn;
	    } else  {
		return;
	    }
	}
    }
    
    /*
     * Note that we are calling the encapsulation routine with the DECnet
     * node address in byte-swapped Ethernet format.
     */
    pak->if_output = thisroute->interface;

    /*
     * If we are converting then hand off to CLNS
     */
    if (thisroute->type == PHASEV) {
	if (!reg_invoke_decnet_convert(LINK_DECNET, pak,
				       (uchar *)data, FALSE)) {
	    retbuffer(pak);
	}
	return;
    }
    
    /*
     * Set the congestion encountered bit if there are any packets in
     * the output queue for this interface.  Here at process level, we
     * check both the system queue and the board level queues.
     *
     * Also, promote forward congestion bits from lower level frame-relay
     * encapsulation. 
     */
    idb = thisroute->interface;
    n = (idb->hwptr->txqlength)? (*idb->hwptr->txqlength)(idb->hwptr) : 0;
    if ((pak->fr_flags & PAK_FR_FECN) || 
	((idb->hwptr->output_qcount + n) > idb->hwptr->dn_congest_limit)) {
	data->next_level_2 |= DN_RTFL2_CON;
	dn_stat[DNST_CONGEST]++;	/* DEBUG ONLY. ??? REMOVE LATER. */
    }
    
    pak->linktype = LINK_DECNET;
    pak->acct_proto = ACCT_PROTO_DECNET;
    pak->dialer_flags |= PAK_DDR_NO_HOLDQ;
    if (pak_encap(pak->if_output->hwptr, pak,
		  get16(dn->paksize) + DN_PAKSIZE_FIELD_LEN,
		  (long)thisroute)) {
	if (!returned) {
	    dn_stat[DNST_FORWARD]++;
	}
	datagram_out(pak);  
	/*
	 * If packet is being returned, print a more appropriate message.
	 */
	if (data->typecode & DN_RET) {
	    dnet_pktbuginf("\nDNET-PKT: Returning packet to %d.%d "
			   "(original destination was %d.%d)", 
			   dn_area(data->dest), dn_node(data->dest),
			   dn_area(data->source), dn_node(data->source));
        }
	else {
	    dnet_pktbuginf("\nDNET-PKT: Packet fwded from %d.%d to %d.%d, ",
   		       dn_area(data->source), dn_node(data->source),
   		       dn_area(data->dest), dn_node(data->dest));
	}
	if (!thisroute->is_int_static_route) {
	    dnet_pktbuginf("via %d.%d, snpa %e, %s", dn_area(thisroute->gateway),
			   dn_node(thisroute->gateway), thisroute->snpa.addr,
			   (thisroute->interface)->namestring);
	}
	else {
	    dnet_pktbuginf("via %s", (thisroute->interface)->namestring);
	}
    } else {
	uchar is_token_ring;
	/*
	 * Check if the failure occurred on a DDR interface. If it did,
	 * we check if the packet caused dialing to occur. If dialing
	 * did occur, we drop this packet instead of returning it. This
	 * way, a host initiating a connection will not quit after 
	 * seeing a returned packet, and will wait instead. The idea
	 * is that it will retransmit packets, and one of these	 packets
	 * will make it after the circuit is opened.
	 */
	if (pak->dialer_flags & PAK_DDR_CALL_PENDING) {
	    dnet_pabuginf("\nDNET: Packet discarded (DDR call pending");
	    retbuffer(pak);
	    return;
	}
	/*
	 * Do not return packets that fail encapsulation if they were
	 * on token ring since the failure may have been due to
	 * needing to do a source route exploration (rare). Otherwise,
	 * the connection would fail immediately which is not what
	 * we want.
	 */
	is_token_ring = (pak->if_output->hwptr->status & IDB_TR);
	if (!is_token_ring) {
	    if (dn_return(DN_NOENCAP, pak, data)) {
	        returned = TRUE;
	        goto sendreturn;
	    }
	    else {
		dnet_pabuginf("\nDNET: Encapsulation failed (1) for "
		 	      "interface %s", pak->if_output->namestring);
	        return;
	    }
	} else {
	    dnet_pabuginf("\nDNET: Encapsulation failed (1) for "
		          "interface %s", pak->if_output->namestring);
	    dn_dispose(DN_NOENCAP, pak);
	    return;
	}
    }
}

/*
 * dn_return - check to see if we should return a packet
 *
 * If DN_RETREQ is set in the typecode then:
 * Clear DN_RETREQ, Set DN_RET, Flip the source and destination addresses.
 * Return TRUE to indicate we should reroute the packet.
 *
 * Else dispose of the packet and return a FALSE.
 */
boolean dn_return (uint code, paktype *pak, dndatapacket *data)
{
    uchar t1, t2;
    
    if (data->typecode & DN_RETREQ) {	/* if we're supposed to return */
	t1 = data->dest[0];		/* flip addresses */
	t2 = data->dest[1];
	data->dest[0] = data->source[0];
	data->dest[1] = data->source[1];
	data->source[0] = t1;
	data->source[1] = t2;
	data->typecode &= ~(DN_RETREQ|DN_LOCAL); /* fix the flags */
	data->typecode |= DN_RET;
	dn_stat[code]++;		/* bump appropriate counters */
	dn_stat[DN_RETURNED]++;
	return(TRUE);
    } else {
	dn_dispose(code, pak);
	return(FALSE);
    }
}


/*
 * decnet_accesscheck
 * Perform an access check on a routed, outgoing DECnet datagram.
 * Returns TRUE to send datagram, FALSE otherwise. A mirror image
 * of this code exists in *mci_dn_accesscheck*, if this function
 * is changed, kindly make the corresponding changes for fast switching.
 * Note, the fast switching routines do not check connect initiate.
 * Those packets are always sent here to process level.
 */

boolean decnet_accesscheck (
    dnnspconpkt *nsp,
    int srcarea,
    int srcnode,
    int destarea,
    int destnode,
    int listnum,
    idbtype *idb)
{
    int srcid, dstid;
    int n;
    int dstlen=0, srclen=0, idlen=0, pwlen=0, aclen=0;
    ulong dgrp=-1, dusr=-1, sgrp=-1, susr=-1;
    ulong dsttype=-1, srctype=-1;
    uchar *dstname=NULL, *srcname=NULL, *id=NULL, *pw=NULL, *ac=NULL;
    uchar *data, format;
    dnacctype *list;
    dnaccitemtype *item, *temp_item;
    boolean source_dest_match;

    srcid = srcnode | srcarea << 10;
    dstid = destnode | destarea << 10;
    list = dn_get_acl(listnum);
    if (!list)
	return(TRUE);

    /*
     * All packets must pass the normal part of the list first.
     */
    for (item = list->normal, n = 1; item; n++, item = item->next) {
	if ((~item->sourcemask & srcid) != item->source) {
	    continue;
	}
	if (item->second) {
	    if ((~item->destmask & dstid) != item->dest) {
		continue;
	    }
	}
	break;
    }

    if (item) {
	/* 
	 * If match found in the normal part of the access list, and it was
	 * a 'permit' return TRUE. However, if the packet is a connect
	 * initiate packet, send it through the connect initiate list as well,
	 * to see if there is a matching entry. If there is, process the 
	 * packet as per the connect initiate entry. If there is no matching
	 * entry, then process it as per the original normal ACE. This will
	 * take care of the case where a normal 'permit' gets processed before
	 * a more specific 'deny' in the connect initiate list.
	 */
	if (item->grant) {
            if ((nsp->msgflg != NSP_CONNECT) && (nsp->msgflg != NSP_RECONNECT))
	        return (TRUE);
            for (temp_item = list->connect; temp_item; 
		                            temp_item = temp_item->next) {
                if (dn_source_dest_match(temp_item, srcid, dstid))
		    break;
	    }
	    /* 
	     * If no matching item found, process like normal list entry
	     */
	    if (!temp_item)
		return (TRUE);
	}
	/* 
	 * If it was a 'deny', return "no access" if no corresponding entry
	 * was found in the extended access list. If an entry was found in
	 * the extended access list, process it as per the ACE in the
	 * extended access list.
	 */
	else {
            if ((nsp->msgflg != NSP_CONNECT)&&(nsp->msgflg != NSP_RECONNECT)){
                if (decnet_conn_debug) 
	            dn_debug_cpacket(TRUE, listnum, n, srcarea, srcnode, 
				     destarea, destnode, idb, -1, NULL, 0,
				     -1, 0, -1,NULL, 0, -1, 0, NULL, 0, FALSE);
	        return(FALSE);
	    }
            for (temp_item = list->connect; temp_item; 
		                            temp_item = temp_item->next) {
                if (dn_source_dest_match(temp_item, srcid, dstid))
		    break;
	    }
	    /* 
	     * If no matching item found, process like normal list entry
	     */
	    if (!temp_item)
		return (FALSE);
        }
    }

    /* 
     * If there was no matching entry in the normal part of the list and
     * there are no connect entries, return "no access". This way, we will
     * handle the case when ONLY extended lists are present.
     */
    if (!item && !list->connect) {
        if (decnet_conn_debug) {
	    dn_debug_cpacket(FALSE, listnum, n, srcarea, srcnode, destarea, 
			     destnode, idb, -1, NULL, 0, -1, 0,
			     -1, NULL, 0, -1, 0, NULL, 0, FALSE);
	}
	return(FALSE);
    }	

    /*
     * Connect-initiate or retransmitted connect-initiate packets
     * must pass the other part of the list if it is present.
     */
    if (((nsp->msgflg != NSP_CONNECT) && (nsp->msgflg != NSP_RECONNECT)) ||
	!list->connect) {
	return(TRUE);
    }


    /*
     * Count the number of items in the first part of the list.
     */
    for (item = list->normal, n=1; item; n++, item = item->next) {
	; /* Null Body */
    }

    /*
     * Parse the connect data in the packet.
     */
    data = &nsp->data;
    /* DSTNAME */
    format = *data++;
    switch ((int) format) {
    case 0:				/* format 0 */
	dsttype = *data++;
	break;
    case 1:				/* format 1 */
	if (*data != 0) {		/* if bad format */
	    dnet_cobuginf("\nDNET-CON: bad dst connect format=1 data=0x%x", 
			  *data);
	    goto check;
	}
	data++;
	dstlen = *data++;
	dstname = data;
	data += dstlen;
	break;
    case 2:				/* format 2 */
	if (*data != 0) {		/* if bad format */
	    dnet_cobuginf("\nDNET-CON: bad dst connect format=2 data=0x%x", 
			  *data);
	    goto check;
	}
	data++;
	/* do not use dgrp = *data++<<8 + *data++ due to a GCC bug */
	dgrp = *data++;
	dgrp += *data++ << 8;
	dusr = *data++;
	dusr += *data++ << 8;
	dstlen = *data++;
	dstname = data;
	data += dstlen;
	break;
    default:
	dnet_cobuginf("\nDNET-CON: unknown dst connect format: 0x%x", format);
	goto check;
    }

    /* SRCNAME */
    format = *data++;
    switch ((int) format) {
    case 0:				/* format 0 */
	srctype = *data++;
	break;
    case 1:				/* format 1 */
	if (*data != 0) {		/* if bad format */
	    dnet_cobuginf("\nDNET-CON: bad dst connect format=1 data=0x%x", 
			  *data);
	    goto check;
	}
	data++;
	srclen = *data++;
	srcname = data;
	data += srclen;
	break;
    case 2:				/* format 2 */
	if (*data != 0) {		/* if bad format */
	    dnet_cobuginf("\nDNET-CON: bad dst connect format=2 data=0x%x", 
			  *data);
	    goto check;
	}
	data++;
	sgrp = *data++;
	sgrp += *data++ << 8;
	susr = *data++;
	susr += *data++ << 8;
	srclen = *data++;
	srcname = data;
	data += srclen;
	break;
    default:
	dnet_cobuginf("\nDNET-CON: unknown src connect format: 0x%x", format);
	goto check;
    }

    /* MENUVER */
    if (*data++ & 1) {		/* if RQSTRID, PASSWRD, ACCOUNT included */
	/* RQSTRID */
	idlen = *data++;
	id = data;
	data += idlen;

	/* PASSWRD */
	pwlen = *data++;
	pw = data;
	data += pwlen;

	/* ACCOUNT */
	aclen = *data++;
	ac = data;
	data += aclen;
    }

    /*
     * Check the connect part of the list. Continue to count
     * items.
     */
 check:
    for (item = list->connect; item; n++, item = item->next) {
        source_dest_match = dn_source_dest_match(item, srcid, dstid);

	/*
	 * Verify all fields specified in the item.  Since regexec()
	 * compares against zero terminated strings, we must
	 * copy the string first.
	 */
	if (item->flags & DNO_ANY) {
	    /* 
	     * source (and destination, if present) in ACE must match source 
	     * and destination info in packet.
	     */
	    if (source_dest_match) goto matched;
	    else goto nomatch;

	}
	if (item->flags & DNO_SON) {
	    switch (item->srcop) {
	    case DNNAM_EQ:
		if (item->son != srctype) goto nomatch;
		else if (!source_dest_match) goto nomatch;
		break;
	    case DNNAM_NEQ:
		if (item->son == srctype) goto nomatch;
		else if (!source_dest_match) goto nomatch;
		break;
	    case DNNAM_LT:
		if (item->son >= srctype) goto nomatch;
		else if (!source_dest_match) goto nomatch;
		break;
	    case DNNAM_GT:
		if (item->son <= srctype) goto nomatch;
		else if (!source_dest_match) goto nomatch;
		break;
	    }
	}
	if (item->flags & DNO_DON) {
	    switch (item->dstop) {
	    case DNNAM_EQ:
		if (item->don != dsttype) goto nomatch;
		else if (!source_dest_match) goto nomatch;
		break;
	    case DNNAM_NEQ:
		if (item->don == dsttype) goto nomatch;
		else if (!source_dest_match) goto nomatch;
		break;
	    case DNNAM_LT:
		if (item->don >= dsttype) goto nomatch;
		else if (!source_dest_match) goto nomatch;
		break;
	    case DNNAM_GT:
		if (item->don <= dsttype) goto nomatch;
		else if (!source_dest_match) goto nomatch;
		break;
	    }
	}
	if (item->flags & DNO_SOE) {
	    if (srclen == 0)		/* ????? SHOULD NULL FIELD MATCH ? */
		goto nomatch;
	    if (!regexec(item->sopattern, cnttostr(srcname, srclen)))
		goto nomatch;
	    else if (!source_dest_match) goto nomatch;
	}
	if (item->flags & DNO_DOE) {
	    if (dstlen == 0)
		goto nomatch;
	    if (!regexec(item->dopattern, cnttostr(dstname, dstlen)))
		goto nomatch;
	    else if (!source_dest_match) goto nomatch;
	}
	if (item->flags & DNO_ID) {
	    if (idlen == 0)
		goto nomatch;
	    if (!regexec(item->idpattern, cnttostr(id, idlen)))
		goto nomatch;
	    else if (!source_dest_match) goto nomatch;
	}
	if (item->flags & DNO_PW) {
	    if (pwlen == 0)
		goto nomatch;
	    if (!regexec(item->pwpattern, cnttostr(pw, pwlen)))
		goto nomatch;
	    else if (!source_dest_match) goto nomatch;
	}
	if (item->flags & DNO_AC) {
	    if (aclen == 0)
		goto nomatch;
	    if (!regexec(item->acpattern, cnttostr(ac, aclen)))
		goto nomatch;
	    else if (!source_dest_match) goto nomatch;
	}
	if (item->flags & DNO_SUIC) {
	    if ((item->susr != susr) || (item->sgrp != sgrp))
		goto nomatch;
	    else if (!source_dest_match) goto nomatch;
	}
	if (item->flags & DNO_DUIC) {
	    if ((item->dusr != dusr) || (item->dgrp != dgrp))
		goto nomatch;
	    else if (!source_dest_match) goto nomatch;
	}

	/*
	 * Packet matched this item's qualifiers, if NEQ,  we don't 
	 * match this item.
	 */
    matched:
	if (!(item->flags&DNO_EQ)) {
	    continue;
	}

    accept:
	/*
	 * Note, cnttostr() returns a pointer to a static buffer
	 * and so may not be used twice in the same printf/buginf.
	 */
	if (decnet_conn_debug) {
	    dn_debug_cpacket(TRUE, listnum, n, srcarea, srcnode, destarea, 
			     destnode, idb, srctype, srcname, srclen, sgrp, 
			     susr, dsttype, dstname, dstlen, dgrp, dusr, id, 
			     idlen, item->grant);
	}
	
	return(item->grant);

	/*
	 * Packet didn't match this item's qualifiers.  If NEQ, 
	 * we match this item, provided source and destination parts match.
	 */
    nomatch:
	if (!(item->flags & DNO_EQ) && source_dest_match) {
	    goto accept;
	}
    }

    if (decnet_conn_debug && 
	((nsp->msgflg == NSP_CONNECT) || (nsp->msgflg == NSP_RECONNECT))) {
	dn_debug_cpacket(FALSE, listnum, n, srcarea, srcnode, destarea, 
			  destnode, idb, srctype, srcname, srclen, sgrp, susr,
			  dsttype, dstname, dstlen, dgrp, dusr, id, idlen,
			 FALSE);
    }

    return(FALSE);
}

/*
 * dn_source_dest_match
 *
 * Return TRUE if source field in ACL matches source info in packet, and
 * destination field in ACL matches destination info in packet (if 
 * destination field in ACL exists
 */
boolean dn_source_dest_match (dnaccitemtype *item, int srcid, int dstid)
{
    if ((~item->sourcemask & srcid) == item->source) {
        if (item->second) {
	    if ((~item->destmask & dstid) == item->dest) {
	        return(TRUE); 		 
	    } else {
		return(FALSE);
	    }
	} else {
	    return(TRUE);
	}
    } else {
        return(FALSE);
    }
}

/*
 * dn_access
 * Access list check.
 */
static boolean dn_access (int srcarea,
	   int srcnode,
	   int destarea,
	   int destnode,
	   int listnum)

{
    dnaccitemtype *item;
    dnacctype *list;
    int dstid, srcid;

    list = dn_get_acl(listnum);
    if (!list)
	return(TRUE);

    srcid = dn_getid(srcnode, srcarea);
    dstid = dn_getid(destnode, destarea);

    for (item = list->normal; item; item = item->next) {
	if ((~item->sourcemask & srcid) != item->source) {
	    continue;
	}
	if (item->second) {
	    if ((~item->destmask & dstid) != item->dest) {
		continue;
	    }
	}
	break;
    }

    if (item)
	return (item->grant);
    else
	return (FALSE);
}

/*
 * dn_data_accesscheck
 * Registered decnet access check routine for data packets only.
 */
boolean decnet_data_accesscheck (paktype *pak, int listnum)
{
    dnhdrtype *dn;
    dndatapacket *dndata;
    uchar *data;
    int firstbyte;
    int srcarea, srcnode, destarea, destnode;

     dn = (dnhdrtype *)dnheadstart(pak);
     data = dn->pakstart;
	    
     /* skip padding bytes */
     firstbyte = *data;
     if (firstbyte & DN_PAD) {
        int padcount;
	 
        padcount = firstbyte & 0x7f;
	data += padcount;
     }
	    
    dndata = (dndatapacket *)data;

    srcnode = dn_node(dndata->source);
    srcarea = dn_area(dndata->source);

    destnode = dn_node(dndata->dest);
    destarea = dn_area(dndata->dest);

    dnet_pktbuginf("\nDNET-PKT: Performing access check for data packet with "
		   "src %d.%d, dst %d.%d",
                   srcarea, srcnode, destarea, destnode);

    return(dn_access(srcarea, srcnode, destarea, destnode, listnum));
}

/*
 * dn_debug_cpacket
 * Print debug info about connect packet.
 */
void dn_debug_cpacket (
    boolean match,
    int listnum,
    int n,
    int srcarea,
    int srcnode,
    int destarea,
    int destnode,
    idbtype *idb,
    ulong srctype,
    uchar *srcname,
    int srclen,
    ulong sgrp,
    ulong susr,
    ulong dsttype,
    uchar *dstname,
    int	dstlen,
    ulong dgrp,
    ulong dusr,
    uchar *id,
    int idlen,
    boolean grant)
{
    char *sp, scratch[256];
    static char *leader = "\nDNET-CON: ";

    if (match) {
	buginf("\nDNET-CON: list %d item #%d matched src=%d.%d dst=%d.%d"
	       " on %s: %s",
	       listnum, n, srcarea, srcnode, destarea, destnode,
	       idb->namestring, (grant)? "permitted" : "denied");
    } else {
	buginf("\nDNET-CON: list %d no items match pkt src=%d.%d dst=%d.%d"
	       " on %s: denied",
	       listnum, srcarea, srcnode, destarea, destnode, idb->namestring);
    }
    strcpy(scratch, leader);
    sp = scratch + strlen(scratch);
    if (srctype != -1) {
	sp+=sprintf(sp, " srcobj=%d", srctype);
    }
    if (srclen) {
	sp+=sprintf(sp, " srcname=\"%s\"", cnttostr(srcname, srclen));
    }
    if (sgrp != -1) {
	sp+=sprintf(sp, " srcuic=[%#o,%#o]", sgrp, susr);
    }
    if (sp > (scratch + 7)) {
	buginf(scratch);
    }
    strcpy(scratch, leader);
    sp = scratch + strlen(scratch);
    if (dsttype != -1) {
	sp+=sprintf(sp, " dstobj=%d", dsttype);
    }
    if (dstlen) {
	sp+=sprintf(sp, " dstname=\"%s\"", cnttostr(dstname, dstlen));
    }
    if (dgrp != -1) {
	sp+=sprintf(sp, " dstuic=[%#o,%#o]", dgrp, dusr);
    }
    if (idlen) {
	sp+=sprintf(sp, " id=\"%s\"", cnttostr(id, idlen));
    }
    if (sp > (scratch + 7)) {
	buginf(scratch);
    }
}



/*
 * cnttostr
 * Convert a counted string of bytes into a zero terminated string.
 * WARNING: returns a pointer to a static buffer.
 */
char *cnttostr (uchar *bytes, int len)
{
    static char buf[256];

    if (len) {
	bcopy(bytes, buf, len);
    }
    buf[len] = 0;

    return(buf);
}

/*
 * decnet_fastcheck
 */

boolean decnet_fastcheck (int area, int node, int listnum)
{
    dnacctype *list;
    dnaccitemtype *item;

    list = dn_get_acl(listnum);
    if (!list)
	return(TRUE);
    item = list->normal;
    if (!item)
	return(TRUE);

    node |= area << 10;
    while (item) {
	if ((~item->sourcemask & node) == item->source) {
	    return(item->grant);
	}
	item = item->next;
    }
    return(FALSE);
}

/*
 * dn_control
 * Process a DECnet control message
 */

void dn_control (paktype *pak, uchar *data, int size, int firstbyte)
{
    dnnet *net;
    int flags = 0;

    if ((pak->if_input == NULL) ||
       ((net = dn_nets[pak->if_input->dn_network]) == NULL)) {
	 if (decnet_debug) {
	     if (pak->if_input == NULL) {
	        buginf("\nDNET: Packet's input interface is NULL");
             } else {
 	        buginf("\nDNET: Input interface net is NULL");
	    }
         }
	 dn_dispose(DN_NOTNET, pak);
	 return;
    }

    switch (firstbyte & DN_CTL_TYPE_MASK) {
    case DN_CTL_TYPE_RT_L2:		/* level 2 routing */
  	if (net->doareas)
  	    dn_rt_input(pak, data, size, 0);
  	else
  	    dn_dispose(DNST_LVL2S,pak);	
  	break;
 
    case DN_CTL_TYPE_RT_L1:		/* level 1 routing */
  	dn_rt_input(pak, data, size, 1);
  	break;
 
    case DN_CTL_TYPE_HELLO_RTR:		/* router hello */
 	if (firstbyte & DN_FP)		/* if phase-iv prime router */
 	    flags |= DN_RT_PRIME;
 	dn_hello_input(pak, flags, (dnhellotype *)data);
 	break;
 
    case DN_CTL_TYPE_HELLO_END:		/* phase iv endnode hello */
 	dn_hello_input(pak, flags, (dnhellotype *)data);
 	break;
 
    case DN_CTL_TYPE_EXT:		/* extended type */
 	switch (firstbyte & DN_CTL_EXTTYPE_MASK) {
 	    case DN_CTL_EXTTYPE_PFPEH:	/* phase iv prime endnode hello */
 	        flags |= DN_RT_PRIME;
 	        dn_hello_input(pak, flags, (dnhellotype *)data);
 	        break;
	    default:
		dnet_pabuginf("\nDNET: unrecognized ctl pkt, extended type "
			      "%02x", firstbyte & DN_CTL_EXTTYPE_MASK);
		dn_dispose(DN_NOTIMP,pak);
		break;
        }
	break;
    default:
        dnet_pabuginf("\nDNET: unrecognized ctl pkt, type %02x", firstbyte);
        dn_dispose(DN_NOTIMP,pak);
        break;
    }
}

/*
 * dn_hello_input
 * Process hello from router or end node.  For the moment, just
 * keep track of adjacencies
 */

void dn_hello_input (paktype *pak, int flags, dnhellotype *hello)
{
    int hisnode, hisarea, why;
    int hello_time, cost, nodetype, blksize, protocol_type, max_blksize;
    dnnet *net;
    
    if (pak->if_input == NULL) {
	dnet_pabuginf("\nDNET: hello with null input interface");
	dn_dispose(DN_BADHELLO,pak);
	return;
    }
    if ((net = dn_nets[pak->if_input->dn_network]) == NULL) {
	dn_dispose(DN_NOTNET, pak);
	return;
    }

    /*
     * Do not process hello if DECnet is not enabled for this interface
     */
    if (pak->if_input->dn_cost == -1) {
	dn_dispose(DNST_RECEIVED, pak);
	return;
    }

    dn_stat[DNST_HELLOS]++;
    hisnode = dn_node(hello->sender);
    hisarea = dn_area(hello->sender);
    if (hello->version != DN_HELLO_VERSION || hisnode > DN_MAXNODES) {
	dnet_pabuginf("\nDNET: bad hello ver %d", hello->version);
	dn_dispose(DN_BADHELLO,pak);
	return;
    }

    if (pak->if_input->dn_in_filter &&
	!decnet_fastcheck(hisarea, hisnode, pak->if_input->dn_in_filter)) {
	dnet_adjbuginf("\nDNET-ADJ: hello from %d.%d, failed access control",
		       hisarea, hisnode);
	dn_dispose(DN_NOACCESS,pak);
	return;
    }

    /* 
     * Do not accept a hello if area > dn_max_area or (node > dn_max_node and
     * the hello is for our area)
     */
    if (hisarea > net->dn_max_area) {
	dnet_pabuginf("\nDNET: Hello from area %d rejected - "
		      "exceeded 'max area' parameter (%d)", hisarea,
		      net->dn_max_area);
	dn_dispose(DN_BADHELLO,pak);
	return;
    }
    else if ((hisarea == net->my_dn_area) &&
	     (hisnode > net->dn_max_node)) {
	dnet_pabuginf("\nDNET: Hello from node %d rejected - "
	              "exceeded 'max node' parameter (%d)", 
		      hisnode, net->dn_max_node);
	dn_dispose(DN_BADHELLO,pak);
	return;
    }
     
    /*
     * Now validate, since we want to make sure the packet is OK before
     * creating any routing entry.
     */
    nodetype = hello->nodetype & DN_NT_MASK;
    switch (nodetype) {
      case DN_NT_LEVEL2: 
      case DN_NT_LEVEL1:
	flags |= DN_RT_ROUTER | DN_RT_HELLO;
	break;
      case DN_NT_END:
	flags |= DN_RT_HELLO;
	break;
      default:
	dnet_pabuginf("\nDNET: bad hello type %x from %d.%d",
		      nodetype, net->my_dn_area, hisnode);
	dn_dispose(DN_BADHELLO,pak);
	return;
    }

    /*
     * The spec says 3 * hello interval.  I prefer to use +1 to allow for
     * slight response differences.
     * For reasons known only to DEC, end nodes and routers use
     * a different format for the hello message.
     */
    if (flags & DN_RT_ROUTER)
	hello_time = ((3 * get16(hello->hello_int)) + 1) * ONESEC;
    else
	hello_time = ((3 * get16(hello->end_hello_int)) + 1) * ONESEC;
    cost = pak->if_input->dn_cost;

    /*
     * Keep track of the smallest blocksize seen on each interface.
     * We only care if it is from a router.  We use this to limit
     * the size of routing updates.  We don't keep track of
     * level 1 and 2 separately as 64 areas will always fit
     * in the minimum sized packet.
     */
    blksize = get16(hello->blocksize);

    if ((flags & DN_RT_ROUTER) && (blksize >= DN_MINPACKET))  {
        if (blksize != pak->if_input->dn_hismtu) {
	    /* 
	     * If his block size has changed, update our max block size.
	     * However, ensure that we will never exceed DN_MAXPACKET.
	     */
	    max_blksize = (pak->if_input->hwptr->maxmtu < DN_MAXPACKET ? 
			   pak->if_input->hwptr->maxmtu : DN_MAXPACKET);
	    pak->if_input->dn_maxblksize = min(blksize, max_blksize);
 	    pak->if_input->dn_hismtu = blksize;
	}
    }

    if (hisarea != net->my_dn_area) {
	if (nodetype != DN_NT_LEVEL2 || !net->doareas) {
	    dn_dispose(DN_OTHER_AREA_HELLO,pak);
	    return;
	}
    }

    /*
     * extract the code that actually inserts/updates the adjacency
     * into a subroutine so that is can be called by clns in conversion mode
     */

    protocol_type = (hello->dececo == DN_PHASEIV_PLUS_VERSION) ? PHASEIV_PLUS :
                                                                 PHASEIV;
    why = dn_add_adjacency(hello->sender, dn_idn(hello->sender), hisarea,
                             hisnode, net, nodetype, flags, hello_time, cost,
                             protocol_type, pak, hello, hello->priority,
                             pak->if_input);

    if (why != DN_SUCCESS) {
        dn_dispose(why, pak);
	return;    
    }
    retbuffer(pak);
}

/*
 * dn_add_adjacency
 *
 */
int dn_add_adjacency (
    uchar *sender,
    ushort addr,
    int hisarea,
    int hisnode,
    dnnet *net,
    int nodetype,
    int flags,
    int hello_time,
    int cost,
    int protocol_type,
    paktype *pak,
    dnhellotype *hello,
    int priority,
    idbtype *idb)
{
    int i, oldcost, index, oursfound, twoway, state, oldstate, nadjacencies;
    uchar *cp;
    ushort dnid = hisarea * 1024 + hisnode;
    dnroutetype *thisroute,**routeptr;
    dnroutetype *route, **table;
    dnroutetype *lowest;
    static char *const nodestring[]=
                       {"Unknown", "Level 2", "Level 1", "Endnode"};
    hwidbtype *hwidb = hwidb_or_null(idb);
    boolean route_was_static = FALSE;

    if (hisarea == net->my_dn_area) {
      table = net->dnroutes;
      index = hisnode;
    } else {
      table = net->dnareas;
      index = hisarea;
    }
    thisroute = table[index];

    if (thisroute && (thisroute->flags & (DN_RT_ROUTER|DN_RT_ME)) == 0) {
	oldcost = thisroute->cost;
    } else {
	oldcost = -1;
    }

    /*
     * Check for hearing hellos from ourself.
     * If we are in loopback or are a serial interface that has detected
     * external loopback, just ignore the hello, otherwise we also
     * print an error message.
     */
    if (thisroute && (thisroute->flags & DN_RT_ME)) {
	if (hwidb->loopback || ((hwidb->status & IDB_SERIAL) && 
				hwidb->inloopback)) {
	    return(DN_BADHELLO);
	}
	errmsg(&msgsym(HEARSELF, DNET),
	       nodetype, hisarea, hisnode, idb->namestring);
	return(DN_BADHELLO);
    }
    /*
     * If native route already present, do not overwrite if called from
     * clns.
     */
    if (protocol_type == PHASEV && thisroute &&
       ((thisroute->type == PHASEIV) || (thisroute->type == PHASEIV_PLUS))) {
	dnet_adjbuginf("\nDNET-ADJ: Native adj (type %s) for %d.%d exists - "
		       "not installing PhaseV route",
		       (thisroute->type == PHASEIV) ? "PhaseIV" : "PhaseIV+",
		       hisarea, hisnode);
        return(DN_SUCCESS);
    }

    /*
     * Look for the adjacency and remove it.  We remove it because if the
     * cost has changed it may go back in somewhere else.  Routeptr points
     * to the previous element so we can do the deletion.
     */
    routeptr = &table[index];
    while (thisroute) {
	if ((thisroute->flags & DN_RT_HELLO) &&
	    thisroute->interface == ((thisroute->type == PHASEV && 
				      idb->clns_cluster_alias) ? NULL : idb) &&
	    thisroute->gateway[0] == sender[0] &&
	    thisroute->gateway[1] == sender[1]) {
	    *routeptr = thisroute->nextadj;
	    break;
	}
	routeptr = &thisroute->nextadj;
	thisroute = thisroute->nextadj;
    }
    /*
     * If the nodetype changed, there will be invalid routing
     * vectors.  Remove them and any routes that go through 
     * here.
     */
    if (thisroute && (nodetype != DN_NT_LEVEL2) && thisroute->areavector) {
	dnet_pabuginf("\nDNET: %d.%d is no longer an area router, "
		      "removing routes", hisarea, hisnode);
	free(thisroute->areavector);
	thisroute->areavector = NULL;

        if (thisroute->type != PHASEIV_PLUS) {
            DECREMENT_OLD_PHASEIV_ADJ_COUNT(thisroute);
        }
        else {
            /*
             * If this was a PhaseIV+ adjacency, decrement the IV+ adj counter,
             * increment the 'old' PhaseIV counter, and change the type to a
             * PhaseIV adjacency.
             */
            DECREMENT_PHASEIV_PLUS_ADJ_COUNT(thisroute);
              ++((thisroute->interface)->dn_old_phaseIV_adj_count);
            thisroute->type = PHASEIV;
        }
	dn_remove_via(net, thisroute, net->dnareas, net->dnroutes-1);
    }
    if (thisroute && (nodetype == DN_NT_END) && thisroute->vector) {
	dnet_pabuginf("\nDNET: %d.%d is no longer a router, removing routes",
		      hisarea, hisnode);
	free(thisroute->vector);
	thisroute->vector = NULL;

        DECREMENT_L1_ADJ_COUNT(thisroute);
 
        if (thisroute->type != PHASEIV_PLUS) {
            DECREMENT_OLD_PHASEIV_ADJ_COUNT(thisroute);
        }
        else {
            DECREMENT_PHASEIV_PLUS_ADJ_COUNT(thisroute);
        }
 
	dn_remove_via(net, thisroute, net->dnroutes, net->dn_maxroute);
    }

    /*
     * For router adjacencies, process the adjacency state.  When 
     * called from CLNS for endnodes, pak will be NULL.
     *
     * I include my state diagram to help decipher the code.  It 
     * may or may not help.
     *
     *         new  ->  None   Rej    Down   Init   Up
     *         state
     * old
     * state
     *
     * None             -      msg    -      msg    -
     *
     * Rej              -      x      -      msg    -
     *
     * Down             -      b      x      msg    -
     *
     * Init             -      b      -      x      msg
     *
     * Up               -      b      msg    -      x
     *
     * symbol action
     * ------ ------
     * msg    Issue a state change message
     * x      no-op
     * -      Should never happen
     * b      Only happens if bumped by another adjacency
     *
     */
    state = 0;
    if ((pak != NULL) && (flags & DN_RT_ROUTER)) {
	/*
	 * Look through his list of adjacencies to see if he has
	 * seen us and if he thinks the connection is two-way.
	 */
	twoway = oursfound = FALSE;
	cp = &hello->adjacencies[4];	/* offset to id */
	for (i = 0; i < hello->count2; i += 7, cp += 7) {
	    if (dn_idn(cp) == net->my_dn_address) {
		oursfound = TRUE;
		if (cp[2] & DN_ADJ_UP) {
		    twoway = TRUE;
		}
		break;
	    }
	}

	/*
	 * If the adjacency already exists and he has seen us
	 * the state is up.  If our state was up but we are not in
	 * his adjacency list, the adjacency goes down.  Otherwise
	 * the state remains initializing.
	 */
	if (thisroute != NULL) {		/* if adjacency exists */
	    oldstate = thisroute->flags & DN_RT_STATE_MASK;
	    state = DN_RT_STATE_INIT;
	    if ((oldstate==DN_RT_STATE_INIT) || (oldstate==DN_RT_STATE_UP)) {
		if (oursfound) {
		    state = DN_RT_STATE_UP;
 		    /* Increment the L1 adjacency count when the adj state
 		     * goes to 'up'. Note that we should increment the count
 		     * if we hear a L2 hello from someone in our own area
		     */

 		    if (oldstate==DN_RT_STATE_INIT) {
			if (nodetype == DN_NT_LEVEL1) 
			    ++((thisroute->interface)->dn_L1_adj_count);
			else if ((nodetype == DN_NT_LEVEL2) &&
				   (hisarea == net->my_dn_area)) {
			    ++((thisroute->interface)->dn_L1_adj_count);
			}
 		       
 		       /* 
                        * Take care of PhaseIV+ here. Basically, a
			* PhaseIV+ hello should supersede a Phase IV hello
  			*/
  		       if (hello->dececo == DN_PHASEIV_PLUS_VERSION) {
  			   ++((thisroute->interface)->dn_phaseIV_plus_adj_count);
  		       }
  		       else {
  			   ++((thisroute->interface)->dn_old_phaseIV_adj_count);
  		       }
			
		       /*
			* Inform CLNS, if this is a L2 adjacency
			*/
		       if (nodetype == DN_NT_LEVEL2)
			   reg_invoke_dn_inform_clns(hisarea, TRUE);

  		       thisroute->type = protocol_type;
  		    }
  		    else if (oldstate==DN_RT_STATE_UP) {
  			if (hello->dececo == DN_PHASEIV_PLUS_VERSION) {
  			    if (thisroute->type != PHASEIV_PLUS) {
				++((thisroute->interface)->dn_phaseIV_plus_adj_count);
  			       DECREMENT_OLD_PHASEIV_ADJ_COUNT(thisroute);
  			       thisroute->type = PHASEIV_PLUS;
  			    }
  			}
		       /*
			* If this is a L2 adjacency, inform CLNS. This will take
			* care of the case when there is an active DECnet adj
			* when CLNS is turned on and we need to inject the
			* relevant CLNS DECnet discard route.
			*/
		       if (nodetype == DN_NT_LEVEL2)
			   reg_invoke_dn_inform_clns(hisarea, TRUE);
 		    }
		}
		else if (oldstate == DN_RT_STATE_UP) {
		    state = DN_RT_STATE_DOWN;

 		    if (nodetype == DN_NT_LEVEL1) {
                         DECREMENT_L1_ADJ_COUNT(thisroute)
 		    }
 		    else if ((nodetype == DN_NT_LEVEL2) &&
 			     (hisarea == net->my_dn_area)) {
                         DECREMENT_L1_ADJ_COUNT(thisroute)
 		    }
 
 		    /*
 		     * Handle the PhaseIV+ case here
 		     */
 		    if (thisroute->type != PHASEIV_PLUS) {
 			DECREMENT_OLD_PHASEIV_ADJ_COUNT(thisroute);
 		    }
 		    else {
 			DECREMENT_PHASEIV_PLUS_ADJ_COUNT(thisroute);
 		    }

		    /*
		     * Inform CLNS, if this is a L2 adjacency
		     */
		    if (nodetype == DN_NT_LEVEL2)
		        reg_invoke_dn_inform_clns(hisarea, FALSE);

		    dnet_adjbuginf("\nDNET-ADJ: %d.%d adjacency down, dropped"
                                   " by adjacent node", hisarea, hisnode);
		    dn_remove_via(net,thisroute,net->dnareas,net->dn_maxroute);
		}
	    }

	    dnet_adjbuginf("\nDNET-ADJ: %s %s hello from %d.%d", 
			   nodestring[nodetype & 3], 
 		           (hello->dececo == DN_PHASEIV_PLUS_VERSION) ? 
			                     "(IV+)" :"",
			   hisarea, hisnode);
	}
	/*
	 * Else this is a new adjacency and the state is initializing. If he
	 * claims the adjacency is already up we log this unusual
	 * event.  This will happen with old cisco routers.
	 */
	else {
	    oldstate = DN_RT_STATE_NONE;
	    state = DN_RT_STATE_INIT;
	    if (decnet_rt_debug && oursfound && twoway) {
		buginf("\nDNET-ADJ: %d.%d new adjacency thinks we're up", 
		       hisarea, hisnode);
	    }

	    dnet_adjbuginf("\nDNET-ADJ: %s hello from %d.%d, creating new"
			   " adjacency",
		           nodestring[nodetype & 3], hisarea, hisnode);

	    thisroute = malloc(sizeof(dnroutetype));
	    if (!thisroute) {
		return(DN_NOMEM);
	    }
	}

	/*
	 * See if we should reject this adjacency due to exceeding 
	 * DN_MAXROUTERS. The state table says we only need
	 * to do this if the old state is NONE or REJ.
	 */
	if ((oldstate == DN_RT_STATE_REJ) || (oldstate == DN_RT_STATE_NONE)) {
	    /*
	     * Count adjacent routers on this interface. Consider it
	     * countable if it is has not been rejected. Keep track of 
	     * the adjacency with the lowest priority/id.
	     */
	    nadjacencies = 0;
	    lowest = NULL;
	    for (routeptr = net->doareas ? net->dnareas : net->dnroutes;
		 routeptr <= net->dn_maxroute; routeptr++) {
		route = *routeptr;
		for (; route; route = route->nextadj) {
		    if ((route->flags & DN_RT_ROUTER) && 
			(route->interface == idb) &&
			((route->flags&DN_RT_STATE_MASK) != DN_RT_STATE_REJ)) {
			nadjacencies++;
			/* find adjacency with lowest priority/id */
			if ((lowest == NULL) || (route->prio < lowest->prio) ||
			    ((route->prio == lowest->prio) && 
			     (dn_id(route->gateway) <
			      dn_id(lowest->gateway)))) {
			    lowest = route;
			}
		    }
		}
	    }

	    /*
	     * If we already have the limit in adjacencies we either 
	     * reject this one or we take the adjacency with the 
	     * lowest priority/id and reject it.
	     */
	    if (nadjacencies >= DN_MAXROUTERS) {
		if ((priority < lowest->prio) ||
		    ((priority == lowest->prio) && 
		     (dnid < dn_id(lowest->gateway)))) {
		    if (decnet_rt_debug && (oldstate != DN_RT_STATE_REJ)) {
			buginf("\nDNET-ADJ: %d.%d adjacency rejected", 
			       hisarea, hisnode);
		    }
		    state = DN_RT_STATE_REJ;
		} else {
		    dnet_adjbuginf("\nDNET-ADJ: %d.%d adjacency down,"
				   " max broadcast routers exceeded",
			           dn_area(lowest->gateway), 
			           dn_node(lowest->gateway));
		    lowest->flags &= ~DN_RT_STATE_MASK;
		    lowest->flags |= DN_RT_STATE_REJ;
		    
		    /* 
		     * If the adjacency being removed was level-1 only, we
		     * need to decrement the count.
		     */
 		    if ((nodetype == DN_NT_LEVEL1) && 
 			(lowest->vector) && !(lowest->areavector)) {
 		       	DECREMENT_L1_ADJ_COUNT(lowest);
 		    }
 
 		    if (lowest->type != PHASEIV_PLUS) {
 			DECREMENT_OLD_PHASEIV_ADJ_COUNT(lowest);
		    }
 		    else {
 			DECREMENT_PHASEIV_PLUS_ADJ_COUNT(lowest);
 		    }

		    /*
		     * If the adjacency being removed is the DR, set our
		     * DR info to zero.
		     */
		    if (dn_id(lowest->interface->dn_desig_router) ==
			      dn_id(lowest->gateway)) {
			lowest->interface->dn_desig_router[0] = 0;
			lowest->interface->dn_desig_router[1] = 0;
		    }

		    dn_remove_via(net, lowest, net->dnareas, net->dnroutes-1);
		}
	    }
	}

	/*
	 * If the state has changed, our hello packet will change so
	 * we are supposed to send out a hello immediately.  
	 * We will send hellos out all interfaces.  If this is decided
	 * to be a bad thing we could add a per interface changed
	 * flag so we only send hellos on interfaces that have
	 * a changed status.
	 */
	if (oldstate != state) {
	    TRIGGER_HELLOS("state changed in dn_add_adjacency");
	}

	/*
	 * These should be the remaining transition messages in
	 * the state table.
	 */
	if (decnet_rt_debug) {
	    if ((oldstate!=DN_RT_STATE_INIT) && (state==DN_RT_STATE_INIT)) {
		buginf("\nDNET-ADJ: %d.%d adjacency initializing",
		       hisarea, hisnode);
	    }	    
	    if ((oldstate==DN_RT_STATE_INIT) && (state==DN_RT_STATE_UP)) {
		buginf("\nDNET-ADJ: %d.%d adjacency up", hisarea, hisnode);
	    }
	}
    } else {				/* endnode */
	/*
	 * Create new adjacency if needed.
	 */
	if (thisroute == NULL) {  /* need new adjacency */
	    dnet_adjbuginf("\nDNET-ADJ: %s hello from %d.%d, creating new "
			   "end node adjacency %s",
			   nodestring[nodetype & 3], hisarea, hisnode,
			   (protocol_type == PHASEV)?" (Phase V type)":"");
	    thisroute = malloc(sizeof(dnroutetype));
	    if (!thisroute) {
		return(DN_NOMEM);
	    }
	} else {
	    dnet_adjbuginf("\nDNET-ADJ: %s hello from %d.%d", 
			   nodestring[nodetype & 3], hisarea, hisnode);
	}
    }

    /* 
     * Now thisroute is the new adjacency.  Put it in the right place.
     */
    routeptr = &table[index];
    route = *routeptr;
    if (routeptr > net->dn_maxroute) {
	net->dn_maxroute = routeptr;
    }

    /* 
     * for end-nodes, it's legal to route to adjacencies, so if we pass any
     * indirect routes that are worse, get rid of them.  For routers, it
     * is not legal to route to adjacencies, so forget it.  Recall that we
     * put adjacencies last, so we pass all non-adjacencies and any 
     * adjacencies that are lower cost.
     *
     * An endnode hello will supersede an NET entry if the cost is
     * better.  This is a user configuration problem.
     */

    while (route && ((route->flags & (DN_RT_HELLO|DN_RT_NET)) == 0 || 
		     route->cost < cost)) {
	if ((route->cost >= cost) && (nodetype == DN_NT_END)) {
	    dnet_robuginf("\nDNET-RT: route to %d.%d superseded by adjacency",
			  hisarea, hisnode);
	    *routeptr = route->nextadj;
	    free(route);
	}
	else if (route->type == PHASEIV_STATIC) {
	    dnet_robuginf("\nDNET-RT: Static route to %d.%d superseded by "
			  "dynamically-created adjacency", hisarea, hisnode);
	    *routeptr = route->nextadj;
	    free(route);
 	    
 	    /*
 	     * Keep track of the fact that this route was a static one, so that
 	     * we can use the (new) route that superseded the static
 	     * route while the new route still has the DN_RT_ROUTER bit
 	     * set in its 'flag'. This flag is cleared when the adjacent
 	     * router sends its first routing update, and the new
 	     * route's flags are reset to zero. We do this to avoid
 	     * blackholing packets when a static route is superseded by a
 	     * dynamic route, but the dynamic route to the next hop
 	     * is not considered "up" until the routing update is
 	     * received from the next hop.
 	     */
             route_was_static = TRUE;
	}
	else {
	    routeptr = &route->nextadj;
	}
	route = *routeptr;
    }
    thisroute->nextadj = route;
    *routeptr = thisroute;

    /*
     * Now update the entry
     *
     * If called from CLNS and we have enabled cluster aliasing, set the
     * interface to be NULL, so that conversion to Phase V will occur during
     * route look-up.
     */
    if ((pak == NULL) && idb->clns_cluster_alias) {
        thisroute->interface = NULL;
    } else {
        thisroute->interface = idb;
    }
    thisroute->hops = 1;  /* direct connection */
    thisroute->cost = cost;
    thisroute->prio = priority;
    thisroute->gateway[0] = sender[0];
    thisroute->gateway[1] = sender[1];
    thisroute->flags = flags | state;
    if (route_was_static)
 	thisroute->flags |= DN_RT_WAS_STATIC;
    TIMER_START(thisroute->routetimer, hello_time);
    thisroute->numroutes = 0;

    thisroute->nextsplit = NULL;

    /*
     * Don't supersede PhaseIV+ with PhaseIV
     */
    if (thisroute->type != PHASEIV_PLUS)
	thisroute->type = protocol_type;

    /*
     * Save the mac address for use by Phase-IV-Prime.
     */
    if (pak)
        (*idb->hwptr->extract_snpa)((paktype *)pak, &thisroute->snpa, SOURCE);

    /*
     * See if we have changed the best route.
     */
    route = table[index];
    if (route && (route->flags & (DN_RT_ROUTER|DN_RT_ME)) == 0) {
	cost = route->cost;
    } else {
	cost = -1;
    }
    if (oldcost != cost) {
	if (hisarea == net->my_dn_area) {
	    TRIGGER_L1_ROUTES("dn_add_adjacency cost change");
	}
	else {
	    TRIGGER_L2_ROUTES("dn_add_adjacency cost change");
	}
    }

    if ((flags & DN_RT_ROUTER) &&
 	(hisarea == net->my_dn_area)) {

        boolean is_dr_hello;

	is_dr_hello = ((dn_area(idb->dn_desig_router) == hisarea) &&
 		       (dn_node(idb->dn_desig_router) == hisnode));
 	/*
 	 * See if this guy should be designated router
 	 */
 	if ((priority > idb->dn_prio) || 
             (priority == idb->dn_prio && hisnode > net->my_dn_node)) {
             if (decnet_rt_debug && idb->dn_designated) {
		 buginf("\nDNET-RT: %d.%d is designated on %s",
			net->my_dn_area, hisnode, idb->namestring);
	     }
	     idb->dn_designated = FALSE;
	     TIMER_START(idb->dn_desig_timer, hello_time);
	     if (dn_id(idb->dn_desig_router) == 0) {
		 idb->dn_dr_prio = priority;
	         dn_addr(idb->dn_desig_router, hisarea, hisnode);
	     }
 	}
 	if ((is_dr_hello && (priority < idb->dn_prio)) ||
 	    (is_dr_hello && (priority != idb->dn_dr_prio))) {
 	    /*
 	     * If DR's priority has become < than ours, or DR's
	     * priority has changed, determine new DR
 	     */
 	    if (decnet_rt_debug) {
 	        buginf("\nDNET-RT: %d.%d is no longer designated on %s",
		       net->my_dn_area, hisnode, idb->namestring);
	    }
 	    idb->dn_desig_router[0] = 0;
 	    idb->dn_desig_router[1] = 0;
	    idb->dn_dr_prio = 0;
 	    GET_TIMESTAMP(idb->dn_desig_timer);
 	}
	else if ((priority > idb->dn_dr_prio) || 
		 ((priority == idb->dn_dr_prio) && 
		  (hisarea == dn_area(idb->dn_desig_router)) &&
		  (hisnode > dn_node(idb->dn_desig_router)))) {

	    /* 
	     * Sending router's priority is > priority of DR or
	     * sending router's node number is > node number of DR
	     */
	    dn_addr(idb->dn_desig_router, hisarea, hisnode);
	    idb->dn_dr_prio = priority;
	}
    }

    /*
     * Make sure routing entry has the right vectors.  We do this last so
     * we can call dn_dispose if this fails.  Note that this leaves us with
     * an entry without proper routing vectors, but the code that plays with
     * routing vectors should always check for a NULL first.
     */
    /* area */
    if (net->doareas && nodetype == DN_NT_LEVEL2) {
	if (thisroute->areavector == NULL) {
	    thisroute->areavector = dn_make_mvec(net->dn_max_area+1);
	    if (!thisroute->areavector) {
		return(DN_NOMEM);
	    }
	}
    } else {
	if (thisroute->areavector) {
	    free(thisroute->areavector);
	    thisroute->areavector = NULL;
	}
    }
    /* normal */
    if (hisarea == net->my_dn_area && ((flags & DN_RT_ROUTER) != 0)) {
	if (thisroute->vector == NULL) {
	    thisroute->vector = dn_make_mvec(net->dn_max_node+1);
	    if (!thisroute->vector) {
		return(DN_NOMEM);
	    }
	}
    } else {
	if (thisroute->vector) {
	    free(thisroute->vector);
	    thisroute->vector = NULL;
	}
    }

    /* If this was called from DECnet add, and we are doing conversion,
     * then add it to the CLNS database.
     *
     * Note: When called from CLNS, pak will be NULL.
     */
    if (pak && (flags & DN_RT_HELLO)) {
	reg_invoke_decnet_dn2clns_neighbor(pak, hisarea, hisnode, hello_time);
    }

    return(DN_SUCCESS);
}

/*
 * dn_change_mtu_size
 *
 * Change the DECnet maximum block size. This is called from 'max_mtu_command'
 * in 'network.c' when the interface MTU is changed.
 */
void dn_change_mtu_size (hwidbtype *hwidb, idbtype *swidb, int delta)
{
    idbtype *idb;
    dnnet *net;
    int blksize;

    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
        if (idb->dn_cost == 0) {
	    continue;
	}

        /*
         * Check for the blocksize being negative. If so, don't change it
         */
        blksize = idb->dn_maxblksize + delta;
        if (blksize < 0)
            continue;
	idb->dn_maxblksize = (blksize < DN_MAXPACKET ? blksize : DN_MAXPACKET);

	if (RUNNING_IF(LINK_DECNET, idb)) {
	    /* 
	     * Send a gratuitous hello, so that the other person knows we have
	     * changed our MTU
	     */
	    dn_sendhello_if(LINK_DECNET_ROUTER_L1, idb, FALSE);
	    
	    net = dn_nets[idb->dn_network];
	    if (net->doareas)
		dn_sendhello_if(LINK_DECNET_ROUTER_L2, idb, FALSE);
	    if (net->iv_prime)
		dn_sendhello_if(LINK_DECNET_PRIME_ROUTER, idb, TRUE);
	}
    }
}

dnmetrictype *dn_make_mvec (int size)
{
    dnmetrictype *vec, *ptr;

    vec = malloc(sizeof(dnmetrictype)*size);
    if (vec == NULL) {
	return(NULL);
    }
    for (ptr = vec; size; ptr++,size--) {
	ptr->cost = 1023;
	ptr->hops = 31;
    }
    return(vec);
}

/*
 * Remove any routes that go via "thisroute".
 */
void dn_remove_via (dnnet *net, dnroutetype *thisroute,
		    dnroutetype **start, dnroutetype **maxx)
{
    dnroutetype *route, **routeptr, **rtp;
    boolean is_L2 = (start < net->dnroutes);

    if (thisroute->flags & DN_RT_ROUTER)
	for (routeptr = start; routeptr <= maxx; routeptr++) {
	    rtp = routeptr;
	    route = *rtp;
	    while (route) {
		if (route->flags & (DN_RT_HELLO|DN_RT_ME|DN_RT_NET)) {
		    break;
		}
		if (route->interface == thisroute->interface &&
		    route->gateway[0] == thisroute->gateway[0] &&
		    route->gateway[1] == thisroute->gateway[1]) {
		    dnet_robuginf("\nDNET-RT: removing route to %s %d",
			          routeptr < net->dnroutes ? "area" : "node",
				  routeptr < net->dnroutes ? 
				  routeptr - net->dnareas:
				  routeptr - net->dnroutes);
		    (*routeptr)->nextsplit = NULL;
		    *rtp = route->nextadj;
		    free(route);

		    /* inform CLNS if necessary */
		    if (is_L2)
		        reg_invoke_dn_inform_clns(routeptr - net->dnareas,
						  FALSE);

		    dn_unsplit(net, routeptr, *routeptr);
		    dn_recomp(routeptr, net);
		    break;
		} else {
		    rtp = &route->nextadj;
		    route = *rtp;
		}
	    }
	}
}

/*
 * dn_rt_input
 * Process routing messages from other routers.
 * Designed to handle either level1 or level2 inputs.
 * rttable and rtendtab point to the appropriate table
 * lvl1 is a flag to indicate whether it is level 1
 */
void dn_rt_input (
    paktype *pak,
    uchar *data,
    int size,
    boolean lvl1)
{
    int his_area;
    dnroutingmsg *msg;
    int node, hops, count, sum;
    int max_index, his_node, max_cost, max_hops, old_cost, index, max_nodes;
    dnmetrictype *rtvector,*rtvecend;
    dnroutetype **routevec,**table;
    dnroutetype *rt, **rtp, *next_hop_adj;
    dnroutetype *route;	/* a4 */
    uchar hisaddress[2];
    int filter;
    dnnet *net;
    
    if (pak->if_input == NULL) {
	dnet_pabuginf("\nDNET: routing update has null input interface");
	dn_dispose(DN_FORMATERR,pak);
	return;
    }
    if ((net = dn_nets[pak->if_input->dn_network]) == NULL) {
	dn_dispose(DN_NOTNET, pak);
	return;
    }

    msg = (dnroutingmsg *)data;

    {
	uchar *ptr;	/* a3 */

	ptr = data + 3;
	sum = 1;
	count = size - (3 + 2);	/* 3 is source and fill we skip, 2 is chksum */
	count = count / 2;
	while (count > 0) {
	    sum += get16(ptr);
	    ptr += 2;
	    count--;
	}
	while (sum & 0xffff0000) {
	    sum = ((sum & 0xffff) + (sum >> 16));
	}
	if (sum != get16(data + size - 2)) {
	    dnet_pabuginf("\nDNET: routing packet has bad checksum: "
			  "computed %x, packet %x",
		          sum, get16(data + size - 2));
	    dn_dispose(DN_FORMATERR,pak);	
	    return;
	}
    }
    hisaddress[0] = msg->source[0];
    hisaddress[1] = msg->source[1];
    his_area = dn_area(hisaddress);
    his_node = dn_node(hisaddress);

    if (lvl1) {
	if (his_area != net->my_dn_area) {
	    dn_dispose(DN_OTHER_AREA_LVL1,pak);
	    return;
	}
	dn_stat[DNST_LVL1S]++;
	max_cost = net->dn_max_cost;
	max_hops = net->dn_max_hops;
	max_index = net->dn_max_node;
	max_nodes = DN_MAXNODES;
	routevec = net->dnroutes;
	table = net->dnroutes;
	index = his_node;
    } else {
	dn_stat[DNST_LVL2S]++;
	max_cost = net->dn_max_area_cost;
	max_hops = net->dn_max_area_hops;
	max_index = net->dn_max_area;
	max_nodes = DN_MAXAREAS;
	routevec = net->dnareas;
	if (his_area != net->my_dn_area) {
	    table = net->dnareas;
	    index = his_area;
	} else {
	    table = net->dnroutes;
	    index = his_node;
	}
    }
    
    /*
     * First find the adjacency for this guy.  The adjacency is only
     * valid if the state is up (two way).
     */
    route = table[index];
    while (route) {
	if ((route->flags & DN_RT_HELLO) &&
            route->interface == ((route->type == PHASEV &&
                                  route->interface->clns_cluster_alias) ? NULL 
                                              : pak->if_input) &&
	    ((route->flags & DN_RT_STATE_MASK) == DN_RT_STATE_UP) &&
	    route->gateway[0] == hisaddress[0] &&
	    route->gateway[1] == hisaddress[1]) {
	    break;
	}
	route = route->nextadj;
    }
    if (route == NULL) {
	dnet_robuginf("\nDNET-RT: routing from %d.%d: can't find adjacency",
		      his_area, his_node);
	dn_dispose(DN_NOVEC,pak);
	return;
    }

    /*
     * Determine if we are 'attached' or not - this applies only if we're a
     * Level-2 router.
     */
    if (net->doareas) {
	int i;
	dnroutetype **temp_rtvec, *temp_route;
	boolean previously_attached;
	/*
	 * Store our currently "attached" state.
	 */
	previously_attached = net->attached;
	net->attached = FALSE;
	/*
	 * Scan the L2 routing table, and see if we have connectivity to
	 * an area other than ours. If we do, we consider ourselves
	 * 'attached'.
	 */
	for (temp_rtvec = net->dnareas, i = 1; i <= net->dn_max_area; 
                          i++, temp_rtvec++) {
	    temp_route = *temp_rtvec;
	    if (temp_route == NULL)
		continue;
	    if ((temp_route->cost <= DN_MAX_COST) && (i != net->my_dn_area)) {
		net->attached = TRUE;
		break;
	    }
	}
	
	/* Handle special cases.. */
	if (previously_attached && !net->attached) {
	    /* 	
	     * Going from attached to unattached (i.e. we've lost L2
	     * connectivity). We set dnroutes[0] to NULL,
	     * so that we will create an entry for it later on. As a
	     * precaution, just ensure that dnroutes[0] points to our own
	     * entry...if not, free the entry that it points to before
	     * setting to NULL. However, if the "attach override" command
	     * is in effect, we set ourselves to be the nearest L2 router.
	     */
	    if (net->dnroutes[0] != net->dnroutes[net->my_dn_node]) {
		dn_free_rt(&net->dnroutes[0]);
	    }
	    if (dn_attach_override)
		net->dnroutes[0] = net->dnroutes[net->my_dn_node];
	    else
		net->dnroutes[0] = NULL;
	}
	else if (!previously_attached && net->attached) {
	    /* 	
	     * Going from unattached to attached. In this case, dnroutes[0]
	     * should be freed (if it doesn't point to our entry), and then
	     * set to point to our own entry.
	     */
	    if (net->dnroutes[0] != net->dnroutes[net->my_dn_node]) {
		dn_free_rt(&net->dnroutes[0]);
	    }
	    net->dnroutes[0] = net->dnroutes[net->my_dn_node];
	}
	/* 
	 * Otherwise, our "state of attachment" is unchanged, so we don't
	 * do anything.
	 */	  
    }
    /*
     * Store the pointer to the next hop adjacency, so that we can update
     * the next hop SNPA for this route
     */
    next_hop_adj = route;

    /*
     * Now get the routing vector
     */
    if (lvl1) {
	rtvector = route->vector;
    } else {
	rtvector = route->areavector;
    }
    if (rtvector == NULL) {
	dnet_robuginf("\nDNET-RT: routing from %d.%d, no routing vector",
		      his_area, his_node);
	dn_dispose(DN_NOVEC,pak);
	return;
    }

    if (decnet_rt_debug) {
        buginf("\nDNET-RT: Received %s routing from %d.%d on %s",
 	       lvl1 ? "level 1" : "level 2", his_area, his_node, 
               pak->if_input->namestring);
    }

    rtvecend = &rtvector[max_index];
    filter = pak->if_input->dn_in_filter;

    size -= 5;				/* 3 byte prolog and 2 byte checksum */
    data += 3;
    while (size >= 4) {
        dnroutetype **routeptr; /* a3 */
	dnmetrictype *vecptr;	/* a2 */

	count = get16(data);
	node = get16(data+2);
	data += 4;
	size -= 4 + (2 * count);
	if (size < 0 || (node + count) > max_nodes) {
	    dnet_pabuginf("\nDNET: routing packet has impossibly large count "
			  "(%d) or node number (%d)", count, node);
	    dn_dispose(DN_FORMATERR,pak);
	    return;
	}
	vecptr = &rtvector[node];
	routeptr = &routevec[node];
	for (;count>0;data+=2,count--,vecptr++,routeptr++) {
	    route = *routeptr;
	    node = get16(data);		/* we are reusing node as a temp */
	    hops = (node>>10) + 1;
	    node = (node&0x3ff) + pak->if_input->dn_cost;/* node is now cost */
	    if (filter) {
		int newarea, newnode;
		if (lvl1) {
		    newarea = his_area;
		    newnode = routeptr - routevec;
		} else {
		    newarea = routeptr - routevec;
		    newnode = 0;
		}
		if (!decnet_fastcheck(newarea, newnode, filter)) {
		    dnet_robuginf("\nDNET-RT: route for %d.%d, failed in "
				  "access control", newarea, newnode);
		    node = DN_MAX_COST + 1; /* say unreachable */
		}
	    }
	    if (vecptr <= rtvecend) {
		vecptr->cost = node;
		vecptr->hops = hops;
	    }
	    /*
	     * there are three major kind of changes we may make at this point:
	     * 1) an existing route has had its metric change (update it, and
	     *    look for a better route if it got worse)
	     * 2) there is no existing route (create a new one)
	     * 3) this one's better than the existing one (replace the old one)
	     * if the old route is an adjacency we don't remove it, but just
	     * stick the new one in front of it.
	     */
	    if (route && (route->flags & DN_RT_ME)) {
		continue;
	    }

            /*
	     * Do not accept routes if they are outside maxnodes or maxareas.
	     */
	    if (vecptr > rtvecend) {
		if (route) {
		    dnet_robuginf("\nDNET-RT: %s routing from %d.%d:, %d "
				  "unreachable, removed",
				  lvl1 ? "Level1" : "Level2", 
				  his_area, his_node,
				  routeptr - routevec);
		    dn_unsplit(net, routeptr, route);
		    *routeptr = route->nextadj;
		    free(route);
		    dn_recomp(routeptr, net);
 
 		    if (lvl1) {
  		       TRIGGER_L1_ROUTES("route outside maxnodes or maxareas");
 		    }
 		    else {
  		       TRIGGER_L2_ROUTES("route outside maxnodes or maxareas");
 		    }
		}
		goto nextentry;
	    }
	    /* 
	     * See if this matches an existing route.  Because
	     * of route splitting, check all routes.
	     */
	    while (route) {
		if (route->flags & (DN_RT_HELLO|DN_RT_NET)) {
		    break;
		}

		if (route->interface == pak->if_input &&
		    route->gateway[0] == hisaddress[0] &&
		    route->gateway[1] == hisaddress[1]) {

		    if (node > max_cost || hops > max_hops) {
			dnet_robuginf("\nDNET-RT: %s routing from %d.%d:, %d"
				      " unreachable, removed",
				      lvl1 ? "Level1" : "Level2", his_area,
				      his_node, routeptr - routevec);
			dn_unsplit(net, routeptr, route);
			*routeptr = route->nextadj;
			free(route);
			dn_recomp(routeptr, net);

			if (lvl1) {
			    TRIGGER_L1_ROUTES("route cost > max cost or hops > max hops");
			}
			else {
			    TRIGGER_L2_ROUTES("route cost > max cost or hops > max hops");
			}
			goto nextentry;
		    }

		    /* check for cost or hops change */
		    if (node != route->cost || hops != route->hops) {
			dnet_robuginf("\nDNET-RT: %s routing from %d.%d:, %d"
				      " metric changed %d/%d to %d/%d",
				      lvl1 ? "Level1" : "Level2", his_area,
				      his_node, routeptr - routevec,
				      route->hops, route->cost, hops, node);
			if (lvl1) {
			    TRIGGER_L1_ROUTES("cost or hops changed");
			}
			else {
			    TRIGGER_L2_ROUTES("cost or hops changed");
			}
		    }

		    /* update the route, including the next-hop snpa */
		    old_cost = route->cost;
		    route->hops = hops;
		    route->cost = node;
		    route->prio = 0;
		    route->type = PHASEIV;
		    bcopy(&next_hop_adj->snpa, &route->snpa, 
			  sizeof(hwaddrtype));

		    /* check for cost getting worse */
		    if (node > old_cost) {
			dn_unsplit(net, routeptr, route);
			dn_recomp(routeptr, net); /* may be a better route */
		    }

		    /* check for cost getting better */
		    if (node < old_cost) {
			dn_unsplit(net, routeptr, route);
		    }

		    goto nextentry;
		}
		route = route->nextadj;
	    }

	    route = *routeptr;
	    if (node > max_cost || hops > max_hops) {
		continue;
	    }

	    /*
	     * See if we want to use this gateway as a new route.
	     * Obviously if no old route we do.  If DN_RT_ROUTER is set, this
	     * is not a legal route.
	     * If new cost is better than old, use the new.
	     * If new cost is the same and the id is better, use the new.
	     * If new cost is the same and split routes allowed and
	     * this is not an adjacency, add the new.
	     *
	     * Checking for a better id depends on keeping the
	     * indirect routes with the lowest (worst) id first in the
	     * list.  Otherwise we would need to run down the whole list
	     * to see if we were better.
	     *
	     * If node zero, this is the definition of the area router.
	     * If the old route was hello type, things are sort of odd.  If
	     * it's from a router, it effectively doesn't exist, so we always
	     * create a new route.  If it's end node, we only create an
	     * indirect route if it would be better.  In either case, if
	     * we decide to do a new route, we leave the adjacency there.
	     * Assume a route cost can't equal an endnode adjacent cost.
	     *
	     * The old code was wrong.  If the cost was the same and the 
	     * id is better we don't want to create an indirect route if 
	     * what we already have is an endnode.  This is because the 
	     * endnode hello code will rip out our route and cause
	     * routes to flap continuously.  This probably didn't happen 
	     * very often as it takes an unusual topology.  Normally you 
	     * will never see an indirect route with the same cost as an 
	     * endnode adjacency.
	     */

	    /* If no route, or a router adjacency */
	    if (route == NULL || (route->flags & DN_RT_ROUTER) ||
		/* or the cost is better */
		node < route->cost ||
		/* or new route is the same cost as another route and */
		(node == route->cost && ((route->flags & DN_RT_HELLO)==0) &&
		 /* the id is better */
		 ((dn_id(hisaddress) > dn_id(route->gateway)) ||
		  /* or splitting routes is ok */
		  (route->numroutes < net->dn_max_paths)))) {

		int numroutes;
		
		rtp = NULL;

		if (decnet_rt_debug) {
		    if (route == NULL || (route->flags & DN_RT_ROUTER)) {
			buginf("\nDNET-RT: %s routes from %d.%d on %s:, "
			       "entry for %s %d created",
			       lvl1 ? "Level1" : "Level2", his_area, his_node,
			       pak->if_input->namestring,
			       lvl1 ? "node" : "area",
			       routeptr - routevec);
		    } else if (route->hops != hops || route->cost != node) {
			buginf("\nDNET-RT: %s routes from %d.%d: %d"
			       "metric changed %d/%d to %d/%d",
			       lvl1 ? "Level1" : "Level2", his_area, his_node,
			       routeptr - routevec, route->hops, route->cost,
			       hops, node);
		    }
		}

		/*
		 * See if we need to build a new route.
		 */
		/* If no route at all */
		if (route == NULL || 
		    /* or an adjacency */
		    (route->flags & (DN_RT_HELLO|DN_RT_NET)) ||
		    /* or we're adding a split route */
		    (node == route->cost && 
		     route->numroutes < net->dn_max_paths && 
		     (route->flags & DN_RT_HELLO) == 0)) {

		    /* If adding a split route */
		    if (route && node == route->cost && 
			net->dn_max_paths > 1 &&
			((route->flags & DN_RT_HELLO)==0)) {
			dnet_robuginf("\nDNET-RT: adding path to %d via "
                                      "%s -> %d.%d",
				      routeptr - routevec, 
				      pak->if_input->namestring,
				      his_area, his_node);
		    }

		    numroutes = (route) ? route->numroutes + 1 : 1;

		    route = malloc(sizeof(dnroutetype));
		    if (!route) {
			dn_stat[DN_NOMEM]++;
			continue;
		    }
		    if (lvl1 && routeptr > net->dn_maxroute) {
			net->dn_maxroute = routeptr;
		    }

		    /* 
		     * New route goes in so that the lowest (worst) 
		     * id is first, but before all adjacencies.
		     */
		    rtp = routeptr;
		    rt = *rtp;
		    while (rt) {
			if ((rt->flags & (DN_RT_HELLO|DN_RT_NET)) ||
			    (dn_id(hisaddress) < dn_id(rt->gateway))) {
			    break;
			}
			rtp = &rt->nextadj;
			rt = *rtp;
		    }
		}
		/*
		 * If the cost got better, split routes are now invalid
		 */
		else if (node < route->cost) {
		    dn_unsplit(net, routeptr, route);
		    numroutes = 1;
		}
		/*
		 * Else we're bumping someone due to a better id.
		 */
		else {
		    dnet_robuginf("\nDNET-RT: %s routes from %s -> %d.%d: "
                                  "better id for %d than %d.%d",
			          lvl1 ? "Level1" : "Level2", 
			          pak->if_input->namestring,
			          his_area, his_node,
			          routeptr - routevec,
			          dn_area(route->gateway), 
			          dn_node(route->gateway));

		    /*
		     * Since we may be bumping someone with a worse id, 
		     * we'll bump the first route on the list which is
		     * the worst id.  We unlink it and put it back in
		     * the right place shortly.
		     * Since we're unlinking the first route, we don't
		     * need to clear nextsplit.
		     */

		    numroutes = route->numroutes;
		    *routeptr = route->nextadj;
		    rtp = routeptr;
		    rt = *rtp;
		    while (rt) {
			if ((rt->flags & (DN_RT_HELLO|DN_RT_NET)) ||
			    (dn_id(hisaddress) < dn_id(rt->gateway))) {
			    break;
			}
			rtp = &rt->nextadj;
			rt = *rtp;
		    }
		}

		route->interface = pak->if_input;
		route->hops = hops;
		route->cost = node;
		route->flags = 0;
		route->prio = 0;
		route->type = PHASEIV;
		route->gateway[0] = hisaddress[0];
		route->gateway[1] = hisaddress[1];
		bcopy(&next_hop_adj->snpa, &route->snpa, sizeof(hwaddrtype));
		route->numroutes = 0;
		route->nextsplit = NULL;

		/* 
		 * Insert the route if we set an insertion point.
		 * Wait until now so route will be filled in before
		 * it is inserted (when the interrupt code could see it).
		 * Before modifying the list, clear any old split route
		 * pointers.
		 */
		if (rtp) {
		    if (*routeptr) {
			(*routeptr)->nextsplit = NULL;
			(*routeptr)->numroutes = 0;
		    }
		    route->nextadj = *rtp;
		    *rtp = route;
		}

		/* 
		 * Set split route pointers
		 */
		(*routeptr)->numroutes = numroutes;
		if (numroutes > 1) {
		    (*routeptr)->nextsplit = *routeptr;
		}

		if (lvl1) {
		    TRIGGER_L1_ROUTES("after split route pointers in dn_rt_input");
		}
		else {
 		    TRIGGER_L2_ROUTES("after split route pointers in dn_rt_input");
		}
	    }
	nextentry:;
	}
    }
    retbuffer((paktype *)pak);
}

/*
 * Remove all split routes from the routing table.
 */
void dn_unsplit_all (dnnet *net)
{
    dnroutetype *route, *bestroute, **routeptr;

    for (routeptr = net->dnareas; routeptr <= net->dn_maxroute; routeptr++) {
	/*
	 * Find best route to keep which will be last indirect route in 
	 * list since we keep them worst priority by id first.
	 */
	bestroute = route = *routeptr;
	while (route) {
	    if (route->flags & (DN_RT_HELLO|DN_RT_NET|DN_RT_ME)) {
		break;
	    }
	    bestroute = route;
	    route = route->nextadj;
	}
	if (bestroute) {
	    dn_unsplit(net, routeptr, bestroute);
	}
    }
}


/*
 * Remove any additional split routes.
 */
void dn_unsplit (
    dnnet *net,
    dnroutetype **routeptr,
    dnroutetype *route)			/* don't remove this route */
{
    dnroutetype *rt, **rtp, **routevec;

    routevec = (routeptr >= net->dnroutes) ? net->dnroutes : net->dnareas;
    rtp = routeptr;
    rt = *rtp;
    if (rt == NULL) {
	return;
    }
    rt->nextsplit = NULL;	     /* insure interrupt code isn't confused */
    rt->numroutes = 0;
    while (rt) {
	if (rt->flags & (DN_RT_HELLO|DN_RT_NET|DN_RT_ME))/* if end of routes */
	    break;

	if (rt == route) {		/* if keeping this one */
	    rtp = &rt->nextadj;
	} else {
	    dnet_robuginf("\nDNET-RT: removing multipath route to %d via"
			  "%s -> %d.%d",
			  routeptr - routevec,
			  rt->interface->namestring,
			  dn_area(rt->gateway), dn_node(rt->gateway));

	    *rtp = rt->nextadj;
	    free(rt);
	}

	rt = *rtp;
    }

    /*
     * Fixup split route parameters.
     */
    rt = *routeptr;
    if (rt) {
	if (rt->flags & (DN_RT_HELLO|DN_RT_NET|DN_RT_ME)) {
	    rt->numroutes = 0;
	} else {
	    rt->numroutes = 1;
	}
	rt->nextsplit = NULL;
    }
}

/*
 * dn_recomp
 *
 * This routine is to be called whenever a route gets worse or
 * goes away.  We look at all the routing vectors to see if we can
 * find a better route.  If we find a good route, we will fix up
 * any existing route or create one if one doesn't exist.  However
 * we won't kill an existing route if we don't find one, so you
 * should kill the existing route yourself if it is no longer
 * valid.  (This would be easy to fix, but the way this is currently
 * called make this behavior reasonable.)  It is called with a 
 * pointer into dnroutes or dnareas, pointing to the route in
 * question.  It is OK if the object in question is larger than max_node or
 * max_area, since we'll just return after doing nothing in this
 * case.  (However it is not advisable to run with max_node or
 * max_area set to be smaller than the actual largest node or area.)
 */

void dn_recomp (dnroutetype **target, dnnet *net)
{
    dnroutetype **routevec;
    dnroutetype **routeptr,*route;
    dnmetrictype *metricptr;
    dnroutetype bestroute, *oldroute;
    int max_cost, max_hops, max_index;
    int index, lvl1, area, node, next_hop_area, next_hop_node;
    int bestcost = 0xffff;
    ushort dn_address;
    decnet_staticQtype *staticQ_entry;

    if (target < net->dnroutes) {
	routevec = net->dnareas;
	max_cost = net->dn_max_area_cost;
	max_hops = net->dn_max_area_hops;
	max_index = net->dn_max_area;
	area = target - routevec;
	node = 0;
	lvl1 = 0;
    } else {
	routevec = net->dnroutes;
	max_cost = net->dn_max_cost;
	max_hops = net->dn_max_hops;
	max_index = net->dn_max_node;
	area = net->my_dn_area;
	node = target - routevec;
	lvl1 = 1;
    }
    
    dn_address = (area << 10) + node;

    index = target - routevec;
    if (index > max_index) {
	return;
    }

    for (routeptr = routevec;
	 routeptr <= &net->dnroutes[net->dn_max_node];
	 routeptr++) {
	route = *routeptr;
	for (;route;route = route->nextadj) {
	    if ((route->flags & DN_RT_ROUTER) && 
		(lvl1 ? route->vector : route->areavector)) {
		if (lvl1) {
		    metricptr = &route->vector[index];
		} else {
		    metricptr = &route->areavector[index];
		}
		if (metricptr->cost > max_cost || metricptr->hops > max_hops) {
		    continue;
		}
		if (metricptr->cost < bestcost ||
		    (metricptr->cost == bestcost &&
		     dn_id(route->gateway) > dn_id(bestroute.gateway))) {
		    bestroute.interface = route->interface;
		    bestroute.hops = metricptr->hops;
		    bestroute.cost = metricptr->cost;
		    bestroute.gateway[0] = route->gateway[0];
		    bestroute.gateway[1] = route->gateway[1];
	            bcopy(&route->snpa, &bestroute.snpa, sizeof(hwaddrtype)); 
		    bestcost = metricptr->cost;
		}
	    }
	}
    }
    oldroute = *target;
    if (bestcost < 0xffff &&
	(oldroute == NULL ||
	 (oldroute->flags & DN_RT_ROUTER) ||
	 (bestcost < oldroute->cost ||
	  (bestcost == oldroute->cost &&
	   dn_id(bestroute.gateway) > dn_id(oldroute->gateway))))) {
	if ((oldroute == NULL) || (oldroute->flags & DN_RT_HELLO)) {
	    route = malloc(sizeof(dnroutetype));
	    if (!route) {
		dn_stat[DN_NOMEM]++;
		return;
	    }
	    route->nextadj = oldroute;
	} else {
	    route = oldroute;
	    target = NULL;
	}

	/*
	 * Before installing this route, check if there is a lower cost static
	 * route in the static queue.
	 */
	staticQ_entry = decnet_search_staticQ(dn_address, TRUE);

	if (!staticQ_entry || (staticQ_entry->cost >= bestroute.cost)) {
	    dnet_robuginf("\nDNET-RT: %d backup route found from"
		          "%s -> %d.%d, metric %d/%d",
		          index, bestroute.interface->namestring,
		          dn_area(bestroute.gateway), dn_node(bestroute.gateway), 
		          bestroute.hops, bestroute.cost);
	    route->interface = bestroute.interface;
	    route->hops = bestroute.hops;
	    route->cost = bestroute.cost;
	    route->gateway[0] = bestroute.gateway[0];
	    route->gateway[1] = bestroute.gateway[1];
	    route->flags = 0;
	    bcopy(&bestroute.snpa, &route->snpa, sizeof(hwaddrtype));
	    route->prio = 0;
	    route->type = PHASEIV;
	    route->numroutes = 1;
	    route->nextsplit = NULL;
	} else {
	    /*
	     * Static route is better; install the static route.
	     */
	    next_hop_area = staticQ_entry->next_hop_dn_addr >> 10;
	    next_hop_node = staticQ_entry->next_hop_dn_addr & 0x3ff;

	    if (decnet_rt_debug) {
		if (!staticQ_entry->idb)
	            buginf("\nDNET-RT: backup STATIC route for %d found with next "
			   "hop %d.%d, metric %d/%d",
		          index, next_hop_area, next_hop_node,
		          staticQ_entry->hops, staticQ_entry->cost);
		else
	            buginf("\nDNET-RT: backup STATIC route for %d found with next "
			   "hop interface %s (%e), metric %d/%d",
		          index, staticQ_entry->idb->namestring,
			  staticQ_entry->snpa.addr,
		          staticQ_entry->hops, staticQ_entry->cost);
	    }

	    route->interface = staticQ_entry->idb;
	    route->hops = staticQ_entry->hops;
	    route->cost = staticQ_entry->cost;
	    dn_addr(route->gateway, next_hop_area, next_hop_node);
	    bcopy(&staticQ_entry->snpa, &route->snpa, sizeof(hwaddrtype)); 	
	    route->flags = 0;
	    route->prio = 0;
	    route->type = PHASEIV_STATIC;
	    route->numroutes = 1;
	    route->nextsplit = NULL;
	}
	/*
	 * Insert new route into table now that it is valid.
	 */
	if (target) {
	    *target = route;
	}
    }
    else {
	/*
	 * See if there are any 'dec advertise' or staic routes we can install.
	 *
	 * First scan the static queue to see if have a static route for this
	 * destination.
	 */

	staticQ_entry = decnet_search_staticQ(dn_address, TRUE);

        /*
	 * We need to handle any 'dec advertise' routes that may be present. This
	 * applies only for a level-2 route, and if we are doing conversion.
	 *
	 * If there is a 'dec advertise' that we can use, we need to see if
	 * there is a static route with lower cost. If there is, we install the
	 * static route; else we install the 'dec advertise' route.
	 */
	if (!lvl1 && dn_conversion &&
	                  (net->dn_adv_area[index]).cost <= DN_MAX_AREA_COST) {
	    if (staticQ_entry) {
		/*
		 * If 'dec advertise' route is lower cost, install that one. If
		 * they are equal cost, we prefer the 'dec advertise' route.
		 */
		if ((net->dn_adv_area[area]).cost <= (staticQ_entry->cost)) {
	            reg_invoke_dn_add_phaseIVroute(net, area,
						  net->dn_adv_area[area].hops,
						  net->dn_adv_area[area].cost);
	            dnet_robuginf("\nDNET-RT: Installing backup route for %d"
			          " created by 'dec advertise' command", area);
	        }
	        else {
		    /*
		     * The static route won; install that one.
		     */
		    decnet_add_static_route (net, dn_address,
					 staticQ_entry->next_hop_dn_addr,
				         staticQ_entry->hops, staticQ_entry->cost,
					 staticQ_entry->idb, &staticQ_entry->snpa,
					 staticQ_entry->is_int_static_route);
	            dnet_robuginf("\nDNET-RT: Installing backup route for %d.%d"
			        " found in static route Q", area, node);
		}
	    } /* if staticQ_entry found */
	    else {
		/*
		 * No static routes to worry about; install the 'dec advertise'
		 * route.
		 */
	         reg_invoke_dn_add_phaseIVroute(net, area,
						net->dn_adv_area[area].hops,
			                        net->dn_adv_area[area].cost);
	         dnet_robuginf("\nDNET-RT: Installing backup route for %d"
			       " created by 'dec advertise' command", area);
	    } 
	} /* if handling 'dec advertise' route */
        else if (staticQ_entry) {
	    /*
	     * If we found a static route, install it.
	     */
	    decnet_add_static_route (net, dn_address,
				 staticQ_entry->next_hop_dn_addr,
			         staticQ_entry->hops, staticQ_entry->cost,
			         staticQ_entry->idb, &staticQ_entry->snpa,
				 staticQ_entry->is_int_static_route);
	    dnet_robuginf("\nDNET-RT: Installing backup route for %d.%d"
		          " found in static route Q", area, node);
	}
    }
}

/*
 * dn_route
 *
 * Send hellos and routing updates
 * Originally, this was taking about 50 msec per invocation.  This is
 * a bit long to not process any packets, so we now allow the scheduler
 * to run a few times.
 */

void dn_route (void)
{
    dnroutetype *route,**routeptr,*rt;
    idbtype *idb;
    int oldstatus, newstatus = 0, n;
    dnroutetype *nextroute;
    dnnet *net;
    boolean timer_expired, intrfc_down;

    edisms((blockproc *)systeminitBLOCK, 0);

    /*
     * next_routing and next_hello are the next scheduled ones.
     * They are recalculated each time one goes off.  special_L?_routing
     * are used when we need to trigger a routing update because
     * something changes.  We compute a single next_activation which
     * is the first of next_routing and next_hello.  This is just to
     * keep the number of tests in dn_routeBLOCK down, since it gets
     * done a lot.  When DECnet is first started, we set the timers
     * to cause an immediate routing update and hello.
     */

    while (TRUE) {

	process_may_suspend();

	/* at least once every 10 sec for bookkeeping */
	TIMER_START(dn_next_activation, DN_MAX_ACTIVATION * ONESEC);

	/* sendhellos and sendroutes update next_hello and next_routing. */
	if (TIMER_EARLIER(dn_next_hello, dn_next_activation)) {
	    COPY_TIMESTAMP(dn_next_hello, dn_next_activation);
	}
	if (TIMER_EARLIER(dn_next_routing, dn_next_activation)) {
	    COPY_TIMESTAMP(dn_next_routing, dn_next_activation);
	}

	edisms((blockproc *)dn_routeBLOCK,0);

	if ((!SLEEPING(dn_next_hello)) ||
	    (TIMER_RUNNING_AND_AWAKE(dn_special_hellos))) {

            dnet_adjbuginf("\nDNET-ADJ: sending %shellos", 
			   (TIMER_RUNNING(dn_special_hellos)) ? "triggered " : "");
	    dn_sendhellos();	/* does process_suspend after each interface */
	}
	if ((!SLEEPING(dn_next_routing)) || 
 	    (TIMER_RUNNING_AND_AWAKE(dn_special_L1_routing)) ||
            (TIMER_RUNNING_AND_AWAKE(dn_special_L2_routing))) {
            if (decnet_rt_debug) {
 	        if (!SLEEPING(dn_next_routing))
 	            buginf("\nDNET-RT: Sending routes");
 
 	         else if (TIMER_RUNNING_AND_AWAKE(dn_special_L1_routing))
 	            buginf("\nDNET-RT: Sending L1 triggered routes");
 
 	         else if (TIMER_RUNNING_AND_AWAKE(dn_special_L2_routing))
 	            buginf("\nDNET-RT: Sending L2 triggered routes");
            }
 
            dn_sendroutes();	/* does process_suspend after each interface */
	}

	/* 
	 * See if any interfaces have changed status.  If one comes
	 * up, send a gratuitous hello and routing update.  This
	 * is essential if we are going to be able to use large
	 * update times on reliable networks.
	 */
	FOR_ALL_SWIDBS(idb) {
	    net = dn_nets[idb->dn_network];	/* warning: may be NULL */  
	    oldstatus = idb->dn_lineup;
	    newstatus = interface_up(idb) && (idb->dn_cost > 0) && net;
	    if (newstatus && ! oldstatus) {	/* if down->up */
		idb->dn_lineup = TRUE;
	        dnet_pabuginf("\nDNET: %s is up: sending hello and routes",
			      idb->namestring);

 	        if (net->iv_prime)
 	            dn_sendhello_if(LINK_DECNET_PRIME_ROUTER, idb, TRUE);
  	        if (!net->iv_prime || dn_idb_is_bilingual(net, idb)) 
 	            dn_sendhello_if(LINK_DECNET_ROUTER_L1, idb, FALSE);
		if (net->doareas)
  	            dn_sendhello_if(LINK_DECNET_ROUTER_L2, idb, FALSE);

  	        /* L1 updates */
  	        dn_sendroutes_if(idb, (idb->dn_L1_adj_count != 0), FALSE,
                                 LINK_DECNET_ROUTER_L1);
  	        dnet_robuginf("\nSending L1 routes");
  
  	        /* L2 updates to the PhaseIV+ multicast */
  	        dn_sendroutes_if(idb, FALSE, TRUE, LINK_DECNET_ROUTER_L2);
	        dnet_robuginf("\nSending L2 routes to PhaseIV+ multicast");
  
  	        /* 
  	         * Send a routing update to the old multicast address if
  	         * any "old" PhaseIV routers are present
  	         */ 
  	        if (idb->dn_old_phaseIV_adj_count != 0) {
  	            dn_sendroutes_if(idb, FALSE, TRUE,
  		  	             LINK_DECNET_ROUTER_L1);
  		dnet_robuginf("\nSending L2 routes to old PhaseIV multicast");
  	        }
    
		/* Give us a chance to hear about somebody else */
		idb->dn_designated = FALSE;
		TIMER_START(idb->dn_desig_timer, 3 * idb->dn_hello_time*ONESEC);
		process_suspend();
	    } else if (oldstatus && ! newstatus) { /* if up->down */
		idb->dn_lineup = FALSE;
	    }
	}

	for (n = 0; n < DN_MAXNETS; n++) {
	    if ((net = dn_nets[n]) == NULL) 
		continue;

	    /* loop over all routes, looking for expired adjacencies */

	    for (routeptr = net->dnareas;
		 routeptr <= net->dn_maxroute;
		 routeptr++) {
		for (route = *routeptr; route; route = nextroute) {
		    /* save, since we may delete route */
		    nextroute = route->nextadj;
		    timer_expired = !SLEEPING(route->routetimer);
		    intrfc_down = (route->interface) &&
			               (!route->interface->dn_lineup);

		    if ((route->flags & DN_RT_HELLO)   /* expire adjacencies */
			&& (timer_expired || intrfc_down)) {
			if (route->interface &&
			    dn_id(route->interface->dn_desig_router) ==
				  dn_id(route->gateway)) {
			    route->interface->dn_desig_router[0] = 0;
			    route->interface->dn_desig_router[1] = 0;
			}
			if (decnet_adj_debug) {
			    if ((route->flags&DN_RT_STATE_MASK) ==
				DN_RT_STATE_UP) {
				buginf("\nDNET-ADJ: %d.%d adjacency down, "
				       "listener timeout",
				       dn_area(route->gateway),
				       dn_node(route->gateway));

			    } else {
				buginf("\nDNET-ADJ: %d.%d adjacency down, "
				       "initialization failure",
				       dn_area(route->gateway),
				       dn_node(route->gateway));
			        if (timer_expired)
				    buginf(" due to timer expired");
				if (intrfc_down)
				    buginf(" due to interface down");
			    }
			}
			/* remove route from the list */
			if (*routeptr == route) {
			    *routeptr = route->nextadj;
			} else {
			    for (rt = *routeptr; rt; rt = rt->nextadj) {
				if (rt->nextadj == route) {
				    rt->nextadj = route->nextadj;
				    break;
				}
			    }
			}

			/*
			 * Inform CLNS about downed adjacency, if this
			 * is a L2 adjacency
			 */
			if (routeptr < net->dnroutes)
			    reg_invoke_dn_inform_clns(routeptr - net->dnareas,
						      FALSE);

			/* if this is a router, kill any routes 
			 * that go through it
			 */
			dn_remove_via(net, route, net->dnareas,
				      net->dn_maxroute);

			/* if this was a L1 router adjacency, decrement the
			 * L1 adjacency count for the interface. Note that a
			 * L2 adjacency in the same area is also considered
			 * as a L1 adjacency, so we need to decrement the
			 * count for that case as well.
			 */
			if (route->flags & DN_RT_ROUTER) {
 		            if ((route->flags&DN_RT_STATE_MASK) == 
				                             DN_RT_STATE_UP) {
				if ((route->vector) && !(route->areavector)) {
				    DECREMENT_L1_ADJ_COUNT(route)
 		                }
 		                else if ((route->areavector) && 
 			            (dn_area(route->gateway) == 
				                     net->my_dn_area)) {
				    DECREMENT_L1_ADJ_COUNT(route)
 		                }
 
				if (route->type == PHASEIV_PLUS) {
				    DECREMENT_PHASEIV_PLUS_ADJ_COUNT(route)
 		                }
				else {
 			            DECREMENT_OLD_PHASEIV_ADJ_COUNT(route)
				}
			    }
			} 

			/* and free it and its pieces */
			free(route->vector);
			free(route->areavector);
			route->nextadj = NULL;
			free(route);

			/* if there's no valid route left, look for one */
			route = *routeptr;
			if ((route == NULL) || (route->flags & DN_RT_ROUTER)) {
			    dn_recomp(routeptr, net);
			}
		    }
		}
	    }

	    /*
	     * After garbage collecting, reset dn_maxroute.
	     * Be suspicious of dnmaxroute value -- look at all possible values
	     * We count anything with a routing entry, although if it's an
	     * adjacency with a router and it has only the adjacency, we can't
	     * actually route through it.
	     */
	    for (routeptr = &net->dnareas[DN_MAXAREAS+DN_MAXNODES-1];
		 (routeptr >= net->dnroutes);
		 routeptr--) {
		if (*routeptr) {
		    net->dn_maxroute = routeptr;
		    break;
		}
	    }
	}					/* for n */

	/*
	 * If we aren't designated, it is because we have gotten a hello from
	 * someone with a higher priority.  See if that has timed out.
	 */
	FOR_ALL_SWIDBS(idb) {
	    if (idb_is_ieeelike(idb) && 
		!idb->dn_designated && !(SLEEPING(idb->dn_desig_timer))) {
                dnet_robuginf("\nDNET-RT: now designated for %s", 
			      idb->namestring);
	        idb->dn_designated = TRUE;
	    }
	}
    }					/* while TRUE */
}

/*
 * dn_routeBLOCK
 * Scheduler test for DECnet router
 */

boolean dn_routeBLOCK (void)
{
    if (dn_running) {
	if ((TIMER_RUNNING_AND_AWAKE(dn_special_L1_routing)) ||
	    (TIMER_RUNNING_AND_AWAKE(dn_special_L2_routing)) ||
	    (TIMER_RUNNING_AND_AWAKE(dn_special_hellos)) ||
	    !SLEEPING(dn_next_activation)) {
	    return(FALSE);
	}
    }
    return(TRUE);
}

/*
 * dn_sendhellos
 * Send hello out all interfaces that are supposed to get them.
 * Sets dn_next_hello
 */

void dn_sendhellos (void)
{
    idbtype *idb;
    dnnet *net;

    TIMER_START(dn_next_hello, DN_LONGTIME * ONESEC);
    FOR_ALL_SWIDBS(idb) {
	if (idb->hwptr->status & IDB_DELETED)
	    continue;

	if (idb->dn_cost > 0) {

	    /* 
	     * Normally we use dn_lineup, but we want to keep the
	     * update times up even if the interface is currently down
	     * If not ready for hello on this int, still need to look
	     * at it for purposes of recomputing dn_next_hello
	     */
	    if (SLEEPING(idb->dn_hello_timer) &&
		(!TIMER_RUNNING_AND_AWAKE(dn_special_hellos))) {
		if (TIMER_EARLIER(idb->dn_hello_timer, dn_next_hello)) {
		    COPY_TIMESTAMP(idb->dn_hello_timer, dn_next_hello);
		}
		continue;
	    }

	    /*
	     * Timer has expired.  Update timers using previous as base,
	     * not msclock, to avoid self-synchronization. If the result
	     * isn't in the future (either because we are running slow
	     * and missed an activation, or maybe because we were just
	     * started), compute the next future step.  To avoid overhead,
	     * we want all 15-sec. guys to activate at the same time, so
	     * we do this in such a way that they end up the same.  If the
	     * clock has wrapped around, we may not synchronize all the
	     * guys with the same period.  oh, well.  In fact we could do
	     * this calculation every time but adding hello_time is faster,
	     * so we try that first.
	     */

	    if (!SLEEPING(idb->dn_hello_timer)) {
		TIMER_START(idb->dn_hello_timer, idb->dn_hello_time * ONESEC);
		if (!SLEEPING(idb->dn_hello_timer)) {
		    /* round up to next interval */
		    TIMER_START_GRANULAR(idb->dn_hello_timer,
					 idb->dn_hello_time * ONESEC,
					 idb->dn_hello_time * ONESEC);
		}
	    }
	    if (TIMER_EARLIER(idb->dn_hello_timer, dn_next_hello)) {
		COPY_TIMESTAMP(idb->dn_hello_timer, dn_next_hello);
	    }

 
           net = dn_nets[idb->dn_network];	/* warning: may be NULL */  
           if (net && idb->dn_lineup) {
 	       /*
 	        * If iv-prime router, send to "All Phase IV-Prime Routers"
 	        * If the interface is bilingual or not a iv-prime router,
 	        * send to "All Phase IV Routers".
 	        */
 	       if (net->iv_prime)
 	           dn_sendhello_if(LINK_DECNET_PRIME_ROUTER, idb, TRUE);
 	       if (!net->iv_prime || dn_idb_is_bilingual(net, idb))
 	           dn_sendhello_if(LINK_DECNET_ROUTER_L1, idb, FALSE);

	       /* 
		* Send hellos to Level 2 multicast if we are a L2
		* router.
		*/
 	       if (net->doareas)
                   dn_sendhello_if(LINK_DECNET_ROUTER_L2, idb, FALSE);
 
 	       /*
 	        * If we are the designated router, also send to 
 	        * "PhIV All Endnodes"
 	        */
 	       if (idb->dn_designated) {
 	           if (net->iv_prime)
 		       dn_sendhello_if(LINK_DECNET_NODE, idb, TRUE);
 	           if (!net->iv_prime || dn_idb_is_bilingual(net, idb))
 		       dn_sendhello_if(LINK_DECNET_NODE, idb, FALSE);
 	        }
  	        process_suspend(); /* don't hog the machine */
	    }
	}
    }
    TIMER_STOP(dn_special_hellos);
}

/*
 * Routine to send hello out one interface.  Caller has decided whether
 * it should be sent, so we don't do any tests.
 */
void dn_sendhello_if (int which, idbtype *idb, int prime)
{
    paktype *pak;
    dnhdrtype *dn;
    int state;
    int length = 0;
    dnhellotype *hello;
    uchar *data;
    dnroutetype *route,**routeptr;
    int routers;
    dnnet *net;

    if ((net = dn_nets[idb->dn_network]) == NULL) {
	return;
    }

    /*
     * iv_prime only supported on Token Ring
     */
    if (prime && !(idb->hwptr->status & IDB_TR))
        return;

    pak = getbuffer(MAXDNDATASIZE);
    if (!pak) {
	return;
    }
    
    dn = (dnhdrtype *)dnheadstart(pak);

    dn->pakstart[0] = DN_CTL_TYPE_HELLO_RTR | DN_CTL;
    if (prime)
	dn->pakstart[0] |= DN_FP;
    hello = (dnhellotype *)&dn->pakstart[1];
    hello->version = DN_HELLO_VERSION;
    hello->dececo = reg_invoke_decnet_hello_version(idb);
    
    /* 
     * We need to send a Phase IV+ hello if we're running Phase IV+, and
     * do not have IV/V conversion enabled.
     */
    if ((which==LINK_DECNET_ROUTER_L2) && (!reg_invoke_dnconversion_report()))
	hello->dececo = DN_PHASEIV_PLUS_VERSION;

    hello->usereco = 0;
    bcopy(dn_e_baseaddr, hello->senderhiaddr, 4);
    dn_addr(hello->sender, net->my_dn_area, net->my_dn_node);
    hello->nodetype = net->doareas ? DN_NT_LEVEL2 : DN_NT_LEVEL1;
    length = idb->dn_maxblksize;
    dnet_adjbuginf("\nDNET-ADJ: Sending version %d.%d hellos to all %s on int "
		   "%s, blksize %d",
		   hello->version, hello->dececo,
		   dn_return_bcast_str(which),
	           idb->namestring, length);

    put16(hello->blocksize,length)
    hello->priority = idb->dn_prio;
    hello->area = 0;
    put16(hello->hello_int,idb->dn_hello_time)
    hello->odd = 0x0f;  /* ?? */
    memset(hello->zeros7, 0, sizeof(hello->zeros7));
    data = hello->adjacencies;
    length = 0;
    routers = 0;

    for (routeptr = net->doareas ? net->dnareas : net->dnroutes;
	 routeptr <= net->dn_maxroute; 
	 routeptr++) {
	route = *routeptr;
	/*
	 * Only put a router into the adjacency list if up or initializing.
	 * Flag as twoway if state is up.
	 */
	for (;route;route=route->nextadj) {
	    state = route->flags & DN_RT_STATE_MASK;
	    if ((route->interface == idb) && (route->flags & DN_RT_ROUTER) &&
		(state == DN_RT_STATE_UP || state == DN_RT_STATE_INIT)) {
		routers++;
		if (routers > DN_MAXROUTERS) {
		    continue;
		}
		bcopy(dn_e_baseaddr, data, 4);
		data[4] = route->gateway[0];
		data[5] = route->gateway[1];
		data[6] = route->prio;
		if (state == DN_RT_STATE_UP) {
		    data[6] |= DN_ADJ_UP;
		}
		data += 7;
		length += 7;
	    }
	}
    }
    hello->count2 = length;
    hello->count1 = length+8;
    put16t(dn->paksize,length+27);
    length += 29;
    pak->linktype = which;
    pak->acct_proto = ACCT_PROTO_DECNET;
    pak->if_output = idb; 
    pak->flags |= PAK_PRIORITY;
    if (pak_encap(idb->hwptr, pak, length, 0)) {
	dn_stat[DNST_HELLOSNT]++;
	datagram_out(pak);
    } else {
	dnet_pabuginf("\nDNET: Encapsulation failed (3) for interface %s",
	              idb->namestring);
	dn_dispose(DN_NOENCAP, pak);
    }
}

/*
 * dn_sendroutes
 * Send routes out all interfaces
 */

void dn_sendroutes (void)
{
    idbtype *idb;
    boolean send_L1_triggered_updates;
    boolean send_L2_triggered_updates;
    boolean routing_timer_expired; /* timer for the 'normal' routing updates */
    dnnet *net;
  
    TIMER_START(dn_next_routing, DN_LONGTIME * ONESEC);

    send_L1_triggered_updates = TIMER_RUNNING_AND_AWAKE(dn_special_L1_routing);
    send_L2_triggered_updates = TIMER_RUNNING_AND_AWAKE(dn_special_L2_routing);
    
    FOR_ALL_SWIDBS(idb) {
	if (idb->dn_cost > 0) {
	    routing_timer_expired = FALSE;
	    net = dn_nets[idb->dn_network];

	    /* if not ready for update on this int, still need to look
	     * at it for purposes of recomputing dn_next_routing */

	    if (SLEEPING(idb->dn_route_timer) && 
		(!send_L1_triggered_updates) && (!send_L2_triggered_updates)) {
		if (TIMER_EARLIER(idb->dn_route_timer, dn_next_routing)) 
		    COPY_TIMESTAMP(idb->dn_route_timer, dn_next_routing);		
		continue;
	    }

	    /* some timer has gone off for this interface.  Update to next
	     * interval.  See comments for sendhello.  Same logic. */
	    
	    if (!SLEEPING(idb->dn_route_timer)) {
		routing_timer_expired = TRUE;
		TIMER_UPDATE(idb->dn_route_timer, idb->dn_route_time * ONESEC);
		if (!SLEEPING(idb->dn_route_timer)) {
		    /* round up to next interval */
		    TIMER_START_GRANULAR(idb->dn_route_timer,
					 idb->dn_route_time * ONESEC,
					 idb->dn_route_time * ONESEC);
		}
	    }
	    if (TIMER_EARLIER(idb->dn_route_timer, dn_next_routing)) 
		COPY_TIMESTAMP(idb->dn_route_timer, dn_next_routing);
	    
	    if (idb->dn_lineup) {
		/*
		 * If we are sending triggered updates, send only
		 * relevant updates
		 */
		
		/*
		 * Only L1 routes changed - send only L1 routes. If no
		 * L1 adjacencies present, then no routes will get sent
		 */
		if (TIMER_RUNNING(dn_special_L1_routing) &&
		    !TIMER_RUNNING(dn_special_L2_routing)) {
		    if ((decnet_rt_debug) && (idb->dn_L1_adj_count != 0)) {
			buginf("\nDNET-RT: Sending L1 triggered routing "
			       "updates on %s", idb->namestring);
		    }
		    dn_sendroutes_if(idb, (idb->dn_L1_adj_count != 0), FALSE,
				     LINK_DECNET_ROUTER_L1);
		}
		
		/*
		 * Only L2 routes changed - send only L2 updates. No
		 * L1 routes sent even if there are L1 adjacencies present.
		 */
		else if (TIMER_RUNNING(dn_special_L2_routing) &&
			 !TIMER_RUNNING(dn_special_L1_routing) &&
			 net->doareas) {
		    if (decnet_rt_debug) {
			buginf("\nDNET-RT: Sending L2 triggered routing "
                               "updates to PhaseIV+ multicast on %s", 
			       idb->namestring);
		    }
 	            dn_sendroutes_if(idb, FALSE, TRUE, LINK_DECNET_ROUTER_L2);
 
		    /* 
		     * Send a routing update to the old multicast address if
		     * any "old" PhaseIV routers are present
		     */
 
		    if (idb->dn_old_phaseIV_adj_count != 0) {
			if (decnet_rt_debug) 
			    buginf("\nDNET-RT: Sending L2 triggered routing "
				   "updates to old PhaseIV multicast on %s", 
				   idb->namestring); 
 	                dn_sendroutes_if(idb, FALSE, TRUE, LINK_DECNET_ROUTER_L1);
		    }
		}
		/*
		 * Otherwise, just send both L1 and L2 updates. Of
		 * course, if no L1 adjacencies are present, we will
		 * not send L1 updates.
		 */
		else if (routing_timer_expired) {
		    /* L1 updates */
		    if (decnet_rt_debug) {
 		        buginf("\nDNET-RT: Sending periodic L1 routing"
			       " updates on %s", idb->namestring);
		    }
 	            dn_sendroutes_if(idb, (idb->dn_L1_adj_count != 0), FALSE,
                                     LINK_DECNET_ROUTER_L1);	   
 
		    /* L2 updates to the PhaseIV+ multicast */
		    if (net->doareas) {
		        if (decnet_rt_debug) 
		           buginf("\nDNET-RT: Sending periodic L2 routing updates"
 		                  " to PhaseIV+ multicast on %s",idb->namestring);

 	                dn_sendroutes_if(idb, FALSE, TRUE, LINK_DECNET_ROUTER_L2);

 	                /* 
		         * Send a routing update to the old multicast address if
		         * any "old" PhaseIV routers are present
		         */
 
		        if (idb->dn_old_phaseIV_adj_count != 0) {
		            if (decnet_rt_debug) 
 		                buginf("\nDNET-RT: Sending periodic L2 routing "
				       "updates to old PhaseIV multicast on %s",
                                       idb->namestring);
 	                    dn_sendroutes_if(idb, FALSE, TRUE,
					     LINK_DECNET_ROUTER_L1);
 	                }
		    } /* if net->doareas */
		}
		process_suspend();
	    }
	}
    }
    TIMER_STOP(dn_special_L1_routing);
    TIMER_STOP(dn_special_L2_routing);
}

/* 
 * dn_sendroutes_if
 * Send routes out an interface.  Do split horizon.
 *
 * 'lvl1' is flag which is TRUE if we want to send level 1 routes.
 *
 * 'send_L2_trig' means do not send L2 routes if this flag is FALSE (even
 * if the flag 'lvl1' is FALSE). We use this flag to suppress unnecessary
 * triggered L2 updates.
 * 
 * 'linktype' indicates which multicast address this packet is being sent to
 */

void dn_sendroutes_if (idbtype *idb, boolean lvl1, boolean send_L2_trig,
		       int linktype)
{
    int length;
    uchar *data;
    dnroutingmsg *msg;
    dnroutetype *route, **routeptr, **table;
    int thisgroup;
    ulong sum;  /* checksum */
    int metric;
    int node, nodesleft, leftinpack, basenode, limitnode;
    int maxinpack, startnode = -1;
    int packnode;			/* first node in packet */
    short dummy, *dn_start;
    boolean sentarea = FALSE;
    boolean dn_fr_split_horizon_off;
    paktype *pak;
    dnhdrtype *dn;
    dnnet *net;

    if ((net = dn_nets[idb->dn_network]) == NULL) {
	return;
    }

    /* 
     * if 'lvl1' is FALSE and 'send_L2_trig' is FALSE, do not send any L2
     * updates.
     */
    if (!lvl1 && !send_L2_trig)
	return;
     
    /*
     * Note that we don't necessarily say all we know.  Because
     * routes don't time out in DECnet, all full updates have to be
     * the same size.  Therefore the update size is a user-settable
     * parameter.  Yuck.  Note that we can also send more than we
     * know.  If we are asked to send something > dn_maxroute, just
     * say unreachable.
     */
    if (lvl1) {
	limitnode = net->dn_max_node + 1;
	if (limitnode < 1) {
	    return;
	}
	basenode = 0;
	table = net->dnroutes;
	dn_start = &idb->dn_startnode;
    } else {
	limitnode = net->dn_max_area + 1;
	basenode = 1;			/* 0 is not a legal area number */
	table = net->dnareas;
	dn_start = &dummy;		/* 64 areas always fits in 576 bytes */
	dummy = 0;
    }

    /*
     * Determine number of entries to go in this packet.
     * We now keep (per interface) the smallest allowable blocksize 
     * seen so that we don't send too big a packet to any node.
     * This was particularly a problem with DECSA routers set
     * to a default max segsize of 576.
     * 
     * For point-to-point links, we need to send a packet size of 
     * min(dn_maxblksize, dn_hismtu), and if it's less than 576, send it as
     * 576. If the medium's MTU is < 576, it will be a config error.
     */
    if (is_p2p(idb)) {
        maxinpack = min(idb->dn_maxblksize, idb->dn_hismtu);
	if (maxinpack < 576) {
	    maxinpack = 576;
	}
 	/* 
 	 * ensure that maxinpack doesn't exceed MAXDNDATASIZE !!
 	 */
 	else
	    maxinpack = min(maxinpack, MAXDNDATASIZE);
    }
    else {
        /* 
	 * for other interfaces, the minimum will still be 576. Else we take
	 * the minimum of the block size and MAXDNDATASIZE. This will avoid
	 * the Curt Bemis problem where we were sending packets larger than
	 * MAXDNDATASIZE (1524) on FDDI (default mtu: 4470). 
	 */
        if (idb->dn_maxblksize < 576) 
	    maxinpack = 576;
	else
	    maxinpack = min(idb->dn_maxblksize, MAXDNDATASIZE);
    }

    maxinpack -= 6;			/* prolog + checksum */
    maxinpack /= (4+2*DN_MAX_RTGROUP);	/* number of groups we can fit */
    maxinpack *= DN_MAX_RTGROUP;	/* number of entries */
    if (decnet_debug)
        buginf("\nDNET: Sending level %s routing updates on interface %s",
               lvl1 ? "1" : "2", idb->namestring);
    /*
     * We want to round-robin the order in which we send out
     * the packets. We do this so that if a node is missing some
     * of our updates, it will have a better chance of getting
     * a complete update eventually.
     */
    node = packnode = *dn_start * maxinpack;
    if (node >= limitnode) {
	if (net->doareas) {
	    *dn_start = 0;
	} else {
	    *dn_start = 1;
	}
    } else {
	*dn_start += 1;
    }

    /*
     * Special split horizon check for frame relay. If user has turned off
     * split horizon check for frame relay, the flag 'dn_fr_split_horizon_off'
     * will be TRUE.
     */
    dn_fr_split_horizon_off = is_frame_relay(idb->hwptr) && 
	                                         (!idb->dn_split_horizon);

    do {
	/* 
	 * Send one packet of data.
	 * Time to send the area routes if we've sent the end
	 * of the level 1 routes.
	 * If level 1 routes fit into one packet, we alternate 
	 * sending the level 1 routes or the area routes first.
	 */
	if (node >= limitnode) {	/* if time to send area routes */
	    node = packnode = basenode;
	    if (sentarea) {
		break;
	    }
	    if (net->doareas && send_L2_trig) {
 		dn_sendroutes_if(idb, FALSE, TRUE,
 				 LINK_DECNET_ROUTER_L2); /* send area routes */
	    }
	    sentarea = TRUE;
	}

	if (startnode == -1) {		/* if first time */
	    startnode = packnode;
	    node = basenode;
	} else if (packnode == startnode) {	/* if back to the beginning */
	    break;
	}

	routeptr = &table[node];
	nodesleft = limitnode - node;	/* max entries this packet could be */

	pak = getbuffer(MAXDNDATASIZE);
	if (!pak) {
	    return;
	}

	dn = (dnhdrtype *)dnheadstart(pak);

	length = 6; /* 4 byte prolog and 2 byte checksum */
	sum = 1;
	if (lvl1) {
 	    dn->pakstart[0] = DN_CTL_TYPE_RT_L1 | DN_CTL;
        } else {
 	    dn->pakstart[0] = DN_CTL_TYPE_RT_L2 | DN_CTL;
	}
	msg = (dnroutingmsg *)(dn->pakstart + 1);
	dn_addr(msg->source,net->my_dn_area,net->my_dn_node)
        msg->fill1 = 0;
        data = dn->pakstart + 4;

	leftinpack = maxinpack;
	if (leftinpack > nodesleft) {
	    leftinpack = nodesleft;
	}

        while (leftinpack > 0) {
	    /*
	     * Send one group of data.
	     * determine number of entries to go in this group.
	     */
	    thisgroup = DN_MAX_RTGROUP;
	    if (thisgroup > leftinpack) {
		thisgroup = leftinpack;
	    }
	    put16(data,thisgroup)
	    sum += thisgroup;
	    put16(data+2,node)
	    sum += node;
	    data += 4;
	    length += 4 + (2 * thisgroup);
	    nodesleft -= thisgroup;
	    leftinpack -= thisgroup;
	    node += thisgroup;

	    /*
	     * This is 1 bit zero, 5 bits hop count, 10 bits cost.
	     */
	    while (thisgroup > 0) {
		route = *routeptr;
		if (idb->dn_out_filter) {
		    if (lvl1 ?
			! decnet_fastcheck(net->my_dn_area, node-thisgroup, 
					   idb->dn_out_filter) :
			! decnet_fastcheck(node-thisgroup, 0,
					   idb->dn_out_filter)) {
			dnet_pabuginf("\nDNET: %s %d, failed out "
				      "access control",
				      lvl1 ? "node" : "area", node-thisgroup);
			route = NULL;
		    }
		}
/*
 * this is roughly the same test for a usable route as used in dn_forward,
 * except we test for information beyond the end of the routing table
 * and split horizon.  Split horizon is not part of the specs.  I use it
 * with some trepidation.
 */

		if (routeptr > net->dn_maxroute ||
		    route == NULL ||
		    (route->flags & DN_RT_ROUTER) ||
		    (((route->flags & DN_RT_HELLO) == 0) &&
		     route->interface == idb &&
		     !dn_fr_split_horizon_off &&
		     !is_x25(idb->hwptr))) {
		    put16(data,0x7fff);
		    sum += 0x7fff;
		/*
		 * Special check for 'decnet advertise' routes. If there is no
		 * corresponding CLNS prefix route for this DECnet area, do not
		 * advertise this route.
		 */
		} else if (net->doareas && (route->type == OSI_INJECTED)) {
		    if (!reg_invoke_dn_check_clns_prefix(node-thisgroup)) {
                        dnet_robuginf("\nDNET-RT: No CLNS prefix route found"
                                      " - advertising route %d with cost"
				      " infinity", node-thisgroup);
                        route->flags &= ~DN_RT_STATE_UP;
		        put16(data, 0x7fff);
		        sum += 0x7fff;
		    }
		    else {
                        dnet_robuginf("\nDNET-RT: CLNS prefix route for area"
				      " %d found" 
			              " - advertising area %d with cost %d",
			              node-thisgroup, node-thisgroup,
			              route->cost);
                        route->flags |= DN_RT_STATE_UP;                       
		        metric = (route->hops << 10) | route->cost;
		        put16(data,metric);
		        sum += metric;
		    }
		/*
		 * Special check for DECnet static routes. Do not advertise
		 * static routes if the 'redistribute' flag is off.
		 */
		} else if (route->type == PHASEIV_STATIC) {
		    if (!net->propagate_static) {
		        put16(data, 0x7fff);
		        sum += 0x7fff;
		    }
		    else {
		        metric = (route->hops << 10) | route->cost;
		        put16(data,metric);
		        sum += metric;
		    }
		} else {
		    metric = (route->hops << 10) | route->cost;
		    put16(data,metric);
		    sum += metric;
		}
		data += 2;
		thisgroup--;
		routeptr++;
	    }
        }
	/* compute and deposit checksum */
	while (sum & 0xffff0000) {
	    sum = ((sum & 0xffff) + (sum >> 16));
	}
	put16(data,sum);
	/* now go back and do count */
	put16(dn->paksize,length);
	pak->if_output = idb;
	pak->linktype = linktype;
	pak->acct_proto = ACCT_PROTO_DECNET;
	if (pak_encap(idb->hwptr, pak, length + DN_PAKSIZE_FIELD_LEN, 0)) {
	    if (lvl1) {
	        dn_stat[DNST_LVL1SNT]++;
	    } else {
	        dn_stat[DNST_LVL2SNT]++;
	    }
	    datagram_out((paktype *)pak);
	} else {
	    dnet_pabuginf("\nDNET: Encapsulation failed (4) for interface %s",
	                  idb->namestring);
	    dn_dispose(DN_NOENCAP, pak);
	}

	packnode += maxinpack;

    } while (lvl1);
}	

/*
 * dn_enqueue
 * Handle a datagram aimed at one of our interfaces.
 */

static void decnet_enqueue (paktype *pak)
{
    pak->clns_flags = 0;
    if (pak->if_input->dn_lineup) {
	process_enqueue_pak(DNpacketQ, pak);
    } else {
	protocol_discard(pak, dn_running);
    }
}

/*
 * show_decnet
 * Display DECnet statistics.
 */
void show_decnet (parseinfo *csb)
{
    int n, netnum, area, node; 
    dnnet *net;

/*  Parse optional network number for show routes. */
    netnum = GETOBJ(int,1);

    /*
     * Allow "show decnet map" even if DECnet not running.  This may or may
     * not be a good idea since we won't write maps into nvram if not
     * running.
     */
    if (csb->which == SHOW_DECNET_MAP) {
	automore_enable(NULL);
	show_dnmap();
	automore_disable();
	return;
    }
    if (!dn_running) {
	printf("%% DECnet is not enabled");
	automore_disable();
	return;
    }

    switch (csb->which) {

      case SHOW_DECNET_ACCESS:
	show_dnaccess_shell(GETOBJ(int,2));
	break;

      case SHOW_DECNET_TRAFFIC:
	dn_traffic();
	break;

      case SHOW_DECNET_ROUTE:
	/* Parse optional address.  Ignore format errors. */
	area = (GETOBJ(paddr,1)->decnet_addr >> 10) & 0x3f;
	node = (GETOBJ(paddr,1)->decnet_addr & 0x3ff);

	automore_enable(NULL);
	if (netnum != -1) {
	    if (dn_nets[netnum] != NULL) {
		show_dnroutes(dn_nets[netnum], area, node);
	    }
	} else {
	    /* all nets */
	    for (n = 0; n < DN_MAXNETS; n++) {
		if ((net = dn_nets[n]) == NULL) {
		    continue;
		}
		if (area || !n) {
		    if (area && n) {
			printf("\n");
		    }
		    show_dnroutes(net, area, node);
		}
	    }
	}
	automore_disable();
	break;

      case SHOW_DECNET_GLOBAL:
	automore_enable(NULL);
	show_dnglobal();
	automore_disable();
	break;

      case SHOW_DECNET_INTERFACE:
	automore_enable(NULL);
	if (!GETOBJ(idb,1)) {
	    show_dnglobal();
	    automore_conditional(0);
	}
	show_swidb_chain(GETOBJ(idb,1), display_dn, 0);
	automore_disable();
	break;

      case SHOW_DECNET_DEBUG:
	printf("\ndn_nets=0x%x dn_netsx=0x%x", dn_nets, dn_netsx);
	break;

      case SHOW_DECNET_NEIGHBORS:
 	automore_enable("Net Node     Interface            MAC address    Flags");
 	dn_show_neighbors();
 	automore_disable();
 	break;

      case SHOW_DECNET_STATIC:
 	automore_enable("Address      Cost    Hops             Next hop         SNPA");
 	decnet_show_static(GETOBJ(paddr,1));
 	automore_disable();
 	break;
 
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}


/*
 * show_dnglobal
 * Show global DECnet parameters
 */

void show_dnglobal (void)
{
    dnnet *net;
    int n;
    dnroutetype *route;
	
    for (n = 0; n < DN_MAXNETS; n++) {
	if ((net = dn_nets[n]) == NULL) {
	    continue;
	}

        printf("\nGlobal DECnet parameters for network %d:", n);
        printf("\n  Local address is %d.%d, node type is %s%s",
 	       net->my_dn_area, net->my_dn_node, 
 	       net->doareas ? "area" : "routing-iv",
 	       net->iv_prime ? " (Phase-IV Prime)" : "");

	if (net->doareas && net->attached) 
            printf("\n  Level-2 'Attached' flag is TRUE");
	else if (net->doareas && dn_attach_override) {
	    /* 
	     * we are unattached, but are being forced to behave like we are attached.
	     */
	    printf("\n  Level-2 'Attached' flag is FALSE, ");
	    printf("(but 'L2-attached' override command is ON)");
        } 
	else {
	    if (net->doareas) {
	        /* we are L2 and not attached */
		printf("\n  Level-2 'Attached' flag is FALSE, ");
		printf("nearest Level-2 router is  ");
	    }
	    else 
	        printf("\n  Nearest Level-2 router is  ");

	    if (net->dnroutes[0] == NULL)
		printf("NONE");
	    else {
	        route = net->dnroutes[0];
		if (!route->is_int_static_route)
		    printf("%d.%d", dn_area(route->gateway),
			            dn_node(route->gateway));
		else
		    printf("out on interface %s", route->interface->namestring);
	    }
	}

        printf("\n  Maximum node is %d, maximum area is %d, "
	       "maximum visits is %d",
	       net->dn_max_node, net->dn_max_area, net->dn_max_visits);
	printf("\n  Maximum paths is %d, path split mode is %s ", 
	       net->dn_max_paths, net->dn_interim_mode ? "interim":"normal");
        printf("\n  Local maximum cost is %d, maximum hops is %d",
	       net->dn_max_cost, net->dn_max_hops);
        printf("\n  Area maximum cost is %d, maximum hops is %d",
	       net->dn_max_area_cost, net->dn_max_area_hops);
        printf("\n  Static routes %s being sent in routing updates",
	       net->propagate_static ? "" : "*NOT*");

	if (decnet_default_route) {
	    printf("\n  Default route configured; ");
            if (!decnet_default_route->is_int_static_route)
                printf("next hop address of %d.%d ",
		       dn_area(decnet_default_route->gateway),
		       dn_node(decnet_default_route->gateway));
            else {
                if (decnet_default_route->interface)
                    printf("next hop interface of %s, ",
			   decnet_default_route->interface->namestring);
                if (decnet_default_route->snpa.length != 0)
		    printf("snpa %e", decnet_default_route->snpa.addr);
	    }
	}

	reg_invoke_decnet_show(net);
    }
}

/*
 * display_dn
 * Show the DECnet configuration of a single network interface
 */

void display_dn (idbtype *idb)
{
    dnnet *net;
    dn_info *dn_tr;

    net = dn_nets[idb->dn_network];

    printf("\n%s is %s, line protocol is %s, encapsulation is %s", 
           idb->namestring, print_idbstate(idb), 
           interface_up(idb) ? "up" : "down", encstring(idb->hwptr->enctype));
    if (idb->dn_cost <= 0) {
	printf("\n  DECnet protocol processing disabled");
	return;
    }
    printf("\n  Interface cost is %d, priority is %d, DECnet network: %d", 
	   idb->dn_cost, idb->dn_prio, idb->dn_network);
    if (idb->dn_designated) {
	printf("\n  We are the designated router");
    } else {
	if ((idb->dn_desig_router[0] != 0) && (idb->dn_desig_router[1] != 0)) {
	    printf("\n  The designated router is %d.%d",
		   dn_area(idb->dn_desig_router),
		   dn_node(idb->dn_desig_router));
	}
    }
    printf("\n  Sending HELLOs every %d seconds, routing updates %d seconds",
	        idb->dn_hello_time, idb->dn_route_time);
    printf("\n  Smallest router blocksize seen is %d bytes", 
	        min(idb->dn_hismtu, idb->dn_maxblksize));
    printf("\n  Routing input list is "); notset(idb->dn_in_filter);
    printf(", output list is "); notset(idb->dn_out_filter);
    printf("\n  Access list is "); notset(idb->dn_accesslist);
    printf("\n  DECnet fast switching is %sabled",
		    	idb->hwptr->dn_fastokay ? "en" : "dis");
    printf("\n  Number of L1 router adjacencies is : %d",idb->dn_L1_adj_count);
    printf("\n  Number of non-PhaseIV+ router adjacencies is : %d",
                        idb->dn_old_phaseIV_adj_count);
    printf("\n  Number of PhaseIV+ router adjacencies is : %d",
                        idb->dn_phaseIV_plus_adj_count);

    if (is_frame_relay(idb->hwptr))
        printf("\n  Split horizon is %s ", 
	       idb->dn_split_horizon ? "ON" : "OFF");

    if (net->iv_prime && (dn_idb_is_bilingual (net, idb)))
	printf("\n  Router is bilingual");
    if (idb->hwptr->status & IDB_TR) {
	dn_tr = idb->hwptr->dn_func;
	if (GETLONG(&dn_tr->dn_tr_iv_routers.lower) != TRFUNC_D_DN_ROUTE) 
	    printf("\n  iv-all-routers functional address is %e", 
		 &dn_tr->dn_tr_iv_routers);
	if (GETLONG(&dn_tr->dn_tr_iv_prime_routers.lower) != TRFUNC_D_DN_ROUTE)
	    printf("\n  iv-prime-all-routers functional address is %e", 
		 &dn_tr->dn_tr_iv_prime_routers);
	if (GETLONG(&dn_tr->dn_tr_iv_endnodes.lower) != TRFUNC_D_DN_NODE)
	    printf("\n  iv-all-endnodes functional address is %e", 
		 &dn_tr->dn_tr_iv_endnodes);
    }
}

static char *header;			/* pointer to header buffer */

/*
 * show_dnroutes
 * Show the DECnet routing table
 */

boolean show_dnroutes (dnnet *net, int a, int n)
{
    dnroutetype **routeptr;
    int node;
    char buffer[20];
    char hdrbuf[80];			/* for deferred header print */
    char *an;
    char *fmt = "%12s Cost  Hops           Next Hop to Node       Expires  Prio";

    if (dn_atg) {
	sprintf(buffer, "Net %d Area  ", net->netn);
    } else {
	sprintf(buffer, "  Area");
    }
    sprintf(hdrbuf, fmt, buffer);

    header = hdrbuf;

    if (a) {					/* if specific route */
	if (a == net->my_dn_area) {		/* if my area */
	    if (n == 0) {
		node = net->my_dn_node;
		routeptr = &net->dnroutes[node];
	    } else {
		node = n;
		routeptr = &net->dnroutes[node];
	    }
	} else {					/* not my area */
	    if (!net->doareas)	{ 		/* if not area router */
		node = 0;
		routeptr = &net->dnroutes[0];
	    } else {				/* we are area router */
		node = a;
		routeptr = &net->dnareas[node];
	    }
	}
	an = (routeptr < net->dnroutes) ? "Area" : "Node";
	if (dn_atg) {
	    sprintf(buffer, "Net %d %s  ", net->netn, an);
	} else {
	    sprintf(buffer, "  %s", an);
	}
	sprintf(hdrbuf, fmt, buffer);
	header = hdrbuf;

	dn_show_route(net, routeptr, node);
	return(TRUE);
    }

    for (routeptr = (net->doareas ? net->dnareas : net->dnroutes), node=0;
	routeptr <= net->dn_maxroute;
        routeptr++,node++) {

	if (routeptr == net->dnroutes) {

	    if (dn_atg) {
		sprintf(buffer, "Net %d Node  ", net->netn);
	    } else {
		sprintf(buffer, "  Node");
	    }
	    sprintf(hdrbuf, fmt, buffer);
	    header = hdrbuf;

	    node = 0;
	}

	if (!dn_show_route(net, routeptr, node)) {
	    return(FALSE);
	}
    }

    /* 
     * Display default route info, if any
     */
    if (decnet_default_route) {
	printf("\n\n*DEFAULT* : ");
	printf("%-4d%-6d",decnet_default_route->cost, decnet_default_route->hops);
        if (!decnet_default_route->is_int_static_route)
            printf("     using next hop address of %d.%d ",
		   dn_area(decnet_default_route->gateway),
		   dn_node(decnet_default_route->gateway));
        else {
            if (decnet_default_route->interface)
                printf("     out on interface %s, ",
	       	      decnet_default_route->interface->namestring);
            if (decnet_default_route->snpa.length != 0)
	        printf("snpa %e", decnet_default_route->snpa.addr);
	}
	printf("\n");
    }
    return(TRUE);
}


boolean dn_show_route (dnnet *net, dnroutetype **routeptr, int node)
{
    dnroutetype *route, *priroute, *route2, *nroute;
    char buffer[100], *str;

    route = *routeptr;
    priroute = NULL;

    while (route) {
	/*
	 * if this is a route and there is an adjacency that matches, just show
	 * the adjacency
	 * If we are going to print an adjacency entry, make sure that
	 * there is a corresponding route entry. We might try to print
	 * an adjacency entry without a route entry during the
	 * initial stages of adjacency bringup.  This is strictly
	 * cosmetic but avoids customers from thinking that we are
	 * picking up higher cost routes. 
	 */
	mem_lock(route);
	if (!(route->flags & DN_RT_ROUTER)) {
	    for (route2 = route->nextadj; route2; route2 = route2->nextadj) {
		if ((route2->flags & DN_RT_ROUTER) && 
		    route->interface == route2->interface &&
		    route->gateway[0] == route2->gateway[0] &&
		    route->gateway[1] == route2->gateway[1] &&
		    route->cost == route2->cost &&
		    route->hops == route2->hops) {
		    priroute = route2;
		    goto nextroute;
		}
	    } 
        } else {
	    for (route2 = *routeptr; route2; route2 = route2->nextadj) {
		if (!(route2->flags & DN_RT_ROUTER) &&
		    route->interface == route2->interface &&
		    route->gateway[0] == route2->gateway[0] &&
		    route->gateway[1] == route2->gateway[1] &&
		    route->cost == route2->cost &&
		    route->hops == route2->hops) {
		    break;
		}
	    }
	    if (!route2)
		goto nextroute;	
	}
		    
	if (header) {
	    printf("\n%s", header);
	    header = NULL;
	}

	if (!validmem(route)) {
	    automore_disable();
	    free(route);
	    return(FALSE);
	}

	if (((*routeptr)->nextsplit == route &&
	    (route->flags & DN_RT_ROUTER) == 0) ||
	    route == priroute ||
	    (route == *routeptr && (*routeptr)->nextsplit == NULL)) {
	    printf("\n*");
	} else {
	    printf("\n ");
	}
	      
	if (routeptr == net->dnroutes) {
	    sprintf(buffer,"(Area)");
	} else if (routeptr < net->dnroutes)  {
	    sprintf(buffer, "%d", node);
	} else {
	    str = decnet_address_lookup(net->my_dn_area, node);
	    if (str) {
	        sprintf(buffer,"%s", str);
	    }
	    else {
	        sprintf(buffer,"%d.%d", net->my_dn_area, node);
	    }
	}

	printf("%7s%-8d%-6d", buffer, route->cost, route->hops);
	if (route->flags & DN_RT_ME) {
	    str = decnet_address_lookup(net->my_dn_area,net->my_dn_node);
	    if (str) {
		printf("%-20s -> %s", "(Local)", str);
	    }
	    else {
	        printf("%-20s -> %d.%d", "(Local)", net->my_dn_area,
		       net->my_dn_node);
	    }
	    goto nextroute;
	}
	if (route->flags & DN_RT_NET) {
	    printf("%-20s", "(Mapped)");
	    goto nextroute;
	}

	if (route->type == PHASEV) {
	    printf("%-20s", "(PhaseV)");
	    printf("%-20d", TIME_LEFT_SLEEPING(route->routetimer)/ONESEC);
	    goto nextroute;
	} else if (route->type == OSI_INJECTED) {
	    str = decnet_address_lookup(net->my_dn_area,net->my_dn_node);
	    if (str) {
		printf("%-20s -> %s", "(OSI)", str);
	    }
	    else {
	        printf("%-20s -> %d.%d", "(OSI)", net->my_dn_area,
		       net->my_dn_node);
	    }
	    printf("%-12s",
		   ((route->flags & DN_RT_STATE_MASK) == DN_RT_STATE_UP) ?
		             "Up" : "Down");
	    goto nextroute;
	} else if (route->type == PHASEIV_STATIC) {
	    printf("%-12s", "(STATIC)");
	    if (route->is_int_static_route) {
	        /*
		 * Interface static route
		 */
		if (is_p2p(route->interface))
	            printf("%-20s ", route->interface->namestring);
		else 
	            printf("%-20s, snpa %s", route->interface->namestring,
			    print_snpa_string(buffer, &route->snpa));
	    }
	    else {
		/*
		 * Static route with a next hop address
		 */
                printf("  forwarding to %d.%d", dn_area(route->gateway),
			dn_node(route->gateway));		
	    }
	    goto nextroute;
	}

        str = decnet_address_lookup(dn_area(route->gateway),
				    dn_node(route->gateway));
        if (str) {
	    sprintf(buffer,"%s", str);
        }
	else {
            sprintf(buffer,"%d.%d", dn_area(route->gateway),
		    dn_node(route->gateway));
	}
	printf("%-20s -> %7s", route->interface->namestring, buffer);

	if (route->flags & DN_RT_HELLO)
	    printf("%-9d", TIME_LEFT_SLEEPING(route->routetimer)/ONESEC);
	if (route->flags & DN_RT_ROUTER)
	    printf("%-7d", route->prio);
	if (route->vector || route->areavector)
	    printf("  ");
	if (route->vector)
	    printf("V");
	if (route->areavector)
	    printf("A");
 
 	if (route->type == PHASEIV_PLUS) 
 	    printf("+");
 
	if (route->flags & DN_RT_ROUTER) {
	    if ((route->flags & DN_RT_STATE_MASK) == DN_RT_STATE_INIT)
		printf(" Initializing");
	    if ((route->flags & DN_RT_STATE_MASK) == DN_RT_STATE_DOWN)
		printf(" Down");
	    if ((route->flags & DN_RT_STATE_MASK) == DN_RT_STATE_REJ)
		printf(" Rejected");
	}

      nextroute:
	nroute = route->nextadj;
	free(route);
	route = nroute;
    }

    return(TRUE);
}

/*
 * dn_show_neighbors
 */
void dn_show_neighbors (void)
{
    int n;
    char scratch[8];
    dnnet *net;
    dnroutetype **routeptr, *route, *next;

    /* for all nets */
    for (n = 0; n < DN_MAXNETS; n++) {
	if ((net = dn_nets[n])) {

	    /* for all routes */
	    for (routeptr = (net->doareas ? net->dnareas : net->dnroutes);
		 routeptr <= net->dn_maxroute; routeptr++) {
		route = *routeptr;
		while (route) {
		    mem_lock(route);
		    if (route->flags & DN_RT_HELLO) {
			sprintf(scratch, "%d.%d", dn_area(route->gateway), 
				dn_node(route->gateway));
			printf("\n%d   %8s %20s %e ", n, scratch, 
                               route->interface->namestring, route->snpa.addr);
			if (route->vector)
			    printf("V ");
			if (route->areavector)
			    printf("A ");
			if (route->flags & DN_RT_PRIME)
			    printf("IV-PRIME");
		    }
		    next = route->nextadj;
		    free(route);
		    route = next;
		}
	    }
	}
	automore_conditional(0);
    }
}

/*
 * dn_traffic
 * DECnet statistics
 */

void dn_traffic (void)
{
    printf("\nTotal: %d received, %d format errors, %d unimplemented",
		dn_stat[DNST_RECEIVED],dn_stat[DN_FORMATERR],
		dn_stat[DN_NOTIMP]);
    printf("\n       %d not a gateway, %d no memory, %d no routing vector",
	        dn_stat[DN_NOTGATEWAY],dn_stat[DN_NOMEM],dn_stat[DN_NOVEC]);
    printf("\n       %d non-empty queue encountered",
	        dn_stat[DNST_CONGEST]);
    printf("\nHellos: %d received, %d bad, %d other area, %d sent",
	        dn_stat[DNST_HELLOS], dn_stat[DN_BADHELLO],
	        dn_stat[DN_OTHER_AREA_HELLO], dn_stat[DNST_HELLOSNT]);
    printf("\nLevel 1 routing: %d received, %d bad, %d other area, %d sent",
	        dn_stat[DNST_LVL1S], dn_stat[DN_BADLVL1],
	        dn_stat[DN_OTHER_AREA_LVL1], dn_stat[DNST_LVL1SNT]);
    printf("\nLevel 2 routing: %d received, %d not primary router, %d sent",
	        dn_stat[DNST_LVL2S], dn_stat[DN_OTHER_LVL2], 
	        dn_stat[DNST_LVL2SNT]);
    printf("\nData: %d received, %d not long format, %d too many visits",
	        dn_stat[DNST_DATAS], dn_stat[DN_NOTLONG],
	        dn_stat[DN_TOOMANYVISITS]);
    printf("\n      %d forwarded, %d returned, %d converted, "
	   "%d local destination ",
	        dn_stat[DNST_FORWARD], dn_stat[DN_RETURNED], 
	        dn_stat[DNST_CONVERT_IV], dn_stat[DNST_LOCAL]);
    printf("\n      %d access control failed, %d no route, "
	   "%d encapsulation failed",
	        dn_stat[DN_NOACCESS], dn_stat[DN_NOROUTE],
                dn_stat[DN_NOENCAP]);
    printf("\n      %d inactive network, %d incomplete map",
		dn_stat[DN_NOTNET], dn_stat[DN_NOMAP]);
};

/*
 * The following is a hack.  NDECNETCMDS is generated in decnet.m4
 * but decnet.h needs the value too soon to be included in.  So, 
 * NDECNETCMDS_H has to manually track the value of NDECNETCMDS.  There is 
 * probably a better way.
 */

/*
 * dn_var_init
 * Initialize configurable DECnet parameters
 */

void dn_var_init (dnnet *net)
{
    int i;

    for (i = 0; i < NDECNETCMDS_H; i++) {
        net->vars[i] = NULL;
    }

    net->vars[DC_MAX_COST] = &net->dn_max_cost;
    net->vars[DC_MAX_HOPS] = &net->dn_max_hops;
    net->vars[DC_AREA_MAX_COST] = &net->dn_max_area_cost;
    net->vars[DC_AREA_MAX_HOPS] = &net->dn_max_area_hops;
    net->vars[DC_MAX_VISITS] = &net->dn_max_visits;
    net->vars[DC_ROUTER_PRIORITY] = &net->dn_prio;
    net->vars[DC_MAX_ADDRESS] = &net->dn_max_node;
    net->vars[DC_MAX_AREA] = &net->dn_max_area;
    net->vars[DC_MAX_PATHS] = &net->dn_max_paths;

    net->dn_max_cost = DN_MAX_COST;
    net->dn_max_hops = DN_MAX_HOPS;
    net->dn_max_area_cost = DN_MAX_AREA_COST;
    net->dn_max_area_hops = DN_MAX_AREA_HOPS;
    net->dn_max_visits = DN_MAX_VISITS;
    net->dn_prio = DN_DEF_PRIO;
    net->dn_max_node = DN_DEF_NODE;
    net->dn_max_area = DN_MAX_AREA;
    net->dn_max_paths = 1;
    net->attached = TRUE;
    net->propagate_static = FALSE;

    for (i = 0; i < DN_MAXAREAS ; i++) {
        net->dn_adv_area[i].cost = DN_MAX_AREA_COST + 1;
        net->dn_adv_area[i].hops = DN_MAX_AREA_HOPS  + 1;
    }
}

void decnet_route(void)
{

}

/*
 * decnet_command
 * Configure global and interface specific DECnet parameters
 */

/*
 * We might want to be more conservative and flush all the
 * routes when things like max_area and max_node are changed.
 */
void decnet_command (parseinfo *csb)
{
    int area, node, netnum;
    idbtype *idb;
    dnnet *net;
    decnet_staticQtype *entry;
    static int cont = -1;   /* -1 if decnet map continuation not active */

    if (csb->nvgen) {
	nv_decnet(csb);
	return;
    }

    netnum = GETOBJ(int,1);

    /* Make sure the continuation flag/network number is reset */
    if (csb->which != DC_MAP) {
	cont = -1;
    }

    if ((net = dn_initnet(netnum)) == NULL) {
	return;
    }

    switch (csb->which) {

      case DC_NODE_TYPE:
	if (csb->sense == FALSE) {
	    printf("\nModifier 'no' not valid - %s", csb->line);
	    return;
	}
   	switch (GETOBJ(int,2)) {
	  case DNN_AREA:
	    if (net->doareas) {
		return;  /* already doing area routing */
	    }
	    /* 
	     * If here, we are turning on area routing
	     * We have to free the entry that used to point to the nearest
	     * router, and then set it up to point to us, as we do when
	     * area routing is on.
	     */
	    dn_free_rt(&net->dnroutes[0]);
	    net->dnroutes[0] = net->dnroutes[net->my_dn_node];
	    
	    /*
	     * Create a L2 entry for our area if it doesn't exist.
	     */
	    if (net->dnareas[net->my_dn_area] == NULL) {
	        dnroutetype *route;

		route = malloc(sizeof(dnroutetype));
		if (!route) {
		    return;
		}
		dn_clear_rt(route);
	        route->interface = NULL;
	        route->hops = 0;
	        route->cost = 0;
	        route->flags = DN_RT_ME;
	        route->prio = 0;
	        net->dnareas[net->my_dn_area] = route;
	    }

	    net->doareas = TRUE;	    

	    /*
	     * Install any static routes from the static route queue, if they
	     * are relevant.
	     */
	    for (entry = (decnet_staticQtype *) decnet_staticQ.qhead; entry; 
		 entry = entry->next) {
		decnet_add_static_route (net,
					 entry->dn_addr, entry->next_hop_dn_addr,
					 entry->hops, entry->cost,
					 entry->idb, &entry->snpa,
					 entry->is_int_static_route);
	    }
	    break;

	  case DNN_ROUTING_IV:
	    if (net->doareas == FALSE) {
		return;  /* already OK */
	    }
	    /*
	     * if here, we are turning off area routing.  dnroutes is
	     * a copy of our own entry.   We have to clear it, or things
	     * will get very confused.
	     */
	    net->dnroutes[0] = NULL;
	    {
		dnroutetype **routeptr;

		for (routeptr = net->dnareas;
		     routeptr<&net->dnareas[DN_MAXAREAS]; routeptr++) {
		    dn_free_rt(routeptr);
		}
	    }

	    /* 
	     * clean up any CLNS discard routes that were created by
	     * us when we were running as L2. Do this before we set
	     * net->doareas to FALSE, since the routine
	     * "dn_clear_discard_routes" will not do the right thing if
	     * net->doareas is FALSE.
	     */
	    reg_invoke_dn_clear_discard_routes();

	    net->doareas = FALSE;

	    /*
	     * Install any static routes from the static route queue, if they
	     * are relevant.
	     */
	    for (entry = (decnet_staticQtype *) decnet_staticQ.qhead; entry; 
		 entry = entry->next) {
		decnet_add_static_route (net,
					 entry->dn_addr, entry->next_hop_dn_addr,
					 entry->hops, entry->cost,
					 entry->idb, &entry->snpa,
					 entry->is_int_static_route);
	    }
	    break;

	  default:
	    bad_parser_subcommand(csb, GETOBJ(int,2));
	    break;
	}
	break;

      case DC_ADDRESS:
      case DC_ROUTING:
	if (csb->sense == FALSE) {
	    /*
	     * turning off "no decnet routing" we don't care if they
	     * gave us area.node.
	     */
	    dn_running = FALSE;
	    FOR_ALL_SWIDBS(idb) {
		if (idb->dn_cost != -1) {
		    idb->dn_cost = -1;
		    dn_disable_if(idb);
		}
 		idb->dn_L1_adj_count = 0;
 		idb->dn_old_phaseIV_adj_count = 0;
 		idb->dn_phaseIV_plus_adj_count = 0;
		decnet_rxtypes_update(idb);
	    }
	    for (netnum = 0; netnum < DN_MAXNETS; netnum++) {
		dn_nets[netnum] = NULL;
	    }

	    /* Clean up CLNS static routes that were created as a result
	     * of having DECnet conversion on.
	     */
	    reg_invoke_dn_clear_discard_routes();

	    /*
	     * Delete default route
	     */	    
	    decnet_remove_default_route();

	    /* should we zero dn_atg here ? */

	    /*
	     * Kill the routing process.
	     */
	    dn_cleanup();
	    return;
	}

	area = GETOBJ(paddr,1)->decnet_addr >> 10;
	node = GETOBJ(paddr,1)->decnet_addr & 0x3ff;
	/*
	 * Turning on DECnet.  Check to see if this is okay dependent on
	 * the configuration.
	 */
	if (node > 0 && node < DN_MAXNODES && 
	    area > 0 && area < DN_MAXAREAS) {

	    /*  OK to use dnnet struct now */
	    dn_nets[netnum] = dn_netsx[netnum];

 	    dn_setaddr(area, node, net, GETOBJ(int,2));

	    /* reinstate static routes */
	    decnet_reinstate_static_routes(net);

	    /* reinstate DECnet 'advertised' routes */
	    reg_invoke_dn_reinstate_adv_routes(net);

	} else {
	    printf("\nBad DECnet address in command - \"%s\"", csb->line);
	}
	break;

      case DC_MAP:
	/* Save netnum if this line contains a continuation character. 
	 * There are four possible states:
	 * decnet 0 map 1.2 2 2.2 \  val5 == TRUE; cont == -1 [set cont = netnum]
	 * 1.3 2 2.3 \		     val5 == TRUE; cont == netnum [use cont]
	 * 1.4 2 2.4		     val5 == FALSE; cont == netnum [use cont & set cont = -1]
	 * decnet 1 map 3.4 3 4.4    val5 == FALSE; cont == -1   [set cont = -1]
	 */
	if (cont == -1) {
	    if (GETOBJ(int,5) == TRUE) {
		cont = netnum;
	    }
	} else {
	    if ((net = dn_initnet(cont)) == NULL) {
		return;
	    }
	    if (GETOBJ(int,5) == FALSE) {
		cont = -1;
	    }
	}
	decnetmap_command(csb, net);
	break;

    case DC_PATH_SPLIT_MODE:
	if (csb->sense == FALSE) {
	    net->dn_interim_mode = FALSE;
	    return;
	}
	net->dn_interim_mode = (GETOBJ(int,2) == DNM_NORMAL) ? FALSE : TRUE;
	break;

    case DC_AREA_MAX_COST:
	if (csb->sense == FALSE) {
	    net->dn_max_area_cost = DN_MAX_AREA_COST;
	    return;
	}
	net->dn_max_area_cost = GETOBJ(int,2);
	break;

    case DC_AREA_MAX_HOPS:
	if (csb->sense == FALSE) {
	    net->dn_max_area_hops = DN_MAX_AREA_HOPS;
	} else {
	    net->dn_max_area_hops = GETOBJ(int,2);
	}
	break;

    case DC_MAX_ADDRESS:
	if (csb->sense == FALSE) {
	    net->dn_max_node = DN_DEF_NODE;
	} else {
	    net->dn_max_node = GETOBJ(int,2);
	}

	{
	    dnroutetype **routeptr,*route;

	    for (routeptr = net->dnroutes;
		 routeptr <= net->dn_maxroute;
		 routeptr++) {
		route = *routeptr;
		while (route) {
		    free(route->vector);
		    route->vector = NULL;
		    route = route->nextadj;
		}
	    }
	}
	break;

    case DC_MAX_AREA:
	if (csb->sense == FALSE) {
	    net->dn_max_area = DN_MAX_AREA;
	} else {
	    net->dn_max_area = GETOBJ(int,2);
	}

	{
	    dnroutetype **routeptr,*route;

	    for (routeptr = net->dnareas;
		 routeptr < net->dn_maxroute;
		 routeptr++) {
		route = *routeptr;
		while (route) {
		    free(route->areavector);
		    route->areavector = NULL;
		    route = route->nextadj;
		}
	    }
	}
	break;

    case DC_MAX_COST:
	if (csb->sense == FALSE) {
	    net->dn_max_cost = DN_MAX_COST;
	} else {
	    net->dn_max_cost = GETOBJ(int,2);
	}
	break;

    case DC_MAX_HOPS:
	if (csb->sense == FALSE) {
	    net->dn_max_hops = DN_MAX_HOPS;
	} else {
	    net->dn_max_hops = GETOBJ(int,2);
	}
	break;

    case DC_MAX_PATHS:
	if (csb->sense == FALSE) {
	    net->dn_max_paths = 1;
	} else {
	    net->dn_max_paths = GETOBJ(int,2);
	    dn_unsplit_all(net);
	}
	break;

    case DC_MAX_VISITS:
	if (csb->sense == FALSE) {
	    net->dn_max_visits = DN_MAX_VISITS;
	} else {
	    net->dn_max_visits = GETOBJ(int,2);
	}
	break;

    case DC_ROUTER_PRIORITY:
	printf("\nRouter priority is now an interface subcommand");
	break;

    case DC_HOST:
	decnet_host_command(csb);
	break;

    case DC_ROUTE:
	decnet_static_route_command(csb);
	break;

    case DC_PROPAGATE:
	if (csb->sense == FALSE) {
	    net->propagate_static = FALSE;
	} else {
	    net->propagate_static = TRUE;
	}
	break;

    case DC_ATTACH_OVERRIDE:
	dn_attach_override = csb->sense;
	if (net->doareas) {
	    if (dn_attach_override) {
		if (!net->attached) {
		    /*
		     * If we are not 'attached', and we're overriding this state,
		     * Behave like we're attached (i.e. point to ourselves as nearest L2 router).
		     */
		    if (net->dnroutes[0] != net->dnroutes[net->my_dn_node]) {
			dn_free_rt(&net->dnroutes[0]);
		    }
		    net->dnroutes[0] = net->dnroutes[net->my_dn_node];
		}
	    }
	    else {
		if (!net->attached) {
		    /*
		     * If we are not 'attached', and the override was turned off.
		     * Behave like we're unattached (i.e. nearest L2 router is NULL). 
		     */
		    if (net->dnroutes[0] != net->dnroutes[net->my_dn_node]) {
			dn_free_rt(&net->dnroutes[0]);
		    }
		    net->dnroutes[0] = NULL;
		}
	    }
	}
	break;

    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * decnetif_command
 * Handle the interface specific variant of the decnet configuration command.
 */

void decnetif_command (parseinfo *csb)
{
    idbtype *idb = csb->interface;
    uchar *macaddr;
    dnnet *net;
    int index;
    int netnum;

    if (csb->nvgen) {
	nv_decnetif(csb->nv_command, csb->interface);
	return;
    }

    netnum = GETOBJ(int,1);
    index = csb->which;

    /*
     * Default the net number to the current network so we don't
     * let the user accidentally switch an interface to another
     * interface by forgetting to specify the network number.
     */
    if (netnum == -1) {
	netnum = idb->dn_network;
    }
    if ((net = dn_initnet(netnum)) == NULL) {
        return;
    }

    /* 
     * Don't allow the user to change the network number of an
     * active interface.  This helps prevent typos from changing
     * an interface's network.
     */
    if ((idb->dn_network != netnum) &&
	(idb->dn_cost != -1)) {
	    printf("\nCan't change the network of an active interface.");
	    printf("\nUse: \"no decnet cost\" first.");
	    return;
    }

    idb->dn_network = netnum;
    
    if ((index != DC_COST) && (idb->dn_cost <= 0)) {
	printf("\n%% Warning: decnet must be enabled on interface using"
	       " \n           \"decnet cost\" command for changes to take"
	       " effect");
    }

    /*
     * When any of the timing parameters is changed, or an interface is
     * turned on, set the timers to go off immediately.  This is to avoid
     * the possibility that there might be an old value lying around that
     * is far in the future.  By making it msclock - timer, we guarantee
     * that they'll go off immediately, and also that the code that computes
     * the next activation will recompute from scratch rather than just
     * adding an increment.  See the code in dn_sendhellos for details.  
     * We also set the activation timers for the router to go off immediately.
     */
    switch (index) {
      case DC_CONGEST_THRESH:
	if (csb->set_to_default)
	    idb->hwptr->dn_congest_limit = DN_DEF_CATHRESH;
	else
	    idb->hwptr->dn_congest_limit = csb->sense ? GETOBJ(int,2) : 0x7fff;
	break;

      case DC_ROUTE_CACHE:
	idb->hwptr->dn_routecache = csb->set_to_default || csb->sense;
	dn_fastsetup(idb->hwptr);

        /*
         * Print warning message if this interface (or encapsulation) does
         * support fast switching.
         */
         if (csb->sense && !(idb->hwptr->fast_switch_flag & FSF_DECNET)) {
             printf("\n Fast switching not supported for "
		    "this interface/encapsulation");
	 }

	break;

      case DC_ACCESS_GROUP:
	idb->dn_accesslist = csb->sense ? GETOBJ(int,2) : 0;
	break;

      case DC_COST: 
        if (csb->sense == FALSE) {
	    idb->dn_cost = -1;
	    decnet_rxtypes_update(idb);
	    idb->dn_network = 0;	/* revert back to default */
	    dn_disable_if(idb);
	    return;
	}
        if (idb->dn_cost < 0) {			/* if going off to on */
	    idb->dn_cost = GETOBJ(int,2);	/* needed by dn_fastsetup */
	    dn_enable_if(idb);			/* and reset code */
	}
        idb->dn_cost = GETOBJ(int,2);
	decnet_rxtypes_update(idb);
	if (idb->dn_prio == 0) {		/* if no priority set yet */
	    idb->dn_prio = net->dn_prio;
	}
	dn_echo_interface = idb;
	break;

      case DC_HELLO_TIME:
        idb->dn_hello_time = csb->sense ? GETOBJ(int,2) : DN_HELLO_TIME;
	GET_TIMESTAMP(idb->dn_hello_timer);
	GET_TIMESTAMP(dn_next_hello);
	GET_TIMESTAMP(dn_next_activation);
	break;

      case DC_ENCAPSULATION:
	if (!(idb->hwptr->status & IDB_TR)) {
	    printf("\nDECnet encaps only applies to token rings");
	    return;
	}

	if ((csb->sense == FALSE) || (DNE_DEC == GETOBJ(int,2))) {
	    PUTLONG(&idb->hwptr->dn_func->dn_tr_iv_routers.lower,
		    TRFUNC_D_DN_ROUTE);
	    PUTLONG(&idb->hwptr->dn_func->dn_tr_iv_prime_routers.lower,
		    TRFUNC_D_DN_ROUTE);
	    PUTLONG(&idb->hwptr->dn_func->dn_tr_iv_endnodes.lower,
		    TRFUNC_D_DN_NODE);
	} else {
	    PUTLONG(&idb->hwptr->dn_func->dn_tr_iv_routers.lower,
		    TRFUNC_C_DN_ROUTE);
	    PUTLONG(&idb->hwptr->dn_func->dn_tr_iv_prime_routers.lower,
		    TRFUNC_C_DN_ROUTE);
	    PUTLONG(&idb->hwptr->dn_func->dn_tr_iv_endnodes.lower,
		    TRFUNC_C_DN_NODE);
	}

	if (idb->dn_cost > 0) {		/* if functionals might have changed */
	    dn_enable_if(idb);
	}
	break;
	
       case DC_MULTI_MAP:
        if (!(idb->hwptr->status & IDB_TR)) {
 	    printf("\nDECnet multicast-map only applies to token rings");
 	    return;
 	}
 
        macaddr = GETOBJ(hwaddr,1)->addr;
 	/*
 	 * Verify proper functional address
 	 */
 	if (csb->sense) {
 	    if ((macaddr[0] != 0xc0) || (macaddr[1] != 0)) {
 		printf("\nFunctional addresses must be C000.xxxx.xxxx");
 		return;
 	    }
 	}
 
 	switch (GETOBJ(int,2)) {
 	case DNE_ALLROUTERS:
 	    if (csb->sense)
 		bcopy(&macaddr[2], &idb->hwptr->dn_func->dn_tr_iv_routers.lower, 4);
 	    else
 		PUTLONG(&idb->hwptr->dn_func->dn_tr_iv_routers.lower,
			TRFUNC_D_DN_ROUTE);
 	    break;
 	case DNE_ALLPRIMEROUTERS:
 	    if (csb->sense)
 		bcopy(&macaddr[2],
		      &idb->hwptr->dn_func->dn_tr_iv_prime_routers.lower,4);
 	    else
 		PUTLONG(&idb->hwptr->dn_func->dn_tr_iv_prime_routers.lower,
			TRFUNC_D_DN_ROUTE);
 	    break;
 	case DNE_ALLENDNODES:
 	    if (csb->sense)
 		bcopy(&macaddr[2], &idb->hwptr->dn_func->dn_tr_iv_endnodes.lower, 4);
 	    else
 		PUTLONG(&idb->hwptr->dn_func->dn_tr_iv_endnodes.lower,
			TRFUNC_D_DN_NODE);
 	    break;
 	}
 	if (idb->dn_cost > 0) {		/* if functionals might have changed */
 	    dn_enable_if(idb);
 	}
 	break;
 
      case DC_ROUTE_TIME:
        idb->dn_route_time = csb->sense ? GETOBJ(int,2) : DN_ROUTE_TIME;
	GET_TIMESTAMP(idb->dn_route_timer);
	GET_TIMESTAMP(dn_next_routing);
	GET_TIMESTAMP(dn_next_activation);
	break;

      case DC_IN_FILTER:
	idb->dn_in_filter = csb->sense ? GETOBJ(int,2) : 0;
	break;

      case DC_OUT_FILTER:
	idb->dn_out_filter = csb->sense ? GETOBJ(int,2) : 0;
	break;

      case DC_ROUTER_PRIORITY:
	idb->dn_prio = csb->sense ? GETOBJ(int,2) : DN_DEF_PRIO;

	/* 
	 * if we are the DR, send out a gratuitous hello, so that adjacencies
	 * will know about the change in priority sooner.
	 */
	if (idb->dn_designated) {
	    dn_sendhello_if(LINK_DECNET_ROUTER_L1, idb, FALSE);
	}
	/* 
	 * also determine who new DR will be
	 */
	GET_TIMESTAMP(idb->dn_desig_timer);
	break;

      case DC_SPLIT_HORIZON:
	idb->dn_split_horizon = csb->set_to_default || csb->sense;
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * dn_disable_if
 * Called when turning off an interface.
 * Put it here to insure we always do it the same way.
 */
void dn_disable_if (idbtype *idb)
{
    idbtype *swidb;
    boolean lastone = TRUE;

    FOR_ALL_SWIDBS_ON_HW(idb->hwptr, swidb)
	if (swidb->dn_cost != -1)
	    lastone = FALSE;
    if (lastone)
	coerce_hwaddr(idb->hwptr->bia, idb->hwptr);
	    
    dn_fastsetup(idb->hwptr);
    /*
     * Token Ring has enough problems with out being forced
     * to continue listening to DECnet Multicasts.  Turn them
     * off by forcing a reset.
     */
    if (idb->hwptr->status & IDB_IEEEMASK) {
	(*idb->hwptr->reset)(idb->hwptr);
    }
}

/*
 * dn_enable_if
 * Called when turning on an interface.
 */

void dn_enable_if (idbtype *idb)
{
    uchar *newaddr;

    /*
     * Initialize some timers.
     * We do the same initialization when setting our address or when an
     * individual interface is turned on. There's no way to tell whether
     * they're going to do the set address or set cost commands first, so we
     * do them both places.  We don't bother to test whether we're coming up
     * for the first time.  If we're changing our address, it probably makes
     * sense to issue gratuitous updates, which is what the effect will be.
     *
     * Also say we have seen somebody else who should be designated router.
     * If there isn't anyone, it will time out shortly.  This prevents us
     * from temporarily claiming to be designated before we have seen the
     * other guys' hellos
     */

    GET_TIMESTAMP(idb->dn_hello_timer);
    GET_TIMESTAMP(idb->dn_route_timer);
    idb->dn_designated = FALSE;
    TIMER_START(idb->dn_desig_timer, 3 * idb->dn_hello_time*ONESEC);
    idb->dn_desig_router[0] = 0;
    idb->dn_desig_router[1] = 0;
    idb->dn_maxblksize = (idb->hwptr->maxmtu < DN_MAXPACKET ? 
			          idb->hwptr->maxmtu : DN_MAXPACKET);
    idb->dn_hismtu = DN_MAXPACKET;
    GET_TIMESTAMP(dn_next_hello);
    GET_TIMESTAMP(dn_next_routing);
    GET_TIMESTAMP(dn_next_activation);

    dn_fastsetup(idb->hwptr);

    /*
     * engage new mac address and turn on multicasts
     */
    if (dn_running && ((idb->hwptr->status & IDB_IEEEMASK)||
	(is_atm(idb->hwptr))) &&
	(newaddr = pick_dn_802addr(idb))) {
	coerce_hwaddr(newaddr, idb->hwptr);
    }
}

/*
 * nv_decnetif
 * Write decnet-interface commands to non-volatile memory
 */
void nv_decnetif (char *nv_command, idbtype *idb)
{
    char decnet[10];

    if (!dn_running || (idb->dn_cost <= 0)) {
	return;
    }

    /*
     * If ATG always include optional integer
     */
    if (dn_atg || idb->dn_network) {
	sprintf(decnet, "%s %d", nv_command, idb->dn_network);
    } else{
	sprintf(decnet, nv_command);
    }

    nv_write(TRUE, "%s cost %d", decnet, idb->dn_cost);
    nv_write(!idb->hwptr->dn_routecache, "no %s route-cache", decnet);
    nv_write(idb->dn_hello_time != DN_HELLO_TIME, "%s hello-timer %d", 
		 decnet, idb->dn_hello_time);
    nv_write(idb->dn_route_time != DN_ROUTE_TIME, "%s routing-timer %d", 
		 decnet, idb->dn_route_time);
    nv_write(idb->dn_prio != DN_DEF_PRIO, "%s router-priority %d",
	     decnet, idb->dn_prio);
    nv_write(idb->dn_accesslist, "%s access-group %d",
		decnet, idb->dn_accesslist);
    nv_write(idb->dn_in_filter, "%s in-routing-filter %d",
		decnet, idb->dn_in_filter);
    nv_write(idb->dn_out_filter, "%s out-routing-filter %d",
		decnet, idb->dn_out_filter);
    if (idb->hwptr->dn_congest_limit == 0x7fff) {
	nv_write(TRUE, "no %s congestion-threshold", decnet);
    } else {
	nv_write(idb->hwptr->dn_congest_limit != DN_DEF_CATHRESH, 
		 "%s congestion-threshold %d", decnet,
		 idb->hwptr->dn_congest_limit);
    }

    /* NVGEN only if split horizon is OFF, and interface is frame relay */
    if (is_frame_relay(idb->hwptr) && !idb->dn_split_horizon)
	nv_write(TRUE, "no decnet split-horizon");
 
    if (idb->hwptr->status & IDB_TR) {
        nv_write((GETLONG(&idb->hwptr->dn_func->dn_tr_iv_routers.lower)
		  != TRFUNC_D_DN_ROUTE),
 		 "decnet multicast-map iv-all-routers %e", 
 		 &idb->hwptr->dn_func->dn_tr_iv_routers);
 	nv_write((GETLONG(&idb->hwptr->dn_func->dn_tr_iv_prime_routers.lower)
		  != TRFUNC_D_DN_ROUTE),
 		 "decnet multicast-map iv-prime-all-routers %e", 
 		 &idb->hwptr->dn_func->dn_tr_iv_prime_routers);
 	nv_write((GETLONG(&idb->hwptr->dn_func->dn_tr_iv_endnodes.lower)
		  != TRFUNC_D_DN_NODE),
 		 "decnet multicast-map iv-all-endnodes %e", 
 		 &idb->hwptr->dn_func->dn_tr_iv_endnodes);
    }
}

/*
 * nv_decnet
 * Write decnet config commands to non-volatile memory
 */

void nv_decnet (parseinfo *csb)
{
    dnmap *map;
    dnnet *net;
    int i, n;
    int toarea, tonode, fromarea, fromnode;
    int first, len;
    char *line;
    char decnet[10];

    if (dn_running != TRUE) {
	return;
    }

    if (csb->which == DC_HOST) {
        nv_decnet_host_cmd(csb);
        return;
    }
    else if (csb->which == DC_ROUTE) {
        nv_decnet_route_cmd(csb);
        return;
    }
    else if (csb->which == DC_PROPAGATE) {
	/*
	 * For now, this will work only for net number zero
	 */
	net = dn_nets[0];
	if (net && net->propagate_static) {
	    nv_write(TRUE, "!");
	    nv_write(TRUE, "decnet propagate static");
	}
	return;
    }
    else if (csb->which == DC_ATTACH_OVERRIDE) {
	nv_write(dn_attach_override, "decnet attach override");
	return;
    }

    line = string_getnext();
    for (n = 0; n < DN_MAXNETS; n++) {
	if ((net = dn_nets[n]) == NULL) {
	    continue;
	}
	/*
	 * If ATG always include optional integer
	 */
	if (dn_atg || n) {
	    sprintf(decnet, "%s %d", csb->nv_command, n);
	} else {
	    sprintf(decnet, csb->nv_command);
	}
	
	nv_write(TRUE, "!");

 	nv_write(TRUE, "%s routing %s%d.%d", decnet, 
 		 (net->iv_prime)? "iv-prime " : "",
 		 net->my_dn_area, net->my_dn_node);

	nv_write(TRUE, "%s node-type %s", decnet,
		 net->doareas ? "area" : "routing-iv");
	nv_write(net->dn_interim_mode, "%s path-split-mode interim", decnet);

	nv_write(net->dn_max_area_cost != DN_MAX_AREA_COST,
		 "%s area-max-cost %d", decnet, net->dn_max_area_cost);

	nv_write(net->dn_max_area_hops != DN_MAX_AREA_HOPS,
		 "%s area-max-hops %d", decnet, net->dn_max_area_hops);

	nv_write(net->dn_max_node != DN_DEF_NODE,
		 "%s max-address %d", decnet, net->dn_max_node);

	nv_write(net->dn_max_area != DN_MAX_AREA,
		 "%s max-area %d", decnet, net->dn_max_area);

	nv_write(net->dn_max_cost != DN_MAX_COST,
		 "%s max-cost %d", decnet, net->dn_max_cost);

	nv_write(net->dn_max_hops != DN_MAX_HOPS,
		 "%s max-hops %d", decnet, net->dn_max_hops);

	nv_write(net->dn_max_paths != 1,
		 "%s max-paths %d", decnet, net->dn_max_paths);

	nv_write(net->dn_max_visits != DN_MAX_VISITS,
                 "%s max-visits %d", decnet, net->dn_max_visits);

	reg_invoke_nv_decnet(decnet, net);
    }

   /*
    * Write the DECnet maps.  To conserve nvram we tag continuation lines
    * with "\".  We have to lag behind a line to know if it will be a
    * continuation line or not.
    */

    nv_write(TRUE, "!");

    for (n = 0; n < DN_MAXNETS; n++) {
	if ((net = dn_nets[n]) == NULL) {
	    continue;
	}

	/* Assume the maps will always need the optional integer */

	first = TRUE;
	for (i = 0; i < DNNHASH; i++) {
	    map = net->nethash[i];
	    while (map) {
		fromarea = map->fromaddr >> 10;
		fromnode = map->fromaddr & 0x3FF;
		toarea = map->toaddr >> 10;
		tonode = map->toaddr & 0x3FF;

		if (first) {
		    len = sprintf(line, "decnet %d map ", n);
		    first = FALSE;
		} else {
		    nv_write(TRUE, "%s \\", line);
		    len = 0;
		}
		len += sprintf(&line[len], "%d.%d %d %d.%d", 
			       fromarea, fromnode, map->tonetn,
			       toarea, tonode);

		/* optional cost and hops */
		if (map->cost || map->hops) {	
		    len += sprintf(&line[len], " %d %d",
				   map->cost, map->hops);
		}

		line[len] = 0;

	        map = map->next;
	    }
	}
	if (!first) {
	    nv_write(TRUE, "%s", line);
	}
    }
}

/*
 * parse_decnetaddr
 * Parse DECnet address of the form "area.node" in decimal.
 */

boolean parse_decnetaddr (
    char *buf,
    int *area,
    int *node)
{
    char *ptr, *sav_buf, c;
    ushort dn_addr;
    nametype *name;
   
    *area = 0;
    sav_buf = buf;

    if ((ptr = octet(buf, (ulong *)area, '.'))) {
	*node = 0;
	c = *ptr++;
	while ((c >= '0') && (c <= '9')) {
	    *node = *node*10 + ((int) c - '0');
	    c = *ptr++;
	}
        /* sanity checks */
	ptr = deblank(--ptr);
	if (*ptr != '\0') 
	    return(FALSE);
	else if ((*area <= 0) || (*area >= DN_MAXAREAS))
	    return(FALSE);
	else if ((*node <= 0) || (*node >= DN_MAXNODES))
	    return(FALSE);

	return(TRUE);
    }
    else {
        /* 
         * Check if a name was specified, and the name is in the name cache.
         */
        name = name_cache_lookup(sav_buf, NULL, NAM_DECNET);
        if (name) {
	    dn_addr = name->v.decnet.adr.addr.decnet_address;
	    *area = dn_addr >> 10;
	    *node = dn_addr & 0x3ff;
	    return (TRUE);
        }
    }
    return(FALSE);
}

/*
 * dnaccess_command
 * Parse a DECnet access list.
 *	[no] access-list <list> permit|deny <src> <srcmask> [<dst> <dstmask>]
 *               [EQ|NEQ <connect items>]
 */

void dnaccess_command (parseinfo *csb)
{
    int number;
    int i;
    boolean grant;
    dnacctype *list;
    dnaccitemtype *grp, *wrk_grp;

    if (csb->nvgen) {
	nv_dnaccess();
	return;
    }

    number = GETOBJ(int,1);
    grant = (boolean)GETOBJ(int,2);

    /*
     * Check if we're deleting this list
     */
    if (!csb->sense) {
	list = dn_get_acl(number);
	if (!list)
	    return;
	dn_set_acl(number, NULL);
	grp = list->normal;
	while (grp) {
	    wrk_grp = grp->next;
	    dnobjfree(grp);
	    grp = wrk_grp;
	}
	grp = list->connect;
	while (grp) {
	    wrk_grp = grp->next;
	    dnobjfree(grp);
	    grp = wrk_grp;
	}
	free(list);
	return;
    }

    grp = malloc(sizeof(dnaccitemtype));
    if (!grp) {
	return;
    }
    
    /*
     * Get the address values and masks
     */
    grp->source = GETOBJ(paddr,1)->decnet_addr;
    grp->sourcemask = GETOBJ(paddr,2)->decnet_addr;
    grp->source &= ~grp->sourcemask;
    if (GETOBJ(paddr,3)->type == ADDR_DECNET) {
	grp->dest = GETOBJ(paddr,3)->decnet_addr;
	grp->destmask = GETOBJ(paddr,4)->decnet_addr;
	grp->dest &= ~grp->destmask;
	grp->second = TRUE;
    } else {
	grp->dest = 0;
	grp->destmask = 0;
	grp->second = FALSE;
    }

    /*
     * Process connect items
     */
    if (GETOBJ(int,3)) {
	grp->flags = GETOBJ(int,3);
    }

    for (i = 0; i < 32; i++) {
	switch (GETOBJ(int,3) & 1<<i) {

	  case 0:    /* Option was not set; fall through. */
	  case DNO_EQ:
	  case DNO_ANY:
	    /* No further action;  setting the flag is all that's needed. */
	    break;

	  case DNO_AC:
	    setstring(&grp->ac, GETOBJ(string,1));
	    grp->acpattern = regcomp(GETOBJ(string,1));
	    break;

	  case DNO_DON:
 	    grp->dstop = GETOBJ(int,9);
	    grp->don = GETOBJ(int,4);
	    break;

	  case DNO_DOE:
	    setstring(&grp->doe, GETOBJ(string,2));
	    grp->dopattern = regcomp(GETOBJ(string,2));
	    break;

	  case DNO_DUIC:
	    grp->dgrp = (ushort)GETOBJ(int,5);
	    grp->dusr = (ushort)GETOBJ(int,6);
	    break;

	  case DNO_ID:
	    setstring(&grp->id, GETOBJ(string,3));
	    grp->idpattern = regcomp(GETOBJ(string,3));
	    break;

	  case DNO_PW:
	    setstring(&grp->pw, GETOBJ(string,4));
	    grp->pwpattern = regcomp(GETOBJ(string,4));
	    break;

	  case DNO_SON:
 	    grp->srcop = GETOBJ(int,10);
	    grp->son = GETOBJ(int,7);
	    break;

	  case DNO_SOE:
	    setstring(&grp->soe, GETOBJ(string,5));
	    grp->sopattern = regcomp(GETOBJ(string,5));
	    break;

	  case DNO_SUIC:
	    grp->sgrp = (ushort)GETOBJ(int,7);
	    grp->susr = (ushort)GETOBJ(int,8);
	    break;

	  default:
	    bad_parser_subcommand(csb, (GETOBJ(int,3) & (1<<i)));
	    goto badobj;
	}
    }

    /*
    * Check for illegal combinations.
    */
    if ((grp->flags & (DNO_SON|DNO_SOE)) == (DNO_SON|DNO_SOE) ||
	(grp->flags & (DNO_DON|DNO_DOE)) == (DNO_DON|DNO_DOE)) {
	printf("%% Cannot combine expression with object number");
	goto badobj;
    }
    if ((grp->flags & (DNO_SON|DNO_SUIC)) == (DNO_SON|DNO_SUIC) ||
	(grp->flags & (DNO_DON|DNO_DUIC)) == (DNO_DON|DNO_DUIC)) {
	printf("%% Cannot combine uic with object number");
	goto badobj;
    }
    /*
     * Make sure "any" not set with other qualifiers.
     */
    if ((grp->flags & DNO_ANY) && (grp->flags & ~(DNO_ANY|DNO_EQ))) {
	printf("%% \"any\" must be the only qualifier");
	goto badobj;
    }
    /*
     * "NEQ ANY" does not make sense.
     */
    if ((grp->flags & DNO_ANY) && !(grp->flags & DNO_EQ)) {
	printf("%% \"NEQ ANY\" is not a legal combination");
	goto badobj;
    }

    /*
     * Get pointer to current access lists.
     */
    list = dn_get_acl(number);
    if (!list) {
	list = malloc(sizeof(dnacctype));
	if (!list) {
	    goto badobj;
	}
	dn_set_acl(number, list);
    }
    if (grp->flags) {
	wrk_grp = list->connect;
    } else {
	wrk_grp = list->normal;
    }

    /*
     * Make sure that item is not already in list
     */
    while (wrk_grp) {
	if ((wrk_grp->source == grp->source) &&
	    (wrk_grp->sourcemask == grp->sourcemask) &&
	    (wrk_grp->dest == grp->dest) &&
	    (wrk_grp->destmask == grp->destmask) &&
	    (wrk_grp->second == grp->second) &&
	    (wrk_grp->flags == grp->flags) &&
	    (!(wrk_grp->flags&DNO_SUIC) || ((wrk_grp->sgrp == grp->sgrp) && 
					    (wrk_grp->susr == grp->susr))) &&
	    (!(wrk_grp->flags&DNO_DUIC) || ((wrk_grp->dgrp == grp->dgrp) &&
					    (wrk_grp->dusr == grp->dusr))) &&
	    (!(wrk_grp->flags&DNO_SON) || ((wrk_grp->son == grp->son) &&
					   (wrk_grp->srcop == grp->srcop))) &&
	    (!(wrk_grp->flags&DNO_DON) || ((wrk_grp->don == grp->don) &&
					   (wrk_grp->dstop == grp->dstop))) &&
	    (!(wrk_grp->flags&DNO_SOE) || !strcmp(wrk_grp->soe, grp->soe)) &&
	    (!(wrk_grp->flags&DNO_DOE) || !strcmp(wrk_grp->doe, grp->doe)) &&
	    (!(wrk_grp->flags&DNO_ID) || !strcmp(wrk_grp->id, grp->id)) &&
	    (!(wrk_grp->flags&DNO_PW) || !strcmp(wrk_grp->pw, grp->pw)) &&
	    (!(wrk_grp->flags&DNO_AC) || !strcmp(wrk_grp->ac, grp->ac))) {
	    /* Whew! :-) */
	    wrk_grp->grant = grant;	/* possibly change permit/deny */
	    dnobjfree(grp);
	    return;
	}
	if (!wrk_grp->next) {
	    break;			/* exit while pointing to last entry */
	}
	wrk_grp = wrk_grp->next;
    }
    grp->grant = grant;

    /*
     * Add new to end of list.
     */
    if (!wrk_grp) {
	if (grp->flags) {
	    list->connect = grp;
	} else{
	    list->normal = grp;
	}
    } else {
	wrk_grp->next = grp;
    }
    return;

 badobj:
    dnobjfree(grp);
    return;
}


/*
 * dnobjfree
 * Free a decnet access list entry.
 */
void dnobjfree (dnaccitemtype *grp)
{
    free(grp->soe);
    free(grp->sopattern);
    free(grp->doe);
    free(grp->dopattern);
    free(grp->id);
    free(grp->idpattern);
    free(grp->pw);
    free(grp->pwpattern);
    free(grp->ac);
    free(grp->acpattern);
    free(grp);
}

void nv_dnaccess (void)
{
    dn_showaccess(TRUE, 0);
}

/*
 * dn_showaccess
 * Routine to foramt access lists.  Will either write the access list
 * to nvram or show it with printf.
 */
void dn_showaccess (boolean nv, int acl)
{
    int i;
    dnacctype *list;
    dnaccitemtype *grp;
    void (*write)(boolean, char *, ...), (*add)(boolean, char *, ...);

    if (nv) {
	write = (void (*)(boolean, char *, ...))nv_write;
	add = (void (*)(boolean, char *, ...))nv_add;
    } else {
	write = (void (*)(boolean, char *, ...))if_printf;
	add = (void (*)(boolean, char *, ...))if_printf;
    }

    for (i = MINDECACC; i <= MAXDECACC; i++) {
	if (acl && (acl != i))
	    continue;
	list = dn_get_acl(i);
	if (!list)
	    continue;
	if (!nv) {
	    if (automore_quit())
		return;
	    printf("\nDECnet access list %d", i);
	}
	grp = list->normal;
	while (grp) {
	    if (nv) {
		(*write)(TRUE,"access-list %d %s %v %v", i,
			 grp->grant ? "permit" : "deny  ",
			 grp->source, grp->sourcemask);
	    } else {
		(*write)(TRUE, "\n    %s %v %v",
			 grp->grant ? "permit" : "deny  ",
			 grp->source, grp->sourcemask);
	    }
	    (*add)(grp->second,"  %v %v", grp->dest, grp->destmask);

	    grp = grp->next;
	}
	grp = list->connect;
	while (grp) {
	    char *opstr;
	    if (nv) {
		(*write)(TRUE,"access-list %d %s %v %v", i,
			 grp->grant ? "permit" : "deny  ",
			 grp->source, grp->sourcemask);
	    } else {
		(*write)(TRUE, "\n    %s %v %v",
		       grp->grant ? "permit" : "deny  ",
		       grp->source, grp->sourcemask);
	    }
	    (*add)(grp->second,"  %v %v", grp->dest, grp->destmask);
	    (*add)(TRUE, " %s", (grp->flags & DNO_EQ)? "eq" : "neq");
	    (*add)((grp->flags & (DNO_SON|DNO_SOE|DNO_SUIC)), " src");
	    if (grp->flags & DNO_SON) {
		opstr = dn_connect_str((ushort) grp->srcop);
		(*add)(TRUE, " %s", opstr ? opstr : "???");
		(*add)(TRUE, " %d", grp->son);
	    }
	    (*add)((grp->flags & DNO_SOE), " exp %s", grp->soe); /*  */
	    (*add)((grp->flags & DNO_SUIC), " uic [%#o,%#o]", grp->sgrp,
		   grp->susr);
	    (*add)((grp->flags & (DNO_DON|DNO_DOE|DNO_DUIC)), " dst");
	    if (grp->flags & DNO_DON) {
		opstr = dn_connect_str((ushort) grp->dstop);
		(*add)(TRUE, " %s", opstr ? opstr : "???");
		(*add)(TRUE, " %d", grp->don);
	    }
	    (*add)((grp->flags & DNO_DOE), " exp %s", grp->doe);
	    (*add)((grp->flags & DNO_DUIC), " uic [%#o,%#o]", grp->dgrp,
		   grp->dusr);
	    (*add)((grp->flags & DNO_ID), " id %s", grp->id);
	    /*
	     * Don't display account or password information when
	     * "show access".
	     */
	    (*add)((grp->flags & DNO_PW), " password %s", 
		   (nv)? grp->pw : "****");
	    (*add)((grp->flags & DNO_AC), " account %s", 
		   (nv)? grp->ac : "****");
	    (*add)((grp->flags & DNO_ANY), " any");

	    grp = grp->next;
	}
    }
}


void if_printf (boolean predicate, char *string, ...)
{
    va_list ap;

    va_start(ap, string);
    if (predicate) {
	(void) _doprnt(NULL, stdio, string, ap, FALSE);
    }
    va_end(ap);
}

/*
 * show_dnaccess
 * Show the DECnet access lists
 */

void show_dnaccess (int listnumber)
{
    dn_showaccess(FALSE, listnumber);
}

/*
 * show_dnaccess_shell
 *
 * Wrap an automore shell around show_atalkaccess so that show_access()
 * doesn't get all ruffled by our use of automore().
 */
void
show_dnaccess_shell (int list)
{
    automore_enable(NULL);
    dn_showaccess(FALSE, list);
    automore_disable();
}    
/*
 * decnet map command
 *
 * [no] decnet map <from-net> <from-address> <to-net> <to-address>
 * Continuation lines now end with "\".  Also parse "-" for compatibility
 * with old command.
 */

void decnetmap_command (parseinfo *csb, dnnet *fromnet)
{
    int tonetn, fromaddr, toaddr;
    int index;
    int farea, fnode, tarea, tnode;
    int cost, hops;
    dnmap *map, *rmap;
    dnmap **last, **rlast;
    dnnet *net;
    dnroutetype **routeptr, *thisroute;

/*
 * FIX THIS:  we currently don't handle "no decnet map" vs "no decnet 0 map"
 *            "no decnet 1 map".  
 *
 * Since we have the network number, we can fix this.  Does dn_purgemap()
 * need to take a value indicating the net to purge? - tcs 23 mar 92
 */

    if (csb->sense == FALSE && GETOBJ(paddr,1)->type != ADDR_DECNET) {
	dn_purgemap();		/* flush the whole map */
	return;
    }

/*  get the from-address */
    farea = GETOBJ(paddr,1)->decnet_addr >> 10;
    fnode = GETOBJ(paddr,1)->decnet_addr & 0x3FF;
    fromaddr = GETOBJ(paddr,1)->decnet_addr;

    tonetn = GETOBJ(int,2);
    tarea = GETOBJ(paddr,2)->decnet_addr >> 10;
    tnode = GETOBJ(paddr,2)->decnet_addr & 0x3FF;
    toaddr = GETOBJ(paddr,2)->decnet_addr;

    /* Get optional cost, hops.  If not in the input, then these values
     * will be zero, which is what we want.
     */
    cost = GETOBJ(int,3);
    hops = GETOBJ(int,4);

    net = fromnet;

/*  find hash table location */

    index = dnhash(fromaddr);
    last = &net->nethash[index];
    map = *last;
    while (map) {
	if (map->fromaddr == fromaddr) {
	    break;
	}
	last = &map->next;
	map = *last;
    }

/*  create a new map entry if not already in list */

    if (map == NULL) {
	if (csb->sense == FALSE) {
	    return;	/* if 'no' and not found */
	}
	map = malloc(sizeof(dnmap));
	if (map == NULL) {
	    printf(nomemory);
	    return;
	}
        map->next = net->nethash[index];
        net->nethash[index] = map;
    } else {
	/* find and unlink old reverse entry since toaddr may change */
	rlast = &net->reverse[dnhash(map->toaddr)];
	rmap = *rlast;
	while (rmap) {
	    if (rmap == map) {
		*rlast = rmap->reverse;		/* unlink this entry */
		break;
	    }
	    rlast = &rmap->reverse;
	    rmap = *rlast;
	}
    }

/* check for 'no' */

    if (csb->sense == FALSE) {
	*last = map->next;			/* unlink this entry */
	free(map);

	net = fromnet;
	if (farea == net->my_dn_area) {
	    routeptr = &net->dnroutes[fnode];
	} else {
	    routeptr = &net->dnareas[farea];
	}

	thisroute = *routeptr;
	while (thisroute) {
	    if (thisroute->flags & DN_RT_NET) {
		*routeptr = thisroute->nextadj;
		free(thisroute);
		break;
	    }
	    routeptr = &thisroute->nextadj;
	    thisroute = thisroute->nextadj;
	}

       /*
        * If not our area, we may have removed a route that is pointed to
        * by other map entries.  Recompute the map to put them back.
        * This could be kind of expensive, but we don't expect to 
        * be removing map entries all that often.
        */

	if (farea != net->my_dn_area) {
	    dn_maproutes(net);
	}

	return;		/* ignore continuation lines if "no" */
    }

/*  fill in the new info */

    map->fromaddr = fromaddr;
    map->toaddr = toaddr;
    map->tonetn = tonetn;
    map->cost = cost;
    map->hops = hops;
    map->use = 0;

/*  add the reverse link */

    index = dnhash(toaddr);
    map->reverse = net->reverse[index];
    net->reverse[index] = map;    

/*  add a mapped route */

    dn_addmaproute(farea, fnode, fromnet, map);

/*  check line continuation */

    /*
     * If there was a continuation character, we change the top of the
     * parse tree to parse only the portion of the map command that
     * is repeated on multiple lines.  When we process a line without
     * a continuation marker, we make sure we're pointing to the top
     * of the config parse tree.
     */

    if (GETOBJ(int,5)) {
	set_mode_byname(&csb->mode, "decnet-map", MODE_VERBOSE);
    } else {
	exit_config_submode(csb);
    }
}


/*
 * dn_purgemap - flush all the map entries.
 */
void dn_purgemap (void)
{
    dnmap *map, **mapptr;
    int i, fromarea, fromnode, n;
    dnroutetype **routeptr, *thisroute;
    dnnet *net;

/*
 * When building/purging maps, also find the hidden struct.
 */
    for (n = 0; n < DN_MAXNETS; n++) {
	if ((net = dn_netsx[n]) == NULL) {
	    continue;
	}

	for (i = 0; i < DNNHASH; i++) {
	    mapptr = &net->nethash[i];
	    map = *mapptr;
	    while (map) {
		fromarea = map->fromaddr >> 10;
		fromnode = map->fromaddr & 0x3FF;
	
		if (net) {
		    /* free the route entry */
		    if (fromarea == net->my_dn_area) {
	                routeptr = &net->dnroutes[fromnode];
		    } else {
		        routeptr = &net->dnareas[fromarea];
		    }

	            thisroute = *routeptr;
	            while (thisroute) {
	              if (thisroute->flags & DN_RT_NET) {
	                  *routeptr = thisroute->nextadj;
		          free(thisroute);
	                  break;
	              }
	              routeptr = &thisroute->nextadj;
	              thisroute = thisroute->nextadj;
	            }
		}

		/* free the map entry */
		*mapptr = map->next;
	        free(map);
                map = *mapptr;
	    }
	}

	/* clear the reverse pointers */

	for (i = 0; i < DNNHASH; i++) {
	    net->reverse[i] = NULL;
	}
    } /* for n */
}



/*
 *  show_dnmap - show the decnet map.
 */
static const char maphdr[] = "Net Node   -> Net Node    Uses     Cost Hops";

void show_dnmap (void)
{
    int i, n;
    dnnet *net;
    dnmap *map;

    automore_enable(maphdr);

/*
 * When building/purging maps, also find the hidden struct.
 */
    for (n = 0; n < DN_MAXNETS; n++) {
	if ((net = dn_netsx[n]) == NULL) {
	    continue;
	}
	for (i = 0; i < DNNHASH; i++) {
	    map = net->nethash[i];
	    while (map) {
 		printf("\n%-3d %7v   %-3d %7v %8d", n, map->fromaddr,
			map->tonetn, map->toaddr, map->use);
		if (map->hops || map->cost) {
		    printf(" %4d %d", map->cost, map->hops);
		}
	        map = map->next;
	    }
	}
    }
    automore_disable();
}

/*
 * decnet_on_idb
 *
 * Return TRUE if Decnet is enabled on the given interface
 */
boolean decnet_on_idb (idbtype *idb)
{
    if ((!dn_running) || (idb->dn_cost <= 0)) {
	return(FALSE);
    } else {
	return(TRUE);
    }
}

/*
 * decnet_clear_counters
 * Handle the 'decnet clear counters' command
 */

void decnet_clear_counters (parseinfo *csb)
{
    int i;

    for (i = 0; i < DNST_MAX; i++) {
	dn_stat[i] = 0;
    }
}

/*
 * dn_idb_is_bilingual
 * 
 * Return TRUE if the interface is bilingual, that is, iv_prime
 * addressing is enabled and we are not using a forced mac
 * address.
 */
boolean dn_idb_is_bilingual (dnnet *net, idbtype *idb)
{
    if (!net->iv_prime)			/* if not iv_prime */
	return FALSE;

    if (((ushort *)idb->hwptr->forced_mac)[0] |
	((ushort *)idb->hwptr->forced_mac)[1] |
	((ushort *)idb->hwptr->forced_mac)[2]) /* if forced */
	return FALSE;	

    return TRUE;
}

/*
 * dn_printf
 * V/v prints a dotted DECnet address.  Takes an integer argument.
 */

int dn_printf (const char **fmt, char **cp, tt_soc *tty, va_list *argp,
	       boolean rjust, int width, int size, int hash, int precision)
{
    int length, d, charcount;
    ulong n;

    charcount = 0;
    n = va_arg(*argp, ulong);
    if (*cp) {
	length = sprintf(*cp,"%d.%d",(n >> 10),
			 (n & 01777));
	*cp += length;
    } else {
	length = _ttyprintf(tty,"%d.%d",(n >> 10), (n & 01777));
    }
    charcount += length;
    if (!rjust && width != -1 && (length < width)) {
	for (d = 0; d < (width-length); d++) {
	    doprintc(cp,tty,' '); /* trailing fill - spaces*/
	    charcount++;
	}
    }
    return(charcount);
}
