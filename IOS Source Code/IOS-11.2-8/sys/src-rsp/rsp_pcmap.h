/* $Id: rsp_pcmap.h,v 3.4.28.1 1996/07/09 21:15:20 getchell Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_pcmap.h,v $
 *------------------------------------------------------------------
 * RSP Memory Map
 * 
 * May 1992, Carl Schaefer
 * 
 * Copyright (c) 1992-1996 by cisco Systems, Inc.  
 * All rights reserved. 
 *------------------------------------------------------------------
 * $Log: rsp_pcmap.h,v $
 * Revision 3.4.28.1  1996/07/09  21:15:20  getchell
 * Modify read bus exception handler and DCL interrrupt handler to deal
 * with dbus accesses to non-existent card.
 * CSCdi58555:  bus exception caused by dbus access to non-existant card
 * Branch: California_branch
 *
 * Revision 3.4  1996/01/18  07:08:10  schaefer
 * CSCdi42858: %RSP-3-ERROR CyBus0 master error/04 page map parity error
 * disable read-around-write when initializing pagemap entries
 *
 * Revision 3.3  1995/12/18  05:50:58  schaefer
 * CSCdi42858: %RSP-3-ERROR CyBus0 master error/04 page map parity error
 *
 * Revision 3.2  1995/11/17  18:49:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:28:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:28:40  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/08/08  03:16:38  mcieslak
 * MC ASIC HW Specification v2.3 was incorrect - remove
 * non-existant registers.
 * CSCdi38262:  Need to clean-up code review issues found in alaska merge
 *
 * Revision 2.4  1995/08/05  01:57:26  mcieslak
 * Remove unused code and adjust MC ASIC definitions.
 * CSCdi38262:  Need to clean-up code review issues found in alaska merge
 *
 * Revision 2.3  1995/08/04  18:02:27  mcieslak
 * Code review changes.
 * CSCdi37853:  CBUS-3-CCBPTIMEOUT with tracebacks
 *
 * Revision 2.2  1995/08/04  02:47:58  mcieslak
 * Fixed the initialization of the MEMD control register to
 * not make any assumptions of its initial state. Removed
 * unused #define relating to this register.
 * CSCdi37853:  CBUS-3-CCBPTIMEOUT with tracebacks
 *
 * Revision 2.1  1995/06/07  22:58:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __RSP_PCMAP_H__
#define __RSP_PCMAP_H__

/* virtual addresses */

#define ADRSPC_FLASH_BASE	0x38000000 /* on-board flash SIMM */

#define ADRSPC_CALENDAR		0x3E000000 /* TOD clock register */

#define ADRSPC_NVRAM		0x3E000000 /* Start of NV RAM */
#define   NVRAM_MAGIC_OFFSET	0x06 	   /* Magic number NVRAM
					      config offset*/
#define   NVRAM_START_OFFSET	0x08 	   /* NVRAM start offset. This
					      MUST be LONG aligned */
#define ADRSPC_NVRAM_END	0x3E07FFFF /* NVRAM address space end (512k)*/

#define ADRSPC_IOREGS		0x3e840000

#define ADRSPC_COUNTER0		0x3e840004	/* counter zero is watchdog */
#define ADRSPC_COUNTER1		0x3e84000c
#define ADRSPC_COUNTER2		0x3e840014

#define ADRSPC_COUNTCTL		0x3e84001c
#define   COUNTCTL_C2ENABLE	0x10
#define   COUNTCTL_C1ENABLE	0x20
#define   COUNTCTL_C0ENABLE	0x40

#define ADRSPC_INTMASK		0x3e840024
#define ADRSPC_INTVALUE		0x3e84002c	/* vector register */
#define ADRSPC_INTEND		0x3e840034
#define  INTEND_LEVEL0		0x01
#define  INTEND_LEVEL1		0x02
#define  INTEND_LEVEL2		0x04
#define  INTEND_LEVEL3		0x08
#define  INTEND_LEVEL4		0x10
#define  INTEND_LEVEL5		0x20
#define  INTEND_LEVEL6		0x40
#define  INTEND_LEVEL7		0x80
#define ADRSPC_INTVALIDMASK	0x3e84003c

#define ADRSPC_CLEAR_TIMER2	0x3e840044

#define ADRSPC_DCL		0x3e84004c
#define   DCL_DRAM_WRBUF	0x0800
#define   DCL_MEMD_WRBUF	0x0400
#define   DCL_IO_WRBUF		0x0200
#define   DCL_DRAM_RDBYPASS	0x0100
#define   DCL_MEMD_RDBYPASS	0x0080
#define   DCL_MEMD_DELAY_RSP1	0x0000
#define   DCL_MEMD_DELAY_RSP2	0x0040

