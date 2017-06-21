/* $Id: if_les.h,v 3.3.20.5 1996/09/05 01:16:42 snyder Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les.h,v $
 *------------------------------------------------------------------
 * if_les.h -- Common definitions for all Pancake/xx interfaces
 *
 * updated: Jan 1993, Dit Morse
 * 
 *          Include in this file any common functions
 *          which have platform specific implementations.
 *          The code will appear in if_les.c, if_xx.c or if_pan.c,
 *          as a stub if that is appropriate.
 *
 * July 1990, Chris Shaker
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_les.h,v $
 * Revision 3.3.20.5  1996/09/05  01:16:42  snyder
 * CSCdi68132:  declare consts in lapb x25 atm_arp tcl
 *              save 248 out of data, 12 from image
 * Branch: California_branch
 *
 * Revision 3.3.20.4  1996/06/17  08:33:48  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.3.20.3  1996/06/16  21:15:54  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.3.20.2  1996/05/09  14:33:59  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.20.1.18.1  1996/04/27  07:15:21  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.20.1.8.1  1996/04/08  01:58:51  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.3.20.1  1996/03/18  20:42:00  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.24.1  1996/03/22  09:40:38  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.10.1  1996/03/03  21:23:59  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.3.8.1  1996/03/02  01:20:03  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.3  1996/01/30  20:57:51  vandys
 * CSCdi46080:  Deep six CS500 in source base
 *
 * Revision 3.2.24.1  1996/01/24  22:23:54  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2.32.1  1996/02/29  07:49:20  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.2  1995/11/17  17:40:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:32:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.62.1  1996/02/03  07:07:36  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.1  1995/09/11  01:56:36  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.28.1  1995/08/31  12:51:15  rlowe
 * Carryover applicable code from old Synergy_ATMlc_branch (10.3)
 * to Synalc2_branch (11.0).  Add SYNALC-specific entries to
 * other (les) platform-specific entries.
 *
 * Branch: Synalc2_branch
 *
 * Revision 2.1  1995/06/07  21:35:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if defined(XX)				/* DosXX */

#include "../../boot/src-68-c4000/pcmap.h"
#include "../../boot/src-c4000/cookie.h"
#include "../src-68-c4000/xx.h"
#include "../src-68-c4000/xx_nim.h"
#include "../src-68-c4000/xx_wrappers.h"
#include "../src-c4000/c4000_nim.h"

#endif /* XX */

#if defined(SIERRA)    	 		/* Sierra */

#include "../src-4k-c4000/pcmap.h"
#include "../../boot/src-c4000/cookie.h"
#include "../src-4k/nevasic.h"
#include "../src-4k-c4000/sierra.h"
#include "../src-4k-c4000/sierra_nim.h"
#include "../src-4k-c4000/sierra_wrappers.h"
#include "../src-c4000/c4000_nim.h"

#endif /* SIERRA */

#if defined(PAN) || defined(BRASIL) || defined(UNIX) /* PAN or UNIX tunnels */

#include "../../boot/src-68-c3000/pcmap.h"
#include "../../boot/src-68-c3000/cookie.h"
#include "../src-68-c3000/c3000.h"
#include "../src-68-c3000/c3000_wrappers.h"

#endif /* PAN */

#if defined(BRASIL)

#include "../src-68-as5200/as5200.h"
#include "../src-68-as5200/as5200_pcmap.h"
#include "../src-68-as5200/as5200_nim.h"
#include "../src-68-as5200/as5200_wrappers.h"

#endif /* BRASIL */


#if defined(SAPPHIRE)

/*
 * FIXME - use cookie.h from rommon/src when clev gets them ready
 */
#include "../src-36-c1000/pcmap.h"
#include "../../boot/src-68-c3000/cookie.h"
#include "../src-36-c1000/c1000.h"

#endif /* SAPPHIRE */

#if defined(SYNALC)                     /* SYNALC */

#include "../../boot/src-68-alc/pcmap.h"
#include "../../boot/src-68-alc/cookie.h"
#include "../src-68-alc/alc.h"
#include "../src-68-alc/alc_wrappers.h"

#endif /* SYNALC */


#define INSTANCE		idb->devctl

/* 
 * IGRP defines for serial lines 
 */
#define VISIBLE_LES_SERIAL_BANDWIDTH 1544 /* 1.544Mbps (T1) */
#define VISIBLE_LES_LSS_BANDWIDTH    115  /* Max speed for low-speed serial
				             lines on cancun platforms is
                                             (115.2 kbps) */
#define LES_SERIAL_DELAY	2000    /* 20 millisecond */

extern int lines;		/* How many lines we have shown */
extern const char not_initialized[];


/*
 * if_les.c 
 */
extern int make_ring_size(int n);
extern int log2n(int n);
extern void select_ring_size(int *rx_size, int *tx_size, int maxdgram);
extern void adj_smds_ring_size(int *rx_size);
extern void show_les_controllers(parseinfo *csb);
extern boolean bridging_on_this_interface(idbtype *idb);

/*
 * [sierra|xx|c3000]_nv.c
 */
extern long get_sys_nvsize(void);
extern uchar *get_nvbase(void);
extern ushort *get_nvmagic(void);
