/* $Id: mac_hw.h,v 1.1.4.1 1996/05/09 15:01:10 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/mac_hw.h,v $
 *------------------------------------------------------------------
 * mac_hw.h -- MAC H/W Definitions & Constants
 *
 * ?
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mac_hw.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:10  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:09  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:33  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:19  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:54:05  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:38  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************/
/*                                                                            */
/* CALYPSO DIAGNOSTICS                                                        */
/*                                                                            */
/* This file contains "MAC H/W Definitions & Constants" that are included as  */
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

#define	FDDI_MIN_PACKETSIZE	13
#define	FDDI_MAX_PACKETSIZE	4489
#define	FDDI_DEFAULT_PACKETSIZE	256

#define	FDDI_MIN_PACKETSIZE_LGEN	13
#define	FDDI_MAX_PACKETSIZE_LGEN	4000
#define	FDDI_DEFAULT_PACKETSIZE_LGEN	256

/* MAC Buffer Memory Pointers */
#define	EACB_VAL_PTR	0x19
#define	EARV_VAL_PTR	(0x0BFFC >> 2) 	/* ~48K */
#define	EAS_VAL_PTR	(0x10FFC >> 2)	/*  20K */
#define	EAA0_VAL_PTR	(0x15FFC >> 2)	/*  20K */
#define	EAA1_VAL_PTR	(0x1AFFC >> 2)	/*  20K */
#define	EAA2_VAL_PTR	(0x1FFFC >> 2)	/*  20K */

#define	EACB_VAL_PTR_LGEN	0x19
#define	EARV_VAL_PTR_LGEN	(0x0FFFC >> 2) /* ~64K */
#define	EAS_VAL_PTR_LGEN	EARV_VAL_PTR_LGEN
#define	EAA0_VAL_PTR_LGEN	(0x1FFFC >> 2) /*  64K */
#define	EAA1_VAL_PTR_LGEN	EAA0_VAL_PTR_LGEN
#define	EAA2_VAL_PTR_LGEN	EAA1_VAL_PTR_LGEN

