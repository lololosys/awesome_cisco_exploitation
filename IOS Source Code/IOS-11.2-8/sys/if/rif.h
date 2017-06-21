/* $Id: rif.h,v 3.6.44.3 1996/07/09 05:58:53 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/if/rif.h,v $
 *------------------------------------------------------------------
 * rif.h -- 802.5 Token Ring RIF cacheing definitions
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rif.h,v $
 * Revision 3.6.44.3  1996/07/09  05:58:53  ppearce
 * CSCdi59527:  inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.6.44.2  1996/05/17  11:23:30  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.32.3  1996/04/26  15:20:32  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6.32.2  1996/04/05  06:06:51  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.6.32.1  1996/04/03  14:38:46  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.6.44.1  1996/03/18  20:14:35  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.14.2  1996/02/20  00:57:37  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6.14.1  1996/01/24  22:18:59  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.6  1995/12/16  22:48:16  motto
 * CSCdi35413:  Alignment errors in R/SRB process level code
 *
 * Revision 3.5  1995/12/13  02:28:31  ppearce
 * CSCdi45473:  %SYS-3-SUPNONE: Registry 6 doesnt exist output during boot
 *   (1) Rename fddi_srb_in() to fddi_to_tr()
 *         to parallel the tr_to_fddi() routine that already exists
 *   (2) Remove srt_fddi_in() registry from srt/srt_registry.reg
 *         and add media_fddi_to_tr() registry into h/media_registry.reg
 *
 * Revision 3.4  1995/12/11  10:20:23  ppearce
 * CSCdi44095:  SRB proxy-explorer causes FDDI packet drops
 *   rif_explore() should call tr_to_media_out() rather than datagram_out()
 *     so that packets can be correctly formatted for output to FDDI
 *
 *     - Rename srb_out() to tr_media_out()
 *     - Change srb_out() to invoke tr_fddi_out() for FDDI output
 *     - Move adjust_pak() code into tr_fddi_out()
 *
 * Revision 3.3  1995/11/17  09:30:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:39:02  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:52:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/07  05:36:48  richl
 * CSCdi38315:  makefile rework
 *              move inlines to tring_inline and utilities to rif_util.c
 *
 * Revision 2.2  1995/07/18  06:46:28  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.1  1995/06/07 20:54:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __RIF_H__
#define __RIF_H__

#include "ieee.h"
#include "../srt/srb_sb.h"

/*
 * Misc. Bridge Definitions
 *
 * These definitions define essentially what a RIF's routing descriptor
 * (RD) look like.  That is how many bits are allocated to the different
 * parts.
 *
 * The max bridge number is 15 or 4 bits.  0 is not allowed for any values.
 * The max ring number is 4K-1 or 12 bits. 0 is not allowed.
 * The max routing descriptor directly determines how big the RIF is allowed
 * to be.  It can range from 2 (rif 6) to 14 (rif 30 bytes).  The default max
 * is 8 (rif 18) which is what IBM originally specified.
 *
 * Set MAX_MAXRD to 8 for IBM, 14 for IEEE 802.5.  If this changes a change
 * most likely needs to be done in packet.h as well.  This is an adjustment
 * for the packet encapsulation overhead.
 */

#define SRB_MIN_MAXRD   (SRB_MIN_MAXHOPS + 1)
#define SRB_DEF_MAXRD   (SRB_DEF_MAXHOPS + 1)
#define SRB_MAX_MAXRD   (SRB_MAX_MAXHOPS + 1)
#define SRB_MAXRIF	(SRB_MAX_MAXRD * 2 + 2)

struct srbroute_t_ {
    uchar rc_blth;
    uchar rc_dlf;
    ushort rd[2];
};

typedef struct _srbrd_t {
    ushort ring_num:12;
    ushort bridge_num:4;
} srbrd_t;

typedef struct _srbrif_t {
    uchar daddr[6];
    uchar saddr[6];
    uchar rc_blth;
    uchar rc_dlf;
    ushort rd[2];
} srbrif_t;
    
/*
 * rc_blth (broadcast, length bits) fields
 *
 * MIN_RIF only applies to specific route rifs. LTHMSK is set to always
 * be even.  RIFs are always supposed to be even.  If it isn't things
 * will be screwed anyway and this ensures we do reasonable things with
 * alignment.
 */
