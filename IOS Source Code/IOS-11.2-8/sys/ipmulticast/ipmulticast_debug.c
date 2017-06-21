/* $Id: ipmulticast_debug.c,v 3.3.50.4 1996/09/13 22:52:43 lwei Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/ipmulticast_debug.c,v $
 *------------------------------------------------------------------
 * ipmulticast_debug.c - Debugging functions for IP multicast routing.
 *
 * November, 1993.
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ipmulticast_debug.c,v $
 * Revision 3.3.50.4  1996/09/13  22:52:43  lwei
 * CSCdi69140:  Fix per code review comments to CSCdi68465
 * Branch: California_branch
 *
 * Revision 3.3.50.3  1996/09/10  05:31:42  lwei
 * CSCdi68465:  RP information is too difficult to change in last-hop
 *              routers
 * Branch: California_branch
 *
 * Revision 3.3.50.2  1996/07/25  09:47:57  snyder
 * CSCdi63981:  eliminate unused code, fix bugs, make a common routine
 *              1. create a routine call generic_debug_all and
 *                 make most debugs use it
 *              2. eliminate if (*_debug_inited) return, most never set
 *              the var
 *                 to TRUE making the code useless.
 *              3. declare some constant arrays const
 *              4. fix bugs found along the way
 *              5. save 2768 bytes from image, 660 out of data section
 * Branch: California_branch
 *
 * Revision 3.3.50.1  1996/04/03  23:19:11  dino
 * CSCdi51598:  Problem of interoperability with switches
 * Branch: California_branch
 *
 * Revision 3.3  1995/12/03  18:06:28  dino
 * CSCdi44874:  no ip dvmrp debug detail turns on debugging.
 *
 * Revision 3.2  1995/11/17  17:29:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:01:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/09  23:14:17  asastry
 * CSCdi38504:  Limit DVMRP debug output using access lists for the
 * command 'debug ip dvmrp detail [in | out]'
 *
 * Revision 2.2  1995/07/18  07:32:41  asastry
 * CSCdi37082:  Enhance 'debug ip dvmrp detail' to allow '[in | out]'.
 *
 * Revision 2.1  1995/06/07  21:03:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "config.h"
#include "../ui/debug.h"
#include "../ipmulticast/ipmulticast_debug.h"

/* 
 * Declare the initialized text for the debugging array.
 */
#define __DECLARE_DEBUG_ARR__
#include "../ipmulticast/ipmulticast_debug_flags.h"

/*
 * IP group addresses to debug on.
 */
ipaddrtype mcache_group_debug;
ipaddrtype mroute_group_debug;
ipaddrtype pim_group_debug;

/* Variable used to restrict output associated with 'debug ip dvmrp detail' */ 
int ip_dvmrp_acl = 0;


/*
 * ipmulticast_debug_commands
 */
void ipmulticast_debug_commands (parseinfo *csb)
{

    boolean *ptr;
    int     flag;

    flag = csb->which;
    ptr = NULL;

    switch (flag) {
    case DEBUG_MCACHE:
	ptr = &mcache_debug;
	mcache_group_debug = GETOBJ(paddr,1)->ip_addr;
	break;
    case DEBUG_MROUTE:
	ptr = &mroute_debug;
	mroute_group_debug = GETOBJ(paddr,1)->ip_addr;
	break;
    case DEBUG_IGMP:
	ptr = &igmp_debug;
	break;
    case DEBUG_DVMRP:
	ptr = &dvmrp_debug;
	ip_dvmrp_acl = GETOBJ(int, 4);
	dvmrp_debug_detail_flag = 0;

	/*
	 * Support for 'debug ip dvmrp [detail [in | out] [acl]]
	 * 
	 * See if user specified 'in' or 'out' after the 'detail'
	 * keyword. If neither was specified, only GETOBJ(int,1)
	 * will be TRUE.
	 */
	if (GETOBJ(int,2)) {
	    dvmrp_debug_detail_flag = DVMRP_INCOMING_DETAIL_DEBUG;
	} else if (GETOBJ(int,3)) {
	    dvmrp_debug_detail_flag = DVMRP_OUTGOING_DETAIL_DEBUG;
	} else if (GETOBJ(int,1)) {
	    dvmrp_debug_detail_flag = DVMRP_DETAIL_DEBUG;
	} else if (GETOBJ(int,5)) {
	    dvmrp_debug_detail_flag = DVMRP_PRUNING_DEBUG;
	}
	break;
    case DEBUG_PIM:
	ptr = &pim_debug;
	pim_group_debug = GETOBJ(paddr,1)->ip_addr;
	break;
    case DEBUG_SD:
	ptr = &sd_debug;
	break;
    case DEBUG_CGMP:
	ptr = &cgmp_debug;
	break;
    case DEBUG_AUTORP:
	ptr = &pim_autorp_debug;
	break;
    default:
	break;
    }

    if (ptr) {
	*ptr = csb->sense;
	debug_show_flag(*ptr, TRUE, ipmulticast_debug_arr[flag].txt);
	if ((csb->which == DEBUG_DVMRP) && (ip_dvmrp_acl != 0))
	    printf(" for access list %d", ip_dvmrp_acl);
    }
}

