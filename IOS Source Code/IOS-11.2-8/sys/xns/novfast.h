/* $Id: novfast.h,v 3.6.4.2 1996/06/12 17:37:59 akr Exp $
 * $Source: /release/112/cvs/Xsys/xns/novfast.h,v $
 *------------------------------------------------------------------
 * novfast.h -- NOVELL Fast Switching definitions.
 *
 * 29-April-1990, Phanindra Jujjavarapu
 * November 1992, Joanne Boyle - Split novfast.h out of xnsfast.h
 *
 * Copyright (c) 1988-1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: novfast.h,v $
 * Revision 3.6.4.2  1996/06/12  17:37:59  akr
 * CSCdi60228:  ipx route-cache can grow without limit
 * Branch: California_branch
 *
 * Revision 3.6.4.1  1996/05/09  14:49:58  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.6.30.1  1996/04/27  07:39:54  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.6.20.1  1996/04/08  02:44:07  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.6.10.1  1996/03/22  22:56:54  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.6  1996/03/09  18:13:21  mschaef
 * CSCdi46978:  IPX fastswitch cache can grow very large
 *
 * Revision 3.5  1996/02/13  19:44:28  hampton
 * Reorder routines to remove platform specific extern statements in
 * fastswitching code.  [CSCdi48844]
 *
 * Revision 3.4.32.1  1996/03/05  05:38:31  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.4.26.1  1996/02/27  21:21:26  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.4.4.1  1996/02/15  19:27:42  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.4  1995/12/15  19:42:30  mschaef
 * CSCdi45600:  ipx cache wrong for some destinations when RIP
 *              route replaced NLSP
 *
 * Revision 3.3  1995/12/01  06:25:55  mschaef
 * CSCdi37234:  Allow fastswitching of certain directed broadcast packets
 *
 * Revision 3.2  1995/11/17  19:23:27  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:59:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:39:19  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/01  03:39:02  hampton
 * Minor Novell IPX Cleanups.  Remove some unused variables and routines.
 * Eliminate direct references to the system clock.  [CSCdi36668]
 *
 * Revision 2.1  1995/06/07 23:28:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __NOVFAST_H__
#define __NOVFAST_H__

#include "../xns/xns.h"
#include "../xns/novell.h"
#include "xnsfast.h"

/*
 * Novell routing cache data structure 
 * and definitions.
 */

/*
 * Allow tracking of cache invalidation:
 */

#define NOVCACHE_STATS TRUE

enum NOVCACHE_REASON {
/* 00 */    NOVCACHE_NULL_REASON, /* Used for uninteresting events */
/* 01 */    NOVCACHE_NETWORK_ENC,
/* 02 */    NOVCACHE_NETWORK_BUMP,
/* 03 */    NOVCACHE_BRINGUP_NET,
/* 04 */    NOVCACHE_KILL_CONN_RT,
/* 05 */    NOVCACHE_BRINGDOWN_NET,
/* 06 */    NOVCACHE_NEWSTATE_DORD_PENDING,
/* 07 */    NOVCACHE_RIP_STATIC_RT,
/* 08 */    NOVCACHE_RIP_AGER_STALE,
/* 09 */    NOVCACHE_RIP_AGER_RESORT,
/* 10 */    NOVCACHE_RIP_AGER_TRIM,
/* 11 */    NOVCACHE_RIP_EQ_PATH_STALE,
/* 12 */    NOVCACHE_RIP_EQ_PATH_FASTER1,
/* 13 */    NOVCACHE_RIP_ADD_EQ_PATH,
/* 14 */    NOVCACHE_RIP_ADD_EQ_PATH_FASTER,
/* 15 */    NOVCACHE_RIP_BETTER_METRIC,
/* 16 */    NOVCACHE_RIP_WORSE_METRIC_LAST,
/* 17 */    NOVCACHE_RIP_WORSE_METRIC,
/* 18 */    NOVCACHE_RIP_BETTER_XROUTE,
/* 19 */    NOVCACHE_FLUSH,
/* 20 */    NOVCACHE_FAST_SETUP,
/* 21 */    NOVCACHE_CLEAR,
/* 22 */    NOVCACHE_IGRP_RT_DEL,
/* 23 */    NOVCACHE_IGRP_ADD_PATH,
/* 24 */    NOVCACHE_IGRP_BETTER_METRIC,
/* 25 */    NOVCACHE_IGRP_WORSE_METRIC_LAST,
/* 26 */    NOVCACHE_IGRP_WORSE_METRIC,
/* 27 */    NOVCACHE_EXTERNAL_EVENT,
/* 28 */    NOVCACHE_RIP_NEW_ROUTE,
/* 29 */    NOVCACHE_IGRP_NEW_ROUTE,
/* 30 */    NOVCACHE_NLSP_NEW_ROUTE,
/* 31 */    NOVCACHE_NLSP_BETTER_METRIC,
/* 32 */    NOVCACHE_NLSP_PATH_CHANGE,
/* 33 */    NOVCACHE_NLSP_WORSE_METRIC,
/* 34 */    NOVCACHE_NLSP_UNREACHABLE,
	    NOVCACHE_REASON_MAX
};

