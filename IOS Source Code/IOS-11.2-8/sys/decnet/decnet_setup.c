/* $Id: decnet_setup.c,v 3.3.54.1 1996/08/07 08:57:49 snyder Exp $
 * $Source: /release/112/cvs/Xsys/decnet/decnet_setup.c,v $
 *------------------------------------------------------------------
 * DECnet setup commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: decnet_setup.c,v $
 * Revision 3.3.54.1  1996/08/07  08:57:49  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/28  03:58:27  widmer
 * CSCdi44175:  Add setup_printf to replace sprintf in setup code
 *
 * Revision 3.2  1995/11/17  08:59:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:16:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:23:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "../ui/setup.h"
#include "decnet_externs.h"

static const char decnet_proto_name[] = "DECnet";

void decnet_setup_global (setup_struct *sp)
{
    ulong dn_area, dn_node;
    boolean doareas;
    dnnet *net = NULL;

    if (system_loading || !router_enable) {
	sp->args[LINK_DECNET] = FALSE;
	return;
    }

    sp->args[LINK_DECNET] = (void *) system_protocol(sp, decnet_proto_name,
						     RUNNING(LINK_DECNET));
    if (sp->args[LINK_DECNET]) {
	net = dn_nets[0];
	dn_area = net ? net->my_dn_area : 1;
	dn_node = net ? net->my_dn_node : 1;
	dn_area = get_num_in_range("    Your area number", 10, 1, 63, dn_area);
	dn_node = get_num_in_range("    Your node number", 10, 1,1023,dn_node);
	doareas = net ? net->doareas : FALSE;
	doareas = yes_or_no("    Area (level 2) routing", doareas, TRUE);
	setup_printf(sp, " %d.%d\ndecnet node-type %s",
		     dn_area, dn_node, doareas? "area" : "routing-iv");
    }
}

void decnet_setup_end (setup_struct *sp)
{
    sp->args[LINK_DECNET] = NULL;
}

void decnet_setup_int (setup_struct *sp)
{
    ulong dummy;

    if (system_loading || !router_enable || !sp->args[LINK_DECNET]) {
	return;
    }

    if (interface_protocol(decnet_proto_name, (sp->idb->dn_cost > 0))) {
	dummy = (sp->idb->dn_cost > 0) ? sp->idb->dn_cost : 10;
	dummy = get_num_in_range("    DECnet cost", 10, 1, 63, dummy);
	setup_printf(sp, "\ndecnet cost %d", dummy);
	sp->args[LINK_MOP_CONSOLE] = (void *) TRUE;
    } else {
	if (sp->idb->dn_cost > 0) {
	    setup_printf(sp, "\nno decnet cost");
	}
    }
}

