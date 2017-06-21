/* $Id: packet.h,v 3.10.10.20 1996/08/16 19:09:55 ahh Exp $
 * $Source: /release/112/cvs/Xsys/h/packet.h,v $
 *------------------------------------------------------------------
 * packet.h  -- protocol independent packet header definitions
 *
 * 1-October-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: packet.h,v $
 * Revision 3.10.10.20  1996/08/16  19:09:55  ahh
 * CSCdi63890:  %SYS-2-NOBLOCK: event dismiss with blocking disabled
 * Cannot block in IP Input context, so drop incoming SYN if we can't
 * respond reliably.
 * Branch: California_branch
 *
 * Revision 3.10.10.19  1996/07/30  01:07:48  mbeesley
 * CSCdi55762:  Need to support fast IP multicasting on platforms
 * capable of scatter/gather IE c7200
 * Branch: California_branch
 *
 * Revision 3.10.10.18  1996/07/20  03:19:06  fred
 * CSCdi63607:  add access list to traffic shaping per customer request
 *              requires change of parameters on function called by
 *              fastsend routines in drivers
 * Branch: California_branch
 *
 * Revision 3.10.10.17  1996/07/17  01:44:49  fsunaval
 * CSCdi59824:  RIF classification done on the VIP for the 1F.
 * Branch: California_branch
 * 1.Off-load RIF classification from the RSP to the VIP for the 1F.
 *   What's next, fragmentation?
 *
 * Revision 3.10.10.16  1996/07/13  11:35:28  fred
 * CSCdi62873:  NULL dereference in fair-queue routines when called by
 *         traffic shaping - make hash_mask independent of caller
 * Branch: California_branch
 *
 * Revision 3.10.10.15  1996/06/27  19:51:23  jhernand
 * CSCdi61415:  Remove PAK_CD2430_FAST flag and its usage in Modular Cobra
 * Branch: California_branch
 *
 * Revision 3.10.10.14  1996/06/18  01:43:34  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.10.10.13  1996/06/17  08:31:23  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.10.10.12  1996/06/04  05:15:23  mshannon
 * CSCdi45507:  sanity checking code required for memory corruptions
 * Branch: California_branch
 * Added initial debug sanity code for memory corruptions
 *
 * Revision 3.10.10.11  1996/05/18  01:25:22  fred
 * CSCdi57547:  Various crashes dequeueing packet buffers on RSP during
 *         congestion. Fix: move PAK_BUF_RSP_QUEUE to pak->flags so that
 *         it doesn't get into the free list.
 * Branch: California_branch
 *
 * Revision 3.10.10.10  1996/05/17  11:16:00  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.10.2.4  1996/05/05  23:05:28  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.10.2.3  1996/05/02  22:47:29  kchiu
 * CSCdi56517:  Image build error for c1005 images in IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.10.2.2  1996/04/15  00:08:56  kchiu
 * Branch: IbuMod_Calif_branch
 * Add dlsw+/fddi support
 *
 * Revision 3.10.2.1  1996/04/03  14:25:50  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.10.10.9  1996/05/04  00:42:26  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Rip out MTR transparent bridging support.
 * - New interafce type: BVI(Bridge-group Virtual Interface).
 * - New fast-switching flavor: FS_BVI.
 * - Introduce idb->tbridge_bridge_linktypes, idb->user_configured_mtu and
 *   pak->irb_rcvd_input.
 *
 * Constrained Multicast Flooding
 * - Declare tbifd_type.
 * - Define the CMF parser link points.
 * - Declare the return_one stub function.
 * - Declare the Transparent Bridging registry.
 * - Add tbridge_cmf to the fastswitch registry.
 *
 * Name Access List
 *
 * Revision 3.10.10.8  1996/04/27  05:47:05  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.10.10.7  1996/04/25  23:11:12  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.7.14.3  1996/04/26  00:50:40  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.7.14.2  1996/04/17  01:32:17  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.7.14.1  1996/01/16  15:32:27  dblair
 * CSCdi42375:  Move Multilink PPP into PPP subsystem
 * Branch: Dial1_branch
 *
 * Revision 3.10.10.6  1996/04/19  15:15:27  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 2.2.6.8  1996/04/09  20:20:00  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.2.6.7  1996/03/19  07:54:53  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 2.2.6.6  1996/02/21  03:16:50  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.2.6.5  1996/02/13  19:17:19  carrel
 * Branch: IosSec_branch
 * Crypto Map checkin (finally)
 *
 * Revision 2.2.6.4  1995/12/17  01:34:16  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.10.10.5  1996/04/17  13:37:36  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.10.10.4  1996/04/04  09:36:54  smackie
 * Un-obfuscate buffer copy code for prolonged simplicity. Remove
 * misguided internal particle structure from paktype. Add a discrete
 * pak_coalesce() call for particle re-assembly. (CSCdi51223)
 * Branch: California_branch
 *
 * Revision 3.10.10.3  1996/03/23  01:30:19  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.10.10.2  1996/03/21  22:38:04  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.10.10.1  1996/03/18  19:37:22  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.5  1996/03/18  23:16:18  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.3.2.4  1996/02/02  06:09:47  mbeesley
 * Branch: ELC_branch
 * Mini-sync ... pull in buffer manager tweaks.
 *
 * Revision 3.3.2.3  1995/12/11  18:46:05  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.3.2.2  1995/12/05  06:34:18  mbeesley
 * Branch: ELC_branch
 * Safeguard static paktype headers from being freed.
 *
 * Revision 3.3.2.1  1995/12/04  22:17:00  mbeesley
 * Branch: ELC_branch
 * Perform mini-sync - particle refcount changes.
 * Revision 3.10  1996/02/23  20:01:58  feisal
 * CSCdi45142:  Support VLAN ISL for new 1FE port adapter
 *
 * Revision 3.9  1996/02/13  20:38:30  hampton
 * Move buffer initialization to the platform_xxx.c files to eliminate
 * platform dependencies from common code.  [CSCdi48844]
 *
 * Revision 3.8  1996/02/02  04:29:03  mbeesley
 * CSCdi48003:  Buffer manager not optimal on all boxes
 * Make pak_set_dirty() and pak_ignore_inputq() platform specific.
 *
 * Revision 3.7  1995/11/30  04:41:03  mleelani
 * CSCdi44573:  IGMP doesnt process leave messages
 * o Add support for leave messages.
 * o Create idb sub-block for IGMP; use managed timers for IGMP timers.
 *
 * Revision 3.6  1995/11/28  02:25:10  smackie
 * Move particle refcount into local memory to improve handling speed. Add
 * utility functions to allow some particle internals to be removed from
 * the drivers. (CSCdi44505)
 *
 * Revision 3.5  1995/11/26  08:08:42  mmcneali
 * CSCdi43630:  ISL VLAN Multicast changed--IOS gs7 images are DEAF
 *
 *              ISL vLAN support for the RSP platform.
 *
 * Revision 3.4  1995/11/21  20:42:41  turadek
 * CSCdi43841:  add frame relay payload compression feature
 * syncing code to 11.0
 *
 * Revision 3.3  1995/11/17  09:13:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:24:22  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:35:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.20  1995/11/08  20:58:54  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.19  1995/10/13  22:26:07  rnaderi
 * CSCdi41309:  router crashes when dlsw configured over frame-relay
 * CLSI message header was too big. Have to shift the message if necessary.
 *
 * Revision 2.18  1995/10/12  06:01:02  tli
 * CSCdi42042:  More buffer system housekeeping
 *
 * Revision 2.17  1995/10/11  12:13:07  tli
 * CSCdi41857:  show buffers variables should be unsigned
 *
 * Revision 2.16  1995/09/25  08:34:22  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.15  1995/09/21  23:58:14  dmccowan
 * CSCdi40832:  SNA XIDs may need additional encap bytes
 * add pak_dup_grow_realign()
 *
 * Revision 2.14  1995/09/20  05:05:59  smackie
 * Remove scary pointer arithmetic from LAT buffer handling. Use a direct
 * embedded pointer to the packet header instead. (CSCdi40672)
 *
 * Revision 2.13  1995/09/18  07:27:04  smackie
 * Prune buffer code into discrete and manageable sections to restore some
 * sanity and focus to buffers.c (CSCdi40529).
 *
 *   o buffers.c - buffer management code
 *   o element.c - queue element code
 *   o queue.c   - singly-linked queue support
 *
 * Revision 2.12  1995/09/13  23:48:55  smackie
 * Rework buffer pool resize logic to allow the "huge" pool to be resized
 * for a user even if buffers are outstanding. (CSCdi38912)
 *
 * Revision 2.11  1995/08/14  17:16:25  fowler
 * CSCdi36923:  Frame relay inverse-arp and priorty list ordering problem.
 *
 * Revision 2.10  1995/08/12  07:08:21  smackie
 * Repair various buffer and memory display foibles. (CSCdi37152)
 *
 *   o Unify memory dump code to one simple general purpose routine
 *   o Enhance buffer debugging by extending buffer display commands
 *   o Show all private buffer pools (so that BRI pools show up)
 *
 * Revision 2.9  1995/07/31  09:16:43  misko
 * CSCdi37919:  DDR needs to allow protocols additional control
 * Add additional controls on a per packet basis for upper layer
 * protocols.  This is to help solve CSCdi33368.  Add new flags
 * to allow the upper layers to override the dialer access lists, so
 * they can specify that a packet is interesting or uninteresting
 * regardless of what the access lists say.  Also, add a flag so that
 * the protocols can tell the dialer not to hold the packet in the holdq
 * if a call is in progress.  Lastly, have a flag to we can tell the
 * protocols if we are dropping the packet (failing encaps) because
 * a call is in progress but not connected.
 *
 * Revision 2.8  1995/07/18  04:10:01  shaker
 * CSCdi35990:  Need packet pool for fast switched packet structure
 * Add new pak_set_dirty()
 *
 * Revision 2.7  1995/07/12  23:43:54  hampton
 * Convert TCP to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37015]
 *
 * Revision 2.6  1995/07/12 07:18:13  tli
 * CSCdi36426:  Policy routing will not disable on an interface
 * - Policy routed packets were creating fastswitching cache entries.
 *   That's not till _next_ release.
 * - Add counters to record packet and byte count on a per route map entry
 *   basis.
 * - Marketing identified a catastrophic deficiency.  Implemented "set ip
 *   tos" and "set ip precedence" to cover this deficiency.
 *
 * Revision 2.5  1995/06/28  23:45:40  myeung
 * CSCdi30267:  router does a tight loop in ip_sendnet when fed a weird
 * sequence
 * - Add ability to detect, drop and log information about the looping
 *   packet
 *
 * Revision 2.4  1995/06/21  21:17:00  fred
 * CSCdi35994:  LLC2 breaks fair queuing
 *         Rewrite fair_enqueue and fair_dequeue to use pak_enqueue
 *         and related services. Augment pak_enqueue services with
 *                 pak_queue_get_head(), pak_queue_get_tail(),
 *                 pak_queue_depth(), pak_queue_max_depth(),
 *                 pak_queue_set_max_depth(), and
 *                 pak_on_data_link_queue(paktype *pak)
 *         The latter is for the IBM code, which needs a way to determine
 *         whether a message is currently on a data link secondary queue
 *
 * Revision 2.3  1995/06/15  06:03:01  dbath
 * CSCdi33071:  IPX fast switching, over equal cost ATM paths is broken
 * Replaced PAK_FR_CACHED flag with a more generic one, PAK_ENCAPS_ONLY,
 * so that we could also use it in ATM-land.  Made IPX ATM fastswitch
 * cache population code cognisant of multiple equal cost paths,
 * like the frame relay case.
 *
 * Revision 2.2  1995/06/11  23:28:40  dkatz
 * CSCdi35384:  ICMP timestamp packets have bad checksums
 * Add a traffic type for malformatted ICMP packets.
 *
 * Revision 2.1  1995/06/07  20:36:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __PACKET_H__
#define __PACKET_H__

#include "linktype.h"
#include "pak_api.h"

/*
 * PAKMACRO defines those fields common to all packet buffers.
 * It MUST BE LONGWORD ALIGNED! SMDS encapsulation will
 * break otherwise.
 *
 * ENCAPBYTES is currently determined by Token Ring
 * and its stupid RIF field.  If Token Ring headers change
 * this will have to be changed.
 * Now it is determined by Apollo domain and its 30 bytes of "private"
 * data in between the (APRA style) ethernet encapsulation and the
 * beginning of the "domain packet".  This private data is not present
 * in the token ring encapsulation, so it really is part of the encaps...
 *
 * Now it is determined by FDDI (snap encapsulation) and Apollo domain.
 *
 * Nope, now it is determined by:
 *     Trhead+RIFMax+SNAP+IP+UDP+VringHeader
 *      14     18     8   20  8   16            = 84
 */

