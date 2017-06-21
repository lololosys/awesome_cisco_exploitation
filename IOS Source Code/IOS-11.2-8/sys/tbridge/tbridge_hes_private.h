/* $Id: tbridge_hes_private.h,v 3.2 1995/11/17 18:45:09 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/tbridge/tbridge_hes_private.h,v $
 *------------------------------------------------------------------
 * tbridge/tbridge_hes_private.h
 *
 * April 1994, Tony Speakman
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * High-end-systems-specific definitions for transparent bridging
 *------------------------------------------------------------------
 * $Log: tbridge_hes_private.h,v $
 * Revision 3.2  1995/11/17  18:45:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:31:18  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  23:03:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * These offsets define the locations of the elements of the MAC header
 * for ARPA, 802.3, and 802.5 packets.  They are defined with specific
 * reference to the encapsulation packet that is used throughout the
 * transparent bridging code to hold the MAC header of packets received
 * on MCI/cBus interfaces.  The comments in tbridge_hes_init() and
 * tbridge_public.h clarify these definitions and describe what
 * tbridge_encap[] is.
 */

/* 1ST LONG */

#define TBE_DA			tbridge_encap[0].d.byte[0]
#define TBE_DADA_0		tbridge_encap[0].d.sword[0]
#define TBE_DADA_1		tbridge_encap[0].d.sword[1]
#define TBE_DADADADA		tbridge_encap[0].d.lword

/* 2ND LONG */

#define TBE_DADA_2		tbridge_encap[1].d.sword[0]
#define TBE_SASA_0		tbridge_encap[1].d.sword[1]
#define TBE_DADASASA		tbridge_encap[1].d.lword

/* 3RD LONG */

#define TBE_SASA_1		tbridge_encap[2].d.sword[0]
#define TBE_SASA_2		tbridge_encap[2].d.sword[1]
#define TBE_SASASASA		tbridge_encap[2].d.lword

/* 4TH LONG */

#define TBE_LLC_802_5		tbridge_encap[3].d.byte[0]
#define TBE_LSAP_802_5		tbridge_encap[3].d.sword[0]
#define TBE_LEN_802_3		tbridge_encap[3].d.sword[0]
#define TBE_TYPE_ARPA		tbridge_encap[3].d.sword[0]

#define TBE_LLC_802_3		tbridge_encap[3].d.byte[2]
#define TBE_LSAP_802_3		tbridge_encap[3].d.sword[1]
#define TBE_CTL_OUI_802_5	tbridge_encap[3].d.sword[1]

#define TBE_SDE_CTL_802_5       tbridge_encap[3].d.byte[2]
#define TBE_SDE_SAID_0_802_5    tbridge_encap[3].d.byte[3]
#define TBE_SDE_CTL_SAID_0_802_5        tbridge_encap[3].d.sword[1]

/* 5TH LONG */

#define TBE_OUI_802_5		tbridge_encap[4].d.sword[0]
#define TBE_SNAPTYPE_802_5	tbridge_encap[4].d.sword[1]
#define TBE_OUI_TYPE_802_5	tbridge_encap[4].d.lword
#define TBE_CTL_OUI_802_3	tbridge_encap[4].d.lword

#define TBE_SDE_SAID_1_2_802_5  tbridge_encap[4].d.sword[0]
#define TBE_SDE_SAID_3_802_5    tbridge_encap[4].d.byte[2]
#define TBE_SDE_SID_0_802_5     tbridge_encap[4].d.byte[3]
#define TBE_SDE_SAID_1_3_SID_0_802_5    tbridge_encap[4].d.lword

#define TBE_SDE_CTL_SAID_0_2_802_3      tbridge_encap[4].d.lword

/* 6TH LONG */

#define TBE_SNAPTYPE_802_3	tbridge_encap[5].d.sword[0]

#define TBE_SDE_SID_1_4_802_5   tbridge_encap[5].d.lword

#define TBE_SDE_SAID_3_SID_0_2_802_3    tbridge_encap[5].d.lword

/* 7TH LONG */

#define TBE_SDE_SID_5_6_802_5   tbridge_encap[6].sword[0]
#define TBE_SDE_SID_7_802_5     tbridge_encap[6].byte[2]
#define TBE_SDE_FLAG_802_5      tbridge_encap[6].byte[3]
#define TBE_SDE_SID_5_7_FLAG_802_5      tbridge_encap[6].d.lword

#define TBE_SDE_SID_3_6_802_3   tbridge_encap[6].d.lword

/* 8TH LONG */

#define TBE_SDE_FID_802_5       tbridge_encap[7].d.lword

#define TBE_SDE_SID_7_FLAG_FID_0_1_802_3        tbridge_encap[7].d.lword

/* 9TH LONG */

#define TBE_SDE_SNAP_LSAP_802_5         tbridge_encap[8].d.sword[0]
#define TBE_SDE_SNAP_CTL_OUI_0_802_5    tbridge_encap[8].d.sword[1]
#define TBE_SDE_SNAP_LSAP_CTL_OUI_0_802_5       tbridge_encap[8].d.lword

#define TBE_SDE_SNAP_LSAP_802_3         tbridge_encap[8].d.sword[1]
#define TBE_SDE_FID_2_3_SNAP_LSAP_802_3 tbridge_encap[8].d.lword

/* 10TH LONG */

#define TBE_SDE_SNAP_OUI_1_2_802_5      tbridge_encap[9].d.sword[0]
#define TBE_SDE_SNAP_TYPE_802_5         tbridge_encap[9].d.sword[1]
#define TBE_SDE_SNAP_OUI_1_2_TYPE_802_5 tbridge_encap[9].d.lword

#define TBE_SDE_SNAP_CTL_OUI_802_3      tbridge_encap[9].d.lword

 /* 11TH LONG */

#define TBE_SDE_SNAP_TYPE_802_3         tbridge_encap[10].d.sword[0]

/*
 * Code switch values to generate code for packet formats
 * with or without an 802 length field.
 */
enum LENGTH_802 {
    LENGTH_802_NOT,             /* fddi, tr, etc. */
    LENGTH_802_YES,             /* ether 802.3, etc. */
};

/*
 * This offset is the standard offset used when writing a bridged packet into
 * the tx1 window of an MCI/cBus interface (see tbridge_mci_tx1_select_inline).
 * It is important that it be odd, and it is important that it be at least
 * large enough to prepend the largest transit header, in this case the FDDI
 * transit header.  It also provides for enough room for the re-encap area
 * required by cBus flooding.  This offset does NOT apply to packets that have
 * been moved from an rx to a tx window.  They are at the original rx offset.
 * Expand the size from FDDI_SNAP_ENCAPWORDS to 
 * (FDDI_SNAP_ENCAPWORDS + SDE_ENCAPWORDS) in order to accommodate SDE packets.
 */
#define TBRIDGE_MCI_TX1_OFFSET	(FDDI_SNAP_ENCAPWORDS + SDE_ENCAPWORDS)


/*
 * External data declarations:
 */

extern paktype *tbridge_encap_pak;
extern charlong *tbridge_encap;

extern boolean tbridge_mci_forward(idbtype *inputsw);
