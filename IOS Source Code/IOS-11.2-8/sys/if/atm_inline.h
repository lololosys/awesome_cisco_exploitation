/* $Id: atm_inline.h,v 3.3.4.2 1996/06/04 21:45:19 ddecapit Exp $
 * $Source: /release/112/cvs/Xsys/if/atm_inline.h,v $
 *------------------------------------------------------------------
 * atm_inline.h - Inline functions for ATM frame classification
 *
 * Sep 28, 1994, Chris Shaker
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: atm_inline.h,v $
 * Revision 3.3.4.2  1996/06/04  21:45:19  ddecapit
 * CSCdi59496:  Code Review Comments for Token Ring LANE
 * Branch: California_branch
 *
 * Revision 3.3.4.1  1996/05/09  14:29:40  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.32.1  1996/04/27  07:03:32  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.22.2  1996/04/09  02:15:22  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Easter bunny had a field day last weekend through the code.  Non-cisco
 * coding styles squashed.  Dangling else's now attached to }'s, else's
 * now attached to if's, 10 character tabs removed...
 *
 * Revision 3.3.22.1  1996/04/08  14:47:27  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.20.1  1996/04/08  07:40:47  ddecapit
 * Sync to LE_Syn_Cal_V111_1_3_branch
 * Branch: TRlane_LE_Syn_1_3_branch
 *
 * Revision 3.3  1996/03/08  23:31:29  shj
 * CSCdi50945:  LANE client does not handle data arriving on multicast-send
 * Treat packets coming in on mcast-send same as mcast-forward.
 * Also avoid raw_enqueue() after retbuffer().
 *
 * Revision 3.2  1995/11/17  09:25:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:47:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:02:42  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:50:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Etype parsing:
 */

static inline ushort
atm_parse_etype (ushort etype)
{
    ushort rxtype;

    switch (etype) {
    case TYPE_IP10MB:
	rxtype = RXTYPE_DODIP;
	break;
    case TYPE_DECNET:
	rxtype = RXTYPE_DECNET;
	break;
    case TYPE_XNS:
	rxtype = RXTYPE_XNS;
	break;
    case TYPE_ETHERTALK:
	rxtype = RXTYPE_APPLE;
	break;
    case TYPE_NOVELL1:
	rxtype = RXTYPE_NOVELL1;
	break;
    case TYPE_APOLLO:
	rxtype = RXTYPE_APOLLO;
	break;
    case TYPE_VINES2:
    case TYPE_VINES_ECHO2:
    case TYPE_VINES:			/* optional */
    case TYPE_VINES_ECHO:		/* optional */
	rxtype = RXTYPE_VINES;
	break;
    case TYPE_CLNS:
	rxtype = RXTYPE_CLNS;
	break;
    case TYPE_RSRB:
	rxtype = RXTYPE_SRB;
	break;
    case TYPE_DEC_SPANNING:
    default:
	rxtype = RXTYPE_UNKNOWN;
	break;
    }
    return(rxtype);
}

/*
 * SNAP parsing:
 */

static inline ushort
atm_parse_snap (snap_hdr *snap, paktype *pak)
{
    ushort	rxtype;
    ushort	dsap_ssap;

    /*
     * Check to see if we've got a SNAPSNAP code
     * in the right place.
     */
    dsap_ssap = GETSHORT(&snap->dsap);
    switch (dsap_ssap) {

    case SNAPSNAP:
	/*
	 * Okay. We've got an ISO2 packet so work
	 * out what type it is from the SNAP header
	 */
	rxtype = atm_parse_etype(GETSHORT(&snap->type));
	pak->network_start = (uchar *) snap->data;
	break;

    case TYPE_CLNS:
	rxtype = RXTYPE_CLNS;
	pak->network_start = (uchar *) snap->oui;
	break;

    default:
	rxtype = RXTYPE_UNKNOWN;
	pak->network_start = (uchar *) snap->data;
    }

    return(rxtype);
}

