/* $Id: isis_nlsp_debug.c,v 3.2 1995/11/17 08:54:05 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/clns/isis_nlsp_debug.c,v $
 *------------------------------------------------------------------
 * Storage for debug flags for ISIS/NLSP
 *
 * November 1994, Dave Katz
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: isis_nlsp_debug.c,v $
 * Revision 3.2  1995/11/17  08:54:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:09:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/25  15:12:08  dkatz
 * CSCdi38296:  ISIS/NLSP SPF may miscalculate
 * Don't loop or truncate the chains on the TENT list.  Extend ISIS/NLSP
 * debugging to reduce chattiness.
 *
 * Revision 2.2  1995/08/23  18:03:17  dkatz
 * CSCdi37692:  LSP checksum errors can melt network
 * Add a knob to discard incoming LSPs with checksum errors, rather
 * than purging them.
 *
 * Add additional debugging capabilities.
 *
 * Revision 2.1  1995/06/07  20:18:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This file contains the basic declarations for the debugging flags for
 * IS-IS and NLSP.  This is in a separate file because it needs to be
 * linked into all images that contain either IPX or CLNS (which means
 * that it needs to be linked into something more central).
 *
 * Ugly, but it's only a few words, luckily.
 */

#include "master.h"
#include "interface.h"
#include "config.h"
#include "../ui/debug.h"
#include "isis_nlsp_debug.h"
#include "isis_nlsp_proc_nums.h"
#include "clns_debug.h"
#include "../xns/novell_debug.h"

boolean isis_nlsp_adj_packet_debug[ISIS_NUM_PROC_TYPE];
boolean isis_nlsp_update_packet_debug[ISIS_NUM_PROC_TYPE];
boolean isis_nlsp_spf_event_debug[ISIS_NUM_PROC_TYPE];
boolean isis_nlsp_terse_spf_event_debug[ISIS_NUM_PROC_TYPE];
boolean isis_nlsp_spf_statistics_debug[ISIS_NUM_PROC_TYPE];
boolean isis_nlsp_spf_trigger_debug[ISIS_NUM_PROC_TYPE];
boolean isis_nlsp_snp_debug[ISIS_NUM_PROC_TYPE];
boolean isis_nlsp_local_update_debug[ISIS_NUM_PROC_TYPE];
boolean isis_nlsp_cksum_error_debug[ISIS_NUM_PROC_TYPE];
boolean isis_nlsp_proto_error_debug[ISIS_NUM_PROC_TYPE];

/*
 * isis_nlsp_debug_command
 *
 * Trap certain debug commands to pick up extra parameters.
 */
void isis_nlsp_debug_command (parseinfo *csb)
{
    switch (csb->which) {
      case DEBUG_ISISSPFEVNT:
	if (csb->sense) {
	    isis_terse_spf_event_debug = GETOBJ(int,1);
	} else {
	    isis_terse_spf_event_debug = FALSE;
	}
	break;

      case DEBUG_NLSPSPFEVNT:
	if (csb->sense) {
	    novellnlsp_terse_spf_event_debug = GETOBJ(int,1);
	} else {
	    novellnlsp_terse_spf_event_debug = FALSE;
	}
	break;
    }
    debug_command(csb);
}