#define ENCAPBYTES 84		/* multiple of four for long alignment */
#define ENCAPWORDS (ENCAPBYTES/2)

/*
 * The following packet enc_flags value means that this packet failed 
 * encapsulation. At present, this will only be used by packets in the 
 * LINK_APPLETALK path for packets we will hold pointers to in the ARP
 * table pending an AARP reply.
 */
#define ENC_NOARPENTRY	1

/*
 * The following flag will be set in 'enc_flags' if an entry has already
 * failed encapsulation once; prevents endless attempts at encapsulation.
 */
#define ENC_FAILED_ONCE 2
/*
 * enc_flags to indicate that the source MAC address to be used at encap
 * time is contained at pak->data_area.
 */
#define ENC_SRCMAC_OVERRIDE 0x4

/*
 * Override the fddi enc. if it dlsw/fddi.
 */
#define ENC_DLSW 0x08

/*
 * X.25 flags
 */
#define PAK_X25_QBIT 1
#define PAK_X25_DBIT 2
#define PAK_X25_ENC 4
/*
 * Frame relay flags
 */
#define PAK_FR_FECN      1
#define PAK_FR_BECN      2
#define PAK_FR_DE        4
#define PAK_FR_IETF      8
#define DEFERED_LATE_Q_COMPRESSION 0x10
#define PAK_FR_BROADCAST           0x20
/*
 * dial on demand flags
 */
