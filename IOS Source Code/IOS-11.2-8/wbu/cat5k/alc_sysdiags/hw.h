/* $Id: hw.h,v 1.1.4.1 1996/05/09 15:01:07 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/hw.h,v $
 *------------------------------------------------------------------
 * hw.h -- H/W Definitions & Constants for diagnostics
 *
 * December 1994, rlb (adapted to Synergy ATM line card)
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: hw.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:07  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:06  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:29  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:16  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:54:01  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:35  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*********************************************************************/
/*                                                                   */
/* CALYPSO DIAGNOSTICS                                               */
/*                                                                   */
/* This file contains "H/W Definitions & Constants" that are         */
/* included as part of CALYPSO Diagnostics.                          */
/*                                                                   */
/* Note: this is a port from Catalyst diagnostic to ATM line card    */
/*  		tabstop = 3                                                 */
/*********************************************************************/
/* 12/20/94  rlb
 *    adapted to Synergy ATM line card 
 */
#ifndef  _HW_H_
#define  _HW_H_

/* BOOTROM Size -> 256KB */
#define	BOOTROM_START	0x00010000
#define	BOOTROM_END		0x0001FFFF

/* FLASH Size -> 4MB */
#define	FLASH_START		0x00000000
#define	FLASH_END		0x000FFFFF
#define	FLASH_LENGTH	(FLASH_END - FLASH_START + 1)
#define	FLASH2_START	0x00100000
#define	FLASH2_END		0x001FFFFF
#define	FLASH2_LENGTH	(FLASH2_END - FLASH2_START + 1)

#ifdef   ALC_2
/* DRAM Size -> 16MB  Rev:2 */
/* #define  DRAM_START     ACP_HW_DRAM_BASE */  /* 0x40000000 32-bit, cachable */
#define  DRAM_START     0x40000000			/* 0x40000000 32-bit, cachable */
#define  DRAM_END       0x40FBFFFF
/* #define  DRAM_START_X   ACP_HW_NCRAM_BASE */  /* 0x48000000 32bit, non-cachable */
#define  DRAM_START_X   0x48000000		  /* 0x48000000 32bit, non-cachable */
#define  DRAM_END_X     0x48FBFFFF
#else    /* !ALC_2 */
/* DRAM Size -> 4MB  Rev:1 */
/* #define  DRAM_START     ACP_HW_DRAM_BASE */  /* 0x40000000 32-bit, cachable */
#define  DRAM_START     0x40000000			/* 0x40000000 32-bit, cachable */
#define  DRAM_END       0x403BFFFF
/* #define  DRAM_START_X   ACP_HW_NCRAM_BASE */ /* 0x48000000 32bit, non-cachable */
#define  DRAM_START_X   0x48000000			/* 0x48000000 32bit, non-cachable */
#define  DRAM_END_X     0x483BFFFF
#endif  /* ALC_2 */

/* Dual-port RAM(8051) Size -> 1KB */
#define	DPRAM_START		0x28000000
#define	DPRAM_END		0x280003ff

#define	MFP_BASE			0x30000000
#define  MFG_JUMPER_MASK         0x80

/*--- I/O Control Registers(byte wide) ---*/
#define	CNTL_REG_0	0x38000000
#define	CNTL_REG_1	(CNTL_REG_0 + 0x01)
#define	CNTL_REG_2	(CNTL_REG_0 + 0x02)
#define	CNTL_REG_3	(CNTL_REG_0 + 0x03)
#define	CNTL_REG_4	(CNTL_REG_0 + 0x04)
#define	CNTL_REG_5	(CNTL_REG_0 + 0x05)
#define	CNTL_REG_6	(CNTL_REG_0 + 0x06)
#define	CNTL_REG_7	(CNTL_REG_0 + 0x07)
#define	CNTL_REG_8	(CNTL_REG_0 + 0x08)
#define	CNTL_REG_9	(CNTL_REG_0 + 0x09)
#define	CNTL_REG_A	(CNTL_REG_0 + 0x0A)
#define	CNTL_REG_B	(CNTL_REG_0 + 0x0B)
#define	CNTL_REG_C	(CNTL_REG_0 + 0x0C)
#define	CNTL_REG_D	(CNTL_REG_0 + 0x0D)
#define	CNTL_REG_E	(CNTL_REG_0 + 0x0E)
#define	CNTL_REG_F	(CNTL_REG_0 + 0x0F)
#define	CNTL_REG_10	(CNTL_REG_0 + 0x10)
#define	CNTL_REG_11	(CNTL_REG_0 + 0x11)
#define	CNTL_REG_12	(CNTL_REG_0 + 0x12)
#define	CNTL_REG_13	(CNTL_REG_0 + 0x13)
#define	CNTL_REG_14	(CNTL_REG_0 + 0x14)
#define	CNTL_REG_15	(CNTL_REG_0 + 0x15)
#define	CNTL_REG_16	(CNTL_REG_0 + 0x16)	
#define	CNTL_REG_17	(CNTL_REG_0 + 0x17)
#define	CNTL_REG_38	(CNTL_REG_0 + 0x38 )

