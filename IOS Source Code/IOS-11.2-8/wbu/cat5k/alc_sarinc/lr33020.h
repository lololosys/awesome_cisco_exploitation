/* $Id: lr33020.h,v 1.1.4.1 1996/05/09 15:00:07 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sarinc/lr33020.h,v $
 *-----------------------------------------------------------------
 * lr33020.h --  lr33020 & lr33120 definitions
 *
 * ?
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lr33020.h,v $
 * Revision 1.1.4.1  1996/05/09  15:00:07  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:03  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:14  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:13  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  02:37:42  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:19  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
** lr33020.h -- lr33020 & lr33120 definitions
*/

#ifndef _LR33020_
#define _LR33020_

#ifdef LANGUAGE_C
/* cp2 control registers */
#define C2_SRCSKEW	0	/* graphics source skew */
#define C2_SRCSHIFT	1	/* graphics source shift */
#define C2_COLOR0	2	/* COLOR0 data */
#define C2_COLOR1	3	/* COLOR1 data */
#define C2_GCPCNTRL	4	/* graphics control */
#define C2_RASTEROP	5	/* RasterOp logical operation */
#define C2_PLANEMASK	6	/* Plane mask */
#define C2_CONFIG	9	/* video control */
#define C2_PSSTAT	24	/* I/O port status */
#define C2_PSCOMM	25	/* I/O port command */

/* cp2 data registers */
#define C2_SCRSTART	0	/* Screen start address */
#define C2_SCRPITCH	1	/* Screen pitch */
#define C2_HWCRSRSTART	2	/* Hardware cursor start addr */
#define C2_HWCRSRCURR	3	/* Current hardware cursor addr */
#define C2_SAMEXTENT	4	/* VRAM serial-access-mode extent */
#define C2_NXTDISP	5	/* Next display addr */
#define C2_CURDISP	6	/* Current display address */
#define C2_LINECOUNT	7	/* Video line count */
#define C2_VBLKSIZE	8	/* Video block size */
#define C2_SRCLINE	9	/* Src start of next line addr */
#define C2_SRCCURR	10	/* Source current addr */
#define C2_SRCPITCH	11	/* Source pitch */
#define C2_DESTLINE	12	/* Dest start of next line addr */
#define C2_DESTCURR	13	/* Destination current addr */
#define C2_DESTPITCH	14	/* Destination pitch */
#define C2_GBLKSIZE	15	/* Graphics src read block size */
#define C2_SERPULS	16	/* Comp sync serration pulse width */
#define C2_HLINTR	17	/* Horizontal interrupt line */
#define C2_BLANKS	18	/* Blank start */
#define C2_SYNCS	19	/* SYNC start */
#define C2_SYNCRESET	20	/* SYNC reset */
#define C2_BLANKE	21	/* Blank end */
#define C2_HWCRSR	22	/* Hardware cursor location */
#define C2_VHWCONFIG	23	/* Video hardware config bits */
#define C2_PSTXB	24	/* PS/2 serial port Tx buffer */
#define C2_PSRCVB	25	/* PS/2 serial port Rx buffer */
#define C2_SRCDATA	28	/* Source data */
#define C2_DESTDATA	29	/* Destination data */
#define C2_LEFTMASK	30	/* Left edge mask */
#define C2_RIGHTMASK	31	/* Right edge mask */

