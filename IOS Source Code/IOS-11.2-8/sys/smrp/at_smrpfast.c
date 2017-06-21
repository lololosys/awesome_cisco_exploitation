/* $Id: at_smrpfast.c,v 3.1.2.1 1996/05/02 08:44:32 dwong Exp $
 * $Source: /release/112/cvs/Xsys/smrp/at_smrpfast.c,v $
 *------------------------------------------------------------------
 * AppleTalk SMRP fast switching support
 *
 * March 1996, Kent Leung
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_smrpfast.c,v $
 * Revision 3.1.2.1  1996/05/02  08:44:32  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.1  1996/04/16  23:03:43  kleung
 * Placeholder for CA commit.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "packet.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/fddi.h"
#include "config.h"
#include "../wan/dialer.h"
#include "../wan/serial.h"
#include "../wan/smds.h"
#include "../wan/frame_relay.h"
#include "../wan/atm_dxi.h"
#include "logger.h"
#include "../les/if_les.h"
#include "../atalk/atalk_private.h"
#include "../atalk/at_fast.h"
#include "../atalk/at_fast_les.h"
#include "../atalk/at_registry.h"
#include "smrp_private.h"
#include "smrp_registry.h"
#include "at_smrpfast.h"

#define DDPLENGTHMASK_LONG 0x03ff0000       /* length field for long */

/*
 *------------------------------------------------------------------
 * atsmrp_UpdateMCache
 *
 * Create a f/s cache entry for the outbound packet.  Appletalk has
 * two primary encapsulations on Ethernet - Ethertalk and 802.3 SNAP.
 * Ethertalk has a 17 byte encapsulation (14 bytes of Ethernet V2 and
 * 3 bytes of Apple's own LLAP) and the 802.3 SNAP encapsulation is
 * 22 bytes.
 *------------------------------------------------------------------
 */
