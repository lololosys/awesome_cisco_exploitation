/* $Id: isis_nlsp_debug.h,v 3.2 1995/11/17 08:54:06 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/clns/isis_nlsp_debug.h,v $
 *------------------------------------------------------------------
 * Declarations for ISIS/NLSP debug variables
 *
 * November 1994, Dave Katz
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: isis_nlsp_debug.h,v $
 * Revision 3.2  1995/11/17  08:54:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:09:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/25  15:12:09  dkatz
 * CSCdi38296:  ISIS/NLSP SPF may miscalculate
 * Don't loop or truncate the chains on the TENT list.  Extend ISIS/NLSP
 * debugging to reduce chattiness.
 *
 * Revision 2.2  1995/08/23  18:03:18  dkatz
 * CSCdi37692:  LSP checksum errors can melt network
 * Add a knob to discard incoming LSPs with checksum errors, rather
 * than purging them.
 *
 * Add additional debugging capabilities.
 *
 * Revision 2.1  1995/06/07  20:18:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _ISIS_NLSP_DEBUG_H_
#define _ISIS_NLSP_DEBUG_H_

#include "../clns/isis_nlsp_proc_nums.h"

extern boolean isis_nlsp_adj_packet_debug[ISIS_NUM_PROC_TYPE];
extern boolean isis_nlsp_update_packet_debug[ISIS_NUM_PROC_TYPE];
extern boolean isis_nlsp_spf_event_debug[ISIS_NUM_PROC_TYPE];
extern boolean isis_nlsp_terse_spf_event_debug[ISIS_NUM_PROC_TYPE];
extern boolean isis_nlsp_spf_statistics_debug[ISIS_NUM_PROC_TYPE];
extern boolean isis_nlsp_spf_trigger_debug[ISIS_NUM_PROC_TYPE];
extern boolean isis_nlsp_snp_debug[ISIS_NUM_PROC_TYPE];
extern boolean isis_nlsp_local_update_debug[ISIS_NUM_PROC_TYPE];
extern boolean isis_nlsp_cksum_error_debug[ISIS_NUM_PROC_TYPE];
extern boolean isis_nlsp_proto_error_debug[ISIS_NUM_PROC_TYPE];

extern void isis_nlsp_debug_command(parseinfo *);

/* For convenience... */

#define isis_adj_packet_debug isis_nlsp_adj_packet_debug[PROC_TYPE_ISIS]
#define isis_update_packet_debug \
    isis_nlsp_update_packet_debug[PROC_TYPE_ISIS]
#define isis_spf_event_debug isis_nlsp_spf_event_debug[PROC_TYPE_ISIS]
#define isis_terse_spf_event_debug \
    isis_nlsp_terse_spf_event_debug[PROC_TYPE_ISIS]
#define isis_spf_statistics_debug \
    isis_nlsp_spf_statistics_debug[PROC_TYPE_ISIS]
#define isis_spf_trigger_debug \
    isis_nlsp_spf_trigger_debug[PROC_TYPE_ISIS]
#define isis_snp_debug \
    isis_nlsp_snp_debug[PROC_TYPE_ISIS]
#define isis_local_update_debug \
    isis_nlsp_local_update_debug[PROC_TYPE_ISIS]
#define isis_cksum_error_debug \
    isis_nlsp_cksum_error_debug[PROC_TYPE_ISIS]
#define isis_proto_error_debug \
    isis_nlsp_proto_error_debug[PROC_TYPE_ISIS]

#define novellnlsp_adj_packet_debug \
    isis_nlsp_adj_packet_debug[PROC_TYPE_NLSP]
#define novellnlsp_update_packet_debug \
    isis_nlsp_update_packet_debug[PROC_TYPE_NLSP]
#define novellnlsp_spf_event_debug \
    isis_nlsp_spf_event_debug[PROC_TYPE_NLSP]
#define novellnlsp_terse_spf_event_debug \
    isis_nlsp_terse_spf_event_debug[PROC_TYPE_NLSP]
#define novellnlsp_spf_statistics_debug \
    isis_nlsp_spf_statistics_debug[PROC_TYPE_NLSP]
#define novellnlsp_spf_trigger_debug \
    isis_nlsp_spf_trigger_debug[PROC_TYPE_NLSP]
#define novellnlsp_snp_debug \
    isis_nlsp_snp_debug[PROC_TYPE_NLSP]
#define novellnlsp_local_update_debug \
    isis_nlsp_local_update_debug[PROC_TYPE_NLSP]
#define novellnlsp_cksum_error_debug \
    isis_nlsp_cksum_error_debug[PROC_TYPE_NLSP]
#define novellnlsp_proto_error_debug \
    isis_nlsp_proto_error_debug[PROC_TYPE_NLSP]

#endif