#define PAK_DDR_INTERESTING	0x0001	/* packet is always interesting */
#define PAK_DDR_UNINTERESTING	0x0002	/* packet is always uninteresting */
#define PAK_DDR_CALL_PENDING	0x0004	/* packet failed because call pending */
#define PAK_DDR_NO_HOLDQ	0x0008	/* always drop this packet */

/*
 * VPN flag
 */
#define PAK_VPN_USED 0x40

typedef ulong buffer_flags_t;
typedef uint flags_t;

typedef struct {
        uchar major;
        uchar minor;
} protocol_event_t;

#define set_major_protocol_event(pak,event) pak->protocol_event.major=event
#define set_minor_protocol_event(pak,event) pak->protocol_event.minor=event
#define get_major_protocol_event(pak) pak->protocol_event.major
#define get_minor_protocol_event(pak) pak->protocol_event.minor

/*
 * pakdata - the header for every data block
 */
typedef struct pakdata_ {
    ulong         magic;
    uchar         data_area[0];
} pakdata;

/*
 * particletype - Scattered datagram support
 */
 
struct particletype_ {
    struct particletype_ *next;           /* pointer to next particle */
    struct particletype_ *original;       /* pointer to original particle */
    pooltype             *pool;           /* pool pointer for this particle */
    paktype              *pak;            /* pointer to fixed buffer */
    pakdata              *data_block;     /* pointer to pakdata block */
    uchar                *data_start;     /* start of data in particle */
    int                   refcount;       /* Reference count (original only) */
    ushort                data_bytes;     /* bytes in particle */
    ushort                flags;          /* flags for this particle */
};

