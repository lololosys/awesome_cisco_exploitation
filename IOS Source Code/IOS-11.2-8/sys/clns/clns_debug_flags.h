/* $Id: clns_debug_flags.h,v 3.2 1995/11/17 08:51:57 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/clns/clns_debug_flags.h,v $
 *------------------------------------------------------------------
 * clns_debug_flags.h -- Debugging flag declarations for CLNS
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1995 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clns_debug_flags.h,v $
 * Revision 3.2  1995/11/17  08:51:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:07:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/23  18:02:50  dkatz
 * CSCdi37692:  LSP checksum errors can melt network
 * Add a knob to discard incoming LSPs with checksum errors, rather
 * than purging them.
 *
 * Add additional debugging capabilities.
 *
 * Revision 2.1  1995/06/07  20:17:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from CLNS code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(clns_debug_arr)

DEBUG_FLAG(clns_event_debug,DEBUG_CLNS_EVENT,"CLNS events")
DEBUG_FLAG(clns_igrp_debug,DEBUG_CLNS_IGRP,"CLNS IGRP packets")
DEBUG_FLAG(clns_packet_debug,DEBUG_CLNS_PACKET,"CLNS packets")
DEBUG_FLAG(clns_routing_debug,DEBUG_CLNS_ROUTING,"CLNS routing events")
DEBUG_FLAG(clns_filter_debug,DEBUG_CLNS_FILTER,"CLNS filters")
DEBUG_FLAG(esis_event_debug,DEBUG_ESIS_EVENT,"ES-IS events")
DEBUG_FLAG(esis_packet_debug,DEBUG_ESIS_PACKET,"ES-IS packets")
DEBUG_FLAG_NO_DECL(isis_adj_packet_debug,DEBUG_ISISADJPKT,
		   "IS-IS Adjacency related packets")
DEBUG_FLAG_NO_DECL(isis_spf_event_debug,DEBUG_ISISSPFEVNT,"IS-IS SPF events")
DEBUG_FLAG_NO_DECL(isis_update_packet_debug,DEBUG_ISISUPDATEPKT,
		   "IS-IS Update related packet")
DEBUG_FLAG_NO_DECL(isis_spf_statistics_debug,DEBUG_ISISSPFSTATS,
		   "IS-IS SPF Timing and Statistics Data")
DEBUG_FLAG_NO_DECL(isis_spf_trigger_debug,DEBUG_ISISSPFTRIGGERS,
		   "IS-IS SPF triggering events")
DEBUG_FLAG_NO_DECL(isis_snp_debug,DEBUG_ISISSNPPACKETS,
		   "IS-IS CSNP/PSNP packets")
DEBUG_FLAG_NO_DECL(isis_local_update_debug,DEBUG_ISISLOCUPDATE,
		   "IS-IS local updates")
DEBUG_FLAG_NO_DECL(isis_cksum_error_debug,DEBUG_ISISCKSUMERR,
		   "IS-IS checksum errors")
DEBUG_FLAG_NO_DECL(isis_proto_error_debug,DEBUG_ISISPROTOERR,
		   "IS-IS protocol errors")

DEBUG_ARRDONE
