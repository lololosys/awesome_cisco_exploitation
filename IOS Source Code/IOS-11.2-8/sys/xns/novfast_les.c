/* $Id: novfast_les.c,v 3.13.4.13 1996/08/15 23:45:32 wilber Exp $
 * $Source: /release/112/cvs/Xsys/xns/novfast_les.c,v $
 *------------------------------------------------------------------
 * novfast_les.c -- Novell Fast Switching module providing low end specific
 *                  Support.
 *
 * William H. Palmer, October 1993
 * 26-June-1990, Phanindra Jujjavarapu
 *
 * Copyright (c) 1988-1996,1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: novfast_les.c,v $
 * Revision 3.13.4.13  1996/08/15  23:45:32  wilber
 * CSCdi66123:  BVI code continues to use a packet after disposal
 * (datagram_done)
 * Fix the missing/misused returns
 * Branch: California_branch
 *
 * Revision 3.13.4.12  1996/08/14  14:12:14  dblair
 * CSCdi64968:  Implment outbound half-fastswitching for multilink PPP
 * Branch: California_branch
 * add multilink half-fastswitching support
 *
 * Revision 3.13.4.11  1996/08/14  02:37:34  hampton
 * Use static inline routines to clean up references to the access list
 * array.  [CSCdi65925]
 * Branch: California_branch
 *
 * Revision 3.13.4.10  1996/08/13  02:27:41  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.13.4.9  1996/07/23  20:20:26  john
 * CSCdi63741:  Misalignment at nov_fastswitch(0x6017aa4c)+0x40
 * Branch: California_branch
 *
 * Revision 3.13.4.8  1996/07/21  23:04:32  john
 * CSCdi63412:  IPX data being corrupted across EIP on 7513
 * Branch: California_branch
 *
 * Revision 3.13.4.7  1996/06/23  15:05:51  boyle
 * CSCdi61041:  Input acl not logged with the action permit
 * Branch: California_branch
 *
 * Revision 3.13.4.6  1996/05/04  02:18:46  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Enqueue to routable packet received on bridged interface to the
 *   appropriate queue instead of bridgeQ.
 * - Restore input to the BVI if necessary when copying packet for
 *   process-level.
 *
 * Name Access List
 *
 * Revision 3.13.4.5  1996/05/03  02:18:36  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.13.4.4  1996/04/27  06:47:47  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.5.16.3  1996/04/26  00:11:46  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.5.16.2  1996/04/17  00:09:26  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.5.16.1  1996/02/23  21:33:36  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.13.4.3  1996/04/17  13:52:25  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.13.4.2  1996/03/29  03:19:32  hou
 * CSCdi50046:  vLAN 802.10 supports only IPX Arpa encaps.
 * Branch: California_branch
 * - should support default novell-ether IPX encap.
 *
 * Revision 3.13.4.1  1996/03/18  22:53:28  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.3  1996/03/16  08:00:02  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.2.2  1996/03/13  02:14:50  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.2.1  1996/02/20  22:00:52  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.13  1996/03/09  18:13:31  mschaef
 * CSCdi46978:  IPX fastswitch cache can grow very large
 *
 * Revision 3.12  1996/03/07  06:49:17  shj
 * CSCdi50312:  IPX/LANE fastswitching on 7500, 4500 produces bogus packets
 * Make sure actual rounded packet length matches ethernet length in
 * in packet.
 *
 * Revision 3.11  1996/02/25  03:53:26  mmcneali
 * CSCdi49686:  IPX non-functional in gs7 images on ISL encapsulating
 *              subinterfaces.
 *
 * Revision 3.10  1996/02/23  09:43:58  dko
 * CSCdi49352:  Two word read misalignments at nov_fastswitch()
 * Fixed word misalignment problems.
 *
 * Revision 3.9  1996/02/13  19:44:33  hampton
 * Reorder routines to remove platform specific extern statements in
 * fastswitching code.  [CSCdi48844]
 *
 * Revision 3.8  1996/02/05  02:07:18  mmcneali
 * CSCdi46205:  IPX packets are corrupted by vLAN 802.10 fast-switch.
 *
 * Revision 3.7  1996/01/23  08:57:28  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 * Revision 3.6  1996/01/20  06:53:35  lbustini
 * CSCdi39980:  BOD not working on PRI when fastswitching is enabled
 * Use correct MIP vc number when selecting output interface.
 *
 * Revision 3.5  1995/12/01  06:26:00  mschaef
 * CSCdi37234:  Allow fastswitching of certain directed broadcast packets
 *
 * Revision 3.4  1995/12/01  03:11:37  shj
 * CSCdi44077:  IPX fastswitching to LANE on 4500 screws up packet length
 * Store correct length in type-or-len after copying encaps from cache.
 *
 * Revision 3.3  1995/11/26  08:24:14  mmcneali
 * CSCdi43630:  ISL VLAN Multicast changed--IOS gs7 images are DEAF
 *
 *              If subinterface configured as a vLAN do not permit another
 *              encapsulation.
 *
 *              Correct counters of vLAN packets switched.
 *
 * Revision 3.2  1995/11/17  19:23:38  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:59:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:39:23  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/08/10  23:31:40  wfried
 * CSCdi37608:  oam f5 cells generated by 4500 are not recognized by
 * analyzer
 * Changed ATM code to pass VC pointer for OAM cells and other related
 * changes
 *
 * Revision 2.4  1995/07/31  21:03:40  john
 * CSCdi36543:  Rconsole is time-out with 2 netware clients (SPX-Spoof)
 *
 * Revision 2.3  1995/07/18  04:16:31  shaker
 * CSCdi35990:  Need packet pool for fast switched packet structure
 * Use new pak_set_dirty() to only set PAK_BUF_DIRTY iff
 * PAK_BUF_USE_DIRTY is set. Set PAK_BUF_USE_DIRTY for RSP fs_pak
 *
 * Revision 2.2  1995/06/28  09:33:49  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:29:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Routines Exported from this module
 * novell_fastswitch:	Fast switches novell packets
 *
 * Both the above routines are called through a switch table "fast_switch"
 * using the RXTYPE_ codes as the index. ( The RXTYPE_ contains packet type
 * code.)
 */

#include "master.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "xns.h"
#include "novell.h"
#include "novfast.h"
#include "access.h"
#include "../if/network.h"
#include "../if/ether.h"
#include "../if/fddi.h"
#include "../wan/serial.h"
#include "../wan/ppp.h"
#include "../wan/dialer.h"
#include "../wan/dialer_private.h"
#include "../wan/frame_relay.h"
#include "../wan/atm_dxi.h"
#include "../wan/smds.h"
#include "../if/tring.h"
#include "../les/if_les.h"
#include "../if/atm.h"

#include "../tbridge/tbridge_sde.h"
#include "../vlan/vlan.h"

#include "../if/atm.h"
#include "../lane/lane.h"


 
/*
 * Layout of datagram sniff buffer
 */

/*
 *	     ARPA	   ISO1		  ISO2		  HDLC
 *
 *	  D  D  D  D	D  D  D  D	D  D  D  D
 *   	  D  D  S  S	D  D  S  S	D  D  S  S
 *	  S  S  S  S	S  S  S  S	S  S  S  S
 *   0	  P  P  x  x	L  L  DS SS	L  L  DS SS	B  B  P  P
 *   1	  x  x  x  x	CT x  x  x	CT SN SN SN	x  x  x  x
 *   2	  x  x  x  x	x  x  x  x	SN SN P  P	x  x  x  x
 *   3	  x  x  x  x	x  x  x  x	x  x  x  x	x  x  x  x
 *   4    x  x  x  x	x  x  x  x	x  x  x  x	x  x  x  x
 *   5    x  x 		x  x  x  x 	x  x  x  x	x  x  x  x
 *   6    		x		x  x  x  x
 *   7    				x  x  x  x
 */

/* 
 * definitions
 */
typedef struct shortlong_ {
    union {
	ulong lword;
	ushort sword[2];
    } d;

} shortlong;

/*
 * Macros to handle DDR fastswitching.
 */
