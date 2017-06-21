/* $Id: atm.h,v 3.12.8.18 1996/09/08 03:03:25 rzagst Exp $
 * $Source: /release/112/cvs/Xsys/if/atm.h,v $
 *------------------------------------------------------------------
 * if/atm.h: Holder of the ATM encapsulation specific info.
 *
 * November, 1993, Rick Li Fo Sjoe
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atm.h,v $
 * Revision 3.12.8.18  1996/09/08  03:03:25  rzagst
 * CSCdi62883:  debug atm packet shows incorrect message for oam cells
 * Branch: California_branch
 * Prevent displaying of oam cells if debug atm packet is running, create
 * a separate atm_debug_swidb to hold the oam interface, and add a flag
 * to indicate whether inarp is running or not on the particular vc.
 *
 * Revision 3.12.8.17  1996/08/05  21:33:16  jwjang
 * CSCdi65054:  atm uni-version command disappear after reboot
 * Branch: California_branch
 *
 * Revision 3.12.8.16  1996/07/25  00:15:50  rbadri
 * CSCdi63919:  LANE Client can invalidate fast cache entries too often
 * Branch: California_branch
 * New flag to facilitate better cache invalidation when VCs go down.
 * When there is no change in line state, subif_line_statechange()
 * should just return.
 *
 * Revision 3.12.8.15  1996/06/29  22:04:41  rzagst
 * CSCdi59683:  OAM cells are not transmitted after ATM int was shut/noshut
 * Branch: California_branch
 *
 * Revision 3.12.8.14  1996/05/31  15:30:53  rzagst
 * CSCdi54940:  ATM driver creates low-bit-rate SVCs
 * Branch: California_branch
 *
 * Revision 3.12.8.13  1996/05/24  23:33:16  achopra
 * CSCdi56238:  Linear search for glommable VCs in the vc HASH table has
 *         been changed to a binary search in a WAVL tree maintained per
 *         swidb, which is keyed on NSAPs, AAL5 encapsulation and
 *         protocol type if encapsulation is AAL5MUX.
 * Branch: California_branch
 *
 * Revision 3.12.8.12  1996/05/24  21:09:43  schrupp
 * CSCdi58635:  ATM: ESI cleanup
 * Branch: California_branch
 *
 *  - Deregister addresses when a new prefix is added by ILMI
 *  - Deregister addresses when ESI/NSAP is changed from command line
 *  - Reregister addresses even if the new prefix is the same as the old
 *  - Deregister addresses when subinterface is shut
 *  - Reregister addresses when subinterface is no shutD
 *  - Don't allow qsaal and ilmi pvc's on subinterfaces
 *  - Init atm_db->peeruni_ifname
 *
 * Revision 3.12.8.11  1996/05/21  16:39:33  jwjang
 * CSCdi57875:  show int atm display wrong uni version
 *              added ATMUNI_VERSION_DEFAULT == ATMUNI_VERSION_30
 * Branch: California_branch
 *
 * Revision 3.12.8.10  1996/05/13  18:41:13  rzagst
 * CSCdi57247:  Burst size handled improperly between AIP and 4500
 * Branch: California_branch
 *
 * Revision 3.12.8.9  1996/05/09  14:29:36  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.12.8.8  1996/04/30  23:16:52  sdurham
 * CSCdi46300:  Interface missing from MIB
 * Branch: California_branch
 *          add atm and aal5 layers to the ifmib
 *
 * Revision 3.12.8.7  1996/04/29  21:08:53  rzagst
 * CSCdi55300:  Invalid vcd of 65535 causes FAIL_CREATEVC error
 * Branch: California_branch
 *
 * Revision 3.12.8.6.2.1  1996/04/27  07:03:27  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.12.8.6  1996/04/25  23:12:49  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.12.8.5  1996/04/09  22:48:11  ronnie
 * CSCdi51737:  SYS-3-INVMEMINT: Invalid memory action (free) at interrupt
 * level
 * Branch: California_branch
 *
 * Revision 3.12.8.4  1996/04/08  22:48:17  ronnie
 * CSCdi50592:  ARP client does not register to arp-server with 2 ATM cards
 * Branch: California_branch
 *
 * Revision 3.12.8.3  1996/04/05  17:59:23  ronnie
 * CSCdi43183:  malloc and free called from at interrupt level running 1577
 * Branch: California_branch
 *
 * Revision 3.12.8.2.4.2  1996/04/10  23:23:32  jwjang
 * CSCdi54325:  commit uni 3.1 to LE_Cal_V112_0_2_branch
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.12.8.2.4.1  1996/04/08  01:54:23  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.12.8.2  1996/03/29  19:26:41  grobelch
 *   Provide traffic shaping for SVCs when possible.
 *   Also:
 *   - Specify map-class rate parameters in kbps.
 *   - Provide a per-interface command to indicate
 *     whether SVC should be established if signalled
 *     parameters cannot be installed.
 *   - Don't attach ATM submaps to map-class block
 *   - Check for map-class params prior to using
 *     them (they can be null).
 *   - Fix help-strings for "clp1" parameters to
 *     indicate for clp0&1 flows.
 *   - Signal best effort if only pcr for clp0&1 are
 *     specified.
 *   - map-class parameters override map-list where
 *     they overlap.
 *   - Fix a couple of erroneous variable setting/usages
 *     in static-map handling.
 * CSCdi51550:  ATMSIG: SVCs should attempt to use traffic shaping
 * Branch: California_branch
 *
 * Revision 3.12.8.1  1996/03/18  20:13:06  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.12.10.1  1996/03/28  17:25:14  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.12  1996/02/27  19:27:33  achopra
 * CSCdi45189:  "show atm traffic" shows different (wrong) packet counts
 *
 * Revision 3.11  1996/02/03  22:17:49  ronnie
 * CSCdi46955:  ATM ARP Server table out of sync with map list
 *
 * Revision 3.10  1996/01/30  20:09:14  rzagst
 * CSCdi46580:  debug atm packet needs to filter on an interface or vc
 * basis
 *
 * Revision 3.9  1996/01/26  03:08:38  awu
 * CSCdi43779:  Declaring or removing a PVC can cause loss of all SVCs.
 *  -change the way of setting up a rateq so that resetting the interf card
 *   is being bypassed.
 *
 * Revision 3.8.14.1  1996/01/24  22:17:47  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.8  1995/12/11  22:22:36  awu
 * CSCdi31527:  number of current VC in show atm vc differs from show int
 * atm.
 *
 * Revision 3.7  1995/11/28  22:33:33  rzagst
 * CSCdi39434:  atm->vc_hash[] should be (vc_info_t *) not unsigned
 *
 * Revision 3.6  1995/11/23  19:58:07  bbenson
 * CSCdi42518:  vc creation fails because of wrong max_vpibits in ILMI
 * Fixes bits per VPI/VCI reported by the ILMI.
 *
 * Revision 3.5  1995/11/23  10:40:13  weismann
 * CSCdi34084:  peak/ave/burst is not always displayed properly in show
 *              atm vc. Fixed atm debug,show output. Handle atm vc
 *              defaults with flags.
 *
 * Revision 3.4  1995/11/22  20:45:04  ronnie
 * CSCdi44212:  Many fixes done at Telecom Finland
 *
 * Revision 3.3  1995/11/17  09:25:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:37:17  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:47:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.18  1995/11/08  21:02:40  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.17  1995/10/22  02:41:48  lmercili
 * CSCdi40829:  AAL34SMDS encap on a PVC allows you to specify inarp
 *
 * Revision 2.16  1995/09/28  20:50:35  ronnie
 * CSCdi40510:  Too many ATM SVCs set up to same destination
 *
 * Revision 2.15  1995/09/14  21:07:17  ronnie
 * CSCdi40212:  Declaring a new ARP Server leaves old servers map lying
 * around
 *
 * Revision 2.14  1995/09/09  00:52:15  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.13  1995/08/28  15:47:34  wfried
 * CSCdi39408:  ATM interface up decision should be based on more than
 * just LOS
 * Declare interface up only if PLCP framing or Cell delineation is
 * recognized for E3/DS3 interfaces.
 *
 * Revision 2.12  1995/08/18  14:23:42  schrupp
 * CSCdi38181:  Add ATMSIG throttling and SSCOP changes
 *
 * Revision 2.11  1995/08/11  20:58:50  ronnie
 * CSCdi38022:  ATM ARP client needs to check for valid NSAP before
 * creating map
 *
 * Revision 2.10  1995/08/10  23:30:41  wfried
 * CSCdi37608:  oam f5 cells generated by 4500 are not recognized by
 * analyzer
 * Changed ATM code to pass VC pointer for OAM cells and other related
 * changes
 *
 * Revision 2.9  1995/08/08  16:47:51  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.8  1995/07/26 15:14:23  schrupp
 * CSCdi37690:  Support ATM ESI addressing with ILMI prefix
 *
 * Revision 2.7  1995/07/19  07:11:31  awu
 * CSCdi36778:  <show atm vc nn> rejects valid vcd numbers.
 *
 * Revision 2.6  1995/06/28  09:26:25  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.5  1995/06/27  00:22:10  wfried
 * CSCdi36069:  Performance tuning and changes on controller screen for
 * new interfac
 * Included more interface status in the show controller output,
 * forced ATM receiving packets to start at 16 byte boundaries for
 * better DBUS utilization, and made some fastswitching optimizations.
 * Created new ATM interface loopback command options and renamed E3
 * framing modes to reflect current terminology
 *
 * Revision 2.4  1995/06/26  19:13:47  schrupp
 * CSCdi36416:  Undo CSCdi36400: restore atm idle-timeout default
 * Set default to 5 minutes.
 *
 * Revision 2.3  1995/06/24  14:42:50  schrupp
 * CSCdi36400:  Set atm idle-timeout default to 0, disabling the feature
 *
 * Revision 2.2  1995/06/24  00:45:46  awu
 * CSCdi35640:  ATM_BAD_VC macro is incorrect.
 *
 * Revision 2.1  1995/06/07  20:50:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __ATM_H__
#define __ATM_H__

#include "../h/ieee.h"
#include "../util/wavl.h"
/*
 * Define the Descriptor Mode bits for per-packet processing
 */
