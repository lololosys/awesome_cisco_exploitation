/*-------------------------------------------------------------------------
 *
 * $Id: tbridge_sde_inline.h,v 3.2.6.1 1996/09/10 02:38:30 hou Exp $
 *
 * tbridge/tbridge_sde_inline.h
 *
 * September 1994, Martin McNealis.
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 *
 * All rights reserved.
 * 
 * Inline functions for Transparently Bridged Virtual LANs. 
 *
 * (a.k.a. IEEE 802.10 Secure Data Exchange (SDE)).
 *
 *-------------------------------------------------------------------------
 *
 * 
$Endlog$
 *
 */


/*
 * tbridge_parse_sde_header_inline
 *
 * Assume caller has verified that this is a SDE packet by examining
 * lsap.
 *
 * Two cases to return FALSE:
 * (1) Couldn't find any said(color) configured on sub-swidb(s) associated 
 * with this hwidb matches the said(color) of this packet AND we are NOT 
 * bridging on major swidb:
 *
 * 		Return FALSE to let caller drop this packet.
 *
 * (2) Find a matched said(color) but something is wrong(e.g. null swidb)
 *
 * 		Return FALSE to let caller drop this packet.
 *
 *
 * Two cases to return TRUE:
 * (1) Couldn't find any said(color) configured on sub-swidb(s) associated 
 * with this hwidb matches the said(color) of this packet AND we are bridging
 * on major swidb - we simply bridge it WITHOUT knowing the existence of
 * SDE header(treat SDE header as native data):
 *
 * 		Return TRUE without adjusting pak->info_start to bridge it.
 *
 * (2) Find a matched said(color);, demultiplex this packet to that 
 * sub-swidb by overwriting the pak->if_input; adjust pak->info_start to
 * point to the native LLC.
 *
 * 		Return TRUE with adjusting pak->info_start to bridge it.
 *
 */
static inline
enum SDE_RETVAL tbridge_parse_sde_header_inline (paktype *pak)

{
     idbtype *inputsw;
     sdehdrtype *sde_hdr;
     ushort said_hash;
     said_descriptor *said_ptr;
     snap_hdr *snap;
     int linktype ;

     /*
      * We should already have SDE header in pak buffer.
      */
     sde_hdr = (sdehdrtype *)(pak->info_start);

     /* Use GETLONG to fix misaligned access. */
     said_hash = nethash(GETLONG(sde_hdr->sde_said));

     said_ptr = said_table[said_hash];

     /*
      * Find out if the entry matches the SAID.
      *
      * Should do SDE packet accounting here.
      */

     while (said_ptr) {
	 if (said_ptr->sdb_said == GETLONG(sde_hdr->sde_said))
	     break;
	 said_ptr = said_ptr->next_said_db;
     }

     /*
      * Couldn't find any configured said(color) associated with this
      * hwidb matching the said(color) on the packet:
      *	If bridging is configured on major swidb, bridge it.
      *	Otherwise, drop it.
      *
      * Assumption is that from parser we destroy the said_db struct
      * of a said when we deconfigure this specific said or when we remove
      * subinterface(s) which has this said configured. So each time we 
      * get a valid said_ptr, we assume that some subinterface(s) has this 
      * said configured.
      */
     if (!said_ptr) {
	 if (pak->if_input->tbridge_on_swidb)
	     return (SDE_MAINIF_BRIDGE);
	 else
	     return (SDE_DISCARD);
     }

     inputsw = said_ptr->sdb_sub_interface[pak->if_input->hwptr->hw_if_index];

     /*
      * We have a valid said_descriptor but returned could find no 
      * swidb to bridge it on, so drop it.
      */
     if (!inputsw) 
	 return (SDE_DISCARD);

     /* 
      * Make sure not to black hole traffic destined for subinterfaces
      * in the deleted state.  This check must come before the following
      * check for bridging, as deleted sub_interfaces are automatically
      * deconfigured for bridging
      */
     if (inputsw->subif_state  == SUBIF_STATE_DELETED)
	 return(SDE_MAINIF_BRIDGE);

     /*
      * Bridging is not configured on the sub i/f
      * then routing must be configured on it
      * because we find it in the sde linklist. Only
      * the routing broadcast pak comes here, whose 
      * station bit isn't set, but the briding bit is
      * set due to bridging configured on another 
      * sub i/f of the same physical i/f.
      */

