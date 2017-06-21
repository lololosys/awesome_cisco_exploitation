/* $Id: snapshot_media.c,v 3.2.60.1 1996/03/18 22:48:54 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/wan/snapshot_media.c,v $
 *------------------------------------------------------------------
 * snapshot_media.c -- media dependent Snapshot routines.
 *
 * June 1994, Joel P. Bion
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: snapshot_media.c,v $
 * Revision 3.2.60.1  1996/03/18  22:48:54  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/07  11:08:32  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.24.1  1996/02/20  21:53:44  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:05:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:51:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:20:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"

/*
 * snapshot_build_address
 *
 * Build an addrtype data structure containing the Snapshot address
 * currently in use on this interface. 0 is okay -- this routine
 * is only used to support kicking the dialer to start up.
 */
void snapshot_build_address (
    idbtype *swidb,
    paktype *pak,
    addrtype *addr)
{
    memset(addr, 0, sizeof(addrtype));
    addr->type = ADDR_SNAPSHOT;
    addr->length = ADDRLEN_SNAPSHOT;
    addr->snapshot_addr = 0;
}

/*
 * snapshot_sprintf_address
 *
 * Take an addrtype data structure and print it as an Snapshot address.
 */
static void snapshot_sprintf_address (
    char *string,
    addrtype *addr)
{
    sprintf(string, "%d", addr->snapshot_addr);
}

/*
 * snapshot_x25_macaddr
 *
 * Just return FALSE, since snapshot packets don't go anywhere.
 * Called by MEDIA_X25_MACADDR, indexed by LINK_SNAPSHOT.
 */
static boolean snapshot_x25_macaddr (paktype *pak, long address,
				     addrtype *proto_addr, lcitype **lci,
				     boolean *broadcast)
{
    return(FALSE);
}

/*
 * snapshot_frame_macaddr
 *
 * Just return FALSE, since snapshot packets don't go anywhere.
 * Called by MEDIA_FRAME_MACADDR, indexed by LINK_SNAPSHOT.
 */
static boolean snapshot_frame_macaddr (paktype *pak, long address,
				       addrtype *proto_addr, boolean *broadcast)
{
    return(FALSE);
}

/*
 * snapshot_ddr_macaddr
 *
 * Turn a Snapshot address into a broadcast flag, or a filled in addrtype
 * structure that will be used to look up a static mapping.
 * Called by MEDIA_DDR_MACADDR, indexed by LINK_SNAPSHOT.
 */
static boolean snapshot_ddr_macaddr (paktype *pak, long address,
				     addrtype *proto_addr, boolean *broadcast)
{
    /*
     * Need to allow the address structure to be filled in for both
     * broadcast and non-broadcast packets.  This is so older dialer
     * maps that mapped the broadcast address still work.
     */
    proto_addr->type = ADDR_SNAPSHOT;
    proto_addr->length = ADDRLEN_SNAPSHOT;
    proto_addr->snapshot_addr = address;
    *broadcast = FALSE;
    pak->dialer_save_type = LINK_SNAPSHOT;
    return(TRUE);
}

/*
 * snapshot_media_registry
 *
 * Register media dependent functions for Snapshot.
 */
void snapshot_media_registry (void)
{
    reg_add_proto_address(LINK_SNAPSHOT,
			  snapshot_build_address, "snapshot_build_address");
    reg_add_sprintf_address_link(LINK_SNAPSHOT,
				 snapshot_sprintf_address,
				 "snapshot_sprintf_address");
    reg_add_sprintf_address_addr(ADDR_SNAPSHOT,
				 snapshot_sprintf_address,
				 "snapshot_sprintf_address");
    reg_add_media_x25_macaddr(LINK_SNAPSHOT, snapshot_x25_macaddr,
			      "snapshot_x25_macaddr");
    reg_add_media_frame_macaddr(LINK_SNAPSHOT, snapshot_frame_macaddr,
				"snapshot_frame_macaddr");
    reg_add_media_ddr_macaddr(LINK_SNAPSHOT, snapshot_ddr_macaddr,
			      "snapshot_ddr_macaddr");
}
