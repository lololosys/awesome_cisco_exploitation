/* $Id: pcmap.h,v 3.4 1996/03/11 21:51:30 nrvankay Exp $
 * $Source: /release/111/cvs/Xsys/src-4k-c4000/pcmap.h,v $
 *------------------------------------------------------------------
 * pcmap.h -- Sierra system image memory mapping and register defines
 *
 * October 1993, Lele Nardin
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: pcmap.h,v $
 * Revision 3.4  1996/03/11  21:51:30  nrvankay
 * CSCdi51276:  Reserve NIM ID for Fast Ethernet NIM
 * Reserve NIM ID for Fast Ethernet
 *
 * Revision 3.3  1995/12/08  19:26:21  smackie
 * Move memory size defines into a central place. (CSCdi45214)
 *
 * Revision 3.2  1995/11/17  18:40:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:22:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:26:56  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/09/01  23:12:35  pilee
 * CSCdi39557:  Additional NIM ID is required.
 *
 * Revision 2.2  1995/06/17  01:00:46  clev
 * CSCdi35421:  Add cache tests to ROM monitor core board diagnostics
 *
 * Revision 2.1  1995/06/07  22:53:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __PCMAP_H__
#define __PCMAP_H__

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
 * Address space allocations (kuseg)
 */
#define ADRSPC_FLASH_BASE	0x30000000 /* Flash EPROM start address */
#define ADRSPC_FLASH		0x30000000
#define ADRSPC_BFLASH_BASE	0x38000000 /* Boot Flash base address */
#define ADRSPC_BFLASH		0x38000000

#define ADRSPC_SLOT_1           0x3C000000 /* Interface card slot 1 */
#define ADRSPC_SLOT_2           0x3C100000 /* Interface card slot 2 */
#define ADRSPC_SLOT_3           0x3C200000 /* Interface card slot 3 */
#define ADRSPC_IACK1_BASE	0x3D000000 /* SLOT 1 Intr ack register base */
#define ADRSPC_IACK2_BASE	0x3D100000 /* SLOT 2 Intr ack register base */
#define ADRSPC_IACK3_BASE	0x3D200000 /* SLOT 3 Intr ack register base */

#define IACK_REG(addr)		(addr+0x5) /* access iack reg on lower 
					      32 bits of long word */

#define ADRSPC_IACK_1		0x3D000005 /* IACK register for slot 1 */
#define ADRSPC_IACK_2		0x3D100005 /* IACK register for slot 2 */
#define ADRSPC_IACK_3		0x3D200005 /* IACK register for slot 3 */

#define ADRSPC_NVRAM		0x3E000000 /* Start of NV RAM */
#define NVRAM_MAGIC_OFFSET	0x06 	   /* Magic number NVRAM
					      config offset*/
#define NVRAM_START_OFFSET	0x08 	   /* NVRAM start offset. This
					      MUST be LONG aligned */
#define ADRSPC_NVRAM_END	0x3E07FFFF /* NVRAM address space end (512k)*/

#define ADRSPC_CALENDAR		0x3E000000 /* TOD clock register */

#define ADRSPC_CPUIO		0x3E800000 /* Base of CPU IO resources */

#define ADRSPC_NEV_ASIC		0x3E800000 /* Start of Nevada address space */
#define ADRSPC_SYS_CTRL 	0x3E800200 /* Nevada control register A */
#define ADRSPC_SHMEM_CTRL       0x3E800208 /* Shared memory control register */
#define ADRSPC_MARG_MBOX_STAT	0x3E800210 /* Chipcom mailbox status register */
#define X2444_PORT 		*(uchar *)0x3E800400 /* cookie data register */
#define ADRSPC_MARG_MBOX_DATA	0x3E810000 /* Chipcom mailbox data register */
#define ADRSPC_DRAM_WBERR	0x3EC00000 /* Reno DRAM write exception
					      address register */
#define ADRSPC_SHMEM_WBERR	0x3ED00000 /* Reno shared mem write eception
					      address register */
#define ADRSPC_IO_WBERR		0x3EE00000 /* Reno IO write exception
					      address register */

#define ADRSPC_PROM	        0x3f000000 /* Start of ROM monitor ROM */
#define ADRSPC_PROM_END	        0x3f080000 /* End of ROM monitor ROM */

#define ADRSPC_SHAREDMEM        0x40000000 /* start of shared memory 
					      (not bit swapped) */
#define ADRSPC_SHMEM_SW		0x50000000 /* start of shared memory
					      (bit swapped by RENO) */

#define ADRSPC_RAM		0x60000000 /* Start of RAM */
#define ADRSPC_RAM_END          0x80000000 /* End of RAM */

/*
 * Level 2 Cache control definitions
 */
#define LVL2_CACHE_TAGLINES_10	0x8000	   /* Tag[Hi.Lo] = 10 shifted right
					      by 20 bits (see asm_4k.S) */