#define RC_RTMASK	0xc0
#define RC_BROADBIT	0x80
#define	RC_SPANBIT	0x40	/* when broad bit is also set */
#define	RC_TRACE_LM	0x40	/* when broad bit is not set */
#define	RC_TRACE_ORIG	0x20	/* when broad bit is not set */

#define RC_SPECIFIC	0x00
#define RC_TRACE_FRAME	0x60
#define RC_TRACE_BOTH	0x60
#define RC_B_ALL_ROUTES	0x80
#define RC_B_SPAN	0xc0

#define RC_LTHMASK	0x1e
#define RC_MIN_RIF	0x6
#define RC_NORD_LTH	0x2

#define RC_DEF_SPAN	0xc2
#define RC_DEF_ALLRINGS	0x82

#define RIF_LENGTH(x)  	((x) & RC_LTHMASK)
/*
 * rc_dlf (direction, largest frame) fields
 */
#define RC_D		0x80

#define RC_LFMASK	0x70
#define RC_LF516	0x00
#define RC_LF1500	0x10
#define RC_LF1470	0x10
#define RC_LF2052	0x20
#define RC_LF4472	0x30
#define RC_LF8144	0x40
#define RC_LF11407	0x50
#define RC_LF11454	0x50
#define RC_LF17800	0x60
#define RC_LFINITIAL	0x70

#define RC_LF_MIN	516
#define RC_LF516_VAL	516
#define RC_LF1470_VAL	1470
#define RC_LF1500_VAL	1500
#define RC_LF2052_VAL	2052
#define RC_LF4472_VAL	4472
#define RC_LF8144_VAL	8144
#define RC_LF11407_VAL	11407
#define RC_LF11454_VAL	11454
#define RC_LF17800_VAL	17800
#define RC_LF_MAX	17800
#define RC_LF_INFINITY	65535

#define RC_TR_LFSERIAL	0x20
#define RC_TR_LFDEF	0x30
#define RC_TR16_LFDEF	0x40	/* bump if we bump LARGEDATA */

/*
 * Ring descriptor format
 *
 *  0                             1
 *  0  1  2  3  4  5  6  7  8  9  0  1 2  3  4  5 
 * +----------------------------------+----------+
 * |               Ring #             | Bridge # |
 * +----------------------------------+----------+
 *                                    ^          ^
 *                Partition length ---+----------+
 *                *** should always be 4 ***
 */
#define RD_RNMASK	0xfff0
#define RD_RNSHIFT	0x4
#define RD_BNMASK	0xf
#define RD_MISMATCH(conf_value, rd_info) ((conf_value ^ rd_info) & 0xfffffff0)

/*
 * The top eight bits in srb_multiring are reserved for the "eavedropping" (and other, future)
 * kludges for the tra_test code.
 */
#define MULTIRING_EAVESLLC 0x01000000
#define MULTIRING_EAVESALL 0x02000000

/*
 * Flags used by rif_lookup().
 */
#define RIF_NOT_PRESENT    0x00000000	/* do not use a rif when sending */
#define RIF_PRESENT        0x00000001	/* Use a rif when sending */
#define RIF_REQUIRED       0x00000002	/* Must use a rif when sending */
#define RIF_USE_ALLROUTES  0x00000004	/* Use all routes explorers */
#define RIF_USE_DN_FUNC    0x00000008	/* DECnet functional address */
#define RIF_USE_LOOP       0x00000010	/* Use LOOP for exploration, not XID */
#define RIF_IN_LLC_DTE     0x00000020   /* Rif immediately proceeds
					   dmac in *address llc dte
					   string passed into
					   tr_vencap */
#define RIF_USE_PAK_PRIORITY 0x00000040 /* set PAK_PRIORITY for
					   exploration packet */

typedef struct _mac_hdr {
    uchar  daddr[IEEEBYTES];
    uchar  saddr[IEEEBYTES];
    uchar  data[0];
} mac_hdr;

/*
 * Hash table entry
 */

