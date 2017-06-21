/* $Id: frame_relay.h,v 3.10.4.17 1996/07/29 22:11:45 snyder Exp $
 * $Source: /release/112/cvs/Xsys/wan/frame_relay.h,v $
 *------------------------------------------------------------------
 * frame_relay.h -- Definitions for Frame Relay Protocol
 *
 * Summer 1990, Greg Fowler
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: frame_relay.h,v $
 * Revision 3.10.4.17  1996/07/29  22:11:45  snyder
 * CSCdi64472:  declare some arrays const in shr_frame, shr_frame_svc
 *              2380 out of data, 40 image
 * Branch: California_branch
 *
 * Revision 3.10.4.16  1996/07/10  07:37:59  chifei
 * CSCdi60658:  Clear counters clears frame relay MIB objects
 * Branch: California_branch
 *
 * Revision 3.10.4.15  1996/06/30  22:23:43  shankar
 * CSCdi59475:  Change units for Be/Bc from bytes to bits
 * Branch: California_branch
 * Images that do not include SVCs fail to link.
 *
 * Revision 3.10.4.14  1996/06/25  14:39:54  shankar
 * CSCdi61103:  Broadcast triggering of SVCs needs a configuration hook
 * for users
 * Branch: California_branch
 *
 * Revision 3.10.4.13  1996/06/18  23:21:55  widmer
 * CSCdi59768:  DLCI is not validated in EZSetup
 * Branch: California_branch
 *
 * Revision 3.10.4.12  1996/06/18  01:50:18  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.10.4.11  1996/06/11  18:15:01  fowler
 * CSCdi59356:  All frame relay subinterface tests are failing - could be
 *              a broadcast
 * Branch: California_branch
 *
 * Revision 3.10.4.10  1996/06/07  17:14:22  skoh
 * CSCdi57690:  Broadcasts must not trigger SVCs
 * Branch: California_branch
 *
 * Revision 3.10.4.9  1996/05/31  18:19:50  vrawat
 * CSCdi58846:  frame-relay debugging enhancements
 * Branch: California_branch
 *
 * Revision 3.10.4.8  1996/05/24  21:21:27  ccpoe
 * CSCdi58460:  map-class frame-relay traffic-shaping features dont work
 * on FR SVC
 * Branch: California_branch
 *
 * Revision 3.10.4.7  1996/05/22  23:57:15  pbettink
 * CSCdi57783:  Restoring Frame Relay minimum packet size to 4
 * Branch: California_branch
 *
 * Revision 3.10.4.6  1996/05/22  20:55:22  raj
 * CSCdi56721:  Debug ip mpacket doesnt show ttl and other important fields
 * Branch: California_branch
 * Added fields for "detail", and specifying acl as well as group.
 *
 * Revision 3.10.4.5  1996/05/15  05:55:01  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.10.4.4  1996/04/30  23:27:21  chifei
 * CSCdi52339:  Frame relay DTE/DCE switching across ip tunnel broken
 * Branch: California_branch
 * Enabled the full functionality of FR switching over IP tunnel,
 * also fixed regular switching function which was broken by improper
 * handling of the "frame intf-type" command.
 *
 * Revision 3.10.4.3  1996/04/25  23:24:20  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.10.4.2  1996/04/17  13:51:24  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.10.4.1  1996/03/18  22:47:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.3  1996/03/16  07:55:29  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.7.2.2  1996/03/07  11:07:20  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7.2.1  1996/02/20  21:51:28  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.10  1996/03/08  23:08:03  fowler
 * CSCdi40775:  clns fails on serial with fr-ietf encapsulation
 *
 * Revision 3.9  1996/02/26  18:56:49  rtio
 * CSCdi48935:  enum FR_LMI_TYPE out of sync with disp_lmi
 *
 * Added comments to frame_relay.h pointing to static variables in
 * fr_switch.c.  Fixed order of strings in disp_lmi[] in fr_switch.c.
 *
 * Revision 3.8  1996/02/08  20:14:46  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.7  1996/01/22  07:45:43  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.6  1996/01/15  19:17:55  turadek
 * CSCdi46908:  cant configure point-to-point frame-relay payload
 * compression
 *
 * permit the command "frame-relay payload-compress packet-by-packet" to
 * be parsed on a frame-relay encapsulated subinterface.
 *
 * also fixes automatic enabling of "half-compression" (i.e.,
 * decompression of
 * incoming packets without compressing outgoing packets.
 *
 * also fixes a bug relating to enabling/disabling compression on multiple
 * DLCIs via the frame-relay map command
 *
 * also removes an error message when attempt is made to compress a packet
 * with refCount > 1
 *
 * Revision 3.5  1995/11/30  07:01:05  ccpoe
 * CSCdi43927:  Frame-relay:  IP TCP Header-Compression causes router crash
 *
 * Revision 3.4  1995/11/21  20:44:19  turadek
 * CSCdi43841:  add frame relay payload compression feature
 * syncing code to 11.0
 *
 * Revision 3.3  1995/11/17  18:02:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:49:51  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:49:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.16  1995/11/08  21:35:33  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.15  1995/10/12  20:59:58  ccpoe
 * CSCdi41674:  bad getbuffer, bytes= 524293
 *
 * Revision 2.14  1995/09/15  02:14:46  dcoli
 * CSCdi39433:  Bad Encapulation Error
 * Break frame_relay and atm_dxi into separate object modules.
 * Make link2nlpid and nlpid2link inline functions so that they can be
 * shared without the overhead of a registry call.
 *
 * Revision 2.13  1995/09/13  20:59:32  fowler
 * CSCdi36199:  Frame relay IETF encapsulation for OSI doesnt comply with
 * 1490
 *
 * Revision 2.12  1995/09/13  07:02:08  hampton
 * All code should use the routine GETSHORT()/PUTSHORT() instead of using
 * the routines getshort()/putshort().  [CSCdi40249]
 *
 * Revision 2.11  1995/09/10 02:32:49  shankar
 * CSCdi40046:  Attempt ANSI LMI befor CCITT LMI for Autoinstall
 * Performance improvement that'll help our US customers.
 *
 * Revision 2.10  1995/09/09  00:54:38  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.9  1995/08/31  18:15:50  ccpoe
 * CSCdi39073:  Frame Relay: show frame relay pvc shows wrong pvc status.
 * Status received from LMI should not be altered in
 * fr_clear_dynamic_entries.
 *
 * Revision 2.8  1995/08/23  17:14:45  ccpoe
 * CSCdi38822:  Frame relay status is not accurate in show frame PVC/map.
 * fr_set_pvc_status() is now used to change the pvc_status.
 *
 * Revision 2.7  1995/08/08  16:50:46  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.6  1995/08/04 21:30:31  fowler
 * CSCdi37783:  FR broadcast queue count incremented for for all broadcasts
 *
 * Revision 2.5  1995/08/01  16:59:36  ccpoe
 * CSCdi37323:  Frame relay not counting FECN/BECN/DE incoming when fast
 * switching. Since we don't set FECN/BECN bit unless the packet is
 * frame-relay switched, counting incoming FECN/BECN/DE suffices the
 * requirement.
 *
 * Revision 2.4  1995/06/27  16:39:07  ccpoe
 * CSCdi35924:  admin. shut subinterfaces come up when configure
 * interface-dlci. A new function fr_set_subif_state() manages all the
 * sub-interface state now.
 *
 * Revision 2.3  1995/06/26  14:03:18  fowler
 * CSCdi33765:  Frame Relay dlci prioritization broken in dynamic mapping
 *
 * Revision 2.2  1995/06/09  04:37:25  ahh
 * CSCdi34470:  telnet slow over frame-relay with tcp header compression
 * Frame relay input can erroneously filter out puny compressed packets.
 *
 * Revision 2.1  1995/06/07  23:18:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __FRAME_RELAY_H__
#define __FRAME_RELAY_H__
#include "address.h"
#ifndef _FR_SVC_
#define _FR_SVC_
#include "../if/static_map.h"
#endif _FR_SVC_
#include "serial.h"
#include "../h/ieee.h"
#include <string.h>
#include <ciscolib.h>
#include "../if/traffic_shape.h"


/*
 * Frame Relay DE bit and DLCI prioritization structures
*/
#define FR_MAXLIST 11 
enum FR_LIST_TYPE {
    FR_DE_LIST,
    FR_DLCI_LIST };
    
