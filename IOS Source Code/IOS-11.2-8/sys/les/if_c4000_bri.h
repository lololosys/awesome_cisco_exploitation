/* $Id: if_c4000_bri.h,v 3.3.62.1 1996/06/16 21:15:10 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c4000_bri.h,v $
 *------------------------------------------------------------------
 * if_c4000_bri.h -- The BRI driver related defines for the 4xxx.
 *
 * December 1993, Michael Hubenthal
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_c4000_bri.h,v $
 * Revision 3.3.62.1  1996/06/16  21:15:10  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.3  1995/11/17  17:38:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:58:02  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:33:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Prevent multiple inculsion.
 */
#ifndef __IF_C4000_BRI_H__
#define __IF_C4000_BRI_H__

#define BRI_M32_B1_CHAN_VALUE 0
#define BRI_M32_B2_CHAN_VALUE 1
#define BRI_M32_D_CHAN_VALUE  2

#define BRI_D_CHAN_RX_SIZE 4
#define BRI_D_CHAN_TX_SIZE 4

#define BRI_DCHAN_M32_XMIT_BUF 2
#define BRI_BCHAN_M32_XMIT_BUF 3

#define BRI_BCHAN_BUF_REDUCTION_FACTOR 2

/*
 * Bri test interface specific defines.
 */
#define BRI_BOTH_LOOP_SKIPPED 2
#define BRI_ONE_LOOP_SKIPPED  1
#define BRI_NO_LOOP_SKIPPED   0

#define BRI_TEST_DATA_SIZE       100
#define BRI_TEST_DATA_COUNT        5
#define BRI_TEST_DATA_GAP          2
#define BRI_TEST_DATA_MAX_SIZE  1500

/*
 * function prototypes
 */
void init_bri_idb(mcn_instance_t *, uint, uint, uint, uint);
void bri_init_idb_vectors (hwidbtype *idb);
hwidbtype *bri_m32_get_idb(uint slot, uint sub_cont, uint sub_unit);

extern void bri_c4000_parser_init(void);
#endif
