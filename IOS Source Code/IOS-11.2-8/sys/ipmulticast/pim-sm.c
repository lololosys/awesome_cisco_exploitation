/* $Id: pim-sm.c,v 3.11.10.29 1996/09/12 21:09:29 chwhite Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/pim-sm.c,v $
 *------------------------------------------------------------------
 * pim-sm.c - Protocol Independent Multicast support. Sparse-mode specific
 *            functions.
 *
 * March, 1994.
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * PIM is a dynamic IP multicasting protocol used to build shared and
 * source-based trees to route multicast datagrams.
 *------------------------------------------------------------------
 * $Log: pim-sm.c,v $
 * Revision 3.11.10.29  1996/09/12  21:09:29  chwhite
 * CSCdi61325:  (*,G) JOINS received by the RP do not trigger (S,G) JOINS
 * to source
 * Branch: California_branch
 *
 * Revision 3.11.10.28  1996/09/10  05:31:54  lwei
 * CSCdi68465:  RP information is too difficult to change in last-hop
 *              routers
 * Branch: California_branch
 *
 * Revision 3.11.10.27  1996/08/28  23:50:36  dino
 * CSCdi67491:  PIM Register messages go throughout MBONE because of a
 *              multicast RP
 * Branch: California_branch
 *
 * Revision 3.11.10.26  1996/08/28  12:51:14  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.11.10.25  1996/08/22  01:37:33  lwei
 * CSCdi66702:  PIM-1-FAST_RP_REACHABLE log message should be removed
 * Branch: California_branch
 *
 * Revision 3.11.10.24  1996/08/21  01:21:23  lwei
 * CSCdi66170:  Sparse-dense/IGMP breaks triggered joins for sparse mode
 *              groups
 * Branch: California_branch
 *
 * Revision 3.11.10.23  1996/08/14  05:03:38  lwei
 * CSCdi64491:  Check (*,G) OIF list when new PIM neighbor comes up
 * Branch: California_branch
 *
 * Revision 3.11.10.22  1996/08/12  23:50:46  lwei
 * CSCdi65760:  RP should update (S,G) uptime for itself
 * Branch: California_branch
 *
 * Revision 3.11.10.21  1996/08/09  03:19:12  mleelani
 * CSCdi61876:  (*,G) mcache entries do not work
 * Branch: California_branch
 * o Correct SPT threshold behaviour.
 * o Allow (*, G) fastswitching if on the shared tree.
 *
 * Revision 3.11.10.20  1996/08/07  23:50:45  dino
 * CSCdi65425:  Make SPARSE_FLAG setting more consistent amoung rtrs in a
 *              PIM domain
 * Branch: California_branch
 *
 * Revision 3.11.10.19  1996/08/05  19:20:29  dino
 * CSCdi65068:  midb->mode is no longer needed. Configured mode need not
 *              ever change
 * Branch: California_branch
 *
 * Revision 3.11.10.18  1996/08/03  01:20:37  lwei
 * CSCdi60059:  rp entries are not timed out in the gdb
 * Branch: California_branch
 *
 * Revision 3.11.10.17  1996/06/26  22:16:45  chwhite
 * CSCdi61203:  registration packets should update (S,G) expire time in RP
 * Branch: California_branch
 *
 * Revision 3.11.10.16  1996/06/25  23:49:25  chwhite
 * CSCdi61203:  registration packets should update (S,G) expire time in RP
 * Branch: California_branch
 *
 * Revision 3.11.10.15  1996/06/25  06:04:57  lwei
 * CSCdi61125:  Fast RP-reachable message can be more concise
 * Branch: California_branch
 *
 * Revision 3.11.10.14  1996/06/24  23:33:50  chwhite
 * CSCdi58419:  <no ip pim rp> should clear the mr table
 * Branch: California_branch
 *
 * Revision 3.11.10.13  1996/06/20  01:13:02  lwei
 * CSCdi60852:  Back out fixes to CSCdi58281
 * Branch: California_branch
 *
 * Revision 3.11.10.12  1996/06/18  01:46:54  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.11.10.11  1996/06/11  02:50:26  lwei
 * CSCdi59607:  RP-reachability messages need to be rate-limited
 * Branch: California_branch
 *
 * Revision 3.11.10.10  1996/05/28  20:33:23  jzwiebel
 * CSCdi58281:  Offset timers keep S,G state alive in RP and neighbor
 * routers
 * Branch: California_branch
 *
 * Revision 3.11.10.9  1996/05/08  04:29:47  lwei
 * CSCdi54559:  IP multicast is sending larger than MTU sized packets over
 * GRE
 * Branch: California_branch
 *
 * Revision 3.11.10.8  1996/05/06  17:07:26  lwei
 * CSCdi56430:  sh ip pim rp returns bogus info if the RP isnt found
 * Branch: California_branch
 *
 * Revision 3.11.10.7  1996/05/04  01:08:50  wilber
 * Branch: California_branch
 *
 * Constrained Multicast Flooding
 * - Relocate the defintions for IGMP versions.
 *
 * Name Access List
 *
 * Revision 3.11.10.6  1996/04/24  19:06:40  dino
 * CSCdi55594:  Code review comment to CSCdi55387
 * Branch: California_branch
 *
 * Revision 3.11.10.5  1996/04/24  05:28:13  lwei
 * CSCdi54097:  A DR should send registers for pure senders
 * Branch: California_branch
 *
 * Revision 3.11.10.4  1996/04/23  15:25:23  dino
 * CSCdi55387:  PIM decapsulated Registers are not going to members when
 *              no PIM nbrs
 * Branch: California_branch
 *
 * Revision 3.11.10.3  1996/04/03  18:26:09  dino
 * CSCdi51596:  Dont process RP-reachability messages from a non-RP router
 * Branch: California_branch
 *
 * Revision 3.11.10.2  1996/03/29  00:45:28  lwei
 * CSCdi51179:  Single packet loss on shared-tree causes no source-tree to
 * exist
 * Branch: California_branch
 *
 * Revision 3.11.10.1  1996/03/18  20:19:03  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.9.4.3  1996/03/16  06:55:26  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.9.4.2  1996/03/13  01:38:49  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.9.4.1  1996/03/07  09:48:15  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.11  1996/02/26  23:29:18  mleelani
 * CSCdi49906:  Spurious accesses on receiving NULL registers
 * Do not dereference data on receiving NULL registers.
 *
 * Revision 3.10  1996/02/22  09:03:15  dino
 * CSCdi49566:  Allow control over which RPs are used in transit routers.
 *
 * Revision 3.9  1996/01/13  02:53:06  dino
 * CSCdi46656:  (*,G) oif null but (S,G) oif non-null causes Join
 *              suppression
 *
 * Revision 3.8  1995/12/17  19:24:24  dino
 * CSCdi44742:  PIM should only switch to the SPT when traffic threshold
 *              is reached
 *
 * Revision 3.7  1995/12/07  04:42:52  dino
 * CSCdi45175:  PIM, IGMP, and DVMRP should use TOS_PREC_INET_CONTROL
 *
 * Revision 3.6  1995/12/01  18:38:46  lwei
 * CSCdi44737:  IP multicast does not support administratively scoped
 * boundaries
 *
 * Revision 3.5  1995/11/30  04:42:13  mleelani
 * CSCdi44573:  IGMP doesnt process leave messages
 * o Add support for leave messages.
 * o Create idb sub-block for IGMP; use managed timers for IGMP timers.
 *
 * Revision 3.4  1995/11/21  23:12:11  dino
 * CSCdi43427:  cisco does not process DVMRP prunes.
 *
 * Revision 3.3  1995/11/17  17:30:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:42:19  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:01:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/24  07:33:47  hampton
 * Transition IP Multicast support to use the passive timer macros for all
 * its timers.  It no longer references the system clock directly.
 * [CSCdi37539]
 *
 * Revision 2.2  1995/07/01 07:46:16  dino
 * CSCdi27921:  Border router needs to send Registers to RP. ISPs want PIM
 *              over DVMRP unicast routing.
 *
 * Revision 2.1  1995/06/07  21:04:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include <ciscolib.h>
#include "name.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "../ipmulticast/msg_pim.c"	/* Not a typo, see logger.h */
#include "config.h"
#include "access.h"
#include "../if/network.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "../ip/ipoptions.h"
#include "../iprouting/route.h"
#include "../ip/ipaccess.h"
#include "../h/access.h"
#include "../ip/ip_registry.h"
#include "../ipmulticast/ipmulticast_debug.h"
#include "../ipmulticast/igmp.h"
#include "../ipmulticast/mroute.h"
#include "../ipmulticast/pim.h"

/*
 * Storage.
 */
queuetype pim_configured_rp;  /* Configured RPs */
ulong pim_message_interval;   /* Configured interval */
boolean pim_has_new_rp; /* TRUE if a new RP is learned */


/*
 * pim_rp_command
 *
 * Process "ip pim rp-address <rp-address> [<group-acl>]
 */
void pim_rp_command (parseinfo *csb)
{

    pim_rptype *rp;
    ipaddrtype rp_address;
    acl_headertype *acl;
    pim_rp_mapping *prm;
    boolean    rp_ann_deleted, rp_disc_deleted;
   
    rp_ann_deleted = FALSE;
    rp_disc_deleted = FALSE;
    rp_address = GETOBJ(paddr,1)->ip_addr;
    if (GETOBJ(int,1) || *GETOBJ(string,1)) {
	acl = access_parse_acl(GETOBJ(int,1), GETOBJ(string,1),
			       GETOBJ(int,1) == 0, ACL_IP_NAMED_SIMPLE);
	if (!acl)
	    return;
    } else
	acl = NULL;
    
    /*
     * Don't accept a bogus IP address for the RP (including group addresses).
     * martian() checks for group addresses.
     */
    if (martian(rp_address, TRUE)) {
	printf("\nIllegal RP address");
	return;
    }

    /*
     * Find entry based on IP address and access-list number.
     */
    for (rp = (pim_rptype *) pim_configured_rp.qhead; rp; rp = rp->next) {
	if (rp_address == rp->rp_address) break;
    }

    if (csb->sense) {
	if (!rp) {
	    rp = malloc(sizeof(pim_rptype));
	    if (!rp) return;
	    rp->rp_address = rp_address;
	    enqueue(&pim_configured_rp, rp);
	}
	rp->group_acl = acl;
	rp->state = RP_UP;
	rp->override = GETOBJ(int,2);
	rp->prm = NULL;
	TIMER_STOP(rp->expires);
	rp->configured = TRUE;
	igmp_check_new_rp(rp_address, 0);
	pim_has_new_rp = TRUE;

	/*
	 * Need to establish the semi-static
	 * CISCO-RP-ANNOUNCE and CISCO-RP-DISCOVERY entries
	 * so that we don't get into a catch 22 situation with RP mappings
	 * learned through Auto-RP.
	 * These two entries don't expire.
	 */
	if (nfast_check(acl, CISCO_RP_ANNOUNCE)) {
	    pim_add_prm(CISCO_RP_ANNOUNCE, MASK_HOST, rp_address, 0, 0, 0, 0);
	}
	if (nfast_check(acl, CISCO_RP_DISCOVERY)) {
	    pim_add_prm(CISCO_RP_DISCOVERY, MASK_HOST, rp_address, 0, 0, 0, 0);
	}
    } else {
	if (!rp) return;
	ip_clear_mroute_cache(rp_address, NULL);
	unqueue(&pim_configured_rp, rp);
	rp->next = NULL;

	/*
	 * Need to remove the semi-static entry, if this RP also serves
	 * CISCO-RP-ANNOUNCE and CISCO-RP-DISCOVERY.
	 */
	if (nfast_check(rp->group_acl, CISCO_RP_ANNOUNCE)) {
	    prm = pim_get_prm(CISCO_RP_ANNOUNCE, MASK_HOST);
	    if (prm && prm->rp_addr == rp->rp_address) {
		pim_delete_prm(prm);
		rp_ann_deleted = TRUE;
	    }
	}
	if (nfast_check(rp->group_acl, CISCO_RP_DISCOVERY)) {
	    prm = pim_get_prm(CISCO_RP_DISCOVERY, MASK_HOST);
	    if (prm && prm->rp_addr == rp->rp_address) {
		pim_delete_prm(prm);
		rp_disc_deleted = TRUE;
	    }
	}

	free(rp);

	/*
	 * If the semi-static entry was deleted due to a "no ip pim rp-add"
	 * command, need to search the remaining configured RP queue and
	 * see if there is another RP covering these two addresses.
	 */
	if (rp_ann_deleted || rp_disc_deleted) {
	    pim_autorp_reset_known_groups(rp_ann_deleted, rp_disc_deleted);
	}
    }
}

