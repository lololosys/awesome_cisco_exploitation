/* $Id: rvip_pcmap.h,v 3.5.48.4 1996/04/23 17:14:07 gshen Exp $
 * $Source: /release/112/cvs/Xsys/src-rvip/rvip_pcmap.h,v $
 *------------------------------------------------------------------
 * rvip_pcmap.h -- Checkers Address space definition
 *
 * January 1995, Michael Beesley
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: rvip_pcmap.h,v $
 * Revision 3.5.48.4  1996/04/23  17:14:07  gshen
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
 * Revision 3.5.48.3  1996/04/09  23:11:24  dlobete
 * CSCdi52358:  VIP1 image fails to boot on a 32MB VIP1 board
 * Branch: California_branch
 *
 * Revision 3.5.48.2  1996/03/29  00:45:42  sjacobso
 * CSCdi45152:  PERFORMANCE: 1FE runs in promiscuous mode instead of
 * filtering
 * Branch: California_branch
 * Add code to allow dec21140 chip to run in perfect
 * address filtering mode whenever possible.  Modify
 * address filter write code to record information
 * about address filter entries that may affect perfect
 * filter operation.  Interface VIP-specific software
 * address filter to RVIP/SVIP/Predator common routines.
 *
 * Revision 3.5.48.1  1996/03/21  23:50:56  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.4  1996/03/07  17:29:27  getchell
 * CSCdi50953:  viper dfs mods
 * Branch: ELC_branch
 *
 * Revision 3.2.2.3  1995/12/11  19:24:49  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.2.2.2  1995/12/04  22:18:55  mbeesley
 * Branch: ELC_branch
 * Perform mini-sync - particle refcount changes.
 *
 * Revision 3.2.2.1  1995/11/22  21:43:56  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.5  1995/12/08  19:26:29  smackie
 * Move memory size defines into a central place. (CSCdi45214)
 *
 * Revision 3.4  1995/12/06  20:48:24  hwang
 * CSCdi45104:  pci io base address for bay 0 need to skip first 8K space
 *
 * Revision 3.3  1995/12/01  02:00:17  mbeesley
 * CSCdi44698:  PCI subsystem needs improvement
 *
 * Revision 3.2  1995/11/17  18:50:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:16  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:59:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __RVIP_PCMAP_H__
#define __RVIP_PCMAP_H__

/*
 * FIXME : These should be moved into an r4k header file
 * R4K Address space definitions.
 */
#define ADRSPC_K0BASE    	(0x80000000)
#define ADRSPC_K0SIZE    	(0x20000000)
#define ADRSPC_K1BASE    	(0xA0000000)
#define ADRSPC_K1SIZE    	(0x20000000)
#define ADRSPC_KSBASE    	(0xC0000000)
#define ADRSPC_KSSIZE    	(0x20000000)
#define ADRSPC_K3BASE    	(0xE0000000)
#define ADRSPC_K3SIZE    	(0x20000000)
#define ADRSPC_KUBASE    	(0x00000000)
#define ADRSPC_KUSIZE    	(0x80000000)
#define KSEG_MSB_ADDR    	0xFFFFFFFF

/*
 * Virtual Address Space definitions (kuseg)
 */
#define ADRSPC_SRAM_HEAP	0x20100000	/* start of SRAM heap */
#define ADRSPC_SRAM_END		0x20200000	/* end of heap SRAM */
#define ADRSPC_LOW_SRAM		0x20000000	/* start of program SRAM */
#define ADRSPC_RAM		0x64000000	/* code, data DRAM */
#define ADRSPC_CBUS		0x30000000	/* memd address range */
#define ADRSPC_PMAC		0x32000000	/* pmac PCI controller */
#define ADRSPC_PCI_IO		0x34000000	/* PCI IO space */
#define ADRSPC_PCI_TYPE1	0x34040000	/* PCI Type 1 Access */
#define ADRSPC_PCI_IDSEL0	0x34080000	/* PCI Type 0, ID sel 0 */
#define ADRSPC_PCI_IDSEL1	0x340C0000	/* PCI Type 0, ID sel 1 */
#define ADRSPC_NEV_ASIC		0x3E800000	/* Nevada ASIC */
#define ADRSPC_DBUS		0x3E900000	/* DBus registers */
#define ADRSPC_DMA		0x3EA00000	/* DMA engine registers */
#define ADRSPC_DRAM_WBERR	0x3EC00000 	/* Reno DRAM write exception
						   address register */
#define ADRSPC_SHMEM_WBERR	0x3ED00000 	/* Reno shared mem write 
						   exception addr register */
#define ADRSPC_IO_WBERR		0x3EE00000 	/* Reno IO write exception
						   address register */
#define ADRSPC_PROM		0x3F000000	/* rom monitor EPROM */

/*
 * IO Mapped IO for each PA bay
 */
#define ADRSPC_PCIBAY_0		0x3C102000	/* PCI PA Bay 0 */
#define ADRSPC_PCIBAY_1		0x3C00A000	/* PCI PA Bay 1 */

/*
 * Memory Mapped IO for each bay
 */