boolean
atsmrp_UpdateMCache(paktype* pak)
{
    idbtype* idb;
    uchar* ds;
    char buf1[40], buf2[40];
    int len;
    ushort	encaptype = AT_FS_MIN;
    uchar phaseOne;
    hwidbtype* hwidb;
    ushort	net;
    uchar	node;
    SMRPForward* fwd = NULL;
    SMRPChildPortLink* child = NULL;
    ddplong* ddp;
    SMRPCache* cptr;
    atalkidbtype *atalkidb;
    
    /* If either input or output interface is null, don't bother. */
    if (   (pak->if_input == NULL)
	|| (pak->if_output == NULL))
	return TRUE;

    /* Handle only SMRP MC packets */
    if (pak->atalk_dstmcast != ATALK_SMRP_MULTICAST) 
	return TRUE;

    idb = pak->if_output;
    hwidb = hwidb_or_null(idb);
    if (hwidb == NULL)
	return TRUE;
    if (!hwidb->atalk_fastokay)
	return TRUE;
    atalkidb = atalk_getidb(idb);
    if (atalkidb == NULL)
	return TRUE;
    
    /* Look up forward entry */
    if (!smrpfast_UpdateMCache(pak, &fwd, &child))
	return TRUE;

    if (fwd == NULL || child == NULL)
	return TRUE;
    
    /* Determine encap type and packet length */
    ds = pak->datagramstart;
    phaseOne = FALSE;

    if (idb_is_ethernet(idb)) {
	if (!(ds[ETALKETHERDST] & ETHER_MAC_MULTICAST))
	    return TRUE;		
	if (atalkif_net_extended(atalkidb)) {
	    len = ETHER_ARPA_ENCAPBYTES - 2; /* Omit the length bytes */
	    encaptype = AT_FS_ETHER_SNAP;
	} else {
	    len = ETALK_ENCAPBYTES; /* Include the AppleTalk lap bytes */
	    phaseOne = TRUE;
	    encaptype = AT_FS_ETHER_ARPA;
	}
    } else if (idb_is_fddi(idb)) {
	switch (hwidb->type) {
	  case IDBTYPE_FDDIT:
	    if (!(ds[1] & ETHER_MAC_MULTICAST))
		return TRUE;
	    break;
	  case IDBTYPE_BSI:
	  case IDBTYPE_FDDI:
	    if (!(ds[1] & TRING_MAC_MULTICAST))
		return TRUE;
	    break;
	  default:
	    return TRUE;
	}
	encaptype = AT_FS_FDDI;
	len = FDDI_ENCAPBYTES - 1;	/* omit the LLC_FC byte */
	ds++;
    } else if ((hwidb->status & IDB_SERIAL)
	       && (   hwidb->enctype == ET_HDLC
		   || idb_is_framerelay(idb)
		   || hwidb->enctype == ET_FR_IETF
		   || hwidb->enctype == ET_ATM_DXI
		   || (hwidb->enctype == ET_PPP && !is_ddr(hwidb))
		   || hwidb->enctype == ET_SMDS)) {
	if (!atalkif_net_extended(atalkidb))
	    return TRUE;

	len = pak->encsize;
	if (len == 0 || len > MAXATMACSTRING) {
	    buginf("\nAT: encapsulation failed, pak->encsize = %d",len);
	    return TRUE;
	}
	switch (pak->enctype) {
	  case ET_HDLC:		encaptype = AT_FS_HDLC;		break;
	  case ET_PPP:		encaptype = AT_FS_PPP;		break;
	  case ET_FR_IETF:	encaptype = AT_FS_FRAME_IETF;	break;
	  case ET_FRAME_RELAY:	encaptype = AT_FS_FRAME;	break;
	  case ET_ATM_DXI:	encaptype = AT_FS_ATM_DXI;	break;
	  case ET_SMDS:		encaptype = AT_FS_SMDS;		break;
	  default:
	    break;
	}
	
    } else {				/* an interface we don't fast-switch */
	return TRUE;
    }

    ATASSERT(len <= MAXATMACSTRING);

    /*
     * Cache consistency checks.
     * Don't insert entries for the following addresses:
     * Network #'s which are not 0.
     * Node #'s which are not 255 (broadcast).
     */
    if ((net = atalk_pak_dnet(pak)) != 0)
	return TRUE;
    node = atalk_pak_dnode(pak);
    if (node != ATALK_BROAD_ADDR)
	return TRUE;
  
    cptr = child->FastCache;
    if (cptr == NULL)
	if ((cptr = (SMRPCache*) smrpfast_CreateCache()) == NULL)
	    return TRUE;

    /* Populate cache */
    /* Save MAC info */
    cptr->mac.at.net = net;
    cptr->mac.at.node = node;
    cptr->mac.at.flags = 0;
    cptr->mac.at.encaptype  = encaptype;
    cptr->mac.at.subint_reference_num =
	reg_invoke_get_subint_ref_num_from_pak(hwidb->enctype, hwidb, pak);
    cptr->mac.at.phaseOne   = phaseOne;
    cptr->mac.at.byte_len   = len;	 /* total length of entry, rif & all */
    cptr->mac.at.word_len   = btow(len); /* do this once, and not at f/s time */
    cptr->mac.at.idb        = idb;
    cptr->mac.at.hwif       = hwidb;
    cptr->mac.at.atalkidb   = atalkidb;
    
    bcopy(ds, cptr->mac.at.m.macstring, len);
    bcopy(ds+1, cptr->mac.at.s.macstring, len-1);
    cptr->mac_len = len;
    cptr->mac_ptr = (uchar*)&cptr->mac.at.m.macstring;

    /* 
     * Save entire DDP header.  But zero out checksum since the
     * src net and node is over-written with local address.  
     */
    ds = pak->network_start;
    ddp = (ddplong*)&cptr->net.at.l;
    
    bcopy(ds,ddp,DDP_OVERHEADBYTES);
    cptr->net_len = DDP_OVERHEADBYTES;
    PUTSHORT((ushort*)ddp->cksum, (ushort)0);
    child->FastCache = cptr;
    
    /* Marked entry as fast switching candidate */
    fwd->CacheUpdated = TRUE;
    
    /* mcache debugging */
    if (smrp_debug(smrp_mcache_debug, SMRP_NULL_MASK))
        buginf("\nSMRP: Cache populated for group %s, \n\tmac - %s\n\tnet - %s",
	       smrp_DisplayGroupAddr(fwd->GroupAddress), 
	       smrpfast_DumpCacheMacHdr(cptr, buf1),
	       smrpfast_DumpCacheNetHdr(cptr, buf2));

    return TRUE;
}


