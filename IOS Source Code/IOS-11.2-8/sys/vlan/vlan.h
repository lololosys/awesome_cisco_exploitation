/*--------------------------------------------------------------------------
 *
 * $Id: vlan.h,v 3.7.10.1 1996/08/15 00:44:59 tylin Exp $
 *
 * vlan/vlan.h
 *
 * April 1995, Martin McNealis.
 *
 * Copyright (c) 1995-1997 by Cisco Systems, Inc.
 *
 * All rights reserved.
 *
 * Definitions supporting IEEE 802.10 and Inter Switch Link (ISL) vLANs.
 *
 *--------------------------------------------------------------------------
 *
 * $Log: vlan.h,v $
 * Revision 3.7.10.1  1996/08/15  00:44:59  tylin
 * CSCdi66084:  cdp should send updates on lowest numbered vlan
 * Branch: California_branch
 *
 * Revision 3.7  1996/02/25  03:52:44  mmcneali
 * CSCdi49686:  IPX non-functional in gs7 images on ISL encapsulating
 *              subinterfaces.
 *
 * Revision 3.6  1996/02/05  04:04:09  mmcneali
 * CSCdi48150:  Cannot bridge between vLANs.
 *
 * Revision 3.5  1996/01/23  08:55:25  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 *
 * $Endlog$
 */


#include "master.h"
#include "interface_private.h"
#include "config.h"

#include "../ui/debug_macros.h"


/*
 * Exported functions and external declarations.
 */

extern  void    vlan_init (subsystype *);
extern  void    encap_isl_vlan (parseinfo *csb);
extern  void    deencap_isl_vlan (idbtype *swidb, boolean sense);

extern  boolean write_isl_encapsulation (paktype *pak, hwidbtype *hwidb,
					 int linktype, char *interior_daddr,
					 char const *other_info);

extern  boolean write_isl_vlan_colour_2_feip (idbtype *subinterface,
                                              boolean on_or_off);

extern  void    isl_vlan_interface_nvram_write (parseinfo *csb);
extern  void    deconfigure_vlan_switching (idbtype *vlan_subinterface);

extern  boolean isl_vlan_bridging_capable_subinterface (idbtype  *input_swidb,
						        int       index,
                                                        spantype *span);

extern  void    set_vlan_sse_switched (idbtype *vlan_subinterface,
				       boolean on_or_off);

extern  void    isl_enqueue (paktype *pak);

extern  uchar   isl_header[];

extern  isl_vlan_descriptor *isl_vlan_table[];


/* 
 * The following details the format of the 26 byte ISL header. This appears
 * prefixed to the native/original packet, refered to within the Virtual LAN
 * code as the "interior packet".
 * Appended to an ISL frame on the wire is an exterior 4 byte CRC coming after
 * the interior packet's own CRC.
 * The vLAN colour has encoded within it a flag indicating whether or not the
 * interior packet is a Spanning Tree BPDU *or* a CDP Hello packet (don't ask
 * why!). Currently this is the bottom/least significant bit, such that the
 * vLAN ID is encoded in 10 bits (the Cisco Catalyst 5000 supports up to 1000
 * distinct vLAN IDs) i.e. bits 1 thru 11. The use of the remaining top 4 bits
 * is undefined.
 * 
 *               DA (6)           SA (6)           Length (2)      SNAP (6)
 *
 *         0x01000C000000    MAC Address of     Reported packet 0xAAAA0300000C
 *                        egressing interface   length plus 30
 *
 *           vLAN Colour (2)  Index Bits (2)  Reserved Bits (2)
 *
 *           Configurable       0x0000            0x0000
 */


/*
 * Defined Macros and Constants.
 */

#define IS_DOT10_VLAN_SUBINTERFACE(swidb)                                    \
                                                                             \
    (swidb && (swidb->sde_said_db != NULL))

#define IS_ISL_VLAN_SUBINTERFACE(swidb)                                      \
                                                                             \
    (swidb && (swidb->isl_vlan != NULL))

#define IS_VLAN_CONFIGURED_SUBINTERFACE(swidb)                               \
                                                                             \
    (IS_DOT10_VLAN_SUBINTERFACE(swidb) || IS_ISL_VLAN_SUBINTERFACE(swidb))

/* Currently Silicon Switching supported for routed ISL vLAN packets */

#define IS_SSE_SWITCHED_VLAN(swidb)                                          \
                                                                             \
    (IS_ISL_VLAN_SUBINTERFACE(swidb) &&                                      \
     (swidb->isl_vlan->vlan_switching_path == vLAN_SSE_SWITCHING))


#define  MAX_VLAN_ID         1000         /* Maximum number for a vLAN ID. */

