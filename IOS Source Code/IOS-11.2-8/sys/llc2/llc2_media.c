/* $Id: llc2_media.c,v 3.5.10.4 1996/07/31 06:48:39 gcox Exp $
 * $Source: /release/112/cvs/Xsys/llc2/llc2_media.c,v $
 *------------------------------------------------------------------
 * llc2_media.c -- media dependent LLC2 routines.
 *
 * 13-April-1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: llc2_media.c,v $
 * Revision 3.5.10.4  1996/07/31  06:48:39  gcox
 * CSCdi64685:  Convert LLC2 to interface_generic.h (except llc2.c)
 * Branch: California_branch
 *
 * Revision 3.5.10.3  1996/05/17  11:27:12  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.2.3  1996/05/05  23:32:48  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.5.2.2  1996/04/19  19:45:06  snix
 * CSCdi52176:  alignment correction message
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.2.1  1996/04/03  20:03:57  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.10.2  1996/04/25  23:15:49  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.5.10.1.2.1  1996/03/24  04:04:25  rzagst
 * ATM modularity: Remove AIP from all files that are not referring to the
 * AIP i/f
 * Branch: ATM_Cal_branch
 *
 * Revision 3.5.10.1  1996/03/18  21:11:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.2  1996/03/16  07:13:24  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.2.1  1996/03/07  10:06:24  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5  1996/02/26  06:53:04  dmccowan
 * CSCdi45288:  need appn configuration changes to support multilink ppp
 * Add configuration option for APPN over a dialer interface
 *
 * Revision 3.4  1996/01/22  03:55:45  dmccowan
 * CSCdi45007:  LLC2 LCP negotiated over PPP when not wanted
 *
 * Revision 3.3  1995/11/17  17:44:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:01:31  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:35:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:15:38  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/13  03:28:27  alam
 * CSCdi35570:  Remove extraneous functions
 * Use llc_process_test and llc_process_xid for FDDI as well as ethernet
 * and FR.
 * Remove FDDI packet debug.
 *
 * Revision 2.1  1995/06/07  21:42:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "media_registry.h"
#include "interface_generic.h"
#include "packet.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/fddi.h"
#include "../h/address.h"
#include "../llc2/llc2.h"

static const uchar ibmnm_llc[SAP_HDRBYTES] = {SAP_IBMNM, SAP_IBMNM, LLC1_UI};

/*
 * ether_et_llc2
 *
 * Write the MAC header for an LLC2 datagram on an Ethernet
 * Called by MEDIA_ETHER_MACWRITE, index is ET_LLC2
 */
static boolean ether_et_llc2 (paktype *pak, hwidbtype *hwidb, int linktype,
			      char *daddr, char const *other_info)
{
    ether_hdr *ether;

    /* 
     * The llc bytes have already been filled in.
     * pak->datagramstart points to the beginning of
     * the llc data.
     */
    pak->datagramstart -= ETHER_ARPA_ENCAPBYTES;
    pak->datagramsize += ETHER_ARPA_ENCAPBYTES;
    ether = (ether_hdr *)pak->datagramstart;
    ieee_copy(daddr, ether->daddr);
    ieee_copy(hwidb->hardware, ether->saddr);
    PUTSHORT( &ether->type_or_len, (pak->datagramsize - ETHER_ARPA_ENCAPBYTES));
    return(TRUE);
}

/*
 * llc2_ether_macaddr
 *
 * Turn an LLC2 address into an encapsulation type and a MAC address
 * Called by MEDIA_ETHER_MACADDR, indexed by LINK_LLC2 and LINK_IBMNM
 */
static boolean llc2_ether_macaddr (paktype *pak, long address, int *enctype,
				   uchar **daddr, uchar const **other_info)
{
    if (pak->linktype == LINK_LLC2 || pak->linktype == LINK_CLS)
	*enctype = pak->llc2_enctype;
    else
	*enctype = ET_SAP;
    *daddr = (uchar *) address;
    return(TRUE);
}

/*
 * token_et_llc2
 *
 * Write the MAC header for an LLC2 datagram on an Token Ring net
 * Called by MEDIA_TOKEN_MACWRITE, index is ET_LLC2
 */
