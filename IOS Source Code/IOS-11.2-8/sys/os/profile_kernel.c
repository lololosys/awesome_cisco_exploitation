/* $Id: profile_kernel.c,v 3.1.16.1 1996/03/18 21:31:01 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/os/profile_kernel.c,v $
 *------------------------------------------------------------------
 * Kernel support for Profiling routines
 *
 * December 1992, Dave Katz
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This module supports CPU profiling.  When the NMI interrupt fires,
 * if "profile_blocks" is non-null (signalling that profiling is in
 * effect), the NMI interrupt routine will call "profile_pc".
 *
 * Profile_pc walks the chain of profile blocks, which are sorted in
 * ascending order by start address.  If the PC falls within the range
 * covered by a block, the appropriate entry in the profile block is
 * incremented.
 *------------------------------------------------------------------
 * $Log: profile_kernel.c,v $
 * Revision 3.1.16.1  1996/03/18  21:31:01  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.6.1  1996/03/09  05:10:13  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.1  1996/02/21  21:52:06  hampton
 * Placeholders for Port Ready.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "config.h"
#include "parser.h"
#include "profile.h"
#include "sched_private.h"

profile *profile_blocks;	/* Linked list of profile blocks */
ulong profile_total;		/* Total number of calls */
boolean profile_enabled;	/* TRUE if profiling is enabled */
boolean selective_profile;	/* TRUE if profiling selected tasks */
boolean profile_interrupts;	/* TRUE if profiling interrupt state */
boolean profile_hogs;		/* TRUE if profiling CPU hogs */

/*
 * profile_pc - bump the appropriate profile block for this pc
 */
void profile_pc (boolean interrupt_state, void *pc)
{
    profile *pro_ptr;

    /* Count the fact that we were here. */

    profile_total++;

    /* Check to see if we're being selective about what we're profiling. */

    if (selective_profile) {

	/*
	 * If we're in interrupt state, only pick it up if profiling
	 * interrupts.
	 */
	if (interrupt_state) {
	    if (!profile_interrupts)
		return;

	} else {			/* Not interrupt state */

	    /*
	     * If not profiling the current task, bail. This code is
	     * allowed to touch forkx directly because it is called from
	     * the clock interrupt path.
	     */
	    
	    if (schedflag || !forkx || !forkx->profile)
		return;
	}
    }

    /* Walk the profiling chain. */

    for (pro_ptr = profile_blocks; pro_ptr; pro_ptr = pro_ptr->next) {
	ulong offset;
       
	/* Skip this block if we're not in it. */

	if (pc > pro_ptr->end_addr)
	    continue;

	/* Get out if we've gone far enough. */

	if (pro_ptr->start_addr > pc)
	    break;

	/* Figure out which bin we want. */

	offset = ((ulong)pc - (ulong)pro_ptr->start_addr) >> pro_ptr->resolution;

	/* Bump the right bin. */

	pro_ptr->bins[offset]++;

    }
}

/*
 * profile_clear_blocks - zero all profile blocks
 */

void profile_clear_blocks (void)
{
    profile *pro_ptr;
    ulong count;
    boolean was_enabled;

    /* Temporarily disable profiling to keep things coherent. */

    was_enabled = profile_enabled;
    profile_enabled = FALSE;

    /* Zero the total. */

    profile_total = 0;

    /* Walk the chain, zeroing all bins. */

    for (pro_ptr = profile_blocks; pro_ptr; pro_ptr = pro_ptr->next) {
	count = bin_count(pro_ptr->start_addr, pro_ptr->end_addr,
			  pro_ptr->resolution);
	memset(pro_ptr->bins, 0, count * sizeof(pro_ptr->bins[0]));
    }

    profile_enabled = was_enabled;	/* Restore it as it was */
}
