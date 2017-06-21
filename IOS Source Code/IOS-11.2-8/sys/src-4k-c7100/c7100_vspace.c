/* $Id: c7100_vspace.c,v 3.2.56.2 1996/04/23 19:21:54 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c7100/c7100_vspace.c,v $
 *------------------------------------------------------------------
 * c7100_vspace.c - Predator virtual address space definition
 *
 * July 1995, Michael Beesley
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c7100_vspace.c,v $
 * Revision 3.2.56.2  1996/04/23  19:21:54  mbeesley
 * CSCdi55360:  Enable cached, write thru view of packet memory
 * Branch: California_branch
 *
 * Revision 3.2.56.1  1996/03/21  23:48:19  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.5  1996/01/04  03:19:01  wfried
 * Branch: ELC_branch
 * Move Bootflash to unmapped address space.
 *
 * Revision 3.2.2.4  1996/01/03  06:06:51  wfried
 * Branch: ELC_branch
 * Fix PCMCIA controller configuration.
 *
 * Revision 3.2.2.3  1995/12/05  20:00:09  mbeesley
 * Branch: ELC_branch
 * Correct some register initialization. Add support for dynamic
 * parallel bridge config. Do not use a small piece of SRAM as
 * fast memory on Rev 2 cards.
 *
 * Revision 3.2.2.2  1995/11/30  02:14:43  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1995/11/22  21:43:30  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  18:41:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:23:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* 
 * Include files 
 */
#include "master.h"
#include "../src-4k/vspace_4k.h"
#include "interface_private.h"
#include "c7100.h"
#include "c7100_pcmap.h"

/*
 * Define the virtual space.
 *
 * vaddr, paddr,
 * page_mask, pshift,
 * cache, dirty, valid, global
 */

/* 
 * Note: The first five entries in the vspace table are shared between
 * IOS and the rom monitor. They should be changed with great caution and
 * great concern with backward compatibility to older rom monitors
 * in the field.
 */

vspace_tbl_t vspace_4k_c7100[] = 
{

/* First half of PCI SRAM, uncached view, used for pcimem */

{ ADRSPC_PCISRAM, PHY_ADRSPC_PCISRAM, ADDR_BIT32_ONE,
  TLB_PAGESIZE_256K, TLB_256K_SHIFT, 
  TLB_UNCACHED, 1, 1, 1 },

/* Second half of PCI SRAM, uncached view, used for pcimem */

{ ADRSPC_PCISRAM+HALF_MEG, PHY_ADRSPC_PCISRAM+HALF_MEG, ADDR_BIT32_ONE,
  TLB_PAGESIZE_256K, TLB_256K_SHIFT, 
  TLB_UNCACHED, 1, 1, 1 },

/* PCMCIA Controller registers (PCI IO space, so set bit 32 */

{ ADRSPC_PCMCIA_REGS, PHY_ADRSPC_PCMCIA_REGS, ADDR_BIT32_ONE,
  TLB_PAGESIZE_4K, TLB_4K_SHIFT, 
  TLB_UNCACHED, 1, 1, 1 },

/* PCMCIA Slot 0 windows (32meg total, uncached) */

{ ADRSPC_PCMCIA_SLOT0, ADRSPC_PCMCIA_SLOT0, ADDR_BIT32_ZERO,
  TLB_PAGESIZE_16M, TLB_16M_SHIFT, 
  TLB_UNCACHED, 1, 1, 1 },

/* PCMCIA Slot 1 windows (32meg total, uncached) */

{ ADRSPC_PCMCIA_SLOT1, ADRSPC_PCMCIA_SLOT1, ADDR_BIT32_ZERO,
  TLB_PAGESIZE_16M, TLB_16M_SHIFT, 
  TLB_UNCACHED, 1, 1, 1 },

/* Bay 0, IO board PCI IO space, uncached, r/w */

{ ADRSPC_BAY0, PHY_ADRSPC_BAY0, PCI_IO_BIT32,
  TLB_PAGESIZE_256K, TLB_256K_SHIFT,
  TLB_UNCACHED, 1, 0, 1 },

/* Bay 1, PCI IO space, uncached, r/w */

{ ADRSPC_BAY1, PHY_ADRSPC_BAY1, PCI_IO_BIT32,
  TLB_PAGESIZE_256K, TLB_256K_SHIFT,
  TLB_UNCACHED, 1, 0, 1 },

/* Bay 2, PCI IO space, uncached, r/w */

{ ADRSPC_BAY2, PHY_ADRSPC_BAY2, PCI_IO_BIT32,
  TLB_PAGESIZE_256K, TLB_256K_SHIFT,
  TLB_UNCACHED, 1, 0, 1 },

/* Bay 3, PCI IO space, uncached, r/w */

{ ADRSPC_BAY3, PHY_ADRSPC_BAY3, PCI_IO_BIT32,
  TLB_PAGESIZE_256K, TLB_256K_SHIFT,
  TLB_UNCACHED, 1, 0, 1 },

/* Bay 4, PCI IO space, uncached, r/w */

{ ADRSPC_BAY4, PHY_ADRSPC_BAY4, PCI_IO_BIT32,
  TLB_PAGESIZE_256K, TLB_256K_SHIFT,
  TLB_UNCACHED, 1, 0, 1 },

/* Bay 5, PCI IO space, uncached, r/w */

{ ADRSPC_BAY5, PHY_ADRSPC_BAY5, PCI_IO_BIT32,
  TLB_PAGESIZE_256K, TLB_256K_SHIFT,
  TLB_UNCACHED, 1, 0, 1 },

/* Bay 6, PCI IO space, uncached, r/w */

{ ADRSPC_BAY6, PHY_ADRSPC_BAY6, PCI_IO_BIT32,
  TLB_PAGESIZE_256K, TLB_256K_SHIFT,
  TLB_UNCACHED, 1, 0, 1 },

/* 512K of ROM, uncached, read only */

{ ADRSPC_PROM, ADRSPC_PROM, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_256K, TLB_256K_SHIFT,
  TLB_UNCACHED, 0, 1, 1 },

/* 2M of NVRAM, uncached, r/w */

{ ADRSPC_NVRAM, ADRSPC_NVRAM, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_1M, TLB_1M_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },

/* 2M of CPU IO, uncached, r/w */

{ ADRSPC_CPUIO, ADRSPC_CPUIO, ADDR_BIT33_ZERO,
  TLB_PAGESIZE_1M, TLB_1M_SHIFT,
  TLB_UNCACHED, 1, 1, 1 },

/* PCI SRAM, cached write-thru, no L2 cache */

{ ADRSPC_PCISRAM+PCIMEM_CWT_MASK, 
  PHY_ADRSPC_PCISRAM, ADDR_BIT33_ONE|ADDR_BIT32_ONE,
  TLB_PAGESIZE_1M, TLB_1M_SHIFT, 
  TLB_CACHE_WT, 1, 1, 1 }

};