struct novpathtype_ {

    struct novpathtype_ *novp_next_ptr;	/* pointer to next path */
    idbtype    *novp_idb_ptr;		/* interface to be used */
    nidbtype   *novp_nidb_ptr;		/* net to be used */
    union {
	uchar  mac_bytes[MAXMAC_BYTES];
	ulong  mac_longs[MAXMAC_LONGS];
	ushort mac_shorts[MAXMAC_SHORTS];
    } novp_mh;                          /* mac header itself */
    union {
	uchar  mac_bytes[MAXMAC_BYTES+1];
	ulong  mac_longs[MAXMAC_LONGS+1];
	ushort mac_shorts[MAXMAC_SHORTS+1];
    } novp_mh_oa;                       /* mac header odd aligned */
    ushort length;			/* required by CTR and MIP (bytes) */
    ushort total_encap_words;		/* btow copy of total_encap_bytes */
    ushort rifwords;			/* RIF length (words) */
    ushort subint_reference_num;	/* for VC accounting */
    int total_encap_bytes;		/* Total bytes of encapsulation */
    int extra_bytes;			/* for odd byte start on old MCIs */
    int enctype;                        /* preset encap type for FS */
    boolean igrp_route;                 /* route learned via IGRP */
    dialergrouptype *dialergroup;	/* dialer group per path */
};

/*
 * Fastswitch cache entry
 *
 * Note the position, size, and use of the novc_active field. This is
 * positioned to be part of the last long word of dest host, and is
 * accessed (usually read-only) immediately after the dest host access
 * during cache entry lookup. Do not move without good reason.
 */
struct novcachetype_ {
    struct novcachetype_ *novc_next_ptr; /* pointer to next entry */
    struct novcachetype_ *novc_hash_next; /* next hash table entry */
    novpathtype *novc_path_list_ptr;	/* is it for connected net ? */
    novpathtype *novc_path_next_ptr;	/* next path*/
    ushort	novc_path_count;	/* number of paths */
    ushort      novc_inactive;          /* Minutes inactive */
    ulong       novc_dest_net;		/* destination net */
    uchar       novc_dest_address[IEEEBYTES]; /* destination net */
    ushort      novc_active;            /* TRUE if active since ager ran */
    long        novc_version;		/* novcache_version when updated */
    sys_timestamp novc_lastupdate;      /* last updated time in milli sec */ 
    uchar       novc_bcast;             /* entry is directed broadcast */
    uchar       novc_ext_cache;         /* entry uploaded to SP/SSE */
    uchar	novc_connected;		/* is it directly conn */
};


/*
 * Define a hash table to organize cache entries by route:
 */
typedef novcachetype *novcache_hashtable_type[NOV_NETHASHLEN];

/*
 * Inline routines to update the hop count while fastswitching. The tc
 * may be in the least significant byte (lsb) or most significant byte
 * (msb) of the low order word of len_hops ... hence the two routines.
 *
 * Note: These routines are certainly not optimal at the present time.
 */

