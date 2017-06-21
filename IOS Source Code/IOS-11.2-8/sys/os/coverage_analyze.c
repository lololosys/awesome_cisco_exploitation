/* $Id: coverage_analyze.c,v 3.3.18.1 1996/03/18 21:29:25 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/os/coverage_analyze.c,v $
 *------------------------------------------------------------------
 * coverage_analyze - Coverage analysis
 *
 * June 10, 1993, Kevin Paul Herbert
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This routine contains coverage analysis support.
 *
 *------------------------------------------------------------------
 * $Log: coverage_analyze.c,v $
 * Revision 3.3.18.1  1996/03/18  21:29:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/07  10:18:08  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.24.1  1996/02/20  16:38:14  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/07  16:14:19  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.2  1995/11/17  18:46:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/18  06:48:06  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.2  1995/06/19 06:48:41  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:54:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_defs_coverage.h"

typedef struct {
    void *pc;			/* PC from trace */
    leveltype level;		/* Priority at time of trace */
    void *caller_pc;		/* Caller's caller */
    ulong repeat;		/* Repeat count */
    sys_timestamp firsttime;	/* Time this entry was added */
    sys_timestamp lasttime;	/* Time routine was last called */
    const char *process_name;	/* Pointer to process name */
} coverage_trace;

coverage_trace (*coverage_trace_table)[]; /* Pointer to trace table */
ulong coverage_trace_size;	/* Size of trace table */
ulong coverage_current_trace;	/* Current trace cell in vector */
ulong coverage_wraps;		/* Count of times wraps have occured */
ulong coverage_pause;		/* Current "pause" counter */
ulong coverage_pause_missed;	/* How many we missed because we were paused */

/*
 * Create a "prototype" for the pointer used by the assembly language
 * hook. Note that no C code other than this module actual references
 * this vector.
 */

extern void (*mcount_handler)(ulong *, void *, leveltype, void *);

/*
 * Make external references to the pointers defined in the assembly
 * language mcount pointers. These variables should not be referred to
 * by any other module, so they are here...
 */

extern ulong mcount_count;	/* Count of times mcount handler called */
extern ulong mcount_recurse_count; /* Count of recursion supression */

/*
 * coverage_mcount - "mcount" interface for the coverage analyzer
 *
 * This routine is used to trace the last N routines called. It uses
 * the "mcount" hook, so any modules must be compiled with the "-p"
 * option in order to be counted.
 */

static void coverage_mcount (ulong *trace, void *pc, leveltype level, void *caller_pc)
{
    coverage_trace *entry;	/* Pointer to current entry */

    if (!coverage_trace_table)
      return;			/* Nothing to do if no trace counter */

    if (coverage_pause) {	/* If temporarily paused */
	coverage_pause_missed++; /* Count it */
	return;			/* Get out */
    }

    /*
     * Get a pointer to the current trace entry
     */

    entry = &((*coverage_trace_table)[coverage_current_trace]);

    /*
     * If this is a repeat, update the repeat count and last time indicator
     */

    if (entry->pc == pc) {
	entry->repeat++;	/* Count the repeat */
	GET_TIMESTAMP(entry->lasttime); /* Save the time */
    } else {

	/*
	 * Not a repeat, set up a new entry
	 */

	if (++coverage_current_trace == coverage_trace_size) {
	    coverage_current_trace = 0; /* Handle wraps */
	    coverage_wraps++;	/* Count the wrap */
	}

	/*
	 * Get the new entry pointer
	 */

	entry = &((*coverage_trace_table)[coverage_current_trace]);


	entry->pc = pc;		/* Set the PC */
	entry->level = level;	/* And the called level */
	entry->caller_pc = caller_pc; /* Set the caller's caller */
	entry->repeat = 1;	/* Initialize the repeat count */
	GET_TIMESTAMP(entry->firsttime); /* Set the first time called */
	GET_TIMESTAMP(entry->lasttime); /* And the last time called */
	process_get_name(THIS_PROCESS, &entry->process_name); /* And the process name */

	*trace = coverage_current_trace; /* Save trace number */
    }
}

/*
 * coverage_config_command - Configuration support
 */

void coverage_config_command (parseinfo *csb)
{
    coverage_trace (*temp_table)[]; /* Temporary table pointer */
    ulong table_size;		/* Table size */

    if (csb->nvgen) {
	switch (csb->which) {
	  case COVERAGE_HISTORY:
	    nv_write((coverage_trace_table != NULL), "%s %d",
		     csb->nv_command, coverage_trace_size);
	    break;
	}
	return;
    }
	  
    switch (csb->which) {
      case COVERAGE_HISTORY:

	/*
	 * If there is an existing table, eliminate it
	 */

	mcount_handler = NULL;	/* Ensure no handler enabled */
	temp_table = coverage_trace_table; /* Remember table pointer */
	coverage_trace_table = NULL; /* Clear table pointer */

	if (temp_table)
	  free(temp_table);	/* Free a table, if there was one */

	/*
	 * Allocate a table at the new size. If the size is zero,
	 * there is nothing to do.
	 */

	table_size = GETOBJ(int, 1); /* Get table size */
	if (!table_size)
	  return;		/* Zero, nothing to do */

	temp_table = (coverage_trace (*)[])
	  malloc(sizeof(coverage_trace) * table_size);
	if (!temp_table) {
	    printf(nomemory);
	    return;
	}

	coverage_trace_size = table_size; /* Set the table size */
	coverage_current_trace = 0; /* Initialize current trace */
	coverage_wraps = 0;	/* No wraps so far */
	coverage_trace_table = temp_table;	/* Set pointer to table */

	mcount_handler = coverage_mcount; /* Set the mcount handler */

	return;			/* All done */

    }
}