#define ADRSPC_DRAM_CONFIG	0x3e840054
#define   DRAM_REFRESH_1US	0x00
#define   DRAM_REFRESH_15US	(0x0f << 3)
#define   DRAM_CONFIG_1BANK	0x00
#define   DRAM_CONFIG_2BANKS	0x80
#define ADRSPC_BANK_CONFIG	0x3e84005c
#define   DRAM_BANK1_64MB	0x01
#define   DRAM_BANK1_32MB	0x02
#define   DRAM_BANK1_16MB	0x04
#define   DRAM_BANK1_8MB	0x08
#define   DRAM_BANK2_64MB	0x10
#define   DRAM_BANK2_32MB	0x20
#define   DRAM_BANK2_16MB	0x40
#define   DRAM_BANK2_8MB	0x80

#define ADRSPC_DRAM_WREXCEPT_HI	0x3e840064
#define ADRSPC_DRAM_WREXCEPT_LO	0x3e84006c
#define ADRSPC_MEMD_WREXCEPT_HI	0x3e840074
#define ADRSPC_MEMD_WREXCEPT_LO	0x3e84007c
#define ADRSPC_IO_WREXCEPT_HI	0x3e840084
#define ADRSPC_IO_WREXCEPT_LO	0x3e84008c

#define ADRSPC_WATCH_HI		0x3e840094
#define ADRSPC_WATCH_LO		0x3e84009c
#define ADRSPC_WATCH_MASK_HI	0x3e8400a4
#define ADRSPC_WATCH_MASK_LO	0x3e8400ac
#define ADRSPC_WATCH_CONTROL	0x3e8400b4
#define   DCL_WATCH_INACTIVE	0x00
#define   DCL_WATCH_WRITE	0x40
#define   DCL_WATCH_READ	0x80
#define   DCL_WATCH_ANYCYCLE	0xc0
#define ADRSPC_WATCH_STATUS	0x3e8400bc
#define   DCL_WATCH_READ_BURST	0x11
#define   DCL_WATCH_READ_1B	0x18
#define   DCL_WATCH_READ_2B	0x19
#define   DCL_WATCH_READ_3B	0x1a
#define   DCL_WATCH_READ_4B	0x1b
#define   DCL_WATCH_READ_5B	0x1c
#define   DCL_WATCH_READ_6B	0x1d
#define   DCL_WATCH_READ_7B	0x1e
#define   DCL_WATCH_READ_8B	0x1f
#define   DCL_WATCH_WRITE_BURST	0x51
#define   DCL_WATCH_WRITE_1B	0x58
#define   DCL_WATCH_WRITE_2B	0x59
#define   DCL_WATCH_WRITE_3B	0x5a
#define   DCL_WATCH_WRITE_4B	0x5b
#define   DCL_WATCH_WRITE_5B	0x5c
#define   DCL_WATCH_WRITE_6B	0x5d
#define   DCL_WATCH_WRITE_7B	0x5e
#define   DCL_WATCH_WRITE_8B	0x5f

#define ADRSPC_TRAP_STATUS	0x3e8400c4
#define   TRAP_WATCH		0x10
#define   TRAP_IOEXCEPT		0x20
#define   TRAP_MEMDEXCEPT	0x40
#define   TRAP_DRAMEXCEPT	0x80

#define ADRSPC_SYSCONTROL	0x3e840204
#define   CONTROL_FLASH_VPP	0x01		/* active low */
#define   CONTROL_DBUS_ENABLE	0x02		/* active low (RSP1 only) */
#define   CONTROL_CBUS_ENABLE	0x04		/* active low */
#define   CONTROL_CBUS_RECEIVE	0x08		/* RO: 0 = another RSP prsnt */
#define   CONTROL_DALLAS_CLK	0x10
#define   CONTROL_DALLAS_DATA	0x20
#define   CONTROL_DALLAS_RESET	0x40
#define   CONTROL_DALLAS_WRITE	0x80

#define ADRSPC_CBUS_LOCK	0x3e84020c
#define   CBUS_LOCK_REQUEST	0x01
#define   CBUS_LOCK_GRANTACTIVE	0x02
#define   CBUS_IOBUS_REQUEST	0x04		/* RSP2 only */

#define ADRSPC_CBUS0REG		0x3e840214
#define ADRSPC_CBUS1REG		0x3e84021c
#define   CBUSX_LATCHED_STALL	0x01
#define   CBUSX_UNLATCHED_STALL	0x02
#define   CBUSX_GRANT_ENABLE	0x04
#define   CBUSX_PARITY_DISABLE	0x08		/* RSP2 ParityValid HW Fix */
#define   CBUSX_STALL_UPDATE	0x10		/* RSP2 only */

