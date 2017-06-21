/* $Id: serial.c,v 3.16.12.21 1996/08/29 02:25:31 xtang Exp $
 * $Source: /release/112/cvs/Xsys/wan/serial.c,v $
 *------------------------------------------------------------------
 * serial.c -- common synchronous serial interface routines
 *
 * 23-June-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: serial.c,v $
 * Revision 3.16.12.21  1996/08/29  02:25:31  xtang
 * CSCdi60447:  FSIP max MTU=14816 not 18000
 * Branch: California_branch
 *              Implemented new FSIP command 93 to pass the available
 *              buffer size in the controller to the system.
 *              The corresponding microcode change are in
 *              cvs -d /micro fsip_q (fsip187-0)
 *              and cvs -d /micro fsip_i (rsp_fsip204-0).
 *
 * Revision 3.16.12.20  1996/08/28  13:22:35  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.16.12.19  1996/08/13  02:27:02  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.16.12.18  1996/08/01  20:01:20  turadek
 * CSCdi64710:  rtbr fails for ip with hdlc encap for gs3 , c4500 and c1005
 * Branch: California_branch
 * undo part of changes for CSCdi63245, to unbreak hdlc bridging.
 *
 * Revision 3.16.12.17  1996/07/31  19:26:33  turadek
 * CSCdi63245:  bridged, compressed HDLC traffic crashes router
 * Branch: California_branch
 * Don't compress bridged traffic for now.
 *
 * Revision 3.16.12.16  1996/07/28  07:33:13  irfan
 * CSCdi60568:  v120 autodetect not working
 * Branch: California_branch
 * allow serial interfaces to autodetect encaps on the wire for incoming
 * connections.
 *
 * Revision 3.16.12.15  1996/07/23  13:32:06  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.16.12.14  1996/07/08  22:58:54  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.16.12.13  1996/06/27  09:45:38  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.16.12.12  1996/06/17  08:36:48  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.16.12.11  1996/06/06  22:52:18  wmay
 * CSCdi58782:  down line stays stuck looped - clear inloopback field when
 * changing state
 * Branch: California_branch
 *
 * Revision 3.16.12.10  1996/05/28  17:01:28  fox
 * CSCdi58271:  SLIP reports interface as looped
 * Branch: California_branch
 * Use correct symbol for 'idb' and compile first!
 *
 * Revision 3.16.12.9  1996/05/28  15:02:40  fox
 * CSCdi58271:  SLIP reports interface as looped
 * Branch: California_branch
 * Clear the inloopback flag when deencapsulating HDLC and PPP.
 *
 * Revision 3.16.12.8  1996/05/22  17:21:58  fox
 * CSCdi57795:  Unable to change a BRI interface from ppp to hdlc.
 * Branch: California_branch
 * Don't clear idb->enctype in ppp_deencap(), slip_deencap() or
 * bstun_deencapsulate().  Move the setting of idb->encaptype to
 * the start of the encapsulation setup functions because calls to
 * other function use this value.
 *
 * Revision 3.16.12.7  1996/05/09  14:49:21  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.16.12.6  1996/05/04  02:17:17  wilber
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
 * Revision 3.16.12.5  1996/04/27  06:38:14  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.16.12.4.4.1  1996/04/27  07:39:19  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.16.12.4  1996/04/22  21:51:55  gtaylor
 * CSCdi54554:  Bridging over isdn leased-line broken for HDLC
 * encapsulation.
 * Branch: California_branch
 *
 * Revision 3.16.12.3  1996/04/22  18:17:15  gstovall
 * CSCdi53071:  media idb init routines will fail on second pass
 * Branch: California_branch
 * Allow media idb init routines to be called again.  Also clean up the
 * handling of the media idb queues.
 *
 * Revision 3.16.12.2.6.1  1996/04/08  02:43:30  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.16.12.2  1996/03/23  01:35:17  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.16.12.1  1996/03/18  22:48:32  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.16.16.1  1996/03/28  17:37:01  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.16  1996/02/13  21:26:59  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.15  1996/02/09  20:09:13  gstovall
 * CSCdi48190:  Bogus restart delay in show int bri0
 * Make sure the state_config vector is setup for all serial type
 * interfaces.
 *
 * Revision 3.14  1996/02/08  20:14:49  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.13  1996/01/25  23:12:56  gstovall
 * CSCdi47390:  down-when-looped command not working (regression)
 * Remember to reset the itemlist before tossing it up to the next level.
 *
 * Revision 3.12.2.1  1996/01/24  23:31:58  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.12  1996/01/22  07:46:04  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.11  1996/01/15  00:17:54  misko
 * CSCdi41345:  BRI-3-BADENCAP
 * Clean up hdlc_deencapsulate so that drivers don't get called with an
 * enctype of null.  Serial drivers should always at least be doing hdlc.
 *
 * Revision 3.10  1995/12/07  22:34:20  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.9  1995/12/06  20:33:08  gstovall
 * CSCdi45056:  HDLC keepalives keep B channel up
 * Resurrect hdlc_encapsulate() for use on keepalives.
 *
 * Revision 3.8  1995/12/06  18:33:39  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.7  1995/11/29  21:59:38  gstovall
 * CSCdi44528:  encap ppp on interface dialer1 crashes router
 * Make sure the dialer idb has a serial subblock.
 *
 * Revision 3.6  1995/11/21  09:42:07  irfan
 * CSCdi44229:  dialer rotary-group caused Low memory error.
 * Some interfaces want to be typed as serial yet don't have
 * their serial subblocks in hwidb. Ensure that we have valid
 * ssb before using it.
 *
 * Revision 3.5  1995/11/20  22:28:39  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.4  1995/11/20  21:09:58  gstovall
 * CSCdi44060:  Show dialer crashes
 * Do not add virtual idbs to the serial idb list.
 *
 * Revision 3.3  1995/11/17  18:04:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:50:29  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:50:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/09/09  00:54:50  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.7  1995/08/25  03:32:10  speakman
 * CSCdi38884:  IP ARP over token/serial broken
 * SNAP over HDLC, who woulda thunk it.  Do the GETSHORT thing.
 *
 * Revision 2.6  1995/08/08  16:51:07  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.5  1995/08/04 18:24:04  wmay
 * CSCdi38244:  changing encaps can cause infinite loop - when calling
 * serial_setencap after a invalid encapsulation, check if the original
 * encap is bad - if so, restore the default encap
 *
 * Revision 2.4  1995/08/01  00:21:44  wmay
 * CSCdi37492:  Crash when attempting to change serial encaps to
 * unsupported encaps - call serial_setencap with old enctype
 *
 * Revision 2.3  1995/07/19  16:13:37  ahh
 * CSCdi37281:  NULL dereference when compression not in image
 * Print the name of the proper interface and rate-limit the error message.
 *
 * Revision 2.2  1995/06/28  09:32:53  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:19:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "../tcp/tcpcompress.h"
#include "serial.h"
#include "serial_private.h"
#include "serial_debug.h"
#include "dialer.h"
#include "dialer_registry.h"
#include "../if/network.h"
#include "../if/ether.h"
#include "../ip/ip_registry.h"
#include "../les/isdn_bri.h"
#include "../snmp/snmp_api.h"
#include "../snmp/ifmib_registry.h"
#include "../parser/parser_defs_compress.h"
#include "../x25/compress.h"
#include "../if/linkdown_event.h"
#include "serial_parser.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../parser/parser_defs_parser.h"
#include "../util/itemlist.h"

/*
 * Forward declarations
 */
static void hdlc_compressed_iqueue(hwidbtype *idb, paktype *pak);


/*
 * hdlc_check_keepalive_time
 * Compare keepalive time with configured value.  Display a buginf if
 * times don't match
 */
static void hdlc_check_keepalive_time (hwidbtype *idb, serialsb *ssb)
{
    ulong temp;

    if (TIMER_RUNNING(ssb->hdlc_last_keep)) {
	if (idb->nokeepalive) {
	    if (serial_debug)
		buginf("\n%s - Got keepalive with none configured",
		       idb->hw_namestring);
	} else {
	    if (serial_debug) {
		temp = ELAPSED_TIME(ssb->hdlc_last_keep);
		temp = temp / ONESEC;
		if ((temp > idb->keep_period + 1)
		    || (temp < idb->keep_period - 1)) {
		    buginf("\n%s - Keepalive time mismatch - should be %d secs, is %d",
			   idb->hw_namestring, idb->keep_period, temp);
		}
	    }

	}
    }
    GET_TIMESTAMP(ssb->hdlc_last_keep);
}

/*
 * hdlc_deencapsulate
 *
 * HDLC has a variable encapsulation vector that we don't want lying
 * around when another encapsulation has been selected.
 */
static void hdlc_deencapsulate (hwidbtype *hwidb)
{
    /*
     * hdlc_deencapsulate use to contain the following code:
     *    idb->vencap = NULL;
     *    idb->enctype = ET_NULL;
     *    idb->downiflooped = FALSE;
     * Since hdlc is the default encapsulation, this was not the proper
     * behavior, as this allowed the enctype to end up as null.  vencap
     * was cleared for historical reasons, but now all encaps routines
     * set this vector, so this is no longer needed.  The downiflooped
     * flag should only be cleared if the user removed the command which
     * set this flag.
     *
     * Bottom line is this is now a null routine.
     */
    /*
     * Clear looped flag here.  It can not be correctly checked from any
     * serial protocol that does not support loop detection so don't
     * burden all the others with clearing the flag.
     */
    hwidb->inloopback = FALSE;
}


/*
 * hdlc_bcast_default
 *
 * Turn an address into an encapsulation type and a MAC address
 * Called by SERVICE_HDLC_BROADCAST, default.
 */
static boolean hdlc_bcast_default (paktype *pak, long address,
				   int *enctype, ushort *flags)
{
    if ((address == 0) || (address == -1))
	*flags |= HDLC_BCAST;
    return(TRUE);
}


