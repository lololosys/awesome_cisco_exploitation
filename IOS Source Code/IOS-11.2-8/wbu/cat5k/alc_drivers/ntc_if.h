/* $Id: ntc_if.h,v 1.1.4.1 1996/05/09 14:55:06 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/ntc_if.h,v $
 *------------------------------------------------------------------
 * ntc_if.h -- NTC driver interface file.
 *
 * This file describes the interface to the Network Termination
 * Controller (NTC).  It describes the NTC registers and the values
 * they can be programmed with, dma channels and statistics records.
 *
 * February 1995, rahuja
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ntc_if.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:06  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:33  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:21  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.2  1996/04/02  05:08:08  rlowe
 * Fix up include file paths.  Cleanup main ALC makefile regarding
 * sar images and tools.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.16.1  1996/03/22  09:10:40  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:33  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:02  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *------------------------------------------------------------------
 * ntc_if.h -- NTC driver interface file.
 *
 *------------------------------------------------------------------
 *
 *  FILE:  ntc_if.h
 *
 *  CONTENTS:
 *
 *  This file describes the interface to the Network Termination Controller
 *	(NTC).  It describes the NTC registers and the values they can be
 *	programmed with, dma channels and statistics records.
 *
 *  OWNER:
 *
 *      Ratinder P. S. Ahuja
 *
*/
 
#ifndef _NTC_IF_H_
#define _NTC_IF_H_

#define PAK __attribute__ ((packed))



/*
**
**	INCLUDES
**
*/

#include	"../alc_cominc/basic_types.h"
#include	"../alc_include/acp_hw_defs.h"
#include	"../alc_include/ntc_client.h"



/*
**
**	NTC REGISTERS
**
*/

#define	NTC_CBASE	ACP_HW_NTC_BASE	/* base address of NTC control register	*/
#define	NTC_SBASE	ACP_HW_NTC_BASE	/* base address of NTC status register	*/

#define	mWRITE_NTC(reg, val)	*((UINT16 *)(reg)) = (UINT16)(val)
#define	mREAD_NTC(reg, val)		(val) = (UINT32)(*((UINT16 *)(reg)))

/*
**
**	C O N T R O L   R E G I S T E R S
**
**	NOTE: These definations are derived from the document:
**		Enhancement Summary NTC MB86683/MB86683A
**	Dated: Edition 1.0, May 1994
**
*/

/* This register is used in REV B NTC ONLY 									*/
#define NTC_FRAMER_ENABLE_CONTROL	(NTC_CBASE)	/* Framer TX and RX Enable	*/


#define	NTC_FRAMER_CONTROL		(NTC_CBASE+2)	/* general framer control	*/

#define	NTCTX_FRAMER_BYTES1		(NTC_CBASE+4)	/* overhead access bytesblk1*/
#define	NTCTX_FRAMER_BYTES2		(NTC_CBASE+6)	/* overhead access bytesblk2*/
#define	NTCTX_FRAMER_BYTES3		(NTC_CBASE+8)	/* overhead access bytesblk3*/
#define	NTCTX_FRAMER_BYTES4		(NTC_CBASE+10)	/* overhead access bytesblk4*/
#define	NTCTX_FRAMER_BYTES5		(NTC_CBASE+12)	/* overhead access bytesblk5*/
#define	NTCTX_FRAMER_BYTES6		(NTC_CBASE+14)	/* overhead access bytesblk6*/
#define	NTCTX_FRAMER_BYTES7		(NTC_CBASE+16)	/* overhead access bytesblk7*/
#define	NTCTX_FRAMER_BYTES8		(NTC_CBASE+18)	/* overhead access bytesblk8*/
#define	NTCTX_FRAMER_BYTES9		(NTC_CBASE+20)	/* overhead access bytesblk9*/
#define	NTCTX_FRAMER_BYTES10	(NTC_CBASE+22)	/* overhead access bytesblk10*/
#define	NTCTX_FRAMER_BYTES11	(NTC_CBASE+24)	/* overhead access bytesblk11*/
#define	NTCTX_FRAMER_BYTES12	(NTC_CBASE+26)	/* overhead access bytesblk12*/
#define	NTCTX_FRAMER_BYTES13	(NTC_CBASE+28)	/* overhead access bytesblk13*/
#define	NTCTX_FRAMER_BYTES14	(NTC_CBASE+30)	/* overhead access bytesblk14*/
#define	NTCTX_FRAMER_BYTES15	(NTC_CBASE+32)	/* overhead access bytesblk15*/
#define	NTCTX_FRAMER_BYTES16	(NTC_CBASE+34)	/* overhead access bytesblk16*/

#define	NTCRECV_CONTROL			(NTC_CBASE+36)	/* cell recv general control*/
#define	NTCRECV_HEC_CONTROL		(NTC_CBASE+38)	/* hec / descrambler control*/
#define	NTCRECV_XMASK1			(NTC_CBASE+40)	/* extract / discard mask	*/
#define	NTCRECV_XMASK2			(NTC_CBASE+42)	/* extract / discard mask	*/
#define	NTCRECV_SMASK1			(NTC_CBASE+44)	/* extract / discard mask	*/
#define	NTCRECV_SMASK2			(NTC_CBASE+46)	/* extract / discard mask	*/
#define	NTCRECV_EXTRACT_CONTROL	(NTC_CBASE+48)	/* extract control buffer	*/	
#define NTCRECV_DISCARD_CONTROL	(NTC_CBASE+50)	/* discard control		 	*/

