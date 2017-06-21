/* $Id: tbridge_public.h,v 3.3.4.6 1996/08/28 13:16:49 thille Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/tbridge_public.h,v $
 *------------------------------------------------------------------
 * tbridge/tbridge_public.h
 *
 * April 1994, Tony Speakman
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Generic definitions for transparent bridging
 *------------------------------------------------------------------
 * $Log: tbridge_public.h,v $
 * Revision 3.3.4.6  1996/08/28  13:16:49  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.4.5  1996/07/23  13:29:48  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.3.4.4  1996/05/22  22:55:05  wilber
 * CSCdi56191:  buginf should be errmsg
 * - Convert the unsolicited message to errmsg
 * - Rewrite the unmeaning output string
 * Branch: California_branch
 *
 * Revision 3.3.4.3  1996/05/08  00:24:01  speakman
 * CSCdi56968:  CMF accounting for flooded packets is incorrect
 * Branch: California_branch
 *  - account for IP multicast rx packets in the SA's bte
 *  - show CMF rx and tx packet counts per-interface
 *  - constrain the initial IGMP report on a given interface
 *    to router ports only to prevent suppression and sequential
 *    joining behaviour
 *  - show IGMP timers only when CMF debugging is enabled
 *  - clear the multicast-router-port state when the multicast-group
 *    state is cleared
 *  - flood (unconstrained) IP multicast data packets in the absence of
 *    any multicast-router ports
 *
 * Revision 3.3.4.2  1996/05/04  05:09:14  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - New file(tbridge_bvi.c) for Bridge-group Virtual Interface
 * functions.
 * - Obsolete tbridge_crb_inline.h with tbridge_irb_inline.h.
 * - Bridge/route decision(tbridge_irb_inline.h).
 * - Learn routable packets(DLLI_IRBPKT) in bridge table.
 * - Move locate_bte_inline() and tbridge_get_arpa_type_inline() to
 *   tbridge_common_inline.h.
 * - Rip out MTR transparent bridging support.
 *
 * Constrained Multicast Flooding
 * - Add the tbridge monitor.
 * - Add the tbridge registry.
 * - Add the entire CMF subsystem.
 * - Convert tbridge flood array to a linked list of tbifd's.
 * - Add the call outs to CMF from the LES, RSP, and PAS forwarding
 *   functions.
 *
 * Revision 3.3.4.1  1996/03/23  22:49:40  ppearce
 * SR/TLB fastswitching feature
 * Branch: California_branch
 *
 * Revision 3.3  1996/03/07  23:33:15  speakman
 * CSCdi48222:  SMF creates chunk storage even when unconfigured
 * Replace SMF chunk memory with managed memory and fix the original
 * problem with a simple check for onintstack.
 *
 * Revision 3.2  1995/11/17  18:45:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:31:34  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/10/26  00:30:49  speakman
 * CSCdi41837:  SMF calls malloc and free at interrupt (SYS-3-INVMEMINT)
 * Initialize tbridge SMF chunk.
 *
 * Revision 2.2  1995/10/21  19:15:08  fred
 * CSCdi39516:  WFQ breaks TB flooding when congested - %SYS-2-BADSHARE
 * recode congestion cases in tbridge_forward_inline and
 * tbridge_flood_inline.
 * These did not adequately handle MTU oversize issues and did not
 * correctly
 * count buffers on multi-VC cases like Frame relay.
 *
 * Revision 2.1  1995/06/07  23:03:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************************

			MAC ENCAPSULATION TYPE BY MEDIA

	These are encapsulations derived from the sizes defined in
	ether.h, fddi.h, and tring.h.


   ETHERNET   |	dest	source	type/	DSAP	SSAP	CTL	OUI	SNAP-
	      |	addr	addr	length					TYPE
standard      |
name	a.k.a.|	6 bytes	6 bytes	2 bytes	1 byte	1 byte	1 byte	3 bytes	2 bytes
--------------|----------------------------------------------------------------
ARPA	ETHER |	DA	SA	TYPE
802.3	      |	DA	SA	LENGTH
SAP	ISO1  |	DA	SA	LENGTH	DSAP	SSAP	CTL
SNAP	ISO2  |	DA	SA	LENGTH	"AA"	"AA"	"03"	"00"	TYPE


     FDDI     |	frame	dest	source	DSAP	SSAP	CTL	OUI	SNAP-
	      |	control	addr	addr					TYPE
standard      |
name	a.k.a.|	1 byte	6 bytes	6 bytes	1 byte	1 byte	1 byte	3 bytes	2 bytes
--------------|----------------------------------------------------------------
	FDDI  |	FC	DA	SA
SAP	ISO1  |	FC	DA	SA	DSAP	SSAP	CTL
SNAP	ISO2  |	FC	DA	SA	"AA"	"AA"	"03"	"00"	TYPE

     ATM      |	frame	dest	source	DSAP	SSAP	CTL	OUI	SNAP-
	      |	control	addr	addr					TYPE
standard      |
name	a.k.a.|	4 bytes	3 bytes	3 bytes	2 bytes	1 or 2 bytes
--------------|----------------------------------------------------------------
	ATM   |	VC/ENC	LLC     OUI     PID     PAD      encapsulated mac packet



  TOKEN RING  |	access	dest	source	DSAP	SSAP	CTL	OUI	SNAP-
	      |	control	addr	addr					TYPE
	      |	frame
	      |	control
standard      |
name	a.k.a.|	2 bytes	6 bytes	6 bytes	1 byte	1 byte	1 byte	3 bytes	2 bytes
--------------|----------------------------------------------------------------
802.5	TRING |	ACFC	DA	SA
SAP	ISO1  |	ACFC	DA	SA	DSAP	SSAP	CTL
SNAP	ISO2  |	ACFC	DA	SA	"AA"	"AA"	"03"	"00"	TYPE



	Packets provided as arguments to transparent bridging functions
	and inlines are expected to conform to the following:

     pak->if_input points to the software idb of the interface on which
                   the packet was received; we need this to check
		   if_input->hwptr->bridge_media_type for translation

     pak->datagramsize is the number of raw bytes received
                       usually supplied by the driver

     pak->datagramstart points to the beginning of the MAC header
                        i.e., the beginning of the received packet
                        INCLUDING any transit encapsulation and
                        preamble to the DA

      - the DA for ether packets
      - the FC byte for fddi packets
      - the AC byte for token packets
      - the HDLC station address (0x0F00) for serial HDLC packets

     pak->mac_start points to the beginning of the MAC header
                    of the packet to be bridged EXCLUDING any
                    transit encapsulation

     For non-transit bridging (ETHER, TOKEN, FDDI),
     datagramstart == mac_start.
     
     For transit bridging (PPP, HDLC, transit FDDI),
     datagramstart points to the beginning of the transit encapsulation
     while mac_start points to the DA of the transit packet.
     The transit packet is assumed to be either ARPA or 802.3 encapsulation

     pak->addr_start points to the DA (NOT the transit DA)

     pak->info_start points to the LLC header if one is present and
     it is NULL if one is not present

     pak->network_start is used in copied packets to mark the boundary
     between the header to be translated and the remaining data.
     For all encapsulations except SNAP, we align the copy so that
     the MAC header ends at the end of the encapsulation area
     (pak_center a.k.a network_start) where the LLC or data begin.
     For SNAP encapsulations, we align the copy so that the entire
     SNAP header, including LLC and type, ends at the end of the
     encapsulation area where the data begins.

******************************************************************************/


