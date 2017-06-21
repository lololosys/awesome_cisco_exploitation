/* $Id: profile.c,v 3.2.62.3 1996/08/06 01:41:38 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/os/profile.c,v $
 *------------------------------------------------------------------
 * Profiling routines
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
 * $Log: profile.c,v $
 * Revision 3.2.62.3  1996/08/06  01:41:38  dkatz
 * CSCdi65170:  Task profiling is broken
 * Branch: California_branch
 * Untangle parser chains.
 *
 * Revision 3.2.62.2  1996/06/28  23:25:04  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/03/18  21:30:56  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/09  05:10:10  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.26.2  1996/03/07  10:19:26  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  16:41:13  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:50:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:47:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:57:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "config.h"
#include "parser.h"
#include "profile.h"

/*
 * profile_add - add a new profile block
 *
 * Returns TRUE if successful, FALSE if out of memory
 *
 * Assumes that the parameters have been checked for reasonableness.
 */
static boolean profile_add(
    void *start,
    void *end,
    ulong resolution,
    ulong type)
{
    profile **pro_ptr, *new;

    /* Allocate the memory. */

    new = malloc(sizeof(profile) +
		 (bin_count(start, end, resolution) * sizeof(new->bins[0])));
    if (new == NULL)
	return(FALSE);

    new->start_addr = start;
    new->end_addr = end;
    new->resolution = resolution;
    new->type = type;

    /*
     * Walk the profile block chain.  Insert block in order sorted by start
     * address, end address, and resolution, in that order.  Give up if there's
     * already a block there.
     */

    pro_ptr = &profile_blocks;
    while (TRUE) {
	if (!*pro_ptr)		/* If end of chain */
	    break;
	if ((*pro_ptr)->start_addr > start)
	    break;
	if ((*pro_ptr)->start_addr == start) {
	    if ((*pro_ptr)->end_addr > end)
		break;
	    if ((*pro_ptr)->end_addr == end) {
		if ((*pro_ptr)->resolution > resolution)
		    break;
		if ((*pro_ptr)->resolution == resolution) { /* a match */
		    free(new);
		    return(TRUE);
		}
	    }
	}
	pro_ptr = &((*pro_ptr)->next);
    }
    new->next = *pro_ptr;
    *pro_ptr = new;
    
    return(TRUE);
}

/*
 * profile_delete - delete a profile block
 *
 * returns TRUE if successful, FALSE if block not found
 */