/*
 * pim_display_mapping_entry
 */
static int pim_display_mapping_entry (rntype *node, va_list args)
{

    pim_rp_mapping *prm;
    nametype       *namep;
    ipaddrtype     group, prefix, mask;
    uchar          uptime[20], expires[20];

    group = va_arg(args, ipaddrtype);
    prm = (pim_rp_mapping *) node;
    
    prefix = ip_radix2addr(prm->group_prefix);
    mask = ip_radix2addr(prm->group_mask);

    /*
     * Display only entry that matches specified group.
     */
    if (group) {
	if ((group & mask) != prefix) return(0);
    } else {
	group = prefix;
    }

    sprint_ticks_in_dhms(uptime, ELAPSED_TIME(prm->uptime));
    if (!TIMER_RUNNING_AND_AWAKE(prm->expires)) {
	if (TIMER_RUNNING(prm->expires)) {
	    sprint_ticks_in_dhms(expires, TIME_LEFT_SLEEPING(prm->expires));
	} else {
	    sprintf(expires, "never");
	}
    } else {
	sprintf(expires, "now");
    }

    printf("\nGroup(s) %i/%d, uptime: %s, expires: %s", group, 
	   ip_bitsinmask(mask), uptime, expires);
    printf("\n    RP %i ", prm->rp_addr);
    namep = reg_invoke_ip_address_lookup(prm->rp_addr);
    printf("(%s)", (namep) ? namep->name : "?");

    if (prm->rp_version) {
        printf(", PIM%s%s", PIM_RP_IS_V2(prm->rp_version) ? "v2 " : "",
	       PIM_RP_IS_V1(prm->rp_version) ? "v1" : "");
    }

    printf("\n    Info source:");
    if (prm->info_source) {
	printf(" %i ", prm->info_source);
	namep = reg_invoke_ip_address_lookup(prm->info_source);
	printf("(%s)", (namep) ? namep->name : "?");
    } else {
	printf(" local ");
    }

    return(0);
}

/*
 * pim_show_rp_mapping
 */
static void pim_show_rp_mapping (ipaddrtype group, boolean in_use)
{
    
    pim_rptype *rp;
    nametype   *namep;
    ipaddrtype prefix, mask;

    automore_enable(NULL);
    printf("PIM Group-to-RP Mappings");
    if (pim_announced_rp.count) printf("\nThis system is an RP");
    if (rp_agent_socket) printf("\nThis system is an RP-mapping agent");
    printf("\n");

    /*
     * Show dynamic ones first.
     */
    if (pim_autorp_cache) {
	rn_walktree_blocking(pim_autorp_cache->rnh_treetop, 
			 pim_display_mapping_entry, group);
    }

    /*
     * Show static ones next.
     */
    for (rp = (pim_rptype *) pim_configured_rp.qhead; rp; rp = rp->next) {
	if (!rp->group_acl) {
	    printf("\nGroup(s): 224.0.0.0/4, ");
	} else {
	    printf("\nAcl: %s, ", rp->group_acl->name);
	}
	printf("%s", (rp->override) ? "Static-Override" : "Static");

	printf("\n    RP: %i", rp->rp_address);
	namep = reg_invoke_ip_address_lookup(rp->rp_address);
	printf(" (%s)", (namep) ? namep->name : "?");
    }

    /*
     * Show the learned RP queue if "in-use" is specified on the
     * command line
     */
    if (in_use) {
	printf("\n\nRPs in Auto-RP cache that are in use:");
	for (rp = (pim_rptype *) pim_learned_rp.qhead; rp; rp = rp->next) {
	     if (rp->prm) {
		prefix = ip_radix2addr(rp->prm->group_prefix);
		mask = ip_radix2addr(rp->prm->group_mask);
	     } else {
		prefix = 0;
		mask = 0;
	     }
	     printf("\nGroup(s): %i/%d,   RP: %i", prefix, ip_bitsinmask(mask),
		    rp->rp_address);
	}
    }

    automore_disable();
}

/*
 * pim_show_rp
 *
 * Process "show ip pim rp" command and variants.
 */
void pim_show_rp (parseinfo *csb)
{

    gdbtype    *gdb;
    ipaddrtype group;
    int        i;
    boolean    in_use;

    group = GETOBJ(paddr,1)->ip_addr;

    /*
     * Process 'show ip pim rp mapping" command.
     */
    if (GETOBJ(int,1)) {
	in_use = GETOBJ(int,2);
	pim_show_rp_mapping(group, in_use);
	return;
    }

    /*
     * Display for a single group.	
     */
    if (group) {
	gdb = ip_get_gdb(group);
	if (!gdb) {
	    printf("\nGroup %i not found", group);
	    return;
	}
	if (!IS_SPARSE_GROUP(gdb)) {
	    printf("\n%i is not a sparse-mode group", gdb->mdb.group);
	    return;
	}
	pim_display_rp(gdb);
	return;
    }

    /*
     * Display for all known sparse-mode groups.
     */
    automore_enable(NULL);
    for (i = 0; i < NETHASHLEN; i++) {
	for (gdb = ip_mroute_cache[i]; gdb; gdb = gdb->next) {
	    if (!IS_SPARSE_GROUP(gdb)) continue;
	    pim_display_rp(gdb);
	    if (automore_quit()) break;
	}
	if (automore_quit()) break;
    }
    automore_disable();
}

/* 
 * pim_display_rp
 */
void pim_display_rp (gdbtype *gdb)
{

    mdb_rptype *rp;
    pim_rptype *global_rp;
    uchar      time[20];
    uchar      uptime[20];

    mem_lock(gdb);
    printf("\nGroup: %i", gdb->mdb.group);

    if (gdb->rp.address) {
	rp = &gdb->rp;
	printf(", RP: %i", rp->address);

	global_rp = pim_find_rp(gdb->mdb.group, rp->address);
	if (global_rp) {
	    if (PIM_RP_IS_V2(global_rp->version)) printf(", v2");
	    if (PIM_RP_IS_V1(global_rp->version)) printf(", v1");
	}

	if (ip_ouraddress(rp->address)) {
	    printf(", next RP-reachable in %TF", gdb->send_rpreach);
	} else {
	    sprint_ticks_in_dhms(uptime, ELAPSED_TIME(rp->uptime));
	    printf(", uptime %s", uptime);
	    if (!TIMER_RUNNING(rp->expires)) {
		sprintf(time, "never");
	    } else if (!AWAKE(rp->expires)) {
		sprint_ticks_in_dhms(time, TIME_LEFT_SLEEPING(rp->expires));
	    } else {
		sprintf(time, "now");
	    }
	    printf(", expires %s", time);
	}
    }
    free(gdb);
}

/*
 * pim_find_learned_rp
 *
 * Search the pim_learned_rp queue, and see if an RP exists for the group.
 * If not search the Auto_RP radix database, and see if there is a newly
 * discovered RP for this group.
 * In the Auto_RP database, only one RP is stored for each Group address.
 */
static pim_rptype *pim_find_learned_rp (ipaddrtype group, ipaddrtype rp_addr)
{
    pim_rptype *rp, *learned_rp;
    pim_rp_mapping *prm;
    boolean new;
    int count;

    /*
     * Search the Auto-RP radix tree, any update of the RP mapping
     * information will be reflected in the learned rp queue, by the
     * end of this function.
     */
    prm = NULL;
    if (group) {
	prm = pim_get_best_rp_mapping(group);
	if (!prm) return(NULL);
    }

    learned_rp = NULL;
    count = 0;

    /*
     * Search the learned RP queue.
     */
    for (rp = (pim_rptype *) pim_learned_rp.qhead; rp; rp = rp->next) {
	count++;
	if (group) {

	    /*
	     * If group is specified, only match on "prm"
	     */
	    if (prm == rp->prm) {
		learned_rp = rp;
		break;
	    }
	} else if (!rp_addr || (rp_addr == rp->rp_address)) {
	    learned_rp = rp;
	    break;
	}
    }

    if (!group) return(learned_rp);

    /*
     * Issue log message, if there are more than 40 items in the
     * learned RP queue. Could possibly be loading the CPU up.
     */
    if (count > 40) {
	errmsg(&msgsym(MANY_RPS, AUTORP), count);
    }

    if (prm) {
	new = FALSE;

	/*
	 * For group CISCO-RP-ANNOUNCE and CISCO-RP-DISCOVERY, should
	 * only use the prm if it is a semi-static entry.
	 */
	if (group == CISCO_RP_ANNOUNCE || group == CISCO_RP_DISCOVERY) {
	    if (TIMER_RUNNING(prm->expires)) return(NULL);
	}

	if (!learned_rp) {

	   /*
	    * Create a new element in the learned RP queue
	    */
	   learned_rp = malloc(sizeof(pim_rptype));
           if (!learned_rp) {
               PIM_BUGINF(group, ("\nPIM: No memory for learned RP"));
               return(NULL);
           }
	   learned_rp->configured = FALSE;
	   enqueue(&pim_learned_rp, learned_rp);
	   new = TRUE;
	}
	if (new || learned_rp->rp_address != prm->rp_addr) {
	    learned_rp->state = RP_UP;
            learned_rp->prm = prm;
            learned_rp->rp_address = prm->rp_addr;
	    TIMER_STOP(learned_rp->expires);
	}
	learned_rp->version = prm->rp_version;
    }
    if (!learned_rp || (rp_addr && rp_addr != learned_rp->rp_address)) {
	return(NULL);
    }
    return(learned_rp);
}

/*
 * pim_get_all_rps
 *
 * Check to see if an RP is known for the group. If there is return TRUE.
 * Store all known RPs in gdb.
 *
 * If the boolean start is TRUE, the timers for the newly added RPs will
 * also be started.
 */
boolean pim_get_all_rps (gdbtype *gdb, boolean start_timer,
		         boolean trigger_join)
{
    pim_rptype *rp, *learned_rp;
    boolean    static_found;

    if (gdb->rp.address) return(TRUE);

    static_found = FALSE;

    /*
     * Try learned RP list first.
     */
    learned_rp = pim_find_learned_rp(gdb->mdb.group, 0);

    /*
     * Only use configured if override is in effect.
     */
    for (rp = (pim_rptype *) pim_configured_rp.qhead; rp; rp = rp->next) {
	if (!nfast_check(rp->group_acl, gdb->mdb.group)) continue;
	if (!rp->send_null_reg && start_timer) {
	    rp->state = RP_UP;
	    TIMER_START(rp->send_register, PIM_NULL_REG_INTERVAL);
	}

	/*
	 * If we have a learned RP and a configured RP conflict. Use the
	 * configured RP only when the "override" keyword is used on the
	 * "ip pim rp-address" command.
	 */
	if (learned_rp && !rp->override) continue;

	pim_store_rp_info(gdb, rp->rp_address, rp->state, trigger_join, FALSE);
	static_found = TRUE;
    }

    /*
     * The dynamicly learned RP overrides the staticly configured
     * one by default.
     */
    if (learned_rp && !static_found) {
	pim_store_rp_info(gdb, learned_rp->rp_address, learned_rp->state,
			  trigger_join, TRUE);
	static_found = TRUE;
    }

    return(static_found);
}

/*
 * pim_find_configured_rp
 * Look up RP address for specified group, in the configured RP queue.
 */
static pim_rptype *pim_find_configured_rp (ipaddrtype group, ipaddrtype rp_address)
{
    pim_rptype *rp, *best_rp;

    best_rp = NULL;
    for (rp = (pim_rptype *) pim_configured_rp.qhead; rp; rp = rp->next) {
	if (group && !nfast_check(rp->group_acl, group)) continue;
	if (rp_address && rp_address == rp->rp_address) {
	    best_rp = rp;
	    break;
	}
	if (!best_rp || rp->rp_address > best_rp->rp_address) best_rp = rp;
    }
    return(best_rp);
}

/*
 * pim_find_rp
 *
 * Look up RP address for specified group and return data structure that
 * matches supplied RP address and passes any accept-rp filters.
 *
 * There may be more than one RP that serves the group.  A dynamically
 * learned RP takes priority over a statically configured RP unless the
 * latter is configured to override.  Also note, the same RP may serve more
 * than one group.
 */
