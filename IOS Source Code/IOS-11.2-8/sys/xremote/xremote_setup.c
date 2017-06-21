/* $Id: xremote_setup.c,v 3.4.48.1 1996/03/18 22:56:50 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/xremote/xremote_setup.c,v $
 *------------------------------------------------------------------
 * XRemote setup commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: xremote_setup.c,v $
 * Revision 3.4.48.1  1996/03/18  22:56:50  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.16.2  1996/03/07  11:23:29  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.16.1  1996/02/20  22:02:46  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/12/10  21:02:52  smackie
 * Next on a very special Geraldo. Unused global booleans and the files
 * that loved them. (CSCdi45340)
 *
 * Revision 3.3  1995/11/28  03:59:26  widmer
 * CSCdi44175:  Add setup_printf to replace sprintf in setup code
 *
 * Revision 3.2  1995/11/17  19:27:12  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:01:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:35:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "ttysrv.h"
#include "packet.h"
#include "../ip/ip.h"
#include "../ui/setup.h"
#include "../xremote/xremote.h"
#include "registry.h"
#include "../ip/ip_registry.h"

/*
 * setup_xremote - Setup for Xremote support
 */
void xremote_setup_global (setup_struct *sp)
{
    char buffer[MAXBUF];
    char *cp;
    int status;
    ipaddrtype address;

    if (router_enable) {
	return;
    }

    (void) sprintf(buffer, "    Configure XRemote font servers");
    if (!yes_or_no(buffer, FALSE, TRUE)) {
	return;
    }

    while (!(stdio->statbits & CONTROLC)) {
	printf("      Enter a font server IP address or press RETURN to exit: ");
	if (!rdtty(buffer, MAXBUF)) {
	    break;
	}

	if (null(buffer)) {
	    break;
	}

	status = parse_ip_address(buffer, &address);
	if (!status || !address ||
	    !reg_invoke_ip_good_address(address, 0L, FALSE)) {
	    cp = (char *) deblank(buffer);
	    if (*cp != '?') {
		printf("\n%% Bad Internet address.");
	    }
	    printf(setup_iphelp);
	    continue;
	}
	setup_printf(sp, "\nxremote tftp host %i", address);
    }
    return;
}
