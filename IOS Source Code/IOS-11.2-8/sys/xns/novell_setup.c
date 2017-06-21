/* $Id: novell_setup.c,v 3.3.56.1 1996/08/07 09:05:23 snyder Exp $
 * $Source: /release/112/cvs/Xsys/xns/novell_setup.c,v $
 *------------------------------------------------------------------
 * Novell setup commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: novell_setup.c,v $
 * Revision 3.3.56.1  1996/08/07  09:05:23  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/28  03:59:21  widmer
 * CSCdi44175:  Add setup_printf to replace sprintf in setup code
 *
 * Revision 3.2  1995/11/17  19:23:14  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:59:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:28:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "packet.h"
#include "../ui/setup.h"
#include "../xns/xns.h"
#include "../xns/novell.h"

static const char novell_proto_name[] = "IPX";

void novell_setup_global (setup_struct *sp)
{
    if (!system_loading && router_enable) {
	sp->args[LINK_NOVELL] =
	    (void *) system_protocol(sp, novell_proto_name,
				     RUNNING(LINK_NOVELL));
    } else {
	sp->args[LINK_NOVELL] = FALSE;
    }
}

void novell_setup_end (setup_struct *sp)
{
    sp->args[LINK_NOVELL] = NULL;
}

void novell_setup_global2 (setup_struct *sp)
{
    idbtype *idb;

    if (sp->args[LINK_NOVELL]) {
	setup_printf(sp, "\n!\n! Turn off IPX to prevent network conflicts.");
	FOR_ALL_SWIDBS(idb) {
	    setup_printf(sp, "\ninterface %s\nno ipx network",
			 idb->namestring);
	}
    }
}

void novell_setup_int (setup_struct *sp)
{
    ulong dummy;
    nidbtype *nidb;

    if (system_loading || !router_enable || !sp->args[LINK_NOVELL]) {
	return;
    }

    nidb = IPX_PRIMARY_NIDB(sp->idb);

    if (interface_protocol(novell_proto_name, (sp->idb->novellnets))) {
	dummy = (nidb && nidb->novellnet) ? nidb->novellnet : sp->defnetwork;
	dummy = get_num_in_range("    IPX network number",
				 16, 1, 0xFFFFFFFEL, dummy);
	setup_printf(sp, "\nipx network %x", dummy);
    } else {
	if (sp->idb->novellnets) {
	    setup_printf(sp, "\nno ipx network");
	}
    }
}

/*
 * Set up novell nets on interfaces that don't exist yet.
 */
void novell_setup_dynamic_int (setup_struct *sp)
{
    nidbtype *nidb;
    ulong dummy = sp->defnetwork;

    if ((int)sp->args[LINK_NOVELL] > 1) {
	/*
	 * Check for net address of existing interface.
	 */
	if (sp->idb) {
	    nidb = IPX_PRIMARY_NIDB(sp->idb);
	     if (nidb && nidb->novellnet) {
		 dummy = nidb->novellnet;
	     }
	}

	dummy = get_num_in_range("    IPX network number",
				 16, 1, 0xFFFFFFFEL, dummy);
	setup_printf(sp, "\nipx network %x", dummy);
	sp->defnetwork = dummy+1;
    }
}

/*
 * Ask about setting up IPX on async lines during async configuration.
 */

void novell_setup_alllines (setup_struct *sp)
{
    if (sp->args[LINK_NOVELL]) {
	if (yes_or_no("    Configure for Async IPX", TRUE,TRUE)) {
	    sp->args[LINK_NOVELL] = (void *) (++(int)sp->args[LINK_NOVELL]);
	}
    }
}
