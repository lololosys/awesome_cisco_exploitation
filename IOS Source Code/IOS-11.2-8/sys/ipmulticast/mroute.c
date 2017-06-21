/* $Id: mroute.c,v 3.31.8.47 1996/09/13 22:52:48 lwei Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/mroute.c,v $
 *------------------------------------------------------------------
 * mroute.c - IP multicasting routing table support functions.
 *
 * November, 1993.
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This file contains a set of functions that manipulate the IP multicast
 * routing table.
 *------------------------------------------------------------------
 * $Log: mroute.c,v $
 * Revision 3.31.8.47  1996/09/13  22:52:48  lwei
 * CSCdi69140:  Fix per code review comments to CSCdi68465
 * Branch: California_branch
 *
 * Revision 3.31.8.46  1996/09/12  21:09:24  chwhite
 * CSCdi61325:  (*,G) JOINS received by the RP do not trigger (S,G) JOINS
 * to source
 * Branch: California_branch
 *
 * Revision 3.31.8.45  1996/09/10  19:01:15  dino
 * CSCdi68127:  static mroutes have competing default routes
 * Branch: California_branch
 *
 * Revision 3.31.8.44  1996/09/10  05:31:48  lwei
 * CSCdi68465:  RP information is too difficult to change in last-hop
 *              routers
 * Branch: California_branch
 *
 * Revision 3.31.8.43  1996/09/10  01:07:13  snyder
 * CSCdi68568:  more constant opportuniites
 *              140 from image, 52 from data
 * Branch: California_branch
 *
 * Revision 3.31.8.42  1996/09/04  17:01:33  dino
 * CSCdi67530:  Periodic duplicates occur on member LAN using sparse-mode.
 * Branch: California_branch
 *
 * Revision 3.31.8.41  1996/08/31  00:35:18  dino
 * CSCdi67770:  Improve readability of ipmbug().
 * Branch: California_branch
 *
 * Revision 3.31.8.40  1996/08/29  19:15:14  lwei
 * CSCdi67578:  Dense mode interface stays in pruned state when local
 *              member exists
 * Branch: California_branch
 *
 * Revision 3.31.8.39  1996/08/28  12:51:07  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.31.8.38  1996/08/21  01:21:12  lwei
 * CSCdi66170:  Sparse-dense/IGMP breaks triggered joins for sparse mode
 *              groups
 * Branch: California_branch
 *
 * Revision 3.31.8.37  1996/08/13  14:52:44  tylin
 * CSCdi61338:  ATM LEC sub-interf missing from PIM-MIB
 *    Incorporate with new ifmib_swidb_get() supporting function.
 * Branch: California_branch
 *
 * Revision 3.31.8.36  1996/08/09  03:19:17  mleelani
 * CSCdi61876:  (*,G) mcache entries do not work
 * Branch: California_branch
 * o Correct SPT threshold behaviour.
 * o Allow (*, G) fastswitching if on the shared tree.
 *
 * Revision 3.31.8.35  1996/08/07  23:50:36  dino
 * CSCdi65425:  Make SPARSE_FLAG setting more consistent amoung rtrs in a
 *              PIM domain
 * Branch: California_branch
 *
 * Revision 3.31.8.34  1996/08/07  22:15:41  chwhite
 * CSCdi63457:  static mroute to 0.0.0.0 entered incorrectly
 * Branch: California_branch
 *
 * Revision 3.31.8.33  1996/08/05  21:14:00  chwhite
 * CSCdi64249:  Integrate forwarding of mtrace reponses with mforward
 * Branch: California_branch
 *
 * Revision 3.31.8.32  1996/08/05  19:20:20  dino
 * CSCdi65068:  midb->mode is no longer needed. Configured mode need not
 *              ever change
 * Branch: California_branch
 *
 * Revision 3.31.8.31  1996/08/03  01:20:31  lwei
 * CSCdi60059:  rp entries are not timed out in the gdb
 * Branch: California_branch
 *
 * Revision 3.31.8.30  1996/07/19  15:01:51  snyder
 * CSCdi63480:  char * to char[], save a tiny bit
 *              36 bytes.
 * Branch: California_branch
 *
 * Revision 3.31.8.29  1996/07/02  05:32:07  lwei
 * CSCdi61586:  Circuit failure fall-over to secondary path can be improved
 * Branch: California_branch
 *
 * Revision 3.31.8.28  1996/06/27  07:59:55  mleelani
 * CSCdi61079:  Occasional traffic loss because of bogus DVMRP prunes
 * Branch: California_branch
 * o Dont send prune on non-RPF p2p link, if it does not appear in the OIF.
 * o Prevent bogus prune sent to RPF neighbor on tunnel when the dvmrp peer
 *   is a non-pruner.
 *
 * Revision 3.31.8.27  1996/06/26  22:34:24  tylin
 * CSCdi60050:  ip mroute command should accept Null0 as a valid interface
 * Branch: California_branch
 *
 * Revision 3.31.8.26  1996/06/26  00:01:33  chwhite
 * CSCdi61181:  crash in ip_rpf_check_mroute buginf
 * Branch: California_branch
 *
 * Revision 3.31.8.25  1996/06/24  23:33:44  chwhite
 * CSCdi58419:  <no ip pim rp> should clear the mr table
 * Branch: California_branch
 *
 * Revision 3.31.8.24  1996/06/21  23:12:19  lwei
 * CSCdi60915:  (*,G) iif, if in dense mode should be added to (S,G) olist
 *              as needed
 * Branch: California_branch
 *
 * Revision 3.31.8.23  1996/06/20  01:12:54  lwei
 * CSCdi60852:  Back out fixes to CSCdi58281
 * Branch: California_branch
 *
 * Revision 3.31.8.22  1996/06/19  07:20:06  mleelani
 * CSCdi60351:  DVMRP routes are preferred over Directly connected routes
 * Branch: California_branch
 * Incorrect RPF lookup for connected routes.
 *
 * Revision 3.31.8.21  1996/06/12  04:18:21  mleelani
 * CSCdi60190:  Source mpackets not logged when vty originates from the
 * same source.
 * Branch: California_branch
 * Log all mpackets.
 *
 * Revision 3.31.8.20  1996/06/12  03:56:10  mleelani
 * CSCdi58720:  Multicast fastswitching does not do fast drop.
 * Branch: California_branch
 * DO fast drop.
 *
 * Revision 3.31.8.19  1996/06/08  04:11:59  lwei
 * CSCdi59415:  mdb timer incorrectly shrank at data forwarding time
 * Branch: California_branch
 *
 * Revision 3.31.8.18  1996/06/07  21:17:42  lwei
 * CSCdi59660:  Improve time needed to switch over to an alternate path at
 *              the RP
 * Branch: California_branch
 *
 * Revision 3.31.8.17  1996/05/28  20:33:18  jzwiebel
 * CSCdi58281:  Offset timers keep S,G state alive in RP and neighbor
 * routers
 * Branch: California_branch
 *
 * Revision 3.31.8.16  1996/05/22  20:55:03  raj
 * CSCdi56721:  Debug ip mpacket doesnt show ttl and other important fields
 * Branch: California_branch
 * Added fields for "detail", and specifying acl as well as group.
 *
 * Revision 3.31.8.15  1996/05/21  22:21:47  lwei
 * CSCdi58021:  DVMRP prune for default route was not processed
 * Branch: California_branch
 *
 * Revision 3.31.8.14  1996/05/21  21:06:41  lwei
 * CSCdi57811:  Back out longest match routing made by CSCdi45503
 * Branch: California_branch
 *
 * Revision 3.31.8.13  1996/05/13  20:10:22  mleelani
 * CSCdi57016:  Multicast helper-maps not functional
 * Branch: California_branch
 * Fix helper-maps.
 *
 * Revision 3.31.8.12  1996/05/08  08:20:52  mleelani
 * CSCdi56507:  removing --igmp join-group-- from interface does not
 * remove mr entry
 * Branch: California_branch
 * Schedule (*, G) expiry if local members are no longer present.
 *
 * Revision 3.31.8.11  1996/05/08  04:29:44  lwei
 * CSCdi54559:  IP multicast is sending larger than MTU sized packets over
 * GRE
 * Branch: California_branch
 *
 * Revision 3.31.8.10  1996/04/24  05:28:10  lwei
 * CSCdi54097:  A DR should send registers for pure senders
 * Branch: California_branch
 *
 * Revision 3.31.8.9  1996/04/20  01:51:06  dino
 * CSCdi54104:  CGMP must support source-only systems in switched Cat5000
 *              network
 * Branch: California_branch
 *
 * Revision 3.31.8.8  1996/04/17  13:42:29  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.31.8.7  1996/04/15  21:17:32  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.31.8.6  1996/04/05  23:35:10  dino
 * CSCdi53826:  <show ip rpf> should not accept a group address
 * Branch: California_branch
 *
 * Revision 3.31.8.5  1996/04/04  04:55:15  dino
 * CSCdi53046:  DR doesnt register when on shared-tree and spt-threshold
 *              configured
 * Branch: California_branch
 *
 * Revision 3.31.8.4  1996/04/03  22:04:11  mleelani
 * CSCdi53495:  Crash in ip_mroute_count when the number of groups is very
 * large
 * Branch: California_branch
 * Deletion of mroute may leave cache ptr in limbo
 *
 * Revision 3.31.8.3  1996/03/27  22:17:50  mleelani
 * CSCdi51176:  Crash during show ip mcache
 * Branch: California_branch
 * Do not cache the OIF before blocking.
 *
 * Revision 3.31.8.2  1996/03/27  21:58:05  mleelani
 * CSCdi49709:  Broadcast/multicast translation using helpers not
 * functional.
 * Branch: California_branch
 * Use helper addresses to provide the translation
 *
 * Revision 3.31.8.1  1996/03/18  20:18:57  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.23.2.4  1996/03/16  06:55:16  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.23.2.3  1996/03/13  01:38:42  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.23.2.2  1996/03/07  09:48:09  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.23.2.1  1996/02/20  01:01:29  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.31  1996/02/29  03:38:34  dino
 * CSCdi50307:  IP multicast maps to incorrect mac address when LANE is
 *              over ATM
 *
 * Revision 3.30  1996/02/22  09:03:11  dino
 * CSCdi49566:  Allow control over which RPs are used in transit routers.
 *
 * Revision 3.29  1996/02/16  19:42:57  dino
 * CSCdi49202:  Users need an easy way to determine RPF failures due to
 *              misconfig
 *
 * Revision 3.28  1996/02/15  22:12:18  dino
 * CSCdi49131:  I broke static mroutes again - cant do longest match on
 *              them
 *
 * Revision 3.27  1996/02/08  03:39:24  lwei
 * CSCdi48369:  IP multicast does not support ip pim sparse-dense
 *              configuration
 *
 * Revision 3.26  1996/02/07  09:31:38  dino
 * CSCdi48313:  mtrace still doesnt return correc errors
 *
 * Revision 3.25  1996/02/02  21:27:05  dino
 * CSCdi46657:  Mroutes that RPF from static mroutes could have bogus RPF
 *              nbr addr
 *
 * Revision 3.24  1996/01/26  20:50:36  dino
 * CSCdi47102:  ip_mroute dereferences NULL
 *
 * Revision 3.23  1996/01/17  07:22:00  dino
 * CSCdi47014:  Static default mroutes are not preferred for direct
 *              connect majornet
 *
 * Revision 3.22  1996/01/13  02:53:03  dino
 * CSCdi46656:  (*,G) oif null but (S,G) oif non-null causes Join
 *              suppression
 *
 * Revision 3.21  1995/12/30  20:43:13  dino
 * CSCdi46247:  Code review comments for CSCdi44742 (SPT threshold feature)
 *
 * Revision 3.20  1995/12/23  00:02:34  dino
 * CSCdi44155:  Logic for RP to prune (S,G) needs to consider DM interfaces
 *
 * Revision 3.19  1995/12/21  20:44:24  dino
 * CSCdi45700:  Dense-mode forwarding router doesnt unprune quickly for
 *              new member
 *
 * Revision 3.18  1995/12/17  19:24:20  dino
 * CSCdi44742:  PIM should only switch to the SPT when traffic threshold
 *              is reached
 *
 * Revision 3.17  1995/12/15  22:02:36  dino
 * CSCdi45677:  DVMRP default route not used when recursive default
 *              unicast exists
 *
 * Revision 3.16  1995/12/14  09:55:00  dino
 * CSCdi45564:  Users are complaining that show ip pim interface is
 *              unreadable
 *
 * Revision 3.15  1995/12/13  22:23:40  dino
 * CSCdi45503:  Less specific DVMRP route preferred over more specific
 *              unicast route
 *
 * Revision 3.14  1995/12/13  07:03:15  dino
 * CSCdi45504:  (S,G) entries are getting DVMRP prune that dont match
 *              aggregate
 *
 * Revision 3.13  1995/12/13  06:32:51  dino
 * CSCdi45503:  Less specific DVMRP route preferred over more specific
 *              unicast route
 *
 * Revision 3.12  1995/12/07  03:29:01  dino
 * CSCdi45160:  DVMRP radix tree is not matching on default route
 *
 * Revision 3.11  1995/12/01  18:38:41  lwei
 * CSCdi44737:  IP multicast does not support administratively scoped
 * boundaries
 *
 * Revision 3.10  1995/11/29  23:23:29  dino
 * CSCdi44667:  ip_get_worst_mdb() returns root node.
 *
 * Revision 3.9  1995/11/29  00:00:58  mleelani
 * CSCdi43368:  7000 crash in ip_get_mdb
 * Add mroute message facility. Make sure the radix entry is
 * deleted before freeing the mdb. Fix memory leaks.
 *
 * Revision 3.8  1995/11/28  22:37:52  mleelani
 * CSCdi44147:  Crash in free() during show ip mroute
 * Radix head getting freed while in use.
 *
 * Revision 3.7  1995/11/22  22:20:13  dino
 * CSCdi44336:  mtrace interface counters not properly counted when
 * partially fs
 *
 * Revision 3.6  1995/11/22  07:01:06  dino
 * CSCdi43351:  rsvp PATH packet gets looped back with PIM sparse
 *
 * Revision 3.5  1995/11/22  06:57:23  dino
 * CSCdi43574:  mtrace gets native interface instead of tunnel when native
 * not mcast
 *
 * Revision 3.4  1995/11/21  23:12:06  dino
 * CSCdi43427:  cisco does not process DVMRP prunes.
 *
 * Revision 3.3  1995/11/17  17:30:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:42:13  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:01:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.27  1995/10/12  18:06:16  lwei
 * CSCdi41171:  PIM failed to send Prune for unwanted (S,G)
 *
 * Revision 2.26  1995/10/10  21:16:02  dino
 * CSCdi41878:  Monitor number of DVMRP routes received and errmsg()
 * during surges.
 *
 * Revision 2.25  1995/10/06  23:04:45  lwei
 * CSCdi41753:  Multicast traffic statistics display lacks up-to-date mean
 * values
 *
 * Revision 2.24  1995/10/05  18:35:02  lwei
 * CSCdi41573:  <show ip mroute active> should show last seconds data not
 * current
 *
 * Revision 2.23  1995/09/28  02:18:53  dino
 * CSCdi41187:  Mtrace is broken in a couple of scenarios and causes
 *              spurious errors
 *
 * Revision 2.22  1995/09/20  23:47:15  dino
 * CSCdi40756:  show ip mroute active should display sd name on group line
 *
 * Revision 2.21  1995/09/20  17:05:28  dino
 * CSCdi36738:  Multicast sender being designated router
 *
 * Revision 2.20  1995/09/19  16:30:23  dino
 * CSCdi40348:  spurious access in ip_mroute_command
 *
 * Revision 2.19  1995/09/19  07:06:33  dino
 * CSCdi40341:  static mroute fails to recursively look up RPF
 *
 * Revision 2.18  1995/09/08  08:11:35  dino
 * CSCdi39938:  PIM MIB displays tunneling for registering
 *
 * Revision 2.17  1995/09/06  07:06:32  dino
 * CSCdi39769:  Modularity: mroute.c has direct calls into mib subsystem
 *
 * Revision 2.16  1995/09/03  23:55:57  richl
 * CSCdi39765:  Modularity: modular-ip broken in nightly build
 *
 * Revision 2.15  1995/08/17  01:58:38  dino
 * CSCdi30823:  PIM Assert metrics use 0 for directly connected sources.
 *
 * Revision 2.14  1995/08/01  06:39:39  dino
 * CSCdi37832:  PIM prunes olist when null but there is local membership.
 *
 * Revision 2.13  1995/08/01  04:34:42  dino
 * CSCdi38008:  no ip multicast rate-limit out wipes out other entries too.
 *
 * Revision 2.12  1995/07/29  01:56:02  dino
 * CSCdi36878:  Fast multicast bypasses access lists, accounting, policy
 *              routing
 *
 * Revision 2.11  1995/07/26  02:12:42  dino
 * CSCdi37666:  Got interrupt before Nulling out freeed data structure.
 *
 * Revision 2.10  1995/07/24  07:33:39  hampton
 * Transition IP Multicast support to use the passive timer macros for all
 * its timers.  It no longer references the system clock directly.
 * [CSCdi37539]
 *
 * Revision 2.9  1995/07/17 08:28:58  dino
 * CSCdi37181:  Need to show active multicast sources - show ip mroute
 * active.
 *
 * Revision 2.8  1995/07/17  07:32:49  bchan
 * CSCdi34760:  Ifindex usage incorrect
 *
 * Revision 2.7  1995/07/14  01:00:18  dino
 * CSCdi36055:  IP multicast doesnt check malloc return code
 *
 * Revision 2.6  1995/07/12  01:16:03  dino
 * CSCdi36958:  All ip mroutes are not examined when doing RPF lookups.
 *
 * Revision 2.5  1995/07/10  18:40:01  dino
 * CSCdi36862:  Do faster pruning
 *
 * Revision 2.4  1995/07/01  07:46:13  dino
 * CSCdi27921:  Border router needs to send Registers to RP. ISPs want PIM
 *              over DVMRP unicast routing.
 *
 * Revision 2.3  1995/06/30  18:13:28  schrupp
 * CSCdi36558:  Spurious accesses on ATM signalling and IP multicast code
 *
 * Revision 2.2  1995/06/10  03:53:49  dino
 * CSCdi35623:  EFT bugs found during 11.0 integration period.
 *
 * Revision 2.1  1995/06/07  21:04:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include <stdarg.h>
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "../ipmulticast/msg_mroute.c"	/* Not a typo, see logger.c */
#include "config.h"
#include "name.h"
#include "ttysrv.h"
#include "access.h"
#include "../if/network.h"
#include "../if/atm.h"
#include "../ui/debug.h"
#include "../lane/lane.h"
#include "../ip/ip.h"
#include "../ip/ip_debug.h"
#include "../ip/ipaccess.h"
#include "../ip/ip_registry.h"
#include "../ip/tunnel.h"
#include "../ip/gre.h"
#include "../iprouting/route.h"
#include "../iprouting/route_map.h"
#include "../ipmulticast/ipmulticast_debug.h"
#include "../ipmulticast/mroute.h"
#include "../ipmulticast/igmp.h"
#include "../ipmulticast/pim.h"
#include "../ipmulticast/dvmrp.h"
#include "../ipmulticast/sr_ipmroutemib.h"
#include "../ipmulticast/sd.h"
#include "../ipmulticast/cgmp.h"
#include "media_registry.h"


/*	
 * Storage.
 */
gdbtype *ip_mroute_cache[NETHASHLEN];
uchar *const midb_state_str[MIDB_MAX_STATES] = {"Null", "Forward", "Prune"};
static sys_timestamp ip_rpf_check_time;
boolean ip_mroute_fs_change;
static mroutetype *ip_static_mroutes;

/*
 * For multicast dups debugging.
 */
ipmpackettype *ip_mpacket_cache;
ulong ip_mpacket_next;
    

/*
 * ip_multicast_command
 *
 * Process "ip multicast-routing" command.
 */
void ip_multicast_command (parseinfo *csb)
{

    if (csb->nvgen) {
	nv_write(ip_mrouter_running, "%s", csb->nv_command);
	return;
    }
   
    if (!(ip_mrouter_running = csb->sense)) ip_clear_mroute_cache(0, NULL);
}