#define ATM_DM_PTI              0xE000          /* (see PTI below) */
#define ATM_DM_XD_INTT          0x1000
#define ATM_DM_EOM_EN           0x0800
#define ATM_DM_APP_CRC32        0x0400
#define ATM_DM_PKT_AAL34        0x0000          /* AAL 3/4 fragmentation */
#define ATM_DM_PKT_AAL5         0x0100          /* AAL 5 fragmentation */
#define ATM_DM_PKT_OAM_VC       0x0200          /* OAM info. from VC table */
#define ATM_DM_PKT_OAM_DM       0x0300          /* OAM info. from PTI */
#define ATM_DM_PKT_MSK          0x0300          /* Packet type field
						   mask */


typedef	struct	atm_multicast {
    struct	atm_multicast 	*next;	/* Ptr. to next struct. */
    struct	atm_multicast 	*prev;	/* Ptr. to prev struct. */
    idbtype			*idb;	/* VIDB with this multicast */
    hwaddrtype			mca;	/* Multicast addr. */
} atm_multicast_t;

typedef enum {
    ATM_SETUP_FAILED = 0,
    ATM_SETUP_SUCCESSFUL,
    ATM_SETUP_DELAYED
} atm_setup_status_t;

#define	ATM_MID_MIN		16
#define	ATM_MID_MAX		1024

#define ATM_KBPS_TO_CELLRATE(d_kbps) ((d_kbps)*1000/53/8)

/*
 * Peakrate and average rate are in the unit of Kbps while
 * rateq (as well as plim line rate) are in the unit of Mbps.
 * They all should be used in Kbps in the future. 
 */
#define ATM_MBPS2KBPS		1000

/*
 * Define the maximum nsap string for atm_printnsap.
 */
#define ATM_MAX_NSAP_STRING	51

/*
 * Define ATM default information
 */
#define	ATM_RXBUFF_DEFAULT	256
#define	ATM_TXBUFF_DEFAULT	256
#define	ATM_EXCQ_DEFAULT	32
#define	ATM_RAWQ_DEFAULT	32
#define	ATM_VC_PER_VP_DEFAULT	1024
#define ATM_MAX_VPI_BITS         8
#define ATM_PAYLOAD_BYTES_IN_A_CELL 48 /* data bytes in an atm cell*/
#define	ATM_VP_FILTER_DEFAULT	0x7b
#define	ATM_MAX_VC_DEFAULT	2048	
#define	ATM_MID_DEFAULT		ATM_MID_MIN
#define	ATM_IDLETIME_DEFAULT	(5 * ONEMIN)
#define ATM_CELLSIZE        53  /* bytes in an ATM cell */
#define ATM_DATASIZE        48  /* bytes of payload in a cell */
#define ATM_AAL34_OVERHEAD  4   /* AAL3/4 steals 4 byts of payload */
#define ATM_ADDREG_DEFAULT    (30 * ONESEC)
#define ATM_ILMIPOLL_DEFAULT  0 
#define ATM_ILMIPOLL_MIN      (3 * ONESEC)
#define	ATM_MTP_INTERVAL_DEFAULT (60 * ONESEC)
#define ATM_MTU_REJECT_DEFAULT FALSE /* only warn don't reject call */

/*
 * used to convert the max vpi/vci bits supported to the corresspodent
 * max vpi/vci value
 */
#define ATM_BITS2NUM(bits) ((2 << ((bits)-1)) - 1)

#define ATM_MAX_VCNUM	(ATM_MAX_VC_DEFAULT - 1)

/*
 * Define min. & max. values for the vp-filter
 */
#define ATM_VP_FILTER_MIN 0
#define ATM_VP_FILTER_MAX 0xFFFF


/*
 * Define encapsulations 
 * For all but AAL5/MUX type VCs, the enctype field identifies one of
 * the following encapsulations that follow.  For MUX VCs, the enctype
 * field contains the actual ethertype associated with the VC.  The RP
 * fills in the ETYPE for outbound frames, and the AIP fills in the ETYPE
 * for all inbound frames.  That indicates that the AIP does know the
 * ETYPE associated with particular MUX VCs!
 */
#define	ATM_ENCTYPE_SNAP	0		/* AAL5-LLC/SNAP type */
#define	ATM_ENCTYPE_SMDS	1		/* AAL3/4-SMDS type */
#define	ATM_ENCTYPE_NLPID	2		/* AAL5-LLC/NLPID */
#define	ATM_ENCTYPE_FRNLPID	3		/* AAL5-LLC/FR-NLPID */
#define	ATM_ENCTYPE_SAAL	4		/* SAAL type frame */
#define	ATM_ENCTYPE_RAW		5		/* RAW or OAM F4/F5 cell */
#define	ATM_ENCTYPE_ILMI	5		/* ILMI frames */
#define ATM_ENCTYPE_LANE        6               /* LAN Emulation frames */
#define	ATM_ENCTYPE_VCSTAT	7		/* VC Statistics packet */

/*
 * The LANE_BUS encap is an anomaly in that the encap is identical to
 * LANE, but we need to handle all BUS packets separately in the fastpath,
 * so that the packets don't get autonomously switched in the event
 * that a unicast packet should be _flooded_ on the BUS.  The SP will
 * classify and set the rxtype but *not* autonomous switch a packet
 * which is received on this enctype.
 *
 * Due to space/code requirements on the SP, the ATM_ENCTYPE_LANE_BUS
 * ETYPE is assigned the same value as ATM_ENTYPE_LANE, with the high
 * bit of the nibble set, so the effective value is 12 or 0xC.
 */
#define ATM_ENCTYPE_LANE_BUS	(ATM_ENCTYPE_LANE | 0x8)

/*
 * Remote bridging(802.1g)
 */
