/* $Id: at_smrp.c,v 3.3.62.5 1996/08/28 13:09:19 thille Exp $
 * $Source: /release/112/cvs/Xsys/smrp/at_smrp.c,v $
 *------------------------------------------------------------------
 * Support for SMRP over AppleTalk.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_smrp.c,v $
 * Revision 3.3.62.5  1996/08/28  13:09:19  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.62.4  1996/07/23  13:28:09  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.3.62.3  1996/05/02  08:44:29  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.3.62.2  1996/03/23  01:33:46  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.3.62.1  1996/03/18  21:47:38  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/13  01:59:19  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.1  1996/02/20  17:10:25  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  18:52:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:23:18  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:12:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:24:50  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/10/12  18:43:08  kleung
 * CSCdi42084:  Some SMRP error messages do not follow convention.
 *
 * Revision 2.4  1995/08/01  00:22:30  slin
 * CSCdi32910: SMRP netrange not updated when AppleTalk cable-range
 *             reconfigured.
 *
 * Revision 2.3  1995/06/21  18:06:41  slin
 * CSCdi33294:  Router crashed in smrp_DisplayRoute() when route is
 *              removed.
 *
 * Revision 2.2  1995/06/14  22:54:53  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Convert SMRP processes to use new scheduler.
 * - Access SMRP fields from idb sub-block.
 * - Include interface.h instead of interface_private.h whereever
 *   possible.
 * - Misc SMRP bugfixes.
 *
 * Revision 2.1  1995/06/07  22:44:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "sys_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "ieee.h"
#include "logger.h"
#include "../smrp/msg_smrp.c"		/* Not a typo, see logger.h */
#include "packet.h"
#include "../atalk/atalk_private.h"
#include "../atalk/at_registry.h"
#include "../atalk/macip_util.h"
#include "../atalk/atalk_nbp.h"
#include "smrp_private.h"
#include "smrp_registry.h"
#include "config.h"
#include "../smf/smf_public.h"

/*
 *------------------------------------------------------------------
 * Externals
 *------------------------------------------------------------------
 */
extern uchar apple_broadcast[];

#define ATSMRP_MAJVERSION 1
#define ATSMRP_MINVERSION 0
#define ATSMRP_EDITVERSION 1

/* SUBSYSTEM HEADER AT BOTTOM OF FILE */

/*******************************************************************
 * Multicast addresses for ethernet
 *******************************************************************/

static uchar const atsmrp_ether_allnodes[] =
    { 0x09, 0x00, 0x07, 0x41, 0xff, 0xff };
static uchar const atsmrp_ether_allendpoints[] =
    { 0x09, 0x00, 0x07, 0x40, 0xff, 0xff };
static uchar const atsmrp_ether_allentities[] =
    { 0x09, 0x00, 0x07, 0x7f, 0xff, 0xff };

static char *atsmrp_nbp_type = "cisco SMRP Router";

extern watched_queue *smrpQ;

/*
 *------------------------------------------------------------------
 * atsmrp_MapDefaultRange		(Module Internal Routine)
 *
 * Set the SMRP network range by using the AppleTalk network.
 *------------------------------------------------------------------
 */
static boolean
atsmrp_MapDefaultRange (SMRPNet* net)
{
    idbtype *swidb = net->NetInterface;
    atalkidbtype *idb;

    idb = atalk_getidb(swidb);
    
    if (idb == NULL )
	return (FALSE);

    if (idb->atalk_cablestart == 0) {
	printf("\n%%ATSMRP: AppleTalk network not configured yet");
	return FALSE;
    }
    
    if (atalkif_net_extended(idb)) {
	net->RangeStart = idb->atalk_cablestart;
	net->RangeEnd = idb->atalk_cableend;
    } else {
	net->RangeStart = atalkif_MyNet(idb);
	net->RangeEnd = atalkif_MyNet(idb);
    }
    return (TRUE);
    
}

/*
 *------------------------------------------------------------------
 * atsmrp_MapGroupIDToMulticast           (Module Internal Routine)
 *
 * Map a group id to a multicast address.  Result is stored in mc.
 *------------------------------------------------------------------
 */
