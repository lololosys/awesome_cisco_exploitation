/* $Id: c7100_pcmap.h,v 3.3.48.6 1996/08/20 00:10:31 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c7100/c7100_pcmap.h,v $
 *------------------------------------------------------------------
 * c7100_pcmap.h - Predator address space definition
 *
 * July 1995, Michael Beesley
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c7100_pcmap.h,v $
 * Revision 3.3.48.6  1996/08/20  00:10:31  mbeesley
 * CSCdi66438:  show version should display amount of packet sram
 * Branch: California_branch
 *
 * Revision 3.3.48.5  1996/07/18  17:58:00  mbeesley
 * CSCdi63193:  Need support for rev2 gt64010. Teach the monlib and
 * the IOS system images the incompatible differences between rev1 and
 * rev2 gt64010 chips :
 *         o Addressing flip on bytes accesses thru PCI IO space
 *         o Internal PCI config register addressing change
 * Branch: California_branch
 *
 * Revision 3.3.48.4  1996/05/24  04:11:24  mbeesley
 * CSCdi58493:  Tweak PCI settings for new arbiters
 * Branch: California_branch
 *
 * Revision 3.3.48.3  1996/04/23  19:21:53  mbeesley
 * CSCdi55360:  Enable cached, write thru view of packet memory
 * Branch: California_branch
 *
 * Revision 3.3.48.2  1996/04/23  17:13:59  gshen
 * CSCdi55375:  Need to have different address separation for IO and
 * memory based IO
 * Branch: California_branch
 *
 * Two new I/O address separation definitions:
 *
 * #define PCI_DEV_IO_MAPPED_IO_SEPARATION  0x0400 / * 1k * /
 * #define PCI_DEV_MEM_MAPPED_IO_SEPARATION 0x4000 / * 16k * /
 *
 * These changes have been code reviewed.
 *
 * Revision 3.3.48.1  1996/03/21  23:48:10  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.20  1996/03/09  01:06:52  mbeesley
 * CSCdi51152:  DMA ring in SRAM should swap entries, not CPU
 * Branch: ELC_branch
 *
 * Revision 3.2.2.19  1996/03/08  21:36:28  mbeesley
 * CSCdi51090:  Need to support rev 2 CPU card
 * Branch: ELC_branch
 *
 * Revision 3.2.2.18  1996/02/13  00:25:50  mbeesley
 * CSCdi48764:  Add address space for MB0 reset
 * Branch: ELC_branch
 *
 * Revision 3.2.2.17  1996/02/01  06:48:07  mbeesley
 * Branch: ELC_branch
 * Add macro to control particle based fastswitching. Also, set the
 * gt64010 into sync mode.
 *
 * Revision 3.2.2.16  1996/01/27  03:45:17  mbeesley
 * Branch: ELC_branch
 * Remove packet corruption. Set PCI timeout1 ot 0xff from the
 * default of 0x07.
 *
 * Revision 3.2.2.15  1996/01/26  22:10:38  mbeesley
 * Branch: ELC_branch
 *         o Debug GT64010 timers and add support for counters
 *         o Increase GT64010 cache line size
 *         o Finish support for fine granularity throttling
 *
 * Revision 3.2.2.14  1996/01/23  19:42:55  mbeesley
 * Branch: ELC_branch
 * Increase PCI timeout, allowing PA's to access packet DRAM while
 * the CPU is executing from ROM.
 *
 * Revision 3.2.2.13  1996/01/09  21:27:50  mbeesley
 * Branch: ELC_branch
 * Enhance/fix Midplane EEPROM utility. Protect against left over
 * interrupts from boot helper image. Fix some gt64010 register naming
 * mistakes.
 *
 * Revision 3.2.2.12  1996/01/04  03:19:00  wfried
 * Branch: ELC_branch
 * Move Bootflash to unmapped address space.
 *
 * Revision 3.2.2.11  1996/01/03  06:06:50  wfried
 * Branch: ELC_branch
 * Fix PCMCIA controller configuration.
 *
 * Revision 3.2.2.10  1995/12/19  19:50:10  wfried
 * Branch: ELC_branch
 * Fix PCI bridges configuration, MP interrupt registers masks and Galileo
 * PCI timing initialization value to Async.
 *
 * Revision 3.2.2.9  1995/12/12  04:50:48  mbeesley
 * Branch: ELC_branch
 * Add an enumeration for the IO card 1fe device number. Also fix some
 * DRAM PCI configuration in the GT64010.
 *
 * Revision 3.2.2.8  1995/12/11  19:23:06  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.2.2.7  1995/12/09  22:52:19  mbeesley
 * Branch: ELC_branch
 * Defeat OIR, ENV processes until hw is verified. Fix initial bugs:
 *         o NVRAM access must be 32 bit cycle, only using bits 7:0
 *         o Always create DRAM particle pool
 *         o Read 32 bytes, not 64 bytes from MP eeprom
 *         o Correct some misdefined hw addresses
 *         o Make packet SRAM usage on Rev 1 optional, with default as
 *           no until hw is verified (define C7100_CPU_SRAM to use it)
 *
 * Revision 3.2.2.6  1995/12/08  05:01:31  mbeesley
 * Branch: ELC_branch
 * Fix some addresses. Remove other unused addresses.
 *
 * Revision 3.2.2.5  1995/12/05  20:00:07  mbeesley
 * Branch: ELC_branch
 * Correct some register initialization. Add support for dynamic
 * parallel bridge config. Do not use a small piece of SRAM as
 * fast memory on Rev 2 cards.
 *
 * Revision 3.2.2.4  1995/12/01  02:10:51  mbeesley
 * Branch: ELC_branch
 * Add a bootflash base address and a bus error address.
 *
 * Revision 3.2.2.3  1995/11/30  02:14:36  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.2.2.2  1995/11/23  19:09:53  szhang
 * Branch: ELC_branch
 * Added KSEG1 address for Midplane FPGA.
 *
 * Revision 3.2.2.1  1995/11/22  21:43:23  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.3  1995/12/08  19:26:24  smackie
 * Move memory size defines into a central place. (CSCdi45214)
 *
 * Revision 3.2  1995/11/17  18:41:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:23:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __C7100_PCMAP_H__
#define __C7100_PCMAP_H__

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

/*
 * Address space allocations for IOS (kuseg)
 */