#define	NTCXMIT_CONTROL			(NTC_CBASE+52)	/* cell xmit general control*/
#define	NTCXMIT_HEC_CONTROL		(NTC_CBASE+54)	/* cell xmit hec/scrambler 	*/
#define	NTCXMIT_XMASK1			(NTC_CBASE+56)	/* extract / discard mask 	*/
#define	NTCXMIT_XMASK2			(NTC_CBASE+58)	/* extract / discard mask 	*/
#define	NTCXMIT_SMASK1			(NTC_CBASE+60)	/* extract / discard mask 	*/
#define	NTCXMIT_SMASK2			(NTC_CBASE+62)	/* extract / discard mask 	*/
#define	NTCXMIT_EXTRACT_CONTROL	(NTC_CBASE+64)	/* extract control buffer	*/
#define	NTCXMIT_DISCARD_CONTROL	(NTC_CBASE+66)	/* discard control 			*/

#define	NTC_OAM_ALARM_CONTROL	(NTC_CBASE+68)	/* OAM framed alarm control	*/
							
						/* 70 - 98 not used */

#define	NTCSWITCH_STATS_CONTROL	(NTC_CBASE+100)	/* switch statistics control*/
#define	NTCNET_STATS_CONTROL	(NTC_CBASE+102)	/* network stats control	*/
#define	NTCNET_STATUS_XMASK1	(NTC_CBASE+104)	/* user cell xmask 1		*/
#define	NTCNET_STATUS_XMASK2	(NTC_CBASE+106)	/* user cell xmask 2		*/
#define	NTCNET_STATUS_SMASK1	(NTC_CBASE+108)	/* user cell smask 1		*/
#define	NTCNET_STATUS_SMASK2	(NTC_CBASE+110)	/* user cell smask 2		*/

#define	NTC_CELLSTREM_CONTROL1	(NTC_CBASE+112)	/* cell stream interface	*/
#define	NTC_CELLSTREM_CONTROL2	(NTC_CBASE+114)	/* cell stream interface	*/

#define	NTC_INTR_ENABLE			(NTC_CBASE+116)	/* interrupt enable			*/
#define	NTC_INTR_UNDERSERVICE	(NTC_CBASE+118)	/* interrupt under service	*/

#define	NTC_DMADESC_PTRTABLE_LO	(NTC_CBASE+120)	/* dma descriptor pointer 	*/
												/* table low				*/
#define	NTC_DMADESC_PTRTABLE_HI	(NTC_CBASE+122)	/* dma descriptor pointer 	*/
												/* table high				*/
#define	NTC_DMA_ACTIVITY		(NTC_CBASE+124)	/* dma channel activity		*/
#define	NTC_DMA_MODE			(NTC_CBASE+126)	/* dma mode					*/



/*
**
**	CONTROL REGISTER VALUES
**
*/

/*
**  Control register 0:  Framer Enable	REGISTER used by REV B NTC ONLY
*/

#define NTC_TX_FRAMER_ENABLE		0x0002	/* REV B NTC ONLY				*/
#define NTC_RX_FRAMER_ENABLE	 	0x0001	/* REV B NTC ONLY				*/

/*
**	Control register 1:  General framer control
*/

#define	FRAME_ENABLE_MASK		0x0001		/* bit 0						*/

#define	FRAME_ENABLE			0x0001		/* Rx/Tx framer enabled  REV A	*/
#define	FRAME_DISABLE			0x0000		/* Rx/Tx framer disabled REV A	*/

#define	XMODE_MASK				0x000E		/* bits 1 - 3					*/
#define	XMODE_CELL				0x0000		/* cell based					*/
#define	XMODE_SDH_STM1			0x0002		/* sdh STM -1					*/
#define	XMODE_SONET_STS3C		0x0004		/* Sonet STS-3c					*/
#define	XMODE_DS3_PLCP			0x0006		/* DS3 with PLCP				*/
#define	XMODE_E3				0x0008		/* E3							*/
#define XMODE_SONET_STS1		0x000A		/* Sonet STS-1					*/
#define	XMODE_DS3				0x000C		/* DS3 non PLCP					*/

#define	TESTMODE_MASK			0x0010		/* bit 4						*/
#define	TESTMODE_ENABLED		0x0010		/* Internal testmode			*/
#define	TESTMODE_DISABLED		0x0000		

#define	STMSCRAMBLER_MASK		0x0020		/* bit 5						*/
#define	STMSCRABLER_ENABLE		0x0020		/* enable 1+x6+x7 scrambler		*/
#define	STMSCRABLER_DISABLE		0x0000

#define	STMDESCRAMBLER_MASK		0x0040		/* bit 6						*/
#define	STMDESCRABLER_ENABLE	0x0040		/* enable 1+x6+x7 de-scrambler	*/
#define	STMDESCRABLER_DISABLE	0x0000

#define	FRAME_SYNCFOUND_MASK	0x0F00		/* bits 8 - 11					*/
#define	FRAME_SYNCLOST_MASK		0xF000		/* bits 12 - 15					*/
#define	FRAME_SYNCFOUND			(6 << 8)
#define	FRAME_SYNCLOST			(7 << 12)

#define	FRAMER_LOOPBACK			0x0080		/* bit 7						*/				

