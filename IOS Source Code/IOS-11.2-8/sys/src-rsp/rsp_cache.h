/* $Id: rsp_cache.h,v 3.2 1995/11/17 18:47:11 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-rsp/rsp_cache.h,v $
 *------------------------------------------------------------------
 * Inline functions for cache line manipulation on IRSP
 *
 * Chris Shaker, Marty Cieslak, December, 1994
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_cache.h,v $
 * Revision 3.2  1995/11/17  18:47:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:57:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __RSP_CACHE_H__
#define __RSP_CACHE_H__

/*
 * Inline functions:
 */

#define HIT_INVALIDATE_DATA		"17"
#define HIT_WRITEBACK_INVALIDATE_DATA	"21"
#define CREATE_DIRTY_EXCLUSIVE_DATA	"13"

/*
 * Due to the 'cache' instruction bug in the 2.0 Orion, we have to
 * ensure that the Orion's internal data cache refill buffer is empty 
 * before we issue our cache instruction. The way to ensure this is by
 * issuing a load instruction to an uncached address. What a pain
 * since this costs us in performance by at least 8 instructions due
 * to the read stall.
 */
static inline
void r4k_CACHE_instruction_fix (void)
{
    /*
     * The fastest uncached read we can do is to a register on the DCL ASIC,
     * so read something safe there (I/O space is mapped uncached).
     */
    register void *ptr = (void *) ADRSPC_COUNTER0;

    asm volatile(".set noreorder");

    asm volatile ("lhu $0,0(%0)" :
	/* no outputs */ : "r" (ptr));

    asm volatile(".set reorder");
}

/*
 * inval_buf_hdr_inline
 * Do a HIT_INVALIDATE_DATA on the supplied buffer header or CCB
 *
 * The input buffer header pointer must be at least 32 bit word aligned.
 * For speed, we expect the caller to know what they are doing, and do
 * not check this.
 */

static inline
void inval_buf_hdr_inline (bufhdr_t *bufptr)
{
    ulong	status;

    /*
     * Efficiently mask all interrupts to prevent timer interrupts
     * between 'cache' instruction bug workaround and cache instruction!
     */
    GLOBAL_INTERRUPT_BLOCK(status);

    /* Protect against 2.0 Orion 'cache' instruction bug. */
    r4k_CACHE_instruction_fix();

    /*
     * Invalidate the buffer header here. Pak_process has already
     * flushed and invalidated the dirty data from the buffer data
     * in the data cache.
     *
     * We will use the:
     *   'cache HIT_INVALIDATE_DATA,<Offset>(<Base_register>)'
     * instruction to invalidate the cache line that the
     * buffer header is in on the router.  This is so that we
     * don't keep stale data around for the other buffer
     * header in the cache line (two buffer headers fit into
     * one cache line). We can do this since the buffer
     * headers are going to be accessed via 'write through'
     * caching, so no dirty data will be in the cache.
     */
    asm volatile(".set noreorder");
    asm volatile(".set mips3");

    asm volatile(" cache "HIT_INVALIDATE_DATA",0(%0)" :
	/* no output */ : "r" (bufptr));

    asm volatile(".set mips2");
    asm volatile(".set reorder");

    /*
     * Restore interrupt enable state:
     */
    GLOBAL_INTERRUPT_UNBLOCK(status);
}

/*
 * hit_invalidate_cache_line_inline
 *
 * ASSUMES THAT 2.0 Orion's refill buffer is empty!!!
 */

static inline
void hit_invalidate_cache_line_inline (bufhdr_t *bufptr)
{
    asm volatile(".set noreorder");
    asm volatile(".set mips3");

    asm volatile(" cache "HIT_INVALIDATE_DATA",0(%0)" :
	/* no output */ : "r" (bufptr));

    asm volatile(".set mips2");
    asm volatile(".set reorder");
}

/*
 * invalidate_data_inline
 * Do a HIT_INVALIDATE_DATA on the specified memory region
 * Used to flush a packet we are discarding out of the data cache
 *
 * The region of memory affected by this routine is larger than that
 * specified, since all cached memory is accessed in units of entire
 * cache lines.
 */

