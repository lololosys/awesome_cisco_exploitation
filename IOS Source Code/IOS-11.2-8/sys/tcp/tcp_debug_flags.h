/* $Id: tcp_debug_flags.h,v 3.2 1995/11/17 18:48:37 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/tcp/tcp_debug_flags.h,v $
 *------------------------------------------------------------------
 * tcp_debug_flags.h -- Debugging flag declarations for TCP
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1995 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tcp_debug_flags.h,v $
 * Revision 3.2  1995/11/17  18:48:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:35:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/25  08:35:24  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.1  1995/06/07  23:05:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from TCP code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(tcp_debug_arr)

DEBUG_FLAG(tcp_debug,DEBUG_TCP,"TCP special event")
DEBUG_FLAG(tcprcmd_debug,DEBUG_TCPRCMD,"RCMD transactions")
DEBUG_FLAG(tcpftp_debug,DEBUG_TCPFTP,"FTP transactions")
DEBUG_FLAG(tcppkt_debug,DEBUG_TCPPKT,"TCP Packet")
DEBUG_FLAG(thc_debug,DEBUG_THC,"TCP header compression")
DEBUG_FLAG(tcpdrv_debug,DEBUG_TCPD,"TCP driver event")
DEBUG_FLAG(tcpdrvpkt_debug,DEBUG_TCPDP,"TCP driver verbose")

DEBUG_ARRDONE
