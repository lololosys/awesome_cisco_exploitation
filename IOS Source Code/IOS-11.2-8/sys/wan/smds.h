/* $Id: smds.h,v 3.4 1996/01/19 22:17:01 wmay Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/wan/smds.h,v $
 *------------------------------------------------------------------
 * smds.h -- Definitions for SMDS Protocol
 *
 * August 6 1990, Vicki Ralls
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smds.h,v $
 * Revision 3.4  1996/01/19  22:17:01  wmay
 * CSCdi45344:  ACL: input access lists are ignored on SMDS subinterfaces
 * Add include file for serial_inline; add sniff buffer for dest address
 *
 * Revision 3.3  1995/12/14  21:00:26  wmay
 * CSCdi45297:  bridging does not work over smds subinterfaces - create
 * registry to convert from smds destination address to swidb that the
 * address is for.  Also some minor cleanups to get rid of global routines
 *
 * Revision 3.2  1995/11/17  18:04:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:51:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:36:56  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/10/20  06:06:25  pst
 * CSCdi42495: Exorcise demons
 *
 * Revision 2.4  1995/09/15  19:22:45  lmercili
 * CSCdi34816:  add dynamic mapping for ipx/smds
 *
 * Revision 2.3  1995/09/09  00:55:04  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.2  1995/08/08  16:51:21  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.1  1995/06/07 23:19:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SMDS_H__
#define __SMDS_H__

#include "address.h"

#define SMDS_HEADER 32
#define SMDS_ADDRESS 0x0c03
#define SMDS_GROUP_TYPE 0xe
#define SMDS_INDIVIDUAL_TYPE 0xc
#define SMDS_LLC 4

static inline boolean SMDS_ISGROUP (uchar c)
{
    return((c >> 4) == SMDS_GROUP_TYPE);
}
static inline boolean SMDS_ISINDIV (uchar c)
{
    return((c >> 4) == SMDS_INDIVIDUAL_TYPE);
}

#define SMDS_TRAILER 4
#define SMDSBYTES 8
#define XHEL_SIZE 3
#define SMDSHASH 	512

#define SMDS_BAD_BASIZE		0
#define SMDS_BAD_XHEL		1
#define SMDS_INVLD_ADDR		2
#define	SMDS_BAD_BETAG		3
#define SMDS_INPUT		4
#define	SMDS_OUTPUT		5
#define SMDS_DXI_CMDFRDSU	6
#define SMDS_DXI_RSPTODSU	7
#define SMDS_DXI_CMDTODSU	8
#define SMDS_DXI_RSPFRDSU	9
#define SMDS_DXI_BADFRAME	10
#define SMDS_MAX_COUNTERS	11

#define	SMDS_MINPAKLEN		32
#define	SMDS_MAXPAKLEN		9224

#define	SMDS_BRIDGE_PAD_SIZE    2          /* 2 bytes */
#define SMDS_BRIDGE_PAD_FDDI_EXTRA	1  /* 1 extra byte for fddi */
#define SMDS_BRIDGE_PAD_FDDI_SIZE	3
#define	SMDS_BRIDGE_PAD_DATA    0

/*
 * Remote bridging over 802.6 
 */
#define SMDS_BRIDGE_OUI0	0x00
#define SMDS_BRIDGE_OUI1	0x80
#define SMDS_BRIDGE_OUI2	0xc2
#define	SMDS_BRIDGE_OUI		0x0080c2
#define	SMDS_BRIDGE_PID_8023FCS	0x1	/* 802.3 - FCS present */
#define	SMDS_BRIDGE_PID_8024FCS	0x2	/* 802.4 - FCS present */
#define	SMDS_BRIDGE_PID_8025FCS	0x3	/* 802.5 - FCS present */
#define	SMDS_BRIDGE_PID_FDDIFCS	0x4	/* FDDI - FCS present */
#define	SMDS_BRIDGE_PID_8026FCS	0x5	/* 802.6 - FCS present */
#define	SMDS_BRIDGE_PID_8029FCS	0x6	/* 802.9 - FCS present */
#define	SMDS_BRIDGE_PID_8023	0x7	/* 802.3 - no FCS */
#define	SMDS_BRIDGE_PID_8024	0x8	/* 802.4 - no FCS */
#define	SMDS_BRIDGE_PID_8025	0x9	/* 802.5 - no FCS */
#define	SMDS_BRIDGE_PID_FDDI	0xA	/* FDDI - no FCS */
#define	SMDS_BRIDGE_PID_8026	0xB	/* 802.6 - no FCS */
#define	SMDS_BRIDGE_PID_8029	0xC	/* 802.9 - no FCS */
#define	SMDS_BRIDGE_PID_BPDU	0xE	/* BPDUs */


/*
 *  map structure
 */

