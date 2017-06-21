/* $Id: xnsfast.h,v 3.4.4.3 1996/06/12 17:38:07 akr Exp $
 * $Source: /release/112/cvs/Xsys/xns/xnsfast.h,v $
 *------------------------------------------------------------------
 * xnsfast.h -- XNS and NOVELL Fast Switching definitions.
 *
 * 29-April-1990, Phanindra Jujjavarapu
 *
 * Copyright (c) 1988-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xnsfast.h,v $
 * Revision 3.4.4.3  1996/06/12  17:38:07  akr
 * CSCdi60228:  ipx route-cache can grow without limit
 * Branch: California_branch
 *
 * Revision 3.4.4.2  1996/05/17  12:18:27  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.2.2  1996/05/02  22:09:00  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.2.1  1996/03/17  18:57:20  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.4.4.1  1996/05/09  14:50:06  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.4.30.1  1996/04/27  07:39:58  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.4.20.1  1996/04/08  02:44:11  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.4.10.1  1996/03/22  22:56:58  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.4  1996/03/09  18:13:51  mschaef
 * CSCdi46978:  IPX fastswitch cache can grow very large
 *
 * Revision 3.3  1996/02/13  19:44:37  hampton
 * Reorder routines to remove platform specific extern statements in
 * fastswitching code.  [CSCdi48844]
 *
 * Revision 3.2.48.1  1996/03/05  05:38:35  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.40.1  1996/02/27  21:21:31  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.14.1  1996/02/15  19:27:46  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.2  1995/11/17  19:25:45  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:00:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/11  23:59:04  hampton
 * Convert XNS and Apollo to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi39966]
 *
 * Revision 2.1  1995/06/07 23:31:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __XNSFAST_H__
#define __XNSFAST_H__
#include "../xns/xns.h"
/*
 * XNS routing cache data structure 
 * and definitions.
 */
#define NULL_CHECKSUM		0xFFFF		/* no checksum in XNS packet */
#define XNS_HASHLEN	256			/* number of hash entries */
#define MAXMAC_BYTES	52			/* max size of mac header */
#define MAXMAC_LONGS	(btol(MAXMAC_BYTES))
#define MAXMAC_SHORTS	(btow(MAXMAC_BYTES))
#define XNSRCACHE_TIMEOUT	(5*ONEMIN)	/* cache entry timeout */
#define NOVRCACHE_TIMEOUT	(2*ONEMIN)	/* cache entry timeout */
#define NOVRCACHE_DEFAULT_MAX_SIZE          (0) /* max route-cache size */
#define NOVRCACHE_DEFAULT_MAX_INACTIVE_AGE  (2) /* max inactive age (min) */
#define NOVRCACHE_DEFAULT_INACTIVE_AGE_RATE (0) /* max invalidation rate */
#define NOVRCACHE_DEFAULT_MAX_UPDATE_AGE    (0) /* max age since update */
#define NOVRCACHE_DEFAULT_UPDATE_AGE_RATE   (0) /* max invalidation rate */
#define XNSRCACHE_DEPTH	3		
#define NOVRCACHE_DEPTH	3		
#define ROUND(value)	(((value) + 1) & 0xFFFE)	

struct xnspathtype_ {

    struct xnspathtype_ *xnsp_next_ptr;	/* pointer to next path */
    idbtype    *xnsp_idb_ptr;		/* interface to be used */
    union {
	uchar  mac_bytes[MAXMAC_BYTES];
	ulong  mac_longs[MAXMAC_LONGS];
	ushort mac_shorts[MAXMAC_SHORTS];
    } xnsp_mh;                          /* mac header itself */
    union {
	uchar  mac_bytes[MAXMAC_BYTES+1];
	ulong  mac_longs[MAXMAC_LONGS+1];
	ushort mac_shorts[MAXMAC_SHORTS+1];
    } xnsp_mh_oa;                       /* mac header odd aligned */
    int length;				/* required by CTR (shorts) */
    int extra_bytes;			/* for odd byte start on old MCIs */
};

typedef struct xnscachetype_ {
    struct xnscachetype_ *xnsc_next_ptr;/* pointer to next entry */
    xnspathtype *xnsc_path_list_ptr;	/* is it for connected net ? */
    xnspathtype *xnsc_path_next_ptr;	/* next path*/
    ushort	xnsc_path_count;	/* number of paths */
    boolean	xnsc_connected;		/* is it directly conn */
    ulong       xnsc_dest_net;		/* destination net */
    uchar       xnsc_dest_address[IEEEBYTES]; /* destination net */
    long        xnsc_version;		/* xnscache_version when updated */
    sys_timestamp xnsc_lastupdate;      /* last updated time in milli sec */ 
} xnscachetype;


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

/*
 * XNS and Novell Cache table
 */
extern long xnscache_version;
extern xnscachetype *xnscache[XNS_HASHLEN];

/*
 * XNS Function Declarations
 *     xns/xnsfast.c
 *     xns/xnsfast_mci.c
 *     xns/xnsfast_pan.c
 */
extern void xns_cache_init(void);
extern void xns_free_entry(xnscachetype *, xnscachetype *);
extern void xns_cache_update(paktype *,ulong);
extern void xns_fscache_age(void);
extern void xns_fscache_flush(void);
extern void show_xns_cache(ushort);
extern void xns_fastsetup(hwidbtype *);
extern void xnscache_increment_version(hwidbtype *, idbtype *,
				       boolean);

#endif __XNSFAST_H__