/*
 * coverage_show_trace_entry - Show a trace entry
 *
 * This routine is used to give a formatted display of a trace entry.
 * It is a helper routine used by other display routines.
 */

static const char coverage_trace_header[] =
"   PC     Caller  Repeat PRI First Time  Last Time         Process Name";

static const char coverage_trace_format[] =
"\n%08x %08x %-6d   %1d %-10d %-10d %-20s";

static void coverage_show_trace_entry (coverage_trace *entry)
{
    printf(coverage_trace_format,
	   entry->pc,
	   entry->caller_pc,
	   entry->repeat,
	   entry->level,
	   entry->firsttime,
	   entry->lasttime,
	   entry->process_name);
}

/*
 * coverage_show_status - Show the status of the coverage system
 */

static void coverage_show_status (void)
{
    printf("\nHistory table size is %d entries. ", coverage_trace_size);
    if (coverage_wraps) {
	printf("Entries have wrapped %d times.", coverage_wraps);
    } else {
	printf("%d entries have been used.", coverage_current_trace);
    }
	   
    printf("\nLow-level count handler has been called %d times."
	   "\nThere were %d entries not traced due to recursion detection."
	   "\nThere were %d entries not traced due to internal pauses.",
	   mcount_count, mcount_recurse_count, coverage_pause_missed);

}

/*
 * coverage_show_command - Handle the "show coverage" command
 */

void coverage_show_command (parseinfo *csb)
{
    ulong first;		/* First/current entry to display */
    ulong quant;		/* Number of entries to show */
    ulong i;			/* Loop counter */

    /*
     * Ensure that tracing is enabled
     */

    if (!coverage_trace_table) {
	printf("\n%% Coverage history is not enabled");
	return;
    }

    coverage_pause++;		/* Pause collection while we run */

    /*
     * Set up our initial context. The "first" entry to display is
     * entry #1 if we have not wrapped, or the current entry + 1 if
     * we have wrapped. The number of entries to display is the
     * size of the table if we have wrapped, or it is the current
     * trace number if we haven't wrapped.
     */

    first = coverage_wraps ? coverage_current_trace + 1 : 1;
    quant = coverage_wraps ? coverage_trace_size : coverage_current_trace;

    /*
     * Dispatch based on the command
     */

    switch (csb->which) {

      case COVERAGE_ALL:

	/*
	 * They want to see the entire table, so we're already set up
	 */

	break;

      case COVERAGE_FIRST:

	/*
	 * They want to see the first "n" entries. So, we'll need to
	 * adjust the number of entries to display, if they entered
	 * a number that is less than the total we have already
	 * calculated.
	 */

	if (GETOBJ(int, 1) < quant)
	  quant = GETOBJ(int, 1);
	break;

      case COVERAGE_LAST:

	/*
	 * They want to see the last "n" entries. In order to do this,
	 * we will adjust both the quantity to display and the first
	 * entry.
	 */

	if (GETOBJ(int, 1) < quant)
	  quant = GETOBJ(int, 1);

	/*
	 * Calculate the starting trace number, taking wrapping into
	 * consideration.
	 */

	if (quant <= coverage_current_trace)
	  first = (coverage_current_trace + 1) - quant;
	else
	  first = coverage_trace_size - (quant - (coverage_current_trace + 1));

	break;

      case COVERAGE_STATUS:

	/*
	 * Give the status report and get out
	 */

	coverage_show_status();	/* Report status */
	coverage_pause--;	/* Resume statistics */
	return;			/* And get out */

      default:

	bad_parser_subcommand(csb, csb->which);
	coverage_pause--;	/* Resume statistics */
	return;

    }

    automore_enable(NULL);	/* Enable auto-more */

    coverage_show_status();	/* Report the summary */
    
    printf("\n");		/* Make a seperator line */

    automore_header(coverage_trace_header); /* Set new header */

    for (i = 0; (i < quant) && !automore_quit() ; i++) {

	/* Handle wraps */

	if (first == coverage_trace_size)
	  first = 0;

	coverage_show_trace_entry((*coverage_trace_table) + first);

	first++;

    }

    automore_disable();
    coverage_pause--;
}
