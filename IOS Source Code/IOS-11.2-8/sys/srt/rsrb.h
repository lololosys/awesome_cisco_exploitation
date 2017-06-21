/* $Id: rsrb.h,v 3.11.4.8 1996/08/26 15:12:35 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/srt/rsrb.h,v $
 *------------------------------------------------------------------
 * rsrb.h --Definitions for multiport and remote source route bridge
 *
 * September 1989, Eric B. Decker
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsrb.h,v $
 * Revision 3.11.4.8  1996/08/26  15:12:35  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.11.4.7  1996/08/07  09:02:39  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.11.4.6  1996/08/03  23:37:50  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.11.4.5  1996/06/27  15:14:40  ppearce
 * CSCdi58693:  Numerous crashes in vrforward_pak()
 * Branch: California_branch
 *   Ensure TRLANE def's are unique in RSRB_ET_xx and RSRB_BN_xx bitmaps
 *
 * Revision 3.11.4.4  1996/05/17  12:13:40  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.9.2.3  1996/04/07  05:57:30  schiang
 * CSCdi50513:  RSRB - Dead NCIA peers cause duplicate ring entries
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.9.2.2  1996/04/03  21:59:16  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.9.2.1  1996/03/17  18:49:52  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.11.4.3  1996/05/09  14:46:41  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.11.4.2.14.1  1996/04/27  07:26:24  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.11.4.2.4.1  1996/04/08  14:49:26  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.11.4.2  1996/03/28  16:47:33  psandova
 * CSCdi47456:  All srb explorers being flushed on one tok interface
 * Branch: California_branch
 *
 * Make the flush timer interface specific.  Also, count flush
 * data rate after packet has passed all filtering checks.
 *
 * Revision 3.11.4.1  1996/03/23  22:49:19  ppearce
 * SR/TLB fastswitching feature
 * Branch: California_branch
 *
 * Revision 3.11  1996/03/11  17:24:50  ppearce
 * CSCdi47301:  CLS/RSRB link activation may fail when LAK enabled
 *   Cannot depend on pak->peer_ptr to represent a valid peer entry
 *     use a different method to find valid peer entry
 *
 * Revision 3.10  1996/03/11  14:05:53  rbatz
 * CSCdi45116:  Duplicate All-Routes Explorers unnecessarily
 *              forwarded in redundant topologies.
 *
 * Revision 3.9  1996/02/21  02:43:53  hampton
 * Clean up platform specific SRB defines in commonly included header
 * files.  [CSCdi48844]
 *
 * Revision 3.8  1996/02/20  17:50:50  ioakley
 * CSCdi40888: Prevent potential peer startq related buffer/memory leaks
 *             and establish a default limit for the peer startq size.
 *
 * Revision 3.7  1996/02/08  18:02:10  ahh
 * CSCdi48380:  TCP: TCP Driver close callback needs more context
 * Have some t with your cookie.
 *
 * Revision 3.6  1995/12/15  22:43:21  rbatz
 * CSCdi43378:  Explorers destined for SDLLC Virtual Ring forwarded to
 *              wrong ring.  Rework of vr_virtualp required.
 *
 * Revision 3.5  1995/12/13  04:24:11  ahh
 * CSCdi44230:  Remove dead code
 * Bring out your dead... *clang* (RSRB/TCP fast-switching)
 *
 * Revision 3.4  1995/12/11  18:40:06  psandova
 * CSCdi43608:  source-bridge explorer-maxrate default too high
 *
 * Set explorer fastswitch maxrate default to 38400 Bytes/sec.
 *
 * Revision 3.3  1995/11/17  18:55:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:42:15  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:31:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:30:24  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/08/25  11:45:22  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.5  1995/08/07 05:44:02  richl
 * CSCdi38315:  makefile rework
 *              header changes for rif_util.c and tring_inline.h
 *              move dlsw_work to its own file, create libibm_util.a for
 *              utility routines used by dlsw and rsrb
 *
 * Revision 2.4  1995/08/01  09:22:37  ogrady
 * CSCdi32342:  update TR_MAXPORTS.
 * Remove constant TR_MAXPORTS.  Determine max number of possible SRB
 * ports dynamically.
 *
 * Revision 2.3  1995/07/06  01:33:40  hampton
 * Eliminate magic timestamps from SDLC and STUN code by converting them
 * to use the passive timer macros.  [CSCdi36750]
 *
 * Revision 2.2  1995/06/21 03:29:41  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.1  1995/06/07  23:01:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * All Remote SRB packets have a tr_vhdrtype on the front of any bridged data.
 * Exact format is as described by the code.  Data depends on packet type.  In
 * most cases it will be raw bridge data but in the case of peer peer
 * information exchange the data can be other things if any.
 *
 * FORWARD         packet contents is raw bridge data.  If the packet is
 *                 larger than what the media can handle it is broken into
 *                 pieces.  These pieces are reassembled by the receiving
 *                 server.
 *
 *                 When the entire packet has reached the server it will be
 *                 kicked out on the target ring using PAK_SRB_OUT which will
 *                 pass the CRC and SRC addr buried in the packet.
 *
 * EXPLORER        packet contents is an explorer that is properly formated.
 *                 It will have already been modified to reflect that it came
 *                 across the virtual ring.  'offset' will point at the route
 *                 descriptor where the outgoing bridge number needs to be
 *                 munged in.  It is guaranteed that there is sufficient room
 *                 for the outgoing ring and the zero'd bridge number.  This
 *                 offset is relative to the start of the explorer packet.
 *
 * RINGXREQ        The data portion contains ring numbers currently owned and
 *                 enabled for bridging in this vring.  Each ring number is
 *                 a 16 bit field.  The list is terminated by -1.  A receiptent
 *                 should reply with a RINGXREPLY that will contain the rings
 *                 that are owned by the other side.  RINGXREQ is used to force
 *                 a response from the other side.
 *
 * RINGXREPLY      same format as above.  It is the required response to a
 *                 RINGXREQ.
 *
 * VERSIONREQ      A request for a version number. A version request always
 *                 contains the version number of the sender, and is always
 *                 accepted by the receiver, independent of receiver version
 *                 number.
 *
 * VERSIONREP      same format and caveats as above. It is the required
 *                 response to a VERSIONREQ.
 *
 * UNKVRN          The ring group in the original packet is not recognized at
 *                 this peer.  The header packet with this op contains the
 *                 bad vrn.
 *
 * UNKPEER         The source of the original packet is not recognized at this
 *                 peer.
 *
 * IGNORE          used internally to denote that the data portion should be
 *                 ignored.  This is used when we don't recognize something
 *                 and are trying to stay in sync.
 *
 * EMPTY           used internally to denote that there is no data portion.
 *                 This denotes that the next data piece should be a hdr.
 */

