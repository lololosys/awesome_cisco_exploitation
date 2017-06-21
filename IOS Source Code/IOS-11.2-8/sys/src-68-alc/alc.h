/* $Id: alc.h,v 3.2.62.1 1996/05/09 14:40:54 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/src-68-alc/alc.h,v $
 *------------------------------------------------------------------
 * alc.h -- Common definitions for all interfaces for the Synergy ATM
 *	    line card (organization based on c3000)
 *
 * April 13, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: alc.h,v $
 * Revision 3.2.62.1  1996/05/09  14:40:54  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.92.1  1996/04/27  07:25:08  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.82.1  1996/04/08  02:08:41  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.68.1  1996/03/22  09:43:27  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.52.1  1996/03/03  21:24:41  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.50.1  1996/03/02  01:20:52  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.34.1  1996/02/29  07:50:04  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.2  1995/11/17  18:42:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:24:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.62.1  1996/02/03  07:08:20  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.1  1995/09/11  01:56:59  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.28.1  1995/08/22  17:51:26  lcheung
 * move file from Synergy_atmlc_branch to Synalc2_branch
 * Branch: Synalc2_branch
 *
 * Revision 2.1  1995/06/07  22:54:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Notes on buffer sizing:
 * The largest packet we must handle is an HDLC bridged, maximum sized
 * Ethernet datagram.  The header in this case is 4 bytes, and the data
 * may be up to 1517 bytes (European interpretation of IEEE).  If we
 * allocate a data buffer of 1520 bytes, this datagram will be received
 * just fine by the HDLC receiver.  The total size of this datagram is
 * 1521 bytes, so we should allow (MAXSERIALSIZE+ENCAPBYTES) == 1524
 * byte datagrams to be transmitted from process level.
 */
#define PAN_MAXDGRAM		1524	/* bytes, for buffer size checking */

#define BRIDGECODE_BYTES 4

/*
 * Maximum size of a RX or TX buffer ring:
 */
#define MAX_RING		16

/*
 * Define the Tx and Rx ring sizes for different mtus
 */
#define MAX_TXRING_BIG		4
#define MAX_RXRING_BIG		16
#define MAX_TXRING_LARGE	4
#define MAX_RXRING_LARGE        8
#define MAX_TXRING_HUGE		2
#define MAX_RXRING_HUGE		4

#define RXBOOT_MAX_TXRING	1
#define RXBOOT_MAX_RXRING	4

/* define multipliers for private buffer pool allocations */
 
#define	PRIV_NUM_BUF(rx_size)	(rx_size * 2)
#define	PRIV_NUM_CACHE(rx_size)	(rx_size / 2)

/*
 * Interrupt array structure
 */
#define	ALC_INTR_CONS	0		/* Must always be first. */
#define	ALC_INTR_BRG	(ALC_INTR_CONS)
#define	ALC_INTR_TSAR	1
#define	ALC_INTR_RSAR	2
#define	ALC_INTR_LCPT	3
#define	ALC_INTR_LCPR	4
#define	ALC_INTR_NTC	5
#define	ALC_MAX_INTER	6		/* Nbr elements in array. */

#define	SIZ_INTR_ARRY	(4*4)		/* For structure below. */
#define	LOG2_INTR_ARRY	(4)

#ifndef	ASMINCLUDE
struct interrupt_array {		/* Like size to be power of 2. */
    ulong	function;
    hwidbtype	*idb1;
    hwidbtype	*idb2;
    ulong	icount ;
};
typedef struct interrupt_array interrupt_array_t;
extern interrupt_array_t alc_int_array [ALC_MAX_INTER];


/*
 * Shared externals:
 */
extern hwidbtype *network_a_idb;	/* IDB pointers */
extern hwidbtype *network_b_idb;
extern hwidbtype *network_c_idb;
extern hwidbtype *network_c0_idb;
extern hwidbtype *network_c1_idb;
extern hwidbtype *network_d_idb;

extern cookie_t *cookie;	/* Address of cookie PROM */

/*
 * Prototypes
 */
extern boolean flash_assert_vpp(void);
extern boolean flash_deassert_vpp(void);
extern boolean flash_check_vpp(void);
extern void    StackBindVector (long VecNum, long index, void * routine) ;
#endif