/*
** Defines for the Monitor
** R4000/Orion Address translation macros
*/
#define PHY_TO_KSEG1(x)    ((x)+0xA0000000)
#define PHY_TO_KSEG0(x)    ((x)+0x80000000)
#define PHY_ADDR(x)        ((x)&0x1FFFFFFF)
#define ROM_OFFSET(x)      ((x)&0x000FFFFF)
#define KSEG1_TO_KSEG0(x)  ((x)-0x20000000)
#define KSEG0_TO_PHY(x)    ((x)-0x80000000)
#define KSEG0_TO_KSEG1(x)  ((x)+0x20000000)

/*
 * Level 2 Cache definitions
 */
#define LVL2_CACHE_TAGLINES_10	0x8000	   	/* TagHigh.Low = 10
						   shifted right 20 bits */

/*
** Stack address definition for the ROM Monitor
*/

#define MON_INIT_SP   	(ADRSPC_K1BASE + 0x6000)
#define MON_FREE_MEM  	(ADRSPC_K1BASE + 0x6000)

/*
** Address space allocations.
** These are physical addresses.
*/
#define PHY_ADRSPC_RAM		0x00000000 /* Start of RAM */
#define PHY_ADRSPC_RAM_END      0x02000000 /* End of RAM area (up to 32 MB) */
#define PHY_ADRSPC_PROM		0x1FC00000 /* Start of EPROM */
#define PHY_ADRSPC_PROM_END	0x20000000 /* End of EPROM area (up to 4 MB)*/
#define PHY_ADRSPC_NVRAM	0x1E000000 /* Start of NV RAM */
#define PHY_ADRSPC_NVRAM_END	0x1E800000 /* End of NVRAM area (8 MB) */
#define PHY_ADRSPC_FLASH    	0x10000000 /* Flash EPROM start address */
#define PHY_ADRSPC_FLASH_BASE	0x10000000 /* Flash EPROM start address */
#define PHY_ADRSPC_FLASH_END	0x18000000 /* End of flash EPROM area (up to
					      128 MB) */
#define PHY_ADRSPC_BFLASH       0x18000000 /* Boot Flash EPROM start address */
#define PHY_ADRSPC_BFLASH_END   0x1C000000 /* End of Boot Flash EPROM area (up
					      to 64 MB) */
#define PHY_ADRSPC_SHAREDMEM    0x08000000 /* Start of shared memory */
#define PHY_ADRSPC_SHMEM_END    0x10000000 /* End of shared memory area (up to
					      128 MB) */
#define PHY_ADRSPC_SLOT_1       0x1C000000 /* Interface card slot 1 */
#define PHY_ADRSPC_SLOT_2       0x1C100000 /* Interface card slot 2 */
#define PHY_ADRSPC_SLOT_3       0x1C200000 /* Interface card slot 3 */
#define PHY_ADRSPC_IACK_1	0x1D000004 /* IACK register, slot 1 */
#define PHY_ADRSPC_IACK_2	0x1D100004 /* IACK register, slot 2 */
#define PHY_ADRSPC_IACK_3	0x1D200004 /* IACK register, slot 3 */

#define PHY_ADRSPC_DRAM_WBERR	0x1EC00000 /* DRAM RENO wberr register */
#define PHY_ADRSPC_SHMEM_WBERR	0x1ED00000 /* Sh. Memory RENO wberr register */
#define PHY_ADRSPC_IO_WBERR	0x1EE00000 /* IO RENO wberr register */

#define PHY_ADRSPC_NEV_ASIC     0x1E800000 /* i/o asic base address */

#define PHY_ADRSPC_BAD_ADDR     0x1DFF0000 /* address that will generate a 
                                              bus error */
#define PHY_ADRSPC_DUART        0x1E820800 /* 2681 DUART            */

/*
 * Virtual address space allocations - Processor in Kernel Mode (kseg1)
 * These are virtual addresses for Kernel Segment 1.
 */

#define KSEG1_ADRSPC_RAM           PHY_TO_KSEG1(PHY_ADRSPC_RAM)
#define KSEG1_ADRSPC_RAM_END       PHY_TO_KSEG1(PHY_ADRSPC_RAM_END)
#define KSEG1_ADRSPC_PROM	   PHY_TO_KSEG1(PHY_ADRSPC_PROM)
#define KSEG1_ADRSPC_PROM_END	   PHY_TO_KSEG1(PHY_ADRSPC_PROM_END)
#define KSEG1_ADRSPC_NVRAM	   PHY_TO_KSEG1(PHY_ADRSPC_NVRAM)
#define KSEG1_ADRSPC_NVRAM_END     PHY_TO_KSEG1(PHY_ADRSPC_NVRAM_END)
#define KSEG1_ADRSPC_FLASH         PHY_TO_KSEG1(PHY_ADRSPC_FLASH)
#define KSEG1_ADRSPC_FLASH_BASE    PHY_TO_KSEG1(PHY_ADRSPC_FLASH_BASE)
#define KSEG1_ADRSPC_FLASH_END     PHY_TO_KSEG1(PHY_ADRSPC_FLASH_END)
#define KSEG1_ADRSPC_BFLASH        PHY_TO_KSEG1(PHY_ADRSPC_BFLASH)
#define KSEG1_ADRSPC_BFLASH_BASE   PHY_TO_KSEG1(PHY_ADRSPC_BFLASH)
#define KSEG1_ADRSPC_BFLASH_END    PHY_TO_KSEG1(PHY_ADRSPC_BFLASH_END)

