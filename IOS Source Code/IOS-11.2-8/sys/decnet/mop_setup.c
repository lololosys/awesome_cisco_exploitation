/* $Id: mop_setup.c,v 3.3 1995/11/28 03:58:29 widmer Exp $
 * $Source: /swtal/cherf.111/ios/sys/decnet/mop_setup.c,v $
 *------------------------------------------------------------------
 * MOP setup commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: mop_setup.c,v $
 * Revision 3.3  1995/11/28  03:58:29  widmer
 * CSCdi44175:  Add setup_printf to replace sprintf in setup code
 *
 * Revision 3.2  1995/11/17  08:59:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:17:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:23:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "../ui/setup.h"
#include "packet.h"
#include "../if/ether.h"
#include "../decnet/mop.h"

void mop_setup_int (setup_struct *sp)
{
    if (system_loading) {
	return;
    }

    if (sp->idb->hwptr->status & (IDB_ETHER | IDB_SERIAL)) {
	if (sp->args[LINK_MOP_CONSOLE]) {
	    setup_printf(sp, "\nmop enabled");
	} else {
	    setup_printf(sp, "\nno mop enabled");
	}
    }
}

