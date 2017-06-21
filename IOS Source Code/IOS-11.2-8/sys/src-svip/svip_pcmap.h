/* $Id: svip_pcmap.h,v 3.1.62.4 1996/07/08 22:54:09 mwu Exp $
 * $Source: /release/112/cvs/Xsys/src-svip/svip_pcmap.h,v $
 *------------------------------------------------------------------
 * svip_pcmap.h -- VIPer II Address space definition
 *
 * September 1995, Steve Jacobson
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: svip_pcmap.h,v $
 * Revision 3.1.62.4  1996/07/08  22:54:09  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.1.62.3  1996/04/23  17:14:13  gshen
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
 * Revision 3.1.62.2  1996/03/29  00:45:47  sjacobso
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
 * Revision 3.1.62.1  1996/03/21  23:51:37  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.10.2  1996/03/07  17:29:49  getchell
 * CSCdi50953:  viper dfs mods
 * Branch: ELC_branch
 *
 * Revision 3.1.10.1  1996/02/10  00:22:11  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/12/26  21:18:45  getchell
 * Placeholder files for Viper development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SVIP_PCMAP_H__
#define __SVIP_PCMAP_H__

/*
 * FIXME : These should be moved into an r4k header file
 * R4K Address space definitions.
 */
#define ADRSPC_K0BASE    	(0x80000000)   /* unmapped cached */
#define ADRSPC_K0SIZE    	(0x20000000)
#define ADRSPC_K1BASE    	(0xA0000000)   /* unmapped uncached */
#define ADRSPC_K1SIZE    	(0x20000000)
#define ADRSPC_KSBASE    	(0xC0000000)   /* mapped */
#define ADRSPC_KSSIZE    	(0x20000000)
#define ADRSPC_K3BASE    	(0xE0000000)   /* mapped */
#define ADRSPC_K3SIZE    	(0x20000000)   
#define ADRSPC_KUBASE    	(0x00000000)   /* mapped */
#define ADRSPC_KUSIZE    	(0x80000000)
#define KSEG_MSB_ADDR    	 0xFFFFFFFF


/* 
 * Virtual address space
 0x2000 0000 - 0x2020 0000       SRAM
 0x3000 0000 - 0x301f 0000       PCIMEM_UNC (2MB)
 0x3100 0000 - 0x311f 0000       PCIMEM_WT (2MB)
 0x4000 0000 - 0x4007 ffff       PMA ASIC (512KB)
 0x4800 0000 - 0x487f ffff       CBUS/MEMD (8 MB)
 0x4C00 2000 - 0x4C00 3FFF       PCI Bay 0 - IO mapped (8KB)
 0x4C00 A000 - 0x4C00 BFFF       PCI Bay 1 - IO mapped (8KB)
 0x4C90 0000 - 0x4C97 FFFF       PCI Bay 0 - Mem mapped (512KB)
 0x4CA0 0000 - 0x4CA7 FFFF       PCI Bay 1 - Mem mapped (512KB)
 0x4E80 0000 - 0x4E80 1FFF       Nevada ASIC (8KB)
 0x4E90 0000 - 0x4E91 FFFF       CYA ASIC (128KB)
 0x4EC0 0000 - 0x4EC0 1FFF       DRAM_WBERR (8KB)
 0x4ED0 0000 - 0x4ED0 1FFF       IO_WBERR (8KB)
 0x5000 0000                     Reserved for PCI memory - bit swapped
 0x6000 0000                     DRAM
*/

/*
 * Virtual Address Space definitions (kuseg)
 */
#define ADRSPC_RAM		0x60000000	/* code, data DRAM */
#define ADRSPC_PMA_ASIC		0x40000000	/* PMA ASIC  */
#define ADRSPC_CBUS		0x48000000	/* memd address range */

#define ADRSPC_PCIBAY_0		0x4C002000	/* PCI PA Bay 0 */
#define ADRSPC_PCIBAY_1		0x4C00A000	/* PCI PA Bay 1  */

/*
 * Memory Mapped IO for each bay
 */
#define ADRSPC_PCIBAY_0_MM	0x4C900000	/* PCI PA Bay 0 */
#define ADRSPC_PCIBAY_1_MM	0x4CA00000	/* PCI PA Bay 1 */


#define ADRSPC_NEV_ASIC		0x4E800000	/* Nevada ASIC */

/*
 * The TLB mapping needs to start at a lower address.  See notes by
 * PHY_ADRSPC_CYA_ASIC.
 */
