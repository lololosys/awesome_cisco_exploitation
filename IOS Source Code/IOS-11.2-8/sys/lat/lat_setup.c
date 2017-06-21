/* $Id: lat_setup.c,v 3.3 1995/11/28 03:58:45 widmer Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/lat_setup.c,v $
 *------------------------------------------------------------------
 * LAT setup commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lat_setup.c,v $
 * Revision 3.3  1995/11/28  03:58:45  widmer
 * CSCdi44175:  Add setup_printf to replace sprintf in setup code
 *
 * Revision 3.2  1995/11/17  17:33:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:25:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "../ui/setup.h"
#include "../lat/mtcsys.h"
#include "../lat/latfnc.h"
#include "../lat/lat.h"

void lat_setup_global (setup_struct *sp)
{
    if (!system_loading && router_enable && lat_enable) {
        sp->args[LINK_LAT] = (void *) yes_or_no("  Configure LAT",
						lat_interfaces, TRUE);
    } else {
        sp->args[LINK_LAT] = FALSE;
    }
}

void lat_setup_end (setup_struct *sp)
{
    sp->args[LINK_LAT] = NULL;
}

void lat_setup_int (setup_struct *sp)
{
    if (system_loading || !sp->args[LINK_LAT]) {
	return;
    }

    if (interface_protocol("LAT", sp->idb->lat_running)) {
	setup_printf(sp, "\nlat enabled");
	sp->args[LINK_MOP_CONSOLE] = (void *) TRUE;
    } else {
	setup_printf(sp, "\nno lat enabled");
    }
}
