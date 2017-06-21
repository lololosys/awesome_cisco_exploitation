/* $Id: if_vip_serial_inline.h,v 3.1.2.1 1996/05/22 23:28:25 rramacha Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_vip_serial_inline.h,v $
 *--------------------------------------------------------------------------
 * if_vip_serial_inline.h -- VIP specific Inline functions for Serial frame 
 *                                   classification
 *
 * May 1996, Ramakrishnan Ramachandran 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *--------------------------------------------------------------------------
 * $Log: if_vip_serial_inline.h,v $
 * Revision 3.1.2.1  1996/05/22  23:28:25  rramacha
 * CSCdi58249:  Enable IP DFS on 4T
 * Branch: California_branch
 *  o receive serial encap cmd
 *  o turn-on DFS on 4T
 *  o decode HDLC, PPP encapsulation on the received packet
 *
 * Revision 3.1  1996/05/22  22:22:15  rramacha
 * Place Holder
 *
 *--------------------------------------------------------------------------
 * $Endlog$
 */

/*
 * vip_serial_decode_encap_inline
 */
static inline ushort vip_serial_decode_encap_inline (hwidbtype *idb, 
						     paktype *pak)
{
    char *datagramstart;
    ushort     rxtype;
    
    /*
     * Classify the packet:
     */
    datagramstart = pak->datagramstart;
    
    switch(idb->enctype) {
      case ET_HDLC:
	rxtype = hdlc_decode_encapsulation_inline(pak,
						  (vhdlctype *)datagramstart);
	break;
	
      case ET_PPP:
	rxtype = ppp_decode_encapsulation_inline(pak,
						 (ppp_hdr *)datagramstart);
	break;

      default:
	rxtype = RXTYPE_UNKNOWN;
	break;
    }
    
    pak->mci_status = rxtype;
    return(rxtype);
}