#define DDR_SETNEXT(OUT, DDB)						\
    output_fast_switch_type = DDB->fs_type;                             \
    if (output->cache_board_encap != NULL) {				\
	/*								\
	 * Set proper MIP channel.					\
	 */								\
	(*output->cache_board_encap)(output, path->novp_mh.mac_bytes,	\
				     sizeof(ushort));			\
    }

#define DDR_CLASSIFY(OFFSET, RESUME)					\
    /*									\
     * Tell lower layers that we have groveled in the packet more than	\
     * normal for the fast switch path and the packet may need to be 	\
     * flushed out of the data cache for some platforms:		\
     */									\
    pak_set_dirty(pak);							\
    if (!nov_accesscheck(pak, dialerlist))				\
	goto RESUME;


#ifdef DEBUG
void
display_trace (hwidbtype *input,
	       hwidbtype *output,
	       novpathtype *path)
{
    if (!novell_debug)
	return;

    if (input->status & IDB_ETHER)
	buginf(" (e");
    else
	buginf(" (s");
    buginf("->");
    if (output->status & IDB_ETHER)
	buginf("e) ");
    else
	buginf("s) ");
    switch (input->rxtype) {
      case RXTYPE_NOVELL1:
	buginf("(nov1");
	break;
      case RXTYPE_NOVELL2:
	if (input->status & IDB_FDDI) {
	    buginf(" raw");
	} else {
	    buginf("(nov2");
	}
	break;
      case RXTYPE_SNAP_NOV1:
	buginf("(snap");
	break;
      case RXTYPE_SAP_NOVELL:
	buginf("(iso1");
	break;
      default:
	if (input->status & IDB_SERIAL) {
	    switch (input->enctype) {
	    case ET_HDLC:
		buginf("(hdlc");
		break;
	    case ET_PPP:
		buginf("( ppp");
		break;
	    case ET_FRAME_RELAY:
		buginf("(  fr");
		break;
	    case ET_ATM_DXI:
		buginf("(atm-");
		break;
	    case ET_SMDS:
		buginf("(smds");
		break;
	    default:
		buginf("(????");
		break;
	    }
	}
	break;
    }
    buginf("->%s)", encstring(path->novp_nidb_ptr->enctype));
}
#endif


/*
 * ipx_fast_supported
 *
 * Determine whether fastswitch support present for given hw/config
 *
 */
static boolean ipx_fast_supported (int linktype, hwidbtype *hwidb)
{
    if (!hwidb || (linktype != LINK_NOVELL)) {
	return(FALSE);
    }
    /*
     * Ethernet:
     */
    if (hwidb->status & IDB_ETHER) {
	return(TRUE);
    }
    /*
     * Fddi:
     */
    if (hwidb->status & IDB_FDDI) {
	return(TRUE);
    }
    /*
     * Tokenring:
     */
    if (hwidb->status & IDB_TR) {
	return(TRUE);
    }
    /*
     * Serial: MK5025, HD64570, M32 and BRI supported if encap is hdlc/ppp.
     *         fr only if ddr is *not* configured
     */
    if (hwidb->status & IDB_SERIAL) {
	if ((hwidb->enctype == ET_HDLC) ||
	     is_ppp(hwidb) ||
	     (is_frame_relay(hwidb) && !is_ddr(hwidb)) ||
	     is_frame_relay(hwidb) ||
	     is_atm_dxi(hwidb) ||
	     is_smds(hwidb)) {
	    return(TRUE);
	}
    }
    /*
     * ATM: all flavors
     */
    if (is_atm(hwidb)) {
	return(TRUE);
    }

    if (hwidb->status & IDB_LEX) {
	return(TRUE);
    }

    /*
     * Bridge-group Virtual Interface - for IRB
     * (Integrated Routing and Bridging)
     */
    if (is_bvi(hwidb)) {
	return (TRUE);
    }

    return(FALSE);
}

static inline void copy_cache (ushort cache_len, ulong *mac, ulong *cache)
{
    register ushort compare;

    compare = sizeof(ulong);

    while (cache_len >= compare) {
	PUTLONG(mac++, GETLONG(cache++));
	cache_len -= compare;
    }
    if (cache_len) {
	PUTSHORT(mac, GETSHORT(cache));
    }
}

static inline void copy_cache_maybe_odd (ushort cache_len, ulong *mac, ulong *cache)
{
    register ushort compare;

    compare = sizeof(ulong);

    while (cache_len >= compare) {
	PUTLONG(mac++, GETLONG(cache++));
	cache_len -= compare;
    }
    if (cache_len) {
	ushort *macs, *caches;
	macs = (ushort *)mac;
	caches = (ushort *)cache;
	if (cache_len & 0x2)
	    PUTSHORT(macs++, GETSHORT(caches++));
	if (cache_len & 0x01)
	    *(uchar *)macs = *(uchar *)caches;
    }
}
    

inline static boolean inline_item_denied (xnseacctype *item,
 					  int list,
 					  uchar proto,
 					  ulong dst_net,
 					  ulong dst_host_1long,
 					  ushort dst_host_2short,
 					  ushort dst_socket,
 					  ulong src_net,
 					  ulong src_host_1long,
 					  ushort src_host_2short,
 					  ushort src_socket)
{
    ulong flags;

    do {
	flags = item->flags;
       	/*
	 * A network of -1 indicates all XNS networks.
	 * A socket of 0 indicates all XNS sockets.
	 */
	if (flags & XNS_SRCNET) { 
	    if (item->srcnet != NOVELL_TYPE_ANY_NET) {
		if (flags & XNS_SRCNETMASK) {
		    if (item->srcnet != (src_net & item->srcnetmask))
			continue;
		} else {
		    if (item->srcnet != src_net)
			continue;
		}
	    }
	}
	if (flags & XNS_DSTNET) { 
	    if (item->dstnet != NOVELL_TYPE_ANY_NET) {
	      if (flags & XNS_DSTNETMASK) {
		if (item->dstnet != (dst_net & item->dstnetmask))
		  continue;
	      } else {
		if (item->dstnet != dst_net)
		  continue;
	      }
	    }
	}
	if (flags & XNS_SRCHOST) {
	    /*
	     * Check for match on source host address
	     */
	    if (flags & XNS_SRCHOSTMASK) {
		if ((src_host_2short & item->srchostmask.u.sword[2])
		    != item->srchost.u.sword[2])
		    continue;
		if ((src_host_1long & item->srchostmask.u.lword[0])
		    != (item->srchost.u.lword[0]))
		    continue;
	    } else {
		if (src_host_2short != item->srchost.u.sword[2])
		    continue;
		if (src_host_1long != item->srchost.u.lword[0])
		    continue;
	    }
	}
	if (flags & XNS_DSTHOST) {
	    /*
	     * Check for match on source host address
	     */
	  if (flags & XNS_DSTHOSTMASK) {
	    if ((dst_host_2short & item->dsthostmask.u.sword[2])
		!= item->dsthost.u.sword[2])
		continue;
	    if ((dst_host_1long & item->dsthostmask.u.lword[0])
		!= (item->dsthost.u.lword[0]))
		continue;
	  } else {
	    if (dst_host_2short != item->dsthost.u.sword[2])
	      continue;
	    if (dst_host_1long != item->dsthost.u.lword[0])
	      continue;
	  }
	}

	/* check if it is extended access list */
	if (list > MAXNOVACC ) {
	    if (flags & XNS_PROTOCOL) {
		if ((item->protocol != (ushort) -1) &&
		    (item->protocol != proto))
		  continue;
	    }
	    if (flags & XNS_SRCSOCKET) {
		if (item->srcsocket && (item->srcsocket != src_socket))
		  continue;
	    }
	    if (flags & XNS_DSTSOCKET) {
		if(item->dstsocket && (item->dstsocket != dst_socket))
		    continue;
	    }
            if (item->log)
                return(TRUE);
	}

#ifdef DEBUG
if ((!item->grant) && novell_debug) buginf("*deny*");
#endif

	return(!item->grant);
	
    } while ((item = item->next));

#ifdef DEBUG
if (novell_debug) buginf("**deny**");
#endif

    /* Got here -- access list says implied deny */
    return(TRUE); /* Return "item_denied" */
}