static inline ulong update_lsb_tc(ulong len_hops, novpathtype *path)
{
    charlong lh;

    if ((novell_igrp_running == 0) ||
	(!NOVELL_IGRP_HOPCOUNT_ALGORITHM_ALLOWED)) {
	/* Always do 4 -> 4 if IGRP is not running on this box */
	len_hops += 1;
	return(len_hops);
    } else {
	lh.d.lword = len_hops;
	if (lh.d.byte[3] <= XNS_MAX_HOPS) {
	    if (path->igrp_route) {
		/* 4 -> 8 */
		if (lh.d.byte[3] == 0)
		    lh.d.byte[3] += 1;
		lh.d.byte[3] += 1;
		if (lh.d.byte[3] <= XNS_MAX_HOPS)
		    lh.d.byte[3] = lh.d.byte[3]*16;
	    } else {
		/* 4 -> 4 */
		lh.d.byte[3] += 1;
	    }
	} else {
	    if (path->igrp_route) {
		/* 8 -> 8 */
		lh.d.byte[3] += 1;
	    } else {
		/* 8 -> 4 */
		lh.d.byte[3] = lh.d.byte[3]/16;
	    }
	}
	len_hops = lh.d.lword;
    }
    return(len_hops);
}

static inline ulong update_msb_tc(ulong len_hops, novpathtype *path)
{
    charlong lh;
    
    if ((novell_igrp_running == 0) ||
	(!NOVELL_IGRP_HOPCOUNT_ALGORITHM_ALLOWED)) {
	/* Always do 4 -> 4 if IGRP is not running on this box */
	len_hops += 256;
	return(len_hops);
    } else {
	lh.d.lword = len_hops;
	if (lh.d.sword[1] < ((XNS_MAX_HOPS+1) << 8)) {
	    if (path->igrp_route) {
		/* 4 -> 8 */
		if (lh.d.sword[1] < 256)
		    lh.d.sword[1] += 256;
		lh.d.sword[1] += 256;
		if (lh.d.sword[1] < ((XNS_MAX_HOPS+1) << 8))
		    lh.d.sword[1] = (((lh.d.byte[2]*16) << 8) | lh.d.byte[3]);
	    } else {
		/* 4 -> 4 */
		lh.d.sword[1] += 256;
	    }
	} else {
	    if (path->igrp_route) {
		/* 8 -> 8 */
		lh.d.sword[1] += 256;
	    } else {
		/* 8 -> 4 */
		lh.d.sword[1] = (((lh.d.byte[2]/16) << 8) | lh.d.byte[3]);
	    }
	}
	len_hops = lh.d.lword;
    }
    return(len_hops);
}

/*
 * Novell generic fast switching definitions:
 */

/*
 * Novell specific fast switching definitions:
 */

/*
 * Word offsets for use by fastswitching code:
 */

#define XNS_CSUM_OFFSET    0            /* checksum */
#define XNS_LEN_OFFSET     1            /* length */
#define XNS_TCPT_OFFSET    2            /* transport ctl/pkt type */
#define XNS_DNET_OFFSET    3            /* dest net */
#define XNS_DHOST_OFFSET   5            /* dest host */
#define XNS_DSOCK_OFFSET   8            /* dest socket */
#define XNS_SNET_OFFSET    9            /* src net */
#define XNS_SHOST_OFFSET  11            /* src host */
#define XNS_SSOCK_OFFSET  14            /* src sock */

/*
 * Define word offset of base of IPX header given input encapsulation of XXX
 * i.e., XXX_IPX_OFFSET.
 */

/*
 * Ethernet:
 *
 * (Note that ARPA and NOVELL-ETHER are equivalent.)
 */

#define ARPA_IPX_OFFSET (MCI_ETHER_OFFSET + (ETHER_ARPA_ENCAPBYTES >> 1))
#define ISO1_IPX_OFFSET (MCI_ETHER_OFFSET + (ETHER_SAP_ENCAPBYTES >> 1))
#define ISO2_IPX_OFFSET (MCI_ETHER_OFFSET + (ETHER_SNAP_ENCAPBYTES >> 1))

/*
 * FDDI:
 *
 */

#define FDDI_ISO1_IPX_OFFSET (FDDI_RECEIVE_OFFSET + (ROUND(FDDI_SAP_ENCAPBYTES) >> 1))
#define FDDI_ISO2_IPX_OFFSET (FDDI_RECEIVE_OFFSET + (ROUND(FDDI_SNAP_ENCAPBYTES) >> 1))
#define FDDI_RAW_IPX_OFFSET  (FDDI_RECEIVE_OFFSET + (ROUND(FDDI_ENCAPBYTES) >> 1))