/*
 *  To keep up the routing speed, please keep the long words
 *  allocated on long word boundaries
 */

struct paktype_ {

    /*
     * These fields before the fast switching variables were moved here
     * so that there are exactly 32 bytes in the packet structure before
     * the start of the fields touched by fast switching. This is the
     * data cache line size on the Sierra and on the RSP systems. Please
     * add any new variables which are used for fast switching in the end
     * of the fast switching section. Please add any new variables which
     * are not used by generic fast switching code after the end of the
     * fast switching variable section.
     */
    struct paktype_ *next;

    uchar *transport_start;		/* ptr to transport layer header */ 
    uchar *session_start;		/* ptr to session layer header */ 
    uchar *presentation_start;		/* ptr to presentation layer header */ 
    uchar *application_start;	        /* ptr to application layer header */ 

    ulong mci_status;                   /* mci status for fddi */ 
    ushort oqnumber;		        /* output interface queue number */
    ushort linktype;		        /* type of protocol packet holds. */ 
    short refcount;			/* count of users of packet */ 
    ushort fr_flags;			/* Frame relay information flags */ 

    /*
     * Fields used in fast switching for most protocols and encapsulations.
     * We want these fields allocated beginning on a 32 byte cache line
     * boundary for Sierra, and for the RSP products.
     * We also want these fields allocated as close together as possible
     * to minimize the number of data cache lines that we touch while fast
     * switching:
     */
    uchar	*data_area;		/* Start of actual frame data area */ 
    uchar	*datagramstart;		/* pointer to start of raw data */ 
    uchar	*mac_start;	        /* pointer to MAC header */ 
    uchar	*addr_start;	        /* pointer to MAC addresses */ 
    uchar	*info_start;	        /* ptr to LLC header (if present) */ 
    uchar	*network_start;		/* pointer to network layer header */ 
    uchar	*buffer_header;		/* saved pointer to buffer header */
    idbtype	*if_input;		/* pointer to input interface */ 

    flags_t	flags;			/* flags describing data in buffer */ 

    ushort	datagramsize;		/* bytes of raw data */ 
    ushort	rxtype;			/* packet type classification */ 
    ushort	enctype;		/* encapsulation type */ 
    ushort	encsize;		/* size of frame encapsulation */ 
    short	acct_proto;		/* protocol type for accounting */ 
    ushort	riflen;			/* for fast switching */ 
    uchar	*rif_start;		/* ptr to RIF (if present) */ 
    uchar       *rif_match;             /* ptr to rif ring/bridge/ring match */
    int         fcs_type;		/* check sum type, for bridging */ 
    int		cryptoflags;		/* crypto information flags */      
    short	crypto_enc_conn_id;	/* encryption connection id */ 
    short 	crypto_dec_conn_id;	/* decryption connection id */ 


    queuetype    particlequeue;         /* Queue of particles */

    /*
     * End of fields touched by generic fast switching:
     */

    ulong       rif_status;  /* VIP interfaces to pass the RIF status */
    vc_info_t	*atm_vc;		/* ATM VC struct for input packet */
    idbtype	*if_output;		/* pointer to output interface */ 
    buffer_flags_t buffer_flags;	/* flags that describe the buffer */ 
    pooltype	*pool;			/* back ptr to mother buffer pool */ 