/*
 * hdlc_vencapsulate
 *
 * Encapsulate a datagram.  Makes no assumptions about how the data
 * is aligned in the packet buffer.  Assumes datagramstart and datagramsize
 * tell the story.
 */
static boolean hdlc_vencapsulate (paktype *pak, long address)
{
    ushort flags = HDLC_STATION;
    int enctype = ET_HDLC;
    vhdlctype *hdlc;
    hwidbtype *hwidb;

    if (!pak->datagramstart)
	return(FALSE);

    hwidb = pak->if_output->hwptr;

    /*
     * We initialize encsize here in case one of the hdlc_macaddr
     * functions (e.g., clns_hdlc_macaddr for the FFB) needs to adjust
     * it. 
     */

    pak->encsize = 0;

    /*
     * Do broadcast_determination.  We want an encapsulation method, and
     * the appropriate hdlc header flags.  The encapsulation method is
     * only used as an index to the mac rewrite routine, or to indicate
     * that the packet has already been completely processed.
     */
    if (!reg_invoke_media_hdlc_macaddr(pak->linktype, pak, address,
				       &enctype, &flags)) {
	if (!reg_used_media_hdlc_macaddr(pak->linktype))
	    errmsg(&msgsym(BADMACREG, LINK), pak->if_output->namestring,
		   pak->linktype);
	return(FALSE);
    }
    if (enctype == ET_NULL)
	return(TRUE);

    switch (enctype) {
      case ET_HDLC:
        pak->info_start = pak->datagramstart;	/* begining of (compression)
						   payload */
	pak->datagramsize += sizeof(vhdlctype);
	pak->datagramstart -= sizeof(vhdlctype);
	if (pak->datagramstart < pak->data_area)
	    return(FALSE);
	hdlc = (vhdlctype *) pak->datagramstart;
	PUTSHORT(&hdlc->var_hdlcflags, flags);
	PUTSHORT(&hdlc->var_hdlctype, link2serial(pak->linktype));

	pak->mac_start  = pak->datagramstart;	/* begining of hdlc
						   encapsulation */
	pak->addr_start = NULL;
	pak->rif_start  = NULL;
	pak->riflen	= 0;
	pak->enctype    = ET_HDLC;
	pak->encsize   += sizeof(vhdlctype);
	break;

      default:
	if (!reg_invoke_media_hdlc_macwrite(enctype, pak, pak->linktype, flags,
					    pak->datagramsize)) {
	    errmsg(&msgsym(BADENCAP, LINK), pak->if_output->namestring,
		   enctype);
	    return(FALSE);
	}
	break;
    }

    /*
     * Make sure routing cache is populated by transit traffic.
     */
    hwidb = pak->if_output->hwptr;
    if (hwidb->board_encap) {
	if (!(*hwidb->board_encap)(pak,hwidb)) {
	    return(FALSE);
	}
    }
    reg_invoke_fs_cache_update(pak->linktype, pak, 0L);
    return(TRUE);
}


/*
 * hdlc_encapsulate
 *
 * Encapsulate a datagram.  Makes no assumptions about how the data
 * is aligned in the packet buffer.  Assumes datagramstart and datagramsize
 * tell the story.
 *
 * Warning! The DDR code can not use pak_encap(), it must use
 * hdlc_encapsulate() for sending keepalives.
 */
static boolean hdlc_encapsulate (paktype *pak, int length, long address)
{
    pak->datagramstart = pak->network_start;
    pak->datagramsize = length;
    return(hdlc_vencapsulate(pak, address));
}


/*
 * link2serial
 * Given a link type, return a serial type code
 */
const char fmt_Illegal_serial_link_type_code[] =
            "\nIllegal serial link type code %d, PC=0x%x\n";
int link2serial (long link)
{
    SAVE_CALLER();

    switch (link) {
	case LINK_IP:		 return(TYPE_IP10MB);
	case LINK_ADDRESS:	 return(TYPE_REVERSE_ARP);
	case LINK_APOLLO:	 return(TYPE_APOLLO);
	case LINK_ARP:           return(TYPE_RFC826_ARP);
 	case LINK_DECNET:
 	case LINK_DECNET_ROUTER_L1:
 	case LINK_DECNET_ROUTER_L2:
        case LINK_DECNET_PRIME_ROUTER:
 	case LINK_DECNET_NODE:	 return(TYPE_DECNET);
	case LINK_XNS:		 return(TYPE_XNS);
	case LINK_NOVELL:	 return(TYPE_NOVELL1);
	case LINK_DEC_SPANNING:	 return(TYPE_DEC_SPANNING);
	case LINK_IEEE_SPANNING: return(TYPE_IEEE_SPANNING);
	case LINK_BRIDGE:        return(TYPE_BRIDGE);
	case LINK_APPLETALK:	 return(TYPE_ETHERTALK);
	case LINK_AARP:		 return(TYPE_AARP);
	case LINK_CLNS:
	case LINK_CLNS_ALL_ES:
	case LINK_CLNS_ALL_IS:
	case LINK_ISIS_ALL_L1_IS:
	case LINK_ISIS_ALL_L2_IS:
	case LINK_CLNS_BCAST:	 return(TYPE_CLNS);
	case LINK_VINES:	 return(TYPE_VINES2);
	case LINK_VINES_ECHO:	 return(TYPE_VINES_ECHO2);
	case LINK_RSRB:		 return(TYPE_RSRB);
	case LINK_DLSW:		 return(TYPE_DLSW);
	case LINK_STUN:		 return(TYPE_STUN);
	case LINK_BSTUN:	 return(TYPE_BSTUN);
	case LINK_FR_ARP:        return(TYPE_FR_ARP);
	case LINK_UNCOMPRESSED_TCP: return(TYPE_UNCOMPRESSED_TCP);
	case LINK_COMPRESSED_TCP: return(TYPE_COMPRESSED_TCP);
	case LINK_CDP:           return(TYPE_CDP);
	default:
	  return(reg_invoke_media_link_to_type(link));
    }
    if (serial_debug)
	buginf(fmt_Illegal_serial_link_type_code, link, caller());
    return(0);
}


/*
 * serial2link
 * Given a serial type code, return a link code
 */
int serial2link (long type)
{
    ulong value;

    SAVE_CALLER();

    switch (type) {
	case TYPE_IP10MB:	 return(LINK_IP);
	case TYPE_APOLLO:	 return(LINK_APOLLO);
	case TYPE_RFC826_ARP:    return(LINK_ARP);
	case TYPE_REVERSE_ARP:	 return(LINK_ADDRESS);
 	case TYPE_DECNET:	 return(LINK_DECNET);
	case TYPE_XNS:		 return(LINK_XNS);
	case TYPE_NOVELL1:	 return(LINK_NOVELL);
	case TYPE_DEC_SPANNING:	 return(LINK_DEC_SPANNING);
	case TYPE_ETHERTALK:	 return(LINK_APPLETALK);
	case TYPE_AARP:		 return(LINK_AARP);
	case TYPE_IEEE_SPANNING: return(LINK_IEEE_SPANNING);
	case TYPE_CLNS:		 return(LINK_CLNS);
	case TYPE_VINES:	 return(LINK_VINES);
	case TYPE_VINES2:	 return(LINK_VINES);
	case TYPE_VINES_ECHO:	 return(LINK_VINES_ECHO);
	case TYPE_VINES_ECHO2:	 return(LINK_VINES_ECHO);
	case TYPE_BRIDGE:        return(LINK_BRIDGE);
	case TYPE_RSRB:		 return(LINK_RSRB);
	case TYPE_DLSW:		 return(LINK_DLSW);
	case TYPE_STUN:		 return(LINK_STUN);
	case TYPE_BSTUN:	 return(LINK_BSTUN);
	case TYPE_FR_ARP:        return(LINK_FR_ARP);
	case TYPE_UNCOMPRESSED_TCP: return(LINK_UNCOMPRESSED_TCP);
	case TYPE_COMPRESSED_TCP: return(LINK_COMPRESSED_TCP);
	case TYPE_CDP:           return(LINK_CDP);
	default:
	  value = reg_invoke_media_serial_to_link(type);
	  if (value)
	      return(value);
    }
    if (serial_debug)
	buginf("\nIllegal HDLC serial type code %d, PC=0x%x\n",
	       type, caller());
    return(LINK_ILLEGAL);
}


/*
 * hdlc_getlink
 * Get link type for an HDLC datagram.
 */
long hdlc_getlink (paktype *pak)
{
    vhdlctype *hdlc;
    dialer_hdr *dialer;

    if (pak->enctype == ET_ETHERTALK)
      return(LINK_APPLETALK);

    if (pak->flags & PAK_TBRIDGE_IN)
      return(ether_getlink(pak));

    if (pak->enctype != ET_HDLC)
      return(LINK_ILLEGAL);

    dialer = (dialer_hdr *)(pak->network_start - DIALER_ENCAPBYTES);
    if (GETSHORT(&dialer->flags) == V25BIS_FLAGS)
      return(LINK_ILLEGAL);
    
    hdlc = (vhdlctype *)(pak->datagramstart);
    return(serial2link(GETSHORT(&hdlc->var_hdlctype) & 0xFFFF));
}


/*
 * hdlc_broadcast
 * Returns TRUE if datagram is physical broadcast
 */
static fuzzy hdlc_broadcast (hwidbtype *hwidb, paktype *pak)
{
    vhdlctype *hdlc;

    hdlc = (vhdlctype *)pak->datagramstart;

    if (GETSHORT(&hdlc->var_hdlcflags) & HDLC_BCAST)
	return FUZ_TRUE;

    return FUZ_FALSE;
}


/*
 * hdlc_send_keepalive
 * Send an HDLC line check packet.
 */