/*
 * ip_multicast_cache_headers_command
 *
 * Process the "ip multicast cache-headers" command.
 */
void ip_multicast_cache_headers_command (parseinfo *csb)
{
    void *temp;

    /*
     * NV generation.
     */
    if (csb->nvgen) {
	nv_write((boolean) ip_mpacket_cache, "%s", csb->nv_command);
	return;
    }

    if (csb->sense) {

	/*
	 * Already enabled.
	 */
	if (ip_mpacket_cache) return;
	ip_mpacket_cache = malloc(sizeof(ipmpackettype) * 
				  IP_MPACKET_CACHE_SIZE);
	if (!ip_mpacket_cache) {
	    printf("\nNo memory");
	    return;
	}
	ip_mpacket_next = 0;
    } else {
	temp = ip_mpacket_cache;
	ip_mpacket_cache = NULL;
	free(temp);
    }
}

/*
 * ip_show_mpacket
 *
 * Process the command "show ip mpacket".
 */
void ip_show_mpacket (parseinfo *csb)
{

    ipmpackettype *mpacket, *old, *new;
    nametype      *namep;
    iphdrtype     *ip;
    ulong         next, index;
    ipaddrtype    group, source, grp, src;
    boolean       detail;

    if (!ip_mpacket_cache) {
	printf("\nIP multicast header caching is disabled");
	return;
    }

    src = GETOBJ(paddr,1)->ip_addr;
    if (IPMULTICAST(src)) {
	grp = src;
	src = 0;
    } else {
	grp = GETOBJ(paddr,2)->ip_addr;
    }
    detail = GETOBJ(int,1);

    /*
     * Get new cache buffer so switching can continue to store packets while
     * we print out this buffer.
     */
    new = malloc(sizeof(ipmpackettype) * IP_MPACKET_CACHE_SIZE);
    if (!new) {
	printf("\nNo memory");
	return;
    }

    next = ip_mpacket_next;
    old = ip_mpacket_cache;
    ip_mpacket_cache = new;
    ip_mpacket_next = 0;

    /*
     * Print buffer.
     */
    automore_enable(NULL);
    printf("\nIP Multicast Header Cache - entry count: %d, next index: %d", 
	   (next) ? next-1 : 0, next & IP_MPACKET_SIZE_MASK);
    printf("\nKey: id/ttl timestamp (name) source group\n");

    mpacket = old;
    next = (next & IP_MPACKET_SIZE_MASK);
    index = next;
    do {
	if (automore_quit()) break;
	ip = &mpacket->ip;
	source = GETLONG(&ip->srcadr);
	group = GETLONG(&ip->dstadr);
	if ((!grp || grp == group) && (!src || src == source)) {

	    /*
	     * There is something at this cached location and we want to
	     * print it.
	     */
	    if (ip->version) {
		printf("\n%04x/%3d %lTa ", ip->id, ip->ttl, 
		       mpacket->timestamp);
		namep = reg_invoke_ip_address_lookup(source);
		printf("(%s) %i %i", (namep) ? namep->name : "?", source, 
		       group);
		if (detail) {
		    printf("\n  tos: 0x%x, tl: %d, prot: %d, ports %d/%d", 
			   ip->tos, ip->tl, ip->prot, mpacket->sport, 
			   mpacket->dport);
		}
	    }
	}
	mpacket++;
	index++;
    } while ((index & IP_MPACKET_SIZE_MASK) != next);

    automore_disable();
    free(old);
}

/*
 * ipmulticast_ttl_command
 *
 * Process "ip multicast ttl-threshold <ttl>" interface command.
 */
void ipmulticast_ttl_command (parseinfo *csb)
{

    idbtype *idb;

    idb = csb->interface;
    if (csb->nvgen) {
	nv_write(idb->ip_multicast_threshold, "%s %d", csb->nv_command,
		 idb->ip_multicast_threshold);
	return;
    }
    idb->ip_multicast_threshold = GETOBJ(int,1);
}

/*
 * ip_multicast_boundary_command
 *
 * Process "ip multicast boundary <access-list>" interface command.
 */
void ip_multicast_boundary_command (parseinfo *csb)
{
    idbtype *idb;
    acl_headertype *acl;

    idb = csb->interface;
    if (csb->nvgen) {
	
	if (idb->ipmulticast_boundary)
	    nv_write(TRUE, "%s %s", csb->nv_command,
		     idb->ipmulticast_boundary->name);
	return;
    }

    if (csb->sense) {
	acl = access_parse_acl(GETOBJ(int,1), GETOBJ(string,1),
			       GETOBJ(int,1) == 0, ACL_IP_NAMED_SIMPLE);
	if (!acl)
	    return;

        idb->ipmulticast_boundary = acl;

	/*
	 * Clearing all existing multicast entries denied by this acl.
	 */
	ip_clear_mroute_cache(0, acl);
    } else {
	idb->ipmulticast_boundary = NULL;
    }
}

/*
 * ip_mroute_command
 *
 * [no] ip mroute <source> [<mask>] | [<protocol><as-number>] 
 *                <rpf-address> | <interface> [<distance>]
 *
 * Configure multicast static routes.
 */
void ip_mroute_command (parseinfo *csb)
{

    idbtype    *idb;
    pdbtype    *pdb;
    uchar      *route_map;
    ipaddrtype source, mask, rpf_address;
    uchar      distance;
    int        pdb_type, len;
    mroutetype *mroute, *last_mroute;

    /*
     * NV generation.
     */
    if (csb->nvgen) {
	for (mroute = ip_static_mroutes; mroute; mroute = mroute->next) {
	    nv_write(TRUE, "%s %i %i", csb->nv_command, mroute->source,
		     mroute->mask);
	    if (mroute->pdb) {
		nv_add(TRUE, " %s", mroute->pdb->name);
		nv_add(mroute->pdb->asystem, " %d", mroute->pdb->asystem);
	    }
	    if (mroute->route_map) {
		nv_add(TRUE, " route-map %s", mroute->route_map);
	    }
	    if (mroute->idb) {
		nv_add(TRUE, " %s", mroute->idb->namestring); 
	    } else {
		nv_add(TRUE, " %i", mroute->rpf_address);
	    }
	    if (mroute->distance) nv_add(TRUE, " %d", mroute->distance);
	}
	return;
    }

    source = GETOBJ(paddr,1)->ip_addr;
    mask = GETOBJ(paddr,2)->ip_addr;
    distance = GETOBJ(int,1);
    idb = GETOBJ(idb,1);
    rpf_address = GETOBJ(paddr,3)->ip_addr;
    pdb_type = PDB_IP;
    pdb = router_parse(csb, TRUE, &pdb_type);
    route_map = GETOBJ(string,1);
    if (!*route_map) route_map = NULL;

    if (idb && !is_p2p(idb) && (idb != nullidb)) {
	printf("Next hop IP address required for multi-access media.\n");
	return;
    }

    /*
     * Find existing entry. Remember tail entry in case of adding a new
     * entry or previous entry in case of deleting an existing entry.
     */
    last_mroute = (mroutetype *) &ip_static_mroutes;
    for (mroute = ip_static_mroutes; mroute; mroute = mroute->next) {
	if (mroute->source == source && mroute->mask == mask &&
	    mroute->pdb == pdb && ((!mroute->route_map && !route_map) || 
				   (mroute->route_map && route_map && 
				   !strcmp(mroute->route_map, route_map)))) {
	    break;
	}
	last_mroute = mroute;
    }

    if (csb->sense) {
	if (!mroute) {
	    mroute = malloc(sizeof(mroutetype));
	    if (!mroute) return;

	    last_mroute->next = mroute;
	    mroute->mask = mask;
	    mroute->source = source & mroute->mask;
	    mroute->pdb = pdb;
	    if (route_map) {
		len = strlen(route_map) + 1;
		mroute->route_map = malloc(len);
		if (mroute->route_map) {
		    bcopy(route_map, mroute->route_map, len);
		}
	    }
	}
	mroute->idb = idb;
	mroute->rpf_address = rpf_address;
	mroute->distance = distance;
    } else {
	if (!mroute) {
	    printf("\nEntry does not exist");
	    return;
	}
	last_mroute->next = mroute->next;
	mroute->next = NULL;
	if (mroute->route_map) {
	    free(mroute->route_map);
	    mroute->route_map = NULL;
	}
	free(mroute);
    }
}

/*
 * ip_add_mroute
 *
 * Add multicast route entry to table. This function can add an (S,G) or
 * (*,G) entry. If source is 0.0.0.0 and mask is 255.255.255.255, this
 * specifies a (*,G) entry. The idb is the expected incoming interface for
 * the entry. To add outgoing interfaces to an entry, ip_add_mroute_idb() or
 * ip_build_outputQ() should be called.
 */
mdbtype *ip_add_mroute (ipaddrtype group, ipaddrtype source, ipaddrtype mask,
			idbtype *idb, ipaddrtype rpf_nbr, uchar rpf_type, 
			ulong expire_time)
{
    gdbtype   *gdb;
    mdbtype   *mdb;
    boolean   wildcard;
    int       hash;
    leveltype level;
    rntype    *rn;

    SAVE_CALLER();

    /*
     * This functions only support Class D IP addresses. The reserved
     * range are not stored in the multicast routing table.
     */
    if (!IPMULTICAST(group) || IPMULTICAST_RSVD(group)) return(NULL);

    wildcard = (!source && (int) mask == MASK_HOST);

    /*
     * Find group entry, if not found, allocate one. Each group has
     * a radix tree for all sources associated with group.
     */
    gdb = ip_get_gdb(group);
    if (!gdb) {
	gdb = malloc(sizeof(gdbtype));
	if (!gdb) return(NULL);
	if (!rn_inithead((void **) &gdb->radix_head, IPRADIXDATAOFFSET)) {
	    errmsg(&msgsym(RADIXINIT, MROUTE), group);
	    free(gdb);
	    return(NULL);
	}
	gdb->mdb.group = group;
	GET_TIMESTAMP(gdb->mdb.uptime);
	TIMER_STOP(gdb->mdb.expires);
	gdb->mdb.in_limit = (int) -1;
	
	if (expire_time) {
	    TIMER_START(gdb->mdb.expires, expire_time);
	}
	ip_store_mdb_timer(&gdb->mdb);
	
	/*
	 * Add to SNMP table before putting in table.
	 */	
	reg_invoke_snmp_add_ip_group(group);

	/*
	 * Set Join SPT default to FALSE. The mroute ager
	 * will provide the correct setting later.
	 */
	gdb->join_spt = FALSE;

	/*
	 * Link in hash table.
	 */
	hash = nethash(group);
	gdb->next = ip_mroute_cache[hash];
	ip_mroute_cache[hash] = gdb;
	MROUTE_BUGINF(group, ("\nMRT: Create (*, %i)", group));

    } else {
	if (wildcard) MROUTE_BUGINF(group, ("\nMRT: Update (*, %i)", group));
    }

    /*
     * Build (*,G) part of entry.
     */
    if (wildcard) {
	TIMER_STOP(gdb->mdb.expires);
	if (expire_time) TIMER_START(gdb->mdb.expires, expire_time);
	ip_store_mdb_timer(&gdb->mdb);
	
	/*
	 * Wildcard entries, have NULL incoming interface, therefore there
	 * is no RPF neighbor.
	 *
	 * gdb->mdb.if_input = idb;
	 * gdb->mdb.rpf_nbr = 0;
	 */
	MROUTE_BUGINF(gdb->mdb.group, (", RPF Null, PC 0x%x", caller()));
	return(&gdb->mdb);
    }

    /*
     * Find source associated with group. Add source entry to tree if one
     * does not exist.
     */
    mdb = ip_get_mdb(gdb, source, mask);
    if (!mdb) {
	mdb = malloc(sizeof(mdbtype));
	if (!mdb) return(NULL);
	mdb->group = group;
	GET_TIMESTAMP(mdb->uptime);
	ip_addr2radix(source, mdb->source);
	ip_addr2radix(mask, mdb->mask);
	mdb->metric_pref = -1;
	mdb->metric = -1;

	level = raise_interrupt_level(NETS_DISABLE);
	rn = rn_addroute(mdb->source, mdb->mask,
			 gdb->radix_head, mdb->radix_nodes);
	reset_interrupt_level(level);

	if (rn != (rntype *) mdb) {
            errmsg(&msgsym(RADIXINSERT, MROUTE), ip_radix2addr(mdb->source),
                   ip_bitsinmask(ip_radix2addr(mdb->mask)), mdb->group,
                   mdb, rn);
	    free(mdb);
	    return(NULL);
	}

	MROUTE_BUGINF(group, ("\nMRT: Create"));
	TIMER_STOP(mdb->expires);
	if (expire_time) {
	    TIMER_START(mdb->expires, expire_time);
	}
	ip_store_mdb_timer(mdb);
	
	/*
	 * Check if we are rate-limiting input.
	 */
	mdb->in_limit = ipmulticast_rate_limit(mdb, NULL, idb, TRUE);
    } else {
	MROUTE_BUGINF(group, ("\nMRT: Update"));
    }

    MROUTE_BUGINF(group, (" (%i/%d, %i), RPF %s/%i, PC 0x%x",
		  source, ip_bitsinmask(mask), group, idb->namestring, 
		  rpf_nbr, caller()));

    /*
     * Update fields.
     */
    mdb->if_input = idb;
    mdb->rpf_nbr = rpf_nbr;
    mdb->rpf_type = rpf_type;
    mdb->flags = gdb->mdb.flags & (CONNECTED_FLAG | LOCAL_FLAG);

    /*
     * If the source is directly connected, we have CGMP configured on the
     * RPF interface, are the IGMP querier for the interface, and there 
     * are no directly connected members for this group, we need to send a 
     * CGMP Join message. This is the case where there is a source-only system
     * in a switched network of Cat5000s and we want those packets to come to 
     * the router(s) (opposed to being flooded throughout the switched
     * network).
     */
    if (idb->cgmp_enabled && igmp_getidb(idb)->multicast_qr) {
	if (!igmp_find_group(group, idb) && ip_address_on_idb(source, idb)) {
	    cgmp_send_self(CGMP_JOIN_TYPE, idb, group);
	}
    }
    return(mdb);
}

/*
 * ip_delete_mroute
 *
 * Delete multicast entry from routing table. A specific (S,G) will be
 * deleted, unless the (*,G) entry is passed in. In this case, all (Si,G)
 * are deleted first then the (*,G) is deleted next.
 */
void ip_delete_mroute (gdbtype *gdb, mdbtype *mdb)
{

    gdbtype *prev;
    boolean wildcard;
    rntype *rn;
    leveltype level;

    SAVE_CALLER();
    wildcard = (&gdb->mdb == mdb);

    /*
     * Free all malloc'ed data from entry.
     */
    ip_mfree_outputQ(&mdb->if_outputQ);

    /*
     * Delete (S,G) entry.
     */
    if (!wildcard) {
	MROUTE_BUGINF(mdb->group, ("\nMRT: Delete (%i/%d, %i), PC 0x%x", 
	   ip_radix2addr(mdb->source), ip_bitsinmask(ip_radix2addr(mdb->mask)),
	   mdb->group, caller()));

	level = raise_interrupt_level(NETS_DISABLE);
	rn = rn_delete(mdb->source, mdb->mask, gdb->radix_head);
	reset_interrupt_level(level);

	if (rn == (rntype *) mdb) {
	    free(mdb);
	} else {
	    errmsg(&msgsym(RADIXDELETE, MROUTE), ip_radix2addr(mdb->source),
		   ip_bitsinmask(ip_radix2addr(mdb->mask)), mdb->group,
		   mdb, rn);
	}
	return;
    }

    /*
     * Free malloc'ed elements in the gdb.
     */
    ip_clear_mroute_radix(gdb);

    /*
     * Delete from SNMP table before delink from table.
     */
    reg_invoke_snmp_delete_ip_group(gdb->mdb.group);

    /*
     * Delink gdb from hash table.
     */
    for (prev = (gdbtype *) &ip_mroute_cache[nethash(mdb->group)]; prev->next;
	 prev = prev->next) {
	if (prev->next == gdb) {
	    prev->next = gdb->next;
	    gdb->next = NULL;
	    break;
	}
    }
    MROUTE_BUGINF(gdb->mdb.group, ("\nMRT: Delete (*, %i), PC 0x%x", 
				   gdb->mdb.group, caller()));
    free(gdb);
}

/*
 * ip_add_mroute_idb
 *
 * Add an interface to the outgoing interface list for a given multicast
 * route entry.
 */
void ip_add_mroute_idb (gdbtype *gdb, mdbtype *mdb, idbtype *idb,
			ipaddrtype next_hop, ulong expire_time, ulong state,
			boolean search)
{

    midbtype *midb;
    ipaddrtype group;
    idbtype *rpf_idb;
    ipaddrtype source;
    ipaddrtype mask;
    uchar rpf_type;
    int idb_mtu;

    if (!gdb) return;

    /*
     * Never add interface to outgoing list, if it is the same as the
     * incoming interface, unless we are in NBMA mode and the next_hop
     * is not the same as the rpf_nbr.
     */
    if (mdb->if_input == idb) {
	if (!idb->pim_nbma_mode) return;
	if (mdb->rpf_nbr == next_hop) return;
    }

    /*
     * If we are an administratively scoped boundary for the group,
     * Don't add this midb.
     */
    group = mdb->group;

    if (ipmulticast_test_boundary(idb, group)) {
	MROUTE_BUGINF(mdb->group,
		   ("\nMRT: Add %s to olist for %i rejected at boundary", 
                   idb->namestring, mdb->group));
        return;
    }

    /*
     * Check whether the incoming interface is an admininistratively
     * scoped boundary.
     */
    rpf_idb = mdb->if_input;
    if (!rpf_idb) {

        /*
         * This is a (*,G) entry.
         * If sparse mode, find the idb toward the 1st RP.
         * else find the idb toward the source.
         * Check if that idb is an administratively scoped boundary.
         */
	if (IS_SPARSE_GROUP(gdb)) {
	    rpf_idb = ip_rpf_lookup(gdb->rp.address, MASK_HOST, NULL,
                                    NULL, NULL, &rpf_type, ROUTINGDEPTH);
	} else {
	    source = ip_radix2addr(mdb->source);
	    mask = ip_radix2addr(mdb->mask);
	    rpf_idb = ip_rpf_lookup(source, mask, NULL, NULL, NULL,
				    &rpf_type, ROUTINGDEPTH);
        }
    }

    if (ipmulticast_test_boundary(rpf_idb, group)){
	MROUTE_BUGINF(mdb->group,
		   ("\nMRT: Add %s to olist for %i rejected at boundary", 
                   idb->namestring, mdb->group));
	return;
    }

    /*
     * Only search existing list if requested by caller.
     */
    midb = NULL;
    if (search) {
	midb = ip_get_midb(&mdb->if_outputQ, idb, next_hop, 
			   IS_SPARSE_GROUP(gdb));
    }

    if (!midb) {

	/*
	 * Don't add interface when next-hop is 0.0.0.0 and interface is in 
	 * NBMA-mode.
	 */
	if (!next_hop && idb->pim_nbma_mode) {
	    if (IS_SPARSE_GROUP(gdb)) return;
	}

	midb = malloc(sizeof(midbtype));
	if (!midb) return;
	midb->idb = idb;
	midb->next_hop = mdb->group;
	GET_TIMESTAMP(midb->uptime);

	/*
	 * Use ALLROUTERS address to minimize map and dialer-list commands.
	 * Distinguish between an ATM WAN and a emulated LAN over ATM.
	 * Don't put a next hop address on an NBMA interface for
	 * dense-mode groups (sparse-dense).
	 */
	if (IS_SPARSE_GROUP(gdb) && (is_p2p(idb) || is_wan(idb->hwptr))) {
	    if (!is_atm_lane(idb)) midb->next_hop = ALLROUTERS;
	}
	
	/*
	 * If fastswitching enabled, put interface at beginning of list.
	 * If fastswitching is disabled, put at end of list. Fastswitching 
	 * will process each interface at beginning of queue with a 
	 * midb->mac_header. The first NULL midb->mac_header found is left 
	 * for process level to finish the list.
	 */
	if (midb->idb->ip_multicast_fs) {
	    if (!mdb->if_outputQ.count) mdb->flags |= FAST_FLAG;
	    requeue(&mdb->if_outputQ, midb);

	    /*
	     * Store the minimal MTU among the existing outgoing
	     * interfaces.
	     */
	    idb_mtu = idb->ip_mtu;
	    if (!mdb->ip_min_mtu || idb_mtu < mdb->ip_min_mtu) {
		mdb->ip_min_mtu = idb_mtu;
	    }
	} else {
	    mdb->flags &= ~FAST_FLAG;
	    enqueue(&mdb->if_outputQ, midb);
	}

	/*
	 * Check to see if rate-limit is configured for this interface for
	 * group and source.
	 */
	midb->out_limit = ipmulticast_rate_limit(mdb, midb, idb, FALSE);
    }

    TIMER_START(midb->expires, expire_time);

    /*
     * Get a proper maximum expiration time among all interfaces, and
     * keep that in mdb->expiration_time. The next packet forwarded
     * will refresh the mdb entry with the correrct expiration time.
     */
    ip_store_mdb_timer(mdb);

    /*
     * Use next-hop IP address if supplied by calling if this outgoing 
     * interface is in NBMA mode and the group is sparse mode.
     */
    if (midb->idb->pim_nbma_mode && next_hop && IS_SPARSE_GROUP(gdb)) {
	midb->next_hop = next_hop;
    }

    /*
     * Caller wants to change state.
     */
    if (state != MIDB_NULL_STATE) midb->state = state;

    /*
     * Whenever transitioning to Forward state, set the forwarder flag in
     * the midb. The midb->forwarder flag is only tested and used when
     * the midb is in Prune state (for Assert transmission and received IGMP 
     * report processing).
     */
    if (state == MIDB_FORWARD_STATE) {
	mdb->flags &= ~PRUNE_FLAG;
	midb->forwarder = TRUE;

	/*
	 * Dense mode interface in forward state never times out,
	 * it only moves to pruned state when a prune is received.
	 *
	 * It does not matter what timer value the midb->expires
	 * held before, set it to zero now.
	 */
	if (IS_DENSE_MODE(midb->idb, gdb)) {
	    TIMER_STOP(midb->expires);
	}
    }
}

