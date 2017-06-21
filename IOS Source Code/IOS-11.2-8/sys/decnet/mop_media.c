/* $Id: mop_media.c,v 3.4 1996/02/29 02:07:37 asastry Exp $
 * $Source: /release/111/cvs/Xsys/decnet/mop_media.c,v $
 *------------------------------------------------------------------
 * mop_media.c - media registry initialization for MOP
 *
 * October 1993, Scott Mackie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: mop_media.c,v $
 * Revision 3.4  1996/02/29  02:07:37  asastry
 * CSCdi49406:  Port fix (for MOP over Frame-Relay) to 10.0 and later
 * releases.
 *
 * Revision 3.3  1995/11/17  08:59:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:15:07  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:17:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:23:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "../if/ether.h"
#include "../wan/serial.h"
#include "../if/rif.h"


/*
 * mop_ether_macaddr
 * 
 * Turn a MOP address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR, indexed by LINK_MOP_BOOT or
 * LINK_MOP_CONSOLE
 */
static boolean mop_ether_macaddr (paktype *pak, long address,
				  int *enctype, uchar **daddr,
				  uchar const **other_info)
{
    *daddr = (uchar *)address;
    *enctype = ET_ARPA;
    return(TRUE);
}

/*
 * mop_fddi_macaddr
 * 
 * Turn a MOP address into an encapsulation type and a MAC address
 * Called by MEDIA_FDDI_MACADDR, indexed by LINK_MOP_BOOT or
 * LINK_MOP_CONSOLE
 */
static boolean mop_fddi_macaddr (paktype *pak, long address,
				 int *enctype, uchar **daddr,
				 uchar const **other_info,
				 ulong *rif_flags)
{
    *daddr = (uchar *) address;
    *enctype = ET_SNAP;
    *other_info = zerosnap;
    *rif_flags = RIF_NOT_PRESENT;
    return(TRUE);
}

/*
 * mop_hdlc_macaddr
 *
 * Turn an MOP address into an encapsulation type and a MAC address
 * Called by MEDIA_HDLC_MACADDR, indexed by LINK_MOP_CONSOLE
 * and LINK_MOP_BOOT
 */
static boolean mop_hdlc_macaddr (paktype *pak, long address,
				 int *enctype, ushort *flags)
{
    if (ether_vencap(pak, address)) {
	pak->datagramsize += HDLC_ENCAPBYTES;
	pak->datagramstart -= HDLC_ENCAPBYTES;
	*((long *)pak->datagramstart) = HDLC_BRIDGECODE;
	pak->enctype = ET_BRIDGE;	/* Set bridge encapsulation */
	*enctype = ET_NULL;
	return(TRUE);
    } else {
	return(FALSE);
    }
    if ((address == 0) || (address == -1))
	*flags |= HDLC_BCAST;
    return(TRUE);
}

/*
 * mop_smds_macaddr
 *
 * Turn an MOP address into an encapsulation type and a MAC address
 * Called by MEDIA_SMDS_MACADDR, indexed by LINK_MOP_CONSOLE and
 * LINK_MOP_BOOT.
 */
static boolean mop_smds_macaddr (paktype *pak, long address, int *enctype,
				 uchar const **daddr, uchar const **other_info,
				 addrtype *proto_addr)
{
    if (!RUNNING_IF(LINK_BRIDGE,pak->if_output))
	return(FALSE);

    if (!(*pak->if_output->hwptr->bridge_vencap)(pak, address))
	return(FALSE);

    pak->linktype = LINK_BRIDGE;
    *enctype = ET_SNAP;
    *other_info = zerosnap;
    return(TRUE);
}

/*
 *
 * Turn a MOP address into a broadcast flag.
 * Called by MEDIA_FRAME_MACADDR, indexed by LINK_MOP_BOOT, or
 * LINK_MOP_CONSOLE.
 */
static boolean mop_frame_macaddr (paktype *pak, long address,
				  addrtype *proto_addr, boolean *broadcast)
{
    if (!RUNNING_IF(LINK_BRIDGE,pak->if_output))
	return(FALSE);

    if (!ether_vencap(pak, address))
	return(FALSE);

    pak->linktype = LINK_BRIDGE;
    pak->enctype = ET_BRIDGE;
    *broadcast = TRUE;

    return(TRUE);
}

/*
 * mop_media_registry
 * Register media dependent code for MOP
 */

void mop_media_registry (void)
{
    reg_add_media_ether_macaddr(LINK_MOP_CONSOLE, mop_ether_macaddr,
				"mop_ether_macaddr");
    reg_add_media_ether_macaddr(LINK_MOP_BOOT, mop_ether_macaddr,
				"mop_ether_macaddr");

    reg_add_media_fddi_macaddr(LINK_MOP_CONSOLE, mop_fddi_macaddr,
			       "mop_fddi_macaddr");
    reg_add_media_fddi_macaddr(LINK_MOP_BOOT, mop_fddi_macaddr,
			       "mop_fddi_macaddr");

    reg_add_media_hdlc_macaddr(LINK_MOP_CONSOLE, mop_hdlc_macaddr,
			       "mop_hdlc_macaddr");
    reg_add_media_hdlc_macaddr(LINK_MOP_BOOT, mop_hdlc_macaddr,
			       "mop_hdlc_macaddr");

    reg_add_media_smds_macaddr(LINK_MOP_CONSOLE, mop_smds_macaddr,
			       "mop_smds_macaddr");
    reg_add_media_smds_macaddr(LINK_MOP_BOOT, mop_smds_macaddr,
			       "mop_smds_macaddr");

    reg_add_media_frame_macaddr(LINK_MOP_CONSOLE, mop_frame_macaddr,
			       "mop_frame_macaddr");
    reg_add_media_frame_macaddr(LINK_MOP_BOOT, mop_frame_macaddr,
			       "mop_frame_macaddr");
}