pim_rptype *pim_find_rp (ipaddrtype group, ipaddrtype rp_address)
{
    pim_rptype *best_rp, *learned_rp;

    best_rp = pim_find_configured_rp(group, rp_address);

    /*
     * If there is no configured RP or if it's not configured to override
     * a dynamically learned RP, look up the dynamically learned RP.
     */
    if (!best_rp || !best_rp->override) {
	learned_rp = pim_find_learned_rp(group, rp_address);
	if (learned_rp) {
	    best_rp = learned_rp;
	}
    }

    if (!best_rp) {
	return(NULL);
    }
    
    return(pim_accept_rp(best_rp->rp_address, group) ? best_rp : NULL);
}

/*
 * pim_delete_one_sparse_dense_olist
 *
 * Process a single multicast route entry. Delete
 * all sparse-dense configured interfaces in the olist.
 */
static int pim_delete_one_sparse_dense_olist (rntype *node, va_list args)
{
    mdbtype *mdb;
    midbtype *midb, *next_midb;

    mdb = (mdbtype *) node;

    for (midb = (midbtype *) mdb->if_outputQ.qhead; midb; ) {
	next_midb = midb->next;
	if (midb->idb->pim_mode == PIM_SPARSE_DENSE_MODE &&
	    !igmp_find_group(mdb->group, midb->idb) &&
	    !DVMRP_ACTIVE(midb->idb)) {
		ip_delete_mroute_idb(mdb, midb->idb, midb->next_hop);
	}
	midb = next_midb;
    }
    return(0);
}

/*
 * pim_delete_sparse_dense_oif
 *
 * Go though (*,G) and all (S,G) oiflists, and delete * all sparse-dense
 * configured interfaces.
 * This function is called when a new RP is learned for gdb, through
 * one of two ways:
 *
 *  1) a (*,G) join or register is received. If the RP is accepted,
 *     it only applies to one group. This function is called from
 *     pim_store_rp_info();
 *
 *  2) when a new rp is configured, or learned in the rp-mapping cache,
 *     pim_has_new_rp is set to TRUE. Inside ip_mroute_ager(), it will
 *     call this function for all the relevant groups.
 */
void pim_delete_sparse_dense_oif (gdbtype *gdb)
{
    pim_delete_one_sparse_dense_olist((rntype *) &gdb->mdb, (va_list) NULL);
    rn_walktree(gdb->radix_head->rnh_treetop,
		pim_delete_one_sparse_dense_olist, NULL);
}

/*
 * pim_store_rp_info
 *
 * Stores RP information in the gdb. Such as RP address and expiration time,
 * RPF interface and neighbor towards the direction of the RP.
 *
 * If the number of RPs known for this group has reached the maximum amount
 * (cisco has a limit), then the lowest addressed RP is thrown out.
 *
 * NOTE: All the PIM code assumes that gdb->rp is the active RP. The
 * active RP is the one that we send perioid Joins to.
 */
void pim_store_rp_info (gdbtype *gdb, ipaddrtype rp_address, ulong state,
			boolean trigger_join, boolean start_expires_timer)
{

    mdb_rptype *rp;
    boolean    ourself;

    /*
     * Check if we are the RP.
     */
    ourself = ip_ouraddress(rp_address);
    rp = &gdb->rp;

    /*
     * Store the supplied RP in the gdb, if the gdb didn't have an RP
     * before, or if the old one has a lower IP address than the current
     * one.
     *
     * When different RPs are used for the same group, it is mostly
     * due to misconfigurations of RP addresses, or the use of obselete
     * software. We use a consistent arbitration criteria, to make
     * the outcome as predictable as possible.
     */
    if (!rp->address || rp->address < rp_address) {

	/*
	 * If this group was not already in sparse mode, make it so.
	 * Also delete all sparse-dense interfaces in (*,G) and (S,G)'s.
	 */
	if (!(gdb->mdb.flags & SPARSE_FLAG)) {
	    gdb->mdb.flags |= SPARSE_FLAG;
	    pim_delete_sparse_dense_oif(gdb);
	}

	/*
	 * Store RP address, expiration time, and supplied state. Start 
	 * RP-reachable message timer if we are RP. Start Join message timer 
	 * if someone else is the RP.
	 */
	rp->address = rp_address;
	TIMER_STOP(rp->expires);
	GET_TIMESTAMP(rp->uptime);
	if (!TIMER_RUNNING(gdb->mdb.send_join)) {
	    TIMER_START_JITTERED(gdb->mdb.send_join, PIM_MESSAGE_INTERVAL,
				 PIM_JITTER_PCT);
	}
	if (ourself) {
	    if (!TIMER_RUNNING(gdb->send_rpreach)) {
		TIMER_START(gdb->send_rpreach, PIM_RP_REACH_INTERVAL);
	    }
	} else {
	    if (start_expires_timer) {
		TIMER_START(rp->expires, PIM_RP_EXPIRES);
	    }
	    TIMER_STOP(gdb->send_rpreach);
	}
	PIM_BUGINF(gdb->mdb.group,
		   ("\nPIM: Check RP %i into the (*, %i) entry",
		     rp_address, gdb->mdb.group));
    } else {

	/*
	 * This is not a new RP. If there is an RPF interface, don't do
	 * lookup.
	 */
	if (gdb->mdb.if_input) return;
    }

    /*
     * There is no RPF information if we are the active RP.
     */
    if (ourself) {
	gdb->mdb.if_input = NULL;
	gdb->mdb.rpf_nbr = 0;
	return;
    }

    /*
     * RP has changed, send triggered Join with WC-bit set and RP-bit set.
     * Only do so, if there is an RPF neighbor to RP, and either our olist
     * is not NULL or we are the designated router for the incoming
     * interface and no membership reports were received on that interface.
     */
    if (pim_rp_rpf_lookup(gdb, rp->address) && trigger_join) {
        if (!MDB_OUTPUTQ_NULL(&gdb->mdb) ||
            (gdb->mdb.if_input && gdb->mdb.if_input->ip_multicast_dr &&
             (gdb->mdb.flags & CONNECTED_FLAG))){
	     pim_send_join_prune(&gdb->mdb, gdb->mdb.if_input,
                                 gdb->mdb.rpf_nbr, TRUE, TRUE, TRUE, TRUE);
        }
    }
}

/*
 * pim_rp_rpf_lookup
 *
 * Store RPF parameters in (*,G). Returns TRUE if there is a route to the RP.
 */
boolean pim_rp_rpf_lookup (gdbtype *gdb, ipaddrtype rp_address)
{

    idbtype    *idb;
    ipaddrtype source, mask, rpf_nbr;

    /*
     * Make the source information in the (*,G) a host route using the RP 
     * address from the source in the Join packet.
     */
    ip_addr2radix(rp_address, gdb->mdb.source);
    ip_addr2radix(MASK_HOST, gdb->mdb.mask);
    rpf_nbr = 0;
    idb = ip_rpf_lookup(rp_address, MASK_HOST, &source, &mask, &rpf_nbr, 
			&gdb->mdb.rpf_type, ROUTINGDEPTH);
    gdb->mdb.if_input = idb;
    if (!idb) {
	gdb->mdb.rpf_nbr = 0;
	return(FALSE);
    } else {

	/*
	 * If RP is directly connected, store RP address as RPF neighbor.
	 */
	if (samecable(rp_address) == idb) rpf_nbr = rp_address;
    }

    gdb->mdb.rpf_nbr = rpf_nbr;

    /*
     * Check to see if incoming interface is in outgoing list. If so,
     * delete it.
     */
    ip_delete_mroute_idb(&gdb->mdb, idb, rpf_nbr);

   /*
    * If the incoming interface is an administrative boundary,
    * remove all interfaces from the olist.
    */
   if (ipmulticast_test_boundary(idb, gdb->mdb.group)) {
        ip_mfree_outputQ(&gdb->mdb.if_outputQ);
   }

    return(TRUE);
}

/*
 * pim_clear_rp_state
 *
 * Clear all dependancies on the group RP from this gdb.
 */
void pim_clear_rp_state (gdbtype *gdb)
{
    gdb->rp.address = 0;
    gdb->mdb.rpf_nbr = 0;
    gdb->mdb.if_input = NULL;
}

/*
 * pim_age_rp
 *
 * This function determines if an RP has gone down. By no receipt of
 * RP-reachable messages, the RP has timed out. Change state to DOWN and
 * try another RP if one is available. Start a timer to retry this RP later
 * (have to do this so the highest IP addressed RP is always used).
 */
void pim_age_rp (gdbtype *gdb)
{

    mdb_rptype *group_rp;
    pim_rptype *rp;
    pim_rp_mapping *prm;
    boolean    reg_flag_set;

    if (!gdb->rp.address) return;

    reg_flag_set = (gdb->mdb.flags & SET_REG_FLAG);

    /*
     * Go through each RP known to group.
     */
    group_rp = &gdb->rp;

    /*
     * Get global RP data structure. If there isn't one, we are a transit
     * router and have learned who the RP is. This case we are a
     * downstream router sending joins to 1 RP.
     */
    rp = pim_find_rp(gdb->mdb.group, 0);

    if (rp) {

	/*
	 * If an RP is found but is different from the group_rp, it means
	 * a change in RP configuration has occured. We need to zero the
	 * RP-related state.  If we are a DR, we'll eventually replace the RP
	 * with the new RP.
	 */
	if (rp->rp_address != group_rp->address) {
	    pim_clear_rp_state(gdb);
	    return;
	}

	/*
	 * Set flag to continue to send Null Registers if we have
	 * sources to Register for.  If flag wasn't set, this is the
	 * first group, set up initial state.
	 */
	if (reg_flag_set) {
	    rp->send_null_reg = TRUE;
	}

	/*
	 * Only age learned RPs.  Configured RPs never go down.
	 */
	if (TIMER_RUNNING(rp->expires)) {

	    boolean sync_timers = TRUE;
	    
	    if (AWAKE(rp->expires)) {
		switch (rp->state) {

		case RP_DOWN:

		    /*
		     * Time to bring back the downed global RP.
		     */
		    TIMER_START(rp->expires, PIM_RP_EXPIRES);
		    rp->state = RP_UP;
		    PIM_BUGINF(0, ("\nPIM: bring RP %i back up",
				   rp->rp_address));
		    break;

		case RP_UP:

		    /*
		     * Global RP went down.  If it went down because we
		     * made the wrong guess of the RP's version, downgrade
		     * to version 1 now.
		     */
		    if (PIM_RP_IS_V2(rp->version)) {
			TIMER_START(rp->expires, PIM_RP_EXPIRES);
			rp->version = PIMV1;
		    } else {
			TIMER_START(rp->expires, PIM_BRING_UP_RP);
			rp->state = RP_DOWN;
			sync_timers = FALSE;
			PIM_BUGINF(0, ("\nPIM: RP %i went down",
				       rp->rp_address));
		    }
		    break;

		}
	    }

	    /*
	     * Sync group RP expiration timer with global RP expiration 
	     * timer.
	     */
	    if (sync_timers) {
		COPY_TIMESTAMP(rp->expires, group_rp->expires);
	    }
	}
    }
    
    /*
     * Age group RP expiration timer. In some cases, when the global RP
     * timer expires, it forces the group RP timer to expire.
     */
    if (TIMER_RUNNING_AND_AWAKE(group_rp->expires)) {

	/*
	 * If RP is learned via auto-rp and there is a auto-rp cache entry
	 * for the RP for this group, copy the auto-rp's caches expiration
	 * timer to the group specific RP expiration timer. If it doesn't
	 * exist, we can update the group specific timer solely by receipt
	 * of RP-reachable messages.
	 */
	if (rp && pim_find_learned_rp(gdb->mdb.group, rp->rp_address)) {
	    prm = pim_get_prm(gdb->mdb.group, 0);
	    if (prm) COPY_TIMESTAMP(prm->expires, group_rp->expires);
	    return;
	}
	if (!(rp && rp->send_null_reg && rp->state == RP_UP)) {

	    /*
	     * RP has timed out because no RP-reachables or Register-Stops
	     * received for group. The last hop router will try to
	     * bring up the RP again, after time-out.
	     */
	    PIM_BUGINF(gdb->mdb.group,
		       ("\nPIM: RP %i for group %i went down", 
		        group_rp->address, gdb->mdb.group));
	    /*
	     * Delete the group RP. It will be added back
	     * when the global RP is UP, or when we want to retry
	     * the same RP.
	     */
	    pim_clear_rp_state(gdb);

	    /*
	     * If global RP is truly gone, convert group to dense mode.
	     */
	    if (!rp) {
		gdb->mdb.flags &= ~SPARSE_FLAG;
	    }
	}
    }
}