#define LCP_RESET_MASK          0x01
#define SAR_RESET_MASK          0x01
#define BIGA_RESET_MASK         0x01
#define WATCH_DOG_PUNCH_MASK    0x01

/*--- I/O Status Register(Byte wide) ---*/
#define	STATUS_REG			0x38000000
#define	COMM_STATUS_POWER_UP_MODE	0x80000000

#define	TXSAR_RAM_START	0xA0000000
#define	TXSAR_RAM_END		0xA007FFFF
#define	RXSAR_RAM_START	0xA8000000
#define	RXSAR_RAM_END		0xA807FFFF
#define	SAR_RAM_LENGTH   	(TXSAR_RAM_END - TXSAR_RAM_START + 1)

#define	SBUGA_START			0xB8000000
#define  LOCAL_BIGA_START	0x88000000    /* 16-bit access */
#define  BIGA_START			0xB8000000    /* 32-bit access */

#define	DMP_FLASH_START	0x08000000
#define	DMP_FLASH_END		0x080FFFFF
#define	DMP_FLASH_LENGTH	(DMP_FLASH_END - DMP_FLASH_START + 1)

#define COMM_DMP_REG4       0xAE3FFF90
#define COMM_DMP_REG5       0xAE3FFF94
#define COMM_DMP_REG6       0xAE3FFF98
#define COMM_DMP_REG7       0xAE3FFF9C
#define COMM_DMP_REG8       0xAE3FFFA0
#define COMM_DMP_REG9       0xAE3FFFA4
#define COMM_DMP_REG10      0xAE3FFFA8
#define COMM_DMP_REG11      0xAE3FFFAC
#define COMM_DMP_REG12      0xAE3FFFB0
#define COMM_DMP_REG13      0xAE3FFFB4
#define COMM_DMP_REG14      0xAE3FFFB8
#define COMM_DMP_REG15      0xAE3FFFBC
#define COMM_DMP_REG16      0xAE3FFFC0
#define COMM_DMP_REG17      0xAE3FFFC4
#define COMM_DMP_REG18      0xAE3FFFC8
#define COMM_DMP_REG19      0xAE3FFFCC
#define COMM_DMP_REG20      0xAE3FFFD0
#define COMM_DMP_REG21      0xAE3FFFD4
#define COMM_DMP_REG22      0xAE3FFFD8
#define COMM_DMP_REG23      0xAE3FFFDC
#define COMM_DMP_REG24      0xAE3FFFE0
#define COMM_DMP_REG25      0xAE3FFFE4
#define COMM_DMP_REG26      0xAE3FFFE8
#define COMM_DMP_REG27      0xAE3FFFEC
#define COMM_DMP_REG28      0xAE3FFFF0
#define COMM_DMP_REG29      0xAE3FFFF4
#define COMM_DMP_REG30      0xAE3FFFF8
#define COMM_DMP_REG31      0xAE3FFFFC

#define	COMM_DMP_COMPILEDATE	0x0E3FFF60
#define	COMM_DMP_COMPILETIME	0x0E3FFF70
#define	COMM_DMP_SYS_STATUS	0x0E3FFFC0
#define	COMM_DMP_MAC_STATUS	0x0E3FFFC2
#define	COMM_DMP_BIGA_STATUS	0x0E3FFFC4
#define	COMM_DMP_ETI_1_STATUS	0x0E3FFFD0
#define	COMM_DMP_ETI_2_STATUS	0x0E3FFFD2
#define	COMM_DMP_ETI_3_STATUS	0x0E3FFFD4
#define	COMM_DMP_ETI_4_STATUS	0x0E3FFFD6
#define	COMM_DMP_ETI_5_STATUS	0x0E3FFFD8
#define	COMM_DMP_ETI_6_STATUS	0x0E3FFFDA
#define	COMM_DMP_ETI_7_STATUS	0x0E3FFFDC
#define	COMM_DMP_ETI_8_STATUS	0x0E3FFFDE

#define	COMM_OEM_STRING		0x0E3FFFC0
#define	COMM_MODEL_STRING		0x0E3FFFC0
#define	COMM_MAC_BASE			0x0E3FFFD0
#define	COMM_SERIAL_NO			0x0E3FFFD8
#define	COMM_HW_VERSION_HIGH	0x0E3FFFDC
#define	COMM_HW_VERSION_LOW	0x0E3FFFDE

