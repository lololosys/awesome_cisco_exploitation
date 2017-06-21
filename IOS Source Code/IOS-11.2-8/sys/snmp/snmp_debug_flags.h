/* $Id: snmp_debug_flags.h,v 3.3 1996/01/20 02:36:04 jjohnson Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/snmp/snmp_debug_flags.h,v $
 *------------------------------------------------------------------
 * snmp_debug_flags.h:  Debugging flag declarations for SNMP
 *
 * January 1996, Jeffrey T. Johnson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: snmp_debug_flags.h,v $
 * Revision 3.3  1996/01/20  02:36:04  jjohnson
 * CSCdi47220:  need snmp packet debugging
 *
 * Revision 3.2  1996/01/17  19:46:14  jjohnson
 * CSCdi47027:  Need mechanism for debugging snmp timers
 *
 * Revision 3.1  1996/01/17  17:45:00  jjohnson
 * Initial placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from SNMP code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(snmp_debug_arr)

DEBUG_FLAG(snmp_packet_debug, DEBUG_SNMP_PACKETS, "SNMP packet")
DEBUG_FLAG(snmp_timer_debug, DEBUG_SNMP_TIMERS, "SNMP timer")

DEBUG_ARRDONE
