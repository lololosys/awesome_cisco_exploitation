/* $Id: tring_inline.h,v 3.5.12.1 1996/05/04 00:46:51 wilber Exp $
 * $Source: /release/112/cvs/Xsys/if/tring_inline.h,v $
 *------------------------------------------------------------------
 * tring_inline.h - Inline functions for Token Ring frame classification
 *
 * June 1993, Scott Mackie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: tring_inline.h,v $
 * Revision 3.5.12.1  1996/05/04  00:46:51  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Restore the input to the Bridge-group Virtual Interface if necessary
 * - Support user settable mtu for the BVI.
 * - Introduce global_irb_enable, SET_BRIDGING_RXTYPE and "show idb irb"
 *
 * Revision 3.5  1996/02/13  05:50:45  ogrady
 * CSCdi45887:  Router crash in rsp_fastsend
 *              Use common routine for token ring output debug.
 *
 * Revision 3.4  1996/01/18  23:01:05  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/11/30  02:59:20  syiu
 * CSCdi44150:  Remote Node NetBEUI on Token Ring platform not working
 *
 * Revision 3.2  1995/11/17  09:31:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:53:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:03:38  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/09/14  22:35:33  kmoberg
 * CSCdi40399:  token_decode_encapsulation_inline() still broken. Get
 * the right rif length.
 *
 * Revision 2.3  1995/07/11  00:51:50  ogrady
 * CSCdi34908:  Process level R/SRB hangs interface (orig AT over SRB
 * fails).  Don't make fastout vector return_false to disable srb fast
 * switching since its called from srb_common() path.  Set AC byte to
 * AC_PRI4 for all srb output paths.  Don't count ignored MAC packets as
 * drops.  Ongoing SRB counter clean up.
 *
 * Revision 2.2  1995/06/14  04:14:36  alam
 * CSCdi35813:  Enable CLS/LLC2 to use RIF over FDDI
 *
 * Revision 2.1  1995/06/07  20:55:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * token_decode_encapsulation_inline
 */
static inline void token_decode_encapsulation_inline (paktype *pak,
						      ushort rxtype,
						      tring_hdr *token)
{
    snap_hdr	*snap;
    ushort	dsap_ssap;

    pak->rxtype      = rxtype;
    pak->mac_start   = &token->ac;
    pak->addr_start  = token->daddr;
 
    pak->riflen = getRifLenPak((srbrif_t *)token->daddr, TR_RII);
    pak->info_start  = &token->data[pak->riflen];
    snap = (snap_hdr *)pak->info_start;
    dsap_ssap = GETSHORT(&snap->dsap);
     
    /*
     * Don't forget the RIF. CSCdi27967
     */
    if (dsap_ssap == SNAPSNAP) {
        pak->network_start = snap->data;
        pak->encsize       = TRING_SNAP_ENCAPBYTES + pak->riflen;
    } else if (dsap_ssap == SAPSAP_VINES) {
        pak->network_start = &snap->oui[1];
        pak->encsize       = TRING_SAP_ENCAPBYTES + pak->riflen + 1;
    } else {
        pak->network_start = snap->oui;
        pak->encsize       = TRING_SAP_ENCAPBYTES + pak->riflen;
    }
}

/*
 * token_get_enctype_infosize
 *
 * Given an LLC header, determine the encapsulation type and length of
 * the LLC or LLC/SNAP header.
 */