/*
 * NVRAM, TOD clock definitions
 */
#define ADRSPC_NVRAM		0x1E000000 /* Start of NV RAM */
#define NVRAM_MAGIC_OFFSET	0x06 	   /* Magic number NVRAM
					      config offset */
#define NVRAM_START_OFFSET	0x08 	   /* NVRAM start offset. This
					      MUST be LONG aligned */
#define ADRSPC_NVRAM_END	0x1E07FFFF /* NVRAM address space end (512k) */
#define ADRSPC_CALENDAR		0x1E000003 /* TOD clock register */

/*
 * Shared packet SRAM defintions
 */
#define ADRSPC_PCISRAM		0x4B000000 /* Start of packet SRAM */

#define PHY_ADRSPC_PCISRAM	0x4B000000 /* CPU physical address of SRAM */

#define PCISRAM_ADDRESS_MASK	0x40000000 /* Mask for seeing if data is in
					      PCI SRAM or not */

#define PCISRAM_SWAP_MASK	0x00800000 /* SRAM swaps data */
#define PCIDRAM_SWAP_MASK	0xC0000000 /* MB0 swaps DRAM data */

#define PCIMEM_CWT_MASK		0x30000000 /* cached write-thru view of mem */

/*
 * Shared packet DRAM definitions, cached write thru view
 */
#define ADRSPC_CACHED_IODRAM	PCIMEM_CWT_MASK

/*
 * Macro to convert address to cached, write-thru, no L2 cache
 */
#define CONVERT_ADDR_CWT(x)	(uchar *)(((ulong)(x) | PCIMEM_CWT_MASK))

/*
 * Boot ROM device
 */
#define ADRSPC_PROM		0x1FC00000 /* Boot ROM */

/*
 * Boot flash simm
 */
