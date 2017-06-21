/* $Id: stacks_sp_chain.c,v 3.1.20.1 1996/03/18 22:29:51 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/unix/stacks_sp_chain.c,v $
 *------------------------------------------------------------------
 * stacks_sp_chain.c -- SPARC specific stack parse routines
 *  
 * June 1985, Kirk Lougheed
 * October 1993, Michael Beesley
 *
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stacks_sp_chain.c,v $
 * Revision 3.1.20.1  1996/03/18  22:29:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.10.1  1996/03/09  05:14:27  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.1  1996/02/21  21:52:38  hampton
 * Placeholders for Port Ready.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../os/sched_private.h"
#include "ttysrv.h"

/*
 * stack_printtrace
 * Hardware specific printing of a stack trace called by stack_backtrace()
 */
void 
stack_printtrace (sprocess *p)
{
    return;
}
