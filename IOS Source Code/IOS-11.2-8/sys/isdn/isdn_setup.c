/* $Id: isdn_setup.c,v 3.3 1995/11/28 03:58:41 widmer Exp $
 * $Source: /swtal/cherf.111/ios/sys/isdn/isdn_setup.c,v $
 *------------------------------------------------------------------
 * ISDN setup commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: isdn_setup.c,v $
 * Revision 3.3  1995/11/28  03:58:41  widmer
 * CSCdi44175:  Add setup_printf to replace sprintf in setup code
 *
 * Revision 3.2  1995/11/17  17:40:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:13:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:15:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "ttysrv.h"
#include "../ui/setup.h"
#include "../les/isdn_bri.h"

void isdn_setup_global2 (setup_struct *sp)
{
    hwidbtype *hwidb;
    char *cp, buffer[MAXBUF];

    if (system_loading || !router_enable) {
	return;
    }

    /*
     * Set-up ISDN configuration
     */
    FOR_ALL_HWIDBS(hwidb) {
        if (stdio->statbits & CONTROLC) {
            break;
        }
        if (hwidb->type == IDBTYPE_BRI) {
            cp = setup_string(buffer, "  Enter ISDN BRI Switch Type",
                              print_isdn_switch(isdn_getswitchtype()));
	    setup_printf(sp, "\nisdn switch-type %s ", cp);
            break;
        }
    }
}