#define ADRSPC_RESETREASON	0x3e840224
#define   REASON_PERIPH		0x08
#define   REASON_SWNMI		0x04
#define   REASON_PBNMI		0x02
#define   REASON_WATCHDOG	0x01
#define   REASON_ALL		(REASON_PERIPH | REASON_SWNMI | \
				 REASON_PBNMI | REASON_WATCHDOG)

#define ADRSPC_SYSRESET		0x3e84022c

/* these bits are active low, invert before assigning */
#define ADRSPC_RESET_PERIPH	0x3e840234
#define   PERIPH_DBUS_IF	0x40		/* RSP2: dbus interface */
#define   PERIPH_DBUS		0x10		/* resets all IPs */
#define   PERIPH_PCMCIA		0x08
#define   PERIPH_IO_FPGA	0x04
#define   PERIPH_QA_ASIC	0x02
#define   PERIPH_MEMD_ASIC	0x01

#define ADRSPC_FLASHSIMM_REG	0x3e84023c
#define   FLASH_HWREV_MASK	0x000f
#define   FLASH_BANKSIZE_MASK	0x0e00
#define     FLASH_BANKSIZE_1M	0x0000
#define     FLASH_BANKSIZE_2M	0x0200
#define     FLASH_BANKSIZE_4M	0x0400
#define     FLASH_BANKSIZE_8M	0x0600
#define     FLASH_BANKSIZE_16M	0x0800
#define     FLASH_BANKSIZE_32M	0x0a00
#define   FLASH_INSTALLED_MASK	0x1000
#define     FLASH_INSTALLED_Y	0x0000
#define     FLASH_INSTALLED_N	0x1000
#define   FLASH_BANKS_MASK	0x2000
#define     FLASH_BANKS_ONE	0x2000
#define     FLASH_BANKS_TWO	0x0000

#define ADRSPC_RSPLED		0x3e840244

#define ADRSPC_PCMCIA_INDEX_REG_RSP1    0x3e840304
#define ADRSPC_PCMCIA_DATA_REG_RSP1     0x3e84030c

#define ADRSPC_PCMCIA_INDEX_REG_RSP2    0x3e8403e0
#define ADRSPC_PCMCIA_DATA_REG_RSP2     0x3e8403e1

#define ADRSPC_NMI		0x3e84024c

#define ADRSPC_DUART		UARTDEVBASEADDR	/* 0x3e840400 - cisco_rsp.h */

#define ADRSPC_DIAGBUS		0x3e841000	/* as master */
#define DIAGBUS_ADRSPC_LEN          0x0800  

#define ADRSPC_IDPROM		0x3e842000

/*
 * DBUS FPGA Registers are RSP2 specific and must be accessed as
 * 16 bit quantities. (0x3e843XXX)
 */
#define ADRSPC_DBUS_SLOT	0x3e843004	/* slave registers (RSP2) */
#define ADRSPC_DBUS_LAST_CMD	0x3e84300c
#define ADRSPC_DBUS_STATUS	0x3e843014

#define ADRSPC_MASTER_CMD	0x3e84302c
#define   MASTER_CMD_NOTHING	3
#define   MASTER_CMD_MASTER	2
#define   MASTER_CMD_SLAVE	1
#define   MASTER_CMD_NONPART	0

#define ADRSPC_MASTER_STATUS	0x3e843034
#define   MASTER_STATUS_MS_TIMER_ENABLE	0x20
#define   MASTER_STATUS_RCV_PRESENT	0x10
#define   MASTER_STATUS_OTHER_MASK	0x0C
#define   MASTER_STATUS_OTHER_NOTHING	0x0C
#define   MASTER_STATUS_OTHER_MASTER	0x08
#define   MASTER_STATUS_OTHER_SLAVE	0x04
#define   MASTER_STATUS_OTHER_NONPART	0x00
#define   MASTER_STATUS_OTHER_SHIFT     2
#define   MASTER_STATUS_THIS_MASK	0x03
#define   MASTER_STATUS_THIS_NOTHING	0x03
#define   MASTER_STATUS_THIS_MASTER	0x02
#define   MASTER_STATUS_THIS_SLAVE	0x01
#define   MASTER_STATUS_THIS_NONPART	0x00

#define ADRSPC_MASTER_TIMER_ENABLE 0x3e84303c
#define    MASTER_TIMER_ENABLE     0x0001

#define ADRSPC_MASTER_TIMER        0x3e843044
#define    MASTER_TIMER_COUNT_MASK 0x00ff
#define    MASTER_TIMER_10MSEC     0x00ff