/*
 * ip_mroute_set_min_mtu
 *
 * Find the minimal MTU among all the fastswitching interfaces on olist
 */
static void ip_mroute_set_min_mtu(mdbtype *mdb)
{
    midbtype *midb;
    int idb_mtu;
    queuetype *queue = &mdb->if_outputQ;

    mdb->ip_min_mtu = 0;
    for (midb = (midbtype *) queue->qhead; midb; midb = midb->next) {
	if (!midb->idb->ip_multicast_fs) continue;

	idb_mtu = midb->idb->ip_mtu;
	if (!mdb->ip_min_mtu || idb_mtu < mdb->ip_min_mtu) {
	    mdb->ip_min_mtu = idb_mtu;
	}
     }
}

/*
 * ip_delete_mroute_idb
 *
 * Delete an interface from the outgoing interface list for a given
 * multicast route entry.
 */
void ip_delete_mroute_idb (mdbtype *mdb, idbtype *idb, ipaddrtype next_hop)
{

    midbtype *midb;
    queuetype *queue = &mdb->if_outputQ;
    boolean  nbma_sparse_check;

    nbma_sparse_check = (next_hop && !IPMULTICAST(next_hop)) ? TRUE : FALSE;

    midb = ip_get_midb(queue, idb, next_hop, nbma_sparse_check);
    if (!midb) return;

    unqueue(queue, midb);
    midb->next = NULL;

    /*
     * Free MAC header area if built.
     */
    if (midb->mac_header) {
	free(midb->mac_header);
	midb->mac_header = NULL;
	midb->mac_length = 0;

	/*
	 * If this interface has the minimal MTU, need to update minimal
	 * MTU for this mdb.
	 */
	if (idb->ip_mtu <= mdb->ip_min_mtu) {
	    ip_mroute_set_min_mtu(mdb);
	}
    }
    free(midb);
}

/*
 * ipmpkt_debug_check
 *
 * See if this packet is one of the types we're interested in for
 * debugging purposes.
 */
boolean ipmpkt_debug_check (paktype *pak)
{
    iphdrtype *ip;
    
    if (!pak) {
	return (FALSE);
    }

    ip = (iphdrtype *)ipheadstart(pak);
    
    if (ipmpkt_group_debug && ipmpkt_group_debug != ip->dstadr) {
	return (FALSE);
    }
    
    if (ipmdebug_acl != 0 && !ip_accesscheck(pak, acl_array[ipmdebug_acl])) {
	return (FALSE);
    }

    return(TRUE);
}

/*
 * ipmbug
 *
 * Display IP multicast packet for debugging purposes. Gets enabled by doing:
 *
 *     debug ip mpacket [<acl>] [<hostname-or-address>] [detail]
 *
 * If level is 1, only display MAC address and IP header fields. If level is 
 * 2, only display IP destination information. If level is 3, do all.
 */ 
void ipmbug (paktype *pak, char *string, int level)
{
    iphdrtype	*ip;
    hwaddrtype	srcaddr;
    addrtype	*addrtype;
    char	buffer[40];

    if (!pak)
	return;

    ip = (iphdrtype *)ipheadstart(pak);

    if (ipmdebug_detail && (level & 0x1)) {
	if (pak->if_input && pak->if_input->hwptr &&
	    pak->if_input->hwptr->extract_snpa) {
	    (*pak->if_input->hwptr->extract_snpa)(pak, &srcaddr, SOURCE);
	    addrtype = reg_invoke_hwaddr_lookup(pak->if_input->hwptr->enctype,
						(uchar *)&srcaddr.addr, pak);
	    print_snpa_string(buffer, &srcaddr),
	    buginf("\nIP: MAC sa=%s (%s)", buffer,
		   pak->if_input ? pak->if_input->namestring : "local");
	    if (addrtype) {
		buginf(", IP last-hop=%i", addrtype->ip_addr);
	    }
	}

	/*
	 * Display IP header fields.
	 */
	buginf("\nIP: IP tos=0x%x, len=%d, id=0x%x, ttl=%d, prot=%d",
	       ip->tos, ip->tl, ip->id, ip->ttl, ip->prot);
    }


    /*
     * Display IP source and destination addresses.
     */
    if (level & 0x2) {
	buginf("\nIP: s=%i", ip->srcadr);
	if (pak->if_input)
	    buginf(" (%s)", pak->if_input->namestring);
	buginf(" d=%i", ip->dstadr);
	if (pak->if_output)
	    buginf(" (%s)", pak->if_output->namestring);
	buginf(" len %d, %s", pak->datagramsize, string);
    }
}

/*
 * ip_mforward
 *
 * Forward an IP multicast packet. Returns TRUE so ip_forward() has no
 * responsiblity for packet.
 */
boolean ip_mforward (paktype *pak)
{

    mdbtype   *mdb;
    paktype   *dup;
    iphdrtype *ip;
    igmptype  *igmp = NULL;

    ip = (iphdrtype *)ipheadstart(pak);

    /*
     * If a reserved group let ip_forward() deliver it to client, if any.
     */
    if (IPMULTICAST_RSVD(ip->dstadr)) return(FALSE);

    /*
     * If IGMP, return unless this is a multicasted mtrace reponse packet
     * in which case we need to forward it.
     */
    if (ip->prot == IGMP_PROT) 	{
	igmp = (igmptype *) ipdatastart(pak);
	if (igmp->type != IGMP_MTRACE_RESP) return(FALSE);
    }	    

    /*
     * Don't accept multicast packets if received on an interface not
     * configured to do multicast routing. If we are not a multicast router,
     * return FALSE, so ip_forward() can call ip_forus() to see if we
     * are a member of this group.
     */
    if (!pak->if_input->ip_mrouting) {
	if (ip_mrouter_running) {
	    IPMBUG(pak, IPMBUG_ALL, "mrouting interface disabled");
	    retbuffer(pak);
	}
	return(ip_mrouter_running);
    }
    
    /*
     * Do multicast route table lookup.
     */
    mdb = ip_mroute(pak, ip);

    /*
     * Route lookup failed.
     */
    if (!mdb) {
	retbuffer(pak);
	return(TRUE);
    }
    
    /*
     * Don't fastswitch an mtrace response 
     */
    if (igmp) mdb->fastswitch = FALSE;

    /*
     * If we are a group member, deliver packet to client.
     */
    if (mdb->flags & LOCAL_FLAG) {

        /*
	 * If incoming interface is an admin-scoped boundary don't forward
         * packet. The following check stops packets from being delivered
         * to the local process when LOCAL_FLAG is set. For mdb's without
         * LOCAL_FLAG set, missing this check does not cause error, since
         * its olist is guaranteed to be NULL.
         */
        if (ipmulticast_test_boundary(mdb->if_input, mdb->group)) {
	    retbuffer(pak);
            return(TRUE);
        }

 	/*
 	 * If we have a multicast helper address on the multicast i/p
 	 * interface, make sure we have multicast flag set so that
 	 * multicast->broadcast conversion can take place in udp
 	 * process.
 	 */
 	if (pak->if_input->ip_mhelpermapQ.qhead) {
 	    pak->flags |= PAK_MULTICAST; 
 	}
	dup = pak_duplicate(pak);
	if (dup) ip_enqueue(dup);
	TIMER_START(mdb->expires, mdb->expiration_time);
    }

    /*
     * If not a multicast router, don't forward packet.
     */
    if (!ip_mrouter_running) {
	IPMBUG(pak, IPMBUG_ALL, "mrouting disabled");
	retbuffer(pak);
	return(TRUE);
    }

    /*
     * Replicate packet.
     */
    ip_mroute_replicate(mdb, pak);
    return(TRUE);
}

/*
 * ip_mroute
 *
 * Decide if multicast packet should be forwarded. Set up cache if entry
 * does not exist. Return NULL, if packet is not to be routed. Otherwise,
 * return longest match (S,G) entry.
 *
 * This function is intended to be called by ip_mforward() only.
 */
mdbtype *ip_mroute (paktype *pak, iphdrtype *ip)
{

    gdbtype    *gdb;
    mdbtype    *mdb;
    idbtype    *idb;
    midbtype   *midb;
    ipaddrtype source, mask, rpf_nbr;
    boolean    first_hop;
    uchar      rpf_type;

    /*
     * Find group entry.
     */
    mdb = NULL;
    gdb = ip_get_gdb(ip->dstadr);
    if (gdb) {

	/*
	 * We have group (*,G) state. Now do longest match on source address
	 * to see if we have (S,G) state.
	 */
	mdb = ip_get_best_mdb(gdb, ip->srcadr);

	/*
	 * No (S,G). If group is in sparse-mode, use (*,G) in all but the 
	 * following cases:
	 *     o Input interface is in dense-mode. Create source state.
	 *     o This router is directly connected to a receiver and there
	 *       is no SPT threshold or it has been exceeded. Create source
	 *       state.
	 *     o This router is directly connected to a source. Create source
	 *       state to send register (if DR).
	 *     o This router is directly connected to a DVMRP cloud. Create
	 *       source state.
	 */
	if (!mdb && gdb->mdb.if_input && IS_SPARSE_GROUP(gdb)) {
	    if (pak->if_input && !IS_DENSE_MODE(pak->if_input, gdb) &&
		!DVMRP_ACTIVE(pak->if_input)) {
		if (!(gdb->mdb.flags & (CONNECTED_FLAG | LOCAL_FLAG)) ||
		    !gdb->join_spt) {
		    if (!pak->if_input->ip_multicast_dr ||
			!ip_address_on_idb(ip->srcadr, pak->if_input)) {

			/*
			 * Use (*,G) for forwarding.	
			 */
			mdb = &gdb->mdb;
		    }
		}
	    }
	}

	/*
	 * Have (S,G) or (*,G) state, use it for forwarding.
	 */
	if (mdb) {
	    
	    /*
	     * Update packets received for this mdb.
	     */
	    mdb->total_received++;

	    /*
	     * If input interface of packet matches entry's expected incoming 
	     * interface, we want to route it. 
	     */
	    if (mdb->if_input == pak->if_input || !pak->if_input) {

		/*
		 * Send Register to RP if we are first hop router and DR for 
		 * incoming interface. This is never the case for dense-mode 
		 * groups.
		 */
		if (mdb->register_flag) pim_send_register(gdb, mdb, pak);
		return(mdb);
	    }

	    /*
	     * If RPF failure for (S,G), might want to use (*,G) entry if
	     * SPT-bit not set for (S,G).
	     */
	    if (mdb != &gdb->mdb && !(mdb->flags & SPT_FLAG)) {
		if (gdb->mdb.if_input == pak->if_input) return(&gdb->mdb);
	    }

	    /*
	     * If packet's received interface is a multi-access network and
	     * appears in the outgoing interface list for this entry, do
	     * PIM Assert processing except for NBMA networks in PIM nbma-mode,
	     *
	     * Send rate-limited prune on point-to-point link, if
	     * packet was received on non-RPF interface.
	     */
            midb = ip_get_midb(&mdb->if_outputQ, pak->if_input,
			       0,  FALSE);
	    if (midb) {
		if (!is_p2p(pak->if_input) && !pak->if_input->pim_nbma_mode) {
		    pim_send_assert(gdb, mdb, midb, ip->srcadr);
		} else if (is_p2p(pak->if_input)) {
		    
		    /*
		     * Send rate-limited prune on point-to-point link. 
		     */
		    if (!TIMER_RUNNING_AND_SLEEPING(mdb->send_prune)) {
			TIMER_START(mdb->send_prune, MDB_PRUNE_FREQUENCY);
		    } 
		    midb->send_prune = TRUE;
		}
	    }
	    mdb->rpf_failed++;
	    IPMBUG(pak, IPMBUG_ALL, "not RPF interface");
	    return(NULL);
	}
    }

    /*
     * ---------- This is where things slow down. ----------
     */

    /*
     * Create (*,G), determine if group operates in sparse-mode or dense-mode.
     */
    if (!gdb) {
	ip_add_mroute(ip->dstadr, 0, MASK_HOST, NULL, 0, 0,
		      MDB_EXPIRATION_TIME);
	gdb = ip_get_gdb(ip->dstadr);
	if (!gdb) return(NULL);

	/*
	 * Find RP to determine if this group should be treated in dense-mode 
	 * or sparse-mode.
	 */
	if (pim_find_rp(ip->dstadr, 0)) {
	    pim_get_all_rps(gdb, TRUE, FALSE);
	} else {
	    gdb->mdb.flags &= ~SPARSE_FLAG;
	}

	/*
	 * Build outgoing interface list for (*,G).
	 */
	ip_build_outputQ(NULL, gdb, &gdb->mdb);
    }

    /*
     * If we are the first hop router to source, create (S,G) state and setup 
     * to send Registers.
     */
    first_hop = (pak->if_input && pak->if_input->ip_multicast_dr && 
		 ip_address_on_idb(ip->srcadr, pak->if_input));

    /*
     * Create (S,G). Do RPF lookup. If packet received on interface used to 
     * reach the source, we can flood it. Otherwise, drop it.
     */
    rpf_nbr = 0;
    idb = ip_rpf_lookup(ip->srcadr, MASK_HOST, &source, &mask, &rpf_nbr, 
			&rpf_type, ROUTINGDEPTH);
    if (!idb) {
	IPMBUG(pak, IPMBUG_ALL, "RPF lookup failed");
	gdb->mdb.total_received++;
	return(NULL);
    }

    /*
     * Sparse-mode PIM needs hosts routes so Joins make it all the way back
     * to source.
     */
    mdb = ip_add_mroute(ip->dstadr, ip->srcadr, MASK_HOST, idb, rpf_nbr, 
			rpf_type, MDB_EXPIRATION_TIME);
    if (!mdb) {
	gdb->mdb.total_received++;
	return(NULL);
    }

    /*
     * Copy outgoing interface list from (*,G) to (S,G). If NULL, send 
     * immediate prune. Copy Local flag from (*,G) to (S,G) so if this 
     * router is a group member, it can get this packet.
     */
    ip_build_outputQ(&gdb->mdb.if_outputQ, gdb, mdb);
    mdb->flags |= (gdb->mdb.flags & LOCAL_FLAG);
    if (MDB_OUTPUTQ_NULL(mdb) && !first_hop && !(mdb->flags & LOCAL_FLAG)) {
	GET_TIMESTAMP(mdb->send_prune);
    }

    /*	
     * Do sparse-mode special processing (i.e. Register flag settings,
     * triggered Join).
     */
    if (IS_SPARSE_GROUP(gdb)) {

  	/*
 	 * Check case we are border router connecting a DVMRP and PIM
 	 * cloud together. If the RPF interface is pointing to the DVMRP cloud,
 	 * the source is in the DVMRP cloud. Send Registers to the RP so its
 	 * packets get on the shared tree. This interoperation assumes that
 	 * senders are also receivers. We will send joins to the RP by 
 	 * virtue of the idb being a dense-mode interface and is placed in
 	 * the (*,G) outgoing interface list.
 	 */
 	if (DVMRP_ACTIVE(idb) && !pim_find_nbr(rpf_nbr, idb)) first_hop = TRUE;

 	/*
  	 * If we are the first hop from the source, set flags to send
 	 * Registers when we call this function again. Do the same, if
 	 * we are a border router.
  	 */
	if (first_hop) {
	    mdb->register_flag = TRUE;
	    mdb->flags |= SPT_FLAG;
	} else {

            /* 
             * If the incoming interface is a dense mode interface, set
             * the SPT bit so that we dont forward on the (*, G).
             */
	    if ((pak->if_input) && (IS_DENSE_MODE(pak->if_input, gdb))) {
		mdb->flags |= SPT_FLAG;
	    }

	    /*
	     * Determine if triggered sparse-mode join should be sent 
	     * towards new source and if triggered sparse-mode prune 
	     * should be sent for source towards RP.
	     */
	    if ((gdb->mdb.flags & (CONNECTED_FLAG | LOCAL_FLAG)) && 	     
		!MDB_OUTPUTQ_NULL(&gdb->mdb)) {
		pim_send_sm_join(gdb, mdb);

		/*
		 * Reset gdb->join_spt.
		 */	
		gdb->join_spt = pim_over_threshold(0, gdb);
		mdb->flags |= JOIN_FLAG;
	    }
	}
    }

    /*
     * Call this function again, so assert/prune/register processing can be
     * done, if any. This loops exactly one time.
     */
    mdb = ip_mroute(pak, ip);
    return(mdb);
}

/*
 * ip_mroute_replicate
 *
 * Go through outgoing interface list and send packet out each interface
 * that is not in Prune state and does not have outgoing access-list
 * restrictions.
 *
 * This inline should be used by ip_mroute() and ip_mtrace_process_request() 
 * only.
 */