typedef struct fr_plisttype_ {
    struct fr_plisttype_ *next;
    uchar linkcode;
    ushort testlist;
    long lt_size;
    long gt_size;
    ushort tcp_port;
    ushort udp_port;
    boolean ip_fragments;
    hwidbtype *interface;
    enum FR_LIST_TYPE type;
} fr_listtype;

/* TCP/IP header Compression related stuff */
# define LINK_IP_INHERIT_COMPR          LINK_MAXLINKTYPE+1
# define LINK_IP_INHERIT_ACTIVE_COMPR   LINK_MAXLINKTYPE+2
# define LINK_IP_INHERIT_PASSIVE_COMPR  LINK_MAXLINKTYPE+3
# define LINK_COMPR_TCP_AND_PASSIVE     LINK_MAXLINKTYPE+4
# define LINK_COMPR_TCP_AND_ACTIVE      LINK_MAXLINKTYPE+5
# define FR_LINK_ILLEGAL_SVC		LINK_COMPR_TCP_AND_ACTIVE + 1

# define COMPRESS_OVER_IP_LINK(x)       \
		((x->link == LINK_IP) &&        \
		(x->compression_flags & FR_MAP_INHERIT_COMPRESSION))

# define COMPRESS_OVER_COMPR_TCP_LINK(x)        \
		(x->link == LINK_COMPRESSED_TCP)

# define COMPRESS_OVER_P2P_LINK(x)              \
		((x->link == LINK_ILLEGAL) &&   \
		(x->compression_flags & FR_MAP_INHERIT_COMPRESSION))

										# define NO_OPTIONS           0
# define LINK_LEARN_OK(link) ((link == LINK_CLNS) || (link == LINK_BRIDGE) \
				|| (link == LINK_LLC2) || (link == LINK_RSRB) \
				|| (link == LINK_STUN) || (link == LINK_DLSW)) 
# define GET_PVC_DLCI(x) (GETSHORT(x+2))

# define MAXDLCI            1023
# define MAXDLCI_VALUES	    1024
# define FR_MIN_PKT_SIZE 4
# define FRAME_RELAY_EA_BITS 0x0001
# define FR_INVALID_EA_BITS  0x0100
# define DLCI_PRESENT 0
# define DLCI_ADDED   2
# define DLCI_DELETED 4
# define DLCI_STATUS_MASK 0x0c
# define DLCI_CHG_MASK    0x06
# define DLCI_ACTIVE(x) (x & 2)
# define IS_DLCI_DELETED(x) (x & 4)
# define DLCI_CONGESTED(x) (x & 1)

# define FR_SIGNALING_DLCI 0  /* DLCI 0 is for in-channel signaling */
# define DLCI0_MASK           0xFCF0
# define NULL_DLCI 0
# define IGNORE_DLCI            9999
# define FR_N391_COUNTER_MAX	256
# define FR_PVC_BW_NOTSET	0
# define FRAME_RELAY_DEFAULT_MULTICAST_DLCI 0
# define FRAME_RELAY_DEFAULT_LOCAL_DLCI 0
# define FR_UNDEFINED_DLCI 0XE000
# define FRAME_RELAY_DEFAULT_DLCI_BITS 10
# define FRAME_RELAY_SNAPENCAPBYTES 10
# define FRAME_RELAY_Q933_NLPID_SIZE      8
# define FRAME_RELAY_ENCAPBYTES 4
# define FR_TRANSIT_ENCSIZE(pak) ((pak->fr_flags & PAK_FR_IETF) ? \
				  FRAME_RELAY_SNAPENCAPBYTES : \
				  FRAME_RELAY_ENCAPBYTES)
