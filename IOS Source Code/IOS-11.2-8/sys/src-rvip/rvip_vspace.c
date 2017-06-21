/* $Id: rvip_vspace.c,v 3.4 1996/03/06 21:24:52 dlobete Exp $
 * $Source: /release/111/cvs/Xsys/src-rvip/rvip_vspace.c,v $
 *------------------------------------------------------------------
 * rvip_vspace.c - Virtual Address Space definition for Checkers
 *
 * January 1995, Michael Beesley
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rvip_vspace.c,v $
 * Revision 3.4  1996/03/06  21:24:52  dlobete
 * CSCdi49840:  Support for 32 MB DRAM required for DFS on RVIP
 * Map the DRAM according to the size we dynamically sense on board.
 * - For 8 MB RVIPs we'll have 6 MB DRAM and 3 MB of PCI space.
 * - For 32 MB RVIPs we'll have 28 MB DRAM and 4 MB of PCI space.
 *
 * Revision 3.3  1995/12/01  02:00:18  mbeesley
 * CSCdi44698:  PCI subsystem needs improvement
 *
 * Revision 3.2  1995/11/17  18:51:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:20  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:59:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "interface_private.h"

#include "../src-4k/vspace_4k.h"
#include "../pas/if_pas.h"

/*
 * PCI memory sizes 
 */
#define PCI_SHIFT	TLB_1M_SHIFT
#define PCI_PAGESIZE	TLB_PAGESIZE_1M

/*
 * On rev2 checkers, some problems with the rom force us to
 * link to an address above 2meg. In order not to waste the
 * first 2 meg of memory, we re-use it for the "sram" pool
 * (allthough  it is really DRAM -;)
 */
#define SRAM_SHIFT	TLB_1M_SHIFT
#define SRAM_PAGESIZE	TLB_PAGESIZE_1M
#define L2_CACHE_ADDR33	ADDR_BIT33_ZERO

/*
 * Define the virtual space.
 *
 * vaddr, paddr,
 * page_mask, pshift,
 * cache, dirty, valid, global
 */

vspace_tbl_t vspace_rvip[] = 
{

/* 512K ROM, uncached, read only */

{ ADRSPC_PROM, PHY_ADRSPC_PROM, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_256K, TLB_256K_SHIFT,
  TLB_UNCACHED, 0, 1, 1 },

/* 8K Nevada ASIC, uncached, r/w */

{ ADRSPC_NEV_ASIC, PHY_ADRSPC_NEV_ASIC, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_4K, TLB_4K_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },

/* 8K PA Bay 0, uncached, r/w */

{ ADRSPC_PCIBAY_0, PHY_ADRSPC_PCIBAY_0, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_4K, TLB_4K_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },

/* 8K PA Bay 1, uncached, r/w */

{ ADRSPC_PCIBAY_1, PHY_ADRSPC_PCIBAY_1, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_4K, TLB_4K_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },

/* 512K PA Bay 0 Memory Mapped, uncached, r/w */

{ ADRSPC_PCIBAY_0_MM, PHY_ADRSPC_MM_BAY0, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_256K, TLB_256K_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },

/* 512K PA Bay 1 Memory Mapped, uncached, r/w */

{ ADRSPC_PCIBAY_1_MM, PHY_ADRSPC_MM_BAY1, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_256K, TLB_256K_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },

/* FIXME : DRAM WBerr may not be needed */

/* 8K DRAM Wr BERR, uncached, r/w */

{ ADRSPC_DRAM_WBERR, PHY_ADRSPC_DRAM_WBERR, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_4K, TLB_4K_SHIFT,          
  TLB_UNCACHED, 1, 1, 1 },

/* 8K Shared memory Wr BERR, uncached, r/w */

{ ADRSPC_SHMEM_WBERR, PHY_ADRSPC_SHMEM_WBERR, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_4K, TLB_4K_SHIFT,          
  TLB_UNCACHED, 1, 1, 1 },

/* 8K IO Wr BERR, uncached, r/w */

{ ADRSPC_IO_WBERR, PHY_ADRSPC_IO_WBERR, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_4K, TLB_4K_SHIFT,          
  TLB_UNCACHED, 1, 1, 1 },

/* 8K PMAC PCI controller, uncached, r/w */

{ ADRSPC_PMAC, PHY_ADRSPC_PMAC, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_4K, TLB_4K_SHIFT,          
  TLB_UNCACHED, 1, 1, 1 },

/* 128K PCI Type 1, uncached, r/w */

{ ADRSPC_PCI_TYPE1, PHY_ADRSPC_PCI_TYPE1, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_64K, TLB_64K_SHIFT,      
  TLB_UNCACHED, 1, 1, 1 },

/* 32K PCI Type 0, ID Sel 0, uncached, r/w */

{ ADRSPC_PCI_IDSEL0, PHY_ADRSPC_PCI_IDSEL0, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_16K, TLB_16K_SHIFT,      
  TLB_UNCACHED, 1, 1, 1 },

/* 32K PCI Type 0, ID Sel 1, uncached, r/w */

{ ADRSPC_PCI_IDSEL1, PHY_ADRSPC_PCI_IDSEL1, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_16K, TLB_16K_SHIFT,      
  TLB_UNCACHED, 1, 1, 1 },

/* SRAM Heap, cached write back, r/w */

{ ADRSPC_LOW_SRAM, PHY_ADRSPC_LOW_SRAM, L2_CACHE_ADDR33,
  SRAM_PAGESIZE, SRAM_SHIFT,
  TLB_CACHE_NONCOHERENT, 1, 1, 1 },

/* 8M CBUS/MEMD, uncached, r/w */

{ ADRSPC_CBUS, PHY_ADRSPC_CBUS, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_4M, TLB_4M_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },

/* 8K DBUS registers, uncached, r/w */

{ ADRSPC_DBUS, PHY_ADRSPC_DBUS, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_4K, TLB_4K_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },

/* 8K DMA registers, uncached, r/w */

{ ADRSPC_DMA, PHY_ADRSPC_DMA, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_4K, TLB_4K_SHIFT,
  TLB_UNCACHED, 1, 1, 1 }

};

#define VSPACE_SIZE    (sizeof(vspace_rvip) / sizeof(vspace_tbl_t))

/*
 * vspace_table_addr :
 * Return a relocated pointer to the above array along
 * with its size (PIC magic)
 */
vspace_tbl_t *vspace_table_addr (long *size)
{
    ulong ptr, call_pc;

    SAVE_CALLER();

    /*
     * We have a table in our data segment, with an address within our
     * virtual address space (IE 0x6*******). We are running through
     * KSeg0 (IE RA=0x8???????). We must calculate a pointer that will
     * point to the table through the same section we are running out
     * of (IE 0x8*******)
     */
    call_pc = caller();
    ptr = (ulong)vspace_rvip & ~R4K_HI_ADDR_MASK;
    ptr += (call_pc & R4K_HI_ADDR_MASK);
    *size = VSPACE_SIZE;

    return ((vspace_tbl_t *)ptr);
}

/* end of file */