#define SSTEP		0x4a00ffff
#define SBSTEP		0x4a40ffff
#define WSTEP		0x4a80ffff
#define WSTEP_L		0x4a88ffff
#define WSTEP_R		0x4a84ffff
#define WSTEP_L_R	0x4a8cffff
#define WSTEP_S		0x4aa0ffff
#define WSTEP_S_L	0x4aa8ffff
#define WSTEP_S_R	0x4aa4ffff
#define WSTEP_S_L_R	0x4aacffff
#define WSTEP_SB	0x4ab0ffff
#define WSTEP_SB_L	0x4ab8ffff
#define WSTEP_SB_R	0x4ab4ffff
#define WSTEP_SB_L_R	0x4abcffff
#define WSTEP_FOUR	0x4a82ffff
#define WSTEP_BFOUR	0x4a81ffff
#define BSTEP_BFOUR	0x4ac1ffff
#define BSTEP		0x4ac0ffff
#define BSTEP_L		0x4ac8ffff
#define BSTEP_R		0x4ac4ffff
#define BSTEP_L_R	0x4accffff
#define BSTEP_S		0x4ae0ffff
#define BSTEP_S_L	0x4ae8ffff
#define BSTEP_S_R	0x4ae4ffff
#define BSTEP_S_L_R	0x4aecffff
#define BSTEP_SB	0x4af0ffff
#define BSTEP_SB_L	0x4af8ffff
#define BSTEP_SB_R	0x4af4ffff
#define BSTEP_SB_L_R	0x4afcffff
#ifdef LR33120
#define WSTEP_I		0x4a807fff
#define WSTEP_L_I	0x4a887fff
#define WSTEP_R_I	0x4a847fff
#define WSTEP_L_R_I	0x4a8c7fff
#define WSTEP_S_I	0x4aa07fff
#define WSTEP_S_L_I	0x4aa87fff
#define WSTEP_S_R_I	0x4aa47fff
#define WSTEP_S_L_R_I	0x4aac7fff
#define WSTEP_SB_I	0x4ab07fff
#define WSTEP_SB_L_I	0x4ab87fff
#define WSTEP_SB_R_I	0x4ab47fff
#define WSTEP_SB_L_R_I	0x4abc7fff
#define WSTEP_FOUR_I	0x4a827fff
#define WSTEP_BFOUR_I	0x4a817fff
#define BSTEP_BFOUR_I	0x4ac17fff
#define BSTEP_I		0x4ac07fff
#define BSTEP_L_I	0x4ac87fff
#define BSTEP_R_I	0x4ac47fff
#define BSTEP_L_R_I	0x4acc7fff
#define BSTEP_S_I	0x4ae07fff
#define BSTEP_S_L_I	0x4ae87fff
#define BSTEP_S_R_I	0x4ae47fff
#define BSTEP_S_L_R_I	0x4aec7fff
#endif
#else /* assembly */
/* cp2 control registers */
#define C2_SRCSKEW	$0	/* graphics source skew */
#define C2_SRCSHIFT	$1	/* graphics source shift */
#define C2_COLOR0	$2	/* COLOR0 data */
#define C2_COLOR1	$3	/* COLOR1 data */
#define C2_GCPCNTRL	$4	/* graphics control */
#define C2_RASTEROP	$5	/* RasterOp logical operation */
#define C2_PLANEMASK	$6	/* Plane mask */
#define C2_CONFIG	$9	/* video control */
#define C2_PSSTAT	$24	/* I/O port status */
#define C2_PSCOMM	$25	/* I/O port command */

/* cp2 data registers */
#define C2_SCRSTART	$0	/* Screen start address */
#define C2_SCRPITCH	$1	/* Screen pitch */
#define C2_HWCRSRSTART	$2	/* Hardware cursor start addr */
#define C2_HWCRSRCURR	$3	/* Current hardware cursor addr */
#define C2_SAMEXTENT	$4	/* VRAM serial-access-mode extent */
#define C2_NXTDISP	$5	/* Next display addr */
#define C2_CURDISP	$6	/* Current display address */
#define C2_LINECOUNT	$7	/* Video line count */
#define C2_VBLKSIZE	$8	/* Video block size */
#define C2_SRCLINE	$9	/* Src start of next line addr */
#define C2_SRCCURR	$10	/* Source current addr */
#define C2_SRCPITCH	$11	/* Source pitch */
#define C2_DESTLINE	$12	/* Dest start of next line addr */
#define C2_DESTCURR	$13	/* Destination current addr */
#define C2_DESTPITCH	$14	/* Destination pitch */
#define C2_GBLKSIZE	$15	/* Graphics src read block size */
#define C2_SERPULS	$16	/* Comp sync serration pulse width */
#define C2_HLINTR	$17	/* Horizontal interrupt line */
#define C2_BLANKS	$18	/* Blank start */
#define C2_SYNCS	$19	/* SYNC start */
#define C2_SYNCRESET	$20	/* SYNC reset */
#define C2_BLANKE	$21	/* Blank end */
#define C2_HWCRSR	$22	/* Hardware cursor location */
#define C2_VHWCONFIG	$23	/* Video hardware config bits */
#define C2_PSTXB	$24	/* PS/2 serial port Tx buffer */
#define C2_PSRCVB	$25	/* PS/2 serial port Rx buffer */
#define C2_SRCDATA	$28	/* Source data */
#define C2_DESTDATA	$29	/* Destination data */
#define C2_LEFTMASK	$30	/* Left edge mask */
#define C2_RIGHTMASK	$31	/* Right edge mask */