static inline
void invalidate_data_inline (uchar *ptr, int bytes)
{
    ulong	first, last, cline_ptr;
    ulong	status;

    /*
     * Efficiently mask all interrupts to prevent timer interrupts
     * between 'cache' instruction bug workaround and cache instruction!
     */
    GLOBAL_INTERRUPT_BLOCK(status);

    /*
     * Invalidate the specified number of bytes of MEMD starting at
     * the address specified by 'ptr'.
     *
     * We will use the:
     *  'cache HIT_INVALIDATE_DATA, <Offset>(<Base_register>)'
     * instruction to invalidate whole cache lines of memory from the
     * data cache.
     *
     * Issue the cache instruction for every cache line in the region.
     * The virtual address passed with the CACHE instruction must be
     * at least 32 bit word aligned.
     */
    first =  (ulong) ptr              & ~(R4K_DLINE_SHIFT - 1);
    last  = ((ulong) ptr + bytes - 1) & ~(R4K_DLINE_SHIFT - 1);

    /* Protect against 2.0 Orion 'cache' instruction bug. */
    r4k_CACHE_instruction_fix();

    for (cline_ptr = first; cline_ptr <= last; cline_ptr += R4K_DLINE_SHIFT) {

	asm volatile(".set noreorder");
	asm volatile(".set mips3");

	asm volatile(" nop");	/* Ensure 4 cycles w/o data cache reference */
	asm volatile(" cache "HIT_INVALIDATE_DATA",0(%0)"
	    : /* no output */
	    : "r" (cline_ptr));

	asm volatile(".set mips2");
	asm volatile(".set reorder");
    }

    /*
     * Restore interrupt enable state:
     */
    GLOBAL_INTERRUPT_UNBLOCK(status);
}

/*
 * writeback_inval_data_inline
 * Do a HIT_WRITEBACK_INVALIDATE_DATA on the specified memory region:
 *
 * The region of memory affected by this routine is larger than that
 * specified, since all cached memory is accessed in units of entire
 * cache lines.
 */

static inline
void writeback_inval_data_inline (uchar *ptr, int bytes)
{
    ulong	first, last, cline_ptr;
    ulong	status;

    /*
     * Efficiently mask all interrupts to prevent timer interrupts
     * between 'cache' instruction bug workaround and cache instruction!
     */
    GLOBAL_INTERRUPT_BLOCK(status);

    /*
     * Write back and invalidate the specified number of bytes of
     * MEMD starting at the address specified by 'ptr'.
     *
     * We will use the:
     *  'cache HIT_WRITEBACK_INVALIDATE_DATA,
     *         <Offset>(<Base_register>)'
     * instruction to flush and invalidate whole cache lines from the
     * data cache into MEMD.
     *
     * Issue the cache instruction for every cache line in the region.
     * The virtual address passed with the CACHE instruction must be
     * at least 32 bit word aligned.
     */
    first =  (ulong) ptr              & ~(R4K_DLINE_SHIFT - 1);
    last  = ((ulong) ptr + bytes - 1) & ~(R4K_DLINE_SHIFT - 1);

    /* Protect against 2.0 Orion 'cache' instruction bug. */
    r4k_CACHE_instruction_fix();

    for (cline_ptr = first; cline_ptr <= last; cline_ptr += R4K_DLINE_SHIFT) {

	asm volatile(".set noreorder");
	asm volatile(".set mips3");

	asm volatile(" nop");	/* Ensure 4 cycles w/o data cache reference */
	asm volatile(" cache "HIT_WRITEBACK_INVALIDATE_DATA",0(%0)"
	    : /* no output */
	    : "r" (cline_ptr));

	asm volatile(".set mips2");
	asm volatile(".set reorder");
    }

    /*
     * Restore interrupt enable state:
     */
    GLOBAL_INTERRUPT_UNBLOCK(status);
}

/*
 * fs_writeback_inval_data_inline
 * Do a HIT_WRITEBACK_INVALIDATE_DATA on the predetermined number of cache
 * lines to ensure that any MAC encapsulation changes and any network layer
 * changes have been flushed to memory before DMA starts:
 *
 * Assume that the encapsulation area has been increased in size to 96
 * bytes, a multiple of our 32 byte cache line size. Assume that the most of
 * the network layer that we will also touch during fast switching is 128
 * bytes.  So, we have to flush and invalidate the first seven cache lines of
 * the buffer data.
 *
 * We will use the:
 *  'cache HIT_WRITEBACK_INVALIDATE_DATA, <Offset>(<Base_register>)'
 * instruction to flush and invalidate 7 cache lines of the buffer data
 * from the CPU cache into MEMD. It will be most efficient to do this via
 * an unrolled loop (a list of cache instructions)
 *
 * Issue the cache instruction for every cache line in the region.
 */