    ulong        caller_pc;             /* PC of caller */
    ipaddrtype	desthost;		/* next hop destination */ 
    ulong	length;			/* used by TCP */ 
    uchar	*dataptr;		/* used by TCP */ 
    lnxtype	*lnx_cb;		/* control block (used by LNX) */ 
    lcitype	*x25_cb;		/* control block (used by X.25) */ 
    void	*bridgeptr;		/* bridging info (used by X.25) */ 
    void	*cacheptr;		/* used by routing cache support */ 
    ulong	unspecified[2];		/* miscellaneous (used by IP/TCP) */ 
    sys_timestamp gp_timer;		/* general purpose timer/timestamp */
    sys_timestamp inputtime;	        /* time when packet entered system */
    sys_timestamp outputtime;	        /* time when packet left system */
    ushort	enc_flags;		/* encapsulation flags */ 
    ushort	slip_length;		/* back to long word alignment */ 
    

    /* Used for MultiBus token ring packet frames in transparent bridging */
    void  *rx_descriptor;	/* all-purpose link layer receive context */

    /*
     * The following fields are storage for values which are modified 
     * after a packet is sent to the "virtual" world for processing 
     * save these values so re-transmit routines (in llc2) can restore 
     * prior to re-submission to virtual world... 
     * If the virtual ring driver changes, to add more fields or 
     * modify more fields, these fields will need to be added here... 
     */     
    idbtype *vidb_input;             
    idbtype *vidb_output;	         
    ulong    vidb_flags;             
    ulong    vidb_datagramsize;	 
    uchar   *vidb_datagramstart;	 
    ushort   vidb_linktype;		 
    ushort   vidb_enctype;		 
    /*  ...end of temp storage area for virtual idb modified fields...  
     */ 

    /*
     * For IRB(Integrated Routing and Bridging) to record the original,
     * received bridged interface before we overwrite if_input to the
     * BVI(Bridge-group Virtual Interface).
     */
    idbtype *irb_rcvd_input;

    ulong snapshot_flags;		/* flags used by snapshot routing */
    hwidbtype *dialer_input;		/* HWIDB packet really came in on */

    uchar *clns_nexthopaddr;	        /* clns next hop nsap */ 
    uchar *clns_dstaddr;		/* clns dst address */ 
    uchar *clns_srcaddr;		/* clns src address */ 
    uchar *clns_segpart;		/* clns segmentation, null if none */
    uchar *clns_optpart;		/* clns options, null if none */ 
    uchar *clns_qos;		        /* clns QOS option */ 
    uchar *clns_datapart;		/* clns start of ulp data */ 
    ulong clns_flags;		        /* clns flags, defined in clns.h */ 
    ulong mnp4_checksum;		/*checksum for mnp4 */
    uchar line_input;			/* packet has no if_input, it's from
						a tty */
    ulong atalk_srcfqa;		        /* atalk src fq address */ 
    ulong atalk_dstfqa;		        /* atalk dst fq address */ 
    ulong atalk_dstmcast;		/* atalk dst multicast type */ 
    ushort atalk_datalen;		/* atalk data len (w/o DDP hdr)*/ 

    ulong smrp_mcaddr;		        /* smrp multicast address */

    nidbtype *nif_input;		/* novell network descriptor */ 
    nidbtype *nif_output;		/* novell network descriptor */ 
    uchar classification;		/* definative sec. classification */ 
    uchar authority;		        /* definative sec. authority */ 

    /* LAT structure definitions - must match latsys.h */ 
    /* Don't add any padding in here! */ 
    void *lat_of_link;			/* Link on transmit queue */ 
    void *lat_of_i_o;			/* Link for internal I/O */ 
    uchar *lat_of_data;		        /* Pointer to data */ 
    int lat_of_size;		        /* Size of data and flags */ 
    ushort lat_of_dst[3];		/* LAT destination address */ 
    idbtype *lat_of_idb;		/* Destination interface */ 
    paktype *lat_of_pak;		/* Output packet */ 
    /* End of LAT structure that must match*/ 

    protocol_event_t protocol_event;

