/* $Id: apollo_setup.c,v 3.3.56.1 1996/08/07 09:05:02 snyder Exp $
 * $Source: /release/112/cvs/Xsys/xns/apollo_setup.c,v $
 *------------------------------------------------------------------
 * Apollo setup commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: apollo_setup.c,v $
 * Revision 3.3.56.1  1996/08/07  09:05:02  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/28  03:59:19  widmer
 * CSCdi44175:  Add setup_printf to replace sprintf in setup code
 *
 * Revision 3.2  1995/11/17  19:20:44  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:57:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:26:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "interface.h"
#include "sys_registry.h"
#include "packet.h"
#include "../ui/setup.h"
#include "../xns/xns.h"
#include "../xns/apollo.h"

static const char apollo_proto_name[] = "Apollo";

void apollo_setup_global (setup_struct *sp)
{
    ulong dummy;

    if (!system_loading && router_enable) {
	sp->args[LINK_APOLLO] = (void *) system_protocol(sp, apollo_proto_name,
							 RUNNING(LINK_APOLLO));
	if (sp->args[LINK_APOLLO]) {
	    dummy = apollo_address ? apollo_address : 1;
	    dummy = get_num_in_range("    Apollo routing number",
				     16, 1, 0xFFFFEL, dummy);
	    setup_printf(sp, " %x", dummy);
	}
    } else {
	sp->args[LINK_APOLLO] = FALSE;
    }
}

void apollo_setup_end (setup_struct *sp)
{
    sp->args[LINK_APOLLO] = NULL;
}

void apollo_setup_global2 (setup_struct *sp)
{
    idbtype *idb;

    if (sp->args[LINK_APOLLO]) {
	setup_printf(sp, "\n!\n! Turn off Apollo to prevent "
		     "network conflicts.");
	FOR_ALL_SWIDBS(idb) {
	    setup_printf(sp, "\ninterface %s\nno apollo network",
			 idb->namestring);
	}
    }
}

void apollo_setup_int (setup_struct *sp)
{
    ulong	dummy;
    idbtype	*idb = sp->idb;
    apollosb	*asb;

    if (system_loading || !router_enable || !sp->args[LINK_APOLLO]) {
	return;
    }

    asb = idb_get_swsb_inline(idb, SWIDB_SB_APOLLO);

    if (interface_protocol(apollo_proto_name,(asb && (asb->apollonet != 0)))) {
	dummy = (asb && asb->apollonet) ? asb->apollonet : sp->defnetwork;
	dummy = get_num_in_range("    Apollo network number",
				 16, 1, 0xFFFFFFFEL, dummy);
	setup_printf(sp, "\nApollo network %x", dummy);
    } else {
	if (asb && asb->apollonet) {
	    setup_printf(sp, "\nno apollo network");
	}
    }
}
