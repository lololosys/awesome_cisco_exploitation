/* $Id: terms.h,v 1.1.4.1 1996/05/09 15:00:12 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarinc/terms.h,v $
 *-----------------------------------------------------------------
 * terms.h --  
 *
 * ?
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: terms.h,v $
 * Revision 1.1.4.1  1996/05/09  15:00:12  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:09  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:21  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:19  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:37:48  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:25  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _TERMS_
#define _TERMS_

#define OP_INIT 1
#define OP_TX 2
#define OP_RX 3
#define OP_RXRDY 4
#define OP_TXRDY 5
#define OP_BAUD 6

#ifdef LANGUAGE_C
typedef unsigned char byte;

struct p8530info {
	byte *siobase;
	byte wr3[2];
	byte wr5[2];
	} ;

struct sableinfo {
	byte *siobase;
	} ;

struct pttyRec {
	unsigned long hr;
	unsigned long sr;
	};

struct pttyinfo {
	byte *siobase;
	struct pttyRec *tx;
	struct pttyRec *rx;
	} ;

struct p2681info {
	byte *siobase;
	byte brate[2];
	byte curacr;
	} ;


struct ps2struct {
	int led_state;
	int sh_state;
	int ctrl_state;
	int curx;
	int cury;
	int rxbuf;
	int prev;
	};
#endif


#endif /* _TERMS_ */
