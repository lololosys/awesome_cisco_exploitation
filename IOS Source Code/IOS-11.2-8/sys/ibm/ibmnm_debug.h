/* $Id: ibmnm_debug.h,v 3.2.58.1 1996/08/28 12:46:38 thille Exp $
 * $Source: /release/112/cvs/Xsys/ibm/ibmnm_debug.h,v $
 *------------------------------------------------------------------
 * ibmnm_debug.h -- Debugging declarations for LAN Net Manager
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ibmnm_debug.h,v $
 * Revision 3.2.58.1  1996/08/28  12:46:38  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1995/11/17  09:22:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:46:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up LNM debugging */
extern void ibmnm_debug_init(void);

/*
 * externs in .c files are gross and disgusting.  Put them in a .h
 * file instead.
 */
extern boolean       lnm_e_debug;
extern boolean       lnm_llc_debug;

/*
 * The actual debugging flags are defined in ibmnm_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../ibm/ibmnm_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../ibm/ibmnm_debug_flags.h"