static boolean
atsmrp_MapGroupIDToMulticast (hwidbtype *hwidb, SMRPGroupID gid, uchar *mc)
{
    idbtype *idb;
    uchar *byteptr;

    if (!is_ieee802(hwidb)) {
	return FALSE;
    }
    
    idb = firstsw_or_null(hwidb);
    if (idb == NULL)
	return FALSE;
    
    /* Ethernet and FDDI use the same multicast addresses */
    if (idb_is_ethernet(idb) || idb_is_fddi(idb)) {
	switch (gid) {
	case AllNodesGroup:
	    ieee_copy(atsmrp_ether_allnodes, mc);
	    break;
	case AllEndpointsGroup:
	    ieee_copy(atsmrp_ether_allendpoints, mc);
	    break;
	case AllEntitiesGroup:
	    ieee_copy(atsmrp_ether_allentities, mc);
	    break;
	default:
	    ieee_copy(apple_ether_multicast, mc);
	    /* Now copy lower 22 bits of SMRP group address */
	    byteptr = (uchar *) &gid;
	    mc[3] = 0x40 | (*++byteptr & 0x3f);
	    mc[4] = *++byteptr;
	    mc[5] = *++byteptr;
	    break;
	}
    } else
	/* This may not be wise, but use the Apple broadcast address */
	ieee_copy(apple_broadcast, mc);
    
    return TRUE;
}

/*
 *------------------------------------------------------------------
 * atsmrp_SpecialMacaddr                 (Module Internal Routine)
 *
 * Called from AppleTalk output routines to map special multicast
 * addresses.  Result is stored in daddr.
 *------------------------------------------------------------------
 */
static boolean
atsmrp_SpecialMacaddr (paktype *pak, uchar **daddr)
{
    static uchar tempMC[IEEEBYTES];
    hwidbtype *hwidb;
    idbtype *idb;

    idb = pak->if_output;
    hwidb = hwidb_or_null(idb);

    if (hwidb == NULL)
	return FALSE;
    
    if (idb_is_ethernet(idb) || idb_is_fddi(idb)) {
	atsmrp_MapGroupIDToMulticast(hwidb, pak->smrp_mcaddr, tempMC);
	*daddr = tempMC;
	return TRUE;
    }
    return FALSE;
}

/*
 *------------------------------------------------------------------
 * atsmrp_set_addresses
 *
 * Listen to multicast addresses on this idb.
 *------------------------------------------------------------------
 */
static void
atsmrp_set_addresses (idbtype *idb)
{
    SMRPPort *port;
    uchar mcaddr[IEEEBYTES];
    hwidbtype *hwidb;
    boolean do_debug;
    SMRPIdbsb *idbsb;

    hwidb = hwidb_or_null(idb);

    if (hwidb == NULL)
	return;
    
    idbsb = smrp_GetIdbsb(idb);

    if (idbsb == NULL)
	return;
    
    port = idbsb->atsmrp_port;
    
    if (!port || port->State == MCNotInitialized)
	return;

    do_debug = smrp_debug(smrp_group_debug, SMRP_NULL_MASK);

    if (atsmrp_MapGroupIDToMulticast(hwidb, AllNodesGroup, mcaddr)) {
        reg_invoke_tbridge_smf_update(mcaddr,
	  			      idb,
				      SMF_MACTYP_AT_BCAST |
				      SMF_ACTION_RECEIVE);
	if_multi_add(hwidb, idb, mcaddr);
	if (do_debug)
	    buginf("\nATSMRP: Subscribed to %e", mcaddr);
    }
    
    if (atsmrp_MapGroupIDToMulticast(hwidb, AllEntitiesGroup, mcaddr)) {
        reg_invoke_tbridge_smf_update(mcaddr,
	  			      idb,
				      SMF_MACTYP_AT_BCAST |
				      SMF_ACTION_RECEIVE);
	if_multi_add(hwidb, idb, mcaddr);
	if (do_debug)
	    buginf("\nATSMRP: Subscribed to %e", mcaddr);
    }
    
    smrp_ListenToForwards(port, atsmrp_MapGroupIDToMulticast);
}

/*
 *------------------------------------------------------------------
 * atsmrp_Subscribe
 *
 * Map a group id to a multicast address and listen to it on the
 * idb.
 *------------------------------------------------------------------
 */