inline void ip_mroute_replicate (mdbtype *mdb, paktype *pak)
{

    paktype    *dup;
    iphdrtype  *ip;
    midbtype   *midb;
    idbtype    *if_input;
    int        bytes;
    ipaddrtype source;
    boolean    sent = FALSE, test_fs, olist_empty;

    /*
     * Test if the packet should have been/was fastswitched ?
     * 
     * If the fastwitching code has already processed this
     * mroute's packet, then PAK_MFS_SWITCHED will be on.
     * If not, the fastswitching code has not been invoked and 
     * we must process switch packets for this mroute.
     */
    test_fs = (mdb->fastswitch && mdb->if_input->ip_multicast_fs &&
               (pak->flags & PAK_MFS_SWITCHED));

    if_input = pak->if_input;
    ip = (iphdrtype *) ipheadstart(pak);
    source = ip->srcadr;
    bytes = ip->tl;

    mdb->flags |= SPT_FLAG;

    /*
     * Should we rate-limit on input?
     */
    if (ip_rate_limit_check(ip->prot, ((udptype *)ipdatastart_iph(ip))->dport,
			    mdb->bits_sec, bytes, mdb->in_limit,
			    mdb->port_number)) {
	IPMBUG(pak, IPMBUG_ALL, "input rate-limited");
	retbuffer(pak);
	return;
    }
    IPMBUG(pak, IPMBUG_MAC, NULL);

    olist_empty = TRUE;
    for (midb = (midbtype *) mdb->if_outputQ.qhead; midb; midb = midb->next) {
	if (midb->state == MIDB_PRUNE_STATE) continue;

        /*
         * Do not forward packets out the loopback interface.
         */
        if (midb->idb->hwptr->type == IDBTYPE_LB) continue;

	/*
	 * Process multicast-threshold. If packet TTL is less than threshold,
	 * forward packet.
	 */
	if (ip->ttl < midb->idb->ip_multicast_threshold) {
	    IPMBUG(pak, IPMBUG_IP, "threshold denied");

	    /*
	     * Prevent pruning because of TTL thresholding.
	     */
	    olist_empty = FALSE;
	    continue;
	}

	/*
	 * Process access-list for outgoing interface.
	 */
	if (midb->idb->ip_output_accesslist) {
	    if (!ip_accesscheck(pak, midb->idb->ip_output_accesslist)) {
		IPMBUG(pak, IPMBUG_IP, "access denied");
		accessfailcnt++;

		/*
		 * Prevent pruning because of access control, but only if
		 * access list is extended.
		 */
		if (midb->idb->ip_output_accesslist->type == ACL_IP_EXTENDED ||
		    midb->idb->ip_output_accesslist->type ==
		    ACL_IP_NAMED_EXTENDED) {
		    olist_empty = FALSE;
		}
		continue;
	    }
	}

	/*
	 * Should we rate-limit on output?
	 */
	if (ip_rate_limit_check(ip->prot,
				((udptype *)ipdatastart_iph(ip))->dport,
				mdb->bits_sec, bytes, midb->out_limit,
				midb->port_number)) {
	    IPMBUG(pak, IPMBUG_IP, "output rate limited");

	    /*
	     * Prevent pruning because of rate limiting.
	     */
	    olist_empty = FALSE;
	    continue;
	}
	
	/*
	 * This is the case where fastswitching processed part of the 
	 * outgoing list and not the rest because it encountered an midb
	 * with no MAC header built. The first Null MAC header is where
	 * process level (here) switching starts the list.
	 */
	if (test_fs) {
	    if (midb->mac_header) {
		olist_empty = FALSE;
                continue;
	    } else {
		test_fs = FALSE;
	    }
	}

	/*
	 * Make physical copy of packet (ugh) if there are more interfaces.
	 * This can be sped up if all prune state interfaces show up before the
	 * forward state interfaces. We can avoid one unnecessary copy.
	 */
	if (midb->next) {
	    dup = pak_duplicate(pak);
	    if (!dup) break;
	} else {
	    dup = pak;
	    pak = NULL;
	}

	/*
	 * Send it out.
	 */
	dup->if_output = midb->idb;
	dup->desthost = midb->next_hop;
	IPMBUG(dup, IPMBUG_IP, "mforward");

	/*
	 * Send packet out.
	 */
	sent |= ipsendnet(dup, TRUE, TRUE);
	midb->uses++;
	midb->idb->ip_multicast_out++;
    }

    /*
     * If at least one copy was forwarded, count usage and update
     * routing table timer so the state stays alive. Otherwise,
     * send prune on point-to-point interface if not rate-limiting.
     */
    if (sent) {
	mdb->uses++;
	mdb->uses_sec++;
	mdb->bytes += bytes;
	mdb->bits_sec += bytes << 3;
 	TIMER_START(mdb->expires, mdb->expiration_time);
			
	if (!mdb->register_flag && !(mdb->flags & LOCAL_FLAG)) {
	    if (mdb->if_input)
		ip_mdb_toggle_fs(mdb, mdb->if_input->ip_multicast_fs, 
				 "ip_mroute_replicate-1");
	}
    } else if (olist_empty) {

	IPMBUG(pak, IPMBUG_IP, "mroute olist null");
	ip_mdb_toggle_fs(mdb, mdb->if_input->ip_multicast_fs,
 			 "ip_mroute_replicate-2");
	/*
 	 * On a P2P link, send a prune immediately if prune timer is not 
 	 * running. Then start the timer for rate-limited prunes.
 	 */
	if (is_p2p(if_input) && !TIMER_RUNNING_AND_SLEEPING(mdb->send_prune) &&
            !(mdb->flags & LOCAL_FLAG)) {
 	    ipmulticast_send_prune(mdb);
 	    TIMER_START(mdb->send_prune, MDB_PRUNE_FREQUENCY);
         }
    } else {
	if (MCACHE_DEBUG(ip->dstadr)) {
          buginf("\nMRC: Packet from %i to %i not processed by process-level",
		 ip->srcadr, ip->dstadr);
	}
    }

    /*
     * The packet was not sent because there were no output interfaces or
     * they were all pruned.
     */
    if (pak) retbuffer(pak);
}

/*
 * ip_get_gdb
 *
 * Find group descriptor block from the multicast hash table.
 */
gdbtype *ip_get_gdb (ipaddrtype group)
{
    return(ip_get_gdb_inline(group));
}

/*
 * ip_get_mdb
 *
 * Find the multicast descriptor block from a group's radix tree. The keys
 * are IP address and network mask.
 */
mdbtype *ip_get_mdb (gdbtype *gdb, ipaddrtype source, ipaddrtype mask)
{

    mdbtype     *mdb;
    ipradixtype address;

    if (!gdb) return(NULL);

    ip_addr2radix(source, address);
    
    mdb = (mdbtype *) rn_match(address, gdb->radix_head);
    
    while (mdb && mask != ip_radix2addr(mdb->mask)) {
	mdb = (mdbtype *) mdb->radix_nodes[0].rn_dupedkey;
    }
    return(mdb);
}

/*
 * ip_next_specific_mdb
 *
 * Get next more specific mdb from radix tree.
 */
mdbtype *ip_next_specific_mdb (gdbtype *gdb, ipaddrtype last_source, 
			       ipaddrtype source, ipaddrtype mask)
{

    mdbtype    *mdb;
    ipaddrtype next_source;

    mdb = ip_next_mdb(gdb, last_source);
    if (mdb) {
	next_source = ip_radix2addr(mdb->source);
	if ((next_source & mask) != source) mdb = NULL;
    }
    return(mdb);
}

/*
 * ip_next_mdb
 *
 * Get next mdb node in radix tree.
 */
mdbtype *ip_next_mdb (gdbtype *gdb, ipaddrtype source)
{

    rntype *rn;

    rn = ip_radix_nextnode(source, gdb->radix_head->rnh_treetop);
    return((mdbtype *) rn);
}

/*
 * ip_get_best_mdb
 *
 * Find the multicast descriptor block from the group's radix tree. A 
 * longest match lookup is performed.
 */
mdbtype *ip_get_best_mdb (gdbtype *gdb, ipaddrtype source)
{
    return(ip_get_best_mdb_inline(gdb, source));
}

/*
 * ip_get_worst_mdb
 *
 * Find the multicast descriptor block from the group's radix tree. The
 * shortest source/mask is retrieved from the radix tree. 
 *
 * Note: this may not return a valid mdb in the tree. However it is used
 * as a starting point for calls to ip_next_specific_mdb() to find all more 
 * specific routes for this aggregate.
 */
mdbtype *ip_get_worst_mdb (gdbtype *gdb, ipaddrtype source, ipaddrtype mask)
{

    rntype      *rn;
    mdbtype     *mdb;
    ipradixtype address, addrmask;
    ipaddrtype  tbladdress, tblmask;

    ip_addr2radix(source, address);
    ip_addr2radix(mask, addrmask);

    rn = rn_search_m(address, gdb->radix_head->rnh_treetop, addrmask);

    if (!rn || !(rn->rn_flags & RNF_ACTIVE)) {
	return(NULL);
    }
    if (rn->rn_flags & RNF_ROOT) {
	rn = rn_nextleaf(rn);
    }

    if (!rn || !(rn->rn_flags & RNF_ACTIVE) || (rn->rn_flags & RNF_ROOT)) {
	return(NULL);
    }

    mdb = (mdbtype *) rn;
    tbladdress = ip_radix2addr(mdb->source);
    tblmask = ip_radix2addr(mdb->mask);

    /*
     * Route in this range does not exist.
     */
    if ((tbladdress & mask) != source) return(NULL);

    /*
     * If table mask is the same length or longer, we want this node.
     */
    if (tblmask >= mask) return(mdb);
    return(NULL);
}

/*
 * ip_get_midb
 * 
 * Get specific multicast idb descriptor block from an outgoing interface
 * list. A match on the idb is performed.
 */
midbtype *ip_get_midb (queuetype *queue, idbtype *idb, ipaddrtype next_hop,   
		       boolean nbma_sparse_check)
{ 
    return(ip_get_midb_inline(queue, idb, next_hop, nbma_sparse_check));
}    

/*
 * ip_mfree_outputQ
 *
 * Dequeue and free all all elements in the outgoing interface list.
 */
void ip_mfree_outputQ (queuetype *queue)
{

    midbtype *entry;

    while ((entry = dequeue(queue))) {
	if (entry->mac_header) {
	    free(entry->mac_header);
	    entry->mac_header = NULL;
	    entry->mac_length = 0;
	}
	free(entry);
    }
}

/*
 * ip_show_rpf
 *
 * Show RPF information about source without creating multicast state. This
 * functions executes the command "show ip rpf <name-or-address".
 */