typedef struct smds_map_ {
    struct smds_map_ *next;
    addrtype dest_addr;
    idbtype *idb;
    ulong link;
    sys_timestamp time;
    uchar hardware[SMDSBYTES];		/* hardware address */
    boolean permanent;
    boolean broadcast;
    mgd_timer timer;
} smdsmaptype;

struct smdsaddr_ {
    smdsaddr *next;
    idbtype *idb;
    ulong link;
    uchar addr[SMDSBYTES];
    ipaddrtype ipsubnet;		/* Used for IP/ARP link types */
    ipaddrtype ipmask;		/* Used for IP/ARP link types */
};

/*
 * SMDS encapsulation definitions
 *
 * The defines below are mask tables for the extended portion.
 */
#define	SMDS_XHLPI_HLPI		0xfc
#define	SMDS_XHLPI_PL		0x3

#define	SMDS_XHEL_QOS		0xf0
#define	SMDS_XHEL_CIB		0x80
#define	SMDS_XHEL_HEL		0x7

/* Standard SMDS */
typedef  struct _smdsheader_ {
    uchar  smds_rsvd;
    uchar  smds_betag;
    ushort smds_basize;
    uchar  smdsdest[STATIONLEN_SMDS64];
    uchar  smdssrc[STATIONLEN_SMDS64];
    uchar  smds_xhlpi;
    uchar  smds_xhel;
    ushort smds_x;
    uchar  smds_hel[12];
    uchar  data[0];
} smdstype;

/* 
 * DXI 3.2 definitions
 */
#define DXI_HBEAT_SIZE	64	/* size of our heartbeat frames */

#define	DXI_LL_BITS	0xF8	/* Logical Link bits */
#define	DXI_LM		0x08	/* Link management LL */
#define	DXI_DATA	0x00	/* Data LL */
#define	DXI_S_BIT	0x04	/* S bit of address field */
#define	DXI_CR_BIT	0x02	/* CR bit of address field */
#define	DXI_AE_BIT	0x01	/* AE bit of address field */
#define	DXI_S_CR_AE	(DXI_S_BIT | DXI_CR_BIT | DXI_AE_BIT)
#define DXI_S_AE        (DXI_S_BIT | DXI_AE_BIT)
#define	DXI_RSPTODSU	(DXI_S_BIT | DXI_CR_BIT | DXI_AE_BIT)	/* 0x07	*/
#define	DXI_CMDTODSU	(DXI_S_BIT | DXI_AE_BIT)		/* 0x05 */
#define	DXI_CMDFRDSU	(DXI_CR_BIT | DXI_AE_BIT)		/* 0x03 */
#define	DXI_RSPFRDSU	(DXI_AE_BIT)				/* 0x01 */

#define	DXI_UI		0x03
#define	DXI_TEST	0xE3
#define	DXI_PF		0x10	/* Poll bit */
#define	DXI_PF_MASK	0xEF	/* Poll bit mask */

#define DXI_MIN_BEAT	5	/* min heartbeat timer value */
#define DXI_MAX_BEAT	30	/* max heartbeat timer value */
#define DXI_NO_ACK_TMR	5000	/* no acknowledgement timer */
/*
 * DXI struct
 */
typedef	struct	_dxi_	{
    uchar	addr;	
    uchar	control;
    ushort 	congest;
} dxitype;

/*
 * Doing a sizeof() a struct that overlays hardware or a packet is a bad idea,
 * so use a #define instead.  Different compilers may give you different
 * answers for sizeof().
 */

typedef	struct	_dxismds_	{
    dxitype	dxi;
    smdstype	smds;
} dxismds;

/*
 * smds DXI heartbeat
 */
typedef struct _dxiheartbeat_ {
        dxitype dxi_header;
        ulong   dxi_hbseq;
} dxiheartbeat;

typedef	struct	_ciscosmds_	{
    ushort	cisco_id;
    smdstype	smds;
} ciscosmds;

/* Standard SMDS Header Extension */
typedef	struct	_smdshe_	{
	uchar	smdshe_length;
	uchar	smdshe_type;
	uchar	smdshe_value;
}	smdshetype;

#define	SMDSHE_VERLEN	3
#define	SMDSHE_VERTYP	0
#define	SMDSHE_VERVAL	1

#define SMDS_CISCOBYTES   2	/* size of ciscotype struct */
#define SMDS_DXIBYTES     4	/* size of dxitype struct */
#define SMDS_SMDSBYTES   36 /* sizeof (smdstype) */
#define SMDS_BASIZEBYTES (SMDS_SMDSBYTES - 4)
#define SMDS_ENCAPBYTES1 (2+SMDS_SMDSBYTES)
#define SMDS_ENCAPBYTES2 (SMDS_ENCAPBYTES1+8+4+4+4)
/* SNAP (8), pad (3, rounded up to 4), trailer (4), and krock (4) */
#define SMDSMAXDGRAM 9188	       	/* max of spec -- not explicitly used */