/* MAC Register Structures */
/*struct MAC_REGS {
  union {
    struct {
      union {
	uInt16 cmdreg1;
	uInt16 st1u;
      } addr0;
	uInt16 unused1;
      union {
	uInt16 cmdreg2;
	uInt16 st1l;
      } addr1;
	uInt16 unused2;
    } lw;
    uInt32 reg0[2];
  }r0;

  union {
    struct {
      uInt16 st2u;
      uInt16 unused1;
      uInt16 st2l;
      uInt16 unused2;
    } lw;
    uInt32 reg1[2];
  }r1;
  union {
    struct {
      uInt16 imsk1u;
      uInt16 unused1;
      uInt16 imsk1l;
      uInt16 unused2;
    } lw;
    uInt32 reg2[2];
  }r2;
  union {
    struct {
      uInt16 imsk2u;
      uInt16 unused1;
      uInt16 imsk2l;
      uInt16 unused2;
    } lw;
    uInt32 reg3[2];
  }r3;
  union {
    struct {
      uInt16 said;
      uInt16 unused1;
      uInt16 laim;
      uInt16 unused2;
    } lw;
    uInt32 reg4[2];
  }r4;
  union {
    struct {
      uInt16 laic;
      uInt16 unused1;
      uInt16 lail;
      uInt16 unused2;
    } lw;
    uInt32 reg5[2];
  }r5;
  union {
    struct {
      uInt16 sagp;
      uInt16 unused1;
      uInt16 lagm;
      uInt16 unused2;
    } lw;
    uInt32 reg6[2];
  }r6;
  union {
    struct {
      uInt16 lagc;
      uInt16 unused1;
      uInt16 lagl;
      uInt16 unused2;
    } lw;
    uInt32 reg7[2];
  }r7;
  union {
    struct {
      uInt16 mdreg1;
      uInt16 unused1;
      uInt16 stmchn;
      uInt16 unused2;
    } lw;
    uInt32 reg8[2];
  }r8;
  union {
    struct {
      uInt16 mir1;
      uInt16 unused1;
      uInt16 mir0;
      uInt16 unused2;
    } lw;
    uInt32 reg9[2];
  }r9;
  union {
    struct {
      uInt16 tmax;
      uInt16 unused1;
      uInt16 tvx;
      uInt16 unused2;
    } lw;
    uInt32 reg10[2];
  }r10;
  union {
    struct {
      uInt16 trt;
      uInt16 unused1;
      uInt16 tht;
      uInt16 unused2;
    } lw;
    uInt32 reg11[2];
  }r11;
  union {
    struct {
      uInt16 tneg;
      uInt16 unused1;
      uInt16 tmrs;
      uInt16 unused2;
    } lw;
    uInt32 reg12[2];
  }r12;
  union {
    struct {
      uInt16 treq0;
      uInt16 unused1;
      uInt16 treq1;
      uInt16 unused2;
    } lw;
    uInt32 reg13[2];
  }r13;
  union {
    struct {
      uInt16 pri0;
      uInt16 unused1;
      uInt16 pri1;
      uInt16 unused2;
    } lw;
    uInt32 reg14[2];
  }r14;
  union {
    struct {
      uInt16 pri2;
      uInt16 unused1;
      uInt16 tsync;
      uInt16 unused2;
    } lw;
    uInt32 reg15[2];
  }r15;
  union {
    struct {
      uInt16 mdreg2;
      uInt16 unused1;
      uInt16 frmthr;
      uInt16 unused2;
    } lw;
    uInt32 reg16[2];
  }r16;
  union {
    struct {
      uInt16 eacb;
      uInt16 unused1;
      uInt16 earv;
      uInt16 unused2;
    } lw;
    uInt32 reg17[2];
  }r17;
  union {
    struct {
      uInt16 eas;
      uInt16 unused1;
      uInt16 eaa0;
      uInt16 unused2;
    } lw;
    uInt32 reg18[2];
  }r18;
  union {
    struct {
      uInt16 eaa1;
      uInt16 unused1;
      uInt16 eaa2;
      uInt16 unused2;
    } lw;
    uInt32 reg19[2];
  }r19;
  union {
    struct {
      uInt16 sacl;
      uInt16 unused1;
      uInt16 sabc;
      uInt16 unused2;
    } lw;
    uInt32 reg20[2];
  }r20;
  union {
    struct {
      uInt16 wpxsf;
      uInt16 unused1;
      uInt16 rpxsf;
      uInt16 unused2;
    } lw;
    uInt32 reg21[2];
  }r21;
  union {
    struct {
      uInt16 unused;
      uInt16 unused1;
      uInt16 rpr;
      uInt16 unused2;
    } lw;
    uInt32 reg22[2];
  }r22;
  union {
    struct {
      uInt16 wpr;
      uInt16 unused1;
      uInt16 swpr;
      uInt16 unused2;
    } lw;
    uInt32 reg23[2];
  }r23;
  union {
    struct {
      uInt16 wpxs;
      uInt16 unused1;
      uInt16 wpxa0;
      uInt16 unused2;
    } lw;
    uInt32 reg24[2];
  }r24;
  union {
    struct {
      uInt16 wpxa1;
      uInt16 unused1;
      uInt16 wpxa2;
      uInt16 unused2;
    } lw;
    uInt32 reg25[2];
  }r25;
  union {
    struct {
      uInt16 swpxs;
      uInt16 unused1;
      uInt16 swpxa0;
      uInt16 unused2;
    } lw;
    uInt32 reg26[2];
  }r26;
  union {
    struct {
      uInt16 swpxa1;
      uInt16 unused1;
      uInt16 swpxa2;
      uInt16 unused2;
    } lw;
    uInt32 reg27[2];
  }r27;
  union {
    struct {
      uInt16 rpxs;
      uInt16 unused1;
      uInt16 rpxa0;
      uInt16 unused2;
    } lw;
    uInt32 reg28[2];
  }r28;
  union {
    struct {
      uInt16 rpxa1;
      uInt16 unused1;
      uInt16 rpxa2;
      uInt16 unused2;
    } lw;
    uInt32 reg29[2];
  }r29;
  union {
    struct {
      uInt16 marr;
      uInt16 unused1;
      uInt16 marw;
      uInt16 unused2;
    } lw;
    uInt32 reg30[2];
  }r30;
  union {
    struct {
      uInt16 mdru;
      uInt16 unused1;
      uInt16 mdrl;
      uInt16 unused2;
    } wd;
    uInt32 reg31[2];
  } data_reg;  

  union {
    struct {
      uInt16 tmsync;
      uInt16 unused1;
      uInt16 fcntr;
      uInt16 unused2;
    } lw;
    uInt32 reg32[2];
  }r32;
  union {
    struct {
      uInt16 lcntr;
      uInt16 unused1;
      uInt16 ecntr;
      uInt16 unused2;
    } lw;
    uInt32 reg33[2];
  }r33;
};*/