static void
atsmrp_Subscribe (idbtype *idb, SMRPGroupID gid)
{
    hwidbtype *hwidb;
    uchar tempMC[IEEEBYTES];
    boolean do_debug;
    SMRPIdbsb *idbsb;

    if (!idb)
	return;

    hwidb = hwidb_or_null(idb);
    if (hwidb == NULL)
	return;

    idbsb = smrp_GetIdbsb(idb);

    if (idbsb == NULL)
	return;
    
    do_debug = smrp_debug(smrp_group_debug, SMRP_NULL_MASK);
    
    if (!atsmrp_MapGroupIDToMulticast(hwidb, gid, tempMC))
	return;
    if (idb_is_ethernet(idb) || idb_is_fddi(idb)) {

	if (smrp_can_add_hwaddr(hwidb)) {
	
            reg_invoke_tbridge_smf_update(tempMC,
	  			          idb,
				          SMF_MACTYP_AT_BCAST |
				          SMF_ACTION_RECEIVE);
	    if_multi_add(hwidb, idb, tempMC);
	    if (do_debug)
		buginf("\nATSMRP: Subscribed to %e on %s", tempMC,
		       idb_get_namestring(idb));
	} else {
	    /* Interface cannot tolerate addition of MAC address.
	     * We must resort to resetting the interface.
	     * Reset will add address.
	     */
	    if (do_debug)
		buginf("\nATSMRP: Attempting to reset %s to add new MAC address",
		       idb_get_namestring(idb));
	    
	    (*hwidb->reset)(hwidb);
	}
	if (!smrp_IsSpecialGroupID(gid)) {
	    if (do_debug)
		buginf("\nATSMRP: Adding %e to database", tempMC);
	    smrp_AddMCToDatabase(&idbsb->smrp_mc_database, tempMC);
	}
    }
}

/*
 *------------------------------------------------------------------
 * atsmrp_Unsubscribe
 *
 * Map a group id to a multicast address and stop listening to it
 * if no one else is using it.
 *------------------------------------------------------------------
 */
static void
atsmrp_Unsubscribe (idbtype *idb, SMRPGroupID gid)
{
    uchar tempMC[IEEEBYTES];
    hwidbtype *hwidb;
    int uses;
    SMRPIdbsb *idbsb;
    
    hwidb = hwidb_or_null(idb);

    if (hwidb == NULL)
	return;

    idbsb = smrp_GetIdbsb(idb);

    if (idbsb == NULL)
	return;
    
    if (!atsmrp_MapGroupIDToMulticast(hwidb, gid, tempMC))
	return;
    
    /* We cannot unsubscribe unless we know that the multicast address
     * isn't being used by another group address.
     */
    uses = smrp_DeleteMCFromDatabase(&idbsb->smrp_mc_database, tempMC);

    if (uses == -1)		/* Couldn't find in database */
	return;
    else if (uses == 0) {
	reg_invoke_tbridge_smf_delete(tempMC,
				      idb,
				      SMF_MACTYP_AT_BCAST |
				      SMF_ACTION_RECEIVE);
	if (smrp_debug(smrp_group_debug, SMRP_NULL_MASK))
	    buginf("\nATSMRP: Resetting interface to unsubscribe from %e",
		   tempMC);
	(*hwidb->reset)(hwidb);
    } else {
	if (smrp_debug(smrp_group_debug, SMRP_NULL_MASK))
	    buginf("\nATSMRP: MC Addr %e still in use, uses = %d", tempMC,
		   uses);
    }
}

/*
 *------------------------------------------------------------------
 * atsmrp_CreatePacket                (SMRP Callback Function)
 *
 * Create an AppleTalk DDP packet big enough to hold datasize bytes
 * in the DDP data frame.  DDP header is not initialized!
 *------------------------------------------------------------------
 */
static paktype *
atsmrp_CreatePacket (SMRPIdbsb *idbsb, int datasize)
{
    paktype *pak;
    
    if (!idbsb)
	return NULL;
    
    if (datasize > DDP_MAX_DATABYTES)
	return NULL;

    pak = getbuffer(datasize + DDP_OVERHEADBYTES);
    if (pak == NULL) {
	smrp_stat[SMRP_NO_BUFFERS]++;
	return NULL;
    }
    pak->transport_start = pak->network_start + DDP_OVERHEADBYTES;
    pak->datagramsize = datasize + DDP_OVERHEADBYTES;
    return pak;
}