#define ADRSPC_FPGA_INTR_STATUS	0x3e84304c
#define ADRSPC_FPGA_INTR_MASK	0x3e843054
#define   FPGA_PUSH_BUTTON	0x20
#define   FPGA_MC_ASIC		0x10
#define   FPGA_MS_CHANGE	0x08
#define   FPGA_MS_TIMEOUT	0x04
#define   FPGA_DBUS_CMD_FAIL	0x02
#define   FPGA_DBUS_SLAVE_REQ	0x01

#define ADRSPC_MEMD		0x40000000
#define ADRSPC_MEMD_QAREGS	0x48000000
#define ADRSPC_MEMD_BUFHDR	0x48002000
#define ADRSPC_MEMD_BUFDATA	(ADRSPC_MEMD + MEMD_BUFDATA_OFFSET)
#define ADRSPC_MEMD_CONTROLSPC	0x48e00000

#define ADRSPC_QA_ERRLOG0_3	0x48e08000
#define   QA_ERR_READWRITE(x)	((x) >> 31)
#define     QA_ERR_READ		1
#define     QA_ERR_WRITE	0
#define   QA_ERR_ATTENTION(x)	(((x) >> 26) & 0x1f)
#define     QA_ATTN_WRPARITY	0x10 /* bad parity on write */
#define     QA_ATTN_REUSE0	0x08 /* enqueue in-use element or zero */
#define     QA_ATTN_RDPARITY	0x04 /* memory parity during read */
#define     QA_ATTN_ALTQWR	0x02 /* write to alternate queue */
#define     QA_ATTN_LOWBITS	0x01 /* bits [3:0] of queue write non-zero */
#define   QA_ERR_SELECT(x)	(((x) >> 24) & 0x03)
#define   QA_ERR_ADDR(x)	(((x) >> 8)  & 0x00ffff)
#define   QA_ERR_DATAPAR(x)	((x) & 0xff)
#define ADRSPC_QA_ERRLOG4_7	0x48e08008
#define ADRSPC_QA_ERRLOG8_11	0x48e08010
#define ADRSPC_QA_BASE		0x48e08018
#define ADRSPC_QA_CONTROL	0x48e08020

#define ADRSPC_PAGEMAP_BASE	0x48e10000
/*
 * to change a pagemap entry, set the high byte first, then the low byte.
 * the change is installed atomically after the second write.
 *
 * due to a MC bug, it is necessary for read-around-write to be disabled
 * around the writes to a given pagemap entry.
 */
#define PAGEMAP_HI(index)	\
	(*(volatile uchar*) (ADRSPC_PAGEMAP_BASE + 0x10*(index)))
#define PAGEMAP_LO(index)	\
	(*(volatile uchar*) (ADRSPC_PAGEMAP_BASE + 0x10*(index) + 8))
#define PMINDEX(slot, page)	((slot) * PAGEMAPS_PER_SLOT + (page))
#define PAGEMAP_SLOTS		16
#define PAGEMAPS_PER_SLOT	32
#define PMVALUE_HI_VALID	0x02
#define PMVALUE_HI(page, valid)	(((((page) >> 9) & 0x7) << 3)		| \
				 ((valid) ? PMVALUE_HI_VALID : 0)	| \
				 (((page) >> 8) & 0x1))
#define PMVALUE_LO(page)	((page) & 0xff)

#define ADRSPC_PAGESIZE8_15	0x48e14000
#define   PAGESIZE_4K		1
#define   PAGESIZE_8K		0
#define ADRSPC_PAGESIZE0_7	0x48e14008
#define ADRSPC_MEMD_CONFIG	0x48e14020
#define   MEMD_CONFIG_512K	0x00
#define   MEMD_CONFIG_2M	0x01
#define ADRSPC_MEMD_ERRSTATUS	0x48e14028
#define   MEMDERR_CHASINT	0x80		/* RSP1 only */
#define   MEMDERR_QA		0x40
#define   MEMDERR_CYMEMD	0x20

#define ADRSPC_QA_CMDINT_MASK	0x48e14030
#define   QA_INT_ENABLE		0x00
#define   QA_INT_DISABLE	0x01
#define ADRSPC_QA_NETINT_MASK	0x48e14038

