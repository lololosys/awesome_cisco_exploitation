/* $Id: channel.c,v 3.3.60.2 1996/04/25 23:13:16 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/if/channel.c,v $
 *------------------------------------------------------------------
 * Generic IBM channel routines
 *
 * April 1994, William H. Palmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Routines to handle static maps for address/vc translations
 *------------------------------------------------------------------
 * $Log: channel.c,v $
 * Revision 3.3.60.2  1996/04/25  23:13:16  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.3.60.1.2.1  1996/03/24  03:55:32  rzagst
 * ATM modularity: Remove AIP from all files that are not referring to the
 * AIP i/f
 * Branch: ATM_Cal_branch
 *
 * Revision 3.3.60.1  1996/03/18  20:13:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  09:43:41  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  14:22:40  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  09:26:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:37:21  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:48:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:50:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "../if/static_map.h"
#include "subsys.h"

static maplist_type *channel_maplist;

/*
 * channel_staticmap_add
 * Add an entry to static map table for address/vc translation
 */
#define CHANNEL_VC 1
static void channel_staticmap_add (maplist_type *list, hwaddrtype *hwconf,
				   addrtype *addr, uint protocol, char *class,
				   void *params)
{
    if (!staticmap_enter(list, addr, hwconf, protocol, ET_CHANNEL,
			 MAP_STATIC, class, params))
	printf("\n%% Can't add map to list");
}

void channel_ipaddr_insert (ushort vc, addrtype *addr)
{
    hwaddrtype hwconf;

    memset(&hwconf, 0, sizeof(hwconf));
    hwconf.type = CHANNEL_VC;
    hwconf.length = STATIONLEN_ATMVC;
    hwconf.target_type = TARGET_UNICAST;
    bcopy(&vc, hwconf.addr, STATIONLEN_ATMVC);
    channel_staticmap_add(channel_maplist, &hwconf, addr, LINK_IP, " channel",
			  NULL);
}

void channel_ipaddr_delete (ipaddrtype ipaddr)
{
    staticmap_type *map_ptr;
    addrtype proto_addr;

    memset(&proto_addr, 0, sizeof(addrtype));
    proto_addr.type = ADDR_IP;
    proto_addr.length = ADDRLEN_IP;
    proto_addr.ip_addr = ipaddr;

    map_ptr = staticmap_lookup_list(channel_maplist, &proto_addr, LINK_IP,
				    ET_CHANNEL);
    if (!map_ptr)
	return;
    staticmap_delete(channel_maplist, map_ptr);
}

static void channel_map_init (void)
{
    channel_maplist = maplist_create(" channel", FALSE);
    p_enqueue(maplistQ, channel_maplist);
}


static void channel_init (subsystype *subsys)
{
    channel_map_init();
}

/*
 * Channel subsystem header
 */

#define CHANNEL_MAJVERSION   1
#define CHANNEL_MINVERSION   0
#define CHANNEL_EDITVERSION  1

SUBSYS_HEADER(channel, CHANNEL_MAJVERSION, CHANNEL_MINVERSION,
	      CHANNEL_EDITVERSION,
	      channel_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      NULL);
