/* $Id: rsvp_debug_flags.h,v 1.1.4.1 1996/04/17 13:57:19 fred Exp $
 * $Source: /release/112/cvs/Xsys/rsvp/rsvp_debug_flags.h,v $
 *------------------------------------------------------------------
 * Resource Reservation Protocol
 *
 * April 1996, Fred Baker
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsvp_debug_flags.h,v $
 * Revision 1.1.4.1  1996/04/17  13:57:19  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from IP routing code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/*
 * Now define the actual flags and the array that points to them.
 */
DEBUG_ARRDECL (rsvp_debug_arr)
DEBUG_FLAG (rsvp_debug, DEBUG_RSVP, "RSVP")
DEBUG_ARRDONE