#define	COMM_YEAR				0x0E3FFFE0
#define	COMM_MONTH				0x0E3FFFE4
#define	COMM_DAY					0x0E3FFFE8
#define	COMM_UART_TXCHAR		0x0E3FFFEC
#define	COMM_UART_RXCHAR		0x0E3FFFED
#define	COMM_DMP_VERSION		0x0E3FFFEE
#define	COMM_DMP_REVISION		0x0E3FFFEF
#define	COMM_STATUS				0x0E3FFFF0
#define	COMM_NMP_TICK_TIME	0x0E3FFFF4
#define	COMM_NMP_SYSTIME		0x0E3FFFF8
#define	COMM_DMP_SYSTIME		0x0E3FFFFC

#define	COMM_STATUS_UART_VIRTUAL	0x00000001
#define	COMM_STATUS_UART_TXSTAT		0x00000002
#define	COMM_STATUS_UART_RXSTAT		0x00000004
#define	COMM_STATUS_STOP_QSRAM_ACCESS	0x00000008
#define	COMM_STATUS_PHY_BYPASS		0x00000010
#define	COMM_STATUS_PHY_LMLOOP		0x00000020
#define	COMM_STATUS_PHY_EBLOOP		0x00000040
#define	COMM_STATUS_PHY_XTLOOP		0x00000080
#define	COMM_STATUS_PHY_PMDLOOP		0x00000100
#define	COMM_STATUS_MEDIA_NOT_PRESENT	0x00000200
#define	COMM_STATUS_PHY_A				0x00000400
#define	COMM_STATUS_PHY_B				0x00000800
#define	COMM_STATUS_DISABLE_ETHERNET_LINKS	0x00001000
#define	COMM_STATUS_ENABLE_ETHERNET_LINKS	0x00002000
#define	COMM_STATUS_RESET_ETHERNET_BANK_A	0x00004000
#define	COMM_STATUS_RESET_ETHERNET_BANK_B	0x00008000
#define	COMM_STATUS_GET_OEM_STRING		0x00010000
#define	COMM_STATUS_GET_EEPROM_DATA		0x00020000
#define	COMM_STATUS_EEPROM_NOT_VALID		0x00040000
#define	COMM_STATUS_NMP_ARBITRATION		0x00080000
#define	COMM_STATUS_SWITCH_TO_NMP		0x00100000
#define	COMM_STATUS_DMP_FLASH_FAILURE	0x01000000
#define	COMM_STATUS_DMP_SRAM_FAILURE	0x02000000
#define	COMM_STATUS_DMP_DRAM_FAILURE	0x04000000
#define	COMM_STATUS_DMP_COMM_FAILURE	0x08000000
#define	COMM_STATUS_BYPASS_TESTS	0x40000000
#define	COMM_STATUS_POWER_UP_MODE	0x80000000


/* Frame Buffer Size -> 1MB,(up to 4 MB is supported) */
#define	FRAMEBUF_START		0xB0000000
#define	FRAMEBUF_END		0xB00FFF7F

#define	FRAMEBUF_START_ARBITRATE	0xB00FFF80
#define	FRAMEBUF_END_ARBITRATE		0xB00FFFBF


/* NVRAM Size -> 32KB */
#define	NVRAM_START		0x20000000
#define	LCPNVRAM		NVRAM_START
#define	NVRAM_END		0x2001FFFF
#define	NVRAM_SYSAREA_START	(NVRAM_END - (2*1024) - 1)

#ifndef BOOTNVRAM_START
#define	BOOTNVRAM_START		NVRAM_START
#define	BOOTNVRAM_LENGTH	(4*1024)
#define	FLASHNVRAM_START	(BOOTNVRAM_START + BOOTNVRAM_LENGTH)
#define	FLASHNVRAM_LENGTH	(2*1024)
#define	SMTMIB_START		(FLASHNVRAM_START + FLASHNVRAM_LENGTH)
#define	SMTMIB_LENGTH		(2*1024)
#define	CRESMIB_START		(SMTMIB_START + SMTMIB_LENGTH)
#define	CRESMIB_LENGTH		(16*1024)
#endif

/* Serial EEPROM Size -> 64x16 bits */
#define	EEPROM_START_L		0x00
#define	EEPROM_END_L		0x3F

#define	SERIAL_WR_PROTECT_MASK	0x01
#define	SERIAL_PRGM_ENABLE_MASK	0x01
#define	SERIAL_EEPROM_DI_MASK	0x01
#define	SERIAL_EEPROM_CS_MASK	0x01
#define	SERIAL_EEPROM_SK_MASK	0x01
#define	SERIAL_EEPROM_DO_MASK	0x80

#define	MAC_START		0x0A800000
#define	MAC_END			0x0A8FFFFF

#endif  /* _HW_H_ */


