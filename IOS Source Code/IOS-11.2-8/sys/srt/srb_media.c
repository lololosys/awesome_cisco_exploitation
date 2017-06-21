/* $Id: srb_media.c,v 3.3.62.4 1996/08/15 22:30:34 motto Exp $
 * $Source: /release/112/cvs/Xsys/srt/srb_media.c,v $
 *------------------------------------------------------------------
 * srb_media.c -- media dependent SRB routines.
 *
 * 13-April-1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: srb_media.c,v $
 * Revision 3.3.62.4  1996/08/15  22:30:34  motto
 * CSCdi62629:  Allignment and spurious memory errors performing chan
 * encap: Bad VCN
 * Branch: California_branch
 *
 * Revision 3.3.62.3  1996/07/23  18:49:14  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.3.62.2  1996/05/17  12:14:33  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.48.2  1996/04/26  07:59:02  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.3.48.1  1996/04/03  21:59:59  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.62.1  1996/03/18  22:11:30  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/13  02:02:48  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.1  1996/02/20  18:50:03  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  18:57:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:42:59  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:32:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/09/05  03:55:15  ppearce
 * CSCdi39777:  LINK-3-BADMACREG: non-existent MACADDR registry for link43
 *   CSNA internal LAN should accept but drop (for now) LNM packets
 *
 * Revision 2.5  1995/09/01  06:02:21  ppearce
 * CSCdi39705:  MEMD-3-BADVCN when using CSNA with RSRB/TCP local-ack
 *   Re-work how CSNA header is added before sending pak to CIP interface
 *
 * Revision 2.4  1995/08/07  05:44:14  richl
 * CSCdi38315:  makefile rework
 *              header changes for rif_util.c and tring_inline.h
 *              move dlsw_work to its own file, create libibm_util.a for
 *              utility routines used by dlsw and rsrb
 *
 * Revision 2.3  1995/07/20  19:00:35  ppearce
 * CSCdi37404:  CIP-to-RSRB with TCP/lack not working
 *
 * Revision 2.2  1995/06/13  03:39:08  alam
 * CSCdi35570:  Remove extraneous functions
 * Use rsrb_token_macaddr for FDDI.
 *
 * Revision 2.1  1995/06/07  23:02:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "media_registry.h"
#include "interface_generic.h"
#include "packet.h"
#include "../if/tring.h"
#include "../if/rif_inline.h"
#include "../h/address.h"
#include "../llc2/llc2.h"
#include "../llc2/llc2_inline.h"


/*
 * rsrb_ether_macaddr
 *
 * Turn an RSRB address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR, indexed by LINK_RSRB
 */
static boolean rsrb_ether_macaddr (paktype *pak, long address,
				   int *enctype, uchar **daddr,
				   uchar const **other_info)
{
    *enctype = ET_SNAP;
    *daddr = (uchar *)address;
    *other_info = zerosnap;
    return(TRUE);
}

/*
 * rsrb_token_macaddr
 *
 * Turn an RSRB address into an encapsulation type and a MAC address
 * Called by MEDIA_TOKEN_MACADDR and MEDIA_FDDI_MACADDR, indexed by LINK_RSRB
 */
static boolean rsrb_token_macaddr (paktype *pak, long address,
				   int *enctype, uchar **daddr,
				   uchar const **other_info, ulong *rif_flags)
{
    idbtype *interface;

    interface = pak->if_output;
    *enctype = ET_SNAP;
    *daddr = (uchar *)address;
    *other_info = zerosnap;
    *rif_flags = determine_rif(interface, MULTIRING_BIT_OTHER);
    return(TRUE);
}

/*
 * rsrb_frame_macaddr
 *
 * Set broadcast flag to FALSE for now, and always return TRUE.
 * Called by MEDIA_FRAME_MACADDR, indexed by LINK_RSRB.
 */
static boolean rsrb_frame_macaddr (
    paktype *pak, 
    long address,
    addrtype *proto_addr, 
    boolean *broadcast)
{
    proto_addr->type = 0;
    proto_addr->length = 0;
    proto_addr->addr.fr_address = (ushort)address;
    *broadcast = FALSE;
    return(TRUE);
}


