/* $Id: if_les_biga.h,v 3.2.62.1 1996/05/09 14:47:00 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/syn-biga/if_les_biga.h,v $
 *------------------------------------------------------------------
 * if_les_biga.h - header file for Synergy In-band I/F (BIGA).
 *
 * May 26 1995, Jin-Jang Chen
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_les_biga.h,v $
 * Revision 3.2.62.1  1996/05/09  14:47:00  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.92.1  1996/04/27  07:26:42  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.82.1  1996/04/08  02:09:57  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.68.1  1996/03/22  09:44:11  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.52.1  1996/03/03  21:25:04  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.50.1  1996/03/02  01:21:20  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.34.1  1996/02/29  07:50:25  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.2  1995/11/17  18:57:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:32:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:02:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * External function declarations:
 */

/* if_les_biga.c */

extern void init_biga_idb_common (hwidbtype *);
extern void biga_if_init (hwidbtype *, boolean);
extern void biga_stop (void);
extern void biga_restart (hwidbtype *);
extern void biga_check (hwidbtype *);
extern void biga_shutdown (hwidbtype *, int);
extern void biga_add_multicast_address (idbtype *, uchar *, uchar);
extern void biga_add_multicast_range (idbtype *, uchar *, uchar *);
extern void biga_setup_bridging (hwidbtype *);
extern void biga_input_error (hwidbtype *);
extern void biga_buffer_ring_adjust (hwidbtype *);
extern void biga_fastsend (hwidbtype *, paktype *);
extern void biga_tx_start (hwidbtype *);
extern void biga_enable (hwidbtype *);
extern int biga_txqlength (hwidbtype *);
extern void biga_periodic (hwidbtype *);
extern boolean biga_show_controller (hwidbtype *, parseinfo *);

/* if_synergy_biga.c */

extern void biga_subsys_init (subsystype *);
extern void biga_if_reset (hwidbtype *);
extern void init_biga_idb (int);
extern void biga_get_default_hw_address (hwidbtype *, uchar *);

#define MAX_BIGA	1514
