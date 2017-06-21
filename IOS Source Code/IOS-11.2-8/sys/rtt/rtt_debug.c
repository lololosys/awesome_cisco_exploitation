/* $Id: rtt_debug.c,v 1.1.4.3 1996/06/07 22:25:59 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/rtt/rtt_debug.c,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor Utilities.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This file contains the RTT Monitor utilities.
 *------------------------------------------------------------------
 * $Log: rtt_debug.c,v $
 * Revision 1.1.4.3  1996/06/07  22:25:59  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.2  1996/06/04  18:28:24  lmetzger
 * CSCdi59459:  See CSCdi589xx, xx = 29,30,31,32,33,35,36,47
 * Branch: California_branch
 * - Group of defects that are interdependent and tested.
 *
 * Revision 1.1.4.1  1996/05/17  11:41:01  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.2  1996/04/19  02:41:22  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 1.1.2.1  1996/03/22  18:41:32  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:25  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "config.h"
#include "../ui/debug.h"
#include "rtt_debug.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "rtt_debug_flags.h"

#include "rtt_util_ext.h"

void rttmon_debug_command (parseinfo * csb)
{
    boolean entryNumberExists;
    long entryNumber;

    entryNumberExists = GETOBJ(int, 2);

    if (entryNumberExists) {
	entryNumber = GETOBJ(int, 1);
    } else {
	entryNumber = -1;
    }
    switch (csb->which) {
    case DEBUG_RTTMON_ERROR:
	turn_rtt_debug(csb->sense,
		       entryNumber);
	break;
    case DEBUG_RTTMON_TRACE:
	turn_rtt_trace(csb->sense,
		       entryNumber);
	break;
    default:
	debug_command(csb);
    } /* switch (csb->which) */ 
}	

/*
 * rtt_debug_all (boolean flag)
 *
 *   Sets state of all RTT debug flags to "on" or "off"
 * 
 *   Registered to be called whenever "debug all" or "undebug all" 
 *     or "no debug all" commands are issued by the user.
 *
 *   Input parm "flag" = TRUE to indicate "debug all" command issued
 *   Input parm "flag" = FALSE to indicate "undebug all" or
 *                          "no debug all" command issued
 *
 */ 
static void rtt_debug_all (boolean flag)
{
    /*
     *  The second parameter of -1 is for all.
     */
    turn_rtt_debug(flag, -1);
    turn_rtt_trace(flag, -1);
    
    return;
} /* End rtt_debug_all () */

/* 
 * rtt_debug_show
 *
 *   Display the values of RTT debugging variables
 */
static void rtt_debug_show (void)
{
    int flagNo;
    int outNum;

    if (RTT_ERROR_FLAG || RTT_TRACE_FLAG) {
	printf("RTR:\n");
    }
    if (RTT_ERROR_FLAG) {
	printf("  ERROR debugging is on for entries:\n");
	printf("    ");
	outNum = 0;
	for (flagNo = 0; flagNo < MAXULONG_BITS; flagNo++) {
	    if ((RTT_ERROR_FLAG & (1 << flagNo)) == (1 << flagNo)) {
		if (outNum > 60) {	/* max line size */
		    printf("\n");
		    outNum = 0;
		}
		printf("%u ", flagNo);
		outNum += 4;		/* output size max */
	    }
	}
	printf("\n");
    }
    if (RTT_TRACE_FLAG) {
	printf("  TRACE debugging is on for entries:\n");
	printf("    ");
	outNum = 0;
	for (flagNo = 0; flagNo < MAXULONG_BITS; flagNo++) {
	    if ((RTT_TRACE_FLAG & (1 << flagNo)) == (1 << flagNo)) {
		if (outNum > 60) {	/* max line size */
		    printf("\n");
		    outNum = 0;
		}
		printf("%u ", flagNo);
		outNum += 4;		/* output size max */
	    }
	}
	printf("\n");
    }
    return;
} 

void rtt_init_debug (void)
{
    /* Register for "debug all" and "show debug" events */
    /* ------------------------------------------------ */
    reg_add_debug_all(rtt_debug_all, "rtt_debug_all");
    reg_add_debug_show(rtt_debug_show, "rtt_debug_show");

    return;
}
