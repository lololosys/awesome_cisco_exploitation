/* $Id: vines_debug_flags.h,v 3.2 1995/11/17 17:57:41 hampton Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/vines/vines_debug_flags.h,v $
 *------------------------------------------------------------------
 * vines_debug_flags.h -- Debugging flag declarations for VINES
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1994 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_debug_flags.h,v $
 * Revision 3.2  1995/11/17  17:57:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:44:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:14:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from VINES code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(vines_debug_arr)

DEBUG_FLAG(vinesarp_debug,DEBUG_VINESARP,"Vines arp")
DEBUG_FLAG(banyan_bugs_debug,DEBUG_VINESBANYAN,"Vines banyan")
DEBUG_FLAG(vines_bcast_debug,DEBUG_VINES_BCAST,"Vines broadcasts")
DEBUG_FLAG(vinescache_debug,DEBUG_VINESCACHE,"Vines cache")
DEBUG_FLAG(vinesecho_debug,DEBUG_VINESECHO,"Vines echo")
DEBUG_FLAG(vinesipc_debug,DEBUG_VINESIPC,"Vines ipc")
DEBUG_FLAG(vinesnetrpc_debug,DEBUG_VINESNETRPC,"Vines netrpc")
DEBUG_FLAG(vinespkt_debug,DEBUG_VINESPKT,"Vines packet")
DEBUG_FLAG(vines_proxy_debug,DEBUG_VINESPROXY,"Vines proxy")
DEBUG_FLAG(vinesrtp_debug,DEBUG_VINESRTP,"Vines routing")
DEBUG_FLAG(vines_reinit_debug,DEBUG_VINES_REINIT,"Vines SRTP reinit")
DEBUG_FLAG(vines_state_debug,DEBUG_VINESSTATE,"Vines state")
DEBUG_FLAG(vfs_debug,DEBUG_VINES_FS,"Vines File Service")
DEBUG_FLAG(vnsm_debug,DEBUG_VINES_NSM,"Vines NSM service")
DEBUG_FLAG(vss_debug,DEBUG_VINES_SS,"Vines Server Service")
DEBUG_FLAG(vst_debug,DEBUG_VINES_ST,"Vines Streettalk service")
DEBUG_FLAG(vvs_debug,DEBUG_VINES_VS,"Vines VS service")
DEBUG_FLAG(vinestable_debug,DEBUG_VINESTABLE,"Vines table")
DEBUG_FLAG(vinestest_debug,DEBUG_VINESTEST,"Vines test")
DEBUG_FLAG(vines_unknown_debug,DEBUG_VINES_UNKNOWN,"Vines unknown packet")

DEBUG_ARRDONE