/*
 * pim_send_rp_reach
 *
 * We have determined that we are the RP for this group. It is time to
 * send an RP reachability message down the RP-tree. This informs downstream
 * routers that the RP is still alive and active.
 */
void pim_send_rp_reach (gdbtype *gdb)
{

    paktype          *ip, *dup;
    igmptype         *igmp;
    pim_rpreach_type *pim;
    midbtype         *midb;
    ulong            size;
    ipaddrtype       rp_address, dest;

    /*
     * Get packet buffer.
     */
    size = IPHEADERBYTES(NOPT) + IGMPHEADERBYTES + sizeof(pim_rpreach_type);

    ip = getbuffer(size);
    if (!ip) return;

    /*
     * Build IGMP fixed part of header.
     */
    igmp = (igmptype *) (ipheadstart(ip) + IPHEADERBYTES(NOPT));
    igmp->type = IGMP_PIM_TYPE;
    igmp->code = PIM_RP_REACHABLE_CODE;
    igmp->checksum = 0;
    igmp->address = PIM_VERSION1;

    /*
     * Build PIM fixed part of header.
     */
    pim = (pim_rpreach_type *) igmp->data;
    pim->group = gdb->mdb.group;
    pim->group_mask = MASK_HOST;
    rp_address = gdb->rp.address;
    pim->rp_address = rp_address;
    pim->reserved = 0;
    pim->holdtime = PIM_RP_REACH_HOLDTIME / ONESEC;

    igmp->checksum = ipcrc((ushort *) igmp, size - IPHEADERBYTES(NOPT));

    /*
     * Send to IP for each outgoing interface in (*,G).
     */
    igmprouteparams.ttl = TTL_DEFAULT;
    for (midb = (midbtype *) gdb->mdb.if_outputQ.qhead; midb; 
	 midb = midb->next) {
	if (IS_DENSE_MODE(midb->idb, gdb)) continue;
	dup = pak_duplicate(ip);
	if (!dup) break;

	PIM_BUGINF(gdb->mdb.group, 
		   ("\nPIM: Send RP-reachability for %i on %s",
		   gdb->mdb.group, midb->idb->namestring));

 	dest = (midb->idb->pim_nbma_mode) ? midb->next_hop : ALLROUTERS;

	ipwrite(dup, size, IGMP_PROT, rp_address, dest, midb->idb,
		NULL, &igmprouteparams);
	igmp_traffic.output_pim++;
    }
    igmprouteparams.ttl = 1;
    retbuffer(ip);
}

/*
 * pim_receive_rp_reach
 *
 * Process received RP reachability message. Forward it out all outgoing
 * interfaces of the (*,G) entry.
 */
void pim_receive_rp_reach (paktype *pak, igmptype *igmp, ulong length)
{

    iphdrtype        *ip;
    paktype          *dup;
    gdbtype          *gdb;
    pim_rpreach_type *pim;
    midbtype         *midb;
    pim_rptype	     *rp;

    /*
     * Validate packet length.
     */
    if (length < sizeof(pim_rpreach_type)) {
	PIM_BUGINF(0, ("\nPIM: Packet too short"));
	return;
    }
    pim = (pim_rpreach_type *) igmp->data;

    PIM_BUGINF(pim->group, ("\n     for group %i", pim->group));

    /*
     * Look up group. Don't create state if group is not found. Joins do
     * that. We're processing (*,G) only.
     */
    gdb = ip_get_gdb(pim->group);
    if (!gdb) {
	PIM_BUGINF(pim->group, ("\nPIM: Group %i not found", pim->group));
	return;
    }

    /*
     * Don't propagate messages, if group is not treated in sparse mode.
     */
    if (!IS_SPARSE_GROUP(gdb)) {
	PIM_BUGINF(pim->group, ("\nPIM: %i is not sparse-mode group", 
				pim->group));
	return;
    }

    ip = iphdrtype_start(pak);

    /*
     * Do RPF check.
     */
    if (pak->if_input != gdb->mdb.if_input) {
	if (gdb->mdb.if_input ||
	    pak->if_input != ip_rpf_lookup(ip->srcadr, MASK_HOST, NULL, NULL,
					   NULL, NULL, ROUTINGDEPTH)) {
	    PIM_BUGINF(pim->group, ("\nPIM: Not RPF interface, group %i",
				    pim->group));
	    return;
	}
    }

    /*
     * Don't update RP expiration timer if the RP is on the
     * other side of the administratively scoped boundary.
     */
     if (ipmulticast_test_boundary(pak->if_input, pim->group)) {
         PIM_BUGINF(pim->group,	
                    ("\nPIM: RP reachable discarded at boundary"));
         return;
     }

    /*
     * When no RP is cached in the gdb, need to see if
     * the real RP has expired.
     */
    if (!gdb->rp.address) {
	pim_get_all_rps(gdb, FALSE, FALSE);
    }

    /*
     * Update group RP expiration timer so RP stays alive.
     */
    if (gdb->rp.address && gdb->rp.address == ip->srcadr) {

	/*
	 * If we have just received the same RP-reachable in less
	 * than holdtime/8 ago, throw away this one.
	 * Actually holdtime/4 is good enough as long as the holdtime
	 * is 3 times the RP reachable interval. But we want it to
	 * leave a safety margin for pathetic network conditions,
	 * and choose pim->holdtime/8.
	 * We use the following in-equality to check this condition:
	 *
	 *    holdtime - time_left_sleeping < holdtime/8
	 *
	 * which resolves to:
	 *
	 *    time_left_sleeping > holdtime * 7/8
	 */
	 if (TIME_LEFT_SLEEPING(gdb->rp.expires) > 
	     ((pim->holdtime * ONESEC * 7) >> 3)) {
	     PIM_BUGINF(pim->group,
			("\nPIM:   Duplicate RP-reachable from %i for %i",
			  gdb->rp.address, gdb->mdb.group));
	     return;
	 }
	 if (TIMER_RUNNING(gdb->rp.expires)) {
	     TIMER_START(gdb->rp.expires, pim->holdtime * ONESEC);
	     PIM_BUGINF(pim->group,
			("\nPIM: Update RP expiration timer (%d sec) for %i",
			 pim->holdtime, pim->group));
	 }

	 /*
	  * Also need to update the global RP expires timer.
	  * If the global RP went down, bring it up now.
	  */
	 rp = pim_find_rp(pim->group, ip->srcadr);
	 if (rp && !rp->configured) {
	     COPY_TIMESTAMP(gdb->rp.expires, rp->expires);
	     if (rp->state == RP_DOWN) rp->state = RP_UP;
	 }
    } else {

	/* 
	 * Couldn't find this RP in our cached list of RPs.
	 * Or, if RP-reachability message is from an RP we're not
	 * expecting, don't forward it.
	 */
	PIM_BUGINF(pim->group, ("\nPIM: RP not known, or mismatch"));
	return;
    } 

    /*
     * Send on each outgoing interface in (*,G). Calling function retbuffers 
     * packet.
     */
    for (midb = (midbtype *) gdb->mdb.if_outputQ.qhead; midb; 
	 midb = midb->next) {
	if (IS_DENSE_MODE(midb->idb, gdb)) continue;
	if (!midb->idb->pim_nbrs) continue;
	dup = pak_duplicate(pak);
	if (!dup) break;

	PIM_BUGINF(gdb->mdb.group, 
		   ("\nPIM: Forward RP-reachability packet for %i on %s",
		    gdb->mdb.group, midb->idb->namestring));

	dup->if_output = midb->idb;
	dup->desthost = midb->next_hop;
	ipsendnet(dup, TRUE, TRUE);
	igmp_traffic.output_pim++;
    }
}

/*
 * pim_send_periodic
 *
 * Transmit periodic Join/Prune messages for a given group. The RP-tree gets
 * a message with the (*,G) in the join-list, and all sources that don't have
 * the same RPF interface as the RP-tree, in the prune-list. The sources that
 * are reachable through the same interface as the RP are put in the join-list.
 *
 * Then each neighbor that existing sources are reachable through, are sent
 * a message with those sources in the join-list part of the message.
 *
 * The code here within is a superset of the Graft code (pim_send_graft())
 * which was originally written. The difference is that Grafts are postively
 * acknowledged, sparse-mode joins/prunes are not (since they are periodic).
 */
void pim_send_periodic (gdbtype *gdb)
{
    paktype *join_pak, *prune_pak, *ip, *pak;
    boolean we_are_rp;

    PIM_BUGINF(gdb->mdb.group, ("\nPIM: Building Join/Prune message for %i", 
				gdb->mdb.group));

    /*
     * Build message destined for RP if there is one and it is up.
     */
    join_pak = NULL;
    if (!pim_init_packet_buffer(gdb, &join_pak, &prune_pak, TRUE)) return;

    /*
     * Are we the RP?
     */
    we_are_rp = ip_ouraddress(gdb->rp.address);

    /*
     * Build packet that is destined for RP. At the same time build packets
     * destined for sources through other RPF neighbors. This allows us
     * to scan the radix tree once.
     */
    ip = NULL;
    rn_walktree(gdb->radix_head->rnh_treetop, pim_build_entry, gdb, 
		&join_pak, &prune_pak, &ip, we_are_rp);

    /*
     * Send packet to RP if buffer exists.
     */
    if (join_pak) pim_send_rp_packet(gdb, join_pak, prune_pak);

    while (ip) {
	pak = ip;
	ip = (paktype *) ip->dataptr;
	pim_send_packet(pak, gdb->mdb.group, FALSE, FALSE);
    }
}

/* 
 * pim_send_batch_join
 *
 * Send a join message to every source the rp has state for in order
 * to get data flow if we have transitioned from a null-oif to forwarding
 * state. This is called by pim_add_mroute in the case where a (*,G) join
 * comes in for a group with sources but no previously active member.
 *
 */
void pim_send_batch_join (gdbtype *gdb)
{

    paktype *ip, *pak, *join_pak;
    boolean we_are_rp;

    PIM_BUGINF(gdb->mdb.group, ("\nPIM: Building batch join message for %i",
				gdb->mdb.group));
    
    /*
     * Are we the RP?
     */
    we_are_rp = ip_ouraddress(gdb->rp.address);

    /*
     * Scan the radix tree and build packets for each source that we
     * have state in the RP for.
     */
    ip = NULL;
    join_pak = NULL;
    rn_walktree(gdb->radix_head->rnh_treetop, pim_build_entry, gdb,
		&join_pak, NULL, &ip, we_are_rp);

    while (ip) {
	pak = ip;
	ip = (paktype *) ip->dataptr;
	pim_send_packet(pak, gdb->mdb.group, FALSE, TRUE);
    }
}

/*
 * pim_init_packet_buffer
 *
 * Get join and prune buffers for message sent to RP.
 */