# define FRAME_RELAY_SNAPENCAPWORDS	(FRAME_RELAY_SNAPENCAPBYTES >> 1)
# define FRAME_RELAY_ENCAPWORDS		(FRAME_RELAY_ENCAPBYTES >> 1)
# define FRAME_RELAY_LMI_ENCAPBYTES 4
# define FRAME_REALY_LMI_ENCAPWORDS	(FRAME_RELAY_ENCAPBYTES >> 1)
# define FR_SNAP_NLPID             0x0300
# define FR_SNAP_CONTROL           0x80
# define FR_Q933_NLPID             0x0308
# define L2_8022                   0x80
# define L3_SNA                    0x81
# define FR_L3_SNA                 0x7081
# define L3_SNA_PERIPHERAL         0x82
# define FR_L3_SNA_PERIPHERAL      0x7082
# define L3_APPN                   0x83
# define FR_L3_APPN                0x7083
# define L3_NETBIOS                0x84
# define FR_L3_NETBIOS             0x7084

typedef struct fr_q933_hdr_ {
	ushort dlci;
	ushort q933_nlpid;
	ushort l2_pid;
	ushort l3_pid;
	uchar  data[0];   /* provides offset only */
} fr_q933_hdr;


typedef struct frame_relay_hdr_ {
    ushort dlci;
    uchar  snap_start[0];	/* provides offset only */
    uchar  osi_type[0];         /* OSI type position */
    ushort type;
    uchar  data[0];
} frame_relay_hdr;

/* 
 * payload compression related stuff
 */

/* flag for following hdr */
#define FR_COMPRESSED			1

typedef struct fr_payload_compressed_hdr_ {
  frame_relay_hdr hdr;		/* carries special type, else normal */
  ushort	  trueType;	/* carries type expected in normal fr-header */
} fr_payload_compressed_hdr;

typedef struct fr_ietf_payload_compressed_hdr_ {
  frame_relay_hdr hdr;	
  uchar		  ui;		/* always 0x03 */
  uchar		  nlpid;	/* always 0xbo */
  uchar		  dcpHdr;	/* not supporting DCCI at this time */
  uchar		  seqNum;
} fr_ietf_payload_compressed_hdr;

/* flags for the dcpHdr field defined */
#define	FR_IETF_COMP_DCP_EXT	0x80


# define FR_SNAP_HDRSIZE  6
/*
 * RFC 1490 PIDs with OUI of 00 80 C2 (bridging)
 */
# define FR1490_ETHER_FCS	0x0001
# define FR1490_ETHER		0x0007
# define FR1490_802_4_FCS	0x0002
# define FR1490_802_4		0x0008
# define FR1490_802_5_FCS	0x0003
# define FR1490_802_5		0x0009
# define FR1490_FDDI_FCS	0x0004
# define FR1490_FDDI		0x000a
# define FR1490_802_6		0x000b
# define FR1490_SPANNING	0x000e

# define FR1490_FDDI_PADBYTES	1

# define RELAYHASH 64
# define DEF_BQSIZE  64
# define DEF_BQBYTES 256000
# define DEF_BQPKTS  36 
# define MIN_DLCI 16
# define MAX_DLCI 1007
# define MAX_FR_MGD_TIMER_COUNT 3

# define FR_ALL_MAPPED         (FR_IP_MAPPED | FR_IPX_MAPPED | FR_AT_MAPPED |\
				FR_XNS_MAPPED | FR_VINES_MAPPED|FR_DNET_MAPPED)

/*	
 * Keep track of which PVCs have map entries
*/				
# define FR_NOT_MAPPED   0
# define FR_IP_MAPPED    0x01
# define FR_IPX_MAPPED   0x02
# define FR_AT_MAPPED    0x04
# define FR_XNS_MAPPED   0x08
# define FR_VINES_MAPPED 0x10
# define FR_DNET_MAPPED  0x20
# define FR_CLNS_MAPPED  0x40
# define FR_BR_MAPPED    0x80
# define FR_RSRB_MAPPED  0x100
# define FR_STUN_MAPPED  0x200
# define FR_LLC2_MAPPED  0x400

# define FR_NOT_DYNAMIC  0
# define FR_IP_DYNAMIC   0x01
# define FR_IPX_DYNAMIC  0x02
# define FR_AT_DYNAMIC   0x04
# define FR_XNS_DYNAMIC  0x08
# define FR_VINES_DYNAMIC  0x10
# define FR_DNET_DYNAMIC   0x20


#define FR_INTF_DTE	0
#define FR_INTF_DCE	1
enum FR_MC_TYPE {
    FR_MULTICAST_NOTSET,
    FR_MULTICAST_STATIC,
    FR_MULTICAST_DYNAMIC };
    
enum PVC_USAGE_TYPE {
    PVC_LOCAL,
    PVC_SWITCHED_SERIAL,
    PVC_SWITCHED_IP_TUNNEL,
    PVC_SVC,
    PVC_NOTSET };

enum PVC_CONGESTION_INFO {
    NOT_CONGESTED,
    FORWARD_CONGESTED,
    BACKWARD_CONGESTED };

enum FR_SHAPE_CONF {
    SHAPE_CREATE,
    SHAPE_DELETE,
    SHAPE_UPDATE };

enum MAP_TYPE {
    FR_MAP_STATIC,
    FR_MAP_DYNAMIC,
    FR_MAP_INT_DLCI,
    FR_MAP_INT_DLCI_SVC,
    FR_MAP_INARP, 
    FR_MAP_AUTOINSTALL,
     FR_MAP_SVC_DYNAMIC}; 

enum PVC_TYPE {
    PVC_STATIC,
    PVC_DYNAMIC, 
    PVC_AUTOINSTALL,
    SVC_DYNAMIC};

/*
 * if the order of these enums are changed, be sure to change
 * the corresponding strings (disp_fr_intf) in fr_switch.c
 */