#define ADRSPC_BFLASH		0xBA000000 /* Boot flash */
#define ADRSPC_FLASH_BASE	ADRSPC_BFLASH

/*
 * PCMCIA socket support
 */
#define ADRSPC_PCMCIA_SLOT0    	0x40000000
#define ADRSPC_PCMCIA_ATTR0	(ADRSPC_PCMCIA_SLOT0)
#define ADRSPC_PCMCIA_WIN0_BASE	(ADRSPC_PCMCIA_SLOT0+ONE_MEG)

#define ADRSPC_PCMCIA_SLOT1    	0x44000000
#define ADRSPC_PCMCIA_ATTR1	(ADRSPC_PCMCIA_SLOT1)
#define ADRSPC_PCMCIA_WIN1_BASE	(ADRSPC_PCMCIA_SLOT1+ONE_MEG)

#define ADRSPC_PCMCIA_REGS     	0x10000000
#define PCMCIA_REGS_OFFSET     	0x00000400
#define PHY_ADRSPC_PCMCIA_REGS	0x00000000

#define ADRSPC_PCMCIA_INDEX_REG	(ADRSPC_PCMCIA_REGS+PCMCIA_REGS_OFFSET+0)
#define ADRSPC_PCMCIA_DATA_REG	(ADRSPC_PCMCIA_REGS+PCMCIA_REGS_OFFSET+1)

#define PCMCIA_REV2_GT64010_OFS	2

/*
 * CPU IO
 */
#define ADRSPC_CPUIO		0x1E800000 /* CPU IO space */

/*
 * I/O FPGA registers
 */
#define ADRSPC_IO_FPGA		0x1E840200 /* IO card registers */

/*
 * Midplane FPGA registers
 */
#define ADRSPC_MP_FPGA		0x1E800000 /* Midplane registers */

/*
 * Interrupt status bytes. They are either in pkt_int_stat_lsw or
 * pkt_int_stat_nsw, bytes 0 .. 3.
 */
#define ADRSPC_BAY0_ISTAT_REG	(ADRSPC_MP_FPGA + 16 + 3)	/* lsw, b0 */
#define ADRSPC_BAY1_ISTAT_REG	(ADRSPC_MP_FPGA + 16 + 2)	/* lsw, b1 */
#define ADRSPC_BAY2_ISTAT_REG	(ADRSPC_MP_FPGA + 24 + 2)	/* nsw, b1 */
#define ADRSPC_BAY3_ISTAT_REG	(ADRSPC_MP_FPGA + 16 + 0)	/* lsw, b3 */
#define ADRSPC_BAY4_ISTAT_REG	(ADRSPC_MP_FPGA + 16 + 1)	/* lsw, b2 */
#define ADRSPC_BAY5_ISTAT_REG	(ADRSPC_MP_FPGA + 24 + 0)	/* nsw, b3 */
#define ADRSPC_BAY6_ISTAT_REG	(ADRSPC_MP_FPGA + 24 + 1)	/* nsw, b2 */

/*
 * Bit bucket
 */
#define ADRSPC_BITBUCKET	0x1F000000 /* CPU bit bucket */

/*
 * MB0 Hardware Reset
 */
#define ADRSPC_MB0_RESET	0xBF800000 /* Register to reset MB0 */

/*
 * PA IO
 */
#define ADRSPC_BAY0		0x3C000000 /* Bay 0 IO, IO card */
#define ADRSPC_BAY1		0x3C100000 /* Bay 1 */
#define ADRSPC_BAY2		0x3C200000 /* Bay 2 */
#define ADRSPC_BAY3		0x3C300000 /* Bay 3 */
#define ADRSPC_BAY4		0x3C400000 /* Bay 4 */
#define ADRSPC_BAY5		0x3C500000 /* Bay 5 */
#define ADRSPC_BAY6		0x3C600000 /* Bay 6 */

/*
 * Assuming memory mapped IO
 */
