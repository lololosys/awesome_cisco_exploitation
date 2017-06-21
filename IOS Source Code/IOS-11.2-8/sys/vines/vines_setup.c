/* $Id: vines_setup.c,v 3.3.56.1 1996/08/07 09:03:59 snyder Exp $
 * $Source: /release/112/cvs/Xsys/vines/vines_setup.c,v $
 *------------------------------------------------------------------
 * Vines setup commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: vines_setup.c,v $
 * Revision 3.3.56.1  1996/08/07  09:03:59  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/28  03:59:15  widmer
 * CSCdi44175:  Add setup_printf to replace sprintf in setup code
 *
 * Revision 3.2  1995/11/17  17:58:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:45:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:15:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "../ui/setup.h"
#include "packet.h"
#include "../vines/vines_private.h"

static const char vines_proto_name[] = "Vines";

void vines_setup_global (setup_struct *sp)
{
    if (!system_loading && router_enable) {
	sp->args[LINK_VINES] =
	    (void *) system_protocol(sp, vines_proto_name,
				     RUNNING(LINK_VINES));
    } else {
	sp->args[LINK_VINES] = FALSE;
    }
}

void vines_setup_end (setup_struct *sp)
{
    sp->args[LINK_VINES] = NULL;
}

void vines_setup_int (setup_struct *sp)
{
    boolean now;

    if (system_loading || !router_enable || !sp->args[LINK_VINES]) {
	return;
    }

    now = reg_invoke_proto_on_swidb(LINK_VINES, sp->idb);
    if (interface_protocol(vines_proto_name, now)) {
	setup_printf(sp, "\nvines metric");
    } else {
	setup_printf(sp, "\nno vines metric");
    }
}