#define TBRIDGE_OUI         0x0080c2
#define TBRIDGE_OUI_1ST_BYTE   0x00
#define TBRIDGE_LLC_OUI_1ST_LONG   0xAAAA0300
#define TBRIDGE_OUI_LAST_SHORT 0x80c2
#define TBRIDGE_PID_8023FCS 0x1     /* 802.3 - FCS present */
#define TBRIDGE_PID_8024FCS 0x2     /* 802.4 - FCS present */
#define TBRIDGE_PID_8025FCS 0x3     /* 802.5 - FCS present */
#define TBRIDGE_PID_FDDIFCS 0x4     /* FDDI - FCS present */
#define TBRIDGE_PID_8026FCS 0x5     /* 802.6 - FCS present */
#define TBRIDGE_PID_8029FCS 0x6     /* 802.9 - FCS present */
#define TBRIDGE_PID_8023    0x7     /* 802.3 - no FCS */
#define TBRIDGE_PID_8024    0x8     /* 802.4 - no FCS */
#define TBRIDGE_PID_8025    0x9     /* 802.5 - no FCS */
#define TBRIDGE_PID_FDDI    0xA     /* FDDI - no FCS */
#define TBRIDGE_PID_8026    0xB     /* 802.6 - no FCS */
#define TBRIDGE_PID_8029    0xC     /* 802.9 - no FCS */
#define TBRIDGE_PID_BPDU    0xE     /* BPDUs */

#define TBRIDGE_ATM_PAD         0
#define	ATM_BRIDGE_PAD_SIZE	2	/* RFC1483 defined PAD area */

/*
 * atm_counters[] definitions
 */
enum	ATM_COUNTERS	{
	ATM_INPUT,			/* All ATM driver input packets */
	ATM_OUTPUT,			/* All ATM driver output packets */
	ATM_BROADCAST,			/* Output Broadcast packets */
	ATM_INPUT_NOVC,			/* Incoming packets on unknown VC */
	ATM_OUT_NOVC,			/* Attempting to o/p on unknown VC */
	ATM_BRIDGE,			/* Unused */
	ATM_CRC_ERRS,			/* Unused */
	ATM_CELL_LOSS,			/* Unused */
	ATM_OAM_INPUT_CELLS,		/* Mgmt. input packets */
	ATM_OAM_OUTPUT_CELLS,		/* Mgmt. output packets */
};

#define	ATM_MAX_COUNTERS	10
extern ulong atm_counters[ATM_MAX_COUNTERS];

/*
 * ATM Forum UNI Signalling version
 * this map to the MIB. 0 is invalid in the MIB. 
 */
typedef enum atm_universiontype_ {
  ATMUNI_VERSION_INVALID = 0,    /* uni_version invalid */
  ATMUNI_VERSION_20,
  ATMUNI_VERSION_30, 
  ATMUNI_VERSION_31,
  ATMUNI_VERSION_40,
  ATMUNI_VERSION_UNSUPPORT 
} atm_uniVersionType;
#define ATMUNI_VERSION_DEFAULT      ATMUNI_VERSION_30

typedef ushort	vcd_t;
typedef int     user_vc_type_t;

typedef	struct	atmaddr	{
    char 			nsap_addr[ADDRLEN_CLNS];
} atmaddr_t;

/*
 * Define the different ATM encapsulation types allowed.
 * Max is 16 for now.  See VC_FLAG_AAL_MASK for the WHY.  We can
 * later change that to a larger no. if need be.
 */
enum	ATM_VC_ENCAP {
	ATM_VC_AAL5SNAP,		/* AAL5 with SNAP encaps */
	ATM_VC_AAL5NLPID,		/* AAL5 with NLPID */
	ATM_VC_AAL5FRNLPID,		/* AAL5 with NLPID for FR */
	ATM_VC_AAL5MUX,			/* AAL5 Muxed.... */
	ATM_VC_AAL34SMDS,		/* AAL3/4 SMDS encaps */
	ATM_VC_AALQSAAL,		/* Signalling AAL */
	ATM_VC_AAL5ILMI,		/* ILMI VC */
	ATM_VC_AAL5LANE,		/* LAN emulation */
};

/*
 * Define the OAM status of a VCD
 */
enum	ATM_OAM_Status	{
	ATM_OAM_DISABLED,		/* No OAMs on this VCD */
	ATM_OAM_SENT,			/* OAM sent.  Waiting for ECHO */
	ATM_OAM_RECEIVED,		/* OAM received from target */
	ATM_OAM_FAILED,			/* Last OAM did not return */
};

#define	ATM_INARP_FREQ		15		/* Minutes */
#define	ATM_VC_INARP		ATM_INARP_FREQ
#define	ATM_VC_OAM		10	/* Default OAM loop value --seconds--*/

#define	VC_FLAG_AAL_MASK	0xF		/* Mask out other flags */
#define	VC_FLAG_ACTIVE		0x10		/* VC is ACTIVE */
#define	VC_FLAG_PVC		0x20		/* VC is a PVC */
#define	VC_FLAG_SVC		0x40		/* VC is a SVC */
#define	VC_FLAG_NOIDLETIME	0x80		/* no idle timeouts on this */
#define	VC_FLAG_TEMP		0x100		/* auto-created not for NVRAM*/
#define	VC_FLAG_MULTIPOINT	0x200		/* Is a multipoint VC */
#define VC_FLAG_DEFAULT_RATE    0x400           /* Default peak,av rate flag */
#define VC_FLAG_DEFAULT_BURST   0x800           /* Default burst Size */
#define VC_FLAG_NOBUMP_CACHE    0x1000		/* Control VC; while removing
						 * this VC, no need to 
						 * invalidate cache entries
						 */
#define VC_FLAG_DELETED         0x2000          /* VC deletion pending */
#define VC_FLAG_PVC_DOWN        0x4000          /* VC PVC deletion pending in
						   hw but since pvc not hashed
						   out */
#define	VC_FLAG_HOLD		0x8000		/* VC is in HOLD mode */

#define	MAX_BURST_SIZE		2047	
#define	MAX_OAM_RETRIES		5		/* Max. # of LOOPBACK tries */
#define UNSUPPORTED_COUNT       -1

struct	vc_info_t_ {
    struct 	vc_info_t_	*next_vc;	/* Next VC in hashqueue */
    struct 	vc_info_t_	*switch_vc;	/* For VC switching */
    wavl_node_type 	wavl;			/* WAVL tree  node */
    idbtype			*swidb;		/* IDB associated with this */
    pooltype			*bufpool;	/* VC's associated buf pool */
    unsigned   		        port_index; 	/* SNMP Port Index */
    struct 	vc_info_t_	*next_inarp;	/* Next InARP VC */
    struct 	vc_info_t_	*prev_inarp;	/* Prev InARP VC */
    unsigned			vpi;		/* VPI for this VC */
    unsigned			vci;		/* VCI for this VC */
    ushort			vc;		/* VC # */
    ushort			etype;		/* ETYPE for MUX VCs */
    int				linktype;	/* protocol for MUX VCs */
    unsigned			flags;		/* FLAGS */
    user_vc_type_t		user_vc_type;	/* per-VC user information */
    unsigned			qos;		/* QOS info. */
    unsigned			peakrate;	/* Peak rate(Kbps) */
    unsigned			avgrate;	/* Avg. rate(Kbps) */
    unsigned			burstcells;	/* Burst cells */
    unsigned			encapsize;	/* Size of encapsulation */
    hwaddrtype 		        calledparty;    /* Who we're talking to */
    unsigned			vcmode;		/* Assigned by aip_setup */
    ushort			mid_start;	/* AAL3/4-Lower MID */
    ushort			mid_end;	/* AAL3/4-Upper MID */
    unsigned 			inarp;		/* InARP frequency 0==OFF */
    mgd_timer			arp_timer;	/* InARP/ARP managed timer */
    boolean                     inarp_timer_running; /* timer running */
    unsigned			oam;		/* OAM frequency 0==OFF */
    enum ATM_OAM_Status		oam_status;	/* Status of OAM */
    unsigned			oam_retries;	/* Retry count */
    mgd_timer			oam_timer;	/* OAM managed timer
						 */
    struct vc_atommib_param_t_	*vc_atommib_param; /* atommib stuff */
/*
 * The infamous counters
 */
    unsigned		inpkts;		/* Packets received */
    unsigned		outpkts;	/* Packets sent */
    unsigned		infast;		/* Fastswitched in */
    unsigned		outfast;	/* Fastswitched out */
    unsigned		broadcasts;	/* No. of broadcasts */
    unsigned		in_as;		/* Packet IN Autonomous */
    unsigned		out_as;		/* Packets OUT Autonomous */
    unsigned		in_oam;		/* # of OAM cells received */
    unsigned		out_oam;	/* # of OAM cells sent */
    unsigned		corr_oam;	/* OAM Correllation TAG */
    /*
     * The AS counters below are updated from a 'getvc_stats()' call to
     * the ATM IP driver.
     */
    unsigned			in_pkts;	/* AIP input packet count */
    unsigned			out_pkts;	/* AIP output packet count */
    unsigned			in_bytes;	/* Total bytes RX */
    unsigned			out_bytes;	/* Total bytes TX */
    struct vc_info_t_		*next_vc_in_bridge_group; /* pointer to
						   next vc in bridge
						   group */
    /*
     * total packets in and out last time we checked.  used to
     * determine whether the link is idle or not.
     */
    unsigned		last_in_pkts;
    unsigned		last_out_pkts;