/*
 * ipmulticast_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the IP multicast routing debug flags at once. The 
 * argument is TRUE for "debug all", FALSE for "undebug all".
 */
void ipmulticast_debug_all (boolean flag)
{

    if (!flag) {
	dvmrp_debug_detail_flag = 0;
	pim_group_debug = 0;
	pim_autorp_debug = FALSE;
	mroute_group_debug = 0;
	mcache_group_debug = 0;
    }

    generic_debug_all(ipmulticast_debug_arr, flag);
}

/*
 * ipmulticast_debug_show
 *
 * Called to display the values of all the IP multicast debugging variables.
 */
void ipmulticast_debug_show (void)
{

    if (!(igmp_debug | dvmrp_debug | pim_debug | mroute_debug |
	  mcache_debug | sd_debug | cgmp_debug | pim_autorp_debug)) return;

    printf("\nIP multicast:");
    if (mcache_debug) {
	printf("\n  IP multicast fast-switching debugging is on");
	if (mcache_group_debug) printf(" for %i", mcache_group_debug);
    }
    if (mroute_debug) {
	printf("\n  IP multicast routing debugging is on");
	if (mroute_group_debug) printf(" for %i", mroute_group_debug);
    }
    if (igmp_debug) printf("\n  IGMP debugging is on");
    if (cgmp_debug) printf("\n  CGMP debugging is on");
    if (dvmrp_debug) {
	printf("\n  DVMRP debugging");
	if (dvmrp_debug_detail_flag == DVMRP_INCOMING_DETAIL_DEBUG)
	    printf(" (detailed, incoming only) is on");
	else if (dvmrp_debug_detail_flag == DVMRP_OUTGOING_DETAIL_DEBUG)
	    printf(" (detailed, outgoing only) is on");
	else if (dvmrp_debug_detail_flag == DVMRP_DETAIL_DEBUG)
	    printf(" (detailed) is on");
	else if (dvmrp_debug_detail_flag == DVMRP_PRUNING_DEBUG)
	    printf(" (pruning) is on");
	else
	    printf(" is on");
	if (ip_dvmrp_acl != 0)
	    printf(" for access list %d", ip_dvmrp_acl);
    }
    if (pim_debug) {
	printf("\n  PIM debugging is on");
	if (pim_group_debug) printf(" for %i", pim_group_debug);
    }
    if (sd_debug) printf("\n  SD debugging is on");
    if (pim_autorp_debug) printf("\n  PIM Auto-RP debugging is on");
}

/*
 * ipmulticast_debug_init
 *
 * Initialize IP multicast debugging variables.
 */
void ipmulticast_debug_init (void)
{

    reg_add_debug_all(ipmulticast_debug_all, "ipmulticast_debug_all");
    reg_add_debug_show(ipmulticast_debug_show, "ipmulticast_debug_show");

    ipmulticast_debug_all(debug_all_p());
}

