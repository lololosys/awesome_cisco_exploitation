/* $Id: acp_hw_defs.h,v 1.1.4.1 1996/05/09 14:55:27 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/acp_hw_defs.h,v $
 *-----------------------------------------------------------------
 * acp_hw_defs.h -- ACP (68k) hardware address map definitions.
 *			This file is used with both assembly and C.
 *
 * November 15, 1994, Rick Lowe
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: acp_hw_defs.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:27  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:55  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:48  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:04  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:55  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:23  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.18  1995/10/20  05:04:52  rlowe
 * Fixes for early LCP reset release, so that ACP does not generate
 * unintended interrupts to the LCP.  Also clean up any stray LCP
 * RxDone interrupts to the ACP once the Module Ready startup message
 * is found.  Fix the LCP driver behaviour if HW guard timeout occurs
 * during Tx to LCP.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__ACP_HW_DEFS_H
#define	__ACP_HW_DEFS_H

#ifndef	SYNALC		/* Ensure this is defined for Synergy ATM Line Card. */
#define	SYNALC
#endif

#ifdef	ASMINCLUDE	/* For compatability with IOS code base. */
#include "asm_mode_defs.h"
#endif

	/* ============================================================	*/
	/*	Defines for standard logic levels.			*/
	/* ============================================================	*/

#define	ON	1			/* Active high. */
#define	OFF	0
#define	ON_	0			/* Active low.  */
#define	OFF_	1

	/* ============================================================	*/
	/*	Following are addresses and sizes of primary hardware	*/
	/*	functional areas.					*/
	/*	For Rev1 HW, PROM was just the first segment of		*/
	/*	the first flash device.					*/
	/*	For Rev2 HW, PROM is its own 512K device while		*/
	/*	flash is available as a SIMM.				*/
	/* ============================================================	*/

#define	ACP_HW_PROM_BASE	0x00000000	/* Byte wide, cachable */
#define	ACP_HW_PROM_SEGSIZE	(ACP_HW_OFLASH_SEGSIZE)
#ifdef	ACP_REV1_HW
#define	ACP_HW_PROM_SIZE	(1 * ACP_HW_PROM_SEGSIZE)	/* 0x00010000 */
#else
#define	ACP_HW_PROM_SIZE	(8 * ACP_HW_PROM_SEGSIZE)	/* 0x00080000 */
#endif

#define	ACP_HW_OFLASH_SEGSIZE	0x00010000
#ifdef	ACP_REV1_HW
#define	ACP_HW_FLASH_SEGSIZE	(ACP_HW_OFLASH_SEGSIZE)
#define	ACP_HW_FLASH_BASE	(1 * ACP_HW_FLASH_SEGSIZE)	/* 0x00010000 */
						/* Byte wide, cachable */
#define	ACP_HW_FLASH_SIZE	(0x00200000 - ACP_HW_PROM_SIZE)
#else
#define	ACP_HW_FLASH_SEGSIZE	(ACP_HW_OFLASH_SEGSIZE * 4) /* 4 devices wide */
#define	ACP_HW_FLASH_BASE	0x90000000	/* Long wide, cachable */
#define	ACP_HW_FLASH_SIZE	0x00400000
#define	ACP_HW_FLASH_DECODE_AREA 0x10000000	/* Flash decode, for ACU. */
#endif

#define	ACP_HW_NVRAM_BASE	0x20000000	/* Byte wide */
#define	ACP_HW_NVRAM_SIZE	0x00020000
#define	ACP_HW_LCP_RAM_BASE	0x28000000	/* Byte wide */
#define	ACP_HW_LCP_RAM_SIZE	0x00000400
#define	ACP_HW_MFP_BASE		0x30000000	/* Byte wide */
#define	ACP_HW_MFP_SIZE		0x00000080
#define	ACP_HW_IOREG_BASE	0x38000000	/* Byte wide */
#define	ACP_HW_IOREG_SIZE	0x00000040

#define	ACP_HW_DRAM_BASE	0x40000000	/* Long wide, cachable */
#ifdef	ACP_REV1_HW
#define	ACP_HW_DRAM_SIZE	0x00400000
#else
#define	ACP_HW_DRAM_SIZE	0x01000000
#endif

#define	ACP_HW_NCRAM_BASE	0x48000000	/* Long wide, non-cachable */
#define	ACP_HW_NCRAM_SIZE	(ACP_HW_DRAM_SIZE)
#define	ACP_HW_NTC_BASE		0x80000000	/* Short wide */
#define	ACP_HW_NTC_SIZE		0x00000080
#define	ACP_HW_BIGA_H_BASE	0x88000000	/* Short wide, host side */
#define	ACP_HW_BIGA_H_SIZE	0x00000100