static boolean token_et_llc2 (paktype *pak, int linktype, char *daddr,
			      char *saddr, char const *other_info,
			      riftype *rifptr)
{
    tring_hdr *trh;
    int riflen;

    /* 
     * The llc bytes have already been filled in.
     * pak->datagramstart points to the beginning of
     * the llc data.
     */
    riflen = rifptr ? rifptr->length : 0;
    pak->datagramstart -= (TRING_ENCAPBYTES + riflen);
    pak->datagramsize += (TRING_ENCAPBYTES + riflen);

    trh = (tring_hdr *)pak->datagramstart;
    trh->ac = AC_PRI0;		/* could set priority here */
    trh->fc = FC_TYPE_LLC;
    ieee_copy(daddr, trh->daddr);
    ieee_copy(saddr, trh->saddr);
    if (riflen > 0) {
	trh->saddr[0] |= TR_RII;	/* indicate we are here */
	bcopy(rifptr->rif, trh->data, riflen);
    }
    return(TRUE);
}

/*
 * llc2_token_macaddr
 *
 * Turn an LLC2 address into an encapsulation type and a MAC address
 * Called by MEDIA_TOKEN_MACADDR and MEDIA_FDDI_MACADDR, 
 * indexed by LINK_LLC2
 */
static boolean llc2_token_macaddr (paktype *pak, long address, int *enctype,
				   uchar **daddr, uchar const **other_info,
				   ulong *rif_flags)
{
    *enctype = pak->llc2_enctype;
    *daddr = (uchar *)address;

    if (*(ushort *)((uchar *)address + RIF_DTE) == 0) {
	/* legacy case... */
	*rif_flags = (RIF_PRESENT | RIF_REQUIRED);
    } else {
	*rif_flags = RIF_IN_LLC_DTE; /* whiz-bang cls case */
    }

    return(TRUE);
}

/*
 * ibmnm_token_macaddr
 *
 * Turn an IBMNM address into an encapsulation type and a MAC address
 * Called by MEDIA_TOKEN_MACADDR and MEDIA_FDDI_MACADDR, indexed by LINK_IBMNM
 */
static boolean ibmnm_token_macaddr (paktype *pak, long address, int *enctype,
				    uchar **daddr, uchar const **other_info,
				    ulong *rif_flags)
{
    if (pak->linktype == LINK_LLC2 || pak->linktype == LINK_CLS ||
        pak->linktype == LINK_IBMNM)
	*enctype = pak->llc2_enctype;
    else
	*enctype = ET_SAP;
    *daddr = (uchar *)address;
    *other_info = ibmnm_llc;
    *rif_flags = (RIF_PRESENT | RIF_REQUIRED);
    return(TRUE);
}

/*
 * fddi_et_llc2
 *
 * Write the MAC header for an LLC2 datagram on an FDDI
 * Called by MEDIA_FDDI_MACWRITE, index is ET_LLC2
 */
static boolean fddi_et_llc2 (paktype *pak, hwidbtype *hwidb, int linktype,
			     char *daddr, char *saddr, char const
			     *other_info, riftype*rifptr)
{
    fddi_hdr *fddi;
    int riflen;

   /* 
     * The llc bytes have already been filled in.
     * pak->datagramstart points to the beginning of
     * the llc data.  Add room for FC and 2 MAC addresses.
     */
    
    riflen = rifptr ? rifptr->length : 0;
    pak->datagramsize += (FDDI_ENCAPBYTES + riflen);
    pak->datagramstart -= (FDDI_ENCAPBYTES + riflen);

    fddi = (fddi_hdr *)pak->datagramstart;
    ieee_copy(daddr, fddi->daddr);
    ieee_copy(saddr, fddi->saddr);
    if (riflen  > 0) {
	if (hwidb->type == IDBTYPE_FDDIT)
	    fddi->saddr[0] |= FDDIT_RII;	/* indicate we are
						   here */
	else
	    fddi->saddr[0] |= FDDI_RII;
	bcopy(rifptr->rif, fddi->data, riflen);
    }
    return(TRUE);
}


/*
 * llc2_frame_macaddr
 *
 * Set braodcast flag to FALSE for now, always return TRUE.
 * Called by MEDIA_FRAME_MACADDR, indexed by LINK_LLC2.
 */
