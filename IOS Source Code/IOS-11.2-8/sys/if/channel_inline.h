/* $Id: channel_inline.h,v 3.3 1996/02/28 18:15:38 kmitchel Exp $
 * $Source: /release/111/cvs/Xsys/if/channel_inline.h,v $
 *------------------------------------------------------------------
 * channel_inline.h - Inline functions for Channel frame classification
 *
 * Feb 16, 1995, Chris Shaker
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: channel_inline.h,v $
 * Revision 3.3  1996/02/28  18:15:38  kmitchel
 * CSCdi43990:  IP packets from the CIP are not being fastswitched on 75XX
 *
 * Revision 3.2  1995/11/17  09:26:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:51:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Etype parsing:
 */

static inline ushort
channel_parse_etype (ushort etype)
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
    case TYPE_NOVELL2:			/* ? XXX */
	rxtype = RXTYPE_NOVELL1;
	break;
    case TYPE_APOLLO:			/* ? XXX */
	rxtype = RXTYPE_APOLLO;
	break;
    case TYPE_VINES2:
    case TYPE_VINES_ECHO2:
    case TYPE_VINES:			/* optional */
    case TYPE_VINES_ECHO:		/* optional */
	rxtype = RXTYPE_VINES;
	break;
    case TYPE_CLNS:			/* ? XXX */
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
 * channel_decode_encapsulation_inline
 *
 * Pull apart and classify an incoming channel frame into it's
 * various segments components, saving the majority of the context
 * in the paktype header.
 */

static inline ushort
channel_decode_encapsulation_inline (paktype *pak, channel_hdr *chan)
{
    ushort	rxtype;

    /*
     * Frame is like ATM AAL5 MUX: enctype is like Ethertype
     */
    rxtype = channel_parse_etype(chan->type);

    pak->enctype = ET_CHANNEL;
    pak->mac_start = pak->datagramstart;
    pak->addr_start = pak->datagramstart;
    pak->rif_start = NULL;
    pak->riflen = 0;
    pak->info_start = NULL;
    pak->encsize = CHANNEL_ENCAPBYTES;

    /* Point past the channel header */
    pak->network_start = pak->datagramstart + pak->encsize;
    return(rxtype);
}
