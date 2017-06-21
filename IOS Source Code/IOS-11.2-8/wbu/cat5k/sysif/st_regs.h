/* $Id: st_regs.h,v 1.1.14.1 1996/05/09 15:01:52 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/sysif/st_regs.h,v $
 *------------------------------------------------------------------
 * st_regs.h -- SAINT Registers definitions header file.
 *
 * February, 1995, Jason K. Kuo.
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: st_regs.h,v $
 * Revision 1.1.14.1  1996/05/09  15:01:52  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.24.1  1996/04/29  19:42:27  cakyol
 * missed these during the sync
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.12.1  1996/04/08  01:43:18  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.6.1  1996/03/22  09:13:58  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/20  11:41:59  rlowe
 * Add Synergy content to placeholder files.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/20  11:30:43  rlowe
 * Placeholders for Cat5k ATM line card -- more headers from Synergy
 * syn_common.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.15.2.2  1995/12/01  18:11:58  jkuo
 * Changed IPG_MII_lo from 0x15 to 0x09 to fix CSCdi44273
 * with which 10/100 card dropped packets at 10Mb/s half duplex.
 *
 * Revision 1.15.2.1  1995/11/18  02:00:52  jkuo
 * Defined MII_LED4_MODE_10BT_FD bit (1) on reg MII_PCS_CONFIG_REG (0x17)\for 10/100 card at port init time.
 *
 * Revision 1.15  1995/11/04  02:06:20  jkuo
 * Removed MII_SCAN def.
 *
 * Revision 1.14  1995/11/03  20:21:34  jkuo
 * 10/100 card SW integration.
 *
 * Revision 1.13  1995/11/01  03:24:24  jkuo
 * Added Speed select init defs for MII.
 *
 * Revision 1.12  1995/10/30  19:10:25  jkuo
 * Moved MII regs defs from common/st_init.h because they are
 * also needed by LCP main.c polling loop.
 *
 * Revision 1.11  1995/10/04  01:07:29  kchristi
 * Fix CVS directives so that cvs-fix-merge script will work.
 *
 * Revision 1.10  1995/09/29  01:45:55  jchiou
 * Defined three new Spanning Tree flags for SAINT2.
 *
 * Revision 1.9  1995/09/23  05:39:38  jkuo
 * Added MII_LINK_SCAN bit for enabling auto MII link scanning.
 *
 * Revision 1.8  1995/08/31  21:29:20  jkuo
 * Added SSA5-SSA3 (0x5D-0x5F) for Saint-2.  No effect on these 3 for Saint-1.
 *
 * Revision 1.7  1995/06/29  16:52:29  jkuo
 * Added more description for PB_DIVIDER.
 *
 * Revision 1.6  1995/06/08  02:33:27  jkuo
 * Added comments for EXTERNAL_LOOPBACK in MAC_CTRL2 (active low for 100BT).
 *
 * Revision 1.5  1995/06/02  22:48:32  jkuo
 * Put the final version of IPGs for 100TX and MII, thanks Tom.
 *
 * Revision 1.4  1995/06/02  04:22:54  jkuo
 * Added IPGs for MII, they are temporarily identical
 * to those of T100Xs.
 *
 * Revision 1.3  1995/05/17  05:32:13  jkuo
 * Changed IPG_100BT_lo to 9 for now until further test result.
 * Added HALF_DUPLEX_10BT (which is identical to DISABLE_LOOPBACK) for clarity.
 *
 * Revision 1.2  1995/04/13  02:16:26  jkuo
 * Added MAC_MODEs 00-03 for MAC_CTRL2 reg.
 *
 * Revision 1.1  1995/02/16  00:41:07  jlockhar
 * First check in for common use between NMP and all line cards.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*---------------------------------------------------------------------------
  st_regs.h

  SAINT Registers definitions header file.

  Jason K. Kuo	Feb/95

 ---------------------------------------------------------------------------*/

