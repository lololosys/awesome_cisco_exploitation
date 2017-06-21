/* $Id: ipmulticast_init.c,v 3.4.52.7 1996/09/10 05:31:44 lwei Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/ipmulticast_init.c,v $
 *------------------------------------------------------------------
 * ipmulticast_init.c - Initialize IP Multicast Routing Protocol support.
 *
 * November 1993.
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipmulticast_init.c,v $
 * Revision 3.4.52.7  1996/09/10  05:31:44  lwei
 * CSCdi68465:  RP information is too difficult to change in last-hop
 *              routers
 * Branch: California_branch
 *
 * Revision 3.4.52.6  1996/07/23  23:38:16  mleelani
 * CSCdi61494:  Multicast fastswitching with incoming Dialer (ISDN) is
 * broken
 * Branch: California_branch
 * Support multicast fastswitching on incoming dialers.
 *
 * Revision 3.4.52.5  1996/07/06  05:54:30  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.4.52.4  1996/06/27  03:27:46  mleelani
 * CSCdi60876:  Multicast fastswitching flag should be compatible with
 * unicast
 * Branch: California_branch
 * Reflect unicast fastswitching polarity
 *
 * Revision 3.4.52.3  1996/06/18  01:46:46  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.4.52.2  1996/05/13  20:10:18  mleelani
 * CSCdi57016:  Multicast helper-maps not functional
 * Branch: California_branch
 * Fix helper-maps.
 *
 * Revision 3.4.52.1  1996/05/06  16:44:01  dino
 * CSCdi56689:  When a multipoint-VC goes away, the IP multicast cache is
 *              not fixed
 * Branch: California_branch
 *
 * Revision 3.4  1995/12/01  18:38:36  lwei
 * CSCdi44737:  IP multicast does not support administratively scoped
 * boundaries
 *
 * Revision 3.3  1995/11/21  23:12:02  dino
 * CSCdi43427:  cisco does not process DVMRP prunes.
 *
 * Revision 3.2  1995/11/17  17:29:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:01:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/20  17:05:24  dino
 * CSCdi36738:  Multicast sender being designated router
 *
 * Revision 2.1  1995/06/07  21:03:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "packet.h"
#include "boxtypes.h"
#include "subsys.h"
#include "access.h"
#include "sys_registry.h"
#include "logger.h"
#include "../os/free.h"
#include "../os/techsupport.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "../ip/ipaddress.h"
#include "../ip/ip_registry.h"
#include "../ip/ipaccess.h"
#include "../ipmulticast/mroute.h"
#include "../ipmulticast/igmp.h"
#include "../ipmulticast/pim.h"
#include "../ipmulticast/dvmrp.h"
#include "../ipmulticast/ipmulticast_debug.h"


/*
 * IP Multicast subsystem header
 */
#define IPMULTICAST_MAJVERSION  1
#define IPMULTICAST_MINVERSION  0
#define IPMULTICAST_EDITVERSION 1

SUBSYS_HEADER(ipmulticast, IPMULTICAST_MAJVERSION, IPMULTICAST_MINVERSION,
	      IPMULTICAST_EDITVERSION, ipmulticast_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost, subsys_req_iphost);

/*
 * ipmulticast_init_idb
 *
 * Initialize IP multicsat specific parts of the idb.
 */
static void ipmulticast_init_idb (idbtype *idb, boolean setup_hwidb)
{

    /*
     * Default IP multicast fastswitching to on except for ISDN
     * B channels and Virtual templates.
     */
    if ((idb->hwptr->status & IDB_VIRTUAL_HW) || 
        (idb->hwptr->status & IDB_VTEMPLATE)) {
	idb->ip_multicast_fs = FALSE;
    } else {
	idb->ip_multicast_fs = TRUE;
    }

    idb->ip_multicast_use_functional = FALSE;
    igmp_init_idb(idb);
    pim_init_idb(idb);
    dvmrp_init_idb(idb);
}

/*
 * ipmulticast_show_techsupport_commands
 *
 * Display useful IP multicast related information.
 */