struct riftype_ {
    struct  riftype_ *next;		/* pointer to next record */
    int     magic;			/* magic no for paranoia code */
    int     flags;
    int     validate_flags;
    uchar   hardware[IEEEBYTES];	/* hardware address */
    hwidbtype *idb;			/* interface last heard on */
    ushort  vrn;			/* ring-group if remote */
    int     length;			/* length of RIF */
    uchar   rif[SRB_MAXRIF];		/* the actual RIF */
    sys_timestamp  create_time;		/* time of creation */
    sys_timestamp  update_time;		/* time of last update */
    sys_timestamp  validate_time;       /* time of last proxy or test command */
    sys_timestamp  explorer_time;       /* time of last explorer  */
    sys_timestamp  piggyback_lookup_time;/*time of last lookup  */
    uchar   src_addr[IEEEBYTES];        /* source address used by LACK */
    sys_timestamp  add_name_query_time;	/* When next ADD_NAME_QUERY will go */
    ulong   netb_saved;			/* Packets saved from transmission */
    ushort  add_name_query_correl; 	/* response correlator w/above */
    uchar   nq_name[17];		/* last name_query name. Read 17 as */
					/* NETBIOS_MAX_NAME_LEN + 1 */
};

static inline uint rif_length (uchar const *rif)
{
  return (rif[0]&RC_LTHMASK);
}

#define RIF_MAXAGE (15*ONEMIN)		/* default max age */
#define RIF_VALIDATE_MAXAGE (2*ONESEC)
#define RIF_MAGIC  0x516A516A

#define RIF_NULL 0		/* none */
#define RIF_INTERFACE 1		/* for one of the routers interfaces */
#define RIF_HANDCRAFT 2		/* entered from config mode */
#define RIF_HANDCRAFT_REM 4	/* entered from config mode for vring */
#define RIF_AGED 8		/* learned from traffic */
#define RIF_AGED_REM 16		/* learned from a remote peer */
#define RIF_NOAGE 32		/* Dynamic, don't age (Local ACK) */
#define RIF_LNM 64		/* Dynamic, don't age, don't flip direction */

/* validate_flags */
#define RIF_VALIDATE_NEW_ROUTE 1 /* rif has been overwritten     */

typedef struct _llc1_xid_t {
    uchar dsap;			/* destinations service access point */
    uchar ssap;			/* source service access point */
    uchar control;		/* frame type */
    uchar data[3];		/* xid junk */
} llc1_xid_t;

typedef struct {
    uchar xid3_fmt;
    uchar xid3_len;
    uchar xid3_reserved[6];	/* other fields to be filled in as needed */
    uchar xid3_char;		/* upper half of characteristics of the sender */
    uchar xid3_esi;		/* lower half, exchange state indicator */

#define XID3_ESI_MASK	      0x0c
#define XID3_ESI_NOEXCH       0x00
#define XID3_ESI_NEG_PREC     0x04
#define XID3_ESI_PRENEG_PREC  0x08
#define XID3_ESI_NONACT       0x0c

    uchar xid3_reserved1[9];	/* more reserved data, not defined here */

} xid3_t;

typedef struct _llc1_test_t {
    uchar dsap;                 /* destinations service access point */
    uchar ssap;                 /* source service access point */
    uchar control;              /* frame type */
} llc1_test_t;

typedef struct _loop_t {
    uchar dsap;			/* destinations service access point */
    uchar ssap;			/* source service access point */
    uchar control;		/* frame type */
    uchar oui[5];		/* oui junk */
    ushort skip;		/* loopback data */
    ushort func1;
    uchar mymac[IEEEBYTES];
    ushort func2;
} loop_t;

/*
 * This handy macro returns the local ring number of a machine given a RIF
 * to it...
 */

/*
 * Handy RIF macros
 */
#define RIF_DEST_RING(r) \
    ((GETSHORT(&((r)->rd\
       [ \
	((!((r)->rc_dlf & RC_D)) ? \
	 (((((r)->rc_blth & RC_LTHMASK) - 2) >> 1) - 1) : \
	 (0)) \
	] \
       )) & RD_RNMASK) >> RD_RNSHIFT)

#define RIF_START_RING(r) \
    ((GETSHORT(&((r)->rd\
       [ \
	((((r)->rc_dlf & RC_D)) ? \
	 (((((r)->rc_blth & RC_LTHMASK) - 2) >> 1) - 1) : \
	 (0)) \
	] \
       )) & RD_RNMASK) >> RD_RNSHIFT)