/*
**	Control register 2 - Transmit framer overhead access bytes block -1
**	Over head byte insert register For:
**	SDH / SONET D15 - D8 Collum 1,  D7 - D0 Row 1
*/

#define	SDH_SONET_C1_MASK	0xFF00
#define	SDH_SONET_R1_MASK	0x00FF
#define	SDH_SONET_C1_SHIFT	8

#define	STS1_ID_C1	0x00000001
#define	STS1_ID_R1	0x00000002

/*
**	Control register 3 - Transmit framer overhead access bytes block -2
**	Over head byte insert register For:
**	SDH / SONET D15 - D8 Row 2,  D7 - D0 E 1
*/

#define	SDH_SONET_R2_MASK	0xFF00
#define	SDH_SONET_E1_MASK	0x00FF
#define	SDH_SONET_R2_SHIFT	8

#define	STS1_ID_R2	0x00000003

/*
**	Control register 4 - Transmit framer overhead access bytes block -3
**	Over head byte insert register For:
**	SDH / SONET D15 - D8 F 1,  D7 - D0 Row 3
*/

#define	SDH_SONET_F1_MASK	0xFF00
#define	SDH_SONET_R3_MASK	0x00FF
#define	SDH_SONET_F1_SHIFT	8


/*
**	Control register 5 - Transmit framer overhead access bytes block -4
**	Over head byte insert register For:
**	SDH / SONET D15 - D8 Row4,  D7 - D0 D1
*/

#define	SDH_SONET_R4_MASK	0xFF00
#define	SDH_SONET_R4_SHIFT	8
#define	SDH_SONET_D1_MASK	0x00FF


/*
**  Control register 6 - Transmit framer overhead access bytes block -5
**  Over head byte insert register For:
**  SDH / SONET D15 - D8  D2,  D7 - D0 D3
*/
 
#define SDH_SONET_D2_MASK   0xFF00
#define SDH_SONET_D2_SHIFT  8
#define SDH_SONET_D_MASK	0x00FF


/*
**  Control register 7 - Transmit framer overhead access bytes block -6
**  Over head byte insert register For:
**  SDH / SONET D15 - D8  K1,  D7 - D0 D4
*/
 
#define SDH_SONET_K1_MASK   0xFF00
#define SDH_SONET_K1_SHIFT  8
#define SDH_SONET_D4_MASK   0x00FF
 

/*
**  Control register 8 - Transmit framer overhead access bytes block -7
**  Over head byte insert register For:
**  SDH / SONET D15 - D8  D5,  D7 - D0 D6
*/
 
#define SDH_SONET_D5_MASK   0xFF00
#define SDH_SONET_D5_SHIFT  8
#define SDH_SONET_D6_MASK   0x00FF

/*
**  Control register 9 - Transmit framer overhead access bytes block -8
**  Over head byte insert register For:
**  SDH / SONET D15 - D8  D7,  D7 - D0 D8
*/
 
#define SDH_SONET_D7_MASK   0xFF00
#define SDH_SONET_D7_SHIFT  8
#define SDH_SONET_D8_MASK   0x00FF


/*
**  Control register 10 - Transmit framer overhead access bytes block -9
**  Over head byte insert register For:
**  SDH / SONET D15 - D8  D9,  D7 - D0 D10
*/
 
#define SDH_SONET_D9_MASK   0xFF00
#define SDH_SONET_D9_SHIFT  8
#define SDH_SONET_D10_MASK  0x00FF

/*
**  Control register 11 - Transmit framer overhead access bytes block -10
**  Over head byte insert register For:
**  SDH / SONET D15 - D8  D11,  D7 - D0 D12
*/
 
#define SDH_SONET_D11_MASK   0xFF00
#define SDH_SONET_D11_SHIFT  8
#define SDH_SONET_D12_MASK   0x00FF


/*
**  Control register 12 - Transmit framer overhead access bytes block -11
**  Over head byte insert register For:
**  SDH / SONET D15 - D8  Z1A,  D7 - D0 Z1B
*/
 
#define SDH_SONET_Z1A_MASK   0xFF00
#define SDH_SONET_Z1A_SHIFT  8
#define SDH_SONET_Z1B_MASK   0x00FF


/*
**  Control register 13 - Transmit framer overhead access bytes block -12
**  Over head byte insert register For:
**  SDH / SONET D15 - D8  Z1C,  D7 - D0 E2
*/
 
#define SDH_SONET_Z1C_MASK   0xFF00
#define SDH_SONET_Z1C_SHIFT  8
#define SDH_SONET_E2_MASK    0x00FF


/*
**  Control register 14 - Transmit framer overhead access bytes block -13
**  Over head byte insert register For:
**  SDH / SONET D15 - D8  R5,  D7 - D0 R6
*/
 
#define SDH_SONET_R5_MASK    0xFF00
#define SDH_SONET_R5_SHIFT   8
#define SDH_SONET_R6_MASK    0x00FF

/*
**  Control register 15 - Transmit framer overhead access bytes block -14
**  Over head byte insert register For:
**  SDH / SONET D15 - D8  J1,  D7 - D0 C2
*/
 
#define SDH_SONET_J1_MASK    0xFF00
#define SDH_SONET_J1_SHIFT   8
#define SDH_SONET_C2_MASK    0x00FF

#define	C2VAL	0x13		/* Path signal level inidicator		*/

/*
**  Control register 16 - Transmit framer overhead access bytes block -15
**  Over head byte insert register For:
**  SDH / SONET D15 - D8  F2,  D7 - D0 Z3
*/
 
