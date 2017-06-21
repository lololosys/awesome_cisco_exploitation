/* $Id: xns_setup.c,v 3.3.56.1 1996/08/07 09:05:40 snyder Exp $
 * $Source: /release/112/cvs/Xsys/xns/xns_setup.c,v $
 *------------------------------------------------------------------
 * XNS setup commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: xns_setup.c,v $
 * Revision 3.3.56.1  1996/08/07  09:05:40  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/28  03:59:23  widmer
 * CSCdi44175:  Add setup_printf to replace sprintf in setup code
 *
 * Revision 3.2  1995/11/17  19:25:35  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:00:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:30:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "../ui/setup.h"
#include "packet.h"
#include "../xns/xns.h"

static const char xns_proto_name[] = "XNS";

void xns_setup_global (setup_struct *sp)
{
    if (!system_loading && router_enable) {
	sp->args[LINK_XNS] =
	    (void *) system_protocol(sp, xns_proto_name,
				     RUNNING(LINK_XNS));
    } else {
	sp->args[LINK_XNS] = FALSE;
    }
}

void xns_setup_end (setup_struct *sp)
{
    sp->args[LINK_XNS] = NULL;
}

void xns_setup_global2 (setup_struct *sp)
{
    idbtype *idb;

    if (sp->args[LINK_XNS]) {
	setup_printf(sp, "\n!\n! Turn off XNS to prevent network conflicts.");
        FOR_ALL_SWIDBS(idb) {
	    setup_printf(sp, "\ninterface %s\nno xns network",
			 idb->namestring);
        }
    }
}

void xns_setup_int (setup_struct *sp)
{
    ulong	dummy;
    xnssb	*xsb;

    if (system_loading || !router_enable || !sp->args[LINK_XNS]) {
	return;
    }

    xsb = idb_get_swsb_inline(sp->idb, SWIDB_SB_XNS);
    if (interface_protocol(xns_proto_name, (xsb && xsb->xnsnet != 0))) {
	dummy = (xsb && xsb->xnsnet) ? xsb->xnsnet : sp->defnetwork;
	dummy = get_num_in_range("    XNS network number",
				 10, 1, 0xFFFFFFFEL, dummy);
	setup_printf(sp, "\nxns network %d", dummy);
    } else {
	if (xsb && xsb->xnsnet) {
	    setup_printf(sp, "\nno xns network");
	}
    }
}