/*
 * Token Ring:
 *
 */

#define TR_ISO1_IPX_OFFSET (CTR_RECEIVE_OFFSET + (TRING_SAP_ENCAPBYTES >> 1))
#define TR_ISO2_IPX_OFFSET (CTR_RECEIVE_OFFSET + (TRING_SNAP_ENCAPBYTES >> 1))

/*
 * Serial:
 *
 */

#define HDLC_IPX_OFFSET (MCI_SERIAL_OFFSET + (HDLC_ENCAPBYTES >> 1))
#define FR_IPX_OFFSET       (MCI_SERIAL_OFFSET +                             \
			    (FRAME_RELAY_ENCAPBYTES >> 1))
#define FR_IETF_IPX_OFFSET (MCI_SERIAL_OFFSET +                              \
			    (FRAME_RELAY_SNAPENCAPBYTES >> 1))

/*
 * AIP:
 */
#define	AIP_IPX_OFFSET(i, a)	(i->rxoffset + ((a->encapsize + 1) >> 1))

/*
 * Define (word) offsets to base of output encapsulation YYY given
 * input encapsulation XXX, i.e., XXX_YYY_OFFSET.
 */

/*
 * Ethernet:
 *
 * (Note that ARPA and NOVELL-ETHER are equivalent.)
 */

/*
 * Input ARPA:
 */
/*
 *       IN   OUT
 */
#define ARPA_ARPA_OFFSET (ARPA_IPX_OFFSET - (ROUND(ETHER_ARPA_ENCAPBYTES) >> 1))
#define ARPA_ISO1_OFFSET (ARPA_IPX_OFFSET - (ROUND(ETHER_SAP_ENCAPBYTES) >> 1))
#define ARPA_ISO2_OFFSET (ARPA_IPX_OFFSET - (ROUND(ETHER_SNAP_ENCAPBYTES) >> 1))

/*
 * Input ISO1:
 */

#define ISO1_ARPA_OFFSET (ISO1_IPX_OFFSET - (ETHER_ARPA_ENCAPBYTES >> 1))
#define ISO1_ISO1_OFFSET (ISO1_IPX_OFFSET - (ETHER_SAP_ENCAPBYTES >> 1))
#define ISO1_ISO2_OFFSET (ISO1_IPX_OFFSET - (ETHER_SNAP_ENCAPBYTES >> 1))

/*
 * Input ISO2:
 */

#define ISO2_ARPA_OFFSET (ISO2_IPX_OFFSET - (ROUND(ETHER_ARPA_ENCAPBYTES) >> 1))
#define ISO2_ISO1_OFFSET (ISO2_IPX_OFFSET - (ROUND(ETHER_SAP_ENCAPBYTES) >> 1))
#define ISO2_ISO2_OFFSET (ISO2_IPX_OFFSET - (ROUND(ETHER_SNAP_ENCAPBYTES) >> 1))

/*
 * Fddi:
 */

#define ARPA_FDDI1_OFFSET (ARPA_IPX_OFFSET - (FDDI_SAP_ENCAPBYTES >> 1))
#define ISO1_FDDI1_OFFSET (ISO1_IPX_OFFSET - (FDDI_SAP_ENCAPBYTES >> 1))
#define ISO2_FDDI1_OFFSET (ISO2_IPX_OFFSET - (FDDI_SAP_ENCAPBYTES >> 1))

#define ARPA_FDDI2_OFFSET (ARPA_IPX_OFFSET - (ROUND(FDDI_SNAP_ENCAPBYTES) >> 1))
#define ISO1_FDDI2_OFFSET (ISO1_IPX_OFFSET - (      FDDI_SNAP_ENCAPBYTES >> 1))
#define ISO2_FDDI2_OFFSET (ISO2_IPX_OFFSET - (ROUND(FDDI_SNAP_ENCAPBYTES) >> 1))

