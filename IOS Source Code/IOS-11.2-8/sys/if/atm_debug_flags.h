/* $Id: atm_debug_flags.h,v 3.3 1996/01/31 00:18:21 rzagst Exp $
 * $Source: /release/111/cvs/Xsys/if/atm_debug_flags.h,v $
 *------------------------------------------------------------------
 * if/atm_debug.h: DEBUG command specific vars. & constants
 *
 * November, 1993, Rick Li Fo Sjoe
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Longer description here.
 *------------------------------------------------------------------
 * $Log: atm_debug_flags.h,v $
 * Revision 3.3  1996/01/31  00:18:21  rzagst
 * CSCdi46580:  debug atm packet needs to filter on an interface or vc
 * basis
 *
 * Revision 3.2  1995/11/17  09:25:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:47:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:50:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(atm_debug_arr)

DEBUG_FLAG(atm_packet_debug,DEBUG_ATMPACKET,"ATM packets")
DEBUG_FLAG(atm_event_debug,DEBUG_ATMEVENTS,"ATM events")
DEBUG_FLAG(atm_errors_debug,DEBUG_ATMERRORS,"ATM errors")
DEBUG_FLAG(atm_oam_debug,DEBUG_ATMOAM,"ATM OAM cells")
DEBUG_FLAG(atm_arp_debug, DEBUG_ATMARP, "ATM ARP events")
DEBUG_FLAG(atm_ilmi_debug, DEBUG_ATMILMI, "ATM ILMI events")
DEBUG_FLAG(atm_interface_debug,DEBUG_ATM_INTERFACE,"ATM_Interface")
DEBUG_FLAG(atm_vc_debug,DEBUG_ATM_VC,"ATM VCD Number")

DEBUG_ARRDONE