static inline
void fs_writeback_inval_data_inline (uchar *ptr)
{
    /*
     * Write back and invalidate the predetermined number of cache
     * lines to MEMD starting at the address specified by 'ptr'.
     */
    ulong	start;
    ulong	status;

    /*
     * Efficiently mask all interrupts to prevent timer interrupts
     * between 'cache' instruction bug workaround and cache instruction!
     */
    GLOBAL_INTERRUPT_BLOCK(status);

    /*
     * The virtual address passed with the CACHE instruction
     * must be at least 32 bit word aligned:
     */
    start = (ulong) ptr & ~(R4K_DLINE_SHIFT - 1);

    /* Protect against 2.0 Orion 'cache' instruction bug. */
    r4k_CACHE_instruction_fix();

    asm volatile(".set noreorder");
    asm volatile(".set mips3");

    /*
     * Ensure that the MAC encapsulation changes have been flushed.
     * 3 32 byte cache lines gives up to 96 bytes of MAC encapsulation:
     */
    asm volatile(" cache "HIT_WRITEBACK_INVALIDATE_DATA",0*32(%0)" :
	/* no output */ : "r" (start));

    asm volatile(" cache "HIT_WRITEBACK_INVALIDATE_DATA",1*32(%0)" :
	/* no output */ : "r" (start));

    asm volatile(" cache "HIT_WRITEBACK_INVALIDATE_DATA",2*32(%0)" :
	/* no output */ : "r" (start));

    /*
     * Ensure that the network layer changes have been flushed.
     * 3 32 byte cache lines gives up to 96 bytes of network layer changes:
     */
    asm volatile(" cache "HIT_WRITEBACK_INVALIDATE_DATA",3*32(%0)" :
	/* no output */ : "r" (start));

    asm volatile(" cache "HIT_WRITEBACK_INVALIDATE_DATA",4*32(%0)" :
	/* no output */ : "r" (start));

    asm volatile(" cache "HIT_WRITEBACK_INVALIDATE_DATA",5*32(%0)" :
	/* no output */ : "r" (start));

    asm volatile(" cache "HIT_WRITEBACK_INVALIDATE_DATA",6*32(%0)" :
	/* no output */ : "r" (start));

    asm volatile(".set mips2");
    asm volatile(".set reorder");

    /*
     * Restore interrupt enable state:
     */
    GLOBAL_INTERRUPT_UNBLOCK(status);
}

/*
 * rsp_create_dirty_exclusive_inline
 * Do a CREATE_DIRTY_EXCLUSIVE_DATA on the cache line containing the
 * specified address to prevent us from reading that cache line as we
 * write it.
 *
 * We will use the:
 *  'cache CREATE_DIRTY_EXCLUSIVE_DATA, <Offset>(<Base_register>)'
 * instruction to avoid loading data needlessly from memory before writing
 * to it.
 */

static inline
void rsp_create_dirty_exclusive_inline (uchar *ptr)
{
    ulong	start;
    ulong	status;

    /*
     * Efficiently mask all interrupts to prevent timer interrupts
     * between 'cache' instruction bug workaround and cache instruction!
     */
    GLOBAL_INTERRUPT_BLOCK(status);

    /*
     * The virtual address passed with the CACHE instruction
     * must be at least 32 bit word aligned:
     */
    start = (ulong) ptr & ~(R4K_DLINE_SHIFT - 1);

    /* Protect against 2.0 Orion 'cache' instruction bug. */
    r4k_CACHE_instruction_fix();

    asm volatile(".set noreorder");
    asm volatile(".set mips3");

    asm volatile(" cache "CREATE_DIRTY_EXCLUSIVE_DATA",0(%0)" :
	/* no output */ : "r" (start));

    asm volatile(".set mips2");
    asm volatile(".set reorder");

    /*
     * Restore interrupt enable state:
     */
    GLOBAL_INTERRUPT_UNBLOCK(status);
}

#endif /* __RSP_CACHE_H__ */