/*
 * We generate bridge-table hash indices based on the last
 * two bytes of MAC address.
 */
#define MAC_HASH1                  4
#define MAC_HASH2                  5

#define ISMULTICAST(x) (*(x) & 0x01)

/*
 * Return values from tbridge_crb_inline:
 */
enum TBRIDGE_IRB_RETVAL {
    TBRIDGE_IRB_FORWARD,	/* packet must be bridged */
    TBRIDGE_IRB_RECEIVE,	/* packet must be received */
    TBRIDGE_IRB_DISCARD,	/* packet must be discarded */
};

/*
 * Return values from tbridge_forward_inline:
 */
enum TBRIDGE_FWD_RETVAL {
    TBRIDGE_FWD_DONE,		/* packet was forwarded */
    TBRIDGE_FWD_FLOOD,		/* packet must be flooded */
    TBRIDGE_FWD_SLOWSWITCH,	/* packet must be bridged at process level */
    TBRIDGE_FWD_RECEIVE,	/* packet must be rec'd by bridge */
};

/*
 * Return values from translation functions tbridge_to_xxx_inline:
 */
enum TBRIDGE_TLB_RETVAL {
    TBRIDGE_TLB_DONE,		/* packet was translated */
    TBRIDGE_TLB_NO_TXBUF,	/* no output resources available */
    TBRIDGE_TLB_EXCEEDS_MTU,	/* translated pkt would be too big */
    TBRIDGE_TLB_UNSUPPORTED,	/* translation not supported */
    TBRIDGE_TLB_DEFERRED,	/* packet translated and enqueued -
				 * which is counted by idb->soutput as
				 * process switched */
};