#define ADRSPC_PCIBAY_0_MM	0x3C800000	/* PCI PA Bay 0 */
#define ADRSPC_PCIBAY_1_MM	0x3C900000	/* PCI PA Bay 1 */

/*
 * PCI Memory definitions
 */
#define PCIMEM_VIRT_BASE_ADDR	0x04600000	/* PCI mem base virt address */
#define PCIMEM_UNC_OFS		0x00000000	/* Offset to uncached view */
#define PCIMEM_CWT_OFS		0x08000000	/* Offset to cwt view */
#define PCIMEM_CWB_OFS		0x18000000	/* Offset to cwb view */
#define PCIMEM_CACHE_MASK       0xE7FFFFFF      /* Cached bits 'and' mask */
#define PCIMEM_L_E_OFS		0x80000000	/* PCI little endian byte 
						   swap address base */
/*
 * PCI memory, uncached view of memory
 */
#define ADRSPC_PCIMEM		(PCIMEM_VIRT_BASE_ADDR+PCIMEM_UNC_OFS)

/*
 * PCI memory, cached write  through view of memory
 */
#define ADRSPC_PCIMEM_CWT       (PCIMEM_VIRT_BASE_ADDR+PCIMEM_CWT_OFS)

#define CONVERT_PARTICLE_ADDR(x) (x) = (uchar *)((ulong)(x) | PCIMEM_CWT_OFS)

/*
 * Level 2 Cache definitions
 */
#define LVL2_CACHE_TAGLINES_10	0x8000	   	/* TagHigh.Low = 10
						   shifted right 20 bits */

/*
 * FIXME: These should be moved into a general r4k header file
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
 * Physical address space definitions
 */
#define PHY_ADRSPC_PCI_TYPE1	0x09200000	/* PCI Type 1 access */
#define PHY_ADRSPC_PCI_IDSEL1	0x09100000	/* PCI Type 0, ID Sel 0 */
#define PHY_ADRSPC_PCI_IDSEL0	0x09110000	/* PCI Type 0, ID Sel 1 */
#define PHY_ADRSPC_PMAC		0x09000000	/* PMAC controller */
#define PHY_ADRSPC_PCI_IO	0x09400000	/* PCI IO space */
#define PHY_ADRSPC_PCIBAY_0	0x09402000	/* PCI PA Bay 0 */
#define PHY_ADRSPC_PCIBAY_1	0x0940A000	/* PCI PA Bay 1 */

#define PA_PCI_IO_LIMIT         0x00007fff      /* IO Limit per PA */

#define PHY_ADRSPC_LOW_SRAM	0x00000000	/* SRAM heap */
#define PHY_ADRSPC_RAM		0x04000000	/* code, data memory */
#define PHY_ADRSPC_CBUS		0x08000000	/* MEMD / CBUS */
#define PHY_ADRSPC_NEV_ASIC	0x1C000000	/* Nevada ASIC */
#define PHY_ADRSPC_DBUS		0x1C800000	/* DBus registers */
#define PHY_ADRSPC_DRAM_WBERR	0x1EC00000	/* DRAM Write bus error */
#define PHY_ADRSPC_SHMEM_WBERR	0x1ED00000	/* Sh. Mem. Write bus error */
#define PHY_ADRSPC_IO_WBERR	0x1EE00000	/* IO Write bus error */
#define PHY_ADRSPC_PROM		0x1FC00000	/* Monitor EPROM */

#define PHY_ADRSPC_PCIMEM	(PHY_ADRSPC_RAM + RVIP_PCIMEM_OFFSET)

#define PHY_ADRSPC_MMIO_BASE	0x09800000	/* Base of PCI MM IO */

/* 
 * DMA register address space
 */
#define PHY_ADRSPC_DMA		0x1E810000	/* DMA engine registers */

/*
 * Addresses as seen on the PCI bus by devices
 */
#define PCI_MEMMAPPED_IO_BAY0	0x00100000	/* PCI mem/mapped IO, bay 0 */
#define PCI_MEMMAPPED_IO_BAY1	0x00500000	/* PCI mem/mapped IO, bay 1 */

/*
 * Physical addresses of memory mapped IO from CPU perspective
 */
#define PHY_ADRSPC_MM_BAY0	(PHY_ADRSPC_MMIO_BASE + PCI_MEMMAPPED_IO_BAY0)
#define PHY_ADRSPC_MM_BAY1	(PHY_ADRSPC_MMIO_BASE + PCI_MEMMAPPED_IO_BAY1)

/*
 * Virtual address space allocations - Processor in Kernel Mode (kseg1)
 * These are virtual addresses for Kernel Segment 1.
 */
#define MON_TIMER_VAL_20MS  	(0x30d40 * 5)  /* 20ms tick */

/*
 * Port separation
 */
#define PCI_BAY_SEPARATION               0x00100000 /* 1 mbyte per PA bay */
#define PCI_DEV_IO_MAPPED_IO_SEPARATION  0x400      /* 1 KB for each device */
#define PCI_DEV_MEM_MAPPED_IO_SEPARATION 0x4000     /* 16 KB for each device */
#define PCI_PA_DEVICE_BASE_OFFSET        0          /* check with manoj */

#endif /* __RVIP_PCMAP_H__ */