    /*
     * total packets in and out last time AIP was polled, to keep Autonomous
     * input and output packets counts in synch. 
     */
    unsigned		total_in_pkts; 		/* Total pkts. in */
    unsigned		total_out_pkts;		/* Total pkts out */
    sys_timestamp	idle_timer;	/* for timing out idle svc's */
    sys_timestamp	last_updown_time;
};

/* return codes for routine to that checks qos parameters */
#define  ATM_TRAFFIC_SHAPING_VERIFIED      0     /* no error */
#define  ATM_PEAKRATE_ERROR     1     /* peakrate bad*/
#define  ATM_AVGRATE_ERROR      2     /* average bad*/
#define  ATM_BURST_ERROR        4     /* burst value bad */
#define  ATM_OUT_OF_RATE_QUEUES 8     /* platform has no more rate queues*/

typedef enum atm_qos_t_ {
    ATM_QOS_BEST_EFFORT = 0,
    ATM_QOS_CLASS_A,
    ATM_QOS_CLASS_B,
    ATM_QOS_CLASS_C,
    ATM_QOS_CLASS_D
    } atm_qos_t;

typedef struct atm_userCellRate_t_ {
    long PeakClp0;     	     /* Peak Cell Rate (CLP = 0) */
    long PeakClp01;          /* Fwd Peak Cell Rate (CLP = 0 + 1) */
    long SusClp0;           /* Fwd Sustainable Cell Rate (CLP = 0) */
    long SusClp01;          /* Fwd Sustainable Cell Rate (CLP = 0 + 1) */
    long MaxBurstClp0;      /* Fwd Max Burst Size (CLP = 0) */
    long MaxBurstClp01;     /* Fwd Max Burst Size (CLP = 0 + 1) */
    long bestEffortIndicator;  /* Best Effort Indicator */
    int Tag;                 /* Traffic Mgmt Options Indicator - Fwd Tag */
} atm_userCellRate_t;

typedef struct atm_trParamQos_t_ {
    atm_qos_t 		qos;
    atm_userCellRate_t	usr_cellrate;
} atm_trParamQos_t;
    

#define	IS_VC_HOLD(a)		((a) && ((a)->flags & VC_FLAG_HOLD))
#define	IS_VC_DELETED(a)        ((a) && ((a)->flags & VC_FLAG_DELETED))
#define IS_PVC_DOWN(a)          ((a) && ((a)->flags & VC_FLAG_PVC_DOWN))
#define IS_ATMVC_SVC(vc)	((vc)->flags & VC_FLAG_SVC)
#define IS_ATMVC_PVC(vc)	(!((vc)->flags & VC_FLAG_SVC))
#define IS_ATMIF_AIP(hwidb)	((hwidb) && (hwidb)->type == IDBTYPE_AIP)

#define	ISVCBROADCAST(a)	((a) && ((a)->flags & VC_FLAG_BROADCAST))
#define	ISVCACTIVE(a)		((a) && ((a)->flags & VC_FLAG_ACTIVE))
#define	ISVCAAL34(a)		((a) && ((a)->etype  == ATM_ENCTYPE_SMDS))
#define	ISVCAAL5SNAP(a)		((a) && ((a)->etype == ATM_ENCTYPE_SNAP))
#define	ISVCAAL5NLPID(a)	((a) && ((a)->etype == ATM_ENCTYPE_NLPID))
#define	ISVCILMI(a)		((a) && ((a)->etype == ATM_ENCTYPE_ILMI))
#define ISVCAAL5LANE(a)		((a) && (((a)->flags & \
					  VC_FLAG_AAL_MASK) == ATM_VC_AAL5LANE))

#define	HASHVC(a, vc)	((vc) % (a)->vchash_buckets)

#define	ATM_CFG_MODE_AAL5	1		/* AAL5 support */
#define	ATM_CFG_MODE_AAL34	2		/* AAL3/4 support */
#define	ATM_CFG_DELAY_TIME	2		/* Config delay time (sec) */
#define	ATM_VC_MINRATE		56		/* Minimum rate(Kbps) */
#define	ATM_VC_MAXRATE		(155 * 1024)	/* Maximum rate(Kbps) */

#define ATM_PLIM_INVALID	0x0
#define ATM_PLIM_4B5B		0x1
#define ATM_PLIM_SONET		0x2
#define ATM_PLIM_E3		0x3
#define ATM_PLIM_DS3		0x4
#define ATM_PLIM_E1		0x5
#define ATM_PLIM_DS1		0x6
#define ATM_PLIM_8B10B		0x7

#define ATM_RATEQUEUE_MAX       8		/* Max # RATEQs supported */
#define ATM_VC_RATEQ_SHIFT      9               /* << value for position */
#define ATM_VC_RATEQ0           0x0000          /* High pri. queue 0 */
#define ATM_VC_RATEQ1           0x0200          /* High pri. Queue 1 */
#define ATM_VC_RATEQ2           0x0400          /* High pri. queue 2 */
#define ATM_VC_RATEQ3           0x0600          /* High pri. queue 3 */
#define ATM_VC_RATEQ4           0x0800          /* Low pri. queue 0 */
#define ATM_VC_RATEQ5           0x0A00          /* Low pri. queue 1 */
#define ATM_VC_RATEQ6           0x0C00          /* Low pri. queue 2 */
#define ATM_VC_RATEQ7           0x0E00          /* Low pri. queue 3 */
#define ATM_RATEQ_MAX           155		/* 155Mbps max. */
#define ATM_RATEQ_MIN           1		/* 1Mbps min. */

#define	PLIM_4B5B_RATE		100		/* 100 Mbps for TAXI */
#define	PLIM_SONET_RATE		155		/* 155 Mbps for 8B10B */
#define	PLIM_E3_RATE		34		/* 34 Mbps for E3 */
#define	PLIM_DS3_RATE		45		/* 45 Mbps for DS3 */
#define	PLIM_E1_RATE		2		/* 2 Mbps for E1 */
#define	PLIM_DS1_RATE		(3/2)		/* 1.5 Mbps for DS1 */

enum atm_media {
  ATM_MEDIA_UNKNOWN,
  ATM_MEDIA_COAX,             /* coaxial cable */
  ATM_MEDIA_SINGLE_FIBER,     /* single mode fiber */
  ATM_MEDIA_MULTI_FIBER,      /* multi mode fiber */
  ATM_MEDIA_STP,              /* shielded twisted pair */
  ATM_MEDIA_UTP,              /* unshielded twisted pair */
};

enum atm_loop {
  ATM_LOOP_NONE,				/* No loopback */
  ATM_LOOP_LINE,				/* Loopback at bit level */
  ATM_LOOP_PAYL,				/* Loopback at payload level */
  ATM_LOOP_CELL,				/* Loopback at cell level */
  ATM_LOOP_DIAG,				/* Internal diag loopback */
  ATM_LOOP_TEST					/* Special test loopback */
};