static inline void token_get_enctype_infosize (
    sap_hdr *sap,
    int *enctype,
    int *info_size, 
    idbtype *idb)
{
    ushort	dsap_ssap;

    dsap_ssap = GETSHORT(&sap->dsap);
    if (dsap_ssap == SNAPSNAP) {
	*enctype = ET_SNAP;
	*info_size = SNAP_HDRBYTES;
	return;
    }
    
    /*
     * CSCdi16607
     * Novell Kludge: We want to bridge novell-ether over token.
     *   This is not LLC type 1 - so we don't assign it as ET_SAP
     *   This is not LLC type 2, either. We will drop it later if
     *   we assign it as ET_LLC2.
     *
     * NetBIOS : The packet got here because we don't have a
     *   NetBIOS stack. This is a LLC type 2 packet but we don't
     *   want to pass it to LLC2 and drop it - we want to bridge it.
     *
     * ET_NOVELL_ETHER and ET_UNSUPPORTED are not supported -
     *   Those two kinds of packets will be classified as
     *   LINK_ILLEGAL as linktype, and will be bridged.
     *
     * Check dsap AS WELL AS ssap here, and this is what we should do.
     *   dsap is not necessarily equal to ssap.
     */
    if (dsap_ssap == SAPSAP_NOVELL_ETHER) {
        *enctype = ET_NOVELL_ETHER;
        sap->control = '\0';
        *info_size = SAP_HDRBYTES - 1;
        return;
    }

    if ( (dsap_ssap == SAPSAP_NETBIOS) &&
	 !(idb && RUNNING_IF(LINK_NBF, idb)) ) {
        *enctype = ET_UNSUPPORTED;
        *info_size = SAP_HDRBYTES;
        return;
    }

    switch (sap->dsap) {
      case SAP_VINES:
	*enctype = ET_VINES_TR;
	*info_size = SAP_HDRBYTES + 1;
	return;
	
      case SAP_CLNS:
      case SAP_NOVELL:
      case SAP_NULL:        /* Some terminal servers use this - CSCdi20448 */
	*enctype = ET_SAP;
	*info_size = SAP_HDRBYTES;
	return;

      case SAP_SDE:
        *enctype = ET_SDE;
        *info_size = SDE_ENCAPBYTES;
        break;

      case SAP_NETBIOS:
      case SAP_IBMNM:
      case SAP_CONS:
      default:
	if ((sap->control & LLC1_CLASS) == LLC1_CLASS_U) {
	    *enctype = ET_SAP;
	    *info_size = SAP_HDRBYTES;
	    return;
	}

	*enctype = ET_LLC2;
	*info_size = SAP_LONGHDRBYTES;
	if ((sap->control & ~LLC1_P) == LLC1_XID)
	    *info_size += 3;
	return;
    }
}

/*
 * token_set_data_pointers
 *
 * Given a packet with datagramstart set, and the sizes of the RIF and
 * INFO fields, set up all the data pointers for the packet.
 */
static inline void token_set_data_pointers (
    paktype *pak,
    int riflen,
    int info_size)
{
    tring_hdr *trh;

    trh = (tring_hdr *)pak->datagramstart;
    pak->mac_start     = pak->datagramstart;
    pak->addr_start    = trh->daddr;
    if (riflen) {
	pak->rif_start = trh->data;
    } else {
	pak->rif_start = NULL;
    }

    pak->riflen = riflen;

    if (info_size) {
	pak->info_start = trh->data + riflen;
    } else {
	pak->info_start = NULL;
    }
    pak->network_start = trh->data + riflen + info_size;

    /*
     * This one should go away eventually...
     */
    pak->llc2_sapoffset = pak->info_start - pak->datagramstart;

    /*
     * Record just how big this encapsulation was rather than every protocol
     * have it's own fun and wacky way to work it out.
     */
    pak->encsize = pak->network_start - pak->datagramstart;
}

/*
 * decode/classify token ring encapsulation
 * assumes datagramstart valid on entry
 * (this routine should replace token_decode_encapsulation_inline()
 * when les code moves to using paktype structs for rx pkts.)
 *
 * return rxtype
 */
