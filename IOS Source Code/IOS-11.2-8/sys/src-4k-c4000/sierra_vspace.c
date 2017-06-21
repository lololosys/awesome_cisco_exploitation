/* $Id: sierra_vspace.c,v 3.2 1995/11/17 18:41:30 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-4k-c4000/sierra_vspace.c,v $
 *------------------------------------------------------------------
 * This file is used by the Sierra branch.  This is a 
 * placeholder file in the main branch.
 *
 * Michael Beesley, November 1993
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sierra_vspace.c,v $
 * Revision 3.2  1995/11/17  18:41:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:23:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:26:58  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:53:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../src-4k/vspace_4k.h"
#include "pcmap.h"

/*
 * Define the virtual space. No cached shared memory segment at the moment.
 *
 * vaddr, paddr,
 * page_mask, pshift,
 * cache, dirty, valid, global
 */

vspace_tbl_t vspace_4k_c4000[] = 
{

/* 512 K of ROM, uncached, read only */

{ ADRSPC_PROM, ADRSPC_PROM, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_256K, TLB_256K_SHIFT,
  TLB_UNCACHED, 0, 1, 1 },

/* 512 K of NVRAM, uncached, r/w */

{ ADRSPC_NVRAM, ADRSPC_NVRAM, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_256K, TLB_256K_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },

/* 32M of system flash, uncached, r/w */

{ ADRSPC_FLASH, ADRSPC_FLASH, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_16M, TLB_16M_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },

/* 32M of bootflash, uncached, r/w */

{ ADRSPC_BFLASH, ADRSPC_BFLASH, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_16M, TLB_16M_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },

/* 512K of CPU IO, uncached, r/w */

{ ADRSPC_CPUIO, ADRSPC_CPUIO, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_256K, TLB_256K_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },

/* 32M of shared memory, uncached, r/w */

{ ADRSPC_SHAREDMEM, ADRSPC_SHAREDMEM, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_16M, TLB_16M_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },

/* 32M of swapped memory, uncached, r/w */

{ ADRSPC_SHMEM_SW, ADRSPC_SHMEM_SW, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_16M, TLB_16M_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },

/* 512K slot 1, uncached, r/w */

{ ADRSPC_SLOT_1, ADRSPC_SLOT_1, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_256K, TLB_256K_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },

/* 512K slot 2, uncached, r/w */

{ ADRSPC_SLOT_2, ADRSPC_SLOT_2, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_256K, TLB_256K_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },

/* 512K slot 3, uncached, r/w */

{ ADRSPC_SLOT_3, ADRSPC_SLOT_3, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_256K, TLB_256K_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },

/* 8K slot 1 IACK, uncached, r/w */

{ ADRSPC_IACK1_BASE, ADRSPC_IACK1_BASE, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_4K, TLB_4K_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },

/* 8K slot 2 IACK, uncached, r/w */

{ ADRSPC_IACK2_BASE, ADRSPC_IACK2_BASE, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_4K, TLB_4K_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },

/* 8K slot 3 IACK, uncached, r/w */

{ ADRSPC_IACK3_BASE, ADRSPC_IACK3_BASE, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_4K, TLB_4K_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },

/* 8K DRAM write bus error, uncached, r/w */

{ ADRSPC_DRAM_WBERR, ADRSPC_DRAM_WBERR, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_4K, TLB_4K_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },

/* 8K Shared Memory write bus error, uncached, r/w */

{ ADRSPC_SHMEM_WBERR, ADRSPC_SHMEM_WBERR, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_4K, TLB_4K_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },

/* 8K IO write bus error, uncached, r/w */

{ ADRSPC_IO_WBERR, ADRSPC_IO_WBERR, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_4K, TLB_4K_SHIFT,
  TLB_UNCACHED, 1, 1, 1 }

};

#define VSPACE_SIZE    (sizeof(vspace_4k_c4000) / sizeof(vspace_tbl_t))

/*
 * vspace_table_addr :
 * Return a relocated pointer to the above array along
 * with its size (PIC magic)
 */
vspace_tbl_t *
vspace_table_addr(long *size)
{
    ulong ptr;

    SAVE_CALLER();
    
    /*
     * We have a table in our data segment, with an address within our
     * virtual address space (IE 0x6*******). We are running through
     * KSeg0 (IE RA=0x8???????). We must calculate a pointer that will
     * point to the table through the same section we are running out
     * of (IE 0x8*******)
     */
    ptr = (ulong)vspace_4k_c4000 & ~R4K_HI_ADDR_MASK;
    ptr += (caller() & R4K_HI_ADDR_MASK);
    *size = VSPACE_SIZE;

    return ((vspace_tbl_t *)ptr);
}