#ifndef __RSRB_H__
#define __RSRB_H__

#include "../if/tring.h"
#include "../h/packet.h"
#include "../ip/ip.h"

#define RSRB_INVALID_ADDRESS (void *) 0xFFFFFFFF

#define MAXDGRAM_RSRB    2104	/* bytes, for buffer size checking */
#define FST_IP_HDR_SIZE 20
#define RSRB_DROP        2
#define RSRB_TCP_WINDOW_SIZE	10240	/* default receive window size */
#define RSRB_MAX_STARTQ  5              /* maximum size of peer startq(s) */

/* Maximum # of rings in a chassis */
#define RSRB_MAXRINGS 22

/* Bandwidth for Spanning Tree Path Cost */
#define VRING_VISIBLE_BANDWIDTH 1025	/* 1.025 Mbps, dunno why... */


/*
 * This version is compatible with one previous version of
 * protocol.
 */

#define RSRB_VERSION		3
#define RSRB_CAP_VERSION	3	/* first version that supports caps. */
#define RSRB_LACK_BC_VERSION	2
#define RSRB_PREV_VERSION	2
#define RSRB_MIN_VERSION	2
#define RSRB_NO_VERSION		0

#define RSRB_OUR_CAPS		(0)
/*
 * Various symbolic parameters to different functions.
 */

/*
 * vrtcpd_gethdr
 */
#define RSRB_CHECK_VER		TRUE
#define RSRB_DONT_CHECK_VER	FALSE

#define RSRB_RIFLF_KNOCKDOWN    TRUE
#define RSRB_RIFLF_LEAVEALONE   FALSE

/*
 * vrsend_version
 */

#define ABORT_ON_FAIL		TRUE
#define DONT_ABORT_ON_FAIL	FALSE

typedef struct _tr_vhdrtype {
    ushort rhdr_version;		/* version and op must be the same */
    ushort op;				/* for all RSRB versions */
    ushort trn;				/* target ring number */
    ushort vrn;				/* virtual ring number/ring group */
    ushort offset;			/* as needed pntr into pkt */
    ushort len;				/* length of this segment/packet */
    ulong  param;			/* misc. parameter */
} tr_vhdrtype;

#define TR_VHDRTYPE_SIZE	16	/* size of tr_vhdrtype */

/* version and op must be the SAME FORMAT for all RSRB versions 
   (except for fst, of course, leading to many (sizeof(tr_vhdrtype)-6)
   constructs in the code, and other hardcoded ____. */

typedef struct _tr_fst_vhdrtype {
    ushort rfst_version;
    ushort op;
    ushort trn;				/* target ring number */
    ushort vrn;				/* virtual ring number/ring group */
    ushort offset;			/* as needed pntr into pkt */
} tr_fst_vhdrtype;

#define TR_FST_VR_HDR_SIZE	10	/* size of tr_fst_vhdrtype */


/*
 * This whole list should be an typedef'ed enum
 */
 
#define RSRB_OP_NONE		0	/* your guess here */
#define RSRB_OP_FORWARD		1	/* just punch this onto target ring */
#define RSRB_OP_EXPLORER	2	/* explorer */
#define RSRB_OP_RINGXREQ	3	/* I'm showing you mine, how bout it */
#define RSRB_OP_RINGXREPLY 	4	/* Well okay, I'll show you mine */
#define RSRB_OP_NOP		5	/* do nothing, possibly force open */
#define RSRB_OP_VERSIONREQ	6	/* What is your RSRB version? */
#define RSRB_OP_VERSIONREPLY	7	/* This is my RSRB version */
#define RSRB_OP_VERSIONXREQ     8 	/* Effects of ops 3 and 6 */
#define RSRB_OP_FORWARDCRC	9	/* Forward, have remote build CRC */
#define RSRB_OP_LINKUP_REQ     10 	/* Create an LLC2 VC with dest addr */
#define RSRB_OP_LINKDOWN_REQ   11 	/* Destroy an LLC2 VC with dest addr */
#define RSRB_OP_LLC2DATA       12 	/* Pass data to LLC2 VC w/dest addr */
#define RSRB_OP_LINKUP_PRSP    13 	/* Positive response to LINKUP_REQ */
#define RSRB_OP_LINKDOWN_PRSP  14 	/* Positive response to LINKDOWN_REQ */
#define RSRB_OP_SDLLC          15       /* SDLLC frame */ 
#define RSRB_OP_FORWARDFST     16	/* just punch this onto target ring */
#define RSRB_OP_FORWARDFSTCRC  17	/* just punch this onto target ring */
#define RSRB_OP_QUENCH_ON      18       /* flow control remote host */
#define RSRB_OP_QUENCH_OFF     19       /* release flow control */

/*
 * The following are to define the new opcodes for the new and improved
 * Local Ack "lockstep" state machine.  Created by Eric and Claude...
 */
#define RSRB_OP_ConnectRequest 20	/* Ask remote LAK connect start */
#define RSRB_OP_ConnectPollReq 21	/* I'm ConnectPending, ack me please... */
#define RSRB_OP_ConnectPollAck 22	/* I saw you pending, goto Connect */
#define RSRB_OP_ConnectPollAckAndReq 23	/* I saw you pending, and do you see me? */
#define RSRB_OP_ConnectWaitAck 24      /* I'm waiting for local llc2 to open,
					  please wait I'm alive! */
#define RSRB_OP_DisconnectRequest 25   /* Please start tearing down LAK connection */
#define RSRB_OP_DisconnectAck     26   /* I saw your request and down I go */
#define RSRB_OP_CAPS_REQ	  27   /* capabilities request */
#define RSRB_OP_CAPS_REPLY	  28   /* capabilities reply */
#define RSRB_OP_LA2_QON		  29   /* flow control remote host */
#define RSRB_OP_LA2_QON_ACK       30   /* acknowledge flow control remote host */
#define RSRB_OP_LA2_QOFF          31   /* release flow control */
#define RSRB_OP_LA2_QOFF_ACK      32   /* acknowledge release of flow control */

/*
 * XID control messages.
 */

#define RSRB_OP_SEND_XID	  33	/* packet contains somekind of XID on behalf of the src addr */

/* Span BPDUs */
#define RSRB_OP_BPDU	       34       /* Carries STP BPDU */
/* 
 * This OP is used to resolve disjoint peer problem 
 * If there are any two or more peers not peered together for a given
 * ring group, and one of those dis-joint peers is a root bridge then STP
 * does not work correctly. The assumption is that all the routers 
 * participating in this STP are fully connected, but there always will be at 
 * least one customer who won't want to do it and complain. So I am going to
 * cover my bun with this.
 */
#define RSRB_OP_SPAN           35

#define RSRB_OP_QLLC		  36	/* like OP_SDLLC, but for
					   QLLC. */
