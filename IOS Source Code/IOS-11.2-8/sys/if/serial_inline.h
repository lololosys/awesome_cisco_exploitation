/* $Id: serial_inline.h,v 3.6.26.5 1996/08/21 19:17:49 fox Exp $
 * $Source: /release/112/cvs/Xsys/if/serial_inline.h,v $
 *------------------------------------------------------------------
 * serial_inline.h - Inline functions for Serial frame classification
 *
 * June 1993, Scott Mackie
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Gurus:
 *------------------------------------------------------------------
 * $Log: serial_inline.h,v $
 * Revision 3.6.26.5  1996/08/21  19:17:49  fox
 * CSCdi57429:  Router cannot distinguish between FCS bit set/not set in
 * PPP header
 * Branch: California_branch
 * Add support for RFC 1638 bridging packet header fields.  Bounce packets
 * to process level if any fields are set.
 *
 * Revision 3.6.26.4  1996/07/23  13:23:57  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.6.26.3  1996/07/20  01:26:47  rharagan
 * CSCdi60761:  SegV when crc 32 config in an SMDS interface in HIP card
 * Branch: California_branch
 * Init pak->network_start if swidb not found.
 *
 * Revision 3.6.26.2  1996/07/11  14:19:25  fbordona
 * CSCdi62521:  DLSw circuit does not connect using FST over ATM peer
 *              Remove DLSw FST hooks from platform/media specific drivers.
 * Branch: California_branch
 *
 * Revision 3.6.26.1  1996/06/11  04:54:40  mschaef
 * CSCdi59601:  Bus Error occurs when switching Ipx,Apple over HDLC
 * Branch: California_branch
 *
 * Revision 3.6  1996/01/19  22:15:11  wmay
 * CSCdi45344:  ACL: input access lists are ignored on SMDS subinterfaces
 * Sniff smds dest addr and switch to correct swidb.
 *
 * Revision 3.5  1995/12/13  04:20:42  ahh
 * CSCdi44230:  Remove dead code
 * Bring out your dead... *clang* (RSRB/TCP fast-switching)
 *
 * Revision 3.4  1995/11/20  19:34:59  turadek
 * CSCdi39149:  Some ethernet frames are not passed on HDLC when compress
 * stac used.  Bridging when using HDLC compression was not working
 * correctly.  Apply some fixes that were in original prototype but
 * didn't make it into the released version.
 * Just syncing this from 11.0.
 *
 * Revision 3.3  1995/11/17  09:31:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:39:05  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:52:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:03:28  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/06/22  22:49:53  wmay
 * CSCdi36246:  Appletalk doesnt fast-switch smds
 *
 * Revision 2.2  1995/06/19  04:07:19  speakman
 * CSCdi35697:  General TB and SRB regression failures
 * Make protocol-specific route/bridge test explicit rather than relying
 * on per-interface rxtypes indicators to provide backward compatible
 * SRB and TB behaviour.
 *
 * Revision 2.1  1995/06/07  20:54:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define SERIAL_ARPA_TYPE_CASES	\
      case TYPE_IP10MB:		\
	type = RXTYPE_DODIP;	\
	break;			\
      case TYPE_DECNET:		\
	type = RXTYPE_DECNET;	\
	break;			\
      case TYPE_XNS:		\
	type = RXTYPE_XNS;	\
	break;			\
      case TYPE_ETHERTALK:	\
	type = RXTYPE_APPLE;	\
	break;			\
      case TYPE_NOVELL1:	\
	type = RXTYPE_NOVELL1;	\
	break;			\
      case TYPE_NOVELL2:	\
	type = RXTYPE_NOVELL1;	\
	break;			\
      case TYPE_APOLLO:		\
	type = RXTYPE_APOLLO;	\
	break;			\
      case TYPE_VINES:		\
      case TYPE_VINES2:		\
	type = RXTYPE_VINES;	\
	break;			\
      case TYPE_CLNS:		\
	type = RXTYPE_CLNS;	\
	break;			\
      case TYPE_RSRB:		\
	type = RXTYPE_SRB;	\
	break;			\
      case TYPE_DLSW:		\
	type = RXTYPE_DLSW;	\
	break;

#define SERIAL_SNAP_TYPE_CASES		\
      case TYPE_IP10MB:			\
	type = RXTYPE_SNAP_DODIP;	\
	break;				\
      case TYPE_DECNET:			\
	type = RXTYPE_SNAP_DECNET;	\
	break;				\
      case TYPE_XNS:			\
	type = RXTYPE_SNAP_XNS;		\
	break;				\
      case TYPE_ETHERTALK:		\
	type = RXTYPE_SNAP_APPLE;	\
	break;				\
      case TYPE_NOVELL1:		\
	type = RXTYPE_SNAP_NOV1;	\
	break;				\
      case TYPE_NOVELL2:		\
	type = RXTYPE_SNAP_NOV1;	\
	break;				\
      case TYPE_APOLLO:			\
	type = RXTYPE_SNAP_APOLLO;	\
	break;				\
      case TYPE_VINES:			\
      case TYPE_VINES2:			\
	type = RXTYPE_SNAP_VINES;	\
	break;				\


/*
 * hdlc_decode_encapsulation_inline
 */