#ifndef	_ST_REGS_H_
#define	_ST_REGS_H_

/*----------------------------------------------------------------------*/
/*			Register offsets in SAINT			*/
/*----------------------------------------------------------------------*/

#define	PB_COMMAND		0x48	/* Packet Buffer Command Reg */
#define	 PB_CMD_INCR_ADDR	flag(0)
#define	 PB_CMD_AUTO_READ	flag(1)
#define	 PB_CMD_AUTO_WRITE	flag(2)

#define	PB_ADDR_HIGH		0x49	/* Packet Buffer */
#define	PB_ADDR_MID		0x4A
#define	PB_ADDR_LOW		0x4B
#define	PB_IMM_COMMAND		0x4C	/* Immediate command (see below) */
#define  PB_IMM_READ		flag(0)
#define  PB_IMM_WRITE		flag(1)
#define  PB_IMM_RESET_PTR	flag(2)

#define	PB_DATA_PTR		0x4D
#define	PB_DIVIDER		0x4E	/* Packet Buffer Rx/Tx divider */
					/* Upper 5 bits of the PB 17 addr bits */
#define	REVISION_REG		0x4F

#define	PB_PTR			0x51	/* Packet buffer pointer */
#define	 LD_P2SRD		flag(0)
#define	 LD_P2SDN		flag(1)
#define	 LD_M2PWRT		flag(2)
#define	 LD_M2PCMT		flag(3)
#define	 LD_P2MRD		flag(4)
#define	 LD_P2MDN		flag(5)
#define	 LD_S2PWRT		flag(6)
#define	 LD_S2PCMT		flag(7)


#define	TYPE_FIELD		0x52    /* values defined in syn_common/utypes.h */

#define	COLOR_HIGH		0x53
#define	COLOR_LOW		0x54

#ifndef SAGE
#define	SDA5			0x55
#define	SDA4			0x56
#define	SDA3			0x57
#define	SDA2			0x58
#define	SDA1			0x59
#define	SSA2			0x5A
#define	SSA1			0x5B
#define	SSA0			0x5C
#define	SSA5			0x5D	/* Saint2 or newer */
#define	SSA4			0x5E	/* Saint2 or newer */
#define	SSA3			0x5F	/* Saint2 or newer */
#endif

#define	SAINT_CTRL1		0x60
#define  SBTX_ENABLE		flag(0)
#define  SADA_MODE		flag(1)
#define  MON_MODE		flag(2)
#define  TYPE_MATCH		flag(3)	/* Type match enable */
#define  COLOR_MATCH		flag(4)	/* Color match enable */
#define  CUT_THRU		flag(5)	/* Cut-Thru enable */
#define  FLUSH_ALL		flag(6)
#define  SB2PB_ENABLE		flag(7)

#define	SAINT_CTRL2		0x61
#define  PB_SIZE		flag(0)	/* 1 = large (128K) */
#define  PB2SB_ENABLE		flag(1)
#define  MAC2PB_ENABLE		flag(2)
#define  BRQ_PRIORITY		flag(3)	/* SW Bus request priority */
#ifndef SAGE				/* if set, priority is 2   */
#define  ENCAP			flag(4)	/* for ISL or NMP */
#endif
#define  MDTIF_ENABLE		flag(5)
#define  SBIFR_ENABLE		flag(6)
#define  MDRIF_ENABLE		flag(7)

#define	SAINT_CTRL3		0x62
#ifndef  SAGE
#define  USE_MY_INDEX		flag(0)
#endif
#define  MATCH_ENABLE		flag(1)
#define  MATCH_ABORT		flag(2)	/* Abort on match/mismatch enable */
#define  SA_SEL			flag(3)
#define  MATCH_MISMATCH		flag(4)	/* Match/Mismatch select */
#define  MATCH_BB		flag(5)	/* Ignore bottom byte */
#define	 T_SWAP_ENABLE		flag(6)	/* in SAGE only */
#define  LEARN                  flag(7) /* SPanning Tree: SAINT2 & above  */
#define	  S_CTRL3_INIT		 0x10