/*
 *------------------------------------------------------------------
 * atsmrp_GetPacketSource		(SMRP Callback Function)
 *
 * Return the source address for this packet.
 *------------------------------------------------------------------
 */
static UnicastAddr
atsmrp_GetPacketSource (paktype* pak)
{
    return (UnicastAddr) pak->atalk_srcfqa;
}

/*
 *------------------------------------------------------------------
 * atsmrp_CheckPacket			(SMRP Callback Function)
 *
 * Check the packet for checksum errors etc.  If this function
 * returns FALSE, the packet has been disposed of.
 *------------------------------------------------------------------
 */
static boolean
atsmrp_CheckPacket (paktype* pak)
{
    return atalk_check(pak, DDP_TYPE_SMRP, -1, FALSE);
}

/*
 *------------------------------------------------------------------
 * atsmrp_SendMCPacket                   (SMRP Callback Function)
 *
 * Multicast a packet to the group address gid.
 *------------------------------------------------------------------
 */
static boolean
atsmrp_SendMCPacket (SMRPIdbsb *idbsb, paktype *pak, SMRPGroupID gid)
{
    if (!idbsb || !pak)
	return FALSE;
    
    clear_if_input(pak);

    /* Initialize DDP header */
    if (!atalk_pak_newInit(pak, atalk_getidb(idbsb->phys_idb),
			   pak->datagramsize - DDP_OVERHEADBYTES,
			   0, ATALK_BROAD_ADDR, ATALK_SOCK_SMRP,
			   ATALK_SOCK_SMRP, DDP_TYPE_SMRP)) {
	datagram_done(pak);
	return FALSE;
    }
    
    /* Since we are going to multicast this packet, we need to calculate
     * a special data-link address.  However, we can't do this here
     * because the packet hasn't been encapsulated yet.  So, mark
     * the packet as a special one so that when encapsulation occurs,
     * a callback routine will calculate the address.
     */
    pak->atalk_dstmcast = ATALK_SMRP_MULTICAST;
    pak->smrp_mcaddr = gid;	/* Save group id for later use. */

    atalk_DDPcksum(pak);
    if (!atalk_send(pak)) 
        return FALSE;

    smrp_stat[SMRP_OUTPUT]++;
    return TRUE;
}

/*
 *------------------------------------------------------------------
 * atsmrp_SendPacket                     (SMRP Callback Function)
 *
 * Send a packet to the Unicast Address addr.
 *------------------------------------------------------------------
 */
static boolean
atsmrp_SendPacket (SMRPIdbsb *idbsb, paktype *pak, UnicastAddr addr)
{
    ATAddress *fqa;

    /* Interpret addr as a fully qualified AppleTalk address */
    fqa = (ATAddress *) &addr;

    /* Initialize the DDP header */
    if (!atalk_pak_newInit(pak, atalk_getidb(idbsb->phys_idb),
			   pak->datagramsize - DDP_OVERHEADBYTES,
			   fqa->f.net, fqa->f.node, ATALK_SOCK_SMRP,
			   fqa->f.spec, DDP_TYPE_SMRP)) {
	datagram_done(pak);
	return FALSE;
    }

    atalk_DDPcksum(pak);
    if (!atalk_send(pak))
        return FALSE;

    smrp_stat[SMRP_OUTPUT]++;
    return TRUE;
}


/*
 *------------------------------------------------------------------
 * atsmrp_GetNetAddress			(SMRP Callback Function)
 *
 * Get the address for an AppleTalk port.
 *------------------------------------------------------------------
 */
static UnicastAddr
atsmrp_GetNetAddress (idbtype *swidb)
{
    ATAddress fqa;
    atalkidbtype *idb;

    idb = atalk_getidb(swidb);
    
    if (idb == NULL)
	return 0;
    fqa.f.net = atalkif_MyNet(idb);
    fqa.f.node = atalkif_MyNode(idb);
    fqa.f.spec = ATALK_SOCK_SMRP;
    return fqa.l;
}