enum FR_INTF_TYPE {
    FR_DTE,
    FR_DCE,
    FR_NNI };

enum FR_TIMER_TYPE {
    FR_INARP_TIMER,
    FR_LMI_TIMER,
    FR_SWITCH_TIMER
};


/*
 * Used to set Subinterface state
 */
#define FR_FORCE_STATE	1
#define FR_ASSIGN_STATE	2
#define FR_PVC_STATE	3
#define FR_HWIF_STATE	4

/*
 * Frame Relay traffic statistics
 */
typedef struct fr_traffic_t_ {
    ulong arpreqsent;
    ulong arprepsent;
    ulong arpreqrcvd;
    ulong arpreprcvd;
    ulong multicasts_sent;
    ulong lmi_upd_rcvd;
    ulong lmi_stat_rcvd;
    ulong lmi_enq_sent;
} fr_traffic_t;
extern fr_traffic_t fr_traffic;

/*
 * new struct to remove all fr stuff from idb and just keep a ptr in the
 * idb for fr. Also add fr switching and DLCI info to the struct.
 */

typedef struct fr_pvc_stats_type_ {
    ulong num_input_pkts;      /* pkts received on serial */
    ulong num_output_pkts;     /* pkts sent out over serial */
    ulong num_bytes_rcv;       /* total no of bytes received on serial */
    ulong num_bytes_send;      /* total no of bytes sent over serial */
    ulong num_fast_input_pkts;      /* pkts received on serial */
    ulong num_fast_output_pkts;     /* pkts sent out over serial */
    ulong num_fast_bytes_rcv;       /* total no of bytes received on serial */
    ulong num_fast_bytes_send;      /* total no of bytes sent over serial */
    ulong num_dropped_pkts;    /* no of dropped packets */
    ulong num_rcvd_fecn_pkts;  /* no of pkts rcvd with FECN set */
    ulong num_rcvd_becn_pkts;  /* no of pkts rcvd with BECN set */
    ulong num_fast_rcvd_fecn_pkts;  /* no of pkts rcvd fswitch with FECN set */
    ulong num_fast_rcvd_becn_pkts;  /* no of pkts rcvd fswitch with BECN set */
    ulong num_fast_rcvd_de_pkts;    /* no of pkts rcvd with DE bit set */
    ulong num_sent_fecn_pkts;  /* no of pkts sent with FECN set */
    ulong num_sent_becn_pkts;  /* no of pkts sent with BECN set */
    ulong num_rcvd_de_pkts;    /* no of pkts rcvd with DE bit set */
    ulong num_sent_de_pkts;    /* no of pkts sent with DE bit set */
    ulong num_pkts_switched;
    ulong num_pkts_broad;
    ulong num_bytess_broad;
} fr_pvc_stats_type;


struct fr_idb_class {
    struct fr_idb_class *next;
    idbtype *swidb;
    mapclass_type *classp;
    uchar idb_inht;
};

static inline ulong fr_pvc_input_pkts (fr_pvc_stats_type *stats)
{
    return(stats->num_input_pkts + stats->num_fast_input_pkts);
}

static inline ulong fr_pvc_output_pkts (fr_pvc_stats_type *stats)
{
    return(stats->num_output_pkts + stats->num_fast_output_pkts);
}

static inline ulong fr_pvc_input_bytes (fr_pvc_stats_type *stats)
{
    return(stats->num_bytes_rcv + stats->num_fast_bytes_rcv);
}

static inline ulong fr_pvc_output_bytes (fr_pvc_stats_type *stats)
{
    return(stats->num_bytes_send + stats->num_fast_bytes_send);
}

static inline ulong fr_pvc_input_fecn_pkts (fr_pvc_stats_type *stats)
{
    return(stats->num_rcvd_fecn_pkts + stats->num_fast_rcvd_fecn_pkts);
}

static inline ulong fr_pvc_input_becn_pkts (fr_pvc_stats_type *stats)
{
    return(stats->num_rcvd_becn_pkts + stats->num_fast_rcvd_becn_pkts);
}

static inline ulong fr_pvc_input_de_pkts (fr_pvc_stats_type *stats)
{
    return(stats->num_rcvd_de_pkts + stats->num_fast_rcvd_de_pkts);
}

static inline void clear_fr_pvc_stats (fr_pvc_stats_type *pvc)
{
    memset(pvc, 0,sizeof(fr_pvc_stats_type));
}


typedef struct fr_route_type_ {
    ushort in_dlci;
    ushort out_dlci;            /* dlci on outgoing packet */
    hwidbtype *in_idb;          /* idb it came in on */
    hwidbtype *out_idb;         /* idb to route it to */
} fr_route_type;
enum FR_ENCAPS_FORM {
    FR_ENCAPS_NULL   = 0,
    FR_ENCAPS_NOTSET = 1,
    FR_ENCAPS_CISCO  = 2,
    FR_ENCAPS_IETF   = 4,
    FR_ENCAPS_CISCO2 = 8
    };
enum FR_CLEAR {
    FR_PVC = 0x1,
    FR_MAP = 0x2,
    FR_BOTH = 0x3,
    FR_LMI_CLEAR = 0x4
    };
  
typedef struct frame_relay_map_ {
    struct frame_relay_map_ *next;
    addrtype proto_addr;
    addrtype p2p_proto_addr;
    ushort dlci;
    ushort dlci_num;
    idbtype *idb;
/*    ushort flags;*/
    uint link;
    uint p2p_link;
    sys_timestamp time;
    enum MAP_TYPE map_type;      /* either a STATIC or DYNAMIC entry */
    boolean broadcast;
    enum FR_ENCAPS_FORM encaps_type;
    uint compression_flags;
    struct thcdbtype_  *tcp_compress;   /* non-NULL if TCP compression on */
} frmaptype;

/*
 * The SVC descriptor structure
 */
struct fr_svc_info_type_ {
    fr_svc_info_type	*next;			/* Pointer to the next svc*/
    fr_svc_info_type	*back;			/* Pointer to the previous svc*/