static void hdlc_send_keepalive (hwidbtype *idb)
{
    paktype *pak;
    struct slarp *ptr;
    serialsb *ssb;

    /*
     * Send HDLC beacon packet
     */
    pak = getbuffer(sizeof(struct slarp));
    if (!pak)
	return;
    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);
    ptr = (struct slarp *)pak->network_start;   
    ptr->code = SLARP_LINECHECK;
    ptr->t.chk.mysequence = ++ssb->hdlc_myseq;
    ptr->t.chk.yoursequence = ssb->hdlc_yoursseen;
    ptr->t.chk.reliability = 0xFFFF;
    ssb->hdlc_unique_sent = create_unique_number();
    PUTLONG(&ptr->t.chk.unique, ssb->hdlc_unique_sent);
    pak->if_output = idb->firstsw;
    pak->linktype = LINK_ADDRESS;
    pak->flags |= PAK_PRIORITY;
    if (hdlc_encapsulate(pak, sizeof(struct slarp), 0))
	datagram_out(pak);
    else
	retbuffer(pak);
}


/*
 * hdlc_address
 * Received an HDLC address resolution packet at interrupt level
 * If we get a SLARP_REQUEST, reset the beaconing since we suspect the other
 * side is coming up.
 */
static void hdlc_address (paktype *pak)
{
    struct slarp *ptr;
    hwidbtype *hwidb;
    serialsb *ssb;

    ptr = (struct slarp *)pak->network_start;
    hwidb = pak->if_input->hwptr;
    ssb = idb_get_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
    switch (ptr->code) {
    case SLARP_REQUEST:
	/*
	 * They sent us a slarp.  That's as good as a keepalive.
	 * Actually, it's better, because we'll keep the line protocol
	 * up without receiving keepalives a lot longer.  But, if we
	 * get a keepalive, we'll only give it the normal amount.
	 */
	ssb->hdlc_yoursseen = 0;
	ssb->hdlc_mineseen = ssb->hdlc_myseq;  /* give us 30 sec. */
	ssb->hdlc_got_keep = TRUE;
	ssb->hdlc_got_slarp = TRUE;
	ssb->hdlc_no_keep_count = 0;
	if (hwidb->hdlc_lineup == FALSE) {
	    hwidb->hdlc_lineup = TRUE;
	    route_adjust(hwidb);
	    hwidb->reason = "Remote end restarted";
	    reg_invoke_ifmib_link_trap(LINK_UP_TRAP, hwidb);
	}
	reg_invoke_ip_slarp(pak);
	break;
      case SLARP_REPLY:
	reg_invoke_ip_slarp(pak);
	break;
      case SLARP_LINECHECK:
	/*
	 * Indicate that we got a keepalive, and that we're not holding
	 * because the other side slarp'ed
	 */
	ssb->hdlc_got_keep = TRUE;
	ssb->hdlc_got_slarp = FALSE;
	ssb->hdlc_yoursseen = ptr->t.chk.mysequence;
	ssb->hdlc_mineseen = ptr->t.chk.yoursequence;
	if (pak->datagramsize + HDLC_ENCAPBYTES > sizeof(struct oldslarp))
	    ssb->hdlc_unique_rcvd = GETLONG(&ptr->t.chk.unique);
	hdlc_check_keepalive_time(hwidb,ssb);
	break;
      default:
        if (reg_invoke_slarp_extended_type(ptr->code, pak))
           return;
	break;
    }
    retbuffer(pak);
}


/*
 * hdlc_periodic
 * Periodic checker for serial interfaces.
 */
static void hdlc_periodic (hwidbtype *idb)
{
    int window;
    serialsb *ssb;
    tinybool got_keep;
    
    if ((idb->state != IDBS_UP)  || (idb->enctype != ET_HDLC))
	return;

    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);
    if (idb->nokeepalive) {
	/* If nokeepalive flag is on, force line protocol up. */
	ssb->hdlc_yoursseen = 0L;
	if (idb->hdlc_lineup == FALSE) {
	    idb->hdlc_lineup = TRUE;
	    idb->reason = "Checking disabled";
	    reg_invoke_ifmib_link_trap(LINK_UP_TRAP, idb);
	}
    } else {
	window = HDLC_KEEPALIVE_WINDOW;
	if (idb->counters.rx_packetrate > HDLC_LOAD_THRESHOLD) {
	    window <<= 1;		/* double the window */
	}
	got_keep = ssb->hdlc_got_keep;
	if (got_keep) {
	    ssb->hdlc_got_keep = FALSE;
	    if (ssb->hdlc_unique_sent == ssb->hdlc_unique_rcvd)
		idb->inloopback = TRUE;
	    else
		idb->inloopback = FALSE;

	    if (idb->hdlc_lineup == FALSE) {
		/*
		 * If the line was down, bring it up - give them 3 tries to
		 * get the sequence number right.  This prevents us from
		 * having to wait until potentiallu the 2nd keepalive from
		 * coming in before we bring the line up.
		 */
		ssb->hdlc_no_keep_count = 0;
		idb->hdlc_lineup = TRUE;
		idb->reason = "Keepalive OK";
		reg_invoke_ifmib_link_trap(LINK_UP_TRAP, idb);
	    }
	    /*
	     * Now check to see if it is within the sequence window
	     * (that tells us if the remote side is receiving our keepalives).
	     * If not in that sequence, give them 3 passes to get it right
	     */

	    if ((ssb->hdlc_myseq - ssb->hdlc_mineseen) <= window) {
		/* response within window - clear the no keep count */
		ssb->hdlc_no_keep_count = 0;
	    } else {
		/*
		 * response outside window - increment no keep count and check
		 * below
		 */
		ssb->hdlc_no_keep_count++;
	    }
	} else {
	    /*
	     * No keeps received in the last period.  Increment the no_keep
	     * count.  If we received only a slarp, increase the window a bit.
	     */
	    if (idb->hdlc_lineup)
		ssb->hdlc_no_keep_count++;
	    if (ssb->hdlc_got_slarp)
		window = HDLC_SLARP_WINDOW;
	}
	/*
	 * Now we check the no_keep count (if the line is up).
	 * If we exceed the window counts, bring the line down.
	 */
	if (idb->hdlc_lineup) {
	    if (ssb->hdlc_no_keep_count >= window) {
		ssb->hdlc_no_keep_count = 0;
		idb->hdlc_lineup = FALSE;
		idb->inloopback = FALSE;
		(*idb->reset)(idb);
		
		/* NmEvent Hook 1 */
		idb->failCause = Hdlc_KeepAliveFailure;
		TIMER_STOP(ssb->hdlc_last_keep);
	    } else if (ssb->hdlc_no_keep_count == (window - 1)) {
		/* reset once before bringing down */
		(*idb->reset)(idb);
	    }
 	}
	if (serial_debug)
	    buginf("\n%s: HDLC myseq %u, mineseen %u%s, yourseen %u, "
		   "line %s %s",
		   idb->hw_namestring, ssb->hdlc_myseq, ssb->hdlc_mineseen,
		   ssb->hdlc_got_slarp ? "%" : (got_keep ? "*" : ""),
		   ssb->hdlc_yoursseen, idb->hdlc_lineup ? "up" : "down",
		   idb->inloopback ? "(looped)" : "");
	hdlc_send_keepalive(idb);
    }
}


/*
 * hdlc_keepalive_init    
 * Need to reset the hdlc keepalive counts.  This is in case we are
 * going from no keepalive to keepalive.  Need to let hdlc re-check
 * the line rather than continue to think that it is still up.
 *
 * Since this is called when the keepalive command is entered, we
 * do not clear the state of whether or not we received a keepalive,
 * the other sides sequence number seen, or the keepalives missed count.
 * This is because those will be cleared when we change interface states.
 * We do clear the state of the slarp received.  That will allow the 
 * user to clear the SLARP window by issueing the keep command
 */
static void hdlc_keepalive_init (hwidbtype *hwidb)
{
    serialsb *ssb;
    if (hwidb->enctype == ET_HDLC) {
	ssb = idb_get_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
        ssb->hdlc_myseq = 0;
        ssb->hdlc_mineseen = 0;
	TIMER_STOP(ssb->hdlc_last_keep);
	ssb->hdlc_got_slarp = FALSE;
    }
}


/*
 * hdlc_cstate
 * Set hdlc_lineup to false if carrier drops.
 */
static void hdlc_cstate (hwidbtype *idb)
{
    serialsb *ssb;

    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);
    hdlc_keepalive_init(idb);
    ssb->hdlc_no_keep_count = 0;
    ssb->hdlc_got_keep = FALSE;
    ssb->hdlc_yoursseen = 0;

    /*
     * HDLC has always been optimistic about the line being up.  So if
     * carrier just came up, set hdlc_lineup to true, and let the 3
     * keepalives lost take it back down.
     */
    idb->inloopback = FALSE;
    if (idb->state != IDBS_UP) {
        idb->hdlc_lineup = FALSE;
    } else {
        idb->hdlc_lineup = TRUE;
	reg_invoke_media_protocol_up(idb, LINK_ALL);
    }
}


/*
 * serial_restart
 * Reset a down serial interface in an attempt to bring it up.
 * Originally, this routine was designed because 
 * the R68561 chip occasionally locks up when a circuit goes down.
 * It is not sufficient to perform a reset when the circuit goes down.
 * It is occasional necessary to reset the interface to get things working
 * again after the circuit comes back up.
 * This is used today for also re-dialing when doing dial backup and DTR
 * dialing.
 */
