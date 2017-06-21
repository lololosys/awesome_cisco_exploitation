/* $Id: cache_mips.c,v 3.2.58.1 1996/03/21 23:29:59 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k/cache_mips.c,v $
 *------------------------------------------------------------------
 * cache_mips.c
 *
 * May 1993, Michael Beesley
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Cache routines for R4000/Orion CPU.
 *------------------------------------------------------------------
 * $Log: cache_mips.c,v $
 * Revision 3.2.58.1  1996/03/21  23:29:59  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:22:38  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:44:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:39:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "monitor.h"
#include "cpu_mips.h"
#include "mon_defs.h"
#include "queryflags.h"

unsigned long monitor_stk;

void clearcaches (void)
{
    cache_flush(0, -1);
}

/*
 * Initialize the cache, status register
 */
void init_cache (void)
{
    unsigned long config = getcp0_config();
    unsigned long index, lines;

    config = (config & ~CONFIG_KSEG0_COHERENCY) | TLB_UNCACHED;
    /*
     * 32 byte line size
     */
    config |= (CONFIG_ICACHE_BLOCK|CONFIG_DCACHE_LINE);
    setcp0_config(config);

    /*
     * Initialize I-CACHE and D-CACHE
     */
    init_dcache();
    init_icache();

    /*
     * And flush them, setting KSeg0 cached, and move the stack to KSeg0
     * If our stack is in DRAM, set KSeg0 cached and move the stack
     */
    if (PHY_ADDR(MON_INIT_SP) > PHY_ADDR(KSEG1_ADRSPC_NVRAM)) {
	cache_flush(TLB_UNCACHED, -1);		/* do not move stack 	*/
	monitor_stk = MON_INIT_SP - STACK_PARAMS;
    } else {
	cache_flush(TLB_CACHE_NONCOHERENT, 0);	/* move the stack	*/
	monitor_stk = KSEG1_TO_KSEG0(MON_INIT_SP - STACK_PARAMS);
    }
}

/*
 * Cache control utility functions. Allow more code sharing between Sierra 
 * and other M68K based platforms
 */
int setcacr (int setting)
{
    return(setting);
}

/* End of Module */