static boolean item_denied (xnseacctype *item,
			    int list,
			    uchar proto,
			    ulong dst_net,
			    ulong dst_host_1long,
			    ushort dst_host_2short,
			    ushort dst_socket,
			    ulong src_net,
			    ulong src_host_1long,
			    ushort src_host_2short,
			    ushort src_socket)
{
    return(inline_item_denied(item,
			      list,
			      proto,
			      dst_net,
			      dst_host_1long,
			      dst_host_2short,
			      dst_socket,
			      src_net,
			      src_host_1long,
			      src_host_2short,
			      src_socket));
}

/*
 * nov_access_check
 * Determine if we are allowed to forward the passed datagram, based on the
 * NOVELL access pointer. Return FALSE if output access is denied.
 *
 * Also do accounting work.
 */
boolean nov_acct_and_access_check_pan (paktype *pak,
				       ipxhdrtype *ipx,
				       idbtype	  *swoutput,
				       boolean    bcast)
{
    xnseacctype *item = NULL;
    int list = 0;
    uchar proto;
    ulong src_net, src_host_1long, dst_net, dst_host_1long;
    ushort src_host_2short, dst_host_2short, dst_socket, src_socket;

    proto = ipx->pt;
    dst_net = GETLONG(ipx->ddnet);
    dst_host_1long = GETLONG(ipx->dhost);
    dst_host_2short = GETSHORT(&ipx->dhost[4]); 
    dst_socket = GETSHORT(&ipx->dsock); 

    src_net = GETLONG(ipx->ssnet);
    src_host_1long = GETLONG(ipx->shost);
    src_host_2short = GETSHORT(&ipx->shost[4]); 
    src_socket = GETSHORT(&ipx->ssock); 


    /*
     * Validate ALL packets against input filter:
     */
    list = pak->if_input->novell_inaccesslist; /* %%% */
    item = ipx_get_ext_acl(list);

#ifdef DEBUG
    if (novell_debug) if (item) buginf("(in)");
#endif

    if (item &&
	item_denied(item, list, proto,
		    dst_net, dst_host_1long, dst_host_2short, dst_socket,
		    src_net, src_host_1long, src_host_2short, src_socket)) {
	return(FALSE);
    }

#ifdef DEBUG
    if (novell_debug) if (item) buginf("(ok)");
#endif

    if (bcast) {
	
#ifdef DEBUG
 	if (novell_debug) buginf(" (bc)");
#endif
  	/*
  	 * Screen out ineligible entries:
  	 */
  	if ((dst_net == 0) ||
  	    (dst_net == NOVELL_TYPE_ANY_NET) ||
  	    ((ipx->pt == NOVELL_TYPE_NETBIOS) && !novell_type20_helpered) ||
  	    (dst_socket == NOVELL_SOCK_RIP) ||
  	    (dst_socket == NOVELL_SOCK_SAP) ||
  	    (dst_socket == NOVELL_SOCK_NLSP) ||
 	    (dst_socket == NOVELL_SOCK_IGRP)) {
	    
#ifdef DEBUG
  	    if (novell_debug) buginf("*bc deny*");
#endif
  	    return(FALSE);
  	}
    }
    
    /*
     * Validate traditional data packets against access-group filter:
     */
    list = swoutput->novell_accesslist;
    item = ipx_get_ext_acl(list);
    
#ifdef DEBUG
    if (novell_debug) if (item) buginf("(out)");
#endif

    if (item &&
	item_denied(item, list, proto, dst_net,
		    dst_host_1long, dst_host_2short,
		    dst_socket, src_net, src_host_1long,
		    src_host_2short, src_socket)) {
	return(FALSE);
    }

#ifdef DEBUG
    if (novell_debug) if (item) buginf("(ok)");
#endif
    
    if (swoutput->ipx_accounting)
	return(nov_fast_acct_do_work(src_net, dst_net, src_host_1long,
				     src_host_2short, dst_host_1long,
				     dst_host_2short, GETSHORT(&ipx->len)));
    return(TRUE);
}

/*
 * nov_fastswitch()
 *
 * The code below has been lovingly hand-crafted to provide a lifetime of
 * reliable use and happiness. ;-)
 *
 * Don't change this stuff unless you're prepared to spend time pouring
 * over hot and sweaty assembler listings and HP analyzer timing output...
 */

/*
 * nov_fastswitch
 */