static void serial_restart (hwidbtype *idb)
{
    serialsb *ssb;

    if (!(idb->status & IDB_SERIAL))
	return;

    ssb = idb_use_hwsb_inline(idb, HWIDB_SB_SERIAL);

    /*
     * Take a chance here.  If the interface is a dialer, don't reset it.
     * Resetting it causes all sorts of potential collisions with incoming
     * calls.  The call ends up getting disconnected because the interface
     * gets reset here.
     * Also bail out if idb is ASYNC. Async PPP interfaces can get
     * aborted during negotiation when this timer occurs before
     * linestate goes up.
     * Also bail out if this is a MIP interface.
     */
    if (hw_interface_up(idb) || (idb->state == IDBS_ADMINDOWN) ||
	is_ddr(idb) ||
	(idb->status & IDB_NOHARDWARE) || (idb->status & IDB_ASYNC) ||
	(ssb->serial_restartdelay == 0)) {
	TIMER_STOP(ssb->serial_restarttimer);
	idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);
	return;
    }
    if (!(TIMER_RUNNING(ssb->serial_restarttimer))) {
	TIMER_START(ssb->serial_restarttimer, ssb->serial_restartdelay);
    } else if (AWAKE(ssb->serial_restarttimer)) {
	if (serial_debug)
	    buginf("\n%s: attempting to restart", idb->hw_namestring);
	(*idb->reset)(idb);
	if (ssb->serial_restartdelay != 0) {
	    TIMER_START(ssb->serial_restarttimer, ssb->serial_restartdelay);
	}
    }

    idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);
}


/*
 * hdlc_linestate
 * Returns current line protocol status for ET_HDLC
 * If we are a spoofing dialer, we are always up.
 * Note: the interface is down if it is looped back and the 
 * interface is configured to go down on loopback
 */
static boolean hdlc_linestate (hwidbtype *hwidb)
{
    serialsb *ssb;
    
    ssb = idb_get_hwsb_inline(hwidb, HWIDB_SB_SERIAL);

    if (reg_invoke_dialer_spoofing_prot(hwidb))
	return(TRUE);
    return(hwidb->hdlc_lineup && !(hwidb->inloopback && ssb->downiflooped));
}


/*
 * hdlc_extract_addr
 *
 * Given a HDLC packet, extract the requested 'MAC' level address
 * into a hwaddrtype structure.
 */
static boolean hdlc_extract_addr (
    paktype *pak,
    hwaddrtype *address,
    enum SRCDST which)
{
    address->type = STATION_CISCO_HDLC;
    address->length = STATIONLEN_CISCO_HDLC;
    address->target_type = TARGET_UNICAST;
    address->addr[0] = 0;
    return(TRUE);
}


/*
 * hdlc_iqueue - Fix up a packet in "hdlc-arpa" encapsulation
 * to not include the HDLC bridge code, and the enqueue it. If this
 * packet came in on an interface that was not part of a spanning
 * tree, ensure that it contains the box-wide address.
 */
static void hdlc_iqueue (hwidbtype *idb, paktype *pak)
{
    hwidbtype *tmp_idb;
    hwidbtype *net_idb;
    ether_hdr *ether;

    pak->datagramstart += HDLC_ENCAPBYTES; /* Point past code */
    ether = (ether_hdr *)pak->datagramstart;


    if (is_isdn(idb) && !ISBRILEASED(idb)) {
      tmp_idb = idb->isdn_info->d_chan_idb;
    } else {
      tmp_idb = idb;
    }

    if (!tmp_idb->tbridge_on_hwidb) {
    	net_idb = get_netidb(idb);
	if ((net_idb == NULL) || !net_idb->tbridge_on_hwidb) {
	    if (((ether->daddr[0] & 0x01) == 0) && /* Not a multicast */
	    	!ieee_equal(ether->daddr, idb->hardware)) {
	    	if (serial_debug)
		    buginf("\n%s: Received bridge packet sent to %e",
			    idb->hw_namestring, ether->daddr);
	    	datagram_done(pak);
	    	return;
	    }
	}
    }

    pak->datagramsize -= HDLC_ENCAPBYTES; /* Fix the size */

    if (GETSHORT((short*)&pak->datagramstart[ARPATYPE]) > MAXETHERSIZE)
	pak->enctype = ET_ARPA; /* Encapsulation is normal ARPA now */
    else if (GETSHORT((short *)&pak->datagramstart[ISO2DSAP]) == SNAPSNAP)
	pak->enctype = ET_SNAP; /* Encapsulation is SNAP */
    /*
     * Deal with NOVELL_ETHER encapsulation: DA(6) + SA(6) + 
     * Length(2) + SAPSAP_NOVELL_ETHER(0xFFFF) 
     * CSCdi56417 and CSCdj03565
     */
    else if (GETSHORT((short *)&pak->datagramstart[ISO2DSAP]) == 
	     SAPSAP_NOVELL_ETHER)
	pak->enctype = ET_NOVELL_ETHER; /* Encap. is NOVELL_ETHER */
    else
	pak->enctype = ET_SAP;  /* Encapsulation is SAP */
    
    pak->flags |= PAK_TBRIDGE_IN; /* Remember our origin */
    (*idb->iqueue)(idb, pak);
}

/*
 * hdlc_parse_packet
 *
 * Set up a packet buffer so that the network level protocol fields
 * are initialized.
 *
 * If data is NULL, pak->datagramstart is assumed to point to the
 * start of the frame to parse. No attempt will be made to align
 * the frame to The Line.
 *
 * For mci/cbus bridging case, we are passing a pointer of sniff buffer as
 * uchar *data from interrupt because we need more than the first four
 * bytes.
 *
 *               byte1  byte2   byte3   byte4
 *  SNIFFBASE+0  HDLC_BRIDGECODE
 *  SNIFFBASE+1  Not valid
 *  SNIFFBASE+2  Not valid
 *  SNIFFBASE+3  Not valid
 *  SNIFFBASE+4  type/len       (dsap)  (ssap)
 *  SNIFFBASE+5  Not valid
 *
 * Caller just make sure we have valid data in
 *      sniff[SNIFFBASE+0] and sniff[SNIFFBASE+4]
 * Don't make assumption of the other sniff buffer bytes because
 * we may call bridge_receive() which rebuilds a "canonical" sniff buffer.
 */
static iqueue_t hdlc_parse_packet (
    hwidbtype *idb,
    paktype *pak,
    uchar *data)
{
    vhdlctype *hdlc;
    charlong firstlong;
    uchar *sniffdata;
    int offset;
    boolean compressed;

    compressed = FALSE;

    /*
     * Check to see if building a frame in a virgin packet, or whether
     * we're just setting up media pointers into the frame
     */
    if (data) {
	/*
	 * We've got a virgin packet. 
	 */
	firstlong.d.lword = GETLONG(data);
	sniffdata = data;
    } else {
	/*
	 * Soiled packet. Start from datagramstart
	 */
	firstlong.d.lword = GETLONG(pak->datagramstart);

	/*
	 * High-end bridging gets it panties heavily twisted here. Be
	 * nice and spoof similar behaviour via the sniffdata pointer
	 */
	sniffdata = pak->datagramstart;
    }

    /*
     * Grab the encapsulation type, and figure out the encapsulation size.
     */
    pak->enctype = ET_HDLC;
    if (firstlong.d.byte[0] & (HDLC_BCAST >> 8))
	idb->counters.input_broad++;

    if (firstlong.d.byte[0] & (HDLC_COMPRESSED >> 8))
        compressed = TRUE;

    if (!compressed && (firstlong.d.lword == HDLC_BRIDGECODE)) {
	ether_hdr *ether;

	pak->enctype = ET_BRIDGE;
	if (GETSHORT(&sniffdata[ARPATYPE + HDLC_ENCAPBYTES]) > MAXETHERSIZE)
	    offset = ETHER_ARPA_ENCAPBYTES;
	else if (GETSHORT(&sniffdata[ETHER_8023_ENCAPBYTES + HDLC_ENCAPBYTES]) 
		== SNAPSNAP)
	    offset = ETHER_SNAP_ENCAPBYTES;
	else if (GETSHORT(&sniffdata[ETHER_NOVELL_ENCAPBYTES + HDLC_ENCAPBYTES])
		== SAPSAP_NOVELL_ETHER)
	    offset = ETHER_NOVELL_ENCAPBYTES;
	else
	    offset = ETHER_SAP_ENCAPBYTES;
	/*
	 * If we're building a virgin frame, we have to set datagramstart
	 */
	if (data) {
	    hdlc = (vhdlctype *)(pak_center(pak) - HDLC_ENCAPBYTES - offset);
	    ether = (ether_hdr *)hdlc->data;
	    pak->datagramstart = (uchar *)hdlc;
	} else {
	    hdlc = (vhdlctype *)pak->datagramstart;
	    ether = (ether_hdr *)hdlc->data;
	}
	pak->mac_start     = (uchar *)ether;
	pak->addr_start    = ether->daddr;
	pak->rif_start     = NULL;
	pak->riflen	   = 0;
        pak->info_start    = ether->data;

	/*
	 * With the advent of IRB, we need to calculate precise
	 * network start value even for a bridge packet because
	 * it might be routable.
	 * We need to consider the LLC length and start on the 
	 * 1st byte of network frame. ETHER_ARPA_ENCAPBYTES is DA+SA 
	 * + LENGTH_OR_TYPE (The minimum ether header value.)
	 * We need to consider the following two LLC cases:
	 * 1. The offset value for ETHER_SAP_ENCAPBYTES 
	 * is DA(6)+SA(6)+LENGTH(2)+DSAP(1)+SSAP(1)+CONTROL(1).
	 * 2. The offset value for ETHER_SNAP_ENCAPBYTES is 
	 * DA(6)+SA(6)+LENGTH(2)+DSAP(1)+SSAP(1)+CONTROL(1)+Org(3)
	 * +EtherType(2)
	 * 3. The offset value for Novell Ether is 
	 * DA(6)+SA(6)+LENGTH(2)
	 * 4. The offset value for ARPA is 
	 * DA(6)+SA(6)+TYPE(2)
	 */
        pak->network_start = (uchar *)ether + offset;

	/*
	 * Record just how big this encapsulation was rather than every
	 * protocol have it's own fun and wacky way to work it out.
	 */
	pak->encsize = pak->network_start - pak->datagramstart;


	return(hdlc_iqueue);	/* Use special enqueue routine */
    } else {
	/*
	 * If we're building a virgin frame, we have to set datagramstart
	 */
	if (data) {
	    /*
	     * For compressed data, start at the maximum distance from center
	     * that we can possible get.  This currently is a bridged SNAP
	     * encapsulated packet.  If you change the bridge code above for
	     * aligning the packet to have something larger, you better change
	     * this too.
	     */
	    if (compressed)
	        hdlc = (vhdlctype *)(pak_center(pak) - HDLC_ENCAPBYTES -
		        ETHER_SNAP_ENCAPBYTES);
	    else
	        hdlc = (vhdlctype *)(pak_center(pak) - HDLC_ENCAPBYTES);

	    pak->datagramstart = (uchar *)hdlc;
	} else {
	    hdlc = (vhdlctype *)pak->datagramstart;
	}
	pak->mac_start     = (uchar *)hdlc;
	pak->addr_start    = NULL;
	pak->rif_start     = NULL;
	pak->riflen	   = 0;
        pak->info_start    = NULL;
	
	/*
	 * If we have a CLNS frame, there's actually an 03 control
	 * byte after the HDLC type field (think of a standard SAP
	 * header. Then shake your head).
	 */
	if (firstlong.d.sword[1] == TYPE_CLNS)
	    pak->network_start = hdlc->data + 1;
	else
	    pak->network_start = hdlc->data;

	/*
	 * Record just how big this encapsulation was rather than every
	 * protocol have it's own fun and wacky way to work it out.
	 */
	pak->encsize = pak->network_start - pak->datagramstart;

	if (compressed)
	    return(hdlc_compressed_iqueue);
	else
	    return(idb->iqueue);
    }
}

