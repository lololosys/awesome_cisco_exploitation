/* $Id: trace.c,v 3.2.62.1 1996/03/18 22:29:33 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/ui/trace.c,v $
 *------------------------------------------------------------------
 * trace.c  -- routines for traceroute
 *  
 * Oct, 1989. Paulina Knibbe
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: trace.c,v $
 * Revision 3.2.62.1  1996/03/18  22:29:33  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/14  01:36:32  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.2.26.1  1996/02/20  21:26:46  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  17:49:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:41:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:11:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "ttysrv.h"
#include "config.h"

#define TRACE_CHARS 40


/*
 * trace_command
 * Handle the network level traceroute commands
 */

void trace_command (parseinfo *csb)
{
    int index, hint;
    char buff[TRACE_CHARS];
    char *args, *cp;
    boolean host_supplied, running;
    
    /*
     * Check for command line arguments.  If there was no recognizable
     * "hint", LINK_ILLEGAL is returned.
     */
    args = GETOBJ(string,1);
    hint = GETOBJ(int,1);
    host_supplied = (args && *args);
    if (host_supplied || (hint != LINK_ILLEGAL)) {
	/*
	 * SERVICE_PING_PARSE figures out what protocol and address may
	 * have been specified on the command line.  It returns true on
	 * the first positive match, false if no match at all.
	 */
	psion(stdio);
	if (!reg_invoke_ping_parse(args, hint, &index, &cp, &running)) {
	    psioff(stdio);
	    printf("%% Unrecognized host or address.\n");
	    return;
	}
	psioff(stdio);
    } else {
	
	/*
	 * No arguments supplied, so prompt the user.
	 * This time a negative hint indicates that "buff" should contain
	 * a protocol name instead of a protocol address.
	 */
	printf("Protocol [ip]: ");
	if (rdtty((char *)buff, TRACE_CHARS) == FALSE)
	    return;
	if (!reg_invoke_ping_parse(buff, -1, &index, &cp, &running)) {
	    printf("%% Unknown protocol - \"%s\", type \"trace ?\" for help",
		   buff);
	    return;
	}
	args = "";
    }
    reg_invoke_trace_run(index, args);
}

/*
 * trace_common
 * This routine does the common parsing of IP traceroute and CLNS traceroute 
 */

boolean trace_common (
    char buff[TRACE_CHARS],
    ulong *seconds,
    ushort *nprobes,
    ushort *minttl,
    ushort *maxttl,
    int maxttlallowed)
{
    *seconds =         get_num_in_range("Timeout in seconds", 10,
					1, 0xFFFFFFFFL, *seconds);
    *nprobes = (ushort)get_num_in_range("Probe count", 10,
					1, 0xFFFF, *nprobes);
    *minttl  = (ushort)get_num_in_range("Minimum Time to Live", 10,
					0, 0xFF, *minttl);
    *maxttl  = (ushort)get_num_in_range("Maximum Time to Live", 10, 
					*minttl, maxttlallowed, *maxttl);
    return(TRUE);
}
