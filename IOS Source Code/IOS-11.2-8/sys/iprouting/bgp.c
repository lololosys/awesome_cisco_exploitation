/* $Id: bgp.c,v 3.27.12.28 1996/09/10 01:07:26 snyder Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/bgp.c,v $
 *------------------------------------------------------------------
 * bgp.c -- Border Gateway Protocol
 *
 * 22-January-1989, Kirk Lougheed
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * Gurus: tli, pst, rchandra
 *
 * $Log: bgp.c,v $
 * Revision 3.27.12.28  1996/09/10  01:07:26  snyder
 * CSCdi68568:  more constant opportuniites
 *              140 from image, 52 from data
 * Branch: California_branch
 *
 * Revision 3.27.12.27  1996/08/28  12:51:49  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.27.12.26  1996/08/21  08:03:47  rchandra
 * CSCdi66662:  BGP: Route Reflector not enabled with peer-groups
 * Branch: California_branch
 * - set the reflector-id when peer-group members are configured
 * - in bgp_send_update do not dereference path if there is no best path
 * - all 'show ip bgp community <community>' to work in priv user mode.
 *
 * Revision 3.27.12.25  1996/08/07  09:00:36  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.27.12.24  1996/08/06  08:28:04  gchristy
 * CSCdi64974:  IP Cache: delayed invalidation can create inconsistancies
 * Branch: California_branch
 *  - Back out changes made for CSCdi55725.
 *
 * Revision 3.27.12.23  1996/07/30  06:55:59  rchandra
 * CSCdi64489:  BGP: private-AS number cannot be removed from AS-path
 * Branch: California_branch
 * - add 'neighbor x.x.x.x remove-private-AS' command.
 * - add 'inbound' keyword to 'neighbor x.x.x.x soft-reconfiguration'
 *   command (CSCdi64487)
 *
 * Revision 3.27.12.22  1996/07/29  22:41:01  rchandra
 * CSCdi64482:  BGP: cannot set MED=0 with outbound route-maps
 * Branch: California_branch
 *
 * Revision 3.27.12.21  1996/07/29  22:10:48  rchandra
 * CSCdi64477:  BGP: inbound soft reconfiguration does not work
 * Branch: California_branch
 *
 * Revision 3.27.12.20  1996/07/29  21:33:04  rchandra
 * CSCdi64437:  BGP: community display is not user-friendly
 * Branch: California_branch
 * - add user friendly community display
 * - add update throttling code (CSCdi64451)
 * - allow 'set community <additive>' during redistribution into BGP
 *
 * Revision 3.27.12.19  1996/06/28  23:18:37  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.27.12.18  1996/06/28  01:20:27  snyder
 * CSCdi61601:  fix some char * that should have been char [] in iprouting
 * Branch: California_branch
 *
 *              You get space back for the pointer
 *              You get space back for each reference
 *              You get bytes out of data space
 *
 * Revision 3.27.12.17  1996/06/24  16:05:20  myeung
 * CSCdi55725:  OSPF CPU HOG
 * Branch: California_branch
 * - Modify IP cache invalidation code to allow delayed invalidation
 *
 * Revision 3.27.12.16  1996/06/17  23:33:16  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.27.12.15  1996/06/08  19:42:27  che
 * CSCdi57850:  password encrytion not working correctly for vty
 * Branch: California_branch
 * Implement proper handling of line password structures when
 * password encryption is turned on.
 *
 * Revision 3.27.12.14  1996/06/03  23:23:47  ahh
 * CSCdi59119:  IP: NAT should support named access-lists
 * And extended access-lists and non-contiguous address pools.
 * Branch: California_branch
 *
 * Revision 3.27.12.13  1996/05/22  17:38:56  rchandra
 * CSCdi56494:  BGP: crash while adding network/route-map command
 * Branch: California_branch
 *
 * Revision 3.27.12.12  1996/05/22  17:16:08  rchandra
 * CSCdi55633:  BGP: set dampening route-map command not accepted
 * Branch: California_branch
 *
 * Revision 3.27.12.11  1996/05/22  16:52:05  rchandra
 * CSCdi53426:  Changes in routing table cause BGP to trigger update
 * Branch: California_branch
 * - if autosummarizing and majornet route is present along with subnets,
 *   pick the majornet metric.
 * - if autosummarizing and majornet route is not present, set metric to 0
 *
 * Revision 3.27.12.10  1996/05/21  09:53:04  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.27.12.9  1996/05/07  18:04:58  snyder
 * CSCdi56844:  spellink errors
 * Branch: California_branch
 *
 * neigbor -> neighbor
 * neighor -> neighbor
 *
 * Revision 3.27.12.8  1996/05/04  01:20:56  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Define MOSPF hello option bit.
 *
 * Name Access List
 *
 * Revision 3.27.12.7  1996/04/16  19:01:10  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.27.12.6  1996/04/15  14:58:22  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.27.12.5  1996/04/05  03:46:21  rchandra
 * CSCdi53755:  BGP: flap dampening settings not flexible enough
 * Branch: California_branch
 * - add route-map support to set flap dampening parameters
 *
 * Revision 3.27.12.4  1996/03/29  00:15:50  rchandra
 * CSCdi52954:  BGP :show ip bgp flap-statistics does not display net/mask
 * Branch: California_branch
 * - do not rely on path index number to print the network/mask information
 *
 * Revision 3.27.12.3  1996/03/28  23:45:37  rchandra
 * CSCdi51820:  BGP backdoor not working (IGP not preferred)
 * Branch: California_branch
 * Do proper check to find if route-map is configured along with network
 * command. This also should fix crash seen with add/delete of
 * network/route-map commands.
 *
 * Revision 3.27.12.2  1996/03/28  22:28:13  rchandra
 * CSCdi52880:  BGP: generate error message for unsupported route-map
 * options
 * Branch: California_branch
 * - generate error message when unsupported route-map option is used
 * - tread confederation EBGP neighbors similar to internal neighbors in
 *   best path selection
 * - fix the bug in bgp_compare_med, so that during MED comparison,
 *   confederation information is skipped properly
 * - confederation EBGP paths should be inserted into the IP table with
 *   internal distance
 * - do not consider common-AS neighbors as confederation neighbors
 * - during 'clear ip bgp dampening' avoid skipping paths
 * - undo the patch to the radix version walker
 *
 * Revision 3.27.12.1  1996/03/18  20:27:37  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.22.2.2  1996/03/07  09:48:58  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.22.2.1  1996/02/20  01:02:05  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.27  1996/02/12  18:18:05  abaindur
 * CSCdi48693:  show process mem does not include alternate mempool mallocs
 *
 * Revision 3.26  1996/02/06  07:52:16  rchandra
 * CSCdi46449:  BGP should sanity-check/sanitize confederation info from
 *              path attrs
 * - If confed info is present in update from non-confed neighbors,
 *   replace confed info with peers AS-number
 *
 * Revision 3.25  1996/02/01  06:05:02  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.24  1996/01/31  02:14:48  snyder
 * CSCdi47864:  spellink errors
 *              loopreceived -> loop received
 *
 * Revision 3.23  1996/01/29  00:30:23  billw
 * CSCdi13190:  Increase password characters encryted
 * Make sure that the maximum length password that can be encrypted is
 * the maximum length that will be accepted.
 *
 * Revision 3.22  1996/01/22  06:13:55  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.21  1996/01/16  03:37:32  ahh
 * CSCdi46921:  TCP: remove unused second parameter to tcp_close
 *
 * Revision 3.20  1996/01/11  02:28:59  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * . merged registry functions ip_tuba_best_local_address and
 *     ip_choose_saddr to function ip_best_local_address().
 *
 * Revision 3.19  1995/12/22  23:26:50  rchandra
 * CSCdi46050:  BGP: Alignment correction due to communities and Route
 * Reflectors
 * - in battrtype, start community and Route Reflector info at word
 * boundary
 *
 * Revision 3.18  1995/12/22  21:58:35  hampton
 * Pass pointers to timestamps instead of passing the complete timestamp
 * structure.  [CSCdi45966]
 *
 * Revision 3.17  1995/12/22  21:52:21  rchandra
 * CSCdi46047:  BGP: memory corruption when bgp confederation peers are
 * removed
 * - set bgp_confed_peers to null to avoid memory corruption
 * - order confederation peers
 * - add support to remove individual confederation peers
 * - split confederation peers command to multiple lines (25 per line)
 *
 * Revision 3.16  1995/12/17  18:30:49  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.15  1995/12/11  07:42:55  rchandra
 * CSCdi45046:  BGP: prefer external route over confederation external
 * route
 * - prefer external route over the confederation external route
 * - do not drop routes with 'no-export' community in the sub-AS boundary
 * - add 'local-AS' special community
 *
 * Revision 3.14  1995/12/02  10:11:21  pst
 * CSCdi44894:  BGP: be cool and subtle, use the hopcount metric for AS
 * path hops
 *
 * Revision 3.13  1995/11/29  22:36:23  pst
 * CSCdi44665:  BGP: put more useful metric in IP forwarding table MIB
 *
 * Revision 3.12  1995/11/25  12:44:52  rchandra
 * CSCdi43016:  BGP: need knob to selectively drop community attribute
 * - add 'set community none' route-map subcommand
 *
 * Revision 3.11  1995/11/25  12:34:55  rchandra
 * CSCdi44014:  BGP: Inbound message processing is not always fair
 * - set an upper limit on the per neighbor input processing time
 *
 * Revision 3.10  1995/11/25  12:26:31  rchandra
 * CSCdi44130:  BGP: aggregate-address with advertise-map option might not
 * work
 * - do not terminate the sub-tree walk when a less specific entry is
 *   encoutered
 *
 * Revision 3.9  1995/11/25  12:16:38  rchandra
 * CSCdi43886:  BGP: MED comparision & distance not correct with
 * confederation
 * - set the distance to be 'internal' for routes learned from EBGP
 *   confederation peers.
 * - while deciding if the MED of the paths are comparable, skip the
 *   confederation related portion of the AS path
 *
 * Revision 3.8  1995/11/25  12:03:53  rchandra
 * CSCdi43626:  BGP: peer-groups are not throttled on update generation
 * - if synchronized neighbors are present, limit the number of messages
 *   generate to BGP_WRITELIMIT
 * - do not replicate update if the neighbor has more than BGP_WRITELIMIT
 *   messages in the writeQ
 *
 * Revision 3.7  1995/11/21  20:58:43  snyder
 * CSCdi44272:  more spellink errors
 *              penaltyIncrease -> penalty. Increase
 *
 * Revision 3.6  1995/11/18  01:39:56  pst
 * CSCdi42970:  BGP: show ip bgp x y subnets doesnt check mask length
 *
 * Revision 3.5  1995/11/18  01:25:00  thille
 * CSCdi38737:  bgp4PathAttrTable requests improperly handled
 * Fix it.
 *
 * Revision 3.4  1995/11/18  00:58:54  thille
 * CSCdi38736:  BGP snmp trap responses getting lost
 * Fix and modularize the bgp trap support.
 *
 * Revision 3.3  1995/11/17  17:31:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:47:47  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:04:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.32  1995/10/25  21:35:01  rchandra
 * CSCdi42860:  BGP: nv-generation of aggregate-address causes alignment
 * fixups
 * - avoid dereferencing NULL
 * - fix a tiny memory leak in 'show ip bgp regexp'
 *
 * Revision 2.31  1995/10/25  21:09:40  rchandra
 * CSCdi42862:  BGP: show ip bgp filter-list display is wrong
 * - pass the correct parameter to the show walker function
 *
 * Revision 2.30  1995/10/20  01:29:31  pst
 * CSCdi42495:  replace broken private reverse bcopy with ANSI/POSIX
 * memmove
 *
 * Revision 2.29  1995/10/09  20:21:37  pst
 * CSCdi41527:  Remove unused macro.
 *
 * Revision 2.28  1995/10/04  22:15:10  rchandra
 * CSCdi41548:  BGP : wrong nexthop could be advertised to EBGP peers
 * - while doing the last resort brute force nexthop calculation, pass the
 *   mask to iprouting_first_hop to pick the correct entry.
 *
 * Revision 2.27  1995/10/02  22:53:58  rchandra
 * CSCdi41400:  BGP : network command could dereference null on callbacks
 *
 * Revision 2.26  1995/09/25  08:30:43  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.25  1995/09/23  21:40:30  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Reference renamed fields.  Clean up various interfaces to the IP
 * forwarding table to support protocol-specific functions.
 *
 * Revision 2.24  1995/09/19  18:37:45  smackie
 * Allow chunks to be created for a specified memory pool or with a
 * declared base alignment by allowing the data to be detached from
 * the chunk header and freelist. (CSCdi40637)
 *
 * Revision 2.23  1995/09/11  23:50:05  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.22  1995/08/29 02:34:46  rchandra
 * CSCdi39464:  BGP : crash on setting community on the inbound update
 * - attrinfo structure should always point to the global community buffer
 *
 * Revision 2.21  1995/08/28  21:14:30  rchandra
 * CSCdi39436:  BGP : Allow conditional unsuppression of suppressed routes
 * - add 'neighbor x.x.x.x unsuppress-map <map>' command
 *
 * Revision 2.20  1995/08/26  00:14:04  rchandra
 * CSCdi39369:  BGP: show ip bgp community must display route with any
 * community
 * - also make the internet community value in community-list to NVGEN.
 *
 * Revision 2.19  1995/08/09  23:43:50  rchandra
 * CSCdi38510:  BGP : With templates, manually conf. neighbors not nv-gened
 * - clear the BN_TEMPORARY flag when the remote-as is configured
 *
 * Revision 2.18  1995/08/04  06:43:44  hampton
 * sched_private.h included too many places  [CSCdi38246]
 *
 * Revision 2.17  1995/07/27 18:52:43  rchandra
 * CSCdi37775:  BGP : crash while doing show ip bgp summary
 * - Null the next pointer before freeing a neighbor structure.
 *
 * Revision 2.16  1995/07/12  17:52:12  rchandra
 * CSCdi36969:  BGP : Add knob to disable client to client reflection
 * - 'bgp client-to-client reflection' command added
 * - when access-list change callback is made, instead of cleaning the
 *   cache immediately, flag the cache for purging by bgp_router.
 *
 * Revision 2.15  1995/07/07  21:52:15  rchandra
 * CSCdi36806:  BGP : Spurious memory access in bgp_nv_neighbor
 *
 * Revision 2.14  1995/07/06  23:12:17  rchandra
 * CSCdi36759:  BGP : Add conditional per neighbor default generation
 * - add 'neighbor <x.x.x.x> default-originate route-map <tag>' command.
 *
 * Revision 2.13  1995/07/06  10:39:30  rchandra
 * CSCdi36761:  BGP : aggregate not generated with advertise-map (again)
 *  - check the dupekey value for NULL before walking thru the dupe chain.
 *
 * Revision 2.12  1995/07/06  06:16:36  rchandra
 * CSCdi36737:  BGP : show ip protocol information is corrupted
 *
 * Revision 2.11  1995/06/30  20:25:03  rchandra
 * CSCdi36620:  BGP requires IBGP neighbors to be fully meshed
 *
 * Revision 2.10  1995/06/28  20:58:08  rchandra
 * CSCdi34539:  BGP : Do not allow local system to be configured as
 * neighbor
 *
 * Revision 2.9  1995/06/28  09:27:38  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.8  1995/06/28  03:20:53  hampton
 * Use the passive timer macros instead of direct references to the system
 * clock.  [CSCdi36502]
 *
 * Revision 2.7  1995/06/19 06:36:36  rchandra
 * CSCdi36056:  BGP doesnt check malloc return code
 *
 * Revision 2.6  1995/06/19  01:07:16  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.5  1995/06/18  06:18:31  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.4  1995/06/17 06:25:52  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.3  1995/06/14  20:33:05  rchandra
 * CSCdi35219:  BGP : aggregate not generated with the advertise-map
 * - when using advertise-map, generate the path string from as path of the
 *   more specific routes.
 *
 * Revision 2.2  1995/06/09  13:06:01  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:05:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * How conditional-advertisement is implemented:
 *
 * All condition-map is evaluated periodically in bgp_nettable_scan() and a
 * flag in the map (camap->cond_change) and a global flag
 * (bgp_advertise_cond_change) is marked if there is a change for a map and
 * for all the condition-maps respectively.
 *
 * The adv_state is marked with four states: UNITIALIZED, ADVERTISE,
 * NOT_ADVERTISE, and DECONFIGURED (for map that user has removed but we
 * haven't serviced the prefixes that might have been conditionally
 * suppressed).
 *
 * Then, in bgp_scan_walker(), we walk the BGP table and bump the version
 * number of prefixes that should be updated.  If no condition-map has changed
 * state, we will not bump the version of any prefix.  If any condition-map
 * has changed state, we check that the prefix matches the corresponding
 * advertise-map or not.  If so, the prefix's version number is bumped.
 *
 * Note that a prefix that has been newly received and should be advertised
 * will have a new version number (higher than any peer's version no.)
 * already.  So, it will be looked at when we walk the BGP table trying to
 * send updates.
 *
 * Update sending:
 *
 * Update is sent by traversing the BGP table looking for prefixes that has a
 * higher version number than the receiving peer.  We check the advertise-map
 * in bgp_update_policy() to see if a prefix matches the receiving peer's
 * advertise-map.  If so, then depending on the state of the condition-map, we
 * advertise or withdraw the prefix.
 *
 * We have to always do this on the outgoing side because a new prefix can
 * come in and matches the advertise-map of a peer while the condition-map
 * state is not changing.
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <stdarg.h>
#include <ciscolib.h>
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "login.h"
#include "config.h"
#include "logger.h"
#include "../iprouting/msg_bgp.c"	/* Not a typo, see logger.h */
#include "access.h"
#include "parser.h"
#include "exception.h"
#include "bitlogic.h"
#include "sys_registry.h"
#include "../ip/ip_registry.h"
#include "../ui/debug.h"
#include "../parser/parser_actions.h"
#include "parser_defs_router.h"
#include "../os/free.h"
#include "../util/regexp.h"
#include "../util/sorted_array.h"
#include "../if/network.h"
#include "../ip/ip_registry.h"
#include "../ip/ip.h"
#include "../ip/ipaccess.h"
#include "../ip/ipaccess_private.h"
#include "../tcp/tcp.h"
#include "route.h"
#include "route_inlines.h"
#include "neighbor.h"
#include "community.h"
#include "ospf_pak.h"
#include "bgp.h"
#include "route_map.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "iprouting_debug.h"
#include "bgp_private.h"
#include "../routing/routing_registry.h"

#define	BGP_PRIVATE static
#define	BGP_PUBLIC extern

/*
 * These external declaration must die for the sake of modularity.
 */
extern regexpacctype *as_path_accesslists[];

/*
 * Forward declarations
 * Each one of these is an admission of design failure.  If you add
 * to this list, I will come after you with a stick.	-- pst 20oct93
 */

BGP_PRIVATE void bgp_reset (neighbortype *bgp, const char *reason);


/*
 * Local Storage
 */
pdbtype *pdb_BGP;
BGP_PRIVATE rnhtype *bgp_head;
BGP_PRIVATE battrtype **bgppathtable;
BGP_PRIVATE bmsgtype **bgpupdatetable;
BGP_PRIVATE bgpgateway **bgp_gwcache;
BGP_PRIVATE bgp_route_map_ctype **bgp_route_map_cache;
BGP_PRIVATE bgp_filter_list_ctype **bgp_filter_list_cache;
BGP_PRIVATE ulong bgptable_version;
BGP_PRIVATE ulong bgp_aggregate_version;
BGP_PRIVATE ulong bgp_nettable_version;
BGP_PRIVATE int bgp_iopid;
BGP_PRIVATE int bgp_scanpid;
BGP_PRIVATE bmsgtype *bgp_emergency_msg;
BGP_PRIVATE char *bgp_router_path_buffer;
BGP_PRIVATE char *bgp_rcvd_aspath_buffer;
BGP_PRIVATE char *bgp_set_aspath_buffer;
BGP_PRIVATE battrtype *bgp_router_attr_buffer;
BGP_PRIVATE ulong *bgp_router_com_buffer;
BGP_PRIVATE battrtype *bgp_edit_attr_buffer;
BGP_PRIVATE ulong *bgp_edit_com_buffer;
BGP_PRIVATE rrinfotype *bgp_router_rrinfo_buffer;
BGP_PRIVATE queuetype bgp_redistQ;
BGP_PRIVATE queuetype bgp_requestQ;
BGP_PRIVATE queuetype bgp_resetQ;
BGP_PRIVATE queuetype bgp_aggregateQ;
BGP_PRIVATE queuetype bgp_backdoorQ;
BGP_PRIVATE boolean bgp_fast_reset = TRUE;
BGP_PRIVATE boolean bgp_redistribute_ibgp;
BGP_PRIVATE boolean bgp_always_compare_med = FALSE;
BGP_PRIVATE boolean bgp_read_queues_nonempty = FALSE;
BGP_PRIVATE ipaddrtype bgp_router_id_cfg;
BGP_PRIVATE const char badbgpneighbor[] = "Bad BGP neighbor argument";
BGP_PRIVATE ushort *bgp_confed_peers;
BGP_PRIVATE uchar bgp_confed_peer_count;
BGP_PRIVATE ushort bgp_confed_id;
BGP_PRIVATE chunk_type *rcache_chunks;
BGP_PRIVATE chunk_type *fcache_chunks;
BGP_PRIVATE chunk_type *damp_chunks;
BGP_PRIVATE ushort bgp_cfg_reuse_penalty;
BGP_PRIVATE ushort bgp_cfg_suppress_penalty;
BGP_PRIVATE ulong bgp_cfg_halflife_time;			/* in ticks */
BGP_PRIVATE ulong bgp_cfg_maximum_stime;			/* in ticks */
BGP_PRIVATE boolean bgp_dampening_enabled;
BGP_PRIVATE boolean bgp_dampening_cfg_changed;
BGP_PRIVATE dampctrlblk *bgp_normal_dcb;
BGP_PRIVATE dampctrlblk *bgp_routemap_dcb;
BGP_PRIVATE route_map_nametype *bgp_dampening_map;
BGP_PRIVATE dampctrlblk **bgp_dcb_array;
BGP_PRIVATE bdampinfotype **bgp_reuse_array;
BGP_PRIVATE ushort bgp_reuse_index;
BGP_PRIVATE ushort bgp_reuse_array_len;
BGP_PRIVATE boolean bgp_purge_cache;
BGP_PRIVATE ulong bgp_cluster_id;
BGP_PRIVATE boolean bgp_client_reflection;
BGP_PRIVATE boolean bgp_dpa_enabled;
BGP_PRIVATE boolean bgp_check_soft_reconfig;
BGP_PRIVATE boolean bgp_multipath_enabled;
BGP_PRIVATE uchar *bgp_neighbor_bitfield;	/* field to track EBGP peers*/
BGP_PRIVATE uchar bgp_bitfield_count;		/* # of tail bits in net */
BGP_PRIVATE uchar bgp_bitfield_size;		/* size of tail bitfield */
BGP_PRIVATE sys_timestamp bgp_neighbor_index_timer;
BGP_PRIVATE boolean bgp_determ_med;
BGP_PRIVATE boolean bgp_advertise_cond_change;
BGP_PRIVATE ulong bgpaccepters_current, bgpaccepters_max;
/* make available for snmp access */
ulong bgp_default_localpref = BGP_DEFAULT_LOCALPREF;

/*
 * The following table has set/match information for BGP route-map
 * users. It is used for sanity checking route-map usage.
 */
static const route_map_userinfo bgp_route_map_users[] = {
    {ROUTE_MAP_BGP_INBOUND, ROUTE_MAP_BGP_INBOUND_SET, 
	 ROUTE_MAP_BGP_INBOUND_MATCH, "BGP inbound"},
    {ROUTE_MAP_BGP_OUTBOUND, ROUTE_MAP_BGP_OUTBOUND_SET, 
	 ROUTE_MAP_BGP_OUTBOUND_MATCH, "BGP outbound"},
    {ROUTE_MAP_BGP_ATTRIBUTE, ROUTE_MAP_BGP_ATTRIBUTE_SET,
	 ROUTE_MAP_BGP_ATTRIBUTE_MATCH, "BGP attribute"},
    {ROUTE_MAP_BGP_ADVERTISE, ROUTE_MAP_BGP_ADVERTISE_SET,
	 ROUTE_MAP_BGP_ADVERTISE_MATCH, "BGP advertise"},
    {ROUTE_MAP_BGP_SUPPRESS, ROUTE_MAP_BGP_SUPPRESS_SET,
	 ROUTE_MAP_BGP_SUPPRESS_MATCH, "BGP suppress"},
    {ROUTE_MAP_BGP_UNSUPPRESS, ROUTE_MAP_BGP_UNSUPPRESS_SET,
	 ROUTE_MAP_BGP_UNSUPPRESS_MATCH, "BGP unsuppress"},
    {ROUTE_MAP_BGP_COND_ADVERTISE, ROUTE_MAP_BGP_COND_ADVERTISE_SET,
	 ROUTE_MAP_BGP_COND_ADVERTISE_MATCH, "BGP advertise-condition"},
    {ROUTE_MAP_BGP_CONDITION, ROUTE_MAP_BGP_CONDITION_SET,
	 ROUTE_MAP_BGP_CONDITION_MATCH, "BGP condition"},
    {0, 0, 0, NULL}			/* should always be the last element */
};

/*
 * String for conditional advertisement state
 */
static const char *bgp_adv_state_strings[] = {
    "Uninitialized",
    "Advertise",
    "Withdraw",
    "Deconfigured"
};

/*
 * Local treewalking return code definitions.
 */
#define	WALK_CONTINUE	(0)
#define	WALK_ABORT	(-1)


/*
 * Decay rate for range of half-life time for every decay tick with
 * five digit precision.
 *		 _				     _
 *		|     	   1			      |
 *		| ---------------------   * log (1/2) |
 *		|  (halflife_time/ticks)	      |
 *		|_				     _|	
 *             e
 *
 * range --> 1 to 45 minutes
 * ticks --> 5 seconds
 */
BGP_PRIVATE const ulong
bgp_decayrate_table[] = {
    0,
    94387, 97153, 98093, 98566, 98851, 
    99041, 99178, 99280, 99360, 99424, 
    99476, 99519, 99556, 99588, 99615, 
    99639, 99660, 99679, 99696, 99711, 
    99725, 99737, 99749, 99759, 99769, 
    99778, 99786, 99793, 99801, 99807, 
    99813, 99819, 99825, 99830, 99835, 
    99839, 99844, 99848, 99852, 99855,
    99859, 99862, 99865, 99868, 99871 };

/*
 * BGP4 prefix maniplation routines and tables
 * to convert from <pfx>+<variable-data> into <addr>/<mask> and back.
 */

#define	PREFIX2BYTES(P)	(bgp_prefix2bytes_table[(P)])
#define	PREFIX2MASK(P)	(bgp_prefix2mask_table[(P)])

BGP_PRIVATE const ipaddrtype
bgp_prefix2mask_table[] = {
    0x00000000,
    0x80000000, 0xc0000000, 0xe0000000, 0xf0000000,
    0xf8000000, 0xfc000000, 0xfe000000, 0xff000000,
    0xff800000, 0xffc00000, 0xffe00000, 0xfff00000,
    0xfff80000, 0xfffc0000, 0xfffe0000, 0xffff0000,
    0xffff8000, 0xffffc000, 0xffffe000, 0xfffff000,
    0xfffff800, 0xfffffc00, 0xfffffe00, 0xffffff00,
    0xffffff80, 0xffffffc0, 0xffffffe0, 0xfffffff0,
    0xfffffff8, 0xfffffffc, 0xfffffffe, 0xffffffff };

BGP_PRIVATE const int
bgp_prefix2bytes_table[] = {
    0,
    1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4 };


BGP_PRIVATE int
bgp_writeprefix (uchar *data, ipaddrtype net, ipaddrtype mask)
{
    uchar *np;
    int i, bytes, prefix;

    net &= mask;
    np = (uchar *) &net;

    prefix = ip_bitsinmask(mask);
    *data++ = prefix;

    bytes = i = PREFIX2BYTES(prefix);
    while (i--)
	*data++ = *np++;

    return (bytes + 1);
}

BGP_PRIVATE int
bgp_readprefix (uchar *data, ipaddrtype *net, ipaddrtype *mask)
{
    int prefix, entry_size, i;
    uchar *np = (uchar *) net;

    prefix = *data++;
    if (prefix > sizeof(ipaddrtype)<<3)
	return (0);

    entry_size = PREFIX2BYTES(prefix);

    *mask = PREFIX2MASK(prefix);

    for (i = 0; i < entry_size; i++)
	*np++ = *data++;

    *net &= *mask;

    return (entry_size + 1);
}

/*
 * bgp_printmsg
 * Format and print a given BGP packet as a buginf
 */

BGP_PRIVATE void
bgp_printmsg (char *label, uchar *data, int length)
{
    static char buffer[256];
    char *p;

    p = buffer;
    p += sprintf(p, "\nBGP %s (%d): ", label, length);

    for ( ; length > 3 && p - buffer < sizeof(buffer) - 11; length -= 4) {
	if ((p - buffer) > (sizeof(buffer) - 11))
	    break;
	p += sprintf(p, "%08x.", *((ulong *)data)++);
    }
    for (; length > 0 && p - buffer < sizeof(buffer) - 3; length--)
	p += sprintf(p, "%02x", *data++);

    buginf(buffer);
}

/*
 * bgp_getbuffer
 * Get a buffer for BGP, waiting if necessary
 * Initialize header.
 */

BGP_PRIVATE bmsgtype *
bgp_getbuffer (int bytes, int typecode, boolean block)
{
    bgp_header *header;
    bmsgtype *msg;

    while (TRUE) {
	msg = malloc(sizeof(bmsgtype) + bytes);
	if (!msg) {
	    if (!block)
		return (NULL);
  	    process_sleep_for(ONESEC/2);
  	    continue;
  	}
	msg->datagramsize = bytes;
	msg->datagramstart = (uchar *) msg + sizeof(bmsgtype);
	header = (bgp_header *) msg->datagramstart;
  	memset(header, 0xff, BGP_MARKERBYTES);
  	PUTSHORT(&header->len[0], bytes);
  	header->type = typecode;
	msg->dataptr = (uchar *) msg->datagramstart + BGP_HEADERBYTES;
	return (msg);
    }
}

/*
 * bgp_free_writeQ
 * Free the messages in a writeQ.
 * Note : msg could be pointing to the data part of another message.
 *        msgheadptr is the pointer to the other message.
 */
BGP_PRIVATE void
bgp_free_writeQ(queuetype *qptr)
{
    bmsgtype *msg;
    while (!QUEUEEMPTY(qptr)) {
	msg = dequeue(qptr);
	if (msg) {
	    free(msg->msgheadptr);
	    free(msg);
	}
    }
}

/*
 * bgp_reset_cache
 * Utility function to reset a cache.
 */
BGP_PRIVATE void
bgp_reset_cache (void **cache, chunk_type *chunks, int hashlen)
{
    int i;
    bgp_nexthelper *p, *p_next;

    for (i = 0; i < hashlen; i++) {
	p = cache[i];
	cache[i] = NULL;
	while (p) {
	    p_next = p->next;
	    chunk_free(chunks, p);
	    p = p_next;
	}
	process_may_suspend();
    }
}
/*
 * bgp_reset_rcache
 */
BGP_PRIVATE void
bgp_reset_rcache (void) 
{
    bgp_reset_cache((void **)bgp_route_map_cache, rcache_chunks, NETHASHLEN);
}
/*
 * bgp_reset_fcache
 */
BGP_PRIVATE void
bgp_reset_fcache (void) 
{
    bgp_reset_cache((void **)bgp_filter_list_cache, fcache_chunks, NETHASHLEN);
}

BGP_PRIVATE bgp_cond_adv_maptype *
bgp_find_cond_adv_map (neighbortype *bgp, char *adv_map, char *cond_map)
{
    bgp_cond_adv_maptype *map;

    for (map = bgp->bgp_cond_adv; map; map = map->next) {
	if (!strcmp(map->bgp_advertise_map->mapname, adv_map) &&
	    !strcmp(map->bgp_condition_map->mapname, cond_map))
	    return(map);
    }

    return(NULL);
}

/* bgp_link_camap
 * Link new conditional advertisement map to the tail of the
 * neighbor's list and the main pdb_BGP's list.
 */
BGP_PRIVATE void
bgp_link_camap (neighbortype *bgp, bgp_cond_adv_maptype *camap)
{
    bgp_cond_adv_maptype *map;

    camap->next = NULL;
    camap->link_next = NULL;
    camap->nbr = bgp;

    if (!bgp->bgp_cond_adv)
	bgp->bgp_cond_adv = camap;
    else {
	for (map = bgp->bgp_cond_adv; map; map = map->next) {
	    if (!map->next) {
		map->next = camap;
		break;
	    }
	}
    }

    if (!pdb_BGP->bgp_cond_adv)
	pdb_BGP->bgp_cond_adv = camap;
    else {
	for (map = pdb_BGP->bgp_cond_adv; map; map = map->link_next) {
	    if (!map->link_next) {
		map->link_next = camap;
		break;
	    }
	}
    }
}

BGP_PRIVATE void
bgp_delink_camap (bgp_cond_adv_maptype *camap)
{
    bgp_cond_adv_maptype *map;
    neighbortype *bgp;

    bgp = camap->nbr;

    if (camap == bgp->bgp_cond_adv)
	bgp->bgp_cond_adv = camap->next;
    else {
	for (map = bgp->bgp_cond_adv; map; map = map->next) {
	    if (map->next == camap) {
		map->next = camap->next;
		break;
	    }
	}
    }

    if (camap == pdb_BGP->bgp_cond_adv)
	pdb_BGP->bgp_cond_adv = camap->link_next;
    else {
	for (map = pdb_BGP->bgp_cond_adv; map; map = map->link_next) {
	    if (map->link_next == camap) {
		map->link_next = camap->link_next;
		break;
	    }
	}
    }

    camap->next = NULL;
    camap->link_next = NULL;
    camap->nbr = NULL;
    free(camap);
}

/*
 * bgp_statestring
 * Given a state code, return the state that we're in.
 */

BGP_PRIVATE char *
bgp_statestring (int state)
{
    switch (state) {
    case BGPS_IDLE: return ("Idle");
    case BGPS_CONNECT: return ("Connect");
    case BGPS_ACTIVE: return ("Active");
    case BGPS_OPENSENT: return ("OpenSent");
    case BGPS_OPENCONFIRM: return ("OpenConfirm");
    case BGPS_ESTAB: return ("Established");
    case BGPS_CLOSING: return ("Closing");
    default: return ("?");
    }
}

/*
 * bgp_statechange
 * Transition from one FSM state to another
 */

BGP_PRIVATE void
bgp_statechange (neighbortype *bgp, int newstate)
{
    int oldstate;

    if (bgpevent_debug && bgp->bgp_state != newstate)
	buginf("\nBGP: %i went from %s to %s", bgp->address,
	       bgp_statestring(bgp->bgp_state), bgp_statestring(newstate));
    oldstate = bgp->bgp_state;
    bgp->bgp_state = newstate;
    if ((newstate == BGPS_ESTAB) && (oldstate != BGPS_ESTAB)) {
	bgp->cameup++;
	GET_TIMESTAMP(bgp->started);
	reg_invoke_ip_bgp_trap(BGP_TRAP_ESTAB, bgp->address,
			       bgp->bgp_last_error, newstate);
    } else if ((oldstate == BGPS_ESTAB) && (newstate != BGPS_ESTAB)) {
	bgp->wentdn++;
	GET_TIMESTAMP(bgp->started);
	if (bgp->bgp_configed_version)	/* reset version negotiation */
	    bgp->protocol_version = bgp->bgp_configed_version;
	else
	    bgp->protocol_version = BGP_VERSION;
    }
    if (newstate < oldstate)
	reg_invoke_ip_bgp_trap(BGP_TRAP_BACKTRANS, bgp->address,
			       bgp->bgp_last_error, newstate);
}

/*
 * bgp_close
 * Close a BGP TCP connection, clean up data structures
 */

BGP_PRIVATE void
bgp_close (neighbortype *bgp)
{
    tcbtype *tcbtemp;

    /*
     * Kill off the active open process if we're trying to establish a
     * connection.
     */
    if ((bgp->bgp_state == BGPS_ACTIVE) && (bgp->open_pid != -1))
	process_kill(bgp->open_pid);

    bgp_statechange(bgp, BGPS_IDLE);
    if (bgp->tcb) {
	if (bgp_debug)
	    buginf("\nBGP: %i closing", bgp->address);
	/*
	 * tcp_close doesn't block anymore, but better safe than sorry.
	 * If it did block and we try to close again while it's
	 * blocked, then the tcb is deallocated twice.  If we mark our
	 * copy of the tcb as gone away already, then we'll really
	 * consider it closed.
	 */
	tcbtemp = bgp->tcb;
  	bgp->tcb = NULL;
	tcbtemp->flags |= TCB_RTRANSTMO;	/* allow TCP to give up */
 	tcp_close(tcbtemp);
    }
    if (bgp->secondary_tcb) {
	tcbtemp = bgp->secondary_tcb;
	bgp->secondary_tcb = NULL;
	tcbtemp->flags |= TCB_RTRANSTMO;	/* allow TCP to give up */
	tcp_close(tcbtemp);
    }
    bgp->version = 0;
    bgp->bgp_upd_version = 0;
    bgp->bgp_upd_checkpoint = 0;
    bgp->flags &= ~BN_RESETCLEAR_FLAGS;
    bgp->bgp_router_id = 0;
    bgp->bgp_holdtime = pdb_BGP->bgp_holdtime*ONESEC;
    bgp->bgp_probetime = pdb_BGP->bgp_keeptime*ONESEC;
    GET_TIMESTAMP(bgp->bgp_lastread);
    GET_TIMESTAMP(bgp->bgp_ktimer);
    TIMER_START(bgp->bgp_last_upd_sent, 0);/* start out with timer expired */
    GET_TIMESTAMP(bgp->bgp_last_upd_check);
    bgp->readstate = RD_START;
    bgp->bgp_netcount = 0;
    bgp->open_pid = -1;
    free(bgp->bgp_netpak);
    bgp->bgp_netpak = NULL;
    bgp_free_writeQ(&bgp->bgp_writeQ);
    bgp_free_writeQ(&bgp->bgp_secondary_writeQ);
    while (!QUEUEEMPTY(&bgp->bgp_readQ))
	free(dequeue(&bgp->bgp_readQ));
}

/*
 * bgp_send_open
 * Write an open messsage.
 */

BGP_PRIVATE void
bgp_send_open (neighbortype *bgp, boolean primary)
{
    bmsgtype *msg;
    int bytes;
    bgp3_open *open3;
    bgp2_open *open2;
    ushort my_as;

    if (bgp->bgp_configed_version)
	bgp->protocol_version = bgp->bgp_configed_version;

    if (bgp_confed_id &&
	!(bgp->yoursystem == pdb_BGP->asystem || (bgp->flags & BN_COMMONADMIN)))
	my_as = bgp_confed_id;
    else
	my_as = pdb_BGP->asystem;

    switch (bgp->protocol_version) {
    case 2:
	bytes = BGP_HEADERBYTES + BGP_MIN_OPEN2;
	msg = bgp_getbuffer(bytes, BGP_TYPE_OPEN, TRUE);
	open2 = (bgp2_open *) msg->dataptr;
	open2->version = 2;
	PUTSHORT(&open2->my_as, my_as);
	PUTSHORT(&open2->holdtime, pdb_BGP->bgp_holdtime);
	open2->authcode = BGP_AUTH_NONE;
	break;
    case 3:
    case 4:
    default:
	bytes = BGP_HEADERBYTES + BGP_MIN_OPEN3;
	msg = bgp_getbuffer(bytes, BGP_TYPE_OPEN, TRUE);
	open3 = (bgp3_open *) msg->dataptr;
	open3->version = (bgp->protocol_version == 3 ? 3 : 4);
	PUTLONG(&open3->identifier, pdb_BGP->bgp_rtr_id);
	PUTSHORT(&open3->my_as, my_as);
	PUTSHORT(&open3->holdtime, pdb_BGP->bgp_holdtime);
	open3->authcode = BGP_AUTH_NONE;
	break;
    }
    if (primary)
	enqueue(&bgp->bgp_writeQ, msg);
    else
	enqueue(&bgp->bgp_secondary_writeQ, msg);
    GET_TIMESTAMP(bgp->bgp_lastread);
    GET_TIMESTAMP(bgp->bgp_ktimer);
    TIMER_START(bgp->bgp_last_upd_sent, 0);/* start out with timer expired */
    GET_TIMESTAMP(bgp->bgp_last_upd_check);
    bgp_statechange(bgp, BGPS_OPENSENT);
    if (bgp_debug) 
	buginf("\nBGP: %i sending OPEN, version %d", bgp->address,
	       bgp->protocol_version); 
}

/*
 * bgp_english_notification
 * Display english description of notification
 */

BGP_PRIVATE const char *
bgp_english_notification (int code, int subcode)
{
    static const char *const headers[] = {
	"header: unknown subcode",
	"header synchronization problems",
	"illegal header length",
	"unsupported message type",
    };
    static const char *const opens[] = {
	"open: unknown subcode",
	"incompatible BGP version",
	"peer in wrong AS",
	"BGP identifier wrong",
	"unsupported authentication type",
	"authentication failure"
    };
    static const char *const updates[] = {
	"update: unknown subcode",
	"update malformed",
	"update unrecognized",
	"update missing required attributes",
	"invalid flags for attribute",
	"invalid length for attribute",
	"invalid origin code",
	"as path loop detected",
	"invalid next hop specified",
	"unsupported option specified",
	"illegal network",
	"invalid or corrupt AS path"
    };
    static const struct {
	char *msg;
	const char *const *submsg;
	int submsgcnt;
    } msgs[] = {
	{ "notification code unknown", NULL, 0 },
	{ "header", headers, sizeof (headers) / sizeof (char *) },
	{ "open", opens, sizeof (opens) / sizeof (char *) },
	{ "update", updates, sizeof (updates) / sizeof (char *) },
	{ "hold time expired", NULL, 0 },
	{ "finite state machine error", NULL, 0 },
	{ "cease",  NULL, 0 }
    };

    if (code > BNOT_CEASE)
	code = 0;

    if (msgs[code].submsg) {
	if (subcode > msgs[code].submsgcnt)
	    subcode = 0;
	return(msgs[code].submsg[subcode]);
    }

    return(msgs[code].msg);
}

/*
 * bgp_notification_data
 * Convert received data to ascii hex string
 */

BGP_PRIVATE void
bgp_notification_data (uchar *data, int count, char *buffer, int bufsize)
{
    int len, i;

    if (data) {
	i = 0;
	while (count--) {
	    if (bufsize < 4)
		break;
	    len = sprintf(buffer, "%02x%s", *data++,
			  ((i++ % 4) == 3) ? " " : "");
	    buffer += len;
	    bufsize -= len;
	}
    }
    *buffer = '\0';
}

/*
 * bgp_send_notification
 * Compose a notification messsage.
 */

BGP_PRIVATE void
bgp_send_notification (neighbortype *bgp, int code, int subcode,
		       uchar *dataptr, int datacount)
{
    bmsgtype *msg;
    int bytes, i;
    uchar *data, *p;
    char buffer[40];

    if (bgp->bgp_state == BGPS_CLOSING)
	return;

    bgp->bgp_last_error[0] = code;		/* store for SNMP */
    bgp->bgp_last_error[1] = subcode;

    bgp->errssent++;
    bytes = BGP_HEADERBYTES + 2 + datacount;
    msg = bgp_getbuffer(bytes, BGP_TYPE_NOTIFICATION, TRUE);
    data = (uchar *) msg->dataptr;
    *data++ = code;
    *data++ = subcode;

    if (dataptr) {
	for (p = dataptr, i = datacount; i--; )
	    *data++ = *p++;
    }

    /*
     * Flush preceding messages, then enqueue our final message and set
     * the connection to go away after writing that final message.
     */
    bgp_free_writeQ(&bgp->bgp_writeQ);
    enqueue(&bgp->bgp_writeQ, msg);
    bgp_statechange(bgp, BGPS_CLOSING);
    GET_TIMESTAMP(bgp->bgp_lastread);
    if (bgp_debug) {
	bgp_notification_data(dataptr, datacount, buffer, sizeof(buffer));
	buginf("\nBGP: %i sending NOTIFICATION %d/%d (%s) %d bytes %s",
	       bgp->address, code, subcode,
	       bgp_english_notification(code, subcode),
	       datacount, buffer);
    }
}

/*
 * bgp_rcv_notification
 * Receive a notification message and print it out
 */

BGP_PRIVATE boolean
bgp_rcv_notification (neighbortype *bgp, uchar *data, int bytes)
{
    uchar code, subcode;
    ushort ver;
    char buffer[40];

    code = *data++;
    subcode = *data++;
    bytes -= 2;

    if (bgp_debug) {
	bgp_notification_data(data, bytes, buffer, sizeof(buffer));
	buginf("\nBGP: %i received NOTIFICATION %d/%d (%s) %d bytes %s",
	      bgp->address, code, subcode,
	      bgp_english_notification(code, subcode), bytes, buffer);
    }

    if ((code == BNOT_OPEN) && (subcode == BNOT_OPEN_VERSION)) {
	ver = GETLONG(data) >> 16;
	if (!bgp->bgp_configed_version) {
	    /*
	     * remote system has reported back his highest supported version
	     * of BGP.
	     *
	     * If he can go higher than us, then we should try to go as high
	     * as we can.
	     *
	     * If his highest is lower than our lowest, then we're screwed
	     * anyway, so just fall back to our default and keep trying until
	     * he does a software upgrade.
	     *
	     * Otherwise, he has told us what his highest BGP version is, so
	     * let's use that the next time we do our open.
	     */
	    if (ver > BGP_MAX_VERSION)
		bgp->protocol_version = BGP_MAX_VERSION;
	    else if (ver < BGP_MIN_VERSION)
		bgp->protocol_version = BGP_VERSION;
	    else 
		bgp->protocol_version = ver;
	}
	return (TRUE);
    }
    return (FALSE);
}

/*
 * bgp_send_keepalive
 * Send a keepalive message.
 */

BGP_PRIVATE void
bgp_send_keepalive (neighbortype *bgp)
{
    bmsgtype *msg;

    msg = bgp_getbuffer(BGP_HEADERBYTES, BGP_TYPE_KEEPALIVE, FALSE);
    if (!msg) {
	errmsg(&msgsym(NOMEMORY, BGP), "keepalive");
	return;
    }
    enqueue(&bgp->bgp_writeQ, msg);
    if (bgpkeep_debug)
	buginf("\nBGP: %i sending KEEPALIVE", bgp->address);
}

/*
 * bgp_rcv_open
 * Handle a received OPEN message
 */

BGP_PRIVATE void
bgp_rcv_open (neighbortype *bgp, uchar *data, int bytes)
{
    ushort yoursystem;
    bgp2_open *open2;
    bgp3_open *open3;
    int authcode, minsize, holdtime;
    ushort version;
    tcbtype *tcbtemp;
    boolean versionok;
    char code, *opt, *optstart;
    int optlen, len, clen;

    open3 = (bgp3_open *)data;
    version = open3->version;
    authcode = 0;
    minsize = 0;
    holdtime = 0;
    optlen = 0;
    optstart = NULL;
    versionok = FALSE;
    if (bgp_debug)
	buginf("\nBGP: %i OPEN rcvd, version %d", bgp->address, version);
    if (!bgp->bgp_configed_version ||
	(bgp->bgp_configed_version == version)) {
	switch (version) {
	case 2:
	    open2 = (bgp2_open *)open3;
	    authcode = open2->authcode;
	    minsize = BGP_MIN_OPEN2;
	    yoursystem = GETSHORT(open2->my_as);
	    holdtime = GETSHORT(open2->holdtime);
	    versionok = TRUE;
	    break;
    case 3:
    case 4:
	    /*
	     * BGP-4 changed authcode to optional parameters length
	     */
	    if (version > 3) {
		optlen = open3->authcode;
		optstart = (char *)&open3->authcode + 1;
	    } else
		authcode = open3->authcode;
	    
	    minsize = BGP_MIN_OPEN3;
	    bgp->bgp_router_id = (ipaddrtype) GETLONG(&open3->identifier);
	    yoursystem = GETSHORT(open3->my_as);
	    holdtime = GETSHORT(open3->holdtime);

	    /*
	     * If there is a connection collision, resolve it here.
	     */
	    if (bgp->secondary_tcb) {
		if ((ulong) bgp->bgp_router_id >
		    (ulong) pdb_BGP->bgp_rtr_id) {
		    if (bgp_debug)
			buginf ("\nBGP: %i closing our tcb", bgp->address);
		    tcbtemp = bgp->secondary_tcb;
		    bgp->secondary_tcb = NULL;
		    tcbtemp->flags |= TCB_RTRANSTMO;	/* allow TCP to give up */
		    tcp_close(tcbtemp);
		} else {
		    if (bgp_debug)
			buginf ("\nBGP: %i closing remote tcb", bgp->address);
		    tcbtemp = bgp->tcb;
		    bgp->tcb = bgp->secondary_tcb;
		    bgp->secondary_tcb = NULL;
		    tcbtemp->flags |= TCB_RTRANSTMO; /* allow TCP to give up */
		    tcp_close(tcbtemp);

		    /*
		     * Be sure to clean up any remanants of the old tcb.
		     */
		    bgp->bgp_netcount = 0;
		    bgp->readstate = RD_START;
		}
		bgp_free_writeQ(&bgp->bgp_secondary_writeQ);
	    }
	    versionok = TRUE;
	    break;
	default:
	    versionok = FALSE;
	}
    }

    /*
     * If we've sent something, and it doesn't match what he's thinking,
     * then check it.
     */
    if ((bgp->bgp_state == BGPS_OPENSENT) &&
	(bgp->protocol_version != version)) {
	versionok = FALSE;
    }

    /*
     * When we performed the passive connection, we went into connect state
     * and delibarately delay sending our open until we receive the active
     * user's open (or 5 seconds, whichever comes first).  When we send this
     * open, we'll change state to BGPS_OPENSENT.
     */
    if (bgp->bgp_state == BGPS_CONNECT) {
	if (versionok)
	    bgp->protocol_version = version;
	bgp_send_open(bgp, TRUE);
    }

    /*
     * If there is a version mismatch, then complain and try again.
     */
    if (!versionok) {
	if (bgp_debug)
	    buginf("\nBGP: %i bad protocol version, remote requested %d,"
		   " local request %d", bgp->address, version, 
		   bgp->protocol_version);
	if ((version > BGP_MAX_VERSION) || (version < BGP_MIN_VERSION))
	    version = BGP_VERSION;
	if (bgp->bgp_configed_version)
	    version = bgp->bgp_configed_version;
	bgp->protocol_version = version;
	bgp_send_notification(bgp, BNOT_OPEN, BNOT_OPEN_VERSION,
			      (uchar *)&version, 2);
	return;
    }

    /*
     * If we have an optional parameters TLV sequence, try to parse it.
     */
    for (opt = optstart, len = optlen; len > 0; opt += clen, len -= clen + 2) {
	code = *opt++;
	clen = *opt++;
	switch (code) {
	case BGP_OPEN_OPT_AUTH:
	    /* save the authentication type, do nothing else */
	    authcode = *opt;
	    if (bgp_debug)
		buginf("\nBGP: %i authentication type %d requested",
		       bgp->address, authcode);
	    break;
	default:
	    if (bgp_debug)
		buginf("\nBGP: %i unrecognized OPEN parameter (%#x/%#x)",
		       bgp->address, code, clen);
	}
    }

    /*
     * Check to make sure the open wasn't too small and that they didn't
     * try authentication (which is undefined at this time).
     */
    if (authcode || (bytes < minsize)) {
	if (bgp_debug)
	    buginf("\nBGP: %i bad OPEN authorization %d or byte count %d",
		   bgp->address, authcode, bytes);
	bgp_send_notification(bgp, BNOT_OPEN, BNOT_OPEN_AUTHCODE, NULL, 0);
	return;
    }

    /*
     * Ensure that AS of peer is what we expect.
     */
    if ((bgp->yoursystem == 0) && (bgp->flags & BN_TEMPORARY)) {
	bgp->yoursystem = yoursystem;
	/*
	 * this dance now clears all relationship between the template
	 * and the newly created child... it's delayed until here so
	 * anonymous neighbors can be configured.
	 */
	if (bgp->flags & BN_PERMCFG)
	    bgp->flags &= ~(BN_PERMCFG|BN_TEMPORARY);
	if (bgp->bgp_internal)
	    bgp->bgp_defminadverttime = BGP_IBGP_MINADVERTINTERVAL*ONESEC;
	else
	    bgp->bgp_defminadverttime = BGP_EBGP_MINADVERTINTERVAL*ONESEC;
	if (!bgp->bgp_minadverttime)
	    bgp->bgp_minadverttime = bgp->bgp_defminadverttime;
    }
    if (yoursystem != bgp->yoursystem) {
	if (bgp_debug)
	    buginf("\nBGP: %i bad OPEN, remote AS is %d, expected %d",
		   bgp->address, yoursystem, bgp->yoursystem);
	bgp_send_notification(bgp, BNOT_OPEN, BNOT_OPEN_PEERAS,
			      (uchar *)&yoursystem, 2);
	return;
    }

    /*
     * Good arguments, set up data structures and confirm with KEEPALIVE
     * The holdtime is the min of ours and his.  Hold times must be more
     * than 3s, and keepalives must be more than 1s and less than the hold
     * time.  Keepalives are also set to be one third of the holdtime.
     * ...unless this is BGP4 and the negotiated holdtime is zero, in which
     * case we never send keepalives and never require them.
     */
    bgp->protocol_version = version;
    holdtime = min(holdtime, pdb_BGP->bgp_holdtime); 
    if ((bgp->protocol_version > 3) && (holdtime == 0)) {
	bgp->bgp_holdtime = 0;
	bgp->bgp_probetime = 0;
    } else {
	holdtime = max(3, holdtime);
	bgp->bgp_holdtime  = holdtime * ONESEC;
	bgp->bgp_probetime = min(pdb_BGP->bgp_keeptime, holdtime - 1) * ONESEC;
	bgp->bgp_probetime = min(bgp->bgp_probetime, bgp->bgp_holdtime/3);
	bgp->bgp_probetime = max(bgp->bgp_probetime, ONESEC);
    }
    bgp->bgp_internal = (yoursystem == pdb_BGP->asystem);
    bgp->version = 0;

    if (!bgp->bgp_internal && (bgp->ebgp_ttl == BGP_DEFAULT_EBGP_TTL)) {
	ipaddrtype dummy = bgp->address;
	bgp->idb = iprouting_first_hop(&dummy, dummy, 0, ROUTINGDEPTH,
				       IP_PATH_EXACT_MATCH, NULL);
    } else
	bgp->idb = NULL;

    bgp_send_keepalive(bgp);
    bgp_statechange(bgp, BGPS_OPENCONFIRM);
}

/*
 * bgp_netread
 * Read a message from the network.  Non-blocking.
 * Returns TRUE if a message has been successfully read.
 */

BGP_PRIVATE boolean
bgp_netread (neighbortype *bgp)
{
    int status, len;
    bmsgtype *msg;
    bgp_header *header;

    if (bgp->bgp_netcount == 0) {
	status = tcp_getstring(bgp->tcb, &bgp->bgp_netptr,
				&bgp->bgp_netcount, MAXINT);
	if ((status != OK) && (status != URGENT_DATA))
	    return (FALSE);
    }
    if (bgp->bgp_netcount == 0)
	return (FALSE);
    if (bgp->readstate == RD_START) {
	bgp->readstate = RD_SEARCH;
	bgp->readptr = bgp->readdata;
	bgp->readcount = 0;
    }
    if (bgp->readstate == RD_SEARCH) {
	while ((bgp->readcount < BGP_HEADERBYTES) && bgp->bgp_netcount) {
	    *bgp->readptr++ = *bgp->bgp_netptr++;
	    bgp->readcount++;
	    bgp->bgp_netcount--;
	}
	if (bgp->readcount != BGP_HEADERBYTES)
	    return (FALSE);
	header = (bgp_header *) bgp->readdata;
	bgp->readlimit = GETSHORT(header->len);
	if ((bgp->readlimit > BGP_MAXBYTES) ||
	    (bgp->readlimit < BGP_HEADERBYTES)) {
	    if (bgp_debug)
		buginf("\nBGP: %i bad message length - %d",
			bgp->address, bgp->readlimit);
	    bgp_send_notification(bgp, BNOT_HEADER, BNOT_HEADER_LENGTH,NULL,0);
	    return (FALSE);
	}	    
	bgp->readstate = RD_MALLOCING;
    }
    GET_TIMESTAMP(bgp->bgp_lastread);
    if (bgp->readstate == RD_MALLOCING) {
	msg = bgp_getbuffer(bgp->readlimit, -1, FALSE);
	if (!msg) {
	    if (!bgp_emergency_msg)
		return (FALSE);
	    msg = bgp_emergency_msg;
	    bgp_emergency_msg = NULL;
	    msg->datagramsize = bgp->readlimit;
	    msg->datagramstart = (uchar *) msg + sizeof(bmsgtype);
	    msg->dataptr = (uchar *) msg->datagramstart + BGP_HEADERBYTES;
	    if (bgp_debug)
		buginf("\nBGP: %i using emergency buffer",
			bgp->address);
	}
	bcopy(bgp->readdata, msg->datagramstart, BGP_HEADERBYTES);
	msg->length = BGP_HEADERBYTES;
	bgp->bgp_netpak = msg;
	bgp->readstate = RD_FILLING;
    }
    if (bgp->readstate == RD_FILLING) {
	msg = bgp->bgp_netpak;
	len = msg->datagramsize - msg->length;
	if (bgp->bgp_netcount < len)
	    len = bgp->bgp_netcount;
	bcopy(bgp->bgp_netptr, msg->dataptr, len);
	bgp->bgp_netptr += len;
	msg->dataptr += len;
	bgp->bgp_netcount -= len;
	msg->length += len;
 	if (msg->length == msg->datagramsize) {
	    bgp->bgp_netpak = NULL;
	    bgp->readstate = RD_START;
	    enqueue(&bgp->bgp_readQ, msg);
	    bgp_read_queues_nonempty = TRUE;
	    return (TRUE);
	}
    }
    return (FALSE);
}

/*
 * bgp_open_active
 * Initiate a local attempt to open a connection
 */

BGP_PRIVATE forktype
bgp_open_active (ipaddrtype address)
{
    tcbtype *tcb;
    neighbortype *bgp;
    int status;
    boolean primary = TRUE, multihop;
    ipaddrtype laddress, dummy;
    idbtype *source_idb;
    sys_timestamp delay;
    addrtype addr;

    TIMER_STOP(delay);				/* stupid compiler (unix only) */

    /* freeze exterior routing on crash */
    process_set_crashblock(THIS_PROCESS,TRUE);

    for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next) {
	if (bgp->address == address)
	    break;
    }
    if (!bgp || !pdb_BGP->running || (bgp->bgp_state != BGPS_ACTIVE)) {
	if (bgp)
	    bgp->open_pid = -1;
	process_kill(THIS_PROCESS);
    }

    multihop = (bgp->bgp_internal || (bgp->ebgp_ttl != BGP_DEFAULT_EBGP_TTL));

    /*
     * Wait for that startup delay jitter to expire.  After it does expire,
     * the bgp pointer may not be there, so look it up again.
     */
    if (!(bgp->flags & BN_RESTART)) {
	TIMER_START_JITTERED(delay, bgp->bgp_internal ? IBGP_OPEN_DELAY :
			     EBGP_OPEN_DELAY, BGP_OPEN_JITTER);
	if (bgp_debug)
	    buginf("\nBGP: %i open active, delay %ums",
		   bgp->address, TIME_LEFT_SLEEPING(delay));
	process_sleep_until(&delay);
    }
			
    while (TRUE) {
	for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next) {
	    if (bgp->address == address)
		break;
	}
	if (!bgp || !pdb_BGP->running || (bgp->bgp_state != BGPS_ACTIVE)) {
	    if (bgp)
		bgp->open_pid = -1;
	    process_kill(THIS_PROCESS);
	}

	/*
	 * If the neighbor is multihop away, then check if we have a route
	 * to the neighbor. If no route, dismiss for OPEN_NOROUTE_DELAY and
	 * retry again.
	 */
	if (multihop) {
	    dummy  = bgp->address;
	    if (!iprouting_first_hop(&dummy, dummy, 0, ROUTINGDEPTH,
				     IP_PATH_EXACT_MATCH, NULL)) {
		TIMER_START_JITTERED(delay, BGP_OPEN_NOROUTE_DELAY,
				     BGP_OPEN_JITTER);
		if (bgp_debug)
		    buginf("\nBGP: %i multihop open delayed %dms (no route)",
			   bgp->address, TIME_LEFT_SLEEPING(delay));
		process_sleep_until(&delay);
		continue;
	    }
	}

	source_idb = bgp->bgp_update_source;
	if (source_idb && source_idb->ip_address && interface_up(source_idb))
	     laddress = source_idb->ip_address;
	else
	     laddress = ip_best_local_address(address, TRUE);

	if (bgp_debug)
	    buginf("\nBGP: %i open active, local address %i", bgp->address,
		   laddress); 
	tcb = tcp_create(NULL, &status, NULL);
	if (tcb) {
	    ulong winsize = BGP_WINDOW;
	    char clear_password[PASSWORDLEN+1];
	    char *pass;

	    tcp_set_property(tcb, TCP_WINDOW_SIZE, &winsize, sizeof(winsize));
	    if (bgp->bgp_password.pw) {
		pass = bgp->bgp_password.pw;
		if (bgp->bgp_password.encrypt_type == ENCRYPT_TEMP) {
		    decrypt(pass, clear_password, PASSWORDLEN);
		    pass = clear_password;
		}
		tcp_set_property(tcb, TCP_MD5KEY, pass, strlen(pass));
	    }

	    addr.type = ADDR_IP;
	    addr.ip_addr = laddress;
	    status = tcp_bind(tcb, &addr, 0);
	    if (status == OK) {
		addr.type = ADDR_IP;
		addr.ip_addr = address;
		status = tcp_connect(tcb, &addr, BGP_PORT);
		if (status != OK) {
		    tcp_close(tcb);
		    tcb = NULL;
		}
	    } else {
		tcp_close(tcb);
		tcb = NULL;
	    }
	}
	if (!tcb) {
	    if (bgp_debug)
		buginf("\nBGP: %i open failed: %s", address,
				tcperror2string(status));
	    process_sleep_for(2*ONEMIN);
	    continue;
	}
	for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next) {
	    if (bgp->address == address)
		break;
	}
	if (!bgp || !pdb_BGP->running) {
	    tcp_abort(tcb);
	    process_kill(THIS_PROCESS);
	}
	if (bgp->bgp_state != BGPS_ACTIVE) {
	    if (bgp_debug)
		buginf("\nBGP: %i connection collision", bgp->address);
	    if ((bgp->protocol_version > 2) &&
		(bgp->bgp_state == BGPS_OPENSENT)) {
		bgp->secondary_tcb = tcb;
		primary = FALSE;
		break;
	    }
	    tcp_abort(tcb);
	    bgp->open_pid = -1;
	    process_kill(THIS_PROCESS);
	} else
	    bgp->tcb = tcb;
	tcb->flags &= ~TCB_RTRANSTMO;
	tcb->flags |= TCB_NAGLE;
	tcb->param.tos = TOS_PREC_INET_CONTROL;

	if (!bgp->bgp_internal)
	    tcb->param.ttl = bgp->ebgp_ttl;

	break;
    }
    bgp_send_open(bgp, primary);
    bgp->open_pid = -1;
    process_kill(THIS_PROCESS);
}

/*
 * bgp_open_check
 * Check if we can open a BGP connection.
 * We must be running a BGP process and we must know the remote address
 * as one of our neighbors.  This is called from IP Protocols before
 * actually creating the TCB so we can know whether or not to send a RST.
 */

BGP_PRIVATE boolean
bgp_open_check (ulong sport, ulong dport, ipaddrtype address)
{
    neighbortype *bgp, *p, *last;

    if (!pdb_BGP || !pdb_BGP->running || !systeminit_flag)
	return (FALSE);

    /*
     * If this request is from an external neighbor without neighbor
     * index, refuse to proceed.
     */
    for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next) {
	if (bgp->address == address) {
	    if (!bgp->bgp_internal && !bgp->neighbor_index)
		return(FALSE);
	    else
		return (bgp->bgp_state < BGPS_OPENSENT);
	}
    }

    /*
     * didn't find it, check the template list
     */
    for (bgp = pdb_BGP->promiscuous; bgp; bgp = bgp->next) {
	if (bgp->pfilter && nfast_check(bgp->pfilter, address)) {
	    p = malloc(sizeof(neighbortype));
	    if (!p)
		return (FALSE);
	    bcopy(bgp, p, sizeof(neighbortype));
	    if (!(bgp->flags & BN_PERMCFG))
		p->creator_template = bgp;	/* point to template that
						   created us */
	    bgp = p;
	    bgp->address = address;
	    sprintf(bgp->neighbor_name, "%i", address);
	    bgp->flags &= ~BN_TEMPLATE;
	    bgp->flags |= BN_TEMPORARY;
	    bgp->bgp_state = BGPS_IDLE;
	    bgp->pfilter = NULL;
	    bgp->next = NULL;

	    /* add to neighbor list in lexigrapical order (for snmp) */
	    if (pdb_BGP->neighbors) {
		last = (neighbortype *) &pdb_BGP->neighbors;
		p = pdb_BGP->neighbors;
		while (p && (address > p->address)) {
		    last = p;
		    p = p->next;
		}
		bgp->next = last->next;
		last->next = bgp;
	    } else
		pdb_BGP->neighbors = bgp;
	    if (bgpevent_debug)
		buginf("\nBGP: %i accepted as new neighbor", address);
	    return (TRUE);
	}
    }
    return (FALSE);
}

/*
 * bgp_guess_md5_key
 * Provide the password currently configured for the specified peer (faddr).
 */

BGP_PRIVATE char *
bgp_guess_md5_key (addrtype *laddr, addrtype *faddr, ushort lport,
		   ushort fport, char *buf, int buflen)
{
    neighbortype *bgp;
    char clear_password[PASSWORDLEN+1];
    char *pass;

    if (!pdb_BGP || !pdb_BGP->running || !systeminit_flag)
	return(NULL);

    for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next) {
	if (bgp->address == faddr->ip_addr) {
	    pass = bgp->bgp_password.pw;
	    if (bgp->bgp_password.encrypt_type == ENCRYPT_TEMP) {
		decrypt(pass, clear_password, PASSWORDLEN);
		pass = clear_password;
	    }
	    strncpy(buf, pass, buflen);
	    buf[buflen-1] = '\0';	/* Make sure string is terminated. */
	    return(buf);
	}
    }

    return(NULL);
}

/*
 * bgp_open_passive_guts
 * Handle a remote attempt to open a connection
 */

BGP_PRIVATE void
bgp_open_passive_guts (tcbtype *tcb)
{
    neighbortype *bgp;
    boolean abortflag;

    if (!tcb)				/* In case TCP is confused. */
	return;

    if (bgp_debug)
	buginf("\nBGP: %i passive open", tcb->foreignhost.ip_addr);

    /*
     * Find the BGP pdb and an idle neighbor.
     * If we fail, abort the TCP connection.
     */
    bgp = NULL;
    abortflag = TRUE;
    if (pdb_BGP && pdb_BGP->running && systeminit_flag) {
	for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next) {
	    if (bgp->address == tcb->foreignhost.ip_addr) {
		abortflag = (bgp->bgp_state > BGPS_ACTIVE) && bgp->tcb;
		break;
	    }
	}
    }

    if (!bgp || abortflag) {
	if (bgp_debug)
	    buginf("\nBGP: %i remote connection attempt failed, local"
		   " address %i", tcb->foreignhost.ip_addr,
		   tcb->localhost.ip_addr);
	tcp_abort(tcb);
	return;
    }

    /*
     * We have formed a good TCP connection to a BGP neighbor.
     * Finish setting up data structures and enter opening state.
     * The bgp_close() initializes our data structures.
     */
    if (bgpevent_debug && bgp->tcb)
	buginf("\nBGP: %i reset connection, passive open", bgp->address);
    bgp_close(bgp);
    bgp_statechange(bgp, BGPS_CONNECT);
    tcb->tty = NULL;
    tcb->flags &= ~TCB_RTRANSTMO;
    tcb->flags |= TCB_NAGLE;
    tcb->param.tos = TOS_PREC_INET_CONTROL;

    if (!bgp->bgp_internal)
	tcb->param.ttl = bgp->ebgp_ttl;

    if (!(tcb->internal_flags & TCB_INT_PRIMITIVES)) /* Old-style connection */
	tcp_set_rcvwnd(tcb, BGP_WINDOW);

    bgp->tcb = tcb;
}

/*
 * bgp_accepter
 * Call bgp_open_passive_guts in a process context.
 */

BGP_PRIVATE process
bgp_accepter (void)
{
    tcbtype *tcb;

    if (!process_get_arg_ptr((void**)&tcb))
	process_kill(THIS_PROCESS);

    bgp_open_passive_guts(tcb);

    /* One less accepter (soon). */
    if (bgpaccepters_current)
        bgpaccepters_current--;
    process_kill(THIS_PROCESS);
}

/*
 * bgp_open_passive
 * Handle a remote attempt to open a connection to one of our
 * host-specific listeners.  Any other attempts will go to the
 * code above.
 */

BGP_PRIVATE void
bgp_open_passive (tcbtype *tcb)
{
    tcbtype *ntcb;
    int error;
    pid_t pid;

    ntcb = tcp_accept(tcb, &error);
    if (ntcb == NULL) {
	if (bgp_debug)
	    buginf("\nBGP: accept failed -- %s", tcperror2string(error));
	return;
    }

    /*
     * The state change processing can block if SNMP traps are enabled,
     * so we have to do the accept in another (short-lived) process
     * since this callback is not allowed to block.
     */
    if (bgpaccepters_current < bgpaccepters_max) {
	pid = process_create(bgp_accepter, "BGP Accepter", SMALL_STACK,
			     PRIO_NORMAL);
	if (pid != NO_PROCESS) {
	    process_set_arg_ptr(pid, ntcb);
	    process_set_crashblock(pid, TRUE);	
	    /* One more accepter. */
	    bgpaccepters_current++;
	    return;
	}
    }

    /*
     * Oops, no process, have to hope we don't block.
     */
    bgp_open_passive_guts(ntcb);
}

/*
 * bgp_lock_msg
 * Lock a BGP update message.
 */

BGP_PRIVATE inline void
bgp_lock_msg (bmsgtype *msg)
{
    mem_lock(msg);
    if (msg->msgheadptr)
	mem_lock(msg->msgheadptr);
}

/*
 * bgp_free_msg
 * Free a BGP update message.
 */

BGP_PRIVATE inline void
bgp_free_msg (bmsgtype *msg)
{
    if (msg->msgheadptr)
	free(msg->msgheadptr);
    free(msg);
}

/*
 * bgpioBLOCK
 * Block until at least one BGP peer needs reading or writing
 */

BGP_PRIVATE boolean
bgpioBLOCK (pdbtype *pdb)
{
    neighbortype *bgp;

    for (bgp = pdb->neighbors; bgp; bgp = bgp->next) {

	/*
	 * We can't do any IO without a tcb.
	 */
	if (!bgp->tcb)
	    continue;
	
	/*
	 * Wake up if we have been sent a message but not read it.
	 */
	if (tcp_activity(bgp->tcb))
	    return (FALSE);
	
	/*
	 * Wake up if it's time to send a keepalive.
	 */
	if (bgp->bgp_probetime && AWAKE(bgp->bgp_ktimer) &&
	    (bgp->bgp_state == BGPS_ESTAB))
	    return (FALSE);

	/*
	 * Wake up if we've waited more than 5 seconds for an open message
	 * after we've received the TCP connection.
	 */
	if ((bgp->bgp_state == BGPS_CONNECT) &&
	    !CLOCK_IN_INTERVAL(bgp->bgp_lastread, BGP_OPEN_DELAY))
	    return (FALSE);

	/*
	 * Wake up if a TCP write would not block AND we have something
	 * to write.
	 */
	if (bgp->bgp_writeQ.qhead && !tcpputBLOCK(bgp->tcb))
  	    return (FALSE);
	if (bgp->secondary_tcb && bgp->bgp_secondary_writeQ.qhead &&
	    !tcpputBLOCK(bgp->secondary_tcb)) 
  	    return (FALSE);
    }
    return (TRUE);
}

/*
 * bgp_io
 * Handle reading, writing, and executing BGP messages.  
 * This is a little funkier than it needs to be since both tcp_putstring()
 * and tcp_putpush() can block.
 */

BGP_PRIVATE forktype
bgp_io (pdbtype *pdb)
{
    bmsgtype *msg;
    neighbortype *bgp;
    int count;
    tcbtype *tcb;

    /* freeze exterior routing on crash */
    process_set_crashblock(THIS_PROCESS,TRUE);

    while (TRUE) {
	edisms((blockproc *)bgpioBLOCK, (ulong)pdb);
	for (bgp = pdb->neighbors; bgp; bgp = bgp->next) {
	    if (!bgp->tcb)
		continue;

  	    /*
	     * Send keepalive probe if we haven't written for a while.
	     */
	    if (bgp->bgp_probetime && AWAKE(bgp->bgp_ktimer) &&
	        (bgp->bgp_state == BGPS_ESTAB)) {
	        bgp_send_keepalive(bgp);
		TIMER_UPDATE(bgp->bgp_ktimer, bgp->bgp_probetime);
	    }

	    /*
	     * If we've been in connect state too long, jump-start the
	     * open process.
	     */
	    if ((bgp->bgp_state == BGPS_CONNECT) &&
		!CLOCK_IN_INTERVAL(bgp->bgp_lastread, BGP_OPEN_DELAY))
		bgp_send_open(bgp, TRUE);

	    /*
	     * Write to the net to process our write queue.
	     * tcp_putstring could block and to avoid dereferencing
	     * freed msg in the while loop, lock the msg.
	     */
	    tcb = bgp->tcb;
	    while (!QUEUEEMPTY(&bgp->bgp_writeQ)) {
		msg = (bmsgtype *) bgp->bgp_writeQ.qhead;
		bgp_lock_msg(msg);
		while (msg->datagramsize) {
		    count = tcp_putstring(tcb, msg->datagramstart,
					  msg->datagramsize);
		    if (bgp->tcb != tcb) {
			bgp_free_msg(msg);
			goto blocked;
		    }
		    if (!count) {
			tcp_putpush(tcb, FALSE, FALSE);
			bgp_free_msg(msg);
			goto blocked;
		    }
		    msg->datagramstart += count;
		    msg->datagramsize -= count;
		}
		bgp_free_msg(msg);
		if (!QUEUEEMPTY(&bgp->bgp_writeQ)) {
		    free(msg->msgheadptr);
		    free(dequeue(&bgp->bgp_writeQ));
		}
		bgp->outmsgs++;
		tcp_putpush(bgp->tcb, TRUE, FALSE);
		if (bgp->tcb != tcb)
		    goto blocked;
	    }

	    /*
	     * Now process the secondary write queue
	     * tcp_putstring could block and to avoid dereferencing
	     * freed msg in the while loop, lock the msg.
	     */
	    tcb = bgp->secondary_tcb;
	    if (!tcb)
		goto blocked;
	    while (!QUEUEEMPTY(&bgp->bgp_secondary_writeQ)) {
		msg = (bmsgtype *) bgp->bgp_secondary_writeQ.qhead;
		bgp_lock_msg(msg);
		while (msg->datagramsize && bgp->secondary_tcb) {
		    count = tcp_putstring(tcb, msg->datagramstart,
					  msg->datagramsize);
		    if (bgp->secondary_tcb != tcb) {
			bgp_free_msg(msg);
			goto blocked;
		    }
		    if (!count) {
			tcp_putpush(tcb, FALSE, FALSE);
			bgp_free_msg(msg);
			goto blocked;
		    }
		    msg->datagramstart += count;
		    msg->datagramsize -= count;
		}
		bgp_free_msg(msg);
		if (!QUEUEEMPTY(&bgp->bgp_secondary_writeQ)) {
		    free(msg->msgheadptr);
		    free(dequeue(&bgp->bgp_secondary_writeQ));
		}
		bgp->outmsgs++;
		tcp_putpush(tcb, TRUE, FALSE);
		if (bgp->secondary_tcb != tcb)
		    goto blocked;
	    }
	blocked:
	    if (!bgp->tcb)
		continue;

	    /* don't read more messages from a pipe we're closing */
	    if (bgp->state == BGPS_CLOSING || bgp->state == BGPS_IDLE)
		continue;

	    /*
	     * Read from the net to create the read queue.
	     */
	    for (count = 0;
		 (count < BGP_READLIMIT) &&
		 (bgp->bgp_netcount || tcp_activity(bgp->tcb));
		 count++) {
		if (!bgp_netread(bgp))
		    break;
  	    }
	    if (count == BGP_READLIMIT && bgpevent_debug)
		buginf("\nBGP: %i reads throttling at %d messages",
		       bgp->address, BGP_READLIMIT);
			
	}
    }
}

BGP_PRIVATE inline void
bgp_bump_net_version (bnettype *net)
{
    net->version = ++bgptable_version;
    rn_mark_parents((rntype *) net, net->version);
}

/*
 * rn2bnettype
 * Cast a radix node to a BGP network entry with type checking enforced.
 */
inline static bnettype *
rn2bnettype(rntype *rn)
{
    return((bnettype *) rn);
}

/*
 * bgp_netlookup
 * Look up a network in the BGP routing table and return it.
 */

BGP_PRIVATE bnettype *
bgp_netlookup (ipaddrtype network, ipaddrtype mask)
{
    ipradixkeytype net_key, mask_key;

    if (mask)
	return (bnettype *) rn_lookup(ipaddr2radixkey(network, &net_key),
			              ipaddr2radixkey(mask, &mask_key),
				      bgp_head);
    else
	return (bnettype *) rn_match(ipaddr2radixkey(network, &net_key),
				     bgp_head);
}

/*
 * bgp_nextnode
 * Find the next radix node (active or not, we don't care)
 * after the target node containing information about "network"
 */

BGP_PRIVATE rntype *
bgp_nextnode (ipaddrtype network)
{
    ipradixkeytype key;
    rntype *rn;

    if (!network)
	return (NULL);

    /* we can call the unsafe routine because we're checking for root here */
    rn = rn_match(ipaddr2radixkey(network, &key), bgp_head);

    /*
     * If the world has changed out from under us so badly that we don't
     * have a parent node anymore, we need to start from the left most
     * node in the tree and fall through to the linear traverser... UGH!
     */
    if (!rn) {
	if (bgp_debug)
	    buginf("\nBGP: nextnode couldn't find starting point for %i",
		   network);

	rn = bgp_head->rnh_treetop;
	while (rn->rn_b >= 0)
	    rn = rn->rn_l;
	if (rn->rn_flags & RNF_ROOT)
	    rn = rn_nextleaf(rn);
    }

    while (!(rn->rn_flags & RNF_ROOT) &&
	    (((bnettype *)rn)->network <= network))
	rn = rn_nextleaf(rn);

    return rn;
}

/*
 * bgp_subtreelookup
 * Look up a network/mask in the BGP routing table and return TRUE if there
 * are routes as MORE specific than the requested pair.
 */

BGP_PRIVATE bnettype *
bgp_subtreelookup (ipaddrtype network, ipaddrtype mask)
{
#define	net	((bnettype *) rn)	/* speed kludge */
    rntype *rn;
    ipradixkeytype net_key, mask_key;

    rn = rn_search_m(ipaddr2radixkey(network, &net_key),
		     bgp_head->rnh_treetop,
		     ipaddr2radixkey(mask, &mask_key));

    if (!(rn->rn_flags & RNF_ACTIVE))
	return (NULL);

    /* are we out of range? fail */
    if ((net->network & mask) != network)
	return (NULL);

    /* if we're in range, the first entry in the dupekey chain is the
       longest match, so we can just check that node's mask against ours */

    if (net->mask > mask)
	return (net);

    /*
     * OK, so we didn't find anything with a smaller mask that was in
     * our range -- but we could be getting screwed by our own aggregate
     * route, so check again, this time looking at the next node in the
     * tree, which may still be in range and have a more specific match.
     */

    rn = bgp_nextnode(network);
    if (!(rn->rn_flags & RNF_ACTIVE))
	return (NULL);

    if ((net->network & mask) != network)
	return (NULL);

    if (net->mask > mask)
	return (net);

    return (NULL);

#undef net
}

/*
 * bgp_fwdtable_lookup
 * Return a pointer to information in the main routing table
 */

BGP_PRIVATE inline ndbtype *
bgp_fwdtable_lookup (ipaddrtype address, ipaddrtype mask)
{
    if (pdb_BGP->auto_summary)
	return mask > get_majormask(address) ?
	       ip_rtlookup(address, mask) :
	       netsuper_lookup(address, mask, FALSE);
    else
	return ip_rtlookup(address, mask);
}

/*
 * bgp_advertised_mask
 * Given a net, return a pointer to the advertised mask field. The
 * bitfield is at the end of the net.  
 */

BGP_PRIVATE inline uchar *
bgp_advertised_mask (bnettype *net)
{
    uchar *bitfield;
    
    bitfield = (uchar *)net + (sizeof(bnettype) + 
			       (net->pathsmalloced * sizeof(bpathtype)));
    return(bitfield);
}

/*
 * bgp_set_advertised_bit
 * Set the advertised-bit for a neighbor. 
 */

BGP_PRIVATE inline void
bgp_set_advertised_bit (bnettype *net, neighbortype *bgp) 
{
    uchar *bitfield;

    bitfield = bgp_advertised_mask(net);
    bitfield[bgp->neighbor_index_offset] |= bgp->neighbor_index_mask;
}

/*
 * bgp_clear_advertised_bit 
 * Clear a neighbor's advertised-bit. 
 */

BGP_PRIVATE inline void
bgp_clear_advertised_bit (bnettype *net, neighbortype *bgp)
{
    uchar *bitfield;

    bitfield = bgp_advertised_mask(net);
    bitfield[bgp->neighbor_index_offset] &= ~bgp->neighbor_index_mask;
}

/*
 * bgp_advertised
 * Check if a net was advertised to an External neighbors. 
 */

BGP_PRIVATE inline boolean
bgp_advertised (bnettype *net, neighbortype *bgp)
{
    uchar *bitfield;

    bitfield = bgp_advertised_mask(net);
    return(bitfield[bgp->neighbor_index_offset] & bgp->neighbor_index_mask);
}

/*
 * bgp_suppress_unreachable
 * Check if unreachable could be suppressed. If unreachable should be sent,
 * clear the advertised bit if it is allowed. In case of peergroup members
 * the advertised bit will be cleared after informing all the peergroup 
 * members!
 */

BGP_PRIVATE inline boolean
bgp_suppress_unreachable (bnettype *net, neighbortype *bgp, 
			  boolean clear_bit)
{
    if (bgp_advertised(net, bgp)) {
	if (clear_bit)
	    bgp_clear_advertised_bit(net, bgp);
	return(FALSE);
    } else {
	return(TRUE);
    }
}

/*
 * bgp_reset_bit_walker
 * Clear an advertised bit.
 */

BGP_PRIVATE int
bgp_reset_bit_walker (rntype *rn, va_list ap)
{
    bnettype *net	= rn2bnettype(rn);
    neighbortype *bgp   = va_arg(ap, neighbortype *);

    bgp_clear_advertised_bit(net, bgp);
    return(WALK_CONTINUE);
}

/*
 * bgp_create_net
 * Create a network (bnettype) entry.
 */

BGP_PRIVATE bnettype *
bgp_create_net (ipaddrtype network, ipaddrtype mask)
{
    bnettype *net;

    net = malloc(sizeof(bnettype) + bgp_bitfield_size +
		 (BGP_INITPATHS * sizeof(bpathtype)));

    if (!net)
	return(NULL);

    net->pathsmalloced = BGP_INITPATHS;
    net->best = BGP_NO_BEST;
    net->firstpath = BGP_NULL_INDEX;

    net->network_key = IPRADIXKEYLEN;
    net->network     = network;
    net->mask_key    = IPRADIXKEYLEN;
    net->mask        = mask;

    if (!rn_addroute(&net->network_key, &net->mask_key,
		     bgp_head, net->rt_nodes))
	errmsg(&msgsym(ADDROUTE, BGP), net->network, net->mask);

    return(net);
}

/*
 * bgp_attr2neighboras
 * Given a battrtype, return the neigbor AS that advertised the path.
 */
BGP_PRIVATE ushort 
bgp_attr2neighboras (battrtype *attr)
{
    uchar *aspath, *nextseg;
    ushort neighboras;
    int seglen, len;

    if (!attr || (attr->aslength == 0))
	return(0);
    neighboras = 0;
    aspath = attr->as_path;

    /*
     * If the first segment is an AS_SEQUENCE, pick the first AS
     * number as the neighbor AS. If the first segment is an
     * ASENTRY_SEQUENCE (confederation), skip it to pick the actual
     * external neighbor AS. 
     */
    if (*aspath == AS_SEQUENCE)
	neighboras = *((ushort *)aspath+1);
    else if (*aspath == ASENTRY_SEQUENCE) {
	seglen = *(aspath+1);
	len = (seglen * PATHSIZE) + 2;
	if (attr->aslength > len) {
	    nextseg = aspath + len;
	    if (*nextseg == AS_SEQUENCE)
		neighboras = *((ushort *)nextseg+1);
	}
    }
    return(neighboras);
}

/*
 * bgp_get_pathindex
 *
 * Paths from the same neighboring AS are stored next to each other in
 * an ascending order of MED.  This function is called to locate the
 * right path index to add a path and set the BFLAG_MEDLOSER flag for
 * all paths that does not have the best MED value (considering the new
 * path as well). RCVD_ONLY path is ignored in MED sorting.
 *
 * Return a path index behind which the new path should be added.
 */
BGP_PRIVATE inline uchar
bgp_get_pathindex (bnettype *net, bpathtype *path)
{
    uchar ix, new_index, prev_index;
    battrtype *a_attr;
    ushort asnumber, path_asnumber;
    boolean found;
    bpathtype *a_path;
    ulong metric, best_metric;

    found = FALSE;
    new_index = BGP_MAXPATHS;
    prev_index = BGP_NULL_INDEX;

    asnumber = path->attr->neighboras;
    metric = path->attr->metric;
    best_metric = (path->flags & BFLAG_RCVD_ONLY) ?
	          METRIC_INACCESSIBLE : metric;
    path->flags &= ~BFLAG_MEDLOSER;

    /* We scan down the path list, clear the MEDLOSER bit first, then set
     * it if MED is worse than the best MED.  The way we choose best MED
     * assumes there is only one path not in the right MED order at any
     * one point in time.  This is true because anytime we delete or
     * replace a path, we fix the ordering immediately.
     */
    for (ix = net->firstpath; ix != BGP_NULL_INDEX; ix = net->paths[ix].next) {
	a_path = &net->paths[ix];
	a_attr = a_path->attr;
	path_asnumber = a_attr->neighboras;

	if (path_asnumber == asnumber) {
	    found = TRUE;

	    if (!(a_path->flags & BFLAG_RCVD_ONLY)) {

		a_path->flags &= ~BFLAG_MEDLOSER;
		if (a_attr->metric <= best_metric)
		    best_metric = a_attr->metric;
		else
		    a_path->flags |= BFLAG_MEDLOSER;

		if (a_attr->metric < metric)
		    path->flags |= BFLAG_MEDLOSER;
		if (a_attr->metric > metric)
		    if (new_index == BGP_MAXPATHS)
			new_index = ix;
	    }
	} else
	    if (found)
		break;
	prev_index = ix;
    }
    /* If we didn't set new_index in the above loop, return prev_index if
     * we did find path with the same neighboring AS (i.e. add the new
     * path at the end of the path list from the same AS), or else return
     * NULL so the new path will be added at the beginning.  If new_index
     * is set, return the index of the path before it.
     */
    if (new_index == BGP_MAXPATHS) {
	if (found)
	    return(prev_index);
	else
	    return(BGP_NULL_INDEX);
    } else
	return(net->paths[new_index].prev);
}

/*
 * bgp_link_path
 * Put a path behind the one pointed to by prev_idx.  If prev_idx is a
 * BGP_NULL_INDEX, then insert at the beginning.
 */
BGP_PRIVATE inline void
bgp_link_path (bnettype *net, uchar path_idx, uchar prev_idx)
{
    bpathtype *path, *prev_path;

    path = &net->paths[path_idx];
    if (prev_idx == BGP_NULL_INDEX) {
	if (net->firstpath != BGP_NULL_INDEX)
	    net->paths[net->firstpath].prev = path_idx;
	path->next = net->firstpath;
	path->prev = BGP_NULL_INDEX;
	net->firstpath = path_idx;
    } else {
	prev_path = &net->paths[prev_idx];
	path->prev = prev_idx;
	path->next = prev_path->next;
	if (path->next != BGP_NULL_INDEX)
	    net->paths[path->next].prev = path_idx;
	prev_path->next = path_idx;
    }
}

/*
 * bgp_delink_path
 * Delink a path from the path chain.
 */
BGP_PRIVATE inline void
bgp_delink_path (bnettype *net, uchar idx)
{
    bpathtype *dpath;

    dpath = &net->paths[idx];

    if (net->firstpath == idx)
	net->firstpath = dpath->next;

    if (dpath->prev != BGP_NULL_INDEX)
	net->paths[dpath->prev].next = dpath->next;

    if (dpath->next != BGP_NULL_INDEX)
	net->paths[dpath->next].prev = dpath->prev;

    dpath->next = dpath->prev = BGP_NULL_INDEX;
}

/*
 * bgp_enlarge_net
 * Allocate a larger bnettype and copy the contents from old net.
 */

BGP_PRIVATE bnettype *
bgp_enlarge_net (bnettype *net, int oldbitfield_size)
{
    bnettype *nnet;
    uchar *newbitfield, *oldbitfield;
    int new_pathcount, new_buffer_size, i, pathsmalloced;
    static int max_bnet_size = sizeof(bnettype);
    
    pathsmalloced = net->pathsmalloced;
  
    if ((net->pathcount >= pathsmalloced) && 
 	(net->pathsmalloced == BGP_MAXPATHS)) {
 	errmsg(&msgsym(MAXPATHS, BGP), net->network, net->mask, BGP_MAXPATHS);
 	return (NULL);
    }
 
    /*
     * No, go ahead and malloc a new network record with more space and
     * splice it into the table at the same place.
     */
    new_pathcount = min((int) pathsmalloced + BGP_ADDPATHS, BGP_MAXPATHS);
    new_buffer_size = (sizeof(bnettype) + bgp_bitfield_size + 
 		       (new_pathcount*sizeof(bpathtype)));
     
    /*
     * If we've grown to a new maximum, allocate another free list
     * to lessen the fragmentation of memory.
     */
    if (new_buffer_size > max_bnet_size) {
 	if (mempool_add_free_list(MEMPOOL_CLASS_LOCAL, new_buffer_size)) {
 	    max_bnet_size = new_buffer_size;
 	    if (bgp_debug)
 		buginf("\nBGP: adding new free list (size %d)",
 		       new_buffer_size);
 	} else {
 	    if (bgp_debug)
 		buginf("\nBGP: unable to add new free list for size %d",
 		       new_buffer_size);
  	}
    }
 
    nnet = malloc(new_buffer_size);
    if (!nnet) {
 	errmsg(&msgsym(NOMEMORY, BGP), "path");
 	bgp_reset(NULL, NULL);
 	return (NULL);
    }
  
    bcopy((uchar *)net + RN_TREE_AREA, (uchar *)nnet + RN_TREE_AREA,
 	  sizeof(bnettype) - RN_TREE_AREA +
 	  (pathsmalloced * sizeof(bpathtype)));
    nnet->pathsmalloced = new_pathcount;
  
     /*
      * Now copy the advertised bitfield. If the advertised bit field is
      * being enlarged, use the passed in old bitfield length in bcopy.
      */
    oldbitfield = bgp_advertised_mask(net);
    newbitfield = bgp_advertised_mask(nnet);
    if (oldbitfield_size)
 	bcopy(oldbitfield, newbitfield, oldbitfield_size);
    else
 	bcopy(oldbitfield, newbitfield, bgp_bitfield_size);
  
     /*
      * We need to remove the old buffer from the tree and insert the
      * new one.  This is simpler than trying to patch all of the pointers
      * and isn't really too nasty performance wise.
      */
    if (!rn_delete(&net->network_key, &net->mask_key, bgp_head))
 	errmsg(&msgsym(DELROUTE, BGP), net->network, net->mask);
  
    free(net);
 
    if (!rn_addroute(&nnet->network_key, &nnet->mask_key,
 		     bgp_head, nnet->rt_nodes))
 	errmsg(&msgsym(ADDROUTE, BGP), nnet->network, nnet->mask);
  
  
     /*
      * The dampinfo structure has back pointer to the net. As the net
      * has changed reset the net field in the dampinfo for all the
      * paths.
      */
    for (i = nnet->firstpath; i != BGP_NULL_INDEX; i = nnet->paths[i].next)
 	if (nnet->paths[i].dampinfo)
 	    nnet->paths[i].dampinfo->net = nnet;
 
    return(nnet);
}

/*
 * bgp_add_path
 * Add a new path to the current network.
 * Note : As the valid path entries are threaded via link list, 
 * there could be holes in the path array. 
 */

BGP_PRIVATE bnettype *
bgp_add_path (bnettype *net, bpathtype *path, battrtype *attr, int *index,
	      uchar prev_idx, uchar next_idx)
{
    int i, npath_idx;
    bpathtype *npath;
    uchar prev_index;

    npath_idx = BGP_NULL_INDEX;
  
    if (net->pathcount >= net->pathsmalloced) {
 	net = bgp_enlarge_net(net, 0);		/* doesn't fit. Enlarge */
 	if (!net)
 	    return(NULL);
  
 	npath_idx = net->pathcount;		/* new path in the end */
    } else {
 	for (i = 0; i < net->pathsmalloced; ++i)
  	    if (net->paths[i].attr == NULL) {
  		npath_idx = i;
  		break;
	    }
    }

    npath = &net->paths[npath_idx];
    bcopy(path, npath, sizeof(bpathtype));
    npath->attr = attr;
    mem_lock(attr);

    /*
     * By default link the new path to the start of the path chain.  If
     * previous or next path index is passed, add new path accordingly.
     * If deterministic MED is turned on, store path in the right
     * MED order.
     */
    if (prev_idx != BGP_NULL_INDEX) {
	npath->next = net->paths[prev_idx].next;
	if(npath->next != BGP_NULL_INDEX)
	    net->paths[npath->next].prev = npath_idx;
	npath->prev = prev_idx;
	net->paths[prev_idx].next = npath_idx;

    } else if (next_idx != BGP_NULL_INDEX) {
	npath->next = next_idx;
	if (net->paths[next_idx].prev != BGP_NULL_INDEX) {
	    npath->prev = net->paths[next_idx].prev;
	    net->paths[npath->prev].next = npath_idx;
	} else
	    net->firstpath = npath_idx;
	net->paths[next_idx].prev = npath_idx;
	
    } else if (bgp_determ_med) {
	prev_index = bgp_get_pathindex(net, npath);
	bgp_link_path(net, npath_idx, prev_index);

    } else {
	if (net->pathcount != 0)
	    net->paths[net->firstpath].prev = npath_idx;
	npath->next = net->firstpath;
	npath->prev = BGP_NULL_INDEX;
	net->firstpath = npath_idx;
    } 

    *index = npath_idx;
    net->pathcount++;
    return (net);
}

/*
 * bgp_relocate_path
 *
 * Move a path to the right place according to its MED.  We store path
 * from the same neighboring AS next to each other in the order of ascending
 * MED.
 */
BGP_PRIVATE void
bgp_relocate_path (bnettype *net, int ix)
{
    uchar prev, rcvd_only_to_be_moved;
    bpathtype *path;

    /* Check if this path is a modified path.  If so, remember to move the
     * RCVD_ONLY path (which is always follow a modified path) to the new
     * place after the modified path has been relocated.
     */
    rcvd_only_to_be_moved = BGP_NULL_INDEX;
    path = &net->paths[ix];
    if (!(path->flags & (BFLAG_RCVD_ONLY | BFLAG_RCVD_USED)) &&
	(path->next != BGP_NULL_INDEX) &&
	(net->paths[path->next].neighbor == path->neighbor) &&
	(net->paths[path->next].flags & BFLAG_RCVD_ONLY)) {
	rcvd_only_to_be_moved = path->next;
    }

    /* Relocate the path
     */
    bgp_delink_path(net, ix);
    prev = bgp_get_pathindex(net, &net->paths[ix]);
    bgp_link_path(net, ix, prev);

    /* Move the RCVD_ONLY path, if needed
     */
    if (rcvd_only_to_be_moved != BGP_NULL_INDEX) {
	bgp_delink_path(net, rcvd_only_to_be_moved);
	bgp_link_path(net, rcvd_only_to_be_moved, ix);
    }
}

/*
 * bgp_replace_path
 * Replace an existing path from a neighbor with a new one. 
 */
BGP_PRIVATE inline void 
bgp_replace_path (bnettype *net, bpathtype *path, bpathtype *pathinfo,
                  battrtype *attr, uchar ix)
{
    ulong saved_flag, old_MED;

    /*
     * As the paths are threaded, it is more expensive to do bcopy as
     * we need to preserve prev/next/dampinfo etc. Instead, copy the
     * fields that could change.
     * Becareful with the suppressed flag. 
     */
    path->gateway = pathinfo->gateway;
    path->weight = pathinfo->weight;

    old_MED = path->attr->metric;
    free(path->attr);
    path->attr = attr;
    mem_lock(attr);

    /* Keep the MEDLOSER bit value.  Then, if MED does change, it
     * will be updated properly later.
     */
    saved_flag = (path->flags & (BFLAG_SUPPRESSED | BFLAG_MEDLOSER));
    path->flags = pathinfo->flags;
    path->flags |= saved_flag;

    /* If MED has changed, move path (along with the RCVD_ONLY path) to the
     * right place based on MED.
     */
    if (bgp_determ_med && (old_MED != attr->metric)) {
	bgp_relocate_path(net, ix);
    }
}

/*
 * bgp_dinfo_free
 */
BGP_PRIVATE inline void
bgp_dinfo_free (bdampinfotype *dinfo)
{
    if (!dinfo)
	return;
    
    if (dinfo->refcount < 1) {
	errmsg(&msgsym(BADSHARE, SYS), "dinfo_free", dinfo, dinfo->refcount);
	return;
    }

    if (--dinfo->refcount)
	return;

    chunk_free(damp_chunks, dinfo);
}

/*
 * bgp_dinfo_lock
 *
 * This is not an inline as it will be locked only from a display chain.
 */
BGP_PRIVATE void
bgp_dinfo_lock (bdampinfotype *dinfo)
{
    if (dinfo->refcount < 1) {
	dinfo->refcount = 1;
	errmsg(&msgsym(BADSHARE, SYS), "dinfo_lock", dinfo, dinfo->refcount);
	return;
    }
    
    dinfo->refcount++;
}

/*
 * bgp_free_dampinfo
 */
BGP_PRIVATE void
bgp_free_dampinfo (bnettype *net, int i)
{
    bdampinfotype *dampinfo;
    
    dampinfo = net->paths[i].dampinfo;
    net->paths[i].dampinfo = NULL;
    net->paths[i].flags &= ~(BFLAG_SUPPRESSED+BFLAG_HISTORY);
    if (dampinfo->next)
	dampinfo->next->prev = dampinfo->prev;
    if (dampinfo->prev)
	dampinfo->prev->next = dampinfo->next;
    dampinfo->next = NULL;
    dampinfo->prev = NULL;
    dampinfo->net = NULL;
    dampinfo->dcb = NULL;
    bgp_dinfo_free(dampinfo);
}

/*
 * bgp_rechoose_best_MED_path
 *
 * Called after a path is deleted and the path is the best path in
 * term of MED value.  This routine will make sure that the next
 * best path is chosen (BFLAG_MEDLOSER gets cleared) or if path
 * with equal MED to the deleted one's exists, we simply bail.
 */
BGP_PRIVATE inline void
bgp_rechoose_best_MED_path (bnettype *net, int idx)
{
    int ix;
    bpathtype *path, *a_path;
    ushort asnumber, path_asnumber;
    ulong best_metric;
    battrtype *a_attr;

    path = &net->paths[idx];
    path_asnumber = path->attr->neighboras;

    /* If there is a path before this path and it is from the same
     * AS, then we don't need to rechoose a new best path because
     * this means that there is still some other path with the same
     * MED as the deleted path.
     */
    if (path->prev != BGP_NULL_INDEX) {
	asnumber = net->paths[path->prev].attr->neighboras;
	if (path_asnumber == asnumber)
	    return;
    }

    best_metric = METRIC_INACCESSIBLE;

    /* Scan down from the current path.  Since paths are already
     * sorted, the first valid path is the best so we only need to
     * scan down until we find the path that has worse MED than the
     * first one.
     */
    for (ix = idx; ix != BGP_NULL_INDEX; ix = net->paths[ix].next) {
	a_path = &net->paths[ix];
	if (a_path->flags & BFLAG_RCVD_ONLY)
	    continue;

	a_attr = a_path->attr;
	asnumber = a_attr->neighboras;
	if (path_asnumber != asnumber)
	    break;
	if (a_attr->metric > best_metric)
	    break;

	a_path->flags &= ~BFLAG_MEDLOSER;
	best_metric   =  a_attr->metric;
    }
}

/*
 * bgp_delete_path
 * As we walk the paths using linked list, deleting a path is simple.
 * Just clean the path structure and delink it from the path chain.
 *
 * Note : If the best path needs to be updated, bgp_bestpath should 
 * be called after bgp_delete_path. DO NOT TOUCH THE BEST FIELD.
 */

BGP_PRIVATE void
bgp_delete_path (bnettype *net, int i)
{
    bpathtype *path;
    int next_path;
    ushort asnumber, n_asnumber;

    if (!(net->pathcount > 0)) {
	errmsg(&msgsym(DELPATH, BGP), net->network, net->mask);
	return;
    }

    path = &net->paths[i];
    next_path = BGP_NULL_INDEX;

    if (bgp_determ_med) {
	/*
	 * If the path to be deleted is a USED path (RCVD_USED or a
	 * modified path) and has the best MED (i.e. MEDLOSER is
	 * not set), mark the next path so we will pick the next
	 * best path after the deletion.
	 */
	if ((path->flags & BFLAG_RCVD_USED) ||
	    !(path->flags & BFLAG_RCVD_ONLY)) {

	    if (!(path->flags & BFLAG_MEDLOSER) &&
		(path->next != BGP_NULL_INDEX)) {

		asnumber = path->attr->neighboras;
		n_asnumber = net->paths[path->next].attr->neighboras;
		if (asnumber == n_asnumber)
		    next_path = path->next;
	    }
	}
    }

    net->pathcount--;
    free(path->attr);
    path->attr = NULL;
    if (path->dampinfo)
	bgp_free_dampinfo(net, i);
    bgp_delink_path(net, i);

    /* Choose the next best path if needed
     */
    if (next_path != BGP_NULL_INDEX) {
	bgp_rechoose_best_MED_path(net, next_path);
    }

    /*
     * If all the paths are deleted, reset the BNET_XXX flags as this
     * net could be reused before bgp_scan_walker frees it.
     *
     * Do not blow away the BNET_SENT_IBGP and BNET_SENT_EBGP flags
     * as we need them in bgp_send_update to withdraw the route.
     */
    if (!net->pathcount)
	net->flags &= BNET_PRESERVE;
}

/*
 * bgp_find_usedpath
 *
 * Return the used path. Always, first path should be the used path.
 *
 * If the current path is received only and if the new path is valid,
 * return the current path after clearing all the flags. Flags are
 * cleared to avoid considering the previous received path as
 * duplicate and short circuiting the update process.
 *
 * If the current path is not received only, we could use the current
 * path for used path. Clear the received-used flag as the path might
 * change from received-used to just used. 
 */
BGP_PRIVATE inline uchar
bgp_find_usedpath (bnettype *net, ipaddrtype address, boolean valid_path)
{
    uchar i;

    for (i = net->firstpath; i != BGP_NULL_INDEX; i = net->paths[i].next) {
	if (net->paths[i].neighbor == address) {
	    if (net->paths[i].flags & BFLAG_RCVD_ONLY) {
		if (!valid_path)
		    return(BGP_NULL_INDEX);
		else
		    net->paths[i].flags = 0;
	    } else
		net->paths[i].flags &= ~BFLAG_RCVD_USED;
	    break;
	}
    }
    return(i);
}

/*
 * bgp_find_path
 */
BGP_PRIVATE inline uchar
bgp_find_path (bnettype *net, ipaddrtype address)
{
    uchar i;
    for (i = net->firstpath; i != BGP_NULL_INDEX; i = net->paths[i].next)
	if (net->paths[i].neighbor == address)
	    break;

    return(i);
}

/*
 * bgp_add_rcvdpath
 */
BGP_PRIVATE boolean
bgp_add_rcvdpath (bnettype *net, ipaddrtype network, ipaddrtype mask, 
		  uchar used_idx, bpathtype *pathinfo, battrtype *rcv_attr, 
		  ipaddrtype rcvd_nexthop, neighbortype *bgp, 
		  ulong changed) 
{
    ulong flag, saved_flag;
    int rcvd_idx;
    bpathtype *path;

    rcvd_idx = BGP_NULL_INDEX;
    if (used_idx != BGP_NULL_INDEX) {		/* path accepted */
	/*
	 * If path changed, see if there is another path from the same
	 * neighbor, if not we will create one later. 
	 * If no change, the received path and used path are the same.
	 */
	path = &net->paths[used_idx];
	if (changed) {
	    flag = BFLAG_RCVD_ONLY;
	    if ((path->next != BGP_NULL_INDEX) &&
		(net->paths[path->next].neighbor == bgp->address))
		rcvd_idx = path->next;
	} else { 
	    flag = BFLAG_RCVD_USED;
	    rcvd_idx = used_idx;
	}

    } else {				/* path not accepted */
	/*
	 * If net has not been passed down, find it. 
	 * If net is present, check if there is any path from this
	 * neighbor. 
	 */
	flag = BFLAG_RCVD_ONLY;
	if (!net)
	    net = bgp_netlookup(network, mask);

	if (!net) {
	    net = bgp_create_net(network, mask);
	    if (!net)
		return(FALSE);
	} else
	    rcvd_idx = bgp_find_path(net, bgp->address);
    }

    /*
     * If still no receive path, create one.
     * If used path is present, add after the used path.
     *
     * Note that we put flag in pathinfo->flag before calling bgp_add_path()
     * so that RCVD_ONLY path will be ignored in MED sorting.  Then, we
     * restore pathinfo->flag to the old value because routine that calls this
     * function may not expect to have the flag variable changes value.  It
     * doesn't matter now but we'd rather be safe than sorry.
     */
    if (rcvd_idx == BGP_NULL_INDEX) {
	saved_flag = pathinfo->flags;
	pathinfo->flags |= flag;
	net = bgp_add_path(net, pathinfo, rcv_attr, &rcvd_idx, used_idx,
			   BGP_NULL_INDEX);
	pathinfo->flags = saved_flag;
	if (!net)
	    return(FALSE);
    }
    
    /*
     * Update receive path.
     * Clear the received flags as the path might change from rcvd
     * only to rcvd and used.  If the path is rcvd only, clear the
     * unwanted fields and flags. 
     */

    path = &net->paths[rcvd_idx];
    path->flags &= ~(BFLAG_RCVD_ONLY|BFLAG_RCVD_USED);
    path->flags |=  flag;

    if (flag & BFLAG_RCVD_ONLY) {
	path->flags &= ~(BFLAG_HISTORY+BFLAG_SUPPRESSED+BFLAG_SYNCED);
	path->weight = 0;
	path->gateway = rcvd_nexthop;

	if (path->attr != rcv_attr) {
	    free(path->attr);
	    path->attr = rcv_attr;
	    mem_lock(rcv_attr);
	}
	
	if (path->dampinfo)
	    bgp_free_dampinfo(net, rcvd_idx);
    }

    /*
     * Make sure that the next path is not from this neighbor. It
     * could happen if the used path became unchanged or if a prevous
     * accepted path is rejected. 
     */
    if ((path->next != BGP_NULL_INDEX) &&
	(net->paths[path->next].neighbor == bgp->address))
	bgp_delete_path(net, path->next);

    return(TRUE);
}

/*
 * bgp_ospftag_check
 */
BGP_PRIVATE inline boolean
bgp_ospftag_check(ndbtype *ndb)
{
    /*
     * RFC 1403 suggests that we not re-import routes with pathlen
     * set to GE1 because they are learned via BGP (we are
     * ignoring the COMPLETE bit because bothe cases should fail)
     */
    if ((ndb->tag & (TAG_AUTOMATIC|TAG_PATHLENPART)) ==
	(TAG_AUTOMATIC|TAG_PATHLEN_GE1))
	return (FALSE);
    else
	return (TRUE);
}

/*
 * bgp_redist_check
 * Check to see if a network should be redistributed into BGP.  Return
 * FALSE if it should NOT be redistributed.
 */

BGP_PRIVATE boolean
bgp_redist_check (ndbtype *ndb)
{
    acl_headertype *acl;
    ipaddrtype mask;
    pdbtype *src_pdb;
    ndbtype *db;

    if (!ip_redist_flag(ndb, pdb_BGP))
	return (FALSE);

    if (ndb->attrflags & NET_NOREDIST)
	return (FALSE);

    if (NDB2PROCTYPE(ndb) == PROC_BGP)
	return (FALSE);

    if (ndb->metric == METRIC_INACCESSIBLE)
	return (FALSE);

    /*
     * Can't redistribute network 0.0.0.0 if default_originate is not set.
     * (but user can specify network 0.0.0.0/0.0.0.0 if they want)
     */
    if (!ndb->netnumber && !pdb_BGP->default_originate)
	return (FALSE);
    
    if (ndb->attrflags & (NET_SUBNET|NET_SUPERNET))
	mask = ndb->netsmask;
    else
	mask = get_majormask(ndb->netnumber);

    /*
     * In case of autosummarization, we want to apply the access-list to
     * the dummy ndb for backward compatability with earlier behaviour.
     */
    if (pdb_BGP->auto_summary && (ndb->attrflags & NET_SUBNET)) 
	db = net_lookup(get_majornet(ndb->netnumber), FALSE);
    else
	db = ndb;

    src_pdb = ip_get_src_pdb(pdb_BGP, ndb, FALSE);
    acl = pdb_BGP->ofltr_proto[src_pdb->index].acl;
    if (acl && !route_check(acl, db->netnumber, mask))
	return (FALSE);
    
    /*
     * Check OSPF specially.
     */
    if (src_pdb->proctype == PROC_OSPF) {
	if (!pdb_BGP->routemap[src_pdb->index] && 
	    !ospf_check_match_routetype(pdb_BGP, src_pdb, ndb))
	    return (FALSE);

	if (!bgp_ospftag_check(ndb))
	    return (FALSE);
    }
    if (!pdb_routemap(pdb_BGP, src_pdb, ndb))
	return (FALSE);

    return (TRUE);
}

/*
 * bgp_path2neighbor
 * Given a path, go find the neighbor.
 */

BGP_PRIVATE neighbortype *
bgp_path2neighbor (bpathtype *path)
{
    neighbortype *bgp;

    for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next) {
	if (path->neighbor == bgp->address)
	    return (bgp);
    }
    return (NULL);
}

/*
 * bgp_pathlength
 * Determine number of hops in a path attribute
 * (AS sets count as one hop).
 */

static inline int
bgp_pathlength (uchar *path, int len)
{
    int i;
    uchar seglen, type;

    i = 0;
    while (len > 0) {
	type = *path++;
	seglen = *path++;
	if (type == AS_SET)
	    i++;
	else if (type == AS_SEQUENCE)
	    i += seglen;
	/* else ASENTRY_SEQ and ASENTRY_SETs have no length */

	path += seglen * PATHSIZE;
	len  -= (seglen * PATHSIZE) + 2;
    }
    return (i);
}

/*
 * bgp_pathvalid
 *
 * Validate this BGP4 style AS path (not needed for BGP3)
 * Point to path (of len bytes)
 */

static inline boolean
bgp_pathvalid (uchar *path, int len)
{
    uchar seglen;		/* segment length in words */

    for (; len > 0; len -= 2) {
	switch(*path++) {
	case AS_SET:
	case AS_SEQUENCE:
	case ASENTRY_SEQUENCE:
	    seglen = *path++;

	    if ((seglen * PATHSIZE) > len)
		return(FALSE);

	    while ((seglen > 0) && (len > 0)) {
		seglen--;
		len  -= PATHSIZE;
		path += PATHSIZE;
	    }
	    break;
	default:
	    return (FALSE);
	}
    }
    return(TRUE);
}

/*
 * bgp_path2string
 * Given binary path data, return a pointer to a static ASCII string with
 * the path data in it.  This makes it easy to drive the regexp package.
 */

BGP_PUBLIC char *
bgp_path2string (uchar *path, int len, char *buf, int buflen)
{
    char  *strptr;
    uchar  seglen;
    uchar *savepath;
    int    savelen;
    uchar  buffer[60];
    
    savepath = path;
    savelen  = len;
    buflen -= ASCII_PATHSIZE;

    for (strptr = buf; len > 0; len -= 2) {
	switch(*path++) {
	case AS_SET:
	    strptr += sprintf(strptr, "{");
	    for (seglen = *path++; (seglen > 0) && (len > 0);
		 seglen--, len -= PATHSIZE, path += PATHSIZE) {
		if (strptr >= &buf[buflen])
		    goto overflow;
		strptr += sprintf(strptr, "%u,", GETSHORT(path));
	    }
	    strptr--;
	    strptr += sprintf(strptr, "} ");
	    break;
	case ASENTRY_SEQUENCE:
	    strptr += sprintf(strptr, "(");
	    for (seglen = *path++; (seglen > 0) && (len > 0);
		 seglen--, len -= PATHSIZE, path += PATHSIZE) {
		if (strptr >= &buf[buflen])
		    goto overflow;
		strptr += sprintf(strptr, "%u ", GETSHORT(path));
	    }
	    strptr--;
	    strptr += sprintf(strptr, ") ");
	    break;
	case AS_SEQUENCE:
	    for (seglen = *path++; (seglen > 0) && (len > 0);
		 seglen--, len -= PATHSIZE, path += PATHSIZE) {
		if (strptr >= &buf[buflen])
		    goto overflow;
		strptr += sprintf(strptr, "%u ", GETSHORT(path));
	    }
	    break;
	default:
	    if (bgp_debug) {
		bgp_notification_data(savepath, savelen, buffer,
				      sizeof(buffer));
		buginf("\nBGP: invalid AS path: %d bytes %s",
		       savelen, buffer);
	    }
	    return (NULL);
	}
    }
  overflow:
    if (strptr > buf)
	strptr--;
    *strptr = '\0';
    return (buf);
}

/*
 * bgp_path2originas
 *
 * given a BGP4 path, return a vector containing the origin AS or AS set
 * used to do prtraceroute
 */
 
BGP_PRIVATE int
bgp_path2originas (uchar *path, int len, ushort *buf, int buflen)
{
    ushort *as;
    uchar  seglen;
  
    as = buf;

    while (len > 0) 
	switch(*path++) {
	case AS_SET:
	    seglen = *path++;
	    len -= 2;
	    while ((seglen > 0) && (len > 0)) {
		if (seglen * PATHSIZE == len)  {
		    if (as >= &buf[buflen])
			goto overflow;
		    *as++ = GETSHORT(path);
		}
		seglen--;
		len -= PATHSIZE;
		path += PATHSIZE;
	    }
	    break;
	case AS_SEQUENCE:
	case ASENTRY_SEQUENCE:
	    seglen = *path++;
	    len -= 2;
	    while ((seglen > 0) && (len > 0)) {
		if ((seglen * PATHSIZE == len) && (len == PATHSIZE))
		    *as++ = GETSHORT(path);
		seglen--;
		len -= PATHSIZE;
		path += PATHSIZE;
	    }
	    break;
	default:
	    return (0);
	}
	
  overflow:
	return (as - buf);
}

/*
 * bgp_path_union
 * Given two AS paths,  merge the second one into the first, converting
 * the first into an AS set if necessary.  This routine should be expanded
 * to try to develop common prefix and common suffix merging when possible.
 * (but we do it better than everyone else right now, so there)
 * Overwrites aspath,  and returns size of new merged path structure.
 */
 
BGP_PRIVATE int
bgp_path_union (uchar *aspath, int aslength, uchar *newpath, int newlen)
{
    ushort *mp, *mergedend;
    int len, mergedlen, seglen;
    uchar *cp, type;
    static ushort mergedpath[BGP_PATHBUFSIZE];

    /* if existing path is empty, the union is the new path */
    if (aslength == 0) {
	bcopy(newpath, aspath, newlen);
	return (newlen);
    }

    /* if new path is null or same as existing AS path, no changes necessary */
    if (newlen == 0 || !bcmp(aspath, newpath, aslength))
	return (aslength);

    /*
     * First, copy out existing AS path as a single AS set
     */
    mergedend = mergedpath;
    for (len = aslength, cp = aspath; len > 0; len -= 2) {
	 type = *cp++;		/* step over AS path type */
	 for (seglen = *cp++;
	      (seglen > 0) && (len > 0) &&
	      (mergedend < &mergedpath[BGP_PATHBUFSIZE]);
	      seglen--, len -= PATHSIZE, cp += PATHSIZE)
	     if (type != ASENTRY_SEQUENCE)
		 *mergedend++ = *(ushort *)cp;
    }

    /*
     * now go through each element of new path and see if it's part of
     * existing AS set, if not tack it on to the end
     */
    for (len = newlen, cp = newpath; len > 0; len -= 2) {
	type = *cp++;		/* step over AS path type */
	for (seglen = *cp++;
	     (seglen > 0) && (len > 0) &&
	     (mergedend < &mergedpath[BGP_PATHBUFSIZE]);
	     seglen--, len -= PATHSIZE, cp += PATHSIZE) {
	    if (type != ASENTRY_SEQUENCE) {
		for (mp = mergedpath; mp < mergedend; mp++)
		    if (*mp == *(ushort *)cp)
			break;
		if (mp == mergedend)
		    *mergedend++ = *(ushort *)cp;
	    }
	}
    }

    /* OK, we've added anything we needed to add */
    mergedlen = mergedend - mergedpath;
    *aspath++ = AS_SET;
    *aspath++ = mergedlen;
    mergedlen *= PATHSIZE;
    bcopy(mergedpath, aspath, mergedlen);

    return (mergedlen + 2);
}

/* 
 * bgp_route_map_match_routetype
 *
 * Match route type from routing table entry with match list.
 * This is a bgp specific version of the similar redistribute match command.
 */

BGP_PRIVATE boolean
bgp_route_map_match_routetype (bpathtype *path,
			       route_map_match_value *match_list)
{
    route_map_match_value *value;

    /*
     * Nothing in list, therefore it matches.
     */
    if (!match_list) {
	return(TRUE);
    }

    /*
     * If any of the values in the list match, return TRUE.
     */
    for (value = match_list; value; value = value->next) {
	switch (value->value) {
	case ROUTE_MAP_INTERNAL:
	    if (path->flags & BFLAG_INTERNAL)
		return(TRUE);
	    break;
	case ROUTE_MAP_EXTERNAL:
	    if (!(path->flags & (BFLAG_INTERNAL|BFLAG_SOURCED|BFLAG_AGGREGATE)))
		return(TRUE);
	    break;
	case ROUTE_MAP_LOCAL:
	    if (path->flags & (BFLAG_SOURCED|BFLAG_AGGREGATE))
		return(TRUE);
	    break;
	default:
	    continue;
	}
    }
    return(FALSE);
}

/*
 * route_map_match_community
 * 
 * Match BGP communities with community-list entry.
 */
static boolean 
route_map_match_community (ulong *community, int count,
			   route_map_match_value *match_list, ulong match_flag)
{
    route_map_match_value *value;
    boolean exact;

    /*
     * Nothing in list, therefore it matches.
     */
    if (!match_list) {
	return(TRUE);
    }

    /*
     * Do community match with the community list number in the match, 
     * return TRUE if matched.
     */
    exact = (match_flag & ROUTE_MAP_MATCH_EXACT_COMMUNITY);
    for (value = match_list; value; value = value->next) {
	if (ip_community_check(community, count, value->value, exact))
	    return(TRUE);
    }
    return(FALSE);
}

/*
 * bgp_filter_list_check
 * Filter-list check on an attribute
 * If the cached entry is present, use it. Or-else perform check and 
 * cache the result.
 */
BGP_PRIVATE boolean
bgp_filter_list_check (battrtype *attr, int list, uchar *pathstr)
{
    bgp_filter_list_ctype *p;

    /*
     * Check if there is a cached entry.
     */
    p = bgp_filter_list_cache[attr->hashbucket];
    for (; p; p = p->next)
	if ((p->attr == attr) && (p->flist == list)) {
	    return(p->grant);
	}

    p = chunk_malloc(fcache_chunks);
    if (!p)
	return(FALSE);
    p->flist = list;
    p->attr = attr;
    p->grant = regexp_accesscheck(pathstr, list, as_path_accesslists);
    p->next = bgp_filter_list_cache[attr->hashbucket];
    bgp_filter_list_cache[attr->hashbucket] = p;
    return(p->grant);
}

/*
 * bgp_route_map_check
 *
 * Apply route_map specified by caller to contents of an net/as path.
 * This function returns the route-map, if all match criteria are met for a 
 * given permit route-map. It returns NULL, for a deny route-map. When all 
 * route-map entries are processed and a match has not been satisfied, the 
 * funtion returns NULL.
 */
BGP_PRIVATE route_maptype *
bgp_route_map_check (route_map_nametype *routemap, battrtype *attr, 
		     bnettype *net, ipaddrtype network, ipaddrtype mask,
		     uchar *as_path, boolean donet)
{
    route_maptype       *map, *first_attrmap;
    route_map_headtype  *head;
    route_map_matchtype *match;
    bpathtype 	        *path;
    bgp_route_map_ctype *rcache;
    boolean             netmatch, attrmatch, skip_attrmatch;
    
    skip_attrmatch = FALSE;
    first_attrmap = NULL;
    rcache = NULL;

    head = routemap->head;
    if (!head)
	return(NULL);

    netmatch = (donet && (head->match_flags & ROUTE_MAP_BGP_NETWORK_MATCHES));
    attrmatch = (head->match_flags & ROUTE_MAP_BGP_ATTRIBUTE_MATCHES);

    /*
     * No match clause. Return the first map as result.
     */
    if (!netmatch && !attrmatch) {
	map = (route_maptype *) head->map_queue.qhead;
	if (map && map->grant)
	    return(map);
	else
	    return(NULL);
    }

    /*
     * If the route-map has attribute matching, check the cache. 
     * If it is attribute match only, return the cached result.
     * If it is network match, use the cached map to start the check. 
     * If the cached result is a deny (map == NULLL) return.
     */
    if (attrmatch) {
       rcache = bgp_route_map_cache[attr->hashbucket];
	for (; rcache; rcache = rcache->next)
	    if ((rcache->attr == attr) && (rcache->head == head)) {
		break;
	    }

       if (rcache && (!netmatch || !rcache->map))
	   return(rcache->map);
    }

    if (rcache) {
	map = rcache->map;
	skip_attrmatch = TRUE;
    } else {
	map = (route_maptype *) head->map_queue.qhead;
    }

    for (; map; map = map->next) {
	match = &(map->match_list);

	/*
	 * If attribute match is allowed, match as_path against
	 * aspath access-list and community against community-list.
	 *
	 * During combined match, the first map that pass the
	 * attribute match is cached. This helps in avoiding the
	 * initial attribute match.
	 */
	if (attrmatch && !skip_attrmatch) {
	    if (as_path && 
		(match->match_flags & ROUTE_MAP_MATCH_ASPATH) &&
		!route_map_match_aspath(as_path, match->as_path))
		continue;

	    if ((match->match_flags & ROUTE_MAP_MATCH_COMMUNITY) &&
		!route_map_match_community(attr->community, attr->comcount, 
					   map->match_list.community,
					   map->match_list.match_flags))
		continue;
	}

	skip_attrmatch = FALSE;

	if (netmatch) {
	    if (!first_attrmap)
		first_attrmap = map;

	    /*
	     * Match net number against IP access-list.
	     */
	    if ((match->match_flags & ROUTE_MAP_MATCH_IPADDRESS) &&
		!route_map_match_ipaddress(network, mask, match->ip_address))
		continue;

	    if (net && 
		(net->best != BGP_NO_BEST)) {
		path = &net->paths[net->best];
		
		/*
		 * Match next hop against IP access-list.
		 */
		if ((match->match_flags & ROUTE_MAP_MATCH_IPNEXTHOP) &&
		    !route_map_match_ipaddress(path->gateway, 0,
					       match->ip_next_hop))
		    continue;
		
		/*
		 * Match route source against IP access-list.
		 */
		if ((match->match_flags & ROUTE_MAP_MATCH_IPROUTESOURCE) &&
		    !route_map_match_ipaddress(path->neighbor, 0,
					       match->ip_route_source))
		    continue;
		
		/*
		 * Match route type
		 */
		if ((match->match_flags & ROUTE_MAP_MATCH_ROUTETYPE) &&
		    !bgp_route_map_match_routetype(path, match->route_type))
		    continue;
	    }
	}

	/*
	 * If no map was found with full matches, or one was found with a
	 * deny action, return FALSE.
	 */
	if (!map->grant)
	    map = NULL;

	break;
    }

    /*
     * If it is attribute-filtering, cache the results.
     */
    if (attrmatch && !rcache) {
	rcache = chunk_malloc(rcache_chunks);
	if (!rcache)
	    return(NULL);
	
	rcache->attr = attr;
	rcache->head = head;
	rcache->map = first_attrmap ? first_attrmap : map;
	rcache->next = bgp_route_map_cache[attr->hashbucket];
	bgp_route_map_cache[attr->hashbucket] = rcache;
    }

    return(map);
}

/*
 * bgp_com_union
 * Utility function to merge the first array into the second array.
 * Return the merged arrays count.
 */
BGP_PRIVATE int
bgp_com_union (ulong *new, int new_count, ulong *main,
	       int main_count, int max_count)
{
    int i, merged_count;
    
    /* If existing community is empty, the union is the new community */
    if (main_count == 0) {
	bcopy(new, main, COMMUNITY_LEN(new_count));
	return (new_count);
    }


    /* If new community is null or same as main, no change necessary. */
    if (new_count == 0 || (new_count == main_count && 
			   !bcmp(main, new, COMMUNITY_LEN(main_count))))
	return (main_count);

    merged_count = main_count;
    for (i = 0; (i < new_count) && (merged_count < max_count); ++i)
	sorted_array_add(main, &merged_count, new[i]);

    return(merged_count);
}

/*
 * bgp_special_community
 * Check for special community and set the appropriate flags in
 * pathinfo.
 */
BGP_PRIVATE inline boolean
bgp_special_community (ulong *community, int comcount, 
		       bpathtype *pathinfo)
{
    int i;
    boolean special;
    
    special = FALSE;
    pathinfo->flags &= ~(BFLAG_NOEXPORT+BFLAG_NOADV+BFLAG_LOCALAS);
    for (i = 0; i < comcount; ++i) {
	if (community[i] == COMMUNITY_NOEXPORT) {
	    pathinfo->flags |= BFLAG_NOEXPORT;
	    special = TRUE;
	} else if (community[i] == COMMUNITY_NOADV) {
	    pathinfo->flags |= BFLAG_NOADV;
	    special = TRUE;
	} else if (community[i] == COMMUNITY_LOCALAS) {
	    pathinfo->flags |= BFLAG_LOCALAS;
	    special = TRUE;
	}
    }
    return(special);
}

/*
 * bgp_get_routemap_setmetric
 * Get the metric from the route-map.
 */
BGP_PRIVATE inline boolean
bgp_get_routemap_setmetric (route_maptype *map, ulong *metric)
{
    ulong value;
    value = map->set_list.metric.value.scalar;

    if (map->set_list.flags & ROUTE_MAP_METRIC_ADD) {
	*metric += value;
    } else if (map->set_list.flags & ROUTE_MAP_METRIC_SUB) {
	if (value >= *metric)
	    *metric = 0;
	else
	    *metric -= value;
    } else {
	*metric = value;
    }
    
    if (*metric)
	return(TRUE);
    else
	return(FALSE);
}

/*
 * bgp_route_map_rcv_set
 */
BGP_PRIVATE boolean
bgp_route_map_rcv_set (neighbortype *bgp, route_maptype *map,
		       battrtype *attrinfo, bpathtype *pathinfo, 
		       uchar **aspathptr, ulong *changed)
{
    uchar *aspath, *asptr;
    int len, comcount, aslength;
    ulong *community, cflag;
    route_map_addrtype *aptr;

    cflag = 0;
    aspath = *aspathptr;
    aslength = attrinfo->aslength;
    community = attrinfo->community;
    comcount = attrinfo->comcount;

    if (map->set_list.set_flags & ROUTE_MAP_LOCALPREF_SET) {
	attrinfo->localpref = map->set_list.local_pref;
	pathinfo->flags |= BFLAG_LOCALPREF;
	cflag |= BGP_ATTRINFO_CHANGED;
    }

    if ((map->set_list.set_flags & ROUTE_MAP_METRIC_SET) &&
	!map->set_list.metric.igrp_metric) {
	if (bgp_get_routemap_setmetric(map, &attrinfo->metric))
	    pathinfo->flags |= BFLAG_METRIC;
	else
	    pathinfo->flags &= ~BFLAG_METRIC;
	cflag |= BGP_ATTRINFO_CHANGED;
    }

    if (map->set_list.set_flags & ROUTE_MAP_WEIGHT_SET) {
	pathinfo->weight = map->set_list.weight;
	cflag |= BGP_ATTRINFO_CHANGED;
    }
    
    if (map->set_list.set_flags & ROUTE_MAP_NEXT_HOP_SET) {
	for (aptr = map->set_list.next_hop; aptr; aptr = aptr->next) {
	    if (aptr->address.type != ADDR_IP)
		continue;
	    pathinfo->gateway = aptr->address.ip_addr;
	    cflag |= BGP_ATTRINFO_CHANGED;
	    break;
	}
    }

    if (map->set_list.set_flags & ROUTE_MAP_ORIGIN_SET) {
	attrinfo->origin = map->set_list.origin;
	cflag |= BGP_ATTRINFO_CHANGED;
    }
    
    if (!bgp->bgp_internal && (bgp->protocol_version > 3) &&
	(map->set_list.set_flags & ROUTE_MAP_ASPATH_PND_SET) &&
	map->set_list.as_path) {
	
	len = map->set_list.ascount * PATHSIZE;
	if ((aslength + len) > BGP_PATHBUFLEN) {
	    errmsg(&msgsym(ASPATH, BGP), "Invalid",
		   bgp_path2string(aspath, attrinfo->aslength,
				   bgp_router_path_buffer,
				   ASCII_PATHBUFSIZE),
		   bgp->address, "AS-path prepend exceeds the limit");
	    return(FALSE);
	}
	*aspathptr = asptr = bgp_set_aspath_buffer;
	attrinfo->aslength += len;

	*asptr++ = aspath[0];
	*asptr++ = aspath[1] + map->set_list.ascount;
	bcopy(map->set_list.as_path, asptr, len);
	bcopy(&aspath[2], asptr + len, aslength - 2); /* skip prelude */
	cflag |= BGP_ASPATH_CHANGED;
    }

    /*
     * Apply the route-map to the communities.  
     * Use the community variable. As it is bgp_router_com_buffer,
     * there is enough space to hold new communities.
     */
    if (map->set_list.community) {
	if (map->set_list.set_flags & ROUTE_MAP_COMMUNITY_ADD) {
	    comcount = bgp_com_union(map->set_list.community->com_array,
				     map->set_list.community->count,
				     community, comcount,
				     BGP_MAXCOMMUNITY);
	} else {
	    comcount = map->set_list.community->count;
	    bcopy(map->set_list.community->com_array, community,
		  COMMUNITY_LEN(comcount));
	}

	bgp_special_community(community, comcount, pathinfo);

	attrinfo->comcount = comcount;
	if (comcount)
	    pathinfo->flags |= BFLAG_COMMUNITY;
	else 
	    pathinfo->flags &= ~BFLAG_COMMUNITY;
	cflag |= BGP_COMMUNITY_CHANGED;
    }

    *changed = cflag;
    return(TRUE);
}

/*
 * bgp_nonmap_path_policy
 * Do non route-map path policy check on the path.
 */
BGP_PRIVATE inline boolean
bgp_nonmap_path_policy (neighbortype *bgp, uchar *pathstr, 
			battrtype *rcv_attr, ushort *weightptr)
{
    ushort weight, asystem;
    astype *as;
    bweightinfo *weightexp;
    uchar *cp;
    int seglen, len;

    weight = bgp->bgp_weight;		/* start with neighbor weight */

    if (bgp->bgp_inlist &&
	!bgp_filter_list_check(rcv_attr, bgp->bgp_inlist, pathstr))
	return(FALSE);

    for (weightexp = (bweightinfo *) bgp->bgp_weightdata.qhead; weightexp;
	 weightexp = weightexp->next)
	if (regexp_accesscheck(pathstr, weightexp->access_list,
			       as_path_accesslists)) {
	    weight = weightexp->weight;
	    break;
	}

    /* obsolete filter-as command ... ditch by 1995 */
    if (!QUEUEEMPTY(&bgp->bgp_asdata)) {
	cp = rcv_attr->as_path;
	len = rcv_attr->aslength;
	for (; len > 0; len -= 2) {
	    cp++;			/* skip over type */
	    for (seglen = *cp++;
		 (seglen > 0) && (len > 0);
		 seglen--, len -= PATHSIZE, cp += PATHSIZE) {
		asystem = GETSHORT(cp);
		for (as = (astype *) bgp->bgp_asdata.qhead; as;
		     as = as->next) {
		    if (as->number == asystem) {
			if (!as->grant)
			    return (FALSE);
			weight += as->weight;
			break;
		    }
		}
	    }
	}
    }

    if (weight)
	*weightptr = weight;

    return(TRUE);
}

/*
 * bgp_path_policy
 * Return a policy pronouncement on this path.
 * comptr will be set to point to the resultant community.
 * Return TRUE to accept the path, FALSE to reject it entirely.
 */

BGP_PRIVATE boolean
bgp_path_policy (neighbortype *bgp, uchar **aspathptr, bpathtype *pathinfo,
		 battrtype *attrinfo, battrtype *rcv_attr, ulong *changed)
{
    route_maptype *map;
    uchar *pathstr;

    pathstr = bgp_path2string(rcv_attr->as_path, rcv_attr->aslength, 
			      bgp_router_path_buffer, ASCII_PATHBUFSIZE);
    if (!pathstr)
	return (FALSE);

    /*
     * First do non route-map path policy
     */
    if (!bgp_nonmap_path_policy(bgp, pathstr, rcv_attr, &pathinfo->weight))
	return(FALSE);

    
    /*
     * If inbound route-map is specified, do route-map path policy.
     */
    if (bgp->bgp_routemapin) {
	map = bgp_route_map_check(bgp->bgp_routemapin, rcv_attr, NULL, 
				  0, 0, pathstr, FALSE);
	if (!map ||
	    !bgp_route_map_rcv_set(bgp, map, attrinfo, pathinfo, aspathptr,
				  changed))
	    return(FALSE);
    }

    /*
     * Check for special well-known communities
     */
    if (bgp_special_community(attrinfo->community, attrinfo->comcount, 
			      pathinfo))
	*changed |= BGP_COMMUNITY_CHANGED;

    return (TRUE);
}

/*
 * bgp_best_metric
 *
 * Quickly (!) find the IGP metric to the destination.
 *
 * If not in the cache, call ip_best_metric to compute it and then
 * store it in the cache.
 *
 * The cache is reset to scratch before each periodic bgp_nettable_scan
 * so we don't have to worry about updating it any faster than that.
 */

BGP_PRIVATE inline ulong
bgp_best_metric_inline (ipaddrtype destination)
{
    bgpgateway *p;
    uchar hashcode;
    ulong metric;

    /* gateways of 0.0.0.0 are local */
    if (!destination)
	return 0;

    hashcode = nethash(destination);

    for (p = bgp_gwcache[hashcode]; p; p = p->next)
	if (p->address == destination)
	    return (p->metric);

    metric = ip_best_metric(destination);
    p = malloc(sizeof(bgpgateway));
    if (p) {
	 p->address = destination;
	 p->metric = metric;
	 p->next = bgp_gwcache[hashcode];
	 bgp_gwcache[hashcode] = p;
    }
    return (metric);
}

BGP_PRIVATE ulong
bgp_best_metric (ipaddrtype destination)
{
    return (bgp_best_metric_inline(destination));
}


/*
 * bgp_reset_gwcache
 *
 * Clear out all entries in the best metric cache.
 *
 * Must not be called at the same time as bgp_best_metric is adding to
 * this data structure.
 */

BGP_PRIVATE void
bgp_reset_gwcache (void)
{
    int i;
    bgpgateway *p, *p_next;

    for (i = 0; i < NETHASHLEN; i++) {
	p = bgp_gwcache[i];
	bgp_gwcache[i] = NULL;
	while (p) {
	    p_next = p->next;
	    free(p);
	    p = p_next;
	}
    }
}

/*
 * bgp_compare_med
 * Check if MED comparision is allowed
 */
BGP_PRIVATE inline boolean
bgp_compare_med (bpathtype *bestpath, bpathtype *path)
{
    ushort path_nextas, best_nextas;

    /* 
     * If a MED is not set, the value will be zero and therefore paths
     * with no MED attribute are more desirable than a path with a
     * nonzero MED. Return TRUE so that comparision will do the right
     * thing 
     */	

   if (bgp_always_compare_med || path->attr->aslength == 0)
	return(TRUE);
 
    /*
     * If the paths are received from neighbors in the same AS, MED is
     * comparable.  
     */

    path_nextas = path->attr->neighboras;
    best_nextas = bestpath->attr->neighboras;

    if (path_nextas && best_nextas && 
	(path_nextas == best_nextas))
	return(TRUE);
    else
	return(FALSE);
}

/*
 * bgp_bestpath
 * Compute the best path to use for a particular network.
 * "last" argument points to the path we just changed.  Bump the table version
 * number if this last change altered our best path information.
 * TODO: If our best external is not the same as our best, flood our best
 * external to all internals. */

BGP_PRIVATE void
bgp_bestpath (bnettype *net, int last, boolean bump_version)
{
    ndbtype *ndb;
    rdbtype *rdb;
    neighbortype *bgp;
    int i, best, rdb_index;
    int mpathbest, mpathcount;
    uchar mpatharray[BGP_MAXPATHS];
    boolean debug;
    bpathtype *path, *bestpath;
    ulong bestpathlocal, pathlocal;
    ulong bestgwmetric, pathgwmetric;
    ushort bestas, pathas;

    debug = bgpupdate_acl_debug(net->network, net->mask);

   /*
     * Clear the multipath flag
     */
    mpathbest = BGP_NULL_INDEX;
    net->flags &= ~BNET_MULTIPATH;

    if (bgp_multipath_enabled)
	memset(mpatharray, BGP_NULL_INDEX, sizeof(uchar)*net->pathsmalloced);

    /*
     * As first guess at best path, find first valid path.
     */
    bestpath = NULL;
    for (best = net->firstpath; best != BGP_NULL_INDEX; best = bestpath->next){
	bestpath = &net->paths[best];
	if (bgp_pathok(bestpath)) {
	    if (bgp_best_metric_inline(bestpath->gateway) <
		METRIC_INACCESSIBLE)
		break;
	}
    }

    /*
     * No valid paths.  Clear net->best so the ager will
     * flush this network and its path information.
     */
    if (best == BGP_NULL_INDEX || !bestpath) {
	if (net->best != BGP_NO_BEST) {
	    bgp_bump_net_version(net);
	    net->best = BGP_NO_BEST;
	}

	if (debug)
	    buginf("\nBGP: no valid path for %i%m", net->network, net->mask);
	return;
    }

    /*
     * If we're synchronized with the IGP, then mark the path.
     */

    if (!pdb_BGP->sync_disabled &&
	(bestpath->flags & BFLAG_INTERNAL) &&
	!(bestpath->flags & BFLAG_SYNCED)) {
	ndb = ip_rtlookup(net->network, net->mask);
	if (ndb && !(NDB2PROCTYPE(ndb) & PROC_BGP)) {
	    bgp = bgp_path2neighbor(bestpath);
	    if (bgp && (bgp->protocol_version > 2) &&
		(NDB2PROCTYPE(ndb) & PROC_OSPF)) {
		for (i = 0; i < ndb->routecount; i++) {
		    rdb = ndb->rdb[i];
		    if (rdb && (bgp->bgp_router_id == rdb->infosource)) {
			bestpath->flags |= BFLAG_SYNCED;
			break;
		    }
		}
	    } else 
		bestpath->flags |= BFLAG_SYNCED;
	}
    }
    
    /*
     * Scan rest of list looking for a better candidate.
     */
    for (i = bestpath->next; i != BGP_NULL_INDEX; i = path->next) {
	bestpath = &net->paths[best];
	path = &net->paths[i];
	
	if (!bgp_pathok(path))
	    continue;

	/*
	 * If we're synchronized with the IGP, then mark the path.
	 */
	if (!pdb_BGP->sync_disabled &&
	    (path->flags & BFLAG_INTERNAL) &&
	    !(path->flags & BFLAG_SYNCED)) {
	    ndb = ip_rtlookup(net->network, net->mask);
	    if (ndb && !(NDB2PROCTYPE(ndb) & PROC_BGP)) {
		bgp = bgp_path2neighbor(path);
		if (bgp && (bgp->protocol_version > 2) &&
		    (NDB2PROCTYPE(ndb) & PROC_OSPF)) {
		    for (rdb_index = 0; rdb_index < ndb->routecount;
			 rdb_index++) {
			rdb = ndb->rdb[rdb_index];
			if (rdb && (bgp->bgp_router_id == rdb->infosource)) {
			    path->flags |= BFLAG_SYNCED;
			    break;
			}
		    }
		} else 
		    path->flags |= BFLAG_SYNCED;
	    }
	}

	/*
	 * Check the next hop metric, if it's not valid, don't select
	 */
	pathgwmetric = bgp_best_metric_inline(path->gateway);
	if (pathgwmetric >= METRIC_INACCESSIBLE)
	    continue;
	
	/*
	 * If synchronization is enabled and paths are internal,
	 * prefer the synchronized path.
	 */
	if (!pdb_BGP->sync_disabled && 
	    (bestpath->flags & BFLAG_INTERNAL) && 
	    (path->flags & BFLAG_INTERNAL)) {
	    if (!(bestpath->flags & BFLAG_SYNCED) &&
		(path->flags & BFLAG_SYNCED)) {
		best = i;
		continue;
	    } else if ((bestpath->flags & BFLAG_SYNCED) && 
		       !(path->flags & BFLAG_SYNCED))
		continue;
	}


	/*
	 * Let weight factors dominate since they are under local control.
	 * Heaviest (greatest) weight wins.
	 */
	if (bestpath->weight < path->weight) {
	    best = i;
	    continue;
	} else if (bestpath->weight > path->weight)
	    continue;

	/*
	 * Next, look at local preference values
	 * Biggest number wins.  If a particular route doesn't
	 * have a local pref, use the user configurable default.
	 */
	bestpathlocal = (bestpath->flags & BFLAG_LOCALPREF) ?
	    bestpath->attr->localpref : bgp_default_localpref;
	pathlocal = (path->flags & BFLAG_LOCALPREF) ?
	    path->attr->localpref : bgp_default_localpref;

	if (bestpathlocal < pathlocal) {
	    best = i;
	    continue;
	} else if (bestpathlocal > pathlocal)
	    continue;

	/*
	 * Locally generated paths are better than learned paths
	 */
	if (path->flags & (BFLAG_SOURCED|BFLAG_AGGREGATE)) {
	    best = i;
	    continue;
	} else if (bestpath->flags & (BFLAG_SOURCED|BFLAG_AGGREGATE))
	    continue;

	/*
	 * Next policy is to use the one with shortest path length.
	 * If we already have a shorter path, continue with next candidate.
	 */
	if (bestpath->attr->pathhops > path->attr->pathhops) {
	    best = i;
	    continue;
	} else if (bestpath->attr->pathhops < path->attr->pathhops)
	    continue;

	/*
	 * Same length paths.
	 * Prefer lowest origin (IGP < EGP < INCOMPLETE)
	 */
	if (bestpath->attr->origin > path->attr->origin) {
	    best = i;
	    continue;
	} else if (bestpath->attr->origin < path->attr->origin)
	    continue;

	/* At this point, path that has lost the MED comparison to
	 * other path from the same AS cannot be compared.  The idea
	 * is: path that wins on MED among all paths from the same AS
	 * is the representative of the group.  Ignore this though if
	 * bgp_always_compare_med is TRUE because all paths are now
	 * fully comparable.
	 */
	if (!bgp_always_compare_med && (path->flags & BFLAG_MEDLOSER))
	    continue;

	/*
	 * Same length paths.
	 * If we have the same neighboring AS (could be ourselves!)
	 * then check the multi exit discriminator to break ties.
	 *
	 * We will respect a given AS's MED over our own IGP/external
	 * decision and also we are now less rigid and will respect the MED
	 * MED if just the neighbor is the same (we used to demand that the
	 * paths were identical).  This is a change in behavior from BGP3.
	 */
	if (bgp_compare_med(bestpath, path)) {
	    if (bestpath->attr->metric > path->attr->metric) {
		best = i;
		continue;
	    } else if (bestpath->attr->metric < path->attr->metric)
		continue;
	}
	

	/*
	 * Same length paths.
  	 * Prefer external links over internal links.
 	 * Always consider Confederation paths as internal paths.
	 * Avoid loops of internal information.
	 */
	if (bestpath->flags & (BFLAG_INTERNAL+BFLAG_CONFED)) {
	    if (!(path->flags & (BFLAG_INTERNAL+BFLAG_CONFED))) {
		best = i;
		continue;
	    }
	} else  if (path->flags & (BFLAG_INTERNAL+BFLAG_CONFED))
	    continue;

	/*
	 * If we got this far, everything looks pretty symmetric.
	 * Consider IGP information.  What we're looking for here is
	 * the shortest distance across our AS, which is consistent
	 * with selecting an external path over an internal path.
	 *
	 * What this means is that if an external AS has specified a
	 * preferred exit gateway, we will respect that over our own
	 * wishes.  This is a change from BGP3.
	 */

	bestgwmetric = bgp_best_metric_inline(bestpath->gateway);
	if (bestgwmetric < pathgwmetric)
	    continue;
	else if (pathgwmetric < bestgwmetric) {
	    best = i;
	    continue;
	}

	/*
	 * Check if the new path is eligible for multipath. If both
	 * are external paths from same AS the paths are eligible for
	 * multipath. Note the current best in the mpath array. Later,
	 * if the current best turns out to be the overall best, we
	 * could do multipath.  Do not change the best if multipath. 
	 */

	if (bgp_multipath_enabled &&
	    !(bestpath->flags & BFLAG_NOTEXTERNAL) &&
	    !(path->flags & BFLAG_NOTEXTERNAL)) {

	    bestas = *((ushort *)(bestpath->attr->as_path)+1);
	    pathas = *((ushort *)(path->attr->as_path)+1);

	    if (bestas == pathas) {
		mpatharray[i] = best;
		mpathbest = best;
		continue;
	    }
	}

	/*
	 * Same length paths.
	 * Tie breaker is the peer with smaller router ID.
	 */
	if ((ulong) bestpath->router_id > (ulong) path->router_id)
	    best = i;

    }

    /*
     * If multipath is allowed, walk all the paths and do multipath
     * processing.  If the overall best is same as mpathbest, then
     * there are candidates for multipath. All the paths with the
     * corresponding mpatharray index set to mpathbest are multipath
     * candiates.
     *
     * If a path changes from multipath to normal path, bump the
     * version number to trigger ip routing table update. 
     */

     if (bgp_multipath_enabled) {

	mpathcount = 0;
	if ((best != BGP_NULL_INDEX) &&
	    (mpathbest == best)) {
	    net->flags |= BNET_MULTIPATH;
	    mpatharray[best] = mpathbest;
	}

	for (i = net->firstpath; i != BGP_NULL_INDEX; i = path->next) {
	    path = &net->paths[i];
	    
	    if ((net->flags & BNET_MULTIPATH) && 
		(mpatharray[i] == mpathbest) &&
		(mpathcount < pdb_BGP->multipath)) {

		mpathcount++;
		if (!(path->flags & BFLAG_MULTIPATH)) {
		    bump_version = TRUE;
		    path->flags |= BFLAG_MULTIPATH;
		}

		if (debug)
		    buginf("\nBGP: Multipath for %i%m from %i", net->network,
			   net->mask, path->neighbor);
			   
	    } else if (path->flags & BFLAG_MULTIPATH) {

		bump_version = TRUE;
		path->flags &= ~BFLAG_MULTIPATH;

	    }
	}

	if ((last != BGP_NULL_INDEX) &&
	    (net->paths[last].flags & BFLAG_MULTIPATH))
	    bump_version = TRUE;
    }

    /*
     * We change the version number of this entry if we changed the path we
     * are using or if the old path was changed/updated in some manner.
     * The BGP routing process will send an update about the new best path. 
     */
    if (bump_version || (best != net->best) || (best == last)) {
	bgp_bump_net_version(net);
    }

    net->best = best;
}

/*
 * bgp_nosource
 * Remove the sourced paths.  Assumes that there is only one such path.
 */

BGP_PRIVATE void
bgp_nosource (bnettype *net)
{
    int i;
    bpathtype *path;

    for (i = net->firstpath; i != BGP_NULL_INDEX; i = path->next) {
	path = (bpathtype *) &net->paths[i];
	if ((path->flags & BFLAG_RCVD_ONLY) ||
	    !(path->flags & BFLAG_SOURCED))
	    continue;
	bgp_delete_path(net, i);
	if (net->best == i)
	    bgp_bestpath(net, i, TRUE);
	return;
    }
}

/*
 * bgp_unsync
 * Unsynchronize a network.
 * Also, remove any sourced paths.
 */

BGP_PRIVATE void
bgp_unsync (bnettype *net)
{
    int i;
    bpathtype *path;

    if (pdb_BGP->sync_disabled)
	return;
    for (i = net->firstpath; i != BGP_NULL_INDEX; i = path->next) {
	path = (bpathtype *) &net->paths[i];
	if (path->flags & BFLAG_RCVD_ONLY)
	    continue;
	if (!(path->flags & BFLAG_INTERNAL))
	    continue;
	if (!(path->flags & BFLAG_SYNCED))
	    continue;
	path->flags &= ~BFLAG_SYNCED;
	if (net->best == i)
	    bgp_bestpath(net, i, TRUE);
    }
}

/*
 * bgp_find_attr
 * Find an attribute record in the current table, or create a new one.
 */
BGP_PRIVATE battrtype *
bgp_find_attr (battrtype *attr, uchar *aspath, ulong *community)
{
    int hashvalue, i, comlength, rrinfolength, align, unalign_bytes;
    uchar *path, *cp;
    battrtype *curr;

    rrinfolength = 0;
    align = unalign_bytes = 0;
    comlength = COMMUNITY_LEN(attr->comcount);
    path = aspath;
    hashvalue = attr->origin ^ attr->metric;
    for (i = attr->aslength / 2; i; i--) {
	hashvalue ^= GETSHORT(path);
	path += PATHSIZE;
    }
    hashvalue = nethash(hashvalue);
    for (curr = bgppathtable[hashvalue]; curr; curr = curr->next) {
	if ((attr->aslength != curr->aslength) ||
	    (attr->comcount != curr->comcount) ||
	    (attr->origin != curr->origin) ||
	    (attr->metric != curr->metric) ||
	    (attr->localpref != curr->localpref) ||
 	    (attr->destpref != curr->destpref) ||
 	    (attr->destpref_as != curr->destpref_as) ||
	    (attr->aggregator.as != curr->aggregator.as) ||
	    (attr->aggregator.ip != curr->aggregator.ip) ||
	    (attr->translen != curr->translen) ||
	    (attr->rrinfo && !curr->rrinfo) ||
	    (!attr->rrinfo && curr->rrinfo))
	    continue;
	if (bcmp(aspath, &curr->as_path, curr->aslength))
	    continue;
	if (bcmp(community, curr->community, comlength))
	    continue;
	if (bcmp(attr->transptr, curr->transptr, curr->translen))
	    continue;
	if (attr->rrinfo &&			/* rrinfo present */
	    ((attr->rrinfo->originator != curr->rrinfo->originator) ||
	     bcmp(attr->rrinfo->clusterlist, curr->rrinfo->clusterlist,
		  attr->rrinfo->clusterlength)))
	    continue;
	break;
    }
    if (curr)
	return (curr);

    /*
     * There isn't an attribute record for this, so go generate one.
     * Space should be allocated for aspath, optional (not supported)
     * transitive attribute, community attribute and rrinfo
     *
     * As community and RR info come after the aspath, special care is
     * required to make they start at the longword boundary. Allocate
     * the required padding and start the community and RR info at
     * appropriate offset.
     */
    if (attr->rrinfo || comlength) {
	unalign_bytes = (sizeof(battrtype) + attr->aslength) % sizeof(ulong);
	if (unalign_bytes)
	    align = sizeof(ulong) - unalign_bytes;
    }

    if (attr->rrinfo)
	rrinfolength = sizeof(rrinfotype) + attr->rrinfo->clusterlength;

    curr = malloc(sizeof(battrtype) + attr->aslength + attr->translen + 
		  comlength + rrinfolength + align);
    if (!curr) {
	errmsg(&msgsym(NOMEMORY, BGP), "attribute");
	bgp_reset(NULL, NULL);
	return NULL;
    }

    bcopy(attr, curr, sizeof(battrtype));
    bcopy(aspath, &curr->as_path, attr->aslength);
    cp = curr->as_path + curr->aslength + align;
    if (comlength) {
	curr->community = (ulong *)cp;
	cp += comlength;
	bcopy(community, curr->community, comlength);
    }
    if (attr->rrinfo) {
	curr->rrinfo = (rrinfotype *)cp;
	cp += rrinfolength;
	bcopy(attr->rrinfo, curr->rrinfo, rrinfolength);
    }
    if (curr->translen) {
	curr->transptr = cp;
	cp += curr->translen;
	bcopy(attr->transptr, curr->transptr, curr->translen);
    }

    curr->neighboras = bgp_attr2neighboras(curr);
    curr->pathhops = bgp_pathlength(curr->as_path, curr->aslength);

    curr->hashbucket = hashvalue;		/* used for cache lookups */
    curr->next = bgppathtable[hashvalue];
    bgppathtable[hashvalue] = curr;
    return (curr);
}

/*
 * bgp_calculate_origin
 * Calculate the origin, weight, and AS path.
 * In case of auto summarization, target_ndb could be different from ndb.
 */

BGP_PRIVATE void
bgp_calculate_origin (battrtype *attr, bpathtype *path, ulong *community,
		      ndbtype *ndb, ndbtype *target_ndb, boolean localnetflag,
		      ushort weight)
{
    int asfixup, i;
    ipaddrtype gate;
    uchar *p;
    route_maptype *map;
    boolean weight_set;
    ndbtype *sdb;
    
    /*
     * Clear community related info. If needed, we will set them later.
     */
    attr->comcount = 0;
    path->flags &= ~(BFLAG_METRIC+BFLAG_LOCAL+BFLAG_LOCALPREF+
		     BFLAG_DESTPREF+BFLAG_COMMUNITY);
    asfixup = 0;
    attr->aslength = 0;
    weight_set = FALSE;

    /* 
     * If we are auto summarizing and looking at a subnet, then donot
     * set the gateway from the rdb instead it should be 0.0.0.0
     */
    if (target_ndb) {
	if (!(pdb_BGP->auto_summary && (target_ndb->attrflags & NET_SUBNET)))
	    for (i = 0; i < target_ndb->routecount; i++) {
		if (target_ndb->rdb[i] == NULL)
		    continue;
		gate = target_ndb->rdb[i]->gateway;
		if (gate && samecable(gate))
		    path->gateway = gate;
	    }
	else
	    path->gateway = 0;

	if (ip_def_metric_configured(pdb_BGP, target_ndb->pdbindex))
	    attr->metric = ip_get_def_metric_scalar(pdb_BGP, 
						    target_ndb->pdbindex);
	else {
	    if (ndb->attrflags & NET_SUBNETTED) {
		sdb = subnet_lookup(ndb, ndb->netnumber, ndb->netsmask);
		if (sdb)
		    attr->metric = sdb->metric;
		else
		    attr->metric = 0;
	    } else
		attr->metric = ndb->metric;

	    path->flags |= BFLAG_METRIC;
	}
    }

    if (localnetflag) {
	attr->origin = ORIGIN_IGP;
	path->neighbor = 0;
	path->router_id = 0;
	path->flags |= BFLAG_LOCAL;
    } else if (ndb && (NDB2PROCTYPE(ndb) & PROC_EGP)) {
	asfixup = INDEX2PDB(ndb->pdbindex)->asystem;
	/*
	 * if AS is zero, we are using core egp and should default
	 * to the tag
	 */
	if (asfixup == 0)
	    asfixup = ndb->tag & MAXUSHORT;
	attr->origin = ORIGIN_EGP;
    } else if (ndb && (NDB2PROCTYPE(ndb) & PROC_OSPF) &&
	    (ndb->tag & TAG_AUTOMATIC)) {
	/* we're dealing with an automaticly generated tag */
	if (ndb->tag & TAG_COMPLETE) {
	    switch(ndb->tag & TAG_PATHLENPART) {
	    case TAG_PATHLEN_0:
		attr->origin = ORIGIN_IGP;
		path->neighbor = 0;
		path->router_id = 0;
		break;
	    case TAG_PATHLEN_1:
		attr->origin = ORIGIN_IGP;
		asfixup = ndb->tag & TAG_ASPART;
		break;
	    case TAG_PATHLEN_GE1:
		if (bgp_debug)
		    buginf("\nBGP: route %i feedback from OSPF",
			   ndb->netnumber);
		attr->origin = ORIGIN_INCOMPLETE;
		break;
	    }
	} else {
	    switch(ndb->tag & TAG_PATHLENPART) {
	    case TAG_PATHLEN_0:
		attr->origin = ORIGIN_EGP;
		break;
	    case TAG_PATHLEN_1:
		attr->origin = ORIGIN_EGP;
		asfixup = ndb->tag & TAG_ASPART;
		break;
	    case TAG_PATHLEN_GE1:
		if (bgp_debug)
		    buginf("\nBGP: route %i feedback from OSPF",
			   ndb->netnumber);
		attr->origin = ORIGIN_INCOMPLETE;
		break;
	    }
	}
    } else
	attr->origin = ORIGIN_INCOMPLETE;

    /*
     * Get weight value configured in route-map, if any -- this overrides
     * all other entries.
     */
    map = pdb_BGP->route_map;
    if (map) {
	if (map->set_list.set_flags & ROUTE_MAP_ORIGIN_SET) {
	    attr->origin = map->set_list.origin;
	    if (attr->origin == ORIGIN_EGP)
		asfixup = map->set_list.origin_as;
	}
	
	if (map->set_list.set_flags & ROUTE_MAP_ASPATH_TAG_SET)
	    asfixup = ndb->tag;
	
	if (map->set_list.set_flags & ROUTE_MAP_WEIGHT_SET) {
	    weight_set = TRUE;
	    weight = map->set_list.weight;
	}

	if ((map->set_list.set_flags & ROUTE_MAP_METRIC_SET) &&
	    !map->set_list.metric.igrp_metric) {
	    attr->metric = map->set_list.metric.value.scalar;
	    path->flags |= BFLAG_METRIC;
	}

	if (map->set_list.set_flags & ROUTE_MAP_LOCALPREF_SET) {
	    attr->localpref = map->set_list.local_pref;
	    path->flags |= BFLAG_LOCALPREF;
	}

	if (map->set_list.set_flags & ROUTE_MAP_DESTPREF_SET) {
	    attr->destpref = map->set_list.destpref;
	    attr->destpref_as = pdb_BGP->asystem;
	    path->flags |= BFLAG_DESTPREF;
	}

	if ((map->set_list.set_flags & ROUTE_MAP_NEXT_HOP_SET) &&
	    map->set_list.next_hop &&
	    (map->set_list.next_hop->address.type == ADDR_IP))
	    path->gateway = map->set_list.next_hop->address.ip_addr;

	/*
	 * Set the community attribute.
	 */
	if (map->set_list.community && 
	    map->set_list.community->count) {
	    attr->comcount = map->set_list.community->count;
	    bcopy(map->set_list.community->com_array, community,
		  COMMUNITY_LEN(map->set_list.community->count));
	    path->flags |= BFLAG_COMMUNITY;
	    bgp_special_community(community, attr->comcount, path);
	}
    }

    if (asfixup && asfixup != pdb_BGP->asystem) {
	attr->aslength = PATHSIZE+2;
	p = attr->as_path;
	*p++ = AS_SEQUENCE;
	*p++ = 1;			/* one entry */
	PUTSHORT(p, asfixup);
    }
    path->weight = weight_set ? weight : BGP_SOURCED_WEIGHT;
}

/*
 * bgp_update_origin
 * Update the origin and AS path.  If localnetflag is set because we've
 * specified the network via a "network" command, then there is no path
 * yet and the origin is IGP.  Anything else has either an INCOMPLETE
 * origin (and no path) or EGP origin (use AS of EGP process).
 * Return TRUE if something changed. A returned value of FALSE could
 * indicate the failure of attribute creation.
 */

BGP_PRIVATE boolean
bgp_update_origin (ndbtype *ndb, ndbtype *target_ndb, bnettype *net, 
		   int pathindex, boolean localnetflag, ushort weight)
{
    bpathtype pathinfo, *path;
    boolean changed;
    battrtype *tmp;
    int comlength;

    changed = FALSE;
    path = &net->paths[pathindex];
    comlength = COMMUNITY_LEN(path->attr->comcount);

    /*
     * Copy the information from the path->attr structure into the
     * premalloced bgp_edit_attr_buffer.
     * No need to copy the community attribute. The only way to set it
     * is via route-map, and it could be got by passing a buffer to
     * calculate_origin.
     * Same for AS ???
     */
    bcopy(path, &pathinfo, sizeof(bpathtype));
    bcopy(path->attr, bgp_edit_attr_buffer, 
	  sizeof(battrtype) + path->attr->aslength);
    bgp_calculate_origin(bgp_edit_attr_buffer, &pathinfo, bgp_edit_com_buffer,
			 ndb, target_ndb, localnetflag, weight);

    if (bcmp(&pathinfo, path, sizeof(bpathtype))) {
	changed = TRUE;
	bcopy(&pathinfo, path, sizeof(bpathtype));
    }
    if (bcmp(bgp_edit_attr_buffer, path->attr, 
	     sizeof(battrtype) + bgp_edit_attr_buffer->aslength) ||
	bcmp(bgp_edit_com_buffer, path->attr->community, comlength)) {
	changed = TRUE;
	tmp = bgp_find_attr(bgp_edit_attr_buffer,
			    bgp_edit_attr_buffer->as_path,
			    bgp_edit_com_buffer); 
	if (tmp) {
	    free(path->attr);
	    path->attr = tmp;
	    mem_lock(path->attr);
	}else 
	    return(FALSE);
    }
    return (changed);
}

/*
 * bgp_setorigin
 * Set the origin and AS path.  If localnetflag is set because we've
 * specified the network via a "network" command, then there is no path
 * yet and the origin is IGP.  Anything else has either an INCOMPLETE
 * origin (and no path) or EGP origin (use AS of EGP process).
 */

BGP_PRIVATE void
bgp_setorigin (ndbtype *ndb, ndbtype *target_ndb, bnettype **rnet, int *i,
	       boolean localnetflag, ushort weight)
{
    bpathtype pathinfo;
    battrtype *attr;
    bnettype *net;

    net = *rnet;
    memset(&pathinfo, 0, sizeof(bpathtype));
    memset(bgp_edit_attr_buffer, 0, sizeof(battrtype) + BGP_PATHBUFLEN);
    bgp_calculate_origin(bgp_edit_attr_buffer, &pathinfo, bgp_edit_com_buffer,
			 ndb, target_ndb, localnetflag, weight);
    attr = bgp_find_attr(bgp_edit_attr_buffer, bgp_edit_attr_buffer->as_path,
			 bgp_edit_com_buffer); 
    if (attr) 
	*rnet = bgp_add_path(net, &pathinfo, attr, i, BGP_NULL_INDEX,
			     BGP_NULL_INDEX);
    else
	*rnet = NULL;
}

/*
 * bgp_edit
 * Given an ndb, update a BGP entry.  If "localnetflag" is set, this is one
 * of the networks that we source from this AS (IGP origin).  If it is not set,
 * we will advertise the network with EGP or INCOMPLETE origin.
 * Note, don't hold pointers to the net structure and then call bgp_edit.
 * The pointers may change on you.
 * In case of auto summarization, tdb could be different from ndb. This will 
 * allow the use of route-metric and tag from the subnet.
 */

BGP_PRIVATE void
bgp_edit (ndbtype *ndb, ndbtype *target_ndb, boolean localnetflag, 
	  ushort weight)
{
    ipaddrtype mask;
    bnettype *net;
    int i, next;
    boolean validflag;

    /*
     * Ignore illegal stuff (belt & suspenders)
     * If this route is a supernet, just check to make sure it is
     * routable in class A/B/C space, otherwise tighten the check
     * to actually insure it's not a martian network or subnet.
     */
    if ((ndb->attrflags & NET_SUPERNET) ?
	CLASSD_AND_GREATER(ndb->netnumber) : martian(ndb->netnumber, FALSE))
	return;

    /*
     * Set flag as to whether this network is reachable via the IGP.
     */
    validflag = (target_ndb->metric < METRIC_INACCESSIBLE);
    if (NDB2PROCTYPE(target_ndb) & PROC_BGP)
	validflag = FALSE;

    if (ndb->attrflags & NET_SUBNETTED)
	mask = get_majormask(ndb->netnumber);
    else
	mask = ndb->netsmask;

    /*
     * Scan BGP table for the network we should be sourcing.
     */
    net = bgp_netlookup(ndb->netnumber, mask);
    if (net) {
	/*
	 * If our best path is the sourced path, do further checking.
	 * Otherwise we need to create the sourced path.
	 */
 	for (i = net->firstpath; i != BGP_NULL_INDEX; i = next) {
 	    next = net->paths[i].next;
	    if ((net->paths[i].flags & BFLAG_SOURCED)) {
		/*
		 * If the network is now invalid, invalidate path and
		 * update our peers.  Otherwise, make sure the sourced
		 * networks have proper origins.
		 */
		if (!validflag) {
		    if (bgpupdate_acl_debug(net->network, net->mask))
			buginf("\nBGP: delete invalid sourced route for"
			       " %i path %d", net->network, i);
		    bgp_delete_path(net, i);
		    if (net->best == i)
			bgp_bestpath(net, i, TRUE);
		} else {
		    if (bgp_update_origin(ndb, target_ndb, net, i, 
					  localnetflag, weight)) {
			bgp_bestpath(net, i, FALSE);
			if (bgpupdate_acl_debug(net->network, net->mask))
			    buginf("\nBGP: sourced route for %i%m "
				   "path #%d changed (weight %u)",
				   net->network, net->mask, i,
				   net->paths[i].weight);
		    }
		}
		return;
	    }
	}
	/*
	 * We get here if:
	 * - we don't have a best path.
	 * - our best path was not through (sourced by) us.
	 */
    }
    
    /*
     * Didn't find a matching path, create a new one
     * unless the new entry will be invalid (why bother?).
     */
    if (!validflag)
	return;

    if (!net) {
	net = bgp_create_net(ndb->netnumber, mask);
	if (!net) {
	    errmsg(&msgsym(NOMEMORY, BGP), "path");
	    bgp_reset(NULL, NULL);
	    return;
	}
    }

    bgp_setorigin(ndb, target_ndb, &net, &i, localnetflag, weight);
    if (!net)
	return;				/* out of memory */
    net->paths[i].flags |= (BFLAG_VALID+BFLAG_ORIGIN+BFLAG_ASPATH+
			    BFLAG_NEXTHOP+BFLAG_SOURCED);
    bgp_bestpath(net, i, FALSE);
}

/*
 * bgp_free_parent_alist
 */
BGP_PRIVATE inline void
bgp_free_parent_alist (battrtype *attr)
{
    attrlist_entrytype *ptr, *nptr;

    ptr = attr->alist;
    attr->alist = NULL;
    while (ptr) {
	free(ptr->attr);	/* decrement the refcount of the parent */
	nptr = ptr->next;
	free(ptr);
	ptr = nptr;
    }
}

/*
 * bgp_extract_community
 * Extract and store the received communities in a sorted order in
 * community.
 */
BGP_PRIVATE inline boolean
bgp_extract_community (char *data, int count, ulong *community, uchar *error)
{
    ulong number;
    int index, i, j, k;

    /*
     * Store the communities as sorted entries as it will speed the
     * exact community MATCH.
     * If same community appear twice, it is error.
     * (This code is ugly. Need to change it)
     */
    index = 0;
    for (i = 0; i < count; ++i, data +=4) {
	number = GETLONG(data);
	for (j = 0; j < index; ++j) {
	    if (number == community[j]) {
		*error = BNOT_UPDATE_MALFORMED;
		return(FALSE);
	    } else if (number > community[j]) {
		continue;
	    }
	    for (k = index; k > j; --k)
		community[k] = community[k-1];
	    break;
	}
	community[j] = number;
	++index;
    }
    return(TRUE);
}

/*
 * bgp_find_set_attr
 * Front-end to bgp_find_attr used by bgp3/4_rcv_updates
 *
 * In-order to speed the in-bound message processing, the attribute
 * information received in the updates are cached. Due to the in-bound-set 
 * criteria, the actual attribute added to the attribute-table could
 * be different. If it is different, add the parent attribute to the
 * parent-list and increment the refcount of the parent attribute.
 */
BGP_PRIVATE inline battrtype *
bgp_find_set_attr (battrtype *set_attr, uchar *aspath, battrtype *rcv_attr)
{

    battrtype *attr;
    attrlist_entrytype *p;

    attr = bgp_find_attr(set_attr, aspath, set_attr->community);
    if (!attr)
	return(NULL);

    if (rcv_attr != attr) {
	for (p = attr->alist; p; p = p->next)
	    if (p->attr == rcv_attr)
		break;

	if (!p) {			/* parent attr not present. Add it. */
	    p = malloc(sizeof(attrlist_entrytype));

	    if (!p) {
		errmsg(&msgsym(NOMEMORY, BGP), "attribute list entry");
		bgp_reset(NULL, NULL);
		return NULL;
	    }
	    
	    p->attr = rcv_attr;
	    p->next = attr->alist;
	    attr->alist = p;
	    mem_lock(rcv_attr);		/* will be freed when attr is freed */
	}
    }
    return(attr);
}

/*
 * bgp3_rcv_update
 * Handle a received update message from a BGP2/3 peer
 */

BGP_PRIVATE void
bgp3_rcv_update (neighbortype *bgp, uchar *data, unsigned bytes)
{
    ipaddrtype network, mask;
    bnettype *net;
    bpathtype pathinfo, *path;
    battrtype attrinfo, *attr, *rcv_attr;
    uchar *cpi, *cpj;
    ushort pathlen, len, errlen, aspathlen;
    ushort target, asystem;
    uchar flags, saveflag, code;
    uchar error, *errptr, *aspath;
    boolean truncated, do_bestpath, deleted_best;
    int i, j, numnets, count;
    uchar *savedata;
    unsigned savebytes;
    ulong changed;

    savedata = data; savebytes = bytes;
    rcv_attr = NULL;

    /*
     * Compute attribute length and the number of networks
     */
    path = NULL;
    pathlen = GETSHORT(data);
    data += sizeof(ushort);
    bytes -= sizeof(ushort);

    numnets = ((bytes - pathlen) / sizeof(ipaddrtype));

    /*
     * Make sure a reasonable number of bytes and that
     * the number of network bytes is modulo 4.
     */
    if (((bytes - pathlen) & 3) != 0) {
	if (bgp_debug)
	    buginf("\nBGP: %i bad update length, path length %d, bytes %d",
		   bgp->address, pathlen, bytes + sizeof(ushort)); 
	bgp_send_notification(bgp, BNOT_UPDATE, BNOT_UPDATE_MALFORMED,
			      data, bytes);
	return;
    }

    /*
     * Scan the path attributes and copy into pathinfo data structure.
     * Use the bgp_router_com_buffer to store the commuity information.
     * (Helpful to add new commuity via route-map).
     */
    rcv_attr = NULL;
    error = 0;
    truncated = FALSE;
    aspath = NULL;
    aspathlen = 0;
    memset(&pathinfo, 0, sizeof(bpathtype));
    memset(&attrinfo, 0, sizeof(battrtype));
    attrinfo.community = bgp_router_com_buffer;
    pathinfo.flags |= BFLAG_VALID;
    pathinfo.neighbor = bgp->address;
    pathinfo.router_id = bgp->bgp_router_id;
    if (bgp->bgp_internal)
	pathinfo.flags |= BFLAG_INTERNAL;
    bytes = pathlen;
    while ((bytes > 0) && !error) {
	errptr = data;
	saveflag = *data++;
	flags = (saveflag & UPF_USED);
	code = *data++;
	if (flags & UPF_EXTENDED) {
	    flags &= ~UPF_EXTENDED;
	    len = GETSHORT(data);
	    data += sizeof(ushort);
	    if (len > bytes) {
		if (bgp_debug)
		    buginf("\nBGP: %i bad extended length, told %d got %d",
			   bgp->address, len, bytes);
		error = BNOT_UPDATE_MALFORMED;
		len = bytes;
	    }
	    bytes -= 4;
	    errlen = len + 4;
	} else {
	    len = *data++;
	    if (len > bytes) {
		if (bgp_debug)
		    buginf("\nBGP: %i bad attribute length, told %d got %d",
			   bgp->address, len, bytes);
		error = BNOT_UPDATE_MALFORMED;
		len = bytes;
	    }
 	    bytes -= 3;
	    errlen = len + 3;
	}
	if (error) {
	    bgp_send_notification(bgp, BNOT_UPDATE, error, errptr, errlen);
	    goto free_and_return;
	}
	switch (code) {
	case ATT3_ORIGIN:
	    if (flags != UPF_TRANSIT) {
		error = BNOT_UPDATE_FLAGS;
		break;
	    }
	    if (pathinfo.flags & BFLAG_ORIGIN) {
		if (bgp_debug)
		    buginf("\nBGP: %i duplicate origin attribute",
			   bgp->address); 
		error = BNOT_UPDATE_MALFORMED;
		break;
	    }
	    if (len != 1) {
		error = BNOT_UPDATE_LENGTH;
		break;
	    }
	    attrinfo.origin = *data;
	    if (attrinfo.origin > ORIGIN_INCOMPLETE)
		error = BNOT_UPDATE_ORIGIN;
	    else
		pathinfo.flags |= BFLAG_ORIGIN;
	    break;

	case ATT3_PATH:
	    if (flags != UPF_TRANSIT) {
		error = BNOT_UPDATE_FLAGS;
		break;
	    }
	    if (pathinfo.flags & BFLAG_ASPATH) {
		if (bgp_debug)
		    buginf("\nBGP: %i duplicate AS path attribute",
			   bgp->address);
		error = BNOT_UPDATE_MALFORMED;
		break;
	    }
	    if (len & 1) {
		error = BNOT_UPDATE_LENGTH;
		break;
	    }
	    attrinfo.aslength = 0;
	    if (len) {
		attrinfo.aslength = len + 2;
		aspathlen = len + 2;
		aspath = bgp_rcvd_aspath_buffer;

		/* convert this into a BGP4 path sequence */
		aspath[0] = AS_SEQUENCE;
		aspath[1] = len >> 1;
		bcopy(data, &aspath[2], len);
  	    }

	    /*
	     * Now we check for an AS routing loop.
	     * Make sure that the next and succeeding AS numbers do not
	     * match the current AS number. Ignore update if it has our
	     * AS number anywhere.
	     */
	    cpi = data;
	    count = len >> 1;
	    for (i = 0; i < count; i++, cpi += PATHSIZE) {
		asystem = GETSHORT(cpi);
		if ((asystem == pdb_BGP->asystem) ||
		    (bgp_confed_id && (asystem == bgp_confed_id))) {
		    pathinfo.flags &= ~BFLAG_VALID;
		    break;
		}
		cpj = cpi + PATHSIZE;
		for (j = i+1; j < count; j++, cpj += PATHSIZE) {
		    target = GETSHORT(cpj);
		    if (target == asystem)
			error = BNOT_UPDATE_LOOP;
		}
	    }

	    pathinfo.flags |= BFLAG_ASPATH;
	    break;

	case ATT3_NEXTHOP:
	    if (flags != UPF_TRANSIT) {
		error = BNOT_UPDATE_FLAGS;
		break;
	    }
	    if (pathinfo.flags & BFLAG_NEXTHOP) {
		if (bgp_debug)
		    buginf("\nBGP: %i duplicate next hop attribute",
			   bgp->address);
		error = BNOT_UPDATE_MALFORMED;
		break;
	    }
	    if (len != sizeof(ipaddrtype)) {
		error = BNOT_UPDATE_LENGTH;
		break;
	    }
	    bcopy(data, &pathinfo.gateway, sizeof(ipaddrtype));
	    if (martian(pathinfo.gateway, TRUE)) {
		if (bgp_debug)
		    buginf("\nBGP: %i martian next hop %i",
			   bgp->address, pathinfo.gateway); 
		error = BNOT_UPDATE_NEXTHOP;
	    } else
		pathinfo.flags |= BFLAG_NEXTHOP;
	    if (!samecable(pathinfo.gateway)) {
		if (!bgp->bgp_internal) 
		    pathinfo.gateway = bgp->address;
	    } else {
		if (ip_up_ouraddress(pathinfo.gateway)) {
		    pathinfo.flags &= ~BFLAG_VALID;
		    if (bgpupdate_debug)
			buginf("\nBGP: %i ignoring update, self as"
			       " next hop", bgp->address); 
		}
	    }
	    break;

	case ATT3_UNREACHABLE:
	    if (flags != UPF_TRANSIT) {
		error = BNOT_UPDATE_FLAGS;
		break;
	    }
	    if (len == 0)
		pathinfo.flags &= ~BFLAG_VALID;
	    else
		error = BNOT_UPDATE_LENGTH;
	    break;

	case ATT3_METRIC:
	    if (flags != UPF_OPTIONAL) {
		error = BNOT_UPDATE_FLAGS;
		break;
	    }
	    if (pathinfo.flags & BFLAG_METRIC) {
		if (bgp_debug)
		    buginf("\nBGP: %i duplicate metric attribute",
			   bgp->address);
		error = BNOT_UPDATE_MALFORMED;
		break;
	    }
	    if (len != 2) {
		error = BNOT_UPDATE_LENGTH;
		break;
	    }
	    attrinfo.metric = GETSHORT(data);
	    pathinfo.flags |= BFLAG_METRIC;
	    break;

	case ATT3_COMMUNITY:
	    if ((flags != UPF_TRANSIT+UPF_OPTIONAL) &&
		(flags != UPF_TRANSIT+UPF_OPTIONAL+UPF_PARTIAL)) {
		error = BNOT_UPDATE_FLAGS;
		break;
	    }

	    if (pathinfo.flags & BFLAG_COMMUNITY) {
		if (bgp_debug)
		    buginf("\nBGP: %i duplicate COMMUNITY path attribute",
			   bgp->address);
		error = BNOT_UPDATE_MALFORMED;
		break;
	    }

	    if ((len & 1) || (len == 0)) {
		error = BNOT_UPDATE_MALFORMED;
		break;
	    }

	    count = COMMUNITY_COUNT(len);
	    if (count > BGP_MAXCOMMUNITY) {
		if (bgp_debug)
		    buginf("\nBGP: %i COMMUNITY attribute with more than %d"
			   " communities", bgp->address, BGP_MAXCOMMUNITY);
		error = BNOT_UPDATE_LENGTH;
		break;
	    }

	    /*
	     * Extract the community attributes and place it in the
	     * attrinfo structure. (ie bgp_router_com_buffer)
	     */
	    if (!bgp_extract_community(data, count, attrinfo.community,&error))
		break;
	    
	    attrinfo.comcount = count;
	    pathinfo.flags |= BFLAG_COMMUNITY;
	    break;

	default:
	    /*
	     * Error if unrecognized attribute is mandatory
	     */
	    if ((flags & UPF_OPTIONAL) == 0) {
		if (bgp_debug)
		    buginf("\nBGP: %i unrecognized mandatory option %d",
			   bgp->address, code);
		error = BNOT_UPDATE_FLAGS;
		break;
	    }

	    /*
	     * Quietly ignore unrecognized, non-transit attributes
	     */
	    if ((flags & UPF_TRANSIT) == 0)
		break;

	    /*
	     * Record and pass along unrecognized transit attributes.
	     * Set partial bit to indicate we didn't update attribute.
	     * 
	     * Use errlen as we need to include the length of the Tag and 
	     * the Length field while storing the TLV.
	     */
	    *errptr |= UPF_PARTIAL;
	    if (attrinfo.translen == 0) {
		attrinfo.transptr = malloc(errlen);
		if (!attrinfo.transptr) 
		    break;
		attrinfo.translen = errlen;
	        bcopy(errptr, attrinfo.transptr, errlen);
	    } else {
		cpi = attrinfo.transptr;
		attrinfo.transptr = malloc(errlen + attrinfo.translen);
		if (!attrinfo.transptr) {
		    attrinfo.transptr = cpi;
		    break;
		}
		bcopy(cpi, attrinfo.transptr, attrinfo.translen);
		bcopy(errptr, attrinfo.transptr+attrinfo.translen, errlen);
		attrinfo.translen += errlen;
		free(cpi);
	    }
	    break;
	}

	/*
	 * Check for errors, else advance to next path attribute.
	 */
	if (error) {
	    bgp_send_notification(bgp, BNOT_UPDATE, error, errptr, errlen);
	    if (bgp_debug)
		bgp_printmsg("bgp3 rcv update error", savedata, savebytes);
	    goto free_and_return;
	}
	data += len;
	bytes -= len;
    }

    /*
     * All done parsing.  Check for truncation and for all mandatory items.
     */
    if ((pathinfo.flags & BFLAG_REQUIRED) != BFLAG_REQUIRED) {
	if (bgp_debug) {
	    buginf("\nBGP: %i update missing attributes, flags = %#x",
		   bgp->address, pathinfo.flags);
	}
	bgp_send_notification(bgp, BNOT_UPDATE, BNOT_UPDATE_MISSING, NULL, 0);
	goto free_and_return;
    }

    /*
     * Add the received attribute entry to the attribute_table. This
     * will help in caching the route-map/filter-list results.
     * Apply policies to this path.
     */
    rcv_attr = bgp_find_attr(&attrinfo, aspath, attrinfo.community);
    if (!rcv_attr)
	return;
    mem_lock(rcv_attr);

    changed = 0;
    if (!bgp_path_policy(bgp, &aspath, &pathinfo, &attrinfo,
			 rcv_attr, &changed))
	pathinfo.flags &= ~BFLAG_VALID;

    /*
     * Scan the network list
     */
    attr = NULL;
    for (; numnets > 0; numnets--, data += sizeof(ipaddrtype)) {
	/*
	 * Peel off a network and run martian check.
	 */
	do_bestpath = FALSE;
	deleted_best = FALSE;
	bcopy(data, &network, sizeof(ipaddrtype));
	network = get_majornet(network);
	mask = get_majormask(network);

	if (network && martian(network, FALSE)) {
	    if (bgp_debug)
		buginf("\nBGP: %i rcv martian network %i", bgp->address,
		       network);
	    continue;
	}	

	/*
	 * See if we are allowed to hear about this network from this peer.
	 */
	if (bgp->bgp_distlistin) {
	    if (!route_check(bgp->bgp_distlistin, network, mask))
	        continue;
	}

	/*
	 * First time in the for loop, create an attribute entry in the 
	 * table if the path information is valid.
	 */
	if (!attr && (pathinfo.flags & BFLAG_VALID)) {
	    if (changed)
		attr = bgp_find_set_attr(&attrinfo, aspath, rcv_attr);
	    else
		attr = rcv_attr;

	    if (!attr) {
		goto free_and_return;
	    } else
		mem_lock(attr);
	}

	if (bgpupdate_acl_debug(network, mask)) {
	    if (pathinfo.flags & BFLAG_VALID)
		buginf("\nBGP: %i rcv UPDATE about %i, next hop %i, path %s",
		       bgp->address, network, pathinfo.gateway,
		       bgp_path2string(attr->as_path, attr->aslength,
				       bgp_router_path_buffer,
				       ASCII_PATHBUFSIZE));
	    else
		buginf("\nBGP: %i rcv UPDATE about %i -- unreachable",
		       bgp->address, network);
	}

	/*
	 * Create new BGP entry if one doesn't already exist
	 * Don't create it if the path is unreachable.
	 */
	net = bgp_netlookup(network, mask);
	if (!net) {
	    if ((pathinfo.flags & BFLAG_VALID) == 0)
		continue;
	    net = bgp_create_net(network, mask);
	    if (!net) {
		errmsg(&msgsym(NOMEMORY, BGP), "path");
		bgp_reset(NULL, NULL);
		free(attr);
		free(rcv_attr);
		return;
	    }
	}

	/*
	 * Now see if we have a path from this peer
	 * If we don't have a path, and the new path is reachable, create one.
	 */
	i = bgp_find_path(net, bgp->address);
	if (i == BGP_NULL_INDEX) {	/* no path from this neighbor */
	    if ((pathinfo.flags & BFLAG_VALID) == 0)
		continue;
	    net = bgp_add_path(net, &pathinfo, attr, &i, BGP_NULL_INDEX,
			       BGP_NULL_INDEX);
	    if (!net) {
		errmsg(&msgsym(NOMEMORY, BGP), "path");
		bgp_reset(NULL, NULL);
		free(attr);
		goto free_and_return;
	    }
	    do_bestpath = TRUE;
	} else {
	    path = &net->paths[i];
	    if (pathinfo.flags & BFLAG_VALID) {
		if ((pathinfo.gateway == path->gateway) &&
		    (pathinfo.weight == path->weight) &&
		    (attr == path->attr) &&
		    ((pathinfo.flags & ~BFLAG_SYNCED) ==
		     (path->flags & ~BFLAG_SYNCED))) {
		    if (bgpupdate_acl_debug(network, mask))
			buginf("...duplicate ignored");
		    /*
		     * If multiple parents and duplicate, the rcv_attr from
		     * this neighbor could have changed but resulting in 
		     * same set_attr. Example : rcv MED changed but have
		     * inbound set-MED. Blow the parent list.
		     */
		    if (attr->alist && attr->alist->next)
			bgp_free_parent_alist(attr);
		    continue;
		}
		
		/*
		 * Non-identical valid path is received. Replace the
		 * existing path with the new one.
		 */
		bgp_replace_path(net, path, &pathinfo, attr, i);
		do_bestpath = TRUE;
	    } else {
		/* If we are here, then the received path is not
		 * valid. Delete the existing path. 
		 */
		if ((net->best == i) ||
		    (net->paths[i].flags & BFLAG_MULTIPATH)) {
		    do_bestpath = TRUE;
		    deleted_best = TRUE;
		}
		bgp_delete_path(net, i);
	    }

	}
	/*
	 * Call bgp_bestpath to make appropriate change to the best
	 * path.
	 */
	if (do_bestpath)
	    bgp_bestpath(net, i, deleted_best);

	process_may_suspend();
    }
    free(attr);
    
    /*
     * Release any temporary storage for transit attributes 
     */
 free_and_return:
    free(rcv_attr);
    free(attrinfo.transptr);
}

/*
 * bgp_dinfo_malloc
 *
 * bdampinfotype structure is allocated from chunk manager. So we need
 * special malloc/free/loc functions.
 *
 * cloned from rdb_malloc/rdb_lock/rdb_free
 */
BGP_PRIVATE inline bdampinfotype *
bgp_dinfo_malloc (void)
{
    bdampinfotype *dinfo;
    
    dinfo = chunk_malloc(damp_chunks);

    if (dinfo)
	dinfo->refcount = 1;

    return dinfo;
}

/*
 * bgp_decay_penalty
 *
 */
BGP_PRIVATE void
bgp_decay_penalty (bdampinfotype *dinfo)
{
    ulong decay_duration, decay_ticks;
    dampctrlblk *dcb;

    if (!dinfo)
	return;

    dcb = dinfo->dcb;
    decay_ticks = 0;
    decay_duration = ELAPSED_TIME(dinfo->decay_time);

    /*
     * When the duration is more than the halflife-time, shift right to
     * account for decay per halflife-time.
     * For the remaining duration, find the number of decay-ticks and 
     * use the decay_array to find the actual decay.
     */
    while (decay_duration >= dcb->halflife_time) {
	decay_duration -= dcb->halflife_time;
	dinfo->penalty = dinfo->penalty >> 1;
    }

    if (decay_duration >= BD_DECAY_INTERVAL) {
	decay_ticks = decay_duration/BD_DECAY_INTERVAL;
	dinfo->penalty = ((dinfo->penalty * dcb->decay_array[decay_ticks]) /
			  BD_PRECISION);
	GET_TIMESTAMP(dinfo->decay_time);
    }
}

/*
 * bgp_get_reuseindex
 *
 * This function converts the ticks into appropriate index with respect
 * to the current reuse index.
 */

BGP_PRIVATE inline ushort
bgp_get_reuseindex (ushort ticks)
{
    ushort index;

    index = bgp_reuse_index + ticks;

    if (index <= bgp_reuse_array_len)
	return(index);
    else
	return(index - bgp_reuse_array_len);
}


/*
 * bgp_get_reuseticks_inline
 */

BGP_PRIVATE inline ushort
bgp_get_reuseticks_inline (bdampinfotype *dampinfo)
{
    ushort ticks, offset_index, n;
    int double_reuse;
    ulong offset_decay_time, total_decay_time, penalty;
    dampctrlblk *dcb;

    dcb = dampinfo->dcb;
    penalty = dampinfo->penalty;

    /*
     * As unsuppression of routes are done at fixed time intervals, it
     * is possible to call this function for suppressed routes with
     * penalty just less than reuse penalty. If that is the case
     * instead of returning 0 ticks, return 1 to avoid printing the
     * reuse time to be 'never'. 
     */
    if (penalty <= dcb->reuse_penalty)
	return(1);

    n = 0;
    double_reuse = 2 * dcb->reuse_penalty;
    while (penalty >= double_reuse) {
	penalty = penalty >> 1;
	n++;
    }
    
    offset_index = (penalty - dcb->reuse_penalty) / 5;
    if (offset_index > dcb->reuse_helper_len)		/* should not happen */
	offset_decay_time = dcb->halflife_time;
    else if (offset_index == 0)
	offset_decay_time = 0;
    else
	offset_decay_time = dcb->reuse_helper[offset_index];
 

    /*
     * Calculate the reuse ticks. If tick is >= array_len, we make
     * it to array_len - 1 so that the entry is suppressed the
     * maximum possible duration. (array_len would have prevented
     * the entry from being suppressed.)
     */

    total_decay_time = (n * dcb->halflife_time) + offset_decay_time;
    ticks = total_decay_time/BD_REUSE_INTERVAL;
    if (ticks >= bgp_reuse_array_len)
	ticks = (bgp_reuse_array_len - 1);

    if (ticks)
	return(ticks);
    else
	return(1);				/* same reason as above */
}

/*
 * bgp_get_reuseticks
 */
BGP_PRIVATE ushort
bgp_get_reuseticks (bdampinfotype *dampinfo)
{
    return(bgp_get_reuseticks_inline(dampinfo));
}

/*
 * bgp_suppress
 *
 * Note : As the withdrawn/down route is always removed from the reuse
 * queue, the path received by this function cannot be present in
 * the reuse queue.
 */

BGP_PRIVATE inline void
bgp_suppress (bnettype *net, bpathtype *path)
{
    bdampinfotype *dampinfo, *next_entry;
    ushort index;
    ulong reuse_ticks;
    dampctrlblk *dcb;

    dampinfo = path->dampinfo;
    if (!dampinfo)
	return;

    next_entry = NULL;
    dcb = dampinfo->dcb;

    if (dampinfo->penalty >= dcb->suppress_penalty) {
	reuse_ticks = bgp_get_reuseticks_inline(dampinfo);
	if (reuse_ticks) {
	    index = bgp_get_reuseindex(reuse_ticks);
	    path->flags |= BFLAG_SUPPRESSED;
	    next_entry = bgp_reuse_array[index];
	    bgp_reuse_array[index] = dampinfo;
	    dampinfo->prev = (bdampinfotype *) &(bgp_reuse_array[index]);
	    dampinfo->next = next_entry;
	    if (next_entry)
		next_entry->prev = dampinfo;

	    if (bgpdamp_acl_debug(net->network, net->mask)) {
		char time[10];
		sprint_ticks_in_dhms(time, reuse_ticks*BD_REUSE_INTERVAL);
		buginf("\nBGP: suppress %i%m path %s for %s (penalty %d)", 
		       net->network, net->mask,
		       bgp_path2string(path->attr->as_path, 
				       path->attr->aslength,
				       bgp_router_path_buffer, 
				       ASCII_PATHBUFSIZE), 
		       time, dampinfo->penalty);
		buginf("\nhalflife-time %d, reuse/suppress %d/%d",
		       dcb->halflife_time/ONEMIN, dcb->reuse_penalty, 
		       dcb->suppress_penalty);
	    }
	}
    }
}

/* bgp_check_routemap_dampening
 *
 * Check if the path is eligible for dampening.
 * Return the appropriate dampening control block.
 */
BGP_PRIVATE inline dampctrlblk *
bgp_check_routemap_dampening (bnettype *net, int i)
{
    route_maptype *map;
    battrtype *attr;
    static battrtype *last_attr = NULL;
    static char	buffer[ASCII_PATHBUFSIZE] = "";

    if (!bgp_dampening_map)
	return(NULL);
    
    attr = net->paths[i].attr;
    if (attr != last_attr)
	if (!bgp_path2string(attr->as_path, attr->aslength, 
			     buffer, sizeof(buffer)))
	    return (NULL);
    last_attr = attr;
    
    map = bgp_route_map_check(bgp_dampening_map, net->paths[i].attr,
			      net, net->network, net->mask, buffer, TRUE);
    if (map)
	return(bgp_dcb_array[map->index]);
    else
	return(NULL);
}

/*
 * bgp_charge_penalty
 *
 * Calculate the penalty for the route for a down event. 
 * If it a change in attribute, penalty is BD_PENALTY/2.
 */

BGP_PRIVATE boolean
bgp_charge_penalty (bnettype *net, int i, boolean attr_change)
{
    bpathtype *path;
    bdampinfotype *dampinfo;
    ushort penalty;
    dampctrlblk *dcb;

    dcb = NULL;
    path = &net->paths[i];
    dampinfo = path->dampinfo;

    /*
     * If dampinfo is already present, decay the penalty before
     * charging more.
     *
     * If dampinfo is not present and route-map dampening is used,
     * make sure that this path is eligible for dampening.
     */
    if (dampinfo) {
	bgp_decay_penalty(dampinfo);
	dcb = dampinfo->dcb;
    } else {
	if (bgp_normal_dcb)
	    dcb = bgp_normal_dcb;
	else
	    dcb = bgp_check_routemap_dampening(net, i);

	if (!dcb)
	    return(TRUE);

	dampinfo = bgp_dinfo_malloc();
	if (!dampinfo)
	    return(FALSE);
	dampinfo->net = net;			/* ptr to net fo  mem_lock */
	dampinfo->pindex = i;
	dampinfo->dcb = dcb;
	GET_TIMESTAMP(dampinfo->start_time);	/* start flap counter timer */
	path->dampinfo = dampinfo;
    }
    
    /*
     * For change in attribute, charge half the penalty compared with
     * down event.
     */
    if (attr_change)
	penalty = BD_PENALTY >> 1;
    else
	penalty = BD_PENALTY;

    if ((dampinfo->penalty+penalty) > dcb->maximum_penalty)
	dampinfo->penalty = dcb->maximum_penalty;
    else
	dampinfo->penalty += penalty;

    GET_TIMESTAMP(dampinfo->decay_time);
    dampinfo->flap_count++;

    /*
     * If the down path is suppressed, reset the flag and remove it
     * from the reuse queue.
     */
    if (path->flags & BFLAG_SUPPRESSED) {
	path->flags &= ~BFLAG_SUPPRESSED;
	dampinfo->prev->next = dampinfo->next;
	if (dampinfo->next)
	    dampinfo->next->prev = dampinfo->prev;
	dampinfo->next = NULL;
	dampinfo->prev = NULL;
    }

    if (bgpdamp_acl_debug(net->network, net->mask)) {
	char time[40];
	sprint_ticks_in_dhms(time, ELAPSED_TIME(dampinfo->start_time));
 	buginf("\nBGP: charge penalty for %i%m path %s with halflife-time %d "
 	       "reuse/suppress %d/%d", net->network, net->mask,
 	       bgp_path2string(path->attr->as_path, path->attr->aslength,
 			       bgp_router_path_buffer, ASCII_PATHBUFSIZE),
 	       dcb->halflife_time/ONEMIN, dcb->reuse_penalty, 
 	       dcb->suppress_penalty);
 	buginf("\nBGP: flapped %d times since %s. New penalty is %d",
  	       dampinfo->flap_count, time, dampinfo->penalty);
    }

    return(TRUE);
}

/*
 * bgp_damp_update
 *
 * When there exists an unsuppressed path from a peer and if a new 
 * path with changed attribute is received, it must be treated
 * like a flap.
 * Charge penalty for the path from the neighbor and check if the
 * path needs to be suppressed. Also update the path to reflect 
 * the new update.
 */

BGP_PRIVATE inline boolean
bgp_damp_update (bnettype *net, bpathtype *pathinfo, battrtype *attr, int i)
{
    bpathtype *path;

    path = &net->paths[i];
    
    /*
     * Replace the new path before checking penalty, as the changed 
     * path might result in use of different set of dampening
     * parameters. (Handle suppressed flag with care!)
     *
     * If this is change in attribute (pre-existing path is valid), 
     * charge penalty.
     * If the pre-existing path is not valid (ie a path went down 
     * and came up), decay the penalty. 
     */
    bgp_replace_path(net, path, pathinfo, attr, i);
    if (path->flags & BFLAG_VALID)
	if (!bgp_charge_penalty(net, i, TRUE))
	    return(FALSE);
    else
	bgp_decay_penalty(path->dampinfo);

    bgp_suppress(net, path);
    return(TRUE);
}

/*
 * bgp_route_map_net_accesslist
 *
 * For each access-list reference in a route-map, do a BGP net table lookup
 * to find corresponding net. If the access-list says deny, the route lookup
 * continues.
 */
bnettype *bgp_route_map_net_accesslist (route_map_match_acl *match_list)
{
    route_map_match_acl   *list;
    fasttype              *grp;
    fastitem              *acc;
    bnettype              *net;
    fasthash              *fast_entry;
    int                   i;

    net = NULL;

    /*
     * For each "match ip address" access-list number, do a route lookup
     * for each wildcard entry for a given access-list.
     */
    for (list = match_list; list; list = list->next) {

	if (!list->acl || !list->acl->list)
	    continue;
	/*
	 * Validate that the user didn't enter an extended access list
	 */
	if ((list->acl->type != ACL_IP_SIMPLE) &&
	    (list->acl->type != ACL_IP_NAMED_SIMPLE))
	    continue; 

	grp = (fasttype *) list->acl->list;

	/*
	 * Scan all hash bucket of non-wildcarded entries.
	 */
	for (i=0, fast_entry = grp->hashlist[i]; i < NETHASHLEN;
	     fast_entry = grp->hashlist[++i]) {

	    for (; fast_entry; fast_entry = fast_entry->next) {
		net = bgp_netlookup(fast_entry->address, 0);
		if (net)
		    if (fast_entry->grant)
			return(net);
		net = NULL;
	    }
	}

	/*
	 * Access list found. Return, if route lookup successful and permit
	 * was configured. Otherwise, continue looking for access-list
	 * references in route-map match list.
	 */
	for (acc = grp->items; acc; acc = acc->next) {
	    net = bgp_netlookup(acc->permitmask, ~acc->wildmask);
	    if (net)
		if (acc->grant)
		    return(net);
	    net = NULL;
	}
    }
    return(net);
}
/*
 * bgp_route_map_find_net
 *
 * Given a configured route-map, find net that corresponds to an access-list
 * specificed destination.
 */
bnettype *bgp_route_map_find_net (route_map_nametype *rmap)
{

    route_maptype      *map;
    route_map_headtype *head;
    bnettype           *net;
    int                i;
    uchar              *pathstr;
    bpathtype          *path;
    battrtype          *attr;

    net = NULL;
    head = rmap->head;
    if (!head)
	return(NULL);

    /*
     * Go through all route-maps with same name.	
     */
    for (map = (route_maptype *) head->map_queue.qhead; map; map = map->next) {
	/*
	 * Get net based on IP access-list reference in route-map.  Then
	 * check other route-map match.
	 */
	net = bgp_route_map_net_accesslist(map->match_list.ip_address);
	if (net) {
	    for (i = net->firstpath; i != BGP_NULL_INDEX; i = path->next) {
		path = &net->paths[i];
		attr = path->attr;
		pathstr = bgp_path2string(attr->as_path, attr->aslength,
					  bgp_router_path_buffer,
					  ASCII_PATHBUFSIZE);
		if (bgp_route_map_check(rmap, attr, net, net->network,
					net->mask, pathstr, TRUE))
		    return(net);
	    }
	}
    }
    return(NULL);
}

/*
 * bgp4_rcv_update
 * Handle a received update message from a BGP4 peer
 */

BGP_PRIVATE void
bgp4_rcv_update (neighbortype *bgp, uchar *data, unsigned bytes)
{
    ipaddrtype network, mask, rcvd_nexthop;
    bnettype *net;
    bpathtype pathinfo, *path;
    battrtype attrinfo, *attr, *rcv_attr;
    uchar *cpi;
    ushort pathlen, netlen, withdrawnsize, len, errlen, aspathlen;
    ushort asystem, normal_weight, rcvd_aslength;
    ulong cluster_id, originator, changed;
    rrinfotype *rrinfo;
    uchar flags, saveflag, code;
    uchar error, *errptr, *aspath, *pathptr, *pathstr;
    boolean truncated, common_admin, same_cluster, do_bestpath;
    boolean routemap_netmatch, firsttime, deleted_best;
    int ix, j, entry_size, count, seglen, used_idx, net_processed;
    uchar *savedata;
    unsigned savebytes;
    route_maptype *map;

    savedata = data;
    savebytes = bytes;
    map = NULL;
    routemap_netmatch = FALSE;
    normal_weight = 0;
    pathstr = NULL;
    rcvd_aslength = 0;
	
    /*
     * Compute attribute length and the number of networks
     */
    path = NULL;

    /*
     * BGP peer under common administration.
     */
    common_admin = (bgp->bgp_internal || (bgp->flags & BN_COMMONADMIN));

    /*
     * Scan the path attributes and copy into pathinfo data structure.
     * Use the bgp_router_com_buffer to store the commuity information.
     * (Helpful to add new commuity via route-map).
     */
    error = 0;
    truncated = FALSE;
    same_cluster = FALSE;
    aspath = NULL;
    aspathlen = 0;
    rcv_attr = NULL;
    memset(bgp_router_rrinfo_buffer, 0, sizeof(rrinfotype)+BGP_RR_BUFLEN);
    rrinfo = bgp_router_rrinfo_buffer;
    memset(&pathinfo, 0, sizeof(bpathtype));
    memset(&attrinfo, 0, sizeof(battrtype));
    attrinfo.community = bgp_router_com_buffer;	/* placeholder for community */

    /*
     * Prepare the pathinfo stucture to parse the update.
     * Set the pathinfo flag to indicate if the path is from RR client
     * or from confederation peer. 
     */
    pathinfo.neighbor = bgp->address;
    pathinfo.router_id = bgp->bgp_router_id;
    pathinfo.flags |= BFLAG_VALID;
    if (bgp->bgp_internal) {
	pathinfo.flags |= BFLAG_INTERNAL;
	if (bgp_cluster_id && 
	    (bgp->flags & BN_RR_CLIENT))
	    pathinfo.flags |= BFLAG_CLIENT;
    }
    if (bgp_confed_id &&
	(bgp->flags & BN_COMMONADMIN))
	pathinfo.flags |= BFLAG_CONFED;

    withdrawnsize = GETSHORT(data);
    data += sizeof(ushort);
    bytes -= sizeof(ushort);

    while (withdrawnsize > 0) {
	entry_size = bgp_readprefix(data, &network, &mask);
	if (!entry_size) {
	    bgp_send_notification(bgp, BNOT_UPDATE, BNOT_UPDATE_NETWORK,
				  data, 1);
	    if (bgp_debug)
		buginf("\nBGP: %i bogus withdrawal route prefix received",
		       bgp->address);
	    return;
	}

	data += entry_size;
	bytes -= entry_size;
	withdrawnsize -= entry_size;

	/*
	 * If this is a natrual net, do full martian checking for validity.
	 * Otherwise, we're in the CIDR zone and we should just check to
	 * make sure that it's not a network entry for non-unicast addresses.
	 */
	if (network && (mask == get_majormask(network)) ?
	    martian(network, FALSE) : CLASSD_AND_GREATER(network)) {
	    if (bgp_debug)
		buginf("\nBGP: %i martian network %i withdrawn",
		       bgp->address, network);
	    continue;
	}

	if (bgpupdate_acl_debug(network, mask))
	    buginf("\nBGP: %i rcv UPDATE about %i%m -- withdrawn",
		   bgp->address, network, mask);
	    
	/* see if network is in BGP database */
	net = bgp_netlookup(network, mask);
	if (!net)			/* didn't have an entry */
	    continue;


 	ix = bgp_find_path(net, bgp->address);
 	if ((ix == BGP_NULL_INDEX) ||		/* didn't have a path */
 	    !(net->paths[ix].flags & BFLAG_VALID))
	    continue;

	path = &net->paths[ix];
	attr = path->attr;	
	path->flags &= ~BFLAG_VALID;

	if (bgp->flags & BN_SOFT_RECONFIG_INOK) {
	    /*
	     * If received entry alone, blow it away and continue.
	     * If the is both received and used, clear the flag and
	     * fall thru.
	     * Or else check if there is the next path is from this 
	     * neighbor. If so, it is rcvd path, blow it away and 
	     * fall thru.
	     */
	    if (path->flags & BFLAG_RCVD_ONLY) {
		bgp_delete_path(net, ix);
		continue;
	    } else if (path->flags & BFLAG_RCVD_USED)
		path->flags &= ~BFLAG_CLEAR_RCVD;
	    else if ((path->next != BGP_NULL_INDEX) &&
		     (net->paths[path->next].neighbor == bgp->address))
		bgp_delete_path(net, path->next);
	}


	if (bgp_dampening_enabled && !bgp->bgp_internal) {
	    if (!bgp_charge_penalty(net, ix, FALSE))
		return;
	    path->flags |= BFLAG_HISTORY;
	} else {
	    /* 
	     *  If attr has multiple parent entries, remove the parent 
	     * list.
	     */
	    if (attr->alist && attr->alist->next)
		bgp_free_parent_alist(attr);
	    bgp_delete_path(net, ix);
	}	    

	if ((net->best == ix) ||
	    (net->paths[ix].flags & BFLAG_MULTIPATH))
	    bgp_bestpath(net, ix, TRUE);

	process_may_suspend();
    }

    /* must have had an error somewhere */
    if (withdrawnsize != 0) {
	if (bgp_debug)
	   buginf("\nBGP: synchronization error processing withdrawn NRLI");
	bgp_send_notification(bgp, BNOT_UPDATE, BNOT_UPDATE_MALFORMED,
			      NULL, 0);
    }

    /*
     * Processed all withdrawn routes, now check for attributes
     * (if no attributes, then it was a poor-man's bgp3 unreachable).
     */
    pathlen = GETSHORT(data);	/* size of path attribute area */
    data += sizeof(ushort);
    bytes -= sizeof(ushort);
    netlen = bytes - pathlen;	/* size of network entry area */

    if (pathlen == 0 && netlen == 0)	/* just withdrawn routes? */
	return;

    bytes = pathlen;
    while ((bytes > 0) && !error) {
	errptr = data;
	saveflag = *data++;
	flags = (saveflag & UPF_USED);
	code = *data++;
	if (flags & UPF_EXTENDED) {
	    flags &= ~UPF_EXTENDED;
	    len = GETSHORT(data);
	    data += sizeof(ushort);
	    if (len > bytes) {
		if (bgp_debug)
		    buginf("\nBGP: %i bad extended length, told %d got %d",
			   bgp->address, len, bytes);
		error = BNOT_UPDATE_MALFORMED;
		len = bytes;
	    }
	    bytes -= 4;
	    errlen = len + 4;
	} else {
	    len = *data++;
	    if (len > bytes) {
		if (bgp_debug)
		    buginf("\nBGP: %i bad attribute length, told %d got %d",
			   bgp->address, len, bytes);
		error = BNOT_UPDATE_MALFORMED;
		len = bytes;
	    }

 	    bytes -= 3;
	    errlen = len + 3;
	}
	if (error) {
	    bgp_send_notification(bgp, BNOT_UPDATE, error, errptr, errlen);
	    goto free_and_return;
	}
	switch (code) {
	case ATT4_ORIGIN:
	    if (flags != UPF_TRANSIT) {
		error = BNOT_UPDATE_FLAGS;
		break;
	    }
	    if (pathinfo.flags & BFLAG_ORIGIN) {
		if (bgp_debug)
		    buginf("\nBGP: %i duplicate origin attribute",
			   bgp->address); 
		error = BNOT_UPDATE_MALFORMED;
		break;
	    }
	    if (len != 1) {
		error = BNOT_UPDATE_LENGTH;
		break;
	    }
	    attrinfo.origin = *data;
	    if (attrinfo.origin > ORIGIN_INCOMPLETE)
		error = BNOT_UPDATE_ORIGIN;
	    else
		pathinfo.flags |= BFLAG_ORIGIN;
	    break;

	case ATT4_PATH:
	    if (flags != UPF_TRANSIT) {
		error = BNOT_UPDATE_FLAGS;
		break;
	    }

	    if (pathinfo.flags & BFLAG_ASPATH) {
		if (bgp_debug)
		    buginf("\nBGP: %i duplicate AS path attribute",
			   bgp->address);
		error = BNOT_UPDATE_MALFORMED;
		break;
	    }

	    if (len & 1) {
		error = BNOT_UPDATE_LENGTH;
		break;
	    }

	    aspath = bgp_rcvd_aspath_buffer;

	    /*
	     * Reject the update if the AS-path has more than BGP_PATHBUFSIZE
	     * AS numbers. Setting BFLAG_ILLEGAL bit will not permit this
	     * into the received-only group also.
	     */
	    if (len > BGP_PATHBUFLEN) {
		errmsg(&msgsym(ASPATH, BGP), "Long",
		       bgp_path2string(data, len,
				       bgp_router_path_buffer,
				       ASCII_PATHBUFSIZE),
		       bgp->address, "Has more than 255 AS");
		pathinfo.flags &= ~BFLAG_VALID;
		pathinfo.flags |= BFLAG_ILLEGAL;
		goto abort_loop_check;
	    }

	    if (!bgp_pathvalid(data, len)) {
		if (bgp_debug)
		    buginf("\nBGP: Invalid path segment in %s received"
			   " from %i",
			   bgp_path2string(data, aspathlen,
					   bgp_router_path_buffer,
					   ASCII_PATHBUFSIZE),
			   bgp->address);
		error = BNOT_UPDATE_BADPATH;
		break;
	    }

	    aspathlen = len;
	    if (aspathlen) {
		/*
		 * If the path from a non common admin neighbor has
		 * confederation information, fix the AS path and
		 * generate error message.
		 */
		pathptr = aspath;
		cpi = data;
		if (!common_admin &&
		    (*cpi == ASENTRY_SEQUENCE)) {
		    errmsg(&msgsym(ASPATH, BGP), "Invalid", 
			   bgp_path2string(data, aspathlen, 
					   bgp_router_path_buffer,
					   ASCII_PATHBUFSIZE),
			   bgp->address, "Non confederation peer");
		    seglen = 2 + (*(cpi+1) * PATHSIZE);
		    cpi += seglen;
		    aspathlen -= seglen;

		    *pathptr++ = AS_SEQUENCE;
		    *pathptr++ = 1;
		    PUTSHORT(pathptr, bgp->yoursystem);
		    pathptr += PATHSIZE;
		    attrinfo.aslength = PATHSIZE + 2;
		}

		bcopy(cpi, pathptr, aspathlen);
		attrinfo.aslength += aspathlen;
  	    }

	    /*
	     * Now we check for an AS routing loop.
	     * This has changed through the BGP4 drafts.  As of IETF
	     * Houston, we now only are to check for -our- AS appearing
	     * anywhere in the path.  If so, just ignore update.
	     * Indicate that its malformed so that we invalidate the prefixes
	     * advertised earlier.
	     */
	    
	    for (cpi = aspath, ix = attrinfo.aslength; ix > 0; ix -= 2){
		cpi++;

		/* for every AS in a particular path segment */
		for (seglen = *cpi++; (seglen > 0) && (ix > 0);
		     seglen--, ix -= PATHSIZE, cpi += PATHSIZE) {

		    asystem = GETSHORT(cpi);
		    if ((asystem == pdb_BGP->asystem) ||
			(bgp_confed_id && (asystem == bgp_confed_id))) {
			pathinfo.flags &= ~BFLAG_VALID;
			pathinfo.flags |= BFLAG_ILLEGAL;
			goto abort_loop_check;
		    }
  		}
  	    }
	abort_loop_check:
	    pathinfo.flags |= BFLAG_ASPATH;
	    break;

	case ATT4_NEXTHOP:
	    if (flags != UPF_TRANSIT) {
		error = BNOT_UPDATE_FLAGS;
		break;
	    }
	    if (pathinfo.flags & BFLAG_NEXTHOP) {
		if (bgp_debug)
		    buginf("\nBGP: %i duplicate next hop attribute",
			   bgp->address);
		error = BNOT_UPDATE_MALFORMED;
		break;
	    }
	    if (len != sizeof(ipaddrtype)) {
		error = BNOT_UPDATE_LENGTH;
		break;
	    }
	    bcopy(data, &pathinfo.gateway, sizeof(ipaddrtype));
	    pathinfo.flags |= BFLAG_NEXTHOP;

	    /*
	     * If the next hop is martian, the update is illegal.
	     */
	    if (martian(pathinfo.gateway, TRUE)) {
		pathinfo.flags &= ~BFLAG_VALID;
		pathinfo.flags |= BFLAG_ILLEGAL;
		errmsg(&msgsym(NEXTHOP, BGP),
		       pathinfo.gateway, bgp->address, "martian next hop");
		break;
	    }

	    /*
	     * If this is a normal one-hop-away EBGP peering 
	     * then ignore any updates that are sent to us with a
	     * non-local next hop.
	     */
	    if (!common_admin && (bgp->ebgp_ttl == BGP_DEFAULT_EBGP_TTL)) {
	        if (!samecable(pathinfo.gateway)) {
		    if (bgp_debug)
			buginf("\nBGP: %i Non-local next hop %i",
			       bgp->address, pathinfo.gateway);
		    pathinfo.flags &= ~BFLAG_VALID;
		    pathinfo.flags |= BFLAG_ILLEGAL;
                }
	    }

	    if (ip_up_ouraddress(pathinfo.gateway)) {
		if (bgp_debug)
		    buginf("\nBGP: %i Next hop is our own address %i",
			   bgp->address, pathinfo.gateway);
		pathinfo.flags &= ~BFLAG_VALID;
		pathinfo.flags |= BFLAG_ILLEGAL;
            }
	    break;

	case ATT4_EXITDISC:
	    if (flags != UPF_OPTIONAL) {
		error = BNOT_UPDATE_FLAGS;
		break;
	    }
	    if (pathinfo.flags & BFLAG_METRIC) {
		if (bgp_debug)
		    buginf("\nBGP: %i duplicate multi-exit-discriminator",
			   bgp->address);
		error = BNOT_UPDATE_MALFORMED;
	    }
	    if (len != 4) {
		error = BNOT_UPDATE_LENGTH;
		break;
	    }
	    attrinfo.metric = GETLONG(data);
	    pathinfo.flags |= BFLAG_METRIC;
	    break;

	case ATT4_LOCALPREF:
	    if (flags != UPF_TRANSIT) {
		error = BNOT_UPDATE_FLAGS;
		break;
	    }
	    /*
	     * Allowed to receive localpref from peer under common
	     * administration.
	     */
	    if (!common_admin) {
		if (bgp_debug)
		    buginf("\nBGP: %i ignoring localpref attribute",
			   bgp->address);
		break;
	    }
	    if (pathinfo.flags & BFLAG_LOCALPREF) {
		if (bgp_debug)
		    buginf("\nBGP: %i duplicate localpref attribute",
			   bgp->address);
		error = BNOT_UPDATE_MALFORMED;
		break;
	    }
	    if (len != 4) {
		error = BNOT_UPDATE_LENGTH;
		break;
	    }
	    attrinfo.localpref = GETLONG(data);
	    pathinfo.flags |= BFLAG_LOCALPREF;
	    break;

	case ATT4_DESTPREF:
	    if (!bgp_dpa_enabled)
		goto unrecognized;

	    if ((flags != UPF_OPTIONAL+UPF_TRANSIT) &&
		(flags != UPF_OPTIONAL+UPF_TRANSIT+UPF_PARTIAL)) {
		error = BNOT_UPDATE_FLAGS;
		break;
	    }
	    if (pathinfo.flags & BFLAG_DESTPREF) {
		if (bgp_debug)
		    buginf("\nBGP: %i duplicate destination preference "
			   "attribute",	 bgp->address);
		error = BNOT_UPDATE_MALFORMED;
		break;
	    }
	    if (len != 6) {
		error = BNOT_UPDATE_LENGTH;
		break;
	    }
	    attrinfo.destpref_as = GETSHORT(data);
	    attrinfo.destpref = GETLONG(data+2);
	    pathinfo.flags |= BFLAG_DESTPREF;
	    break;

	case ATT4_ATOMICAGG:
	    if ((flags != UPF_TRANSIT) &&
		(flags != UPF_TRANSIT+UPF_OPTIONAL)) {
		error = BNOT_UPDATE_FLAGS;
		break;
	    }
	    if (pathinfo.flags & BFLAG_ATOMICAGG) {
		if (bgp_debug)
		    buginf("\nBGP: %i duplicate atomic aggregate attribute",
			   bgp->address);
		error = BNOT_UPDATE_MALFORMED;
		break;
	    }
	    if (len != 0) {
		error = BNOT_UPDATE_LENGTH;
		break;
	    }
	    pathinfo.flags |= BFLAG_ATOMICAGG;
	    break;


	case ATT4_AGGREGATOR:
	    if ((flags != UPF_OPTIONAL+UPF_TRANSIT) &&
		(flags != UPF_OPTIONAL+UPF_TRANSIT+UPF_PARTIAL)) {
		error = BNOT_UPDATE_FLAGS;
		break;
	    }
	    if (pathinfo.flags & BFLAG_AGGREGATOR) {
		if (bgp_debug)
		    buginf("\nBGP: %i duplicate aggregator attribute",
			   bgp->address);
		error = BNOT_UPDATE_MALFORMED;
		break;
	    }
	    if (len < 2) {
		error = BNOT_UPDATE_LENGTH;
		break;
	    }

	    pathinfo.flags |= BFLAG_AGGREGATOR;
	    attrinfo.aggregator.as = GETSHORT(data);

	    if (len >= 6)
                attrinfo.aggregator.ip = GETLONG(data+2);
            else
		attrinfo.aggregator.ip = 0;
	    break;

	case ATT4_COMMUNITY:
	    if ((flags != UPF_TRANSIT+UPF_OPTIONAL) &&
		(flags != UPF_TRANSIT+UPF_OPTIONAL+UPF_PARTIAL)) {
		error = BNOT_UPDATE_FLAGS;
		break;
	    }

	    if (pathinfo.flags & BFLAG_COMMUNITY) {
		if (bgp_debug)
		    buginf("\nBGP: %i duplicate COMMUNITY path attribute",
			   bgp->address);
		error = BNOT_UPDATE_MALFORMED;
		break;
	    }

	    if ((len & 1) || (len == 0)) {
		error = BNOT_UPDATE_MALFORMED;
		break;
	    }
	    
	    count = COMMUNITY_COUNT(len);
	    if (count > BGP_MAXCOMMUNITY) {
		if (bgp_debug)
		    buginf("\nBGP: %i COMMUNITY attribute with more than %d"
			   " communities", bgp->address, BGP_MAXCOMMUNITY);
		error = BNOT_UPDATE_LENGTH;
		break;
	    }

	    /*
	     * Extract the community attributes and place it in
	     * attrinfo structure. (ie bgp_router_com_buffer).
	     */
	    if (!bgp_extract_community(data, count, attrinfo.community,&error))
		break;
	    
	    attrinfo.comcount = count;
	    pathinfo.flags |= BFLAG_COMMUNITY;
	    break;

	case ATT4_CLUSTLIST:
	    if (flags != UPF_OPTIONAL) {
		error = BNOT_UPDATE_FLAGS;
		break;
	    }

	    if (!bgp->bgp_internal) {
		if (bgp_debug)
		    buginf("\nBGP: %i cluster list attribute from external "
			   "neighbor", bgp->address);
		error = BNOT_UPDATE_MALFORMED;
		break;
	    }
	    
	    if (pathinfo.flags & BFLAG_CLUSTLIST) {
		if (bgp_debug)
		    buginf("\nBGP: %i duplicate cluster list attribute",
			   bgp->address);
		error = BNOT_UPDATE_MALFORMED;
		break;
	    }

	    if (len & 1) {
		error = BNOT_UPDATE_LENGTH;
		break;
	    }

	    if (len > BGP_RR_BUFLEN) {
		if (bgp_debug)
		    buginf("\nBGP: %i Clusterlist attribute length(%d) more"
			   " than %d", bgp->address, len, BGP_RR_BUFLEN);
		error = BNOT_UPDATE_LENGTH;
		break;
	    }

	    attrinfo.rrinfo = rrinfo;
	    rrinfo->clusterlength = len;

	    /*
	     * Check for cluster loop. Look for our cluster-id
	     * in the clusterlist. Indicate that the path is malformed
	     * if there is a loop.
	     * 
	     * If the first entry in the cluster-list is same as the
	     * local cluster-id, then the neighbor is configured to be
	     * in the same cluster.
	     */
	    for (cpi = data, ix = len, j = 0; ix > 0; ix -= 4, cpi += 4) {
		cluster_id = GETLONG(cpi);
		if (cluster_id == bgp_cluster_id) {
		    if (j == 0)
			same_cluster = TRUE;
		    else {
			if (bgp_debug)
			    buginf("\nBGP: %i Route Reflector cluster loop "
				   "received cluster-id %i",
				   bgp->address, cluster_id);
			pathinfo.flags &= ~BFLAG_VALID;
			pathinfo.flags |= BFLAG_ILLEGAL;
			goto abort_cluster_loop_check;
		    } 
		}
		rrinfo->clusterlist[j++] = cluster_id;
	    }
	abort_cluster_loop_check:
	    pathinfo.flags |= BFLAG_CLUSTLIST;
	    break;

	case ATT4_ORIGINATOR:
	    if (flags != UPF_OPTIONAL) {
		error = BNOT_UPDATE_FLAGS;
		break;
	    }

	    if (!bgp->bgp_internal) {
		if (bgp_debug)
		    buginf("\nBGP: %i originator attribute from external "
			   "neighbor", bgp->address);
		error = BNOT_UPDATE_MALFORMED;
		break;
	    }
	    
	    if (len != sizeof(ipaddrtype)) {
		error = BNOT_UPDATE_LENGTH;
		break;
	    }
	    
	    originator = GETLONG(data);

	    /*
	     * If the originator is same as the local then it
	     * indicates that a local client is connected to a RR in
	     * some other cluster. Discard the update, its malformed.
	     */
	    if (originator == pdb_BGP->bgp_rtr_id) {
		if (bgp_debug)
		    buginf("\nBGP: %i local router is the originator, "
			   "discard update", bgp->address);
		pathinfo.flags &= ~BFLAG_VALID;
		pathinfo.flags |= BFLAG_ILLEGAL;
		break;
	    }
	    
	    /*
	     * Use the ORIGINATOR attribute value as the router-id
	     * for the received updates. 
	     */
	    attrinfo.rrinfo = rrinfo;
	    rrinfo->originator = originator;
	    pathinfo.router_id = originator;
	    pathinfo.flags |= BFLAG_ORIGINATOR;
	    break;

	default:
	    /*
	     * Error if unrecognized attribute is mandatory
	     */
	unrecognized:
	    if ((flags & UPF_OPTIONAL) == 0) {
		if (bgp_debug)
		    buginf("\nBGP: %i unrecognized mandatory option %d",
			   bgp->address, code);
		error = BNOT_UPDATE_FLAGS;
		break;
	    }

	    /*
	     * Quietly ignore unrecognized, non-transit attributes
	     */
	    if ((flags & UPF_TRANSIT) == 0)
		break;
	    
	    /*
	     * Record and pass along unrecognized transit attributes.
	     * Set partial bit to indicate we didn't update attribute.
	     * 
	     * Use errlen as we need to include the length of the Tag and 
	     * the Length field while storing the TLV.
	     */
	    *errptr |= UPF_PARTIAL;
	    if (attrinfo.translen == 0) {
		attrinfo.transptr = malloc(errlen);
		if (!attrinfo.transptr) 
		    break;
		attrinfo.translen = errlen;
	        bcopy(errptr, attrinfo.transptr, errlen);
	    } else {
		cpi = attrinfo.transptr;
		attrinfo.transptr = malloc(errlen + attrinfo.translen);
		if (!attrinfo.transptr) {
		    attrinfo.transptr = cpi;
		    break;
		}
		bcopy(cpi, attrinfo.transptr, attrinfo.translen);
		bcopy(errptr, attrinfo.transptr+attrinfo.translen, errlen);
		attrinfo.translen += errlen;
		free(cpi);
	    }
	    break;
	}

	/*
	 * Check for errors, else advance to next path attribute.
	 */
	if (error) {
	    bgp_send_notification(bgp, BNOT_UPDATE, error, errptr, errlen);
	    goto free_and_return;
	}
	data += len;
	bytes -= len;
    }

    /*
     * All done parsing attributes.  Check for truncation and for all
     * mandatory items.
     */
    if (((pathinfo.flags & BFLAG_REQUIRED) != BFLAG_REQUIRED) ||
	 (bgp->bgp_internal & !(pathinfo.flags & BFLAG_LOCALPREF))) {
	if (bgp_debug)
	    buginf("\nBGP: %i update missing attributes, flags = %#x",
		   bgp->address, pathinfo.flags);
	bgp_send_notification(bgp, BNOT_UPDATE, BNOT_UPDATE_MISSING, NULL, 0);
	goto free_and_return;
    }

    /*
     * If it is a reflected update, discard the prefix.
     */
    if (same_cluster) {
	if (rrinfo->originator != bgp->bgp_router_id) {
	    if (bgp_debug)
		buginf("\nBGP: %i RR in same cluster. Reflected update "
		       "dropped", bgp->address);
	    pathinfo.flags &= ~BFLAG_VALID;
	    pathinfo.flags |= BFLAG_ILLEGAL;
	}
    }
	    

    /*
     * Add the received attribute entry to the attribute_table. This
     * will help in caching the route-map/filter-list results.
     */
    rcv_attr = bgp_find_attr(&attrinfo, aspath, attrinfo.community);
    if (!rcv_attr)
	return;
    mem_lock(rcv_attr);				/* suspend-lock */

    pathinfo.weight = bgp->bgp_weight;
    rcvd_nexthop = pathinfo.gateway;
    rcvd_aslength = attrinfo.aslength;

    /*
     * Perform non route-map path filtering. At this stage, BFLAG_ILLEGAL
     * means that the received BGP update is incorrect.
     */
    if ((bgp->flags & BN_INBOUND_PATHPOLICY) &&
        ((pathinfo.flags & BFLAG_ILLEGAL) == 0)) {
	pathstr = bgp_path2string(rcv_attr->as_path, rcv_attr->aslength,
				  bgp_router_path_buffer, ASCII_PATHBUFSIZE);
	if (!pathstr)
	    return;

	if (!bgp_nonmap_path_policy(bgp, pathstr, rcv_attr, &pathinfo.weight))
	    pathinfo.flags &= ~BFLAG_VALID;

	normal_weight = pathinfo.weight;
    }

    /*
     * Handle the received special communities
     */
    bgp_special_community(attrinfo.community, attrinfo.comcount, &pathinfo);

    /*
     * Scan the network list
     */
    attr = NULL;
    changed = 0;
    firsttime = TRUE;
    net_processed = 0;
    while (netlen > 0) {
	do_bestpath = FALSE;
	deleted_best = FALSE;
	net = NULL;
	ix = BGP_NULL_INDEX;
	entry_size = bgp_readprefix(data, &network, &mask);
	if (!entry_size) {
	    bgp_send_notification(bgp, BNOT_UPDATE, BNOT_UPDATE_NETWORK,
				  data, 1);
	    if (bgp_debug)
		buginf("\nBGP: %i bogus update route prefix received",
		       bgp->address);
	    free(rcv_attr);			/* unlock suspend-lock */
	    return;
	}
	
	data += entry_size;
	netlen -= entry_size;
	net_processed++;

	/*
	 * If this is a natural net, do full martian checking for validity.
	 * Otherwise, we're in the CIDR zone and we should just check to
	 * make sure that it's not a network entry for non-unicast addresses.
	 */
	if (network && (mask == get_majormask(network)) ?
	    martian(network, FALSE) : CLASSD_AND_GREATER(network)) {
	    if (bgp_debug)
		buginf("\nBGP: %i martian network %i received",
		       bgp->address, network);
	    continue;
	}

	/*
	 * GateD has the stupid habit of sending out a host route for us.
	 * Do not take it or redistribute it.
	 */
	if ((mask == MASK_HOST) && ip_ouraddress(network)) {
	    if (bgp_debug)
		buginf("\nBGP: %i announced my interface %i back to me",
		       bgp->address, network);
	    continue;
	}


	/*
	 * Inbound route-map processing.
	 *
	 * First time, if it is a valid path do the match. If permit,
	 * proceed to do route-map set.
	 *
	 * If it not the first time and if the inbound routemap has
	 * prefix match, then we need do match/set again for the new
	 * NLRI. As the previous set might have modified the received
	 * attribute, we need to recreate the received attribute. For
	 * speed, use the info from previous set to do selective copy 
	 */
	if (bgp->bgp_routemapin) {
	    if ((firsttime && 
		 (pathinfo.flags & BFLAG_VALID)) ||
		routemap_netmatch) {

		firsttime = FALSE;
		routemap_netmatch = (bgp->bgp_routemapin->head->match_flags &
				     ROUTE_MAP_BGP_NETWORK_MATCHES);

		map = bgp_route_map_check(bgp->bgp_routemapin, rcv_attr, 
					  NULL, network, mask, pathstr, TRUE);
		if (!map)
		    pathinfo.flags &= ~BFLAG_VALID;
		else {
		    /*
		     * If there is any change, reset the attribute 
		     * and pathinfo back to the received value before
		     * calling rcv_set function.
		     * Note : attribute should point to the global
		     *        community buffer to support community
		     *        addition!
		     * Fix the aspath and its length to point to the
		     * received values before applying map_set again.
		     */
		    if (changed) {
			pathinfo.gateway = rcvd_nexthop;
			pathinfo.weight = normal_weight;
			aspath = bgp_rcvd_aspath_buffer;
			attrinfo.aslength = rcvd_aslength;

			if (changed & BGP_ATTRINFO_CHANGED) {
			    bcopy(rcv_attr, &attrinfo, sizeof(battrtype));
			    attrinfo.community = bgp_router_com_buffer;
			}

			if (changed & BGP_COMMUNITY_CHANGED) {
			    bcopy(rcv_attr->community, attrinfo.community,
				  COMMUNITY_LEN(rcv_attr->comcount));
			    bgp_special_community(attrinfo.community,
						  attrinfo.comcount, 
						  &pathinfo);
			}
		    }

		    attr = NULL;
		    pathinfo.flags |= BFLAG_VALID;
		    if (!bgp_route_map_rcv_set(bgp, map, &attrinfo, &pathinfo,
					       &aspath, &changed))
			return;
		}
	    }
	}


	/*
	 * See if we are allowed to hear about this network from this peer.
	 */
	if (bgp->bgp_distlistin)
	    if (!route_check(bgp->bgp_distlistin, network, mask))
		goto add_rcvdpath;

	/*
	 * First time in the for loop, create an attribute entry in the 
	 * table if the path information is valid.
	 */
	if (!attr && (pathinfo.flags & BFLAG_VALID)) {
	    if (changed)
		attr = bgp_find_set_attr(&attrinfo, aspath, rcv_attr);
	    else
		attr = rcv_attr;

	    if (!attr) {
		goto free_and_return;
	    } else 
		mem_lock(attr);			/* suspend-lock */
	}

	if (bgpupdate_acl_debug(network, mask))
	    if (pathinfo.flags & BFLAG_VALID)
		buginf("\nBGP: %i rcv UPDATE about %i%m, next hop %i,"
		       " path %s metric %d", bgp->address, network, mask,
		       pathinfo.gateway,
		       bgp_path2string(attr->as_path, attr->aslength,
				       bgp_router_path_buffer,
				       ASCII_PATHBUFSIZE), attr->metric);
	    else
		buginf("\nBGP: %i rcv UPDATE about %i%m -- denied",
		       bgp->address, network, mask);

	
	/*
	 * Create new BGP entry if one doesn't already exist
	 * If entry exists, find the path from the neighbor.
	 *
	 * At this stage, BFLAG_VALID indicates that policies permitted
	 * this prefix.
	 */
	net = bgp_netlookup(network, mask);
	if (!net) {
	    if ((pathinfo.flags & BFLAG_VALID) == 0)
		goto add_rcvdpath;
	    net = bgp_create_net(network, mask);
	    if (!net) {
		errmsg(&msgsym(NOMEMORY, BGP), "path");
		bgp_reset(NULL, NULL);
		free(attr);			/* unlock suspend-lock */
		free(rcv_attr);			/* unlock suspend-lock */
		return;
	    }
	} else {
            /*
             * If the path is malformed, BFLAG_VALID will not be set.
             */
	    if (bgp->flags & BN_SOFT_RECONFIG_INOK)
		ix = bgp_find_usedpath(net, bgp->address,
				       (pathinfo.flags & BFLAG_VALID));
	    else
		ix = bgp_find_path(net, bgp->address);
	}

	if (ix == BGP_NULL_INDEX) {	/* no path from this neighbor */
	    if ((pathinfo.flags & BFLAG_VALID) == 0)
		goto add_rcvdpath;
	    net = bgp_add_path(net, &pathinfo, attr, &ix, BGP_NULL_INDEX,
			       BGP_NULL_INDEX);
	    if (!net) {
		errmsg(&msgsym(NOMEMORY, BGP), "path");
		bgp_reset(NULL, NULL);
		free(attr);			/* unlock suspend-lock */
		goto free_and_return;
	    }
	    do_bestpath = TRUE;
	} else {

	    /*
	     * check if the entry it is identical update. While checking
	     * do not consider the suppressed flag. If an entry is 
	     * valid and suppressed, it will get unsuppressed at the
	     * right time.
	     * If multiple parents and duplicate, the rcv_attr from
	     * this neighbor could have changed but resulting in same 
	     * set_attr. Example : rcv MED changed but have inbound set
	     * MED. Blow the parent list.
	     *
	     */
	    path = &net->paths[ix];
	    if (pathinfo.flags & BFLAG_VALID) {
		if (((pathinfo.flags & BFLAG_IDENTICAL) ==
		     (path->flags & BFLAG_IDENTICAL)) &&
		    (attr == path->attr) &&
		    (pathinfo.gateway == path->gateway) &&
		    (pathinfo.weight == path->weight)) {
		    if (bgpupdate_acl_debug(network, mask))
			buginf("...duplicate ignored");
		    if (attr->alist && attr->alist->next)
			bgp_free_parent_alist(attr);
		    goto add_rcvdpath;

		} else if (bgp_dampening_enabled && !bgp->bgp_internal) {
		    if (!bgp_damp_update(net, &pathinfo, attr, ix)) {
			errmsg(&msgsym(NOMEMORY, BGP), "dampening");
			bgp_reset(NULL, NULL);
			free(attr);		/* unlock suspend-lock */
			goto free_and_return;
		    }
		    do_bestpath = TRUE;

		} else {
		
		    /*
		     * Non-identical valid path is received. Replace the
		     * existing path with the new one.
		     */
		    bgp_replace_path(net, path, &pathinfo, attr, ix);
		    do_bestpath = TRUE;
		}
	    } else {

		/* 
		 * If we are here, then the received path is not
		 * valid. Delete the existing path.
		 */
		bgp_delete_path(net, ix);
		if ((net->best == ix) ||
		    (net->paths[ix].flags & BFLAG_MULTIPATH)) {
		    deleted_best = TRUE;
		    do_bestpath = TRUE;
		}
	    }
	}

	/*
	 * Call bgp_bestpath to make appropriate change to the best
	 * path.
	 */
	if (do_bestpath)
	    bgp_bestpath(net, ix, deleted_best);

    add_rcvdpath:
        /*
         * If soft reconfiguration inbound is enabled and if the path is
         * not malformed, store it. Otherwise, punt.
         */
	if (bgp->flags & BN_SOFT_RECONFIG_INOK) {
	    if ((pathinfo.flags & BFLAG_ILLEGAL) == 0) {
		used_idx = (pathinfo.flags & BFLAG_VALID) ?
		    ix : BGP_NULL_INDEX;
	        if (!bgp_add_rcvdpath(net, network, mask, used_idx,
                        &pathinfo, rcv_attr, rcvd_nexthop, bgp, changed)) {
		    bgp_reset(NULL, "unable to add received path");
		    free(attr);
		    goto free_and_return;
		}
	    }
	    else {
		if (bgpupdate_acl_debug(network, mask))
		    buginf("\nBGP: %i Prefix %i%m rejected "
			   "by inbound soft reconfiguration",
			   bgp->address, network, mask);
	    }
	}

	/* Only check suspend every four packets
	 */
	if (!(net_processed & 0x3))
	    process_may_suspend();
    }
    free(attr);

    /*
     * Release any temporary storage for transit attributes 
     */
 free_and_return:
    free(attrinfo.transptr);
    free(rcv_attr);

    /* must have had an error somewhere */
    if (netlen != 0) {
	if (bgp_debug)
	   buginf("\nBGP: synchronization error processing update NLRI");
	bgp_send_notification(bgp, BNOT_UPDATE, BNOT_UPDATE_MALFORMED,
			      NULL, 0);
    }
}

/*
 * bgp3_find_update
 * Look in the outbound update cache for a matching update.  If there is
 * one, add the network to it and return the update.  If there isn't one,
 * then return NULL.
 */
BGP_PRIVATE bmsgtype *
bgp3_find_update (battrtype *attr, ipaddrtype gateway, bnettype *net,
		  route_maptype *map)
{
    bmsgtype *msg;

    msg = bgpupdatetable[nethash((ulong)attr)];
    for ( ; msg; msg = msg->next) {
	if ((msg->attr == attr) &&
	    (msg->gateway == gateway) && 
	    (msg->map == map)) {

	    /*
	     * This matches, try adding it to the end of the update.
	     */
	    if (msg->length + sizeof(ipaddrtype) > msg->datagramsize)
		continue;
	    bcopy(&net->network, msg->dataptr, sizeof(ipaddrtype));
	    msg->dataptr += sizeof(ipaddrtype);
	    msg->length += sizeof(ipaddrtype);
	    return (msg);
	}
    }
    return (NULL);
}

/*
 * bgp4_find_update
 * Look in the outbound update cache for a matching update.  If there is
 * one, add the network to it and return the update.  If there isn't one,
 * then return NULL.
 */

BGP_PRIVATE bmsgtype *
bgp4_find_update (battrtype *attr, ipaddrtype gateway, bnettype *net,
		  route_maptype *map, ulong metric)
{
    bmsgtype *msg;
    ushort unfeasable;
    int len;
    uchar buffer[sizeof(ipaddrtype)+1];;

    msg = bgpupdatetable[nethash((ulong)attr)];
    for ( ; msg; msg = msg->next) {
	if ((msg->attr == attr) &&
	    (msg->gateway == gateway) &&
	    (msg->map == map) &&
	    (msg->metric == metric)) {

	    /*
	     * This matches, try adding it to the update.
	     */
	    if (msg->length + sizeof(ipaddrtype) + 1 > msg->datagramsize)
		continue;

	    /* if this is an unreachable */
	    if (attr == 0 && gateway == 0) {
		len = bgp_writeprefix(buffer, net->network, net->mask);

		unfeasable = GETSHORT(msg->unfeasable);
		unfeasable += len;
		PUTSHORT(msg->unfeasable, unfeasable);

		/* safe bcopy for overlaps */
		memmove(msg->attributes+len, msg->attributes,
			msg->dataptr - msg->attributes);

		/* now copy the prefix to the end of the buffer */
		bcopy(buffer, msg->attributes, len);
		msg->attributes += len;

	    } else		/* append to buffer */
		len = bgp_writeprefix(msg->dataptr, net->network, net->mask);

	    msg->dataptr += len;
	    msg->length += len;

	    return (msg);
	}
    }
    return (NULL);
}

/*
 * bgp_format_unreachable
 * Format an unreachable and add a network to it.  Return FALSE if it
 * failed (usually due to a transient memory problem).
 */

BGP_PRIVATE boolean
bgp3_format_unreachable (neighbortype *bgp, bnettype *net)
{
    int attrbytes;
    bmsgtype *msg;
    uchar *cp;

    msg = bgp_getbuffer(BGP_MAXBYTES, BGP_TYPE_UPDATE, FALSE);
    if (!msg)
	return (FALSE);

    cp = (uchar *) msg->dataptr;

    /*
     * Point to attributes size ushort
     */
    msg->attributes = cp;
    memset(cp, 0, sizeof(ushort));
    cp += sizeof(ushort);

    /*
     * Put in the ORIGIN attribute
     */
    *cp++ = UPF_TRANSIT;
    *cp++ = ATT3_ORIGIN;
    *cp++ = 1;
    *cp++ = ORIGIN_INCOMPLETE;

    /*
     * Put in the AS PATH attribute
     * NOTE: We only put in an external path here due to a bug in the ENSS.
     * Otherwise, don't ever include a path.
     */
    *cp++ = UPF_TRANSIT;
    *cp++ = ATT3_PATH;
    if (!bgp->bgp_internal) {
	*cp++ = PATHSIZE;
	PUTSHORT(cp, bgp_confed_id ? bgp_confed_id : pdb_BGP->asystem);
	cp += PATHSIZE;
    } else
	*cp++ = 0;

    /*
     * Put in the NEXT_HOP attribute.
     */
    *cp++ = UPF_TRANSIT;
    *cp++ = ATT3_NEXTHOP;
    *cp++ = sizeof(ipaddrtype);
    bcopy(&bgp->tcb->localhost.ip_addr, cp, sizeof(ipaddrtype));
    cp += sizeof(ipaddrtype);

    /*
     * Put in the UNREACHABLE attribute 
     */
    *cp++ = UPF_TRANSIT;
    *cp++ = ATT3_UNREACHABLE;
    *cp++ = 0;

    /*
     * Now go back and fill in the size of the attribtues region.
     */
    attrbytes = cp - msg->attributes - sizeof(ushort);
    PUTSHORT(msg->attributes, attrbytes);

    /*
     * Finish with the list of networks.  Currently just one.
     */
    bcopy(&net->network, cp, sizeof(ipaddrtype));
    cp += sizeof(ipaddrtype);

    /*
     * Now go back and fill in the length of the entire message.
     */
    msg->length = (cp - msg->dataptr) + BGP_HEADERBYTES;

    if (bgpupdate_acl_debug(net->network, net->mask))
	buginf("\nBGP: %i send UPDATE %i%m -- unreachable",
	       bgp->address, net->network, net->mask);

    msg->next = bgpupdatetable[0];
    bgpupdatetable[0] = msg;
    msg->attr = 0;
    msg->gateway = 0;
    msg->map = NULL;
    msg->dataptr = cp;
    return (TRUE);
}

/*
 * bgp4_format_unreachable
 * Format a withdrawl and add a network to it.  Return FALSE if it
 * failed (usually due to a transient memory problem).
 */

BGP_PRIVATE boolean
bgp4_format_unreachable (neighbortype *bgp, ipaddrtype network, 
			 ipaddrtype mask)
{
    bmsgtype *msg;
    uchar *cp;
    ushort len;
    static ushort zero = 0;

    msg = bgp_getbuffer(BGP_MAXBYTES, BGP_TYPE_UPDATE, FALSE);
    if (!msg)
	return (FALSE);
    cp = (uchar *) msg->dataptr;

    msg->unfeasable = cp;
    cp += sizeof(ushort);

    len = bgp_writeprefix(cp, network, mask);
    cp += len;

    PUTSHORT(msg->unfeasable, len);

    msg->attributes = cp;
    PUTSHORT(msg->attributes, zero);
    cp += sizeof(ushort);

    if (bgpupdate_acl_debug(network, mask))
	buginf("\nBGP: %i send UPDATE %i%m -- unreachable",
	       bgp->address, network, mask);

    msg->length = (cp - msg->dataptr) + BGP_HEADERBYTES;
    msg->next = bgpupdatetable[0];
    bgpupdatetable[0] = msg;
    msg->attr = 0;
    msg->gateway = 0;
    msg->map = NULL;
    msg->dataptr = cp;

    return (TRUE);
}

/*
 * bgp3_format_update
 * BGP2/3 peers only
 * Format an update message for a reachable network.  Return FALSE if there
 * is a memory problem.
 */

BGP_PRIVATE boolean
bgp3_format_update (neighbortype *bgp, battrtype *attr, bpathtype *path,
		    ipaddrtype gateway, bnettype *net, route_maptype *map)
{
    ulong metric;
    boolean sendmetric, dupe, community_ok;
    int hashvalue, pathbytes, attrbytes, len, seglen;
    int comlen, comcount;
    bmsgtype *msg;
    uchar *cp, *bp, *pathptr, *cpi, segtype;
    ushort as;
    ulong *comptr;

    /*
     * Figure out if we are going to send an inter-AS metric in this update
     */
    community_ok = (bgp->flags & BN_COMMUNITYOK);
    sendmetric = FALSE;
    metric = 0;

    /*
     * Initially attr->community is the community to be sent in updates.
     */
    comptr = attr->community;
    comcount  = attr->comcount;

    if (map) {
	/*
	 * If automatic metric generation is enabled
	 *
	 *    If we are traversing an AS boundary, set the MED to our
	 *    distance from the original entry point.
	 *    For the sourced and donot get the best metric for gateway 
	 *    (it could be 0.0.0.0). The IGP metric for these routes are 
	 *    available in attr->metric. 
	 */
	if ((map->set_list.set_flags & ROUTE_MAP_METRICTYPE_SET) &&
	    (map->set_list.metric_type == ROUTE_MAP_SET_INTERNAL)) {
	    if (!bgp->bgp_internal) { 
		if (!(path->flags && BFLAG_SOURCED))
		    metric = bgp_best_metric(path->gateway);
		else
		    metric = attr->metric;
		sendmetric = TRUE;
	    }
	}

	if ((map->set_list.set_flags & ROUTE_MAP_METRIC_SET) &&
	    !map->set_list.metric.igrp_metric) {
	    sendmetric = bgp_get_routemap_setmetric(map, &metric);
	}

	/*
	 * Apply route-map to community attribute
	 * If needed use the bgp_router_com_buffer for merging.
	 */
	if (community_ok && map->set_list.community) {
	    if (map->set_list.set_flags & ROUTE_MAP_COMMUNITY_ADD) {
		bcopy(comptr, bgp_router_com_buffer, COMMUNITY_LEN(comcount));
		comcount = bgp_com_union(map->set_list.community->com_array,
					 map->set_list.community->count,
					 bgp_router_com_buffer, comcount,
					 BGP_MAXCOMMUNITY);
		comptr = bgp_router_com_buffer;
	    } else {
		comcount = map->set_list.community->count;
		comptr = map->set_list.community->com_array;
	    }	
	}    

    }

    /*
     * If we are sending this route to one of our internal peers, we want
     * to send on the same metric that we received, so it can make the same
     * decisions we made.
     *
     * otherwise...
     *
     * If default-metric has been set, use it (compatibility with old
     * behavior in bgp3 code).
     *
     */
    if (!sendmetric) {
	if (bgp->bgp_internal &&
	    (path->flags & (BFLAG_METRIC|BFLAG_SOURCED))) {
	    metric = attr->metric;
	    sendmetric = TRUE;
	} else if (pdb_BGP->defaultmetricflag) {
	    metric = pdb_BGP->default_metric;
	    sendmetric = TRUE;
	}
    }

    if (metric > MAXUSHORT)
	metric = MAXUSHORT;

    /*
     * Write update for a single network.
     */

    msg = bgp_getbuffer(BGP_UPDATESIZE, BGP_TYPE_UPDATE, FALSE);
    if (!msg)
	return (FALSE);

    cp = (uchar *) msg->dataptr;

    /*
     * Point to attributes size ushort
     */
    msg->attributes = cp;
    memset(cp, 0, sizeof(ushort));
    cp += sizeof(ushort);

    /*
     * Put in the ORIGIN attribute
     */
    *cp++ = UPF_TRANSIT;
    *cp++ = ATT3_ORIGIN;
    *cp++ = 1;
    *cp++ = attr->origin;

    /*
     * Convert the BGP4 path into a BGP3 compatible path and also
     * compute the converted size of the BGP3 path
     */
    pathbytes = 0;
    if (!bgp->bgp_internal)
	pathbytes += PATHSIZE;

    memset(bgp_router_path_buffer, 0, ASCII_PATHBUFSIZE);
    pathptr = bgp_router_path_buffer;
    for (len = attr->aslength, bp = attr->as_path; len > 0; len -= 2) {
	segtype = *bp++;
	for (seglen = *bp++;
	     (seglen > 0) && (len > 0);
	     seglen--, len -= PATHSIZE, bp += PATHSIZE) {
	     /* BGP3 peers are never in confederations, strip the confed */
	     if (segtype != ASENTRY_SEQUENCE) {
		 as = GETSHORT(bp);
		 dupe = FALSE;
		 for (cpi = bgp_router_path_buffer; cpi < pathptr;
		      cpi += PATHSIZE)
		     if (GETSHORT(cpi) == as) {
			 dupe = TRUE;
			 break;
		     }
		 if (!dupe &&
		     (as != pdb_BGP->asystem) &&
		     (as != bgp_confed_id)) {
		     PUTSHORT(pathptr, as);
		     pathptr += PATHSIZE;
		     pathbytes += PATHSIZE;
		 }
	     }
	}
    }

    /*
     * Put in the AS PATH attribute
     */
    if (pathbytes <= 255) {
	*cp++ = UPF_TRANSIT;
	*cp++ = ATT3_PATH;
	*cp++ = pathbytes;
    } else {
	*cp++ = UPF_EXTENDED+UPF_TRANSIT;
	*cp++ = ATT3_PATH;
	PUTSHORT(cp, pathbytes);
	cp += sizeof(ushort);
    }
    if (!bgp->bgp_internal) {
	PUTSHORT(cp, bgp_confed_id ? bgp_confed_id : pdb_BGP->asystem);
	cp += PATHSIZE;
	pathbytes -= PATHSIZE;
    }
    if (pathbytes) {
	bcopy(bgp_router_path_buffer, cp, pathbytes);
	cp += pathbytes;
    }
    /*
     * Put in the NEXT_HOP attribute.
     */
    *cp++ = UPF_TRANSIT;
    *cp++ = ATT3_NEXTHOP;
    *cp++ = sizeof(ipaddrtype);
    PUTLONG(cp, gateway);
    cp += sizeof(ipaddrtype);

    /*
     * Put in the INTER-AS METRIC attribute if necessary
     */
    if (sendmetric && !bgp->bgp_internal) {
	*cp++ = UPF_OPTIONAL;
        *cp++ = ATT3_METRIC;
	*cp++ = sizeof(ushort);
	PUTSHORT(cp, metric);
	cp += sizeof(ushort);
    }
    /*
     * Put in the communities attribute next.
     */
    if (community_ok) {
	comlen = COMMUNITY_LEN(comcount);
	if (comlen) {
	    if (comlen <= 255) {
		*cp++ = UPF_OPTIONAL + UPF_TRANSIT;
		*cp++ = ATT3_COMMUNITY;
		*cp++ = comlen;
	    } else {
		*cp++ = UPF_OPTIONAL + UPF_EXTENDED + UPF_TRANSIT;
		*cp++ = ATT3_COMMUNITY;
		PUTSHORT(cp, comlen);
		cp += sizeof(ushort);
	    }
	    bcopy(comptr, cp, comlen);
	    cp += comlen;
	}
    }

    /*
     * Put in any unrecognized transit path attributes
     */
    if (attr->translen) {
	bcopy(attr->transptr, cp, attr->translen);
	cp += attr->translen;
    }

    /*
     * Now go back and fill in the size of the attribtues region.
     */
    attrbytes = cp - msg->attributes - sizeof(ushort);
    PUTSHORT(msg->attributes, attrbytes);

    /*
     * Finish with the list of networks.  Currently just one.
     */
    bcopy(&net->network, cp, sizeof(ipaddrtype));
    cp += sizeof(ipaddrtype);

    /*
     * Now go back and fill in the length of the entire message.
     */
    msg->length = (cp - msg->dataptr) + BGP_HEADERBYTES;

    if (bgpupdate_acl_debug(net->network, net->mask)) {
	buginf("\nBGP: %i send UPDATE %i%m, next %i, metric %u, path %s",
	       bgp->address, net->network, net->mask, gateway, metric,
	       bgp_path2string(attr->as_path, attr->aslength,
			       bgp_router_path_buffer,
			       ASCII_PATHBUFSIZE));
    }

    hashvalue = nethash((ulong)attr);
    msg->next = bgpupdatetable[hashvalue];
    bgpupdatetable[hashvalue] = msg;
    msg->attr = attr;
    msg->gateway = gateway;
    msg->map = map;
    msg->dataptr = cp;

    return (TRUE);
}

/*
 * bgp4_format_update
 * BGP4 peers only
 * Format an update message for a reachable network.  Return FALSE if there
 * is a memory problem.
 */

BGP_PRIVATE boolean
bgp4_format_update (neighbortype *bgp, ipaddrtype network, ipaddrtype mask,
		    battrtype *attr, bpathtype *path, ipaddrtype gateway, 
		    route_maptype *map, boolean reflected,
		    ulong metric, boolean sendmetric)
{
    ulong localpref, destpref;
    boolean sendlocalpref, common_admin, community_ok;
    boolean same_cluster, senddestpref;
    int hashvalue, pathbytes, attrbytes, comlen, comcount, aslen, seglen;
    int clusterlength, debug_pathlen, i;
    bmsgtype *msg;
    ushort *set_aspath, output_as, destpref_as, asnumber;
    uchar *cp, *asstart, set_aslength, set_ascount, segtype, *debug_pathstart;
    uchar *cpi, origin;
    ulong *comptr;

    /*
     * Peer in the same AS or under common administration
     */
    common_admin = (bgp->bgp_internal || (bgp->flags & BN_COMMONADMIN));
    community_ok = (bgp->flags & BN_COMMUNITYOK);
    sendlocalpref = senddestpref = FALSE;
    localpref = 0;
    destpref = 0;
    destpref_as = 0;
    set_aslength = 0;
    set_ascount = 0;
    set_aspath = NULL;
    clusterlength = 0;
    same_cluster = FALSE;
    origin = attr->origin;			/* start with attr origin */

    /*
     * Initially attr->community is the community to be sent in updates.
     */
    comptr = attr->community;
    comcount  = attr->comcount;

    if (map && !reflected) {
	if (map->set_list.set_flags & ROUTE_MAP_LOCALPREF_SET) {
	    localpref = map->set_list.local_pref;
	    sendlocalpref = TRUE;
	}

	if (map->set_list.set_flags & ROUTE_MAP_DESTPREF_SET) {
	    destpref = map->set_list.destpref;
	    destpref_as = pdb_BGP->asystem;
	    senddestpref = TRUE;
	}
	
	if (map->set_list.set_flags & ROUTE_MAP_ORIGIN_SET) {
	    origin = map->set_list.origin;
	}

	if ((map->set_list.set_flags & ROUTE_MAP_ASPATH_PND_SET) &&
	    map->set_list.as_path) {
	    set_aspath = map->set_list.as_path;
	    set_ascount = map->set_list.ascount;
	    set_aslength = (set_ascount * PATHSIZE);
	}
 	
	/*
	 * Apply route-map to community attribute
	 * If needed use the bgp_router_com_buffer for merging.
	 */
	if (community_ok && map->set_list.community) {
	    if (map->set_list.set_flags & ROUTE_MAP_COMMUNITY_ADD) {
		bcopy(comptr, bgp_router_com_buffer, COMMUNITY_LEN(comcount));
		comcount = bgp_com_union(map->set_list.community->com_array,
					 map->set_list.community->count,
					 bgp_router_com_buffer, comcount,
					 BGP_MAXCOMMUNITY);
		comptr = bgp_router_com_buffer;
	    } else {
		comcount = map->set_list.community->count;
		comptr = map->set_list.community->com_array;
	    }	
	}   
    }

    /*
     * If we haven't specified localpref in a route-map, then send either
     * what we have or the magic default value.
     */
    if (!sendlocalpref) {
	sendlocalpref = TRUE;
	if (path->flags & BFLAG_LOCALPREF)
	    localpref = attr->localpref;
	else
	    localpref = bgp_default_localpref;
    }

    /*
     * If we haven't specified destpref in a route-map, then send
     * the destpref we have.
     */
    if (!senddestpref && 
	(path->flags & BFLAG_DESTPREF)) {
	destpref = attr->destpref;
	destpref_as = attr->destpref_as;
	senddestpref = TRUE;
    }

    /*
     * If we are sending this route to one of our internal peers or to a
     * peer under common administration, we want to send on the same metric 
     * that we received, so it can make the same decisions we made.
     *
     * If it's an external peer, we don't want to pass on a metric from a
     * non sourced route because that would make the BGP learned metric
     * transitive across ASs which is evil.
     *
     * otherwise...
     *
     * If default-metric has been set, use it (compatibility with old
     * behavior in bgp3 code).
     *
     */
    if (!sendmetric) {
	if (path->flags & BFLAG_METRIC) {
	    if (common_admin ||
		(path->flags & (BFLAG_SOURCED|BFLAG_AGGREGATE))) {
		metric = attr->metric;
		sendmetric = TRUE;
	    }
	} else if (!reflected && pdb_BGP->defaultmetricflag) {
	    metric = pdb_BGP->default_metric;
	    sendmetric = TRUE;
	}
    }

    /*
     * Write update for a single network.
     */

    /*
     * In case of external neighbors we're going to need to tack on our AS 
     * and AS-path set via route-maps.
     *
     * (a) if we are a transit node in a confederation, merely add ourselves
     *     to the confederation path
     * (b) if not...
     *     (1) if we are leaving a confederation, strip the confederation off
     *     (2) add our AS to the path
     */
    asstart   = attr->as_path;
    aslen     = attr->aslength;

    if (bgp->bgp_internal)
	pathbytes = 0;
    else {
	if (bgp_confed_id) {
	    if (common_admin) {
		segtype = ASENTRY_SEQUENCE;
		output_as = pdb_BGP->asystem;
	    } else {
		/* leaving a confederation */
		segtype = AS_SEQUENCE;
		output_as = bgp_confed_id;

		/* strip off confederation */
		if (aslen && (*asstart == ASENTRY_SEQUENCE)) {
		    seglen     = 2 + (*(asstart+1) * PATHSIZE);
		    asstart   += seglen;
		    aslen     -= seglen;
		}
	    }
	} else {		/* not in a confederation */
	    segtype = AS_SEQUENCE;
	    output_as = pdb_BGP->asystem;
	}

	/*
	 * Private AS stripping.
	 * If there is only one AS sequence containing private AS
	 * number, strip the private AS sequence. Do not strip if the
	 * neighbor AS is part of the sequence.
	 *
	 */

	if (bgp->flags & BN_REMOVE_PRIVATE_AS) {
	    
	    cpi = asstart;
	    i   = aslen - 2;
	    if (*cpi++ == AS_SEQUENCE) {
		for(seglen = *cpi++; (seglen > 0); 
		    seglen--, i -= PATHSIZE, cpi += PATHSIZE) {

		    asnumber = GETSHORT(cpi);
		    if ((asnumber == bgp->yoursystem) ||
			!bgp_private_as(asnumber))
			break;
		}
		
		if (i == 0)
		    aslen = 0;
	    }
	}

	cp = bgp_router_path_buffer;

	/*
	 * If we're already in the proper segment, just add our
	 * AS (or confederation) to the segment, otherwise start
	 * a new segment and put our identification in it.
	 *
	 * Append the as-path within the first path segment, if we are
	 *   Not in confederation, prepend to the existing AS_SEQUENCE
	 *   path segment.
	 *   Within a confederation, prepend to the existing
	 *   ASENTRY_SEQUENCE path segment.
	 *   Leaving a confederation, prepend to the AS_SEQUENCE since
	 *   ASENTRY_SEQUENCE path segment would have been stripped by now.
	 */
	if (aslen && (*asstart == AS_SEQUENCE) && (segtype == AS_SEQUENCE)) {
	    asstart++;
	    *cp++ = AS_SEQUENCE;
	    *cp++ = 1 + *asstart++ + set_ascount;

	    PUTSHORT(cp, output_as);
	    cp += PATHSIZE;

	    if (set_aslength) {
		bcopy(set_aspath, cp, set_aslength);
		cp += set_aslength;
	    }

	    aslen -= 2;
	    /* the rest of the sequence will get copied in later */

	} else {
	    if (aslen &&
		(*asstart == ASENTRY_SEQUENCE) &&
		(segtype == ASENTRY_SEQUENCE)) {
		asstart++;
		seglen = *asstart++;
		*cp++ = ASENTRY_SEQUENCE;
		*cp++ = 1 + seglen + set_ascount;

		PUTSHORT(cp, output_as);
		cp += PATHSIZE;

		/* copy in the rest of the current confederation */
		seglen *= PATHSIZE;
		bcopy(asstart, cp, seglen);
		cp += seglen;

		asstart += seglen;
		aslen -= (seglen + 2);

	    } else {
		*cp++ = segtype;
		*cp++ = 1 + set_ascount;

		PUTSHORT(cp, output_as);
		cp += PATHSIZE;
	    }

	    if (set_aslength) {
		bcopy(set_aspath, cp, set_aslength);
		cp += set_aslength;
	    }
	}
	pathbytes = (cp - (uchar *)bgp_router_path_buffer);
    }

    msg = bgp_getbuffer(BGP_UPDATESIZE, BGP_TYPE_UPDATE, FALSE);
    if (!msg)
	return (FALSE);
    cp = (uchar *) msg->dataptr;

    /*
     * Set up empty withdrawn routes area
     */
    msg->unfeasable = cp;
    memset(msg->unfeasable, 0, sizeof(ushort));
    cp += sizeof(ushort);

    /*
     * Set up size of attributes section
     */
    msg->attributes = cp;
    memset(msg->attributes, 0, sizeof(ushort));
    cp += sizeof(ushort);

    /*
     * Put in the ORIGIN attribute
     */
    *cp++ = UPF_TRANSIT;
    *cp++ = ATT4_ORIGIN;
    *cp++ = 1;
    *cp++ = origin;

    /*
     * Put in the AS PATH attribute
     */
    if (pathbytes+aslen <= 255) {
	*cp++ = UPF_TRANSIT;
	*cp++ = ATT4_PATH;
	*cp++ = pathbytes+aslen;
    } else {
	*cp++ = UPF_EXTENDED+UPF_TRANSIT;
	*cp++ = ATT4_PATH;
	PUTSHORT(cp, pathbytes+aslen);
	cp += sizeof(ushort);
    }

    /* copy in the stuff we just computed */
    bcopy(bgp_router_path_buffer, cp, pathbytes);
    debug_pathstart = cp;
    cp += pathbytes;

    /* copy in any additional AS path information */
    bcopy(asstart, cp, aslen);
    cp += aslen;
    debug_pathlen = cp - debug_pathstart;

    /*
     * Put in the NEXT_HOP attribute.
     */
    *cp++ = UPF_TRANSIT;
    *cp++ = ATT4_NEXTHOP;
    *cp++ = sizeof(ipaddrtype);
    bcopy(&gateway, cp, sizeof(ipaddrtype));
    cp += sizeof(ipaddrtype);

    /*
     * Put in the local preference or multi-exit discriminator attribute
     * if necesssary
     */
    if (sendmetric) {
	*cp++ = UPF_OPTIONAL;
        *cp++ = ATT4_EXITDISC;
	*cp++ = sizeof(ulong);
	PUTLONG(cp, metric);
	cp += sizeof(ulong);
    }

    /*
     * send local preference to internal neigbors or external neighbor
     * under the common administration.
     */
    if (common_admin && sendlocalpref) {
	*cp++ = UPF_TRANSIT;
        *cp++ = ATT4_LOCALPREF;
	*cp++ = sizeof(ulong);
	PUTLONG(cp, localpref);
	cp += sizeof(ulong);
    }

    /*
     * Put the destination preference attribute if necessary
     */
    if (bgp_dpa_enabled && senddestpref) {
	*cp++ = UPF_TRANSIT+UPF_OPTIONAL;
	*cp++ = ATT4_DESTPREF;
	*cp++ = sizeof(ulong) + sizeof(ushort);
	PUTSHORT(cp, destpref_as);
	cp += sizeof(ushort);
	PUTLONG(cp, destpref);
	cp += sizeof(ulong);
    }

    /*
     * Put in the ATOMIC_AGGREGATE attribute if necessary
     */
    if (path->flags & BFLAG_ATOMICAGG) {
	*cp++ = UPF_TRANSIT;
        *cp++ = ATT4_ATOMICAGG;
	*cp++ = 0;			/* zero length flag */
    }

    /*
     * Put in the AGGREGATOR attribute if necessary
     */
    if (path->flags & BFLAG_AGGREGATOR) {
	*cp++ = UPF_OPTIONAL+UPF_TRANSIT;
        *cp++ = ATT4_AGGREGATOR;
	*cp++ = PATHSIZE+sizeof(ipaddrtype);
	PUTSHORT(cp, attr->aggregator.as);
	cp += PATHSIZE;
	PUTLONG(cp, attr->aggregator.ip);
	cp += sizeof(ipaddrtype);
    }

    /*
     * Put in the communities attribute next.
     */
    if (community_ok) {
	comlen = COMMUNITY_LEN(comcount);
	if (comlen) {
	    if (comlen <= 255) {
		*cp++ = UPF_OPTIONAL + UPF_TRANSIT;
		*cp++ = ATT4_COMMUNITY;
		*cp++ = comlen;
	    } else {
		*cp++ = UPF_OPTIONAL + UPF_EXTENDED + UPF_TRANSIT;
		*cp++ = ATT4_COMMUNITY;
		PUTSHORT(cp, comlen);
		cp += sizeof(ushort);
	    }
	    bcopy(comptr, cp, comlen);
	    cp += comlen;
	}
    }


    /*
     * If we are a RR and update is to an internal neighbor, cluster-list 
     * and Originator attributes must be included.
     * For internal routes use the router-id in the path as the originator
     * and for other routes (sourced & external) use the local router-id.
     */
    if (bgp->bgp_internal && bgp_cluster_id) {
	if (attr->rrinfo) {
	    clusterlength = attr->rrinfo->clusterlength;
	    if (attr->rrinfo->clusterlist[0] == bgp_cluster_id)
		same_cluster = TRUE;
	}
	
	if (!same_cluster)
	    clusterlength += sizeof(ulong);

	*cp++ = UPF_OPTIONAL;
	*cp++ = ATT4_CLUSTLIST;
	*cp++ = clusterlength;

	if (!same_cluster) {
	    PUTLONG(cp, bgp_cluster_id);
	    cp += sizeof(ulong);
	}

	if (attr->rrinfo) {
	    bcopy(attr->rrinfo->clusterlist, cp, attr->rrinfo->clusterlength);
	    cp += attr->rrinfo->clusterlength;
	}

	/*
	 * Add the Originator attribute 
	 */
	*cp++ = UPF_OPTIONAL;
	*cp++ = ATT4_ORIGINATOR;
	*cp++ = sizeof(ipaddrtype);
	if (path->flags & BFLAG_INTERNAL)
	    PUTLONG(cp, path->router_id);
	else					/* sourced by us */
	    PUTLONG(cp, pdb_BGP->bgp_rtr_id);
	cp += sizeof(ipaddrtype);
    }


    /*
     * Put in any unrecognized transit path attributes
     */
    if (attr->translen) {
	bcopy(attr->transptr, cp, attr->translen);
	cp += attr->translen;
    }

    /*
     * Now go back and fill in the size of the attributes
     * region.
     */
    attrbytes = cp - msg->attributes - sizeof(ushort);
    PUTSHORT(msg->attributes, attrbytes);

    /*
     * Finish with the list of networks.  Currently just one.
     */
    cp += bgp_writeprefix(cp, network, mask);

    /*
     * Now go back and fill in the length of the entire message.
     */
    msg->length = (cp - msg->dataptr) + BGP_HEADERBYTES;

    if (bgpupdate_acl_debug(network, mask)) {
	buginf("\nBGP: %i send UPDATE %i%m, next %i, metric %u, path %s",
	       bgp->address, network, mask, gateway, metric,
	       bgp_path2string(debug_pathstart, debug_pathlen,
			       bgp_router_path_buffer,
			       ASCII_PATHBUFSIZE));  
    }

    hashvalue = nethash((ulong)attr);
    msg->next = bgpupdatetable[hashvalue];
    bgpupdatetable[hashvalue] = msg;
    msg->attr = attr;
    msg->gateway = gateway;
    msg->map = map;
    msg->metric = metric;
    msg->dataptr = cp;

    return (TRUE);
}

/*
 * bgp_next_hop
 * Compute next hop for this network
 *
 * Initial assumption is that we are the next hop gateway.
 * If we already have a next hop, use it if it is on the same cable
 * as the peer.  This makes the DDN and X.25 networks work properly.
 * If we learned this route via the IGP, check in routing table to see
 * if our next hop address is another router on the same cable as the
 * BGP peer.  If so, then announce that other router as the first hop
 * (third party BGP).
 */

static inline
ipaddrtype bgp_next_hop (neighbortype *bgp, bnettype *net, int pathindex,
			 route_maptype *map)
{
    idbtype *nextidb;
    ipaddrtype gateway, gate;
    ipaddrtype subnet, subnet_mask;
    bpathtype *path;
    ndbtype *ndb;
    rdbtype *rdb;
    int i;

    /*
     * Route map overrides anything else
     */
    if (map)
	if ((map->set_list.set_flags & ROUTE_MAP_NEXT_HOP_SET) &&
	    map->set_list.next_hop &&
	    (map->set_list.next_hop->address.type == ADDR_IP)) {
	    gateway = map->set_list.next_hop->address.ip_addr;
	    goto done;
	}

    /*
     * Default is our local TCP address.
     */
    gateway = bgp->tcb->localhost.ip_addr;

    /*
     * If next hop calculation is disabled, return default.
     * If multiple paths installed in the IP table, return self as
     * nexthop 
     */
    if ((bgp->flags & BN_NOHOPCALC) ||
	(net->flags & BNET_MULTIPATH))
	goto done;

    /*
     * BGP4 allows IGP's to carry non-connected next hops
     * An EBGP peer with common administrative control  is passed the same 
     * next_hop as an internal peer.
     */
    if ((bgp->bgp_internal && (bgp->protocol_version >= 4)) ||
	(!bgp->bgp_internal && (bgp->flags & BN_COMMONADMIN))) {
	/* short circuit all of the connected checks */
	subnet = subnet_mask = 0;
    } else {

	/*
	 * Figure out which subnet our neighbor is on.  If it isn't connected,
	 * then return default.
	 */
	if (!ip_match_connected_subnet(bgp->address, &subnet, &subnet_mask))
	    goto done;
	    
	subnet &= subnet_mask;
    }

    path = &net->paths[pathindex];

    /*
     * If we are a locally generated aggregate, then we must be the
     * next hop.
     */
    if (path->flags & BFLAG_AGGREGATE)
	goto done;

    /*
     * If we have a decent BGP next hop, use that.
     */
    if (path->gateway && ((path->gateway & subnet_mask) == subnet)) {
	if (bgpupdate_acl_debug(net->network, net->mask))
	    buginf("\nBGP: NEXT_HOP part 1 net %i%m, neigh %i, next %i",
		   net->network, net->mask, bgp->address, path->gateway);
	gateway = path->gateway;
	goto done;
    }

    /*
     * Look to see if there's a usable route via that subnet in the
     * routing table.
     */
    ndb = bgp_fwdtable_lookup(net->network, net->mask);
    if (ndb) {
	/*
	 * If we've gotten to here, we're probably summarizing a route.
	 * However, because of VLSM, we need to avoid the temptation of
	 * chosing the subnet default route and instead we just advertize
	 * ourselves as the next hop.
	 */
	if (ndb->attrflags & NET_SUBNETTED)
	    goto done;

	for (i = 0; i < ndb->routecount; i++) {
	    rdb = ndb->rdb[i];
	    if (!rdb)
		break;
	    gate = rdb->gateway;
	    if (gate && ((gate & subnet_mask) == subnet)) {
		if (bgpupdate_acl_debug(net->network, net->mask))
		    buginf("\nBGP: NEXT_HOP part 2 net %i%m, "
			   "neigh %i, next %i", net->network, net->mask,
			   bgp->address, gateway); 
		gateway = gate;
		goto done;
	    }
	}
    }

    /*
     * Still nothing?  Well, try the brute force recursive lookup.
     * Be careful, as first_hop can return a martian network number when
     * we're connected.
     */

    gate = net->network;
    nextidb = iprouting_first_hop(&gate, gate, net->mask, ROUTINGDEPTH,
				  IP_PATH_EXACT_MATCH, NULL);
    if (nextidb && ((gate & subnet_mask) == subnet) && 
	!martian(gate, TRUE) && (gate != net->network)) {
	if (bgpupdate_acl_debug(net->network, net->mask))
	    buginf("\nBGP: NEXT_HOP part 3 net %i%m, neigh %i, next %i",
		   net->network, net->mask, bgp->address, gate); 
	gateway = gate;
    }

 done:

    /*
     * Whatever happens, don't advertise an address back to a neighbor as a
     * next_hop. This breaks the ENSS.
     */

    if (gateway == bgp->address || martian(gateway, TRUE)) {
	if (bgpupdate_acl_debug(net->network, net->mask))
	    buginf("\nBGP: Can't advertise %i%m to %i with NEXT_HOP %i",
		   net->network, net->mask, bgp->address, gateway);
	gateway = bgp->tcb->localhost.ip_addr;
    }
    return(gateway);
}

/*
 * bgp_update_policy
 */
BGP_PRIVATE inline boolean
bgp_update_policy (neighbortype *bgp, bnettype *net, battrtype *attr,
		   route_maptype **map)
{
    route_map_nametype *routemap_name;
    route_maptype *adv_map;
    bgp_cond_adv_maptype *camap;
    boolean advertise;

    /*
     * These statics help us short-cut expensive path2string conversion.
     */
    static battrtype    *last_attr = NULL;
    static char		 buffer[ASCII_PATHBUFSIZE] = "";

    if (attr != last_attr)
	if (!bgp_path2string(attr->as_path, attr->aslength, 
			     buffer, sizeof(buffer)))
	    return (FALSE);

    last_attr = attr;
    if (bgp->bgp_outlist && 
	!bgp_filter_list_check(attr, bgp->bgp_outlist, buffer))
	return(FALSE);

    /*
     * If we are unsuppressing a suppressed entry, use the unsuppress
     * routemap or else use the outbound route-map.
     */
    if (net->flags & BNET_SUPPRESSED)
	routemap_name = bgp->bgp_unsuppress_map;
    else
	routemap_name = bgp->bgp_routemapout;

    if (routemap_name) {
	*map = bgp_route_map_check(routemap_name, attr, net, net->network,
				   net->mask, buffer, TRUE);
	if (!*map)
	    return(FALSE);
    }

    /* If there is an advertise map, check to see if this net matches any
     * advertise map and should be advertised or withdrawn depending on
     * the state of the condition map.  Any other state than ADVERTISED
     * (including UNINITIALIZED) is treated as withdraw to avoid sending
     * and withdrawing.  We'd rather advertise a little later than cause
     * a flap.
     */
    if (bgp->bgp_cond_adv) {
	for (camap = bgp->bgp_cond_adv; camap; camap = camap->next) {
	    adv_map = bgp_route_map_check(camap->bgp_advertise_map, attr, net,
					  net->network, net->mask, buffer,
					  TRUE);
	    if (adv_map) {
		advertise = (camap->adv_state == BGP_ADV_STATE_ADVERTISED);
		if (bgpupdate_acl_debug(net->network, net->mask))
		    buginf("\nBGP: %i %i matches advertise map %s, state: %s",
			   net->network, net->mask,
			   camap->bgp_advertise_map->mapname,
			   bgp_adv_state_strings[camap->adv_state]);
		if (advertise)
		    *map = adv_map;
		return(advertise);
	    }
	}
    }

    return(TRUE);
}

/*
 * bgp_send_update
 * Write an update messsage about a path to a BGP peer
 */

BGP_PRIVATE boolean
bgp_send_update (neighbortype *bgp, bnettype *net, int *msg_count,
		 boolean clear_bit)
{
    ipaddrtype gateway;
    bpathtype *path;
    battrtype *attr;
    bmsgtype *msg;
    route_maptype *map;
    boolean reflected, client_ibgp, sendmetric;
    ulong metric;

    reflected = FALSE;
    client_ibgp = (bgp->flags & BN_RR_CLIENT);
    metric = 0;
    sendmetric = FALSE;

    /*
     * See if we allow this network to be advertised to this peer.  If
     * outbound soft reconfiguration is inn progress, withdraw just in
     * case the previous policy allowed us to advertiese this network 
     */
    if (bgp->bgp_distlistout) {
	if (!route_check(bgp->bgp_distlistout, net->network, net->mask)) {
	    if (bgp->flags & BN_SOFT_RECONFIG_OUT)
		goto unreachable;
	    else
		return (TRUE);
	}
    }

    if (!pdb_outfilter(pdb_BGP, NULL, NULL, net->network)) {
	if (bgp->flags & BN_SOFT_RECONFIG_OUT)
	    goto unreachable;
	else
	    return (TRUE);
    }

    /*
     * Don't send default to BGP2/3 peers or to a neighbor to which we've
     * already sent default because of the "neighbor <x.x.x.x>
     * default-originate" command.
     */
    if (!net->network) {
	if (bgp->protocol_version < 4)
	    return (TRUE);

	if (bgp->bgp_default_state == BGP_DEFAULT_SENT)
	    return (TRUE);
    }

    /* can only send out 'natural' networks to old peers */
    if ((bgp->protocol_version < 4) &&
	(net->mask != get_majormask(net->network)))
	return (TRUE);
    
    /*
     * If there is no best path, send unreachable.
     * Note : Do not dereference path when there is no best path.
     */
    if (net->best == BGP_NO_BEST)
	goto unreachable;

    path = &net->paths[net->best];
    attr = path->attr;

    /*
     * If BFLAG_NOADV flag is set donot advertise.
     * If it is an external peer and NOEXPORT flag is set donot
     * send the net (exclude the external confederation peers).
     * If the net was previously announced to external peers we
     * need to withdraw it. (The BNET_SENT_EBGP flags will be
     * cleared for such nets by bgp_scan_walker.)
     */
    if ((path->flags & BFLAG_NOADV) || 
	(!bgp->bgp_internal && 
	 !(bgp_confed_id && 
	   (bgp->flags & BN_COMMONADMIN)) &&
	 (path->flags & BFLAG_NOEXPORT)) ||
	(!bgp->bgp_internal &&
	 (path->flags & BFLAG_LOCALAS)))
	goto unreachable;

    /*
     * Can we just send an unreachable instead?
     * If we don't have a best path, or it's through the neighbor that
     * we're sending to, then take a shortcut.
     * Never send an update about something we learned from this neighbor.
     * If this is an internal link, don't send updates about paths
     * learned via internal links.  All BGP speakers are fully meshed.
     */
    if (net->paths[net->best].neighbor == bgp->address)
	goto unreachable;
    if ((net->flags & BNET_SUPPRESSED) &&
	!bgp->bgp_unsuppress_map)
	goto unreachable;
    if (!pdb_BGP->sync_disabled && (path->flags & BFLAG_INTERNAL) &&
	!(path->flags & BFLAG_SYNCED))
	goto unreachable;

    /*
     * If we are an RR, we might reflect internal path to some IBGP 
     * neighbors.
     * If the path is learned from a client, reflect to all IBGP neighbors
     * except the originator of the route.
     * If the path is a non-client path, reflect it only to clients.
     */

     if (bgp->bgp_internal && (path->flags & BFLAG_INTERNAL)) {
	 if (!bgp_cluster_id)				/* normal operation */
	     goto unreachable;

	 if (path->flags & BFLAG_CLIENT) {
	     if (client_ibgp && !bgp_client_reflection)
		 goto unreachable;
	     if ((bgp->bgp_router_id == path->router_id) &&
		 !bgp->peergroup)
		 goto unreachable;
	 } else						/* normal IBGP path */
	     if (!client_ibgp)				/* normal IBGP peer */
		 goto unreachable;

	 reflected = TRUE;
     }	

    /*
     * Apply the per-neighbor update policies.
     */
    map = NULL;
    if (bgp->bgp_outlist || bgp->bgp_routemapout || bgp->bgp_cond_adv ||
	(net->flags & BNET_SUPPRESSED))			/* unsuppress-map */
	if (!bgp_update_policy(bgp, net, attr, &map))
	    goto unreachable;

    /*
     * Adjust next hop field as appropriate.
     */
    if (reflected)
	gateway = path->gateway;
    else
	gateway = bgp_next_hop(bgp, net, net->best, map);

    /*
     * Record if we're sending a path to a IBGP neighbor.
     */
    if (bgp->bgp_internal) {
	if (client_ibgp)
	    net->flags |= BNET_SENT_CLIENT;
	else
	    net->flags |= BNET_SENT_IBGP;
    } else {
	bgp_set_advertised_bit(net, bgp);
	net->flags |= BNET_SENT_EBGP;
    }
    
    if (map && !reflected) {
	/*
	 * For the sourced and aggregate routes donot get the best metric
	 * for gateway (it could be 0.0.0.0). The IGP metric for these
	 * routes are available in attr->metric. 
	 */
	if ((map->set_list.set_flags & ROUTE_MAP_METRICTYPE_SET) &&
	    (map->set_list.metric_type == ROUTE_MAP_SET_INTERNAL)) {
	    if (!(path->flags & (BFLAG_SOURCED|BFLAG_AGGREGATE)))
		metric = bgp_best_metric(path->gateway);
	    else
		metric = attr->metric;
	    sendmetric = TRUE;
	}

	if ((map->set_list.set_flags & ROUTE_MAP_METRIC_SET) &&
	    !map->set_list.metric.igrp_metric) {
	    sendmetric = bgp_get_routemap_setmetric(map, &metric);
	}
    }

    /*
     * Now find an update to add it to, or build a new one.
     */
    if (bgp->protocol_version < 4)
	msg = bgp3_find_update(attr, gateway, net, map);
    else
	msg = bgp4_find_update(attr, gateway, net, map, metric);

    if (msg) {
	if (bgpupdate_acl_debug(net->network, net->mask)) {
	    buginf("\nBGP: %i send UPDATE %i%m, next %i, path %s",
		   bgp->address, net->network, net->mask, gateway,
		   bgp_path2string(attr->as_path, attr->aslength,
				   bgp_router_path_buffer,
				   ASCII_PATHBUFSIZE));  
	}
    } else {
	*msg_count = *msg_count + 1;
	if (bgp->protocol_version < 4)
	    return (bgp3_format_update(bgp, attr, path, gateway, net, map));
	else
	    return (bgp4_format_update(bgp, net->network, net->mask, attr, 
				       path, gateway, map, reflected,
				       metric, sendmetric));
    }
    return (TRUE);


  unreachable:
    /*
     * If soft reconfiguration is in progress, always withdraw and 
     * clear the advertised bit if it is allowed. During normal update,
     * don't tell internal neighbors if we never had an external path.
     * Don't tell external neighbors if we never told that neighbor.
     */
    if (bgp->flags & BN_SOFT_RECONFIG_OUT) {
	if (!bgp->bgp_internal && clear_bit)
	    bgp_clear_advertised_bit(net, bgp);
    } else {
	if (bgp->bgp_internal) {
	    if ((!client_ibgp && 			/* normal ibgp */
		 !(net->flags & BNET_SENT_IBGP)) ||
		(client_ibgp && 
		 !(net->flags & BNET_SENT_CLIENT)))
		return (TRUE);
	} else {
	    if (bgp_suppress_unreachable(net, bgp, clear_bit))
		return(TRUE);
	}

	/*
	 * No need to send unreachables to a peer that's just coming up!
	 */
	if (bgp->version == 0)
	    return (TRUE);
    }

    if (bgp->protocol_version < 4)
	msg = bgp3_find_update(0, 0, net, NULL);
    else
	msg = bgp4_find_update(0, 0, net, NULL, 0);
    if (msg) {
	if (bgpupdate_acl_debug(net->network, net->mask))
	    buginf("\nBGP: %i send UPDATE %i%m -- unreachable",
		   bgp->address, net->network, net->mask);
    } else
	if (bgp->protocol_version < 4)
	    return (bgp3_format_unreachable(bgp, net));
	else
	    return (bgp4_format_unreachable(bgp, net->network, net->mask));
    return (TRUE);
}

/*
 * bgp_pgrp_enqueue_updates
 * Members of the same peer-group eligible for replication will get
 * a copy of all the messages. Create the msgheader and make them 
 * point to the data part of the calculated messages.
 */
BGP_PRIVATE void
bgp_pgrp_enqueue_updates (neighbortype *neighbor, ulong start_version,
			  boolean clear_bit, boolean abort) 
{
    int i;
    neighbortype *bgp, *pgrp;
    bmsgtype *newmsg, *msg;
    boolean firstpeer, verbose;
    bgp_header *header;
    ulong msgbytes, msgs, msgsize, maxmsgsize;

    verbose = (bgpupdate_debug && !bgpupdate_acl) || bgpevent_debug;
    firstpeer = TRUE;
    msgbytes = msgs = maxmsgsize = 0;
    pgrp = neighbor->peergroup;
    bgp = pgrp->peerhead;
    
    /*
     * Enqueue to members eligible for replication. If we are running
     * low on memory and if we had cleared advertised bit, try to send
     * all the messages as there is no way to recover the lost state
     * information.
     */
    for (; bgp; bgp = bgp->nextpeer) {
	if (bgp == neighbor)
	    continue;

	if (!(bgp->flags & BN_REPLICATE_UPDATE))
	    continue;
	bgp->flags &= ~BN_REPLICATE_UPDATE;

	if (mempool_is_empty(MEMPOOL_CLASS_LOCAL) &&
	    !clear_bit) {
	    for (; bgp; bgp = bgp->nextpeer)
		bgp->flags &= ~BN_REPLICATE_UPDATE;
	    return;
	}
	
	if (bgp->bgp_state != BGPS_ESTAB)
	    continue;

	for (i = 0; i < NETHASHLEN; ++i) {
	    msg = bgpupdatetable[i];
	    while(msg) {

		newmsg = malloc(sizeof(bmsgtype));
		if (!newmsg) {
		    errmsg(&msgsym(NOMEMORY, BGP), "message");
		    bgp_reset(NULL, NULL);
		    return;
		}
		    
		msgsize = msg->length;
		if (firstpeer) {
		    header = (bgp_header *)msg->datagramstart;
		    PUTSHORT(header->len, msgsize);

		    if ((GETSHORT(header->len) > BGP_MAXBYTES)&& bgp_debug)
			buginf("\nBGP: %i sending bad header length %d "
			       "should be %d", bgp->address, header->len, 
			       msgsize);

		    msgs++;
		    maxmsgsize = max(maxmsgsize, msgsize);
		    msgbytes += msgsize;
		}
		    
		newmsg->datagramsize = msgsize;
		newmsg->datagramstart = msg->datagramstart;
		newmsg->msgheadptr = msg;
		newmsg->gateway = bgp->address;
		bgp->outupdates++;

		mem_lock(msg);
		enqueue(&bgp->bgp_writeQ, newmsg);
		msg = msg->next;
	    }
	}

	/*
	 * Stopped in the middle. Update the upd_version field, as
	 * one of these neighbors might be used to start the next
	 * update.
	 * Also update the upd_checkpoint field to help in picking
	 * the correct neighbors for the next update/replication. 
	 */
	if (abort) {
	    bgp->bgp_upd_version = neighbor->bgp_upd_version;
	    bgp->bgp_upd_checkpoint = neighbor->bgp_upd_checkpoint;
	} else {
	    bgp->bgp_upd_version = 0;
	    bgp->bgp_upd_checkpoint = 0;
	    TIMER_START_JITTERED(bgp->bgp_last_upd_sent, 
				 bgp->bgp_minadverttime, BGP_UPDATE_JITTER);
	    bgp->version = start_version;
	}

	if (msgs && verbose) {
	    if (firstpeer) {
		buginf("\nBGP: %u updates (average = %u, maximum = %u)",
		       msgs, msgs ? msgbytes/msgs : 0, maxmsgsize);
		buginf("\nBGP: %i updates replicated for neighbors : %i", 
		       neighbor->address, bgp->address);
	    } else 
		buginf(", %i", bgp->address);
	}
	firstpeer = FALSE;
	process_may_suspend();
    }
    TIMER_START_JITTERED(pgrp->bgp_last_upd_sent, pgrp->bgp_minadverttime, 
			 BGP_UPDATE_JITTER);
}


/*
 * bgp_enqueue_updates
 * Take the updates that have been calculated in bgpupdatetable, and queue
 * them on the write queue for the neighbor.
 */

BGP_PRIVATE void
bgp_enqueue_updates (neighbortype *bgp)
{
    int i;
    bmsgtype *msg, *nmsg;
    bgp_header *header;
    ulong msgbytes, msgs, maxmsgsize;

    msgbytes = msgs = maxmsgsize = 0;

    for (i = 0; i < NETHASHLEN; i++) {
	msg = bgpupdatetable[i];
	bgpupdatetable[i] = NULL;
	while (msg) {
	    nmsg = msg->next;
	    msg->next = NULL;
	    header = (bgp_header *) msg->datagramstart;
	    PUTSHORT(header->len, msg->length);
	    if ((GETSHORT(header->len) > BGP_MAXBYTES) && bgp_debug) {
		buginf("\nBGP: %i sending bad header length %d should be %d",
		       bgp->address, header->len, msg->length);
	    }
	    msg->datagramsize = msg->length;

	    msgbytes += msg->datagramsize;
	    maxmsgsize = max(maxmsgsize, msg->datagramsize);
	    msgs++;

	    bgp->outupdates++;
	    enqueue(&bgp->bgp_writeQ, msg);
	    msg = nmsg;
	}
    }

    if (bgpupdate_debug && !bgpupdate_acl && msgs)
	buginf("\nBGP: %i %u updates enqueued (average=%u, maximum=%u)",
	       bgp->address, msgs, msgs ? msgbytes / msgs : 0, maxmsgsize);

}

/*
 * bgp_synchronize_pgrp_members
 * Find all the peergroup members that are synchronized with the candidate.
 * Mark them as eligible for update replication. If a synchronized member
 * is slow in consuming the update, do not mark him for replication.
 */

BGP_PRIVATE boolean
bgp_synchronize_pgrp_members (neighbortype *candidate)
{
    neighbortype *bgp;
    boolean found;

    found = FALSE;
    bgp = candidate->peergroup->peerhead;
    for (; bgp; bgp = bgp->nextpeer) {
	if (bgp == candidate)
	    continue;

	if ((bgp->bgp_state == BGPS_ESTAB) &&
	    (bgp->version == candidate->version) &&
	    (QUEUESIZE(&bgp->bgp_writeQ) < BGP_WRITELIMIT) &&
	    (bgp->bgp_upd_checkpoint == candidate->bgp_upd_checkpoint)) {
	    bgp->flags |= BN_REPLICATE_UPDATE;
	    found = TRUE;
	} else 
	    bgp->flags &= ~BN_REPLICATE_UPDATE;
    }
    
    if (found)
	return(TRUE);
    else
	return(FALSE);
}

/*
 * bgp_clear_peergroup_bit
 * Check if it is ok to clear the peergroup advertised bit.
 */

BGP_PRIVATE boolean
bgp_clear_peergroup_bit (neighbortype *candidate)
{
    neighbortype *bgp;

    /*
     * If candidate is the peergroup member with the least version, we
     * could clear the peergroup advertised bit after sending
     * withdrawn message.
     *
     * If a peergroup member with same version is not marked for 
     * replication, it indicates that this neighbor is falling back.
     * In that case, we cannot clear the advertised bit.
     */
    for (bgp = candidate->peergroup->peerhead; bgp; bgp = bgp->nextpeer) {
	if ((bgp == candidate) || 
	    (bgp->bgp_state != BGPS_ESTAB))
	    continue;

	if (bgp->version < candidate->version)
	    break;

	if ((bgp->version == candidate->version) &&
	    !(bgp->flags & BN_REPLICATE_UPDATE))
	    break;
    }

    if (bgp)
	return(FALSE);
    else
	return(TRUE);
}

/*
 * bgp_update_walker
 * Do BGP updates for a neighbor on a particular network.
 * If an entry has a version number greater than the BGP neighbor version,
 * send an update. Limit the number of messages generated to BGP_WRITELIMIT.
 * Callback from bgp_updates via tree walker. 
 */

BGP_PRIVATE int
bgp_update_walker (rntype *rn, va_list ap)
{
    bnettype *net	 = rn2bnettype(rn);
    neighbortype *bgp	 = va_arg(ap, neighbortype *);
    ulong *start_version = va_arg(ap, ulong *);
    int *count           = va_arg(ap, int *);
    boolean clear_bit    = va_arg(ap, boolean);
    boolean throttle, update_ok;

    throttle = FALSE;
    if ((net->version <= bgp->version) || (net->version > *start_version))
	return WALK_CONTINUE;

    if (net->network < bgp->bgp_upd_checkpoint)
	return WALK_CONTINUE;

    update_ok = bgp_send_update(bgp, net, count, clear_bit);
    if (*count >= BGP_WRITELIMIT)
	throttle = TRUE;

    if (mempool_is_empty(MEMPOOL_CLASS_LOCAL) || !update_ok || throttle) {
	if ((bgp->bgp_upd_version == *start_version) && !throttle) {
	    /*
	     * Malloc failed.  Try to send less
	     * next time.  It's always safe to decrease
	     * start_version as it will only cause us to
	     * send duplicate updates.  Note the cute hack
	     * here to compute the average between current
	     * and attempted, but at least request progress.
	     */
	    *start_version = (bgp->version + *start_version) >> 1; 
	    *start_version = max(*start_version, bgp->version + 1);
	}
	bgp->bgp_upd_checkpoint = net->network;
	if (bgpevent_debug)
	    buginf("\nBGP: %i%s update throttling, remote ver %u, "
		   "goal ver %u, checkpoint ver %u, stopped at %i",
		   bgp->address, throttle ? " peer-group" : "",
		   bgp->version, *start_version, bgp->bgp_upd_version,
		   net->network); 
	return WALK_ABORT;
    } 

    return WALK_CONTINUE;
}

/*
 * bgp_updates
 * Do BGP updates for a neighbor.
 * There is a version number for the master BGP table that is incremented each
 * time we get a new update.  Each entry maintains the version number of the
 * table when that entry was last updated.  Each BGP neighbor maintains a
 * version number indicating the version of the most recent items we sent it.
 * If an entry has a version number greater than the BGP neighbor version,
 * send an update.  When all entries at or above a certain version level have
 * been sent to a BGP peer, set the BGP peer's version number to that level.
 * Return TRUE if blocked.
 */

BGP_PRIVATE boolean
bgp_updates (neighbortype *bgp)
{
    ulong start_version, initial_start_version;
    ulong starting_runtime;
    boolean verbose, abort, clear_bit, replicate;
    int count;

    count = 0;
    abort = FALSE;
    verbose = (bgpupdate_debug && !bgpupdate_acl) || bgpevent_debug;
    if (bgp->bgp_state != BGPS_ESTAB)
	return (FALSE);

    /*
     * Throttling conditions:
     * - If the output queue for the neighbor is already full, don't
     *   compute more updates.  It only saps our memory.  Checkpoint our
     *   progress and continue later.
     * - Suspend occasionally to give TCP a chance to shove out updates.
     */
    if (bgp->bgp_upd_version)
	start_version = bgp->bgp_upd_version;
    else 
	start_version = bgptable_version;

    if (bgp->version >= start_version) 
	return (FALSE);

    /*
     * If the guy hasn't drained his queue below this threshold,
     * don't feed him more (slow link).
     * Note: Peer-group members will always come here with queue
     *       below the threshold.
     */
    if (!bgp->peergroup &&
 	(QUEUESIZE(&bgp->bgp_writeQ) >= BGP_WRITELIMIT)) {
	if (verbose)
	    buginf("\nBGP: %i write queue size of %d exceeded limit of %d"
		   " messages", bgp->address, QUEUESIZE(&bgp->bgp_writeQ),
		   BGP_WRITELIMIT);
	return (TRUE);
    }

    if (verbose)
	buginf("\nBGP: %i computing updates, neighbor version %d,"
	       " table version %d, starting at %i",
	       bgp->address, bgp->version, start_version,
	       bgp->bgp_upd_checkpoint);

    starting_runtime = runtime();
    initial_start_version = start_version;
    
    /*
     * If this is peergroup neighbor, find other members eligible for
     * update replication.
     */
    replicate = FALSE;
    if (bgp->peergroup)
	replicate = bgp_synchronize_pgrp_members(bgp);

    clear_bit = FALSE;
    if (!bgp->bgp_internal)
	clear_bit = (bgp->peergroup ? bgp_clear_peergroup_bit(bgp) : TRUE);

    if (rn_walktree_version(bgp_head, bgp->version, bgp_update_walker,
			    ip_radix_ver_successor, bgp, &start_version,
			    &count, clear_bit) == WALK_ABORT) {
	/*
	 * If walker has detected low memory condition or stoped
	 * due to peer-group throttling, remember the upd_version
	 * to continue later.
	 */
	abort = TRUE;
	bgp->bgp_upd_version = start_version;
    }

    if (replicate) {
	bgp_pgrp_enqueue_updates(bgp, start_version, clear_bit, abort);
    }
    bgp_enqueue_updates(bgp);

    if (verbose)
	buginf("\nBGP: %i update run %s, ran for %lums, neighbor version %lu, "
	       "start version %lu, throttled to %lu, check point net %i ",
	       bgp->address, abort ? "throttled" : "completed",
	       runtime() - starting_runtime, bgp->version, 
	       initial_start_version, start_version, 
	       abort ? bgp->bgp_upd_checkpoint : 0);

    if (abort)
	return(TRUE);

    TIMER_START_JITTERED(bgp->bgp_last_upd_sent, bgp->bgp_minadverttime, 
			 BGP_UPDATE_JITTER);
    bgp->version = start_version;
    bgp->bgp_upd_version = 0;
    bgp->bgp_upd_checkpoint = 0;
    process_may_suspend();
    return (FALSE);
}

/*
 * bgp_clean_dampinfo
 *
 * clean the dampinfo from all the paths of a net.
 */

BGP_PRIVATE void
bgp_clean_dampinfo (bnettype *net)
{
    int i, next;
    bpathtype *path;

    /*
     * If the path entry is maintained for history info, delete it.
     * Otherwise, free the dampinfo structure and reset the flag as
     * the path might have been suppressed.
     */

    for (i = net->firstpath; i != BGP_NULL_INDEX; i = next) {
	path = &net->paths[i];
 	next = path->next;
	if (path->dampinfo) {
	    if (path->flags & BFLAG_HISTORY)
		bgp_delete_path(net, i);
	    else {
		bgp_free_dampinfo(net, i);
		path->flags &= ~BFLAG_SUPPRESSED;
		bgp_bestpath(net, i, FALSE);
	    }
	}
    }
}

/*
 * bgp_free_dampinfo_walker
 */

BGP_PRIVATE int
bgp_free_dampinfo_walker (rntype *rn, va_list ap)
{
    bgp_clean_dampinfo(rn2bnettype(rn));
    return WALK_CONTINUE;
}

/*
 * bgp_free_dampctrlblk 
 *
 * Free malloced structures from dampening control block.  Do not
 * reset the other fields as this function could be called during
 * reconfiguration. 
 */
BGP_PRIVATE void
bgp_free_dampctrlblk (dampctrlblk *dcb)
{
    if (!dcb)
	return;

    dcb->next = NULL;
    
    free(dcb->decay_array);
    dcb->decay_array = NULL;
    dcb->decay_array_len = 0;

    free(dcb->reuse_helper);
    dcb->reuse_helper = NULL;
    dcb->reuse_helper_len = 0;
    free(dcb);
}

/*
 * bgp_create_decay_array
 * Function to generate the bgp_decay_array.
 */

BGP_PRIVATE boolean
bgp_create_decay_array (dampctrlblk *dcb)
{
    int offset, a, b, i;

    dcb->decay_array_len = dcb->halflife_time/BD_DECAY_INTERVAL;
    dcb->decay_array = malloc((dcb->decay_array_len + 1) * sizeof(ulong));
    if (!dcb->decay_array) {
	errmsg(&msgsym(NOMEMORY, BGP), "Decay array");
	return(FALSE);
    }

    /*
     * The decay for N ticks is the Nth square of precomputed decay rate.
     * ie decay_rate at N is (decay_rate at N-1) * decay_rate/tick.
     * 
     * Doing direct multiplication will exceed the MAXULONG. Instead
     * scale the numbers by an offset(60000) and do the multiplication.
     * let 
     *	   a = (decay_rate for 1 tick) - offset
     *     b = (decay_rate at N-1 tick) - offset 
     * Now decay_rate at 
     *	   N = (offset + a) * (offset + b)
     *       = (3600000000 + (a*b) + (a+b)*offset)/precision
     *       = (36000 + (a*b)/precision + ((a+b)*offset)/precision
     *
     * The last term could exceed the MAXULONG, hence replace it with
     * ((a+b)*6)/10.
     */

    offset = 60000;
    dcb->decay_array[1] = bgp_decayrate_table[dcb->halflife_time/ONEMIN];
    a = (dcb->decay_array[1] - offset);
    for (i = 2; i <= dcb->decay_array_len; ++i) {
	b = dcb->decay_array[i -1] - offset;
	dcb->decay_array[i] = (36000 + ((a * b)/BD_PRECISION) +
			       (((a + b) * 6) / 10));
    }
    return(TRUE);
}

/*
 * bgp_create_reuse_helper
 *
 * Given a penalty value, we could find the time it will take to reach
 * the reuse value as follows :
 * successively dividing it by 2 until the value is greater than 
 * twice the reuse-limit. Let N be the number of such division. Then 
 * the actual suppress time = (N * halflife-time) + (time for remaining 
 * value to reach reuse-limit).
 * 
 * The following function creates the array that helps in finding the
 * time for the penalty value to reach reuse-limit. The reuse_helper
 * has decay time for range of values from reuse to  (2 * reuse) in 
 * increments of 5.
 */

BGP_PRIVATE boolean
bgp_create_reuse_helper (dampctrlblk *dcb)
{
    int i, j, max_value, value;
    ulong *temp;

    max_value = (2 * dcb->reuse_penalty);
    dcb->reuse_helper_len = dcb->reuse_penalty/5;

    /*
     * Create a temporary array with decayed value for a range of decay
     * ticks upto one halflife time.
     * Start with twice the reuse-limit. It is usefull to create the 
     * residue array.
     */
    temp = malloc((dcb->decay_array_len + 1) * sizeof(ulong));
    if (!temp)
	return(FALSE);

    temp[0] = max_value;
    for (i = 1; i <= dcb->decay_array_len; ++i)
	temp[i] = (max_value * dcb->decay_array[i])/BD_PRECISION;

    dcb->reuse_helper = malloc((dcb->reuse_helper_len + 1) * sizeof(ulong));
    if (!dcb->reuse_helper) {
	free(temp);
	errmsg(&msgsym(NOMEMORY, BGP), "Decay reuse helper");
	return(FALSE);
    }

    /*
     * Use the temp array to calculate the decay time.
     * For each increment of 5, look in temp for the value that is 
     * just greater. From the index of that element calculate the approximate
     * decay time.
     */
    dcb->reuse_helper[0] = 0;	
    j = dcb->decay_array_len;
    value = dcb->reuse_penalty;
    for (i = 1; i <= dcb->reuse_helper_len; ++i) {
	value += 5;
	for (; j >= 0; --j)
	    if (temp[j] >= value)
		break;
	dcb->reuse_helper[i] = (dcb->decay_array_len - j) * BD_DECAY_INTERVAL;
    }

    free(temp);
    return(TRUE);
}

/*
 * bgp_create_reuse_array
 *
 * This function creates the reuse array that help in unsuppressing
 * the suppressed 'UP' route at the correct time. 
 */

BGP_PRIVATE boolean
bgp_create_reuse_array (dampctrlblk *dcb)
{
    ulong penalty, stime;
    ushort array_len;
    int offset;

    /*
     * From the configured maximum suppress time calculate the maximum
     * penalty allowed.
     *
     * The maximum penalty is the value that will take maximum suppress
     * time to reach the reuse value. As the value decays to half every
     * halflife-time, shifting reuse_penalty N number of times will get
     * the value that will decay to reuse_penalty in N halflife-time.
     * The suppress time might not be multiple of halflife-time. If so
     * shift the penalty one more time and this penalty is for suppress
     * time a little bit more than required. Calculate the number of 
     * ticks that will get the exact value and decay the penalty with
     * appropriate value using the decay array.
     */
    penalty = 0;
    offset = 0;
    stime = dcb->maximum_stime;
    while (stime >= dcb->halflife_time) {
	stime -= dcb->halflife_time;
	if (!penalty)
	    penalty = dcb->reuse_penalty << 1;
	else
	    penalty = penalty << 1;
    }

    if (stime) {
	penalty = penalty << 1;				/* extra shift */
	offset = (dcb->halflife_time - stime)/BD_DECAY_INTERVAL;
	if (offset)
	    penalty = (penalty * dcb->decay_array[offset])/BD_PRECISION;
    }

    dcb->maximum_penalty = penalty;
    if (dcb->maximum_penalty > BD_MAXIMUM_PENALTY) {
	printf("\n%% Maximum penalty is more than allowed maximum."
	       " Reduce maximum suppress time");
	return(FALSE);
    }
    if (dcb->maximum_penalty < dcb->suppress_penalty) {
	printf("\n%% Maximum penalty is less than suppress penalty."
	       " Increase maximum suppress time");
	return(FALSE);
    }

    /*
     * If the new reuse array is bigger than the already existing
     * reuse array, create a new one. 
     */
    array_len = dcb->maximum_stime/BD_REUSE_INTERVAL;
    if (array_len > bgp_reuse_array_len) {
	free(bgp_reuse_array);
	bgp_reuse_array = malloc((array_len + 1) * sizeof(bdampinfotype *));
	if (!bgp_reuse_array) {
	    errmsg(&msgsym(NOMEMORY, BGP), "Reuse Array");
	    return(FALSE);
	}

	bgp_reuse_array_len = array_len;
    }

    bgp_reuse_index = 1;
    return(TRUE);
}

/*
 * bgp_start_dampening
 * Allocate dampening control block and create different structures
 * needed for dampening.
 */
BGP_PRIVATE dampctrlblk *
bgp_start_dampening (ushort halflife_time, ushort reuse_penalty, 
		     ushort suppress_penalty, ushort maximum_stime)
{
    dampctrlblk *dcb;

    dcb = malloc(sizeof(dampctrlblk));
    if (!dcb) {
	errmsg(&msgsym(NOMEMORY, BGP), "dampening control block");	
	return(NULL);;
    }

    dcb->halflife_time = halflife_time*ONEMIN;
    dcb->reuse_penalty = reuse_penalty;
    dcb->suppress_penalty = suppress_penalty;
    dcb->maximum_stime = maximum_stime*ONEMIN;
    
    if (!bgp_create_decay_array(dcb))
	goto error;

    if (!bgp_create_reuse_helper(dcb))
	goto error;

    if (!bgp_create_reuse_array(dcb))
	goto error;

    if (!damp_chunks) {
	damp_chunks = chunk_create(sizeof(bdampinfotype),
				   DAMP_CHUNK_MAX, CHUNK_FLAGS_DYNAMIC,
				   NULL, 0, "BGP damper chunk");
	if (!damp_chunks) {
	    errmsg(&msgsym(NOMEMORY, BGP), "Damp chunks");
	    goto error;
	}
    }

    if (bgp_dampening_map && !bgp_dcb_array) {
	bgp_dcb_array = malloc(sizeof(dampctrlblk *) * BD_MAXIMUM_DAMPCTRLBLK);
	if (!bgp_dcb_array) {
	    errmsg(&msgsym(NOMEMORY, BGP), "Dampening control block array");
	    goto error;
	}
    }

    GET_TIMESTAMP(pdb_BGP->bgp_reusetime);	/* start the reuse timer */

    if (bgpdamp_debug)
	buginf("\nBGP: Created dampening structures with halflife time %d, "
	       "reuse/suppress %d/%d", halflife_time, reuse_penalty, 
	       suppress_penalty);
    return(dcb);

 error:
    bgp_free_dampctrlblk(dcb);
    return(NULL);
}

/*
 * bgp_deconfig_dampening
 */
BGP_PRIVATE void
bgp_deconfig_dampening (void)
{
    dampctrlblk *dcb, *next;

    dcb = (bgp_normal_dcb ? bgp_normal_dcb : bgp_routemap_dcb);
    bgp_normal_dcb = NULL;
    bgp_routemap_dcb = NULL;
    while (dcb) {
	next = dcb->next;
	bgp_free_dampctrlblk(dcb);
	dcb = next;
    }
    
    free(bgp_reuse_array);
    bgp_reuse_array = NULL;
    bgp_reuse_array_len = 0;
    free(bgp_dcb_array);
    bgp_dcb_array = NULL;
}

/*
 * bgp_config_dampening
 * Called from bgp_scanner to gracefully start/stop dampening.
 * Always clear all the dampening related info from the BGP table.
 * If dampening is enabled, create/sanity-check all the dampening
 * related structures.
 *
 * When route-map dampening is used, the access of dampening control
 * block corresponding to route-map entries should be fast. To acheive
 * it, we store the control block corresponding to each route-map
 * entry at the route-map index location in bgp_dcb_array. 
 */
BGP_PRIVATE void
bgp_config_dampening(void)
{
    dampctrlblk *dcb;
    route_map_headtype *head;
    route_map_settype *set;
    route_maptype *map;
    
    /*
     * clear all the dampening information from the BGP table.  As
     * dampening is turned off already, we could reset the cfg_changed
     * flag after timed walk. 
     */
    rn_walktree_timed(bgp_head, bgp_free_dampinfo_walker, ip_radix_successor);
    bgp_dampening_cfg_changed = FALSE;

    /*
     * When dampening is enabled/modified or the dampening route-map
     * is modified, upto-date information is in bgp_cfg_* or
     * route-map. Blow away all the normal and route map dampening
     * control blocks and recreate new ones as required.
     */
    bgp_deconfig_dampening();
    
    /*
     * Start normal dampening
     */
    if (bgp_cfg_halflife_time)
	bgp_normal_dcb = bgp_start_dampening(bgp_cfg_halflife_time,
					     bgp_cfg_reuse_penalty,
					     bgp_cfg_suppress_penalty, 
					     bgp_cfg_maximum_stime);

    /* 
     * Route-map dampening is configured. For each route-map entry
     * with dampening parameter, start dampening.
     */
    if (bgp_dampening_map) {
	head = bgp_dampening_map->head;
	if (!head)
	    goto exit;
	
	if (!(head->set_flags & ROUTE_MAP_DAMPENING_SET))
	    goto exit;

	map = (route_maptype *)head->map_queue.qhead;
	for(; map; map = map->next) {
	    set = &(map->set_list);
	    if (!(set->set_flags & ROUTE_MAP_DAMPENING_SET))
		continue;
	    
	    if (map->index >= BD_MAXIMUM_DAMPCTRLBLK) {
		bgp_deconfig_dampening();
		goto exit;
	    }

	    dcb = bgp_start_dampening(set->damp_value.halflife_time,
				      set->damp_value.reuse_limit,
				      set->damp_value.suppress_limit,
				      set->damp_value.maximum_stime);
	    if (dcb) {
		bgp_dcb_array[map->index] = dcb;
		if (bgp_routemap_dcb)
		    dcb->next = bgp_routemap_dcb;
		bgp_routemap_dcb = dcb;
	    } else {
		bgp_deconfig_dampening();
		break;
	    }
	}
    }

    if (bgp_normal_dcb || bgp_routemap_dcb)
	bgp_dampening_enabled = TRUE;

 exit:
    if (bgpdamp_debug)
	buginf("\nBGP: %sdampening %sabled",
	       bgp_dampening_map ? "route-map " : "",
	       bgp_dampening_enabled ? "en" : "dis");
}

/*
 * bgp_redist_check_subnets
 * Frontend to the bgp_redist_check function to perform redist check on
 * subnets while autosummary is enabled.
 */
BGP_PRIVATE boolean
bgp_redist_check_subnets (ndbtype *ndb)
{
    int i;
    sdbtype *sdb;

    if (pdb_BGP->auto_summary && (ndb->attrflags & NET_SUBNETTED)) {
	for (i = 0; i < NETHASHLEN; i++)
	    for (sdb = ndb->subnettable[i]; sdb; sdb = sdb->next)
		if (bgp_redist_check(sdb))
		    return TRUE;
	return FALSE;
    } else
	return bgp_redist_check(ndb);
}

/*
 * bgp_scan_walker
 * Periodically scan the routing and BGP tables and force them to be
 * consistent.  Purge old entries.
 * Called by bgp_nettable_scan via radix tree walker
 */

BGP_PRIVATE int
bgp_scan_walker (rntype *rn, va_list ap)
{
    bnettype *net	       = rn2bnettype(rn);
    ulong min_neighbor_version = va_arg(ap, ulong);

    bpathtype *path;
    ndbtype *ndb;
    boolean localflag;
    int i, j, k, next;
    route_maptype *adv_map;
    battrtype *attr;
    bgp_cond_adv_maptype *camap;

    /*
     * These statics help us short-cut expensive path2string conversion.
     */
    static battrtype    *last_attr = NULL;
    static char		 buffer[ASCII_PATHBUFSIZE] = "";

    /*
     * Except for the suppressed paths, check for dampinfo and see if
     * the decayed penalty is below the minimum. If it is so, if the
     * path is used to maintain damp history, delete the path or else
     * free the dampinfo structure..
     */
    if (bgp_dampening_enabled) {
  	for (i = net->firstpath; i != BGP_NULL_INDEX; i = next) {
	    path = &net->paths[i];
	    next = path->next;

 	    if (path->flags & BFLAG_RCVD_ONLY)
 		continue;

	    if (path->dampinfo) {
		if (path->flags & BFLAG_SUPPRESSED)
		    continue;

		bgp_decay_penalty(path->dampinfo);

		if (path->dampinfo->penalty <= BD_MINIMUM_PENALTY) {
		    if (path->flags & BFLAG_HISTORY)
			bgp_delete_path(net, i);
		    else
			bgp_free_dampinfo(net, i);
		}
	    }
	}
    }

    /*
     * If network has no valid paths, flush it.
     */
    if ((net->best == BGP_NO_BEST) && (net->pathcount == 0) &&
	(net->version <= min_neighbor_version)) {
	if (!rn_delete(&net->network_key, &net->mask_key, bgp_head))
	    errmsg(&msgsym(DELROUTE, BGP), net->network, net->mask);
	else
	    free(net);
	return WALK_CONTINUE;
    }

    /*
     * Nothing interesting to be done here...waiting to die
     */

    if (net->pathcount == 0)
	return WALK_CONTINUE;

    /*
     * IGP information about next hop may force a path change or a
     * reachability change.
     */
    bgp_bestpath(net, BGP_NULL_INDEX, FALSE);

    /*
     * If there is no best path, we certainly don't have to worry
     * about retracting it.
     */
    if (net->best == BGP_NO_BEST)
	goto next;

    /*
     * If there is a change in any advertising condition, check if
     * this net is allowed by the corresponding advertise map of that
     * condition or not.
     */
    if (bgp_advertise_cond_change) {
	attr = net->paths[net->best].attr;
	if (attr != last_attr) {
	    if (!bgp_path2string(attr->as_path, attr->aslength, 
				 buffer, sizeof(buffer)))
		return WALK_CONTINUE;
	    last_attr = attr;
	}

	for (camap = pdb_BGP->bgp_cond_adv; camap; camap = camap->link_next) {
	    if (!camap->cond_change)
		continue;
	    adv_map = bgp_route_map_check(camap->bgp_advertise_map, attr, net,
					  net->network, net->mask, buffer,
					  TRUE);
	    if (adv_map) {
		bgp_bump_net_version(net);
		if (bgpupdate_acl_debug(net->network, net->mask))
		    buginf("\nBGP: net %i %i matches ADV MAP %s: "
			   "bump version to %d", net->network, net->mask,
			   camap->bgp_advertise_map->mapname, net->version);
		break;
	    }
	}
    }

    /*
     * If the network is now in sync, and the best path is an
     * internal, and if we are not a RR, then we can clear 
     * SENT_IBGP.
     * NOTE : If we are a RR, we cannot clear the SENT_IBGP flag 
     *	      because the best internal path might have been
     *        Reflected by us.
     *
     * If the best path has LOCALAS set and if the net is sync, it 
     * indicates that this net has been withdrawn from all external 
     * neighbor. Then we can clear SENT_EBGP.
     * If the best path has NOEXPORT set and iff confederation is not
     * configured, clear the SENT_EBGP flag.
     */
    k = net->best;
    path = (bpathtype *) &net->paths[k];

    if (net->version <= min_neighbor_version) {
	if (!bgp_cluster_id && 
	    (path->flags & BFLAG_INTERNAL))
	    net->flags &= ~BNET_SENT_IBGP;
	if ((path->flags & BFLAG_LOCALAS) ||
	    (!bgp_confed_id && 
	     (path->flags & BFLAG_NOEXPORT)))
	    net->flags &= ~BNET_SENT_EBGP;
   }
    /*
     * If we are sourcing this network, make sure the IGP information
     * is still valid and that we're still redistributing it.
     */
    if (path->flags & BFLAG_SOURCED) {
	if (path->flags & BFLAG_LOCAL) {
	    if (path->flags & BFLAG_VALID) {
		localflag = FALSE;
		for (j = 0; j < pdb_BGP->netcount; j++) {
		    if ((pdb_BGP->networks[j] == net->network) &&
			(pdb_BGP->network_mask[j] == net->mask)) {
			localflag = TRUE;
			break;
		    }
		}
		if (!localflag) {
		    if (bgpupdate_acl_debug(net->network, net->mask))
			buginf("\nBGP: nettable_scan: invalidate local "
			       "path for %i%m", net->network, net->mask);
		    path->flags &= ~BFLAG_LOCAL;
		}
	    }
	}
	ndb = bgp_fwdtable_lookup(net->network, net->mask);
	if (!ndb || (!(path->flags & BFLAG_LOCAL) && 
		     !bgp_redist_check_subnets(ndb))) {
	    if (bgpupdate_acl_debug(net->network, net->mask))
		buginf("\nBGP: nettable_scan: invalidate sourced "
		       "path for %i%m", net->network, net->mask);
	    bgp_delete_path(net, k);
	    if (net->best == k)
		bgp_bestpath(net, k, TRUE);
	}
    }

  next:
    return WALK_CONTINUE;
}

/*
 * bgp_network_check
 */
BGP_PRIVATE ndbtype *
bgp_network_check (ndbtype *ndb, char *route_map) 
{
    ndbtype *sdb;
    pdbtype *src_pdb;
    int i;

    if (ndb->attrflags & NET_SUBNETTED) {
	for (i = 0; i < NETHASHLEN; i++) {
	    for (sdb = ndb->subnettable[i]; sdb; sdb = sdb->next) {
		if (sdb->metric >= METRIC_INACCESSIBLE)
		    continue;
		if (NDB2PROCTYPE(sdb) & PROC_BGP)
		    continue;
		if ((NDB2PROCTYPE(sdb) & PROC_OSPF) && 
		    (!bgp_ospftag_check(sdb)))
		    continue;
		if (route_map) {
		    src_pdb = ip_get_src_pdb(pdb_BGP, sdb, FALSE);
		    if (!src_pdb)
			src_pdb = INDEX2PDB(sdb->pdbindex);
		    pdb_BGP->route_map = route_map_ip_check(route_map, sdb, 
							    src_pdb);
		    if (!pdb_BGP->route_map)
			continue;
		}
		return sdb;
	    }
	}
	return NULL;
    } else {
	if ((NDB2PROCTYPE(ndb) & PROC_OSPF) &&
	    (!bgp_ospftag_check(ndb)))
	    return NULL;
	if (route_map) {
	    src_pdb = ip_get_src_pdb(pdb_BGP, ndb, FALSE);
	    if (!src_pdb)
		src_pdb = INDEX2PDB(ndb->pdbindex);
	    pdb_BGP->route_map = route_map_ip_check(route_map, ndb, src_pdb);
	    if (!pdb_BGP->route_map)
		return NULL;
	}
	return ndb;
    }
}

/*
 * bgp_nettable_scan
 * Periodically scan the routing and BGP tables and force them to be
 * consistent.  Purge old entries.
 */

BGP_PRIVATE void
bgp_nettable_scan (void)
{
    ndbtype *ndb, *target_ndb, *sdb, *db;
    int ndb_bucket, sdb_bucket;
    battrtype *attr, *pattr;
    int i, j, adv_state;
    boolean localflag, advertised;
    ipaddrtype mask;
    neighbortype *bgp;
    ulong min_neighbor_version;
    bgp_filter_list_ctype *fcache, *prev_fcache;
    bgp_route_map_ctype *rcache, *prev_rcache;
    bnettype *net;
    bgp_cond_adv_maptype *camap, *next_camap;

    bgp_reset_gwcache();		/* reset next hop cache */

    /*
     * Calculate the minimum neighbor version number.
     */
    min_neighbor_version = bgp_nettable_version;
    for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next) {
	if (bgp->bgp_state == BGPS_ESTAB)
	    min_neighbor_version = min(min_neighbor_version, bgp->version);
    }

    /* Evaluate all condition maps for conditional advertisement.  Check if
     * the condition is currently true or false and mark it if the condition
     * has changed from last time.  DECONFIGURED is marked as changed so
     * that the corresponding advertise-map has a chance to get re-advertised.
     * UNINITIALIZED is a newly-added map getting evaluated for the first
     * time.
     */
    for (camap = pdb_BGP->bgp_cond_adv; camap; camap = camap->link_next) {

	if (camap->adv_state == BGP_ADV_STATE_DECONFIGURED) {
 	    camap->cond_change = TRUE;
 	    bgp_advertise_cond_change = TRUE;
	    continue;
	}

	net = bgp_route_map_find_net(camap->bgp_condition_map);
	advertised = ((camap->cond_type && net) ||(!camap->cond_type && !net));
	adv_state  = advertised ? BGP_ADV_STATE_ADVERTISED :
	    BGP_ADV_STATE_NOT_ADVERTISED;
	if ((camap->adv_state != adv_state) ||
	    (camap->adv_state == BGP_ADV_STATE_UNINITIALIZED)) {

 	    camap->cond_change = TRUE;
 	    bgp_advertise_cond_change = TRUE;
	    camap->adv_state = adv_state;

	    if (bgpupdate_debug)
		buginf("\nBGP: Condition %s changes to %s",
		       camap->bgp_condition_map->mapname,
		       bgp_adv_state_strings[camap->adv_state]);
	}
    }

    /*
     * Update BGP routing table from main routing table.
     * These are the redistributed networks that have origin INCOMPLETE.
     */
    ndb_bucket = sdb_bucket = 0;
    ndb = sdb = NULL;

    while ((db = geteachroute(&ndb, &ndb_bucket, &sdb, &sdb_bucket, TRUE))) {
	target_ndb = db;

	if (!bgp_redist_check(db))
	    continue;

	/*
	 * If we are summarizing subnets, check to see if we should be
	 * considering dummy ndb.
	 */
	if (pdb_BGP->auto_summary && (db->attrflags & NET_SUBNET)) {
	    db = net_lookup(get_majornet(db->netnumber), FALSE);
	    if (!db)
		continue;

	    /* skip over all of the rest of the sdb's of this ndb */
	    sdb = NULL;
	    sdb_bucket = 0;
	}

	if (target_ndb->pdbindex != pdb_BGP->index)
	    db->advertisingmask |= pdb_BGP->mask;

	localflag = FALSE;

	if (db->attrflags & NET_SUBNETTED)
	    mask = get_majormask(db->netnumber);
	else
	    mask = db->netsmask;

	for (j = 0; j < pdb_BGP->netcount; j++) {
	    if ((db->netnumber == pdb_BGP->networks[j]) &&
		(mask == pdb_BGP->network_mask[j])) {
		localflag = TRUE;
		break;
	    }
	}

	if (!localflag)
	    bgp_edit(db, target_ndb, FALSE,
		     pdb_BGP->metric[db->pdbindex].weight);
    }

    process_may_suspend();

    /*
     * Update BGP routing table from main routing table.
     * These are the local networks with origin IGP or EGP.
     */
    for (i = 0; i < pdb_BGP->netcount; i++) {
	if (pdb_BGP->networkflags[i] & NETF_BACKDOOR)
	    continue;
	db = bgp_fwdtable_lookup(pdb_BGP->networks[i], 
				 pdb_BGP->network_mask[i]);
	if (!db)
	    continue;
	
	pdb_BGP->route_map = NULL;
	target_ndb = bgp_network_check(db, pdb_BGP->network_map[i]);
	if (!target_ndb)
	    continue;

	bgp_edit(db, target_ndb, TRUE, pdb_BGP->network_weight[i]);
	process_may_suspend();
    }

    /*
     * If the dampening configuration is changed reconfigure
     * dampening. 
     */
     if (bgp_dampening_cfg_changed)
	 bgp_config_dampening();
     
    /*
     * Scan BGP table, purge any old entries, update entries derived from
     * the IP routing table, and cause updates to IP routing table.
     */
    rn_walktree_timed(bgp_head, bgp_scan_walker, ip_radix_successor,
		      min_neighbor_version);

    /* Now reset the flags for conditional advertisement after we have
     * scanned the BGP table.  Remove the map that has been deconfigured.
     */
    bgp_advertise_cond_change = FALSE;
    for (camap = pdb_BGP->bgp_cond_adv; camap; camap = next_camap) {
	camap->cond_change = FALSE;
	next_camap = camap->link_next;
	if (camap->adv_state == BGP_ADV_STATE_DECONFIGURED) {
	    route_map_deconfig(&camap->bgp_advertise_map);
	    route_map_deconfig(&camap->bgp_condition_map);
	    bgp_delink_camap(camap);
	}
    }

    /*
     * Scan the path attribute table and remove old attributes.
     * If parent-list is present, decrement the refcount of the parent 
     * attribute and free the parent-list entries.
     * 
     */
    for (i = 0; i < NETHASHLEN; i++) {
	process_may_suspend();

	prev_fcache = (bgp_filter_list_ctype *) &bgp_filter_list_cache[i];
	fcache = prev_fcache->next;
	for (; fcache; prev_fcache = fcache, fcache = fcache->next) {
	    if (mem_refcount(fcache->attr) == 1) {
		prev_fcache->next = fcache->next;
		fcache->next = NULL;
		chunk_free(fcache_chunks, fcache);
		fcache = prev_fcache;
	    }
	}

	prev_rcache = (bgp_route_map_ctype *) &bgp_route_map_cache[i];
	rcache = prev_rcache->next;
	for (; rcache; prev_rcache = rcache, rcache = rcache->next) {
	    if (mem_refcount(rcache->attr) == 1) {
		prev_rcache->next = rcache->next;
		rcache->next = NULL;
		chunk_free(rcache_chunks, rcache);
		rcache = prev_rcache;
	    }
	}

	pattr = (battrtype *) &bgppathtable[i];
	for (attr = pattr->next; attr; pattr = attr, attr = attr->next) {
	    if (mem_refcount(attr) == 1) {
		pattr->next = attr->next;
		attr->next = NULL;
		bgp_free_parent_alist(attr);
		free(attr);
		attr = pattr;
	    }
	}
    }
}

/*
 * bgp_scanner
 * Scan the BGP and main routing tables to ensure consistency.
 * This is its own process since it takes a *VERY* long time to
 * scan a new BGP table.  We don't want to starve out other processes
 * and actions for runtime, especially things like KEEPALIVE handling.
 */

BGP_PRIVATE forktype
bgp_scanner (void)
{
    process_set_priority(PRIO_LOW);

    /* freeze exterior routing on crash */
    process_set_crashblock(THIS_PROCESS,TRUE);

    while (TRUE) {
	process_sleep_for(pdb_BGP->broadcasttime);
	if (bgpevent_debug)
	    buginf("\nBGP: scanning routing tables");
	bgp_nettable_scan();
    }
}

/*
 * bgp_rdb_update
 *
 * BGP, like IGRP, can make use of the extended metric vector that is
 * contained in the RDB.  If we successfully added an entry, go fix up
 * the metric vector.
 *
 * Currently, we could muck with hop count, delay, bandwidth, reliability,
 * and MTU, but we only have pseudo-hop-count information.
 */

BGP_PRIVATE void
bgp_rdb_update (rdbtype *rdb, bnettype *net)
{
    bpathtype *path;
    battrtype *attr;

    if (!rdb)
	return;

    path = &net->paths[net->best];
    attr = path->attr;

    rdb->hopcount = attr->pathhops;
}

/*
 * bgp_revise_route
 * Revise a BGP route in the IP routing table.  Only delete the existing
 * route if it contains invalid information.  Only install a new route
 * if we deleted the route in the routing table or there was not one
 * there.
 */

BGP_PRIVATE void
bgp_revise_route (bnettype *net, bnexthoptype *nexthop, idbtype *idb,
		  long metric, long flags, ulong tag, ushort route_type, 
		  int pathcount)
{
    ndbtype *ndb;
    rdbtype *rdb;
    int i, j;
    boolean ndb_delete;

    if (net->network && !net->mask) {
	errmsg(&msgsym(BADMASK, BGP), "bgp_revise_route", net->network);
	return;
    }

    ndb_delete = FALSE;
    ndb = ip_rtlookup(net->network, net->mask);
    if (ndb && (NDB2PROCTYPE(ndb) & PROC_BGP)) {
	if ((ndb->tag != tag) ||
	    ((ndb->attrflags & flags) != flags) ||
	    (ndb->route_type != route_type)) {
	    ndb_delete = TRUE;

	} else {

	    for (i = 0; i < ndb->routecount; ++i) {
		rdb = ndb->rdb[i];
		if (!rdb)
		    continue;
		
		for (j = 0; j < pathcount; ++j)
		    if ((rdb->gateway == nexthop[j].gateway) &&
			(rdb->infosource == nexthop[j].source))
			break;
	    
		if (j >= pathcount) {	
		    ndb_delete = TRUE;
		    break;
		} else
		    nexthop[j].install = FALSE;
	    }
	}

	if (ndb_delete)
	    ip_route_delete(pdb_BGP, net->network, net->mask, 
			    IPADDR_ZERO, IPADDR_ZERO, NULL, 0);
    }

    for (j = 0; j < pathcount; ++j) {
	if (!ndb_delete && !nexthop[j].install)
	    continue;

	if (bgpupdate_acl_debug(net->network, net->mask))
	    buginf("\nBGP: revise route installing %i/%i -> %i",
		   net->network, net->mask, nexthop[j].gateway);

	rdb = network_update(pdb_BGP, net->network, net->mask, 
			     nexthop[j].gateway, idb, metric, flags, 
			     nexthop[j].source, tag, NULL, route_type);
	if (rdb) { 
	    bgp_rdb_update(rdb, net);
	    bgp_nosource(net);
	    bgp_unsync(net);
	}
    }
}

/*
 * bgp_update_nettable_walker
 * Similar to bgp_updates, but just update the main IP routing table.
 * For now, this does not supercede all of the functionality of
 * bgp_nettable_scan.  Just enough stuff to get us to converge quickly in
 * the common case that some path changed.
 * Be careful because network_update can block.
 * Callback from bgp_update_nettable via tree walker.
 */

BGP_PRIVATE int
bgp_update_nettable_walker (rntype *rn, va_list ap)
{
    bnettype *net	= rn2bnettype(rn);
    ulong start_version = va_arg(ap, ulong);

    battrtype *attr;
    bpathtype *path, *mpath;
    boolean delete_net;
    ulong tag, netflags, metric;
    ushort type;
    idbtype *idb;
    route_maptype *map;
    char *pathstr;
    bnexthoptype nexthop[MAXROUTES];
    int pathcount, i;
    boolean debug;

    if ((net->version <= bgp_nettable_version) ||
	(net->version > start_version))
	return WALK_CONTINUE;

    debug = bgpupdate_acl_debug(net->network, net->mask);

    blocking_disabled = TRUE;

    /*
     * Check the version of the network against the version of the
     * main IP routing table.  Possibly delete things if something
     * has changed.
     */
    delete_net = FALSE;

    if (net->best == BGP_NO_BEST) {
	if (debug)
	    buginf("\nBGP: nettable_walker %i/%i no best path selected",
		   net->network, net->mask);
	delete_net = TRUE;
	goto next;
    }

    /*
     * If we learned the entry from BGP and it is still
     * valid, update the IP routing table.  Don't enter
     * routes whose gateways we can't reach.  If the BGP
     * entry was learned via an internal link, then set
     * NET_NOREDIST so it doesn't leak into an IGP (we're
     * not the exit gateway in that case).   If we sourced
     * this route, never inject it back into the routing table.
     */

    path = &net->paths[net->best];
    attr = path->attr;
    netflags = 0;
    if ((path->flags & BFLAG_INTERNAL) && !bgp_redistribute_ibgp)
	netflags |= NET_NOREDIST;
    if (path->flags & BFLAG_SOURCED) {
	if (debug)
	    buginf("\nBGP: nettable_walker %i/%i route sourced locally",
		   net->network, net->mask);
	delete_net = TRUE;
	goto next;
    }

    /*
     * If an unsynchronized path was best, don't install it, and remove it
     * if it's already there.
     */
    if (!pdb_BGP->sync_disabled &&
	(path->flags & BFLAG_INTERNAL) && !(path->flags & BFLAG_SYNCED)) {
	if (debug)
	    buginf("\nBGP: nettable_walker %i/%i not synchronized",
		   net->network, net->mask);
	delete_net = TRUE;
	goto next;
    }

    /*
     * Be compatible with existing BGP implementation, tag == next hop as
     * or 0 if local
     */
    tag = 0;
    if (attr->aslength)
	tag = GETSHORT(&attr->as_path[2]);

    /*
     * Set IGP metric to inter-as metric as specified in RFC1403.
     * Note: the BGP4/OSPF interaction document asks us to pull rabbits
     * out of our hats by doing magic with local-pref, but doesn't say
     * how,  so screw them.  This is backwards compatible with old
     * cisco implementations.  If they want, they can change it with
     * the table-map.
     */
    metric = path->flags & BFLAG_METRIC ? attr->metric : 0;

    pathcount = 0;
    if (net->flags & BNET_MULTIPATH)
	for (i = net->firstpath; i != BGP_NULL_INDEX; i = mpath->next) {
	    mpath = &net->paths[i];
	    if ((mpath->flags & BFLAG_MULTIPATH) &&
		(pathcount < pdb_BGP->multipath)) {
		nexthop[pathcount].gateway = mpath->gateway;
		nexthop[pathcount].source  = mpath->neighbor;
		nexthop[pathcount].install = TRUE;
		pathcount++;
	    }
	}
    else {
	nexthop[pathcount].gateway = path->gateway;
	nexthop[pathcount].source  = path->neighbor;
	nexthop[pathcount].install = TRUE;
	pathcount++;
    }

    if (pdb_BGP->table_map) {
	pathstr = bgp_path2string(attr->as_path, attr->aslength,
    			          bgp_router_path_buffer,
				  ASCII_PATHBUFSIZE);
	map = bgp_route_map_check(pdb_BGP->table_map, attr, net, net->network,
				  net->mask, pathstr, TRUE);
	if (map) {
	    if ((map->set_list.set_flags & ROUTE_MAP_METRIC_SET) &&
		!map->set_list.metric.igrp_metric)
		metric = map->set_list.metric.value.scalar;

	    if ((map->set_list.set_flags & ROUTE_MAP_NEXT_HOP_SET) &&
		map->set_list.next_hop &&
		(map->set_list.next_hop->address.type == ADDR_IP))
		for (i = 0; i < pathcount; ++i)
		    nexthop[i].gateway = 
			map->set_list.next_hop->address.ip_addr;

	    if (map->set_list.set_flags & ROUTE_MAP_AUTOTAG_SET) {
		tag = TAG_AUTOMATIC;
		if (map->set_list.set_flags & ROUTE_MAP_TAG_SET)
		    tag |= (map->set_list.tag << TAG_ARBSHIFT) & TAG_ARBPART;
		if (attr->aslength) {
		    tag |= GETSHORT(&attr->as_path[2]) & TAG_ASPART;
		    if (attr->aslength == 2+PATHSIZE) {
			tag |= TAG_PATHLEN_1;
			if (attr->origin != ORIGIN_INCOMPLETE)
			    tag |= TAG_COMPLETE;
		    } else
			tag |= TAG_PATHLEN_GE1;
		}
	    } else
		if (map->set_list.set_flags & ROUTE_MAP_TAG_SET)
		    tag = map->set_list.tag;
	}
    }

    if (path->flags & BFLAG_AGGREGATE) {
	idb = nullidb;
	type = RTYPE_BGP_LOC;
    } else {
	idb = NULL;
	if (net->flags & BNET_BACKDOOR)
	    type = RTYPE_BGP_LOC;
	else
	    type = (path->flags & (BFLAG_INTERNAL+BFLAG_CONFED)) ? 
		RTYPE_BGP_INT :RTYPE_BGP_EXT;
    }

    if (debug)
	buginf("\nBGP: nettable_walker %i/%i calling revise_route",
	       net->network, net->mask);

    bgp_revise_route(net, nexthop, idb, metric,
		     netflags, tag, type, pathcount);

 next:
    if (delete_net)
	ip_route_delete(pdb_BGP, net->network, net->mask,
			IPADDR_ZERO, IPADDR_ZERO, NULL, 0);


    blocking_disabled = FALSE;

    return WALK_CONTINUE;
}

/*
 * bgp_update_nettable
 * Parent routine, real work is done in bgp_update_nettable_walker
 * Similar to bgp_updates, but just update the main IP routing table.
 * For now, this does not supercede all of the functionality of
 * bgp_nettable_scan.  Just enough stuff to get us to converge quickly in
 * the common case that some path changed.
 * Be careful because network_update can block.
 */

BGP_PRIVATE void
bgp_update_nettable (void)
{
    ulong start_version;

    start_version = bgptable_version;
    if (bgp_nettable_version < start_version) {
	rn_walktree_version(bgp_head, bgp_nettable_version,
			    bgp_update_nettable_walker, ip_radix_ver_successor,
			    start_version);
	bgp_nettable_version = start_version;
    }
}

/*
 * bgp_docommand
 * Execute commands from a neighbor
 */

BGP_PRIVATE void
bgp_docommand (neighbortype *bgp)
{
    bmsgtype *msg;
    bgp_header *header;
    uchar *data;
    unsigned bytes, i;
    boolean fast;
    sys_timestamp read_start_time;

    GET_TIMESTAMP(read_start_time);
    while (!QUEUEEMPTY(&bgp->bgp_readQ)) {
	if (CLOCK_OUTSIDE_INTERVAL(read_start_time, BGP_PEER_MAXREADTIME))
	    return;
	process_may_suspend();
	bgp->inmsgs++;
	msg = dequeue(&bgp->bgp_readQ);
	header = (bgp_header *) msg->datagramstart;
	data = msg->datagramstart + BGP_HEADERBYTES;

	/*
	 * Get byte count (already range checked) and verify marker field
	 */
	bytes = GETSHORT(header->len);
	bytes -= BGP_HEADERBYTES;
	for (i = 0; i < BGP_MARKERBYTES; i++) {
	    if (header->marker[i] != BGP_MARKER_VALUE) {
		if (bgp_debug)
		    buginf("\nBGP: %i sync error", bgp->address);
		if (bgp->bgp_state != BGPS_CLOSING)
		    bgp_send_notification(bgp,BNOT_HEADER,BNOT_HEADER_SYNC,
					  NULL,0);
		free(msg);
		return;
	    }
	}

	if ((bgp->bgp_state != BGPS_CLOSING) ||
	    (header->type == BGP_TYPE_NOTIFICATION)) {
	    /*
	     * Switch on message type
	     */
	    switch (header->type) {
	    case BGP_TYPE_KEEPALIVE:
		if (bgpkeep_debug)
		    buginf("\nBGP: %i KEEPALIVE rcvd", bgp->address);
		if (bgp->bgp_state == BGPS_OPENCONFIRM)
		    bgp_statechange(bgp, BGPS_ESTAB);
		break;
	    case BGP_TYPE_OPEN:
		bgp_rcv_open(bgp, data, bytes);
		break;
	    case BGP_TYPE_NOTIFICATION:
		bgp->errsrcvd++;
		fast = bgp_rcv_notification(bgp, data, bytes);
		bgp_statechange(bgp, BGPS_CLOSING);
		bgp_reset(bgp, "notification received");
		if (fast)
		    bgp->flags |= BN_RESTART;
		break;
	    case BGP_TYPE_UPDATE:
		bgp->inupdates++;
		GET_TIMESTAMP(bgp->inupdatetime);
		if (bgp->protocol_version < 4)
		    bgp3_rcv_update(bgp, data, bytes);
		else
		    bgp4_rcv_update(bgp, data, bytes);
		break;
	    default:
		if (bgp_debug) 
		    buginf("\nBGP: %i unknown message type %#x", bgp->address,
			   header->type);
		bgp_send_notification(bgp, BNOT_HEADER, BNOT_HEADER_TYPE,
				      &header->type, 1);
		break;
	    }
	}
	if (msg->emergency && !bgp_emergency_msg)
	    bgp_emergency_msg = msg;
	else
	    free(msg);
    }
}

/*
 * bgp_nettable_add_req
 * Receive an add request from the main IP routing table.
 *
 * Pseudo-public routine called via pdb vector.
 */

BGP_PRIVATE void
bgp_nettable_add_req (pdbtype *pdb, ipaddrtype address, ipaddrtype mask,
		      backuptype *backup)
{
    bworktype *work;

    if (!(work = malloc(sizeof(bworktype)))) {
	/*
	 * Ok, if we can't do anything, then shove everything into the
	 * routing table and free up the work that we've queued.
	 */
	bgp_nettable_version = 1;
	while (!QUEUEEMPTY(&bgp_requestQ)) 
	    free(dequeue(&bgp_requestQ));
	return;
    }
    work->address = address;
    work->mask = mask;
    enqueue(&bgp_requestQ, work);
}

/*
 * bgp_service_add_req
 * Fulfill an add request by doing the route injection for this route.
 */

BGP_PRIVATE void
bgp_service_add_req (bworktype *work)
{
    bnettype *net;
    bpathtype *path, *mpath;
    ulong tag, netflags, metric;
    battrtype *attr;
    route_maptype *map;
    char *pathstr;
    idbtype *idb;
    ushort type;
    rdbtype *rdb;
    bnexthoptype nexthop[MAXROUTES];
    int i, pathcount;

    pathstr = NULL;
    if (bgpupdate_debug)
	buginf("\nBGP: add request for %i%m", work->address, work->mask);
    net = bgp_netlookup(work->address, work->mask);
    if (!net || (net->best == BGP_NO_BEST))
	goto done;

    path = &net->paths[net->best];
    attr = path->attr;

    /*
     * If we learned the entry from BGP and it is still
     * valid, update the IP routing table.  If the BGP
     * entry was learned via an internal link, then set
     * NET_NOREDIST so it doesn't leak into an IGP (we're
     * not the exit gateway in that case).  If we sourced
     * this route, never inject it back into the routing
     * table.
     */
    
    netflags = 0;
    if (path->flags & BFLAG_INTERNAL)
	netflags |= NET_NOREDIST;
    if (path->flags & BFLAG_SOURCED)
	goto done;

    /*
     * Be compatible with existing BGP implementation, tag == next hop as
     * or 0 if local
     */
    tag = 0;
    if (attr->aslength)
	tag = GETSHORT(&attr->as_path[2]);

    /*
     * Set IGP metric to inter-as metric as specified in RFC1403.
     * Note: the BGP4/OSPF interaction document asks us to pull rabbits
     * out of our hats by doing magic with local-pref, but doesn't say
     * how,  so screw them.  This is backwards compatible with old
     * cisco implementations.  If they want, they can change it with
     * the table-map.
     */
    metric = path->flags & BFLAG_METRIC ? attr->metric : 0;

    pathcount = 0;
    if (net->flags & BNET_MULTIPATH)
	for (i = net->firstpath; i != BGP_NULL_INDEX; i = mpath->next) {
	    mpath = &net->paths[i];
	    if ((mpath->flags & BFLAG_MULTIPATH) &&
		(pathcount < pdb_BGP->multipath)) {
		nexthop[pathcount].gateway = mpath->gateway;
		nexthop[pathcount].source  = mpath->neighbor;
		nexthop[pathcount].install = TRUE;
		pathcount++;
	    }
	}
    else {
	nexthop[pathcount].gateway = path->gateway;
	nexthop[pathcount].source  = path->neighbor;
	nexthop[pathcount].install = TRUE;
	pathcount++;
    }

    if (pdb_BGP->table_map) {
	pathstr = bgp_path2string(attr->as_path, attr->aslength,
				  bgp_router_path_buffer,
				  ASCII_PATHBUFSIZE);
	map = bgp_route_map_check(pdb_BGP->table_map, attr, net, net->network,
				  net->mask, pathstr, TRUE);
	if (map) {
	    if ((map->set_list.set_flags & ROUTE_MAP_METRIC_SET) &&
		!map->set_list.metric.igrp_metric)
		metric = map->set_list.metric.value.scalar;

	    if ((map->set_list.set_flags & ROUTE_MAP_NEXT_HOP_SET) &&
		map->set_list.next_hop &&
		(map->set_list.next_hop->address.type == ADDR_IP))
		for (i = 0; i < pathcount; ++i)
		    nexthop[i].gateway = 
			map->set_list.next_hop->address.ip_addr;

	    if (map->set_list.set_flags & ROUTE_MAP_AUTOTAG_SET) {
		tag = TAG_AUTOMATIC;
		if (map->set_list.set_flags & ROUTE_MAP_TAG_SET)
		    tag |= (map->set_list.tag << TAG_ARBSHIFT) & TAG_ARBPART;
		if (attr->aslength) {
		    tag |= GETSHORT(&attr->as_path[2]) & TAG_ASPART;
		    if (attr->aslength == 2+PATHSIZE) {
			tag |= TAG_PATHLEN_1;
			if (attr->origin != ORIGIN_INCOMPLETE)
			    tag |= TAG_COMPLETE;
		    } else
			tag |= TAG_PATHLEN_GE1;
		}
	    } else
		if (map->set_list.set_flags & ROUTE_MAP_TAG_SET)
		    tag = map->set_list.tag;
	}
    }

    if (path->flags & BFLAG_AGGREGATE) {
	idb = nullidb;
	type = RTYPE_BGP_LOC;
    } else {
	idb = NULL;
	if (net->flags & BNET_BACKDOOR)
	    type = RTYPE_BGP_LOC;
	else
	    type = (path->flags & (BFLAG_INTERNAL+BFLAG_CONFED)) ? 
		RTYPE_BGP_INT :RTYPE_BGP_EXT;
    }


    for (i = 0; i < pathcount; i++) {

	if (bgpupdate_acl_debug(net->network, net->mask))
	    buginf("\nBGP: service add request installing %i/%i -> %i",
		   net->network, net->mask, nexthop[i].gateway);

	/*
	 * If the update succeeds (returns non-null), then BGP now owns the
	 * route in the routing table.  If so, then we cannot be synchronized.
	 */
	rdb = network_update(pdb_BGP, net->network, net->mask,
			     nexthop[i].gateway, idb, metric, netflags,
			     nexthop[i].source, tag, NULL, type);
	if (rdb) {
	    bgp_rdb_update(rdb, net);
	    bgp_nosource(net);
	    bgp_unsync(net);
	}
    }
 done:
    free(work);
}

/*
 * bgp_nettable_query
 * Receive a request from the main IP routing table to give a complete
 * update.  Just tweak the bgp_nettable_version and the update process will
 * do the rest.
 *
 * Pseudo-public function called via pdb vector.
 */

BGP_PRIVATE void
bgp_nettable_query (pdbtype *pdb, idbtype *idb)
{
    if (!idb)
	bgp_nettable_version = 1;
}

/*
 * bgp_network_command
 * Called via pdb when a 'network' command is added or deleted.
 */
BGP_PUBLIC void
bgp_network_command (parseinfo *csb)
{
    bworktype *bptr;
    bnettype *net;
    ipaddrtype address, mask;
    pdbtype *pdb;
    uchar i, j;
    boolean found, backdoor;

    backdoor = found = FALSE;
    pdb = csb->protocol;

    if (csb->nvgen) {
	for (i = 0; i < pdb->netcount; i++) {
	    nv_write(TRUE,"%s %i", csb->nv_command, pdb->networks[i]);
	    nv_add(pdb->network_mask[i] != get_majormask(pdb->networks[i]),
		   " mask %i", pdb->network_mask[i]);
	    nv_add(pdb->network_weight[i], " weight %d",
		   pdb->network_weight[i]);
	    nv_add(pdb->network_map[i] != NULL, 
		   " route-map %s", pdb->network_map[i]);
	    nv_add((pdb->networkflags[i] & NETF_BACKDOOR)," backdoor");
	}
	return;
    }

    address = GETOBJ(paddr,1)->ip_addr;
    if (GETOBJ(paddr,2)->type == ADDR_IP)
	mask = GETOBJ(paddr,2)->ip_addr;
    else
	mask = get_majormask(address);

    for (i = 0; i < pdb->netcount; ++i) {
	if ((pdb->networks[i] == address) &&
	    (pdb->network_mask[i] == mask)) {
	    found = TRUE;
	    break;
	}
    }

    if (csb->sense) {
	if (!found) {
	    if (pdb->netcount >= MAXNETWORKS) {
		printf("\n%% To many network entries");
		return;
	    }
	    i = pdb->netcount;
	    pdb->netcount++;
	}

	pdb->networks[i] = address;
	pdb->network_mask[i] = mask;
	pdb->network_weight[i] = GETOBJ(int,1);
	pdb->networkflags[i] = 0;
	pdb->network_map[i] = NULL;
	if (GETOBJ(int,2)) {
	    pdb->networkflags[i] |= NETF_BACKDOOR;
	    backdoor = TRUE;
	}
	if (*GETOBJ(string, 1)) {
	    setstring(&pdb->network_map[i], GETOBJ(string, 1));
	    if (!pdb->network_map[i])
		return;
	}
    } else {
	if (!found)
	    return;

	pdb->netcount--;
	free(pdb->network_map[i]);
	pdb->network_map[i] = NULL;
	if (pdb->networkflags[i] & NETF_BACKDOOR) 
	    backdoor = TRUE;
	for (j = i; j < pdb->netcount; j++) {
	    pdb->networks[j] = pdb->networks[j+1];
	    pdb->networkflags[j] = pdb->networkflags[j+1];
	    pdb->network_mask[j] = pdb->network_mask[j+1];
	    pdb->network_info[j] = pdb->network_info[j+1];
	    pdb->network_weight[j] = pdb->network_weight[j+1];
	    pdb->network_map[j] = pdb->network_map[j+1];
	}
    }

    if (!backdoor)
	return;

    for (bptr = (bworktype *)bgp_backdoorQ.qhead; bptr; 
	 bptr = bptr->next)
	if ((bptr->address == address) && (bptr->mask == mask))
	    break;
    
    if ((csb->sense && bptr) || (!csb->sense && !bptr))
	return;

    net = bgp_netlookup(address, mask);
    if (csb->sense) {
	bptr = malloc(sizeof(bworktype));
	if (!bptr)
	    return;

	bptr->address = address;
	bptr->mask = mask;
	enqueue(&bgp_backdoorQ, bptr);
	if (net)
	    net->flags |= BNET_BACKDOOR;
	else 
	    return;
    } else {
	unqueue(&bgp_backdoorQ, bptr);
	free(bptr);
	if (net)
	    net->flags &= ~BNET_BACKDOOR;
	else
	    return;
    }

    ip_route_delete(pdb, address, mask, IPADDR_ZERO,
		    IPADDR_ZERO, NULL, 0);
    bgp_nettable_add_req(pdb, address, mask, NULL);
}

/*
 * bgp_redist_callback  (bgp_redist_walker)
 * Called when a route is added or removed from the routing table.  We use
 * this for IGP synchronization as well as redistribution.
 *
 * Pseudo-public function called via pdb vector.
 */

BGP_PRIVATE void
bgp_redist_callback (pdbtype *pdb, ndbtype *ndb, int event, int flag)
{
    bworktype *work;
    ipaddrtype mask;
    int i;

    if (!pdb->running)
	return;

    if (ndb->attrflags & NET_SUBNETTED)
	mask = get_majormask(ndb->netnumber);
    else
	mask = ndb->netsmask;

    /*
     * if auto-summarization is enabled, ignore subnets unless they
     * have been configured with a network command
     */
    if ((ndb->attrflags & NET_SUBNET) && pdb_BGP->auto_summary &&
	mask != get_majormask(ndb->netnumber)) {
	for (i = 0; i < pdb_BGP->netcount; i++) /* 4am ugly code */
	    if ((ndb->netnumber == pdb_BGP->networks[i]) &&
	        (mask == pdb_BGP->network_mask[i]))
		break;
	if (i == pdb_BGP->netcount)
	    return;
    }


    if (!(work = malloc(sizeof(bworktype)))) {
	process_sleep_for(ONESEC);
	if (!(work = malloc(sizeof(bworktype)))) {
	    return;
	}
    }
    work->event = event;
    work->address = ndb->netnumber;
    work->mask  = mask;

    enqueue(&bgp_redistQ, work);
}

/*
 * bgp_service_callback
 * Service a pending callback request.
 */

BGP_PRIVATE void
bgp_service_callback (bworktype *work)
{
    bnettype *net;
    int i, rdb_index;
    boolean localflag, redist;
    ushort weight;
    bpathtype *path;
    neighbortype *bgp;
    ndbtype *ndb;
    rdbtype *rdb;

    if (!bgp_iopid)			/* bail if we're not running */
	goto done;

    ndb = bgp_fwdtable_lookup(work->address, work->mask);

    redist = FALSE;
    localflag = FALSE;

    weight = BGP_SOURCED_WEIGHT;	/* assume no modification of weight */

    if (!ndb) 
	work->event = ROUTE_DOWN;
    else {
	
	/*
	 * First check to see if it needs to be redistributed.
	 */
	redist = bgp_redist_check(ndb);
	if (redist) {
	    weight = pdb_BGP->metric[ndb->pdbindex].weight;
	    if (ndb->pdbindex != pdb_BGP->index)
		ndb->advertisingmask |= pdb_BGP->mask;
	}
    }

    /*
     * Next check if it is local.
     */
    for (i = 0; i < pdb_BGP->netcount; i++) {
	if ((work->address == pdb_BGP->networks[i]) &&
	    (work->mask == pdb_BGP->network_mask[i])) {
	    pdb_BGP->route_map = NULL;	/* don't apply errant routemaps */
	    redist = FALSE;
	    if (!(pdb_BGP->networkflags[i] & NETF_BACKDOOR))
		if (!ndb ||	    	/* could be ROUTE_UP event */
		    bgp_network_check(ndb, pdb_BGP->network_map[i])) {
		    localflag = TRUE;
		    weight = pdb_BGP->network_weight[i];
		    redist = TRUE;
		}
	    break;
	}
    }

    net = bgp_netlookup(work->address, work->mask);
    switch (work->event) {
    case ROUTE_UP:
	if (redist) { 
	    if (bgpupdate_acl_debug(ndb->netnumber, work->mask))
		buginf("\nBGP: route up %i%m", work->address, work->mask);
	    bgp_edit(ndb, ndb, localflag, weight);
	    net = bgp_netlookup(work->address, work->mask);
	}
	break;

    case ROUTE_DOWN:
	if (!net)
	    break;
	if (bgpupdate_acl_debug(net->network, net->mask))
	    buginf("\nBGP: route down %i%m", net->network, net->mask);
	bgp_unsync(net);
	bgp_nosource(net);
	break;

    case ROUTE_MODIF:
	if (!net)
	    break;
	path = NULL;
 	for (i = net->firstpath; i != BGP_NULL_INDEX; i = net->paths[i].next) {
	    if (net->paths[i].flags & BFLAG_SOURCED) {
		path = (bpathtype *) &net->paths[i];
		break;
	    }
	}
	if (!path)
	    break;
	if (bgpupdate_acl_debug(net->network, net->mask))
	    buginf("\nBGP: route modified %i%m", net->network, net->mask);
	if ((path->flags & BFLAG_VALID) && !localflag)
	    path->flags &= ~BFLAG_LOCAL;
	bgp_unsync(net);
	if (redist || localflag) {
	    bgp_edit(ndb, ndb, localflag, weight);
	    net = bgp_netlookup(work->address, work->mask);
	} else {
	    bgp_nosource(net);
	}
	break;
    default:
	break; 
    }
    
    /*
     * Finally, check to see if we synchronize a path.
     */
    if (!net || (net->pathcount == 0))
	goto done;

    if (work->event == ROUTE_DOWN)
	goto done;

    if (pdb_BGP->sync_disabled)
	goto done;

    for (i = net->firstpath; i != BGP_NULL_INDEX; i = net->paths[i].next) {
	path = (bpathtype *) &net->paths[i];
	if (path->flags & BFLAG_RCVD_ONLY)
	    continue;
	if (!(path->flags & BFLAG_INTERNAL))
	    continue;
	if ((path->flags & BFLAG_SYNCED))
	    continue;
	if (NDB2PROCTYPE(ndb) & PROC_OSPF) {
	    bgp = bgp_path2neighbor(path);
	    if (!bgp || bgp->protocol_version < 3)
		continue;
	    for (rdb_index = 0; rdb_index < ndb->routecount; rdb_index++) {
		rdb = ndb->rdb[rdb_index];
		if (rdb && (bgp->bgp_router_id == rdb->infosource))
		    goto ok;
	    }
	    continue;
	}
    ok:
	path->flags |= BFLAG_SYNCED;
	bgp_bestpath(net, i, FALSE);
	if (bgpupdate_acl_debug(net->network, net->mask))
	    buginf("\nBGP: route synced %i%m", net->network, net->mask);
	goto done;
    }
 done:
    free(work);
}
/*
 * bgp_rtr_id
 * Calculate the BGP router id.
 */

ipaddrtype
bgp_rtr_id (void)
{
    pdbtype *pdb;

    /*
     * Has user overridden automatic router id selection?
     */
    if (bgp_router_id_cfg)
	return (bgp_router_id_cfg);

    /*
     * First, figure out if we are redistributing routes into an OSPF
     * router.  If so, we need to use that router ID so that other internal
     * routers can detect our routes in OSPF and forward the right paths.
     */
    for (pdb = (pdbtype *) protoQ.qhead; pdb; pdb = pdb->next)
	if ((pdb->proctype & PROC_OSPF) &&
	    (pdb->distflag & pdb_BGP->mask))
	    return ((ipaddrtype) pdb->ospf_rtr_id);

    /*
     * If we're not redistributing into OSPF, then just calculate the
     * router ID the "normal" way.
     */
    return (allocate_rtr_id(FALSE));
}

/*
 * bgp_reset_walker
 *
 * If the neighbor (bgp) is null, remove all routes and information
 * for a particular network and free up datastructures.  If non-null,
 * just clean up all info learned from the particular neighbor.
 */

BGP_PRIVATE int
bgp_reset_walker (rntype *rn, va_list ap)
{
    bnettype     *net = rn2bnettype(rn);
    neighbortype *bgp = va_arg(ap, neighbortype *);
    boolean clear_advertised_bit = va_arg(ap, boolean);

    int j, next;

    if (!bgp) {
	/*
	 * Deleting all network entries.  Flush all path entries,
	 * then delete the network entry.
	 * Clean dampinfo structure for each path. As we suspend 
	 * during reset, bgp_scanner might try to flush it.
	 */
 	for (j = net->firstpath; j != BGP_NULL_INDEX; j = next) {
 	    next = net->paths[j].next;
	    free(net->paths[j].attr);
	    net->paths[j].attr = NULL;
 	    net->paths[j].next = BGP_NULL_INDEX;
 	    net->paths[j].prev = BGP_NULL_INDEX;
	    if (net->paths[j].dampinfo)
		bgp_free_dampinfo(net, j);
	}

	if (!rn_delete(&net->network_key, &net->mask_key, bgp_head))
	    errmsg(&msgsym(DELROUTE, BGP), net->network, net->mask);

	free(net);
    } else {
	if (clear_advertised_bit)
	    bgp_clear_advertised_bit(net, bgp);

	/*
	 * Just one peer going away.
	 * Invalidate all its paths and recompute the best path.
	 */
	j = bgp_find_path(net, bgp->address);
	if (j != BGP_NULL_INDEX) {
	    next = net->paths[j].next;
	    bgp_delete_path(net, j);
	    if ((net->best == j) ||
		(net->paths[j].flags & BFLAG_MULTIPATH))
		bgp_bestpath(net, j, TRUE);

	    if ((next != BGP_NULL_INDEX) &&
		(net->paths[next].neighbor == bgp->address))
		bgp_delete_path(net, next);
	}
    }

    return WALK_CONTINUE;
}

/*
 * bgp_reset
 * If argument is NULL, reset all BGP data structures, otherwise just
 * invalidate all information for that particular neighbor.
 */

BGP_PRIVATE void
bgp_reset (neighbortype *bgp, const char *reason)
{
    bmsgtype *msg, *nmsg;
    battrtype *attr, *nattr;
    boolean auto_cluster_id, clear_advertised_bit;
    neighbortype *pbgp;
    int i;

    bgptable_version++;
    auto_cluster_id = FALSE;

    /*
     * If neighbor_index is present for non-peergroup member, clear 
     * the advertised bit. Usually it is present for external members
     * but it could also be present if an external neighbor is changed
     * to be an internal member. As the peergroup members use the same
     * index, do not clear peergroup member's advertised bits!
     */
    if (bgp && bgp->neighbor_index && !bgp->peergroup)
 	clear_advertised_bit = TRUE;
    else
 	clear_advertised_bit = FALSE;
 
    /*
     * Wipe out all paths, or all paths to a particular neighbor.
     */
    rn_walktree_timed(bgp_head, bgp_reset_walker, ip_radix_successor, bgp,
		      clear_advertised_bit);

    /*
     * Abort TCP connections
     * If reclaim flag is set, reclaim the index. Reset it either
     * to peergroup index or 0. Also set the default_state flag to down.
     */
    if (bgp) {
	if (bgpevent_debug && reason)
	    buginf("\nBGP: %i reset by %s", bgp->address, reason);

 	if (bgp->flags & BN_RECLAIM_INDEX) {
 	    BITMASK_CLEAR(bgp_neighbor_bitfield, bgp->neighbor_index);
 	    bgp->flags &= ~BN_RECLAIM_INDEX;
 	    if (bgp->peergroup) {
 		pbgp = bgp->peergroup;
 		bgp->neighbor_index        = pbgp->neighbor_index;
 		bgp->neighbor_index_offset = pbgp->neighbor_index_offset;
 		bgp->neighbor_index_mask   = pbgp->neighbor_index_mask;
 	    } else {
 		bgp->neighbor_index        = 0;
 		bgp->neighbor_index_offset = 0;
 		bgp->neighbor_index_mask   = 0;
 	    }
 	}
 
	bgp->bgp_default_state = BGP_DEFAULT_DOWN;
	bgp_close(bgp);
    } else {
	if (bgpevent_debug && reason)
	    buginf("\nBGP: reset all neighbors due to %s", reason);
	bgptable_version = bgp_nettable_version = bgp_aggregate_version = 1;
	for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next) {
	    bgp->bgp_default_state = BGP_DEFAULT_DOWN;
	    bgp_close(bgp);
	}
	table_prune_pdb(pdb_BGP);


	/*
	 * Clear the route-map and filter-list cache.
	 */
	bgp_reset_fcache();
	bgp_reset_rcache();
		    
	/*
	 * Flush the path attribute table.
	 */
	for (i = 0; i < NETHASHLEN; i++) {
	    attr = bgppathtable[i];
	    bgppathtable[i] = NULL;
	    while (attr) {
		nattr = attr->next;
		attr->next = NULL;
		bgp_free_parent_alist(attr);
		free(attr);
		attr = nattr;
	    }
	}

	/*
	 * Flush the update table.
	 */
	for (i = 0; i < NETHASHLEN; i++) {
	    msg = bgpupdatetable[i];
	    bgpupdatetable[i] = NULL;
	    while (msg) {
		if (bgpupdate_debug)
		    buginf("\nBGP: leftover update %#x", msg);
		nmsg = msg->next;
		free(msg);
		msg = nmsg;
	    }
	}

	/*
	 * Recalculate the router ID.
	 * If the cluster ID was configured automagically, 
	 * reset it after recalculating the router id.
	 */
	if (bgp_cluster_id &&
	    (bgp_cluster_id == pdb_BGP->bgp_rtr_id))
	    auto_cluster_id = TRUE;

	pdb_BGP->bgp_rtr_id = bgp_rtr_id();

	if (auto_cluster_id)
	    bgp_cluster_id = pdb_BGP->bgp_rtr_id;
    }
}

/*
 * bgp_service_reset
 * Service reset requests
 */

BGP_PRIVATE void
bgp_service_reset (bworktype *work)
{
    neighbortype *bgp;
    static const char reason[] = "user initiated reset request";

    if (work->address) {
	for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next) {
	    if (bgp->address == work->address) {
		if (bgpevent_debug)
		    buginf("\nBGP: %i reset requested", bgp->address);
		bgp_reset(bgp, reason);
		bgp->flags |= BN_RESTART;
		break;
	    }
	}
    } else {
	bgp_reset(NULL, reason);
	for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next) 
	    bgp->flags |= BN_RESTART;
    }
    free(work);
}

/*
 * bgp_free_neighbor
 * Before freeing the neighbor structure, free all the malloced
 * memory hanging from it.
 */

BGP_PRIVATE void
bgp_free_neighbor (neighbortype *bgp)
{
    route_map_deconfig(&bgp->bgp_routemapin);
    route_map_deconfig(&bgp->bgp_routemapout);
    route_map_deconfig(&bgp->bgp_unsuppress_map);
    route_map_deconfig(&bgp->bgp_default_map);
    if (bgp->bgp_password.pw) {
	free(bgp->bgp_password.pw);
	bgp->bgp_password.pw = NULL;	/* paranoia */
    }
    if (bgp->bgp_listener) {
	tcp_close(bgp->bgp_listener);
	bgp->bgp_listener = NULL;	/* more paranoia */
    }
    bgp->next = NULL;
    bgp->nextpeer = NULL;
    free(bgp);
}

/*
 * bgp_delete_neighbor
 * Delete a neighbor from the configuration
 */

BGP_PRIVATE void
bgp_delete_neighbor (neighbortype *bgp)
{
    neighbortype **pred, *p;
    
    if (!bgp)
	return;

    if (bgp_debug && bgp->tcb)
	buginf("\nBGP: %i delete neighbor", bgp->address);
    
    /*
     * If this non-peergroup neighbor has an index, mark it for index
     * reclaim. bgp_reset() will do it.
     */
    if (bgp->neighbor_index &&
 	!bgp->peergroup) {
 	bgp->flags |= BN_RECLAIM_INDEX;
    }

    bgp_reset(bgp, NULL);
    
    /*
     * If member of peer group, remove from the peer-group members list.
     */
    if (bgp->peergroup) {
	for (p = pdb_BGP->peergroup_list; p; p = p->next)
	    if (bgp->peergroup == p)
		break;

	pred = &(p->peerhead);
	while ((*pred)) {
	    if ((*pred) == bgp) {
		(*pred) = (*pred)->nextpeer;
		break;
	    }
	    pred = &((*pred)->nextpeer);
	}
    }

    pred = &(pdb_BGP->neighbors);
    while ((*pred)) {
	if ((*pred) == bgp) {
	    (*pred) = (*pred)->next;
	    bgp_free_neighbor(bgp);
	    break;
	}
	pred = &((*pred)->next);
    }

    /*
     * If we are configured to be a RR via route-reflector-client 
     * command, check if we are still RR.
     */
    if (bgp_cluster_id == pdb_BGP->bgp_rtr_id) {
	bgp_cluster_id = 0;
	for (p = pdb_BGP->neighbors; p; p = p->next)
	    if (p->flags & BN_RR_CLIENT) {
		bgp_cluster_id = pdb_BGP->bgp_rtr_id;
		break;
	    }
    }
}

/*
 * bgp_delete_group
 * Delete a template or a peergroup from the configuration
 */

BGP_PRIVATE void
bgp_delete_group (neighbortype *bgp)
{
    neighbortype **pred, *p;
    
    if (!bgp)
	return;

    /*
     * If we're deleting a template, we should also delete all
     * temporary neighbors created by this template.
     * If we're deleting a peer-group, we should also delete all
     * the peer-group members
     */
    for (p = pdb_BGP->neighbors; p; p = p->next) {
	if ((bgp->flags & BN_TEMPLATE) && (p->creator_template == bgp))
	    p->flags |= BN_DELETED;
	else if ((bgp->flags & BN_PEERGROUP) &&(p->peergroup == bgp)) {
	    p->nextpeer = NULL;
	    p->peergroup = NULL;
	    p->flags |= BN_DELETED;
	}
    }
    
    if (bgp->flags & BN_TEMPLATE)
	pred = &(pdb_BGP->promiscuous);
    else
	pred = &(pdb_BGP->peergroup_list);

    while ((*pred)) {
	if ((*pred) == bgp) {
	    (*pred) = (*pred)->next;
	    bgp_free_neighbor(bgp);
	    return;
	}
	pred = &((*pred)->next);
    }
}

/*
 * bgp_backdoor
 * Set the BNET_BACKDOOR flag on the backdoor routes.
 */
BGP_PRIVATE inline void
bgp_backdoor (void)
{
    bworktype *bptr;
    bnettype *net;

    for (bptr = (bworktype *)bgp_backdoorQ.qhead; bptr;
	 bptr = bptr->next) {
	net = bgp_netlookup(bptr->address, bptr->mask);
	if (net)
	    net->flags |= BNET_BACKDOOR;
    }
}

/*
 * bgp_free_aggregate
 * Free an bgp_aggregateQ entry.
 */
BGP_PRIVATE void
bgp_free_aggregate (addrsumtype *aggregate)
{
    route_map_deconfig(&aggregate->suppress_map);
    route_map_deconfig(&aggregate->advertise_map);
    route_map_deconfig(&aggregate->attribute_map);
    free(aggregate);
}

/*
 * bgp_suppress_children
 * Set/clear the BNET_SUPPRESSED bit on any routes more specific than the
 * existing aggregate.
 */

BGP_PRIVATE void
bgp_suppress_children (addrsumtype *aggregate, boolean suppress)
{
    bnettype *net;
    battrtype *attr, *last_attr;
    int old_flags;

    last_attr = NULL;

    /*
     * Crank through the table looking for networks that are
     * subsets of this aggregate.  If we find one, merge in any information
     * we've got about this network.
     * Do -not- suppress locally generated routes (created with either the
     * 'network' command or the 'aggregate-address command).
     */

    net = bgp_subtreelookup(aggregate->network, aggregate->mask);
    while (net &&
	   ((net->network & aggregate->mask) == aggregate->network)) {
	if ((net->best != BGP_NO_BEST) && (net->mask > aggregate->mask)) {
	    if (aggregate->suppress_map) {
		attr = net->paths[net->best].attr;
		if (attr != last_attr) {
		    if (!bgp_path2string(attr->as_path, attr->aslength,
					 bgp_router_path_buffer,
					 ASCII_PATHBUFSIZE))
			return;
		    last_attr = attr;
		}

		if (!bgp_route_map_check(aggregate->suppress_map, attr, net,
					 net->network, net->mask,
					 bgp_router_path_buffer, TRUE))
		    goto next;
	    }

	    old_flags = net->flags;

	    if (suppress && !(net->paths[net->best].flags & BFLAG_AGGREGATE))
		net->flags |= BNET_SUPPRESSED;
	    else
		net->flags &= ~BNET_SUPPRESSED;

	    if (old_flags != net->flags)
		bgp_bump_net_version(net);
	}
    next:
	if (((rntype *)net)->rn_dupedkey)
	    net = (bnettype *)(((rntype *)net)->rn_dupedkey);
	else
	    net = (bnettype *) bgp_nextnode(net->network);
    }
}

/*
 * bgp_calculate_aggregate
 * Calculate the origin, weight, and AS path.
 */

BGP_PRIVATE void
bgp_calculate_aggregate (addrsumtype *work, bnettype *aggnet, bpathtype *path,
			 battrtype *attr, ulong *community)
{
    bnettype      *net;
    battrtype     *ap, *last_ap, *last_used_ap;
    ushort         af;
    ulong	  *set_community;
    char          *pathstr;
    route_maptype *map;
    ushort	   egpfixup;
    uchar	  *p, set_comcount;

    egpfixup = 0;
    last_ap  = NULL;
    last_used_ap = NULL;

    attr->comcount = 0;
    attr->aslength = 0;
    attr->origin = ORIGIN_IGP;
    attr->aggregator.as = bgp_confed_id ? bgp_confed_id : pdb_BGP->asystem;
    attr->aggregator.ip = pdb_BGP->bgp_rtr_id;

    path->weight = BGP_SOURCED_WEIGHT;
    path->flags &= ~(BFLAG_LOCALPREF+BFLAG_METRIC+BFLAG_COMMUNITY+
		     BFLAG_DESTPREF);
    path->flags |= BFLAG_LOCAL+BFLAG_ORIGIN+BFLAG_ASPATH+
	BFLAG_NEXTHOP+BFLAG_AGGREGATOR+BFLAG_AGGREGATE;

    /*
     * OK, now crank through the table looking for networks that are
     * subsets of this aggregate.  If we find one, merge in any information
     * we've got about this network.
     */
    if (work->flags & ADDRSUM_AS_SET) {
	net = bgp_subtreelookup(aggnet->network, aggnet->mask);
	while (net &&
	       ((net->network & aggnet->mask) == aggnet->network)) {
	    if ((net->best != BGP_NO_BEST) && (net->mask > aggnet->mask)) {
		ap = net->paths[net->best].attr;
		if (work->advertise_map) {
		    if (ap != last_ap) {
			if (!bgp_path2string(ap->as_path, ap->aslength,
					     bgp_router_path_buffer,
					     ASCII_PATHBUFSIZE))
			    return;
			last_ap = ap;
		    }
		    
		    if (!bgp_route_map_check(work->advertise_map, ap, net,
					     net->network, net->mask,
					     bgp_router_path_buffer, TRUE)) {
 			path->flags |= BFLAG_ATOMICAGG;
			goto next;
		    }
		}

		af = net->paths[net->best].flags;
		path->flags |= af & (BFLAG_ATOMICAGG|BFLAG_LOCALPREF);
		if (af & BFLAG_LOCALPREF) 
		    attr->localpref = max(attr->localpref, ap->localpref);
		if (ap != last_used_ap) {
		    attr->origin = max(attr->origin, ap->origin);
		    attr->aslength = bgp_path_union(attr->as_path, 
						    attr->aslength,
						    ap->as_path, ap->aslength);
		    attr->comcount = bgp_com_union(ap->community, ap->comcount,
						   community, attr->comcount,
						   BGP_MAXCOMMUNITY);
		    last_used_ap = ap;
		}
	    }
	next:
	    if (((rntype *)net)->rn_dupedkey)
		net = (bnettype *)(((rntype *)net)->rn_dupedkey);
	    else
		net = (bnettype *) bgp_nextnode(net->network);
	}
	pathstr = bgp_path2string(attr->as_path, attr->aslength,
				  bgp_router_path_buffer,
				  ASCII_PATHBUFSIZE);
    } else {
	path->flags |= BFLAG_ATOMICAGG;
	pathstr = "";
    }

    /*
     * Allow manual override of aggregate values (not for the faint of heart)
     * Handle the old and new style aggregate command.
     */
    if (work->attribute_map) {
	map = bgp_route_map_check(work->attribute_map, attr, aggnet, 
				  aggnet->network, aggnet->mask, pathstr,TRUE);
	if (map) {
	    if (map->set_list.set_flags & ROUTE_MAP_ORIGIN_SET) {
		attr->origin = map->set_list.origin;
		if (attr->origin == ORIGIN_EGP) {
		    egpfixup = map->set_list.origin_as;
		}
	    }

	    if (map->set_list.set_flags & ROUTE_MAP_LOCALPREF_SET) {
		attr->localpref = map->set_list.local_pref;
		path->flags |= BFLAG_LOCALPREF;
	    }

	    if (map->set_list.set_flags & ROUTE_MAP_DESTPREF_SET) {
		attr->destpref = map->set_list.destpref;
		attr->destpref_as = pdb_BGP->asystem;
		path->flags |= BFLAG_DESTPREF;
	    }

	    if ((map->set_list.set_flags & ROUTE_MAP_METRIC_SET) &&
		!map->set_list.metric.igrp_metric) {
		attr->metric = map->set_list.metric.value.scalar;
		path->flags |= BFLAG_METRIC;
	    }

	    if (map->set_list.set_flags & ROUTE_MAP_WEIGHT_SET)
		path->weight = map->set_list.weight;

	    if ((map->set_list.set_flags & ROUTE_MAP_NEXT_HOP_SET) &&
		map->set_list.next_hop &&
		(map->set_list.next_hop->address.type == ADDR_IP))
		path->gateway = map->set_list.next_hop->address.ip_addr;

	    /*
	     * Set the community attribute.
	     */
	    if (map->set_list.community) {
		set_community = map->set_list.community->com_array;
		set_comcount = map->set_list.community->count;
		if (map->set_list.set_flags & ROUTE_MAP_COMMUNITY_ADD) {
		    attr->comcount = bgp_com_union(set_community, set_comcount,
						   community, attr->comcount,
						   BGP_MAXCOMMUNITY);
		} else {
		    attr->comcount = set_comcount;
		    bcopy(set_community, community, 
			  COMMUNITY_LEN(set_comcount));
		}
	    }	
	}
    }
    
    if (egpfixup && (egpfixup != pdb_BGP->asystem)) {
	p = attr->as_path + attr->aslength;
	attr->aslength += PATHSIZE+2;
	*p++ = AS_SEQUENCE;
	*p++ = 1;			/* one entry */
	PUTSHORT(p, egpfixup);
    }

    if (attr->comcount) {
	bgp_special_community(community, attr->comcount, path);
	path->flags |= BFLAG_COMMUNITY;
    }
}

/*
 * bgp_update_aggregate
 * Update the origin and AS path.
 * Return FALSE if out of memory.
 */

BGP_PRIVATE boolean
bgp_update_aggregate (addrsumtype *work, bnettype *net, int pathindex,
		      boolean *changed)
{
    bpathtype pathinfo, *path;
    battrtype *tmp;
    int comlength;

    *changed = FALSE;
    path = &net->paths[pathindex];
    comlength = COMMUNITY_LEN(path->attr->comcount);

    /*
     * Copy the information from the path->attr structure to preallocated 
     * global structure.
     * No need to copy the community attribute. Anyway they are going 
     * to be regenerated by calculate_aggregate.
     */
    bcopy(path, &pathinfo, sizeof(bpathtype));
    bcopy(path->attr, bgp_router_attr_buffer, 
	  sizeof(battrtype) + path->attr->aslength);
    bgp_calculate_aggregate(work, net, &pathinfo, bgp_router_attr_buffer, 
			    bgp_router_com_buffer);

    if (bcmp(&pathinfo, path, sizeof(bpathtype))) {
	*changed = TRUE;
	bcopy(&pathinfo, path, sizeof(bpathtype));
    }
    if (bcmp(bgp_router_attr_buffer, path->attr,
	     sizeof(battrtype) + bgp_router_attr_buffer->aslength) ||
	bcmp(bgp_router_com_buffer, path->attr->community, comlength)) {
	*changed = TRUE;
	tmp = bgp_find_attr(bgp_router_attr_buffer, 
			    bgp_router_attr_buffer->as_path,
			    bgp_router_com_buffer); 
	if (tmp) {
	    free(path->attr);
	    path->attr = tmp;
	    mem_lock(path->attr);
	} else
	    return(FALSE);
    }
    return (TRUE);
}

/*
 * bgp_setaggregate
 * Set the origin and AS path.
 */

BGP_PRIVATE void
bgp_setaggregate (addrsumtype *work, bnettype **rnet, int *i)
{
    bpathtype pathinfo;
    battrtype *attr;
    bnettype *net;

    net = *rnet;
    memset(&pathinfo, 0, sizeof(bpathtype));
    memset(bgp_router_attr_buffer, 0, sizeof(battrtype) + BGP_PATHBUFLEN);

    bgp_calculate_aggregate(work, net, &pathinfo, bgp_router_attr_buffer, 
			    bgp_router_com_buffer);
    attr = bgp_find_attr(bgp_router_attr_buffer, 
			 bgp_router_attr_buffer->as_path,
			 bgp_router_com_buffer);
    if (attr) 
	*rnet = bgp_add_path(net, &pathinfo, attr, i, BGP_NULL_INDEX,
			     BGP_NULL_INDEX);
    else
	*rnet = NULL;
}

/*
 * bgp_morespecific
 * Determine if a VALID more specific route exists
 */

BGP_PRIVATE boolean
bgp_morespecific (ipaddrtype network, ipaddrtype mask,
		  route_map_nametype *map)
{
    bnettype *net;
    battrtype *attr;

    net = bgp_subtreelookup(network, mask);
    while (net &&
	   ((net->network & mask) == network)) {
	if ((net->mask > mask) && (net->best != BGP_NO_BEST)) {
	    attr = net->paths[net->best].attr;
	    if (!map)
		return(TRUE);
	    else {
		if (!bgp_path2string(attr->as_path, attr->aslength,
				     bgp_router_path_buffer,
				     ASCII_PATHBUFSIZE))
		    return(FALSE);
		if (bgp_route_map_check(map, attr, net, net->network,net->mask,
					bgp_router_path_buffer, TRUE))
		    return (TRUE);
	    }
	}
	if (((rntype *)net)->rn_dupedkey)
	    net = (bnettype *)(((rntype *)net)->rn_dupedkey);
	else
	    net = (bnettype *) bgp_nextnode(net->network);
    }
    return (FALSE);
}

/*
 * bgp_aggregate
 * Run through the summary queue and create or delete aggregate entries
 * as appropriate.
 */

BGP_PRIVATE void
bgp_aggregate (queuetype *queue)
{
    bnettype *net, *snet;
    int i, entry, next;
    ndbtype *ndb;
    addrsumtype *work;
    boolean changed, reaggregate;

    for (entry = 0; (work = queuepick(queue, entry++)); ) {
	net = bgp_netlookup(work->network, work->mask);
	ndb = ip_rtlookup(work->network, work->mask);
	
	/*
	 * Check if the more specifics have changed lately. Recalculate the
	 * aggregation only if changed. Otherwise punt.
	 */
	reaggregate = FALSE;
	snet = bgp_subtreelookup(work->network, work->mask);
	while (snet && ((snet->network & work->mask) == work->network)) {
	    if ((snet->mask > work->mask) &&
		(snet->version > bgp_aggregate_version)) {
		reaggregate = TRUE;
		break;
	    }
	    if (((rntype *) snet)->rn_dupedkey)
		snet = (bnettype *) (((rntype *) snet)->rn_dupedkey);
	    else
		snet = (bnettype *) bgp_nextnode(snet->network);
	}
	if (!reaggregate)
	    continue;

	if (!bgp_morespecific(work->network, work->mask, work->advertise_map)||
	    !(work->flags & ADDRSUM_VALID)) {

	    /* see if we need to wipe this guy out */
	    if (net) {
 		for (i = net->firstpath; i != BGP_NULL_INDEX; i = next) {
 		    next = net->paths[i].next;
		    if (net->paths[i].flags & BFLAG_AGGREGATE) {
			bgp_delete_path(net, i);
			if (net->best == i)
			    bgp_bestpath(net, i, TRUE);
			if (bgpupdate_acl_debug(net->network, net->mask))
			    buginf("\nBGP: removed aggregate route for %i%m",
				   net->network, net->mask);
		    }
		}
	    }

	    /*
	     * OK, we've deleted the net from the BGP table, now we
	     * can delete the entry from the summary queue (finally)
	     */
	    if (!(work->flags & ADDRSUM_VALID)) {
		if (work->flags & ADDRSUM_AUTOFLTR || work->suppress_map)
		    bgp_suppress_children(work, FALSE);
		unqueue(&bgp_aggregateQ, work);
		bgp_free_aggregate(work);
	    }

	} else {		/* got some children */

	    if (net) {
 		for (i = net->firstpath; i != BGP_NULL_INDEX; 
 		     i = net->paths[i].next) {
		    if ((net->paths[i].flags & BFLAG_AGGREGATE)) {
			if (!bgp_update_aggregate(work, net, i, &changed))
			    return;
			if (changed) {
			    bgp_bestpath(net, i, FALSE);
			    if (bgpupdate_acl_debug(net->network, net->mask))
				buginf("\nBGP: aggregate route for %i%m "
				       "path #%d changed",
				       net->network, net->mask, i);
			}
			if (work->flags & ADDRSUM_AUTOFLTR ||
			    work->suppress_map)
			    bgp_suppress_children(work, TRUE);
			goto next_address;	/* all done */
		    }
		}
	    } else { 		/* need to create a new network */
		net = bgp_create_net(work->network, work->mask);
		if (!net) {
		    errmsg(&msgsym(NOMEMORY, BGP), "path");
		    bgp_reset(NULL, NULL);
		    return;
		}
	    }

	    /* create aggregate labeled path */
	    bgp_setaggregate(work, &net, &i);
	    if (!net)
		return;
	    net->paths[i].flags |= BFLAG_VALID;

	    bgp_bestpath(net, i, FALSE);
	    if (bgpupdate_acl_debug(net->network, net->mask))
		buginf("\nBGP: created aggregate route for %i%m",
		       net->network, net->mask);
	    if (work->flags & ADDRSUM_AUTOFLTR || work->suppress_map)
		bgp_suppress_children(work, TRUE);
	}
    next_address:
	process_may_suspend();
    }
}

/*
 * bgp_unsuppress
 */
BGP_PRIVATE void
bgp_unsuppress (void)
{
    bnettype *net;
    bpathtype *path;
    bdampinfotype *dampinfo, *next;

    if (bgp_reuse_array[bgp_reuse_index]) {
	dampinfo = bgp_reuse_array[bgp_reuse_index];
	bgp_reuse_array[bgp_reuse_index] = NULL;

	while (dampinfo) {
	    net = dampinfo->net;
	    path = &net->paths[dampinfo->pindex];
	    path->flags &= ~BFLAG_SUPPRESSED;
	    bgp_bestpath(net, dampinfo->pindex, FALSE);
	    
	    if (bgpdamp_acl_debug(net->network, net->mask))
		buginf("\nBGP: Unsuppressed %i%m, path %s", net->network, 
		       net->mask, bgp_path2string(path->attr->as_path,
						  path->attr->aslength,
						  bgp_router_path_buffer,
						  ASCII_PATHBUFSIZE));

	    next = dampinfo->next;
	    dampinfo->prev = NULL;
	    dampinfo->next = NULL;
	    dampinfo = next;
	}
    }

    bgp_reuse_index = bgp_get_reuseindex(1);	/* get next index */
}			

/*
 * bgp_default_originate
 * Originate/withdraw per neighbor defaults
 */
BGP_PRIVATE inline void
bgp_default_originate (neighbortype *bgp)
{
    boolean send_default, sendmetric;
    route_maptype *map;
    bpathtype dpath;
    battrtype dattr;
    ipaddrtype gateway;
    ulong metric;

    gateway = 0;
    map = NULL;
    metric = 0;
    sendmetric = FALSE;

    if (bgp->bgp_state != BGPS_ESTAB)
	return;

    /*
     * If it the peer is not configured to originate default or if
     * state is not withdraw (could happen on deconfig), return.
     */
    if (!(bgp->flags & BN_DEFAULT_ORIGINATE) &&
	(bgp->bgp_default_state != BGP_DEFAULT_WITHDRAW))
	return;

    send_default = FALSE;
    if (bgp->flags & BN_DEFAULT_ORIGINATE) {
	if (bgp->bgp_default_map) {
	    map = route_map_default_check(bgp->bgp_default_map->mapname);
	    if (map)
		send_default = TRUE;
	} else
	    send_default = TRUE;
    }

    if ((send_default && 
	 (bgp->bgp_default_state == BGP_DEFAULT_SENT)) ||
	(!send_default && 
	 (bgp->bgp_default_state == BGP_DEFAULT_DOWN)))
	return;
	
    if (send_default) {
	gateway = bgp->tcb->localhost.ip_addr;

	memset(&dattr, 0, sizeof(battrtype));
	dattr.origin = ORIGIN_IGP;

	memset(&dpath, 0, sizeof(bpathtype));
	dpath.flags |= (BFLAG_LOCAL+BFLAG_VALID+BFLAG_ASPATH+BFLAG_SOURCED);

	if (map) {
	    if ((map->set_list.set_flags & ROUTE_MAP_METRIC_SET) &&
		!map->set_list.metric.igrp_metric) {
		sendmetric = bgp_get_routemap_setmetric(map, &metric);
	    }
	}

	if (bgp4_format_update(bgp, 0, 0, &dattr, &dpath, gateway, map, FALSE,
			       metric, sendmetric))
	    bgp->bgp_default_state = BGP_DEFAULT_SENT;
    } else
	if (bgp4_format_unreachable(bgp, 0, 0))
	    bgp->bgp_default_state = BGP_DEFAULT_DOWN;
    bgp_enqueue_updates(bgp);
}

/*
 * bgprouterBLOCK
 * Block until there's work to do or until our timer expires.
 */

BGP_PRIVATE boolean
bgprouterBLOCK (pdbtype *pdb)
{
    if (bgp_read_queues_nonempty)
	return (FALSE);
    if (!TIMER_RUNNING_AND_SLEEPING(pdb->timer)) {
	TIMER_STOP(pdb->timer);
	return (FALSE);
    }
    if (!QUEUEEMPTY(&bgp_redistQ))
	return (FALSE);
    if (!QUEUEEMPTY(&bgp_requestQ))
	return (FALSE);
    if (!QUEUEEMPTY(&bgp_resetQ))
	return (FALSE);
    return (TRUE);
}

/*
 * bgp_upd_candidate
 * Select a neighbor from the group for update. Do not consider
 * neighbors with write queue size exceeding the limit. 
 */

BGP_PRIVATE inline neighbortype *
bgp_upd_candidate (neighbortype *pgrp, ulong upd_version)
{
    neighbortype *bgp, *candidate, *nmin;

    /*
     * Find the neighbor that has the least version number without
     * exceeding the write queue limit.
     */
    candidate = NULL;
    for(bgp = pgrp->peerhead; bgp; bgp = bgp->nextpeer) {
	if ((bgp->bgp_state == BGPS_ESTAB) &&
	    (QUEUESIZE(&bgp->bgp_writeQ) < BGP_WRITELIMIT)) {
	    if (!candidate || (candidate->version > bgp->version))
		candidate = bgp;
	}
    }

    if (!candidate)
	return(NULL);

    /*
     * If the candidate is in the middle of an update, go ahead
     * and complete it.
     */
    if (candidate->bgp_upd_version)
	return(candidate);


    /* 
     * We have a candidate ready for catchup. Lets find a neighbor the 
     * candidate should catch up!
     * Note : Don't bother about the queue size of next neighbor!
     */
    nmin = NULL;
    for(bgp = pgrp->peerhead; bgp; bgp = bgp->nextpeer) {
	if ((bgp->bgp_state == BGPS_ESTAB) &&
	    (bgp != candidate) &&
	    (bgp->version > candidate->version)) {
	    if (!nmin || (nmin->version > bgp->version))
		nmin = bgp;
	}
    }

    /*
     * If there is a next minimum neighbor, do catch up. The 
     * upd_version field is used to indicate the target version.
     *
     * If there is not next minimum neighbor, all the members of
     * the group are in sync. Check if they are upto date.
     */
    if (nmin) {
	candidate->bgp_upd_version = nmin->version;
	candidate->bgp_upd_checkpoint = 0;
	return(candidate);
    } else if (candidate->version != upd_version) {	
	    candidate->bgp_upd_version = upd_version;
	    candidate->bgp_upd_checkpoint = 0;
	    return(candidate);
    }
    return(NULL);
}

BGP_PRIVATE int
bgp_config_multipath_walker (rntype *rn, va_list ap)
{
    bnettype *net	= rn2bnettype(rn);

    boolean do_bestpath;
    int i;

    do_bestpath = FALSE;

    if (bgp_multipath_enabled)
	bgp_bestpath(net, BGP_NULL_INDEX, FALSE);
    else {
	for (i = net->firstpath; i != BGP_NULL_INDEX; i = net->paths[i].next)
	    if (net->paths[i].flags & BFLAG_MULTIPATH) {
		net->paths[i].flags &= ~BFLAG_MULTIPATH;
		do_bestpath = TRUE;
	    }
	
	if (do_bestpath)
	    bgp_bestpath(net, BGP_NULL_INDEX, TRUE);
    }

    process_may_suspend();
    return WALK_CONTINUE;
}

BGP_PRIVATE void
bgp_config_multipath(void)
{
    if ((pdb_BGP->multipath != BGP_DEFAULT_MULTIPATH) &&
	!bgp_multipath_enabled)
	bgp_multipath_enabled = TRUE;
    else
	bgp_multipath_enabled = FALSE;

    rn_walktree_blocking(bgp_head->rnh_treetop, bgp_config_multipath_walker);
}

/*
 * bgp_accept_path
 */

BGP_PRIVATE inline boolean
bgp_accept_path (bnettype *net, bpathtype *rcvd_path, bpathtype *pathinfo,
		 battrtype *attrinfo, uchar *aspath, int rcvd_idx, 
		 int used_idx, boolean changed)
{
    battrtype *attr;
    uchar next_path;

    if (bgpupdate_acl_debug(net->network, net->mask)) {
	buginf("\nBGP: Update accepted.");
    }

    /*
     * We could be here with the receive update changed or unchanged.
     * For each we could come with rcvd-used path, rcvd-only path or
     * seperate used path.
     */
    if (!changed) {
	if (rcvd_path->flags & BFLAG_RCVD_ONLY) {

	    /* We accept and there is no change so we change flag of the
	     * rcvd_path to RCVD_USED and then delete the modified one (if
	     * there is one, pointed to by used_idx).
	     */
	    rcvd_path->flags &= ~BFLAG_CLEAR_RCVD;
	    rcvd_path->flags |= (BFLAG_RCVD_USED+BFLAG_VALID);

	    if (used_idx != BGP_NULL_INDEX)
		bgp_delete_path(net, used_idx);
	    used_idx = rcvd_idx;

	    /* Relocate this path to the right place because a RCVD_ONLY
	     * path was not considered in sorting so it may be in the wrong
	     * place.
	     */
	    if (bgp_determ_med)
		bgp_relocate_path(net, used_idx);
	}
	attr = net->paths[used_idx].attr;

    } else {
	attr = bgp_find_set_attr(attrinfo, aspath, rcvd_path->attr);

	if ((used_idx == BGP_NULL_INDEX) ||
	    (rcvd_path->flags & BFLAG_RCVD_USED)) {

	    /* We don't have a used path (i.e. have a RCVD_ONLY path) or
	     * the path is a RCV_USED but now policy changes, so a new
	     * (modified) path has to be added (with no RCVD_* flag set).
	     * The current path becomes RCVD_ONLY.
	     */
	    rcvd_path->flags &= ~BFLAG_CLEAR_RCVD;
	    rcvd_path->flags |= BFLAG_RCVD_ONLY;

	    next_path = (bgp_determ_med) ? BGP_NULL_INDEX : rcvd_idx;
	    net = bgp_add_path(net, pathinfo, attr, &used_idx, 
			       BGP_NULL_INDEX, next_path);
	    if (!net)
		return(FALSE);

	    if (bgp_determ_med) {
		/*
		 * Move rcvd_idx path to be behind used_idx
		 */
		bgp_delink_path(net, rcvd_idx);
		bgp_link_path(net, rcvd_idx, used_idx);
	    }

	} else
	    /* used_index is set but it is not a RCVD_USED, i.e. we have
	     * a modified path and it changes so we need to update it.
	     */
	    bgp_replace_path(net, &net->paths[used_idx], pathinfo,
			     attr, used_idx);

	if (rcvd_path->dampinfo) 
	    bgp_free_dampinfo(net, rcvd_idx);

    }

    /* We set BFLAG_VALID here because a path that was initially denied was
     * considered invalid.  Now it is accepted so we set it.  This is an
     * interim solution before we separate the many different usages of this
     * flag to its own flag.
     */
    net->paths[used_idx].flags |= BFLAG_VALID;
    bgp_special_community(attr->community, attr->comcount, 
			  &net->paths[used_idx]);
    bgp_bestpath(net, used_idx, FALSE);
    return(TRUE);
}

/*
 * bgp_deny_path
 */

BGP_PRIVATE inline void
bgp_deny_path (bnettype *net, bpathtype *rcvd_path, int rcvd_idx, 
	       int used_idx)
{
    if (bgpupdate_acl_debug(net->network, net->mask)) {
	buginf("\nBGP: update denied");
	if (used_idx != BGP_NULL_INDEX)
	    buginf(", previous used path deleted");
    }

    /*
     * If the received path is seperate, check if there is 
     * used path. If there is used path, delete it.
     */
    if (rcvd_path->flags & BFLAG_RCVD_ONLY)
	if (used_idx == BGP_NULL_INDEX)
	    return;
	else
	    bgp_delete_path(net, used_idx);
 
    /*
     * If the received path is also the used path, set the appropriate
     * flag and blow away the dampinfo. 
     */
    if (rcvd_path->flags & BFLAG_RCVD_USED) {
	rcvd_path->flags &= ~BFLAG_CLEAR_RCVD;
	rcvd_path->flags |= BFLAG_RCVD_ONLY;
	if (rcvd_path->dampinfo)
	    bgp_free_dampinfo(net, rcvd_idx);
    }

    if ((used_idx != BGP_NULL_INDEX) &&
	(net->best == used_idx))
	bgp_bestpath(net, used_idx, FALSE);

    return;
}

/*
 * bgp_reconfig_path
 *
 * Apply new policy on the received path
 */

BGP_PRIVATE inline boolean
bgp_reconfig_path (bnettype *net, neighbortype *bgp, bpathtype *path,
		   int rcvd_idx, int used_idx)
{
    bpathtype pathinfo;
    battrtype *rcvd_attr, *used_attr, attrinfo;
    route_maptype *map;
    uchar *pathstr, *aspathptr;
    boolean accept, debug;
    ulong changed;

    accept = TRUE;
    changed = FALSE;
    rcvd_attr = path->attr;		/* initially rcvd and used are same */
    used_attr = path->attr;
    aspathptr = rcvd_attr->as_path;
    debug = bgpupdate_acl_debug(net->network, net->mask);

    /*
     * Apply inbound distribution list.
     */
    if (bgp->bgp_distlistin)
	if (!route_check(bgp->bgp_distlistin, net->network, net->mask)) {
	    accept = FALSE;
	    if (debug)
		buginf("\nBGP: Prefix rejected by inbound distribute-list.");
	    goto update;
	}


    /*
     * Store the received path in pathinfo for path policy processing. 
     */
    bcopy(path, &pathinfo, sizeof(bpathtype));
    pathinfo.flags &= ~(BFLAG_RCVD_ONLY+BFLAG_RCVD_USED+BFLAG_SUPPRESSED);
    pathinfo.dampinfo = NULL;
    pathinfo.attr = NULL;


    /*
     * If there is no inbound path policy bail
     */
    if (!(bgp->flags & BN_INBOUND_PATHPOLICY))
	goto update;


    pathstr = bgp_path2string(rcvd_attr->as_path, rcvd_attr->aslength,
			      bgp_router_path_buffer, ASCII_PATHBUFSIZE);
    if (!pathstr)
	return(FALSE);


    /*
     * Do non-routemap and routemap path policy.
     * The resultant attribute and path are stored in used_attr
     * and pathinfo structures.
     */
    if (!bgp_nonmap_path_policy(bgp, pathstr, rcvd_attr, &pathinfo.weight)) { 
	accept = FALSE;
	if (debug)
	    buginf("\nBGP: Prefix rejected by inbound filter-list.");
	goto update;
    }


    if (bgp->bgp_routemapin) {
	map = bgp_route_map_check(bgp->bgp_routemapin, rcvd_attr, net,
				  net->network, net->mask, pathstr, TRUE);
	if (!map) {
	    accept = FALSE;
	    if (debug)
		buginf("\nBGP: Prefix rejected by inbound route-map.");
	    goto update;
	}

	/*
	 * Use attrinfo as used attribute as rcvd_set function might
	 * modify the attribute. 
	 */
	used_attr = &attrinfo;
	bcopy(rcvd_attr, used_attr, sizeof(battrtype));
	used_attr->community = bgp_router_com_buffer;
	bcopy(rcvd_attr->community, used_attr->community, 
	      COMMUNITY_LEN(rcvd_attr->comcount));

	if (!bgp_route_map_rcv_set(bgp, map, used_attr, &pathinfo, 
				   &aspathptr, &changed))
	    return(FALSE);
    } 


 update:
    if (accept) {
	if (!bgp_accept_path(net, path, &pathinfo, used_attr, aspathptr, 
			     rcvd_idx, used_idx, changed))
	    return WALK_ABORT; 
    } else 
	bgp_deny_path(net, path, rcvd_idx, used_idx);

    return(TRUE);
}

/*
 * bgp_soft_reconfig_walker
 *
 * For each network from this neighbor, re-evaluate the received path
 * and force the new policy to take effect.
 */

BGP_PRIVATE int
bgp_soft_reconfig_walker (rntype *rn, va_list ap)
{
    bnettype *net	= rn2bnettype(rn);
    neighbortype *bgp	= va_arg(ap, neighbortype *);

    bpathtype *path;
    int i, rcvd_idx, used_idx;

    rcvd_idx = used_idx = BGP_NULL_INDEX;
    path = NULL;
 
    /*
     * Find path from this neighbor and set rcvd/used idx.
     */
    for (i = net->firstpath; i != BGP_NULL_INDEX; i = path->next) {
	path = &net->paths[i];

	if (path->neighbor != bgp->address)
	    continue;
	
	if (path->flags & BFLAG_RCVD_ONLY) { /* it is not used */
	    rcvd_idx = i;
	    used_idx = BGP_NULL_INDEX;

	} else if (path->flags & BFLAG_RCVD_USED) { /* it is used with no
						       change */
	    rcvd_idx = used_idx = i;

	} else {
	    /* no flag, look for a RCV_ONLY path that follows.
	     */
	    if ((path->next != BGP_NULL_INDEX) &&
		(net->paths[path->next].neighbor == bgp->address) &&
		(net->paths[path->next].flags & BFLAG_RCVD_ONLY)) {
		used_idx = i;
		rcvd_idx = path->next;
		path = &net->paths[rcvd_idx];

	    } else {
		if (bgpupdate_acl_debug(net->network, net->mask))
		    buginf("\nBGP: no rcvd path for %i%m from %i",
			   net->network, net->mask, path->neighbor);
	    }
	}

	break;
    }


    /*
     * If received path is present, reconfigure it.
     */
    if (rcvd_idx != BGP_NULL_INDEX) {

	if (bgpupdate_acl_debug(net->network, net->mask))
	    buginf("\nBGP: process %i%m, next hop %i, metric %d from %i", 
		   net->network, net->mask, path->gateway, path->attr->metric,
		   bgp->address);
	
	if (!bgp_reconfig_path(net, bgp, path, rcvd_idx, used_idx))
	    return WALK_ABORT;
    }

    process_may_suspend();
    return WALK_CONTINUE;
}

/*
 * bgp_soft_reconfig
 */

BGP_PRIVATE void
bgp_soft_reconfig (void)
{
    neighbortype *bgp;
    sys_timestamp max_reconfig_time;
    ulong starting_runtime;

    GET_TIMESTAMP(max_reconfig_time);

    for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next) {
	if (bgp->bgp_state != BGPS_ESTAB) {
	    bgp->flags &= ~(BN_SOFT_RECONFIG_DOOUT+BN_SOFT_RECONFIG_DOIN+
			    BN_SOFT_RECONFIG_OUT);
	    continue;
       }


	/*
	 * First outbound soft reconfig.  Clear the version field and
	 * set the appropriate flag and the magic will happen! 
	 * For external neighbors, clear the neighbor bit from all
	 * prefixes.
	 */
	if (bgp->flags & BN_SOFT_RECONFIG_DOOUT) {
	    bgp->version = 0;
	    bgp->bgp_soft_reconfig_version = bgptable_version;
	    bgp->bgp_upd_version = 0;
	    bgp->bgp_upd_checkpoint = 0;
	    bgp->flags |= BN_SOFT_RECONFIG_OUT;
	    bgp->flags &= ~BN_SOFT_RECONFIG_DOOUT;

	    if (!bgp->bgp_internal && !bgp->peergroup)
		rn_walktree_blocking(bgp_head->rnh_treetop, 
				     bgp_reset_bit_walker, bgp);

	    if (bgpupdate_debug)
		buginf("\nBGP: start outbound soft reconfiguration for %i", 
		       bgp->address);
	}


	/*
	 * Inbound reconfiguration.
	 */
	if (bgp->flags & BN_SOFT_RECONFIG_DOIN) {
	    if (bgp->flags & BN_SOFT_RECONFIG_INOK) {
		if (bgpupdate_debug)
		    buginf("\nBGP: start inbound soft reconfiguration for %i",
			   bgp->address);

		starting_runtime = runtime();
		rn_walktree_blocking(bgp_head->rnh_treetop, 
				     bgp_soft_reconfig_walker, bgp);

		if (bgpupdate_debug)
		    buginf("\nBGP: complete inbound soft reconfiguration"
			   ", ran for %lums", runtime() - starting_runtime);
	    }

	    bgp->flags &= ~BN_SOFT_RECONFIG_DOIN;

	    if (CLOCK_OUTSIDE_INTERVAL(max_reconfig_time, BGP_MAXRECONFIGTIME))
		return;
	    process_suspend();
	}


	/*
	 * Check if outbound soft reconfiguration is finished
	 */
	if (bgp->version >= bgp->bgp_soft_reconfig_version)
	    bgp->flags &= ~BN_SOFT_RECONFIG_OUT;
    }


    /*
     * Check if recofiguration request is pending or in progress. 
     * If it is not, clear the reconfig_check flag. 
     */
    for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next)
	if (bgp->flags & (BN_SOFT_RECONFIG_DOIN+BN_SOFT_RECONFIG_DOOUT+
			  BN_SOFT_RECONFIG_OUT))
	    break;

    if (!bgp)
	bgp_check_soft_reconfig = FALSE;
}

/*
 * bgp_enlarge_net_walker
 * Walk the BGP table and enlarge each net to handle larger advertized
 * bitfield.
 */

BGP_PRIVATE  int
bgp_enlarge_net_walker (rntype *rn, va_list ap)
{
    bnettype *net	= rn2bnettype(rn);
    int oldbitfield_size= va_arg(ap, int);
    
    if (!bgp_enlarge_net(net, oldbitfield_size))
	return(WALK_ABORT);

    process_may_suspend();
    return(WALK_CONTINUE);
}

/*
 * bgp_enlarge_neighbor_bitfield
 */

BGP_PRIVATE void
bgp_enlarge_neighbor_bitfield (void)
{
    uchar *new_bitfield;
    neighbortype *bgp, *pbgp;
    int count, old_bitfield_size, new_bitfield_count, new_bitfield_size;

    /*
     * Count all external peergroups requiring index
     */
    count = 0;
    for (pbgp = pdb_BGP->peergroup_list; pbgp; pbgp = pbgp->next) {
	if (pbgp->peerhead &&
	    !pbgp->peerhead->bgp_internal &&
	    (pbgp->flags & BN_ALLOCATE_INDEX))
	    count++;
    }

    /*
     * Count all non-peergroup external neighbors requiring index
     */
    for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next) {
	if (!bgp->bgp_internal &&
	    !bgp->peergroup &&
	    (bgp->flags & BN_ALLOCATE_INDEX))
	    count++;
    }

    new_bitfield_count = bgp_bitfield_count + count;
	
    /*
     * resize the neighbor bitfield
     */
    old_bitfield_size = bgp_bitfield_size;	/* need for net enlarge! */
    new_bitfield_size = BITMASK_BYTES_IN_ARRAY(new_bitfield_count);
    new_bitfield = malloc(new_bitfield_size);
    if (!new_bitfield)
	return;

    bcopy(bgp_neighbor_bitfield, new_bitfield, bgp_bitfield_size);
    free(bgp_neighbor_bitfield);
    bgp_neighbor_bitfield = new_bitfield;
    bgp_bitfield_size = new_bitfield_size;
    bgp_bitfield_count = new_bitfield_size << 3;

    /*
     * Walk the BGP table and enlarge all the entries.
     */
    rn_walktree_blocking(bgp_head->rnh_treetop, bgp_enlarge_net_walker,
			 old_bitfield_size);
}

/*
 * bgp_set_neighbor_index
 * Set a neighbor index for a neighbor.
 */

BGP_PRIVATE boolean
bgp_set_neighbor_index (neighbortype *bgp)
{
    ulong index;

    /*
     * Reserve bit 0. Now we could identify a neighbor with index by
     * doing non-zero test! 
     */
    BITMASK_SET(bgp_neighbor_bitfield, 0);

    index = 0;
    if (BITMASK_FIND_FIRST_CLEAR(bgp_neighbor_bitfield, 
				 bgp_bitfield_count, &index)) {
	BITMASK_SET(bgp_neighbor_bitfield, index);
	bgp->neighbor_index 	   = index;
	bgp->neighbor_index_offset = BITMASK_OFFSET_BYTE(index);
	bgp->neighbor_index_mask   = BITMASK_ONE_BIT_MASK(index);
	return(TRUE);
    } else
	return(FALSE);
}

/*
 * bgp_neighbor_indexing
 * Allocate neighbor index for EBGP and EBGP peergroups. If we need to
 * enlarge bitfield, enlarge and return. This is done to avoid race
 * condition with exec code adding/deleting neighbors.
 */

BGP_PRIVATE void 
bgp_neighbor_indexing (void)
{
    neighbortype *bgp, *pbgp;

    /*
     * First allocate index for external peergroups
     */
    for (pbgp = pdb_BGP->peergroup_list; pbgp; pbgp = pbgp->next) {
	if (pbgp->peerhead &&
	    !pbgp->peerhead->bgp_internal &&
	    (pbgp->flags & BN_ALLOCATE_INDEX)) {
	    /*
	     * Get an index. If failed, enlarge the bitfield
	     * and continue later.
	     */
	    if (!bgp_set_neighbor_index(pbgp)) {
		bgp_enlarge_neighbor_bitfield();
		return;
	    }

	    /*
	     * Update all members except members scheduled for
	     * reclaiming the index.
	     */
	    pbgp->flags &= ~BN_ALLOCATE_INDEX;
	    for (bgp = pbgp->peerhead; bgp; bgp = bgp->nextpeer)
		if (!(bgp->flags & BN_RECLAIM_INDEX)) {
		    bgp->neighbor_index       = pbgp->neighbor_index;
		    bgp->neighbor_index_offset = pbgp->neighbor_index_offset;
		    bgp->neighbor_index_mask   = pbgp->neighbor_index_mask;
		    bgp->flags &= ~BN_ALLOCATE_INDEX;
		}
	}
    }

    /*
     * Allocate index for external neighbors
     */
    for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next) {
	if (!bgp->bgp_internal &&
	    (bgp->flags & BN_ALLOCATE_INDEX)) {
	    /*
	     * If peergroup member, get neighbor index from peergroup.
	     * For normal EBGP neighbors, allocate index. On failure
	     * enlarge the bitfield and continue later.
	     */
	    if (bgp->peergroup) {
		pbgp = bgp->peergroup;
		bgp->neighbor_index        = pbgp->neighbor_index;
		bgp->neighbor_index_offset = pbgp->neighbor_index_offset;
		bgp->neighbor_index_mask   = pbgp->neighbor_index_mask;
	    } else {
		if (!bgp_set_neighbor_index(bgp)) {
		    bgp_enlarge_neighbor_bitfield();
		    return;
		}
	    }
	    bgp->flags &= ~BN_ALLOCATE_INDEX;
	}
    }

    TIMER_STOP(bgp_neighbor_index_timer);
}


/*
 * bgp_router
 * Do periodic or time driven BGP activities.
 *
 * Pseudo-public routine called via PDB vector
 */

BGP_PRIVATE void
bgp_router (pdbtype *pdb)
{
    neighbortype *bgp, *bgp_next, *pgrp;
    boolean readmsgs, suspend, blocked, perform_updates;
    ipaddrtype last_peer_checked;
    sys_timestamp max_update_time;
    ulong upd_version;

    process_wait_on_system_init();

    bgp_iopid = cfork((forkproc *) bgp_io, (long)pdb, 0, "BGP I/O", 0L);
    bgp_scanpid = cfork(bgp_scanner, (long)pdb, 0, "BGP Scanner", 0L);
    suspend = FALSE;
    blocked = FALSE;
    last_peer_checked = 0;

    /* freeze exterior routing on crash */
    process_set_crashblock(THIS_PROCESS,TRUE);

    while (TRUE) {
	if (suspend) {
	    TIMER_START(pdb->timer, BGP_DISPATCH);
	    suspend = FALSE;
	} else 
	    TIMER_START(pdb->timer, ONESEC);

	bgp_read_queues_nonempty = FALSE;
	for (bgp = pdb->neighbors; bgp; bgp = bgp->next)
	    bgp_read_queues_nonempty |= QUEUESIZE(&bgp->bgp_readQ);

	edisms((blockproc *)bgprouterBLOCK, (ulong) pdb);

	/*
	 * If we don't have a router ID, don't do anything.
	 */
	if (!pdb_BGP->bgp_rtr_id) {
	    pdb_BGP->bgp_rtr_id = bgp_rtr_id();
	    if (!pdb_BGP->bgp_rtr_id)
		goto suspend_processing;
	}

	readmsgs = FALSE;
	for (bgp = pdb->neighbors; bgp; bgp = bgp_next) {
	    bgp_next = bgp->next;

	    process_may_suspend();

	    if (bgp->flags & BN_DELETED) {
		bgp_delete_neighbor(bgp);
		continue;
	    }

	    /*
	     * If connection is in an error state (BGPS_CLOSING)
	     * wait until bgp_writeQ is empty before aborting connection.
	     */
 	    if (bgp->bgp_state == BGPS_CLOSING) {
		if (QUEUEEMPTY(&bgp->bgp_writeQ) &&
		    QUEUEEMPTY(&bgp->bgp_readQ) &&
		    CLOCK_OUTSIDE_INTERVAL(bgp->bgp_lastread, 5*ONESEC)) { 
		    if (bgp_debug)	
			buginf("\nBGP: %i local error close", bgp->address);
		    bgp_reset(bgp, NULL);
		    continue;
		}
	    }

	    /*
	     * Try opening or restarting a closed connection.
	     * Make sure there is at least one minute since we last read
	     * from the connection to ensure some opening hysteresis.
	     * Skip this neighbor if it is waiting for index processing.
	     */
	    if ((bgp->bgp_state == BGPS_IDLE) &&
		!(bgp->flags & BN_TEMPORARY) &&
		(CLOCK_OUTSIDE_INTERVAL(bgp->bgp_lastread, BGP_OPEN_DELAY) ||
		 (bgp->flags & BN_RESTART))) {
		/*
		 * If we are in the process of recovering/allocating
		 * neighbor index, do not try to restart. Also do not
		 * restart an external neighbor without neighbor
		 * index.
		 */
		if ((bgp->flags & (BN_ALLOCATE_INDEX+BN_RECLAIM_INDEX)) ||
		    (!bgp->bgp_internal && !bgp->neighbor_index))
		    continue;

		if (bgpevent_debug && bgp->tcb)
		    buginf("\nBGP: %i restarting", bgp->address);
		bgp_close(bgp);

                /*
                 * ebgp peers must be directly connected, or the
                 * multi-hop hack has been set for this neighbor
                 */
                if (bgp->bgp_internal ||
		    bgp->ebgp_ttl != BGP_DEFAULT_EBGP_TTL ||
                    samecable(bgp->address)) {
                    bgp_statechange(bgp, BGPS_ACTIVE);
                    bgp->open_pid = cfork((forkproc *)bgp_open_active,
					   bgp->address, 0, "BGP Open", 0L); 
                }
		continue;
	    }

	    /*
	     * Must have a TCP connection after here
	     */
	    if (bgp->tcb == NULL) {
		if (!(bgp->bgp_state == BGPS_IDLE) &&
		    !(bgp->bgp_state == BGPS_CONNECT) &&
		    !(bgp->bgp_state == BGPS_ACTIVE)) {
		    if (bgp_debug)
			buginf("\nBGP: %i lost tcb for active connection",
			       bgp->address); 
		    bgp_reset(bgp, NULL);
		}
		continue;
	    }

	    /*
	     * Handle BGP connections that have timed out
	     */
	    if (bgp->bgp_holdtime &&
		CLOCK_OUTSIDE_INTERVAL(bgp->bgp_lastread, bgp->bgp_holdtime)) {
		if (bgp_debug)
		    buginf("\nBGP: %i connection timed out "
			   "%ums (last update) %ums (hold time)",
			   bgp->address,
			   ELAPSED_TIME(bgp->bgp_lastread),
			   bgp->bgp_holdtime);
		bgp_reset(bgp, NULL);
		continue;
	    }

	    /*
	     * Handle BGP connection closed by the other side
	     */
	    if (tcp_closing(bgp->tcb)) {
		if (bgp_debug)
		    buginf("\nBGP: %i remote close, state %s",
			   bgp->address, tcp_statestring(bgp->tcb->state));
		bgp_reset(bgp, NULL);
		continue;
	    }

  	    /*
	     * If reading got us any commands, execute those commands.
	     */
	    if (!QUEUEEMPTY(&bgp->bgp_readQ)) {
		bgp_docommand(bgp);
		readmsgs = TRUE;
		suspend = TRUE;
	    }

	    /*
	     * Check if per neighbor default need to be originated
	     */
	    bgp_default_originate(bgp);
	}

	process_suspend();

	/*
	 * Delay for a while and handle EBGP neighbor indexing
	 * This will allow as us to handle major config changes
	 * in one single try! 
	 */
	if (TIMER_RUNNING_AND_AWAKE(bgp_neighbor_index_timer)) {
	    bgp_neighbor_indexing();
	}

	/*
	 * Check reuse array every BD_REUSE_INTERVAL for entries
	 * to be unsuppressed.
	 * This is done first so that paths that are unsuppressed
	 * are used for other purpose.
	 */
	if (bgp_dampening_enabled && 
	    CLOCK_OUTSIDE_INTERVAL(pdb->bgp_reusetime, BD_REUSE_INTERVAL)) {
	    bgp_unsuppress();
	    GET_TIMESTAMP(pdb->bgp_reusetime);
	    process_may_suspend();
	}

	/*
	 * Clean the routemap/filter-list cache.
	 * bgp_pruge_cache flag is set whenever there is an
	 * access-list or community-list or route-map change.
	 */
	if (bgp_purge_cache) {
	    bgp_reset_rcache();
	    bgp_reset_fcache();
	    bgp_purge_cache = FALSE;
	}

	/*
	 * Service any redistribution/synchronization requests.
	 * This goes first so that we can update path info before we
	 * service a possible add request.
	 */
	while (!QUEUEEMPTY(&bgp_redistQ)) {
	    bgp_service_callback(dequeue(&bgp_redistQ));
	    readmsgs = TRUE;
	    process_may_suspend();
	}

	/*
	 * Service any reset requests.
	 */
	while (!QUEUEEMPTY(&bgp_resetQ)) {
	    bgp_service_reset(dequeue(&bgp_resetQ));
	    readmsgs = TRUE;
	    process_may_suspend();
	}

	/*
	 * check if multipath configuration has changed.
	 */
	if (((pdb_BGP->multipath != BGP_DEFAULT_MULTIPATH) &&
	     !bgp_multipath_enabled) ||
	    ((pdb_BGP->multipath == BGP_DEFAULT_MULTIPATH) &&
	     bgp_multipath_enabled)) {
	    bgp_config_multipath();
	}
	

	/*
	 * Service any add requests.
	 */
	while (!QUEUEEMPTY(&bgp_requestQ)) {
	    bgp_service_add_req(dequeue(&bgp_requestQ));
	    readmsgs = TRUE;
	    process_may_suspend();
	}

	/*
	 * Check to see if there are any aggregates to install and
	 */
	if (!QUEUEEMPTY(&bgp_aggregateQ) &&
	    (bgp_aggregate_version != bgptable_version)) {
	    bgp_aggregate(&bgp_aggregateQ);
	    bgp_aggregate_version = bgptable_version;
	}

	if (!QUEUEEMPTY(&bgp_backdoorQ))
	    bgp_backdoor();

	GET_TIMESTAMP(max_update_time);

	/*
	 * Check if soft reconfiguration needs to be done.
	 */
	if (bgp_check_soft_reconfig)
	    bgp_soft_reconfig();

	/*
	 * Update the main IP routing table if necessary.
	 */
	if (bgptable_version != bgp_nettable_version)
	    bgp_update_nettable();

	perform_updates = (!readmsgs || blocked);
	blocked = FALSE;

	/*
	 * First go thru all the members of the peer-group.
	 * Peergroup timer values are used to decide on update
	 * to group members.
	 */
	for (pgrp = pdb->peergroup_list; pgrp; pgrp = pgrp->next) {
	    if (!pgrp->peerhead)
		continue;

	    if (!perform_updates && 
		!CLOCK_OUTSIDE_INTERVAL(pgrp->bgp_last_upd_check,
					3*pgrp->bgp_minadverttime))
		continue;

	    if (TIMER_RUNNING_AND_SLEEPING(pgrp->bgp_last_upd_sent))
		continue;
		
	    /*
	     * Get the candidate for update. Could loop multiple 
	     * times until all the members are in sync. When all 
	     * are sync, this loop will be executed not more than
	     * once.
	     *
	     * When we suspend, bgp_scanner could change the 
	     * bgptable_version. Remember it to avoid race condition.
	     */
	    upd_version = bgptable_version;
	    bgp = bgp_upd_candidate(pgrp, upd_version);
	    while (bgp) {

		if (mempool_is_empty(MEMPOOL_CLASS_LOCAL)) {
		    suspend = FALSE;
		    blocked = TRUE;
		    if ((bgpupdate_debug && !bgpupdate_acl) || bgpevent_debug)
			buginf("\nBGP: all updates throttled - low memory "
			       "(%u bytes free)",
			       mempool_get_free_bytes(MEMPOOL_CLASS_LOCAL,
						      MEMPOOL_ALTERNATE_INCLUDE));
		    goto suspend_processing;
		}

		if (bgp_updates(bgp))
		    blocked = TRUE;
		suspend = TRUE;

		if (CLOCK_OUTSIDE_INTERVAL(max_update_time, BGP_MAXUPDATETIME))
		    goto suspend_processing;

		bgp = bgp_upd_candidate(pgrp, upd_version);
	    }
	    GET_TIMESTAMP(pgrp->bgp_last_upd_check);
	}


	for (bgp = pdb->neighbors; bgp; bgp = bgp->next) {

	    if (bgp->peergroup || 		       /* skip group members */
		(last_peer_checked >= bgp->address))
		continue;

	    last_peer_checked = bgp->address;

	    if (perform_updates || 
		CLOCK_OUTSIDE_INTERVAL(bgp->bgp_last_upd_check,
				       3*bgp->bgp_minadverttime)) {

		GET_TIMESTAMP(bgp->bgp_last_upd_check);
	
		/*
		 * If the table version has changed, force an update.
		 */
		if ((bgp->version != bgptable_version) &&
		    (bgp->bgp_state == BGPS_ESTAB) &&
		    TIMER_RUNNING_AND_AWAKE(bgp->bgp_last_upd_sent)) {
		    if (mempool_is_empty(MEMPOOL_CLASS_LOCAL)) {
			suspend = FALSE;
			blocked = TRUE;
			if ((bgpupdate_debug && !bgpupdate_acl) ||
			    bgpevent_debug)
			    buginf("\nBGP: all updates throttled - low memory "
				   "(%u bytes free)",
				   mempool_get_free_bytes(MEMPOOL_CLASS_LOCAL,
							  MEMPOOL_ALTERNATE_INCLUDE));
			goto suspend_processing;
		    }

		    if (bgp_updates(bgp))
			blocked = TRUE;
		    suspend = TRUE;
		}
	    }
	    if (CLOCK_OUTSIDE_INTERVAL(max_update_time, BGP_MAXUPDATETIME))
		goto suspend_processing;
	}
	last_peer_checked = 0;
    suspend_processing:
    }
}

/*
 * bgp_async_reset
 * Create a reset request to clear a given neighbor
 */

BGP_PRIVATE void
bgp_async_reset (ipaddrtype address)
{
    bworktype *work;

    if (!(work = malloc(sizeof(bworktype)))) {
	return;
    }

    work->address = address;
    enqueue(&bgp_resetQ, work);
}

/*
 * bgp_route_adjust
 *
 * Notification that an interface has changed state -- if the interface is
 * now down, reset any ebgp peers that are through that interface.
 */

BGP_PRIVATE void
bgp_route_adjust (idbtype *idb, boolean passive)
{
    neighbortype *bgp;

    if (!idb || passive || !pdb_BGP || !bgp_fast_reset || interface_up(idb))
      return;			/* nothing to be done */

    for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next)
	if ((bgp->bgp_state == BGPS_ESTAB) && (bgp->idb == idb)) {
	    if (bgp_debug)
		buginf("\nBGP: %i resetting - interface %s down",
		       bgp->address, idb->namestring);
	    bgp_async_reset(bgp->address);
	}
}

/*
 * clear_ip_bgp_dampinfo
 * clear the damping information for all or a selected route
 */
BGP_PUBLIC void
clear_ip_bgp_dampinfo (parseinfo *csb)
{
    ipaddrtype address, mask;
    bnettype *net;

    if (!pdb_BGP || !pdb_BGP->running || !bgp_dampening_enabled)
	return;

    address = mask = 0L;
    if (GETOBJ(paddr, 1)->type == ADDR_IP) {
	address = GETOBJ(paddr, 1)->ip_addr;

	if (GETOBJ(paddr, 2)->type == ADDR_IP)
	    mask = GETOBJ(paddr, 2)->ip_addr;

	net = bgp_netlookup(address, mask);
	if (net)
	    bgp_clean_dampinfo(net);
    } else
	rn_walktree_timed(bgp_head, bgp_free_dampinfo_walker, 
			  ip_radix_successor);
}

/*
 * clear_ip_bgp_pgrp 
 * clear the connections of peer-group neighbors
 */
BGP_PUBLIC void
clear_ip_bgp_pgrp (parseinfo *csb)
{
    neighbortype *bgp, *pgrp;
    uchar *nametag;

    if (!pdb_BGP || !pdb_BGP->running)
	return;
    
    nametag = GETOBJ(string, 1);
    for (pgrp = pdb_BGP->peergroup_list; pgrp; pgrp = pgrp->next)
	if (!strcmp(pgrp->neighbor_name, nametag))
	    break;

    if (!pgrp) {
	printf ("\n%% No such peer-group");
	return;
    }

    for (bgp = pgrp->peerhead; bgp; bgp = bgp->nextpeer)
	bgp_async_reset(bgp->address);
}

/*
 * bgp_clear_fstat_flist_walker
 */
BGP_PRIVATE int
bgp_clear_fstat_flist_walker (rntype *rn, va_list ap)
{
    bnettype *net	= rn2bnettype(rn);
    char *path_buffer	= va_arg(ap, char *);
    int acl_number	= va_arg(ap, int);

    bpathtype *path;
    int i;
    battrtype *attr;
    char *pathstr;

    for (i = net->firstpath; i != BGP_NULL_INDEX; i = path->next) {
	path = &net->paths[i];
	if (!path->dampinfo ||
	    !path->dampinfo->flap_count)
	    continue;

	attr = path->attr;
	pathstr = bgp_path2string(attr->as_path, attr->aslength, path_buffer,
				  ASCII_PATHBUFSIZE);
	if (!regexp_accesscheck(pathstr, acl_number, as_path_accesslists))
	    continue;

	GET_TIMESTAMP(path->dampinfo->start_time);
	path->dampinfo->flap_count = 0;
	}

    process_may_suspend();
    return WALK_CONTINUE;
}

/*
 * bgp_clear_fstat_regexp_walker
 */
BGP_PRIVATE int
bgp_clear_fstat_regexp_walker (rntype *rn, va_list ap)
{
    bnettype *net	= rn2bnettype(rn);
    char *path_buffer	= va_arg(ap, char *);
    regexp *pattern	= va_arg(ap, regexp *);

    bpathtype *path;
    int i;
    battrtype *attr;
    char *pathstr;

    for (i = net->firstpath; i != BGP_NULL_INDEX; i = path->next) {
	path = &net->paths[i];
	if (!path->dampinfo)
	    continue;

	attr = path->attr;
	pathstr = bgp_path2string(attr->as_path, attr->aslength, path_buffer,
				  ASCII_PATHBUFSIZE);
	if (!pathstr || (!regexec(pattern, pathstr)))
	    continue;

	GET_TIMESTAMP(path->dampinfo->start_time);
	path->dampinfo->flap_count = 0;
    }
    
    process_may_suspend();
    return WALK_CONTINUE;
}

/*
 * bgp_clear_fstat_neighbor_walker
 */
BGP_PRIVATE int
bgp_clear_fstat_neighbor_walker (rntype *rn, va_list ap)
{
    bnettype *net	= rn2bnettype(rn);
    neighbortype *bgp	= va_arg(ap, neighbortype *);

    bpathtype *path;
    int i;

    for (i = net->firstpath; i != BGP_NULL_INDEX; i = path->next) {
	path = &net->paths[i];
	if ((!path->dampinfo) ||
	    (path->neighbor != bgp->address))
	    continue;

	GET_TIMESTAMP(path->dampinfo->start_time);
	path->dampinfo->flap_count = 0;
    }
    
    process_may_suspend();
    return WALK_CONTINUE;
}

/*
 * bgp_clear_fstat
 */
BGP_PRIVATE void
bgp_clear_fstat (bnettype *net)
{
    int i;
    bpathtype *path;

    for (i = net->firstpath; i != BGP_NULL_INDEX; i = path->next) {
	path = &net->paths[i];
	if (!path->dampinfo)
	    continue;

	GET_TIMESTAMP(path->dampinfo->start_time);
	path->dampinfo->flap_count = 0;
    }
}

/*
 * bgp_clear_fstat_walker
 */
BGP_PRIVATE int
bgp_clear_fstat_walker (rntype *rn, va_list ap)
{
    bnettype *net	= rn2bnettype(rn);
    
    bgp_clear_fstat(net);
    
    process_may_suspend();
    return WALK_CONTINUE;
}

/*
 * clear_ip_bgp_soft
 *
 * Set the appropriate BN flag. The actual clear will be done via
 * bgp_router.
 */
BGP_PUBLIC void
clear_ip_bgp_soft (parseinfo *csb)
{
    ipaddrtype address;
    neighbortype *bgp, *pgrp;
    uchar *nametag;
    ulong flag;
    int asnumber;
    boolean match;

    if (!pdb_BGP || !pdb_BGP->running)
	return;

    if (GETOBJ(int,2))
	flag = BN_SOFT_RECONFIG_DOIN;
    else if (GETOBJ(int,3))
	flag = BN_SOFT_RECONFIG_DOOUT;
    else
	flag = (BN_SOFT_RECONFIG_DOIN+BN_SOFT_RECONFIG_DOOUT);

    switch (csb->which) {
    case BGP_CLEAR_SOFT_NEIGH:
	address = GETOBJ(paddr,1)->ip_addr;
	for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next)
	    if (bgp->address == address)
		break;

	if (!bgp) {
	    printf("\n%% Unknown neighbor - \"%i\"", address);
	    return;
	}
	
	if ((flag & BN_SOFT_RECONFIG_DOIN) &&
	    !(bgp->flags & BN_SOFT_RECONFIG_INOK)) {
	    printf("\n%% Inbound soft reconfiguration not enabled");
	    flag &= ~BN_SOFT_RECONFIG_DOIN;
	}

	bgp->flags |= flag;
	bgp_check_soft_reconfig = TRUE;
	break;

    case BGP_CLEAR_SOFT_PGRP:
	nametag = GETOBJ(string, 1);
	for (pgrp = pdb_BGP->peergroup_list; pgrp; pgrp = pgrp->next)
	    if (!strcmp(pgrp->neighbor_name, nametag))
		break;

	if (!pgrp) {
	    printf ("\n%% No such peer-group");
	    return;
	}

	/*
	 * As inbound policy could be different for peer-group
	 * members, check individual inbound soft reconfig in
	 * bgp_soft_reconfig(). 
	 */
	for (bgp = pgrp->peerhead; bgp; bgp = bgp->nextpeer)
		bgp->flags |= flag;

	bgp_check_soft_reconfig = TRUE;
	break;

    case BGP_CLEAR_SOFT_ALL:
	for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next)
	    bgp->flags |= flag;

	bgp_check_soft_reconfig = TRUE;
	break;

    case BGP_CLEAR_SOFT_AS :
      asnumber = GETOBJ(int,1);
      match = FALSE;
      for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next) {
          if (bgp->yoursystem == asnumber) {
              match = TRUE;
              bgp->flags |= flag;
              if ((flag & BN_SOFT_RECONFIG_DOIN) &&
                  !(bgp->flags & BN_SOFT_RECONFIG_INOK)) {
                  printf("\n%% Inbound soft reconfiguration disabled for %i",
                         bgp->address);
                  bgp->flags &= ~BN_SOFT_RECONFIG_DOIN;
              }
          }
      }
      if (match)
          bgp_check_soft_reconfig = TRUE;
      else
          printf("\n%% No peer is configured with AS %d", asnumber);
      break;

    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * clear_ip_bgp
 * Clear all of our connections or one of them.
 */

BGP_PUBLIC void
clear_ip_bgp (parseinfo *csb)
{
    ipaddrtype address, mask;
    bnettype *net;
    neighbortype *bgp;
    regexp *pattern;
    int number, asnumber;
    char *path_buffer;
    boolean match;

    if (!pdb_BGP || !pdb_BGP->running)
	return;

    path_buffer = malloc(ASCII_PATHBUFSIZE);
    if (!path_buffer) {
	printf(nomemory);
	return;
    }

    switch (csb->which) {
    case BGP_CLEAR_NEIGH :
	if (GETOBJ(int,1) == TRUE) {
	    address = GETOBJ(paddr,1)->ip_addr;
	    for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next)
		if (bgp->address == address)
		    break;
	    if (!bgp) {
		printf("\n%% Unknown neighbor - \"%i\"", address);
		return;
	    }
	} else
	    address = 0;

	bgp_async_reset(address);
	break;

    case BGP_CLEAR_FSTAT :
	rn_walktree_blocking(bgp_head->rnh_treetop, bgp_clear_fstat_walker);
	break;

    case BGP_CLEAR_FSTAT_ADDR :
	address = GETOBJ(paddr,1)->ip_addr;
	mask = 0L;				/* do longest match */
	
	if (GETOBJ(paddr,2)->type == ADDR_IP)
	    mask = GETOBJ(paddr,2)->ip_addr;
	net = bgp_netlookup(address, mask);
	if (!net) {
	    printf("\n%% Network not in table");
	    break;
	}
	
	bgp_clear_fstat(net);
	break;

    case BGP_CLEAR_FSTAT_REGEXP :
	pattern = regcomp(GETOBJ(string,1));
	if (pattern)
	    rn_walktree_blocking(bgp_head->rnh_treetop, 
				 bgp_clear_fstat_regexp_walker, path_buffer,
				 pattern);
	else
	    printf("\n%% Invalid regular expression");
	break;

    case BGP_CLEAR_FSTAT_FLIST :
	number = GETOBJ(int,1);
	if (!as_path_accesslists[number]) {
	    printf("\n%% %u is not a valid AS-path access-list number",
		   number);
	    break;
	}
	rn_walktree_blocking(bgp_head->rnh_treetop,
			     bgp_clear_fstat_flist_walker, 
			     path_buffer, number);
	break;

    case BGP_CLEAR_FSTAT_NEIGH :
	address = GETOBJ(paddr,1)->ip_addr;
	for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next)
	    if (bgp->address == address)
		break;

	if (!bgp) {
	    printf("\n%% Unknown neighbor - \"%i\"", address);
	    return;
	}
	rn_walktree_blocking(bgp_head->rnh_treetop,
			     bgp_clear_fstat_neighbor_walker, bgp);
	break;

    case BGP_CLEAR_NEIGH_AS :
	asnumber = GETOBJ(int, 1);
	match = FALSE;
	for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next) {
	    if (bgp->yoursystem == asnumber) {
		bgp_async_reset(bgp->address);
		match = TRUE;
	    }
	}
	if (!match)
	    printf("\n%% No neighbor is configured with AS %d", asnumber);
	break;

    default :
	bad_parser_subcommand(csb, csb->which);
    }
}

/*
 * bgp_distance
 * Return the administrative distance for a particular path.
 */

BGP_PUBLIC int
bgp_distance (pdbtype *pdb, ipaddrtype target, ushort route_type, int flags,
	      int *pdb_peerindex)
{
    if (route_type == RTYPE_BGP_EXT) 
	return (pdb->distance);
    else if (route_type == RTYPE_BGP_LOC)
	return (pdb->distance3);
    else
	return (pdb->distance2);
}

/*
 * bgp_originstring
 * Return the string associated with the origin code.
 */

BGP_PRIVATE const char *
bgp_originstring (int origin, boolean uppercase)
{
    switch (origin) {
      case ORIGIN_IGP: return (uppercase ? "IGP" : "igp");
      case ORIGIN_EGP: return (uppercase ? "EGP" : "egp");
      case ORIGIN_INCOMPLETE: return ("incomplete");
    }
    return ("unknown");
}

/*
 * bgp_originchar
 * Return a single character origin code.
 */

BGP_PRIVATE char
bgp_originchar (int origin)
{
    switch (origin) {
      case ORIGIN_IGP: return ('i');
      case ORIGIN_EGP: return ('e');
      case ORIGIN_INCOMPLETE: return ('?');
    }
    return ('?');
}

/*
 * bgp_print_entry
 * Display a bgp entry.
 * Side Effect: i is set to 0 when the 1st route to a net is displayed
 */

BGP_PRIVATE void
bgp_print_entry (bnettype *net, bpathtype *path, battrtype *attr,
		 char *pathstr, int i, boolean print_net_addr) 
{
    int len;

    printf("\n%c%c%c",
	   ((net->flags & BNET_SUPPRESSED) ? 's' : 
	    ((path->flags & BFLAG_VALID) ? '*' : ' ')),
	   ((i == net->best) ? '>' : 
	    ((path->flags & BFLAG_SUPPRESSED) ? 'd' : 
	     ((path->flags & BFLAG_HISTORY) ? 'h' : ' '))),
	   ((path->flags & BFLAG_INTERNAL) ? 'i' : ' '));

     if (print_net_addr) {
	 len = printf("%i", net->network);
	 if (net->mask != get_majormask(net->network))
	     len += printf("/%d", ip_bitsinmask(net->mask));
	 printf("%*c", 17-len, ' ');
	 printf("%15i ", path->gateway);
     } else
	 printf("%16c %15i ", ' ', path->gateway);

    if (path->flags & BFLAG_METRIC)
	printf("%-8u ", attr->metric);
    else
	printf("%8c ", ' ');

    if (path->flags & BFLAG_LOCALPREF)
	printf("%-6u ", attr->localpref);
    else
	printf("%6c ", ' ');

    printf("%-6u ", path->weight);

    if (attr->aslength)
	printf("%s ", pathstr);

    printc(bgp_originchar(attr->origin));
}

/*
 * bgp_process_onceflag
 * Used to process the onceflag while displaying.
 */

BGP_PRIVATE const char bgp_show_prologue[] =
  "\nBGP table version is %u, local router ID is %i"
  "\nStatus codes: s suppressed, d damped, h history, * valid, > best, i - internal"
  "\nOrigin codes: i - IGP, e - EGP, ? - incomplete\n";

BGP_PRIVATE const char bgpheader[] =
  "   Network          Next Hop          Metric LocPrf Weight Path";

BGP_PRIVATE const char bgp_notrunning[] =
  "%% BGP not active\n";

BGP_PRIVATE const char bgp_fstat_header[] =
  "   Network          From            Flaps Duration Reuse    Path";

static inline void
bgp_process_onceflag (int *onceflag)
{
    if (!*onceflag) {
	printf(bgp_show_prologue, bgptable_version, pdb_BGP->bgp_rtr_id);
	automore_header(bgpheader);
	*onceflag = TRUE;
    }
}
static inline void
bgp_process_fstat_onceflag (int *onceflag)
{
    if (!*onceflag) {
	printf(bgp_show_prologue, bgptable_version, pdb_BGP->bgp_rtr_id);
	automore_header(bgp_fstat_header);
	*onceflag = TRUE;
    }
}

/*
 * bgp_print_fstat_entry
 * Display a flap statistics for an entry.
 */
BGP_PRIVATE void
bgp_print_fstat_entry (bnettype *net, bpathtype *path, battrtype *attr,
		       char *pathstr, int i, boolean print_net_addr)
{
    int len;
    ulong reuse_time;
    char time[10];
    bdampinfotype *dampinfo;

    /*
     * As dcb is not locked get reuse time before starting to print.
     * Decay the penalty before calculating the reuse time.
     */
    reuse_time = 0;
    dampinfo = path->dampinfo;
    if (path->flags & BFLAG_SUPPRESSED) {
	bgp_decay_penalty(dampinfo);
	reuse_time = bgp_get_reuseticks(dampinfo) * BD_REUSE_INTERVAL;
    }

    printf("\n%c%c%c",
	   ((net->flags & BNET_SUPPRESSED) ? 's' : 
	    ((path->flags & BFLAG_VALID) ? '*' : ' ')), 
	   ((i == net->best) ? '>' : 
	    ((path->flags & BFLAG_SUPPRESSED) ? 'd' : 
	     ((path->flags & BFLAG_HISTORY) ? 'h' : ' '))),
	   ((path->flags & BFLAG_INTERNAL) ? 'i' : ' '));

     if (print_net_addr) {
	 len = printf("%i", net->network);
	 if (net->mask != get_majormask(net->network))
	     len += printf("/%d", ip_bitsinmask(net->mask));
	 printf("%*c", 17-len, ' ');
	 printf("%15i ", path->neighbor);
     } else
	 printf("%16c %15i ", ' ', path->neighbor);

    printf("%5u ", dampinfo->flap_count);

    printf("%8TE ", dampinfo->start_time);

    if (path->flags & BFLAG_SUPPRESSED) {
	sprint_ticks_in_dhms(time, reuse_time);
	printf("%8s ", time);
    } else
	printf("%8c ", ' ');

    if (attr->aslength)
	printf("%s ", pathstr);
}

/*
 * bgp_show_walker
 * Show all the entries in the bgp table.
 * Callback from show_bgp via tree walker.
 */

BGP_PRIVATE int
bgp_show_walker (rntype *rn, va_list ap)
{
    bnettype *net	  = rn2bnettype(rn);
    char     *path_buffer = va_arg(ap, char *);
    boolean  *onceflag    = va_arg(ap, boolean *);
    boolean   cidr_only   = va_arg(ap, boolean);

    bpathtype *path;
    int i;
    battrtype *attr;
    char *pathstr;
    boolean print_net_addr;

    if (cidr_only && net->mask == get_majormask(net->network)) {
	process_may_suspend();
	return WALK_CONTINUE;
    }

    print_net_addr = TRUE;
    for (i = net->firstpath; i != BGP_NULL_INDEX; i = path->next) {
	path = &net->paths[i];
	if (path->flags & BFLAG_RCVD_ONLY)
	    continue;

	attr = path->attr;
	pathstr = bgp_path2string(attr->as_path, attr->aslength, path_buffer,
				  ASCII_PATHBUFSIZE);
	mem_lock(attr);
	bgp_process_onceflag(onceflag);
	bgp_print_entry(net, path, attr, pathstr, i, print_net_addr);
	print_net_addr = FALSE;
	free(attr);
    }

    process_may_suspend();
    if (automore_quit())
	return WALK_ABORT;
    else	
	return WALK_CONTINUE;
}

/*
 * bgp_show_inconsistent_walker
 * Show all the entries in the bgp table that have inconsistent origin ASs
 * Callback from show_bgp via tree walker.
 */

BGP_PRIVATE int
bgp_show_inconsistent_walker (rntype *rn, va_list ap)
{  
    bnettype *net     = rn2bnettype(rn);
    char *path_buffer = va_arg(ap, char *);
    boolean *onceflag = va_arg(ap, boolean *);

    bpathtype *path;
    int i;
    battrtype *attr;
    char *pathstr;
    int first_len, check_len;
    ushort first_buf[BGP_ORIGIN_BUFLEN], check_buf[BGP_ORIGIN_BUFLEN];
    boolean print_net_addr;

    if (net->pathcount < 2)
	goto punt;

    for (i = net->firstpath; i != BGP_NULL_INDEX; i = net->paths[i].next)
	if ((net->paths[i].flags & BFLAG_RCVD_ONLY) ||
	    (net->paths[i].flags & BFLAG_HISTORY))
	    continue;
	else
	    break;

    if (i == BGP_NULL_INDEX)
	goto punt;
	    
    path = &net->paths[i];
    attr = path->attr;
    first_len = bgp_path2originas(attr->as_path, attr->aslength,
				  first_buf, BGP_ORIGIN_BUFLEN);

    for (i = path->next; i != BGP_NULL_INDEX; i = net->paths[i].next) {
	if ((net->paths[i].flags & BFLAG_RCVD_ONLY) ||
	    (net->paths[i].flags & BFLAG_HISTORY))
	    continue;

	attr = net->paths[i].attr;
	check_len = bgp_path2originas(attr->as_path, attr->aslength,
				      check_buf, BGP_ORIGIN_BUFLEN);
	if ((check_len != first_len) || bcmp(first_buf, check_buf, first_len))
	    break;
    }

    if (i == BGP_NULL_INDEX)
	goto punt;

    bgp_process_onceflag(onceflag);
    print_net_addr = TRUE;

    for (i = net->firstpath; i != BGP_NULL_INDEX; i = net->paths[i].next) {
	if ((net->paths[i].flags & BFLAG_RCVD_ONLY) ||
	    (net->paths[i].flags & BFLAG_HISTORY))
	    continue;

	path = &net->paths[i];
	attr = path->attr;
	pathstr = bgp_path2string(attr->as_path, attr->aslength, path_buffer,
				  ASCII_PATHBUFSIZE);
	mem_lock(attr);
	bgp_print_entry(net, path, attr, pathstr, i, print_net_addr);
	print_net_addr = FALSE;
	free(attr);
    }

  punt:
    process_may_suspend();

    if (automore_quit())
	return WALK_ABORT;
    else	
	return WALK_CONTINUE;
}

/*
 * show_bgp_net_walker
 * Show all the entries in the bgp table matching address/mask.
 * Callback from bgp_show_nets_walker via tree walker.
 */

BGP_PRIVATE int
bgp_show_nets_walker (rntype *rn, va_list ap)
{  
    bnettype *net	= rn2bnettype(rn);
    char *path_buffer	= va_arg(ap, char *);
    boolean *onceflag	= va_arg(ap, boolean *);
    ipaddrtype address	= va_arg(ap, ipaddrtype);
    ipaddrtype mask	= va_arg(ap, ipaddrtype);
    boolean flap_stat	= va_arg(ap, boolean);

    bpathtype *path;
    int i;
    battrtype *attr;
    char *pathstr;
    boolean print_net_addr;

    if ((net->network & mask) != address)
	return WALK_ABORT;

    if (net->mask < mask)
	return WALK_CONTINUE;

    print_net_addr = TRUE;
    for (i = net->firstpath; i != BGP_NULL_INDEX; i = path->next) {
	path = &net->paths[i];

	if (flap_stat) {
	    if (!bgp_dampening_enabled)
		return WALK_ABORT;
	    else if (!path->dampinfo ||
		     !path->dampinfo->flap_count)
		continue;
	}

 	if (path->flags & BFLAG_RCVD_ONLY)
 	    continue;


	attr = path->attr;
	pathstr = bgp_path2string(attr->as_path, attr->aslength, path_buffer,
				  ASCII_PATHBUFSIZE);
	mem_lock(attr);
	if (flap_stat) {
	    bgp_dinfo_lock(path->dampinfo);
	    bgp_process_fstat_onceflag(onceflag);
	    bgp_print_fstat_entry(net, path, attr, pathstr, i, print_net_addr);
	    bgp_dinfo_free(path->dampinfo);
	} else {
	    bgp_process_onceflag(onceflag);
	    bgp_print_entry(net, path, attr, pathstr, i, print_net_addr);
	}
	print_net_addr = FALSE;
	free(attr);
	print_net_addr = FALSE;
    }

    if (automore_quit())
	return WALK_ABORT;
    else	
	return WALK_CONTINUE;
}

/*
 * show_bgp_filter_list_walker
 * Show the entries that confirm to the access-list.
 * Callback from show_bgp via tree walker.
 */

BGP_PRIVATE int
bgp_show_filter_list_walker (rntype *rn, va_list ap)
{  
    bnettype *net	= rn2bnettype(rn);
    char *path_buffer	= va_arg(ap, char *);
    boolean *onceflag	= va_arg(ap, boolean *);
    int acl_number	= va_arg(ap, int);
    boolean flap_stat	= va_arg(ap, boolean);

    bpathtype *path;
    int i;
    battrtype *attr;
    char *pathstr;
    boolean print_net_addr;

    print_net_addr = TRUE;

    for (i = net->firstpath; i != BGP_NULL_INDEX; i = path->next) {
	path = &net->paths[i];

	if (flap_stat) {
	    if (!bgp_dampening_enabled)
		return WALK_ABORT;
	    else if (!path->dampinfo ||
		     !path->dampinfo->flap_count)
		continue;
	}

 	if (path->flags & BFLAG_RCVD_ONLY)
 	    continue;

	attr = path->attr;
	pathstr = bgp_path2string(attr->as_path, attr->aslength, path_buffer,
				  ASCII_PATHBUFSIZE);
	if (!regexp_accesscheck(pathstr, acl_number, as_path_accesslists))
	    continue;
	
	mem_lock(attr);
	if (flap_stat) {
	    bgp_dinfo_lock(path->dampinfo);
	    bgp_process_fstat_onceflag(onceflag);
	    bgp_print_fstat_entry(net, path, attr, pathstr, i, print_net_addr);
	    bgp_dinfo_free(path->dampinfo);
	} else {
	    bgp_process_onceflag(onceflag);
	    bgp_print_entry(net, path, attr, pathstr, i, print_net_addr);
	}
	print_net_addr = FALSE;
	free(attr);
    }

    process_may_suspend();
    if (automore_quit())
	return WALK_ABORT;
    else	
	return WALK_CONTINUE;
}

/*
 * bgp_show_regexp_walker
 * Show the entries that confirm to the regular expression.
 * Callback from show_bgp via tree walker.
 */

BGP_PRIVATE int
bgp_show_regexp_walker (rntype *rn, va_list ap)
{  
    bnettype *net	= rn2bnettype(rn);
    char *path_buffer	= va_arg(ap, char *);
    boolean *onceflag	= va_arg(ap, boolean *);
    regexp *pattern	= va_arg(ap, regexp *);
    boolean flap_stat	= va_arg(ap, boolean);

    bpathtype *path;
    int i;
    battrtype *attr;
    char *pathstr;
    boolean print_net_addr;

    print_net_addr = TRUE;

    for (i = net->firstpath; i != BGP_NULL_INDEX; i = path->next) {
	path = &net->paths[i];
	if (flap_stat) {
	    if (!bgp_dampening_enabled)
		return WALK_ABORT;
	    else if (!path->dampinfo ||
		     !path->dampinfo->flap_count)
		continue;
	}

 	if (path->flags & BFLAG_RCVD_ONLY)
 	    continue;

	attr = path->attr;
	pathstr = bgp_path2string(attr->as_path, attr->aslength, path_buffer,
				  ASCII_PATHBUFSIZE);
	if (!pathstr || (!regexec(pattern, pathstr)))
	    continue;

	mem_lock(attr);
	if (flap_stat) {
	    bgp_dinfo_lock(path->dampinfo);
	    bgp_process_fstat_onceflag(onceflag);
	    bgp_print_fstat_entry(net, path, attr, pathstr, i, print_net_addr);
	    bgp_dinfo_free(path->dampinfo);
	} else {
	    bgp_process_onceflag(onceflag);
	    bgp_print_entry(net, path, attr, pathstr, i, print_net_addr);
	}
	print_net_addr = FALSE;
	free(attr);
    }
    
    process_may_suspend();
    if (automore_quit())
	return WALK_ABORT;
    else	
	return WALK_CONTINUE;
}

/*
 * bgp_calc_memory_walker
 *
 * Calculate the ammount of memory taken up by a given
 * bgp table entry.  Paths are counted, attribute entries are not.
 * Malloc overhead is not taken into account (up to 60 bytes per entry)
 * Callback from show_bgp via tree walker.
 */

BGP_PRIVATE int
bgp_calc_memory_walker (rntype *rn, va_list ap)
{
    int i;

    bnettype *net	= rn2bnettype(rn);
    ulong *networks	= va_arg(ap, ulong *);
    ulong *paths	= va_arg(ap, ulong *);
    ulong *mpaths	= va_arg(ap, ulong *);
    ulong *memory	= va_arg(ap, ulong *);
    ulong *history	= va_arg(ap, ulong *);
    ulong *received	= va_arg(ap, ulong *);

    /*
     * Do not consider the entries having 0 path. They will be present
     * until removed by bgp_nettable_scan during garbage collection.
     */
    if (net->pathcount > 0) {  
	*networks += 1;
	*paths += net->pathcount;
	*mpaths += net->pathsmalloced;
	for (i = net->firstpath; i != BGP_NULL_INDEX; i = net->paths[i].next) {
	    if (net->paths[i].flags & BFLAG_HISTORY)
		*history += 1;
	    if (net->paths[i].flags & BFLAG_RCVD_ONLY)
		*received += 1;
	}
    }
    *memory += get_block_size(net, NULL);

    return WALK_CONTINUE;
}

/*
 * bgp_show_network_detail
 * Show a single network entry with more detail.
 */

BGP_PRIVATE void
bgp_show_network_detail (bnettype *net, char *path_buffer)
{
    int i, j, count;
    bpathtype *path;
    battrtype *attr;
    uint bits;
    ulong gwmetric, reuse_time;
    bdampinfotype *dampinfo;
    char time[10];

    reuse_time = 0;
    automore_enable(NULL);
    mem_lock(net);
    printf("BGP routing table entry for %i%m, version %u"
	   "\nPaths: (%d available, ",
	   net->network, net->mask, net->version, net->pathcount);
    if (net->best != BGP_NO_BEST) {
 	j = 0;
 	for (i = net->firstpath; i != BGP_NULL_INDEX; i = net->paths[i].next) {
 	    j++;
 	    if (i == net->best)
 		break;
 	}
 	printf("best #%d", j);
	if (net->paths[net->best].flags & BFLAG_NOADV)
	    printf(", not advertised to any peer");
 	else if (net->paths[net->best].flags & BFLAG_NOEXPORT)	
 	    printf(", not advertised to EBGP peer");
	else if (net->paths[net->best].flags & BFLAG_LOCALAS)
	    printf(", not advertised outside local AS");
    } else {
 	printf("no best path");
    }
    bits = net->flags & (BNET_SENT_IBGP | BNET_SENT_EBGP | BNET_SENT_CLIENT);
    if (bits)
	printf(", advertised over ");
    if (bits & (BNET_SENT_IBGP | BNET_SENT_CLIENT)) 
	printf("IBGP%s", (bits & BNET_SENT_EBGP) ? ", " : NULL);
    if (bits & BNET_SENT_EBGP)
	printf("EBGP");
    printf(")");
    if (net->flags & BNET_SUPPRESSED)
	printf("\nAdvertisements of this net are suppressed by an aggregate.");
    for (i = net->firstpath; i != BGP_NULL_INDEX; i = path->next) {
	path = &net->paths[i];
	attr = path->attr;
	mem_lock(attr);
	if (attr->aslength)
	    printf("\n  %s",
		   bgp_path2string(attr->as_path, attr->aslength,
				   path_buffer, ASCII_PATHBUFSIZE));
	else
	    printf ("\n  Local");

	if (path->flags & BFLAG_AGGREGATOR)
	    printf(", (aggregated by %u %i)",
		   attr->aggregator.as, attr->aggregator.ip);

	if (path->flags & BFLAG_SUPPRESSED)
	    printf(", (suppressed due to dampening)");
	if (path->flags & BFLAG_HISTORY)
	    printf(" (history entry)");

	if (path->flags & BFLAG_CLIENT)
	    printf(", (Received from a RR-client)");

	if (path->flags & BFLAG_RCVD_ONLY)
	    printf(", (received-only)");
	if (path->flags & BFLAG_RCVD_USED)
	    printf(", (received & used)");

	printf("\n    %i", path->gateway);
	gwmetric = bgp_best_metric(path->gateway);
	if (gwmetric) {
	    if (gwmetric < METRIC_INACCESSIBLE)
		printf(" (metric %u)", gwmetric);
	    else
		printf(" (inaccessible)");
	}

	if (path->neighbor) {
	    printf(" from %i", path->neighbor);
	    if (path->neighbor != path->router_id)
		printf(" (%i)", path->router_id);
	}

	printf("\n      Origin %s", bgp_originstring(attr->origin, TRUE));
        if (path->flags & BFLAG_METRIC)
            printf(", metric %u", attr->metric);
	if (path->flags & BFLAG_MEDLOSER)
	    printf(" (not lowest)");
        if (path->flags & BFLAG_LOCALPREF)
            printf(", localpref %u", attr->localpref);
	if (path->weight)
	    printf(", weight %u", path->weight);
	if (path->flags & BFLAG_VALID)
	    printf(", valid");
	if (path->flags & (BFLAG_SOURCED|BFLAG_AGGREGATE)) {
	    if (path->flags & BFLAG_SOURCED)
		printf(", sourced");
	    if (path->flags & BFLAG_AGGREGATE)
		printf(", aggregated");
	    if (path->flags & BFLAG_LOCAL)
		printf(", local");
	} else 
	    printf(", %s%sternal",
		   (path->flags & BFLAG_CONFED) ? "confed-" : "",
		   (path->flags & BFLAG_INTERNAL) ? "in" : "ex");
	if ((path->flags & BFLAG_INTERNAL) && (path->flags & BFLAG_SYNCED))
	    printf(", synchronized");
	if (path->flags & BFLAG_ATOMICAGG)
	    printf(", atomic-aggregate");
	if (i == net->best)
	    printf(", best");
	if (path->flags & BFLAG_MULTIPATH)
	    printf(", multipath");
 	if (path->flags & BFLAG_DESTPREF)
 	    printf("\n      Destination preference %u, set by AS %d",
 		   attr->destpref, attr->destpref_as);
	if (attr->comcount) {
	    printf("\n      Community: ");
	    ip_community_print_array(attr->community, attr->comcount);
	}
	if (attr->rrinfo) {
	    printf("\n      Originator : %i", attr->rrinfo->originator);
	    if (attr->rrinfo->clusterlength) {
		count = btol(attr->rrinfo->clusterlength);
		printf(", Cluster list: ");
		for (j = 0; j < count; ++j) {
		    if (j > 0)
			printf(", ");
		    printf("%i", attr->rrinfo->clusterlist[j]);
		}
		}
	    }
	if (bgp_dampening_enabled && 
	    path->dampinfo) {
	    dampinfo = path->dampinfo;
	    bgp_dinfo_lock(dampinfo);
	    bgp_decay_penalty(dampinfo);
	    if (path->flags & BFLAG_SUPPRESSED)
		reuse_time = bgp_get_reuseticks(dampinfo) * BD_REUSE_INTERVAL;

	    printf("\n      Dampinfo: penalty %d, flapped %d times in %8TE", 
		   dampinfo->penalty, dampinfo->flap_count, 
		   dampinfo->start_time); 
	    if (path->flags & BFLAG_SUPPRESSED) {
		sprint_ticks_in_dhms(time, reuse_time);
		printf(", reuse in %8s", time);
	    }
	    
	    bgp_dinfo_free(dampinfo);
	}
	free(attr);
    }

    free(net);
    automore_disable();
}

/*
 * bgp_show_paths
 * Show the BGP path table.
 */

BGP_PUBLIC void
bgp_show_paths (parseinfo *csb)
{
    int i;
    battrtype *attr, *attr_next;
    regexp *pattern;
    char *path_buffer, *match_path_buffer, *pathstr;

    static const char header[] =
	"Address    Hash Refcount Metric Path";

    if (!pdb_BGP || !pdb_BGP->running) {
	return;
    }

    attr_next = NULL;
    path_buffer = malloc(ASCII_PATHBUFSIZE);
    match_path_buffer = malloc(ASCII_PATHBUFSIZE);
    if (!path_buffer || !match_path_buffer) {
	printf(nomemory);
	return;
    }
    
    automore_enable(header);
    pattern = regcomp(GETOBJ(string, 1));
    attr = NULL;
    for (i = 0; i < NETHASHLEN; i++) {

	if (automore_quit())
	    goto punt;


	for (attr = bgppathtable[i]; attr; attr = attr_next) {
	    if (pattern) {
		pathstr = bgp_path2string(attr->as_path, attr->aslength,
					  match_path_buffer,ASCII_PATHBUFSIZE);
		if (!pathstr || (!regexec(pattern, pathstr))) {
		    attr_next = attr->next;
		    continue;
		}
	    }

	    mem_lock(attr);
	    /*
	     * In calculating the reference count, subtract off 1 for the
	     * lock we just installed and 1 for the link in the chain.
	     */
	    printf("\n%#10x %-4d %-8d %-6d ", attr, i,
		   mem_refcount(attr) - 2, attr->metric);
	    if (attr->aslength)
		printf("%s ", bgp_path2string(attr->as_path, attr->aslength,
					      path_buffer, ASCII_PATHBUFSIZE));
	    printc(bgp_originchar(attr->origin));
	    attr_next = attr->next;
	    free(attr);
	}
    }
  punt:
    automore_disable();
    free(path_buffer);
}

/*
 * bgp_show_nets
 * Display BGP networks matching address/mask
 */

BGP_PRIVATE void
bgp_show_nets (ipaddrtype address, ipaddrtype mask, char *path_buffer,
	       boolean flap_stat)
{
    rntype *rn;
    ipradixkeytype net_key, mask_key;
    boolean onceflag;
    
    onceflag = FALSE;
    rn = rn_search_m(ipaddr2radixkey(address, &net_key),
		     bgp_head->rnh_treetop,
		     ipaddr2radixkey(mask, &mask_key));

    rn_walktree_blocking(rn, bgp_show_nets_walker, path_buffer,
			 &onceflag, address, mask, flap_stat);
}

/*
 * bgp_show_community_walker
 * Show function for 'show ip bgp community ... <exact-match>'.
 * Callback from bgp_show via tree walker.
 */

BGP_PRIVATE int
bgp_show_community_walker (rntype *rn, va_list ap)
{  
    bnettype *net	= rn2bnettype(rn);
    ulong *community	= va_arg(ap, ulong *);
    uchar comcount	= va_arg(ap, uchar);
    boolean exact	= va_arg(ap, boolean);
    char *path_buffer	= va_arg(ap, char *);
    boolean *onceflag	= va_arg(ap, boolean *);

    bpathtype *path;
    int i;
    battrtype *attr;
    char *pathstr;
    boolean print_net_addr;

    print_net_addr = TRUE;

    /*
     * Call sorted_array_partial_compare or bcmp to do the checking.
     *
     * If no community was specified, display all the paths with any
     * community
     */
    for (i = net->firstpath; i != BGP_NULL_INDEX; i = path->next) {
	path = &net->paths[i];
	if (path->flags & BFLAG_RCVD_ONLY)
	    continue;

	attr = path->attr;
	if (comcount)
	    if (exact) {
		if ((comcount != attr->comcount) ||
		    bcmp(community, attr->community, COMMUNITY_LEN(comcount)))
		    continue;
	    } else {
		if (!sorted_array_partial_compare(community, comcount,
						  attr->community,
						  attr->comcount))
		    continue;	
	    }
	else
	    if (!attr->comcount)		/* no community */
		continue;

	mem_lock(attr);
	pathstr = bgp_path2string(attr->as_path, attr->aslength, path_buffer,
				  ASCII_PATHBUFSIZE);
	bgp_process_onceflag(onceflag);
	bgp_print_entry(net, path, attr, pathstr, i, print_net_addr);	
	print_net_addr = FALSE;
	free(attr);
    }
    
    process_may_suspend();

    if (automore_quit())
	return WALK_ABORT;
    else	
	return WALK_CONTINUE;
}

/*
 * bgp_show_communitylist_walker
 * Show function for 'show ip bgp community-list <x> exact-match'.
 * Callback from bgp_show via tree walker.
 */

BGP_PRIVATE int
bgp_show_communitylist_walker (rntype *rn, va_list ap)
{  
    bnettype *net	= rn2bnettype(rn);
    int	comlist		= va_arg(ap, int);
    boolean exact	= va_arg(ap, boolean);
    char *path_buffer	= va_arg(ap, char *);
    boolean *onceflag	= va_arg(ap, boolean *);

    bpathtype *path;
    int i;
    battrtype *attr;
    char *pathstr;
    boolean print_net_addr;

    print_net_addr = TRUE;
    /*
     * Call commuity_check to do the checking.
     *
     */
    for (i = net->firstpath; i != BGP_NULL_INDEX; i = path->next) {
	path = &net->paths[i];
	if (path->flags & BFLAG_RCVD_ONLY)
	    continue;

	attr = path->attr;
	mem_lock(attr);
	
	if (!ip_community_check(attr->community, attr->comcount, comlist, 
				exact)) {
	    free(attr);
	    continue;
	}

	pathstr = bgp_path2string(attr->as_path, attr->aslength, path_buffer,
				  ASCII_PATHBUFSIZE);
	bgp_process_onceflag(onceflag);
	bgp_print_entry(net, path, attr, pathstr, i, print_net_addr);	
	print_net_addr = FALSE;
	free(attr);
    }
    
    process_may_suspend();
    if (automore_quit())
	return WALK_ABORT;
    else	
	return WALK_CONTINUE;
}

/*
 * bgp_show_dampinfo
 */

BGP_PRIVATE const char bgp_dampheader[] =
  "   Network          From             Reuse   Path";

BGP_PRIVATE void
bgp_show_dampinfo (bdampinfotype *dampinfo, ushort index,
		   char *path_buffer, boolean *onceflag)
{

    int len, i;
    ulong reuse_time;
    uchar time[10];
    bnettype *net;
    bpathtype *path;
    battrtype *attr;

    net = dampinfo->net;
    i = dampinfo->pindex;
    path = &net->paths[i];
    attr = path->attr;

    if (!net || !attr)
	return;
    mem_lock(net);
    mem_lock(attr);
    
    if (!*onceflag) {
	printf(bgp_show_prologue, bgptable_version, pdb_BGP->bgp_rtr_id);
	automore_header(bgp_dampheader);
	*onceflag = TRUE;
    }

    printf("\n%c%c%c",
	   ((net->flags & BNET_SUPPRESSED) ? 's' : 
	    ((path->flags & BFLAG_VALID) ? '*' : ' ')), 
	   ((i == net->best) ? '>' : 
	    ((path->flags & BFLAG_SUPPRESSED) ? 'd' : ' ')),
	   ((path->flags & BFLAG_INTERNAL) ? 'i' : ' '));

    len = printf("%i", net->network);
    if (net->mask != get_majormask(net->network))
	len += printf("/%d", ip_bitsinmask(net->mask));
    printf("%*c", 17-len, ' ');

    len = printf("%i", path->neighbor);
    printf("%*c", 17-len, ' ');

    /*
     * decay the penalty before calculating the reuse time
     */
    bgp_decay_penalty(dampinfo);
    reuse_time = bgp_get_reuseticks(dampinfo) * BD_REUSE_INTERVAL;
    sprint_ticks_in_dhms(time, reuse_time);
    printf("%7s ", time);

    if (attr->aslength) 
	printf("%s ", bgp_path2string(attr->as_path, attr->aslength,
				      path_buffer, ASCII_PATHBUFSIZE));

    printc(bgp_originchar(attr->origin));

    free(net);					/* unlock display locks */
    free(attr);
}

/*
 * bgp_show_dampened_routes
 */

BGP_PRIVATE void
bgp_show_dampened_routes (char *path_buffer, neighbortype *bgp)
{
    int index, curr_index, start_index, i;
    bdampinfotype *dampinfo, *next;
    bpathtype  *path;
    boolean onceflag;

    if (!bgp_dampening_enabled) {
	if (bgp_cfg_halflife_time || bgp_dampening_map)
	    printf("\n%% dampening reconfiguration in progress");
	else
	    printf("\n%% dampening not enabled.");
	return;
    }

    onceflag = FALSE;
    index = start_index = bgp_reuse_index;
    curr_index = 0;			/* used to terminate the loop */
    i = 1;

    /*
     * The bgp_reuse_index is a moving target. Make sure that i does
     * not exceed the array length (reuse array could have been
     * recreated due to reconfig during suspend). 
     */
    while ((curr_index != start_index) &&
	   (i < bgp_reuse_array_len)) {
	if (bgp_reuse_array[index]) {
	    dampinfo = bgp_reuse_array[index];
	    while (dampinfo && bgp_dampening_enabled) {
		bgp_dinfo_lock(dampinfo);
		path = &dampinfo->net->paths[dampinfo->pindex];

		if (!bgp || (bgp->address == path->neighbor))
		    bgp_show_dampinfo(dampinfo, index, path_buffer, &onceflag);

		next = dampinfo->next;
		bgp_dinfo_free(dampinfo);
		dampinfo = next;
	    }		
	}

	process_may_suspend();
	if (automore_quit() || !bgp_dampening_enabled)
	    return;

	curr_index = index = bgp_get_reuseindex(i);
	i++;
    }
}

/*
 * bgp_show_fstat
 */
BGP_PRIVATE void
bgp_show_fstat (bnettype *net, char *path_buffer, boolean *onceflag)
{
    bpathtype *path;
    int i;
    battrtype *attr;
    char *pathstr;
    boolean print_net_addr;
    
    print_net_addr = TRUE;
    for (i = net->firstpath; i != BGP_NULL_INDEX; i = path->next) {
	path = &net->paths[i];
	if (!bgp_dampening_enabled)
	    return;
	else if (!path->dampinfo ||
		 !path->dampinfo->flap_count)
	    continue;

	attr = path->attr;
	pathstr = bgp_path2string(attr->as_path, attr->aslength, path_buffer,
				  ASCII_PATHBUFSIZE);
	mem_lock(attr);
	bgp_dinfo_lock(path->dampinfo);
	bgp_process_fstat_onceflag(onceflag);
	bgp_print_fstat_entry(net, path, attr, pathstr, i, print_net_addr);
	bgp_dinfo_free(path->dampinfo);
	free(attr);
	print_net_addr = FALSE;
    }
}

/*
 * bgp_show_fstat_walker
 * Display flap statistics for all the entries that has dampinfo. 
 */
BGP_PRIVATE int
bgp_show_fstat_walker (rntype *rn, va_list ap)
{
    bnettype *net	= rn2bnettype(rn);
    char *path_buffer	= va_arg(ap, char *);
    boolean *onceflag	= va_arg(ap, boolean *);
    
    bgp_show_fstat(net, path_buffer, onceflag);

    process_may_suspend();
    if (automore_quit())
	return WALK_ABORT;
    else
	return WALK_CONTINUE;
}

/*
 * bgp_show
 * Display BGP data structures
 */

BGP_PUBLIC void
bgp_show (parseinfo *csb)
{
    bnettype *net;
    ipaddrtype address, mask;
    boolean onceflag, exact;
    regexp *pattern;
    int number, comcount;
    char *path_buffer;
    ulong *community, value;

    community = NULL;

    if (!pdb_BGP || !pdb_BGP->running) {
	printf(bgp_notrunning);
	return;
    }

    path_buffer = malloc(ASCII_PATHBUFSIZE);
    if (!path_buffer) {
	printf(nomemory);
	return;
    }
    
    automore_enable(NULL);
    onceflag = FALSE;
    number = 0;
    pattern = NULL;

    switch (csb->which) {
    case BGP_ADDRESS:				/* address is specified */
	address = GETOBJ(paddr,1)->ip_addr;
	mask = 0L;				/* do longest match */
	
	if (GETOBJ(paddr,2)->type == ADDR_IP)
	    mask = GETOBJ(paddr,2)->ip_addr;
	net = bgp_netlookup(address, mask);
	if (!net) {
	    printf("\n%% Network not in table");
	    break;
	}
	if (GETOBJ(int, 2))			/* flap statistics */
	    bgp_show_fstat(net, path_buffer, &onceflag);
	else
	    bgp_show_network_detail(net, path_buffer);
	break;

    case BGP_ADDRESS_SUBNETS:			/* address is specified */
	address = GETOBJ(paddr,1)->ip_addr;
	mask = GETOBJ(paddr,2)->ip_addr;
	address &= mask;
	bgp_show_nets(address, mask, path_buffer, GETOBJ(int, 2));
	break;
    
    case BGP_LIST:				/* access-list specified */
	number = GETOBJ(int,1);
	if (!as_path_accesslists[number]) {
	    printf("\n%% %u is not a valid AS-path access-list number",
		   number);
	    break;
	}
	rn_walktree_blocking(bgp_head->rnh_treetop,
			     bgp_show_filter_list_walker, path_buffer, 
			     &onceflag, number, GETOBJ(int, 2));
	break;

    case BGP_REGEXP:				/* regexp specified */
	pattern = regcomp(GETOBJ(string,1));
	if (pattern)
	    rn_walktree_blocking(bgp_head->rnh_treetop, bgp_show_regexp_walker,
				 path_buffer, &onceflag, pattern, 
				 GETOBJ(int,2));
	else
	    printf("\n%% Invalid regular expression");
	break;

    case BGP_CIDR:				/* show ip bgp cidr */
	rn_walktree_blocking(bgp_head->rnh_treetop, bgp_show_walker,
			     path_buffer, &onceflag, TRUE);
	break;

    case BGP_BOGUS:
	rn_walktree_blocking(bgp_head->rnh_treetop,
			     bgp_show_inconsistent_walker,
			     path_buffer, &onceflag);
	break;

    case BGP_COMMUNITY:
	community = malloc(COMMUNITY_LEN(BGP_SHOW_COMCOUNT));
	if (!community)
	    goto abort;
	comcount = 0;		/* very ugly. Need to fix the parser. */

	if (new_community_format) {
	    if (GETOBJ(int, 1)) {
		value = (GETOBJ(int,1) << 16) + GETOBJ(int,2);
		sorted_array_add(community, &comcount, value);
	    }
	    if (GETOBJ(int, 3)) {
		value = (GETOBJ(int,3) << 16) + GETOBJ(int,4);
		sorted_array_add(community, &comcount, value);
	    }
	    if (GETOBJ(int, 5)) {
		value = (GETOBJ(int,5) << 16) + GETOBJ(int,6);
		sorted_array_add(community, &comcount, value);
	    }
	    if (GETOBJ(int, 7)) {
		value = (GETOBJ(int,7) << 16) + GETOBJ(int,8);
		sorted_array_add(community, &comcount, value);
	    }
	    if (GETOBJ(int, 9)) {
		value = (GETOBJ(int,9) << 16) + GETOBJ(int,10);
		sorted_array_add(community, &comcount, value);
	    }
	} else {
	    if (GETOBJ(int, 1))
		sorted_array_add(community, &comcount, GETOBJ(int, 1));
	    if (GETOBJ(int, 2))
		sorted_array_add(community, &comcount, GETOBJ(int, 2));
	    if (GETOBJ(int, 3))
		sorted_array_add(community, &comcount, GETOBJ(int, 3));
	    if (GETOBJ(int, 4))
		sorted_array_add(community, &comcount, GETOBJ(int, 4));
	    if (GETOBJ(int, 5))
		sorted_array_add(community, &comcount, GETOBJ(int, 5));
	}

	if (GETOBJ(int, 11))
	    sorted_array_add(community, &comcount, GETOBJ(int, 11));
	if (GETOBJ(int, 12))
	    sorted_array_add(community, &comcount, GETOBJ(int, 12));
	if (GETOBJ(int, 13))
	    sorted_array_add(community, &comcount, GETOBJ(int, 13));

	exact = GETOBJ(int, 14);
	rn_walktree_blocking(bgp_head->rnh_treetop, 
			     bgp_show_community_walker,
			     community, comcount, exact, path_buffer, 
			     &onceflag);
	break;

    case BGP_COMMUNITY_LIST:
	number = GETOBJ(int, 1);
	exact  = GETOBJ(int, 2);
	rn_walktree_blocking(bgp_head->rnh_treetop, 
			     bgp_show_communitylist_walker,
			     number, exact, path_buffer, &onceflag);
	break;

    case BGP_DAMPENED_ROUTES:
	bgp_show_dampened_routes(path_buffer, NULL);
	break;
	
    case BGP_FSTAT:
	rn_walktree_blocking(bgp_head->rnh_treetop, bgp_show_fstat_walker,
			     path_buffer, &onceflag);
	break;

    case BGP_DEFAULT:				/* just 'show bgp' */
	rn_walktree_blocking(bgp_head->rnh_treetop, bgp_show_walker,
			     path_buffer, &onceflag, FALSE);
	break;
    }

 abort:
    automore_disable();
    free(path_buffer);
    free(community);
    free(pattern);
}

/*
 * bgp_show_neighbor_paths_walker
 * For a node in the radix tree, if path is from given neighbor then display
 * Called via the tree walker by bgp_show_neighbor_paths
 */

BGP_PRIVATE int
bgp_show_neighbor_paths_walker (rntype *rn, va_list ap)
{
    bnettype *net	= rn2bnettype(rn);
    char *path_buffer	= va_arg(ap, char *);
    char *match_path_buffer = va_arg(ap, char *);
    ipaddrtype address	= va_arg(ap, ipaddrtype);
    regexp *pattern	= va_arg(ap, regexp *);
    attrlist_entrytype **scanned_pathtable = va_arg(ap, attrlist_entrytype **);
    boolean *onceflag	= va_arg(ap, boolean *);

    bpathtype *path;
    battrtype *attr;
    uchar *aspath, *pathstr;
    attrlist_entrytype *curr;
    int i, j, hashvalue;

    for (i = net->firstpath; i != BGP_NULL_INDEX; i = path->next) {
	path = &net->paths[i];
	if (path->flags & BFLAG_RCVD_ONLY)
	    continue;

	attr = path->attr;
	mem_lock(attr);
	aspath = attr->as_path;
 	if (path->neighbor == address) {
	    if (pattern) {
		pathstr = bgp_path2string(attr->as_path, attr->aslength,
					  match_path_buffer,ASCII_PATHBUFSIZE);
		if (!pathstr || (!regexec(pattern, pathstr))) {
		    free(attr);
		    continue;
		}
	    }

	    /* compute a hash value for the attribute pointer */
	    /* stolen from bgp_find_attr */
	    hashvalue = attr->origin ^ attr->metric;
	    for (j = attr->aslength/2; j; j--) {       
		hashvalue ^= GETSHORT(aspath);
		aspath += PATHSIZE;
	    }
	    hashvalue = nethash(hashvalue); 

	    /* Check if the current entry is already displayed */
	    for (curr = scanned_pathtable[hashvalue]; curr; curr = curr->next)
		if (curr->attr == attr)
		    break;

	    if (curr) {		/* already displayed this path */
		free(attr);
		continue;
	    }

	    /* store this attribute in a hash table so we don't redisplay */
	    curr = malloc(sizeof(attrlist_entrytype));
	    if (!curr) {
		free(attr);
		free(net);
		return WALK_ABORT;
	    }
	    curr->attr = attr;
	    curr->next = scanned_pathtable[hashvalue];
	    scanned_pathtable[hashvalue] = curr;

	    if (!*onceflag) {
		automore_enable("Address    Refcount Metric Path");
		*onceflag = TRUE;
	    }

	    printf("\n%#10x %-8d %-6d ", attr,
		   mem_refcount(attr) - 2, attr->metric);

	    if (attr->aslength)
		printf("%s ", bgp_path2string(attr->as_path, attr->aslength,
					      path_buffer, ASCII_PATHBUFSIZE));

	    printc(bgp_originchar(attr->origin));
	}
	free(attr);
    }

    process_may_suspend();
    if (automore_quit())
	return WALK_ABORT;
    else
	return WALK_CONTINUE;
}

/*
 * bgp_show_neighbor_paths
 * Show the paths learned from a particular BGP neighbor
 */

BGP_PUBLIC void
bgp_show_neighbor_paths (parseinfo *csb)
{
    ipaddrtype address;
    neighbortype *bgp;
    int i;
    boolean onceflag;
    attrlist_entrytype **scanned_pathtable, *entry, *next;
    char *path_buffer, *match_path_buffer;
    regexp *pattern;

    if (!pdb_BGP || !pdb_BGP->running) {
    	printf(bgp_notrunning);
	return;
    }

    path_buffer = malloc(ASCII_PATHBUFSIZE);
    match_path_buffer = malloc(ASCII_PATHBUFSIZE);
    if (!path_buffer || !match_path_buffer) {
	printf(nomemory);
	return;
    }

    pattern = regcomp(GETOBJ(string, 1));
    address = GETOBJ(paddr, 1)->ip_addr;      /* Check if neighbhor exists */

    for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next)
	if (bgp->address == address)
	    break;

    if (!bgp) {
        printf("\n%% No such neighbor");
        return;
    }

    scanned_pathtable = malloc(NETHASHLEN * sizeof(attrlist_entrytype *));
    if (!scanned_pathtable) {
	printf(nomemory);
	return;
    }

    onceflag = FALSE;

    rn_walktree_blocking(bgp_head->rnh_treetop, bgp_show_neighbor_paths_walker,
			 path_buffer, match_path_buffer, address, pattern, 
			 scanned_pathtable, &onceflag);

    automore_disable();

    /*
     * Free the hash entries we created
     */
    for (i = 0; i < NETHASHLEN; i++) {
	for (entry = scanned_pathtable[i]; entry; entry = next) {
	    next = entry->next;
	    entry->next = NULL;
	    free(entry);
	}
    }

    free(scanned_pathtable);
    free(path_buffer);
}

/*
 * bgp_display_summary_header
 * Display header information for show summary
 */

BGP_PUBLIC void
bgp_display_summary_header (void)
{
    ulong networks, paths, memory, attr_entries, attr_memory, history;
    ulong rcache_entries, fcache_entries, mpaths, received;
    bgp_route_map_ctype *rcache;
    bgp_filter_list_ctype *fcache;
    battrtype *attr;
    bdampinfotype *dampinfo;
    int i, j;
    j = 0;

    printf("BGP table version is %u, main routing table version %u\n",
	   bgptable_version, bgp_nettable_version);

    networks = paths = mpaths = memory = history = received = 0;
    rn_walktree_timed(bgp_head, bgp_calc_memory_walker, ip_radix_successor,
		      &networks, &paths, &mpaths, &memory, &history,
		      &received);

/*
 * This code is in here just for checking the performance of the radix
 * trie as Keith and I play games.  It can safely be removed if you want,
 * but I'd like to keep it even though it's evil and ugly. -- pst 16oct94
 */
#ifdef	RADIX_STATS
    if (bgp_debug) {
	int leaves, maxlevel, avglevel, dupes, maxdupelen;
	extern void radix_stats(struct radix_node_head *,
				int *, int *, int *, int *, int *);
	radix_stats(bgp_head,
		    &leaves, &maxlevel, &avglevel, &dupes, &maxdupelen);
	printf("%u leaves, %u maximum depth, %u average depth, %u dupes, "
	       "%u maximum dupe len\n", leaves, maxlevel, avglevel, dupes,
					maxdupelen);
    }
#endif

    /*
     * Do not consider the entries with refcount = 1. They will be present
     * until removed by bgp_nettable_scan during garbage collection.
     */
    attr_entries = attr_memory = 0;
    for (i = 0; i < NETHASHLEN; i++)
	for (attr = bgppathtable[i]; attr; attr = attr->next) {
	    if (mem_refcount(attr) > 1)
		attr_entries++;
	    attr_memory += get_block_size(attr, NULL);
	}
    
    rcache_entries = 0;
    for (i = 0; i < NETHASHLEN; i++)
	for (rcache = bgp_route_map_cache[i]; rcache; rcache = rcache->next)
	    rcache_entries++;

    fcache_entries = 0;
    for (i = 0; i < NETHASHLEN; i++)
	for (fcache = bgp_filter_list_cache[i]; fcache;fcache = fcache->next)
	    fcache_entries++;

    if (networks) {
	printf("%lu network entries (%lu/%lu paths) using %lu bytes of "
	       "memory\n", networks, paths, mpaths, memory);
	printf("%lu BGP path attribute entries using %lu bytes of memory\n",
	       attr_entries, attr_memory);
	printf("%lu BGP route-map cache entries using %lu bytes of memory\n",
	       rcache_entries, (rcache_entries * sizeof(bgp_route_map_ctype)));
	printf("%lu BGP filter-list cache entries using %lu bytes of memory\n",
	       fcache_entries, (fcache_entries*sizeof(bgp_filter_list_ctype)));
    
	if (bgp_dampening_enabled) {
	    for (i = 0; i < bgp_reuse_array_len; ++i) {
		dampinfo = bgp_reuse_array[i];
		for (; dampinfo; dampinfo = dampinfo->next)
		    ++j;
	    }
	    printf("Dampening enabled. %u history paths, %u dampened paths\n", 
		   history, j);
	} 
	if (received)
	    printf("%lu received paths for inbound soft reconfiguration\n",
		   received);
    }
}

/*
 * bgp_display_summary_entry
 */

BGP_PRIVATE void
bgp_display_summary_entry (neighbortype *bgp, boolean firsttime)
{
    int version;
    char time[40];
    static const char bgp_summary_header [] =
"Neighbor        V    AS MsgRcvd MsgSent   TblVer  InQ OutQ Up/Down  State";

    if (firsttime) {
	bgp_display_summary_header();
	automore_header(bgp_summary_header);
    }

    sprint_dhms(time, bgp->started);
    version = bgp->protocol_version;
    if (bgp->bgp_state < BGPS_OPENSENT && bgp->bgp_configed_version)
	version = bgp->bgp_configed_version;
    printf("\n%15i %1d %-5u %-7u %-7u %-8u %-4u %-4u %8s %s",
	   bgp->address, version, bgp->yoursystem,
	   bgp->inmsgs, bgp->outmsgs, bgp->version,
	   QUEUESIZE(&bgp->bgp_readQ), QUEUESIZE(&bgp->bgp_writeQ), time,
	   bgp->bgp_state == BGPS_ESTAB ? "" : 
	   bgp_statestring(bgp->bgp_state));
}
/*
 * bgp_show_pgrp_summary
 * Show summary of peer-group member status
 */
BGP_PUBLIC void
bgp_show_pgrp_summary (parseinfo *csb)
{
    neighbortype *bgp, *bgp_next, *pgrp;
    uchar *nametag;
    boolean firsttime;

    if (!pdb_BGP || !pdb_BGP->running) {
	printf(bgp_notrunning);
	return;
    }
    if (!GETOBJ(int, 1))
	return;

    nametag = GETOBJ(string, 1);
    for (pgrp = pdb_BGP->peergroup_list; pgrp; pgrp = pgrp->next)
	if (!strcmp(nametag, pgrp->neighbor_name))
	    break;
    if (!pgrp) {
	printf("\n%% No such peer-group");
	return;
    }

    mem_lock(pgrp);
    automore_enable(NULL);
    firsttime = TRUE;
    for (bgp = pgrp->peerhead; bgp; bgp = bgp_next) {
	mem_lock(bgp);
	bgp_display_summary_entry(bgp, firsttime);
	firsttime = FALSE;
	bgp_next = bgp->nextpeer;
	free(bgp);	/* un-mem_lock() */
    }
    free(pgrp);
    automore_disable();
}

/*
 * bgp_show_summary
 * Show a summary of BGP neighbor status.
 */

BGP_PUBLIC void
bgp_show_summary (parseinfo *csb)
{
    neighbortype *bgp, *bgp_next;
    boolean firsttime;

    if (!pdb_BGP || !pdb_BGP->running) {
	printf(bgp_notrunning);
	return;
    }

    automore_enable(NULL);
    firsttime = TRUE;
    for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp_next) {
	mem_lock(bgp);
	bgp_display_summary_entry(bgp, firsttime);
	firsttime = FALSE;
	bgp_next = bgp->next;
	free(bgp);	/* un-mem_lock() */
    }
    automore_disable();
}

/*
 * bgp_show_single_neighbor
 * Show all info about a single neighbor or peergroup
 * Assumes neighbor is already locked.
 * Return TRUE to continue, false to stop.
 */

BGP_PRIVATE boolean
bgp_show_single_neighbor (neighbortype *bgp)
{
    bweightinfo *weightexp;
    boolean peergroup;
    bgp_cond_adv_maptype *camap;

    peergroup = bgp->flags & BN_PEERGROUP;
    
    if (peergroup)
	printf("\nBGP neighbor is %s, peer-group leader", bgp->neighbor_name);
    else
	printf("\nBGP neighbor is %i", bgp->address);
    if (bgp->yoursystem)
	printf(",  remote AS %d", bgp->yoursystem);
    if (!peergroup)
	printf(", %sternal link", bgp->bgp_internal ? "in" : "ex"); 
    printf("\n Index %d, Offset %d, Mask 0x%x", bgp->neighbor_index,
	   bgp->neighbor_index_offset, bgp->neighbor_index_mask);
    if (bgp->flags & BN_TEMPORARY)
	printf(", temporary");
    if (bgp->flags & BN_RR_CLIENT)
	printf("\n  Route-Reflector Client");
    if (bgp->peergroup)
	printf("\n  %s peer-group member", bgp->peergroup->neighbor_name);
    if (bgp->flags & BN_SOFT_RECONFIG_INOK)
	printf("\n  Inbound soft reconfiguration allowed");
    if (bgp->flags & BN_REMOVE_PRIVATE_AS)
	printf("\n  Private AS number removed from updates to this neighbor");
    if (bgp->flags & BN_NOHOPCALC)
	printf("\n  NEXT_HOP is always this router");
    if (bgp->flags & BN_COMMUNITYOK)
	printf("\n  Community attribute sent to this neighbor");
    printf("\n  BGP version %d", bgp->protocol_version);
    if (!peergroup) {
	if (bgp->bgp_configed_version &&
	    bgp->bgp_configed_version != bgp->protocol_version)
	    printf(", configured version %d", bgp->bgp_configed_version);
	if (bgp->protocol_version > 2)
	    printf(", remote router ID %i", bgp->bgp_router_id);
	if (bgp->flags & BN_COMMONADMIN)
	    printf("\n  Neighbor under common administration");
	printf("\n  BGP state = %s, table version = %u",
	       bgp_statestring(bgp->bgp_state), bgp->version);
	if (bgp->bgp_state == BGPS_ESTAB) {
	    printf(", up for ");
	    print_dhms(bgp->started);
	}
	if (bgp->flags & BN_ALLOCATE_INDEX)
	    printf("\n  Scheduled for new neighbor index allocation");
	if (bgp->flags & BN_RECLAIM_INDEX)
	    printf("\n  Scheduled for reclaiming index");
	printf("\n  Last read ");
	print_dhms(bgp->bgp_lastread);
	printf(", hold time is %d, keepalive interval is %d seconds",
	       bgp->bgp_holdtime/ONESEC, bgp->bgp_probetime/ONESEC);
    }
    if (bgp->flags & BN_DEFAULT_ORIGINATE) {
	printf("\n  Default information originate");
	if (bgp->bgp_default_map)
	    printf(", default route-map %s", bgp->bgp_default_map->mapname);
	printf(", default %ssent", 
	       (bgp->bgp_default_state == BGP_DEFAULT_SENT) ? "" : "not ");
    }
    printf("\n  Minimum time between advertisement runs is %d seconds",
	   bgp->bgp_minadverttime/ONESEC);
    if (!peergroup) {
	printf("\n  Received %d messages, %d notifications, %d in queue",
	       bgp->inmsgs, bgp->errsrcvd, QUEUESIZE(&bgp->bgp_readQ));
	printf("\n  Sent %d messages, %d notifications, %d in queue",
	       bgp->outmsgs, bgp->errssent, QUEUESIZE(&bgp->bgp_writeQ));
    }
    if (bgp->bgp_weight != BGP_LEARNED_WEIGHT)
	printf("\n  Default weight %u", bgp->bgp_weight);
    if (bgp->flags & BN_INBOUND_PATHPOLICY)
	printf("\n  Inbound path policy configured");
    if (bgp->flags & BN_OUTBOUND_PATHPOLICY)
	printf("\n  Outbound path policy configured");
    if (bgp->bgp_distlistin)
	printf("\n  Incoming update network filter list is %s",
	       bgp->bgp_distlistin->name);
    if (bgp->bgp_distlistout)
	printf("\n  Outgoing update network filter list is %s",
	       bgp->bgp_distlistout->name);
    for (weightexp = (bweightinfo *) bgp->bgp_weightdata.qhead;
	 weightexp; weightexp = weightexp->next) {
	printf("\n  Incoming update AS path weight filter list %d,"
	       " weight %u", weightexp->access_list, weightexp->weight);
    }
    if (bgp->bgp_inlist)
	printf("\n  Incoming update AS path filter list is %d",
	       bgp->bgp_inlist);
    if (bgp->bgp_outlist)
	printf("\n  Outgoing update AS path filter list is %d",
	       bgp->bgp_outlist);
    if (bgp->bgp_routemapin)
	printf("\n  Route map for incoming advertisements is %s",
	       bgp->bgp_routemapin->mapname);
    if (bgp->bgp_routemapout)
	printf("\n  Route map for outgoing advertisements is %s",
	       bgp->bgp_routemapout->mapname);
    if (bgp->bgp_unsuppress_map)
	printf("\n  Route map for selective unsuppress is %s",
	       bgp->bgp_unsuppress_map->mapname);
    if (bgp->bgp_cond_adv)
	for (camap = bgp->bgp_cond_adv; camap; camap = camap->next)
	    printf("\n  Condition-map %s, Advertise-map %s, status: %s",
		   camap->bgp_condition_map->mapname,
		   camap->bgp_advertise_map->mapname,
		   bgp_adv_state_strings[camap->adv_state]);
    if (!peergroup) {
	printf("\n  Connections established %d; dropped %d",
	       bgp->cameup, bgp->wentdn);
	if (bgp->ebgp_ttl != BGP_DEFAULT_EBGP_TTL)
	    printf("\n  External BGP neighbor may be up to %d hops away.",
		   bgp->ebgp_ttl);
	else if (!bgp->bgp_internal && samecable(bgp->address) == NULL)
	    printf("\n  External BGP neighbor not directly connected.");
	if (!bgp->tcb)
	    printf("\n  No active TCP connection");
	else {
	    automore_conditional(0);
	    if (bgp->tcb)
		tcp_status(bgp->tcb);
	}
    }
    return (TRUE);
}

/*
 * bgp_show_pgrp
 * Display the peer-groups
 */

BGP_PUBLIC void
bgp_show_pgrp (parseinfo *csb)
{
    neighbortype *pgrp, *pgrp_next;
    uchar *nametag;

    if (!pdb_BGP || !pdb_BGP->running) {
	printf(bgp_notrunning);
	return;
    }
    
    automore_enable(NULL);
    nametag = NULL;
    if (GETOBJ(int, 1)) {			/* peer-group name entered */
	nametag = GETOBJ(string, 1);
	for (pgrp = pdb_BGP->peergroup_list; pgrp; pgrp = pgrp->next)
	    if (!strcmp(pgrp->neighbor_name, nametag))
		break;
	if (!pgrp) {
	    printf("\n%% No such peer-group");
	    automore_disable();
	    return;
	}
	mem_lock(pgrp);
	bgp_show_single_neighbor(pgrp);
	free(pgrp);
    } else {
	for (pgrp = pdb_BGP->peergroup_list; pgrp; pgrp = pgrp_next) {
	    mem_lock(pgrp);
	    bgp_show_single_neighbor(pgrp);
	    pgrp_next = pgrp->next;
	    free(pgrp);
	    if (pgrp_next) {
		automore_conditional(0);
		printf("\n");
	    }
	}
    }
    automore_disable();
}

/*
 * bgp_show_neighbors
 * Step through the BGP neighbor list
 */

BGP_PUBLIC void
bgp_show_neighbors (parseinfo *csb)
{
    neighbortype *bgp, *bgp_next;
    ipaddrtype address;
    boolean cont;

    if (!pdb_BGP || !pdb_BGP->running) {
	printf(bgp_notrunning);
	return;
    }
    
    automore_enable(NULL);
    address = GETOBJ(paddr,1)->ip_addr;
    if (GETOBJ(paddr,1)->type != ADDR_IP) {
	for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp_next) {
	    mem_lock(bgp);
	    cont = bgp_show_single_neighbor(bgp);
	    bgp_next = bgp->next;
	    free(bgp);	/* un-mem_lock() */

	    if (!cont)
		break;

	    if (bgp_next) {
		automore_conditional(0);
		printf("\n");
	    }
       	}
    } else {
	for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next) {
	    if (bgp->address == address)
		break;
	}
	if (!bgp) {
	    printf("\n%% No such neighbor");
	    automore_disable();
	    return;
	}
	mem_lock(bgp);
	bgp_show_single_neighbor(bgp);
	free(bgp);	/* un-mem_lock() */
    }
    automore_disable();
}

/*
 * bgp_show_neighbor_route_walker
 *
 * Show any routes for a particular network that have been learned
 * via a given neighbor.
 * Callback routine from tree walker.
 */

BGP_PRIVATE int
bgp_show_neighbor_route_walker (rntype *rn, va_list ap)
{
    bnettype *net	= rn2bnettype(rn);
    char *path_buffer	= va_arg(ap, char *);
    neighbortype *bgp	= va_arg(ap, neighbortype *);
    boolean *onceflag	= va_arg(ap, boolean *);
    boolean flap_stat	= va_arg(ap, boolean);
    boolean rcvd_routes = va_arg(ap, boolean);

    battrtype *attr;
    bpathtype *path;
    int j;
    char *pathstr;
    boolean print_net_addr;

    print_net_addr = TRUE;
    for (j = net->firstpath; j != BGP_NULL_INDEX; j = path->next) {
	path = &net->paths[j];
	if (bgp->address != path->neighbor)
	    continue;

	if (flap_stat) {
	    if (!bgp_dampening_enabled)
		return WALK_ABORT;
	    else if (!path->dampinfo ||
		     !path->dampinfo->flap_count)
		continue;
	}

 	if ((rcvd_routes && 
 	     !(path->flags & (BFLAG_RCVD_ONLY|BFLAG_RCVD_USED))) ||
 	    (!rcvd_routes && 
 	     (path->flags & BFLAG_RCVD_ONLY)))
  	    continue;

	attr = path->attr;
	pathstr = bgp_path2string(attr->as_path, attr->aslength, path_buffer,
				  ASCII_PATHBUFSIZE);
	mem_lock(attr);
	if (flap_stat) {
	    bgp_dinfo_lock(path->dampinfo);
	    bgp_process_fstat_onceflag(onceflag);
	    bgp_print_fstat_entry(net, path, attr, pathstr, j, print_net_addr);
	    bgp_dinfo_free(path->dampinfo);
	} else {
	    bgp_process_onceflag(onceflag);
	    bgp_print_entry(net, path, attr, pathstr, j, print_net_addr);
	}

	/*
	 * Do some consistancy checking for soft reconfig.
	 * I might ripe this out later.. 
	 */
	if (!flap_stat && 
	    (bgp->flags & BN_SOFT_RECONFIG_INOK)) {
	    if (path->flags & BFLAG_RCVD_ONLY) {
		if ((path->next != BGP_NULL_INDEX) &&
		    (net->paths[path->next].neighbor == bgp->address))
		    printf("\nBGP: Rcvd only has next path from neighbor");

	    } else if (path->flags & BFLAG_RCVD_USED) {
		if ((path->next != BGP_NULL_INDEX) &&
		    (net->paths[path->next].neighbor == bgp->address))
		    printf("\nBGP: Rcvd used has next path from neighbor");
		if ((path->prev != BGP_NULL_INDEX) &&
		    (net->paths[path->prev].neighbor == bgp->address))
		    printf("\nBGP: Rcvd used has prev path from neighbor");

	    } else {
		if (path->next == BGP_NULL_INDEX)
		    printf("\nBGP: Used path with no rcvd path");
		else if (net->paths[path->next].neighbor != bgp->address)
		    printf("\nBGP: Used path misses rcvd path");
		else if (!(net->paths[path->next].flags & BFLAG_RCVD_ONLY))
		    printf("\nBGP: Used path not followed by rcvd-only path");
	    }
	}
	print_net_addr = FALSE;
	free(attr);
    }

    process_may_suspend();
    if (automore_quit())
	return WALK_ABORT;
    else
	return WALK_CONTINUE;
}

/*
 * bgp_show_neighbor_advertised_routes_walker
 *
 * Show any routes for a particular network that have been advertised
 * to a given EBGP neighbor.
 * Callback routine from tree walker.
 */

BGP_PRIVATE int
bgp_show_neighbor_advertised_routes_walker (rntype *rn, va_list ap)
{
    bnettype *net	= rn2bnettype(rn);
    neighbortype *bgp	= va_arg(ap, neighbortype *);
    char *path_buffer	= va_arg(ap, char *);
    boolean *onceflag	= va_arg(ap, boolean *);

    battrtype *attr;
    bpathtype *path;
    char *pathstr;

    if ((!*onceflag) && (bgp->bgp_default_state == BGP_DEFAULT_SENT)) {
	printf(bgp_show_prologue, bgptable_version, pdb_BGP->bgp_rtr_id);
	printf("\nOriginating default network 0.0.0.0\n");
	automore_header(bgpheader);
	*onceflag = TRUE;
    }

    if (bgp_advertised(net, bgp)) {
	if (net->best == BGP_NO_BEST) {
	    process_may_suspend();
	    return WALK_CONTINUE;
	}
	bgp_process_onceflag(onceflag);
	path = &net->paths[net->best];
	attr = path->attr;
	pathstr = bgp_path2string(attr->as_path, attr->aslength,
				  path_buffer, ASCII_PATHBUFSIZE);
	mem_lock(attr);
	bgp_print_entry(net, path, attr, pathstr, net->best, TRUE);
	free(attr);
    }
	
    process_may_suspend();
    if (automore_quit())
	return WALK_ABORT;
    else
	return WALK_CONTINUE;
}

/*
 * bgp_show_neighbor_routes
 * Show the routes learned from a particular neighbor.
 */

BGP_PUBLIC void
bgp_show_neighbor_routes (parseinfo *csb)
{
    neighbortype *bgp;
    ipaddrtype address;
    boolean onceflag;
    char *path_buffer;

    if (!pdb_BGP || !pdb_BGP->running) {
	printf(bgp_notrunning);
	return;
    }

    path_buffer = malloc(ASCII_PATHBUFSIZE);
    if (!path_buffer) {
	printf(nomemory);
	return;
    }
    
    bgp = NULL;
    address = GETOBJ(paddr,1)->ip_addr;
    if (GETOBJ(paddr,1)->type == ADDR_IP) {
	for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next) {
	    if (bgp->address == address)
		break;
	}
    }
    if (!bgp) {
	printf("\n%% No such neighbor");
	return;
    }
    mem_lock(bgp);
    automore_enable(NULL);
    onceflag = FALSE;

    switch (csb->which) {
    case BGP_NEIGH_ROUTES :
	rn_walktree_blocking(bgp_head->rnh_treetop, 
			     bgp_show_neighbor_route_walker, path_buffer, 
			     bgp, &onceflag, FALSE, FALSE);
	break;

    case BGP_NEIGH_FSTAT :
	rn_walktree_blocking(bgp_head->rnh_treetop, 
			     bgp_show_neighbor_route_walker, path_buffer, 
			     bgp, &onceflag, TRUE, FALSE);
	break;

    case BGP_NEIGH_RCVDROUTES :
	if (!(bgp->flags & BN_SOFT_RECONFIG_INOK)) {
	    printf("\n%% Inbound soft reconfiguration not enabled");
	    return;
	}

	rn_walktree_blocking(bgp_head->rnh_treetop,
			     bgp_show_neighbor_route_walker, path_buffer,
			     bgp, &onceflag, FALSE, TRUE);
	break;

    case BGP_NEIGH_ADVROUTES:
	if (bgp->bgp_internal) {
	    printf("\n%% Command not supported for Internal neighbors");
	    return;
	}

	rn_walktree_blocking(bgp_head->rnh_treetop,
			     bgp_show_neighbor_advertised_routes_walker, bgp,
			     path_buffer, &onceflag);
	break;

    case BGP_NEIGH_DAMPROUTES:
	bgp_show_dampened_routes(path_buffer, bgp);
	break;
    }

    free(bgp);	/* un-mem_lock() */
    automore_disable();
    free(path_buffer);
}

/*
 * bgp4_pathattr_walker
 * Find the lexically next network and path given the prefix, the
 * prefix length, and neighbor address.  Called by tree walker routine.
 *
 * Sleaze alert:  This routine is supposed to return an int value
 * telling the treewalker routine whether to keep looking or not.
 * This routine ASSUMES that WALK_CONTINUE == 0 == NULL, and returns
 * WALK_CONTINUE if nothing found and the point to the found network
 * if something found.  This will break if ported to an architecture
 * where sizeof(int) != sizeof(pointer) or where NULL != 0.
 *
 * Returns pointer to network found or NULL if none found and sets
 * path_index to the index of the desired path in the network struct.  
 */
BGP_PRIVATE int 
bgp4_pathattr_walker (rntype *rn, va_list ap)
{
    bnettype *net	= rn2bnettype(rn);
    ipaddrtype prefix	= va_arg(ap, ipaddrtype);
    ipaddrtype mask	= PREFIX2MASK(va_arg(ap, ulong));
    ipaddrtype peer_addr= va_arg(ap, ipaddrtype);
    int *path_index	= va_arg(ap, int *);

    bnettype *tmp_net = NULL;
    bnettype *last_net = NULL;
    ulong best_neighbor = MAXULONG;
    int idx;
    /*
     * First, test the ip address prefix.  Should be >= target prefix.
     */
    if (net->network < prefix)
	return WALK_CONTINUE;

    /* 
     * If we have upped the prefix, reset mask and peer_addr to 0.
     */
    if (net->network > prefix) {
	mask = 0;
	peer_addr = 0;
    }

    /*
     * Okay, now test prefix length.  Remember that prefixes are
     * sorted in a linked list, from longest to shortest.  If the
     * first prefix_len is less than the target then we are done with
     * this ip address prefix and need to move on to next prefix.
     */
    if (net->mask < mask)
	return WALK_CONTINUE;

    /*
     * Okay, we definitely have one valid prefix here, see if there
     * are others.  Find last one (the one with the least specific
     * mask).  Save the next to last prefix found in case all
     * neighbors in the last one found have been reported.
     */
    tmp_net = rn2bnettype(rn->rn_dupedkey);

    while (tmp_net && (tmp_net->mask >= mask)) {
	last_net = net;
	net = tmp_net;
	tmp_net = rn2bnettype(((rntype *)net)->rn_dupedkey);
    }

    /* 
     * Okay, got network and prefix that we may care about, see if we
     * can find an interesting neighbor address.  First time through
     * check net.  If there are no interesting neighbors there, back
     * up to the last network saved (last_net) and check there.
     */
    while (best_neighbor == MAXULONG) {
	for (idx = net->firstpath; idx != BGP_NULL_INDEX; 
	     idx = net->paths[idx].next) {
	    if (net->paths[idx].neighbor >= peer_addr) {
		if (net->paths[idx].neighbor < best_neighbor) {
		    best_neighbor = net->paths[idx].neighbor;
		    *path_index = idx;
		}
	    }
	}
	/*
	 * We may be done.  If we found an interesting neighbor
	 * (lexically next for this prefix and mask), then we are
	 * done.  Return the network.  If not, back up to last_net
	 * and try again. 
	 */
	if (best_neighbor != MAXULONG)
	    return (int)net;

	/*
	 * Not done.  See if we have a last_net still to check.
	 * If not, bail and check next prefix.
	 * If so, bump the peer_addr back to 0 and check it.
	 */
	if (!last_net || last_net == net)
	    return WALK_CONTINUE;

	net = last_net;
	peer_addr = 0;
    }

    return WALK_CONTINUE;		/* This should never be reached. */
}

/*
 * bgp4_find_pathattr
 * Find the lexically next network and path given the prefix, the
 * prefix length, and neighbor address.  bgp4_pathattr_walker does all
 * the work.  Casts the return from bgp4_pathattr_walker to a bnettype
 * and returns that.  It will be address of the found network or NULL.
 */

BGP_PUBLIC bnettype *
bgp4_find_pathattr (ipaddrtype prefix, ipaddrtype prefix_len,
		    ipaddrtype peer_addr, int *path_index)
{

    rntype *rn = NULL;
    ipradixkeytype key;

    /* we can call the unsafe routine because we're checking for root here */
    rn = rn_match_unsafe(ipaddr2radixkey(prefix, &key), bgp_head);

    if (rn == NULL)
	return(NULL);

    return (bnettype *) rn_walktree(rn, bgp4_pathattr_walker,
				    prefix, prefix_len,
				    peer_addr, path_index);
}

/*
 * bgp_as_parse
 * Parse the as keyword of a bgp neighbor command
 */

BGP_PRIVATE void
bgp_as_parse (parseinfo *csb, neighbortype *bgp)
{
    astype *as;
    ushort number;

    /*
     * Get routing domain (AS) number and see if we've seen it before
     */
    number = GETOBJ(int,1);
    for (as = (astype *) bgp->bgp_asdata.qhead; as; as = as->next) {
	if (as->number == number) {
	    if (!csb->sense) {
		unqueue(&bgp->bgp_asdata, as);
		free(as);
		return;
	    } else
		break;
	}
    }

    /*
     * If we didn't find this AS in our list, append a new entry
     */
    if (!as) {
	as = malloc(sizeof(astype));
	if (!as)
	    return;
	as->number = number;
	as->grant = TRUE;
	enqueue(&bgp->bgp_asdata, as);
	printf("\n%% filter-as is an obsolete subcommand, "
	       "use filter-list instead");
    }
    if (GETOBJ(int,2)) {	/* permit */
	as->grant = TRUE;
	as->weight = GETOBJ(int,3);
    } else {		/* deny */
	as->grant = FALSE; 
    }
}

/*
 * bgp_filter_parse
 * Parse a neighbor filter-list command.
 */

BGP_PRIVATE void
bgp_filter_parse (parseinfo *csb, neighbortype *bgp)
{
    int list;
    bweightinfo *weightexp;

    /*
     * Get the list number.
     */
    list = GETOBJ(int,1);

    /*
     * Check direction.
     */
    switch (GETOBJ(int,2)) {
      case IN:
	bgp->bgp_inlist =  csb->sense ? list : 0;
	return;

      case OUT:
	bgp->bgp_outlist = csb->sense ? list : 0;
	return;

      case NEIGHBOR_BGP_WEIGHT:
	for (weightexp = (bweightinfo *) bgp->bgp_weightdata.qhead;
	     weightexp; weightexp = weightexp->next)
	    if (weightexp->access_list == list)
		break;
	if (csb->sense) {
	    if (!weightexp) {
		weightexp = malloc(sizeof(bweightinfo));
		if (!weightexp)
		    return;
		weightexp->access_list = list;
		enqueue(&bgp->bgp_weightdata, weightexp);
	    }
	    weightexp->weight = GETOBJ(int,3);
	} else {
	    if (!weightexp)
		return;
	    unqueue(&bgp->bgp_weightdata, weightexp);
	    free(weightexp);
	}
	return;

      default:
	printf(badbgpneighbor);
	return;
    }
}

/*
 * bgp_init_tcp_listener
 * Create a TCP handle for accept incoming connections
 * from a given neighbor.
 */

BGP_PRIVATE int
bgp_init_tcp_listener (neighbortype *bgp)
{
    tcbtype *tcb;
    int error;
    ulong winsize = BGP_WINDOW;

    /*
     * Remove the existing listener if there's one there.
     */
    if (bgp->bgp_listener) {
	tcp_close(bgp->bgp_listener);
	bgp->bgp_listener = NULL;
    }

    /*
     * Create and initialize the handle.
     */
    tcb = tcp_create(NULL, &error, NULL);
    if (tcb == NULL) {
	if (bgp_debug)
	    buginf("\nBGP: %i passive create failed: %s",
		   bgp->address, tcperror2string(error));
	return(-1);
    }

    tcp_set_property(tcb, TCP_WINDOW_SIZE, &winsize, sizeof(winsize));

    error = tcp_bind(tcb, NULL, BGP_PORT);
    if (error != OK) {
	if (bgp_debug)
	    buginf("\nBGP: %i bind failed: %s",
		   bgp->address, tcperror2string(error));
	tcp_close(tcb);
	return(-1);
    }

    /*
     * Only accept connections from this host.
     */
    tcb->foreignhost.type = ADDR_IP;
    tcb->foreignhost.ip_addr = bgp->address;

    tcp_set_property(tcb, TCP_ACCESS_CHECK, bgp_open_check, sizeof(void *));

    error = tcp_listen(tcb, 1, bgp_open_passive);
    if (error != OK) {
	buginf("\nBGP: %i listen failed: %s",
	       bgp->address, tcperror2string(error));
	tcp_close(tcb);
	return(-1);
    }

    bgp->bgp_listener = tcb;

    return(0);
}

/*
 * bgp_schedule_neighbor_indexing
 * If neighbor indexing should be performed, schedule one by setting 
 * appropriate flags in the neighbor structure. We delay indexing to
 * catch major config change!
 */

BGP_PRIVATE void
bgp_schedule_neighbor_indexing (neighbortype *bgp)
{
    int peergroup_index;

    /*
     * EBGP neighbors should have neighbor index. If not present, mark
     * the neighbor for later allocation. If this is a peergroup member
     * and the index is different from peergroup index, mark the neighbor
     * so that we could reclaim the current index and reset it to 
     * peergroup index later. Also, if the peergroup doesn't have an
     * index, mark it for later allocation.
     */
    if (bgp->peergroup) {
	peergroup_index = bgp->peergroup->neighbor_index;
	if (peergroup_index) {
	    /*
	     * peergroup with index.
	     */
	    if (bgp->neighbor_index &&
		(bgp->neighbor_index != peergroup_index))
		bgp->flags |= BN_RECLAIM_INDEX;
	    else
		bgp->flags |= BN_ALLOCATE_INDEX;

	} else {
	    /*
	     * peergroup without index.
	     */
	    if (bgp->neighbor_index)
		bgp->flags |= BN_RECLAIM_INDEX;
	    else 
		bgp->flags |= BN_ALLOCATE_INDEX;

	    bgp->peergroup->flags |= BN_ALLOCATE_INDEX;
	}

    } else if (!bgp->neighbor_index) {
	/*
	 * Normal neighbor without index
	 */
	bgp->flags |= BN_ALLOCATE_INDEX;
    }

    /*
     * If we need to allocate new index, schedule a new allocation
     */
    if (bgp->flags & BN_ALLOCATE_INDEX)
	TIMER_START(bgp_neighbor_index_timer, BGP_INDEXING_DELAY);
}

/*
 * bgp_neighbor_init
 * Initialize a neighbor. Called when a new neighbor is added to a 
 * peergroup, new neighbor is configured or when the AS is changed.
 */
BGP_PRIVATE void
bgp_neighbor_init (pdbtype *pdb, neighbortype *bgp, ushort asnumber, 
		   ipaddrtype address, boolean add)
{
    int i;
    neighbortype *p, *last;

    bgp->address = address;
    bgp->flags = 0;
    bgp_init_tcp_listener(bgp);
    sprintf(bgp->neighbor_name, "%i", address);
    bgp->yoursystem = asnumber;
    bgp->bgp_internal = (bgp->yoursystem == pdb->asystem);
    bgp->bgp_state = BGPS_IDLE;
    bgp->protocol_version = BGP_VERSION;
    bgp->bgp_weight = BGP_LEARNED_WEIGHT;
    if (bgp->yoursystem != pdb_BGP->asystem)
	bgp->bgp_defminadverttime = BGP_EBGP_MINADVERTINTERVAL*ONESEC;
    else
	bgp->bgp_defminadverttime = BGP_IBGP_MINADVERTINTERVAL*ONESEC;
    bgp->bgp_minadverttime = bgp->bgp_defminadverttime;
	    
    /* set the common administration flag */
    for (i = 0; i < bgp_confed_peer_count; ++i)
	if (bgp->yoursystem == bgp_confed_peers[i])
	    bgp->flags |= BN_COMMONADMIN;

    /* check for scheduling neighbor indexing */
    if (!bgp->bgp_internal)
	bgp_schedule_neighbor_indexing(bgp);

    /* add to neighbor list in lexigrapical order (for snmp) */
    if (add) {
	if (pdb->neighbors) {
	    last = (neighbortype *) &pdb->neighbors;
	    p = pdb->neighbors;
	    while (p && (address > p->address)) {
		last = p;
		p = p->next;
	    }
	    bgp->next = last->next;
	    last->next = bgp;
	} else
	    pdb->neighbors = bgp;
    }
}

/*
 * bgp_route_map_config
 */
BGP_PRIVATE boolean
bgp_route_map_config (route_map_nametype **pptr, char *mapname,
		      int userid)
{
    boolean found;
    const route_map_userinfo *userinfo;

    found = FALSE;
    for (userinfo = bgp_route_map_users; userinfo->userid; userinfo++)
	if (userinfo->userid == userid) {
	    found = TRUE;
	    break;
	}

    if (found)
	return(route_map_config(pptr, mapname, userinfo));
    else
	return(route_map_config(pptr, mapname, NULL));

}

/*
 * bgp_pgrp_member
 * process a peer group member configuration.
 */
BGP_PRIVATE boolean
bgp_pgrp_member (neighbortype *bgp, pdbtype *pdb, ipaddrtype address,
		 char *nametag, boolean sense)
{
    neighbortype *pgrp, *p, *last;
    ushort asnumber;
    char clear_password[PASSWORDLEN+1];
    char *pass;
    idbtype *cur_idb, *new_idb;

    if (sense) {
	if (bgp->peergroup) {
	    if (strcmp(nametag, bgp->peergroup->neighbor_name))
		printf("\n%% Cannot change the peer-group. Deconfigure first");
	    return(FALSE);
	}

	for (pgrp = pdb->peergroup_list; pgrp; pgrp = pgrp->next)
	    if (!strcmp(pgrp->neighbor_name, nametag))
		break;
	if (!pgrp) {
	    printf("\n%% Configure the peer-group first");
	    return(FALSE);					
	}
	
	/* 
	 * For EBGP peergroup, post a warning if the peers are not on the
	 * same cable or the connected interface for the new peer could not
	 * be found out. Exception to this are those which are pre-configured
	 * with EBGP-multihop.
	 */
	if (pgrp->peerhead && !pgrp->peerhead->bgp_internal) {
	    if (bgp->ebgp_ttl == BGP_DEFAULT_EBGP_TTL) {
		cur_idb = samecable(pgrp->peerhead->address);
		new_idb = samecable(address);
		if ((cur_idb != new_idb) || !new_idb)
		    printf("\n%%Non directly connected member %i for "
			   "EBGP peergroup %s", address, nametag);
	    }
	}

	/*
	 * Initialize the neighbor. Use the address field to 
	 * identify new neighbors.
	 * add the neighbor to the peerlist in lexigrapical order
	 */
	bgp->peergroup = pgrp;			/* needed for index check! */
	asnumber = pgrp->yoursystem ? pgrp->yoursystem : bgp->yoursystem;
	bgp_neighbor_init(pdb, bgp, asnumber, address, !bgp->address);

	last = NULL;
	p = pgrp->peerhead;
	while(p && (address > p->address)) {
	    last = p;
	    p = p->nextpeer;
	}
	if (last) {
	    bgp->nextpeer = last->nextpeer;
	    last->nextpeer = bgp;
	} else {
	    bgp->nextpeer = pgrp->peerhead;
	    pgrp->peerhead = bgp;
	}

	/*
	 * Update the peer group member with the information
	 * from the peergroup structure.
	 * Few values will be set always with the peergroup value.
	 * Others will be set only in the presence of default.
	 */
	bgp->protocol_version	= pgrp->protocol_version;
	bgp->bgp_distlistout	= pgrp->bgp_distlistout;
	bgp->bgp_update_source	= pgrp->bgp_update_source;
	bgp->bgp_outlist      	= pgrp->bgp_outlist;
	bgp->bgp_minadverttime	= pgrp->bgp_minadverttime;
	bgp->ebgp_ttl		= pgrp->ebgp_ttl;
	bgp->flags 	       |= (pgrp->flags & PGRP2MEMBER_FLAGS);

 	if (pgrp->flags & BN_RR_CLIENT) {
 	    bgp->flags |= BN_RR_CLIENT;
 	    if (!bgp_cluster_id)		/* No explicit cluster-id */
 		bgp_cluster_id = pdb_BGP->bgp_rtr_id;
 	}

	if (pgrp->bgp_routemapout) {
	    if (!bgp_route_map_config(&bgp->bgp_routemapout, 
				      pgrp->bgp_routemapout->mapname,
				      ROUTE_MAP_BGP_OUTBOUND))
		return(FALSE);
	}
	if (pgrp->bgp_unsuppress_map) {
	    if (!bgp_route_map_config(&bgp->bgp_unsuppress_map, 
				      pgrp->bgp_unsuppress_map->mapname,
				      ROUTE_MAP_BGP_UNSUPPRESS))
		return(FALSE);
	}
	if (pgrp->bgp_default_map) {
	    if (!bgp_route_map_config(&bgp->bgp_default_map, 
				      pgrp->bgp_default_map->mapname,
				      ROUTE_MAP_BGP_DEFAULT))
		return(FALSE);
	}
	if (pgrp->bgp_password.pw) {
	    bgp->bgp_password.encrypt_type = pgrp->bgp_password.encrypt_type;
	    setstring(&bgp->bgp_password.pw, pgrp->bgp_password.pw);
	    if (bgp->bgp_listener) {
		pass = bgp->bgp_password.pw;
		if (bgp->bgp_password.encrypt_type == ENCRYPT_TEMP) {
		    decrypt(pass, clear_password, PASSWORDLEN);
		    pass = clear_password;
		}
		tcp_set_property(bgp->bgp_listener, TCP_MD5KEY, pass,
				 strlen(pass));
	    }
	}
	if (!bgp->bgp_inlist)
	    bgp->bgp_inlist = pgrp->bgp_inlist;
	if (!bgp->bgp_distlistin)
	    bgp->bgp_distlistin = pgrp->bgp_distlistin;
	if (!bgp->bgp_routemapin && pgrp->bgp_routemapin) {
	    if (!bgp_route_map_config(&bgp->bgp_routemapin, 
				      pgrp->bgp_routemapin->mapname,
				      ROUTE_MAP_BGP_INBOUND))
		return(FALSE);
	}
	if (bgp->bgp_weight == BGP_LEARNED_WEIGHT)
	    bgp->bgp_weight = pgrp->bgp_weight;
	
	bgp_async_reset(bgp->address);

    } else {
	bgp->flags |= BN_DELETED;
    }
    return(TRUE);
}

/*
 * bgp_neighbor_parse
 * Process the neighbor command
 */

BGP_PRIVATE boolean
bgp_neighbor_parse (pdbtype *pdb, neighbortype *bgp,
		    parseinfo *csb, ipaddrtype address,
		    char *nametag, boolean sense)
{
    int list;
    neighbortype *p;
    char *mapname;
    ushort asnumber;
    boolean new;
    char *name;
    acl_headertype *acl;
    ulong old_flags;
    bgp_cond_adv_maptype *camap;

    new = FALSE;

    switch (csb->which) {
	
    case NEIGHBOR_BGP_DIST:
	list = GETOBJ(int,1);
	name = GETOBJ(string,1);
	if (csb->sense) {
	    if (list) {
		acl = access_find_or_create_acl(list);
	    } else {
		if (!is_valid_nacl(name)) {
		    printf(nacl_name_error);
		    return(FALSE);
		}
		acl = access_find_or_create_nacl(name, ACL_UNDEFINED);
	    }
	} else {
	    acl = NULL;
	}
	switch (GETOBJ(int,2)) {
	case IN:
	    bgp->bgp_distlistin = acl;
	    break;
	case OUT:
	    bgp->bgp_distlistout = acl;
	    break;
	}
	break;
	
    case NEIGHBOR_BGP_FILAS:
	bgp_as_parse(csb, bgp);
	break;
	
    case NEIGHBOR_BGP_FILLIST:
	bgp_filter_parse(csb, bgp);
	bgp_reset_fcache();
	break;

    case NEIGHBOR_BGP_ROUTEMAP:
	mapname = GETOBJ(string,2);
	switch (GETOBJ(int,2)) {
	case IN:
	    if (csb->sense) {
		if (!bgp_route_map_config(&bgp->bgp_routemapin, mapname,
					  ROUTE_MAP_BGP_INBOUND))
		    return(FALSE);
	    } else
		route_map_deconfig(&bgp->bgp_routemapin);
	    break;
	case OUT:
	    if (csb->sense) {
		if (!bgp_route_map_config(&bgp->bgp_routemapout, mapname,
					  ROUTE_MAP_BGP_OUTBOUND))
		    return(FALSE);
	    } else
		route_map_deconfig(&bgp->bgp_routemapout);
	    break;
	}
	bgp_reset_rcache();
	break;

    case NEIGHBOR_BGP_UNSUPPRESS:
	mapname = GETOBJ(string,2);
	if (csb->sense) {
	    if (!bgp_route_map_config(&bgp->bgp_unsuppress_map, mapname,
				      ROUTE_MAP_BGP_UNSUPPRESS))
		return(FALSE);
	} else
	    route_map_deconfig(&bgp->bgp_unsuppress_map);
	break;

    case NEIGHBOR_BGP_EMHOP:
	bgp->ebgp_ttl = csb->sense ? GETOBJ(int,3) : BGP_DEFAULT_EBGP_TTL;
	break;
	
    case NEIGHBOR_BGP_REMAS:
	asnumber = GETOBJ(int, 1);
	if (bgp->flags & BN_PEERGROUP) {		/* peer group */
	    bgp->yoursystem = asnumber;
	    bgp->bgp_internal = (bgp->yoursystem == pdb->asystem);
	    return (TRUE);
	}
	if (!bgp->address) {
	    bgp_neighbor_init(pdb, bgp, asnumber, address, TRUE);
	    bgp_close(bgp);
	} else {
	    /*
	     * An existing neighbor. 
	     */
	    if (sense) {
		bgp->flags &= ~BN_TEMPORARY;    /* could be template created*/
		if (bgp->yoursystem != asnumber) {
		    bgp_neighbor_init(pdb, bgp, asnumber, address, FALSE);
		    bgp_reset(bgp, NULL);
		}
		return (FALSE);
	    }
	    /* simply mark for deletion to avoid race condition */
	    bgp->flags |= BN_DELETED;
	}
	break;
	
    case NEIGHBOR_BGP_NEIGHLIST:        /* new entry */
	list = GETOBJ(int,1);
	name = GETOBJ(string,2);

	if (!bgp->protocol_version) {	/* partly handled above*/
	    strcpy(bgp->neighbor_name, nametag);

	    bgp->bgp_state = BGPS_IDLE;
	    bgp->protocol_version = BGP_VERSION;
	    bgp->bgp_weight = BGP_LEARNED_WEIGHT;

	    bgp->flags |= BN_TEMPLATE;

	    acl = access_parse_acl(list, name, (list == 0),
				   ACL_IP_NAMED_SIMPLE);

	    if (!acl)
		return(FALSE);

	    bgp->pfilter = acl;

	    /* add to template list */
	    if (pdb->promiscuous) {
		for (p = pdb->promiscuous; p->next; p = p->next)
		    ;
	        p->next = bgp;
	    } else
		pdb->promiscuous = bgp;

	} else {
	    /*
	     * An existing neighbor.
	     */
	    if (bgp->flags & BN_PEERGROUP) {
		printf("\n%% Invalid command for a peer-group");
		return(FALSE);
	    }

	    if (sense) {
		acl = access_parse_acl(list, name, (list == 0),
				       ACL_IP_NAMED_SIMPLE);

		if (!acl)
		    return(FALSE);

		bgp->pfilter = acl;
	    } else {
		bgp_delete_group(bgp);
		return(FALSE);
	    }
	}
	break;

    case NEIGHBOR_BGP_RRCLIENT:
	/*
	 * If this is the first neighbor to be a client and if the 
	 * cluster-id is not explicitly configured, set the cluster-id 
	 * to the router_id.
	 *
	 * If there is no more clients left and if the cluster-id was
	 * not configured explicitly, clear it.
	 */
	if (csb->sense) {
	    bgp->flags |= BN_RR_CLIENT;
	    if (!bgp_cluster_id)
		bgp_cluster_id = pdb_BGP->bgp_rtr_id;
	} else {
	    bgp->flags &= ~BN_RR_CLIENT;
	    if (bgp_cluster_id == pdb_BGP->bgp_rtr_id) {
		for (p = pdb_BGP->neighbors; p; p = p->next)
		    if (p->flags & BN_RR_CLIENT)
			break;
		if (!p)
		    bgp_cluster_id = 0;
	    }
	}

	/*
	 * Check if the neighbor is already a RR_CLIENT. If so, do not
	 * clear the BGP connection.
	 */
	if ( (bgp->address) && !(bgp->flags & BN_RR_CLIENT) )
	    bgp_async_reset(bgp->address);
	break;

    case NEIGHBOR_PERM_CFG:
	if (csb->sense)
	    bgp->flags |= BN_PERMCFG;
	else
	    bgp->flags &= ~BN_PERMCFG;
	break;
	
    case NEIGHBOR_BGP_DEFAULT:
	if (csb->sense) {
	    /* Check if we are configured to default-originate already
	     * or not before setting the flag and set default_state to
	     * DOWN.
	     */
	    if (!(bgp->flags & BN_DEFAULT_ORIGINATE)) {
		bgp->flags |= BN_DEFAULT_ORIGINATE;
		bgp->bgp_default_state = BGP_DEFAULT_DOWN;
	    }
	    if (!bgp_route_map_config(&bgp->bgp_default_map, GETOBJ(string, 2),
				      ROUTE_MAP_BGP_DEFAULT))
		return(FALSE);
	} else {
	    bgp->flags &= ~BN_DEFAULT_ORIGINATE;
	    if (bgp->bgp_default_state == BGP_DEFAULT_SENT)
		bgp->bgp_default_state = BGP_DEFAULT_WITHDRAW;
	    route_map_deconfig(&bgp->bgp_default_map);
	}
	break;

    case NEIGHBOR_BGP_UPDSRC:
	bgp->bgp_update_source = csb->sense ? GETOBJ(idb,1) : NULL;
	break;
	
    case NEIGHBOR_BGP_VERSION:
	bgp->bgp_configed_version = csb->sense ? GETOBJ(int,1) : 0;
	if (bgp->state == BGPS_IDLE)
	    bgp->protocol_version = bgp->bgp_configed_version ?
				    bgp->bgp_configed_version : BGP_VERSION;
	break;
	
    case NEIGHBOR_BGP_WEIGHT:
	bgp->bgp_weight = csb->sense ? GETOBJ(int,1) : BGP_LEARNED_WEIGHT;
	break;
	
    case NEIGHBOR_BGP_MINADVERT:
	bgp->bgp_minadverttime = 
	    (csb->sense ? ONESEC * GETOBJ(int,1) : bgp->bgp_defminadverttime);
	break;

    case NEIGHBOR_BGP_NHSELF:	/* set next hop to be self for this neighbor */
	if (csb->sense) 
	    bgp->flags |= BN_NOHOPCALC;
	else
	    bgp->flags &= ~BN_NOHOPCALC;
	break;

    case NEIGHBOR_BGP_COMMOK:	/* can send community attr to this neighbor */
	if (csb->sense) 
	    bgp->flags |= BN_COMMUNITYOK;
	else
	    bgp->flags &= ~BN_COMMUNITYOK;
	break;

    case NEIGHBOR_BGP_BACKCOMPAT:
	if (csb->sense) 
	    bgp->flags |= BN_BACKCOMPAT;
	else
	    bgp->flags &= ~BN_BACKCOMPAT;
	break;
	
    case NEIGHBOR_BGP_RECONFIG_INOK:
	old_flags = bgp->flags;

	if (csb->sense)
	    bgp->flags |= BN_SOFT_RECONFIG_INOK;
	else
	    bgp->flags &= ~BN_SOFT_RECONFIG_INOK;

	if (bgp->address &&
	    (old_flags != bgp->flags))
	    bgp_async_reset(bgp->address);

	break;
	
    case NEIGHBOR_BGP_REMOVE_PRIVATE_AS:
	if (csb->sense) {

	    if (bgp->bgp_internal) {
		printf("\n%% Private AS cannot be removed for IBGP peers");
		return(FALSE);
	    }
	    bgp->flags |= BN_REMOVE_PRIVATE_AS;

	} else 
	    bgp->flags &= ~BN_REMOVE_PRIVATE_AS;

	break;
	    
    case NEIGHBOR_BGP_PEERGROUP:
	if (csb->sense) {

	    if (bgp->flags & BN_TEMPLATE) {
		printf("\n%% Cannot convert a template to a peer-group");
		return(FALSE);
	    }

	    if (!(bgp->flags & BN_PEERGROUP)) {		/* new peer group */
		strcpy(bgp->neighbor_name, nametag);
		bgp->flags |= BN_PEERGROUP;
		bgp->protocol_version = BGP_VERSION;
		bgp->bgp_defminadverttime = BGP_GROUP_MINADVERTINTERVAL*ONESEC;
		bgp->bgp_minadverttime = bgp->bgp_defminadverttime;

		/*
		 * Give PEERGROUP_UPD_DELAY time for the group
		 * members to establish session.
		 */
		TIMER_START_JITTERED(bgp->bgp_last_upd_sent, 
				     BGP_GROUP_UPD_DELAY, BGP_UPDATE_JITTER);
		COPY_TIMESTAMP(bgp->bgp_last_upd_sent,bgp->bgp_last_upd_check);

		/* Add to peer group list */
		if (pdb->peergroup_list) {
		    for (p = pdb->peergroup_list; p->next; p = p->next)
			;
			p->next = bgp;
		} else
		    pdb->peergroup_list = bgp;
	    }
	} else {
	    bgp_delete_group(bgp);
	    return (FALSE);
	}
	break;

    case NEIGHBOR_BGP_PEERGP_MEM:
	return (bgp_pgrp_member(bgp, pdb, address, nametag, sense));
	break;

    case NEIGHBOR_BGP_PASSWORD:
	if (csb->sense) {
	    char temppassword[PASSWORDLEN_ENCRYPTED+1];
	    char clear_password[PASSWORDLEN+1];
	    char *oldpass, *pass;

	    bgp->bgp_password.encrypt_type = GETOBJ(int,1);
	    oldpass = bgp->bgp_password.pw;
	    pass = GETOBJ(string,2);

	    /*
	     * If we are encrypting
	     * and we got a non encrypted password
	     * encrypt it.
	     */
	    if ((bgp->bgp_password.encrypt_type == ENCRYPT_NONE) &&
		password_encryption_enable) {
		memset(temppassword, 0, sizeof(temppassword));
		bgp->bgp_password.encrypt_type =
		    encrypt(pass, temppassword, 0, PASSWORDLEN_ENCRYPTED);
		pass = temppassword;
	    }
	    setstring(&bgp->bgp_password.pw, pass);
	    /* Do some cleanup if the password actually changed. */
	    if ((oldpass == NULL) || (strcmp(oldpass, pass) == 0)) {
		if (bgp->bgp_password.pw && bgp->bgp_listener) {
		    pass = bgp->bgp_password.pw;
		    if (bgp->bgp_password.encrypt_type == ENCRYPT_TEMP) {
			decrypt(pass, clear_password, PASSWORDLEN);
			pass = clear_password;
		    }
		    tcp_set_property(bgp->bgp_listener, TCP_MD5KEY, pass,
				     strlen(pass));
		}
		/* Bounce the connection */
		bgp_reset(bgp, "neighbor password changed");
	    }
	} else {
	    if (bgp->bgp_password.pw) {
		free(bgp->bgp_password.pw);
		bgp->bgp_password.pw = NULL;
		if (bgp->bgp_listener)
		    tcp_set_property(bgp->bgp_listener, TCP_MD5KEY, NULL, 0);
		/* Bounce the connection */
		bgp_reset(bgp, "neighbor password changed");
	    }
	}
	break;

    case NEIGHBOR_BGP_COND_ADV:
	camap = bgp_find_cond_adv_map(bgp, GETOBJ(string,2), GETOBJ(string,3));
	if (csb->sense) {
	    if (!camap) {
		camap = malloc(sizeof(bgp_cond_adv_maptype));
		if (!camap)
		    return(FALSE);
		if (!bgp_route_map_config(&camap->bgp_advertise_map,
					  GETOBJ(string, 2),
					  ROUTE_MAP_BGP_COND_ADVERTISE))
		    return(FALSE);
		if (!bgp_route_map_config(&camap->bgp_condition_map,
					  GETOBJ(string, 3),
					  ROUTE_MAP_BGP_CONDITION))
		    return(FALSE);
		camap->cond_type = FALSE;  /* for non-exist type */
		camap->cond_change = FALSE;
		camap->adv_state = BGP_ADV_STATE_UNINITIALIZED;
		bgp_link_camap(bgp, camap);
	    } else {
		if (camap->adv_state == BGP_ADV_STATE_DECONFIGURED)
		    camap->adv_state = BGP_ADV_STATE_UNINITIALIZED;
	    }
	} else {
	    if (camap)
		camap->adv_state = BGP_ADV_STATE_DECONFIGURED;
	}
	break;

    default:
	printf("\n%% BGP: Invalid command - %s", csb->line);
	return (FALSE);
    }
    return (TRUE);
}

/*
 * bgp_nv_neighbor
 * Display configuration information about a neighbor.
 *
 * For the members of the peergroup, following commands will 
 * not be nvgen :
 *  	    bgp x.x.x.x update-source
 *		    	next-hop-self
 *			version
 *			advertisement-interval
 *			distribute-list <list> out
 *			filter-list <list> out
 *			route-map <foo> out
 *			unsuppress-map <map>
 *			allow-soft-reconfig
 * Following commands will be nvgen only if it differs from the
 * peer-group leader :
 *	    bgp x.x.x.x remote-as
 *			distribute-list <list> in
 *			filter-list <list> in
 *			route-map <foo> in
 */

BGP_PRIVATE void
bgp_nv_neighbor (neighbortype *bgp, parseinfo *csb)
{
    astype *as;
    bweightinfo *weightexp;
    neighbortype *pgrp;
    bgp_cond_adv_maptype *camap;

    pgrp = bgp->peergroup;

    if (bgp->pfilter)
	nv_write(TRUE, "%s %s neighbor-list %s",
		 csb->nv_command, bgp->neighbor_name, bgp->pfilter->name);
    nv_write(bgp->flags & BN_PEERGROUP, "%s %s peer-group",
	     csb->nv_command, bgp->neighbor_name);
    if (!pgrp || (bgp->yoursystem != pgrp->yoursystem))
	nv_write(bgp->yoursystem, "%s %s remote-as %d",
		 csb->nv_command, bgp->neighbor_name, bgp->yoursystem);
    nv_write(pgrp != NULL, "%s %s peer-group %s",
	     csb->nv_command, bgp->neighbor_name, pgrp->neighbor_name);
    nv_write(bgp->flags & BN_BACKCOMPAT, "%s %s backwards-compatible",
	     csb->nv_command, bgp->neighbor_name);
    nv_write(bgp->flags & BN_PERMCFG, "%s %s configure-neighbors",
	     csb->nv_command, bgp->neighbor_name);
    if ((bgp->ebgp_ttl != BGP_DEFAULT_EBGP_TTL) && 
	(!pgrp || (bgp->ebgp_ttl != pgrp->ebgp_ttl)))
	nv_write(TRUE, "%s %s ebgp-multihop %d",
		 csb->nv_command, bgp->neighbor_name, bgp->ebgp_ttl);
    if (!pgrp) {
	nv_write((bgp->flags & BN_RR_CLIENT), "%s %s route-reflector-client",
		 csb->nv_command, bgp->neighbor_name);
	if (bgp->bgp_update_source && bgp->bgp_update_source->ip_address)
	    nv_write(TRUE, "%s %s update-source %s", csb->nv_command, 
		     bgp->neighbor_name, bgp->bgp_update_source->namestring);
	nv_write(bgp->flags & BN_NOHOPCALC, "%s %s next-hop-self",
		 csb->nv_command, bgp->neighbor_name);
	nv_write(bgp->flags & BN_COMMUNITYOK, "%s %s send-community",
		 csb->nv_command, bgp->neighbor_name);
	if (bgp->flags & BN_DEFAULT_ORIGINATE) {
	    nv_write(TRUE, "%s %s default-originate", csb->nv_command, 
		     bgp->neighbor_name);
 	    if ((bgp->bgp_default_map != NULL) &&
 		(bgp->bgp_default_map->mapname != NULL))
 		 nv_add(TRUE , " route-map %s",
		   bgp->bgp_default_map->mapname);
	}
	nv_write((bgp->flags & BN_SOFT_RECONFIG_INOK), 
		 "%s %s soft-reconfiguration inbound",
		 csb->nv_command, bgp->neighbor_name);
	nv_write(bgp->flags & BN_REMOVE_PRIVATE_AS,
		 "%s %s remove-private-AS", 
		 csb->nv_command, bgp->neighbor_name);
	nv_write(bgp->bgp_configed_version, "%s %s version %d",
		 csb->nv_command, bgp->neighbor_name,
		 bgp->bgp_configed_version);
	nv_write(bgp->bgp_minadverttime != bgp->bgp_defminadverttime,
		 "%s %s advertisement-interval %d", csb->nv_command,
		 bgp->neighbor_name, bgp->bgp_minadverttime/ONESEC);
    }

    if (bgp->bgp_distlistin &&
	(!pgrp || (bgp->bgp_distlistin != pgrp->bgp_distlistin)))
	nv_write(TRUE, "%s %s distribute-list %s in",
		 csb->nv_command, bgp->neighbor_name,
		 bgp->bgp_distlistin->name); 
    if (!pgrp && bgp->bgp_distlistout)
	nv_write(TRUE, "%s %s distribute-list %s out",
		 csb->nv_command, bgp->neighbor_name,
		 bgp->bgp_distlistout->name); 

    if (bgp->bgp_routemapin && 
	(!pgrp || !pgrp->bgp_routemapin ||
	 strcmp(bgp->bgp_routemapin->mapname, pgrp->bgp_routemapin->mapname)))
	nv_write(TRUE, "%s %s route-map %s in", 
		 csb->nv_command, bgp->neighbor_name, 
		 bgp->bgp_routemapin->mapname);
    if (!pgrp && bgp->bgp_routemapout)
	nv_write(TRUE, "%s %s route-map %s out",
		 csb->nv_command, bgp->neighbor_name, 
		 bgp->bgp_routemapout->mapname);
    if (!pgrp && bgp->bgp_unsuppress_map)
	nv_write(TRUE, "%s %s unsuppress-map %s",
		 csb->nv_command, bgp->neighbor_name, 
		 bgp->bgp_unsuppress_map->mapname);
    if (!pgrp && bgp->bgp_cond_adv)
	for (camap = bgp->bgp_cond_adv; camap; camap = camap->next) {
	    if (camap->adv_state != BGP_ADV_STATE_DECONFIGURED)
		nv_write(TRUE, "%s %s advertise-map %s %sexist-map %s",
			 csb->nv_command, bgp->neighbor_name, 
			 camap->bgp_advertise_map->mapname,
			 camap->cond_type ? "" : "non-",
			 camap->bgp_condition_map->mapname);
	}

    if (bgp->bgp_password.pw &&
	(!pgrp || !pgrp->bgp_password.pw ||
	 (strcmp(bgp->bgp_password.pw, pgrp->bgp_password.pw) != 0) ||
	 (bgp->bgp_password.encrypt_type != pgrp->bgp_password.encrypt_type))){
	char *prefix;
	char temppassword[PASSWORDLEN_ENCRYPTED+1];

	prefix = string_getnext();
	sprintf(prefix, "%s %s password", csb->nv_command,
		bgp->neighbor_name);
	nv_password(csb, prefix, PRIV_MAX, NULL, &bgp->bgp_password,
		    temppassword, PASSWORDLEN_ENCRYPTED);
    }

    /*
     * Obsolete commands...
     */
    if (bgp->bgp_inlist &&
	(!pgrp || (bgp->bgp_inlist != pgrp->bgp_inlist))) 
	nv_write(TRUE, "%s %s filter-list %d in",
		 csb->nv_command, bgp->neighbor_name, bgp->bgp_inlist);
    nv_write(!pgrp && bgp->bgp_outlist, "%s %s filter-list %d out",
	     csb->nv_command, bgp->neighbor_name, bgp->bgp_outlist);
    for (weightexp = (bweightinfo *) bgp->bgp_weightdata.qhead;
	 weightexp; weightexp = weightexp->next) {
	nv_write(TRUE, "%s %s filter-list %d weight %u",
		csb->nv_command, bgp->neighbor_name,
		weightexp->access_list,
		weightexp->weight);
    }

    nv_write(bgp->bgp_weight != BGP_LEARNED_WEIGHT, "%s %s weight %u",
	     csb->nv_command, bgp->neighbor_name, bgp->bgp_weight);

    for (as = (astype *) bgp->bgp_asdata.qhead; as; as = as->next) {
	nv_write(TRUE, "%s %s filter-as %d %s",
		 csb->nv_command, bgp->neighbor_name, as->number,
		 as->grant ? "permit" : "deny"); 
	nv_add(as->grant, " %u", as->weight);
    }
}

/*
 * bgp_neighbor_valid_command
 * Verify if this command is allowed for this bgp neighbor
 */
BGP_PRIVATE boolean
bgp_neighbor_valid_command (neighbortype *bgp, parseinfo *csb,
			    ipaddrtype address) 
{
    neighbortype *pgrp;
    ushort grp_asnumber, cfg_asnumber;

    /*
     * This neighbor must exist, or else it could be
     * 		neighbor <address> remote-as
     *		neighbor <address> peer-group <tag>
     * 		neighbor <tag> neighbor-list
     * 		neighbor <tag> peer-group
     *
     *	If it is peergroup member, the peergroup must exist and a 
     *  remote-as should be configured for it.
     */
    if (!bgp) {
	switch (csb->which) {
	case NEIGHBOR_BGP_PEERGP_MEM:
	    for (pgrp = pdb_BGP->peergroup_list; pgrp; pgrp = pgrp->next)
		if (!strcmp(pgrp->neighbor_name, GETOBJ(string, 1)))
		    break;

	    if (!pgrp) {
		printf("\n%% Configure the peer-group first");
		return(FALSE);
	    }
	    if (!pgrp->yoursystem) {
		printf("\n%% Specify remote-as or peer-group remote AS first");
		return(FALSE);
	    }
	    break;
	case NEIGHBOR_BGP_REMAS:
	    if (!address) {
		printf("\n%% Create the peer-group first");
		return(FALSE);
	    }
	    break;
	case NEIGHBOR_BGP_NEIGHLIST:
	case NEIGHBOR_BGP_PEERGROUP:
	    if (address) {
		printf("\n%% Cannot use IP address as peer-group name");
		return(FALSE);
	    }
	    break;
	default:	/* None of the allowed commands */
	    printf("\n%% Specify remote-as or peer-group commands first");
	    return(FALSE);
	}
	return(TRUE);
    }

    /*
     * If it is a peer-group member, check if the command is 
     * valid.
     */
    if (bgp->peergroup) {		    /* peer group member */
	if ((csb->which == NEIGHBOR_BGP_VERSION) ||
	    (csb->which == NEIGHBOR_BGP_UPDSRC) ||
	    (csb->which == NEIGHBOR_BGP_NHSELF) ||
	    (csb->which == NEIGHBOR_BGP_MINADVERT) ||
	    (csb->which == NEIGHBOR_BGP_COMMOK) ||
	    (csb->which == NEIGHBOR_BGP_RRCLIENT) ||
	    (csb->which == NEIGHBOR_BGP_UNSUPPRESS) ||
	    (((csb->which == NEIGHBOR_BGP_FILLIST) ||
	      (csb->which == NEIGHBOR_BGP_DIST) ||
	      (csb->which == NEIGHBOR_BGP_ROUTEMAP)) && 
	     (GETOBJ(int, 2) == OUT))) {
	    printf("\n%% Invalid command for a peer-group member");
	    return(FALSE);
	}	

	/*
	 * If peer-group is configured with AS number, [de]configuring
	 * remote-as for members of the group is not allowed.
	 */
	if (csb->which == NEIGHBOR_BGP_REMAS) {
	    grp_asnumber = bgp->peergroup->yoursystem;
	    cfg_asnumber = GETOBJ(int, 1);

	    if (grp_asnumber &&
		((csb->sense && (grp_asnumber != cfg_asnumber)) ||
		 !csb->sense)) {
		printf("\n%% Peer-group AS %d. "
		       "Cannot %sconfigure remote-as for members",
		       grp_asnumber, csb->sense ? "" : "de");
		return(FALSE);
	    }	
	}	
    }

    /*
     * Checks the Route Reflector commands for consistancy.
     */
    if (!bgp->bgp_internal && 
	(csb->which == NEIGHBOR_BGP_RRCLIENT)) {
	printf("\n%% Invalid command. Not an internal neighbor");
	return(FALSE);
    }

    /*
     * Do not allow the remote-as to be specified for templates.
     */
    if ((bgp->flags & BN_TEMPLATE)  &&
	(csb->which == NEIGHBOR_BGP_REMAS)) {
	printf("\n%% Template neighbor. Cannot specify the remote-as");
	return(FALSE);
    }
    
    return(TRUE);
}

/*
 * bgp_neighbor
 * Parse a neighbor command for BGP
 *
 * Pseudo-public routine called via pdb vector.
 */

BGP_PRIVATE void
bgp_neighbor (pdbtype *pdb, ipaddrtype address,
	      parseinfo *csb, boolean sense)
{
    neighbortype *bgp, *p;
    char *nametag;

    if (csb->nvgen) {
	for (bgp = pdb->peergroup_list; bgp; bgp = bgp->next)
	    bgp_nv_neighbor(bgp, csb);

	for (bgp = pdb->promiscuous; bgp; bgp = bgp->next)
	    bgp_nv_neighbor(bgp, csb);
	
	for (bgp = pdb->neighbors; bgp; bgp = bgp->next)
	    if (!(bgp->flags & BN_TEMPORARY))
		bgp_nv_neighbor(bgp, csb);
	
	return;
    }

    nametag = GETOBJ(string,1);
    
    /*
     * An address of 0 indicates that we've gotten "neighbor <tag>".
     * Search the promiscous neighbors list and the peergroup lists. 
     */
    if (address == 0L) {
	if (nametag == NULL || *nametag == '\0')
	    nametag = "any";
	
	for (bgp = pdb->promiscuous; bgp; bgp = bgp->next)
	    if (!strcmp(bgp->neighbor_name, nametag))
		break;

	if (!bgp) {
	    for (bgp = pdb->peergroup_list; bgp; bgp = bgp->next)
		if (!strcmp(bgp->neighbor_name, nametag))
		    break;
	}
    } else {
	if (csb->sense && ip_ouraddress(address)) {
	    printf("\n%% Cannot configure the local system as neighbor");
	    return;
	}
	for (bgp = pdb->neighbors; bgp; bgp = bgp->next)
	    if (bgp->address == address)
		break;
    }

    /*
     * If no keyword and negative sense, then delete neighbor,
     * if it exists.
     */
    if (csb->which == NEIGHBOR_STD) {
	if (!sense && bgp) {
	    if ((bgp->flags & BN_TEMPLATE) || (bgp->flags & BN_PEERGROUP))
		bgp_delete_group(bgp);
	    else
		bgp->flags |= BN_DELETED;
	}
	return;
    }
    
    if (!bgp_neighbor_valid_command(bgp, csb, address))
	return;

    /*
     * If bgp is still null then we need to create it.
     */
    if (!bgp) {
	if (!sense)
	    return;
	bgp = malloc(sizeof(neighbortype));
	if (!bgp)
	    return;
	bgp->ebgp_ttl = BGP_DEFAULT_EBGP_TTL;
    }

    if (!bgp_neighbor_parse(pdb, bgp, csb, address, nametag, sense))
	return;

    /* 
     * Pass the change in template to the corresponding anonymous neighbors.
     */
    if ((bgp->flags & BN_TEMPLATE) && (csb->which != NEIGHBOR_BGP_NEIGHLIST)){
	for (p = pdb->neighbors; p; p = p->next) {
	    if (p->creator_template == bgp) {
		if (csb->which == NEIGHBOR_PERM_CFG) {
		    if (sense) {
			p->creator_template = NULL;
			p->flags &= ~BN_TEMPORARY;
		    }
		}
		else
		    bgp_neighbor_parse(pdb, p, csb, address, nametag, sense);
	    }
	}
    }

    /*
     * Pass the change in the peer-group to the corresponding members 
     */
    if ((bgp->flags & BN_PEERGROUP) && (csb->which != NEIGHBOR_BGP_PEERGROUP)){
	for (p = pdb->neighbors; p; p = p->next) {
	    if (p->peergroup == bgp)
		bgp_neighbor_parse(pdb, p, csb, p->address, nametag, sense);
	}
    }

    
    /*
     * Check and set the inbound and outbound path policy flag for all
     * neighbors.  
     * Note : It is not efficient but this will be done only during
     * configuration
     */

    for (p = pdb->neighbors; p; p = p->next) {
	p->flags &= ~(BN_INBOUND_PATHPOLICY+BN_OUTBOUND_PATHPOLICY);

	if (p->bgp_inlist ||
	    !QUEUEEMPTY(&p->bgp_weightdata) ||
	    !QUEUEEMPTY(&p->bgp_asdata) ||
	    p->bgp_routemapin)
	    p->flags |= BN_INBOUND_PATHPOLICY;

	if (p->bgp_outlist ||
	    p->bgp_unsuppress_map ||
	    p->bgp_routemapout)
	    p->flags |= BN_OUTBOUND_PATHPOLICY;
    }
}	

/*
 * bgp_aggregate_command
 * Configure BGP aggregates */

BGP_PUBLIC void
bgp_aggregate_command (parseinfo *csb)
{
    addrsumtype *sum, *p, *last;
    ipaddrtype address, mask;
    pdbtype *pdb;
    ulong flags;
    char *suppress_map, *advertise_map, *attribute_map;

    pdb = (pdbtype *) csb->protocol;
    if (pdb->family != PDB_IP)
	return;

    if (csb->nvgen) {
	for (sum = (addrsumtype *) bgp_aggregateQ.qhead; sum;
	     sum = sum->next) {
	    if (sum->flags & ADDRSUM_VALID) {
		nv_write(TRUE, "%s %i %i", csb->nv_command, sum->network,
			 sum->mask);
		nv_add(sum->flags & ADDRSUM_AS_SET, " as-set");
		nv_add(sum->flags & ADDRSUM_AUTOFLTR, " summary-only");
		if (sum->attribute_map != NULL)
		    nv_add(TRUE, " attribute-map %s", 
			   sum->attribute_map->mapname);
		if (sum->advertise_map != NULL)
		    nv_add(TRUE,  " advertise-map %s", 
			   sum->advertise_map->mapname);
		if (sum->suppress_map != NULL)
		    nv_add(TRUE, " suppress-map %s", 
			   sum->suppress_map->mapname);
	    }
	}
	return;
    }

    address = GETOBJ(paddr, 1)->ip_addr;
    mask = GETOBJ(paddr,2)->ip_addr;
    flags = GETOBJ(int,1) | GETOBJ(int,2);
    attribute_map = GETOBJ(string,1);
    suppress_map = GETOBJ(string,2);
    advertise_map = GETOBJ(string,3);

    address &= mask;

    if (mask == 0 && csb->sense) {
	printf("\n% Aggregating to create default makes no sense,"
	       "\nuse a network statement instead.");
	return;
    }

    for (sum = (addrsumtype *) bgp_aggregateQ.qhead; sum; sum = sum->next)
	if (sum->network == address && sum->mask == mask)
	    break;

    if (csb->sense) {
	if (!sum) {
	    sum = malloc(sizeof(addrsumtype));
	    if (!sum)
		return;

	    /* insert entries in order... least specific entries must come
	       before more specific entries so that auto-suppression doesn't
	       fail */

	    last = NULL;
	    p  = (addrsumtype *) bgp_aggregateQ.qhead;
	    while (p) {
		if (address < p->network)
		    break;
		if ((address == p->network) && (mask < p->mask))
		    break;
		last = p;
		p = p->next;
	    }

	    insqueue(&bgp_aggregateQ, sum, last);
	}

	/*
	 * Reset back the aggregate version number so that we get a chance
	 * to look at the complete BGP table.
	 */
	bgp_aggregate_version = 1;

	sum->network = address;
	sum->mask = mask;
	sum->template = NULL;
	if (!bgp_route_map_config(&sum->suppress_map, suppress_map,
				  ROUTE_MAP_BGP_SUPPRESS) ||
	    !bgp_route_map_config(&sum->advertise_map, advertise_map,
				  ROUTE_MAP_BGP_ADVERTISE) ||
	    !bgp_route_map_config(&sum->attribute_map, attribute_map,
				  ROUTE_MAP_BGP_ATTRIBUTE))
	    return;
	sum->flags = flags | ADDRSUM_VALID;

    } else {

	/*
	 * we can't just delete the summary from the list, we need to
	 * tag it for deletion so it will be withdrawn. Reset the aggregate
	 * version number to expose complete BGP table.
	 */
	if (sum) {
	    sum->flags &= ~ADDRSUM_VALID;
	    bgp_aggregate_version = 1;
	}
    }
}

/*
 * bgp_distance_command
 * Parse a "distance bgp" command.
 */

BGP_PUBLIC void
bgp_distance_command (parseinfo *csb)
{
    pdbtype *pdb = csb->protocol;

    if (!router_enable)
	return;

    if (!(pdb->proctype & PROC_BGP))
	return;

    if (csb->nvgen) {
	if ((pdb->distance  != pdb->def_distance)  ||
	    (pdb->distance2 != pdb->def_distance2) ||
	    (pdb->distance3 != pdb->def_distance3))
	    nv_write(TRUE, "%s %d %d %d", csb->nv_command,
		     pdb->distance, pdb->distance2, pdb->distance3);
	return;
    }

    if (csb->sense) {
	pdb->distance = GETOBJ(int,1);
	pdb->distance2 = GETOBJ(int,2);
	pdb->distance3 = GETOBJ(int,3);
    } else {
	pdb->distance = pdb->def_distance;
	pdb->distance2 = pdb->def_distance2;
	pdb->distance3 = pdb->def_distance3;
    }
}

/*
 * bgp_dampening_command
 * Parse bgp dampening command
 */
BGP_PUBLIC void
bgp_dampening_command (parseinfo *csb)
{
    ulong reuse, suppress, stime, htime;

    if (csb->nvgen) {
	if (bgp_cfg_halflife_time) {
	    nv_write(TRUE, "%s", csb->nv_command);
	    nv_add(bgp_cfg_halflife_time != BD_DEFAULT_HALFLIFE, " %d",
		   bgp_cfg_halflife_time);
	    if ((bgp_cfg_reuse_penalty != BD_REUSE_PENALTY) ||
		(bgp_cfg_suppress_penalty != BD_SUPPRESS_PENALTY) ||
		(bgp_cfg_maximum_stime != 
		 (BD_STIME_HALFLIFE_RATIO * bgp_cfg_halflife_time)))
		nv_add(TRUE, " %d %d %d", bgp_cfg_reuse_penalty, 
		       bgp_cfg_suppress_penalty, bgp_cfg_maximum_stime);
	} else
	    if (bgp_dampening_map)
		nv_write(TRUE, "%s route-map %s", csb->nv_command,
			 bgp_dampening_map->mapname);
	return;
    }

    /*
     * Whenever there is change in dampening configuration disable
     * dampening and set the flag for the scanner process to clean
     * up all dampening related data structures. If dampening is
     * enabled, the scanner will also kick start dampening
     */
    if (csb->sense) {
	if (csb->which == BGP_RMAP_DAMPENING) {
	    bgp_cfg_halflife_time = 0;		/* disable normal dampening */
	    if (!bgp_route_map_config(&bgp_dampening_map, GETOBJ(string,1),
				      ROUTE_MAP_BGP_DAMPENING))
		return;

	} else {
	    route_map_deconfig(&bgp_dampening_map);/* disable rmap dampening */

	    htime    = GETOBJ(int,1) ? GETOBJ(int,1) : BD_DEFAULT_HALFLIFE;
	    reuse    = GETOBJ(int,2) ? GETOBJ(int,2) : BD_REUSE_PENALTY;
	    suppress = GETOBJ(int,3) ? GETOBJ(int,3) : BD_SUPPRESS_PENALTY;
	    stime    = GETOBJ(int,4) ? GETOBJ(int,4) :
		       (BD_STIME_HALFLIFE_RATIO * htime);

	    if (reuse > suppress) {
		printf("\n%% Reuse value cannot be less than suppress value");
		return;
	    }
	    if (suppress < reuse) {
		printf("\n%% Suppress value cannot be less than reuse value");
		return;
	    }
	    if (stime < htime) {
		printf("\n%% Maximum suppress time cannot be less than half "
		       "life time");
		return;
	    }

	    if ((bgp_cfg_halflife_time == htime) &&
		(bgp_cfg_reuse_penalty == reuse) &&
		(bgp_cfg_suppress_penalty == suppress) &&
		(bgp_cfg_maximum_stime == stime))
		return;

	    bgp_cfg_halflife_time = htime;
	    bgp_cfg_reuse_penalty = reuse;
	    bgp_cfg_suppress_penalty = suppress;
	    bgp_cfg_maximum_stime = stime;

	} 
    } else {
	if (!bgp_cfg_halflife_time && 
	    !bgp_dampening_map)
	    return;

	if (bgp_cfg_halflife_time)
	    bgp_cfg_halflife_time = 0;
	else
	    route_map_deconfig(&bgp_dampening_map);
    }

    bgp_dampening_enabled = FALSE;
    bgp_dampening_cfg_changed = TRUE;
}

/*
 * bgp_command
 * Parse router bgp subcommands
 */

BGP_PUBLIC void
bgp_command (parseinfo *csb)
{
    neighbortype *bgp;

    if (csb->nvgen) {
	switch (csb->which) {
	case BGP_DEF_LOCALPREF:
	    nv_write(bgp_default_localpref != BGP_DEFAULT_LOCALPREF,
		     "%s %d", csb->nv_command, bgp_default_localpref);
	    break;
	case BGP_FAST_RESET:
	    nv_write(!bgp_fast_reset, "no %s", csb->nv_command);
	    break;
 	case BGP_DPA_ENABLED:
 	    nv_write(bgp_dpa_enabled, "%s", csb->nv_command);
 	    break;
	case BGP_ALWAYS_COMPARE_MED:
	    nv_write(bgp_always_compare_med, "%s", csb->nv_command);
	    break;
	case BGP_REDIST_IBGP:
	    nv_write(bgp_redistribute_ibgp, "%s", csb->nv_command);
	    break;
	case BGP_ROUTER_ID:
	    nv_write(bgp_router_id_cfg, "%s %i", csb->nv_command,
		     bgp_router_id_cfg);
	    break;
	case BGP_CONFED_ID:
	    nv_write(bgp_confed_id, "%s %d", csb->nv_command, bgp_confed_id);
	    break;
	case BGP_CLUSTER_ID:
	    if (bgp_cluster_id && 
		(bgp_cluster_id != pdb_BGP->bgp_rtr_id))
		nv_write(TRUE, "%s %d", csb->nv_command, bgp_cluster_id);
	    break;
	case BGP_CLIENT_REFLECTION:
	    if (!bgp_client_reflection)
		nv_write(TRUE, "no %s", csb->nv_command);
	    break;
	case BGP_DETERM_MED:
 	    nv_write(bgp_determ_med, "%s", csb->nv_command);
 	    break;
	}
	return;
    }

    switch (csb->which) {
    case BGP_DEF_LOCALPREF:
	bgp_default_localpref = csb->sense ?
		GETOBJ(int,1) : BGP_DEFAULT_LOCALPREF;
	break;
    case BGP_FAST_RESET:
	bgp_fast_reset = csb->set_to_default || csb->sense;
	break;
     case BGP_DPA_ENABLED:
 	bgp_dpa_enabled = csb->sense;
 	break;
    case BGP_REDIST_IBGP:
	bgp_redistribute_ibgp = csb->sense;
	break;
    case BGP_ALWAYS_COMPARE_MED:
	bgp_always_compare_med = csb->sense;
	break;
    case BGP_ROUTER_ID:
	bgp_router_id_cfg = csb->sense ? GETOBJ(paddr,1)->ip_addr : 0;
	break;
    case BGP_CONFED_ID:
	bgp_confed_id = csb->sense ? GETOBJ(int,1) : 0;
	break;
    case BGP_CLUSTER_ID:
	for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next)
	    if (bgp->flags & BN_RR_CLIENT)
		break;

	if (bgp) {
	    printf("\n%% Neighbors configured as RR-client. "
		   "Deconfigure first.");
	    return;
	}

	if (csb->sense)
	    bgp_cluster_id = GETOBJ(int, 1);
	else
	    if (bgp_cluster_id != pdb_BGP->bgp_rtr_id)
		bgp_cluster_id = 0;

	break;
    case BGP_CLIENT_REFLECTION:
	bgp_client_reflection = csb->set_to_default || csb->sense;
	break;
    case BGP_DETERM_MED:
 	bgp_determ_med = csb->sense;
 	break;
    default:
	printf("\n%% BGP: Invalid command - %s", csb->line);
	break;
    }
}

/*
 * bgp_confed_peers_command
 * Configure the "bgp confederation peers <as-numbers>" command
 * Note: This function will be called once for each as-number configured.
 */
BGP_PUBLIC void
bgp_confed_peers_command (parseinfo *csb)
{
    uchar i, j;
    ushort as;
    neighbortype *bgp;

    /*
     * As the parser cannot accept more than 256 characters, split
     * the confederation peers to multiple lines
     */
    if (csb->nvgen) {
	if (bgp_confed_peer_count)
	    for (i = 0, j = 0; i < bgp_confed_peer_count; ++i) {
		if (j == 0)
		    nv_write(TRUE, "%s ", csb->nv_command);
		nv_add(TRUE, "%d ", bgp_confed_peers[i]);
		j++;
		if (j == 25)
		    j = 0;
	    }
	return;
    }

    if (!csb->sense) {
	as = GETOBJ(int, 1);
	
	if (!as) {
	    for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next)
		bgp->flags &= ~BN_COMMONADMIN;
	    free(bgp_confed_peers);
	    bgp_confed_peers = NULL;
	    bgp_confed_peer_count = 0;

	} else {
	    for (i = 0; i < bgp_confed_peer_count; ++i) {
		if (bgp_confed_peers[i] != as)
		    continue;
		break;
	    }
	    
	    if (i < bgp_confed_peer_count) {
		bgp_confed_peer_count--;
		for (; i < bgp_confed_peer_count; ++i)
		    bgp_confed_peers[i] = bgp_confed_peers[i+1];

		for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next)
		    if (bgp->yoursystem == as)
			bgp->flags &= ~BN_COMMONADMIN;
	    }
	}
    } else {
	if (!bgp_confed_peers) {
	    bgp_confed_peers = malloc(BGP_MAX_COMMONADMIN * PATHSIZE);
	    if (!bgp_confed_peers)
		return;
	    bgp_confed_peer_count = 0;
	}

	if (bgp_confed_peer_count == BGP_MAX_COMMONADMIN) {
	    printf("\n%% Cannot have more than %d common administrative AS",
		   BGP_MAX_COMMONADMIN);
	    return;
	}

	as = GETOBJ(int, 1);
	if (!as)
	    return;

	/*
	 * Store the confederation peers in sorted order
	 */
	for (i = 0; i < bgp_confed_peer_count; ++i) {
	    if (bgp_confed_peers[i] == as)	/* duplicate AS */
		return;
	    if (bgp_confed_peers[i] < as)
		continue;
	    for (j = bgp_confed_peer_count; j > i; --j)
		bgp_confed_peers[j] = bgp_confed_peers[j-1];
	    break;
	}
	bgp_confed_peers[i] = as;
	bgp_confed_peer_count++;

	for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next)
	    if (bgp->yoursystem == as)
		bgp->flags |= BN_COMMONADMIN;

	return;
    }
}

/*
 * bgp_timers_command
 * Set keepalive and hold timers for BGP
 * Called after parsing router subcommand "timers bgp"
 */

BGP_PUBLIC void
bgp_timers_command (parseinfo *csb)
{
    pdbtype *pdb = csb->protocol;

    if (!router_enable)
	return;

    if (!(pdb->proctype & PROC_BGP))
	return;

    if (csb->nvgen) {
	nv_write(((pdb->bgp_keeptime != BGP_HOLDSECONDS/3) ||
		  (pdb->bgp_holdtime != BGP_HOLDSECONDS)),
		 "%s %d %d", csb->nv_command,
		 pdb->bgp_keeptime, pdb->bgp_holdtime);
	return;
    }

    if (!csb->sense) {
	pdb->bgp_keeptime = BGP_HOLDSECONDS/3;
	pdb->bgp_holdtime = BGP_HOLDSECONDS;
    } else {
	pdb->bgp_keeptime = GETOBJ(int,1);
	pdb->bgp_holdtime = GETOBJ(int,2);
    }
}

/*
 * route_map_free_community
 *
 * Free each item of a particular community match list.
 */
BGP_PUBLIC void 
route_map_free_community (parseinfo *csb)
{    
    route_maptype *map; 
    route_map_match_value *value, *next;

    map = csb->route_map;
    if (!map)
	return;

    value = map->match_list.community;
    for ( ; value; value = next) {
	next = value->next;
	free(value);
    }
    map->match_list.community = NULL;
    map->match_list.match_flags &= ~ROUTE_MAP_MATCH_EXACT_COMMUNITY;
}

/*
 * route_map_match_aspath_command
 */
BGP_PUBLIC void route_map_match_aspath_command (parseinfo *csb)
{
    route_maptype *map;

    if (!(map = csb->route_map)) {
	return;
    }
    route_map_process_match_item(&map->match_list.as_path, GETOBJ(int,2),
				 csb->sense);
}

/*
 * route_map_set_as_path_tag_command
 */

BGP_PUBLIC void route_map_set_as_path_tag_command (parseinfo *csb)
{
    route_maptype *map;

    map = csb->route_map;

    if (csb->sense)
	map->set_list.set_flags |= ROUTE_MAP_ASPATH_TAG_SET;
    else
	map->set_list.set_flags &= ~ROUTE_MAP_ASPATH_TAG_SET;
}

/*
 * route_map_set_as_path_prepend_command
 */

BGP_PUBLIC void route_map_set_as_path_prepend_command (parseinfo *csb)
{
    route_maptype *map;
    ushort *as_path;

    map = csb->route_map;
    if (csb->sense) {
	as_path = map->set_list.as_path;
	if (!as_path) {
	    as_path = malloc(ROUTE_MAP_MAX_ASPATH * sizeof(ushort));
	    if (!as_path)
		return;
	    map->set_list.as_path = as_path;
	    map->set_list.ascount = 0;
	}

	if (map->set_list.ascount >= ROUTE_MAP_MAX_ASPATH) {
	    printf("\n%% Cannot have more than %d as-paths prepended",
		   ROUTE_MAP_MAX_ASPATH);
	    return;
	}

	as_path[map->set_list.ascount] = GETOBJ(int, 1);
	map->set_list.ascount += 1;
	map->set_list.set_flags |= ROUTE_MAP_ASPATH_PND_SET;
    } else {
	free (map->set_list.as_path);
	map->set_list.as_path = NULL;
	map->set_list.set_flags &= ~ROUTE_MAP_ASPATH_PND_SET;
    }
}

/*
 * route_map_set_as_path_prepend_fresh_command
 * This routine called before analysing the as_path numbers, will clear the
 * existing as_path set. This lets route_map_set_as_path_prepend_command()
 * routine create a new as_path set.
 */

BGP_PUBLIC void route_map_set_as_path_prepend_fresh_command (parseinfo *csb)
{
    route_maptype *map;

    map = csb->route_map;
    free (map->set_list.as_path);
    map->set_list.as_path = NULL;
    map->set_list.set_flags &= ~ROUTE_MAP_ASPATH_PND_SET;
}

/*
 * valid_match_community_command
 * This routine will validate the match community command and returns FALSE
 * is the new argument is wrong for the existing match list.
 */

BGP_PRIVATE boolean
valid_match_community_command(parseinfo *csb, route_maptype *map)
{
    route_map_match_value *ele;

    if (GETOBJ(int, 3)) {
	for (ele = map->match_list.community; ele; ele = ele->next) {
	    if (ele->value > MAX_STD_COMLIST)
		return (FALSE);
	}
    }
    if (map->match_list.match_flags & ROUTE_MAP_MATCH_EXACT_COMMUNITY) {
	if (GETOBJ(int, 2) > MAX_STD_COMLIST)
	    return (FALSE);
    }
    return (TRUE);
}

/*
 * route_map_match_community_command
 */
BGP_PUBLIC void route_map_match_community_command (parseinfo *csb)
{
    route_maptype *map;
    route_map_match_value *ele, *next, **prev;
    static ulong match_flag;
    static int first_arg;

    map = csb->route_map;
    if (!map)
	return;
    
    /* The parse chain loops to get all the list values. For each list value,
     * this function will be called. If it is the first time, remember the
     * first argument
     */
    switch (GETOBJ(int,4)) {
    case ROUTEMAP_MATCH_COMMARGS_BEGIN :
	first_arg = 0;
	match_flag = map->match_list.match_flags;
	break;

    case ROUTEMAP_MATCH_COMMARGS :
	/*
	 * Validate the command.
	 */
	if (!valid_match_community_command(csb, map)) {
	    printf("\n%%Exact-match cannot be specified for extended lists");
	    if (first_arg) {
		/*
		 * Remove the elements added by this invalid command.
		 */
		for (prev = &map->match_list.community, ele = *prev;
		     ele;
		     prev = &ele->next, ele = ele->next) {
		    if (first_arg == ele->value) {
			*prev = NULL;
			break;
		    }
		}
		while (ele) {
		    next = ele->next;
		    free(ele);
		    ele = next;
		}
	    }
	    map->match_list.match_flags = match_flag;
	    return;
	}

	/*
	 * After all the list values the "exact-match" keyword is parsed.
	 * Hence, this function could be called just with that.
	 */
	if (GETOBJ(int, 3)) {
	    if (csb->sense)
		map->match_list.match_flags |=
		    ROUTE_MAP_MATCH_EXACT_COMMUNITY;
	    else
		map->match_list.match_flags &=
		    ~ROUTE_MAP_MATCH_EXACT_COMMUNITY;
	    break;
	}

	route_map_process_match_item(&map->match_list.community,
				     GETOBJ(int,2), csb->sense);
	if (!first_arg)
	    first_arg = GETOBJ(int, 2);
	break;
    }
    return;
}

/*
 * route_map_process_community_item
 * This function is responsible for adding/removing a community attribute 
 * to the route_map_community structure.
 */
BGP_PRIVATE void
route_map_process_community_item(route_map_community **pptr,
				 ulong		     value,
				 boolean	     sense)
{
    route_map_community *community = *pptr;

    if (sense) {
	/*
	 * If the community structure is not present malloc it.
	 */
	if (!community) {
	    community = malloc(sizeof(route_map_community));
	    if (!community)
		return;
	    
	    community->count = 0;
	    *pptr = community;
	}

	if (community->count == ROUTE_MAP_MAX_COMMUNITY) {
	    printf("\n%% Can not have more than %d community attributes",
		   ROUTE_MAP_MAX_COMMUNITY);
	    return;
	}

	/*
	 * Add the value to the array in sorted order.
	 */
	sorted_array_add(community->com_array, &community->count, value);
    }
}

/*
 * route_map_set_community_command
 */
void route_map_set_community_command (parseinfo *csb)
{
    route_maptype *map;
    ulong community;

    map = csb->route_map;
    if (csb->sense) {
	if (GETOBJ(int, 6)) {
	    map->set_list.set_flags |= ROUTE_MAP_COMMUNITY_ADD;
	} else {
	    if (GETOBJ(int,3))
		community = GETOBJ(int,3);
	    else
		community = (GETOBJ(int,4) << 16) + GETOBJ(int,5);

	    map->set_list.set_flags |= ROUTE_MAP_COMMUNITY_SET;
	    route_map_process_community_item(&map->set_list.community, 
					     community, csb->sense);

	    /*
	     * If 'set community none', set count to zero.
	     */
	    if (GETOBJ(int, 2) == 1)
		map->set_list.community->count = 0;
	}
    } else {
	free (map->set_list.community);
	map->set_list.community = NULL;
	map->set_list.set_flags &= ~(ROUTE_MAP_COMMUNITY_ADD+
				     ROUTE_MAP_COMMUNITY_SET);
    }
}

/*
 * route_map_set_weight_command
 */
void route_map_set_weight_command (parseinfo *csb)
{
    route_maptype *map;

    map = csb->route_map;

    if (csb->sense) {
	map->set_list.set_flags |= ROUTE_MAP_WEIGHT_SET;
	map->set_list.weight = GETOBJ(int,2);
    } else {
	map->set_list.set_flags &= ~ROUTE_MAP_WEIGHT_SET;
    }
}

/*
 * route_map_set_dampening_command
 */
void route_map_set_dampening_command (parseinfo *csb)
{
    route_maptype *map;
    ushort htime, reuse, suppress, max_stime;

    map = csb->route_map;
    
    if (csb->sense) {
	htime = GETOBJ(int,2);
	reuse = GETOBJ(int,3);
	suppress = GETOBJ(int,4);
	max_stime = GETOBJ(int, 5);

	if (reuse > suppress) {
	    printf("\n%% Reuse value cannot be less than suppress value");
	    return;
	}
	if (suppress < reuse) {
	    printf("\n%% Suppress value cannot be less than reuse value");
	    return;
	}
	if (max_stime < htime) {
	    printf("\n%% Maximum suppress time cannot be less than half "
		   "life time");
	    return;
	}

	map->set_list.set_flags |= ROUTE_MAP_DAMPENING_SET;
	map->set_list.damp_value.halflife_time = htime;
	map->set_list.damp_value.reuse_limit = reuse;
	map->set_list.damp_value.suppress_limit = suppress;
	map->set_list.damp_value.maximum_stime = max_stime;
    } else {
	/*
	 * to deconfigure set halflife-time to zero.
	 */
	map->set_list.set_flags &= ~ROUTE_MAP_DAMPENING_SET;
	map->set_list.damp_value.halflife_time = 0;
    }
}

/*
 * route_map_set_destpref_command
 */
void route_map_set_destpref_command (parseinfo *csb)
{
    route_maptype *map;

    map = csb->route_map;

    if (csb->sense) {
	map->set_list.set_flags |= ROUTE_MAP_DESTPREF_SET;
	map->set_list.destpref = GETOBJ(int,2);
    } else {
	map->set_list.set_flags &= ~ROUTE_MAP_DESTPREF_SET;
    }
}

/*
 * route_map_set_origin_command
 */
void route_map_set_origin_command (parseinfo *csb)
{
    route_maptype *map;

    map = csb->route_map;

    if (csb->sense) {
	map->set_list.set_flags |= ROUTE_MAP_ORIGIN_SET;
	map->set_list.origin = GETOBJ(int,2);
	map->set_list.origin_as = GETOBJ(int,3);
    } else {
	map->set_list.set_flags &= ~ROUTE_MAP_ORIGIN_SET;
	map->set_list.origin_as = 0;
    }
}

/*
 * route_map_set_localpref_command
 */
void route_map_set_localpref_command (parseinfo *csb)
{
    route_maptype *map;

    map = csb->route_map;

    if (csb->sense) {
	map->set_list.set_flags |= ROUTE_MAP_LOCALPREF_SET;
	map->set_list.local_pref = GETOBJ(int,2);
    } else {
	map->set_list.set_flags &= ~ROUTE_MAP_LOCALPREF_SET;
    }
}

/*
 * bgp_route_map_match_nvgen
 * NVgen BGP match clauses.
 */

BGP_PRIVATE void bgp_route_map_match_nvgen (parseinfo *csb,
					    route_maptype *map)
{
    if (map->match_list.as_path) {
	nv_write(TRUE, "%s as-path", csb->nv_command);
	route_map_nvadd_number(map->match_list.as_path);
    }

    if (map->match_list.community) {
	nv_write(TRUE, "%s community", csb->nv_command);
	route_map_nvadd_number(map->match_list.community);
	if (map->match_list.match_flags & ROUTE_MAP_MATCH_EXACT_COMMUNITY)
	    nv_add(TRUE, " exact-match");
    }
}

/*
 * bgp_route_map_set_nvgen
 * NVgen BGP set clauses.
 */
static const id_string origin_values[] = {
	{ORIGIN_IGP, "igp"},
	{ORIGIN_EGP, "egp"},
	{ORIGIN_INCOMPLETE, "incomplete"},
	{0, NULL}
};

BGP_PRIVATE void bgp_route_map_set_nvgen (parseinfo *csb,
					  route_maptype *map)
{
    route_map_community *community;
    route_map_dampening *damp_value;
    int i;

    if (map->set_list.set_flags & ROUTE_MAP_LOCALPREF_SET) {
	nv_write(TRUE, "%s local-preference %u", csb->nv_command,
		 map->set_list.local_pref);
    }

    if (map->set_list.set_flags & ROUTE_MAP_WEIGHT_SET) {
	nv_write(TRUE, "%s weight %u", csb->nv_command, map->set_list.weight);
    }

    if (map->set_list.set_flags & ROUTE_MAP_DESTPREF_SET) {
 	nv_write(TRUE, "%s destination-preference %u", csb->nv_command,
 		 map->set_list.destpref);
    }

    if (map->set_list.set_flags & ROUTE_MAP_ORIGIN_SET) {
	nv_write(TRUE, "%s origin ", csb->nv_command);
	nv_add_id_string(origin_values, map->set_list.origin);
	nv_add(map->set_list.origin_as, " %u", map->set_list.origin_as);
    }

    if ((map->set_list.set_flags & ROUTE_MAP_ASPATH_PND_SET) &&
	map->set_list.as_path) {
	nv_write(TRUE, "%s as-path prepend", csb->nv_command);
	for (i = 0; i < map->set_list.ascount; ++i)
	    nv_add(TRUE, " %d", map->set_list.as_path[i]);
    }

    if (map->set_list.set_flags & ROUTE_MAP_ASPATH_TAG_SET)
	nv_write(TRUE, "%s as-path tag", csb->nv_command);

    if (map->set_list.set_flags & ROUTE_MAP_DAMPENING_SET) {
	damp_value = &(map->set_list.damp_value);
	nv_write(TRUE, "%s dampening %d %d %d %d", csb->nv_command,
		 damp_value->halflife_time, damp_value->reuse_limit, 
		 damp_value->suppress_limit, damp_value->maximum_stime);
    }

    community = map->set_list.community;
    if (community) {
	nv_write(TRUE, "%s community", csb->nv_command);
	if (community->count) {
	    ip_community_nv_array(community->com_array, community->count);
	    if (map->set_list.set_flags & ROUTE_MAP_COMMUNITY_ADD)
		nv_add(TRUE, " additive");
	} else 
	    nv_add(TRUE, " none");
    }
}

/*
 * bgp_show_route_map_match
 * Show match clauses.
 */

BGP_PRIVATE void bgp_show_route_map_match (route_maptype *map)
{
    route_map_match_value *value;

    value = map->match_list.as_path;
    if (value) {
	printf("\n    as-path (as-path filter): ");
    }
    for (; value; value = value->next) {
	printf("%d ", value->value);
    }
    
    value = map->match_list.community;
    if (value) {
	printf("\n    community (community-list filter): ");
    }
    for (; value; value = value->next) {
	printf("%d ", value->value);
    }
    if (map->match_list.community && 
	(map->match_list.match_flags & ROUTE_MAP_MATCH_EXACT_COMMUNITY))
	printf("exact-match");
}

/*
 * bgp_show_route_map_set
 * Show set clauses.
 */

BGP_PRIVATE void bgp_show_route_map_set (route_maptype *map)
{
    route_map_community   *community;
    route_map_dampening   *damp_value;
    int i;

    if (map->set_list.set_flags & ROUTE_MAP_LOCALPREF_SET) {
	printf("\n    local-preference %u", map->set_list.local_pref);
    }

    if (map->set_list.set_flags & ROUTE_MAP_DESTPREF_SET) {
	printf("\n    destination-preference %d", 
	       map->set_list.destpref);
    }
    
    if ((map->set_list.set_flags & ROUTE_MAP_ASPATH_PND_SET) &&
	map->set_list.as_path) {
	printf("\n    as-path prepend");
	for (i = 0;	i < map->set_list.ascount; ++i)
	    printf(" %d", map->set_list.as_path[i]);
    }
    
    if (map->set_list.set_flags & ROUTE_MAP_ASPATH_TAG_SET)
	printf("\n    as-path tag");
    
    if (map->set_list.community) {
	community = map->set_list.community;
	printf("\n    community ");
	ip_community_print_array(community->com_array, community->count);
	if (map->set_list.set_flags & ROUTE_MAP_COMMUNITY_ADD)
	    printf(" additive");
	if (community->count == 0)
	    printf("none");
    }
    
    if (map->set_list.set_flags & ROUTE_MAP_WEIGHT_SET) {
	printf("\n    weight %u", map->set_list.weight);
    }
    
    if (map->set_list.set_flags & ROUTE_MAP_ORIGIN_SET) {
	printf("\n    origin ");
	printf_id_string(origin_values, map->set_list.origin);
	if (map->set_list.origin_as)
	    printf(" %d", map->set_list.origin_as);
    }

    if (map->set_list.set_flags & ROUTE_MAP_DAMPENING_SET) {
	damp_value = &(map->set_list.damp_value);
	printf("\n    dampening %d %d %d %d", damp_value->halflife_time,
	       damp_value->reuse_limit, damp_value->suppress_limit,
	       damp_value->maximum_stime);
    }
}

/* 
 * bgp_show_ip_proto
 * Show bgp specific information
 *
 * Pseudo-public routine called via pdb vector.
 */

BGP_PRIVATE void
bgp_show_ip_proto (pdbtype *pdb) 
{
    neighbortype *neigh;
    addrsumtype *sum;
    int headerflag, entry, len, i;

    static const char header[] = 
	"    Address          FiltIn FiltOut DistIn DistOut Weight RouteMap";

    if (bgp_confed_peer_count) {
	printf("\n  BGP confederation consists of ASs:");
	for (i = 0; i < bgp_confed_peer_count; i++)
	    printf(" %d", bgp_confed_peers[i]);
	printf("\n");
	if (bgp_confed_id)
	    printf("\n  BGP confederation advertized as AS %d",
		   bgp_confed_id);
    }

    if (bgp_cluster_id) {
	printf("\n  Route Reflector");
	if (bgp_cluster_id != pdb_BGP->bgp_rtr_id)
	    printf(" with the cluster-id %lu", bgp_cluster_id);
        if (!bgp_client_reflection)
	    printf(", no client to client reflection");
    }

    if (bgp_cfg_halflife_time || bgp_dampening_map) {
	printf("\n  Route flap dampening enabled. ");
	if (bgp_cfg_halflife_time) {
	    printf("\n  Halflife time %d minutes, reuse value %d",
		   bgp_cfg_halflife_time, bgp_cfg_reuse_penalty);
	    printf("\n  Suppress value %d, maximum suppress time %d",
		   bgp_cfg_suppress_penalty, bgp_cfg_maximum_stime);
	} else 
	    printf("Dampening routemap is %s", bgp_dampening_map->mapname);
    }

    printf("\n  IGP synchronization is %sabled", 
	   pdb->sync_disabled ? "dis" : "en");

    printf("\n  Automatic route summarization is %sabled",
	    pdb->auto_summary ? "en" : "dis");

    if (pdb->defaultmetricflag)
	printf("\n  Default redistribution metric is %d", pdb->default_metric);
    if (bgp_default_localpref != BGP_DEFAULT_LOCALPREF)
	printf("\n  Default local-preference applied to incoming routes is %u",
	       bgp_default_localpref);
    showipproto_redistribute(pdb);

    if (!QUEUEEMPTY(&bgp_aggregateQ)) {
	printf("\n  Aggregate Generation:");
	for (entry = 0; (sum = queuepick(&bgp_aggregateQ, entry++)); )
	    if ((sum->flags & (ADDRSUM_VALID|ADDRSUM_HIDDEN))
		== ADDRSUM_VALID) {
		len = printf("\n    %i/%d",
		       sum->network, ip_bitsinmask(sum->mask));
		printf("%*c", 23-len, ' ');
		if (sum->flags & ADDRSUM_AS_SET)
		    printf(" as-set");
		if (sum->flags & ADDRSUM_AUTOFLTR)
		    printf(" summary-only");
		if (sum->attribute_map)
		    printf(" attribute-map %s", sum->attribute_map->mapname);
		if (sum->advertise_map)
		    printf(" advertise-map %s", sum->advertise_map->mapname);
		if (sum->suppress_map)
		    printf(" suppress-map %s", sum->suppress_map->mapname);
	    }
    }

    for (neigh = pdb->neighbors, headerflag = TRUE;
	 neigh; neigh = neigh->next) {
	if (headerflag) {
	    printf("\n  Neighbor(s):");
	    automore_header(header);
	    headerflag = FALSE;
	}
   
	printf("\n    %15i ", neigh->address);

	if (neigh->bgp_inlist) 
	    printf(" %-6d", neigh->bgp_inlist);
	else
	    printf(" %6c", ' ');

	if (neigh->bgp_outlist)
	    printf(" %-7d", neigh->bgp_outlist);
	else
	    printf(" %7c", ' ');

	if (neigh->bgp_distlistin)
	    printf(" %-6s", neigh->bgp_distlistin->name);
	else
	    printf(" %6c", ' ');

	if (neigh->bgp_distlistout)
	    printf(" %-7s", neigh->bgp_distlistout->name);
	else
	    printf(" %-7c", ' ');

	if (neigh->bgp_weight)
	    printf(" %-6u", neigh->bgp_weight);
	else
	    printf(" %6c", ' ');

	if (neigh->bgp_routemapin)
	    printf(" %s", neigh->bgp_routemapin->mapname);
    }
    automore_header(NULL);
}

/*
 * bgp_ip_path
 *
 * Return a string containing any BGP information we know about the source
 * of this network/mask... used for route maps, called via a registry.
 */

BGP_PRIVATE char *
bgp_ip_path (ipaddrtype addr, ipaddrtype mask, char *buffer, int buflen)
{
    bnettype *net;
    battrtype *attr;

    if (!pdb_BGP || !pdb_BGP->running)
	return (NULL);

    net = bgp_netlookup(addr, mask);

    if (!net || net->best == BGP_NO_BEST)
	return (NULL);

    attr = net->paths[net->best].attr;

    return bgp_path2string(attr->as_path, attr->aslength, buffer, buflen);
}

/*
 * bgp_ip_originas
 *
 * return a string containing any BGP information we know about the source
 * of this network/mask... used for the AS path printing in traceroute.
 */
 
BGP_PRIVATE boolean
bgp_ip_originas (ipaddrtype network, ipaddrtype mask, ushort *buf, int size)
{
    bnettype *net;
    battrtype *attr;

    if (!pdb_BGP || !pdb_BGP->running)
	return (FALSE);

    net = bgp_netlookup(network, mask);

    if (!net || net->best == BGP_NO_BEST)
	return (FALSE);

    attr = net->paths[net->best].attr;
    return bgp_path2originas(attr->as_path, attr->aslength, buf, size);
}

/*
 * bgp_community_list_callback
 *
 * Callback function when a community-list is changed.
 * Just flag the cache to be purged. Later bgp_router will do the 
 * cleaning.
 */
BGP_PUBLIC void
bgp_community_list_callback (int list)
{
    if (!pdb_BGP || !pdb_BGP->running)
	return;
    bgp_purge_cache = TRUE;
}

/*
 * bgp_access_list_callback
 * Callback function when a ip access-list or a as-path access-list
 * is changed.
 * Just flag the cache to be purged. Later bgp_router will do the
 * cleaning.
 */
BGP_PRIVATE void
bgp_access_list_callback (acl_headertype *acl)
{
    if (!pdb_BGP || !pdb_BGP->running)
	return;

    bgp_purge_cache = TRUE;
}

/*
 * bgp_aspath_access_list_callback
 * Callback function when a as-path access-list is changed.
 * Used as a wrapper until as-paths grok acl_headertype.
 */
BGP_PRIVATE void
bgp_aspath_access_list_callback (int list)
{
    bgp_access_list_callback(NULL);
}

/*
 * bgp_set_route_map_head
 */
BGP_PRIVATE boolean
bgp_set_route_map_head (route_map_nametype *map, char *mapname,
			route_map_headtype *head)
{
    if (map && (strcmp(map->mapname, mapname) == 0)) {
	map->head = head;
	route_map_sanity_check(map);
	return(TRUE);
    }
    return(FALSE);
}

/*
 * bgp_route_map_inform
 * Callback function when a add/delete/change of a route-map happens.
 */
BGP_PRIVATE void
bgp_route_map_inform (char *mapname, int event)
{
    neighbortype *bgp;
    route_map_headtype *head;
    addrsumtype *sum;
    bgp_cond_adv_maptype *map;

    if (!pdb_BGP)
	return ;

    if (pdb_BGP->running)
	bgp_purge_cache = TRUE;

    /*
     * If it is a route-map add or delete command, update the
     * head pointer in the route_map_nametype structure.
     */
    head = find_route_map_head(mapname);
    bgp_set_route_map_head(pdb_BGP->table_map, mapname, head);

    for (bgp = pdb_BGP->neighbors; bgp; bgp = bgp->next) {
	bgp_set_route_map_head(bgp->bgp_routemapin, mapname, head);
	bgp_set_route_map_head(bgp->bgp_routemapout, mapname, head);
	bgp_set_route_map_head(bgp->bgp_default_map, mapname, head);
	bgp_set_route_map_head(bgp->bgp_unsuppress_map, mapname, head);
	for (map = bgp->bgp_cond_adv; map; map = map->next) {
	    bgp_set_route_map_head(map->bgp_advertise_map, mapname, head);
	    bgp_set_route_map_head(map->bgp_condition_map, mapname, head);
	}
    }

    for (bgp = pdb_BGP->peergroup_list; bgp; bgp = bgp->next) {
	bgp_set_route_map_head(bgp->bgp_routemapin, mapname, head);
	bgp_set_route_map_head(bgp->bgp_routemapout, mapname, head);
	bgp_set_route_map_head(bgp->bgp_default_map, mapname, head);
	bgp_set_route_map_head(bgp->bgp_unsuppress_map, mapname, head);
	for (map = bgp->bgp_cond_adv; map; map = map->next) {
	    bgp_set_route_map_head(map->bgp_advertise_map, mapname, head);
	    bgp_set_route_map_head(map->bgp_condition_map, mapname, head);
	}
    }
    
    for (sum = (addrsumtype *) bgp_aggregateQ.qhead; sum; sum = sum->next) {
	bgp_set_route_map_head(sum->attribute_map, mapname, head);
	bgp_set_route_map_head(sum->advertise_map, mapname, head);
	bgp_set_route_map_head(sum->suppress_map, mapname, head);
    }
    
    /*
     * If the dampening route-map is affected, disable dampening and
     * set the flag so that bgp scanner could handle the dampening
     * related changes later 
     */
    if (bgp_set_route_map_head(bgp_dampening_map, mapname, head)) {
	if (bgpdamp_debug) 
	    buginf("\nBGP: Dampening route-map modified.");
	bgp_dampening_enabled = FALSE;
	bgp_dampening_cfg_changed = TRUE;
    }
}

/*
 * bgp_cleanup
 * Shutdown BGP processing, kill BGP processes.
 * Neighbors have been flushed by support code in route3.c
 * Does not release storage for pdb.
 *
 * Pseudo-public routine called through PDB vector
 */

BGP_PRIVATE void
bgp_cleanup (pdbtype *pdb)
{
    neighbortype *bgp, *next;

    if (bgp_iopid) {
	process_kill(bgp_iopid);
	bgp_iopid = 0;
    }
    if (bgp_scanpid) {
	process_kill(bgp_scanpid);
	bgp_scanpid = 0;
    }
    pdb->redist_callback = NULL;
    bgp_reset(NULL, NULL);	/* Will free the flist/rmap cache entries*/
    bgp_reset_gwcache();
    while (!QUEUEEMPTY(&bgp_redistQ))
	free(dequeue(&bgp_redistQ));
    while (!QUEUEEMPTY(&bgp_requestQ))
	free(dequeue(&bgp_requestQ));
    while (!QUEUEEMPTY(&bgp_resetQ))
	free(dequeue(&bgp_resetQ));
    while (!QUEUEEMPTY(&bgp_aggregateQ))
	bgp_free_aggregate(dequeue(&bgp_aggregateQ));
    while (!QUEUEEMPTY(&bgp_backdoorQ))
	free(dequeue(&bgp_backdoorQ));

    /* free up neighbor entries */
    bgp = pdb->neighbors;
    pdb->neighbors = NULL;
    while (bgp) {
	next = bgp->next;
	bgp_free_neighbor(bgp);
	bgp = next;
    }

    /* free up templates */
    bgp = pdb->promiscuous;
    pdb->promiscuous = NULL;
    while (bgp) {
	next = bgp->next;
	bgp_free_neighbor(bgp);
	bgp = next;
    }

    /* free up peergroup leader entries */
    bgp = pdb->peergroup_list;
    pdb->peergroup_list = NULL;
    while (bgp) {
	next = bgp->next;
	bgp_free_neighbor(bgp);
	bgp = next;
    }

    /* free up dampening related structures */
    bgp_deconfig_dampening();
    bgp_dampening_enabled = FALSE;
    route_map_deconfig(&bgp_dampening_map);

    unqueue(&protoQ, pdb);
    free_pdbindex(pdb);
    free(pdb);
    pdb_BGP = NULL;

    free(bgp_router_path_buffer);	bgp_router_path_buffer = NULL;
    free(bgp_rcvd_aspath_buffer);	bgp_rcvd_aspath_buffer = NULL;
    free(bgp_set_aspath_buffer);	bgp_set_aspath_buffer = NULL;
    free(bgp_router_attr_buffer);	bgp_router_attr_buffer = NULL;
    free(bgp_router_com_buffer);	bgp_router_com_buffer = NULL;
    free(bgp_edit_attr_buffer);		bgp_edit_attr_buffer = NULL;
    free(bgp_edit_com_buffer);		bgp_edit_com_buffer = NULL;
    free(bgp_router_rrinfo_buffer);	bgp_router_rrinfo_buffer = NULL;
    free(bgppathtable);			bgppathtable = NULL;
    free(bgpupdatetable);		bgpupdatetable = NULL;
    free(bgp_gwcache);			bgp_gwcache = NULL;
    free(bgp_filter_list_cache);	bgp_filter_list_cache = NULL;
    free(bgp_route_map_cache);		bgp_route_map_cache = NULL;
    free(bgp_emergency_msg);		bgp_emergency_msg = NULL;
    free(bgp_confed_peers);		bgp_confed_peers = NULL;
    free(bgp_neighbor_bitfield);	bgp_neighbor_bitfield = NULL;
    chunk_destroy(fcache_chunks);	fcache_chunks = NULL;
    chunk_destroy(rcache_chunks);	rcache_chunks = NULL;
    chunk_destroy(damp_chunks);		damp_chunks = NULL;
    bgp_confed_peer_count = 0;
}

/*
 * bgp_pdb_init
 * Initialize a BGP Protocol Descriptor Block
 */

BGP_PRIVATE pdbtype *
bgp_pdb_init (ushort asystem)
{
    pdbtype *pdb;

    pdb = pdb_BGP;
    if (pdb) {
        if (pdb->running) {
	    printf("\nBGP is already running; AS is %d", pdb->asystem);
	    return (NULL);
        }
    } else {
        pdb_BGP = pdb = malloc(sizeof(pdbtype));
        if (pdb == NULL) {
	    return(NULL);
        }
    }
 
    if (!bgp_router_path_buffer)
	bgp_router_path_buffer = malloc(ASCII_PATHBUFSIZE);
    if (!bgp_rcvd_aspath_buffer)
	bgp_rcvd_aspath_buffer = malloc(BGP_PATHBUFLEN);
    if (!bgp_set_aspath_buffer)
	bgp_set_aspath_buffer = malloc(BGP_PATHBUFLEN);
    if (!bgppathtable)
	bgppathtable = malloc(sizeof(battrtype *) * NETHASHLEN);
    if (!bgpupdatetable)
	bgpupdatetable = malloc(sizeof(bmsgtype *) * NETHASHLEN);
    if (!bgp_gwcache)
	bgp_gwcache = malloc(sizeof(bgpgateway *) * NETHASHLEN);
    if (!bgp_filter_list_cache)
	bgp_filter_list_cache = malloc(sizeof(bgp_filter_list_ctype *) * 
				       NETHASHLEN);
    if (!bgp_route_map_cache)
	bgp_route_map_cache = malloc(sizeof(bgp_route_map_ctype *) * 
				     NETHASHLEN);
    if (!bgp_emergency_msg)
	bgp_emergency_msg = malloc(sizeof(bmsgtype) + BGP_MAXBYTES);
    if (!bgp_router_attr_buffer)
	bgp_router_attr_buffer = malloc(sizeof(battrtype) + BGP_PATHBUFLEN);
    if (!bgp_router_com_buffer)
	bgp_router_com_buffer = malloc(BGP_COMMUNITY_BUFSIZE);
    if (!bgp_edit_attr_buffer)
	bgp_edit_attr_buffer = malloc(sizeof(battrtype) + BGP_PATHBUFLEN);
    if (!bgp_edit_com_buffer)
	bgp_edit_com_buffer = malloc(BGP_COMMUNITY_BUFSIZE);
    if (!bgp_router_rrinfo_buffer)
	bgp_router_rrinfo_buffer = malloc(sizeof(rrinfotype) + BGP_RR_BUFLEN);
    if (!bgp_neighbor_bitfield)
	bgp_neighbor_bitfield = malloc(BGP_INIT_BITFIELD_SIZE);

    /*
     * Initialize the route_map and filter_list chunks.
     */
    if (!rcache_chunks)
	rcache_chunks = chunk_create(sizeof(bgp_route_map_ctype), 
				     RCACHE_CHUNK_MAX, CHUNK_FLAGS_DYNAMIC, 
				     NULL, 0,
				     "BGP rcache chunk");
    if (!fcache_chunks)
	fcache_chunks = chunk_create(sizeof(bgp_filter_list_ctype), 
				     FCACHE_CHUNK_MAX, CHUNK_FLAGS_DYNAMIC, 
				     NULL, 0,
				     "BGP fcache chunk");

    /*
     * If we didn't have enough memory to intiailize our main workspace,
     * sign off.
     */
    if (!bgp_router_path_buffer || !bgppathtable || !bgpupdatetable ||
	!bgp_gwcache || !bgp_emergency_msg ||!bgp_router_com_buffer ||
	!bgp_router_attr_buffer || !bgp_edit_com_buffer ||
	!bgp_edit_attr_buffer || !bgp_filter_list_cache ||
	!bgp_route_map_cache || !rcache_chunks || !fcache_chunks ||
	!bgp_router_rrinfo_buffer || !bgp_neighbor_bitfield ||
	!bgp_rcvd_aspath_buffer || !bgp_set_aspath_buffer) {
	free(pdb);			pdb_BGP = NULL;
	free(bgp_router_path_buffer);	bgp_router_path_buffer = NULL;
	free(bgp_rcvd_aspath_buffer);	bgp_rcvd_aspath_buffer = NULL;
	free(bgp_set_aspath_buffer);	bgp_set_aspath_buffer = NULL;
	free(bgp_router_attr_buffer);	bgp_router_attr_buffer = NULL;
	free(bgp_edit_com_buffer);	bgp_edit_com_buffer = NULL;
	free(bgp_edit_attr_buffer);	bgp_edit_attr_buffer = NULL;
	free(bgp_router_com_buffer);	bgp_router_com_buffer = NULL;
	free(bgp_router_rrinfo_buffer);	bgp_router_rrinfo_buffer= NULL;
	free(bgppathtable);		bgppathtable = NULL;
	free(bgpupdatetable);		bgpupdatetable = NULL;
	free(bgp_gwcache);		bgp_gwcache = NULL;
	free(bgp_filter_list_cache);	bgp_filter_list_cache = NULL;
	free(bgp_route_map_cache);	bgp_route_map_cache = NULL;
	free(bgp_emergency_msg);	bgp_emergency_msg = NULL;
	free(bgp_neighbor_bitfield);	bgp_neighbor_bitfield = NULL;
	if (rcache_chunks) {
	    chunk_destroy(rcache_chunks);
	    rcache_chunks = NULL;
	}
	if (fcache_chunks) {
	    chunk_destroy(fcache_chunks);
	    fcache_chunks = NULL;
	}
	return (NULL);
    }

    bgp_emergency_msg->emergency = TRUE;

    if (pdb->mask)
	free_pdbindex(pdb);

    memset(pdb, 0, sizeof(pdbtype));

    pdb->name = "bgp";
    pdb->family = PDB_IP;
    pdb->pname = "BGP Router";

    pdb->process = bgp_router;
    pdb->cleanup = bgp_cleanup;
    pdb->neighbor = bgp_neighbor;
    pdb->showipproto = bgp_show_ip_proto;
    pdb->majorupdate = NULL;
    pdb->holddown = simple_holddown;
    pdb->query = bgp_nettable_query;
    pdb->add_route = bgp_nettable_add_req;
    pdb->redist_callback = bgp_redist_callback;
    pdb->distance_cb = bgp_distance;
    pdb->distance_cmp = iprouting_distcmp_generic;

    pdb->proctype = PROC_BGP;
    if (allocate_pdbindex(pdb) == -1)
	return (NULL);
    pdb->distflag = 0;       /* never can be NET_BGP */
    pdb->redistallowed = PROC_REDISTMASK;
    pdb->auto_summary = TRUE;
    pdb->broadcasttime = pdb->def_broadcasttime = BGP_SCANTIME;
    pdb->invalidtime = pdb->def_invalidtime = 0L;
    pdb->holdtime = pdb->def_holdtime = 0L;
    pdb->flushtime = pdb->def_flushtime = 0L;
    pdb->multipath = pdb->def_multipath = BGP_DEFAULT_MULTIPATH;
    pdb->bgp_keeptime = BGP_HOLDSECONDS/3;
    pdb->bgp_holdtime = BGP_HOLDSECONDS;
    pdb->distance = pdb->def_distance = BGP_EXTERNAL_DISTANCE;
    pdb->distance2 = pdb->def_distance2 = BGP_INTERNAL_DISTANCE;
    pdb->distance3 = pdb->def_distance3 = BGP_LOCAL_DISTANCE;
    pdb->asystem = asystem;
    pdb->atomic_update = TRUE;
    pdb->malloc_backup = FALSE;

    if (!checkqueue(&protoQ, pdb))
	enqueue(&protoQ, pdb);

    /* add a free list for default size net entry */
    mempool_add_free_list(MEMPOOL_CLASS_LOCAL, sizeof(bnettype)+BGP_INITPATHS);

    /* and one for BGP write buffers (these are big and chunky) buffers */
    mempool_add_free_list(MEMPOOL_CLASS_LOCAL, sizeof(bmsgtype)+BGP_MAXBYTES);
    mempool_add_free_list(MEMPOOL_CLASS_LOCAL, sizeof(bmsgtype)+BGP_UPDATESIZE);

    /* add one for BGP message type (used during update replication) */
    mempool_add_free_list(MEMPOOL_CLASS_LOCAL, sizeof(bmsgtype));

    bgp_reset(NULL, NULL);
    bgp_deconfig_dampening();
    bgp_dampening_enabled = FALSE;
    bgp_dampening_cfg_changed = FALSE;
    bgp_cluster_id = 0;
    bgp_client_reflection = TRUE;
    bgp_purge_cache = FALSE;
    bgp_dpa_enabled = FALSE;
    bgp_check_soft_reconfig = FALSE;
    bgp_multipath_enabled = FALSE;
    bgp_bitfield_size = BGP_INIT_BITFIELD_SIZE;
    bgp_bitfield_count = BGP_INIT_BITFIELD_SIZE << 3;
    TIMER_STOP(bgp_neighbor_index_timer);
    bgp_advertise_cond_change = FALSE;
    return (pdb);
}

/*
 * bgp_init
 *
 * Initialize BGP subsystem
 */
static void
bgp_init (subsystype *subsys)
{
    /*
     * Parser chain initialization
     */
    bgp_parser_init();

    /*
     * Initialize radix code
     */
    if (!rn_inithead((void **) &bgp_head, IPRADIXDATAOFFSET))
	errmsg(&msgsym(RADIXINIT, BGP));

    queue_init(&bgp_redistQ, 0);
    queue_init(&bgp_requestQ, 0);
    queue_init(&bgp_resetQ, 0);
    queue_init(&bgp_aggregateQ, 0);
    queue_init(&bgp_backdoorQ, 0);

    bgpaccepters_current = 0;
    bgpaccepters_max = BGP_MAX_ACCEPTERS;

    /*
     * Register some functions
     */
    reg_add_tcp_listen(BGP_PORT, bgp_open_passive_guts,
		       "bgp_open_passive_guts");
    reg_add_tcp_access_check(BGP_PORT, bgp_open_check, "bgp_open_check");
    reg_add_tcp_guess_md5_key(BGP_PORT, bgp_guess_md5_key,
			      "bgp_guess_md5_key");
    reg_add_ip_origin(bgp_ip_originas, "bgp_ip_originas");
    reg_add_ip_path(PROC_BGP, bgp_ip_path, "bgp_ip_path");
    reg_add_ip_aspath_access_list(bgp_aspath_access_list_callback,
				 "bgp_aspath_access_list_callback");
    reg_add_ip_access_list(bgp_access_list_callback,
			   "bgp_access_list_callback");
    reg_add_ip_interface_route_adjust(bgp_route_adjust, "bgp_route_adjust");
    reg_add_iprouting_pdb_init(PROC_BGP, bgp_pdb_init, "bgp_pdb_init");
    reg_add_ip_route_map_inform(bgp_route_map_inform, "bgp_route_map_inform");

    reg_add_route_map_match_nvgen(bgp_route_map_match_nvgen,
				  "bgp_route_map_match_nvgen");
    reg_add_route_map_set_nvgen(bgp_route_map_set_nvgen,
				"bgp_route_map_set_nvgen");
    reg_add_route_map_show_match(bgp_show_route_map_match,
				 "bgp_show_route_map_match");
    reg_add_route_map_show_set(bgp_show_route_map_set,
			       "bgp_show_route_map_set");
    pdb_BGP  = NULL;
}

/*
 * BGP subsystem header
 */

#define BGP_MAJVERSION 1
#define BGP_MINVERSION 0
#define BGP_EDITVERSION  1

SUBSYS_HEADER(bgp, BGP_MAJVERSION, BGP_MINVERSION,
	      BGP_EDITVERSION, bgp_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: iprouting, routing", "req: iprouting, routing");