typedef struct	atm_config_ {
    ushort			mtu;		/* Maximum buffer size */
    ushort			max_intf_vcs;   /* Max. no of TX VCs */
    ushort			mode;		/* 1=AAL5, 2=AAL34, 3=both */
    ushort			rx_buff;	/* # of receive buffers */
    ushort			tx_buff;	/* # of transmit buffers */
    ushort			exq;		/* Exception Q length */
    ushort			raw_queue_size;	/* Raw queue length */
    ushort			rt_vc_per_vp;	/* VCIs per VPIs */
    ushort			vp_filter;	/* VPI filter */
    ushort			rx_vc;		/* No. of RX VCs */
    ushort			rateq[ATM_RATEQUEUE_MAX]; /* Rate Q defs. */
    ushort			mid_per_vc;	/* MIDs per VC */
    ushort			max_vpi_bits;	/* Number of VPI bits */
    ushort			max_vci_bits;	/* Number of VCI bits */
} atm_config;

typedef struct atm_def_config_ {
    ushort			rt_vc_per_vp;	/* VCIs per VPIs */
    ushort			max_vpi_bits;	/* Number of VPI bits */
    ushort			max_vci_bits;	/* Number of VCI bits */
    ushort                      max_burst;      /* Number of burst cells */
    ushort                      min_peak_rate;  /* Minimum peak rate */
} atm_def_config;

#define	ATM_HASH_DEFAULT	256		/* Default HASH entries */
#define	ATM_BAD_VC(atm_db, vcnum)	(!(atm_db) ||			\
				 (vcnum) >= (atm_db)->config.max_intf_vcs)
#define ATM_OUT_OF_VCDS     ((vcd_t) (~0)) /* all bitss set in unsigned word*/

#define	ATM_DEFAULT_MTU		4470
#define	ATM_BUFFERSIZE_WORDS	(btow(ATM_DEFAULT_MTU+ sizeof(atm_aal5snap_t)))

#define	MAX_ATM_INTFC		16		/* Max. no. of ATM intfcs. */
#define MAX_DEBUG_FILTER_TYPES   2               /* packet or OAM */
#define ATM_PACKET_FILTERING     0               /* filtering packets */
#define ATM_OAM_FILTERING        1               /* filtering oam */


typedef	void	(*rst_t)(hwidbtype *);
typedef	void	(*love_note_t)(cdbtype *, ushort);
typedef	void	(*rawinput_t)(hwidbtype *, paktype *);
typedef	unsigned (*encapsulate_t)(atm_db_t *, paktype *, vc_info_t *, ushort);
typedef	boolean	(*setupvc_t)(hwidbtype *, vc_info_t *);
typedef	boolean	(*teardownvc_t)(hwidbtype*, vc_info_t *);
typedef	boolean	(*config_t)(hwidbtype *);
typedef atm_setup_status_t (*setup_rateq_t)(hwidbtype *, ushort);
typedef boolean	(*getvc_stats_t)(hwidbtype *, vc_info_t *);
typedef ushort *(*sniff_t)(hwidbtype *, paktype *pak, uchar *,
                           ushort *, int, int);
typedef uint (*qos_param_verify_t)(hwidbtype *,unsigned peakrate,
				      unsigned avgrate,unsigned burstcells);


typedef	struct	intfc_functions_t_ {
    rst_t			reset;		/* Reset intfc. */
    love_note_t			love_note;	/* Love Note handling */
    encapsulate_t		encapsulate;	/* Add intfc. encaps. */
    setupvc_t			setupvc;	/* Make a VC */
    teardownvc_t		teardownvc;	/* Kill a VC */
    config_t			config;		/* Config the intfc. */
    setup_rateq_t		config_rateq;	/* config the rate-q */
    rawinput_t			rawinput;	/* OAM/ILMI cell processing */
    periodic_t			periodic;	/* Periodic function */
    getvc_stats_t		getvc_stats;	/* Update AS packet nos. */
    sniff_t                     sniff;          /* "sniff" data from IO mem */
    qos_param_verify_t          qos_params_verify; /* platform check of qos
						     paramters */
} intfc_func_t;

typedef struct	atm_MIB_stats_t_	{
    unsigned			ifInOctets;	/* Bytes in */
    unsigned			ifOutOctets;	/* Bytes out */
    unsigned			ifInDiscards;	/* 0 */
    /*
     * the rest have been deleted as they are unsupported in the codebase
     */
} atm_MIB_stat_t;


/* to save time...the LEC MIB fills in a struct of the ATM IF-MIB
 * counters that are supported...used in the atm_getvc_full_counts
 * function because the atm_getvc_counts function only returns 
 * inpkts and outpkts. The atm_MIB_stat_t structs do not get
 * filled in at this time
 */
typedef struct	atm_real_stats_t_	{
    unsigned			inOctets;
    unsigned			outOctets;
    unsigned			inPkts;
    unsigned			outUcastPkts;
    unsigned			outNUcastPkts;
} atm_real_stats_t;

enum	Config_Status {
	ATM_CONFIG_OK,
	ATM_CONFIG_CHANGED,
	ATM_CONFIG_ENABLE,
	ATM_CONFIG_IN_PROGRESS,
	ATM_VCS_NOT_UP,
	ATM_VCS_UP,
};

/*
 * Define different RATE_Q states:
 *	RATE_QUEUE_DISABLED	-	RateQ is disabled
 *	RATE_QUEUE_PERMANENT	-	RateQ created with config. command
 *	RATE_QUEUE_DYNAMIC	-	RateQ created dynamically
 */
enum	RATE_QUEUE_STATE	{
	RATE_QUEUE_DISABLED,
	RATE_QUEUE_PERMANENT,
	RATE_QUEUE_DYNAMIC
};

typedef	struct rateq_info {
	enum	RATE_QUEUE_STATE	state;
	uint				rate;	/* Speed of this rate-queue */
	ushort				num_vc;	/* Num VCs on this rateQ */
} rateq_info_t;

typedef struct rateq_param_t_ {
    ushort			rq_id;
    enum  RATE_QUEUE_STATE	rq_state;
    uint			rq_rate;
} rateq_param_t;
    

/* Additions to atm_db_t for ilmiReg */
#define ILMI_PREFIX_SIZE 13
#define ILMI_ESI_OFFSET ILMI_PREFIX_SIZE 
#define ILMI_ESI_SIZE 7
#define ILMI_ADDR_SET_LEN 34
#define OID_HDR_SIZE 14

/* Structure attached to atmidb for each NSAP to LISTEN to */
typedef struct atmSig_nsap_info_ {
   struct atmSig_nsap_info_ *next;
   idbtype *idb;                      /* port to listen on */
   hwaddrtype  nsap;                  /* dest Atm addr to look for in Setup */
   void (*sig_user_callback)(void *); /* sig user's callback function */
   /* Additions for ILMI */
   boolean useIlmiPrefix;             /* set if ilmi (ATM_ESI) */
   boolean ilmiPrefixSet;             /* set if Prefix configured */
   boolean ilmiAddrRegistered;        /* set if addr registered with sig */
} atmSig_nsap_info;

#define	VCD_UPDATE_INTERVAL	(30 * ONESEC)	/* Set per-vc update */


struct	atm_db_t_	{
    vc_info_t                   **vc_hash_tbl;	/* VC hash table */
    unsigned			vchash_buckets;	/* Buckets in VC hash table */
    hwidbtype			*hwidb;		/* HWIDB for this struct. */
    struct 	vc_info_t_	*inarp_vc;	/* Ptr. to first InARP VC */
    short			num_vc;		/* # of active VCs */
    vcd_t			last_vcd;	/* # of last assigned VCD */
    vcd_t 			ilmi_vc;	/* VCD of ILMI VC */
    unsigned			flags;		/* FLAGS */
#define	ATM_FLAG_OAMF5		0x1		/* Support OAM(F5) cells */
#define	ATM_FLAG_OAMF4		0x2		/* Simulate OAM(F4) cells */
#define	ATM_FLAG_PVC		0x4		/* PVC mode */
#define	ATM_FLAG_UP		0x8		/* Card is init'ed */
#define	ATM_FLAG_DS3SCRAM	0x10		/* DS3 Scramble enable */
#define ATM_FLAG_FLUSH_CACHE	0x20 		/* We must flush AR caches */
#define ATM_FLAG_ARP_RESET	0x40 		/* We must call arp reset */
#define	ATM_FLAG_DB_INUSE	0x8000		/* This DB is in use */