/*
 * hdlc_autodetect_start_interface
 *
 * attempt to start hdlc on this interface. called via
 * serial autodetect registries.
 *
 * truthfully, we are hoping this will never be called
 * as most likely HDLC would be the configured encap
 * for the hwidb.
 */
static boolean hdlc_autodetect_start_interface (hwidbtype *hwidb)
{
    /*
     * sanity check
     */
    if (!hwidb) {
	return (FALSE);
    }
    if (hwidb->enctype == hwidb->enctype_cfg) {
	/*
	 * this should be pre-checked in autodetect.
	 * but just in case.
	 */
	return (TRUE);
    }
    /*
     * we don't implement switching into HDLC mode
     * undestructively from any other encaps.
     */
    if (serial_debug) {
	buginf("\nSer-Autodetect %s: switching to hdlc encaps not"
	       " supported", hwidb->hw_short_namestring);
    }
    return (FALSE);
}

/*
 * hdlc_autodetect_test
 *
 * tells us if we are seeing HDLC frames on
 * this B-channel/sync/async-line
 */
static boolean hdlc_autodetect_test (hwidbtype *hwidb, paktype *pak)
{

    vhdlctype *hdlc;

    /*
     * sanity check
     */
    if ((!pak) || (!hwidb)) {
	return (FALSE);
    }

    if (serial_debug) {
	uchar *data = pak->datagramstart;
	int i;

	buginf("\nHDLC: Autodetect trying to detect HDLC mode on %s",
	       (hwidb) ? hwidb->hw_short_namestring : "unknown");

	buginf("\nHDLC sampled pkt:  %d bytes: ", pak->datagramsize);
	for (i=0; i<((pak->datagramsize < 10)?pak->datagramsize : 10);i++)
	    buginf(" %X", *(data+i));
    }
    return (FALSE);  /* we still don't have the correct code below */
    switch (GETSHORT(pak->datagramstart)) {

      case HDLC_BCAST:
      case HDLC_STATION:
      case HDLC_PADDED:

	hdlc = (vhdlctype *)(pak->datagramstart);
	if (LINK_ILLEGAL !=
	    serial2link(GETSHORT(&hdlc->var_hdlctype) & 0xFFFF)) {
	    return (TRUE);
	}
	break;

      case HDLC_COMPRESSED:
      default:

	break;
    }

    return (FALSE);
}


/*
 * hdlc_setup
 * Set up a serial interface to use raw HDLC
 */
boolean hdlc_setup (parseinfo *csb, hwidbtype *idb)
{
    idbtype *swidb;
    serialsb *ssb;

    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);
    idb->enctype = idb->enctype_def = idb->enctype_cfg = ET_HDLC;
    /*
     * Set the fastswitch flavor
     */
    idb->fast_switch_type = FS_HDLC;

    idb->vencap = hdlc_vencapsulate;
    idb->bridge_vencap = (bridge_vencap_t)return_null;
    idb->deencap = hdlc_deencapsulate;
    idb->getlink = hdlc_getlink;
    set_default_queueing(idb);
    idb->iqueue = raw_enqueue;
    idb->parse_packet = hdlc_parse_packet;
    idb->periodic = hdlc_periodic;
    idb->extract_hwaddr = invalidate_hwaddr;
    idb->extract_snpa = hdlc_extract_addr;
    idb->broadcast = hdlc_broadcast;
    idb->show_support = NULL;
    idb->lineaction = hdlc_cstate;
    idb->linestate = hdlc_linestate;
    holdq_reset(idb);
    idb->hdlc_lineup = FALSE;
    hdlc_keepalive_init(idb);
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	reg_invoke_media_enctype_setup(swidb, ET_HDLC);
	swidb->ip_enctype = ET_HDLC;
	swidb->tbridge_media = TBR_HDLC_TRANSIT;
 	swidb->ip_routecache_same = swidb->ip_routecache_same_def = TRUE;
    }
    idb->encsize = HDLC_HEADER_SIZE;
    idb->span_encapsize = idb->encsize;
    if (!idb->name)
        idb->name = (idb->status & IDB_HSSI) ? IFNAME_HSSI : IFNAME_SERIAL;

    /*
     * Declare that the line is not in loopback.  HDLC is the only
     * protocol that can determine this.  Also prep the sent field so
     * that we don't compare 0 == 0 before we sent the first keepalive
     * packet.
     *
     * Moved this to here from setencap as setencap was not necessarily
     * called as HDLC is the default protocol so there may not be an
     * encapsulation hdlc command in the configuration.
     */
    ssb->hdlc_unique_sent = create_unique_number();
    idb->inloopback = FALSE;
    /*
     * If idb->set_maxdgram is ZERO then it's during system init. and we 
     * don't call set_maxdgram()!
     */
    if (idb->set_maxdgram) {
        if (!(idb->status & IDB_HSSI))
            (*idb->set_maxdgram)(idb, idb->maxmtu + ETHER_BAGGAGE, 
	            idb->maxmtu + ETHER_BAGGAGE + HDLC_ENCAPBYTES);
        else /* HSSI */
            (*idb->set_maxdgram)(idb, idb->maxmtu + DS3_OVERHEAD_BYTES, 
	            idb->maxmtu + DS3_BAGGAGE);
        (*(idb->reset))(idb);
    }
    return(TRUE);
}

static boolean serial_hw_set_config (hwidbtype *hwidb, itemlist *ilist,
				     serialsb *ssb)
{
    itemdesc *item;
    boolean any_unhandled = FALSE;   /* assume we've handled all items */
    tinybool this_item_unhandled;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;

	switch (item->item) {
	  case SERIAL_HW_HSSI_ICLK:
	    ssb->hssi_iclk = item->u.l_value;
	    break;

	  case SERIAL_HW_HSSI_LC_INHIBIT:
	    ssb->hssi_lc_inhibit = item->u.l_value;
	    break;

	  case SERIAL_HW_SERIAL_PROTOCOL:
	    ssb->serial_protocol = item->u.l_value;
	    break;

	  case SERIAL_HW_SERIAL_RESTARTDELAY:
	    ssb->serial_restartdelay = item->u.l_value;
	    break;

	  case SERIAL_HW_SERIAL_RESTARTDELAY_DEF:
	    ssb->serial_restartdelay_def = item->u.l_value;
	    break;

	  case SERIAL_HW_SERIAL_CLOCKINDEX:
	    ssb->serial_clockindex = item->u.l_value;
	    break;

	  case SERIAL_HW_DCE_TERMINAL_TIMING_ENABLE:
	    ssb->dce_terminal_timing_enable = item->u.l_value;
	    break;

	  case SERIAL_HW_INVERT_TXC:
	    ssb->invert_txc = item->u.l_value;
	    break;

	  case SERIAL_HW_NRZI_ENABLE:
	    ssb->nrzi_enable = item->u.l_value;
	    break;

	  case SERIAL_HW_CRC_32_ENABLED:
	    ssb->crc_32_enabled = item->u.l_value;
	    break;

	  case SERIAL_HW_START_SLOT:
	    ssb->start_slot = item->u.s_value[0];
	    break;

	  case SERIAL_HW_STOP_SLOT:
	    ssb->stop_slot = item->u.s_value[0];
	    break;

	  case SERIAL_HW_TIMESLOT16_ENABLED:
	    ssb->timeslot16_enabled = item->u.l_value;
	    break;

	  case SERIAL_HW_CRC4_ENABLED:
	    ssb->crc4_enabled = item->u.l_value;
	    break;

	  case SERIAL_HW_CLOCKSOURCE_ENABLED:
	    ssb->clocksource_enabled = item->u.l_value;
	    break;

	  case SERIAL_HW_RX_SYNC_DISABLE:
	    ssb->rx_sync_disable = item->u.l_value;
	    break;

	  case SERIAL_HW_IGNORE_DCD:
	    ssb->ignore_dcd = item->u.l_value;
	    break;

	  case SERIAL_HW_DOWNIFLOOPED:
	    ssb->downiflooped = item->u.l_value;
	    break;

          case SERIAL_HW_CONTROLLED_CARRIER:
            ssb->controlled_carrier = item->u.l_value;
            break;

	  case SERIAL_HW_RTS_TIMEOUT:
	    ssb->hdx_rts_timeout = item->u.l_value;
	    break;

	  case SERIAL_HW_CTS_DELAY:
	    ssb->hdx_cts_delay = item->u.l_value;
	    break;

          case SERIAL_HW_RTS_DROP_DELAY:
            ssb->hdx_rts_drop_delay = item->u.l_value;
            break;

          case SERIAL_HW_CTS_DROP_TIMEOUT:
            ssb->hdx_cts_drop_timeout= item->u.l_value;
            break;

          case SERIAL_HW_DCD_TXSTART_DELAY:
            ssb->hdx_dcd_txstart_delay = item->u.l_value;
            break;

          case SERIAL_HW_DCD_DROP_DELAY:
            ssb->hdx_dcd_drop_delay = item->u.l_value;
            break;

          case SERIAL_HW_TRANSMIT_DELAY:
            ssb->hdx_transmit_delay = item->u.l_value;
            break;

          case SERIAL_HW_HDX_LOG_ENABLE:
            ssb->hdx_log_enable = item->u.l_value;
            break;

	  default:
	    this_item_unhandled = TRUE;
	    any_unhandled = TRUE;
	    break;
	}

	if (!this_item_unhandled)
	    item->handled = TRUE;
    }

    return(!any_unhandled);
}
 