boolean pim_init_packet_buffer (gdbtype *gdb, paktype **join_pak, 
				paktype **prune_pak, boolean insert_rp)
{

    paktype       *jpak, *ppak;
    pim_grouptype *pim_group;
    pim_addrtype  *pim_addr;

    /*
     * If there is no RPF interface to RP or we are the RP, don't send a Join.
     */
    if (!gdb->mdb.if_input) return(TRUE);

    /*
     * If the interface to RP is a boundary for the group, don't send a join.
     */
    if (ipmulticast_test_boundary(gdb->mdb.if_input, gdb->mdb.group)) {
	PIM_BUGINF(gdb->mdb.group,
		   ("\nPIM: Join to RP not sent across boundary"));
	return(TRUE);
    }

    /*
     * If there are no outgoing interfaces in (*,G) and the group is 
     * not directly connected (or we're a member of), don't send Join for
     * (*,G). However, we might want to include (S,G)s in this packet so
     * we still want to send a Join but without the RP-bit,WC-bit entry in the
     * join-list.
     */
    if (MDB_OUTPUTQ_NULL(&gdb->mdb) && 
	!(gdb->mdb.flags & (CONNECTED_FLAG | LOCAL_FLAG))) {
	insert_rp = FALSE;
    }

    /*
     * If the RPF neighbor is not a PIM router, don't send any PIM messages.
     */
    if (!pim_find_nbr(gdb->mdb.rpf_nbr, gdb->mdb.if_input)) return(TRUE);

    /*
     * If the RP is down (received no RP-Reachability messages), don't send
     * Join.
     */
    if (!gdb->rp.address) return(TRUE);

    /*
     * Allocate Join buffer.
     */
    jpak = getbuffer(gdb->mdb.if_input->ip_mtu);
    if (!jpak) return(FALSE);

    /*
     * Allocate Prune buffer.
     */
    ppak = getbuffer(gdb->mdb.if_input->ip_mtu);
    if (!ppak) {
	retbuffer(jpak);
	return(FALSE);
    }
    jpak->length = IPHEADERBYTES(NOPT) + IGMPHEADERBYTES + sizeof(pimtype)
	+ sizeof(pim_grouptype);
    ppak->length = 0;

    /*
     * Put (*,G) in join-list. Set wildcard bit and address of RP.
     */
    pim_group = (pim_grouptype *) (((char *) ipheadstart(jpak)) + 
	   IPHEADERBYTES(NOPT) + IGMPHEADERBYTES + sizeof(pimtype));

    pim_group->group = gdb->mdb.group;
    pim_group->group_mask = MASK_HOST;
    pim_group->num_join_sources = 0;
    pim_group->num_prune_sources = 0;

    /*
     * Add RP entry in join-list if request by caller.
     */
    if (insert_rp) {
	pim_addr = (pim_addrtype *) (pim_group + 1);
	pim_addr->pad = 0;
	pim_addr->sparse_bit = TRUE;
	pim_addr->wc_bit = TRUE;
	pim_addr->rp_bit = TRUE;
	pim_addr->mask_length = ip_bitsinmask(MASK_HOST);
	PUTLONG(pim_addr->address, gdb->rp.address);
	pim_group->num_join_sources++;
	jpak->length += PIM_ADDRTYPE_SIZE;
	PIM_BUGINF(gdb->mdb.group, 
		   ("\nPIM: For RP, Join-list: %i/%d, RP-bit, WC-bit",
		    gdb->rp.address, pim_addr->mask_length));
    }

    /*
     * Return buffer pointers to caller.
     */
    *join_pak = jpak;
    *prune_pak = ppak;
    return(TRUE);
}

/*
 * pim_select_join_prune_actions -
 *
 * Decide if an (S,G) entry should be inserted into a Join/Prune message.
 * This function decides 1) if an (S,G),RP-bit Prune should be sent, 2) if
 * this (S,G) should be included in the Join-List sent towards the S, or 3)
 * if this (S,G) should be included in the Join-list sent towards the RP.
 */
static pim_entrytype *pim_select_join_prune_actions (mdbtype *mdb, gdbtype *gdb,
						     boolean we_are_rp, 
						     pim_entrytype *ent)
{  
    midbtype *midb;
    boolean  null_olist;

    ent->send_join_toward_rp = FALSE;
    ent->send_rpbit_prune = FALSE;
    ent->send_join_toward_source = FALSE;

    /*
     * Bail if there is no incoming interface.
     */
    if (!mdb->if_input) return(NULL);

    null_olist = MDB_OUTPUTQ_NULL(mdb);

    /*
     * If (S,G) has null olist and we are the RP, if the incoming interface for
     * (S,G) is not in the olist for the (*,G) (or it is a dense-mode 
     * configured interface), don't insert this (S,G) in the Join/Prune 
     * message.
     */
    if (null_olist && we_are_rp) {
        midb = ip_get_midb(&gdb->mdb.if_outputQ, mdb->if_input, mdb->rpf_nbr,
			   IS_SPARSE_GROUP(gdb));
        if (!midb || IS_DENSE_MODE(midb->idb, gdb)) return(NULL);
    }

    /*
     * If the RPF neighbors are different between the (S,G) and (*,G), we
     * do want to send an RP-bit prune. Also, if the RP bit is set with a 
     * NULL OIF list, we want to send an RP-bit prune.
     *
     * If the RPF neighbors are the same, we want to send a join for (S,G) 
     * towards the RP, if no null olist (checked later). 
     * However, if the RP bit is set, we dont want to send the join.
     *
     * If the RPF neigbors are different, we want to send join towards the 
     * source, if no null olist (checked later).
     */
    ent->send_rpbit_prune = (mdb->rpf_nbr != gdb->mdb.rpf_nbr) ||
	((mdb->flags & RP_BIT_FLAG) && MDB_OUTPUTQ_NULL(mdb));
    ent->send_join_toward_rp = (mdb->rpf_nbr == gdb->mdb.rpf_nbr) && 
	!(mdb->flags & RP_BIT_FLAG);
    ent->send_join_toward_source = (mdb->rpf_nbr != gdb->mdb.rpf_nbr);

    /*
     * Check for null OIF list.
     *
     * The DR should send joins (even when the olist is null) towards the RP
     * or source if there are directly connected members on the incoming 
     * interface for (S,G). Otherwise, do not send Joins.
     */
    if (null_olist && !(mdb->if_input->ip_multicast_dr &&
	(mdb->flags & (CONNECTED_FLAG | LOCAL_FLAG)))) {
        ent->send_join_toward_rp = FALSE;
        ent->send_join_toward_source = FALSE;
    }

    /*
     * If RPF neighbor for source is not a PIM router, do not send joins.
     */
    if (!pim_find_nbr(mdb->rpf_nbr, mdb->if_input)) {
        ent->send_join_toward_rp = FALSE;
 	ent->send_join_toward_source = FALSE;
    } 

    /*
     * Return the selection.
     */
    if (ent->send_join_toward_rp || ent->send_join_toward_source || 
	ent->send_rpbit_prune) {
      return(ent);
    }
    return(NULL);
}


/*
 * pim_build_entry
 *
 * For each (S,G):
 *
 * 1) Put in join-list sent to RP if RPF neighbor is the same as RP. Do not
 *    put in any other neighbors join-lists.
 * 2) Put in prune-list sent to RP if RPF neighbor different than RP. Put
 *    in join-list for corresponding RPF neighbor (this is the part that
 *    has the same logic for Grafts).
 *
 * In the case of building a packet for the RP. Maintain a join-list buffer
 * and a prune-list buffer. Once the sum of the two buffer lengths hits
 * the RPF interface's MTU, send the packet to the RP and get new buffers.
 * If there is not enough data to fill an MTU-sized packet, the calling
 * function will send the packet to the RP.
 *
 * In the case of building a packet for each RPF neighbor associated with
 * (S,G)s, maintain a linked list of buffers, one for each RPF neighbor.
 * Once any buffer hits the RPF interface's MTU, the packet is sent. Same
 * goes if there is not enough data. The packet is transmitted in the
 * calling function (for all RPF neighbors).
 *
 * All this is done to optimize scaning a potentially large group database
 * a single time.
 */
int pim_build_entry (rntype *node, va_list args)
{

    gdbtype       *gdb;
    mdbtype       *mdb;
    paktype       **join_pak, **prune_pak, **pak;
    pim_grouptype *pim_group;
    pim_addrtype  *pim_addr;
    paktype       *jpak, *ppak, *ip;
    boolean       new_pak, we_are_rp;
    ipaddrtype    addr;
    pim_entrytype pim_entry_template, *entry;

    mdb = (mdbtype *) node;
    gdb = va_arg(args, gdbtype *);
    join_pak = va_arg(args, paktype **);
    prune_pak = va_arg(args, paktype **);
    pak = va_arg(args, paktype **);
    we_are_rp = va_arg(args, boolean);
  
    /*
     * Select what joins/prunes to be sent.
     */
    entry = pim_select_join_prune_actions(mdb, gdb, we_are_rp,
                                          &pim_entry_template);
    if (!entry) {
        return(0);
    }

    /*
     * Build variable part of message sent to RP. If RP is not a PIM router,
     * don't build/send it. If RP is down or there is no RPF interface for
     * it, don't build it. Continue to process other RPF neighbors.
     */
    if (*join_pak && (entry->send_rpbit_prune || entry->send_join_toward_rp)) {
	jpak = *join_pak;
	ppak = *prune_pak;
	pim_group = (pim_grouptype *) (((char *) ipheadstart(jpak)) + 
	       IPHEADERBYTES(NOPT) + IGMPHEADERBYTES + sizeof(pimtype));

	/*
	 * No more room for message to RP. Send what we have and start a
	 * new set.
	 */
	if (jpak->length + ppak->length + PIM_ADDRTYPE_SIZE >
	    gdb->mdb.if_input->ip_mtu) {
	    pim_send_rp_packet(gdb, jpak, ppak);
	    pim_init_packet_buffer(gdb, join_pak, prune_pak, FALSE);
	    jpak = *join_pak;
	    ppak = *prune_pak;
	    pim_group = (pim_grouptype *) (((char *) ipheadstart(jpak)) + 
		   IPHEADERBYTES(NOPT) + IGMPHEADERBYTES + sizeof(pimtype));
	}

	/*
	 * Send an RP-bit prune or add the source to the join-list
	 * towards the RP.
	 */
	if (entry->send_rpbit_prune) {
	    pim_addr = (pim_addrtype *) (ppak->data_area + ppak->length);
	    ppak->length += PIM_ADDRTYPE_SIZE;
	    pim_group->num_prune_sources++;
	    pim_addr->rp_bit = TRUE;
	    PIM_BUGINF(mdb->group, ("\nPIM: For RP, Prune-list:"));
	} else {
	    pim_addr = (pim_addrtype *) (((char *) pim_group) + 
		 sizeof(pim_grouptype) + 
		 pim_group->num_join_sources * PIM_ADDRTYPE_SIZE);
	    jpak->length += PIM_ADDRTYPE_SIZE;
	    pim_group->num_join_sources++;
	    pim_addr->rp_bit = FALSE;
	    PIM_BUGINF(mdb->group, ("\nPIM: For RP, Join-list:"));
	}

	pim_addr->pad = 0;
	pim_addr->sparse_bit = IS_SPARSE_GROUP(gdb) ? TRUE : FALSE;
	pim_addr->wc_bit = 0;
	pim_addr->mask_length = ip_bitsinmask(ip_radix2addr(mdb->mask));
	addr = ip_radix2addr(mdb->source);
	PUTLONG(pim_addr->address, addr);
	if (PIM_DEBUG(mdb->group)) {
	    buginf(" %i/%d%s", addr, pim_addr->mask_length, 
		   (pim_addr->rp_bit) ? ", RP-bit" : " ");
	}
    }

    /*
     * If we dont have to send joins towrds the RPF neighbor towards the
     * source, bail now.
     */
    if (!entry->send_join_toward_source)
	return(0);
    
    /*
     * Build message for RPF neighbor, put source in join-list.
     *
     * See if buffer is allocated for RPF neighbor. If so, add entry to
     * it. Otherwise, allocate new buffer. If there is no room in existing
     * buffer for entry, send packet and get new buffer.
     */
    new_pak = FALSE;
    for (ip = *pak; ip; ip = (paktype *) ip->dataptr) {
	if (mdb->rpf_nbr == ip->desthost) break;
    }
    if (ip && (ip->length + PIM_ADDRTYPE_SIZE) > mdb->if_input->ip_mtu) {
	pim_delink_packet(pak, ip);
	pim_send_packet(ip, mdb->group, FALSE, FALSE);
	ip = NULL;
    }

    if (!ip) {
	ip = getbuffer(mdb->if_input->ip_mtu);
	if (!ip) return(1);

	ip->length = IPHEADERBYTES(NOPT) + IGMPHEADERBYTES + sizeof(pimtype) + 
	    + sizeof(pim_grouptype);
	ip->desthost = mdb->rpf_nbr;
	ip->if_output = mdb->if_input;
	ip->dataptr = (*pak) ? (uchar *) *pak : NULL;
	*pak = ip;
	new_pak = TRUE;
    }

    /*
     * Set up address pointers in packet to store join-list count and
     * source address.
     */
    pim_group = (pim_grouptype *) 
	(((char *) ipheadstart(ip)) + IPHEADERBYTES(NOPT) + IGMPHEADERBYTES +
	 sizeof(pimtype));
    if (new_pak) {
	pim_group->group = mdb->group;
	pim_group->group_mask = MASK_HOST;
	pim_group->num_join_sources = 0;
	pim_group->num_prune_sources = 0;
	pim_addr = (pim_addrtype *) (pim_group + 1);
    } else {
	pim_addr = (pim_addrtype *) (((char *) pim_group) + 
	     sizeof(pim_grouptype) + 
	     pim_group->num_join_sources * PIM_ADDRTYPE_SIZE);
    }

    /*
     * Insert source adddress, mask count in packet.	
     */
    pim_addr->pad = 0;
    pim_addr->sparse_bit = IS_SPARSE_GROUP(gdb) ? TRUE : FALSE;
    pim_addr->wc_bit = FALSE;
    pim_addr->rp_bit = (mdb->flags & RP_BIT_FLAG) ? TRUE : FALSE;
    pim_addr->mask_length = ip_bitsinmask(ip_radix2addr(mdb->mask));
    addr = ip_radix2addr(mdb->source);
    PUTLONG(pim_addr->address, addr);

    PIM_BUGINF(mdb->group, ("\nPIM: For %i, Join-list: %i/%d%s", ip->desthost,
			    addr, pim_addr->mask_length,
			    (pim_addr->rp_bit) ? ", RP-bit" : " "));

    /*
     * Add to number of sources in packet and adjust size of packet.	
     */
    pim_group->num_join_sources++;
    ip->length += PIM_ADDRTYPE_SIZE;
    return(0);
}