#define KSEG1_ADRSPC_SHAREDMEM     PHY_TO_KSEG1(PHY_ADRSPC_SHAREDMEM)
#define KSEG1_ADRSPC_SHMEM_END     PHY_TO_KSEG1(PHY_ADRSPC_SHMEM_END)
#define KSEG1_ADRSPC_BAD_ADDR      PHY_TO_KSEG1(PHY_ADRSPC_BAD_ADDR)

#define KSEG1_ADRSPC_SLOT_1        PHY_TO_KSEG1(PHY_ADRSPC_SLOT_1)
#define KSEG1_ADRSPC_SLOT_2        PHY_TO_KSEG1(PHY_ADRSPC_SLOT_2)
#define KSEG1_ADRSPC_SLOT_3        PHY_TO_KSEG1(PHY_ADRSPC_SLOT_3)

#define KSEG1_ADRSPC_IACK_1	   PHY_TO_KSEG1(PHY_ADRSPC_IACK_1)
#define KSEG1_ADRSPC_IACK_2	   PHY_TO_KSEG1(PHY_ADRSPC_IACK_2)
#define KSEG1_ADRSPC_IACK_3	   PHY_TO_KSEG1(PHY_ADRSPC_IACK_3)

#define KSEG1_ADRSPC_DRAM_WBERR	   PHY_TO_KSEG1(PHY_ADRSPC_DRAM_WBERR)
#define KSEG1_ADRSPC_SHMEM_WBERR   PHY_TO_KSEG1(PHY_ADRSPC_SHMEM_WBERR)
#define KSEG1_ADRSPC_IO_WBERR	   PHY_TO_KSEG1(PHY_ADRSPC_IO_WBERR)

#define KSEG1_ADRSPC_NEV_ASIC      PHY_TO_KSEG1(PHY_ADRSPC_NEV_ASIC)
#define KSEG1_ADRSPC_DUART         PHY_TO_KSEG1(PHY_ADRSPC_DUART)

#define MON_TIMER_VAL_20MS         (0x30d40 * 5)  /* 20ms tick */

/*
 * Define some slot separation literals
 */
#define SIERRA_SLOT_SEPARATION	0x00100000
#define SIERRA_IACK_SEPARATION  0x00100000

/*
 * Definition of ID Prom on DosXX
#ifdef TBD
 * Keep these here for now. They should go to a common file h/defs_c4000.h
#endif
 */

#define XX_FDDI_SINGLE   0x00       /* Single Attach, no bridge hardware */
#define XX_FDDI_DUAL     0x01       /* Dual Attach, no bridge hardware */
#define XX_FDDI_SBRIDGE  0x02       /* Single Attach, bridge hardware */
#define XX_FDDI_DBRIDGE  0x03       /* Dual Attach, bridge hardware */
#define XX_ETHER         0x04       /* Ethernet, Lance Type */
#define XX_TOKEN_RING    0x05       /* Token Ring */
#define XX_SERIAL_SINGLE 0x06       /* Single Serial I/O */
#define XX_SERIAL_DUAL   0x07       /* Dual Serial I/O */
#define XX_ETHER_SINGLE  0x08       /* Dual Ethernet, Single Port, Lance */
#define XX_ETHER_DUAL    0x09       /* Dual Ethernet, Dual Port, Lance */
#define XX_TOKEN_SINGLE  0x0A       /* Dual Token Ring, Single Port, C16 */
#define XX_TOKEN_DUAL    0x0B       /* Dual Token Ring, Dual Port, C16 */
#define XX_SERIAL_QUAD   0x0C       /* Quad Serial, Hatachi chip */
#define XX_MCN           0x0D       /* Multi Channel Serial Nim, Munich32 */
#define XX_PCI           0x0E       /* PCI NIM */
#define XX_ATM_BX50      0x0F       /* ATMizer BX-50 */
#define XX_ATM_RES1      0x10       /* ATM NIM reserved ID */
#define XX_ATM_RES2      0x11       /* ATM NIM reserved ID */
#define XX_ATM_RES3      0x12       /* ATM NIM reserved ID */
#define XX_SERIAL_2T16S  0x13       /* Serial I/O for 2T(Hitachi 64570), 16S
                                       (Cirrus Logic CD2402). */
#define XX_FAST_ETHER    0x14       /* FastEthernet NIM ID */

#endif /* !__PCMAP_H__ */