static void ipmulticast_show_techsupport_commands (parseinfo *csb)
{
    show_techsupport_command(csb, "show ip pim interface");
    show_techsupport_command(csb, "show ip pim interface count");
    show_techsupport_command(csb, "show ip pim neighbor");
    show_techsupport_command(csb, "show ip pim rp");
    show_techsupport_command(csb, "show ip igmp groups");
    show_techsupport_command(csb, "show ip igmp interface");
    show_techsupport_command(csb, "show ip mroute count");
    show_techsupport_command(csb, "show ip mroute");
    show_techsupport_command(csb, "show ip mcache");
    show_techsupport_command(csb, "show ip dvmrp route");
}

/*
 * ipmutlicast_register_thyself
 */
static void ipmulticast_register_thyself (void)
{

    /*
     * Set up debugging flags.
     */
    ipmulticast_debug_init();

    /*
     * Register some functions with the REG_SYS super registry
     */
    reg_add_sw_idb_init(ipmulticast_init_idb, "ipmulticast_init_idb");

    /*
     * Register ip_mforward() so there is no IP unicast routing dependency
     * on IP multicast routing.
     */
    reg_add_ip_mforward(ip_mforward, "ip_mforward");

    /*
     * Register to be notified when idb is deleted.
     */
    reg_add_swif_erase(pim_disable_idb, "pim_disable_idb");
    reg_add_swif_comingup(pim_idb_comingup, "pim_idb_comingup");
    reg_add_swif_goingdown(pim_idb_goingdown, "pim_idb_goingdown");

    /*
     * Register to be notified when the next-hop of a tunnel endpoint has
     * changed.
     */	
    reg_add_ip_adjust_tunnel_mroutes(ip_adjust_tunnel_mroutes, 
				     "ip_adjust_tunnel_mroutes");

    /*
     * Registry to setup multicast fastswitching based on unicast
     * fastswitching.
     */
    reg_add_ipmulticast_fs_setup_interface(ipmulticast_fs_setup_interface,
					    "ipmulticast_fs_setup_interface");
  
    /*
     * Register ip_update_mcache(). This is called by unicast process level
     * switching.
     */
    reg_add_ip_mcache_update(ip_mcache_update, "ip_mcache_update");

    /*
     * Register ip_invalidate_mcache(). This is called when an ATM VC gets
     * cleared and we need to clear the encapsulation stored in the mcache.
     */
    reg_add_ip_invalidate_mcache(ip_invalidate_mcache, "ip_invalidate_mcache");

    /*
     * Register the loopback check function.
     */
    reg_add_ipmulticast_loopback(ipmulticast_loopback, "ipmulticast_loopback");
 
    /*
     * Register the multicast helper map registry.
     */
    reg_add_ip_mhelper_forward(ipmulticast_helper_forward,
			       "ipmulticast_helper_forward");

    /*
     * Register the routine to be notified when an access-list is changed.
     */
    reg_add_ip_access_list(pim_access_list_callback,
			   "pim_access_list_callback");
    /*
     * Register the tech-support command.
     */
    reg_add_show_techsupport_protocol(TECHSUPPORT_IPMULTICAST,
				      ipmulticast_show_techsupport_commands,
				      "ipmulticast_show_techsupport_commands");
}

/*
 * ipmulticast_init
 *
 * Initialize the IP multicast handling.
 */
void ipmulticast_init (subsystype *subsys)
{

    idbtype *idb;
    int     i;

    /* 
     * Create some nice free lists. 
     */
    mempool_add_free_list(MEMPOOL_CLASS_LOCAL, sizeof(gdbtype));
    mempool_add_free_list(MEMPOOL_CLASS_LOCAL, sizeof(mdbtype));
    mempool_add_free_list(MEMPOOL_CLASS_LOCAL, sizeof(midbtype));

    ipmulticast_register_thyself();

    /*
     * Initialize multicast support protocols. Order is important here.
     * Do PIM first. This is so local group membership are flagged in mdb.
     */
    pim_init();
    igmp_init();

    /*
     * Initialize IP multicast specific interface variables to default values.
     */
    FOR_ALL_SWIDBS(idb) {
	ipmulticast_init_idb(idb, TRUE);
    }

    /*
     * Zero out IP multicast routing table.
     */
    for (i = 0; i < NETHASHLEN; i++) {
	ip_mroute_cache[i] = NULL;
    }

    /*
     * clear accounting array
     */
    memset(&igmp_traffic, 0, sizeof(igmp_traffic));
}
