/* $Id: biga_hw.h,v 1.1.4.1 1996/05/09 15:00:47 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/biga_hw.h,v $
 *-----------------------------------------------------------------
 * biga_hw.h --  BIGA H/W Definitions & Constants
 *
 * ?
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: biga_hw.h,v $
 * Revision 1.1.4.1  1996/05/09  15:00:47  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:44  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:05  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:56  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:53:39  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:16  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************/
/*                                                                            */
/* CALYPSO DIAGNOSTICS                                                        */
/*                                                                            */
/* This file contains "BIGA H/W Definitions & Constants" that are included as */
/* part of CALYPSO Diagnostics.                                               */
/*                                                                            */
/* NOTE TO THE REVIEWER:                                                      */
/* --------------------                                                       */
/* You may find that Comments & Remarks are kept to the minimum extent, which */
/* eventually would be updated in due course of time.  The author believes in */
/* "Self-documentation" by way of meaningful names for procedures, variables  */
/* etc. and "Hierarchical programming" style which would work out better in   */
/* terms of time & productivity.  In the meantime, if you encounter any       */
/* problems in going thru the code or have any comments/suggestions, do let   */
/* the author know about it.                                                  */
/*                                                                            */
/******************************************************************************/

	/* BIGA Register Structures */
/*	struct BIGA_REGS {
#define	   NPCTL_RESET	0x01
#define	   NPCTL_BYPN	0x02
#define	   NPCTL_BACKEN	0x04
#define	   NPCTL_SBCI	0x08
#define	   NPCTL_IMOD	0x10
#define	   NPCTL_NPCI	0x20
#define	   NPCTL_THHLC	0x40
#define	   NPCTL_RHHLC	0x80
	   union {
	      struct {
	         uInt16	npctl16;
	         uChar	f[14];
	      }		reg0;
	      uInt32	npctl32;
	   }		r0;
	   struct {
	      uInt16	com;
	      uChar	f[14];
	   }		r1;
	   struct {
	      uChar	int0;
	      uChar	f[15];
	   }		r2;
	   struct {
	      uChar	int1;
	      uChar	f[15];
	   }		r3;
	   struct {
	      uInt16	npim;
	      uChar	f[14];
	   }		r4;
	   struct {
	      uInt16	nist;
	      uChar	f[14];
	   }		r5;
	   struct {
	      uChar	i0v;
	      uChar	f[15];
	   }		r6;
	   struct {
	      uChar	i1v;
	      uChar	f[15];
	   }		r7;
	   struct {
	      uChar	rsrcv;
	      uChar	f[15];
	   }		r8;
	   struct {
	      uChar	rcvev;
	      uChar	f[15];
	   }		r9;
	   struct {
	      uChar	xmtev;
	      uChar	f[15];
	   }		r10;
	   struct {
	      uChar	rdnpv;
	      uChar	f[15];
	   }		r11;
	   struct {
	      uChar	rdsbv;
	      uChar	f[15];
	   }		r12;
	   struct {
	      uChar	xmtdv;
	      uChar	f[15];
	   }		r13;
	   struct {
	      uChar	mfnpv;
	      uChar	f[15];
	   }		r14;
	   struct {
	      uChar	lev;
	      uChar	f[15];
	   }		r15;
	   struct {
	      uChar	aev;
	      uChar	f[15];
	   }		r16;
	   struct {
	      uChar	cstat;
	      uChar	f[15];
	   }		r17;
	   struct {
	      uChar	parv;
	      uChar	f[15];
	   }		r18;
	   struct {
	      uInt16	sist;
	      uChar	f[14];
	   }		r19;
	   struct {
	      uInt16	hica;
	      uChar	f[14];
	   }		r20;
	   struct {
	      uInt16	hicb;
	      uChar	f[14];
	   }		r21;
	   struct {
	      uChar	hicc;
	      uChar	f[15];
	   }		r22;
	   struct {
	      uInt16	upad;
	      uChar	f[14];
	   }		r23;
	   struct {
	      uInt16	dctrl;
	      uChar	f[14];
	   }		r24;
	   struct {
	      uChar	dstat;
	      uChar	f[15];
	   }		r25;
	   struct {
	      uChar	dctrl2;
	      uChar	f[15];
	   }		r26;
	   struct {
	      uInt16	pctrl;
	      uChar	f[14];
	   }		r27;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	unused;
	      }		thead;
	      uChar	f1[4];
	      struct {
	         uInt16	high;
	         uInt16	unused
	      }		thead1;
	      uChar	f2[4];
	   }		r28;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	unused;
	      }		ttail;
	      uChar	f1[4];
	      struct {
	         uInt16	high;
	         uInt16	unused;
	      }		ttail1;
	      uChar	f2[4];
	   }		r29;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	unused;
	      }		ttmph;
	      uChar	f1[4];
	      struct {
	         uInt16	high;
	         uInt16	unused;
	      }		ttmph1;
	      uChar	f2[4];
	   }		r30;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	unused;
	      }		tptr;
	      uChar	f1[4];
	      struct {
	         uInt16	high;
	         uInt16	unused;
	      }		tptr1;
	      uChar	f2[4];
	   }		r31;
	   struct {
	      uChar	tqsel;
	      uChar	f[15];
	   }		r32;
	   struct {
	      uInt16	tlen;
	      uChar	f[14];
	   }		r33;
	   struct {
	      uInt16	tdsc;
	      uChar	f[14];
	   }		r34;
	   struct {
	      uInt16	pstat;
	      uChar	f[14];
	   }		r35;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	unused;
	      }		rdesc;
	      uChar	f1[4];
	      struct {
	         uInt16	high;
	         uInt16	unused;
	      }		rdesc;
	      uChar	f2[4];
	   }		r36;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	unused;
	      }		rhead;
	      uChar	f1[4];
	      struct {
	         uInt16	high;
	         uInt16	unused;
	      }		rhead1
	      uChar	f2[4];
	   }		r37;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	unused;
	      }		rtail;
	      uChar	f1[4];
	      struct {
	         uInt16	high;
	         uInt16	unused;
	      }		rtail;
	      uChar	f2[4];
	   }		r38;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	unused;
	      }		rtmph;
	      uChar	f1[4];
	      struct {
	         uInt16	high;
	         uInt16	unused;
	      }		rtmph;
	      uChar	f2[4];
	   }		r39;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	unused;
	      }		rplen;
	      uChar	f1[4];
	      struct {
	         uInt16	high;
	         uInt16	unused;
	      }		rplen;
	      uChar	f2[4];
	   }		r40;
	   struct {
	      uInt16	rtlen;
	      uChar	f[14];
	   }		r41;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	unused;
	      }		rptr;
	      uChar	f1[4];
	      struct {
	         uInt16	high;
	         uInt16	unused;
	      }		rptr;
	      uChar	f2[4];
	   }		r42;
	   struct {
	      uInt16	rlen;
	      uChar	f[14];
	   }		r43;
	   struct {
	      uChar	fltr;
	      uChar	f[15];
	   }		r44;
	   struct {
	      uChar	fc;
	      uChar	f[15];
	   }		r45;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	high;
	      }		sfq;
	      uChar	f[12];
	   }		r46;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	high;
	      }		sfqt;
	      uChar	f[12];
	   }		r47;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	high;
	      }		rq;
	      uChar	f[12];
	   }		r48;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	high;
	      }		stq;
	      uChar	f[12];
	   }		r49;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	high;
	      }		stqt;
	      uChar	f[12];
	   }		r50;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	high;
	      }		atq0;
	      uChar	f[12];
	   }		r51;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	high;
	      }		atq0t;
	      uChar	f[12];
	   }		r52;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	high;
	      }		atq1;
	      uChar	f[12];
	   }		r53;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	high;
	      }		atq1t;
	      uChar	f[12];
	   }		r54;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	high;
	      }		atq2;
	      uChar	f[12];
	   }		r55;
	   struct {
	      struct {
	         uInt16	low;
	         uInt16	high;
	      }		atq2t;
	      uChar	f[12];
	   }		r56;
	   struct {
	      uChar	clrdma0;
	      uChar	f[15];
	   }		r57;
	   struct {
	      uChar	clrdma1;
	      uChar	f[15];
	   }		r58;
	   struct {
	      uChar	rev;
	      uChar	f[15];
	   }		r59;
	   struct {
	      uInt32	cfg;
	      uChar	f[12];
	   }		r60;
	};*/

	struct BIGA_RBD {
	   uChar	*rbuf_ptr;
	   uInt32	rbuf_usable_len;
	   union {
	      struct {
	         unsigned	m : 1;
	         unsigned	rbuf_used_len : 15;
	         uInt16	dup;
	      }		info;
	      uInt32	info_u;
	   }		rcv_info;
	   uInt32	rcv_desc;
	   struct BIGA_RBD *rbd_ptr;
	};

	struct BIGA_TBD {
	   uChar	*tbuf_ptr;
	   union {
	      struct {
	         unsigned	m : 1;
	         unsigned	d : 1;
	         unsigned	x : 1;
	         unsigned	f : 1;
	         unsigned	a : 1;
	         unsigned	qcode : 3;
	         uChar	ud;
	         uInt16	tbuf_len;
	      }		control;
	      uInt32	control_u;
	   }		tbd_control;
	   struct BIGA_TBD *tbd_ptr;
	};

