/* $Id: rtt_debug_flags.h,v 1.1.4.2 1996/06/07 22:26:03 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/rtt/rtt_debug_flags.h,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor parse chain header file.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1996,1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This header file contains the  definitions for
 * the RTT Monitor debug parse chain.
 *------------------------------------------------------------------
 * $Log: rtt_debug_flags.h,v $
 * Revision 1.1.4.2  1996/06/07  22:26:03  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/17  11:41:07  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.1  1996/03/22  18:41:34  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:26  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(rttmon_debug_arr)

DEBUG_FLAG(rttmon_debug_error, DEBUG_RTTMON_ERROR, "RTR Error")
DEBUG_FLAG(rttmon_debug_trace, DEBUG_RTTMON_TRACE, "RTR Trace")

DEBUG_ARRDONE
