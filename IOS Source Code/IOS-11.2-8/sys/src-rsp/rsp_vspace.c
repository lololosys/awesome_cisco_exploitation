/* $Id: rsp_vspace.c,v 3.2 1995/11/17 18:49:52 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-rsp/rsp_vspace.c,v $
 *------------------------------------------------------------------
 * Describes virtual address space allocation on RSP
 *
 * Michael Beesley, November 1993
 * Carl Schaefer, September 1994
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_vspace.c,v $
 * Revision 3.2  1995/11/17  18:49:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:28:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:28:53  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/01  17:08:49  shaker
 * CSCdi36423:  fastswitching speedup
 * Cache MEMD write through instead of writeback on the RSP family. This
 * buys us 6,000 to 7,000 pps gain for IPX and for Appletalk fast
 * switching.  Per Darren Kerr's suggestion.
 *
 * Revision 2.1  1995/06/07  22:58:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../src-4k/vspace_4k.h"
#include "../src-rsp/rsp_pcmap.h"
#include "../src-rsp/rsp_bufhdr.h"

#ifdef DEBUG_CACHING
#define MEMD_CACHE_POLICY	TLB_UNCACHED
#define BUFHDR_CACHE_POLICY	TLB_UNCACHED
#define TURBO_CACHE_POLICY      TLB_UNCACHED
#else
#define MEMD_CACHE_POLICY	TLB_CACHE_WT
#define BUFHDR_CACHE_POLICY	TLB_CACHE_WT
#define TURBO_CACHE_POLICY TLB_CACHE_WT
#endif

/*
 * Define the virtual space for all spaces except main CPU memory
 *
 * vaddr, paddr,
 * page_mask, pshift,
 * cache, dirty, valid, global
 */

static const vspace_tbl_t vspace_4k[] = 
{
    /* XXX - MEMD regions not configured for ASID */
    /* all of MEMD, bitswapped */
    {
	ADRSPC_MEMD_BITSWAP, PHY_MEMD_BITSWAP, ADDR_BIT33_ZERO,
	TLB_PAGESIZE_1M, TLB_1M_SHIFT, TLB_UNCACHED, 
	TLB_RO, TLB_VALID, TLB_GLOBAL,
    },

    /* all of MEMD - QA/bufhdrs/packet memory - 2Mbytes */
    {
	ADRSPC_MEMD, PHY_MEMD, ADDR_BIT33_ZERO,
	TLB_PAGESIZE_1M, TLB_1M_SHIFT, MEMD_CACHE_POLICY, 
	TLB_RW, TLB_VALID, TLB_GLOBAL,
    },

    /* MEMD as viewed by flow switching code - write-thru */
    {
	ADRSPC_MEMD_WRITETHRU, PHY_MEMD, ADDR_BIT33_ZERO,
	TLB_PAGESIZE_1M, TLB_1M_SHIFT, TURBO_CACHE_POLICY, 
	TLB_RW, TLB_VALID, TLB_GLOBAL,
    },

    /* all of MEMD, uncached */
    {
	ADRSPC_MEMD_UNCACHED, PHY_MEMD, ADDR_BIT33_ZERO,
	TLB_PAGESIZE_1M, TLB_1M_SHIFT, TLB_UNCACHED, 
	TLB_RW, TLB_VALID, TLB_GLOBAL,
    },

    /* QA ASIC - 8Kbytes */
    {
	ADRSPC_MEMD_QAREGS, PHY_MEMD_QAREGS, ADDR_BIT33_ZERO,
	TLB_PAGESIZE_4K, TLB_4K_SHIFT, TLB_UNCACHED, 
	TLB_RW, TLB_VALID, TLB_GLOBAL,
    },

    /* MEMD bufhdr segment - 56 kbytes */
    {				/* first 8Kbytes */
	ADRSPC_MEMD_BUFHDR, PHY_MEMD_BUFHDR, ADDR_BIT33_ZERO,
	TLB_PAGESIZE_4K, TLB_4K_SHIFT, BUFHDR_CACHE_POLICY, 
	TLB_RW, TLB_VALID, TLB_GLOBAL,
    },
    {				/* second 8Kbytes */
	ADRSPC_MEMD_BUFHDR + 1*8*1024, PHY_MEMD_BUFHDR + 1*8*1024,
	ADDR_BIT33_ZERO, TLB_PAGESIZE_4K, TLB_4K_SHIFT, BUFHDR_CACHE_POLICY, 
	TLB_RW, TLB_VALID, TLB_GLOBAL,
    },
    {				/* third 8Kbytes */
	ADRSPC_MEMD_BUFHDR + 2*8*1024, PHY_MEMD_BUFHDR + 2*8*1024,
	ADDR_BIT33_ZERO, TLB_PAGESIZE_4K, TLB_4K_SHIFT, BUFHDR_CACHE_POLICY, 
	TLB_RW, TLB_VALID, TLB_GLOBAL,
    },
    {				/* last 32Kbytes */
	ADRSPC_MEMD_BUFHDR + 3*8*1024, PHY_MEMD_BUFHDR + 3*8*1024,
	ADDR_BIT33_ZERO, TLB_PAGESIZE_16K, TLB_16K_SHIFT, BUFHDR_CACHE_POLICY,
	TLB_RW, TLB_VALID, TLB_GLOBAL,
    },

    /* QA diag & MEMD control - 2Mbytes */
    {
	ADRSPC_MEMD_CONTROLSPC, PHY_MEMD_CONTROLSPC, ADDR_BIT33_ZERO,
	TLB_PAGESIZE_1M, TLB_1M_SHIFT, TLB_UNCACHED, 
	TLB_RW, TLB_VALID, TLB_GLOBAL,
    },

    /* PCMCIA - XXX */

    /* NVRAM - 512Kbytes */
    {
	ADRSPC_NVRAM, ADRSPC_NVRAM, ADDR_BIT33_ZERO,
	TLB_PAGESIZE_256K, TLB_256K_SHIFT, TLB_UNCACHED, 
	TLB_RW, TLB_VALID, TLB_GLOBAL,
    },

    /* misc I/O registers - 32Kbytes */
    {
	ADRSPC_IOREGS, PHY_IOREGS, ADDR_BIT33_ZERO,
	TLB_PAGESIZE_16K, TLB_16K_SHIFT, TLB_UNCACHED, 
	TLB_RW, TLB_VALID, TLB_GLOBAL,
    },

    /* on-board flash - 32Mbytes */
    {
	ADRSPC_FLASH_BASE, PHY_FLASH_BASE, ADDR_BIT33_ZERO,
	TLB_PAGESIZE_16M, TLB_16M_SHIFT, TLB_UNCACHED, 
	TLB_RW, TLB_VALID, TLB_GLOBAL,
    },
};

#define VSPACE_SIZE    (sizeof(vspace_4k) / sizeof(vspace_tbl_t))

/*
 * vspace_table_addr :
 * Return a relocated pointer to the above array along
 * with its size (PIC magic)
 * XXX - make the table non-static and share this code w/Sierra?
 */
vspace_tbl_t *
vspace_table_addr (long *size)
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
    ptr = (ulong)vspace_4k & ~R4K_HI_ADDR_MASK;
    ptr += (caller() & R4K_HI_ADDR_MASK);
    *size = VSPACE_SIZE;

    return ((vspace_tbl_t *)ptr);
}
