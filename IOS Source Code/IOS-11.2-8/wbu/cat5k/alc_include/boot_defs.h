/* $Id: boot_defs.h,v 1.1.4.1 1996/05/09 14:55:31 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/boot_defs.h,v $
 *------------------------------------------------------------------
 * boot_defs.h -- ACP Boot code definitions header file.
 *
 * These definitions are typically common to both C and assembly
 * code.  Anything that C-specific is indicated (negatively) with
 * the flag ASM_MODE_FLG.
 *
 * November 1994, Rick Lowe
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: boot_defs.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:31  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:59  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:53  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:08  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:00  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:27  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__BOOT_DEFS_H
#define	__BOOT_DEFS_H

	/* ============================================================	*/
	/*	This is the ACP Boot code definitions header file.	*/
	/*	These definitions are typically common to both C and	*/
	/*	assembly code.  Anything that C-specific is indicated	*/
	/*	(negatively) with the flag ASM_MODE_FLG.		*/
	/*	By Rick Lowe, cisco, 94-11-01.				*/
	/*	This is:  %W% %G%	*/
	/* ============================================================	*/

#ifndef	__ACP_HW_DEFS_H
#include "acp_hw_defs.h"
#endif

	/* Basic HW values. */
#define	BOOT_PROM_START		(ACP_HW_BASEADR (PROM))		/* Boot code. */
#define	BOOT_FLASH_START	(ACP_HW_BASEADR (FLASH))
#define	BOOT_NVRAM_START	(ACP_HW_BASEADR (NVRAM))
#define	BOOT_MFP_START		(ACP_HW_BASEADR (MFP))
#define	BOOT_HW_IOREG_START	(ACP_HW_BASEADR (IOREG))
#define	BOOT_DRAM_START		(ACP_HW_BASEADR (DRAM))
#define	BOOT_TXSAR_RAM_START	(ACP_HW_BASEADR (TSAR_RAM))
#define	BOOT_RXSAR_RAM_START	(ACP_HW_BASEADR (RSAR_RAM))

#define	BOOT_NVRAM_SIZE		(ACP_HW_BASELEN (NVRAM))
#define	BOOT_DRAM_SIZE		(ACP_HW_BASELEN (DRAM))

	/* Public Boot usage values. */
#define	BOOT_PROM_SEGS		(ACP_HW_PROM_SIZE / ACP_HW_PROM_SEGSIZE)
						/* Flash segs for Boot code. */

#define	BOOT_MAX_STACK		(0x4000)	/* Boot should not exceed. */
#define	BOOT_EXCP_DATA_SZ	(180)

	/* Derived Boot usage values. */
#define	BOOT_HW_CPU_CLK_MHZ	(ACP_HW_CPU_CLK_HZ/1000000)

	/* Private Boot usage values. */
#define	BOOT_EXC_BREAKPOINT	1		/* For breakpoint exceptions. */
#define	BOOT_EXC_TRACE		2		/* For trace trap exceptions. */
#define	BOOT_EXC_DBGINIT	3		/* For Debug init exceptions. */
#define	BOOT_EXC_BUSERR		4		/* For Bus Error exceptions. */
#define	BOOT_EXC_ADDRERR	5		/* For Addr Error exceptions. */
#define	BOOT_EXC_ILLGLOP	6		/* For Illegal op exceptions. */

#define	BOOT_POWDIAG_PTRN_1	(0x55555555)	/* Power-up diag test pattern. */
#define	BOOT_POWDIAG_PTRN_2	(0xAAAAAAAA)	/* Power-up diag test pattern. */
#define	BOOT_POWDIAG_PTRN_3	(0x00553377)	/* Power-up diag test pattern. */

#define	ASM_PUSH_C_REGS	\
		moveml	a1/a0/d1/d0,sp@-	/* Protect regs used by C. */

#define	ASM_POP_C_REGS	\
		moveml  sp@+,d0/d1/a0/a1	/* Restore regs used by C. */

#define	ASM_CALL_C_ROUTINE(c_routine)	\
		ASM_PUSH_C_REGS ;	\
		jbsr	c_routine		/* Try C call posn-indep. */ ; \
		ASM_POP_C_REGS

#define	ASM_ADDR_STORE(addr,where)		/* Store given address. */\
		movel	&(addr),where

#define	ASM_ADDR_BOOT_CODE(where)		/* Get main BOOT entry pt. */\
		ASM_ADDR_STORE (__Boot_main, where)

#define	ASM_ADDR_PROM_CODE(where)		/* PROM == BOOT for us. */\
		ASM_ADDR_BOOT_CODE (where)

#define	ASM_ADDR_FLASH_CODE(where)		/* Get main FLASH entry pt. */\
		ASM_ADDR_STORE (BOOT_FLASH_START, where)

#define	ASM_ADDR_DRAM_CODE(where)		/* Get main DRAM entry pt. */\
		ASM_ADDR_STORE (BOOT_DRAM_START, where)

#define	ASM_ADDR_MFP(where)			/* Get base addr of MFP. */\
		ASM_ADDR_STORE (BOOT_MFP_START, where)

#define	ASM_ADDR_HW_REGS(where)			/* Get base addr of HW regs. */\
		ASM_ADDR_STORE (BOOT_HW_IOREG_START, where)