#define SDH_SONET_F2_MASK    0xFF00
#define SDH_SONET_F2_SHIFT   8
#define SDH_SONET_Z3_MASK    0x00FF


/*
**  Control register 17 - Transmit framer overhead access bytes block -16
**  Over head byte insert register For:
**  SDH / SONET D15 - D8  Z4,  D7 - D0 Z5
*/
 
#define SDH_SONET_Z4_MASK    0xFF00
#define SDH_SONET_Z4_SHIFT   8
#define SDH_SONET_Z5_MASK    0x00FF

/*
**	Control register 18 - Cell receiver general control
*/

#define	CELL_RECV_ENABLE		0x0001		/* cell receiver enabled		*/
#define	CELL_RECV_DISABLE		0x0000		/* cell receiver disabled		*/
#define	RECVOAM_CRC10CHECK_ENABLED	0x0002	/* crc10 check enabled			*/
#define	RECVOAM_CRC10CHECK_DISABLED	0x0000	/* crc10 check disabled			*/
#define	RECVOAM_CRC10GENERATE	0x0004		/* crc10 check generate			*/
#define	RECV_INSERT_PRI_BUFFER	0x0008		/* priority for insert buffer 	*/
#define	RECV_INSERT_PRI_USER	0x0000		/* user cells have priority		*/
/* NOTE:  not defining peak cell insertion rates */
#define	CELL_RECV_IGNORE_GFC	0x0000		/* ignore gfc, 12 bits for vpi	*/
#define	CELL_RECV_GFC			0x0100		/* gfc used, 8 bit for vpi		*/
#define	CELL_RECV_TESTMODE		0x8000		/* interneal test mode, set to 0*/


/*
**	Control register 19 Cell receiver HEC/descrambler control
*/

#define	RECV_DESCRAM_ENABLE		0x0001		/* enable 1+x43 descrambler		*/
#define	RECV_DESCRAM_DISABLE	0x0000		/* disable descrambler			*/
#define	RECV_HECMASK_ENABLE		0x0004		/* enable hec recv mask :		*/
											/* ( 01010101 )					*/
#define	RECV_HECMASK_DISABLE	0x0000		/* enable hec recv mask disbale	*/
#define	ERROR_CORRECT_ENABLE	0x0008		/* enable error correction		*/
#define	ERROR_CORRECT_DISABLE	0x0000	
#define	BYTE_ALLIGN				0x0010		/* byte aligned data			*/
#define	BYTE_NONALIGN			0x0000		/* non-byte aligned data		*/
#define	CELL_SYNCFOUND_MASK		0x0F00		/* cell sync found count		*/
#define	CELL_SYNCLOST_MASK		0xF000		/* cell sync lost count			*/
#define CELL_SYNCFOUND			(6<<8)
#define	CELL_SYNCLOST			(7<<12)


/*
**	Control register 20 - Cell receiver user defined extract/discard xmask 1
**	Control register 21 - Cell receiver user defined extract/discard xmask 2
**	Control register 22 - Cell receiver user defined extract/discard smask 1
**	Control register 23 - Cell receiver user defined extract/discard smask 2
*/
#define CTRL_XMASK_DISCARD_UNASS_AND_IDLE1		0xf000
#define CTRL_XMASK_DISCARD_UNASS_AND_IDLE2		0x000f
#define CTRL_SMASK_DISCARD_UNASS_AND_IDLE1		0x0000
#define CTRL_SMASK_DISCARD_UNASS_AND_IDLE2		0x0000


/*
**	Control register 24 - Cell receiver extract buffer control
*/

#define	RECV_EXTRACT_DISABLE	0x0000


/*
**	Control register 25 - Cell receiver discard control
*/

#define	RECV_DISACRD_DISABLE	0x0000
#define RECV_DISCARD_MASK_DEF   0x0001
#define RECV_UNASSIGN_DISCARD	0x0040


/*
**	Control register 26 - Cell transmitter general control
*/


#define	CELLTX_ENABLE			0x0001		/* cell transmitter enabled	*/
#define	CELLTX_DISABLE			0x0000			
#define	TXOAM_CRC10CHECK_ENABLED	0x0002	/* OAM cell crc check enable*/
#define	TXOAM_CRC10CHECK_DISABLED	0x0000	/* OAM cell crc check enable*/
#define	TXOAM_CRC10CHECK_GENERATE	0x0004	/* OAM cell crc check enable*/
#define TX_INSERT_PRI_BUFFER  	0x0008      /* priority for insert buffer   */
#define TX_INSERT_PRI_USER    	0x0000      /* user cells have priority     */
/* NOTE:  not defining peak cell insertion rates */
#define CELL_TX_IGNORE_GFC    	0x0000      /* ignore gfc, 12 bits for vpi  */
#define CELL_TX_GFC           	0x0100      /* gfc used, 8 bit for vpi      */
#define	CELL_TX_IDLECELL_GEN	0x0000		/* idle cells generated			*/
#define	CELL_TX_UASSIGN_GEN		0x0200		/* unassigned cell generation	*/
#define CELL_TX_TESTMODE      	0x8000      /* interneal test mode, set to 0*/


/*
**	Control register 27 - Cell transmitter HEC/Scrambler control
*/