#define PHY_ADRSPC_BAY0		0x48000000 /* Bay 0 IO, IO card */
#define PHY_ADRSPC_BAY1		0x48800000 /* Bay 1 */
#define PHY_ADRSPC_BAY2		0x4D000000 /* Bay 2 */
#define PHY_ADRSPC_BAY3		0x49000000 /* Bay 3 */
#define PHY_ADRSPC_BAY4		0x4D800000 /* Bay 4 */
#define PHY_ADRSPC_BAY5		0x49800000 /* Bay 5 */
#define PHY_ADRSPC_BAY6		0x4E000000 /* Bay 6 */

#define PCI_IO_BIT32		0x0

/*
 * GT64010 system controller
 */
#define ADRSPC_GT64010		0xB4000000 /* Gallileo system controller */

/*
 * Level 2 Cache control definitions
 */
#define LVL2_CACHE_TAGLINES_10	0x8000	   /* Tag[Hi.Lo] = 10 shifted right
					      by 20 bits (see asm_4k.S) */

/*
 * Define PCI IO memory utilization
 */
#define PCI_BAY_SEPARATION               0x00100000 /* 1 mbyte per PA bay */
#define PCI_DEV_MEM_MAPPED_IO_SEPARATION 0x4000     /* 16 KB for each device */

/*
 * Macro to bitflip return values from the monitor
 */
#define ACTIVE_LOW(x)		(~(x))

/*
 * Reset reasons
 */
#define REASON_PERIPH		0x08
#define REASON_SWNMI		0x04
#define REASON_PBNMI		0x02
#define REASON_WATCHDOG		0x01
#define REASON_ALL		(REASON_PERIPH | REASON_SWNMI | \
				 REASON_PBNMI | REASON_WATCHDOG)

/*
 * Defintions for the GT64010 initialization. Check these !!
 */
#define GT64010_PCI_IO_LOW_INIT		0x00080000	/* 0x1,0000,0000 */
#define GT64010_PCI_IO_HIGH_INIT	0x00000000	/* 0x1,0000,ffff */
#define GT64010_PCI_MEM_LOW_INIT	0x00020000	/* 0x0,4000,0000 */
#define GT64010_PCI_MEM_HIGH_INIT	0x7F000000	/* 0x0,4fff,ffff */

#define GT64010_PCI_RAS10_INIT		0xFFFF7F00	/* 8 meg */
#define GT64010_PCI_RAS32_INIT		0xFFFF7F00	/* 8 meg */

#define GT64010_PCI_CS20_INIT		0x00000000	/* Unused space */
#define GT64010_PCI_CS3_BOOT_INIT	0x00000000	/* Unused space */

#define GT64010_PCI_CMD_INIT		0x05000000	/* Swap, Sync TCLk and
							   PCI clock */

#define GT64010_PCI_TIMEOUT_INIT_REV1   0x28ff0000	/* Timeouts, Retries */
#define GT64010_PCI_TIMEOUT_INIT        0xffff0000	/* Timeouts, Retries */

#define GT64010_STAT_CMD_INIT		0x1F010000	/* All errors on */

#define GT64010_CACHE_LINE		0xFF000000	/* size = infinite */

/*
 * Defines for the Monitor
 * R4000/Orion Address translation macros
 */
#define PHY_TO_KSEG1(x)    ((x)+0xA0000000)
#define PHY_TO_KSEG0(x)    ((x)+0x80000000)
#define PHY_ADDR(x)        ((x)&0x1FFFFFFF)
#define ROM_OFFSET(x)      ((x)&0x000FFFFF)
#define KSEG1_TO_KSEG0(x)  ((x)-0x20000000)

/*
 * Stack address definition for the ROM Monitor
 */
#define MON_INIT_SP   	(ADRSPC_K1BASE + 0x6000)
#define MON_FREE_MEM  	(ADRSPC_K1BASE + 0x6000)

/*
 * Address space allocations.
 * These are physical addresses.
 */
#define PHY_ADRSPC_RAM		0x00000000 /* Start of RAM */
#define PHY_ADRSPC_RAM_END      0x08000000 /* End of RAM area (up to 256 MB) */