/*
 *------------------------------------------------------------------
 * atsmrp_ShowProtocol			(SMRP Callback Routine)
 *
 * Show protocol for an AppleTalk port.
 *------------------------------------------------------------------
 */
static void
atsmrp_ShowPortProtocol (idbtype *idb)
{
    SMRPIdbsb *idbsb;
    SMRPPort *port;
    SMRPNetRange net;

    if (idb == NULL)
	return;

    idbsb = smrp_GetIdbsb(idb);

    if (idbsb == NULL)
	return;

    port = idbsb->atsmrp_port;
    
    if (port == NULL)
	return;
    
    net.Start = port->RangeStart;
    net.End = port->RangeEnd;

    printf("\n  SMRP network %s gateway via AppleTalk address %A",
	   smrp_PrintNetRange(&net), atalkif_MyAddress(atalk_getidb(idb)));
}

/*
 *------------------------------------------------------------------
 * atsmrp_GetLinkState			(SMRP Callback Function)
 *
 * Return the link state for an AppleTalk port.
 *------------------------------------------------------------------
 */
static boolean
atsmrp_GetLinkState (SMRPIdbsb *idbsb)
{
    atalkidbtype *idb;

    idb = atalk_getidb(idbsb->phys_idb);
    
    if (idb == NULL)
	return FALSE;
    return atalkif_operational(idb);
}

/*
 *------------------------------------------------------------------
 * atsmrp_MaxPacketSize			(SMRP Callback Function)
 *
 * Return the maximum SMRP datagram size for an AppleTalk port.
 *------------------------------------------------------------------
 */
static int
atsmrp_MaxPacketSize (SMRPIdbsb *idb)
{
    return DDP_MAX_DATABYTES;
}

/*
 *------------------------------------------------------------------
 * atsmrp_PrintNetAddress		(SMRP Callback Routine)
 *
 * sprintf the appletalk address into the buffer.
 *------------------------------------------------------------------
 */
static char *
atsmrp_PrintNetAddress (idbtype *idb, char *buf, UnicastAddr addr)
{
    switch (addr) {
    case AllNodesAddr:
	strcpy(buf, "all routers");
        break;
    case AllEndpointsAddr:
	strcpy(buf, "all endpoints");
        break;
    case AllEntitiesAddr:
	strcpy(buf, "all entities");
        break;
    default:
        /* shift out the socket byte */
        sprintf(buf,"%A",addr>>8);
    }
    return buf;
}

/*
 *------------------------------------------------------------------
 * atsmrp_NBPname
 *
 * Generate NBP name to be registered on this interface.  Result is
 * stored in nve.
 *------------------------------------------------------------------
 */
static void
atsmrp_NBPname (nbpEntity *nve, atalkidbtype *idb)
{
    ziptabletype *ztab;
    str32 ifzone;
    char ifname[100];
    
    /* formulate interface name */
    sprintf(ifname, "%s.%s", hostname, idb_get_namestring(idb->phys_idb));
    ifname[32] = '\0';
    
    /* make primary zone name a str32 */
    ztab = atalkif_primary_zone(idb);
    str32ninit(&ifzone, (char *) ztab->name, ztab->length);
    
    /* create name, type, zone */
    atnbp_mkname(nve, ifname, atsmrp_nbp_type, STR32PTR(&ifzone));
}

/*
 *------------------------------------------------------------------
 * atsmrp_register
 *
 * Register a NBP name on this interface.
 *------------------------------------------------------------------
 */
static void
atsmrp_register (atalkidbtype *idb)
{
    nbpEntity nve;
    nbp_result err;
    ataddress src;

    if (idb == NULL)
	return;
    
    /* generate NBP name for this interface */
    atsmrp_NBPname(&nve, idb);
    
    src.net  = atalkif_MyNet(idb);
    src.node = atalkif_MyNode(idb);
    src.spec = ATALK_SOCK_SMRP;

    err = atnbp_register(&nve, &src, TRUE);
    if (err != NBP_REG_PENDING) {
	buginf("\nATSMRP:  couldn't register '%s'@[%s]:  %s",
	       atnbp_entityStr(&nve), ddpAddrStr(&src), atnbp_resultStr(err));
	return;
    } else {
	if (at_debug(atalk_events, ATALK_INTERFACE_MASK, idb))
	    errmsg(AT_ERR_REGPENDING, atnbp_entityStr(&nve), ddpAddrStr(&src));
    }
}