#define FDDI1_ARPA_OFFSET (FDDI_ISO1_IPX_OFFSET - (ETHER_ARPA_ENCAPBYTES >> 1))
#define FDDI1_ISO1_OFFSET (FDDI_ISO1_IPX_OFFSET - (ETHER_SAP_ENCAPBYTES >> 1))
#define FDDI1_ISO2_OFFSET (FDDI_ISO1_IPX_OFFSET - (ETHER_SNAP_ENCAPBYTES >> 1))

#define FDDI2_ARPA_OFFSET (FDDI_ISO2_IPX_OFFSET - (ROUND(ETHER_ARPA_ENCAPBYTES) >> 1))
#define FDDI2_ISO1_OFFSET (FDDI_ISO2_IPX_OFFSET - (ROUND(ETHER_SAP_ENCAPBYTES) >> 1))
#define FDDI2_ISO2_OFFSET (FDDI_ISO2_IPX_OFFSET - (ROUND(ETHER_SNAP_ENCAPBYTES) >> 1))

#define FDDI1_FDDI1_OFFSET (FDDI_ISO1_IPX_OFFSET - (FDDI_SAP_ENCAPBYTES >> 1))
#define FDDI2_FDDI1_OFFSET (FDDI_ISO2_IPX_OFFSET - (FDDI_SAP_ENCAPBYTES >> 1))
#define FDDI1_FDDI2_OFFSET (FDDI_ISO1_IPX_OFFSET - (FDDI_SNAP_ENCAPBYTES >> 1))
#define FDDI2_FDDI2_OFFSET (FDDI_ISO2_IPX_OFFSET - (ROUND(FDDI_SNAP_ENCAPBYTES) >> 1))
#define FDDI1_TR1_OFFSET (FDDI_ISO1_IPX_OFFSET - (TRING_SAP_ENCAPBYTES >> 1))
#define FDDI1_TR2_OFFSET (FDDI_ISO1_IPX_OFFSET - (TRING_SNAP_ENCAPBYTES >> 1))
#define FDDI2_TR1_OFFSET (FDDI_ISO2_IPX_OFFSET - (ROUND(TRING_SAP_ENCAPBYTES) >> 1))
#define FDDI2_TR2_OFFSET (FDDI_ISO2_IPX_OFFSET - (TRING_SNAP_ENCAPBYTES >> 1))
/*
 * Token Ring:
 */

#define ARPA_TR1_OFFSET (ARPA_IPX_OFFSET - (ROUND(TRING_SAP_ENCAPBYTES) >> 1))
#define ISO1_TR1_OFFSET (ISO1_IPX_OFFSET - (TRING_SAP_ENCAPBYTES >> 1))
#define ISO2_TR1_OFFSET (ISO2_IPX_OFFSET - (ROUND(TRING_SAP_ENCAPBYTES) >> 1))

#define ARPA_TR2_OFFSET (ARPA_IPX_OFFSET - (TRING_SNAP_ENCAPBYTES >> 1))
#define ISO1_TR2_OFFSET (ISO1_IPX_OFFSET - (TRING_SNAP_ENCAPBYTES >> 1))
#define ISO2_TR2_OFFSET (ISO2_IPX_OFFSET - (TRING_SNAP_ENCAPBYTES >> 1))

#define TR1_ARPA_OFFSET (TR_ISO1_IPX_OFFSET - (ETHER_ARPA_ENCAPBYTES >> 1))
#define TR1_ISO1_OFFSET (TR_ISO1_IPX_OFFSET - (ETHER_SAP_ENCAPBYTES >> 1))
#define TR1_ISO2_OFFSET (TR_ISO1_IPX_OFFSET - (ETHER_SNAP_ENCAPBYTES >> 1))

#define TR2_ARPA_OFFSET (TR_ISO2_IPX_OFFSET - (ROUND(ETHER_ARPA_ENCAPBYTES) >> 1))
#define TR2_ISO1_OFFSET (TR_ISO2_IPX_OFFSET - (ROUND(ETHER_SAP_ENCAPBYTES) >> 1))
#define TR2_ISO2_OFFSET (TR_ISO2_IPX_OFFSET - (ROUND(ETHER_SNAP_ENCAPBYTES) >> 1))