/*
 * pim_send_rp_packet
 *
 * Send periodic Join/Prune packet to RP specifically.
 */
void pim_send_rp_packet (gdbtype *gdb, paktype *join_pak, paktype *prune_pak)
{

    /*
     * Copy prune-list buffer into the join-list buffer that has IP header,
     * IGMP header and PIM fixed part header.
     */
    bcopy(prune_pak->data_area, 
	  ((uchar *) ipheadstart(join_pak)) + join_pak->length,
	  prune_pak->length);
    join_pak->length += prune_pak->length;
    join_pak->if_output = gdb->mdb.if_input;
    join_pak->desthost = gdb->mdb.rpf_nbr;

    /*
     * No longer need prune-list buffer.
     */
    retbuffer((paktype *) prune_pak);

    /*
     * Send it.
     */
    pim_send_packet(join_pak, gdb->mdb.group, TRUE, FALSE);
}

/*
 * pim_send_packet
 *
 * Send periodic Join/Prune packet.
 */
void pim_send_packet (paktype *pak, ipaddrtype group, boolean to_rp, 
		      boolean batch)
{
    
    idbtype       *idb;
    igmptype      *igmp;
    pimtype       *pim;
    pim_grouptype *pimgroup;
    ipaddrtype    nbr;
    ulong         length;

    length = pak->length;
    idb = pak->if_output;
    nbr = pak->desthost;
    if (!nbr && is_p2p(idb) && idb->pim_nbrs) {
	nbr = idb->pim_nbrs->nbr_address;
    }
    if (!nbr) nbr = ALLROUTERS;

    /*
     * Build IGMP fixed part of header.
     */
    igmp = (igmptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));
    igmp->type = IGMP_PIM_TYPE;
    igmp->code = PIM_JOIN_PRUNE_CODE;
    igmp->checksum = 0;
    igmp->address = PIM_VERSION1;

    /*
     * Build PIM fixed part of header. The join-list is built in
     * pim_build_graft_entry().
     */
    pim = (pimtype *) igmp->data;
    pim->address = nbr;
    pim->holdtime = (PIM_MESSAGE_INTERVAL * PIM_MULTIPLIER) / ONESEC;
    pim->reserved1 = 0;
    pim->reserved2 = 0;
    pim->maddr_length = sizeof(ipaddrtype);
    pim->addr_length = sizeof(ipaddrtype);
    pim->num_groups = 1;

    /*
     * Stop here if number of join sources and prune sources are 0. Don't
     * send packet. This assumes 1 group is encoded in packet.
     */
    pimgroup = (pim_grouptype *) pim->data;
    if ((pimgroup->num_join_sources + pimgroup->num_prune_sources) == 0) {
	PIM_BUGINF(group, ("\nPIM: No sources in join or prune list"));
	datagram_done(pak);
	return;
    }

    igmp->checksum = ipcrc((ushort *) igmp, length - IPHEADERBYTES(NOPT));

    if (batch) {
	PIM_BUGINF(group, ("\nPIM: Send triggered batch join to %i (%s)",
			   nbr, idb->namestring));
    } else {
	PIM_BUGINF(group, ("\nPIM: Send periodic Join/Prune to%s%i (%s)", 
		       (to_rp) ? " RP via " : " ", nbr, idb->namestring));
    }

    /*
     * If unnumbered interface, make sure packet goes out desired interface
     * rather than the one the neighbor address is on. We may be connected
     * on the numbered interface and don't want to send out on it.
     */
    if (idb->ip_unnumbered) pak->if_output = idb;

    if (!is_p2p(idb) && !idb->pim_nbma_mode) {
	nbr = ALLROUTERS;
    }

    /*
     * Send to IP.
     */
    ipwrite(pak, length, IGMP_PROT, 0, nbr, idb, NULL, &igmprouteparams);
    igmp_traffic.output_pim++;
}

/*
 * pim_send_sm_join
 *
 * Sends a Join towards the source and a Prune for the source towards the
 * RP. 
 */
void pim_send_sm_join (gdbtype *gdb, mdbtype *mdb)
{

    /*
     * Send Join for (S,G) towards S.
     */
    pim_send_join_prune(mdb, mdb->if_input, mdb->rpf_nbr, TRUE, FALSE, FALSE,
			TRUE);

    /*
     * Send Prune for (S,G) towards RP if the RPF neighbor is different
     * than where the Join was sent.
     */
    if (gdb->mdb.rpf_nbr != mdb->rpf_nbr) {
	pim_send_join_prune(mdb, gdb->mdb.if_input, gdb->mdb.rpf_nbr, FALSE, 
			    FALSE, TRUE, TRUE);
    }
}

/*
 * pimv2_send_register_packet
 *
 * Build and send Register message to RP. If pak is null, then we are
 * sending a NULL Register.
 */
static void pimv2_send_register_packet (paktype *pak, ipaddrtype rp_address)
{

    iphdrtype *pakhdr;
    paktype   *ip;
    pimv2_header *pim;
    uchar     *ptr;
    ulong     *register_bits;
    ulong     size;

    pakhdr = NULL;
    size = IPHEADERBYTES(NOPT) + PIMHEADERBYTES + PIMV2_REGISTER_SIZE;
    if (pak) {
	pakhdr = (iphdrtype *) ipheadstart(pak);
	size += pakhdr->tl;
    }

    /*
     * Get packet buffer.
     */
    ip = getbuffer(size);
    if (!ip) return;

    /*
     * Build PIMv2 fixed part of header.
     */
    pim = (pimv2_header *) (ipheadstart(ip) + IPHEADERBYTES(NOPT));
    pim->ver = PIMV2;
    pim->type = PIMV2_REGISTER_TYPE;
    pim->reserved = 0;
    pim->checksum = 0;
    register_bits = (ulong *) (pim + 1);
    *register_bits = 0;

    /*
     * Data packet is encapsulated in the Register.
     */
    if (pakhdr) {
	ptr = ((uchar *) pim) + PIMHEADERBYTES + PIMV2_REGISTER_SIZE;
	bcopy(ipheadstart(pak), ptr, pakhdr->tl);
	PIM_BUGINF(pakhdr->dstadr, 
		   ("\nPIM: Send v2 Register to %i for %i, group %i", 
		    rp_address, pakhdr->srcadr, pakhdr->dstadr));
    } else {
	*register_bits |= PIMV2_NULL_REGISTER_BIT;
	PIM_BUGINF(0, ("\nPIM: Send v2 Null Register to %i", rp_address));
    }

    /*
     * Do IGMP checksum on whole packet.
     */
    pim->checksum = ipcrc((ushort *) pim, size - IPHEADERBYTES(NOPT));

    /*
     * Send down to IP. Use data's ttl if a data Register. Use default ttl
     * and Internet Control precedence (default setting) for Null Registers.
     */
    if (pakhdr) {
	pimrouteparams.ttl = pakhdr->ttl;
	pimrouteparams.tos = TOS_PREC_ROUTINE;
    } else {
	pimrouteparams.ttl = TTL_DEFAULT;
    }
    ipwrite(ip, size, PIM_PROT, 0, rp_address, NULL, NULL, &pimrouteparams);
    pim_traffic.output_registers++;
    pimrouteparams.ttl = 1;
    pimrouteparams.tos = TOS_PREC_INET_CONTROL;
}

/*
 * pim_send_register
 *
 * Send Register messages to RPs so (S,G) state can be created from here
 * to all possible RPs associated with group.
 */
void pim_send_register (gdbtype *gdb, mdbtype *mdb, paktype *pak)
{

    pim_rptype *rp;

    if (gdb->rp.address) {
	if (!mdb->register_flag) {
	    return; 
	}
	if (ip_ouraddress(gdb->rp.address)) {
	    mdb->register_flag = FALSE;
	    return;
	} else {
	    mdb->flags |= SET_REG_FLAG;
	    gdb->mdb.flags |= SET_REG_FLAG;
	}

	/*
	 * Start NULL Register timer, if not started.
	 */
	rp = pim_find_rp(gdb->mdb.group, gdb->rp.address);
	if (rp && !TIMER_RUNNING(rp->send_register)) {
	    TIMER_START(rp->send_register, PIM_NULL_REG_INTERVAL);
	}

	if (rp) {

	    /*
	     * If we are configured to run PIMv2, try version 2 register if
	     *  1) the version of this RP is not known, i.e. 0;
	     *  2) the version of this RP is 2;
	     *
	     * This makes sure the DR always starts by trying version 2
	     * register. Once a a version 2 register_stop is received,
	     * subsequent registers to this RP will be in version 2, until
	     * it times out. Then it tries version 1 register. If the
	     * version 1 register also times out, the rp is removed,
	     * and reinstalled, thus starting another cycle.
	     */
	    if ((pim_version & PIMV2) && 
		(!rp->version || (PIM_RP_IS_V2(rp->version)))) {
		pimv2_send_register_packet(pak, gdb->rp.address);
	    } else {
		pim_send_register_packet(pak, gdb->rp.address);
	    }
	}
	return;
    }

    /*
     * There are no RPs, store RP info in gdb and send Registers. This means
     * that the group state was just created (by receipt of a data packet).
     */
    pim_get_all_rps(gdb, TRUE, FALSE);

    /*
     * We've built some RP state. Set flags so we know which RPs to send
     * Registers to. Then recurse exactly one time.
     */
    if (gdb->rp.address) {
	mdb->register_flag = TRUE;
	pim_send_register(gdb, mdb, pak);
    }
}

/*
 * pim_send_register_packet
 *
 * Build and send Register message to RP. If pak is null, then we are
 * sending a NULL Register to see if the RP has come back up.
 */
void pim_send_register_packet (paktype *pak, ipaddrtype rp_address)
{

    iphdrtype *pakhdr;
    paktype   *ip;
    igmptype  *igmp;
    uchar     *ptr;
    ulong     size;

    pakhdr = NULL;
    size = IPHEADERBYTES(NOPT) + IGMPHEADERBYTES;
    if (pak) {
	pakhdr = (iphdrtype *) ipheadstart(pak);
	size += pakhdr->tl;
    }

    /*
     * Get packet buffer.
     */
    ip = getbuffer(size);
    if (!ip) return;

    /*
     * Build IGMP fixed part of header.
     */
    igmp = (igmptype *) (ipheadstart(ip) + IPHEADERBYTES(NOPT));
    igmp->type = IGMP_PIM_TYPE;
    igmp->code = PIM_REGISTER_CODE;
    igmp->checksum = 0;
    igmp->address = PIM_VERSION1;

    /*
     * Data packet is encapsulated in the Register.
     */
    if (pakhdr) {
	ptr = (uchar *) igmp->data;
	bcopy(ipheadstart(pak), ptr, pakhdr->tl);
	PIM_BUGINF(pakhdr->dstadr, 
		   ("\nPIM: Send Register to %i for %i, group %i", 
		    rp_address, pakhdr->srcadr, pakhdr->dstadr));
    } else {
	PIM_BUGINF(0, ("\nPIM: Send Null Register to %i", rp_address));
    }

    /*
     * Do IGMP checksum on whole packet.
     */
    igmp->checksum = ipcrc((ushort *) igmp, size - IPHEADERBYTES(NOPT));

    /*
     * Send down to IP. Use data's ttl if a data Register. Use default ttl
     * and Internet Control precedence (default setting) for Null Registers.
     */
    if (pakhdr) {
	igmprouteparams.ttl = pakhdr->ttl;
	igmprouteparams.tos = TOS_PREC_ROUTINE;
    } else {
	igmprouteparams.ttl = TTL_DEFAULT;
    }
    ipwrite(ip, size, IGMP_PROT, 0, rp_address, NULL, NULL, &igmprouteparams);
    igmprouteparams.ttl = 1;
    igmprouteparams.tos = TOS_PREC_INET_CONTROL;
}