static inline ushort hdlc_decode_encapsulation_inline (paktype   *pak,
						       vhdlctype *hdlc)
{
  ushort type;
  
  pak->mac_start     = (uchar *)hdlc;
  pak->addr_start    = (uchar *)hdlc;
  pak->info_start    = (uchar *)NULL;
  pak->network_start = hdlc->data;
  pak->encsize       = HDLC_ENCAPBYTES;
  
  if ((hdlc->var_hdlcflags & HDLC_COMPRESSED) == HDLC_COMPRESSED) {
    type = RXTYPE_UNKNOWN;
  } else  {
    switch(hdlc->var_hdlctype) {
      SERIAL_ARPA_TYPE_CASES;
    case TYPE_BRIDGE:
      type = RXTYPE_TRANSIT;
      break;
    default:
      type = RXTYPE_UNKNOWN;
      break;
    }
  }
  
  /*
   * Fudgebyte revenge. We have to skip a packing byte between
   * the end of the type field and the start of the CLNS fixed
   * header. This is required for backwards compatibility.
   */
  if (type == RXTYPE_CLNS) {
    pak->network_start++;
    pak->encsize++;
  }
  
  pak->rxtype = type;
  return(type);
}

/*
 * ppp_decode_encapsulation_inline
 */
static inline ushort ppp_decode_encapsulation_inline (paktype *pak,
						      ppp_hdr *ppp)
{
    ushort type, bh;
    uchar *bh_ptr;

    pak->mac_start     = (uchar *)ppp;
    pak->addr_start    = (uchar *)ppp;
    pak->info_start    = (uchar *)NULL;
    pak->network_start = ppp->data;
    pak->encsize       = PPP_ENCAPBYTES;

    switch(ppp->plen) {
      case TYPE_PPP_IP:
	type = RXTYPE_DODIP;
	break;
	
      case TYPE_PPP_NOVELL1:
        type = RXTYPE_NOVELL1;
        break;
	
      case TYPE_PPP_LEX:
        type = RXTYPE_LEX;
        break;

      case TYPE_PPP_VINES:
        type = RXTYPE_VINES;
        break;

      case TYPE_PPP_ETHERTALK:
	type = RXTYPE_APPLE;
	break;

      case TYPE_PPP_BRIDGE:
	bh_ptr = ppp->data;
	PPP_GETSHORT(bh, bh_ptr);
	if (bh != PPP_FIZC_MACTYPE) {
	    type = RXTYPE_UNKNOWN;
	    break;
	}
	type = RXTYPE_TRANSIT;
	pak->encsize = PPP_BRIDGE_ENCAPBYTES;
	break;	

      default:
	type = RXTYPE_UNKNOWN;
	break;
    }

    pak->rxtype = type;
    return(type);
}


/*
 * smds_get_rxtype_inline
 */