    ulong			idle_time;	/* mills an SVC can be idle */
    ulong			mema_cmd;	/* MEMA addr. for cmds. */
    enum 	Config_Status	config_status;	/* Config change */
    sys_timestamp		config_time;	/* Time remaining to config.*/
    uint                        configs_pending; /* configs remaining before
						    safe to create vc */
    sys_timestamp		vcd_update;	/* Last per-vc update time */
    ushort			plimtype;	/* Type of PLIM in use */
    ushort			plimrate;	/* Max speed in Mbps. */
    ushort                      burstcells_factor; /* factor to account for
						      platform differences
						      in burst size
						      resolution */
    enum atm_media 		medium;     	/* physical medium */
    uchar			ds3_framing; /* current ds3 framing mode */
    uchar			ds3_def_framing; /* default ds3 framing mode */
    uchar			e3_framing; /* current e3 framing mode */
    uchar			e3_def_framing;	/* default e3 framing mode */
    unsigned			txclock;	/* TX clock source */
    unsigned			sonetmode;	/* SONET mode */
    ushort			config_delay;	/* Config reset delay (sec) */
    ushort			ratequeue_no;	/* # of rate queues */
    rateq_info_t		rateq[ATM_RATEQUEUE_MAX]; /* RateQ Info. */
    intfc_func_t		funcs;		/* Functions for the intfc. */
    atm_config			config;		/* Config. stuff */
    atm_def_config		def_config;	/* Platform specific defaults*/
    boolean                     mtu_reject_call; /* reject calls on mismatch
						    mtu */
    struct      vc_info_t_      *fast_vc_info;  /* for fastswitching */
    mapclass_type               *def_class;     /* config default qos for i/f*/
/* 
 * Signalling details.
 */
   /* hwddrtype         nsap;            NSAP addr for this interface */
    uchar      		atmsig_retry_cnt; /* ATM Signalling retry count */
    ushort              sig_vcnum;      /* signalling vc fot this port */
    ulong               timer_t303;     /* Setup msg sent timer */
    ulong               timer_t309;     /* re-connect timer */
    ulong               timer_t310;     /* Call Procdng msg sent timer */
    ulong               timer_t313;     /* Connect msg sent timer */
    ulong               timer_t322;     /* Status Enq msg sent timer */
    ulong               timer_t308;     /* Release msg sent timer */
    ulong               timer_t316;     /* Restart msg sent */
    ulong               timer_t398;     /* Drop Party msg sent */
    ulong               timer_t399;     /* Add Party msg sent */
    atm_uniVersionType  uni_version;    /* UNI Version 3.0/3.1 */
    ushort              uni_version_init;
    ushort              ignore_peer_uni;     /* ignore peer uni if true */
    ushort              uni_side;       /* UNI Side user/network */
    ushort              conn_pending;   /* # of connections pending */
    ushort              max_conn_pending; /* max # of connections pending */
    ulong               bc_interval;    /* Broadcast interval */
    boolean             atmsig_tr_shape_strict;/* ATM strict traffic shaping flag*/
                                         /* TRUE = release SVC if driver can't   */
                                         /*  install VC with signalled parameters*/
/*
 * The infamous counters
 */
    unsigned		inpkts;		/* Packets received */
    unsigned		outpkts;	/* Packets sent */
    unsigned		infast;		/* Fastswitched in */
    unsigned		outfast;	/* Fastswitched out */
    unsigned		cur_status;	/* Current command status */
    unsigned            out_ts_drop;    /* traffic shaping or invalid encap */
    atm_MIB_stat_t	aal5;		/* AAL5 counters */
    atm_MIB_stat_t	cell;		/* CELL counters */
/*
 * Pointer to NSAP info structure
 */
    atmSig_nsap_info    *nsap_list_head;/* head of list of NSAP info structs */
    boolean             promiscuous_nsap;
    void                (*promiscuous_sig_user_callback)(void *); /*callback */

/*
 * ILMI Related
 */
    boolean		address_reg;
    ulong		ilmipoll_interval;
    ipaddrtype		peeruni_ip;
    char		*peeruni_ifname;
    atm_uniVersionType	peeruni_version;
    boolean             disable_ilmi_traps;

    /* additions to atm_db_t for ESI with ILMI */
    uchar		ilmiPrefix[ILMI_PREFIX_SIZE]; /* ilmi prefix */
    boolean		ilmiPrefixValid;    /* ILMI prefix valid flag */
    int	check_nsap; 	   /* used by 1577 code to speed periodic() handling */
};

typedef struct	atm_map_t_	{
    struct	atm_map_t_	*next;
    hwidbtype			*idb;		/* Associated IDB */
    boolean			broadcast;	/* BROADCAST set? */
    unsigned			flags;		/* See FLAGS */
    unsigned			vcnum;		/* VC no. for map */
    unsigned			addrlen;	/* Address length */
    uint			protocol;	/* Protocol type */
    addrtype			address;	/* Protocol address */
} atm_map_t;

/*
 * ATM static map parameters
 */
#define ATM_FWPKRATE0  -1 
#define ATM_BKPKRATE0  -1 
#define ATM_FWPKRATE1  -1 
#define ATM_BKPKRATE1  -1 
#define ATM_FWSTRATE0  -1 
#define ATM_BKSTRATE0  -1 
#define ATM_FWSTRATE1  -1 
#define ATM_BKSTRATE1  -1 
#define ATM_FWBSIZE0   -1 
#define ATM_BKBSIZE0   -1 
#define ATM_FWBSIZE1   -1 
#define ATM_BKBSIZE1   -1 
#define ATM_TRAFMGMT   -1 
#define ATM_BLLIE      -1

typedef struct atm_params_type_ {
    boolean broadcast;
    boolean aal5mux;
    struct atmSig_smap_ *smap;
    struct atmSig_smap_ *bc_smap;
    int layer2id;
    long fwpkrate0;
    long bkpkrate0;
    long fwpkrate1;
    long bkpkrate1;
    long fwstrate0;
    long bkstrate0;
    long fwstrate1;
    long bkstrate1;
    long fwbsize0;
    long bkbsize0;
    long fwbsize1;
    long bkbsize1;
    long trafmgmt;
} atm_params_type;

typedef	struct	atm_encap_ {
    vcd_t			vcnum;		/* VC to xmit/rcv on */
    ushort			enctype;	/* Encapsulation type */
    uchar			data[0];
} atm_encap_t;

typedef	struct	atm_aal5mux_ {
    atm_encap_t			encap;		/* Basic encap stuff */
} atm_aal5mux_t;

typedef	struct	atm_aal5snap_ {
    atm_encap_t			encap;		/* Basic encap stuff */
    snap_hdr			snap;		/* SNAP block */
    uchar			data[0];
} atm_aal5snap_t;

typedef struct  atm_nlpid_snap {
    uchar  control;             /* frame type */
    uchar  oui[3];              /* snap extension */
    ushort type;                /* ethernet type field */
    uchar  data[0];
} atm_nlpid_snap_t;

typedef struct  atm_aal5nlpid_ {
    atm_encap_t                 encap;
    ushort                      nlpid;
    uchar			data[0];
} atm_aal5nlpid_t;

typedef struct  atm_aal5frnlpid_ {
    atm_encap_t                 encap;
    ushort                      dlci;
    ushort                      nlpid;
    uchar			data[0];
} atm_aal5frnlpid_t;

typedef struct  atm_bridge_ether_hdr_ {
    atm_encap_t                 encap;
    uchar  llc[3];              /* always AA AA 03 */
    uchar  oui[3];              /* always 00 80 C2 */
    ushort pid;                 /* encapsulated media type */
    ushort pad;                 /* pad 2 bytes */
    uchar data[0];
} atm_bridge_ether_hdr;

typedef struct  atm_bridge_other_hdr_ {
    atm_encap_t                 encap;
    uchar  llc[3];              /* always AA AA 03 */
    uchar  oui[3];              /* always 00 80 C2 */
    ushort pid;                 /* encapsulated media type */
    ushort pad;                 /* pad 2 bytes */
    union {
      ushort non_ether_pad;              /* pad for token ring & fddi is 3 */
      uchar  pad_FC[2];                  /* plus 1 for the FC byte */
    } pad2;
    uchar data[0];
} atm_bridge_other_hdr;

typedef struct atm_aal5lane_ {
    atm_encap_t			encap;
    ushort			lecid;
    uchar			data[0];
} atm_aal5lane_t;