#define RSRB_OP_TR_LANE	          37	 					 

/*
 * Special Internal use or Error Ops.
 */

#define RSRB_OP_IGNORE		0x7ffe	/* dump this data */
#define RSRB_OP_EMPTY		0x7fff	/* header entry in peer is empty */

/*
 * Error op codes.
 *
 * The high order bit indicates error.  Next bit down indicates non-fatal
 */

#define RSRB_OP_ERR		0x8000	/* bit that will be set for errs */
#define RSRB_OP_UNKVRN		0x8001	/* unknown ring group */
#define RSRB_OP_UNKPEER		0x8002	/* peer unknown, abort */
#define RSRB_OP_UNKTRN		0x8003	/* target ring unknown */
#define RSRB_OP_BADOP		0x8004	/* illegal opcode */
#define RSRB_OP_UNKVER		0x8005	/* incompatible remote version */

#define RSRB_OP_UNKCAP		0xC001	/* capability not supported, not fatal */

/* Bits in the "bridge number" field for RSRB version 2 (and later) */

#define RSRB_BN_MASK		0x000f  /* Bridge field in definition */
#define RSRB_BN_SDLLC		0x0010  /* Remote ring is SDLLC */
#define RSRB_BN_QLLC		0x0020  /* Remote ring is QLLC */
#define RSRB_BN_TR_LANE		0x0040  /* Remote ring is ATM - LAN Emulation */
#define RSRB_RN_DONE		0xffff	/* This ring has already been done */

#ifdef RSRB_VER_1_IS_NOW_DEAD
/* Virtual Ring Exchange items for RSRB version 1 */
typedef struct _tr_vrxtype_1_ {
    ushort bn;
    ushort rn;
} tr_vrxtype_1;
#endif

/* Virtual Ring Exchange items for RSRB version 2 (and further) */
typedef struct _tr_vrxtype_2_ {
    ushort bn;
    ushort rn;
    uchar macaddr[6];
} tr_vrxtype_2;
 

/*
 * The information that describes a particular vring's configuration is
 * contained in the group structure.  This structure points to two linked
 * lists.
 *
 * The first is a linked list of vring peers associated with this bridge for
 * this vring.  Peers are only used for remote operations.  Local vring
 * bridging is handled entirely by entries in the ring list.
 *
 * The second describes the ring numbers associated with this ring group.  It
 * is built dynamically from incoming Explorer_Replies for remote bridging.
 * For local bridging the entries are built statically from configuration
 * commands.
 *
 * It is intended that IP based peers include an entry in the peer structures
 * for itself.  This peer is never opened (what would be the point).  This
 * entry is also used to identify which of our IP addresses this peer is
 * known by.  The first peer entry that is IP and is the ip address of one
 * of the interfaces confiugured for IP is used as the peer name.  The
 * peername element is no longer used and has been deleted.
 */

struct tr_vgrouptype_ {
    tr_vgrouptype *next;
    ushort vrn;
    ushort flags;
    queuetype peers;
    queuetype rings;
    queuetype local_rings;              /* list of local rings attached to vrg */
    queuetype dead_rings;
    uchar vr_vmac[IEEEBYTES];		/* default vmac address */
    VrgT *vr_vdlc;			/* control structure for VDLC */
    ulong drops;
    int largest_frame;			/* largest frame size for vring */
    idbtype *virtual_bridge;		/* Virtual bridge to
					   transparent */
    idbtype *span_idb;			/* virtual interface used for
					   AST (SRB AST Support */
    int next_vrp_snmp_index;            /* snmp index to identify next vrp
                                           to be created */  
} ;

/* macro to traverse all the ring groups */

#define FOR_ALL_RING_GROUPS(vrg) \
    for ((vrg) = (tr_vgrouptype *) vringQ.qhead; (vrg); (vrg) = (vrg)->next)

#define VRGF_GENUPDATE	0x0001		/* generate peer update */
#define VRGF_FORCEXCHG	0x0002		/* force unopened peers open */

typedef boolean (*vre_les_fastout_type)(hwidbtype *, hwidbtype *, paktype *,
					int);
typedef boolean (*vre_hes_fastout_type)(hwidbtype *, hwidbtype *, uchar *, int,
					ulong *, int, hwidbtype *);

struct tr_ventrytype_ {
    struct tr_ventrytype_ *next;
    struct tr_ventrytype_ *tmp_next;	/* for sorting local entries for cbus*/
    ushort bn;
    ushort rn;
    ulong cbus_index;			/* rn/bn/mci_index for cbus */
    uchar macaddr[6];			/* If there is one... */
    int type;
    union { 		                /* rem: peer owner | local: idb */
	hwidbtype *idb;
	tr_vpeertype *vp;
    } outhole;
    ulong forwards;
    ulong explorers;			/* only if type local */
    hwidbtype *virtual_idb;		/* Virtual IDB for this ring */
    boolean (*virtualp)(tr_ventrytype *, uchar *);
    vre_les_fastout_type vre_les_fastout;
    vre_hes_fastout_type vre_hes_fastout;
};

/*
 *  List of local rings (SRB / FDDI)
 *  Currently only used by dup-ARE-filter stuff
 */
typedef struct _srb_local_ring_type {
    struct _srb_local_ring_type *next;
    hwidbtype *idb;
} srb_local_ring_type;


#define RSRB_ET_EITHER		0x0000	/* used only in searching */
#define RSRB_ET_LOCAL		0x0001
#define RSRB_ET_REMOTE		0x0002
#define RSRB_ET_REMSTATIC	0x0004
#define RSRB_ET_SDLLC           0x0008
#define RSRB_ET_REMSDLLC	0x0010
#define RSRB_ET_QLLC            0x0020
#define RSRB_ET_REMQLLC		0x0040
#define RSRB_ET_LOCAL_VIRTUAL	0x0080
#define RSRB_ET_TR_LANE         0x0100
#define RSRB_ET_REMTR_LANE	0x0200

/*
 * Which of the above ET types uses the macaddr component
 */
#define RSRB_ET_USESMAC		(	  \
		RSRB_ET_LOCAL 		| \
		RSRB_ET_REMOTE 		| \
		RSRB_ET_REMSTATIC 	| \
		RSRB_ET_REMSDLLC	| \
		RSRB_ET_REMQLLC         | \
                RSRB_ET_REMTR_LANE)

#define RSRB_BN_DONTCARE	((ushort) -1)

/*
 * info describing a rsrb peer.
 *
 * Peer States.
 */

