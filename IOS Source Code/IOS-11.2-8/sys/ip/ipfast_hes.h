/* $Id: ipfast_hes.h,v 3.2.60.1 1996/05/17 11:24:48 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipfast_hes.h,v $
 *------------------------------------------------------------------
 * ipfast_hes.h -- IP Fast Switching Definitions for High End Platforms
 *
 * 1-Aug-1994, Paul Traina
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This lot was moved out of ipfast.h
 *------------------------------------------------------------------
 * $Log: ipfast_hes.h,v $
 * Revision 3.2.60.1  1996/05/17  11:24:48  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/05/02  22:05:21  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.1  1996/04/03  14:42:11  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2  1995/11/17  09:34:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:56:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:04:43  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/09/07  23:09:51  wmay
 * CSCdi39353:  MIP to XXX fast switching failures - use common inline
 * for congestion path on high end and ip multicast, and make it go a
 * little more efficiently.
 *
 * Revision 2.3  1995/08/23  05:31:00  gchristy
 * CSCdi28311:  UDP spanning-tree flooding with DLD creates duplicate
 * packets
 *  - Nuke bridge_flood_pak. Create and initialize udp_flood_pak during FS
 *    init.
 *
 *  - Instead of iterating over all interfaces, use the spanning tree
 *    flood table associated with the input interface to get the list of
 *    interfaces participating in the bridge-group.
 *
 *  - If a potential output interface is part of a circuit-group, use a
 *    hash of the source ip address and destination udp port number to
 *    choose one of the members of the group.
 *
 * Revision 2.2  1995/08/08  19:29:42  fred
 * CSCdi36704:  IP Fastswitching / WFQ cleanup for high-end
 *
 * Revision 2.1  1995/06/07  20:59:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Layout of datagram sniff buffer:
 *
 *	     ARPA	   ISO1		  ISO2		  HDLC
 *
 *	  D  D  D  D	D  D  D  D	D  D  D  D
 *   	  D  D  S  S	D  D  S  S	D  D  S  S
 *	  S  S  S  S	S  S  S  S	S  S  S  S
 *   0	  P  P  x  x	L  L  DS SS	L  L  DS SS	B  B  P  P
 *   1	  x  x  x  x	CT x  x  x	CT SN SN SN	x  x  x  x
 *   2	  x  x  x  x	x  x  x  x	SN SN P  P	x  x  x  x
 *   3    x  x  x  x	x  x  x  x	x  x  x  x	x  x  x  x
 *   4    x  x  x  x	x  x  x  x	x  x  x  x	x  x  x  x
 *   5    x  x 		x  x  x  x 	x  x  x  x	x  x  x  x
 *   6    		x		x  x  x  x
 *   7    				x  x  x  x
 */

#define MCI_IP_LENGTH_OFFSET	1	/* in words */
#define MCI_IP_FRAG_OFFSET	3
#define MCI_IP_CHECKSUM_OFFSET	5
#define MCI_IP_SRCADR_OFFSET	6
#define MCI_IP_DSTADR_OFFSET	8

#define ISO2_OVERHEAD		3

/*
 * Small inline functions to force the compiler to put code where we want it.
 */
inline static void ip_write_ether_header (
    mciregtype *regptr,
    ipcache_rntype *cptr)
{
    regptr->write1long = cptr->m.maclong[0];
    regptr->write1long = cptr->m.maclong[1];
    regptr->write1long = cptr->m.maclong[2];		
}

inline static void ip_write_fddi_header (
    mciregtype *regptr,
    ipcache_rntype *cptr)
{
    int i;

    regptr->write1short = FDDI_LLC_FC_BYTE;
    regptr->write1long = cptr->m.maclong[0];
    regptr->write1long = cptr->m.maclong[1];
    regptr->write1long = cptr->m.maclong[2];
    for (i = IEEEBYTES; i < cptr->length/2 - 1; i++)
	regptr->write1short = cptr->m.macshort[i];
}

/*
 * The following inline is optimized for the case when we know no RIF
 * is present.
 */

inline static void ip_write_fddi_header_norif (
    mciregtype *regptr,
    ipcache_rntype *cptr)
{
    regptr->write1short = FDDI_LLC_FC_BYTE;
    regptr->write1long = cptr->m.maclong[0];
    regptr->write1long = cptr->m.maclong[1];
    regptr->write1long = cptr->m.maclong[2];
    regptr->write1long = cptr->m.maclong[3];
    regptr->write1short = 0;
}

inline static void ip_write_token_header (
    mciregtype *regptr,
    ipcache_rntype *cptr)
{
    int i;

    regptr->write1short = (AC_PRI0 << 8) | FC_TYPE_LLC;
    regptr->write1long = cptr->m.maclong[0];
    regptr->write1long = cptr->m.maclong[1];
    regptr->write1long = cptr->m.maclong[2];
    for (i = IEEEBYTES; i < cptr->length/2 - 1; i++)
	regptr->write1short = cptr->m.macshort[i];
}

inline static void ip_write_token2_header (
    ushort *dgstart,
    ipcache_rntype *cptr)
{
    int i;

    *dgstart++ = (AC_PRI0 << 8) | FC_TYPE_LLC;
    *dgstart++ = cptr->m.macshort[0];
    *dgstart++ = cptr->m.macshort[1];
    *dgstart++ = cptr->m.macshort[2];
    *dgstart++ = cptr->m.macshort[3];
    *dgstart++ = cptr->m.macshort[4];
    *dgstart++ = cptr->m.macshort[5];
    for (i = 6; i < cptr->length/2 - 1; i++)
	*dgstart++ = cptr->m.macshort[i];
}

