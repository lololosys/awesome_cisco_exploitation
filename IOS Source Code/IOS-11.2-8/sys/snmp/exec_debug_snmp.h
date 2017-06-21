/* $Id: exec_debug_snmp.h,v 3.3 1996/01/20 02:36:01 jjohnson Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/snmp/exec_debug_snmp.h,v $
 *------------------------------------------------------------------
 * exec_debug_snmp.h:  parse chains to control snmp debugging
 *
 * January 1996, Jeffrey T. Johnson
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_snmp.h,v $
 * Revision 3.3  1996/01/20  02:36:01  jjohnson
 * CSCdi47220:  need snmp packet debugging
 *
 * Revision 3.2  1996/01/17  19:46:12  jjohnson
 * CSCdi47027:  Need mechanism for debugging snmp timers
 *
 * Revision 3.1  1996/01/17  17:44:58  jjohnson
 * Initial placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * debug snmp packets
 *
 */
EOLS    (debug_snmp_packets_eol, debug_command, DEBUG_SNMP_PACKETS);
KEYWORD (debug_snmp_packets, debug_snmp_packets_eol, no_alt,
         "packets", "SNMP packets", PRIV_OPR);

/******************************************************************
 * debug snmp timers
 *
 */
EOLS    (debug_snmp_timers_eol, debug_command, DEBUG_SNMP_TIMERS);
KEYWORD (debug_snmp_timers, debug_snmp_timers_eol, debug_snmp_packets,
         "timers", "SNMP timer activity", PRIV_OPR);

/******************************************************************
 * debug snmp
 *
 */
KEYWORD_DEBUG(debug_snmp, debug_snmp_timers, ALTERNATE,
              OBJ(pdb,1), snmp_debug_arr,
              "snmp", "SNMP information", PRIV_OPR);

#undef  ALTERNATE
#define ALTERNATE       debug_snmp