      /* Pointer to Interface layer */
    uint    call_id;                          /* Call Identifier for upper layer */

      /* Parameters RELATED */
    struct fr_svc_params     params;          /* store all parameters here */
    uchar             setup_resp_count;       /* first response must */
                                              /* contain the DLCI IE */
    uchar             setup_resp_type;        /* CALL PROC, SETUP etc. */
    uchar             call_clear_msg_count;   /* Keep track of call clearing */
                                              /* messages */
      /* SVC related timers */
    sys_timestamp     svc_t303;               /* SETUP sent */
    sys_timestamp     svc_t305;               /* DISCONNECT sent */
    sys_timestamp     svc_t308;               /* RELEASE sent */
    sys_timestamp     svc_t309;               /* Data Link got disconnected */
    sys_timestamp     svc_t310;               /* CALL PROCEEDING received */
    sys_timestamp     svc_t322;               /* STATUS ENQUIRY sent */
    uchar             t303_retry_count;       /* retry count is 2 */
    uchar             t308_retry_count;       /* retry count is 2 */
    uchar             t322_retry_count;       /* retry count is 2 */

      /* IE MANAGEMENT RELATED */
    uchar             last_ie_seen;           /* IEs must have increasing order */
    uchar             last_llc_ie_seen;       /* keep the info in the LLC IE in order */
    uchar             call_state;             /* State of call on this dlci */
    uchar             call_state_inc_msg;     /* Call state within a STATUS message */
    uchar             last_msg_seen;          /* Keep track of messages coming in */
    uchar             num_status_enq_sent;    /* keep track of STATUS ENQ sent */
    boolean           bearer_cap_ie_present;  /* Is the bearer capability present */
    boolean           dlci_ie_present;        /* DLCI IE present */
    boolean           ll_core_ie_present;     /* link layer IE is present */
    boolean           cg_pty_num_ie_present;  /* Calling party IE is present */
    boolean           cg_subaddr_ie_present;  /* Subaddress exists */
    boolean           cd_pty_ie_present;      /* Called Party IE present */
    boolean           cd_subaddr_ie_present;  /* Called Party Subaddr present */
    boolean           cn_number_ie_present;   /* connected number IE is present */
    boolean           cn_subaddr_ie_present;  /* connected subaddr IE is present */
    boolean           xit_nwk_ie_present;     /* Transit Network IE is present */
    boolean           ll_comp_ie_present;     /* Low Layer compatibility IE */
    boolean           user_user_ie_present;   /* User to User IE is present */
    boolean           cause_ie_present;       /* Many of these can be present */
    boolean           call_state_ie_present;  /* used by the STATUS message */

    boolean 		fr_svc_b2b; 		/* back to back SVCs */
};

struct fr_svc_nli_type_ {
    fr_svc_nli_type	*flink;		/* Pointer to the next nli */
    fr_svc_nli_type	*blink;		/* Pointer to the previous nli */

      /* Interface related */
    idbtype           *swidb;                 /* Pointer to a SW idb */

      /* IDB RELATED */
    ushort            dlci;                   /* DLCI for the call */
    ushort            filler1;
    uint              call_id;                /* Call Identifier for NLI layer */
    uint              call_ref;                /* Call Identifier for L3*/
    uint              nl_state;               /* Current state of the call */
    queuetype                 svc_holdQ;              /* temporary hold queue */
    struct fr_svc_params  *map_params;        /* pointer to user configuration */
    struct fr_svc_params  *in_params;         /* pointer to incoming configuration */
    staticmap_type    *map;                   /* Pointer to a static map */
    frmaptype		*frmap[32];	/* array of pointer to a fr map */
						/* the nli is referenced */
};

struct fr_pvc_info_type_ {
    struct fr_pvc_info_type_ *next;
    mapclass_type *pvc_class;
    uchar pvc_inht;
    uchar traffic_scope;
    uchar pvc_status;            /* status of this PVC active/inactive */
    uchar nni_pvc_status;        /* status received from NNI */
    ushort fr_list;
    boolean fr_if_set;
    enum PVC_CONGESTION_INFO pvc_congestion;        /* if pvc is congested */
    enum PVC_USAGE_TYPE pvc_usage;             /* is it local , switched etc */
    ushort route_flag;            /* route exists or not */
    ushort map_flag;              /* map exists or not */
    ulong pvc_bw;                 /* Bandwidth of this PVC */
    ulong pvc_intime;             /* last time pkt received on PVC */
    ulong pvc_outtime;            /* last time pkt sent on PVC */
    sys_timestamp pvc_create_time; /* time pvc was created */
    sys_timestamp pvc_change_time; /* last time pvc status was changed */
    struct VCQ *fr_vcq;
    traffic_shape_t *shape;

    ulong stats_save;		/* ptr to saved pvc stats at clear counters */
    fr_pvc_stats_type pvc_stats;  /* statistics info for the PVC */
    fr_route_type fr_route;       /* switching info if any for PVC */
    idbtype *idb;                 /* added because required for MIB */
    ushort use_count;              /* number of refs by maps to this PVC */
    ushort fr_pr_dlci_group;
#define this_dlci fr_pr_dlcis[0]
    ushort fr_pr_dlcis[4];
    ushort primary_dlci; /* if using multiple DLCIS for priority, main  DLCI */
    
    boolean fr_pr_enabled; 
    ushort pvc_type;              /* dynamic or static */
    uchar pvc_upd_seq;            /* update seq for deleted pvc's */
                                  /* also use this seq for NEW bit for DCE */
    uchar dummy;
    fr_pvc_info_type *bridge_flood_next;
    enum FR_ENCAPS_FORM bridge_encap;
    boolean tbridging_on_pvc;
    ushort convert_dlci;	  /* for fast-bridging */
    sys_timestamp     t_last_activity;        /* reset upon line activity */
    ulong svc_idle_timer;
    sys_timestamp svc_reset_time;        /* last time svc idle timer was reset */
};