static inline ushort smds_decode_encapsulation_inline (paktype *pak,
						       uchar *ptr)
{
    
    smdstype *smds;
    snap_hdr *snap;
    ushort type;
    idbtype *tempidb;

    smds = (smdstype *)(ptr + 
			pak->if_input->hwptr->smds_pdb->fast_input_modelen);
    pak->mac_start  = (uchar *)smds;
    pak->addr_start = smds->smdsdest;
    pak->info_start = smds->data;

    snap = (snap_hdr *)smds->data;

    switch(snap->dsap) {
      case SAP_SNAP:
	pak->network_start = snap->data;
	pak->encsize       = SMDS_SNAP_ENCAPBYTES;

	switch(GETSHORT(&snap->type)) {
	    SERIAL_ARPA_TYPE_CASES;

	case SMDS_BRIDGE_PID_8023FCS:
	case SMDS_BRIDGE_PID_8023:
	case SMDS_BRIDGE_PID_8025FCS:
	case SMDS_BRIDGE_PID_8025:
	case SMDS_BRIDGE_PID_FDDIFCS:
	case SMDS_BRIDGE_PID_FDDI:
	    if (ieee_equal_oui(smds_bridge_oui, snap->oui))
		type = RXTYPE_TRANSIT;
	    else
		type = RXTYPE_UNKNOWN;
	    break;

	  default:
	    type = RXTYPE_UNKNOWN;
	    break;
	}
	break;

      case SAP_CLNS:
	pak->network_start = snap->oui;
	pak->encsize       = SMDS_SAP_ENCAPBYTES;
	type = RXTYPE_CLNS;
	break;

      default:
	pak->network_start = snap->oui;
	pak->encsize       = SMDS_SAP_ENCAPBYTES;
	type = RXTYPE_UNKNOWN;
	break;
    }

    tempidb = reg_invoke_media_addr_to_swidb(ET_SMDS,
					     smds->smdsdest,
					     pak->if_input->hwptr);
    if (tempidb == NULL) {
	type = RXTYPE_UNKNOWN;
    } else
	pak->if_input = tempidb;

    pak->rxtype = type;
    return(type);
}

/*
 * atm_dxi_decode_encapsulation_inline
 * Now found in wan/atm_dxi_fast_les.h
 */

/*
 * serial_fix_datagramstart
 * Fix datagramstart for the serial drivers, which requires pak->datagramstart
 * to be short word aligned, and then shift the packet data to the new
 * position.
 */

/*
 * It may be necessary to allocate another buffer and copy the packet into the 
 * buffer as it is possible for there to be multiple references to the packet
 * from other protocols.
 */

static inline paktype *
serial_fix_datagramstart (paktype *pak, hwidbtype *idb)
{
    if (pak_shared(pak)) {
        paktype *newpak = getbuffer(pak->datagramsize);
	if (!newpak) {
	    return(NULL);
	}

	newpak->enctype = pak->enctype;
	newpak->datagramsize = pak->datagramsize;
	
	bcopy(pak->datagramstart, newpak->datagramstart, pak->datagramsize);
	
	/* Do some fixups required by bridging, custom/fair/priority queueing */
	newpak->if_output = pak->if_output;
	newpak->if_input = pak->if_input;
	newpak->linktype = pak->linktype;
	newpak->flags = pak->flags;
	newpak->traffic_shape = pak->traffic_shape;
	newpak->network_start =
	  newpak->datagramstart + (pak->network_start - pak->datagramstart);
	newpak->mac_start =
	  newpak->datagramstart + (pak->mac_start - pak->datagramstart);
	
	datagram_done(pak);
	return(newpak);
    } else {
        uchar *old_start = pak->datagramstart;
	pak->datagramstart--;
	pak->network_start--;
	pak->mac_start--;
	bcopy(old_start, pak->datagramstart, pak->datagramsize);
	return(pak);
    }
}


/*
 * Initial fast switch routine that sets up the proper subinterface
 * pointer, and looks for PVC switching.  This routine is called for all
 * rxtypes prior to calling a network layer protocol specific routine.
 */