#define TR1_FDDI1_OFFSET (TR_ISO1_IPX_OFFSET - (FDDI_SAP_ENCAPBYTES >> 1))
#define TR1_FDDI2_OFFSET (TR_ISO1_IPX_OFFSET - (FDDI_SNAP_ENCAPBYTES >> 1))
#define TR2_FDDI1_OFFSET (TR_ISO2_IPX_OFFSET - (FDDI_SAP_ENCAPBYTES >> 1))
#define TR2_FDDI2_OFFSET (TR_ISO2_IPX_OFFSET - (ROUND(FDDI_SNAP_ENCAPBYTES) >> 1))

#define TR1_TR1_OFFSET (TR_ISO1_IPX_OFFSET - (TRING_SAP_ENCAPBYTES >> 1))
#define TR1_TR2_OFFSET (TR_ISO1_IPX_OFFSET - (TRING_SNAP_ENCAPBYTES >> 1))
#define TR2_TR1_OFFSET (TR_ISO2_IPX_OFFSET - (ROUND(TRING_SAP_ENCAPBYTES) >> 1))
#define TR2_TR2_OFFSET (TR_ISO2_IPX_OFFSET - (TRING_SNAP_ENCAPBYTES >> 1))
/*
 * Serial: - doesn't use equates - It's possible to have different
 * encapsulation sizes - uses hwidb->encsize
 */

#define ARPA_HDLC_OFFSET (ARPA_IPX_OFFSET - (HDLC_ENCAPBYTES >> 1))
#define ISO1_HDLC_OFFSET (ISO1_IPX_OFFSET - (HDLC_ENCAPBYTES >> 1))
#define ISO2_HDLC_OFFSET (ISO2_IPX_OFFSET - (HDLC_ENCAPBYTES >> 1))

#define FDDI1_HDLC_OFFSET (FDDI_ISO1_IPX_OFFSET - (HDLC_ENCAPBYTES >> 1))
#define FDDI2_HDLC_OFFSET (FDDI_ISO2_IPX_OFFSET - (HDLC_ENCAPBYTES >> 1))

#define   TR1_HDLC_OFFSET   (TR_ISO1_IPX_OFFSET - (HDLC_ENCAPBYTES >> 1))
#define   TR2_HDLC_OFFSET   (TR_ISO2_IPX_OFFSET - (HDLC_ENCAPBYTES >> 1))

#define HDLC_ARPA_OFFSET (HDLC_IPX_OFFSET - (ETHER_ARPA_ENCAPBYTES >> 1))
#define HDLC_ISO1_OFFSET (HDLC_IPX_OFFSET - (ROUND(ETHER_SAP_ENCAPBYTES) >> 1))
#define HDLC_ISO2_OFFSET (HDLC_IPX_OFFSET - (ETHER_SNAP_ENCAPBYTES >> 1))

#define HDLC_FDDI1_OFFSET (HDLC_IPX_OFFSET - (FDDI_SAP_ENCAPBYTES >> 1))
#define HDLC_FDDI2_OFFSET (HDLC_IPX_OFFSET - (ROUND(FDDI_SNAP_ENCAPBYTES) >> 1))

#define HDLC_TR1_OFFSET (HDLC_IPX_OFFSET - (ROUND(TRING_SAP_ENCAPBYTES) >> 1))
#define HDLC_TR2_OFFSET (HDLC_IPX_OFFSET - (TRING_SNAP_ENCAPBYTES >> 1))

#define FR_ARPA_OFFSET  (FR_IPX_OFFSET - (ETHER_ARPA_ENCAPBYTES >> 1))
#define FR_ISO1_OFFSET  (FR_IPX_OFFSET - (ROUND(ETHER_SAP_ENCAPBYTES) >> 1))
#define FR_ISO2_OFFSET  (FR_IPX_OFFSET - (ETHER_SNAP_ENCAPBYTES >> 1))

#define FR_FDDI1_OFFSET (FR_IPX_OFFSET - (FDDI_SAP_ENCAPBYTES >> 1))
#define FR_FDDI2_OFFSET (FR_IPX_OFFSET - (ROUND(FDDI_SNAP_ENCAPBYTES) >> 1))

#define FR_TR1_OFFSET   (FR_IPX_OFFSET - (ROUND(TRING_SAP_ENCAPBYTES) >> 1))
#define FR_TR2_OFFSET   (FR_IPX_OFFSET - (TRING_SNAP_ENCAPBYTES >> 1))