/*
 * Code switch values to generate forwarding and flooding code for
 * platforms with different packet memories.
 */
enum TBRIDGE_PAKMEM {
    TBRPAK_RSP,			/*
				 * CyBus packet memory - almost like network
				 * packet buffers ... but not quite
				 */
    TBRPAK_BFR,			/* network packet buffers */
    TBRPAK_MCI,			/* MCI/cBus packet memory */
    TBRPAK_VBR,			/* Virtual bridge packet memory */
};

/*
 * Code switch values to generate translational code between interfaces
 * depending upon the nature of the input and output packet memories.
 */

enum TBRIDGE_TRANSLATE {

/*
 * Low-end and RSP translation for both forwarding and flooding is done
 * entirely between network buffers.
 */

    TLB_BFR_TO_BFR,		/* translate between network buffers */

/*
 * High-end translation is all over the dance floor.
 */

    /* forwarding between MCI/cBus interfaces */

    TLB_WITHIN_MCI,		/* translate within the same MCI/cBus */
    TLB_MCI_TO_MCI,		/* translate between different MCI/cBus's */

    /* flooding to MCI/cBus interfaces*/

    TLB_BFR_TO_MCI,		/* translate from network buffer to MCI/cBus */
};

/*
 * Please keep the tbridge_dlli_type_ structure long aligned
 */
typedef struct tbridge_dlli_type_ {
    enum TBR_MEDIA src_media_type; /* type of packet, ETHER, FDDI, etc */
    ushort da_offset;              /* offset to the MAC destination addr
				      currently only used for hi end */
    ushort mac_offset;             /* offset to the MAC header
				      currently only used for hi end */
    ushort transit_encapsize;      /* transit encap size. includes circuit
				      switched header if it exists */
    ushort circuit_id;             /* really VC identifier for circuit
				      switched networks */
    ushort packet_pid;             /* PID of packet if virtual circuit */
    ushort orig_datagramsize;	   /* original datagramsize - used for transit
				      encaps */
    uchar *smdssrc_ptr;		   /* pointer to smds src address */
    uchar smdssrc[STATIONLEN_SMDS64];
    ulong dlli_attributes;
} tbridge_dlli_type;

enum DLLI_ATTRIBUTE_BIT {
    DLLI_IRBPKT_BIT	=  0,
    DLLI_IPMCAST_BIT	=  1,
};

#define DLLI_IRBPKT	(1L << DLLI_IRBPKT_BIT)
#define DLLI_IPMCAST	(1L << DLLI_IPMCAST_BIT)


/******************************************************************************

For CMF, a queue of router ports has been added to the span structure to list
the router ports known in the bridge group.  In addition, a hash table of
queues of group ports has been added to the span structure to list the group
ports in each group known in the bridge group.

Queues of ports are queues of transparent bridging interface descriptors
(tbifd's) each of which points to the swidb of a corresponding interface
in the bridge group.

Queues of group ports are queues of transparent bridging multicast group
descriptors (tbmgd's) each of which points to an outgoing interface
queue (tbmgd_oifQ) of tbifd's, one tbifd for each interface in the group.

So a single multicast group in a given bridge group is characterized by
a tbmgd which describes the group's IP multicast address, outgoing
interface list, and other attributes.

The following diagram illustrates the principle data structures for a single
bridge group:

swidb -> span -> queuetype floodQ -> tbifd -> tbifd -> ...
  ^
  |      span -> queuetype mcast_router_ports -> tbifd -> tbifd -> ...
  |
  |      span -> queuetype *mcast_groups[NETHASHLEN] -> tbmgd -> tbmgd -> ...
  |                                                     -ipaddr  -ipaddr
  |                                                     -oifQ    -oifQ
  |                                                       |        |
  |                                                       V        V
  ----------------------------------------------------  tbifd    tbifd
                                                          |        |
                                                          V        V
                                                        tbifd    tbifd
                                                          .        .
                                                          .        .
                                                          .        .

Notes:
------

 o tbifd's always point back to a corresponding swidb.
 o tbifd's on the tbmgd_oifQ also point back to the corresponding tbmgd
   (not shown above).
 o Every oifQ includes tbifd's for the interfaces in the queue of router
   ports.  In other words, router ports are described both on the queue
   of router ports as well as on each oifQ.

******************************************************************************/


/*
 * Transparent Bridging Multicast Group Descriptor (tbmgd)
 * for describing multicast groups within a bridge group.
 */