#define  MAX_ISL_VLANS       256          /* Dimension of hashed array
					     of ISL vLAN colours. */

#define  BPDU_OR_CDP_MASK    0x0001       /* If bottom bit of ISL colour field
                                             set then packet is either a CDP
					     or Spanning Tree BPDU packet! */

#define  ISL_HEADER_LENGTH   26           /* Bytes of prepended ISL header */

#define  ISL_CRC_LENGTH      4            /* Bytes of appended ISL CRC */

#define  ISL_COLOUR_OFFSET   10           /* The offset of the 2 byte colour
                                             within the ISL header. Note MCI
                                             word alignment so 20 bytes. */

#define  ISL_COLOUR_1        1		  /* i.e. VLAN #1 */


#define  ISL_VLAN_ID_OFFSET  20           /* ISL colour offset on packet
					     memory based machines */

#define  ISL_INTERIOR_PACKET 13           /* The Start of the Interior MAC
					     Packet, i.e. skip the 26 bytes
                                             of ISL Header. */

#define  INTERIOR_SNAP_PACKET_TYPE_OFFSET 23 /* The offset to the Interior
                                                Packet's Type Field. */

#define  ISL_DO_CRC_MASK    0x2000        /* Indicates to the ucode that it
					     should compute a CRC for the
					     interior packet */

#define  ISL_IP_TTL_OFFSET    4           /* Offset of the IP Time2Live byte in
					     an ISL encapsulated IP datagram.
					     Skip the Version, TOS, Length, ID,
					     Flags and Fragmentation Offset and
					     decrement the fucker 'coz the
					     ucode won't (word aligned). */

#define ISL_MCAST_LONG        0x01000C00  /* The top four bytes of the Cisco
					     multicast DA used by ISL. */

/*
 * vLAN datagrams may be Silicon Switched on the CxBus/C7000 platform by
 * having the Fast Ethernet line card maintain its own list of vLAN IDs
 * that it checks received packets against in order to determine how they
 * should be presented to the SSE (i.e. as a vLAN packet, or the actual
 * internal datagram. Currently this is supported for ISL, but the mechanism
 * could be applied to dot10 on say the FIP card.
 *
 * Primitives used in communicating with the FEIP to toggle a vLAN ID SSE
 * switched or not. We write the colour via the arg1 register such that if
 * the top bit is set it's on, if it's zero then the FEIP won't attempt to
 * have the SSE switch the interior packet.
 */

#define  SET_vLAN_SSE_SWITCHING   0x8000
#define  CLEAR_vLAN_SSE_SWITCHING 0x0000

/*
 * Indicate which switching path is used for a particular vLAN on the C7000.
 */

enum VLAN_SWITCHING_CACHE {

    vLAN_FAST_SWITCHING,                  /* The default */
    vLAN_SSE_SWITCHING,
};


/*
 * This is the Inter Switch Link (ISL) Virtual LAN data structure.
 *
 * Note we make provision for multiple software IDBs associated with a
 * particular colour. Of course they must be software IDBs of different
 * hardware IDBs.
 *
 * Let's keep this long word aligned ...
 */

struct isl_vlan_descriptor_ {

    idbtype             *isl_vlan_subinterface[MAX_INTERFACES]; 

                                                   /* Software IDBs associated
                                                      with this ISL entity */

    ushort               colour;                   /* Virtual LAN Identifier */

    ushort               number_subif_in_isl_vlan; /* Keep a count */

    isl_vlan_descriptor *next_isl_vlan_db;         /* Descriptors Linked List */

    ulong                ingressing_isl_packets[VLAN_PROTOCOLS_MAX];
    ulong                egressing_isl_packets[VLAN_PROTOCOLS_MAX];

    enum VLAN_SWITCHING_CACHE vlan_switching_path; /* Fast or SSE */

};


/*
 * write_dot10_cache_macheader()
 *
 * Construct an IEEE 802.10 vLAN encapsulation header for any routed
 * packet having a Fast Switching dot10 cache entry. This is used for
 * FDDI and Token Ring, i.e. no length computation.
 *
 * Nota Bene: Originating MAC header may be in 802.3/802.2 or SNAP
 *            format i.e. this can be variable length, denoted by 
 *            "dot2_format".
 */