struct fr_bootp_stuff_ {
	boolean fr_ai_initiated;	/* flag to indicate autoinstall */
	boolean fr_ai_need_fs;		/* Wait for full status from switch */
	int bootp_client_pid;		/* BOOTP client process */
};
struct fr_idb_struct_type_ {
    hwidbtype         *hwidb;
    struct fr_idb_class *fr_idb_classQ;
    queuetype fr_br_packetQ;    /* background output queue */
    queuetype fr_pvcQ;    /* orderd PVC queue */
    oqueue_t save_oqueue; /* save original oqueue vector */ 
#define FR_BQ_INT 10      /* number of intervals for broadcast queue */
    ulong bq_bytes;
    ulong bq_pkts;
    ulong bq_bytes_cur;
    ulong bq_pkts_cur;
    ushort bq_cur_int;
    ushort fr_dlci_bits;
    ushort fr_local_dlci;
    ushort fr_multicast_dlci;
    uchar fr_multicast_gid;
    enum FR_MC_TYPE fr_multicast_status;
    ushort fr_lsb_to_dlci_mask;
    ushort fr_lsb_to_num_mask;
    ushort fr_lsb_dlci_shift;
    ushort fr_msb_to_dlci_mask;
    ushort fr_msb_to_num_mask;
    ushort fr_msb_dlci_shift;
    
    boolean fr_lineup;	            /* current state of line protocol */
    boolean fr_traffic_shape;
    ushort fr_intf_type;            /* frame relay DTE/DCE/NNI */
    /*
     * ARP, LMI, VCQ idb structs
     */
    struct fr_idb_arp *fr_arp;
    struct fr_idb_lmi *fr_lmi;
    struct fr_idb_vcq *fr_vcq;
    /*
     * keep count of pvc's and routes
     */
    boolean fr_svc_configured;
    boolean fr_svc_enabled;   /* SVC function enabled or not */
    boolean fr_svc_b2b;		/* back to back SVC config */
    boolean lapfup;           /* SVC layer2 status, TRUE if in service */
    ushort num_idb_pvcs;
    ushort fr_err_dlci;  /* DLCI of last error pkt */
    fr_pvc_info_type *fr_dlci_tab[MAXDLCI_VALUES];
    ushort map_dynamic[MAXDLCI_VALUES];            /* track dynamic maps */
    ushort map_status[MAXDLCI_VALUES];           /*Track map existence per PVC*/

    boolean fr_ietf_encaps;
    boolean fr_bridge;
    boolean encaps_is_atm_dxi;
    boolean fr_idb_is_up; 
    ulong num_broad_frbq;
    ulong num_broad_total;
    ulong num_drop;
    fr_bootp_stuff *fr_bootp; 
    ulong lapf_cb;    /* pointer to layer 2 control block */
    boolean iqueue_initialized;                       /* I queue for SVCs */
    fr_svc_nli_type *nli_flink;                       /* Pointer to FR NLI queue head */
    fr_svc_nli_type *nli_blink;                       /* Pointer to FR NLI queue tail */
    fr_svc_info_type *svc_head;          /* Pointer to FR SVC queue head */
    fr_svc_info_type *svc_tail;          /* Pointer to FR SVC queue tail */
    sys_timestamp     timer_309;      /* DL layer up/down wait */

    /* used to auto-enable decompression of incoming packets */
    boolean needsCompressionContext;
};

# define FALSE_BUT_CONTINUE   -253

struct frame_relay_multicast_struct {
    uchar multicast_ie_id;
    uchar multicast_ie_length;
    uchar multicast_gid;
    ushort PACKED(multicast_dlci);
    ushort PACKED(multicast_source_dlci);
    uchar multicast_dlci_status;
};

struct frame_relay_pvc_struct {
    uchar pvc_id_ie;
    uchar pvc_id_length;
    ushort pvc_dlci;
    uchar pvc_status;
    ulong PACKED(pvc_bandwidth);
};

# define MULTICAST_SOURCE_DLCI_INDEX 5
# define MULTICAST_DLCI_INDEX 3
# define MULTICAST_DLCI_STATUS_INDEX 7

# define PVC_DLCI_INDEX 2
# define PVC_STATUS_INDEX 4
# define PVC_BANDWIDTH_INDEX 5

extern boolean frame_relay_debug;
extern boolean frame_relay_events_debug;


/*
 * DLCI map structure
 */
#define NUM_TO_DLCI(x) (((x & (ushort)fr_idb->fr_msb_to_dlci_mask) \
			<< fr_idb->fr_msb_dlci_shift ) | ((x & (ushort)fr_idb->fr_lsb_to_dlci_mask) \
			  << fr_idb->fr_lsb_dlci_shift ))
#define DLCI_TO_NUM(x) (((x & ( ushort)fr_idb->fr_msb_to_num_mask ) \
			>> fr_idb->fr_msb_dlci_shift) | ((x & (ushort)fr_idb->fr_lsb_to_num_mask ) \
			>> fr_idb->fr_lsb_dlci_shift))


#define BR_NUM_TO_DLCI(x,y) (((x & (ushort)y->fr_msb_to_dlci_mask) \
			<< y->fr_msb_dlci_shift ) | ((x & (ushort)y->fr_lsb_to_dlci_mask) \
			  << y->fr_lsb_dlci_shift ))
#define BR_DLCI_TO_NUM(x,y) (((x & ( ushort)y->fr_msb_to_num_mask ) \
			>> y->fr_msb_dlci_shift) | ((x & (ushort)y->fr_lsb_to_num_mask ) \
			>> y->fr_lsb_dlci_shift))

#define NUM_TO_ANSI_DLCI(x) (((x & 0x03f0) << 4) | ((x & 0x0f) << 3) | 0x0080) 

#define ANSI_DLCI_TO_NUM(x) (((x & 0x3f00) >> 4) | ((x & 0x78) >> 3)) 
#define HWIDB_TO_FR_PVC(hwidb,num) (hwidb->frame_relay_stuff->fr_dlci_tab[num])