    uchar *lat_groupmask;		/* Pointer to LAT group mask */ 
    llctype *llc2_cb;		        /* Control block used by llc2 */ 
    ushort llc2_sapoffset;		/* Offset to sap codes */ 
    ushort llc2_enctype;		/* Encapsulation type */ 
    uchar llc2_sap;			/* LLC sap */ 
    uchar lack_opcode;		        /* Local ACK opcode */ 
    void *peer_ptr;			/* pointer to corresponding peer */ 
    ulong sscop_nps;                    /* sscop */
    ulong ip_flags;			/* Flags for IP (option processing) */
    uchar tcp_flags;		        /* Various flags for tcp */
    uchar tcpdriver_flags;		/* Various flags for tcpdriver */
    ushort tunnelled_type;		/* back to long word alignment */ 
    ushort dialer_save_type;		/* prot type for pak in holdq */
    long dialer_save_address;		/* prot addr for pak in holdq */
    idbtype *dialer_output;		/* IDB to transmit packet on */
    ulong dialer_flags;			/* DDR information flags */
    ulong x25_flags;			/* X.25 information flags */
    traffic_shape_t *traffic_shape;	/* pointer to rate limits */
    ulong fair_sequence;		/* WFQ message sequence number */
    ushort fair_weight;			/* weight applied */
    ushort fair_conversation;		/* conversation number */
    ushort fq_hash_mask;		/* hash mask for hashed classifiers */
    ushort shape_acl;			/* access list that matched shaping */
    ushort fr_dlci;                     /* FR incoming DLCI */ 
    ushort vcd;				/* Used by ATM clients */
    ushort atm_vcid;			/* ATM VC index */
    ushort atm_enctype;			/* ATM VC encapsulation type */

    /* 
     * Multilink PPP definitions implement buffer chaining with paktype
     *  structure
     */
    paktype *mlp_next_frag;             /* points to next multilink fragment */
    paktype *mlp_prior_frag;
    ushort mlp_frag_size;
    /* End of multilink PPP definitions */
};

typedef struct paktype_offset_ {
    uint datagramoffset;
    uint mac_offset;
    uint addr_offset;
    uint rif_offset;
    uint info_offset;
    uint network_offset;
    uint transport_offset;
    uint session_offset;
    uint presentation_offset;
    uint application_offset;
} paktype_offset;

/*
 * pak_delta_adjust_start
 *
 * Adjust packet, to add or remove a layer of encapsulation.
 *  This is recommended procedure to make life saner when scatter/gather
 * makes its appearance.
 */

static inline void pak_delta_adjust_start(paktype *pak, int delta)
{
    if (pak != NULL) {
	pak->datagramstart += delta;
	pak->datagramsize  -= delta;
    }
    return;
}




/*
 * Packet size definitions
 */

/* These sizes MUST BE LONGWORD ALIGNED! */
# define DEF_HUGEDATA 18024		/* huge buffer size by default */
# define HUGEDATA pak_pool_maximum_size() /* current huge buffer size */
# define LARGEDATA 5024			/* large buffer size */
# define VERYBIGDATA 4520               /* very big buffer size */
# define BIGDATA 1524			/* big buffer size */
# define MEDDATA 600			/* medium size buffers */
# define SMALLDATA 104			/* minimum buffer size */

# define MAXDATAGRAMSIZE HUGEDATA	/* largest datagram we can allocate */

/*
 * Number of packets a single process can handle before it must
 * reschedule.  Allows for fairness among protocol stacks.
 */

#define PAK_SWITCHCOUNT  20


/*
 * Queueing element definitions
 */

#define MINELEMENTS 100

typedef struct elementtype_ {
    struct elementtype_ *next;
    paktype *pakptr;
} elementtype;

/*
 * Flags defined for 'buffer_flags' in a datagram header.
 * These flags describe the buffer itself, not what is
 * contained in it. Sometimes called 'permanent flags':
 *
 * The PAK_BUF_ACCT_DONE flag signals to tx_start code to NOT increment the
 * interface's tx_cumbytes and outputs counters.  This allows us to correctly
 * distinguish process vs. fastswitch even when fastswitched packets are
 * copied to the hold queue due to transmit congestion via the backing-store
 * option.
 */
# define PAK_BUF_PERMANENT (buffer_flags_t)0x00000001 /* permanent buffer */
# define PAK_BUF_IGNOREQ   (buffer_flags_t)0x00000002 /* no input q acctng */
# define PAK_BUF_USE_DIRTY (buffer_flags_t)0x00000004 /* use pak dirty bit */
# define PAK_BUF_DIRTY     (buffer_flags_t)0x00000008 /* pak dirty in cache */
# define PAK_BUF_ACCT_DONE (buffer_flags_t)0x00000010 /* f/s stats done */
# define PAK_BUF_DO_NOT_Q  (buffer_flags_t)0x00000020 /* do not Q to process */




/*
 * Flags defined for 'flags' in a datagram header.
 * These flags describe the data in the packet buffer,
 * not the buffer itself. Sometimes called 'temporary flags':
 */