static boolean profile_delete (
    void *start,
    void *end,
    ulong resolution,
    ulong type)
{
    profile **pro_ptr, *block;

    /* Walk the chain looking for the block. */

    for (pro_ptr = &profile_blocks; *pro_ptr; pro_ptr = &((*pro_ptr)->next)) {

	/* If the block matches, delink it, free it, and exit. */

	if ((*pro_ptr)->start_addr == start &&
	    (*pro_ptr)->end_addr == end &&
	    (*pro_ptr)->resolution == resolution &&
	    (*pro_ptr)->type == type) {
	    block = *pro_ptr;
	    *pro_ptr = (*pro_ptr)->next;
	    free(block);
	    return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * profile_delete_all - delete all profile blocks
 */

static void profile_delete_all (void)
{
    profile **pro_ptr;

    /* Walk the chain, freeing all exec blocks. */

    pro_ptr = &profile_blocks;

    while (*pro_ptr) {
	profile *this = *pro_ptr;
	profile *next = this->next;
	if (this->type == PROFILE_EXEC) {
	    *pro_ptr = next;	/* Delete it */
	    free(this);		/* And free it */
	} else {
	    pro_ptr = &(this->next);
	}
    }
    if (!profile_blocks)
	profile_total = 0;
}


/*
 * show_profile_command - show profiling information
 *
 * OBJ(int,1) = true if "detail" set
 * OBJ(int,2) = true if "terse" set (implies "detail")
 */

void show_profile_command (parseinfo *csb)
{
    profile *pro_ptr;
    ulong *bin_ptr;
    ulong num_bin;
    ulong count;
    ulong ticks;
    ulong block_num;
    uchar *bin_addr;
    boolean skip = FALSE;

    if (profile_blocks) {
	if (!GETOBJ(int,2)) {	/* If verbose */
	    automore_enable(NULL);
	    printf("\nProfiling %sabled", profile_enabled ? "en" : "dis");
	    if (profile_hogs)
		printf(" (profiling CPU hogs)");
	    printf("\n");
	}
	for (block_num = 0, pro_ptr = profile_blocks; pro_ptr; 
	     pro_ptr = pro_ptr->next, block_num++) {
	    bin_ptr = pro_ptr->bins;
	    ticks = 0;
	    if (GETOBJ(int,2))	/* Terse */
		printf("\nPROF %x %x %x", pro_ptr->start_addr, 
		       pro_ptr->end_addr, 1 << pro_ptr->resolution);
	    else {
		printf("\nBlock %d: start = %x, end = %x, increment = %x, ",
		       block_num, pro_ptr->start_addr, pro_ptr->end_addr, 
		       1 << pro_ptr->resolution);
		switch (pro_ptr->type) {
		case PROFILE_CONFIG:
		    printf("CONF");
		    break;
		case PROFILE_EXEC:
		    printf("EXEC");
		    break;
		default:
		    break;
		}
	    }

	    bin_addr = (uchar *) pro_ptr->start_addr;
	    num_bin = bin_count(pro_ptr->start_addr, pro_ptr->end_addr,
				pro_ptr->resolution);

	    for (count = 0; count < num_bin; ticks += *bin_ptr, 
		 bin_addr += 1 << pro_ptr->resolution, count++, bin_ptr++) {
		if (GETOBJ(int,1) || GETOBJ(int,2)) { /* "detail" or "terse" */
		    if ((count & 0x7) == 0) { /* every eighth one */
			boolean lastskip = skip;
			int i;
			skip = TRUE;
			for (i=0; i<8; i++) {
			    if (i + count >= num_bin)
				break;
			    if (*(bin_ptr+i)) {
				skip = FALSE;
				break;
			    }
			}
			if (skip)
			    continue;
			if (lastskip) {
			    if (GETOBJ(int,2)) /* Terse */
				printf("\nPROFSKIP %x", bin_addr);
			    else
				printf("\n   ...");
			}
			if (GETOBJ(int,2)) { /* Terse */
			    printf("\n");
			} else {
			    printf("\n 0x%06x", bin_addr);
			}
		    }
		    if (!skip) {
			if (GETOBJ(int,2)) { /* Terse */
			    printf(" %d", *bin_ptr);
			} else {
			    if (*bin_ptr)
				printf(" %-6d", *bin_ptr);
			    else
				printf("      .");
			}
		    }
		}
	    }
	    if (!GETOBJ(int,2))	/* Verbose */
		printf("\nTotal = %d", ticks);
	}
	if (GETOBJ(int,2))	/* Terse */
	    printf("\nPROFTOT %d", profile_total);
	else
	    printf("\nSystem total = %d", profile_total);
	if (!GETOBJ(int,2))	/* If verbose */
	    automore_disable();
    }
}


/*
 * clear_profile_command - clear profile counters
 */

void clear_profile_command (parseinfo *csb)
{
    profile_clear_blocks();
}


/*
 * NV Generation for profiling
 */
void profile_nvgens(parseinfo *csb) {

    profile *pro_ptr;
    
    for (pro_ptr = profile_blocks; pro_ptr; pro_ptr = pro_ptr->next) {
	nv_write (pro_ptr->type == PROFILE_CONFIG, "%s %x %x %x", 
		  csb->nv_command, pro_ptr->start_addr, pro_ptr->end_addr,
		  1 << pro_ptr->resolution);
    }
}


/*
 * profile_command - configure profile
 *
 * profile startpc endpc resolution (exec or config)
 * unprofile startpc endpc resolution | all  (exec mode)
 * no profile [startpc endpc resolution] (config mode)
 * profile start | stop | hog (exec mode)
 *
 * OBJ(int,1) = TRUE if "all", FALSE if addresses present
 * OBJ(int,2) = start address
 * OBJ(int,3) = end address
 * OBJ(int,4) = increment
 * OBJ(int,5) = TRUE if "profile", FALSE if "unprofile" (if exec)
 * csb->which = PROFILE_CONFIG if config, PROFILE_EXEC if exec
 *            = PROFILE_START if "profile start"
 *            = PROFILE_STOP if "profile stop"
 *	      = PROFILE_HOG if "profile hog"
 * csb->sense valid if config
 *
 * (nvgens done in profile_nvgens, not here.)
 */

void profile_command (parseinfo *csb)
{
    ulong resolution;
    ulong increment;

    /* First check for "profile start | stop | hog". */

    switch (csb->which) {
      case PROFILE_START:
	profile_enabled = TRUE;
	profile_hogs = FALSE;
	return;

      case PROFILE_STOP:
	profile_enabled = FALSE;
	profile_hogs = FALSE;
	return;

      case PROFILE_HOG:
	profile_enabled = TRUE;
	profile_hogs = TRUE;
	return;
    }

    resolution = 0;
    increment = GETOBJ(int,4);

    /* Check parameters for consistency, if present. */

    if (!GETOBJ(int,1)) {	/* If parameters */
	if (GETOBJ(int,2) > GETOBJ(int,3)) {
	    printf("\n%%PROFILE: End address can't be less than start address");
	    return;
	}
	while (TRUE) {
	    increment >>= 1;
	    if (!increment)
		break;
	    resolution++;
	}
	if (GETOBJ(int,4) != (1 << resolution)) {
	    printf("\n%%PROFILE: increment not a power of 2; using 0x%x",
		   1 << resolution);
	}
    }

    if (((csb->which == PROFILE_EXEC) && GETOBJ(int,5)) ||
	(csb->which == PROFILE_CONFIG && csb->sense)) {/* if adding a block */

	/* Add new profile block. */

	if (!profile_add((void *)GETOBJ(int,2), (void *)GETOBJ(int,3), 
			 resolution, csb->which)) {
	    printf("\n%%PROFILE: Insufficient memory");
	}

    } else {			/* deleting */

	/* Delete either one profile block or all of them. */
	
	if (!GETOBJ(int,1)) {	/* If parameters */
	    if (!profile_delete((void *)GETOBJ(int,2), (void *)GETOBJ(int,3),
				resolution, csb->which)) {
		printf("\n%%PROFILE: No match");
		return;
	    }
	} else {		/* no parameters */
	    profile_delete_all();
	}
    }

    /* Enable/disable profiling as appropriate. */

    profile_enabled = (profile_blocks != NULL);
}

/*
 * Parse the "profile task" command
 *
 *  profile task [interrupt] [all] [PID1] [PID2] [PID3] ... [PID9]
 *  unprofile task
 *
 *  OBJ(int,1) = TRUE if "interrupt" specified
 *  OBJ(int,2) = number of PIDs specified (max 10)
 *  OBJ(int,3) = TRUE if "all" specified
 *  csb->uiarray[] = list of PIDs
 *  OBJ(int,5) = TRUE if "profile task", FALSE if "unprofile task"
 *
 */
void profile_task_command (parseinfo *csb)
{
    ulong pidnum;
    boolean good_pid;

    /* If "unprofile task", turn off the flag and clear all process flags. */

    if (!GETOBJ(int,5)) {
	selective_profile = FALSE;
	profile_interrupts = FALSE;
	process_set_all_profiles(FALSE);
	return;
    }

    /* "Profile task".  Check the parameters. */

    selective_profile = TRUE;
    profile_interrupts = GETOBJ(int,1);

    /* Set the appropriate task profiles. */

    if (GETOBJ(int,3)) {
	process_set_all_profiles(TRUE);
	return;
    }
    for (pidnum = 0; pidnum < GETOBJ(int,2); pidnum++) {
	good_pid = process_set_profile(csb->uiarray[pidnum], TRUE);
	if (!good_pid)
	    printf("\n%%PROFILE:  Bad PID number %d", 
		   csb->uiarray[pidnum]);
    }
}