typedef struct atm_aal5_cspdu_trail_ {
    uchar			uu;		/* User-User information */
    uchar			cpi;		/* Common Part Indicator */
    ushort			length;		/* CPAAL5_SDU length */
    ulong			crc32;		/* CPAAL5_PDU CRC-32 */
} atm_aal5_cspdu_trail_t;

#define	ATM_ENCAP_HDRBYTES	(sizeof(atm_encap_t))
#define	ATM_MUX_HDRBYTES	0
#define	ATM_NLPID_HDRBYTES	(sizeof(atm_aal5nlpid_t) - ATM_ENCAP_HDRBYTES)
#define	ATM_FRNLPID_HDRBYTES	(sizeof(atm_aal5frnlpid_t)-ATM_ENCAP_HDRBYTES)
#define	ATM_SAP_HDRBYTES	3
#define	ATM_SNAP_SAP_ADJBYTES	5	/* SNAP -> SAP adjustment bytes */
#define	ATM_SNAP_HDRBYTES	SNAP_HDRBYTES
#define	ATM_SMDS_L3_HDRBYTES	(sizeof(smdstype))
#define	ATM_SMDS_HDRBYTES	(ATM_SMDS_L3_HDRBYTES + SNAP_HDRBYTES)
#define	ATM_NLPID_SNAP_HDRBYTES	(sizeof(atm_nlpid_snap_t))
#define	ATM_AAL5_CSPDU_TRLBYTES	(sizeof(atm_aal5_cspdu_trail_t))

#define ATM_BRIDGE_ETHER_ENCAPBYTES   sizeof(atm_bridge_ether_hdr)
#define ATM_BRIDGE_OTHER_ENCAPBYTES   sizeof(atm_bridge_other_hdr)
#define ATM_BRIDGE_ETHER_ENCAPWORDS   (ATM_BRIDGE_ETHER_ENCAPBYTES / 2)
#define ATM_BRIDGE_OTHER_ENCAPWORDS   (ATM_BRIDGE_OTHER_ENCAPBYTES / 2)
#define ATM_BRIDGE_PID_OFFSET_BYTE    (ATM_BRIDGE_ETHER_ENCAPBYTES - 4)

#define	ATM_SNAP_ENCAPBYTES	(ATM_ENCAP_HDRBYTES + ATM_SNAP_HDRBYTES)
#define	ATM_SAP_ENCAPBYTES	(ATM_ENCAP_HDRBYTES + ATM_SAP_HDRBYTES)
#define	ATM_MUX_ENCAPBYTES	(ATM_ENCAP_HDRBYTES + ATM_MUX_HDRBYTES)
#define	ATM_SMDS_ENCAPBYTES	(ATM_ENCAP_HDRBYTES + ATM_SMDS_HDRBYTES)
#define ATM_NLPID_ENCAPBYTES    (ATM_ENCAP_HDRBYTES + ATM_NLPID_HDRBYTES)
#define	ATM_FRNLPID_ENCAPBYTES	0		/* Unsupported */
#define	ATM_MAX_ENCAPBYTES	(ETHER_ARPA_ENCAPBYTES + ATM_SMDS_HDRBYTES)

/*
 * LANE Data packets are detected in atm_parse_packet and treated as if
 * they are ethernet (ignoring the atm_encap_t).
 */
#define ATM_LANE_LECID_ENCAPBYTES	sizeof(ushort)
#define ATM_LANE_CONTROL_ENCAPBYTES	ATM_ENCAP_HDRBYTES
#define ATM_LANE_DATA_ENCAPBYTES	(ATM_ENCAP_HDRBYTES + \
					 ATM_LANE_LECID_ENCAPBYTES + \
					 ETHER_ARPA_ENCAPBYTES)

#define ATM_AAL5_LANE_ENCAPBYTES        (ATM_ENCAP_HDRBYTES + \
                                         ATM_LANE_LECID_ENCAPBYTES)
 
#define ATM_AAL5_LANE_ENCAPWORDS        (ATM_AAL5_LANE_ENCAPBYTES / 2)


#define ATM_NLPID_SNAP_ENCAPBYTES (sizeof(atm_nlpid_snap_t))

#define atm_snapvc	e.encaps[ENCAPWORDS - ATM_SNAPSIZE]
#define	atm_nlpidvc	e.encaps[ENCAPWORDS - ATM_NLPIDSIZE]
#define	atm_frnlpidvc	e.encaps[ENCAPWORDS - ATM_FRNLPIDSIZE]
#define	atm_smdsvc	e.encaps[ENCAPWORDS - ATM_SMDSSIZE]
#define	atm_muxvc	e.encaps[ENCAPWORDS - ATM_MUXSIZE]

/*
 * Constants for the support of RFC1483 encaps
 */
#define	ATM_NLPID_CTL		0x0003	/* NLPID CTL value */
#define	ATM_NLPID_PID_IP	0xcc
#define	ATM_NLPID_PID_SNAP1	0x00
#define	ATM_NLPID_PID_SNAP0	0x80
#define	ATM_NLPID_PID_CLNP	0x81
#define	ATM_NLPID_PID_ESIS	0x82
#define	ATM_NLPID_PID_ISIS	0x83
#define	ATM_NLPID_PID_ISO	0x84
#define ATM_NLPID_PID_Q2931     0x09

#define ATM_NLPID_IETF_IP       ((ATM_NLPID_CTL << 8) | ATM_NLPID_PID_IP)
#define ATM_NLPID_SNAP1PAD      ((ATM_NLPID_CTL << 8) | ATM_NLPID_PID_SNAP1)
#define ATM_NLPID_SNAP0PAD      ((ATM_NLPID_CTL << 8) | ATM_NLPID_PID_SNAP0)
#define ATM_NLPID_CLNP          ((ATM_NLPID_CTL << 8) | ATM_NLPID_PID_CLNP)
#define ATM_NLPID_ESIS          ((ATM_NLPID_CTL << 8) | ATM_NLPID_PID_ESIS)
#define ATM_NLPID_ISIS          ((ATM_NLPID_CTL << 8) | ATM_NLPID_PID_ISIS)
#define ATM_NLPID_ISO          	((ATM_NLPID_CTL << 8) | ATM_NLPID_PID_ISO)
#define ATM_NLPID_Q2931        	((ATM_NLPID_CTL << 8) | ATM_NLPID_PID_Q2931)

#define ATM_NLPID_DEF(type) ((type == LINK_IP) || (type == LINK_CLNS))

/*
 * Generic accounting type macros
 */
#define	ATM_OUTFAST(hwout) {				\
    atm_db_t *atm = hwout->atm_db;			\
    atm->outfast++;					\
    atm_counters[ATM_OUTPUT]++;				\
    }
#define	ATM_INFAST(hwin) {				\
    atm_db_t *atm = hwin->atm_db;			\
    atm->infast++;					\
    atm_counters[ATM_INPUT]++;				\
    }
#define	ATM_VCINFAST(hwin, inreg) {			\
    atm_db_t *atm = hwin->atm_db;			\
    vc_info_t *vc = NULL;				\
    unsigned vcnum;					\
    inreg->argreg = hwin->rxoffset;    		\
    inreg->argreg = MCI_CMD_RX_SELECT;			\
    vcnum = inreg->readshort;				\
    vc = atm_getvc(atm, vcnum);		                \
    if(vc) vc->infast++;				\
    }
#define	ATM_VCOUTFAST(hwout, vcnum) {			\
    atm_db_t *atm = hwout->atm_db;			\
    vc_info_t *vc = atm_getvc(atm, vcnum);	        \
    if(vc) vc->outfast++;				\
    }
#define	ATM_INAS(atm, vc) {						\
    ulong in_as;							\
        if (vc->in_pkts) {						\
	    vc->total_in_pkts += vc->in_pkts;			        \
	    in_as = vc->total_in_pkts - 				\
		(vc->inpkts + vc->infast + vc->in_as);    		\
	    if ((long)in_as > 0) {					\
                atm_counters[ATM_INPUT] += in_as;			\
                atm->inpkts += in_as;					\
                vc->in_as += in_as;			                \
            }								\
        }								\
    }