boolean nov_fastswitch (paktype *pak)
{
    novcachetype *cache;
    novpathtype *path;
    hwidbtype *input, *output;
    idbtype *swoutput;
    idbtype *swinput;
    shortlong net, addrhigh;
    ushort addrlow, temp_length;
    int enctype;
    ipxhdrtype *ipx;
    ulong *macptr, *cache_macptr, indx;
    ushort *macsptr, *cache_macsptr;
    int i;
    enum FSTYPE output_fast_switch_type;

    /*
     * Get pointer to IPX header based on input encapsulation:
     */
    input = pak->if_input->hwptr;
    pak->linktype = LINK_NOVELL;
    ipx = (ipxhdrtype *)ipxheadstart(pak);

    /* Sanity check the ipx length in the ipx header */
    if (GETSHORT(&ipx->len) > ((ulong) pak->datagramsize - 
		    ((ulong) ipx - (ulong) pak->datagramstart)))
      return(FALSE);

    /* If spx spoofing is enabled, force processing in the slow path */
    if (is_ddr(input)) {
      /* get the dialer hw interface */
      hwidbtype *netidb;

      netidb = get_netidb(input);
      if (netidb->novell_spxspoof && 
	  (ipx->pt == NOVELL_TYPE_SPX))
	return(FALSE);
    } else {
      if (input->novell_spxspoof && (ipx->pt == NOVELL_TYPE_SPX))
	return(FALSE);
    }
    /* Check if hop count valid (including reserved tc bits) */
    if (ipx->tc < novell_max_hops) {

      nov_hop_count_ok:
	net.d.lword      =  GETLONG(ipx->ddnet);
	addrhigh.d.lword =  GETLONG(ipx->dhost);
	addrlow          = GETSHORT(ipx->dhost+4);

	indx = net.d.lword;
	indx ^= addrhigh.d.lword;
	indx ^= addrlow;
	indx ^= (indx >> 16);
	indx ^= (indx >> 8);
	indx &=0xFF;

	for (cache = novcache[indx]; cache; cache = cache->novc_next_ptr) {
	    if ((*(ushort *)&cache->novc_dest_address[4] == addrlow) &&
		(*(ulong *)cache->novc_dest_address == addrhigh.d.lword) &&
		(cache->novc_dest_net == net.d.lword)) {
		if (cache->novc_version == novcache_version) {
  		    if (cache->novc_active) {
  		      nov_resume_active:
			path = cache->novc_path_next_ptr;
			swoutput = path->novp_idb_ptr;
			output = swoutput->hwptr;
			
			/* Check total size; dump to slow switch if too big */
			if ((ulong) (GETSHORT(&ipx->len) +
				     path->total_encap_bytes) >
			    output->max_pak_size) {
			    return(FALSE);
			}
			
			/*
			 * Remote routes round-robin if multiple cache
			 * entries exist:
			 */
			cache->novc_path_next_ptr = path->novp_next_ptr;
			
			/*
			 * Check if an access list is enabled on *any* interface.
			 * If so, check if *this* interface. If so, filter.
			 */
			if (novell_global_acl_checks || cache->novc_bcast) {
			    if (swoutput->novell_output_acl_checks ||
				input->novell_input_acl_checks ||
				cache->novc_bcast) {
				/*
				 * Tell lower layers that we have groveled in the
				 * packet more than normal for the fast switch path
				 * and the packet may need to be flushed out of the
				 * data cache for some platforms:
				 */
				pak_set_dirty(pak);
				
				/*
				 * Run input swidb match iff input-access-group:
				 */
				if (input->novell_input_acl_checks) {
				    if (input->status & IDB_IEEEMASK) {
					FOR_ALL_SWIDBS_ON_HW(input, swinput) {
					    if (swinput->novellnets &&
						(swinput->primary_nidb->enctype ==
						 pak->enctype)) {
						pak->if_input = swinput;
						break;
					    }
					}
				    }
				}
				if (!nov_acct_and_access_check_pan
				    (pak, ipx, swoutput, cache->novc_bcast))
				    return(FALSE);
			    }
			}
			
			/*
			 * Do some accounting of packets and bytes
			 * in and out.
			 */
			novell_stat[NOVELL_INPUT]++;
			novell_stat[NOVELL_FORWARDED]++;
			
			/*
			 * Bail if source net is 0:
			 */
			if (GETLONG(ipx->ssnet) == 0)
			    return(FALSE);
			
			input->counters.inputs_fast[ACCT_PROTO_NOVELL]++;
			input->counters.rx_cumbytes_fast[ACCT_PROTO_NOVELL] += pak->datagramsize;
			pak->acct_proto = ACCT_PROTO_NOVELL;
			/*
			 * Update the hop count:
			 */
			if (novell_igrp_running == 0 ||
			    !NOVELL_IGRP_HOPCOUNT_ALGORITHM_ALLOWED) {
			    /* Always do 4 -> 4 if IGRP not running on this box */
			    ipx->tc += 1;
			} else if (ipx->tc <= XNS_MAX_HOPS) {
			    if (path->igrp_route) {
				/* 4 -> 8 */
				if (ipx->tc == 0)
				    ipx->tc = 1;
				ipx->tc += 1;
				if (ipx->tc <= XNS_MAX_HOPS)
				    ipx->tc = (ipx->tc * 16);
			    } else {
				/* 4 -> 4 */
				ipx->tc += 1;
			    }
			} else {
			    if (path->igrp_route) {
				/* 8 -> 8 */
				ipx->tc += 1;
			    } else {
				/* 8 -> 4 */
				ipx->tc = (ipx->tc / 16);
			    }
			}
			temp_length = GETSHORT(&ipx->len);
			output_fast_switch_type = output->fast_switch_type;
			
			/*
			 * Re entry point for DDR fastswitching.  output now points
			 * to the physical output interface.
			 */
		      resume_dialer:
			
			switch (output_fast_switch_type) {
			    
			  case FS_ETHER:
			    
			    enctype = path->novp_nidb_ptr->enctype;
			    
			    if (enctype == ET_SAP) {
				pak->datagramstart =
				    ipxheadstart(pak) - ETHER_SAP_ENCAPBYTES;
				
				macptr = (ulong *)pak->datagramstart;
				cache_macptr = path->novp_mh.mac_longs;
				
				/*
				 * Write the DA and SA
				 */
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				
				/*
				 * Write the IEEE802.3 length
				 */
				macsptr = (ushort *)macptr;
				PUTSHORT(macsptr++, ROUND(temp_length + SAP_HDRBYTES));
				
				/*
				 * Write the LLC header
				 */
				PUTSHORT(macsptr++, SAPSAP_NOVELL);
				*(uchar *)macsptr = LLC1_CLASS_U;
				
				temp_length += ETHER_SAP_ENCAPBYTES;
				
			    } else if (enctype == ET_SNAP) {
				pak->datagramstart =
				    ipxheadstart(pak) - ETHER_SNAP_ENCAPBYTES;
				
				macptr = (ulong *)pak->datagramstart;
				cache_macptr = path->novp_mh.mac_longs;
				
				/*
				 * Write the DA and SA
				 */
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				
				/*
				 * Write the IEEE802.3 length
				 */
				macsptr = (ushort *)macptr;
				PUTSHORT(macsptr++, ROUND(temp_length + SNAP_HDRBYTES));
				
				/*
				 * Write the SNAP header
				 */
				PUTSHORT(macsptr++, SNAPSNAP);
				macptr = (ulong *)macsptr;
				PUTLONG(macptr++, OLD_ETHEROUI);
				macsptr = (ushort *)macptr;
				PUTSHORT(macsptr++, TYPE_NOVELL1);
				
				temp_length += ETHER_SNAP_ENCAPBYTES;
				
			    } else if (enctype == ET_ISL) {
				
				/* ISL vLAN encapsulation */
				
				pak->datagramstart = ipxheadstart(pak) -
				    (ISL_HEADER_LENGTH + ETHER_8023_ENCAPBYTES);
				
				macptr = (ulong *)pak->datagramstart;
				cache_macptr = path->novp_mh.mac_longs;
				
				/*
				 * Write the exterior ISL header, followed
				 * by the Novell-Ethernet format, for now.
				 *
				 * Mark the packet as requiring ISL Inner CRC
				 * generation; done by the FEIP ucode until the
				 * MICE hardware assist is available at which
				 * time the "PAK_DO_ISL_CRC" becomes redundant.
				 */
				
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				
				cache_macsptr = (ushort *) cache_macptr;
				macsptr       = (ushort *) macptr;
				
				PUTSHORT(macsptr++, *cache_macsptr);
				PUTSHORT(macsptr++, ROUND(temp_length));
				PUTSHORT(macsptr, NOVELL_NO_CHECKSUM);
				
				temp_length += (ETHER_8023_ENCAPBYTES +
						ISL_HEADER_LENGTH);
				
				pak->flags |= PAK_DO_ISL_CRC;
				
				swoutput->isl_vlan->
				    egressing_isl_packets[VLAN_ROUTE_NOVELL]++;
				
			    } else if (enctype == ET_SDE) {
				
				/* 802.10 vLAN encapsulation, maybe with SNAP */
				
				if (path->total_encap_bytes == SDE_ENCAPBYTES +
				    ETHER_8023_ENCAPBYTES) {
				    
				    pak->datagramstart = ipxheadstart(pak) -
					(SDE_ENCAPBYTES + ETHER_8023_ENCAPBYTES);
				    
				    temp_length += (ETHER_8023_ENCAPBYTES +
						    SDE_ENCAPBYTES);
				} else {
				    
				    pak->datagramstart = ipxheadstart(pak) -
					(SDE_ENCAPBYTES + ETHER_SNAP_ENCAPBYTES);
				    
				    temp_length += (ETHER_SNAP_ENCAPBYTES +
						    SDE_ENCAPBYTES);
				}
				
				macptr = (ulong *)pak->datagramstart;
				
				cache_macptr = path->novp_mh.mac_longs;
				
				write_ether_dot10_cache_macheader(
								  macptr, cache_macptr,
								  (temp_length -
								   ETHER_8023_ENCAPBYTES),
								  path->total_encap_bytes);
				
				swoutput->sde_said_db->
				    egressing_dot10_packets[VLAN_ROUTE_NOVELL]++;
				
			    } else {
				pak->datagramstart =
				    ipxheadstart(pak) - ETHER_ARPA_ENCAPBYTES;
				
				macptr = (ulong *)pak->datagramstart;
				cache_macptr = path->novp_mh.mac_longs;
				
				/*
				 * Write the DA and SA
				 */
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				
				/*
				 * Write either the Novell Type or IEEE802.3 length
				 * (the latter is the 0xFFFF bogon Novell encap).
				 */
				macsptr = (ushort *)macptr;
				if (enctype == ET_ARPA) {
				    PUTSHORT(macsptr, TYPE_NOVELL1);
				} else {
				    PUTSHORT(macsptr++, ROUND(temp_length));
				    PUTSHORT(macsptr, NOVELL_NO_CHECKSUM);
				}
				
				temp_length += ETHER_ARPA_ENCAPBYTES;	
			    }
			    
			    if (temp_length < MINETHERSIZE)
				temp_length = MINETHERSIZE;
			    else
				temp_length = ROUND(temp_length);
			    
			    pak->datagramsize = temp_length;
			    
			    pak->if_output = swoutput;
			    (*output->fastsend)(output, pak);
			    return(TRUE);
			    
			  case FS_FDDI:
			    
			    enctype = path->novp_nidb_ptr->enctype;
			    
			    if (enctype == ET_SAP) {
				pak->datagramstart =
				    ipxheadstart(pak) - path->total_encap_bytes;
				
				/*
				 * Write the FC control byte
				 */
				*pak->datagramstart = FDDI_ASYNCHRONOUS_FRAME |
				    FDDI_48BIT_ADDRESS |
					FDDI_LLC_FRAME |
					    FDDI_PRIORITY_ZERO;
				macptr = (ulong *)((fddi_hdr *)pak->datagramstart)->daddr;
				cache_macptr = path->novp_mh.mac_longs;
				
				/*
				 * Write the DA and SA
				 */
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				
				/*
				 * Write the RIF, if required
				 */
				macsptr = (ushort *)macptr;
				if (path->rifwords > 0) {
				    cache_macsptr = (ushort *)cache_macptr;
				    for (i = 0; i < path->rifwords; i++) {
					PUTSHORT(macsptr++, *cache_macsptr++);
				    }
				}
				
				/*
				 * Write the LLC header
				 */
				PUTSHORT(macsptr++, SAPSAP_NOVELL);
				*(uchar *)macsptr = LLC1_CLASS_U;
				
				pak->datagramsize = temp_length + path->total_encap_bytes;
			    } else if (enctype == ET_SNAP) {
				pak->datagramstart =
				    ipxheadstart(pak) - path->total_encap_bytes;
				
				/*
				 * Write the FC control byte
				 */
				*pak->datagramstart = FDDI_ASYNCHRONOUS_FRAME |
				    FDDI_48BIT_ADDRESS |
					FDDI_LLC_FRAME |
					    FDDI_PRIORITY_ZERO;
				macptr = (ulong *)((fddi_hdr *)pak->datagramstart)->daddr;
				cache_macptr = path->novp_mh.mac_longs;
				
				/*
				 * Write the DA and SA
				 */
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				
				/*
				 * Write the RIF, if required
				 */
				macsptr = (ushort *)macptr;
				if (path->rifwords > 0) {
				    cache_macsptr = (ushort *)cache_macptr;
				    for (i = 0; i < path->rifwords; i++) {
					PUTSHORT(macsptr++, *cache_macsptr++);
				    }
				    cache_macptr = (ulong *)cache_macsptr;
				}
				/*
				 * Write the SNAP header
				 */
				macptr = (ulong *)macsptr;
				
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				pak->datagramsize = temp_length +
				    path->total_encap_bytes;
				
			    } else if (enctype == ET_NOVELL_FDDI) {
				pak->datagramstart =
				    ipxheadstart(pak) - path->total_encap_bytes;
				
				/*
				 * Write the FC control byte
				 */
				*pak->datagramstart = FDDI_ASYNCHRONOUS_FRAME |
				    FDDI_48BIT_ADDRESS |
					FDDI_LLC_FRAME |
					    FDDI_PRIORITY_ZERO;
				macptr = (ulong *)((fddi_hdr *)pak->datagramstart)->daddr;
				cache_macptr = path->novp_mh.mac_longs;
				
				/*
				 * Write the DA and SA
				 */
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				
				/*
				 * Write the RIF, if required
				 */
				macsptr = (ushort *)macptr;
				if (path->rifwords > 0) {
				    cache_macsptr = (ushort *)cache_macptr;
				    for (i = 0; i < path->rifwords; i++) {
					PUTSHORT(macsptr++, *cache_macsptr++);
				    }
				    cache_macptr = (ulong *)cache_macsptr;
				}
				/*
				 * Write no header whatsoever:
				 */
				pak->datagramsize = temp_length + path->total_encap_bytes;
				
			    } else if (enctype == ET_SDE) {
				
				/* IEEE 802.10 vLAN on FDDI, maybe with SNAP */
				
				pak->datagramstart = ipxheadstart(pak) -
				    path->total_encap_bytes;
				/*
				 * Write the FC control byte.
				 */
				
				*pak->datagramstart = FDDI_ASYNCHRONOUS_FRAME |
				    FDDI_48BIT_ADDRESS |
					FDDI_LLC_FRAME |
					    FDDI_PRIORITY_ZERO;
				macptr = (ulong *)
				    ((fddi_hdr *)pak->datagramstart)->daddr;
				
				cache_macptr = path->novp_mh.mac_longs;
				
				if (path->total_encap_bytes == (FDDI_SNAP_ENCAPBYTES
				    + SDE_ENCAPBYTES - SNAP_HDRBYTES)) 
				  /* write DA + SA + SDE */
				  {
				    PUTLONG(macptr++, *cache_macptr++);
				    PUTLONG(macptr++, *cache_macptr++);
				    PUTLONG(macptr++, *cache_macptr++);
				    PUTLONG(macptr++, *cache_macptr++);
				    PUTLONG(macptr++, *cache_macptr++);
				    PUTLONG(macptr++, *cache_macptr++);
				    PUTLONG(macptr++, *cache_macptr++);
    

				  } else if (path->total_encap_bytes == (FDDI_SNAP_ENCAPBYTES
									 + SDE_ENCAPBYTES)) {    
				      write_dot10_cache_macheader(macptr, cache_macptr, TRUE);

				  } else  
				      write_dot10_cache_macheader(macptr, cache_macptr, FALSE);
				
				pak->datagramsize = temp_length +
				    path->total_encap_bytes;
				
				swoutput->sde_said_db->
				    egressing_dot10_packets[VLAN_ROUTE_NOVELL]++;
			    }
			    
			    pak->if_output = swoutput;
			    (*output->fastsend)(output, pak);
			    
			    return(TRUE);
			    
			  case FS_CHANNEL:
			    /*
			     * Fast Switch to Channel output interface:
			     */
			    pak->datagramstart =
				ipxheadstart(pak) - path->total_encap_bytes;
			    pak->datagramsize =
				temp_length + path->total_encap_bytes;
			    bcopy(path->novp_mh.mac_longs, pak->datagramstart,
				  path->total_encap_bytes);
			    
			    pak->if_output = swoutput;
			    (*output->fastsend)(output, pak);
			    return(TRUE);
			    
			  case FS_TOKEN:
			    
			    enctype = path->novp_nidb_ptr->enctype;
			    
			    if (enctype == ET_SAP) {
				pak->datagramstart =
				    ipxheadstart(pak) - path->total_encap_bytes;
				
				/*
				 * Write the AC and FC control bytes
				 */
				PUTSHORT(pak->datagramstart,
					 ((AC_PRI0 << 8) | FC_TYPE_LLC));
				macptr = (ulong *)((tring_hdr *)pak->datagramstart)->daddr;
				cache_macptr = path->novp_mh.mac_longs;
				
				/*
				 * Write the DA and SA
				 */
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				
				/*
				 * Write the RIF, if required
				 */
				macsptr = (ushort *)macptr;
				if (path->rifwords > 0) {
				    cache_macsptr = (ushort *)cache_macptr;
				    for (i = 0; i < path->rifwords; i++) {
					PUTSHORT(macsptr++, *cache_macsptr++);
				    }
				}
				
				/*
				 * Write the LLC header
				 */
				PUTSHORT(macsptr++, SAPSAP_NOVELL);
				*(uchar *)macsptr = LLC1_CLASS_U;
				
				pak->datagramsize = temp_length + path->total_encap_bytes;
			    } else if (enctype == ET_SNAP) {
				pak->datagramstart =
				    ipxheadstart(pak) - path->total_encap_bytes;
				
				/*
				 * Write the AC and FC control bytes
				 */
				PUTSHORT(pak->datagramstart,
					 ((AC_PRI0 << 8) | FC_TYPE_LLC));
				macptr = (ulong *)((tring_hdr *)pak->datagramstart)->daddr;
				cache_macptr = path->novp_mh.mac_longs;
				
				/*
				 * Write the DA and SA
				 */
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				
				/*
				 * Write the RIF, if required
				 */
				if (path->rifwords > 0) {
				    macsptr = (ushort *)macptr;
				    cache_macsptr = (ushort *)cache_macptr;
				    for (i = 0; i < path->rifwords; i++) {
					PUTSHORT(macsptr++, *cache_macsptr++);
				    }
				    macptr = (ulong *)macsptr;
				    cache_macptr = (ulong *)cache_macsptr;
				}
				
				/*
				 * Write the SNAP header
				 */
				PUTLONG(macptr++, GETLONG(cache_macptr++));
				PUTLONG(macptr++, GETLONG(cache_macptr++));
				
				pak->datagramsize = temp_length +
				    path->total_encap_bytes;
				
			    } else if (enctype == ET_SDE) {
				
				/* IEEE 802.10 vLAN encapsulation maybe with SNAP */
				
				pak->datagramstart = ipxheadstart(pak) -
				    path->total_encap_bytes;
				/*
				 * Write the AC and FC control bytes.
				 */
				
				PUTSHORT(pak->datagramstart,
					 ((AC_PRI0 << 8) | FC_TYPE_LLC));
				
				macptr = (ulong *)
				    ((tring_hdr *)pak->datagramstart)->daddr;
				
				cache_macptr = path->novp_mh.mac_longs;
				
				
				if (path->total_encap_bytes == FDDI_SAP_ENCAPBYTES +
				    SDE_ENCAPBYTES -
				    TR_ACFC_SIZE)
				    
				    write_dot10_cache_macheader(macptr,
								cache_macptr,
								FALSE);
				else
				    
				    write_dot10_cache_macheader(macptr,
								cache_macptr,
								TRUE);
				
				pak->datagramsize = temp_length +
				    path->total_encap_bytes;
				
				swoutput->sde_said_db->
				    egressing_dot10_packets[VLAN_ROUTE_NOVELL]++;
			    }
			    
			    pak->if_output = swoutput;
			    (*output->fastsend)(output, pak);
			    
			    return(TRUE);
			    
			  case FS_HDLC:
			  case FS_PPP:
			    pak->datagramstart =
				ipxheadstart(pak) - path->total_encap_bytes;
			    pak->datagramsize =
				temp_length + path->total_encap_bytes;
			    /*
			     * Write the header ... assume minimum 4 bytes:
			     */
			    PUTLONG(pak->datagramstart, path->novp_mh.mac_longs[0]);
			    if (path->total_encap_bytes > HDLC_ENCAPBYTES) {
				i = path->total_encap_words -
				    btow(HDLC_ENCAPBYTES);
				macsptr =
				    (ushort*)pak->datagramstart+
					btow(HDLC_ENCAPBYTES);
				cache_macsptr =
				    (ushort*)&path->novp_mh.mac_longs[1];
				while (i-- > 0) {
				    PUTSHORT(macsptr++, *cache_macsptr++);
				}
			    }
			    pak->if_output = swoutput;
			    (*output->fastsend)(output, pak);
			    return(TRUE);
			    
			  case FS_FR:
			    pak->datagramstart =
				ipxheadstart(pak) - path->total_encap_bytes;
			    pak->datagramsize =
				temp_length + path->total_encap_bytes;
			    /*
			     * Write the header ... assume minimum 4 bytes:
			     */
			    copy_cache(path->total_encap_bytes,
				       (ulong *)pak->datagramstart,
				       path->novp_mh.mac_longs);
			    INC_PVC_FAST_OUTPUT(HWIDB_TO_FR_PVC(output,
								path->subint_reference_num),
						pak->datagramsize);
			    pak->if_output = swoutput;
			    (*output->fastsend)(output, pak);
			    return(TRUE);
			    
			  case FS_ATM_DXI:
			    pak->datagramsize =
				temp_length + path->length;
			    pak->datagramstart =
				ipxheadstart(pak) - path->length;
			    copy_cache(path->total_encap_bytes,
				       (ulong *)pak->datagramstart,
				       path->novp_mh.mac_longs);
			    INC_DXI_PVC_FAST_OUTPUT(HWIDB_TO_DXI_PVC(output,
								     path->subint_reference_num),
						    pak->datagramsize);
			    pak->if_output = swoutput;
			    (*output->fastsend)(output, pak);
			    return(TRUE);
			    
			  case FS_SMDS:
			    {
				smdstype *sm;
				smds_pdb_t *spd;
				uint len,pad;
				uchar *trailptr;
				
				spd = output->smds_pdb;
				pak->datagramstart =
				    ipxheadstart(pak) - path->total_encap_bytes;
				/*
				 * write the encap header
				 */
				copy_cache(path->total_encap_bytes,
					   (ulong *)pak->datagramstart,
					   path->novp_mh.mac_longs);
				/*
				 * Calculate the trailer pointer.  Calculate
				 * the number of pad bytes and the other
				 * smds per packet fields
				 */
				len = temp_length;
				trailptr = ipxheadstart(pak) + len;
				pad = 3 - ((len + 3) % 4);
				if (pad) {
				    PUTLONG(trailptr, 0);
				    trailptr += pad;
				    len	 += pad;
				}
				sm = (smdstype *)(pak->datagramstart +
						  spd->fast_output_modelen);
				sm->smds_xhlpi &= ~3;
				sm->smds_xhlpi |= pad;
				sm->smds_betag = output->smds_begin_tag++;
				len += path->total_encap_bytes;
				PUTSHORT(&sm->smds_basize,
					 (len - (sizeof(ulong) +
						 spd->fast_output_modelen)));
				pak->datagramsize = len + sizeof(ulong);
				/* trailer copy */
				PUTLONG(trailptr, GETLONG(&sm->smds_rsvd));
				pak->if_output = swoutput;
				(*output->fastsend)(output,pak);
				return(TRUE);
			    }
			    
			  case FS_AIP:
			    /*
			     * Fast Switch to ATM
			     */
			    pak->atm_vcid = path->novp_mh.mac_shorts[0];
			    pak->atm_enctype = path->novp_mh.mac_shorts[1];
			    pak->datagramsize =
				temp_length + path->total_encap_bytes;
			    pak->datagramstart =
				ipxheadstart(pak) - path->total_encap_bytes;
			    copy_cache_maybe_odd(path->total_encap_bytes,
						 (ulong *)pak->datagramstart,
						 path->novp_mh.mac_longs);
			    /*
			     * for LANE, must insert correct "ethernet" length
			     * for all encaps except ARPA, and force cksum to
			     * FFFF for NOVELL_ETHER encap.
			     */
			    enctype = path->novp_nidb_ptr->enctype;
			    if (lec_enabled(swoutput)) {
				if (enctype != ET_ARPA) {
				    ether_hdr   *ehead =
					(ether_hdr *)(pak->datagramstart +
						      ATM_AAL5_LANE_ENCAPBYTES);
				    
				    PUTSHORT(&ehead->type_or_len,
					     ROUND(pak->datagramsize -
						   ETHER_ARPA_ENCAPBYTES - 
						   ATM_AAL5_LANE_ENCAPBYTES));
				    if (enctype == ET_NOVELL_ETHER) {
					PUTSHORT(&ehead->data, NOVELL_NO_CHECKSUM);
				    }
				}
				pak->datagramsize = ROUND(pak->datagramsize);
			    }
			    pak->if_output = swoutput;
			    (*output->fastsend)(output, pak);
			    return(TRUE);
			    
			  case FS_LEX:
			    enctype = path->novp_nidb_ptr->enctype;
			    
			    if (enctype == ET_SAP) {
				pak->datagramstart =
				    ipxheadstart(pak) - path->length;
				
				macsptr = (ushort *)pak->datagramstart;
				cache_macsptr = path->novp_mh.mac_shorts;
				
				/*
				 * Write Lex Header.
				 */
				PUTSHORT(macsptr++, *cache_macsptr++);
				PUTSHORT(macsptr++, *cache_macsptr++);
				PUTSHORT(macsptr++, *cache_macsptr++);
				
				/*
				 * Write the DA and SA
				 */
				PUTSHORT(macsptr++, *cache_macsptr++);
				PUTSHORT(macsptr++, *cache_macsptr++);
				PUTSHORT(macsptr++, *cache_macsptr++);
				PUTSHORT(macsptr++, *cache_macsptr++);
				PUTSHORT(macsptr++, *cache_macsptr++);
				PUTSHORT(macsptr++, *cache_macsptr++);
				
				/*
				 * Write the IEEE802.3 length
				 */
				PUTSHORT(macsptr++, ROUND(temp_length + SAP_HDRBYTES));
				
				/*
				 * Write the LLC header
				 */
				PUTSHORT(macsptr++, SAPSAP_NOVELL);
				*(uchar *)macsptr = LLC1_CLASS_U;
				
				temp_length += path->length;
			    } else if (enctype == ET_SNAP) {
				pak->datagramstart =
				    ipxheadstart(pak) - path->length;
				
				macsptr = (ushort *)pak->datagramstart;
				cache_macsptr = path->novp_mh.mac_shorts;
				
				/*
				 * Write Lex Header.
				 */
				PUTSHORT(macsptr++, *cache_macsptr++);
				PUTSHORT(macsptr++, *cache_macsptr++);
				PUTSHORT(macsptr++, *cache_macsptr++);
				
				/*
				 * Write the DA and SA
				 */
				PUTSHORT(macsptr++, *cache_macsptr++);
				PUTSHORT(macsptr++, *cache_macsptr++);
				PUTSHORT(macsptr++, *cache_macsptr++);
				PUTSHORT(macsptr++, *cache_macsptr++);
				PUTSHORT(macsptr++, *cache_macsptr++);
				PUTSHORT(macsptr++, *cache_macsptr++);
				
				/*
				 * Write the IEEE802.3 length
				 */
				PUTSHORT(macsptr++, ROUND(temp_length + SNAP_HDRBYTES));
				
				/*
				 * Write the SNAP header
				 */
				PUTSHORT(macsptr++, SNAPSNAP);
				macptr = (ulong *)macsptr;
				PUTLONG(macptr++, OLD_ETHEROUI);
				macsptr = (ushort *)macptr;
				PUTSHORT(macsptr++, TYPE_NOVELL1);
				
				temp_length += path->length;
			    } else {
				pak->datagramstart =
				    ipxheadstart(pak) - path->length;
				
				macsptr = (ushort *)pak->datagramstart;
				cache_macsptr = path->novp_mh.mac_shorts;
				
				/*
				 * Write Lex Header.
				 */
				PUTSHORT(macsptr++, *cache_macsptr++);
				PUTSHORT(macsptr++, *cache_macsptr++);
				PUTSHORT(macsptr++, *cache_macsptr++);
				
				/*
				 * Write the DA and SA
				 */
				PUTSHORT(macsptr++, *cache_macsptr++);
				PUTSHORT(macsptr++, *cache_macsptr++);
				PUTSHORT(macsptr++, *cache_macsptr++);
				PUTSHORT(macsptr++, *cache_macsptr++);
				PUTSHORT(macsptr++, *cache_macsptr++);
				PUTSHORT(macsptr++, *cache_macsptr++);
				
				/*
				 * Write either the Novell Type or IEE802.3 length
				 * (the latter is the 0xFFFF bogon Novell encap).
				 */
				if (enctype == ET_ARPA) {
				    PUTSHORT(macsptr, TYPE_NOVELL1);
				} else {
				    PUTSHORT(macsptr++, ROUND(temp_length));
				    PUTSHORT(macsptr, NOVELL_NO_CHECKSUM);
				}
				
				temp_length += path->length;
			    }
			    
			    pak->datagramsize = ROUND(temp_length);
			    
			    pak->if_output = swoutput;
			    (*output->fastsend)(output, pak);
			    return(TRUE);
			    
			  case FS_DIALER:
			    /*
			     * Switch to a DDR interface (serial, dialer, PRI, BRI)
			     */
			    {
				DDR_FASTSWITCH(input, output, cache, DDR_SETNEXT,
					       ipx_dialerlist, DDR_CLASSIFY, 0,
					       path->dialergroup,
					       resume_dialer);
			    }
			    
			  case FS_TUNNEL:
			    /*
			     * GRE fastswitching.
			     */
			    pak->if_output = swoutput;
			    (*output->fastsend)(output, pak);
			    return(TRUE);
			    

			  case FS_BVI:
 
			    /*
			     * Switch to BVI(Bridge-group Virtual Interface).
			     * We need to pass the packet to tbridge
			     * fast-switching code
			     */
			    
			    enctype = path->novp_nidb_ptr->enctype;
			    
			    if (enctype == ET_SAP) {
				pak->datagramstart =
				    ipxheadstart(pak) - ETHER_SAP_ENCAPBYTES;
				
				macptr = (ulong *)pak->datagramstart;
				cache_macptr = path->novp_mh.mac_longs;
 
				/*
				 * Write the DA and SA
				 */
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				
				/*
				 * Write the IEEE802.3 length
				 */
				macsptr = (ushort *)macptr;
				PUTSHORT(macsptr++, ROUND(temp_length + SAP_HDRBYTES));
				
				/*
				 * Write the LLC header
				 */
				PUTSHORT(macsptr++, SAPSAP_NOVELL);
				*(uchar *)macsptr = LLC1_CLASS_U;
				
				temp_length += ETHER_SAP_ENCAPBYTES;
				
			    } else if (enctype == ET_SNAP) {
				pak->datagramstart =
				    ipxheadstart(pak) - ETHER_SNAP_ENCAPBYTES;
				
				macptr = (ulong *)pak->datagramstart;
				cache_macptr = path->novp_mh.mac_longs;
				
				/*
				 * Write the DA and SA
				 */
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				
				/*
				 * Write the IEEE802.3 length
				 */
				macsptr = (ushort *)macptr;
				PUTSHORT(macsptr++, ROUND(temp_length + SNAP_HDRBYTES));
				
				/*
				 * Write the SNAP header
				 */
				PUTSHORT(macsptr++, SNAPSNAP);
				macptr = (ulong *)macsptr;
				PUTLONG(macptr++, OLD_ETHEROUI);
				macsptr = (ushort *)macptr;
				PUTSHORT(macsptr++, TYPE_NOVELL1);
				
				temp_length += ETHER_SNAP_ENCAPBYTES;
				
			    } else {
				pak->datagramstart =
				    ipxheadstart(pak) - ETHER_ARPA_ENCAPBYTES;
				
				macptr = (ulong *)pak->datagramstart;
				cache_macptr = path->novp_mh.mac_longs;
				
				/*
				 * Write the DA and SA
				 */
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				PUTLONG(macptr++, *cache_macptr++);
				
				/*
				 * Write either the Novell Type or IEEE802.3 length
				 * (the latter is the 0xFFFF bogon Novell encap).
				 */
				macsptr = (ushort *)macptr;
				if (enctype == ET_ARPA) {
				    PUTSHORT(macsptr, TYPE_NOVELL1);
				} else {
				    PUTSHORT(macsptr++, ROUND(temp_length));
				    PUTSHORT(macsptr, NOVELL_NO_CHECKSUM);
				}
				
 			    temp_length += ETHER_ARPA_ENCAPBYTES;	
			    }
			    
			    /* Virtual ether */
			    if (temp_length < MINETHERSIZE)
				temp_length = MINETHERSIZE;
			    else
				temp_length = ROUND(temp_length);
			    
			    pak->datagramsize = temp_length;
			    
			    /*
			     * Overwrite the input interface to the BVI
			     * Reason is for bridging code, it could then 
			     * figure out which bridge group to bridge 
			     * the packet; Otherwise, the input interface 
			     * is a routed interface, no bridging 
			     * attributes associated with it.
			     * Clear the output interface to let bridging
			     * code sort it out
			     */
			    if (!change_if_input(pak, swoutput)) {
				datagram_done(pak);
				return (TRUE);
			    }
			    
			    pak->if_output = NULL;
			    
			    /*
			     * Set mac_start and addr_start since bridging
			     * code assumes it is the same as datagramstart
			     * for ether case
			     */
			    pak->mac_start = pak->addr_start = pak->datagramstart;
			    if (enctype == ET_ARPA)
				pak->info_start = NULL;
			    else
				pak->info_start = pak->datagramstart + 
				    ETHER_8023_ENCAPBYTES;
                            
                            /* Charge bvi interface with bridge packet output
                             * per transition 5 of CSCdi92583
                             */
                            output->counters.outputs_fast[ACCT_PROTO_BRIDGE]++;
                            output->counters.tx_cumbytes_fast[ACCT_PROTO_BRIDGE] +=
                            pak->datagramsize;
			    return (reg_invoke_tbridge_forward(pak));
			    
			default:
			    return(FALSE);
			    
			}    /* end of switch of FS_TYPE */
		    } else {
 			cache->novc_active++;
 			goto nov_resume_active;
  		    }
		}
	    }
	}
    } else {
	/*
	 * The max hop count may have been reached. However, now do a
	 * comprehensive check to determine if the hop count is excessive.
	 */
	if (NOVELL_IGRP_HOPCOUNT_ALGORITHM_ALLOWED) {
	    if (((ipx->tc >= XNS_IGRP_MIN_HOPS) &&
		 (ipx->tc <  XNS_IGRP_MAX_HOPS)) ||
		(ipx->tc  < (XNS_MAX_HOPS+1))) {
		goto nov_hop_count_ok;
	    }
	}
    }
    return(FALSE);
}