# define PAK_BROADCAST   (flags_t)0x00000001 /* input dgram is broadcast */
# define PAK_NOREDIRECT  (flags_t)0x00000002 /* don't send redirects */
# define PAK_ALIAS	 (flags_t)0x00000004 /* sent to an IP alias of ours */
# define PAK_DLQ	 (flags_t)0x00000008 /* packet on datalink queue */
# define PAK_PRIORITY	 (flags_t)0x00000010 /* place pkt at head of queue */
# define PAK_CONTROL	 (flags_t)0x00000020 /* packet is a control packet */
# define PAK_HEARSELF	 (flags_t)0x00000040 /* enable hdw hearself when TX */
# define PAK_MCICHECKED	 (flags_t)0x00000080 /* pkt has been checked by MCI */
# define PAK_ABORT	 (flags_t)0x00000100 /* packet has been aborted */
# define PAK_INPUTQ	 (flags_t)0x00000200 /* pkt counted on if input queue*/
# define PAK_MBIT	 (flags_t)0x00000400 /* has/should set M bit in X.25 */
# define PAK_DO_ISL_CRC	 (flags_t)0x00000800 /* Indicates ISL vLAN requiring
						inner CRC generation */
# define PAK_IS_ISL    	 (flags_t)0x00000800 /* Indicates ISL packet */
# define PAK_SENDSELF	 (flags_t)0x00001000 /* packet has been looped once */
# define PAK_MULTICAST   (flags_t)0x00002000 /* is a multicast packet for us */
# define PAK_PRIORITIZED (flags_t)0x00004000 /* packet already prioritized */
# define PAK_XMITBCAST	 (flags_t)0x00008000 /* Xmit as physical broadcast */

# define PAK_RSP_QUEUE	 (flags_t)0x00010000 /* buffer used by RSP */
# define PAK_MFS_SWITCHED (flags_t)0x00080000 /* Semi fs for mcast (any prot)*/
# define PAK_IPXCOMPRESS (flags_t)0x00020000 /* Packet is IPX compressed */
# define PAK_SHAPED      (flags_t)0x00100000 /* traffic shaping */
# define PAK_RSRB_EXPLORER (flags_t)0x00200000 /* tr-rsrb - rsrb explorer input */
# define PAK_SRB_IN	 (flags_t)0x00400000 /* trsrb - bridge input */
# define PAK_SRB_OUT	 (flags_t)0x00800000 /* trsrb - outgoing - normal */
# define PAK_SRB_EXPLORER (flags_t)0x01000000 /* trsrb - explorer packet */
# define PAK_TBRIDGE_IN  (flags_t)0x02000000 /* Packet can in using transit */
					     /* bridge encapsulation */
# define PAK_LACK_IN     (flags_t)0x04000000 /* Local ACK - input to LLC2 */
# define PAK_LACK_OUT    (flags_t)0x08000000 /* Local ACK - output from LLC2 */
# define PAK_SSCOP_RTS   (flags_t)0x10000000 /* SSCOP flag */
# define PAK_ENCAPS_ONLY (flags_t)0x20000000 /* generate encaps, don't cache */
# define PAK_SRB_EXP_FAST (flags_t)0x40000000 /* explorers fastswitched */
# define PAK_SRB_EXP_SENT (flags_t)0x80000000 /* explorers already sent */

/*
 * Macros for manipulating various parts of the packet header
 */

/* pak_lock - place a lock on a packet. Free lock with datagram_done() */
#define pak_lock(pak) ((pak)->refcount++)
#define pak_unlock(pak) ((pak)->refcount--)

/* pak_shared - TRUE if a packet is shared by other processes */

#define pak_shared(pak) (pak->refcount > 1)


/*
 * platform_pak_set_dirty_inline
 *
 * Set PAK_BUF_DIRTY if PAK_BUF_USE_DIRTY is set. This tells the device
 * driver to flush the packet out of the data cache before giving the
 * buffer to a DMA device. It also prevents us from using stale data out
 * of the data cache when we next read this buffer. It only needs to be
 * done for packet buffers allocated in cached IO memory. For such buffer
 * PAK_BUF_USE_DIRTY must be set.
 *
 * Note : Each platforms defines pak_set_dirty() to either nothing
 * or to platform_pak_set_dirty_inline(). No direct calls should ever
 * be made to the private inline. The platform specific pak_set_dirty()
 * should always be used.
 */
static inline void platform_pak_set_dirty_inline (paktype *pak)
{
    if (pak->buffer_flags & PAK_BUF_USE_DIRTY)
        pak->buffer_flags |= PAK_BUF_DIRTY;
}

/*
 * pak_center
 *
 * Lets have a big hand for our old friend, The Line.
 */

#define pak_center(pak) ((pak)->data_area + ENCAPBYTES)

/*
 * Datagram alignment macros
 *
 * If ALIGN_DATAGRAM is defined (usually in the defs_*.h file
 * for the platform), then the command inside the macro is compiled
 * into the code. Else, it gets left in the dust...
 */

#ifdef ALIGN_DATAGRAM
#define ALIGN_DATAGRAM_CMD(x) x
#else
#define ALIGN_DATAGRAM_CMD(x)
#endif

/*
 * masks to get the correct memory address alignments
 */
