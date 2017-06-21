 /* $Id: at_smrpfast_les.c,v 3.4.12.5 1996/05/02 08:44:36 dwong Exp $
 * $Source: /release/112/cvs/Xsys/smrp/at_smrpfast_les.c,v $
 *------------------------------------------------------------------
 * SMRP's AppleTalk fast switching support
 *
 * August 1995, Kent Leung
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_smrpfast_les.c,v $
 * Revision 3.4.12.5  1996/05/02  08:44:36  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.4.12.4  1996/04/27  06:46:11  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.3.20.3  1996/04/26  00:50:18  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.3.20.2  1996/04/17  04:05:44  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.3.20.1  1996/02/23  20:54:19  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.4.12.3  1996/04/17  13:47:14  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.4.12.2  1996/03/23  01:33:51  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.4.12.1  1996/03/18  21:47:39  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/13  01:59:21  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.2  1996/03/07  10:42:31  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  17:10:27  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/13  08:15:48  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.3  1995/11/17  18:52:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:23:19  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:12:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:24:51  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/05  02:20:16  kleung
 * Placeholder
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
#include "../wan/serial.h"
#include "../wan/smds.h"
#include "../wan/frame_relay.h"
#include "../wan/atm_dxi.h"
#include "logger.h"
#include "../les/if_les.h"
#include "../wan/dialer.h"
#include "../atalk/atalk_private.h"
#include "../atalk/at_fast.h"
#include "../atalk/at_fast_les.h"
#include "smrp_private.h"
#include "../atalk/at_registry.h"
#include "smrp_registry.h"
#include "smrp_fast.h"
#include "at_smrpfast.h"

#ifdef SMRP_FAST_DEBUG
#include "ttysrv.h"
#endif

#define DDPLENGTHMASK_LONG 0x03ff0000       /* length field for long */

/*
 * Forward declarations.
 */
static void atsmrp_les_fs_init (subsystype* subsys);
static boolean atsmrp_les_fastswitch (paktype*, SMRPCache*);
static void atsmrp_RegisterPorts (idbtype*);
static inline void atsmrp_write_ddp_header (ulong*, ddplong*, ushort);

    
/*
 * SMRP Appletalk LES FS subsystem header
 */

#define ATSMRPLESFS_MAJVERSION 1
#define ATSMRPLESFS_MINVERSION 0
#define ATSMRPLESFS_EDITVERSION  1

SUBSYS_HEADER(atsmrp_les_fs, ATSMRPLESFS_MAJVERSION, ATSMRPLESFS_MINVERSION, ATSMRPLESFS_EDITVERSION,
	      atsmrp_les_fs_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: smrp",
	      "req: fastswitch, atalk, smrp");

/*
 *------------------------------------------------------------------
 * atsmrp_les_fs_init			(Module Internal Function)
 *
 * Initialization for ATSMRP LES f/s.
 *------------------------------------------------------------------
 */
static void 
atsmrp_les_fs_init (subsystype* subsys)
{
    reg_add_smrp_fs_register_ports(LINK_APPLETALK, atsmrp_RegisterPorts,
				       "atsmrp_RegisterPorts");
    reg_add_atalk_smrp_fs_update_mcache(atsmrp_UpdateMCache,
					"atsmrp_UpdateMCache");
    reg_add_atalk_smrp_fastswitch(smrpfast_fastswitch, "smrpfast_fastswitch");
}

/*
 *------------------------------------------------------------------
 *
 * smrp_RegisterPorts			(Module Internal Function)
 *
 * Register the port for f/s.
 *------------------------------------------------------------------
 */
static void
atsmrp_RegisterPorts (idbtype* idb)
{
    SMRPIdbsb* idbsb;
    hwidbtype *hwidb = hwidb_or_null(idb);

    if (hwidb == NULL)
	return;
    
    idbsb = smrp_GetIdbsb(idb);
    if (idbsb == NULL)
	return;

    /* We are now up if fsreason == fsEnabled, therefore can now reset */
    if (hwidb->atalk_routecache == fsEnabled) {
	/* This is actually the function called when packet is going out
	 * this port.  All incoming packets are generically processed.
	 */
	idbsb->atsmrp_port->Fastswitch = atsmrp_les_fastswitch;
	idbsb->atsmrp_port->Fastswitch_name = "SMRP AppleTalk";
    }
    
    idbsb->atsmrp_port->RouteCache = hwidb->atalk_routecache;
	
}

