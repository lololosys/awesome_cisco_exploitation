/* $Id: profile.h,v 3.2.62.1 1996/03/18 21:30:59 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/os/profile.h,v $
 *------------------------------------------------------------------
 * Declarations for profiling support
 *
 * Dave Katz, December 1992
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: profile.h,v $
 * Revision 3.2.62.1  1996/03/18  21:30:59  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/03/09  05:10:12  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2  1995/11/17  18:50:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:47:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:57:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _PROFILE_H_
#define _PROFILE_H_

/*
 * Data structure for profiling.  Basically a linked list of blocks
 * of bins.
 */

typedef struct profile_ {
    struct profile_ *next;	/* Link to next block */
    void *start_addr;		/* Starting address to profile */
    void *end_addr;		/* Ending address to profile */
    ulong resolution;		/* Log2 of resolution */
    ulong type;			/* PROFILE_CONFIG or PROFILE_EXEC */
    ulong bins[0];		/* Statistics bins */
} profile;

enum {
    PROFILE_CONFIG,			/* Profile configured */
    PROFILE_EXEC,			/* Profile added by EXEC command */
    PROFILE_START,			/* "profile start" command */
    PROFILE_STOP,			/* "profile stop" command */
    PROFILE_HOG				/* "profile hog" command */
};

/* External declarations */

extern profile *profile_blocks;		/* Linked list of profile blocks */
extern ulong profile_total;		/* Total number of calls */
extern boolean profile_enabled;		/* TRUE if profiling is enabled */
extern boolean selective_profile;	/* TRUE if profiling selected tasks */
extern boolean profile_interrupts;	/* TRUE if profiling interrupt state */
extern boolean profile_hogs;		/* TRUE if profiling CPU hogs */
extern void profile_clear_blocks(void);

/* Inline declarations */

/*
 * profile_check_for_hogs
 *
 * Returns TRUE if we are actively profiling CPU hogs
 */
static inline boolean profile_check_for_hogs (void)
{
    return(profile_hogs && profile_enabled);
}

/*
 * profile_hog_occurred
 *
 * Note the fact that a CPU hog occurred, as necessary
 */
static inline void profile_hog_occurred (void)
{
    if (profile_hogs)			/* If we are profiling hogs */
	profile_enabled = FALSE;	/* Stop picking up profile info */
}

/*
 * bin_count - return the bin count for a given address range and resolution
 */
static inline ulong bin_count (
    void *start, 
    void *end, 
    ulong resolution)
{
    ulong count;
    count = (((ulong)end - (ulong)start) >> resolution) + 1;
    return(count);
}

#endif /* PROFILE_H */