/*
 * Define mask to check ANSI PVC status bits
 * Only the EA bit should be set
*/
#define FR_ANSI_VALID_PVC_MASK 0xc087
#define FR_ANSI_EA_BIT 0x80
/*
 * PVC status masks
*/
#define FR_ANSI_VALID_STATUS_MASK 0xf5

#define FR_SNAP1PAD    0x0300
#define FR_SNAP0PAD    0x0380
#define FR_OUI23_BRIDGE  0x80c2

#define NLPID_DEF(type) ((type == LINK_IP) || (type == LINK_CLNS))


#define ISFR_FECN(x) (x & 8)
#define ISFR_BECN(x) (x & 4)
#define ISFR_DE(x) (x & 2)

#define FR_FECN_BIT 0x0008
#define FR_BECN_BIT 0x0004
#define FR_DE_BIT   0x0002
#define FR_CR_BIT   0x0200
#define Q922_TEST  0xE3

/*
 * defines required for the MIB
 */
#define Q921_DLCI       13	/* 13 bit DLCI */
#define Q922MAR90_DLCI  11	/* 11 bit DLCI */
#define Q922NOV90_DLCI  10	/* 10 bit DLCI - the only one we support */
				/* = final version of q922 */

#define GET_FR_IDB(hwidb, fr_idb)   \
        if (hwidb->frame_relay_stuff) {fr_idb = hwidb->frame_relay_stuff; \
         } else return;

inline static boolean fr_pak_is_compressed(uint x)
{
	return((x == LINK_UNCOMPRESSED_TCP) || (x == LINK_COMPRESSED_TCP));
}

inline static boolean fr_valid_linktype_change(uint x, uint y)
{
	return((x == LINK_IP) && (y == LINK_COMPRESSED_TCP));
}

inline static boolean fr_valid_encaps_change(idbtype *x, int y, int z)
{
	return(((y == FR_ENCAPS_CISCO || y == FR_ENCAPS_NOTSET) &&
		z == FR_ENCAPS_IETF) &&
		(x->fr_tcp_hdr_comp == TRUE));
}

inline static boolean fr_valid_link_linktype_combo(uint x, uint y)
{
    return((x == LINK_IP && y == LINK_COMPRESSED_TCP) ||
	   (x == LINK_COMPRESSED_TCP && y == LINK_IP));
}

static inline void INC_PVC_SWITCHED (fr_pvc_info_type *pvc)
{
    pvc->pvc_stats.num_pkts_switched++;
}

static inline void INC_PVC_FAST_INPUT (fr_pvc_info_type *pvc,
                                       ushort bytes,
				       ushort dlci)
{
    pvc->pvc_stats.num_fast_input_pkts++;
    pvc->pvc_stats.num_fast_bytes_rcv += bytes;
    if (ISFR_FECN(dlci)) {
	pvc->pvc_stats.num_fast_rcvd_fecn_pkts++;
	if (reg_invoke_generic_shaping_fecn_received(pvc->idb, dlci))
	    pvc->pvc_stats.num_sent_becn_pkts++;
    }
    if (ISFR_BECN(dlci)) {
	pvc->pvc_stats.num_fast_rcvd_becn_pkts++;
	reg_invoke_generic_shaping_becn_received(pvc->idb,pvc);
    }
    if (ISFR_DE(dlci))
	pvc->pvc_stats.num_fast_rcvd_de_pkts++;

}
static inline void INC_PVC_FAST_OUTPUT (fr_pvc_info_type *pvc,
                                        ushort bytes)
{
    if (pvc) {
        pvc->pvc_stats.num_fast_output_pkts++;
        pvc->pvc_stats.num_fast_bytes_send += bytes;
	/*
	 * Need to reset the activity timer for SVCs
	 */
	TIMER_STOP(pvc->t_last_activity);
	TIMER_START(pvc->t_last_activity, pvc->svc_idle_timer);
    }
}

/*
 * link2nlpid
 * Given a link type, return a serial type code
 * Needed to make this an inline because it is called by the frame-relay
 * and atm-dxi subsystems.
 */
static inline ushort link2nlpid (ulong link)
{
    SAVE_CALLER();
    switch (link) {
        case LINK_IP:            return(FR_IETF_IP);
        case LINK_CLNS:
        case LINK_CLNS_BCAST:    return(FR_CLNP);
        case LINK_CLNS_ALL_ES:   return(FR_ESIS);
        case LINK_CLNS_ALL_IS:
        case LINK_ISIS_ALL_L1_IS:
        case LINK_ISIS_ALL_L2_IS: return(FR_ISIS);
    }

    if (serial_debug)
        buginf(fmt_Illegal_serial_link_type_code,
               link, caller());
    return(0);
}

/*
 * nlpid2link
 * Given a encapulation type, return a linktype
 * Needed to make this an inline because it is called by the frame-relay
 * and atm-dxi subsystems.
 */
static inline ulong nlpid2link (ushort nlpid)
{
    switch (nlpid) {
        case FR_IETF_IP:
            return(LINK_IP);

        case TYPE_CLNS:
        case FR_ESIS:
        case FR_ISIS:
        case FR_CLNP:
	case FR_ISO_IGRP:
        case ATM_DXI_ESIS_102:
        case ATM_DXI_ISIS_102:
        case ATM_DXI_CLNP_102:
            return(LINK_CLNS);

        default:
            return(LINK_ILLEGAL);
    }
}
/*
 * fr_svc_get_exponential
 * Some svc parameters are expressed in the form of multiplier*(10**magnitude)
 * This function is to get the value from multiplier and magnitude.
 */
static inline uint fr_svc_get_exponential(uint multiplier, uint magnitude)
{
    uint i, temp = 1;

    for (i = 1; i <= magnitude; i++) {
        temp *= 10;
    }
    return (temp * multiplier);
}

/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

/*
 * frame_relay.c
 */
extern frmaptype *relaytable[];
extern hwidbtype *fr_debug_hwidb;
extern ushort    fr_debug_dlci;
extern idbtype   *fr_debug_swidb;