typedef enum _tr_vpeer_statetype {
    RSRB_PS_DEAD,			/* temporarily shutdown */
    RSRB_PS_CLOSED,			/* no activity */
    RSRB_PS_OPENING,			/* waiting for connector */
    RSRB_PS_OPENWAIT,			/* waiting for transport open */
    RSRB_PS_REMWAIT,			/* transport open, waiting for rsp */
    RSRB_PS_REMGO,			/* remote said do it */
    RSRB_PS_REMOPENED,			/* remote opened before we did */
    RSRB_PS_DRAINING,			/* sending start queued paks */
    RSRB_PS_CONNECTED			/* fully open */
} tr_vpeer_statetype;

/*
 * During the connect window passive connections are assumed to be
 * part of the simultaneous open situation.  Outside the connect window
 * passive connections will abort any current peer connection for that
 * ring-group/peer.  This allows us to detect the other side going away
 * while still dealing with the simultaneous situation.
 */

#define RSRB_CONNECT_WINDOW	(30*ONESEC)
#define RSRB_INITIAL_START_WINDOW (3*60*ONESEC)

/*
 * RSRB_SERIAL_MAXBYTES is the number of bytes we can transport across
 * a raw serial RSRB connection.  This is very dependent on what the
 * physical interface can do.  It is based on a bridged info field of
 * 2052 bytes plus MAC header (14 bytes), plus max IBM RIF (18 bytes).
 *
 * Note we have to put a vring header on it (currently 16 bytes).
 * Basic equation is <interface MTU> - <vring header>.
 */

#define RSRB_SERIAL_MAXBYTES	2084

/*
 * Define cleanly the number of bytes needed in a packet header for FST.
 * The first is for most frames; the second, for RSRB_OP_FORWRADFST and
 * RSRB_OP_FORWARDFSTCRC
 */
#define RSRB_FST_PACKET_SIZE (TRING_ENCAPBYTES + SRB_MAXRIF + ET_SNAP_LEN + \
                              IPHEADERBYTES(NOPT) + sizeof(tr_vhdrtype))

#define RSRB_FST_FORWARD_PACKET_SIZE (TRING_ENCAPBYTES + SRB_MAXRIF + \
				      ET_SNAP_LEN + IPHEADERBYTES(NOPT) + \
				      (sizeof(tr_vhdrtype) - 6))


#define MAX_PEER_NAME 37

/*
 * sender is a routine that is used to actually send a block of data
 * to the peer for dealing with.  It is called with the following syntax.
 *
 * boolean sender(peer, hdr, src_pak)
 *     tr_vpeertype *peer;
 *     tr_vhdrtype *hdr;
 *     paktype *src_pak;
 *
 * How we talk to a peer is determined by the routine sender.  Essentially
 * there is one routine for each way of talking.  Thus there will be a routine
 * that knows how to talk IP and another that knows how to talk directly to an
 * interface.  Information in the vring_peer structure (*peer) should contain
 * any information that might be needed for the differenct encapsulation
 * styles.
 *
 * hdr is a tr_vhdrtype that has been filled in by the caller.  It is owned by
 * the caller so if need be sender iwll make its own copy.  If it is NULL
 * than the header is already at the front of the packet.  It is assumed that
 * pak->datagramstart points at it.
 *
 * src_pak is assumed to be some form of bridged packet although it may also
 * be one of the protocol packets used to control the virtual ring groups and
 * peers.  pak->datagramstart is assumed to point at the raw start of the
 * information.  Note that this packet may be used directly by the sender if
 * it is determined that this can be done by the encapsulator.  If hdr is NULL
 * this packet will also contain the transaction header.
 *
 * Force was removed as a parameter.  In the TCP sender, there is some logic
 * to manipulate the TCP queue size for critical frames such as ring xchg and
 * local-ack packets.
 *
 * "outpeer" was simply used for debugging and was always set to the vrp
 * which was passed in.  Removed this brain-dead parameter.
 *
 * The routine will return true if it has dealt with pak.
 *
 * direct is a routine that is similar in function to sender but it knows
 * nothing about the state of the connection nor about headers.  It is
 * used to directly pump data at the other side.  It is used by the
 * routines responsible for establishing the full connection.
 *
 * boolean direct(peer, pak)
 *     tr_vpeertype *peer;
 *     paktype *pak;
 *
 * It returns TRUE if it has dealt with the packet.
 *
 * abort will rip the connection down.  Essentially aborting.  It
 * does not deallocate any data structures.
 *
 * void abort(peer)
 *     tr_vpeertype *peer;
 *
 * close prevents further input but makes sure that any pending output
 * is physically transmitted.  'param' is anything that the close
 * routine might need to do its job.  For tcpdriver peers this is the
 * tcpdriver cookie which for our purposes is the vring peer pointer
 * associated with this tcpdriver stream.  If it is NULL no close
 * procedure is called.
 *
 * void close(peer, param)
 *     tr_vpeertype *peer;
 *     ulong param;
 */

typedef enum _tr_vpeer_typetype {
    RSRB_PT_NONE = 0,			/* no type, constrainted to 0 */
    RSRB_PT_TCPD,			/* TCP, tcpdriver implementation */
    RSRB_PT_IF,				/* Direct interface */
    RSRB_PT_LAN,			/* LAN interface */
    RSRB_PT_FST,			/* FST datagram service */
    RSRB_PT_FR				/* Frame Relay Direct Encapsulation */
} tr_vpeer_typetype;

typedef enum _vpeer_connrtn {		/* return code from vrp->connect */
    RSRB_CONN_ABORTED,			/* structures changed, forget it */
    RSRB_CONN_FAILED,			/* Couldn't connect */
    RSRB_CONN_CONNECTED
} vpeer_connrtn;

/*
 * This is for sequence numbering of frames. There are two ranges to allow
 * for non-collision between interrupt level and process levels
 *
 * Sequence number algorithm is a modified 'lollipop.' It works, so let's
 * use it.
 *
 * The first RSRB_SEQNUM_RESTART_WINDOW sequence numbers are reserved for
 * startup. A peer is assumed to have restarted its sequence when its
 * sequence numbers are within the startup range. A machine always sets its
 * current sequence number to one of these, unless it itself is reading within
 * the restart window.
 * 
 * The RSRB_SEQNUM_OUT_OF_ORDER_WINDOW gives a number of frames BEFORE the
 * current sequence number that are assumed to be out of order. That is,
 * frames which fall outside of that window are to imply that a restart or
 * re-wrap has happened, with this side obviously losing some small->large
 * number of frames in between. 
 *
 * A frame grabbed inside the out-of-order window is assumed to then be an
 * out of order frame.
 *
 */
#define RSRB_SEQNUM_RESTART_WINDOW (20)
#define RSRB_SEQNUM_OUT_OF_ORDER_WINDOW (50)

#define RSRB_SEQNUM_PROCESS_BASE (1)
#define RSRB_SEQNUM_PROCESS_TOP  (16383)
#define RSRB_SEQNUM_PROCESS_RESTART (RSRB_SEQNUM_PROCESS_BASE + \
				     RSRB_SEQNUM_RESTART_WINDOW)