#define	TX_SCRAM_ENABLE			0x0001		/* enable 1+x43 descrambler		*/
#define	TX_SCRAM_DISABLE		0x0000		/* disable descrambler			*/
#define	TX_HECMASK_ENABLE		0x0004		/* enable hec recv mask :		*/
											/* ( 01010101 )					*/
#define	TX_HECMASK_DISABLE		0x0000		/* hec mask disbale				*/


/*
**  Control register 28 - Cell transmitter user defined extract/discard xmask 1
**  Control register 29 - Cell transmitter user defined extract/discard xmask 2
**  Control register 30 - Cell transmitter user defined extract/discard smask 1
**  Control register 31 - Cell transmitter user defined extract/discard smask 2
**   Not used
*/


/*
**  Control register 32 - Cell transmitter extract buffer control
*/
 
#define TX_EXTRACT_DISABLE    0x0000
 
 
/*
**  Control register 25 - Cell transmitter discard control
*/
 
#define TX_DISACRD_DISABLE    0x0000


/*
**  Control register 34 - OAM alarm control
*/

#define	OAM_ENABLE				0x0001		/* OAM controller enable		*/
#define	OAM_DISABLE				0x0000		/* OAM controller enable		*/
#define	NORMAL_BIP				0x0000
#define	INVERT_BIP1				0x0020		/* inverted BIP					*/
#define	INVERT_BIP2				0x0040		/* inverted BIP					*/
#define	INVERT_BIP3				0x0080		/* inverted BIP					*/
#define	TX_PATH_AIS_SET			0x1000		/* transmit path AIS set		*/
#define	TX_PATH_AIS_CLEAR		0x0000		/* transmit path AIS clear		*/
#define	TX_SECTION_AIS_SET		0x2000		/* transmit section AIS set		*/
#define	TX_SECTION_AIS_CLEAR	0x0000		/* transmit section AIS clear	*/
#define	TX_PATH_FERF_CLEAR		0x0000
#define	TX_PATH_FERF_AUTO		0x0100
#define	TX_PATH_FERF_SET		0x0200
#define	TX_SECTION_FERF_CLEAR	0x0000
#define	TX_SECTION_FERF_AUTO	0x0400
#define	TX_SECTION_FERF_SET		0x0800
#define	TX_PATH_FEBE_DISABLE	0x0000
#define	TX_PATH_FEBE_ENABLED	0x4000
#define	TX_SECTION_FEBE_DISABLE	0x0000
#define	TX_SECTION_FEBE_ENABLE	0x8000


/*
**  Control registers 35 - 49 not used
*/



/*
**  Control registers 50 - Switch statistics handler control
*/

#define	SWITCH_STAT_ENABLE		0x0001
#define	SWITCH_STAT_DISABLE		0x0000
#define	SWITCH_STAT_TESTENABLE	0x0001
#define	SWITCH_STAT_TESTDISBALE	0x0000


/*
**  Control registers 51 - Network statistics record general control
*/

#define	NETSTAT_DISABLED		0x0000		
#define	NETSTAT_HEADER_ONLY		0x0001		/* netstat header only, no vci's*/
#define	NETSTAT_HEADER_16VCI	0x0002		/* header + 16 vci stats		*/
#define	NETSTAT_HEADER_32VCI	0x0003		/* header + 32 vci stats		*/
#define	RXSTAT_ENABLED			0x0004		/* receive user stats enabled	*/
#define	RXSTAT_DISABLED			0x0000
#define	TXSTAT_ENABLED			0x0008		/* transmit user stats enabled	*/
#define	TXSTAT_DISABLED			0x0000
#define	VPVC_MASK_DISABLED		0x0000		/* vp/vc filter mask disabled	*/
#define	VPVC_MASK_ENABLED		0x0010		/* filter mask enabled			*/
#define	VPVC_MASK_INVALID		0x0020		/* filter disabled				*/
#define	VPVC_NOT_MASK_ENABLED	0x0030		/* filter ~MASK enabled			*/
#define	DMA_TIMEOUT_MASK		0x0f00		/* time after which dma starts	*/
											/* clock * (2/i\ (26 - timeout))*/
#define	DMA_TIMEOUT_SHIFT		8
#define	DMA_TIMER_ENABLE		0x1000		
#define	DMA_TIMER_DISABLE		0x0000

#define DMA_COUNT_OVERFLOW_EN	0x2000		/* Use in REV B NTC ONLY		*/
#define DMA_TABLE_OVERFLOW_EN	0x4000		/* Use in REV B NTC ONLY		*/

#define	DMA_OVERFLOW_ENABLE		0x2000		/* Use in REV A NTC ONLY        */
#define	DMA_OVERFLOW_DISABLE	0x0000 		/* Use in REV A NTC ONLY        */

#define	FORCE_DMA_TRANSFER		0x8000




/*
**  Control registers 56 - Cell stream interface control 1
*/

#define CELL_STREAM_TX_ENABLE   0x0002		/* Use in REV B NTC ONLY		*/
#define CELL_STREAM_RX_ENABLE	0x0001		/* Use in REV B NTC ONLY		*/

#define	CELL_STREAM_ENABLE		0x0001		/* Use in REV A NTC ONLY		*/
#define	CELL_STREAM_DISABLE		0x0000		/* Use in REV A NTC ONLY		*/
#define	CSI_LOOPBACK_ENABLE_A	0x0002		/* Use in REV A NTC ONLY		*/
#define	CSI_LOOPBACK_DISABLE_A	0x0000		/* Use in REV A NTC ONLY		*/