#define RIF_SECOND_RING(r) \
    ((GETSHORT(&((r)->rd\
       [ \
	((((r)->rc_dlf & RC_D)) ? \
	 (((((r)->rc_blth & RC_LTHMASK) - 2) >> 1) - 2) : \
	 (1)) \
	] \
       )) & RD_RNMASK) >> RD_RNSHIFT)

#define RIF_THIRD_RING(r) \
    ((GETSHORT(&((r)->rd\
       [ \
	((((r)->rc_dlf & RC_D)) ? \
	 (((((r)->rc_blth & RC_LTHMASK) - 2) >> 1) - 3) : \
	 (2)) \
	] \
       )) & RD_RNMASK) >> RD_RNSHIFT)

#define RIF_THIRD_RING_BACK(r) \
    ((GETSHORT(&((r)->rd\
       [ \
        ((!((r)->rc_dlf & RC_D)) ? \
         (((((r)->rc_blth & RC_LTHMASK) - 2) >> 1) - 3) : \
         (2)) \
        ] \
       )) & RD_RNMASK) >> RD_RNSHIFT)

#define RIF_START_BRIDGE(r) \
    (GETSHORT(&((r)->rd\
      [ \
       ((((r)->rc_dlf & RC_D)) ? \
	(((((r)->rc_blth & RC_LTHMASK) - 2) >> 1) - 2) : \
	(0)) \
       ] \
      )) & RD_BNMASK)

#define RIF_SECOND_BRIDGE(r) \
    ((GETSHORT(&(r)->rd\
      [ \
       ((((r)->rc_dlf & RC_D)) ? \
	(((((r)->rc_blth & RC_LTHMASK) - 2) >> 1) - 3) : \
	(1)) \
       ] \
      )) & RD_BNMASK)

#define RIF_FOURTH_RING(r) \
    ((GETSHORT(&((r)->rd\
       [ \
        ((((r)->rc_dlf & RC_D)) ? \
         (((((r)->rc_blth & RC_LTHMASK) - 2) >> 1) - 4) : \
         (3)) \
        ] \
       )) & RD_RNMASK) >> RD_RNSHIFT)
 
#define RIF_THIRD_BRIDGE(r) \
    ((GETSHORT(&(r)->rd\
      [ \
       ((((r)->rc_dlf & RC_D)) ? \
        (((((r)->rc_blth & RC_LTHMASK) - 2) >> 1) - 4) : \
        (2)) \
       ] \
      )) & RD_BNMASK)

#define FREE_RIF_LW 10
#define FREE_RIF_HW 25

#define rifhash(p) ((*p & ~TR_RII) ^ *(p+1) ^ *(p+2) ^ *(p+3) ^ *(p+4) ^ *(p+5))

extern riftype *riftable[NETHASHLEN];
extern int rif_validation_age;         /* max age of rif entries    */
extern boolean rif_validate_route_cache;
extern boolean tr_rif_xid_explorer;
extern boolean rif_validate_enable;

extern boolean rif_merge_rifs(paktype *, paktype *, riftype *);
extern boolean unset_rif(hwidbtype *, ushort, uchar *, uchar *, boolean);
extern int getHwLf(hwidbtype *idb);

extern int rif_count_bridges(uchar *, hwidbtype *);
extern riftype *rif_lookup_callback(uchar *, hwidbtype *, uchar *, int,
				    int linktype, idbtype *idb,
				    long protocol_address);
extern void rif_update_llc(hwidbtype *, ushort, uchar *, uchar *, 
			   struct srbroute_t_ *, struct sap_hdr_ *, int);
extern paktype *tr_setup_xid(uchar *, uchar *, hwidbtype *, uchar *, boolean,
			     int, uint, uint, boolean);
extern paktype *tr_setup_lnx_xid(uchar *daddr, uchar *saddr, lnxtype *lnxp, 
				 boolean response, hwidbtype *vidb,
				 uchar *rif, boolean longxid);

extern void force_rif_flag(uchar *da, uchar *sa, int flags);
extern boolean isRifExplorerPak(paktype *pak);

extern void rif_init(void);
extern void rif_ager(boolean, boolean);

extern void tr_to_media_out(paktype *);
extern void fddi_to_tr(paktype *pak, hwidbtype *idb);
extern void tr_to_fddi(paktype *pak, hwidbtype *idb);
/*
 * tring_setup.c
 */
extern void tring_setup_int(setup_struct *);

#endif __RIF_H__