#define	SAINT_CTRL4		0x63
#define  HASH_SEL		flag(0)
#define  HASH_IGNORE_COLOR	flag(1)
#define  MAX_RETRY_LSB		flag(2)
#define  MAX_RETRY_MSB		flag(5)
#define  BPDU_BLOCK             flag(6) /* SPanning Tree: SAINT 2 & above */
#define  CDP_BLOCK              flag(7) /* SPanning Tree: SAINT 2 & above */
#define	  S_CTRL4_INIT		 0x3C

#define	MATCH_ADDR5		0x64
#define	MATCH_ADDR4		0x65
#define	MATCH_ADDR3		0x66
#define	MATCH_ADDR2		0x67
#define	MATCH_ADDR1		0x68
#define	MATCH_ADDR0		0x69
#define HIGH_WATER 		0x6A	/* upper 8 bits of PB_ADDR (17) */
					/* Threshold for hi bus priority (3) */
#ifndef SAGE
#define	MAC_CTRL1		0x6B
#define	 RETRY_LATE		flag(0)
#define	 CRC_APPEND		flag(1)
#define	 PAD_ENABLE		flag(2)
#define	 NO_PREAMBLE		flag(3)
#define  COLL_POLARITY		flag(4)
#define  CRS_POLARITY		flag(5)
#define  JAB_POLARITY		flag(6)
#define  RXC_POLARITY		flag(7)

#define	MAC_CTRL2		0x6C
#define  MAC_MODE_LSB		flag(0)
#define  MAC_MODE_MSB		flag(1)
#define	  MAC_MODE_10BT			0x00
#define	  MAC_MODE_T100X		0x01
#define	  MAC_MODE_MII			0x02
#define	  MAC_MODE_RESERVED		0x03
#define  LOOPBACK_100X		flag(2)
#define  HUGE_ENABLE		flag(3)
#define  FULL_DUPLEX		flag(4)
#define  DISABLE_LOOPBACK	flag(5)	/* active low, bit unused for 100TX */
#define  HALF_DUPLEX_10BT	flag(5)	/* enable collision detect for 10BT */
#define  EXTERNAL_LOOPBACK	flag(6) /* active low for 100BT */
#define	  M_CTRL2_INIT		 0x10

#define	MAC_CTRL3		0x6D
#define  DISCARD_UNDER_LEN	flag(0)
#define  DISCARD_DRIBBLE	flag(1)
#define  DISCARD_BAD_RCV_CODE	flag(2)
#define  DISCARD_BAD_CRC	flag(3)
#endif

#define	SAINT_RESET		0x6E
#define	 RESET_MAC_TX_L		flag(0)
#define	 RESET_MAC_RX_L		flag(1)
#define	 RESET_PB_L		flag(2)
#define	 RESET_TX_L		flag(3)
#define	 RESET_RX_L		flag(4)
#define	 RESET_MAC_L		flag(5)

#define TEST_CTRL		0x6F
#define  TEST_TEST_MODE		flag(0)
#define  TEST_MUX0		flag(1)
#define  TEST_MUX1		flag(2)
#define  TEST_MUX2		flag(3)
#define  TEST_MUX3		flag(4)

#ifndef SAGE
#define INDEX_HIGH		0x70	/* index reg bit [16] */
#define INDEX_MID		0x71	/* index reg bits [15:8] */
#define INDEX_LOW		0x72	/* index reg bits [7:0] */

#define	CTRL_BYTE		0x73
#define  C_BPDU_PACKET		flag(0)
#define  C_DONT_LEARN		flag(1)
#define  C_INDEX_ROUTE		flag(2)
#define  C_IGNORE_COLOR_TYPE	flag(3)
#define  C_MONITOR		flag(4)
#define  C_DONT_FORWARD		flag(5)
#define  C_DISBL_INDEX_LEARN	flag(6)
#define  C_DISBL_NEW_LEARN	flag(7)
#endif