/*
 * srb_channel_macaddr
 *
 * if (linktype == LINK_IBMNM)
 *   return FALSE 
 * else (linktype == LINK_SRB)
 *   Set broadcast flag to FALSE (for now)
 *   return TRUE
 *
 * Called by MEDIA_FRAME_MACADDR
 *   indexed by LINK_SRB and LINK_IBMNM
 */
static boolean srb_channel_macaddr (
    paktype *pak, 
    long address,
    int *enctype,				    
    boolean *broadcast,
    staticmap_type **map)
{
    if (pak->linktype == LINK_IBMNM) {
      return(FALSE);
    }
    *broadcast = FALSE;
    return(TRUE);
}

/*
 * llc_channel_macaddr
 *
 * channel_vencap() will only be called 
 *   with linktype LINK_LLC2 by RSRB/LAK and CLSI
 * 
 * RSRB/LAK and CLSI may not pass the packet with TokenRing encap header
 *   add TokenRing encap header if it doesn't already exist
 *   
 * Reset linktype to LINK_SRB - indicates packet from SRB-to-CIP
 * Set enctype to ET_CHANNEL  - indicates packet encap'd for CIP
 * Set broadcast flag to FALSE 
 *
 * Called by MEDIA_FRAME_MACADDR, indexed by LINK_LLC2.
 *
 */
static boolean llc_channel_macaddr (
    paktype *pak, 
    long address,
    int *enctype,				    
    boolean *broadcast,
    staticmap_type **map)
{

    uchar   *daddr = (uchar *) address;

    if (!pak->llc2_cb) {
        /*
         * Return FALSE so packet is not processed
         * any further and will be freed back to 
         * buffer pool.
         */
        return FALSE;
    }
    
   /*
     * If from RP/RSP LLC2 stack, then must be
     * a rsrb_lack or cls pak.
     * Then set PAK_LACK_OUT in pak->flags so tr_vencap
     * uses the macaddr from the llc cb 
     * instead of the macaddr from the hwidb for the smac
     * in the packets MAC header.
     */
    pak->flags |= PAK_LACK_OUT;
    
    if (!ieee_equal_sans_rii(daddr, pak->datagramstart+TR_DA_OFFSET)) {
        if (!tr_vencap(pak, address)) {
            return(FALSE);
	}
    }
    
    pak->linktype = LINK_SRB;
    pak->enctype = *enctype = ET_CHANNEL;

    *broadcast = FALSE;
    return(TRUE);
}

/*
 * srb_media_registry
 *
 * Register media dependent functions for SRB
 */
void srb_media_registry (void)
{
    reg_add_media_ether_macaddr(LINK_RSRB, rsrb_ether_macaddr,
				"rsrb_ether_macaddr");
    reg_add_media_token_macaddr(LINK_RSRB, rsrb_token_macaddr,
				"rsrb_token_macaddr");
    reg_add_media_fddi_macaddr(LINK_RSRB, rsrb_token_macaddr,
			       "rsrb_token_macaddr");
    reg_add_media_frame_macaddr(LINK_RSRB, rsrb_frame_macaddr,
			       "rsrb_frame_macaddr");

    /*
     * Generic enough for DLSw use.
     */
    reg_add_media_ether_macaddr(LINK_DLSW, rsrb_ether_macaddr,
                                "rsrb_ether_macaddr");
    reg_add_media_token_macaddr(LINK_DLSW, rsrb_token_macaddr,
                                "rsrb_token_macaddr");
    reg_add_media_fddi_macaddr(LINK_DLSW, rsrb_token_macaddr,
                               "rsrb_token_macaddr");
    reg_add_media_frame_macaddr(LINK_DLSW, rsrb_frame_macaddr,
                               "rsrb_frame_macaddr");

    /*
     * channel vc mapping
     */
    reg_add_media_channel_macaddr(LINK_SRB, srb_channel_macaddr,
				  "srb_channel_macaddr");
    reg_add_media_channel_macaddr(LINK_IBMNM, srb_channel_macaddr,
				  "srb_channel_macaddr");
    reg_add_media_channel_macaddr(LINK_LLC2, llc_channel_macaddr,
				  "llc_channel_macaddr");
}