#define ADRSPC_MEMD_DIAG	0x48e14040
#define   DIAG_ERRCK_ENABLE	0x01 /* check RP -> MEMD parity, alignment */
#define   DIAG_BAD_PARITY	0x02 /* force bad parity into page map */
#define   DIAG_CYIO_DISABLE	0x04 /* RSP1 - disable requests on cybus */
#define   DIAG_CY0IO_ENABLE	0x04 /* RSP2 - enable requests on cy0 */
#define   DIAG_CY1IO_ENABLE	0x08 /* RSP2 - enable requests on cy1 */
#define   DIAG_CY_CYCLESTEAL	0x10 /* enable cycle stealing */
#define   DIAG_PARITY_EVEN	0x20 /* set even parity (defaults to odd) */
#define   DIAG_IN_ORDER		0x40 /* ??? */
#define ADRSPC_RPERRSTAT	0x48e14048
#define   RPERR_CY_TIMEOUT	0x01 /* RSP2 - slave transfer to cy timeout */
#define   RPERR_CY_ALIGN	0x02 /* RSP2 - misaligned slave access */
#define   RPERR_MEMD_DIAG	0x04
#define   RPERR_QA_WRITE	0x08
#define   RPERR_QA_READ		0x10
#define   RPERR_SRAM_WRITE	0x20
#define   RPERR_SRAM_READ	0x40
#define   RPERR_ADDRESS		0x80 /* RSP1 - address out of range */
#define ADRSPC_CYDATPARERR	0x48e14050	/* RSP1 (ushort) */
#define   CYDATPAR_WRITE(x)	((x) >> 8)	/* 1 indicates error */
#define   CYDATPAR_READ(x)	(~(x) & 0xff)	/* 0 indicates error */
#define ADRSPC_MDERRINFO	0x48e14050	/* RSP2 (u64) */
#define   MDERR_LOG_VALID	0x0080
#define   MDERR_MC_CTRL(x)	(((x) & 0x0070) >> 4)
#define   MDERR_RP_ERRS(x)	((x) & 0x000F)
#define   MDERR_SRAM_ERRS(x)	(((x) & 0xF000) >> 12)
#define   MDERR_QA_ERRS(x)	(((x) & 0x0300) >> 8)
#define   MDERR_CY0_ERRS(x)	(((x) & 0x00F0) >> 4)
#define   MDERR_CY1_ERRS(x)	(((x) & 0x000F))
#define   MDERR_MERGE_HIGH_LOW(x, y) \
		((((x) & 0x0C) << 4) | ((((x) & 0x03) | ((y) & 0x0C)) << 2) | \
		((y) & 0x03))
#define ADRSPC_CYVA23_17	0x48e14058
#define   CYVIRTADDR23_17(x)	((x) >> 1)
#define ADRSPC_CYPHYSADDR	0x48e14060
#define   CYPHYSADDR20_16(x)	((x) & 0x1f)	/* RSP1 */
#define   CYPHYSADDR20_13(x)	(x)		/* RSP2 */
#define ADRSPC_CYMISC		0x48e14068
#define   CYMISC_PA15(x)	((x) >> 7)		/* RSP1 */
#define   CYMISC_PA12(x)	((x) >> 7)		/* RSP2 */
#define   CYMISC_PA2_0(x)	(((x) >> 4) & 0x07)	/* RSP1 */
#define   CYMISC_PA3_1(x)	(((x) >> 4) & 0x07)	/* RSP2 */
#define   CYMISC_CYADDRPAR23_16	0x08
#define   CYMISC_CYADDRPAR15_8	0x04
#define   CYMISC_CYADDRPAR7_0	0x02
#define ADRSPC_CYERRCMD		0x48e14070
#define   CYERRCMD_CMD(x)	((x) & 0x0f)
#define     CYCMD_READ_8B	0x01
#define     CYCMD_WRITE_8B	0x07
#define     CYCMD_READ_4B	0x08
#define     CYCMD_WRITE_1B	0x0b
#define     CYCMD_WRITE_2B	0x0d
#define     CYCMD_WRITE_4B	0x0e
#define ADRSPC_CYERR		0x48e14078
#define   CYERR_DATPAR_MEMDRD	0x01		/* RSP1 */
#define   CYERR_BADWRITEDAT_PM	0x02		/* RSP1 */
#define   CYERR_PM_PARITY	0x04		/* RSP1 */
#define   CYERR_PM_INVALIDREG	0x08		/* RSP1 */
#define   CYERR_CMDADDR		0x10
#define   CYERR_INVALIDCMD	0x20		/* RSP1 */
#define   CYERR_ADDRPARITY	0x40		/* RSP1 */
#define   CYERR_DATAPARITY	0x80		/* RSP1 */
#define   CYERR_PMT_WRPARITY	0x02		/* RSP2 */
#define   CYERR_PMT_PARITY	0x04		/* RSP2 */
#define   CYERR_PMT_INVALID	0x08		/* RSP2 */
#define   CYERR_BADCMD		0x20		/* RSP2 */
#define   CYERR_BADADDR		0x40		/* RSP2 */
#define ADRSPC_MEMD_BITSWAP	0x50000000
#define ADRSPC_MEMD_WRITETHRU	0x54000000
#define ADRSPC_MEMD_UNCACHED	0x58000000

/*
 * RSP2 registers
 */