static void ipx_les_register_drivers (ulong flags, hwidbtype* idb)
{
    /*
     * Check to see if we're configuring IPX
     */
    if (!(flags & (FSF_NOVELL1 | FSF_NOVELL2)))
	return;

    if (idb->novell_fastokay) {
        reg_add_ether_fs(RXTYPE_NOVELL1,
                         (service_ether_fs_type)nov_fastswitch,
                         "nov_fastswitch");
        reg_add_fddi_fs(RXTYPE_NOVELL1,
                        (service_fddi_fs_type)nov_fastswitch,
                        "nov_fastswitch");
        reg_add_framerelay_fs(RXTYPE_NOVELL1,
                        (service_framerelay_fs_type)nov_fastswitch,
                        "nov_fastswitch");
        reg_add_hdlc_fs(RXTYPE_NOVELL1,
                        (service_hdlc_fs_type)nov_fastswitch,
                        "nov_fastswitch");
        reg_add_ppp_fs(RXTYPE_NOVELL1,
                       (service_ppp_fs_type)nov_fastswitch,
                       "nov_fastswitch");
        reg_add_ppp_from_process_fs(RXTYPE_NOVELL1,
                       (service_ppp_from_process_fs_type)nov_fastswitch,
                       "nov_fastswitch");
	reg_add_atm_dxi_fs(RXTYPE_NOVELL1,
			   (service_atm_dxi_fs_type)nov_fastswitch,
			   "nov_fastswitch");
	reg_add_smds_fs(RXTYPE_NOVELL1,
			(service_smds_fs_type)nov_fastswitch,
			"nov_fastswitch");
        reg_add_aip_fs(RXTYPE_NOVELL1,
			(service_aip_fs_type)nov_fastswitch,
			"nov_fastswitch");
	reg_add_tunnel_fs(RXTYPE_NOVELL1,
			  (service_tunnel_fs_type)nov_fastswitch,
			  "nov_fastswitch");

#ifdef FUTURE
	reg_add_channel_fs(RXTYPE_NOVELL1,
			   (service_channel_fs_type)nov_fastswitch,
			   "nov_fastswitch");
#endif

        reg_add_ether_fs(RXTYPE_NOVELL2,
                         (service_ether_fs_type)nov_fastswitch,
                         "nov_fastswitch");
        reg_add_framerelay_fs(RXTYPE_NOVELL2,
                        (service_framerelay_fs_type)nov_fastswitch,
                        "nov_fastswitch");
        reg_add_hdlc_fs(RXTYPE_NOVELL2,
                        (service_hdlc_fs_type)nov_fastswitch,
                        "nov_fastswitch");
        reg_add_ppp_fs(RXTYPE_NOVELL2,
                       (service_ppp_fs_type)nov_fastswitch,
                       "nov_fastswitch");
        reg_add_ppp_from_process_fs(RXTYPE_NOVELL2,
                       (service_ppp_from_process_fs_type)nov_fastswitch,
                       "nov_fastswitch");
	reg_add_atm_dxi_fs(RXTYPE_NOVELL2,
			   (service_atm_dxi_fs_type)nov_fastswitch,
			   "nov_fastswitch");
	reg_add_smds_fs(RXTYPE_NOVELL2,
			(service_smds_fs_type)nov_fastswitch,
			"nov_fastswitch");
        reg_add_aip_fs(RXTYPE_NOVELL2,
                        (service_aip_fs_type)nov_fastswitch,
                        "nov_fastswitch");
        reg_add_ether_fs(RXTYPE_SNAP_NOV1,
                         (service_ether_fs_type)nov_fastswitch,
                         "nov_fastswitch");
        reg_add_fddi_fs(RXTYPE_SNAP_NOV1,
                        (service_fddi_fs_type)nov_fastswitch,
                        "nov_fastswitch");
        reg_add_fddi_fs(RXTYPE_NOVELL2,
                        (service_fddi_fs_type)nov_fastswitch,
                        "nov_fastswitch");
        reg_add_framerelay_fs(RXTYPE_SNAP_NOV1,
                        (service_framerelay_fs_type)nov_fastswitch,
                        "nov_fastswitch");
        reg_add_atm_dxi_fs(RXTYPE_SNAP_NOV1,
			   (service_atm_dxi_fs_type)nov_fastswitch,
			   "nov_fastswitch");
	reg_add_smds_fs(RXTYPE_SNAP_NOV1,
			(service_smds_fs_type)nov_fastswitch,
			"nov_fastswitch");
        reg_add_aip_fs(RXTYPE_SNAP_NOV1,
			   (service_aip_fs_type)nov_fastswitch,
			   "nov_fastswitch");
        reg_add_token_fs(RXTYPE_SNAP_NOV1,
                         (service_token_fs_type)nov_fastswitch,
                         "nov_fastswitch");

        reg_add_ether_fs(RXTYPE_SAP_NOVELL,
                         (service_ether_fs_type)nov_fastswitch,
                         "nov_fastswitch");
        reg_add_fddi_fs(RXTYPE_SAP_NOVELL,
                        (service_fddi_fs_type)nov_fastswitch,
                        "nov_fastswitch");
        reg_add_token_fs(RXTYPE_SAP_NOVELL,
                         (service_token_fs_type)nov_fastswitch,
                         "nov_fastswitch");
        reg_add_aip_fs(RXTYPE_SAP_NOVELL,
			   (service_aip_fs_type)nov_fastswitch,
			   "nov_fastswitch");
    } else {
            fs_table_fill((fastswitch_type)return_false, "return_false",
                          RXTYPE_NOVELL1);
            fs_table_fill((fastswitch_type)return_false, "return_false",
                          RXTYPE_NOVELL2);
            fs_table_fill((fastswitch_type)return_false, "return_false",
                          RXTYPE_SNAP_NOV1);
            fs_table_fill((fastswitch_type)return_false, "return_false",
                          RXTYPE_SAP_NOVELL);
    }
}

static void ipx_les_fs_init (subsystype *subsys)
{
    reg_add_register_drivers(ipx_les_register_drivers,
                             "ipx_les_registers_drivers");
    reg_add_fast_supported(ipx_fast_supported,
			   "ipx_fast_supported");
}

/*
 * Novell LES FS subsystem header
 */

#define IPXLESFS_MAJVERSION 1
#define IPXLESFS_MINVERSION 0
#define IPXLESFS_EDITVERSION  1

SUBSYS_HEADER(ipx_les_fs,
	      IPXLESFS_MAJVERSION, IPXLESFS_MINVERSION, IPXLESFS_EDITVERSION,
              ipx_les_fs_init, SUBSYS_CLASS_PROTOCOL,
              "seq: ipx",
              "req: fastswitch, ipx");
