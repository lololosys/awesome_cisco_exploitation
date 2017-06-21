/* $Id: snmp_debug.h,v 3.3 1996/01/20 02:36:03 jjohnson Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/snmp/snmp_debug.h,v $
 *------------------------------------------------------------------
 * snmp_debug.h:  Debugging declarations for SNMP
 *
 * January 1996, Jeffrey T. Johnson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: snmp_debug.h,v $
 * Revision 3.3  1996/01/20  02:36:03  jjohnson
 * CSCdi47220:  need snmp packet debugging
 *
 * Revision 3.2  1996/01/17  19:46:13  jjohnson
 * CSCdi47027:  Need mechanism for debugging snmp timers
 *
 * Revision 3.1  1996/01/17  17:44:59  jjohnson
 * Initial placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up SNMP debugging */
extern void snmp_debug_init(void);

/* Function to print an SNMP message */
extern void snmp_print_message(Pdu *message);

/*
 * The actual debugging flags are defined in snmp_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../snmp/snmp_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../snmp/snmp_debug_flags.h"