#define ADRSPC_PA_SLAVE_XLATE	0x48e14200
#define ADRSPC_ATHI_COUNT_HIGH	0x48e14300
#define ADRSPC_ATHI_COUNT_LOW	0x48e14308
#define ADRSPC_ATLO_COUNT_HIGH	0x48e14300
#define ADRSPC_ATLO_COUNT_LOW	0x48e14308
#define ADRSPC_CY1VA23_17	0x48e14500
#define ADRSPC_CY1PHYSADDR	0x48e14508
#define ADRSPC_CY1MISC		0x48e14510
#define ADRSPC_CY1ERRCMD	0x48e14518
#define ADRSPC_CY1ERR		0x48e14520
#define ADRSPC_CYMASTER_ERR	0x48e14528
#define   CYMERR0_NO_ACK_NACK	0x01
#define   CYMERR0_NACK		0x02
#define   CYMERR0_INVALIDCMD	0x04
#define   CYMERR1_NO_ACK_NACK	0x08
#define   CYMERR1_NACK		0x10
#define   CYMERR1_INVALIDCMD	0x20
#define   CYMERR0_ARB_TIMEOUT	0x40
#define   CYMERR1_ARB_TIMEOUT	0x80
#define   CYMERR0_MASK		(CYMERR0_ARB_TIMEOUT | CYMERR0_INVALIDCMD | \
				CYMERR0_NACK | CYMERR0_NO_ACK_NACK)
#define   CYMERR1_MASK		(CYMERR1_ARB_TIMEOUT | CYMERR1_INVALIDCMD | \
				CYMERR1_NACK | CYMERR1_NO_ACK_NACK)
#define ADRSPC_MC_INTR_STATUS	0x48e14530
#define   MC_INTR_RP_PROBLEM	0x02
#define   MC_INTR_QA		0x04
#define   MC_INTR_MD1		0x08
#define   MC_INTR_MD0		0x10
#define   MC_INTR_CY1		0x20
#define   MC_INTR_CY0		0x40
#define   MC_INTR_RP_FAIL	0x80
#define ADRSPC_MC_INTR_MASK	0x48e14538

#define ADRSPC_QA_STATUS_RANGE	0x48e14540
#define   RANGE(upper,lower)	(((upper) << 4) | (lower))
#define   RANGE_UPPER(x)	((x) >> 4)
#define   RANGE_LOWER(x)	((x) & 0x0f)
#define ADRSPC_QA_RANGE		0x48e14548
#define ADRSPC_BUFHDR_RANGE	0x48e14550
#define ADRSPC_BUFFER_RANGE	0x48e14558
#define ADRSPC_NACK_RANGE	0x48e14560
#define ADRSPC_CY0_7DETERMINE	0x48e14568
#define ADRSPC_CY8_15DETERMINE	0x48e14570
#define ADRSPC_CY_DIAG_LOOPBACK	0x48e14578	/* boolean, active high */

#define ADRSPC_CYPERF_RP2	0x48e14608	/* read unlocks counter */
#define ADRSPC_CYPERF_RP1	0x48e14610
#define ADRSPC_CYPERF_RP0	0x48e14618	/* read locks counter value */

#define ADRSPC_CYPERF_CY0_2	0x48e14628
#define ADRSPC_CYPERF_CY0_1	0x48e14630
#define ADRSPC_CYPERF_CY0_0	0x48e14638

#define ADRSPC_CYPERF_CY1_2	0x48e14648
#define ADRSPC_CYPERF_CY1_1	0x48e14650
#define ADRSPC_CYPERF_CY1_0	0x48e14658

/*
 * Level 2 Cache control. The level 2 cache is controlled by doing
 * uncached reads at certain addresses. Presently, RSP does not
 * have a level 2 cache, so we point these reads at the first
 * location in DRAM.
 */
#define LVL2_CACHE_TAGLINES_10	0x0000

/* physical addresses */

#define PHY_MEMD		0x08000000
#define PHY_MEMD_OTHER		0x40000000
#define PHY_MEMD_QAREGS		PHY_MEMD
#define PHY_MEMD_BUFHDR		(PHY_MEMD + MEMD_BUFHDR_OFFSET)
#define PHY_MEMD_CONTROLSPC	(PHY_MEMD + 0xe00000)
#define PHY_FLASH_BASE		0x18000000
#define PHY_IOREGS		0x1e840000
#define PHY_PROM		0x1fc00000

#define PHY_MEMD_BITSWAP	0x50000000

/*
 * Level 2 Cache definitions - Note that the RSP will never support
 * a level 2 cache
 */
#define LVL2_CACHE_TAGLINES_10	0x0000	   	

#define ACTIVE_LOW(x)		(~(x))

/* 
 * moved here from pcmap.h of RSP ROM Monitor
 */



/*
 * R4K Address space definitions
 */