extern boolean fr_vencapsulate(paktype *, long);
extern boolean fr_encaps(paktype *, long, frmaptype *);
extern boolean fr_doencaps (idbtype *, paktype *, ushort, uint,
 			    enum FR_ENCAPS_FORM, uchar, frmaptype *);
extern frmaptype *fr_addr_enter(idbtype *, addrtype *, ushort,
				boolean, enum FR_ENCAPS_FORM,
				enum MAP_TYPE, enum PVC_TYPE, uint, uint,
				 mapclass_type *);
extern frmaptype *fr_broadcast(idbtype *, paktype *, uint);
extern frmaptype *fr_broadcast_on_all_eligible_pvcs(idbtype *, paktype *, uint);
extern boolean fr_broadcast_cdp(idbtype *, paktype *);
extern frmaptype *frame_relay_addr_lookup(idbtype *, addrtype *, uint);
extern long frame_relay_getlink(paktype *);
extern void dump_packet(paktype *, uchar *, ulong);
extern void fr_lmi_init(void);
extern fr_pvc_info_type *fr_pvc_init(idbtype *, ushort,
				     enum PVC_USAGE_TYPE, enum PVC_TYPE,
				     boolean, mapclass_type *);
extern void fr_pvc_clear(hwidbtype *, ushort);
extern void fr_clear_map(idbtype *);
extern void fr_interface(hwidbtype *);
extern void frame_relay_display_map(frmaptype *);
extern void frame_relay_display_int_dlci (frmaptype *);
extern void frame_relay_hash_init(void);
extern void frame_relay_input(hwidbtype *, paktype *);
extern void frame_relay_map_delete(frmaptype *);
extern int frame_relay_parse_map(parseinfo *, idbtype *);
extern void frame_relay_periodic(hwidbtype *);
extern uchar * frame_relay_pvc_ie_parse(hwidbtype *, paktype *, uchar *);
extern void hexdump_packet(paktype *, ulong);
extern void print_dlci_status(uchar, hwidbtype *);
extern void print_ka_ie(uchar *);
extern void print_mc_ie(uchar *);
extern void print_pvc_ie(uchar *, uchar);
extern void print_report_ie(uchar *);
extern void show_fr_traffic(void);
extern void show_frame_relay_map(void);
extern void fr_cstate(hwidbtype *);
extern boolean fr_lmi_hdr_check(hwidbtype *, paktype *);
extern void fr_init_encaps_idb(idbtype *swidb, boolean setup_hwidb);
extern boolean fr_parse_p2p_dlci(idbtype *, ushort, boolean, 
			enum FR_ENCAPS_FORM, uint, mapclass_type *, addrtype);
extern void fr_parse_multi_dlci (idbtype *, ushort);
extern void fr_parse_no_if_dlci (idbtype *, ushort);
extern void set_fr_keepalive(hwidbtype *);
extern void frame_relay_parser_init(void);
extern void fr_update_n393(fr_idb_struct_type *, ushort, ushort *, ushort);
extern boolean fr_switching_idb(idbtype *idb);
extern void fr_encaps_nvram_write(parseinfo *); 
extern void fr_encaps_show_protocol_attr(idbtype *); 
extern void fr_clear_dynamic_entries(hwidbtype *, long, ushort, enum FR_CLEAR);
extern boolean fr_header_compression_set(idbtype *, boolean, boolean);
extern void fr_tcpip_header_compression_command(parseinfo *);
extern frmaptype *fr_verify_compression(hwidbtype *, uint, ushort);
extern paktype *fr_tcpip_hdr_compress(idbtype *, paktype *, frmaptype *);
extern void show_frame_relay_compression(parseinfo *csb);
extern void show_frame_relay_maplist(parseinfo *csb);
extern void fr_svc_teardown_calls (fr_idb_struct_type *);
extern void fr_svc_teardown_processes (boolean );
extern uint fr_inherit_compr_attributes(idbtype *, uint, uint, uchar);
extern void fr_debug_pak(paktype *, char *);
extern ushort l3pid(uchar);
extern uchar fr_extract_l3pid_value(paktype *);
extern void fr_update_ctrl_bits (paktype *, fr_pvc_info_type *, ushort ); 
extern boolean fr_sort_packet(hwidbtype *, paktype **, ushort); 
extern void fr_bootp_complete(hwidbtype *);
extern void fr_cleanup_after_ai(hwidbtype *);
extern int fr_bring_line_up(parseinfo *); 
extern void fr_bootp_setup(parseinfo *); 
extern void fr_wait(int);
extern addrtype *fr_addr_search (hwidbtype *, ushort, paktype *);
extern boolean fr_enter_static_map( hwidbtype *, addrtype *, ushort);
extern ushort fr_copy_snpa(hwaddrtype *);
extern ushort tb1490link2serial(paktype *); 
extern boolean fr_encaps_check(parseinfo *, hwidbtype *);
extern ushort fr_pr_set_dlci(paktype *, fr_pvc_info_type *, ushort );
extern void fr_pr_init_dlci_group(fr_idb_struct_type *,fr_pvc_info_type *);
extern void fr_set_pvc_status(fr_pvc_info_type *, ushort);
extern void fr_set_subif_state(idbtype *idb, int flag, uchar status);
extern void fr_svc_parser_init(void);
extern void fr_dec_pvc_count(fr_idb_struct_type *, hwidbtype *);
extern void fr_initiate_autosense(hwidbtype *);
extern void fr_mc_delete_dlci (hwidbtype *, ushort);
extern void fr_add_mc_dlci (hwidbtype *, enum FR_MC_TYPE, ushort);
extern boolean  frame_relay_lmi_debug(hwidbtype *hwidb);
extern boolean frame_relay_packet_debug(idbtype *idb, ushort dlci);
extern void fr_svc_broadcast_handler(idbtype *, paktype *);
extern boolean fr_switching;
#endif __FRAME_RELAY_H__