/*
	Commands for Command Register 1 (CMDREG1)
*/
#define SW_RESET		0x01
#define IRMEMWI			0x02
#define IRMEMWO			0x03
#define IDLE_LISTEN		0x04
#define CLAIM_LISTEN		0x05
#define BEACON_LISTEN		0x06
#define LOAD_TVX		0x07
#define NON_RES_TOKEN		0x08
#define ENTER_NON_RES_TOKEN	0x09
#define ENTER_RES_TOKEN		0x0a
#define RES_TOKEN		0x0b
#define SEND_UNRES_TOKEN	0x0c
#define SEND_RES_TOKEN		0x0d
#define ENTER_SEND_IMM		0x0e
#define EXIT_SEND_IMM		0x0f
#define CLR_SYNC_LOCK		0x11
#define CLR_ASYNC_LOCK0		0x12
#define CLR_ASYNC_LOCK1		0x14
#define CLR_ASYNC_LOCK2		0x18
#define CLR_RCV_LOCK		0x20
#define CLR_ALL_LOCKS		0x3f


/*
	Commands for Command Register 2 (CMDREG2)
*/
#define	TRANSMIT_SYNC		0x01
#define TRANSMIT_ASYNC0		0x02
#define TRANSMIT_ASYNC1		0x04
#define TRANSMIT_ASYNC2		0x08
#define ABORT_CURNT_XMIT	0x10
#define RESET_XMIT_QUEUES	0x20
#define SET_TAG_BIT		0x30
#define EN_RCV_SINGLE_FRM	0x40


/*
	Status register 1 upper (ST1U).
*/
#define SXMTABT				0x8000
#define STXABRA2			0x4000
#define STXABRA1			0x2000
#define STXABRA0			0x1000
#define STXABRS				0x0800
#define STBFLS				0x0400
#define STBFLA				0x0200
#define STEXDONS			0x0100
#define STECFRMA2			0x0080
#define STECFRMA1			0x0040
#define STECFRMA0			0x0020
#define STECFRMS			0x0010
#define STEFRMA2			0x0008
#define STEFRMA1			0x0004
#define STEFRMA0			0x0002
#define STEFRMS				0x0001


/*
	Status register 1 lower (ST1L)
*/
#define STBURA2				0x8000
#define STBURA1				0x4000
#define STBURA0				0x2000
#define STBURS				0x1000
#define SPCEPDA2			0x0800
#define SPCEPDA1			0x0400
#define SPCEPDA0			0x0200
#define SPCEPDS				0x0100
#define STXINFLA2			0x0080
#define STXINFLA1			0x0040
#define STXINFLA0			0x0020
#define STXINFLS			0x0010
#define SQLCKA2				0x0008
#define SQLCKA1				0x0004
#define SQLCKA0				0x0002
#define SQLCKS				0x0001


/*
	Status register 2 upper (ST2U)
*/
#define	SRCOMP				0x8000
#define SRBMT				0x4000
#define SRABT				0x2000
#define SRBFL				0x1000
#define SRCVOVR				0x0800
#define SRCVFRM				0x0400
#define SRFRCTOV			0x0200
#define SNFSLD				0x0100
#define SERRSF				0x0080
#define SCLM				0x0040
#define SMYCLM				0x0020
#define SHICLM				0x0010
#define SLOCLM				0x0008
#define SBEC				0x0004
#define SMYBEC				0x0002
#define SOTRBEC				0x0001


/*
	Status register 2 lower (ST2L)
*/
#define SRNGOP				0x8000
#define SMULTDA				0x4000
#define STKERR				0x2000
#define STKISS				0x1000
#define STVXEXP				0x0800
#define STRTEXP				0x0400
#define SMISFRM				0x0200
#define SADET				0x0100
#define SPHINV				0x0080
#define SLSTCTR				0x0040
#define SERRCTR				0x0020
#define SFRMCTR				0x0010
#define SSIFG				0x0008
#define SDUPCLM				0x0004
#define STRTEXR				0x0002


/*
	Mode Register 1 (MDREG1)
*/
#define SNGLFRM			0x8000
#define INITIALIZE_MAC		0x0000
#define MEMORY_ACTIVE		0x1000
#define ON_LINE_SPECIAL		0x2000
#define MAC_ONLINE		0x3000
#define INTERNAL_LOOPBACK	0x4000
#define EXTERNAL_LOOPBACK	0x7000
#define SELSA			0x0800
#define ADDR_DET_MODE0		0x0000		/* Rec DA=MA 		*/
#define ADDR_DET_MODE1		0x0100		/* Rec DA=MA or SA=MA	*/
#define ADDR_DET_MODE2		0x0200		
#define ADDR_DET_MODE3		0x0300
#define ADDR_DET_MODE4		0x0400
#define ADDR_DET_MODE5		0x0600
#define ADDR_DET_MODE6		0x0700		/* Promiscuous		*/
#define SELRA			0x0080
#define DISCRY			0x0040
#define EXGPA1			0x0020
#define EXGPA0			0x0010
#define LOCKTX			0x0008
#define FULL_HALF		0x0004
#define XMTINH			0x0002