#define ATM_OUTAS(atm, vc) {                                            \
    ulong out_as;                                                       \
        if (vc->out_pkts) {                      			\
	    vc->total_out_pkts += vc->out_pkts;				\
	    out_as = vc->total_out_pkts -				\
	    (vc->outpkts + vc->outfast + vc->broadcasts + vc->out_as);  \
	    if ((long)out_as > 0) {				        \
                atm_counters[ATM_OUTPUT] += out_as;        		\
                atm->outpkts += out_as;                    		\
                vc->out_as += out_as;                                   \
            }                                                           \
        }                                                               \
    }



extern ulong atm_counters[];

extern idbtype *atm_debug_swidb[MAX_DEBUG_FILTER_TYPES];
extern vcd_t atm_debug_vc;

/*
 * Basic inlines for cleanliness & speed. 
 */
static inline void 
atm_in_inc(atm_db_t *atm, vc_info_t *vc)
{
    atm_counters[ATM_INPUT]++;
    atm->inpkts++;
    vc->inpkts++;
}

static inline void
atm_out_inc(atm_db_t *atm, vc_info_t *vc)
{
    atm_counters[ATM_OUTPUT]++;
    atm->outpkts++;
    vc->outpkts++;
}

/*
 * This routine returns a vc_info_t pointer corresponding to its wavl node
 * i.e get to the start of vc_info_t struct.
 */
static inline vc_info_t *
wavl_to_vcinfo (wavl_node_type *wavl)
{
    if (wavl == NULL)
        return(NULL);
    return (vc_info_t *)((char *)wavl - (int)(&((vc_info_t *)0)->wavl));
}

/*
 * Simply for clarity, returning void * to be used as a wavl_init argument. 
 */
static inline void *
wavl_to_vcinfo_void (wavl_node_type *wavl)
{
    return wavl_to_vcinfo(wavl);
}

/*
 * This routine checks if the passed vc is a candidate for glomming, i.e if
 * it is a point-to-point SVC with either ATM_VC_AAL5SNAP or ATM_VC_AAL5MUX
 * encapsulaton.
 */
static inline boolean
atm_is_glommable_vc(vc_info_t *vc) 
{
    if (!(vc->flags & VC_FLAG_SVC))
	return(FALSE);
    if (vc->flags & VC_FLAG_MULTIPOINT)
	return(FALSE);
    if (((vc->flags & VC_FLAG_AAL_MASK) != ATM_VC_AAL5SNAP) &&
	    ((vc->flags & VC_FLAG_AAL_MASK) != ATM_VC_AAL5MUX))
	return(FALSE);

    return(TRUE);
}

/*
 * for fastswitching
 */

static inline vc_info_t *
atm_getvc_inline (atm_db_t *atm, vcd_t vc)
{
    vc_info_t *cur;

    if (!atm->vc_hash_tbl)
	return(NULL);
    cur = atm->vc_hash_tbl[HASHVC(atm, vc)];
    while (cur) {
    	if (cur->vc == vc) {
	    if (ISVCACTIVE(cur))
		return(cur);
	    else
		return(NULL);
	}
	cur = cur->next_vc;
    }
    return(NULL);
}

static inline boolean  is_atm_ds3 (hwidbtype *hwidb)
{
    return (hwidb && hwidb->atm_db->plimtype == ATM_PLIM_DS3);
}

static inline boolean  is_atm_e3 (hwidbtype *hwidb)
{
    return (hwidb && hwidb->atm_db->plimtype == ATM_PLIM_E3);
}

static inline boolean  is_atm_sonet (hwidbtype *hwidb)
{
    return (hwidb && hwidb->atm_db->plimtype == ATM_PLIM_SONET);
}

/*
 * Extern declarations
 */
extern atm_db_t atm_db[];
extern boolean atm_event_debug;
extern boolean atm_ilmi_enabled;

extern vc_info_t *atm_getvc(atm_db_t *, vcd_t);
extern vc_info_t *atm_getvc_all(atm_db_t *, vcd_t);
extern vc_info_t *atm_getvc_deleted(atm_db_t *, vcd_t);
extern boolean atm_getvc_mib_id(vcd_t, hwidbtype*, long*, long*, long*);
extern void atm_getvc_counts(hwidbtype *, vcd_t, unsigned *, unsigned *);
extern boolean atm_getvc_full_counts(hwidbtype *, vcd_t, atm_real_stats_t  *);
extern void atm_cpcs_encap(paktype *, atm_db_t *, vc_info_t *, uchar
			   const *, hwaddrtype *, uchar l3pid);
extern boolean atm_saal_encap(paktype *, vc_info_t *);

extern boolean atm_ilmi_encap(paktype *);
extern boolean atm_interface_up(idbtype *);
extern void atm_interface_commands(parseinfo *);
extern void atm_show_packet(hwidbtype *, paktype *, boolean);
extern vc_info_t *atm_addr_lookup(idbtype *, atm_db_t *, staticmap_type *);
extern void atm_show_commands(parseinfo *);
extern void atm_debug_command(parseinfo *);
extern boolean atm_parse_proto_addr(parseinfo *, uint, addrtype *, uint);
extern boolean atm_add_smap(hwidbtype *, unsigned, addrtype *, unsigned, boolean);
extern void atm_remove_smap(hwidbtype *, addrtype *, unsigned);
extern void atm_parse_vc_stats(hwidbtype *, boolean, ulong *, ulong);
extern void atm_params_init(atm_params_type *);
extern vc_info_t *atm_vpivci_to_vc(atm_db_t *, unsigned, unsigned);
extern vc_info_t *atm_vpivci_to_vc_all(atm_db_t *, unsigned, unsigned);
extern boolean atm_inarp_on_aal(uint);
extern uint atm_get_aal34vcnum(idbtype *);

extern void atm_update_rateq(atm_db_t *, rateq_param_t *);
extern unsigned atm_rateq_ratesum(atm_db_t *);
extern ushort atm_calc_vpi_bits(ushort total_vpi_vci_bits,ushort max_vci_bits);
extern ushort atm_calc_vci_bits(ushort vc_per_vc);
extern ushort atm_calc_max_vpivci(ushort max_bits);
extern void (*atm_arp_start_func)(void);

/*
 * Signalling support routines
 */
extern void atm_remove_vc(hwidbtype *, unsigned);
extern void atm_activate_pvc(hwidbtype *,vc_info_t *);
extern void atm_inactivate_pvc(hwidbtype *,vc_info_t *);
extern vc_info_t *atm_create_vc(hwidbtype *, 
				vcd_t vcnum, 
				uint vpi,
				uint vci, 
				hwaddrtype call_addr, 
				uint qos, 
				uint rateq, 
				uint mtu, 
				uint flags, 
				uint protocol,
				user_vc_type_t user_vc_type,
				idbtype *swidb, 
				uint peak, 
				uint avg, 
				uint burst, 
				uint midlow, 
				uint midhigh,
				uint inarp,
				uint oam);

extern void atm_newconfig(atm_db_t *atm);
extern void atm_encaps_show_protocol_attr(idbtype *);
extern void atm_delete_multicast(idbtype *idb);
extern boolean atm_add_multicast(idbtype *idb,hwaddrtype *hwaddr);
extern vcd_t atm_getvcnum(hwidbtype *);
extern void atm_parser_init(void);
extern atm_multicast_t *atm_get_multicast(idbtype *idb);
extern void atm_hangup_vc(vc_info_t *);
extern void atm_map_command(parseinfo *);
extern void atm_mapclass_command(parseinfo *csb);
extern void atm_printnsap(uchar *, uchar *);
extern void atm_printprefix(uchar *, uchar *);
extern void atm_printesi(uchar *, uchar *);
extern char *atm_framing_string(hwidbtype *hwidb);

/*
 * ARP Server and Client support routines
 */
extern void atm_arp_serv_init(idbtype *, int);
extern void atm_arp_client_init(idbtype *, hwaddrtype *);
extern void atm_arp_serv_off(idbtype *);
extern void atm_arp_client_off(idbtype *, hwaddrtype *);
extern void atm_arp_serv_periodic(hwidbtype *);
extern void atm_arp_serv_show(idbtype *);

#endif __ATM_H__
