/* $Id: sum.c,v 3.4.12.1 1996/05/10 23:53:19 gstovall Exp $
 * $Source: /release/112/cvs/Xsys/os/sum.c,v $
 *------------------------------------------------------------------
 * sum.c -- perform checksum over given range of addresses
 *
 * January 1989, Greg Satz
 *
 * Copyright (c) 1989-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sum.c,v $
 * Revision 3.4.12.1  1996/05/10  23:53:19  gstovall
 * CSCdi56792:  Need to remove unused files
 * Branch: California_branch
 * Nuke support for the Irix image, also stamp out the last of the CS500.
 *
 * Revision 3.4  1996/02/17  05:48:19  smaser
 * CSCdi46943:  break command in Rom Monitor causes image checksum
 * validation to fail
 *
 * Revision 3.3  1995/11/17  18:52:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:19:03  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:48:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/21  19:16:43  hampton
 * Make sure that text segment checksumming gets turned off for all
 * platforms where the text segment is read-only.  [CSCdi35788]
 *
 * Revision 2.1  1995/06/07 21:59:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sum.h"
#include "exception.h"
#include "sum_magic_number.h"
#include "subsys.h"
#include "sys_registry.h"
#include "region.h"
#include "../dev/monitor1.h"

#define SUSPEND_INTERVAL 100 		/* msec */
#define SUSPEND_LOOPCOUNT 20000 	/* loops per suspend check */

ulong MAGIC_VARIABLE = MAGIC_VALUE;
static boolean dont_validate = FALSE;

/*
 * Do the guts of the summation.  If "suspend" is true, we will
 * suspend periodically.  Note that the tests for "suspend" will
 * optimize out.  Love that compiler.
 */

static inline uint dosum_inline (void *start, void *end, boolean suspend)
{
    ulong sum;
    ulong *current;
    int nwords;
    int looplimit;
    sys_timestamp suspendtime;

    sum = 0;
    nwords = ((ulong)end - (ulong)start) / sizeof(ulong);
    current = (ulong *)start;
    if (suspend) {
	TIMER_START(suspendtime, SUSPEND_INTERVAL);
	while (TRUE) {
	    looplimit = SUSPEND_LOOPCOUNT;
	    while (looplimit--) {
		ADD_AROUND(*current++, sum);
		nwords--;
		if (!nwords)
		    return(sum);
	    }
	    if (AWAKE(suspendtime)) {
		process_suspend();
		TIMER_START(suspendtime, SUSPEND_INTERVAL);
	    }
	}
    } else {
	while (nwords--) {
	    ADD_AROUND(*current++, sum);
	}
    }
    return(sum);
}

/* Here's a version that just does the sum without suspending. */

uint checksum_text_segment (void)
{
    uint result = dosum_inline(TEXT_START,TEXT_END,FALSE);
    
    if (result == MAGIC_VALUE) {
	result++;
    }
    return(result);
}

/*
 * show_sum
 * Show current checksum of text segment
 */

void show_sum (parseinfo *csb)
{
    uint newsum;

    newsum = dosum_inline(TEXT_START,TEXT_END,TRUE);
    if (newsum == MAGIC_VALUE) {
        newsum++;
    }
 
    if (MAGIC_VARIABLE == MAGIC_VALUE) {
	printf("\nChecksumming disabled for this image");
    } else if (newsum != textsum) {
	printf("\nWarning: original checksum of %#x does not match new checksum of %#x",
	       textsum, newsum);
    } else {
	printf("\nNew checksum of %#x matched original checksum",  newsum);
    }
}

/*
 * validate_sum - crash if text segment has been corrupted
 */
void validate_sum(void)
{
    uint result;
    if (dont_validate || (mon_sys_chksum_disable() == TRUE)) {
	return;
    }
    result = dosum_inline(TEXT_START,TEXT_END,TRUE);
    if (result == MAGIC_VALUE) {
	result++;
    }

    if ((MAGIC_VARIABLE != MAGIC_VALUE) && (result != textsum)) {
	printf("\nCode checksum failed--memory corruption likely");
	crashdump(5);
    }
}

/*
 * defeat_sum - disable checksumming.  This should only be called
 * by GDB initialization, since we may be planting breakpoints, which
 * look like corruption!
 * NOTE: Any rom monitor based debuggers which plant break points
 * also need to do this. To do this, the platform's rom monitor
 * needs to have the EMT call EMT_DONT_VALIDATE implemented.
 */
void defeat_sum(void)
{
    dont_validate = TRUE;
}

/*
 * check_compile_time_sum
 *
 * Check to see that the system checksum equals the value
 * calculated when the system was built.
 *
 * This is called once when the system first starts up.
 */
void check_compile_time_sum(void)
{

    /* Don't do it if we're not supposed to. */

    if (dont_validate)
	return;

    /* Issue a warning if the compile-time checksum wasn't done. */

    if (MAGIC_VARIABLE == MAGIC_VALUE) {
	printf("\nWarning:  the compile-time code checksum does not "
	       "appear to be present.");
    } else {

	/* Crash and burn if the checksum is wrong. */

	if (checksum_text_segment() != MAGIC_VARIABLE) {
	    printf("\nCode checksum failed--corrupted system image!");
	    crashdump(5);
	}
    }
}
/*
 * sum_region()
 * routine to take a region and do a checksum on it as per the
 * algorithm specified in the cksum_type field. Upon failure the
 * value 0 is returned, success returns the generated checksum.
 */
static unsigned long sum_region (int cksum_type, uchar *buff, 
			  int bufflen,int suspend_flag)
{
    switch(cksum_type) {
      case SIMPLE_CHECKSUM_ALGORITHM:
	return(dosum_inline(buff,(buff+bufflen),suspend_flag));
	break;

      default:
	return(0);
	break;
    }
    return(0);
}

/*
 * sum_values()
 * this routine sums together two values as per the checksumming algorithm
 * specified.
 */
static unsigned long sum_values (int cksum_type, ulong value1, ulong value2)
{
    switch(cksum_type) {
      case SIMPLE_CHECKSUM_ALGORITHM:
	ADD_AROUND(value1, value2);
	return(value2);

      default:
	return(0);
    }
    return(0);
}

void sum_init(void)
{
    /*
     * Add registry calls
     */
    reg_add_sum_region(sum_region, "sum_region");
    reg_add_sum_values(sum_values, "sum_values");
}