#define	RXATC_ENABLED			0x0004		/* address translation control	*/
#define	RXATC_DISABLED			0x0000
#define	RXATE_FORWARD			0x0008		/* forward AT error cells		*/
#define	RXATE_DISCARD			0x0000		/* discard AT error cells		*/
#define	TXATC_ENABLED			0x0400		/* address translation control	*/
#define	TXATC_DISABLED			0x0000
#define	TXATE_FORWARD			0x0800		/* forward AT error cells		*/
#define	TXATE_DISCARD			0x0000		/* discard AT error cells		*/

#define	RXCLP_FROM_HEADER		0x0000		
#define	RXCLP_FROM_ATC			0x0010
#define	RXCLP_0					0x0020
#define	RXCLP_1					0x0030

#define	TXCLP_FROM_HEADER		0x0000		
#define	TXCLP_FROM_ATC			0x1000
#define	TXCLP_0					0x2000
#define	TXCLP_1					0x3000

#define	RXPLT_FROM_HEADER		0x0000		/* payload congestion bit option*/
#define	RXPLT_FROM_ATC			0x0040
#define	RXPLT_0					0x0080		/* force to 0					*/
#define	RXPLT_1					0x00C0		/* force to 1					*/

#define	TXPLT_FROM_HEADER		0x0000		/* payload congestion bit option*/
#define	TXPLT_FROM_ATC			0x4000
#define	TXPLT_0					0x8000		/* force to 0					*/
#define	TXPLT_1					0xC000		/* force to 1					*/

#define	ROUTING_TAG_SZ0			0x0000
#define	ROUTING_TAG_MASK		0x0300


/*
**  Control registers 57 - Cell stream interface control 2
*/

#define	TX_OMITHEC				0x0000		/* 53 byte cell transmission	*/

#define	RXHEC_ENABLE			0x0002		/* generate recv path hec 		*/
#define	RXHEC_DISBALE			0x0000

#define	RXHECMASK_ENABLE		0x0004		/* ACSI Rx path HEC mask enabled*/
#define	RXHECMASK_DISABLE		0x0000		/* ACSI Rx path HEC mask enabled*/

#define	CSI_LOOPBACK_ENABLE_B	0x0008		/* Use in REV B NTC ONLY		*/

/*
**  Control registers 52 - Statistics VP filter XMASK
*/

/*
**  Control registers 53 - Statistics VC filter XMASK
*/


/*
**  Control registers 54 - Statistics VP filter SMASK
*/

/*
**  Control registers 55 - Statistics VC filter SMASK
*/

/*
**  Control registers 58 - Interrupt enable
*/

#define	DMA_INTR_ENABLE			0x0001		/* DMA channel 0-5 interrupt 	*/
#define	LOS_INTR_ENABLE			0x0002		/* enable loss of signal intr	*/
#define	LOF_INTR_ENABLE			0x0004		/* enable loss of frame intr	*/
#define	LOP_INTR_ENABLE			0x0008		/* enable loss of pointer inter	*/
#define	OCD_INTR_ENABLE			0x0010		/* out of cell delineation inter*/
#define	ICD_INTR_ENABLE			0x0020		/* into cell delineation inter	*/
#define	BUFOV_INTR_ENABLE		0x0040		/* buffer overflow interrupt	*/
#define	SOFRX_INTR_ENABLE		0x0100		/* start of recv frame interrupt*/
#define	SOVRX_INTR_ENABLE		0x0200		/* start of VC recv interrupt	*/
#define	SOFTX_INTR_ENABLE		0x0400		/* start of frame transmit intr	*/
#define	BERR_INTR_ENABLE		0x0800		/* bus error interrupt			*/


/*
**  Control registers 59 - Interrupt under service
**	Set by the process to indicate corresponding interrupt is being 
**	serviced.  (1) It clears the interrupt request, (2) Inhibit any furhter 
**	interrupt requests of associated type being generated
*/

#define DMA_INTR_ACK         0x0001      /* DMA channel 0-5 interrupt    */
#define LOS_INTR_ACK         0x0002      /* enable loss of signal intr   */
#define LOF_INTR_ACK         0x0004      /* enable loss of frame intr    */
#define LOP_INTR_ACK         0x0008      /* enable loss of pointer inter */
#define OCD_INTR_ACK         0x0010      /* out of cell delineation inter*/
#define ICD_INTR_ACK         0x0020      /* into cell delineation inter  */
#define BUFOV_INTR_ACK       0x0040      /* buffer overflow interrupt    */
#define SOFRX_INTR_ACK       0x0100      /* start of recv frame interrupt*/
#define SOVRX_INTR_ACK       0x0200      /* start of VC recv interrupt   */
#define SOFTX_INTR_ACK       0x0400      /* start of frame transmit intr */
#define BERR_INTR_ACK        0x0800      /* bus error interrupt          */


/*
**  Control registers 60 - DMA Desriptor pointer table low.  Data written to
**	this register form the least significant 16 bits of the DMA descritptor
*	pointer table
*/


/*
**  Control registers 61 - DMA Desriptor pointer table low.  Data written to
**	this register form the most significant 7 bits of the DMA descritptor
**	pointer table
*/


/*
**  Control registers 62 - DMA channel activity
*/

#define	CHANNEL0_ENABLE		0x0001
#define	CHANNEL1_ENABLE		0x0002
#define	CHANNEL2_ENABLE		0x0004
#define	CHANNEL3_ENABLE		0x0008
#define	CHANNEL4_ENABLE		0x0010
#define	CHANNEL5_ENABLE		0x0020


