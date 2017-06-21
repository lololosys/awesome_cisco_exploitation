/* $Id: srt_debug_flags.h,v 3.2 1995/11/17 18:57:28 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/srt/srt_debug_flags.h,v $
 *------------------------------------------------------------------
 * srt_debug_flags.h -- Debugging flag declarations for SRT
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1995 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: srt_debug_flags.h,v $
 * Revision 3.2  1995/11/17  18:57:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:32:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:02:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from SRT code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(srt_debug_arr)

DEBUG_FLAG(access_expr_debug,DEBUG_ACCESS_EXPR,"Boolean access expression")
DEBUG_FLAG(lack_debug_packet,DEBUG_LACK_PACKET,"Local Acknowledgement packets")
DEBUG_FLAG(lack_debug_err,DEBUG_LACK_ERR,"Local Acknowledgement errors")
DEBUG_FLAG(lack_debug_state,DEBUG_LACK_STATE,"Local Acknowledgement states")
DEBUG_FLAG(span_debug,DEBUG_SPAN,"Spanning Tree BPDU")
DEBUG_FLAG(spanevents_debug,DEBUG_SPANEVENTS,"Spanning Tree event")
DEBUG_FLAG(srb_debug,DEBUG_SRB,"Source Bridge")
DEBUG_FLAG(srbevent_debug,DEBUG_SRBEVENT,"Source Bridge Event")
DEBUG_FLAG(srberr_debug,DEBUG_SRBERROR,"Source Bridge Error")
DEBUG_FLAG(bridge_lat_debug,DEBUG_BRIDGELAT,"Bridge LAT filtering")
DEBUG_FLAG(bridge_latpkt_debug,DEBUG_BRIDGELATPKT,"Bridged LAT announcements")

DEBUG_ARRDONE