#define ADRSPC_K0BASE    (0x80000000)
#define ADRSPC_K0SIZE    (0x20000000)
#define ADRSPC_K1BASE    (0xA0000000)
#define ADRSPC_K1SIZE    (0x20000000)
#define ADRSPC_KSBASE    (0xC0000000)
#define ADRSPC_KSSIZE    (0x20000000)
#define ADRSPC_K3BASE    (0xE0000000)
#define ADRSPC_K3SIZE    (0x20000000)
#define ADRSPC_KUBASE    (0x00000000)
#define ADRSPC_KUSIZE    (0x80000000)
#define KSEG_MSB_ADDR    0xFFFFFFFF


#define NVRAM_MAGIC_OFFSET	0x06 	   /* Magic number NVRAM
					      config offset*/
#define NVRAM_START_OFFSET	0x08 	   /* NVRAM start offset */

/* 
 * This value is related to the internal clock of R4k
 */
#define MON_TIMER_VAL_20MS      (0x30d40 * 5)  /* 20ms tick */

#define RESET_VEC_OFFSET        0xC00000   /* vector offset to eprom base */

/*
** Defines for the Monitor
** R4000/Orion Address translation macros
*/
#define PHY_TO_KSEG1(x)    ((x)+0xA0000000)
#define PHY_TO_KSEG0(x)    ((x)+0x80000000)
#define PHY_ADDR(x)        ((x)&0x1FFFFFFF)
#define ROM_OFFSET(x)      ((x)&0x000FFFFF)
#define KSEG1_TO_KSEG0(x)  ((x)-0x20000000)
#define KUSEG_TO_KSEG1(x)  (((x)-0x20000000)+0xA0000000)


/*
** Address space allocations.
** These are physical addresses.
*/

#define PHY_ADRSPC_RAM		0x00000000 /* Start of RAM */
#define PHY_ADRSPC_RAM_END      0x08000000 /* End of RAM area (up to 128 MB) */

#define PHY_ADRSPC_MEMD    	0x08000000 /* Start of MEMD */
#define PHY_ADRSPC_MEMD_END    	0x10000000 /* End of MEMD area (up to 128 MB) */
#define PHY_ADRSPC_MEMD_ASIC    0x08E10000 /* Start of MEMD ASIC Diagnostic Area */
#define MEMD_ADR_LINE		20	   /* Addr0-27 to address MEMD */

#define PHY_ADRSPC_FLASH    	0x10000000 /* Flash EPROM start address */
#define PHY_ADRSPC_FLASH_BASE	0x10000000 /* Flash EPROM start address */
#define PHY_ADRSPC_FLASH_END	0x17000000 /* End of flash EPROM area (up to 128 M) */

#define PHY_ADRSPC_PCMCIA    	0x17000000 /* PCMCIA slots start address */
#define PHY_ADRSPC_PCMCIA_END	0x18000000 /* End of PCMCIA slots */
#define PHY_ADRSPC_SLOT0        0x17400000 /* PCMCIA slot a */
#define PHY_ADRSPC_SLOT1        0x17c00000 /* PCMCIA slot b */
#define PHY_ADRSPC_PCMCIA_REG  	0x1E840304 /* PCMCIA config reg start address */

#define PHY_ADRSPC_ID_EEPROM    0x1E842000 /* ID EEPROM */

#define PHY_ADRSPC_RXFLASH    	0x18000000 /* RXBoot Flash Memory start address */
#define PHY_ADRSPC_RXFLASH_END	0x1C000000 /* End of RXBoot Flash Memory(up to 64 MB)*/

/*
#define PHY_ADRSPC_SLOTIO    	0x1C000000 
#define PHY_ADRSPC_SLOTIO_END	0x1E000000 
*/

#define PHY_ADRSPC_NVRAM	0x1E000000 /* Start of NV RAM */
#define PHY_ADRSPC_NVRAM_END	0x1E008000 /* End of NVRAM area (32k) */

/*
#define PHY_ADRSPC_CPUIO	0x1E800000 
#define PHY_ADRSPC_CPUIO_END	0x1F000000 
*/

#define PHY_ADRSPC_PROM		0x1F000000 /* Start of EPROM */
#define PHY_ADRSPC_PROM_END	0x20000000 /* End of EPROM area (up to 16 MB)*/

#define PHY_ADRSPC_DCL_ASIC     0x1E840000 /* DCL asic base address */

#define PHY_ADRSPC_REG_FPGA     0x1E840200 /* Register FPGA base address */

#define PHY_ADRSPC_IRSP2_DBUS_FPGA	0x1E843000 /* RP DBus base address */

#define PHY_ADRSPC_BAD_ADDR     0x1DFF0000 /* address that will generate a 
					      bus error */