static inline void write_dot10_cache_macheader (ulong   *packet_macptr,
					        ulong   *cache_macptr,
                                                boolean  dot2_format)
{

    if (dot2_format) {

        PUTLONG(packet_macptr++, *cache_macptr++);
        PUTLONG(packet_macptr++, *cache_macptr++);
        PUTLONG(packet_macptr++, *cache_macptr++);
        PUTLONG(packet_macptr++, *cache_macptr++);
        PUTLONG(packet_macptr++, *cache_macptr++);
        PUTLONG(packet_macptr++, *cache_macptr++);
        PUTLONG(packet_macptr++, *cache_macptr++);
        PUTSHORT(packet_macptr,  *((ushort *)cache_macptr));
	(uchar *)  packet_macptr += 2;
        *(uchar *) packet_macptr = LLC1_UI;

    } else {    /* Interior packet is in SNAP format */

        PUTLONG(packet_macptr++, *cache_macptr++);
        PUTLONG(packet_macptr++, *cache_macptr++);
        PUTLONG(packet_macptr++, *cache_macptr++);
        PUTLONG(packet_macptr++, *cache_macptr++);
        PUTLONG(packet_macptr++, *cache_macptr++);
        PUTLONG(packet_macptr++, *cache_macptr++);
        PUTLONG(packet_macptr++, *cache_macptr++);
        PUTLONG(packet_macptr++, *cache_macptr++);
        PUTLONG(packet_macptr++, *cache_macptr++);
    }
}


/*
 * write_isl_cache_macheader()
 *
 * Prepend an ISL vLAN encapsulation and interior MAC header to any routed
 * packet having a Fast Switching ISL cache entry.
 *
 * Nota Bene:- ISL assumes that the interior packet is in Ethernet V2 format,
 *             hence pass in an Ethernet Type field. Also explicitly writes
 *             the length field which must allow for the exterior CRC.
 *
 *             This is only defined on Fast Ethernet.
 */

static inline void write_isl_cache_macheader (ulong *packet_macptr,
			 	       	      ulong *cache_macptr)
{
    /* Cisco multicast destination and egressing interface address */

    PUTLONG(packet_macptr++, *cache_macptr++);
    PUTLONG(packet_macptr++, *cache_macptr++);
    PUTLONG(packet_macptr++, *cache_macptr++);

    /* 802.3 length computed by the FEIP ucode */

    PUTLONG(packet_macptr++, *cache_macptr++);
    PUTLONG(packet_macptr++, *cache_macptr++);

    /* ISL vLAN Colour, Index Bits and Reserved Bits (currently unused) */

    PUTLONG(packet_macptr++, *cache_macptr++);
    PUTLONG(packet_macptr++, *cache_macptr++);

    /* and the interior Ethernet V2 header */

    PUTLONG(packet_macptr++, *cache_macptr++);
    PUTLONG(packet_macptr++, *cache_macptr++);
    PUTLONG(packet_macptr,   *cache_macptr);
}


/*
 * minimal_isl_cache_header_write()
 *
 * Let's not do any more than we absolutely have to in order to have
 * this packet ISL'ed - it should never have defined 26 bytes to carry
 * a 10 bit colour field to begin with :-)
 *
 * IOS portability is sacraficed here for speed; if that becomes
 * an issue for non RSP platforms, revert to the previous version
 * write_isl_cache_macheader().
 *
 * The total ISL cache write is an even 40 bytes ...
 */

static inline void minimal_isl_cache_header_write (ulong *packet_macptr,
			     		           ulong *cache_macptr)
{
    /* Cisco multicast destination, bottom 2 bytes are 0x00 anyways */

    *packet_macptr++ = ISL_MCAST_LONG;
    *packet_macptr   = 0x00000000;

    /*
     * Skip the other cruft up until the pseudo-SNAP
     * TYPE field which is used to carry the colour.
     */

    packet_macptr += 4;
    cache_macptr  += 5;

    *packet_macptr++ = *cache_macptr++;

    /* Trailing 00's of the ISL header, then the interior Ethernet MAC */

    *packet_macptr++ = *cache_macptr++;
    *packet_macptr++ = *cache_macptr++;
    *packet_macptr++ = *cache_macptr++;
    *packet_macptr   = *cache_macptr;
}


/*
 * write_ether_dot10_cache_macheader()
 *
 * Construct an IEEE 802.10 vLAN encapsulation header to any routed
 * packet type on Ethernet having a Fast Switching dot10 cache entry.
 *
 * Nota Bene, with Ethernet we have to set up the 802dot3 length field.
 */

static inline void write_ether_dot10_cache_macheader (ulong *packet_macptr,
					              ulong *cache_macptr,
                                                      ushort dot3_length_field,
                                                      ushort encap_size)
{

register ushort index;

    PUTLONG(packet_macptr++, *cache_macptr++);
    PUTLONG(packet_macptr++, *cache_macptr++);
    PUTLONG(packet_macptr++, *cache_macptr++);

    PUTSHORT(packet_macptr++, dot3_length_field);

    cache_macptr += 2;

    /* Now sribble the remainder of the preamble ... */

    for (index = ETHER_8023_ENCAPBYTES; index <= encap_size; index++) {

        *packet_macptr++ = *cache_macptr++;
    }
}
