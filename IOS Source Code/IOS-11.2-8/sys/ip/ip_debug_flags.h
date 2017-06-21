/* $Id: ip_debug_flags.h,v 3.2.58.1 1996/04/16 18:53:08 rchandra Exp $
 * $Source: /release/112/cvs/Xsys/ip/ip_debug_flags.h,v $
 *------------------------------------------------------------------
 * ip_debug_flags.h -- Debugging flag declarations for IP
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ip_debug_flags.h,v $
 * Revision 3.2.58.1  1996/04/16  18:53:08  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:33:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:55:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/18  19:35:39  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.1  1995/06/07  20:57:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from IP code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(ip_debug_arr)

DEBUG_FLAG(domain_debug,DEBUG_DOMAIN,"Domain Name System")
DEBUG_FLAG(icmppkt_debug,DEBUG_ICMPPKT,"ICMP packet")
DEBUG_FLAG(ippkt_debug,DEBUG_IPPKT,"IP packet")
DEBUG_FLAG(ipsecurity_debug,DEBUG_IPSECURITY,"IP security")
DEBUG_FLAG(ipmpkt_debug,DEBUG_IPMPKT,"IP multicast packets")
DEBUG_FLAG(iperror_debug,DEBUG_IPERROR,"IP packet errors")
DEBUG_FLAG(peer_debug,DEBUG_PEER,"IP peer address activity")
DEBUG_FLAG(ipnat_debug,DEBUG_NAT,"IP NAT")
DEBUG_FLAG(ipnat_detailed_debug,DEBUG_NAT_DETAILED,"IP NAT detailed")

DEBUG_ARRDONE