/*
 *------------------------------------------------------------------
 * Here's what the DDP frame looks like from the pak->network_start
 * on...
 *
 * +-------+-------+-------+-------+-------+-------+-------+-------+
 * |       |       |       |       |       |       |       |       |
 * | hop ct. & len |   DDP cksum   |   dest net.   |  src net      |
 * |       |       |       |       |       |       |       |       |
 * +-------+-------+-------+-------+-------+-------+-------+-------+
 * {        first long write      }{     second long write        }
 *
 * +-------+-------+-------+-------+-------+
 * |       |       |       |       |       |
 * | dnode | snode | dskt  | sskt  | type  |
 * |       |       |       |       |       |
 * +-------+-------+-------+-------+-------+
 * {        third long write      }
 *
 * The only fields that must be overwritten are the source network
 * and node.  The other fields remain the same, but are overwritten
 * for convenience.  The length field is retained if input and
 * and output are the same link type.
 *------------------------------------------------------------------
 */

static inline void
atsmrp_write_ddp_header (ulong *ptr, ddplong *ddp, ushort len)
{
    ulong hoplensum;
    
    /* 
     * Recalculate length if inbound link type is different.
     * For now, it's an no-op.
     */

    /* 
     * Don't overwrite datagram length, but zero out hop count
     * and checksum.  The src net and node is replaced with local
     * addr.  The other fields don't change.  Skip DDP type field.  
     */
    hoplensum = GETLONG(ptr) & DDPLENGTHMASK_LONG; 
    PUTLONG(ptr++, hoplensum);
                                             /* hop & len, cksum */
    PUTLONG(ptr++, GETLONG(&ddp->dnet));     /* dnet, dnet, snet, snet */
    PUTLONG(ptr++, GETLONG(&ddp->dnode));    /* dnode, snode, dsock, ssock */

}

/*
 *------------------------------------------------------------------
 * This routine is mostly a regurgitation of at_fastswitch().
 * The DDP header is overwritten from the SMRP cache.  Then only
 * the supported media/encaps are f/s.
 *------------------------------------------------------------------
 */