#define RSRB_SEQNUM_INTERRUPT_BASE (16384)
#define RSRB_SEQNUM_INTERRUPT_TOP (32767)
#define RSRB_SEQNUM_INTERRUPT_RESTART (RSRB_SEQNUM_INTERRUPT_BASE + \
				       RSRB_SEQNUM_RESTART_WINDOW)

/*
 * The following macros handle all cases of coping with the sequence number
 * when sending and receiving data with FST. This makes the code much cleaner.
 */

/*
 * Reset the send and receive variables for a peer
 */
#define RSRB_FST_RESET_SEQUENCE_NUMBERS(fstpeer) \
{ \
    (fstpeer)->ip_seqnumin = RSRB_SEQNUM_PROCESS_BASE; \
    (fstpeer)->ip_seqnumout = RSRB_SEQNUM_PROCESS_BASE; \
    (fstpeer)->ip_int_seqnumin = RSRB_SEQNUM_INTERRUPT_BASE; \
    (fstpeer)->ip_int_seqnumout = RSRB_SEQNUM_INTERRUPT_BASE; \
}

/*
 * Increment the send variable for a peer; store in 'outident' the value
 * of the sequence number to use for the next packet
 */
#define RSRB_FST_INCREMENT_SEND(fstpeer, atinterruptlevel, outident) \
{ \
    if ((atinterruptlevel)) \
      { \
	  outident = (fstpeer)->ip_int_seqnumout; \
	  (fstpeer)->ip_int_seqnumout = ((fstpeer)->ip_int_seqnumout % \
					 RSRB_SEQNUM_INTERRUPT_TOP) + 1; \
          if ((fstpeer)->ip_int_seqnumout == 1) { \
	     (fstpeer)->ip_int_seqnumout = RSRB_SEQNUM_INTERRUPT_RESTART; \
	  } \
      } \
    else \
      { \
	  outident = (fstpeer)->ip_seqnumout; \
	  (fstpeer)->ip_seqnumout = ((fstpeer)->ip_seqnumout % \
					 RSRB_SEQNUM_PROCESS_TOP) + 1; \
          if ((fstpeer)->ip_seqnumout == 1) { \
	     (fstpeer)->ip_seqnumout = RSRB_SEQNUM_PROCESS_RESTART; \
	  } \
      } \
}

/*
 * Take a given sequence number (supposedly from a frame) and return in
 * 'outresult' TRUE or FALSE depending on if the next frame is acceptable.
 * Update the counters in the peer as appropriate. outresult must be a simple
 * variable reference, not an expression! 'inputid' is the received, accepted,
 * sequence number.
 */
#define RSRB_FST_ACCEPTOR(fstpeer, inputid, outresult) \
{ \
    outresult = TRUE; \
    if ((inputid) >= RSRB_SEQNUM_INTERRUPT_BASE) { \
	/* Frame from remote side's interrupt level */ \
	/* Sequence number wrap or restart? */ \
        if ((inputid) <= RSRB_SEQNUM_INTERRUPT_RESTART) { \
  	    (fstpeer)->ip_int_seqnumin = (inputid); \
        } else { \
            /* Not within the 'out of order window? */ \
  	    if ((fstpeer)->ip_int_seqnumin < (inputid)) { \
	        if ((((fstpeer)->ip_int_seqnumin - \
		    RSRB_SEQNUM_INTERRUPT_RESTART + \
		    RSRB_SEQNUM_INTERRUPT_TOP) - (inputid)) > \
		    RSRB_SEQNUM_OUT_OF_ORDER_WINDOW) { \
		       (fstpeer)->ip_int_seqnumin = (inputid); \
	        } else { \
		  outresult = FALSE; \
		} \
	    } else { \
		if (((fstpeer)->ip_int_seqnumin - (inputid)) > \
		    RSRB_SEQNUM_OUT_OF_ORDER_WINDOW) { \
		    (fstpeer)->ip_int_seqnumin = (inputid); \
		} else { \
		    outresult = FALSE; \
		} \
	    } \
	} \
    } else { \
	/* Frame from remote side's process level */ \
	/* Sequence number wrap or restart? */ \
        if ((inputid) <= RSRB_SEQNUM_PROCESS_RESTART) { \
  	    (fstpeer)->ip_seqnumin = (inputid); \
        } else { \
            /* Not within the 'out of order window? */ \
  	    if ((fstpeer)->ip_seqnumin < (inputid)) { \
	        if ((((fstpeer)->ip_seqnumin - \
		    RSRB_SEQNUM_PROCESS_RESTART + \
		    RSRB_SEQNUM_PROCESS_TOP) - (inputid)) > \
		    RSRB_SEQNUM_OUT_OF_ORDER_WINDOW) { \
		       (fstpeer)->ip_seqnumin = (inputid); \
	        } else { \
		  outresult = FALSE; \
		} \
	    } else { \
		if (((fstpeer)->ip_seqnumin - (inputid)) > \
		    RSRB_SEQNUM_OUT_OF_ORDER_WINDOW) { \
		    (fstpeer)->ip_seqnumin = (inputid); \
		} else { \
		    outresult = FALSE; \
		} \
	    } \
	} \
    } \
    if (outresult == FALSE) { \
	if (srb_debug || srbevent_debug) { \
	  buginf("\nRSRB: Bad FST Seq input. (%s). C=%d, R=%d, IPS:%i", \
	         (((inputid) >= RSRB_SEQNUM_INTERRUPT_BASE) ? "interrupt" : "process"), \
	         (((inputid) >= RSRB_SEQNUM_INTERRUPT_BASE) ? (fstpeer)->ip_int_seqnumin : (fstpeer)->ip_seqnumin), \
		 (inputid), (fstpeer)->ipaddr); \
        } \
    } \
}


/*
 * Number of "sub" priorities we can have (top priority is priority zero)
 */
#define RSRB_PRIORITY_COUNT 3

typedef int (*fastlanout_type)(hwidbtype *, tr_vpeertype *, tr_ventrytype *,
                      ushort, paktype *);
typedef int (*hes_fastlanout_type)(hwidbtype *, tr_vpeertype *, tr_ventrytype *,
		      ushort, uchar *, int);
typedef int (*les_fastlanout_type)(hwidbtype *, tr_vpeertype *, tr_ventrytype *,
		      ushort, paktype *, int);