/*
 * pim_receive_register
 *
 * Process received Register packet. By receiving a Register packet, determines
 * we are RP for the group. Create (*,G) state if it does not exist. Trigger
 * Join to source of Register. Decapsulate packet and send down (*,G) tree.
 *
 * This function processes both PIMv1 and PIMv2 registers.
 */
static void pim_receive_register (iphdrtype *ip, iphdrtype *datapak,
				  idbtype *if_input, uchar version)
{

    gdbtype    *gdb;
    mdbtype    *mdb, *smdb;
    idbtype    *idb;
    midbtype   *midb;
    paktype    *newip, *dup;
    ipaddrtype source, mask, rpf_nbr;
    boolean    send_join, sent, no_mdb;
    uchar      rpf_type;
    pim_rptype *rp;
    
    /*
     * Validate if we are going to be the RP. If not, send a Register-Stop
     * to the source DR to shut it up. Issue log message.
     */
    if (datapak && (!pim_accept_rp(ip->dstadr, datapak->dstadr))) {
	pim_send_register_stop(datapak->dstadr, ip->dstadr, ip->srcadr, 
			       datapak->srcadr, version);
	errmsg(&msgsym(INVALID_RP_REG, PIM), ip->srcadr, datapak->dstadr);
	return;
    }

    /*
     * Don't accept Registers that are not addressed to us. The Register 
     * could have been erroneously sent to a multicast address or an old
     * IP address assigned to us.
     */
    if (!ip_ouraddress(ip->dstadr)) {
	PIM_BUGINF(0, ("\nPIM: Register sent to invalid address %i",
		       ip->dstadr));
	return;
    }

    /*
     * Test to see the Register source address is bogus.
     */
    if (martian(ip->srcadr, TRUE)) {
	PIM_BUGINF(0, ("\nPIM: Register sent with invalid source address %i",
		       ip->srcadr));
	return;
    }

    gdb = NULL;
    mdb = NULL;
    if (datapak) gdb = ip_get_gdb(datapak->dstadr);
    if (gdb) mdb = ip_get_best_mdb(gdb, datapak->srcadr);

    /*
     * Acknowledge Register so source router stops encapsulating. Acknowledge
     * Null Register so source router knows RP is still up. Only send
     * Register-Stop if we have received data natively, or if we have
     * "ip pim fast-register-stop" configured.
     */
    if (!datapak) {
	pim_send_register_stop(0, ip->dstadr, ip->srcadr, 0, version);
	return;
    } else if (pim_immediate_register_stop ||
	       (mdb && ((mdb->flags & SPT_FLAG) || MDB_OUTPUTQ_NULL(mdb)))) {
	pim_send_register_stop(datapak->dstadr, ip->dstadr, ip->srcadr, 
			       datapak->srcadr, version);
	if (mdb && (mdb->flags & SPT_FLAG)) return;
    }

    /*
     * No (S,G) state, create it. There may be no (*,G) state either.
     */
    no_mdb = FALSE;
    send_join = FALSE;
    if (!mdb) {
	no_mdb = TRUE;
	rpf_nbr = 0;
	idb = ip_rpf_lookup(datapak->srcadr, MASK_HOST, &source, &mask, 
			    &rpf_nbr, &rpf_type, ROUTINGDEPTH);
	if (!idb) {
	    PIM_BUGINF(datapak->dstadr, ("\nPIM: RPF lookup failed to source %i", 
			datapak->srcadr));
	    return;
	}

	mdb = ip_add_mroute(datapak->dstadr, datapak->srcadr, MASK_HOST, idb, 
			    rpf_nbr, rpf_type, MDB_EXPIRATION_TIME);
	if (!mdb) return;

	/*
	 * If group didn't exist, create state for it and populate
	 * outgoing interface list for (*,G).
	 */
	if (!gdb) {
	    gdb = ip_get_gdb(datapak->dstadr);
	    if (!gdb) return;
	    ip_build_outputQ(NULL, gdb, &gdb->mdb);
	}
    
	/*
	 * Build outgoing interface list for (S,G).
	 */
	ip_build_outputQ(&gdb->mdb.if_outputQ, gdb, mdb);
	ip_store_mdb_timer(mdb);

	/*
	 * Send Join if (S,G) list is non-NULL. If it is NULL, we may
	 * still want to send a Join. The RPF interface to the source
	 * may also have downstream members.
	 */
	send_join = !MDB_OUTPUTQ_NULL(&gdb->mdb);

	/*
	 * Clear flag to indicate that the source's packets are being 
	 * encapsulated in Registers. This flag is set once we get
	 * data natively. This means the Join made it to the source.
	 */
	mdb->flags &= ~SPT_FLAG;
    }

    /*
     * Store/update RP info.
     */
    rp = pim_find_rp(mdb->group, ip->dstadr);
    pim_store_rp_info(gdb, ip->dstadr, RP_UP, FALSE,
		      rp ? !rp->configured : TRUE);

    /*
     * Set up data packet to be natively sent.
     */
    newip = getbuffer(datapak->tl);
    if (!newip) return;
    ip = (iphdrtype *) ipheadstart(newip);
    bcopy(datapak, ip, datapak->tl);

    /*
     * Fix up TTL and checksum header.
     */
    ip->ttl = datapak->ttl;
    ip->checksum = 0;
    ip->checksum = ipcrc((ushort *) ip, datapak->ihl << 2);

    /*
     * If packet has Router-Alert option, pass to IP input. This might be
     * an RSVP message. RSVP messages don't get forwarded by multicast
     * routing.
     */
    ipopt_inproc(newip);
    if (newip->ip_flags & PAKOPT_HASALERT) {
	change_if_input(newip, nullidb);
	process_enqueue_pak(IPpacketQ, newip);
	return;
    }

    /*
     * Forward data packet down RP-tree.
     */
    sent = FALSE;
    smdb = mdb;
    mdb = &gdb->mdb;
    for (midb = (midbtype *) mdb->if_outputQ.qhead; midb; midb = midb->next) {
	if (!midb->idb->pim_nbrs && 
	    !igmp_find_group(gdb->mdb.group, midb->idb)) continue;

	/*
	 * If the source is directly conneced on one of the (*,G) outgoing
	 * interfaces, don't send packet back out on it since it has already
	 * been sent and encapsulated by the DR on the LAN.
	 */
	if (midb->idb == samecable(datapak->srcadr)) continue;
	 
	/*
	 * Do access control checks.
	 */
	if (ip->ttl < midb->idb->ip_multicast_threshold) continue;
	if (midb->idb->ip_output_accesslist) {
	    if (!ip_accesscheck(newip, midb->idb->ip_output_accesslist)) {
		continue;
	    }
	}
	
	/*
	 * Should we rate-limit on output?
	 */
	if (mdb->bits_sec + (ip->tl << 3) > midb->out_limit) continue;

	dup = pak_duplicate(newip);
	if (!dup) break;

	PIM_BUGINF(gdb->mdb.group, 
		   ("\nPIM: Forward decapsulated data packet for %i on %s",
		    gdb->mdb.group, midb->idb->namestring));

	dup->if_output = midb->idb;

	/*
	 * Set a non-null value in the if_input, so that ipsendnet() wouldn't
	 * loop back this packet by treating it as a locally generated packet.
	 */
	dup->if_input = midb->idb;
	dup->desthost = midb->next_hop;
	sent |= ipsendnet(dup, TRUE, TRUE);
    }

    if (no_mdb) {

	/*
	 * Send triggered join or prune towards source. Depends if there
	 * is anything in the outgoing interface list. Note that the join
	 * is sent after the decapsulated data packet was forwarded.
	 */
	pim_send_join_prune(smdb, smdb->if_input, smdb->rpf_nbr, send_join, 
			    FALSE, FALSE, TRUE);
    }

    /*
     * Count packets sent on RP-tree.
     */
    if (sent) {
	mdb->total_received++;
	mdb->uses++;
	mdb->uses_sec++;
	mdb->bytes += ip->tl;
	mdb->bits_sec += ip->tl << 3;
	TIMER_START(mdb->expires, mdb->expiration_time);
    }

    /*
     * Always update the (S,G) timer on receipt of a register, although
     * packet was forwarded according to the (*,G) state.
     */
    TIMER_START(smdb->expires, smdb->expiration_time);
    
    /*
     * Calling function returns original Register buffer.
     */
    retbuffer(newip);
}

/*
 * pimv1_receive_register
 * Wrapper for PIMv1 register message
 */
void pimv1_receive_register (paktype *pak, igmptype *igmp, idbtype *if_input)
{
    iphdrtype  *ip;
    iphdrtype  *datapak;

    ip = (iphdrtype *) ipheadstart(pak);

    /*
     * Determine if there is data in the Register.
     */
    if (ip->tl == (ip->ihl << 2) + IGMPHEADERBYTES) {
	datapak = NULL;
    } else {
	datapak = (iphdrtype *) igmp->data;
    }

    pim_receive_register(ip, datapak, if_input, PIMV1);
}

/*
 * pimv2_receive_register
 * Wrapper for PIMv2 register message
 */
void pimv2_receive_register (paktype *pak, pimv2_header *pim,
			     idbtype *if_input)
{
    iphdrtype *ip;
    iphdrtype   *datapak;

    ip = (iphdrtype *) ipheadstart(pak);

    /*
     * Determine if there is data in the Register.
     */
    if (ip->tl == (ip->ihl << 2) + PIMHEADERBYTES + PIMV2_REGISTER_SIZE) {
	datapak = NULL;
    } else {
	datapak = (iphdrtype *) 
	    (((uchar *) pim) + PIMHEADERBYTES + PIMV2_REGISTER_SIZE);
    }

    pim_receive_register(ip, datapak, if_input, PIMV2);

}

/*
 * pimv1_send_register_stop
 *
 * Build and send a version 1 Register-Stop packet to source of Register.
 */
static void pimv1_send_register_stop (ipaddrtype group, ipaddrtype srcadr,
			     ipaddrtype dstadr, ipaddrtype source)
{

    paktype  *pak;
    igmptype *igmp;
    ulong    *pim;
    ulong    size;

    /*
     * Get packet buffer.
     */
    size = IPHEADERBYTES(NOPT) + IGMPHEADERBYTES + PIM_REGISTER_STOP_SIZE;

    pak = getbuffer(size);
    if (!pak) return;

    /*
     * Build IGMP fixed part of header.
     */
    igmp = (igmptype *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));
    igmp->type = IGMP_PIM_TYPE;
    igmp->code = PIM_REGISTER_STOP_CODE;
    igmp->checksum = 0;
    igmp->address = PIM_VERSION1;
    pim = (ulong *) igmp->data;
    *pim++ = group;
    *pim = source;

    /*
     * Do IGMP checksum on whole packet.
     */
    igmp->checksum = ipcrc((ushort *) igmp, size - IPHEADERBYTES(NOPT));
    
    PIM_BUGINF(group, ("\nPIM: Send Register-Stop to %i for %i, group %i", 
		       dstadr, source, group));

    /*
     * Send down to IP.
     */
    igmprouteparams.ttl = TTL_DEFAULT;
    ipwrite(pak, size, IGMP_PROT, srcadr, dstadr, NULL, NULL, 
	    &igmprouteparams);
    igmprouteparams.ttl = 1;
}

/*
 * pimv2_send_register_stop
 *
 * Build and send a version 2 Register-Stop packet to source of Register.
 */