static boolean serial_hw_read_config (hwidbtype *hwidb, itemlist *ilist,
				      serialsb *ssb)
{
    itemdesc *item;
    boolean any_unhandled = FALSE;   /* assume we've handled all items */
    tinybool	this_item_unhandled;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;

	switch (item->item) {
	  case SERIAL_HW_HSSI_ICLK:
	    item->u.l_value = ssb->hssi_iclk;
	    item->length = sizeof(boolean);
	    break;

	  case SERIAL_HW_HSSI_LC_INHIBIT:
	    item->u.l_value = ssb->hssi_lc_inhibit;
	    item->length = sizeof(boolean);
	    break;

	  case SERIAL_HW_SERIAL_PROTOCOL:
	    item->u.l_value = ssb->serial_protocol;
	    item->length = sizeof(int);
	    break;

	  case SERIAL_HW_SERIAL_RESTARTDELAY:
	    item->u.l_value = ssb->serial_restartdelay;
	    item->length = sizeof(ulong);
	    break;

	  case SERIAL_HW_SERIAL_RESTARTDELAY_DEF:
	    item->u.l_value = ssb->serial_restartdelay_def;
	    item->length = sizeof(ulong);
	    break;

	  case SERIAL_HW_SERIAL_CLOCKINDEX:
	    item->u.l_value = ssb->serial_clockindex;
	    item->length = sizeof(int);
	    break;

	  case SERIAL_HW_DCE_TERMINAL_TIMING_ENABLE:
	    item->u.l_value = ssb->dce_terminal_timing_enable;
	    item->length = sizeof(boolean);
	    break;

	  case SERIAL_HW_INVERT_TXC:
	    item->u.l_value = ssb->invert_txc;
	    item->length = sizeof(boolean);
	    break;

	  case SERIAL_HW_NRZI_ENABLE:
	    item->u.l_value = ssb->nrzi_enable;
	    item->length = sizeof(boolean);
	    break;

	  case SERIAL_HW_CRC_32_ENABLED:
	    item->u.l_value = ssb->crc_32_enabled;
	    item->length = sizeof(boolean);
	    break;

	  case SERIAL_HW_START_SLOT:
	    item->u.s_value[0] = ssb->start_slot;
	    item->length = sizeof(ushort);
	    break;

	  case SERIAL_HW_STOP_SLOT:
	    item->u.s_value[0] = ssb->stop_slot;
	    item->length = sizeof(ushort);
	    break;

	  case SERIAL_HW_TIMESLOT16_ENABLED:
	    item->u.l_value = ssb->timeslot16_enabled;
	    item->length = sizeof(boolean);
	    break;

	  case SERIAL_HW_CRC4_ENABLED:
	    item->u.l_value = ssb->crc4_enabled;
	    item->length = sizeof(boolean);
	    break;

	  case SERIAL_HW_CLOCKSOURCE_ENABLED:
	    item->u.l_value = ssb->clocksource_enabled;
	    item->length = sizeof(boolean);
	    break;

	  case SERIAL_HW_RX_SYNC_DISABLE:
	    item->u.l_value = ssb->rx_sync_disable;
	    item->length = sizeof(boolean);
	    break;

	  case SERIAL_HW_IGNORE_DCD:
	    item->u.l_value = ssb->ignore_dcd;
	    item->length = sizeof(boolean);
	    break;

	  case SERIAL_HW_IGNORE_LOCAL_LOOPBACK:
	    item->u.l_value = ssb->ignore_local_loopback;
	    item->length = sizeof(boolean);
	    break;

	  case SERIAL_HW_DOWNIFLOOPED:
	    item->u.l_value = ssb->downiflooped;
	    item->length = sizeof(boolean);
	    break;

          case SERIAL_HW_CONTROLLED_CARRIER:
            item->u.l_value = ssb->controlled_carrier;
            item->length = sizeof(boolean);
            break;

	  case SERIAL_HW_RTS_TIMEOUT:
            item->u.l_value = ssb->hdx_rts_timeout;
            item->length = sizeof(ulong);
	    break;

	  case SERIAL_HW_CTS_DELAY:
	    item->u.l_value = ssb->hdx_cts_delay;
            item->length = sizeof(ulong);
	    break;

	  case SERIAL_HW_RTS_DROP_DELAY:
	    item->u.l_value = ssb->hdx_rts_drop_delay;
            item->length = sizeof(ulong);
	    break;

	  case SERIAL_HW_CTS_DROP_TIMEOUT:
	    item->u.l_value = ssb->hdx_cts_drop_timeout;
            item->length = sizeof(ulong);
	    break;

	  case SERIAL_HW_DCD_TXSTART_DELAY:
	    item->u.l_value = ssb->hdx_dcd_txstart_delay;
            item->length = sizeof(ulong);
	    break;

	  case SERIAL_HW_DCD_DROP_DELAY:
	    item->u.l_value = ssb->hdx_dcd_drop_delay;
            item->length = sizeof(ulong);
	    break;

	  case SERIAL_HW_TRANSMIT_DELAY:
	    item->u.l_value = ssb->hdx_transmit_delay;
            item->length = sizeof(ulong);
	    break;

          case SERIAL_HW_HDX_LOG_ENABLE:
            item->u.l_value = ssb->hdx_log_enable;
            item->length = sizeof(boolean);
            break;

	  default:
	    this_item_unhandled = TRUE;
	    any_unhandled = TRUE;
	    break;
	}

	if (!this_item_unhandled)
	    item->handled = TRUE;
    }
    
    return(!any_unhandled);
}


static boolean serial_hw_set_state (hwidbtype *hwidb, itemlist *ilist,
				    serialsb *ssb)
{
    itemdesc *item;
    boolean any_unhandled = FALSE;   /* assume we've handled all items */
    tinybool this_item_unhandled;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;

	switch (item->item) {
	  case SERIAL_HW_HSSI_LOOP_REQ:
	    ssb->hssi_loop_req = item->u.l_value;
	    break;

	  case SERIAL_HW_APPLIQUE:
	    ssb->applique = item->u.l_value;
	    break;

	  case SERIAL_HW_RX_DISABLED:
	    ssb->rx_disabled = item->u.l_value;
	    break;

	  case SERIAL_HW_RX_AIS:
	    ssb->rx_ais = item->u.l_value;
	    break;

	  case SERIAL_HW_RX_LOF:
	    ssb->rx_lof = item->u.l_value;
	    break;

	  case SERIAL_HW_RX_LOS:
	    ssb->rx_los = item->u.l_value;
	    break;

	  case SERIAL_HW_RX_RAI:
	    ssb->rx_rai = item->u.l_value;
	    break;

	  case SERIAL_HW_SI_APPLIQUE:
	    ssb->si_applique = item->u.l_value;
	    break;

	  case SERIAL_HW_SI_RX_DISABLED:
	    ssb->si_rx_disabled = item->u.l_value;
	    break;

	  case SERIAL_HW_SI_RX_AIS:
	    ssb->si_rx_ais = item->u.l_value;
	    break;

	  case SERIAL_HW_SI_RX_LOF:
	    ssb->si_rx_lof = item->u.l_value;
	    break;

	  case SERIAL_HW_SI_RX_LOS:
	    ssb->si_rx_los = item->u.l_value;
	    break;

	  case SERIAL_HW_SI_RX_RAI:
	    ssb->si_rx_rai = item->u.l_value;
	    break;

	  case SERIAL_HW_CLEAR_STATS:
	    ssb->si_applique = ssb->applique;
	    ssb->si_rx_disabled = ssb->rx_disabled;
	    ssb->si_rx_ais = ssb->rx_ais;
	    ssb->si_rx_lof = ssb->rx_lof;
	    ssb->si_rx_los = ssb->rx_los;
	    ssb->si_rx_rai = ssb->rx_rai;
	    break;

	  case SERIAL_HW_SERIAL_RESTARTTIMER:
	    ssb->serial_restarttimer = item_get_timestamp(ilist, item);
	    break;

	  default:
	    this_item_unhandled = TRUE;
	    any_unhandled = TRUE;
	    break;
	}

	if (!this_item_unhandled)
	    item->handled = TRUE;
    }

    return(!any_unhandled);
}