struct tr_vpeertype_ {
    struct tr_vpeertype_ *next;
    netbios_acclisttype *netbios_h_oacf;/* NETBIOS host access filter (out) */
    netbios_acclisttype *netbios_b_oacf;/* NETBIOS bytes access filter (out) */
    int rsrb_lsap_out;			/* rsrb output LSAP filter */
    netbios_acclisttype *rsrb_netb_out;	/* rsrb netbios host access filter output */
    ushort vrp_version;			/* Version of remote peer */
    ulong vrp_capabilities;		/* capabilities of this peer */
    boolean vrp_starting_up;		/* This peer needs a startup sequence */
    sys_timestamp last_keepalive;	/* last time a keep alive was sent */
    sys_timestamp keep_timer;           /* timer for keepalives */
    tr_vpeer_typetype type;
    tr_vpeer_statetype state;		/* what state this peer is in */
    boolean connector_owns;		/* true if connector owns this peer */
    boolean local_ack;			/* Local ACK to be enabled this peer */
    boolean rnr_disable;                  
    int retries;			/* used on open */
    sys_timestamp connect_timer;	/* used for connect window */
    tr_vgrouptype *vrg;
    iphdrtype *ip_header;		/* IP header for FST */
    ulong ip_seqnumin;			/* Sequence # of last received frame */
    ulong ip_seqnumout;			/* Sequence # of next sent frame */
    ulong ip_int_seqnumin;		/* # of last rcvd frame, from intrpt */
    ulong ip_int_seqnumout;		/* # of next sent frame, from intrpt */
    /*
     * Peer control vectors.
     */
    boolean	  (*sender)(tr_vpeertype *, tr_vhdrtype *, paktype *);
    boolean	  (*direct)(tr_vpeertype *, paktype *);
                                        /* just send it */
    void	  (*abort)(tr_vpeertype *); /* immediate flush */
    void	  (*close)(tr_vpeertype *, ulong); /* no input, drain out */
    vpeer_connrtn (*connect)(tr_vpeertype *, ushort);

    /*
     * Other control cells.
     */
    int largest_frame;			/* largest frame size for this peer */
    int largest_frame_def;		/* largest frame size for this peer */
    queuetype startq;			/* holds outgoing till connected */
    tr_vhdrtype inhdr;			/* incoming header */
    ulong drops;
    ulong pkts_rx;			/* num pkts incoming */
    ulong pkts_tx;			/* num pkts outgoing */
    ulong explorers;			/* num explorers we've seen */
    ulong bytes_rx;                     /* num bytes incoming */
    ulong bytes_tx;                     /* num bytes outgoing */
    ulong snmp_index;                   /* vrp index for snmp purposes */

    /*
     * TCP DRIVER and FST implementations
     * RSRB_PT_TCPD, RSRB_PT_FST
     */
    int port;				/* tcpdriver and udp */
    ipaddrtype ipaddr;			/* tcpdriver and udp */
    tcp_encaps *t;			/* tcpdriver */
    long ipversion_of_fst_route; 	/* IP Version number of FST route */
    ipaddrtype desthost_ipaddr;		/* next hop ip address */
    ulong tcp_rcv_window;		/* setting from parser or default */

    /*
     * TCP only fields (and only with LOCAL ACK)
     */
    boolean priority;
    struct tr_vpeertype_ *priority_peers[RSRB_PRIORITY_COUNT + 1];
    struct tr_vpeertype_ *priority_top;
     

    /*
     * Direct Interface implementation
     * RSRB_PT_IF
     */
    ushort dlci;			/* DLCI number for RSRB_PT_FR */
    idbtype *outswidb;
    hwidbtype *outidb;			/* direct interface idb */
    fastlanout_type vrp_fast_lanout;
    les_fastlanout_type vrp_les_fast_lanout;
    hes_fastlanout_type vrp_hes_fast_lanout;
    uchar macaddr[6];			/* if a mac address is needed. */
    struct tr_vpeertype_ *idbnext;	/* if more than one on this cable */

    /*
     * "reliable backup group" support for backup links (no load sharing)
     * for direct interface types
     */
    ulong	backup_group;		/* Backup group for this peer */
    queuetype   lackie;		        /* to associate lackie with vrp */
};

/* The default backup group (i.e., none) */
#define RSRB_BACKUP_GROUP_NONE	((ulong) (0))
#define BACKUP_GROUP_COUNT ((ulong) (100))

#define RSRB_MAX_OPEN_TRIES	3
#define RSRB_REMOPEN_DEFTMO	(10 * ONESEC)

/*
 * Remote Peer keep alive junk.
 */

#define RSRB_KEEPALIVE_DEFAULT_INTERVAL	(30 * ONESEC)
#define RSRB_KEEPALIVE_TX_PERIOD	(30 * ONESEC)
#define RSRB_KEEPALIVE_MISSES		3

/*
 * It is entirely possible to completely consume system memory with
 * a stream of packets sent to TCP, which are removed from accounting on the
 * input queue, but cannot get to their destination (for whatever reason).
 * Therefore, we implement a "throttle" on TCP connections. The users
 * may change this.
 */
#define RSRB_TCP_MAX_QUEUE_DEFAULT (100)


/*
 * We want to allow overqueueing for local-ack sessions for UI frames
 * but only to a certain limit . This limit will be 400% of the system
 * specified rsrb_tcp_max_queue_default or vring_tcpqmax as specified by the 
 * user . The current max for rsrb tcp queue is 4096 . Therefore we could
 * only ever queue a maximum of 20k on any single tcp queue . This UI frame
 * limitation is to prevent any rogue or smart applications from monopolising
 * system resources in the event of a problem with tcp delivering the frames
 * to there final destination .
 */         
#define RSRB_TCP_MAX_UI_FRAME_OVERQUEUE 5
/*
 * How long should a side doing a LACK "linkup" connection attempt wait for
 * a positive response? (in seconds)
 */
#define RSRB_LACK_CONNECTION_TIMEOUT_DEFAULT (30)

/*
 * The startq is a delay queue that holds packets destined for a given
 * server peer until it is open.  It points to an entry that contains an
 * age and two pointers.  One points to the original bridged packet and
 * the other points to a block of memory describing the header built so
 * far.
 *
 * If hdr is NULL, then the header is actually already on the front of
 * the packet.  It is pointed to by datagramstart and is included in
 * datagramsize.
 *
 * If it is not NULL than it is assumed to be a regular packet.  It is assumed
 * to already have had its sized checked and it won't fit on the front of the
 * data packet.  This will be true since the only way onto the startq is via
 * the sender which plays all these games.
 */

typedef struct _tr_starttype {
    struct _tr_starttype *next;
    paktype *pak;
} tr_starttype;


/*
 * When a peer is first opened up it is actually handed to RSRB Connector.
 * This process is actually responsible for opening up the peers one at a
 * time.  The vrconnQ feeds it until empty at which time it nukes itself.  The
 * packet that forced the open to occur is put into the peer's startq.  Any
 * other incoming packets for this peer are also stored there until the open
 * completes.  An open for this peer is then hung on the vrconnQ and the
 * Connector kicked.  This connection entry is defined here.
 */