#define	PB_DATA0		0x74	/* holding reg bits [7:0] */
#define	PB_DATA1		0x75	/* holding reg bits [15:8] */
#define	PB_DATA2		0x76	/* holding reg bits [23:16] */

#define	S2PCMT_hi		0x77
#define	S2PCMT_mid		0x78
#define	S2PCMT_lo		0x79
#define	S2PWRT_hi		0x7A
#define	S2PWRT_mid		0x7B
#define	S2PWRT_lo		0x7C

#define	P2MDN_hi		0x7D
#define	P2MDN_mid		0x7E
#define	P2MDN_lo		0x7F
#define	P2MRD_hi		0x80
#define	P2MRD_mid		0x81
#define	P2MRD_lo		0x82

#define	M2PCMT_hi		0x83
#define	M2PCMT_mid		0x84
#define	M2PCMT_lo		0x85

#define	M2PWRT_hi		0x86
#define	M2PWRT_mid		0x87
#define	M2PWRT_lo		0x88

#define	P2SDN_hi		0x89
#define	P2SDN_mid		0x8A
#define	P2SDN_lo		0x8B
#define	P2SRD_hi		0x8C
#define	P2SRD_mid		0x8D
#define	P2SRD_lo		0x8E

#define	INTRPT_MASK		0x8F	/* bits[0:4] below */

#define	INTRPT_REG     		0x90
#define	 LEN_ERR_RX		flag(0)	/* Length error on Rx */
#define	 LEN_ERR_TX		flag(1)	/* Length error on Tx */
#define	 TX_OVERFLOW		flag(2)
#define	 SBIFR_UNDERFLOW	flag(3)
#define	 LINK_FAIL_PIPE		flag(4)	/* Link (Cable) disconnect */
#define	 PB_REQ_PIPE		flag(5)	/* Status only: Saint PB Busy */

#define	PB2MAC_LEN_hi		0x91
#define	PB2MAC_LEN_lo		0x92
#define	PB2SB_LEN_hi		0x93
#define	PB2SB_LEN_lo		0x94

#define	MDTIF_HOLD5		0x98
#define	MDTIF_HOLD4		0x99
#define	MDTIF_HOLD3		0x9A
#define	MDTIF_HOLD2		0x9B
#define	MDTIF_HOLD1		0x9C
#define	MDTIF_HOLD0		0x9D

#ifndef SAGE
#define	MAC_IPG_lo		0x9E	/* bits [7:0] */
#define	MAC_IPG_mid		0x9F	/* bits [15:8] */
#define	MAC_IPG_hi		0xA0	/* bits [20:16] */
					/*   MAC Inter-Packet Gap (IPG) */
#define	 IPG_10BT_lo		0x55			/* For 10 Base T */
#define	 IPG_10BT_hi		0x59
#define	 IPG_10BT_mid		((IPG_10BT_lo + IPG_10BT_hi) / 3)

#define	 IPG_100BT_lo		0x15			/* For 100TX */
#define	 IPG_100BT_mid		0x0A
#define	 IPG_100BT_hi		0x11

#define	 IPG_MII_lo		0x09		/* For MII */
#define	 IPG_MII_mid		0x0C
#define	 IPG_MII_hi		0x14

#define	MAC_ADDR		0xC0	/* Mac Address C0-FF */

#define	MAC_MASKED_RANDOM	0xC4	/* Masked random number */
#define	MAC_RANDOM_GEN		0xC5	/* Rrandom number generator */
#define	MAC_TX_BYTE_CNT		0xC6	/* Tx byte counter */
#define	MAC_PNCT_lo		0xC7	/* lower nibble counter pnct[15:0] */

#define	MAC_PNCT_hi		0xCE	/* upper nibble counter pnct[17:16] */

#define	MAC_RX_BYTE_CNT		0xD6	/* Rx byte counter */

