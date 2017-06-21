/* $Id: fddi_debug_flags.h,v 3.2 1995/11/17 09:27:45 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/fddi_debug_flags.h,v $
 *------------------------------------------------------------------
 * fddi_debug_flags.h -- Debugging flag declarations for FDDI
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1995 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fddi_debug_flags.h,v $
 * Revision 3.2  1995/11/17  09:27:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:49:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/13  03:28:12  alam
 * CSCdi35570:  Remove extraneous functions
 * Use llc_process_test and llc_process_xid for FDDI as well as ethernet
 * and FR.
 * Remove FDDI packet debug.
 *
 * Revision 2.1  1995/06/07  20:52:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from FDDI code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(fddi_debug_arr)

DEBUG_FLAG(fddi_cmt_events_debug,DEBUG_FDDI_CMT_EVENTS,"CMT events")
DEBUG_FLAG(fddi_debug,DEBUG_FDDI,"FDDI event")
DEBUG_FLAG(fddi_smt_packet_debug,DEBUG_FDDI_SMT_PACKET,"SMT packets")
DEBUG_ARRDONE
