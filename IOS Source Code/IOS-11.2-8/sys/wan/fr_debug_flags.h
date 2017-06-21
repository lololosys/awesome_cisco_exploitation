/* $Id: fr_debug_flags.h,v 3.2.62.2 1996/05/31 18:19:26 vrawat Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_debug_flags.h,v $
 *------------------------------------------------------------------
 * fr_debug_flags.h -- Debugging flag declarations for Frame Relay
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1997 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fr_debug_flags.h,v $
 * Revision 3.2.62.2  1996/05/31  18:19:26  vrawat
 * CSCdi58846:  frame-relay debugging enhancements
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/04/25  23:23:16  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.2.66.1  1996/04/24  04:11:17  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.2  1995/11/17  18:02:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:17:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from Frame Relay code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(fr_debug_arr)

DEBUG_FLAG(frame_relay_debug,DEBUG_FRAME_RELAY,"Frame Relay")
DEBUG_FLAG(frame_relay_events_debug,DEBUG_FRAME_RELAY_EVENTS,"Frame Relay events")
DEBUG_FLAG(frlmi_debug,DEBUG_FRAME_RELAY_LMI,"Frame Relay LMI")
DEBUG_FLAG(frpkt_debug,DEBUG_FRAME_RELAY_PACKET,"Frame Relay packet")
DEBUG_FLAG(fr_tcpip_hdr_comp,DEBUG_FRAME_RELAY_TCPIP_COMP,
		"Frame Relay TCP/IP header compression")
DEBUG_FLAG(fr_de_debug,DEBUG_FRAME_RELAY_DE,"Frame Relay DE bit")
DEBUG_FLAG(fr_llc2_debug,DEBUG_FRAME_RELAY_LLC2,"LLC2 over Frame Relay")
DEBUG_FLAG(fr_rsrb_debug,DEBUG_FRAME_RELAY_RSRB,"RSRB over Frame Relay")
DEBUG_FLAG(fr_dlsw_debug,DEBUG_FRAME_RELAY_DLSW,"DLSW over Frame Relay")
DEBUG_FLAG(lapf_debug,DEBUG_FRAME_RELAY_LAPF,"Frame Relay SVC layer 2")
DEBUG_FLAG(fr_l3_debug, DEBUG_FRAME_RELAY_SVC_L3CC, "Frame Relay L3 stuff")
DEBUG_FLAG(fr_l3ie_debug, DEBUG_FRAME_RELAY_SVC_L3IE, "Frame Relay L3 IE stuff")
DEBUG_FLAG(fr_nli_debug, DEBUG_FRAME_RELAY_SVC_NLI, "Frame Relay NLI stuff")
DEBUG_FLAG(fr_svc_detd_debug, DEBUG_FRAME_RELAY_SVC_DETAILED,
	"Frame Relay SVC detailed")

DEBUG_ARRDONE
