/* $Id: svip_vspace.c,v 3.1.62.1 1996/03/21 23:51:43 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-svip/svip_vspace.c,v $
 *------------------------------------------------------------------
 * svip_vspace.c - Virtual Address Space definition for VIPer II
 *
 * September 1995, Steve Jacobson
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_vspace.c,v $
 * Revision 3.1.62.1  1996/03/21  23:51:43  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.10.1  1996/02/10  00:22:15  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/12/26  21:18:47  getchell
 * Placeholder files for Viper development
 *
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

#define PCI_PAGESIZE	TLB_PAGESIZE_1M
#define PCI_SHIFT	TLB_1M_SHIFT

/*
 * Presently Viper image is linked to 2 meg, wasting
 * the first 2 meg of DRAM.
 */

/*
 * Define the virtual space.
 *
 * vaddr, paddr,
 * page_mask, pshift,
 * cache, dirty, valid, global
 */

vspace_tbl_t vspace_svip[] = 
{

/* 512K ROM, uncached, read only */

{ ADRSPC_PROM, PHY_ADRSPC_PROM, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_256K, TLB_256K_SHIFT,
  TLB_UNCACHED, 0, 1, 1 },

/* 8K Nevada ASIC, uncached, r/w */

{ ADRSPC_NEV_ASIC, PHY_ADRSPC_NEV_ASIC, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_4K, TLB_4K_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },

/* 8K PA Bay 0, I/O space, uncached, r/w */

{ ADRSPC_PCIBAY_0, PHY_ADRSPC_PCIBAY_0, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_4K, TLB_4K_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },

/* 8K PA Bay 1, I/O space, uncached, r/w */

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

/* 8K DRAM Wr BERR, uncached, r/w */

{ ADRSPC_DRAM_WBERR, PHY_ADRSPC_DRAM_WBERR, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_4K, TLB_4K_SHIFT,          
  TLB_UNCACHED, 1, 1, 1 },

/* 8K IO Wr BERR, uncached, r/w */

{ ADRSPC_IO_WBERR, PHY_ADRSPC_IO_WBERR, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_4K, TLB_4K_SHIFT,          
  TLB_UNCACHED, 1, 1, 1 },

/* 2M PCI memory, uncached, r/w */

{ ADRSPC_PCIMEM_UNC, PHY_ADRSPC_PCIMEM, ADDR_BIT33_ONE,
  PCI_PAGESIZE, PCI_SHIFT,          
  TLB_UNCACHED, 1, 1, 1 },

/* 2M PCI memory, cached write through, r/w */

{ ADRSPC_PCIMEM_CWT, PHY_ADRSPC_PCIMEM, ADDR_BIT33_ONE,
  PCI_PAGESIZE, PCI_SHIFT,          
  TLB_CACHE_WT, 1, 1, 1 },

/* 512K PMA PCI controller, uncached, r/w */

{ ADRSPC_PMA_ASIC, PHY_ADRSPC_PMA_ASIC, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_256K, TLB_256K_SHIFT,          
  TLB_UNCACHED, 1, 1, 1 },

/* 8M CBUS/MEMD, uncached, r/w */

{ ADRSPC_CBUS, PHY_ADRSPC_CBUS, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_4M, TLB_4M_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },

/* 128K CYA registers, uncached, r/w */

{ ADRSPC_CYA_ASIC_MAP, PHY_ADRSPC_CYA_ASIC_MAP, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_64K, TLB_64K_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },
};

#define VSPACE_SIZE    (sizeof(vspace_svip) / sizeof(vspace_tbl_t))

/*
 * vspace_table_addr:
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
    ptr = (ulong)vspace_svip & ~R4K_HI_ADDR_MASK;
    ptr += (call_pc & R4K_HI_ADDR_MASK);
    *size = VSPACE_SIZE;

    return ((vspace_tbl_t *)ptr);
}

/* end of file */