#define ADRSPC_CYA_ASIC_MAP	0x4E900000	/* CYA ASIC address space */
#define ADRSPC_CYA_ASIC		(ADRSPC_CYA_ASIC_MAP + 0x1000)  /* CYA ASIC
                                                                   registers */
#define ADRSPC_DBUS             ADRSPC_CYA_ASIC /* Dbus registers */
#define ADRSPC_DRAM_WBERR	0x4EC00000 	/* Reno DRAM write exception
						   address register */
#define ADRSPC_IO_WBERR		0x4ED00000 	/* Reno IO write exception
						   address register */
#define ADRSPC_PROM		0x4F000000	/* rom monitor EPROM */

/*
 * PCI Memory definitions
 */
#define PCIMEM_VIRT_BASE_ADDR	0x30000000	/* PCI mem base virt address */
#define PCIMEM_UNC_OFS		0x00000000	/* Offset to uncached view */
#define PCIMEM_CWT_OFS		0x01000000	/* Offset to cwt view */
#define PCIMEM_CWT_HIGH         0x0100          /* High order nibble */
#define PCIMEM_CWB_OFS		0x03000000	/* Offset to cwb view */
#define PCIMEM_CACHE_MASK	0xFCFFFFFF	/* Cached bits 'and' mask */
#define PCI_PCIMEM_LE_OF	0x80000000	/* PCI view - little endian
                                                   byte swap address base */

#define ADRSPC_PCIMEM_BITSW	0x50000000	/* PCI memory, bit swapped */

/*
 * PCI memory, uncached view of memory
 */
#define ADRSPC_PCIMEM		(PCIMEM_VIRT_BASE_ADDR+PCIMEM_UNC_OFS)

/*
 * PCI memory, uncached view of memory
 */
#define ADRSPC_PCIMEM_UNC	(PCIMEM_VIRT_BASE_ADDR+PCIMEM_UNC_OFS)

/*
 * PCI memory, cached write  through view of memory
 */
#define ADRSPC_PCIMEM_CWT	(PCIMEM_VIRT_BASE_ADDR+PCIMEM_CWT_OFS)

/*
 * PCI memory, cached write back view of memory
 */
#define ADRSPC_PCIMEM_CWB	(PCIMEM_VIRT_BASE_ADDR+PCIMEM_CWB_OFS)

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
#define PHY_ADRSPC_LOW_SRAM	0x00000000	/* SRAM heap */
#define PHY_ADRSPC_RAM		0x00000000	/* code, data memory */
#define PHY_ADRSPC_RAM_END      0x08000000      /* End of RAM area (up to 128 MB) */
#define PHY_ADRSPC_RAM		0x00000000	/* code, data memory */
#define PHY_ADRSPC_PMA_ASIC	0x10000000	/* PMA asic conf space */

/*
 * IMPORTANT:  The register space on the CYA chip starts at physical
 * address 0x10041000.  However, the TLB requires all the low-order
 * bits in the address to be 0 at the start of the page.  To solve
 * this, the _MAP addresses were created.  The _MAP and non-_MAP
 * addresses will coexist for now, as developers are currently using
 * the non-_MAP variant.
 */
#define PHY_ADRSPC_CYA_ASIC_MAP 0x10040000	/* CYA ASIC address space */
#define PHY_ADRSPC_CYA_ASIC     0x10041000	/* CYA ASIC registers */
#define PHY_ADRSPC_DBUS         PHY_ADRSPC_CYA_ASIC   /* Dbus registers */
#define PHY_ADRSPC_CBUS		0x11000000	/* MEMD / CBUS */
#define PHY_ADRSPC_PCIMEM	0x12000000	/* PCI memory */

#define PHY_ADRSPC_PCIBAY_0	0x13002000	/* PCI PA Bay 0, I/O space */
#define PHY_ADRSPC_PCIBAY_1	0x1300A000	/* PCI PA Bay 1, I/O space */

#define PHY_ADRSPC_MMIO_BASE	0x14000000	/* Base of PCI MM IO */

#define PHY_ADRSPC_NEV_ASIC	0x1C000000	/* Nevada ASIC */
#define PHY_ADRSPC_PROM		0x1FC00000	/* Monitor EPROM */
#define PHY_ADRSPC_PROM_END     0x1FFFFFFF	/* EPROM end */
#define PHY_ADRSPC_PCIMEM_BITSW	0x50000000	/* PCI memory, bit swapped */

#define PHY_ADRSPC_DRAM_WBERR	0x1EC00000	/* DRAM Write bus error reg. */
#define PHY_ADRSPC_IO_WBERR	0x1ED00000	/* IO Write bus error reg. */

#define PHY_ADRSPC_PCI_IO	0x10000000	/* PCI IO space */

