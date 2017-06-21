/* $Id: xremote_debug_flags.h,v 3.2 1995/11/17 19:26:59 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xremote/xremote_debug_flags.h,v $
 *------------------------------------------------------------------
 * xremote_debug_flags.h -- Debugging flag declarations for XRemote
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xremote_debug_flags.h,v $
 * Revision 3.2  1995/11/17  19:26:59  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:01:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:35:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from XRemote code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(xr_debug_arr)

DEBUG_FLAG(xrbsflow_debug,DEBUG_XRBSFLO,"XREMOTE bytestream flow")
DEBUG_FLAG(xrbsproto_debug,DEBUG_XRBSPRO,"XREMOTE bytestream protocol")
DEBUG_FLAG(xrdelta_debug,DEBUG_XRDELTA,"XREMOTE delta")
DEBUG_FLAG(xrfiles_debug,DEBUG_XRFILES,"XREMOTE file")
DEBUG_FLAG(xrflow_debug,DEBUG_XRFLOW,"XREMOTE client flow")
DEBUG_FLAG(xrpacket_debug,DEBUG_XRPACKET,"XREMOTE packet")
DEBUG_FLAG(xrreply_debug,DEBUG_XRREPLIES,"XREMOTE reply")
DEBUG_FLAG(xrrequest_debug,DEBUG_XRREQUESTS,"XREMOTE request")
DEBUG_FLAG(xrxdm_debug,DEBUG_XRXDM,"XREMOTE xdm client")

DEBUG_ARRDONE
