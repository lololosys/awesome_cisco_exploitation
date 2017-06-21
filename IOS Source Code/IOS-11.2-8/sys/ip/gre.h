/* $Id: gre.h,v 3.2.60.1 1996/03/18 20:15:52 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/ip/gre.h,v $
 *------------------------------------------------------------------
 * Generic Route Encapsulation
 *
 * April, 1992
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: gre.h,v $
 * Revision 3.2.60.1  1996/03/18  20:15:52  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/13  01:37:10  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.1  1996/02/20  14:25:08  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:33:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:04:00  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:56:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __GRE_H__
#define __GRE_H__

/*
 * User defined GRE protocol types -- used if we don't have 10mb ethernet ptype
 * (these are specified with constants because they will be interoperating
 *  with other non-cisco encapsulators at some other time)
 */

typedef enum gre_ptype_ {
    GRE_PTYPE_UNKNOWN			= 0x0000,
    GRE_PTYPE_SNA			= 0x0004,
    GRE_PTYPE_IEEE_SPANNING		= 0x0042,
    GRE_PTYPE_SNAP			= 0x00aa,
    GRE_PTYPE_OSI			= 0x00fe,
    GRE_PTYPE_PUP			= 0x0200,
    GRE_PTYPE_XNS			= 0x0600,
    GRE_PTYPE_IP			= 0x0800,
    GRE_PTYPE_RFC826_ARP		= 0x0806,
    GRE_PTYPE_FR_ARP			= 0x0808,
    GRE_PTYPE_VINES			= 0x0bad,
    GRE_PTYPE_VINES_ECHO		= 0x0bae,
    GRE_PTYPE_VINES_LOOP		= 0x0baf,
    GRE_PTYPE_NHRP			= 0x2001,
    GRE_PTYPE_DECNET			= 0x6003,
    GRE_PTYPE_BRIDGE			= 0x6558,
    GRE_PTYPE_FR_SWITCH			= 0x6559,
    GRE_PTYPE_DEC_SPANNING		= 0x8038,
    GRE_PTYPE_APPLETALK			= 0x809b,
    GRE_PTYPE_APOLLO			= 0x8019,
    GRE_PTYPE_NOVELL			= 0x8137,
    GRE_PTYPE_TAG			= 0x8847,
    GRE_PTYPE_MTAG			= 0x8848,
    GRE_PTYPE_IP_AS			= 0xfffe,
    GRE_PTYPE_RESERVED			= 0xffff
} gre_ptype;

/*
 * This is the fixed position part of a GRE header:
 */

typedef struct gretype_ {
#ifdef	BIGENDIAN		/* 68k, network byte order */
    boolean have_checksum: 1;
    boolean have_routing:  1;
    boolean have_key:      1;
    boolean have_sequence: 1;
    boolean strict_route:  1;
    ushort  reserved_1:    8;
    ushort  version:	   3;
#else				/* VAX, Intel */
    ushort  version:	   3;
    ushort  reserved_1:    8;
    boolean strict_route:  1;
    boolean have_sequence: 1;
    boolean have_key:      1;
    boolean have_routing:  1;
    boolean have_checksum: 1;
#endif
    ushort  ptype;
    ushort  checksum;			/* optional, fixed position */
    ushort  offset;			/* optional, fixed position */
    /* ulong key */			/* optional, variable position */
    /* ulong sequence */		/* optional, variable position */
    /* ulong routing */			/* optional, variable position,len */
} gretype;

/*
 * source route entry (follows gre header)
 */
typedef struct sretype_ {
    ushort address_family;		/* address family (ethernet ptype) */
    uchar offset;			/* offset of currently active entry */
    uchar length;			/* length of data to follow */
} sretype;

#define	GRE_SEQUENCE_WINDOW	64	/* size of accept field in sequencer */

#define	MINGREHEADERBYTES       sizeof(ulong)
#define	GREHEADERBYTES(ti)						\
  ( sizeof(ulong) +				/* required stuff */	\
    (ti->checksum || ti->srclen			/* checksum OR sre */	\
		    ? sizeof(ulong) : 0) +				\
    (ti->enable_key ? sizeof(ulong) : 0) +	/* key */		\
    (ti->enable_seq ? sizeof(ulong) : 0) +	/* sequence */		\
    ti->srclen )				/* source path len */

#endif

#define ISGRE(idb) (is_tunnel(idb) && ((idb)->tunnel) && ((idb)->tunnel->mode==TUN_MODE_GRE_IP))