static void pimv2_send_register_stop (ipaddrtype group, ipaddrtype srcadr,
			     ipaddrtype dstadr, ipaddrtype source)
{
    paktype  *pak;
    pimv2_header *pim;
    pimv2_encoded_group *encoded_group;
    pimv2_unicast_address *unicast_source;
    ulong    size;

    /*
     * Get packet buffer.
     */
    size = IPHEADERBYTES(NOPT) + PIMHEADERBYTES + PIMV2_REGISTER_STOP_SIZE;

    pak = getbuffer(size);
    if (!pak) return;

    /*
     * Build PIM fixed part of header.
     */
    pim = (pimv2_header *) (ipheadstart(pak) + IPHEADERBYTES(NOPT));
    pim->ver = PIMV2;
    pim->type = PIMV2_REG_STOP_TYPE;
    pim->reserved = 0;
    pim->checksum = 0;
    encoded_group = (pimv2_encoded_group *) (pim + 1);
    encoded_group->addr_family = ADDR_IP;
    encoded_group->encode_type = 0;
    encoded_group->reserved = 0;
    encoded_group->mask_len = HOST_MASK_LEN;
    encoded_group->group_addr = group;
    unicast_source = (pimv2_unicast_address *) (encoded_group + 1);
    unicast_source->addr_family = ADDR_IP;
    unicast_source->encode_type = 0;
    PUTLONG(unicast_source->address, source);

    /*
     * Do PIM checksum on whole packet.
     */
    pim->checksum = ipcrc((ushort *) pim, size - IPHEADERBYTES(NOPT));
    
    PIM_BUGINF(group, ("\nPIM: Send v2 Register-Stop to %i for %i, group %i", 
		       dstadr, source, group));

    /*
     * Send down to IP.
     */
    pimrouteparams.ttl = TTL_DEFAULT;
    ipwrite(pak, size, PIM_PROT, srcadr, dstadr, NULL, NULL, 
	    &pimrouteparams);
    pim_traffic.output_register_stops++;
    pimrouteparams.ttl = 1;
}

/*
 * pim_send_register_stop
 * wrapper that sends either version1 or version2 register stop
 */
void pim_send_register_stop (ipaddrtype group, ipaddrtype srcadr,
			    ipaddrtype dstadr, ipaddrtype source,
			    uchar version)
{
    if (version == PIMV1) {
        pimv1_send_register_stop(group, srcadr, dstadr, source);
    } else if (version == PIMV2) {
        pimv2_send_register_stop(group, srcadr, dstadr, source);
    }
}

/*
 * pim_receive_register_stop
 *
 * Process a Register-Stop packet from the RP.
 */
static void pim_receive_register_stop (ipaddrtype group, ipaddrtype source,
				ipaddrtype rp_address, ulong length,
				uchar version)
{

    gdbtype    *gdb;
    mdbtype    *mdb;
    pim_rptype *rp, *rp1;

    /*
     * If group address is 0.0.0.0, this is a Register-Stop in response
     * to a Null Register sent. Reset Null Register transmission interval.
     * If RP has gone down, we can bring it up. We will send encapsulated
     * Registers now. This is flagged in ip_mroute_ager(). Non-null 
     * Register-Stops can detect that an RP has come up too.
     */
    rp = pim_find_rp(group, rp_address);
    if (rp) {
	if (!rp->configured) {
	    TIMER_START(rp->expires, PIM_RP_EXPIRES);
	}
	TIMER_START(rp->send_register, PIM_NULL_REG_INTERVAL);

	/*
	 * Set state to UP and keep expiration timer to 0, this tells 
	 * pim_age_rp() that RP has just come back up.
	 */
	if (rp->state == RP_DOWN) {
	    rp->state = RP_UP;
	    PIM_BUGINF(0, ("\nPIM: RP %i is back up", rp->rp_address));
	}

	/*
	 * Remember the version of this RP.
	 */
	rp->version = version;

	/*
	 * If the rp above is a semi-static entry from the learned RP
	 * queue, the same RP address is also in the configured_rp queue,
	 * we need to mark the version there too.
	 */
	rp1 = pim_find_configured_rp(group, rp_address);
	if (rp1 && rp1->rp_address == rp->rp_address) rp1->version = version;
    }
    if (!group) return;

    /*
     * Process a Register-Stop for a (S,G).
     */
    gdb = ip_get_gdb(group);
    if (!gdb) return;

    mdb = ip_get_best_mdb(gdb, source);
    if (!mdb) {
	PIM_BUGINF(group, ("\nPIM: No state for source %i, group %i", 
			   source, group));
	return;
    }

    /*
     * Find RP for group G. Update group RP expiration timer.
     */
    if (rp_address == gdb->rp.address) {
	TIMER_START(gdb->rp.expires, PIM_RP_EXPIRES);
    } else {

	/*
	 * RP not found.
	 */
	PIM_BUGINF(group, ("\nPIM: %i is not RP for %i", rp_address, group));
	return;
    }

    /*
     * Clear Register flag, so we don't have to encapsulate to this RP anymore
     * for this (S,G).
     */
    mdb->register_flag = FALSE;
    PIM_BUGINF(group, ("\nPIM: Clear register flag to %i for (%i/%d, %i)", 
	       rp_address, ip_radix2addr(mdb->source),
	       ip_bitsinmask(ip_radix2addr(mdb->mask)), group));
}

/*
 * pimv1_receive_register_stop
 *
 * Wrapper that extracts the source, group addresses from a version 1
 * register stop and calls pim_receive_register_stop()
 */
void pimv1_receive_register_stop (igmptype *igmp, ipaddrtype rp_address,
				  ulong length)
{
    ulong      *pim;
    ulong      group, source;

    /*
     * Validate Register-Stop is long enough.
     */
    if (length < PIM_REGISTER_STOP_SIZE) {
	PIM_BUGINF(0, ("\nPIM: Packet too short"));
	return;
    }

    pim = (ulong *) igmp->data;
    group = *pim++;
    source = *pim;
    pim_receive_register_stop(group, source, rp_address, length, PIMV1);
}
/*
 * pimv2_receive_register_stop
 *
 * Wrapper that extracts the group and source addresses from a version 2
 * register stop, and calls pim_receive_register_stop()
 */
void pimv2_receive_register_stop (pimv2_header *pim,
				  ipaddrtype rp_address, ulong length)
{
    ipaddrtype source, group;
    pimv2_encoded_group *encode_group;
    pimv2_unicast_address *unicast_source;

    if (length < PIMV2_REGISTER_STOP_SIZE + PIMHEADERBYTES) {
	PIM_BUGINF(0, ("\nPIM: Packet too short"));
	return;
    }
    encode_group = (pimv2_encoded_group *) (pim + 1);
    unicast_source = (pimv2_unicast_address *) (encode_group + 1) ;

    if (encode_group->addr_family != ADDR_IP || encode_group->encode_type) {
	PIM_BUGINF(0,
	    ("\nPIM: Unknown group address format (family:%d, encoding:%d)",
	     encode_group->addr_family, encode_group->encode_type));
	return;
    }
    if (unicast_source->addr_family != ADDR_IP || 
	unicast_source->encode_type) {
	PIM_BUGINF(0, ("\nPIM: Unknown source format (family:%d, encoding:%d)",
		 unicast_source->addr_family, unicast_source->encode_type));
	return;
    }
    group = encode_group->group_addr;
    source = GETLONG(unicast_source->address);
    PIM_BUGINF(group, ("\nPIM:   for source %i, group %i", source, group));
    pim_receive_register_stop(group, source, rp_address, length, PIMV2);
}

/*
 * pim_fix_null_register_version
 *
 * Figure out the correct null register version to send, and send it.
 * There is no IP header inside this NULL register.
 */
static void pim_fix_null_register_version (pim_rptype *rp,
					   boolean is_learned)
{
    boolean probe_rp;
    ulong ver;

    if (!rp->send_null_reg) return;
    if (!TIMER_RUNNING_AND_AWAKE(rp->send_register)) return;

    ver = rp->version;

    /*
     * Only need to probe the version of RP, if (1) this is a configured RP;
     * Or (2) this is a learned RP but it does not specify a version.
     */
    probe_rp = (is_learned && (!rp->prm || !rp->prm->rp_version)) ?
               FALSE : TRUE;

    if (!probe_rp) {
        if (rp->prm) {
	    ver = rp->prm->rp_version;
	} else {
	    ver = PIMV1;
	}
    } else {

        /*
	 * Probe RP's version. If (1) we are configured to run
	 * version 2; (2) rp->version is 0, and we are a PIMv1+ DR
	 * for some source. We send alternating ver1 and ver2 NULL
	 * registers, to trigger ver1 or ver2 register stops.
	 * Note that once a ver2 register-stop is received, it stops
	 * toggling between ver1 and ver2, until timeout.
	 */
	if (!(PIM_RP_IS_V2(rp->version)) && (pim_version & PIMV2)) {
	    ver = rp->null_register_ver ? PIMV2 : PIMV1;
	    rp->null_register_ver = !rp->null_register_ver;
        }
    }

    if (PIM_RP_IS_V2(ver)) {
	pimv2_send_register_packet(NULL, rp->rp_address);
    } else {
	pim_send_register_packet(NULL, rp->rp_address);
    }
    TIMER_START(rp->send_register, PIM_NULL_REG_INTERVAL);
}

/*
 * pim_send_null_register
 *
 * Send a Register packet with no data to see if the RP has come back up.
 * This is done by DRs only.
 */
void pim_send_null_register (void)
{

    idbtype    *idb;
    pim_rptype *rp;
    list_element *l_elt;
    
    /*
     * Check to see if we are DR on any multicast interface.
     */
    FOR_ALL_IPMULTICAST_IDBS(idb, l_elt) {
	if (!idb->ip_mrouting) continue;
	if (idb->ip_multicast_dr) break;
    }

    /*
     * We're not DR on any multicast interface, cancel Null Register timer
     * so we know when to start it when/if we become the DR again.
     */
    if (!idb) {	
	for (rp = (pim_rptype *) pim_configured_rp.qhead; rp; rp = rp->next) {
	    TIMER_STOP(rp->send_register);
	}
	for (rp = (pim_rptype *) pim_learned_rp.qhead; rp; rp = rp->next) {
	    TIMER_STOP(rp->send_register);
	}
	return;
    }

    /*
     * Go through each configured RP, and if it is time to send a Register,
     * then do it. We send 1 Register to each RP regardless of the number of
     * groups the RP is supporting. Skip the INACTIVE configured RPs.
     */
    for (rp = (pim_rptype *) pim_configured_rp.qhead; rp; rp = rp->next) {
        pim_fix_null_register_version(rp, FALSE);
    }

    /*
     * Do the same for the learned RPs
     */
    for (rp = (pim_rptype *) pim_learned_rp.qhead; rp; rp = rp->next) {
        pim_fix_null_register_version(rp, TRUE);
    }
}

/*
 * pim_over_threshold
 *
 * Find SPT threshold, if any for this group. Returns TRUE if threshold
 * is exceeded, FALSE if not.
 */
boolean pim_over_threshold (ulong      current_bps,
			    gdbtype    *gdb)
{

    pim_spttype *thres;
    boolean     join_spt_candidate = FALSE;
    ipaddrtype  group;

    group = gdb->mdb.group;
    
    /*
     * If we are a DM group or if the (*,G) iif is NULL, set join_spt to TRUE
     * always.
     */
    if (!IS_SPARSE_GROUP(gdb) || !gdb->mdb.if_input) {
	return(TRUE);
    }

    /*
     * If a DVMRP neighbor is detected on the input interface
     * of the (*,G), join SPT.
     */
    if (DVMRP_ACTIVE(gdb->mdb.if_input)) {
	return(TRUE);
    }

    /*
     * Compatibility mode. If no thresholds are configured,
     * we want to switch to source tree.
     */
    thres = pim_spt_thresholds;
    if (!thres) {
	join_spt_candidate = TRUE;
    }

    /*
     * If access-list match, compare current bit-rate with configured
     * threshold bit-rate.
     */
    for ( ; thres; thres = thres->next) {
	if (!thres->group_acl || nfast_check(thres->group_acl, group)) {
	    if (current_bps >= thres->spt_threshold) {
		join_spt_candidate = TRUE;
	    }
	}
    }

    /*
     * Set the SPT Flag only if we have connected members.
     */
    if (join_spt_candidate &&
	(gdb->mdb.flags & (CONNECTED_FLAG | LOCAL_FLAG))) {
	return(TRUE);
    }
    
    return(FALSE);
}