/* don't use .word, SGI workstations don't like them in text sections */
#define sstep		c2	0x00ffff
#define sbstep		c2	0x40ffff
#define wstep		c2	0x80ffff
#define wstep_l		c2	0x88ffff
#define wstep_r		c2	0x84ffff
#define wstep_l_r	c2	0x8cffff
#define wstep_s		c2	0xa0ffff
#define wstep_s_l	c2	0xa8ffff
#define wstep_s_r	c2	0xa4ffff
#define wstep_s_l_r	c2	0xacffff
#define wstep_sb	c2	0xb0ffff
#define wstep_sb_l	c2	0xb8ffff
#define wstep_sb_r	c2	0xb4ffff
#define wstep_sb_l_r	c2	0xbcffff
#define wstep_four	c2	0x82ffff
#define wstep_bfour	c2	0x81ffff
#define bstep_bfour	c2	0xc1ffff
#define bstep		c2	0xc0ffff
#define bstep_l		c2	0xc8ffff
#define bstep_r		c2	0xc4ffff
#define bstep_l_r	c2	0xccffff
#define bstep_s		c2	0xe0ffff
#define bstep_s_l	c2	0xe8ffff
#define bstep_s_r	c2	0xe4ffff
#define bstep_s_l_r	c2	0xecffff
#define bstep_sb	c2	0xf0ffff
#define bstep_sb_l	c2	0xf8ffff
#define bstep_sb_r	c2	0xf4ffff
#define bstep_sb_l_r	c2	0xfcffff
#ifdef LR33120
#define wstep_i		c2	0x807fff
#define wstep_l_i	c2	0x887fff
#define wstep_r_i	c2	0x847fff
#define wstep_l_r_i	c2	0x8c7fff
#define wstep_s_i	c2	0xa07fff
#define wstep_s_l_i	c2	0xa87fff
#define wstep_s_r_i	c2	0xa47fff
#define wstep_s_l_r_i	c2	0xac7fff
#define wstep_sb_i	c2	0xb07fff
#define wstep_sb_l_i	c2	0xb87fff
#define wstep_sb_r_i	c2	0xb47fff
#define wstep_sb_l_r_i	c2	0xbc7fff
#define wstep_four_i	c2	0x827fff
#define wstep_bfour_i	c2	0x817fff
#define bstep_bfour_i	c2	0xc17fff
#define bstep_i		c2	0xc07fff
#define bstep_l_i	c2	0xc87fff
#define bstep_r_i	c2	0xc47fff
#define bstep_l_r_i	c2	0xcc7fff
#define bstep_s_i	c2	0xe07fff
#define bstep_s_l_i	c2	0xe87fff
#define bstep_s_r_i	c2	0xe47fff
#define bstep_s_l_r_i	c2	0xec7fff
#endif
#endif

/* register bit assignments */
/* M_CFGREG */
#define CR_BANKMASK	(7<<5)
#define CR_BANKSHFT	5

/* C2_GCPCNTRL creg4 */
#define GCPC_SPCLWEMASK	0x00006000
#define GCPC_SPCLWESHIFT 13
#define GCPC_WECOLOR	(0<<13)
#define GCPC_WEMASK	(1<<13)
#define GCPC_WEPERSIST	(2<<13)
#define GCPC_WENORMAL	(3<<13)
#define GCPC_YDIR	(1<<12)
#define GCPC_XDIR	(1<<11)
#define GCPC_WO		(1<<9)
#define GCPC_MW		(1<<8)
#define GCPC_MASK	(1<<6)
#define GCPC_EXPND	(1<<4)
#define GCPC_TRAN	(1<<3)
#define GCPC_PIXSIZMASK	0x00000007
#define GCPC_PIXSIZSHIFT 0
#define DEFAULT_BBCP_CONTROL_VALUE +(0xfffffff8 | LOG2BPP)

/* C2_RASTEROP creg5 */
#define ROP_ZERO	0
#define ROP_AND		1
#define ROP_SRC		3
#define ROP_DEST	5
#define ROP_XOR		6
#define ROP_OR		7
#define ROP_ONE		15