#define	MAC_EVCT_lo		0xD7	/* lower event vector evct[15:0] */

#define	MAC_EVCT_hi		0xDE	/* upper event vector evct[17:16] */

#define	MAC_SX_LS1CTL		0xE8
#define	 MAC_SX_LS1CTL_FRCQ	flag(0)
#define	 MAC_SX_LS1CTL_NORCFR	flag(12)
#define	 MAC_SX_LS1CTL_NOTCFR	flag(13)

#define	MAC_LFCT		0xE9	 /* Link fail counter lfct[13:0] */

#define	MAC_MII_CTRL		0xF0	 /* MII control register */
#define	 MAC_MII_EGMDC		flag(7)	 /* speed up mdc_o */

#define	MAC_MII_INT_STATES	0xF1
#define	 MII_BUSY		flag(3)

#define	MAC_MII_READ_STAT_REG	0xF2
#define	MAC_MII_SEND_SYNCH	0xF3
#define	MAC_MII_READ_STAT_REQ	0xF4	 /* Read status request */
#define	MAC_MII_WRITE_DATA	0xF5	 /* MII write data reg & request */
#define	MAC_MII_ADDR		0xF6	 /* Address register */
#define  MII_LINK_SCAN	        flag(7)	 /* start auto HW link scanning */
					 /* For MII_BASIC_MODE_STAT_REG	*/


/*----------------------------------------------------------------
 * MII PHY Serial Handshake sequence:
 *
 *   Write Op:	<16-bit PHY_ADDR+REG_No> <16-bit REG_VALUE>
 *   Read  Op:  <16-bit PHY_ADDR+REG_No> <MAC_MII_READ_STAT_REQ>
 *
 * Notes:
 *   Each MII can have up to 32 PHY devices (5 bits).
 *   PHY_ADDR = 0x00 (Hard-wired for MCP & 10/100 cards)
 *   REG_No   = bits 4:0 (Bit 7 = Scan bit)
 *   PB_DATA1 = high byte for I/O holding register
 *						-jason (09/18/95)
 *----------------------------------------------------------------
 */

#define	MII_BASIC_MODE_CTRL_REG	0x00
#define	 MII_BMODE_RESET	 0x8000 /* Reset bit		*/
#define  MII_BMODE_FULL_DUPLEX	 0x0100 /* Full-duplex Auto-off */
#define	 MII_BMODE_SELECT_10	 0x0000 /* Select 100M mode	*/
#define	 MII_BMODE_SELECT_100	 0x2000 /* Select 100M mode	*/
#define	 MII_BMODE_SELECT_AUTO	 0x1000 /* Select Auto-Nego mode*/
#if 1
#define	 MII_BMODE_LOOPBACK	 0x6100 /* Loopback in 100M mode*/
#else
#define	 MII_BMODE_LOOPBACK	 0x5000 /* Loopback MDIO test   */
#endif

#if defined(MCP)
#define	 MII_PHY_ADDR		0x00
#define	 MII_BMODE_INIT		0x2100  /* 100Mb/s, Full Duplex */
#else  /* LCP 10/100 */
#define	 MII_PHY_ADDR		0x1F
#define	 MII_BMODE_INIT		0x1000  /* Auto-Negotiation on  */
#endif


#define	MII_BASIC_MODE_STAT_REG	0x01
#define  MII_LINK_UP		 flag(2)

#define	MII_PCS_CONFIG_REG	0x17
#define	 MII_LED4_MODE_10BT_FD	 flag(1) /* LED4 output (pin#37) to
					    indicate Full Duplex in
					    10-BT mode */

#define MII_10BT_CONFIG_REG	0x1C
#define	 MII_10BT_INIT		 0xE3A9


#define MII_PHY_ADDR_REG	0x19
#define	 MII_SPEED_10		 flag(6) /* 10 Mb link running  */


#endif /* for SAGE */

#endif