#define	ACP_HW_TSAR_RAM_BASE	0xa0000000	/* Long wide */
#ifdef	ACP_REV1_HW
#define	ACP_HW_TSAR_RAM_SIZE	0x00020000
#else
#define	ACP_HW_TSAR_RAM_SIZE	0x00080000
#endif

#define	ACP_HW_RSAR_RAM_BASE	0xa8000000	/* Long wide */
#ifdef	ACP_REV1_HW
#define	ACP_HW_RSAR_RAM_SIZE	0x00020000
#else
#define	ACP_HW_RSAR_RAM_SIZE	0x00080000
#endif

#define	ACP_HW_FRBUF_BASE	0xb0000000	/* Long wide */
#define	ACP_HW_FRBUF_SIZE	0x00100000	/* Can grow to 4 Meg. */
#define	ACP_HW_BIGA_S_BASE	0xb8000000	/* Long wide */
#define	ACP_HW_BIGA_S_SIZE	0x00000100

#define	ACP_HW_LOCK_ADRBIT	0x01000000
#define	ACP_HW_NCRAM_ADRBIT	0x08000000	/* Flip cacheable <-> non. */
#define	ACP_HW_NONCACHE_ADR(adr) (((u_int32)(adr)) | (ACP_HW_NCRAM_ADRBIT))

	/* ============================================================	*/
	/*	Generator macros for basic definitions given above.	*/
	/*	Use "ACP_HW_GEN_DEF" as indirect step to ensure that	*/
	/*	"name" is always expanded out, eg. if another macro.	*/
	/* ============================================================	*/

#define	ACP_HW_GEN_DEF(name,tag) (ACP_HW_ ## name ## tag)
#define	ACP_HW_BASEADR(name)	(ACP_HW_GEN_DEF (name, _BASE))
#define	ACP_HW_BASEPTR(name)	((u_int8 *)(ACP_HW_BASEADR (name)))
#define	ACP_HW_BASELEN(name)	(ACP_HW_GEN_DEF (name, _SIZE))

	/* ============================================================	*/
	/*	Following are other physical constants for ACP board.	*/
	/* ============================================================	*/

#define	ACP_HW_CPU_CLK_HZ	25000000	/* Clock input freq. to CPU. */

	/* ============================================================	*/
	/*	Following are ACP board-level I/O register definitions.	*/
	/*	All registers are byte-wide, but only the debug and	*/
	/*	read registers have > 1 bit defined.  ie. Most write	*/
	/*	registers only use their LS bit.  Note that write	*/
	/*	registers are NOT readable and must be shadowed.	*/
	/*	All offsets given are relative to "ACP_HW_IOREG_BASE".	*/
	/* ============================================================	*/

#define	ACP_RDRG_RST_REASONS	0x00		/* 8-bits defined */
#define	ACP_RDRG_DEBUG		0x38		/* 8-bits defined */

#define	ACP_WRRG_LCP_RESET_	0x00		/* 1 bit, active low */
#define	ACP_WRRG_TSAR_RESET_	0x01		/* 1 bit, active low */
#define	ACP_WRRG_RSAR_RESET_	0x02		/* 1 bit, active low */
#define	ACP_WRRG_BIGA_RESET_	0x03		/* 1 bit, active low */
#define	ACP_WRRG_MFP_RESET_	0x04		/* 1 bit, active low */
#define	ACP_WRRG_NTC_RESET_	0x05		/* 1 bit, active low */
#define	ACP_WRRG_PMD_RESET_	0x06		/* 1 bit, active low */
#define	ACP_WRRG_SW_FORCE_RESET	0x07		/* 1 bit, active high */

#ifndef	ACP_REV1_HW		/* Not available with Rev-1. */
#define	ACP_WRRG_PGM_FLASH	0x08		/* 1 bit, active high */
#define	ACP_WRRG_ENAB_WDOG	0x0b		/* 1 bit, active high */
#endif

#define	ACP_WRRG_TP_TO_LCP_	0x0c		/* 1 bit, active low */
#define	ACP_WRRG_RD_FR_LCP_	0x0d		/* 1 bit, active low */
#define	ACP_WRRG_PMD_LOOP	0x0e		/* 1 bit, active high */
#define	ACP_WRRG_PMD_EXTCLK	0x0f		/* 1 bit, active high */

#define	ACP_WRRG_PMD_EEPROM_WR	0x10		/* 1 bit, active high */
#define	ACP_WRRG_PMD_EEPROM_SEL	0x11		/* 1 bit, active high */
#define	ACP_WRRG_PMD_EEPROM_CLK	0x12		/* 1 bit, active high */
#define	ACP_WRRG_PMD_EEPROM_PRE	0x13		/* 1 bit, active high */
#define	ACP_WRRG_PMD_EEPROM_PE	0x14		/* 1 bit, active high */
#define	ACP_WRRG_CLR_RST_REASNS	0x15		/* 1 bit, active high */
#define	ACP_WRRG_SW_FORCE_NMI	0x16		/* 1 bit, active high */
#define	ACP_WRRG_WATCHDOG	0x17		/* 1 bit, active high */