static boolean
atsmrp_les_fastswitch (paktype* pak, SMRPCache* cache)
{
    atcachetype* cptr;
    hwidbtype* output;
    uchar *datagramstart = pak->datagramstart;
    ushort datagramsize;
    ddplong* ddp;
    shortbyte hopword;

    if (cache == NULL)
	return FALSE;
    
    cptr = &cache->mac.at;
    ddp = (ddplong *)pak->network_start;
    hopword.d.sword = GETSHORT(ddp->length);
    datagramsize = hopword.d.sword & DDPLENGTHMASK;

    atsmrp_write_ddp_header((ulong*)ddp, (ddplong*)&cache->net.at, 
			    (ushort)datagramsize);
    
    output = cptr->hwif;

#ifdef SMRP_FAST_DEBUG
    if (smrp_debug(smrp_mcache_debug, SMRP_NULL_MASK)) 
	ttyprintf(CONTTY,
		  "\nATSMRP(fast): %s: pkt = %08x %08x %08x %08x %08x %08x", 
		  output->hw_namestring,
		  GETLONG(datagramstart + 12),
		  GETLONG(datagramstart + 16),
		  GETLONG(datagramstart + 20),
		  GETLONG(datagramstart + 24),
		  GETLONG(datagramstart + 28),
		  GETLONG(datagramstart + 32));
#endif

    switch (output->fast_switch_type) {

      case FS_ETHER:
	/*
	 * Switch to ethernet
	 */
	if (cptr->phaseOne) {
	    /*
	     * Switch to ATp1 ethernet
	     */
	    datagramsize  = datagramsize + ETALK_ENCAPBYTES;
	    datagramstart = pak->network_start - ETALK_ENCAPBYTES;

	    at_write_etalk_header((ulong *)datagramstart,
				  cptr,
				  hopword.d.byte[0] & 0x3);
	} else {
  	    /*
  	     * Switch to ATp2 ethernet
  	     */
	    datagramsize  = datagramsize + ETHER_SNAP_ENCAPBYTES;
	    datagramstart = pak->network_start - ETHER_SNAP_ENCAPBYTES;

	    at_write_ether_header((ulong *)datagramstart,
				  cptr,
				  datagramsize,
				  hopword.d.byte[0] & 0x3);
	}

	if (datagramsize < MINETHERSIZE)
	    datagramsize = MINETHERSIZE;
	pak->datagramsize  = datagramsize;
	pak->datagramstart = datagramstart;

	pak->if_output = cptr->idb;
	(*output->fastsend)(output, pak);
	return (TRUE);

      case FS_FDDI:
	/*
	 * Switch to FDDI
	 */
	datagramsize  = datagramsize + FDDI_SNAP_ENCAPBYTES;
	datagramstart = pak->network_start - FDDI_SNAP_ENCAPBYTES;

	at_write_fddi_header((ulong *)datagramstart,
			     cptr,
			     hopword.d.byte[0] & 0x3);

	pak->datagramsize  = datagramsize;
	pak->datagramstart = datagramstart;

	pak->if_output = cptr->idb;
	(*output->fastsend)(output, pak);
	return(TRUE);

      case FS_HDLC:
      case FS_PPP:
	/*
	 * Switch to serial, HDLC and PPP
	 */
	datagramsize  = datagramsize + cptr->byte_len;
	datagramstart = pak->network_start - cptr->byte_len;

	at_write_serial_header((ulong *)datagramstart,
			       cptr,
			       hopword.d.byte[0] & 0x3);

	pak->datagramsize  = datagramsize;
	pak->datagramstart = datagramstart;

	pak->if_output = cptr->idb;
	(*output->fastsend)(output, pak);
	return (TRUE);

      case FS_FR:
	/*
	 * Switch to serial, Frame Relay
	 */
	{
	    fr_pvc_info_type *fr_pvc;
	    datagramsize  = datagramsize + cptr->byte_len;
	    datagramstart = pak->network_start - cptr->byte_len;

	    at_write_serial_header((ulong *)datagramstart,
				   cptr,
				   hopword.d.byte[0] & 0x3);

	    pak->datagramsize  = datagramsize;
	    pak->datagramstart = datagramstart;

	    pak->if_output = cptr->idb;
	    (*output->fastsend)(output, pak);
	    fr_pvc = HWIDB_TO_FR_PVC(output, cptr->subint_reference_num);
	    INC_PVC_FAST_OUTPUT(fr_pvc, datagramsize);
	}
	return (TRUE);

      case FS_ATM_DXI:
	/*
	 * Switch to serial, ATM_DXI
	 */
	datagramsize  = datagramsize + cptr->byte_len;
	datagramstart = pak->network_start - cptr->byte_len;

	at_write_serial_header((ulong *)datagramstart,
			       cptr,
			       hopword.d.byte[0] & 0x3);

	pak->datagramsize  = datagramsize;
	pak->datagramstart = datagramstart;

	pak->if_output = cptr->idb;
	(*output->fastsend)(output, pak);
	INC_DXI_PVC_FAST_OUTPUT(HWIDB_TO_DXI_PVC(output,
						 cptr->subint_reference_num),
				datagramsize);
	return (TRUE);

      case FS_SMDS:
	/*
	 * Switch to SMDS. We're going to do this all in-line because
	 * it will make writing the trailing encapsulation easier.
	 * SMDS is proof positive that Darwinism no longer works
	 * because someone who was stupid enough to design a protocol
	 * with not only a trailing length field, but a trailing length
	 * field which is padded to a longword boundary (speed considerations,
	 * don't you know) wasn't killed during lunchtime in a pedestrian
	 * crosswalk.
	 */
	{
	    smdstype	*sm;
	    uint	modelen, pad;
	    uchar	*trailptr;

	    modelen = output->smds_pdb->fast_output_modelen;

	    /* trailer needs to be at the end - easiest calculation is here */
	    trailptr = pak->network_start + datagramsize;
	    pak->datagramstart = pak->network_start - cptr->byte_len;

	    sm = (smdstype *)(pak->datagramstart + modelen);
	    /*
	     * Copy the header from the cache, then write the
	     * miscellaneous smds stuff directly into the pak
	     */
	    at_write_serial_header((ulong *)pak->datagramstart, cptr,
				   hopword.d.byte[0] & 0x3);

	    pad = 3 - ((datagramsize + 3) % 4);
	    datagramsize += cptr->byte_len;
	    sm->smds_xhlpi &= ~3;
	    sm->smds_xhlpi |= pad;
	    sm->smds_betag = output->smds_begin_tag++;

	    /*
	     * Now write the trailer stuff (0 the pad bytes, then
	     * write a long at the end)  Basize is also written here,
	     * because the calculation is easier.
	     */
	    PUTLONG(trailptr, 0);
	    
	    datagramsize += pad;
	    trailptr += pad;
	    PUTSHORT(&sm->smds_basize,
		     (datagramsize - (sizeof(ulong) + modelen)));
	    pak->datagramsize = datagramsize + sizeof(ulong);
	    PUTLONG(trailptr, GETLONG(&sm->smds_rsvd)); /* Trailer copy */
	    pak->if_output = cptr->idb;
	    (*output->fastsend)(output, pak);
	}
	return(TRUE);

      default:
	return (FALSE);
    }
}