typedef struct tbmgd_type_ {
    struct tbmgd_type_	*tbmgd_next;
    ipaddrtype		tbmgd_ipaddr;	/* 32-bit IP multicast address */
    queuetype		tbmgd_oifQ;	/* outgoing interface list */
    int			tbmgd_member_count;
    int			tbmgd_no_rpt_count;
    ulong		tbmgd_attributes;
} tbmgd_type;

#define TBMGD_ATTRIBUTE_SIZE	(sizeof(ulong) * 8)

enum TBMGD_ATTRIBUTE_BIT {
    TBMGD_IGMP_RPT_SUPPRESS_BIT	=  0,
    TBMGD_IGMP_QRY_CURRENT_BIT	=  1,
};

#define TBMGD_IGMP_RPT_SUPPRESS	(1L << TBMGD_IGMP_RPT_SUPPRESS_BIT)
#define TBMGD_IGMP_QRY_CURRENT	(1L << TBMGD_IGMP_QRY_CURRENT_BIT)

/*
 * Transparent Bridging Interface Descriptor (tbifd)
 * for maintaining general bridged-interface lists.
 */
struct tbifd_type_ {
    struct tbifd_type_	*tbifd_next;
    idbtype		*tbifd_swidb;
    ulong		tbifd_attributes;
    ulong		tbifd_rx_flood_packets;
    ulong		tbifd_tx_flood_packets;
    tbmgd_type		*tbifd_tbmgd;
    mgd_timer		tbifd_igmp_qry_tmr;
    mgd_timer		tbifd_igmp_rpt_tmr;
    mgd_timer		tbifd_mrouter_tmr;
    int			tbifd_igmp_qry_expired_count;
    int			tbifd_igmp_rpt_expired_count;
    int			tbifd_mrouter_expired_count;
};

#define TBIFD_ATTRIBUTE_SIZE	(sizeof(ulong) * 8)

enum TBIFD_ATTRIBUTE_BIT {
    /*
     * NOTA BENE: DO NOT change this definition of TBIFD_FLOOD_BIT;
     * it MUST be the first bit so that TBIFD_FLOOD == 1.  This is
     * required since the stub registry for CMF returns 1 when CMF
     * is not present to indicate that a packet should be
     * conventionally flooded.
     */
    TBIFD_FLOOD_BIT		=  0,
    TBIFD_AUTO_FLOOD_BIT	=  1,
    TBIFD_IPMCAST_GROUP_BIT	=  8,
    TBIFD_IPMCAST_ROUTER_BIT	=  9,
};

#define TBIFD_NO_FLOOD		(0L)
#define TBIFD_FLOOD		(1L << TBIFD_FLOOD_BIT)
#define TBIFD_AUTO_FLOOD	(1L << TBIFD_AUTO_FLOOD_BIT)
#define TBIFD_IPMCAST_GROUP	(1L << TBIFD_IPMCAST_GROUP_BIT)
#define TBIFD_IPMCAST_ROUTER	(1L << TBIFD_IPMCAST_ROUTER_BIT)

/*
 * The data-link-layer information structure that serves as a
 * handy dandy scratch pad for data-link-layer attributes for
 * the life of the packet in the receive interrupt.
 */

extern tbridge_dlli_type tbridge_dlli;

/*
 * Everything defined in tbridge.c needs to be declared here.
 */

extern boolean
tbridge_type_sap_filter(paktype *pak, int lsap, int type);

extern void
tbridge_record_proxy_src_addr(paktype *pak,
			      btetype *entry,
			      hwidbtype *inputhw,
			      boolean  bte_created);

extern void
tbridge_check_lat_compression(idbtype *outputsw,
			      paktype *pak);

extern void
tbridge_log_giant(hwidbtype *idb,
		  int length,
		  int mtu,
		  paktype *giant);

extern void
tbridge_print_message (void);

extern btetype *
tbridge_locate_bte(spantype *, uchar const *);

extern boolean
tbridge_smf_update(uchar *mac_addr, idbtype *swidb, ulong result);

extern boolean
tbridge_smf_delete(uchar *mac_addr, idbtype *swidb, ulong result);

extern boolean
tbridge_smf_delete_by_result(idbtype *swidb, ulong result);

extern boolean
bvi_smf_update(uchar *mac_addr, idbtype *swidb, ulong result, ulong flag);

extern boolean
bvi_smf_delete(uchar *mac_addr, idbtype *swidb, ulong result, ulong flag);

extern boolean
bvi_smf_delete_by_result(idbtype *swidb, ulong result);

extern void
tbridge_smf_show(idbtype *swidb);

/*
 * Functions defined in tbridge_monitor.c:
 */

extern void
tbridge_monitor_fiat_lux(void);

extern void
tbridge_monitor_lights_out(void);