#define PHY_ADRSPC_BFLASH       0x1A000000 /* Boot Flash EPROM start address */
#define PHY_ADRSPC_FLASH_BASE	PHY_ADRSPC_BFLASH

#define PHY_ADRSPC_PROM		0x1FC00000 /* Start of EPROM */
#define PHY_ADRSPC_PROM_END	0x20000000 /* End of EPROM area (up to 16MB) */

#define PHY_ADRSPC_NVRAM	0x1E000000 /* Start of NVRAM */
#define PHY_ADRSPC_NVRAM_END	0x1E800000 /* End of NVRAM area (8 MB) */

#define PHY_ADRSPC_IO_REGS      0x1E800000 /* i/o asic base address */

#define PHY_ADRSPC_DUART        0x1E840400 /* 2681 DUART            */

#define PHY_ADRSPC_BAD_ADDR     0x1DFF0000 /* address that will generate a 
                                              bus error */

#define PHY_ADRSPC_PRED_FPGA    0x1E840200 /* Predator FPGA register address */

#define PHY_ADRSPC_GT64010_INTERNAL_SPC_DECODE_INIT    0x14000068

#define PHY_ADRSPC_GT64010_BASE 0x14000000

#define PHY_ADRSPC_MP_FPGA	0x1E800000 /* Midplane registers */

/*
 * Virtual address space allocations - Processor in Kernel Mode (kseg1)
 * These are virtual addresses for Kernel Segment 1.
 */
#define KSEG1_ADRSPC_RAM           PHY_TO_KSEG1(PHY_ADRSPC_RAM)
#define KSEG1_ADRSPC_RAM_END       PHY_TO_KSEG1(PHY_ADRSPC_RAM_END)

#define KSEG1_ADRSPC_BFLASH        PHY_TO_KSEG1(PHY_ADRSPC_BFLASH)
#define KSEG1_ADRSPC_BFLASH_BASE   PHY_TO_KSEG1(PHY_ADRSPC_BFLASH)
#define KSEG1_ADRSPC_FLASH_BASE	   PHY_TO_KSEG1(PHY_ADRSPC_FLASH_BASE)
#define KSEG1_ADRSPC_BFLASH_END    PHY_TO_KSEG1(PHY_ADRSPC_BFLASH_END)

#define KSEG1_ADRSPC_PROM	   PHY_TO_KSEG1(PHY_ADRSPC_PROM)
#define KSEG1_ADRSPC_PROM_END	   PHY_TO_KSEG1(PHY_ADRSPC_PROM_END)
#define KSEG1_ADRSPC_BAD_ADDR      PHY_TO_KSEG1(PHY_ADRSPC_BAD_ADDR)

#define KSEG1_ADRSPC_NVRAM	   PHY_TO_KSEG1(PHY_ADRSPC_NVRAM)
#define KSEG1_ADRSPC_NVRAM_END     PHY_TO_KSEG1(PHY_ADRSPC_NVRAM_END)

#define KSEG1_ADRSPC_IO_REGS	   PHY_TO_KSEG1(PHY_ADRSPC_IO_REGS)

#define KSEG1_ADRSPC_DUART         PHY_TO_KSEG1(PHY_ADRSPC_DUART)

#define KSEG1_ADRSPC_PRED_FPGA     PHY_TO_KSEG1(PHY_ADRSPC_PRED_FPGA)

#define KSEG1_ADRSPC_GT64010_INTERNAL_SPC_DECODE_INIT  \
    PHY_TO_KSEG1(PHY_ADRSPC_GT64010_INTERNAL_SPC_DECODE_INIT)

#define KSEG1_ADRSPC_GT64010_BASE  PHY_TO_KSEG1(PHY_ADRSPC_GT64010_BASE)

#define KSEG1_ADRSPC_MP_FPGA	   PHY_TO_KSEG1(PHY_ADRSPC_MP_FPGA)

#define MON_TIMER_VAL_20MS         (0x30d40 * 5)  /* 20ms tick */

#define RESET_VEC_OFFSET           0xC00000   /* vector offset to eprom base */

#endif /* !__C7100_PCMAP_H__ */