#define	ACP_WRRG_PMD_STAT_LED	0x30		/* LSBs of 2 bytes */
#define	ACP_WRRG_PMD_TOP_LED	0x32		/* LSBs of 2 bytes */
#define	ACP_WRRG_PMD_MID_LED	0x34		/* LSBs of 2 bytes */
#define	ACP_WRRG_PMD_BOT_LED	0x36		/* LSBs of 2 bytes */

#define	ACP_WRRG_DEBUG		0x38		/* 8-bits defined */

	/* ============================================================	*/
	/*	Following bit numbers are for Reset Reason Register.	*/
	/*	Use "(1 << bit-number)" to generate bit-mask.		*/
	/* ============================================================	*/

#define	ACP_RRB_BUTTON_NMI_	0		/* active low */
#define	ACP_RRB_SOFTWARE_NMI	1
#define	ACP_RRB_WATCHDOG_TO_	2		/* active low */
#define	ACP_RRB_BUTTON_RST	3
#define	ACP_RRB_SOFTWARE_RST	4
#define	ACP_RRB_BACKPLANE_RST	5
#define	ACP_RRB_WATCHDOG_RST	6
#define	ACP_RRB_PMD_EEPROM_RD	7

	/* ============================================================	*/
	/*	Following patterns control PMD LEDs.  Each LED is	*/
	/*	controlled by LSBs of a pair of consecutive bytes.	*/
	/*	Status LED uses different patterns than other LEDs.	*/
	/* ============================================================	*/

#define	ACP_PMD_STAT_LED_RED	0x0000
#define	ACP_PMD_STAT_LED_OFF	0x0100
#define	ACP_PMD_STAT_LED_YEL	0x0001
#define	ACP_PMD_STAT_LED_GRN	0x0101

#define	ACP_PMD_LEVL_LED_OFF	0x0000		/* Also 0x0101 */
#define	ACP_PMD_LEVL_LED_GRN	0x0100
#define	ACP_PMD_LEVL_LED_YEL	0x0001

	/* ============================================================	*/
	/*	Following bit numbers are for Debug Read Register.	*/
	/*	Use "(1 << bit-number)" to generate bit-mask.		*/
	/* ============================================================	*/

#define	ACP_RRB_JMP_DEBUG_	4		/* active low */
#define	ACP_RRB_JMP_FASTBOOT_	5		/* active low */
#define	ACP_RRB_JMP_SPARE_	6		/* active low */

#define	ACP_RRB_JMP_BASE_SHIFT	(ACP_RRB_JMP_DEBUG_)

	/* ============================================================	*/
	/*	Following bit numbers are for Debug Write Register.	*/
	/*	Use "(1 << bit-number)" to generate bit-mask.		*/
	/* ============================================================	*/

#define	ACP_WRB_DEBUG_LED0_	0		/* active low */
#define	ACP_WRB_DEBUG_LED1_	1		/* active low */
#define	ACP_WRB_DEBUG_LED2_	2		/* active low */
#define	ACP_WRB_DEBUG_LED3_	3		/* active low */

	/* ============================================================	*/
	/*	Following are for supporting interrupts to Tx/Rx SARs.	*/
	/* ============================================================	*/

#define	ACP_I2S_TX	(0)			/* For Tx SAR. */
#define	ACP_I2S_RX	(1)			/* For Rx SAR. */
#define	ACP_I2S_RSO	(1 << 27)		/* Rx SAR addr offset. */
#define	ACP_I2S_IRO	(1 << 21)		/* ACP->SAR Intr Reg Offset. */
#define	ACP_I2S_IDN	(1 << 24)		/* ACP->SAR Test Intr. */
#define	ACP_I2S_IDT	(1 << 31)		/* ACP->SAR Auto Intr. */

#define	ACP_INTR2_SAR_REG(tx_rx)		/* ACP->SAR interrupt reg. */\
		((ACP_HW_TSAR_RAM_BASE)    + \
		 ((tx_rx) * (ACP_I2S_RSO)) + \
		 (ACP_I2S_IRO))

#define	ACP_INTR2_SAR_DAT(normal,test)		/* ACP->SAR interrupt types. */\
		(((normal) * (ACP_I2S_IDN)) + ((test) * (ACP_I2S_IDT)))

	/* ============================================================	*/
	/*	Add any new definitions just above this comment.	*/
	/* ============================================================	*/

#endif	/* __ACP_HW_DEFS_H */