/*
 *------------------------------------------------------------------
 * atsmrp_unregister
 *
 * Unregister NBP name on this interface.
 *------------------------------------------------------------------
 */
static void
atsmrp_unregister (idbtype *swidb)
{
    ataddress ddp;
    atalkidbtype *idb = atalk_getidb(swidb);
    
    ddp.net  = atalkif_MyNet(idb);
    ddp.node = atalkif_MyNode(idb);
    ddp.spec = ATALK_SOCK_SMRP;

    atnbp_DDPunregister(&ddp);
}

/*
 *------------------------------------------------------------------
 * atsmrp_OpenPort
 *
 * Perform any Appletalk-specific actions when opening a smrp port.
 *------------------------------------------------------------------
 */
static boolean
atsmrp_OpenPort (idbtype *swidb)
{
    hwidbtype *hwidb = hwidb_or_null(swidb);
    atalkidbtype *idb;
    char buf[14];

    if (hwidb == NULL)
	return FALSE;

    idb = atalk_getidb(swidb);
    
    /* Register nbp type in our primary zone. */
    if (!atalkif_CaymanTunnel(idb) && !atalkif_AURPTunnel(idb))
	atsmrp_register(idb);

    if (idb_is_fddi(swidb) &&
	hwidb->atalk_compat_flags & AT_COMPAT_FDDI) {
	errmsg(&msgsym(NOFDDICOMPAT, SMRP),
	       atsmrp_PrintNetAddress(swidb, buf,
				      atsmrp_GetNetAddress(swidb)));
	return FALSE;		/* Prefdditalk not supported */
    }    
    atalk_SMRP_running = TRUE;
    return TRUE;
}

/*
 *------------------------------------------------------------------
 * atsmrp_ClosePort
 *
 * Perform any AppleTalk-specific actions when closing a port.
 *------------------------------------------------------------------
 */
static void
atsmrp_ClosePort (SMRPIdbsb *idbsb)
{
    idbtype *idb;

    idb = idbsb->phys_idb;
    
    /* Unregister nbp type */
    atsmrp_unregister(idb);

    /* Reset interface to unsubscribe from mc addresses */
    (idb->hwptr->reset)(idb->hwptr);
}

/*
 *------------------------------------------------------------------
 * atsmrp_DeletePort
 *
 * Perform any AppleTalk-specific actions when deleting a port.
 *------------------------------------------------------------------
 */
static void
atsmrp_DeletePort (SMRPIdbsb *idbsb)
{
    uchar mcaddr[IEEEBYTES];
    idbtype *idb = idbsb->phys_idb;
    hwidbtype *hwidb = idb->hwptr;

    /* Remove MAC filters */
    if (atsmrp_MapGroupIDToMulticast(hwidb, AllNodesGroup, mcaddr)) {
        reg_invoke_tbridge_smf_delete(mcaddr,
                                      idb,
                                      SMF_MACTYP_AT_BCAST |
                                      SMF_ACTION_RECEIVE);
    }
    if (atsmrp_MapGroupIDToMulticast(hwidb, AllEntitiesGroup, mcaddr)) {
        reg_invoke_tbridge_smf_delete(mcaddr,
                                      idb,
                                      SMF_MACTYP_AT_BCAST |
                                      SMF_ACTION_RECEIVE);
    }

    /* Unlink idb from port */
    if (idbsb)
	idbsb->atsmrp_port = NULL;
}

/*
 *------------------------------------------------------------------
 * atsmrp_DoTunnelRequest
 *
 * Handle a TunnelRequest packet from an endpoint.
 *------------------------------------------------------------------
 */