void ip_show_rpf (parseinfo *csb)
{

    idbtype    *idb;
    nametype   *name;
    uchar      rpf_type;
    ipaddrtype source, rpf_source, rpf_mask, rpf_nbr;
    boolean    directly_connected;

    source = GETOBJ(paddr,1)->ip_addr;
    if (IPMULTICAST(source)) {
	printf("\nInvalid address, must be a unicast address");
	return;
    }

    rpf_nbr = 0;
    idb = ip_rpf_lookup(source, MASK_HOST, &rpf_source, &rpf_mask, 
			&rpf_nbr, &rpf_type, ROUTINGDEPTH);

    printf("\nRPF information for ");

    name = reg_invoke_ip_address_lookup(source);
    printf("%s (%i)", (name) ? name->name : "?", source);

    /*
     * Didn't find route.
     */
    if (!idb) {
	printf(" failed, no route exists");
	return;
    }

    /*
     * Found route, display returned information.
     */
    printf("\n  RPF interface: %s", idb->namestring);
    printf("\n  RPF neighbor: ");
    directly_connected = FALSE;
    if (rpf_nbr) {
	name = reg_invoke_ip_address_lookup(rpf_nbr);
    } else {
	name = NULL;
	if (idb == samecable(source)) directly_connected = TRUE;
    }
    printf("%s (%i)", (name) ? name->name : "?", rpf_nbr);
    if (directly_connected) printf(" - directly connected");
    printf("\n  RPF route/mask: %i/%i", rpf_source, rpf_mask);
    printf("\n  RPF type: %s", (rpf_type == RPF_UNICAST) ? "unicast" : 
		 (rpf_type == RPF_DVMRP) ? "dvmrp" : "static mroute");
}
static const uchar mroute_banner[] = {
"IP Multicast Routing Table
Flags: D - Dense, S - Sparse, C - Connected, L - Local, P - Pruned
       R - RP-bit set, F - Register flag, T - SPT-bit set, J - Join SPT
Timers: Uptime/Expires
Interface state: Interface, Next-Hop, State/Mode\n" };

/*
 * ip_show_mroute
 *
 * Process command:
 *
 * show ip mroute [<group>] [<source>] [summary] [count] [active <kbps>]
 */
void ip_show_mroute (parseinfo *csb)
{

    gdbtype    *gdb, *next;
    mdbtype    *mdb;
    ipaddrtype group, source;
    ipaddrtype addr1, addr2;
    int        i;
    boolean    summary, source_specified;
    rnhtype    *head;

    addr1 = GETOBJ(paddr,1)->ip_addr;
    addr2 = GETOBJ(paddr,2)->ip_addr;
    source_specified = GETOBJ(int,1);
    summary = GETOBJ(int,2);

    if (source_specified) {
	if (IPMULTICAST(addr1)) {
	    if (IPMULTICAST(addr2)) {
		printf("\nTwo group addresses not permitted");
		return;
	    }
	    group = addr1;
	    source = addr2;
	} else if (IPMULTICAST(addr2)) {
	    group = addr2;
	    source = addr1;
	} else {
	    printf("\nIP group address required");
	    return;
	}
    } else {
	group = addr1; /* Parser ensures this is either 0 or multicast */
	source = 0;
    }
	
    /*
     * Do "show ip mroute [<group>] active [<kbps>]".
     */	
    if (GETOBJ(int,4)) {
	ip_show_mroute_active(group, GETOBJ(int,4));
	return;
    }

    /*
     * Do "show ip mroute count".
     */
    if (GETOBJ(int,3)) {
	ip_show_mroute_count(group, source, source_specified);
	return;
    }

    /*
     * Do "show ip mroute static".
     */
    if (GETOBJ(int,5)) {
	ip_show_mroute_static();
	return;
    }

    /*
     * See if group exists, if it was requested on command line.
     */
    gdb = NULL;
    if (group) {
	gdb = ip_get_gdb(group);
	if (!gdb) {
	    printf("\nGroup %i not found", group);
	    return;
	}
    }

    automore_enable(mroute_banner);
    automore_header(NULL);

    /*
     * Display either all sources for a group, or a specific (source, group)
     * pair.
     */
    if (group) {
	mdb = NULL;
	if (source_specified) {
	    mdb = ip_get_best_mdb(gdb, source);

	    /*
	     * Display single (S,G) entry. If a source entry was not found,
	     * display the (*,G) entry.
	     */
	    if (mdb) {
		ip_display_one_mroute(NULL, mdb, summary);
	    } else {
		ip_display_one_mroute(gdb, &gdb->mdb, summary);
	    }
	    automore_disable();
	    return;
	}

	/*
	 * Display all sources for G.
	 */
	mem_lock(gdb);
	ip_display_one_mroute(gdb, &gdb->mdb, summary);
	if (gdb->radix_head) {
 	    head = gdb->radix_head;
 	    mem_lock(head);
	    rn_walktree_blocking(gdb->radix_head->rnh_treetop, 
				 ip_display_mroute, summary);
	    free(head);
	}
	free(gdb);
	automore_disable();
	return;
    }	

    /*
     * Display entire multicast routing table.
     */
    for (i = 0; i < NETHASHLEN; i++) {
	for (gdb = ip_mroute_cache[i]; gdb; gdb = next) {
	    mem_lock(gdb);
	    ip_display_one_mroute(gdb, &gdb->mdb, summary);
	    if (gdb->radix_head) {
 		head = gdb->radix_head;
 	        mem_lock(head);
		rn_walktree_blocking(gdb->radix_head->rnh_treetop, 
				     ip_display_mroute, summary);
 		free(head);
	    }
	    next = gdb->next;
	    free(gdb);
	    if (summary) printf("\n");
	}
	if (automore_quit()) break;
    }
    automore_disable();
}

/*
 * ip_display_mroute
 */
int ip_display_mroute (rntype *node, va_list args)
{

    boolean summary;

    summary = va_arg(args, boolean);

    ip_display_one_mroute(NULL, (mdbtype *) node, summary);
    if (automore_quit()) return(1);
    return(0);
}

/* 
 * ip_display_one_mroute
 */
void ip_display_one_mroute (gdbtype *gdb, mdbtype *mdb, boolean summary)
{

    midbtype *midb, *next;
    uchar    uptime[20];
    uchar    expires[20];

    /*
     * Display (*,G) entry only.
     */
    if (gdb) {
	printf("\n(*, %i), ", mdb->group);
    } else {
	printf("\n");
	if (summary) printf("  ");
	printf("(%i/%d, %i), ", ip_radix2addr(mdb->source), 
	       ip_bitsinmask(ip_radix2addr(mdb->mask)), mdb->group);
    }

    if (!TIMER_RUNNING_AND_AWAKE(mdb->expires)) {
	sprint_ticks_in_dhms(expires, TIMER_RUNNING(mdb->expires) ?
			    TIME_LEFT_SLEEPING(mdb->expires) : 0);
    } else {
	sprintf(expires, "now");
    }
    sprint_ticks_in_dhms(uptime, ELAPSED_TIME(mdb->uptime));

    /*
     * Print first line of output.
     */
    printf("%s/%s", uptime, expires);
    if (gdb) {
	printf(", RP %i", gdb->rp.address);
    }
    if (mdb->flags || !mdb->if_outputQ.count) {
	printf(", flags: ");
 	if (gdb) {
	    printf("%s", IS_SPARSE_GROUP(gdb) ? "S" : "D");
	    if (gdb->join_spt) printf("J");
	}
	if (MDB_OUTPUTQ_NULL(mdb)) printf("P");
	if (mdb->flags & CONNECTED_FLAG) printf("C");
	if (mdb->flags & LOCAL_FLAG) printf("L");
	if (mdb->flags & SET_REG_FLAG) printf("F");
	if (mdb->flags & RP_BIT_FLAG) printf("R");
	if (mdb->flags & JOIN_FLAG) printf("J");
	if (!gdb && mdb->flags & SPT_FLAG) printf("T");
    }

    if (summary) return;

    /*
     * Print second line.
     */
    printf("\n  Incoming interface: %s, RPF nbr %i", (mdb->if_input) ? 
	   mdb->if_input->namestring : "Null", mdb->rpf_nbr);
    if (mdb->use_assert_rpf) printf("*");

    /*
     * Print where we RPFing from.
     */
    if (mdb->rpf_type == RPF_DVMRP) printf(", Dvmrp");
    if (mdb->rpf_type == RPF_STATIC) printf(", Mroute");

    /*
     * Print Register flags bit string.
     */
    if (mdb->register_flag) {
	printf(", Registering");
    }

    /*
     * Print input rate-limit if any.
     */
    if (mdb->in_limit != -1) {
	printf("\n%21c Limit %d kbps", ' ', mdb->in_limit/1000);
	if (mdb->port_number && mdb->port_number != -1)
	    printf(", port %d", mdb->port_number);
    }

    /*
     * Print outgoing interface list, if any.
     */
    printf("\n  Outgoing interface list:");
    if (!mdb->if_outputQ.count) {
	printf(" Null\n");
	return;
    }
    for (midb = (midbtype *) mdb->if_outputQ.qhead; midb; midb = next) {
	if (!TIMER_RUNNING_AND_AWAKE(midb->expires)) {
	    sprint_ticks_in_dhms(expires, TIMER_RUNNING(midb->expires) ?
				TIME_LEFT_SLEEPING(midb->expires) : 0);
	} else {
	    sprintf(expires, "now");
	}
	sprint_ticks_in_dhms(uptime, ELAPSED_TIME(midb->uptime));

	mem_lock(midb);
	printf("\n    %s", midb->idb->namestring);
	if (!gdb) {
	    gdb = ip_get_gdb(mdb->group);
	}
	if (gdb && midb->idb->pim_nbma_mode &&
	    !IS_DENSE_MODE(midb->idb, gdb)) {
	    printf(", %i", midb->next_hop);
	}
	printf(", %s/%s, %s/%s", midb_state_str[midb->state], 
	       (DVMRP_TUNNEL(midb->idb)) ? "Dvmrp" :
	       (midb->idb->pim_mode == PIM_DENSE_MODE) ? "Dense" : 
	       (midb->idb->pim_mode == PIM_SPARSE_MODE) ? "Sparse" : 
	       "Sparse-Dense", uptime, expires);

	/*
	 * Print output rate-limit if any.
	 */
	if (midb->out_limit != -1) {
	    printf(", limit %d kbps", midb->out_limit/1000);
	    if (midb->port_number && midb->port_number != -1)
		printf(", port %d", midb->port_number);
	}
	next = midb->next;
	free(midb);
    }
    if (!summary) printf("\n");
}

/*
 * ip_addr2radix
 *
 * Move IP address into a 5-byte format. First byte is a length of
 * the address (plus 1 for the length byte) and the next 4 bytes are the 
 * IP address. This type of address is used in the radix routines.
 */
inline void ip_addr2radix (ipaddrtype source, ipradixtype address)
{
    *address = sizeof(ipaddrtype) + 1;
    PUTLONG(address + 1, source);
}

/*
 * ip_radix2addr
 *
 * Return ipaddrtype value from ipradixtype 5-byte format (for radix tree).
 */
inline ipaddrtype ip_radix2addr (ipradixtype address)
{
    return(GETLONG(address + 1));
}

/*
 * ip_build_outputQ
 *
 * Build a multicast output queue. This function either copys the outgoing
 * interface state from one entry to another or builds one from scratch.
 * if sourceQ == NULL, it is built from scratch.
 */
void ip_build_outputQ (queuetype *sourceQ, gdbtype *gdb, mdbtype *mdb)
{

    midbtype       *midb;
    idbtype        *idb;
    igmp_cachetype *direct_members;
    boolean        null_olist;
    list_element   *l_elt;
    
    idb = NULL;

    /*
     * Copying interfaces from sourceQ to destQ. The state and timers
     * from source are carried over too.
     */
    if (sourceQ) {
	null_olist = TRUE;
	for (midb = (midbtype *) sourceQ->qhead; midb; midb = midb->next) {

	    ip_add_mroute_idb(gdb, mdb, midb->idb, midb->next_hop, 
			      MDB_EXPIRATION_TIME, midb->state, FALSE);
	    if (midb->state == MIDB_FORWARD_STATE) null_olist = FALSE;
	}

	/*
	 * If the group is in sparse mode, and the (*,G) iif is non-NULL
	 * we need to add it to the olist if the following are satisfied:
	 *  o  the (*,G) iif is not the same as the (S,G) iif;
	 *  o  the (*,G) iif has
	 *       - a dvmrp neighbor, or
	 *       - a directly connected member, or
	 *       - the (*,G) iif is in dense mode and it has PIM neighbor(s)
	 */
	if (gdb->mdb.if_input) {
	    idb = gdb->mdb.if_input;

	    if (idb != mdb->if_input) {
		if (DVMRP_ACTIVE(idb) || igmp_find_group(mdb->group, idb) ||
		    (idb->pim_mode == PIM_DENSE_MODE && idb->pim_nbrs)) {
		    ip_add_mroute_idb(gdb, mdb, idb, gdb->mdb.rpf_nbr,
				      MDB_EXPIRATION_TIME, MIDB_FORWARD_STATE,
				      TRUE);
		}
	    }
	}

	/*
	 * Set PRUNE_FLAG if (*,G)s have all Prune state entries in the olist.
	 */
	if (null_olist) mdb->flags |= PRUNE_FLAG;
	return;
    }

    /*
     * Build outgoing interfaces from scratch.
     */
    FOR_ALL_IPMULTICAST_IDBS(idb, l_elt) {

	/*
	 * Skip interface if we are not multicast routing on it.
	 */
	if (!idb->ip_mrouting) continue;

	/*
	 * Never put incoming interface into outputQ.
	 */
	if (mdb->if_input == idb) continue;

	direct_members = igmp_find_group(mdb->group, idb);

	/*
	 * Only populate output queue with interfaces for flooding. This
	 * is the dense-mode flood and prune algorithm. Always include
	 * interface if there are direct members or DVMRP neighbors present
	 * regardless if interface is in sparse-mode or dense-mode.
	 * An interface configured in sparse-dense mode is also excluded
	 * if the mdb entry is in sparse mode.
	 */
	if (!DVMRP_ACTIVE(idb)) {
	    if (!direct_members) {
		if (!(IS_DENSE_MODE(idb, gdb) && idb->pim_nbrs)) continue;
	    } else {

		/*
		 * If this is a sparse-mode interface, we only have the DR 
		 * put the interface (with direct members) in its olist. This 
		 * is so we can optimize to avoid periodic Assert behavior and
		 * therefore avoid short duplicate bursts every 3 minutes.
		 */
		if (!IS_DENSE_MODE(idb, gdb) && !idb->ip_multicast_dr) {
		    continue;
		}
	    }
	}

	/*
	 * Set Connected/Local flag in mdb if router has direct members,
	 * is connected to a DVMRP cloud, or is a member itself.
	 */
	if (direct_members) {
	    mdb->flags |= CONNECTED_FLAG;
	    if (direct_members->flags & IGMP_LOCAL_FLAG) {
		mdb->flags |= LOCAL_FLAG;
		TIMER_STOP(mdb->expires);
	    }
	}
	if (DVMRP_ACTIVE(idb)) mdb->flags |= CONNECTED_FLAG;

	/*
	 * Any other case, build an outgoing interface entry.
	 */
	ip_add_mroute_idb(gdb, mdb, idb, 0, MDB_EXPIRATION_TIME, 
			  MIDB_FORWARD_STATE, FALSE);
    }
}

/*
 * ip_get_static_mroute
 *
 * Get static mroute that best matches the mroute.
 */	
static mroutetype *ip_get_static_mroute (ipaddrtype host, ndbtype *ndb)
{

    mroutetype *mroute, *best_mroute;

    best_mroute = NULL;
    for (mroute = ip_static_mroutes; mroute; mroute = mroute->next) {
	if ((host & mroute->mask) != mroute->source) continue;
	if (ndb) {
	    if (mroute->pdb) {
		if (INDEX2PDB(ndb->pdbindex) != mroute->pdb) continue;

		/*
		 * Route matches protocol description on mroute. Use the mroute
		 * unless it is the gateway of last resort.
		 */
		if (ndb == default_ndb) continue;
	    }

	    /*
	     * Check against route-map, if specified.
	     */
	    if (mroute->route_map) {
		if (!route_map_ip_check(mroute->route_map, ndb, 
					INDEX2PDB(ndb->pdbindex))) continue;
	    }
	}
	if (!best_mroute || mroute->mask > best_mroute->mask) {
	    best_mroute = mroute;
	}
    }
    return(best_mroute);
}

/*
 * ip_rpf_static_mroute
 *
 * Determine if a static mroute should be used in this RPF calculation. If
 * it is not used, none of the output parameters are modified.
 */
static void 
ip_rpf_static_mroute_lookup (ipaddrtype host, ipaddrtype mask, ndbtype *ndb,
	   dvmrptype *dvmrp, idbtype **idb, ipaddrtype *next_hop, 
	   ipaddrtype *source, ipaddrtype *rpf_mask, uchar *rpf_type)
{

    mroutetype *mroute;
    ndbtype    *db;
    idbtype    *rpf_idb;
    ipaddrtype rpf_addr;
    int        i;

    rpf_idb = NULL;
    rpf_addr = 0;

    /*
     * Find static mroute that matches host address.
     */
    mroute = ip_get_static_mroute(host, ndb);
    if (!mroute) return;

    /*
     * If static mroute has worse distance then either a unicast or DVMRP
     * route, bail. Give precedent to directly connected routes when their
     * distance is the same as the mroute.
     */
    if (ndb) {
	if (ndb->distance < mroute->distance) return;

 	/*
 	 * May be using unicast routing process to select a specific static 
 	 * mroute. Don't use it now to compare against static mroute.
 	 */
 	if (!mroute->pdb) {
	    if (ndb->distance == mroute->distance &&
		(ndb->attrflags & NET_ATTACHED)) return;
	}
    }
    if (dvmrp) {
	if (dvmrp->distance < mroute->distance) return;
    }

    /*	
     * Use interface if no RPF address was given on command. If it is
     * a point-to-point interface, see if there is a PIM neighbor and use it
     * as the RPF neighbor address.
     */
    if (mroute->idb) {
	if (interface_up(mroute->idb)) {
	    rpf_idb = mroute->idb;
	    if (is_p2p(rpf_idb)) {
		if (rpf_idb->pim_nbrs) {
		    rpf_addr = rpf_idb->pim_nbrs->nbr_address;
		} else if (DVMRP_ACTIVE(rpf_idb)) {
		    rpf_addr = rpf_idb->hwptr->tunnel->destination;
		}
	    }
	}
    } else {
	    
	/*
	 * Use configured RPF address. If it is a directly connected 
	 * neighbor address, use it. Otherwise, do recursive lookup until 
	 * we get a directly connected neighbor.
	 */
	rpf_addr = mroute->rpf_address;
	for (i = 0; i < ROUTINGDEPTH; i++) {
	    if (!(db = ip_rtlookup(rpf_addr, 0))) return;

	    /*
	     * Directly connected.
	     */
	    if ((db->attrflags & NET_ATTACHED) || !db->routecount) {
		rpf_idb = db->lastinterface;
		break;
	    } else {
		rpf_addr = db->rdb[0]->gateway;
	    }
	}
    }

    /*	
     * If we obtained an idb, fill in rest of output parameters. Otherwise
     * don't touch them.
     */
    if (rpf_idb) {
	*idb = rpf_idb;
	if (next_hop) *next_hop = rpf_addr;
	if (source) *source = host;
	if (rpf_mask) *rpf_mask = mroute->mask;
	if (rpf_type) *rpf_type = RPF_STATIC;
    }
}


/*
 * ip_rpf_get_mask
 *
 * Find longest mask from unicast routing table. Exclude host routes. 
 * This function is a noop if the supplied mask is longer than the routing
 * table mask. Otherwise, we use the longer mask to keep consistent 
 * information throughout an partially aggregated topology.
 */
ipaddrtype ip_rpf_get_mask (ndbtype *ndb, ipaddrtype mask)
{

    ipaddrtype table_mask;

    /*
     * Assume network is not variably subnetted.
     */
    table_mask = ndb->netsmask;

    /*
     * If variably subnetted, return table mask if longer than mask passed
     * in. Exclude host masks. We want something to scale :-)
     */
    if (ndb->attrflags & NET_VARIABLE) {
	if (ndb->maskcount > 1 && ndb->masktable[0].mask == MASK_HOST) {
	    table_mask = ndb->masktable[1].mask;
	} else {
	    table_mask = ndb->masktable[0].mask;
	}
    }

    /*
     * If supplied mask is shorter than routing table mask, return the
     * longer mask. Otherwise, use supplied mask to build multicast route
     * entry.
     */
    if (mask < table_mask) {
	return(table_mask);
    }
    return(0);
}

/*
 * ip_rpf_unicast_lookup -
 *
 * Do a unicast routing table lookup on host. Return address/mask entry
 * from longest match lookup as well as next-hop interface to reach host.
 *
 * This function does some of the same things that first_hop() does. However,
 * it cannot be used since it does not return address/mask information.
 *
 * If source, mask, and next_hop are NULL, the values will not be returned
 * to caller. 
 */
static idbtype *ip_rpf_unicast_lookup (ipaddrtype host, ipaddrtype mask, 
				       ipaddrtype *source, 
				       ipaddrtype *rpf_mask, ndbtype **net,
				       ipaddrtype *next_hop, int depth)
{
    idbtype    *idb = NULL;
    ndbtype    *ndb, *db;
    ipaddrtype src, msk, table_msk;
    
    /*
     * Lookup unicast route.
     */
    db = ip_rtlookup(host, 0);

    /*	
     * Network or subnet not found or we matched on 0.0.0.0.
     * Prefer gateway of last resort to 0.0.0.0, if it exists.
     * This makes lookup consistent with unicast default selection.
     */
    if (!db || (db == ndb_zero)) {
    
	/*
	 * If candidate default is present, choose that.
	 */
	if (default_ndb) {
	    db = default_ndb;
        }
	
	/*
	 * We did not find a unicast route. So bail.
	 */
	if (!db) {
	    return(NULL);
	}
	
	src = 0;
	msk = 0;
    } else {
	src = db->netnumber;
	msk = db->netsmask;
    }

    /*
     * Find ndb and see if this network is variably subnetted. Use longest
     * mask to build (S,G). Also, the network might have a single subnet
     * but we matched on 0.0.0.0. Use the single subnet mask.
     */
    ndb = net_lookup(host, FALSE);
    if (ndb) {
	table_msk = ip_rpf_get_mask(ndb, mask);
	if (table_msk) {
	    msk = table_msk;
	    src = host & msk;
	}
    }

    /*
     * If the mask found in the unicast routing table is longer than
     * the advertised mask, don't build the entry.
     */
    if (msk > mask) {
	return(NULL);
    }

    /*
     * Source and RPF mask back to be returned to caller.
     */
    if (source) *source = src;
    if (rpf_mask) *rpf_mask = msk;

    idb = ipmulticast_resolve_nexthop(db, next_hop, depth);
    
    /*
     * Return RPF interface and the route.
     */
    *net = db;
    return(idb);
}

/*
 * ipmulticast_resolve_nexthop
 *
 * Get interface of next-hop.  If there are multiple equal cost next-hops,
 * choose the highest IP addressed PIM neighbor.  If there aren't any PIM
 * neighbors, choose the highest IP addressed nexthop [this is the case
 * where DVMRP may be active on the interface].
 */
idbtype *
ipmulticast_resolve_nexthop (ndbtype *db, ipaddrtype *next_hop, int depth)
{
    idbtype *idb;
    ndbtype *dummy;
    rdbtype *best_rdb, *rdb;
    int i;

    idb = NULL;
    
    if (!db->routecount) {
	idb = db->lastinterface;
    } else {
	best_rdb = NULL;
	for (i = 0; i < db->routecount; i++) {
	    if (!(rdb = db->rdb[i])) continue;

	    if (!best_rdb) {
		best_rdb = rdb;
		continue;
	    }

	    if (pim_find_nbr(rdb->gateway, rdb->idb)) {
		if (pim_find_nbr(best_rdb->gateway, best_rdb->idb)) {
		    if (rdb->gateway > best_rdb->gateway) best_rdb = rdb;
		} else {
		    best_rdb = rdb;
		}
	    } else {
		if (rdb->gateway > best_rdb->gateway) {
		    if (!pim_find_nbr(best_rdb->gateway, best_rdb->idb)) {
			best_rdb = rdb;
		    }
		}
	    }
	}

	/*
	 * Found the best possible path(route). Now, resolve the
	 * interface and next hop address.
	 */
	if (best_rdb) {
	    if (next_hop) {
		if (best_rdb->gateway) *next_hop = best_rdb->gateway;
	    }
	    idb = best_rdb->idb;

	    /*
	     * Static route or recursive route; do recursive lookup.
	     */
	    if (!idb && depth) {
		depth--;
		idb = ip_rpf_unicast_lookup(best_rdb->gateway, MASK_HOST, 
					    NULL, NULL, &dummy, next_hop,
					    depth);
	    }
	}
    }
    return(idb);
}
/*
 * ip_rpf_lookup
 *
 * Lookup unicast routing table, dvmrp routing table and static mroute table
 * to decide RPF interface and next hop address.
 *
 * If source, mask, and next_ hop are NULL, the values will not be returned
 * to caller.
 */
idbtype *ip_rpf_lookup (ipaddrtype host, ipaddrtype mask, ipaddrtype *source, 
			ipaddrtype *rpf_mask, ipaddrtype *next_hop, 
			uchar *rpf_type, int depth)
{
    
    idbtype    *idb = NULL;
    ndbtype    *db = NULL;
    dvmrptype  *route;

    /*
     * Do unicast routing table lookup.
     */
    if (rpf_type) *rpf_type = RPF_UNICAST;
    idb = ip_rpf_unicast_lookup(host, mask, source, rpf_mask, &db, next_hop, 
				depth);

    /*
     * If we did not find an RPF interface or the RPF interface is
     * not enabled for multicast, do not use that route.
     */
    if (!idb || !idb->ip_mrouting) {
	
	/*
	 * Special case NULL0 interface so that we pass RPF checks even
	 * if multicast is not enabled. We want to return an idb because 
	 * DVMRP pruning wants to learn about a mask. Otherwise clear idb and db.
	 */
	if (idb != nullidb) {
            db = NULL;
	    idb = NULL;
	}
    } else {
	
	/*
	 * Always try and get a non-zero RPF IP address.
	 */
	if (next_hop && !*next_hop && is_p2p(idb)) {
	    if (idb->pim_nbrs) {
		*next_hop = idb->pim_nbrs->nbr_address;
	    }
	}
    }
    
    /*
     * If we have DVMRP routing table, a DVMRP route might have preference
     * over the unicast route. If so, use it as the RPF neighbor.
     */
    route = NULL;
    if (DVMRP_ROUTES) {
	route = dvmrp_get_route(host, mask, TRUE);
	if (!DVMRP_VALID(route)) {
	    route = NULL;
	} else if (!dvmrp_prefer_unicast_route(route, db)) {
	    idb = route->idb;
	    if (next_hop) *next_hop = route->next_hop;
	    if (source) *source = ip_radix2addr(route->dest);
	    if (rpf_mask) *rpf_mask = ip_radix2addr(route->mask);
	    if (rpf_type) *rpf_type = RPF_DVMRP;
	}
    }

    /*
     * If there are static mroutes, see if we should use them.
     */
    if (ip_static_mroutes) {
	ip_rpf_static_mroute_lookup(host, mask, db, route, &idb, next_hop,
				    source, rpf_mask, rpf_type);
    }

    /*
     * Return RPF interface.
     */
    return(idb);
}

/*
 * ip_mroute_ager
 *
 * Age entries in the multicast routing table. Also do some PIM periodic
 * processing functions.
 */
void ip_mroute_ager (void)
{

    gdbtype    *gdb, *next;
    pim_rptype *rp;
    int        i;
    ipaddrtype group;

    for (i = 0; i < NETHASHLEN; i++) {
	for (gdb = ip_mroute_cache[i]; gdb; gdb = next) {
	    next = gdb->next;

	    /*
	     * Process each (Si,G). Clear SET_REG_FLAG flag in gdb because
	     * it is only set if any (Si,G) have it set. It will get set
	     * when we walk them now.
	     */
	    gdb->mdb.flags &= ~SET_REG_FLAG;
	    rn_walktree(gdb->radix_head->rnh_treetop, ip_age_one_mroute, gdb);

	    /*
	     * Set flag for switching to the source-tree, if the shared-tree
	     * threshold has exceeded for this group. The next source's
	     * packet is probably the high-rate source.
	     */
	    gdb->join_spt = pim_over_threshold(gdb->mdb.bits_sec, gdb);

	    /*
	     * Process the (*,G) entry itself.
	     */
	    group = gdb->mdb.group;
	    ip_age_one_mroute((rntype *) &gdb->mdb, (va_list) &gdb);

	    /*
	     * Determine if PIM/DVMRP dense-mode grafts should be sent for this
	     * group. A lookup is done because the group may have been
	     * aged out.
	     */
	    gdb = ip_get_gdb(group);
	    if (!gdb) continue;
	    if (TIMER_RUNNING_AND_AWAKE(gdb->send_graft_time)) {
		pim_send_graft(gdb);
	    }

	    /*
	     * Check if any RP reachability messages need to be sent. Only
	     * done when we are RP for a group.
	     */
	    if (TIMER_RUNNING_AND_AWAKE(gdb->send_rpreach)) {
		pim_send_rp_reach(gdb);
		TIMER_START(gdb->send_rpreach, PIM_RP_REACH_INTERVAL);
	    }

	    /*
	     * Clear flag that sends Null Registers to RP. If there are
	     * any gdb's with SET_REG_FLAG set, it will be set in pim_age_rp().
	     */
	    for (rp = (pim_rptype *) pim_configured_rp.qhead; rp; 
		 rp = rp->next) rp->send_null_reg = FALSE;
	    for (rp = (pim_rptype *) pim_learned_rp.qhead; rp;
		 rp = rp->next) rp->send_null_reg = FALSE;

	    /*
	     * If a new RP mapping is learned or configured
	     * Check if this group should change into sparse mode.
	     * Need to delete all sparse-dense interfaces if we are
	     * turning into sparse mode for the first time.
	     * This takes care of routers that are not leaves, and
	     * for groups that pim_store_rp_info() was not executed for.
	     * pim_has_new_rp is reset to FALSE after this loop.
	     */
	    if (pim_has_new_rp && !IS_SPARSE_GROUP(gdb) &&
		pim_find_rp(gdb->mdb.group, 0)) {
		gdb->mdb.flags |= SPARSE_FLAG;
		pim_delete_sparse_dense_oif(gdb);
	    }
	    
	    /*
	     * Check to see if RPs have expired, if so, try other RPs. 
	     */
	    if (IS_SPARSE_GROUP(gdb)) {
		pim_age_rp(gdb);
	    }

	    /*
	     * Check to see if we should send periodic NULL Registers to
	     * known RPs.
	     */
	    pim_send_null_register();

	    /*
	     * Do periodic PIM-SM (send Join/Prune messages) processing.
	     */
	    if (TIMER_RUNNING_AND_AWAKE(gdb->mdb.send_join)) {
		if (!IS_SPARSE_GROUP(gdb)) {
		    TIMER_STOP(gdb->mdb.send_join);
		} else {  
		    pim_send_periodic(gdb);
		    TIMER_START_JITTERED(gdb->mdb.send_join,
					 PIM_MESSAGE_INTERVAL,
					 PIM_JITTER_PCT);
		}
	    }
	}
    }
    
    /*
     * Reset the new RP flag.
     */
    pim_has_new_rp = FALSE;

    /*
     * There may be no more groups depending on an RP. If the Null register
     * flag is not set, make the global RP state UP and don't run the timer.
     * This will allow us to run the group specific timers only for RP
     * reachability status.
     */
    for (rp = (pim_rptype *) pim_configured_rp.qhead; rp; rp = rp->next) {
	if (!rp->send_null_reg) {
	    rp->state = RP_UP;
	    TIMER_STOP(rp->expires);
	}
    }
    for (rp = (pim_rptype *) pim_learned_rp.qhead; rp; rp = rp->next) {
	if (!rp->send_null_reg) {
	    rp->state = RP_UP;
	    TIMER_STOP(rp->expires);
	}
    }

    /*
     * Reset the RPF check timer if it expired.
     */
    if (AWAKE(ip_rpf_check_time)) {
	TIMER_START(ip_rpf_check_time, IP_RPF_CHECK_FREQUENCY);
    }
    
    /*
     * Reset ip_mroute_fs_change flag. We already processed it for all
     * mdbs.
     */
    ip_mroute_fs_change = FALSE;
}

/*
 * ipmulticast_signal_graft -
 *
 * Interface has changed to forwarding state from prune state. Check if
 * we need to send a graft.
 */
static void ipmulticast_signal_graft (gdbtype *gdb, mdbtype *mdb, 
				      midbtype *midb)
{
    /*
     * Process only if the outputQ was empty.
     */
    if (!MDB_OUTPUTQ_NULL(mdb)) {
	return;
    }
          
    /*
     * If the RPF neighbor is a DVMRP machine, the prune times 
     * would be timing out coreectly. Do not send grafts.
     */
    if ((mdb->rpf_type == RPF_DVMRP) ||
	((!pim_find_nbr(mdb->rpf_nbr, mdb->if_input)) &&
	 DVMRP_ACTIVE(mdb->if_input))) {
	return;
    }
    
     /*
      * Set signal to send Graft. We might get a Graft
      * in this state and won't trigger one for an upstream
      * router that might have a negative cache entry. We
      * purposely don't set the gdb->send_graft_time because
      * we only want to send the Graft if we receive one.
      */
    mdb->send_graft = TRUE;
			
    /*
     * If the OIF is a DVMRP tunnel, do send a
     * graft. This is to prevent blackholes
     * in DVMRP regions which use shorter
     * prune times and which have a cisco PIM
     * cloud between them.
     */
    if (DVMRP_TUNNEL(midb->idb)) { 
	GET_TIMESTAMP(gdb->send_graft_time);
    }
}

/*
 * ip_age_one_mroute
 *
 * Process a single multicast route entry. Age out outgoing interface list
 * entries too.
 */
int ip_age_one_mroute (rntype *node, va_list args)
{

    gdbtype        *gdb;
    mdbtype        *mdb;
    midbtype       *midb, *next_midb;
    igmp_cachetype *igmp;
    boolean        connected;
    boolean        old_outputq_null;

    mdb = (mdbtype *) node;
    mdb->uses_sec_last = mdb->uses_sec;
    mdb->bits_sec_last = mdb->bits_sec;
    mdb->bits_min += mdb->bits_sec;
    mdb->bits_age_cnt++;
    if (mdb->bits_age_cnt >= MDB_BPS_AVG_DURATION) {
	mdb->bits_age_cnt = 0;
	mdb->bits_min = 0;
    }

    mdb->uses_sec = 0;
    mdb->bits_sec = 0;
    gdb = va_arg(args, gdbtype *);

    /*
     * If mdb has timed out, free the memory used for the outgoing interface
     * list and delete mdb from radix tree.
     */
    if (TIMER_RUNNING_AND_AWAKE(mdb->expires)) {
	ip_delete_mroute(gdb, mdb);
	return(0);
    }

    /*
     * If source's traffic has gone below group's threshold and we are
     * a downstream router on the shared tree (opposed to a router between
     * the source and the RP), delete the (S,G) and this source's low-rate
     * data will be sent now on the (*,G) tree. Make sure the (S,G) has
     * been up for a little while. We don't want to oscillate this state.
     *
     * This action is done only for (S,G) that this router (as a leaf node)
     * has explicitly joined the source-tree for due to SPT thresholding.
     */
    if (mdb != &gdb->mdb && (mdb->flags & JOIN_FLAG) &&
	ELAPSED_TIME(mdb->uptime) > ONEMIN &&
	!pim_over_threshold(mdb->bits_sec_last, gdb)) {

	/*
	 * The source isn't busy right now. Check if this is a very brief
	 * idle period. Check the 1 minute rate, if it is above the threshold
	 * don't delete the (S,G). Send a triggered join up the RP-tree (for
	 * this source) to undo negative cache state on the shared tree.
	 */
	if (mdb->bits_age_cnt >= (MDB_BPS_AVG_DURATION/6) &&
	    !pim_over_threshold(mdb->bits_min, gdb)) {
	    if (!MDB_OUTPUTQ_NULL(mdb)) {
		pim_send_join_prune(mdb, gdb->mdb.if_input, gdb->mdb.rpf_nbr, 
				    TRUE, FALSE, TRUE, TRUE);
		ip_delete_mroute(gdb, mdb);
	    }
	    return(0);
	}
    }

    /*
     * If it is time to do an RPF consistency check on (S,G), do so.
     * If unicast route does not exist, set the mdb->if_input to NULL.
     */
    if (AWAKE(ip_rpf_check_time)) {
	if (!ip_rpf_check_mroute(gdb, mdb) && &gdb->mdb != mdb) {
	    mdb->if_input = NULL;
	}
    }

    /*
     * Check if register flags need setting if the RP just came up. Set 
     * SET_REG_FLAG in gdb if mdb has it set.     
     */
    if (mdb != &gdb->mdb && mdb->flags & SET_REG_FLAG) {
	gdb->mdb.flags |= SET_REG_FLAG;
    }

    /*
     * Check if any rate-limiting has been configured on an input interface
     * for this (S,G).
     */
    if (mdb->if_input) {
	mdb->in_limit = ipmulticast_rate_limit(mdb, NULL, mdb->if_input, TRUE);
    }

    /*
     * Mdb is still alive, check outgoing interface list for time-out.
     */
    connected = FALSE;
    old_outputq_null = (MDB_OUTPUTQ_NULL(mdb));
    for (midb = (midbtype *) mdb->if_outputQ.qhead; midb; midb = next_midb) {
	next_midb = midb->next;

	if (interface_up(midb->idb) &&
	    (!DVMRP_TUNNEL(midb->idb) || DVMRP_ACTIVE(midb->idb))) {

	    /*
	     * Check if any rate-limiting has been configured on an output
	     * interface for this (S,G).
	     */
	    midb->out_limit = ipmulticast_rate_limit(mdb, midb, midb->idb, FALSE);

	    /*
	     * Determine if an interface should transition to prune state.
	     */
	    if (TIMER_RUNNING_AND_AWAKE(midb->prune_time)) {
		pim_prune_interface(gdb, mdb, midb, MDB_EXPIRATION_TIME);
		continue;
	    }

	    /*
	     * midb has not timed out. Keep in outgoing interface list.
	     */
	    if (TIMER_RUNNING_AND_SLEEPING(midb->expires))
		continue;

	    /*
	     * Keep entry from expiring, if there are directly connected
	     * members.
	     */
	    igmp = igmp_find_group(mdb->group, midb->idb);
	    if (igmp) {
		      
              midb->state = MIDB_FORWARD_STATE;

               /*
                * if dense mode interface, keep the interface in the OIF list.
                */
               if (IS_DENSE_MODE(midb->idb, gdb)) {
                   continue;
               } else if (midb->idb->ip_multicast_dr) {

		   /*
		    * Keep the interface in OIF list if it is the DR in sparse
		    * mode and copy the expiry time from the igmp cache.
		    */
		   COPY_TIMESTAMP(igmp->expire_time, midb->expires);
		   continue;
	       }
	    }
	    
	    /*
	     * Keep entry alive if the interface is dense-mode and PIM 
	     * neighbors are active.
	     * Regardless of interface type, keep entry alive if there are
	     * DVMRP neighbors active. We may have to go from Prune to Forward
	     * state (now that we do DVMRP Pruning).
	     *
	     * Otherwise, delete sparse-mode interface from outgoing list.
	     */
	    if (DVMRP_ACTIVE(midb->idb) || (IS_DENSE_MODE(midb->idb, gdb) &&
		midb->idb->pim_nbrs)) {

		/*
		 * The dense mode interface has timed out, it goes into
		 * forward state. A midb may be in sparse mode, forward
		 * state. But when timed out, it goes into dense forward state.
		 * Dense mode interface in forward state does not time out,
		 * Stop the timer.
		 * (To go into prune state, it needs to receive a prune.)
		 */
		TIMER_STOP(midb->expires);

		/*
		 * If interface was in prune state, move to forward state.
		 */
		if (midb->state == MIDB_PRUNE_STATE) {

		    /*
		     * Find if we need to send a graft.
		     */
		    ipmulticast_signal_graft(gdb, mdb, midb);
		    
		    midb->state = MIDB_FORWARD_STATE;
		    mdb->flags &= ~PRUNE_FLAG;

		    /*
		     * Since the interface in pruned state might have been
		     * created with a long prune timer, Need to check
		     * if the remaining interfaces do not need such long
		     * expiration timers.
		     */
		    ip_store_mdb_timer(mdb);
		}
		continue;
	    }
	} else {

	    /*
	     * Interface went down. Clear DVMRP neighbor timer.
	     */
	    dvmrp_disable_idb(midb->idb);
	}

	/*
	 * Delete mroute's midb due to expiry or interface went down.
	 */
	MROUTE_BUGINF(mdb->group, ("\nMRT: Delete %s/%i from ", 
				   midb->idb->namestring, midb->next_hop));
	if (&gdb->mdb == mdb) {
	    MROUTE_BUGINF(mdb->group, ("(*, %i)", mdb->group));
	} else {
	    MROUTE_BUGINF(mdb->group, 
			  ("(%i/%d, %i)", ip_radix2addr(mdb->source),
			   ip_bitsinmask(ip_radix2addr(mdb->mask)), 
			   mdb->group));
	}
	ip_delete_mroute_idb(mdb, midb->idb, midb->next_hop);
    }

    /*
     * Find out if group still has directly connected members as well as
     * if we are a member. If not, clear the LOCAL and/or CONNECTED flags.
     */
    ip_update_mdb_flags(gdb, mdb);

    /*
     * Keep (*,G) alive if there are any (S,G) entries alive.
     */
    if (&gdb->mdb != mdb) {
	TIMER_START(gdb->mdb.expires, MDB_EXPIRATION_TIME);
    }

    /*
     * Check if fastswitching was turned off on any interface. If so,
     * re-sort outgoing interface list.
     */
    if (ip_mroute_fs_change) ip_check_mdb_fastswitch(mdb);

    /*
     * If there are no more interfaces in the outgoing interface list,
     * set an expiration time for the mdb. This gives us time to send
     * triggered prunes.
     */
    if (!old_outputq_null && MDB_OUTPUTQ_NULL(mdb)) {

	/*
	 * Send (S,G) prune if this not the wildcard entry. Otherwise, send 
	 * prune on (*,G) tree. If this is a sparse-mode group and the
	 * RPF for the source and RP are the same, only send (*,G) prunes. 
	 * The receiver will prune off all (Si,G). 
	 */
	if (&gdb->mdb != mdb) {
	    if (gdb->mdb.rpf_nbr != mdb->rpf_nbr) {
		GET_TIMESTAMP(mdb->send_prune);
	    }
	} else {
	    pim_send_join_prune(mdb, mdb->if_input, mdb->rpf_nbr, FALSE, TRUE, 
				TRUE, TRUE);
	    TIMER_STOP(mdb->send_prune);
	}
    }

    /*
     * Determine if a delayed join should be sent now. If outgoing interface
     * list went to NULL during delay period, don't send join.
     */
    if (&gdb->mdb != mdb && TIMER_RUNNING_AND_AWAKE(mdb->send_join)) {
	if (!MDB_OUTPUTQ_NULL(mdb) || pim_rp_needs_to_join(gdb, mdb)) {
	    pim_send_join_prune(mdb, mdb->if_input, mdb->rpf_nbr, TRUE, FALSE, 
				FALSE, IS_SPARSE_GROUP(gdb));
	}
	TIMER_STOP(mdb->send_join);
    }

    /*
     * If we are the RP and the (S,G) is a negative cache entry, don't send
     * a prune upstream if we still have members denoted in (*,G) outgoing
     * interface list. The negative cache entry means that downstream
     * routers have pruned S off the RP-tree.
     *
     * But if the (S,G) incoming interface is a dense mode interface,
     * or it has a dvmrp neighbor, the prune will be sent.
     */
    if (IS_SPARSE_GROUP(gdb) && ip_ouraddress(gdb->rp.address)) {
	if (mdb->if_input && !DVMRP_ACTIVE(mdb->if_input)) {
	    if (!IS_DENSE_MODE(mdb->if_input, gdb)) {
		if (ip_mroute_has_members(gdb, mdb)) {
		    TIMER_STOP(mdb->send_prune);
		}
	    }
	}
    }

    /*
     * Determine if prune should be sent. This is the mechanism to
     * rate limit prunes.
     */
    if (TIMER_RUNNING_AND_AWAKE(mdb->send_prune)) {
	ipmulticast_send_prune(mdb);
    }
    return(0);
}

/*
 * ip_mroute_has_members
 *
 * This function tests if the (*,G) has outgoing non-pruned interfaces in its
 * outgoing interface list. If there is a single interface but it is the
 * incoming interface for (S,G), then we consider the group not having
 * members for this (S,G) and should send a prune to S.
 */
boolean ip_mroute_has_members (gdbtype *gdb, mdbtype *mdb)
{

    midbtype *midb;

    for (midb = gdb->mdb.if_outputQ.qhead; midb; midb = midb->next) {
	if (midb->state == MIDB_FORWARD_STATE) {
	    if (midb->idb == mdb->if_input) continue;
	    return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * ip_clear_mroute_radix
 *
 * Clear all (S,G) entries in the radix tree pointed to by the (*,G) entry.
 * Caution: this is called by ip_delete_mroute() when deleting the (*,G). 
 * The walk function will call ip_delete_mroute() for each (S,G) were in
 * turn *will not* call this function.
 */
void ip_clear_mroute_radix (gdbtype *gdb)
{

    rnhtype *head;

    rn_walktree(gdb->radix_head->rnh_treetop, ip_clear_one_mroute_radix, gdb);

    /*
     * Might get interrupts for multicast fastswitching in the middle of 
     * free(). Don't want the radix_head to be in an intermediate state.
     */
    head = gdb->radix_head;
    gdb->radix_head = NULL;
    free(head);
}

/*
 * ip_clear_one_mroute_radix
 *
 * Clear single (S,G) entry from radix tree. 
 */
int ip_clear_one_mroute_radix (rntype *node, va_list args)
{

    mdbtype *mdb;
    gdbtype *gdb;

    mdb = (mdbtype *) node;
    gdb = va_arg(args, gdbtype *);

    ip_delete_mroute(gdb, mdb);
    return(0);
}

/*
 * ip_update_mdb_flags
 *
 * Clear/Set connected/local flag from mdb if there are no more directly
 * connected members or we are not a member of this group. For (S,G),
 * the (*,G) can be quickly checked. For (*,G), all interfaces must
 * be looked up in the IGMP cache to determine the membership status.
 */
void ip_update_mdb_flags (gdbtype *gdb, mdbtype *mdb)
{

    igmp_cachetype *igmp;
    midbtype       *midb;

    /*
     * If (S,G) entry, copy CONNECTED and LOCAL flags from (*,G).
     */
    if (&gdb->mdb != mdb) {
	if (!(gdb->mdb.flags & CONNECTED_FLAG)) {
	    mdb->flags &= ~CONNECTED_FLAG;
	} else {
	    mdb->flags |= CONNECTED_FLAG;
	}
	if (!(gdb->mdb.flags & LOCAL_FLAG)) {
	    mdb->flags &= ~LOCAL_FLAG;
	} else {
	    mdb->flags |= LOCAL_FLAG;
	}
	return;
    }

    /*
     * Check outgoing interfaces for (*,G).
     */
    mdb->flags &= ~(CONNECTED_FLAG | LOCAL_FLAG);
    for (midb = (midbtype *) mdb->if_outputQ.qhead; midb; midb = midb->next) {
	igmp = igmp_find_group(mdb->group, midb->idb);
	if (igmp) {
	    mdb->flags |= CONNECTED_FLAG;
	    if (igmp->flags & IGMP_LOCAL_FLAG) {
		mdb->flags |= LOCAL_FLAG;
                TIMER_STOP(mdb->expires);
		return;
	    }
	}
	if (DVMRP_ACTIVE(midb->idb)) mdb->flags |= CONNECTED_FLAG;
    }

    /*
     * Check (*,G) incoming interface for local/direct membership. 
     */
    if (mdb->if_input) {
	igmp = igmp_find_group(mdb->group, mdb->if_input);
	if (igmp) {
	    mdb->flags |= CONNECTED_FLAG;
	    if (igmp->flags & IGMP_LOCAL_FLAG) {
		mdb->flags |= LOCAL_FLAG;
                TIMER_STOP(mdb->expires);
		return;
	    }
	}
	if (DVMRP_ACTIVE(mdb->if_input)) mdb->flags |= CONNECTED_FLAG;
    }

    /*
     * The (*,G) entry timer was stopped previously since the group
     * was local; and now if it does not have any more local members,
     * schedule expiration for the entry.
     */
    if ((AWAKE(mdb->expires)) && (!(mdb->flags & LOCAL_FLAG))) {
	TIMER_START(mdb->expires, MDB_EXPIRATION_TIME);
    }
}

static const uchar ip_mroute_count_banner[] = {
"IP Multicast Statistics - Group count: %u, Average sources per group: %d.%02d
Forwarding Counts: Pkt Count/Pkts per second/Avg Pkt Size/Kilobits per second
Other counts: Total received/RPF failed/Other drops(OIF-null, rate-limit etc)"};

static const uchar ip_mroute_count_str[] = 
{"\n\nGroup: %i, Source count: %u, Group pkt count: %u" };

/*
 * ip_show_mroute_count
 *
 * Display IP multicast related statistic information.
 */
void ip_show_mroute_count (ipaddrtype group, ipaddrtype source,
			   boolean source_specified)
{

    gdbtype    *gdb, *next;
    mdbtype    *mdb;
    ulong      group_count;
    ulong      source_count;
    ulong      packet_count;
    ulong      avg;
    int        i;
    rnhtype    *head;

    /*
     * First scan, get group count and total source count to compute
     * average.
     */
    group_count = 0;
    source_count = 0;
    for (i = 0; i < NETHASHLEN; i++) {
	for (gdb = ip_mroute_cache[i]; gdb; gdb = gdb->next) {
	    group_count++;
	    rn_walktree(gdb->radix_head->rnh_treetop, ip_count_mroute, 
			&source_count, &packet_count);
	}
    }

    if (!group_count) return;

    automore_enable(NULL);
    avg = (100 * source_count) / group_count;
    printf(ip_mroute_count_banner, group_count, avg/100, avg%100);

    /*
     * Second scan, display group, source count, and packet count.
     */
    for (i = 0; i < NETHASHLEN; i++) {
	for (gdb = ip_mroute_cache[i]; gdb; gdb = next) {
	    if (group && group != gdb->mdb.group) {
		next = gdb->next;
		continue;
	    }

	    /*
	     * Count sources and total packets.
	     */
	    source_count = 0;
	    packet_count = 0;
	    rn_walktree(gdb->radix_head->rnh_treetop, ip_count_mroute,
			&source_count, &packet_count);

	    mem_lock(gdb);
	    printf(ip_mroute_count_str, gdb->mdb.group, source_count, 
		   packet_count);

	    if (gdb->radix_head) {
		if (source_specified) {
		    if (source) {
			mdb = ip_get_best_mdb(gdb, source);
			if (!mdb) {
			    printf("\nSource not found");
			    free(gdb);
			    return;
			}
		    } else {
			if (!IS_SPARSE_GROUP(gdb)) {
			    printf("\nNo RP-tree for dense-mode group");
			    free(gdb);
			    return;
			}
			mdb = &gdb->mdb;
		    }
		    ip_count_one_mroute((rntype *) mdb, (va_list) &gdb);
		} else {

		    /*
		     * Display (*,G) first then each source.
		     */
		    if (IS_SPARSE_GROUP(gdb)) {
			ip_count_one_mroute((rntype *) &gdb->mdb, 
					    (va_list) &gdb);
		    }
 		    head = gdb->radix_head;
 		    mem_lock(head);
		    rn_walktree_blocking(gdb->radix_head->rnh_treetop, 
					 ip_count_one_mroute, gdb);
		    free(head);
		}
	    }
	    next = gdb->next;
	    free(gdb);

	    if (group) break;
	    if (automore_quit()) break;
	}
    }
    automore_disable();
}

/*
 * ip_count_mroute
 */
int ip_count_mroute (rntype *node, va_list args)
{

    ulong *scount;
    ulong *pcount;

    scount = va_arg(args, ulong *);
    pcount = va_arg(args, ulong *);

    (*scount)++;
    *pcount += ((mdbtype *) node)->uses;

    return(0);
}

/*
 * ip_count_one_mroute
 */
int ip_count_one_mroute (rntype *node, va_list args)
{
    int     drops;
    gdbtype *gdb;
    mdbtype *mdb;
    
    mdb = (mdbtype *) node;
    gdb = va_arg(args, gdbtype *);

    if (mdb == &gdb->mdb) {
	printf("\n  RP-tree:");
    } else {
	printf("\n  Source: %i/%d,", ip_radix2addr(mdb->source), 
	   ip_bitsinmask(ip_radix2addr(mdb->mask)));
    }
    printf(" Forwarding: %u/%u/%u/%u", mdb->uses, mdb->uses_sec_last,
	   (mdb->uses) ? mdb->bytes/mdb->uses : 0, mdb->bits_sec_last/1000);
    drops = mdb->total_received - mdb->uses - mdb->rpf_failed;
    printf(", Other: %u/%u/%u", mdb->total_received, mdb->rpf_failed,
	   (drops > 0) ? drops : 0);
    
    return(0);
}

/*
 * ip_show_mroute_active
 *
 * Show active multicast sources.
 */
void ip_show_mroute_active (ipaddrtype group, ulong kbps)
{
    gdbtype *gdb, *next;
    rnhtype *head;
    va_list args[3];
    boolean print_group;
    int     i;

    automore_enable(NULL);
    printf("Active IP Multicast Sources - sending >= %d kbps", kbps);

    for (i = 0; i < NETHASHLEN; i++) {
	for (gdb = ip_mroute_cache[i]; gdb; gdb = next) {
	    if (group && group != gdb->mdb.group) {
		next = gdb->next;
		continue;
	    }

	    /*
	     * Print RP-tree rate.
	     */
	    print_group = TRUE;
	    mem_lock(gdb);
	    args[0] = (va_list) kbps;
	    args[1] = (va_list) gdb;
	    args[2] = (va_list) &print_group;
	    ip_walk_active_mroute((rntype *) &gdb->mdb, (va_list) args);

	    /*
	     * Go through each (S,G).
	     */
	    if (gdb->radix_head) {
 		head = gdb->radix_head;
 		mem_lock(head);
		rn_walktree_blocking(gdb->radix_head->rnh_treetop, 
			     ip_walk_active_mroute, kbps, gdb, &print_group);
		free(head);
	    }

	    /*
	     * Exit if displaying a single group.
	     */
	    if (group == gdb->mdb.group) {
		automore_disable();
		free(gdb);
		return;
	    }

	    next = gdb->next;
	    free(gdb);

	    /*
	     * Page wait after each group.
	     */
	    if (!print_group) automore_conditional(0);

	}
	if (automore_quit()) break;
    }
    automore_disable();
}

/*
 * ip_walk_active_mroute
 *
 * Look at all sources of this group to see if they are active. If they
 * meet the kbps requirement, print them out.
 */
int ip_walk_active_mroute (rntype *node, va_list args)
{

    gdbtype    *gdb;
    mdbtype    *mdb;
    boolean    *print_group;
    nametype   *namep;
    sdtype     *sd;
    ulong      kbps, avg_kbps, uptime;
    ipaddrtype source;

    mdb = (mdbtype *) node;
    kbps = va_arg(args, ulong);

    /*
     * Don't care about this source.
     */
    if (mdb->bits_sec_last < (kbps * 1000)) return(0);

    /*
     * Print group line if this is the first active source.
     */
    gdb = va_arg(args, gdbtype *);
    print_group = va_arg(args, boolean *);
    if (*print_group) {
	printf("\n\nGroup: %i, ", mdb->group);

	sd = sd_get_session(mdb->group);
	if (sd) {
	    printf("%s", sd->name);
	} else {
	    namep = reg_invoke_ip_address_lookup(mdb->group);
	    printf("(%s)", (namep) ? namep->name : "?");
	}
	*print_group = FALSE;
    }

    /*
     * Print source info.
     */
    source = ip_radix2addr(mdb->source);
    namep = reg_invoke_ip_address_lookup(source);
    if (mdb == &gdb->mdb) {
	printf("\n  RP-tree:");
    } else {
	printf("\n   Source: %i (%s)", source, (namep) ? namep->name : "?");
    }
    printf("\n     Rate: %d pps/%d kbps(1sec)",
	   mdb->uses_sec_last, mdb->bits_sec_last/1000);
    printf(", %d kbps(last %d secs)",
	   (mdb->bits_age_cnt) ? mdb->bits_min/(mdb->bits_age_cnt * 1000) :
	   mdb->bits_sec_last/1000, mdb->bits_age_cnt);

    uptime = ELAPSED_TIME(mdb->uptime) / ONESEC;
		     
    avg_kbps = (uptime) ? (mdb->bytes * 8) / uptime : 0;
    printf(", %d kbps(life avg)", avg_kbps / 1000);

    if (automore_quit()) return(1);
    return(0);
}

/*
 * ip_show_mroute_static
 *
 * Display static multicast routes.
 */
void ip_show_mroute_static (void)
{
    mroutetype *mroute;

    automore_enable(NULL);

    for (mroute = ip_static_mroutes; mroute; mroute = mroute->next) {
	printf("\nMroute: %i/%d, ", mroute->source, ip_bitsinmask(mroute->mask));
	if (mroute->idb) {
	    printf("interface: %s", mroute->idb->namestring);
	} else {
	    printf("RPF neighbor: %i", mroute->rpf_address);
	}
	printf("\n  Protocol: ");
	if (mroute->pdb) {
	    printf("%s", mroute->pdb->name);
	    if (mroute->pdb->asystem) {
		printf(" %d", mroute->pdb->asystem);
	    }
	} else {
	    printf("none");
	}
	printf(", distance: %d, route-map: %s", mroute->distance,
	       (mroute->route_map ? (char *)mroute->route_map : "none"));

	if (automore_quit()) break;
    }

    automore_disable();
}

/*
 * ip_clear_mroute_command
 *
 * Process "clear ip mroute [<group> [<source-address>]]  | [*]" command.
 */
void ip_clear_mroute_command (parseinfo *csb)
{

    gdbtype    *gdb;
    mdbtype    *mdb;
    ipaddrtype group, source;
    boolean    group_specified, source_specified;

    group_specified = GETOBJ(int,1);
    source_specified = GETOBJ(int,2);
    group = GETOBJ(paddr,1)->ip_addr;
    source = GETOBJ(paddr,2)->ip_addr;

    if (group_specified) {
	gdb = ip_get_gdb(group);
	if (!gdb) {
	    printf("\nGroup %i not found", group);
	    return;
	}

	/*
	 * Delete longest match (S,G).
	 */
	if (source_specified) {
	    mdb = ip_get_best_mdb(gdb, source);
	    if (!mdb) {
		printf("\nNo mroute found");
		return;
	    }
	    ip_delete_mroute(gdb, mdb);
	    return;
	}
	
	/*
	 * Delete entire group.	This is (*,G) as well as (Si,G).
	 */
	ip_delete_mroute(gdb, &gdb->mdb);
	return;
    }

    /*
     * Delete entire multicast routing table.
     */
    ip_clear_mroute_cache(0, NULL);
}

/*
 * ip_clear_mroute_cache
 *
 * If rp is non-zero, clear multicast routing entries using that rp; or
 * if acl is non-zero, clear multicast routing entries denied by the acl;
 * otherwise clear the entire multicast routing table.
 */
void ip_clear_mroute_cache (ipaddrtype rp, acl_headertype *acl)
{

    gdbtype *gdb, *next;
    int     i;

    for (i = 0; i < NETHASHLEN; i++) {
	for (gdb = ip_mroute_cache[i]; gdb; gdb = next) {	
	    next = gdb->next;

	    /*
	     * Keep this entry if it uses a different RP from the one
	     * passed in. Note that an entry is also removed 
	     * if it has no RP cached, and pim_find_rp() finds an RP for it.
	     */
	    if (rp && rp != gdb->rp.address) {
		   if (gdb->rp.address) continue;
		   if (!pim_find_rp(gdb->mdb.group, rp)) continue;
	    }

	    /*
	     * If this entry's group address is not denied by the acl,
	     * keep it.
	     */
	    if (acl && nfast_check(acl, gdb->mdb.group)) continue;

	    ip_delete_mroute(gdb, &gdb->mdb);
	}
    }
}

/*
 * ip_rpf_check_mroute
 *
 * Do RPF lookup on this mroute to determine if RPF neigbhor and incoming
 * interface is still the same
 */
boolean ip_rpf_check_mroute (gdbtype *gdb, mdbtype *mdb)
{
    
    idbtype    *idb;
    midbtype   *midb;
    ipaddrtype addr, source, mask, rpf_nbr;
    ulong      mask_count;
    boolean    rp_entry, rp_bit_entry;
    uchar      rpf_type;

    rp_entry = FALSE;
    rp_bit_entry = (mdb->flags & RP_BIT_FLAG);
    addr = ip_radix2addr(mdb->source);
    mask_count = ip_bitsinmask(ip_radix2addr(mdb->mask));

    /*
     * If (*,G), do RPF lookup on RP address, if any. Otherwise, do RPF
     * lookup on source.
     */
    if (&gdb->mdb == mdb) {

	/*
	 * Don't do lookup if there are no RPs or we are the RP for the
	 * group.
	 */
	if (!gdb->rp.address || ip_ouraddress(gdb->rp.address)) return(TRUE);
	rp_entry = TRUE;
	rp_bit_entry = FALSE;
    }

    /*
     * If (S,G) has RP-bit set, the RPF information is the same as the RP.
     */
    if (rp_bit_entry) {
	rpf_nbr = gdb->mdb.rpf_nbr;
	idb = gdb->mdb.if_input;
	rpf_type = gdb->mdb.rpf_type;
    } else {
	rpf_nbr = 0;
	idb = ip_rpf_lookup(addr, ip_radix2addr(mdb->mask), &source, &mask, 
			    &rpf_nbr, &rpf_type, ROUTINGDEPTH);
	if (!idb) {
	    MROUTE_BUGINF(mdb->group, 
		  ("\nMRT: RPF lookup failed for (%i/%d, %i) %s", addr,
		   mask_count, mdb->group, (rp_entry) ? "RP" : " "));

	    /*
	     * If RPF neighbor to RP has gone away, expire RP right now. This
	     * allows us to change to another RP if available.
	     */
	    if (rp_entry) {
		GET_TIMESTAMP(gdb->rp.expires);
		pim_age_rp(gdb);
	    }
	    return(FALSE);
	}

	/*
	 * If RP is directly connected, store RP address as RPF neighbor.
	 */
	if (rp_entry) {
	    if (samecable(addr) == idb) rpf_nbr = addr;
	}
    }

    mdb->rpf_type = rpf_type;

    /*
     * Nothing has changed.
     */
    if (idb == mdb->if_input && rpf_nbr == mdb->rpf_nbr) return(TRUE);

    /*
     * Interface has not changed but RPF neighbor has. If the RPF neighbor
     * was chosen from an Assert, it overrides the unicast routing table.
     * Otherwise, reset the metrics stored in the mdb so the next Assert
     * starts from scratch. Treat like the interface HAS changed if nbma.
     */
    if (!(idb && idb->pim_nbma_mode) && idb == mdb->if_input) {
	if (mdb->use_assert_rpf) {
	    mdb->use_assert_rpf--;
	    if (mdb->use_assert_rpf) return(TRUE);
	}
	mdb->rpf_nbr = rpf_nbr;
	mdb->metric_pref = -1;
	mdb->metric = -1;
	MROUTE_BUGINF(mdb->group,
	      ("\nMRT: RPF neighbor is %i for (%i/%d, %i)%s%s", rpf_nbr,
	       addr, mask_count, mdb->group, (rp_entry) ? " RP " : " ",
	      (rp_bit_entry) ? "RP-bit" : " "));

	/*
	 * Send Graft message to new RPF neighbor for dense-mode groups.
	 */
	if (!IS_SPARSE_GROUP(gdb) && !MDB_OUTPUTQ_NULL(mdb)) {
	    GET_TIMESTAMP(gdb->send_graft_time);
	    mdb->send_graft = TRUE;
	}
	return(TRUE);
    }

    /*
     * Incoming interface has changed. Delete new incoming interface from
     * outgoing list. Add old incoming interface to outgoing list if is in 
     * the (*,G) outgoing interface list. Record new RPF neighbor and incoming
     * interface.
     */
    ip_delete_mroute_idb(mdb, idb, rpf_nbr);
    midb = ip_get_midb(&gdb->mdb.if_outputQ, mdb->if_input,
		       mdb->rpf_nbr, IS_SPARSE_GROUP(gdb));
    if (midb) {
	ip_add_mroute_idb(gdb, mdb, mdb->if_input, midb->next_hop, 
			  MDB_EXPIRATION_TIME, MIDB_FORWARD_STATE, TRUE);
    }
    mdb->rpf_nbr = rpf_nbr;
    mdb->if_input = idb;
    mdb->in_limit = ipmulticast_rate_limit(mdb, NULL, idb, TRUE);

    MROUTE_BUGINF(mdb->group, 
	  ("\nMRT: RPF change %i/%s for (%i/%d, %i)%s%s", rpf_nbr,
	   idb ? idb->namestring : " ", addr, mask_count, mdb->group,
	   (rp_entry) ? " RP " : " ", (rp_bit_entry) ? "RP-bit" : " "));

    /*
     * We want to immediately send out joins/prunes so that the change
     * in topology is reflected in mroute state upstream.
     * Now, restart the delayed join timer with a small jittered value.
     * The maximum delay is 3 seconds, jitter is 99% of the maximum
     * value.
     *
     * If this is a dense-mode group, immediately send out a Graft message.
     */
    if (IS_SPARSE_GROUP(gdb)) {
	TIMER_START_JITTERED(mdb->send_join, 3 * ONESEC, 99);
    } else {
	
	/*
 	 * Dense mode group. Send a graft if OIF list is non-empty.
 	 * Also, send a graft if the incoming interface has local/connected
 	 * members.
 	 */
	if (!MDB_OUTPUTQ_NULL(mdb) || igmp_find_group(mdb->group, idb)) {
	    GET_TIMESTAMP(gdb->send_graft_time);
	    mdb->send_graft = TRUE;
	}
    }

    return(TRUE);
}

/*
 * ip_multicast_enabled
 *
 * Test to see if multicast routing is enabled on interface. Call by the
 * SNMP MIB functions.
 */
boolean ip_multicast_enabled (idbtype *swidb, long* index)
{
    return(swidb->ip_mrouting);
}

/*
 * ip_check_mdb_fastswitch
 *
 * Check to see if fastswitching is on for interface. If not on for an
 * incoming interface, take out of cache. If not no for outgoing interface,
 * free MAC header and put at end of outgoing interface list. This allows
 * part of the list to be fastswitched and the rest to be process switched.
 */
void ip_check_mdb_fastswitch (mdbtype *mdb)
{

    midbtype *midb, *next;
    boolean set_fast_flag;
    
    /*
     * Incoming interface check.
     */
    if (mdb->fastswitch && mdb->if_input && !mdb->if_input->ip_multicast_fs) {
	ip_mdb_toggle_fs(mdb, mdb->if_input->ip_multicast_fs, 
			 "ip_check_mdb_fastswitch");
    }

    /*
     * Outgoing interface check.
     */
    set_fast_flag = TRUE;
    for (midb = (midbtype *) mdb->if_outputQ.qhead; midb; midb = next) {
	next = midb->next;
	if (!midb->idb->ip_multicast_fs) {

	    set_fast_flag = FALSE;

	    /*
	     * We don't want a slow-switched interface in the middle of
	     * a list of fastswitched interfaces.
	     */
	    if (midb->mac_header || (next && next->idb->ip_multicast_fs)) {
		if (midb->mac_header) {
		    free(midb->mac_header);
		    midb->mac_header = NULL;
                    midb->mac_length = 0;
		}

		/*
		 * Take off queue and put on tail.
		 */
		unqueue(&mdb->if_outputQ, midb);
		enqueue(&mdb->if_outputQ, midb);
	    }
	}
	if (midb->idb->ip_multicast_fs && !midb->mac_header) {

	    /*
	     * Take off queue and put on head.
	     */
	    unqueue(&mdb->if_outputQ, midb);
	    requeue(&mdb->if_outputQ, midb);
	}
    }

    /*
     * Determine if FAST_FLAG should be set in mdb.
     */
    if (set_fast_flag == TRUE)
	mdb->flags |= FAST_FLAG;
    else
	mdb->flags &= ~FAST_FLAG;
}

/*
 * ip_adjust_tunnel_mroutes
 *
 * The next hop encapsulation for a tunnel endpoint has changed. Delete
 * MAC header in midb for each multicast routing entry.
 */
void ip_adjust_tunnel_mroutes (idbtype *idb)
{

    gdbtype    *gdb;
    int        i;

    for (i = 0; i < NETHASHLEN; i++) {
	for (gdb = ip_mroute_cache[i]; gdb; gdb = gdb->next) {

	    /*
	     * Process each (S,G) entry.
	     */
	    rn_walktree(gdb->radix_head->rnh_treetop, 
			ip_adjust_one_tunnel_mroute, idb);

	    /*
	     * Process the (*,G) entry.
	     */
	    ip_adjust_one_tunnel_mroute((rntype *) &gdb->mdb, (va_list) &idb);
	}
    }
}

/*
 * ip_adjust_one_tunnel_mroute
 *
 * If tunnel is outgoing interface list of an mdb, delete reference to
 * MAC header (so new one can be built at process level).
 */
int ip_adjust_one_tunnel_mroute (rntype *node, va_list args)
{

    mdbtype  *mdb;
    idbtype  *idb;
    midbtype *midb;
    uchar    *hdr;

    mdb = (mdbtype *) node;
    idb = va_arg(args, idbtype *);

    midb = ip_get_midb(&mdb->if_outputQ, idb, 0, FALSE);
    if (!midb) return(0);

    /*
     * Free MAC header area if built. Use stack variable so fastswitching
     * interrupt doesn't use a freed memory.
     */
    if (midb->mac_header) {
	hdr = midb->mac_header;
	midb->mac_header = NULL;
	midb->mac_length = 0;
	free(hdr);

	ip_mroute_set_min_mtu(mdb);
    }
    return(0);
}

/*
 * ip_ratelimit_command
 *
 * Process the following interface subcommands:
 *
 * ip multicast rate-limit [in | out] group-list <acl> source-list <acl> <kbps>
 * no ip multicast rate-limit [in | out] group-list <acl> source-list <acl>
 *
 */
void ip_ratelimit_command (parseinfo *csb)
{

    ratetype *rate, *last;
    idbtype  *idb;
    acl_headertype *group_acl = NULL, *source_acl = NULL;
    boolean  in = FALSE;
    ulong    kbps = 0;
    ushort   media = SD_MEDIA_UNDEFINED;

    idb = csb->interface;

    if (!csb->nvgen) {
	in = GETOBJ(int,1);
	group_acl = (GETOBJ(int,2) || *GETOBJ(string,2))
	    ? access_parse_acl(GETOBJ(int,2), GETOBJ(string,2),
			       GETOBJ(int,2) == 0, ACL_IP_NAMED_SIMPLE)
	    : NULL;
	source_acl = (GETOBJ(int,3) || *GETOBJ(string,3))
	    ? access_parse_acl(GETOBJ(int,3), GETOBJ(string,3),
			       GETOBJ(int,3) == 0, ACL_IP_NAMED_SIMPLE)
	    : NULL;
	kbps = GETOBJ(int,4) * 1000;
	media = GETOBJ(int,5);
    }
    
    /*	
     * Go through linked list to either find an existing entry or NVgen each
     * entry for a given interface.
     */
    last = (ratetype *) &idb->ip_multicast_rate_list;
    for (rate = idb->ip_multicast_rate_list; rate; rate = rate->next) {
	if (csb->nvgen) {
	    nv_write(TRUE, "%s %s", csb->nv_command, 
		     (rate->in) ? "in" : "out");
	    if (rate->group_acl)
		nv_add(TRUE, " group-list %s", rate->group_acl->name);
	    if (rate->source_acl)
		nv_add(TRUE, " source-list %s", rate->source_acl->name);
	    nv_add(rate->media_type != SD_MEDIA_UNDEFINED, " %s",
		   sd_media_strings[rate->media_type]);
	    nv_add(TRUE, " %d", rate->kbps/1000);
	    
	} else {
	    if (rate->in == in && rate->group_acl == group_acl &&
		rate->source_acl == source_acl) {
		break;
	    }
	}
	last = rate;
    }

    /*
     * Did NV generation, return.
     */
    if (csb->nvgen) return;

    /*
     * Configuring a rate-limit.
     */
    if (csb->sense) {
	if (!rate) {
	    rate = malloc(sizeof(ratetype));
	    if (!rate) return;
	    rate->next = last->next;
	    last->next = rate;
	    rate->group_acl = group_acl;
	    rate->source_acl = source_acl;
	    rate->in = in;
	}
	rate->kbps = kbps;
	rate->media_type = media;
    } else {
	if (!rate) {
	    printf("\nEntry not found");
	    return;
	}

	/*
	 * Deleting a rate-limit.
	 */
	last->next = rate->next;
	rate->next = NULL;
	free(rate);
    }
}

/*
 * ipmulticast_rate_limit
 *
 * Check configuration information to determine if there is rate-limiting for
 * this (S,G). If there isn't set the limit to infinity.
 */
ulong ipmulticast_rate_limit (mdbtype *mdb, midbtype *midb, idbtype *idb,
			      boolean in)
{
    ratetype   *rate;
    ipaddrtype src;
    ulong      *port_ptr;
    
    if (!idb) return(-1);

    port_ptr = (in ? &mdb->port_number : &midb->port_number);
    
    for (rate = idb->ip_multicast_rate_list; rate; rate = rate->next) {
	if (rate->in != in) continue;
	if (!nfast_check(rate->group_acl, mdb->group)) continue;
	src = ip_radix2addr(mdb->source);
	if (!nfast_check(rate->source_acl, src)) continue;

	if (rate->media_type == SD_MEDIA_UNDEFINED) {
	    *port_ptr = 0;
	} else {
	    if (!sd_get_media_port(mdb->group, rate->media_type, port_ptr)) {
		*port_ptr = -1; /* Set this to -1 so that port will
				 * never match when rate-limit check is
				 * performed
				 */
		return (-1);
	    }
	}
	return(rate->kbps);
    }
    return(-1);
}

/*
 * ip_create_mroute_state
 *
 * Create an (S,G) entry for RSVP. This may be a temporary function. This
 * routine should really do the same thing (or be replace by) ip_mroute().
 *
 * This is here to get RSVP over dense-mode PIM to work for demo purposes.
 *
 * And now (later) it has been extended to set the PIM register flag for
 * sparse-mode groups. This function works in hand with ipmulticast_loopback()
 * for sparse-mode groups.
 */
mdbtype *ip_create_mroute_state (ipaddrtype source, ipaddrtype group)
{

    gdbtype    *gdb;
    mdbtype    *mdb;
    idbtype    *idb;
    ipaddrtype src, rpf_nbr, mask;
    uchar      rpf_type;

    /*
     * Get group state. If doesn't exist create it.
     */
    gdb = ip_get_gdb(group);
    if (!gdb) {
	ip_add_mroute(group, 0, MASK_HOST, NULL, 0, 0, MDB_EXPIRATION_TIME);
	gdb = ip_get_gdb(group);
	if (!gdb) return(NULL);

	/*
	 * Determine if group should be in sparse-mode.
	 */
	if (pim_find_rp(group, 0)) {
	    pim_get_all_rps(gdb, TRUE, FALSE);
	} else {
	    gdb->mdb.flags &= ~SPARSE_FLAG;
	}

	/*
	 * Build outgoing interface list for (*,G).
	 */
	ip_build_outputQ(NULL, gdb, &gdb->mdb);
    }

    /*
     * Get or create (S,G) entry. If no (S,G) found, do some sparse-mode
     * group checks to determine if (*,G) should be given to RSVP.
     */
    mdb = ip_get_best_mdb(gdb, source);

    /*
     * Do RPF lookup on source address.
     */
    idb = NULL;
    if (!mdb) {
	rpf_nbr = 0;
	idb = ip_rpf_lookup(source, MASK_HOST, &src, &mask, &rpf_nbr, 
			    &rpf_type, ROUTINGDEPTH);
    }

    /*
     * Use (*,G) state in almost every sparse-mode case unless we are a 
     * first-hop router directly connected to the source.
     */
    if (IS_SPARSE_GROUP(gdb) && gdb->mdb.if_input) {
	if (!mdb) {
	    if (!idb || !idb->ip_multicast_dr || 
		!ip_address_on_idb(source, idb)) mdb = &gdb->mdb;
	}
    }

    /*
     * Create the source state.
     */
    if (!mdb && idb) {
	mdb = ip_add_mroute(group, source, MASK_HOST, idb, rpf_nbr, 
			    rpf_type, MDB_EXPIRATION_TIME);
	if (!mdb) return(NULL);

	ip_build_outputQ(&gdb->mdb.if_outputQ, gdb, mdb);
	mdb->flags |= (gdb->mdb.flags & LOCAL_FLAG);
	ip_store_mdb_timer(mdb);

	/*
	 * If sparse-mode group and we are DR on the incoming interface,
	 * set PIM register flags.
	 */
	if (IS_SPARSE_GROUP(gdb)) {
	    if (idb->ip_multicast_dr && ip_address_on_idb(source, idb)) {
		mdb->register_flag = TRUE;
	    }
	    if (DVMRP_ACTIVE(idb)) mdb->register_flag = TRUE;
	}
    }
    return(mdb);
}

/*
 * ipmulticast_send_prune
 *
 * Send either PIM or DVMRP Prune message depending on what type of multicast
 * router is upstream.
 */
void ipmulticast_send_prune (mdbtype *mdb)
{

    midbtype   *midb;
    idbtype    *idb;
    boolean    non_rpf_prune_sent;
    ipaddrtype nbr;

    TIMER_STOP(mdb->send_prune);
    
    /*
     * Look at all interfaces in the outgoing list for a specific (S,G). This
     * checks to see if we should send a Prune on a point-to-point non-RPF
     * interface to either a PIM neighbor or a DVMRP neighbor.
     */
    non_rpf_prune_sent = FALSE;
    for (midb = (midbtype *) mdb->if_outputQ.qhead; midb; midb = midb->next) {
	
	if (!midb->send_prune) continue;

	idb = midb->idb;
	if (idb->pim_nbrs) {
	    nbr = idb->pim_nbrs->nbr_address;
	    pim_send_join_prune(mdb, idb, nbr, FALSE, FALSE, FALSE, FALSE);
	} else {
	    if (DVMRP_ACTIVE(idb) && 
		DVMRP_PRUNER(idb, idb->dvmrp_nbr_version)) {
		dvmrp_send_prune(mdb, idb);
	    }
	}
	midb->send_prune = FALSE;
	non_rpf_prune_sent = TRUE;
    }

    /*
     * If we sent a prune to the non-RPF neighbor, dont try to send 
     * an RPF prune.
     */
    if (non_rpf_prune_sent) {
	return;
    } 

    /*
     * Check if RPF neighbor Prune needs to be sent.
     */
    idb = mdb->if_input;
    if (!idb) return;

    /*
     * If neighbor is a PIM router, always give precedence to PIM. This
     * router could be a cisco running DVMRP unicast routing and PIM multicast
     * routing. In this case we want to send PIM Prunes.
     */
    if (pim_find_nbr(mdb->rpf_nbr, idb)) {
	pim_send_join_prune(mdb, mdb->if_input, mdb->rpf_nbr, FALSE, FALSE, 
			FALSE, FALSE);
	return;
    }

    /*
     * If DVMRP tunnel and neighbor doesn't do pruning, don't send prune.
     */
    if (DVMRP_TUNNEL(idb) && 
	!DVMRP_PRUNER(idb, idb->dvmrp_nbr_version)) return;

    /*
     * Upstream neighbor is definitely a DVMRP-only router that has advertised
     * the source to us. Send it a Prune.
     */
    if (mdb->rpf_type == RPF_DVMRP) {
	dvmrp_send_prune(mdb, NULL);
	return;
    }

    /*
     * We may be doing DVMRP interaction over a LAN where we don't cache
     * DVMRP reports. If DVMRP is active, send a Prune. The upstream neighbor
     * may be a DVMRP router. If not, it will not understand it.
     */
    if (DVMRP_ACTIVE(idb)) {
	dvmrp_send_prune(mdb, NULL);
	return;
    }
}

/*
 * ipmulticast_loopback
 *
 * This is called from ipsendnet() when a multicast datagram is locally 
 * originated. If this system is doing IP multicast routing and this system
 * is the PIM designated router for the interface the packet is being sent on
 * and the group is sparse-mode, we need to get the packet to the RP.
 */
boolean ipmulticast_loopback (paktype *pak)
{
    
    gdbtype   *gdb;
    mdbtype   *mdb;
    idbtype   *idb;
    iphdrtype *ip;

    ip = (iphdrtype *) ipheadstart(pak);
    idb = pak->if_output;

    /*
     * Don't loopback packets on point-to-point links.
     */
    if (is_p2p(idb)) return(FALSE);

    /*
     * If this is a reserved multicast group, we don't forward them. 
     */
    if (IPMULTICAST_RSVD(ip->dstadr)) return(FALSE);

    /*
     * If we are not and multicast router or not the DR for this interface, 
     * don't worry about it.
     */
    if (!ip_mrouter_running || !idb->ip_multicast_dr) return(FALSE);

    gdb = ip_get_gdb(ip->dstadr);
    if (!gdb) {
	if (pim_find_rp(ip->dstadr, 0)) return(TRUE);
    } else {
	if (IS_SPARSE_GROUP(gdb)) {
	    mdb = ip_get_best_mdb(gdb, ip->srcadr);
	    if (!mdb || mdb->register_flag) return(TRUE);
	}
    }
    return(FALSE);
}
 
/*
 * The following functions are to support ip multicast helper-map.
 * 
 */

/*
 * Parse the helper-map command.
 */
void ipmulticast_helper_command (parseinfo *csb)
{
    ipmhelpertype  *ptr;
    idbtype        *idb;
    ipaddrtype     address_before, address_after;
    ulong          listnum;
    acl_headertype *acl;
    
    idb = csb->interface;
 
    /*
     * Do Nvgen of all helper-maps for this interface.
     */
    if (csb->nvgen) {
	for (ptr = (ipmhelpertype *) idb->ip_mhelpermapQ.qhead;
	     ptr;
 	     ptr = ptr->next) {
	    
	    nv_write(TRUE, "%s ", csb->nv_command);

	    if (ptr->address_before) {
		nv_add(TRUE, "%i ", ptr->address_before);
	    } else {
		nv_add(TRUE, "broadcast ");
	    }

	    nv_add(TRUE, "%i ", ptr->address_after);

	    if ((ptr->acl) && (ptr->acl->name))
 		nv_add(TRUE, "%s", ptr->acl->name);
         
	    if (ptr->ttl) 
		nv_add(TRUE, " ttl %d", ptr->ttl);
 	}
 	return;
    }

    address_before = GETOBJ(paddr,1)->ip_addr;
    address_after = GETOBJ(paddr,2)->ip_addr;
    
    /*
     * Do some sanity checking before accepting the map.
     */
    if (!address_before) { 	/* broadcast keyword was entered */
	if (!IPMULTICAST(address_after)) {
	    printf(" Multicast address required");
	    return;
	}
    } else if (!IPMULTICAST(address_before)) {
	printf(" Multicast address or broadcast keyword required");
	return;
    }
    
    /*
     * See if we want to remove a map.
     */
    for (ptr = (ipmhelpertype *) idb->ip_mhelpermapQ.qhead;
 	 ptr; ptr = ptr->next) {
 	if ((ptr->address_after == address_after) &&
 	    (ptr->address_before == address_before)) {
	    if (!csb->sense) {
		unqueue(&idb->ip_mhelpermapQ, ptr);

		/*
		 * Note that we dont remove the igmp group when we delete th
		 * helper map. This is because there could be other helper 
		 * maps needing the same group or the 
		 * user may have configured it intentionally.
		 */
		free(ptr);
	    }
 	    return;
	}
    }

    /*
     * If we are adding a map, enqueue the map into
     * the mhelperQ, and add igmp group membership,
     * if the former address is a multicast address.
     */
    if (csb->sense) {
	listnum = GETOBJ(int, 1);
	if (!(acl = access_parse_acl(listnum, GETOBJ(string,1),
				     listnum == 0, ACL_IP_NAMED_EXTENDED)))
	    return;
	
	ptr = malloc(sizeof(ptr));
	if (ptr) {
	    ptr->address_after = address_after;
	    ptr->address_before = address_before;
	    ptr->acl = acl;
	    ptr->ttl = 0; 

            /*
             * Add TTL remapping option.
             */
	    if (!IPMULTICAST(address_before) && GETOBJ(int, 2)) {
 		ptr->ttl = GETOBJ(int, 2);
                if (ptr->ttl) {
		    printf(" TTL remapping will occur only if there are no unicast helper maps");
		}
 	    }
	    
	    enqueue(&idb->ip_mhelpermapQ, ptr);
 	    if (IPMULTICAST(address_before))
		igmp_join_group_command(address_before, idb, TRUE);
	}
    }
}


/*
 * Called from udp_forward through a registry if a multicast/broadcast
 * helper map has been configured.
 *
 * If the address to be helpered is a multicast address,
 * check the incoming broadcast address, check the access-list for
 * the appropriate port no and then forward the packet.
 *
 * If the address to be helpered is a broadcast address, check
 * the incoming multicast address, the access list and forward the
 * packet.
 *
 */
boolean ipmulticast_helper_forward (paktype *pak)
{
    ipmhelpertype *ptr;
    iphdrtype     *ip, *ipcopy;
    udptype       *udp, *udpcopy;
    idbtype       *input;
    paktype       *pakcopy;
    ipaddrtype    address;

    /*
     * Bail if it is a locally generated packet.
     */
    input = pak->if_input;
    if (!input) {
	return(FALSE);
    }
    
    ip  = (iphdrtype *)ipheadstart(pak);
    udp = (udptype *)ipdatastart(pak);

    /*
     * Scan through the list of helpers and forward the packet if
     * necessary.
     */
    for (ptr = (ipmhelpertype *) input->ip_mhelpermapQ.qhead; ptr;
		 ptr = ptr->next) {

	/*
	 * Do an access check.
	 */
	if (!slow_check(ptr->acl, TRUE, input, ip->prot, ip->srcadr,
			ip->dstadr, udp->sport, udp->dport, FALSE,
			ip->tos)) {
	    continue;
	}

	/*
	 * If the incoming packet is a multicast packet, check to
	 * make sure if this group has to be forwarded.
	 */
	if (IPMULTICAST(ip->dstadr)) {
	    if (ptr->address_before != ip->dstadr) {
	        continue;
	    }
	}
         
	/*
	 * Do not forward packets with ttl == 0, if ttl re-mapping is
	 * not configured.
	 */
	if (!ptr->ttl) {
 	    if (ip->ttl == 0)
 		continue;
 	    if (!(pak->flags & PAK_MCICHECKED)) {
 		if (ip->ttl == 1)
 		    continue;
 	    }
 	} 	
	
	/*
	 * Make a copy of the packet and send along for each
	 * helper address on the input interface.
	 */
	pakcopy = pak_duplicate(pak);
	if (!pakcopy)
	    continue;
	address = ptr->address_after;
	udpcopy = (udptype *)ipdatastart(pakcopy);

	/*
	 * If we are translating broadcast to multicast,
	 * use multicast forwarding engine.
	 */
	if (IPMULTICAST(address)) {
	    ipcopy = (iphdrtype *)ipheadstart(pakcopy);
	    
	    if (ptr->ttl) {
 		
 		/*
		 * Map the new ttl. IP CRC is recomputed later anyways.
 		 */
 		ipcopy->ttl = ptr->ttl;
	    }
	    ipcopy->dstadr = address;

	    /*
	     * Compute new checksums.
	     */
	    udp_compute_checksums(ipcopy, udpcopy);

	    /*
	     * Call mforward to send the mcast datagram.
	     */
	    if (reg_invoke_ip_mforward(pakcopy))
		continue;

	    retbuffer(pakcopy);
	    continue;
	}
	
	if (!ip_forward_check(pakcopy, address)) {
	    retbuffer(pakcopy);
	    continue;
	}
	
	if (!udp_flood(pakcopy, udpcopy))
	    retbuffer(pakcopy);
    }
    return(TRUE);
}


/*
 * ipmulticast_test_boundary
 *
 * Test whether an interface is a boundary for the given group.
 * Return TRUE if yes.
 */
boolean ipmulticast_test_boundary(idbtype *idb, ipaddrtype group)
{

  if (!idb) return (FALSE);

  return (!nfast_check(idb->ipmulticast_boundary, group));

}

/*
 * ip_store_mdb_timer
 *
 * Search the olist of the mroute entry and record the appropriate
 * maximum expiration time in mdb->expiration_time
 */
void ip_store_mdb_timer(mdbtype *mdb)
{
    midbtype	*midb;
    ulong	timer = 0, time_left = 0;

    for (midb = (midbtype *) mdb->if_outputQ.qhead; midb;
	 midb = midb->next) {
	time_left = TIME_LEFT_SLEEPING(midb->expires);
	if (time_left > timer) {
	    timer = time_left;
	}
    }

    mdb->expiration_time = (timer <= MDB_EXPIRATION_TIME) ?
	                    MDB_EXPIRATION_TIME : timer;
}

/*
 * ipmulticast_add_idb
 *
 * Add an idb to our private list of IP Multicast-enabled idbs.  We generally 
 * add an idb to this list only if pim and/or igmp is enabled on the idb.
 */
void ipmulticast_add_idb (idbtype *idb)
{
    ulong idblist_type;

    /*
     * Check for ipmulticast swidb list.
     */
    if (idb_get_listhead(SWIDBLIST_IP_MCAST) == NULL) {
	if (!idb_create_list(SWIDBLIST_IP_MCAST, &idblist_type) ||
	    idblist_type != SWIDBLIST_IP_MCAST)
	    return;
    }

    idb_add_swidb_to_list(idb, SWIDBLIST_IP_MCAST);
}

/*
 * ipmulticast_remove_idb
 *
 * Remove an idb from our private list of IP Multicast-enabled idbs.
 */
void ipmulticast_remove_idb (idbtype *idb)
{
    if (!idb->pim_enabled && !idb->igmp_enabled)
	idb_remove_from_list(idb, SWIDBLIST_IP_MCAST);
}

/*
 * ipmulticast_functional_command
 *
 * Process the 'ip multicast use-functional' interface command.
 */
void ipmulticast_functional_command (parseinfo *csb)
{
    idbtype *idb;
    hwidbtype *hwidb;
    
    idb = csb->interface;
    hwidb = idb->hwptr;
        
    if (csb->nvgen) {
	nv_write(idb->ip_multicast_use_functional, "%s", csb->nv_command);
    } else {
	idb->ip_multicast_use_functional = csb->sense ? TRUE : FALSE;
	if (hwidb->set_extra_addrs)
	    (*hwidb->set_extra_addrs)(hwidb);
    }
}