#define BUFFER_WORD_ALIGN     0xfffffffe
#define BUFFER_LONGWORD_ALIGN 0xfffffffc

/*
 * Externs (from os/element.c)
 */

extern int elements;			/* count of free elements */
extern uint elementsmissed;		/* count of failures on get */
extern uint elementsmade;		/* count of new buffers made */

/*
 * Externs (from os/buffers.c)
 */

extern pooltype *fs_particle_pool;
extern pooltype *small;
extern pooltype *middle;
extern pooltype *big;
extern pooltype *verybig;
extern pooltype *large;
extern pooltype *huge;
extern pooltype *headerpool;
extern pooltype *fspak_pool;

/*
 * Prototypes (from os/buffers.c)
 */

extern pooltype *pak_pool_create(char *name, int group, int size,
				     uint flags, mempool *mempool);
extern boolean pak_pool_create_cache(pooltype *pool, int max_size);
extern void pak_pool_init(void);
extern void pak_pool_periodic(void);
extern paktype *pool_getbuffer(pooltype *pool);
extern paktype *input_getbuffer(int size, hwidbtype *idb);
extern pooltype *pak_pool_find_by_name(char *name);
extern pooltype *pak_pool_find_by_size(int size);
extern int pak_pool_maximum_size(void);
extern boolean pak_realign(paktype *, uchar *);
extern paktype *pak_coalesce(paktype *pak, paktype *newpak);
extern paktype *pak_clone(paktype *pak, paktype *newpak);
extern void pak_copy(paktype *pak, paktype *newpak, int size);
extern void pak_copy_and_recenter(paktype *pak, paktype *newpak, int size);
extern paktype *pak_duplicate(paktype *pak);
extern paktype *pak_clip(paktype *pak, int copy_size);
extern void pakqueue_resize(queuetype *queue, int newqsize);
extern paktype *pak_grow(paktype *pak, int oldsize, int size);
extern paktype *pak_dup_grow_realign(paktype *pak, int copy_size, int hdrbytes);
extern paktype *pak_grow_realign(paktype *pak, int copy_size, int hdrbytes);
extern void clear_if_input(paktype *ptr);
extern void set_if_input(paktype *ptr, idbtype *idb);
extern boolean change_if_input(paktype *ptr, idbtype *new_input);
extern void retbuffer(paktype *ptr);
extern void discard_buffer(paktype *pak);
extern void show_pool(pooltype *pool);
extern void element_init(void);
extern void grow_element(int count);
extern boolean elementBLOCK(void);
extern void element_grow_request(void);
extern void *pak_get_element(paktype *);
extern paktype *pak_enqueue(queuetype *queue, paktype *pak);
extern paktype *pak_enqueue_reserve(queuetype *queue, paktype *pak, int reserve);
extern paktype *pak_insqueue(queuetype *queue, paktype *pak, elementtype *after);
extern paktype *pak_dequeue(queuetype *queue);
extern paktype *pak_requeue(queuetype *queue, paktype *pak);
extern paktype *pak_unqueue(queuetype *queue, paktype *pak);
extern void element_dequeue(queuetype *queue, elementtype *element);
extern void *data_enqueue(queuetype *queue, void *data);
extern void *data_dequeue(queuetype *queue);
extern void data_walklist(queuetype *queue, void *rtn);
extern boolean data_insertlist(queuetype *queue, void *newitem, void *insert_in_front);
extern int element_num_elements(void);
extern void buffers_command(parseinfo *csb);

extern void create_queue_registry(void);

extern boolean pak_encap(hwidbtype *hwidb, paktype *pak, int length,
			 long address);

/*
 * inlines for quick access to pak queues
 */

inline static paktype *
pak_queue_get_head (queuetype *queue) {
    elementtype *element = (elementtype *)queue->qhead;
    return ((element) ? element->pakptr : (paktype *)NULL);
}
 
inline static paktype *
pak_queue_get_tail (queuetype *queue) {
    elementtype *element = (elementtype *)queue->qtail;
    return ((element) ? element->pakptr : (paktype *)NULL);
}
 
inline static ulong 
pak_queue_depth (queuetype *queue) {
    return queue->count;
}
 
inline static ulong 
pak_queue_max_depth (queuetype *queue) {
    return queue->maximum;
}
 
inline static void 
pak_queue_set_max_depth (queuetype *queue, ulong maximum) {
    queue->maximum = maximum;
}
/*
 * returns true if pak is in the secondary queue maintained by
 * holdq_en/dequeue, priority_en/dequeue, custom_dequeue, or fair_en/dequeue
 *
 * note that this does not include hardware-specific queues
 */
inline static boolean
pak_on_data_link_queue (paktype *pak) {
    return(pak->flags & PAK_DLQ? TRUE: FALSE);
}
#endif __PACKET_H__