static inline
boolean atm_dxi_classify_pak_and_fs_les (paktype *pak)
{
    ushort dfa;
    dxi_pvc_info_type *dxi_pvc;
    ushort type;
    uchar *start;
    ushort bytes;

    start = pak->datagramstart;
    pak->mac_start  = start;
    pak->addr_start = start;

    dfa = DFA_TO_NUM(GETSHORT(start));

    dxi_pvc = pak->if_input->hwptr->atm_dxi_stuff->dxi_dfa_tab[dfa];
    if (dxi_pvc == NULL) {
	pak->rxtype = RXTYPE_UNKNOWN;
	return(FALSE);
    }

    bytes = pak->datagramsize;
    /* Now we need to get the atm_dxi structure for this dfa */
    /* That gives us the encap type */
    switch (dxi_pvc->encaps_type) {
    case DXI_MAP_SNAP:
	{
	    atm_dxi_llc_hdr *header = (atm_dxi_llc_hdr *)start;
	    pak->info_start = header->llc_start;
	    if (GETSHORT(&header->llc) == DXI_ISO_LLC) {
		/* Have an iso pdu */
		pak->network_start = header->iso_pdu_start;
		type = RXTYPE_CLNS;
		pak->encsize = ATM_DXI_ISOPDU_HDRBYTES;
	    } else {
		/* Might want to check for bridged packets */
		pak->network_start = (uchar *)header->snap_data_start;
		switch (GETSHORT(&header->etype)) {
		    SERIAL_SNAP_TYPE_CASES;
		default:
		    type = RXTYPE_SNAP_UNK;
		    break;
		}
		pak->encsize = ATM_DXI_SNAPENCAPBYTES;
	    }
	}
	break;
    case DXI_MAP_NLPID:
	{
	    atm_dxi_hdr *dxi_hdr = (atm_dxi_hdr *)start;

	    pak->info_start    = (char *)NULL;
	    switch (GETSHORT(&dxi_hdr->type)) {
	    case ATM_DXI_IETF_IP:
		pak->network_start = dxi_hdr->data;
		pak->encsize = ATM_DXI_ENCAPBYTES;
		type = RXTYPE_DODIP;
		break;
	    case ATM_DXI_CLNP:
	    case ATM_DXI_ESIS:
	    case ATM_DXI_ISIS:
	    default:
		type = RXTYPE_UNKNOWN;
		pak->encsize = ATM_DXI_ENCAPBYTES;
		break;
	    case ATM_DXI_SNAP0PAD:
		{
		    /* Might have to be - 1 here */
		    snap_hdr *snap = (snap_hdr *)(dxi_hdr->snap_start - 1);

		    pak->info_start = (uchar *)snap;
		    pak->network_start = snap->data;
		    switch (GETSHORT(&snap->type)) {
			SERIAL_SNAP_TYPE_CASES;
		    default:
			type = RXTYPE_SNAP_UNK;
			break;
		    }
		    pak->encsize = pak->network_start - pak->datagramstart;
		}
		break;
	    case ATM_DXI_SNAP1PAD:
		{
		    snap_hdr *snap  = (snap_hdr *)dxi_hdr->snap_start;
		    pak->info_start = (uchar *)snap;
		    pak->network_start = snap->data;
		    switch (GETSHORT(&snap->type)) {
			SERIAL_SNAP_TYPE_CASES;
		    default:
			type = RXTYPE_SNAP_UNK;
			break;
		    }
		    pak->encsize = pak->network_start - pak->datagramstart;
		}
		break;

	    }
	}
	break;
    case DXI_MAP_MUX:
	pak->info_start = NULL;
	pak->network_start = &start[2];
	pak->encsize = ATM_DXI_MUXENCAPBYTES;
	type = dxi_pvc->rxtype;
	/* Get the type from the atm_dxi info */
	break;
    default:
	type = RXTYPE_UNKNOWN;
	pak->encsize = ATM_DXI_ENCAPBYTES;
	break;
    }
    pak->rxtype = type;

    /*
     * Protect against fast switching killer packet fragments
     * smaller than the encapsulation size
     */
    if (bytes > pak->encsize) {
	if (reg_invoke_atm_dxi_fs(type, pak)) {
	    INC_DXI_PVC_FAST_INPUT(dxi_pvc, bytes);
	    return(TRUE);
	}
    }
    return(FALSE);

}