inline static void ip_write_serial_header (
    mciregtype *regptr,
    ipcache_rntype *cptr)
{
    ushort compare, len;
    ulong *src;
    
    for (len = cptr->length, compare = sizeof(ulong), src = cptr->m.maclong;
	 len >= compare;
	 len -= compare) {
	regptr->write1long = *src++;
    }
    if (len)
	regptr->write1short = GETSHORT(src);
}


inline static void ip_write_smds_header (
     mciregtype *regptr,
     ipcache_rntype *cptr,
     ushort head_offset)
{
    regptr->argreg = head_offset;
    regptr->cmdreg = MCI_CMD_TX1_SELECT;

    ip_write_serial_header(regptr,cptr);
}

inline static void ip_write_smds_trailer (
     mciregtype *regptr,
     ushort	*src,
     char       pad,
     ulong      lastlong,
     ushort     pad_offset)					  
    			
{
  
    regptr->argreg = pad_offset;
    regptr->cmdreg = MCI_CMD_TX1_SELECT;
    switch (pad) {
    case 3: lastlong &= 0xff000000; break;
    case 2: lastlong &= 0xffff0000; break;
    case 1: lastlong &= 0xffffff00; break;
    default: break;
    }
    if (pad)
	regptr->write1long = lastlong;
    regptr->write1short = *src++;
    regptr->write1short = *src;
}

/*
 * AIP header write routine
 *
 * Look closely, this routine is supposed to handle FS traffic to and from
 * VCs that have NLPID, MUX, LLC/SNAP & SMDS encapsulations.
 *
 * WARNING WARNING: this has been greatly simplified from what was
 * originally put in 10.2 by rlfs because his extra stuff did nothing
 * but slow down code.
 */
inline static void ip_write_aip_header (mciregtype *regptr,
				        ipcache_rntype *cptr,
					uint txoff, uint pdulen)
{
    ushort *src = cptr->m.macshort;
    ushort enclen = cptr->length;

    regptr->argreg = txoff;
    regptr->cmdreg = MCI_CMD_TX1_SELECT;

    while (enclen > 0) {
	regptr->write1short = *src++;
	enclen -= sizeof(short);
    }
}

inline static void ip_write_channel_header (mciregtype *regptr,
					    ipcache_rntype *cptr)
{
    regptr->write1long = cptr->m.maclong[0];
}

/*
 * This routine must write 16 bit words.  This is because a MIP header
 * 16 bits may be at the front of a packet.
 */
inline static void ip_write_lex_header (mciregtype *regptr,
					ipcache_rntype *cptr)
{
    ushort *src = cptr->m.macshort;
    ushort enclen = cptr->length;

    while (enclen) {
	regptr->write1short = *src++;
	enclen -= sizeof(short);
    }
}


/*
 * Lay down 38 bytes of dot10 vLAN encapsulating IP Header, prepended to
 * an Ethernet V2 header.
 */

inline static void ip_write_enet_dot10_vlan_header (mciregtype *regptr,
                                                    ipcache_rntype *cache_ptr)
{
    regptr->write1long = cache_ptr->m.maclong[0];
    regptr->write1long = cache_ptr->m.maclong[1];
    regptr->write1long = cache_ptr->m.maclong[2];
    regptr->write1long = cache_ptr->m.maclong[3];
    regptr->write1long = cache_ptr->m.maclong[4];
    regptr->write1long = cache_ptr->m.maclong[5];
    regptr->write1long = cache_ptr->m.maclong[6];
    regptr->write1long = cache_ptr->m.maclong[7];
    regptr->write1long = cache_ptr->m.maclong[8];
    regptr->write1short = cache_ptr->m.macshort[9];
}


/* 
 * Lay down a 26 byte ISL vLAN Header, prepended to an Ethernet V2 header,
 * 40 bytes in total! (refer to the header format in vlan/vlan.h).
 *
 * Nota Bene: Currently we do not update the 802.3 length field since ISL
 *            doesn't look at it. However when the MICE (Mini ISL CRC Engine)
 *            is developed, then we'll need to compute the actual packet
 *            length inclusive of the CRC, by passing in the `base_len' value,
 *            because it'll probably validate it.
 * 
 * Note this is only applicable on Fast Ethernet interfaces.
 */

inline static
void ip_write_fast_ethernet_isl_vlan_header (ulong      *cache_word,
					     mciregtype *regptr)
{
    regptr->write1long = *cache_word++;
    regptr->write1long = *cache_word++;
    regptr->write1long = *cache_word++;
    regptr->write1long = *cache_word++;
    regptr->write1long = *cache_word++;
    regptr->write1long = *cache_word++;
    regptr->write1long = *cache_word++;
    regptr->write1long = *cache_word++;
    regptr->write1long = *cache_word++;
    regptr->write1long = *cache_word;
}


/*
 * Max number of bytes to ask the cbus controller to bcopy for us at any
 * one time.
 */
#define FCI_BCOPY_LIMIT	200



/*
 * External functions
 */

extern boolean ether_ip_fastswitch(hwidbtype *);
extern boolean fddi_ip_fastswitch(hwidbtype *);
extern boolean hdlc_ip_fastswitch(hwidbtype *);
extern boolean smds_ip_fastswitch(hwidbtype *);
extern boolean smds_unknown_fastswitch(hwidbtype *);
extern boolean ppp_unknown_fastswitch(hwidbtype *);
extern boolean hdlc_ipunc_fastswitch(hwidbtype *);
extern boolean token_ip_fastswitch(hwidbtype *);
extern boolean udp_fast_flood(idbtype *);
extern void udp_fast_flood_init(void);
extern boolean aip_ip_fastswitch(hwidbtype *);
extern boolean channel_ip_fastswitch(hwidbtype *);
extern boolean isl_ether_ip_fastswitch(hwidbtype *);