#define PHY_ADRSPC_DUART     	0x1E840400 /* DUART base address */
#define PHY_ADRSPC_DIAGBUS      0x1E841000 /* Beginning of DBUS) */

/*
 * Virtual address space allocations - Processor in Kernel Mode (kseg1)
 * These are virtual addresses for Kernel Segment 1.
 */

#define KSEG1_ADRSPC_RAM           PHY_TO_KSEG1(PHY_ADRSPC_RAM)
#define KSEG1_ADRSPC_RAM_END       PHY_TO_KSEG1(PHY_ADRSPC_RAM_END)

#define KSEG1_ADRSPC_MEMD          PHY_TO_KSEG1(PHY_ADRSPC_MEMD)
#define KSEG1_ADRSPC_MEMD_END      PHY_TO_KSEG1(PHY_ADRSPC_MEMD_END)

#define KSEG1_ADRSPC_FLASH         PHY_TO_KSEG1(PHY_ADRSPC_FLASH)
#define KSEG1_ADRSPC_FLASH_BASE    PHY_TO_KSEG1(PHY_ADRSPC_FLASH_BASE)
#define KSEG1_ADRSPC_FLASH_END     PHY_TO_KSEG1(PHY_ADRSPC_FLASH_END)

#define KSEG1_ADRSPC_PCMCIA_REG	   PHY_TO_KSEG1(PHY_ADRSPC_PCMCIA_REG)

#define KSEG1_ADRSPC_PCMCIA        PHY_TO_KSEG1(PHY_ADRSPC_PCMCIA)
#define KSEG1_ADRSPC_PCMCIA_END    PHY_TO_KSEG1(PHY_ADRSPC_PCMCIA_END)
#define KSEG1_ADRSPC_SLOT0         PHY_TO_KSEG1(PHY_ADRSPC_SLOT0)
#define KSEG1_ADRSPC_SLOT1         PHY_TO_KSEG1(PHY_ADRSPC_SLOT1)

#define KSEG1_ADRSPC_ID_EEPROM     PHY_TO_KSEG1(PHY_ADRSPC_ID_EEPROM)

#define KSEG1_ADRSPC_RXFLASH       PHY_TO_KSEG1(PHY_ADRSPC_RXFLASH)
#define KSEG1_ADRSPC_RXFLASH_END   PHY_TO_KSEG1(PHY_ADRSPC_RXFLASH_END)

#define KSEG1_ADRSPC_BAD_ADDR      PHY_TO_KSEG1(PHY_ADRSPC_BAD_ADDR)

#define KSEG1_ADRSPC_SLOTIO        PHY_TO_KSEG1(PHY_ADRSPC_SLOTIO)
#define KSEG1_ADRSPC_SLOTIO_END    PHY_TO_KSEG1(PHY_ADRSPC_SLOTIO_END)

#define KSEG1_ADRSPC_NVRAM         PHY_TO_KSEG1(PHY_ADRSPC_NVRAM)
#define KSEG1_ADRSPC_NVRAM_END     PHY_TO_KSEG1(PHY_ADRSPC_NVRAM_END)

#define KSEG1_ADRSPC_CPUIO         PHY_TO_KSEG1(PHY_ADRSPC_CPUIO)
#define KSEG1_ADRSPC_CPUIO_END     PHY_TO_KSEG1(PHY_ADRSPC_CPUIO_END)

#define KSEG1_ADRSPC_PROM          PHY_TO_KSEG1(PHY_ADRSPC_PROM)
#define KSEG1_ADRSPC_PROM_END	   PHY_TO_KSEG1(PHY_ADRSPC_PROM_END)

#define KSEG1_ADRSPC_DCL_ASIC      PHY_TO_KSEG1(PHY_ADRSPC_DCL_ASIC)
#define KSEG1_ADRSPC_MEMD_ASIC     PHY_TO_KSEG1(PHY_ADRSPC_MEMD_ASIC)

#define KSEG1_ADRSPC_REG_FPGA      PHY_TO_KSEG1(PHY_ADRSPC_REG_FPGA)

#define KSEG1_ADRSPC_IRSP2_DBUS_FPGA	PHY_TO_KSEG1(PHY_ADRSPC_IRSP2_DBUS_FPGA)

#define KSEG1_ADRSPC_DUART         PHY_TO_KSEG1(PHY_ADRSPC_DUART)

#define KSEG1_ADRSPC_DIAGBUS       PHY_TO_KSEG1(PHY_ADRSPC_DIAGBUS)

/*
** Stack address definition for the ROM Monitor
*/

#define MON_INIT_SP   	(ADRSPC_K1BASE + 0x6000) 

#define MON_FREE_MEM  	(ADRSPC_K1BASE + 0x6000)


#endif /* ifndef __RSP_PCMAP_H__ */