static boolean
atsmrp_DoTunnelRequest (SMRPPort *port, SMRPResponse *res)
{
    
    SMRPRoute *route;
    SMRPNetRange range;
    MCTunnelPacket *pkt;
    nbpEntity nve;
    
    pkt = (MCTunnelPacket *)res->RequestData;
    
    range.Start = GETSHORT(&pkt->RangeStart);
    range.End = GETSHORT(&pkt->RangeEnd);
    
    route = smrp_FindRoute(&range);

    if (route) {
	if (route->Distance != 0 || (route->TunnelDistance != 0 &&
				     !smrp_IsOurNodePortPrimary(port))) {
	    /* We should Forward request, but ignore for now. */
	} else if (route->TunnelDistance == 0) {
	    smrp_SendRejectResponse(res, MCNoTunnelNeeded);
	    return TRUE;
	} else if (smrp_IsOurNodePortPrimary(port)) {
	    PUTLONG(&pkt->Address, port->ThisNodeAddress);
	    atsmrp_NBPname(&nve,
			   atalk_getidb(port->NetworkReference->phys_idb));
	    str32cpy((str32 *) smrp_datastart_offset((MCHeader *)res->ResponseBuffer),
		     &nve.srvr_name);
	    res->ResponseSize = SMRP_HEADER_SIZE + SMRP_TUNNEL_PKT_SIZE;
	    smrp_SendConfirmResponse(res);
	    return TRUE;
	}
    }
    smrp_SetResponseState(res, MCRequestComplete);
    smrp_DiscardResponse(res);
    return TRUE;
}

static SMRPCallDowns const atalk_nethandlers = {
    atsmrp_Subscribe,
    atsmrp_Unsubscribe,
    atsmrp_CreatePacket,
    atsmrp_GetPacketSource,
    atsmrp_CheckPacket,
    atsmrp_SendMCPacket,
    atsmrp_SendPacket,
    atsmrp_MaxPacketSize,
    atsmrp_PrintNetAddress,
    atsmrp_GetLinkState,
    atsmrp_GetNetAddress,
    atsmrp_ShowPortProtocol,
    atsmrp_OpenPort,
    atsmrp_ClosePort,
    atsmrp_DeletePort,
    atsmrp_DoTunnelRequest
};

/*
 *------------------------------------------------------------------
 * atsmrp_MapATtoSMRPaddr
 *
 * Map AppleTalk address to SMRP unicast address.
 *------------------------------------------------------------------
 */
static UnicastAddr
atsmrp_MapATtoSMRPaddr (ataddrtype ataddr)
{
    ATAddress fqa;
    
    fqa.f.net = atalk_netonly(ataddr);
    fqa.f.node = atalk_nodeonly(ataddr);
    fqa.f.spec = ATALK_SOCK_SMRP;
    return fqa.l;
}

/*
 *------------------------------------------------------------------
 * atsmrp_ParseNetAddr
 *
 * Check if passed net can be configured.
 *------------------------------------------------------------------
 */
static void
atsmrp_ParseNetAddr (addrtype* netaddr, UnicastAddr* smrpaddr)
{
    ushort net;
    uchar node;
    ataddrtype atalk_addr;
    
    net = netaddr->apple_addr.net;
    node = netaddr->apple_addr.host;
    atalk_addr = atalk_address(net, node);
    *smrpaddr = atsmrp_MapATtoSMRPaddr(atalk_addr);
}

/*
 *------------------------------------------------------------------
 * atsmrp_AddNetRange
 *
 * Check if passed net can be configured.
 *------------------------------------------------------------------
 */
static void
atsmrp_AddNetRange (SMRPNet *net, SMRPCallDowns const **handlers,
		    boolean checkport)
{
    idbtype *idb = net->NetInterface;

    if (!idb)
	return;

    if (checkport) {
	SMRPIdbsb *idbsb;

	idbsb = smrp_GetIdbsb(idb);

	/* First check if port already defined. */
	if (idbsb && idbsb->atsmrp_port) {
	    printf("\n%%ATSMRP: AppleTalk port already defined on %s",
		   idb_get_namestring(idb));
	    return;
	}
    }
    
    if (!net->ConfiguredRange)
	if (!atsmrp_MapDefaultRange(net)) {
	    printf("\n%%ATSMRP: Unable to map default range from network layer");
	    return;
	}
    
    *handlers = &atalk_nethandlers;
}

/*
 *------------------------------------------------------------------
 * atsmrp_nvgen_interface_protocol
 *
 * Do NV generation for 'smrp protocol'.
 *------------------------------------------------------------------
 */