#define FR_IETF_ARPA_OFFSET  (FR_IETF_IPX_OFFSET - (ETHER_ARPA_ENCAPBYTES >> 1))
#define FR_IETF_ISO1_OFFSET  (FR_IETF_IPX_OFFSET - (ROUND(ETHER_SAP_ENCAPBYTES) >> 1))
#define FR_IETF_ISO2_OFFSET  (FR_IETF_IPX_OFFSET - (ETHER_SNAP_ENCAPBYTES >> 1))

#define FR_IETF_FDDI1_OFFSET (FR_IETF_IPX_OFFSET - (FDDI_SAP_ENCAPBYTES >> 1))
#define FR_IETF_FDDI2_OFFSET (FR_IETF_IPX_OFFSET - (ROUND(FDDI_SNAP_ENCAPBYTES) >> 1))

#define FR_IETF_TR1_OFFSET   (FR_IETF_IPX_OFFSET - (ROUND(TRING_SAP_ENCAPBYTES) >> 1))
#define FR_IETF_TR2_OFFSET   (FR_IETF_IPX_OFFSET - (TRING_SNAP_ENCAPBYTES >> 1))

#define AIP_ARPA_OFFSET ((lineoff) - (ETHER_ARPA_ENCAPBYTES >> 1))
#define AIP_ISO1_OFFSET ((lineoff) - (ROUND(ETHER_SAP_ENCAPBYTES) >> 1))
#define AIP_ISO2_OFFSET ((lineoff) - (ETHER_SNAP_ENCAPBYTES >> 1))

#define AIP_FDDI1_OFFSET ((lineoff) - (FDDI_SAP_ENCAPBYTES >> 1))
#define AIP_FDDI2_OFFSET ((lineoff) - (ROUND(FDDI_SNAP_ENCAPBYTES) >> 1))

#define AIP_TR1_OFFSET ((lineoff) - (ROUND(TRING_SAP_ENCAPBYTES) >> 1))
#define AIP_TR2_OFFSET ((lineoff) - (TRING_SNAP_ENCAPBYTES >> 1))


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

/*
 * Novell Cache table
 */
extern long novcache_version;
extern ulong novcache_max_size;
extern ushort novcache_max_inactive_age;
extern ushort novcache_inactive_age_rate;
extern ulong novcache_max_update_age;
extern ushort novcache_update_age_rate;
extern novcachetype *novcache[XNS_HASHLEN];
#ifdef NOVCACHE_STATS
#define NOVCACHE_HISTORY_MAX 16
extern ulong novcache_reason[NOVCACHE_REASON_MAX];
extern ulong novcache_param[NOVCACHE_REASON_MAX];
extern sys_timestamp novcache_time[NOVCACHE_REASON_MAX];
extern ulong         novcache_history_reason[NOVCACHE_HISTORY_MAX];
extern ulong         novcache_history_param[NOVCACHE_HISTORY_MAX];
extern sys_timestamp novcache_history_time[NOVCACHE_HISTORY_MAX];
extern ulong         novcache_trigger_reason[NOVCACHE_HISTORY_MAX];
extern sys_timestamp novcache_trigger_time[NOVCACHE_HISTORY_MAX];
extern int novcache_history_index;
extern int novcache_trigger_index;
extern ulong novcache_trigger;
#endif

/*
 * Novell  Function Declarations
 */
extern void nov_adjust_fastflags(idbtype *);
extern void nov_cache_init(void);
extern void nov_cache_update(paktype *, ulong);
extern void nov_fscache_age(void);
extern void nov_fscache_flush(void);
extern void show_nov_cache(void);
extern void show_nov_cache_hash_table(void);
extern void show_nov_cache_history(parseinfo *);
extern void nov_fastsetup(hwidbtype *);
extern void novcache_increment_version(hwidbtype *, int, ulong, ulong);
extern void novcache_external_increment_version(hwidbtype *, idbtype *,
						boolean);

/*
 * Stupid compiler forces the charlong structure to be a stack variable.
 * The shortlong structure is held in a register -- performance consideration.
 */