/*
**  Control registers 63 - DMA mode
*/


#define	DMA_ENABLE			0x0001
#define	DMA_STOP			0x0002
#define	DMA_PRIORITY_0		0x0000			/* channel 0 has priority		*/
#define	DMA_PRIORITY_5		0x0004			/* channel 5 has priority		*/
#define	DMA_FAST_TRANSFER	0x0008			/* fast DMA cycle for descriptor*/
											/* transfer						*/
#define	DMA_TESTMODE_ENABLE	0x0080			/* internal testmode enabled	*/



/*
**
**	S T A T U S    R E G I S T E R S
**
*/

#define	NTC_RECVFRAMER_BYTE1	(NTC_SBASE+4)	/* recv framer overhead 	*/
												/* access bytes block 1 - 16*/
#define	NTC_RECVFRAMER_BYTE2	(NTC_SBASE+6)	
#define	NTC_RECVFRAMER_BYTE3	(NTC_SBASE+8)	
#define	NTC_RECVFRAMER_BYTE4	(NTC_SBASE+10)	
#define	NTC_RECVFRAMER_BYTE5	(NTC_SBASE+12)	
#define	NTC_RECVFRAMER_BYTE6	(NTC_SBASE+14)	
#define	NTC_RECVFRAMER_BYTE7	(NTC_SBASE+16)	
#define	NTC_RECVFRAMER_BYTE8	(NTC_SBASE+18)	
#define	NTC_RECVFRAMER_BYTE9	(NTC_SBASE+20)	
#define	NTC_RECVFRAMER_BYTE10	(NTC_SBASE+22)	
#define	NTC_RECVFRAMER_BYTE11	(NTC_SBASE+24)	
#define	NTC_RECVFRAMER_BYTE12	(NTC_SBASE+26)	
#define	NTC_RECVFRAMER_BYTE13	(NTC_SBASE+28)	
#define	NTC_RECVFRAMER_BYTE14	(NTC_SBASE+30)	
#define	NTC_RECVFRAMER_BYTE15	(NTC_SBASE+32)	
#define	NTC_RECVFRAMER_BYTE16	(NTC_SBASE+34)	

#define	NTC_CSI_RXTXC_BUFFFILL	(NTC_SBASE+114)	/* csi receive and transmit	*/
												/* buffer fill levels		*/
#define	NTC_INTERRUPT_STATUS	(NTC_SBASE+116)	/* interrupt status			*/
#define	NTC_PHY_ALARM			(NTC_SBASE+118)	/* physical layer alarm rcvd*/

#define	NTC_BUFOVERFLOW_INTR	(NTC_SBASE+122)	/* internal buffer overflow	*/
												/* interrupt indication		*/
#define	NTC_DMA_ACTIVITY_STATUS	(NTC_SBASE+124)	/* channel activity status	*/
#define	NTC_BERRDMA_INTR		(NTC_SBASE+126)	/* BERR/DMA interrupt ind	*/


/*
**	Status register 56 - ACSI Receive Transmit buffer fill level.  
**	This register shows the fill level of the ACSI receive and transmit 
**	cell buffer.  The count will be in cells (0-10)
**
*/


#define	TXBUF_FILL_LEVEL_MASK	0x0F00		
#define	TXBUF_FILL_LEVEL_SHIFT	8

#define	RXBUF_FILL_LEVEL_MASK	0xF000		
#define	RXBUF_FILL_LEVEL_SHIFT	12


/*
**	Status register 58 - Interrupt status register.  This register
**	shows the source of the interrupt.  The bits are identical to 
**	the interrupt control register (58)
**
*/

#define DMA_INTR				0x0001      /* DMA channel 0-5 interrupt    */
#define LOS_INTR				0x0002      /* enable loss of signal intr   */
#define LOF_INTR		        0x0004      /* enable loss of frame intr    */
#define LOP_INTR		        0x0008      /* enable loss of pointer inter */
#define OCD_INTR		        0x0010      /* out of cell delineation inter*/
#define ICD_INTR		        0x0020      /* into cell delineation inter  */
#define BUFOV_INTR				0x0040      /* buffer overflow interrupt    */
#define SOFRX_INTR				0x0100      /* start of recv frame interrupt*/
#define SOVRX_INTR				0x0200      /* start of VC recv interrupt   */
#define SOFTX_INTR				0x0400      /* start of frame transmit intr */
#define BERR_INTR		        0x0800      /* bus error interrupt          */


/*
**	Status register 59 - Physical layer alarms received
*/


#define	MS_AIS_ALARM_MASK		0x0001
#define	MS_FERF_ALARM_MASK		0x0002
#define	P_AIS_ALARM_MASK		0x0003
#define	P_FERF_ALARM_MASK		0x0004
#define	RAI_ALARM_MASK			0x0010
#define	B1BIP_ALARM_MASK		0x0004			/* set if b1/b2/b3 counts 	*/
#define	B2BIP_ALARM_MASK		0x0008			/* disagree with calculated */
#define	B3BIP_ALARM_MASK		0x0400			/* BIP						*/


/*
**	Status Register 61 - Internal Buffer Overflow Interrupt Indication
*/

#define HW_REV_MASK				0x0300			/* d8 and d9 indicate Rev B */