typedef struct _tr_conntype {
    struct _tr_conntype *next;
    ushort vrn;				/* group being opened for */
    tr_vpeertype *vrp;			/* the description of the peer */
} tr_conntype;

/*
 * A few defines for explorer handling.
 */

/* default explorerQ depth */
#define DEFAULT_EXPLORERQ_DEPTH   30

/* Max time (in msecs) for srb_background() explorer processing */
#define MAX_SRB_EXPLORERQ_TIME    10

/* Max time interval (in 10th seconds) for explorer flush processing. */
#define MAXRATE_PERIOD            10

/* explorer fast-switch buffer size. */
#define EXPLORER_BUF_SIZE         8192

/* Default max fast explorer rate in bytes per second:
 * 38400 Bytes/sec = 600 pps * 64 Bytes/packet
 */
#define DEFAULT_FS_MAXRATE_HES    38400
#define DEFAULT_FS_MAXRATE_LES    38400

/* Clear some counters related to explorer fast-switching. */
#define fs_clear_counts(idb) if ((idb)->fs_expl_input != NULL) { \
    fs_explorer.total=fs_explorer.flush=\
    (idb)->fs_expl_input->data_rate=(idb)->fs_expl_input->count=\
    (idb)->fs_expl_input->out_drops=(idb)->fs_expl_input->storms=\
    (idb)->fs_expl_input->congested=(idb)->fs_expl_input->queue_depth=0; }

/* Define the high and low thresholds for the tcpQ to flow control the end stations */
#define LACK_TCPQ_THRESHOLD_HIGH  90
#define LACK_TCPQ_THRESHOLD_LOW   80

/*
 * Cookie for calls to create_vrpeer et al
 */
typedef struct rsrb_fr_ {
    idbtype *swidb;		/* Frame Relay Direct interface */
    ushort dlci;
} rsrb_fr_t;

typedef union rsrb_cookie_ {
    ipaddrtype ip;		/* Peer address */
    hwidbtype *idb;		/* Direct interface */
    rsrb_fr_t fr_cookie;
} rsrb_cookie;

enum { LLC_TRC_UFRAME, LLC_TRC_SFRAME, LLC_TRACE_IFRAME };
extern void init_llctrace(void);
extern void show_llctrace(void);
extern void make_llctrace(sap_hdr *, ulong, ulong, ulong, ulong);
extern void frame_make_llctrace(uchar *, ulong, ulong, ulong, ulong);

/*
 * Remote Source Route Bridging external variables
 */
extern watched_boolean *vr_wake_up;
extern int vrconn_pid;
extern queuetype vrconnQ;
extern watched_queue *vrconn_pakQ;
extern queuetype vringQ;
extern watched_queue *vrxchgQ;
extern ulong vrconn_timeout;		/* how long to wait for conn estab */
extern sys_timestamp vrconn_wait;	/* delay timer for connector */
extern ulong rsrb_keepalive_interval;	/* how long to wait before timing out a peer */
extern sys_timestamp rsrb_next_keepalive; /* when to wake up again for keep alive purposes */

/*
 * Remote Source Route Bridging prototypes
 */

extern tr_ventrytype *malloc_vre(const char *name);
void vring_init(void);
void rsrb_newstate(tr_vpeertype *vrp, tr_vpeer_statetype newstate);
paktype *vrpak(ushort version, ushort op, ushort trn, ushort vrn, ushort offset, ushort len);
tr_vgrouptype *vrfind_ringgroup(ushort vrn);
tr_ventrytype *vrfind_rn2entry(ushort rn, ushort bn, tr_ventrytype *se, ushort kind);
tr_ventrytype *vrfind_idb2entry(hwidbtype *idb, tr_vgrouptype *vrg);
tr_vpeertype *vrfind_peer(tr_vgrouptype *vrg, tr_vpeer_typetype type,
			  rsrb_cookie cookie, int lport, int fport,
			  uchar *macaddr);