#define	ASM_ADDR_TXSAR_RAM(where)		/* Get addr TxSar RAM area. */\
		ASM_ADDR_STORE (BOOT_TXSAR_RAM_START, where)

#define	ASM_ADDR_RXSAR_RAM(where)		/* Get addr RxSar RAM area. */\
		ASM_ADDR_STORE (BOOT_RXSAR_RAM_START, where)

#define	ASM_DBG_RBIT_JDEBUG	(ACP_RRB_JMP_DEBUG_)
#define	ASM_DBG_RBIT_JFASTBT	(ACP_RRB_JMP_FASTBOOT_)
#define	ASM_DBG_RBIT_JSPARE	(ACP_RRB_JMP_SPARE_)

#define	ASM_DBG_WBIT_LED0	(ACP_WRB_DEBUG_LED0_)
#define	ASM_DBG_WBIT_LED1	(ACP_WRB_DEBUG_LED1_)
#define	ASM_DBG_WBIT_LED2	(ACP_WRB_DEBUG_LED2_)
#define	ASM_DBG_WBIT_LED3	(ACP_WRB_DEBUG_LED3_)

#define	ASM_READ_HWREG(basereg,which,where)	/* Read a HW Rd register. */\
		moveb	basereg@(which),where

#define	ASM_WRIT_HWREG(what,basereg,which)	/* Write HW Wr register. */\
		moveb	what,basereg@(which)

#define	ASM_DBG_RDREG(basereg,where)		/* Read from Debug Rd Reg. */\
		ASM_READ_HWREG (basereg, ASM_HW_RREG_DEBUG, where)

#define	ASM_DBG_WRREG(what,basereg)		/* Write to Debug Wr Reg. */\
		ASM_WRIT_HWREG (what, basereg, ASM_HW_WREG_DEBUG)

#define	ASM_RREG_RST_BIT_PB_NMI	(ACP_RRB_BUTTON_NMI_)
#define	ASM_RREG_RST_BIT_SW_NMI	(ACP_RRB_SOFTWARE_NMI)
#define	ASM_RREG_RST_BIT_WDGNMI	(ACP_RRB_WATCHDOG_TO_)

#define	ASM_HW_RREG_RSTRSNS	(ACP_RDRG_RST_REASONS)
#define	ASM_HW_RREG_DEBUG	(ACP_RDRG_DEBUG)

#define	ASM_HW_WREG_TXSAR_RESET	(ACP_WRRG_TSAR_RESET_)
#define	ASM_HW_WREG_RXSAR_RESET	(ACP_WRRG_RSAR_RESET_)
#define	ASM_HW_WREG_MFP_RESET	(ACP_WRRG_MFP_RESET_)
#define	ASM_HW_WREG_FRC_RESET	(ACP_WRRG_SW_FORCE_RESET)
#ifdef	ACP_REV1_HW
#define	ASM_HW_WREG_RED_ST_LED	8		/* RKL:  temporary only ! */
#define	ASM_HW_WREG_GRN_ST_LED	9
#define	ASM_HW_WREG_GRN_TX_LED	10
#define	ASM_HW_WREG_GRN_RX_LED	11
#endif
#define	ASM_HW_WREG_CLR_RSTRSN	(ACP_WRRG_CLR_RST_REASNS)
#define	ASM_HW_WREG_WATCHDOG	(ACP_WRRG_WATCHDOG)
#define	ASM_HW_WREG_DEBUG	(ACP_WRRG_DEBUG)

#define	ASM_CLR_HWREG(basereg,which)		/* Clr a HW Wr register. */\
		ASM_WRIT_HWREG (&(0), basereg, which)

#define	ASM_SET_HWREG(basereg,which)		/* Set a HW Wr register. */\
		ASM_WRIT_HWREG (&(1), basereg, which)

#define	ASM_CLR_RSTRSNS(basereg)	\
		ASM_SET_HWREG (basereg, ASM_HW_WREG_CLR_RSTRSN) ; \
		ASM_CLR_HWREG (basereg, ASM_HW_WREG_CLR_RSTRSN) ; \
		/* Write 1 to clear then 0 to allow latching next reason. */

#define	ASM_PET_WATCHDOG(basereg)	\
		ASM_SET_HWREG (basereg, ASM_HW_WREG_WATCHDOG) ; \
		ASM_CLR_HWREG (basereg, ASM_HW_WREG_WATCHDOG) ; \
		/* Write 1 then 0 to wdog to ensure pulse is generated. */

#define	ASM_TST_BIT(which,where)		/* Check indicated bit. */\
		btst	&(which),where

#define	ASM_CHKJPR_DEBUG(where)			/* Chk debug (j9) jumper. */\
		ASM_TST_BIT (ASM_DBG_RBIT_JDEBUG, where)

#define	ASM_CHKJPR_FASTBT(where)		/* Chk fastboot (j3) jumper. */\
		ASM_TST_BIT (ASM_DBG_RBIT_JFASTBT, where)

#define	ASM_CHKJPR_SPARE(where)			/* Chk spare (j2) jumper. */\
		ASM_TST_BIT (ASM_DBG_RBIT_JSPARE, where)

#endif	/* __BOOT_DEFS_H */