/*
**	DMA DESCRIPTORS
**		There are 6 dma channels:
**		channel 0: NTC statistics
**		channel 1: Switch statistics
**		channel 2: Cell receiver extraction
**		channel 3: Cell receiver insertion
**		channel 4: Cell transmitter extraction
**		channel 5: Cell transmitter insertion
**
*/

#define	NUM_NTC_DMA		6


/*
**	Descriptor location table consists of 6 channel descritpion location 
**	entries. For 32 bit operation, each entry has: 1 byte control, 24 bit 
**	descriptor pointer.
*/

typedef struct channeldesc_location {

	union {

		struct {
			UINT32	ccr_resvd:			4 PAK;	
			UINT32	ccr_fasttransfer:	1 PAK;
			UINT32	ccr_chainenable:	1 PAK;
			UINT32	ccr_inihibit:		1 PAK;
			UINT32	ccr_dec_inc:		1 PAK;
			UINT32	descriptor_ptr:		24 PAK;
		} channel_loc_bits;

		UINT32	channel_loc;

	} channel_loc_u;

} tCHANNELDESC_LOC, *tpCHANNELDESC_LOC;


extern	tCHANNELDESC_LOC channel_desc_table[NUM_NTC_DMA];
extern  tpCHANNELDESC_LOC	pDescLocTable;


/*
**	Descriptors:
*/


typedef struct channel_desc {

	void	*pData;
	UINT32	count;
	UINT32	resvd;
	UINT32  lcc; /* Last Character Count */
	UINT32	blockstatus;
	void	*pNext;

} tCHANNEL_DESC, *tpCHANNEL_DESC;

#define DIE_BCF 0x1000000 /* DMA Interrupt Enable in BCF */




/*
**
**	NETWORK STATISTICS RECORDS
**
*/

/* Network statistics header */

typedef struct nsrhdr {

	UINT32	nsrhdr_timestamp;		/* 32 bit time stamp					*/
	UINT32	nsrhdr_dcount;			/* Total number of cells recvd	at the 	*/
									/* interface but discarded due to errs	*/
	UINT32	nsrhdr_rcount;			/* Total numner of assigned ATM cells	*/
									/* recvd and not discarded				*/
	UINT32	nsrhdr_tcount;			/* Total number of ATM cells xmitted	*/
	UINT16	nsrhdr_hec_detected;	/* HEC detected count					*/
	UINT16	nsrhdr_hec_corrected;	/* HEC corrected count					*/
	UINT16	nsrhdr_b2febe;			/* B2 Far End Block Error byte count	*/
	UINT16	nsrhdr_b3febe;			/* B3 Far End Block Error byte count	*/
	UINT16	nsrhdr_uassigned_rx;	/* Unassigned cell received count		*/
	UINT16	nsrhdr_uassigned_tx;	/* Unassigned cell transmited count		*/
	UINT16	nsrhdr_ate_rx;			/* Addrees Translation error recv count	*/
	UINT16	nsrhdr_ate_tx;			/* Addrees Translation error xmit count	*/

} tNSRHDR, *tpNSRHDR;


/* User cell statistics with 32 bit counters */

typedef struct cellstats {

	union {

        struct {
			UINT32	active:	1 PAK;
			UINT32	resvd:	3 PAK;
			UINT32	vpi:	12 PAK;
			UINT32	vci:	16 PAK;
		} vcivpi_bits;
	
		UINT32	vcivpi1;

	} vcivpi1_u;

    union {

        struct {
            UINT32  active: 1 PAK;
            UINT32  resvd:  3 PAK;
            UINT32  vpi:    12 PAK;
            UINT32  vci:    16 PAK;
        } vcivpi_bits;
   
        UINT32  vcivpi2;

    } vcivpi2_u;

	
	UINT16	rxcell_count1;
	UINT16	txcell_count1;

	UINT16	rxcell_count2;
	UINT16	txcell_count2;
	
} t2CELLSTAT, *tp2CELLSTAT;

/* DMA Channel 0 (Zero) Block */
typedef struct ntc_dma_ch0_blk_ {
	tNSRHDR	nsr_hdr;				 /* Network Statistics Record	*/
	t2CELLSTAT user_2cell_stats[16]; /* Per VC Cell Stats (max 32)	*/
} tNTC_DMA_CH0_BLK, *tpNTC_DMA_CH0_BLK;


#define	CELLSTAT_ACTIVE			0x80000000
#define	CELLSTAT_VPI_MASK		0x0fff0000
#define	CELLSTAT_VPI(vcivpi)	( ((vcivpi)&CELLSTAT_VPI_MASK) >> 16 )
#define	CELLSTAT_VCI_MASK		0x0000ffff
#define	CELLSTAT_VCI(vcivpi)	( ((vcivpi)&CELLSTAT_VCI_MASK) )


/*
**	Operations on the NTC 
*/

#define	NTC_ENABLE				1
#define	NTC_DISBALE				2
#define	NTC_XMIT_ON				3
#define	NTC_XMIT_OFF			4
#define	NTC_RECV_ON				5
#define	NTC_RECV_OFF			6
#define	NTC_CSI_LOOPBACK_ON		7
#define	NTC_FRAMER_LOOPBACK_ON	8
#define	NTC_CSI_LOOPBACK_OFF	9
#define	NTC_RXTX_FILL			10
#define	NTC_OAM_ALARMS			11
#define	NTC_RECV_FRAME			12

#endif /* _NTC_IF_H_ */