char *vr_op2str(ushort op);
char *vrpeer_id(tr_vpeertype *vrp, char *buff);
char *vrpeer_state(tr_vpeer_statetype state);
int rif_match_offset(srbroute_t *rif, int ltrn, int tbn, int rtrn, ushort *rn, ushort *bn, char **why);
void vrforce_xchg_delayed(tr_vgrouptype *vrg);
paktype *ring_xchg_pak(tr_vgrouptype *vrg, ushort op, ushort version);
void vrqueue_null_xchg(tr_vpeertype *vrp);
boolean rsrb_checkqueue(queuetype *q, tr_vpeertype *e);
void vrprocess_ring_xchg(tr_vpeertype *vrp, paktype *pak);
void update_peers(void);
void vrsend_version(tr_vpeertype *vrp, ushort op, boolean abort_on_fail);
boolean vrsend_caps(tr_vpeertype *vrp, ushort op, ulong caps, char *label);
boolean vrsend_xchg(tr_vpeertype *vrp, ushort op, char *label, boolean doabort);
boolean vr_version_reasonable(tr_vpeertype *vrp, tr_vhdrtype *vrh, char *label);
boolean rsrb_tcp_correct_port(tr_vpeertype *vrp);
boolean vrcheck_conn_state(tr_vgrouptype *vrg, tr_vpeertype *vrp);
boolean vrconnectorBLOCK(tr_vpeertype *vrp);
void vrconnect1(tr_conntype *vrc);
forktype vrconnector(void);
void rsrb_vidb_iqueue(hwidbtype *idb, paktype *pak);
boolean rsrb_vidb_oqueue(hwidbtype *idb, paktype *pak, enum HEADTAIL which);
hwidbtype *vr_new_vidb(ushort vrn, hwidbtype *tr_idb);
boolean virtualp_default(tr_ventrytype *vre, uchar *macaddr);
tr_ventrytype * vr_virtualp(tr_vgrouptype *outvrg, ushort vrn, uchar *macaddr);
uchar *vr_first_virtual(uint vrn);
uchar *vr_next_virtual(ushort vrn, uchar *macaddr);
void vr_vre_input(tr_ventrytype *vre, paktype *pak, boolean nuke_crc, int op);
void vr_nukepak(paktype *pak);
void vrif_err(tr_vpeertype *vrp, tr_vhdrtype *vrh, char *label);
void vrif_send_punt(tr_vpeertype *vrp, ushort op, tr_vhdrtype *vrh, char *label);
void vrif_input(paktype *pak);
boolean vrif_sender(tr_vpeertype *peer, tr_vhdrtype *hdr, paktype *pak);
boolean vrif_direct(tr_vpeertype *peer, paktype *pak);
void vrif_abort(tr_vpeertype *peer);
void vrif_close(tr_vpeertype *peer, ulong param);
vpeer_connrtn vrif_connect(tr_vpeertype *vrp, ushort vrn);
void vrfst_err(tr_vpeertype *vrp, tr_vhdrtype *vrh, char *label);
void vrfst_send_punt(tr_vpeertype *vrp, ushort op, tr_vhdrtype *vrh, char *label);
void vrfst_input(paktype *pak);
paktype *rsrb_fst_full_duplicate(paktype *oldpak, tr_vhdrtype *hdr, boolean long_hdr);
boolean vrfst_sender(tr_vpeertype *peer, tr_vhdrtype *hdr, paktype *pak);
boolean vrfst_direct(tr_vpeertype *peer, paktype *pak);
void vrfst_abort(tr_vpeertype *peer);
void vrfst_close(tr_vpeertype *peer, ulong param);
vpeer_connrtn vrfst_connect(tr_vpeertype *vrp, ushort vrn);
int rsrb_packet_classify(paktype *pak);
tr_vpeertype *rsrb_packet_classify_peer(paktype *pak, tr_vpeertype *rootpeer);
boolean vrtcpd_sender(tr_vpeertype *inpeer, tr_vhdrtype *hdr, paktype *pak);
boolean vrtcpd_direct(tr_vpeertype *vrp, paktype *pak);
void vrtcpd_abort(tr_vpeertype *vrp);
void vrtcpd_close(tr_vpeertype *vrp, ulong param);
vpeer_connrtn vrtcpd_connect(tr_vpeertype *vrp, ushort vrn);
ipaddrtype vrtcp_peername(tr_vgrouptype *vrg);
void vrtcpd_send_close(tcp_encaps *t, ushort op, tr_vhdrtype *vrh, ulong param, char *label, tr_vpeertype *close_cookie, boolean close);
void vrtcpd_err(tcp_encaps *t, tr_vhdrtype *vrh, char *label);
boolean vrtcpd_gethdr(tcp_encaps *t, tcbtype *tcb, int thresh, tr_vpeertype *vrp, tr_vhdrtype *vrh, char *label, boolean check_ver);
void vrtcpd_drain(tcp_encaps *t, tcbtype *tcb, int thresh, void *cookie);
boolean vrtcpd_op_expected(tcp_encaps *t, tr_vhdrtype *vrh, tr_vpeertype *vrp, ushort expected_op, char *label);
void vrtcpd_phdr(tcp_encaps *t, tcbtype *tcb, int thresh, tr_vpeertype *vrp);
void vrtcpd_ahdr(tcp_encaps *t, tcbtype *tcb, int thresh, tr_vpeertype *vrp);
void vrtcpd_rdhdr(tcp_encaps *t, tcbtype *tcb, int thresh, void *cookie);
void vrtcpd_rddata(tcp_encaps *t, tcbtype *tcb, int thresh, void *cookie);
void vrtcpd_fini(tcp_encaps *t, tr_vpeertype *vrp);
void vrtcpd_listen(tcbtype *tcb);
boolean add_vring_interface(tr_vgrouptype *vrg, hwidbtype *idb, ushort kind);
void set_vring_interfaces(void);
void remove_vring_interface(hwidbtype *idb, ushort vrn);
boolean vre_fastout_err(idbtype *idb, idbtype *oidb, uchar *dgstart, int dgsize);
void rsrb_shutdownallfst(void);
void vrnail_peer(tr_vgrouptype *vrg, tr_vpeertype *vrp);
tr_vgrouptype *create_vring(ushort vrn, uchar *vmac);
void destroy_vring(int vrn, uchar *vmac);
void create_vrpeer(int vrn, int type, rsrb_cookie cookie, uchar *macaddr,
		   int largest, boolean local_ack, boolean rnr_disable,
		   ulong backup_group, boolean priority, int force_version,
		   netbios_acclisttype *netbios_host_list, netbios_acclisttype *netbios_bytes_list,
		   ulong tcprcvwnd);
void destroy_vrpeer(int vrn, int type, rsrb_cookie cookie, uchar *macaddr);
void vrforward_send_all_peers(paktype *ppak, tr_vpeertype *input_vrp);
void vrforward_explorer(paktype *pak, int offset, tr_vpeertype *input_vrp, int unit);
void vrforward_pak(paktype *pak, boolean sdllc_frame, int type, boolean processRifLf);
void vring_background(void);
void rsrb_periodic_per_peer(tr_vpeertype *vrp);
void rsrb_periodic(void);
void set_rsrb_tcpqlen(ulong len);
void set_rsrb_lack_connect_timeout(ulong len);
void rsrb_clear_counters(void);
void rsrb_print_peer_info(tr_vpeertype *vrp, ipaddrtype peername, int i);
void show_rsrb(void);
boolean rsrb_active(ulong sport, ulong dport, ipaddrtype fhost);
void rsrb_newbia(hwidbtype *idb);
void set_rsrb_largest(int vrn, int size);
boolean fs_srtlb_packets(paktype *pak, hwidbtype *idb);
boolean fs_srtlb_explorers(paktype *pak, fs_hdrtype *hdr, hwidbtype *idb);
boolean rsrb_vbridge_oqueue(hwidbtype *idb, paktype *pak, enum HEADTAIL which);
boolean rsrb_create_virtual_bridge(tr_vgrouptype *vrg, int bridge_num, int local_ring, int bridge_group, boolean netbios_cache, ulong transit_oui);
idbtype *rsrb_find_virtual_bridge(int bridge_group);
void rsrb_delete_virtual_bridge(tr_vgrouptype *vrg);
tr_vgrouptype *rsrb_exists_translational(int rgn);
void rsrb_netbios_delete_list(netbios_acclisttype *netbios_list);
boolean rsrb_opcode_valid(uint op);
/* SRB AST support */
extern idbtype *create_vidb(int);
extern void rsrb_delete_virtual_span(tr_vgrouptype *);
extern void rsrb_enable_virtual_span(idbtype *);
extern void rsrb_disable_virtual_span(idbtype *);
extern void vrforward_bpdu(paktype *);
extern boolean rsrb_vidb_span_forward(tr_vgrouptype *, tr_vpeertype *);
extern void rsrb_add_virtual_span(tr_vgrouptype *);
boolean rsrb_out_check(uchar *dgstart, ulong dgsize, hwidbtype *outidb, ushort riflen, tr_vpeertype *vrp);
boolean rsrb_netbios_acfcheck(uchar *dgstart, ulong dgsize, tr_vpeertype *vrp);
extern void rsrb_netbios_delete_list(netbios_acclisttype *);

extern tr_vpeertype *rsrb_find_vrp(paktype *pak);
extern boolean rsrb_is_pak_from_lak_peer(paktype *pak);

#endif __RSRB_H__
