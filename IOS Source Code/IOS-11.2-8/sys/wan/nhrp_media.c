/* $Id: nhrp_media.c,v 3.3.50.2 1996/08/28 13:22:14 thille Exp $
 * $Source: /release/112/cvs/Xsys/wan/nhrp_media.c,v $
 *------------------------------------------------------------------
 * Next Hop Resolution Protocol.
 *
 * October 1995, Bruce Cole
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nhrp_media.c,v $
 * Revision 3.3.50.2  1996/08/28  13:22:14  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.50.1  1996/04/10  04:23:07  bcole
 * CSCdi54192:  IPX pings over a multipoint GRE tunnel fail intermittently
 * Branch: California_branch
 *
 * Fixup IPX encapsulation over multipoint tunnels.
 *
 * Revision 3.3  1995/12/08  02:01:54  bcole
 * CSCdi45234:  NHRP: Dont refresh unused cache entries
 *
 * Keep track of which cache entries are used, and avoid refreshing unused
 * entries.  Fix holddown time for implicit cache entries.
 *
 * Use IANA defined SNAP OUI for NHRP.
 *
 * Revision 3.2  1995/11/17  18:03:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:50:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:36:27  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  21:55:01  bcole
 * Placeholder files for 11.1 NHRP work.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "address.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/static_map.h"
#include "../wan/serial.h"
#include "nhrp_public.h"
#include "../ui/debug.h"
#include "nhrp_debug.h"
#include "../util/radix.h"
#include "nhrp.h"
#include "smds.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"


/*
 * nhrp_ether_macaddr
 * 
 * Turn a NHRP address into an encapsulation type and a MAC address.
 * Called by MEDIA_ETHER_MACADDR, indexed by LINK_NHRP.
 */
static boolean nhrp_ether_macaddr (paktype *pak, long address,
				  int *enctype, uchar **daddr,
				  uchar const **other_info)
{
    hwaddrtype *hw;
    
    hw = (hwaddrtype *)address;
    if (!reg_invoke_media_ether_macaddr(hw->type, pak, *(long *)hw->addr,
					enctype, daddr, other_info)) {
	if (!reg_used_media_ether_macaddr(hw->type))
	    errmsg(&msgsym(BADMACREG, LINK), pak->if_output->namestring,
		   hw->type);
	return(FALSE);
    }
    if (nhrp_debug) {
	buginf("\nNHRP: Encapsulation succeeded.  MAC addr %e.",
	       *daddr);
    }
    *enctype = ET_SNAP;
    *other_info = iana_snap;
    return(TRUE);
}

/*
 * nhrp_smds_macaddr
 * 
 * Turn a NHRP address into an encapsulation type and a MAC address
 * Called by MEDIA_SMDS_MACADDR, indexed by LINK_NHRP.
 */
static boolean nhrp_smds_macaddr (paktype *pak, long address, int *enctype,
				 uchar const **daddr,
				 uchar const **other_info,
				 addrtype *proto_addr)
{
    hwaddrtype *hw;
    smdsmaptype *smdsmap;
    
    hw = (hwaddrtype *)address;
    if (!reg_invoke_media_smds_macaddr(hw->type, pak, *(long *)hw->addr,
				       enctype,
				       daddr, other_info, proto_addr)) {
	if (!reg_used_media_smds_macaddr(hw->type))
	    errmsg(&msgsym(BADMACREG, LINK), pak->if_output->namestring,
		   hw->type);
	return(FALSE);
    }
    *enctype = ET_SNAP;
    *other_info = iana_snap;
    /*
     * Need to do the SMDS static mapping lookup here, as the encapsulation
     * routine does not know what our underlying linktype is.  The
     * encapsulation routine would expect a static mapping for the NHRP link
     * type, which certainly won't exist.
     */
    if (*daddr != baddr) {
	smdsmap = NULL;
	reg_invoke_media_map_lookup(ET_SMDS, proto_addr, hw->type,
				    (void *)&smdsmap);
        if (smdsmap) {
	    *daddr = smdsmap->hardware;
	}
    }
    return(TRUE);
}

/*
 * nhrp_atm_macaddr
 *
 * Turn a NHRP address into a broadcast flag.
 *
 * Called by MEDIA_ATM_MACADDR, indexed by LINK_NHRP.
 */
static boolean nhrp_atm_macaddr (paktype *pak, long address, hwaddrtype *daddr, 
				uchar const **snap_info, staticmap_type **map, 
				boolean *broadcast)
{
    hwaddrtype *hw;
    uchar nbma_buf[NHRP_NBMA_MAXSTR];
    
    hw = (hwaddrtype *)address;
    if (!reg_invoke_media_atm_macaddr(hw->type, pak, *(long *)hw->addr, daddr,
				      snap_info, map, broadcast)) {
	return(FALSE);
    }
    if (nhrp_debug) {
	if (*map)
	    nhrp_printnsap((*map)->hwconf.addr, nbma_buf);
	else
	    nbma_buf[0] = '\0';
	buginf("\nNHRP: Encapsulation succeeded.  NSAP addr %s", nbma_buf);
    }
    *snap_info = iana_snap;
    return(TRUE);
}

/*
 * nhrp_tunnel_macaddr
 * For multi-point GRE tunnels, determine IP address to use in the
 * underlying payload network.
 */

static boolean nhrp_tunnel_macaddr (paktype *pak, long hwaddress,
				    addrtype *proto_addr, boolean *broadcast)
{
    hwaddrtype *hw;

    hw = (hwaddrtype *)hwaddress;
    if (!reg_invoke_media_tunnel_macaddr(hw->type, pak, *(long *)hw->addr,
					 proto_addr, broadcast)) {
	return(FALSE);
    }
    if (nhrp_debug) {
	buginf("\nNHRP: Encapsulation succeeded.  Tunnel IP addr %i",
	       proto_addr->ip_addr);
    }
    return(TRUE);
}

/*
 * nhrp_media_registry
 * Register media dependent code for NHRP
 */

void nhrp_media_registry (void)
{
    reg_add_media_macaddr_map(nhrp_macaddr_map, "nhrp_macaddr_map");

    reg_add_media_ether_macaddr(LINK_NHRP, nhrp_ether_macaddr,
				"nhrp_ether_macaddr");
    reg_add_media_smds_macaddr(LINK_NHRP, nhrp_smds_macaddr,
			        "nhrp_smds_macaddr");
    reg_add_media_atm_macaddr(LINK_NHRP, nhrp_atm_macaddr,
				"nhrp_atm_macaddr");
    reg_add_media_tunnel_macaddr(LINK_NHRP, nhrp_tunnel_macaddr,
				 "nhrp_tunnel_macaddr");
    reg_add_media_link_to_type(LINK_NHRP, TYPE_NHRP, "nhrp type");
    reg_add_media_type_to_link(TYPE_NHRP, LINK_NHRP, "nhrp link");
    reg_add_media_serial_to_link(TYPE_NHRP, LINK_NHRP, "nhrp link");
}