static void
atsmrp_nvgen_interface_protocol (parseinfo *csb)
{
    SMRPPort *port;
    idbtype *idb;
    SMRPIdbsb *idbsb;
    
    if (!smrp_running)
	return;
    
    if (csb->nvgen) {
	idb = csb->interface;

	if (idb == NULL)
	    return;

	idbsb = smrp_GetIdbsb(idb);
	if (idbsb == NULL)
	    return;
	
	port = idbsb->atsmrp_port;

	if (port && (port->NetworkType == LINK_APPLETALK)) {
	    nv_write(TRUE, "%s appletalk ", csb->nv_command);
	    nv_add(port->ConfiguredRange, "netrange %d-%d",
		   port->RangeStart, port->RangeEnd);
	}
	
    }	
}

/*
 *------------------------------------------------------------------
 * atsmrp_port_cstate
 *
 * Called from ATALK to signal SMRP of change in state.
 *------------------------------------------------------------------
 */
static void
atsmrp_port_cstate (atalkidbtype *idb)
{
    SMRPIdbsb *idbsb;
    
    if (idb == NULL)
	return;
    
    idbsb = smrp_GetIdbsb(idb->phys_idb);

    if (idbsb == NULL)
	return;
    
    smrp_PortIDLinkState(idbsb->atsmrp_port, idb->atalk_operational);
}

/*
 * atsmrp_port_route_adjust
 *
 * Perform route adjust actions on ATSMRP port when interface flaps. 
 */
static void
atsmrp_port_route_adjust (idbtype *idb)
{
    SMRPPort *port;
    SMRPIdbsb *idbsb;

    if (idb == NULL)
	return;

    idbsb = smrp_GetIdbsb(idb);

    if (idbsb == NULL)
	return;
    
    port = idbsb->atsmrp_port;

    if (!port)
	return;

    if (interface_up(idb)) {
	if (!smrp_IsPortActive(port))
	    smrp_StartPort(port);
    } else {
	if (smrp_IsPortActive(port))
	    smrp_ClosePort(port);
    }
}

/*
 *------------------------------------------------------------------
 * atsmrp_routing_change
 *
 * SMRP registry call to perform any AppleTalk-specific actions
 * when SMRP routing is enabled.
 *------------------------------------------------------------------
 */
static void
atsmrp_routing_change (boolean start)
{
    if (start) {
	atalk_BindSocket(smrpQ, "SMRP", ATALK_SOCK_SMRP,
			 smrp_FastProcessForward);
    } else {
	atalk_UnBindSocket(ATALK_SOCK_SMRP);
    }
}

static void
atsmrp_register_thyself (void)
{
    reg_add_atalk_if_operational(atsmrp_port_cstate, "atsmrp_port_cstate");
    reg_add_atalk_if_disable(atsmrp_port_cstate, "atsmrp_port_cstate");
    
    reg_add_atalk_special_macaddr(ATALK_SMRP_MULTICAST,
				  atsmrp_SpecialMacaddr,
				  "atsmrp_special_macaddr");
    reg_add_smrp_routing_change(atsmrp_routing_change,
				"atsmrp_routing_change");
    reg_add_smrp_port_route_adjust(atsmrp_port_route_adjust,
				   "atsmrp_port_route_adjust");
    
    reg_add_smrp_add_netrange(LINK_APPLETALK, atsmrp_AddNetRange,
			      "atsmrp_AddNetRange");

    reg_add_smrp_parse_netaddr(LINK_APPLETALK, atsmrp_ParseNetAddr,
			      "atsmrp_ParseNetAddr");

    reg_add_smrp_nvgen_interface_protocol(atsmrp_nvgen_interface_protocol,
					  "atsmrp_nvgen_interface_protocol");
    reg_add_smrp_set_addresses(atsmrp_set_addresses, "atsmrp_set_addresses");
    
}

static void
atsmrp_init (subsystype *subsys)
{
    atsmrp_register_thyself();

}

SUBSYS_HEADER (atsmrp, ATSMRP_MAJVERSION, ATSMRP_MINVERSION,
	       ATSMRP_EDITVERSION, atsmrp_init, SUBSYS_CLASS_PROTOCOL,
	       "seq: atalk, smrp", "req: atalk, smrp");