static boolean llc2_frame_macaddr (paktype *pak, long address,
			addrtype *proto_addr, boolean *broadcast)
{
    llctype *llc;

	proto_addr->type = 0;
	proto_addr->length = 0;
	proto_addr->addr.fr_address = *(ushort *)address;
        if (pak->llc2_enctype == ET_BRIDGE) { 
            llc = (llctype *)address;
            proto_addr->addr.fr_address = llc->fDlci;
        }
	*broadcast = FALSE;
	return(TRUE);
}

/*
 * llc2_smds_macaddr
 *
 * Turn an LLC2 address into an encapsulation type and a MAC/SAP address
 * Called by MEDIA_SMDS_MACADDR, indexed by LINK_LLC2.
 */
static boolean llc2_smds_macaddr (paktype *pak, long address,
                                  int *enctype, uchar const **daddr,
                                  uchar const **other_info, 
                                  addrtype *proto_addr)
{
    *daddr = (uchar *)address;
    *enctype = ET_NULL;

    return(TRUE);
}

/*
 * llc2_atm_macaddr
 *
 * Set braodcast flag to FALSE for now, always return TRUE.
 * Called by MEDIA_ATM_MACADDR, indexed by LINK_LLC2.
 */
static boolean llc2_atm_macaddr (paktype *pak, 
				 long address,
				 hwaddrtype *daddr,
				 uchar const **snap_info,
				 staticmap_type **map,
				 boolean *broadcast)

{
    uchar* temp_dte = (uchar*) address;

    snpa_init(daddr, 
	      STATION_ATMVC, 
	      TARGET_UNICAST, 
	      &temp_dte[ATM_VCD_DTE]);
    *snap_info = NULL;
    *map = NULL;
    *broadcast = FALSE;
    return(TRUE);
}

/*
 * llc2_ddr_macaddr
 * Set braodcast flag to FALSE for now, always return TRUE.
 * Called by MEDIA_DDR_MACADDR, indexed by LINK_LLC2.
 *
 */
static boolean llc2_ddr_macaddr (paktype *pak, long address,
				 addrtype *proto_addr, boolean *broadcast)
{
    *broadcast = FALSE;
    proto_addr->type = 0;
    proto_addr->length = 0;
    pak->dialer_save_type = LINK_LLC2;
    return(TRUE);
}


/*
 * llc2_media_registry
 *
 * Register media dependent functions for LLC2
 */
void llc2_media_registry (void)
{
    reg_add_media_ether_macwrite(ET_LLC2, ether_et_llc2, "ether_et_llc2");
    reg_add_media_ether_macaddr(LINK_LLC2, llc2_ether_macaddr,
				"llc2_ether_macaddr");


    reg_add_media_token_macwrite(ET_LLC2, token_et_llc2, "token_et_llc2");

    reg_add_media_token_macaddr(LINK_LLC2, llc2_token_macaddr,
				"llc2_token_macaddr");
    reg_add_media_token_macaddr(LINK_IBMNM, ibmnm_token_macaddr,
				"ibmnm_token_macaddr");
    reg_add_media_token_macaddr(LINK_CLS, llc2_token_macaddr,
				"llc2_token_macaddr (cls)");


    reg_add_media_fddi_macaddr(LINK_IBMNM, ibmnm_token_macaddr,
				"ibmnm_token_macaddr");
    reg_add_media_fddi_macwrite(ET_LLC2, fddi_et_llc2, "fddi_et_llc2");
    reg_add_media_fddi_macaddr(LINK_LLC2, llc2_token_macaddr,
			       "llc2_macaddr_fddi");

    reg_add_media_frame_macaddr(LINK_LLC2, llc2_frame_macaddr,
				"llc2_frame_macaddr");
    reg_add_media_atm_macaddr(LINK_LLC2, llc2_atm_macaddr,
			      "llc2_atm_macaddr");

    reg_add_media_smds_macaddr(LINK_LLC2, llc2_smds_macaddr,
			       "llc2_smds_macaddr");
    reg_add_media_ddr_macaddr(LINK_LLC2, llc2_ddr_macaddr,
			       "llc2_ddr_macaddr");

}