#define SMDS_SAP_ENCAPBYTES (SMDS_SMDSBYTES + SAP_HDRBYTES)
#define SMDS_SNAP_ENCAPBYTES (SMDS_SMDSBYTES+ SNAP_HDRBYTES)

/*
 * smds PDB
 *
 * note that the fast_input_modelen should only be used on incoming packets
 * while fast_output_modelen should be used when dealing with caches and
 * output packets.  When running smds over MIP, these may or may not
 * include the MIP's board encapsulation depending on the platform as
 * follows:
 *                            c7000               rsp
 *     fast_input_modelen       +2                 -
 *     fast_output_modelen      +2                 +2
 *
 * Since RSP uses low-end switching/parsing code, we strip the MIP's
 * vc encaps off upon arrival, hence we do not adjust fast_input_modelen
 * to reflect the extra two bytes of board encaps (since we're removed
 * them).  The c7000, however, carries the two bytes of board encaps
 * around with the packet, so parse routines looking at the packet need
 * to know to skip them, and hence, we do adjust fast_input_modelen
 * by 2 on the c7000.
 *
 * On the output side, when dealing with route caches and fully
 * encapsulated output packets, both platforms have two bytes of 
 * board encaps, and so both reflect this by adding 2 to fast_output_modelen
 */
struct	smds_pdb_t_	{
	smdsaddr 	*smds_bcast;	/* Ptr. to broadcast table */
	boolean		necmode;	/* NEC mode enabled */
	boolean		dximode;	/* DXI 3.2 mode enabled */
	boolean		smds_enabled;	/* Enabled? */
	boolean		smds_heartbeat;	/* Heartbeat enabled? */
        ulong		smds_myseq;	/* hearbeat sequence number */
        ulong		smds_lastseen;	/* last hearbeat received sn */
	sys_timestamp	smds_timesent;	/* last heartbeat sent time */
	ulong		smds_dxihbsent; /* # of DXI hearbeat packets sent */
	ulong		smds_dxihbrcvd; /* # of DXI hearbeat packets rcvd */
	ulong		fast_input_modelen; /* mode len for fast switching */
	ulong		fast_output_modelen; /* mode len for fast switching */
	uchar		smds_hardware[8]; /* Individual SMDS addr */
	uchar		fast_smds_dest[8]; /* hes sniffed smds dest addr */
};

enum {
    MAP                 /* map timer  with context smdsmaptype */
};

#define GLEAN_TIMEOUT_DEF  (5*ONEMIN)

/*
 * SMDS software idb info
 */
struct smds_gleantype_    {
    ulong       timeout;   /* timeout value for gleaning */
    boolean     bcast;     /* TRUE if glean maps are bcast */
    uint        link;      /* protocol name specified in the glean command */
};



/* smds.c */
extern const uchar smds_bridge_oui[];

extern boolean smds_l3_encapsulate(paktype *, int, idbtype *, uchar const *);
extern void smds_input(hwidbtype *, paktype *);
extern long smds_getlink(paktype *);
extern smdsmaptype *smds_addr_enter(addrtype *, boolean, uchar *, 
				    idbtype *, uint, uint);
extern smdsmaptype *smds_addr_lookup(addrtype *, ulong);
extern smdsaddr *smds_bcast_enter(uchar *, idbtype *, uint, ipaddrtype, ipaddrtype);

extern uchar *smds_bcast_lookup(ulong, idbtype *, ipaddrtype, boolean);
extern void smds_bcast_delete(ulong, idbtype *, ipaddrtype);
extern boolean smds_bcast_valid(idbtype *, uchar *);
extern void smds_map_delete(smdsmaptype *);
extern void smds_delete_subif(idbtype *);
extern void smds_write_map(idbtype *, smdsmaptype *);
extern void nv_smds_bcast(idbtype *,char *);
extern void smds_hash_init(void);
extern void smds_periodic(hwidbtype *);
extern void show_smds_counters(void);
extern void show_smds_map(void);
extern void show_smds_packet(idbtype *, paktype *, int);
extern void smds_display_map(smdsmaptype *);
extern void smds_cstate(hwidbtype *);
extern void smds_display_bcast(idbtype *);
extern void display_smds(void);
extern uint xsnethash(addrtype *);
extern void smds_deencap(hwidbtype *);
extern boolean smdsequal(uchar *, uchar *);
extern void smds_interface(hwidbtype *);
extern void smds_lm_frame(idbtype *, paktype *);
extern void clear_smds_counters(hwidbtype *);
extern uchar *smds_bridge_datagram(paktype *, long);

/*
 * smds_chain.c
 */
extern void smds_parser_init(void);
#endif