static inline int tring_decode_encaps (paktype *pak)
{
    snap_hdr *snap;
    tring_hdr *trh;

    trh = (tring_hdr *)pak->datagramstart;
    pak->mac_start   = &trh->ac;
    pak->addr_start  = trh->daddr;

    pak->riflen = get_tr_riflen(trh);
    pak->info_start  = &trh->data[pak->riflen];
    snap = (snap_hdr *)pak->info_start;

    /*
     * SNAP?
     */
    if (GETSHORT(&snap->dsap) == SNAPSNAP) {

        pak->network_start = snap->data;
        pak->encsize       = TRING_SNAP_ENCAPBYTES + pak->riflen;

        switch (GETSHORT(&snap->type)) {
          case TYPE_IP10MB:
            if (ROUTING_IF(pak->if_input, LINK_IP))
                return RXTYPE_SNAP_DODIP;
            else
                return RXTYPE_UNKNOWN;

          case TYPE_RFC826_ARP:
            return RXTYPE_SNAP_DODIP;

          case TYPE_DECNET:
            return RXTYPE_SNAP_DECNET;

          case TYPE_XNS:
            return RXTYPE_SNAP_XNS;

          case TYPE_ETHERTALK:
            return RXTYPE_SNAP_APPLE;

          case TYPE_NOVELL1:
            return RXTYPE_SNAP_NOV1;

          case TYPE_APOLLO:
            return RXTYPE_SNAP_APOLLO;

          case TYPE_VINES:
          case TYPE_VINES2:
          case TYPE_VINES_LOOP:
          case TYPE_VINES_ECHO:
          case TYPE_VINES_ECHO2:
            return RXTYPE_SNAP_VINES;

          case TYPE_RSRB:
            return RXTYPE_SRB;

          default:
            return RXTYPE_UNKNOWN;

        }
    } else {

        pak->network_start = snap->oui;
        pak->encsize       = TRING_SAP_ENCAPBYTES + pak->riflen;

        switch (snap->dsap) {
          case SAP_IP:
            if (ROUTING_IF(pak->if_input, LINK_IP))
                return RXTYPE_SAP_IP;
            else
                return RXTYPE_SAP_UNKN;

          case SAP_CLNS:
            return RXTYPE_SAP_CLNS;

          case SAP_NOVELL:
            return RXTYPE_SAP_NOVELL;

          case SAP_VINES:
            pak->network_start += 1;
            pak->encsize       += 1;
            return RXTYPE_SAP_VINES;

          default:
            return RXTYPE_SAP_UNKN;
        }
    }
}

/*
 * return ptr to ring-bridge-ring number match in rif field
 * or NULL if not found.
 */
static inline uchar *
fs_rif_match (
    srbroute_t *rif,
    int riflen,
    ulong rng_br_rng_f,             /* bits31-4 == forward dir match val */
    ulong rng_br_rng_r)             /* bits31-4 == reverse dir match val */
{
    uchar *ptr;
    ulong rd_info;

    ptr = (uchar *)rif->rd;

    if (rif->rc_dlf & RC_D) {

        /*
         * reverse
         *
         * min riflen of 6 required to start (rc field + ulong of rng/b/rng,
	 *  do one extra compare for each 2 bytes after that
         */

        for ( ; riflen >= 6; riflen -= 2, ptr += 2) {
            rd_info = GETLONG(ptr);
            if (!RD_MISMATCH(rng_br_rng_r, rd_info)) {
                return(ptr);
            }
        }
    } else {
        /*
         * forward
         */
        for ( ; riflen >= 6; riflen -= 2, ptr += 2) {
            rd_info = GETLONG(ptr);
            if (!RD_MISMATCH(rng_br_rng_f, rd_info)) {
                return(ptr);
            }
        }
    }
    return(NULL);
}



static inline void token_output_debug (hwidbtype *hwidb, paktype *pak)
{
    if (tokenring_debug) {
        tring_hdr *trh;         /* pointer to TR MAC header */
        int riflen;             /* length of RIF in TR header */
        ulong *llc1;            /* pointer to the LLC header */
 
        trh = (tring_hdr *)pak->datagramstart;
        if (trh->saddr[0] & TR_RII)          /* have a rif, get its length */
	   riflen = ((trrif_t *)trh)->rc_blth & RC_LTHMASK;
        else
           riflen = 0;
        llc1 = (ulong *) (&trh->data[riflen]);
        tr_print_packet(hwidb, "out:", trh, llc1,
               ((pak->flags & PAK_SRB_OUT) ? BF_BRIDGE :
               ((pak->flags & PAK_SRB_EXPLORER) ?
               (BF_BRIDGE|BF_EXPLORER) : 0)), pak->datagramsize);
    }
}
 

