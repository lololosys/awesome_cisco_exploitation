/* $Id: sched_intval.c,v 3.5.12.3 1996/08/12 16:05:25 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/sched_intval.c,v $
 *------------------------------------------------------------------
 * Code supporting "scheduler-interval" configuration command
 *
 * February 1995, Carl Schaefer
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sched_intval.c,v $
 * Revision 3.5.12.3  1996/08/12  16:05:25  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.5.12.2  1996/05/21  09:58:24  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.12.1  1996/03/18  21:31:23  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.2  1996/03/07  10:19:47  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.12.1  1996/02/20  16:41:39  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/13  20:25:02  hampton
 * Move platform specific defines from common include files to platform
 * specific include files.  [CSCdi48844]
 *
 * Revision 3.4  1995/12/23  13:10:24  hampton
 * Fix a timing window where interfaces can be mistakenly throttled.
 * [CSCdi44951]
 *
 * Revision 3.3  1995/12/17  18:34:24  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:51:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:48:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:58:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "subsys.h"
#include "sched_private.h"
#include "config.h"
#include "sys_registry.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"

/*
 * The following hack is to get round a sapphire latency problem.  This
 * fix must be removed as soon as posable.  The value 2000 was determined
 * by trial and error.
 */
#if defined (SAPPHIRE)
#define DEFAULT_SCHED_INTERVAL  2000
#else
#define DEFAULT_SCHED_INTERVAL	0	/* current default -- none */
#endif

/*
 * scheduler_interval_command.
 * 	scheduler-interval <millisecond time>
 * Function to set/change the time interval that the scheduler is
 * required to run i.e. it will run peridically at this time interval.
 */
static void
scheduler_interval_command (parseinfo *csb)
{
    if (csb->nvgen) {
	nv_write((sched_run_interval != DEFAULT_SCHED_INTERVAL), 
		 "%s %d", csb->nv_command, sched_run_interval);
	return;
    }

    if (!csb->sense) {
	/*
	 * no scheduler-interval == the default interval.
	 */
	sched_run_interval = DEFAULT_SCHED_INTERVAL;
	return;
    }
    
    /*
     * Get the millisecond value.
     */
    sched_run_interval = GETOBJ(int,1);

    return;
}

/***************************************************************
 * [no] scheduler interval [<milliseconds>]
 */

EOLNS	(sched_int_eol, scheduler_interval_command);
NUMBER	(sched_int_value, sched_int_eol, no_alt,
	 OBJ(int, 1), MIN_SCHED_INTERVAL, MAXINT, "Milliseconds");

NVGENNS	(sched_int_nv, sched_int_value, scheduler_interval_command);
NOPREFIX (sched_int_no, sched_int_nv, sched_int_eol);

KEYWORD (sched_int, sched_int_no, NONE,
	 "interval", "Maximum interval before running lowest priority process",
	 PRIV_CONF);

LINK_POINT(sched_intval, sched_int);

/***************************************************************
 * scheduler-interval (for compatibility)
 */

KEYWORD_MM (osched_int, sched_int_value, NONE,
	    "scheduler-interval", NULL, PRIV_CONF|PRIV_HIDDEN|PRIV_NONVGEN,
	    11);

LINK_POINT(osched_intval, osched_int);



/*
 * sched_intval_ui_init
 *
 * Add the "scheduler interval" command to the parsechain and start
 * the timer.
 */

static void sched_intval_ui_init (subsystype *subsys)
{
    sched_run_interval = DEFAULT_SCHED_INTERVAL;
    TIMER_START(sched_run_time, sched_run_interval);

    parser_add_commands(PARSE_ADD_CFG_TOP_CMD, &pname(osched_intval),
			"scheduler-interval");
    parser_add_commands(PARSE_ADD_CFG_SCHED_CMD, &pname(sched_intval),
			"interval");
}

/*
 * sched_intval_ui subsystem header
 */
 
#define SCHED_INTVAL_UI_MAJVERSION    1
#define SCHED_INTVAL_UI_MINVERSION    0
#define SCHED_INTVAL_UI_EDITVERSION   1
 
SUBSYS_HEADER(sched_intval_ui,
	      SCHED_INTVAL_UI_MAJVERSION, SCHED_INTVAL_UI_MINVERSION,
              SCHED_INTVAL_UI_EDITVERSION,
              sched_intval_ui_init, SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);