typedef struct ipx_shortlong_ {
    union {
	ulong lword;
	ushort sword[2];
    } d;
} ipx_shortlong;

static inline boolean nov_fast_acct_do_work (ulong srcnet, ulong dstnet,
					     ulong srchost_high,
					     ushort srchost_low,
					     ulong dsthost_high,
					     ushort dsthost_low,
					     ushort ipxlen)
{
    ipx_shortlong hash;
    ipx_acctg_entry_type *ipxae;
    ipx_acctg_data_type *ipxad;
    int indx;
    ipx_acctg_acc_entry_type *acc_entry;

    if (ipx_acctg_acc != NULL) {
	for (indx = 0, acc_entry = &ipx_acctg_acc->list[0];
	     indx < ipx_acctg_acc->count; indx++, acc_entry++) {
	    if ((((srcnet & ~acc_entry->mask_net) == acc_entry->addr_net) &&
		 ((srchost_high & ~acc_entry->mask_host.u.lword[0]) ==
		  acc_entry->addr_host.u.lword[0]) &&
		 ((srchost_low & ~acc_entry->mask_host.u.sword[2]) ==
		  acc_entry->addr_host.u.sword[2])) ||
		(((dstnet & ~acc_entry->mask_net) == acc_entry->addr_net) &&
		 ((dsthost_high & ~acc_entry->mask_host.u.lword[0]) ==
		  acc_entry->addr_host.u.lword[0]) &&
		 ((dsthost_low & ~acc_entry->mask_host.u.sword[2]) ==
		  acc_entry->addr_host.u.sword[2]))) {
		goto hit;
	    }
	}
	if (ipx_acctg_acc->count == 0)
	  goto hit;
	if (ipx_acctg_acc->transits == 0)
	  return(TRUE);
	ipx_acctg_acc->transits--;
    }
hit:
    /*
     * Compute the hash index into the accounting hash.
     */
    hash.d.lword = srcnet;
    hash.d.lword ^= dstnet;
    hash.d.sword[1] ^= (hash.d.sword[0]);
    hash.d.sword[1] ^= (hash.d.sword[1] >> 8);
    for (ipxae = ipx_acctg_info.current->store[hash.d.lword &= 0xff];
	 ipxae; ipxae = ipxae->next) {
	if ((ipxae->srcnet == srcnet) && (ipxae->dstnet == dstnet) &&
	    (srchost_high == GETLONG(&ipxae->srchost[0])) &&
	    (srchost_low == GETSHORT(&ipxae->srchost[4])) &&
	    (dsthost_high == GETLONG(&ipxae->dsthost[0])) &&
	    (dsthost_low == GETSHORT(&ipxae->dsthost[4]))) {
	    ipxae->packets++;
	    ipxae->bytes += ipxlen;
	    return(TRUE);
	}
    }
    if (ipx_acctg_info.count < ipx_acctg_info.threshold) {
	/*
	 * Entry not found, see if another entry exists on the queue.
	 * If it does, dequeue it and add the entry to the hash.
	 */
	ipxae = dequeue(&ipxacctQ);
	if (ipxae) {
	    ipxae->srcnet = srcnet;
	    ipxae->dstnet = dstnet;
	    PUTLONG(&(ipxae->srchost[0]), srchost_high);
	    PUTSHORT(&(ipxae->srchost[4]), srchost_low);
	    PUTLONG(&(ipxae->dsthost[0]), dsthost_high);
	    PUTSHORT(&(ipxae->dsthost[4]), dsthost_low);
	    ipxae->bytes  = ipxlen;
	    ipxae->packets = 1;
	    ipxad = ipx_acctg_info.current;
	    ipxae->next = ipxad->store[hash.d.lword];
	    ipxad->store[hash.d.lword] = ipxae;
	    ipx_acctg_info.count++;
	    return(TRUE);
	}
	/*
	 * We need to get more memory -- do it at process level.
	 */
	return(FALSE);
    } else {
	/*
	 * If reached our threshold level, bump counters and return TRUE.
	 */
	ipxad = ipx_acctg_info.current;
	ipxad->packets_tossed++;
	ipxad->bytes_tossed += ipxlen;
	return(TRUE);
    }
}

#endif __NOVFAST_H__