#define PA_PCI_IO_LIMIT         0x00007fff      /* IO Limit per PA */

/*
 * Addresses as seen on the PCI bus by devices
 */
#define PCI_MEMMAPPED_IO_BAY0	0x00100000	/* PCI mem/mapped IO, bay 0 */
#define PCI_MEMMAPPED_IO_BAY1	0x00200000	/* PCI mem/mapped IO, bay 1 */

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
 * Virtual address space allocations - Processor in Kernel Mode (kseg1)
 * These are virtual addresses for Kernel Segment 1. ROM Monitor use 
 * the KSEG1 most of the time except its stack space
 */
#define KSEG1_ADRSPC_LOW_SRAM        PHY_TO_KSEG1(PHY_ADRSPC_LOW_SRAM)
#define KSEG1_ADRSPC_RAM             PHY_TO_KSEG1(PHY_ADRSPC_RAM)
#define KSEG1_ADRSPC_RAM_END         PHY_TO_KSEG1(PHY_ADRSPC_RAM_END)
#define KSEG1_ADRSPC_CYA_ASIC        PHY_TO_KSEG1(PHY_ADRSPC_CYA_ASIC)
#define KSEG1_ADRSPC_CBUS            PHY_TO_KSEG1(PHY_ADRSPC_CBUS)
#define KSEG1_ADRSPC_NEV_ASIC        PHY_TO_KSEG1(PHY_ADRSPC_NEV_ASIC)
#define KSEG1_ADRSPC_PMA_ASIC        PHY_TO_KSEG1(PHY_ADRSPC_PMA_ASIC)
#define KSEG1_ADRSPC_DBUS            PHY_TO_KSEG1(PHY_ADRSPC_DBUS)
#define KSEG1_ADRSPC_IO_WBERR        PHY_TO_KSEG1(PHY_ADRSPC_IO_WBERR)
#define KSEG1_ADRSPC_PROM            PHY_TO_KSEG1(PHY_ADRSPC_PROM)
#define KSEG1_ADRSPC_PROM_END        PHY_TO_KSEG1(PHY_ADRSPC_PROM_END)
#define KSEG1_ADRSPC_PCIMEM          PHY_TO_KSEG1(PHY_ADRSPC_PCIMEM)

#define PHY_ADRSPC_MEMD    	     0x08000000 /* Start of MEMD */
#define KSEG1_ADRSPC_MEMD            PHY_TO_KSEG1(PHY_ADRSPC_MEMD)

/* these are virtual device, not physical ones in the SVIP */
#define PHY_ADRSPC_NVRAM             0x00006000
#define KSEG1_ADRSPC_NVRAM           PHY_TO_KSEG1(PHY_ADRSPC_NVRAM)
#define PHY_ADRSPC_VIP_VFPGA         0x00007000
#define KSEG1_ADRSPC_VIP_VFPGA       PHY_TO_KSEG1(PHY_ADRSPC_VIP_VFPGA)
/*
 * Port separation
 */
#define PCI_BAY_SEPARATION               0x00100000 /* 1 mbyte per PA bay */
#define PCI_DEV_IO_MAPPED_IO_SEPARATION  0x400      /* 1 KB for each device */
#define PCI_DEV_MEM_MAPPED_IO_SEPARATION 0x4000     /* 16 KB for each device */
#define PCI_PA_DEVICE_BASE_OFFSET        0          /* check with manoj */

/*
 * PCI view of SRAM (through PMA).  These define where the start of
 * PCI memory appears from PCI I/O and memory spaces.  Note that
 * the PCI bus view of PCI memory is the same as the Orion virtual
 * address of PCI memory.  This saves us address translation cycles.
 */
#define PCI_MEM_MAP_ADRSPC_PCIMEM    ADRSPC_PCIMEM_UNC /* Mem space mem 
							  start */
#define PCI_IO_MAP_ADRSPC_PCIMEM     ADRSPC_PCIMEM_UNC /* I/O space mem 
							  start */

/*
 * PCI view of SRAM (through PMA).  These define where the start of
 * PCI memory appears from PCI I/O and memory spaces, and provide a
 * hardware, little-endian byte swap facility.
 */
#define PCI_MEM_MAP_ADRSPC_PCIMEM_LE (ADRSPC_PCIMEM_UNC | PCI_PCIMEM_LE_OFS)
#define PCI_IO_MAP_ADRSPC_PCIMEM_LE  (ADRSPC_PCIMEM_UNC | PCI_PCIMEM_LE_OFS)

#endif /* __SVIP_PCMAP_H__ */