     /*
      * Check if the protocol of the input packet is configured for routing.
      * The reason we are doing this check is that the high end FDDI
      * broadcast will call the bridging first.  And the packet have to
      * classify its internal link type to decide whether we can
      * route or not.  We don't want to bridge the packet if it is a
      * routing packet.
      */
     snap = (snap_hdr *)(pak->info_start + SDE_ENCAPBYTES);
     switch (snap->dsap) {
     case SAP_SNAP:
       linktype = type2link(GETSHORT(&snap->type));
       break;
	    
     default:
	    linktype = sap2link(snap->dsap);
	    break;
     }
     /* if this interface can be routing */
     if (ROUTING_IF(inputsw, linktype))
       return(SDE_SUBIF_ROUTE);

     /*
      * For now, 802.10 SDE encap is bridging specific. For safety,
      * check bridging is configured on that swidb we found.
      * If bridging is not configured on that swidb, should drop it
      */

     /*
      * Overwrite pak->if_input.
      */
     pak->if_input = inputsw;

     /*
      * Align pak->info_start to point to native LLC.
      */
     pak->info_start += SDE_ENCAPBYTES;

     /*
      * Don't need to move the "type" to the area after MAC addresses
      * since we always translate ARPA to SDE-SNAP which has
      * valid pak->info_start to point to the snap header and thus
      * we could get the "type" of snap header in tbridge_type_sap_filter().
      */

     /*
      * Let caller continue processing this packet.
      */
     return (SDE_SUBIF_BRIDGE);
}


/*
 * Write an 802.10 SDE header to an network buffer.
 *
 */
static inline
void tbridge_bfr_sde_header_wr_inline (idbtype *outputsw, uchar *bfr_wr_ptr,
				       uchar *src_addr)
{
    ulong said;

    if (!outputsw->sde_said_db)
        return;

    said = outputsw->sde_said_db->sdb_said;

    /*
     * Write SDE header.
     */
    PUTSHORT(bfr_wr_ptr, (SAP_SDE | SAP_SDE << 8));
    bfr_wr_ptr += 2;
    PUTSHORT(bfr_wr_ptr, (LLC1_UI << 8 | said >> 24));
    bfr_wr_ptr += 2;
    PUTSHORT(bfr_wr_ptr, (ushort)(said >> 8));
    bfr_wr_ptr += 2;
    PUTSHORT(bfr_wr_ptr, (((uchar)(said & 0xFF)) << 8 | *src_addr));
    bfr_wr_ptr += 2;
    PUTLONG(bfr_wr_ptr, GETLONG(&src_addr[1]));
    bfr_wr_ptr += 4;

    /* Flag is zero for now - no fragmentation support */
    PUTLONG(bfr_wr_ptr, (*((uchar *)(&src_addr[5]))) << 24);
}

/*
 * sde_extract_header_inline
 *
 * Remove sde encapsulation from a sde packet to suit
 * caller's needs.
 */

static inline void sde_extract_header_inline (paktype *pak)
{
    ushort copy_len;
    uchar *datagram_start;


    /*
     * Move the MAC portion down by SDE_ENCAPBYTES bytes to 'extract'
     * the SDE header.
     * pak->info_start is already adjusted by SDE-header parsing
     * routine to point to native LLC.
     * Adjust pak->datagramstart, pak->addr_start.
     */
     datagram_start = pak->datagramstart;
     pak->datagramstart += SDE_ENCAPBYTES;
     pak->addr_start += SDE_ENCAPBYTES;
     pak->datagramsize -= SDE_ENCAPBYTES;
     copy_len = pak->info_start - (datagram_start + SDE_ENCAPBYTES);
     if (copy_len <= SDE_ENCAPBYTES)
	 bcopy(datagram_start, pak->datagramstart, copy_len);
     else {
	 /*
	  * If all fields are alligned, should use PUTLONG or PUTSHORT to speed
	  * up, sigh! 
	  */
	 uchar *src, *dst;
	 src = pak->info_start - SDE_ENCAPBYTES;
	 dst = pak->info_start;
	 for ( ; copy_len > 0; copy_len--)
	     *(--dst) =  *(--src);
     }

     /*
      * Adjust the 'length' field of ether-like packet.
      */
      if ((TBR_ETHER_SDE == pak->if_input->tbridge_media) ||
          (TBR_HDLC_TRANSIT_SDE == pak->if_input->tbridge_media)) {
          ether_hdr *ether = (ether_hdr *)pak->addr_start;
          (ether->type_or_len) -= SDE_ENCAPBYTES;
      }

}