#define	QC_SPECIAL	2
#define	QC_RX     	3
#define	QC_SYNC   	4
#define	QC_ASYNC0 	5
#define	QC_ASYNC1 	6
#define	QC_ASYNC2 	7

#define	DMA_DUMP	0x8000
#define	DMA_FIXED	0x4000
#define	DMA_FCP1	0x2000
#define	DMA_FCP0	0x1000
#define	DMA_RSTRT	0x0800
#define	DMA_RXA		0x0400
#define	DMA_TSTRT	0x0200
#define	DMA_TXA		0x0100
#define	DMA_RXEN	0x0080
#define	DMA_TXEN	0x0040
#define	DMA_BSIZ16	0x003F
#define	DMA_BSIZ8 	0x001F
#define	DMA_BSIZ4 	0x000F
#define	DMA_BSIZ2 	0x0007
#define	DMA_BSIZ1 	0x0003
#define	DMA_BSIZH 	0x0001
#define	DMA_BSIZ0 	0x0000

#define	SIST_XMTD	0x0010
#define	SIST_RDSB	0x0008

#ifndef RISC
#define	RX_DESC_P_START	0x0900FFFC
#define	RX_DESC_START	0x09000000

#define	TX_DESC_P_START	0x0901FFFC
#define	TX_DESC_START	0x09010000

#define	RX_BUF_START	0x0E000000
#define	RX_BUF_SIZE	0x4000

#define	TX_BUF_START	0x0E100000
#define	TX_BUF_SIZE	0x4000
#else /* RISC */
#define	RX_DESC_P_START	0xA902FFFC
#define	RX_DESC_START	0xA9020000

#define	TX_DESC_P_START	0xA903FFFC
#define	TX_DESC_START	0xA9030000

#define	RX_BUF_START	0xAE200000
#define	RX_BUF_SIZE	0x4000

#define	TX_BUF_START	0xAE300000
#define	TX_BUF_SIZE	0x4000
#endif /* RISC */