#define	MAC_MODE_MASK		0x7000		/* Mode bits mask	*/


/*
	Mode Register 2 (MDREG2) bit definitions.
*/
#define BMMODE				0x8000
#define STRPFCS				0x4000
#define CHKPAR				0x2000
#define PARITY				0x1000
#define LSB				0x0800
#define RXFBB1				0x0400
#define RXFBB0				0x0200
#define ENHSRQ				0x0100
#define ENNPRQ				0x0080
#define SYNPRQ				0x0040
#define SYMCTL				0x0020
#define RCVERR				0x0010
#define AFULL3				0x0008
#define AFULL2				0x0004
#define AFULL1				0x0002
#define AFULL0				0x0001
/*
	State Machine register (STMCHN) bit definitions.
*/
#define MDRTAG				0x0004
#define SNPPND				0x0008
#define TSTATE0				0x0010
#define TSTATE1				0x0020
#define TSTATE2				0x0040
#define RSTATE0				0x0080
#define RSTATE1				0x0100
#define RSTATE2				0x0200
#define TM0				0x0400
#define TM1				0x0800
#define SNDIMM				0x1000

#define RESTRTICTED_TOKEN_MODE	(TM0 | TM1)
/*
	Definitions for the default timer values.
*/
#define	FORMACP_DEFAULT_TVX	0x85		/* 2.5 msec		*/

#define FORMACP_DEFAULT_TMAX	0x03c7		/* 165.29664 msec 	*/
/*#define FORMACP_DEFAULT_TMAX	0x0001*/	/* 167.7696 msecs */


/*
	Received frame status word definitions.
*/
#define MSVALID			0x8000		/* Valid frame.		*/
#define MSRABT			0x4000		/* Receive aborted.	*/
#define SOURCE_ROUTING		0x1000		/* MSB of SA set	*/
#define E_INDICATOR		0x0800		/* E indicator received	*/
#define A_INDICATOR		0x0400		/* A indicator received	*/
#define C_INDICATOR		0x0200		/* C indicator received	*/
#define SFRMERR			0x0100		/* Frame error occurred.*/
#define SADRRG			0x0080		/* MAC Address match	*/
#define SFRMTY2			0x0040		/* Frame type bit 2	*/
#define SFRMTY1			0x0020		/* Frame type bit 1	*/
#define SFRMTY0			0x0010		/* Frame type bit 0	*/
#define ERFBB1			0x0002		/* End of frame byte bound */
#define ERFBB0			0x0001		/* End of frame byte bound */


/*
	Definitions for frame type bits.
*/
#define ASYNC_SMT_VOID		0x0
#define ASYNC_LLC		(SFRMTY0)
#define ASYNC_IMPLEMENTOR	(SFRMTY1)
#define SYNC_MAC		(SFRMTY2)
#define SYNC_LLC		(SFRMTY0 | SFRMTY2)
#define SYNC_IMPLEMENTOR	(SFRMTY1 | SFRMTY2)


/*
	EAC bit mask.
*/
#define EAC_BIT_MASK		(E_INDICATOR | A_INDICATOR | C_INDICATOR)


/*
	A mask for looking at frame type.
*/
#define FRAME_TYPE_MASK		(SFRMTY0 | SFRMTY1 | SFRMTY2)

#define BEACON_FC 	0xc2
#define CLAIM_FC	0xc3
#define VOID_FC		0x40
#define LLC_FC		0x50		/* seg */

/*
	Xmit descriptor definitions.
*/
#define	MORE_BIT	0x80000000
#define RES_BITS	0x60000000
#define RES_BIT_ST	0x40000000
#define TXFBB1		0x10000000
#define TXFBB0		0x08000000
#define XDONE		0x04000000
#define NFCS		0x02000000
#define XMTABT		0x01000000
#define LENGTH_MASK     0x0000ffff

#define SMT_XMIT_HDRH	0x4000


/*
	Xmit Pointer definitions.
*/
#define PTR_CNTRL	0xa0000000
#define PTR_H		0xa000
#define PTR_MASK	0x0000ffff