/*
 * serial_RX_interrupt_inline()
 * This routine handles the genric stuff done during a RX interrupt
 */
static inline void serial_RX_interrupt_inline (hwidbtype *idb, paktype *pak)
{
    char *datagramstart;
    ushort     rxtype;
    
    /*
     * First, classify the packet:
     */
    datagramstart = pak->datagramstart;
    
    switch(idb->enctype) {
      case ET_HDLC:
	rxtype = hdlc_decode_encapsulation_inline(pak,
						  (vhdlctype *)datagramstart);
	if (reg_invoke_hdlc_fs(rxtype, pak)) {
	    return;
	}
	break;
	
      case ET_FRAME_RELAY:
	if (reg_invoke_fr_pvc_switch_or_fs(pak)) {
	    return;
	}
	rxtype = pak->rxtype;	
	break;
	
      case ET_SMDS:
	rxtype = smds_decode_encapsulation_inline(pak,
						  datagramstart);
	if (reg_invoke_smds_fs(rxtype, pak)) {
	    return;
	}
	break;
	
      case ET_PPP:
	rxtype = ppp_decode_encapsulation_inline(pak,
						 (ppp_hdr *)datagramstart);
	if (reg_invoke_ppp_fs(rxtype, pak)) {
	    return;
	}
	break;

      case ET_ATM_DXI:
	if (atm_dxi_classify_pak_and_fs_les(pak)) {
	    return;
	}
	rxtype = pak->rxtype;
	break;	

      default:
	rxtype = RXTYPE_UNKNOWN;
	pak->encsize = idb->encsize;
	break;
    }
    
    /*
     * Handle receive frame special cases
     */
    switch(rxtype) {
	/*
	 * Consider a frame for bridging
	 */
      case RXTYPE_TRANSIT:
	if (idb->tbridge_on_hwidb) {
	    /*
	     * Mark as bridge encapsulation
	     */
	    pak->enctype = ET_BRIDGE;

	    if (reg_invoke_tbridge_forward(pak)) {
		return;
	    }
	}
	break;
	
	/*
	 * Consider a frame for RSRB
	 */
      case RXTYPE_SRB:
	if (reg_invoke_nim_vring_input(idb, pak)) {
	    return;
	}
	break;

	/*
	 * Consider a frame for DLSw
	 */
      case RXTYPE_DLSW:
	if (reg_invoke_nim_dlsw_input(idb, pak)) {
	    return;
	}
	break;

      case RXTYPE_LEX:
	{ 
	    idbtype *lex_swidb = idb->firstsw->lex_swidb;
	    
	    /*
	     * Lex not bound to serial.
	     */
	    if (lex_swidb == NULL) {
		datagram_done(pak);
		return;
	    }
	    
	    /*
	     * Only bridge protocols that we are not routing.
	     */
	    if (BRIDGING_RXTYPE(idb->firstsw, pak->rxtype)) {
		/*
		 * Is there a bridge group on the lex interface.
		 */
		if (lex_swidb->tbridge_on_swidb) {
		    /*
		     * Mark as bridge encapsulation
		     */
		    pak->enctype = ET_BRIDGE;
		    /*
		     * Change the input interface so that it points to
		     * the lex interface.
		     */
		    change_if_input(pak, lex_swidb);
		    
		    if (reg_invoke_tbridge_forward(pak)) { 
			return;
		    }
		    change_if_input(pak, idb->firstsw);
		}
	    }
	}
	break;
	
	/*
	 * Consider a frame for FST
	 */
      case RXTYPE_DODIP:
	if (ROUTING_IF(idb->firstsw, LINK_IP)) {
	    iphdrtype *ip = (iphdrtype *)ipheadstart(pak);
	    if (ip->prot == IPINIP_PROT || ip->prot == GRE_PROT) {
		if (reg_invoke_tunnel_ip_les_fs(pak))
		    return;
	    }
	}
	
      default:
	break;
    }
    
    /*
     * We did not fast switch the serial datagram.
     * Queue packet for process level:
     */
    serial_queue_for_process(idb, pak);
}