static boolean serial_hw_read_state (hwidbtype *hwidb, itemlist *ilist,
				     serialsb *ssb)
{
    itemdesc *item;
    boolean any_unhandled = FALSE;   /* assume we've handled all items */
    tinybool this_item_unhandled = FALSE;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;

	switch (item->item) {
	  case SERIAL_HW_HSSI_LOOP_REQ:
	    item->u.l_value = ssb->hssi_loop_req;
	    item->length = sizeof(boolean);
	    break;

	  case SERIAL_HW_APPLIQUE:
	    item->u.l_value = ssb->applique;
	    item->length = sizeof(ulong);
	    break;

	  case SERIAL_HW_RX_DISABLED:
	    item->u.l_value = ssb->rx_disabled;
	    item->length = sizeof(ulong);
	    break; 

	  case SERIAL_HW_RX_AIS:
	    item->u.l_value = ssb->rx_ais;
	    item->length = sizeof(ulong);
	    break;

	  case SERIAL_HW_RX_LOF:
	    item->u.l_value = ssb->rx_lof;
	    item->length = sizeof(ulong);
	    break;

	  case SERIAL_HW_RX_LOS:
	    item->u.l_value = ssb->rx_los;
	    item->length = sizeof(ulong);
	    break;

	  case SERIAL_HW_RX_RAI:
	    item->u.l_value = ssb->rx_rai;
	    item->length = sizeof(ulong);
	    break;

	  case SERIAL_HW_SI_APPLIQUE:
	    item->u.l_value = ssb->si_applique;
	    item->length = sizeof(ulong);
	    break;

	  case SERIAL_HW_SI_RX_DISABLED:
	    item->u.l_value = ssb->si_rx_disabled;
	    item->length = sizeof(ulong);
	    break; 

	  case SERIAL_HW_SI_RX_AIS:
	    item->u.l_value = ssb->si_rx_ais;
	    item->length = sizeof(ulong);
	    break;

	  case SERIAL_HW_SI_RX_LOF:
	    item->u.l_value = ssb->si_rx_lof;
	    item->length = sizeof(ulong);
	    break;

	  case SERIAL_HW_SI_RX_LOS:
	    item->u.l_value = ssb->si_rx_los;
	    item->length = sizeof(ulong);
	    break;

	  case SERIAL_HW_SI_RX_RAI:
	    item->u.l_value = ssb->si_rx_rai;
	    item->length = sizeof(ulong);
	    break;

	  case SERIAL_HW_APPLIQUE_STAT:
	    item->u.l_value = ssb->applique - ssb->si_applique;
	    item->length = sizeof(ulong);

	  case SERIAL_HW_RX_AIS_STAT:
	    item->u.l_value = ssb->rx_ais - ssb->si_rx_ais;
	    item->length = sizeof(ulong);
	    break;

	  case SERIAL_HW_RX_LOF_STAT:
	    item->u.l_value = ssb->rx_lof - ssb->si_rx_lof;
	    item->length = sizeof(ulong);
	    break;

	  case SERIAL_HW_RX_LOS_STAT:
	    item->u.l_value = ssb->rx_los - ssb->si_rx_los;
	    item->length = sizeof(ulong);
	    break;

	  case SERIAL_HW_RX_RAI_STAT:
	    item->u.l_value = ssb->rx_rai - ssb->si_rx_rai;
	    item->length = sizeof(ulong);
	    break;	
	  
	  case SERIAL_HW_SERIAL_RESTARTTIMER:
	    item_put_timestamp(ilist, item, ssb->serial_restarttimer);
	    item->length = sizeof(sys_timestamp);
	    break;

	  default:
	    this_item_unhandled = TRUE;
	    any_unhandled = TRUE;
	    break;
	}

	if (!this_item_unhandled)
	    item->handled = TRUE;
    }

    return(!any_unhandled);
}

static boolean serial_hwidb_state_config (hwidbtype *hwidb, ulong opcode,
					  itemlist *ilist)
{
    boolean ret;
    serialsb *ssb;

    if ((hwidb == NULL) && (ilist == NULL))
	return (FALSE);

    if (ilist->item_count == -1)
	ilist->item_count = itemlist_count(ilist);

    ssb = idb_use_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
    if (ssb == NULL)
	return (FALSE);

    switch (opcode) {
      case IDB_CONTROL_SET_CONFIG:
	ret = serial_hw_set_config(hwidb, ilist, ssb);
	break;
	
      case IDB_CONTROL_READ_CONFIG:
	ret = serial_hw_read_config(hwidb, ilist, ssb);
	break;
	
      case IDB_CONTROL_SET_STATE:
	ret = serial_hw_set_state(hwidb, ilist, ssb);
	break;
	
      case IDB_CONTROL_READ_STATE:
	ret = serial_hw_read_state(hwidb, ilist, ssb);
	break;
	
      default:
	ret = FALSE;
	break;
    }
    idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);

    if (!ret) {
	itemlist_reset(ilist);
	ret = (*ssb->super_state_config)(hwidb, opcode, ilist);
    }
    return (ret);
}


/*
 * create_serial_sb
 * Create a serial subblock and attach it to the IDB.  Also add idb to the 
 * serial idb list.  If we can't alloc a serial subblock, crash and burn.
 */
void create_serial_sb (hwidbtype *hwidb)
{
    serialsb *ssb;
    hwidb_sb_t type = HWIDB_SB_SERIAL;

    /*
     * If already there, don't do it again
     */
    ssb = idb_get_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
    if (ssb == NULL) {
	ssb = malloc_named(sizeof(serialsb), "IDB: Serial Info");
	
	if (ssb == NULL) {
	    /*
	     * This is way bad, we have no memory, crash and burn
	     */
	    crashdump(0);
	}
	
	if (!idb_add_hwsb(hwidb, &type, ssb)) {
	    if (serial_debug)
		buginf("Can't get subblock\n");
	}
     } else {
	return;	/* Serial subblock exists already */
    }

    /*
     * Stack the state/config vectors, if not already done
     */
    if (hwidb->state_config != serial_hwidb_state_config) {
	ssb->super_state_config = hwidb->state_config;
	hwidb->state_config = serial_hwidb_state_config;  
    }  
}

/*
 * serial_idb_enqueue()
 * Add a serial idb to the idb queues,
 */
void serial_idb_enqueue (hwidbtype *hwidb)
{
    ulong idblist_type; 

    if (!(hwidb->status & IDB_SERIAL)) return;

    /*
     * Check to make sure this isn't a virtual idb.  If so, don't add to
     * the serial idb list.
     */
    if (!(hwidb->status & IDB_VIRTUAL)) {
	/*
	 * Check for serial hwidb list if not there, create it.
	 */
	if (idb_get_listhead(HWIDBLIST_SERIAL) == NULL) {
	    if (!idb_create_list(HWIDBLIST_SERIAL, &idblist_type) ||
		idblist_type != HWIDBLIST_SERIAL) {
		if (serial_debug) {
		    idb_report_create_error("serial");
		}
		return;
	    }
	}
	
	/*
	 * Add the idb to the serial idb list
	 */
	if (!idb_add_hwidb_to_list(hwidb, HWIDBLIST_SERIAL)) {
	    if (serial_debug) {
		idb_report_add_error("serial");
	    }
	}
    }
}


/*
 * serial_idb_dequeue()
 * Dequeue an idb from the serial idb list
 */
void serial_idb_dequeue (hwidbtype *hwidb)
{
    if (!idb_remove_from_list(hwidb, HWIDBLIST_SERIAL)) {
	if (serial_debug) {
	    idb_report_removal_error("serial");
	}
    }
}


/*
 * serial_idb_init
 * Initialize serial idb common stuff
 * If you want a name other than hssi or serial, you should init it
 * before this routine, as hdlc_setup() will set it to serial or hssi.
 */
void serial_idb_init (hwidbtype *hwidb, ushort maxmtu, ulong maxdgram,
		      ulong vis_band, ulong delay)
{
    idbtype *swidb = hwidb->firstsw;
    serialsb *ssb;

    create_serial_sb(hwidb);

    ssb = idb_use_hwsb_inline(hwidb, HWIDB_SB_SERIAL);

    hwidb->setencap = serial_encapsulation;
    hwidb->maxmtu = hwidb->maxmtu_def = maxmtu;
    hwidb->firstsw->sub_mtu = hwidb->maxmtu_def;

    ssb->serial_restartdelay = ssb->serial_restartdelay_def = (30 * ONESEC);
    set_default_interface_bandwidth(swidb, vis_band);
    set_default_interface_delay(swidb, delay);

    holdq_init(hwidb, HDLC_HOLDQ);
    /* Just to be sure it doesn't get called by hdlc_setup */
    hwidb->set_maxdgram = NULL;
    hdlc_setup(NULL, hwidb);
    hwidb->max_pak_size_def = MAX_SERIAL_PAK;
    if_maxdgram_adjust(hwidb, maxdgram);

    hwidb->dte_interface = FALSE;

    idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
}


/*
 * serial_high_priority
 * Return true if the packet is a high priority serial packet that we should
 * take pains to keep when we are short on buffers, ie - an HDLC or
 * Frame Relay keepalive packet:
 */
boolean serial_high_priority (ushort hdlc_type)
{
    if (hdlc_type == TYPE_REVERSE_ARP)
	return(TRUE);
    return(FALSE);
}

/*
 * serial_queue_for_process
 * Attempt to queue this packet for process level. If packet is not queued
 * successfully, it will be freed.
 */
void serial_queue_for_process (
    hwidbtype *idb,
    paktype *pak)