/*
 * atm_decode_encapsulation_inline
 *
 * Pull apart and classify an incoming ATM frame into it's
 * various segments components, saving the majority of the context
 * in the paktype header.
 */

static inline ushort
atm_decode_encapsulation_inline (paktype        *pak,
                                 atm_encap_t    *atm,
                                 vc_info_t      *vc,
                                 boolean        *lane_flush,
                                 boolean        *try_tbridge)
{
    ushort	atmtype;
    ushort	rxtype;

    /*
     * Is our frame AAL5 Mux?
     */
    atmtype = atm->enctype;
    if (atmtype >= 1500) {

	/*
	 * Yes. AAL5 Mux. Atmtype is like Ethertype:
	 */
	pak->network_start = (uchar *) atm->data;
	rxtype = atm_parse_etype(atmtype);
	
    } else {
	snap_hdr	*snap;

	/*
	 * Switch based on the AIP type value:
	 */
	switch (atmtype) {

	case ATM_ENCTYPE_SNAP:		/* AAL5-LLC/SNAP type */
	    {
		atm_aal5snap_t *atm_snap;

		atm_snap = (atm_aal5snap_t *) atm;
		snap = &atm_snap->snap;
		/*
		 * atm_parse_snap() sets network_start
		 */
		rxtype = atm_parse_snap(snap, pak);
	    }
	    break;

	case ATM_ENCTYPE_NLPID:		/* AAL5-LLC/NLPID */
	    {				/* RFC1483 encaps */
		atm_aal5nlpid_t *atm_nlpid;

		atm_nlpid = (atm_aal5nlpid_t *) atm;

		switch (atm_nlpid->nlpid) {

		    /*
		     * The CLNP and IETF_IP cases are different
		     * from the others:
		     */
		case ATM_NLPID_CLNP:		/* LLC/NLPID/CLNP */
		    pak->network_start = (uchar *) &atm->data[1];
		    rxtype = RXTYPE_CLNS;
		    break;

		case ATM_NLPID_IETF_IP:		/* LLC/NLPID/IETF_IP */
		    pak->network_start = (uchar *) atm_nlpid->data;
		    rxtype = RXTYPE_DODIP;
		    break;

		    /*
		     * Rest are similar to each other:
		     */
		case ATM_NLPID_SNAP1PAD:	/* LLC/NLPID/SNAP */
		    snap = (snap_hdr *) atm->data;
		    pak->network_start = (uchar *) snap->data;
		    rxtype = atm_parse_etype(GETSHORT(&snap->type));
		    break;

		case ATM_NLPID_SNAP0PAD:
		    snap = (snap_hdr *) ((uint)(atm->data) - 1);
		    pak->network_start = (uchar *) snap->data;
		    rxtype = atm_parse_etype(GETSHORT(&snap->type));
		    break;

		case ATM_NLPID_ESIS:
		case ATM_NLPID_ISIS:
		case ATM_NLPID_ISO:
		default:
		    rxtype = RXTYPE_UNKNOWN;
		    pak->network_start = pak->datagramstart + vc->encapsize;
		}
	    }
	    break;

	case ATM_ENCTYPE_SMDS:		/* AAL3/4-SMDS type */
	    {
		smdstype *smds;

		smds = (smdstype *) atm->data;
		snap = (snap_hdr *) smds->data;
		/*
		 * atm_parse_snap() sets network_start
		 */
		rxtype = atm_parse_snap(snap, pak);
	    }
	    break;

        case ATM_ENCTYPE_LANE:          /* LANE */
        case ATM_ENCTYPE_LANE_BUS:
        /*
         *                                rxtype        try_tbridge lane_flush
         * LANE control packets &
         *  pkts to LES/BUS             LANE_CONTROL       false      false
         *
         * LANE data pkts w/ my lecid   LANE_CONTROL       false      true
         *
         * LANE data pkts with dest
         *  MAC == unicast, me          from ether_decode  false      false
         *
         * LANE data pkts with dest
         *  MAC == unicast, not me      from ether_decode  true       true
         *
         * LANE data pkts with dest
         *  MAC == multicast            from ether_decode  true       false
         */
            pak->network_start = pak->datagramstart + vc->encapsize;
            switch(vc->user_vc_type) {
            case LANE_LEC_FORWARD:
            case LANE_LEC_SEND:
                if (lane_from_my_lecid_inline(pak, vc)) {
                    rxtype = RXTYPE_LANE_CONTROL;
                    *lane_flush = TRUE;
                    break;
                }
                /* fall through: treat as LEC_DATA if not from our lecid */
            case LANE_LEC_DATA:

                if (lane_is_control_pak_inline(pak)) {
                    rxtype = RXTYPE_LANE_CONTROL;
                } else {
                    rxtype = ether_decode_encapsulation_inline(pak,
			       (ether_hdr *)(pak->datagramstart +
			       ATM_ENCAP_HDRBYTES +
                               ATM_LANE_LECID_ENCAPBYTES));
                    if (!lane_is_my_mac_inline(pak, vc)) {
                        *try_tbridge = TRUE;
                        if (lane_is_unicast_mac_inline(pak)) {
                            *lane_flush = TRUE;
                        }
                    }
                }
                break;
            case LANE_LEC_TR_FORWARD:
                if (lane_from_my_lecid_inline(pak, vc)) {
                    rxtype = RXTYPE_LANE_CONTROL;
                    *lane_flush = TRUE;
                    break;
                }
                /* fall through: treat as LEC_DATA if not from our lecid */
            case LANE_LEC_TR_DATA:

                if (lane_is_control_pak_inline(pak)) {
                    rxtype = RXTYPE_LANE_CONTROL;
                } else {

                    /* move pak->datagramstart to the beginning of tring_hdr */
                    pak->datagramstart += ATM_ENCAP_HDRBYTES + ATM_LANE_LECID_ENCAPBYTES;
                    pak->datagramsize  -= ATM_ENCAP_HDRBYTES + ATM_LANE_LECID_ENCAPBYTES;
                    rxtype = tring_decode_encaps(pak);

                    /* restore pak->datagramstart to LECID after pak is classified */
                    pak->datagramstart -= ATM_ENCAP_HDRBYTES + ATM_LANE_LECID_ENCAPBYTES;
                    pak->datagramsize  += ATM_ENCAP_HDRBYTES + ATM_LANE_LECID_ENCAPBYTES;
                }
                break;

            default:
                rxtype = RXTYPE_LANE_CONTROL;
                break;
            }
            break;

	case ATM_ENCTYPE_RAW:		/* RAW or ILMI OAM F4/F5 cell */
                                        /* or LANE BUS                */
            /*
             * vc's coming into the LANE BUS have enctype set to RAW
             * in order to keep the cbus microcode from messing with with
             * packets to the BUS.  If this is a LANE vc, it must be a
             * packet coming into the BUS, so set rxtype for LANE.
             */
            pak->network_start = pak->datagramstart + vc->encapsize;
            if ((vc->flags & VC_FLAG_AAL_MASK) == ATM_VC_AAL5LANE) {
                pak->atm_vc = vc;
                rxtype = RXTYPE_LANE_CONTROL;
            } else {
                rxtype = RXTYPE_UNKNOWN;
            }
            break;

        case ATM_ENCTYPE_FRNLPID:	/* AAL5-LLC/FR-NLPID */
	case ATM_ENCTYPE_SAAL:		/* SAAL type frame */
	default:
	    rxtype = RXTYPE_UNKNOWN;
	    pak->network_start = pak->datagramstart + vc->encapsize;

	}
    }
    pak->enctype = ET_ATM;
    pak->mac_start = pak->datagramstart;
    pak->addr_start = NULL;
    pak->rif_start = NULL;
    pak->riflen = 0;
    pak->info_start = pak->datagramstart + ATM_ENCAP_HDRBYTES;

    /*
     * Record just how big this encapsulation was rather than every protocol
     * have it's own fun and wacky way to work it out.
     */
    pak->encsize = pak->network_start - pak->datagramstart;

    return(rxtype);
}
