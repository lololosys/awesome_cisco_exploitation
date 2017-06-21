/* $Id: channel_debug_flags.h,v 3.2.60.1 1996/05/29 18:44:55 motto Exp $
 * $Source: /release/112/cvs/Xsys/if/channel_debug_flags.h,v $
 *------------------------------------------------------------------
 * channel_debug_flags.h -- Debugging flag declarations for IBM channel
 *
 * April 1994, William H. Palmer
 *
 * Copyright (c) 1993-1996,1997 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: channel_debug_flags.h,v $
 * Revision 3.2.60.1  1996/05/29  18:44:55  motto
 * CSCdi46832:  vencap failed debug messages displayed for non cip
 * explorers
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:26:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/13  19:42:23  motto
 * Fixed all channel tcp/ip show commands to be consistent and work
 * properly.
 * CSCdi36252:  show ex ch <slot>/<port> tcp-connections needs parms to be
 * optional
 *
 * Revision 2.1  1995/06/07  20:51:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * We get included (multiple times) from channel code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(channel_debug_arr)

DEBUG_FLAG(channel_pkt_debug,      DEBUG_CHANNEL_PKTS,     "Channel packets")
DEBUG_FLAG(channel_debug,          DEBUG_CHANNEL,          "Channel events")
DEBUG_FLAG(channel_event_sna_debug,DEBUG_CHANNEL_EVENT_SNA,"Channel events sna")
DEBUG_FLAG(channel_debug_blocks,   DEBUG_CHANNEL_BLOCKS,   "Channel event data blocks")
DEBUG_FLAG(channel_debug_encaps,   DEBUG_CHANNEL_ENCAPS,   "Channel encaps")
DEBUG_FLAG(channel_sysmgt_debug,   DEBUG_CHANNEL_SYSMGT,   "Channel System Management")
DEBUG_FLAG(channel_debug_love,     DEBUG_CHANNEL_LOVE,     "Channel love letters")
DEBUG_FLAG(channel_debug_ilan_ack, DEBUG_CHANNEL_ILAN_ACK, "Internal LANs positive ack")
DEBUG_FLAG(channel_debug_ilan_nack,DEBUG_CHANNEL_ILAN_NACK,"Internal LANs negative ack")
DEBUG_FLAG(channel_debug_ilan_filt,DEBUG_CHANNEL_ILAN_FILT,"Internal LANs MAC filter")
DEBUG_FLAG(channel_debug_ilan,     DEBUG_CHANNEL_ILAN,     "Channel internal LANs")

DEBUG_ARRDONE