/* X definitions for C2_RASTEROP */
#define	GXclear			0x0		/* 0 */
#define GXand			0x1		/* src AND dst */
#define GXandReverse		0x2		/* src AND NOT dst */
#define GXcopy			0x3		/* src */
#define GXandInverted		0x4		/* NOT src AND dst */
#define	GXnoop			0x5		/* dst */
#define GXxor			0x6		/* src XOR dst */
#define GXor			0x7		/* src OR dst */
#define GXnor			0x8		/* NOT src AND NOT dst */
#define GXequiv			0x9		/* NOT src XOR dst */
#define GXinvert		0xa		/* NOT dst */
#define GXorReverse		0xb		/* src OR NOT dst */
#define GXcopyInverted		0xc		/* NOT src */
#define GXorInverted		0xd		/* NOT src OR dst */
#define GXnand			0xe		/* NOT src OR NOT dst */
#define GXset			0xf		/* 1 */


/* C2_CONFIG creg9 */
#define CONFIG_BENDS		(1<<24)	/* big endian swizzle */
#define CONFIG_BW8		(1<<23)	/* 8 word blkwrite, INLVDBW reqd */
#define CONFIG_INLVDBW		(1<<22) /* interleaved block writes */
#define CONFIG_QSF		(1<<21)
#define CONFIG_HCUR		(1<<19)
#define CONFIG_DRAM		(1<<18)
#define CONFIG_SAM		(1<<17)
#define CONFIG_VRAM		(1<<16)
#define CONFIG_D3		(1<<15)
#define CONFIG_IORV3MASK 	(7<<12)
#define CONFIG_IOWAIT3MASK 	(0xf<<8)
#define CONFIG_D2		(1<<7)
#define CONFIG_IORV2MASK 	(7<<4)
#define CONFIG_IOWAIT2MASK 	(0xf<<0)

/* C2_PSSTAT creg24 */
#define PSSTAT_FERR	(1<<7)	/* framing error */
#define PSSTAT_PAR	(1<<6)	/* parity error */
#define PSSTAT_RXIN	(1<<5)	/* rx in progress */
#define PSSTAT_RXBF	(1<<4)	/* rx buffer full */
#define PSSTAT_TXBE	(1<<3)	/* tx buffer empty */
#define PSSTAT_TXIN	(1<<2)	/* tx in progress */
#define PSSTAT_CLKX	(1<<1)	/* clock inhibit */
#define PSSTAT_SI1	(1<<1) 	/* standard port1 (DATA) input */
#define PSSTAT_CLK	(1<<0)	/* clock signal */
#define PSSTAT_SI0	(1<<0) 	/* standard port0 (CLK) input */

/* C2_PSCOMM creg25 */
#define PSCOMM_IO	(1<<7)	/* standard i/o mode */
#define PSCOMM_CLKINH	(1<<4)	/* clock inhibit */
#define PSCOMM_RCVINT	(1<<3)	/* receive interrupt enable */
#define PSCOMM_TXINT	(1<<2)	/* transmit interrupt enable */
#define PSCOMM_TXEN	(1<<1)	/* tx enable */
#define PSCOMM_SO1	(1<<1) 	/* standard port1 (DATA) output */
#define PSCOMM_INTHTX	(1<<0)	/* inhibit clock after tx */
#define PSCOMM_SO0	(1<<0) 	/* standard port0 (CLK) output */

/* C2_VHWCONFIG dreg23 */
#define VHWCONFIG_EN		(1<<31)
#define VHWCONFIG_CROSSINV	(1<<18)
#define VHWCONFIG_CROSSDAT	(1<<17)
#define VHWCONFIG_CROSSEN	(1<<16)
#define VHWCONFIG_CSYNCEN	(1<<15)
#define VHWCONFIG_OVRSCN	(1<<14)
#define VHWCONFIG_VEND		(1<<13)
#define VHWCONFIG_VSYNCOUTEN	(1<<12)
#define VHWCONFIG_VSYNCINEN	(1<<11)
#define VHWCONFIG_HWCRSR	(1<<10)
#define VHWCONFIG_SHFTMASK	(3<<8)
#define VHWCONFIG_VSPOS		(1<<7)
#define VHWCONFIG_HSPOS		(1<<6)
#define VHWCONFIG_VSINT		(1<<5)
#define VHWCONFIG_HLINT		(1<<4)
#define VHWCONFIG_VLINTEN	(1<<1)
#define VHWCONFIG_HLINTEN	(1<<0)

#ifdef LR33120
/* C2_SRCSHIFT	creg1 */
#define SRCSH_INCMASK	(0x1f<<SRCSH_INCSHIFT)
#define SRCSH_INCSHIFT	5
#endif

#endif /* _LR33020_ */