#define VSPACE_SIZE    (sizeof(vspace_4k_c7100) / sizeof(vspace_tbl_t))

/*
 * vspace_table_addr :
 * Return a relocated pointer to the above array along
 * with its size (PIC magic)
 */
vspace_tbl_t *vspace_table_addr (long *size)
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
    ptr = (ulong)vspace_4k_c7100 & ~R4K_HI_ADDR_MASK;
    ptr += (caller() & R4K_HI_ADDR_MASK);
    *size = VSPACE_SIZE;

    return ((vspace_tbl_t *)ptr);
}

/*
 * c7100_bay2vaddr:
 * Map a PA bay in/out of the virtual address space
 */
boolean c7100_bay2vaddr (uint pa_bay, boolean inout, boolean valid)
{
    struct r4k_tlb_t tlb_e;
    vspace_tbl_t *vspace;
    ulong paddr, addr;

    if (pa_bay >= MAX_PA_BAYS) {
	return(FALSE);
    }
    vspace = &vspace_4k_c7100[pa_bay+5];

    /*
     * Setup the fields in the TLB
     */
    tlb_e.pmask.mask = vspace->page_mask;
    tlb_e.hi.vpn2 = (vspace->vaddr >> TLB_HI_VPN2SHIFT);
    if (inout) {
	addr = vspace->paddr;
    } else {
	addr = ADRSPC_BITBUCKET;
    }
    paddr = (addr >> (TLB_HI_VPN2SHIFT - 1 + vspace->pshift));
    tlb_e.lo0.pfn = paddr << vspace->pshift;
    tlb_e.lo1.pfn = (paddr + 1) << vspace->pshift;
    tlb_e.lo0.pfn |= (vspace->paddr_a35_a32 << ADDR_BIT35_32_SHIFT);
    tlb_e.lo1.pfn |= (vspace->paddr_a35_a32 << ADDR_BIT35_32_SHIFT);

    /*
     * Setup page characteristics
     */
    tlb_e.lo0.c = tlb_e.lo1.c = vspace->cache;
    tlb_e.lo0.d = tlb_e.lo1.d = vspace->dirty;
    tlb_e.lo0.g = tlb_e.lo1.g = vspace->global;
    if (valid) {
	tlb_e.lo0.v = tlb_e.lo1.v = 1;
    } else {
	tlb_e.lo0.v = tlb_e.lo1.v = 0;
    }

    /*
     * Program the TLB. The vspace table has been setup where
     * TLB entries 5 to 12 are used for the IO slot and the six
     * PA bays
     */
    r4k_settlb(&tlb_e, pa_bay+5, FALSE);
    return(TRUE);
}

/* end of file */
