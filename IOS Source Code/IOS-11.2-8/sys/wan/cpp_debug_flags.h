/* $Id: cpp_debug_flags.h,v 3.1.2.1 1996/05/10 01:44:25 wmay Exp $
 * $Source: /release/112/cvs/Xsys/wan/cpp_debug_flags.h,v $
 *------------------------------------------------------------------
 * cpp_debug_flags.h -- Debugging flag declarations for CPP
 *
 * October, 1995, Bill May
 *
 * Copyright (c) 1995-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cpp_debug_flags.h,v $
 * Revision 3.1.2.1  1996/05/10  01:44:25  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/05/09  17:24:38  wmay
 * Placeholder for California branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
 * We get included (multiple times) from cpp code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(cpp_debug_arr)


DEBUG_FLAG(cpp_event_debug,DEBUG_CPP_EVENT,
	   "cpp protocol events")

DEBUG_FLAG(cpp_negotiation_debug,
	   DEBUG_CPP_NEGOTIATION,
	   "cpp protocol negotiation")

DEBUG_FLAG(cpp_packet_debug,
	   DEBUG_CPP_PACKET,
	   "cpp packet display")

DEBUG_ARRDONE
