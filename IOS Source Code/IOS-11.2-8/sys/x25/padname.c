/* $Id: padname.c,v 3.2.62.1 1996/03/18 22:49:45 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/x25/padname.c,v $
 *------------------------------------------------------------------
 * padname.c -- X.121 host and address lookup
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: padname.c,v $
 * Revision 3.2.62.1  1996/03/18  22:49:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  11:19:09  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:55:28  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:06:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:53:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:21:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "address.h"
#include "name.h"
#include "config.h"
#include <ciscolib.h>

/*
 * padname_show_hosts
 * Display PAD host information
 * Called by SERVICE_NAME_SHOW_HOSTS with NAM_PAD index
 */

static void padname_show_hosts (nametype *ptr, char *continue_text)
{
    int i;

    printf("  X.121");
	    
    /*
     * Print out the X.121 style address(es).
     */
    for (i = 0; i < MAX121ADDRESSES; i++) {
	if (ptr->v.x121.adr[i].length == 0)
	    break;
	if ((i != 0) && !(i & 1)) {
	    if (automore_quit()) {
		ptr->sharecount--;
		return;
	    }
	    printf(continue_text);
	}
	printf("  %s",ptr->v.x121.adr[i].addr.x121_address);
	if (ptr->v.x121.userdata)
	    printf("/%s", ptr->v.x121.userdata);
    }
}

/*
 * padname_match_number
 * Check if address matches one of addresses in this name cache entry
 * Returns TRUE if match succeeds
 * Called by SERVICE_NAME_MATCH_NUMBER, index is ADDR_X121
 */

static boolean padname_match_number (nametype *ptr, addrtype *address)
{
    int i;

    for (i = 0; i < MAX121ADDRESSES; i++) {
	if (!strcasecmp((char *)&ptr->v.x121.adr[i].addr.x121_address[0], 
		     (char *)&address->addr.x121_address[0])) {
	    return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * padname_init
 * Initialize pad name handling support
 */

void padname_init (void)
{
    reg_add_name_show_hosts(NAM_PAD, padname_show_hosts, "padname_show_hosts");
    reg_add_name_match_number(ADDR_X121,
		      padname_match_number, "padname_match_number");
}