{
    void (*siqueue)(hwidbtype *, paktype *);
    uchar *ptr, *testlong;
    ushort count;

    /*
     * Use idb->parse_packet to see if we need to change datagramstart,
     * and, to count broadcast packets:
     */
    count = pak->datagramsize;
    ptr = pak->datagramstart;
    testlong = ptr;
    if (is_smds(idb)) {
	/*
	 * Snip the last 4 bytes of the encapsulation
	 */
	testlong = pak->network_start - sizeof(ulong);
    }

#ifdef ALIGN_DATAGRAM
    siqueue = (*idb->parse_packet)(idb, pak, testlong);
#else
    siqueue = (*idb->parse_packet)(idb, pak, NULL);
#endif

    if (siqueue == NULL) {
	idb->counters.input_error++;
	idb->unknown_encaps++;
	if (serial_debug)
	    buginf("\n%s: Illegal packet encapsulation = 0x%04x\n",
		   idb->hw_namestring, GETLONG(ptr));
	retbuffer(pak);
	MAYBE_DROPPED(idb);
	return;
    }

    /*
     * Did idb->parse_packet change datagramstart?
     */
    if (pak->datagramstart != ptr) {
	paktype *tmp;

	/*
	 * Yes, we need to copy the packet into a new buffer:
	 */
	tmp = pak;
	/*
	 * 
	 */
	pak = input_getbuffer((uint)count, idb);
	if (pak) {
	    /*
 	     * If IRB(Integrated Routing and Bridging is enabled,
	     * there is a possibility that we overwrote the input 
	     * interface when making bridge/route decision in tbridge
	     * fast-switching code. Restore irb_rcvd_input. We need to
	     * do this before 2nd parse_packet because inside the parse
	     * packet we need to tell if this packet is from BVI and
	     * perform some layer 3 parsing
	     */
	    if (is_bvi(tmp->if_input->hwptr)) {
		change_if_input(pak, tmp->if_input);
		pak->irb_rcvd_input = tmp->irb_rcvd_input;
	    }

	    /* Call parse_packet again to set up the new packet. */
	    (*idb->parse_packet)(idb, pak, testlong);
	    pak->datagramsize = count;
	    bcopy(ptr, pak->datagramstart, (uint)count);
	    pak->flags = tmp->flags;
	    /*
	     * Save the rxtype for later getlink support
	     */
	    pak->rxtype = tmp->rxtype;
	    retbuffer(tmp);
	} else {
	    /*
	     * Failed to get new buffer to copy packet:
	     */
	    retbuffer(tmp);
	    MAYBE_DROPPED(idb);
	    return;
	}
    }

    /*
     * Toss it to the process level
     */
    idb->counters.rx_cumbytes += pak->datagramsize;
    idb->counters.inputs++;

    if (idb->remif != NULL) {
	(*idb->iqueue)(idb, pak);
    } else {
	(*siqueue)(idb, pak);
    }
}


/* no pak-by-pak for this encap-type... */
static paktype *default_encaps_compress (paktype *pak)
{ 
  return (pak);
}

/*
 * hdlc_pbp_compress
 * Compress (if possible) an HDLC packet.  Leave any and all encaps alone...
 * start at info_start.  Compress the body, and then update datagram
 * size.  Set the compressed bit in the header if needed.
 */
static paktype *hdlc_pbp_compress (paktype *pak)
{
    vhdlctype *hdlc;
    paktype *newpak;
    short headerSize;
    boolean stat;
    
    if ( (!pak->if_output->hwptr->comp_db)
       || (pak->if_output->hwptr->comp_db->comp_configed == COMP_NOT_ENABLED)
       || (pak->info_start == NULL) )
	return pak;

    hdlc = (vhdlctype *)(pak->info_start - sizeof(vhdlctype));
    if (hdlc->var_hdlctype == TYPE_BRIDGE)	/* HACK CSCdi63268 */
      return pak;
    
    /*
     * If the packet is shared then it may have all sorts of wierd things
     * going on and we shouldn't play with the packet proper.  Make a copy
     * and continue. 
     */
    if (pak->refcount > 1) {
	newpak = pak_duplicate(pak);
	if (newpak == NULL) {
	    if (serial_debug)
		buginf("\ncompression: pak_duplicate failed on originating packet");
	    return(pak);
	}
	datagram_done(pak);
	pak = newpak;
	hdlc = (vhdlctype *)(pak->info_start - sizeof(vhdlctype));
    }
    /*
     * The compress routine wants to start compressing at info_start
     * for datagramsize.  So "strip" off the HDLC header before calling
     * the compression routine.
     */
    headerSize = pak->info_start - pak->datagramstart;
    pak->datagramsize -= headerSize;

    stat = reg_invoke_late_q_compress(COMP_STAC, pak, 0);
    
    pak->datagramsize += headerSize;
    if (stat)
	hdlc->var_hdlcflags |= HDLC_COMPRESSED;
    else
        hdlc->var_hdlcflags &= ~HDLC_COMPRESSED;
    return(pak);
}


/*
 * hdlc_compressed_iqueue
 * We received a compressed HDLC packet.  We need to uncompress it, and
 * align it correctly in the packet.  So this has to re-call hdlc_parse_packet
 * which does the work for us.
 */
static void hdlc_compressed_iqueue (hwidbtype *idb, paktype *pak)
{
  vhdlctype vhdlc;
  vhdlctype *hdlc;
  void (*iqueue)(hwidbtype *, paktype *);
  
  hdlc = (vhdlctype *)pak->datagramstart;
  vhdlc = *hdlc;
  if ( reg_used_late_q_uncompress(COMP_STAC) ) {
    pak = reg_invoke_late_q_uncompress(COMP_STAC, pak, 0);
  } else {
    pak->flags |= PAK_ABORT;
    errmsg(&msgsym(COMPREG, LINK), pak->if_input->namestring);
  }
  pak->datagramstart -= HDLC_ENCAPBYTES;
  pak->datagramsize += HDLC_ENCAPBYTES;
  hdlc = (vhdlctype *)pak->datagramstart;

  /*
   * Put the hdlc header back on and
   * clear the HDLC_COMPRESSED flag before calling parse_packet as
   * we've now uncompressed the packet...
   */
  hdlc->var_hdlcflags = vhdlc.var_hdlcflags & ~HDLC_COMPRESSED;
  hdlc->var_hdlctype = vhdlc.var_hdlctype;
  iqueue = hdlc_parse_packet(idb, pak, NULL);
  (*iqueue)(idb, pak);
}


/*
 * serial_pulsetimer
 * Check to see if pulse timer is set and reset interface if timer expired.
 * Pulse timers are used to ensure that DTR is held low for a minimum
 * amount of time.  This is useful in resynchronizing encryption gear.
 * Called by SERVICE_IF_ONESEC.
 */
static void serial_pulsetimer (hwidbtype *idb)
{
    if (TIMER_RUNNING_AND_AWAKE(idb->serial_pulsetimer)) {
        (*idb->reset)(idb);
    }
}


/*
 * serial_default_check_mtu
 * does nothing if no platform specific mtu check needs to be
 * performed
 */
boolean serial_default_check_mtu (hwidbtype *hwidb, ushort mtu)
{
    return (TRUE);
}
/*
 * serial_init
 *
 * Initialize global serial related information.
 */
static void serial_init (subsystype *subsys)
{
    /*
     * Add serial debugging support
     */
    serial_debug_init();

    /*
     * Add parser support
     */
    serial_parser_init();

    reg_add_keepalive_changed(ET_HDLC,
		      hdlc_keepalive_init, "hdlc_keepalive_init");
    reg_add_if_onesec(serial_pulsetimer, "serial_pulsetimer");

    reg_add_media_serial_setup(ET_HDLC, hdlc_setup, "hdlc_setup");
    reg_add_media_hwidb_enqueue_private_list(serial_idb_enqueue, 
					     "serial_idb_enqueue");
    reg_add_default_media_hdlc_macaddr(hdlc_bcast_default,
				       "hdlc_bcast_default");
    reg_add_default_media_serial_to_link(LINK_ILLEGAL, "link_illegal");
    reg_add_default_media_check_mtu(serial_default_check_mtu,
                                  "serial_default_check_mtu");
  
    reg_add_default_media_link_to_serial(0L, "illegal type");

    reg_add_encaps_compress(ET_HDLC, hdlc_pbp_compress, "hdlc_pbp_compress");
    reg_add_default_encaps_compress(default_encaps_compress,
				    "default_encaps_compress");

    reg_add_if_onesec(serial_restart, "serial_restart");

    reg_add_media_serial_hdlc_address(hdlc_address, "hdlc_address");
    reg_add_media_physical_layer_command(physical_layer_command,
                                         "physical_layer_command");
    reg_invoke_serial_autodetect_register_encaps(ET_HDLC,
						 hdlc_autodetect_test,
						 hdlc_autodetect_start_interface);
}


/*
 * serial_pulse_DTR()
 * If we pulse DTR on a reset, make sure we start the pulse timer.
 * Clear timer and finish reset if pulse timer has expired.
 * This code is useful for encryption boxes that need DTR pulses
 * to re-synchronize.
 */
boolean serial_pulse_DTR (hwidbtype *idb)
{
    if (idb->serial_pulseinterval) {
	if (TIMER_RUNNING(idb->serial_pulsetimer)) {
	    if (SLEEPING(idb->serial_pulsetimer)) {
		return(FALSE);
	    } else
		TIMER_STOP(idb->serial_pulsetimer);
	} else {
	    TIMER_START(idb->serial_pulsetimer, idb->serial_pulseinterval);
	    return(FALSE);
	}
    }
    return(TRUE);
}

/*
 * Serial subsystem header
 */
#define SERIAL_MAJVERSION 1
#define SERIAL_MINVERSION 0
#define SERIAL_EDITVERSION 1

SUBSYS_HEADER (serial, SERIAL_MAJVERSION, SERIAL_MINVERSION,
	       SERIAL_EDITVERSION,
	       serial_init, SUBSYS_CLASS_KERNEL,
	       NULL,
	       NULL);
      
