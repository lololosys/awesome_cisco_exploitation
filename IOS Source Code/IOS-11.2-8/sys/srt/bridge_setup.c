/* $Id: bridge_setup.c,v 3.3.56.1 1996/05/17 12:12:30 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/srt/bridge_setup.c,v $
 *------------------------------------------------------------------
 * Bridging setup commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: bridge_setup.c,v $
 * Revision 3.3.56.1  1996/05/17  12:12:30  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.42.1  1996/02/28  21:24:45  ppearce
 * CSCdi50230:  IBU modularity - Remove unnecessary header file includes
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3  1995/11/28  03:59:01  widmer
 * CSCdi44175:  Add setup_printf to replace sprintf in setup code
 *
 * Revision 3.2  1995/11/17  18:52:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:00:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "ieee.h"
#include "../ui/setup.h"
#include "../srt/span.h"

void bridge_setup_global (setup_struct *sp)
{
    if (!system_loading && router_enable && bridge_enable) {
	sp->args[LINK_BRIDGE] = (void *) yes_or_no("  Configure bridging",
						   (boolean)spanarray[1],
						   TRUE);
	if (sp->args[LINK_BRIDGE]) {
	    setup_printf(sp, "\nbridge 1 protocol dec");
	} else {
	    setup_printf(sp, "\nno bridge 1");
	}
    } else {
	sp->args[LINK_BRIDGE] = FALSE;
    }
}

void bridge_setup_end (setup_struct *sp)
{
    sp->args[LINK_BRIDGE] = NULL;
}

void bridge_setup_int (setup_struct *sp)
{
    if (system_loading || !router_enable || !sp->args[LINK_BRIDGE]) {
	return;
    }

    if (interface_protocol("bridging", (sp->idb->span_index != 0))) {
	setup_printf(sp, "\nbridge-group 1");
    } else {
	if (sp->idb->tbridge_on_swidb) {
	    setup_printf(sp, "\nno bridge-group 1");
	}
    }
}
